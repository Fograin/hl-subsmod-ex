// =============================================
//  Half-Life Subtitles Mod: Sound Utils Header
//              written by Vit_amiN
// =============================================

#ifndef _SUBS_MOD_UTILS_SND_H
#define _SUBS_MOD_UTILS_SND_H

// Define DISABLE_ENGINE_HOOKS to disable hooks
// WARNING: DO NOT CALL THESE HOOKS EXPLICITLY!

#ifndef DISABLE_ENGINE_HOOKS
	#ifdef __cplusplus
	extern "C" {
	#endif
		extern float SM_Hook_Shared_PM_PlaySound				(                  const char * const,                      const int, const float, const float, const int, const int );
	#ifdef CLIENT_DLL
		extern float SM_Hook_Client_EV_PlaySound				( const int,       const char * const, const float * const, const int, const float, const float, const int, const int );
		extern float SM_Hook_Client_pfnPlaySoundByName			(                  const char * const,                                 const float                                    );
		extern float SM_Hook_Client_pfnPlaySoundByIndex			(                  const int,                                          const float                                    );
		extern float SM_Hook_Client_pfnPlaySoundByNameAtPitch	(                  const char * const,                                 const float,                         const int );
		extern float SM_Hook_Client_pfnPlaySoundByNameAtLocation(                  const char * const, const float * const,            const float                                    );
		extern float SM_Hook_Client_pfnPlaySoundVoiceByName		(                  const char * const,                                 const float,                         const int );
	#else
		extern void  SM_Hook_Server_EMIT_SOUND_DYN2				( edict_t * const, const char * const,                      const int, const float, const float, const int, const int );
		extern void  SM_Hook_Server_EMIT_AMBIENT_SOUND			( edict_t * const, const char * const, const float * const,            const float, const float, const int, const int );
	#endif	// CLIENT_DLL
	#ifdef __cplusplus
	}	// extern "C"
	#endif
#endif // DISABLE_ENGINE_HOOKS

#endif // _SUBS_MOD_UTILS_SND_H
