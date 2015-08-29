/*
    Copyright 2001 to 2004. The Battle Grounds Team and Contributors

    This file is part of the Battle Grounds Modification for Half-Life.

    The Battle Grounds Modification for Half-Life is free software;
    you can redistribute it and/or modify it under the terms of the
    GNU Lesser General Public License as published by the Free
    Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    The Battle Grounds Modification for Half-Life is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with The Battle Grounds Modification for Half-Life;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place,
    Suite 330, Boston, MA  02111-1307  USA

    You must obey the GNU Lesser General Public License in all respects for
    all of the code used other than code distributed with the Half-Life
    SDK developed by Valve.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.
*/

// definition of the particle systems used in bg

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include "pm_shared.h"

#include "particle_header.h"

// defaults for internal variables
CParticleSystem::CParticleSystem()
{
	m_flLastDraw = gEngfuncs.GetClientTime();
	m_flSystemsAge = 0;
	m_flParticleCreationTime = 0;

	// all non mapped or grass systems are 0
	// in theory we could send a ps message with id 0 to kill all particles effect particles
	m_iID = 0; 

	pParticleTexture = NULL;
}

// remove this system's particles
CParticleSystem::~CParticleSystem() {}

// adds a particle to the system
inline void CParticleSystem::AddParticle( CParticle *pParticle )
{
	 pParticleManager->AddParticle(pParticle);
}

// returns the number of particles that we are to create when initialising our particle system
unsigned int CParticleSystem::StartingParticles( void )
{
	unsigned int iParticles =  static_cast<unsigned int>(g_ParticleCount->value);
	return (m_iStartingParticles * iParticles / PERCENT);		
}

// returns the number of new particles to be created this second
float CParticleSystem::NewParticlesCreationDelay( void )
{
	unsigned int iParticles =  static_cast<unsigned int>(g_ParticleCount->value);
	return (m_flNewParticles * iParticles / PERCENT);
}

// tests whether this system has died or now
bool CParticleSystem::TestSystem( void )
{
	return !!(m_flSystemsAge <= m_flSystemMaxAge);
}

// updates this system's timers
void CParticleSystem::UpdateSystem( void )
{
	m_flSystemsAge += TimeSinceLastDraw();
}

// draws all particles belonging to this system
bool CParticleSystem::DrawSystem( void )
{
	if (TestSystem() == false) {
		return false;
	}

	UpdateSystem();
	m_flLastDraw = gEngfuncs.GetClientTime();
	return true;
}

// creates a flintlock smoke particle system
CFlintlockSmokeParticleSystem::CFlintlockSmokeParticleSystem(vec3_t vPosition) : CParticleSystem() 
{
	m_vPosition = vPosition;

	m_iStartingParticles = STARTING_FLINKLOCK_SMOKE_PARTICLES;
	m_flSystemMaxAge = FLINTLOCK_SMOKE_SYSTEM_LIFE;

	pParticleTexture = LoadTGA(NULL, const_cast<char*>(FLINTLOCK_SMOKE_PARTICLE));

	int n = StartingParticles();
	for(int i = 0; i < n; i++) {
		CParticle *pParticle = new CFlintParticle(vPosition, pParticleTexture);
		AddParticle(pParticle);
	}
}

// creates a barrel smoke particle system
CBarrelSmokeParticleSystem::CBarrelSmokeParticleSystem(vec3_t vPosition, vec3_t vDirection)	: CParticleSystem()
{
	int i = 0;
	// create the memory for our textures
	for(i = 0; i < NUM_DIFFERENT_BARREL_PARTICLES; i++) {
		pParticleTextures[i] = LoadTGA(NULL, const_cast<char*>(BARREL_SMOKE_PARTICLES[i]));
	}

	m_vPosition = vPosition;
	m_vDirection = vDirection;

	m_flNewParticles = NEW_BARREL_SMOKE_PARTICLES_SECOND;
	m_iStartingParticles = STARTING_BARREL_SMOKE_PARTICLES;
	m_flSystemMaxAge = BARREL_SMOKE_SYSTEM_LIFE;

	int particles = StartingParticles();
	int n = NUM_DIFFERENT_BARREL_PARTICLES;
	for(i = 0; i < particles; i++) {
		CParticle *pParticle = new CBarrelParticle(m_vPosition, m_vDirection, pParticleTextures[i % n]);
		AddParticle(pParticle);
	}
}

// updates a barrel smoke particle system
void CBarrelSmokeParticleSystem::UpdateSystem( void )
{
	CParticleSystem::UpdateSystem();
	m_flParticleCreationTime += TimeSinceLastDraw();

	int i = 0;
	int n = NUM_DIFFERENT_BARREL_PARTICLES;

	// creates the specified amount of new particles,
	// during the first few moments of this systems life
	while(m_flParticleCreationTime > 0 && m_flSystemsAge < BARREL_SMOKE_PARTICLE_CREATION_TIME)
	{
		CParticle *pParticle = new CBarrelParticle(m_vPosition, m_vDirection, pParticleTextures[i % n]);
		AddParticle(pParticle);
		m_flParticleCreationTime -= (A_SECOND / NewParticlesCreationDelay());
	}
}

// creates a spark particle system
CSparkParticleSystem::CSparkParticleSystem(vec3_t vPosition, vec3_t vDirection)	: CParticleSystem()
{
	m_vPosition = vPosition;
	m_vDirection = vDirection;

	m_flNewParticles = NEW_SPARK_PARTICLES_SECOND;
	m_iStartingParticles = STARTING_SPARK_PARTICLES;
	m_flSystemMaxAge = SPARK_SYSTEM_LIFE;

	pParticleTexture = LoadTGA(NULL, const_cast<char*>(SPARK_PARTICLE)); 

	int n = StartingParticles();
	for(int i = 0; i < n; i++) {
		CParticle *pParticle = new CSparkParticle(m_vPosition, m_vDirection, pParticleTexture);
		AddParticle(pParticle);
	}
}

// updates the spark system and adds more particles after creation
void CSparkParticleSystem::UpdateSystem( void )
{
	CParticleSystem::UpdateSystem();
	m_flParticleCreationTime += TimeSinceLastDraw();

	// creates the specified amount of new particles,
	//during the first few moments of this systems life
	while(m_flParticleCreationTime > 0 && m_flSystemsAge < SPARK_PARTICLE_CREATION_TIME)
	{
		CParticle *pParticle = new CSparkParticle(m_vPosition, m_vDirection, pParticleTexture);
		AddParticle(pParticle);
		m_flParticleCreationTime -= (A_SECOND / NewParticlesCreationDelay());
	}
}

// creates a flintlock smoke particle system
CWhiteSmokeParticleSystem::CWhiteSmokeParticleSystem(vec3_t vPosition, vec3_t vDirection) : CParticleSystem()
{
	m_vPosition = vPosition;
	m_vDirection = vDirection;
/*
	m_iStartingParticles = STARTING_WHITE_SMOKE_PARTICLES;
	m_flSystemMaxAge = WHITE_SMOKE_SYSTEM_LIFE;*/

	m_iStartingParticles = 2;
	m_flSystemMaxAge = 3;

	pParticleTexture = LoadTGA(NULL, const_cast<char*>(WHITE_SMOKE_PARTICLE)); 

	int n = StartingParticles();
	for(int i = 0; i < n; i++) {
		CParticle *pParticle = new CWhiteSmokeParticle(m_vPosition, m_vDirection, pParticleTexture);
		AddParticle(pParticle);
	}
}

// creates a flintlock smoke particle system
CBrownSmokeParticleSystem::CBrownSmokeParticleSystem(vec3_t vPosition, vec3_t vDirection) : CParticleSystem()
{
	m_vPosition = vPosition;
	m_vDirection = vDirection;

	m_iStartingParticles = STARTING_BROWN_SMOKE_PARTICLES;
	m_flSystemMaxAge = BROWN_SMOKE_SYSTEM_LIFE;

	pParticleTexture = LoadTGA(NULL, const_cast<char*>(BROWN_SMOKE_PARTICLE)); 

	int n = StartingParticles();
	for(int i = 0; i < n; i++) {
		CParticle *pParticle = new CWhiteSmokeParticle(m_vPosition, m_vDirection, pParticleTexture);
		AddParticle(pParticle);
	}
}

CMuzzleFlashParticleSystem::CMuzzleFlashParticleSystem(vec3_t vPosition, vec3_t vDirection, int iType) : CParticleSystem()
{
	m_vPosition = vPosition;
	m_vDirection = vDirection;

	m_iStartingParticles = 1;
	m_flSystemMaxAge = 1;
/*
	if (iType == MUZZ_AK) 
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_AK)); 
	else if (iType == MUZZ_DEAGLE) 
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_DEAGLE)); 
	else if (iType == MUZZ_M16) 
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_M16)); 
	else if (iType == MUZZ_M4A1) 
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_M4A1)); 
	else if (iType == MUZZ_M249) 
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_M249)); 
	else
	pParticleTexture = LoadTGA(NULL, const_cast<char*>(MUZZ_PARTICLE_GEN)); 
*/
	int n = StartingParticles();
	for(int i = 0; i < n; i++) {
		CParticle *pParticle = new CMuzzleFlashParticle(m_vPosition, m_vDirection, pParticleTexture);
		AddParticle(pParticle);
	}
}