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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "skill.h"
#include "items.h"
#include "gamerules.h"

extern int gmsgItemPickup;
extern int gEvilImpulse101;


class CWorldItem : public CBaseEntity
{
public:
	void	KeyValue(KeyValueData *pkvd ); 
	void	Spawn( void );
	int		m_iType;
};
LINK_ENTITY_TO_CLASS(world_items, CWorldItem);


void CWorldItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "type"))
	{
		m_iType = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


void CWorldItem::Spawn( void )
{
	CBaseEntity *pEntity = NULL;

	switch (m_iType) 
	{
		case 44: // ITEM_BATTERY:
			pEntity = CBaseEntity::Create( "item_battery", pev->origin, pev->angles );
		break;

		case 42: // ITEM_ANTIDOTE:
			pEntity = CBaseEntity::Create( "item_antidote", pev->origin, pev->angles );
		break;

		case 43: // ITEM_SECURITY:
			pEntity = CBaseEntity::Create( "item_security", pev->origin, pev->angles );
		break;

		case 45: // ITEM_SUIT:
			pEntity = CBaseEntity::Create( "item_suit", pev->origin, pev->angles );
		break;
	}

	if (!pEntity)
		ALERT( at_console, "unable to create world_item %d\n", m_iType );
	else
	{
		pEntity->pev->target = pev->target;
		pEntity->pev->targetname = pev->targetname;
		pEntity->pev->spawnflags = pev->spawnflags;
	}

	REMOVE_ENTITY(edict());
}


void CItem::Spawn( void )
{
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	SetTouch(&CItem::ItemTouch);

	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f\n", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove( this );
		return;
	}
}


void CItem::ItemTouch( CBaseEntity *pOther )
{
	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// ok, a player is touching this item, but can he have it?
	if ( !g_pGameRules->CanHaveItem( pPlayer, this ) )
		return;	// no? Ignore the touch.

	if (MyTouch( pPlayer ))
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		SetTouch( NULL );
		g_pGameRules->PlayerGotItem( pPlayer, this );	// player grabbed the item.
		UTIL_Remove( this );
	}
	else if(gEvilImpulse101)	// Fix for Impulse 101 spawning
		UTIL_Remove( this );
}


void CItem::Materialize( void )
{
	if ( pev->effects & EF_NODRAW )
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150 );
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}

	SetTouch( &CItem::ItemTouch );
}



//========================================//
// item_antidote - Antidote item (legacy)
//========================================//
class CItemAntidote : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_antidote.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_antidote.mdl");
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		// Fograin92: Only if this is HL1 game
		if (CVAR_GET_FLOAT("sm_hud") == 0)
			pPlayer->SetSuitUpdate("!HEV_DET4", FALSE, SUIT_NEXT_IN_1MIN);
		
		pPlayer->m_rgItems[ITEM_ANTIDOTE] += 1;
		return TRUE;
	}
};
LINK_ENTITY_TO_CLASS(item_antidote, CItemAntidote);



//========================================//
// item_security - Security card (legacy)
//========================================//
class CItemSecurity : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_security.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_security.mdl");
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		pPlayer->m_rgItems[ITEM_SECURITY] += 1;
		return TRUE;
	}
};
LINK_ENTITY_TO_CLASS(item_security, CItemSecurity);



//==========================================================================//
// item_airtank - Air tank, used to refil air underwater, also it explodes!
//==========================================================================//
class CAirtank : public CGrenade
{
	void Spawn( void )
	{
		Precache( );
		// motor
		pev->movetype = MOVETYPE_FLY;
		pev->solid = SOLID_BBOX;

		SET_MODEL(ENT(pev), "models/w_oxygen.mdl");
		UTIL_SetSize(pev, Vector( -16, -16, 0), Vector(16, 16, 36));
		UTIL_SetOrigin( pev, pev->origin );

		SetTouch( &CAirtank::TankTouch );
		SetThink( &CAirtank::TankThink );

		pev->flags |= FL_MONSTER;
		pev->takedamage		= DAMAGE_YES;
		pev->health			= 20;
		pev->dmg			= 50;
		m_state				= 1;
	}

	void CAirtank::Precache( void )
	{
		PRECACHE_MODEL("models/w_oxygen.mdl");
		PRECACHE_SOUND("doors/aliendoor3.wav");
	}

	void CAirtank :: Killed( entvars_t *pevAttacker, int iGib )
	{
		pev->owner = ENT( pevAttacker );

		// UNDONE: this should make a big bubble cloud, not an explosion
		// Fograin92: Fuck it, explosions = more fun
		Explode( pev->origin, Vector( 0, 0, -1 ) );
	}

	void CAirtank::TankThink( void )
	{
		// Fire trigger
		m_state = 1;
		SUB_UseTargets( this, USE_TOGGLE, 0 );
	}


	void CAirtank::TankTouch( CBaseEntity *pOther )
	{
		if ( !pOther->IsPlayer() )
			return;

		if (!m_state)
		{
			// "no oxygen" sound
			EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 1.0, ATTN_NORM );
			return;
		}
			
		// give player 12 more seconds of air
		pOther->pev->air_finished = gpGlobals->time + 12;

		// suit recharge sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "doors/aliendoor3.wav", 1.0, ATTN_NORM );

		// recharge airtank in 30 seconds
		pev->nextthink = gpGlobals->time + 30;
		m_state = 0;
		SUB_UseTargets( this, USE_TOGGLE, 1 );
	}
	
	int	 BloodColor( void )
	{
		return DONT_BLEED;
	};

	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	int	 m_state;
};
LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );

TYPEDESCRIPTION	CAirtank::m_SaveData[] = 
{
	DEFINE_FIELD( CAirtank, m_state, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CAirtank, CGrenade );



//============================//
// item_suit - HEV, PCV SUIT
//============================//
#define SF_SUIT_SHORTLOGON		0x0001

class CItemSuit : public CItem
{
	void Spawn( void )
	{ 
		Precache( );

		// Fograin92: If this is Opposing Force, we use PCV model
		if (CVAR_GET_FLOAT("sm_hud") == 2.0 )
			SET_MODEL(ENT(pev), "models/w_pcv.mdl");
		else
			SET_MODEL(ENT(pev), "models/w_suit.mdl");

		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_suit.mdl");
		PRECACHE_MODEL ("models/w_pcv.mdl");
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		// Fograin92: We already have HEV/PCV we don't need another one
		if ( pPlayer->pev->weapons & (1<<WEAPON_SUIT) )
			return FALSE;

		// Fograin92: Only if this is HL1 game
		if (CVAR_GET_FLOAT("sm_hud") == 0.0)
		{
			// Only if it's not impulse 101 (aka. STFU logon spam when testing)
			if (!gEvilImpulse101)
			{
				if ( pev->spawnflags & SF_SUIT_SHORTLOGON )
					EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
				else
					EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon
			}
		}

		pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

		// Fograin92: Let's send message to the client that we picked up HEV. (Required to invoke new HUD update)
		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();

		return TRUE;
	}
};
LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);



//============================//
// item_battery - HEV Battery
//============================//
class CItemBattery : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_battery.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_battery.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
			return FALSE;

		if (
				(pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY) &&	// Fograin92: If current armor value is lower than max
				(pPlayer->pev->weapons & (1<<WEAPON_SUIT)) &&		// Fograin92: AND if we have HEV suit
				!((CVAR_GET_FLOAT("sm_hud") == 1.0 ))				// Fograin92: AND if it's NOT Blue Shift
			)
		{
			int pct;
			char szcharge[64];

			pPlayer->pev->armorvalue += gSkillData.batteryCapacity;
			pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY);

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();


			// Fograin92: Only if this is HL1 game
			if (CVAR_GET_FLOAT("sm_hud") == 0.0)
			{
				// Suit reports new power level
				// For some reason this wasn't working in release build -- round it.
				pct = (int)( (float)(pPlayer->pev->armorvalue * 100.0) * (1.0/MAX_NORMAL_BATTERY) + 0.5);
				pct = (pct / 5);
				if (pct > 0)
					pct--;

				sprintf( szcharge,"!HEV_%1dP", pct );
				pPlayer->SetSuitUpdate(szcharge, FALSE, SUIT_NEXT_IN_30SEC);
			}
			return TRUE;		
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);



//======================================//
// item_longjump - HEV Long jump module
//======================================//
class CItemLongJump : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_longjump.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_longjump.mdl");
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->m_fLongJump )
			return FALSE;

		// Fograin92: Only when we have HEV suit and it's HL1 game
		if ( (pPlayer->pev->weapons & (1<<WEAPON_SUIT)) && (CVAR_GET_FLOAT("sm_hud") == 0.0) )
		{
			pPlayer->m_fLongJump = TRUE;// player now has longjump module
			g_engfuncs.pfnSetPhysicsKeyValue( pPlayer->edict(), "slj", "1" );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			EMIT_SOUND_SUIT( pPlayer->edict(), "!HEV_A1" );

			return TRUE;		
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );



//=======================================//
// item_healthkit - Simple First Aid Kit
//=======================================//
class CHealthKit : public CItem
{
	void CHealthKit :: Spawn( void )
	{
		Precache( );
		SET_MODEL(ENT(pev), "models/w_medkit.mdl");
		CItem::Spawn();
	}

	void CHealthKit::Precache( void )
	{
		PRECACHE_MODEL("models/w_medkit.mdl");
		PRECACHE_SOUND("items/smallmedkit1.wav");
	}

	BOOL CHealthKit::MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
			return FALSE;

		if ( pPlayer->TakeHealth( gSkillData.healthkitCapacity, DMG_GENERIC ) )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM);
			UTIL_Remove(this);

			return TRUE;
		}

		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( item_healthkit, CHealthKit );



//=====================================//
// item_helmet - Security Guard Helmet
//=====================================//
class CItemBsHelmet : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/barney_helmet.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/barney_helmet.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
			return FALSE;

		// Fograin92: If current armor value is lower than max AND if it's Blue Shift
		if ((pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY) && (CVAR_GET_FLOAT("sm_hud") == 1.0 ))
		{
			pPlayer->pev->armorvalue += gSkillData.fSkill_bsHelmet;
			pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY);

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			return TRUE;	
		}

		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_helmet, CItemBsHelmet);


//=====================================//
// item_helmet - Security Guard Helmet
//=====================================//
class CItemBsVest : public CItem
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/barney_vest.mdl");
		CItem::Spawn( );
	}

	void Precache( void )
	{
		PRECACHE_MODEL ("models/barney_vest.mdl");
		PRECACHE_SOUND( "items/gunpickup2.wav" );
	}

	BOOL MyTouch( CBasePlayer *pPlayer )
	{
		if ( pPlayer->pev->deadflag != DEAD_NO )
			return FALSE;

		// Fograin92: If current armor value is lower than max AND if it's Blue Shift
		if ((pPlayer->pev->armorvalue < MAX_NORMAL_BATTERY) && (CVAR_GET_FLOAT("sm_hud") == 1.0 ))
		{
			pPlayer->pev->armorvalue += gSkillData.fSkill_bsVest;
			pPlayer->pev->armorvalue = min(pPlayer->pev->armorvalue, MAX_NORMAL_BATTERY);

			EMIT_SOUND( pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );

			MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
				WRITE_STRING( STRING(pev->classname) );
			MESSAGE_END();

			return TRUE;	
		}

		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS(item_armorvest, CItemBsVest);