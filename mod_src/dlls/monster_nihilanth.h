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
#ifndef MONSTER_NIHILANTH_H
#define MONSTER_NIHILANTH_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"

#define N_SCALE		15
#define N_SPHERES	20







class CNihilanth : public CBaseMonster
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int  Classify( void ) { return CLASS_ALIEN_MILITARY; };
	int  BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );
	void GibMonster( void );

	void SetObjectCollisionBox( void )
	{
		pev->absmin = pev->origin + Vector( -16 * N_SCALE, -16 * N_SCALE, -48 * N_SCALE );
		pev->absmax = pev->origin + Vector( 16 * N_SCALE, 16 * N_SCALE, 28 * N_SCALE );
	}

	void HandleAnimEvent( MonsterEvent_t *pEvent );

	void EXPORT StartupThink( void );
	void EXPORT HuntThink( void );
	void EXPORT CrashTouch( CBaseEntity *pOther );
	void EXPORT DyingThink( void );
	void EXPORT StartupUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT NullThink( void );
	void EXPORT CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void FloatSequence( void );
	void NextActivity( void );

	void Flight( void );

	BOOL AbsorbSphere( void );
	BOOL EmitSphere( void );
	void TargetSphere( USE_TYPE useType, float value );
	CBaseEntity *RandomTargetname( const char *szName );
	void ShootBalls( void );
	void MakeFriend( Vector vecPos );
	
	int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	void PainSound( void );
	void DeathSound( void );

	static const char *pAttackSounds[];	// vocalization: play sometimes when he launches an attack
	static const char *pBallSounds[];	// the sound of the lightening ball launch
	static const char *pShootSounds[];	// grunting vocalization: play sometimes when he launches an attack
	static const char *pRechargeSounds[];	// vocalization: play when he recharges
	static const char *pLaughSounds[];	// vocalization: play sometimes when hit and still has lots of health
	static const char *pPainSounds[];	// vocalization: play sometimes when hit and has much less health and no more chargers
	static const char *pDeathSounds[];	// vocalization: play as he dies
	
	// x_teleattack1.wav	the looping sound of the teleport attack ball.

	float m_flForce;

	float m_flNextPainSound;

	Vector m_velocity;
	Vector m_avelocity;

	Vector m_vecTarget;
	Vector m_posTarget;

	Vector m_vecDesired;
	Vector m_posDesired;

	float  m_flMinZ;
	float  m_flMaxZ;

	Vector m_vecGoal;

	float m_flLastSeen;
	float m_flPrevSeen;

	int m_irritation;

	int m_iLevel;
	int m_iTeleport;

	EHANDLE m_hRecharger;

	EHANDLE m_hSphere[N_SPHERES];
	int	m_iActiveSpheres;

	float m_flAdj;

	CSprite *m_pBall;

	char m_szRechargerTarget[64];
	char m_szDrawUse[64];
	char m_szTeleportUse[64];
	char m_szTeleportTouch[64];
	char m_szDeadUse[64];
	char m_szDeadTouch[64];

	float m_flShootEnd;
	float m_flShootTime;

	EHANDLE m_hFriend[3];
};


class CNihilanthHVR : public CBaseMonster
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );

	void CircleInit( CBaseEntity *pTarget );
	void AbsorbInit( void );
	void TeleportInit( CNihilanth *pOwner, CBaseEntity *pEnemy, CBaseEntity *pTarget, CBaseEntity *pTouch );
	void GreenBallInit( void );
	void ZapInit( CBaseEntity *pEnemy );

	void EXPORT HoverThink( void );
	BOOL CircleTarget( Vector vecTarget );
	void EXPORT DissipateThink( void );

	void EXPORT ZapThink( void );
	void EXPORT TeleportThink( void );
	void EXPORT TeleportTouch( CBaseEntity *pOther );
	
	void EXPORT RemoveTouch( CBaseEntity *pOther );
	void EXPORT BounceTouch( CBaseEntity *pOther );
	void EXPORT ZapTouch( CBaseEntity *pOther );

	CBaseEntity *RandomClassname( const char *szName );

	// void EXPORT SphereUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void MovetoTarget( Vector vecTarget );
	virtual void Crawl( void );

	void Zap( void );
	void Teleport( void );

	float m_flIdealVel;
	Vector m_vecIdeal;
	CNihilanth *m_pNihilanth;
	EHANDLE m_hTouch;
	int m_nFrames;
};


#endif // MONSTER_NIHILANTH_H