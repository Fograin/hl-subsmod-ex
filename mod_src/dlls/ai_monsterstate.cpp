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
//=========================================================
// monsterstate.cpp - base class monster functions for 
// controlling core AI.
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "nodes.h"
#include "monsters.h"
#include "animation.h"
#include "saverestore.h"
#include "soundent.h"
#include "particle_defs.h"	// Fograin92: BG Particle system
extern int gmsgParticles;	// Fograin92: BG Particle system


//=========================================================
// SetState
//=========================================================
void CBaseMonster :: SetState ( MONSTERSTATE State )
{
/*
	if ( State != m_MonsterState )
	{
		ALERT ( at_aiconsole, "State Changed to %d\n", State );
	}
*/
	
	switch( State )
	{
	
	// Drop enemy pointers when going to idle
	case MONSTERSTATE_IDLE:

		if ( m_hEnemy != NULL )
		{
			m_hEnemy = NULL;// not allowed to have an enemy anymore.
			ALERT ( at_aiconsole, "Stripped\n" );
		}
		break;
	}

	m_MonsterState = State;
	m_IdealMonsterState = State;
}

//=========================================================
// RunAI
//=========================================================
void CBaseMonster :: RunAI ( void )
{
	// Fograin92: Cast water waves when NPC is in the water (Inspired by code from: SysOP)
	if( CVAR_GET_FLOAT("sm_particles") > 0 )
	{
		int iRandomness = 0;

		// Fograin92: If monster is moving in the water
		if( IsMoving() )
			iRandomness = 1; // Fograin92: Set one to cast wave every AI tick
		else
			iRandomness = RANDOM_FLOAT( 1, 100 ); // Fograin92: Random waves

		// Fograin92: Cast waves ONLY if iRandomness is set below 25. This means everytime monster is moving OR 25% chance to cast when IDLE
		if( iRandomness < 26 )
		{
			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				
				// Fograin92: Feet in water
				if (pev->waterlevel == 1)
				{
					// Fograin92: Cast waves at NPC origin
					WRITE_COORD( pev->origin.x );
					WRITE_COORD( pev->origin.y );
					WRITE_COORD( pev->origin.z );
				}

				// Fograin92: Waist in water
				else if (pev->waterlevel == 2)
				{
					// Fograin92: Cast waves at NPC center XYZ
					Vector		vecBlastSrc;
					vecBlastSrc = Center();

					WRITE_COORD( vecBlastSrc.x );
					WRITE_COORD( vecBlastSrc.y );
					WRITE_COORD( vecBlastSrc.z );
				}

				// Fograin92: Head in water
				else if (pev->waterlevel == 3)
				{
					// Fograin92: Cast waves at head XYZ
					Vector vecHeadPos;
					vecHeadPos = pev->origin + pev->view_ofs;

					WRITE_COORD( vecHeadPos.x );
					WRITE_COORD( vecHeadPos.y );
					WRITE_COORD( vecHeadPos.z );
				}
				
				WRITE_COORD( 0 );
				WRITE_COORD( 0 );
				WRITE_COORD( 0 );

				// Fograin92: Handle specific NPCs here (e.g. Icky)
				if( FClassnameIs(pev, "monster_ichthyosaur") )
					WRITE_SHORT(iWaterIcky);
				else
					WRITE_SHORT(iDefaultWaves);

			MESSAGE_END();
		}
	}


	// to test model's eye height
	//UTIL_ParticleEffect ( pev->origin + pev->view_ofs, g_vecZero, 255, 10 );

	// IDLE sound permitted in ALERT state is because monsters were silent in ALERT state. Only play IDLE sound in IDLE state
	// once we have sounds for that state.
	if ( ( m_MonsterState == MONSTERSTATE_IDLE || m_MonsterState == MONSTERSTATE_ALERT ) && RANDOM_LONG(0,99) == 0 && !(pev->flags & SF_MONSTER_GAG) )
	{
		IdleSound();
	}

	if ( m_MonsterState != MONSTERSTATE_NONE	&& 
		 m_MonsterState != MONSTERSTATE_PRONE   && 
		 m_MonsterState != MONSTERSTATE_DEAD )// don't bother with this crap if monster is prone. 
	{
		// collect some sensory Condition information.
		// don't let monsters outside of the player's PVS act up, or most of the interesting
		// things will happen before the player gets there!
		// UPDATE: We now let COMBAT state monsters think and act fully outside of player PVS. This allows the player to leave 
		// an area where monsters are fighting, and the fight will continue.
		if ( !FNullEnt( FIND_CLIENT_IN_PVS( edict() ) ) || ( m_MonsterState == MONSTERSTATE_COMBAT ) )
		{
			Look( m_flDistLook );
			Listen();// check for audible sounds. 

			// now filter conditions.
			ClearConditions( IgnoreConditions() );

			GetEnemy();
		}

		// do these calculations if monster has an enemy.
		if ( m_hEnemy != NULL )
		{
			CheckEnemy( m_hEnemy );
		}

		CheckAmmo();
	}

	FCheckAITrigger();

	PrescheduleThink();

	MaintainSchedule();

	// if the monster didn't use these conditions during the above call to MaintainSchedule() or CheckAITrigger()
	// we throw them out cause we don't want them sitting around through the lifespan of a schedule
	// that doesn't use them. 
	m_afConditions &= ~( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE );
}

//=========================================================
// GetIdealState - surveys the Conditions information available
// and finds the best new state for a monster.
//=========================================================
MONSTERSTATE CBaseMonster :: GetIdealState ( void )
{
	int	iConditions;

	iConditions = IScheduleFlags();
	
	// If no schedule conditions, the new ideal state is probably the reason we're in here.
	switch ( m_MonsterState )
	{
	case MONSTERSTATE_IDLE:
		
		/*
		IDLE goes to ALERT upon hearing a sound
		-IDLE goes to ALERT upon being injured
		IDLE goes to ALERT upon seeing food
		-IDLE goes to COMBAT upon sighting an enemy
		IDLE goes to HUNT upon smelling food
		*/
		{
			if ( iConditions & bits_COND_NEW_ENEMY )			
			{
				// new enemy! This means an idle monster has seen someone it dislikes, or 
				// that a monster in combat has found a more suitable target to attack
				m_IdealMonsterState = MONSTERSTATE_COMBAT;
			}
			else if ( iConditions & bits_COND_LIGHT_DAMAGE )
			{
				MakeIdealYaw ( m_vecEnemyLKP );
				m_IdealMonsterState = MONSTERSTATE_ALERT;
			}
			else if ( iConditions & bits_COND_HEAVY_DAMAGE )
			{
				MakeIdealYaw ( m_vecEnemyLKP );
				m_IdealMonsterState = MONSTERSTATE_ALERT;
			}
			else if ( iConditions & bits_COND_HEAR_SOUND )
			{
				CSound *pSound;
				
				pSound = PBestSound();
				ASSERT( pSound != NULL );
				if ( pSound )
				{
					MakeIdealYaw ( pSound->m_vecOrigin );
					if ( pSound->m_iType & (bits_SOUND_COMBAT|bits_SOUND_DANGER) )
						m_IdealMonsterState = MONSTERSTATE_ALERT;
				}
			}
			else if ( iConditions & (bits_COND_SMELL | bits_COND_SMELL_FOOD) )
			{
				m_IdealMonsterState = MONSTERSTATE_ALERT;
			}

			break;
		}
	case MONSTERSTATE_ALERT:
		/*
		ALERT goes to IDLE upon becoming bored
		-ALERT goes to COMBAT upon sighting an enemy
		ALERT goes to HUNT upon hearing a noise
		*/
		{
			if ( iConditions & (bits_COND_NEW_ENEMY|bits_COND_SEE_ENEMY) )			
			{
				// see an enemy we MUST attack
				m_IdealMonsterState = MONSTERSTATE_COMBAT;
			}
			else if ( iConditions & bits_COND_HEAR_SOUND )
			{
				m_IdealMonsterState = MONSTERSTATE_ALERT;
				CSound *pSound = PBestSound();
				ASSERT( pSound != NULL );
				if ( pSound )
					MakeIdealYaw ( pSound->m_vecOrigin );
			}
			break;
		}
	case MONSTERSTATE_COMBAT:
		/*
		COMBAT goes to HUNT upon losing sight of enemy
		COMBAT goes to ALERT upon death of enemy
		*/
		{
			if ( m_hEnemy == NULL )
			{
				m_IdealMonsterState = MONSTERSTATE_ALERT;
				// pev->effects = EF_BRIGHTFIELD;
				ALERT ( at_aiconsole, "***Combat state with no enemy!\n" );
			}
			break;
		}
	case MONSTERSTATE_HUNT:
		/*
		HUNT goes to ALERT upon seeing food
		HUNT goes to ALERT upon being injured
		HUNT goes to IDLE if goal touched
		HUNT goes to COMBAT upon seeing enemy
		*/
		{
			break;
		}
	case MONSTERSTATE_SCRIPT:
		if ( iConditions & (bits_COND_TASK_FAILED|bits_COND_LIGHT_DAMAGE|bits_COND_HEAVY_DAMAGE) )
		{
			ExitScriptedSequence();	// This will set the ideal state
		}
		break;

	case MONSTERSTATE_DEAD:
		m_IdealMonsterState = MONSTERSTATE_DEAD;
		break;
	}

	return m_IdealMonsterState;
}

