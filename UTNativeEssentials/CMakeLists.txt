#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

cmake_minimum_required(VERSION 3.13)

project(UTNativeEssentials)
set(OLDUNREAL_VERSION "469c")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PROJECT_VERSION_MAJOR "0")
set(CMAKE_PROJECT_VERSION_MINOR "1")
set(CMAKE_PROJECT_VERSION_PATCH "0")

include(CheckCCompilerFlag)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


# Control where the static and shared libraries are built
# We would want a semi-packaging fashioned way where all bianries are together
# whilst accessible from IDE (local debugger for instance!).
set(REPOSITORYROOT "${CMAKE_CURRENT_SOURCE_DIR}/../")

# UT GOTY directory
set(UTGOTYROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../")


################################################################################
# Figure out build type and target platform
################################################################################
# we need add_link_options
# Sanity checks
if(WIN32 AND NOT WINDOWS)
  set(WINDOWS TRUE)
  set(PLATFORM_PREFIX "Win")
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LINUX TRUE)
  set(PLATFORM_PREFIX "Linux")
endif()
if(APPLE)
  set(MACOS TRUE)
  set(PLATFORM_PREFIX "MacOS")
endif()

# See what CPU we appear to be targeting...
macro(check_cpu _CDEFS _NAME _VALUE)
  check_c_source_compiles(
    "
        #if ( ${_CDEFS} )
        int main(int argc, char **argv) { int yup = 1; return 0; }
        #else
        #error Not targeting this CPU architecture.
        #endif
        "
    ${_VALUE}
  )

  if(${_VALUE})
    if(OLDUNREAL_CHOSE_CPU)
      message(STATUS "We appear to see two different CPU architectures!")
      message(STATUS "We saw '${OLDUNREAL_CPU}' and '${_NAME}'.")
      message(FATAL_ERROR "Please fix this before continuing.")
    endif()
    set(OLDUNREAL_CHOSE_CPU TRUE)
    set(OLDUNREAL_CPU ${_NAME})
  endif()
endmacro(check_cpu)

check_cpu(
  "defined(__i386__) || defined(__i686__) || defined(_M_IX86) || defined(i386)"
  "x86" OLDUNREAL_X86
)
check_cpu("defined(__x86_64__) || defined(_M_X64)" "amd64" OLDUNREAL_AMD64)
check_cpu("defined(__arm__)" "arm" OLDUNREAL_ARM)
check_cpu("defined(__arm64__)" "arm64" OLDUNREAL_ARM64)

if (NOT OLDUNREAL_CHOSE_CPU)
  message(FATAL_ERROR "We don't support this architecture yet")
endif()

message(STATUS "*****************************************************************************************")
message(STATUS "* Native Build Environment: ${PLATFORM_PREFIX} platform and ${OLDUNREAL_CPU} cpu detected")
message(STATUS "* Setting files for native mod build for Unreal Torunament ${OLDUNREAL_VERSION}")
message(STATUS "*****************************************************************************************")

################################################################################
# Build options
################################################################################

#add_compile_definitions(WINDOW_API __declspec(dllexport))

################################################################################
# Set the output directory
################################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${REPOSITORYROOT}/UnpleasentnessBin/${PLATFORM_PREFIX}/${OLDUNREAL_CPU}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${REPOSITORYROOT}/UnpleasentnessBin/${PLATFORM_PREFIX}/${OLDUNREAL_CPU}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${REPOSITORYROOT}/UnpleasentnessBin/${PLATFORM_PREFIX}/${OLDUNREAL_CPU}/${CMAKE_BUILD_TYPE}")

################################################################################
# Set compiler and linker flags
################################################################################

add_compile_definitions(UNICODE=1)
add_compile_definitions(_UNICODE=1)


if(WINDOWS)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
  add_compile_definitions(_CRT_NO_VA_START_VALIDATION)

  add_compile_definitions(WIN32=1)
endif()

if(LINUX)
  add_compile_definitions(__LINUX_ARM__)
endif()

if(MACOS)
  add_compile_definitions(HAVE_PRAGMA_PACK)
  add_compile_definitions(MACOSX)
  add_compile_definitions(__APPLE_ARM64__)
  set(CMAKE_OSX_ARCHITECTURES "${ARCHS_STANDARD}")
endif()

if(OLDUNREAL_AMD64)
   add_compile_definitions(WIN64=1)
elseif(WINDOWS)
    #
    # /Zc:wchar_t-
    # do not treat wchar_t as a built-in type. Doing so would break binary
    # compat with almost all native mods for earlier UT versions
    #
    # /arch:SSE2
    # assume we have at least a Pentium 4
    #
    # /LARGEADDRESSAWARE
    # we can handle more than 2Gb of RAM even in 32bit builds
    #
    # /Zp4
    # use 4-byte alignment for structs. This is necessary for binary compatibility
    #
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t- /arch:SSE2 /Zp4 -DWINDOWS_IGNORE_PACKING_MISMATCH")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zc:wchar_t- /arch:SSE2 /Zp4 -DWINDOWS_IGNORE_PACKING_MISMATCH")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /LARGEADDRESSAWARE")
endif()

# the recursive garbage collector needs a lot of stack space
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} /STACK:16000000,1000000"
)

if(CMAKE_COMPILER_IS_GNUCC OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fsigned-char -fno-strict-aliasing -Wno-inline-new-delete")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fsigned-char -fno-strict-aliasing")
endif()

if(OLDUNREAL_USE_ASAN)
  add_compile_options("-fsanitize=address")
  link_libraries("-fsanitize=address")
endif()

#add_compile_definitions(MOD_BUILD)

################################################################################
# Including External and (or) non Epic libraries
################################################################################

################################################################################
# Epic Essentials
################################################################################
add_subdirectory(Core)
add_subdirectory(Engine)

################################################################################
# Link Directories
################################################################################
#link_directories(IDK MiscLibs)
link_directories(UEHook Core/Lib Engine/Lib Window/Lib)

################################################################################
# Target Definitions / Custom Modding Stuff
################################################################################
add_subdirectory(MiscLibs/Detours)

add_subdirectory(IDK105)
add_subdirectory(Unreal-Engine-1-Hook)

target_include_directories(IDK
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Inc)
	
target_include_directories(UEHook
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Inc)	

################################################################################
# Target Linking Rules
################################################################################
target_link_libraries(IDK PUBLIC Core Engine Detours)
target_link_libraries(UEHook PUBLIC Core Engine)

# end of CMakeLists.txt ...
