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
#ifndef MONSTER_BARNACLE_H
#define MONSTER_BARNACLE_H

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"ai_schedule.h"


//==================//
// monster_barnacle
//==================//
#define	BARNACLE_BODY_HEIGHT		44 // how 'tall' the barnacle's model is.
#define BARNACLE_PULL_SPEED			8
#define BARNACLE_KILL_VICTIM_DELAY	5 // how many seconds after pulling prey in to gib them. 

// Monster's Anim Events Go Here
#define	BARNACLE_AE_PUKEGIB	2

class CBarnacle : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	CBaseEntity *TongueTouchEnt ( float *pflLength );
	int  Classify ( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void EXPORT BarnacleThink ( void );
	void EXPORT WaitTillDead ( void );
	void Killed( entvars_t *pevAttacker, int iGib );
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	float m_flAltitude;
	float m_flCachedLength;	// tongue cached length
	float m_flKillVictimTime;
	int   m_cGibs;		// barnacle loads up on gibs each time it kills something.
	BOOL  m_fTongueExtended;
	BOOL  m_fLiftingPrey;
	float m_flTongueAdj;

	// FIXME: need a custom barnacle model with non-generic hitgroup
	// otherwise we can apply to damage to tongue instead of body
#ifdef BARNACLE_FIX_VISIBILITY
	void SetObjectCollisionBox( void )
	{
		pev->absmin = pev->origin + Vector( -16, -16, -m_flCachedLength );
		pev->absmax = pev->origin + Vector( 16, 16, 0 );
	}
#endif
};


#endif // MONSTER_BARNACLE_H