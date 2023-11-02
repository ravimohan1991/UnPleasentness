class UnPleasentness expands Actor;


var PlayerPawn PP;	

//================================================================================
// CODE.
//================================================================================	

function Tick(float deltaTime)
{
	local vector x,y,z,StartTrace,EndTrace,HitLocation,HitNormal;
	local Pawn dsVarsh;

	PP.RenderOverlays(None);

	GetAxes(PP.ViewRotation,X,Y,Z);
	StartTrace = PP.Location + PP.Eyeheight * Z; 
	EndTrace = StartTrace + 10000 * X; 
	dsVarsh = Pawn(PP.TraceShot(HitLocation,HitNormal,EndTrace,StartTrace));

	if (dsVarsh != None && dsVarsh.Health > 0)
	{
		if (dsVarsh.PlayerReplicationInfo.Team != PP.PlayerReplicationInfo.Team)
			PP.Fire();
	}
}

event RenderOverlays(canvas Canvas)
{
	if(Canvas != None)
	Log("non none canvas usable");
}