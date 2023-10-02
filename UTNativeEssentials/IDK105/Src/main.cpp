/*
    IDK 1.0.5d Source Code
    Copyright (C) 2007,2008 CheatatOr http://www.cheatator.tk
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    Credits: HelioS, Grief, Saphex, r00t.        
*/
#include "main.h"
#include "cRadar.h"
#include "cMenu.h"
#include "cAimbot.h"
#include "iostream"

void WINAPI MyProcessEvent(class UFunction* Function, void* Parms, void* Result = NULL);
void WINAPI MProcessEvent(class UFunction* Function, void* Parms, void* Result = NULL);

typedef void(WINAPI* PrEv)(class UFunction*, void*, void*);  // PrEv define

PrEv orgProcessEvent; // Storage for Original ProcessEvent
DWORD Bump; // Bump

UCanvas* pCanvas; // Private Canvas

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
	/*
	if ((fp = fopen("c:/hook.log", "a+")) != NULL)
	{
		fprintf(fp, "%s\n", logbuf);
		fclose(fp);
	}
	*/
}

void Init (UCanvas* Canvas)
{
    if (bInit) return;

	LoadTexture(WhiteTexture,	"UWindow.WhiteTexture");

	bInit = true;
}

void CheckKeys (UCanvas* Canvas)
{
    if (GetAsyncKeyState(VK_NUMPAD0)&(1==1))
	{
		bHook = !bHook;
	}
    else if(GetAsyncKeyState(VK_NUMPAD1)&(1==1))
	{
	    (bAutoAim==2)?(bAutoAim=0):(bAutoAim++);
	}
    else if (GetAsyncKeyState(VK_NUMPAD2)&(1==1))
	{
        bAutoFire = !bAutoFire;	
	}
    else if (GetAsyncKeyState(VK_NUMPAD3)&(1==1))
	{
	    bTrigger = !bTrigger;	
	}
    else if (GetAsyncKeyState(VK_NUMPAD4)&(1==1))
	{
        b3DRadar = !b3DRadar;	
	}
    else if (GetAsyncKeyState(VK_NUMPAD5)&(1==1))
	{
	    b2DRadar = !b2DRadar;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD6)&(1==1))
	{
        bHealthbars = !bHealthbars;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD7)&(1==1))
	{
        bSettings = !bSettings;	
	}
    else if (GetAsyncKeyState(VK_NUMPAD8)&(1==1))
	{
	    bInfo = !bInfo;
	}
}

void GetCameraLocation (FVector &InputLocation, FRotator &InputRotation)
{
    struct APlayerPawn_eventPlayerCalcView_Parms
	{
	    class AActor* ViewActor;
		FVector CameraLocation;
		FRotator CameraRotation;
	};

	static APlayerPawn_eventPlayerCalcView_Parms Parms;

	if(Me == NULL)return;

	Parms.ViewActor = Me;

	UFunction* GetCameraLocation = Me->FindFunction(TEXT("PlayerCalcView"));

	if(GetCameraLocation == NULL)return;

	Me->ProcessEvent(GetCameraLocation, &Parms, 0);

	InputLocation = Parms.CameraLocation;
	InputRotation = Parms.CameraRotation;
}

void inline PawnIterator(UCanvas* Canvas)
{
	if (!bHook ) return;

	if(b2DRadar)
	{
		Hook.cM->DrawBox(Canvas,10,177,134,137);
		Hook.cM->DrawRadarMenu(Canvas);
	}

	Deltatime = ((Me->Level->TimeSeconds - LastTimerCheck) / Me->Level->TimeDilation);
	LastTimerCheck = Me->Level->TimeSeconds;

	APawn* BestTarget = NULL;
	GetCameraLocation(MyCameraLocation,MyCameraRotation);
	bValidAim = (Hook.cA->GoodAim() && Hook.cA->IsGoodWeapon() && !bTrigger);

	for (AObjectIterator<APawn> Pawn = AObjectIterator<APawn>(DefPawn); Pawn; ++Pawn)
	{
	    APawn *Target= (APawn*)*Pawn;

		if ( ValidTarget(Target))
		{
		    Hook.cR->DrawPlayerOnRadar(Canvas,Target);
		    Hook.cR->DrawPlayer2DRadar(Canvas,Target);

			Hook.cA->Trigger(Target);

			 if ( bAutoAim )
			 {
			     if ( bValidAim && IsEnemy(Target) && Hook.cA->IsVisible(Target) )
				 {
				     BestTarget = Hook.cA->GetBestTarget(Canvas,BestTarget, Target);
				 }
			 }
		 }
	 }
	 if (( BestTarget != NULL) && Hook.cA->IsInRange(BestTarget))
	 {
	     //Me->MouseSensitivity = 0;
		 block = true;	
	     Hook.cA->SetMyRotation(Canvas, BestTarget);
		 if ( bAutoFire)
		 {
		     Hook.cA->FireMyWeapon();
		 }
	 }
	 else
	 {
	     //Me->MouseSensitivity = 3;
		 Hook.cA->StopMyWeapon();
		 block = false;
	 }
}

void MyPostRender (UCanvas* Canvas)
{
	if ( !ValidRender(Canvas) ) return;

	Me = Canvas->Viewport->Actor;

	PawnIterator(Canvas);
	CheckKeys(Canvas);		
	Init(Canvas);
		
	if (!bHook ) return;

	if(bSettings)
	{
		Hook.cM->DrawBox(Canvas,10,340,134,137);
		Hook.cM->DrawSettings(Canvas);
	}

	if(bInfo)
	{
		Hook.cM->DrawBox(Canvas,10,501,134,137);
		Hook.cM->MyInfos(Canvas);
	}
}

typedef void(WINAPI *tPostRender)(struct FSceneNode *);
tPostRender oPostRender = NULL;
int WINAPI xPostRender(struct FSceneNode *FS)
{
	//log_add("Taking detour for PostRender");

	oPostRender(FS);
	MyPostRender(FS->Viewport->Canvas);

	GLog->Logf(TEXT("Detour is in action"));

	return 1;
}

DWORD WINAPI LoaderThread( LPVOID lpParam )
{
	log_add("LoaderThread spawned. Attempt to hook into Render.dll");

	HMODULE hDll = LoadLibraryA("Render.dll");

	if (hDll != nullptr)
	{
		log_add("Got handle from Render.dll at address %x", hDll);
	}
	else
	{
		log_add("Couldn't get hold of the handle. Last error: %u", GetLastError());

		return getchar();
	}

	void* pAddress = (void*)GetProcAddress(hDll, "?PostRender@URender@@UAEXPAUFSceneNode@@@Z");

	if (pAddress != nullptr)
	{
		log_add("Attempt to hook at address %x", pAddress);
	}
	else
	{
		log_add("Couldn't hook into the PostRender routine");

		return getchar();
	}
	
	/*oPostRender errorCode = (tPostRender)DetourAttach((PBYTE)pAddress, (PBYTE)xPostRender)*/;

	oPostRender = (tPostRender)DetourFindFunction("Render.dll", "?PostRender@URender@@UAEXPAUFSceneNode@@@Z");

	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	LONG errorCode = DetourAttach(&(PVOID&)oPostRender, xPostRender);
	DetourTransactionCommit();

	log_add("Detour done with original %i", errorCode);

	return 1;
}

BOOL __stdcall DllMain(HMODULE hDll, DWORD reason, PVOID lpReserved)
{
	switch(reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			log_add("Attaching dll to unreal tournament process");

			CreateThread(0, 0, LoaderThread, 0, 0, 0);
			DisableThreadLibraryCalls(hDll);
			
			/*log_add("===================================================");
			log_add("Attaching DLL");
			DisableThreadLibraryCalls(hDll);
			ReDirectFunction("Core.dll", "?ProcessEvent@UObject@@UAEXPAVUFunction@@PAX1@Z", (DWORD)&MyProcessEvent); // redirect ProcessEvent*/
			return true;
		}
		default:
		{
			return false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: MyProcessEvent(class UFunction* Function, void* Params, void* Result=NULL)
 *
 *  PURPOSE: Hooked ProcessEvent from RedirectFunction
 *
 *  COMMENTS:
 *
 *       Calls original ProcessEvent and MyProcessEvent in an orderly fasion
 *
 */
void WINAPI MyProcessEvent(class UFunction* Function, void* Parms, void* Result)
{
	log_add("MyProcess Event");

	DWORD Bumpeh = Function->GetIndex();
	if (Bump)
	{
		if (Bump != Bumpeh)
		{
			orgProcessEvent(Function, Parms, Result); // call original ProcessEvent
			MProcessEvent(Function, Parms, Result); // call Our ProcessEvent
		}
		else
		{
			//zz
		}
	}
	else
	{
		orgProcessEvent(Function, Parms, Result); // call original ProcessEvent
		MProcessEvent(Function, Parms, Result); // call Our ProcessEvent
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: WINAPI MProcessEvent(class UFunction* Function, void* Params, void* Result=NULL)
 *
 *  PURPOSE: Called After the original ProcessEvent, allows you to hook the majority of
 *				UEngine functions.
 *
 */
void WINAPI MProcessEvent(class UFunction* Function, void* Parms, void* Result)
{
	FString func;
	func = (FString)Function->GetName();
	char buffer[255];

	for (INT i = 0; i < func.Len(); i++)
	{
		buffer[i] = func.Mid(i, 1);
		//sprintf(&buffer[i], "%s", func.Mid(i, 1));
	}
	log_add("FUNC %s  %d", buffer, Function->GetIndex()); // log all functions called through ProcessEvent

	if (func.InStr(TEXT("PostRender")) != -1) // called PostRender
	{
		AHUD_eventPostRender_Parms* PostRend; // storage for hooked PostRender
		PostRend = (AHUD_eventPostRender_Parms*)Parms; // hook PostRender
		if (!doonce)
		{
			log_add("hooked PostRender @ 0x%x", (DWORD)PostRend); // log that it's been hooked
			doonce = true;
		}
		pCanvas = PostRend->Canvas; // make hooked canvas = our private canvas
		pPostRender(); // call hooked PostRender
	}
	else if (func.InStr(TEXT("PreRender")) != -1) // called PreRender
	{
		AHUD_eventPreRender_Parms* PreRend; // storage for hooked PreRender
		PreRend = (AHUD_eventPreRender_Parms*)Parms; // hook PreRender
		if (!doonce1)
		{
			log_add("hooked PreRender @ 0x%x", (DWORD)PreRend); // log that it's been hooked
			doonce1 = true;
		}
		pPreRender(); // call hooked PreRender
	}
	else if (func.InStr(TEXT("Tick")) != -1) // called Tick
	{
		AActor_eventTick_Parms* Tickeh; // storage for hooked Tick
		Tickeh = (AActor_eventTick_Parms*)Parms; // hook Tick
		if (!doonce2)
		{
			log_add("hooked Tick @ 0x%x", (DWORD)Tickeh); // log that it's been hooked
			doonce2 = true;
		}
		pTick(); // call hooked Tick
	}
	else if (func.InStr(TEXT("Bump")) != -1) // bump called
	{
		Bump = Function->GetIndex();
	}
	else
	{
		// zz
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: pPostRender(void)
 *
 *  PURPOSE: Execute code through hooked PostRender
 *
 *  COMMENTS:
 *
 *		One of the most important functions that you hook, PostRender. You
 *		will put the majority of your code here since it is called every time
 *		that the original PostRender is called.
 */
void pPostRender()
{
	/*
	pCanvas->Color = FColor(0, 255, 255, 0);
	pCanvas->CurX = 50;
	pCanvas->CurY = 50;
	pCanvas->WrappedPrintf(pCanvas->MedFont, 0, TEXT("Herro Worrd!"));
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: pTick(void)
 *
 *  PURPOSE: Execute code through hooked Tick
 *
 *  COMMENTS:
 *
 *		Almost like PostRender but different, you may get use out of this too.
 */
void pTick()
{
	// zz
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: pPreRender(void)
 *
 *  PURPOSE: Execute code through hooked PreRender
 *
 *  COMMENTS:
 *
 */
void pPreRender()
{
	// zz
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  FUNCTION: ReDirectFunction(char* strDllName, char* strFunctionName, DWORD newFuncAddy)
 *
 *  PURPOSE: Redirect functions from withinside a dll to a hooked function.
 *
 *  COMMENTS:
 *
 *		Used in this case to Redirect the most important function of all - ProcessEvent
 *
 */
int ReDirectFunction(const char* strDllName, const char* strFunctionName, DWORD newFuncAddy)
{
	DWORD                       dwBackup;
	DWORD                       dwIndex;
	DWORD                       dwOffset;
	HMODULE                     hmHL;
	PIMAGE_DATA_DIRECTORY       pDataDirectory;
	PIMAGE_DOS_HEADER           pDosHeader;
	PDWORD                      pdwIAT;
	PDWORD                      pdwINT;
	PIMAGE_IMPORT_DESCRIPTOR    pImportDescriptor;
	PIMAGE_IMPORT_BY_NAME       pImportName;
	PIMAGE_OPTIONAL_HEADER      pOptionalHeader;
	PIMAGE_NT_HEADERS           pPeHeader;
	PSTR                        strCurrent;
	hmHL = LoadLibraryA(strDllName); // unicode love

	log_add("Attempting redirect with hooked %s", strDllName);

	if (hmHL == nullptr)
	{
		log_add("Couldn't hook %s", strDllName);

		return getchar();
	}
	else
	{
		log_add("Found the handle at address %x", hmHL);
	}

	pDosHeader = PIMAGE_DOS_HEADER(hmHL);
	dwOffset = pDosHeader->e_lfanew;
	pPeHeader = PIMAGE_NT_HEADERS(long(hmHL) + dwOffset);
	pOptionalHeader = &pPeHeader->OptionalHeader;
	pDataDirectory = pOptionalHeader->DataDirectory;
	dwOffset = pDataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	pImportDescriptor = PIMAGE_IMPORT_DESCRIPTOR(long(hmHL) + dwOffset);

	/*for (dwIndex = 0; true; dwIndex++)
	{
		log_add("%u", dwIndex);
		dwOffset = pImportDescriptor[dwIndex].Name;
		strCurrent = PSTR(long(hmHL) + dwOffset);
		if (stricmp(strCurrent, strDllName) == 0)
		{
			break;
		}
	}*/

	strCurrent = (PSTR)"Hmm";
	dwIndex = 0;

	while (stricmp(strCurrent, strDllName) != 0)
	{
		log_add("dwIndex: %u, strCurrent: %s", dwIndex, strCurrent);

		dwOffset = pImportDescriptor[dwIndex].Name;
		strCurrent = PSTR(long(hmHL) + dwOffset);

		dwIndex++;
	}

	log_add("1. dwIndex is %u", dwIndex);

	dwOffset = pImportDescriptor[dwIndex].FirstThunk;
	pdwIAT = PDWORD(long(hmHL) + dwOffset);
	dwOffset = pImportDescriptor[dwIndex].OriginalFirstThunk;
	pdwINT = PDWORD(long(hmHL) + dwOffset);
	for (dwIndex = 0; true; dwIndex++)
	{
		dwOffset = pdwINT[dwIndex];
		pImportName = PIMAGE_IMPORT_BY_NAME(long(hmHL) + dwOffset);
		strCurrent = PSTR(pImportName->Name);
		if (stricmp(strCurrent, strFunctionName) == 0) break;
	}

	log_add("2. dwIndex is %u", dwIndex);

	VirtualProtect(&pdwIAT[dwIndex], sizeof(DWORD), PAGE_READWRITE, &dwBackup);
	orgProcessEvent = (PrEv)pdwIAT[dwIndex]; // ding ding
	pdwIAT[dwIndex] = PtrToUlong(newFuncAddy);
	VirtualProtect(&pdwIAT[dwIndex], sizeof(DWORD), dwBackup, &dwOffset);

	log_add("======== Injection success =========");
	return 1;
}