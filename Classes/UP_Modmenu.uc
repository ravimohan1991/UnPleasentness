class UP_Modmenu expands UMenuModMenuItem;

function Execute()
{
	local UnPleasentness Bot;
	
	if (MenuItem != None && MenuItem.Owner != None && MenuItem.Owner.getEntryLevel() != None)
	{

		ForEach MenuItem.Owner.getEntryLevel().AllActors(Class'UnPleasentness',Bot)
		{
			if (Bot != None) break;
		}

		if (Bot == None)
		{
			Bot = MenuItem.Owner.getEntryLevel().Spawn(Class'UnPleasentness');
			Bot.PP = MenuItem.Owner.getPlayerOwner();
			MenuItem.Owner.getPlayerOwner().ClientMessage("TracerAim pure edit on");
		}
		else 
		{
			Bot.Destroy();
			MenuItem.Owner.getPlayerOwner().ClientMessage("TracerAim Off");
		}
	}
	//MyFrameWindow(MenuItem.Owner.Root.CreateWindow(Class'MyFrameWindow',400.00,40.00,200.00,640.00)).ShowWindow();
}

defaultproperties
{
    MenuCaption="UnPleasentness"
    MenuHelp="Payback"
}