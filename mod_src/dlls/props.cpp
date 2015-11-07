//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	Valve LLC.
//	Id Software, Inc. ("Id Technology")
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "props.h"


//=========//
// Linkers
//=========//
LINK_ENTITY_TO_CLASS( prop_default, CPropDefault );


//==============================================//
// prop_default - Basic model based prop entity
//==============================================//
void CPropDefault::Spawn( void )
{
	Precache();
	SET_MODEL( edict(), STRING(pev->model) );

	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	SetSequence();

	// Should prop have a collision?
	if( FBitSet( pev->spawnflags, SF_PROP_SOLID ))
	{
		pev->solid = SOLID_SLIDEBOX;

		// Automatically set collision box
		studiohdr_t *pstudiohdr;
		pstudiohdr = (studiohdr_t *)GET_MODEL_PTR( edict() );

		if( pstudiohdr == NULL )
		{
			UTIL_SetSize( pev, Vector( -10, -10, -10 ), Vector( 10, 10, 10 ));
			ALERT( at_console, "^2prop_default: Unable to get model pointer!\n" );
			return;
		}

		mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
		UTIL_SetSize( pev, pseqdesc[pev->sequence].bbmin, pseqdesc[pev->sequence].bbmax );
	}

	// Drop model to the floor
	if( FBitSet( pev->spawnflags, SF_PROP_DROPTOFLOOR ))
	{
		UTIL_SetOrigin( pev, pev->origin );
		if (DROP_TO_FLOOR(ENT(pev)) == 0)
		{
			ALERT(at_error, "Prop %s fell out of level at %f,%f,%f\n", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
			UTIL_Remove( this );
			return;
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}


// Precache
void CPropDefault::Precache( void )
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );
}


// Key values from map
void CPropDefault::KeyValue( KeyValueData *pkvd )
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
	else if (FStrEq(pkvd->szKeyName, "m_iAction_On"))
	{
		m_iAction_On = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iAction_Off"))
	{
		m_iAction_Off = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseAnimating::KeyValue( pkvd );
	}
}


// Let our prop think
void CPropDefault::Think( void )
{
	StudioFrameAdvance(); // set m_fSequenceFinished if necessary

	if( m_fSequenceFinished && !m_fSequenceLoops )
	{
		int iTemp;

		if( pev->spawnflags & SF_PROP_OFF )
			iTemp = m_iAction_Off;
		else
			iTemp = m_iAction_On;

		switch( iTemp )
		{
			case 2:	// change state
				if( pev->spawnflags & SF_PROP_OFF )
					pev->spawnflags &= ~SF_PROP_OFF;
				else
					pev->spawnflags |= SF_PROP_OFF;
				SetSequence();
			break;
		
			// Freeze
			default:
				return;
		}
	}

	pev->nextthink = gpGlobals->time + 0.1;
}


// Set prop sequence animation
void CPropDefault::SetSequence( void )
{
	int iszSeq;

	if( pev->spawnflags & SF_PROP_OFF )
		iszSeq = m_iszSequence_Off;
	else
		iszSeq = m_iszSequence_On;

	if( !iszSeq )
		return;
	pev->sequence = LookupSequence( STRING( iszSeq ));

	if( pev->sequence == -1 )
	{
		if( pev->targetname )
			ALERT( at_error, "prop_default %s: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		else
			ALERT( at_error, "prop_default: unknown sequence \"%s\"\n", STRING( pev->targetname ), STRING( iszSeq ));
		pev->sequence = 0;
	}

	pev->frame = 0;
	ResetSequenceInfo();

	if( pev->spawnflags & SF_PROP_OFF )
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

// Handle "use"
void CPropDefault::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( ShouldToggle( useType, GetState() ))
	{
		if( pev->spawnflags & SF_PROP_OFF )
			pev->spawnflags &= ~SF_PROP_OFF;
		else
			pev->spawnflags |= SF_PROP_OFF;

		SetSequence();
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CPropDefault::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	UTIL_Ricochet( ptr->vecEndPos, 1.0 );
		
	CBaseAnimating::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

TYPEDESCRIPTION	CPropDefault::m_SaveData[] = 
{
	DEFINE_FIELD( CPropDefault,		m_iszSequence_On,	FIELD_STRING ),
	DEFINE_FIELD( CPropDefault,		m_iszSequence_Off,	FIELD_STRING ),
	DEFINE_FIELD( CPropDefault,		m_iAction_On,		FIELD_INTEGER ),
	DEFINE_FIELD( CPropDefault,		m_iAction_Off,		FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CPropDefault, CBaseAnimating );
