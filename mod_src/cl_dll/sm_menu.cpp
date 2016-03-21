//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Fograin92
//	This code might contain parts written by: BUzer
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "sm_menu.h"
#include <windows.h>	// MessageBox functionality



//======= UTILITY FUNCTIONS =======//


// IDs for all HUD objects that require adjustments
enum sm_mainmenu_e
{
	ID_MENU = 0,	// New main menu ID
	AMOUNT			// Length of this enum
};


// Adjustment array
const int iMenuAdjustmentArray[][5] =
{
	//--OBJECT ID-------ResX----ResY---AdjX---AdjY
	{ ID_MENU,	1024,	576,	5,		540	},
	{ ID_MENU,	1024,	600,	5,		565	},
	{ ID_MENU,	1280,	720,	5,		680	},
	{ ID_MENU,	1360,	768,	5,		730	},
	{ ID_MENU,	1366,	768,	5,		730	},
	{ ID_MENU,	1440,	900,	5,		860	},
	{ ID_MENU,	1600,	900,	5,		860	},
	{ ID_MENU,	1680,	1050,	5,		1005},
	{ ID_MENU,	1920,	1080,	5,		1035}
};


// Return adjusted position of HUD object, based on current screen resolution
int CMainMenuNew::MenuAdjustPosition(int iHudObjectID, bool bReturnY)
{
	// If we already know that resolution is not supported, skip this whole code
	if(bResError)
		return 0;

	int iArraySizeY = sizeof iMenuAdjustmentArray / sizeof iMenuAdjustmentArray[0];
	//gEngfuncs.Con_Printf( "^2SM-> Screen resolution: %dx%d\n", ScreenWidth, ScreenHeight);

	// Loop through every row from iAdjustmentArray
	for( int iY=0; iY<iArraySizeY; iY++ )
	{
		// Try to match HUD Object with current screen resolution
		if( (iMenuAdjustmentArray[iY][0] == iHudObjectID) && (iMenuAdjustmentArray[iY][1] == ScreenWidth) && (iMenuAdjustmentArray[iY][2] == ScreenHeight) )
		{
			// We found current object ID and screen resolution is in iAdjustmentArray
			if(bReturnY)
			{
				//gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Res in array -> Adjusting Y position.\n", iHudObjectID);
				return iMenuAdjustmentArray[iY][4]; // Return Y adjustment
			}
			else
			{
				//gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Res in array -> Adjusting X position.\n", iHudObjectID);
				return iMenuAdjustmentArray[iY][3]; // Return X adjustment
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
			if( (iMenuAdjustmentArray[iY][0] == iHudObjectID) && (iMenuAdjustmentArray[iY][2] == ScreenHeight) )
			{
				// We found something for this Y resolution
				//gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Custom res -> Adjusting Y position.\n", iHudObjectID);
				return iMenuAdjustmentArray[iY][4];
			}
		}
		// We want X adjustment
		else
		{
			// Let's try to find X resolution match
			if( (iMenuAdjustmentArray[iY][0] == iHudObjectID) && (iMenuAdjustmentArray[iY][1] == ScreenWidth) )
			{
				// We found something for this X resolution
				//gEngfuncs.Con_Printf( "^3SM-> HUD OBJECT ID: %d -> Custom res -> Adjusting X position.\n", iHudObjectID);
				return iMenuAdjustmentArray[iY][3];
			}
		}
	}

	// If we are here, that means we didn't find any possible match for X or Y resolutions. HUD Object id is wrong or player is using fucked up resolution.
	// Let's see if HUD Object with that ID even exists
	for( int iY=0; iY<iArraySizeY; iY++ )
	{
		// HUD Object with that ID exists, so player is using unsupported resolution.
		if( iMenuAdjustmentArray[iY][0] == iHudObjectID )
		{
			char cErrorString[128];
			sprintf(cErrorString, "MAIN MENU: Your screen resolution: %dx%d is not supported.", ScreenWidth, ScreenHeight);
			MessageBox(NULL, cErrorString, "ERROR", MB_OK);
			bResError = true;
			return 0;
		}
	}

	// If we're still here, that means the HUD Object with that ID does NOT exists in array, someone fucked up here...
	char cErrorString[256];
	sprintf(cErrorString, "MAIN MENU: HUD object with id: %d is not inside adjustment array. Yell at programmer, cuz he fucked it up.", ScreenWidth, ScreenHeight);
	MessageBox(NULL, cErrorString, "ERROR", MB_OK);

	return 0;
}


// Reset Main Menu Variables 
void CMainMenuNew::ResetVars(bool bInitOnly)
{
	// Called by constructor and destructor
	if( bInitOnly )
	{
		bDrawMenu = false;
	}

	// foo
}


// Check if we should draw our main menu
// Called every CHud:Redraw tick
int CMainMenuNew::ShouldDrawMenu()
{
	char cMapName[64];
	strcpy( cMapName, gEngfuncs.pfnGetLevelName() );
	//gEngfuncs.Con_Printf( "^3->%s\n", cMapName );

	// Fograin92: Check if main menu map is currently loaded
	if ( !strncmp(cMapName, "maps/sm4_mainmenu", 17) )
	{
		// Fograin92: If yes, we force draw our main menu
		this->bDrawMenu = true;
	}


	// Fograin92: Check if we should draw our menu
	if( this->bDrawMenu )
	{
		// Fograin92: Looks good, let's draw it
		this->setVisible(true);
		this->paint();
		return 1;
	}
	// Fograin92: Else, let's hide our main menu
	else
	{
		this->setVisible(false);
		return 0;
	}

}


// This function updates HUD variables (colors, icons, texts) and also checks if we should hide or show HUD
// Called when; New level has been loaded, when savedgame has been loaded and when item has been picked up.
void CMainMenuNew::UpdateMainMenu()
{
	ShouldDrawMenu();
}



//======= SHARED/MUTATOR FUNCTIONS =======//

// FOO


//======= NEW MAIN MENU =======//



// New main menu constructor
CMainMenuNew::CMainMenuNew() : Panel(0, 0, XRES(640), YRES(480))
{
	// Set main menu variables to initial values
	ResetVars(true);

	this->setBgColor(0, 0, 0, 100);
	this->setPaintBackgroundEnabled(true);

	

	UpdateMainMenu();
}


// New main menu destructor
CMainMenuNew::~CMainMenuNew()
{
	// Clean vars
	ResetVars(true);

	// Delete objects
	// FOO
}



// Called every frame when main menu is visable
void CMainMenuNew::paint()
{
	//gEngfuncs.Con_Printf( "^2HLU -> CMainMenuNew -> paint()\n" );
	//char cString[32];	// Helper string


	Panel::paint();		// Proceed with rendering
}


