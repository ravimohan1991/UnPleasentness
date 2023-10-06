//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyFrameWindow extends UWindowFramedWindow;

function Created ()
{
	Super.Created();
}


//================================================================================
// DEFAULTPROPERTIES.
//================================================================================

defaultproperties
{
    ClientClass=Class'MyWindow'
    WindowTitle="elfLITE Config Window"
}