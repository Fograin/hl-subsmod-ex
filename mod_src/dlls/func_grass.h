/***
*
*	Copyright (c) 2007, 
*	
*	(FCC)™DJShark
*   Generation Half-Life 
****/

#ifndef FUNC_GRASS
#define FUNC_GRASS

#include "particle_defs.h"

#define SF_GRASS_ACTIVE 1

class CGrass : public CBaseEntity {
	char sParticleDefintionFile[MAX_PARTICLE_PATH];
	bool bIsOn;
	unsigned int iID;
public:
    void Spawn( void );
    void KeyValue( KeyValueData* pKeyValueData );
    void MakeAware( CBaseEntity* pEnt );
	void EXPORT Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void EXPORT ResumeThink( void );
	void EXPORT TurnOn( void );
	void EXPORT TurnOff( void );
	void Precache( void );
};

#endif