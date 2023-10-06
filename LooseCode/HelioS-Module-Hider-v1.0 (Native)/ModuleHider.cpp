//=====================================================================================
// Project : HelioS-Module-Hider
// Version : 1.0
// Coder   : [ELF]HelioS
// Site    : http://www.ArtificialAiming.tk
//=====================================================================================

// What:
//  Hide a Dynamic Link Library from the module list on NT systems

// References:
//  Tetsuo       (http://www.game-deception.com)
//  NTinternals  (http://undocumented.ntinternals.net)
//  Phrack Inc.  (http://www.phrack.org)


#include "ModuleHider.h"


void HideModuleEx (HINSTANCE hModule, ELIST eList)
{
	PPEB Peb = (PPEB)0x7ffdf000; // Static pointer to the Process Environment Block
	char szPath[MAX_PATH];

	GetModuleFileNameA(hModule, szPath, MAX_PATH);

	if (Peb && Peb->LoaderData && Peb->LoaderData->Initialized)
	{
		PLIST_ENTRY pListHead = NULL, pListPtr = NULL;
		DWORD dOffset = 0;

		switch (eList) // There are 3 module lists with different offsets
		{
			case LIST_LoadOrder:
				pListHead = pListPtr = &(Peb->LoaderData->InLoadOrderModuleList);
				dOffset = 0x24;
				break;

			case LIST_MemoryOrder:
				pListHead = pListPtr = &(Peb->LoaderData->InMemoryOrderModuleList);
				dOffset = 0x1C;
				break;

			case LIST_InitOrder:
				pListHead = pListPtr = &(Peb->LoaderData->InInitializationOrderModuleList);
				dOffset = 0x14;
				break;
		}

		if (pListHead && pListPtr)
		{
			// LIST_ENTRY is a double linked list, so we need to loop thru 2 lists

			do // Loop thru Flink
			{
				char name[MAX_PATH];
				wsprintfA(name, "%S", ((PUNICODE_STRING)((PCHAR)pListPtr->Flink + dOffset))->Buffer);

				if (!stricmp(name, szPath)) // We found our module, so lets hide it
				{
					pListPtr->Flink = pListPtr->Flink->Flink;
				}

				pListPtr = pListPtr->Flink;

			} while (pListPtr->Flink != pListHead->Flink);

			do // Loop thru Blink
			{
				char name[MAX_PATH];
				wsprintfA(name, "%S", ((PUNICODE_STRING)((PCHAR)pListPtr->Blink + dOffset))->Buffer);

				if (!stricmp(name, szPath)) // We found our module, so lets hide it
				{
					pListPtr->Blink = pListPtr->Blink->Blink;
				}

				pListPtr = pListPtr->Blink;

			} while (pListPtr->Blink != pListHead->Blink);
		}
	}
}


void HideModule (HINSTANCE hModule)
{
	HideModuleEx(hModule, LIST_LoadOrder);
	HideModuleEx(hModule, LIST_MemoryOrder);
	HideModuleEx(hModule, LIST_InitOrder);
}


BOOL WINAPI DllMain (HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
  	if (dwReason == DLL_PROCESS_ATTACH)
  	{
  		HideModule(hModule);
	}

	return TRUE;
}