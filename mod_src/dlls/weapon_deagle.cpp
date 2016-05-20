//========= Copyright © 2004-2008, Raven City Team, All rights reserved. ============//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

enum deagle_e
{
	DEAGLE_IDLE1 = 0,
	DEAGLE_IDLE2,
	DEAGLE_IDLE3,
	DEAGLE_IDLE4,
	DEAGLE_IDLE5,
	DEAGLE_SHOOT,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_RELOAD_NOT_EMPTY,
	DEAGLE_DRAW,
	DEAGLE_HOLSTER,
};
LINK_ENTITY_TO_CLASS( weapon_eagle, CDeagle );

LINK_ENTITY_TO_CLASS( eagle_laser, CEagleLaser );
//=========================================================
//=========================================================
CEagleLaser *CEagleLaser::CreateSpotDeagle( void )
{
	CEagleLaser *pEagleLaser = GetClassPtr( (CEagleLaser *)NULL );
	pEagleLaser->Spawn();

	pEagleLaser->pev->classname = MAKE_STRING("eagle_laser");

	return pEagleLaser;
}

//=========================================================
//=========================================================
void CEagleLaser::Spawn( void )
{
	Precache( );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;
	pev->scale = 0.40;

	SET_MODEL(ENT(pev), "sprites/laserdot.spr");
	UTIL_SetOrigin( pev, pev->origin );
};

//=========================================================
// Suspend- make the laser sight invisible. 
//=========================================================
void CEagleLaser::Suspend( float flSuspendTime )
{
	pev->effects |= EF_NODRAW;
	SetThink( &CEagleLaser::Revive );
	pev->nextthink = gpGlobals->time + flSuspendTime;
}

//=========================================================
// Revive - bring a suspended laser sight back.
//=========================================================
void CEagleLaser::Revive( void )
{
	pev->effects &= ~EF_NODRAW;
	SetThink( NULL );
}
void CEagleLaser::Precache( void )
{
	PRECACHE_MODEL("sprites/laserdot.spr");
	PRECACHE_MODEL("sprites/laserbeam.spr");
};


void CDeagle::Spawn( )
{

	pev->classname = MAKE_STRING("weapon_eagle"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_DEAGLE;
	SET_MODEL(ENT(pev), "models/w_desert_eagle.mdl");

	m_iDefaultAmmo = MAX_CLIP_DEAGLE;

	FallInit();// get ready to fall down.
}


void CDeagle::Precache( void )
{
	UTIL_PrecacheOther( "eagle_laser" );
	PRECACHE_MODEL("models/v_desert_eagle.mdl");
	PRECACHE_MODEL("models/v_desert_eagle_bs.mdl");	// Fograin92
	PRECACHE_MODEL("models/v_desert_eagle_of.mdl");	// Fograin92
	PRECACHE_MODEL("models/w_desert_eagle.mdl");
	PRECACHE_MODEL("models/p_desert_eagle.mdl");
	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	PRECACHE_SOUND("weapons/deagle_slideforward.wav");
	PRECACHE_SOUND("weapons/deagle_magout.wav");
	PRECACHE_SOUND("weapons/deagle_magin.wav");

	PRECACHE_SOUND ("weapons/desert_eagle_fire.wav");
	PRECACHE_SOUND ("weapons/desert_eagle_sight.wav");
	PRECACHE_SOUND ("weapons/desert_eagle_sight2.wav");

	m_usEagle = PRECACHE_EVENT( 1, "events/eagle.sc" );
}

int CDeagle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = MAX_CARRY_357;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = MAX_CLIP_DEAGLE;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iWeight = WEIGHT_DEAGLE;

	return 1;
}

// Fograin92: Deploy correct weapon model
BOOL CDeagle::Deploy( )
{
	if (CVAR_GET_FLOAT("sm_hud") == 1 )	// Blue Shift
		return DefaultDeploy( "models/v_desert_eagle_bs.mdl", "models/p_desert_eagle.mdl", DEAGLE_DRAW, "onehanded", 0 );
	
	if (CVAR_GET_FLOAT("sm_hud") == 2 )	// Opposing Force
		return DefaultDeploy( "models/v_desert_eagle_of.mdl", "models/p_desert_eagle.mdl", DEAGLE_DRAW, "onehanded", 0 );

	return DefaultDeploy( "models/v_desert_eagle.mdl", "models/p_desert_eagle.mdl", DEAGLE_DRAW, "onehanded", 0 );
}


void CDeagle::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( DEAGLE_HOLSTER );

	if (m_pEagleLaser)
	{
		m_pEagleLaser->Killed( NULL, GIB_NEVER );
		m_pEagleLaser = NULL;
	}
}

void CDeagle::SecondaryAttack()
{
	m_fEagleLaserActive = ! m_fEagleLaserActive;
	if (!m_fEagleLaserActive && m_pEagleLaser)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/desert_eagle_sight2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
		m_pEagleLaser->Killed( NULL, GIB_NORMAL );
		m_pEagleLaser = NULL;
	}
	else
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/desert_eagle_sight.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
	}
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.2;
}
void CDeagle::PrimaryAttack()
{
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
		UpdateSpot( );
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	UpdateSpot( );

	float flSpread = 0.01;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;
	BOOL m_fLaserOn;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	vecAiming = gpGlobals->v_forward;

	Vector vecDir;
	if (m_pEagleLaser)
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_DEAGLE, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		m_flNextPrimaryAttack = 0.50;
		m_pEagleLaser->Suspend( 0.75 );
		m_fLaserOn = TRUE;
	}
	else
	{
		flSpread = 0.1;
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_DEAGLE, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		m_flNextPrimaryAttack = 0.20;
		m_fLaserOn = FALSE;
	}

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usEagle, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, flSpread, flSpread, ( m_iClip == 0 ) ? 1 : 0, 0 );

	// Fograin92
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/desert_eagle_fire.wav", 0.80, ATTN_NORM);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	// HEV suit - indicate out of ammo condition
	m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CDeagle::Reload( void )
{

	if ( m_pEagleLaser && m_fEagleLaserActive )
	{
		m_pEagleLaser->Suspend( 1.25 );
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.25;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == MAX_CLIP_DEAGLE)
		return;

	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 7, DEAGLE_RELOAD, 1.25 );
	else
		iResult = DefaultReload( 7, DEAGLE_RELOAD_NOT_EMPTY, 1.25 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
}

void CDeagle::UpdateSpot( void )
{
#ifndef CLIENT_DLL
	if (m_fEagleLaserActive)
	{
		if (!m_pEagleLaser)
		{
			m_pEagleLaser = CEagleLaser::CreateSpotDeagle();
		}

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition( );;
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );
		
		UTIL_SetOrigin( m_pEagleLaser->pev, tr.vecEndPos );

		if ( UTIL_PointContents(tr.vecEndPos) == CONTENT_SKY )
		{
			UTIL_Remove( m_pEagleLaser );
			m_pEagleLaser = FALSE;
		}

	}
#endif
}


void CDeagle::WeaponIdle( void )
{
	UpdateSpot( );

	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		ResetEmptySound( );
		m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

		// only idle if the slid isn't back
		if (m_iClip != 0)
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

			if (flRand <= 0.3 + 0 * 0.75)
			{
				iAnim = DEAGLE_IDLE5;//Done
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 34.0 / 16.0;
			}
			else if (flRand <= 0.6 + 0 * 0.75)
			{
				iAnim = DEAGLE_IDLE4;//Done
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
			}
			else if (flRand <= 0.9 + 0 * 0.75)
			{
				iAnim = DEAGLE_IDLE1;//Done
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
			}
			else if (flRand <= 0.3 + 0 * 0.80)
			{
				iAnim = DEAGLE_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
			}
			else
			{
				iAnim = DEAGLE_IDLE3;//Done
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 25.0 / 16;
			}
			SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );
		}
	}
}