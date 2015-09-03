//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Fograin92
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
using namespace vgui;


class ImageHolder;


// Define our main HUD container
class CHudNew : public Panel
{

public:
    CHudNew();
	~CHudNew();

	// Shared functions
	void UpdateHUD();							// Update HUD variables
	void SetHealthVar(int iPlayerHealth);		// Mutator <- Send new health value
	void SetArmorVar(int iPlayerArmor);			// Mutator <- Send new armor value
	void PickedUpItem( const char *szName );	// Called when player picked up item
	int ShouldDrawHUD();						// Should we draw HEV? (Called every HUD::Redraw, required for hud_draw cvar)

	// Shared vars
	bool bHaveHEV;		// Do we have HEV/PCV equiped?

	
protected:

	// Utility functions
	int AdjustPosition(int iHudObjectID, bool bReturnY);		// Return adjusted X position of HUD object, based on current screen resolution
	void GetGame();	// Set proper HUD variables depending on "sm_hud" value
	
	// Main functions
	virtual void paint();		// Called every frame when HUD is visable
	//virtual void solve();		// This is called every frame (Thanks to: BUzer for nice find!)
	
	// Base vars
	bool bResError;				// Resolution is not supported
	int iHudColor[3];			// HUD Color
	CSchemeManager *pSchemes;	// Scheme manager data
	Font *pFont;				// Base FONT

	// Health + Armor panels
	int				iHealthSizeX, iHealthSizeY;
	Panel			*pHealthPanel;		// Health+Armor panel background/container
	ImageHolder		*pHealthIcon;		// Health icon
	int				iHealth;			// Health value
	Label			*pHealthLab;		// Health value text
	ImageHolder		*pArmorIcon;		// Armor icon HL
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

};


#endif // _SM_HUD_H_