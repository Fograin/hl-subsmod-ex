//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Fograin92
//	This code might contain parts written by: BUzer
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "sm_hud.h"
#include <windows.h>	// MessageBox functionality


//======= USEFUL CLASSES/TEMPLATES =======//


// Load best possible result for our resolution
// Written by: BUzer | Edited by: Fograin92
BitmapTGA* LoadResolutionImage (const char *imgname)
{
	BitmapTGA *pBitmap;
	static int resArray[] =
	{
		1024,
		1280,
		1600
	};

	// Try to load image directly (if %d is not specified)
	pBitmap = vgui_LoadTGA(imgname);

	// We can't load image directly
	if (!pBitmap)
	{
		int resArrayIndex = 0;
		int i = 0;
		while ((resArray[i] <= ScreenWidth) && (i < 3))
		{
			resArrayIndex = i;
			i++;
		}

		// Try to load resolution based image
		while(pBitmap == NULL && resArrayIndex >= 0)
		{
			char imgName[64];
			sprintf(imgName, imgname, resArray[resArrayIndex]);
			gEngfuncs.Con_Printf( "^3-> %s\n", imgName );
			pBitmap = vgui_LoadTGA(imgName);
			resArrayIndex--;
		}
	}

	return pBitmap;
}


// Simple class that owns pointer to a bitmap and draws it
// Written by: BUzer
class ImageHolder : public Panel
{
public:
	ImageHolder(const char *imgname, Panel *parent) : Panel(0, 0, 10, 10)
	{
		
		m_pBitmap = LoadResolutionImage(imgname);
		if (m_pBitmap)
		{
			setParent(parent);
			setPaintBackgroundEnabled(false);
			setVisible(true);
			m_pBitmap->setPos(0, 0);
		}
		
	}

	~ImageHolder() {delete m_pBitmap;}
	BitmapTGA *GetBitmap() {return m_pBitmap;}

protected:
	virtual void paint()
	{
		if (m_pBitmap)
				m_pBitmap->doPaint(this);
	}

	BitmapTGA *m_pBitmap;
};



//======= UTILITY FUNCTIONS =======//

// IDs for all HUB objects
enum hudobjects_e
{
	ID_HUD = 0,		// New HUD ID
	ID_HA_PANEL,	// Health+Armor panel
	ID_HEALTH_ICON,	// Health Icon
	ID_HEALTH_LAB,	// Health value label
	ID_ARMOR_ICON,	// Armor Icon
	ID_ARMOR_LAB,	// Armor value label

	AMOUNT			// Length of this enum
};


// Adjustment array
const int iAdjustmentArray[][5] =
{
	//--OBJECT ID-------ResX----ResY---AdjX---AdjY

	// Health+Armor panel adjustments
	{ ID_HA_PANEL,	1024,	576,	5,		540	},
	{ ID_HA_PANEL,	1024,	600,	5,		565	},
	{ ID_HA_PANEL,	1280,	720,	5,		680	},
	{ ID_HA_PANEL,	1360,	768,	5,		730	},
	{ ID_HA_PANEL,	1366,	768,	5,		730	},
	{ ID_HA_PANEL,	1440,	900,	5,		860	},
	{ ID_HA_PANEL,	1600,	900,	5,		860	},
	{ ID_HA_PANEL,	1680,	1050,	5,		1005},
	{ ID_HA_PANEL,	1920,	1080,	5,		1035},
	{ ID_HA_PANEL,	1920,	1200,	5,		1035},
	{ ID_HA_PANEL,	2560,	1600,	5,		1035},


	// Health icon adjustments
	{ ID_HEALTH_ICON,	1024,	576,	0,		0},
	{ ID_HEALTH_ICON,	1024,	600,	0,		0},
	{ ID_HEALTH_ICON,	1280,	720,	0,		5},
	{ ID_HEALTH_ICON,	1360,	768,	0,		5},
	{ ID_HEALTH_ICON,	1366,	768,	0,		5},
	{ ID_HEALTH_ICON,	1440,	900,	0,		5},
	{ ID_HEALTH_ICON,	1600,	900,	0,		5},
	{ ID_HEALTH_ICON,	1680,	1050,	0,		10},
	{ ID_HEALTH_ICON,	1920,	1080,	0,		10},
	{ ID_HEALTH_ICON,	1920,	1200,	0,		10},
	{ ID_HEALTH_ICON,	2560,	1600,	0,		10},


	// Health Label adjustments
	{ ID_HEALTH_LAB,	1024,	576,	30,		0 },
	{ ID_HEALTH_LAB,	1024,	600,	30,		0 },
	{ ID_HEALTH_LAB,	1280,	720,	30,		0 },
	{ ID_HEALTH_LAB,	1360,	768,	30,		0 },
	{ ID_HEALTH_LAB,	1366,	768,	30,		0 },
	{ ID_HEALTH_LAB,	1440,	900,	30,		0 },
	{ ID_HEALTH_LAB,	1600,	900,	30,		0 },
	{ ID_HEALTH_LAB,	1680,	1050,	30,		0 },
	{ ID_HEALTH_LAB,	1920,	1080,	30,		0 },
	{ ID_HEALTH_LAB,	1920,	1200,	30,		0 },
	{ ID_HEALTH_LAB,	2560,	1600,	30,		0 },


	// Armor icon adjustments
	{ ID_ARMOR_ICON,	1024,	576,	100,	0 },
	{ ID_ARMOR_ICON,	1024,	600,	100,	0 },
	{ ID_ARMOR_ICON,	1280,	720,	120,	5 },
	{ ID_ARMOR_ICON,	1360,	768,	120,	5 },
	{ ID_ARMOR_ICON,	1366,	768,	120,	5 },
	{ ID_ARMOR_ICON,	1440,	900,	120,	5 },
	{ ID_ARMOR_ICON,	1600,	900,	140,	5 },
	{ ID_ARMOR_ICON,	1680,	1050,	140,	10 },
	{ ID_ARMOR_ICON,	1920,	1080,	145,	10 },
	{ ID_ARMOR_ICON,	1920,	1200,	145,	10 },
	{ ID_ARMOR_ICON,	2560,	1600,	160,	10 },


	// Armor Label adjustments
	{ ID_ARMOR_LAB,		1024,	576,	130,	0 },
	{ ID_ARMOR_LAB,		1024,	600,	130,	0 },
	{ ID_ARMOR_LAB,		1280,	720,	150,	0 },
	{ ID_ARMOR_LAB,		1360,	768,	150,	0 },
	{ ID_ARMOR_LAB,		1366,	768,	150,	0 },
	{ ID_ARMOR_LAB,		1440,	900,	150,	0 },
	{ ID_ARMOR_LAB,		1600,	900,	170,	0 },
	{ ID_ARMOR_LAB,		1680,	1050,	170,	0 },
	{ ID_ARMOR_LAB,		1920,	1080,	175,	0 },
	{ ID_ARMOR_LAB,		1920,	1200,	175,	0 },
	{ ID_ARMOR_LAB,		2560,	1600,	190,	0 }
};


// Return adjusted position of HUD object, based on current screen resolution
int CHudNew::AdjustPosition(int iHudObjectID, bool bReturnY)
{
	// If we already know that resolution is not supported, skip this whole code
	if(bResError)
		return 0;

	//int iArraySizeX = sizeof iAdjustmentArray[0] / sizeof(int);
	int iArraySizeY = sizeof iAdjustmentArray / sizeof iAdjustmentArray[0];
	gEngfuncs.Con_Printf( "^2SM-> Screen resolution: %dx%d\n", ScreenWidth, ScreenHeight);


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
		//TODO: Change HUD images
		iHudColor[0] = 0;	// R
		iHudColor[1] = 96;	// G
		iHudColor[2] = 180;	// B
	}

	// Opposing Force HUD
	else if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
	{
		//TODO: Change HUD images
		iHudColor[0] = 0;	// R
		iHudColor[1] = 160;	// G
		iHudColor[2] = 0;	// B
	}

	// Default / Half-Life 1 HUD
	else
	{
		//TODO: Change HUD images
		iHudColor[0] = 255;	// R
		iHudColor[1] = 160;	// G
		iHudColor[2] = 0;	// B
	}
}

// Check if we should draw HUD
// Called every CHud:Redraw tick
int CHudNew::ShouldDrawHUD()
{
	//gEngfuncs.Con_Printf( "^3SM-> CHudNew -> ShouldDrawHUD()\n" );

	// hud_draw == 0, hide hud
	if (!gHUD.m_pCvarDraw->value)
	{
		//gEngfuncs.Con_Printf( "^3 hud_draw == 0\n" );
		this->setVisible(false);
		return 0;
	}

	// Check if we have HEV (also counts for BS) or PCV
	//if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
	// For some retarted reason we can't check m_iWeaponBits for WEAPON_SUIT, so let's do this retarted way
	if (!bHaveHEV)
	{
		// We don't have HEV/PCV, hide hud
		//gEngfuncs.Con_Printf( "^3 We don't have HEV\n" );
		this->setVisible(false);
		return 0;
	}


	// In any other cases -> show hud
	//gEngfuncs.Con_Printf( "^3Return 1\n" );
	this->setVisible(true);
	return 1;
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

	UpdateHUD();
}

// Mutator <- Send new armor value
void CHudNew::SetArmorVar(int iPlayerArmor)
{
	// If new value is exactly the same as old one
	if (iPlayerArmor == iArmor)
		return;	// We don't need to update

	// Update health value
	iArmor = iPlayerArmor;

	UpdateHUD();
}

// Called when we picked up item
void CHudNew::PickedUpItem( const char *szName )
{
	gEngfuncs.Con_Printf( "^3SM -> CHudNew -> Picked up: %s\n", szName );

	// If we picked up HEV/PCV
	if( !strcmp( szName, "item_suit" ) )
	{
		// TODO: Play HEV animation?
		bHaveHEV = true;
	}

	UpdateHUD();
}



//======= NEW HUD =======//



// New HUD constructor
CHudNew::CHudNew() : Panel(0, 0, XRES(640), YRES(480))
{
	// Let's make our container invisible
	this->setPaintBackgroundEnabled(false);

	// Initial settings
	pSchemes	= gViewPort->GetSchemeManager();	// Get data from scheme manager
	pFont		= pSchemes->getFont( pSchemes->getSchemeHandle( "Subtitles", true, true) );	// Get font
	
	// Initial values
	bHaveHEV = false;
	bResError = false;
	iHealthSizeX = XRES(125);
	iHealthSizeY = YRES(25);
	iHealth = 0;
	iArmor = 0;


// Initialize Health + ARMOR panel START

	// Initialize Health/Armor, background panel / container
	pHealthPanel = new Panel( AdjustPosition(ID_HA_PANEL, false), AdjustPosition(ID_HA_PANEL, true), iHealthSizeX, iHealthSizeY);
	pHealthPanel->setParent(this);
	//pHealthPanel->setBgColor(0, 0, 0, 100);
	pHealthPanel->setPaintBackgroundEnabled(false);

	// Health icon
	pHealthIcon = new ImageHolder("gfx/vgui/vgui_health.tga", pHealthPanel);
	pHealthIcon->setSize(iHealthSizeY, iHealthSizeY);
	pHealthIcon->setPos( AdjustPosition(ID_HEALTH_ICON, false), AdjustPosition(ID_HEALTH_ICON, true) );
	
	// Health value
	pHealthLab = new Label("0%");
	pHealthLab->setFont( pFont );
	pHealthLab->setParent( pHealthPanel );
	pHealthLab->setPaintBackgroundEnabled(false);
	pHealthLab->setPos( AdjustPosition(ID_HEALTH_LAB, false), AdjustPosition(ID_HEALTH_LAB, true) );

	// Armor icon
	pArmorIcon = new ImageHolder("gfx/vgui/vgui_armor.tga", pHealthPanel);
	pArmorIcon->setSize(iHealthSizeY, iHealthSizeY);
	pArmorIcon->setPos( AdjustPosition(ID_ARMOR_ICON, false), AdjustPosition(ID_ARMOR_ICON, true) );
	
	// Health value
	pArmorLab = new Label("0%");
	pArmorLab->setFont( pFont );
	pArmorLab->setParent( pHealthPanel );
	pArmorLab->setPaintBackgroundEnabled(false);
	pArmorLab->setPos( AdjustPosition(ID_ARMOR_LAB, false), AdjustPosition(ID_ARMOR_LAB, true) );


// Health + ARMOR panel  END


	// This is called everytime new level is loaded, so we don't need to execute it here.
	UpdateHUD();	// Fuck it, let's call it, to be 100% sure that initalization part is finished.
}


// Destructor (aka. Shit destroyer)
CHudNew::~CHudNew()
{
	// Clean vars (YES! I'm a safety freak!)
	iHealth = 0;
	iArmor = 0;
	bHaveHEV = false;
	bResError = false;

	// Delete objects
	if(pArmorIcon)
		delete pArmorIcon;

	if(pArmorLab)
		delete pArmorLab;

	if(pHealthIcon)
		delete pHealthIcon;

	if(pHealthLab)
		delete pHealthLab;

	if(pHealthPanel)
		delete pHealthPanel;
}


// This function updates HUD variables (colors, icons, texts) and also checks if we should hide or show HUD
// Called when; New level has been loaded, when savedgame has been loaded and when item has been picked up.
void CHudNew::UpdateHUD()
{
	gEngfuncs.Con_Printf( "^2SM -> CHudNew -> UpdateHUD()\n" );
	char cString[32];	// Helper string
	GetGame();	// Set proper VARS

	// Update Health icon
	if (pHealthIcon)
	{
		if (iHealth < 26)
			pHealthIcon->GetBitmap()->setColor( Color(255, 0, 0, 1) );
		else
			pHealthIcon->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
	}

	// Update Health text value
	if( pHealthLab )
	{
		if (iHealth < 26)
			pHealthLab->setFgColor( 255, 0, 0, 0 );
		else
			pHealthLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );

		sprintf(cString, "%d\%%%%", iHealth);
		pHealthLab->setText( cString );
	}

	// Update Armor icon
	if (pArmorIcon)
	{
		pArmorIcon->GetBitmap()->setColor( Color(iHudColor[0], iHudColor[1], iHudColor[2], 1));
	}

	// Update Armor text value
	if( pArmorLab )
	{
		pArmorLab->setFgColor( iHudColor[0], iHudColor[1], iHudColor[2], 0 );
		sprintf(cString, "%d\%%%%", iArmor);
		pArmorLab->setText( cString );
	}
}

// Called every frame when HUD is visable
void CHudNew::paint()
{
	//gEngfuncs.Con_Printf( "^2SM -> CHudNew -> paint()\n" );
	Panel::paint();		// Proceed with rendering
}


