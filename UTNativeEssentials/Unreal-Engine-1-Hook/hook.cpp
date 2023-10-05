/*******************************************************************************************
 *    Unreal Engine Pathogen
 *    Windows: Adapted from https://github.com/mschadev/detours-example/tree/master
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 1, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************************/

#include "hook.h"
#include <wx/filedlg.h>
#include <wx/filesys.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/filehistory.h>

KelvinFrame* UE1HookApp::m_Frame = nullptr;

wxIMPLEMENT_APP_CONSOLE(UE1HookApp);

bool UE1HookApp::OnInit()
{
	m_Frame = new KelvinFrame();
	m_Frame->Show(true);

	return true;
}

KelvinFrame::KelvinFrame()
	: wxFrame(nullptr, wxID_ANY, "UnPleasentness Injector")
{
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Load Antigen...\tCtrl-H",
		"Select a DLL, SO, or DYLIB");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to UE1Hook!");

	{
		// Setup manager
		m_PaneManager = new wxAuiManager(this);
		m_PaneManager->SetManagedWindow(this);

		m_ProcessInfoPanel = new InfoPanel(this, -1);
		m_PaneManager->AddPane(m_ProcessInfoPanel, wxAuiPaneInfo().
				Name(_("ProcessInfo")).
				Caption(_("Process Information")).
				TopDockable(true).
				CloseButton(false).
				BottomDockable(false).
				BestSize(wxSize(180, 25)).
				Show(true).
				Resizable(false).
				Center().Layer(1));

		wxString tempStr;
		MyConfigBase::Get()->Read(_T("LastPerspective"), &tempStr, wxEmptyString);
		m_PaneManager->LoadPerspective(tempStr);
		m_PaneManager->Update();
	}


	Bind(wxEVT_MENU, &KelvinFrame::OnOpenFile, this, ID_Hello);
	Bind(wxEVT_MENU, &KelvinFrame::OnAbout, this, wxID_ABOUT);

	Bind(wxEVT_MENU, &KelvinFrame::OnExit, this, wxID_EXIT);
}

void KelvinFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void KelvinFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("With regards from The_Cowboy",
		"About UE1Hook", wxOK | wxICON_INFORMATION);
}

void KelvinFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}

void KelvinFrame::OnOpenFile(wxCommandEvent& event)
{
	wxFileDialog dialog(this, "Please choose DLL",
		wxEmptyString, wxEmptyString, "", wxFD_OPEN);

	if (dialog.ShowModal() == wxID_OK)
	{
		wxString filename(dialog.GetPath());

		wxFileSystem currentFileSystem;

		wxFSFile* currentFile = currentFileSystem.OpenFile(filename);

		if (filename.find(".dll") == 0)
		{
			wxLogMessage("Sorry, UE1Hook can't and won't work with unfamiliar \"antigens\".");
			return;
		}
		else
		{
			OpenFile(filename, true);
		}
	}
}

void KelvinFrame::OpenFile(wxString filename, bool openAtRight)
{
	
}

InfoPanelGui::InfoPanelGui(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer(wxVERTICAL);

	m_InfoPanelText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_InfoPanelText->Wrap(-1);
	mainSizer->Add(m_InfoPanelText, 0, wxALL, 2);


	this->SetSizer(mainSizer);
	this->Layout();
}

InfoPanelGui::~InfoPanelGui()
{
}

void InfoPanel::Set(wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD, wxString XORKey)
{
	static wxMutex mutexinfo;
	mutexinfo.Lock();

	struct stat* sbufptr = new struct stat;
	fstat(FD, sbufptr);

	wxString info_string;
	info_string = info_string + _("Name:") + wxT("\t") + flnm.GetFullName() + wxT("\n") +
		_("Path:") + wxT("\t") + flnm.GetPath() + wxT("\n") +
		_("Size:") + wxT("\t") + wxFileName::GetHumanReadableSize(wxULongLong(lenght)) + wxT("\n") +
		_("Access:") + wxT("\t") + AccessMode + wxT("\n") +
		_("Device:") + wxT("\t") +
#ifdef __WXMSW__
		(sbufptr->st_mode == 0 ? _("BLOCK") : _("FILE"))
#else
		(S_ISREG(sbufptr->st_mode) ? _("FILE") :
			S_ISDIR(sbufptr->st_mode) ? _("DIRECTORY") :
			S_ISCHR(sbufptr->st_mode) ? _("CHARACTER") :
			S_ISBLK(sbufptr->st_mode) ? _("BLOCK") :
			S_ISFIFO(sbufptr->st_mode) ? _("FIFO") :
			S_ISLNK(sbufptr->st_mode) ? _("LINK") :
			S_ISSOCK(sbufptr->st_mode) ? _("SOCKET") :
			wxT("?")
			)
#endif
		+ wxT("\n");
#ifdef __WXMSW__
	if (sbufptr->st_mode == 0)	//Enable block size detection code on windows targets,
#else
	if (S_ISBLK(sbufptr->st_mode))
#endif
	{
		info_string += _("Sector Size: ") + wxString::Format(wxT("%u\n"), 0);//FDtoBlockSize(FD)); <----------- Needs to be written
		info_string += _("Sector Count: ") + wxString::Format("%" wxLongLongFmtSpec "u\n", 0);//FDtoBlockCount(FD));
	}

	if (XORKey != wxEmptyString)
		info_string += wxString(_("XORKey:")) + wxT("\t") + XORKey + wxT("\n");

	m_InfoPanelText->SetLabel(info_string);

#ifdef _DEBUG_
	std::cout << flnm.GetPath().ToAscii() << ' ';
	if (S_ISREG(sbufptr->st_mode))
		printf("regular file");
	else if (S_ISDIR(sbufptr->st_mode))
		printf("directory");
	else if (S_ISCHR(sbufptr->st_mode))
		printf("character device");
	else if (S_ISBLK(sbufptr->st_mode)) {
		printf("block device");
	}
	else if (S_ISFIFO(sbufptr->st_mode))
		printf("FIFO");
#ifndef __WXMSW__
	else if (S_ISLNK(sbufptr->st_mode))
		printf("symbolic link");
	else if (S_ISSOCK(sbufptr->st_mode))
		printf("socket");
#endif
	printf("\n");
#endif
	//		S_IFMT 	0170000 	bitmask for the file type bitfields
	//		S_IFSOCK 	0140000 	socket
	//		S_IFLNK 	0120000 	symbolic link
	//		S_IFREG 	0100000 	regular file
	//		S_IFBLK 	0060000 	block device
	//		S_IFDIR 	0040000 	directory
	//		S_IFCHR 	0020000 	character device
	//		S_IFIFO 	0010000 	FIFO
	//		S_ISUID 	0004000 	set UID bit
	//		S_ISGID 	0002000 	set-group-ID bit (see below)
	//		S_ISVTX 	0001000 	sticky bit (see below)
	//		S_IRWXU 	00700 	mask for file owner permissions
	//		S_IRUSR 	00400 	owner has read permission
	//		S_IWUSR 	00200 	owner has write permission
	//		S_IXUSR 	00100 	owner has execute permission
	//		S_IRWXG 	00070 	mask for group permissions
	//		S_IRGRP 	00040 	group has read permission
	//		S_IWGRP 	00020 	group has write permission
	//		S_IXGRP 	00010 	group has execute permission
	//		S_IRWXO 	00007 	mask for permissions for others (not in group)
	//		S_IROTH 	00004 	others have read permission
	//		S_IWOTH 	00002 	others have write permission
	//		S_IXOTH 	00001 	others have execute permission
	delete sbufptr;
	mutexinfo.Unlock();
}

/*

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(),
	  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
	  &hToken))
	{
		printf("OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,          // lookup privilege on local system
	  lpszPrivilege,   // privilege to lookup
	  &luid))         // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (bEnablePrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken,  // 
	  FALSE,  // TURE
	  &tp,        // TOKEN_PRIBILEGES 
	  sizeof(TOKEN_PRIVILEGES),   //
	  (PTOKEN_PRIVILEGES)NULL,    //
	  (PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE                  hProcess, hThread;
	LPVOID                  pRemoteBuf;
	DWORD                   dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE  pThreadProc;


	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		log_add("OpenProcess(%d) failed", dwPID);
		return FALSE;
	}


	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize,
	  MEM_COMMIT, PAGE_READWRITE);


	WriteProcessMemory(hProcess, pRemoteBuf,
	  (LPVOID)szDllPath, dwBufSize, NULL);

	pThreadProc = (LPTHREAD_START_ROUTINE)
	  GetProcAddress(GetModuleHandle(L"kernel32.dll"),
	    "LoadLibraryW");

	hThread = CreateRemoteThread(hProcess, NULL, 0,
	  pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}


BOOL EjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	BOOL                    bMore = FALSE, bFound = FALSE;
	HANDLE                  hSnapshot, hProcess, hThread;
	MODULEENTRY32           me = { sizeof(me) };
	LPTHREAD_START_ROUTINE  pThreadProc;

	if (INVALID_HANDLE_VALUE ==
	  (hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID)))
	return FALSE;

	bMore = Module32First(hSnapshot, &me);
	for (; bMore; bMore = Module32Next(hSnapshot, &me))
	{
		if (!_tcsicmp(me.szModule, szDllPath) ||
		  !_tcsicmp(me.szExePath, szDllPath))
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		log_add("OpenProcess(%d) failed", dwPID);
		CloseHandle(hSnapshot);
		return FALSE;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)
	  GetProcAddress(GetModuleHandle(L"kernel32.dll"),
	    "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0,
	  pThreadProc, me.modBaseAddr, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	
	CloseHandle(hThread);
	CloseHandle(hProcess);

	CloseHandle(hSnapshot);

	return TRUE;
}*/

/*
int main()
{
	SetPrivilege(SE_DEBUG_NAME, TRUE);
	PROCESSENTRY32 entry;

	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, L"UnrealTournament.exe") == 0)
			{
				log_add("Process found, injecting custom code");
				InjectDll(entry.th32ProcessID, L"IDK.dll");
				log_add("Press Enter to unhook and exit");
				std::cin.get();
				EjectDll(entry.th32ProcessID, L"IDK.dll");
				break;
			}
		}
	}

	CloseHandle(snapshot);

	return 0;
}*/
