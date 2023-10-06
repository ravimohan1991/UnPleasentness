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
#include <wx/apptrait.h>

KelvinFrame* UE1HookApp::m_Frame = nullptr;

wxIMPLEMENT_APP_CONSOLE(UE1HookApp);

bool UE1HookApp::OnInit()
{
	m_Frame = new KelvinFrame();
	m_Frame->Show(true);

	m_InjectorLoop = false;
	ActivateInjectorLoop(true);
	return true;
}

void UE1HookApp::ActivateInjectorLoop(bool on)
{
	if(on && !m_InjectorLoop)
	{
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(UE1HookApp::OnIdle));
		m_InjectorLoop = true;
	}
	else if(!on && m_InjectorLoop)
	{
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(UE1HookApp::OnIdle));
		m_InjectorLoop = false;
	}
}

void UE1HookApp::OnIdle(wxIdleEvent& event)
{
	if(m_InjectorLoop)
	{
		// ha looping when Idle. Must have a story.
		event.RequestMore(); // render continuously, not only once on idle
	}
}

KelvinFrame::KelvinFrame()
	: wxFrame(nullptr, wxID_ANY, "UnPleasentness Injector")
{
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Load Antigen...\tCtrl-H",
		"Select a DLL, SO, or DYLIB");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, "Quit UE1Hook", "Quit Hooking");

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

		m_ProcessInfoPanel = new InfoPanel(this, 0);
		m_PaneManager->AddPane(m_ProcessInfoPanel, wxAuiPaneInfo().
				Name(_("ProcessInfo")).
				Caption(_("Process Information")).
				TopDockable(true).
				CloseButton(false).
				BottomDockable(false).
				BestSize(wxSize(180, 35)).
				Show(true).
				Resizable(false).
				Center());

		m_LogPanel = new LogPanel(this, 1);
		m_PaneManager->AddPane(m_LogPanel, wxAuiPaneInfo().
				Name(_("OperationLog")).
				Caption(_("Operation Log")).
				TopDockable(false).
				CloseButton(false).
				BottomDockable(true).
				BestSize(wxSize(180, 1225)).
				Show(true).
				Resizable(false).
				Center());

		wxString tempStr;
		MyConfigBase::Get()->Read(_T("LastPerspective"), &tempStr, wxEmptyString);
		m_PaneManager->LoadPerspective(tempStr);
		m_PaneManager->Update();
	}

	//wxEventLoopBase *injectorLoop = wxAppTraits::CreateEventLoop();

	Bind(wxEVT_MENU, &KelvinFrame::OnOpenFile, this, wxID_OPEN);
	Bind(wxEVT_MENU, &KelvinFrame::OnAbout, this, wxID_ABOUT);

	Bind(wxEVT_MENU, &KelvinFrame::OnExit, this, wxID_EXIT);
}

void KelvinFrame::OnExit(wxCommandEvent& event)
{
	wxGetApp().ActivateInjectorLoop(false);
	event.Skip(); // don't stop event, we still want window to close

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

void KelvinFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
	std::cout<< "update ui";
}


UEHookPanel::UEHookPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
}

UEHookPanel::~UEHookPanel()
{
}

void InfoPanel::Set(wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD, wxString XORKey)
{

}

LogPanel::LogPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer(wxVERTICAL);

	m_LogTextControl = new wxTextCtrl(this, id, wxEmptyString, wxDefaultPosition, wxSize(11024, 11024), wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_READONLY);
	mainSizer->Add(m_LogTextControl, 1, wxALL, 2);

	this->SetSizer(mainSizer);
	this->Layout();

	m_LogTextControl->AppendText(wxString("hmm, And yes, if you do so before building the DLL and the application it helps. But this is only a workaround and hope you are able to provide a more professional solution.\n"));
	m_LogTextControl->AppendText(wxString("next?"));
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
