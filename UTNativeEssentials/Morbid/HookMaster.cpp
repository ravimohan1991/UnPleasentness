//===================================================================================
//
// Morb!D Windows example source
//
// based on the daru radar source (because i was too lazy to write a new one hehe;) )
//
//====================================================================================

#include "HookMaster.h"

APlayerPawn* Me;
FRotator	 MyCameraRotation;
FVector		 MyCameraLocation;
APawn*		 MyCameraActorPawn;

bool		bHook			= true;
bool		b2DRadar		= false;
bool		bHealthBars		= true;

bool inline ValidRender (UCanvas* Canvas)
{
	return	(Canvas != NULL) &&
			(Canvas->Viewport != NULL) &&
			(Canvas->Viewport->Actor != NULL) &&
			(Canvas->Viewport->Actor->PlayerReplicationInfo != NULL) &&
			(Canvas->Viewport->Actor->XLevel != NULL);
}

void* DetourJump (void *orig, void *dest)
{
	DWORD oldprot;
	VirtualProtect(orig, 5, PAGE_EXECUTE_READWRITE, &oldprot);

	DWORD orgJmp = DWORD(*((void **) ((unsigned int) orig + 1))) + 5 + (unsigned int) orig;

	*((unsigned char *) orig ) = 0xE9; // 0xE9 = Jmp // 0xE8 = Call
	*((void **) ((unsigned int) orig + 1)) = (void *) (((unsigned int) dest) - (((unsigned int) orig) + 5));

	VirtualProtect(orig, 5, oldprot, &oldprot);

	return (void*)orgJmp;
}
//##########################
//The example function
//##########################

void MyWindows(UCanvas * Canvas)
{
	//define the used windows
	MorbidWindow Window1;
	MorbidWindow Window2;
	MorbidWindow Window3;
	MorbidWindow Window4;

	//drawwindow
	MWindow.DrawWindow(Window1,0,0,Canvas->ClipX,80,2,true,NULL);
	//Drawtext
	MWindow.MakeShadowText(20,20,TEXT("Morb!D-Windows 1.0"),Color.Red(),BigFont);
	MWindow.MakeShadowText(200,55,TEXT("This is some Example ShadowText...."),Color.White(),MediumFont);
	MWindow.MakeShadowText(500,30,TEXT("This is some Example ShadowText in SmallFont...."),Color.White(),SmallFont);

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

void inline ExtendedPostRender (UCanvas* Canvas)
{
	if(bMenuVisible)
	{
		MyWindows(Canvas);
	}
}


bool bValidRender = false;

void inline MyPostRender (UCanvas* Canvas)
{
	FColor	OldCColor;

	MWindow.MorbidWindowsMain(Canvas);

	if ( !ValidRender(Canvas) )
	{
		bValidRender = false;
		return;
	}
	else
	{
		bValidRender = true;
	}

	OldCColor	= Canvas->Color;
	Me			= Canvas->Viewport->Actor;

	ExtendedPostRender( Canvas );

	Canvas->Color = OldCColor;
}


void PrePlayerInput (FLOAT DeltaTime)
{
	if (Me == NULL) return;
	
	Mouse.UpdateMouse(DeltaTime);
}

void inline PreProcessEvent (class UFunction* Function, void* Parms)
{
	if (wcsicmp(Function->GetName(), L"PlayerInput") == 0)
	{
		if ( Function->GetOwnerClass()->IsChildOf(APlayerPawn::StaticClass()) )
		{
			PrePlayerInput(((APlayerPawn_eventPlayerInput_Parms*)Parms)->DeltaTime);
		}
	}
}

void inline PostProcessEvent (class UFunction* Function, void* Parms)
{
	if (wcsicmp(Function->GetName(), L"PostRender") == 0)
	{
		if ( Function->GetOwnerClass()->IsChildOf(APlayerPawn::StaticClass()) )
		{
			MyPostRender(((APlayerPawn_eventPostRender_Parms*)Parms)->Canvas);
		}
	}

	if (wcsicmp(Function->GetName(), L"PlayerCalcView") == 0)
	{
		if ( Function->GetOwnerClass()->IsChildOf(APlayerPawn::StaticClass()) )
		{
			MyCameraLocation  = ((APlayerPawn_eventPlayerCalcView_Parms*)Parms)->CameraLocation;
			MyCameraRotation  = ((APlayerPawn_eventPlayerCalcView_Parms*)Parms)->CameraRotation;
			MyCameraActorPawn = (APawn*)((APlayerPawn_eventPlayerCalcView_Parms*)Parms)->ViewActor;
		}
	}
}

void *pProcessEvent;

void WINAPI xProcessEvent (class UFunction* Function, void* Parms, void* Result=NULL)
{
	__asm pushad

	PreProcessEvent( Function, Parms );

	__asm popad

	_asm
	{
		push Result
		push Parms
		push Function
		call pProcessEvent
	}

	__asm pushad

	PostProcessEvent( Function, Parms );

	__asm popad
}

void HookFunctions (void)
{
	HMODULE hCore = GetModuleHandleA("Core.dll");

	if ( hCore != NULL )
	{
		void *pAddress = (void*)GetProcAddress(hCore, "?ProcessEvent@UObject@@UAEXPAVUFunction@@PAX1@Z");

		if ( pAddress != NULL )
		{
			pProcessEvent = DetourJump(pAddress, xProcessEvent);
		}
	}
}

BOOL APIENTRY DllMain (HMODULE hDll, DWORD reason, PVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
  	{
		DisableThreadLibraryCalls(hDll);
		HookFunctions();
	}

	return TRUE;
}