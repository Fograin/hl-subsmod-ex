//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include <math.h>
#include <VGUI_Color.h>
#include "..\cl_dll\hud.h"
#include "..\cl_dll\cl_util.h"
#include "..\cl_dll\vgui_TeamFortressViewport.h"
#include "sm_util_txt.h"
#include "sm_main.h"

void SM_RegisterAllConVars()
{
    VGUI_MsgsBasePanel::RegisterCVarsTree_S();
}

const client_textmessage_bundle_t * const SM_ConvMessageToBundle(const client_textmessage_t * const pMessage)
{
    static client_textmessage_bundle_t msgBundle = { 0 };   // Zero flush
	msgBundle.pSrcMessage = const_cast<client_textmessage_t *>(pMessage);
    msgBundle.TextMessage = *pMessage;  // Copy the msg over the previous
    return &msgBundle;
}

























const char * const VGUI_ClientMsgPool::VGUI_ClientMsgFile::GetClMsgFileName() const
{
    return m_pClMsgFileName;
}

const byte * const VGUI_ClientMsgPool::VGUI_ClientMsgFile::GetClMsgFileData() const
{
    return m_pClMsgFileData;
}

void VGUI_ClientMsgPool::VGUI_ClientMsgFile::SetClMsgFileName(const char * const value)
{
    m_pClMsgFileName = value;
}

void VGUI_ClientMsgPool::VGUI_ClientMsgFile::SetClMsgFileData(const byte * const value)
{
    m_pClMsgFileData = value;
}

VGUI_ClientMsgPool::VGUI_ClientMsgFile::VGUI_ClientMsgFile(const char * const filename)
{
    char * file_name = new char[strlen(filename) + 1];
    strcpy(file_name, filename);
    SetClMsgFileName(file_name);

    m_pMessagesArray.ensureCapacity(2048);  // It is not the max, it is the optimal amount

    byte * msgs_file = gEngfuncs.COM_LoadFile(file_name, 5, NULL);

    if (msgs_file)
    {
        char * curr_pos = reinterpret_cast<char *>(msgs_file);
        char * prev_eol = curr_pos;

        int line_num = 0;
        bool get_name = true;

        client_textmessage_bundle_t msg_bundle = { 0 };

        while(!SM_CurrentlyAtEOF(curr_pos))
        {
            line_num++;

            char * offs_beg = curr_pos;
            SM_SkipWSPSetFrom(offs_beg);

            char * curr_eol = offs_beg;
            SM_FindCurrentEOL(curr_eol);

            char * next_beg = curr_eol;
            SM_SkipCurrentEOL(next_beg);

            char * offs_end;
            for (offs_end = curr_eol; (offs_end > offs_beg) && SM_CurrentlyAtWSP(offs_end - 1); offs_end--);
            int text_len = offs_end - offs_beg;

            if (get_name)
            {
                if (!((text_len == 0) || ((text_len > 1) && (offs_beg[0] == '/') && (offs_beg[1] == '/'))))
                {
                    if (offs_beg[0] == '$')
                    {
                        text_len--;
                        offs_beg++;

                        static float flt_array[4];
                        static int   int_array[4];
                        const char * parse_str;

                        if (SM_CurrentlyAtToken(offs_beg, "position"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 2))
                            {
                                msg_bundle.TextMessage.x = flt_array[0];
                                msg_bundle.TextMessage.y = flt_array[1];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "effect"))
                        {
                            if (SM_ParseIntArray(offs_beg, text_len, int_array, 1))
                            {
                                msg_bundle.TextMessage.effect = int_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "fxtime"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 1))
                            {
                                msg_bundle.TextMessage.fxtime = flt_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "color2"))
                        {
                            if (SM_ParseIntArray(offs_beg, text_len, int_array, 3))
                            {
                                msg_bundle.TextMessage.r2 = int_array[0];
                                msg_bundle.TextMessage.g2 = int_array[1];
                                msg_bundle.TextMessage.b2 = int_array[2];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "color"))
                        {
                            if (SM_ParseIntArray(offs_beg, text_len, int_array, 3))
                            {
                                msg_bundle.TextMessage.r1 = int_array[0];
                                msg_bundle.TextMessage.g1 = int_array[1];
                                msg_bundle.TextMessage.b1 = int_array[2];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "fadein"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 1))
                            {
                                msg_bundle.TextMessage.fadein = flt_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "fadeout"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 1))
                            {
                                msg_bundle.TextMessage.fadeout = flt_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "holdtime"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 1))
                            {
                                msg_bundle.TextMessage.holdtime = flt_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "norepeat"))
                        {
                            if (SM_ParseFltArray(offs_beg, text_len, flt_array, 1))
                            {
                                msg_bundle.fNoReptTime = flt_array[0];
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "chained"))
                        {
                            if (SM_ParseStrParam(offs_beg, text_len, parse_str))
                            {
                                msg_bundle.pChainedMsg = parse_str;
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "ignore"))
                        {
                            if (SM_ParseIntArray(offs_beg, text_len, int_array, 1))
                            {
                                msg_bundle.bMustIgnore = (int_array[0] != 0);
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "sfx"))
                        {
                            if (SM_ParseIntArray(offs_beg, text_len, int_array, 1))
                            {
                                msg_bundle.bHasSFXFlag = (int_array[0] != 0);
                            }
                        }
                        else if (SM_CurrentlyAtToken(offs_beg, "scheme"))
                        {
                            if (SM_ParseStrParam(offs_beg, text_len, parse_str))
                            {
                                msg_bundle.pSchemeName = parse_str;
                            }
                        }
                        else
                        {
                            gEngfuncs.Con_DPrintf("WARNING: VGUI_ClientMsgFile(): Unknown token \"%s\" in \"%s\" at line %i\n", offs_beg, GetClMsgFileName(), line_num);
                        }
                    }
                    else if (offs_beg[0] == '{')
                    {
                        msg_bundle.TextMessage.pMessage = next_beg;
                        get_name = false;   // Get the message text
                    }
                    else if (offs_beg[0] == '}')
                    {
                        gEngfuncs.Con_DPrintf("ERROR: VGUI_ClientMsgFile(): Unexpected end-of-text token '}' in \"%s\" at line %i\n", GetClMsgFileName(), line_num);
                        break;
                    }
                    else
                    {
                        msg_bundle.TextMessage.pName = offs_beg;
                        offs_end[0] = '\0'; // Set the name EOF
                    }
                }
            }
            else
            {
                if (offs_beg[0] == '}')
                {
                    m_pMessagesArray.addElement(new client_textmessage_bundle_t(msg_bundle));

                    prev_eol[0] = '\0'; // Set the text EOF
                    get_name = true;    // Get the next msg

                    msg_bundle.TextMessage.pName    = NULL;
                    msg_bundle.TextMessage.pMessage = NULL;
                    msg_bundle.pSchemeName          = NULL,
                    msg_bundle.pChainedMsg          = NULL;
                    msg_bundle.bHasSFXFlag          = false;
                    msg_bundle.bMustIgnore          = false;
                    msg_bundle.fNoReptTime          = 0.0f;
                }
                else if (offs_beg[0] == '{')
                {
                    gEngfuncs.Con_DPrintf("ERROR: VGUI_ClientMsgFile(): Unexpected start-of-text token '{' in \"%s\" at line %i\n", GetClMsgFileName(), line_num);
                    break;
                }
            }
            prev_eol = curr_eol;
            curr_pos = next_beg;
        }
        gEngfuncs.Con_DPrintf("INFO: VGUI_ClientMsgFile(): Parsed %i messages from \"%s\" (%i lines)\n", m_pMessagesArray.getCount(), GetClMsgFileName(), line_num);
    }
    else
    {
        gEngfuncs.Con_DPrintf("ERROR: VGUI_ClientMsgFile(): Unable to open \"%s\" for parsing!\n", GetClMsgFileName());
    }
    SetClMsgFileData(msgs_file);
}

VGUI_ClientMsgPool::VGUI_ClientMsgFile::~VGUI_ClientMsgFile()
{
    for (int msg_index = m_pMessagesArray.getCount() - 1; msg_index >= 0; msg_index--)
    {
        delete m_pMessagesArray[msg_index];
        m_pMessagesArray.setElementAt(NULL, msg_index);
        m_pMessagesArray.removeElementAt(msg_index);
    }

    if (GetClMsgFileData())
    {
        gEngfuncs.COM_FreeFile(const_cast<byte *>(GetClMsgFileData()));
        SetClMsgFileData(NULL);
    }

    delete[] GetClMsgFileName();
    SetClMsgFileName(NULL);
}

const client_textmessage_bundle_t * const VGUI_ClientMsgPool::VGUI_ClientMsgFile::GetClientMessage(const char * const msg_name)
{
    for (int msg_index = m_pMessagesArray.getCount() - 1; msg_index >= 0; msg_index--)
    {
        const client_textmessage_bundle_t * result = m_pMessagesArray[msg_index];
        if (result && !stricmp(msg_name, result->TextMessage.pName)) return result;
    }
    return NULL;
}























char VGUI_ClientMsgPool::m_szUnknMsgText[TXT_MSG_MAX_TEXT_LEN] = { 0 };

const client_textmessage_bundle_t VGUI_ClientMsgPool::m_stUnknMessage =
{
    {
        MSG_EFF_WRITEOUT | MSG_FLAG_IS_VGUI,
        140,
        140,
        140,
        VGUI_ALPHA_OPAQUE,
        240,
        110,
        0,
        VGUI_ALPHA_OPAQUE,
        -1.0f,
        0.7f,
        0.01f,
        1.5f,
        5.0f,
        0.25f,
        "UNKNOWN_MESSAGE",
        m_szUnknMsgText
    },
	NULL,
    NULL,
    NULL,
    false,
    false,
    0.0f
};

bool VGUI_ClientMsgPool::m_bCVarNamesReg = false;

const bool & VGUI_ClientMsgPool::GetAllowUnknown() const
{
    return m_bAllowUnknown;
}

void VGUI_ClientMsgPool::SetAllowUnknown(const bool & value)
{
    m_bAllowUnknown = value;
}

VGUI_ClientMsgPool::VGUI_ClientMsgPool(const char * const * const fn_array, const int & fn_count)
{
    SetAllowUnknown(false);

    if (fn_array)
    {
        for (int fn_index = 0; fn_index < fn_count; fn_index++)
        {
            const char * curr_file = fn_array[fn_index];
            if (curr_file && !HasMsgFileLoaded(curr_file))
            {
                m_pClFilesArray.addElement(new VGUI_ClientMsgFile(curr_file));
            }
        }
    }
}

VGUI_ClientMsgPool::~VGUI_ClientMsgPool()
{
    for (int fn_index = m_pClFilesArray.getCount() - 1; fn_index >= 0; fn_index--)
    {
        delete m_pClFilesArray[fn_index];
        m_pClFilesArray.setElementAt(NULL, fn_index);
        m_pClFilesArray.removeElementAt(fn_index);
    }
}

void VGUI_ClientMsgPool::Initialize()
{
    RegisterCVarNames();
    AcquireCVarValues();
}

const bool VGUI_ClientMsgPool::HasMsgFileLoaded(const char * const file_name)
{
    for (int file_index = m_pClFilesArray.getCount() - 1; file_index >= 0; file_index--)
    {
        const VGUI_ClientMsgFile * result = m_pClFilesArray[file_index];
        if (result && !stricmp(file_name, result->GetClMsgFileName()))
        {
            return true;
        }
    }
    return false;
}

const client_textmessage_bundle_t * const VGUI_ClientMsgPool::GetClientMessage(const char * const msg_name)
{
    const char * real_name = (msg_name[0] == '#') ? msg_name + 1 : msg_name;

    for (int f_index = m_pClFilesArray.getCount() - 1; f_index >= 0; f_index--) // Search in pool'ed files
    {
        const client_textmessage_bundle_t * result = m_pClFilesArray[f_index]->GetClientMessage(real_name);
        if (result) return result;
    }

    const client_textmessage_t * text_msg = TextMessageGet(real_name);  // Not found; search in titles.txt
    if (text_msg) return SM_ConvMessageToBundle(text_msg);

    if (GetAllowUnknown())  // Message was not found neither in pool nor titles.txt; make it if allowed to
    {
        strncpy(m_szUnknMsgText, msg_name, TXT_MSG_MAX_TEXT_LEN - 1);
        m_szUnknMsgText[TXT_MSG_MAX_TEXT_LEN - 1] = '\0';
        
        return &m_stUnknMessage;    // Unknown msgs use static bundle
    }

    return NULL;
}

void VGUI_ClientMsgPool::AcquireCVarValues()
{
    SetAllowUnknown(CVAR_GET_FLOAT("cl_sm_create_na_msgs") != 0.0f);
}

void VGUI_ClientMsgPool::RegisterCVarNames_S()
{
    if (!m_bCVarNamesReg)
    {
        m_bCVarNamesReg = true;
        CVAR_CREATE("cl_sm_create_na_msgs", "0", FCVAR_ARCHIVE);
    }
}




























VGUI_MsgsBasePanel::VGUI_TextMsgsPanel * const VGUI_MsgsBasePanel::GetTextMsgsPanel()
{
    return m_pTextMsgsPanel;
}

VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel * const VGUI_MsgsBasePanel::GetSubtMsgsPanel()
{
    return m_pSubtMsgsPanel;
}

VGUI_MsgsBasePanel::VGUI_MsgsBasePanel(vgui::Panel * const parent,
                                       const char * const * const fn_array, const int & fn_count,
                                       const int & pos_x, const int & pos_y,
                                       const int & size_x, const int & size_y):
                                       VGUI_ClientMsgPool(fn_array, fn_count),
                                       Panel(pos_x, pos_y, size_x, size_y),
                                       m_pTextMsgsPanel(new VGUI_TextMsgsPanel(pos_x, pos_y, size_x, size_y)),
                                       m_pSubtMsgsPanel(new VGUI_SubtMsgsPanel(pos_x, pos_y, size_x, size_y))
{
    setVisible(true);
    setEnabled(true);
    setAutoFocusNavEnabled(false);
    setPaintEnabled(false);
    setPaintBackgroundEnabled(false);
    
    addChild(GetTextMsgsPanel());
    addChild(GetSubtMsgsPanel());
    
    // It must be the first child, otherwise it will mess up with other menus because it's always visible
    if (parent)
    {
        parent->insertChildAt(this, 0);
    }
}

VGUI_MsgsBasePanel::~VGUI_MsgsBasePanel()
{
    setParent(NULL);
    setVisible(false);
    removeChild(GetSubtMsgsPanel());
    removeChild(GetTextMsgsPanel());
    delete GetSubtMsgsPanel();
    delete GetTextMsgsPanel();
}

void VGUI_MsgsBasePanel::Initialize()
{
    //VGUI_ClientMsgPool::Initialize();
    RegisterCVarNames();
    SynchronizeBase();
    InitSubElements();
}

void VGUI_MsgsBasePanel::SynchronizeBase()
{
    AcquireCVarValues();
    setSize(ScreenWidth, ScreenHeight);
}

void VGUI_MsgsBasePanel::SynchronizeFull()
{
    SynchronizeBase();
    SyncSubElements();
}

bool VGUI_MsgsBasePanel::ShowSubElements()
{
    return (gHUD.m_pCvarDraw->value != 0.0f);
}

void VGUI_MsgsBasePanel::InitSubElements()
{
    bool show_sub = ShowSubElements();
    GetTextMsgsPanel()->Initialize(show_sub);
    GetSubtMsgsPanel()->Initialize(show_sub);
}

void VGUI_MsgsBasePanel::SyncSubElements()
{
    bool show_sub = ShowSubElements();
    float sync_time = gHUD.m_flTime;
    GetTextMsgsPanel()->SynchronizeFull(show_sub, sync_time);
    GetSubtMsgsPanel()->SynchronizeFull(show_sub, sync_time);
}

void VGUI_MsgsBasePanel::ClrTextMessages()
{
    GetTextMsgsPanel()->ClrVGUIMessages();
    GetSubtMsgsPanel()->ClrVGUIMessages();
}

void VGUI_MsgsBasePanel::AddTextMessage(const int & msgType, const client_textmessage_bundle_t * const pBundle, const void * const pSpecData, const float & fBirthTime)
{
    if (pBundle)
    {
        SynchronizeBase();
		const client_textmessage_t * text_msg = &pBundle->TextMessage;

        if ((text_msg->effect & MSG_FLAG_IS_VGUI) == MSG_FLAG_IS_VGUI)
        {
            switch (msgType)
            {
                case MSG_TYPE_HUD_TEXT:
                {
                    GetTextMsgsPanel()->AddVGUIMessage(pBundle, pSpecData, ShowSubElements(), fBirthTime);
                    break;
                }
                case MSG_TYPE_SUBTITLE:
                {
                    GetSubtMsgsPanel()->AddVGUIMessage(pBundle, pSpecData, ShowSubElements(), fBirthTime);
                    break;
                }
                case MSG_TYPE_GARBLING:
                {
                    gEngfuncs.Con_DPrintf("WARNING: AddTextMessage(): Skipped message \"%s\" is supposed to be garbled!\n", text_msg->pName);
                    break;
                }
                default:
                {
                    gEngfuncs.Con_DPrintf("ERROR: AddTextMessage(): Message \"%s\" has unknown type ID 0x%08x!\n", text_msg->pName, msgType);
                    break;
                }
            }
        } else {
            gEngfuncs.Con_DPrintf("ERROR: AddTextMessage(): Message \"%s\" has unknown effect 0x%08x!\n", text_msg->pName, text_msg->effect);
        }
    } else {
        gEngfuncs.Con_DPrintf("ERROR: AddTextMessage(): Incoming message bundle is NULL!\n");
    }
}

void VGUI_MsgsBasePanel::AcquireCVarValues()
{
    VGUI_ClientMsgPool::AcquireCVarValues();
}

void VGUI_MsgsBasePanel::paintTraverse(bool repaint)
{
    if (repaint) SynchronizeFull();
    Panel::paintTraverse(repaint);
}

void VGUI_MsgsBasePanel::RegisterCVarNames_S()
{
    VGUI_ClientMsgPool::RegisterCVarNames_S();
}

void VGUI_MsgsBasePanel::RegisterCVarsTree_S()
{
    VGUI_MsgsBasePanel::RegisterCVarNames_S();
    VGUI_TextMsgsPanel::RegisterCVarNames_S();
    VGUI_SubtMsgsPanel::RegisterCVarNames_S();
}


































bool VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::m_bCVarNamesReg = false;

const bool & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::GetGameIsRunning() const
{
    return m_bGameIsRunning;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::GetAbsLSyncTime() const
{
    return m_fAbsLSyncTime;
}

const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::GetShadowOffsetX() const
{
    return m_iShadowOffsetX;
}

const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::GetShadowOffsetY() const
{
    return m_iShadowOffsetY;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SetGameIsRunning(const bool & value)
{
    m_bGameIsRunning = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SetAbsLSyncTime(const float & value)
{
    m_fAbsLSyncTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SetShadowOffsetX(const int & value)
{
    m_iShadowOffsetX = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SetShadowOffsetY(const int & value)
{
    m_iShadowOffsetY = value;
}

VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgsPanel(const int & pos_x, const int & pos_y, const int & size_x, const int & size_y):
                                        Panel(pos_x, pos_y, size_x, size_y)
{
    setVisible(false);
    setEnabled(false);
    setAutoFocusNavEnabled(false);
    setPaintEnabled(false);
    setPaintBackgroundEnabled(false);
    SetGameIsRunning(false);
    SetAbsLSyncTime(-1.0f);
    SetShadowOffsetX(0);
    SetShadowOffsetY(0);
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SynchronizeFull(const bool & is_enabled, const float & sync_time)
{
    SynchronizeBase(is_enabled);
    SyncSubElements(sync_time);
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SynchronizeBase(const bool & is_enabled)
{
    AcquireCVarValues();
    setEnabled(is_enabled);
    setSize(ScreenWidth, ScreenHeight);
}

VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::~VGUI_TextMsgsPanel()
{
    setVisible(false);
    ClearSubElements(true);
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::Initialize(const bool & is_enabled)
{
    RegisterCVarNames();
    SynchronizeBase(is_enabled);

    if (!ShowSubElements())
    {
        ClearSubElements(false);
    }
}

bool VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::MsgAddPermitted(const client_textmessage_bundle_t * const pBundle)
{
    // The same message wants to be displayed twice
    // FIXME: use NoRepeatTime instead!
    bool result = !pBundle->bMustIgnore;
    
    for (int cur_index = getChildCount() - 1; result && cur_index >= 0; cur_index--)
    {
        VGUI_TextMsgRecord * cur_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
        if (cur_panel)
        {
            if (!cur_panel->RecordIsAZombie())
            {
                result = (strcmp(cur_panel->GetMessageText(), pBundle->TextMessage.pMessage) != 0); // Already in the list?
            }
        }
    }

    return result;
}

bool VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::ShowSubElements()
{
    return isEnabled();
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::SyncSubElements(const float & sync_time)
{
    FixupLTimeChange(sync_time);
    CheckForZombies();
    BuryDeadZombies(ZOMBIE_DELETE_DELAY);

    for (int cur_index = getChildCount() - 1; cur_index >= 0; cur_index--)
    {
        VGUI_TextMsgRecord * cur_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
        if (cur_panel)
        {
            cur_panel->SynchronizeFull(ShowSubElements(), GetAbsLSyncTime());
        }
    }
    
    for (int cur_index = m_aLivingZombies.getCount() - 1; cur_index >= 0; cur_index--)
    {
        VGUI_TextMsgRecord * cur_panel = m_aLivingZombies[cur_index];
        if (cur_panel)
        {
            cur_panel->SynchronizeFull(ShowSubElements(), GetAbsLSyncTime());
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::ClearSubElements(const bool & is_cleanup)
{
    TurnIntoZombies();
    if (is_cleanup)
    {
        BuryDeadZombies(0); // Forcing zombies to die after 0 syncs: any will be dead
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::ClrVGUIMessages()
{
    ClearSubElements(false);    // Currently msgs get 'zombified' without the cleanup
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::FixupLTimeChange(const float & sync_time)
{
    float loc_delta = - GetAbsLSyncTime();
    SetAbsLSyncTime(sync_time);
    loc_delta += GetAbsLSyncTime();
    SetGameIsRunning(loc_delta != 0.0f);

    if (loc_delta < 0.0f)
    {
        for (int cur_index = getChildCount() - 1; cur_index >= 0; cur_index--)
        {
            VGUI_TextMsgRecord * cur_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
            if (cur_panel)
            {
                cur_panel->FixupLTimeDelta(loc_delta);  // No need to adjust it for a server to spawn
            }
        }
        for (int cur_index = m_aLivingZombies.getCount() - 1; cur_index >= 0; cur_index--)
        {
            VGUI_TextMsgRecord * cur_panel = m_aLivingZombies[cur_index];
            if (cur_panel)
            {
                cur_panel->FixupLTimeDelta(loc_delta);  // No need to adjust it for a server to spawn
            }
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::TurnIntoAZombie(const int & cur_index)
{
    VGUI_TextMsgRecord * dead_child = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
    if (dead_child)
    {
        dead_child->TurnIntoAZombie();
        ExecPreDelOper(cur_index);
        dead_child->SendNextMessage();
        AckThisIsAZombie(cur_index);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::TurnIntoZombies()
{
    for (int cur_index = getChildCount() - 1; cur_index >= 0; cur_index--)
    {
        TurnIntoAZombie(cur_index);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::AckThisIsAZombie(const int & cur_index)
{
    VGUI_TextMsgRecord * dead_child = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
    if (dead_child && dead_child->RecordIsAZombie())
    {
        removeChild(dead_child);
        m_aLivingZombies.putElement(dead_child);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::CheckForAZombie(const int & cur_index)
{
    VGUI_TextMsgRecord * cur_child = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
    if (cur_child && cur_child->RecordIsAZombie())
    {
        ExecPreDelOper(cur_index);
        cur_child->SendNextMessage();
        AckThisIsAZombie(cur_index);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::CheckForZombies()
{
    for (int cur_index = getChildCount() - 1; cur_index >= 0; cur_index--)
    {
        CheckForAZombie(cur_index);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::BuryDeadZombies(const int & delete_delay)
{
    for (int cur_index = m_aLivingZombies.getCount() - 1; cur_index >= 0; cur_index--)
    {
        VGUI_TextMsgRecord * cur_panel = m_aLivingZombies[cur_index];
        if (cur_panel && cur_panel->GetZombieSyncNum() >= delete_delay)
        {
            m_aLivingZombies.setElementAt(NULL, cur_index);
            m_aLivingZombies.removeElementAt(cur_index);
            delete cur_panel;   // WARNING: base class has non-virtual destructor
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::ExecPreAddOper(Panel * const new_panel)
{
    for (int cur_index = getChildCount() - 1; cur_index >= 0; cur_index--)
    {
        VGUI_TextMsgRecord * cur_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
        if (cur_panel)
        {
            cur_panel->PreAddOperation(new_panel);
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::ExecPreDelOper(const int & cur_index)
{
    VGUI_TextMsgRecord * cur_child = dynamic_cast<VGUI_TextMsgRecord *>(getChild(cur_index));
    if (cur_child)
    {
        if ((cur_child->GetAffectedGroup() & AFFECTED_GROUP_NEXT) == AFFECTED_GROUP_NEXT)
        {
            for (int flw_index = getChildCount() - 1; flw_index > cur_index; flw_index--)
            {
                VGUI_TextMsgRecord * flw_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(flw_index));
                if (flw_panel)
                {
                    flw_panel->PreDelOperation(cur_child);
                }
            }
        }
        if ((cur_child->GetAffectedGroup() & AFFECTED_GROUP_PREV) == AFFECTED_GROUP_PREV)
        {
            for (int pre_index = cur_index - 1;       pre_index >= 0;        pre_index--)
            {
                VGUI_TextMsgRecord * pre_panel = dynamic_cast<VGUI_TextMsgRecord *>(getChild(pre_index));
                if (pre_panel)
                {
                    pre_panel->PreDelOperation(cur_child);
                }
            }
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::AddVGUIMessage(const client_textmessage_bundle_t * const pBundle, const void * const pSpecData, const bool & bDrawable, const float & fBirthTime)
{
    if (pBundle)    // Here we've nothing to do with the pSpecData
    {
        SynchronizeBase(bDrawable);

        if (MsgAddPermitted(pBundle))
        {
            VGUI_TextMsgRecord * new_panel = new VGUI_TextMsgRecord(pBundle, fBirthTime, GetShadowOffsetX(), GetShadowOffsetY()
                                                                    /* Do not specify the msg type ID explicitly, omit it! */);
            ExecPreAddOper(new_panel);
            addChild(new_panel);    // It will be added as the last
            
            // FIXME: also update NoRepeat state
        }
    } else {
        gEngfuncs.Con_DPrintf("ERROR: AddVGUIMessage(): Incoming message bundle is NULL!\n");
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::AcquireCVarValues()
{
    setVisible(CVAR_GET_FLOAT("cl_sm_textmsgs_enabled") != 0.0f);
    SetShadowOffsetX(CVAR_GET_FLOAT("cl_sm_textmsgs_shadow_offset_x"));
    SetShadowOffsetY(CVAR_GET_FLOAT("cl_sm_textmsgs_shadow_offset_y"));
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::RegisterCVarNames_S()
{
    if (!m_bCVarNamesReg)
    {
        m_bCVarNamesReg = true;
        CVAR_CREATE("cl_sm_textmsgs_enabled",         "1", FCVAR_ARCHIVE);
        CVAR_CREATE("cl_sm_textmsgs_shadow_offset_x", "0", 0);
        CVAR_CREATE("cl_sm_textmsgs_shadow_offset_y", "0", 0);
    }
}































bool VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::m_bCVarNamesReg = false;

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetExternBorderL() const
{
    return m_fExternBorderL;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetExternBorderR() const
{
    return m_fExternBorderR;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetExternBorderT() const
{
    return m_fExternBorderT;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetExternBorderB() const
{
    return m_fExternBorderB;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetBackgrOpacity() const
{
    return m_fBackgrOpacity;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetBackgrBorderX() const
{
    return m_fBackgrBorderX;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::GetBackgrBorderY() const
{
    return m_fBackgrBorderY;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetExternBorderL(const float & value)
{
    m_fExternBorderL = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetExternBorderR(const float & value)
{
    m_fExternBorderR = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetExternBorderT(const float & value)
{
    m_fExternBorderT = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetExternBorderB(const float & value)
{
    m_fExternBorderB = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetBackgrOpacity(const float & value)
{
    m_fBackgrOpacity = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetBackgrBorderX(const float & value)
{
    m_fBackgrBorderX = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::SetBackgrBorderY(const float & value)
{
    m_fBackgrBorderY = value;
}

VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgsPanel(const int & pos_x, const int & pos_y, const int & size_x, const int & size_y):
                                        VGUI_TextMsgsPanel(pos_x, pos_y, size_x, size_y)
{
    SetExternBorderL(0.0f);
    SetExternBorderR(0.0f);
    SetExternBorderT(0.0f);
    SetExternBorderB(0.0f);
    SetBackgrOpacity(0.0f);
    SetBackgrBorderX(0.0f);
    SetBackgrBorderY(0.0f);
}

VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::~VGUI_SubtMsgsPanel()
{
    setVisible(false);
    ClearSubElements(true);
}

bool VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::MsgAddPermitted(const client_textmessage_bundle_t * const pBundle)
{
    // Fograin92: Check if the message name differs from the last added one's to suppress subtitling ingame fake echo
    // when the same sentence is played repeatedly at almost the same time (e.g., Nihilanth's voice on the c4a1b map)
    // FIXME: when NoRepeat will be done, it will become obsolete
    
    
    bool result = !pBundle->bMustIgnore;
    
    for (int last_idx = getChildCount() - 1; result && last_idx >= 0; last_idx--)
    {
        VGUI_SubtMsgRecord * lst_panel = dynamic_cast<VGUI_SubtMsgRecord *>(getChild(last_idx));
        if (lst_panel && !lst_panel->RecordIsAZombie())
        {
            result = (stricmp(pBundle->TextMessage.pName, lst_panel->GetMessageName()) != 0);
        }
    }
    return /* GetGameIsRunning() && */ result;  // Ignore message if on pause (currently disabled)
}

bool VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::ShowSubElements()
{
    return VGUI_TextMsgsPanel::ShowSubElements() && GetGameIsRunning();
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::AddVGUIMessage(const client_textmessage_bundle_t * const pBundle, const void * const pSpecData, const bool & bDrawable, const float & fBirthTime)
{
    if (pBundle && pSpecData)
    {
        SynchronizeBase(bDrawable);

        if (MsgAddPermitted(pBundle))
        {
            VGUI_SubtMsgRecord * new_panel = new VGUI_SubtMsgRecord(pBundle, pSpecData, fBirthTime,
                                                                    GetShadowOffsetX(), GetShadowOffsetY(), GetBackgrOpacity(),
                                                                    GetBackgrBorderX(), GetBackgrBorderY(), GetExternBorderL(),
                                                                    GetExternBorderR(), GetExternBorderT(), GetExternBorderB()
                                                                    /* Do not specify the msg type ID explicitly, omit it! */);
            ExecPreAddOper(new_panel);
            addChild(new_panel);    // It will be added as the last
            
            // FIXME: also update NoRepeat state
        }
    } else {
        gEngfuncs.Con_DPrintf("ERROR: AddVGUIMessage(): Incoming message bundle and/or msg-specific data is NULL!\n");
    }
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::AcquireCVarValues()
{
    setVisible(CVAR_GET_FLOAT( "cl_sm_subtitles_enabled" ) != 0.0f);
    SetShadowOffsetX(CVAR_GET_FLOAT("cl_sm_subtitles_shadow_offset_x"  ));
    SetShadowOffsetY(CVAR_GET_FLOAT("cl_sm_subtitles_shadow_offset_y"  ));
    SetExternBorderL(CVAR_GET_FLOAT("cl_sm_subtitles_ext_border_left"  ));
    SetExternBorderR(CVAR_GET_FLOAT("cl_sm_subtitles_ext_border_right" ));
    SetExternBorderT(CVAR_GET_FLOAT("cl_sm_subtitles_ext_border_top"   ));
    SetExternBorderB(CVAR_GET_FLOAT("cl_sm_subtitles_ext_border_bottom"));
    SetBackgrOpacity(CVAR_GET_FLOAT("cl_sm_subtitles_bg_opacity"       ));
    SetBackgrBorderX(CVAR_GET_FLOAT("cl_sm_subtitles_bg_border_x"      ));
    SetBackgrBorderY(CVAR_GET_FLOAT("cl_sm_subtitles_bg_border_y"      ));
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::RegisterCVarNames_S()
{
    if (!m_bCVarNamesReg)
    {
        m_bCVarNamesReg = true;
        CVAR_CREATE("cl_sm_subtitles_enabled",             "1", FCVAR_ARCHIVE);
        CVAR_CREATE("cl_sm_subtitles_ext_border_left",  "0.05", 0);
        CVAR_CREATE("cl_sm_subtitles_ext_border_right", "0.05", 0);
        CVAR_CREATE("cl_sm_subtitles_ext_border_top",    "0.1", 0);
        CVAR_CREATE("cl_sm_subtitles_ext_border_bottom", "0.1", 0);
        CVAR_CREATE("cl_sm_subtitles_bg_opacity",        "0.5", 0);
        CVAR_CREATE("cl_sm_subtitles_bg_border_x",    "0.0100", 0);
        CVAR_CREATE("cl_sm_subtitles_bg_border_y",    "0.0105", 0);
        CVAR_CREATE("cl_sm_subtitles_shadow_offset_x",     "1", 0);
        CVAR_CREATE("cl_sm_subtitles_shadow_offset_y",     "1", 0);
    }
}



































const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMessageTypeID() const
{
    return m_iMessageTypeID;
}

const char * const VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMessageName() const
{
    return m_szMessageName;
}

const char * const VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetNextMsgName() const
{
    return m_szNextMsgName;
}

const char * const VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMessageText() const
{
    return m_szMessageText;
}

const void * const VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetSpecificData() const
{
    return m_pSpecificData;
}

const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMsgEffectID() const
{
    return m_iMsgEffectID;
}

const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetAffectedGroup() const
{
    return m_iAffectedGroup;
}

const int & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetZombieSyncNum() const
{
    return m_iZombieSyncNum;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMsgPositionX() const
{
    return m_fMsgPositionX;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMsgPositionY() const
{
    return m_fMsgPositionY;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetAbsLSyncTime() const
{
    return m_fAbsLSyncTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetAbsBirthTime() const
{
    return m_fAbsBirthTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetAbsDeathTime() const
{
    return m_fAbsDeathTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelLSyncDelta() const
{
    return m_fRelLSyncDelta;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelFadeInTime() const
{
    return m_fRelFadeInTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelCharInTime() const
{
    return m_fRelCharInTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelColorFXTime() const
{
    return m_fRelColorFXTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelHoldTime() const
{
    return m_fRelHoldTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetRelFadeOutTime() const
{
    return m_fRelFadeOutTime;
}

const float & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMsgBgOpacity() const
{
    return m_fMsgBgOpacity;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetFXClrChannelR() const
{
    return m_bFXClrChannelR;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetFXClrChannelG() const
{
    return m_bFXClrChannelG;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetFXClrChannelB() const
{
    return m_bFXClrChannelB;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetFXClrChannelA() const
{
    return m_bFXClrChannelA;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetHTClrChannelR() const
{
    return m_bHTClrChannelR;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetHTClrChannelG() const
{
    return m_bHTClrChannelG;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetHTClrChannelB() const
{
    return m_bHTClrChannelB;
}

const byte & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetHTClrChannelA() const
{
    return m_bHTClrChannelA;
}

const bool & VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetAdditiveMode() const
{
    return m_bAdditiveMode;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_TextMsgRecord::GetExternBordersH(int & bdr_l, int & bdr_r) const
{
    bdr_l = m_iExternBorderL;
    bdr_r = m_iExternBorderR;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_TextMsgRecord::GetExternBordersV(int & bdr_t, int & bdr_b) const
{
    bdr_t = m_iExternBorderT;
    bdr_b = m_iExternBorderB;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetMsgBgBorders(int & bdr_x, int & bdr_y) const
{
    bdr_x = m_iMsgBgBorderX;
    bdr_y = m_iMsgBgBorderY;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::GetShadowOffsets(int & off_x, int & off_y) const
{
    off_x = m_iShadowOffsetX;
    off_y = m_iShadowOffsetY;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMessageName(const char * const value)
{
    m_szMessageName = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetNextMsgName(const char * const value)
{
    m_szNextMsgName = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::DelNextMsgName()
{
    delete[] m_szNextMsgName;
    m_szNextMsgName = NULL;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMessageText(const char * const value)
{
    m_szMessageText = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetSpecificData(const void * const value)
{
    m_pSpecificData = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMsgEffectID(const int & value)
{
    m_iMsgEffectID = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetAffectedGroup(const int & value)
{
    m_iAffectedGroup = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetZombieSyncNum(const int & value)
{
    m_iZombieSyncNum = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::IncZombieSyncNum()
{
    m_iZombieSyncNum++;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMsgPositionX(const float & value)
{
    m_fMsgPositionX = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMsgPositionY(const float & value)
{
    m_fMsgPositionY = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetAbsLSyncTime(const float & value)
{
    m_fAbsLSyncTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetAbsBirthTime(const float & value)
{
    m_fAbsBirthTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetAbsDeathTime(const float & value)
{
    m_fAbsDeathTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelLSyncDelta(const float & value)
{
    m_fRelLSyncDelta = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelFadeInTime(const float & value)
{
    m_fRelFadeInTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelCharInTime(const float & value)
{
    m_fRelCharInTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelColorFXTime(const float & value)
{
    m_fRelColorFXTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelHoldTime(const float & value)
{
    m_fRelHoldTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetRelFadeOutTime(const float & value)
{
    m_fRelFadeOutTime = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::MulRelFadeInTime(const float & value)
{
    m_fRelFadeInTime *= value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::MulRelCharInTime(const float & value)
{
    m_fRelCharInTime *= value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::MulRelColorFXTime(const float & value)
{
    m_fRelColorFXTime *= value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::MulRelHoldTime(const float & value)
{
    m_fRelHoldTime *= value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::MulRelFadeOutTime(const float & value)
{
    m_fRelFadeOutTime *= value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMsgBgOpacity(const float & value)
{
    m_fMsgBgOpacity = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetFXClrChannelR(const byte & value)
{
    m_bFXClrChannelR = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetFXClrChannelG(const byte & value)
{
    m_bFXClrChannelG = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetFXClrChannelB(const byte & value)
{
    m_bFXClrChannelB = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetFXClrChannelA(const byte & value)
{
    m_bFXClrChannelA = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetHTClrChannelR(const byte & value)
{
    m_bHTClrChannelR = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetHTClrChannelG(const byte & value)
{
    m_bHTClrChannelG = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetHTClrChannelB(const byte & value)
{
    m_bHTClrChannelB = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetHTClrChannelA(const byte & value)
{
    m_bHTClrChannelA = value;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetAdditiveMode(const bool & value)
{
    m_bAdditiveMode = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_TextMsgRecord::SetExternBordersH(const int & bdr_l, const int & bdr_r)
{
    m_iExternBorderL = bdr_l;
    m_iExternBorderR = bdr_r;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_TextMsgRecord::SetExternBordersV(const int & bdr_t, const int & bdr_b)
{
    m_iExternBorderT = bdr_t;
    m_iExternBorderB = bdr_b;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetMsgBgBorders(const int & bdr_x, const int & bdr_y)
{
    m_iMsgBgBorderX = bdr_x;
    m_iMsgBgBorderY = bdr_y;
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SetShadowOffsets(const int & off_x, const int & off_y)
{
    m_iShadowOffsetX = off_x;
    m_iShadowOffsetY = off_y;
}

VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::VGUI_TextMsgRecord(
    const client_textmessage_bundle_t * const msg_bundle,
    const float & birth_time,
    const int & sh_offs_x,
    const int & sh_offs_y,
    const float & bg_opacity,
    const float & bg_bord_x,
    const float & bg_bord_y,
    const float & ext_bdr_l,
    const float & ext_bdr_r,
    const float & ext_bdr_t,
    const float & ext_bdr_b,
    const int & scr_size_x,
    const int & scr_size_y,
    const bool & use_msg_pos,
    const bool & is_additive,
    const int & msg_type):
    Panel(0, 0, scr_size_x, scr_size_y),
    m_iMessageTypeID(msg_type)
{
    setVisible(false);
    setEnabled(true);   // Isn't killed yet
    setAutoFocusNavEnabled(false);
    setPaintEnabled(true);
    setPaintBackgroundEnabled(true);
    setBgColor(0, 0, 0, VGUI_ALPHA_TRANSP);

    int bord_h_l = scr_size_x * ext_bdr_l,
        bord_h_r = scr_size_x * ext_bdr_r,
        bord_v_t = scr_size_y * ext_bdr_t,
        bord_v_b = scr_size_y * ext_bdr_b;

    int max_rec_w = scr_size_x - (bord_h_l + bord_h_r),
        max_rec_h = scr_size_y - (bord_v_t + bord_v_b);

    int bg_bdr_ix = max_rec_w * bg_bord_x,
        bg_bdr_iy = max_rec_h * bg_bord_y;

    SetSpecificData(NULL);  // No spec data
    SetExternBordersH(bord_h_l, bord_h_r);
    SetExternBordersV(bord_v_t, bord_v_b);
    SetMsgBgBorders(bg_bdr_ix, bg_bdr_iy);
    SetShadowOffsets(sh_offs_x, sh_offs_y);
    SetAffectedGroup(AFFECTED_GROUP_ALL);
    SetZombieSyncNum(0);
    SetAbsLSyncTime(birth_time);
    SetAbsBirthTime(birth_time);
    SetRelLSyncDelta(0.0f);
    SetMsgBgOpacity(bg_opacity);
    SetAdditiveMode(is_additive);

    SetMessageName(NULL);
    SetMessageText(NULL);
    SetNextMsgName(NULL);

    if (msg_bundle)
    {
		const client_textmessage_t * text_msg = &msg_bundle->TextMessage;





        char * msg_name = new char[strlen(text_msg->pName) + 1];
        strcpy(msg_name, text_msg->pName);
        SetMessageName(msg_name);

        char * msg_text = new char[strlen(text_msg->pMessage) + 1];
        strcpy(msg_text, text_msg->pMessage);
        SetMessageText(msg_text);

        if (msg_bundle->pChainedMsg)
        {
            char * next_msg = new char[strlen(msg_bundle->pChainedMsg) + 1];
            strcpy(next_msg, msg_bundle->pChainedMsg);
            SetNextMsgName(next_msg);
        }

        int text_len = strlen(GetMessageText());

        SetMsgEffectID(text_msg->effect & ~(MSG_FLAG_IS_VGUI));
        SetMsgPositionX(text_msg->x);
        SetMsgPositionY(text_msg->y);
        
        if (GetAdditiveMode())
        {
            // Simulates the additive mode: calculates the alpha to adjust RGB with
            byte  color_a = max(max(text_msg->r2, text_msg->g2), text_msg->b2);
            float color_n = color_a ? VGUI_ALPHA_TRANSP / (float)(color_a) : 0.0f;
            SetFXClrChannelR(text_msg->r2 * color_n);
            SetFXClrChannelG(text_msg->g2 * color_n);
            SetFXClrChannelB(text_msg->b2 * color_n);
            SetFXClrChannelA(VGUI_ALPHA_TRANSP - color_a);

            color_a = max(max(text_msg->r1, text_msg->g1), text_msg->b1);
            color_n = color_a ? VGUI_ALPHA_TRANSP / (float)(color_a) : 0.0f;
            SetHTClrChannelR(text_msg->r1 * color_n);
            SetHTClrChannelG(text_msg->g1 * color_n);
            SetHTClrChannelB(text_msg->b1 * color_n);
            SetHTClrChannelA(VGUI_ALPHA_TRANSP - color_a);
        }
        else
        {
            SetFXClrChannelR(text_msg->r2);
            SetFXClrChannelG(text_msg->g2);
            SetFXClrChannelB(text_msg->b2);
            SetFXClrChannelA(text_msg->a2);

            SetHTClrChannelR(text_msg->r1);
            SetHTClrChannelG(text_msg->g1);
            SetHTClrChannelB(text_msg->b1);
            SetHTClrChannelA(text_msg->a1);
        }

        switch (GetMsgEffectID())
        {
            case MSG_EFF_SHOWHIDE:
            case MSG_EFF_FLICKERY:
            {
                SetRelFadeInTime(text_msg->fadein);
                SetRelCharInTime(0.0f);
                SetRelColorFXTime(0.0f);
                SetRelHoldTime(text_msg->holdtime);
                SetRelFadeOutTime(text_msg->fadeout);
                break;
            }
            case MSG_EFF_WRITEOUT:
            {
                SetRelFadeInTime(text_msg->fadein * text_len);
                SetRelCharInTime(text_msg->fadein);
                SetRelColorFXTime(text_msg->fxtime);
                SetRelHoldTime(text_msg->holdtime);
                SetRelFadeOutTime(text_msg->fadeout);
                break;
            }
            default:
            {
                gEngfuncs.Con_DPrintf("ERROR: VGUI_TextMsgRecord(): Unknown effect 0x%08x!\n", GetMsgEffectID());

                SetRelFadeInTime(0.0f);
                SetRelCharInTime(0.0f);
                SetRelColorFXTime(0.0f);
                SetRelHoldTime(0.0f);
                SetRelFadeOutTime(0.0f);
                break;
            }
        }

        SetAbsDeathTime(GetAbsBirthTime() + GetRelFadeInTime() + GetRelHoldTime() + GetRelFadeOutTime());





        const char * font_name = msg_bundle->pSchemeName;
        
        if (!font_name)
        {
            if (GetMessageTypeID() == MSG_TYPE_SUBTITLE)
            {
                font_name = "Subtitles Text";
            }
            else switch (GetMsgEffectID())
            {
                case MSG_EFF_SHOWHIDE:
                {
                    font_name = "Show-Hide Text";
                    break;
                }
                case MSG_EFF_FLICKERY:
                {
                    font_name = "Credits Text";
                    break;
                }
                case MSG_EFF_WRITEOUT:
                {
                    font_name = "Writeout Text";
                    break;
                }
                default:
                {
                    font_name = "Default Scheme";
                    break;
                }
            }
        }
        
        CSchemeManager * scheme_mgr = gViewPort->GetSchemeManager();
        Font * cur_font = scheme_mgr->getFont(scheme_mgr->getSchemeHandle(font_name, true, true));













        bool xl_align = false, xc_align = false,    // the X alignment, checking order: L,C,R (R = none)
             yt_align = false, yc_align = false;    // the Y alignment, checking order: T,C,B (B = none)

        float xpos_ratio = GetMsgPositionX(),
              ypos_ratio = GetMsgPositionY();

        if (xpos_ratio == -1.0f)
        {
            xc_align = true;
            if (use_msg_pos) xpos_ratio = 0.5f;
        }
        else if (xpos_ratio < -1.0f)
        {
            xc_align = true;
            if (use_msg_pos) xpos_ratio += 2.0f;
        }
        else if (xpos_ratio <  0.0f)
        {
            // alignment: right
            if (use_msg_pos) xpos_ratio += 1.0f;
        }
        else
        {
            xl_align = true;    // Default: left
        }

        if (ypos_ratio == -1.0f)
        {
            yc_align = true;
            if (use_msg_pos) ypos_ratio = 0.5f;
        }
        else if (ypos_ratio < -1.0f)
        {
            yc_align = true;
            if (use_msg_pos) ypos_ratio += 2.0f;
        }
        else if (ypos_ratio <  0.0f)
        {
            // alignment: bottom
            if (use_msg_pos) ypos_ratio += 1.0f;
        }
        else
        {
            yt_align = true;    // Default: top
        }

        int addw_max = (use_msg_pos ? max_rec_w * (xl_align ? 1.0f - xpos_ratio : (xc_align ? 2.0f * min(xpos_ratio, 1.0f - xpos_ratio) : xpos_ratio)) : max_rec_w) - 2 * bg_bdr_ix - abs(sh_offs_x);

        int max_wdth = use_msg_pos ? 0 : addw_max,
            b_x_corr = bg_bdr_ix - min(0, sh_offs_x),
            cur_hght = bg_bdr_iy - min(0, sh_offs_y),
            fnt_tall = cur_font->getTall();

        const char * curr_end = GetMessageText();
        bool last_eol = false;

        if (GetMsgEffectID() == MSG_EFF_WRITEOUT)
        {
            m_pMsgTextImages.ensureCapacity(text_len);  // Store characters as separate images
        }

        while (!SM_CurrentlyAtEOF(curr_end))    // This loop searches and adds msg text images
        {
            int cur_wdth = 0, beg_pref = 0, prv_post = 0, addw_rem = addw_max;

            const char * curr_beg = curr_end, * ackd_end = curr_end;
            last_eol = SM_SkipCurrentEOL(curr_end); // True if this text line has the EOL only
            bool find_nxt = !last_eol;  // If this line has no text, there are no words to get

            while (find_nxt)    // This loop appends words found; no need to check for the EOL
            {
                int add_wdth = 0, bkp_post = prv_post;  // No need to backup the beg_pref data
                const char * prev_end = curr_end;
                bool at_start = (curr_end == curr_beg);
                bool is_first = at_start;

                find_nxt = SM_FindCurrentDLM(curr_end); // False if we have found the text EOF

                for (const char * temp_end = ackd_end; temp_end < curr_end; temp_end++)
                {
                    int chr_pref, chr_wdth, chr_post;
                    cur_font->getCharABCwide(temp_end[0], chr_pref, chr_wdth, chr_post);

                    // Take the empty space before the first char or width deltas into account
                    if (is_first) beg_pref = chr_pref; else chr_wdth += prv_post + chr_pref;
                    prv_post = chr_post;    // The next char will add the current's post-space

                    if (at_start && !is_first && (chr_wdth > addw_rem - add_wdth))
                    {
                        // The line just started and the non-first char falls out of text area
                        curr_end = temp_end;    // Fixed curr_end'll also break the inner loop
                    } else {
                        // The first char of the line gets picked anyway to avoid the inf loop
                        add_wdth += chr_wdth;   // Conditions weren't met, simply add the char
                        is_first = false;
                    }
                }

                bool width_ok = (add_wdth <= addw_rem);

                if (width_ok || at_start)   // Skip the overflow check for the very first char
                {
                    bool skip_wsp;
                    if (curr_end == ackd_end)
                    {
                        skip_wsp = true;    // It'll save us as we are struggling with the WSP
                    } else {
                        ackd_end = curr_end;
                        cur_wdth += add_wdth;
                        addw_rem -= add_wdth;
                        skip_wsp = width_ok;
                    }
                    if (find_nxt)   // False if we've found the text EOF (so, nothing to skip)
                    {
                        if (skip_wsp) SM_SkipWSPSetFrom(curr_end);  // Skip the set if allowed
                        last_eol = SM_SkipCurrentEOL(curr_end); // Skip the line EOL if exists
                        // Stop on the text line width overflow, or if either EOL or EOF found
                        find_nxt = width_ok && !(last_eol || SM_CurrentlyAtEOF(curr_end));
                    }
                } else {
                    find_nxt = false;   // We cannot add the next word, the line'd be too long
                    curr_end = prev_end;    // Falling back on the previously stored text line
                    prv_post = bkp_post;
                }
            }
            max_wdth = max(cur_wdth, max_wdth);

            if (GetMsgEffectID() == MSG_EFF_WRITEOUT)   // If true, adds the line char by char
            {
                int tmp_xpos = 0;
                bool is_first = true;
                // This loop'll never be ignored (we can't be here if the message has no text)
                for (const char * temp_end = curr_beg; temp_end < curr_end; temp_end++)
                {
                    int chr_pref, chr_wdth, chr_post;
                    cur_font->getCharABCwide(temp_end[0], chr_pref, chr_wdth, chr_post);
                    if (is_first)
                    {
                        tmp_xpos = xl_align ? b_x_corr - chr_pref :
                                              cur_wdth + (xc_align ? 2 * chr_pref : chr_pref);
                        is_first = false;
                    }
                    chr_wdth += chr_pref;   // The total image width is chr_pref pixels bigger 
                    TextImage * txt_image = new TextImage("");
                    txt_image->setFont(cur_font);
                    txt_image->setPos(tmp_xpos, cur_hght);
                    txt_image->setSize(chr_wdth, fnt_tall);
                    txt_image->setText(2, temp_end);
                    m_pMsgTextImages.addElement(txt_image);
                    tmp_xpos += chr_wdth + chr_post;
                }
            } else {
                int adj_wdth = cur_wdth;
                bool is_first = (ackd_end == curr_beg);
                // This loop'll be ignored if the curr line has no trailing delims but the EOF
                for (const char * temp_end = ackd_end; temp_end < curr_end; temp_end++)
                {
                    int chr_pref, chr_wdth, chr_post;
                    cur_font->getCharABCwide(temp_end[0], chr_pref, chr_wdth, chr_post);
                    if (is_first)
                    {
                        beg_pref = chr_pref;    // The empty space before the first char image
                        is_first = false;
                    } else chr_wdth += prv_post + chr_pref; // Take width changes into account
                    prv_post = chr_post;    // The next char will add the curr char post-space
                    adj_wdth += chr_wdth;
                }
                TextImage * txt_image = new TextImage("");
                txt_image->setFont(cur_font);
                txt_image->setPos(xl_align ? b_x_corr - beg_pref :
                                             cur_wdth + (xc_align ? 2 * beg_pref : beg_pref),
                                  cur_hght);
                // There must be enough pixels for chars to avoid the text image auto-wrapping
                txt_image->setSize(beg_pref + adj_wdth, fnt_tall);
                txt_image->setText(curr_end - curr_beg + 1, curr_beg);
                m_pMsgTextImages.addElement(txt_image);
            }
            cur_hght += fnt_tall;
        }

        if (last_eol) cur_hght += fnt_tall;

        if (!xl_align)  // If the message text is left-aligned, correct X positions were set during Dar fulfilling
        {
            int imgs_num = m_pMsgTextImages.getCount(), cr_bound = b_x_corr + (xc_align ? max_wdth / 2 : max_wdth);
            if (GetMsgEffectID() == MSG_EFF_WRITEOUT)
            {
                int x_adjust = 0, str_ypos = INT_MIN;
                for (int char_index = 0; char_index < imgs_num; char_index++)
                {
                    int cur_xpos, cur_ypos;
                    m_pMsgTextImages[char_index]->getPos(cur_xpos, cur_ypos);
                    if ((cur_ypos != str_ypos) || (char_index == 0))    // Found the next line, must update values
                    {
                        str_ypos = cur_ypos;
                        x_adjust = cr_bound - (xc_align ? cur_xpos / 2 : cur_xpos);
                        m_pMsgTextImages[char_index]->setPos(x_adjust, cur_ypos);
                        x_adjust -= cur_xpos;
                    } else {
                        m_pMsgTextImages[char_index]->setPos(cur_xpos + x_adjust, cur_ypos);
                    }
                }
            } else {
                for (int line_index = 0; line_index < imgs_num; line_index++)
                {
                    int str_wdth, cur_ypos;
                    m_pMsgTextImages[line_index]->getPos(str_wdth, cur_ypos);
                    m_pMsgTextImages[line_index]->setPos(cr_bound - (xc_align ? str_wdth / 2 : str_wdth), cur_ypos);
                }
            }
        }

        int rec_xpos = bord_h_l, rec_ypos = bord_v_t;
        max_wdth += 2 * bg_bdr_ix + abs(sh_offs_x);
        cur_hght += max(0, sh_offs_y) + bg_bdr_iy;

        if (use_msg_pos)
        {
            rec_xpos += max_rec_w * xpos_ratio - (xl_align ? 0 : (xc_align ? max_wdth / 2 : /* right  */ max_wdth));
            rec_ypos += max_rec_h * ypos_ratio - (yt_align ? 0 : (yc_align ? cur_hght / 2 : /* bottom */ cur_hght));
        }

        setBounds(rec_xpos, rec_ypos, max_wdth, cur_hght);
    }
    else
    {
        gEngfuncs.Con_DPrintf("ERROR: VGUI_TextMsgRecord(): Message bundle for the record to be made from is NULL!\n");

        SetMsgEffectID(0);
        SetMsgPositionX(0);
        SetMsgPositionY(0);

        SetFXClrChannelR(0);
        SetFXClrChannelG(0);
        SetFXClrChannelB(0);
        SetFXClrChannelA(0);

        SetHTClrChannelR(0);
        SetHTClrChannelG(0);
        SetHTClrChannelB(0);
        SetHTClrChannelA(0);

        SetRelFadeInTime(0.0f);
        SetRelCharInTime(0.0f);
        SetRelColorFXTime(0.0f);
        SetRelHoldTime(0.0f);
        SetRelFadeOutTime(0.0f);

        SetAbsDeathTime(0.0f);
        setBounds(0, 0, 0, 0);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SynchronizeFull(const bool & show_rec, const float & sync_time)
{
    if (RecordIsAZombie()) IncZombieSyncNum();
    SetRelLSyncDelta(sync_time - GetAbsLSyncTime());
    SetAbsLSyncTime(sync_time);
    setVisible(!RecordIsAZombie() && show_rec);

    if (isVisibleUp())  // Do nothing if invisible
    {
        float stage_time = GetAbsLSyncTime() - GetAbsBirthTime();

        if (stage_time > 0.0f)
        {
            float live_ratio, dead_ratio = 0.0f, time_alive = stage_time;

            if (stage_time <= GetRelFadeInTime())
            {
                live_ratio = stage_time / GetRelFadeInTime();
            } else {
                stage_time -= GetRelFadeInTime();
                if (stage_time <= GetRelHoldTime())
                {
                    live_ratio = 1.0f;
                } else {
                    stage_time -= GetRelHoldTime();
                    if (stage_time <= GetRelFadeOutTime())
                    {
                        dead_ratio = stage_time / GetRelFadeOutTime();
                        live_ratio = 1.0f - dead_ratio;
                    } else {
                        dead_ratio = 1.0f;
                        live_ratio = 0.0f;
                    }
                }
            }

            setBgColor(0, 0, 0, VGUI_ALPHA_TRANSP * (1.0f - live_ratio * GetMsgBgOpacity()));
            int images_num = m_pMsgTextImages.getCount();
            Color text_clr;

            switch (GetMsgEffectID())
            {
                case MSG_EFF_SHOWHIDE:
                case MSG_EFF_FLICKERY:
                {
                    byte ht_chn_a = VGUI_ALPHA_TRANSP - (VGUI_ALPHA_TRANSP - GetHTClrChannelA()) * live_ratio;

                    if (GetMsgEffectID() == MSG_EFF_FLICKERY && (rand() % 100) < 10)
                    {
                        if (GetAdditiveMode())
                        {
                            byte chan_r = min((GetFXClrChannelR() * (VGUI_ALPHA_TRANSP - GetFXClrChannelA()) + 
                                               GetHTClrChannelR() * (VGUI_ALPHA_TRANSP - ht_chn_a)) / 255, 255);
                            byte chan_g = min((GetFXClrChannelG() * (VGUI_ALPHA_TRANSP - GetFXClrChannelA()) +
                                               GetHTClrChannelG() * (VGUI_ALPHA_TRANSP - ht_chn_a)) / 255, 255);
                            byte chan_b = min((GetFXClrChannelB() * (VGUI_ALPHA_TRANSP - GetFXClrChannelA()) +
                                               GetHTClrChannelB() * (VGUI_ALPHA_TRANSP - ht_chn_a)) / 255, 255);
                            byte chan_a = max(max(chan_r, chan_g), chan_b);

                            float c_norm = (chan_a == 0) ? 0.0f : VGUI_ALPHA_TRANSP / (float)(chan_a);
                            text_clr.setColor(chan_r * c_norm, chan_g * c_norm, chan_b * c_norm, VGUI_ALPHA_TRANSP - chan_a);
                        } else {
                            text_clr.setColor((GetFXClrChannelR() + GetHTClrChannelR()) / 2,    // Blending colors directly
                                              (GetFXClrChannelG() + GetHTClrChannelG()) / 2,
                                              (GetFXClrChannelB() + GetHTClrChannelB()) / 2,
                                              (GetFXClrChannelA() + ht_chn_a) / 2);
                        }
                    } else {
                        text_clr.setColor(GetHTClrChannelR(), GetHTClrChannelG(), GetHTClrChannelB(), ht_chn_a);
                    }
                    for (int line_index = 0; line_index < images_num; line_index++)
                    {
                        m_pMsgTextImages[line_index]->setColor(text_clr);
                    }
                    break;
                }
                case MSG_EFF_WRITEOUT:
                {
                    for (int char_index = 0; char_index < images_num; char_index++)
                    {
                        time_alive -= GetRelCharInTime();

                        if (time_alive > 0.0f)
                        {
                            byte chan_r, chan_g, chan_b, chan_a;

                            if (time_alive <= GetRelColorFXTime())
                            {
                                float fx_blend = time_alive / GetRelColorFXTime();
                                chan_r = GetFXClrChannelR() + (GetHTClrChannelR() - GetFXClrChannelR()) * fx_blend;
                                chan_g = GetFXClrChannelG() + (GetHTClrChannelG() - GetFXClrChannelG()) * fx_blend;
                                chan_b = GetFXClrChannelB() + (GetHTClrChannelB() - GetFXClrChannelB()) * fx_blend;
                                chan_a = GetFXClrChannelA() + (GetHTClrChannelA() - GetFXClrChannelA()) * fx_blend;
                            } else {
                                chan_r = GetHTClrChannelR();
                                chan_g = GetHTClrChannelG();
                                chan_b = GetHTClrChannelB();
                                chan_a = GetHTClrChannelA();
                            }
                            text_clr.setColor(chan_r, chan_g, chan_b, chan_a + (VGUI_ALPHA_TRANSP - chan_a) * dead_ratio);
                        } else {
                            text_clr.setColor(GetFXClrChannelR(), GetFXClrChannelG(), GetFXClrChannelB(), VGUI_ALPHA_TRANSP);
                        }
                        m_pMsgTextImages[char_index]->setColor(text_clr);
                    }
                    break;
                }
            }
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::PreAddOperation(Panel * const cur_panel)
{
    if (!RecordIsAZombie())
    {
        VGUI_TextMsgRecord * cur_rec = dynamic_cast<VGUI_TextMsgRecord *>(cur_panel);

        if (cur_rec)
        {
            if (fabs(GetMsgPositionX() - cur_rec->GetMsgPositionX()) < MSG_X_POS_EPSILON &&
                fabs(GetMsgPositionY() - cur_rec->GetMsgPositionY()) < MSG_Y_POS_EPSILON)
            {
                TurnIntoAZombie();
            }
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::PreDelOperation(Panel * const cur_panel)
{
    if (!RecordIsAZombie())
    {
        VGUI_TextMsgRecord * cur_rec = dynamic_cast<VGUI_TextMsgRecord *>(cur_panel);

        if (cur_rec)
        {
            // For the VGUI_TextMsgRecord class there are no actions to perform on record death events
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::SendNextMessage()
{
    if (GetNextMsgName())
    {
        if (gViewPort)
        {
            const client_textmessage_bundle_t * msg_bundle = gViewPort->GetSMClientMessage(GetNextMsgName());
            if (msg_bundle)
            {
                gViewPort->AddSMTextMessage(GetMessageTypeID(), msg_bundle, GetSpecificData(), GetAbsDeathTime());
            } else {
                gEngfuncs.Con_DPrintf("WARNING: Unable to find the chained message \"%s\"!\n", GetNextMsgName());
            }
        }
        DelNextMsgName();
    }
}

VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::~VGUI_TextMsgRecord()
{
    setVisible(false);
    DelNextMsgName();
    delete[] GetMessageText();
    SetMessageText(NULL);
    delete[] GetMessageName();
    SetMessageName(NULL);

    for (int cur_index = m_pMsgTextImages.getCount() - 1; cur_index >= 0; cur_index--)
    {
        delete m_pMsgTextImages[cur_index];
        m_pMsgTextImages.setElementAt(NULL, cur_index);
        m_pMsgTextImages.removeElementAt(cur_index);
    }
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::TurnIntoAZombie()
{
    setEnabled(false);  // If isEnabled() equals false it means the message is killed
    setVisible(false);  // Keeps the implication !isEnabled() -> !isVisible() in sync
    SetAffectedGroup(AFFECTED_GROUP_NONE);  // No one will care the message is killed
    DelNextMsgName( );
}

bool VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::RecordIsAZombie()
{
    return GetAbsLSyncTime() > GetAbsDeathTime() || !isEnabled();   // Died/is killed
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::FixupLTimeDelta(const float & time_delta)
{
    SetAbsLSyncTime(GetAbsLSyncTime() + time_delta);    // FIXME: избыточный Get! сделать Add-функции
    SetAbsBirthTime(GetAbsBirthTime() + time_delta);
    SetAbsDeathTime(GetAbsDeathTime() + time_delta);
}

void VGUI_MsgsBasePanel::VGUI_TextMsgsPanel::VGUI_TextMsgRecord::paint()
{
    int sh_off_x, sh_off_y, img_count = m_pMsgTextImages.getCount();
    GetShadowOffsets(sh_off_x, sh_off_y);

    if ((sh_off_x != 0) || (sh_off_y != 0)) // Text lines have a shadow
    {
        for (int cur_index = 0; cur_index < img_count; cur_index++)
        {
            Color orig_clr;
            int i_dummy, i_pos_x, i_pos_y, color_a;

            m_pMsgTextImages[cur_index]->getColor(orig_clr);
            orig_clr.getColor(i_dummy, i_pos_x, i_pos_y, color_a);
            m_pMsgTextImages[cur_index]->getPos(i_pos_x, i_pos_y);

            m_pMsgTextImages[cur_index]->setColor(Color(0, 0, 0, color_a));
            m_pMsgTextImages[cur_index]->setPos(i_pos_x + sh_off_x,
                                                i_pos_y + sh_off_y);

            m_pMsgTextImages[cur_index]->doPaint(this);

            m_pMsgTextImages[cur_index]->setPos(i_pos_x, i_pos_y);
            m_pMsgTextImages[cur_index]->setColor(orig_clr);
        }
    }

    for (int cur_index = 0; cur_index < img_count; cur_index++)
    {
        m_pMsgTextImages[cur_index]->doPaint(this);
    }
}



























const client_textmessage_sndprops_t & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::GetSubSoundProps() const
{
    return m_pSubSoundProps;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::GetRelTimeToMove() const
{
    return m_fRelTimeToMove;
}

const float & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::GetYMovementPend() const
{
    return m_fYMovementPend;
}

const int & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::GetYMovementToDo() const
{
    return m_iYMovementToDo;
}

const bool & VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::GetIsStoppedSnd() const
{
    return m_bIsStoppedSnd;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SetSubSoundProps(const client_textmessage_sndprops_t & value)
{
    m_pSubSoundProps = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SetRelTimeToMove(const float & value)
{
    m_fRelTimeToMove = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SetYMovementPend(const float & value)
{
    m_fYMovementPend = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SetYMovementToDo(const int & value)
{
    m_iYMovementToDo = value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::AddRelTimeToMove(const float & value)
{
    m_fRelTimeToMove += value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::AddYMovementPend(const float & value)
{
    m_fYMovementPend += value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::AddYMovementToDo(const int & value)
{
    m_iYMovementToDo += value;
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SetIsStoppedSnd(const bool & value)
{
    m_bIsStoppedSnd = value;
}

VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::VGUI_SubtMsgRecord(
    const client_textmessage_bundle_t * const msg_bundle,
    const void * const spec_data,
    const float & birth_time,
    const int & sh_offs_x,
    const int & sh_offs_y,
    const float & bg_opacity,
    const float & bg_bord_x,
    const float & bg_bord_y,
    const float & ext_bdr_l,
    const float & ext_bdr_r,
    const float & ext_bdr_t,
    const float & ext_bdr_b,
    const int & scr_size_x,
    const int & scr_size_y,
    const int & msg_type):
    VGUI_TextMsgRecord(msg_bundle, birth_time, sh_offs_x, sh_offs_y, bg_opacity, bg_bord_x, bg_bord_y,
                       ext_bdr_l, ext_bdr_r, ext_bdr_t, ext_bdr_b, scr_size_x, scr_size_y,
                       false /* don't apply the pos */, false /* don't use additive mode */, msg_type)
{
    if (GetMessageTypeID() == MSG_TYPE_SUBTITLE)
    {
        if (spec_data)
        {
            SetSubSoundProps(*(static_cast<const client_textmessage_sndprops_t *>(spec_data)));
            SetSpecificData(&(GetSubSoundProps()));
        } else {
            gEngfuncs.Con_DPrintf("ERROR: VGUI_SubtMsgRecord(): Msg-specific data is NULL!\n");
        }
    }

    int bdr_v_b, r_pos_x, r_pos_y;
    GetExternBordersV(r_pos_y, bdr_v_b);
    getPos(r_pos_x, r_pos_y);
    setPos(r_pos_x, scr_size_y - bdr_v_b);

    SetAffectedGroup(AFFECTED_GROUP_PREV);
    SetIsStoppedSnd(false);
    
    float life_time = GetRelHoldTime(), // For subtitles, hold time is the guaranteed precise life time they stay visible
          min_ltime = GetRelFadeInTime() + GetRelFadeOutTime(); // Hold time will be stretched during lifetime correction
    
    SetAbsDeathTime(GetAbsBirthTime() + life_time);

    if (life_time < min_ltime)
    {
        SetRelHoldTime(0.0f);
        if (min_ltime != 0.0f) min_ltime = life_time / min_ltime;
        MulRelCharInTime (min_ltime);
        MulRelFadeInTime (min_ltime);
        MulRelColorFXTime(min_ltime);
        MulRelFadeOutTime(min_ltime);
    } else
        SetRelHoldTime(life_time - min_ltime);
    
    SetRelTimeToMove(GetRelFadeInTime());
    SetYMovementToDo(- getTall());
    SetYMovementPend(0.0f);
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::SynchronizeFull(const bool & is_visible, const float & sync_time)
{
    VGUI_TextMsgRecord::SynchronizeFull(is_visible, sync_time);

    // YMovementToDo:  int, the total movement that the subtitle must crawl; '+' direction: screen bottom
    // RelTimeToMove:  float, the time remaining (in seconds) for the YMovementToDo movement to be done
    // YMovementPend:  float, the precise accumulator makes movement approx. linear (int part is applied)
    
    if (GetIsStoppedSnd()) DelNextMsgName(); // FIXME: it's a temp workaround, StopSound() must do this (NOT via turning into a zombie!)

    if (isVisibleUp())  // Do nothing if invisible
    {
        int cur_pos_x, cur_pos_y, move_decr;

        if (GetYMovementToDo() == 0)
        {
            SetRelTimeToMove(0.0f);
            SetYMovementPend(0.0f);
        } else {
            if (GetRelTimeToMove() == 0.0f)
            {
                getPos(cur_pos_x, cur_pos_y);
                setPos(cur_pos_x, cur_pos_y + GetYMovementToDo());
                SetYMovementToDo(0);
                SetYMovementPend(0.0f);
            } else {
                if (GetRelLSyncDelta() >= GetRelTimeToMove())
                {
                    getPos(cur_pos_x, cur_pos_y);
                    setPos(cur_pos_x, cur_pos_y + GetYMovementToDo());
                    SetYMovementToDo(0);
                    SetRelTimeToMove(0.0f);
                    SetYMovementPend(0.0f);
                } else {
                    AddYMovementPend(GetYMovementToDo() * GetRelLSyncDelta()/GetRelTimeToMove());   // 0-safe
                    AddRelTimeToMove(- GetRelLSyncDelta());
                    move_decr = - GetYMovementPend();   // Implicit float -> int cast for fraction truncating

                    if (move_decr != 0) // Note that move_decr has inverted sign to be used with Add*() funcs
                    {
                        AddYMovementPend(move_decr);
                        AddYMovementToDo(move_decr);
                        getPos(cur_pos_x, cur_pos_y);
                        setPos(cur_pos_x, cur_pos_y - move_decr);
                    }
                }
            }
        }

        getPos(cur_pos_x, cur_pos_y);
        GetExternBordersV(move_decr, cur_pos_x);    // move_decr: top external border, cur_pos_x: placeholder

        if ((cur_pos_y < move_decr) || GetIsStoppedSnd())
        {
            // Start fading out subtitle if it hit the top border or the sound that created it is supposed to
            // be stopped (i.e. not playing); do nothing if sub is already fading out (the purpose of min()).
            SetAbsBirthTime(min(GetAbsLSyncTime() - (GetRelFadeInTime() + GetRelHoldTime()), GetAbsBirthTime()));
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::PreAddOperation(Panel * const cur_panel)
{
    if (!RecordIsAZombie())
    {
        VGUI_SubtMsgRecord * cur_rec = dynamic_cast<VGUI_SubtMsgRecord *>(cur_panel);

        if (cur_rec)
        {
            SetRelTimeToMove(cur_rec->GetRelTimeToMove());
            AddYMovementToDo(cur_rec->GetYMovementToDo());
        }
    }
}

void VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::PreDelOperation(Panel * const cur_panel)
{
    if (!RecordIsAZombie())
    {
        VGUI_SubtMsgRecord * cur_rec = dynamic_cast<VGUI_SubtMsgRecord *>(cur_panel);

        if (cur_rec)
        {
            SetRelTimeToMove(cur_rec->GetRelFadeOutTime());
            AddYMovementToDo(cur_rec->getTall());
        }
    }
}

VGUI_MsgsBasePanel::VGUI_SubtMsgsPanel::VGUI_SubtMsgRecord::~VGUI_SubtMsgRecord()
{
    setVisible(false);
    SetSpecificData(NULL);
}
