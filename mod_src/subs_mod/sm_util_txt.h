//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef _SUBS_MOD_UTILS_TXT_H
#define _SUBS_MOD_UTILS_TXT_H

#ifndef DISABLE_ENGINE_HOOKS
	extern const char * const SM_SubstEOFsInMemory(      char * const szString);
	extern const char * const SM_SubstEOFsInBuffer(const char * const szString);
#endif // DISABLE_ENGINE_HOOKS

extern const bool SM_CurrentlyAtEOF(const char * const);
extern const bool SM_CurrentlyAtEOL(const char * const);
extern const bool SM_CurrentlyAtWSP(const char * const);
extern const bool SM_CurrentlyAtDLM(const char * const);

extern const bool SM_CurrentlyAtToken(const char * const, const char * const);

extern const bool SM_FindCurrentEOL(const char * &);
extern const bool SM_FindCurrentEOL(      char * &);
extern const bool SM_FindCurrentWSP(const char * &);
extern const bool SM_FindCurrentWSP(      char * &);
extern const bool SM_FindCurrentDLM(const char * &);
extern const bool SM_FindCurrentDLM(      char * &);

extern const bool SM_SkipCurrentEOL(const char * &);
extern const bool SM_SkipCurrentEOL(      char * &);
extern const bool SM_SkipCurrentWSP(const char * &);
extern const bool SM_SkipCurrentWSP(      char * &);
extern const bool SM_SkipCurrentDLM(const char * &);
extern const bool SM_SkipCurrentDLM(      char * &);

extern const bool SM_SkipEOLSetFrom(const char * &);
extern const bool SM_SkipEOLSetFrom(      char * &);
extern const bool SM_SkipWSPSetFrom(const char * &);
extern const bool SM_SkipWSPSetFrom(      char * &);
extern const bool SM_SkipDLMSetFrom(const char * &);
extern const bool SM_SkipDLMSetFrom(      char * &);

extern const bool SM_ParseFltArray(char * const, const int &, float * const, const int &);
extern const bool SM_ParseIntArray(char * const, const int &, int   * const, const int &);
extern const bool SM_ParseStrParam(char * const, const int &, const char * &);
extern const bool SM_ParseStrParam(char * const, const int &,       char * &);

#endif // _SUBS_MOD_UTILS_TXT_H
