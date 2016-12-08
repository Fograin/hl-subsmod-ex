//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	Valve LLC.
//	Id Software, Inc. ("Id Technology")
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "entity_types.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_materials.h"

#include "eventscripts.h"
#include "ev_hldm.h"

#include "r_efx.h"
#include "event_api.h"
#include "event_args.h"
#include "in_defs.h"

#include <string.h>

#include "r_studioint.h"
#include "com_model.h"

#include "soundengine.h" // Fograin92

extern engine_studio_api_t IEngineStudio;

static int tracerCount[ 32 ];

extern "C" char PM_FindTextureType( char *name );

void V_PunchAxis( int axis, float punch );
void VectorAngles( const float *forward, float *angles );

extern cvar_t *cl_lw;

extern "C"
{
	// HLDM
	void EV_FireGlock1( struct event_args_s *args  );
	void EV_FireGlock2( struct event_args_s *args  );
	void EV_FireShotGunSingle( struct event_args_s *args  );
	void EV_FireShotGunDouble( struct event_args_s *args  );
	void EV_FireMP5( struct event_args_s *args  );
	void EV_FireMP52( struct event_args_s *args  );
	void EV_FirePython( struct event_args_s *args  );
	void EV_FireGauss( struct event_args_s *args  );
	void EV_SpinGauss( struct event_args_s *args  );
	//void EV_Crowbar( struct event_args_s *args  );
	void EV_FireCrossbow( struct event_args_s *args  );
	void EV_FireCrossbow2( struct event_args_s *args  );
	void EV_FireRpg( struct event_args_s *args  );
	void EV_EgonFire( struct event_args_s *args  );
	void EV_EgonStop( struct event_args_s *args  );
	void EV_HornetGunFire( struct event_args_s *args  );
	void EV_TripmineFire( struct event_args_s *args  );
	void EV_SnarkFire( struct event_args_s *args  );

	// Fograin92: Update MOD stuff
	void EV_FireEagle( struct event_args_s *args  );
	void EV_Knife( struct event_args_s *args  );
	void EV_FireM249( struct event_args_s *args  );
	void EV_FireM40a1( struct event_args_s *args  );
	void EV_SpinDisplacer( struct event_args_s *args  );
	void EV_FireDisplacer( struct event_args_s *args  );
	void EV_SporeLauncherRocket( struct event_args_s *args  );
	void EV_SporeLauncherGrenade( struct event_args_s *args  );
	void EV_FireShock( struct event_args_s *args  );
	void EV_WrenchNormal( struct event_args_s *args  );
	void EV_WrenchLarge( struct event_args_s *args  );

	void EV_TrainPitchAdjust( struct event_args_s *args );
}

#define VECTOR_CONE_1DEGREES Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES Vector( 0.06105, 0.06105, 0.06105 )	
#define VECTOR_CONE_8DEGREES Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES Vector( 0.17365, 0.17365, 0.17365 )

// play a strike sound based on the texture that was hit by the attack traceline.  VecSrc/VecEnd are the
// original traceline endpoints used by the attacker, iBulletType is the type of bullet that hit the texture.
// returns volume of strike instrument (crowbar) to play
float EV_HLDM_PlayTextureSound( int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType )
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = CHAR_TEX_CONCRETE;
	float fvol;
	float fvolbar;
	char *rgsz[4];
	int cnt;
	float fattn = ATTN_NORM;
	int entity;
	char *pTextureName;
	char texname[ 64 ];
	char szbuffer[ 64 ];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace( ptr );

	// FIXME check if playtexture sounds movevar is set
	//

	chTextureType = 0;

	// Player
	if ( entity >= 1 && entity <= gEngfuncs.GetMaxClients() )
	{
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	}
	else if ( entity == 0 )
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture( ptr->ent, vecSrc, vecEnd );
		
		if ( pTextureName )
		{
			strcpy( texname, pTextureName );
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
			{
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			{
				pTextureName++;
			}
			
			// '}}'
			strcpy( szbuffer, pTextureName );
			szbuffer[ CBTEXTURENAMEMAX - 1 ] = 0;
				
			// get texture type
			chTextureType = PM_FindTextureType( szbuffer );	
		}
	}
	
	switch (chTextureType)
	{
		default:
		case CHAR_TEX_CONCRETE: 
			fvol = 0.9;	
			fvolbar = 0.6;

			// Fograin92: Check if OF
			if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
			{
				rgsz[0] = "player/pl_step1_of.wav";
				rgsz[1] = "player/pl_step2_of.wav";
			}
			else
			{
				rgsz[0] = "player/pl_step1.wav";
				rgsz[1] = "player/pl_step2.wav";
			}
			cnt = 2;
		break;

		case CHAR_TEX_METAL: 
			fvol = 0.9; 
			fvolbar = 0.3;
			rgsz[0] = "player/pl_metal1.wav";
			rgsz[1] = "player/pl_metal2.wav";
			cnt = 2;
		break;

		case CHAR_TEX_DIRT:
			fvol = 0.9;
			fvolbar = 0.1;
			rgsz[0] = "player/pl_dirt1.wav";
			rgsz[1] = "player/pl_dirt2.wav";
			rgsz[2] = "player/pl_dirt3.wav";
			cnt = 3;
		break;

		case CHAR_TEX_VENT:
			fvol = 0.5;
			fvolbar = 0.3;
			rgsz[0] = "player/pl_duct1.wav";
			rgsz[1] = "player/pl_duct1.wav";
			cnt = 2;
		break;

		case CHAR_TEX_GRATE:
			fvol = 0.9;
			fvolbar = 0.5;
			rgsz[0] = "player/pl_grate1.wav";
			rgsz[1] = "player/pl_grate4.wav";
			cnt = 2;
		break;

		case CHAR_TEX_TILE:
			fvol = 0.8;
			fvolbar = 0.2;
			
			// Fograin92: Check if OF
			if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
			{
				rgsz[0] = "player/pl_tile1_of.wav";
				rgsz[1] = "player/pl_tile3_of.wav";
				rgsz[2] = "player/pl_tile2_of.wav";
				rgsz[3] = "player/pl_tile4_of.wav";
			}
			else
			{
				rgsz[0] = "player/pl_tile1.wav";
				rgsz[1] = "player/pl_tile3.wav";
				rgsz[2] = "player/pl_tile2.wav";
				rgsz[3] = "player/pl_tile4.wav";
			}

			cnt = 4;
		break;

		case CHAR_TEX_SLOSH:
			fvol = 0.9;
			fvolbar = 0.0;
			rgsz[0] = "player/pl_slosh1.wav";
			rgsz[1] = "player/pl_slosh3.wav";
			rgsz[2] = "player/pl_slosh2.wav";
			rgsz[3] = "player/pl_slosh4.wav";
			cnt = 4;
		break;

		case CHAR_TEX_WOOD: 
			fvol = 0.9; 
			fvolbar = 0.2;
			rgsz[0] = "debris/wood1.wav";
			rgsz[1] = "debris/wood2.wav";
			rgsz[2] = "debris/wood3.wav";
			cnt = 3;
		break;

		case CHAR_TEX_GLASS:
		case CHAR_TEX_COMPUTER:
			fvol = 0.8; 
			fvolbar = 0.2;
			rgsz[0] = "debris/glass1.wav";
			rgsz[1] = "debris/glass2.wav";
			rgsz[2] = "debris/glass3.wav";
			cnt = 3;
		break;

		case CHAR_TEX_FLESH:
			if (iBulletType == BULLET_PLAYER_CROWBAR)
				return 0.0; // crowbar already makes this sound
			fvol = 1.0;	
			fvolbar = 0.2;
			rgsz[0] = "weapons/bullet_hit1.wav";
			rgsz[1] = "weapons/bullet_hit2.wav";
			fattn = 1.0;
			cnt = 2;
		break;

		// Fograin92: More footsteps
		case CHAR_TEX_SNOW: 
			fvol = 0.9; 
			fvolbar = 0.2;
			rgsz[0] = "player/pl_snow1.wav";
			rgsz[1] = "player/pl_snow3.wav";
			rgsz[2] = "player/pl_snow2.wav";
			rgsz[3] = "player/pl_snow4.wav";
			cnt = 4;
		break;
	}

	// play material hit sound
	// Fograin92: Replaced with new sound engine
	gSoundEngine.PlaySound( rgsz[gEngfuncs.pfnRandomLong(0,cnt-1)], g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL );

	return fvolbar;
}

char *EV_HLDM_DamageDecal( physent_t *pe )
{
	static char decalname[ 32 ];
	int idx;

	if ( pe->classnumber == 1 )
	{
		idx = gEngfuncs.pfnRandomLong( 0, 2 );
		sprintf( decalname, "{break%i", idx + 1 );
	}
	else if ( pe->rendermode != kRenderNormal )
	{
		sprintf( decalname, "{bproof1" );
	}
	else
	{
		idx = gEngfuncs.pfnRandomLong( 0, 4 );
		sprintf( decalname, "{shot%i", idx + 1 );
	}
	return decalname;
}

void EV_HLDM_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName )
{
	int iRand;
	physent_t *pe;

	gEngfuncs.pEfxAPI->R_BulletImpactParticles( pTrace->endpos );

	iRand = gEngfuncs.pfnRandomLong(0,0x7FFF);
	if ( iRand < (0x7fff/2) )// not every bullet makes a sound.
	{
		switch( iRand % 5)
		{
			// Fograin92: Use new audio engine
			case 0:
				gSoundEngine.PlaySound( "weapons/ric1.wav", pTrace->endpos, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL);
			break;

			case 1:
				gSoundEngine.PlaySound( "weapons/ric2.wav", pTrace->endpos, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL);
			break;

			case 2:
				gSoundEngine.PlaySound( "weapons/ric3.wav", pTrace->endpos, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL);
			break;

			case 3:
				gSoundEngine.PlaySound( "weapons/ric4.wav", pTrace->endpos, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL);
			break;

			case 4:
				gSoundEngine.PlaySound( "weapons/ric5.wav", pTrace->endpos, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL);
			break;
		}
	}

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );

	// Only decal brush models such as the world etc.
	if (  decalName && decalName[0] && pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
	{
		if ( CVAR_GET_FLOAT( "r_decals" ) )
		{
			gEngfuncs.pEfxAPI->R_DecalShoot( 
				gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( decalName ) ), 
				gEngfuncs.pEventAPI->EV_IndexFromTrace( pTrace ), 0, pTrace->endpos, 0 );
		}
	}
}

void EV_HLDM_DecalGunshot( pmtrace_t *pTrace, int iBulletType )
{
	physent_t *pe;

	pe = gEngfuncs.pEventAPI->EV_GetPhysent( pTrace->ent );

	if ( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ))
	{
		switch( iBulletType )
		{
			case BULLET_PLAYER_9MM:
			case BULLET_MONSTER_9MM:
			case BULLET_PLAYER_MP5:
			case BULLET_MONSTER_MP5:
			case BULLET_PLAYER_BUCKSHOT:
			case BULLET_PLAYER_357:
			case BULLET_PLAYER_556:		// Fograin92
			case BULLET_PLAYER_762:		// Fograin92
			case BULLET_PLAYER_DEAGLE:	// Fograin92
			case BULLET_MONSTER_556:	// Fograin92
			case BULLET_MONSTER_357:	// Fograin92
			default:
				// smoke and decal
				EV_HLDM_GunshotDecalTrace( pTrace, EV_HLDM_DamageDecal( pe ) );
				break;
		}
	}
}

int EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount )
{
	int tracer = 0;
	int i;
	qboolean player = idx >= 1 && idx <= gEngfuncs.GetMaxClients() ? true : false;

	if ( iTracerFreq != 0 && ( (*tracerCount)++ % iTracerFreq) == 0 )
	{
		vec3_t vecTracerSrc;

		if ( player )
		{
			vec3_t offset( 0, 0, -4 );

			// adjust tracer position for player
			for ( i = 0; i < 3; i++ )
			{
				vecTracerSrc[ i ] = vecSrc[ i ] + offset[ i ] + right[ i ] * 2 + forward[ i ] * 16;
			}
		}
		else
		{
			VectorCopy( vecSrc, vecTracerSrc );
		}
		
		if ( iTracerFreq != 1 )		// guns that always trace also always decal
			tracer = 1;

		switch( iBulletType )
		{
			case BULLET_PLAYER_MP5:
			case BULLET_MONSTER_MP5:
			case BULLET_MONSTER_9MM:
			case BULLET_MONSTER_12MM:
			case BULLET_MONSTER_556:
			case BULLET_MONSTER_357:
			default:
				EV_CreateTracer( vecTracerSrc, end );
				break;
		}
	}

	return tracer;
}


/*
================
FireBullets

Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, float flSpreadX, float flSpreadY )
{
	int i;
	pmtrace_t tr;
	int iShot;
	int tracer;
	
	for ( iShot = 1; iShot <= cShots; iShot++ )	
	{
		vec3_t vecDir, vecEnd;
			
		float x, y, z;
		//We randomize for the Shotgun.
		if ( iBulletType == BULLET_PLAYER_BUCKSHOT )
		{
			do {
				x = gEngfuncs.pfnRandomFloat(-0.5,0.5) + gEngfuncs.pfnRandomFloat(-0.5,0.5);
				y = gEngfuncs.pfnRandomFloat(-0.5,0.5) + gEngfuncs.pfnRandomFloat(-0.5,0.5);
				z = x*x+y*y;
			} while (z > 1);

			for ( i = 0 ; i < 3; i++ )
			{
				vecDir[i] = vecDirShooting[i] + x * flSpreadX * right[ i ] + y * flSpreadY * up [ i ];
				vecEnd[i] = vecSrc[ i ] + flDistance * vecDir[ i ];
			}
		}//But other guns already have their spread randomized in the synched spread.
		else
		{

			for ( i = 0 ; i < 3; i++ )
			{
				vecDir[i] = vecDirShooting[i] + flSpreadX * right[ i ] + flSpreadY * up [ i ];
				vecEnd[i] = vecSrc[ i ] + flDistance * vecDir[ i ];
			}
		}

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
	
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

		tracer = EV_HLDM_CheckTracer( idx, vecSrc, tr.endpos, forward, right, iBulletType, iTracerFreq, tracerCount );

		// do damage, paint decals
		if ( tr.fraction != 1.0 )
		{
			switch(iBulletType)
			{
			default:
			case BULLET_PLAYER_9MM:		
				
				EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
				EV_HLDM_DecalGunshot( &tr, iBulletType );
			
					break;
			case BULLET_PLAYER_MP5:		
				
				if ( !tracer )
				{
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					EV_HLDM_DecalGunshot( &tr, iBulletType );
				}
				break;
			case BULLET_PLAYER_BUCKSHOT:
				
				EV_HLDM_DecalGunshot( &tr, iBulletType );
			
				break;
			case BULLET_PLAYER_357:
				
				EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
				EV_HLDM_DecalGunshot( &tr, iBulletType );
				
				break;

			// Fograin92
			case BULLET_PLAYER_556:		
				
				if ( !tracer )
				{
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					EV_HLDM_DecalGunshot( &tr, iBulletType );
				}
				break;

			// Fograin92
			case BULLET_PLAYER_762:		
				if ( !tracer )
				{
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					EV_HLDM_DecalGunshot( &tr, iBulletType );
				}
				break;

			// Fograin92
			case BULLET_PLAYER_DEAGLE:		
				if ( !tracer )
				{
					EV_HLDM_PlayTextureSound( idx, &tr, vecSrc, vecEnd, iBulletType );
					EV_HLDM_DecalGunshot( &tr, iBulletType );
				}
				break;

			}
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
}

//======================
//	    GLOCK START
//======================
void EV_FireGlock1( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	empty = args->bparam1;
	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");	// brass shell

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( empty ? GLOCK_SHOOT_EMPTY : GLOCK_SHOOT, 2 );

		V_PunchAxis( 0, -2.0 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );

	// Fograin92: This shall be changed in future
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_9MM, 0, 0, args->fparam1, args->fparam2 );
}

void EV_FireGlock2( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t vecSpread;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( GLOCK_SHOOT, 2 );

		V_PunchAxis( 0, -2.0 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_9MM, 0, &tracerCount[idx-1], args->fparam1, args->fparam2 );
	
}
//======================
//	   GLOCK END
//======================

//======================
//	  SHOTGUN START
//======================
void EV_FireShotGunDouble( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	int j;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t vecSpread;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shotgunshell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_FIRE2, 2 );
		V_PunchAxis( 0, -10.0 );
	}

	for ( j = 0; j < 2; j++ )
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32, -12, 6 );

		EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHOTSHELL ); 
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 8, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], 0.17365, 0.04362 );
	}
	else
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 12, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], 0.08716, 0.08716 );
	}
}

void EV_FireShotGunSingle( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t vecSpread;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shotgunshell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOTGUN_FIRE, 2 );

		V_PunchAxis( 0, -5.0 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 32, -12, 6 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHOTSHELL ); 

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 4, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], 0.08716, 0.04362 );
	}
	else
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 6, vecSrc, vecAiming, 2048, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], 0.08716, 0.08716 );
	}
}
//======================
//	   SHOTGUN END
//======================

//======================
//	    MP5 START
//======================
void EV_FireMP5( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");// brass shell
	
	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( MP5_FIRE1 + gEngfuncs.pfnRandomLong(0,2), 2 );

		V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -2, 2 ) );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	if ( gEngfuncs.GetMaxClients() > 1 )
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1], args->fparam1, args->fparam2 );
	}
	else
	{
		EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1], args->fparam1, args->fparam2 );
	}
}

// We only predict the animation and sound
// The grenade is still launched from the server.
void EV_FireMP52( event_args_t *args )
{
	int idx;
	vec3_t origin;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( MP5_LAUNCH, 2 );
		V_PunchAxis( 0, -10 );
	}
}
//======================
//		 MP5 END
//======================



//======================
//	   PHYTON START 
//	     ( .357 )
//======================
void EV_FirePython( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		// Python uses different body in multiplayer versus single player
		int multiplayer = gEngfuncs.GetMaxClients() == 1 ? 0 : 1;

		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( PYTHON_FIRE1, multiplayer ? 1 : 0 );

		V_PunchAxis( 0, -10.0 );
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_357, 0, 0, args->fparam1, args->fparam2 );
}
//======================
//	    PHYTON END 
//	     ( .357 )
//======================

//======================
//	   GAUSS START 
//======================
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

void EV_SpinGauss( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int iSoundState = 0;

	int pitch;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	pitch = args->iparam1;
	iSoundState = args->bparam1 ? SND_CHANGE_PITCH : 0;

	//gSoundEngine.PlaySound( "weapons/gauss_spin.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL, args->entindex );
}

/*
==============================
EV_StopPreviousGauss

==============================
*/
void EV_StopPreviousGauss( int idx )
{
	// Make sure we don't have a gauss spin event in the queue for this guy
	gEngfuncs.pEventAPI->EV_KillEvents( idx, "events/gaussspin.sc" );
	//gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_WEAPON, "ambience/pulsemachine.wav" );
	// Fograin92: Replaced with new audio engine
	gSoundEngine.StopSound(idx, CHAN_WEAPON);
}

extern float g_flApplyVel;

void EV_FireGauss( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float flDamage = args->fparam1;
	int primaryfire = args->bparam1;

	int m_fPrimaryFire = args->bparam1;
	int m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	vec3_t vecSrc;
	vec3_t vecDest;
	edict_t		*pentIgnore;
	pmtrace_t tr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 10;
	physent_t *pEntity;
	int m_iBeam, m_iGlow, m_iBalls;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	if ( args->bparam2 )
	{
		EV_StopPreviousGauss( idx );
		return;
	}

//	Con_Printf( "Firing gauss with %f\n", flDamage );
	EV_GetGunPosition( args, vecSrc, origin );

	m_iBeam = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smoke.spr" );
	m_iBalls = m_iGlow = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/hotglow.spr" );
	
	AngleVectors( angles, forward, right, up );

	VectorMA( vecSrc, 8192, forward, vecDest );

	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, -2.0 );
		gEngfuncs.pEventAPI->EV_WeaponAnimation( GAUSS_FIRE2, 2 );

		if ( m_fPrimaryFire == false )
			 g_flApplyVel = flDamage;	
			 
	}

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
		
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
	
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecDest, PM_STUDIO_BOX, -1, &tr );

		gEngfuncs.pEventAPI->EV_PopPMStates();

		if ( tr.allsolid )
			break;

		if (fFirstBeam)
		{
			if ( EV_IsLocal( idx ) )
			{
				// Add muzzle flash to current weapon model
				EV_MuzzleFlash();
			}
			fFirstBeam = 0;

			gEngfuncs.pEfxAPI->R_BeamEntPoint( 
				idx | 0x1000,
				tr.endpos,
				m_iBeam,
				0.1,
				m_fPrimaryFire ? 1.0 : 2.5,
				0.0,
				m_fPrimaryFire ? 128.0 : flDamage,
				0,
				0,
				0,
				m_fPrimaryFire ? 255 : 255,
				m_fPrimaryFire ? 128 : 255,
				m_fPrimaryFire ? 0 : 255
			);
		}
		else
		{
			gEngfuncs.pEfxAPI->R_BeamPoints( vecSrc,
				tr.endpos,
				m_iBeam,
				0.1,
				m_fPrimaryFire ? 1.0 : 2.5,
				0.0,
				m_fPrimaryFire ? 128.0 : flDamage,
				0,
				0,
				0,
				m_fPrimaryFire ? 255 : 255,
				m_fPrimaryFire ? 128 : 255,
				m_fPrimaryFire ? 0 : 255
			);
		}

		pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		if ( pEntity == NULL )
			break;

		if ( pEntity->solid == SOLID_BSP )
		{
			float n;

			pentIgnore = NULL;

			n = -DotProduct( tr.plane.normal, forward );

			if (n < 0.5) // 60 degrees	
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				vec3_t r;
			
				VectorMA( forward, 2.0 * n, tr.plane.normal, r );

				flMaxFrac = flMaxFrac - tr.fraction;
				
				VectorCopy( r, forward );

				VectorMA( tr.endpos, 8.0, forward, vecSrc );
				VectorMA( vecSrc, 8192.0, forward, vecDest );

				gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage * n / 255.0, flDamage * n * 0.5 * 0.1, FTENT_FADEOUT );

				vec3_t fwd;
				VectorAdd( tr.endpos, tr.plane.normal, fwd );

				gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100,
									255, 100 );

				// lose energy
				if ( n == 0 )
				{
					n = 0.1;
				}
				
				flDamage = flDamage * (1 - n);

			}
			else
			{
				// tunnel
				EV_HLDM_DecalGunshot( &tr, BULLET_MONSTER_12MM );

				gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 1.0, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );

				// limit it to one hole punch
				if (fHasPunched)
				{
					break;
				}
				fHasPunched = 1;
				
				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( !m_fPrimaryFire )
				{
					vec3_t start;

					VectorMA( tr.endpos, 8.0, forward, start );

					// Store off the old count
					gEngfuncs.pEventAPI->EV_PushPMStates();
						
					// Now add in all of the players.
					gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );

					gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
					gEngfuncs.pEventAPI->EV_PlayerTrace( start, vecDest, PM_STUDIO_BOX, -1, &beam_tr );

					if ( !beam_tr.allsolid )
					{
						vec3_t delta;
						float n;

						// trace backwards to find exit point

						gEngfuncs.pEventAPI->EV_PlayerTrace( beam_tr.endpos, tr.endpos, PM_STUDIO_BOX, -1, &beam_tr );

						VectorSubtract( beam_tr.endpos, tr.endpos, delta );
						
						n = Length( delta );

						if (n < flDamage)
						{
							if (n == 0)
								n = 1;
							flDamage -= n;

							// absorption balls
							{
								vec3_t fwd;
								VectorSubtract( tr.endpos, forward, fwd );
								gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100,
									255, 100 );
							}

	//////////////////////////////////// WHAT TO DO HERE
							// CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

							EV_HLDM_DecalGunshot( &beam_tr, BULLET_MONSTER_12MM );
							
							gEngfuncs.pEfxAPI->R_TempSprite( beam_tr.endpos, vec3_origin, 0.1, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT );
			
							// balls
							{
								vec3_t fwd;
								VectorSubtract( beam_tr.endpos, forward, fwd );
								gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, beam_tr.endpos, fwd, m_iBalls, (int)(flDamage * 0.3), 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 200,
									255, 40 );
							}
							
							VectorAdd( beam_tr.endpos, forward, vecSrc );
						}
					}
					else
					{
						flDamage = 0;
					}

					gEngfuncs.pEventAPI->EV_PopPMStates();
				}
				else
				{
					if ( m_fPrimaryFire )
					{
						// slug doesn't punch through ever with primary 
						// fire, so leave a little glowy bit and make some balls
						gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, vec3_origin, 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, 200.0 / 255.0, 0.3, FTENT_FADEOUT );
			
						{
							vec3_t fwd;
							VectorAdd( tr.endpos, tr.plane.normal, fwd );
							gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 8, 0.6, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100,
								255, 200 );
						}
					}

					flDamage = 0;
				}
			}
		}
		else
		{
			VectorAdd( tr.endpos, forward, vecSrc );
		}
	}
}
//======================
//	   GAUSS END 
//======================

//======================
//	   CROWBAR START
//======================
/*
enum crowbar_e {
	CROWBAR_IDLE = 0,
	CROWBAR_DRAW,
	CROWBAR_HOLSTER,
	CROWBAR_ATTACK1HIT,
	CROWBAR_ATTACK1MISS,
	CROWBAR_ATTACK2MISS,
	CROWBAR_ATTACK2HIT,
	CROWBAR_ATTACK3MISS,
	CROWBAR_ATTACK3HIT
};

int g_iSwing;

//Only predict the miss sounds, hit sounds are still played 
//server side, so players don't get the wrong idea.
void EV_Crowbar( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	
	//Play Swing sound
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, PITCH_NORM); 

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( CROWBAR_ATTACK1MISS, 1 );

		switch( (g_iSwing++) % 3 )
		{
			case 0:
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( CROWBAR_ATTACK1MISS, 1 ); break;
			case 1:
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( CROWBAR_ATTACK2MISS, 1 ); break;
			case 2:
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( CROWBAR_ATTACK3MISS, 1 ); break;
		}
	}
}
*/
//======================
//	   CROWBAR END 
//======================

//======================
//	  CROSSBOW START
//======================
enum crossbow_e 
{
	CROSSBOW_IDLE1 = 0,	// full
	CROSSBOW_IDLE2,		// empty
	CROSSBOW_FIDGET1,	// full
	CROSSBOW_FIDGET2,	// empty
	CROSSBOW_FIRE1,		// full
	CROSSBOW_FIRE2,		// reload
	CROSSBOW_FIRE3,		// empty
	CROSSBOW_RELOAD,	// from empty
	CROSSBOW_DRAW1,		// full
	CROSSBOW_DRAW2,		// empty
	CROSSBOW_HOLSTER1,	// full
	CROSSBOW_HOLSTER2,	// empty
};

//=====================
// EV_BoltCallback
// This function is used to correct the origin and angles 
// of the bolt, so it looks like it's stuck on the wall.
//=====================
void EV_BoltCallback ( struct tempent_s *ent, float frametime, float currenttime )
{
	ent->entity.origin = ent->entity.baseline.vuser1;
	ent->entity.angles = ent->entity.baseline.vuser2;
}

void EV_FireCrossbow2( event_args_t *args )
{
	vec3_t vecSrc, vecEnd;
	vec3_t up, right, forward;
	pmtrace_t tr;

	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );

	VectorCopy( args->velocity, velocity );
	
	AngleVectors( angles, forward, right, up );

	EV_GetGunPosition( args, vecSrc, origin );

	VectorMA( vecSrc, 8192, forward, vecEnd );

	if ( EV_IsLocal( idx ) )
	{
		if ( args->iparam1 )
			gEngfuncs.pEventAPI->EV_WeaponAnimation( CROSSBOW_FIRE1, 1 );
		else if ( args->iparam2 )
			gEngfuncs.pEventAPI->EV_WeaponAnimation( CROSSBOW_FIRE3, 1 );
	}

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );
	
	//We hit something
	if ( tr.fraction < 1.0 )
	{
		physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent ); 

		//Not the world, let's assume we hit something organic ( dog, cat, uncle joe, etc ).
		if ( pe->solid != SOLID_BSP )
		{
			switch( gEngfuncs.pfnRandomLong(0,1) )
			{
				case 0:
					// Fograin92:
					//gEngfuncs.pEventAPI->EV_PlaySound( idx, tr.endpos, CHAN_BODY, "weapons/xbow_hitbod1.wav", 1, ATTN_NORM, 0, PITCH_NORM );
					gSoundEngine.PlaySound( "weapons/xbow_hitbod1.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL, args->entindex );

				break;

				case 1:
					// Fograin92:
					//gEngfuncs.pEventAPI->EV_PlaySound( idx, tr.endpos, CHAN_BODY, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM, 0, PITCH_NORM );
					gSoundEngine.PlaySound( "weapons/xbow_hitbod2.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL, args->entindex );
				break;
			}
		}
		//Stick to world but don't stick to glass, it might break and leave the bolt floating. It can still stick to other non-transparent breakables though.
		else if ( pe->rendermode == kRenderNormal ) 
		{
			// Fograin92
			//gEngfuncs.pEventAPI->EV_PlaySound( 0, tr.endpos, CHAN_BODY, "weapons/xbow_hit1.wav", gEngfuncs.pfnRandomFloat(0.95, 1.0), ATTN_NORM, 0, PITCH_NORM );
			gSoundEngine.PlaySound( "weapons/xbow_hit1.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 98 + gEngfuncs.pfnRandomLong( 0, 3 ), ATTN_NORM, NULL, args->entindex );

			//Not underwater, do some sparks...
			if ( gEngfuncs.PM_PointContents( tr.endpos, NULL ) != CONTENTS_WATER)
				 gEngfuncs.pEfxAPI->R_SparkShower( tr.endpos );

			vec3_t vBoltAngles;
			int iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex( "models/crossbow_bolt.mdl" );

			VectorAngles( forward, vBoltAngles );

			TEMPENTITY *bolt = gEngfuncs.pEfxAPI->R_TempModel( tr.endpos - forward * 10, Vector( 0, 0, 0), vBoltAngles , 5, iModelIndex, TE_BOUNCE_NULL );
			
			if ( bolt )
			{
				bolt->flags |= ( FTENT_CLIENTCUSTOM ); //So it calls the callback function.
				bolt->entity.baseline.vuser1 = tr.endpos - forward * 10; // Pull out a little bit
				bolt->entity.baseline.vuser2 = vBoltAngles; //Look forward!
				bolt->callback = EV_BoltCallback; //So we can set the angles and origin back. (Stick the bolt to the wall)
			}
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

//TODO: Fully predict the fliying bolt.
void EV_FireCrossbow( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	//Only play the weapon anims if I shot it. 
	if ( EV_IsLocal( idx ) )
	{
		if ( args->iparam1 )
			gEngfuncs.pEventAPI->EV_WeaponAnimation( CROSSBOW_FIRE1, 1 );
		else if ( args->iparam2 )
			gEngfuncs.pEventAPI->EV_WeaponAnimation( CROSSBOW_FIRE3, 1 );

		V_PunchAxis( 0, -2.0 );
	}
}
//======================
//	   CROSSBOW END 
//======================

//======================
//	    RPG START 
//======================
enum rpg_e 
{
	RPG_IDLE = 0,
	RPG_FIDGET,
	RPG_RELOAD,		// to reload
	RPG_FIRE2,		// to empty
	RPG_HOLSTER1,	// loaded
	RPG_DRAW1,		// loaded
	RPG_HOLSTER2,	// unloaded
	RPG_DRAW_UL,	// unloaded
	RPG_IDLE_UL,	// unloaded idle
	RPG_FIDGET_UL,	// unloaded fidget
};

void EV_FireRpg( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	
	//Only play the weapon anims if I shot it. 
	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( RPG_FIRE2, 1 );
	
		V_PunchAxis( 0, -5.0 );
	}
}
//======================
//	     RPG END 
//======================

//======================
//	    EGON START 
//======================
enum egon_e
{
	EGON_IDLE1 = 0,
	EGON_FIDGET1,
	EGON_ALTFIREON,
	EGON_ALTFIRECYCLE,
	EGON_ALTFIREOFF,
	EGON_FIRE1,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_FIRE4,
	EGON_DRAW,
	EGON_HOLSTER
};

int g_fireAnims1[] = { EGON_FIRE1, EGON_FIRE2, EGON_FIRE3, EGON_FIRE4 };
int g_fireAnims2[] = { EGON_ALTFIRECYCLE };

enum EGON_FIRESTATE { FIRE_OFF, FIRE_CHARGE };
enum EGON_FIREMODE { FIRE_NARROW, FIRE_WIDE};

#define	EGON_PRIMARY_VOLUME		450
#define EGON_BEAM_SPRITE		"sprites/xbeam1.spr"
#define EGON_FLARE_SPRITE		"sprites/XSpark1.spr"
#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"

#define ARRAYSIZE(p)		(sizeof(p)/sizeof(p[0]))

BEAM *pBeam;
BEAM *pBeam2;
//TEMPENTITY *pFlare;	// Vit_amiN: egon's beam flare
/*
void EV_EgonFlareCallback( struct tempent_s *ent, float frametime, float currenttime )
{
	float delta = currenttime - ent->tentOffset.z;	// time past since the last scale
	if ( delta >= ent->tentOffset.y )
	{
		ent->entity.curstate.scale += ent->tentOffset.x * delta;
		ent->tentOffset.z = currenttime;
	}
}
*/

void EV_EgonFire( event_args_t *args )
{
	int idx, iFireState, iFireMode;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	iFireState = args->iparam1;
	iFireMode = args->iparam2;
	int iStartup = args->bparam1;


	if ( iStartup )
	{
		gSoundEngine.PlaySound( "weapons/egon_windup2.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 93 + gEngfuncs.pfnRandomLong( 0, 0x1f ), ATTN_NORM, NULL, idx );
	}
	else
	{
		gSoundEngine.PlaySound( "weapons/egon_run3.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 93 + gEngfuncs.pfnRandomLong( 0, 0x1f ), ATTN_NORM, NULL, idx );
	}

	//Only play the weapon anims if I shot it.
	if ( EV_IsLocal( idx ) )
		gEngfuncs.pEventAPI->EV_WeaponAnimation ( g_fireAnims1[ gEngfuncs.pfnRandomLong( 0, 3 ) ], 1 );

	if ( iStartup == 1 && EV_IsLocal( idx ) && !pBeam && !pBeam2 && cl_lw->value ) //Adrian: Added the cl_lw check for those lital people that hate weapon prediction.
	{
		vec3_t vecSrc, vecEnd, origin, angles, forward, right, up;
		pmtrace_t tr;

		cl_entity_t *pl = gEngfuncs.GetEntityByIndex( idx );

		if ( pl )
		{
			VectorCopy( gHUD.m_vecAngles, angles );
			
			AngleVectors( angles, forward, right, up );

			EV_GetGunPosition( args, vecSrc, pl->origin );

			VectorMA( vecSrc, 2048, forward, vecEnd );

			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
				
			// Store off the old count
			gEngfuncs.pEventAPI->EV_PushPMStates();
			
			// Now add in all of the players.
			gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

			gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
			gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

			gEngfuncs.pEventAPI->EV_PopPMStates();

			int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex( EGON_BEAM_SPRITE );

			float r = 50.0f;
			float g = 50.0f;
			float b = 125.0f;

			if ( IEngineStudio.IsHardware() )
			{
				r /= 100.0f;
				g /= 100.0f;
			}
				
		
			pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 3.5, 0.2, 0.7, 55, 0, 0, r, g, b );

			if ( pBeam )
				 pBeam->flags |= ( FBEAM_SINENOISE );
 
			pBeam2 = gEngfuncs.pEfxAPI->R_BeamEntPoint ( idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 5.0, 0.08, 0.7, 25, 0, 0, r, g, b );

			// Vit_amiN: egon beam flare
			//pFlare = gEngfuncs.pEfxAPI->R_TempSprite ( tr.endpos, vec3_origin, 1.0, gEngfuncs.pEventAPI->EV_FindModelIndex( EGON_FLARE_SPRITE ), kRenderGlow, kRenderFxNoDissipation, 1.0, 99999, FTENT_SPRCYCLE | FTENT_PERSIST );
		}
	}
	// Vit_amiN: store the last mode for EV_EgonStop()
	/*
	if ( pFlare )
	{
		pFlare->tentOffset.x = (iFireMode == FIRE_WIDE) ? 1.0f : 0.0f;
	}
	*/
}

void EV_EgonStop( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy ( args->origin, origin );

	// Fograin92
	gSoundEngine.StopSound(idx, CHAN_WEAPON);
	
	if ( args->iparam1 )
		gSoundEngine.PlaySound( "weapons/egon_off1.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 93 + gEngfuncs.pfnRandomLong( 0, 0x1f ), ATTN_NORM, NULL, idx );

	if ( EV_IsLocal( idx ) ) 
	{
		if ( pBeam )
		{
			pBeam->die = 0.0;
			pBeam = NULL;
		}
			
		
		if ( pBeam2 )
		{
			pBeam2->die = 0.0;
			pBeam2 = NULL;
		}

		// Vit_amiN: egon beam flare
		/*
		if ( pFlare )	
		{
			pFlare->die = gEngfuncs.GetClientTime();

			if ( gEngfuncs.GetMaxClients() == 1 || !(pFlare->flags & FTENT_NOMODEL) )
			{
				if ( pFlare->tentOffset.x != 0.0f )	// true for iFireMode == FIRE_WIDE
				{
					pFlare->callback = &EV_EgonFlareCallback;
					pFlare->fadeSpeed = 2.0;			// fade out will take 0.5 sec
					pFlare->tentOffset.x = 10.0;		// scaling speed per second
					pFlare->tentOffset.y = 0.1;			// min time between two scales
					pFlare->tentOffset.z = pFlare->die;	// the last callback run time
					pFlare->flags = FTENT_FADEOUT | FTENT_CLIENTCUSTOM;
				}
			}

			pFlare = NULL;
		}
		*/
	}
}
//======================
//	    EGON END 
//======================

//======================
//	   HORNET START
//======================
enum hgun_e
{
	HGUN_IDLE1 = 0,
	HGUN_FIDGETSWAY,
	HGUN_FIDGETSHAKE,
	HGUN_DOWN,
	HGUN_UP,
	HGUN_SHOOT
};

void EV_HornetGunFire( event_args_t *args )
{
	int idx, iFireMode;
	vec3_t origin, angles, vecSrc, forward, right, up;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	iFireMode = args->iparam1;

	//Only play the weapon anims if I shot it.
	if ( EV_IsLocal( idx ) )
	{
		V_PunchAxis( 0, gEngfuncs.pfnRandomLong ( 0, 2 ) );
		gEngfuncs.pEventAPI->EV_WeaponAnimation ( HGUN_SHOOT, 1 );
	}
}
//======================
//	   HORNET END
//======================

//======================
//	   TRIPMINE START
//======================
enum tripmine_e
{
	TRIPMINE_IDLE1 = 0,
	TRIPMINE_IDLE2,
	TRIPMINE_ARM1,
	TRIPMINE_ARM2,
	TRIPMINE_FIDGET,
	TRIPMINE_HOLSTER,
	TRIPMINE_DRAW,
	TRIPMINE_WORLD,
	TRIPMINE_GROUND,
};

//We only check if it's possible to put a trip mine
//and if it is, then we play the animation. Server still places it.
void EV_TripmineFire( event_args_t *args )
{
	int idx;
	vec3_t vecSrc, angles, view_ofs, forward;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy( args->origin, vecSrc );
	VectorCopy( args->angles, angles );

	AngleVectors ( angles, forward, NULL, NULL );
		
	if ( !EV_IsLocal ( idx ) )
		return;

	// Grab predicted result for local player
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );

	vecSrc = vecSrc + view_ofs;

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecSrc + forward * 128, PM_NORMAL, -1, &tr );

	//Hit something solid
	if ( tr.fraction < 1.0 )
		 gEngfuncs.pEventAPI->EV_WeaponAnimation ( TRIPMINE_DRAW, 0 );
	
	gEngfuncs.pEventAPI->EV_PopPMStates();
}
//======================
//	   TRIPMINE END
//======================

//======================
//	   SQUEAK START
//======================
enum squeak_e {
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};

#define VEC_HULL_MIN		Vector(-16, -16, -36)
#define VEC_DUCK_HULL_MIN	Vector(-16, -16, -18 )

void EV_SnarkFire( event_args_t *args )
{
	int idx;
	vec3_t vecSrc, angles, view_ofs, forward;
	pmtrace_t tr;

	idx = args->entindex;
	VectorCopy( args->origin, vecSrc );
	VectorCopy( args->angles, angles );

	AngleVectors ( angles, forward, NULL, NULL );
		
	if ( !EV_IsLocal ( idx ) )
		return;
	
	if ( args->ducking )
		vecSrc = vecSrc - ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
	
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	
	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc + forward * 20, vecSrc + forward * 64, PM_NORMAL, -1, &tr );

	//Find space to drop the thing.
	if ( tr.allsolid == 0 && tr.startsolid == 0 && tr.fraction > 0.25 )
		 gEngfuncs.pEventAPI->EV_WeaponAnimation ( SQUEAK_THROW, 0 );
	
	gEngfuncs.pEventAPI->EV_PopPMStates();
}
//======================
//	   SQUEAK END
//======================




//======================
//	    DEAGLE START
//======================
void EV_FireEagle( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	int laseractive;

	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	empty = args->bparam1;
	AngleVectors( angles, forward, right, up );
	laseractive = args->bparam2;

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/shell.mdl");// brass shell

	if ( EV_IsLocal( idx ) )
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( empty ? DEAGLE_SHOOT_EMPTY : DEAGLE_SHOOT, 2 );

		V_PunchAxis( 0, -5.0 );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 


	//gSoundEngine.PlaySound( "weapons/desert_eagle_fire.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 94 + gEngfuncs.pfnRandomLong( 0, 0xf ), ATTN_NORM, NULL, args->entindex );
		
	EV_GetGunPosition( args, vecSrc, origin );
	
	VectorCopy( forward, vecAiming );


//EV_Dynamic_MuzzleFlash ( vecSrc );
	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_DEAGLE, 0, 0, args->fparam1, args->fparam2 );
}
//======================
//	   DEAGLE END
//======================
//======================
//	   KNIFE START
//======================

enum knife_e {
	KNIFE_IDLE1 = 0,
	KNIFE_DRAW,
	KNIFE_HOLSTER,
	KNIFE_SLASHHIT,
	KNIFE_SLASHMISS,
	KNIFE_CHARGE,
	KNIFE_STAB,
};

int g_iSwing_knife;

//Only predict the miss sounds, hit sounds are still played 
//server side, so players don't get the wrong idea.
void EV_Knife( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( KNIFE_SLASHMISS, 1 );

		switch ( gEngfuncs.pfnRandomLong ( 0 , 1 ) )
		{
			case 0:	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/knife_swipe1.wav", 1, ATTN_NORM, 0, 100 );	break;
			case 1:	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/knife_swipe2.wav", 1, ATTN_NORM, 0, 100 );	break;
		}
	}
}
//======================
//	   KNIFE END 
//======================
//======================
//	    M249 START
//======================
int g_iShellType; // Ported to client side, no need to send through messages.
void EV_FireM249( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float flDamage = args->fparam1;
	pmtrace_t tr;

	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;

	int	nMaxHits = 5;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	float flSpread = 0.01;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );
	
	g_iShellType = !g_iShellType;

	if ( g_iShellType )
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/saw_link.mdl");// saw shell
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex ("models/saw_shell.mdl");// saw shell

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation( M249_FIRE1 + gEngfuncs.pfnRandomLong(0,1), args->iparam1 );


		V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -2, 2 + gEngfuncs.pfnRandomLong(0,0.3) ) );
		V_PunchAxis( 1, gEngfuncs.pfnRandomFloat( 2 + gEngfuncs.pfnRandomLong(0,0.1) , -2 ) );
	}

	EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 10, -12, 8 );
	EV_EjectBrass ( ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL ); 
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
//EV_Dynamic_MuzzleFlash ( vecSrc );

	EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_556, 2, &tracerCount[idx-1], args->fparam1, args->fparam2 );
}
//======================
//		 M249 END
//======================
//======================
//	    SNIPER START
//======================
void EV_FireM40a1( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;

	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	float flSpread = 0.001;

	empty = args->bparam1;
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( empty ? SNIPER_FIRELASTROUND : SNIPER_FIRE, 2 );

		V_PunchAxis( 0, -2.0 );
	}

	//gSoundEngine.PlaySound( "weapons/sniper_fire.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 94 + gEngfuncs.pfnRandomLong( 0, 0xf ), ATTN_NORM, NULL, args->entindex );

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
//EV_Dynamic_MuzzleFlash ( vecSrc );

EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, 8192, BULLET_PLAYER_762, 0, 0, flSpread, flSpread );
}
//======================
//	   SNIPER END
//======================
//======================
//	   DISPLACER START
//======================
void EV_SpinDisplacer(event_args_t *args)//Original code idea by Xwider
{
	if (EV_IsLocal(args->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(DISPLACER_SPINUP, 0);
		cl_entity_t *view = gEngfuncs.GetViewModel();
		if (view != NULL)
		{
			float life = 1.05;

			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x2000, view->index | 0x4000, args->iparam2, life, 0.6, 0.5, 0.005, 1.2, 0, 10, 96, 128, 16);	
			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x3000, view->index | 0x2000, args->iparam2, life, 0.6, 0.5, 0.005, 1.2, 0, 10, 96, 128, 16);
			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x3000, view->index | 0x4000, args->iparam2, life, 0.6, 0.5, 0.005, 1.2, 0, 10, 96, 128, 16);
		}
	}
	if (args->bparam2 == 1)// sound mode
		gSoundEngine.PlaySound( "weapons/displacer_spin.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 94 + gEngfuncs.pfnRandomLong( 0, 0xf ), ATTN_NORM, NULL, args->entindex );
	else
		gSoundEngine.PlaySound( "weapons/displacer_spin2.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 94 + gEngfuncs.pfnRandomLong( 0, 0xf ), ATTN_NORM, NULL, args->entindex );
}
void EV_FireDisplacer(event_args_t *args)
{
	if (EV_IsLocal(args->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( DISPLACER_FIRE, 0 );

		V_PunchAxis( 0, -2.0 );
	}
	gSoundEngine.PlaySound( "weapons/displacer_fire.wav", g_vecZero, SND_RELATIVE, CHAN_WEAPON, CVAR_GET_FLOAT("sm_snd_sfx"), 94 + gEngfuncs.pfnRandomLong( 0, 0xf ), ATTN_NORM, NULL, args->entindex );
}
//======================
//	   DISPLACER END
//======================
//======================
//	    SPORE START 
//======================
enum sporelauncher_e {
	SPORELAUNCHER_IDLE = 0,
	SPORELAUNCHER_FIDGET,
	SPORELAUNCHER_RELOAD_REACH,		// reload prepare
	SPORELAUNCHER_RELOAD_LOAD,		// reload in progress
	SPORELAUNCHER_RELOAD_AIM,		// reload end
	SPORELAUNCHER_FIRE,				// fire rocket
	SPORELAUNCHER_HOLSTER,			// holster
	SPORELAUNCHER_DRAW,				// draw
	SPORELAUNCHER_FIRE2,			// grenadefire
};

void EV_SporeLauncherRocket( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/splauncher_fire.wav", 0.9, ATTN_NORM, 0, PITCH_NORM );

	//Only play the weapon anims if I shot it. 
	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SPORELAUNCHER_FIRE, 1 );
	
		V_PunchAxis( 0, -5.0 );
	}
}

void EV_SporeLauncherGrenade( event_args_t *args )
{
	int idx;
	vec3_t origin;

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/splauncher_altfire.wav", 0.9, ATTN_NORM, 0, PITCH_NORM );

	//Only play the weapon anims if I shot it. 
	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( SPORELAUNCHER_FIRE, 1 );
	
		V_PunchAxis( 0, -5.0 );
	}
}
//======================
//	     SPORE END 
//======================
//======================
//	   SHOCK START
//======================
enum shockrifle_e {
	SHOCK_IDLE1 = 0,
	SHOCK_FIRE,
	SHOCK_DRAW,
	SHOCK_HOLSTER,
	SHOCK_IDLE3
};

void EV_FireShock( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );

	AngleVectors( angles, forward, right, up );

		gEngfuncs.pEventAPI->EV_WeaponAnimation(SHOCK_FIRE, 2);
		cl_entity_t *view = gEngfuncs.GetViewModel();
		if (view != NULL)
		{
			float life = 0.1;

			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x2000, view->index | 0x1000, args->iparam2, life, 0.8, 0.5, 0.5, 0.6, 0, 10, 0, 4, 10);
			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x3000, view->index | 0x1000, args->iparam2, life, 0.8, 0.5, 0.5, 0.6, 0, 10, 0, 4, 10);
			gEngfuncs.pEfxAPI->R_BeamEnts(view->index | 0x4000, view->index | 0x1000, args->iparam2, life, 0.8, 0.5, 0.5, 0.6, 0, 10, 0, 4, 10);
		}

	idx = args->entindex;
	VectorCopy( args->origin, origin );
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/shock_fire.wav", 1, ATTN_NORM, 0, 93 + gEngfuncs.pfnRandomLong(0,0xF) );

	//Only play the weapon anims if I shot it. 
	if ( EV_IsLocal( idx ) )
	{
		if ( args->iparam1 )
			gEngfuncs.pEventAPI->EV_WeaponAnimation( SHOCK_FIRE, 1 );
	}

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight( 999 );
	dl->die = gEngfuncs.GetClientTime();
	dl->origin[0] = vecSrc.x;
	dl->origin[1] = vecSrc.y;
	dl->origin[2] = vecSrc.z;
	dl->color.r = 0;
	dl->color.g = 255;
	dl->color.b = 255;
	dl->radius = 260;
}
//======================
//	   SHOCK END
//======================
//======================
//	   WRENCH START
//======================
enum wrench_e {
	WRENCH_IDLE1 = 0,
	WRENCH_IDLE2,
	WRENCH_IDLE3,
	WRENCH_DRAW,
	WRENCH_HOLSTER,
	WRENCH_ATTACK1HIT,
	WRENCH_ATTACK1MISS,
	WRENCH_ATTACK2HIT,
	WRENCH_ATTACK2MISS,
	WRENCH_ATTACK3HIT,
	WRENCH_ATTACK3MISS,
	WRENCH_BIGWIND,
	WRENCH_BIGHIT,
	WRENCH_BIGMISS,
	WRENCH_BIGLOOP,
};

//int g_iSwing_knife;
int g_iSwing;

//Only predict the miss sounds, hit sounds are still played 
//server side, so players don't get the wrong idea.
void EV_WrenchNormal( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;

	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation( WRENCH_ATTACK1MISS, 1 );
		switch( (g_iSwing++) % 3 )
		{
			case 0:
				gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/pwrench_miss2.wav", 1, ATTN_NORM, 0, PITCH_NORM); 
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( WRENCH_ATTACK1MISS, 1 ); break;
			case 1:
				gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/pwrench_miss1.wav", 1, ATTN_NORM, 0, PITCH_NORM); 
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( WRENCH_ATTACK2MISS, 1 ); break;
			case 2:
				gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/pwrench_miss2.wav", 1, ATTN_NORM, 0, PITCH_NORM); 
				gEngfuncs.pEventAPI->EV_WeaponAnimation ( WRENCH_ATTACK3MISS, 1 ); break;
		}
	}
}
void EV_WrenchLarge( event_args_t *args )
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	idx = args->entindex;
	VectorCopy( args->origin, origin );

	if ( EV_IsLocal( idx ) )
	{

		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, "weapons/pwrench_big_miss.wav", 1, ATTN_NORM, 0, PITCH_NORM);
		gEngfuncs.pEventAPI->EV_WeaponAnimation( WRENCH_BIGMISS, 1 );
		V_PunchAxis( 0, 5.0 );
	}
}
//======================
//	   WRENCH END 
//======================



void EV_TrainPitchAdjust( event_args_t *args )
{
	int idx;
	vec3_t origin;

	unsigned short us_params;
	int noise;
	float m_flVolume;
	int pitch;
	int stop;
	
	char sz[ 256 ];

	idx = args->entindex;
	
	VectorCopy( args->origin, origin );

	us_params = (unsigned short)args->iparam1;
	stop	  = args->bparam1;

	m_flVolume	= (float)(us_params & 0x003f)/40.0;
	noise		= (int)(((us_params) >> 12 ) & 0x0007);
	pitch		= (int)( 10.0 * (float)( ( us_params >> 6 ) & 0x003f ) );

	switch ( noise )
	{
	case 1: strcpy( sz, "plats/ttrain1.wav"); break;
	case 2: strcpy( sz, "plats/ttrain2.wav"); break;
	case 3: strcpy( sz, "plats/ttrain3.wav"); break; 
	case 4: strcpy( sz, "plats/ttrain4.wav"); break;
	case 5: strcpy( sz, "plats/ttrain6.wav"); break;
	case 6: strcpy( sz, "plats/ttrain7.wav"); break;
	default:
		// no sound
		strcpy( sz, "" );
		return;
	}

	if ( stop )
	{
		//gEngfuncs.pEventAPI->EV_StopSound( idx, CHAN_STATIC, sz );
		// Fograin92: Replaced with new audio engine
		gSoundEngine.StopSound(idx, CHAN_STATIC);
	}
	else
	{
		//gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_STATIC, sz, m_flVolume, ATTN_NORM, SND_CHANGE_PITCH, pitch );
		// Fograin92: Replaced with new audio engine
		gSoundEngine.PlaySound(sz, origin, SND_CHANGE_PITCH, CHAN_STATIC, VOL_NORM, pitch, ATTN_NORM, NULL, idx);
	}
}

int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 )
{
	return 0;
}