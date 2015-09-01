//=============================================================//
//	Half-Life Subtitles MOD
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
#include <gl/gl.h>
#include <gl/glaux.h>
#include "com_model.h"	// Fograin92: alight_t
#include "pm_shared.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "particle_header.h"

// sets up opengl for all particles
void Particle_InitOpenGL( void ){
	glEnable(GL_BLEND);
	glDepthMask(false);
	glDisable(GL_CULL_FACE);
}

void Particle_FinishOpenGL( void )
{
	glEnable(GL_CULL_FACE);
}

// creates a particle
CParticle::CParticle() {
	sParticle.flLastSort = 0.0;
	sParticle.flSquareDistanceToPlayer = 0.0;

	sParticle.iRed = 255;
	sParticle.iBlue = sParticle.iGreen = sParticle.iRed;
}

// deletes a particle
CParticle::~CParticle(){}

// tests whether we have any life left
bool CParticle::Test( void )
{
	if(sParticle.flAge < sParticle.flMaxAge)
		return true;
	else
		return false;
}

// are we further away from the player of not?
// we're reversing this so that nearer particles are drawn last in a reversed draw order
bool less_than(CParticle* pPart1, CParticle* pPart2)
{
	if(!pPart1 || !pPart2)
		return false;

	float flOurLength = pPart1->DistanceToThisPlayer(true);
	float flCompLength = pPart2->DistanceToThisPlayer(true);

	if(flOurLength > flCompLength)
		return true;
	else
		return false;
}

// prepares this particle for drawing
void CParticle::Prepare( void )
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);
}

// returns the square of the distance to the player if wished
float CParticle::DistanceToThisPlayer( bool bSquare )
{
	// we haven't updated this sort so do so
	if( sParticle.flLastSort != gEngfuncs.GetClientTime())
	{
		vec3_t vOrigin = sParticle.vPosition;
		float deltaX = flPlayerOrigin.x - vOrigin.x;
		float deltaY = flPlayerOrigin.y - vOrigin.y;
		float deltaZ = flPlayerOrigin.z - vOrigin.z;

		sParticle.flSquareDistanceToPlayer = ((deltaX*deltaX) + (deltaY*deltaY) + (deltaZ*deltaZ)); 
		sParticle.flLastSort = gEngfuncs.GetClientTime();
	}

	if (bSquare)
		return sParticle.flSquareDistanceToPlayer;
	else 
		return sqrt(sParticle.flSquareDistanceToPlayer);
}

// do the basic draw function
//extern void RenderFog ( void );	// Fograin92: Disabled
void CParticle::Draw( void )
{
	if(sParticle.pTexture == NULL || !sParticle.pTexture->imageData)
		return;

	int iHealth = 0;
	
	// flint and barrel calc using trans,  white smoke uses 255
	if(sParticle.bUseTrans == true)
		iHealth = (sParticle.iTransparency -( sParticle.flAge * sParticle.iTransparency / sParticle.flMaxAge));
	else
		iHealth = (255 -( sParticle.flAge * 255 / sParticle.flMaxAge));

	if (iHealth < 0)
		return;

	vec3_t vNormal,vForward, vRight, vUp, vPoint, vPosition;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	// We then get the view angles for the player so that we can "billboard" the sprites
	if(g_iUser1 == OBS_IN_EYE || g_iUser1 == OBS_CHASE_LOCKED) {
		cl_entity_t* pEnt = gEngfuncs.GetEntityByIndex( g_iUser2 );
		VectorCopy(pEnt->angles, vNormal);
		vNormal[0]*=-3.0f; // no idea view.cpp said "see CL_ProcessEntityUpdate()"
	} else {
		gEngfuncs.GetViewAngles((float*)vNormal);
	}

	vNormal.z += sParticle.flCurrentRotation;
	AngleVectors(vNormal, vForward, vRight, vUp);

	//RenderFog();	// Fograin92: Disabled
	glColor4ub(sParticle.iRed, sParticle.iGreen, sParticle.iBlue, iHealth); 

	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);

	// Finally, we draw the particle
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

   	glTexCoord2f(0, 1);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(1, 1); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	glTexCoord2f(1, 0);
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);    

	glEnd();
}

// Flint Particles
CFlintParticle::CFlintParticle(vec3_t vPosition, particle_texture_s *pParticleTexture) : CParticle()
{
	InitValues(); 
	sParticle.vPosition = vPosition;
	sParticle.pTexture = pParticleTexture;
}

// give the flint particle its default values
void CFlintParticle::InitValues( void )
{
	sParticle.iTransparency = 200;
	sParticle.bUseTrans = true;
	sParticle.bIgnoreSort = false;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = 10.0f;

	sParticle.flSize = 0.6f;
	sParticle.flGrowth = 0.3f;
	sParticle.flScale = 30.0f;

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -18, 18 );
	sParticle.flCurrentRotation = gEngfuncs.pfnRandomFloat( -40, 40 );
	
	sParticle.vWind.x = gEngfuncs.pfnRandomFloat( 0.01f, 0.14f );
	sParticle.vWind.y = gEngfuncs.pfnRandomFloat( 0.01f, 0.11f );
	sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.01f, 0.04f );

	sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat( -2, 2 );
	sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat( -2, 2 );
	sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat( 0.01f, 9 );
}

// updates the particle's speed and position
void CFlintParticle::Update( float flTimeSinceLastDraw )
{
	//damping
	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(flTimeSinceLastDraw * sParticle.vVelocity.x)));
	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(flTimeSinceLastDraw * sParticle.vVelocity.y)));
	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(flTimeSinceLastDraw * sParticle.vVelocity.z)));
	
	//wind
	sParticle.vVelocity.x += sParticle.vWind.x * flTimeSinceLastDraw;
	sParticle.vVelocity.y += sParticle.vWind.y * flTimeSinceLastDraw;
	sParticle.vVelocity.z += sParticle.vWind.z * flTimeSinceLastDraw;

	VectorMA( sParticle.vPosition, 60.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );
	
	//rotation
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth * 20;
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}

// Barrel Particles
CBarrelParticle::CBarrelParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture) : CParticle()
{
	sParticle.vDirection = vDirection;
	InitValues(); 
	sParticle.vPosition = vPosition;
	sParticle.pTexture = pParticleTexture;

	// check lightlevel once when the particle is spawned
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
	
	// make it a little brighter because we want bright smoke
	sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight * 1.5f);
	sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight * 1.5f);
	sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight * 1.5f);

	// but make sure we dont go over the limits
	if (sParticle.iRed > 255)
		sParticle.iRed = 255;
	if (sParticle.iGreen > 255)
		sParticle.iGreen = 255;
	if (sParticle.iBlue > 255)
		sParticle.iBlue = 255;
}

// give the flint particle its default values
void CBarrelParticle::InitValues( void )
{
	sParticle.iTransparency = 200;
	sParticle.bUseTrans = true;
	sParticle.bIgnoreSort = false;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = 6.0f;

	sParticle.flSize = 0.2f;
	sParticle.flGrowth = 0.5f;
	sParticle.flScale = 30.0f;

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -40, 40 );
	sParticle.flCurrentRotation = 0;
	
	sParticle.vWind.x = gEngfuncs.pfnRandomFloat( 0.01f, 0.3f );
	sParticle.vWind.y = gEngfuncs.pfnRandomFloat( 0.01f, 0.35f );
	sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.01f, 0.25f );

	sParticle.flSpread = gEngfuncs.pfnRandomFloat( 0.2f, 7.0f );

	sParticle.vVelocity.x = (sParticle.flSpread * sParticle.vDirection.x );
	sParticle.vVelocity.y = (sParticle.flSpread * sParticle.vDirection.y );
	sParticle.vVelocity.z = (sParticle.flSpread * sParticle.vDirection.z );
}

// updates the particle's details
void CBarrelParticle::Update( float flTimeSinceLastDraw )
{
	//damping
	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(1.2 * flTimeSinceLastDraw * sParticle.vVelocity.x)));
	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(1.2 * flTimeSinceLastDraw * sParticle.vVelocity.y)));
	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(1.2 * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	
	VectorMA( sParticle.vPosition, 60.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );

	//wind
	sParticle.vVelocity.x += sParticle.vWind.x * flTimeSinceLastDraw;
	sParticle.vVelocity.y += sParticle.vWind.y * flTimeSinceLastDraw;
	sParticle.vVelocity.z += sParticle.vWind.z * flTimeSinceLastDraw;
	
	//growth, we let the slow moving particles also grow slower to get a nice funnel shape
	float flGrowthBalance = (((sParticle.vVelocity.x*sParticle.vVelocity.x) + (sParticle.vVelocity.y*sParticle.vVelocity.y) + (sParticle.vVelocity.z*sParticle.vVelocity.z)))/1.5f;
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth * flGrowthBalance;

	//rotation
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}

// Spark Particles
CSparkParticle::CSparkParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture) : CParticle()
{
	InitValues(); 
	sParticle.vPosition = vPosition;
	sParticle.vDirection = vDirection;
	sParticle.pTexture = pParticleTexture;
}

// give the flint particle its default values
void CSparkParticle::InitValues( void )
{
	sParticle.iTransparency = 200;
	sParticle.bIgnoreSort = false;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = 3.0f;

	sParticle.flSize = 0.15f;
	sParticle.flGrowth = 0.0f;
	sParticle.flScale = 30.0f;

	sParticle.vVelocity.x = gEngfuncs.pfnRandomFloat( -1, 1 );
	sParticle.vVelocity.y = gEngfuncs.pfnRandomFloat( -1, 1 );
	sParticle.vVelocity.z = gEngfuncs.pfnRandomFloat( -1, 1 );
}

// having to have this function is a minor annoyance, but its needed
void CSparkParticle::Draw( void )
{
	int iHealth = (255 -( sParticle.flAge * 255 / sParticle.flMaxAge));
	if (iHealth < 0)
		return;

	vec3_t vNormal,vForward, vRight, vUp, vPoint, vPosition;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	// We then get the view angles for the player so that we can "billboard" the sprites 
	gEngfuncs.GetViewAngles((float*)vNormal);
	AngleVectors(vNormal, vForward, vRight, vUp);

	glEnable(GL_COLOR_MATERIAL);
	glColor3ub(iHealth, iHealth, iHealth);

	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);

	// Finally, we draw the particle
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

   	glTexCoord2f(0, 1);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(1, 1); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	glTexCoord2f(1, 0);
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);    

	glEnd();
}

// prepares this particle's drawing
void CSparkParticle::Prepare( void )
{
	glBlendFunc(GL_ONE,GL_ONE);
}

// updates the particle's details
void CSparkParticle::Update( float flTimeSinceLastDraw )
{
	//damping
	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(0.5 * flTimeSinceLastDraw * sParticle.vVelocity.x)));
	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(0.5 * flTimeSinceLastDraw * sParticle.vVelocity.y)));
	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(0.5 * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	
	sParticle.vVelocity.z -= 0.065f * 15 * flTimeSinceLastDraw;
	VectorMA( sParticle.vPosition, 60.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );

	//rotation
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}

// Barrel Particles
CWhiteSmokeParticle::CWhiteSmokeParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture) : CParticle()
{
	sParticle.vDirection = vDirection;
	InitValues(); 
	sParticle.vPosition = vPosition;
	sParticle.pTexture = pParticleTexture;

	// check lightlevel once when the particle is spawned
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
	
	sParticle.iRed = (int)(lighting.color[0] * lighting.shadelight * 1.5f);
	sParticle.iGreen = (int)(lighting.color[1] * lighting.shadelight * 1.5f);
	sParticle.iBlue = (int)(lighting.color[2] * lighting.shadelight * 1.5f);

	if (sParticle.iRed > 255)
		sParticle.iRed = 255;
	if (sParticle.iGreen > 255)
		sParticle.iGreen = 255;
	if (sParticle.iBlue > 255)
		sParticle.iBlue = 255;
}

// give the flint particle its default values
void CWhiteSmokeParticle::InitValues( void )
{
	sParticle.iTransparency = gEngfuncs.pfnRandomFloat( 99, 180 );//180
//	sParticle.bUseTrans = false;
	sParticle.bUseTrans = true;
	sParticle.bIgnoreSort = true;
/*
	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = 6.0f;

	sParticle.flSize = 0.4f;
	sParticle.flGrowth = 0.5f;
	sParticle.flScale = 30.0f;
*/
	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = gEngfuncs.pfnRandomFloat( 1.0f, 0.3f );//6-3

	sParticle.flSize = 0.2f;//0.2
	sParticle.flGrowth = gEngfuncs.pfnRandomFloat( 1.0f, 2.0f );
	sParticle.flScale = 5.0f;

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -18, 18 );
	sParticle.flCurrentRotation = gEngfuncs.pfnRandomFloat( -180, 180 );
	
	sParticle.vWind.x = gEngfuncs.pfnRandomFloat( -0.3f, 0.3f );
	sParticle.vWind.y = gEngfuncs.pfnRandomFloat( -0.3f, 0.3f );
	sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.01f, 0.3f );

	sParticle.flSpread = gEngfuncs.pfnRandomFloat( 0.0f, 0.0f );

	sParticle.vVelocity.x = (sParticle.flSpread * (sParticle.vDirection.x + gEngfuncs.pfnRandomFloat( -0.01f, 0.01f )));
	sParticle.vVelocity.y = (sParticle.flSpread * (sParticle.vDirection.y + gEngfuncs.pfnRandomFloat( -0.01f, 0.01f )));
	sParticle.vVelocity.z = (sParticle.flSpread * (sParticle.vDirection.z + gEngfuncs.pfnRandomFloat( -0.01f, 0.01f )));
}

// updates the particle's details
void CWhiteSmokeParticle::Update( float flTimeSinceLastDraw )
{
	//damping
	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.x)));
	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.y)));
	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	
	//wind
	sParticle.vVelocity.x += sParticle.vWind.x * flTimeSinceLastDraw;
	sParticle.vVelocity.y += sParticle.vWind.y * flTimeSinceLastDraw;
	sParticle.vVelocity.z += sParticle.vWind.z * flTimeSinceLastDraw;

	VectorMA( sParticle.vPosition, 60.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );

	//rotation
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) {
		sParticle.flCurrentRotation -= 360;
	}
 	
	sParticle.flAge += flTimeSinceLastDraw;
}


CMuzzleFlashParticle::CMuzzleFlashParticle(vec3_t vPosition, vec3_t vDirection, particle_texture_s *pParticleTexture) : CParticle()
{
	sParticle.vDirection = vDirection;
	InitValues(); 
	sParticle.vPosition = vPosition;
	sParticle.pTexture = pParticleTexture;
}
// prepares this particle's drawing
void CMuzzleFlashParticle::Prepare( void )
{
	glBlendFunc(GL_ONE,GL_ONE);
}

// having to have this function is a minor annoyance, but its needed
void CMuzzleFlashParticle::Draw( void )
{
	int iHealth = (255 -( sParticle.flAge * 255 / sParticle.flMaxAge));
	if (iHealth < 0)
		return;

	vec3_t vNormal,vForward, vRight, vUp, vPoint, vPosition;

	// We again copy part->origin into another vector to prevent us accidentally messing with it
	VectorCopy( sParticle.vPosition, vPosition );

	// We then get the view angles for the player so that we can "billboard" the sprites 
	gEngfuncs.GetViewAngles((float*)vNormal);
	AngleVectors(vNormal, vForward, vRight, vUp);

	glEnable(GL_COLOR_MATERIAL);
	glColor3ub(iHealth, iHealth, iHealth);

	glBindTexture(GL_TEXTURE_2D, sParticle.pTexture->iID);

	// Finally, we draw the particle
	glBegin(GL_TRIANGLES);

	glTexCoord2f(0, 0);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

   	glTexCoord2f(0, 1);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(1, 1); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	// Repeats for triangles.

	glTexCoord2f(0, 0);
	VectorMA ( vPosition, (sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);

	glTexCoord2f(1, 0);
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (-sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint);    

	glTexCoord2f(1, 1); 
	VectorMA ( vPosition, (-sParticle.flSize * sParticle.flScale), vUp, vPoint);
	VectorMA ( vPoint, (sParticle.flSize * sParticle.flScale), vRight, vPoint);
	glVertex3fv(vPoint); 

	glEnd();
}
void CMuzzleFlashParticle::InitValues( void )
{
	sParticle.iTransparency = 255;
//	sParticle.bUseTrans = false;
	sParticle.bIgnoreSort = false;

	sParticle.flAge = 0.0f;
	sParticle.flMaxAge = 0.2f;//1

	sParticle.flSize = 0.2f;
	sParticle.flGrowth = 0.5f;
	
	sParticle.flScale = 15.0f;//30

	sParticle.flRotation = gEngfuncs.pfnRandomFloat( -180, 180 );//18
	sParticle.flCurrentRotation = gEngfuncs.pfnRandomFloat( -180, 180 );

	sParticle.vWind.x = gEngfuncs.pfnRandomFloat( 0.01f, 0.3f );
	sParticle.vWind.y = gEngfuncs.pfnRandomFloat( 0.01f, 0.34f );
	sParticle.vWind.z = gEngfuncs.pfnRandomFloat( 0.1f, 0.51f );

	sParticle.flSpread = gEngfuncs.pfnRandomFloat( 0.4f, 2.0f );

	sParticle.vVelocity.x = (sParticle.flSpread * 
			(sParticle.vDirection.x + gEngfuncs.pfnRandomFloat( -0.3f, 0.3f )));
	sParticle.vVelocity.y = (sParticle.flSpread * 
			(sParticle.vDirection.y + gEngfuncs.pfnRandomFloat( -0.3f, 0.3f )));
	sParticle.vVelocity.z = (sParticle.flSpread * 
			(sParticle.vDirection.z + gEngfuncs.pfnRandomFloat( -0.3f, 0.3f )));
}

// updates the particle's details
void CMuzzleFlashParticle::Update( float flTimeSinceLastDraw )
{
	//damping
//	sParticle.vVelocity.x *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.x)));
//	sParticle.vVelocity.y *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.y)));
//	sParticle.vVelocity.z *= ( 1 / ( 1 + fabsf(2 * flTimeSinceLastDraw * sParticle.vVelocity.z)));
	
	//wind
//	sParticle.vVelocity.x += sParticle.vWind.x * flTimeSinceLastDraw;
//	sParticle.vVelocity.y += sParticle.vWind.y * flTimeSinceLastDraw;
//	sParticle.vVelocity.z += sParticle.vWind.z * flTimeSinceLastDraw;

	VectorMA( sParticle.vPosition, 0.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );
//	VectorMA( sParticle.vPosition, 60.0 * flTimeSinceLastDraw, sParticle.vVelocity, sParticle.vPosition );

	//rotation
	sParticle.flSize += flTimeSinceLastDraw * sParticle.flGrowth;
	sParticle.flCurrentRotation += flTimeSinceLastDraw * sParticle.flRotation;
	while (sParticle.flCurrentRotation > 360) 
	{
		sParticle.flCurrentRotation -= 360;
	}

	sParticle.flAge += flTimeSinceLastDraw;
}