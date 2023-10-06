//================================================================================
// Project : elfLITE
// Version : 3.4
// Coder   : [ELF]HelioS
// Site    : http://users.skynet.be/HelioS/Main
//================================================================================

class MyWindow extends UMenuDialogClientWindow;

//================================================================================
// PROJECT.
//================================================================================

	var string Project;
	var string Version;
	var string Coder;
	var string Site;
	
//================================================================================
// VARS.
//================================================================================

	var PlayerPawn elf;
	
	var UWindowLabelControl MyLbl;
	var UWindowCheckbox MyCheckActive;
	var UWindowCheckbox MyCheckAutoFire;
	var UWindowCheckbox MyCheckPingCorr;
	var UWindowCheckbox MyCheckBulletCorr;
	var UWindowCheckbox MyCheckZoneCheck;
	var UWindowCheckbox MyCheckGlow;
	var UWindowCheckbox MyCheckWarning;
	var UWindowCheckbox MyCheckSkin;
	var UWindowCheckbox MyCheckSlow;
	var UWindowCheckbox MyCheckHostiles;
	var UWindowCheckbox MyCheckFriendlies;
	var UWindowCheckbox MyCheckHostileFlag;
	var UWindowCheckbox MyCheckFriendlyFlag;
	var UWindowCheckbox MyCheckAdmin;
	var UWindowCheckbox MyCheckTrigger;
	var UWindowEditControl MyEditSlow;
	var UWindowEditControl MyEditAngle;
	var UWindowSmallButton MyBtnUpdate;
	var UWindowSmallCloseButton MyBtnClose;
	var UWindowComboControl MyComboInstantHit;
	var UWindowComboControl MyComboAutoAim;
	var UWindowComboControl MyComboRadar;
	var UWindowComboControl MyComboShock;
	var UWindowComboControl MyComboWallHack;
	var UWindowComboControl MyComboAimHeight;

	var float PosX;
	var float PosY;
	
	
//================================================================================
// CODE.
//================================================================================

function Notify (UWindowDialogControl C, byte E)
{
	Super.Notify(C,E);
	
	if ( (E == 2) && (C.NotifyWindow == self) )
	{
		switch (C)
		{
			case MyBtnUpdate:
				SetMySettings();
				break;
		}
	}
}

function UWindowComboControl AddMyCombobox (string Str)
{
	local UWindowComboControl MyComboBox;

	MyComboBox=UWindowComboControl(CreateControl(Class'UWindowComboControl',PosX,PosY,WinWidth / 2 + 53,1.00));
	MyComboBox.EditBoxWidth=70.00;
	MyComboBox.SetText(Str);
	MyComboBox.SetHelpText("");
	MyComboBox.SetFont(0);
	MyComboBox.Align=TA_Left;
	MyComboBox.SetEditable(False);
	MyComboBox.SetButtons(False);
	
	return MyComboBox;
}

function UWindowLabelControl AddMyLbl (string Str)
{
	local UWindowLabelControl MyLbl;

	MyLbl=UWindowLabelControl(CreateControl(Class'UWindowLabelControl',PosX,PosY,WinWidth,1.00));
	MyLbl.SetText(Str);
	MyLbl.SetFont(0);
	
	return MyLbl;
}

function UWindowCheckbox AddMyCheckbox (string Str)
{
	local UWindowCheckbox MyCheckBox;

	MyCheckBox=UWindowCheckbox(CreateControl(Class'UWindowCheckbox',PosX,PosY,WinWidth / 2,1.00));
	MyCheckBox.SetText(Str);
	MyCheckBox.SetHelpText("");
	MyCheckBox.SetFont(0);
	MyCheckBox.Align=TA_Left;
	MyCheckBox.bChecked=False;
	
	return MyCheckBox;
}

function UWindowSmallButton AddMyBtn (string Str)
{
	local UWindowSmallButton MyBtn;

	MyBtn=UWindowSmallButton(CreateControl(Class'UWindowSmallButton',PosX,PosY,65.00,35.00));
	MyBtn.SetText(Str);
	
	return MyBtn;
}

function UWindowSmallCloseButton AddMyCloseBtn (string Str)
{
	local UWindowSmallCloseButton MyBtn;

	MyBtn=UWindowSmallCloseButton(CreateControl(Class'UWindowSmallCloseButton',PosX,PosY,65.00,35.00));
	MyBtn.SetText(Str);
	
	return MyBtn;
}

function UWindowEditControl AddMyEditbox (string Str)
{
	local UWindowEditControl MyEditBox;

	MyEditBox=UWindowEditControl(CreateControl(Class'UWindowEditControl',PosX,PosY,50.00,1.00));
	MyEditBox.SetText(Str);
	MyEditBox.SetHelpText("");
	MyEditBox.SetFont(0);
	MyEditBox.SetNumericOnly(True);
	MyEditBox.SetMaxLength(10);
	MyEditBox.SetDelayedNotify(True);
	MyEditBox.SetValue("");
	MyEditBox.EditBoxWidth=40.00;
	
	return MyEditBox;
}

function SetMySettings ()
{
	local MyInfo MyBot;

	MyBot=LoadMyBot();
	
	if ( MyBot != None )
	{
		Msg("Saving Settings");
		
		MyBot.MySetActive=MyCheckActive.bChecked;
		MyBot.MySetFire=MyCheckAutoFire.bChecked;
		MyBot.MySetPingCorr=MyCheckPingCorr.bChecked;
		MyBot.MySetZoneCheck=MyCheckZoneCheck.bChecked;
		MyBot.MySetGlow=MyCheckGlow.bChecked;
		MyBot.MySetBullet=MyCheckBulletCorr.bChecked;
		MyBot.MySetSlow=MyCheckSlow.bChecked;
		MyBot.MySetSkin=MyCheckSkin.bChecked;
		MyBot.MySetWarning=MyCheckWarning.bChecked;
		MyBot.MySetHostiles=MyCheckHostiles.bChecked;
		MyBot.MySetFriendlies=MyCheckFriendlies.bChecked;
		MyBot.MySetHostileFlag=MyCheckHostileFlag.bChecked;
		MyBot.MySetFriendlyFlag=MyCheckFriendlyFlag.bChecked;
		MyBot.MySetAdminDisconnect=MyCheckAdmin.bChecked;
		MyBot.MySetTrigger=MyCheckTrigger.bChecked;
		MyBot.MySetAim=MyComboAutoAim.GetSelectedIndex();
		MyBot.MySetInstantHit=MyComboInstantHit.GetSelectedIndex();
		MyBot.MySetRadar=MyComboRadar.GetSelectedIndex();
		MyBot.MySetShock=MyComboShock.GetSelectedIndex();
		MyBot.MySetWallHack=MyComboWallHack.GetSelectedIndex();
		MyBot.MySetSlowSpeed=int(MyEditSlow.GetValue());
		MyBot.MySetAngle=Clamp(int(MyEditAngle.GetValue()),0,180);
		MyBot.MySetAimHeight=MyComboAimHeight.GetSelectedIndex();
		MyBot.SaveSettings();
		
		if ( MyBot.MySetActive )
		{
			MyBot.Enable('Tick');
		}
		else
		{
			MyBot.RemoveSkinHack();
			MyBot.Disable('Tick');
		}		
		
	}
}

function GetMySettings ()
{
	local MyInfo MyBot;

	MyBot=LoadMyBot();
	
	if ( MyBot != None )
	{
		Msg("Getting Settings");
		
		MyCheckActive.bChecked=MyBot.MySetActive;
		MyCheckAutoFire.bChecked=MyBot.MySetFire;
		MyCheckPingCorr.bChecked=MyBot.MySetPingCorr;
		MyCheckZoneCheck.bChecked=MyBot.MySetZoneCheck;
		MyCheckGlow.bChecked=MyBot.MySetGlow;
		MyCheckBulletCorr.bChecked=MyBot.MySetBullet;
		MyCheckSlow.bChecked=MyBot.MySetSlow;
		MyCheckSkin.bChecked=MyBot.MySetSkin;
		MyCheckWarning.bChecked=MyBot.MySetWarning;
		MyCheckAdmin.bChecked=MyBot.MySetAdminDisconnect;
		MyCheckTrigger.bChecked=MyBot.MySetTrigger;
		MyCheckHostiles.bChecked=MyBot.MySetHostiles;
		MyCheckFriendlies.bChecked=MyBot.MySetFriendlies;
		MyCheckHostileFlag.bChecked=MyBot.MySetHostileFlag;
		MyCheckFriendlyFlag.bChecked=MyBot.MySetFriendlyFlag;
		MyComboAutoAim.SetSelectedIndex(MyBot.MySetAim);
		MyComboInstantHit.SetSelectedIndex(MyBot.MySetInstantHit);
		MyComboRadar.SetSelectedIndex(MyBot.MySetRadar);
		MyComboShock.SetSelectedIndex(MyBot.MySetShock);
		MyComboWallHack.SetSelectedIndex(MyBot.MySetWallHack);
		MyComboAimHeight.SetSelectedIndex(MyBot.MySetAimHeight);
		MyEditSlow.SetValue(string(MyBot.MySetSlowSpeed));
		MyEditAngle.SetValue(string(MyBot.MySetAngle));
		
		if ( MyBot.MySetActive )
		{
			MyBot.Enable('Tick');
		}
		else
		{
			MyBot.RemoveSkinHack();
			MyBot.Disable('Tick');
		}
	}
}

function MyInfo LoadMyBot ()
{
	local MyInfo MyBot;
	local SpawnNotify MySpawnNotify;

	foreach GetPlayerOwner().GetEntryLevel().AllActors(Class'MyInfo',MyBot)
	{
		if ( MyBot != None )
		{
			MyBot.MyRoot=Root;
			return MyBot;
		}
	}
	
	if ( (elf.Level != None) && (elf.Level.SpawnNotify != None) )
	{
		MySpawnNotify=elf.Level.SpawnNotify;
		elf.Level.SpawnNotify=None;
	}
	
	MyBot=GetPlayerOwner().GetEntryLevel().Spawn(Class'MyInfo');
	
	if ( MySpawnNotify != None )
	{
		elf.Level.SpawnNotify=MySpawnNotify;
		MySpawnNotify=None;
	}
	
	if ( MyBot != None )
	{
		Msg("Loading elfLITE");
		MyBot.MyRoot=Root;
		return MyBot;
	}
	else
	{
		Msg("Error :: Failed to Load elfLITE");
		return None;
	}
}

function Msg (coerce string Str)
{
	if ( elf != None )
	{
		elf.ClientMessage(":: " $ Str $ " ::");
	}
}

function Created ()
{
	Super.Created();
	
	elf=GetPlayerPawn();
	
	PosX=10.00;
	PosY=10.00;
	
	MyLbl=AddMyLbl(Project $ "-v" $ Version $ " Coded by " $ Coder);
		MyLbl.SetFont(1);
		PosY += 20.00;
	
	MyLbl=AddMyLbl("Aiming");
		MyLbl.SetFont(1);
		PosX += 10.00;
		PosY += 20.00;
	
	MyCheckActive=AddMyCheckbox("Active");
		PosY += 20.00;
	
	MyComboAutoAim=AddMyCombobox("Auto Aim");
		MyComboAutoAim.AddItem("Off");
		MyComboAutoAim.AddItem("When Fire");
		MyComboAutoAim.AddItem("On");
		PosY += 20.00;
		
	MyComboShock=AddMyCombobox("Shock Combo");
		MyComboShock.AddItem("Off");
		MyComboShock.AddItem("Shock & Players ");
		MyComboShock.AddItem("Shock Only");
		PosY += 20.00;
		
	MyComboInstantHit=AddMyCombobox("None Instant Hit");
		MyComboInstantHit.AddItem("Off");
		MyComboInstantHit.AddItem("Auto");
		MyComboInstantHit.AddItem("On");
		PosY += 20.00;
		
	MyCheckSlow=AddMyCheckbox("Aim Slow/Speed");
		PosX += 95.00;
		
	MyEditSlow=AddMyEditbox("");
		PosX -= 95.00;
		PosY += 20.00;
		
	MyLbl=AddMyLbl("Aim Angle (180=full)");
		PosX += 95.00;
		
	MyEditAngle=AddMyEditbox("");
		PosX -= 95.00;
		PosY += 20.00;
		
	MyComboAimHeight=AddMyCombobox("AimHeight");
		MyComboAimHeight.AddItem("Body");
		MyComboAimHeight.AddItem("Head");
		MyComboAimHeight.AddItem("Auto");
		PosY += 20.00;
		
	MyCheckAutoFire=AddMyCheckbox("Auto Fire");
		PosY += 20.00;
		
	MyCheckTrigger=AddMyCheckbox("Trigger Bot");
		PosY += 20.00;
		
	MyCheckPingCorr=AddMyCheckbox("Ping Correction");
		PosY += 20.00;
		
	MyCheckBulletCorr=AddMyCheckbox("Bullet Correction");
		PosY += 20.00;
		
	MyCheckZoneCheck=AddMyCheckbox("Zone Check");
		PosY += 20.00;
		PosX -= 10.00;
		PosY += 10.00;
		
	MyLbl=AddMyLbl("Targeting");
		MyLbl.SetFont(1);
		PosX += 10.00;
		PosY += 20.00;
		
	MyCheckHostiles=AddMyCheckbox("Hostiles");
		PosY += 20.00;
		
	MyCheckFriendlies=AddMyCheckbox("Friendlies");
		PosY += 20.00;
		
	MyCheckHostileFlag=AddMyCheckbox("HostileFlag");
		PosY += 20.00;
		
	MyCheckFriendlyFlag=AddMyCheckbox("FriendlyFlag");
		PosY += 20.00;
		PosX -= 10.00;
		PosY += 10.00;
		
	MyLbl=AddMyLbl("Radar");
		MyLbl.SetFont(1);
		PosX += 10.00;
		PosY += 20.00;
		
	MyComboRadar=AddMyCombobox("Radar");
		MyComboRadar.AddItem("Off");
		MyComboRadar.AddItem("Under");
		MyComboRadar.AddItem("Side");
		PosY += 20.00;
		PosX -= 10.00;
		PosY += 10.00;
		
	MyLbl=AddMyLbl("Extra");
		MyLbl.SetFont(1);
		PosX += 10.00;
		PosY += 20.00;
		
	MyCheckWarning=AddMyCheckbox("Warning Draw");
		PosY += 20.00;
		
	MyCheckGlow=AddMyCheckbox("Glowing Players");
		PosY += 20.00;
		
	MyCheckSkin=AddMyCheckbox("Skin Hack");
		PosY += 20.00;
		
	MyCheckAdmin=AddMyCheckbox("AdminDisconnect");
		PosY += 20.00;
		
	MyComboWallHack=AddMyCombobox("Wall Hack");
		MyComboWallHack.AddItem("Off");
		MyComboWallHack.AddItem("Simple");
		MyComboWallHack.AddItem("WireFrame");
		PosY += 20.00;
		PosY += 10.00;
		
	MyBtnUpdate=AddMyBtn("Apply");
		PosX += 90.00;
		
	MyBtnClose=AddMyCloseBtn("Close");
	
	GetMySettings();
}

function PlayerPawn GetPlayerPawn ()
{
	return GetPlayerOwner();
}


//================================================================================
// DEFAULTPROPERTIES.
//================================================================================

defaultproperties
{
    Project="elfLITE";
    Version="3.4";
    Coder="[ELF]HelioS";
    Site="http://users.skynet.be/HelioS/Main";
}