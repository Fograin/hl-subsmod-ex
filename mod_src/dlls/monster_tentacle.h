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
#ifndef MONSTER_ALIEN_S_H
#define MONSTER_ALIEN_S_H

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"weapons.h"
#include	"soundent.h"

#define ACT_T_IDLE		1010
#define ACT_T_TAP			1020
#define ACT_T_STRIKE		1030
#define ACT_T_REARIDLE	1040

// stike sounds
#define TE_NONE -1
#define TE_SILO 0
#define TE_DIRT 1
#define TE_WATER 2


class CTentacle : public CBaseMonster
{
public:
	CTentacle( void );

	void Spawn( );
	void Precache( );
	void KeyValue( KeyValueData *pkvd );

	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	// Don't allow the tentacle to go across transitions!!!
	virtual int	ObjectCaps( void ) { return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void SetObjectCollisionBox( void )
	{
		pev->absmin = pev->origin + Vector(-400, -400, 0);
		pev->absmax = pev->origin + Vector(400, 400, 850);
	}

	void EXPORT Cycle( void );
	void EXPORT CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT Start( void );
	void EXPORT DieThink( void );

	void EXPORT Test( void );

	void EXPORT HitTouch( CBaseEntity *pOther );

	float HearingSensitivity( void ) { return 2.0; };

	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void Killed( entvars_t *pevAttacker, int iGib );

	MONSTERSTATE GetIdealState ( void ) { return MONSTERSTATE_IDLE; };
	int CanPlaySequence( BOOL fDisregardState ) { return TRUE; };

	int Classify( void );

	int Level( float dz );
	int MyLevel( void );
	float MyHeight( void );

	float m_flInitialYaw;
	int m_iGoalAnim;
	int m_iLevel;
	int m_iDir;
	float m_flFramerateAdj;
	float m_flSoundYaw;
	int m_iSoundLevel;
	float m_flSoundTime;
	float m_flSoundRadius;
	int m_iHitDmg;
	float m_flHitTime;

	float m_flTapRadius;

	float m_flNextSong;
	static int g_fFlySound;
	static int g_fSquirmSound;

	float m_flMaxYaw;
	int m_iTapSound;

	Vector m_vecPrevSound;
	float m_flPrevSoundTime;

	static const char *pHitSilo[];
	static const char *pHitDirt[];
	static const char *pHitWater[];
};

class CTentacleMaw : public CBaseMonster
{
public:
	void Spawn( );
	void Precache( );
};




#endif // MONSTER_ALIEN_S_H