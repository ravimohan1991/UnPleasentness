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
#include "MorbiD.h"

void WINAPI MyProcessEvent(class UFunction* Function, void* Parms, void* Result = NULL);
void WINAPI MProcessEvent(class UFunction* Function, void* Parms, void* Result = NULL);

typedef void(WINAPI* PrEv)(class UFunction*, void*, void*);  // PrEv define

PrEv orgProcessEvent; // Storage for Original ProcessEvent
DWORD Bump; // Bump

UCanvas* pCanvas; // Private Canvas

//For configuring
MorbidWindow ConfigurationWindow;

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

	LoadTexture(WhiteTexture, "UWindow.WhiteTexture");
	
	// some loading sequence for dynamic fonts.
	
	LoadFont(SmallFont, "LadderFonts.UTLadder22");

	bInit = true;
}

// Windows specific
void CheckKeys (UCanvas* Canvas)
{
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x01)
	{
		bHook = !bHook;
	}
	else if (GetAsyncKeyState(VK_NUMPAD1) & 0x01)
	{
		(bAutoAim==2)?(bAutoAim=0):(bAutoAim++);
	}
	else if (GetAsyncKeyState(VK_NUMPAD2) & 0x01)
	{
		bAutoFire = !bAutoFire;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD3) & 0x01)
	{
		bTrigger = !bTrigger;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD4) & 0x01)
	{
		b3DRadar = !b3DRadar;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD5) & 0x01)
	{
		b2DRadar = !b2DRadar;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD6) & 0x01)
	{
		bHealthbars = !bHealthbars;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD7) & 0x01)
	{
		bSettings = !bSettings;	
	}
	else if (GetAsyncKeyState(VK_NUMPAD8) & 0x01)
	{
		bInfo = !bInfo;
	}
	else if (GetAsyncKeyState(VK_HOME) & 0x01)
	{
		Scale += 0.1f;
	}
	else if (GetAsyncKeyState(VK_END) & 0x01)
	{
		Scale -= 0.1f;
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

struct DrawActor_Parms
{
	AActor* Actor;
	BITFIELD WireFrame;
	BITFIELD ClearZ;
};

void inline PawnIterator(FSceneNode* FS)
{
	UCanvas* Canvas = FS->Viewport->Canvas;

	if (!bHook || !Me) return;

	if(b2DRadar)
	{
		Hook.cM->DrawBox(Canvas, 10, 177.0f, 134.0f * Scale, 137.0f * Scale);
		Hook.cM->DrawRadarMenu(Canvas, 10, 177.0f, 134.0f * Scale, 137.0f * Scale);
	}

	Deltatime = ((Me->Level->TimeSeconds - LastTimerCheck) / Me->Level->TimeDilation);
	LastTimerCheck = Me->Level->TimeSeconds;

	APawn* BestTarget = NULL;
	GetCameraLocation(MyCameraLocation, MyCameraRotation);
	bValidAim = (Hook.cA->GoodAim() && Hook.cA->IsGoodWeapon() && !bTrigger);

	if (!Me->Level->PawnList)
	{
		return;
	}

	for(APawn* Pawn = Me->Level->PawnList; Pawn->nextPawn != nullptr; Pawn = Pawn->nextPawn)
	{
		APawn *Target = Pawn;

		// Glow hack
		if (Target && IsEnemy(Target))
		{
			Target->LightEffect = LE_NonIncidence;
			Target->LightType = LT_Steady;
			Target->AmbientGlow = 150;
			Target->LightRadius = 005;
			Target->LightBrightness = 255;
			Target->LightSaturation = 100;

			switch (Target->PlayerReplicationInfo->Team)
			{
				case 0:
				Target->LightHue = 255;
				break;

				case 1:
				Target->LightHue = 170;
				break;

				case 2:
				Target->LightHue = 85;
				break;

				case 3:
				Target->LightHue = 40;
				break;

			default:
				Target->LightHue = 85;
				break;
			}
		}

		if (ValidTarget(Target))
		{
			Hook.cR->DrawPlayerOnRadar(Canvas, Target);
			Hook.cR->DrawPlayer2DRadar(Canvas, Target, 10, 177.0f, 134.0f * Scale, 137.0f * Scale);

			Hook.cA->Trigger(Target);

			if (bAutoAim)
			{
				if (bValidAim && IsEnemy(Target) && Hook.cA->IsVisible(Target))
				{
					BestTarget = Hook.cA->GetBestTarget(Canvas, BestTarget, Target);
				}
			}
		}
	}

	if ((BestTarget != NULL) && Hook.cA->IsInRange(BestTarget))
	{
		//Me->MouseSensitivity = 0;
		block = true;	
		Hook.cA->SetMyRotation(Canvas, BestTarget);
		if (bAutoFire)
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

float setting1x, setting1y;
float setting2x, setting2y;
float setting3x, setting3y;
float setting4x, setting4y;
float setting5x, setting5y;
float setting6x, setting6y;
float setting7x, setting7y;
float setting8x, setting8y;

void DrawMorbiDConfigurationWindow(UCanvas* Canvas)
{
	float YPosTracker = 900.0f;
	float XPosTracker = Canvas->ClipX - 134.0f * Scale - 10;
	static float verticalGap = 30.0f;
	float rightAlignment = 15.0f;

	//Draw our configuration window
	MWindow.DrawWindow(ConfigurationWindow, XPosTracker, YPosTracker, 134.0f * Scale, 2 * 134.0f * Scale, 1, false, TEXT("Settings Window"));
	
	YPosTracker += 56.0f;// temporary

	//Add all the variety of checkboxes for the settings we have

	// 1. bHook = !bHook;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, bHook, true, TEXT("Aimbot"));
	setting1x = XPosTracker + rightAlignment;
	setting1y = YPosTracker;
	YPosTracker += verticalGap;

	// 2 bAutoAim (bAutoAim==2)?(bAutoAim=0):(bAutoAim++)
	FColor autoColor;
	switch (bAutoAim)
	{
	case 0:
		autoColor = Color.White();
		break;
	case 1:
		autoColor = Color.Blue();
		break;
	case 2:
		autoColor = Color.Pink();
		break;
	}

	MWindow.AddCheckBoxByColor(XPosTracker + rightAlignment, setting2y, autoColor, TEXT("Aimbot"));
	setting2x = XPosTracker + rightAlignment;
	setting2y = YPosTracker;
	YPosTracker += verticalGap;

	// 3. bTrigger = !bTrigger
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, bTrigger, true, TEXT("Trigger Bot"));
	setting3x = XPosTracker + rightAlignment;
	setting3y = YPosTracker;
	YPosTracker += verticalGap;

	// 4. b2DRadar = !b2DRadar;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, b2DRadar, true, TEXT("Radar 2D"));
	setting4x = XPosTracker + rightAlignment;
	setting4y = YPosTracker;
	YPosTracker += verticalGap;

	// 5. b3DRadar = !b3DRadar;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, b3DRadar, true, TEXT("Radar 3D"));
	setting5x = XPosTracker + rightAlignment;
	setting5y = YPosTracker;
	YPosTracker += verticalGap;

	// 6. bHealthbars = !bHealthbars;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, bHealthbars, true, TEXT("Health Bars"));
	setting6x = XPosTracker + rightAlignment;
	setting6y = YPosTracker;
	YPosTracker += verticalGap;

	// 7. bSettings = !bSettings;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, bSettings, true, TEXT("Settings"));
	setting7x = XPosTracker + rightAlignment;
	setting7y = YPosTracker;
	YPosTracker += verticalGap;

	// 8. bInfo = !bInfo;
	MWindow.AddCheckBox(XPosTracker + rightAlignment, YPosTracker, bInfo, true, TEXT("Information"));
	setting8x = XPosTracker + rightAlignment;
	setting8y = YPosTracker;
}

void LookForMouseInput()
{
	float rightAlignment = 15.0f;

	if (Mouse.CheckBoxClick(setting1x, setting1y, true))
	{
		bHook = !bHook;
	}
	else if (Mouse.CheckBoxClick(setting2x, setting2y, true))
	{
		(bAutoAim == 2) ? (bAutoAim = 0) : (bAutoAim++);
	}
	else if (Mouse.CheckBoxClick(setting3x, setting3y, true))
	{
		bTrigger = !bTrigger;
	}
	else if (Mouse.CheckBoxClick(setting4x, setting4y, true))
	{
		b2DRadar = !b2DRadar;
	}
	else if(Mouse.CheckBoxClick(setting5x, setting5y, true))
	{
		b3DRadar = !b3DRadar;
	}
	else if(Mouse.CheckBoxClick(setting6x, setting6y, true))
	{
		bHealthbars = !bHealthbars;
	}
	else if(Mouse.CheckBoxClick(setting7x, setting7y, true))
	{
		bSettings = !bSettings;
	}
	else if (Mouse.CheckBoxClick(setting8x, setting8y, true))
	{
		bInfo = !bInfo;
	}
}

void MyPostRender (FSceneNode* FS)
{
	UCanvas* Canvas = FS->Viewport->Canvas;

	if (!ValidRender(Canvas))
	{
		return;
	}

	Me = Canvas->Viewport->Actor;

	PawnIterator(FS);
	CheckKeys(Canvas);

	Init(Canvas);
	MWindow.MorbidWindowsMain(Canvas);
		
	if (!bHook ) return;

	if(bSettings)
	{
		Hook.cM->DrawBox(Canvas, 10, 177.0f + 137.0f * Scale + 25, 134.0f * Scale, 137.0f * Scale);
		Hook.cM->DrawSettings(Canvas, 10, 177.0f + 137.0f * Scale + 25, 134.0f * Scale, 137.0f * Scale);
	}

	if(bInfo)
	{
		Hook.cM->DrawBox(Canvas, 10, 177.0f + 137.0f * Scale + 137.0f * Scale + 50, 134.0f * Scale, 137.0f * Scale);
		Hook.cM->MyInfos(Canvas, 10, 177.0f + 137.0f * Scale + 137.0f * Scale + 50, 134.0f * Scale, 137.0f * Scale);
	}

	if (bControls)
	{
		DrawMorbiDConfigurationWindow(Canvas);
		LookForMouseInput();
	}
}

typedef void(*tPostRender)(FSceneNode *);
tPostRender oPostRender = NULL;

void xPostRender(FSceneNode *FS)
{
	MyPostRender(FS);

	//GLog->Logf(TEXT("Detour is in action"));
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
	
	oPostRender = (tPostRender)DetourFindFunction("Render.dll", "?PostRender@URender@@UAEXPAUFSceneNode@@@Z");
	log_add("Another approach for hookable address %x, error code: %i", oPostRender, GetLastError());


	DetourRestoreAfterWith();
	log_add("RestoreAfter %i", GetLastError());

	DetourTransactionBegin();
	log_add("Transaction begin %i", GetLastError());
	
	DetourUpdateThread(GetCurrentThread());
	log_add("Update Thread %i", GetLastError());

	LONG errorCode = DetourAttach(&(LPVOID&)oPostRender, xPostRender);
	DetourTransactionCommit();

	log_add("Detour done with original %i and %i", errorCode, GetLastError());

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
			return true;
		}
		case DLL_PROCESS_DETACH:
		{
			log_add("Gracefully detaching from the process");

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(LPVOID&)oPostRender, xPostRender);
			DetourTransactionCommit();
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