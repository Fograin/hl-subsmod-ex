/*
menu_btnsbmp_table.h - btns_main layout
Copyright (C) 2011 CrazyRussian

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef MENU_BTNSBMP_TABLE_H
#define MENU_BTNSBMP_TABLE_H
	
enum
{
	PC_NEW_GAME = 0,
	PC_RESUME_GAME,
	PC_HAZARD_COURSE,
	PC_CONFIG,
	PC_LOAD_GAME,
	PC_SAVE_LOAD_GAME,
	PC_QUIT,
	PC_EASY,
	PC_MEDIUM,
	PC_DIFFICULT,
	PC_SAVE_GAME,
	PC_LOAD_GAME2,
	PC_CANCEL,
	PC_GAME_OPTIONS,
	PC_VIDEO,
	PC_AUDIO,
	PC_CONTROLS,
	PC_DONE,
	PC_USE_DEFAULTS,
	PC_OK,
	PC_VID_OPT,
	PC_VID_MODES,
	PC_ADV_CONTROLS,
	PC_DELETE,
	PC_CONSOLE,
	PC_ADV_OPT,
	PC_CUSTOM_GAME,
	PC_ACTIVATE,
	PC_REFRESH_LIST,
	PC_DEACTIVATE,
	PC_BUTTONCOUNT		// must be last
};

#define BUTTON_NOFOCUS	0 
#define BUTTON_FOCUS	1
#define BUTTON_PRESSED	2

extern const char *MenuButtons[PC_BUTTONCOUNT];

inline int PicButtonWidth( int pic_id )
{
	if( pic_id < 0 || pic_id > PC_BUTTONCOUNT )
		return 0;
	
	return strlen( MenuButtons[pic_id] );
}

#endif//MENU_BTNSBMP_TABLE_H