//==============================================================================================
//
//Morb!D-Windows by Morb!D
//1.1
//
//Unreal Tournament 2004 & Tactical Ops Crossfire version
//
//Feel free to Modivy all of this;)
//But as long as you use something out of this, give me some credits that all:P
//Have fun and read the ReadMe!
//
//Credits go 2:	
//		Helios      : for all his tutorials an everything he ever made;)
//		N4ps3r      : for his help whenever i need it
//		Cheatat0r   : for support thx m8;)
//		Cheaterelite: 1337
//		Daru		: For releasing his source(i used it as a base:P)
//
//===============================================================================================

#include "Hookmaster.h"

#pragma warning(disable:4700)
#pragma warning(disable:4554)
#pragma warning(disable:4101)

struct MorbidWindow
{
	float PosX, PosY, Width, Height;
	int Style;
	bool Translucent;

	TCHAR* Title;
};

struct MorbidButton
{
	float PosX, PosY, Width, Height;
	int Style;
	bool Translucent, Pressed, bOption;

	TCHAR* ButtonName;
};

struct MorbidCheckBox
{
	float PosX, PosY;
	bool bOption;

	TCHAR* Name;
};

class ColorClass
{
	public:
		FColor Black();
		FColor Red();
		FColor Green();
		FColor Blue();
		FColor White();
		FColor Yellow();
		FColor LightBlue();
		FColor Pink();
		FColor Gray();
		FColor DarkGray();
};

class Drawings
{
	public:
		void GetCanvas(UCanvas * Canvas);
		void Draw(float PosX, float PosY, float Width, float Height, FColor Color);
		void DrawTransparent(float PosX, float PosY, float Width, float Height, FColor Color);
		void Print(float PosX, float PosY, FColor Color, TCHAR * Text);
		void DrawOutSideBox(float PosX, float PosY, float Width, float Height);
		void CreateNewWindow(float PosX, float PosY, float Width, float Height, int Style, bool Translucent,TCHAR*Title);
		void CreateNewButton(float PosX, float PosY, float Width, float Height, bool bDown, bool Translucent, TCHAR*Title);
		void CreateNewStatic(float PosX, float PosY, float Width, float Height);
		void CreateNewCheckBox(float PosX, float PosY, bool bOn);
		void CreateNewMouseCursor (float &PosX, float &PosY);
};

class MouseCheck
{
	public:
		void UpdateMouse(FLOAT DeltaTime);
		bool CheckBoxClick(float PositionX, float PositionY, bool RightAlinedText);
		bool ButtonClick(float PositionX, float PositionY,float Width,float Height);
		bool InWindow(MorbidWindow Win);
};


class UnrealWindow
{
	public:
		void DrawStruct(MorbidWindow Window);
		void DrawWindow(MorbidWindow Window, float PositionX,float PositionY,float Width,float Height, int Style, bool Translucent, TCHAR*Title);
		void AddCheckBox(float PositionX, float PositionY, bool Option, bool bRightAlinedText, TCHAR * BoxName);
		void AddButton(float PositionX, float PositionY,float Width,float Height, TCHAR * ButtonName,bool Transparent);
		void AddStatic(float PositionX, float PositionY, float Width, float Height);
		void MakeShadowText(float PosX, float PosY, TCHAR * Text, FColor Color,UFont * Font);
		void Init();
		void MorbidWindowsMain(UCanvas * Canvas);
		void DrawText(float PosX, float PosY, TCHAR * Text, FColor FontColor,UFont * Font);
		void CreateMouse();
		void DrawRadarBox(float PositionX,float PositionY,float Width,float Height, bool Translucent);
	private:
		UFont* InitializeFont (const TCHAR* Name);
		UTexture* InitializeTexture (const TCHAR* Name);
		void DisableMouseFire (void);
		void EnableMouseFire (void);
		void CheckKeys ( void );
};

extern Drawings MorbidDraw;
extern ColorClass Color;
extern MouseCheck	Mouse;
extern UnrealWindow MWindow;

extern UTexture* WhiteTexture;
extern UTexture* MouseTexture;
extern UFont   * SmallFont;
extern float MyMouseX; 
extern float MyMouseY;

extern bool bMenuVisible;
