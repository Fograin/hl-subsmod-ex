// =======================================
//  Half-Life Subtitles Mod: Utils Source
//           written by Vit_amiN
// =======================================

#include "hud.h"
#include "cl_util.h"
#include "sm_consts.h"
#include "sm_util_txt.h"
#include "sm_util_msg.h"

static client_textmessage_advprops_t advProps;
static const char * const emptyStr = "";
static char prsBuff[PARSER_MAX_TOKEN_LEN],
            snmBuff[PARSER_MAX_TOKEN_LEN],
            chnBuff[PARSER_MAX_TOKEN_LEN];

static const char * const pTextMsgSchemeNames[] = 
{
	"Default Scheme",   // The last resort
	"Subtitles Text",   // For subtitles
	"Show-Hide Text",   // For '$effect 0'
	"Credits Text",     // For '$effect 1'
	"Writeout Text",    // For '$effect 2'
};

#ifdef CLIENT_DLL

#include "vgui_TeamFortressViewport.h"

client_textmessage_t * const SM_GetClientTextMessage(const char * const pMsgName)
{
	const char * pRealMsgName = (pMsgName[0] == '#') ? pMsgName + 1 : pMsgName;
    client_textmessage_t * result = TextMessageGet(pRealMsgName);
    return (!result && gViewPort) ? gViewPort->GetSMClientMessage(pMsgName) : result;
}

#endif // CLIENT_DLL

const client_textmessage_advprops_t * const SM_GetAdvMsgProperties(const client_textmessage_t * const pMsg, const int & msgType)
{
    if (pMsg)
    {
        if (msgType == MSG_TYPE_SUBTITLE)
        {
            advProps.pSchemeName = pTextMsgSchemeNames[1];
        }
        else switch (pMsg->effect & ~(MSG_FLAG_ANY_SET))
        {
            case MSG_EFF_SHOWHIDE:
            {
                advProps.pSchemeName = pTextMsgSchemeNames[2];
                break;
            }
            case MSG_EFF_FLICKERY:
            {
                advProps.pSchemeName = pTextMsgSchemeNames[3];
                break;
            }
            case MSG_EFF_WRITEOUT:
            {
                advProps.pSchemeName = pTextMsgSchemeNames[4];
                break;
            }
            default:
            {
                advProps.pSchemeName = pTextMsgSchemeNames[0];
                break;
            }
        }
        advProps.pChnMsgName = emptyStr;

        const char * txt_ptr = pMsg->pMessage;
        bool no_matches = !txt_ptr;
        
        while (!(no_matches || SM_CurrentlyAtEOF(txt_ptr)))
        {
            switch (txt_ptr[0])
            {
                case MSG_AP_ID_SC_NAME:
                {
                    txt_ptr++;
                    const char * eol_ptr = txt_ptr,
                               * end_ptr = txt_ptr,
                               * mov_ptr = txt_ptr;
                    bool got_eol = SM_FindCurrentEOL(eol_ptr);
                    while ((mov_ptr <= eol_ptr) && !SM_CurrentlyAtEOF(end_ptr))
                    {
                        end_ptr = mov_ptr;
                        mov_ptr = gEngfuncs.COM_ParseFile(const_cast<char *>(mov_ptr), prsBuff);	// Warning: usage of const_cast!
                    }
                    if (end_ptr > txt_ptr)
                    {
                        advProps.pSchemeName = snmBuff;
                        int memcpy_len = min(end_ptr - txt_ptr, PARSER_MAX_TOKEN_LEN - 1);
                        memcpy(snmBuff, txt_ptr, memcpy_len); snmBuff[memcpy_len] = '\0';
                    }
                    if (got_eol) SM_SkipCurrentEOL(eol_ptr);
                    txt_ptr = eol_ptr;
                    no_matches = !got_eol;  // True if we've found the EOF
                    break;
                }
                case MSG_AP_ID_NXT_MSG:
                {
                    txt_ptr++;
                    const char * eol_ptr = txt_ptr,
                               * end_ptr = txt_ptr,
                               * mov_ptr = txt_ptr;
                    bool got_eol = SM_FindCurrentEOL(eol_ptr);
                    while ((mov_ptr <= eol_ptr) && !SM_CurrentlyAtEOF(end_ptr))
                    {
                        end_ptr = mov_ptr;
                        mov_ptr = gEngfuncs.COM_ParseFile(const_cast<char *>(mov_ptr), prsBuff);	// Warning: usage of const_cast!
                    }
                    if (end_ptr > txt_ptr)
                    {
                        advProps.pChnMsgName = chnBuff;
                        int memcpy_len = min(end_ptr - txt_ptr, PARSER_MAX_TOKEN_LEN - 1);
                        memcpy(chnBuff, txt_ptr, memcpy_len); chnBuff[memcpy_len] = '\0';
                    }
                    if (got_eol) SM_SkipCurrentEOL(eol_ptr);
                    txt_ptr = eol_ptr;
                    no_matches = !got_eol;  // True if we've found the EOF
                    break;
                }
                default:
                {
                    no_matches = true;
                    break;
                }
            }
        }
        advProps.pCurMsgName = pMsg->pName;
        advProps.pCharsStart = txt_ptr;
    } else {
        gEngfuncs.Con_Printf("ERROR: SM_GetAdvMsgProperties(): Incoming message is NULL!\n");

        advProps.pSchemeName = pTextMsgSchemeNames[0];
        advProps.pCurMsgName = emptyStr;    // No data
        advProps.pCharsStart = emptyStr;    // No data
        advProps.pChnMsgName = emptyStr;    // No data
    }

    return &advProps;
}
