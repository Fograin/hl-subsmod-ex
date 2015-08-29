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
#include "func_grass.h"
#include "particle_emitter.h"

// create ourselves a garden
extern int gmsgGrassParticles;

void CGrass :: Spawn ( void )
{
	pev->spawnflags |= SF_GRASS_ACTIVE;

	pev->solid = SOLID_NOT;// always solid_not 
	SET_MODEL( ENT(pev), STRING(pev->model) );
	pev->effects |= EF_NODRAW;

	if ( FStringNull( pev->targetname ))
	{
		pev->spawnflags |= SF_GRASS_ACTIVE;
		ALERT(at_console, "WARNING: CGrass :: Spawn, Can't find targetname.\n");
	}

	if (pev->spawnflags & SF_GRASS_ACTIVE)
	{
		ALERT (at_console, "CGrass Spawn in <ON> Mode\nNextthink in 0.5 Seconds\n");

		bIsOn = true;
		iID = ++iParticleIDCount;

		SetThink( &CGrass::TurnOn );
		pev->nextthink = gpGlobals->time + 0.5;
	}
}

void CGrass :: Precache ( void )
{
	if (pev->spawnflags & SF_GRASS_ACTIVE)
	{
		SetThink(&CGrass :: ResumeThink );

		pev->nextthink = 0.1;
	}
}

//Load values from the bsp
void CGrass::KeyValue( KeyValueData* pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "definition_file") )
	{
		strncat(sParticleDefintionFile, pkvd->szValue, strlen(pkvd->szValue));
		pkvd->fHandled = true;
	} else {
		CBaseEntity::KeyValue( pkvd );
	}
}

void CGrass :: TurnOn ( void )
{
	ALERT(at_console, "CGrass :: TurnOn, Current Time: %f\n", gpGlobals->time);

	pev->spawnflags |= SF_GRASS_ACTIVE;

	//the first time the particle spawns

	//	MakeAware( this);

	// increment the count just in case
	iID = ++iParticleIDCount;

	// tell everyone about our new grass
	if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
			WRITE_SHORT(iID);
			WRITE_BYTE(0);
			WRITE_COORD(pev->absmax.x);
			WRITE_COORD(pev->absmax.y);
			WRITE_COORD(pev->absmax.z);
			WRITE_COORD(pev->absmin.x);
			WRITE_COORD(pev->absmin.y);
			WRITE_COORD(pev->absmin.z);
	//		WRITE_STRING(sParticleDefintionFile);
			WRITE_STRING(STRING(pev->message));
		MESSAGE_END();
	}
}

void CGrass :: TurnOff ( void )
{
	ALERT(at_console, "CGrass :: TurnOff Call Treated as MakeAware\n");

	// increment the count just in case
	iID = ++iParticleIDCount;

	// tell everyone about our new grass
	if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
			WRITE_SHORT(iID);
			WRITE_BYTE(0);
			WRITE_COORD(pev->absmax.x);
			WRITE_COORD(pev->absmax.y);
			WRITE_COORD(pev->absmax.z);
			WRITE_COORD(pev->absmin.x);
			WRITE_COORD(pev->absmin.y);
			WRITE_COORD(pev->absmin.z);
	//		WRITE_STRING(sParticleDefintionFile);
			WRITE_STRING(STRING(pev->message));
		MESSAGE_END();
	}

	ALERT(at_console, "CGrass :: DontThink();\n");
	DontThink();
}

void CGrass :: ResumeThink ( void )
{
	ALERT(at_console, "CGrass :: ResumeThink, Current Time: %f\n", gpGlobals->time);
	SetThink(&CGrass ::TurnOff);

	pev->nextthink = 0.1;
}


void CGrass :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (ShouldToggle(useType, pev->body))
	{
		if (pev->spawnflags & SF_GRASS_ACTIVE)
			TurnOff();
		else
			TurnOn();
	}
}

void CGrass::MakeAware( CBaseEntity *pEnt )
{
	
	ALERT(at_console, "CGrass :: MakeAware\n");
	
	bool bTurnOn = true;

	// increment the count just in case
	iID = ++iParticleIDCount;

	// tell everyone about our new grass
	MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
		WRITE_SHORT(iID);
		WRITE_BYTE(0);
		WRITE_COORD(pev->absmax.x);
		WRITE_COORD(pev->absmax.y);
		WRITE_COORD(pev->absmax.z);
		WRITE_COORD(pev->absmin.x);
		WRITE_COORD(pev->absmin.y);
		WRITE_COORD(pev->absmin.z);
		WRITE_STRING(STRING(pev->message));
//		WRITE_STRING(sParticleDefintionFile);
	MESSAGE_END();
}

LINK_ENTITY_TO_CLASS( func_grass, CGrass );