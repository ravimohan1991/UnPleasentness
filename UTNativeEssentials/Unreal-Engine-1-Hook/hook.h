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

#include "windows.h"
#include "iostream"

/*
 *  FUNCTION: log_add(const char *fmt, ...)
 *
 *  PURPOSE: log information to a file with timestamps.
 *
 *  COMMENTS:
 *
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

	printf("%s\n", logbuf);
}

class KelvinFrame;

class UE1HookApp : public wxApp
{
public:
	bool OnInit() override;

	KelvinFrame* GetMyFrame() const { return m_Frame; }

private:
	static KelvinFrame* m_Frame;
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

public:
	/**
	 * @brief Opens the specified file for analysis
	 *
	 * @see UDKHalo::OnOpenFile(wxCommandEvent& event)
	 */
	void OpenFile(wxString filename, bool openAtRight);
};

enum
{
	ID_Hello = 1
};