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

// definition of the grass particle

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "gl/gl.h"
#include "gl/glaux.h"
#include "com_model.h"	// Fograin92: alight_t
#include "pm_shared.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "particle_header.h"

extern vec3_t v_angles;
// creates a grass particle
CGrassParticle::CGrassParticle(grass_particle_system *pSystem, particle_texture_s *pParticleTexture) : CParticle()
{	
	pSys = pSystem;
	sParticle.pTexture = pParticleTexture;
	InitValues();
}

// give all variables default values
void CGrassParticle::InitValues( void ) 
{
	sParticle.vPosition = pSys->vPostion;
	sParticle.vDirection = pSys->vDirection;
	sParticle.vVelocity = Vector(0, 0, 0);
	
	sParticle.iSystemId = pSys->iId;
	sParticle.iRed = 255;
	sParticle.iBlue = sParticle.iGreen = sParticle.iRed;
	sParticle.iTransparency = pSys->iTransparency;

//if( pSys->bUseRandom )
		
	if(pSys->bUseRandom == true)
	sParticle.flSize = pSys->flSize + gEngfuncs.pfnRandomFloat( 0.0, pSys->fRandomVal );
	else
	sParticle.flSize = pSys->flSize;

	sParticle.bIgnoreSort = pSys->bIgnoreSort;

	m_bCollisionChecked = false;
	m_bIngoreParticle = false;
	
	float flLeanMin = 90 - pSys->flLeaningMin;
	float flLeanMax = 90 - pSys->flLeaningMax;

	m_vNormal[0] = gEngfuncs.pfnRandomFloat( flLeanMin, flLeanMax );
	m_vNormal[1] = pSys->Yaw;
	m_vNormal[2] = gEngfuncs.pfnRandomFloat( 0.0, 0.0 );

	m_vWaveNormal[0] = m_vNormal[0];
	m_vWaveNormal[1] = m_vNormal[1];
	m_vWaveNormal[2] = m_vNormal[2];

	// Level of detail scattering.  1/6 grass particles will go anywhere allowed
//	m_flLodMinDistance = GRASS_THRESHOLD_START;//600

	m_flLodMinDistance = CVAR_GET_FLOAT("cl_particle_min_lod_dist");//600
	
//	int OneInSix = (int)gEngfuncs.pfnRandomFloat( 0, 6 );
/*
	if (OneInSix == 1)
		m_flLodMaxDistance = 10000.0f; // as far as the eye can see
	else 
		m_flLodMaxDistance = GRASS_THRESHOLD_END;
*/
	int OneInSix = (int)gEngfuncs.pfnRandomFloat( 0, 50 );

	if (OneInSix == 1)
		m_flLodMaxDistance = 10000.0f; // as far as the eye can see
	else 
		m_flLodMaxDistance = CVAR_GET_FLOAT("cl_particle_max_lod_dist");


	m_flParticleTime = 0.0;

	// great thanks go to Sneaky_Bastard and randomnine for helping me alot to finding 
	// this great method of obtaining lightlevel on the clientside
	
	// create temporary entity to get illumination from it
	alight_t lighting;
	cl_entity_t *LightEntity;
	vec3_t dir;

	// bogus data as valve calls it :)
	LightEntity = gEngfuncs.GetLocalPlayer();

	if ( !LightEntity )
		return;

	// move it in the particles location
	LightEntity->origin = sParticle.vPosition;
	
	// I have no idea what this does but if you don't do it -> crash
	lighting.plightvec = dir;

	IEngineStudio.StudioDynamicLight(LightEntity, &lighting );
	IEngineStudio.StudioSetupLighting (&lighting);
	IEngineStudio.StudioEntityLight( &lighting );
	
	sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight /*+50*/);
	sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight /*+50*/);
	sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight /*+50*/);
}

// draws this grass particle
//extern void RenderFog ( void );	// Fograin92: Disabled
void CGrassParticle::Draw( void )
{
	if (m_bIngoreParticle == true)
		return;

	if(sParticle.pTexture == NULL) {
		gEngfuncs.Con_Printf("Null texture in particle\n");
		return;
	}

	Vector vForward, vRight, vUp, vDir;
	AngleVectors(v_angles, vForward, vRight, vUp );
	vDir = ( sParticle.vPosition - flPlayerOrigin ).Normalize( );
	if ( DotProduct ( vDir, vForward ) < 0 )
		return;

	int iHealth = 0;

	// lets make sure transparency doesn't overflow or udnerflow
	if (sParticle.iTransparency > 255)
		sParticle.iTransparency = 255;
	if (sParticle.iTransparency < 0)
		sParticle.iTransparency = 0;
	iHealth = sParticle.iTransparency;

	if (pSys->bLOD) // fade out particles that are further away (LOD)
	{
		float flDistance = sqrt(sParticle.flSquareDistanceToPlayer);
		if ((flDistance > m_flLodMinDistance) && (flDistance < m_flLodMaxDistance))
		{
			float flTransparencyFactor = 1 - ((flDistance - m_flLodMinDistance) / (m_flLodMaxDistance - m_flLodMinDistance));
				
			if (flTransparencyFactor > 1)
				flTransparencyFactor = 1;
			if (flTransparencyFactor < 0)
				flTransparencyFactor = 0;
			iHealth *= flTransparencyFactor;
		}
	}

	vec3_t vPoint, vPosition;
	vec3_t vWaveForward, vWaveRight, vWaveUp;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	AngleVectors(m_vNormal, vForward, vRight, vUp);
	AngleVectors(m_vWaveNormal, vWaveForward, vWaveRight, vWaveUp);

	glColor4ub(sParticle.iRed, sParticle.iGreen, sParticle.iBlue,iHealth);  

	//RenderFog();	// Fograin92: Disabled

	// Finally, we draw the particle
	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0.95f);
	VectorMA (sParticle.vPosition, sParticle.flSize, vWaveUp, vPoint);
	VectorMA (vPoint, -sParticle.flSize, vWaveRight, vPoint);
	glVertex3fv(vPoint); 

	glTexCoord2f(0.95f, 0.95f); 
	VectorMA (sParticle.vPosition, sParticle.flSize, vWaveUp, vPoint);
	VectorMA (vPoint, sParticle.flSize, vWaveRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(0.95f, 0);
	VectorMA (sParticle.vPosition, -sParticle.flSize, vUp, vPoint);
	VectorMA (vPoint, sParticle.flSize, vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(0, 0);
	VectorMA (sParticle.vPosition, -sParticle.flSize, vUp, vPoint);
	VectorMA (vPoint, -sParticle.flSize, vRight, vPoint);
	glVertex3fv(vPoint); 
   
	glEnd();
}

// update our particles internals, making it wave
void CGrassParticle::Update(float flTimeSinceLastDraw)
{
	m_flParticleTime += flTimeSinceLastDraw;
	
	// shouldn't be possible
	if (flTimeSinceLastDraw < 0)
		flTimeSinceLastDraw = -flTimeSinceLastDraw;

	// don't draw particles that are a league away
	if (pSys->bLOD) {	
		if (sqrt(sParticle.flSquareDistanceToPlayer) > m_flLodMaxDistance)
			m_bIngoreParticle = true;
		else
			m_bIngoreParticle = false;
	}
	
	//wind for grass
	//make it wave
	if (pSys->flWaveSpeed > 0)	
	{
		float m_flWaveState = ((pSys->flWaveSpeed / 3) * flTimeSinceLastDraw * 
			cos((m_flParticleTime * pSys->flWaveSpeed / 30) + 
			(sParticle.vPosition.x / 100) + (sParticle.vPosition.y / 100)));
		if (m_vWaveNormal.y > 180)
			m_vWaveNormal.x += m_flWaveState;
		else
			m_vWaveNormal.x -= m_flWaveState;
	}
	
	VectorMA( sParticle.vPosition, 60.0*flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );
}

// testing is used to get the particle to the ground rather than killing it here
bool CGrassParticle::Test()
{
	if( !pSys )
		return false;

	// We dont want the particle to drop on the ground so don't calc anything
	if (pSys->bDropOnGround == false)
		return true;
	
	// if grass particle is on ground, we don't need to do any more checks so return true
	if (m_bCollisionChecked == true)
		return true;

	// Drop particle on ground
	pmtrace_t* pTrace = NULL;
	// ignore no entity
	int iIgnoreEnt = -1;	

	pTrace = gEngfuncs.PM_TraceLine( sParticle.vPosition, (sParticle.vPosition + Vector( 0, 0, -1 ) * 8192), PM_TRACELINE_PHYSENTSONLY, 2, iIgnoreEnt );
	
	// This makes sure grass surfaces leaning to the side will also have their lowest point on ground
	float flAngleFactor = sin(((90 - m_vNormal.x) / 180) * 3.1516f);
	
	float flGrassHeight = (sParticle.flSize * flAngleFactor ); 

	// set new origin
	sParticle.vPosition.x = pTrace->endpos.x;
	sParticle.vPosition.y = pTrace->endpos.y;
	sParticle.vPosition.z = pTrace->endpos.z + flGrassHeight;

	// we've hit the ground we don't need to do this again
	m_bCollisionChecked = true;

	return true;
}
