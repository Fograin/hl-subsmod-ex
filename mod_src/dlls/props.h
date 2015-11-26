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
#ifndef	PROPS_H
#define	PROPS_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "studio.h"
//#include "animation.h"
//#include "saverestore.h"

//#include "monsters.h"
//#include "weapons.h"
//#include "nodes.h"
//#include "player.h"
//#include "skill.h"
//#include "items.h"
//#include "gamerules.h"


// Prop states
typedef enum
{
	STATE_OFF = 0,	// disabled, inactive, invisible, closed, or stateless.
	STATE_ON,		// enabled, active, visisble, or open.
	STATE_TURN_ON,	// door opening or other event playing
	STATE_TURN_OFF,	// door closing or other event ending
	STATE_IN_USE,	// player is in control (train/tank/barney/scientist).
	STATE_DEAD,	// entity dead
} ePropState;

// Prop material type
typedef enum
{
	matGlass = 0,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matNone,
	matCount,	// How many materials
} ePropMaterial;


static const char * const pSoundHitWood[] = 
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
};

static const char * const pSoundsFlesh[] = 
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh5.wav",
	"debris/flesh6.wav",
	"debris/flesh7.wav",
};

static const char * const pSoundsMetal[] = 
{
	"debris/metal1.wav",
	"debris/metal2.wav",
	"debris/metal3.wav",
};

static const char * const pSoundsConcrete[] = 
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
};

static const char * const pSoundsGlass[] = 
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
};


//==============================================//
// prop_default - Basic model based prop entity
//==============================================//
#define SF_PROP_OFF				1
#define SF_PROP_SOLID			2
#define SF_PROP_DROPTOFLOOR		4

class CPropDefault : public CBaseAnimating
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	ePropState GetState( void )
	{
		return FBitSet( pev->spawnflags, SF_PROP_OFF ) ? STATE_OFF : STATE_ON;
	}

	void Think( void );
	void SetSequence( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	virtual int ObjectCaps( void )
	{
		return CBaseAnimating::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;
	}

	// Save-Restore part
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	
	// Vars
	string_t m_iszSequence_On;
	string_t m_iszSequence_Off;

	int m_iAction_On;
	int m_iAction_Off;

	ePropMaterial	m_PropMaterial;
};



#endif	//PROPS_H
