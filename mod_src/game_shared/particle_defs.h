//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	The Battle Grounds Team and Contributors.
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//

#ifndef PARTICLE_DEFS_H
#define PARTICLE_DEFS_H


//========== BASE DEFS ===========//

// Number of particles (adjusted by the cvar value of course) that are created with the particle system
const unsigned int STARTING_FLINKLOCK_SMOKE_PARTICLES	= 3;
const unsigned int STARTING_BARREL_SMOKE_PARTICLES		= 5;
const unsigned int STARTING_SPARK_PARTICLES				= 60;
const unsigned int STARTING_WHITE_SMOKE_PARTICLES		= 5;
const unsigned int STARTING_BROWN_SMOKE_PARTICLES		= 3;

// Number of new particles that can be created after the system has been started
const float NEW_BARREL_SMOKE_PARTICLES_SECOND	= 2.0;
const float NEW_SPARK_PARTICLES_SECOND			= 200.0;

// Amount of time after creation new particles can be made
const float BARREL_SMOKE_PARTICLE_CREATION_TIME		= 0.4f;
const float SPARK_PARTICLE_CREATION_TIME			= 0.2f;

// General helpful constant
const float A_SECOND		= 1.0f;
const unsigned int PERCENT	= 100;

// Define the threshold for the mapped particle
const unsigned int PARTICLE_THRESHOLD_START		= 130;
const unsigned int PARTICLE_THRESHOLD_END		= 280;
const unsigned int GRASS_THRESHOLD_START		= 600;
const unsigned int GRASS_THRESHOLD_END			= 1000;

// Maximum life of particle systems in seconds
const unsigned int FLINTLOCK_SMOKE_SYSTEM_LIFE = 10;
const unsigned int BARREL_SMOKE_SYSTEM_LIFE = 9;
const unsigned int SPARK_SYSTEM_LIFE = 6;
const unsigned int WHITE_SMOKE_SYSTEM_LIFE = 6;
const unsigned int BROWN_SMOKE_SYSTEM_LIFE = 6;

// Particles Paths
const unsigned int MAX_PARTICLE_PATH = 256;
const char FLINTLOCK_SMOKE_PARTICLE[MAX_PARTICLE_PATH] = "particles/smoke3.tga";
const unsigned int NUM_DIFFERENT_BARREL_PARTICLES = 3;
const char BARREL_SMOKE_PARTICLES[NUM_DIFFERENT_BARREL_PARTICLES][MAX_PARTICLE_PATH] = 
{
	"particles/smoke03.tga",
	"particles/smoke04.tga",
	"particles/smoke05.tga"
};

// Defines the shape of where the particles are emitted
typedef enum
{
	SHAPE_POINT = 0,
	SHAPE_PLANE = 1,
	SHAPE_AROUND_PLAYER = 2
}SYSTEM_SHAPE;

// Definces the display mode of the particles
typedef enum
{
	THIRTY_TWO_BIT = 0,
	TWENTY_FOUR_BIT_ADDITIVE = 1
} PARTICLE_DISPLAY_MODES;

// Determines the animation speed over the particle's life
typedef enum
{
	ANIMATE_OVER_LIFE = 0,
	START_FAST_END_SLOW = 1,
	CUSTOM = 2
} SYSTEM_ANIM_SPEED;

// determines what happens when all frames in the texture have been played
typedef enum
{
	LOOP = 0,
	REVERSE_LOOP = 1,
	ONCE_THROUGH = 2
} SYSTEM_ANIM_BEHAVIOUR;

// Determines what happens when the particle touches a solid brush
typedef enum
{
	PARTICLE_PASS_THROUGH = 0,
	PARTICLE_STUCK = 1,
	PARTICLE_DIE = 2,
	PARTICLE_BOUNCE = 3,
	PARTICLE_SPLASH = 4,
	PARTICLE_NEW_SYSTEM = 5
} PARTICLE_COLLISION;

// Determines the fall off for generation of particles through the particle system's life
typedef enum
{
	NO_FALLOFF = 0,
	LINEAR_LIFETIME = 1,
	LINEAR_HALF_LIFETIME = 2,
	LINEAR_QUARTER_LIFETIME = 3
} GENERATION_FALLOFF;

// determines how often the lightlevel of a particle is adapted
typedef enum
{
	NO_CHECK = 0,
	CHECK_ONCE = 1,
	CHECK_EVERY_SORT = 2,
} LIGHT_CHECK;

// determines particle aligning
typedef enum
{
	PLAYER_VIEW = 0,
	LOCKED_Z = 1,
	PLANAR = 2,
	NO_ALIGN = 3,
	VELOCITY_VECTOR = 4,
} PARTICLE_ALIGN;


// Server has problems with some members of these structures, plus it doesn't need to know
#ifdef CLIENT_DLL 

// contains all variables relating to a mapped particle system
struct mapped_particle_system
{
	unsigned int iStartingParticles;
	float flNewParticlesPerSecond;
	GENERATION_FALLOFF iGenerationFalloff;

	char sParticleTexture[MAX_PARTICLE_PATH];

	float flParticleScaleSize;
	float flScaleIncreasePerSecond;
	float flGravity;
	float flParticleRotationSpeed;

	float flParticleLife;
	float flParticleLifeVariation;
	float flSystemLifetime;

	vec3_t vStartingVel;
	vec3_t vVelocityVar;
	float flVelocityDampening;

	unsigned int iTransparency;
	PARTICLE_DISPLAY_MODES iDisplayMode;
    
	SYSTEM_SHAPE iSystemShape;
	unsigned int iPlaneXLength;
	unsigned int iPlaneYLength;

	SYSTEM_ANIM_BEHAVIOUR iAnimBehaviour;
	SYSTEM_ANIM_SPEED iAnimSpeed;
	unsigned int iFPS;
	unsigned int iStartingFrame;
	unsigned int iEndingFrame;
	unsigned int iFramesPerTexture;

	char sParticleNewSys[MAX_PARTICLE_PATH];

//	bool bCreateTraces;
//	char sParticleNewSysForTraces[MAX_PARTICLE_PATH];

	PARTICLE_COLLISION iParticleCollision;
	LIGHT_CHECK iParticleLightCheck;
	PARTICLE_ALIGN iParticleAlign;
	
	vec3_t vRotationVel;
	vec3_t vRotationVelVarMax;

	bool bWindy;
	bool bFadeIn;
	bool bFadeOut;
	bool bSmoke;
	bool bIgnoreSort;

	bool bUseRandom;
	float fRandomVal;

	// and from the server
	vec3_t vPostion;
	vec3_t vDirection;
	unsigned int iId;
};

// holds details about a single texture in the grass particle system
struct grass_particle_system
{
	vec3_t vAbsMin;
	vec3_t vAbsMax;
	float flLeaningMin;
	float flLeaningMax;
	float flWaveSpeed;
	float Yaw;

	float flSize;
	unsigned int iTransparency;
	unsigned int iCount;
	bool bLOD;
	bool bDropOnGround;
	bool bIgnoreSort;

	bool bUseRandom;
	float fRandomVal;

	// and from the server
	vec3_t vPostion;
	vec3_t vDirection;
	unsigned int iId;
	char sParticleTexture[MAX_PARTICLE_PATH];
};

// contains all the basic attributes for a particle
struct base_particle
{
	base_particle() { pTexture = NULL; }
	~base_particle() { pTexture = NULL; }
	vec3_t vPosition;
	vec3_t vVelocity;
	vec3_t vDirection;
	vec3_t vWind;

	unsigned int iSystemId;
	unsigned int iRed;
	unsigned int iBlue;
	unsigned int iGreen;
	unsigned int iTransparency;

	float flSize;
	float flGrowth;
	float flSpread;
	float flScale;
	float flRotation;
	float flCurrentRotation;
	float flAge;
	float flMaxAge;

	particle_texture_s *pTexture;

	bool bUseTrans;
	bool bIgnoreSort;

	float flSquareDistanceToPlayer;
	float flLastSort;
};

// Management features of the ps
struct particle_system_management
{
	~particle_system_management() {}
	vec3_t vPosition;
	vec3_t vDirection;
	vec3_t vAbsMin;
	vec3_t vAbsMax;

	unsigned int iID;
};

// Holds the array of particles and the corresponding system for each set
struct grass_particle_types {
	grass_particle_system *pSystem;
	particle_texture_s *pParticleTexture;
};

// Holds an entry for the particle cache
struct particle_texture_cache
{
	char sTexture[MAX_PARTICLE_PATH];
	particle_texture_s *pTexture;
};

#endif	// CLIENT_DLL

// Fograin92: Particle IDs
enum ParticleIDs
{
	iImpactBloodRed = 1,	// Fograin92: Hit impact / Red blood particles
	iImpactBloodYellow
};



//========== PARTICLE DEFS ===========//

// Fograin92: Pulled from BattleGrounds
const char SPARK_PARTICLE[MAX_PARTICLE_PATH]		= "particles/spark01.tga";
const char WHITE_SMOKE_PARTICLE[MAX_PARTICLE_PATH]	= "particles/smoke_gun.tga";
const char BROWN_SMOKE_PARTICLE[MAX_PARTICLE_PATH]	= "particles/brownsmoke.tga";

// Fograin92: Red blood particles
const char blood_red_animated[MAX_PARTICLE_PATH]	= "materials/fx/blood_red_animated.tga";
const char blood_red_impact[MAX_PARTICLE_PATH]		= "materials/fx/blood_red_impact.tga";
const char blood_red_drips[MAX_PARTICLE_PATH]		= "materials/fx/blood_red_drips.tga";

// Fograin92: Alien blood particles
const char blood_yellow_animated[MAX_PARTICLE_PATH]	= "materials/fx/blood_yellow_animated.tga";
const char blood_yellow_impact[MAX_PARTICLE_PATH]		= "materials/fx/blood_yellow_impact.tga";
const char blood_yellow_drips[MAX_PARTICLE_PATH]		= "materials/fx/blood_yellow_drips.tga";






#endif	// PARTICLE_DEFS_H