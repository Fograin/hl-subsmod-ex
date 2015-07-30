//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "vgui_SchemeManager.h"
#include "cl_util.h"

#include <string.h>


void Scheme_Init()
{
	CVAR_CREATE( "bitmapfonts", "1", 0 );
}



//-----------------------------------------------------------------------------
// Purpose: Font class with proper (unsigned) handling of character code
//-----------------------------------------------------------------------------
class CTargaFont: public vgui::Font
{
	public:
		CTargaFont(
			const char * const	fontName,
			void * const		fontRaw,
			const int &			fileSize,
			const int &			tall,
			const int &			width,
			const float &		rotation,
			const int &			weight,
			const bool &		italic,
			const bool &		underline,
			const bool &		strikeout,
			const bool &		symbolic
		):
			vgui::Font(
				fontName,
				fontRaw,
				fileSize,
				tall,
				width,
				rotation,
				weight,
				italic,
				underline,
				strikeout,
				symbolic
			)
		{};

		CTargaFont(
			const char * const	fontName,
			const int &			tall,
			const int &			width,
			const float &		rotation,
			const int &			weight,
			const bool &		italic,
			const bool &		underline,
			const bool &		strikeout,
			const bool &		symbolic
		):
			vgui::Font(
				fontName,
				tall,
				width,
				rotation,
				weight,
				italic,
				underline,
				strikeout,
				symbolic
			)
		{};

		virtual void getCharRGBA(
			int		charCode,
			int		rgbaX,
			int		rgbaY,
			int		rgbaWide,
			int		rgbaTall,
			uchar *	rgba
		)
		{
			return vgui::Font::getCharRGBA(
				static_cast<unsigned char>(static_cast<char>(charCode)),
				rgbaX,
				rgbaY,
				rgbaWide,
				rgbaTall,
				rgba
			);
		};

		virtual void getCharABCwide(
			int		charCode,
			int &	charPref,
			int &	charWidth,
			int &	charPost
		)
		{
			return vgui::Font::getCharABCwide(
				static_cast<unsigned char>(static_cast<char>(charCode)),
				charPref,
				charWidth,
				charPost
			);
		};
};

//-----------------------------------------------------------------------------
// Purpose: Scheme managers data container
//-----------------------------------------------------------------------------
class CSchemeManager::CScheme
{
public:
	enum { 
		SCHEME_NAME_LENGTH = 32,
		FONT_NAME_LENGTH = 48,
		FONT_FILENAME_LENGTH = 64,
		FONT_FLAG_NONE_FLAGS = 0x00,
		FONT_FLAG_GENERATED = 0x01,
		FONT_FLAG_RES_DEPEND = 0x02,
		FONT_FLAG_SUBSTITUTE = 0x04,
		FONT_FLAG_GEN_R_IND = FONT_FLAG_GENERATED | FONT_FLAG_NONE_FLAGS,
		FONT_FLAG_GEN_R_DEP = FONT_FLAG_GENERATED | FONT_FLAG_RES_DEPEND,
		FONT_FLAG_GEN_SUBST = FONT_FLAG_GENERATED | FONT_FLAG_SUBSTITUTE,
	};

	// name
	char schemeName[SCHEME_NAME_LENGTH];

	int resIndex;	// -1 if scheme has the generated res-independent font
	int fontFlags;

	// font
	char fontName[FONT_NAME_LENGTH];

	int fontSize;
	int charWidth;
	float charRot;
	int fontWeight;
	int fontStyle;

	vgui::Font *font;
	int ownFontPointer; // true if the font is ours to delete

	// scheme
	byte fgColor[4];
	byte bgColor[4];
	byte armedFgColor[4];
	byte armedBgColor[4]; 
	byte mousedownFgColor[4];
	byte mousedownBgColor[4];
	byte borderColor[4];

	// construction/destruction
	CScheme();
	~CScheme();
};

CSchemeManager::CScheme::CScheme()
{
	schemeName[0] = '\0';
	resIndex = 0;
	fontFlags = FONT_FLAG_NONE_FLAGS;
	fontName[0] = '\0';
	fontSize = 0;
	charWidth = 0;
	charRot = 0.0f;
	fontWeight = 0;
	fontStyle = FONT_STYLE_NOT_SET;
	font = NULL;
	ownFontPointer = false;
}

CSchemeManager::CScheme::~CScheme()
{
	// only delete our font pointer if we own it
	if ( ownFontPointer )
	{
		delete font;
	}
}

//-----------------------------------------------------------------------------
// Purpose: resolution information
//-----------------------------------------------------------------------------
extern const int g_ResArray[] =
{
	320,
	400,
	512,
	640,
	800,
	1024,
	1152,
	1280,
	1600
};
extern const int g_NumReses = sizeof(g_ResArray) / sizeof(int);

static char *LoadFileByResolutionIndex( const char *filePrefix, int resNum, const char *filePostfix )
{
	for ( byte *pFile = NULL; resNum >= 0; resNum-- )
	{
		// try open the file
		static char fname[256];
		sprintf( fname, "%s%d%s", filePrefix, g_ResArray[resNum], filePostfix );
		pFile = gEngfuncs.COM_LoadFile( fname, 5, NULL );
		if (pFile) return reinterpret_cast<char *>(pFile);
	};

	return NULL;
}

static int GetAppropriateResolutionIndex( const int & screenWidth )
{
	int resIndex = g_NumReses - 1;
	while ( resIndex > 0 && g_ResArray[resIndex] > screenWidth ) resIndex--;
	return resIndex;
}

static void ParseRGBIAFromRGBAString( byte colorArray[4], const char *colorVector )
{
	int r = 0, g = 0, b = 0, a = 0;
	sscanf( colorVector, "%d %d %d %d", &r, &g, &b, &a );
	colorArray[0] = r;
	colorArray[1] = g;
	colorArray[2] = b;
	colorArray[3] = 255 - a;
}

//-----------------------------------------------------------------------------
// Purpose: setting up default colors (alpha channel is inverted)
//-----------------------------------------------------------------------------
const byte CSchemeManager::defFgColor[4]			= { 255, 255, 255,   0 };
const byte CSchemeManager::defBgColor[4]			= {   0,   0,   0, 255 };
const byte CSchemeManager::defArmedFgColor[4]		= { 255, 255, 255,   0 };
const byte CSchemeManager::defArmedBgColor[4]		= {   0,   0,   0, 255 };
const byte CSchemeManager::defMousedownFgColor[4]	= { 255, 255, 255,   0 };
const byte CSchemeManager::defMousedownBgColor[4]	= {   0,   0,   0, 255 };
const byte CSchemeManager::defBorderColor[4]		= {   0,   0,   0, 255 };

//-----------------------------------------------------------------------------
// Purpose: initializes the scheme manager
//			loading the scheme files for the current resolution
// Input  : xRes - 
//			yRes - dimensions of output window
//-----------------------------------------------------------------------------
CSchemeManager::CSchemeManager( int xRes, int yRes )
{
	// basic setup
	m_pSchemeList.ensureCapacity( 64 );
	m_xResIndex = GetAppropriateResolutionIndex( xRes );

	// find the closest matching scheme file to our resolution
	static char token[1024];
	char *pFile = LoadFileByResolutionIndex( "", m_xResIndex, "_textscheme.txt" );

	char *pFileStart = pFile;
	
	//
	// Read the scheme descriptions from the text file, into a temporary buffer
	// format is simply:
	// <paramName name> = <paramValue>
	//
	// a <paramName name> of "SchemeName" signals a new scheme is being described
	//

	CScheme cur_scheme;
	bool cur_exists = false;

	if ( !pFile )
	{
		gEngfuncs.Con_DPrintf( "Unable to find *_textscheme.txt\n");
	} else {

		// record what has been entered so we can create defaults from the different values
		bool hasFgColor = false, hasBgColor = false, hasArmedFgColor = false, hasArmedBgColor = false, hasMouseDownFgColor = false, hasMouseDownBgColor = false, hasBorderColor = false;

		pFile = gEngfuncs.COM_ParseFile( pFile, token );
		while ( strlen(token) > 0 )
		{

			// get the paramName name
			static const int tokenSize = 64;
			static char paramName[tokenSize], paramValue[tokenSize];

			strncpy( paramName, token, tokenSize - 1 );
			paramName[tokenSize - 1] = '\0'; // ensure null termination

			// get the '=' character
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			if ( stricmp( token, "=" ) )
			{
				if ( m_pSchemeList.getCount() == 0 )
				{
					gEngfuncs.Con_Printf( "error parsing font scheme text file at file start - expected '=', found '%s'\n", token );
				}
				else
				{
					gEngfuncs.Con_Printf( "error parsing font scheme text file at scheme '%s' - expected '=', found '%s'\n", cur_scheme.schemeName, token );
				}
				break;
			}

			// get paramValue
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			strncpy( paramValue, token, tokenSize - 1 );
			paramValue[tokenSize - 1] = '\0'; // ensure null termination

			// is this a new scheme?
			if ( !stricmp(paramName, "SchemeName") )
			{
				// setup the defaults for the current scheme before reinitializing buffer
				if ( cur_exists )
				{
					if ( !hasFgColor )
					{
						memcpy( cur_scheme.fgColor, defFgColor, sizeof(cur_scheme.fgColor) );
					}
					if ( !hasBgColor )
					{
						memcpy( cur_scheme.bgColor, defBgColor, sizeof(cur_scheme.bgColor) );
					}
					if ( !hasArmedFgColor )
					{
						memcpy( cur_scheme.armedFgColor, defArmedFgColor, sizeof(cur_scheme.armedFgColor) );
					}
					if ( !hasArmedBgColor )
					{
						memcpy( cur_scheme.armedBgColor, defArmedBgColor, sizeof(cur_scheme.armedBgColor) );
					}
					if ( !hasMouseDownFgColor )
					{
						memcpy( cur_scheme.mousedownFgColor, defMousedownFgColor, sizeof(cur_scheme.mousedownFgColor) );
					}
					if ( !hasMouseDownBgColor )
					{
						memcpy( cur_scheme.mousedownBgColor, defMousedownBgColor, sizeof(cur_scheme.mousedownBgColor) );
					}
					if ( !hasBorderColor )
					{
						memcpy( cur_scheme.borderColor, defBorderColor, sizeof(cur_scheme.borderColor) );
					}


					// font size
					if ( !cur_scheme.fontSize )
					{
						cur_scheme.fontSize = 17;
					}
					if ( !cur_scheme.fontName[0] )
					{
						strcpy( cur_scheme.fontName, "Arial" );
					}

					// creating the new scheme this way will prevent array from having two equal schemes stored in it
					getSchemeHandle(
						cur_scheme.schemeName,
						false,
						false,
						cur_scheme.fontName,
						cur_scheme.fontSize,
						cur_scheme.charWidth,
						cur_scheme.charRot,
						cur_scheme.fontWeight,
						cur_scheme.fontStyle,
						cur_scheme.fgColor,
						cur_scheme.bgColor,
						cur_scheme.armedFgColor,
						cur_scheme.armedBgColor,
						cur_scheme.mousedownFgColor,
						cur_scheme.mousedownBgColor,
						cur_scheme.borderColor
					);
				}

				cur_exists = true;
				cur_scheme.fontName[0] = '\0';
				cur_scheme.fontSize = 0;
				cur_scheme.charWidth = 0;
				cur_scheme.charRot = 0.0f;
				cur_scheme.fontWeight = 0;
				cur_scheme.fontStyle = FONT_STYLE_NOT_SET;

				hasFgColor = hasBgColor = hasArmedFgColor = hasArmedBgColor = hasMouseDownFgColor = hasMouseDownBgColor = hasBorderColor = false;

				strncpy( cur_scheme.schemeName, paramValue, CScheme::SCHEME_NAME_LENGTH - 1 );
				cur_scheme.schemeName[CScheme::SCHEME_NAME_LENGTH - 1] = '\0'; // ensure null termination of string
			}

			if ( !cur_exists )
			{
				gEngfuncs.Con_Printf( "font scheme text file MUST start with a 'SchemeName'\n");
				break;
			}

			// pull the data out into the scheme
			if ( !stricmp(paramName, "FontName") )
			{
				strncpy( cur_scheme.fontName, paramValue, CScheme::FONT_NAME_LENGTH - 1 );
				cur_scheme.fontName[CScheme::FONT_NAME_LENGTH - 1] = '\0';
			}
			else if ( !stricmp(paramName, "FontSize") )
			{
				cur_scheme.fontSize = atoi( paramValue );
			}
			else if ( !stricmp(paramName, "FontWeight") )
			{
				cur_scheme.fontWeight = atoi( paramValue );
			}
			else if ( !stricmp(paramName, "FgColor") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.fgColor, paramValue );
				hasFgColor = true;
			}
			else if ( !stricmp(paramName, "BgColor") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.bgColor, paramValue );
				hasBgColor = true;
			}
			else if ( !stricmp(paramName, "FgColorArmed") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.armedFgColor, paramValue );
				hasArmedFgColor = true;
			}	
			else if ( !stricmp(paramName, "BgColorArmed") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.armedBgColor, paramValue );
				hasArmedBgColor = true;
			}
			else if ( !stricmp(paramName, "FgColorMousedown") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.mousedownFgColor, paramValue );
				hasMouseDownFgColor = true;
			}
			else if ( !stricmp(paramName, "BgColorMousedown") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.mousedownBgColor, paramValue );
				hasMouseDownBgColor = true;
			}
			else if ( !stricmp(paramName, "BorderColor") )
			{
				ParseRGBIAFromRGBAString( cur_scheme.borderColor, paramValue );
				hasBorderColor = true;
			}

			// get the new token last, so we know if the loop needs to be continued or not
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
		}

		// free the file
		gEngfuncs.COM_FreeFile( pFileStart );
	}

	// make sure we have at least 1 valid font
	if ( m_pSchemeList.getCount() == 0 )
	{
		getSchemeHandle("Default Scheme");
	}
}

//-----------------------------------------------------------------------------
// Purpose: frees all the memory used by the scheme manager
//-----------------------------------------------------------------------------
CSchemeManager::~CSchemeManager()
{
	for (int index = m_pSchemeList.getCount() - 1; index >= 0; index--)
	{
		delete m_pSchemeList[index];
		m_pSchemeList.setElementAt(NULL, index);
		m_pSchemeList.removeElementAt(index);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Finds a scheme in the list, by name
// Input  : const char * const schemeName - string name of the scheme;
//          other input parameters are optional, for detailed info see
//          comments in the header file
// Output : SchemeHandle_t - handle to the scheme
//-----------------------------------------------------------------------------
SchemeHandle_t CSchemeManager::getSchemeHandle(
	const char * const	schemeName,
	const bool &		useCurrRes,
	const bool &		isResDepend,
	const char * const	fontName,
	const int &			fontSize,
	const int &			charWidth,
	const float &		charRotation,
	const int &			fontWeight,
	const int &			fontStyle,
	const byte * const	fgColor,
	const byte * const	bgColor,
	const byte * const	armedFgColor,
	const byte * const	armedBgColor,
	const byte * const	mousedownFgColor,
	const byte * const	mousedownBgColor,
	const byte * const	borderColor
)
{
	bool load_bitmap = (CVAR_GET_FLOAT( "bitmapfonts" ) != 0.0f);

	int fnt_flags = (load_bitmap ? CScheme::FONT_FLAG_NONE_FLAGS : CScheme::FONT_FLAG_GENERATED) |
					(isResDepend ? CScheme::FONT_FLAG_RES_DEPEND : CScheme::FONT_FLAG_NONE_FLAGS);

	int res_index = (fnt_flags & CScheme::FONT_FLAG_GEN_R_DEP) == CScheme::FONT_FLAG_GEN_R_IND ?
					-1 : (useCurrRes ? GetAppropriateResolutionIndex(ScreenWidth) : m_xResIndex);

	// iterate through the list
	for (int index = m_pSchemeList.getCount() - 1; index >= 0; index--)
	{
		if (
			!stricmp(schemeName, m_pSchemeList[index]->schemeName) &&
			res_index == m_pSchemeList[index]->resIndex &&
			(
				fnt_flags == m_pSchemeList[index]->fontFlags ||
				(
					(fnt_flags & CScheme::FONT_FLAG_GENERATED) == CScheme::FONT_FLAG_NONE_FLAGS &&
					(m_pSchemeList[index]->fontFlags & CScheme::FONT_FLAG_GEN_SUBST) == CScheme::FONT_FLAG_GEN_SUBST &&
					(fnt_flags & CScheme::FONT_FLAG_RES_DEPEND) == (m_pSchemeList[index]->fontFlags & CScheme::FONT_FLAG_RES_DEPEND)
				)
			)
		)
		{
			// schemes have the same names and X resolutions, and their flags are the same or the required .TGA font has an already generated substitute with the same 'resolution dependency' state
			return index;
		}
	}

	return	loadNewScheme(
				schemeName,
				res_index,
				fnt_flags,
				fontName,
				fontSize,
				charWidth,
				charRotation,
				fontWeight,
				fontStyle,
				fgColor,
				bgColor,
				armedFgColor,
				armedBgColor,
				mousedownFgColor,
				mousedownBgColor,
				borderColor
			);
}

SchemeHandle_t CSchemeManager::loadNewScheme(
	const char * const	schemeName,
	const int &			resIndex,
	const int &			fontFlags,
	const char * const	fontName,
	const int &			fontSize,
	const int &			charWidth,
	const float &		charRotation,
	const int &			fontWeight,
	const int &			fontStyle,
	const byte * const	fgColor,
	const byte * const	bgColor,
	const byte * const	armedFgColor,
	const byte * const	armedBgColor,
	const byte * const	mousedownFgColor,
	const byte * const	mousedownBgColor,
	const byte * const	borderColor
)
{
	CScheme * curr_schm = new CScheme();
	
	strncpy( curr_schm->schemeName, schemeName, CScheme::SCHEME_NAME_LENGTH - 1 );
	curr_schm->schemeName[CScheme::SCHEME_NAME_LENGTH - 1] = '\0';
	
	curr_schm->resIndex = resIndex;
	curr_schm->fontFlags = fontFlags;
	
	strncpy( curr_schm->fontName, fontName, CScheme::FONT_NAME_LENGTH - 1 );
	curr_schm->fontName[CScheme::FONT_NAME_LENGTH - 1] = '\0';
	
	curr_schm->fontSize = fontSize;
	curr_schm->charWidth = charWidth;
	curr_schm->charRot = charRotation;
	curr_schm->fontWeight = fontWeight;
	curr_schm->fontStyle = fontStyle;
	
	if ((curr_schm->fontFlags & CScheme::FONT_FLAG_RES_DEPEND) == CScheme::FONT_FLAG_RES_DEPEND)
	{
		float fontScale = g_ResArray[curr_schm->resIndex] / BASE_XRES;	// BASE_XRES is float
		curr_schm->fontSize *= fontScale;
		curr_schm->charWidth *= fontScale;
	}
	
	memcpy(curr_schm->fgColor,			fgColor,			sizeof(curr_schm->fgColor));
	memcpy(curr_schm->bgColor,			bgColor,			sizeof(curr_schm->bgColor));
	memcpy(curr_schm->armedFgColor,		armedFgColor,		sizeof(curr_schm->armedFgColor));
	memcpy(curr_schm->armedBgColor,		armedBgColor,		sizeof(curr_schm->armedBgColor));
	memcpy(curr_schm->mousedownFgColor,	mousedownFgColor,	sizeof(curr_schm->mousedownFgColor));
	memcpy(curr_schm->mousedownBgColor,	mousedownBgColor,	sizeof(curr_schm->mousedownBgColor));
	memcpy(curr_schm->borderColor,		borderColor,		sizeof(curr_schm->borderColor));
	
	static char fontFilename[CScheme::FONT_FILENAME_LENGTH];
	void * pFontData = NULL;
	int fontFileLength = -1;
	
	if ((curr_schm->fontFlags & CScheme::FONT_FLAG_GENERATED) == CScheme::FONT_FLAG_NONE_FLAGS)
	{
		for (int leq_rID = curr_schm->resIndex; !pFontData && leq_rID >= 0; leq_rID--)
		{
			sprintf(fontFilename, "gfx\\vgui\\fonts\\%d_%s.tga", g_ResArray[leq_rID], schemeName);
			pFontData = gEngfuncs.COM_LoadFile(fontFilename, 5, &fontFileLength);
			if (!pFontData) gEngfuncs.Con_DPrintf("Missing stored bitmap font: \"%s\"\n", fontFilename);
		}
		
		if (!pFontData)
		{
			gEngfuncs.Con_Printf("Failed to find LEQ stored bitmap font for scheme \"%s\"\n", schemeName);
			for (int gtr_rID = curr_schm->resIndex + 1; !pFontData && gtr_rID < g_NumReses; gtr_rID++)
			{
				sprintf(fontFilename, "gfx\\vgui\\fonts\\%d_%s.tga", g_ResArray[gtr_rID], schemeName);
				pFontData = gEngfuncs.COM_LoadFile(fontFilename, 5, &fontFileLength);
				if (!pFontData) gEngfuncs.Con_DPrintf("Missing stored bitmap font: \"%s\"\n", fontFilename);
			}
			
			if (!pFontData)
			{
				gEngfuncs.Con_Printf("Failed to find GTR stored bitmap font for scheme \"%s\"\n", schemeName);
				curr_schm->fontFlags |= CScheme::FONT_FLAG_GEN_SUBST;
				if ((curr_schm->fontFlags & CScheme::FONT_FLAG_RES_DEPEND) == CScheme::FONT_FLAG_NONE_FLAGS)
				{
					curr_schm->resIndex = -1;	// the font substitute to be generated is resolution independent
				}
			}
		}
	}
	
	// check if required font already exists (loaded as .TGA or generated from TrueType)
	for (int index = m_pSchemeList.getCount() - 1; !curr_schm->font && index >= 0; index--)
	{
		if ((curr_schm->fontFlags & CScheme::FONT_FLAG_GENERATED) == CScheme::FONT_FLAG_NONE_FLAGS ?
			(
				(m_pSchemeList[index]->fontFlags & CScheme::FONT_FLAG_GENERATED) == CScheme::FONT_FLAG_NONE_FLAGS &&
				curr_schm->resIndex == m_pSchemeList[index]->resIndex &&
				!stricmp(curr_schm->schemeName, m_pSchemeList[index]->schemeName)
			) : (
				(m_pSchemeList[index]->fontFlags & CScheme::FONT_FLAG_GENERATED) == CScheme::FONT_FLAG_GENERATED &&
				!stricmp(curr_schm->fontName, m_pSchemeList[index]->fontName) &&
				curr_schm->fontSize == m_pSchemeList[index]->fontSize &&
				curr_schm->charWidth == m_pSchemeList[index]->charWidth &&
				curr_schm->charRot == m_pSchemeList[index]->charRot &&
				curr_schm->fontWeight == m_pSchemeList[index]->fontWeight &&
				curr_schm->fontStyle == m_pSchemeList[index]->fontStyle
			)
		)
		{
			// copy the found font pointer and leave a new scheme marked as not owning it
			curr_schm->font = m_pSchemeList[index]->font;
		}
	}
	
	// if we haven't found the font already, load it ourselves
	if (!curr_schm->font)
	{
		bool isItalic    = (curr_schm->fontStyle & FONT_STYLE_ITALIC)    == FONT_STYLE_ITALIC,
			 isUnderline = (curr_schm->fontStyle & FONT_STYLE_UNDERLINE) == FONT_STYLE_UNDERLINE,
			 isStrikeout = (curr_schm->fontStyle & FONT_STYLE_STRIKEOUT) == FONT_STYLE_STRIKEOUT,
			 isSymbolic  = (curr_schm->fontStyle & FONT_STYLE_SYMBOLIC)  == FONT_STYLE_SYMBOLIC;
		
		curr_schm->ownFontPointer = true;
		
		if (pFontData)
		{
			gEngfuncs.Con_Printf("Loading stored bitmap font \"%s\"\n", fontFilename);
			curr_schm->font = new CTargaFont(
									  fontName, pFontData, fontFileLength, curr_schm->fontSize,
									  charWidth, charRotation, fontWeight, isItalic, isUnderline,
									  isStrikeout, isSymbolic
								  );
		} else {
			gEngfuncs.Con_Printf("Generating bitmap font for scheme \"%s\" [%i]\n", schemeName,
								 curr_schm->resIndex < 0 ? -1 : g_ResArray[curr_schm->resIndex]);
			curr_schm->font = new CTargaFont(
									  fontName, curr_schm->fontSize, charWidth, charRotation,
									  fontWeight, isItalic, isUnderline, isStrikeout, isSymbolic
								  );
		}
	} else if (pFontData) gEngfuncs.COM_FreeFile(pFontData);	// font'll leak without this call
	
	m_pSchemeList.addElement(curr_schm);
	return m_pSchemeList.getCount() - 1;
}

//-----------------------------------------------------------------------------
// Purpose: always returns a valid scheme handle
// Input  : schemeHandle - 
// Output : CScheme
//-----------------------------------------------------------------------------
CSchemeManager::CScheme *CSchemeManager::getSafeScheme( SchemeHandle_t schemeHandle )
{
	return m_pSchemeList[schemeHandle < m_pSchemeList.getCount() ? schemeHandle : 0];
}


//-----------------------------------------------------------------------------
// Purpose: Returns the schemes pointer to a font
// Input  : schemeHandle - 
// Output : vgui::Font
//-----------------------------------------------------------------------------
vgui::Font *CSchemeManager::getFont( SchemeHandle_t schemeHandle )
{
	return getSafeScheme( schemeHandle )->font;
}

void CSchemeManager::getFgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->fgColor[0];
	g = pScheme->fgColor[1];
	b = pScheme->fgColor[2];
	a = pScheme->fgColor[3];
}

void CSchemeManager::getBgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->bgColor[0];
	g = pScheme->bgColor[1];
	b = pScheme->bgColor[2];
	a = pScheme->bgColor[3];
}

void CSchemeManager::getFgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->armedFgColor[0];
	g = pScheme->armedFgColor[1];
	b = pScheme->armedFgColor[2];
	a = pScheme->armedFgColor[3];
}

void CSchemeManager::getBgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->armedBgColor[0];
	g = pScheme->armedBgColor[1];
	b = pScheme->armedBgColor[2];
	a = pScheme->armedBgColor[3];
}

void CSchemeManager::getFgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->mousedownFgColor[0];
	g = pScheme->mousedownFgColor[1];
	b = pScheme->mousedownFgColor[2];
	a = pScheme->mousedownFgColor[3];
}

void CSchemeManager::getBgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->mousedownBgColor[0];
	g = pScheme->mousedownBgColor[1];
	b = pScheme->mousedownBgColor[2];
	a = pScheme->mousedownBgColor[3];
}

void CSchemeManager::getBorderColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	r = pScheme->borderColor[0];
	g = pScheme->borderColor[1];
	b = pScheme->borderColor[2];
	a = pScheme->borderColor[3];
}



