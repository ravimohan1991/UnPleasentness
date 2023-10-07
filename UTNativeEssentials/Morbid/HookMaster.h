//===================================================================================
//
// Morb!D Windows example source
//
// based on the daru radar source (because i was too lazy to write a new one hehe;) )
//
//====================================================================================

#pragma once

#include <windows.h>
#include <winbase.h>
#include <math.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void * _AddressOfReturnAddress(void);
EXTERNC void * _ReturnAddress(void);

#pragma intrinsic(_AddressOfReturnAddress)
#pragma intrinsic(_ReturnAddress)


#define ENGINE_API DLL_IMPORT

#include "Core.h"				// Unreal core public header file
#include "UnObj.h"				// Standard object definitions.
#include "UnPrim.h"				// Primitive class.
#include "UnModel.h"			// Model class.
#include "UnTexFmt.h"
#include "UnColor.h"
#include "UnTex.h"				// Texture and palette.
#include "UnAnim.h"
#include "EngineClasses.h"		// All actor classes.
#include "UnReach.h"			// Reach specs.
#include "UnURL.h"				// Uniform resource locators.
#include "UnLevel.h"			// Level object.
#include "UnIn.h"				// Input system.
#include "UnPlayer.h"			// Player class.
#include "UnEngine.h"			// Unreal engine.
#include "UnGame.h"				// Unreal game engine.
#include "UnCamera.h"			// Viewport subsystem.
#include "UnMesh.h"				// Mesh objects.
#include "UnSkeletalMesh.h"		// Skeletal model objects.
#include "UnActor.h"			// Actor inlines.
#include "UnAudio.h"			// Audio code.
#include "UnDynBsp.h"			// Dynamic Bsp objects.
#include "UnScrTex.h"			// Scripted textures.
#include "UnRenderIterator.h"	// Enhanced Actor Render Interface
#include "UnRenderIteratorSupport.h"
#include "UnRender.h"
#include "UnCon.h"

#include "MorbiD-Windows.h"

extern APlayerPawn*  Me;
extern FRotator		 MyCameraRotation;
extern FVector		 MyCameraLocation;
extern APawn*		 MyCameraActorPawn;

extern bool		bHook;

extern bool		b2DRadar;
extern bool		bHealthBars;

extern bool bInitHook;


extern void inline Cmd (TCHAR* S);
extern bool inline ValidTarget (APawn* Target);
extern void* DetourJump (void *orig, void *dest);
extern void PrePlayerInput (FLOAT DeltaTime);
extern void WINAPI xProcessEvent (class UFunction* Function, void* Parms, void* Result);
extern void HookFunctions (void);
