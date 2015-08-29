/***
*
*	Copyright (c) 2007, 
*	
*	(FCC)™DJShark
*   Generation Half-Life 
****/


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "particle_emitter.h"

// create ourselves a particle emitter
void CParticleEmitter::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL (ENT(pev), STRING(pev->model));
	pev->effects |= EF_NODRAW;
	
	UTIL_SetOrigin( pev, pev->origin ); 		
	UTIL_SetSize( pev, pev->absmin, pev->absmax );

    SetUse ( &CParticleEmitter::Use );

	if (pev->spawnflags & SF_START_ON)
	{
		ALERT (at_console, "CParticleEmitter Spawn in <ON> Mode\nNextthink in 0.5 Seconds\n");

		bIsOn = true;
		iID = ++iParticleIDCount;

		SetThink( &CParticleEmitter::TurnOn );
		pev->nextthink = gpGlobals->time + 0.5;
	}
	else
	{
		bIsOn = false;
	}

	IsTriggered(NULL);
	iID = ++iParticleIDCount;
	flTimeTurnedOn = 0.0;
}

//Load values from the bsp
void CParticleEmitter::KeyValue( KeyValueData* pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "definition_file") )
	{
		strncat(sParticleDefintionFile, pkvd->szValue, strlen(pkvd->szValue));
		pkvd->fHandled = true;
	} else {
		CBaseEntity::KeyValue( pkvd );
	}
}

extern int gmsgParticles;

void CParticleEmitter :: Precache ( void )
{
	if (pev->spawnflags & SF_START_ON)
	{
		SetThink(&CParticleEmitter :: ResumeThink );

		pev->nextthink = 0.1;
	}
}
void CParticleEmitter :: TurnOn ( void )
{
	ALERT(at_console, "CParticleEmitter :: TurnOn, Current Time: %f\n", gpGlobals->time);

	pev->spawnflags |= SF_START_ON;

	//the first time the particle spawns

	// increment the count just in case
	iID = ++iParticleIDCount;

	// lets give them everything
	if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);//enviar a todos... qué importa??
			WRITE_SHORT(iID);
			WRITE_BYTE(0);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pev->angles.x);
			WRITE_COORD(pev->angles.y);
			WRITE_COORD(pev->angles.z);
			WRITE_SHORT(0);
			WRITE_STRING(STRING(pev->message));
			WRITE_STRING(sParticleDefintionFile);
		MESSAGE_END();
	}
}

void CParticleEmitter :: TurnOff ( void )
{
	ALERT(at_console, "CParticleEmitter :: TurnOff Call Treated as MakeAware\n");

	// increment the count just in case
	iID = ++iParticleIDCount;

	// lets give them everything
	if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgParticles);//enviar a todos... qué importa??
			WRITE_SHORT(iID);
			WRITE_BYTE(0);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(pev->angles.x);
			WRITE_COORD(pev->angles.y);
			WRITE_COORD(pev->angles.z);
			WRITE_SHORT(0);
			WRITE_STRING(STRING(pev->message));
			WRITE_STRING(sParticleDefintionFile);
		MESSAGE_END();
	}

	ALERT(at_console, "CParticleEmitter :: DontThink();\n");
	DontThink();
}

void CParticleEmitter :: ResumeThink ( void )
{
	ALERT(at_console, "CParticleEmitter :: ResumeThink, Current Time: %f\n", gpGlobals->time);
	SetThink(&CParticleEmitter ::TurnOff);

	pev->nextthink = 0.1;
}


void CParticleEmitter :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (ShouldToggle(useType, pev->body))
	{
		if (pev->spawnflags & SF_START_ON)
			TurnOff();
		else
			TurnOn();
	}
}

// Let the player know there is a particleemitter
void CParticleEmitter::MakeAware( CBaseEntity *pEnt )
{
	ALERT(at_console, "CParticleEmitter :: MakeAware\n");
	
	bool bTurnOn = true;

	// lets give them everything
	MESSAGE_BEGIN(MSG_ONE, gmsgParticles, NULL, pEnt->pev);
		WRITE_SHORT(iID);
		WRITE_BYTE(0);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_COORD(pev->angles.x);
		WRITE_COORD(pev->angles.y);
		WRITE_COORD(pev->angles.z);
		WRITE_SHORT(0);
		WRITE_STRING(STRING(pev->message));
		WRITE_STRING(sParticleDefintionFile);
	MESSAGE_END();
}

TYPEDESCRIPTION	CParticleEmitter::m_SaveData[] = 
{
	DEFINE_FIELD( CParticleEmitter, bIsOn, FIELD_BOOLEAN )
};

IMPLEMENT_SAVERESTORE( CParticleEmitter, CPointEntity );

LINK_ENTITY_TO_CLASS( env_particleemitter, CParticleEmitter );

