// ======================================
//  Half-Life Update Mod: Main Header
//    written by Fograin92 & Vit_amiN
// ======================================
//  Inspired by: Half-Life: Paranoia mod
//    subtitle system written by BUzer
// ======================================

#ifndef _SUBS_MOD_MAIN_H
#define _SUBS_MOD_MAIN_H

#include <VGUI_Dar.h>
#include <VGUI_TextImage.h>
#include <VGUI_Panel.h>

#include "sm_consts.h"
#include "sm_structs.h"

class VGUI_ClientMsgPool    // Stores client message files loaded into heap
{
    protected:
        class VGUI_ClientMsgFile
        {
            private:
                const char                             * m_pClMsgFileName;
                const byte                             * m_pClMsgFileData;
                vgui::Dar<client_textmessage_bundle_t *> m_pMessagesArray;

            public:
                const char * const GetClMsgFileName() const;
                const byte * const GetClMsgFileData() const;

            protected:
                void SetClMsgFileName(const char * const);
                void SetClMsgFileData(const byte * const);

            public:
                VGUI_ClientMsgFile(const char * const);
                virtual ~VGUI_ClientMsgFile();

            public:
                virtual const client_textmessage_bundle_t * const GetClientMessage(const char * const);
        };

    private:
        static const client_textmessage_bundle_t m_stUnknMessage;
        static char                              m_szUnknMsgText[];
        static bool                              m_bCVarNamesReg;

    private:
        vgui::Dar<VGUI_ClientMsgFile *> m_pClFilesArray;
        bool                            m_bAllowUnknown;

    public:
        const bool & GetAllowUnknown() const;

    protected:
        void SetAllowUnknown(const bool &);

    public:
        VGUI_ClientMsgPool(const char * const * const, const int &);
        virtual ~VGUI_ClientMsgPool();
        virtual void Initialize();

    public:
        virtual const bool HasMsgFileLoaded(const char * const);
        virtual const client_textmessage_bundle_t * const GetClientMessage(const char * const);

    protected:
        virtual void RegisterCVarNames() const { RegisterCVarNames_S(); };
        virtual void AcquireCVarValues();

    public:
        static void RegisterCVarNames_S();
};

class VGUI_MsgsBasePanel: public VGUI_ClientMsgPool, protected vgui::Panel
{
    public:
        enum
        {
            ZOMBIE_DELETE_DELAY = 64,   // Do not lower, or else the game will CTD!
        };

    protected:
        class VGUI_TextMsgsPanel: public vgui::Panel
        {
            public:
                enum
                {
                    AFFECTED_GROUP_NONE = 0x00,
                    AFFECTED_GROUP_PREV = 0x01,
                    AFFECTED_GROUP_NEXT = 0x02,
                    AFFECTED_GROUP_ALL  = AFFECTED_GROUP_PREV | AFFECTED_GROUP_NEXT,
                };

            protected:
                class VGUI_TextMsgRecord: public vgui::Panel
                {
                    private:
                        const int                    m_iMessageTypeID;
                        const char                 * m_szMessageName,
                                                   * m_szNextMsgName,
                                                   * m_szMessageText;
                        const void                 * m_pSpecificData;
                        vgui::Dar<vgui::TextImage *> m_pMsgTextImages;
                        int                          m_iMsgEffectID,
                                                     m_iExternBorderL,
                                                     m_iExternBorderR,
                                                     m_iExternBorderT,
                                                     m_iExternBorderB,
                                                     m_iMsgBgBorderX,
                                                     m_iMsgBgBorderY,
                                                     m_iShadowOffsetX,
                                                     m_iShadowOffsetY,
                                                     m_iAffectedGroup,
                                                     m_iZombieSyncNum;
                        float                        m_fMsgPositionX,
                                                     m_fMsgPositionY,
                                                     m_fAbsLSyncTime,
                                                     m_fAbsBirthTime,
                                                     m_fAbsDeathTime,
                                                     m_fRelLSyncDelta,
                                                     m_fRelFadeInTime,
                                                     m_fRelCharInTime,
                                                     m_fRelColorFXTime,
                                                     m_fRelHoldTime,
                                                     m_fRelFadeOutTime,
                                                     m_fMsgBgOpacity;
                        byte                         m_bFXClrChannelR,
                                                     m_bFXClrChannelG,
                                                     m_bFXClrChannelB,
                                                     m_bFXClrChannelA,
                                                     m_bHTClrChannelR,
                                                     m_bHTClrChannelG,
                                                     m_bHTClrChannelB,
                                                     m_bHTClrChannelA;
                        bool                         m_bAdditiveMode;

                    public:
                        const int        & GetMessageTypeID()  const;
                        const char * const GetMessageName()    const;
                        const char * const GetNextMsgName()    const;
                        const char * const GetMessageText()    const;
                        const void * const GetSpecificData()   const;
                        const int        & GetMsgEffectID()    const;
                        const int        & GetAffectedGroup()  const;
                        const int        & GetZombieSyncNum()  const;
                        const float      & GetMsgPositionX()   const;
                        const float      & GetMsgPositionY()   const;
                        const float      & GetAbsLSyncTime()   const;
                        const float      & GetAbsBirthTime()   const;
                        const float      & GetAbsDeathTime()   const;
                        const float      & GetRelLSyncDelta()  const;
                        const float      & GetRelFadeInTime()  const;
                        const float      & GetRelCharInTime()  const;
                        const float      & GetRelColorFXTime() const;
                        const float      & GetRelHoldTime()    const;
                        const float      & GetRelFadeOutTime() const;
                        const float      & GetMsgBgOpacity()   const;
                        const byte       & GetFXClrChannelR()  const;
                        const byte       & GetFXClrChannelG()  const;
                        const byte       & GetFXClrChannelB()  const;
                        const byte       & GetFXClrChannelA()  const;
                        const byte       & GetHTClrChannelR()  const;
                        const byte       & GetHTClrChannelG()  const;
                        const byte       & GetHTClrChannelB()  const;
                        const byte       & GetHTClrChannelA()  const;
                        const bool       & GetAdditiveMode()   const;
                        void               GetExternBordersH(int &, int &) const;
                        void               GetExternBordersV(int &, int &) const;
                        void               GetMsgBgBorders(int &, int &)   const;
                        void               GetShadowOffsets(int &, int &)  const;

                    protected:
                        void SetMessageName(const char * const);
                        void SetNextMsgName(const char * const);
                        void DelNextMsgName();
                        void SetMessageText(const char * const);
                        void SetSpecificData(const void * const);
                        void SetMsgEffectID(const int &);
                        void SetAffectedGroup(const int &);
                        void SetZombieSyncNum(const int &);
                        void IncZombieSyncNum();
                        void SetMsgPositionX(const float &);
                        void SetMsgPositionY(const float &);
                        void SetAbsLSyncTime(const float &);
                        void SetAbsBirthTime(const float &);
                        void SetAbsDeathTime(const float &);
                        void SetRelLSyncDelta(const float &);
                        void SetRelFadeInTime(const float &);
                        void SetRelCharInTime(const float &);
                        void SetRelColorFXTime(const float &);
                        void SetRelHoldTime(const float &);
                        void SetRelFadeOutTime(const float &);
                        void MulRelFadeInTime(const float &);
                        void MulRelCharInTime(const float &);
                        void MulRelColorFXTime(const float &);
                        void MulRelHoldTime(const float &);
                        void MulRelFadeOutTime(const float &);
                        void SetMsgBgOpacity(const float &);
                        void SetFXClrChannelR(const byte &);
                        void SetFXClrChannelG(const byte &);
                        void SetFXClrChannelB(const byte &);
                        void SetFXClrChannelA(const byte &);
                        void SetHTClrChannelR(const byte &);
                        void SetHTClrChannelG(const byte &);
                        void SetHTClrChannelB(const byte &);
                        void SetHTClrChannelA(const byte &);
                        void SetAdditiveMode(const bool &);
                        void SetExternBordersH(const int &, const int &);
                        void SetExternBordersV(const int &, const int &);
                        void SetMsgBgBorders(const int &, const int &);
                        void SetShadowOffsets(const int &, const int &);

                    public: // TextMsgRecord is supposed to be created by TextMsgsPanel on AddVGUIMessage()
                        VGUI_TextMsgRecord(const client_textmessage_bundle_t * const, const float &,
                                           const int & = 0, const int & = 0, const float & = 0.0f,
                                           const float & = 0.0f, const float & = 0.0f, const float & = 0.0f,
                                           const float & = 0.0f, const float & = 0.0f, const float & = 0.0f,
                                           const int & = ScreenWidth, const int & = ScreenHeight,
                                           const bool & = true, /* Says to apply the msg pos via setPos */
                                           const bool & = true, /* Use a pseudo-additive text draw mode */
                                           const int & = MSG_TYPE_HUD_TEXT /* Do not change this value! */);
                        virtual ~VGUI_TextMsgRecord();
                        virtual void PreAddOperation(vgui::Panel * const);
                        virtual void PreDelOperation(vgui::Panel * const);
                        virtual void FixupLTimeDelta(const float &);
                        virtual void SynchronizeFull(const bool &, const float &);
                        virtual void SendNextMessage();
                        virtual bool RecordIsAZombie();
                        virtual void TurnIntoAZombie();

                    protected:
                        virtual void paint();
                };

            private:
                static bool m_bCVarNamesReg;

            private:
                bool                            m_bGameIsRunning;
                float                           m_fAbsLSyncTime;
                int                             m_iShadowOffsetX,
                                                m_iShadowOffsetY;
                vgui::Dar<VGUI_TextMsgRecord *> m_aLivingZombies;

            public:
                const bool  & GetGameIsRunning() const;
                const float & GetAbsLSyncTime()  const;
                const int   & GetShadowOffsetX() const;
                const int   & GetShadowOffsetY() const;

            protected:
                void SetGameIsRunning(const bool &);
                void SetAbsLSyncTime(const float &);
                void SetShadowOffsetX(const int &);
                void SetShadowOffsetY(const int &);

            public: // TextMsgsPanel must be created by MsgsBasePanel only, avoid creating it directly!
                VGUI_TextMsgsPanel(const int & = 0, const int & = 0, const int & = ScreenWidth, const int & = ScreenHeight);
                virtual ~VGUI_TextMsgsPanel();
                virtual void Initialize(const bool &);

            protected:
                virtual void SynchronizeBase(const bool &);

            public:
                virtual void SynchronizeFull(const bool &, const float &);

            protected:
                virtual bool MsgAddPermitted(const client_textmessage_bundle_t * const);
                virtual bool ShowSubElements();
                virtual void SyncSubElements(const float &);
                virtual void ClearSubElements(const bool &);

            public:
                virtual void AddVGUIMessage(const client_textmessage_bundle_t * const, const void * const, const bool &, const float &);
                virtual void ClrVGUIMessages();

            protected:
                virtual void FixupLTimeChange(const float &);
                virtual void TurnIntoAZombie(const int &);
                virtual void TurnIntoZombies();
                virtual void AckThisIsAZombie(const int &);
                virtual void CheckForAZombie(const int &);
                virtual void CheckForZombies();
                virtual void BuryDeadZombies(const int &);
                virtual void ExecPreAddOper(vgui::Panel * const);
                virtual void ExecPreDelOper(const int &);
                virtual void RegisterCVarNames() const { RegisterCVarNames_S(); };
                virtual void AcquireCVarValues();

            public:
                static void RegisterCVarNames_S();
        };

        class VGUI_SubtMsgsPanel: public VGUI_TextMsgsPanel
        {
            protected:
                class VGUI_SubtMsgRecord: public VGUI_TextMsgRecord
                {
                    private:
                        client_textmessage_sndprops_t m_pSubSoundProps;
                        float                         m_fRelTimeToMove,
                                                      m_fYMovementPend;
                        int                           m_iYMovementToDo;
                        bool                          m_bIsStoppedSnd;

                    public:
                        const float                         & GetRelTimeToMove() const;
                        const float                         & GetYMovementPend() const;
                        const int                           & GetYMovementToDo() const;
                        const bool                          & GetIsStoppedSnd()  const;
                        const client_textmessage_sndprops_t & GetSubSoundProps() const;

                    protected:
                        void SetRelTimeToMove(const float &);
                        void SetYMovementPend(const float &);
                        void SetYMovementToDo(const int &);
                        void AddRelTimeToMove(const float &);
                        void AddYMovementPend(const float &);
                        void AddYMovementToDo(const int &);
                        void SetIsStoppedSnd(const bool &);
                        void SetSubSoundProps(const client_textmessage_sndprops_t &);

                    public: // SubtMsgRecord is supposed to be created by SubtMsgsPanel on AddVGUIMessage()
                        VGUI_SubtMsgRecord(const client_textmessage_bundle_t * const, const void * const,
                                           const float &,
                                           const int & = 0, const int & = 0, const float & = 0.0f,
                                           const float & = 0.0f, const float & = 0.0f, const float & = 0.0f,
                                           const float & = 0.0f, const float & = 0.0f, const float & = 0.0f,
                                           const int & = ScreenWidth, const int & = ScreenHeight,
                                           const int & = MSG_TYPE_SUBTITLE /* Do not change this value! */);
                        virtual ~VGUI_SubtMsgRecord();
                        virtual void PreAddOperation(vgui::Panel * const);
                        virtual void PreDelOperation(vgui::Panel * const);
                        virtual void SynchronizeFull(const bool &, const float &);
                };

            private:
                static bool m_bCVarNamesReg;

            private:
                float m_fExternBorderL,
                      m_fExternBorderR,
                      m_fExternBorderT,
                      m_fExternBorderB,
                      m_fBackgrOpacity,
                      m_fBackgrBorderX,
                      m_fBackgrBorderY;

            public:
                const float & GetExternBorderL() const;
                const float & GetExternBorderR() const;
                const float & GetExternBorderT() const;
                const float & GetExternBorderB() const;
                const float & GetBackgrOpacity() const;
                const float & GetBackgrBorderX() const;
                const float & GetBackgrBorderY() const;

            protected:
                void SetExternBorderL(const float &);
                void SetExternBorderR(const float &);
                void SetExternBorderT(const float &);
                void SetExternBorderB(const float &);
                void SetBackgrOpacity(const float &);
                void SetBackgrBorderX(const float &);
                void SetBackgrBorderY(const float &);

            public: // SubtMsgsPanel must be created by MsgsBasePanel only, avoid creating it directly!
                VGUI_SubtMsgsPanel(const int & = 0, const int & = 0, const int & = ScreenWidth, const int & = ScreenHeight);
                virtual ~VGUI_SubtMsgsPanel();

            protected:
                virtual bool MsgAddPermitted(const client_textmessage_bundle_t * const);
                virtual bool ShowSubElements();

            public:
                virtual void AddVGUIMessage(const client_textmessage_bundle_t * const, const void * const, const bool &, const float &);

            protected:
                virtual void RegisterCVarNames() const { RegisterCVarNames_S(); };
                virtual void AcquireCVarValues();

            public:
                static void RegisterCVarNames_S();
        };

    private:
        VGUI_TextMsgsPanel * const m_pTextMsgsPanel;
        VGUI_SubtMsgsPanel * const m_pSubtMsgsPanel;

    public:
        VGUI_TextMsgsPanel * const GetTextMsgsPanel();
        VGUI_SubtMsgsPanel * const GetSubtMsgsPanel();

    public:
        VGUI_MsgsBasePanel(vgui::Panel * const, const char * const * const, const int &, const int & = 0, const int & = 0, const int & = ScreenWidth, const int & = ScreenHeight);
        virtual ~VGUI_MsgsBasePanel();
        virtual void Initialize();

    protected:
        virtual void SynchronizeBase();

    public:
        virtual void SynchronizeFull();

    protected:
        virtual bool ShowSubElements();
        virtual void InitSubElements();
        virtual void SyncSubElements();

    public:
        virtual void AddTextMessage(const int &, const client_textmessage_bundle_t * const, const void * const, const float &);
        virtual void ClrTextMessages();

    protected:
        virtual void RegisterCVarNames() const { RegisterCVarNames_S(); };
        virtual void AcquireCVarValues();
        virtual void paintTraverse(bool);

    public:
        static void RegisterCVarNames_S();
        static void RegisterCVarsTree_S();
};

extern void SM_RegisterAllConVars();
extern const client_textmessage_bundle_t * const SM_ConvMessageToBundle(const client_textmessage_t * const);

#endif // _SUBS_MOD_MAIN_H
