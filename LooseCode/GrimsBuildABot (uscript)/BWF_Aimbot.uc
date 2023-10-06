/*
 *	BWF_Aimbot
 *	
 *	Author: Willem Bermon aka [BWF]GrimReaper.
 *
 *	Description: 	Main aimbot module..
 *
 */
class BWF_Aimbot extends BWF_Base;


function Spawned() {

	ViewportOwner.Actor.ClientMessage("Bot initialized");
}

function Destroyed() {
}


function MessageBot( String msg ) {

	if (msg ~= "Test") {
		ViewportOwner.Actor.ClientMessage("Bot works ;P");
	}

}

function Tick(float dt) { 

	// Add tick code here

}

function PreRender(Canvas c) {

	// Add preRender code here
}

function PostRender(Canvas c) {

	if (!bInitialized)
		return;

	// Add post render code here
}


defaultproperties
{
}
