//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	The Battle Grounds Team and Contributors.
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

// needed for all list work
#include <algorithm>
#include <vector>

using std::vector;

// lets do some forward declaring so we don't get into too much trouble
class CParticle;
class CParticleSystem;
class CParticleSystemManager;

// now the real headers
#include "particle_texture.h"
#include "particle_defs.h"
#include "system_manager.h"
#include "particle_systems.h"
#include "particle.h"

// our particle related externs

// percentage of particles we want
extern cvar_t* g_ParticleCount;
// are we debugging the particle system
extern cvar_t* g_ParticleDebug;
// how many sorts we are going to have a second
extern cvar_t* g_ParticleSorts;
// this player's position from pm_shared
extern "C" vec3_t flPlayerOrigin;
// sets up ogl for all particles
extern inline void Particle_InitOpenGL( void );
// "closes" opengl again when drawing is done so we don't screw up settings for HL
extern inline void Particle_FinishOpenGL( void );
// loads a tga to a pointer
extern particle_texture_s * LoadTGA(particle_texture_s *pTexture, char* filename);
// this is the pointer that all particle systems add themselves to.
// it must be the only one as it deletes the particle systems
// and we don't want dangling pointers
extern CParticleSystemManager *pParticleManager;

#endif
