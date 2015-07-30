// =====================================
//  Half-Life Subtitles Mod: Text Hooks
//          written by Vit_amiN
// =====================================

#ifndef _SUBS_MOD_HOOKS_TXT_H
#define _SUBS_MOD_HOOKS_TXT_H

#ifdef DISABLE_ENGINE_HOOKS
	#define		SUBST_EOFS_IN_MEMORY(STR)	STR
	#define		SUBST_EOFS_IN_BUFFER(STR)	STR
#else
    #include "sm_util_txt.h"
	#define		SUBST_EOFS_IN_MEMORY(STR)	SM_SubstEOFsInMemory(STR)
	#define		SUBST_EOFS_IN_BUFFER(STR)	SM_SubstEOFsInBuffer(STR)
#endif // DISABLE_ENGINE_HOOKS

#endif // _SUBS_MOD_HOOKS_TXT_H
