//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	The Battle Grounds Team and Contributors.
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>

#include "particle_header.h"

// creates a grass particle system
CGrassParticleSystem::CGrassParticleSystem( char *sParticleDefinition, particle_system_management *pSysDetails) : CParticleSystem()
{
	m_sParticleFile = sParticleDefinition;
	m_iID = pSysDetails->iID;
	
	// only do stuff on succesful
	if(LoadParticleDefinition(pSysDetails)) {
		m_flNewParticles = 0;

		// loop through all particle grass types adding them in
		for(unsigned int i = 0; i < m_cGrassTypes.size(); i++) {
			grass_particle_types *pGrassType = m_cGrassTypes[i];
			pGrassType->pParticleTexture = LoadTGA(NULL, pGrassType->pSystem->sParticleTexture);
			pGrassType->pSystem->iId = m_iID;

			if(pGrassType->pParticleTexture == NULL)
				continue;

			// add all the starting particles we were asked to
			float flGrassPercentage = (CVAR_GET_FLOAT("cl_grassamount")/100);

			if (flGrassPercentage < 0)
				flGrassPercentage = 0;
			if (flGrassPercentage > 50)
				flGrassPercentage = 50;

			int iParticles = (int)(pGrassType->pSystem->iCount * flGrassPercentage);

			for(int j = 0; j < iParticles; j++) {
				// randomize this particle's position
				Vector vOrigin;
				vOrigin.x = gEngfuncs.pfnRandomFloat(pSysDetails->vAbsMin.x, pSysDetails->vAbsMax.x);
				vOrigin.y = gEngfuncs.pfnRandomFloat(pSysDetails->vAbsMin.y, pSysDetails->vAbsMax.y);
				vOrigin.z = gEngfuncs.pfnRandomFloat(pSysDetails->vAbsMin.z, pSysDetails->vAbsMax.z);
				pGrassType->pSystem->vPostion = vOrigin;
				
				pGrassType->pSystem->Yaw = gEngfuncs.pfnRandomFloat( 0.0, 360.0 );

				AddParticle(new CGrassParticle(pGrassType->pSystem, pGrassType->pParticleTexture), pGrassType);
			}
		}
	} else {
		m_flSystemMaxAge = 0.01;
	}
}

// removes all particles
CGrassParticleSystem::~CGrassParticleSystem()
{
	// loop through our vector of types
	unsigned int i = 0;
	unsigned int iGrasses = m_cGrassTypes.size();
	grass_particle_types *pGrassType = NULL;
	for (; i < iGrasses; i++)
	{
		// only delete the system.
		// the texture and the particle are deleted by the system manager
		pGrassType = m_cGrassTypes[i];
		delete pGrassType->pSystem;
		pGrassType->pSystem = NULL; 

		delete pGrassType;
		pGrassType = NULL;
		m_cGrassTypes.erase(m_cGrassTypes.begin() + i);
		i--;
		iGrasses--;
	}
}

// adds a particle to the system
void CGrassParticleSystem::AddParticle(CParticle* pParticle, grass_particle_types* pGrassType)
{
	CParticleSystem::AddParticle(pParticle);
}

// set the defaults so that mappers don't have to define properties if they aren't the defaults
grass_particle_system * CGrassParticleSystem::CreateDefaultParticleSystem( void )
{
	grass_particle_system *m_pSystem = new grass_particle_system;

	_snprintf(m_pSystem->sParticleTexture, MAX_PARTICLE_PATH, "particles/grass01.tga");

	m_pSystem->vAbsMax.x = 0;
	m_pSystem->vAbsMax.y = 0;
	m_pSystem->vAbsMax.z = 0;

	m_pSystem->vAbsMin.x = 0;
	m_pSystem->vAbsMin.y = 0;
	m_pSystem->vAbsMin.z = 0;

	m_pSystem->flWaveSpeed = 0.0;
	m_pSystem->flLeaningMax = 45.0;
	m_pSystem->flLeaningMin = 90.0;

	m_pSystem->flSize = 30.0;
	m_pSystem->iCount = 20;
	m_pSystem->iTransparency = 255;

	m_pSystem->bLOD = true;
	m_pSystem->bDropOnGround = true;
	m_pSystem->bIgnoreSort = true;

	return m_pSystem;
}

// loads and parses the particle definition file
bool CGrassParticleSystem::LoadParticleDefinition( particle_system_management *pSysDetails )
{
	// check we've set a file
	if(!m_sParticleFile) {
		gEngfuncs.Con_Printf("No Grass Particle definition file specified");
		return false;
	}

	// load file checking it exists
	char *sFile = (char *)gEngfuncs.COM_LoadFile(m_sParticleFile, 5 , null);
	if(!sFile) {
		gEngfuncs.Con_Printf("Bad Grass Particle definition file specified %s\n", m_sParticleFile);
		return false;
	}

	char sSetting[256];
	char sValue[256];

	// pointer to the current system
	grass_particle_system *pSystem = NULL;

	// better than while(1)
	bool bLooping = true;
	while(bLooping) {
		// read the first chunk.  it should be a setting
		sFile = gEngfuncs.COM_ParseFile(sFile, sSetting);

		// eof before a setting is fine
		if(!sFile) {
			break;
		}

		// not eof but no setting... !?
		if(!sSetting) {
			gEngfuncs.Con_Printf("Unexpected error in file after %sin %s", sValue, m_sParticleFile);
			gEngfuncs.COM_FreeFile(sFile);
			return false;
		}

		// we're finished with this particle, create a new particle and parse
		if((!stricmp(sSetting, "new_particle"))) {
			pSystem = CreateDefaultParticleSystem();
			pSystem->vDirection = pSysDetails->vDirection;
			pSystem->vAbsMax = pSysDetails->vAbsMax;
			pSystem->vAbsMin = pSysDetails->vAbsMin;
			pSystem->vPostion = pSysDetails->vPosition;

			// create the new gras type
			grass_particle_types *pGrassSystem = new grass_particle_types;
			pGrassSystem->pSystem = pSystem;
			pGrassSystem->pParticleTexture = NULL;
			m_cGrassTypes.push_back(pGrassSystem);
			continue;
		}

		// getting the value for this setting as setting and value's should be paired
		sFile = gEngfuncs.COM_ParseFile(sFile, sValue);

		// hitting eof is unaccpetable so no need to check for it and return true;
		if(!sValue) {
			gEngfuncs.Con_Printf("Unexpected error in file after %s in %s", sSetting, m_sParticleFile);
			gEngfuncs.COM_FreeFile(sFile);
			return false;
		}

		// first particle
		if(pSystem == NULL) {
			pSystem = CreateDefaultParticleSystem();
			pSystem->vDirection = pSysDetails->vDirection;
			pSystem->vAbsMax = pSysDetails->vAbsMax;
			pSystem->vAbsMin = pSysDetails->vAbsMin;
			pSystem->vPostion = pSysDetails->vPosition;

			// create the new gras type
			grass_particle_types *pGrassSystem = new grass_particle_types;
			pGrassSystem->pSystem = pSystem;
			pGrassSystem->pParticleTexture = NULL;
			m_cGrassTypes.push_back(pGrassSystem);
		}

		// temp values so we don't have to keep converting when performing calcs on the inputted variables
		int iTemp = 0;

		// particle texture path
		if(!stricmp(sSetting, "texture")) {
			_snprintf(pSystem->sParticleTexture, MAX_PARTICLE_PATH, "%s", sValue);
		}
		// size of texture * this value 
		else if(!stricmp(sSetting, "scale")) {
			pSystem->flSize = atof(sValue);
		}
		// the maximum amount this grass can lean
		else if(!stricmp(sSetting, "leaning_max")) {
			pSystem->flLeaningMax = atof(sValue);
		}
		// the maximum amount this grass can lean
		else if(!stricmp(sSetting, "leaning_min")) {
			pSystem->flLeaningMin = atof(sValue);
		}
		// how fast the wind is going
		else if(!stricmp(sSetting, "wave_speed")) {
			pSystem->flWaveSpeed = atof(sValue);
		}
		// the number of particles in this system
		else if(!stricmp(sSetting, "particles")) {
			pSystem->iCount = atoi(sValue);
		}
		// how transparent the particles are
		else if(!stricmp(sSetting, "transparency")) {
			iTemp = atoi(sValue);
		
			// make sure it isn't out of range
			if(iTemp > 255 || iTemp < 0)
				pSystem->iTransparency = 255;
			else
				pSystem->iTransparency = iTemp;
		}
		// level of detail
		else if(!stricmp(sSetting, "lod")) {
			pSystem->bLOD = (!!(atoi(sValue)));
		}
		// drop on ground ??
		else if(!stricmp(sSetting, "drop_on_ground")) {
			pSystem->bDropOnGround = (!!(atoi(sValue)));
		}
		// ignore_sort - Do not sort these particles with the rest ( saves cpu time )
		else if(!stricmp(sSetting, "ignore_sort")) {
			pSystem->bIgnoreSort = (!!(atoi(sValue)));
		}
		// randomize - set different scales based on iRandomVal's value
		else if(!stricmp(sSetting, "randomize")) {
			pSystem->bUseRandom = (!!(atoi(sValue)));
		}
		// different scales values
		else if(!stricmp(sSetting, "random_modifier")) {
			pSystem->fRandomVal = atof(sValue);
		}
		// catch all to accomodate for my spelling at first and then mapper's spelling later on
		else {
			gEngfuncs.Con_Printf("Unknown setting - %s in %s\n\0", sSetting, m_sParticleFile);
		}
	}
	return true;
}
