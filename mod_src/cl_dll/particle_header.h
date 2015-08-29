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

// includes all other particle headers

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
