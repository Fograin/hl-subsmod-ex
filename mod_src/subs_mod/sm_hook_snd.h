//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef _SUBS_MOD_HOOKS_SND_H
#define _SUBS_MOD_HOOKS_SND_H

#ifdef DISABLE_ENGINE_HOOKS
	#define			PM_PlaySound(                        ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		PM_PlaySound_Hooked(                  ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )
	#ifdef CLIENT_DLL
		#define		EV_PlaySound(       IENTITY, ORIGIN, ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		EV_PlaySound_Hooked( IENTITY, ORIGIN, ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )
		//#define		pfnPlaySoundByName(                         SZNAME, FVOL                         )		pfnPlaySoundByName_Hooked(                   SZNAME, FVOL                         )
		#define		pfnPlaySoundByIndex(                        ISOUND, FVOL                         )		pfnPlaySoundByIndex_Hooked(                  ISOUND, FVOL                         )
		#define		pfnPlaySoundByNameAtPitch(                  SZNAME, FVOL,                 IPITCH )		pfnPlaySoundByNameAtPitch_Hooked(            SZNAME, FVOL,                 IPITCH )
		//#define		pfnPlaySoundByNameAtLocation(               SZNAME, FVOL, ORIGIN                 )		pfnPlaySoundByNameAtLocation_Hooked(         SZNAME, FVOL, ORIGIN                 )
		#define		pfnPlaySoundVoiceByName(                    SZNAME, FVOL,                 IPITCH )		pfnPlaySoundVoiceByName_Hooked(              SZNAME, FVOL,                 IPITCH )
	#else
		#define		EMIT_S_SUB_DYN2(    SENTITY,         ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		((void)0)	// No operation
		#define		EMIT_AMBIENT_S_SUB( SENTITY, ORIGIN,        SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		((void)0)	// No operation
	#endif // CLIENT_DLL
#else
	#include "sm_util_snd.h"
	// WARNING: It's the VERY hacky way to hook EmitSound/PlaySound related engine function calls, but it's the ONLY possible way to hook them at once. Note the original HL C/C++ code isn't going to be changed because function pointer names stay the same.
	#define			PM_PlaySound(                        ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		PM_PlaySound_Hooked(                  ICHAN, SZNAME, SM_Hook_Shared_PM_PlaySound(                 SZNAME,         ICHAN, FVOL, FATTEN, IFLAGS, IPITCH ), FATTEN, IFLAGS, IPITCH )
	#ifdef CLIENT_DLL
		#define		EV_PlaySound(       IENTITY, ORIGIN, ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		EV_PlaySound_Hooked( IENTITY, ORIGIN, ICHAN, SZNAME, SM_Hook_Client_EV_PlaySound(        IENTITY, SZNAME, ORIGIN, ICHAN, FVOL, FATTEN, IFLAGS, IPITCH ), FATTEN, IFLAGS, IPITCH )
		//#define		pfnPlaySoundByName(                         SZNAME, FVOL                         )		pfnPlaySoundByName_Hooked(                   SZNAME, SM_Hook_Client_pfnPlaySoundByName(           SZNAME,                FVOL                         )                         )
		#define		pfnPlaySoundByIndex(                        ISOUND, FVOL                         )		pfnPlaySoundByIndex_Hooked(                  ISOUND, SM_Hook_Client_pfnPlaySoundByIndex(          ISOUND,                FVOL                         )                         )
		#define		pfnPlaySoundByNameAtPitch(                  SZNAME, FVOL,                 IPITCH )		pfnPlaySoundByNameAtPitch_Hooked(            SZNAME, SM_Hook_Client_pfnPlaySoundByNameAtPitch(    SZNAME,                FVOL,                 IPITCH ),                 IPITCH )
		//#define		pfnPlaySoundByNameAtLocation(               SZNAME, FVOL, ORIGIN                 )		pfnPlaySoundByNameAtLocation_Hooked(         SZNAME, SM_Hook_Client_pfnPlaySoundByNameAtLocation( SZNAME, ORIGIN,        FVOL                         ), ORIGIN                 )
		#define		pfnPlaySoundVoiceByName(                    SZNAME, FVOL,                 IPITCH )		pfnPlaySoundVoiceByName_Hooked(              SZNAME, SM_Hook_Client_pfnPlaySoundByNameAtPitch(    SZNAME,                FVOL,                 IPITCH ),                 IPITCH )
	#else
		// WARNING: To be sure you have working server-side hooks avoid calling EMIT_SOUND_DYN2/EMIT_AMBIENT_SOUND engine callbacks directly in your code. Use EMIT_SOUND_DYN/UTIL_EmitAmbientSound functions from the dlls/sound.h file instead, respectively.
		#define		EMIT_S_SUB_DYN2(    SENTITY,         ICHAN, SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		                                                     SM_Hook_Server_EMIT_SOUND_DYN2(     SENTITY, SZNAME,         ICHAN, FVOL, FATTEN, IFLAGS, IPITCH )
		#define		EMIT_AMBIENT_S_SUB( SENTITY, ORIGIN,        SZNAME, FVOL, FATTEN, IFLAGS, IPITCH )		                                                     SM_Hook_Server_EMIT_AMBIENT_SOUND(  SENTITY, SZNAME, ORIGIN,        FVOL, FATTEN, IFLAGS, IPITCH )
	#endif  // CLIENT_DLL
#endif // DISABLE_ENGINE_HOOKS

#endif // _SUBS_MOD_HOOKS_SND_H
