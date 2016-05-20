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
#define ID_BTN_CREDITS			6
#define ID_BTN_QUIT				7

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
	menuPicButton_s	credits;
	menuPicButton_s	quit;

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
	uiMain.credits.generic.flags		^= QMF_INACTIVE|QMF_GRAYED;
	uiMain.quit.generic.flags			^= QMF_INACTIVE|QMF_GRAYED;


	// Show/Hide proper popup window/sub-menu
	if (iMenuID == ID_QUIT_WINDOW)
	{
		uiMain.quitDialog.generic.flags		^= QMF_HIDDEN;
		uiMain.quitMessage.generic.flags	^= QMF_HIDDEN;
		uiMain.quitBtnNo.generic.flags		^= QMF_HIDDEN;
		uiMain.quitBtnYes.generic.flags		^= QMF_HIDDEN;
	}

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
	
		case ID_BTN_QUIT:
			UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;


	
		case ID_QUIT_BTN_YES:
			if( !( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				CLIENT_COMMAND( FALSE, "quit\n" );
		break;
	
		case ID_QUIT_BTN_NO:
				UI_MainMenu_ToggleButtons(ID_QUIT_WINDOW);
		break;
		
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

	// Fograin92: Credits button
	uiMain.credits.generic.id = ID_BTN_QUIT;
	uiMain.credits.generic.type = QMTYPE_BM_BUTTON;
	uiMain.credits.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.credits.generic.name = "Credits";
	uiMain.credits.generic.x = 72;
	uiMain.credits.generic.y = 430;
	uiMain.credits.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.credits, PC_CREDITS );

	// Fograin92: Quit game button
	uiMain.quit.generic.id = ID_BTN_QUIT;
	uiMain.quit.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quit.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.quit.generic.name = "Quit";
	uiMain.quit.generic.x = 72;
	uiMain.quit.generic.y = 480;
	uiMain.quit.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quit, PC_QUIT );

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
	UI_AddItem( &uiMain.menu, (void *)&uiMain.credits );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quit );
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