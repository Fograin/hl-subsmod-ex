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
#include "customentity.h"
#include "decals.h"
#include "func_break.h"
#include "explode.h"
#include "trains.h"
#include "particle_defs.h"

extern int gmsgParticles;
//=================================
// Opposing Forces spore ammo plant 
// Copyright Demiurge, Copyright Highlander
//=================================

LINK_ENTITY_TO_CLASS( spore, CSpore );
CSpore *CSpore::CreateSporeGrenade( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner )
{
	CSpore *pSpore = GetClassPtr( (CSpore *)NULL );
	UTIL_SetOrigin( pSpore->pev, vecOrigin );
	pSpore->pev->angles = vecAngles;
	pSpore->m_iPrimaryMode = FALSE;
	pSpore->pev->angles = vecAngles;
	pSpore->pev->owner = pOwner->edict();
	pSpore->pev->classname = MAKE_STRING("spore");
	pSpore->Spawn();

	return pSpore;
}

//=========================================================
//=========================================================
CSpore *CSpore::CreateSporeRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner )
{
	CSpore *pSpore = GetClassPtr( (CSpore *)NULL );
	UTIL_SetOrigin( pSpore->pev, vecOrigin );
	pSpore->pev->angles = vecAngles;
	pSpore->m_iPrimaryMode = TRUE;
	pSpore->pev->angles = vecAngles;
	pSpore->pev->owner = pOwner->edict();
	pSpore->pev->classname = MAKE_STRING("spore");
	pSpore->Spawn();
 	pSpore->pev->angles = vecAngles;

	return pSpore;
}
//=========================================================
//=========================================================

void CSpore :: Spawn( void )
{
	Precache( );
	// motor
	if (m_iPrimaryMode)
		pev->movetype = MOVETYPE_FLY;
	else
		pev->movetype = MOVETYPE_BOUNCE;

	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/spore.mdl");
	UTIL_SetSize(pev, Vector( -4, -4, -4), Vector(4, 4, 4));
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_MakeVectors( pev->angles );

	pev->gravity = 0.5;
	Glow ();


	if (m_iPrimaryMode)
	{
		SetThink( &CSpore::FlyThink );
		SetTouch( &CSpore::ExplodeThink );
		pev->velocity = gpGlobals->v_forward * 250;
	}
	else
	{
		SetThink( &CSpore::FlyThink );
		SetTouch( &CSpore::BounceThink );
	}

	pev->dmgtime = gpGlobals->time + 2;
	pev->nextthink = gpGlobals->time + 0.1;

	pev->dmg = gSkillData.sk_plr_spore;
}
//=========================================================

void CSpore :: Precache( void )
{
	PRECACHE_MODEL("models/spore.mdl");
	m_iDrips = PRECACHE_MODEL("sprites/tinyspit.spr");
	m_iGlow = PRECACHE_MODEL("sprites/glow02.spr");
	m_iExplode = PRECACHE_MODEL ("sprites/spore_exp_01.spr");
	m_iExplodeC = PRECACHE_MODEL ("sprites/spore_exp_c_01.spr");
	PRECACHE_SOUND ("weapons/splauncher_impact.wav");
	PRECACHE_SOUND ("weapons/spore_hit1.wav");//Bounce grenade
	PRECACHE_SOUND ("weapons/spore_hit2.wav");//Bounce grenade
	PRECACHE_SOUND ("weapons/spore_hit3.wav");//Bounce grenade
}
//=========================================================

void CSpore :: Glow( void )
{
	m_pSprite = CSprite::SpriteCreate( "sprites/glow02.spr", pev->origin, FALSE );
	m_pSprite->SetAttachment( edict(), 0 );
	m_pSprite->pev->scale = 0.75;
	m_pSprite->SetTransparency( kRenderTransAdd, 150, 158, 19, 155, kRenderFxNoDissipation );
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
}
//=========================================================

void CSpore :: FlyThink( void  )
{
	pev->nextthink = gpGlobals->time + 0.001;
 
	TraceResult tr;
 
	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT( pev ), &tr );

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SPRITE_SPRAY );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x + RANDOM_LONG(-5,5));	// Send to PAS because of the sound
		WRITE_COORD( pev->origin.y + RANDOM_LONG(-5,5) );
		WRITE_COORD( pev->origin.z + RANDOM_LONG(-5,5));
		WRITE_COORD( tr.vecPlaneNormal.x );
		WRITE_COORD( tr.vecPlaneNormal.y );
		WRITE_COORD( tr.vecPlaneNormal.z );
		WRITE_SHORT( m_iDrips );
		WRITE_BYTE( 1  ); // count
		WRITE_BYTE( 18  ); // speed
		WRITE_BYTE( 1000 );
	MESSAGE_END();

	if (!m_iPrimaryMode)
	{
		if (pev->dmgtime <= gpGlobals->time)
			Explode ();
	}
}
//=========================================================

void CSpore::BounceThink(  CBaseEntity *pOther  )
{
	if ( pOther->pev->flags & FL_MONSTER || pOther->IsPlayer()  )
	{
		if ( !FClassnameIs( pOther->pev, "monster_maria" ) 
			&& !FClassnameIs( pOther->pev, "monster_boris" ) )
		Explode ();
	}

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( m_pSprite );
		UTIL_Remove( this );
		return;
	}

#ifndef CLIENT_DLL
	// only do damage if we're moving fairly fast
	if (m_flNextAttack < gpGlobals->time && pev->velocity.Length() > 100)
	{
		entvars_t *pevOwner = VARS( pev->owner );
		if (pevOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace( );
			ClearMultiDamage( );
			pOther->TraceAttack(pevOwner, 1, gpGlobals->v_forward, &tr, DMG_GENERIC ); 
			ApplyMultiDamage( pev, pevOwner);
		}
		m_flNextAttack = gpGlobals->time + 1.0; // debounce
	}
#endif

	Vector vecTestVelocity;

	vecTestVelocity = pev->velocity; 
	vecTestVelocity.z *= 0.45;

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8;

		pev->sequence = RANDOM_LONG( 1, 1 );
	}
	else
	{
		// play bounce sound
		BounceSound();
	}
	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;

}
//=========================================================

void CSpore :: BounceSound( void )
{
	switch (RANDOM_LONG (0, 2))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit1.wav", 0.25, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit2.wav", 0.25, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/spore_hit3.wav", 0.25, ATTN_NORM); break;
	}
}
//=========================================================

void CSpore::ExplodeThink(  CBaseEntity *pOther  )
{
	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( m_pSprite );
		UTIL_Remove( this );
		return;
	}

	if ( !FClassnameIs( pOther->pev, "monster_maria" ) 
		|| !FClassnameIs( pOther->pev, "monster_boris" ) )
		return;

	Explode ();
}
//=========================================================

void CSpore::Explode( void )
{
	SetTouch( NULL );
	SetThink( NULL );
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/splauncher_impact.wav", 1, ATTN_NORM);

	/*

	TraceResult tr;
	if ( CVAR_GET_FLOAT("r_particle" ) == 0 )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE );		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			switch ( RANDOM_LONG( 0, 1 ) )
			{
				case 0:	
					WRITE_SHORT( m_iExplode );
					break;

				default:
				case 1:
					WRITE_SHORT( m_iExplodeC );
					break;
			}
			WRITE_BYTE( 25  ); // scale * 10
			WRITE_BYTE( 155  ); // framerate
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SPRITE_SPRAY );		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( tr.vecPlaneNormal.x );
			WRITE_COORD( tr.vecPlaneNormal.y );
			WRITE_COORD( tr.vecPlaneNormal.z );
			WRITE_SHORT( m_iDrips );
			WRITE_BYTE( 50  ); // count
			WRITE_BYTE( 30  ); // speed
			WRITE_BYTE( 640 );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
			WRITE_SHORT(0);
			WRITE_BYTE(0);
			WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( 0 );
			WRITE_COORD( 0 );
			WRITE_COORD( 0 );
			WRITE_SHORT(iDefaultSporeExplosion);
		MESSAGE_END();
	}
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD( pev->origin.x );	// X
		WRITE_COORD( pev->origin.y );	// Y
		WRITE_COORD( pev->origin.z );	// Z
		WRITE_BYTE( 12 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 180 );		// g
		WRITE_BYTE( 0 );		// b
		WRITE_BYTE( 20 );		// time * 10
		WRITE_BYTE( 20 );		// decay * 0.1
	MESSAGE_END( );
	*/

    	entvars_t *pevOwner;
	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set


	::RadiusDamage ( pev->origin, pev, pevOwner, pev->dmg, 128, CLASS_PLAYER_BIOWEAPON, DMG_GENERIC );

	if (m_iPrimaryMode)
	{
		TraceResult tr;
		UTIL_TraceLine( pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT( pev ), &tr );
		//UTIL_DecalTrace(&tr, DECAL_SPORESPLAT1 + RANDOM_LONG(0,2));
	}

	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;
	UTIL_Remove ( this );
	UTIL_Remove( m_pSprite );
	m_pSprite = NULL;
}
//=================================
// Opposing Forces spore ammo plant 
// Copyright Demiurge
//=================================
typedef enum
{
	SPOREAMMO_IDLE = 0,
	SPOREAMMO_SPAWNUP,
	SPOREAMMO_SNATCHUP,
	SPOREAMMO_SPAWNDOWN,
	SPOREAMMO_SNATCHDOWN,
	SPOREAMMO_IDLE1,
	SPOREAMMO_IDLE2,
} SPOREAMMO;

LINK_ENTITY_TO_CLASS( ammo_spore, CSporeAmmo );

TYPEDESCRIPTION	CSporeAmmo::m_SaveData[] = 
{
	DEFINE_FIELD( CSporeAmmo, m_flTimeSporeIdle, FIELD_TIME ),
	DEFINE_FIELD( CSporeAmmo, borntime, FIELD_FLOAT ),
};
IMPLEMENT_SAVERESTORE( CSporeAmmo, CBaseEntity );

void CSporeAmmo :: Precache( void )
{
	PRECACHE_MODEL("models/spore_ammo.mdl");
	m_iExplode = PRECACHE_MODEL ("sprites/spore_exp_c_01.spr");
	PRECACHE_SOUND("weapons/spore_ammo.wav");
	UTIL_PrecacheOther ( "spore" );
}
//=========================================================
// Spawn
//=========================================================
void CSporeAmmo :: Spawn( void )
{
	TraceResult tr;
	Precache( );
	SET_MODEL(ENT(pev), "models/spore_ammo.mdl");
	UTIL_SetSize(pev, Vector( -20, -20, -8 ), Vector( 20, 20, 16 ));
	UTIL_SetOrigin( pev, pev->origin );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_FLY;
	pev->framerate		= 1.0;
	pev->animtime		= gpGlobals->time + 0.1;
	borntime = 1;

	pev->sequence = SPOREAMMO_IDLE;
	pev->body = 0;
	SetThink (&CSporeAmmo::BornThink);
	SetTouch (&CSporeAmmo::AmmoTouch);
	
	m_flTimeSporeIdle = gpGlobals->time + 22;
	pev->nextthink = gpGlobals->time + 0.1;
}
	
//=========================================================
// Override all damage
//=========================================================
int CSporeAmmo :: TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	return TRUE;
}

void CSporeAmmo::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (!borntime) // rigth '!borntime'  // blast in anytime 'borntime || !borntime'
	{
		Vector vecSrc = pev->origin + gpGlobals->v_forward * -20;

		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
			WRITE_COORD( vecSrc.x );	// Send to PAS because of the sound
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );
			WRITE_SHORT( m_iExplode );
			WRITE_BYTE( 25  ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NOSOUND );
		MESSAGE_END();

			
		ALERT( at_aiconsole, "angles %f %f %f\n", pev->angles.x, pev->angles.y, pev->angles.z );

				Vector angles = pev->angles + gpGlobals->v_up;
		if (abs(angles.x) != 0)
				angles.x = angles.x + 90;
		if (abs(angles.y) != 0)
				angles.y = angles.y + 90;
		if (abs(angles.z) != 0)
				angles.y = angles.y + 90;
		ALERT( at_aiconsole, "angles %f %f %f\n", angles.x, angles.y, angles.z );

		CSpore *pSpore = CSpore::CreateSporeGrenade( vecSrc, angles, this );
		pSpore->pev->velocity = pSpore->pev->velocity + gpGlobals->v_forward * 1000;

		pev->framerate		= 1.0;
		pev->animtime		= gpGlobals->time + 0.1;
		pev->sequence		= SPOREAMMO_SNATCHDOWN;
		pev->body			= 0;
		borntime			= 1;
		m_flTimeSporeIdle = gpGlobals->time + 1;
		SetThink (&CSporeAmmo::IdleThink);
	}
	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

//=========================================================
// Thinking begin
//=========================================================
void CSporeAmmo :: BornThink ( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if ( m_flTimeSporeIdle > gpGlobals->time )
		return;

	pev->sequence = 3;
	pev->framerate		= 1.0;
	pev->animtime		= gpGlobals->time + 0.1;
	pev->body = 1;
	borntime = 0;
	SetThink (&CSporeAmmo::IdleThink);
	
	m_flTimeSporeIdle = gpGlobals->time + 16;
}

void CSporeAmmo :: IdleThink ( void )
{

		pev->nextthink = gpGlobals->time + 0.1;
	if ( m_flTimeSporeIdle > gpGlobals->time )
		return;

	if (borntime)
	{
		pev->sequence = SPOREAMMO_IDLE;

		m_flTimeSporeIdle = gpGlobals->time + 18;
		SetThink(&CSporeAmmo::BornThink);
		return;
	}
	else
	{
		pev->sequence = SPOREAMMO_IDLE1;
	}
}

void CSporeAmmo :: AmmoTouch ( CBaseEntity *pOther )
{
	Vector		vecSpot;
	TraceResult	tr;

	if ( pOther->pev->velocity == g_vecZero || !pOther->IsPlayer() )
		return;

	if (borntime)
		return;

	int bResult = (pOther->GiveAmmo( 5, "spore", MAX_CARRY_SPORELAUNCHER) != -1);
	if (bResult)
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/spore_ammo.wav", 1, ATTN_NORM);

		pev->framerate		= 1.0;
		pev->animtime		= gpGlobals->time + 0.1;
		pev->sequence = SPOREAMMO_SNATCHDOWN;
		pev->body = 0;
		borntime = 1;
		m_flTimeSporeIdle = gpGlobals->time + 1;
		SetThink (&CSporeAmmo::IdleThink);
	}
}
//=================================
// Opposing Forces shock beam
// Copyright Demiurge, Copyright Highlander
//=================================
LINK_ENTITY_TO_CLASS( shock_beam, CShockBeam );
//=========================================================
//=========================================================
void CShockBeam :: GetAttachment ( int iAttachment, Vector &origin, Vector &angles )
{
	GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
}
CShockBeam *CShockBeam::ShockCreate( void )
{
	// Create a new entity with CShockBeam private data
	CShockBeam *pShock = GetClassPtr( (CShockBeam *)NULL );
	pShock->pev->classname = MAKE_STRING("shock_beam");
	pShock->Spawn();

	return pShock;
}

void CShockBeam::Spawn( )
{
	Precache( );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/shock_effect.mdl");

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	BlastOn();
	Glow ();

	SetTouch( &CShockBeam::ShockTouch );

	if ( g_pGameRules->IsMultiplayer() )
		pev->dmg = gSkillData.sk_plr_shockroachm;
	else
		pev->dmg = gSkillData.sk_plr_shockroachs;

	UTIL_MakeAimVectors( pev->angles );

	m_vecForward = gpGlobals->v_forward;
	pev->gravity = 0.5;

	pev->nextthink = gpGlobals->time + 0.1;
}


void CShockBeam::Precache( )
{
	PRECACHE_MODEL("models/shock_effect.mdl");

	PRECACHE_SOUND( "weapons/shock_fire.wav" );
	PRECACHE_SOUND( "weapons/shock_impact.wav" );

	PRECACHE_MODEL( "sprites/lgtning.spr" );
	PRECACHE_MODEL( "sprites/glow02.spr" );
}

int	CShockBeam :: Classify ( void )
{
	return	CLASS_NONE;
}

void CShockBeam::ShockTouch( CBaseEntity *pOther )
{
	if ( FClassnameIs( pOther->pev, "monster_maria" ) 
		|| FClassnameIs( pOther->pev, "monster_boris" ) )
		return;

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;

		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;

		UTIL_Remove( m_pSprite );
		m_pSprite = NULL;

		UTIL_Remove( this );
		return;
	}

	SetTouch( NULL );
	SetThink( NULL );

	if (pOther->pev->takedamage)
	{
		TraceResult tr = UTIL_GetGlobalTrace( );
		entvars_t	*pevOwner;

		pevOwner = VARS( pev->owner );

		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage( );

		if ( g_pGameRules->IsMultiplayer() )
			pOther->TraceAttack(pevOwner, gSkillData.sk_plr_shockroachm, pev->velocity.Normalize(), &tr, DMG_ENERGYBEAM );
		else
			pOther->TraceAttack(pevOwner, gSkillData.sk_plr_shockroachs, pev->velocity.Normalize(), &tr, DMG_ENERGYBEAM );

		ApplyMultiDamage( pev, pevOwner );

		if ( pOther->pev->flags & FL_MONSTER | pOther->IsPlayer() )
		{
			pOther->pev->renderfx = kRenderFxGlowShell;
			pOther->pev->rendercolor.x = 0; // R
			pOther->pev->rendercolor.y = 255; // G
			pOther->pev->rendercolor.z = 255; // B
			pOther->pev->renderamt = 1;
			pShockedEnt = pOther;
		}

		SetThink ( &CShockBeam::FadeShock );
		pev->nextthink = gpGlobals->time + 1.0;

		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/shock_impact.wav", VOL_NORM, ATTN_NORM);
	}
		UTIL_Sparks( pev->origin );
		ExplodeThink();
}

void CShockBeam::ExplodeThink( void )
{
	int iContents = UTIL_PointContents ( pev->origin );
	int iScale;
	
	BlastOff();
	iScale = 10;

	EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/shock_impact.wav", 1, ATTN_NORM);

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE( 10 );		// radius * 0.1
		WRITE_BYTE( 0 );		// r
		WRITE_BYTE( 255 );		// g
		WRITE_BYTE( 255 );		// b
		WRITE_BYTE( 10 );		// time * 10
		WRITE_BYTE( 10 );		// decay * 0.1
	MESSAGE_END( );


	entvars_t *pevOwner;

	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set

	TraceResult tr;

	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity * 10, dont_ignore_monsters, ENT( pev ), &tr );
	UTIL_DecalTrace(&tr, DECAL_SMALLSCORCH1 + RANDOM_LONG(0,2));
}
void CShockBeam::BlastOff ( void )
{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;

		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;

		UTIL_Remove( m_pSprite );
		m_pSprite = NULL;

}
void CShockBeam::FadeShock ( void )
{
	if ( pShockedEnt )
	{
		pShockedEnt->pev->renderfx = kRenderFxNone;
		pShockedEnt->pev->rendercolor.x = 0; // R
		pShockedEnt->pev->rendercolor.y = 0; // G
		pShockedEnt->pev->rendercolor.z = 0; // B
		pShockedEnt->pev->renderamt = 255;
	}
	UTIL_Remove( this );
}
void CShockBeam :: Glow( void )
{
	Vector		posGun, angleGun;
	int m_iAttachment = 1;
	GetAttachment( m_iAttachment, posGun, angleGun );

	m_pSprite = CSprite::SpriteCreate( "sprites/glow02.spr", pev->origin, FALSE );
	m_pSprite->SetAttachment( edict(), m_iAttachment );
	m_pSprite->pev->scale = 0.25;
	m_pSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 125, kRenderFxNoDissipation );
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;
	m_pSprite->pev->flags |= FL_SKIPLOCALHOST;
}
void CShockBeam::BlastOn ( void )
{
	Vector		posGun, angleGun;
	TraceResult trace;

	UTIL_MakeVectors( pev->angles );
	
	m_pBeam = CBeam::BeamCreate( "sprites/lgtning.spr", 30 );

	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	Vector vecEnd = (gpGlobals->v_forward * 40) + posGun;
	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pBeam->EntsInit( entindex(), entindex() );
	m_pBeam->SetStartAttachment( 1 );
	m_pBeam->SetEndAttachment( 2 );
	m_pBeam->SetBrightness( 190 );
	m_pBeam->SetScrollRate( 20 );
	m_pBeam->SetNoise( 20 );
	m_pBeam->DamageDecal( 1 );
	m_pBeam->SetFlags( BEAM_FSHADEOUT );
	m_pBeam->SetColor( 0, 255, 255 );
	m_pBeam->pev->spawnflags = SF_BEAM_TEMPORARY;

	m_pNoise = CBeam::BeamCreate( "sprites/lgtning.spr", 30 );

	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pNoise->EntsInit( entindex(), entindex() );
	m_pNoise->SetStartAttachment( 1 );
	m_pNoise->SetEndAttachment( 2 );
	m_pNoise->SetBrightness( 190 );
	m_pNoise->SetScrollRate( 20 );
	m_pNoise->SetNoise( 65 );
	m_pNoise->DamageDecal( 1 );
	m_pNoise->SetFlags( BEAM_FSHADEOUT );
	m_pNoise->SetColor( 255, 255, 173 );
	m_pNoise->pev->spawnflags = SF_BEAM_TEMPORARY;

	//EXPORT RelinkBeam();
	//EXPORT CBeam::RelinkBeam();
}
//=================================================
// Displacer Targets	
//=================================================
// Xen Target
LINK_ENTITY_TO_CLASS( info_displacer_xen_target, CPointEntity );
// Earth Target
LINK_ENTITY_TO_CLASS( info_displacer_earth_target, CPointEntity );

//=================================================
// Displacer Ball	
//=================================================

LINK_ENTITY_TO_CLASS( displacer_ball, CDisplacerBall );

CDisplacerBall *CDisplacerBall::BallCreate( void )
{
	CDisplacerBall *pBall = GetClassPtr( (CDisplacerBall *)NULL );
	pBall->pev->classname = MAKE_STRING("displacer_ball");
	pBall->m_fIsSelfTeleporter = FALSE;
	pBall->Spawn();

	return pBall;
}
CDisplacerBall *CDisplacerBall::SelfCreate( void )
{
	CDisplacerBall *pBall = GetClassPtr( (CDisplacerBall *)NULL );
	pBall->pev->classname = MAKE_STRING("displacer_ball");
	pBall->m_fIsSelfTeleporter = TRUE;
	pBall->Spawn();
	pBall->SelfRemove();

	return pBall;
}
void CDisplacerBall::Spawn( )
{
	Precache( );

	if ( !m_fIsSelfTeleporter )
	{
		pev->movetype = MOVETYPE_FLY;
		pev->solid = SOLID_BBOX;
		pev->velocity = gpGlobals->v_forward * DISPLACER_BALL_SPEED;
	}

	SET_MODEL(ENT(pev), "sprites/exit1.spr");
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
	if ( !m_fIsSelfTeleporter )
	{
		SetTouch( &CDisplacerBall::BallTouch );
		SetThink( &CDisplacerBall::FlyThink );
		pev->nextthink = gpGlobals->time + 0.05;
	}
}


void CDisplacerBall::Precache( )
{
	PRECACHE_MODEL("sprites/exit1.spr");
	m_iBeamSprite = PRECACHE_MODEL("sprites/lgtning.spr");
	PRECACHE_MODEL("sprites/xflare1.spr");

	PRECACHE_SOUND("weapons/displacer_teleport.wav");
	PRECACHE_SOUND("weapons/displacer_teleport_player.wav");
	PRECACHE_SOUND("weapons/displacer_impact.wav");

	m_iDispRing = PRECACHE_MODEL ("sprites/disp_ring.spr");
}
int	CDisplacerBall :: Classify ( void )
{
	return	CLASS_PLAYER_BIOWEAPON;
}
void CDisplacerBall :: BallTouch ( CBaseEntity *pOther )
{
	TraceResult tr;
	Vector		vecSpot;
	Vector		vecSrc;
	pev->enemy = pOther->edict();
	CBaseEntity *pTarget = NULL;
	vecSpot = pev->origin - pev->velocity.Normalize() * 32;

	if ( FClassnameIs( pOther->pev, "monster_maria" ) 
		|| FClassnameIs( pOther->pev, "monster_boris" ) )
		return;

	if ( UTIL_PointContents(pev->origin) == CONTENT_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	UTIL_TraceLine( vecSpot, vecSpot + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr );
	if( !LockRing )
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 840);
			WRITE_SHORT( m_iDispRing );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 10 );
			WRITE_BYTE( 3 );
			WRITE_BYTE( 20 );
			WRITE_BYTE( 0 );
			WRITE_BYTE( 255 );
			WRITE_BYTE( 255 );
			WRITE_BYTE( 255 );
			WRITE_BYTE( 255 );
			WRITE_BYTE( 0 );
		MESSAGE_END();

		if ( g_pGameRules->IsMultiplayer() )
		{
			if ( pOther->IsPlayer() )
			{
				for ( int i = RANDOM_LONG(1,5); i > 0; i-- )
				pTarget = UTIL_FindEntityByClassname(pTarget, "info_player_deathmatch" );

				if (pTarget)
				{
					UTIL_ScreenFade( pOther, Vector(0, 160, 0), 0.5, 0.5, 255, FFADE_IN );

					Vector tmp = pTarget->pev->origin;
					tmp.z -= pOther->pev->mins.z;
					tmp.z++;
					UTIL_SetOrigin( pOther->pev, tmp );

					pOther->pev->angles = pTarget->pev->angles;
					pOther->pev->velocity = pOther->pev->basevelocity = g_vecZero;

					CSprite *pSpr = CSprite::SpriteCreate( "sprites/xflare1.spr", vecSrc, TRUE );
					pSpr->AnimateAndDie( 6 );
					pSpr->SetTransparency(kRenderGlow, 184, 250, 214, 255, kRenderFxNoDissipation);

					EMIT_SOUND(ENT(pOther->pev), CHAN_WEAPON, "weapons/displacer_teleport_player.wav", 1, ATTN_NORM);
					
					vecSrc = pTarget->pev->origin;
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
						WRITE_BYTE(TE_DLIGHT);
						WRITE_COORD( vecSrc.x );	// X
						WRITE_COORD( vecSrc.y );	// Y
						WRITE_COORD( vecSrc.z );	// Z
						WRITE_BYTE( 24 );		// radius * 0.1
						WRITE_BYTE( 255 );		// r
						WRITE_BYTE( 180 );		// g
						WRITE_BYTE( 96 );		// b
						WRITE_BYTE( 20 );		// time * 10
						WRITE_BYTE( 0 );		// decay * 0.1
					MESSAGE_END( );
				}
			}
		}
		else if ( pOther->pev->flags & FL_MONSTER )
		{
			if ( pOther->pev->health <= 200 && !FClassnameIs( pOther->pev, "monster_nihilanth" )
				&& !FClassnameIs( pOther->pev, "monster_apache" ) && !FClassnameIs( pOther->pev, "monster_osprey" )
				&& !FClassnameIs( pOther->pev, "monster_superapache" ) && !FClassnameIs( pOther->pev, "monster_gargantua" )
				&& !FClassnameIs( pOther->pev, "monster_bigmomma" ))
			{
				pOther->Killed( pev, GIB_NEVER );
				pRemoveEnt = pOther;
			}
			else
			{
				pOther->TakeDamage( pev, pev, 100, DMG_ENERGYBEAM | DMG_NEVERGIB );
			}

		}

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(pev->origin.x);	// X
			WRITE_COORD(pev->origin.y);	// Y
			WRITE_COORD(pev->origin.z);	// Z
			WRITE_BYTE( 32 );		// radius * 0.1
			WRITE_BYTE( 255 );		// r
			WRITE_BYTE( 180 );		// g
			WRITE_BYTE( 96 );		// b
			WRITE_BYTE( 60 );		// time * 10
			WRITE_BYTE( 20 );		// decay * 0.1
		MESSAGE_END( );

		SetThink ( &CDisplacerBall::ExplodeThink );
		pev->nextthink = gpGlobals->time + 0.6;
	}

	LockRing = TRUE;
	pev->velocity = g_vecZero;
}
void CDisplacerBall :: SelfRemove ( void )
{
	pev->velocity = g_vecZero;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z);
		WRITE_COORD( pev->origin.x);
		WRITE_COORD( pev->origin.y);
		WRITE_COORD( pev->origin.z + 640);
		WRITE_SHORT( m_iDispRing );
		WRITE_BYTE( 0 );	// startframe
		WRITE_BYTE( 10 );	// framerate
		WRITE_BYTE( 3 );	// life
		WRITE_BYTE( 20 );	// width
		WRITE_BYTE( 0 );	// noise
		WRITE_BYTE( 255 );	// r, g, b
		WRITE_BYTE( 255 );	// r, g, b
		WRITE_BYTE( 255 );	// r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 0 );	// speed
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(pev->origin.x);	// X
		WRITE_COORD(pev->origin.y);	// Y
		WRITE_COORD(pev->origin.z);	// Z
		WRITE_BYTE( 32 );		// radius * 0.1
		WRITE_BYTE( 255 );		// r
		WRITE_BYTE( 180 );		// g
		WRITE_BYTE( 96 );		// b
		WRITE_BYTE( 60 );		// time * 10
		WRITE_BYTE( 20 );		// decay * 0.1
	MESSAGE_END( );

	SetThink ( &CDisplacerBall::ExplodeThink );
	pev->nextthink = gpGlobals->time + 0.3;
}
void CDisplacerBall::FlyThink( void )
{
	CBeam *pBeam;
	TraceResult tr;
	Vector vecDest;
	float flDist = 1.0;

	for (int i = 0; i < 10; i++)
	{
		Vector vecDir = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir = vecDir.Normalize();
		TraceResult         tr1;
		UTIL_TraceLine( pev->origin, pev->origin + vecDir * 1024, ignore_monsters, ENT(pev), &tr1 );
		if (flDist > tr1.flFraction)
		{
			tr = tr1;
			flDist = tr.flFraction;
		}
	};

	if ( flDist == 1.0 ) return;

	pBeam = CBeam::BeamCreate("sprites/lgtning.spr",200);
	pBeam->PointEntInit( tr.vecEndPos, entindex() );
	pBeam->SetStartPos( tr.vecEndPos );
	pBeam->SetEndEntity( entindex() );
	pBeam->SetColor( 96, 128, 16 );
	pBeam->SetNoise( 65 );
	pBeam->SetBrightness( 255 );
	pBeam->SetWidth( 30 );
	pBeam->SetScrollRate( 35 );
	pBeam->LiveForTime( 1 );

	pev->frame += 1; //animate teleball

	if(pev->frame > 24)
		pev->frame = fmod( pev->frame, 24 );

	pev->nextthink = gpGlobals->time + 0.05;
}

void CDisplacerBall::ExplodeThink( void )
{
	pev->effects |= EF_NODRAW;

	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/displacer_teleport.wav", VOL_NORM, ATTN_NORM);

	if ( pRemoveEnt )
	{
		UTIL_Remove( pRemoveEnt );
	}

	entvars_t *pevOwner;
	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;
		pev->owner = NULL;

	UTIL_Remove( this );
	::RadiusDamage( pev->origin, pev, pevOwner, gSkillData.sk_plr_displacer_other, gSkillData.sk_plr_displacer_radius , CLASS_NONE, DMG_ENERGYBEAM );
}

/*
//=======================
// Opposing Force Rope
//=======================

LINK_ENTITY_TO_CLASS( env_rope, CRope );
void CRope :: Spawn ( void )
{
	pev->effects |= EF_NODRAW;
	Precache();
	Initialize();
}
void CRope :: Precache ( void )
{
	//PRECACHE_MODEL( (char *)STRING(m_BodyModel) );
	//PRECACHE_MODEL( (char *)STRING(m_EndingModel) );
	PRECACHE_MODEL( "models/wire_blue8.mdl" );
	PRECACHE_MODEL( "models/wire_copper16.mdl" );
}
void CRope :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "segments"))
	{
		m_iSegments = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "bodymodel"))
	{
		m_BodyModel = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "endingmodel"))
	{
		m_EndingModel = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "disable"))
	{
		m_fDisabled = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}
void CRope :: Initialize ( void )
{
	vecAngles.z = 270;
	BOOL m_fElectrified = FALSE;
	CRopeSample *pSegment = CRopeSample::CreateSample( pev->origin, vecAngles, m_BodyModel,  m_EndingModel, m_fDisabled, m_iSegments, m_fElectrified, this );
}
//=======================
// Opposing Force Rope Sample
//=======================
CRopeSample *CRopeSample::CreateSample( Vector vecOrigin, Vector vecAngles, string_t m_BodyModel, string_t m_EndingModel, BOOL m_fDisabled, int m_iSegments, BOOL m_fElectrified, CBaseEntity *pOwner )
{
	CRopeSample *pSegment = GetClassPtr( (CRopeSample *)NULL );

	UTIL_SetOrigin( pSegment->pev, vecOrigin );
	pSegment->pev->angles = vecAngles;
	pSegment->pev->owner = pOwner->edict();
	pSegment->m_Model = m_BodyModel;
	pSegment->m_EndingModel = m_EndingModel;
	pSegment->m_iSegments = m_iSegments;
	pSegment->m_fDisabled = m_fDisabled;
	pSegment->m_fElectrified = m_fElectrified;
	pSegment->m_iSegments = m_iSegments;
	pSegment->Spawn();

	return pSegment;
}
void CRopeSample :: Spawn ( void )
{
	Precache();
	Initialize();
	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BBOX;
	pev->gravity = 1;

	//if ( m_iSegments == 0 ) 
	//	SET_MODEL(ENT(pev), STRING( m_EndingModel ));
	//else
	//	SET_MODEL(ENT(pev), STRING( m_Model ));

	if ( m_iSegments == 0 ) 
		SET_MODEL(ENT(pev), "models/wire_copper16.mdl");
	else
		SET_MODEL(ENT(pev), "models/wire_blue8.mdl");

	SetTouch ( RopeTouch );

	UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	if (m_iSegments != 0)
	{
		CreateSample();
	}
	else
	{
		SetThink ( RopeThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}
void CRopeSample :: Initialize ( void )
{
}
void CRopeSample :: RopeTouch ( CBaseEntity *pOther )
{
	Vector vecEnd, vecSrc, vecAngles;
	TraceResult		tr1;

	if ( pOther->IsPlayer() )
	{
		GET_ATTACHMENT( ENT(pev), 0, vecEnd, NULL );
		UTIL_TraceLine( vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr1 );

		pOther->pev->flags = PFLAG_ON_ROPE;
	}
	else
	{
		GET_ATTACHMENT( ENT(pev), 0, vecEnd, NULL );
		UTIL_TraceLine( vecSrc, vecSrc + vecEnd, ignore_monsters, ENT(pev), &tr1 );
	}
}
void CRopeSample :: RopeThink ( void )
{
	Vector vecSrc, vecAngles;
	entvars_t *pevOwner = VARS( pev->owner );
	if ( pevOwner )
	{
		if ( FClassnameIs( pevOwner, "env_rope" ) )
		{
			pev->origin = pevOwner->origin;
		}
		else
		{
			GET_ATTACHMENT( ENT(pevOwner), 0, vecSrc, NULL );
			pev->origin = vecSrc;

			pev->nextthink = gpGlobals->time + 0.1;
		}
	}
}
void CRopeSample :: Precache ( void )
{
	PRECACHE_SOUND( "items/rope1.wav" );
	PRECACHE_SOUND( "items/rope2.wav" );
	PRECACHE_SOUND( "items/rope3.wav" );

	PRECACHE_MODEL("sprites/xflare1.spr");

	PRECACHE_SOUND( "items/grab_rope.wav" );

	//PRECACHE_MODEL( (char *)STRING(m_Model) );
	//PRECACHE_MODEL( (char *)STRING(m_EndingModel) );

	PRECACHE_MODEL( "models/wire_copper16.mdl" );
	PRECACHE_MODEL( "models/wire_blue8.mdl" );
}
void CRopeSample :: GetAttachment ( int iAttachment, Vector &origin, Vector &angles )
{
	GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
}
void CRopeSample :: CreateSample( void )
{
	Vector vecSrc, vecAngles;
	m_iSegments --;

	GetAttachment ( 0, vecSrc, vecAngles );
	vecAngles.z = 270;
	CRopeSample *pSegment = CRopeSample::CreateSample( vecSrc, vecAngles, m_Model, m_EndingModel, m_fDisabled, m_iSegments, m_fElectrified, this );

	SetThink ( RopeThink );
	pev->nextthink = gpGlobals->time + 0.1;
}
LINK_ENTITY_TO_CLASS( rope_sample, CRopeSample );

//=======================
// info_savetitle - Contains the chapter title
//=======================
LINK_ENTITY_TO_CLASS( info_savetitle, CPointEntity );

//=========================================================
// Trigger to disable a player
//=========================================================

class CPlayerFreeze : public CBaseDelay
{
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

void CPlayerFreeze::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pPlayer;
	pPlayer = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex( 1 ));

	if (pPlayer && pPlayer->pev->flags & FL_CLIENT)
	{
		if (pPlayer->pev->flags & FL_FROZEN)
		{
			// unfreeze him
			((CBasePlayer *)pPlayer)->EnableControl(TRUE);
		}
		else
		{
			// freeze him
			((CBasePlayer *)pPlayer)->EnableControl(FALSE);
		}
	}
}

LINK_ENTITY_TO_CLASS( trigger_playerfreeze, CPlayerFreeze );


//===============================================
// Countdown Timer - I'm moving the countdown to
// the player, so I can make sure it works fine.
// I need the timer to be global, and stable.
// It must work for the reactor area, and the ship,
// and of course the elevators
//===============================================
#define SF_TIMER_RUNNING 1

class CTimer : public CPointEntity
{
public:
	void Spawn( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void KeyValue( KeyValueData *pkvd );

	float m_flTime;
	BOOL m_fTimerActive;
};

LINK_ENTITY_TO_CLASS( game_timer, CTimer );

TYPEDESCRIPTION	CTimer::m_SaveData[] = 
{
	DEFINE_FIELD( CTimer, m_flTime, FIELD_TIME ),
	DEFINE_FIELD( CTimer, m_fTimerActive, FIELD_BOOLEAN ),
};
IMPLEMENT_SAVERESTORE( CTimer, CPointEntity );

void CTimer :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "time"))
	{
		m_flTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
};
void CTimer::Spawn( void )
{
	pev->solid = SOLID_NOT;	// Remove model & collision
	pev->effects |= EF_NODRAW;

	if ( m_flTime <= 0 )
		m_flTime = 15;

	if ( pev->spawnflags & SF_TIMER_RUNNING ) // A timer is already running on the player, we need to deactivate it when we are triggered
		m_fTimerActive = TRUE;

};
void CTimer::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pPlayer;
	pPlayer = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex( 1 ));

	if ( !m_fTimerActive )
	{
		((CBasePlayer *)pPlayer)->InitCountdown( m_flTime, TRUE, pev->target, this );
		m_fTimerActive = TRUE;
	}
	else
	{
		((CBasePlayer *)pPlayer)->InitCountdown( NULL, FALSE, NULL, NULL ); // Turn the timer off
		m_fTimerActive = FALSE;
	}
};
*/