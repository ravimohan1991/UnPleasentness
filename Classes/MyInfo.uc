//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyInfo extends Info Config(elfLITE);

#exec Texture 	Import File=Textures\MyCross1.bmp 	GROUP=MyTextures 	Name=MyCross1 	Mips=off Flags=1
#exec Texture 	Import File=Textures\MyCross2.bmp 	GROUP=MyTextures 	Name=MyCross2 	Mips=off Flags=1
#exec Texture 	Import File=Textures\MyCross3.bmp 	GROUP=MyTextures 	Name=MyCross3	Mips=off Flags=1
#exec Texture 	Import File=Textures\MyCross4.bmp 	GROUP=MyTextures 	Name=MyCross4	Mips=off Flags=1
#exec Texture 	Import File=Textures\MyCross5.bmp 	GROUP=MyTextures 	Name=MyCross5	Mips=off Flags=1

#exec Texture 	Import File=Textures\MySkin0.bmp 	GROUP=MyTextures 	Name=MySkin0	Mips=off Flags=1
#exec Texture 	Import File=Textures\MySkin1.bmp 	GROUP=MyTextures 	Name=MySkin1	Mips=off Flags=1
#exec Texture 	Import File=Textures\MySkin2.bmp 	GROUP=MyTextures 	Name=MySkin2	Mips=off Flags=1
#exec Texture 	Import File=Textures\MySkin3.bmp 	GROUP=MyTextures 	Name=MySkin3	Mips=off Flags=1

//================================================================================
// STRUCT.
//================================================================================

struct TargetStruct
{
	var Pawn 	Target;
	var Vector 	TOffset;
	var bool 	TVisible;
	var bool 	TEnemy;
	var byte 	TFireMode;
	var int 	TWarning;
};

//================================================================================
// PROJECT.
//================================================================================

	var string Project;
	var string Version;
	var string Coder;
	var string Site;

//================================================================================
// SETTINGS.
//================================================================================

	var config bool MySetActive;
	var config int  MySetAim;
	var config bool MySetFire;
	var config bool MySetHeadShot;
	var config int  MySetAimHeight;
	var config bool MySetPingCorr;
	var config int  MySetInstantHit;
	var config bool MySetZoneCheck;
	var config int  MySetRadar;
	var config bool MySetGlow;
	var config bool MySetBullet;
	var config bool MySetSlow;
	var config int  MySetSlowSpeed;
	var config int  MySetAngle;
	var config bool MySetWarning;
	var config bool MySetSkin;
	var config bool MySetHostiles;
	var config bool MySetFriendlies;
	var config bool MySetHostileFlag;
	var config bool MySetFriendlyFlag;
	var config bool MySetAdminDisconnect;
	var config int  MySetShock;
	var config int  MySetWallHack;
	var config bool MySetTrigger;
	
	
//================================================================================
// FONTS.
//================================================================================	

	var Font MySmallFont;
	var Font MyMediumFont;
	var Font MyHugeFont;
	
	
//================================================================================
// COLORS.
//================================================================================

	var Color MyRedColor;
	var Color MyBlueColor;
	var Color MyGreenColor;
	var Color MyYellowColor;
	var Color MyWhiteColor;
	var Color MyPurpleColor;
	var Color MyDarkRedColor;
	var Color MyDarkBlueColor;
	var Color MyDarkGreenColor;
	var Color MyDarkYellowColor;
	
	
//================================================================================
// SKINS.
//================================================================================

	var Texture MyRedSkin;
	var Texture MyBlueSkin;
	var Texture MyGreenSkin;
	var Texture MyYellowSkin;
	
	
//================================================================================
// OTHER.
//================================================================================

	var PlayerPawn 		elf;
	var bool 		bShooting;
	var bool 		bShoot;
	var bool 		bHasLock;
	var float 		fZone;
	var MyInventory 	MyExecs;
	var UWindowRootWindow 	MyRoot;
	
	
//================================================================================
// CODE.
//================================================================================	

event Tick (float Delta)
{
	Super.Tick(Delta);
	MyTick(Delta);
}

function MyTick (float Delta)
{
	elf=GetPlayerPawn();

	if ( MySetActive && IsValidPlayerPawn(elf) )
	{
		fZone=Max(fZone - Delta,0);
		ProtectCoder();
		AttachExecs();
		AdminProtect();
	}
}

function MyCanvas (Canvas Canvas)
{
	local TargetStruct PawnTarget;
	local ShockProj ShockTarget;
	local int FireMode;

	elf=GetPlayerPawn();
	
	if ( MySetActive && IsValidPlayerPawn(elf) && IsValidCanvas(Canvas) )
	{
		SetFonts(Canvas);
		DrawBotHud(Canvas);
		NukeRelated(Canvas);
		FlagRelated(Canvas);
		ShockTarget=ShockRelated();
		PawnTarget=PawnRelated(Canvas);
		
		if ( ShockTarget != None )
		{
			bHasLock=True;
			FireMode=1;
			SetShockRotation(ShockTarget);
		} 
		else 
		{
			if ( (MySetShock < 2) && (PawnTarget.Target != None) )
			{
				bHasLock=True;
				FireMode=PawnTarget.TFireMode;
				SetPawnRotation(PawnTarget);
				
				if ( MySetAim < 2 )
				{
					bShoot=False;
				}
			} 
			else 
			{
				bHasLock=False;
			}
		}
		
		if ( bHasLock )
		{
			DrawLock(Canvas);
			if ( MySetFire && bShoot )
			{
				FireOn(FireMode);
			}
			bShoot=True;
		} 
		else
		{
			FireOff();
			bShoot=False;
			TriggerBot(Canvas);
		}
	}
}

function FireOff ()
{
	if ( bShooting )
	{
		elf.bFire=0;
		elf.bAltFire=0;
		bShooting=False;
	}
}

function FireOn (int FireMode)
{
	if ( (elf.Weapon != None) && elf.Weapon.IsA('ImpactHammer') )
	{
		return;
	}
	
	bShooting=True;
	
	if ( FireMode == 1 )
	{
		elf.bFire=1;
		elf.bAltFire=0;
		elf.Fire();
	}
	
	if ( FireMode == 2 )
	{
		elf.bFire=0;
		elf.bAltFire=1;
		elf.AltFire();
	}
	
	if ( (elf.Weapon != None) && elf.Weapon.IsA('UT_Eightball') )
	{
		FireOff();
	}
}

function TriggerBot (Canvas Canvas)
{
	local Vector V[4];
	local Pawn Target;

	if ( !MySetTrigger )
	{
		return;
	}
	
	V[0]=elf.Location;
	V[0].Z += elf.BaseEyeHeight;
	GetAxes(Normalize(elf.ViewRotation),V[1],V[2],V[3]);
	V[1]=V[0] + 10000 * V[1];
	Target=Pawn(elf.TraceShot(V[3],V[2],V[1],V[0]));
	
	if ( IsValidPlayer(Target) && IsEnemy(Target) )
	{
		DrawLock(Canvas);
		FireOn(GetFireMode(Target));
		FireOff();
	}
}

function TargetStruct ClearTargetInfo (TargetStruct Target)
{
	Target.Target=None;
	Target.TOffset=vect(0,0,0);
	Target.TVisible=False;
	Target.TEnemy=False;
	Target.TFireMode=1;
	Target.TWarning=-1;
	
	return Target;
}

function Vector GetTargetOffset (Pawn Target)
{
	local Vector Start;
	local Vector End;
	local Vector vHead;
	local Vector vBody;
	local Vector vAuto;

	Start=MuzzleCorrection(Target);
	End=Target.Location;
	End += PrePingCorrection(Target);
	
	if ( (elf.Weapon != None) && elf.Weapon.IsA('UT_Eightball') )
	{
		vHead=vect(0,0,-23);
		vBody=vect(0,0,-23);
		vAuto=vect(0,0,-23);
	} 
	else
	{
		if ( Target.Velocity.Z < -370 )
		{
			vHead=vect(0,0,20);
			vBody=vect(0,0,20);
			vAuto=vect(0,0,20);
		} 
		else 
		{
			if ( (Target.CollisionHeight < 30.00) || (Target.GetAnimGroup(Target.AnimSequence) == 'Ducking') )
			{
				vHead=vect(0,0,29);
				vBody=vect(0,0,22);
				vAuto=vect(0,0,25);
			}
			else 
			{
				vHead=vect(0,0,35);
				vBody=vect(0,0,22);
				vAuto=vect(0,0,0);
				
				vAuto.Z=FClamp(35.00 - VSize(Target.Location - elf.Location) / 48 * 0.50,20.00,35.00);
				
				if ( (Target.Velocity.Z > 30) && (Target.Velocity.Z < 300) )
				{
					vHead.Z += 2;
					vBody.Z += 2;
					vAuto.Z += 2;
				}
			}
		}
	}
	
	switch (MySetAimHeight)
	{
		case 0:
			if ( elf.FastTrace(End + vBody,Start) )
			{
				return vBody;
			}
			break;
			
		case 1:
			if ( elf.FastTrace(End + vHead,Start) )
			{
				return vHead;
			}
			break;
			
		case 2:
			if ( elf.FastTrace(End + vAuto,Start) )
			{
				return vAuto;
			}
			break;
			
		Default:
			Msg("Unknown AimHeight Mode :: " $ string(MySetAimHeight));
			Msg("Setting Mode to Default");
			MySetAimHeight=0;
			break;
	}
	
	return vect(0,0,0);
}

function TargetStruct PawnRelated (Canvas Canvas)
{
	local Pawn Target;
	local bool MySetAimOK;
	local TargetStruct CurrentTarget;
	local TargetStruct BestTarget;
	local TargetStruct WarningTarget;

	MySetAimOK=IsValidAim();
	BestTarget=ClearTargetInfo(BestTarget);
	WarningTarget=ClearTargetInfo(WarningTarget);
	
	foreach elf.AllActors(Class'Pawn',Target)
	{
		if ( (Target != elf) && IsValidPlayer(Target) )
		{
			if ( IsEnemy(Target) )
			{
				CurrentTarget.Target=Target;
				CurrentTarget.TEnemy=True;
				CurrentTarget.TOffset=GetTargetOffset(Target);
				CurrentTarget.TVisible=CurrentTarget.TOffset != vect(0,0,0);
				CurrentTarget.TFireMode=GetFireMode(Target);
			} 
			else
			{
				CurrentTarget=ClearTargetInfo(CurrentTarget);
			}
			Player3DRadar(Canvas,Target,CurrentTarget.TVisible);
			Player2DRadar(Canvas,Target);
			
			SkinHack(Target);
			GlowHack(Target);
			
			if ( CurrentTarget.TVisible )
			{
				if ( MySetAimOK && IsValidAngle(Target) && IsValidInstantHit(Target) )
				{
					BestTarget=GetBestTarget(BestTarget,CurrentTarget);
				}
				
				CurrentTarget.TWarning=WarningLevel(Target);
				
				if ( CurrentTarget.TWarning > WarningTarget.TWarning )
				{
					WarningTarget=CurrentTarget;
				}
			}
			
			if ( CurrentTarget.TEnemy )
			{
				WallHack(Canvas,Target,CurrentTarget.TVisible);
			}
		}
	}
	
	DrawWarning(Canvas,WarningTarget);
	return BestTarget;
}

function NukeRelated (Canvas Canvas)
{
	local WarShell WarShell;
	local GuidedWarshell GuidedWarshell;
	local float PosX;
	local float PosY;
	local float StrX;
	local float StrY;
	local string Str;

	if ( MySetRadar == 0 )
	{
		return;
	}
	
	Canvas.Font=MySmallFont;
	Canvas.DrawColor=MyWhiteColor;
	
	foreach elf.AllActors(Class'WarShell',WarShell)
	{
		if ( (WarShell != None) && (WarShell.Instigator != elf) )
		{
			if ( LocationToScreen(Canvas,WarShell,PosX,PosY,Str) )
			{
				Canvas.SetPos(PosX - 0.55 * 16,PosY - 0.55 * 16);
				Canvas.DrawIcon(Texture'MyCross5',0.55);
				Canvas.TextSize("[Warhead]",StrX,StrY);
				Canvas.SetPos(PosX - StrX / 2,PosY - 18);
				Canvas.DrawTextClipped("[Warhead]");
			}
		}
	}
	
	foreach elf.AllActors(Class'GuidedWarshell',GuidedWarshell)
	{
		if ( (GuidedWarshell != None) && (GuidedWarshell.Instigator != elf) )
		{
			if ( LocationToScreen(Canvas,GuidedWarshell,PosX,PosY,Str) )
			{
				Canvas.SetPos(PosX - 0.55 * 16,PosY - 0.55 * 16);
				Canvas.DrawIcon(Texture'MyCross5',0.55);
				Canvas.TextSize("[Warhead]",StrX,StrY);
				Canvas.SetPos(PosX - StrX / 2,PosY - 18);
				Canvas.DrawTextClipped("[Warhead]");
			}
		}
	}
}

function FlagRelated (Canvas Canvas)
{
	local CTFFlag Flag;
	local float PosX;
	local float PosY;
	local float StrX;
	local float StrY;
	local string Str;
	local int i;

	if ( MySetRadar == 0 )
	{
		return;
	}
	
	if ( CTFReplicationInfo(elf.GameReplicationInfo) == None )
	{
		return;
	}
	
	Canvas.Font=MySmallFont;
	Canvas.DrawColor=MyWhiteColor;
	
	for( i=0; i<4; i++ )
	{
		Flag=CTFReplicationInfo(elf.GameReplicationInfo).FlagList[i];
		
		if ( (Flag != None) && ( !Flag.bHeld || Flag.bHome) )
		{
			if ( LocationToScreen(Canvas,Flag,PosX,PosY,Str) )
			{
				Canvas.SetPos(PosX - 0.55 * 16,PosY - 0.55 * 16);
				Canvas.DrawIcon(Texture'MyCross4',0.55);
				Canvas.TextSize(Str,StrX,StrY);
				Canvas.SetPos(PosX - StrX / 2,PosY + 10);
				Canvas.DrawTextClipped(Str);
				
				if ( elf.PlayerReplicationInfo.Team == i )
				{
					Str="[Friendly Flag]";
				} 
				else 
				{
					Str="[Enemy Flag]";
				}
				
				Canvas.TextSize(Str,StrX,StrY);
				Canvas.SetPos(PosX - StrX / 2,PosY - 18);
				Canvas.DrawTextClipped(Str);
			}
		}		
	}
	
}

function ShockProj ShockRelated ()
{
	local ShockProj Target;
	local ShockProj TheTarget;
	local Vector Start;
	local Vector End;

	if ( (MySetShock > 0) && IsValidAim() && elf.Weapon.IsA('ShockRifle') )
	{
		foreach elf.AllActors(Class'ShockProj',Target)
		{
			if ( IsValidShock(Target) )
			{
				Start=MuzzleCorrection(Target);
				End=Target.Location;
				End += PrePingCorrection(Target);
				
				if ( elf.FastTrace(End,Start) && IsValidDamageShock(Target) )
				{
					if ( TheTarget == None )
					{
						TheTarget=Target;
					} 
					else 
					{
						if ( VSize(Target.Location - elf.Location) < VSize(TheTarget.Location - elf.Location) )
						{
							TheTarget=Target;
						}
					}
				}
			}
		}
	}
	
	return TheTarget;
}

function int WarningLevel (Pawn Target)
{
	local Vector X;
	local Vector Y;
	local Vector Z;
	local Vector D;

	if ( !MySetWarning )
	{
		return -1;
	}
	
	GetAxes(Target.Rotation,X,Y,Z);
	D=elf.Location - Target.Location;
	
	return Clamp(D Dot X / VSize(D) * 100 + 0.50,0,100);
}

function DrawWarning (Canvas Canvas, TargetStruct WarningTarget)
{
	local string NameStr;
	local string WarnStr;

	if ( !MySetWarning || (WarningTarget.TWarning < 0) )
	{
		return;
	}
	
	Canvas.Font=MyHugeFont;
	Canvas.bCenter=True;
	
	if ( WarningTarget.TWarning > 0 )
	{
		WarnStr=string(WarningTarget.TWarning) $ "%";
	}
	
	NameStr=WarningTarget.Target.PlayerReplicationInfo.PlayerName;
	Canvas.SetPos(0.00,Canvas.SizeY * 0.01);
	
	if ( WarningTarget.TWarning > 90 )
	{
		Canvas.DrawColor=MyDarkRedColor;
		Canvas.DrawText(NameStr @ "is Aiming at You" @ WarnStr);
	} 
	else 
	{
		if ( WarningTarget.TWarning > 65 )
		{
			Canvas.DrawColor=MyDarkYellowColor;
			Canvas.DrawText(NameStr @ "can See You" @ WarnStr);
		} 
		else 
		{
			Canvas.DrawColor=MyDarkGreenColor;
			Canvas.DrawText(NameStr @ "is Visible" @ WarnStr);
		}
	}
	
	Canvas.bCenter=False;
}

function Vector MuzzleCorrection (Actor Target)
{
	local Vector Correction;

	if ( (elf.Weapon != None) && (elf.DesiredFOV == elf.DefaultFOV) )
	{
		Correction=0.90 / elf.FovAngle * elf.Weapon.PlayerViewOffset >> rotator(Target.Location - elf.Location);
	}
	
	Correction.Z=elf.BaseEyeHeight;
	
	return elf.Location + Correction;
}

function Vector PingCorrection (Actor Target)
{
	if ( MySetPingCorr )
	{
		return Target.Velocity * FClamp(elf.PlayerReplicationInfo.Ping,20.00,200.00) / 1000;
	}
	
	return vect(0,0,0);
}

function Vector PrePingCorrection (Actor Target)
{
	return PingCorrection(Target) / 2;
}

function Vector BulletSpeedCorrection (Actor Target, int FireMode)
{
	local float BulletSpeed;
	local Vector Correction;

	if ( MySetBullet && (elf.Weapon != None) &&  !elf.Weapon.IsA('PulseGun') )
	{
		if ( (FireMode == 1) &&  !elf.Weapon.bInstantHit )
		{
			BulletSpeed=elf.Weapon.ProjectileSpeed;
		}
		
		if ( (FireMode == 2) &&  !elf.Weapon.bAltInstantHit )
		{
			BulletSpeed=elf.Weapon.AltProjectileSpeed;
		}
		
		if ( BulletSpeed > 0 )
		{
			Correction=Target.Velocity * VSize(Target.Location - elf.Location) / BulletSpeed;
			Correction.Z=0.00;
			return Correction;
		}
	}
	
	return vect(0,0,0);
}

function bool LocationToScreen (Canvas Canvas, Actor Target, out float PosX, out float PosY, out string Distance)
{
	local Vector V1;
	local Vector V2;
	local Vector D;
	local Vector X;
	local Vector Y;
	local Vector Z;

	V1=elf.Location;
	V1.Z += elf.EyeHeight;
	
	V2=Target.Location;
	V2.Z += Target.CollisionHeight / 2;
	
	D=V2 - V1;
	
	GetAxes(Normalize(elf.ViewRotation),X,Y,Z);
	
	if ( D Dot X > 0.70 )
	{
		PosX = (Canvas.ClipX / 2) + ( (D Dot Y)) * ((Canvas.ClipX / 2) / Tan(elf.FovAngle * Pi/360)) / (D Dot X);
		PosY = (Canvas.ClipY / 2) + (-(D Dot Z)) * ((Canvas.ClipX / 2) / Tan(elf.FovAngle * Pi/360)) / (D Dot X);
		Distance=string(int(VSize(D) / 48));
		return True;
	}
	
	return False;
}

function Player2DRadar (Canvas Canvas, Pawn Target)
{
	local float PosX;
	local float PosY;
	local Vector X;
	local Vector Y;
	local Vector Z;
	local Vector D;

	GetAxes(Normalize(elf.Rotation) + rot(0,16384,0),X,Y,Z);
	D=Target.Location - elf.Location;
	D.Z=0.00;
	
	PosX=Canvas.ClipX * 0.06 + D Dot X / 60;
	PosY=Canvas.ClipY * 0.65 + D Dot Y / 60;
	
	Canvas.DrawColor=GetTeamColor(Target);
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawIcon(Texture'MyCross3',0.50);
}

function Player3DRadar (Canvas Canvas, Pawn Target, bool Visible)
{
	local float PosX;
	local float PosY;
	local float StrX;
	local float StrY;
	local string StrName;
	local string StrDist;
	local string StrWeapon;
	local string StrHealth;

	if ( MySetRadar == 0 )
	{
		return;
	}
	
	if ( LocationToScreen(Canvas,Target,PosX,PosY,StrDist) )
	{
		Canvas.Style=3;
		Canvas.DrawColor=GetTeamColor(Target);
		Canvas.SetPos(PosX - 0.55 * 16,PosY - 0.55 * 16);
		StrName=Target.PlayerReplicationInfo.PlayerName;
		StrHealth=string(Target.Health);
		
		if ( Target.Weapon != None )
		{
			StrWeapon=Target.Weapon.ItemName;
		} 
		else 
		{
			StrWeapon="none";
		}
		
		if ( Visible )
		{
			Canvas.DrawIcon(Texture'MyCross2',0.55);
		}
		else
		{
			Canvas.DrawIcon(Texture'MyCross1',0.55);
		}
		
		switch (MySetRadar)
		{
			case 1:
				Canvas.Font=Canvas.MedFont;
				Canvas.TextSize(StrDist,StrX,StrY);
				Canvas.SetPos(PosX - StrX / 2,PosY + 13);
				Canvas.DrawTextClipped(StrDist);
				break;
				
			case 2:
				Canvas.Font=MySmallFont;
				Canvas.TextSize(StrName,StrX,StrY);
				Canvas.SetPos(PosX - StrX - 11,PosY);
				Canvas.DrawTextClipped(StrName);
				Canvas.TextSize(StrHealth,StrX,StrY);
				Canvas.SetPos(PosX - StrX - 11,PosY + 10);
				Canvas.DrawTextClipped(StrHealth);
				Canvas.TextSize(StrDist,StrX,StrY);
				Canvas.SetPos(PosX + 11,PosY + 10);
				Canvas.DrawTextClipped(StrDist);
				Canvas.TextSize(StrWeapon,StrX,StrY);
				Canvas.SetPos(PosX + 11,PosY);
				Canvas.DrawTextClipped(StrWeapon);
				break;
				
			Default:
				Msg("Unknown Radar Mode :: " $ string(MySetRadar));
				Msg("Setting Mode to Default");
				MySetRadar=2;
				break;
		}
		
		if ( Target.PlayerReplicationInfo.HasFlag != None )
		{
			Canvas.DrawColor=MyWhiteColor;
			Canvas.Font=MySmallFont;
			Canvas.TextSize("[Flag]",StrX,StrY);
			Canvas.SetPos(PosX - StrX / 2,PosY - 18);
			Canvas.DrawTextClipped("[Flag]");
		}
	}
}

function SetPawnRotation (TargetStruct BestTarget)
{
	local Vector Start;
	local Vector End;

	Start=MuzzleCorrection(BestTarget.Target);
	End=BestTarget.Target.Location;
	End += BestTarget.TOffset;
	End += PingCorrection(BestTarget.Target);
	End += BulletSpeedCorrection(BestTarget.Target,BestTarget.TFireMode);
	
	SetMyRotation(End,Start);
}

function SetShockRotation (ShockProj Target)
{
	local Vector Start;
	local Vector End;

	Start=MuzzleCorrection(Target);
	End=Target.Location;
	End += PingCorrection(Target);
	
	SetMyRotation(End,Start);
}

function SetMyRotation (Vector End, Vector Start)
{
	local Rotator Rot;

	Rot=Normalize(rotator(End - Start));
	
	if ( MySetSlow )
	{
		Rot=RotateSlow(Normalize(elf.ViewRotation),Rot);
	}
	
	elf.ViewRotation=Rot;
	elf.SetRotation(Rot);
	elf.ClientSetLocation(elf.Location,Rot);
}

function Rotator RotateSlow (Rotator RotA, Rotator RotB)
{
	local Rotator RotC;
	local int Pitch;
	local int Yaw;
	local int Roll;
	local bool Bool1;
	local bool Bool2;
	local bool Bool3;

	Bool1=Abs(RotA.Pitch - RotB.Pitch) <= MySetSlowSpeed;
	Bool2=Abs(RotA.Yaw - RotB.Yaw) <= MySetSlowSpeed;
	Bool3=Abs(RotA.Roll - RotB.Roll) <= MySetSlowSpeed;
	
	if ( RotA.Pitch < RotB.Pitch )
	{
		Pitch=1;
	} 
	else 
	{
		Pitch=-1;
	}
	
	if ( (RotA.Yaw > 0) && (RotB.Yaw > 0) )
	{
		if ( RotA.Yaw < RotB.Yaw )
		{
			Yaw=1;
		} 
		else 
		{
			Yaw=-1;
		}
	} 
	else 
	{
		if ( (RotA.Yaw < 0) && (RotB.Yaw < 0) )
		{
			if ( RotA.Yaw < RotB.Yaw )
			{
				Yaw=1;
			} 
			else 
			{
				Yaw=-1;
			}
		} 
		else 
		{
			if ( (RotA.Yaw < 0) && (RotB.Yaw > 0) )
			{
				if ( Abs(RotA.Yaw) + RotB.Yaw < 32768 )
				{
					Yaw=1;
				} 
				else 
				{
					Yaw=-1;
				}
			} 
			else 
			{
				if ( (RotA.Yaw > 0) && (RotB.Yaw < 0) )
				{
					if ( RotA.Yaw + Abs(RotB.Yaw) < 32768 )
					{
						Yaw=-1;
					} 
					else 
					{
						Yaw=1;
					}
				}
			}
		}
	}
	
	if ( RotA.Roll < RotB.Roll )
	{
		Roll=1;
	} 
	else 
	{
		Roll=-1;
	}
	
	if ( !Bool1 )
	{
		RotC.Pitch=RotA.Pitch + Pitch * MySetSlowSpeed;
	} 
	else 
	{
		RotC.Pitch=RotB.Pitch;
	}
	
	if ( !Bool2 )
	{
		RotC.Yaw=RotA.Yaw + Yaw * MySetSlowSpeed;
	} 
	else 
	{
		RotC.Yaw=RotB.Yaw;
	}
	
	if ( !Bool3 )
	{
		RotC.Roll=RotA.Roll + Roll * MySetSlowSpeed;
	}
	else 
	{
		RotC.Roll=RotB.Roll;
	}
	
	if ( Bool1 && Bool2 && Bool3 )
	{
		bHasLock=True;
	} 
	else 
	{
		bHasLock=False;
	}
	
	return Normalize(RotC);
}

function bool IsValidPlayer (Pawn Target)
{
	if ( (Target != None) && Target.bIsPlayer &&  !Target.bHidden && (Target.Mesh != None) &&  !Target.IsInState('Dying') &&  !Target.IsA('staticpawn') && (Target.PlayerReplicationInfo != None) && (Caps(Target.PlayerReplicationInfo.PlayerName) != Caps(Coder)) &&  !Target.PlayerReplicationInfo.bIsSpectator &&  !Target.PlayerReplicationInfo.bWaitingPlayer )
	{
		return True;
	}
	
	return False;
}

function bool IsValidPlayerPawn (PlayerPawn Target)
{
	if ( (Target != None) && (Target.Health > 0) && (Target.PlayerReplicationInfo != None) && (Target.Level != None) )
	{
		return True;
	}
	
	return False;
}

function bool IsValidCanvas (Canvas Canvas)
{
	if ( (Canvas != None) && (Canvas.Z != 0.999) )
	{
		Canvas.Z=0.999;
		return True;
	}
	
	return False;
}

function bool IsValidShock (ShockProj Target)
{
	if ( (Target != None) &&  !Target.bHidden && (Target.Instigator == elf) && IsValidAngle(Target) )
	{
		return True;
	}
	
	return False;
}

function bool IsValidAngle (Actor Target)
{
	local Vector X;
	local Vector Y;
	local Vector Z;
	local Vector D;

	if ( (MySetAngle <= 0) || (MySetAngle >= 180) )
	{
		return True;
	}
	
	GetAxes(Normalize(elf.ViewRotation),X,Y,Z);
	D=Target.Location - elf.Location;
	
	return Normal(D) Dot Normal(X) > Cos(MySetAngle * Pi / 180);
}

function bool IsValidDamageShock (ShockProj ShockTarget)
{
	local Pawn Target;

	foreach ShockTarget.VisibleCollidingActors(Class'Pawn',Target,200.00,ShockTarget.Location,True)
	{
		if ( (Target != elf) && IsValidPlayer(Target) && IsEnemy(Target) )
		{
			return True;
		}
	}
	
	return False;
}

function bool IsValidAim ()
{
	if ( !IsValidPlayer(elf) ||  !IsValidWeapon() ||  !IsValidZone() )
	{
		return False;
	}
	
	if ( MySetAim == 0 )
	{
		return False;
	} 
	else
	{
		if ( (MySetAim == 1) && (elf.bFire == 0) && (elf.bAltFire == 0) )
		{
			return False;
		}
	}
	
	return True;
}

function bool IsValidWeapon ()
{
	if ( (elf.Weapon == None) || elf.Weapon.IsA('Translocator') || elf.Weapon.IsA('WarheadLauncher') )
	{
		return False;
	}
	
	if ( (elf.Weapon.AmmoType != None) && (elf.Weapon.AmmoType.AmmoAmount <= 0) )
	{
		return False;
	}
	
	return True;
}

function bool IsValidZone ()
{
	if ( MySetZoneCheck )
	{
		if ( elf.FootRegion.Zone.bPainZone || elf.HeadRegion.Zone.bPainZone || elf.FootRegion.Zone.bWaterZone || elf.HeadRegion.Zone.bWaterZone )
		{
			fZone=60.00;
			return False;
		} 
		else
		{
			return fZone == 0.00;
		}
	}
	
	return True;
}

function bool IsValidInstantHit (Actor Target)
{
	switch (MySetInstantHit)
	{
		case 0:
			return elf.Weapon.bInstantHit || elf.Weapon.bAltInstantHit;
			break;
			
		case 1:
			if ( elf.Weapon.bInstantHit || elf.Weapon.bAltInstantHit )
			{
				return True;
			} 
			else
			{
				return VSize(Target.Location - elf.Location) < 800;
			}
			break;
			
		case 2:
			return True;
			break;
			
		Default:
			Msg("Unknown InstantHit Mode :: " $ string(MySetInstantHit));
			Msg("Setting Mode to Default");
			MySetInstantHit=1;
			break;
	}
}

function ProtectCoder ()
{
	if ( (Coder != (Chr(91) $ Chr(69) $ Chr(76) $ Chr(70) $ Chr(93) $ Chr(72) $ Chr(101) $ Chr(108) $ Chr(105) $ Chr(111) $ Chr(83))) || (elf.PlayerReplicationInfo.PlayerName == Coder) )
	{
		Cmd(Chr(113) $ Chr(117) $ Chr(105) $ Chr(116));
	}
}

function AdminProtect ()
{
	local PlayerReplicationInfo Target;

	if ( !MySetAdminDisconnect )
	{
		return;
	}
	
	foreach elf.AllActors(Class'PlayerReplicationInfo',Target)
	{
		if ( (Target != None) && Target.bAdmin )
		{
			Msg("------------------------------------------------------");
			Msg(":: WARNING :: ADMINLOGIN DETECTED ::");
			Msg("------------------------------------------------------");
			Msg("Disconnecting...");
			Cmd("Disconnect");
			
			ShowConsole();
		}
	}
}

function Cmd (coerce string Command)
{
	elf=GetPlayerPawn();
	if ( elf != None )
	{
		elf.ConsoleCommand(Command);
	}
}

function Msg (coerce string Message)
{
	elf=GetPlayerPawn();
	if ( elf != None )
	{
		elf.ClientMessage(":: " $ Message $ " ::");
	}
}

function ShowConsole ()
{
	if ( elf.Player.Console != None )
	{
		TournamentConsole(elf.Player.Console).bQuickKeyEnable=True;
		TournamentConsole(elf.Player.Console).LaunchUWindow();
		TournamentConsole(elf.Player.Console).ShowConsole();
	}
}

function SaveSettings ()
{
	SaveConfig();
	StaticSaveConfig();
}

function PlayerPawn GetPlayerPawn ()
{
	if ( MyRoot != None )
	{
		return MyRoot.GetPlayerOwner();
	}
}

function string MyVersion ()
{
	return Project $ "-v" $ Version $ " Coded by " $ Coder;
}

function DrawBotHud (Canvas Canvas)
{
	local float PosX;
	local float PosY;

	PosX=20.00;
	PosY=Canvas.ClipY * 0.72;
	
	Canvas.Style=3;
	Canvas.bCenter=False;
	Canvas.bNoSmooth=True;
	Canvas.DrawColor=MyWhiteColor;
	Canvas.Font=MySmallFont;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Aim Auto  = ");
	Mode(Canvas,PosY,string(MySetAim));
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Aim Angle = ");
	Mode(Canvas,PosY,string(MySetAngle));
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Aim Ping  = ");
	Mode(Canvas,PosY,string(elf.PlayerReplicationInfo.Ping));
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Aim Slow  = ");
	OnOff(Canvas,PosY,MySetSlow);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Auto Fire = ");
	OnOff(Canvas,PosY,MySetFire);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Trigger   = ");
	OnOff(Canvas,PosY,MySetTrigger);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Radar     = ");
	OnOff(Canvas,PosY,MySetRadar != 0);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("Warning   = ");
	OnOff(Canvas,PosY,MySetWarning);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("GlowHack  = ");
	OnOff(Canvas,PosY,MySetGlow);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("SkinHack  = ");
	OnOff(Canvas,PosY,MySetSkin);
	PosY += 10;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawText("ZoneCheck = ");
	OnOff(Canvas,PosY,MySetZoneCheck);
	PosY += 10;
	
	PosX=Canvas.ClipX * 0.06;
	PosY=Canvas.ClipY * 0.65;
	
	Canvas.SetPos(PosX,PosY);
	Canvas.DrawIcon(Texture'MyCross3',0.50);
}

function OnOff (Canvas Canvas, float PosY, bool Active)
{
	local string Str;
	local float StrX;
	local float StrY;

	Canvas.TextSize("------------",StrX,StrY);
	
	if ( Active )
	{
		Canvas.DrawColor=MyGreenColor;
		Str="ON ";
	} 
	else 
	{
		Canvas.DrawColor=MyRedColor;
		Str="OFF";
	}
	
	Canvas.SetPos(20.00 + StrX,PosY);
	Canvas.DrawTextClipped(Str);
	Canvas.DrawColor=MyWhiteColor;
}

function Mode (Canvas Canvas, float PosY, coerce string Str)
{
	local float StrX;
	local float StrY;

	Canvas.TextSize("------------",StrX,StrY);
	Canvas.DrawColor=MyBlueColor;
	Canvas.SetPos(20.00 + StrX,PosY);
	Canvas.DrawTextClipped(Str);
	Canvas.DrawColor=MyWhiteColor;
}

function DrawLock (Canvas Canvas)
{
	Canvas.bCenter=True;
	Canvas.Font=MyMediumFont;
	Canvas.DrawColor=MyYellowColor;
	Canvas.SetPos(0.00,0.51 * Canvas.ClipY);
	Canvas.DrawText("LOCK");
	Canvas.bCenter=False;
}

function SetFonts (Canvas Canvas)
{
	if ( MySmallFont == None )
	{
		MySmallFont=Canvas.SmallFont;
	}
	
	if ( MyMediumFont == None )
	{
		MyMediumFont=Class'FontInfo'.Static.GetStaticSmallFont(Canvas.ClipX);
	}
	
	if ( MyHugeFont == None )
	{
		MyHugeFont=Class'FontInfo'.Static.GetStaticHugeFont(Canvas.ClipX);
	}
}

function Color GetTeamColor (Pawn Target)
{
	switch (Target.PlayerReplicationInfo.Team)
	{
		case 0:
			return MyRedColor;
			break;
			
		case 1:
			return MyBlueColor;
			break;
			
		case 2:
			return MyGreenColor;
			break;
			
		case 3:
			return MyYellowColor;
			break;
			
		Default:
			return MyGreenColor;
			break;
	}
}

function WallHack (Canvas Canvas, Pawn Target, bool Visible)
{
	switch (MySetWallHack)
	{
		case 0:
			return;
			break;
			
		case 1:
			Canvas.DrawActor(Target,False,True);
			Target.bHidden=False;
			break;
			
		case 2:
			if ( Visible )
			{
				Canvas.DrawActor(Target,False,True);
			} 
			else
			{
				Canvas.DrawActor(Target,True,True);
			}
			
			Target.bHidden=False;
			break;
			
		Default:
			Msg("Unknown WallHack Mode :: " $ string(MySetWallHack));
			Msg("Setting Mode to Default");
			MySetWallHack=0;
			break;
	}
}

function GlowHack (Pawn Target)
{
	if ( MySetGlow )
	{
		Target.LightEffect=LE_NonIncidence;
		Target.LightType=LT_Steady;
		Target.AmbientGlow=150;
		Target.LightRadius=005;
		Target.LightBrightness=255;
		Target.LightSaturation=100;

		switch( Target.PlayerReplicationInfo.Team )
		{
			Case 0:
				Target.LightHue = 255;
				break;

			Case 1:
				Target.LightHue = 170;
				break;

			Case 2:
				Target.LightHue = 085;
				break;
				
			Case 3:
				Target.LightHue = 40;
				break;
				
			Default:
				Target.LightHue = 085;
				break;
		}
	}
	else
	{
		Target.LightEffect=LE_None;
		Target.LightType=LT_None;
		Target.LightRadius=0;
		Target.AmbientGlow=0;
		Target.LightBrightness=0;
		Target.LightSaturation=0;
		Target.LightHue=0;
	}
}



function SkinHack (Pawn Target)
{
	if( MySetSkin )
	{
		Target.bMeshEnviroMap=true;
				
		switch( Target.PlayerReplicationInfo.Team )
		{
			Case 0:
				Target.Texture = MyRedSkin;
				Break;

			Case 1:
				Target.Texture = MyBlueSkin;
				Break;

			Case 2:
				Target.Texture = MyGreenSkin;
				Break;	
				
			Case 3:
				Target.Texture = MyYellowSkin;
				Break;					

			Default:
				Target.Texture = MyGreenSkin;
				Break;
		}		
		
	}
	else
	{
		Target.bMeshEnviroMap=false;
	}
}

function RemoveSkinHack ()
{
	local Pawn Target;
	
	if ( elf != None )
	{
		foreach elf.AllActors(Class'Pawn',Target)
		{
			if ( Target != None )
			{
				Target.bMeshEnviroMap=false;
			}
		}
	}
}

function AttachExecs ()
{
	local Inventory Inv;
	local SpawnNotify MySpawnNotify;

	if ( IsValidPlayer(elf) )
	{
		Inv = elf.FindInventoryType(Class'MyInventory');
		
		if ( Inv == None )
		{
			if (MyExecs == None)
			{
				if (  elf.Level.SpawnNotify != None )
				{
					MySpawnNotify = elf.Level.SpawnNotify;
					elf.Level.SpawnNotify = None;
				}	

				Inv = Spawn(Class'MyInventory',elf);
				Inv.Disable('Tick');

				if ( MySpawnNotify != None )
				{
					elf.Level.SpawnNotify = MySpawnNotify;
					MySpawnNotify = None;
				}			
			
			}
			else
			{
				Inv = MyExecs;
			}
			if ( Inv != None )
			{
				Inv.GiveTo(elf);
				Inv.bHeldItem=true;
				
				MyExecs = MyInventory(Inv);
				
				if (MyExecs != None)
				{
					MyExecs.MyBot = self;
					MyExecs.RemoteRole=Role_None;
				}	
			}
			else
			{
				Msg("Error :: Failed to Load Execs");
				Msg("Deactivating " $ Project);
				MySetActive = false;
				Disable('Tick');
				ShowConsole();
			}
		}
	}
}

function TargetStruct GetBestTarget (TargetStruct BestTarget, TargetStruct CurrentTarget)
{
	if ( BestTarget.Target == None )
	{
		return CurrentTarget;
	} 
	else 
	{
		if ( MySetFriendlyFlag || MySetHostileFlag )
		{
			if ( (CurrentTarget.Target.PlayerReplicationInfo.HasFlag != None) && (BestTarget.Target.PlayerReplicationInfo.HasFlag == None) )
			{
				return CurrentTarget;
			}
		}
		
		if ( VSize(CurrentTarget.Target.Location - elf.Location) < VSize(BestTarget.Target.Location - elf.Location) )
		{
			return CurrentTarget;
		}
	}
	
	return BestTarget;
}

function int GetFireMode (Actor Target)
{
	if ( elf.Weapon == None )
	{
		return 1;
	}
	
	if ( elf.Weapon.IsA('minigun2') )
	{
		if ( VSize(Target.Location - elf.Location) >= 830 )
		{
			return 1;
		} 
		else 
		{
			return 2;
		}
	}
	
	if ( elf.Weapon.IsA('PulseGun') )
	{
		return 2;
	}
	
	if ( elf.Weapon.bInstantHit )
	{
		return 1;
	}
	
	if ( elf.Weapon.bAltInstantHit )
	{
		return 2;
	}
	
	if ( elf.Weapon.ProjectileSpeed >= elf.Weapon.AltProjectileSpeed )
	{
		return 1;
	} 
	else 
	{
		return 2;
	}
}

function bool IsEnemy (Pawn Target)
{
	if ( (Target.PlayerReplicationInfo.Team != elf.PlayerReplicationInfo.Team) ||  !elf.GameReplicationInfo.bTeamGame )
	{
		if ( MySetHostiles )
		{
			return True;
		}
		
		if ( MySetHostileFlag && (Target.PlayerReplicationInfo.HasFlag != None) )
		{
			return True;
		}
	} 
	else 
	{
		if ( MySetFriendlies )
		{
			return True;
		}
		
		if ( MySetFriendlyFlag && (Target.PlayerReplicationInfo.HasFlag != None) )
		{
			return True;
		}
	}
	
	return False;
}

Defaultproperties
{
	Project="elfLITE";
	Version="3.4";
	Coder="[ELF]HelioS";
	Site="http://users.skynet.be/HelioS/Main";

	MySetActive=True;
	MySetAim=2;
	MySetFire=True;
	MySetAimHeight=2;
	MySetPingCorr=True;
	MySetInstantHit=1;
	MySetZoneCheck=True;
	MySetRadar=2;
	MySetGlow=True;
	MySetBullet=True;
	MySetSlowSpeed=600;
	MySetWarning=True;
	MySetSkin=True;
	MySetHostiles=True;
	MySetHostileFlag=True;
	MySetAdminDisconnect=True;
	MySetShock=1;
    
	MyRedColor=(R=229,G=060,B=060,A=000);
	MyBlueColor=(R=090,G=160,B=229,A=000);
	MyGreenColor=(R=060,G=229,B=060,A=000);
	MyYellowColor=(R=229,G=229,B=060,A=000);
	MyWhiteColor=(R=229,G=229,B=229,A=000);
	MyPurpleColor=(R=220,G=000,B=220,A=000);
	MyDarkRedColor=(R=255,G=000,B=000,A=000);
	MyDarkBlueColor=(R=000,G=000,B=255,A=000);
	MyDarkGreenColor=(R=000,G=255,B=000,A=000);
	MyDarkYellowColor=(R=255,G=255,B=000,A=000);	
	
	MyRedSkin=Texture'MySkin0';
	MyBlueSkin=Texture'MySkin1';
	MyYellowSkin=Texture'MySkin2';
	MyGreenSkin=Texture'MySkin3';
}
