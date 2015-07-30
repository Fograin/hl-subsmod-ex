// ===================================================
//  Half-Life Subtitles Mod: Sound Props Utils Header
//                 written by Vit_amiN
// ===================================================

#ifndef _SUBS_MOD_UTILS_SP_H
#define _SUBS_MOD_UTILS_SP_H

#include "sm_structs.h"

#ifdef CLIENT_DLL
    const char * const SM_READ_SUBTITLE(const char * const, const void * &);
#else
    const char * const SM_SUBTITLE_TO_STR(const char * const, const client_textmessage_sndprops_t * const);
#endif // CLIENT_DLL

#endif // _SUBS_MOD_UTILS_SP_H
