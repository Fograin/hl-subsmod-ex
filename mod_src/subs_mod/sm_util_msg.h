// =======================================
//  Half-Life Subtitles Mod: Utils Header
//           written by Vit_amiN
// =======================================

#ifndef _SUBS_MOD_UTIL_MSG_H
#define _SUBS_MOD_UTIL_MSG_H

#include "sm_structs.h"

#ifdef CLIENT_DLL
	extern client_textmessage_t * const SM_GetClientTextMessage(const char * const);
#endif // CLIENT_DLL

//const client_textmessage_advprops_t * const SM_GetAdvMsgProperties(const client_textmessage_t * const, const int &);
const int client_textmessage_advprops_t * const SM_GetAdvMsgProperties(const client_textmessage_t * const, const int &);

#endif // _SUBS_MOD_UTIL_MSG_H
