// =========================================
//  Half-Life Subtitles Mod: Structs Header
//            written by Vit_amiN
// =========================================

#ifndef _SUBS_MOD_STRUCTS_H
#define _SUBS_MOD_STRUCTS_H

#ifdef CLIENT_DLL

struct client_textmessage_bundle_t
{
    client_textmessage_t   TextMessage;
	client_textmessage_t * pSrcMessage;
    const char           * pSchemeName,
                         * pChainedMsg;
    bool                   bHasSFXFlag,
                           bMustIgnore;
    float                  fNoReptTime;
};

#endif // CLIENT_DLL

struct client_textmessage_sndprops_t
{
    int         sndEntity;
    bool        hasOrigin;
    float       sndOrigin[3];
    int         sndChannel;
    float       sndVolume;
    float       sndAtten;
    int         sndFlags;
    int         sndPitch;
};

struct client_textmessage_sentence_t
{
    const char * pFileName,
               * pSentence;
};

struct client_textmessage_sndgroup_t
{
    const char                          * pGroupName;
    int                                   iGrNameLen;
    const client_textmessage_sentence_t * pSentences;
    int                                   iGroupSize;
};

#endif // _SUBS_MOD_STRUCTS_H
