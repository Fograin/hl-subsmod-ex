// ==============================================
//  Half-Life Subtitles Mod: Game-Specific Stuff
//              written by Vit_amiN
// ==============================================

#ifndef _SUBS_MOD_GAMESPEC_H
#define _SUBS_MOD_GAMESPEC_H

#include "sm_structs.h"
#include "sm_defines.h"

// Custom .txt files to load, shared goes first
static const char * const pCustomTitleFilesArray[] = 
{
	"scripts/sm_text_base.txt",
	"scripts/sm_text_subs.txt",
};

// Sentences for subtitles testing (impulse 98)
static const char * const pPlayerSpeechSentences[] =
{
	"!SC_GMORN",
	"!SC_BIGDAY",
	"!SC_THANKGOD",
	"!SC_BLIND",
	"!SC_TAU4A",
	"!SC_GLUON2A",
	"!SC_FOOL",
	"!SC_ZOMBIE5A",
	"!SC_CRIT1A",
	"!SC_HAZ_WAIT0",
	"!SC_PIDLE1",
	"!SC_PIDLE4",
	"!SC_FEAR7",
	"!SC_FEAR8",
	"!SC_FEAR9",
	"!SC_FEAR10",
	"!SC_FEAR11",
	"!SC_FEAR12",
	"!SC_HEAL3",
	"!SC_HEAL4",
	"!SC_HEAL5",
	"!SC_HEAL6",
	"!SC_HEAL7",
	"!SC_QUESTION24",
	"!SC_QUESTION25",
	"!SC_QUESTION26",
	"!SC_ANSWER27",
	"!SC_ANSWER28",
	"!SC_ANSWER29",
	"!SC_CONSOLE1A",
};

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

static const client_textmessage_sentence_t pHLGManSounds[] =
{
	{ "/gman_noreg.wav",	"!GM_REGRET"	},
	{ "/gman_wise.wav",		"!GM_WISE"		},
};

static const client_textmessage_sentence_t pHLHGruntSounds[] =
{
	{ "/gr_die1.wav",			"!HG_DIE"		},
	{ "/gr_die2.wav",			"!HG_DIE"		},
	{ "/gr_die3.wav",			"!HG_DIE"		},
	{ "/gr_pain1.wav",			"!HG_PAIN"		},
	{ "/gr_pain2.wav",			"!HG_PAIN"		},
	{ "/gr_pain3.wav",			"!HG_PAIN"		},
	{ "/gr_pain4.wav",			"!HG_PAIN"		},
	{ "/gr_pain5.wav",			"!HG_PAIN"		},
	{ "/c2a3_ambush_fx.wav",	"!HG_AMBUSH_FX"	},
	{ "/c2a3_ambush_vox.wav",	"!HG_AMBUSH"	},
	{ "/c2a5_hg_abandon.wav",	"!HG_ABANDON"	},
	{ "/c2a5_hg_lebuz.wav",		"!HG_LEBUZ"		},
};

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

static const client_textmessage_sndgroup_t pSoundGroups[] = 
{
    { "agrunt",			strlen("agrunt"),		NULL,				0								},
    { "ambience",		strlen("ambience"),		NULL,				0								},
    { "apache",			strlen("apache"),		NULL,				0								},
    { "aslave",			strlen("aslave"),		NULL,				0								},
    { "barnacle",		strlen("barnacle"),		NULL,				0								},
    { "barney",			strlen("barney"),		pHLBarneySounds,	ARRAYSIZE(pHLBarneySounds)		},
    { "boid",			strlen("boid"),			NULL,				0								},
    { "bullchicken",	strlen("bullchicken"),	NULL,				0								},
    { "buttons",		strlen("buttons"),		NULL,				0								},
    { "common",			strlen("common"),		NULL,				0								},
    { "controller",		strlen("controller"),	NULL,				0								},
    { "debris",			strlen("debris"),		NULL,				0								},
    { "doors",			strlen("doors"),		NULL,				0								},
    { "fans",			strlen("fans"),			NULL,				0								},
    { "fvox",			strlen("fvox"),			NULL,				0								},
    { "garg",			strlen("garg"),			NULL,				0								},
    { "gman",			strlen("gman"),			pHLGManSounds,		ARRAYSIZE(pHLGManSounds)		},
    { "gonarch",		strlen("gonarch"),		NULL,				0								},
    { "hassault",		strlen("hassault"),		NULL,				0								},
    { "headcrab",		strlen("headcrab"),		NULL,				0								},
    { "hgrunt",			strlen("hgrunt"),		pHLHGruntSounds,	ARRAYSIZE(pHLHGruntSounds)		},
    { "holo",			strlen("holo"),			pHLHoloSounds,		ARRAYSIZE(pHLHoloSounds)		},
    { "hornet",			strlen("hornet"),		NULL,				0								},
    { "houndeye",		strlen("houndeye"),		NULL,				0								},
    { "ichy",			strlen("ichy"),			NULL,				0								},
    { "items",			strlen("items"),		NULL,				0								},
    { "leech",			strlen("leech"),		NULL,				0								},
    { "misc",			strlen("misc"),			NULL,				0								},
    { "nihilanth",		strlen("nihilanth"),	pHLNihilanthSounds,	ARRAYSIZE(pHLNihilanthSounds)	},
    { "plats",			strlen("plats"),		NULL,				0								},
    { "player",			strlen("player"),		NULL,				0								},
    { "roach",			strlen("roach"),		NULL,				0								},
    { "scientist",		strlen("scientist"),	pHLScientistSounds,	ARRAYSIZE(pHLScientistSounds)	},
    { "squeek",			strlen("squeek"),		NULL,				0								},
    { "tentacle",		strlen("tentacle"),		NULL,				0								},
    { "tride",			strlen("tride"),		pHLTRideSounds,		ARRAYSIZE(pHLTRideSounds)		},
    { "turret",			strlen("turret"),		NULL,				0								},
    { "vox",			strlen("vox"),			NULL,				0								},
    { "weapons",		strlen("weapons"),		NULL,				0								},
    { "x",				strlen("x"),			NULL,				0								},
    { "zombie",			strlen("zombie"),		NULL,				0								},
};

#endif // _SUBS_MOD_GAMESPEC_H
