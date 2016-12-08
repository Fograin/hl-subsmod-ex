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
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "ofentities.h"

enum displacer_e {
    DISPLACER_IDLE1 = 0,
    DISPLACER_IDLE2,
    DISPLACER_SPINUP,
    DISPLACER_SPIN,
    DISPLACER_FIRE,
    DISPLACER_DRAW,
    DISPLACER_HOLSTER,
};

LINK_ENTITY_TO_CLASS( weapon_displacer, CDisplacer );

#define DISPLACER_SECONDARY_USAGE 60
#define DISPLACER_PRIMARY_USAGE 20

//=================================================
// Displacer weapon	
//=================================================
void CDisplacer::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_displacer"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_DISPLACER;
	SET_MODEL(ENT(pev), "models/w_displacer.mdl");

	m_iDefaultAmmo = DEFAULT_GIVE_DISPLACER;

	FallInit();// get ready to fall down.
}

int CDisplacer::AddToPlayer( CBasePlayer *pPlayer )
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

void CDisplacer::Precache( void )
{
	UTIL_PrecacheOther( "displacer_ball" );

	PRECACHE_MODEL("models/v_displacer.mdl");
	PRECACHE_MODEL("models/w_displacer.mdl");
	PRECACHE_MODEL("models/p_displacer.mdl");

	PRECACHE_SOUND("buttons/button11.wav");

	PRECACHE_SOUND("weapons/displacer_spin.wav");
	PRECACHE_SOUND("weapons/displacer_spin2.wav");
	PRECACHE_SOUND("weapons/displacer_fire.wav");
	PRECACHE_SOUND("weapons/displacer_start.wav");
	PRECACHE_SOUND("weapons/displacer_self.wav");

	m_iBeam = PRECACHE_MODEL("sprites/lgtning.spr");

	PRECACHE_MODEL("sprites/xflare1.spr");

	m_usSpinDisplacer = PRECACHE_EVENT( 1, "events/spindisplacer.sc" );
	m_usFireDisplacer = PRECACHE_EVENT( 1, "events/firedisplacer.sc" );
}

int CDisplacer::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = MAX_CARRY_URANIUM;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 5;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_DISPLACER;
	p->iWeight = WEIGHT_DISPLACER;

	return 1;
}

// Fograin92: Deploy correct weapon model
BOOL CDisplacer::Deploy( )
{
	return DefaultDeploy( "models/v_displacer.mdl", "models/p_displacer.mdl", DISPLACER_DRAW, "egon", 0 );
}
void CDisplacer::Holster( int skiplocal )
{
	SetThink( NULL );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( DISPLACER_HOLSTER );
}
void CDisplacer::SecondaryAttack()
{
	int flags;
	flags = 0;
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < DISPLACER_SECONDARY_USAGE)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "buttons/button11.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	m_iSecondaryMode = FALSE;
	pev->nextthink = gpGlobals->time + 1.05;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase()  + 1.05;
	SetThink( &CDisplacer::AltFireThink );
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.20;

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSpinDisplacer, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, m_iBeam, 0, m_iSecondaryMode); 
}
void CDisplacer::PrimaryAttack()
{
	int flags;
	flags = 0;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < DISPLACER_PRIMARY_USAGE)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "buttons/button11.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}
	m_iSecondaryMode = TRUE;
	pev->nextthink = gpGlobals->time + 1.05;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.05;
	SetThink( &CDisplacer::FireThink );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.20;
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSpinDisplacer, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, m_iBeam, 0, m_iSecondaryMode); 
}
void CDisplacer::FireThink( void )
{
	int flags;
	flags = 0;

	SetThink ( NULL );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireDisplacer, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0); 
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 35/30;
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= DISPLACER_PRIMARY_USAGE;

	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );

	anglesAim.x		= -anglesAim.x;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir	 = gpGlobals->v_forward;

	#ifndef CLIENT_DLL
		CDisplacerBall *pBall = CDisplacerBall::BallCreate();
		pBall->pev->owner = m_pPlayer->edict();
		pBall->pev->origin = vecSrc;
		pBall->pev->angles = anglesAim;
		pBall->pev->avelocity.z = 10;
	#endif
}
void CDisplacer::AltFireThink( void )
{
#ifndef CLIENT_DLL
	m_iWorld = !m_iWorld;
	Vector	vecSrc;
	CBaseEntity *pTarget = NULL;
	SendWeaponAnim ( DISPLACER_SPINUP );
	SetThink ( NULL );

	if ( g_pGameRules->IsMultiplayer() )
	{
		// Randomize the destination in multiplayer
		for ( int i = RANDOM_LONG(1,5); i > 0; i-- )
		pTarget = UTIL_FindEntityByClassname(pTarget, "info_player_deathmatch" );
	}
	else
	{
		if (!m_iWorld)
		pTarget = UTIL_FindEntityByClassname(pTarget, "info_displacer_earth_target" );
		else if (m_iWorld)
		pTarget = UTIL_FindEntityByClassname(pTarget, "info_displacer_xen_target" );
	}
	if ( pTarget )
	{
		Vector vecSrc	 = m_pPlayer->pev->origin;
		CDisplacerBall *pBall = CDisplacerBall::SelfCreate();
		pBall->pev->owner = m_pPlayer->edict();
		pBall->pev->origin = vecSrc;

		UTIL_ScreenFade( m_pPlayer, Vector(0, 160, 0), 0.5, 0.5, 255, FFADE_IN );

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= DISPLACER_SECONDARY_USAGE;
		Vector tmp = pTarget->pev->origin;
		tmp.z -= m_pPlayer->pev->mins.z;
		tmp.z++;
		UTIL_SetOrigin( m_pPlayer->pev, tmp );

		m_pPlayer->pev->angles = pTarget->pev->angles;
		m_pPlayer->pev->velocity = m_pPlayer->pev->basevelocity = g_vecZero;
		vecSrc = pTarget->pev->origin;

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD( vecSrc.x );	// X
			WRITE_COORD( vecSrc.y );	// Y
			WRITE_COORD( vecSrc.z );	// Z
			WRITE_BYTE( 24 );		// radius * 0.1
			WRITE_BYTE( 255 );		// r
			WRITE_BYTE( 180 );		// g
			WRITE_BYTE( 96 );		// b
			WRITE_BYTE( 31.66 );		// time * 10
			WRITE_BYTE( 1 );		// decay * 0.1
		MESSAGE_END( );

		CSprite *pSpr = CSprite::SpriteCreate( "sprites/xflare1.spr", vecSrc, TRUE );
			pSpr->AnimateAndDie( 6 );
			pSpr->SetTransparency(kRenderGlow, 128, 250, 214, 255, kRenderFxNoDissipation);

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/displacer_self.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
	}
	else
	{
		//ALERT( at_console, "Cannot find target %s\n", pTarget );
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "buttons/button11.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
	}
#endif
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}
void CDisplacer::WeaponIdle( void )
{
	int iAnim;
	if ( m_flTimeWeaponIdle < UTIL_WeaponTimeBase() )
	{
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		if (flRand <= 0.8)
		{
			iAnim = DISPLACER_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (35.0/12.0);// * RANDOM_LONG(2, 5);
		}
		else
		{
			iAnim = DISPLACER_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (25.0/9.0);
		}
		SendWeaponAnim( iAnim );
	}
}