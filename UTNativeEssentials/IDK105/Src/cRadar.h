         ,-.
        / \  `.  __..-,O
       :   \ --''_..-'.'
       |    . .-' `. '.
       :     .     .`.'
        \     `.  /  ..
         \      `.   ' .
          `,       `.   \
         ,|,`.        `-.\
        '.||  ``-...__..-`
         |  |
         |__|
         /||\
        //||\\
       // || \\
    __//__||__\\__
   '--------------' SSt

#pragma once
#ifndef _RADAR_
#define _RADAR_

class cRadar : public HookClass
{
protected:
	FVector WorldToScreen (UCanvas* Canvas, FVector WorldLocation);
	void DrawBoundingBox(UCanvas* Canvas, APawn* Target);
public:
	cRadar();
	~cRadar();
	void DrawPlayerOnRadar (UCanvas* Canvas, APawn *Target);
	void inline DrawPlayer2DRadar (UCanvas* Canvas, APawn* Target, float PosX, float PosY, float Width, float Height);
	void inline DrawHealthBar (UCanvas* Canvas, float PosX, float PosY, int Health);
};

cRadar::cRadar() {};
cRadar::~cRadar(){};

#endif _RADAR_

void inline cRadar::DrawHealthBar (UCanvas* Canvas, float PosX, float PosY, int Health)
{
	float healthMeterAdjustment = 40 * Scale;
	PosY = PosY - 9.5f * Scale;// hehe, what a value

	DrawRec(Canvas, PosX - healthMeterAdjustment, PosY, 40 * Scale, 6.0f * Scale, WhiteTexture, FColor(0, 0, 0, 0));
	DrawRec(Canvas, PosX - healthMeterAdjustment + 1, PosY + 1, 38 * Scale, 4.0f * Scale, WhiteTexture, FColor(255, 255, 255, 255));

	int Tmp = Min(int(Health * 2.55), 255);
	Canvas->Color = FColor(255 - Tmp,Tmp,0);
	Tmp = Min(int(Health * 0.38), 38);

	DrawRec(Canvas, PosX - healthMeterAdjustment + 1, PosY + 1, Tmp * Scale, 4.0f * Scale, WhiteTexture, Canvas->Color);
}

FVector cRadar::WorldToScreen (UCanvas* Canvas, FVector WorldLocation)
{
	FVector X,Y,Z,D,Out;
	GetAxes(MyCameraRotation,X,Y,Z);
	D = WorldLocation - MyCameraLocation;
	Out.X = (Canvas->ClipX/2)+( Dot(D,Y))*((Canvas->ClipX/2)/tan(Me->FovAngle*PI/360))/Dot(D,X);
	Out.Y = (Canvas->ClipY/2)+(-Dot(D,Z))*((Canvas->ClipX/2)/tan(Me->FovAngle*PI/360))/Dot(D,X);
	Out.Z = 0;
	return Out;
}

void cRadar::DrawBoundingBox(UCanvas* Canvas, APawn* Target)
{
	static TCHAR StrTemp[MAX_PATH];
	FVector X,Y,Z,E,D,F,top,bottom;
	float width, Left, Right, Top, Bot;
	FColor Color = GetTeamColor(Target);

	GetAxes(MyCameraRotation,X,Y,Z);

	D = Target->Location - MyCameraLocation;
	E = D /= D.Size();

	if (Dot(E, X) <= cos(90 * PI / 180))
	{
		return;
	}
 
	top = Target->Location;
	top.Z += Target->CollisionHeight * 1.1;
	top = WorldToScreen(Canvas,top);

	bottom = Target->Location;
	bottom.Z -= Target->CollisionHeight * 1.1;
	bottom = WorldToScreen(Canvas,bottom);

	width = (top.Y - bottom.Y) * 0.25;
	Left  = top.X + width;
	Right = top.X - width;
	Top   = top.Y;
	Bot   = bottom.Y;

	//WhiteTexture->MinFrameRate = 35;

	Canvas->Color = GetTeamColor(Target);
	DrawRec(Canvas, Left, Top,           1, Bot-Top, WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Left, Bot,1+Right-Left,       1, WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Right, Top,           1, Bot-Top,WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Left, Top,  Right-Left,       1, WhiteTexture, Canvas->Color);

	F = Target->Location - MyCameraLocation;

	INT XL, YL;
	UFont* theFont = SmallFont;

	Canvas->Font = SmallFont;
	Canvas->WrappedStrLenf(SmallFont, XL, YL, L"Test");

	appSnprintf(StrTemp, 40, L"%s", Target->PlayerReplicationInfo->PlayerName);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, Right + 3, Top, GetTeamColor(Target), theFont);

	appSnprintf(StrTemp, 40, L"H:%d", Target->Health);
	DrawMyText(Canvas, StrTemp, Right + 3, Top + YL + 1, GetTeamColor(Target), theFont);

	appSnprintf(StrTemp, 40, TEXT("D:%.0f"), F.Size() / 48);
	DrawMyText(Canvas,StrTemp,Right + 3,Top + YL + YL + 2,GetTeamColor(Target), theFont);

	if (bHealthbars)
	{
		DrawHealthBar(Canvas, top.X, Top + 10, Target->Health);
	}
}

void cRadar::DrawPlayerOnRadar (UCanvas* Canvas, APawn *Target)
{
	if ( !b3DRadar ) return;

	FVector X,Y,Z,D,E,top,bottom;;
	FColor Color = GetTeamColor(Target);

	GetAxes(MyCameraRotation,X,Y,Z);

	D = Target->Location - MyCameraLocation;
	E = D /= D.Size();

	if (Dot(E, X) <= cos(90 * PI / 180))
	{
		return;
	}

	DrawBoundingBox(Canvas,Target);
}

void inline cRadar::DrawPlayer2DRadar (UCanvas* Canvas, APawn* Target, float PosX_, float PosY_, float Width, float Height)
{
	static float borderGap = 8.0f;

	if (!b2DRadar)
	{
		return;
	}
	
	FVector X,Y,Z,D;
	FRotator R = MyCameraRotation + FRotator(0, 16384, 0);
	R.Roll = 0;
	R.Pitch = 0;

	GetAxes(R, X, Y, Z);
	
	D = (Target->Location - MyCameraLocation);
	D.Z = 0;

	static float largestTargetD = 0.0f;

	if (D.Size2D() > largestTargetD)
	{
		largestTargetD = D.Size2D();
	}

	// Embrace for some High-School mathematics, eh!!
	
	float origPosX = PosX_ + Width / 2.0f; // center x
	float origPosY = PosY_ + Height / 2.0f; // center y

	float xRadarLength = (Width - 2.0f * borderGap) / 2 * (D.Size2D() / largestTargetD);
	float yRadarLength = (Height - 2.0f * borderGap) / 2 * (D.Size2D() / largestTargetD);


	float PosX = origPosX + Dot(D, X) / (D.Size2D()) * xRadarLength;
	float PosY = origPosY + Dot(D, Y) / (D.Size2D()) * yRadarLength;

	Canvas->Color = GetTeamColor(Target);

	DrawRec(Canvas, PosX, PosY, 4 * Scale, 4 * Scale, WhiteTexture, FColor(0,0,0));
	DrawRec(Canvas, PosX + 1, PosY + 1, 2 * Scale, 2 * Scale, WhiteTexture, Canvas->Color);
}
