//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifdef CLIENT_DLL
	#include "..\cl_dll\wrect.h"
	#include "..\cl_dll\cl_dll.h"
#endif // CLIENT_DLL

#include "port.h"
#include "sm_consts.h"
#include "sm_util_sp.h"

static client_textmessage_sndprops_t g_pSubSoundProps;
static char g_szSubPropsStr[ENG_MSG_MAX_BYTE_LEN];

#ifdef CLIENT_DLL

const char * const SM_READ_SUBTITLE(const char * const start_ptr, const void * & res_props)
{
    const char * result = start_ptr;

    int sent_offs = 0;
    
    int read_num = sscanf(
        start_ptr,
        "%*c %i %u %g %g %g %i %g %g %i %i %n",
        &g_pSubSoundProps.sndEntity,
        &g_pSubSoundProps.hasOrigin,
        &g_pSubSoundProps.sndOrigin[0],
        &g_pSubSoundProps.sndOrigin[1],
        &g_pSubSoundProps.sndOrigin[2],
        &g_pSubSoundProps.sndChannel,
        &g_pSubSoundProps.sndVolume,
        &g_pSubSoundProps.sndAtten,
        &g_pSubSoundProps.sndFlags,
        &g_pSubSoundProps.sndPitch,
        &sent_offs);
    

	if (read_num == 10) // Subtitle flag ignored, and the last parameter is counter, not a real value
    {
        result += sent_offs;
        res_props = &g_pSubSoundProps;
		/*
        gEngfuncs.Con_DPrintf("CLIENT got sound \"%s\":\n" \
                              "    entity ID = %i,  has origin = %i,  position = (%.2f, %.2f, %.2f),\n" \
                              "    channel = %i,  flags = %i,  volume = %.2f,  atten = %.2f,  pitch = %i\n" \
                              "\n",
                              result,
                              g_pSubSoundProps.sndEntity,
                              g_pSubSoundProps.hasOrigin,
                              g_pSubSoundProps.sndOrigin[0],
                              g_pSubSoundProps.sndOrigin[1],
                              g_pSubSoundProps.sndOrigin[2],
                              g_pSubSoundProps.sndChannel,
                              g_pSubSoundProps.sndFlags,
                              g_pSubSoundProps.sndVolume,
                              g_pSubSoundProps.sndAtten,
                              g_pSubSoundProps.sndPitch
                             );
							 */
    } else {
        res_props = NULL;
        gEngfuncs.Con_DPrintf("ERROR: CLIENT got broken sound \"%s\" (read: %i)\n", result, read_num);
    };
    
    return result;
}

#else

const char * const SM_SUBTITLE_TO_STR(const char * const sentence, const client_textmessage_sndprops_t * const snd_props)
{
	_snprintf(g_szSubPropsStr, ENG_MSG_MAX_BYTE_LEN,
        "%c %i %u %g %g %g %i %g %g %i %i %.*s",
        SND_MSG_SUBT_FLAG,
        snd_props->sndEntity,
        snd_props->hasOrigin,
        snd_props->sndOrigin[0],
        snd_props->sndOrigin[1],
        snd_props->sndOrigin[2],
        snd_props->sndChannel,
        snd_props->sndVolume,
        snd_props->sndAtten,
        snd_props->sndFlags,
        snd_props->sndPitch,
        TXT_MSG_MAX_TEXT_LEN,
        sentence
    );
    g_szSubPropsStr[ENG_MSG_MAX_BYTE_LEN - 1] = '\0';

    return g_szSubPropsStr;
}

#endif // CLIENT_DLL
