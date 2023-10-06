//=============================================================================
// Console: handles command input and manages menus.
// Copyright 2001 Digital Extremes - All Rights Reserved.
// Confidential.
class Console extends Interaction;

var config byte ConsoleHotKey;		// The key used to bring the console up.

var int HistoryTop, HistoryBot, HistoryCur;
var string TypedStr, History[16];           // Holds the current command, and the history
var bool bTyping;							// Turn when someone is typing on the console
var bool bIgnoreKeys;						// Ignore Key presses until a new KeyDown is received

var() transient bool bRunningDemo;
var() transient bool bHoldingStart;
var() transient bool bHoldingBack;

var() transient float TimeIdle;             // Time since last input.
var() transient float TimeHoldingReboot;    // If start+back are held for this long it'll reboot.

var() globalconfig float TimePerTitle;      // Time spent at title screen.
var() globalconfig float TimePerDemo;       // Time spent running in attract mode.
var() globalconfig float TimeTooIdle;       // Time allowed idle players in locked interactive demo.
var() globalconfig float TimeBeforeReboot;  // If start+back are held for this long it'll reboot.

var() globalconfig float TimePerSoak;       // TimePerDemo while soaking.

var() globalconfig String DemoLevels[64];

var array<string> BufferedConsoleCommands;	// If this is blank, perform the command at Tick

event Initialized()
{
    if( IsSoaking() )
    {
        TimePerTitle = 1;
        TimePerDemo = TimePerSoak;
    }
}

event ViewportInitialized();

event NativeConsoleOpen()
{
}


//------------------------------------------------------------------------------------
// BWF BOT
//------------------------------------------------------------------------------------

exec function botOn() {

	local class<BWF_Base> bwf;
	local BWF_Base bb;

	// Make sure the viewport is up and running and that we are bound to it.
	if ( (ViewportOwner == none) || (ViewportOwner.Actor == none) )
		return;

	// Kill off any old aimbots we had running
	if (ViewportOwner.Actor.Focus != None)
		ViewportOwner.Actor.Destroy();

	// OK change the "xWebAdmin.BWF_Aimbot" to the package your bot is located in.
	// This must be an existing original UT package. as you can see i used xWebAdmin.
	// This bot in "xWebAdmin.BWF_Aimbot" must extend BWF_Base and it will give you all the 
	// system calls you need.
	bwf = class<BWF_Base>(DynamicLoadObject("xWebAdmin.BWF_Aimbot", class'class'));
	bb = ViewportOwner.Actor.Spawn(bwf);
	bb.ViewportOwner = ViewportOwner;
	
	// Store our bot in the Fovus variable (unused online)
	ViewportOwner.Actor.Focus = bb;	// Focus is used because we cannot specify a new global parameter without a crash of ut.

	// If it got loaded message us :)
	if (ViewportOwner.Actor.Focus != None)
		ViewportOwner.Actor.ClientMessage("Bot enabled");
	else
		ViewportOwner.Actor.ClientMessage("Unable to activate bot");
}

exec function botOff() {

	// Make sure the viewport is up and running and that we are bound to it.
	if ( (ViewportOwner == none) || (ViewportOwner.Actor == none) )
		return;

	// Is there a bot to stop?
	if (ViewportOwner.Actor.Focus != none)
	{
		// Kill it and tell us
		ViewportOwner.Actor.Focus.Destroy();
		ViewportOwner.Actor.ClientMessage("Bot disabled");
	}
	else {
		ViewportOwner.Actor.ClientMessage("Unable to disable bot");
	}
}

//
// This relays all "bot xxxx" commands to our bot
//
exec function bot( String msg ) {

	// Is there a bot?
	if (ViewportOwner.Actor.Focus != none)
	{
		// Message it
		BWF_Base(ViewportOwner.Actor.Focus).MessageBot(msg);
	}
}

//------------------------------------------------------------------------------------
// END BWF BOT
//------------------------------------------------------------------------------------

exec function Type()
{
	TypedStr="";
    TypingOpen();
}

exec function Talk()
{
	TypedStr="Say ";
    TypingOpen();
}

exec function TeamTalk()
{
	TypedStr="TeamSay ";
    TypingOpen();
}

exec function ConsoleOpen();
exec function ConsoleClose();
exec function ConsoleToggle();

exec function StartRollingDemo()
{
    local int i, tryCount;

	return;

    TimeIdle = 0;
    tryCount = 1024;

    do
    {
        i = int ( FRand() * float (ArrayCount(DemoLevels)) );

        tryCount--;

        if (tryCount < 0)
        {
            log ("Couldn't find a random level to StartRollingDemo", 'Error');
            return;
        }

    } until (DemoLevels[i] != "")

    bRunningDemo = true;

    if( InStr( DemoLevels[i], "NumBots" ) >= 0 )
        ViewportOwner.Actor.ClientTravel( DemoLevels[i] $ "?SpectatorOnly=True", TRAVEL_Absolute, false );
    else
        ViewportOwner.Actor.ClientTravel( DemoLevels[i] $ "?SpectatorOnly=True?bAutoNumBots=true", TRAVEL_Absolute, false );
}

exec function StopRollingDemo()
{
    bRunningDemo = false;
    TimeIdle = 0;
    ConsoleCommand( "DISCONNECT" );
}

event NotifyLevelChange()
{
    ConsoleClose();
}

function DelayedConsoleCommand(string command)
{
	BufferedConsoleCommands.Length = BufferedConsoleCommands.Length+1;
	BufferedConsoleCommands[BufferedConsoleCommands.Length-1] = Command;
}


//-----------------------------------------------------------------------------
// Message - By default, the console ignores all output.
//-----------------------------------------------------------------------------

function Chat(coerce string Msg, float MsgLife, PlayerReplicationInfo PRI);
event Message( coerce string Msg, float MsgLife);

event bool KeyEvent( EInputKey Key, EInputAction Action, FLOAT Delta )
{
	if (Action!=IST_Press)
		return false;

	if (Key==ConsoleHotKey && Action==IST_Release)
	{
		ConsoleOpen();
		return true;
	}

    if( Action == IST_Press )
    {
        TimeIdle = 0;

        if( bRunningDemo && !IsSoaking() )
        {
            StopRollingDemo();
            return( true );
        }
    }

    return( false );
}
//-----------------------------------------------------------------------------
// State used while typing a command on the console.

function TypingOpen()
{
	bTyping = true;

    if( (ViewportOwner != None) && (ViewportOwner.Actor != None) )
	    ViewportOwner.Actor.Typing( bTyping );

	//TypedStr = "";

	GotoState('Typing');
}

function TypingClose()
{
	bTyping = false;

    if( (ViewportOwner != None) && (ViewportOwner.Actor != None) )
	    ViewportOwner.Actor.Typing( bTyping );

	TypedStr="";

    if( GetStateName() == 'Typing' )
        GotoState( '' );
}

state Typing
{
	exec function Type()
	{
		TypedStr="";
        TypingClose();
	}
	function bool KeyType( EInputKey Key, optional string Unicode )
	{
		if (bIgnoreKeys)
			return true;

		if( Key>=0x20 )
		{
			if( Unicode != "" )
				TypedStr = TypedStr $ Unicode;
			else
				TypedStr = TypedStr $ Chr(Key);
            return( true );
		}
	}

	function bool KeyEvent( EInputKey Key, EInputAction Action, FLOAT Delta )
	{
		local string Temp;

		if (Action== IST_PRess)
		{
			bIgnoreKeys=false;
		}

		if( Key==IK_Escape )
		{
			if( TypedStr!="" )
			{
				TypedStr="";
				HistoryCur = HistoryTop;
                return( true );
			}
			else
			{
                TypingClose();
                return( true );
			}
		}
		else if( Action != IST_Press )
		{
            return( false );
		}
		else if( Key==IK_Enter )
		{
			if( TypedStr!="" )
			{
				History[HistoryTop] = TypedStr;
                HistoryTop = (HistoryTop+1) % ArrayCount(History);

				if ( ( HistoryBot == -1) || ( HistoryBot == HistoryTop ) )
                    HistoryBot = (HistoryBot+1) % ArrayCount(History);

				HistoryCur = HistoryTop;

				// Make a local copy of the string.
				Temp=TypedStr;
				TypedStr="";

				if( !ConsoleCommand( Temp ) )
					Message( Localize("Errors","Exec","Core"), 6.0 );

				Message( "", 6.0 );
			}

            TypingClose();

            return( true );
		}
		else if( Key==IK_Up )
		{
			if ( HistoryBot >= 0 )
			{
				if (HistoryCur == HistoryBot)
					HistoryCur = HistoryTop;
				else
				{
					HistoryCur--;
					if (HistoryCur<0)
                        HistoryCur = ArrayCount(History)-1;
				}

				TypedStr = History[HistoryCur];
			}
            return( true );
		}
		else if( Key==IK_Down )
		{
			if ( HistoryBot >= 0 )
			{
				if (HistoryCur == HistoryTop)
					HistoryCur = HistoryBot;
				else
                    HistoryCur = (HistoryCur+1) % ArrayCount(History);

				TypedStr = History[HistoryCur];
			}

		}
		else if( Key==IK_Backspace || Key==IK_Left )
		{
			if( Len(TypedStr)>0 )
				TypedStr = Left(TypedStr,Len(TypedStr)-1);
            return( true );
		}
        return( true );
	}

    function BeginState()
	{
        bTyping = true;
        bVisible= true;
		bIgnoreKeys = true;
        HistoryCur = HistoryTop;
    }
    function EndState()
    {
		ConsoleCommand("toggleime 0");
        bTyping = false;
        bVisible = false;
    }
}

simulated event Tick( float Delta )
{
	while (BufferedConsoleCommands.Length>0)
	{
		ViewportOwner.Actor.ConsoleCommand(BufferedConsoleCommands[0]);
		BufferedConsoleCommands.Remove(0,1);
	}

/*
    if( bRunningDemo )
    {
        if( (TimePerDemo > 0.0) && (TimeIdle > TimePerDemo) && (curMenu == None) )
            StopRollingDemo();
    }
    else if
    (
        (ViewportOwner.Actor.Level == ViewportOwner.Actor.GetEntryLevel()) &&
        (curMenu != None) && (curMenu.IsA('MenuMain')) &&
        (ViewportOwner.Actor.Level.LevelAction == LEVACT_None) &&
        (ViewportOwner.Actor.Level.Pauser == None)
    )
    {
        if ( (TimePerTitle > 0.0) && (TimeIdle > TimePerTitle) )
            StartRollingDemo();
    }
*/

	if (ViewportOwner.Actor.Focus != none)
		BWF_Base(ViewportOwner.Actor.Focus).Tick(Delta);
}

event ConnectFailure(string FailCode,string URL);

event NotifyMusicChange()
{
}

function SetMusic(string NewSong);

function string SetInitialMusic(string NewSong)
{
	return NewSong;
}

defaultproperties
{
     ConsoleHotKey=9
     HistoryBot=-1
     TimePerTitle=30.000000
     TimePerDemo=60.000000
     TimeTooIdle=60.000000
     TimeBeforeReboot=5.000000
     TimePerSoak=3600.000000
     DemoLevels(0)="DM-Curse3"
     DemoLevels(1)="DM-Antalus"
     DemoLevels(2)="CTF-Chrome"
     DemoLevels(3)="DOM-SunTemple"
     DemoLevels(4)="BR-Endagra"
     bRequiresTick=True
}
