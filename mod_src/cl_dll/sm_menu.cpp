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


// IDs for all main menu objects that require adjustments
enum sm_mainmenu_e
{
	ID_MENU = 0,		// New main menu ID

	ID_LABEL_GAMETITLE,
	ID_PANEL_MAINMENU,
	ID_BTN_CONSOLE,
	ID_BTN_NEWGAME,
	ID_BTN_LOADGAME,
	ID_BTN_SAVEGAME,
	ID_BTN_OPTIONS,
	ID_BTN_EXTRAS,
	ID_BTN_QUIT,

	AMOUNT				// Length of this enum
};


// Adjustment array
const int iMenuAdjustmentArray[][5] =
{
	//--OBJECT ID-------ResX----ResY---AdjX---AdjY
	{ ID_LABEL_GAMETITLE,	1024,	576,	5,		540	},
	{ ID_LABEL_GAMETITLE,	1024,	600,	5,		565	},
	{ ID_LABEL_GAMETITLE,	1280,	720,	5,		680	},
	{ ID_LABEL_GAMETITLE,	1360,	768,	5,		730	},
	{ ID_LABEL_GAMETITLE,	1366,	768,	5,		730	},
	{ ID_LABEL_GAMETITLE,	1440,	900,	5,		860	},
	{ ID_LABEL_GAMETITLE,	1600,	900,	5,		860	},
	{ ID_LABEL_GAMETITLE,	1680,	1050,	5,		1005},
	{ ID_LABEL_GAMETITLE,	1920,	1080,	40,		400},

	{ ID_PANEL_MAINMENU,	1024,	576,	5,		540	},
	{ ID_PANEL_MAINMENU,	1024,	600,	5,		565	},
	{ ID_PANEL_MAINMENU,	1280,	720,	5,		680	},
	{ ID_PANEL_MAINMENU,	1360,	768,	5,		730	},
	{ ID_PANEL_MAINMENU,	1366,	768,	5,		730	},
	{ ID_PANEL_MAINMENU,	1440,	900,	5,		860	},
	{ ID_PANEL_MAINMENU,	1600,	900,	5,		860	},
	{ ID_PANEL_MAINMENU,	1680,	1050,	5,		1005},
	{ ID_PANEL_MAINMENU,	1920,	1080,	40,		480}
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
		bResError = false;
	}

	// foo
}


extern int	g_iVisibleMouse;
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

		// Fograin92: Draw mouse cursor
		g_iVisibleMouse = 1;
		App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow) );
		
		return 1;
	}
	// Fograin92: Else, let's hide our main menu
	else
	{
		this->setVisible(false);

		// Fograin92: Hide mouse cursor
		g_iVisibleMouse = 0;
		App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none) );
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

	this->setBgColor(0, 0, 0, 50);
	this->setPaintBackgroundEnabled(true);

	//m_LocalPlayer = gEngfuncs.GetLocalPlayer();	// Get the local player's index

	// Get scheme data
	pSchemes	= gViewPort->GetSchemeManager();
	pFont		= pSchemes->getFont( pSchemes->getSchemeHandle( "MAINMENU_Big", true, true) );		// Get font for game title
	pFontText	= pSchemes->getFont( pSchemes->getSchemeHandle( "MAINMENU_Normal", true, true) );	// Get font for VGUI Text


	// Game title
	pLabelGameTitle = new Label("");
	pLabelGameTitle->setFont( pFont );
	pLabelGameTitle->setParent( this );
	pLabelGameTitle->setPaintBackgroundEnabled(false);
	pLabelGameTitle->setPos( MenuAdjustPosition(ID_LABEL_GAMETITLE, false), MenuAdjustPosition(ID_LABEL_GAMETITLE, true) );
	pLabelGameTitle->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_GAMETITLE" ) ) );
	pLabelGameTitle->setFgColor( 255, 255, 255, 0 );


	// Main menu buttons panel
	pPanelMainMenu = new Panel( MenuAdjustPosition(ID_PANEL_MAINMENU, false), MenuAdjustPosition(ID_PANEL_MAINMENU, true), 240, 300);
	pPanelMainMenu->setParent(this);
	pPanelMainMenu->setBgColor(0, 0, 0, 100);
	pPanelMainMenu->setPaintBackgroundEnabled(true);


	// Main menu -> Console
	pBtn_Console = new CommandButton( "1", 0, 0, 240, 40);
	pBtn_Console->setFont( pFontText );
	pBtn_Console->setContentAlignment(vgui::Label::a_west);
    pBtn_Console->setParent( pPanelMainMenu );
	pBtn_Console->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_CONSOLE" )) );
	pBtn_Console->setPaintBackgroundEnabled(false);
    //pBtn_Console->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> New Game button
	pBtn_NewGame = new CommandButton( "2", 0, 40, 240, 40);
	pBtn_NewGame->setFont( pFontText );
	pBtn_NewGame->setContentAlignment(vgui::Label::a_west);
    pBtn_NewGame->setParent( pPanelMainMenu );
	pBtn_NewGame->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_NEWGAME" )) );
	pBtn_NewGame->setPaintBackgroundEnabled(false);
    //pBtn_NewGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> Load game button
	pBtn_LoadGame = new CommandButton( "3", 0, 80, 240, 40);
	pBtn_LoadGame->setFont( pFontText );
	pBtn_LoadGame->setContentAlignment(vgui::Label::a_west);
    pBtn_LoadGame->setParent( pPanelMainMenu );
	pBtn_LoadGame->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_LOADGAME" )) );
	pBtn_LoadGame->setPaintBackgroundEnabled(false);
    //pBtn_LoadGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> Load game button
	pBtn_SaveGame = new CommandButton( "4", 0, 120, 240, 40);
	pBtn_SaveGame->setFont( pFontText );
	pBtn_SaveGame->setContentAlignment(vgui::Label::a_west);
    pBtn_SaveGame->setParent( pPanelMainMenu );
	pBtn_SaveGame->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_SAVEGAME" )) );
	pBtn_SaveGame->setPaintBackgroundEnabled(false);
    //pBtn_SaveGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> Options button
	pBtn_SaveGame = new CommandButton( "5", 0, 160, 240, 40);
	pBtn_SaveGame->setFont( pFontText );
	pBtn_SaveGame->setContentAlignment(vgui::Label::a_west);
    pBtn_SaveGame->setParent( pPanelMainMenu );
	pBtn_SaveGame->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_OPTIONS" )) );
	pBtn_SaveGame->setPaintBackgroundEnabled(false);
    //pBtn_SaveGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> Extras button
	pBtn_Extras = new CommandButton( "6", 0, 200, 240, 40);
	pBtn_Extras->setFont( pFontText );
	pBtn_Extras->setContentAlignment(vgui::Label::a_west);
    pBtn_Extras->setParent( pPanelMainMenu );
	pBtn_Extras->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_EXTRAS" )) );
	pBtn_Extras->setPaintBackgroundEnabled(false);
    //pBtn_SaveGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	// Main menu -> Quit button
	pBtn_Quit = new CommandButton( "7", 0, 240, 240, 40);
	pBtn_Quit->setFont( pFontText );
	pBtn_Quit->setContentAlignment(vgui::Label::a_west);
    pBtn_Quit->setParent( pPanelMainMenu );
	pBtn_Quit->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_QUIT" )) );
	pBtn_Quit->setPaintBackgroundEnabled(false);
    //pBtn_SaveGame->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

	

	UpdateMainMenu();
}


// New main menu destructor
CMainMenuNew::~CMainMenuNew()
{
	// Clean vars
	ResetVars(true);

	// Delete objects
	if(pBtn_Quit)			delete pBtn_Quit;
	if(pBtn_Extras)			delete pBtn_Extras;
	if(pBtn_Options)		delete pBtn_Options;
	if(pBtn_LoadGame)		delete pBtn_LoadGame;
	if(pBtn_NewGame)		delete pBtn_NewGame;
	if(pBtn_Console)		delete pBtn_Console;
	if(pPanelMainMenu)		delete pPanelMainMenu;
	if(pLabelGameTitle)		delete pLabelGameTitle;
	// FOO
}



// Called every frame when main menu is visable
void CMainMenuNew::paint()
{
	//gEngfuncs.Con_Printf( "^2HLU -> CMainMenuNew -> paint()\n" );
	//char cString[32];	// Helper string
	Panel::paint();		// Proceed with rendering
}


