//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyConfig extends UMenuModMenuItem;

function Execute ()
{
	MyFrameWindow(MenuItem.Owner.Root.CreateWindow(Class'MyFrameWindow',400.00,40.00,200.00,640.00)).ShowWindow();
}