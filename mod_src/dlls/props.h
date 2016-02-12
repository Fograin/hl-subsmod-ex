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
#include "saverestore.h"
#include "studio.h"
#include "decals.h"
#include "explode.h"

//#include "animation.h"
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


//========================//
// Material Impact Sounds
//========================//
static const char * const pSoundHitWood[] = 
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
};

static const char * const pSoundsHitFlesh[] = 
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh5.wav",
	"debris/flesh6.wav",
	"debris/flesh7.wav",
};

static const char * const pSoundsHitMetal[] = 
{
	"debris/metal1.wav",
	"debris/metal3.wav",
	"debris/metal4.wav",
};

static const char * const pSoundsHitConcrete[] = 
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
};

static const char * const pSoundsHitGlass[] = 
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
};


//==============//
// Break sounds
//==============//

static const char * const pSoundsBustCeiling[] = 
{
	"debris/bustceiling.wav",
	// TODO: MOAR?
};

static const char * const pSoundsBustConcrete[] = 
{
	"debris/bustconcrete1.wav",
	"debris/bustconcrete2.wav",
};
	
static const char * const pSoundsBustCrate[] = 
{
	"debris/bustcrate1.wav",
	"debris/bustcrate2.wav",
	"debris/bustcrate3.wav",
};
	
static const char * const pSoundsBustFlesh[] = 
{
	"debris/bustflesh1.wav",
	"debris/bustflesh2.wav",
};
	
static const char * const pSoundsBustGlass[] = 
{
	"debris/bustglass1.wav",
	"debris/bustglass2.wav",
	"debris/bustglass3.wav",
};
	
static const char * const pSoundsBustMetal[] = 
{
	"debris/bustmetal1.wav",
	"debris/bustmetal2.wav",
};

// Explosion sounds
static const char * const pSoundsExplode[] = 
{
	"weapons/explode3.wav",
	"weapons/explode4.wav",
	"weapons/explode5.wav",
};

// Spawn items when broke
static const char * const pSpawnObjects[] = 
{
	NULL,					// 0
	"item_battery",			// 1
	"item_healthkit",		// 2
	"weapon_9mmhandgun",	// 3
	"ammo_9mmclip",			// 4
	"weapon_9mmAR",			// 5
	"ammo_9mmAR",			// 6
	"ammo_ARgrenades",		// 7
	"weapon_shotgun",		// 8
	"ammo_buckshot",		// 9
	"weapon_crossbow",		// 10
	"ammo_crossbow",		// 11
	"weapon_357",			// 12
	"ammo_357",				// 13
	"weapon_rpg",			// 14
	"ammo_rpgclip",			// 15
	"ammo_gaussclip",		// 16
	"weapon_handgrenade",	// 17
	"weapon_tripmine",		// 18
	"weapon_satchel",		// 19
	"weapon_snark",			// 20
	"weapon_hornetgun",		// 21
};


//==================//
// SHARED FUNCTIONS
//==================//

// Trace attack of model based prop OR brush entity
inline void PropSharedTraceAttack( entvars_t *pevAttacker, 
								   float flDamage, 
								   Vector vecDir, 
								   TraceResult *ptr, 
								   int bitsDamageType, 
								   ePropMaterial m_PropMaterial,
								   entvars_t *pevE
								   )
{
	// A little bit of variation :)
	int pitch = 95 + RANDOM_LONG(0,9);

	switch( m_PropMaterial )
	{
		case matGlass:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitGlass[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitGlass)-1) ], 1.0, ATTN_NORM, 0, pitch );
		break;

		case matWood:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundHitWood[ RANDOM_LONG(0, ARRAYSIZE(pSoundHitWood)-1) ], 1.0, ATTN_NORM, 0, pitch );
		break;

		case matMetal:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitMetal[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitMetal)-1) ], 1.0, ATTN_NORM, 0, pitch );
			UTIL_Ricochet( ptr->vecEndPos, 1.0 );
		break;

		case matFlesh:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitFlesh[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitFlesh)-1) ], 1.0, ATTN_NORM, 0, pitch );
		break;

		case matCinderBlock:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitConcrete[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitConcrete)-1) ], 1.0, ATTN_NORM, 0, pitch );
		break;

		case matCeilingTile:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitConcrete[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitConcrete)-1) ], 1.0, ATTN_NORM, 0, pitch );
		break;

		case matComputer:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitGlass[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitGlass)-1) ], 1.0, ATTN_NORM, 0, pitch );
			UTIL_Ricochet( ptr->vecEndPos, 1.0 );
		break;

		case matUnbreakableGlass:
			EMIT_SOUND_DYN( ENT(pevE), CHAN_VOICE, pSoundsHitGlass[ RANDOM_LONG(0, ARRAYSIZE(pSoundsHitGlass)-1) ], 1.0, ATTN_NORM, 0, pitch );
			UTIL_Ricochet( ptr->vecEndPos, 1.0 );
		break;

		case matRocks:

		break;

		default:
			break;
	}
}



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
