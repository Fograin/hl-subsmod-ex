//========= Copyright © 2004-2006, Raven City Team, All rights reserved. ============
//
// Purpose:	Opposing Forces Sniper Rifle, Copyright Highlander
//
// $NoKeywords: $
//===================================================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

enum sniper_e {
	SNIPER_DRAW = 0,
	SNIPER_SLOWIDLE1,
	SNIPER_FIRE,
	SNIPER_FIRELASTROUND,
	SNIPER_RELOAD1,
	SNIPER_RELOAD2,
	SNIPER_RELOAD3,
	SNIPER_SLOWIDLE2,
	SNIPER_HOLSTER,
};

LINK_ENTITY_TO_CLASS( weapon_sniperrifle, CSniperRifle );

void CSniperRifle::Spawn( )
{

	pev->classname = MAKE_STRING("weapon_sniperrifle"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_SNIPERRIFLE;
	SET_MODEL(ENT(pev), "models/w_m40a1.mdl");

	m_iDefaultAmmo = MAX_CLIP_SNIPERRIFLE;

	FallInit();// get ready to fall down.
}


void CSniperRifle::Precache( void )
{
	PRECACHE_MODEL("models/v_m40a1.mdl");
	PRECACHE_MODEL("models/v_m40a1_bs.mdl");	// Fograin92
	PRECACHE_MODEL("models/v_m40a1_of.mdl");	// Fograin92
	PRECACHE_MODEL("models/w_m40a1.mdl");
	PRECACHE_MODEL("models/p_m40a1.mdl");

	PRECACHE_SOUND ("weapons/sniper_bolt1.wav");
	PRECACHE_SOUND ("weapons/sniper_bolt2.wav");
	PRECACHE_SOUND ("weapons/sniper_fire.wav");
	PRECACHE_SOUND ("weapons/sniper_reload_first_seq.wav");
	PRECACHE_SOUND ("weapons/sniper_reload_second_seq.wav");
	PRECACHE_SOUND ("weapons/sniper_reload3.wav");
	PRECACHE_SOUND ("weapons/sniper_zoom.wav");

	m_usM40a1 = PRECACHE_EVENT( 1, "events/m40a1.sc" );
}

int CSniperRifle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762";
	p->iMaxAmmo1 = MAX_CARRY_SNIPERRIFLE;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = MAX_CLIP_SNIPERRIFLE;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SNIPERRIFLE;
	p->iWeight = WEIGHT_SNIPERRIFLE;

	return 1;
}

// Fograin92: Changed deploy function
BOOL CSniperRifle::Deploy( )
{
	if (CVAR_GET_FLOAT("sm_hud") == 1 )	// Blue Shift
		return DefaultDeploy( "models/v_m40a1_bs.mdl", "models/p_m40a1_bs.mdl", SNIPER_DRAW, "gauss", 0 );
	
	if (CVAR_GET_FLOAT("sm_hud") == 2 )	// Opposing Force
		return DefaultDeploy( "models/v_m40a1_of.mdl", "models/p_m40a1_of.mdl", SNIPER_DRAW, "gauss", 0 );

	return DefaultDeploy( "models/v_m40a1.mdl", "models/p_m40a1.mdl", SNIPER_DRAW, "gauss", 0 );
}

int CSniperRifle::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CSniperRifle::Holster( int skiplocal )
{
	m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( SNIPER_HOLSTER );

	if ( m_fInZoom )
	{
		SecondaryAttack();
	}
}

void CSniperRifle::SecondaryAttack()
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = 0;
	}
	else if ( m_pPlayer->pev->fov != 15 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 15;
		m_fInZoom = 1;
	}
	
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/sniper_zoom.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);

	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
}
void CSniperRifle::PrimaryAttack()
{
	if ( m_fInSpecialReload )
		return;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	float flSpread = 0.001;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	vecAiming = gpGlobals->v_forward;

	Vector vecDir;

	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_762, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	m_flNextPrimaryAttack = 1.75;
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usM40a1, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 1 : 0, 0 );

	// Fograin92
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sniper_fire.wav", 0.80, ATTN_NORM);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)

	// HEV suit - indicate out of ammo condition
	m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 68.0 / 38.0;
}


void CSniperRifle::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == MAX_CLIP_SNIPERRIFLE)
		return;

	int iResult;

	if ( m_pPlayer->pev->fov != 0 )
	{
		SecondaryAttack();
	}

	if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0 )
	{
		iResult = DefaultReload( 5, SNIPER_RELOAD1, 80 / 34 );
		m_fInSpecialReload = 1;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.25;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.25;
	}
	else
	{
		iResult = DefaultReload( 5, SNIPER_RELOAD3, 2.25 );
	}
}
void CSniperRifle::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if ( m_fInSpecialReload )
		{
			m_fInSpecialReload = 0;
			SendWeaponAnim( SNIPER_RELOAD2 );
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 49.0 / 27.0;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 27.0;
		}
		else
		{
			m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );
			if (m_iClip <= 0)
			{
				iAnim = SNIPER_SLOWIDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 80.0 / 16.0;
			}
			else
			{
				iAnim = SNIPER_SLOWIDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 67.5 / 16;
			}
			SendWeaponAnim( iAnim, 1 );
		}
	}
}

class CSniperRifleAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_m40a1_clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_m40a1_clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( MAX_CLIP_SNIPERRIFLE, "762", MAX_CARRY_SNIPERRIFLE) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_762, CSniperRifleAmmo )