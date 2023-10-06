//=====================================================
//
//Morb!D-Windows 1.1
//
//=====================================================

#include "HookMaster.h"

int iCanvas = 0;
int iSwitch = 0;


//=====================================================================================
// Global Settings:
//=====================================================================================
	APlayerController*	PC			= NULL;
	APawn*				Me			= NULL;

	FVector				MyCameraLocation;
	FRotator			MyCameraRotation;

	float				DeltaTime;


bool bHealthBars = true;
bool b2DRadar	 = false;


void DrawMyWindows(UCanvas * Canvas)
{
//define the used windows
	MorbidWindow Window1;
	MorbidWindow Window2;
	MorbidWindow Window3;
	MorbidWindow Window4;

	//drawwindow
	MWindow.DrawWindow(Window1,0,0,Canvas->ClipX,80,2,true,NULL);
	//Drawtext
	MWindow.MakeShadowText(200,30,TEXT("This is some Example ShadowText"),Color.White(),SmallFont);
	MWindow.DrawText(500,60,TEXT("Some Normal Text........"),Color.Black(),SmallFont);
	MWindow.DrawText(500,50,TEXT("Some Normal Text........"),Color.Red(),SmallFont);
	MWindow.DrawText(500,40,TEXT("Some Normal Text........"),Color.Yellow(),SmallFont);

	//drawwindow
	MWindow.DrawWindow(Window2,100,200,150,300,1,false,TEXT("CheckBox Window"));
	//addcheckboxes rightalined
	MWindow.AddCheckBox(120,250,bHealthBars,true,TEXT("Example option"));
	MWindow.AddCheckBox(120,265,b2DRadar   ,true,TEXT("Example option2"));
	MWindow.AddCheckBox(120,280,b2DRadar   ,true,TEXT("Example option3"));
	MWindow.AddCheckBox(120,295,bHealthBars,true,TEXT("Example option4"));
	MWindow.AddCheckBox(120,310,b2DRadar   ,true,TEXT("...."));
	
	//addbottons
	MWindow.AddButton(120,340,70,17,TEXT("Button1"),false);
	MWindow.AddButton(120,360,70,17,TEXT("Button2"),false);
	MWindow.AddButton(120,380,60,50,TEXT("Big one"),false);

	//addcheckboxes left alined
	MWindow.AddCheckBox(120,440,b2DRadar   ,false,TEXT("Left text"));
	MWindow.AddCheckBox(120,455,bHealthBars,false,TEXT("Left text2"));

	//draw transparent window
	MWindow.DrawWindow(Window3,300,300,300,200,1,true,TEXT("Transparent Window"));

	//add transparent button
	MWindow.AddButton(350,350,70,17,TEXT("Trans"),true);
	MWindow.AddButton(350,370,70,17,TEXT("Trans2"),true);

	//draw smal window without titlebox
	MWindow.DrawWindow(Window4,600,600,200,50,2,true,NULL);

	//draw radar box
	MWindow.DrawRadarBox(900,400,100,100,true);

	//draw
	MWindow.CreateMouse();
}


void MyPostRender(UCanvas *Canvas)
{
	PC = Canvas->Viewport->Actor;
	Me = PC->Pawn;

	MWindow.MorbidWindowsMain(Canvas);

	try
	{
		if ((PC != NULL) && (Me != NULL))
		{
			DrawMyWindows(Canvas);
		}
	}
	catch(...){}
}


void MyTick (float Delta)
{
	try
	{
		if ((PC != NULL) && (Me != NULL))
		{
			Mouse.UpdateMouse(DeltaTime);
		}
	}
	catch(...)
	{
	}
}


void WINAPI PreProcessEvent(class UFunction* Function, void* Parms, void* Result=NULL)
{
	if(wcsicmp(Function->GetName(), L"PlayerTick") == 0)
	{
		MyTick(((Event_PlayerTick_Parms*)Parms)->DeltaTime);
	}
}

void WINAPI PostProcessEvent(class UFunction* Function, void* Parms, void* Result=NULL)
{
	if(wcsicmp(Function->GetName(), L"PostRender") == 0)
	{
		MyPostRender(((Event_PostRender_Parms*)Parms)->Canvas);
	} 

	if(wcsicmp(Function->GetName(), L"PlayerCalcView") == 0)
	{
		MyCameraLocation = ((Event_PlayerCalcView_Parms*)Parms)->CameraLocation;
		MyCameraRotation = ((Event_PlayerCalcView_Parms*)Parms)->CameraRotation;
	}
}

//=====================================================================================
// (PreProcessed) ProcessEvent:
//=====================================================================================
void* oProcessEvent;
void WINAPI xProcessEvent(class UFunction* Function, void* Parms, void* Result=NULL)
{
	__asm pushad;
	PreProcessEvent(Function, Parms, Result);
	__asm popad;

	__asm
	{
		push Result
		push Parms
		push Function
		call oProcessEvent
	}

	__asm pushad;
	PostProcessEvent(Function, Parms, Result);
	__asm popad;
}

//=====================================================================================
// Core Hooking:
//=====================================================================================

void* ReDirectFunction(char* strDllName, char* strFunctionName, DWORD newFuncAddy)
{
	DWORD                       dwBackup;
	DWORD                       dwIndex;
	DWORD                       dwOffset;
	HMODULE                     hEng;
	PIMAGE_DATA_DIRECTORY       pDataDirectory;
	PIMAGE_DOS_HEADER           pDosHeader;
	PDWORD                      pdwIAT;
	PDWORD                      pdwINT;
	PIMAGE_IMPORT_DESCRIPTOR    pImportDescriptor;
	PIMAGE_IMPORT_BY_NAME       pImportName;
	PIMAGE_OPTIONAL_HEADER      pOptionalHeader;
	PIMAGE_NT_HEADERS           pPeHeader;
	PSTR                        strCurrent;
	hEng = GetModuleHandleA("Engine.dll");
	void *offset;
  		
	pDosHeader = PIMAGE_DOS_HEADER(hEng);
	dwOffset = pDosHeader->e_lfanew;
	pPeHeader = PIMAGE_NT_HEADERS(long(hEng) + dwOffset);
	pOptionalHeader = &pPeHeader->OptionalHeader;
	pDataDirectory = pOptionalHeader->DataDirectory;
	dwOffset = pDataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	pImportDescriptor = PIMAGE_IMPORT_DESCRIPTOR(long(hEng) + dwOffset);
	for(dwIndex = 0; true; dwIndex++)
	{
		dwOffset = pImportDescriptor[dwIndex].Name;
		strCurrent = PSTR(long(hEng) + dwOffset);
		if(stricmp(strCurrent, strDllName) == 0) break;
	} 
	dwOffset = pImportDescriptor[dwIndex].FirstThunk;
	pdwIAT = PDWORD(long(hEng) + dwOffset);
	dwOffset = pImportDescriptor[dwIndex].OriginalFirstThunk;
	pdwINT = PDWORD(long(hEng) + dwOffset);
	for(dwIndex = 0; true; dwIndex++)
	{
		dwOffset = pdwINT[dwIndex];
		pImportName = PIMAGE_IMPORT_BY_NAME(long(hEng) + dwOffset);
		strCurrent = PSTR(pImportName->Name);
    			if(stricmp(strCurrent, strFunctionName) == 0) break;
	}
	VirtualProtect(&pdwIAT[dwIndex], sizeof(DWORD), PAGE_READWRITE, &dwBackup);
	offset = (void*)pdwIAT[dwIndex];
	pdwIAT[dwIndex] = PtrToUlong(newFuncAddy);
	VirtualProtect(&pdwIAT[dwIndex], sizeof(DWORD), dwBackup, &dwOffset);
	return offset;
}

//=====================================================================================
// Dll entry:
//=====================================================================================

BOOL APIENTRY DllMain(HMODULE hDll, DWORD reason, PVOID lpReserved)
{
		if(reason == DLL_PROCESS_ATTACH)
		{
			DisableThreadLibraryCalls(hDll);
			oProcessEvent = ReDirectFunction("Core.dll", "?ProcessEvent@UObject@@UAEXPAVUFunction@@PAX1@Z", (DWORD)&xProcessEvent);
		}
		return TRUE;
}