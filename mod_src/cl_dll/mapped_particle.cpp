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
#define HULL_PLANE_SKIP
#include "com_model.h"
#include "pm_shared.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "particle_header.h"

extern vec3_t v_angles;

// creates the mapped particle
CMappedParticle::CMappedParticle(mapped_particle_system *pSystem, particle_texture_s *pParticleTexture) : CParticle()
{
	pSys = pSystem;
	sParticle.pTexture = pParticleTexture;
	InitValues();
}

// Loads the values the mapper set
void CMappedParticle::InitValues( void )
{
	sParticle.vPosition = pSys->vPostion;
	sParticle.vDirection = pSys->vDirection;
	sParticle.vVelocity = pSys->vStartingVel;

	sParticle.iSystemId = pSys->iId;
	sParticle.iRed = 255;
	sParticle.iBlue = sParticle.iGreen = sParticle.iRed;
	sParticle.iTransparency = pSys->iTransparency;

	sParticle.bUseTrans = true;
	sParticle.bIgnoreSort = pSys->bIgnoreSort;

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -pSys->flParticleRotationSpeed, pSys->flParticleRotationSpeed);
	sParticle.flCurrentRotation = 0.0;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = pSys->flParticleLife + gEngfuncs.pfnRandomFloat( -pSys->flParticleLifeVariation, pSys->flParticleLifeVariation );

	sParticle.flSize = pSys->flParticleScaleSize;
	sParticle.flGrowth = pSys->flScaleIncreasePerSecond;
	sParticle.flScale = 30.0f;

	if(pSys->iSystemShape == SHAPE_PLANE) {
		sParticle.vPosition.x = sParticle.vPosition.x + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneXLength/2), (float)(pSys->iPlaneXLength/2));
		sParticle.vPosition.y = sParticle.vPosition.y + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneYLength/2), (float)(pSys->iPlaneYLength/2));
	} else if(pSys->iSystemShape == SHAPE_AROUND_PLAYER) {
		cl_entity_t* m_pPlayer = gEngfuncs.GetLocalPlayer();
		sParticle.vPosition.x = (m_pPlayer->origin.x + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneXLength), pSys->iPlaneXLength ));
		sParticle.vPosition.y = (m_pPlayer->origin.y + gEngfuncs.pfnRandomFloat( -(float)(pSys->iPlaneYLength), pSys->iPlaneYLength));
		sParticle.vPosition.z = (m_pPlayer->origin.z + 450.0);
	}

	if(pSys->bWindy == true) {
		sParticle.vWind.x = gEngfuncs.pfnRandomFloat( 0.01f,0.74f );
		sParticle.vWind.y = gEngfuncs.pfnRandomFloat( 0.01f,0.81f );
		sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.01f,0.64f );
	}

	sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.x - pSys->vVelocityVar.x), (sParticle.vVelocity.x + pSys->vVelocityVar.x));
	sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.y - pSys->vVelocityVar.y), (sParticle.vVelocity.y + pSys->vVelocityVar.y));
	sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat((sParticle.vVelocity.z - pSys->vVelocityVar.z), (sParticle.vVelocity.z + pSys->vVelocityVar.z));

	flParticleTime = 0.0;
	bWindChanged = false;
	bIgnoreParticle = false;

	for(int i = 0; i < 3; i++) {
		bCollisionChecked[i] = false;
		iCollisionTimer[i] = 0;
	}

	sParticle.flLastSort = 0.0;
	sParticle.flSquareDistanceToPlayer = 0.0;

	vRotationVelVar.x = gEngfuncs.pfnRandomFloat(-pSys->vRotationVelVarMax.x/2, pSys->vRotationVelVarMax.x/2);
	vRotationVelVar.y = gEngfuncs.pfnRandomFloat(-pSys->vRotationVelVarMax.y/2, pSys->vRotationVelVarMax.y/2);
	vRotationVelVar.z = gEngfuncs.pfnRandomFloat(-pSys->vRotationVelVarMax.z/2, pSys->vRotationVelVarMax.z/2);

	vNormal.x = 0;
	vNormal.y = 0;
	vNormal.z = 0;

	// check lightlevel once when the particle is spawned
//	if (pSys->iParticleLightCheck == NO_CHECK)
		
	if (pSys->iParticleLightCheck == CHECK_ONCE)
	{
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
		
		sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight);
		sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight);
		sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight);
	}

}

// Draws a mapped particle
// draws a mapped particle
void CMappedParticle::Draw( void )
{
	if(bIgnoreParticle)
		return;

	if(sParticle.pTexture == NULL)
		return;

	Vector vForward, vRight, vUp, vDir;
	AngleVectors(v_angles, vForward, vRight, vUp );
	vDir = ( sParticle.vPosition - flPlayerOrigin ).Normalize( );

	// Fograin92: Disabled because bullshit happens
	//if ( DotProduct ( vDir, vForward ) < 0 )
	//	return;

	int iHealth = 0;

	// fading in happens in the first half of the particles lifetime
	if (pSys->bFadeIn && ((sParticle.flAge / sParticle.flMaxAge) < 0.5)) {
		iHealth = (sParticle.flAge * sParticle.iTransparency / (sParticle.flMaxAge / 2));
		if (iHealth < 0)
			return;

	} 
	// fading out happens in the second half of the particles lifetime
	else if (pSys->bFadeOut && ((sParticle.flAge / sParticle.flMaxAge) > 0.5)) {
		iHealth = sParticle.iTransparency - ((sParticle.flAge - (sParticle.flMaxAge / 2)) * sParticle.iTransparency / (sParticle.flMaxAge / 2));
		if (iHealth < 0)
			return;
	} 
	// make sure we don't go over the limits
	else
	{
		if (sParticle.iTransparency > 255)
			sParticle.iTransparency = 255;
		if (sParticle.iTransparency < 0)
			sParticle.iTransparency = 0;
		iHealth = sParticle.iTransparency;
	}

	// fade out particles that are closer to the player if smoke is true
	if (pSys->bSmoke) {
		float flThresholdStartSqrd = (PARTICLE_THRESHOLD_START * PARTICLE_THRESHOLD_START);
		float flThresholdEndSqrd = (PARTICLE_THRESHOLD_END * PARTICLE_THRESHOLD_END);

		float flDistance = sParticle.flSquareDistanceToPlayer;
		if ((flDistance > flThresholdStartSqrd) && (flDistance < flThresholdEndSqrd)) {
			float flTransparencyFactor = ((flDistance - flThresholdStartSqrd) / (flThresholdEndSqrd - flThresholdStartSqrd));
			if (flTransparencyFactor > 1)
				flTransparencyFactor = 1;
			if (flTransparencyFactor <0)
				flTransparencyFactor = 0;
			iHealth *= flTransparencyFactor;
		}
	}

	vec3_t vPoint, vPosition;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	// particle aligning
	// We then get the view angles for the player so that we can "billboard" the sprites 
	if (pSys->iParticleAlign == PLAYER_VIEW)
		{
		if(g_iUser1 == OBS_IN_EYE || g_iUser1 == OBS_CHASE_LOCKED) {
			cl_entity_t* pEnt = gEngfuncs.GetEntityByIndex( g_iUser2 );
			VectorCopy(pEnt->angles, vNormal);
			vNormal[0]*=-3.0f; // no idea view.cpp said "see CL_ProcessEntityUpdate()"
		} else {
			vNormal = v_angles;
		}
	}

	if (pSys->iParticleAlign == PLANAR) {
		vNormal.x = 90;
		vNormal.y = 0;
		vNormal.z = 0;
	}
	
	if (pSys->iParticleAlign == LOCKED_Z) {
		if(g_iUser1 == OBS_IN_EYE || g_iUser1 == OBS_CHASE_LOCKED) {
			cl_entity_t* pEnt = gEngfuncs.GetEntityByIndex( g_iUser2 );
			VectorCopy(pEnt->angles, vNormal);
			vNormal[0]*=-3.0f; // no idea view.cpp said "see CL_ProcessEntityUpdate()"
		} else {
			gEngfuncs.GetViewAngles((float*)vNormal);
		}
		vNormal.x = 0;
		vNormal.z = 0;	
	}

	if (pSys->iParticleAlign == NO_ALIGN)
	{
	// do nothing ;)	
	}

	if (pSys->iParticleAlign == VELOCITY_VECTOR)
	{
	// todo
	// no idea how to solve that yet
	// will be an intensive night of maths I guess :)
	}
	 
	if (!(pSys->iParticleAlign == NO_ALIGN))
		vNormal.z += sParticle.flCurrentRotation;

	AngleVectors(vNormal, vForward, vRight, vUp);

	float flInverse = 0.0;
	unsigned int iNumFrames = pSys->iFramesPerTexture;
	
	if (iNumFrames)
		flInverse = 1 / sqrt((float)iNumFrames);
	if ((int)sqrt((float)iNumFrames) != (int)abs(sqrt((float)iNumFrames)))
		return;

	if (iNumFrames == 0)
		iNumFrames = 1;

	float co01, co02, co03, co04;
	co01 = flInverse * (int)((iCurrentFrame) % ((int)sqrt((float)iNumFrames)));
	co02 = 1 - flInverse * (int)((iCurrentFrame) / sqrt((float)iNumFrames));
	co03 = 1 - flInverse * (int)((iCurrentFrame) / sqrt((float)iNumFrames) + 1);
	co04 = flInverse * (int)( (iCurrentFrame) % ((int)sqrt((float)iNumFrames)) + 1);

	if (pSys->iDisplayMode == TWENTY_FOUR_BIT_ADDITIVE)
		glColor3ub(iHealth, iHealth, iHealth);
	else
		glColor4ub(sParticle.iRed, sParticle.iGreen, sParticle.iBlue,iHealth); 

	// Finally, we draw the particle
	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);
	//glBindTexture(GL_TEXTURE_2D, (*(sParticle.pTexture)->iID));
	glBegin(GL_QUADS);

	glTexCoord2f(co01, co02);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

   	glTexCoord2f(co04, co02);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(co04, co03); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	glTexCoord2f(co01, co03);
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);    

	glEnd();
}

// Updates the internals of a mapped particle
void CMappedParticle::Update( float flTimeSinceLastDraw )
{
	flParticleTime += flTimeSinceLastDraw;

	// limit to at least 1 sort per second as 0 sorts can cause problems 
	if (g_ParticleSorts->value == 0)
		g_ParticleSorts->value = 1;

	// update distance between particle and player g_ParticleSorts times a second
	float flTimeSinceLastSort = (gEngfuncs.GetClientTime() - sParticle.flLastSort);
	if(flTimeSinceLastSort == 0 || (((int)(1 / flTimeSinceLastSort)) < g_ParticleSorts->value))
	{
		// update lightlevel everytime we sort
		//	if (pSys->iParticleLightCheck == NO_CHECK)

		if (pSys->iParticleLightCheck == CHECK_EVERY_SORT)
		{
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

			sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight );
			sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight );
			sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight );
		}
	}

	// dont draw the particle if smoke is enabled and its close to the player
	if (pSys->bSmoke == true)
	{
		if (sParticle.flSquareDistanceToPlayer < (PARTICLE_THRESHOLD_START*PARTICLE_THRESHOLD_START))
			bIgnoreParticle = true;
		else
			bIgnoreParticle = false;
	}

	if (pSys->iParticleAlign == NO_ALIGN)
	{
		vNormal.x += flTimeSinceLastDraw * (pSys->vRotationVel.x + vRotationVelVar.x);
		vNormal.y += flTimeSinceLastDraw * (pSys->vRotationVel.y + vRotationVelVar.y);
		vNormal.z += flTimeSinceLastDraw * (pSys->vRotationVel.z + vRotationVelVar.z);
		
		while (vNormal.x > 360)
			vNormal.x -= 360;
		while (vNormal.y > 360)
			vNormal.y -= 360;
		while (vNormal.z > 360)
			vNormal.z -= 360;
	}	
	
	// if we're using a custom mode for displaying our texture
	if (pSys->iFPS != 0) {
		if (pSys->iAnimSpeed == CUSTOM) {
			if (pSys->iAnimBehaviour == LOOP) {
				while (flParticleTime > (1.0f / pSys->iFPS)) {
					iCurrentFrame++;
					flParticleTime -= (1.0f / pSys->iFPS);
				}

				if (iCurrentFrame > (pSys->iEndingFrame - 1))
					iCurrentFrame = (pSys->iStartingFrame - 1);

			} else if (pSys->iAnimBehaviour == REVERSE_LOOP) {
				bool bCountUp = true;

				if (iCurrentFrame >= (pSys->iEndingFrame - 1))
					bCountUp = false;
				if (iCurrentFrame <= (pSys->iStartingFrame - 1))
					bCountUp = true;

				while (flParticleTime > (1.0f / pSys->iFPS))
				{
					if (bCountUp == true)
						iCurrentFrame++;
					else
						iCurrentFrame--;

					flParticleTime -= (1.0f / pSys->iFPS);
				}
			} else { // once_through
				while (flParticleTime > (1.0f / pSys->iFPS))
				{
					iCurrentFrame++;
					flParticleTime -= (1.0f / pSys->iFPS);
				}
				if (iCurrentFrame > (pSys->iEndingFrame - 1))
					iCurrentFrame = (pSys->iEndingFrame - 1);
			}
		}
	}


	if (pSys->iAnimSpeed == START_FAST_END_SLOW)
		iCurrentFrame = pSys->iEndingFrame * (1 - exp(-3*sParticle.flAge));

	if (pSys->iAnimSpeed == ANIMATE_OVER_LIFE && sParticle.flMaxAge)
		iCurrentFrame = (pSys->iEndingFrame) * (sParticle.flAge / sParticle.flMaxAge);
	
	if (flTimeSinceLastDraw < 0) // how the hell can the time between updates be less than nothing?
		flTimeSinceLastDraw = -flTimeSinceLastDraw;

	if (pSys->flVelocityDampening > 0.01) {
		sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.x)));
		sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.y)));
		sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(pSys->flVelocityDampening * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	}
	
	sParticle.vVelocity.z -= pSys->flGravity * flTimeSinceLastDraw;

	if (pSys->bWindy) {
		sParticle.vVelocity.x += (sParticle.vWind.x * flTimeSinceLastDraw);
		sParticle.vVelocity.y += (sParticle.vWind.y * flTimeSinceLastDraw);
		sParticle.vVelocity.z += (sParticle.vWind.z * flTimeSinceLastDraw);
	}
	
	VectorMA( sParticle.vPosition, (60.0 * flTimeSinceLastDraw), sParticle.vVelocity, sParticle.vPosition );
	
	// Fograin92: Handle custom scaling (HACKY WAY)
	if( pSys->flScaleIncreasePerSecond < 0.1 )
	{
		// Fograin92: Limit scaling
		if( sParticle.flSize < 0.8 )
			sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	}
	else
	{
		// Fograin92: Default grow code
		sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	}


	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}

// tests whether the particle has collided or whether it needs to die
bool CMappedParticle::Test( void )
{
	if( !pSys )
		return false;

	int i = 0;
	for (i  = 0; i < 3; i++) {
		if (bCollisionChecked[i]) {
			iCollisionTimer[i]--;
			if (iCollisionTimer[i] < 1)
				bCollisionChecked[i] = false;
		}
	}

	if (pSys->iParticleCollision != PARTICLE_PASS_THROUGH) {
		// Now we create four test vectors and copy origin into each one
		vec3_t vTest[6];
		for( i = 0; i < 6; i++ )
			VectorCopy( sParticle.vPosition, vTest[i] );

		// We then set the test vectors
		vTest[0].z -= 5; // down
		vTest[1].x += 5;  // x+
		vTest[2].x -= 5; // x-
		vTest[3].y += 5; // y+
		vTest[4].y -= 5; // y-
		vTest[5].z += 5; // up


		bool bCollX = false;
		bool bCollY = false;
		bool bCollZ = false;

		
		// This long test basically checks whether any of the test vectors are in solid ground 
		// It also checks if the particle is in the sky, in which case we do NOT want to do these solid tests
				
		if(gEngfuncs.PM_PointContents( vTest[0], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[5], NULL ) == CONTENTS_SOLID)
			bCollY = true;
		
		if(gEngfuncs.PM_PointContents( vTest[1], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[2], NULL ) == CONTENTS_SOLID)
			bCollX = true;
		
		if(gEngfuncs.PM_PointContents( vTest[3], NULL ) == CONTENTS_SOLID ||
		gEngfuncs.PM_PointContents( vTest[4], NULL ) == CONTENTS_SOLID)
			bCollZ = true;

		if ((bCollX == true || bCollY == true || bCollZ == true) && (pSys->iParticleCollision == PARTICLE_STUCK)) {
			
			sParticle.vWind.z = 0;
			pSys->flGravity = 0;
			vNormal.x = 0;
			vNormal.y = 0;
			vNormal.z = 0;
			pSys->vRotationVel.x = 0.0f;
			pSys->vRotationVel.y = 0.0f;
			pSys->vRotationVel.z = 0.0f;
			pSys->vRotationVelVarMax.x = 0.0f;
			pSys->vRotationVelVarMax.y = 0.0f;
			pSys->vRotationVelVarMax.z = 0.0f;
			if (pSys->bWindy == true && (bWindChanged == false))
			{
				sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat( (0.01*(cos(10*sParticle.vPosition.y)+1)), (0.3*(sin(10*sParticle.vPosition.x)+1))); 
				sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat( (0.01*(cos(10*sParticle.vPosition.y)+1)), (0.3*(cos(10*sParticle.vPosition.y)+1))); 
				sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat( 0.01f, (0.3*cos(10*sParticle.vPosition.y)+1));
				sParticle.flRotation = gEngfuncs.pfnRandomFloat( -100, 100);
				bWindChanged = true;
			} else if (pSys->bWindy == false) {
				sParticle.vVelocity = Vector(0, 0, 0); 
				sParticle.flRotation = 0.0;			
			}
		}
		
		if ((bCollX == true || bCollY == true || bCollZ == true) && (pSys->iParticleCollision == PARTICLE_DIE))
			return false;

		if(pSys->iParticleCollision == PARTICLE_BOUNCE) {
			if (bCollZ) {
				if (bCollisionChecked[0] != true) { //  bounce up/down particle
					sParticle.vVelocity.z *= -0.3;
					bCollisionChecked[0] = true;
					iCollisionTimer[0] = 5;
				}
			}			

			if (bCollX) {
				if (bCollisionChecked[1] != true) { //  bounce x particle
					sParticle.vVelocity.x *= -0.3;
					bCollisionChecked[1] = true;
					iCollisionTimer[1] = 5;
				}
			}
		
			if (bCollY) {
				if (bCollisionChecked[2] != true) { //  bounce y particle
					sParticle.vVelocity.y *= -0.3;
					bCollisionChecked[2] = true;
					iCollisionTimer[2] = 5;
				}
			}
		}

		// specifically for rain
		if((bCollX == true || bCollY == true || bCollZ == true) && pSys->iParticleCollision == PARTICLE_SPLASH) {
			// big splash for water and a ripple
			if(gEngfuncs.PM_PointContents(sParticle.vPosition, NULL) == CONTENT_WATER) {
				particle_system_management pSystem;
				pSystem.vDirection = -sParticle.vDirection;
				pSystem.vPosition = sParticle.vPosition;
				pSystem.iID = 0;

				int iWater = gEngfuncs.PM_WaterEntity(pSystem.vPosition);
				if ( iWater >= 0 && iWater < MAX_PHYSENTS )
				{
					cl_entity_t* pWater = gEngfuncs.GetEntityByIndex( iWater );
					if(pWater && pWater->model != NULL) {
						pSystem.vPosition.z  = pWater->curstate.origin.z + pWater->model->maxs.z;
					}
				}

				pParticleManager->CreateMappedPS("particles/water_ripple.txt", &pSystem);
				pParticleManager->CreateMappedPS("particles/water_splash.txt", &pSystem);
			}

			// remove the particle that hit
			return false;
		}

		// create a new system bounced off the position this one hit
		if((bCollX == true || bCollY == true || bCollZ == true) && pSys->iParticleCollision == PARTICLE_NEW_SYSTEM) {
			// create the new system with the current position and direction of our old one
			particle_system_management pSystem;
			pSystem.vDirection = sParticle.vDirection;
			pSystem.vPosition = sParticle.vPosition;
			pSystem.iID = 0;

			// bounce the particle. we can't fiddle with the velocity as thats defined in the particle
			// def file, so fiddle which way it is point
			if (bCollZ)
					pSystem.vDirection.z *= -1;

			if (bCollX)
					pSystem.vDirection.x *= -1;

			if (bCollY)
					pSystem.vDirection.y *= -1;

			// create a ripple no matter what falls into the water
			if(gEngfuncs.PM_PointContents(sParticle.vPosition, NULL) == CONTENT_WATER) {
				int iWater = gEngfuncs.PM_WaterEntity(pSystem.vPosition);
				if ( iWater >= 0 && iWater < MAX_PHYSENTS )
				{
					cl_entity_t* pWater = gEngfuncs.GetEntityByIndex( iWater );
					if(pWater && pWater->model != NULL) {
						pSystem.vPosition.z  = pWater->curstate.origin.z + pWater->model->maxs.z;
					}
				}

				pParticleManager->CreateMappedPS("particles/water_ripple.txt", &pSystem);
			}
			// then create the new system
			pParticleManager->CreateMappedPS(pSys->sParticleNewSys, &pSystem);
			
			return false;
		}
	}
	return sParticle.flAge < sParticle.flMaxAge;
}

// prepares the system for drawing
void CMappedParticle::Prepare( void )
{
	glEnable(GL_COLOR_MATERIAL);
	if (pSys->iDisplayMode == TWENTY_FOUR_BIT_ADDITIVE) {
		glBlendFunc(GL_ONE,GL_ONE);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}