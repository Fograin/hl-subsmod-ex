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
#include <windows.h>

#include "particle_header.h"
#include "event_api.h"
#include "r_efx.h"
#include "pm_shared.h"

CParticleSystemManager *pParticleManager = NULL;
cvar_t* g_ParticleCount;
cvar_t* g_ParticleDebug;
cvar_t* g_ParticleSorts;

// updates all systems
void CParticleSystemManager::UpdateSystems( void )
{
	CParticleSystem *pSystem = NULL;
	signed int i = 0;
	signed int iSystems = (signed)m_pParticleSystems.size();
	// iterate through all the particle systems, drawing each
	for (; i < iSystems; i++)
	{
		pSystem = m_pParticleSystems[i];
		// remove the system if the system requests it
		if( pSystem && pSystem->DrawSystem() == false)
		{
			delete pSystem;
			pSystem = NULL;
			m_pParticleSystems.erase((m_pParticleSystems.begin() + i));
			i--;
			iSystems--;
		}
	}

	// we couldn't return earlier as we need to have the sorting before the ps updating
	// however no sorting when we can't see the particles
	if(CheckDrawSystem() == false)
		return;

	// prepare opengl
	Particle_InitOpenGL();

	// declated variables we need for both unsorted and sorted
	int iParticles = m_pUnsortedParticles.size();

	float 	flTimeSinceLastDraw = TimeSinceLastDraw();

/*	if ( CVAR_GET_FLOAT( "slowmo" ) != 0 )
	flTimeSinceLastDraw = (TimeSinceLastDraw() * 25) /100;
	else
	flTimeSinceLastDraw = TimeSinceLastDraw();
*/
	int iDrawn = 0;

	// draw all unsorted particles first, so they are at the back of the screen.
	if(iParticles > 0) {
		// loop through all particles drawing them
		CParticle *pParticle = NULL;
		for(i = 0; i < iParticles ; i++) {
			if(m_pUnsortedParticles[i]) {
				pParticle = m_pUnsortedParticles[i];
				if(pParticle && pParticle->Test()) {
					pParticle->Update(flTimeSinceLastDraw);

					// don't draw in certain spec modes
					if(g_iUser1 != OBS_MAP_FREE && g_iUser1 != OBS_MAP_CHASE) {
						// unfortunately we have to prepare every particle now
						// as we can't prepare for a batch of the same type anymore
						pParticle->Prepare(); 
						pParticle->Draw();
						iDrawn++;
					}
				// particle wants to die, so kill it
				} else {
					RemoveParticle(pParticle);
					i--;
					iParticles--;
				}
			}
		}
	}

	iParticles = m_pParticles.size();

	// sort and draw the sorted particles list
	if(iParticles > 0) {
		// calculate the fraction of a second between sorts
		float flTimeSinceLastSort = (gEngfuncs.GetClientTime() - m_flLastSort);
		// 1 / time between sorts will give us a number like 5
		// if it is less than the particlesorts cvar then it is a small value 
		// and therefore a long time since last sort
		if((((int)(1 / flTimeSinceLastSort)) < g_ParticleSorts->value)) {
			m_flLastSort = gEngfuncs.GetClientTime();
			std::sort(m_pParticles.begin(), m_pParticles.end(), less_than);
		}

		// loop through all particles drawing them
		CParticle *pParticle = NULL;
		for(i = 0; i < iParticles ; i++) {
			if(m_pParticles[i]) {
				pParticle = m_pParticles[i];
				if(pParticle && pParticle->Test()) {
					pParticle->Update(flTimeSinceLastDraw);

					// don't draw in certain spec modes
					if(g_iUser1 != OBS_MAP_FREE && g_iUser1 != OBS_MAP_CHASE) {
						// unfortunately we have to prepare every particle now
						// as we can't prepare for a batch of the same type anymore
						pParticle->Prepare(); 
						pParticle->Draw();
						iDrawn++;
					}
				// particle wants to die, so kill it
				} else {
					RemoveParticle(pParticle);
					i--;
					iParticles--;
				}
			}
		}
	}

	// finished particle drawing
	Particle_FinishOpenGL();

	// print out how fast we've been drawing the systems in debug mode
	if (g_ParticleDebug->value != 0 && ((m_flLastDebug + 1) <= gEngfuncs.GetClientTime()))
	{
		gEngfuncs.Con_Printf("%i Particles Drawn this pass in %i systems %i Textures in Cache\n\0", iDrawn, m_pParticleSystems.size(), m_pTextures.size());
		m_flLastDebug = gEngfuncs.GetClientTime();
	}

	m_flLastDraw = gEngfuncs.GetClientTime();
}

// Handles all the present particle systems
void CParticleSystemManager::CreatePresetPS(unsigned int iPreset, particle_system_management *pSystem)
{
	// Fograin92: Choose particle FX
	switch(iPreset)
	{
		// Fograin92: Hit impact / Red blood particles (FULL FX)
		case iImpactBloodRed:
			CreateMappedPS("particles/gore/blood_red_animated.txt", pSystem);	// Blood impact animated
			CreateMappedPS("particles/gore/blood_red_impact.txt", pSystem);		// Blood impact-mist
		break;

		// Fograin92: Hit impact / Red blood particles (LOW FX)
		case iImpactBloodRedLOW:
			CreateMappedPS("particles/gore/blood_red_animated.txt", pSystem);	// Blood impact animated
			CreateMappedPS("particles/gore/blood_red_impact.txt", pSystem);		// Blood impact-mist
		break;

		// Fograin92: Hit impact / Alien blood particles (FULL FX)
		case iImpactBloodYellow:
			CreateMappedPS("particles/gore/blood_yellow_animated.txt", pSystem);	// Blood impact animated
			CreateMappedPS("particles/gore/blood_yellow_impact.txt", pSystem);		// Blood impact-mist
		break;

		// Fograin92: Hit impact / Alien blood particles (LOW FX)
		case iImpactBloodYellowLOW:
			CreateMappedPS("particles/gore/blood_yellow_animated.txt", pSystem);	// Blood impact animated
			CreateMappedPS("particles/gore/blood_yellow_impact.txt", pSystem);		// Blood impact-mist
		break;

		// Fograin92: Water hit impact / splash (FULL FX)
		case iImpactWater:
			CreateMappedPS("particles/water/water_impact_core.txt", pSystem);
			CreateMappedPS("particles/water/water_impact_drops.txt", pSystem);
			CreateMappedPS("particles/water/water_impact_wave.txt", pSystem);
		break;

		// Fograin92: Default explosion (FULL FX)
		case iExplosionDefault:
		{
			CreateMappedPS("particles/exp/explo1_fire.txt", pSystem);
			CreateMappedPS("particles/exp/explo1_shockwave.txt", pSystem);

			// Create dynamic light
			dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight (0);
			VectorCopy (pSystem->vPosition, dl->origin);
			dl->radius = 350;//500
			dl->color.r = 245;//254;
			dl->color.g = 216;//160;
			dl->color.b = 200;//24;
			dl->decay = 0.2;
			dl->die = (gEngfuncs.GetClientTime() + 0.2);
		}
		break;

		// Fograin92: BSP Impact, Concrete
		case iImpactBSPconcrete:
			CreateMappedPS("particles/impact/e_impacts_chunks.txt", pSystem);
			CreateMappedPS("particles/impact/e_impacts_smoke.txt", pSystem);
		break;

		// Fograin92: Water waves
		case iDefaultWaves:
			CreateMappedPS("particles/water/water_waves.txt", pSystem);
		break;

		// Fograin92: Icky waves
		case iWaterIcky:
			CreateMappedPS("particles/water/water_icky_splash.txt", pSystem);
		break;

		// Fograin92: Glass impact
		case iImpactBSPglass:
			CreateMappedPS("particles/impact/e_impacts_glass.txt", pSystem);
		break;

		// Fograin92: Wood impact
		case iImpactBSPwood:
			CreateMappedPS("particles/impact/e_impacts_wood.txt", pSystem);
			CreateMappedPS("particles/impact/e_impacts_wood2.txt", pSystem);
			CreateMappedPS("particles/impact/e_impacts_wood3.txt", pSystem);
		break;

		// Fograin92: Metal-like impact
		case iImpactBSPmetal:
		case iImpactBSPvent:
		case iImpactBSPgrate:
		case iImpactBSPcomputer:
			// TODO: Some nice sparks?
			//CreateMappedPS("particles/impact/e_impacts_sparks.txt", pSystem);
		break;

		// Fograin92: Dynamic blood puddle (RED)
		case iBloodRedPuddle1:
			CreateMappedPS("particles/gore/blood_red_puddle1.txt", pSystem);
		break;

		case iBloodRedPuddle2:
			CreateMappedPS("particles/gore/blood_red_puddle2.txt", pSystem);
		break;

		// Fograin92: Dynamic blood puddle (ALIEN)
		case iBloodAlienPuddle1:
			CreateMappedPS("particles/gore/blood_yellow_puddle1.txt", pSystem);
		break;

		case iBloodAlienPuddle2:
			CreateMappedPS("particles/gore/blood_yellow_puddle2.txt", pSystem);
		break;

	}

	if(pSystem == NULL)
		return;



/*
// REFERENCE Purposes

	// Fograin92: REF for level based fire?
	// TODO: Add some fire after explosion?
	if(iPreset == iDefaultFire) 
	{
//		gEngfuncs.Con_Printf("iPreset == iDefaultFire\n");

		CreateMappedPS("particles/engine/e_fire.txt", pSystem);
		CreateMappedPS("particles/engine/e_fire_smoke_temp.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultSmoke) 
	{
//		gEngfuncs.Con_Printf("iPreset == iDefaultSmoke\n");

		CreateMappedPS("particles/engine/e_smoke.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultFinalFire) 
	{
//		gEngfuncs.Con_Printf("iPreset == iDefaultFinalFire\n");

		CreateMappedPS("particles/engine/e_fire_final.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultFinalSmoke) 
	{
//		gEngfuncs.Con_Printf("iPreset == iDefaultFinalSmoke\n");

		CreateMappedPS("particles/engine/e_fire_smoke.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}
	if(iPreset == iDefaultSmokeRPG) 
	{
		CreateMappedPS("particles/engine/e_smoke_rpg.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}	
	
	

	if(iPreset == iDefaultScorch) 
	{   
//		gEngfuncs.Con_Printf("iPreset == iDefaultScorch\n");
		
		CreateMappedPS("particles/engine/e_scorch.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultBloodRedPit) 
	{     
//		gEngfuncs.Con_Printf("iPreset == iDefaultBloodRedPit\n");
		
		CreateMappedPS("particles/engine/e_bloodpit_red.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}
		
	if(iPreset == iDefaultBloodGreenPit) 
	{    
//		gEngfuncs.Con_Printf("iPreset == iDefaultBloodGreenPit\n");
		
		CreateMappedPS("particles/engine/e_bloodpit_green.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultGasCanister) 
	{    
//		gEngfuncs.Con_Printf("iPreset == iDefaultGasCanister\n");
		
		CreateMappedPS("particles/engine/e_impacts_gascan_drops.txt", pSystem);
		CreateMappedPS("particles/engine/e_impacts_gascan_core.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultLeaves) 
	{    
//		gEngfuncs.Con_Printf("iPreset == iDefaultGasCanister\n");
		
		CreateMappedPS("particles/engine/e_impacts_leaves.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}
	
	if(iPreset == iDefaultTeleportWave) 
	{    		
		CreateMappedPS("particles/engine/e_teleport_wave.txt", pSystem);
		CreateMappedPS("particles/engine/e_teleport_portal.txt", pSystem);
		CreateMappedPS("particles/engine/e_teleport_flare.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}

		// create dynamic light
		dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight (0);
		VectorCopy (pSystem->vPosition, dl->origin);
		dl->radius = 222;
		dl->color.r = 100;
		dl->color.g = 160;
		dl->color.b = 24;
		dl->decay = 0.2;
		dl->die = (gEngfuncs.GetClientTime() + 3);
	}


	if(iPreset == iDefaultTeleportWave2) 
	{    	
		CreateMappedPS("particles/engine/e_teleport_portal_img.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultSlowMoEffect) 
	{    	
		CreateMappedPS("particles/engine/e_effect_slowmo.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == DEBUG_DOT) 
	{    	
		CreateMappedPS("particles/engine/dot.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}
	if(iPreset == DEBUG_DOT_RED) 
	{    	
		CreateMappedPS("particles/engine/dot_red.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}
	}

	if(iPreset == iDefaultExplosionGas) 
	{
		CreateMappedPS("particles/exp_gas_darksmoke.txt", pSystem);
		CreateMappedPS("particles/exp_gas_debris.txt", pSystem);
		CreateMappedPS("particles/exp_gas_fireball.txt", pSystem);
		CreateMappedPS("particles/exp_gas_glow.txt", pSystem);
		CreateMappedPS("particles/exp_gas_sparks.txt", pSystem);
		CreateMappedPS("particles/exp_gas_tong.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}

		// play a sound as well
		gEngfuncs.pEventAPI->EV_PlaySound( 0, pSystem->vPosition, 0, "sound\\weapons\\explode_dist.wav", 1.0, ATTN_NONE, 0, PITCH_NORM );

		// create dynamic light
		dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight (0);
		VectorCopy (pSystem->vPosition, dl->origin);
		dl->radius = 100;
		dl->color.r = 254;
		dl->color.g = 160;
		dl->color.b = 24;
		dl->decay = 0.2;
		dl->die = (gEngfuncs.GetClientTime() + 0.5);
	}

	if(iPreset == iDefaultFireSpark) 
	{
		CreateMappedPS("particles/engine/e_fire_spark.txt", pSystem);
		CreateMappedPS("particles/engine/e_smoke_rpg.txt", pSystem);

		if(pSystem == NULL) 
		{
			return;
		}

		// create dynamic light
	}
*/
}


// Grass system
void CParticleSystemManager::CreateGrassPS( char* sFile, particle_system_management* pSystem )
{
	if(pSystem == NULL) {
		return;
	}

	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return;

	//AddSystem(new CGrassParticleSystem(sFile, pSystem));
}


void CParticleSystemManager::CreateMuzzleFlash(vec3_t vPosition, vec3_t vDirection, int iType)
{
	if(CheckDrawSystem() == false)
		return;

	AddSystem(new CMuzzleFlashParticleSystem(vPosition, vDirection, iType));
}


// mapped ps
void CParticleSystemManager::CreateMappedPS( char* sFile, particle_system_management* pSystem )
{
	if(pSystem == NULL)
		return;

	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return;

	AddSystem(new CMappedParticleSystem(sFile, pSystem));
}

// are we allowed to draw atm
bool CParticleSystemManager::CheckDrawSystem( void )
{
	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_ALL))
		return false;

	// no d3d/software
	if (IEngineStudio.IsHardware() == false)
		return false;

	return true;
}

// adds a new texture to out cache
// using a map would be preferable but you can't snprintf into the index
void CParticleSystemManager::AddTexture(char* sName, particle_texture_s *pTexture) {
	// create a new entry and then fill it with the values
	particle_texture_cache *pCacheEntry = new particle_texture_cache;
	_snprintf(pCacheEntry->sTexture, MAX_PARTICLE_PATH-1, "%s\0", sName);
	pCacheEntry->pTexture = pTexture;

	// add the cache entry
	m_pTextures.push_back(pCacheEntry);
}

// check for a texture with the same path
particle_texture_s* CParticleSystemManager::HasTexture(char* sName) {

	unsigned int i = 0;
	unsigned int iTextures = m_pTextures.size();
	particle_texture_cache *pCacheEntry = NULL;

	// loop through all cache entries, comparing stored path with parameter path
	for (; i < iTextures; i++)
	{
		pCacheEntry = m_pTextures[i];
		if(!stricmp(pCacheEntry->sTexture, sName)) {
			return pCacheEntry->pTexture;
		}
	}
	// return the texture if we've found it, otherwise null
	return NULL;
}

// cache the most used tgas, so we don't get lag on first firing the gun
void CParticleSystemManager::PrecacheTextures( void ) 
{
	gEngfuncs.Con_Printf("Caching frequently used particles, this may take a few moments\n");

	// Fograin92: Red blood 
	LoadTGA(NULL, const_cast<char*>(blood_red_animated));
	LoadTGA(NULL, const_cast<char*>(blood_red_impact));
	LoadTGA(NULL, const_cast<char*>(blood_red_drips));
	LoadTGA(NULL, const_cast<char*>(blood_red_puddle1));
	LoadTGA(NULL, const_cast<char*>(blood_red_puddle2));

	// Fograin92: Yellow blood
	LoadTGA(NULL, const_cast<char*>(blood_yellow_animated));
	LoadTGA(NULL, const_cast<char*>(blood_yellow_impact));
	LoadTGA(NULL, const_cast<char*>(blood_yellow_drips));
	LoadTGA(NULL, const_cast<char*>(blood_yellow_puddle01));
	LoadTGA(NULL, const_cast<char*>(blood_yellow_puddle02));

	// Fograin92: Water hit impact / splash particles
	LoadTGA(NULL, const_cast<char*>(water_impact_core));
	LoadTGA(NULL, const_cast<char*>(water_impact_drops));
	LoadTGA(NULL, const_cast<char*>(water_impact_wave));

	// Fograin92: Default explosion
	LoadTGA(NULL, const_cast<char*>(explo01));
	LoadTGA(NULL, const_cast<char*>(shockwave));
	LoadTGA(NULL, const_cast<char*>(smoke1));
	LoadTGA(NULL, const_cast<char*>(smoke09));
	LoadTGA(NULL, const_cast<char*>(smoke10));

	// Fograin92: BSP Impact
	LoadTGA(NULL, const_cast<char*>(particle_black));
	LoadTGA(NULL, const_cast<char*>(particle_blue));
	LoadTGA(NULL, const_cast<char*>(particle_brown));
	LoadTGA(NULL, const_cast<char*>(particle_drop));
	LoadTGA(NULL, const_cast<char*>(particle_green));
	LoadTGA(NULL, const_cast<char*>(particle_grey));
	LoadTGA(NULL, const_cast<char*>(particle_red));
	LoadTGA(NULL, const_cast<char*>(particle_white));
	LoadTGA(NULL, const_cast<char*>(particle_yellow));

	LoadTGA(NULL, const_cast<char*>(fleck_cement1));
	LoadTGA(NULL, const_cast<char*>(particle_wood));
	LoadTGA(NULL, const_cast<char*>(particle_wood2));
	LoadTGA(NULL, const_cast<char*>(particle_wood3));
	LoadTGA(NULL, const_cast<char*>(glass_shards));
	


/*
//used by crete shot
	LoadTGA(NULL, const_cast<char*>(fleck_cement1));
	LoadTGA(NULL, const_cast<char*>(particle_grey));
	LoadTGA(NULL, const_cast<char*>(debris_crete));

	LoadTGA(NULL, const_cast<char*>(fleck_wood1));
	LoadTGA(NULL, const_cast<char*>(particle_brown));

	LoadTGA(NULL, const_cast<char*>(sparks01));
	LoadTGA(NULL, const_cast<char*>(particle_blood_splat01));
	LoadTGA(NULL, const_cast<char*>(particle_blood_splat02));
	LoadTGA(NULL, const_cast<char*>(glass));

//used by explosions
	LoadTGA(NULL, const_cast<char*>(exp1));
	LoadTGA(NULL, const_cast<char*>(exp_glow));
	LoadTGA(NULL, const_cast<char*>(smoke01));
	LoadTGA(NULL, const_cast<char*>(debris_concrete001a));
	LoadTGA(NULL, const_cast<char*>(spark01));
	LoadTGA(NULL, const_cast<char*>(exp_tong));
	LoadTGA(NULL, const_cast<char*>(smoke09));
	LoadTGA(NULL, const_cast<char*>(chunk));
	LoadTGA(NULL, const_cast<char*>(dirt01));
		



	LoadTGA(NULL, const_cast<char*>(BARREL_SMOKE_PARTICLES[0]));
	LoadTGA(NULL, const_cast<char*>(BARREL_SMOKE_PARTICLES[1]));
	LoadTGA(NULL, const_cast<char*>(BARREL_SMOKE_PARTICLES[2]));
*/
	gEngfuncs.Con_Printf("Finished caching frequently used particles, game loading will now continue\n");
}
// adds a particle into the global particle tracker
void CParticleSystemManager::AddParticle(CParticle* pParticle)
{
	if(pParticle->sParticle.bIgnoreSort == true)
		m_pUnsortedParticles.push_back(pParticle);
	else
		m_pParticles.push_back(pParticle);

	pParticle = NULL;
}

// removes a particle from the global tracker and from the system
void CParticleSystemManager::RemoveParticle(CParticle* pParticle)
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();

	// remove a particle from the sorted list
	for (; i < iParticles; i++) {
		if(pParticle == m_pParticles[i]) {
			delete m_pParticles[i];
			pParticle = NULL;
			m_pParticles.erase(m_pParticles.begin() + i);
			i--;
			iParticles--;
			return;
		}
	}

	// remove a particle from the unsorted list
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		if(pParticle == m_pUnsortedParticles[i]) {
			delete m_pUnsortedParticles[i];
			pParticle = NULL;
			m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
			i--;
			iParticles--;
			return;
		}
	}
}

// remove all trackers in the system
void CParticleSystemManager::RemoveParticles()
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();

	// remove the sorted particles
	for (i = 0; i < iParticles; i++) {
		delete m_pParticles[i];
		m_pParticles[i] = NULL;
		m_pParticles.erase(m_pParticles.begin() + i);
		i--;
		iParticles--;
	}
	m_pParticles.clear();

	// remove the unsorted particles
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		delete m_pUnsortedParticles[i];
		m_pUnsortedParticles[i] = NULL;
		m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
		i--;
		iParticles--;
	}
	m_pUnsortedParticles.clear();
}


// adds a new system
void CParticleSystemManager::AddSystem(CParticleSystem *pSystem) {
	m_pParticleSystems.push_back(pSystem);
}

// tbh highly inefficent but we shouldn't have any large number of ps's,
// and we won't be force removing very often so this won't be too bad
void CParticleSystemManager::RemoveSystem( unsigned int iSystem )
{
	unsigned int i = 0;
	unsigned int iParticles = m_pParticles.size();
	CParticle *pParticle = NULL;
	// remove the sorted particles
	for (i = 0; i < iParticles; i++) {
		pParticle = m_pParticles[i];
		if(pParticle && pParticle->SystemID() == iSystem) {
			delete pParticle;
			pParticle = NULL;
			m_pParticles.erase(m_pParticles.begin() + i);
			i--;
			iParticles--;
		}
	}

	// remove the unsorted particles
	iParticles = m_pUnsortedParticles.size();
	for (i = 0; i < iParticles; i++) {
		pParticle = m_pUnsortedParticles[i];
		if(pParticle && pParticle->SystemID() == iSystem) {
			delete pParticle;
			pParticle = NULL;
			m_pUnsortedParticles.erase(m_pUnsortedParticles.begin() + i);
			i--;
			iParticles--;
		}
	}

	CParticleSystem *pSystem = NULL;	
	unsigned int iSystems = m_pParticleSystems.size();
	for (; i < iSystems; i++)
	{
		pSystem = m_pParticleSystems[i];
		// i != the system id, as the server or the client can generate these
		if(pSystem && pSystem->SystemID() == iSystem) {
			delete pSystem;
			pSystem = NULL;
			m_pParticleSystems.erase(m_pParticleSystems.begin() + i);
			i--;
			iSystems--;
		}
	}
}


// deletes all systems
void CParticleSystemManager::RemoveSystems( void )
{
	unsigned int i = 0;
	unsigned int iSystems = m_pParticleSystems.size();
	for (; i < iSystems; i++) {
		delete m_pParticleSystems[i];
		m_pParticleSystems[i] = NULL;
		m_pParticleSystems.erase(m_pParticleSystems.begin() + i);
		i--;
		iSystems--;
	}
	m_pParticleSystems.clear();
}