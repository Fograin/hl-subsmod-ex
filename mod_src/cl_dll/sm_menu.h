//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Fograin92
//  Inspired by: Half-Life: Paranoia mod VGUI HUD
//	This code might contain parts written by: BUzer
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef _SM_MENU_H_
#define _SM_MENU_H_

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_loadtga.h"
#include "soundengine.h" // Fograin92
#include "sm_hud.h"
using namespace vgui;


// Load best possible result for our resolution
// Written by: BUzer | Edited by: Fograin92
inline BitmapTGA* LoadResolutionMenuImage (const char *imgname)
{
	BitmapTGA *pBitmap;
	static int resArray[] =
	{
		1024,
		1280,
		1360,
		1440,
		1600,
		1680,
		1920
	};

	// Try to load image directly (if %d is not specified)
	pBitmap = vgui_LoadTGA(imgname);

	// We can't load image directly
	if (!pBitmap)
	{
		int resArrayIndex = 0;
		int i = 0;
		while ((resArray[i] <= ScreenWidth) && (i < 7))
		{
			resArrayIndex = i;
			i++;
		}

		// Try to load resolution based image
		while(pBitmap == NULL && resArrayIndex >= 0)
		{
			char imgName[64];
			sprintf(imgName, imgname, resArray[resArrayIndex]);
			gEngfuncs.Con_Printf( "^3-> %s\n", imgName );
			pBitmap = vgui_LoadTGA(imgName);
			resArrayIndex--;
		}
	}

	return pBitmap;
}

// Simple class that owns pointer to a bitmap and draws it
// Written by: BUzer
class MenuImageHolder : public Panel
{
public:
	MenuImageHolder(const char *imgname, Panel *parent) : Panel(0, 0, 10, 10)
	{
		m_pBitmap = LoadResolutionMenuImage(imgname);
		if (m_pBitmap)
		{
			setParent(parent);
			setPaintBackgroundEnabled(false);
			setVisible(true);
			m_pBitmap->setPos(0, 0);
			m_pBitmap->getSize(sizeX, sizeY);	// Fograin92: Get X,Y size from bitmap
			setSize(sizeX, sizeY);	// Fograin92: Now panel have the size of the bitmap
		}
		
	}

	~MenuImageHolder() {delete m_pBitmap;}
	BitmapTGA *GetBitmap() {return m_pBitmap;}

protected:
	virtual void paint()
	{
		if (m_pBitmap)
				m_pBitmap->doPaint(this);
	}

	int sizeX, sizeY;	// Fograin92: Used for panel size
	BitmapTGA *m_pBitmap;
};


// Prepare command button
class CommandButton;


// Define our main menu container
class CMainMenuNew : public Panel
{

public:
    CMainMenuNew();
	~CMainMenuNew();

	// Shared functions
	int ShouldDrawMenu();		// Should we draw main menu? (Called every HUD::Redraw)
	void UpdateMainMenu();		// Update Main Menu variables

	// Shared vars
	bool bDrawMenu;				// Used to control if menu should be displayed or not, also used to force draw.



protected:

	// Utility functions
	int MenuAdjustPosition(int iHudObjectID, bool bReturnY);		// Return adjusted position of menu object, based on current screen resolution

	// Main functions
	void ResetVars(bool bInitOnly);		// Reset main menu Variables (called by constructor and destructor)
	virtual void paint();				// Called every frame when main menu is visable

	// Base vars
	bool bResError;						// Resolution is not supported error

	// Player pointer
	//cl_entity_t *m_LocalPlayer;

	// Objects
	CSchemeManager *pSchemes;		// Scheme manager data
	Font *pFont;					// Base FONT (BIG)
	Font *pFontText;				// Text FONT (Normal)


	// Main menu game title
	Label			*pLabelGameTitle;

	// Main menu buttons panel
	Panel			*pPanelMainMenu;

	// Main menu buttons
	CommandButton	*pBtn_Console;
	CommandButton	*pBtn_NewGame;
	CommandButton	*pBtn_LoadGame;
	CommandButton	*pBtn_SaveGame;
	CommandButton	*pBtn_Options;
	CommandButton	*pBtn_Extras;
	CommandButton	*pBtn_Quit;
};


// Fograin92: Handle button click
class CMainMenuNew_BTNhandler : public ActionSignal
{
private:
	int	_iBTNnum;	// ID of clicked button

public:
	CMainMenuNew_BTNhandler(int	iBTNnum)
	{
		_iBTNnum = iBTNnum;
	}

	// Used as public handler
	void handleAction(int iBTNnum);

	// Link with action signal
	virtual void actionPerformed(Panel* panel)
	{
		this->handleAction(_iBTNnum);
	}
};


// Fograin92: Handle changing cvars
/*
class CMainMenuNew_CVARhandler : public ActionSignal
{
private:
	struct cvar_s * m_cvar;

public:
	CMainMenuNew_CVARhandler( char * cvarname )
	{
		m_cvar = gEngfuncs.pfnGetCvarPointer( cvarname );
	}

	virtual void actionPerformed(Panel* panel)
	{
		if ( m_cvar->value )
			m_cvar->value = 0.0f;
		else
			m_cvar->value = 1.0f;

		gViewPort->UpdateSpectatorPanel();
	}
};
*/


#endif // _SM_MENU_H_