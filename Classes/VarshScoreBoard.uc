//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class VarshScoreBoard extends ScoreBoard;

var MyInfo MyBot;
var ScoreBoard OriginalScoreBoard;

var ChallengeHUD OriginalHUD;


event PostBeginPlay()
{
	AssociateHUD();
	super.PostBeginPlay();
}

function AssociateHUD()
{
	foreach AllActors(Class'ChallengeHUD', OriginalHUD)
	{
		break;
	}
}

function ShowScores(canvas Canvas)
{
	if(MyBot != None)
	{
		MyBot.MyCanvas(Canvas);
		//MyBot.oSB.ShowScores(Canvas);
		
		if(OriginalHUD != None)
		{
			OriginalHUD.DrawCrossHair(Canvas, 0, 0);
		}
		else
		{
			AssociateHUD();
		}
	}
}
