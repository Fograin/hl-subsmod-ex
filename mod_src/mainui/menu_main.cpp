/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "extdll.h"
#include "basemenu.h"
#include "utils.h"
#include "keydefs.h"
#include "menu_btnsbmp_table.h"
#include "menu_strings.h"

#define ART_MINIMIZE_N	"gfx/shell/min_n"
#define ART_MINIMIZE_F	"gfx/shell/min_f"
#define ART_MINIMIZE_D	"gfx/shell/min_d"
#define ART_CLOSEBTN_N	"gfx/shell/cls_n"
#define ART_CLOSEBTN_F	"gfx/shell/cls_f"
#define ART_CLOSEBTN_D	"gfx/shell/cls_d"

#define ID_BACKGROUND		0
#define ID_CONSOLE			1
#define ID_RESUME			2
#define ID_NEWGAME			3
#define ID_HAZARDCOURSE		4
#define ID_CONFIGURATION	5
#define ID_SAVERESTORE		6	
#define ID_CUSTOMGAME		8	// Fograin92: Custom game will be used to load different translations
#define ID_QUIT				10
#define ID_QUIT_BUTTON		11
#define ID_MINIMIZE			12
#define ID_MSGBOX	 		13
#define ID_MSGTEXT	 		14
#define ID_YES	 			130
#define ID_NO	 			131

typedef struct
{
	menuFramework_s	menu;

	menuBitmap_s	background;
	menuPicButton_s	console;
	menuPicButton_s	resumeGame;
	menuPicButton_s	newGame;
	menuPicButton_s	hazardCourse;
	menuPicButton_s	configuration;
	menuPicButton_s	saveRestore;
	menuPicButton_s	customGame;
	menuPicButton_s	quit;

	// quit dialog
	menuAction_s	msgBox;
	menuAction_s	quitMessage;
	menuAction_s	dlgMessage1;
	menuPicButton_s	yes;
	menuPicButton_s	no;
} uiMain_t;

static uiMain_t		uiMain;

/*
=================
UI_MsgBox_Ownerdraw
=================
*/
static void UI_MsgBox_Ownerdraw( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;
	UI_FillRect( item->x, item->y, item->width, item->height, uiPromptBgColor );
}

/*
=================
UI_Background_Ownerdraw
=================
*/
static void UI_Background_Ownerdraw( void *self )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	// map has background
	if( CVAR_GET_FLOAT( "cl_background" ))
		return;

	UI_DrawBackground_Callback( self );

	if (uiStatic.m_fHaveSteamBackground || uiStatic.m_fDisableLogo)
		return; // no logos for steam background
}

static void UI_QuitDialog( void )
{
	// toggle main menu between active\inactive
	// show\hide quit dialog
	uiMain.console.generic.flags ^= QMF_INACTIVE; 
	uiMain.resumeGame.generic.flags ^= QMF_INACTIVE;
	uiMain.newGame.generic.flags ^= QMF_INACTIVE;
	uiMain.hazardCourse.generic.flags ^= QMF_INACTIVE;
	uiMain.saveRestore.generic.flags ^= QMF_INACTIVE;
	uiMain.configuration.generic.flags ^= QMF_INACTIVE;
	uiMain.customGame.generic.flags ^= QMF_INACTIVE;
	uiMain.quit.generic.flags ^= QMF_INACTIVE;

	uiMain.msgBox.generic.flags ^= QMF_HIDDEN;
	uiMain.quitMessage.generic.flags ^= QMF_HIDDEN;
	uiMain.no.generic.flags ^= QMF_HIDDEN;
	uiMain.yes.generic.flags ^= QMF_HIDDEN;
}

static void UI_PromptDialog( void )
{
	// toggle main menu between active\inactive
	// show\hide quit dialog
	uiMain.console.generic.flags ^= QMF_INACTIVE; 
	uiMain.resumeGame.generic.flags ^= QMF_INACTIVE;
	uiMain.newGame.generic.flags ^= QMF_INACTIVE;
	uiMain.hazardCourse.generic.flags ^= QMF_INACTIVE;
	uiMain.saveRestore.generic.flags ^= QMF_INACTIVE;
	uiMain.configuration.generic.flags ^= QMF_INACTIVE;
	uiMain.customGame.generic.flags ^= QMF_INACTIVE;
	uiMain.quit.generic.flags ^= QMF_INACTIVE;

	uiMain.msgBox.generic.flags ^= QMF_HIDDEN;
	uiMain.dlgMessage1.generic.flags ^= QMF_HIDDEN;
	uiMain.no.generic.flags ^= QMF_HIDDEN;
	uiMain.yes.generic.flags ^= QMF_HIDDEN;
}

/*
=================
UI_Main_KeyFunc
=================
*/
static const char *UI_Main_KeyFunc( int key, int down )
{
	if( down && key == K_ESCAPE )
	{
		if ( CL_IsActive( ))
		{
			if(!( uiMain.dlgMessage1.generic.flags & QMF_HIDDEN ))
				UI_PromptDialog();
			else if(!( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				UI_QuitDialog();
			else UI_CloseMenu();
		}
		else UI_QuitDialog();
		return uiSoundNull;
	}
	return UI_DefaultKey( &uiMain.menu, key, down );
}

/*
=================
UI_Main_ActivateFunc
=================
*/
static void UI_Main_ActivateFunc( void )
{
	if ( !CL_IsActive( ))
		uiMain.resumeGame.generic.flags |= QMF_HIDDEN;

	if( gpGlobals->developer )
	{
		uiMain.console.generic.y = CL_IsActive() ? 180 : 230;
		UI_ScaleCoords( NULL, &uiMain.console.generic.y, NULL, NULL );
	}
}

/*
=================
UI_Main_HazardCourse
=================
*/
static void UI_Main_HazardCourse( void )
{
	if( CVAR_GET_FLOAT( "host_serverstate" ) && CVAR_GET_FLOAT( "maxplayers" ) > 1 )
		HOST_ENDGAME( "end of the game" );

	CVAR_SET_FLOAT( "skill", 1.0f );
	CVAR_SET_FLOAT( "deathmatch", 0.0f );
	CVAR_SET_FLOAT( "teamplay", 0.0f );
	CVAR_SET_FLOAT( "pausable", 1.0f ); // singleplayer is always allowing pause
	CVAR_SET_FLOAT( "coop", 0.0f );

	BACKGROUND_TRACK( NULL, NULL );

	CLIENT_COMMAND( FALSE, "hazardcourse\n" );
}

/*
=================
UI_Main_Callback
=================
*/
static void UI_Main_Callback( void *self, int event )
{
	menuCommon_s	*item = (menuCommon_s *)self;

	switch( item->id )
	{
		case ID_QUIT_BUTTON:
			if( event == QM_PRESSED )
				((menuBitmap_s *)self)->focusPic = ART_CLOSEBTN_D;
			else
				((menuBitmap_s *)self)->focusPic = ART_CLOSEBTN_F;
		break;

		case ID_MINIMIZE:
			if( event == QM_PRESSED )
				((menuBitmap_s *)self)->focusPic = ART_MINIMIZE_D;
			else
				((menuBitmap_s *)self)->focusPic = ART_MINIMIZE_F;
		break;
	}

	if( event != QM_ACTIVATED )
		return;

	switch( item->id )
	{
		case ID_CONSOLE:
			UI_SetActiveMenu( FALSE );
			KEY_SetDest( KEY_CONSOLE );
		break;
	
		case ID_RESUME:
			UI_CloseMenu();
		break;
	
		case ID_NEWGAME:
			UI_NewGame_Menu();
		break;
	
		case ID_HAZARDCOURSE:
			if( CL_IsActive( ))
				UI_PromptDialog();
			else
				UI_Main_HazardCourse();
		break;
	
		case ID_CONFIGURATION:
			UI_Options_Menu();
		break;
	
		case ID_SAVERESTORE:
			if( CL_IsActive( ))
				UI_SaveLoad_Menu();
			else
				UI_LoadGame_Menu();
		break;
	
		case ID_CUSTOMGAME:
			UI_CustomGame_Menu();
		break;
	
		case ID_QUIT:
		case ID_QUIT_BUTTON:
			UI_QuitDialog();
		break;
	
		case ID_MINIMIZE:
			CLIENT_COMMAND( FALSE, "minimize\n" );
		break;
	
		case ID_YES:
			if( !( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				CLIENT_COMMAND( FALSE, "quit\n" );
			else
				UI_Main_HazardCourse();
		break;
	
		case ID_NO:
			if( !( uiMain.quitMessage.generic.flags & QMF_HIDDEN ))
				UI_QuitDialog();
			else
				UI_PromptDialog();
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
	uiMain.console.generic.id = ID_CONSOLE;
	uiMain.console.generic.type = QMTYPE_BM_BUTTON;
	uiMain.console.generic.name = "Console";
	uiMain.console.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW;
	uiMain.console.generic.x = 72;
	uiMain.console.generic.y = CL_IsActive() ? 180 : 230;
	uiMain.console.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.console, PC_CONSOLE );	// Fograin92: GTFO with images, we need to support translations the easy way

	// Fograin92: Resume game button
	uiMain.resumeGame.generic.id = ID_RESUME;
	uiMain.resumeGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.resumeGame.generic.name = "Resume game";
	uiMain.resumeGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.resumeGame.generic.x = 72;
	uiMain.resumeGame.generic.y = 230;
	uiMain.resumeGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.resumeGame, PC_RESUME_GAME );

	// Fograin92: New game button
	uiMain.newGame.generic.id = ID_NEWGAME;
	uiMain.newGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.newGame.generic.name = "New game";
	uiMain.newGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.newGame.generic.x = 72;
	uiMain.newGame.generic.y = 280;
	uiMain.newGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.newGame, PC_NEW_GAME );

	// Fograin92: Hazard Course / Boot Camp button
	uiMain.hazardCourse.generic.id = ID_HAZARDCOURSE;
	uiMain.hazardCourse.generic.type = QMTYPE_BM_BUTTON;
	uiMain.hazardCourse.generic.name = "Hazard course";
	uiMain.hazardCourse.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.hazardCourse.generic.x = 72;
	uiMain.hazardCourse.generic.y = 330;
	uiMain.hazardCourse.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.hazardCourse, PC_HAZARD_COURSE );

	// Fograin92: SAVE or LOAD game buttons
	uiMain.saveRestore.generic.id = ID_SAVERESTORE;
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
	uiMain.saveRestore.generic.y = 380;
	uiMain.saveRestore.generic.callback = UI_Main_Callback;

	// Fograin92: Options button
	uiMain.configuration.generic.id = ID_CONFIGURATION;
	uiMain.configuration.generic.type = QMTYPE_BM_BUTTON;
	uiMain.configuration.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.configuration.generic.name = "Configuration";
	uiMain.configuration.generic.x = 72;
	uiMain.configuration.generic.y = 430;
	uiMain.configuration.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.configuration, PC_CONFIG );

	// Fograin92: Change language button
	uiMain.customGame.generic.id = ID_CUSTOMGAME;
	uiMain.customGame.generic.type = QMTYPE_BM_BUTTON;
	uiMain.customGame.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.customGame.generic.name = "Custom Game";
	uiMain.customGame.generic.x = 72;
	uiMain.customGame.generic.y = 480;
	uiMain.customGame.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.customGame, PC_CUSTOM_GAME );

	// Fograin92: Quit game button
	uiMain.quit.generic.id = ID_QUIT;
	uiMain.quit.generic.type = QMTYPE_BM_BUTTON;
	uiMain.quit.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_NOTIFY;
	uiMain.quit.generic.name = "Quit";
	uiMain.quit.generic.x = 72;
	uiMain.quit.generic.y = 530;
	uiMain.quit.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.quit, PC_QUIT );


	uiMain.msgBox.generic.id = ID_MSGBOX;
	uiMain.msgBox.generic.type = QMTYPE_ACTION;
	uiMain.msgBox.generic.flags = QMF_INACTIVE|QMF_HIDDEN;
	uiMain.msgBox.generic.ownerdraw = UI_MsgBox_Ownerdraw; // just a fill rectangle
	uiMain.msgBox.generic.x = 192;
	uiMain.msgBox.generic.y = 256;
	uiMain.msgBox.generic.width = 640;
	uiMain.msgBox.generic.height = 256;

	uiMain.quitMessage.generic.id = ID_MSGBOX;
	uiMain.quitMessage.generic.type = QMTYPE_ACTION;
	uiMain.quitMessage.generic.flags = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	uiMain.quitMessage.generic.name = (CL_IsActive( )) ? MenuStrings[HINT_QUIT_ACTIVE] : MenuStrings[HINT_QUIT];
	uiMain.quitMessage.generic.x = 192;
	uiMain.quitMessage.generic.y = 280;
	uiMain.quitMessage.generic.width = 640;
	uiMain.quitMessage.generic.height = 256;

	uiMain.dlgMessage1.generic.id = ID_MSGTEXT;
	uiMain.dlgMessage1.generic.type = QMTYPE_ACTION;
	uiMain.dlgMessage1.generic.flags = QMF_INACTIVE|QMF_DROPSHADOW|QMF_HIDDEN|QMF_CENTER_JUSTIFY;
	uiMain.dlgMessage1.generic.name = MenuStrings[HINT_RESTART_HZ];
	uiMain.dlgMessage1.generic.x = 192;
	uiMain.dlgMessage1.generic.y = 280;
	uiMain.dlgMessage1.generic.width = 640;
	uiMain.dlgMessage1.generic.height = 256;

	uiMain.yes.generic.id = ID_YES;
	uiMain.yes.generic.type = QMTYPE_BM_BUTTON;
	uiMain.yes.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.yes.generic.name = "Ok";
	uiMain.yes.generic.x = 380;
	uiMain.yes.generic.y = 460;
	uiMain.yes.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.yes, PC_OK );

	uiMain.no.generic.id = ID_NO;
	uiMain.no.generic.type = QMTYPE_BM_BUTTON;
	uiMain.no.generic.flags = QMF_HIGHLIGHTIFFOCUS|QMF_DROPSHADOW|QMF_HIDDEN;
	uiMain.no.generic.name = "Cancel";
	uiMain.no.generic.x = 530;
	uiMain.no.generic.y = 460;
	uiMain.no.generic.callback = UI_Main_Callback;
	UI_UtilSetupPicButton( &uiMain.no, PC_CANCEL );

	// Fograin92: Add items
	UI_AddItem( &uiMain.menu, (void *)&uiMain.background );
	if ( gpGlobals->developer )
		UI_AddItem( &uiMain.menu, (void *)&uiMain.console );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.resumeGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.newGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.hazardCourse );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.saveRestore );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.configuration );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.customGame );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quit );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.msgBox );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.quitMessage );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.dlgMessage1 );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.no );
	UI_AddItem( &uiMain.menu, (void *)&uiMain.yes );
}

/*
=================
UI_Main_Precache
=================
*/
void UI_Main_Precache( void )
{
	PIC_Load( ART_BACKGROUND );
	PIC_Load( ART_MINIMIZE_N );
	PIC_Load( ART_MINIMIZE_F );
	PIC_Load( ART_MINIMIZE_D );
	PIC_Load( ART_CLOSEBTN_N );
	PIC_Load( ART_CLOSEBTN_F );
	PIC_Load( ART_CLOSEBTN_D );
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