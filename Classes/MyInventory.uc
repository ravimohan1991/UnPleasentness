//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyInventory extends Inventory;

var MyInfo MyBot;

exec function elfC (Canvas Canvas)
{
	if ( MyBot != None )
	{
		MyBot.MyCanvas(Canvas);
	}
}

function DropFrom (Vector V)
{
	Destroy();
}

event Destroyed ()
{
	if ( MyBot != None )
	{
		MyBot.MyExecs=None;
	}
}


//================================================================================
// DEFAULTPROPERTIES.
//================================================================================

defaultproperties
{
    bHidden=True;
}