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
 *******************************************************************************************
 */

#include "hook.h"
#include <wx/filedlg.h>
#include <wx/filesys.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/filehistory.h>
#include <wx/apptrait.h>

#if defined HOOK_LINUX_PLATFORM || defined HOOK_MAC_PLATFORM
#define HOOK_UNIX_PLATFORM
#endif

// Global Define for this translation unit

#ifdef HOOK_MAC_PLATFORM
const char* AntigenFile = "dylib";
#elif HOOK_WINDOWS_PLATFORM
const char* AntigenFile = "dll";
#endif

#ifdef HOOK_MAC_PLATFORM
const char* ProcessFile = "unix process file";
#elif HOOK_WINDOWS_PLATFORM
const char* ProcessFile = "exe";
#endif

KelvinFrame* UE1HookApp::m_Frame = nullptr;

wxIMPLEMENT_APP_CONSOLE(UE1HookApp);

// Global application logging function
void AddLogText(const wxString log)
{
	wxGetApp().GetMyFrame()->LogMessage(log);
}

void InitHooking()
{
	bool bShouldLoop = HookAlpha();

	if (bShouldLoop)
	{
		AddLogText("UE1Hook is ready for, well, Hooking! Hook'em all!!");
		wxGetApp().SetStatus(HookStatus::Ready);
	}
	else
	{
		AddLogText("Encountered an injection obstruction");
	}
}

bool UE1HookApp::OnInit()
{
	m_Frame = new KelvinFrame();
	m_Frame->Show(true);

	m_InjectorLoop = false;
	
	m_FileName = m_ProcessName = wxString("");
	m_HookStatus = HookStatus::NotReady;

	InitHooking();
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
		if (m_ProcessName != "" && m_FileName != "")
		{
			HookingLoop(m_ProcessName.c_str(), m_FileName.c_str());
		}
		event.RequestMore(); // render continuously, not only once on idle
	}
}

void UE1HookApp::SetStatus(HookStatus status)
{ 
	m_HookStatus = status; 
	GetMyFrame()->GetProcessInfoPanel()->SetHookStatus(m_HookStatus);
}

KelvinFrame::KelvinFrame()
	: wxFrame(nullptr, wxID_ANY, "UnPleasentness Injector")
{
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Load Antigen...\tCtrl-H",
		"Select a DLL, SO, or DYLIB");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Process, "&Load Process...\tCtrl-P",
		"Select an executable process");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_RESET, "&Reset", "Reset the hooking state");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, "Quit UE1Hook", "Quit Hooking");

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	// For memorizing recently opened processes
	m_MenuProcessOpenRecent = new wxMenu();

	// For memorizing recently opened antigen
	m_AntigenOpenRecent = new wxMenu();


	menuBar->Append(m_MenuProcessOpenRecent, "&Recent Process");
	menuBar->Append(m_AntigenOpenRecent, "&Recent Antigen");

	// Setup histories
	m_ProcessHistory = new wxFileHistory();
	m_ProcessHistory->UseMenu(m_MenuProcessOpenRecent);
	m_ProcessHistory->AddFilesToMenu(m_MenuProcessOpenRecent);
	//m_MenuProcessOpenRecent->Remove(*m_MenuProcessOpenRecent->GetMenuItems().begin()); //Removes "no recent file" message
	m_ProcessHistory->Load(*MyConfigBase::Get());

	m_HisAntigen = new wxFileHistory();
	m_HisAntigen->UseMenu(m_AntigenOpenRecent);
	//m_AntigenOpenRecent->Remove(*m_AntigenOpenRecent->GetMenuItems().begin());
	m_HisAntigen->Load(*MyConfigBase::Get());

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to UE1Hook!");

	{
		// Setup manager
		m_PaneManager.reset(new wxAuiManager(this));
		m_PaneManager->SetManagedWindow(this);

		m_ProcessInfoPanel.reset(new InfoPanel(this, 0));
		m_PaneManager->AddPane(m_ProcessInfoPanel.get(), wxAuiPaneInfo().
				Name(_("ProcessInfo")).
				Caption(_("Process Information")).
				TopDockable(true).
				CloseButton(false).
				BottomDockable(false).
				BestSize(wxSize(180, 35)).
				Show(true).
				Resizable(false).
				Center());

		m_LogPanel.reset(new LogPanel(this, 1));
		m_PaneManager->AddPane(m_LogPanel.get(), wxAuiPaneInfo().
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
	//Bind(wxEVT_MENU, &KelvinFrame::OnHistoricOpenFIle, this, ID_MemProcess);
	Bind(wxEVT_MENU, &KelvinFrame::OnOpenProcess, this, ID_Process);
	Bind(wxEVT_MENU, &KelvinFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &KelvinFrame::OnReset, this, wxID_RESET);
	Bind(wxEVT_MENU, &KelvinFrame::OnExit, this, wxID_EXIT);
}

void KelvinFrame::OnReset(wxCommandEvent& event)
{
	switch (wxGetApp().GetStatus())
	{
		case HookStatus::Hooked:
		case HookStatus::Looping:
			wxGetApp().ActivateInjectorLoop(false);
			HookOmega("User Reset");
			InitHooking();
			break;

		case HookStatus(Ready | ProcessKnown):
			wxGetApp().SetStatus(Ready);
			wxGetApp().GetMyFrame()->GetProcessInfoPanel()->SetProcessName(wxT("No Process"));
			wxGetApp().GetMyFrame()->GetProcessInfoPanel()->PaintNow();
			HookOmega("User reset");
			InitHooking();
			break;
		case HookStatus::NotReady:
			break;
	}
}

void KelvinFrame::OnExit(wxCommandEvent& event)
{
	wxGetApp().ActivateInjectorLoop(false);
	event.Skip(); // don't stop event, we still want window to close

	m_ProcessHistory->Save(*MyConfigBase::Get());
	m_HisAntigen->Save(*MyConfigBase::Get());

	//delete (MyConfigBase::Get());

	//delete m_ProcessHistory;
	//delete m_HisAntigen;

	HookOmega("");
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

void KelvinFrame::OnHistoricAntigenLoad(wxCommandEvent& event)
{
	char buffer[50];

	wxSprintf(buffer, "%d", event.GetId());

	LogMessage(wxString("Attempt open from memory ") + wxString(buffer));

	wxString hisFile(m_ProcessHistory->GetHistoryFile(event.GetId() - wxID_FILE1));
	{
		if (!hisFile.empty())
		{
			OpenAntigenFile(hisFile);
		}
	}
}

void  KelvinFrame::OnHistoricOpenFile(wxCommandEvent& event)
{
	char buffer[50];

	wxSprintf(buffer, "%d", event.GetId());

	LogMessage(wxString("Attempt open from memory ") + wxString(buffer));

	wxString hisFile(m_ProcessHistory->GetHistoryFile(event.GetId() - wxID_FILE1));
	{
		if (!hisFile.empty())
		{
			OpenProcessFile(hisFile);
		}
	}
}

void KelvinFrame::OnOpenProcess(wxCommandEvent& event)
{
	static wxString title = wxString("Please choose ") + wxString(ProcessFile).MakeUpper();

	wxFileDialog dialog(this, title, wxEmptyString, wxEmptyString, "", wxFD_OPEN);

	if (dialog.ShowModal() == wxID_OK)
	{
		wxString filename(dialog.GetFilename());

		wxFileSystem currentFileSystem;

		wxFSFile* currentFile = currentFileSystem.OpenFile(filename);

#ifdef HOOK_WINDOWS_PLATFORM
		if (wxGetApp().GetStatus() == HookStatus::Ready && filename.find(ProcessFile) == wxString::npos)
#elif defined HOOK_UNIX_PLATFORM
		if (wxGetApp().GetStatus() == HookStatus::Ready && filename.find(".") != wxString::npos)
#endif
		{
			LogMessage("Sorry, UE1Hook can't and won't work with unfamiliar \"processes\".");

#ifdef HOOK_WINDOWS_PLATFORM
		static wxString correctionMessage = wxString("Please look for .") + wxString(ProcessFile);
#elif defined HOOK_UNIX_PLATFORM
		static wxString correctionMessage = wxString("Please look for ") + wxString(ProcessFile);
#endif

			LogMessage(correctionMessage);
		}
		else
		{
			HookStatus status = wxGetApp().GetStatus();
			switch (wxGetApp().GetStatus())
			{
			case HookStatus::Ready:
				OpenProcessFile(filename);
				if (wxGetApp().GetStatus() == HookStatus::BothProcessAntigen)
				{
					wxGetApp().ActivateInjectorLoop(true);
					wxGetApp().SetStatus(HookStatus::Looping);
				}
				else
				{
					AddLogText("Antigen file name required");
				}
				break;

			case HookStatus::AntigenLoaded | HookStatus::Ready:
				OpenProcessFile(filename);
				if (wxGetApp().GetStatus() == HookStatus(BothProcessAntigen | Ready))
				{
					wxGetApp().ActivateInjectorLoop(true);
					wxGetApp().SetStatus(HookStatus::Looping);
				}
				break;

			case HookStatus::NotReady:
				AddLogText(wxString("UE1Hook not ready for injection"));
				break;

			case HookStatus::Hooked:
				AddLogText(wxString("UE1Hook already hooked a file (containing antigen)"));
				AddLogText("Try after File->Reset");
				break;

			case HookStatus::Looping:
				AddLogText(wxString("In middle of hooking a file (containing antigen)"));
				AddLogText("Try after File->Reset");
				break;

			default:
				break;
			}

		}
	}

}

// File with Antigen
void KelvinFrame::OnOpenFile(wxCommandEvent& event)
{
	static wxString title = wxString("Please choose ") + wxString(AntigenFile).MakeUpper();

	wxFileDialog dialog(this, title, wxEmptyString, wxEmptyString, "", wxFD_OPEN);

	if (dialog.ShowModal() == wxID_OK)
	{
		wxString filename(dialog.GetPath());

		wxFileSystem currentFileSystem;

		wxFSFile* currentFile = currentFileSystem.OpenFile(filename);

		if (wxGetApp().GetStatus() == HookStatus::Ready && filename.find(AntigenFile) == wxString::npos)
		{
			LogMessage("Sorry, UE1Hook can't and won't work with unfamiliar \"antigens\".");

			static wxString correctionMessage = wxString("Please look for .") + wxString(AntigenFile);

			LogMessage(correctionMessage);
		}
		else
		{
			switch (wxGetApp().GetStatus())
			{
				case HookStatus::Ready:
					OpenAntigenFile(filename);
					if (wxGetApp().GetStatus() == HookStatus::BothProcessAntigen)
					{
						wxGetApp().ActivateInjectorLoop(true);
						wxGetApp().SetStatus(HookStatus::Looping);
					}
					else
					{
						AddLogText("Target process name required");
					}
					break;

				case HookStatus::ProcessKnown | HookStatus::Ready:
					OpenAntigenFile(filename);
					if (wxGetApp().GetStatus() == HookStatus(BothProcessAntigen | Ready))
					{
						wxGetApp().ActivateInjectorLoop(true);
						wxGetApp().SetStatus(HookStatus::Looping);
					}
					break;

				case HookStatus::NotReady:
					AddLogText(wxString("UE1Hook not ready for injection"));
					break;

				case HookStatus::Hooked:
					AddLogText(wxString("UE1Hook already hooked a file (containing antigen)"));
					AddLogText("Try after File->Reset");
					break;

				case HookStatus::Looping:
					AddLogText(wxString("In middle of hooking a file (containing antigen)"));
					AddLogText("Try after File->Reset");
					break;

				default:
					break;
			}

		}
	}
}

void UE1HookApp::SetProcessName(const wxString processName)
{
	m_ProcessName = processName;
	GetMyFrame()->GetProcessInfoPanel()->SetProcessName(m_ProcessName);

	GetMyFrame()->GetProcessInfoPanel()->PaintNow();
}

void KelvinFrame::OpenProcessFile(wxString filename)
{
	wxGetApp().OrStatus(HookStatus::ProcessKnown);

	int found = -1;

	//For loop updates Open Recent Menu properly.
	for(unsigned counter = 0; counter < m_ProcessHistory->GetCount(); counter++)
	{
		if(m_ProcessHistory->GetHistoryFile(counter) == filename)
		{
			found = counter;
		}
	}

	if(found != -1)
	{
		m_ProcessHistory->RemoveFileFromHistory(found);
	}
	m_ProcessHistory->AddFileToHistory(filename);
	m_ProcessHistory->Save(*(MyConfigBase::Get()));
	MyConfigBase::Get()->Flush();

	wxGetApp().SetProcessName(filename);
	LogMessage("Targetting the process " + filename);
}

void KelvinFrame::OpenAntigenFile(wxString filename)
{
	int found = -1;

	//For loop updates Open Recent Menu properly.
	for(unsigned counter = 0; counter < m_HisAntigen->GetCount(); counter++)
	{
		if(m_HisAntigen->GetHistoryFile(counter) == filename)
		{
			found = counter;
		}
	}

	if(found != -1)
	{
		m_ProcessHistory->RemoveFileFromHistory(found);
	}
	m_HisAntigen->AddFileToHistory(filename);
	m_HisAntigen->Save(*(MyConfigBase::Get()));
	MyConfigBase::Get()->Flush();

	wxGetApp().SetFileName(filename);

	wxString tempoString(filename);
	LogMessage(wxString("Antigen code file ") + tempoString + wxString(" loaded"));

	wxGetApp().OrStatus(HookStatus::AntigenLoaded);
}

void KelvinFrame::LogMessage(const wxString& logMessage)
{
	m_LogPanel->GetLogTextControl()->AppendText(logMessage);
	m_LogPanel->GetLogTextControl()->AppendText(wxString("\n"));
}

/*
void KelvinFrame::OnUpdateUI(wxEvent& event)
{
	m_LogPanel->GetLogTextControl()->AppendText(wxString("Update UI\n"));
}*/

InfoPanel::InfoPanel(wxWindow* parent, int id, wxPoint pos, wxSize size, int style) : wxPanel(parent, id, pos, size, style)
{
	m_ProcessName = "No Process";
}

void InfoPanel::PaintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	Render(dc);
}

void InfoPanel::PaintNow()
{
	wxClientDC dc(this);
	Render(dc);
}

void InfoPanel::Render(wxDC& dc)
{
	// draw some text
	if (m_ProcessName == "No Process")
	{
		dc.Clear();
		dc.DrawText(m_ProcessName, 10, 5);
	}
	else if(m_HookStatus == HookStatus(Ready | ProcessKnown) && m_HookStatus != HookStatus(Ready | BothProcessAntigen))
	{
		dc.Clear();

		wxColour processColor(150, 75, 0);
		dc.SetTextForeground(processColor);

		dc.DrawText(m_ProcessName, 10, 5);

		// Maybe indicate about process active or not. May become complicated

		wxColour statusColor(200, 10, 10);
		dc.SetTextForeground(statusColor);

		dc.DrawText(wxT("UnHooked"), dc.GetTextExtent(m_ProcessName).x + 40, 5);
	}
}

void InfoPanel::Set()
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
}

/////////////////////////////////////// Mac Hooking ///////////////////////////////////////
#ifdef HOOK_MAC_PLATFORM

bool HookAlpha()
{
	// no hooking for now, bypass
	return true;
}

void HookOmega(const wxString haltMessage)
{
	if(haltMessage != "")
	{
		AddLogText(haltMessage);
	}
}

void HookingLoop(const char* processName, const char* dllPath)
{
	// nothing to do
}

#endif // HOOK_MAC_PLATFORM

/////////////////////////////////////// Windows Hooking ///////////////////////////////////////

#ifdef HOOK_WINDOWS_PLATFORM

#include <TlHelp32.h>
#include <tchar.h>

// Courtsey: https://stackoverflow.com/a/8032108
const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

/*
 *  FUNCTION: log_add(const char *fmt, ...)
 *
 *  PURPOSE: log information to a visible medium, with timestamps.
 *
 *  COMMENTS: we are feeding the buffer to UE1Hook's logging system
 */
void log_add(const char* fmt, ...)
{
	va_list va_alist;
	static char logbuf[1024];
	if (fmt == NULL)
	{
		return;
	}

	//FILE* fp;
	struct tm* current_tm;
	time_t current_time;
	time(&current_time);
	current_tm = localtime(&current_time);

	sprintf(logbuf, "%02d:%02d:%02d - ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);
	va_start(va_alist, fmt);
	_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
	va_end(va_alist);

	AddLogText(wxString(logbuf));
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(),
	  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
	  &hToken))
	{
		log_add("OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,          // lookup privilege on local system
	  lpszPrivilege,   // privilege to lookup
	  &luid))         // receives LUID of privilege
	{
		log_add("LookupPrivilegeValue error: %u\n", GetLastError());
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
		log_add("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		log_add("The token does not have the specified privilege.");
		log_add("Try running the application with administrator rights.");
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

	log_add("Applying hooking procedure on process %x with PID %i", hProcess, dwPID);

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize,
	  MEM_COMMIT, PAGE_READWRITE);

	log_add("Writing to process memory");
	WriteProcessMemory(hProcess, pRemoteBuf,
	  (LPVOID)szDllPath, dwBufSize, NULL);

	log_add("Attempting to creat new thread");
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
}

PROCESSENTRY32 entry;
HANDLE snapshot;

const char* dllFullPath = nullptr;

bool HookAlpha()
{
	if (SetPrivilege(SE_DEBUG_NAME, TRUE) == false)
	{
		return false;
	}

	entry.dwSize = sizeof(PROCESSENTRY32);

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void HookOmega(const wxString haltMessage)
{
	if(haltMessage == "")
	{
		log_add("Unhooking, goodbye");
	}
	else
	{
		log_add(haltMessage.c_str());
	}

	if (dllFullPath != nullptr)
	{
		EjectDll(entry.th32ProcessID, GetWC(dllFullPath));
	}
	wxGetApp().SetStatus(HookStatus::NotReady);

	// reset variables
	snapshot = nullptr;
	dllFullPath = nullptr;

	CloseHandle(snapshot);
}

void HookingLoop(const char* processName, const char* dllPath)
{
	wxGetApp().SetStatus(HookStatus::Looping);

	if (Process32Next(snapshot, &entry) == TRUE)
	{
		if (wcscmp(entry.szExeFile, GetWC(processName)) == 0)
		{
			log_add("Process found, injecting custom code");
			InjectDll(entry.th32ProcessID, GetWC(dllPath));
			wxGetApp().ActivateInjectorLoop(false);
			wxGetApp().SetStatus(HookStatus::Hooked);
			dllFullPath = dllPath;// cache for later use
		}
	}
}
#endif // HOOK_WINDOWS_PLATFORM
