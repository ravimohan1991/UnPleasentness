//==============================================================================================
//
//Morb!D-Windows by Morb!D
//1.0
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


//==============================
//Some Definitions
//==============================

UCanvas * GC;

UTexture* WhiteTexture;
UTexture* MouseTexture;
UFont	* SmallFont;
UFont	* MediumFont;
UFont	* BigFont;
float	  MyMouseX; 
float	  MyMouseY;
bool      bMenuVisible=false;

#define MySetPos(PosX,PosY) \
	GC->CurX = PosX; \
	GC->CurY = PosY; \

#define MyResetCanvas \
	GC->SpaceX=0; \
	GC->SpaceY=0; \
	GC->OrgX=0; \
	GC->OrgY=0; \
	GC->CurX=0; \
	GC->CurY=0; \

//==============================
//Class Definitions
//==============================

Drawings MorbidDraw;
ColorClass Color;
MouseCheck	Mouse;
UnrealWindow MWindow;

//==============================
//Colors
//==============================

FColor ColorClass:: Black()
{
	return FColor(0,0,0);
}
FColor ColorClass::Red()
{
	return FColor(255,0,0);
}
FColor ColorClass::Green()
{
	return FColor(0,255,0);
}
FColor ColorClass::Blue()
{
	return FColor(0,0,255);
}
FColor ColorClass::White()
{
	return FColor(255,255,255);
}
FColor ColorClass::Yellow()
{
	return FColor(255,255,0);
}
FColor ColorClass::LightBlue()
{
	return FColor(0,255,255);
}
FColor ColorClass::Pink()
{
	return FColor(255,0,255);
}
FColor ColorClass::Gray()
{
	return FColor(92,92,96);
}
FColor ColorClass::DarkGray()
{
	return FColor(32,32,36);
}

//==============================
//Drawings
//==============================

void Drawings::GetCanvas(UCanvas * Canvas)
{
	GC = Canvas;
}

void Drawings::Draw(float PosX, float PosY, float Width, float Height, FColor Color)
{
	GC->DrawIcon(WhiteTexture, PosX, PosY,Width,Height, NULL, 1, Color.Plane(), FPlane(1,1,1,1),0);
}

void Drawings::DrawTransparent(float PosX, float PosY, float Width, float Height, FColor Color)
{
	GC->DrawIcon(WhiteTexture, PosX, PosY,Width,Height, NULL, 1, Color.Plane(), FPlane(1,1,1,1),PF_Translucent);
}

void Drawings::Print(float PosX, float PosY, FColor Color, const TCHAR * Text)
{
	FColor TempColor = GC->Color;

	MySetPos(PosX, PosY)
	GC->Color = Color;
	GC->WrappedPrintf(SmallFont, 0,Text);

	GC->Color = TempColor;

	MyResetCanvas;
}

void Drawings::DrawOutSideBox(float PosX, float PosY, float Width, float Height)
{
	//white border
	MorbidDraw.Draw(PosX,PosY,Width-1,1,Color.White());
	MorbidDraw.Draw(PosX,PosY,1,Height,Color.White());
	//black border
	MorbidDraw.Draw(PosX,PosY+Height,Width,1,Color.Black());
	MorbidDraw.Draw(PosX+Width-1,PosY,1,Height,Color.Black());
}


void Drawings::CreateNewWindow(float PosX, float PosY, float Width, float Height, int Style, bool Translucent, const TCHAR*Title)
{
	//OutsideBox
	MorbidDraw.DrawOutSideBox(PosX,PosY,Width,Height);

	//Darkgray Border
	MorbidDraw.Draw(PosX+1,PosY+1,2,Height-1,Color.DarkGray());
	MorbidDraw.Draw(PosX+1,PosY+1,Width-4, 2,Color.DarkGray());
	MorbidDraw.Draw(PosX+1+Width-4,PosY+1,2,Height-1,Color.DarkGray());
	MorbidDraw.Draw(PosX+1,PosY+1+Height-3,Width-4, 2,Color.DarkGray());
	//TitleBox
	if(Style==1)
	{
		MorbidDraw.Draw(PosX+3,PosY+3,Width-6,16,Color.White());
		//Blackline under the title box
		MorbidDraw.Draw(PosX+3,PosY+19,Width-6,1,Color.Black());
		//Title
		MorbidDraw.Print(PosX+10,PosY+7,Color.Black(),Title);
		//Darkgray
		MorbidDraw.Draw(PosX+3,PosY+20,Width-4, 2,Color.DarkGray());
	}
	else if(Style==2)
	{
		//Borders
		MorbidDraw.Draw(PosX+3,PosY+3,Width-4,1,Color.Black());
	}

	//MainWindow
	if(Style==1)
	{
		if(!Translucent)
		{
			MorbidDraw.Draw(PosX+3,PosY+20,Width-6,Height-22,Color.Gray());
		}
		else
		{
			MorbidDraw.DrawTransparent(PosX+3,PosY+20,Width-6,Height-22,Color.Gray());
		}
		//Borders
		//-up
		MorbidDraw.Draw(PosX+3,PosY+22,Width-6,1,Color.Black());
		//|left
		MorbidDraw.Draw(PosX+3,PosY+22,1,Height-25,Color.Black());
		//|right
		MorbidDraw.Draw(PosX+Width-4,PosY+22,1,Height-25,Color.Black());
		//_Down
		MorbidDraw.Draw(PosX+3,PosY+Height-3,Width-6,1,Color.Black());
	}
	else if(Style==2)
	{
		if(!Translucent)
		{
			MorbidDraw.Draw(PosX+3,PosY+3,Width-6,Height-6,Color.Gray());
		}
		else
		{
			MorbidDraw.DrawTransparent(PosX+3,PosY+3,Width-6,Height-6,Color.Gray());
		}
	}
}

void Drawings::CreateNewButton(float PosX, float PosY, float Width, float Height, bool bDown, bool Translucent, const TCHAR*Title)
{
	if(!Translucent)
	{
		if(!bDown)
		{
			//OutsideBox
			MorbidDraw.DrawOutSideBox(PosX,PosY,Width,Height);

			MorbidDraw.Draw(PosX+1,PosY+1,Width-2,Height-1,Color.Gray());
			MorbidDraw.Print(PosX+13,PosY+((Height/2)-3),Color.Black(),Title);
		}
		else
		{
			//Static
			MorbidDraw.CreateNewStatic(PosX,PosY,Width,Height);

			MorbidDraw.Draw(PosX+1,PosY+1,Width-2,Height-1,Color.DarkGray());
			MorbidDraw.Print(PosX+13,PosY+((Height/2)-3),Color.White(),Title);
		}
	}
	else
	{
		if(!bDown)
		{
			//OutsideBox
			MorbidDraw.DrawOutSideBox(PosX,PosY,Width,Height);

			MorbidDraw.DrawTransparent(PosX+1,PosY+1,Width-1,Height-2,Color.Gray());
			MorbidDraw.Print(PosX+13,PosY+((Height/2)-3),Color.Black(),Title);
		}
		else
		{
			//Static
			MorbidDraw.CreateNewStatic(PosX,PosY,Width,Height);

			MorbidDraw.DrawTransparent(PosX+1,PosY+1,Width-1,Height-2,Color.DarkGray());
			MorbidDraw.Print(PosX+13,PosY+((Height/2)-3),Color.White(),Title);
		}
	}
}

void Drawings::CreateNewStatic(float PosX, float PosY, float Width, float Height)
{
	MorbidDraw.Draw(PosX,PosY,Width-1,1,Color.Black());
	MorbidDraw.Draw(PosX,PosY,1,Height,Color.Black());
	MorbidDraw.Draw(PosX,PosY+Height,Width,1,Color.Black());
	MorbidDraw.Draw(PosX+Width-1,PosY,1,Height,Color.Black());
}

void Drawings::CreateNewCheckBox(float PosX, float PosY,bool bOn)
{
	//OutsideBox
	MorbidDraw.CreateNewStatic(PosX,PosY,10,10);
	
	//White
	MorbidDraw.Draw(PosX+1,PosY+1,8,8,Color.White());

	if(bOn)
	{
		MorbidDraw.Draw(PosX+2,PosY+2,6,6,Color.Black());
	}
}

void Drawings::CreateNewMouseCursor (float &PosX, float &PosY)
{
	if ( PosX < 0 ) PosX = 0;
	if ( PosX > GC->ClipX) PosX = GC->ClipX;

	if ( PosY < 0 ) PosY = 0;
	if ( PosY > GC->ClipY) PosY = GC->ClipY;

	if (GetAsyncKeyState(VK_LBUTTON) < 0)
	{
		GC->DrawIcon(MouseTexture, PosX , PosY , 32, 32, NULL, 1, Color.Red().Plane(), FPlane(1,1,1,1),0);
	}
	else
	{
		GC->DrawIcon(MouseTexture, PosX , PosY , 32, 32, NULL, 1, Color.Gray().Plane(), FPlane(1,1,1,1),0);
	}
}

//==============================
//Mouse
//==============================

void MouseCheck::UpdateMouse(FLOAT DeltaTime)
{
	if(bMenuVisible)
	{
		MyMouseX += Me->aMouseX * 0.01;
		MyMouseY -= Me->aMouseY * 0.01;
		Me->aMouseX = 0;
		Me->aMouseY = 0;
	}
}

bool MouseCheck::CheckBoxClick(float PositionX, float PositionY, bool RightAlinedText)
{
	if(GetAsyncKeyState(VK_LBUTTON) < 0)
	{
		if(RightAlinedText)
		{
			if( ((MyMouseX >= PositionX) && (MyMouseX <= PositionX+10)) 
			&&	((MyMouseY >= PositionY) && (MyMouseY <= PositionY+10)) )
			{
				Sleep(100);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if( ((MyMouseX >= PositionX+65) && (MyMouseX <= PositionX+75)) 
			&&	((MyMouseY >= PositionY+ 1) && (MyMouseY <= PositionY+11)) )
			{
				Sleep(100);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool MouseCheck::ButtonClick(float PositionX, float PositionY,float Width,float Height)
{
	if(GetAsyncKeyState(VK_LBUTTON) < 0)
	{
		if( ((MyMouseX >= PositionX) && (MyMouseX <= PositionX+Width)) 
		&&	((MyMouseY >= PositionY) && (MyMouseY <= PositionY+Height)) )
		{
			Sleep(100);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool MouseCheck::InWindow(MorbidWindow Win)
{
	if( ((MyMouseX >= Win.PosX) && (MyMouseX <= Win.PosX+Win.Width)) 
	&&	((MyMouseY >= Win.PosY) && (MyMouseY <= Win.PosY+Win.Height)) )
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}


//==============================
//UWindow
//==============================

void UnrealWindow::DrawWindow(MorbidWindow Window, float PositionX,float PositionY,float Width,float Height, int Style, bool Translucent, const TCHAR*Title)
{
	Window.PosX				= PositionX;
	Window.PosY				= PositionY;
	Window.Width			= Width;
	Window.Height			= Height;
	Window.Style			= Style;
	Window.Translucent		= Translucent;
	Window.Title			= Title;

	MorbidDraw.CreateNewWindow(PositionX,PositionY,Width,Height,Style,Translucent,Title);
}

void UnrealWindow::DrawRadarBox(float PositionX,float PositionY,float Width,float Height, bool Translucent)
{
	MorbidDraw.CreateNewWindow(PositionX,PositionY,Width,Height,2,Translucent,NULL);
	MorbidDraw.Draw(PositionX+3,PositionY+(Height/2),Width-6,1,Color.Black());
	MorbidDraw.Draw(PositionX+(Width/2),PositionY+3,1,Height-6,Color.Black());
}

void UnrealWindow::DrawStruct(MorbidWindow Window)
{
	MorbidDraw.CreateNewWindow(Window.PosX,Window.PosY,Window.Width,Window.Height,Window.Style,Window.Translucent,Window.Title);
}

void UnrealWindow::AddCheckBox(float PositionX, float PositionY, bool Option, bool bRightAlinedText, const TCHAR * BoxName)
{
	if(bRightAlinedText)
	{
		MorbidDraw.CreateNewCheckBox(PositionX,PositionY,Option);
		MorbidDraw.Print(PositionX+14,PositionY+1,Color.Black(),BoxName);
	}
	else
	{
		MorbidDraw.Print(PositionX,PositionY+1,Color.Black(),BoxName);
		MorbidDraw.CreateNewCheckBox(PositionX+65,PositionY,Option);
	}
}

void UnrealWindow::AddButton(float PositionX, float PositionY,float Width,float Height, const TCHAR * ButtonName,bool Transparent)
{
	MorbidDraw.CreateNewButton(PositionX,PositionY,Width,Height,false,Transparent,ButtonName);
}

void UnrealWindow::AddStatic(float PositionX, float PositionY, float Width, float Height)
{
	MorbidDraw.CreateNewStatic(PositionX,PositionY,Width,Height);
}

UTexture* UnrealWindow::InitializeTexture (const TCHAR* Name)
{
	UTexture* Tmp = (UTexture*)UTexture::StaticLoadObject( UTexture::StaticClass(), NULL, Name, NULL, LOAD_NoWarn, NULL );
	if ( Tmp != NULL) Tmp->SetFlags(RF_Standalone);

	return Tmp;
}

UFont* UnrealWindow::InitializeFont (const TCHAR* Name)
{
	UFont* Tmp = (UFont*)UFont::StaticLoadObject( UFont::StaticClass(), NULL, Name, NULL, LOAD_NoWarn, NULL );
	if ( Tmp != NULL) Tmp->SetFlags(RF_Standalone);

	return Tmp;
}

bool bInit=false;

void UnrealWindow::Init()
{
	if(!bInit)
	{
		bInit=true;
		WhiteTexture	= MWindow.InitializeTexture(TEXT("UWindow.WhiteTexture"));
		MouseTexture	= MWindow.InitializeTexture(TEXT("UWindow.MouseCursor"));
		BigFont			= MWindow.InitializeFont(TEXT("LadderFonts.UTLadder30"));
		SmallFont		= MWindow.InitializeFont(TEXT("Engine.SmallFont"));
		MediumFont		= MWindow.InitializeFont(TEXT("Engine.MedFont"));
	}
}

void UnrealWindow::MakeShadowText(float PosX, float PosY, const TCHAR * Text, FColor FontColor,UFont * Font)
{
	FColor TempColor = GC->Color;
	GC->Color = Color.Black();

	MySetPos(PosX-1, PosY)
	GC->WrappedPrintf(Font,0,L"%s",Text);

	MySetPos(PosX+1, PosY)
	GC->WrappedPrintf(Font,0,L"%s",Text);

	MySetPos(PosX, PosY-1)
	GC->WrappedPrintf(Font,0,L"%s",Text);

	MySetPos(PosX, PosY+1)
	GC->WrappedPrintf(Font,0,L"%s",Text);


	MySetPos(PosX, PosY)
	GC->Color = FontColor;
	GC->WrappedPrintf(Font,0,L"%s",Text);
	GC->Color = TempColor;
}

void UnrealWindow::DrawText(float PosX, float PosY, TCHAR * Text, FColor FontColor,UFont * Font)
{
	FColor TempColor = GC->Color;

	MySetPos(PosX, PosY)
	GC->Color = FontColor;
	GC->WrappedPrintf(Font,0,L"%s",Text);
	GC->Color = TempColor;
}


void UnrealWindow::DisableMouseFire (void)
{
	UViewport* ViewPort = (UViewport*)Me->Player;

	ViewPort->Input->Bindings[IK_LeftMouse] = TEXT(" ");
	ViewPort->Input->Bindings[IK_RightMouse] = TEXT(" ");
}

void UnrealWindow::EnableMouseFire (void)
{
	UViewport* ViewPort = (UViewport*)Me->Player;

	ViewPort->Input->Bindings[IK_LeftMouse] = TEXT("Fire");
	ViewPort->Input->Bindings[IK_RightMouse] = TEXT("AltFire");
}

void UnrealWindow::CheckKeys ( void )
{
	if(GetAsyncKeyState( VK_INSERT  ) < 0 )
	{
		bMenuVisible = !bMenuVisible;

		if ( bMenuVisible )
			DisableMouseFire();
		else
			EnableMouseFire();
		Sleep(100);
	}
	if(GetAsyncKeyState( VK_ESCAPE  ) < 0 )
	{
		bMenuVisible = false;
		EnableMouseFire();
		Sleep(100);
	}
}

void UnrealWindow::CreateMouse()
{
	MorbidDraw.CreateNewMouseCursor(MyMouseX,MyMouseY);
}

void UnrealWindow::MorbidWindowsMain(UCanvas * Canvas)
{
	MWindow.Init();
	MorbidDraw.GetCanvas(Canvas);
	MWindow.CheckKeys();
}






