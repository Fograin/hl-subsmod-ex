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

typedef enum
{
	STATE_OFF = 0,	// disabled, inactive, invisible, closed, or stateless. Or non-alert monster.
	STATE_ON,		// enabled, active, visisble, or open. Or alert monster.
	STATE_TURN_ON,	// door opening, env_fade fading in, etc.
	STATE_TURN_OFF,	// door closing, monster dying (?).
	STATE_IN_USE,	// player is in control (train/tank/barney/scientist).
	STATE_DEAD,	// entity dead
} STATE;


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

	STATE GetState( void )
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
};






#endif	//PROPS_H



/*




LINK_ENTITY_TO_CLASS( env_model, CEnvModel );

BEGIN_DATADESC( CEnvModel )
	DEFINE_KEYFIELD( m_iszSequence_On, FIELD_STRING, "m_iszSequence_On" ),
	DEFINE_KEYFIELD( m_iszSequence_Off, FIELD_STRING, "m_iszSequence_Off" ),
	DEFINE_KEYFIELD( m_iAction_On, FIELD_INTEGER, "m_iAction_On" ),
	DEFINE_KEYFIELD( m_iAction_Off, FIELD_INTEGER, "m_iAction_Off" ),
END_DATADESC()

void CEnvModel :: KeyValue( KeyValueData *pkvd )
{
	if( FStrEq( pkvd->szKeyName, "m_iszSequence_On" ))
	{
		m_iszSequence_On = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if( FStrEq( pkvd->szKeyName, "m_iszSequence_Off" ))
	{
		m_iszSequence_Off = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if( FStrEq(pkvd->szKeyName, "m_iAction_On" ))
	{
		m_iAction_On = Q_atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if( FStrEq( pkvd->szKeyName, "m_iAction_Off" ))
	{
		m_iAction_Off = Q_atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{
		BaseClass::KeyValue( pkvd );
	}
}

void CEnvModel :: Spawn( void )
{
	Precache();

	SET_MODEL( edict(), STRING(pev->model) );
	RelinkEntity( TRUE );

	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	SetSequence();

	if( FBitSet( pev->spawnflags, SF_ENVMODEL_SOLID ))
	{
		if( UTIL_AllowHitboxTrace( this ))
			pev->solid = SOLID_BBOX;
		else pev->solid = SOLID_SLIDEBOX;
		AutoSetSize();
	}

	if( FBitSet( pev->spawnflags, SF_ENVMODEL_DROPTOFLOOR ))
	{
		Vector origin = GetLocalOrigin();
		origin.z += 1;
		SetLocalOrigin( origin );
		UTIL_DropToFloor( this );
	}
	
	SetNextThink( 0.1 );
}

void CEnvModel::Precache( void )
{
	PRECACHE_MODEL( GetModel() );
}

void CEnvModel::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( ShouldToggle( useType ))
	{
		if( pev->spawnflags & SF_ENVMODEL_OFF )
			pev->spawnflags &= ~SF_ENVMODEL_OFF;
		else pev->spawnflags |= SF_ENVMODEL_OFF;

		SetSequence();
		SetNextThink( 0.1 );
	}
}

// automatically set collision box
void CEnvModel :: AutoSetSize( void )
{
	studiohdr_t *pstudiohdr;
	pstudiohdr = (studiohdr_t *)GET_MODEL_PTR( edict() );

	if( pstudiohdr == NULL )
	{
		UTIL_SetSize( pev, Vector( -10, -10, -10 ), Vector( 10, 10, 10 ));
		ALERT( at_error, "env_model: unable to fetch model pointer!\n" );
		return;
	}

	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
	UTIL_SetSize( pev, pseqdesc[pev->sequence].bbmin, pseqdesc[pev->sequence].bbmax );
}

void CEnvModel::Think( void )
{
	StudioFrameAdvance ( ); // set m_fSequenceFinished if necessary

	if( m_fSequenceFinished && !m_fSequenceLoops )
	{
		int iTemp;

		if( pev->spawnflags & SF_ENVMODEL_OFF )
			iTemp = m_iAction_Off;
		else
			iTemp = m_iAction_On;

		switch( iTemp )
		{
		case 2:	// change state
			if( pev->spawnflags & SF_ENVMODEL_OFF )
				pev->spawnflags &= ~SF_ENVMODEL_OFF;
			else pev->spawnflags |= SF_ENVMODEL_OFF;
			SetSequence();
			break;
		default:	// remain frozen
			return;
		}
	}

	SetNextThink( 0.1 );
}

void CEnvModel :: SetSequence( void )
{
	int iszSeq;

	if( pev->spawnflags & SF_ENVMODEL_OFF )
		iszSeq = m_iszSequence_Off;
	else
		iszSeq = m_iszSequence_On;

	if( !iszSeq ) return;
	pev->sequence = LookupSequence( STRING( iszSeq ));

	if( pev->sequence == -1 )
	{
		if( pev->targetname )
			ALERT( at_error, "env_model %s: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		else
			ALERT( at_error, "env_model: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		pev->sequence = 0;
	}

	pev->frame = 0;
	ResetSequenceInfo( );

	if( pev->spawnflags & SF_ENVMODEL_OFF )
	{
		if( m_iAction_Off == 1 )
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
	else
	{
		if( m_iAction_On == 1 )
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
}

// =================== ENV_STATIC ==============================================
#define SF_STATIC_SOLID	BIT( 0 )
#define SF_STATIC_DROPTOFLOOR	BIT( 1 )

class CEnvStatic : public CBaseEntity
{
	DECLARE_CLASS( CEnvStatic, CBaseEntity );
public:
	void Spawn( void );
	void AutoSetSize( void );
	virtual int ObjectCaps( void ) { return FCAP_IGNORE_PARENT; }
	void SetObjectCollisionBox( void );
};

LINK_ENTITY_TO_CLASS( env_static, CEnvStatic );

void CEnvStatic :: Spawn( void )
{
	// don't allow to change scale
	pev->scale = 1.0f;

	PRECACHE_MODEL( GetModel() );
	SET_MODEL( edict(), GetModel() );

	if( FBitSet( pev->spawnflags, SF_STATIC_SOLID ))
	{
		if( WorldPhysic->Initialized( ))
			pev->solid = SOLID_CUSTOM;
		pev->movetype = MOVETYPE_NONE;
		AutoSetSize();
	}

	if( FBitSet( pev->spawnflags, SF_STATIC_DROPTOFLOOR ))
	{
		Vector origin = GetLocalOrigin();
		origin.z += 1;
		SetLocalOrigin( origin );
		UTIL_DropToFloor( this );
	}
	else
	{
		UTIL_SetOrigin( this, GetLocalOrigin( ));
	}

	if( FBitSet( pev->spawnflags, SF_STATIC_SOLID ))
	{
		m_pUserData = WorldPhysic->CreateStaticBodyFromEntity( this );
		RelinkEntity( TRUE );
	}
	else
	{
		// remove from server
		MAKE_STATIC( edict() );
	}
}

void CEnvStatic :: SetObjectCollisionBox( void )
{
	// expand for rotation
	TransformAABB( EntityToWorldTransform(), pev->mins, pev->maxs, pev->absmin, pev->absmax );

	pev->absmin.x -= 1;
	pev->absmin.y -= 1;
	pev->absmin.z -= 1;
	pev->absmax.x += 1;
	pev->absmax.y += 1;
	pev->absmax.z += 1;
}

// automatically set collision box
void CEnvStatic :: AutoSetSize( void )
{
	studiohdr_t *pstudiohdr;
	pstudiohdr = (studiohdr_t *)GET_MODEL_PTR( edict() );

	if( pstudiohdr == NULL )
	{
		UTIL_SetSize( pev, Vector( -10, -10, -10 ), Vector( 10, 10, 10 ));
		ALERT( at_error, "env_model: unable to fetch model pointer!\n" );
		return;
	}

	mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
	UTIL_SetSize( pev, pseqdesc[pev->sequence].bbmin, pseqdesc[pev->sequence].bbmax );
}
*/