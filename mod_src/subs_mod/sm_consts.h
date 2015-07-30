// ===========================================
//  Half-Life Subtitles Mod: Constants Header
//             written by Vit_amiN
// ===========================================

#ifndef _SUBS_MOD_CONSTANTS_H
#define _SUBS_MOD_CONSTANTS_H

enum VGUI_Alpha_Values
{
    VGUI_ALPHA_OPAQUE = 0x00,
    VGUI_ALPHA_TRANSP = 0xFF,
};

enum Text_Message_Params
{
    TXT_MSG_MAX_TEXT_LEN = 1024,
};

enum Engine_Message_Params
{
    ENG_MSG_MAX_BYTE_LEN = 512,
};

enum Message_Type_IDs
{
    MSG_TYPE_GARBLING = 0x00,
    MSG_TYPE_HUD_TEXT = 0x01,
    MSG_TYPE_SUBTITLE = 0x02,
};

enum Message_Effect_IDs
{
    MSG_EFF_SHOWHIDE = 0x00,
    MSG_EFF_FLICKERY = 0x01,
    MSG_EFF_WRITEOUT = 0x02,
};

enum Message_Flag_IDs
{
    MSG_FLAG_NOT_SET = 0x00,
    MSG_FLAG_IS_VGUI = 0x10,
};

// Epsilons for comparing msgs position
const float MSG_X_POS_EPSILON = 0.0001f,
            MSG_Y_POS_EPSILON = 0.0001f;

// Flag at the start of the caption msg
const char SND_MSG_SUBT_FLAG = '\x001';

enum Sound_Pitch_Values
{
    SND_PITCH_MIN_VALUE = 0x01,
    SND_PITCH_MAX_VALUE = 0xFF,
};

// Sound max distance and min threshold
const float SND_CLIP_DISTANCE = 1024.0f,
            SND_VOL_THRESHOLD =   0.05f;

// Chars for READ_STRING/WRITE_STRING
const char STR_SUBS_EOF_CHAR = '\x01A',
           STR_NETW_EOF_CHAR = '\x0FF';

#endif // _SUBS_MOD_CONSTANTS_H
