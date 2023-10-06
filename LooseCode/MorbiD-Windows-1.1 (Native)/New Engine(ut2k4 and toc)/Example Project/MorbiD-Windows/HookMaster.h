//=====================================================
//
//Morb!D-Windows 1.1
//
//=====================================================

#pragma once

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <Winuser.h>

#define ENGINE_API DLL_IMPORT

#include "../Core/Inc/Core.h"				// Unreal core public header file
#include "../Engine/Inc/UnObj.h"				// Standard object definitions.
#include "../Engine/Inc/UnPrim.h"				// Primitive class.
#include "../Engine/Inc/UnModel.h"			// Model class.
#include "../Engine/Inc/UnTex.h"				// Texture and palette.
#include "../Engine/Inc/UnAnim.h"
#include "../Engine/Inc/EngineClasses.h"		// All actor classes.
#include "../Engine/Inc/UnReach.h"			// Reach specs.
#include "../Engine/Inc/UnURL.h"				// Uniform resource locators.
#include "../Engine/Inc/UnLevel.h"			// Level object.
#include "../Engine/Inc/UnIn.h"				// Input system.
#include "../Engine/Inc/UnPlayer.h"			// Player class.
#include "../Engine/Inc/UnEngine.h"			// Unreal engine.
#include "../Engine/Inc/UnGame.h"				// Unreal game engine.
#include "../Engine/Inc/UnCamera.h"			// Viewport subsystem.
#include "../Engine/Inc/UnMesh.h"				// Mesh objects.

#include "Morbid-Windows.h"

extern APlayerController*	PC;
extern APawn*				Me;

extern FVector				MyCameraLocation;
extern FRotator				MyCameraRotation;


struct Event_PostRender_Parms
{
	UCanvas* Canvas;
};

struct Event_PlayerTick_Parms
{
	float DeltaTime;
};

struct Event_PlayerCalcView_Parms
{
		class AActor* ViewActor;
		FVector CameraLocation;
		FRotator CameraRotation;
};
