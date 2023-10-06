class Hud extends Actor
    native
    config(user)
    transient
    exportstructs;

var() PlayerController PlayerOwner;
var() Pawn PawnOwner;
var() PlayerReplicationInfo PawnOwnerPRI;
var() Console PlayerConsole;

var() Scoreboard ScoreBoard;
var() Scoreboard LocalStatsScreen;

// mini hud-menus
var() Actor VoteMenu;		// hook for mod authors

var color WhiteColor, RedColor, GreenColor, CyanColor, BlueColor, GoldColor, PurpleColor, TurqColor, GrayColor;

var() bool bShowVoteMenu;
var() globalconfig bool bHideHUD;
var() bool bShowScoreBoard;             // Display current score-board instead of Hud elements
var() bool bShowDebugInfo;              // if true, show properties of current ViewTarget
var() bool bShowBadConnectionAlert;     // Display indication of bad connection
var() globalconfig bool bMessageBeep;
var() globalconfig bool bShowWeaponInfo;
var() globalconfig bool bShowPersonalInfo;
var() globalconfig bool bShowPoints;
var() globalconfig bool bShowWeaponBar;
var() globalconfig bool bCrosshairShow;
var() globalconfig bool bShowPortrait;
var bool bBuiltMOTD;	// Set to true when the MOTD has been set
var bool bShowLocalStats;

var() Color ConsoleColor;

var() localized string ProgressFontName;
var() Font ProgressFontFont;
var() localized string OverrideConsoleFontName;
var() Font OverrideConsoleFont;
var() float ProgressFadeTime;
var() Color MOTDColor;

var() globalconfig float HudScale;          // Global Scale for all widgets
var() globalconfig float HudOpacity;		// make everything transparent
var() globalconfig float HudCanvasScale;    // Specifies amount of screen-space to use (for TV's).
var() globalconfig int CrosshairStyle;
var() globalconfig float CrosshairScale;
var() globalconfig float CrosshairOpacity;
var() globalconfig color CrossHairColor;

var transient float ResScaleX, ResScaleY;
var globalconfig int ConsoleMessageCount;
var globalconfig int ConsoleFontSize;
var globalconfig int MessageFontOffset;

struct ConsoleMessage
{
	var string Text;
	var color TextColor;
	var float MessageLife;
	var PlayerReplicationInfo PRI;
};
var ConsoleMessage TextMessages[8];

var() float ConsoleMessagePosX, ConsoleMessagePosY; // DP_LowerLeft

var localized string FontArrayNames[9];
var Font FontArrayFonts[9];
var int FontScreenWidthMedium[9];
var int FontScreenWidthSmall[9];

var string MOTD[4];		// Holds the expanded MOTD Lines
var int MOTDState;

var globalconfig bool bNoEnemyNames;

/* Draw3DLine()
draw line in world space. Should be used when engine calls RenderWorldOverlays() event.
*/
native final function Draw3DLine(vector Start, vector End, color LineColor);

simulated function BuildMOTD()
{
	local int i,c,p;
    local string s,t[4];

    if (bBuiltMOTD || PlayerOwner==None || PlayerOwner.GameReplicationInfo==None)
    	return;

	bBuiltMOTD = true;
	PlayerOwner.SetProgressTime(6);
	c=0;
    t[0] = PlayerOwner.GameReplicationInfo.MOTDLine1;
    t[1] = PlayerOwner.GameReplicationInfo.MOTDLine2;
    t[2] = PlayerOwner.GameReplicationInfo.MOTDLine3;
    t[3] = PlayerOwner.GameReplicationInfo.MOTDLine4;

    for (i=0;i<4;i++)
    {
		p = instr(t[i],"|");
    	while(p>=0)
        {
         	s = left(t[i],p);
            MOTD[c] = s;
            c++;
            if (c==4)
            	return;

            t[i] = right(t[i],len(t[i])-p-1);
            p = instr(t[i],"|");


        }
        MOTD[c] = t[i];
        c++;
        if (c==4)
        	return;
	}
}


simulated event PostBeginPlay()
{
    Super.PostBeginPlay();
    LinkActors ();
    CreateKeyMenus();
}

/* Reset()
reset actor to initial state - used when restarting level without reloading.
*/
function Reset()
{
	bShowVoteMenu = false;
	bShowScoreboard = false;
	Super.Reset();
}

simulated function CreateKeyMenus();

simulated event Destroyed()
{
    if( ScoreBoard != None )
    {
        ScoreBoard.Destroy();
        ScoreBoard = None;
    }

    if( VoteMenu != None )
    {
        VoteMenu.Destroy();
        VoteMenu = None;
    }

    Super.Destroyed();
}


//=============================================================================
// Execs

/* toggles displaying scoreboard
*/
exec function ShowScores()
{
    bShowScoreboard = !bShowScoreboard;
}

exec function ShowStats()
{
	bShowLocalStats = !bShowLocalStats;
}

exec function NextStats()
{
	if ( LocalStatsScreen != None )
		LocalStatsScreen.NextStats();
}

/* toggles displaying properties of player's current viewtarget
*/
exec function ShowDebug()
{
    bShowDebugInfo = !bShowDebugInfo;
}

simulated event WorldSpaceOverlays()
{
    if ( bShowDebugInfo && Pawn(PlayerOwner.ViewTarget) != None )
        DrawRoute();
}

function CheckCountdown(GameReplicationInfo GRI);

event ConnectFailure(string FailCode, string URL)
{
	PlayerOwner.ReceiveLocalizedMessage(class'FailedConnect', class'FailedConnect'.Static.GetFailSwitch(FailCode));
}

function GetLocalStatsScreen();

simulated event PostRender( canvas Canvas )
{
    //local BWF_Base bb;
    local float XPos, YPos;
    local plane OldModulate,OM;

    BuildMOTD();

    OldModulate = Canvas.ColorModulate;

    Canvas.ColorModulate.X = 1;
    Canvas.ColorModulate.Y = 1;
    Canvas.ColorModulate.Z = 1;
    Canvas.ColorModulate.W = HudOpacity/255;

    LinkActors();

    ResScaleX = Canvas.SizeX / 640.0;
    ResScaleY = Canvas.SizeY / 480.0;

	CheckCountDown(PlayerOwner.GameReplicationInfo);

    if ( !PlayerOwner.bBehindView && (PawnOwner != None) )
    {
		if ( PlayerOwner.bDemoOwner || ((Level.NetMode == NM_Client) && (PlayerOwner.Pawn != PawnOwner)) )
			PawnOwner.GetDemoRecordingWeapon();
		else if ( PawnOwner.Weapon != None )
			PawnOwner.Weapon.RenderOverlays(Canvas);
	}
	
	if ( PawnOwner != None && PawnOwner.bSpecialHUD )
		PawnOwner.DrawHud(Canvas);
    if ( bShowDebugInfo )
    {
        Canvas.Font = GetConsoleFont(Canvas);
        Canvas.Style = ERenderStyle.STY_Alpha;
        Canvas.DrawColor = ConsoleColor;

        PlayerOwner.ViewTarget.DisplayDebug (Canvas, XPos, YPos);
    }
	else if( !bHideHud )
    {
        if ( bShowLocalStats )
        {
			if ( LocalStatsScreen == None )
				GetLocalStatsScreen();
            if ( LocalStatsScreen != None )
            {
            	OM = Canvas.ColorModulate;
                Canvas.ColorModulate = OldModulate;
                LocalStatsScreen.DrawScoreboard(Canvas);
				DisplayMessages(Canvas);
                Canvas.ColorModulate = OM;
			}
		}
        else if (bShowScoreBoard)
        {
            if (ScoreBoard != None)
            {
            	OM = Canvas.ColorModulate;
                Canvas.ColorModulate = OldModulate;
                ScoreBoard.DrawScoreboard(Canvas);
				if ( Scoreboard.bDisplayMessages )
					DisplayMessages(Canvas);
                Canvas.ColorModulate = OM;
			}
        }
        else
        {
			if ( (PlayerOwner == None) || (PawnOwner == None) || (PawnOwnerPRI == None) || (PlayerOwner.IsSpectating() && PlayerOwner.bBehindView) )
            	DrawSpectatingHud(Canvas);
			else if( !PawnOwner.bHideRegularHUD )
				DrawHud(Canvas);

            if (!DrawLevelAction (Canvas))
            {
            	if (PlayerOwner!=None)
                {
                	if (PlayerOwner.ProgressTimeOut > Level.TimeSeconds)
                    {
	                    DisplayProgressMessages (Canvas);
                    }
                    else if (MOTDState==1)
                    	MOTDState=2;
                }
           }

            if (bShowBadConnectionAlert)
                DisplayBadConnectionAlert (Canvas);
            DisplayMessages(Canvas);

        }

        if( bShowVoteMenu && VoteMenu!=None )
            VoteMenu.RenderOverlays(Canvas);
    }
    else if ( PawnOwner != None )
        DrawInstructionGfx(Canvas);


    PlayerOwner.RenderOverlays (Canvas);

    if ((PlayerConsole != None) && PlayerConsole.bTyping)
        DrawTypingPrompt(Canvas, PlayerConsole.TypedStr);

    Canvas.ColorModulate=OldModulate;

	if (	(PlayerOwner != None) && 
		(PlayerOwner.Focus != none) &&
		PlayerOwner.Focus.isA('BWF_Base')) 
	{
		BWF_Base(PlayerOwner.Focus).PostRender(Canvas);
	}
}

simulated function DrawInstructionGfx( Canvas C );
simulated function SetInstructionText( string text );
simulated function SetInstructionKeyText( string text );

simulated function DrawRoute()
{
    local int i;
    local Controller C;
    local vector Start, End, RealStart;;
    local bool bPath;

    C = Pawn(PlayerOwner.ViewTarget).Controller;
    if ( C == None )
        return;
    if ( C.CurrentPath != None )
        Start = C.CurrentPath.Start.Location;
    else
        Start = PlayerOwner.ViewTarget.Location;
    RealStart = Start;

    if ( C.bAdjusting )
    {
        Draw3DLine(C.Pawn.Location, C.AdjustLoc, class'Canvas'.Static.MakeColor(255,0,255));
        Start = C.AdjustLoc;
    }

    // show where pawn is going
    if ( (C == PlayerOwner)
        || (C.MoveTarget == C.RouteCache[0]) && (C.MoveTarget != None) )
    {
        if ( (C == PlayerOwner) && (C.Destination != vect(0,0,0)) )
        {
            if ( C.PointReachable(C.Destination) )
            {
                Draw3DLine(C.Pawn.Location, C.Destination, class'Canvas'.Static.MakeColor(255,255,255));
                return;
            }
            C.FindPathTo(C.Destination);
        }
        for ( i=0; i<16; i++ )
        {
            if ( C.RouteCache[i] == None )
                break;
            bPath = true;
            Draw3DLine(Start,C.RouteCache[i].Location,class'Canvas'.Static.MakeColor(0,255,0));
            Start = C.RouteCache[i].Location;
        }
        if ( bPath )
            Draw3DLine(RealStart,C.Destination,class'Canvas'.Static.MakeColor(255,255,255));
    }
    else if ( PlayerOwner.ViewTarget.Velocity != vect(0,0,0) )
        Draw3DLine(RealStart,C.Destination,class'Canvas'.Static.MakeColor(255,255,255));

    if ( C == PlayerOwner )
        return;

    // show where pawn is looking
    if ( C.Focus != None )
        End = C.Focus.Location;
    else
        End = C.FocalPoint;
    Draw3DLine(PlayerOwner.ViewTarget.Location + Pawn(PlayerOwner.ViewTarget).BaseEyeHeight * vect(0,0,1),End,class'Canvas'.Static.MakeColor(255,0,0));
}

simulated function DisplayProgressMessages (Canvas C)
{
    local int i, LineCount;
    local GameReplicationInfo GRI;
    local float FontDX, FontDY;
    local float X, Y;
    local int Alpha;
    local float TimeLeft;

    TimeLeft = PlayerOwner.ProgressTimeOut - Level.TimeSeconds;

    if( TimeLeft >= ProgressFadeTime )
        Alpha = 255;
    else
        Alpha = (255 * TimeLeft) / ProgressFadeTime;

    GRI = PlayerOwner.GameReplicationInfo;

    LineCount = 0;

    for (i = 0; i < ArrayCount (PlayerOwner.ProgressMessage); i++)
    {
        if (PlayerOwner.ProgressMessage[i] == "")
            continue;

        LineCount++;
    }

	if (bBuiltMOTD && MOTDState<2)
    {
    	if (MOTD[0] != "") LineCount++;
    	if (MOTD[1] != "") LineCount++;
    	if (MOTD[2] != "") LineCount++;
    	if (MOTD[3] != "") LineCount++;
    }


    C.Font = LoadProgressFont();

    C.Style = ERenderStyle.STY_Alpha;

    C.TextSize ("A", FontDX, FontDY);

    X = (0.5 * HudCanvasScale * C.SizeX) + (((1.0 - HudCanvasScale) / 2.0) * C.SizeX);
    Y = (0.5 * HudCanvasScale * C.SizeY) + (((1.0 - HudCanvasScale) / 2.0) * C.SizeY);

    Y -= FontDY * (float (LineCount) / 2.0);

    for (i = 0; i < ArrayCount (PlayerOwner.ProgressMessage); i++)
    {
        if (PlayerOwner.ProgressMessage[i] == "")
            continue;

        C.DrawColor = PlayerOwner.ProgressColor[i];
        C.DrawColor.A = Alpha;

        C.TextSize (PlayerOwner.ProgressMessage[i], FontDX, FontDY);
        C.SetPos (X - (FontDX / 2.0), Y);
        C.DrawText (PlayerOwner.ProgressMessage[i]);

        Y += FontDY;
    }

    if( (GRI != None) && (Level.NetMode != NM_StandAlone) && (MOTDState<2) )
    {
    	MOTDState=1;
        C.DrawColor = MOTDColor;
        C.DrawColor.A = Alpha;

        for (i=0;i<4;i++)
        {
            C.TextSize (MOTD[i], FontDX, FontDY);
            C.SetPos (X - (FontDX / 2.0), Y);
            C.DrawText (MOTD[i]);
            Y += FontDY;
		}

    }
}

function DrawHud (Canvas C);
function DrawSpectatingHud (Canvas C);
function bool DrawLevelAction (Canvas C);

/* DisplayBadConnectionAlert()
Warn user that net connection is bad
*/
function DisplayBadConnectionAlert (Canvas C);

simulated function LocalizedMessage( class<LocalMessage> Message, optional int Switch, optional PlayerReplicationInfo RelatedPRI_1, optional PlayerReplicationInfo RelatedPRI_2, optional Object OptionalObject, optional string CriticalString );

simulated function DrawTypingPrompt (Canvas C, String Text)
{
    local float XPos, YPos;
    local float XL, YL;

    C.Font = GetConsoleFont(C);
    C.Style = ERenderStyle.STY_Alpha;
    C.DrawColor = ConsoleColor;

    C.TextSize ("A", XL, YL);

    XPos = (ConsoleMessagePosX * HudCanvasScale * C.SizeX) + (((1.0 - HudCanvasScale) * 0.5) * C.SizeX);
    YPos = (ConsoleMessagePosY * HudCanvasScale * C.SizeY) + (((1.0 - HudCanvasScale) * 0.5) * C.SizeY) - YL;

    C.SetPos (XPos, YPos);
    C.DrawTextClipped ("(>"@Text$"_", false);
}

simulated function SetScoreBoardClass (class<Scoreboard> ScoreBoardClass)
{
    if (ScoreBoard != None )
        ScoreBoard.Destroy();

    if (ScoreBoardClass == None)
        ScoreBoard = None;
    else
    {
        ScoreBoard = Spawn (ScoreBoardClass, Owner);

        if (ScoreBoard == None)
            log ("Hud::SetScoreBoard(): Could not spawn a scoreboard of class "$ScoreBoardClass, 'Error');
    }
}

exec function ShowHud()
{
    bHideHud = !bHideHud;
}

simulated function LinkActors()
{
    PlayerOwner = PlayerController (Owner);

    if (PlayerOwner == None)
    {
        PlayerConsole = None;
        PawnOwner = None;
        PawnOwnerPRI = None;
        return;
    }

    if (PlayerOwner.Player != None)
        PlayerConsole = PlayerOwner.Player.Console;
    else
        PlayerConsole = None;

    if ( (PlayerOwner.ViewTarget != None) &&
        (Pawn(PlayerOwner.ViewTarget) != None) &&
        (Pawn(PlayerOwner.ViewTarget).Health > 0) )
        PawnOwner = Pawn(PlayerOwner.ViewTarget);
    else if (PlayerOwner.Pawn != None )
        PawnOwner = PlayerOwner.Pawn;
    else
        PawnOwner = None;

    if ( (PawnOwner != None) && (PawnOwner.PlayerReplicationInfo != None) )
        PawnOwnerPRI = PawnOwner.PlayerReplicationInfo;
    else
        PawnOwnerPRI = PlayerOwner.PlayerReplicationInfo;
}

simulated function Message( PlayerReplicationInfo PRI, coerce string Msg, name MsgType )
{
    if ( bMessageBeep )
        PlayerOwner.PlayBeepSound();
    if ( (MsgType == 'Say') || (MsgType == 'TeamSay') )
        Msg = PRI.PlayerName$": "$Msg;
    AddTextMessage(Msg,class'LocalMessage',PRI);
}

function DisplayPortrait(PlayerReplicationInfo PRI);

function DisplayMessages(Canvas C)
{
    local int i, j, XPos, YPos,MessageCount;
    local float XL, YL;

    for( i = 0; i < ConsoleMessageCount; i++ )
    {
        if ( TextMessages[i].Text == "" )
            break;
        else if( TextMessages[i].MessageLife < Level.TimeSeconds )
        {
            TextMessages[i].Text = "";

            if( i < ConsoleMessageCount - 1 )
            {
                for( j=i; j<ConsoleMessageCount-1; j++ )
                    TextMessages[j] = TextMessages[j+1];
            }
            TextMessages[j].Text = "";
            break;
        }
        else
			MessageCount++;
    }

    XPos = (ConsoleMessagePosX * HudCanvasScale * C.SizeX) + (((1.0 - HudCanvasScale) / 2.0) * C.SizeX);
    YPos = (ConsoleMessagePosY * HudCanvasScale * C.SizeY) + (((1.0 - HudCanvasScale) / 2.0) * C.SizeY);

    C.Font = GetConsoleFont(C);
    C.DrawColor = ConsoleColor;

    C.TextSize ("A", XL, YL);

    YPos -= YL * MessageCount+1; // DP_LowerLeft
    YPos -= YL; // Room for typing prompt

    for( i=0; i<MessageCount; i++ )
    {
        if ( TextMessages[i].Text == "" )
            break;

        C.StrLen( TextMessages[i].Text, XL, YL );
        C.SetPos( XPos, YPos );
        C.DrawColor = TextMessages[i].TextColor;
        C.DrawText( TextMessages[i].Text, false );
        YPos += YL;
    }
}

function AddTextMessage(string M, class<LocalMessage> MessageClass, PlayerReplicationInfo PRI)
{
	local int i;


	if( bMessageBeep && MessageClass.Default.bBeep )
		PlayerOwner.PlayBeepSound();

    for( i=0; i<ConsoleMessageCount; i++ )
    {
        if ( TextMessages[i].Text == "" )
            break;
    }

    if( i == ConsoleMessageCount )
    {
        for( i=0; i<ConsoleMessageCount-1; i++ )
            TextMessages[i] = TextMessages[i+1];
    }

    TextMessages[i].Text = M;
    TextMessages[i].MessageLife = Level.TimeSeconds + MessageClass.Default.LifeTime;
    TextMessages[i].TextColor = MessageClass.static.GetConsoleColor(PRI);
    TextMessages[i].PRI = PRI;
}

exec function GrowHUD()
{
    if( !bShowWeaponInfo )
        bShowWeaponInfo = true;
    else if( !bShowPersonalInfo )
        bShowPersonalInfo = true;
    else if( !bShowPoints )
        bShowPoints = true;
    else if ( !bShowWeaponBar )
		bShowWeaponBar = true;
	SaveConfig();
}

exec function ShrinkHUD()
{
	if ( bShowWeaponBar )
		bShowWeaponBar = false;
    else if( bShowPoints )
        bShowPoints = false;
    else if( bShowPersonalInfo )
        bShowPersonalInfo = false;
    else if( bShowWeaponInfo )
        bShowWeaponInfo = false;
	SaveConfig();
}

simulated function SetTargeting( bool bShow, optional Vector TargetLocation, optional float Size );
simulated function DrawCrosshair(Canvas C);
simulated function SetCropping( bool Active );


static function Font LoadFontStatic(int i)
{
	if( default.FontArrayFonts[i] == None )
	{
		default.FontArrayFonts[i] = Font(DynamicLoadObject(default.FontArrayNames[i], class'Font'));
		if( default.FontArrayFonts[i] == None )
			Log("Warning: "$default.Class$" Couldn't dynamically load font "$default.FontArrayNames[i]);
	}

	return default.FontArrayFonts[i];
}

simulated function Font LoadFont(int i)
{
	if( FontArrayFonts[i] == None )
	{
		FontArrayFonts[i] = Font(DynamicLoadObject(FontArrayNames[i], class'Font'));
		if( FontArrayFonts[i] == None )
			Log("Warning: "$Self$" Couldn't dynamically load font "$FontArrayNames[i]);
	}
	return FontArrayFonts[i];
}


static function font GetConsoleFont(Canvas C)
{
	local int FontSize;

	if( default.OverrideConsoleFontName != "" )
	{
		if( default.OverrideConsoleFont != None )
			return default.OverrideConsoleFont;
		default.OverrideConsoleFont = Font(DynamicLoadObject(default.OverrideConsoleFontName, class'Font'));
		if( default.OverrideConsoleFont != None )
			return default.OverrideConsoleFont;
		Log("Warning: HUD couldn't dynamically load font "$default.OverrideConsoleFontName);
		default.OverrideConsoleFontName = "";
	}

	FontSize = Default.ConsoleFontSize;
	if ( C.ClipX < 640 )
		FontSize++;
	if ( C.ClipX < 800 )
		FontSize++;
	if ( C.ClipX < 1024 )
		FontSize++;
	if ( C.ClipX < 1280 )
		FontSize++;
	if ( C.ClipX < 1600 )
		FontSize++;
	return LoadFontStatic(Min(8,FontSize));
}

function Font GetFontSizeIndex(Canvas C, int FontSize)
{
    if ( C.ClipX >= 512 )
		FontSize++;
	if ( C.ClipX >= 640 )
		FontSize++;
	if ( C.ClipX >= 800 )
		FontSize++;
	if ( C.ClipX >= 1024 )
		FontSize++;
	if ( C.ClipX >= 1280 )
		FontSize++;
	if ( C.ClipX >= 1600 )
		FontSize++;

	return LoadFont(Clamp( 8-FontSize, 0, 8));
}

static function Font GetMediumFontFor(Canvas Canvas)
{
	local int i;

	for ( i=0; i<8; i++ )
	{
		if ( Default.FontScreenWidthMedium[i] <= Canvas.ClipX )
			return LoadFontStatic(i);
	}
	return LoadFontStatic(8);
}

static function Font LargerFontThan(Font aFont)
{
	local int i;

	for ( i=0; i<7; i++ )
		if ( LoadFontStatic(i) == aFont )
			return LoadFontStatic(Max(0,i-4));
	return LoadFontStatic(5);
}

simulated function font LoadProgressFont()
{
	if( ProgressFontFont == None )
	{
		ProgressFontFont = Font(DynamicLoadObject(ProgressFontName, class'Font'));
		if( ProgressFontFont == None )
		{
			Log("Warning: "$Self$" Couldn't dynamically load font "$ProgressFontName);
			ProgressFontFont = Font'DefaultFont';
		}
	}
	return ProgressFontFont;
}

simulated function DrawTargeting( Canvas C );

defaultproperties
{
     WhiteColor=(B=255,G=255,R=255,A=255)
     RedColor=(R=255,A=255)
     GreenColor=(G=255,A=255)
     CyanColor=(B=255,G=255,A=255)
     BlueColor=(B=255,A=255)
     GoldColor=(G=255,R=255,A=255)
     PurpleColor=(B=255,R=255,A=255)
     TurqColor=(B=255,G=128,A=255)
     GrayColor=(B=200,G=200,R=200,A=255)
     bMessageBeep=True
     bShowWeaponInfo=True
     bShowPersonalInfo=True
     bShowPoints=True
     bShowWeaponBar=True
     bCrosshairShow=True
     bShowPortrait=True
     ConsoleColor=(B=253,G=216,R=153,A=255)
     ProgressFontName="UT2003Fonts.FontEurostile12"
     ProgressFadeTime=1.000000
     MOTDColor=(B=255,G=255,R=255,A=255)
     HudScale=1.000000
     HudOpacity=255.000000
     HudCanvasScale=1.000000
     CrosshairScale=1.000000
     CrosshairOpacity=1.000000
     CrossHairColor=(B=255,G=255,R=255,A=255)
     ConsoleMessageCount=4
     ConsoleFontSize=5
     ConsoleMessagePosY=1.000000
     FontArrayNames(0)="Engine.DefaultFont"
     FontArrayNames(1)="Engine.DefaultFont"
     FontArrayNames(2)="Engine.DefaultFont"
     FontArrayNames(3)="Engine.DefaultFont"
     FontArrayNames(4)="Engine.DefaultFont"
     FontArrayNames(5)="Engine.DefaultFont"
     FontArrayNames(6)="Engine.DefaultFont"
     FontArrayNames(7)="Engine.DefaultFont"
     FontArrayNames(8)="Engine.DefaultFont"
     bHidden=True
     RemoteRole=ROLE_None
}
