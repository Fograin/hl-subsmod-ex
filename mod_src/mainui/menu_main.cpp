//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	Id Software, Inc. ("Id Technology")
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "extdll.h"
#include "basemenu.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include "menu_strings.h"


#define ID_BACKGROUND		0

// Main menu buttons
#define ID_BTN_CONSOLE			1
#define ID_BTN_RESUME			2
#define ID_BTN_NEWGAME			3
#define ID_BTN_OPTIONS			4
#define ID_BTN_SAVERESTORE		5	
#define ID_BTN_LANGUAGE			6	// Fograin92: Change language button
#define ID_BTN_CREDITS			7
#define ID_BTN_EXTRAS			8
#define ID_BTN_QUIT				9

// New Game dialog
#define ART_NG_HEADER			"gfx/shell/menu_newgame_header"
#define ART_NG_HL				"gfx/shell/menu_newgame_hl"
#define ID_ART_NG_HEADER		201
#define ID_ART_NG_HL			202

#define ID_NEWGAME_WINDOW			20
#define ID_NEWGAME_BTN_HLHAZARD		21	// HL - Hazard Course
#define ID_NEWGAME_BTN_HLSTORY		22	// HL - New Game
#define ID_NEWGAME_BTN_HLUPLINK		23	// HL - Uplink
#define ID_NEWGAME_BTN_BSHAZARD		24	// BS - Hazard Course
#define ID_NEWGAME_BTN_BSSTORY		25	// BS - New Game
#define ID_NEWGAME_BTN_OFBOOT		26	// OF - Boot Camp
#define ID_NEWGAME_BTN_OFSTORY		27	// OF - New Game
#define ID_NEWGAME_BTN_NO			28

// Quit Game dialog
#define ID_QUIT_WINDOW	 	30
#define ID_QUIT_TEXT		31
#define ID_QUIT_BTN_YES	 	32
#define ID_QUIT_BTN_NO		33




// Main menu structure
typedef struct
{
	menuFramework_s	menu;
	menuBitmap_s	background;

	// Main menu buttons
	menuPicButton_s	console;
	menuPicButton_s	resumeGame;
	menuPicButton_s	newGame;
	menuPicButton_s	configuration;
	menuPicButton_s	saveRestore;
	menuPicButton_s	customGame;
	menuPicButton_s	credits;
	menuPicButton_s	extras;
	menuPicButton_s	quit;

	// New Game dialog
	menuBitmap_s	newgameArtHeader;
	menuBitmap_s	newgameArtHL;
	menuAction_s	newgameDialog;
	menuPicButton_s	newgameBtnHLHazard;		// HL - Hazard Course
	menuPicButton_s	newgameBtnHLStory;		// HL - New Game
	menuPicButton_s	newgameBtnHLUplink;		// HL - Uplink
	menuPicButton_s	newgameBtnBSHazard;		// BS - Hazard Course
	menuPicButton_s	newgameBtnBSStory;		// BS - New Game
	menuPicButton_s	newgameBtnOFBoot;		// OF - Boot Camp
	menuPicButton_s	newgameBtnOFStory;		// OF - New Game
	menuPicButton_s	newgameBtnNo;			// Cancel

	// Quit Game dialog
	menuAction_s	quitDialog;
	menuAction_s	quitMessage;
	menuPicButton_s	quitBtnYes;
	menuPicButton_s	quitBtnNo;

} uiMain_t;
static uiMain_t		uiMain;


//============================
// Function to over-draw menu
//============================
static void UI_Background_Ownerdraw( void *self )
{
	// Map has .bsp background
	if( CVAR_GET_FLOAT( "cl_background" ))
		return;

	// Draw 2D image background
	menuCommon_s	*item = (menuCommon_s *)self;
	UI_DrawBackground_Callback( self );
}


//==================================
// Enable/Disable main menu buttons
//==================================
static void UI_MainMenu_ToggleButtons( int iMenuID )
{
	// Enable/Disable menu buttons
	uiMain.console.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.resumeGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.newGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.configuration.generic.flags	^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.saveRestore.generic.flags	^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.customGame.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.credits.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.extras.generic.flags			^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.quit.generic.flags			^= QMF_INACTIVE|QMF_GRAYED;


	// Show/Hide proper popup window/sub-menu
	if (iMenuID == ID_NEWGAME_WINDOW)
	{
		uiMain.newgameDialog.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameArtHeader.generic.flags	^= QMF_HIDDEN;
		uiMain.newgameArtHL.generic.flags		^= QMF_HIDDEN;

		uiMain.newgameBtnHLHazard.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnHLStory.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnHLUplink.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnBSHazard.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnBSStory.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnOFBoot.generic.flags		^= QMF_HIDDEN;
		uiMain.newgameBtnOFStory.generic.flags		^= QMF_HIDDEN;

		uiMain.newgameBtnNo.generic.flags		^= QMF_HIDDEN;
		
	}
	else if (iMenuID == ID_QUIT_WINDOW)
	{
		uiMain.quitDialog.generic.flags		^= QMF_HIDDEN;
		uiMain.quitMessage.generic.flags	^= QMF_HIDDEN;
		uiMain.quitBtnNo.generic.flags		^= QMF_HIDDEN;
		uiMain.quitBtnYes.generic.flags		^= QMF_HIDDEN;
	}

}


//================================
// UI WINDOW -> NEW GAME DIALOG
//================================
static void UI_Window_NewGame( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// Draw border
	UI_DrawRectangleExt( item->x, item->y, item->width, item->height, 0xFFDBA500, 1 );

	// Draw center
	UI_FillRect( item->x, item->y, item->width, item->height, 0xCC000000 );

	// Draw separator [---]
	UI_FillRect( item->x+15, item->y+55, item->width-30, 1, 0xFFDBA500 );

	// Draw separator [---]
	UI_FillRect( item->x+15, item->y+515, item->width-30, 1, 0xFFDBA500 );
}

//================================
// UI WINDOW -> QUIT GAME DIALOG
//================================
static void UI_Window_Quit( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// Draw border
	UI_DrawRectangleExt( item->x, item->y, item->width, item->height, 0xFFDBA500, 1 );
	// Draw center
	UI_FillRect( item->x, item->y, item->width, item->height, 0xCC000000 );
}


//=======================
// Capture keybord input
//=======================
static const char *UI_Main_KeyFunc( int key, int down )
{
	// ESC key has been pressed
	if( down && key == K_ESCAPE )
	{
		if ( CL_IsActive())
		{
			//if( (!( uiMain.dlgMessage1.generic.flags & QMF_HIDDEN )) || (!( uiMain.quitMessage.generic.flags & QMF_HIDDEN )) )
			if( (!( uiMain.quitMessage.generic.flags & QMF_HIDDEN )) )
				UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
			else
				UI_CloseMenu();
		}
		else 
			UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		return uiSoundNull;
	}
	return UI_DefaultKey( &uiMain.menu, key, down );
}


//============================================
// Activate buttons under certain conditions
//============================================
static void UI_Main_ActivateFunc( void )
{
	// Resume game should only be avaible when player is ingame
	if ( !CL_IsActive( ))
		uiMain.resumeGame.generic.flags |= QMF_HIDDEN;


	// Fograin92: We enable developer console always
	uiMain.console.generic.y = CL_IsActive() ? 180 : 230;
	UI_ScaleCoords( NULL, &uiMain.console.generic.y, NULL, NULL );
}


//=================================
// Buttons action/event handler
//=================================
static void UI_Main_Callback( void *self, int event )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	if( event != QM_ACTIVATED )
		return;

	switch( item->id )
	{
		//============================
		// Main Menu buttons handler
		//============================
		case ID_BTN_CONSOLE:
			UI_SetActiveMenu( FALSE );
			KEY_SetDest( KEY_CONSOLE );
		break;
	
		case ID_BTN_RESUME:
			UI_CloseMenu();
		break;
	
		case ID_BTN_NEWGAME:
			// Fograin92: Load chapter selection menu
			CLIENT_COMMAND( FALSE, "map sm4_mainmenu\n" );
			//UI_MainMenu_ToggleButtons(ID_NEWGAME_WINDOW);
		break;

		case ID_BTN_OPTIONS:
			UI_Options_Menu();
		break;
	
		case ID_BTN_SAVERESTORE:
			if( CL_IsActive( ))
				UI_SaveLoad_Menu();
			else
				UI_LoadGame_Menu();
		break;
	
		case ID_BTN_LANGUAGE:
			UI_CustomGame_Menu();
		break;
	
		case ID_BTN_QUIT:
			UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;

		//=================================
		// New Game dialog buttons handler
		//=================================

		// Exec - HL: Hazard Course
		case ID_NEWGAME_BTN_HLHAZARD:
			//if( !( uiMain.newgameMessage.generic.flags & QMF_HIDDEN ))

			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			//	CVAR_SET_FLOAT( "skill", 1.0f );	// We will set skill in options menu
			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			//CLIENT_COMMAND( FALSE, "hazardcourse\n" );
			CLIENT_COMMAND( FALSE, "map t0a0\n" );	// Start Hazard Course
		break;

		// Exec - HL: New Game
		case ID_NEWGAME_BTN_HLSTORY:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map c0a0\n" );
		break;

		// Exec - HL: Uplink
		case ID_NEWGAME_BTN_HLUPLINK:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map c1a1\n" );	// TODO: Change this
		break;

		// Exec - BS: Hazard Course
		case ID_NEWGAME_BTN_BSHAZARD:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map ba_hazard1\n" );
		break;

		// Exec - BS: New Game
		case ID_NEWGAME_BTN_BSSTORY:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map ba_tram1\n" );
		break;

		// Exec - OF: Boot Camp
		case ID_NEWGAME_BTN_OFBOOT:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map ofboot0\n" );
		break;

		// Exec - OF: New Game
		case ID_NEWGAME_BTN_OFSTORY:
			// If player is already ingame, end current session
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
				HOST_ENDGAME( "end of the game" );

			CVAR_SET_FLOAT( "deathmatch", 0.0f );	// This is not deathmatch
			CVAR_SET_FLOAT( "teamplay", 0.0f );		// This is not teamplay
			CVAR_SET_FLOAT( "coop", 0.0f );			// This is not CO-OP
			CVAR_SET_FLOAT( "pausable", 1.0f );		// Enable pause
	
			BACKGROUND_TRACK( NULL, NULL );		// STFU current soundtrack

			CLIENT_COMMAND( FALSE, "map of0a0\n" );
		break;


		// Close new game dialog
		case ID_NEWGAME_BTN_NO:
				UI_MainMenu_ToggleButtons(ID_NEWGAME_WINDOW);
		break;




	
		case ID_QUIT_BTN_YES:
			if( !( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				CLIENT_COMMAND( FALSE, "quit\n" );
		break;
	
		case ID_QUIT_BTN_NO:
				UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;

		/*
		case ID_NEWGAME_BTN_YES:
			if( !( uiMain.newgameMessage.generic.flags & QMF_HIDDEN ))

				// Fograin92: temporary code
			if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
		HOST_ENDGAME( "end of the game" );

	CVAR_SET_FLOAT( "skill", 1.0f );
	CVAR_SET_FLOAT( "deathmatch", 0.0f );
	CVAR_SET_FLOAT( "teamplay", 0.0f );
	CVAR_SET_FLOAT( "pausable", 1.0f ); // singleplayer is always allowing pause
	CVAR_SET_FLOAT( "coop", 0.0f );

	BACKGROUND_TRACK( NULL, NULL );

	CLIENT_COMMAND( FALSE, "hazardcourse\n" );
		break;
		*/
	
		
	}
}

/*
=================
UI_Main_Init
=================
*/
static void UI_Main_Init( void )
{
	memset( &uiMain, 0, sizeof( uiMain_t ));

	uiMain.menu.vidInitFunc = UI_Main_Init;
	uiMain.menu.keyFunc = UI_Main_KeyFunc;
	uiMain.menu.activateFunc = UI_Main_ActivateFunc;

	// Fograin92: Set background
	uiMain.background.generic.id = ID_BACKGROUND;
	uiMain.background.generic.type = QMTYPE_BITMAP;
	uiMain.background.generic.flags = QMF_INACTIVE;
	uiMain.background.generic.x = 0;
	uiMain.background.generic.y = 0;
	uiMain.background.generic.width = 1024;
	uiMain.background.generic.height = 768;
	uiMain.background.pic = ART_BACKGROUND;
	uiMain.background.generic.ownerdraw = UI_Background_Ownerdraw;

	// Fograin92: Console button
	uiMain.console.generic.id = ID_BTN_CONSOLE;
	uiMain.console.generic.type = QMTYPE_BM_BUTTON;
	uiMain.console.generic.name = "Console";
	uiMain.console.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	uiMain.console.generic.x = 72;
	uiMain.console.generic.y = 180;
	uiMain.console.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.console, PC_CONSOLE );

	// Fograin92: Resume game button
	uiMain.resumeGame.generic.id = ID_BTN_RESUME;
	uiMain.resumeGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.resumeGame.generic.name = "Resume game";
	uiMain.resumeGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.resumeGame.generic.x = 72;
	uiMain.resumeGame.generic.y = 230;
	uiMain.resumeGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.resumeGame, PC_RESUME_GAME );

	// Fograin92: New game button
	uiMain.newGame.generic.id = ID_BTN_NEWGAME;
	uiMain.newGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newGame.generic.name = "New game";
	uiMain.newGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.newGame.generic.x = 72;
	uiMain.newGame.generic.y = 280;
	uiMain.newGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newGame, PC_NEW_GAME );

	// Fograin92: SAVE or LOAD game buttons
	uiMain.saveRestore.generic.id = ID_BTN_SAVERESTORE;
	uiMain.saveRestore.generic.type = QMTYPE_BM_BUTTON;
	uiMain.saveRestore.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	if( CL_IsActive( ))
	{
		uiMain.saveRestore.generic.name = "Save\\Load Game";
		UI_UtilSetupPicButton(&uiMain.saveRestore,PC_SAVE_LOAD_GAME);
	}
	else
	{
		uiMain.saveRestore.generic.name = "Load Game";
		uiMain.resumeGame.generic.flags |= QMF_HIDDEN;
		UI_UtilSetupPicButton( &uiMain.saveRestore, PC_LOAD_GAME );
	}
	uiMain.saveRestore.generic.x = 72;
	uiMain.saveRestore.generic.y = 330;
	uiMain.saveRestore.generic.callback = UI_Main_Callback;

	// Fograin92: Options button
	uiMain.configuration.generic.id = ID_BTN_OPTIONS;
	uiMain.configuration.generic.type = QMTYPE_BM_BUTTON;
	uiMain.configuration.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.configuration.generic.name = "Configuration";
	uiMain.configuration.generic.x = 72;
	uiMain.configuration.generic.y = 380;
	uiMain.configuration.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.configuration, PC_CONFIG );

	// Fograin92: Change language button
	uiMain.customGame.generic.id = ID_BTN_LANGUAGE;
	uiMain.customGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.customGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.customGame.generic.name = "Custom Game";
	uiMain.customGame.generic.x = 72;
	uiMain.customGame.generic.y = 430;
	uiMain.customGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.customGame, PC_CUSTOM_GAME );

	// Fograin92: Extras button
	uiMain.extras.generic.id = ID_BTN_EXTRAS;
	uiMain.extras.generic.type = QMTYPE_BM_BUTTON;
	uiMain.extras.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.extras.generic.name = "Extras";
	uiMain.extras.generic.x = 72;
	uiMain.extras.generic.y = 480;
	uiMain.extras.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.extras, PC_EXTRAS );

	// Fograin92: Credits button
	uiMain.credits.generic.id = ID_BTN_QUIT;
	uiMain.credits.generic.type = QMTYPE_BM_BUTTON;
	uiMain.credits.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.credits.generic.name = "Credits";
	uiMain.credits.generic.x = 72;
	uiMain.credits.generic.y = 530;
	uiMain.credits.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.credits, PC_CREDITS );

	// Fograin92: Quit game button
	uiMain.quit.generic.id = ID_BTN_QUIT;
	uiMain.quit.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quit.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.quit.generic.name = "Quit";
	uiMain.quit.generic.x = 72;
	uiMain.quit.generic.y = 580;
	uiMain.quit.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quit, PC_QUIT );


	//=================
	// New Game dialog
	//=================

	// Dialog window
	uiMain.newgameDialog.generic.id = ID_NEWGAME_WINDOW;
	uiMain.newgameDialog.generic.type = QMTYPE_ACTION;
	uiMain.newgameDialog.generic.flags = QMF_INACTIVE|QMF_HIDDEN;
	uiMain.newgameDialog.generic.ownerdraw = UI_Window_NewGame;
	uiMain.newgameDialog.generic.x = 192;
	uiMain.newgameDialog.generic.y = 220;
	uiMain.newgameDialog.generic.width = 515;
	uiMain.newgameDialog.generic.height = 400;

	// Header text
	uiMain.newgameArtHeader.generic.id		= ID_ART_NG_HEADER;
	uiMain.newgameArtHeader.generic.type	= QMTYPE_BITMAP;
	uiMain.newgameArtHeader.generic.flags	= QMF_INACTIVE|QMF_HIDDEN|QMF_DRAW_ADDITIVE;
	uiMain.newgameArtHeader.generic.x		= 200;
	uiMain.newgameArtHeader.generic.y		= 220;
	uiMain.newgameArtHeader.generic.width	= 400;
	uiMain.newgameArtHeader.generic.height	= 40;
	uiMain.newgameArtHeader.pic				= ART_NG_HEADER;

	// Game logos
	uiMain.newgameArtHL.generic.id		= ID_ART_NG_HL;
	uiMain.newgameArtHL.generic.type	= QMTYPE_BITMAP;
	uiMain.newgameArtHL.generic.flags	= QMF_INACTIVE|QMF_HIDDEN|QMF_DRAW_ADDITIVE;
	uiMain.newgameArtHL.generic.x		= 200;
	uiMain.newgameArtHL.generic.y		= 260;
	uiMain.newgameArtHL.generic.width	= 500;
	uiMain.newgameArtHL.generic.height	= 220;
	uiMain.newgameArtHL.pic				= ART_NG_HL;

	// HL - Hazard Course button
	uiMain.newgameBtnHLHazard.generic.id = ID_NEWGAME_BTN_HLHAZARD;
	uiMain.newgameBtnHLHazard.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnHLHazard.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnHLHazard.generic.name = "Hazard Course";
	uiMain.newgameBtnHLHazard.generic.x = 210;
	uiMain.newgameBtnHLHazard.generic.y = 470;
	uiMain.newgameBtnHLHazard.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnHLHazard, PC_HAZARD_COURSE );

	// HL - New Game button
	uiMain.newgameBtnHLStory.generic.id = ID_NEWGAME_BTN_HLSTORY;
	uiMain.newgameBtnHLStory.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnHLStory.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnHLStory.generic.name = "New Game";
	uiMain.newgameBtnHLStory.generic.x = 210;
	uiMain.newgameBtnHLStory.generic.y = 495;
	uiMain.newgameBtnHLStory.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnHLStory, PC_NEW_GAME );

	// HL - Uplink button
	uiMain.newgameBtnHLUplink.generic.id = ID_NEWGAME_BTN_HLUPLINK;
	uiMain.newgameBtnHLUplink.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnHLUplink.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnHLUplink.generic.name = "Uplink";
	uiMain.newgameBtnHLUplink.generic.x = 210;
	uiMain.newgameBtnHLUplink.generic.y = 520;
	uiMain.newgameBtnHLUplink.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnHLUplink, PC_UPLINK );

	// BS - Hazard Course button
	uiMain.newgameBtnBSHazard.generic.id = ID_NEWGAME_BTN_BSHAZARD;
	uiMain.newgameBtnBSHazard.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnBSHazard.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnBSHazard.generic.name = "Hazard Course";
	uiMain.newgameBtnBSHazard.generic.x = 375;
	uiMain.newgameBtnBSHazard.generic.y = 470;
	uiMain.newgameBtnBSHazard.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnBSHazard, PC_HAZARD_COURSE );

	// BS - Hazard Course button
	uiMain.newgameBtnBSStory.generic.id = ID_NEWGAME_BTN_BSSTORY;
	uiMain.newgameBtnBSStory.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnBSStory.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnBSStory.generic.name = "New Game";
	uiMain.newgameBtnBSStory.generic.x = 375;
	uiMain.newgameBtnBSStory.generic.y = 495;
	uiMain.newgameBtnBSStory.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnBSStory, PC_NEW_GAME );

	// OF - Boot Camp button
	uiMain.newgameBtnOFBoot.generic.id = ID_NEWGAME_BTN_OFBOOT;
	uiMain.newgameBtnOFBoot.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnOFBoot.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnOFBoot.generic.name = "Boot Camp";
	uiMain.newgameBtnOFBoot.generic.x = 540;
	uiMain.newgameBtnOFBoot.generic.y = 470;
	uiMain.newgameBtnOFBoot.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnOFBoot, PC_BOOTCAMP );

	// OF - Boot Camp button
	uiMain.newgameBtnOFStory.generic.id = ID_NEWGAME_BTN_OFSTORY;
	uiMain.newgameBtnOFStory.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnOFStory.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnOFStory.generic.name = "New Game";
	uiMain.newgameBtnOFStory.generic.x = 540;
	uiMain.newgameBtnOFStory.generic.y = 495;
	uiMain.newgameBtnOFStory.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnOFStory, PC_NEW_GAME );

	
	// Cancel button
	uiMain.newgameBtnNo.generic.id = ID_NEWGAME_BTN_NO;
	uiMain.newgameBtnNo.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newgameBtnNo.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.newgameBtnNo.generic.name = "Cancel";
	uiMain.newgameBtnNo.generic.x = 625;
	uiMain.newgameBtnNo.generic.y = 585;
	uiMain.newgameBtnNo.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newgameBtnNo, PC_CANCEL );




	// Quit Game dialog
	uiMain.quitDialog.generic.id = ID_QUIT_WINDOW;
	uiMain.quitDialog.generic.type = QMTYPE_ACTION;
	uiMain.quitDialog.generic.flags = QMF_INACTIVE|QMF_HIDDEN;
	uiMain.quitDialog.generic.ownerdraw = UI_Window_Quit; // just a fill rectangle
	uiMain.quitDialog.generic.x = 192;
	uiMain.quitDialog.generic.y = 256;
	uiMain.quitDialog.generic.width = 640;
	uiMain.quitDialog.generic.height = 256;

	uiMain.quitMessage.generic.id = ID_QUIT_WINDOW;
	uiMain.quitMessage.generic.type = QMTYPE_ACTION;
	uiMain.quitMessage.generic.flags = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	uiMain.quitMessage.generic.name = "Are you sure?";
	uiMain.quitMessage.generic.x = 192;
	uiMain.quitMessage.generic.y = 280;
	uiMain.quitMessage.generic.width = 640;
	uiMain.quitMessage.generic.height = 256;

	uiMain.quitBtnYes.generic.id = ID_QUIT_BTN_YES;
	uiMain.quitBtnYes.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quitBtnYes.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.quitBtnYes.generic.name = "Ok";
	uiMain.quitBtnYes.generic.x = 380;
	uiMain.quitBtnYes.generic.y = 460;
	uiMain.quitBtnYes.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quitBtnYes, PC_OK );

	uiMain.quitBtnNo.generic.id = ID_QUIT_BTN_NO;
	uiMain.quitBtnNo.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quitBtnNo.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.quitBtnNo.generic.name = "Cancel";
	uiMain.quitBtnNo.generic.x = 530;
	uiMain.quitBtnNo.generic.y = 460;
	uiMain.quitBtnNo.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quitBtnNo, PC_CANCEL );









	// Fograin92: Add items
	UI_AddItem( &uiMain.menu, (void *)&uiMain.background );

	UI_AddItem( &uiMain.menu, (void *)&uiMain.console );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.resumeGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.saveRestore );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.configuration );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.customGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.extras );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.credits );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quit );

	
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameDialog );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameArtHeader );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameArtHL );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnOFBoot );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnOFStory );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnBSHazard );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnBSStory );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnHLHazard );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnHLStory );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnHLUplink );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newgameBtnNo );

	
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitDialog );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitMessage );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitBtnNo );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitBtnYes );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
	PIC_Load( ART_BACKGROUND );
	PIC_Load( ART_NG_HEADER );
	PIC_Load( ART_NG_HL );
}

/*
=================
UI_Main_Menu
=================
*/
void UI_Main_Menu( void )
{
	UI_Main_Precache();
	UI_Main_Init();
	UI_PushMenu( &uiMain.menu );
}