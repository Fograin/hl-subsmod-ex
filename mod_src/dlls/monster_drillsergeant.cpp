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
#include	"monster_drillsergeant.h"	// Fograin92


//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CDrillSGT :: ISoundMask ( void) 
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_CARCASS	|
			bits_SOUND_MEAT		|
			bits_SOUND_GARBAGE	|
			bits_SOUND_DANGER	|
			bits_SOUND_PLAYER;
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CDrillSGT :: Classify ( void )
{
	return	CLASS_PLAYER_ALLY;
}


//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CDrillSGT :: SetYawSpeed ( void )
{
	int ys;

	ys = 0;

	switch ( m_Activity )
	{
		case ACT_IDLE:		
			ys = 70;
			break;

		case ACT_WALK:
			ys = 70;
			break;

		case ACT_RUN:
			ys = 90;
			break;

		default:
			ys = 70;
			break;
	}

	pev->yaw_speed = ys;
}



//=========================================================
// Spawn
//=========================================================
void CDrillSGT :: Spawn()
{
	Precache( );

	SET_MODEL(ENT(pev), "models/drill.mdl");
	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	pev->health			= gSkillData.barneyHealth;
	pev->view_ofs		= Vector ( 0, 0, 50 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState		= MONSTERSTATE_NONE;
	m_afCapability		= bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;

	MonsterInit();
	SetUse( &CTalkMonster::FollowerUse );
}


//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CDrillSGT :: Precache()
{
	PRECACHE_MODEL("models/drill.mdl");
	PRECACHE_SOUND("drill/shit.wav");	// Fograin92: For all yours pain and death sounds needs.

	TalkInit();
	CTalkMonster::Precache();
}	


// Init talk data
void CDrillSGT :: TalkInit()
{
	CTalkMonster::TalkInit();

	// Drill instructor will try to talk to friends in this order:
	m_szFriends[0] = "monster_drillsergeant";
	m_szFriends[1] = "monster_recruit";
	m_szFriends[2] = "monster_human_grunt_ally";

	if( pev->skin == 1 )
	{
		m_szGrp[TLK_ANSWER]		=	"DR_DR2_ADD";
		m_szGrp[TLK_QUESTION]	=	"DR_DR2_ADD";
		m_szGrp[TLK_IDLE]		=	"NULL";
		m_szGrp[TLK_STARE]		=	"NULL";
		m_szGrp[TLK_USE]		=	"DR_DR2_ADD";
		m_szGrp[TLK_UNUSE]		=	"DR_DR2_ADD";
		m_szGrp[TLK_STOP]		=	"NULL";
		m_szGrp[TLK_NOSHOOT]	=	"NULL";
		m_szGrp[TLK_HELLO]		=	"NULL";
		m_szGrp[TLK_PHELLO]		=	"NULL";
		m_szGrp[TLK_PIDLE]		=	"NULL";
		m_szGrp[TLK_PQUESTION]	=	"NULL";
		m_szGrp[TLK_SMELL]		=	"NULL";
		m_szGrp[TLK_WOUND]		=	"NULL";
		m_szGrp[TLK_MORTAL]		=	"NULL";
	}
	else
	{
		m_szGrp[TLK_ANSWER]		=	"DR_ADD";
		m_szGrp[TLK_QUESTION]	=	"DR_ADD";
		m_szGrp[TLK_IDLE]		=	"NULL";
		m_szGrp[TLK_STARE]		=	"NULL";
		m_szGrp[TLK_USE]		=	"DR_ADD";
		m_szGrp[TLK_UNUSE]		=	"DR_ADD";
		m_szGrp[TLK_STOP]		=	"NULL";
		m_szGrp[TLK_NOSHOOT]	=	"NULL";
		m_szGrp[TLK_HELLO]		=	"NULL";
		m_szGrp[TLK_PHELLO]		=	"NULL";
		m_szGrp[TLK_PIDLE]		=	"NULL";
		m_szGrp[TLK_PQUESTION]	=	"NULL";
		m_szGrp[TLK_SMELL]		=	"NULL";
		m_szGrp[TLK_WOUND]		=	"NULL";
		m_szGrp[TLK_MORTAL]		=	"NULL";
	}

	m_voicePitch = 100;
}

int CDrillSGT :: TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	// make sure friends talk about it if player hurts talkmonsters...
	int ret = CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	return ret;
}

	
//=========================================================
// PainSound
//=========================================================
void CDrillSGT :: PainSound ( void )
{
	if (gpGlobals->time < m_painTime)
		return;
	
	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "drill/shit.wav", 1, ATTN_NORM, 0, GetVoicePitch());
}

//=========================================================
// DeathSound 
//=========================================================
void CDrillSGT :: DeathSound ( void )
{
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "drill/shit.wav", 1, ATTN_NORM, 0, GetVoicePitch());
}


void CDrillSGT::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	switch( ptr->iHitgroup)
	{
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_BLAST))
			{
				flDamage = flDamage / 2;
			}
		break;

		case 10:
			if (bitsDamageType & (DMG_BULLET | DMG_SLASH | DMG_CLUB))
			{
				flDamage -= 20;
				if (flDamage <= 0)
				{
					UTIL_Ricochet( ptr->vecEndPos, 1.0 );
					flDamage = 0.01;
				}
			}
			// always a head shot
			ptr->iHitgroup = HITGROUP_HEAD;
		break;
	}

	CTalkMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}


void CDrillSGT::Killed( entvars_t *pevAttacker, int iGib )
{
	// Fograin92: Drop item?

		//Vector vecGunPos;
		//Vector vecGunAngles;

		//pev->body = BARNEY_BODY_GUNGONE;

		//GetAttachment( 0, vecGunPos, vecGunAngles );
		
		//CBaseEntity *pGun = DropItem( "weapon_9mmhandgun", vecGunPos, vecGunAngles );

	SetBodygroup( DRILL_GROUP_ACCESSORIES, DRILL_ACCESSORIES_NONE );	// Fograin92: Empty hands
	SetUse( NULL );	
	CTalkMonster::Killed( pevAttacker, iGib );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Schedule_t* CDrillSGT :: GetScheduleOfType ( int Type )
{
	return CTalkMonster::GetScheduleOfType( Type );
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CDrillSGT :: GetSchedule ( void )
{
	switch( m_MonsterState )
	{

		case MONSTERSTATE_ALERT:	
		case MONSTERSTATE_IDLE:
			if ( HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
			{
				// flinch if hurt
				return GetScheduleOfType( SCHED_SMALL_FLINCH );
			}

			if ( m_hEnemy == NULL && IsFollowing() )
			{
				if ( !m_hTargetEnt->IsAlive() )
				{
					// UNDONE: Comment about the recently dead player here?
					StopFollowing( FALSE );
					break;
				}
				else
				{
					if ( HasConditions( bits_COND_CLIENT_PUSH ) )
					{
						return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );
					}
					return GetScheduleOfType( SCHED_TARGET_FACE );
				}
			}

			if ( HasConditions( bits_COND_CLIENT_PUSH ) )
			{
				return GetScheduleOfType( SCHED_MOVE_AWAY );
			}

		break;
	}
	
	return CTalkMonster::GetSchedule();
}


MONSTERSTATE CDrillSGT :: GetIdealState ( void )
{
	return CTalkMonster::GetIdealState();
}


void CDrillSGT::DeclineFollowing( void )
{
	if( pev->skin == 1 )
		PlaySentence( "DR_DR2_ADD", 2, VOL_NORM, ATTN_NORM );
	else
		PlaySentence( "DR_ADD", 2, VOL_NORM, ATTN_NORM );
}


//=========//
// LINKERS
//=========//
LINK_ENTITY_TO_CLASS( monster_drillsergeant, CDrillSGT );
