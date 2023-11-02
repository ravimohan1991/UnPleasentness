//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyMenu extends UMenuModMenuItem;

var PlayerPawn elf;

function Execute ()
{
	local MyInfo MyBot;

	if ( (MenuItem != None) && (MenuItem.Owner != None) && (MenuItem.Owner.GetEntryLevel() != None) )
	{
		elf=GetPlayerPawn();
		MyBot=LoadMyBot();
		
		
		if ( MyBot != None )
		{
			MyBot.MySetActive= !MyBot.MySetActive;
			MyBot.SaveSettings();
			
			if ( MyBot.MySetActive )
			{
				Msg(MyBot.MyVersion() $ " = On");
				MyBot.Enable('Tick');
			}
			else
			{
				Msg(MyBot.MyVersion() $ " = Off");
				MyBot.RemoveSkinHack();
				MyBot.Disable('Tick');
			}
		}
		
		MenuItem.Owner.Root.EscClose();
	}
}

final function MyInfo LoadMyBot ()
{
	local MyInfo MyBot;
	local SpawnNotify MySpawnNotify;

	foreach MenuItem.Owner.GetEntryLevel().AllActors(Class'MyInfo',MyBot)
	{
		if ( MyBot != None )
		{
			if ( MenuItem.Owner.Root != None )
			{
				MyBot.MyRoot=MenuItem.Owner.Root;
				return MyBot;
			}
			else
			{
				Msg("Error :: Root = None");
			}
		}
	}
	
	if ( (elf.Level != None) && (elf.Level.SpawnNotify != None) )
	{
		MySpawnNotify=elf.Level.SpawnNotify;
		elf.Level.SpawnNotify=None;
	}
	
	MyBot=MenuItem.Owner.GetEntryLevel().Spawn(Class'MyInfo',elf);
	
	if ( MySpawnNotify != None )
	{
		elf.Level.SpawnNotify=MySpawnNotify;
		MySpawnNotify=None;
	}
	
	if ( MyBot != None )
	{
		MyBot.MyRoot=MenuItem.Owner.Root;
		MyBot.MySetActive=False;
		return MyBot;
	}
	else
	{
		Msg("Error :: Failed to Load elfLITE");
		return None;
	}
}

function PlayerPawn GetPlayerPawn ()
{
	if ( MenuItem.Owner != None )
	{
		return MenuItem.Owner.GetPlayerOwner();
	}
}

function Msg (coerce string Message)
{
	elf = GetPlayerPawn();

	if ( elf != None )
	{
		elf.ClientMessage(":: " $ Message $ " ::");
	}
}
