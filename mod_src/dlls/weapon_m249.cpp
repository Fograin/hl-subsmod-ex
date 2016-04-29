//========= Copyright © 2004-2006, Raven City Team, All rights reserved. ============
//
// Purpose:	Opposing Forces M249, Copyright Highlander
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
#include "soundent.h"
#include "gamerules.h"

enum m249_e
{
	M249_SLOWIDLE = 0,
	M249_IDLE1,
	M249_RELOAD,
	M249_RELOAD2,
	M249_HOLSTER,
	M249_DRAW,
	M249_FIRE1,
	M249_FIRE2,
	M249_FIRE3,
};

LINK_ENTITY_TO_CLASS( weapon_m249, CM249 );

//=========================================================
//=========================================================

void CM249::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_m249"); // hack to allow for old names
	Precache( );
	SET_MODEL(ENT(pev), "models/w_saw.mdl");
	m_iId = WEAPON_M249;

	m_iDefaultAmmo = DEFAULT_GIVE_M249;

	FallInit();// get ready to fall down.
}


void CM249::Precache( void )
{
	PRECACHE_MODEL("models/v_saw.mdl");
	PRECACHE_MODEL("models/v_saw_of.mdl");	// Fograin92
	PRECACHE_MODEL("models/v_saw_bs.mdl");	// Fograin92
	PRECACHE_MODEL("models/w_saw.mdl");
	PRECACHE_MODEL("models/p_saw.mdl");

	m_iShell = PRECACHE_MODEL ("models/saw_shell.mdl");// saw shellTE_MODEL
	m_iLink = PRECACHE_MODEL ("models/saw_link.mdl");//  saw linkTE_MODEL

	PRECACHE_MODEL("models/w_saw_clip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND("weapons/saw_reload.wav");
	PRECACHE_SOUND("weapons/saw_reload2.wav");

	PRECACHE_SOUND ("weapons/saw_fire1.wav");
	PRECACHE_SOUND ("weapons/saw_fire2.wav");
	PRECACHE_SOUND ("weapons/saw_fire3.wav");

	PRECACHE_SOUND ("weapons/357_cock1.wav");

	m_usM249 = PRECACHE_EVENT( 1, "events/m249.sc" );
}

int CM249::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556";
	p->iMaxAmmo1 = MAX_CARRY_M249;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = MAX_CLIP_M249;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_M249;
	p->iWeight = WEIGHT_M249;

	return 1;
}

int CM249::AddToPlayer( CBasePlayer *pPlayer )
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

// Fograin92: Changed deploy function
BOOL CM249::Deploy( )
{
	UpdateClip();

	if (CVAR_GET_FLOAT("sm_hud") == 1 )	// Blue Shift
		return DefaultDeploy( "models/v_saw_bs.mdl", "models/p_saw_bs.mdl", M249_DRAW, "mp5", UseDecrement() ? 1 : 0, pev->body );
	
	if (CVAR_GET_FLOAT("sm_hud") == 2 )	// Opposing Force
		return DefaultDeploy( "models/v_saw_of.mdl", "models/p_saw_of.mdl", M249_DRAW, "mp5", UseDecrement() ? 1 : 0, pev->body );

	return DefaultDeploy( "models/v_saw.mdl", "models/p_saw.mdl", M249_DRAW, "mp5", UseDecrement() ? 1 : 0, pev->body );
}

void CM249::Holster( int skiplocal /* = 0 */)
{
	UpdateClip();
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( M249_HOLSTER, UseDecrement() ? 1 : 0, pev->body );
}

void CM249::PrimaryAttack()
{
	if ( m_fInSpecialReload )
		return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		UpdateClip();
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	UpdateClip();

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8192, BULLET_PLAYER_556, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );

  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usM249, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.0675;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.0675;

	if ( !FBitSet( m_pPlayer->pev->flags, FL_DUCKING ) )
	{
		float flOldPlayerVel = m_pPlayer->pev->velocity.z;
		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity + (50 * -gpGlobals->v_forward);
		m_pPlayer->pev->velocity.z = flOldPlayerVel;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 7.0 / 30.0;
}

void CM249::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == MAX_CLIP_M249)
		return;

	if (m_iClip != 50 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0 )
	{
		UpdateClip();
		m_fInSpecialReload = 1;
		DefaultReload( MAX_CLIP_M249, M249_RELOAD, 1.5, pev->body );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + (61 / 40)+(111 / 45);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 61 / 40;
	}
}
void CM249::UpdateClip( void )
{
	if (m_iClip <= 8)
		pev->body = 1;
	if (m_iClip <= 7)
		pev->body = 2;
	if (m_iClip <= 6)
		pev->body = 3;
	if (m_iClip <= 5)
		pev->body = 4;
	if (m_iClip <= 4)
		pev->body = 5;
	if (m_iClip <= 3)
		pev->body = 6;
	if (m_iClip <= 2)
		pev->body = 7;
	if (m_iClip <= 1)
		pev->body = 8;
	if (m_iClip == 0)
		pev->body = 8;

	if ( m_iClip != 0 
		&& m_iClip != 1 
		&& m_iClip != 2 
		&& m_iClip != 3 
		&& m_iClip != 4 
		&& m_iClip != 5 
		&& m_iClip != 6 
		&& m_iClip != 7 
		&& m_iClip != 8 
		&& m_iClip != 9 )
		pev->body = 0;
}
void CM249::WeaponIdle( void )
{
	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if ( m_fInSpecialReload )
		{
			UpdateClip();
			m_fInSpecialReload = 0;
			SendWeaponAnim( M249_RELOAD2 , UseDecrement() ? 1 : 0, pev->body );
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 111.0 / 45.0;
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 111.0 / 45.0;
		}
		else
		{
			ResetEmptySound( );
			UpdateClip();

			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = M249_SLOWIDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (45.0/9.0);
			}
			else
			{
				iAnim = M249_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (74.0/12.0);// * RANDOM_LONG(2, 5);
			}
			SendWeaponAnim( iAnim , UseDecrement() ? 1 : 0, pev->body );
		}
	}
}

class CM249AmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_saw_clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_saw_clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( MAX_CLIP_M249, "556", MAX_CARRY_M249) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_556, CM249AmmoClip )