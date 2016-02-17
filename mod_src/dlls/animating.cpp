/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== monsters.cpp ========================================================

  Monster-related utility code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "animation.h"
#include "saverestore.h"

#include "particle_defs.h"	// Fograin92: BG Particle system
extern int gmsgParticles;	// Fograin92: BG Particle system

TYPEDESCRIPTION	CBaseAnimating::m_SaveData[] = 
{
	DEFINE_FIELD( CBaseMonster, m_flFrameRate, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flGroundSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flLastEventCheck, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceFinished, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceLoops, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CBaseAnimating, CBaseDelay );


//=========================================================
// StudioFrameAdvance - advance the animation frame up to the current time
// if an flInterval is passed in, only advance animation that number of seconds
//=========================================================
float CBaseAnimating :: StudioFrameAdvance ( float flInterval )
{
	if (flInterval == 0.0)
	{
		flInterval = (gpGlobals->time - pev->animtime);
		if (flInterval <= 0.001)
		{
			pev->animtime = gpGlobals->time;
			return 0.0;
		}
	}
	if (! pev->animtime)
		flInterval = 0.0;
	
	pev->frame += flInterval * m_flFrameRate * pev->framerate;
	pev->animtime = gpGlobals->time;

	if (pev->frame < 0.0 || pev->frame >= 256.0) 
	{
		if (m_fSequenceLoops)
			pev->frame -= (int)(pev->frame / 256.0) * 256.0;
		else
			pev->frame = (pev->frame < 0.0) ? 0 : 255;
		m_fSequenceFinished = TRUE;	// just in case it wasn't caught in GetEvents
	}

	return flInterval;
}

//=========================================================
// LookupActivity
//=========================================================
int CBaseAnimating :: LookupActivity ( int activity )
{
	ASSERT( activity != 0 );
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivity( pmodel, pev, activity );
}

//=========================================================
// LookupActivityHeaviest
//
// Get activity with highest 'weight'
//
//=========================================================
int CBaseAnimating :: LookupActivityHeaviest ( int activity )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivityHeaviest( pmodel, pev, activity );
}

//=========================================================
//=========================================================
int CBaseAnimating :: LookupSequence ( const char *label )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupSequence( pmodel, label );
}


//=========================================================
//=========================================================
void CBaseAnimating :: ResetSequenceInfo ( )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ((GetSequenceFlags() & STUDIO_LOOPING) != 0);
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
}



//=========================================================
//=========================================================
BOOL CBaseAnimating :: GetSequenceFlags( )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::GetSequenceFlags( pmodel, pev );
}

//=========================================================
// DispatchAnimEvents
//=========================================================
void CBaseAnimating :: DispatchAnimEvents ( float flInterval )
{
	MonsterEvent_t	event;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	if ( !pmodel )
	{
		ALERT( at_aiconsole, "Gibbed monster is thinking!\n" );
		return;
	}

	// FIXME: I have to do this or some events get missed, and this is probably causing the problem below
	flInterval = 0.1;

	// FIX: this still sometimes hits events twice
	float flStart = pev->frame + (m_flLastEventCheck - pev->animtime) * m_flFrameRate * pev->framerate;
	float flEnd = pev->frame + flInterval * m_flFrameRate * pev->framerate;
	m_flLastEventCheck = pev->animtime + flInterval;

	m_fSequenceFinished = FALSE;
	if (flEnd >= 256 || flEnd <= 0.0) 
		m_fSequenceFinished = TRUE;

	int index = 0;

	while ( (index = GetAnimationEvent( pmodel, pev, &event, flStart, flEnd, index ) ) != 0 )
	{
		HandleAnimEvent( &event );
	}
}


//=========================================================
//=========================================================
float CBaseAnimating :: SetBoneController ( int iController, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return SetController( pmodel, pev, iController, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: InitBoneControllers ( void )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	SetController( pmodel, pev, 0, 0.0 );
	SetController( pmodel, pev, 1, 0.0 );
	SetController( pmodel, pev, 2, 0.0 );
	SetController( pmodel, pev, 3, 0.0 );
}

//=========================================================
//=========================================================
float CBaseAnimating :: SetBlending ( int iBlender, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::SetBlending( pmodel, pev, iBlender, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetBonePosition ( int iBone, Vector &origin, Vector &angles )
{
	GET_BONE_POSITION( ENT(pev), iBone, origin, angles );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetAttachment ( int iAttachment, Vector &origin, Vector &angles )
{
	GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
}

//=========================================================
//=========================================================
int CBaseAnimating :: FindTransition( int iEndingSequence, int iGoalSequence, int *piDir )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	
	if (piDir == NULL)
	{
		int iDir;
		int sequence = ::FindTransition( pmodel, iEndingSequence, iGoalSequence, &iDir );
		if (iDir != 1)
			return -1;
		else
			return sequence;
	}

	return ::FindTransition( pmodel, iEndingSequence, iGoalSequence, piDir );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetAutomovement( Vector &origin, Vector &angles, float flInterval )
{

}

void CBaseAnimating :: SetBodygroup( int iGroup, int iValue )
{
	::SetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup, iValue );
}

int CBaseAnimating :: GetBodygroup( int iGroup )
{
	return ::GetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup );
}


int CBaseAnimating :: ExtractBbox( int sequence, float *mins, float *maxs )
{
	return ::ExtractBbox( GET_MODEL_PTR( ENT(pev) ), sequence, mins, maxs );
}

//=========================================================
//=========================================================

void CBaseAnimating :: SetSequenceBox( void )
{
	Vector mins, maxs;

	// Get sequence bbox
	if ( ExtractBbox( pev->sequence, mins, maxs ) )
	{
		// expand box for rotation
		// find min / max for rotations
		float yaw = pev->angles.y * (M_PI / 180.0);
		
		Vector xvector, yvector;
		xvector.x = cos(yaw);
		xvector.y = sin(yaw);
		yvector.x = -sin(yaw);
		yvector.y = cos(yaw);
		Vector bounds[2];

		bounds[0] = mins;
		bounds[1] = maxs;
		
		Vector rmin( 9999, 9999, 9999 );
		Vector rmax( -9999, -9999, -9999 );
		Vector base, transformed;

		for (int i = 0; i <= 1; i++ )
		{
			base.x = bounds[i].x;
			for ( int j = 0; j <= 1; j++ )
			{
				base.y = bounds[j].y;
				for ( int k = 0; k <= 1; k++ )
				{
					base.z = bounds[k].z;
					
				// transform the point
					transformed.x = xvector.x*base.x + yvector.x*base.y;
					transformed.y = xvector.y*base.x + yvector.y*base.y;
					transformed.z = base.z;
					
					if (transformed.x < rmin.x)
						rmin.x = transformed.x;
					if (transformed.x > rmax.x)
						rmax.x = transformed.x;
					if (transformed.y < rmin.y)
						rmin.y = transformed.y;
					if (transformed.y > rmax.y)
						rmax.y = transformed.y;
					if (transformed.z < rmin.z)
						rmin.z = transformed.z;
					if (transformed.z > rmax.z)
						rmax.z = transformed.z;
				}
			}
		}
		rmin.z = 0;
		rmax.z = rmin.z + 1;
		UTIL_SetSize( pev, rmin, rmax );
	}


	// Fograin92: Spawn blood puddle FX
	if( CVAR_GET_FLOAT("sm_particles") > 0 )
	{
		Vector vecSrc;
		vecSrc = Center();

		// Fograin92: Only if NOT in water
		if( pev->waterlevel == 0 )
		{
			// Fograin92: Handle specific NPCs
			if( FClassnameIs(pev, "monster_alien_controller")
				|| FClassnameIs(pev, "monster_apache")
				|| FClassnameIs(pev, "monster_barnacle")
				|| FClassnameIs(pev, "monster_cockroach")
				|| FClassnameIs(pev, "monster_flyer")
				|| FClassnameIs(pev, "monster_gargantua")
				|| FClassnameIs(pev, "monster_geneworm")
				|| FClassnameIs(pev, "monster_ichthyosaur")
				|| FClassnameIs(pev, "monster_leech")
				|| FClassnameIs(pev, "monster_nihilanth")
				|| FClassnameIs(pev, "monster_osprey")
				|| FClassnameIs(pev, "monster_pitworm")
				|| FClassnameIs(pev, "monster_rat")
				|| FClassnameIs(pev, "monster_robo_loader")
				|| FClassnameIs(pev, "monster_tentacle")
				)
			{
				// Fograin92: Placeholder for different after death particles
			}
			else
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
					WRITE_SHORT(0);
					WRITE_BYTE(0);
					WRITE_COORD(pev->origin.x);	// pev->origin xyz
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z + 0.2);
					WRITE_COORD(0);
					WRITE_COORD(0);
					WRITE_COORD(0);
					//WRITE_COORD(pev->angles.x); // pev->angles xyz
					//WRITE_COORD(pev->angles.y);
					//WRITE_COORD(pev->angles.z);
					
					if( BloodColor() == BLOOD_COLOR_RED )
					{
						//int iRand = RANDOM_LONG(0,1);
						if( RANDOM_LONG(0,1) )
							WRITE_SHORT(iBloodRedPuddle1);
						else
							WRITE_SHORT(iBloodRedPuddle2);
					}
					else
					{
						if( RANDOM_LONG(0,1) )
							WRITE_SHORT(iBloodAlienPuddle1);
						else
							WRITE_SHORT(iBloodAlienPuddle2);
					}
				MESSAGE_END();
			}
		} // if( pev->waterlevel == 0 )
	} // if(CVAR_GET_FLOAT("sm_particles") > 0)

}

