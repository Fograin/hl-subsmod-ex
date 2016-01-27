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
/*
	Made by: Demiurge.
	Based on Grapping hook code by Phantasiere (nrich@iinet.net.au)

	Separate thanks for BUzer because without his councils,
	the code would be a nightmare of the programmer.

	Edits by: Fograin92 and Kevin "Rara" Clifford
*/
#include "extdll.h" 
#include "util.h" 
#include "cbase.h" 
#include "monsters.h" 
#include "weapons.h" 
#include "nodes.h" 
#include "player.h" 
#include "soundent.h" 
#include "shake.h" 
#include "gamerules.h"
#include "vector.h"

#ifndef CLIENT_DLL
	#include "particle_defs.h"
	extern int gmsgParticles;
#endif


//=================//
// Barnacle Tongue
//=================//
class CGrappleHook : public CBaseEntity
{ 
public: 
	void Spawn( void ); 
	void Precache( void ); 
	void EXPORT Move( void ); 
	void EXPORT Hit( CBaseEntity* ); 
	void Killed(entvars_t *pev, int gib);

	static	CGrappleHook* Create( Vector, Vector, CBasePlayer*);
	int		m_Chain; 
	int		m_iIsMoving; 
	int		m_iTrail_Length;
	int		m_iHitMonster;	// Fograin92: Used to handle what monster type we did hit
	BOOL	bPullBack;		// Fograin92: Used to "pull-back" tongue after miss or release

	CBasePlayer *myowner;
	CBaseEntity *myHitMonster;	// Fograin92: Pointer to our monster
	CBeam		*m_pTongue;		// Fograin92: New tongue
};
LINK_ENTITY_TO_CLASS( proj_hook, CGrappleHook );


void CGrappleHook :: Spawn( void ) 
{ 
	Precache( ); 

	SET_MODEL( ENT(pev), "models/v_bgrap_tonguetip.mdl" );
	pev->movetype	= MOVETYPE_FLY; 
	pev->solid		= SOLID_BBOX; 
	pev->rendermode = kRenderNormal;
	pev->renderamt	= 0;

	m_iHitMonster = 0;	// Fograin92: We didn't hit any monsters
	bPullBack = false;	// Fograin92: We just launched the tongue, we don't want to pull it back

	UTIL_SetSize( pev, Vector(0,0,0), Vector(0,0,0) ); 
	UTIL_SetOrigin( pev, pev->origin ); 

	pev->classname = MAKE_STRING( "proj_hook" );

	SetThink(&CGrappleHook::Move); 
	SetTouch(&CGrappleHook::Hit);

	UTIL_MakeVectors(pev->angles);
	pev->velocity = gpGlobals->v_forward * 700; 
	pev->gravity = 0; 
	pev->nextthink = gpGlobals->time + 0.01;	// Fograin92: Changed this to 0.01 it will be much smoother
	pev->dmg = 0;
}


void CGrappleHook :: Precache( void ) 
{ 
	PRECACHE_MODEL( "sprites/_tongue.spr" );
	PRECACHE_MODEL( "models/v_bgrap_tonguetip.mdl" ); 
	PRECACHE_SOUND("weapons/bgrapple_impact.wav");
}


void CGrappleHook :: Hit( CBaseEntity* Target ) 
{
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/bgrapple_impact.wav", 1, ATTN_NORM);

	TraceResult TResult; 
	Vector StartPosition;
	Vector delta = Vector( 8, 8, 0 );
	Vector mins = pev->origin - delta;
	Vector maxs = pev->origin + delta;
	maxs.z = pev->origin.z;

#ifndef CLIENT_DLL

	CBaseEntity *pList[1];
	int count = UTIL_EntitiesInBox( pList, 1, mins, maxs, (FL_MONSTER) );

	// Fograin92: We did hit a monster
	if ( count )
	{
		m_iHitMonster = 1;	// Fograin92: Let's assume we hit bigger monster.

		// Fograin92: Loop through everything we did hit.
		for (int i = 0; i < count; i++ )
		{
			ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Hit ^3%s\n", STRING(pList[i]->pev->classname) );
			myHitMonster = pList[i]; // Fograin92: Hook pointer to our monster.

			// Fograin92: Check what did we hit
			if ((FClassnameIs (pList[i]->pev, "monster_babycrab" ))
				||	(FClassnameIs (pList[i]->pev, "monster_headcrab" ))
				)
			{
				m_iHitMonster = 2;	// Fograin92: This is tiny monster.
				SetTouch(NULL);		// Fograin92: Let's reset this, we don't want multiple HIT execs when target is moving.
				break;
			}
			else if (FClassnameIs (pList[i]->pev, "monster_barnacle" ))
			{
				// Fograin92: We did hit another barnacle
				bPullBack = true;	// Let's pull back the tongue, we don't want any barnacle <-> barnacle love here...
			}
		}

		// Fograin92: If it's still "1" then we did hit bigger monster
		if (m_iHitMonster == 1)
		{
			pev->velocity = pev->velocity.Normalize( );
			myowner->m_afPhysicsFlags |= PFLAG_ON_GRAPPLE; // Set physics flag to "on grapple"
			myowner->pev->movetype = MOVETYPE_BOUNCE; // Remove gravity effect on player
			SetTouch(NULL);	// Fograin92: Let's reset this, we don't want multiple HIT execs when target is moving.
		}
	}
	else
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Tongue hit a brush entity.\n");
		TraceResult tr;
		float rgfl1[3];
		float rgfl2[3];
		const char *pTextureName;
		Vector vecSrc = pev->origin;
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 8;

		UTIL_MakeVectors( pev->v_angle );
		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( pev ), &tr );

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		vecSrc.CopyToArray(rgfl1);
		vecEnd.CopyToArray(rgfl2);
		if (pEntity)
		{
			// Fograin92: Did we hit a gib?
			if (pEntity->Classify() == CLASS_GIBS)
			{
				// Fograin92: Let's pull this gib
				myHitMonster = pEntity;
				m_iHitMonster = 2;
				SetTouch(NULL);
				return;
			}

			// Fograin92: If we are still here, try to trace texture
			pTextureName = TRACE_TEXTURE( ENT(pEntity->pev), rgfl1, rgfl2 );
		}
		else
			pTextureName = TRACE_TEXTURE( ENT(0), rgfl1, rgfl2 );

		if (pTextureName && myowner && (strnicmp (pTextureName, "xeno_", 5) == 0))
		{
			pev->velocity = pev->velocity.Normalize( );
			myowner->m_afPhysicsFlags |= PFLAG_ON_GRAPPLE; // Set physics flag to "on grapple"
			myowner->pev->movetype = MOVETYPE_BOUNCE; // Remove gravity effect on player
		}
		else
		{
			// Fograin92: We did hit a non organic wall, let's pull-back the tongue
			bPullBack = true;
		}
	}
#endif
}


void CGrappleHook::Killed(entvars_t *pev, int gib)
{
	ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Tongue was killed.\n");

	// Fograin92: Clear player
	myowner->pev->movetype = MOVETYPE_WALK; //Re-apply gravity
	myowner->m_afPhysicsFlags &= ~PFLAG_ON_GRAPPLE; //Remove "on grapple" flag
	myowner->m_iGrappleExists = 0;
	myowner->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;

	// Fograin92: Clear monster
	if( (m_iHitMonster == 2) && (myHitMonster->IsAlive()) )
		myHitMonster->pev->movetype = MOVETYPE_STEP;	// Re-apply gravity to the pulled monster, if it's alive
	
	// Fograin92: Clear tongue leftovers
	bPullBack = false;
	UTIL_Remove( m_pTongue );
	m_pTongue = NULL;

	m_iHitMonster = 0;
	SetThink(NULL);
	SetTouch(NULL);
	SUB_Remove( ); 
}


CGrappleHook* CGrappleHook :: Create( Vector Pos, Vector Aim, CBasePlayer* Owner ) 
{
	ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Tongue spawned.\n");
	CGrappleHook* Hook = GetClassPtr( (CGrappleHook*)NULL ); 
	UTIL_SetOrigin( Hook->pev, Pos ); 
	Hook->pev->angles = Aim; 
	Hook->Spawn(); 
	Hook->SetTouch( &CGrappleHook::Hit ); 
	Hook->pev->owner = Owner->edict();
	Hook->myowner = Owner;
	return Hook;
}


void CGrappleHook::Move( void ) 
{
	// Fograin92: If owner (player) is dead
	if( !myowner->IsAlive() )
	{
		Killed(pev, 0); // Remove tongue instantly
		return;
	}

	// Fograin92: Player isn't holding attack buttons
	if( !(myowner->pev->button & (IN_ATTACK|IN_ATTACK2)) )
	{
		bPullBack = true;	// Fograin92: We should pull the tongue back
	}

	// Fograin92: Animate pull-back tongue animation ONLY if we didn't hit a monster
	if(bPullBack)
	{
		UTIL_MakeVectors( myowner->pev->v_angle + myowner->pev->punchangle ); 
		Vector GunPosition = myowner->GetGunPosition();
		GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 6;

		pev->velocity = (GunPosition - pev->origin) * 10;	// Pull back the tongue tip
		float fDistance = (GunPosition - pev->origin).Length2D();	// Calculate distance between tongue tip and player
		//ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> %f\n", fDistance );

		if (fDistance < 40)
		{
			Killed(pev, 0);
			return;
		}
		
	}
	else
	{
		// Fograin92: We did hit a monster
		if(m_iHitMonster > 0)
		{
			// Fograin92: Let's "stick" grapple tongue XYZ to target's center XYZ
			pev->origin = myHitMonster->Center();

			// Fograin92: We did hit tiny monster, let's pull it
			if(m_iHitMonster == 2)
			{
				myHitMonster->pev->movetype = MOVETYPE_FLY;	// Remove gravity effect on our pulled monster
				myHitMonster->pev->velocity = (myowner->pev->origin - myHitMonster->pev->origin) * 4;	// Pull our monster
			}

			// Fograin92: Check distance (player <-> monster)
			float fDistance = (myowner->pev->origin - myHitMonster->pev->origin).Length2D();

			// Fograin92: The monster is very close to player, let's OWN IT!
			if (fDistance < 40)
			{
				ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> OWNED -> ^3%s\n", STRING(myHitMonster->pev->classname) );

				// Fograin92: Did we pull the gib?
				if( myHitMonster->Classify() == CLASS_GIBS )
					myHitMonster->SUB_Remove();
				else 
					myHitMonster->TakeDamage(myHitMonster->pev, myowner->pev, 10000, DMG_GENERIC);

				Killed(pev, 0);	// Fograin92: Target died, kill tongue
			}

		}
	}

	// Fograin92: If tongue (beam) exists
	if( m_pTongue )
	{
		UTIL_MakeVectors( myowner->pev->v_angle + myowner->pev->punchangle ); 
		Vector GunPosition = myowner->GetGunPosition();
		GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 6;

		m_pTongue->PointEntInit( GunPosition, this->entindex() );
	}
	else
	{
		// Fograin92: Create tongue (beam)
		m_pTongue = CBeam::BeamCreate( "sprites/_tongue.spr", 8 );
		m_pTongue->SetFlags( 0x20 );	// Solid beam
		m_pTongue->SetColor( 100, 100, 100 );
		m_pTongue->SetScrollRate( 20 );
	}

	pev->nextthink = gpGlobals->time + 0.01;
}



//=================//
// Barnacle Weapon
//=================//
enum grapple_e
{
	GRAPPLE_BREATHE = 0,	// 2.6 sec
	GRAPPLE_LONGIDLE,		// 10 sec
	GRAPPLE_SHORTIDLE,		// 1.36 sec
	GRAPPLE_COUGH,			// 4.63 sec
	GRAPPLE_DOWN,			// 1.36 sec
	GRAPPLE_UP,				// 1 sec
	GRAPPLE_FIRE,			// 0.63 sec
	GRAPPLE_FIREWAITING,	// 0.56 sec
	GRAPPLE_FIREREACHED,	// 4.7 sec
	GRAPPLE_FIRETRAVEL,		// 0.68 sec
	GRAPPLE_FIRERELEASE		// 1.03 sec
};
LINK_ENTITY_TO_CLASS( weapon_grapple, CGrapple );


BOOL CGrapple::IsUseable( void )
{
	return TRUE;
}


void CGrapple::Spawn( void ) 
{ 
	pev->classname = MAKE_STRING( "weapon_grapple" ); 
	Precache( ); 
	SET_MODEL(ENT(pev), "models/w_bgrap.mdl");
	m_iId = WEAPON_GRAPPLE; 
	m_iDefaultAmmo = 1; 
	FallInit( ); 
}


void CGrapple::Precache( void ) 
{ 
	PRECACHE_MODEL("models/v_bgrap.mdl");
	PRECACHE_MODEL("models/p_bgrap.mdl");
	PRECACHE_MODEL("models/w_bgrap.mdl");

	PRECACHE_SOUND("weapons/bgrapple_pull.wav");
	PRECACHE_SOUND("weapons/bgrapple_fire.wav");
	PRECACHE_SOUND("weapons/bgrapple_release.wav");
	PRECACHE_SOUND("weapons/bgrapple_wait.wav");
	PRECACHE_SOUND("weapons/bgrapple_cough.wav");

	UTIL_PrecacheOther("proj_hook");
}


int CGrapple::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}


int CGrapple::GetItemInfo( ItemInfo* Info ) 
{ 
	Info->pszName = STRING( pev->classname ); 
	Info->pszAmmo1 = NULL; 
	Info->iMaxAmmo1 = -1; 
	Info->pszAmmo2 = NULL; 
	Info->iMaxAmmo2 = -1; 
	Info->iMaxClip = 1; 
	Info->iSlot = 0; 
	Info->iPosition = 3; 
	Info->iFlags = 0;
	Info->iId = WEAPON_GRAPPLE; 
	Info->iWeight = GRAPPLE_WEIGHT;
	return 1; 
}


BOOL CGrapple::Deploy( void ) 
{
	pev->nextthink = gpGlobals->time + 1.0;
	SetThink (&CGrapple::WeaponIdle); // Fograin92: Force IDLE think
	return DefaultDeploy( "models/v_bgrap.mdl", "models/p_bgrap.mdl", GRAPPLE_UP, "hive", 0 );
}


void CGrapple::Holster( int skiplocal )
{
	STFU_Grapple();	// Fograin92: Stop looped sounds
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( GRAPPLE_DOWN );	// Fograin92: Holster anim
	pev->nextthink = gpGlobals->time + 0.1;
	SetThink (NULL);
}


void CGrapple::PrimaryAttack() 
{
	// Fograin92: If player already have a tongue
	if( m_pPlayer->m_iGrappleExists ) //if player already has a grapple
		return;

	// Fograin92: If weapon is still on cooldown
	if(m_flNextPrimaryAttack > gpGlobals->time)
		return;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	SendWeaponAnim( GRAPPLE_FIRE, 1 );
	SetThink(&CGrapple::FlyThink);
	pev->nextthink = gpGlobals->time + 0.58;

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_fire.wav", 1, ATTN_NORM);
	m_pPlayer->m_iGrappleExists = 1;
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 ); 

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle ); 
	Vector AimingDir = gpGlobals->v_forward; 
	Vector GunPosition = m_pPlayer->GetGunPosition( );

	GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 16;
	m_pPlayer->m_MyGrapple = CGrappleHook :: Create( GunPosition, m_pPlayer->pev->v_angle, m_pPlayer );
}


void CGrapple::FlyThink( void ) 
{
	// Fograin92: Grapple is pulling player
	if( m_pPlayer->m_afPhysicsFlags & PFLAG_ON_GRAPPLE ) //If we are on a grapple
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Pulling player.\n");
		SendWeaponAnim( GRAPPLE_FIRETRAVEL, 1 );
		pev->nextthink = gpGlobals->time + 0.6;
	}

	// Fograin92: Grapple tongue died
	else if ( !m_pPlayer->m_iGrappleExists )
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Released grapple hook.\n");
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_release.wav", 1, ATTN_NORM);

		SendWeaponAnim( GRAPPLE_FIRERELEASE, 1 );
		pev->nextthink = gpGlobals->time + 0.98;
		SetThink (&CGrapple::WeaponIdle);
		
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.5;	// Fograin92: Cooldown before we can use weapon again
		if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;
	}
	else
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> Tongue flying\n");
		EMIT_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_pull.wav", 1, ATTN_NORM);
		SendWeaponAnim( GRAPPLE_FIREWAITING, 1 );
		pev->nextthink = gpGlobals->time + 0.56;
	}
}


void CGrapple::WeaponIdle( void ) 
{
	STFU_Grapple();	// Fograin92: Let's shutup any looped idle sounds
	int iAnim = 0;				// Fograin92: Animation number
	int iRand = (rand()%100)+1;	// Fograin92: Random animation chance (1-100)
	float flTime = 0.0;			// Fograin92: Next WeaponIdle think time


	if(iRand >= 50)	
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> BREATHE IDLE.\n");
		iAnim = GRAPPLE_BREATHE;
		pev->nextthink = gpGlobals->time + 2.6;
	}
	else if( (iRand < 50) && (iRand >= 25))
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> LONG IDLE.\n");
		iAnim = GRAPPLE_LONGIDLE;
		pev->nextthink = gpGlobals->time + 10.0;
	}
	else if( (iRand < 25) && (iRand >= 10))
	{
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> SHORT IDLE.\n");
		iAnim = GRAPPLE_SHORTIDLE;
		pev->nextthink = gpGlobals->time + 1.36;
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_wait.wav", 1, ATTN_NORM);
	}
	else if (iRand < 10)
	{
		// Fograin92: 'cough' idle is rare animation, that way we won't annoy players with it.
		ALERT( at_console, "^2HLU -> ^3weapon_grapple ^2-> COUGH IDLE.\n");
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_cough.wav", 1, ATTN_NORM);

		iAnim = GRAPPLE_COUGH;
		pev->nextthink = gpGlobals->time + 3.33;
		SetThink (&CGrapple::PukeGibs); // Fograin92: Second part of "cough" animation
	}
	
	SendWeaponAnim( iAnim, 1 );
}

// Fograin92: Simple hacky function to shutup grapple sounds
void CGrapple::STFU_Grapple(void)
{
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_wait.wav" );
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_cough.wav" );
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/bgrapple_pull.wav" );
}

// Fograin92: Extension of "cough" animation, just for fun
void CGrapple::PukeGibs(void)
{
	// Fograin92: Make sure we go back to idle anim after this
	SetThink(&CGrapple::WeaponIdle);
	pev->nextthink = gpGlobals->time + 1.3;

#ifndef CLIENT_DLL

	// Fograin92: Get proper XYZ values
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle ); 
	Vector GunPosition = m_pPlayer->GetGunPosition();
	GunPosition = GunPosition + gpGlobals->v_up * -4 + gpGlobals->v_right * 3 + gpGlobals->v_forward * 16;

	CGib *pGib = GetClassPtr( (CGib *)NULL );

	// Fograin92: Changed model dir and names
			int iRandom = RANDOM_LONG(2, 11);

			switch( iRandom)
			{
				case 2:		pGib->Spawn( "models/gibs/human_gib2.mdl" );	break;
				case 3:		pGib->Spawn( "models/gibs/human_gib3.mdl" );	break;
				case 4:		pGib->Spawn( "models/gibs/human_gib4.mdl" );	break;
				case 5:		pGib->Spawn( "models/gibs/human_gib5.mdl" );	break;
				case 6:		pGib->Spawn( "models/gibs/human_gib6.mdl" );	break;
				case 7:		pGib->Spawn( "models/gibs/human_gib7.mdl" );	break;
				case 8:		pGib->Spawn( "models/gibs/human_gib8.mdl" );	break;
				case 9:		pGib->Spawn( "models/gibs/human_gib9.mdl" );	break;
				case 10:	pGib->Spawn( "models/gibs/human_gib10.mdl" );	break;
				case 11:	pGib->Spawn( "models/gibs/human_gib11.mdl" );	break;

				default:	pGib->Spawn( "models/gibs/human_gib3.mdl" );	break;
			}

			pGib->pev->body = 0;


	//pGib->Spawn( "models/hgibs.mdl" );
	pGib->m_bloodColor = BLOOD_COLOR_RED;
	pGib->pev->body = 0;
	pGib->pev->origin = GunPosition + gpGlobals->v_forward * 40;
	pGib->pev->velocity = gpGlobals->v_forward * 100;

	// Fograin92: Some spin variations
	pGib->pev->avelocity.x = RANDOM_LONG( 200, 600 );
	pGib->pev->avelocity.y = RANDOM_LONG( 200, 600 );
	pGib->pev->avelocity.z = RANDOM_LONG( 100, 200 );

	pGib->pev->nextthink = gpGlobals->time + 10.0;
	pGib->SetThink( &CBaseEntity::SUB_FadeOut );

	// Fograin92: Cough some blood
	MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
		WRITE_SHORT(0);
		WRITE_BYTE(0);
		WRITE_COORD( pGib->pev->origin.x );
	    WRITE_COORD( pGib->pev->origin.y );
		WRITE_COORD( pGib->pev->origin.z );
		WRITE_COORD( pGib->pev->avelocity.x );
		WRITE_COORD( pGib->pev->avelocity.y );
		WRITE_COORD( pGib->pev->avelocity.z );
		WRITE_SHORT(iImpactBloodRed);
    MESSAGE_END();

#endif
}

