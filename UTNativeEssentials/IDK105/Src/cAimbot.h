#pragma once
#ifndef _AIM_
#define _AIM_

class cAimbot : public HookClass
{
protected:
	FVector AimHeight (AActor* Target);
	FVector PingCorrection(APawn* Target);
	FVector BulletSpeedCorrection (APawn *Player);
	int GetWeaponFireMode();
	int GetWeaponRange (void);
	FVector MuzzleCorrection (float Time);
public:
	cAimbot();
	~cAimbot();
	EWEAPON inline GetWeaponID (void);
	void FireMyWeapon (void);
	void StopMyWeapon (void);
	bool IsGoodWeapon (void);
	void SetMyRotation(UCanvas* Canvas, APawn* Target);
	APawn* GetBestTarget (UCanvas * Canvas,APawn* BestTarget, APawn* Target);
	bool IsVisible (APawn* Target);
	bool inline IsInRange (APawn* Target);
	bool GoodAim ();
	void Trigger (APawn* Target);
};

cAimbot::cAimbot() {};
cAimbot::~cAimbot(){};

#endif _AIM_

EWEAPON inline cAimbot::GetWeaponID (void)
{
	if ( Me->Weapon != NULL )
	{
		FString Name = Me->Weapon->GetClass()->GetName();

		if (!Name)								return WP_None;
		if ( Name == TEXT("Translocator") )		return WP_TransLocator;
		if ( Name == TEXT("ChainSaw") )			return WP_Chainsaw;
		if ( Name == TEXT("enforcer") )			return WP_Enforcer;
		if ( Name == TEXT("doubleenforcer") )	return WP_DoubleEnforcer;
		if ( Name == TEXT("ImpactHammer") )		return WP_ImpactHammer;
		if ( Name == TEXT("PulseGun") )			return WP_PulseRifle;
		if ( Name == TEXT("ripper") )			return WP_Ripper;
		if ( Name == TEXT("ShockRifle") )		return WP_ShockRifle;
		if ( Name == TEXT("minigun2") )			return WP_Minigun;
		if ( Name == TEXT("SuperShockRifle") )	return WP_ShockRifle;
		if ( Name == TEXT("SniperRifle") )		return WP_SniperRifle;
		if ( Name == TEXT("ut_biorifle") )		return WP_BioRifle;
		if ( Name == TEXT("UT_Eightball") )		return WP_RocketLauncher;
		if ( Name == TEXT("UT_FlakCannon") )	return WP_FlakCannon;
		if ( Name == TEXT("WarheadLauncher") )	return WP_Redeemer;

		return WP_Unknown;
	}

	return WP_None;
}

bool inline cAimbot::IsInRange (APawn* Target)
{
	float Range = (Target->Location - Me->Weapon->Location).Size() / 48;

	switch ( MyWeapon )
	{
		case WP_Redeemer:
		case WP_TransLocator:
			return false;
			break;

		case WP_ImpactHammer:
		case WP_Chainsaw:
			return Range < 10;
			break;

		case WP_BioRifle:
			return Range < 15;
			break;

		case WP_RocketLauncher:
		case WP_FlakCannon:
		case WP_PulseRifle:
			return Range < 20;
			break;

		case WP_Ripper:
			return Range < 25;
			break;

		case WP_Enforcer:
		case WP_DoubleEnforcer:
			return Range < 45;
			break;
	}

	return true;
}

FVector cAimbot::BulletSpeedCorrection (APawn *Player)
{
	float BulletSpeed = 0;
	FVector Correction;

	if (Me->Weapon)
	{
		if (Me->bFire)
		{
			BulletSpeed = Me->Weapon->ProjectileSpeed;
		}
		if ( BulletSpeed > 0 )
		{
			if(BulletSpeed == 1000) BulletSpeed = 20000;
			Correction=Player->Velocity * (Player->Location - Me->Location).Size() / BulletSpeed;
			Correction.Z=0.00;
			return Correction;
		}
		else
		{
			return FVector(0,0,0);
		}
	}
	return FVector(0,0,0);
}

FVector cAimbot::MuzzleCorrection (float Time)
{
	FVector Corr;

	Corr = Me->Location + FVector(0,0,30);
	Corr += Me->Velocity * Time + Me->Acceleration * 0.5 * Square(Time);

	return Corr;
}

FVector cAimbot::AimHeight (AActor* Target)
{
    FVector HisPos = Target->Location;

	if ( MyWeapon == WP_RocketLauncher )
	{
	    HisPos.Z -= 23;
	}
	else
	{
        if ( FString(*Target->AnimSequence).InStr(TEXT("Duck")) != -1 )
	    {
		    HisPos.Z += Target->CollisionHeight * 0.2;
	    }

	    else if ( FString(*Target->AnimSequence).InStr(TEXT("Swim")) != -1 )
	    {
		    HisPos.Z += Target->CollisionHeight * 0.15;
	    }

	    else if ( FString(*Target->AnimSequence).InStr(TEXT("Jump")) != -1 )
	    {
		    HisPos.Z += Target->CollisionHeight * 0.765;
	    }

		else
		{
            HisPos.Z += Target->CollisionHeight * 0.7;
		}
	}
	return HisPos;
}

FVector cAimbot::PingCorrection(APawn* Target) 
{ 
    FVector V = FVector(0,0,0);
	if ( Me->Role < ROLE_Authority)
	{
		V = Target->Velocity * Me->PlayerReplicationInfo->Ping / 1000;
		V += Target->Velocity * Deltatime;
		V -= Me->Velocity * Deltatime;
	}
	return V;
}


void cAimbot::FireMyWeapon (void)
{
    bBotShooting = true;
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_RIGHTUP,0,0,0,0);
}

void cAimbot::StopMyWeapon (void)
{
    if ( bBotShooting )
	{
	    bBotShooting = false;
		mouse_event(MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_LEFTUP,0,0,0,0);
	}
}

struct Ammo_Parms
{
	BITFIELD Mode;
	int returnvalue;
};

bool inline cAimbot::IsGoodWeapon (void)
{
	static Ammo_Parms AmmoParms;
	UFunction* AmmoFunction;

	AmmoParms.Mode = 0;

	if ( Me->Weapon != NULL )
	{
		AmmoFunction = Me->Weapon->FindFunction(TEXT("GetRemainingBullets"));

		if ( AmmoFunction != NULL )
			Me->Weapon->ProcessEvent(AmmoFunction, &AmmoParms);

		if ( Me->Weapon->AmmoType != NULL )
			return (Me->Weapon->AmmoType->AmmoAmount > 0) || (Me->Weapon->AmmoType->MaxAmmo <= 0);

		else
		{
			if (AmmoParms.returnvalue == 0)
				return false;

			return true;
		}
	}

	return false;
}

#define Normalize(Rot) \
	while (Rot.Yaw > 32768) Rot.Yaw -= 65536; \
	while (Rot.Yaw < -32768) Rot.Yaw += 65536; \
	while (Rot.Pitch > 32768) Rot.Pitch -= 65536; \
	while (Rot.Pitch < -32768) Rot.Pitch += 65536; \

void cAimbot::SetMyRotation(UCanvas* Canvas, APawn* Target)
{
    FVector MyPos, HisPos;
	FRotator Rotation;

	HisPos = AimHeight(Target);
	HisPos += BulletSpeedCorrection(Target) + PingCorrection(Target);

	FVector MyLocation = MuzzleCorrection(Deltatime);
	BestRotation = (HisPos - MyLocation).Rotation();
	Normalize(BestRotation);

	float MouseSensitivity = Me->MouseSensitivity;
	float SmoothX=Me->SmoothMouseX;
	float SmoothY=Me->SmoothMouseY;

	Me->SmoothMouseX=0.0;
	Me->SmoothMouseY=0.0;
	Me->MouseSensitivity=0.0;

	Me->MouseSensitivity = MouseSensitivity;
	Me->SmoothMouseX=SmoothX;
	Me->SmoothMouseY=SmoothY;

	struct ClientSetLocation_Parms
	{
		FVector Location;
		FRotator Rotation;
	};

	ClientSetLocation_Parms Parms;

	Parms.Location = Me->Location;
	Parms.Rotation = BestRotation;

	UFunction* F = Me->FindFunction(TEXT("ClientSetLocation"));

	if (F != NULL)
	{
		Me->ProcessEvent(F,&Parms);
	}
}

APawn* cAimbot::GetBestTarget (UCanvas * Canvas,APawn* BestTarget, APawn* Target)
{
    if ( BestTarget == NULL )
	{
	    return Target;
	}
	else
	{
	    if ( (Target->Location - Me->Location).Size() < (BestTarget->Location - Me->Location).Size() )
		{
		    return Target;
		}
		else
		{
		    return BestTarget;
		}
	}
}

bool cAimbot::IsVisible (APawn* Target)
{
    FVector MyPos, HisPos;

	MyPos = Me->Location;
	MyPos.Z += Me->BaseEyeHeight;

	HisPos = Target->Location;
	HisPos.Z += Target->CollisionHeight * 0.73;
	HisPos += PingCorrection(Target);

	return ( Me->XLevel->Model->FastLineCheck(HisPos, MyPos) > 0 );
}

bool cAimbot::GoodAim ()
{
    if(bAutoAim == 2) 
	{
	    return (GetAsyncKeyState(VK_LBUTTON) < 0 || GetAsyncKeyState(VK_RBUTTON) < 0);
	}
	else if(bAutoAim == 1) return true;

	return false;
}

DWORD wynik2;
void cAimbot::Trigger (APawn* Target)
{
	if (!bTrigger) return;

	FCheckResult R;
	FVector X, Y, Z, Start,Dir,HisPos;

	GetAxes(MyCameraRotation,X,Y,Z);
	Start = Me->Location + Me->EyeHeight * Z;
	Dir = X * Me->CollisionRadius;

	Me->XLevel->TraceVisible(Dir, R, Me, Start, TRACE_Pawns | TRACE_Level, 10000);

	APawn* hitPawn = Cast<APawn>(R.Actor);

	if (hitPawn && hitPawn == Target && IsEnemy(Target))
	{
		CreateThread(0, 0, MouseDown, 0, 0, &wynik2);
	}
}