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
#ifndef MONSTER_GONOME_H
#define MONSTER_GONOME_H

// Written by: Base Source-Code written by Raven City and Marc-Antoine Lortie (https://github.com/malortie).
// Edits by: Hammermaps.de DEV Team (support@hammermaps.de) and Fograin92
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"
#include	"schedule.h"
#include	"player.h"
#include	"soundent.h"
#include    "weapons.h"
#include    "skill.h"
#include	"effects.h"
#include	"decals.h"
#include	"monster_bullchicken.h"

class CGonomeSpit : public CBaseEntity {
public:
	void Spawn(void);
	void Precache(void);
	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void Touch(CBaseEntity *pOther);
	void EXPORT Animate(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int iGonomeSpitSprite;
	int  m_maxFrame;
};

// monster_gonome
class CGonome : public CBullsquid
{
	public:
		void Spawn(void);
		void Precache(void);
		void SetYawSpeed(void);
		int  ISoundMask(void);

		int  Classify(void);
		void HandleAnimEvent(MonsterEvent_t *pEvent);
		void PainSound(void);
		void DeathSound(void);
		void AlertSound(void);
		void IdleSound(void);
		void AttackSound(void);
		void StartTask(Task_t *pTask);

		int	Save(CSave &save);
		int Restore(CRestore &restore);
		static TYPEDESCRIPTION m_SaveData[];

		Schedule_t *GetSchedule(void);
		Schedule_t *GetScheduleOfType(int Type);

		static const char *pAttackSounds[];
		static const char *pIdleSounds[];
		static const char *pPainSounds[];
		static const char *pDeathSounds[];
		static const char *pAttackHitSounds[];
		static const char *pAttackMissSounds[];

		BOOL CheckMeleeAttack1(float flDot, float flDist);
		BOOL CheckMeleeAttack2(float flDot, float flDist);
		BOOL CheckRangeAttack1(float flDot, float flDist);
		void RunAI(void);

		int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
		int IgnoreConditions(void);
		MONSTERSTATE GetIdealState(void);
		CUSTOM_SCHEDULES;

		int iGonomeSpitSprite;
		float m_flLastHurtTime;// we keep track of this, because if something hurts a gonome, it will forget about its love of headcrabs for a while.
		float m_flNextSpitTime;// last time the Gonome used the spit attack.
		float m_flNextFlinch;
};


#endif // MONSTER_GONOME_H