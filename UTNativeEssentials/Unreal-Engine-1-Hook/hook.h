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

#pragma once

#include <wx/wx.h>
#include <wx/fileconf.h>

// Forward declarations
class wxAuiManager;
class InfoPanel;
class InjectorLoop;
class wxCollapsiblePane;
class wxSpinCtrl;
class UDKHexEditor;
class wxFileHistory;
class wxFileName;

class KelvinFrame;

class UE1HookApp : public wxApp
{
public:
	bool OnInit() override;
	KelvinFrame* GetMyFrame() const { return m_Frame; }
	void OnIdle(wxIdleEvent& evt);

	// https://wiki.wxwidgets.org/Making_a_render_loop
	void ActivateInjectorLoop(bool on);

private:
	static KelvinFrame* m_Frame;
	bool m_InjectorLoop;
};

class KelvinFrame : public wxFrame
{
public:
	KelvinFrame();

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	/**
	 * @brief OpenFile entry callback
	 *
	 * Opens up OpenFile dialog window
	 *
	 * @see UDKHalo::UDKHalo()
	 */
	void OnOpenFile(wxCommandEvent& event);

	void OnUpdateUI(wxUpdateUIEvent& event);

public:
	/**
	 * @brief Opens the specified file for analysis
	 *
	 * @see UDKHalo::OnOpenFile(wxCommandEvent& event)
	 */
	void OpenFile(wxString filename, bool openAtRight);

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
	wxAuiManager* m_PaneManager;

	/**
	 * @brief Pane for file information
	 *
	 * All the useful information about the .u file package is \n
	 *
	 * @see InfoPanel
	 */
	InfoPanel* m_ProcessInfoPanel;

	/**
	 * @brief Loop for injection purposes
	 *
	 * @see
	 */
	static InjectorLoop m_InjectorLoop;
};

enum
{
	ID_Hello = 1
};

/// <summary>
/// Super class of InfoPanel
/// </summary>
class InfoPanelGui : public wxPanel
{
private:

protected:
	wxStaticText* m_InfoPanelText;

public:

	InfoPanelGui(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(140, 111), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);
	~InfoPanelGui();

};

/// <summary>
/// Class which displays relevant information of a file \n
/// including name, path, size, and access
/// </summary>
class InfoPanel : public InfoPanelGui
{
public:
	InfoPanel(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize(-1, -1), int style = wxTAB_TRAVERSAL)
		:InfoPanelGui(parent, id, pos, size, style) {}
	void Set(wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD, wxString XORKey);
	void OnUpdate(wxCommandEvent& event) {}
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
};
