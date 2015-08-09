//=============================================================//
//	Half-Life Subtitles MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	Valve LLC.
//	Id Software, Inc. ("Id Technology")
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//



// e3/prealpha only. 
class CCineBlood : public CBaseEntity
{
public:
	void Spawn( void );
	void EXPORT BloodStart ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT BloodGush ( void );
};

LINK_ENTITY_TO_CLASS( cine_blood, CCineBlood );


void CCineBlood :: BloodGush ( void )
{
	Vector	vecSplatDir;
	TraceResult	tr;
	pev->nextthink = gpGlobals->time + 0.1;

	UTIL_MakeVectors(pev->angles);
	if ( pev->health-- < 0 )
		REMOVE_ENTITY(ENT(pev));
// CHANGE_METHOD ( ENT(pev), em_think, SUB_Remove );

	if ( RANDOM_FLOAT ( 0 , 1 ) < 0.7 )// larger chance of globs
	{
		UTIL_BloodDrips( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, 10 );
	}
	else// slim chance of geyser
	{
		UTIL_BloodStream( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, RANDOM_LONG(50, 150) );
	}

	if ( RANDOM_FLOAT ( 0, 1 ) < 0.75 )
	{// decals the floor with blood.
		vecSplatDir = Vector ( 0 , 0 , -1 );
		vecSplatDir = vecSplatDir + (RANDOM_FLOAT(-1,1) * 0.6 * gpGlobals->v_right) + (RANDOM_FLOAT(-1,1) * 0.6 * gpGlobals->v_forward);// randomize a bit
		UTIL_TraceLine( pev->origin + Vector ( 0, 0 , 64) , pev->origin + vecSplatDir * 256, ignore_monsters, ENT(pev), &tr);
		if ( tr.flFraction != 1.0 )
		{
			// Decal with a bloodsplat
			UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
		}
	}
}

void CCineBlood :: BloodStart ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CCineBlood::BloodGush );
	pev->nextthink = gpGlobals->time;// now!
}

void CCineBlood :: Spawn ( void )
{
	pev->solid = SOLID_NOT;
	SetUse ( &CCineBlood::BloodStart );
	pev->health = 20;//hacked health to count iterations
}

