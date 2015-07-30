// =========================================
//  Half-Life Subtitles Mod: Defines Header
//            written by Vit_amiN
// =========================================

#ifndef _SUBS_MOD_DEFINES_H
#define _SUBS_MOD_DEFINES_H

#ifndef ARRAYSIZE
	#define ARRAYSIZE(p)	(sizeof(p)/sizeof(p[0]))	// eiface.h macro
#endif

#ifdef CLIENT_DLL
    #define		Con_StdMessage( ... )	gEngfuncs.Con_Printf(  __VA_ARGS__ )
    #define		Con_DevMessage( ... )	gEngfuncs.Con_DPrintf( __VA_ARGS__ )
#else
    #define		Con_StdMessage( ... )	ALERT( at_console, __VA_ARGS__ )
    #define		Con_DevMessage( ... )	ALERT( at_console, __VA_ARGS__ )
#endif

#endif // _SUBS_MOD_DEFINES_H
