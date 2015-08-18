//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "port.h"
#include "sm_consts.h"
#include "sm_util_txt.h"

#ifndef DISABLE_ENGINE_HOOKS

const char * const SM_SubstEOFsInMemory(char * const text_ptr)
{
	for (char * cur_char = text_ptr; !SM_CurrentlyAtEOF(cur_char); cur_char++)
	{
		if (cur_char[0] == STR_NETW_EOF_CHAR) cur_char[0] = STR_SUBS_EOF_CHAR;
	}
	return text_ptr;
}

const char * const SM_SubstEOFsInBuffer(const char * const text_ptr)
{
	static char textBuff[TXT_MSG_MAX_TEXT_LEN];

	strncpy(textBuff, text_ptr, TXT_MSG_MAX_TEXT_LEN - 1);	// Copy data to the static buffer
	textBuff[TXT_MSG_MAX_TEXT_LEN - 1] = '\0';	// Ensure that this buffer is null-terminated

	return SM_SubstEOFsInMemory(textBuff);
}

#endif	// DISABLE_ENGINE_HOOKS

const bool SM_CurrentlyAtEOF(const char * const text_ptr)
{
	return (text_ptr[0] == '\0');
}

const bool SM_CurrentlyAtEOL(const char * const text_ptr)
{
	return (text_ptr[0] == '\x0D') || (text_ptr[0] == '\x0A');
}

const bool SM_CurrentlyAtWSP(const char * const text_ptr)
{
	return (text_ptr[0] == ' ') || (text_ptr[0] == '\t');
}

const bool SM_CurrentlyAtDLM(const char * const text_ptr)
{
	return SM_CurrentlyAtWSP(text_ptr) || SM_CurrentlyAtEOL(text_ptr);
}

const bool SM_CurrentlyAtToken(const char * const pCmpText, const char * const pToken)
{
	int token_len = strlen(pToken);
	const char * token_end = pCmpText + token_len;	// If strnicmp will pass, token_end is the token end in pCmpText
	return !strnicmp(pCmpText, pToken, token_len) && (SM_CurrentlyAtDLM(token_end) || SM_CurrentlyAtEOF(token_end));
}

const bool SM_FindCurrentEOL(const char * & text_ptr)
{
	while (!(SM_CurrentlyAtEOL(text_ptr) || SM_CurrentlyAtEOF(text_ptr)))
	{
		text_ptr++;
	}
	return !SM_CurrentlyAtEOF(text_ptr);
}

const bool SM_FindCurrentWSP(const char * & text_ptr)
{
	while (!(SM_CurrentlyAtWSP(text_ptr) || SM_CurrentlyAtEOF(text_ptr)))
	{
		text_ptr++;
	}
	return !SM_CurrentlyAtEOF(text_ptr);
}

const bool SM_FindCurrentDLM(const char * & text_ptr)
{
	while (!(SM_CurrentlyAtDLM(text_ptr) || SM_CurrentlyAtEOF(text_ptr)))
	{
		text_ptr++;
	}
	return !SM_CurrentlyAtEOF(text_ptr);
}

const bool SM_SkipCurrentEOL(const char * & text_ptr)
{
	bool result = true;	// No redundant calls needed
	if (text_ptr[0] == '\x0D')
	{
		text_ptr++;
		if (text_ptr[0] == '\x0A') text_ptr++;
	} else {
		if (text_ptr[0] == '\x0A') text_ptr++;
		else result = false;	// No EOL found here
	}
	return result;
}

const bool SM_SkipCurrentWSP(const char * & text_ptr)
{
	const bool result = SM_CurrentlyAtWSP(text_ptr);
	if (result) text_ptr++;	// Since any WSP len = 1
	return result;
}

const bool SM_SkipCurrentDLM(const char * & text_ptr)
{
	// It will return instantly if WSP is skipped (it won't skip EOL)
	return SM_SkipCurrentWSP(text_ptr) || SM_SkipCurrentEOL(text_ptr);
}

const bool SM_SkipEOLSetFrom(const char * & text_ptr)
{
	const bool result = SM_SkipCurrentEOL(text_ptr);
	if (result) while (SM_SkipCurrentEOL(text_ptr));
	return result;
}

const bool SM_SkipWSPSetFrom(const char * & text_ptr)
{
	const bool result = SM_SkipCurrentWSP(text_ptr);
	if (result) while (SM_SkipCurrentWSP(text_ptr));
	return result;
}

const bool SM_SkipDLMSetFrom(const char * & text_ptr)
{
	const bool result = SM_SkipCurrentDLM(text_ptr);
	if (result) while (SM_SkipCurrentDLM(text_ptr));
	return result;
}

const bool SM_FindCurrentEOL(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_FindCurrentEOL(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_FindCurrentWSP(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_FindCurrentWSP(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_FindCurrentDLM(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_FindCurrentDLM(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipCurrentEOL(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipCurrentEOL(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipCurrentWSP(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipCurrentWSP(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipCurrentDLM(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipCurrentDLM(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipEOLSetFrom(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipEOLSetFrom(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipWSPSetFrom(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipWSPSetFrom(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_SkipDLMSetFrom(char * & text_ptr)
{
	const char * text_const = text_ptr;
	bool result = SM_SkipDLMSetFrom(text_const);
	text_ptr = const_cast<char * &>(text_const);
	return result;
}

const bool SM_ParseFltArray(char * const pPrevToken, const int & iMaxLength, float * const pFillArray, const int & iElemCount)
{
	const char * pCurrPtr = pPrevToken;
	bool result = !SM_CurrentlyAtEOF(pCurrPtr);
	char char_backup = pPrevToken[iMaxLength];
	pPrevToken[iMaxLength] = '\0';

	for (int elem_index = 0; result && elem_index < iElemCount; elem_index++)
	{
		result = SM_FindCurrentWSP(pCurrPtr) && SM_SkipWSPSetFrom(pCurrPtr) && !SM_CurrentlyAtEOF(pCurrPtr);
		if (result) pFillArray[elem_index] = atof(pCurrPtr);	// Stop parsing values if got the buffer end
	}

	pPrevToken[iMaxLength] = char_backup;
	return result;
}

const bool SM_ParseIntArray(char * const pPrevToken, const int & iMaxLength, int * const pFillArray, const int & iElemCount)
{
	const char * pCurrPtr = pPrevToken;
	bool result = !SM_CurrentlyAtEOF(pCurrPtr);
	char char_backup = pPrevToken[iMaxLength];
	pPrevToken[iMaxLength] = '\0';

	for (int elem_index = 0; result && elem_index < iElemCount; elem_index++)
	{
		result = SM_FindCurrentWSP(pCurrPtr) && SM_SkipWSPSetFrom(pCurrPtr) && !SM_CurrentlyAtEOF(pCurrPtr);
		if (result) pFillArray[elem_index] = atoi(pCurrPtr);	// Stop parsing values if got the buffer end
	}

	pPrevToken[iMaxLength] = char_backup;
	return result;
}

const bool SM_ParseStrParam(char * const pPrevToken, const int & iMaxLength, const char * & pParsedStr)
{
	const char * pCurrPtr = pPrevToken;
	char char_back = pPrevToken[iMaxLength];
	pPrevToken[iMaxLength] = '\0';

	bool result = SM_FindCurrentWSP(pCurrPtr) && SM_SkipWSPSetFrom(pCurrPtr) && !SM_CurrentlyAtEOF(pCurrPtr);
	if (result) pParsedStr = pCurrPtr; else pPrevToken[iMaxLength] = char_back;	// Leave EOF only on success

	return result;
}

const bool SM_ParseStrParam(char * const pPrevToken, const int & iMaxLength, char * & pParsedStr)
{
	const char * text_const = pParsedStr;
	bool result = SM_ParseStrParam(pPrevToken, iMaxLength, text_const);
	pParsedStr = const_cast<char * &>(text_const);	// Casting is safe
	return result;
}
