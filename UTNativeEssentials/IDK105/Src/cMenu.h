#pragma once
#ifndef _MENU_
#define _MENU_

#include "main.h"

class cMenu : public HookClass
{
public:
	cMenu();
	~cMenu();
    void DrawSettings (UCanvas*Canvas, float PosX, float PosY, float Width, float Height);
	void DrawBox (UCanvas* Canvas, float PosX, float PosY, float Width, float Height);
	void inline DrawRadarMenu (UCanvas* Canvas, float PosX, float PosY, float Width, float Height);
	void MyInfos (UCanvas* Canvas, float PosX, float PosY, float Width, float Height);
};

cMenu::cMenu() {};
cMenu::~cMenu(){};

#endif _MENU_

void cMenu::MyInfos (UCanvas* Canvas, float PosX, float PosY, float Width, float Height)
{
	static wchar_t StrTemp[MAX_PATH];

	INT XL = 0;
	INT YL = 0;
	float TextMargin = 5.0f / 134.0f * Width * 2.0f;

	float Ycounter = PosY + 5;

	UFont* theFont = SmallFont;

	Canvas->Font = SmallFont;
	Canvas->WrappedStrLenf(SmallFont, XL, YL, L"Dummy");// weird XL, YL appear if this trial is not done, heh

	appSnprintf(StrTemp, 40, L"Self Information");
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	DrawMyText(Canvas, StrTemp, PosX + Width / 2.0f - (float)XL / 2.0f, Ycounter, FColor(255, 255, 255), theFont);
	Ycounter += YL;

	DrawRec(Canvas, PosX, Ycounter, Width, 2, WhiteTexture, FColor(180, 180, 180));
	Ycounter += 4;

	appSnprintf(StrTemp, 40, L"%s", TEXT("Name:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	appSnprintf(StrTemp, 40, L"%s", Me->PlayerReplicationInfo->PlayerName);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 255, 255, 255), theFont);

	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"%s", TEXT("Kills:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	appSnprintf(StrTemp, 40, L"%.0f", Me->PlayerReplicationInfo->Score);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 255, 255, 255), theFont);

	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"%s", TEXT("Deaths:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	appSnprintf(StrTemp, 40, L"%.0f", Me->PlayerReplicationInfo->Deaths);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 255, 255, 255), theFont);

	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"%s", TEXT("Ping:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	appSnprintf(StrTemp, 40, L"%d" ,Me->PlayerReplicationInfo->Ping);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 255, 255, 255), theFont);

	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"%s", TEXT("ID:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	appSnprintf(StrTemp, 40, L"%d",Me->PlayerReplicationInfo->PlayerID);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 255, 255, 255), theFont);

	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"%s", TEXT("Health:"));
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	Hook.cR->DrawHealthBar(Canvas, PosX + Width - TextMargin, Ycounter + YL, Me->Health);

	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	appSnprintf(StrTemp, 40, L"[www.eatsleeput.com]", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	DrawMyText(Canvas, StrTemp, PosX + Width / 2.0f - XL / 2.0f, PosY + Height - YL, FColor(180, 180, 180), theFont);
}

void cMenu::DrawSettings (UCanvas* Canvas, float PosX, float PosY, float Width, float Height)
{
	static wchar_t StrTemp[MAX_PATH];

	INT XL = 0;
	INT YL = 0;
	float TextMargin = 5.0f / 134.0f * Width * 2.0f;

	float Ycounter = PosY + 5;

	UFont* theFont = SmallFont;

	Canvas->Font = SmallFont;
	Canvas->WrappedStrLenf(SmallFont, XL, YL, L"Dummy");// weird XL, YL appear if this trial is not done, heh

	appSnprintf(StrTemp, 40, L"IDK-UnPleasentness", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	DrawMyText(Canvas, StrTemp, PosX + Width / 2.0f - (float) XL / 2.0f, Ycounter, FColor(255, 255, 255), theFont);
	Ycounter += YL;

	appSnprintf(StrTemp, 40, L"[www.eatsleeput.com]", 0);

	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);
	DrawMyText(Canvas, StrTemp, PosX + Width / 2.0f - XL / 2.0f, PosY + Height - YL, FColor(180, 180, 180), theFont);

	DrawRec(Canvas, PosX, Ycounter, Width, 2, WhiteTexture, FColor(180, 180, 180));
	Ycounter += 4;

	appSnprintf(StrTemp, 40, L"Aimbot:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	if (bAutoAim == 1)
	{
		appSnprintf(StrTemp, 40, L"[ON] - 1", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else if (bAutoAim == 2)
	{
		appSnprintf(StrTemp, 40, L"[ON] - 2", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"Autofire:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);
	if (bAutoFire)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]",0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"Triggerbot:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	if (bTrigger)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"3DRadar:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	if (b3DRadar)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"2DRadar:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	if (b2DRadar)
	{
		appSnprintf(StrTemp, 40 ,L"[ON]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
	Ycounter += YL + 2;

	appSnprintf(StrTemp, 40, L"Healthbars:", 0);
	Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

	DrawMyText(Canvas, StrTemp, PosX + TextMargin, Ycounter, FColor(255, 255, 255), theFont);

	if (bHealthbars)
	{
		appSnprintf(StrTemp, 40, L"[ON]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(0, 255, 0), theFont);
	}
	else
	{
		appSnprintf(StrTemp, 40, L"[OFF]", 0);
		Canvas->WrappedStrLenf(SmallFont, XL, YL, StrTemp);

		DrawMyText(Canvas, StrTemp, PosX + Width - TextMargin - XL, Ycounter, FColor(255, 0, 0), theFont);
	}
}

void cMenu::DrawBox (UCanvas* Canvas, float PosX, float PosY, float Width, float Height)
{
	DrawRec(Canvas, PosX, PosY, Width, Height, WhiteTexture, FColor(67,75,86));

	DrawRec(Canvas, PosX-2, PosY-2,Width+3 ,2, WhiteTexture, FColor(0,0,0));
	DrawRec(Canvas, PosX+Width, PosY-2, 2, Height+3, WhiteTexture, FColor(0,0,0));
	DrawRec(Canvas, PosX-2, PosY-2,2,Height+3, WhiteTexture, FColor(0,0,0));
	DrawRec(Canvas, PosX-2, PosY+Height,Width+3, 2, WhiteTexture, FColor(0,0,0));
}

void inline cMenu::DrawRadarMenu (UCanvas* Canvas, float PosX, float PosY, float Width, float Height)
{
	// 10, 177, 134, 137

	static float borderGap = 8.0f;

	DrawRec(Canvas, PosX + Width / 2.0f, PosY + borderGap, 1, Height - 2.0f * borderGap, WhiteTexture, FColor(180, 180, 180));
	DrawRec(Canvas, PosX + borderGap, PosY + Height / 2.0f, Width - 2.0f * borderGap, 1, WhiteTexture, FColor(180, 180, 180));
}