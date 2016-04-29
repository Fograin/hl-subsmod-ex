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
#ifndef MONSTER_DRILL_SGT_H
#define MONSTER_DRILL_SGT_H

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


#define DRILL_GROUP_BODY			0
#define DRILL_GROUP_ACCESSORIES		0

#define DRILL_ACCESSORIES_NONE			0
#define DRILL_ACCESSORIES_WHISTLE		1
#define DRILL_ACCESSORIES_MEGAPHONE		2
#define DRILL_ACCESSORIES_BINOCULARS	3


class CDrillSGT : public CTalkMonster
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
};

#endif // MONSTER_DRILL_SGT_H