/**
 * @file cDynamicFont.h
 * @author The_Cowboy
 * @brief This file contains dynamic fonts tailored for ut99 aimbots
 * @version 1.0
 * @date October 8, 2023
 * 
 * Thanks to Anthrax and "external" ut99 patch team
 *
 * @copyleft Entities of ut99.org and eatsleeput.com */

#include <vector>

#define LoadFont(Font,Name) \
if (Font == NULL) \
{ \
	Font = (UFont*)UFont::StaticLoadObject(UFont::StaticClass(), NULL, TEXT(Name), NULL, LOAD_NoWarn, NULL); \
	if (Font != NULL) Font->SetFlags(RF_Keep); \
} \

static class UFont* SmallFont = NULL;

bool bCacheSetup = false;
bool bEnableInGameScaling = false;
bool bCurrentInGameScaling;
float CurrentInGameScalingFactor;


struct FontCache
{
	UFont* Font;
	float MinSize;
	float MaxSize;
};
std::vector<FontCache> CachedFonts;

UFont* SavedFont[7];
float SavedWidth[7];


void AddNewFontCache(UFont* Font, float MinSize, float MaxSize)
{
	INT i;
	FontCache NewCache;

	if (Font == NULL || !bCacheSetup)
	{
		return;
	}

	NewCache.Font = Font;
	NewCache.MinSize = MinSize;
	NewCache.MaxSize = MaxSize;

	i = CachedFonts.size();

	CachedFonts.push_back(NewCache);
}


void SetScalingState(bool bEnableScaling = true)
{
	INT i;

	if (bEnableScaling)
	{
		if (!bCacheSetup)
		{
			UFont* tempoFont = NULL;
			bCacheSetup = true;

			LoadFont(tempoFont, "LadderFonts.UTLadder8");
			AddNewFontCache(tempoFont, 7, 10);
			LoadFont(tempoFont, "LadderFonts.UTLadder10");
			AddNewFontCache(tempoFont, 7, 10);
			LoadFont(tempoFont, "LadderFonts.UTLadder12");
			AddNewFontCache(tempoFont, 10, 12);
			LoadFont(tempoFont, "LadderFonts.UTLadder14");
			AddNewFontCache(tempoFont, 12, 14);
			LoadFont(tempoFont, "LadderFonts.UTLadder16");
			AddNewFontCache(tempoFont, 14, 16);
			LoadFont(tempoFont, "LadderFonts.UTLadder18");
			AddNewFontCache(tempoFont, 16, 20);
			LoadFont(tempoFont, "LadderFonts.UTLadder20");
			AddNewFontCache(tempoFont, 20, 22);
			LoadFont(tempoFont, "LadderFonts.UTLadder22");
			AddNewFontCache(tempoFont, 22, 24);
			LoadFont(tempoFont, "LadderFonts.UTLadder24");
			AddNewFontCache(tempoFont, 24, 30);
			LoadFont(tempoFont, "LadderFonts.UTLadder30");
			AddNewFontCache(tempoFont, 30, 36);
			LoadFont(tempoFont, "LadderFonts.UTLadder36");
			AddNewFontCache(tempoFont, 36, 42);
			LoadFont(tempoFont, "LadderFonts.UTLadder42");
			AddNewFontCache(tempoFont, 42, 48);
			LoadFont(tempoFont, "LadderFonts.UTLadder48");
			AddNewFontCache(tempoFont, 48, 54);
			LoadFont(tempoFont, "LadderFonts.UTLadder54");
			AddNewFontCache(tempoFont, 54, 60);
			LoadFont(tempoFont, "LadderFonts.UTLadder60");
			AddNewFontCache(tempoFont, 60, 85);
		}
	}

	// Reset resolution caches to make bEnableInGameScaling changes effective immediately
	for (i = 0; i < 7; ++i)
	{
		SavedFont[i] = NULL;
		SavedWidth[i] = 0;
	}

	bEnableInGameScaling = bEnableScaling;
	bCurrentInGameScaling = bEnableScaling;
	//CurrentInGameScalingFactor = InGameScalingFactor;
}

/*---------------- (469) Dynamically create fonts and cache them -----------------*/

//Precache fonts if font scaling is enabled at game start
void PrecacheFonts()
{
	SetScalingState();
}

UFont* GetFontBySize(float IdealSize)
{
	INT i;
	float NewBase;

	UFont* NewFont;

	// Don't blow up the dynamic font cache
	IdealSize = fClamp(Abs(IdealSize) * InGameScalingFactor, 6, 100);

	for(i = 0; i < CachedFonts.size(); i++)
	{
		if (CachedFonts[i].Font == NULL)
		{
			CachedFonts.Remove(i--);
			continue;
		}
		if ((IdealSize >= CachedFonts[i].MinSize) && (IdealSize <= CachedFonts[i].MaxSize))
			return CachedFonts[i].Font;
	}

	// Dynamically created fonts will scale up using 'Square(X)' where X increments by 0.5
	NewBase = Sqrt(IdealSize);
	NewBase = NewBase - (NewBase % 0.5);
	NewFont = class'Canvas'.static.CreateFont(FF_Arial, int(Square(NewBase)), false, false, false, false, true);

	// Font could not be created, choose latest in cache
	if (NewFont == None)
		return CachedFonts[CachedFonts.Length - 1].Font;

	AddNewFontCache(NewFont, Square(NewBase), Square(NewBase + 0.5));
	return NewFont;
}


UFont* GetFontIndex(int i, float Width)
{

	if ((SavedFont[i] != NULL) && (Width == SavedWidth[i]))
	{
		return SavedFont[i];
	}

	SavedWidth[i] = Width;

	if (bEnableInGameScaling)
	{
		//In lower resolutions fonts need to be a bit bigger
		SavedFont[i] = GetFontBySize((Width + (6 - i) * 32) / FontDiv[i]);
	}
	else
	{
		switch (i)
		{
		case 0:
			SavedFont[0] = GetStaticACompletelyUnreadableFont(Width);
			break;
		case 1:
			SavedFont[1] = GetStaticAReallySmallFont(Width);
			break;
		case 2:
			SavedFont[2] = GetStaticSmallestFont(Width);
			break;
		case 3:
			SavedFont[3] = GetStaticSmallFont(Width);
			break;
		case 4:
			SavedFont[4] = GetStaticMediumFont(Width);
			break;
		case 5:
			SavedFont[5] = GetStaticBigFont(Width);
			break;
		case 6:
			SavedFont[6] = GetStaticHugeFont(Width);
			break;
		}
	}
	return SavedFont[i];
}


/*------------------------------- Font selector ----------------------------------*/

UFont* GetHugeFont(float Width)
{
	return GetFontIndex(6, Width);
}

UFont* GetBigFont(float Width)
{
	return GetFontIndex(5, Width);
}

UFont* GetMediumFont(float Width)
{
	return GetFontIndex(4, Width);
}

UFont* GetSmallFont(float Width)
{
	return GetFontIndex(3, Width);
}

UFont* GetSmallestFont(float Width)
{
	return GetFontIndex(2, Width);
}

UFont* GetAReallySmallFont(float Width)
{
	return GetFontIndex(1, Width);
}

UFont* GetACompletelyUnreadableFont(float Width)
{
	return GetFontIndex(0, Width);
}