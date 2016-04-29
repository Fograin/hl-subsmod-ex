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
//=========================================================
// skill.h - skill level concerns
//=========================================================
#ifndef SKILL_H
#define SKILL_H

struct skilldata_t
{

	int iSkillLevel; // game skill level

// Monster Health & Damage
	float	agruntHealth;
	float agruntDmgPunch;

	float apacheHealth;
	
	float barneyHealth;

	float bigmommaHealthFactor;		// Multiply each node's health by this
	float bigmommaDmgSlash;			// melee attack damage
	float bigmommaDmgBlast;			// mortar attack damage
	float bigmommaRadiusBlast;		// mortar attack radius

	float bullsquidHealth;
	float bullsquidDmgBite;
	float bullsquidDmgWhip;
	float bullsquidDmgSpit;

	float gargantuaHealth;
	float gargantuaDmgSlash;
	float gargantuaDmgFire;
	float gargantuaDmgStomp;

	float hassassinHealth;

	float headcrabHealth;
	float headcrabDmgBite;

	float hgruntHealth;
	float hgruntDmgKick;
	float hgruntShotgunPellets;
	float hgruntGrenadeSpeed;

	float houndeyeHealth;
	float houndeyeDmgBlast;

	float slaveHealth;
	float slaveDmgClaw;
	float slaveDmgClawrake;
	float slaveDmgZap;

	float ichthyosaurHealth;
	float ichthyosaurDmgShake;

	float leechHealth;
	float leechDmgBite;

	float controllerHealth;
	float controllerDmgZap;
	float controllerSpeedBall;
	float controllerDmgBall;

	float nihilanthHealth;
	float nihilanthZap;

	float scientistHealth;

	float snarkHealth;
	float snarkDmgBite;
	float snarkDmgPop;

	float zombieHealth;
	float zombieDmgOneSlash;
	float zombieDmgBothSlash;

	float turretHealth;
	float miniturretHealth;
	float sentryHealth;


// Player Weapons
	float plrDmgCrowbar;
	float plrDmg9MM;
	float plrDmg357;
	float plrDmgMP5;
	float plrDmgM203Grenade;
	float plrDmgBuckshot;
	float plrDmgCrossbowClient;
	float plrDmgCrossbowMonster;
	float plrDmgRPG;
	float plrDmgGauss;
	float plrDmgEgonNarrow;
	float plrDmgEgonWide;
	float plrDmgHornet;
	float plrDmgHandGrenade;
	float plrDmgSatchel;
	float plrDmgTripmine;
	
// weapons shared by monsters
	float monDmg9MM;
	float monDmgMP5;
	float monDmg12MM;
	float monDmgHornet;

// health/suit charge
	float suitchargerCapacity;
	float batteryCapacity;
	float healthchargerCapacity;
	float healthkitCapacity;
	float scientistHeal;

// monster damage adj
	float monHead;
	float monChest;
	float monStomach;
	float monLeg;
	float monArm;

// player damage adj
	float plrHead;
	float plrChest;
	float plrStomach;
	float plrLeg;
	float plrArm;

// Fograin92: Blue Shift variables
	float fSkill_bsHelmet;
	float fSkill_bsVest;

// Fograin92: Opposing Force variables
	float ammo_556;
	float ammo_762;
	float sk_plr_eagle;
	float sk_plr_spore;
	float sk_otis_health;
	float sk_pitdrone_health;
	float sk_pitdrone_dmg_bite;
	float sk_pitdrone_dmg_whip;
	float sk_pitdrone_dmg_spit;
	float sk_hgrunt_ally_health;
	float sk_hgrunt_ally_kick;
	float sk_hgrunt_ally_pellets;
	float sk_hgrunt_ally_gspeed;
	float sk_medic_ally_health;
	float sk_medic_ally_kick;
	float sk_medic_ally_gspeed;
	float sk_medic_ally_heal;
	float sk_torch_ally_health;
	float sk_torch_ally_kick;
	float sk_torch_ally_gspeed;
	float sk_massassin_health;
	float sk_massassin_kick;
	float sk_massassin_gspeed;
	float sk_shocktrooper_health;
	float sk_shocktrooper_kick;
	float sk_shocktrooper_gspeed;
	float sk_shocktrooper_maxcharge;
	float sk_shocktrooper_rchgspeed;
	float sk_cleansuit_scientist_health;
	float sk_cleansuit_scientist_heal;
	float sk_voltigore_health;
	float sk_voltigore_dmg_punch;
	float sk_voltigore_dmg_beam;
	float sk_babyvoltigore_health;
	float sk_babyvoltigore_dmg_punch;
	float sk_pitworm_health;
	float sk_pitworm_dmg_swipe;
	float sk_pitworm_dmg_beam;
	float sk_geneworm_health;
	float sk_geneworm_dmg_spit;
	float sk_geneworm_dmg_hit;
	float sk_zombie_barney_health;
	float sk_zombie_barney_dmg_one_slash;
	float sk_zombie_barney_dmg_both_slash;
	float sk_zombie_soldier_health;
	float sk_zombie_soldier_dmg_one_slash;
	float sk_zombie_soldier_dmg_both_slash;
	float sk_gonome_health;
	float sk_gonome_dmg_one_slash;
	float sk_gonome_dmg_guts;
	float sk_gonome_dmg_one_bite;
	float sk_shockroach_health;
	float sk_shockroach_dmg_bite;
	float sk_shockroach_lifespan;
	float sk_plr_pipewrench;
	float sk_plr_knife;
	float sk_plr_grapple;
	float sk_plr_displacer_self;
	float sk_plr_displacer_other;
	float sk_plr_displacer_radius;
	float sk_plr_shockroachs;
	float sk_plr_shockroachm;
};

extern	DLL_GLOBAL	skilldata_t	gSkillData;
float GetSkillCvar( char *pName );

extern DLL_GLOBAL int		g_iSkillLevel;

#define SKILL_EASY		1
#define SKILL_MEDIUM	2
#define SKILL_HARD		3

#endif //SKILL_H