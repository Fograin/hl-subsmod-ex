//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	Written by: Vit_amiN
//  Edits by: Fograin92
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#ifndef _SUBS_MOD_GAMESPEC_H
#define _SUBS_MOD_GAMESPEC_H

#include "sm_structs.h"
#include "sm_defines.h"


// Fograin92: English subtitles
// Load shared .txt files and dialogue subtitles
static const char * const pCustomTitleFilesArray[] = 
{
	"scripts/english_text_base.txt",
	"scripts/english_text_subs.txt",
};

// Load shared .txt files, dialogue subtitles AND closed captions
static const char * const pCustomTitleFilesArrayCC[] = 
{
	"scripts/english_text_base.txt",
	"scripts/english_text_subs.txt",
	"scripts/english_text_CC.txt",
};


//================================//
// Fograin92: Localized subtitles
//================================//

// Fograin92: Polish locale
static const char * const pCustomTitleFilesArray_PL[] = 
{
	"scripts/polish_text_base.txt",
	"scripts/polish_text_subs.txt",
};

static const char * const pCustomTitleFilesArrayCC_PL[] = 
{
	"scripts/polish_text_base.txt",
	"scripts/polish_text_subs.txt",
	"scripts/polish_text_CC.txt",
};

// Fograin92: Russian locale
static const char * const pCustomTitleFilesArray_RU[] = 
{
	"scripts/russian_text_base.txt",
	"scripts/russian_text_subs.txt",
};

static const char * const pCustomTitleFilesArrayCC_RU[] = 
{
	"scripts/russian_text_base.txt",
	"scripts/russian_text_subs.txt",
	"scripts/russian_text_CC.txt",
};











// Fix sentences, Half-Life Barney
static const client_textmessage_sentence_t pHLBarneySounds[] =
{
	{ "/ba_attack1.wav",		"!BA_ATTACK"	},
	{ "/ba_die1.wav",			"!BA_DIE"		},
	{ "/ba_die2.wav",			"!BA_DIE"		},
	{ "/ba_die3.wav",			"!BA_DIE"		},
	{ "/ba_pain1.wav",			"!BA_PAIN"		},
	{ "/ba_pain2.wav",			"!BA_PAIN"		},
	{ "/ba_pain3.wav",			"!BA_PAIN"		},
	{ "/c1a4_ba_octo3.wav",		"!BA_OCTO3"		},
	{ "/c2a4_ba_1tau.wav",		"!BA_TAU1A"		},
	{ "/c2a4_ba_3tau.wav",		"!BA_TAU3A"		},
	{ "/c2a4_ba_5tau.wav",		"!BA_TAU5A"		},
	{ "/c2a4_ba_steril.wav",	"!BA_STERIL"	},
	{ "/heybuddy.wav",			"!BA_HELLO5"	},
};


// Fix sentences, Half-Life GMan
static const client_textmessage_sentence_t pHLGManSounds[] =
{
	{ "/gman_noreg.wav",	"!GM_REGRET"	},
	{ "/gman_wise.wav",		"!GM_WISE"		},
};


// Fix sentences, Half-Life Human Grunts
static const client_textmessage_sentence_t pHLHGruntSounds[] =
{
	{ "/gr_die1.wav",			"!HG_DIE"			},
	{ "/gr_die2.wav",			"!HG_DIE"			},
	{ "/gr_die3.wav",			"!HG_DIE"			},
	{ "/gr_pain1.wav",			"!HG_PAIN"			},
	{ "/gr_pain2.wav",			"!HG_PAIN"			},
	{ "/gr_pain3.wav",			"!HG_PAIN"			},
	{ "/gr_pain4.wav",			"!HG_PAIN"			},
	{ "/gr_pain5.wav",			"!HG_PAIN"			},
	{ "/c2a3_ambush_fx.wav",	"!HG_AMBUSH_FX"		},
	{ "/c2a3_ambush_vox.wav",	"!HG_AMBUSH"		},
	{ "/c2a5_hg_abandon.wav",	"!HG_ABANDON"		},
	{ "/c2a5_hg_lebuz.wav",		"!HG_LEBUZ"			},
	{ "/hg_canal1.wav",			"!HG_CANAL1"		},
	{ "/hg_canal2.wav",			"!HG_CANAL2"		},
	{ "/hg_canal3.wav",			"!HG_CANAL3"		},
	{ "/hg_got_barney.wav",		"!HG_GOT_BARNEY"	},
};


// Fix sentences, Half-Life Gina Hologram
static const client_textmessage_sentence_t pHLHoloSounds[] =
{
	{ "/tr_holo_breath.wav",		"!HOLO_BREATH"		},
	{ "/tr_holo_done.wav",			"!HOLO_DONE"		},
	{ "/tr_holo_fallshort.wav",		"!HOLO_FALLSHORT"	},
	{ "/tr_holo_fantastic.wav",		"!HOLO_FANTASTIC"	},
	{ "/tr_holo_flashlight.wav",	"!HOLO_FLASHLIGHT"	},
	{ "/tr_holo_greatwork.wav",		"!HOLO_GREATWORK"	},
	{ "/tr_holo_jumpdown.wav",		"!HOLO_JUMPDOWN"	},
	{ "/tr_holo_keeptrying.wav",	"!HOLO_KEEPTRYING"	},
	{ "/tr_holo_lightoff.wav",		"!HOLO_LIGHTOFF"	},
	{ "/tr_holo_nicejob.wav",		"!HOLO_NICEJOB"		},
	{ "/tr_holo_radiation.wav",		"!HOLO_RADIATION"	},
};


// Fix sentences, Half-Life Nihilanth
static const client_textmessage_sentence_t pHLNihilanthSounds[] =
{
	{ "/nil_alone.wav",			"!NIL_ALONE"	},
	{ "/nil_comes.wav",			"!NIL_COMES"	},
	{ "/nil_deceive.wav",		"!NIL_DECEIVE"	},
	{ "/nil_die.wav",			"!NIL_DIE"		},
	{ "/nil_done.wav",			"!NIL_DONE"		},
	{ "/nil_freeman.wav",		"!NIL_FREEMAN"	},
	{ "/nil_last.wav",			"!NIL_LAST"		},
	{ "/nil_man_notman.wav",	"!NIL_NOTMAN"	},
	{ "/nil_now_die.wav",		"!NIL_NOWDIE"	},
	{ "/nil_slaves.wav",		"!NIL_SLAVES"	},
	{ "/nil_thelast.wav",		"!NIL_THELAST"	},
	{ "/nil_thetruth.wav",		"!NIL_THETRUTH"	},
	{ "/nil_thieves.wav",		"!NIL_THIEVES"	},
	{ "/nil_win.wav",			"!NIL_WIN"		},
};


// Fix sentences, Half-Life Scientist
static const client_textmessage_sentence_t pHLScientistSounds[] =
{
	{ "/sci_dragoff.wav",			"!SC_DRAGOFF"	},
	{ "/sci_fear4.wav",				"!SC_SCREAM3"	},
	{ "/sci_fear7.wav",				"!SC_SCREAM6"	},
	{ "/sci_pain1.wav",				"!SC_PAIN"		},
	{ "/sci_pain2.wav",				"!SC_PAIN"		},
	{ "/sci_pain3.wav",				"!SC_PAIN"		},
	{ "/sci_pain4.wav",				"!SC_PAIN"		},
	{ "/sci_pain5.wav",				"!SC_PAIN"		},
	{ "/scream01.wav",				"!SC_SCREAM"	},
	{ "/scream1.wav",				"!SC_SCREAM"	},
	{ "/scream02.wav",				"!SC_SCREAM"	},
	{ "/scream2.wav",				"!SC_SCREAM"	},
	{ "/scream3.wav",				"!SC_SCREAM"	},
	{ "/scream4.wav",				"!SC_SCREAM"	},
	{ "/scream05.wav",				"!SC_SCREAM"	},
	{ "/scream07.wav",				"!SC_SCREAM"	},
	{ "/scream10.wav",				"!SC_SCREAM"	},
	{ "/scream14.wav",				"!SC_SCREAM"	},
	{ "/scream15.wav",				"!SC_SCREAM"	},
	{ "/scream25.wav",				"!SC_SCREAM"	},
	{ "/weartie.wav",				"!SC_PIDLE1"	},
	{ "/c1a0_sci_bigday.wav",		"!SC_BIGDAY"	},
	{ "/c1a0_sci_catscream.wav",	"!SC_CATSCREAM"	},
	{ "/c1a0_sci_crit3a.wav",		"!SC_CRIT3A"	},
	{ "/c1a0_sci_dis14a.wav",		"!SC_DIS14A"	},
	{ "/c1a0_sci_stall.wav",		"!SC_STALL"		},
	{ "/c1a0_sci_stayback.wav",		"!SC_STAYBACK"	},
	{ "/c1a1_sci_2scan.wav",		"!SC_CONSOLE2A"	},
	{ "/c1a2_sci_dangling.wav",		"!SC_DANGLING"	},
	{ "/c1a2_sci_darkroom.wav",		"!SC_DARKROOM"	},
	{ "/c1a3_sci_thankgod.wav",		"!SC_THANKGOD"	},
	{ "/c2a4_sci_2tau.wav",			"!SC_TAU2A"		},
	{ "/c2a4_sci_4tau.wav",			"!SC_TAU4A"		},
	{ "/c1a4_sci_rocket.wav",		"!SC_ROCKET"	},
	{ "/c1a4_sci_tent.wav",			"!SC_TENT"		},
};


// Fix sentences, Half-Life Train Ride
static const client_textmessage_sentence_t pHLTRideSounds[] =
{
	{ "/c0a0_tr_arrive.wav",	"!TR_ARRIVE"	},
	{ "/c0a0_tr_dest.wav",		"!TR_DEST"		},
	{ "/c0a0_tr_emerg.wav",		"!TR_EMERG"		},
	{ "/c0a0_tr_exit.wav",		"!TR_EXIT"		},
	{ "/c0a0_tr_gmorn.wav",		"!TR_GMORN"		},
	{ "/c0a0_tr_haz.wav",		"!TR_HAZ"		},
	{ "/c0a0_tr_jobs.wav",		"!TR_JOBS"		},
	{ "/c0a0_tr_noeat.wav",		"!TR_NOEAT"		},
	{ "/c0a0_tr_time.wav",		"!TR_TIME"		},
	{ "/c0a0_tr_tourn.wav",		"!TR_TOURN"		},
};


// Fix sentences, Blue-Shift Train Ride
static const client_textmessage_sentence_t pBSTramSounds[] =
{
	{ "/ba_tram0.wav",		"!BA_TRAM0"		},
	{ "/ba_tram1.wav",		"!BA_TRAM1"		},
	{ "/ba_tram2.wav",		"!BA_TRAM2"		},
	{ "/ba_tram3.wav",		"!BA_TRAM3"		},
	{ "/ba_tram4.wav",		"!BA_TRAM4"		},
	{ "/ba_tram5.wav",		"!BA_TRAM5"		},
	{ "/ba_tram6.wav",		"!BA_TRAM6"		},
	{ "/ba_tram7.wav",		"!BA_TRAM7"		},
	{ "/ba_tram8.wav",		"!BA_TRAM8"		},
};

// Fix sentences, Blue-Shift Hazard Course hologram
static const client_textmessage_sentence_t pBSHoloSounds[] =
{
	{ "/tr_holo_breath.wav",		"!BS_BREATH"		},
	{ "/tr_holo_done.wav",			"!BS_DONE"			},
	{ "/tr_holo_fallshort.wav",		"!BS_FALLSHORT"		},
	{ "/tr_holo_fantastic.wav",		"!BS_FANTASTIC"		},
	{ "/tr_holo_flashlight.wav",	"!BS_FLASHLIGHT"	},
	{ "/tr_holo_greatwork.wav",		"!BS_GREATWORK"		},
	{ "/tr_holo_jumpdown.wav",		"!BS_JUMPDOWN"		},
	{ "/tr_holo_keeptrying.wav",	"!BS_KEEPTRYING"	},
	{ "/tr_holo_lightoff.wav",		"!BS_LIGHTOFF"		},
	{ "/tr_holo_nicejob.wav",		"!BS_NICEJOB"		},
	{ "/tr_holo_radiation.wav",		"!BS_RADIATION"		},
};

// Fograin92: Fix sentences, Blue-Shift Rosenberg
static const client_textmessage_sentence_t pBSRosenbergSounds[] =
{
	{ "/ro_xen_call1.wav",		"!RO_XEN_CALL1"	},
	{ "/ro_xen_call2.wav",		"!RO_XEN_CALL2"	},
	{ "/ro_xen_call3.wav",		"!RO_XEN_CALL3"	},
};


// Fograin92: Closed captions
static const client_textmessage_sentence_t pCCSounds[] =
{
	// Weapons
	{ "/357_cock1.wav",			"WPN_EMPTY"				},
	{ "/dryfire1.wav",			"WPN_EMPTY"				},

	{ "/357_reload1.wav",		"WPN_RELOAD"			},
	{ "/dryfire1.wav",			"WPN_RELOAD"			},
	{ "/reload1.wav",			"WPN_RELOAD"			},
	{ "/reload2.wav",			"WPN_RELOAD"			},
	{ "/reload3.wav",			"WPN_RELOAD"			},
	{ "/xbow_reload1.wav",		"WPN_RELOAD"			},
	{ "/cliprelease1.wav",		"WPN_RELOAD"			},

	{ "/cbar_hit1.wav",			"WPN_CBAR_HITWRLD"		},
	{ "/cbar_hit2.wav",			"WPN_CBAR_HITWRLD"		},
	{ "/cbar_hitbod1.wav",		"WPN_CBAR_HITFLESH"		},
	{ "/cbar_hitbod2.wav",		"WPN_CBAR_HITFLESH"		},
	{ "/cbar_hitbod3.wav",		"WPN_CBAR_HITFLESH"		},
	{ "/cbar_miss1.wav",		"WPN_CBAR_MISS"			},

	{ "/pl_gun3.wav",			"WPN_PISTOL_SHOT"		},

	{ "/357_shot1.wav",			"WPN_357_SHOT"			},
	{ "/357_shot2.wav",			"WPN_357_SHOT"			},

	{ "/hks1.wav",				"WPN_MP5_SHOT"			},
	{ "/hks2.wav",				"WPN_MP5_SHOT"			},
	{ "/hks3.wav",				"WPN_MP5_SHOT"			},
	{ "/glauncher.wav",			"WPN_MP5_SHOT2"			},
	{ "/glauncher2.wav",		"WPN_MP5_SHOT2"			},

	{ "/sbarrel1.wav",			"WPN_SHOTGUN_SHOT"		},
	{ "/dbarrel1.wav",			"WPN_SHOTGUN_SHOT2"		},

	{ "/xbow_fire1.wav",		"WPN_XBOW_SHOT"			},
	{ "/rocketfire1.wav",		"WPN_RPG_SHOT"			},
	{ "/gauss2.wav",			"WPN_GAUSS_SHOT"		},
	{ "/egon_windup2.wav",		"WPN_EGON_SHOT"			},

	{ "/ag_fire1.wav",			"WPN_HIVE_SHOT"			},
	{ "/ag_fire2.wav",			"WPN_HIVE_SHOT"			},
	{ "/ag_fire3.wav",			"WPN_HIVE_SHOT"			},

	{ "/mine_deploy.wav",		"WPN_TM_DEPLOY"			},
	{ "/mine_activate.wav",		"WPN_TM_ACTIVATE"		},


	// HUD and Player actions
	{ "/breathe1.wav",			"PLR_BREATH"			},
	{ "/breathe2.wav",			"PLR_BREATH"			},

	{ "/geiger1.wav",			"PLR_GEIGER"			},
	{ "/geiger2.wav",			"PLR_GEIGER"			},
	{ "/geiger3.wav",			"PLR_GEIGER"			},
	{ "/geiger4.wav",			"PLR_GEIGER"			},
	{ "/geiger5.wav",			"PLR_GEIGER"			},
	{ "/geiger6.wav",			"PLR_GEIGER"			},

	{ "/heartbeat1.wav",		"PLR_HEARTHBEAT"		},

	{ "/9mmclip1.wav",			"PLR_AMMO_PICKUP"		},
	{ "/9mmclip2.wav",			"PLR_AMMO_PICKUP"		},
	{ "/ammopickup1.wav",		"PLR_AMMO_PICKUP"		},
	{ "/ammopickup2.wav",		"PLR_AMMO_PICKUP"		},

	{ "/flashlight1.wav",		"PLR_FLASHLIGHT"		},

	{ "/medshot4.wav",			"PLR_HK_START"			},
	{ "/medshotno1.wav",		"PLR_HK_NO"				},
	{ "/smallmedkit1.wav",		"PLR_HK_SMALL"			},
	{ "/smallmedkit2.wav",		"PLR_HK_SMALL"			},

	{ "/suitchargeok1.wav",		"PLR_HEVC_START"		},
	{ "/suitchargeno1.wav",		"PLR_HEVC_NO"			},

	{ "/pcv_vest.wav",			"PLR_PCV_ON"			},

	{ "/wpn_denyselect.wav",	"PLR_USE_NO"			},



	// NPCS
	{ "/ag_alert1.wav",			"NPC_AGRUNT_ALERT"		},
	{ "/ag_alert2.wav",			"NPC_AGRUNT_ALERT"		},
	{ "/ag_alert3.wav",			"NPC_AGRUNT_ALERT"		},
	{ "/ag_alert4.wav",			"NPC_AGRUNT_ALERT"		},
	{ "/ag_alert5.wav",			"NPC_AGRUNT_ALERT"		},
	{ "/ag_attack1.wav",		"NPC_AGRUNT_ALERT"		},
	{ "/ag_attack2.wav",		"NPC_AGRUNT_ALERT"		},
	{ "/ag_attack3.wav",		"NPC_AGRUNT_ALERT"		},
	{ "/ag_die1.wav",			"NPC_AGRUNT_DEATH"		},
	{ "/ag_die2.wav",			"NPC_AGRUNT_DEATH"		},
	{ "/ag_die3.wav",			"NPC_AGRUNT_DEATH"		},
	{ "/ag_die4.wav",			"NPC_AGRUNT_DEATH"		},
	{ "/ag_die5.wav",			"NPC_AGRUNT_DEATH"		},

	{ "/slv_alert1.wav",		"NPC_ASLAVE_ALERT"		},
	{ "/slv_alert3.wav",		"NPC_ASLAVE_ALERT"		},
	{ "/slv_alert4.wav",		"NPC_ASLAVE_ALERT"		},
	{ "/slv_die1.wav",			"NPC_ASLAVE_DEATH"		},
	{ "/slv_die2.wav",			"NPC_ASLAVE_DEATH"		},

	{ "/bcl_alert2.wav",		"NPC_BARNACLE_ALERT"	},
	{ "/bcl_bite3.wav",			"NPC_BARNACLE_BITE"		},
	{ "/bcl_chew1.wav",			"NPC_BARNACLE_CHEW"		},
	{ "/bcl_chew2.wav",			"NPC_BARNACLE_CHEW"		},
	{ "/bcl_chew3.wav",			"NPC_BARNACLE_CHEW"		},
	{ "/bcl_die1.wav",			"NPC_BARNACLE_DIE"		},
	{ "/bcl_die2.wav",			"NPC_BARNACLE_DIE"		},
	{ "/bcl_die3.wav",			"NPC_BARNACLE_DIE"		},

	{ "/ba_die1.wav",			"NPC_BARNEY_DIE"		},
	{ "/ba_die2.wav",			"NPC_BARNEY_DIE"		},
	{ "/ba_die3.wav",			"NPC_BARNEY_DIE"		},

	{ "/boid_alert1.wav",		"NPC_BOID_IDLE"			},
	{ "/boid_alert2.wav",		"NPC_BOID_IDLE"			},
	{ "/boid_idle1.wav",		"NPC_BOID_IDLE"			},
	{ "/boid_idle2.wav",		"NPC_BOID_IDLE"			},
	{ "/boid_idle3.wav",		"NPC_BOID_IDLE"			},

	{ "/bc_attack1.wav",		"NPC_BC_SPIT"			},
	{ "/bc_attack2.wav",		"NPC_BC_SPIT"			},
	{ "/bc_attack3.wav",		"NPC_BC_SPIT"			},
	{ "/bc_attackgrowl.wav",	"NPC_BC_ATTACK"			},
	{ "/bc_attackgrowl2.wav",	"NPC_BC_ATTACK"			},
	{ "/bc_attackgrowl3.wav",	"NPC_BC_ATTACK"			},
	{ "/bc_die1.wav",			"NPC_BC_DEATH"			},
	{ "/bc_die2.wav",			"NPC_BC_DEATH"			},
	{ "/bc_die3.wav",			"NPC_BC_DEATH"			},
	{ "/bc_idle2.wav",			"NPC_BC_ALERT"			},
	{ "/bc_pain1.wav",			"NPC_BC_ALERT"			},

	{ "/con_alert1.wav",		"NPC_CON_ALERT"			},
	{ "/con_alert2.wav",		"NPC_CON_ALERT"			},
	{ "/con_alert3.wav",		"NPC_CON_ALERT"			},
	{ "/con_die1.wav",			"NPC_CON_DEATH"			},
	{ "/con_die2.wav",			"NPC_CON_DEATH"			},

	{ "/gar_alert1.wav",		"NPC_GARG_ALERT"		},
	{ "/gar_alert2.wav",		"NPC_GARG_ALERT"		},
	{ "/gar_alert3.wav",		"NPC_GARG_ALERT"		},
	{ "/gar_die1.wav",			"NPC_GARG_DEATH"		},
	{ "/gar_die2.wav",			"NPC_GARG_DEATH"		},
	{ "/gar_stomp1.wav",		"NPC_GARG_STOMP"		},
	{ "/gar_flameon1.wav",		"NPC_GARG_FLAME"		},
	{ "/gar_breathe1.wav",		"NPC_GARG_BREATH"		},
	{ "/gar_breathe2.wav",		"NPC_GARG_BREATH"		},
	{ "/gar_breathe3.wav",		"NPC_GARG_BREATH"		},

	{ "/gon_alert1.wav",		"NPC_GON_ALERT"			},
	{ "/gon_alert2.wav",		"NPC_GON_ALERT"			},
	{ "/gon_alert3.wav",		"NPC_GON_ALERT"			},
	{ "/gon_die1.wav",			"NPC_GON_DEATH"			},

	{ "/hc_alert1.wav",			"NPC_HC_ALERT"			},
	{ "/hc_alert2.wav",			"NPC_HC_ALERT"			},
	{ "/hc_attack1.wav",		"NPC_HC_JUMP"			},
	{ "/hc_die1.wav",			"NPC_HC_DIE"			},
	{ "/hc_die2.wav",			"NPC_HC_DIE"			},

	{ "/gr_die1.wav",			"NPC_GR_DIE"			},
	{ "/gr_die2.wav",			"NPC_GR_DIE"			},
	{ "/gr_die3.wav",			"NPC_GR_DIE"			},

	{ "/he_alert1.wav",			"NPC_HE_ALERT"			},
	{ "/he_alert2.wav",			"NPC_HE_ALERT"			},
	{ "/he_alert3.wav",			"NPC_HE_ALERT"			},
	{ "/he_attack1.wav",		"NPC_HE_ATTACK"			},
	{ "/he_attack2.wav",		"NPC_HE_ATTACK"			},
	{ "/he_attack3.wav",		"NPC_HE_ATTACK"			},
	{ "/he_die1.wav",			"NPC_HE_DIES"			},
	{ "/he_die2.wav",			"NPC_HE_DIES"			},
	{ "/he_die3.wav",			"NPC_HE_DIES"			},

	{ "/ichy_alert1.wav",		"NPC_ICKY_ALERT"		},
	{ "/ichy_alert2.wav",		"NPC_ICKY_ALERT"		},
	{ "/ichy_alert3.wav",		"NPC_ICKY_ALERT"		},
	{ "/ichy_bite1.wav",		"NPC_ICKY_BITE"			},
	{ "/ichy_bite2.wav",		"NPC_ICKY_BITE"			},
	{ "/ichy_die1.wav",			"NPC_ICKY_DEATH"		},
	{ "/ichy_die2.wav",			"NPC_ICKY_DEATH"		},
	{ "/ichy_die3.wav",			"NPC_ICKY_DEATH"		},
	{ "/ichy_die4.wav",			"NPC_ICKY_DEATH"		},

	{ "/sqk_blast1.wav",		"NPC_SNARK_DEATH"		},
	{ "/sqk_deploy1.wav",		"NPC_SNARK_ALERT"		},
	{ "/sqk_hunt1.wav",			"NPC_SNARK_HUNTS"		},
	{ "/sqk_hunt2.wav",			"NPC_SNARK_HUNTS"		},
	{ "/sqk_hunt3.wav",			"NPC_SNARK_HUNTS"		},

	{ "/te_death2.wav",			"NPC_TE_DEATH"			},
	{ "/te_strike1.wav",		"NPC_TE_STRIKE"			},
	{ "/te_strike2.wav",		"NPC_TE_STRIKE"			},

	{ "/tu_deploy.wav",			"NPC_TU_DEPLOY"			},
	{ "/tu_alert.wav",			"NPC_TU_ALERT"			},
	{ "/tu_die.wav",			"NPC_TU_DEATH"			},
	{ "/tu_die2.wav",			"NPC_TU_DEATH"			},
	{ "/tu_die3.wav",			"NPC_TU_DEATH"			},
	{ "/tu_retract.wav",		"NPC_TU_RETRACT"		},

	{ "/x_die1.wav",			"NPC_NIH_DEATH"			},

	{ "/claw_miss1.wav",		"NPC_ZMB_ATTACK"		},
	{ "/claw_miss2.wav",		"NPC_ZMB_ATTACK"		},
	{ "/claw_strike1.wav",		"NPC_ZMB_ATTACK"		},
	{ "/claw_strike2.wav",		"NPC_ZMB_ATTACK"		},
	{ "/claw_strike3.wav",		"NPC_ZMB_ATTACK"		},
	{ "/zo_alert10.wav",		"NPC_ZMB_ALERT"			},
	{ "/zo_alert20.wav",		"NPC_ZMB_ALERT"			},
	{ "/zo_alert30.wav",		"NPC_ZMB_ALERT"			},
	{ "/zo_attack1.wav",		"NPC_ZMB_ROAR"			},
	{ "/zo_attack2.wav",		"NPC_ZMB_ROAR"			},

	/*
	{ "/death1.wav",			"NPC_FGRUNT_DIES"		},
	{ "/death2.wav",			"NPC_FGRUNT_DIES"		},
	{ "/death3.wav",			"NPC_FGRUNT_DIES"		},
	{ "/death4.wav",			"NPC_FGRUNT_DIES"		},
	{ "/death5.wav",			"NPC_FGRUNT_DIES"		},
	{ "/death6.wav",			"NPC_FGRUNT_DIES"		},
	*/

	{ "/geneworm_death.wav",	"NPC_GENE_DIES"			},
	{ "/geneworm_entry.wav",	"NPC_GENE_ENTRY"		},

	{ "/gonome_death2.wav",		"NPC_GON_DEATH"			},
	{ "/gonome_death3.wav",		"NPC_GON_DEATH"			},
	{ "/gonome_death4.wav",		"NPC_GON_DEATH"			},

	{ "/pit_drone_alert1.wav",	"NPC_PIT_ALERT"			},
	{ "/pit_drone_alert2.wav",	"NPC_PIT_ALERT"			},
	{ "/pit_drone_alert3.wav",	"NPC_PIT_ALERT"			},
	{ "/pit_drone_die1.wav",	"NPC_PIT_DIES"			},
	{ "/pit_drone_die2.wav",	"NPC_PIT_DIES"			},
	{ "/pit_drone_die3.wav",	"NPC_PIT_DIES"			},

	{ "/pit_worm_alert.wav",			"NPC_PITW_ALERT"		},
	{ "/pit_worm_alert(scream).wav",	"NPC_PITW_ALERT"		},
	{ "/pit_worm_death.wav",			"NPC_PITW_DIES"			},

	{ "/shock_angry.wav",	"NPC_SHOCKR_ALERT"			},
	{ "/shock_die.wav",		"NPC_SHOCKR_DIES"			},

	{ "/shock_trooper_attack.wav",	"NPC_SHOCKT_ALERT"	},
	{ "/shock_trooper_die1.wav",	"NPC_SHOCKT_DIES"	},
	{ "/shock_trooper_die2.wav",	"NPC_SHOCKT_DIES"	},
	{ "/shock_trooper_die3.wav",	"NPC_SHOCKT_DIES"	},
	{ "/shock_trooper_die4.wav",	"NPC_SHOCKT_DIES"	},

	{ "/voltigore_alert1.wav",	"NPC_VOLTI_ALERT"	},
	{ "/voltigore_alert2.wav",	"NPC_VOLTI_ALERT"	},
	{ "/voltigore_alert3.wav",	"NPC_VOLTI_ALERT"	},
	{ "/voltigore_die1.wav",	"NPC_VOLTI_DIES"	},
	{ "/voltigore_die2.wav",	"NPC_VOLTI_DIES"	},
	{ "/voltigore_die3.wav",	"NPC_VOLTI_DIES"	},


	// SFX
	{ "/explode3.wav",			"SFX_EXPLOSION"		},
	{ "/explode4.wav",			"SFX_EXPLOSION"		},
	{ "/explode5.wav",			"SFX_EXPLOSION"		},
	{ "/mortar.wav",			"SFX_MORTAR"		},
	{ "/static.wav",			"SFX_STATIC"		},
	{ "/mgun_burst1.wav",		"SFX_DISF"			},
	{ "/mgun_burst2.wav",		"SFX_DISF"			},
	{ "/mgun_burst3.wav",		"SFX_DISF"			},
	{ "/mgun_burst4.wav",		"SFX_DISF"			},
	{ "/train_use1.wav",		"SFX_TRAIN_USE"		},
	{ "/ttrain_start1.wav",		"SFX_TRAIN_START"	},
	{ "/ttrain_brake1.wav",		"SFX_TRAIN_BRAKES"	},
	{ "/button1.wav",			"SFX_BTN_USE"	},
	{ "/button3.wav",			"SFX_BTN_USE"	},
	{ "/button4.wav",			"SFX_BTN_USE"	},
	{ "/button5.wav",			"SFX_BTN_USE"	},
	{ "/button6.wav",			"SFX_BTN_USE"	},
	{ "/button7.wav",			"SFX_BTN_USE"	},
	{ "/button8.wav",			"SFX_BTN_USE"	},
	{ "/button9.wav",			"SFX_BTN_USE"	},
	{ "/button10.wav",			"SFX_BTN_USE"	},
	{ "/button2.wav",			"SFX_BTN_USEN"	},
	{ "/button11.wav",			"SFX_BTN_USEN"	},
	{ "/latchlocked1.wav",		"SFX_LOCKED"	},
	{ "/latchlocked2.wav",		"SFX_LOCKED"	},
	{ "/lever1.wav",			"SFX_LEVER"		},
	{ "/lever2.wav",			"SFX_LEVER"		},
	{ "/lever3.wav",			"SFX_LEVER"		},
	{ "/lever4.wav",			"SFX_LEVER"		},
	{ "/lever5.wav",			"SFX_LEVER"		},
	{ "/bell1.wav",				"SFX_BELL"		},
	{ "/elevbell1.wav",			"SFX_EBELL"		},
	{ "/warn1.wav",				"SFX_ALARM"		},
	{ "/warn2.wav",				"SFX_ALARM"		},
	{ "/warn3.wav",				"SFX_ALARM"		},

};




// Fix sentences
static const client_textmessage_sndgroup_t pSoundGroups[] = 
{
    { "agrunt",				strlen("agrunt"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "ambience",			strlen("ambience"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "apache",				strlen("apache"),			NULL,					0								},
    { "aslave",				strlen("aslave"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "barnacle",			strlen("barnacle"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "barney",				strlen("barney"),			pHLBarneySounds,		ARRAYSIZE(pHLBarneySounds)		},
    { "boid",				strlen("boid"),				pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "bullchicken",		strlen("bullchicken"),		pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "buttons",			strlen("buttons"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "common",				strlen("common"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "controller",			strlen("controller"),		pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "debris",				strlen("debris"),			NULL,					0								},
    { "doors",				strlen("doors"),			NULL,					0								},
    { "fans",				strlen("fans"),				NULL,					0								},
    { "fvox",				strlen("fvox"),				NULL,					0								},
    { "garg",				strlen("garg"),				pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "gman",				strlen("gman"),				pHLGManSounds,			ARRAYSIZE(pHLGManSounds)		},
    { "gonarch",			strlen("gonarch"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "hassault",			strlen("hassault"),			NULL,					0								},
    { "headcrab",			strlen("headcrab"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "hgrunt",				strlen("hgrunt"),			pHLHGruntSounds,		ARRAYSIZE(pHLHGruntSounds)		},
    { "holo",				strlen("holo"),				pHLHoloSounds,			ARRAYSIZE(pHLHoloSounds)		},
    { "hornet",				strlen("hornet"),			NULL,					0								},
    { "houndeye",			strlen("houndeye"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "ichy",				strlen("ichy"),				pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "items",				strlen("items"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "leech",				strlen("leech"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "misc",				strlen("misc"),				NULL,					0								},
    { "nihilanth",			strlen("nihilanth"),		pHLNihilanthSounds,		ARRAYSIZE(pHLNihilanthSounds)	},
    { "plats",				strlen("plats"),			NULL,					0								},
    { "player",				strlen("player"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "roach",				strlen("roach"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "scientist",			strlen("scientist"),		pHLScientistSounds,		ARRAYSIZE(pHLScientistSounds)	},
    { "squeek",				strlen("squeek"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "tentacle",			strlen("tentacle"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "tride",				strlen("tride"),			pHLTRideSounds,			ARRAYSIZE(pHLTRideSounds)		},
    { "turret",				strlen("turret"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "vox",				strlen("vox"),				NULL,					0								},
    { "weapons",			strlen("weapons"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "x",					strlen("x"),				pCCSounds,				ARRAYSIZE(pCCSounds)			},
    { "zombie",				strlen("zombie"),			pCCSounds,				ARRAYSIZE(pCCSounds)			},

	// Blue Shift
	{ "tram",				strlen("tram"),				pBSTramSounds,			ARRAYSIZE(pBSTramSounds)		},
	{ "ba_holo",			strlen("ba_holo"),			pBSHoloSounds,			ARRAYSIZE(pBSHoloSounds)		},
	{ "rosenberg",			strlen("rosenberg"),		pBSRosenbergSounds,		ARRAYSIZE(pBSRosenbergSounds)	},

	// Opposing Force

};



// Fograin92: Blue Shift map list
static const char * const pBSmaps[] = 
{
	"ba_canal1",
	"ba_canal1b",
	"ba_canal2",
	"ba_canal3",
	"ba_elevator",
	"ba_hazard1",
	"ba_hazard2",
	"ba_hazard3",
	"ba_hazard4",
	"ba_hazard5",
	"ba_hazard6",
	"ba_maint",
	"ba_outro",
	"ba_power1",
	"ba_power2",
	"ba_security1",
	"ba_security2",
	"ba_teleport1",
	"ba_teleport2",
	"ba_tram1",
	"ba_tram2",
	"ba_tram3",
	"ba_xen1",
	"ba_xen2",
	"ba_xen3",
	"ba_xen4",
	"ba_xen5",
	"ba_xen6",
	"ba_yard1",
	"ba_yard2",
	"ba_yard3",
	"ba_yard3a",
	"ba_yard3b",
	"ba_yard4",
	"ba_yard4a",
	"ba_yard5",
	"ba_yard5a",
};


// Fograin92: Opposing Force map list
static const char * const pOFmaps[] = 
{
	"of0a0",
	"of1a1",
	"of1a2",
	"of1a3",
	"of1a4",
	"of1a4b",
	"of1a5",
	"of1a5b",
	"of1a6",
	"of2a1",
	"of2a1b",
	"of2a2",
	"of2a3",
	"of2a4",
	"of2a5",
	"of2a6",
	"of3a1",
	"of3a1b",
	"of3a2",
	"of3a4",
	"of3a5",
	"of3a6",
	"of4a1",
	"of4a2",
	"of4a3",
	"of4a4",
	"of4a5",
	"of5a1",
	"of5a2",
	"of5a3",
	"of5a4",
	"of6a1",
	"of6a2",
	"of6a3",
	"of6a4",
	"of6a4b",
	"of6a5",
	"of7a0",
	"ofboot0",
	"ofboot1",
	"ofboot2",
	"ofboot3",
	"ofboot4",
};


#endif // _SUBS_MOD_GAMESPEC_H
