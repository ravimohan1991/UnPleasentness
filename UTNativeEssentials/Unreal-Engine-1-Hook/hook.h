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

#pragma once

#include <wx/wx.h>
#include <wx/fileconf.h>

// Forward declarations

// Classes
class wxAuiManager;
class InfoPanel;
class LogPanel;
class InjectorLoop;
class wxCollapsiblePane;
class wxSpinCtrl;
class UDKHexEditor;
class wxFileHistory;
class wxFileName;
class KelvinFrame;

// Global functions
bool HookAlpha();
void HookOmega(const wxString haltText);

void HookingLoop(const char*, const char*);

/**
 * @brief Enum for UE1Hook status
 */
enum HookStatus
{
	NotReady						= 0x00000000,						///< Some obstruction encountered, so not ready
	AntigenLoaded					= 0x00000100,						///< Loaded antigen
	ProcessKnown					= 0x00000010,						///< Target process made known
	Ready							= 0x00000002,						///< Ready for hooking a process
	Looping							= 0x00000004,						///< Looping in search of process, usually after loading shared object code file (DLL)
	Hooked							= 0x00000006,						///< Hooked the custom native code into the process
	BothProcessAntigen				= ProcessKnown | AntigenLoaded		//< Both process and antigen are cached
};

/**
 * @brief For tracking OpenProcess menu entry
 */
enum
{
	ID_Process = 1,				///< Process open menu entry
	ID_MemProcess,				///< For remembering the previously opened processes
	///<
	ID_MemAntigen				///< For remembering the previoulsy opened antigen
};

/**
 * @brief The app wxWidgets class for UE1Hook
 */
class UE1HookApp : public wxApp
{
public:
	/**
	 * @brief Initialize stuff
	 *
	 * Purpose is two-fold
	 * 1. Create a frame (from KelvinFrame) for UI display
	 * 2. Activate a loop for tracking platform processe(s)
	 *
	 * @return true after doing all work
	 * @see UE1HookApp:: ActivateInjectorLoop
	 */
	bool OnInit() override;

	/**
	 * @brief Getter for sole KelvinFrame instance
	 */
	KelvinFrame* GetMyFrame() const { return m_Frame; }

	/**
	 * @brief For realtime-like (I am asuming) looping logic
	 *
	 * @see UE1HookApp:: ActivateInjectorLoop
	 */
	void OnIdle(wxIdleEvent& evt);

	/**
	 * @brief Routine to enable and disable realtime-like loop
	 *
	 * @remark Visit https://wiki.wxwidgets.org/Making_a_render_loop
	 */
	void ActivateInjectorLoop(bool on);

	/**
	 * @brief For querying the status of the hook
	 */
	HookStatus GetStatus() const { return m_HookStatus; }

	/**
	 * @brief For setting the hook status
	 */
	void SetStatus(HookStatus status);

	/**
	 * @brief For setting multiple status flags
	 * 
	 * So that we may know if both process and antigen are loaded
	 */
	inline void OrStatus(HookStatus status) { SetStatus(HookStatus(m_HookStatus | status)); }

	inline void SetFileName(const wxString fileName) { m_FileName = fileName; }
	void SetProcessName(const wxString processName);

	inline const wxString& GetFileName() const { return m_FileName; }
	inline const wxString& GetProcessName() const { return m_ProcessName; }

private:

	/**
	 * Reference to the sole instance of KelvinFrame
	 */
	static KelvinFrame* m_Frame;

	/**
	 * Variable showing the status of real-time like loop
	 */
	bool m_InjectorLoop;

	/**
	 * @brief The name of the process to be hooked
	 */
	wxString m_ProcessName;

	/**
	 * @brief The full path of the DLL, SO, or DYLIB with injection code
	 */
	wxString m_FileName;

	/**
	 * @brief The current status of the UE1Hook application
	 */
	HookStatus m_HookStatus;
};

/**
 * @brief wxFrame for UE1HookApp
 */
class KelvinFrame : public wxFrame
{
public:
	/**
	 * @brief Constructor for setting up menus, menubar, statusbar, and
	 * variety of frames for UE1Hook application.
	 */
	KelvinFrame();

private:
	/**
	 * @brief For legacy purpose from tutorial
	 */
	void OnHello(wxCommandEvent& event);

	/**
	 * @brief Exit entry callback
	 *
	 * Close the window after deactivating real-time like loop
	 */
	void OnExit(wxCommandEvent& event);

	/**
	 * @brief About entry callback
	 *
	 * Shows information aboout the hook application and my name :D
	 *
	 * @todo Think what information should be displayed
	 */
	void OnAbout(wxCommandEvent& event);

	/**
	 * @brief OpenFile entry callback
	 *
	 * Opens up OpenFile dialog window
	 */
	void OnOpenFile(wxCommandEvent& event);

	void OnHistoricOpenFile(wxCommandEvent& event);
	void OnHistoricAntigenLoad(wxCommandEvent& event);

	/**
	 * @brief Open executable process callback
	 */
	void OnOpenProcess(wxCommandEvent& event);

	/**
	 * @brief Reset the state of UE1Hook
	 */
	void OnReset(wxCommandEvent& event);

public:
	/**
	 * @brief Select and cache the antigen
	 *
	 * @param filename		The absolute path of file (DLL or SO or DYLIB)
	 * 
	 * @todo write injection process for platforms
	 */
	void OpenAntigenFile(wxString filename);

	/**
	 * @brief Select and cache the target process
	 */
	void OpenProcessFile(wxString filename);

	/**
	 * @brief Getter for Log panel
	 */
	std::shared_ptr<LogPanel> GetLogPanel() const { return m_LogPanel; }

	/**
	 * @brief Logs a message in the application logging area followed by new line;
	 *
	 * @param logMessage		The actual message to be logged
	 */
	void LogMessage(const wxString& logMessage);

	/**
	 * @brief getter for m_ProcessInfoPanel
	 */
	std::shared_ptr<InfoPanel> GetProcessInfoPanel() const { return m_ProcessInfoPanel; }

	/**
	 * @brief Getter for m_ProcessHistory
	 */
	wxFileHistory* GetFileHistory() const { return m_ProcessHistory; }
private:
	/**
	 * @brief For managing varitey of panes or panels
	 *
	 * Chiefly used for adding, handling resizing, docking, and more \n
	 * of utility panes (to be) present in UDKHalo.\n
	 *
	 * wxAuiManager works as follows: the programmer adds panes to the \n
	 * class, or makes changes to existing pane properties (dock \n
	 * position, floating state, show state, etc.). To apply these \n
	 * changes, wxAuiManager's Update() function is called. This batch \n
	 * processing can be used to avoid flicker, by modifying more than \n
	 * one pane at a time, and then "committing" all of the changes at \n
	 * once by calling Update().
	 */
	std::shared_ptr<wxAuiManager> m_PaneManager;

	/**
	 * @brief Pane for process information
	 *
	 * All the useful information about target process
	 *
	 * @see InfoPanel
	 */
	std::shared_ptr<InfoPanel> m_ProcessInfoPanel;

	/**
	 * @brief Pane for logging operations
	 *
	 * Shows the logs of operations being performed for antigen injection
	 * of the custom shared object code (in most of your language(s), DLL)
	 */
	std::shared_ptr<LogPanel> m_LogPanel;

	/**
	 * @brief Caching the process history
	 */
	wxFileHistory* m_ProcessHistory;

	/**
	 * @brief For display of recently used items (processes)
	 */
	wxMenu* m_MenuProcessOpenRecent;

	/**
	 * @brief Caching the antigen history
	 */
	wxFileHistory* m_HisAntigen;

	/**
	 * @brief For display of recently used items (dlls)
	 */
	wxMenu* m_AntigenOpenRecent;

	wxFileConfig* AppConfigFile;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(KelvinFrame, wxFrame)
	EVT_MENU(ID_MemProcess, KelvinFrame::OnHistoricOpenFile)
	EVT_MENU(ID_MemAntigen, KelvinFrame::OnHistoricAntigenLoad)
END_EVENT_TABLE()

/// <summary>
/// Class for panel with information of the target process
/// </summary>
class InfoPanel : public wxPanel
{
public:
	InfoPanel(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize(-1, -1), int style = wxTAB_TRAVERSAL);

	void Set();
	void OnUpdate(wxCommandEvent& event) {}

	void SetProcessName(const wxString& pName) { m_ProcessName = pName; }
	void SetPID(const wxString pID) { m_PID = pID; }
	void SetHookStatus(HookStatus status) { m_HookStatus = status; }
	void SetProcessIcon(const wxIcon& icon) { m_ProcessIcon = icon; }
	const wxString& GetProcessName() const { return m_ProcessName; }
	const wxString& GetPID() const { return m_PID; }
	const HookStatus& GetHookStatus() const { return m_HookStatus; }
	const wxIcon& GetProcessIcon() const { return m_ProcessIcon; }

	// Rendering part
	// Courtsey: https://wiki.wxwidgets.org/Drawing_on_a_panel_with_a_DC

	/**
	 * @brief Called by the system of by wxWidgets when the panel needs
	 * to be redrawn. You can also trigger this call by
	 * calling Refresh()/Update().
	 */
	void PaintEvent(wxPaintEvent& evt);

	/**
	 * Alternatively, you can use a clientDC to paint on the panel
	 * at any time. Using this generally does not free you from
	 * catching paint events, since it is possible that e.g. the window
	 * manager throws away your drawing when the window comes to the
	 * background, and expects you will redraw it when the window comes
	 * back (by sending a paint event).
	 *
	 * In most cases, this will not be needed at all; simply handling
	 * paint events and calling Refresh() when a refresh is needed
	 * will do the job.
	 */
	void PaintNow();

	/**
	 * Here we do the actual rendering. I put it in a separate
	 * method so that it can work no matter what type of DC
	 * (e.g. wxPaintDC or wxClientDC) is used.
	 */
	void Render(wxDC& dc);

private:
	wxString m_ProcessName;
	wxString m_PID;
	wxIcon m_ProcessIcon;
	HookStatus m_HookStatus;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(InfoPanel, wxPanel)

// catch paint events
EVT_PAINT(InfoPanel::PaintEvent)

END_EVENT_TABLE()

/// <summary>
/// Class for panel with logging
/// </summary>
class LogPanel : public wxPanel
{
public:
	LogPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(273, 310), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);

	void Set(wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD, wxString XORKey);
	void OnUpdate(wxCommandEvent& event) {}

	wxTextCtrl* GetLogTextControl() const { return m_LogTextControl; }

protected:
	/**
	 * @brief A wxWidget text control for logging
	 *
	 * For read only log text to be displayed
	 *
	 * @see
	 */
	wxTextCtrl* m_LogTextControl;
};

/// <summary>
/// Wrapper for Portable vs Registry configbase.\n
/// if there are wxHexEditor.cfg file exits on current path, wxHexEditor switches to portable version.
/// </summary>
class MyConfigBase
{
public:
	static wxConfigBase* Get()
	{
		static wxFileConfig* AppConfigFile = new wxFileConfig("/store", "", "ue1Hook.cfg", "", wxCONFIG_USE_RELATIVE_PATH);
		if (wxFileExists("ue1Hook.cfg"))
		{
			return AppConfigFile;
		}
		else
		{
			return wxConfigBase::Get();
		}
	}
};
/*
class MyConfigAntigenBase
{
public:
	static wxConfigBase* Get()
	{
		static wxFileConfig* AppConfigFile = new wxFileConfig("", "", "wxHexEditor.cfg", "", wxCONFIG_USE_RELATIVE_PATH);
		if (wxFileExists("wxHexEditor.cfg"))
		{
			return AppConfigFile;
		}
		else
		{
			return wxConfigBase::Get();
		}
	}
};*/
