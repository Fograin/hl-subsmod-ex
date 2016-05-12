//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	Valve LLC.
//	Id Software, Inc. ("Id Technology")
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef ITEMS_H
#define ITEMS_H

// Fograin92: Shared includes
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "skill.h"
#include "gamerules.h"


// Fograin92: Base item class
class CItem : public CBaseEntity
{
public:
	void	Spawn( void );
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	void	EXPORT Materialize( void );
	virtual BOOL MyTouch( CBasePlayer *pPlayer ) { return FALSE; };
};


// Fograin92: world_items - ITEM SPAWNER
class CWorldItem : public CBaseEntity
{
public:
	void	KeyValue(KeyValueData *pkvd ); 
	void	Spawn( void );
	int		m_iType;
};


//========================================//
// item_antidote - Antidote item (legacy)
//========================================//
class CItemAntidote : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//========================================//
// item_security - Security card (legacy)
//========================================//
class CItemSecurity : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//==========================================================================//
// item_airtank - Air tank, used to refil air underwater, also it explodes!
//==========================================================================//
class CAirtank : public CGrenade
{
public:
	void Spawn( void );
	void Precache( void );
	void Killed( entvars_t *pevAttacker, int iGib );
	void TankThink( void );
	void TankTouch( CBaseEntity *pOther );
	int	 BloodColor( void ) { return DONT_BLEED; };

	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	int	 m_state;
};


//============================//
// item_suit - HEV, PCV SUIT
//============================//
#define SF_SUIT_SHORTLOGON		0x0001	// Fograin92: Short logon flag; Now used to disable HEV logon sound

class CItemSuit : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//=====================================================//
// item_suit_long - HEV suit with long logon animation
//=====================================================//
class CItemSuitLong : public CItemSuit
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//============================//
// item_battery - HEV Battery
//============================//
class CItemBattery : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	void BatteryThink( void );	// Fograin92
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//======================================//
// item_longjump - HEV Long jump module
//======================================//
class CItemLongJump : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//=======================================//
// item_healthkit - Simple First Aid Kit
//=======================================//
class CHealthKit : public CItem
{
public:

	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//=====================================//
// item_helmet - Security Guard Helmet
//=====================================//
class CItemBsHelmet : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};


//=====================================//
// item_helmet - Security Guard Helmet
//=====================================//
class CItemBsVest : public CItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
};



#endif // ITEMS_H
