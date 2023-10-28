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
#include "detours.h"
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
	//_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
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
	/*if (GetAsyncKeyState(VK_NUMPAD0) & 0x01)
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
	}*/
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

	MWindow.AddCheckBoxByColor(XPosTracker + rightAlignment, setting2y, autoColor, TEXT("AutoAim"));
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

	// For linux based testing
	GLog->Logf(TEXT("Detour is in action"));
}

DWORD WINAPI LoaderThread(LPVOID lpParam)
{
#ifdef HOOK_WINDOWS_PLATFORM
	oPostRender = (tPostRender)DetourFindFunction("Render.dll", "?PostRender@URender@@UAEXPAUFSceneNode@@@Z");
#elif HOOK_LINUX_PLATFORM
	oPostRender = (tPostRender)DetourFindFunction("Render.so", "_ZN7URender10PostRenderEP10FSceneNode");
#endif

#ifdef HOOK_WINDOWS_PLATFORM
	DetourRestoreAfterWith();
	//log_add("RestoreAfter %i", GetLastError());

	DetourTransactionBegin();
	//log_add("Transaction begin %i", GetLastError());

	DetourUpdateThread(GetCurrentThread());
	//log_add("Update Thread %i", GetLastError());

	LONG errorCode = DetourAttach(&(LPVOID&)oPostRender, xPostRender);
	DetourTransactionCommit();
#elif HOOK_LINUX_PLATFORM
	TRACED_HOOK_HANDLE aHandle;
	LONG errorCode = DetourInstallHook(&oPostRender, (void*)&xPostRender, NULL, aHandle);
#endif

	return 1;
}

#ifdef HOOK_LINUX_PLATFORM
/*
 * loadMsg()
 *
 * This function is automatically called when the sample library is injected
 * into a process. It calls hello() to output a message indicating that the
 * library has been loaded.
 *
 */
 #include <dlfcn.h>
__attribute__((constructor))
void loadMsg()
{

	void* handle = dlopen("Rende.so", RTLD_NOW);
	oPostRender = (tPostRender)dlsym(handle, "_ZN7URender10PostRenderEP10FSceneNode");

	GLog->Logf(TEXT("##################### %x"), oPostRender);

	//LoaderThread(NULL);
	//oPostRender = (tPostRender)DetourFindFunction("Render.so", "_ZN7URender10PostRenderEP10FSceneNode");

	TRACED_HOOK_HANDLE aHandle;
	LONG errorCode = DetourInstallHook(&oPostRender, (void*)&xPostRender, NULL, aHandle);
}
#endif

#ifdef HOOK_WINDOWS_PLATFORM
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
#endif
