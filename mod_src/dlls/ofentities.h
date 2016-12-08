//========= Copyright © 2004-2008, Raven City Team, All rights reserved. ============//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

//=================================
// Opposing Forces spore ammo plant 
// Copyright Demiurge, Copyright Highlander
//=================================
class CSpore : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void Glow( void );
	void Explode( void );
	void BounceSound( void );
	void EXPORT ExplodeThink( CBaseEntity *pOther );
	void EXPORT BounceThink( CBaseEntity *pOther );
	void EXPORT FlyThink( void );
	static CSpore *CreateSporeRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner );
	static CSpore *CreateSporeGrenade( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner );

	float m_iDrips;
	float m_iGlow;
	float m_iExplode;
	float m_iExplodeC;
	int m_iPrimaryMode;

	CSprite *m_pSprite;
};
//=========================================================
// Opposing Forces Spore Ammo
//=========================================================
#define		SACK_GROUP			1
#define		SACK_EMPTY			0
#define		SACK_FULL			1


class CSporeAmmo : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT BornThink ( void );
	void EXPORT IdleThink ( void );
	void EXPORT AmmoTouch ( CBaseEntity *pOther );
	int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

	int	Save( CSave &save ); 
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	float m_iExplode;
	float borntime;
	float m_flTimeSporeIdle;
};
//=================================
// Opposing Forces shock beam
// Copyright Demiurge, Copyright Highlander
//=================================

#define SHOCK_AIR_VELOCITY		20000

class CShockBeam : public CBaseEntity
{
	void Spawn( void );
	void Precache( void );
	int  Classify ( void );
	void EXPORT ShockTouch( CBaseEntity *pOther );
	void EXPORT FadeShock( void );
	void EXPORT ExplodeThink( void );
	void EXPORT BlastOn( void );
	void EXPORT BlastOff( void );
	void Glow( void );
	void GetAttachment ( int iAttachment, Vector &origin, Vector &angles );

	CBeam *m_pBeam;
	CBeam *m_pNoise;
	CSprite *m_pSprite;

	int m_iFadeCount;
	int m_iTrail;
	Vector m_vecForward;

	CBaseEntity *pShockedEnt;

public:
	static CShockBeam *ShockCreate( void );
};
//=================================================
// Displacer Ball	
//=================================================

#define DISPLACER_BALL_SPEED 500

class CDisplacerBall : public CBaseMonster
{
	void Spawn( void );
	void Precache( void );
	int  Classify ( void );
	void EXPORT BallTouch( CBaseEntity *pOther );
	void EXPORT ExplodeThink( void );
	void EXPORT FlyThink( void );
	void SelfRemove( void );

    BOOL LockRing;
	BOOL m_fIsSelfTeleporter;
	float m_flWaitFinished;
	int m_iDispRing;
	int m_iBeamSprite;
	CBaseEntity *pRemoveEnt;

public:
	static CDisplacerBall *BallCreate( void );
	static CDisplacerBall *SelfCreate( void );
};