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
	void inline DrawPlayer2DRadar (UCanvas* Canvas, APawn* Target);
	void inline DrawHealthBar (UCanvas* Canvas, float PosX, float PosY, int Health);
};

cRadar::cRadar() {};
cRadar::~cRadar(){};

#endif _RADAR_

void inline cRadar::DrawHealthBar (UCanvas* Canvas, float PosX, float PosY, int Health)
{
	DrawRec(Canvas, PosX - 20, PosY - 20, 40, 6, WhiteTexture, FColor(0,0,0,0));
	DrawRec(Canvas,PosX - 19, PosY - 19, 38, 4, WhiteTexture, FColor(255,255,255,255));

	int Tmp = Min(int(Health * 2.55), 255);
	Canvas->Color = FColor(255 - Tmp,Tmp,0);
	Tmp = Min(int(Health * 0.38), 38);

	DrawRec(Canvas, PosX - 19, PosY - 19, Tmp, 4, WhiteTexture, Canvas->Color);
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

	if(Dot(E,X) <= cos(90 * PI / 180)) 
	     return;
 
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

	WhiteTexture->MinFrameRate = 35;

	Canvas->Color = GetTeamColor(Target);
	DrawRec(Canvas, Left, Top,           1, Bot-Top, WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Left, Bot,1+Right-Left,       1, WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Right, Top,           1, Bot-Top,WhiteTexture, Canvas->Color);
	DrawRec(Canvas, Left, Top,  Right-Left,       1, WhiteTexture, Canvas->Color);

	F = Target->Location - MyCameraLocation;

	appSprintf(StrTemp,L"%s",Target->PlayerReplicationInfo->PlayerName);
	DrawText(Canvas,StrTemp,Right + 3,Top,GetTeamColor(Target));

	appSprintf(StrTemp,L"H:%d",Target->Health);
	DrawText(Canvas,StrTemp,Right + 3,Top + 8,GetTeamColor(Target));

    appSprintf(StrTemp, TEXT("D:%.0f"),F.Size() / 48);
	DrawText(Canvas,StrTemp,Right + 3,Top + 16,GetTeamColor(Target));

	if (bHealthbars)
	     DrawHealthBar(Canvas, top.X, Top + 10, Target->Health);
}

void cRadar::DrawPlayerOnRadar (UCanvas* Canvas, APawn *Target)
{
    if ( !b3DRadar ) return;

	FVector X,Y,Z,D,E,top,bottom;;
	FColor Color = GetTeamColor(Target);

	GetAxes(MyCameraRotation,X,Y,Z);

	D = Target->Location - MyCameraLocation;
	E = D /= D.Size();

	if(Dot(E,X) <= cos(90 * PI / 180))
	    return;

	DrawBoundingBox(Canvas,Target);
}

void inline cRadar::DrawPlayer2DRadar (UCanvas* Canvas, APawn* Target)
{
    if ( !b2DRadar ) return;

    FVector X,Y,Z,D;
    FRotator R = MyCameraRotation + FRotator(0,16384,0);
    R.Roll = 0;
    R.Pitch = 0;
    GetAxes(R, X, Y, Z);
    D = (Target->Location - MyCameraLocation);
    D.Z = 0;

    int origPosX = + 75, origPosY = + 245;

	int PosX = origPosX + Dot(D, X) / ((60 * 48) / 60);
	int PosY = origPosY + Dot(D, Y) / ((60 * 48) / 60);

	if (PosX < + 17)
		PosX = + 17;
	if (PosX > + 133)
		PosX = + 133;
	if (PosY < + 185)
		PosY = + 185;
	if (PosY > + 303)
		PosY = + 303;

	Canvas->Color = GetTeamColor(Target);

	DrawRec(Canvas,PosX,PosY, 4, 4, WhiteTexture,FColor(0,0,0));
	DrawRec(Canvas,PosX + 1, PosY + 1, 2, 2, WhiteTexture,Canvas->Color);
}