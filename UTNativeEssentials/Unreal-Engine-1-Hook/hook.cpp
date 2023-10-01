/************************************************************************
 *   Unreal Engine Pathogen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ************************************************************************/

#include "hook.h"
#include <iostream>

#include <tchar.h>

#include <atlstr.h>

int main(int argc, char** argv)
{
	HANDLE snapshot = 0;
	HANDLE process = 0;
	HMODULE hDll = 0;
	//DWORD exitCode;
	//const char* dll_path = "G:\\SteamLibrary\\UnrealTournamentGOTY\\ChatDiamond\\ChatDiamondBin\\Win\\x86\\Release\\IDK.dll";
	void* lpBaseAddress = nullptr;

	PROCESSENTRY32 pe32 = { 0 };

	pe32.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(snapshot, &pe32);

	do 
	{
		if (wcscmp(pe32.szExeFile, L"UnrealTournament.exe") == 0) 
		{
			std::cout << "Process found, injecting code\n";
			const char* path = "IDK.dll";

			HMODULE handle = LoadLibraryA(path);

			break;
		}
	} 
	while (Process32Next(snapshot, &pe32));

	getchar();

	return 0;
}