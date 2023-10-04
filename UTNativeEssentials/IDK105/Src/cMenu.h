#pragma once
#ifndef _MENU_
#define _MENU_

class cMenu : public HookClass
{
public:
	cMenu();
	~cMenu();
    void DrawSettings (UCanvas*Canvas);
	void DrawBox (UCanvas* Canvas, float PosX, float PosY, float Width, float Height);
	void inline DrawRadarMenu (UCanvas* Canvas);
	void MyInfos (UCanvas* Canvas);
};

cMenu::cMenu() {};
cMenu::~cMenu(){};

#endif _MENU_

void cMenu::MyInfos (UCanvas* Canvas)
{
	static TCHAR StrTemp[MAX_PATH];

	appSnprintf(StrTemp, 40, L"Self Information");
	DrawText(Canvas, StrTemp, 28, 506, FColor(255, 255, 255));

	DrawRec(Canvas, +10, +521, 134, 2, WhiteTexture, FColor(180, 180, 180));

	appSnprintf(StrTemp, 40, L"%s", TEXT("Name:"));
	DrawText(Canvas, StrTemp, 15, 531, FColor (255, 255, 255, 255));

	appSnprintf(StrTemp, 40, L"%s", Me->PlayerReplicationInfo->PlayerName);
	DrawText(Canvas, StrTemp, 95, 531, FColor( 255,255,255,255));

	appSnprintf(StrTemp, 40, L"%s", TEXT("Kills:"));
	DrawText(Canvas, StrTemp, 15, 541, FColor (255, 255, 255, 255));

	appSnprintf(StrTemp, 40, L"%.0f", Me->PlayerReplicationInfo->Score);
	DrawText(Canvas, StrTemp, 95, 541, FColor( 255,255,255,255));

	appSnprintf(StrTemp, 40, L"%s", TEXT("Deaths:"));
	DrawText(Canvas, StrTemp, 15, 551, FColor (255, 255, 255, 255));

	appSnprintf(StrTemp, 40, L"%.0f", Me->PlayerReplicationInfo->Deaths);
	DrawText(Canvas, StrTemp, 95, 551, FColor( 255,255,255,255));

	appSnprintf(StrTemp, 40, L"%s", TEXT("Ping:"));
	DrawText(Canvas, StrTemp, 15, 561, FColor (255, 255, 255, 255));

	appSnprintf(StrTemp, 40, L"%d" ,Me->PlayerReplicationInfo->Ping);
	DrawText(Canvas, StrTemp, 95, 561, FColor( 255,255,255,255));

	appSnprintf(StrTemp, 40, L"%s", TEXT("ID:"));
	DrawText(Canvas, StrTemp, 15, 571, FColor (255, 255, 255, 255));

	appSnprintf(StrTemp, 40, L"%d",Me->PlayerReplicationInfo->PlayerID);
	DrawText(Canvas, StrTemp, 95, 571, FColor( 255,255,255,255));

	appSnprintf(StrTemp, 40, L"%s", TEXT("Health:"));
	DrawText(Canvas, StrTemp, 15, 591, FColor (255, 255, 255, 255));

	Hook.cR->DrawHealthBar(Canvas, 115, 613, Me->Health);

	appSnprintf(StrTemp, 40, L"[www.eatsleeput.com]", 0);
	DrawText(Canvas, StrTemp, 14, 625, FColor(180,180,180));
}

void cMenu::DrawSettings (UCanvas*Canvas)
{
	static TCHAR StrTemp[MAX_PATH];

	appSnprintf(StrTemp, 40, L"IDK-UnPleasentness", 0);
	DrawText(Canvas, StrTemp, 35, 345, FColor(255, 255, 255));

	appSnprintf(StrTemp, 40, L"[www.eatsleeput.com]", 0);
	DrawText(Canvas, StrTemp, 14, 464, FColor(180, 180, 180));

	DrawRec(Canvas, + 10, + 360, 134, 2, WhiteTexture, FColor(180, 180, 180));

	appSnprintf(StrTemp, 40, L"Aimbot:", 0);
	DrawText(Canvas,StrTemp,15,370,FColor(255, 255, 255));
	if (bAutoAim == 1)
	{
		appSnprintf(StrTemp, 40, L"[ON] - 1", 0);
		DrawText(Canvas, StrTemp, 95, 370, FColor(0, 255, 0));
	}
	else if (bAutoAim == 2)
	{
		appSnprintf(StrTemp, 40, L"[ON] - 2", 0);
		DrawText(Canvas, StrTemp, 95, 370, FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		DrawText(Canvas, StrTemp, 95, 370, FColor(255, 0, 0));
	}

	appSnprintf(StrTemp, 40, L"Autofire:", 0);
	DrawText(Canvas, StrTemp, 15, 380, FColor(255, 255, 255));
	if (bAutoFire)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		DrawText(Canvas, StrTemp, 95, 380, FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]",0);
		DrawText(Canvas, StrTemp, 95, 380, FColor(255, 0, 0));
	}

	appSnprintf(StrTemp, 40, L"Triggerbot:", 0);
	DrawText(Canvas, StrTemp, 15, 390, FColor(255,255,255));
	if (bTrigger)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		DrawText(Canvas, StrTemp, 95, 390,FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		DrawText(Canvas, StrTemp, 95, 390, FColor(255, 0, 0));
	}

	appSnprintf(StrTemp, 40, L"3DRadar:", 0);
	DrawText(Canvas, StrTemp, 15, 410, FColor(255, 255, 255));
	if (b3DRadar)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		DrawText(Canvas, StrTemp, 95, 410, FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		DrawText(Canvas, StrTemp, 95, 410, FColor(255, 0, 0));
	}

	appSnprintf(StrTemp, 40, L"2DRadar:", 0);
	DrawText(Canvas, StrTemp, 15, 420, FColor(255, 255, 255));
	if (b2DRadar)
	{
		appSnprintf(StrTemp, 40 ,L"[ON]", 0);
		DrawText(Canvas, StrTemp, 95, 420, FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		DrawText(Canvas, StrTemp, 95, 420, FColor(255, 0, 0));
	}

	appSnprintf(StrTemp, 40, L"Healthbars:", 0);
	DrawText(Canvas, StrTemp, 15, 430, FColor(255, 255, 255));
	if (bHealthbars)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		DrawText(Canvas, StrTemp, 95, 430, FColor(0, 255, 0));
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		DrawText(Canvas, StrTemp, 95, 430, FColor(255, 0, 0));
	}
}

void cMenu::DrawBox (UCanvas* Canvas, float PosX, float PosY, float Width, float Height )
{
	DrawRec(Canvas,PosX,PosY,Width,Height,WhiteTexture,FColor(67,75,86));

	DrawRec(Canvas,PosX-2,PosY-2,Width+3,2,WhiteTexture,FColor(0,0,0));
	DrawRec(Canvas,PosX+Width,PosY-2,2,Height+3,WhiteTexture,FColor(0,0,0));
	DrawRec(Canvas,PosX-2,PosY-2,2,Height+3,WhiteTexture,FColor(0,0,0));
	DrawRec(Canvas,PosX-2,PosY+Height,Width+3,2,WhiteTexture,FColor(0,0,0));
}

void inline cMenu::DrawRadarMenu (UCanvas* Canvas)
{
	DrawRec(Canvas, + 77, + 185, 1, 122, WhiteTexture, FColor(180,180,180));
	DrawRec(Canvas, + 18, + 246, 118, 1, WhiteTexture, FColor(180,180,180));
}