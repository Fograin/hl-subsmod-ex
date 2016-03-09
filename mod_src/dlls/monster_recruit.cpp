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
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"talkmonster.h"
#include	"ai_schedule.h"
#include	"ai_default.h"
#include	"ai_scripted.h"
#include	"weapons.h"
#include	"soundent.h"


class CRecruit : public CTalkMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  ISoundMask( void );
	int  Classify ( void );

	virtual int	ObjectCaps( void ) { return CTalkMonster :: ObjectCaps() | FCAP_IMPULSE_USE; }
	int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void DeclineFollowing( void );

	// Override these to set behavior
	Schedule_t *GetScheduleOfType ( int Type );
	Schedule_t *GetSchedule ( void );
	MONSTERSTATE GetIdealState ( void );

	void DeathSound( void );
	void PainSound( void );
	void TalkInit( void );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void Killed( entvars_t *pevAttacker, int iGib );

private:
	float	m_painTime;

	static const char *pReDeathSounds[];
};

LINK_ENTITY_TO_CLASS( monster_recruit, CRecruit );


// Fograin92: Recruit death sounds
const char *CRecruit::pReDeathSounds[] = 
{
	"fgrunt/death1.wav",
	"fgrunt/death2.wav",
	"fgrunt/death3.wav",
	"fgrunt/death4.wav",
	"fgrunt/death5.wav",
	"fgrunt/death6.wav",
};


//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CRecruit :: ISoundMask ( void) 
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
int	CRecruit :: Classify ( void )
{
	return	CLASS_PLAYER_ALLY;
}


//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CRecruit :: SetYawSpeed ( void )
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
void CRecruit :: Spawn()
{
	Precache( );

	SET_MODEL(ENT(pev), "models/recruit.mdl");
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
void CRecruit :: Precache()
{
	PRECACHE_MODEL("models/recruit.mdl");
	PRECACHE_SOUND("drill/shit.wav");	// Fograin92: For all yours pain and death sounds needs.

	// Fograin92: Precache death sounds
	for ( int i = 0; i < ARRAYSIZE( pReDeathSounds ); i++ )
		PRECACHE_SOUND((char *)pReDeathSounds[i]);

	TalkInit();
	CTalkMonster::Precache();
}	


// Init talk data
void CRecruit :: TalkInit()
{
	CTalkMonster::TalkInit();

	m_szGrp[TLK_ANSWER]		=	"NULL";
	m_szGrp[TLK_QUESTION]	=	"NULL";
	m_szGrp[TLK_IDLE]		=	"NULL";
	m_szGrp[TLK_STARE]		=	"NULL";
	m_szGrp[TLK_USE]		=	"RE_NEG";
	m_szGrp[TLK_UNUSE]		=	"RE_NEG";
	m_szGrp[TLK_STOP]		=	"NULL";
	m_szGrp[TLK_NOSHOOT]	=	"NULL";
	m_szGrp[TLK_HELLO]		=	"NULL";
	m_szGrp[TLK_PHELLO]		=	"NULL";
	m_szGrp[TLK_PIDLE]		=	"NULL";
	m_szGrp[TLK_PQUESTION]	=	"NULL";
	m_szGrp[TLK_SMELL]		=	"NULL";
	m_szGrp[TLK_WOUND]		=	"NULL";
	m_szGrp[TLK_MORTAL]		=	"NULL";

	m_voicePitch = 100;
}

int CRecruit :: TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	// make sure friends talk about it if player hurts talkmonsters...
	int ret = CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	return ret;
}

	
//=========================================================
// PainSound
//=========================================================
void CRecruit :: PainSound ( void )
{
	if (gpGlobals->time < m_painTime)
		return;
	
	m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);
	EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, "drill/shit.wav", 1, ATTN_NORM, 0, GetVoicePitch());
}

//=========================================================
// DeathSound 
//=========================================================
void CRecruit :: DeathSound ( void )
{
	EMIT_SOUND_DYN ( ENT(pev), CHAN_VOICE, pReDeathSounds[ RANDOM_LONG(0,ARRAYSIZE(pReDeathSounds)-1) ], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5,5) );
}


void CRecruit::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
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


void CRecruit::Killed( entvars_t *pevAttacker, int iGib )
{
	SetUse( NULL );	
	CTalkMonster::Killed( pevAttacker, iGib );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Schedule_t* CRecruit :: GetScheduleOfType ( int Type )
{
	return CTalkMonster::GetScheduleOfType( Type );
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CRecruit :: GetSchedule ( void )
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


MONSTERSTATE CRecruit :: GetIdealState ( void )
{
	return CTalkMonster::GetIdealState();
}


void CRecruit::DeclineFollowing( void )
{
	
}

