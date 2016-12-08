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
/*
  Header file for monster-related utility code
*/
#ifndef MONSTERS_H
#include "skill.h"
#define MONSTERS_H


// CHECKLOCALMOVE result types 
#define	LOCALMOVE_INVALID					0 // move is not possible
#define LOCALMOVE_INVALID_DONT_TRIANGULATE	1 // move is not possible, don't try to triangulate
#define LOCALMOVE_VALID						2 // move is possible

// Hit Group standards
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7

// Fograin92
#define	SF_MONSTER_SPAWNFLAG_1		    1
#define	SF_MONSTER_SPAWNFLAG_2			2
#define SF_MONSTER_SPAWNFLAG_4			4
#define SF_MONSTER_SPAWNFLAG_8			8
#define SF_MONSTER_SPAWNFLAG_16			16
#define SF_MONSTER_SPAWNFLAG_32			32
#define SF_MONSTER_SPAWNFLAG_64			64
#define SF_MONSTER_SPAWNFLAG_128		128
#define SF_MONSTER_SPAWNFLAG_256		256
#define SF_MONSTER_SPAWNFLAG_512		512
#define SF_MONSTER_SPAWNFLAG_1024		1024
#define SF_MONSTER_SPAWNFLAG_2048		2048
#define SF_MONSTER_SPAWNFLAG_4096		4096
#define SF_MONSTER_SPAWNFLAG_8192		8192
#define SF_MONSTER_SPAWNFLAG_16384		16384
#define SF_MONSTER_SPAWNFLAG_32768		32768
#define SF_MONSTER_SPAWNFLAG_65536		65536
#define SF_MONSTER_SPAWNFLAG_131072		131072
#define SF_MONSTER_SPAWNFLAG_262144		262144
#define SF_MONSTER_SPAWNFLAG_524288		524288
#define SF_MONSTER_SPAWNFLAG_1048576	1048576
#define SF_MONSTER_SPAWNFLAG_2097152	2097152
#define SF_MONSTER_SPAWNFLAG_4194304	4194304
#define SF_MONSTER_SPAWNFLAG_8388608	8388608


// Monster Spawnflags
#define	SF_MONSTER_WAIT_TILL_SEEN		1// spawnflag that makes monsters wait until player can see them before attacking.
#define	SF_MONSTER_GAG					2 // no idle noises from this monster
#define SF_MONSTER_HITMONSTERCLIP		4
//										8
#define SF_MONSTER_PRISONER				16 // monster won't attack anyone, no one will attacke him.
//										32
//										64
#define	SF_MONSTER_WAIT_FOR_SCRIPT		128 //spawnflag that makes monsters wait to check for attacking until the script is done or they've been attacked
#define SF_MONSTER_PREDISASTER			256	//this is a predisaster scientist or barney. Influences how they speak.
#define SF_MONSTER_FADECORPSE			512 // Fade out corpse after death
#define SF_MONSTER_FALL_TO_GROUND		0x80000000

// specialty spawnflags
#define SF_MONSTER_TURRET_AUTOACTIVATE	32
#define SF_MONSTER_TURRET_STARTINACTIVE	64
#define SF_MONSTER_WAIT_UNTIL_PROVOKED	64 // don't attack the player unless provoked



// MoveToOrigin stuff
#define		MOVE_START_TURN_DIST	64 // when this far away from moveGoal, start turning to face next goal
#define		MOVE_STUCK_DIST			32 // if a monster can't step this far, it is stuck.


// MoveToOrigin stuff
#define		MOVE_NORMAL				0// normal move in the direction monster is facing
#define		MOVE_STRAFE				1// moves in direction specified, no matter which way monster is facing

// spawn flags 256 and above are already taken by the engine
extern void UTIL_MoveToOrigin( edict_t* pent, const Vector &vecGoal, float flDist, int iMoveType ); 

Vector VecCheckToss ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flGravityAdj = 1.0 );
Vector VecCheckThrow ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj = 1.0 );
extern DLL_GLOBAL Vector		g_vecAttackDir;
extern DLL_GLOBAL CONSTANT float g_flMeleeRange;
extern DLL_GLOBAL CONSTANT float g_flMediumRange;
extern DLL_GLOBAL CONSTANT float g_flLongRange;
extern void EjectBrass (const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype );
extern void ExplodeModel( const Vector &vecOrigin, float speed, int model, int count );

BOOL FBoxVisible ( entvars_t *pevLooker, entvars_t *pevTarget );
BOOL FBoxVisible ( entvars_t *pevLooker, entvars_t *pevTarget, Vector &vecTargetOrigin, float flSize = 0.0 );

// monster to monster relationship types
#define R_AL	-2 // (ALLY) pals. Good alternative to R_NO when applicable.
#define R_FR	-1// (FEAR)will run
#define	R_NO	0// (NO RELATIONSHIP) disregard
#define R_DL	1// (DISLIKE) will attack
#define R_HT	2// (HATE)will attack this character instead of any visible DISLIKEd characters
#define R_NM	3// (NEMESIS)  A monster Will ALWAYS attack its nemsis, no matter what


// these bits represent the monster's memory
#define MEMORY_CLEAR					0
#define bits_MEMORY_PROVOKED			( 1 << 0 )// right now only used for houndeyes.
#define bits_MEMORY_INCOVER				( 1 << 1 )// monster knows it is in a covered position.
#define bits_MEMORY_SUSPICIOUS			( 1 << 2 )// Ally is suspicious of the player, and will move to provoked more easily
#define bits_MEMORY_PATH_FINISHED		( 1 << 3 )// Finished monster path (just used by big momma for now)
#define bits_MEMORY_ON_PATH				( 1 << 4 )// Moving on a path
#define bits_MEMORY_MOVE_FAILED			( 1 << 5 )// Movement has already failed
#define bits_MEMORY_FLINCHED			( 1 << 6 )// Has already flinched
#define bits_MEMORY_KILLED				( 1 << 7 )// HACKHACK -- remember that I've already called my Killed()
#define bits_MEMORY_CUSTOM4				( 1 << 28 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM3				( 1 << 29 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM2				( 1 << 30 )	// Monster-specific memory
#define bits_MEMORY_CUSTOM1				( 1 << 31 )	// Monster-specific memory

// trigger conditions for scripted AI
// these MUST match the CHOICES interface in halflife.fgd for the base monster
enum 
{
	AITRIGGER_NONE = 0,
	AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER,
	AITRIGGER_TAKEDAMAGE,
	AITRIGGER_HALFHEALTH,
	AITRIGGER_DEATH,
	AITRIGGER_SQUADMEMBERDIE,
	AITRIGGER_SQUADLEADERDIE,
	AITRIGGER_HEARWORLD,
	AITRIGGER_HEARPLAYER,
	AITRIGGER_HEARCOMBAT,
	AITRIGGER_SEEPLAYER_UNCONDITIONAL,
	AITRIGGER_SEEPLAYER_NOT_IN_COMBAT,
};
/*
		0 : "No Trigger"
		1 : "See Player"
		2 : "Take Damage"
		3 : "50% Health Remaining"
		4 : "Death"
		5 : "Squad Member Dead"
		6 : "Squad Leader Dead"
		7 : "Hear World"
		8 : "Hear Player"
		9 : "Hear Combat"
*/

//
// A gib is a chunk of a body, or a piece of wood/metal/rocks/etc.
//
class CGib : public CBaseEntity
{
public:
	void Spawn( const char *szGibModel );
	void EXPORT BounceGibTouch ( CBaseEntity *pOther );
	void EXPORT StickyGibTouch ( CBaseEntity *pOther );
	void EXPORT WaitTillLand( void );
	void LimitVelocity( void );

	int	Classify ( void ) { return	CLASS_GIBS; } // Fograin92: Classify as a GIB
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType); // Fograin92: Allow shooting at gibs
	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType); // Fograin92: Let the gib take the damage
	void Killed( entvars_t *pevAttacker, int iGib );	// Fograin92: Override kill function


	virtual int	ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static	void SpawnHeadGib( entvars_t *pevVictim );
	static	void SpawnRandomGibs( entvars_t *pevVictim, int cGibs, int human );
	static  void SpawnStickyGibs( entvars_t *pevVictim, Vector vecOrigin, int cGibs );

	int		m_bloodColor;
	int		m_cBloodDecals;
	int		m_material;
	float	m_lifeTime;
};


// Fograin92: FX gibs
enum fxgibs_types_e
{
	// Fograin92: World debris
	FXG_CONCRETE = 0,
	FXG_WOOD,
	FXG_VENT,
	FXG_ROCK,
	FXG_PAPER,
	FXG_METAL,
	FXG_METAL_BLACK,
	FXG_METAL_GREEN,
	FXG_METAL_FLOOR,
	FXG_GLASS,
	FXG_CEILING,
	FXG_CACTUS,

	// Fograin92: Gore FX
	FXG_BLOOD_RED,
	FXG_BLOOD_YELLOW,

	AMOUNT			// Length of this enum
};


class CFXGibs : public CGib
{
public:
	void Spawn( const char *szGibModel );
	int	Classify ( void )
	{
		return	CLASS_GIBS;	// Fograin92: Classify as a GIB
	} 

	static	void SpawnFXGibs( Vector vPosition, int iFXGibType, int iFXGibCount );
	void EXPORT WaitTillLand( void );
	void EXPORT BounceGibTouch ( CBaseEntity *pOther );

	// Fograin92: Override this stuff
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void Killed( entvars_t *pevAttacker, int iGib );

	int		m_bloodColor;
	int		m_material;
	float	m_lifeTime;
};


// Fograin92: Dropped clip/magazine/item with Gib physics
class CDropClip : public CFXGibs
{
public:
	void Spawn( const char *szGibModel, float fDelay);
	int	Classify ( void )
	{
		return	CLASS_GIBS;	// Fograin92: Classify as a GIB
	} 

	void EXPORT WaitTillSpawn( void );	// Fograin92
	void EXPORT WaitTillLand( void );
	void EXPORT BounceGibTouch ( CBaseEntity *pOther );
	
	// Fograin92: Override this stuff
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void Killed( entvars_t *pevAttacker, int iGib );

	int		m_bloodColor;
	int		m_material;
	float	m_lifeTime;
	float	m_DelayedSpawn;
};



#define CUSTOM_SCHEDULES\
		virtual Schedule_t *ScheduleFromName( const char *pName );\
		static Schedule_t *m_scheduleList[];

#define DEFINE_CUSTOM_SCHEDULES(derivedClass)\
	Schedule_t *derivedClass::m_scheduleList[] =

#define IMPLEMENT_CUSTOM_SCHEDULES(derivedClass, baseClass)\
		Schedule_t *derivedClass::ScheduleFromName( const char *pName )\
		{\
			Schedule_t *pSchedule = ScheduleInList( pName, m_scheduleList, ARRAYSIZE(m_scheduleList) );\
			if ( !pSchedule )\
				return baseClass::ScheduleFromName(pName);\
			return pSchedule;\
		}



#endif	//MONSTERS_H
