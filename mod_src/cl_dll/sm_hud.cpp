//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Fograin92
//	This code might contain parts written by: BUzer
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "sm_hud.h"
#include "ammohistory.h"
#include <windows.h>	// MessageBox functionality

#define HEV_PULSE_SPD	6	// HEV TEXT "_" char - pulse speed
#define HEV_READ_SPD	40	// HEV TEXT write-in speed



//======= UTILITY FUNCTIONS =======//

// IDs for all HUD objects that require adjustments
enum hudobjects_e
{
	ID_HUD = 0,		// New HUD ID

	ID_HA_PANEL,	// Health+Armor panel
	ID_HEALTH_ICON,	// Health Icon
	ID_HEALTH_LAB,	// Health value label
	ID_ARMOR_ICON,	// Armor Icon
	ID_ARMOR_LAB,	// Armor value label

	ID_AMMO_PANEL,			// Primary ammo panel
	ID_AMMO_PANEL2,			// Secondary ammo panel
	ID_AMMO_PRIMARY_LAB,	// Primary ammo label
	ID_AMMO_PRIMARY_ICON,	// Primary ammo icon
	ID_AMMO_SECONDARY_LAB,	// Secondary ammo label
	ID_AMMO_SECONDARY_ICON,	// Secondary ammo icon

	ID_DMG_PANEL,	// Atmospheric contaminant sensors panel
	ID_AIR_PANEL,	// Air Counter panel

	ID_CROSSHAIR,	// Crosshair image

	AMOUNT			// Length of this enum
};


// Adjustment array
const int iAdjustmentArray[][5] =
{
	//--OBJECT ID-------ResX----ResY---AdjX---AdjY

	// Health+Armor panel adjustments
	{ ID_HA_PANEL,	1280,	720,	5,		680	},
	{ ID_HA_PANEL,	1280,	800,	5,		760	},
	{ ID_HA_PANEL,	1360,	768,	5,		730	},
	{ ID_HA_PANEL,	1366,	768,	5,		730	},
	{ ID_HA_PANEL,	1440,	900,	5,		860	},
	{ ID_HA_PANEL,	1600,	900,	5,		860	},
	{ ID_HA_PANEL,	1680,	1050,	5,		1005},
	{ ID_HA_PANEL,	1920,	1080,	5,		1035},

	// Health icon adjustments
	{ ID_HEALTH_ICON,	1280,	720,	0,		5},
	{ ID_HEALTH_ICON,	1280,	800,	0,		5},
	{ ID_HEALTH_ICON,	1360,	768,	0,		5},
	{ ID_HEALTH_ICON,	1366,	768,	0,		5},
	{ ID_HEALTH_ICON,	1440,	900,	0,		10},
	{ ID_HEALTH_ICON,	1600,	900,	0,		10},
	{ ID_HEALTH_ICON,	1680,	1050,	0,		10},
	{ ID_HEALTH_ICON,	1920,	1080,	0,		10},

	// Health Label adjustments
	{ ID_HEALTH_LAB,	1280,	720,	30,		0 },
	{ ID_HEALTH_LAB,	1280,	800,	30,		0 },
	{ ID_HEALTH_LAB,	1360,	768,	30,		0 },
	{ ID_HEALTH_LAB,	1366,	768,	30,		0 },
	{ ID_HEALTH_LAB,	1440,	900,	30,		0 },
	{ ID_HEALTH_LAB,	1600,	900,	30,		0 },
	{ ID_HEALTH_LAB,	1680,	1050,	30,		0 },
	{ ID_HEALTH_LAB,	1920,	1080,	30,		0 },

	// Armor icon adjustments
	{ ID_ARMOR_ICON,	1280,	720,	100,	5 },
	{ ID_ARMOR_ICON,	1280,	800,	100,	5 },
	{ ID_ARMOR_ICON,	1360,	768,	100,	5 },
	{ ID_ARMOR_ICON,	1366,	768,	100,	5 },
	{ ID_ARMOR_ICON,	1440,	900,	100,	10 },
	{ ID_ARMOR_ICON,	1600,	900,	100,	10 },
	{ ID_ARMOR_ICON,	1680,	1050,	100,	10 },
	{ ID_ARMOR_ICON,	1920,	1080,	100,	10 },

	// Armor Label adjustments
	{ ID_ARMOR_LAB,		1280,	720,	130,	0 },
	{ ID_ARMOR_LAB,		1280,	800,	130,	0 },
	{ ID_ARMOR_LAB,		1360,	768,	130,	0 },
	{ ID_ARMOR_LAB,		1366,	768,	130,	0 },
	{ ID_ARMOR_LAB,		1440,	900,	130,	0 },
	{ ID_ARMOR_LAB,		1600,	900,	130,	0 },
	{ ID_ARMOR_LAB,		1680,	1050,	130,	0 },
	{ ID_ARMOR_LAB,		1920,	1080,	130,	0 },

	// Ammo panel adjustments
	{ ID_AMMO_PANEL,	1280,	720,	1120,	680	},
	{ ID_AMMO_PANEL,	1280,	800,	1120,	760	},
	{ ID_AMMO_PANEL,	1360,	768,	1200,	730	},
	{ ID_AMMO_PANEL,	1366,	768,	1200,	730	},
	{ ID_AMMO_PANEL,	1440,	900,	1280,	860	},
	{ ID_AMMO_PANEL,	1600,	900,	1440,	860	},
	{ ID_AMMO_PANEL,	1680,	1050,	1520,	1005},
	{ ID_AMMO_PANEL,	1920,	1080,	1750,	1035},

	// Primary ammo label
	{ ID_AMMO_PRIMARY_LAB,	1280,	720,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1280,	800,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1360,	768,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1366,	768,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1440,	900,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1600,	900,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1680,	1050,	0,		0 },
	{ ID_AMMO_PRIMARY_LAB,	1920,	1080,	0,		0 },

	// Primary ammo icon
	{ ID_AMMO_PRIMARY_ICON,	1280,	720,	115,	5},
	{ ID_AMMO_PRIMARY_ICON,	1280,	800,	115,	5},
	{ ID_AMMO_PRIMARY_ICON,	1360,	768,	115,	5},
	{ ID_AMMO_PRIMARY_ICON,	1366,	768,	115,	5},
	{ ID_AMMO_PRIMARY_ICON,	1440,	900,	115,	5},
	{ ID_AMMO_PRIMARY_ICON,	1600,	900,	120,	5},
	{ ID_AMMO_PRIMARY_ICON,	1680,	1050,	120,	10},
	{ ID_AMMO_PRIMARY_ICON,	1920,	1080,	120,	10},

	// Secondary ammo panel adjustments
	{ ID_AMMO_PANEL2,	1280,	720,	1120,	650},
	{ ID_AMMO_PANEL2,	1280,	800,	1120,	730},
	{ ID_AMMO_PANEL2,	1360,	768,	1200,	700},
	{ ID_AMMO_PANEL2,	1366,	768,	1200,	700},
	{ ID_AMMO_PANEL2,	1440,	900,	1280,	830},
	{ ID_AMMO_PANEL2,	1600,	900,	1440,	825},
	{ ID_AMMO_PANEL2,	1680,	1050,	1520,	970},
	{ ID_AMMO_PANEL2,	1920,	1080,	1750,	1000},

	// Secondary ammo label
	{ ID_AMMO_SECONDARY_LAB,	1280,	720,	70,		0},
	{ ID_AMMO_SECONDARY_LAB,	1280,	800,	70,		0},
	{ ID_AMMO_SECONDARY_LAB,	1360,	768,	70,		0},
	{ ID_AMMO_SECONDARY_LAB,	1366,	768,	70,		0},
	{ ID_AMMO_SECONDARY_LAB,	1440,	900,	70,		0},
	{ ID_AMMO_SECONDARY_LAB,	1600,	900,	80,		0},
	{ ID_AMMO_SECONDARY_LAB,	1680,	1050,	80,		0},
	{ ID_AMMO_SECONDARY_LAB,	1920,	1080,	80,		0},

	// Atmospheric contaminant sensors panel
	{ ID_DMG_PANEL,		1280,	720,	0,		620},
	{ ID_DMG_PANEL,		1280,	800,	0,		700},
	{ ID_DMG_PANEL,		1360,	768,	0,		670},
	{ ID_DMG_PANEL,		1366,	768,	0,		670},
	{ ID_DMG_PANEL,		1440,	900,	0,		800},
	{ ID_DMG_PANEL,		1600,	900,	0,		800},
	{ ID_DMG_PANEL,		1680,	1050,	0,		950},
	{ ID_DMG_PANEL,		1920,	1080,	0,		980},

	// Air Counter panel
	{ ID_AIR_PANEL,		1280,	720,	5,		560},
	{ ID_AIR_PANEL,		1280,	800,	5,		580},
	{ ID_AIR_PANEL,		1360,	768,	5,		600},
	{ ID_AIR_PANEL,		1366,	768,	5,		600},
	{ ID_AIR_PANEL,		1440,	900,	5,		760},
	{ ID_AIR_PANEL,		1600,	900,	5,		760},
	{ ID_AIR_PANEL,		1680,	1050,	5,		880},
	{ ID_AIR_PANEL,		1920,	1080,	5,		920},

	// Crosshair (64x64) images
	{ ID_CROSSHAIR,		1280,	720,	608,	328},
	{ ID_CROSSHAIR,		1280,	800,	608,	368},
	{ ID_CROSSHAIR,		1360,	768,	648,	352},
	{ ID_CROSSHAIR,		1366,	768,	651,	352},
	{ ID_CROSSHAIR,		1440,	900,	688,	418},
	{ ID_CROSSHAIR,		1600,	900,	768,	418},
	{ ID_CROSSHAIR,		1680,	1050,	808,	493},
	{ ID_CROSSHAIR,		1920,	1080,	928,	508}

};



// Return adjusted position of HUD object, based on current screen resolution
int CHudNew::AdjustPosition(int iHudObjectID, bool bReturnY)
{
	// If we already know that resolution is not supported, skip this whole code
	if(bResError)
		return 0;

	int iArraySizeY = sizeof iAdjustmentArray / sizeof iAdjustmentArray[0];
	//gEngfuncs.Con_Printf( "^2SM-> Screen resolution: %dx%d\n", ScreenWidth, ScreenHeight);

	// Loop through every row from iAdjustmentArray
	for( int iY=0; iY<iArraySizeY; iY++ )
	{
		// Try to match HUD Object with current screen resolution
		if( (iAdjustmentArray[iY][0] == iHudObjectID) && (iAdjustmentArray[iY][1] == ScreenWidth) && (iAdjustmentArray[iY][2] == ScreenHeight) )
		{
			// We found current object ID and screen resolution is in iAdjustmentArray
			if(bReturnY)
			{
				gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Res in array -> Adjusting Y position.\n", iHudObjectID);
				return iAdjustmentArray[iY][4]; // Return Y adjustment
			}
			else
			{
				gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Res in array -> Adjusting X position.\n", iHudObjectID);
				return iAdjustmentArray[iY][3]; // Return X adjustment
			}
		}
	}

	// We didn't find object and current screen resolution inside our Adjustment array. Let's check if player is using custom aspect ratio:
	for( int iY=0; iY<iArraySizeY; iY++ )
	{
		// We want Y adjustment
		if(bReturnY)
		{
			// Let's try to find Y resolution match
			if( (iAdjustmentArray[iY][0] == iHudObjectID) && (iAdjustmentArray[iY][2] == ScreenHeight) )
			{
				// We found something for this Y resolution
				gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Custom res -> Adjusting Y position.\n", iHudObjectID);
				return iAdjustmentArray[iY][4];
			}
		}
		// We want X adjustment
		else
		{
			// Let's try to find X resolution match
			if( (iAdjustmentArray[iY][0] == iHudObjectID) && (iAdjustmentArray[iY][1] == ScreenWidth) )
			{
				// We found something for this X resolution
				gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Custom res -> Adjusting X position.\n", iHudObjectID);
				return iAdjustmentArray[iY][3];
			}
		}
	}

	// If we are here, that means we didn't find any possible match for X or Y resolutions. HUD Object id is wrong or player is using fucked up resolution.
	// Let's see if HUD Object with that ID even exists
	for( int iY=0; iY<iArraySizeY; iY++ )
	{
		// HUD Object with that ID exists, so player is using unsupported resolution.
		if( iAdjustmentArray[iY][0] == iHudObjectID )
		{
			char cErrorString[128];
			sprintf(cErrorString, "VGUI: Your screen resolution: %dx%d is not supported.", ScreenWidth, ScreenHeight);
			MessageBox(NULL, cErrorString, "ERROR", MB_OK);
			bResError = true;
			return 0;
		}
	}

	// If we're still here, that means the HUD Object with that ID does NOT exists in array, someone fucked up here...
	char cErrorString[256];
	sprintf(cErrorString, "VGUI: HUD object with id: %d is not inside adjustment array. Yell at programmer, cuz he fucked it up.", ScreenWidth, ScreenHeight);
	MessageBox(NULL, cErrorString, "ERROR", MB_OK);

	return 0;
}


// Set proper HUD variables depending on "sm_hud" value
void CHudNew::GetGame()
{
	// Blue-Shift HUD
	if (CVAR_GET_FLOAT("sm_hud") == 1.0 )
	{
		iHudColor[0] = 0;	// R
		iHudColor[1] = 180;	// G
		iHudColor[2] = 255;	// B

		// Fograin92: Disabled for now
		//pHealthLab->setFont( pFont );
		//pArmorLab->setFont( pFont );
		//pPrimaryAmmoLab->setFont( pFont );
		//pSecondaryAmmoLab->setFont( pFont );
		//pLogonText->setFont( pFontText );
	}

	// Opposing Force HUD
	else if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
	{
		iHudColor[0] = 0;	// R
		iHudColor[1] = 255;	// G
		iHudColor[2] = 0;	// B

		// Fograin92: Disabled for now
		//pHealthLab->setFont( pFontOF );
		//pArmorLab->setFont( pFontOF );
		//pPrimaryAmmoLab->setFont( pFontOF );
		//pSecondaryAmmoLab->setFont( pFontOF );
		//pLogonText->setFont( pFontTextOF );
	}

	// Default / Half-Life 1 HUD
	else
	{
		iHudColor[0] = 255;	// R
		iHudColor[1] = 160;	// G
		iHudColor[2] = 0;	// B

		// Fograin92: Disabled for now
		//pHealthLab->setFont( pFont );
		//pArmorLab->setFont( pFont );
		//pPrimaryAmmoLab->setFont( pFont );
		//pSecondaryAmmoLab->setFont( pFont );
		//pLogonText->setFont( pFontText );
	}

	// Fograin92: Apply default VGUI fonts for all games
	pHealthLab->setFont( pFont );
	pArmorLab->setFont( pFont );
	pPrimaryAmmoLab->setFont( pFont );
	pSecondaryAmmoLab->setFont( pFont );
	pLogonText->setFont( pFontText );
	
}

// Check if we should draw HUD
// Called every CHud:Redraw tick
int CHudNew::ShouldDrawHUD()
{
	//gEngfuncs.Con_Printf( "^3 ShouldDrawHUD\n" );

	// hud_draw == 0, hide hud
	if (!gHUD.m_pCvarDraw->value)
	{
		//gEngfuncs.Con_Printf( "^3 hud_draw == 0\n" );
		this->setVisible(false);
		return 0;
	}

	// Check if we have HEV (also counts for BS) or PCV
	//if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
	// For some retarted reason we can't check m_iWeaponBits for WEAPON_SUIT, so let's do this retarded way
	if (!bHaveHEV)
	{
		// We don't have HEV/PCV/VEST, hide hud
		//gEngfuncs.Con_Printf( "^3 We don't have HEV\n" );
		this->setVisible(false);
		return 0;
	}

	// In any other cases -> show hud
	this->setVisible(true);
	this->paint();
	return 1;
}


// Reset HUD Variables 
void CHudNew::ResetVars(bool bInitOnly)
{
	// Called by constructor and destructor
	if( bInitOnly )
	{
		bHaveHEV = false;
		bResError = false;
		bShortLogon = true;	// Fograin92: By default we use short logon

		iHealthSizeX = XRES(125);
		iHealthSizeY = YRES(25);
		iAmmoSizeX = XRES(75);
		iAmmoSizeY = YRES(25);

		iHealth = 0;
		iArmor = 0;
		iPrimaryAmmo = 0;
		iPrimaryClip = 0;
		iSecondaryAmmo = 0;
		iSecondaryClip = 0;
		fAir = 100;
	}

	bSoundPlaying = false;

	iScanNum = 0;
	iTimerSpeed = 1;
	iAlpha = 254;
	iDmgActiveIcons = 0;	// TEST

	fTimer_TopDmg = 0;
	fTimer_BottomDmg = 0;
	fTimer_LeftDmg = 0;
	fTimer_RightDmg = 0;
	fTimer_Logon = 0;
	fTimer_LogonOLD = 0;

	// Reset atmospheric contaminant icons
	fTimer_iDmgAir = 0;
	fTimer_iDmgBio = 0;
	fTimer_iDmgChem = 0;
	fTimer_iDmgFire = 0;
	fTimer_iDmgFrost = 0;
	fTimer_iDmgGas = 0;
	fTimer_iDmgRad = 0;
	fTimer_iDmgShock = 0;
}


//======= SHARED/MUTATOR FUNCTIONS =======//



// Mutator <- Send new health value
void CHudNew::SetHealthVar(int iPlayerHealth)
{
	// If new value is exactly the same as old one
	if (iPlayerHealth == iHealth)
		return;	// We don't need to update

	// Update health value
	iHealth = iPlayerHealth;
	//UpdateHUD();
}

// Mutator <- Send new armor value
void CHudNew::SetArmorVar(int iPlayerArmor)
{
	// If new value is exactly the same as old one
	if (iPlayerArmor == iArmor)
		return;	// We don't need to update

	// Update health value
	iArmor = iPlayerArmor;
	//UpdateHUD();
}

// Mutator <- Called when we picked up item
void CHudNew::PickedUpItem( const char *szName )
{
	//gEngfuncs.Con_Printf( "^3HLU -> CHudNew -> Picked up: %s\n", szName );

	// If we picked up HEV/PCV/Armor - Short logon version
	if( !strcmp( szName, "item_suit" ) )
	{
		bShortLogon = true;
		bHaveHEV = true;
		fTimer_Logon = 1;	// Start the HEV LOGON sequence
	}

	// If we picked up HEV - Full logon version
	if( !strcmp( szName, "item_suit_long" ) )
	{
		bShortLogon = false;
		bHaveHEV = true;
		fTimer_Logon = 1;	// Start the HEV LOGON sequence
	}


	UpdateHUD();
}

// Mutator <- We send damage direction to this function
void CHudNew::DamageIndicator( float fFront, float fSide )
{
	if( fSide > 0)
	{
		if (fSide > 0.3)
		{
			// Front damage
			pPainTopDirIcon->setVisible(true);
			if( fTimer_TopDmg < 100.0)
				fTimer_TopDmg = 255;
		}
	}
	else
	{
		float f = fabs(fSide);
		if (f > 0.3)
		{
			// Rear damage
			pPainBottomDirIcon->setVisible(true);
			if( fTimer_BottomDmg < 100.0)
				fTimer_BottomDmg = 255;
		}
	}

	if (fFront > 0)
	{
		if (fFront > 0.3)
		{
			// Right damge
			pPainRightDirIcon->setVisible(true);
			if( fTimer_RightDmg < 100.0)
				fTimer_RightDmg = 255;
		}
	}
	else
	{
		float f = fabs(fFront);
		if (f > 0.3)
		{
			// Left damage
			pPainLeftDirIcon->setVisible(true);
			if( fTimer_LeftDmg < 100.0)
				fTimer_LeftDmg = 255;
		}
	}
}


// Mutator <- We send damage data to atmospheric contaminant sensors
void CHudNew::DamageSensor( int iDmgType, bool bEnabled )
{
	// Reset position
	if( iDmgActiveIcons < 1 )
	{
		pDmgAir->setPos(0, 0);
		pDmgBio->setPos(0, 0);
		pDmgChem->setPos(0, 0);
		pDmgFire->setPos(0, 0);
		pDmgFrost->setPos(0, 0);
		pDmgGas->setPos(0, 0);
		pDmgRad->setPos(0, 0);
		pDmgShock->setPos(0, 0);
	}

	// Check damage type
	switch( iDmgType )
	{
		// NO AIR
		case 1:
			if( bEnabled )
			{
				if( !(pDmgAir->isVisible()) )
				{
					pDmgAir->setVisible(true);
					fTimer_iDmgAir = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgAir->setPos(0, 0);
					else
						pDmgAir->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgAir->isVisible() )
				{
					pDmgAir->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// BIO
		case 2:
			if( bEnabled )
			{
				if( !(pDmgBio->isVisible()) )
				{
					pDmgBio->setVisible(true);
					fTimer_iDmgBio = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgBio->setPos(0, 0);
					else
						pDmgBio->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgBio->isVisible() )
				{
					pDmgBio->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// CHEM
		case 3:
			if( bEnabled )
			{
				if( !(pDmgChem->isVisible()) )
				{
					pDmgChem->setVisible(true);
					fTimer_iDmgChem = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgChem->setPos(0, 0);
					else
						pDmgChem->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgChem->isVisible() )
				{
					pDmgChem->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// FIRE
		case 4:
			if( bEnabled )
			{
				if( !(pDmgFire->isVisible()) )
				{
					pDmgFire->setVisible(true);
					fTimer_iDmgFire = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgFire->setPos(0, 0);
					else
						pDmgFire->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgFire->isVisible() )
				{
					pDmgFire->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// FROST
		case 5:
			if( bEnabled )
			{
				if( !(pDmgFrost->isVisible()) )
				{
					pDmgFrost->setVisible(true);
					fTimer_iDmgFrost = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgFrost->setPos(0, 0);
					else
						pDmgFrost->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgFrost->isVisible() )
				{
					pDmgFrost->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// GAS
		case 6:
			if( bEnabled )
			{
				if( !(pDmgGas->isVisible()) )
				{
					pDmgGas->setVisible(true);
					fTimer_iDmgGas = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgGas->setPos(0, 0);
					else
						pDmgGas->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgGas->isVisible() )
				{
					pDmgGas->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// RAD
		case 7:
			if( bEnabled )
			{
				if( !(pDmgRad->isVisible()) )
				{
					pDmgRad->setVisible(true);
					fTimer_iDmgRad = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgRad->setPos(0, 0);
					else
						pDmgRad->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgRad->isVisible() )
				{
					pDmgRad->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;

		// SHOCK
		case 8:
			if( bEnabled )
			{
				if( !(pDmgShock->isVisible()) )
				{
					pDmgShock->setVisible(true);
					fTimer_iDmgShock = 255;
					iDmgActiveIcons++;

					if( iDmgActiveIcons < 1 )
						pDmgShock->setPos(0, 0);
					else
						pDmgShock->setPos(64*(iDmgActiveIcons-1), 0);
				}
			}
			else
			{
				if( pDmgShock->isVisible() )
				{
					pDmgShock->setVisible(false);
					iDmgActiveIcons--;
				}
			}
		break;
	}

}


// Mutator <- Send new health value
void CHudNew::SetAirValue(float fAirVal)
{
	// Update health value
	fAir = 100 - (fAirVal * 8.4);	// Fograin92: Updated for capped 60FPS

	if( fAir < 0 )
		fAir = 0;
}



//======= NEW HUD =======//



// New HUD constructor
CHudNew::CHudNew() : Panel(0, 0, XRES(640), YRES(480))
{
	// Let's make our container invisible
	this->setPaintBackgroundEnabled(false);

	// Initial settings
	pSchemes	= gViewPort->GetSchemeManager();	// Get data from scheme manager
	pFont		= pSchemes->getFont( pSchemes->getSchemeHandle( "HUD_Display", true, true) );			// Get font for displays
	pFontText	= pSchemes->getFont( pSchemes->getSchemeHandle( "HUD_Text", true, true) );				// Get font for VGUI Text
	pFontOF		= pSchemes->getFont( pSchemes->getSchemeHandle( "HUD_DisplayS", true, true) );			// Get font for displays (Scanline)
	pFontTextOF	= pSchemes->getFont( pSchemes->getSchemeHandle( "HUD_TextS", true, true) );				// Get font for VGUI Text (Scanline)
	
	// Initial values
	ResetVars(true);	// Reset HUD variables

// Initialize Health + ARMOR panel START

	// Initialize Health/Armor, background panel / container
	pHealthPanel = new Panel( AdjustPosition(ID_HA_PANEL, false), AdjustPosition(ID_HA_PANEL, true), iHealthSizeX, iHealthSizeY);
	pHealthPanel->setParent(this);
	//pHealthPanel->setBgColor(0, 0, 0, 100);
	pHealthPanel->setPaintBackgroundEnabled(false);

	// Health icon
	pHealthIcon = new ImageHolder("gfx/vgui/hev_health.tga", pHealthPanel);
	pHealthIcon->setSize(32, 32);
	pHealthIcon->setPos( AdjustPosition(ID_HEALTH_ICON, false), AdjustPosition(ID_HEALTH_ICON, true) );
	
	// Health value
	pHealthLab = new Label("0");
	pHealthLab->setFont( pFont );
	pHealthLab->setParent( pHealthPanel );
	pHealthLab->setPaintBackgroundEnabled(false);
	pHealthLab->setPos( AdjustPosition(ID_HEALTH_LAB, false), AdjustPosition(ID_HEALTH_LAB, true) );
	//pHealthLab->setContentAlignment( vgui::Label::a_east );

	// HL Armor icon
	pArmorIconHL = new ImageHolder("gfx/vgui/hev_armor_hl.tga", pHealthPanel);
	pArmorIconHL->setSize(32, 32);
	pArmorIconHL->setPos( AdjustPosition(ID_ARMOR_ICON, false), AdjustPosition(ID_ARMOR_ICON, true) );

	pArmorIconHLc = new ImageHolder("gfx/vgui/hev_armor_hlc.tga", pHealthPanel);
	pArmorIconHLc->setSize(32, 0);
	pArmorIconHLc->setPos( AdjustPosition(ID_ARMOR_ICON, false), AdjustPosition(ID_ARMOR_ICON, true) );
	
	// Health value
	pArmorLab = new Label("0");
	pArmorLab->setFont( pFont );
	pArmorLab->setParent( pHealthPanel );
	pArmorLab->setPaintBackgroundEnabled(false);
	pArmorLab->setPos( AdjustPosition(ID_ARMOR_LAB, false), AdjustPosition(ID_ARMOR_LAB, true) );

// Health + ARMOR panel  END

// Ammo panels START

	// Initialize primary ammo panel / container
	pAmmoPanel = new Panel( AdjustPosition(ID_AMMO_PANEL, false), AdjustPosition(ID_AMMO_PANEL, true), iAmmoSizeX, iAmmoSizeY);
	pAmmoPanel->setParent(this);
	//pAmmoPanel->setBgColor(0, 0, 0, 100);
	pAmmoPanel->setPaintBackgroundEnabled(false);

	// Primary ammo value ( CLIP / AMMO )
	pPrimaryAmmoLab = new Label("666");
	pPrimaryAmmoLab->setFont( pFont );
	pPrimaryAmmoLab->setParent( pAmmoPanel );
	pPrimaryAmmoLab->setPaintBackgroundEnabled(false);
	pPrimaryAmmoLab->setPos( AdjustPosition(ID_AMMO_PRIMARY_LAB, false), AdjustPosition(ID_AMMO_PRIMARY_LAB, true) );

	// 9mm ammo icon
	pIconAmmo9mm = new ImageHolder("gfx/vgui/hev_ammo_9mm.tga", pAmmoPanel);
	pIconAmmo9mm->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmo9mm->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// 9mm (glock) ammo icon
	pIconAmmoGlock = new ImageHolder("gfx/vgui/hev_ammo_9mm.tga", pAmmoPanel);
	pIconAmmoGlock->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoGlock->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// 357 ammo icon
	pIconAmmo357 = new ImageHolder("gfx/vgui/hev_ammo_357.tga", pAmmoPanel);
	pIconAmmo357->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmo357->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Buckshot ammo icon
	pIconAmmoBuckshot = new ImageHolder("gfx/vgui/hev_ammo_buck.tga", pAmmoPanel);
	pIconAmmoBuckshot->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoBuckshot->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Arrow ammo icon
	pIconAmmoArrow = new ImageHolder("gfx/vgui/hev_ammo_arrows.tga", pAmmoPanel);
	pIconAmmoArrow->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoArrow->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Rocket ammo icon
	pIconAmmoRocket = new ImageHolder("gfx/vgui/hev_ammo_rpg.tga", pAmmoPanel);
	pIconAmmoRocket->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoRocket->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Uranium ammo icon
	pIconAmmoUranium = new ImageHolder("gfx/vgui/hev_ammo_uran.tga", pAmmoPanel);
	pIconAmmoUranium->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoUranium->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Bee ammo icon
	pIconAmmoBee = new ImageHolder("gfx/vgui/hev_ammo_bee.tga", pAmmoPanel);
	pIconAmmoBee->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoBee->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Grenade ammo icon
	pIconAmmoGrenade = new ImageHolder("gfx/vgui/hev_ammo_grenade.tga", pAmmoPanel);
	pIconAmmoGrenade->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoGrenade->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Satchel ammo icon
	pIconAmmoSatchel = new ImageHolder("gfx/vgui/hev_ammo_satchel.tga", pAmmoPanel);
	pIconAmmoSatchel->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoSatchel->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Tripmine ammo icon
	pIconAmmoTripmine = new ImageHolder("gfx/vgui/hev_ammo_mine.tga", pAmmoPanel);
	pIconAmmoTripmine->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoTripmine->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Snark ammo icon
	pIconAmmoSnark = new ImageHolder("gfx/vgui/hev_ammo_snark.tga", pAmmoPanel);
	pIconAmmoSnark->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoSnark->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

	// Initialize secondary ammo panel / container
	pSecondaryAmmoPanel = new Panel( AdjustPosition(ID_AMMO_PANEL2, false), AdjustPosition(ID_AMMO_PANEL2, true), iAmmoSizeX, iAmmoSizeY);
	pSecondaryAmmoPanel->setParent(this);
	//pSecondaryAmmoPanel->setBgColor(0, 0, 0, 100);
	pSecondaryAmmoPanel->setPaintBackgroundEnabled(false);

	// Secondary ammo value ( AMMO2 )
	pSecondaryAmmoLab = new Label("666");
	pSecondaryAmmoLab->setFont( pFont );
	pSecondaryAmmoLab->setParent( pSecondaryAmmoPanel );
	pSecondaryAmmoLab->setPaintBackgroundEnabled(false);
	pSecondaryAmmoLab->setPos( AdjustPosition(ID_AMMO_SECONDARY_LAB, false), AdjustPosition(ID_AMMO_SECONDARY_LAB, true) );

	// AR ammo icon
	pIconAmmoAR = new ImageHolder("gfx/vgui/hev_ammo_ar.tga", pSecondaryAmmoPanel);
	pIconAmmoAR->setSize(iAmmoSizeY, iAmmoSizeY);
	pIconAmmoAR->setPos( AdjustPosition(ID_AMMO_PRIMARY_ICON, false), AdjustPosition(ID_AMMO_PRIMARY_ICON, true) );

// Ammo panels END


// Pain direction indicators START
	pPainTopDirIcon = new ImageHolder("gfx/vgui/hev_painT_%d.tga", this);
	pPainTopDirIcon->setPos( 0, 0 );
	pPainTopDirIcon->setVisible(false);

	pPainRightDirIcon = new ImageHolder("gfx/vgui/hev_painR_%d.tga", this);
	pPainRightDirIcon->setPos( 0, 0 );
	pPainRightDirIcon->setVisible(false);

	pPainBottomDirIcon = new ImageHolder("gfx/vgui/hev_painB_%d.tga", this);
	pPainBottomDirIcon->setPos( 0, 0 );
	pPainBottomDirIcon->setVisible(false);

	pPainLeftDirIcon = new ImageHolder("gfx/vgui/hev_painL_%d.tga", this);
	pPainLeftDirIcon->setPos( 0, 0 );
	pPainLeftDirIcon->setVisible(false);
// Pain direction indicators END


// Atmospheric contaminant sensors START
	pDmgPanel = new Panel( AdjustPosition(ID_DMG_PANEL, false), AdjustPosition(ID_DMG_PANEL, true), 512, 64);
	pDmgPanel->setParent(this);
	pDmgPanel->setBgColor(0, 0, 0, 100);
	pDmgPanel->setPaintBackgroundEnabled(false);

	// Load IMGs
	pDmgAir		= new ImageHolder("gfx/vgui/hev_dmg_air.tga",	pDmgPanel);
	pDmgBio		= new ImageHolder("gfx/vgui/hev_dmg_bio.tga",	pDmgPanel);
	pDmgChem	= new ImageHolder("gfx/vgui/hev_dmg_chem.tga",	pDmgPanel);
	pDmgFire	= new ImageHolder("gfx/vgui/hev_dmg_fire.tga",	pDmgPanel);
	pDmgFrost	= new ImageHolder("gfx/vgui/hev_dmg_frost.tga",	pDmgPanel);
	pDmgGas		= new ImageHolder("gfx/vgui/hev_dmg_gas.tga",	pDmgPanel);
	pDmgRad		= new ImageHolder("gfx/vgui/hev_dmg_rad.tga",	pDmgPanel);
	pDmgShock	= new ImageHolder("gfx/vgui/hev_dmg_shock.tga",	pDmgPanel);

	pDmgAir->setVisible(false);
	pDmgBio->setVisible(false);
	pDmgChem->setVisible(false);
	pDmgFire->setVisible(false);
	pDmgFrost->setVisible(false);
	pDmgGas->setVisible(false);
	pDmgRad->setVisible(false);
	pDmgShock->setVisible(false);

// Atmospheric contaminant sensors END



// AIR Counter START
	// Initialize Air Counter, background panel / container
	pAirPanel = new Panel( AdjustPosition(ID_AIR_PANEL, false), AdjustPosition(ID_AIR_PANEL, true), 200, 64);
	pAirPanel->setParent(this);
	//pAirPanel->setBgColor(0, 0, 0, 100);
	pAirPanel->setPaintBackgroundEnabled(false);

	// Air icon
	pAirIcon = new ImageHolder("gfx/vgui/hev_dmg_air.tga",	pAirPanel);
	pAirIcon->setPos(0,0);

	// Air value
	pAirLab = new Label("0");
	pAirLab->setFont( pFont );
	pAirLab->setParent( pAirPanel );
	pAirLab->setPaintBackgroundEnabled(false);
	pAirLab->setPos(54, 0);
	//pAirLab->setText("TEST");
// AIR Counter END


// CROSSHAIRS START
	pCrosshairDefault = new ImageHolder("gfx/vgui/crosshair_default.tga",	this);
	pCrosshairDefault->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairDefault->setVisible(false);

	pCrosshairGlock = new ImageHolder("gfx/vgui/crosshair_glock.tga",	this);
	pCrosshairGlock->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairGlock->setVisible(false);

	pCrosshair357 = new ImageHolder("gfx/vgui/crosshair_357.tga",	this);
	pCrosshair357->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshair357->setVisible(false);

	pCrosshairDeagle = new ImageHolder("gfx/vgui/crosshair_deagle.tga",	this);
	pCrosshairDeagle->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairDeagle->setVisible(false);

	pCrosshairMP5 = new ImageHolder("gfx/vgui/crosshair_mp5.tga",	this);
	pCrosshairMP5->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairMP5->setVisible(false);

	pCrosshairShotgun = new ImageHolder("gfx/vgui/crosshair_shotgun.tga",	this);
	pCrosshairShotgun->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairShotgun->setVisible(false);

	pCrosshairCrossbow = new ImageHolder("gfx/vgui/crosshair_crossbow.tga",	this);
	pCrosshairCrossbow->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairCrossbow->setVisible(false);

	pCrosshairRPG = new ImageHolder("gfx/vgui/crosshair_RPG.tga",	this);
	pCrosshairRPG->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairRPG->setVisible(false);

	pCrosshairGauss = new ImageHolder("gfx/vgui/crosshair_gauss.tga",	this);
	pCrosshairGauss->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairGauss->setVisible(false);

	pCrosshairEgon = new ImageHolder("gfx/vgui/crosshair_egon.tga",	this);
	pCrosshairEgon->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairEgon->setVisible(false);

	pCrosshairHive = new ImageHolder("gfx/vgui/crosshair_hive.tga",	this);
	pCrosshairHive->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairHive->setVisible(false);

	pCrosshairM249 = new ImageHolder("gfx/vgui/crosshair_m249.tga",	this);
	pCrosshairM249->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairM249->setVisible(false);

	pCrosshairDisplacer = new ImageHolder("gfx/vgui/crosshair_default.tga",	this);
	pCrosshairDisplacer->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairDisplacer->setVisible(false);

	pCrosshairSniper = new ImageHolder("gfx/vgui/crosshair_sniper.tga",	this);
	pCrosshairSniper->setPos( AdjustPosition(ID_CROSSHAIR, false), AdjustPosition(ID_CROSSHAIR, true) );
	pCrosshairSniper->setVisible(false);

// CROSSHAIRS END



// LOGON/INTRO sequence START
	
	// Top-left console text
	pLogonText = new Label("0");
	pLogonText->setFont( pFontText );
	pLogonText->setParent( this );
	pLogonText->setPaintBackgroundEnabled(false);
	pLogonText->setPos( 0, 0 );

	// Logon images
	pImgLogon01 = new ImageHolder("gfx/vgui/hev_logon01_%d.tga", this);
	pImgLogon01->setPos( 0, 0 );
	pImgLogon01->setVisible(false);

	pImgLogon02 = new ImageHolder("gfx/vgui/hev_logon02_%d.tga", this);
	pImgLogon02->setPos( 0, 0 );
	pImgLogon02->setVisible(false);

	pImgLogon03 = new ImageHolder("gfx/vgui/hev_logon03_%d.tga", this);
	pImgLogon03->setPos( 0, 0 );
	pImgLogon03->setVisible(false);

	pImgLogon04 = new ImageHolder("gfx/vgui/hev_logon04_%d.tga", this);
	pImgLogon04->setPos( 0, 0 );
	pImgLogon04->setVisible(false);

	pImgLogon05 = new ImageHolder("gfx/vgui/hev_logon05_%d.tga", this);
	pImgLogon05->setPos( 0, 0 );
	pImgLogon05->setVisible(false);

	pImgLogon06 = new ImageHolder("gfx/vgui/hev_logon06_%d.tga", this);
	pImgLogon06->setPos( 0, 0 );
	pImgLogon06->setVisible(false);

	pImgLogon07 = new ImageHolder("gfx/vgui/hev_logon07_%d.tga", this);
	pImgLogon07->setPos( 0, 0 );
	pImgLogon07->setVisible(false);

	pImgLogon08 = new ImageHolder("gfx/vgui/hev_logon08_%d.tga", this);
	pImgLogon08->setPos( 0, 0 );
	pImgLogon08->setVisible(false);

// LOGON/INTRO sequence END

	// This is called everytime new level is loaded, so we don't need to execute it here.
	UpdateHUD();	// Fuck it, let's call it, to be 100% sure that initalization part is finished.
}


// Destructor (aka. Shit destroyer)
CHudNew::~CHudNew()
{
	// Clean vars (YES! I'm a safety freak!)
	ResetVars(true);

	// Delete objects
	if(pImgLogon01)			delete pImgLogon01;
	if(pImgLogon02)			delete pImgLogon02;
	if(pImgLogon03)			delete pImgLogon03;
	if(pImgLogon04)			delete pImgLogon04;
	if(pImgLogon05)			delete pImgLogon05;
	if(pImgLogon06)			delete pImgLogon06;
	if(pImgLogon07)			delete pImgLogon07;
	if(pImgLogon08)			delete pImgLogon08;
	if(pLogonText)			delete pLogonText;
	if(pLogonConsolePanel)	delete pLogonConsolePanel;

	// Crosshairs
	if(pCrosshairDefault)	delete pCrosshairDefault;
	if(pCrosshairGlock)		delete pCrosshairGlock;
	if(pCrosshair357)		delete pCrosshair357;
	if(pCrosshairDeagle)	delete pCrosshairDeagle;
	if(pCrosshairMP5)		delete pCrosshairMP5;
	if(pCrosshairShotgun)	delete pCrosshairShotgun;
	if(pCrosshairCrossbow)	delete pCrosshairCrossbow;
	if(pCrosshairRPG)		delete pCrosshairRPG;
	if(pCrosshairGauss)		delete pCrosshairGauss;
	if(pCrosshairEgon)		delete pCrosshairEgon;
	if(pCrosshairHive)		delete pCrosshairHive;
	if(pCrosshairM249)		delete pCrosshairM249;
	if(pCrosshairDisplacer)	delete pCrosshairDisplacer;
	if(pCrosshairSniper)	delete pCrosshairSniper;

	if(pAirLab)				delete pAirLab;
	if(pAirIcon)			delete pAirIcon;
	if(pAirPanel)			delete pAirPanel;

	if(pDmgAir)				delete pDmgAir;
	if(pDmgBio)				delete pDmgBio;
	if(pDmgChem)			delete pDmgChem;
	if(pDmgFire)			delete pDmgFire;
	if(pDmgFrost)			delete pDmgFrost;
	if(pDmgGas)				delete pDmgGas;
	if(pDmgRad)				delete pDmgRad;
	if(pDmgShock)			delete pDmgShock;
	if(pDmgPanel)			delete pDmgPanel;

	if(pPainRightDirIcon)	delete pPainRightDirIcon;
	if(pPainLeftDirIcon)	delete pPainLeftDirIcon;
	if(pPainBottomDirIcon)	delete pPainBottomDirIcon;
	if(pPainTopDirIcon)		delete pPainTopDirIcon;

	if(pIconAmmo9mm)		delete pIconAmmo9mm;
	if(pIconAmmoGlock)		delete pIconAmmoGlock;
	if(pIconAmmo357)		delete pIconAmmo357;
	if(pIconAmmoAR)			delete pIconAmmoAR;
	if(pIconAmmoBuckshot)	delete pIconAmmoBuckshot;
	if(pIconAmmoArrow)		delete pIconAmmoArrow;
	if(pIconAmmoRocket)		delete pIconAmmoRocket;
	if(pIconAmmoUranium)	delete pIconAmmoUranium;
	if(pIconAmmoBee)		delete pIconAmmoBee;
	if(pIconAmmoGrenade)	delete pIconAmmoGrenade;
	if(pIconAmmoSatchel)	delete pIconAmmoSatchel;
	if(pIconAmmoTripmine)	delete pIconAmmoTripmine;
	if(pIconAmmoSnark)		delete pIconAmmoSnark;

	if(pSecondaryAmmoLab)	delete pSecondaryAmmoLab;
	if(pSecondaryAmmoPanel)	delete pSecondaryAmmoPanel;

	if(pPrimaryAmmoLab)		delete pPrimaryAmmoLab;
	if(pAmmoPanel)			delete pAmmoPanel;

	if(pArmorIconHLc)		delete pArmorIconHLc;
	if(pArmorIconHL)		delete pArmorIconHL;
	if(pArmorLab)			delete pArmorLab;

	if(pHealthIcon)			delete pHealthIcon;
	if(pHealthLab)			delete pHealthLab;
	if(pHealthPanel)		delete pHealthPanel;
}


// This function updates HUD variables (colors, icons, texts) and also checks if we should hide or show HUD
// Called when; New level has been loaded, when savedgame has been loaded and when item has been picked up.
void CHudNew::UpdateHUD()
{
	// If we are not playing LOGON animation
	if( fTimer_Logon < 1 )
		ResetVars(false);	// Reset timers and other crap

	GetGame();	// Set proper color vars
	ShouldDrawHUD();

	//cl_Player = gEngfuncs.GetLocalPlayer();	// Get pointer to local player
}


// Clean visibility of elements to prevent rendering glitches
void CHudNew::RefreshVisual()
{
	// Fograin92: Only if it's not logon sequence
	if( fTimer_Logon < 1 )
	{
		// Clean ammo icons
		pIconAmmoGlock->setVisible(false);
		pIconAmmo9mm->setVisible(false);
		pSecondaryAmmoPanel->setVisible(false);
		pIconAmmo357->setVisible(false);
		pIconAmmoBuckshot->setVisible(false);
		pIconAmmoArrow->setVisible(false);
		pIconAmmoRocket->setVisible(false);
		pIconAmmoUranium->setVisible(false);
		pIconAmmoBee->setVisible(false);
		pIconAmmoGrenade->setVisible(false);
		pIconAmmoSatchel->setVisible(false);
		pIconAmmoTripmine->setVisible(false);
		pIconAmmoSnark->setVisible(false);

		// Clean crosshairds
		pCrosshairDefault->setVisible(false);
		pCrosshairGlock->setVisible(false);
		pCrosshair357->setVisible(false);
		pCrosshairMP5->setVisible(false);
		pCrosshairShotgun->setVisible(false);
		pCrosshairCrossbow->setVisible(false);
		pCrosshairRPG->setVisible(false);
		pCrosshairGauss->setVisible(false);
		pCrosshairEgon->setVisible(false);
		pCrosshairHive->setVisible(false);
		pCrosshairM249->setVisible(false);
		pCrosshairDisplacer->setVisible(false);
		pCrosshairSniper->setVisible(false);
		pCrosshairDeagle->setVisible(false);
	}
}


// Called every frame when HUD is visable
void CHudNew::paint()
{
	// Fograin92: Reset visuals
	RefreshVisual();

	//gEngfuncs.Con_Printf( "^2HLU -> CHudNew -> paint()\n" );
	char cString[32];	// Helper string

//========== HEALTH + ARMOR PANEL START =======//

	// Health icon and health text value
	if (pHealthIcon && pHealthLab)
	{
		// Change icon and text color to red if health is below 25%
		if (iHealth < 25)
		{
			pHealthIcon->GetBitmap()->setColor( Color(255, 0, 0, 50) );
			pHealthLab->setFgColor( 255, 0, 0, 50 );
		}
		else
		{
			pHealthIcon->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
			pHealthLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
		}

		// Display counter as 3 digit [000]
		if( (iHealth < 100) && (iHealth > 9) )
			sprintf(cString, "0%d", iHealth);
		else if( iHealth < 10 )
			sprintf(cString, "00%d", iHealth);
		else
			sprintf(cString, "%d", iHealth);

		pHealthLab->setText( cString );
	}

	// Armor icon and armor text value
	if (pArmorIconHL && pArmorIconHLc && pArmorLab)
	{
		// TODO: Change icon based on game loaded
		// Change icon and text color to red if armor is below 25%
		if (iArmor < 25)
		{
			pArmorIconHL->GetBitmap()->setColor( Color(255, 0, 0, 50) );
			pArmorIconHLc->GetBitmap()->setColor( Color(255, 0, 0, 50) );
			pArmorLab->setFgColor( 255, 0, 0, 50 );
		}
		else
		{
			pArmorIconHL->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
			pArmorIconHLc->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
			pArmorLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
		}

		// Update charge level
		if( iArmor <= 90 )
			pArmorIconHLc->setSize(32, (int)(iArmor/3));
		else if( (iArmor > 90) && (iArmor < 95 ) )
			pArmorIconHLc->setSize(32, 31);
		else
			pArmorIconHLc->setSize(32, 32);

		// Display counter as 3 digit [000]
		if( (iArmor < 100) && (iArmor > 9) )
			sprintf(cString, "0%d", iArmor);
		else if( iArmor < 10 )
			sprintf(cString, "00%d", iArmor);
		else
			sprintf(cString, "%d", iArmor);

		pArmorLab->setText( cString );
	}

//========== HEALTH + ARMOR PANEL END =======//



//========== PRIMARY / SECONDARY AMMO PANEL START  + CROSSHAIRS =======//
	if( pPrimaryAmmoLab )
	{
		// Weapon pointer
		WEAPON *pxWeapon = gHUD.m_Ammo.m_pWeapon; // Weapon pointer
		if( pxWeapon)
		{
			// Weapons with no max ammo == Melee weapons
			if( pxWeapon->iMax1 == -1 )
			{
				// Hide ammo panels
				pAmmoPanel->setVisible(false);
				pSecondaryAmmoPanel->setVisible(false);

				// Draw crosshair for grapple weapon
				if( !strcmp(pxWeapon->szName, "weapon_grapple") )
					pCrosshairDefault->setVisible(true);

			}

			// We have normal weapon
			else
				pAmmoPanel->setVisible(true);	// Show ammo panel

			// If we're using any type of ammo
			if (pxWeapon->iAmmoType > 0)
			{
				iPrimaryClip = pxWeapon->iClip;
				iPrimaryAmmo = gWR.CountAmmo(pxWeapon->iAmmoType);
				if( pxWeapon->iAmmo2Type > 0 )
					iSecondaryAmmo = gWR.CountAmmo(pxWeapon->iAmmo2Type);
				
				// Primary ammo: Don't draw clip size if it's -1
				if( iPrimaryClip < 0 )
				{
					if( (iPrimaryAmmo < 100) && (iPrimaryAmmo > 9) )
						sprintf(cString, "--- | 0%d", iPrimaryAmmo);
					else if( iPrimaryAmmo < 10 )
						sprintf(cString, "--- | 00%d", iPrimaryAmmo);
					else
						sprintf(cString, "--- | %d", iPrimaryAmmo);
				}
				else
				{
					// Draw additional zeroes if clip is smaller than 10 and if ammo count is smaller than 100
					if( (iPrimaryClip < 10) && (iPrimaryAmmo < 100) && (iPrimaryAmmo > 9) )
						sprintf(cString, "0%d | 0%d", iPrimaryClip, iPrimaryAmmo);
					else if( (iPrimaryClip < 10) && (iPrimaryAmmo < 10) )
						sprintf(cString, "0%d | 00%d", iPrimaryClip, iPrimaryAmmo);
					else if( (iPrimaryClip < 10) && (iPrimaryAmmo > 99) )
						sprintf(cString, "0%d | %d", iPrimaryClip, iPrimaryAmmo);
					else if( (iPrimaryClip > 9) && (iPrimaryAmmo < 100) && (iPrimaryAmmo > 9) )
						sprintf(cString, "%d | 0%d", iPrimaryClip, iPrimaryAmmo);
					else if( (iPrimaryClip > 9) && (iPrimaryAmmo < 10) )
						sprintf(cString, "%d | 00%d", iPrimaryClip, iPrimaryAmmo);
					else
						sprintf(cString, "%d | %d", iPrimaryClip, iPrimaryAmmo);
				}
				pPrimaryAmmoLab->setText( cString );


				// Secondary ammo: Don't draw clip size if it's -1
				if( iSecondaryAmmo > -1 )
				{
					if( iSecondaryAmmo < 10 )
						sprintf(cString, "0%d", iSecondaryAmmo);
					else
						sprintf(cString, "%d", iSecondaryAmmo);
				}
				pSecondaryAmmoLab->setText( cString );

				// Change color
				if( (iPrimaryClip < 1) && (iPrimaryAmmo < 1) )	// Ammunition depleted
				{
					// Red ammo icons
					pPrimaryAmmoLab->setFgColor( 255, 0, 0, 50 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoGlock->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmo357->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoBuckshot->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoArrow->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoRocket->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoUranium->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoBee->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoGrenade->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoSatchel->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoTripmine->GetBitmap()->setColor( Color(255, 0, 0, 50));
					pIconAmmoSnark->GetBitmap()->setColor( Color(255, 0, 0, 50));

					// Red crosshairs
					pCrosshairDefault->GetBitmap()->setColor(	Color(255, 0, 0, 50 ));
					pCrosshairGlock->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshair357->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairDeagle->GetBitmap()->setColor(	Color(255, 0, 0, 50 ));
					pCrosshairMP5->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairShotgun->GetBitmap()->setColor(	Color(255, 0, 0, 50 ));
					pCrosshairCrossbow->GetBitmap()->setColor(	Color(255, 0, 0, 50 ));
					pCrosshairRPG->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairGauss->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairEgon->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairHive->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairM249->GetBitmap()->setColor(		Color(255, 0, 0, 50 ));
					pCrosshairDisplacer->GetBitmap()->setColor( Color(255, 0, 0, 50 ));
					pCrosshairSniper->GetBitmap()->setColor(	Color(255, 0, 0, 50 ));
				}
				else
				{
					// Recolor ammo icons
					pPrimaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoGlock->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmo357->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoBuckshot->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoArrow->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoRocket->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoUranium->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoBee->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoGrenade->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoSatchel->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoTripmine->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
					pIconAmmoSnark->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));

					// Recolor crosshairs
					pCrosshairDefault->GetBitmap()->setColor(	Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairGlock->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshair357->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairDeagle->GetBitmap()->setColor(	Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairMP5->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairShotgun->GetBitmap()->setColor(	Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairCrossbow->GetBitmap()->setColor(	Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairRPG->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairGauss->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairEgon->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairHive->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairM249->GetBitmap()->setColor(		Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairDisplacer->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
					pCrosshairSniper->GetBitmap()->setColor(	Color(iHudColor[0], iHudColor[1], iHudColor[2], 50 ));
				}

				// Secondary ammo color
				if( iSecondaryAmmo < 1 )
				{
					pSecondaryAmmoLab->setFgColor( 255, 0, 0, 50 );
					pIconAmmoAR->GetBitmap()->setColor( Color(255, 0, 0, 50));
				}
				else
				{
					pSecondaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
					pIconAmmoAR->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50));
				}

				//sprintf(cString, "%s", pxWeapon->szName);
				//gEngfuncs.Con_Printf( "^3HLU -> CHudNew -> %s\n", pxWeapon->szName );

				// Show / Hide -> 9mm (glock) ammo
				if( !strcmp(pxWeapon->szName, "weapon_glock") || !strcmp(pxWeapon->szName, "weapon_9mmhandgun"))
				{
					pIconAmmoGlock->setVisible(true);
					pCrosshairGlock->setVisible(true);
				}

				// Show / Hide -> 9mm (mp5) ammo
				if( !strcmp(pxWeapon->szName, "weapon_9mmAR") || !strcmp(pxWeapon->szName, "weapon_mp5"))
				{
					pIconAmmo9mm->setVisible(true);
					pSecondaryAmmoPanel->setVisible(true);
					pCrosshairMP5->setVisible(true);
				}
				
				// Show / Hide -> 357 ammo
				if( !strcmp(pxWeapon->szName, "weapon_357") )
				{
					pIconAmmo357->setVisible(true);
					pCrosshair357->setVisible(true);
				}
				

				// Show / Hide -> Buckshot ammo
				if( !strcmp(pxWeapon->szName, "weapon_shotgun") )
				{
					pIconAmmoBuckshot->setVisible(true);
					pCrosshairShotgun->setVisible(true);
				}
				

				// Show / Hide -> Crossbow ammo
				if( !strcmp(pxWeapon->szName, "weapon_crossbow") )
				{
					pIconAmmoArrow->setVisible(true);
					pCrosshairCrossbow->setVisible(true);
				}
				

				// Show / Hide -> Rocket ammo
				if( !strcmp(pxWeapon->szName, "weapon_rpg") )
				{
					pIconAmmoRocket->setVisible(true);
					pCrosshairRPG->setVisible(true);
				}
				

				// Show / Hide -> Gauss ammo
				if( !strcmp(pxWeapon->szName, "weapon_gauss") )
				{
					pIconAmmoUranium->setVisible(true);
					pCrosshairGauss->setVisible(true);
				}
				
				// Show / Hide -> Egon ammo
				if( !strcmp(pxWeapon->szName, "weapon_egon") )
				{
					pIconAmmoUranium->setVisible(true);
					pCrosshairEgon->setVisible(true);
				}
				
				// Show / Hide -> Bee ammo
				if( !strcmp(pxWeapon->szName, "weapon_hornetgun") )
				{
					pIconAmmoBee->setVisible(true);
					pCrosshairHive->setVisible(true);
				}

				// Show / Hide -> Handgrenade ammo
				if( !strcmp(pxWeapon->szName, "weapon_handgrenade") )
					pIconAmmoGrenade->setVisible(true);

				// Show / Hide -> Satchel ammo
				if( !strcmp(pxWeapon->szName, "weapon_satchel") )
					pIconAmmoSatchel->setVisible(true);
				
				// Show / Hide -> Tripmine ammo
				if( !strcmp(pxWeapon->szName, "weapon_tripmine") )
					pIconAmmoTripmine->setVisible(true);
				
				// Show / Hide -> Snark ammo
				if( !strcmp(pxWeapon->szName, "weapon_snark") )
					pIconAmmoSnark->setVisible(true);

				// Show / Hide -> Deagle ammo
				if( !strcmp(pxWeapon->szName, "weapon_eagle") )
				{
					//pIconAmmoBee->setVisible(true);
					pCrosshairDeagle->setVisible(true);
				}
				
				// Show / Hide -> M249 ammo
				if( !strcmp(pxWeapon->szName, "weapon_m249") )
				{
					//pIconAmmoBee->setVisible(true);
					pCrosshairM249->setVisible(true);
				}
				
				// Show / Hide -> M249 ammo
				if( !strcmp(pxWeapon->szName, "weapon_displacer") )
				{
					pIconAmmoUranium->setVisible(true);
					pCrosshairDisplacer->setVisible(true);
				}
				
				// Show / Hide -> Sniper ammo
				if( !strcmp(pxWeapon->szName, "weapon_sniperrifle") )
				{
					//pIconAmmoUranium->setVisible(true);
					pCrosshairSniper->setVisible(true);
				}

			}
		}
	}

//========== PRIMARY / SECONDARY AMMO PANEL END =======//



//========== PAIN DIRECTION INDICATORS START =======//

	// TOP
	if (fTimer_TopDmg > 4.0)
	{
		fTimer_TopDmg -= (gHUD.m_flTimeDelta * 200);
		if (fTimer_TopDmg <= 0)
			fTimer_TopDmg = 1;

		pPainTopDirIcon->GetBitmap()->setColor( Color( 255, 0, 0, 257-fTimer_TopDmg ));
	}
	else
	{
		fTimer_TopDmg = 0;
		pPainTopDirIcon->setVisible(false);
	}

	// BOTTOM
	if (fTimer_BottomDmg > 4.0)
	{
		fTimer_BottomDmg -= (gHUD.m_flTimeDelta * 200);
		if (fTimer_BottomDmg <= 0)
			fTimer_BottomDmg = 1;

		pPainBottomDirIcon->GetBitmap()->setColor( Color( 255, 0, 0, 257-fTimer_BottomDmg ));
	}
	else
	{
		fTimer_BottomDmg = 0;
		pPainBottomDirIcon->setVisible(false);
	}

	// LEFT
	if (fTimer_LeftDmg > 4.0)
	{
		fTimer_LeftDmg -= (gHUD.m_flTimeDelta * 200);
		if (fTimer_LeftDmg <= 0)
			fTimer_LeftDmg = 1;

		pPainLeftDirIcon->GetBitmap()->setColor( Color( 255, 0, 0, 257-fTimer_LeftDmg ));
	}
	else
	{
		fTimer_LeftDmg = 0;
		pPainLeftDirIcon->setVisible(false);
	}

	// RIGHT
	if (fTimer_RightDmg > 4.0)
	{
		fTimer_RightDmg -= (gHUD.m_flTimeDelta * 200);
		if (fTimer_RightDmg <= 0)
			fTimer_RightDmg = 1;

		pPainRightDirIcon->GetBitmap()->setColor( Color( 255, 0, 0, 257-fTimer_RightDmg ));
	}
	else
	{
		fTimer_RightDmg = 0;
		pPainRightDirIcon->setVisible(false);
	}

//========== PAIN DIRECTION INDICATORS END =======//



//========== AIR COUNTER START =======//
	if(pAirLab && pAirIcon)
	{
		// Fograin92: Only display Air Counter if air value is below 100
		if( fAir < 100 )
		{
			pAirIcon->setVisible(true);
			pAirLab->setVisible(true);
		}
		else
		{
			pAirIcon->setVisible(false);
			pAirLab->setVisible(false);
		}

		
		// Fograin92: Change color if value is below 26
		if(fAir < 26)
		{
			pAirIcon->GetBitmap()->setColor( Color(255, 0, 0, 50) );
			pAirLab->setFgColor( 255, 0, 0, 50 );
		}
		else
		{
			pAirIcon->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 50) );
			pAirLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
		}
		

		pAirLab->setText("%d", (int)fAir);
	}
//========== AIR COUNTER END =======//


//========== ATMOSPHERIC CONTAMINANT SENSORS START =======//

	// ONLY if it's not LOGON animation
	if (fTimer_Logon < 1)
	{
		// Limit icons
		if (iDmgActiveIcons < 0)
			iDmgActiveIcons = 0;
		else if (iDmgActiveIcons > 8)
			iDmgActiveIcons = 8;

		if (pDmgAir)
		{
			if (fTimer_iDmgAir > 4.0)
			{
				fTimer_iDmgAir -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgAir <= 0)
					fTimer_iDmgAir = 1;

				// Fograin92: Check Air counter objects
				if( pAirIcon && pAirLab )
				{
					pAirIcon->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgAir) );
					pAirLab->setFgColor( 255, 0, 0, 257-fTimer_iDmgAir );
				}

				pDmgAir->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgAir));
			}
			else
			{
				if( pAirIcon->isVisible() && pAirLab->isVisible() && fAir < 1 )
				{
					// Fograin92: Reset flash timer
					fTimer_iDmgAir = 255;
				}
				else
				{
					fTimer_iDmgAir = 0;
					//DamageSensor(1, false);
				}
			}
		}

		if (pDmgBio)
		{
			if (fTimer_iDmgBio > 4.0)
			{
				fTimer_iDmgBio -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgBio <= 0)
					fTimer_iDmgBio = 1;

				pDmgBio->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgBio));
			}
			else
			{
				fTimer_iDmgBio = 0;
				DamageSensor(2, false);
			}
		}

		if (pDmgChem)
		{
			if (fTimer_iDmgChem > 4.0)
			{
				fTimer_iDmgChem -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgChem <= 0)
					fTimer_iDmgChem = 1;

				pDmgChem->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgChem));
			}
			else
			{
				fTimer_iDmgChem = 0;
				DamageSensor(3, false);
			}
		}

		if (pDmgFire)
		{
			if (fTimer_iDmgFire > 4.0)
			{
				fTimer_iDmgFire -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgFire <= 0)
					fTimer_iDmgFire = 1;

				pDmgFire->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgFire));
			}
			else
			{
				fTimer_iDmgFire = 0;
				DamageSensor(4, false);
			}
		}

		if (pDmgFrost)
		{
			if (fTimer_iDmgFrost > 4.0)
			{
				fTimer_iDmgFrost -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgFrost <= 0)
					fTimer_iDmgFrost = 1;

				pDmgFrost->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgFrost));
			}
			else
			{
				fTimer_iDmgFrost = 0;
				DamageSensor(5, false);
			}
		}

		if (pDmgGas)
		{
			if (fTimer_iDmgGas > 4.0)
			{
				fTimer_iDmgGas -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgGas <= 0)
					fTimer_iDmgGas = 1;

				pDmgGas->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgGas));
			}
			else
			{
				fTimer_iDmgGas = 0;
				DamageSensor(6, false);
			}
		}

		if (pDmgRad)
		{
			if (fTimer_iDmgRad > 4.0)
			{
				fTimer_iDmgRad -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgRad <= 0)
					fTimer_iDmgRad = 1;

				pDmgRad->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgRad));
			}
			else
			{
				fTimer_iDmgRad = 0;
				DamageSensor(7, false);
			}
		}

		if (pDmgShock)
		{
			if (fTimer_iDmgShock > 4.0)
			{
				fTimer_iDmgShock -= (gHUD.m_flTimeDelta * 200);
				if (fTimer_iDmgShock <= 0)
					fTimer_iDmgShock = 1;

				pDmgShock->GetBitmap()->setColor( Color(255, 0, 0, 257-fTimer_iDmgShock));
			}
			else
			{
				fTimer_iDmgShock = 0;
				DamageSensor(8, false);
			}
		}

	}	// END fTimer_Logon < 1

//========== ATMOSPHERIC CONTAMINANT SENSORS END =======//








//========== H.E.V. LOGON/INTRO SEQUENCE START =======//
	
	if (fTimer_Logon > 0)
	{
		//gEngfuncs.Con_Printf( "^2HLU -> fTimer_Logon -> %.0f\n", fTimer_Logon );
		char cStringA[1024] = "";
		char cStringB[1024] = "";


		// Fograin92: First let's clean the HUD elements, before animation starts
		if( fTimer_Logon < 2 )
		{
			// Hide HUD elements (leave panels, but hide contents)
			pHealthPanel->setVisible(true);
			pHealthIcon->setVisible(false);
			pHealthLab->setVisible(false);
			pArmorIconHLc->setVisible(false);
			pArmorIconHL->setVisible(false);
			pArmorLab->setVisible(false);

			pAmmoPanel->setVisible(true);
			pPrimaryAmmoLab->setVisible(false);
			pSecondaryAmmoPanel->setVisible(true);
			pSecondaryAmmoLab->setVisible(false);

			pIconAmmo9mm->setVisible(false);
			pIconAmmoGlock->setVisible(false);
			pIconAmmo357->setVisible(false);			
			pIconAmmoAR->setVisible(false);			
			pIconAmmoBuckshot->setVisible(false);		
			pIconAmmoArrow->setVisible(false);	
			pIconAmmoRocket->setVisible(false);		
			pIconAmmoUranium->setVisible(false);		
			pIconAmmoBee->setVisible(false);			
			pIconAmmoGrenade->setVisible(false);
			pIconAmmoSatchel->setVisible(false);
			pIconAmmoTripmine->setVisible(false);
			pIconAmmoSnark->setVisible(false);

			pPainTopDirIcon->setVisible(false);
			pPainBottomDirIcon->setVisible(false);
			pPainLeftDirIcon->setVisible(false);
			pPainRightDirIcon->setVisible(false);

			pAirPanel->setVisible(true);
			pAirIcon->setVisible(false);
			pAirLab->setVisible(false);

			iTimerSpeed = HEV_PULSE_SPD;
			//fTimer_Logon = 566;

			// Fograin92: If this is Blue-Shift OR player disabled logon then skip the whole logon text
			if( (CVAR_GET_FLOAT("sm_hud") == 1 ) || (CVAR_GET_FLOAT("sm_hev_logon") == 0) )
			{
				fTimer_Logon = 566;
				bShortLogon = true;
			}

			pLogonText->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 50 );
			pLogonText->setBgColor( 0, 0, 0, 0 );
		}



//========= HEV/PCV LOGON PHASE 1 START =========//
		if( fTimer_Logon <= 565 )
		{
			pLogonText->setVisible(true);
			pLogonText->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
			
			// Pulse "_" sign for first 15 ticks
			if( fTimer_Logon < 15 )
			{
				// Play HEV INIT/BELL SOUND
				if( !bSoundPlaying )
				{
					// Fograin92: If it's opposing force
					if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
						gSoundEngine.PlaySound("player/pcv_vest.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
					else
						gSoundEngine.PlaySound("fvox/bell.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
					
					bSoundPlaying = true;
				}

				// Set new timer speed
				iTimerSpeed = HEV_PULSE_SPD;
			}

			// Load Copyright text
			else if( (fTimer_Logon >= 15) && (fTimer_Logon <= 163) )
			{
				bSoundPlaying = false;	// Reset sound
				iTimerSpeed = HEV_READ_SPD;	// Speed up timer
			}

			// Load the rest of the HEV_LOGON_01 text
			else if( (fTimer_Logon > 163) && (fTimer_Logon <= 168) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 168) && (fTimer_Logon <= 219) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 219) && (fTimer_Logon <= 224) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 224) && (fTimer_Logon <= 243) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 243) && (fTimer_Logon <= 252) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 252) && (fTimer_Logon <= 272) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 272) && (fTimer_Logon <= 275) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 275) && (fTimer_Logon <= 294) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 294) && (fTimer_Logon <= 298) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 298) && (fTimer_Logon <= 353) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 353) && (fTimer_Logon <= 356) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 356) && (fTimer_Logon <= 390) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 390) && (fTimer_Logon <= 396) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 396) && (fTimer_Logon <= 425) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 425) && (fTimer_Logon <= 429) )	iTimerSpeed = HEV_PULSE_SPD;	// Wait
			else if( (fTimer_Logon > 429) && (fTimer_Logon <= 460) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else if( (fTimer_Logon > 460) && (fTimer_Logon <= 466) )	iTimerSpeed = HEV_READ_SPD;		// Wait
			else if( (fTimer_Logon > 466) && (fTimer_Logon <= 530) )	iTimerSpeed = HEV_READ_SPD;		// Read
			else
			{
				// Set new timer speed
				iTimerSpeed = HEV_PULSE_SPD;
			}

			// Check if we need to display new chars
			if( iTimerSpeed == HEV_READ_SPD)
			{
				// If current timer value is different than old timer value
				if( (int)fTimer_Logon != (int)fTimer_LogonOLD )
				{
					iScanNum++; // Increment scan number
					fTimer_LogonOLD = fTimer_Logon; // Set new timer value
				}
			}

			// Fograin92: Check if it's HEV or PCV
			if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
			{
				// Fograin92: It's Opposing Force, let's play PCV startup
				sprintf( cStringB, "%s", SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#PCV_LOGON_01" ) ) );
			}
			else
			{
				// Fograin92: It's NOT Opposing Force, play default HEV startup
				sprintf( cStringB, "%s", SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#HEV_LOGON_01" ) ) );
			}

			// Load text from HEV_LOGON_01 (PHASE 1)
			strncpy( cStringA, cStringB, iScanNum );

			// Let's use simple modulo here to create pulsing "_" sign
			if( (int)fTimer_Logon % 2 == 0 )
				pLogonText->setText( "%s_\n", cStringA);
			else
				pLogonText->setText( "%s\n", cStringA);
		}
//========= HEV LOGON PHASE 1 END =========//


		// Fograin92: Check if it's a short logon or full logon
		if( (fTimer_Logon > 565) && bShortLogon )
		{
			// Fograin92: It's a HEV short logon or PCV pickup, skip to cleanup phase
			fTimer_Logon = 0;
		}



//========= HEV LOGON PHASE 2 START =========//
		// Fograin92: This phase is executed when it's a full logon
		else if( (fTimer_Logon > 565) && (fTimer_Logon <= 4000) )
		{
			// Clear text and play HEV LOGON sound
			if( fTimer_Logon <= 567 )
			{
				iScanNum = 0;
				pLogonText->setText("");
				pLogonText->setVisible(false);

				// Play LOGON SOUND
				if( !bSoundPlaying )
				{
					gSoundEngine.PlaySound("fvox/hev_logon.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
					bSoundPlaying = true;
				}
				fTimer_Logon = 611;
			}
			
			// hev_logon.wav - part
			else if( (fTimer_Logon > 612) && (fTimer_Logon <= 735) )
			{
				bSoundPlaying = false;
				
				// HEV LOGON img01 - fade in
				if( fTimer_Logon <= 632 )
					iAlpha = iAlpha - 1;
				
				// HEV LOGON img01 - fade out
				if( fTimer_Logon >= 715 )
					iAlpha = iAlpha + 1;

				// Limit alpha range
				if( iAlpha < 4 )
					iAlpha = 4;

				if( iAlpha > 255 )
					iAlpha = 255;

				pImgLogon01->setVisible(true);
				//pImgLogon01->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], iAlpha) );
			}
			
			// powerarmor_on.wav - part
			else if( (fTimer_Logon > 735) && (fTimer_Logon <= 790) )
			{
				pImgLogon01->setVisible(false);
				pImgLogon02->setVisible(true);
				pArmorIconHL->setVisible(true);
				pArmorIconHLc->setVisible(true);
				pArmorLab->setVisible(true);

				iArmor = 0;

				if( fTimer_Logon <= 750 )
				{
					pArmorLab->setText( "0" );
				}
				else if( (fTimer_Logon > 750) && (fTimer_Logon <= 755) )
				{
					pArmorLab->setText( "00" );
				}
				else if( (fTimer_Logon > 755) && (fTimer_Logon < 760) )
				{
					pArmorLab->setText( "000" );
				}
				else
				{
					iArmor = ((int)fTimer_Logon-760)+1;

					if( iArmor > 25 )
						iArmor = 25; // Cap the value at 25 max
				}

				iAlpha = 255; // Reset alpha
			}

			
			// atmospherics_on.wav - part
			else if( (fTimer_Logon > 790) && (fTimer_Logon <= 850) )
			{
				//pLogonText->setText( "atmospherics_on -> %d\n", (int)fTimer_Logon);	// DEBUG
				pImgLogon02->setVisible(false);
				pImgLogon03->setVisible(true);


				// Show air counter
				pAirIcon->setVisible(true);
				pAirLab->setVisible(true);

				// Animate air counter
				int iFakeAir = (((int)fTimer_Logon-800)*2)+2;

				if( iFakeAir > 100)
					iFakeAir = 100;
				
				if( iFakeAir < 0)
					iFakeAir = 0;

				if( iFakeAir < 26 )
				{
					pAirIcon->GetBitmap()->setColor( Color(255, 0, 0, 1) );
					pAirLab->setFgColor( 255, 0, 0, 0 );
				}
				else
				{
					pAirIcon->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pAirLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
				}
					
				pAirLab->setText("%d", iFakeAir);



				
				// Show damage icons
				if( (fTimer_Logon >= 791) && (fTimer_Logon <= 795) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );

					pDmgAir->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 795) && (fTimer_Logon <= 800) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );

					pDmgAir->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgBio->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 800) && (fTimer_Logon <= 805) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );


					pDmgBio->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgChem->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 805) && (fTimer_Logon <= 810) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );

					pDmgChem->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgFire->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 810) && (fTimer_Logon <= 815) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );

					pDmgFire->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgFrost->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 815) && (fTimer_Logon <= 820) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, false );
					DamageSensor( 8, false );

					pDmgFrost->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgGas->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 820) && (fTimer_Logon <= 825) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, false );

					pDmgGas->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgRad->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 825) && (fTimer_Logon <= 830) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgRad->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
					pDmgShock->GetBitmap()->setColor( Color(255, 0, 0, 1) );
				}

				if( (fTimer_Logon > 830) && (fTimer_Logon <= 834) )
				{
					DamageSensor( 1, true );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgShock->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
				}

				// Hide damage icons
				if( (fTimer_Logon > 834) && (fTimer_Logon <= 836) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, true );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgBio->setPos		(0,		0);
					pDmgChem->setPos	(64*1,	0);
					pDmgFire->setPos	(64*2,	0);
					pDmgFrost->setPos	(64*3,	0);
					pDmgGas->setPos		(64*4,	0);
					pDmgRad->setPos		(64*5,	0);
					pDmgShock->setPos	(64*6,	0);

					pDmgShock->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1) );
				}

				if( (fTimer_Logon > 836) && (fTimer_Logon <= 838) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, true );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgChem->setPos	(0,	0);
					pDmgFire->setPos	(64*1,	0);
					pDmgFrost->setPos	(64*2,	0);
					pDmgGas->setPos		(64*3,	0);
					pDmgRad->setPos		(64*4,	0);
					pDmgShock->setPos	(64*5,	0);
				}

				if( (fTimer_Logon > 838) && (fTimer_Logon <= 840) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, true );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgFire->setPos	(0,	0);
					pDmgFrost->setPos	(64*1,	0);
					pDmgGas->setPos		(64*2,	0);
					pDmgRad->setPos		(64*3,	0);
					pDmgShock->setPos	(64*4,	0);
				}

				if( (fTimer_Logon > 840) && (fTimer_Logon <= 842) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, true );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgFrost->setPos	(0,	0);
					pDmgGas->setPos		(64*1,	0);
					pDmgRad->setPos		(64*2,	0);
					pDmgShock->setPos	(64*3,	0);
				}

				if( (fTimer_Logon > 842) && (fTimer_Logon <= 844) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, true );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgGas->setPos		(0,	0);
					pDmgRad->setPos		(64*1,	0);
					pDmgShock->setPos	(64*2,	0);
				}

				if( (fTimer_Logon > 844) && (fTimer_Logon <= 846) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, true );
					DamageSensor( 8, true );

					pDmgRad->setPos(0, 0);
					pDmgShock->setPos(64, 0);
				}

				if( (fTimer_Logon > 846) && (fTimer_Logon <= 848) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, true );

					pDmgShock->setPos(0,0);
				}

				if( (fTimer_Logon > 848) && (fTimer_Logon <= 850) )
				{
					DamageSensor( 1, false );
					DamageSensor( 2, false );
					DamageSensor( 3, false );
					DamageSensor( 4, false );
					DamageSensor( 5, false );
					DamageSensor( 6, false );
					DamageSensor( 7, false );
					DamageSensor( 8, false );	
				}

			}
			
			// vitalsigns_on.wav + automedic_on.wav - part
			else if( (fTimer_Logon > 850) && (fTimer_Logon <= 935) )
			{
				pImgLogon03->setVisible(false);
				pImgLogon04->setVisible(true);

				pHealthIcon->setVisible(true);
				pHealthLab->setVisible(true);

				if( fTimer_Logon < 885 )
				{
					// Set one digit for now
					if( (fTimer_Logon >= 850) && (fTimer_Logon < 860) )
					{
						// Set dafault health value to 0
						iHealth = 0;
						pHealthLab->setText( "0" );
					}

					// Animate pain counters
					if( (fTimer_Logon >= 860) && (fTimer_Logon < 865) )
					{
						pHealthLab->setText( "00" );
						pPainTopDirIcon->setVisible(true);
						pPainRightDirIcon->setVisible(false);
						pPainBottomDirIcon->setVisible(false);
						pPainLeftDirIcon->setVisible(false);
					}
					else if( (fTimer_Logon >= 865) && (fTimer_Logon < 870) )
					{
						pPainTopDirIcon->setVisible(false);
						pPainRightDirIcon->setVisible(true);
						pPainBottomDirIcon->setVisible(false);
						pPainLeftDirIcon->setVisible(false);
					}
					else if( (fTimer_Logon >= 870) && (fTimer_Logon < 875) )
					{
						pPainTopDirIcon->setVisible(false);
						pPainRightDirIcon->setVisible(false);
						pPainBottomDirIcon->setVisible(true);
						pPainLeftDirIcon->setVisible(false);
					}
					else if( (fTimer_Logon >= 875) && (fTimer_Logon < 880) )
					{
						pPainTopDirIcon->setVisible(false);
						pPainRightDirIcon->setVisible(false);
						pPainBottomDirIcon->setVisible(false);
						pPainLeftDirIcon->setVisible(true);
					}
				}
				else
				{
					pImgLogon04->setVisible(false);
					pImgLogon05->setVisible(true);

					// Clear pain indicators
					pPainTopDirIcon->setVisible(false);
					pPainRightDirIcon->setVisible(false);
					pPainBottomDirIcon->setVisible(false);
					pPainLeftDirIcon->setVisible(false);

					iHealth = (((int)fTimer_Logon-885)*2)+2;
					if( iHealth > 100 )
						iHealth = 100; // Cap the value at 100 max	
				}
			}
			
			// weaponselect_on.wav - part
			else if( (fTimer_Logon > 935) && (fTimer_Logon <= 980) )
			{
				pImgLogon05->setVisible(false);
				pImgLogon06->setVisible(true);
				//pLogonText->setText( "weaponselect_on -> %d\n", (int)fTimer_Logon);	// DEBUG

				pAirIcon->setVisible(false);
				pAirLab->setVisible(false);
			}

			// munitionview_on.wav - part
			else if( (fTimer_Logon > 980) && (fTimer_Logon <= 1027) )
			{
				pLogonText->setText( "munitionview_on -> %d\n", (int)fTimer_Logon);	// DEBUG
				pImgLogon06->setVisible(false);
				pImgLogon07->setVisible(true);

				// Make 100% sure that panels are rendered
				pAmmoPanel->setVisible(true);
				pPrimaryAmmoLab->setVisible(true);
				pSecondaryAmmoPanel->setVisible(true);
				pSecondaryAmmoLab->setVisible(true);

				if( (fTimer_Logon > 980) && (fTimer_Logon <= 990) )
				{
					pPrimaryAmmoLab->setText("-- | ---");
					pPrimaryAmmoLab->setFgColor( 255, 0, 0, 0 );

					pSecondaryAmmoLab->setText("--");
					pSecondaryAmmoLab->setFgColor( 255, 0, 0, 0 );

					pIconAmmo9mm->setVisible(true);
					pIconAmmo9mm->GetBitmap()->setColor( Color(255, 0, 0, 1));

					pIconAmmoAR->setVisible(true);
					pIconAmmoAR->GetBitmap()->setColor( Color(255, 0, 0, 1));
				}

				if( (fTimer_Logon > 990) && (fTimer_Logon <= 992) )
				{
					pPrimaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
					pPrimaryAmmoLab->setText("0- | ---");
				}
				if( (fTimer_Logon > 992) && (fTimer_Logon <= 994) )		pPrimaryAmmoLab->setText("-0 | ---");
				if( (fTimer_Logon > 994) && (fTimer_Logon <= 996) )		pPrimaryAmmoLab->setText("-- | 0--");
				if( (fTimer_Logon > 996) && (fTimer_Logon <= 998) )		pPrimaryAmmoLab->setText("-- | -0-");
				if( (fTimer_Logon > 998) && (fTimer_Logon <= 1000) )	pPrimaryAmmoLab->setText("-- | --0");
				if( (fTimer_Logon > 1000) && (fTimer_Logon <= 1002) )	pPrimaryAmmoLab->setText("-- | -00");
				if( (fTimer_Logon > 1002) && (fTimer_Logon <= 1004) )	pPrimaryAmmoLab->setText("-- | 000");
				if( (fTimer_Logon > 1004) && (fTimer_Logon <= 1006) )	pPrimaryAmmoLab->setText("-0 | 000");
				if( (fTimer_Logon > 1006) && (fTimer_Logon <= 1008) )	pPrimaryAmmoLab->setText("00 | 000");

				if( (fTimer_Logon > 1010) && (fTimer_Logon <= 1012) )
				{
					pSecondaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
					pIconAmmoAR->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
					pSecondaryAmmoLab->setText("0-");
				}

				if( (fTimer_Logon > 1012) && (fTimer_Logon <= 1014) )	pSecondaryAmmoLab->setText("-0");
				if( (fTimer_Logon > 1014) && (fTimer_Logon <= 1016) )	pSecondaryAmmoLab->setText("00");

				if( (fTimer_Logon > 1020) && (fTimer_Logon <= 1022) )
				{
					pPrimaryAmmoLab->setFgColor( 255, 0, 0, 0 );
					pSecondaryAmmoLab->setFgColor( 255, 0, 0, 0 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(255, 0, 0, 1));
					pIconAmmoAR->GetBitmap()->setColor( Color(255, 0, 0, 1));
				}

				if( (fTimer_Logon > 1022) && (fTimer_Logon <= 1024) )
				{
					pPrimaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
					pSecondaryAmmoLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
					pIconAmmoAR->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
				}

				if( (fTimer_Logon > 1024) && (fTimer_Logon <= 1026) )
				{
					pPrimaryAmmoLab->setFgColor( 255, 0, 0, 0 );
					pSecondaryAmmoLab->setFgColor( 255, 0, 0, 0 );
					pIconAmmo9mm->GetBitmap()->setColor( Color(255, 0, 0, 1));
					pIconAmmoAR->GetBitmap()->setColor( Color(255, 0, 0, 1));
				}

				
				if( fTimer_Logon > 1026)
				{
					pSecondaryAmmoPanel->setVisible(false);
					pAmmoPanel->setVisible(false);
				}

			}
			// communications_on.wav - part
			else if( (fTimer_Logon > 1027) && (fTimer_Logon <= 1070) )
			{
				pImgLogon07->setVisible(false);
				pImgLogon08->setVisible(true);
				//pLogonText->setText( "communications_on -> %d\n", (int)fTimer_Logon);	// DEBUG
			}
			// safe_day.wav - part
			else if( (fTimer_Logon > 1070) && (fTimer_Logon <= 1130) )
			{
				pImgLogon08->setVisible(false);
				//pLogonText->setText( "safe_day -> %d\n", (int)fTimer_Logon);	// DEBUG
			}
			else if( fTimer_Logon > 1130 )
			{
				// Fograin92: Logon finished
				fTimer_Logon = 0;
			}
		}
//========= HEV LOGON PHASE 2 END =========//


		// Fograin92: Check if we did disable timer (aka. short logon)
		if( fTimer_Logon < 1 )
		{
			// Fograin92: Clean logon text
			pLogonText->setText("");
			pLogonText->setVisible(false);

			// Fograin92: Display health icon and HP level
			pHealthIcon->setVisible(true);
			pHealthLab->setVisible(true);

			// Fograin92: Display armor icons and power level
			pArmorIconHL->setVisible(true);
			pArmorIconHLc->setVisible(true);
			pArmorLab->setVisible(true);

			// Fograin92: Clean ammo panels
			pAmmoPanel->setVisible(false);
			pPrimaryAmmoLab->setVisible(true);
			pSecondaryAmmoPanel->setVisible(false);
			pSecondaryAmmoLab->setVisible(true);
			pIconAmmoAR->setVisible(true);
		}
		else
		{
			// Fograin92: The animation is still going, icrement timer and let the magic of animation happen
			fTimer_Logon += (gHUD.m_flTimeDelta * iTimerSpeed);
		}


	}
	// ELSE -> fTimer_Logon == 0 or less, disable animation
	else
	{
		fTimer_Logon = 0;
	}

// Animate LOGON/INTRO sequence - END

	

	Panel::paint();		// Proceed with rendering
}


