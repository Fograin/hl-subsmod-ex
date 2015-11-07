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

// creates a new mapped particle system
CMappedParticleSystem::CMappedParticleSystem( char *sParticleDefinition, particle_system_management *pSysDetails ) : CParticleSystem()
{
	m_sParticleFile = sParticleDefinition;
	m_iID = pSysDetails->iID;

	// loads the defaults so that we can be guarenteed a fully fleshed out ps
	// even if the definition file doesn't give all the values
	CreateDefaultParticleSystem();

	// save these off for the system
	m_pSystem->vPostion = pSysDetails->vPosition;
	m_pSystem->vDirection = pSysDetails->vDirection;

	// only do stuff on succesful
	if(LoadParticleDefinition()) {
		// set these internal variables so that CParticle System can do most of the work
		m_iStartingParticles = m_pSystem->iStartingParticles;
		m_flNewParticles = m_pSystem->flNewParticlesPerSecond;
		m_flSystemMaxAge = m_pSystem->flSystemLifetime;
		m_pSystem->iId = m_iID;

		pParticleTexture = LoadTGA(NULL, m_pSystem->sParticleTexture);

		// add all the starting particles we were asked to
		int iParticles = StartingParticles();
		CParticle *pParticle = NULL;
		for(int i = 0; i <iParticles; i++) {
			pParticle = new CMappedParticle(m_pSystem, pParticleTexture);
			AddParticle(pParticle);
		}
	} else {
		m_pSystem->flSystemLifetime = 0.01;
		m_flSystemMaxAge = 0.01;
	}
}

CMappedParticleSystem::~CMappedParticleSystem() 
{
	delete m_pSystem;
	m_pSystem = NULL;
}

// tests whether we want this system to live some more
bool CMappedParticleSystem::TestSystem( void )
{
	// infinte lifetime
	if(m_pSystem->flSystemLifetime == -1.0)
		return true;

	return CParticleSystem::TestSystem();
}

// updates the particles in the system and the counter
void CMappedParticleSystem::UpdateSystem( void )
{
	CParticle *pParticle = NULL;
	CParticleSystem::UpdateSystem();
	m_flParticleCreationTime += TimeSinceLastDraw();

	// this prevents overly zealous particle creaion
	float flParticleDelay = NewParticlesCreationDelay();

	// calculate the time into this particle system's life that a particle now could last out till
	float flLongestLife = m_flSystemsAge + m_pSystem->flParticleLife + m_pSystem->flParticleLifeVariation;
	pParticleTexture = LoadTGA(NULL, m_pSystem->sParticleTexture);

	// infinite life particle systems, so add at a constant rate
	if(m_pSystem->flSystemLifetime == -1.0) 
	{
		//if alt tabbing out of the game make sure we dont get 100000 particles to add when we tab back in
		if (m_flParticleCreationTime > 3) // 3 seconds
			m_flParticleCreationTime = 3;
		
		while(m_flParticleCreationTime > 0) 
		{
			pParticle = new CMappedParticle(m_pSystem, pParticleTexture);
			AddParticle(pParticle);
			m_flParticleCreationTime -= (1.0f / flParticleDelay);
		}
	}
	// these are finite particles, so create particles at a exponentially falling rate
	else {
		while((m_flParticleCreationTime > 0) && (m_flSystemsAge < (0.9999f * m_flSystemMaxAge))) {
			pParticle = new CMappedParticle(m_pSystem, pParticleTexture);
			AddParticle(pParticle);

			// oh how i love thee bp's equations.  Make functions in future bp
			if((m_pSystem->iGenerationFalloff == LINEAR_HALF_LIFETIME) &&  (flLongestLife > (m_flSystemMaxAge * 0.5))) {
				m_flParticleCreationTime -= 1.0f / ( flParticleDelay * (1 - ((flLongestLife - (0.5 * m_flSystemMaxAge)) / m_flSystemMaxAge)));
			}
			else if((m_pSystem->iGenerationFalloff == LINEAR_QUARTER_LIFETIME) && (flLongestLife > (m_flSystemMaxAge * 0.25))) {
				m_flParticleCreationTime -= 1.0f / ( flParticleDelay * (1 - ((flLongestLife - (0.25 * m_flSystemMaxAge)) / m_flSystemMaxAge)));
			}
			else {
				m_flParticleCreationTime -= (1.0f / flParticleDelay);
			}
		}
	}
}

// sets the defaults for the whole mappers particle system
void CMappedParticleSystem::CreateDefaultParticleSystem( void )
{
	m_pSystem = new mapped_particle_system;

	m_pSystem->iStartingParticles = 5;
	m_pSystem->flNewParticlesPerSecond = 5.0;
	m_pSystem->iGenerationFalloff = NO_FALLOFF;

	_snprintf(m_pSystem->sParticleTexture, MAX_PARTICLE_PATH, "materials/fx/smoke1.tga");

	m_pSystem->flParticleScaleSize = 1.0;
	m_pSystem->flScaleIncreasePerSecond = 1.0;
	m_pSystem->flGravity = -1.0;
	m_pSystem->flParticleRotationSpeed = 0.0;

	m_pSystem->flParticleLife = 5.0;
	m_pSystem->flParticleLifeVariation = 1.0;
	m_pSystem->flSystemLifetime = -1.0;

	m_pSystem->vStartingVel.x = 0.0;
	m_pSystem->vStartingVel.y = 0.0;
	m_pSystem->vStartingVel.z = 5.0;
	m_pSystem->vVelocityVar.x = 2.0;
	m_pSystem->vVelocityVar.y = 2.0;
	m_pSystem->vVelocityVar.z = 2.0;
	m_pSystem->flVelocityDampening = 1.0;

	m_pSystem->iTransparency = 255;
	m_pSystem->iDisplayMode = THIRTY_TWO_BIT;
    
	m_pSystem->iSystemShape = SHAPE_POINT;
	m_pSystem->iPlaneXLength = 5;
	m_pSystem->iPlaneYLength = 5;

	m_pSystem->iAnimBehaviour = ONCE_THROUGH;
	m_pSystem->iAnimSpeed = ANIMATE_OVER_LIFE;
	m_pSystem->iFPS = 30;
	m_pSystem->iStartingFrame = 1;
	m_pSystem->iEndingFrame = 1;
	m_pSystem->iFramesPerTexture = 1;

	m_pSystem->iParticleCollision = PARTICLE_PASS_THROUGH;
	_snprintf(m_pSystem->sParticleNewSys, MAX_PARTICLE_PATH, "");
	m_pSystem->iParticleLightCheck = NO_CHECK;
	
	m_pSystem->iParticleAlign = PLAYER_VIEW;

	m_pSystem->vRotationVel.x = 0.0f;
	m_pSystem->vRotationVel.y = 0.0f;
	m_pSystem->vRotationVel.z = 0.0f;
	m_pSystem->vRotationVelVarMax.x = 0.0f;
	m_pSystem->vRotationVelVarMax.y = 0.0f;
	m_pSystem->vRotationVelVarMax.z = 0.0f;

	m_pSystem->bWindy = true;
	m_pSystem->bFadeIn = true;
	m_pSystem->bFadeOut = true;
	m_pSystem->bSmoke = false;
	m_pSystem->bIgnoreSort = false;
}

// loads and parses the particle definition file
bool CMappedParticleSystem::LoadParticleDefinition( void )
{
	// check we've set a file
	if(!m_sParticleFile) 
	{
		gEngfuncs.Con_Printf("No Mapped Particle definition file specified");
		return false;
	}

	//SysOp: 
	//BUGBUG: You will get
	//the following message error "Bad Mapped Particle definition file specified none". Thats why this func
	//looks for "sParticleDefintionFile". It can't found it because the variable we use to store particle's path
	// is "pev->message", so, finally, it return "none" (I think).

	// load file checking it exists
	char *sFile = (char *)gEngfuncs.COM_LoadFile(m_sParticleFile, 5 , null);
	if(!sFile) 
	{
//		gEngfuncs.Con_Printf("Bad Mapped Particle definition file specified %s\n", m_sParticleFile);
		return false;
	}

	char sSetting[256];
	char sValue[256];

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

		// getting the value for this setting as setting and value's should be paired
		sFile = gEngfuncs.COM_ParseFile(sFile, sValue);

		// hitting eof is unaccpetable so no need to check for it and return true;
		if(!sValue) {
			gEngfuncs.Con_Printf("Unexpected error in file after %s in %s", sSetting, m_sParticleFile);
			gEngfuncs.COM_FreeFile(sFile);
			return false;
		}

		// temp values so we don't have to keep converting when performing calcs on the inputted variables
		int iTemp = 0;
		float flTemp = 0.0;

		// look for an associated setting
		// omfg this is going to be long :)
		// if anyone comes up with a nice way of doing this,
		// i'd love to here it, cause this is fugly

		// number of particles at creation
		if(!stricmp(sSetting, "starting_particles")) {
			m_pSystem->iStartingParticles = atoi(sValue);
		}
		// new particles per second
		else if(!stricmp(sSetting, "new_particles_per_second")) {
			m_pSystem->flNewParticlesPerSecond = atof(sValue);
		}
		// particle texture
		else if(!stricmp(sSetting, "texture")) {
			_snprintf(m_pSystem->sParticleTexture, MAX_PARTICLE_PATH, "%s", sValue);
		}
		// size of texture * this value 
		else if(!stricmp(sSetting, "scale")) {
			m_pSystem->flParticleScaleSize = atof(sValue);
		}
		// how much the particle shrinks/grows per second
		else if(!stricmp(sSetting, "growth")) {
			m_pSystem->flScaleIncreasePerSecond = atof(sValue);
		}
		// how long the average particle lasts for
		else if(!stricmp(sSetting, "lifetime")) {
			m_pSystem->flParticleLife = atof(sValue);
		} 
		// variation of the particles' lifetime per system instance
		else if(!stricmp(sSetting, "lifetime_var")) {
			m_pSystem->flParticleLifeVariation = atof(sValue);
		}
		// how fast the particles in the system rotate
		else if(!stricmp(sSetting, "rotation_speed")) {
			m_pSystem->flParticleRotationSpeed = atof(sValue);
		} 
		// how large is the gravity that effects the particles
		else if(!stricmp(sSetting, "gravity")) {
			m_pSystem->flGravity = atof(sValue);
		}
		// starting velocity of the particles in x, y and z
		else if(!stricmp(sSetting, "starting_velocity_x")) {
			m_pSystem->vStartingVel.x = atof(sValue);
		} else if(!stricmp(sSetting, "starting_velocity_y")) {
			m_pSystem->vStartingVel.y = atof(sValue);
		} else if(!stricmp(sSetting, "starting_velocity_z")) {
			m_pSystem->vStartingVel.z = atof(sValue);
		}
		// variation of the velocity per system instance
		else if(!stricmp(sSetting, "velocity_var_x")) {
			m_pSystem->vVelocityVar.x = atof(sValue);
		} else if(!stricmp(sSetting, "velocity_var_y")) {
			m_pSystem->vVelocityVar.y = atof(sValue);
		} else if(!stricmp(sSetting, "velocity_var_z")) {
			m_pSystem->vVelocityVar.z = atof(sValue);
		}
		// how much velocity is reduced over time
		else if(!stricmp(sSetting, "damping")) {
			m_pSystem->flVelocityDampening = atof(sValue);
		}
		// how transparent the particles are
		else if(!stricmp(sSetting, "transparency")) {
			iTemp = atoi(sValue);
		
			// make sure it isn't out of range
			if(iTemp > 255 || iTemp < 0)
				m_pSystem->iTransparency = 255;
			else
				m_pSystem->iTransparency = iTemp;
		}
		// how long the system lasts for
		else if(!stricmp(sSetting, "system_life")) {
			flTemp = atof(sValue);

			if(flTemp < 0.0)
				flTemp = -1.0;

			m_pSystem->flSystemLifetime = flTemp;
		}
		// what mode we will display these particles in
		else if(!stricmp(sSetting, "display_mode")) {
			if(!stricmp(sValue, "24"))
				m_pSystem->iDisplayMode = TWENTY_FOUR_BIT_ADDITIVE;
			else
				m_pSystem->iDisplayMode = THIRTY_TWO_BIT;
		}
		// emitter shape
		else if(!stricmp(sSetting, "emitter_shape")) {
			if(!stricmp(sValue, "plane"))
				m_pSystem->iSystemShape = SHAPE_PLANE;
			else if(!stricmp(sValue, "around_player"))
				m_pSystem->iSystemShape = SHAPE_AROUND_PLAYER;
			else
				m_pSystem->iSystemShape = SHAPE_POINT;
		}
		// how long the plane is if SHAPE_PLANE or how far around the player if SHAPE_AROUND_PLAYER has been set
		else if(!stricmp(sSetting, "plane_x_length")) {
			m_pSystem->iPlaneXLength = atoi(sValue);
		}
		else if(!stricmp(sSetting, "plane_y_length")) {
			m_pSystem->iPlaneYLength = atoi(sValue);
		}
		// the animation speed mode used
		else if(!stricmp(sSetting, "anim_speed")) {
			if(!stricmp(sValue, "fast_to_slow"))
				m_pSystem->iAnimSpeed = START_FAST_END_SLOW;
			else if(!stricmp(sValue, "custom"))
				m_pSystem->iAnimSpeed = CUSTOM;
			else
				m_pSystem->iAnimSpeed = ANIMATE_OVER_LIFE;
		}
		// fps for the anim
		else if(!stricmp(sSetting, "fps")) {
			m_pSystem->iFPS = abs(atoi(sValue));
		}
		// first frame we will use
		else if(!stricmp(sSetting, "starting_frame")) {
			m_pSystem->iStartingFrame = abs(atoi(sValue));
		}
		// last frame we will use
		else if(!stricmp(sSetting, "ending_frame")) {
			m_pSystem->iEndingFrame = abs(atoi(sValue));
		}
		// how many frames in this image, must be a sqaure number
		else if(!stricmp(sSetting, "frames_in_image")) {
			flTemp = abs(atof(sValue)); // must be position
			if((int)sqrt(flTemp) == (int)abs(sqrt(flTemp))) {
				if(flTemp <= 0.0f)
					flTemp = 1.0f;
				m_pSystem->iFramesPerTexture = (int)flTemp;
			}
			else {
				gEngfuncs.Con_Printf("Frames in Image (%i), not square in %s", (int)flTemp, m_sParticleFile);
			}
		}
		// how looping of the animation is handled
		else if(!stricmp(sSetting, "loop_behaviour")) {
			if(!stricmp(sValue, "loop"))
				m_pSystem->iAnimBehaviour = LOOP;
			else if(!stricmp(sValue, "reverse"))
				m_pSystem->iAnimBehaviour = REVERSE_LOOP;
			else
				m_pSystem->iAnimBehaviour = ONCE_THROUGH;
		}
		// how collisions with solid brushes are handled
		else if(!stricmp(sSetting, "collision")) {
			if(!stricmp(sValue, "stuck"))
				m_pSystem->iParticleCollision = PARTICLE_STUCK;
			else if(!stricmp(sValue, "die"))
				m_pSystem->iParticleCollision = PARTICLE_DIE;
			else if(!stricmp(sValue, "bounce"))
				m_pSystem->iParticleCollision = PARTICLE_BOUNCE;
			else if(!stricmp(sValue, "splash"))
				m_pSystem->iParticleCollision = PARTICLE_SPLASH;
			else if(!stricmp(sValue, "off"))
				m_pSystem->iParticleCollision = PARTICLE_PASS_THROUGH;
			else {
				m_pSystem->iParticleCollision = PARTICLE_NEW_SYSTEM;
				_snprintf(m_pSystem->sParticleNewSys, MAX_PARTICLE_PATH, "%s", sValue);
			}
		}
		// the falloff in the number of particles being created after the system has started
		else if(!stricmp(sSetting, "falloff")) {
			if(!stricmp(sValue, "lifetime"))
				m_pSystem->iGenerationFalloff = LINEAR_LIFETIME;
			else if(!stricmp(sValue, "half"))
				m_pSystem->iGenerationFalloff = LINEAR_HALF_LIFETIME;
			else if(!stricmp(sValue, "quarter"))
				m_pSystem->iGenerationFalloff = LINEAR_QUARTER_LIFETIME;
			else
				m_pSystem->iGenerationFalloff = NO_FALLOFF;
		}
		// light check adapts the particles brightness to the maps light at the particles location
		else if(!stricmp(sSetting, "light_check")) 
		{
			if(!stricmp(sValue, "never"))
				m_pSystem->iParticleLightCheck = NO_CHECK;
			else if(!stricmp(sValue, "once"))
				m_pSystem->iParticleLightCheck = CHECK_ONCE;//CHECK_ONCE;
			else if(!stricmp(sValue, "every_sort"))
				m_pSystem->iParticleLightCheck = CHECK_EVERY_SORT;//CHECK_EVERY_SORT;
			else
				m_pSystem->iParticleLightCheck = NO_CHECK;
		}
		// the falloff in the number of particles being created after the system has started
		else if(!stricmp(sSetting, "particle_align")) {
			if(!stricmp(sValue, "player_view"))
				m_pSystem->iParticleAlign = PLAYER_VIEW;
			else if(!stricmp(sValue, "locked_z"))
				m_pSystem->iParticleAlign = LOCKED_Z;
			else if(!stricmp(sValue, "planar"))
				m_pSystem->iParticleAlign = PLANAR;
			else if(!stricmp(sValue, "none"))
				m_pSystem->iParticleAlign = NO_ALIGN;
			else if(!stricmp(sValue, "velocity_vector"))
				m_pSystem->iParticleAlign = VELOCITY_VECTOR;
			else
				m_pSystem->iParticleAlign = PLAYER_VIEW;
		}
		// rotation velocity
		else if(!stricmp(sSetting, "rotation_velocity_pitch")) {
			m_pSystem->vRotationVel.x = atof(sValue);
		}
		else if(!stricmp(sSetting, "rotation_velocity_yaw")) {
			m_pSystem->vRotationVel.y = atof(sValue);
		}
		else if(!stricmp(sSetting, "rotation_velocity_roll")) {
			m_pSystem->vRotationVel.z = atof(sValue);
		}
		// rotation velocity variation
		else if(!stricmp(sSetting, "rotation_velocity_var_pitch")) {
			m_pSystem->vRotationVelVarMax.x = atof(sValue);
		}
		else if(!stricmp(sSetting, "rotation_velocity_var_yaw")) {
			m_pSystem->vRotationVelVarMax.y = atof(sValue);
		}
		else if(!stricmp(sSetting, "rotation_velocity_var_roll")) {
			m_pSystem->vRotationVelVarMax.z = atof(sValue);
		}
		// the system is effected by wind
		else if(!stricmp(sSetting, "wind")) {
			m_pSystem->bWindy = (!!(atoi(sValue)));
		}
		// the system fades in
		else if(!stricmp(sSetting, "fade_in")) {
			m_pSystem->bFadeIn = (!!(atoi(sValue)));
		}
		// the system fades out
		else if(!stricmp(sSetting, "fade_out")) {
			m_pSystem->bFadeOut = (!!(atoi(sValue)));
		}
		// smoke - fade out particles close to the player
		else if(!stricmp(sSetting, "smoke")) {
			m_pSystem->bSmoke = (!!(atoi(sValue)));
		}
		// ignore_sort - Do not sort these particles with the rest ( saves cpu time )
		else if(!stricmp(sSetting, "ignore_sort")) {
			m_pSystem->bIgnoreSort = (!!(atoi(sValue)));
		}
		// randomize - set different scales based on iRandomVal's value
		else if(!stricmp(sSetting, "randomize")) {
			m_pSystem->bUseRandom = (!!(atoi(sValue)));
		}
		// different scales values
		else if(!stricmp(sSetting, "random_modifier")) {
			m_pSystem->fRandomVal = atof(sValue);
		}
		// catch all to accomodate for my spelling at first and then mapper's spelling later on
		else {
			gEngfuncs.Con_Printf("Unknown setting - %s in %s\n\0", sSetting, m_sParticleFile);
		}
	}

	// release the file
	gEngfuncs.COM_FreeFile(sFile);
	return true;
}