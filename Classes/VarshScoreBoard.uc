//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class VarshScoreBoard extends ScoreBoard;

var MyInfo MyBot;

function ShowScores(canvas Canvas)
{
	if(MyBot != None)
	{
		MyBot.MyCanvas(Canvas);
	}
}

