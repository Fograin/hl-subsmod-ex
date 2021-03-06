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
// ALSO, IDs for btn handlers
enum sm_mainmenu_e
{
	ID_MENU = 0,		// New main menu ID

	ID_LABEL_GAMETITLE,	// Top text

	// Main menu panel
	ID_PANEL_MAINMENU,
	//ID_BTN_CONSOLE,	// Developer console button
	ID_BTN_NEWGAME,	// Story mode
	ID_BTN_GAMESET,	// Game settings
	ID_BTN_ARCADE,	// Arcade mode
	ID_BTN_QUIT,	// Quit - Return to engine main menu

	// Story mode -> new game
	ID_PANEL_NEWGAME,
	ID_BTN_NEWGAME_CLOSE,
	ID_BTN_NEWGAME_NEXT,
	ID_BTN_NEWGAME_PREV,
	ID_BTN_NEWGAME_HL,
	ID_BTN_NEWGAME_OF,
	ID_BTN_NEWGAME_BS,
	ID_BTN_NEWGAME_CHAPTER01,
	ID_BTN_NEWGAME_CHAPTER02,
	ID_BTN_NEWGAME_CHAPTER03,

	// Game set
	ID_BTN_GAMESET_CLOSE,
	ID_BTN_GAMESET_DIFF01,
	ID_BTN_GAMESET_DIFF02,
	ID_BTN_GAMESET_DIFF03,
	ID_BTN_GAMESET_PARTICLES01,
	ID_BTN_GAMESET_PARTICLES02,
	ID_BTN_GAMESET_PARTICLES03,
	ID_BTN_GAMESET_ADV01,
	ID_BTN_GAMESET_ADV02,
	ID_BTN_GAMESET_ADV03,
	ID_BTN_GAMESET_ADV04,
	ID_BTN_GAMESET_ADV05,
	ID_BTN_GAMESET_ADV06,
	ID_BTN_GAMESET_ADV07,
	ID_BTN_GAMESET_ADV08,
	ID_BTN_GAMESET_ADV09,
	ID_BTN_GAMESET_ADV10,

	MM_AMOUNT				// Length of this enum
};


// Adjustment array
const int iMenuAdjustmentArray[][5] =
{
	//--OBJECT ID-------ResX----ResY---AdjX---AdjY
	{ ID_LABEL_GAMETITLE,	1280,	720,	5,		240},
	{ ID_LABEL_GAMETITLE,	1280,	800,	5,		240},
	{ ID_LABEL_GAMETITLE,	1360,	768,	5,		320},
	{ ID_LABEL_GAMETITLE,	1366,	768,	5,		320},
	{ ID_LABEL_GAMETITLE,	1440,	900,	25,		360},
	{ ID_LABEL_GAMETITLE,	1600,	900,	30,		360},
	{ ID_LABEL_GAMETITLE,	1680,	1050,	30,		400},
	{ ID_LABEL_GAMETITLE,	1920,	1080,	40,		400},

	{ ID_PANEL_MAINMENU,	1280,	720,	5,		280},
	{ ID_PANEL_MAINMENU,	1280,	800,	5,		280},
	{ ID_PANEL_MAINMENU,	1360,	768,	5,		380},
	{ ID_PANEL_MAINMENU,	1366,	768,	5,		380},
	{ ID_PANEL_MAINMENU,	1440,	900,	5,		440},
	{ ID_PANEL_MAINMENU,	1600,	900,	5,		440},
	{ ID_PANEL_MAINMENU,	1680,	1050,	5,		480},
	{ ID_PANEL_MAINMENU,	1920,	1080,	40,		480},

	{ ID_PANEL_NEWGAME,		1280,	720,	5,		50},
	{ ID_PANEL_NEWGAME,		1280,	800,	5,		50},
	{ ID_PANEL_NEWGAME,		1360,	768,	50,		100},
	{ ID_PANEL_NEWGAME,		1366,	768,	50,		100},
	{ ID_PANEL_NEWGAME,		1440,	900,	100,	200},
	{ ID_PANEL_NEWGAME,		1600,	900,	200,	200},
	{ ID_PANEL_NEWGAME,		1680,	1050,	200,	280},
	{ ID_PANEL_NEWGAME,		1920,	1080,	300,	300}

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
		iActiveNewGameSP = 1;
		iActiveNewGameG = 1;
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
	else
	{
		// Fograin92: Any other map is loaded, let's close the chapter selection menu
		this->bDrawMenu = false;
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


// This function handles all key input passed from TeamFortressViewport.
// It returns 0 if key wasn't handled, and it should be processed by TeamFortressViewport or Engine
int CMainMenuNew::HandleKeyboardInput(int iKey)
{
	// MAIN MENU is visible
	if( bDrawMenu )
	{

		// Handle ESC key, when main menu is visible
		if( iKey == K_ESCAPE )
		{
			// Check if we're drawing sub-menus, if YES then close them first
			if( pPanelNewGame->isVisible() )
			{
				pPanelNewGame->setVisible(false);
				pPanelMainMenuFade->setVisible(false);
				gSoundEngine.PlaySound("common/launch_dnmenu1.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);

				// Main menu is visible or was visiable moments ago, choke key input
				return K_ESCAPE;
			}
			else if( pPanelGameSet->isVisible() )
			{
				pPanelGameSet->setVisible(false);
				pPanelMainMenuFade->setVisible(false);
				gSoundEngine.PlaySound("common/launch_dnmenu1.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);

				// Main menu is visible or was visiable moments ago, choke key input
				return K_ESCAPE;
			}

			/*
			// Looks like we're only drawing new game menu, let's show engine main menu
			else
			{
				bDrawMenu = false;
				SetCursorPos( gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY() ); // Clear mouse position
			}
			*/

			
		} // END  iKey == K_ESCAPE


		// Main menu is visible or was visiable moments ago, choke key input
		//return 1;
	}

	// ELSE, main menu is not visable, check if we should draw it
	/*
	else
	{
		if( iKey == K_ESCAPE )
		{
			// ESC was pressed, draw menu
			bDrawMenu = true;
			return K_ESCAPE;
		}
	}
	*/

	// Fograin92: The passed key wasn't handled, return 0
	return 0;
}


// Change submenu, Update submenu visuals
void CMainMenuNew::Submenu_NewGame(int iGame, int iPage)
{
	// Clean visible sub menus (prevent overlay)
	pIMGmm_HL01->setVisible(false);
	pIMGmm_HL02->setVisible(false);
	pIMGmm_HL03->setVisible(false);
	pIMGmm_HL04->setVisible(false);
	pIMGmm_HL05->setVisible(false);
	pIMGmm_HL06->setVisible(false);
	pIMGmm_HL07->setVisible(false);
	pBtn_newgame_next->setVisible(false);
	pBtn_newgame_prev->setVisible(false);
	pIMGmm_BS01->setVisible(false);
	pIMGmm_BS02->setVisible(false);

	// Update control vars
	iActiveNewGameG = iGame;
	iActiveNewGameSP = iPage;


	// Limit (and fix) vars
	if( iActiveNewGameG < 1 )
		iActiveNewGameG = 1;

	if( iActiveNewGameG > 3 )
		iActiveNewGameG = 3;

	if( iActiveNewGameSP < 1 )
		iActiveNewGameSP = 1;


	// Limit HL pages
	if( iActiveNewGameG == 1 )
	{
		if( iActiveNewGameSP > 7 )
			iActiveNewGameSP = 7;
	}

	// Limit BS pages
	if( iActiveNewGameG == 3 )
	{
		if( iActiveNewGameSP > 2 )
			iActiveNewGameSP = 2;
	}



	// Show elements based on current vars
	if( iActiveNewGameG == 1 )
	{
		// HL -> Sub menu 1
		if( iActiveNewGameSP == 1 )
		{
			pIMGmm_HL01->setVisible(true);
			pBtn_newgame_next->setVisible(true);
		}
		// HL -> Sub menu 2
		else if( iActiveNewGameSP == 2 )
		{
			pIMGmm_HL02->setVisible(true);
			pBtn_newgame_next->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
		// HL -> Sub menu 3
		else if( iActiveNewGameSP == 3 )
		{
			pIMGmm_HL03->setVisible(true);
			pBtn_newgame_next->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
		// HL -> Sub menu 4
		else if( iActiveNewGameSP == 4 )
		{
			pIMGmm_HL04->setVisible(true);
			pBtn_newgame_next->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
		// HL -> Sub menu 5
		else if( iActiveNewGameSP == 5 )
		{
			pIMGmm_HL05->setVisible(true);
			pBtn_newgame_next->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
		// HL -> Sub menu 6
		else if( iActiveNewGameSP == 6 )
		{
			pIMGmm_HL06->setVisible(true);
			pBtn_newgame_next->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
		// HL -> Sub menu 7
		else if( iActiveNewGameSP == 7 )
		{
			pIMGmm_HL07->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
	}
	// Blue-Shift
	else if( iActiveNewGameG == 3 )
	{
		// BS -> Sub menu 1
		if( iActiveNewGameSP == 1 )
		{
			pIMGmm_BS01->setVisible(true);
			pBtn_newgame_next->setVisible(true);
		}
		// BS -> Sub menu 2
		else if( iActiveNewGameSP == 2 )
		{
			pIMGmm_BS02->setVisible(true);
			pBtn_newgame_prev->setVisible(true);
		}
	}

}



void CMainMenuNew::StartChapter(int iSelectedFrame)
{
	// HL 1 -> SUB MENU 01
	if( iActiveNewGameG == 1 && iActiveNewGameSP == 1 )
	{
		switch( iSelectedFrame )
		{
			// HAZARD COURSE
			case 1:
				ClientCmd("map t0a0\n");
			break;

			// BLACK MESA INBOUND
			case 2:
				ClientCmd("map c0a0\n");
			break;

			// ANOMALOUS MATERIALS
			case 3:
				ClientCmd("map c1a0\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 02
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 2 )
	{
		switch( iSelectedFrame )
		{
			// UNFORESEEN CONSEQUENCES
			case 1:
				ClientCmd("map c1a0c\n");
			break;

			// OFFICE COMPLEX
			case 2:
				ClientCmd("map c1a2\n");
			break;

			// "WE'VE GOT HOSTILES"
			case 3:
				ClientCmd("map c1a3\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 03
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 3 )
	{
		switch( iSelectedFrame )
		{
			// BLAST PIT
			case 1:
				ClientCmd("map c1a4\n");
			break;

			// POWER UP
			case 2:
				ClientCmd("map c2a1\n");
			break;

			// ON A RAIL
			case 3:
				ClientCmd("map c2a2\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 04
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 4 )
	{
		switch( iSelectedFrame )
		{
			// APPREHENSION
			case 1:
				ClientCmd("map c2a3\n");
			break;

			// RESIDUE PROCESSING
			case 2:
				ClientCmd("map c2a4\n");
			break;

			// QUESTIONABLE ETHICS
			case 3:
				ClientCmd("map c2a4d\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 05
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 5 )
	{
		switch( iSelectedFrame )
		{
			// SURFACE TENSION
			case 1:
				ClientCmd("map c2a5\n");
			break;

			// "FORGET ABOUT FREEMAN!"
			case 2:
				ClientCmd("map c3a1\n");
			break;

			// LAMBDA CORE
			case 3:
				ClientCmd("map c3a2\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 06
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 6 )
	{
		switch( iSelectedFrame )
		{
			// XEN
			case 1:
				ClientCmd("map c4a1\n");
			break;

			// GONARCH'S LAIR
			case 2:
				ClientCmd("map c4a2\n");
			break;

			// INTERLOPER
			case 3:
				ClientCmd("map c4a1a\n");
			break;
		}
	}

	// HL 1 -> SUB MENU 07
	else if( iActiveNewGameG == 1 && iActiveNewGameSP == 7 )
	{
		switch( iSelectedFrame )
		{
			// NIHILANTH
			case 1:
				ClientCmd("map c4a1f\n");
			break;

			// ENDGAME
			case 2:
				ClientCmd("map c5a1\n");
			break;

			// UPLINK
			case 3:
				ClientCmd("map XXX\n");
			break;
		}
	}


	// BS -> SUB MENU 01
	else if( iActiveNewGameG == 3 && iActiveNewGameSP == 1 )
	{
		switch( iSelectedFrame )
		{
			// HAZARD COURSE
			case 1:
				ClientCmd("map ba_hazard1\n");
			break;

			// Insecurity
			case 2:
				ClientCmd("map ba_tram1\n");
			break;

			// Duty Calls
			case 3:
				ClientCmd("map ba_canal1\n");
			break;
		}
	}

	// BS -> SUB MENU 02
	else if( iActiveNewGameG == 3 && iActiveNewGameSP == 2 )
	{
		switch( iSelectedFrame )
		{
			// Captive Freight
			case 1:
				ClientCmd("map ba_yard1\n");
			break;

			// Focal Point
			case 2:
				ClientCmd("map ba_xen1\n");
			break;

			// Power Struggle
			case 3:
				ClientCmd("map ba_power1\n");
			break;
		}
	}

}


// Handle new main menu VGUI button actions
int CMainMenuNew::HandleMainMenuInput(int iBTN)
{
	// Play sounds for different buttons
	switch( iBTN )
	{
		// Default open panel sound
		case ID_BTN_NEWGAME:
		case ID_BTN_GAMESET:
		case ID_BTN_ARCADE:
			gSoundEngine.PlaySound("common/launch_select2.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
		break;

		// Panel close sound
		case ID_BTN_NEWGAME_CLOSE:
		case ID_BTN_GAMESET_CLOSE:
			gSoundEngine.PlaySound("common/launch_dnmenu1.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
		break;

		default:
			gSoundEngine.PlaySound("common/launch_select2.wav", g_vecZero, SND_2D, 0, SM_VOLUME_HEV);
		break;
	}


	// Handle button actions
	switch( iBTN )
	{
		// Main menu buttons
		/*
		case ID_BTN_CONSOLE:
			gEngfuncs.Con_Printf( "^3-> BTN_CONSOLE\n");
			ClientCmd("toggleconsole\n");
		break;
		*/

		// Game settings
		case ID_BTN_GAMESET:
		{
			pPanelMainMenuFade->setVisible(true);
			pPanelGameSet->setVisible(true);
			//Submenu_NewGame(iActiveNewGameG, iActiveNewGameSP);
		}
		break;


		// Story mode
		case ID_BTN_NEWGAME:
		{
			pPanelMainMenuFade->setVisible(true);
			pPanelNewGame->setVisible(true);
			Submenu_NewGame(iActiveNewGameG, iActiveNewGameSP);
		}
		break;

		// Quit
		case ID_BTN_QUIT:
			ClientCmd("disconnect\n");	// Fograin92: Return to main menu
		break;



		// New game panel buttons
		case ID_BTN_NEWGAME_CLOSE:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_CLOSE\n");
			pPanelMainMenuFade->setVisible(false);
			pPanelNewGame->setVisible(false);
		break;

		case ID_BTN_NEWGAME_NEXT:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_NEXT\n");
			Submenu_NewGame(iActiveNewGameG, iActiveNewGameSP+1);
		break;

		case ID_BTN_NEWGAME_PREV:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_PREV\n");
			Submenu_NewGame(iActiveNewGameG, iActiveNewGameSP-1);
		break;

		case ID_BTN_NEWGAME_HL:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_HL\n");
			Submenu_NewGame(1, 1);

		break;

		case ID_BTN_NEWGAME_OF:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_OF\n");
			//Submenu_NewGame(2, 1);
		break;

		case ID_BTN_NEWGAME_BS:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_BS\n");
			Submenu_NewGame(3, 1);
		break;

		case ID_BTN_NEWGAME_CHAPTER01:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_CHAPTER01\n");
			StartChapter(1);
			pPanelMainMenuFade->setVisible(false);	// Close submenu
			pPanelNewGame->setVisible(false);
			HandleKeyboardInput(K_ESCAPE); // Force ESC cmd
		break;

		case ID_BTN_NEWGAME_CHAPTER02:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_CHAPTER02\n");
			StartChapter(2);
			pPanelMainMenuFade->setVisible(false);
			pPanelNewGame->setVisible(false);
			HandleKeyboardInput(K_ESCAPE); // Force ESC cmd
		break;

		case ID_BTN_NEWGAME_CHAPTER03:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_NEWGAME_CHAPTER03\n");
			StartChapter(3);
			pPanelMainMenuFade->setVisible(false);
			pPanelNewGame->setVisible(false);
			HandleKeyboardInput(K_ESCAPE); // Force ESC cmd
		break;


		// Game set panel buttons
		case ID_BTN_GAMESET_CLOSE:
			gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_GAMESET_CLOSE\n");
			pPanelMainMenuFade->setVisible(false);
			pPanelGameSet->setVisible(false);
		break;

		// Game set difficulty settings
		case ID_BTN_GAMESET_DIFF01:
			//gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_GAMESET_CLOSE\n");
			ClientCmd("skill 1\n");
		break;

		case ID_BTN_GAMESET_DIFF02:
			//gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_GAMESET_CLOSE\n");
			ClientCmd("skill 2\n");
		break;

		case ID_BTN_GAMESET_DIFF03:
			//gEngfuncs.Con_Printf( "^3HLU -> ID_BTN_GAMESET_CLOSE\n");
			ClientCmd("skill 3\n");
		break;


		// Particles settings
		case ID_BTN_GAMESET_PARTICLES01:
			ClientCmd("sm_particles 0\n");
		break;

		case ID_BTN_GAMESET_PARTICLES02:
			ClientCmd("sm_particles 1\n");
		break;

		case ID_BTN_GAMESET_PARTICLES03:
			ClientCmd("sm_particles 2\n");
		break;


		// Advanced settings
		case ID_BTN_GAMESET_ADV01:
			if( CVAR_GET_FLOAT("sm_hev_logon") == 1 )
				ClientCmd("sm_hev_logon 0\n");
			else
				ClientCmd("sm_hev_logon 1\n");
		break;

		case ID_BTN_GAMESET_ADV02:
			if( CVAR_GET_FLOAT("sm_dyn_lights") == 1 )
				ClientCmd("sm_dyn_lights 0\n");
			else
				ClientCmd("sm_dyn_lights 1\n");
		break;

		case ID_BTN_GAMESET_ADV03:
			if( CVAR_GET_FLOAT("gl_shadows") == 1 )
				ClientCmd("gl_shadows 0\n");
			else
				ClientCmd("gl_shadows 1\n");
		break;

		case ID_BTN_GAMESET_ADV04:
			if( CVAR_GET_FLOAT("sm_hev_pick") == 1 )
				ClientCmd("sm_hev_pick 0\n");
			else
				ClientCmd("sm_hev_pick 1\n");
		break;

		case ID_BTN_GAMESET_ADV05:
			if( CVAR_GET_FLOAT("sm_fx_flesh") == 1 )
				ClientCmd("sm_fx_flesh 0\n");
			else
				ClientCmd("sm_fx_flesh 1\n");
		break;

		case ID_BTN_GAMESET_ADV06:
			if( CVAR_GET_FLOAT("sm_fx_dropclip") == 1 )
				ClientCmd("sm_fx_dropclip 0\n");
			else
				ClientCmd("sm_fx_dropclip 1\n");
		break;

		case ID_BTN_GAMESET_ADV07:
			if( CVAR_GET_FLOAT("sm_longjump_snd") == 1 )
				ClientCmd("sm_longjump_snd 0\n");
			else
				ClientCmd("sm_longjump_snd 1\n");
		break;

		// Unhandled button
		case 0:
		default:
			gEngfuncs.Con_Printf( "^3-> UNHANDLED BTN\n");
		break;
	}


	// Fograin92: The passed button wasn't handled, return 0
	return 0;
}





//======= NEW MAIN MENU =======//



// New main menu constructor
CMainMenuNew::CMainMenuNew() : Panel( 0, 0, XRES(640), YRES(480) )
{
	// Set main menu variables to initial values
	ResetVars(true);
	this->setBgColor(0, 0, 0, 50);
	this->setPaintBackgroundEnabled(false);

	//m_LocalPlayer = gEngfuncs.GetLocalPlayer();	// Get the local player's index

	// Get scheme data
	pSchemes	= gViewPort->GetSchemeManager();
	pFont		= pSchemes->getFont( pSchemes->getSchemeHandle( "MAINMENU_Big", true, true) );		// Get font for game title
	pFontText	= pSchemes->getFont( pSchemes->getSchemeHandle( "MAINMENU_Normal", true, true) );	// Get font for VGUI Text


	// Top text
	pLabelGameTitle = new Label("");
	pLabelGameTitle->setFont( pFont );
	pLabelGameTitle->setParent( this );
	pLabelGameTitle->setPaintBackgroundEnabled(false);
	pLabelGameTitle->setPos( MenuAdjustPosition(ID_LABEL_GAMETITLE, false), MenuAdjustPosition(ID_LABEL_GAMETITLE, true) );
	pLabelGameTitle->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_GAMETITLE" ) ) );
	pLabelGameTitle->setFgColor( 255, 255, 255, 0 );


	// Main menu buttons panel
	pPanelMainMenu = new Panel( MenuAdjustPosition(ID_PANEL_MAINMENU, false), MenuAdjustPosition(ID_PANEL_MAINMENU, true), 300, 400);
	pPanelMainMenu->setParent(this);
	pPanelMainMenu->setBgColor(0, 0, 0, 100);
	pPanelMainMenu->setPaintBackgroundEnabled(false);


	// Main menu -> Game settings button
	pBtn_GameSet = new CommandButton( "1", 0, 0, 280, 40);
	pBtn_GameSet->setFont( pFontText );
	pBtn_GameSet->setContentAlignment(vgui::Label::a_west);
    pBtn_GameSet->setParent( pPanelMainMenu );
	pBtn_GameSet->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_GAMESET" )) );
	pBtn_GameSet->setPaintBackgroundEnabled(false);
    pBtn_GameSet->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET, this) );

	// Main menu -> Story mode button
	pBtn_NewGame = new CommandButton( "2", 0, 40, 280, 40);
	pBtn_NewGame->setFont( pFontText );
	pBtn_NewGame->setContentAlignment(vgui::Label::a_west);
    pBtn_NewGame->setParent( pPanelMainMenu );
	pBtn_NewGame->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_STORYMODE" )) );
	pBtn_NewGame->setPaintBackgroundEnabled(false);
    pBtn_NewGame->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME, this) );

	// Main menu -> Arcade mode button
	pBtn_Arcade = new CommandButton( "3", 0, 80, 280, 40);
	pBtn_Arcade->setFont( pFontText );
	pBtn_Arcade->setContentAlignment(vgui::Label::a_west);
    pBtn_Arcade->setParent( pPanelMainMenu );
	pBtn_Arcade->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_ARCADE" )) );
	pBtn_Arcade->setPaintBackgroundEnabled(false);
    pBtn_Arcade->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_ARCADE, this) );

	// Main menu -> Quit button
	pBtn_Quit = new CommandButton( "4", 0, 120, 280, 40);
	pBtn_Quit->setFont( pFontText );
	pBtn_Quit->setContentAlignment(vgui::Label::a_west);
    pBtn_Quit->setParent( pPanelMainMenu );
	pBtn_Quit->setText( SUBST_EOFS_IN_MEMORY( CHudTextMessage::BufferedLocaliseTextString( "#MAINMENU_BTN_QUIT" )) );
	pBtn_Quit->setPaintBackgroundEnabled(false);
    pBtn_Quit->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_QUIT, this) );


	// Main menu "fade-like" background
	pPanelMainMenuFade = new Panel( 0, 0, XRES(640), YRES(480) );
	pPanelMainMenuFade->setParent(this);
	pPanelMainMenuFade->setBgColor(0, 0, 0, 50);
	pPanelMainMenuFade->setPaintBackgroundEnabled(true);
	pPanelMainMenuFade->setVisible(false);


	// Main menu -> new game panel
	pPanelNewGame = new Panel( MenuAdjustPosition(ID_PANEL_NEWGAME, false), MenuAdjustPosition(ID_PANEL_NEWGAME, true), 1000, 600);
	pPanelNewGame->setParent(this);
	pPanelNewGame->setPaintBackgroundEnabled(false);
	pPanelNewGame->setVisible(false);
	
	// New game -> background HL IMG 01
	pIMGmm_HL01 = new MenuImageHolder("gfx/vgui/mm_newHL01.tga", pPanelNewGame);
	pIMGmm_HL01->setPos( 0, 0 );
	pIMGmm_HL01->setVisible(false);

	// New game -> background HL IMG 02
	pIMGmm_HL02 = new MenuImageHolder("gfx/vgui/mm_newHL02.tga", pPanelNewGame);
	pIMGmm_HL02->setPos( 0, 0 );
	pIMGmm_HL02->setVisible(false);

	// New game -> background HL IMG 03
	pIMGmm_HL03 = new MenuImageHolder("gfx/vgui/mm_newHL03.tga", pPanelNewGame);
	pIMGmm_HL03->setPos( 0, 0 );
	pIMGmm_HL03->setVisible(false);

	// New game -> background HL IMG 04
	pIMGmm_HL04 = new MenuImageHolder("gfx/vgui/mm_newHL04.tga", pPanelNewGame);
	pIMGmm_HL04->setPos( 0, 0 );
	pIMGmm_HL04->setVisible(false);

	// New game -> background HL IMG 05
	pIMGmm_HL05 = new MenuImageHolder("gfx/vgui/mm_newHL05.tga", pPanelNewGame);
	pIMGmm_HL05->setPos( 0, 0 );
	pIMGmm_HL05->setVisible(false);

	// New game -> background HL IMG 06
	pIMGmm_HL06 = new MenuImageHolder("gfx/vgui/mm_newHL06.tga", pPanelNewGame);
	pIMGmm_HL06->setPos( 0, 0 );
	pIMGmm_HL06->setVisible(false);

	// New game -> background HL IMG 07
	pIMGmm_HL07 = new MenuImageHolder("gfx/vgui/mm_newHL07.tga", pPanelNewGame);
	pIMGmm_HL07->setPos( 0, 0 );
	pIMGmm_HL07->setVisible(false);

	// New game -> background BS IMG 01
	pIMGmm_BS01 = new MenuImageHolder("gfx/vgui/mm_newBS01.tga", pPanelNewGame);
	pIMGmm_BS01->setPos( 0, 0 );
	pIMGmm_BS01->setVisible(false);

	// New game -> background BS IMG 02
	pIMGmm_BS02 = new MenuImageHolder("gfx/vgui/mm_newBS02.tga", pPanelNewGame);
	pIMGmm_BS02->setPos( 0, 0 );
	pIMGmm_BS02->setVisible(false);

	// New game -> close button
	pBtn_newgame_close = new CommandButton( "", 940, 20, 32, 32);
	pBtn_newgame_close->setFont( pFontText );
    pBtn_newgame_close->setParent( pPanelNewGame );
	pBtn_newgame_close->setPaintBackgroundEnabled(true);
    pBtn_newgame_close->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_CLOSE, this) );

	// New game -> HL1 button
	pBtn_newgame_hl = new CommandButton( "", 448, 98, 32, 32);
	pBtn_newgame_hl->setFont( pFontText );
    pBtn_newgame_hl->setParent( pPanelNewGame );
	pBtn_newgame_hl->setPaintBackgroundEnabled(true);
    pBtn_newgame_hl->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_HL, this) );

	// New game -> OF button
	pBtn_newgame_of = new CommandButton( "", 484, 98, 32, 32);
	pBtn_newgame_of->setFont( pFontText );
    pBtn_newgame_of->setParent( pPanelNewGame );
	pBtn_newgame_of->setPaintBackgroundEnabled(true);
    pBtn_newgame_of->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_OF, this) );

	// New game -> BS button
	pBtn_newgame_bs = new CommandButton( "", 520, 98, 32, 32);
	pBtn_newgame_bs->setFont( pFontText );
    pBtn_newgame_bs->setParent( pPanelNewGame );
	pBtn_newgame_bs->setPaintBackgroundEnabled(true);
    pBtn_newgame_bs->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_BS, this) );

	// New game -> Chapter 01
	pBtn_newgame_chapter01 = new CommandButton( "", 40, 198, 300, 300);
	pBtn_newgame_chapter01->setFont( pFontText );
    pBtn_newgame_chapter01->setParent( pPanelNewGame );
	pBtn_newgame_chapter01->setPaintBackgroundEnabled(true);
    pBtn_newgame_chapter01->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_CHAPTER01, this) );

	// New game -> Chapter 02
	pBtn_newgame_chapter02 = new CommandButton( "", 350, 198, 300, 300);
	pBtn_newgame_chapter02->setFont( pFontText );
    pBtn_newgame_chapter02->setParent( pPanelNewGame );
	pBtn_newgame_chapter02->setPaintBackgroundEnabled(true);
    pBtn_newgame_chapter02->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_CHAPTER02, this) );

	// New game -> Chapter 03
	pBtn_newgame_chapter03 = new CommandButton( "", 660, 198, 300, 300);
	pBtn_newgame_chapter03->setFont( pFontText );
    pBtn_newgame_chapter03->setParent( pPanelNewGame );
	pBtn_newgame_chapter03->setPaintBackgroundEnabled(true);
    pBtn_newgame_chapter03->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_CHAPTER03, this) );

	// New game -> Next
	pBtn_newgame_next = new CommandButton( "", 872, 548, 98, 28);
	pBtn_newgame_next->setFont( pFontText );
    pBtn_newgame_next->setParent( pPanelNewGame );
	pBtn_newgame_next->setPaintBackgroundEnabled(true);
    pBtn_newgame_next->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_NEXT, this) );
	pBtn_newgame_next->setVisible(false);

	// New game -> Previous
	pBtn_newgame_prev = new CommandButton( "", 30, 548, 194, 28);
	pBtn_newgame_prev->setFont( pFontText );
    pBtn_newgame_prev->setParent( pPanelNewGame );
	pBtn_newgame_prev->setPaintBackgroundEnabled(true);
    pBtn_newgame_prev->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_NEWGAME_PREV, this) );
	pBtn_newgame_prev->setVisible(false);



	// GAME SETTINGS PANEL
	// Game set panel
	pPanelGameSet = new Panel( MenuAdjustPosition(ID_PANEL_NEWGAME, false), MenuAdjustPosition(ID_PANEL_NEWGAME, true), 1000, 600);
	pPanelGameSet->setParent(this);
	pPanelGameSet->setPaintBackgroundEnabled(false);
	pPanelGameSet->setVisible(false);
	
	// Game set BG
	pIMGmm_GameSet = new MenuImageHolder("gfx/vgui/mm_gamemode.tga", pPanelGameSet);
	pIMGmm_GameSet->setPos( 0, 0 );
	pIMGmm_GameSet->setVisible(true);

	// Game set panel -> CLOSE 
	pBtn_GameSet_close = new CommandButton( "", 940, 20, 32, 32);
	pBtn_GameSet_close->setFont( pFontText );
    pBtn_GameSet_close->setParent( pIMGmm_GameSet );
	pBtn_GameSet_close->setPaintBackgroundEnabled(true);
    pBtn_GameSet_close->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_CLOSE, this) );
	pBtn_GameSet_close->setVisible(true);

	// Game set panel -> OK
	pBtn_GameSet_ok = new CommandButton( "", 909, 548, 64, 28);
	pBtn_GameSet_ok->setFont( pFontText );
    pBtn_GameSet_ok->setParent( pPanelGameSet );
	pBtn_GameSet_ok->setPaintBackgroundEnabled(true);
    pBtn_GameSet_ok->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_CLOSE, this) );
	pBtn_GameSet_ok->setVisible(true);


	// Game set difficulty buttons
	pIMGmm_GameSet_diff1 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_diff1->setPos( 33, 112 );
	pIMGmm_GameSet_diff1->setVisible(true);

	pBtn_GameSet_diff1 = new CommandButton( "", 33, 112, 24, 24);
	pBtn_GameSet_diff1->setFont( pFontText );
    pBtn_GameSet_diff1->setParent( pIMGmm_GameSet );
	pBtn_GameSet_diff1->setPaintBackgroundEnabled(true);
    pBtn_GameSet_diff1->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_DIFF01, this) );
	pBtn_GameSet_diff1->setVisible(true);

	pIMGmm_GameSet_diff2 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_diff2->setPos( 33, 141 );
	pIMGmm_GameSet_diff2->setVisible(true);

	pBtn_GameSet_diff2 = new CommandButton( "", 33, 141, 24, 24);
	pBtn_GameSet_diff2->setFont( pFontText );
    pBtn_GameSet_diff2->setParent( pIMGmm_GameSet );
	pBtn_GameSet_diff2->setPaintBackgroundEnabled(true);
    pBtn_GameSet_diff2->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_DIFF02, this) );
	pBtn_GameSet_diff2->setVisible(true);

	pIMGmm_GameSet_diff3 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_diff3->setPos( 33, 170 );
	pIMGmm_GameSet_diff3->setVisible(true);

	pBtn_GameSet_diff3 = new CommandButton( "", 33, 170, 24, 24);
	pBtn_GameSet_diff3->setFont( pFontText );
    pBtn_GameSet_diff3->setParent( pIMGmm_GameSet );
	pBtn_GameSet_diff3->setPaintBackgroundEnabled(true);
    pBtn_GameSet_diff3->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_DIFF03, this) );
	pBtn_GameSet_diff3->setVisible(true);


	// Game set subtitles buttons
	pIMGmm_GameSet_particles1 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_particles1->setPos( 653, 112 );
	pIMGmm_GameSet_particles1->setVisible(true);

	pBtn_GameSet_particles1 = new CommandButton( "", 653, 112, 24, 24);
	pBtn_GameSet_particles1->setFont( pFontText );
    pBtn_GameSet_particles1->setParent( pIMGmm_GameSet );
	pBtn_GameSet_particles1->setPaintBackgroundEnabled(true);
    pBtn_GameSet_particles1->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_PARTICLES01, this) );
	pBtn_GameSet_particles1->setVisible(true);

	pIMGmm_GameSet_particles2 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_particles2->setPos( 653, 141 );
	pIMGmm_GameSet_particles2->setVisible(true);

	pBtn_GameSet_particles2 = new CommandButton( "", 653, 141, 24, 24);
	pBtn_GameSet_particles2->setFont( pFontText );
    pBtn_GameSet_particles2->setParent( pIMGmm_GameSet );
	pBtn_GameSet_particles2->setPaintBackgroundEnabled(true);
    pBtn_GameSet_particles2->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_PARTICLES02, this) );
	pBtn_GameSet_particles2->setVisible(true);

	pIMGmm_GameSet_particles3 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_particles3->setPos( 653, 170 );
	pIMGmm_GameSet_particles3->setVisible(true);

	pBtn_GameSet_particles3 = new CommandButton( "", 653, 170, 24, 24);
	pBtn_GameSet_particles3->setFont( pFontText );
    pBtn_GameSet_particles3->setParent( pIMGmm_GameSet );
	pBtn_GameSet_particles3->setPaintBackgroundEnabled(true);
    pBtn_GameSet_particles3->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_PARTICLES03, this) );
	pBtn_GameSet_particles3->setVisible(true);


	// Game set - advanced options
	pIMGmm_GameSet_adv1 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv1->setPos( 33, 247 );
	pIMGmm_GameSet_adv1->setVisible(true);
	pBtn_GameSet_adv1 = new CommandButton( "", 33, 247, 24, 24);
	pBtn_GameSet_adv1->setFont( pFontText );
    pBtn_GameSet_adv1->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv1->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv1->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV01, this) );
	pBtn_GameSet_adv1->setVisible(true);

	pIMGmm_GameSet_adv2 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv2->setPos( 33, 276 );
	pIMGmm_GameSet_adv2->setVisible(true);
	pBtn_GameSet_adv2 = new CommandButton( "", 33, 276, 24, 24);
	pBtn_GameSet_adv2->setFont( pFontText );
    pBtn_GameSet_adv2->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv2->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv2->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV02, this) );
	pBtn_GameSet_adv2->setVisible(true);

	pIMGmm_GameSet_adv3 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv3->setPos( 33, 305 );
	pIMGmm_GameSet_adv3->setVisible(true);
	pBtn_GameSet_adv3 = new CommandButton( "", 33, 305, 24, 24);
	pBtn_GameSet_adv3->setFont( pFontText );
    pBtn_GameSet_adv3->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv3->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv3->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV03, this) );
	pBtn_GameSet_adv3->setVisible(true);

	pIMGmm_GameSet_adv4 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv4->setPos( 33, 334 );
	pIMGmm_GameSet_adv4->setVisible(true);
	pBtn_GameSet_adv4 = new CommandButton( "", 33, 334, 24, 24);
	pBtn_GameSet_adv4->setFont( pFontText );
    pBtn_GameSet_adv4->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv4->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv4->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV04, this) );
	pBtn_GameSet_adv4->setVisible(true);

	pIMGmm_GameSet_adv5 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv5->setPos( 33, 364 );
	pIMGmm_GameSet_adv5->setVisible(true);
	pBtn_GameSet_adv5 = new CommandButton( "", 33, 364, 24, 24);
	pBtn_GameSet_adv5->setFont( pFontText );
    pBtn_GameSet_adv5->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv5->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv5->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV05, this) );
	pBtn_GameSet_adv5->setVisible(true);

	pIMGmm_GameSet_adv6 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv6->setPos( 33, 394 );
	pIMGmm_GameSet_adv6->setVisible(true);
	pBtn_GameSet_adv6 = new CommandButton( "", 33, 394, 24, 24);
	pBtn_GameSet_adv6->setFont( pFontText );
    pBtn_GameSet_adv6->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv6->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv6->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV06, this) );
	pBtn_GameSet_adv6->setVisible(true);

	pIMGmm_GameSet_adv7 = new MenuImageHolder("gfx/vgui/mm_check.tga", pIMGmm_GameSet);
	pIMGmm_GameSet_adv7->setPos( 33, 424 );
	pIMGmm_GameSet_adv7->setVisible(true);
	pBtn_GameSet_adv7 = new CommandButton( "", 33, 424, 24, 24);
	pBtn_GameSet_adv7->setFont( pFontText );
    pBtn_GameSet_adv7->setParent( pIMGmm_GameSet );
	pBtn_GameSet_adv7->setPaintBackgroundEnabled(true);
    pBtn_GameSet_adv7->addActionSignal( new CMainMenuNew_BTNhandler(ID_BTN_GAMESET_ADV07, this) );
	pBtn_GameSet_adv7->setVisible(true);

	
	UpdateMainMenu();
}


// New main menu destructor
CMainMenuNew::~CMainMenuNew()
{
	// Clean vars
	ResetVars(true);

	// Delete objects
	if(pIMGmm_GameSet_adv10)		delete pIMGmm_GameSet_adv10;
	if(pIMGmm_GameSet_adv9)			delete pIMGmm_GameSet_adv9;
	if(pIMGmm_GameSet_adv8)			delete pIMGmm_GameSet_adv8;
	if(pIMGmm_GameSet_adv7)			delete pIMGmm_GameSet_adv7;
	if(pIMGmm_GameSet_adv6)			delete pIMGmm_GameSet_adv6;
	if(pIMGmm_GameSet_adv5)			delete pIMGmm_GameSet_adv5;
	if(pIMGmm_GameSet_adv4)			delete pIMGmm_GameSet_adv4;
	if(pIMGmm_GameSet_adv3)			delete pIMGmm_GameSet_adv3;
	if(pIMGmm_GameSet_adv2)			delete pIMGmm_GameSet_adv2;
	if(pIMGmm_GameSet_adv1)			delete pIMGmm_GameSet_adv1;

	if(pIMGmm_GameSet_particles3)	delete pIMGmm_GameSet_particles3;
	if(pIMGmm_GameSet_particles2)	delete pIMGmm_GameSet_particles2;
	if(pIMGmm_GameSet_particles1)	delete pIMGmm_GameSet_particles1;
	if(pBtn_GameSet_particles3)		delete pBtn_GameSet_particles3;
	if(pBtn_GameSet_particles2)		delete pBtn_GameSet_particles2;
	if(pBtn_GameSet_particles1)		delete pBtn_GameSet_particles1;

	if(pIMGmm_GameSet_diff3)	delete pIMGmm_GameSet_diff3;
	if(pIMGmm_GameSet_diff2)	delete pIMGmm_GameSet_diff2;
	if(pIMGmm_GameSet_diff1)	delete pIMGmm_GameSet_diff1;
	if(pBtn_GameSet_diff3)		delete pBtn_GameSet_diff3;
	if(pBtn_GameSet_diff2)		delete pBtn_GameSet_diff2;
	if(pBtn_GameSet_diff1)		delete pBtn_GameSet_diff1;

	if(pBtn_GameSet_ok)		delete pBtn_GameSet_ok;
	if(pBtn_GameSet_close)	delete pBtn_GameSet_close;
	if(pIMGmm_GameSet)		delete pIMGmm_GameSet;
	if(pPanelGameSet)		delete pPanelGameSet;

	if(pIMGmm_BS02)			delete pIMGmm_BS02;
	if(pIMGmm_BS01)			delete pIMGmm_BS01;

	if(pIMGmm_HL07)			delete pIMGmm_HL07;
	if(pIMGmm_HL06)			delete pIMGmm_HL06;
	if(pIMGmm_HL05)			delete pIMGmm_HL05;
	if(pIMGmm_HL04)			delete pIMGmm_HL04;
	if(pIMGmm_HL03)			delete pIMGmm_HL03;
	if(pIMGmm_HL02)			delete pIMGmm_HL02;
	if(pIMGmm_HL01)			delete pIMGmm_HL01;

	if(pBtn_newgame_prev)	delete pBtn_newgame_prev;
	if(pBtn_newgame_next)	delete pBtn_newgame_next;
	if(pBtn_newgame_bs)		delete pBtn_newgame_bs;
	if(pBtn_newgame_of)		delete pBtn_newgame_of;
	if(pBtn_newgame_hl)		delete pBtn_newgame_hl;
	if(pBtn_newgame_close)	delete pBtn_newgame_close;
	if(pPanelNewGame)		delete pPanelNewGame;

	if(pPanelMainMenuFade)	delete pPanelMainMenuFade;

	if(pBtn_Quit)			delete pBtn_Quit;
	if(pBtn_Arcade)			delete pBtn_Arcade;
	if(pBtn_NewGame)		delete pBtn_NewGame;
	if(pBtn_GameSet)		delete pBtn_GameSet;

	if(pPanelMainMenu)		delete pPanelMainMenu;
	if(pLabelGameTitle)		delete pLabelGameTitle;
}



// Called every frame when main menu is visable
void CMainMenuNew::paint()
{
	//gEngfuncs.Con_Printf( "^2HLU -> CMainMenuNew -> paint()\n" );
	//char cString[32];	// Helper string


	// Fograin92: Display correct checkbox options
	if( pPanelGameSet->isVisible() )
	{
		// Difficulty settings
		if( CVAR_GET_FLOAT("skill") == 2 )
		{
			pIMGmm_GameSet_diff1->setVisible(false);
			pIMGmm_GameSet_diff2->setVisible(true);
			pIMGmm_GameSet_diff3->setVisible(false);
		}
		else if( CVAR_GET_FLOAT("skill") == 3 )
		{
			pIMGmm_GameSet_diff1->setVisible(false);
			pIMGmm_GameSet_diff2->setVisible(false);
			pIMGmm_GameSet_diff3->setVisible(true);
		}
		else
		{
			pIMGmm_GameSet_diff1->setVisible(true);
			pIMGmm_GameSet_diff2->setVisible(false);
			pIMGmm_GameSet_diff3->setVisible(false);
		}

		// Particles settings
		if( CVAR_GET_FLOAT("sm_particles") == 1 )
		{
			pIMGmm_GameSet_particles1->setVisible(false);
			pIMGmm_GameSet_particles2->setVisible(true);
			pIMGmm_GameSet_particles3->setVisible(false);
		}
		else if( CVAR_GET_FLOAT("sm_particles") == 2 )
		{
			pIMGmm_GameSet_particles1->setVisible(false);
			pIMGmm_GameSet_particles2->setVisible(false);
			pIMGmm_GameSet_particles3->setVisible(true);
		}
		else
		{
			pIMGmm_GameSet_particles1->setVisible(true);
			pIMGmm_GameSet_particles2->setVisible(false);
			pIMGmm_GameSet_particles3->setVisible(false);
		}


		// Advanced settings
		if( CVAR_GET_FLOAT("sm_hev_logon") == 1 )
			pIMGmm_GameSet_adv1->setVisible(true);
		else
			pIMGmm_GameSet_adv1->setVisible(false);

		if( CVAR_GET_FLOAT("sm_dyn_lights") == 1 )
			pIMGmm_GameSet_adv2->setVisible(true);
		else
			pIMGmm_GameSet_adv2->setVisible(false);

		if( CVAR_GET_FLOAT("gl_shadows") == 1 )
			pIMGmm_GameSet_adv3->setVisible(true);
		else
			pIMGmm_GameSet_adv3->setVisible(false);

		if( CVAR_GET_FLOAT("sm_hev_pick") == 1 )
			pIMGmm_GameSet_adv4->setVisible(true);
		else
			pIMGmm_GameSet_adv4->setVisible(false);

		if( CVAR_GET_FLOAT("sm_fx_flesh") == 1 )
			pIMGmm_GameSet_adv5->setVisible(true);
		else
			pIMGmm_GameSet_adv5->setVisible(false);

		if( CVAR_GET_FLOAT("sm_fx_dropclip") == 1 )
			pIMGmm_GameSet_adv6->setVisible(true);
		else
			pIMGmm_GameSet_adv6->setVisible(false);

		if( CVAR_GET_FLOAT("sm_longjump_snd") == 1 )
			pIMGmm_GameSet_adv7->setVisible(true);
		else
			pIMGmm_GameSet_adv7->setVisible(false);

		
	}


	Panel::paint();		// Proceed with rendering
}


