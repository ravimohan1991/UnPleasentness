/* Support for executing system calls in the context of the game process. */

static const int injection_size = 4;

static const char nop_code_bytes[injection_size] = {
    /* This is the byte pattern used to pad function
       addresses to multiples of 16 bytes. It consists
       of RET and a sequence of NOPs. The NOPs are not
       supposed to be used, so they can be overwritten. */
    0xC3, 0x90, 0x90, 0x90
};

static QByteArray nop_code(nop_code_bytes, injection_size);

static const char injection_code_bytes[injection_size] = {
    /* This is the injected pattern. It keeps the
       original RET, but adds:
           INT 80h
           INT 3h (breakpoint) */
    0xC3, 0xCD, 0x80, 0xCC
};

static QByteArray injection_code(injection_code_bytes, injection_size);

VIRTADDR DFInstanceLinux::find_injection_address()
{
    if (m_inject_addr != unsigned(-1))
        return m_inject_addr;

    if (!m_executable)
        return m_inject_addr = 0;

    int step = 0x8000; // 32K steps
    VIRTADDR pos = m_executable->start_addr + step; // skip first

    // This loop is expected to succeed on the first try:
    for (; pos < m_executable->end_addr; pos += step)
    {
        int size = m_executable->end_addr - pos;
        if (step < size) size = step;
        QByteArray buf;
        read_raw(pos, size, buf);

        // Try searching for existing injection code
        int offset = buf.indexOf(injection_code);
        // Try searching for empty space
        if (offset < 0)
            offset = buf.indexOf(nop_code);

        if (offset >= 0) {
            m_inject_addr = pos + offset;
            LOGD << "injection point found at" << hex << m_inject_addr;
            return m_inject_addr;
        }
    }

    return m_inject_addr = 0;
}

#if __WORDSIZE == 64
#define R_ESP  rsp
#define R_EIP  rip
#define R_ORIG_EAX orig_rax
#define R_SCID rax
#define R_ARG0 rbx
#define R_ARG1 rcx
#define R_ARG2 rdx
#define R_ARG3 rsi
#define R_ARG4 rdi
#define R_ARG5 rbp
#else
#define R_ESP  esp
#define R_EIP  eip
#define R_ORIG_EAX orig_eax
#define R_SCID eax
#define R_ARG0 ebx
#define R_ARG1 ecx
#define R_ARG2 edx
#define R_ARG3 esi
#define R_ARG4 edi
#define R_ARG5 ebp
#endif

qint32 DFInstanceLinux::remote_syscall(int syscall_id,
     qint32 arg0, qint32 arg1, qint32 arg2,
     qint32 arg3, qint32 arg4, qint32 arg5)
{
    /* Find the injection place; on failure bail out */
    VIRTADDR inj_addr = find_injection_address();
    if (!inj_addr)
        return -1;

    /* Save the current value of the main thread registers */
    struct user_regs_struct saved_regs, work_regs;

    if (ptrace(PTRACE_GETREGS, m_pid, 0, &saved_regs) == -1) {
        perror("ptrace getregs");
        LOGE << "Could not retrieve register information";
        return -1;
    }

    /* Prepare the injected code */
    QByteArray inj_area_save;
    if (read_raw(inj_addr, injection_size, inj_area_save) < injection_size ||
        write_raw(inj_addr, injection_size, (void*)injection_code_bytes) < injection_size) {
        LOGE << "Could not prepare the injection area";
        return -1;
    }

    /* Prepare the registers */
    VIRTADDR jump_eip = inj_addr+1; // skip the first RET

    work_regs = saved_regs;
    work_regs.R_EIP = jump_eip;
    work_regs.R_ORIG_EAX = -1; // clear the interrupted syscall state
    work_regs.R_SCID = syscall_id;
    work_regs.R_ARG0 = arg0;
    work_regs.R_ARG1 = arg1;
    work_regs.R_ARG2 = arg2;
    work_regs.R_ARG3 = arg3;
    work_regs.R_ARG4 = arg4;
    work_regs.R_ARG5 = arg5;

    /* Upload the registers. Note that after this point,
       if this process crashes before the context is
       restored, the game will immediately crash too. */
    if (ptrace(PTRACE_SETREGS, m_pid, 0, &work_regs) == -1) {
        perror("ptrace setregs");
        LOGE << "Could not set register information";
        return -1;
    }

    /* Run the thread until the breakpoint is reached */
    int status;
    if (ptrace(PTRACE_CONT, m_pid, 0, SIGCONT) != -1) {
        status = wait_for_stopped();
        // If the process is stopped for some reason, restart it
        while (WSTOPSIG(status) == SIGSTOP &&
               ptrace(PTRACE_CONT, m_pid, 0, SIGCONT) != -1)
            status = wait_for_stopped();
        if (WSTOPSIG(status) != SIGTRAP) {
            LOGE << "Stopped on" << WSTOPSIG(status) << "instead of SIGTRAP";
        }
    } else {
        LOGE << "Failed to run the call.";
    }

    /* Retrieve registers with the syscall result. */
    if (ptrace(PTRACE_GETREGS, m_pid, 0, &work_regs) == -1) {
        perror("ptrace getregs");
        LOGE << "Could not retrieve register information after stepping";
    }

    /* Restore the registers. */
    if (ptrace(PTRACE_SETREGS, m_pid, 0, &saved_regs) == -1) {
        perror("ptrace setregs");
        LOGE << "Could not restore register information";
    }

    /* Defuse the pending signal state:
       If this process crashes before it officially detaches
       from the game, the last signal will be delivered to it.
       If the signal is SIGSTOP, nothing irreversible happens.
       SIGTRAP on the other hand will crash it, losing all data.
       To avoid it, send SIGSTOP to the thread, and resume it
       to immediately be stopped again.
    */
    syscall(SYS_tkill, m_pid, SIGSTOP);

    if (ptrace(PTRACE_CONT, m_pid, 0, 0) == -1 ||
        (status = wait_for_stopped(), WSTOPSIG(status)) != SIGSTOP) {
        LOGE << "Failed to restore thread and stop.";
    }

    /* Restore the modified injection area (not really necessary,
       since it is supposed to be inside unused padding, but...) */
    if (write_raw(inj_addr, injection_size, inj_area_save.data()) < injection_size) {
        LOGE << "Could not restore the injection area";
    }

    if (VIRTADDR(work_regs.R_EIP) == jump_eip+3)
        return work_regs.R_SCID;
    else {
        LOGE << "Single step failed: EIP" << hex << work_regs.R_EIP << "; expected" << jump_eip+3;
        return -4095;
    }
}

/* Memory allocation for strings using remote mmap. */

VIRTADDR DFInstanceLinux::mmap_area(VIRTADDR start, int size) {
    VIRTADDR return_value = remote_syscall(192/*mmap2*/, start, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Raw syscalls can't use errno, so the error is in the result.
    // No valid return value can be in the -4095..-1 range.
    if (int(return_value) < 0 && int(return_value) >= -4095) {
        LOGE << "Injected MMAP failed with error: " << -return_value;
        return_value = -1;
    }

    return return_value;
}

VIRTADDR DFInstanceLinux::alloc_chunk(int size) {
    if (size > 1048576 || size <= 0)
        return 0;

    if ((m_alloc_end - m_alloc_start) < size) {
        int apages = (size*2 + 4095)/4096;
        int asize = apages*4096;

        // Try to request contiguous allocation as a hint
        VIRTADDR new_block = mmap_area(m_alloc_end, asize);
        if (new_block == VIRTADDR(-1))
            return 0;

        if (new_block != m_alloc_end)
            m_alloc_start = new_block;
        m_alloc_end = new_block + asize;
    }

    VIRTADDR rv = m_alloc_start;
    m_alloc_start += size;
    return rv;
}

VIRTADDR DFInstanceLinux::get_string(const QString &str) {
    if (m_string_cache.contains(str))
        return m_string_cache[str];

    CP437Codec *codec = new CP437Codec;
    QByteArray data = codec->fromUnicode(str);

    STLStringHeader header;
    header.capacity = header.length = data.length();
    header.refcnt = 1000000; // huge refcnt to avoid dealloc

    QByteArray buf((char*)&header, sizeof(header));
    buf.append(data);
    buf.append(char(0));

    VIRTADDR addr = alloc_chunk(buf.length());
    if (addr) {
        write_raw(addr, buf.length(), buf.data());
        addr += sizeof(header);
    }

    return m_string_cache[str] = addr;
}
