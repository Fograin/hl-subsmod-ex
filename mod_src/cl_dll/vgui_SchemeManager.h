//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//
//-----------------------------------------------------------------------------
//
// Rewritten by Vit_amiN to make it able to handle any number of schemes and
// stay compatible with the interface of the original Valve's version at the
// same time.
//
//=============================================================================

#ifndef VGUI_SCHEMEMANAGER_H
#define VGUI_SCHEMEMANAGER_H

#include <VGUI_Dar.h>	// Vit_amiN
#include <VGUI_Font.h>


// handle to an individual scheme
typedef int SchemeHandle_t;


// Register console variables, etc..
void Scheme_Init();


//-----------------------------------------------------------------------------
// Purpose: Handles the loading of text scheme description from disk
//			supports different font/color/size schemes at different resolutions 
//-----------------------------------------------------------------------------
class CSchemeManager
{
public:
	enum	// Vit_amiN: font style flags
	{
		FONT_STYLE_NOT_SET		= 0x00,
		FONT_STYLE_ITALIC		= 0x01,
		FONT_STYLE_UNDERLINE	= 0x02,
		FONT_STYLE_STRIKEOUT	= 0x04,
		FONT_STYLE_SYMBOLIC		= 0x08,
	};

	// initialization
	CSchemeManager( int xRes, int yRes );
	virtual ~CSchemeManager();

	// scheme handling; Vit_amiN: compatible with the original version
	SchemeHandle_t getSchemeHandle(
		const char * const,							// scheme name
		const bool & = false,						// use current X res
		const bool & = false,						// scale font size
		const char * const = "Arial",				// font name
		const int & = 17,							// chars height
		const int & = 0,							// chars width
		const float & = 0.0f,						// chars rotation
		const int & = 0,							// font weight
		const int & = FONT_STYLE_NOT_SET,			// font style
		const byte * const = defFgColor,			// fg color
		const byte * const = defBgColor,			// bg color
		const byte * const = defArmedFgColor,		// fg armed color
		const byte * const = defArmedBgColor,		// bg armed color
		const byte * const = defMousedownFgColor,	// fg mousedown color
		const byte * const = defMousedownBgColor,	// bg mousedown color
		const byte * const = defBorderColor			// border color
	);

private:
	// Vit_amiN: new scheme loader (for internal use)
	SchemeHandle_t loadNewScheme(
		const char * const,		// scheme name
		const int &,			// X resolution index
		const int &,			// font flags
		const char * const,		// font name
		const int &,			// chars height
		const int &,			// chars width
		const float &,			// chars rotation
		const int &,			// font weight
		const int &,			// font style
		const byte * const,		// fg color
		const byte * const,		// bg color
		const byte * const,		// fg armed color
		const byte * const,		// bg armed color
		const byte * const,		// fg mousedown color
		const byte * const,		// bg mousedown color
		const byte * const		// border color
	);

public:
	// getting info from schemes
	vgui::Font *getFont( SchemeHandle_t schemeHandle );
	void getFgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getFgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getFgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBorderColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );

private:
	class CScheme;
	vgui::Dar<CScheme *> m_pSchemeList;	// Vit_amiN: now it can handle any number of schemes
	static const byte defFgColor[4];	// Vit_amiN: these arrays are storing default colors
	static const byte defBgColor[4];
	static const byte defArmedFgColor[4];
	static const byte defArmedBgColor[4];
	static const byte defMousedownFgColor[4];
	static const byte defMousedownBgColor[4];
	static const byte defBorderColor[4];

	// Vit_amiN: the index of the X res we were initted at.
	int		m_xResIndex;

	CScheme *getSafeScheme( SchemeHandle_t schemeHandle );
};

// Vit_amiN: resolutions data
extern const int g_ResArray[];
extern const int g_NumReses;

#endif // VGUI_SCHEMEMANAGER_H
