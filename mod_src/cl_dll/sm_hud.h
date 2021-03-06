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
#ifndef _SM_HUD_H_
#define _SM_HUD_H_

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_loadtga.h"
#include "soundengine.h" // Fograin92
using namespace vgui;


// Load best possible result for our resolution
// Written by: BUzer | Edited by: Fograin92
inline BitmapTGA* LoadResolutionImage (const char *imgname)
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
class ImageHolder : public Panel
{
public:
	ImageHolder(const char *imgname, Panel *parent) : Panel(0, 0, 10, 10)
	{
		
		m_pBitmap = LoadResolutionImage(imgname);
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

	~ImageHolder() {delete m_pBitmap;}
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


// Define our main HUD container
class CHudNew : public Panel
{

public:
    CHudNew();
	~CHudNew();

	// Shared functions
	void UpdateHUD();									// Update HUD variables
	void SetHealthVar(int iPlayerHealth);				// Mutator <- Send new health value
	void SetArmorVar(int iPlayerArmor);					// Mutator <- Send new armor value
	void PickedUpItem( const char *szName );			// Mutator <- Called when player picked up item
	void DamageIndicator( float fFront, float fSide );	// Mutator <- We send damage direction to this function
	void DamageSensor( int iDmgType, bool bEnabled );	// Mutator <- We send damage data to atmospheric contaminant sensors
	int ShouldDrawHUD();								// Should we draw HEV? (Called every HUD::Redraw, required for hud_draw cvar)
	void SetAirValue(float fAirVal);					// Mutator <- Send new air value

	// Shared vars
	bool bHaveHEV;		// Do we have HEV/PCV/Armor equiped?

protected:

	// Utility functions
	int AdjustPosition(int iHudObjectID, bool bReturnY);		// Return adjusted X position of HUD object, based on current screen resolution
	void GetGame();	// Set proper HUD variables depending on "sm_hud" value
	void ResetVars(bool bInitOnly);	// Reset HUD Variables (called by constructor and destructor)
	void RefreshVisual();	// Reset visibility of HUD elements
	
	// Main functions
	virtual void paint();		// Called every frame when HUD is visable
	//virtual void solve();		// This is called every frame (Thanks to: BUzer for nice find!)
	
	// Base vars
	bool bResError;				// Resolution is not supported
	bool bSoundPlaying;			// Used to check if we're playing HUD sound
	int iHudColor[3];			// HUD Color
	CSchemeManager *pSchemes;	// Scheme manager data
	Font *pFont;				// Base FONT
	Font *pFontText;			// Text FONT
	Font *pFontOF;				// Scanline FONT
	Font *pFontTextOF;			// Scanline Text FONT

	// Health + Armor panels
	int				iHealthSizeX, iHealthSizeY;
	Panel			*pHealthPanel;		// Health+Armor panel background/container
	ImageHolder		*pHealthIcon;		// Health icon
	int				iHealth;			// Health value
	Label			*pHealthLab;		// Health value text
	ImageHolder		*pArmorIconHL;		// Armor icon HL
	ImageHolder		*pArmorIconHLc;		// Armor icon HL charged
	int				iArmor;				// Armor value
	Label			*pArmorLab;			// Armor value text

	// Ammo panel
	int				iAmmoSizeX, iAmmoSizeY;			// Primary ammo panel size
	int				iPrimaryClip, iPrimaryAmmo;		// Primary clip and ammo value
	int				iSecondaryClip, iSecondaryAmmo;	// Secondary clip and ammo value

	Panel			*pAmmoPanel;			// Primary ammo panel background/container
	Label			*pPrimaryAmmoLab;		// Primary ammo label
	
	Panel			*pSecondaryAmmoPanel;	// Secondary ammo panel background/container
	Label			*pSecondaryAmmoLab;		// Secondary ammo label

	// Ammo icons
	ImageHolder		*pIconAmmo9mm;			// 9mm (mp5) ammo icon
	ImageHolder		*pIconAmmoGlock;		// 9mm (glock) ammo icon
	ImageHolder		*pIconAmmo357;			// .44 ammo icon
	ImageHolder		*pIconAmmoAR;			// AR grenade
	ImageHolder		*pIconAmmoBuckshot;		// Shotgun ammo icon
	ImageHolder		*pIconAmmoArrow;		// Crossbow arrows icon
	ImageHolder		*pIconAmmoRocket;		// RPG rockets icon
	ImageHolder		*pIconAmmoUranium;		// Depleted uranium icon
	ImageHolder		*pIconAmmoBee;			// NO! NOT THE BEES!
	ImageHolder		*pIconAmmoGrenade;
	ImageHolder		*pIconAmmoSatchel;
	ImageHolder		*pIconAmmoTripmine;
	ImageHolder		*pIconAmmoSnark;

	// Pain direction indicators
	float fTimer_TopDmg, fTimer_BottomDmg, fTimer_LeftDmg, fTimer_RightDmg;	// Used for alpha animations
	ImageHolder		*pPainTopDirIcon;		// Top direction indicator
	ImageHolder		*pPainBottomDirIcon;	// Bottom direction indicator
	ImageHolder		*pPainLeftDirIcon;		// Left direction indicator
	ImageHolder		*pPainRightDirIcon;		// Right direction indicator

	// Atmospheric contaminant sensors
	Panel			*pDmgPanel;
	int				iDmgActiveIcons;		// Number of displayed icons
	ImageHolder		*pDmgAir;
	ImageHolder		*pDmgBio;
	ImageHolder		*pDmgChem;
	ImageHolder		*pDmgFire;
	ImageHolder		*pDmgFrost;
	ImageHolder		*pDmgGas;
	ImageHolder		*pDmgRad;
	ImageHolder		*pDmgShock;
	float			fTimer_iDmgAir;			// Icon timers
	float			fTimer_iDmgBio;
	float			fTimer_iDmgChem;
	float			fTimer_iDmgFire;
	float			fTimer_iDmgFrost;
	float			fTimer_iDmgGas;
	float			fTimer_iDmgRad;
	float			fTimer_iDmgShock;


	// Air counter
	Panel			*pAirPanel;			// Air Counter panel background/container
	ImageHolder		*pAirIcon;			// Air Counter icon
	float			fAir;				// Air value
	Label			*pAirLab;			// Air value text
	//cl_entity_t		*cl_Player;		// Pointer to the player, because why the fuck not


	// Crosshairs
	ImageHolder		*pCrosshairDefault;		// Default crosshair
	ImageHolder		*pCrosshairGlock;
	ImageHolder		*pCrosshair357;
	ImageHolder		*pCrosshairDeagle;
	ImageHolder		*pCrosshairMP5;
	ImageHolder		*pCrosshairShotgun;
	ImageHolder		*pCrosshairCrossbow;
	ImageHolder		*pCrosshairRPG;
	ImageHolder		*pCrosshairGauss;
	ImageHolder		*pCrosshairEgon;
	ImageHolder		*pCrosshairHive;
	ImageHolder		*pCrosshairM249;
	ImageHolder		*pCrosshairDisplacer;
	ImageHolder		*pCrosshairSniper;


	// HEV Logon/Intro sequence
	bool	bShortLogon;		// Should we use short-intro version?
	float	fTimer_Logon;		// Logon/Intro sequence timer, when set the sequence starts, when it's over the whole sequence is over.
	float	fTimer_LogonOLD;	// Previous value of the timer, used to check if timer was changed between paint() execs.
	int		iScanNum;			// Length of characters to display. Used for write-in effect.
	int		iTimerSpeed;		// Used to controll the speed of the timer
	int		iAlpha;				// Used as a helper for ALPHA color animations

	Panel			*pLogonConsolePanel;	// Top-left console panel (console text container)
	Label			*pLogonText;			// Top-left console text
	ImageHolder		*pImgLogon01;
	ImageHolder		*pImgLogon02;
	ImageHolder		*pImgLogon03;
	ImageHolder		*pImgLogon04;
	ImageHolder		*pImgLogon05;
	ImageHolder		*pImgLogon06;
	ImageHolder		*pImgLogon07;
	ImageHolder		*pImgLogon08;
};


#endif // _SM_HUD_H_