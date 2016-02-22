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
//
// Entity classes exported by Halflife.
////////////////////////////////////////////

#include "extdll.h"
#include "util.h"

#include "cbase.h"

// Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
server_physics_api_t g_physfuncs;



// Half-Life: Update MOD [START]

typedef void (__cdecl *CLGETMDL)(int, void **); 
CLGETMDL ClientGetModelByIndex;

typedef void (__cdecl *CLGETVIS)(void *); 
CLGETVIS ClientGetVIS;

// Reset engine function
typedef void (__cdecl *CLSNDRES)(void); 
CLSNDRES ClientSoundReset;

// Precache sound function - sample
typedef void (__cdecl *CLSNDPRE)(void *); 
CLSNDPRE ClientSoundPrecache;

// Emit sound function - sample, edict, entindex, volume, atten, flags, pitch
typedef void (__cdecl *CLSNDEMIT)(void *, void *, int, float, float, int, int, int); 
CLSNDEMIT ClientSoundEmit;

// Emit ambient function - sample, edict, entindex, volume, atten, position, flags, pitch
typedef void (__cdecl *CLSNDAMB)(void *, void *, int, float, float, float *, int, int); 
CLSNDAMB ClientSoundAmbient;

// Half-Life: Update MOD [END]



#ifdef _WIN32

// Required DLL entry point
BOOL WINAPI DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved)
{
	if      (fdwReason == DLL_PROCESS_ATTACH)
    {
    }
	else if (fdwReason == DLL_PROCESS_DETACH)
    {
    }
	return TRUE;
}

void DLLEXPORT GiveFnptrsToDll(	enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;


	// Half-Life: Update MOD [START]

	HMODULE hClient = GetModuleHandleA("client.dll");
	if(!hClient)
	{
		MessageBox(NULL, "ERROR: Failed to locate client.dll!\n\nPress Ok to quit the game.\n", "ERROR", MB_OK);
		exit(-1);
	}

	// Reset Sound Engine function
	ClientSoundReset = (CLSNDRES)GetProcAddress(hClient, "CL_SoundEngineReset");
	if(!ClientSoundReset)
	{
		MessageBox(NULL, "ERROR: Failed to link CL_SoundEngineReset!\n\nPress Ok to quit the game.\n", "ERROR", MB_OK);
		exit(-1);
	}

	// Precache Sound function
	ClientSoundPrecache = (CLSNDPRE)GetProcAddress(hClient, "CL_SoundPrecache");
	if(!ClientSoundPrecache)
	{
		MessageBox(NULL, "ERROR: Failed to link CL_SoundPrecache!\n\nPress Ok to quit the game.\n", "ERROR", MB_OK);
		exit(-1);
	}

	// Emit Sound function
	ClientSoundEmit = (CLSNDEMIT)GetProcAddress(hClient, "CL_EmitSound");
	if(!ClientSoundEmit)
	{
		MessageBox(NULL, "ERROR: Failed to link CL_EmitSound!\n\nPress Ok to quit the game.\n", "ERROR", MB_OK);
		exit(-1);
	}

	// Emit Ambient function
	ClientSoundAmbient = (CLSNDAMB)GetProcAddress(hClient, "CL_EmitAmbient");
	if(!ClientSoundAmbient)
	{
		MessageBox(NULL, "ERROR: Failed to link CL_EmitAmbient!\n\nPress Ok to quit the game.\n", "ERROR", MB_OK);
		exit(-1);
	}

	// Half-Life: Update MOD [END]
}


#else

extern "C" 
{
	void GiveFnptrsToDll(	enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
	{
		memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
		gpGlobals = pGlobals;
	}
}

#endif


//============================//
// NEW AUDIO ENGINE FUNCTIONS
// Thanks to: Richard Roháè
//============================//

// Reset engine function
typedef void (__cdecl *CLSNDRES)(void); 
extern CLSNDRES ClientSoundReset;

void EXResetEngine( void )
{
	ClientSoundReset();
};

// Precache sound function - sample
typedef void (__cdecl *CLSNDPRE)(void *); 
extern CLSNDPRE ClientSoundPrecache;

void EXPrecacheSound( char *szFile )
{
	ClientSoundPrecache(szFile);
};

// Emit sound function - sample, edict, entindex, volume, atten, flags, pitch
typedef void (__cdecl *CLSNDEMIT)(void *, void *, int, float, float, int, int, int); 
extern CLSNDEMIT ClientSoundEmit;

void EXEmitSound( edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch )
{
	//ALERT ( at_console, "PASS -> %f\n", volume );
	ClientSoundEmit((void *)entity, (void *)sample, ENTINDEX(entity), volume, attenuation, fFlags, pitch, channel);
};

// Emit ambient function - sample, edict, entindex, volume, atten, position, flags, pitch
typedef void (__cdecl *CLSNDAMB)(void *, void *, int, float, float, float *, int, int); 
extern CLSNDAMB ClientSoundAmbient;

void EXEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	ClientSoundAmbient((void *)entity, (void *)samp, ENTINDEX(entity), vol, attenuation, pos, fFlags, pitch);
};