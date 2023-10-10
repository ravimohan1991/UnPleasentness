#pragma once
#ifndef _MAIN_
#define _MAIN_

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "detours.h"

#define ENGINE_API __declspec(dllimport) 

//
// A specific pixel encoding.
//
// All texture data is encoded as little endian.
//
enum ETextureFormat
{
	// Original.
	TEXF_P8 = 0x00,
	TEXF_BGRA8_LM = 0x01, // Highest bit is ignored. LightMaps: 0..127 maps to 0.0..2.0, alpha is ignored. FogMaps: 0..127 maps to 0.0..1.0. Prior: TEXF_RGBA7.
	TEXF_R5G6B5 = 0x02, // Prior: TEXF_RGB16. !! Conflicted with naming scheme.
	TEXF_BC1 = 0x03, // Solid alpha variant. See TEXF_BC1_PA for punchthrough alpha variant.
	TEXF_RGB8 = 0x04,
	TEXF_BGRA8 = 0x05, // Prior: TEXF_RGBA8. !! Conflicted with naming scheme.

	// S3TC (continued).
	TEXF_BC2 = 0x06,
	TEXF_BC3 = 0x07,

	// RGTC.
	TEXF_BC4 = 0x08,
	TEXF_BC4_S = 0x09,
	TEXF_BC5 = 0x0a,
	TEXF_BC5_S = 0x0b,

	// BPTC.
	TEXF_BC7 = 0x0c,
	TEXF_BC6H_S = 0x0d,
	TEXF_BC6H = 0x0e,

	// Normalized RGBA.
	TEXF_RGBA16 = 0x0f,
	TEXF_RGBA16_S = 0x10,
	TEXF_RGBA32 = 0x11,
	TEXF_RGBA32_S = 0x12,

	// Meta.
	TEXF_NODATA = 0x13,
	TEXF_UNCOMPRESSED = 0x14, // Default quality uncompressed meta target.
	TEXF_UNCOMPRESSED_LOW = 0x15, // Low quality uncompressed meta target.
	TEXF_UNCOMPRESSED_HIGH = 0x16, // High quality uncompressed meta target.
	TEXF_COMPRESSED = 0x17, // Default quality compressed meta target.
	TEXF_COMPRESSED_LOW = 0x18, // Low quality compressed meta target.
	TEXF_COMPRESSED_HIGH = 0x19, // High quality compressed meta target.

	// S3TC (continued).
	TEXF_BC1_PA = 0x1a, // Punchthrough alpha variant. See TEXF_BC1 for solid alpha variant.

	// Normalized RGBA (continued).
	TEXF_R8 = 0x1b,
	TEXF_R8_S = 0x1c,
	TEXF_R16 = 0x1d,
	TEXF_R16_S = 0x1e,
	TEXF_R32 = 0x1f,
	TEXF_R32_S = 0x20,
	TEXF_RG8 = 0x21,
	TEXF_RG8_S = 0x22,
	TEXF_RG16 = 0x23,
	TEXF_RG16_S = 0x24,
	TEXF_RG32 = 0x25,
	TEXF_RG32_S = 0x26,
	TEXF_RGB8_S = 0x27,
#if _REALLY_WANT_TEXF_RGB16
	TEXF_RGB16 = 0x28, // !! This is not the old TEXF_RGB16 which was 16 bits/color instead of 48 bits/color.
#endif
	TEXF_RGB16_ = 0x28, // Snake'd.
	TEXF_RGB16_S = 0x29,
	TEXF_RGB32 = 0x2a,
	TEXF_RGB32_S = 0x2b,
#if _REALLY_WANT_TEXF_RGBA8
	TEXF_RGBA8 = 0x2c, // !! This is not the old TEXF_RGBA8 which was BGRA instead of RGBA data.
#endif
	TEXF_RGBA8_ = 0x2c, // Snake'd.
	TEXF_RGBA8_S = 0x2d,

	// Floating point RGBA.
	TEXF_R16_F = 0x2e,
	TEXF_R32_F = 0x2f,
	TEXF_RG16_F = 0x30,
	TEXF_RG32_F = 0x31,
	TEXF_RGB16_F = 0x32,
	TEXF_RGB32_F = 0x33,
	TEXF_RGBA16_F = 0x34,
	TEXF_RGBA32_F = 0x35,

	// ETC1/ETC2/EAC.
	TEXF_ETC1 = 0x36,
	TEXF_ETC2 = 0x37,
	TEXF_ETC2_PA = 0x38,
	TEXF_ETC2_RGB_EAC_A = 0x39,
	TEXF_EAC_R = 0x40,
	TEXF_EAC_R_S = 0x41,
	TEXF_EAC_RG = 0x42,
	TEXF_EAC_RG_S = 0x43,

	// ASTC.
	TEXF_ASTC_4x4 = 0x44, //
	TEXF_ASTC_5x4 = 0x45, // ASTC can store an
	TEXF_ASTC_5x5 = 0x46, // HDR alpha channel.
	TEXF_ASTC_6x5 = 0x47, //
	TEXF_ASTC_6x6 = 0x48,
	TEXF_ASTC_8x5 = 0x49,
	TEXF_ASTC_8x6 = 0x4a,
	TEXF_ASTC_8x8 = 0x4b,
	TEXF_ASTC_10x5 = 0x4c,
	TEXF_ASTC_10x6 = 0x4d,
	TEXF_ASTC_10x8 = 0x4e,
	TEXF_ASTC_10x10 = 0x4f,
	TEXF_ASTC_12x10 = 0x50,
	TEXF_ASTC_12x12 = 0x51,
	TEXF_ASTC_3x3x3 = 0x52, //
	TEXF_ASTC_4x3x3 = 0x53, // Block can has
	TEXF_ASTC_4x4x3 = 0x54, // three deezs.
	TEXF_ASTC_4x4x4 = 0x55, //
	TEXF_ASTC_5x4x4 = 0x56,
	TEXF_ASTC_5x5x4 = 0x57,
	TEXF_ASTC_5x5x5 = 0x58,
	TEXF_ASTC_6x5x5 = 0x59,
	TEXF_ASTC_6x6x5 = 0x60,
	TEXF_ASTC_6x6x6 = 0x6a,

	// PVRTC.
	TEXF_PVRTC1_2BPP = 0x6b, // Could use better names, maybe
	TEXF_PVRTC1_4BPP = 0x6c, // something about the block size
	TEXF_PVRTC2_2BPP = 0x6d, // but I have no real clue about
	TEXF_PVRTC2_4BPP = 0x6e, // these formats. --han

	// RGBA (Integral).
	TEXF_R8_UI = 0x6f,
	TEXF_R8_I = 0x70,
	TEXF_R16_UI = 0x71,
	TEXF_R16_I = 0x72,
	TEXF_R32_UI = 0x73,
	TEXF_R32_I = 0x74,
	TEXF_RG8_UI = 0x75,
	TEXF_RG8_I = 0x76,
	TEXF_RG16_UI = 0x77,
	TEXF_RG16_I = 0x78,
	TEXF_RG32_UI = 0x79,
	TEXF_RG32_I = 0x7a,
	TEXF_RGB8_UI = 0x7b,
	TEXF_RGB8_I = 0x7c,
	TEXF_RGB16_UI = 0x7d,
	TEXF_RGB16_I = 0x7e,
	TEXF_RGB32_UI = 0x7f,
	TEXF_RGB32_I = 0x80,
	TEXF_RGBA8_UI = 0x81,
	TEXF_RGBA8_I = 0x82,
	TEXF_RGBA16_UI = 0x83,
	TEXF_RGBA16_I = 0x84,
	TEXF_RGBA32_UI = 0x85,
	TEXF_RGBA32_I = 0x86,

	// Special.
	TEXF_ARGB8 = 0x87,
	TEXF_ABGR8 = 0x88,
	TEXF_RGB10A2 = 0x89,
	TEXF_RGB10A2_UI = 0x8a,
	TEXF_RGB10A2_LM = 0x8b, // Highest bit and alpha is ignored. Otherwise same old song as TEXF_BGRA8_LM: 0..1023 maps to 0.0..2.0.
	TEXF_RGB9E5 = 0x8c, // No sign bit, individual 9 bit mantissa with a shared 5 bit exponent.
	TEXF_P8_RGB9E5 = 0x8d, // Palette for RGB9E5 data. Intended as an option to store HDR data for FireTexture.
	TEXF_R1 = 0x8e, // Unsigned normalized red format stored as 8x1 blocks taking 1 byte each. Upper left pixel is stored in least significant bit.
	TEXF_RGB10A2_S = 0x8f,
	TEXF_RGB10A2_I = 0x90,
	TEXF_R11G11B10_F = 0x91,

	// Normalized BGR.
	TEXF_B5G6R5 = 0x92, // Mostly just for specifc file formats,
	TEXF_BGR8 = 0x93, // which store their data in BGR order.

	// Double precission floating point RGBA.
	TEXF_R64_F = 0x94,
	TEXF_RG64_F = 0x95,
	TEXF_RGB64_F = 0x96,
	TEXF_RGBA64_F = 0x97,

	//
	// Idea is to set this to the last defined format the
	// code here is valid, so it can be used to filter out
	// any (at this point) unknown format in binaries.
	//
	TEXF_LAST = TEXF_RGBA64_F,

	// Max.
	TEXF_MAX = 0xff,
};

#include "Core.h"
#include "UnObj.h"
#include "UnPrim.h"
#include "UnModel.h"
#include "UnColor.h"
#include "UnTex.h"
#include "UnAnim.h"
#include "EngineClasses.h"
#include "UnReach.h"
#include "UnURL.h"
#include "UnLevel.h"
#include "UnIn.h"
#include "UnPlayer.h"
#include "UnEngine.h"
#include "UnGame.h"
#include "UnCamera.h"
#include "UnMesh.h"
#include "UnSkeletalMesh.h"
#include "UnActor.h"
#include "UnAudio.h"
#include "UnDynBsp.h"
#include "UnScrTex.h"
#include "UnRenderIterator.h"
#include "UnRenderIteratorSupport.h"
#include "UnRender.h"
#include "UnCon.h"

//#include "cDynamicFont.h"

int ReDirectFunction(const char* strDllName, const char* strFunctionName, DWORD newFuncAddy);
void log_add(const char* fmt, ...); //logging
void pPostRender();
void pTick();
void pPreRender();

class FVector	MyCameraLocation	= FVector(0,0,0);
class FRotator	MyCameraRotation	= FRotator(0,0,0);

class APlayerPawn*	        Me			= NULL;

static class UTexture*		WhiteTexture    = NULL;
static class UFont*			SmallFont = NULL;

UCanvas* MyCanvas = NULL;
float LastTimerCheck = 0.0;
float Deltatime = 0.0;

enum EWEAPON
{
	WP_None,
	WP_TransLocator,
	WP_ImpactHammer,
	WP_Chainsaw,
	WP_Enforcer,
	WP_DoubleEnforcer,
	WP_BioRifle,
	WP_ShockRifle,
	WP_PulseRifle,
	WP_Ripper,
	WP_Minigun,
	WP_FlakCannon,
	WP_RocketLauncher,
	WP_SniperRifle,
	WP_Redeemer,
	WP_Unknown
};
EWEAPON			MyWeapon		= WP_None;

/**
 * @breif In game configurables and switches for tweaking
 */
bool bHook    = true;
bool b3DRadar = true;
bool b2DRadar = true;
bool bHealthbars = true;
bool bTrigger = true;
bool bInfo    = true;
bool bSettings= true;
bool bControls = true;
bool bBotShooting = false;
bool bValidAim	= false;
bool bAutoFire	= true;
int  bAutoAim	= 0;
float Scale = 1.0f;

/**
 * @brief Global Variables
 */
FRotator BestRotation = FRotator(0,0,0);
FVector	 BestLocation = FVector (0,0,0);

bool block = false;
bool bInit;

class HookClass
{
public:
	HookClass();
	~HookClass();

class cRadar * cR;
class cMenu  * cM;
class cAimbot* cA;
class cSecure* cS;
};

HookClass::HookClass()  { }
HookClass::~HookClass() { }

HookClass Hook; 

#define SetPos(PosX,PosY) \
	Canvas->CurX = PosX;  \
	Canvas->CurY = PosY;  \

#define LoadTexture(Texture,Name) \
if (Texture == NULL) \
{ \
	Texture = (UTexture*)UTexture::StaticLoadObject(UTexture::StaticClass(), NULL, TEXT(Name), NULL, LOAD_NoWarn, NULL); \
	if (Texture != NULL) Texture->SetFlags(RF_Keep); \
} \

#define LoadFont(Font,Name) \
if (Font == NULL) \
{ \
	Font = (UFont*)UFont::StaticLoadObject(UFont::StaticClass(), NULL, TEXT(Name), NULL, LOAD_NoWarn, NULL); \
	if (Font != NULL) Font->SetFlags(RF_Keep); \
} \

//static class UFont* SmallFont = NULL;

#define ValidRender(Canvas) ( \
	(Canvas != NULL) && \
	(Canvas->SmallFont != NULL) && \
	(Canvas->Viewport != NULL) ) && \
	(Canvas->Viewport->Actor != NULL) && \
	(Canvas->Viewport->Actor->PlayerReplicationInfo != NULL) && \
	(Canvas->Viewport->Actor->XLevel != NULL) \

#define DefPawn		(UClass*)UClass::StaticLoadClass(AActor::StaticClass(), NULL, TEXT("Engine.Pawn"),		NULL, LOAD_NoFail, NULL)
template<class T> class AObjectIterator : public FObjectIterator
{
	public:
		AObjectIterator(UClass* InClass = T::StaticClass())
			:	FObjectIterator(InClass)
		{}

		T* operator  * ()
		{
			return (T*)FObjectIterator::operator  *();
		}

		T* operator -> ()
		{
			return (T*)FObjectIterator::operator ->();
		}
};

bool ValidTarget (APawn* Target)
{
    if(Target == NULL)return false;
	if(Target == Me)return false;
	if(Target->bHidden && Target->Health <= 0){return false;}
	if(Target->bDeleteMe)return false;
	if(Target->Level != Me->Level)return false;
	if(Target->PlayerReplicationInfo == NULL)return false;
	if(Target->PlayerReplicationInfo->bIsSpectator) return false;
	if(Target->PlayerReplicationInfo->bWaitingPlayer) return false;
	return true;
}

float inline Dot (const FVector& V1, const FVector& V2)
{
	return (V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z);
};

void GetAxes (FRotator R, FVector &X, FVector &Y, FVector &Z)
{
	X = R.Vector();
	X.Normalize();

	R.Yaw += 16384;

	FRotator R2 = R;
	R2.Pitch = 0.f;
	
	Y = R2.Vector();
	
	//Y.Z = 0.f;
	Y.Normalize();
	
	R.Yaw -= 16384;
	R.Pitch += 16384;
	
	Z = R.Vector();
	
	//Z.Z = 0.f;
	Z.Normalize();
}

bool IsEnemy (APawn* Target)
{
    if(!Me->GameReplicationInfo->bTeamGame) return true;
    return (Target->PlayerReplicationInfo->Team != Me->PlayerReplicationInfo->Team);
}

FColor GetTeamColor (APawn* Target)
{
    if(!Me->GameReplicationInfo->bTeamGame)return FColor(0,200,0);

	switch( Target->PlayerReplicationInfo->Team )
	{
	    case 0:
		     return FColor(200,0,0);
			 break;

		case 1:
			 return FColor(0,200,200);
			 break;

		default:
			 return FColor(0,200,0);
			 break;
	}
}

void DrawMyText(UCanvas* Canvas, FString Str, float PosX, float PosY, FColor Color, UFont* Font)
{
	FColor TempColor = Canvas->Color;
	Canvas->Color = FColor(0, 0, 0, 255);

	SetPos(PosX-1, PosY)
	Canvas->WrappedPrintf(Font, 0, L"%s", Str);

	SetPos(PosX+1, PosY)
	Canvas->WrappedPrintf(Font, 0, L"%s", Str);

	SetPos(PosX, PosY-1)
	Canvas->WrappedPrintf(Font, 0, L"%s", Str);

	SetPos(PosX, PosY+1)
	Canvas->WrappedPrintf(Font, 0, L"%s", Str);


	SetPos(PosX, PosY)
	Canvas->Color = Color;
	Canvas->WrappedPrintf(Font, 0, L"%s", Str);
	Canvas->Color = TempColor;
}

DWORD WINAPI MouseDown ( PVOID pParam )
{
	mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
	Sleep(70);
	mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	ExitThread(0);
	return 0;
}

DWORD WINAPI MouseUp ( PVOID pParam )
{
	mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	ExitThread(0);
	return 0;
}

void DrawRec(UCanvas* Canvas, float ScreenX, float ScreenY, float Width, float Height, UTexture* Tex, FColor Color)
{
	struct DrawRect_Parms
	{
		UTexture *Tex;
		float X;
		float Y;
	};

	UFunction* F = Canvas->FindFunction(L"DrawRect");

	if (F != NULL)
	{
		int TempStyle = Canvas->Style;
		FColor TempColor = Canvas->Color;

		DrawRect_Parms Parms;

		Parms.Tex	= Tex;
		Parms.X		= Width;
		Parms.Y		= Height;

		Canvas->CurX = ScreenX; 
		Canvas->CurY = ScreenY; 
		Canvas->Color = Color;
		Canvas->Style = 1;
		Canvas->ProcessEvent(F, &Parms);
		Canvas->Style = TempStyle;
		Canvas->Color = TempColor;
	}
}

bool doonce = false;
bool doonce1 = false;
bool doonce2 = false;

#endif _MAIN_