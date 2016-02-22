//===============================================
// Amnesia Sound Engine Class 
//
// Created by Richard Roh·Ë
//
// Thanks Andrew Luke for sentences.txt parser
//		  Hunk Guerrius for beta testing 
//
// Following work is released under LGPL license,
// shipped with this source code.
//
//===============================================

#include "hud.h"
#include "cl_util.h"

#include "const.h"
#include "entity_state.h"

#include "soundengine.h"

extern "C"
{
	#include "pm_shared.h"
}

#include "edict.h"

#pragma warning( disable: 4018 )

Vector g_vecZero(0,0,0);

/*
========================================
WatchThread

Watch Half-Life's window for minimized
and inactive states. If one of these
window styles are used, stop all the
sounds. Values passed in must be thread
safe and their change must be executed
in critical section.
========================================
*/

//Global thread handle
HANDLE g_hThreadHandle;

//Global event signalizing exit of thread
HANDLE g_hExitEvent;

//Global critical section
CRITICAL_SECTION g_CS;

//Global Half-Life's window handle
HWND g_hWnd = 0;

//Pointer structure passed to new thread
typedef struct thread_s
{
	sound_t *pSounds;
} thread_t;

thread_t sThreadData;

FMOD_RESULT (_stdcall *SetPaused) ( FMOD_CHANNEL *channel, FMOD_BOOL paused );

DWORD WINAPI WatchThread( LPVOID lpParam )
{
	thread_t *pThreadData = (thread_t*)lpParam;
	bool bAppStatus, bCleaned;

	HMODULE hFmodDll = GetModuleHandle("fmodex.dll");
	if(!hFmodDll)
	{
		return 0;
	}

	(FARPROC&)SetPaused = GetProcAddress(hFmodDll, "FMOD_Channel_SetPaused");
	if(!SetPaused)
	{
		return 0;
	}

	while(1)
	{
		if(WaitForSingleObject(g_hExitEvent, 0) == WAIT_OBJECT_0)
		{
			ExitThread(0);
			return 0; //let's be sure
		}

		// Fograin92: Search for all possible window names
		g_hWnd = FindWindow(NULL, "Half-Life");
		if(!g_hWnd)
		{
			g_hWnd = FindWindow(NULL, "Half-Life: Update Mod");
			if(!g_hWnd)
			{
				g_hWnd = FindWindow(NULL, "Xash3D Launcher");
				if(!g_hWnd)
				{
					g_hWnd = FindWindow(NULL, "Xash3D Launcher (32 bit)");
					// Fograin92: No active window detected
					if(!g_hWnd)
					{
						MessageBox(NULL, "Can't find any window lol", "Sound engine error!", MB_OK | MB_SETFOREGROUND | MB_ICONERROR);
						return 0;
					}
				}
			}
		}

		LONG nStyle = GetWindowLongPtr(g_hWnd, GWL_STYLE);

		if(!(nStyle & WS_VISIBLE) || (nStyle & WS_MINIMIZE))
		{
			bAppStatus = false;
		}
		else
		{
			bCleaned = false;
			bAppStatus = true;
		}

		if(bAppStatus == false && bCleaned == false)
		{
			EnterCriticalSection(&g_CS);
			for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
			{
				if(pThreadData->pSounds[i].pSound)
				{
					SetPaused(pThreadData->pSounds[i].pChannel, true);
				}
			}
			LeaveCriticalSection(&g_CS);

			bCleaned = true;
		}
	}

	return 0;
}

/*
====================
GetDllPointers

====================
*/
void CSoundEngine::GetDllPointers( void )
{
	char szPath[256];

	strcpy(szPath, gEngfuncs.pfnGetGameDirectory());
	strcat(szPath, "/bin/fmodex.dll");

	m_hFmodDll = LoadLibrary(szPath);
	if(!m_hFmodDll)
		FatalError("Couldn't load %s!\n", szPath);

	(FARPROC&)_FMOD_System_Create = GetProcAddress(m_hFmodDll, "FMOD_System_Create");
	(FARPROC&)_FMOD_System_GetVersion = GetProcAddress(m_hFmodDll, "FMOD_System_GetVersion");
	(FARPROC&)_FMOD_System_GetNumDrivers = GetProcAddress(m_hFmodDll, "FMOD_System_GetNumDrivers");
	(FARPROC&)_FMOD_System_SetOutput = GetProcAddress(m_hFmodDll, "FMOD_System_SetOutput");
	(FARPROC&)_FMOD_System_GetDriverCaps = GetProcAddress(m_hFmodDll, "FMOD_System_GetDriverCaps");
	(FARPROC&)_FMOD_System_SetSpeakerMode = GetProcAddress(m_hFmodDll, "FMOD_System_SetSpeakerMode");
	(FARPROC&)_FMOD_System_SetDSPBufferSize = GetProcAddress(m_hFmodDll, "FMOD_System_SetDSPBufferSize");
	(FARPROC&)_FMOD_System_GetDriverInfo = GetProcAddress(m_hFmodDll, "FMOD_System_GetDriverInfo");
	(FARPROC&)_FMOD_System_SetSoftwareFormat = GetProcAddress(m_hFmodDll, "FMOD_System_SetSoftwareFormat");
	(FARPROC&)_FMOD_System_Init = GetProcAddress(m_hFmodDll, "FMOD_System_Init");
	(FARPROC&)_FMOD_System_Set3DSettings = GetProcAddress(m_hFmodDll, "FMOD_System_Set3DSettings");
	(FARPROC&)_FMOD_System_Release = GetProcAddress(m_hFmodDll, "FMOD_System_Release");
	(FARPROC&)_FMOD_Channel_Stop = GetProcAddress(m_hFmodDll, "FMOD_Channel_Stop");
	(FARPROC&)_FMOD_Sound_Release = GetProcAddress(m_hFmodDll, "FMOD_Sound_Release");
	(FARPROC&)_FMOD_System_SetReverbAmbientProperties = GetProcAddress(m_hFmodDll, "FMOD_System_SetReverbAmbientProperties");
	(FARPROC&)_FMOD_Channel_SetPaused = GetProcAddress(m_hFmodDll, "FMOD_Channel_SetPaused");
	(FARPROC&)_FMOD_System_Update = GetProcAddress(m_hFmodDll, "FMOD_System_Update");
	(FARPROC&)_FMOD_System_Set3DListenerAttributes = GetProcAddress(m_hFmodDll, "FMOD_System_Set3DListenerAttributes");
	(FARPROC&)_FMOD_Channel_IsPlaying = GetProcAddress(m_hFmodDll, "FMOD_Channel_IsPlaying");
	(FARPROC&)_FMOD_Channel_GetPaused = GetProcAddress(m_hFmodDll, "FMOD_Channel_GetPaused");
	(FARPROC&)_FMOD_Channel_Set3DAttributes = GetProcAddress(m_hFmodDll, "FMOD_Channel_Set3DAttributes");
	(FARPROC&)_FMOD_Channel_GetPosition = GetProcAddress(m_hFmodDll, "FMOD_Channel_GetPosition");
	(FARPROC&)_FMOD_Channel_SetVolume = GetProcAddress(m_hFmodDll, "FMOD_Channel_SetVolume");
	(FARPROC&)_FMOD_Channel_SetFrequency = GetProcAddress(m_hFmodDll, "FMOD_Channel_SetFrequency");
	(FARPROC&)_FMOD_Channel_GetFrequency = GetProcAddress(m_hFmodDll, "FMOD_Channel_GetFrequency");
	(FARPROC&)_FMOD_System_CreateSound = GetProcAddress(m_hFmodDll, "FMOD_System_CreateSound");
	(FARPROC&)_FMOD_System_PlaySound = GetProcAddress(m_hFmodDll, "FMOD_System_PlaySound");
	(FARPROC&)_FMOD_Channel_Set3DMinMaxDistance = GetProcAddress(m_hFmodDll, "FMOD_Channel_Set3DMinMaxDistance");
	(FARPROC&)_FMOD_System_CreateStream = GetProcAddress(m_hFmodDll, "FMOD_System_CreateStream");
	(FARPROC&)_FMOD_Channel_SetDelay = GetProcAddress(m_hFmodDll, "FMOD_Channel_SetDelay");
	(FARPROC&)_FMOD_Channel_SetPosition = GetProcAddress(m_hFmodDll, "FMOD_Channel_SetPosition");
	(FARPROC&)_FMOD_Sound_GetFormat = GetProcAddress(m_hFmodDll, "FMOD_Sound_GetFormat");

	if(!_FMOD_System_Create ||
		!_FMOD_System_GetVersion ||
		!_FMOD_System_GetNumDrivers ||
		!_FMOD_System_SetOutput ||
		!_FMOD_System_GetDriverCaps ||
		!_FMOD_System_SetSpeakerMode ||
		!_FMOD_System_SetDSPBufferSize ||
		!_FMOD_System_GetDriverInfo ||
		!_FMOD_System_SetSoftwareFormat ||
		!_FMOD_System_Init ||
		!_FMOD_System_Set3DSettings ||
		!_FMOD_System_Release ||
		!_FMOD_Channel_Stop ||
		!_FMOD_Sound_Release ||
		!_FMOD_System_SetReverbAmbientProperties ||
		!_FMOD_Channel_SetPaused ||
		!_FMOD_System_Update ||
		!_FMOD_System_Set3DListenerAttributes ||
		!_FMOD_Channel_IsPlaying ||
		!_FMOD_Channel_GetPaused ||
		!_FMOD_Channel_Set3DAttributes ||
		!_FMOD_Channel_GetPosition ||
		!_FMOD_Channel_SetVolume ||
		!_FMOD_Channel_SetFrequency ||
		!_FMOD_Channel_GetFrequency ||
		!_FMOD_System_CreateSound ||
		!_FMOD_System_PlaySound ||
		!_FMOD_Channel_Set3DMinMaxDistance ||
		!_FMOD_System_CreateStream ||
		!_FMOD_Channel_SetDelay ||
		!_FMOD_Channel_SetPosition ||
		!_FMOD_Sound_GetFormat)
	{
		FatalError("Couldn't load functions from %s\n", szPath);
	}
}

/*
====================
Init

====================
*/
void CSoundEngine::Init( void )
{
	//Parse fmodex.dll
	GetDllPointers();

	m_hResult = _FMOD_System_Create(&m_pSystem);
	ErrorCheck(true);

	m_hResult = _FMOD_System_GetVersion(m_pSystem, &m_iVersion);
	ErrorCheck(true);

	if(m_iVersion != FMOD_VERSION)
		FatalError("Engine detected wrong version of FMOD Ex library loaded!\nPress OK to exit the game.");

	m_hResult = _FMOD_System_GetNumDrivers(m_pSystem, &m_iNumDrivers);
	ErrorCheck(true);

	if(m_iNumDrivers == 0)
	{
		m_hResult = _FMOD_System_SetOutput(m_pSystem, FMOD_OUTPUTTYPE_NOSOUND);
		ErrorCheck(true);
	}
	else
	{
		m_hResult = _FMOD_System_GetDriverCaps(m_pSystem, 0, &m_sCaps, 0, 0, &m_sSpeakerMode);
		ErrorCheck(true);

		m_hResult = _FMOD_System_SetSpeakerMode(m_pSystem, m_sSpeakerMode);
		ErrorCheck(true);

		if(m_sCaps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			gEngfuncs.pfnClientCmd("escape\n");	
			MessageBox(NULL, "Engine detected sound device acceleration turned off!\nThis may cause sound issues in game!\nPress OK to continue.", "Warning", MB_OK | MB_SETFOREGROUND | MB_ICONWARNING);

			m_hResult = _FMOD_System_SetDSPBufferSize(m_pSystem, 1024, 10);
			ErrorCheck(true);
		}

		m_hResult = _FMOD_System_GetDriverInfo(m_pSystem, 0, m_szDeviceName, 256, 0);
		ErrorCheck(true);

		if(strstr(m_szDeviceName, "SigmaTel"))
		{
			m_hResult = _FMOD_System_SetSoftwareFormat(m_pSystem, 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			ErrorCheck(true);
		}
	}

	m_hResult = _FMOD_System_Init(m_pSystem, MAX_ACTIVE_SOUNDS, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if(m_hResult == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		m_hResult = _FMOD_System_SetSpeakerMode(m_pSystem, FMOD_SPEAKERMODE_STEREO);
		ErrorCheck(true);

		m_hResult = _FMOD_System_Init(m_pSystem, MAX_ACTIVE_SOUNDS, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
		ErrorCheck(true);
	}

	// 0.3048 / 16 = 0.01905
	m_hResult = _FMOD_System_Set3DSettings(m_pSystem, 0.0f, 0.01905f, 0.0f);
	ErrorCheck(true);

	// Parse sentences.txt
	LoadSentences();

	// Get convars
	m_pCvarRoomType = gEngfuncs.pfnGetCvarPointer("room_type");

	m_bPlayingMusic = false;
	m_bReloaded = false;
	m_iViewEntity = 0;

	// Setup watch thread
	InitializeCriticalSection(&g_CS);

	g_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!g_hExitEvent)
		FatalError("Couldn't create thread exit event!\n");

	sThreadData.pSounds = m_sSounds;

	DWORD dwThreadID;
	g_hThreadHandle = CreateThread(NULL, 0, WatchThread, &sThreadData, 0, &dwThreadID);
}

/*
====================
Shutdown

====================
*/
void CSoundEngine::Shutdown( void )
{
	SetEvent(g_hExitEvent);
	WaitForSingleObject(g_hThreadHandle, 500);
	CloseHandle(g_hThreadHandle);
	CloseHandle(g_hExitEvent);

	ResetEngine();
	_FMOD_System_Release(m_pSystem);

	DeleteCriticalSection(&g_CS);

	if(m_hFmodDll)
		FreeLibrary(m_hFmodDll);
}

/*
====================
ResetEngine

====================
*/
void CSoundEngine::ResetEngine( void )
{
	if(m_iNumActiveSounds != 0)
	{
		EnterCriticalSection(&g_CS);
		for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
		{
			sound_t *pSound = &m_sSounds[i];
			if(!pSound->pSound)
				continue;

			_FMOD_Channel_Stop(pSound->pChannel);
			_FMOD_Sound_Release(pSound->pSound);
		}

		memset(m_sSounds, 0, sizeof(m_sSounds));
		m_iNumActiveSounds = 0;
		LeaveCriticalSection(&g_CS);
	}

	if(m_iNumCachedSounds != 0)
	{
		for(int i = 0; i < m_iNumCachedSounds; i++)
		{
			delete [] m_sSoundCache[i].pFile;
		}

		memset(m_sSoundCache, 0, sizeof(m_sSoundCache));
		m_iNumCachedSounds = 0;
	}

	FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_OFF;
	_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);

	m_iCurrentRoomType = 0;

	_FMOD_Channel_SetPaused(m_pMusicChannel, true);
}

/*
====================
VidInit

====================
*/
void CSoundEngine::VidInit( void )
{
	if(gEngfuncs.pfnGetCvarFloat("s_eax"))
	{
		gEngfuncs.pfnClientCmd("escape\n");	
		MessageBox(NULL, 
			"Amnesia Sound Engine detected EAX enabled by Half-Life.\nThis mod isn't compatible with this setting.\nIn order to continue, turn off EAX in Half-Life, and restart the game.\n", 
			"Sound engine warning", 
			MB_OK | MB_SETFOREGROUND | MB_ICONWARNING); 
	}

	m_bReloaded = false;
	m_iViewEntity = 0;
}

/*
====================
ErrorCheck

====================
*/
void CSoundEngine::ErrorCheck( bool bInit )
{
	if(m_hResult != FMOD_OK)
	{
		if(bInit)
			FatalError("FMOD error! (%d) %s\n", m_hResult, FMOD_ErrorString(m_hResult));
		else
			gEngfuncs.Con_Printf("FMOD error! (%d) %s\n", m_hResult, FMOD_ErrorString(m_hResult));
	}
};

/*
====================
FatalError

====================
*/
void CSoundEngine::FatalError( char *pszFormat, ... )
{
	va_list pszArgList;
	char szText[1024];

	gEngfuncs.pfnClientCmd("escape\n");	

	va_start(pszArgList, pszFormat);
	vsprintf(szText, pszFormat, pszArgList);
	va_end(pszArgList);

	MessageBox(NULL, szText, "Sound engine error!", MB_OK | MB_SETFOREGROUND | MB_ICONERROR);
	
	gEngfuncs.pfnClientCmd("quit\n");
};

/*
====================
SetupFrame

====================
*/
void CSoundEngine::SetupFrame( ref_params_t *pparams )
{
	VectorCopy(pparams->vieworg, m_vViewOrigin);
	VectorCopy(pparams->viewangles, m_vViewAngles);
	m_iViewEntity = pparams->viewentity;


	// Fograin92: Temporary disabled
	// Setup reverbation
	/*
	if(pmove->waterlevel == 3)
	{
		if(m_iCurrentRoomType != 13)
		{
			FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_UNDERWATER;
			_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);

			m_iCurrentRoomType = 13;
		}
	}
	else
	{
		SetupReverbation();
	}
	*/
	SetupReverbation();

	// Update sound elements
	SetupSounds();
	SetupListener();
	SetupMusic();
	_FMOD_System_Update(m_pSystem);
}

/*
====================
SetupListener

====================
*/
void CSoundEngine::SetupListener( void )
{
	vec3_t vForward, vUp;
	FMOD_VECTOR vOriginFM, vForwardFM, vUpFM;
	gEngfuncs.pfnAngleVectors(m_vViewAngles, vForward, NULL, vUp);

	VectorCopyFM(m_vViewOrigin, vOriginFM);
	VectorCopyFM(vForward, vForwardFM);
	VectorCopyFM(vUp, vUpFM);

	m_hResult = _FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &vOriginFM, 0, &vForwardFM, &vUpFM);
	ErrorCheck();
}

/*
====================
SetupSounds

====================
*/
void CSoundEngine::SetupSounds( void )
{
	FMOD_BOOL	bState, bPaused;
	FMOD_VECTOR vPos;
	cl_entity_t *pEntity = NULL;
	uint iChanPos = NULL;

	static float flLastTime = 0;
	float flTime = gEngfuncs.GetClientTime();
	float flFrameTime = flTime - flLastTime;
	flLastTime = flTime;

	EnterCriticalSection(&g_CS);
	for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
	{
		sound_t *pSound = &m_sSounds[i];
		if(!pSound->pSound)
			continue;

		_FMOD_Channel_IsPlaying(pSound->pChannel, &bState);
		_FMOD_Channel_GetPaused(pSound->pChannel, &bPaused);

		if(pSound->iFlags & SND_SENTENCE)
		{
			pEntity = gEngfuncs.GetEntityByIndex(pSound->iEntIndex);

			// Get playback position
			_FMOD_Channel_GetPosition(pSound->pChannel, &iChanPos, FMOD_TIMEUNIT_PCMBYTES);
			if(pSound->iStopPos > 0 && iChanPos >= pSound->iStopPos)
			{
				sentence_t *pSentence = pSound->pSentence;
				pEntity->mouth.mouthopen = 0;
				pEntity->mouth.sndavg = 0;
				pEntity->mouth.sndcount = 0;

				if(pSound->iCurChunk >= pSentence->pOptions.size())
				{
					_FMOD_Channel_Stop(pSound->pChannel);
					_FMOD_Sound_Release(pSound->pSound);

					memset(&m_sSounds[i], 0, sizeof(sound_t));
					m_iNumActiveSounds--; 
					continue;
				}
				else
				{
					_FMOD_Channel_Stop(pSound->pChannel);
					_FMOD_Sound_Release(pSound->pSound);

					PlaySentenceChunk(pSound, pSentence, &pSentence->pOptions[pSound->iCurChunk]);
					pSound->iCurChunk++;
				}
			}
		}

		if(!bState)
		{
			if(pSound->iFlags & SND_SENTENCE)
			{
				sentence_t *pSentence = pSound->pSentence;
				pEntity->mouth.mouthopen = 0;
				pEntity->mouth.sndavg = 0;
				pEntity->mouth.sndcount = 0;

				if(pSound->iCurChunk >= pSentence->pOptions.size())
				{
					_FMOD_Channel_Stop(pSound->pChannel);
					_FMOD_Sound_Release(pSound->pSound);

					memset(&m_sSounds[i], 0, sizeof(sound_t));
					m_iNumActiveSounds--; 
					continue;
				}
				else
				{
					_FMOD_Channel_Stop(pSound->pChannel);
					_FMOD_Sound_Release(pSound->pSound);

					PlaySentenceChunk(pSound, pSentence, &pSentence->pOptions[pSound->iCurChunk]);
					pSound->iCurChunk++;
				}
			}
			else
			{
				_FMOD_Channel_Stop(pSound->pChannel);
				_FMOD_Sound_Release(pSound->pSound);
				memset(&m_sSounds[i], 0, sizeof(sound_t));
				m_iNumActiveSounds--; 
				continue;
			}
		}

		if(flFrameTime <= 0 && !bPaused)
			_FMOD_Channel_SetPaused(pSound->pChannel, true);

		if(flFrameTime > 0 && bPaused)
			_FMOD_Channel_SetPaused(pSound->pChannel, false);

		if(pSound->iFlags & SND_RELATIVE)
		{
			VectorCopyFM(m_vViewOrigin, vPos);
			_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vPos, NULL);
		}

		// Fograin92: temporary disabled
		else if(pSound->pEdict)
		{
			
			edict_t *pEdict = pSound->pEdict;
			if(pEdict->free)
				continue;

			if(pEdict->v.modelindex)
			{
				vec3_t vRealMins = pEdict->v.mins + pEdict->v.origin;
				vec3_t vRealMaxs = pEdict->v.maxs + pEdict->v.origin;
				vec3_t vCenter = (vRealMins + vRealMaxs) * 0.5;

				VectorCopyFM(vCenter, vPos);
				_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vPos, NULL);
			}
			else
			{
				VectorCopyFM(pEdict->v.origin, vPos);
				_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vPos, NULL);
			}
			
		}
		
		else if(!(pSound->iFlags & SND_2D))
		{
			VectorCopyFM(pSound->vOrigin, vPos);
			_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vPos, NULL);
		}

		if(pSound->iFlags & SND_SENTENCE && !bPaused)
		{
			if(pEntity)
			{
				BYTE *pData, *pEnd;
				double nInput[2] = { 0, 0 }, nOutput[2] = { 0, 0 };
				int iMouthOpen = NULL;

				// Get pointer to sound data at this position
				pData = (BYTE*)pSound->pCache->pFile + iChanPos;
				pEnd = (BYTE*)pSound->pCache->pFile + pSound->pCache->iSize;

				// Something can go wrong so to be sure
				if(pData >= pEnd)
					continue;

				// Fill input array with sound data
				for(int i = 0; i < 2; i++)
				{
					pData += i;

					if(pData >= pEnd)
						continue;

					if(!pData)
						continue;

					nInput[i] = *pData;
				}

				FFT(0, 1, nInput, nOutput);

				// Calculate average value
				for(int i = 0; i < 2; i++)
				{
					int iReal = nInput[i];
					int iImaginary = nOutput[i];

					iMouthOpen += AmplitudeScaled(iReal, iImaginary, 1, 40);
				}

				iMouthOpen = iMouthOpen / 2;

				if(iMouthOpen > 255)
					iMouthOpen = 255;
		
				pEntity->mouth.mouthopen = iMouthOpen;
			}
		}
	}
	LeaveCriticalSection(&g_CS);
}

/*
====================
SetupMusic

====================
*/
void CSoundEngine::SetupMusic( void )
{
	if(!m_bPlayingMusic)
		return;

	FMOD_BOOL bPaused;

	static float flLastTime = 0;
	float flTime = gEngfuncs.GetClientTime();
	float flFrameTime = flTime - flLastTime;
	flLastTime = flTime;

	_FMOD_Channel_GetPaused(m_pMusicChannel, &bPaused);

	if(flFrameTime <= 0 && !bPaused)
		_FMOD_Channel_SetPaused(m_pMusicChannel, true);

	if(flFrameTime > 0 && bPaused)
		_FMOD_Channel_SetPaused(m_pMusicChannel, false);
}

/*
======================
SetupGeometry

======================
*/
void CSoundEngine::SetupGeometry( void )
{
	if(m_bReloaded)
		return;

	//I've decided to remove geometry, was slow and not very reliable on some places

	m_bReloaded = true;
}

/*
====================
SetupReverbation

====================
*/
void CSoundEngine::SetupReverbation( void )
{
	if(m_pCvarRoomType->value == m_iCurrentRoomType)
		return;

	m_iCurrentRoomType = m_pCvarRoomType->value;

	if(m_pCvarRoomType->value == 0)
	{
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_OFF;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 1)
	{
		// Room
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_ROOM;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 2 || m_pCvarRoomType->value == 3 || m_pCvarRoomType->value == 4)
	{
		// Metal small/medium/large
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_SEWERPIPE;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 5 || m_pCvarRoomType->value == 6 || m_pCvarRoomType->value == 7)
	{
		// Tunnel small/large/medium
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_HANGAR;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 8 || m_pCvarRoomType->value == 9 || m_pCvarRoomType->value == 10)
	{
		// Chamber small/large/medium
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_STONECORRIDOR;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 11 || m_pCvarRoomType->value == 12 || m_pCvarRoomType->value == 13)
	{
		// Bright small/large/medium
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_STONECORRIDOR;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 14 || m_pCvarRoomType->value == 15 || m_pCvarRoomType->value == 16)
	{
		// Underwater
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_UNDERWATER;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 17 || m_pCvarRoomType->value == 18 || m_pCvarRoomType->value == 19)
	{
		//Concrete small/large/medium
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_STONECORRIDOR;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 20 || m_pCvarRoomType->value == 21 || m_pCvarRoomType->value == 22)
	{
		// Big 1/2/3
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_MOUNTAINS;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 23 || m_pCvarRoomType->value == 24 || m_pCvarRoomType->value == 25)
	{
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_ARENA;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
	else if(m_pCvarRoomType->value == 26 || m_pCvarRoomType->value == 27 || m_pCvarRoomType->value == 28)
	{
		FMOD_REVERB_PROPERTIES sReverb = FMOD_PRESET_SEWERPIPE;
		_FMOD_System_SetReverbAmbientProperties(m_pSystem, &sReverb);
	}
}

/*
====================
PrepareSound

====================
*/
sound_t *CSoundEngine::PrepareSound( const char *szFile, int iEntIndex, int iFlags, int iChannel )
{	
	EnterCriticalSection(&g_CS);
	if((iFlags & SND_HUD) || (iFlags & SND_TEMPENT))
	{
		int iNumTempSounds = 0;
		for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
		{
			if(!m_sSounds[i].pSound)
				continue;

			if((m_sSounds[i].iFlags & SND_HUD) && (iFlags & SND_HUD)
				|| (m_sSounds[i].iFlags & SND_TEMPENT) && (iFlags & SND_TEMPENT))
			{
				if(m_sSounds[i].iFlags & SND_TEMPENT)
				{
					if(iNumTempSounds < MAX_ACTIVE_TEMP_SOUNDS)
					{
						iNumTempSounds++;
						continue;
					}

					LeaveCriticalSection(&g_CS);
					return NULL;
				}
				else
				{
					_FMOD_Channel_Stop(m_sSounds[i].pChannel);
					_FMOD_Sound_Release(m_sSounds[i].pSound);
					memset(&m_sSounds[i], 0, sizeof(sound_t));
					m_iNumActiveSounds--;

					LeaveCriticalSection(&g_CS);
					return &m_sSounds[i];
				}
			}
		}
	}

	if(iEntIndex)
	{
		for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
		{
			if(!m_sSounds[i].pSound)
				continue;

			if(m_sSounds[i].iFlags & SND_HUD)
				continue;

			if(m_sSounds[i].iEntIndex == 0)
				continue;

			if(m_sSounds[i].iEntIndex == iEntIndex
				&& m_sSounds[i].iChannel == iChannel)
			{
				_FMOD_Channel_Stop(m_sSounds[i].pChannel);
				_FMOD_Sound_Release(m_sSounds[i].pSound);
				memset(&m_sSounds[i], 0, sizeof(sound_t));
				m_iNumActiveSounds--;

				LeaveCriticalSection(&g_CS);
				return &m_sSounds[i];
			}
		}
	}

	if(m_iNumActiveSounds == MAX_ACTIVE_SOUNDS)
	{
		for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
		{
			if(!m_sSounds[i].pSound)
				continue;

			if(m_sSounds[i].iFlags & SND_AMBIENT)
				continue;

			if(m_sSounds[i].iFlags & SND_SENTENCE)
				continue;

			if(m_sSounds[i].iChannel == CHAN_WEAPON)
				continue;

			_FMOD_Channel_Stop(m_sSounds[i].pChannel);
			_FMOD_Sound_Release(m_sSounds[i].pSound);
			memset(&m_sSounds[i], 0, sizeof(sound_t));
			m_iNumActiveSounds--;
			break;
		}
	}

	for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
	{
		if(!m_sSounds[i].pSound)
		{
			LeaveCriticalSection(&g_CS);
			return &m_sSounds[i];
		}
	}
	LeaveCriticalSection(&g_CS);

	return 0;
}

/*
====================
PlaySound

====================
*/
void CSoundEngine::PlaySound( const char *szFile, vec3_t vOrigin, int iFlags, int iChannel, float fVolume, int iPitch, float flAttenuation, edict_t *pEdict, int iEntIndex )
{
	char szPath[256];
	sentence_t *pSentence = NULL;
	FMOD_CREATESOUNDEXINFO sSoundInfo;
	int iSoundFlags = NULL;


	if(szFile[0] == '!')
	{
		iFlags |= SND_SENTENCE;
		int iID = atoi(&szFile[1]);
		pSentence = &m_sSentences[iID];

		// Fograin92 START
		char xszPath[256];
		sprintf(xszPath, "PlaySound-> SentenceID -> %i\n", iID);
		gEngfuncs.Con_Printf(xszPath);
		// Fograin92 END


		strcpy(szPath, "sound/");
		strcat(szPath, pSentence->szParentDir);

		if(pSentence->pOptions[0].szFile[strlen(pSentence->pOptions[0].szFile)-1] == '.')
			pSentence->pOptions[0].szFile[strlen(pSentence->pOptions[0].szFile)-1] = '\0';

		strcat(szPath, pSentence->pOptions[0].szFile);
		strcat(szPath, ".wav");

		// Fograin92 START
		sprintf(xszPath, "PlaySound->szFile-> %s\n", szFile);
		gEngfuncs.Con_Printf(xszPath);
		sprintf(xszPath, "PlaySound->szPath-> %s\n", szPath);
		gEngfuncs.Con_Printf(xszPath);
		// Fograin92 END
	}
	else if(szFile[0] == '*')
	{
		strcpy(szPath, "sound/");
		strcat(szPath, &szFile[1]);
	}
	else
	{
		strcpy(szPath, "sound/");
		strcat(szPath, szFile);
	}

	


	EnterCriticalSection(&g_CS);
	if((iFlags & SND_STOP) || (iFlags & SND_CHANGE_VOL) || (iFlags & SND_CHANGE_PITCH))
	{
		for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
		{
			if(!m_sSounds[i].pSound)
				continue;

			if(m_sSounds[i].iEntIndex == 0)
				continue;

			if(!strcmp(m_sSounds[i].pCache->szFile, szPath)
				&& m_sSounds[i].iEntIndex == iEntIndex)
			{
				if(iFlags & SND_STOP)
				{
					_FMOD_Channel_Stop(m_sSounds[i].pChannel);
					_FMOD_Sound_Release(m_sSounds[i].pSound);
					memset(&m_sSounds[i], 0, sizeof(sound_t));
					m_iNumActiveSounds--;

					LeaveCriticalSection(&g_CS);
					return;
				}
				else if(iFlags & SND_CHANGE_VOL)
				{
					m_sSounds[i].flVolume = fVolume;
					if(m_sSounds[i].flVolume > 1)
						m_sSounds[i].flVolume = 1;

					if(m_sSounds[i].bStereo)
						_FMOD_Channel_SetVolume(m_sSounds[i].pChannel, (m_sSounds[i].flVolume/1.3f));
					else
						_FMOD_Channel_SetVolume(m_sSounds[i].pChannel, m_sSounds[i].flVolume);

					LeaveCriticalSection(&g_CS);
					return;
				}
				else if(iFlags & SND_CHANGE_PITCH)
				{
					m_sSounds[i].iPitch = iPitch;
					_FMOD_Channel_SetFrequency(m_sSounds[i].pChannel, ((float)m_sSounds[i].iPitch / 100) * m_sSounds[i].flFrequency);

					LeaveCriticalSection(&g_CS);
					return;
				}
			}
		}
		LeaveCriticalSection(&g_CS);
		return;
	}
	LeaveCriticalSection(&g_CS);

	scache_t *pSoundData = GetSoundFromCache(szPath);
	if(!pSoundData)
		return;

	sound_t *pSound = PrepareSound(szPath, iEntIndex, iFlags, iChannel);
	if(!pSound)
		return;

	pSound->pCache = pSoundData;
	pSound->pEdict = pEdict;
	pSound->iFlags = iFlags;
	pSound->iEntIndex = iEntIndex;
	pSound->iChannel = iChannel;
	pSound->iStopPos = -1;
	
	if(pSentence && (iFlags & SND_SENTENCE))
	{
		pSound->iPitch = pSentence->pOptions[0].iPitch;
		pSound->flVolume = (float)pSentence->pOptions[0].iVolume / 100;
	}
	else
	{
		pSound->iPitch = iPitch;
		pSound->flVolume = fVolume;
		if(pSound->iChannel & CHAN_WEAPON)
			pSound->flVolume *= 2;
	}

	if(pSentence && (iFlags & SND_SENTENCE))
	{
		pSound->pSentence = pSentence;
		pSound->iCurChunk++;
	}

	if(!iEntIndex)
		pSound->pEdict = 0;


	// Fograin92: Temporary disabled
	if(pSound->pEdict)
	{
		vec3_t vRealMins = pSound->pEdict->v.maxs + pSound->pEdict->v.origin;
		vec3_t vRealMaxs = pSound->pEdict->v.mins + pSound->pEdict->v.origin;
		pSound->vOrigin = (vRealMins + vRealMaxs) * 0.5;
	}
	else
	{
		VectorCopy(vOrigin, pSound->vOrigin);
	}

	if(pSound->iEntIndex == m_iViewEntity)
	{
		pSound->iFlags |= SND_RELATIVE;
	}
	else
	{
		if(flAttenuation != ATTN_NONE)
		{
			if(flAttenuation >= ATTN_NORM)
				pSound->flRadius = MAX_DISTANCE + (1.0 - flAttenuation) * (0.5*MAX_DISTANCE);
			else
				pSound->flRadius = MAX_DISTANCE + (1.0 - flAttenuation) * (4.0*MAX_DISTANCE);
		
			if(pSound->flRadius < 0)
				pSound->flRadius = 0;
		}
		else
		{
			pSound->iFlags |= SND_RELATIVE;
		}
	}

	if(!vOrigin.Length() && !pSound->pEdict && !iEntIndex || !pSound->flRadius)
	{
		if(!(pSound->iFlags & SND_RELATIVE))
			pSound->iFlags |= SND_2D;
	}

	memset(&sSoundInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	sSoundInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	sSoundInfo.length = pSoundData->iSize;

	iSoundFlags = (FMOD_SOFTWARE | FMOD_OPENMEMORY);
	if(pSound->iFlags & SND_2D)
		iSoundFlags |= FMOD_2D;
	else
		iSoundFlags |= (FMOD_3D | FMOD_3D_LINEARROLLOFF);

	if(pSound->pCache->iLoopStart != -1 && pSound->pEdict)
		iSoundFlags |= FMOD_LOOP_NORMAL;
	else
		iSoundFlags |= FMOD_LOOP_OFF;

	m_hResult = _FMOD_System_CreateSound(m_pSystem, (const char*)pSound->pCache->pFile, iSoundFlags, &sSoundInfo, &pSound->pSound);
	ErrorCheck();

	int iChannels = 0;
	m_hResult = _FMOD_Sound_GetFormat(pSound->pSound, NULL, NULL, &iChannels, NULL);
	if(iChannels >= 2)
		pSound->bStereo = true;
	else
		pSound->bStereo = false;

	if(pSound->pEdict)
		m_hResult = _FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound->pSound, true, &pSound->pChannel);
	else
		m_hResult = _FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound->pSound, false, &pSound->pChannel);

	ErrorCheck();

	if(pSentence && (iFlags & SND_SENTENCE))
	{
		unsigned int iDelay = (pSentence->pOptions[0].flDelay*250) + (pSentence->pOptions[0].iTime);
		m_hResult = _FMOD_Channel_SetDelay(pSound->pChannel, FMOD_DELAYTYPE_END_MS, iDelay, 0);
		ErrorCheck();

		float flStartPos = ((pSound->pCache->iSize - 44) * (pSentence->pOptions[0].iStart)) / 100;
		m_hResult = _FMOD_Channel_SetPosition(pSound->pChannel, (int)flStartPos, FMOD_TIMEUNIT_PCMBYTES);
		ErrorCheck();

		if(pSentence->pOptions[0].iEnd > 0)
			pSound->iStopPos = (int)((pSound->pCache->iSize - 44) * (pSentence->pOptions[0].iEnd)) / 100;
	}

	if(!(pSound->iFlags & SND_2D))
	{
		FMOD_VECTOR vOriginFM;
		VectorCopyFM(pSound->vOrigin, vOriginFM);

		_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vOriginFM, NULL);
		_FMOD_Channel_Set3DMinMaxDistance(pSound->pChannel, MIN_DISTANCE, pSound->flRadius);
	}

	if(pSound->flVolume > 1)
		pSound->flVolume = 1;

	if(pSound->bStereo)
		_FMOD_Channel_SetVolume(pSound->pChannel, (pSound->flVolume/1.3f));
	else
		_FMOD_Channel_SetVolume(pSound->pChannel, pSound->flVolume);

	_FMOD_Channel_GetFrequency(pSound->pChannel, &pSound->flFrequency);
	_FMOD_Channel_SetFrequency(pSound->pChannel, ((float)pSound->iPitch / 100) * pSound->flFrequency);

	m_iNumActiveSounds++;
}

/*
====================
StopSound

====================
*/
void CSoundEngine::StopSound( int iEntIndex, int iChannel )
{
	EnterCriticalSection(&g_CS);
	for(int i = 0; i < MAX_ACTIVE_SOUNDS; i++)
	{
		if(!m_sSounds[i].pSound)
			continue;

		if(m_sSounds[i].iEntIndex == iEntIndex
			&& m_sSounds[i].iChannel == iChannel)
		{
			_FMOD_Channel_Stop(m_sSounds[i].pChannel);
			_FMOD_Sound_Release(m_sSounds[i].pSound);
			memset(&m_sSounds[i], 0, sizeof(sound_t));
			m_iNumActiveSounds--;

			LeaveCriticalSection(&g_CS);
			return;
		}
	}
	LeaveCriticalSection(&g_CS);
}

/*
====================
PlaySentenceChunk

====================
*/
void CSoundEngine::PlaySentenceChunk( sound_t *pSound, sentence_t *pSentence, soption_t *pChunk )
{
	char szPath[256];
	FMOD_CREATESOUNDEXINFO sSoundInfo;

	strcpy(szPath, "sound/");
	strcat(szPath, pSentence->szParentDir);

	if(pChunk->szFile[strlen(pChunk->szFile)-1] == '.')
		pChunk->szFile[strlen(pChunk->szFile)-1] = '\0';

	strcat(szPath, pChunk->szFile);
	strcat(szPath, ".wav");

	scache_t *pSoundData = GetSoundFromCache(szPath);
	if(!pSoundData)
		return;

	pSound->pCache = pSoundData;
	pSound->iPitch = pChunk->iPitch;
	pSound->flVolume = (float)pChunk->iVolume / 100;

	memset(&sSoundInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	sSoundInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	sSoundInfo.length = pSoundData->iSize;

	FMOD_VECTOR vOriginFM;
	VectorCopyFM(pSound->vOrigin, vOriginFM);

	_FMOD_System_CreateSound(m_pSystem, (const char *)pSoundData->pFile, 
		(FMOD_SOFTWARE | FMOD_OPENMEMORY | FMOD_3D | FMOD_3D_LINEARROLLOFF), 
		&sSoundInfo, &pSound->pSound);

	_FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, pSound->pSound, false, &pSound->pChannel);

	unsigned int iDelay = (pChunk->flDelay*250) + (pChunk->iTime);
	m_hResult = _FMOD_Channel_SetDelay(pSound->pChannel, FMOD_DELAYTYPE_END_MS, iDelay, 0);
	ErrorCheck();

	float flStartPos = ((pSound->pCache->iSize - 44) * (pChunk->iStart)) / 100;
	m_hResult = _FMOD_Channel_SetPosition(pSound->pChannel, (int)flStartPos, FMOD_TIMEUNIT_PCMBYTES);
	ErrorCheck();

	if(pChunk->iEnd > 0)
		pSound->iStopPos = (int)((pSound->pCache->iSize - 44) * (pChunk->iEnd)) / 100;
	else
		pSound->iStopPos = -1;

	if(pSound->flVolume > 1)
		pSound->flVolume = 1;

	if(pSound->bStereo)
		_FMOD_Channel_SetVolume(pSound->pChannel, (pSound->flVolume/1.3f));
	else
		_FMOD_Channel_SetVolume(pSound->pChannel, pSound->flVolume);

	_FMOD_Channel_GetFrequency(pSound->pChannel, &pSound->flFrequency);
	_FMOD_Channel_SetFrequency(pSound->pChannel, ((float)pSound->iPitch / 100) * pSound->flFrequency);

	_FMOD_Channel_Set3DAttributes(pSound->pChannel, &vOriginFM, NULL);
	_FMOD_Channel_Set3DMinMaxDistance(pSound->pChannel, MIN_DISTANCE, pSound->flRadius);
}

/*
====================
PlayMusic

====================
*/
void CSoundEngine::PlayMusic( char *pszFile )
{
	if(m_bPlayingMusic)
	{
		_FMOD_Channel_Stop(m_pMusicChannel);
		_FMOD_Sound_Release(m_pMusicSound);
	}

	char szPath[256];
	sprintf(szPath, "%s/media/%s", gEngfuncs.pfnGetGameDirectory(), pszFile);

	_FMOD_System_CreateStream(m_pSystem, szPath, (FMOD_SOFTWARE | FMOD_2D), NULL, &m_pMusicSound);
	_FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, m_pMusicSound, true, &m_pMusicChannel);
	_FMOD_Channel_SetVolume(m_pMusicChannel, 1.5f);

	m_bPlayingMusic = true;
}

/*
====================
GetLoopPoint

====================
*/
void CSoundEngine::GetLoopPoint( scache_t *pCache )
{
	BYTE *pFile = (BYTE*)pCache->pFile;
	BYTE *pEnd = (BYTE*)pCache->pFile + pCache->iSize;
	BYTE *pPosition = pFile;

	Int32BigEndian nChunkName;

	while(1)
	{
		// reached end of the file
		if(pFile >= pEnd)
			break;

		pPosition = pFile + 4;
		if(pPosition >= pEnd)
			break;

		// read chunk name
		nChunkName =
		((Int32BigEndian)*(pFile)<<24) |
		((Int32BigEndian)*(pFile+1)<<16) |
		((Int32BigEndian)*(pFile+2)<<8) | 
		((Int32BigEndian)*(pFile+3));

		if(nChunkName == 0x63756520) //"cue " chunk
		{
			UInt32LittleEndian nChunkLength;

			pPosition = pFile + 8;
			if(pPosition >= pEnd)
				break;

			pFile = pFile + 4;

			nChunkLength =
			((UInt32LittleEndian)*(pFile+3)<<24) |
			((UInt32LittleEndian)*(pFile+2)<<16) |
			((UInt32LittleEndian)*(pFile+1)<<8) | 
			((UInt32LittleEndian)*(pFile));
			
			if(nChunkLength / 24 > 0)
			{
				pCache->iLoopStart = 0;
			}

			// cue chunk was found, so return
			return;
		}

		// go ahead one byte
		pFile = pFile + 1;
	}
}

/*
====================
PrecacheSound

====================
*/
scache_t* CSoundEngine::PrecacheSound( char *szFile )
{
	for(int i = 0; i < m_iNumCachedSounds; i++)
	{
		if(!strcmp(m_sSoundCache[i].szFile, szFile))
		{
			return &m_sSoundCache[i];
		}
	}

	if(m_iNumCachedSounds == MAX_CACHED_SOUNDS)
	{
		gEngfuncs.Con_Printf("Sound cache is full! Sound (%s) won't be part of playback!\n", szFile);
		return 0;
	}

	int	iSize = 0;
	byte *pData = (byte *)gEngfuncs.COM_LoadFile(szFile, 5, &iSize);

	if(!pData)
	{
		gEngfuncs.Con_Printf("Couldn't precache (%s).\n", szFile);
		return 0;
	}

	scache_t *pCache = &m_sSoundCache[m_iNumCachedSounds];
	
	pCache->pFile = new byte[iSize];
	memcpy(pCache->pFile, pData, sizeof(byte)*iSize);
	pCache->iSize = iSize;

	strcpy(pCache->szFile, szFile);
	gEngfuncs.COM_FreeFile(pData);

	pCache->iLoopStart = -1;
	pCache->iLoopEnd = -1;

	GetLoopPoint(pCache);

	m_iNumCachedSounds++;

	return pCache;
}

/*
====================
GetSoundFromCache

====================
*/
scache_t* CSoundEngine::GetSoundFromCache( char *szFile )
{
	for(int i = 0; i < m_iNumCachedSounds; i++)
	{
		if(!strcmp(szFile, m_sSoundCache[i].szFile))
			return &m_sSoundCache[i];
	}

	return PrecacheSound(szFile);
}

/*
====================
TempEntPlaySound

====================
*/
void CSoundEngine::TempEntPlaySound( TEMPENTITY *pTemp, float flVolume )
{
	if(pTemp->entity.origin == pTemp->entity.prevstate.origin)
		return;

	pTemp->entity.prevstate.origin = pTemp->entity.origin;

	// Glass impact
	if(pTemp->hitSound & BOUNCE_GLASS)
	{
		switch(gEngfuncs.pfnRandomLong(0, 2))
		{
		case 1:
			PlaySound("debris/glass1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("debris/glass2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("debris/glass3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}
	
	// Metal Impact
	if(pTemp->hitSound & BOUNCE_METAL)
	{
		switch(gEngfuncs.pfnRandomLong(0, 5))
		{
		case 1:
			PlaySound("debris/metal1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("debris/metal2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 3:
			PlaySound("debris/metal3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 4:
			PlaySound("debris/metal4.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("debris/metal5.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}

	// Flesh impact
	if(pTemp->hitSound & BOUNCE_FLESH)
	{
		switch(gEngfuncs.pfnRandomLong(0, 6))
		{
		case 1:
			PlaySound("debris/flesh1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2: 
			PlaySound("debris/flesh2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 3:
			PlaySound("debris/flesh3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 4:
			PlaySound("debris/flesh4.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 5:
			PlaySound("debris/flesh5.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 6:
			PlaySound("debris/flesh6.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("debris/flesh7.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}

	// Wood impact
	if(pTemp->hitSound & BOUNCE_WOOD)
	{
		switch(gEngfuncs.pfnRandomLong(0, 2))
		{
		case 1:
			PlaySound("debris/wood1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("debris/wood2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("debris/wood3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}

	// Shell impact
	if(pTemp->hitSound & BOUNCE_SHELL)
	{
		switch(gEngfuncs.pfnRandomLong(0, 2))
		{
		case 1:
			PlaySound("player/pl_shell2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("player/pl_shell3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("player/pl_shell1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}

	// Concrete impact
	if(pTemp->hitSound & BOUNCE_CONCRETE)
	{
		switch(gEngfuncs.pfnRandomLong(0, 2))
		{
		case 1:
			PlaySound("debris/concrete1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("debris/concrete2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("debris/concrete3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}

	// Shotgun shell impact
	if(pTemp->hitSound & BOUNCE_SHOTSHELL)
	{
		switch(gEngfuncs.pfnRandomLong(0, 2))
		{
		case 1:
			PlaySound("weapons/sshell2.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		case 2:
			PlaySound("weapons/sshell3.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		default:
			PlaySound("weapons/sshell1.wav", pTemp->entity.origin, SND_TEMPENT, CHAN_AUTO, flVolume, PITCH_NORM);
		}
	}
}

/*
====================
LoadSentences

====================
*/
void CSoundEngine::LoadSentences( void )
{
	if(m_iNumSentences)
		return;

	int iSize = 0;
	char *pFile = (char *)gEngfuncs.COM_LoadFile("scripts/sentences.txt", 5, &iSize);

	if(!pFile || !iSize)
	{
		gEngfuncs.Con_Printf("ERROR: Couldn't load sentences.txt!");
		return;
	}

	int i = 0;
	while(1)
	{
		if(i >= iSize)
			break;

		if(pFile[i] == '\n' || pFile[i] == '\r')
		{
			i++;

			if(i >= iSize)
				break;

			continue;
		}

		// Skip whitelines
		if(pFile[i] == ' ' || pFile[i] == '\0')
		{
			while(1)
			{
				if(pFile[i] != ' ' && pFile[i] == '\0')
					break;

				if(pFile[i] == '\n')
				{
					i++;
					break;
				}

				i++;
			}
			continue;
		}

		if(pFile[i] == '/' || pFile[i] == '\\')
		{
			// Skip comments
			while(1)
			{
				if(i >= iSize)
					break;

				if(pFile[i] == '\n')
				{
					i++;
					break;
				}

				i++;
			}

			// Begin from start
			continue;
		}

		// If we got here, it means we found a valid entry
		sentence_t *pSentence = &m_sSentences[m_iNumSentences];
		sprintf(pSentence->szID, "!%i", m_iNumSentences); //Set ID
		m_iNumSentences++;

		int iDefaultPitch = 100;
		int iDefaultTime = 0;
		int iDefaultStart = 0;
		int iDefaultEnd = 0;
		int iDefaultVol = 100;

		// Skip sentence name
		while(1)
		{
			if(i >= iSize)
			{
				gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
				gEngfuncs.COM_FreeFile(pFile);
				return;
			}

			if(pFile[i] == ' ')
			{
				// Skip to next token
				while(1)
				{
					if(i >= iSize)
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
						gEngfuncs.COM_FreeFile(pFile);
						return;
					}

					if(pFile[i] != ' ')
						break;

					i++;
				}
				break;
			}
			i++;
		}

		int j = i;
		while(1)
		{
			if(j >= iSize)
			{
				gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
				gEngfuncs.COM_FreeFile(pFile);
				return;
			}

			if(pFile[j] == '\n' || pFile[j] == '\r')
			{
				// Seems Valve takes a default as "vox/"
				strcpy(pSentence->szParentDir, "vox/");
				break;
			}

			if(pFile[j] == '\\' || pFile[j] == '/')
			{
				j = 0;
				while(1)
				{
					if(i >= iSize)
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
						gEngfuncs.COM_FreeFile(pFile);
						return;
					}

					pSentence->szParentDir[j] = pFile[i];
					i++; j++;

					if(pFile[(i - 1)] == '\\' || pFile[(i - 1)] == '/')
					{
						pSentence->szParentDir[i] = 0;//terminate
						break;
					}
				}
				break;
			}

			j++;
		}

		//Lets set options up
		while(1)
		{
			//Skip white spaces
			while(1)
			{
				if(i >= iSize)
				{
					// Reached EOF
					break;
				}

				if(pFile[i] != ' ')
					break;

				i++;
			}

			if(i >= iSize)
			{
				// Reached EOF
				break;
			}

			if(pFile[i] == '\n' || pFile[i] == '\r')
			{
				i++;
				break;
			}

			pSentence->pOptions.resize((pSentence->pOptions.size()+1));
			soption_t *pOption = &pSentence->pOptions[(pSentence->pOptions.size()-1)];
			memset(pOption, 0, sizeof(soption_t));

			// Set defaults
			pOption->iPitch = iDefaultPitch;
			pOption->iStart = iDefaultStart;
			pOption->iEnd = iDefaultEnd;
			pOption->iTime = iDefaultTime;
			pOption->iVolume = iDefaultVol;
			pOption->flDelay = 0.0f;

			while(1)
			{
				// Skip whitespace
				while(1)
				{
					if(i >= iSize)
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
						gEngfuncs.COM_FreeFile(pFile);
						return;
					}

					if(pFile[i] != ' ')
						break;

					i++;
				}

				if(pFile[i] != '(')
					break;

				//If there's a bracelet before any sentence chunk, it means that it's defaulted for forthcoming options.
				i++; // Go into bracelet
				while(1)
				{
					if(i >= iSize)
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
						gEngfuncs.COM_FreeFile(pFile);
						return;
					}

					if(pFile[i] != ' ')
						break;

					// Go ahead
					i++;
				}

				if(pFile[i] == '\n' || pFile[i] == '\r')
				{
					gEngfuncs.Con_Printf("Sentences.txt error: Unexpected newline\n");
					return;
				}

				//Extract option parameters
				j = 0;
				while(1)
				{
					// Yes, this sucks
					while(1)
					{
						if(i >= iSize)
						{
							gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
							gEngfuncs.COM_FreeFile(pFile);
							return;
						}

						if(pFile[i] != ' ')
							break;

						i++;
					}

					if(pFile[i] == ')')
					{
						i++;
						break;
					}

					if(i >= iSize)
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
						gEngfuncs.COM_FreeFile(pFile);
						return;
					}

					char szValue[5];
					char *pName = &pFile[i]; 
					i++; //Skip to value

					// Skip any whitespace(who knows)
					while(1)
					{
						if(i >= iSize)
						{
							gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
							gEngfuncs.COM_FreeFile(pFile);
							return;
						}

						if(pFile[i] != ' ')
							break;

						i++;
					}

					int j = 0;
					while(1)
					{
						// Skip whitespace
						while(1)
						{
							if(i >= iSize)
							{
								gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
								gEngfuncs.COM_FreeFile(pFile);
								return;
							}

							if(pFile[i] != ' ')
								break;

							i++;
						}

						if(i >= iSize)
						{
							gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
							gEngfuncs.COM_FreeFile(pFile);
							return;
						}

						if(pFile[i] != '0' && pFile[i] != '1' &&
							pFile[i] != '2' && pFile[i] != '3' &&
							pFile[i] != '4' && pFile[i] != '5' &&
							pFile[i] != '6' && pFile[i] != '7' &&
							pFile[i] != '8' && pFile[i] != '9')
						{
							szValue[j] = 0;
							break;
						}

						szValue[j] = pFile[i];
						i++; j++;
					}
					
					if(*pName == 'p')
						pOption->iPitch = iDefaultPitch = atoi(szValue);
					else if(*pName == 't')
						pOption->iTime = iDefaultTime = atoi(szValue);
					else if(*pName == 's')
						pOption->iStart = iDefaultStart = atoi(szValue);
					else if(*pName == 'e')
						pOption->iEnd = iDefaultEnd = atoi(szValue);
					else if(*pName == 'v')
						pOption->iVolume = iDefaultVol = atoi(szValue);
				}
			}

			j = 0;
			while(1)
			{
				if(i >= iSize)
				{
					// Reached EOF
					break;
				}

				if(pFile[i] == ' ')
				{
					i++;
					break;
				}

				if(pFile[i] == '\n' || pFile[i] == '\r')
				{
					pOption->szFile[j] = 0;
					break;
				}

				// Sound params
				if(pFile[i] == '(')
				{
					i++; // Go into bracelet
					while(1)
					{
						if(i >= iSize)
						{
							gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
							gEngfuncs.COM_FreeFile(pFile);
							return;
						}

						if(pFile[i] != ' ')
							break;

						i++;
					}

					if(pFile[i] == '\n' || pFile[i] == '\r')
					{
						gEngfuncs.Con_Printf("Sentences.txt error: Unexpected newline\n");
						return;
					}

					//Extract option parameters
					j = 0;
					while(1)
					{
						// Yes, this sucks
						while(1)
						{
							if(i >= iSize)
							{
								gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
								gEngfuncs.COM_FreeFile(pFile);
								return;
							}

							if(pFile[i] != ' ')
								break;

							i++;
						}

						if(pFile[i] == ')')
						{
							i++;

							if(pFile[i] == ',')
								pOption->flDelay = 0.5; i++;
	
							break;
						}

						char szValue[5];
						char *pName = &pFile[i]; 
						i++; //Skip to value

						// Skip any whiteline(who knows)
						while(1)
						{
							if(i >= iSize)
							{
								gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
								gEngfuncs.COM_FreeFile(pFile);
								return;
							}

							if(pFile[i] != ' ')
								break;

							i++;
						}

						int j = 0;
						while(1)
						{
							// Skip whitespace
							while(1)
							{
								if(i >= iSize)
								{
									gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
									gEngfuncs.COM_FreeFile(pFile);
									return;
								}

								if(pFile[i] != ' ')
									break;

								i++;
							}

							if(i >= iSize)
							{
								gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
								gEngfuncs.COM_FreeFile(pFile);
								return;
							}

							if(pFile[i] != '0' && pFile[i] != '1' &&
								pFile[i] != '2' && pFile[i] != '3' &&
								pFile[i] != '4' && pFile[i] != '5' &&
								pFile[i] != '6' && pFile[i] != '7' &&
								pFile[i] != '8' && pFile[i] != '9')
							{
								szValue[j] = 0;
								break;
							}

							szValue[j] = pFile[i];
							i++; j++;
						}
						
						if(*pName == 'p')
							pOption->iPitch = atoi(szValue);
						else if(*pName == 't')
							pOption->iTime = atoi(szValue);
						else if(*pName == 's')
							pOption->iStart = atoi(szValue);
						else if(*pName == 'e')
							pOption->iEnd = atoi(szValue);
						else if(*pName == 'v')
							pOption->iVolume = atoi(szValue);
					}

					// Break main loop
					break;
				}

				if(pFile[i] == ',')
				{
					pOption->flDelay = 0.5; i++;
					continue;
				}

				if(i >= iSize)
				{
					gEngfuncs.Con_Printf("Sentences.txt error: Unexpected end!\n");
					gEngfuncs.COM_FreeFile(pFile);
					return;
				}

				pOption->szFile[j] = pFile[i];
				i++; j++;
			}
		}
	}
	
	// Free file
	gEngfuncs.COM_FreeFile(pFile);
}

/*
====================
MsgFunc_PlayAudio

====================
*/
int CSoundEngine::MsgFunc_PlayAudio( const char *pszName, int iSize, void *pBuf )
{
	BEGIN_READ(pBuf, iSize);
	char *pszFile = READ_STRING();
	int iLooped = READ_BYTE();

	PlayMusic(pszFile);

	return 1;
}

/*
===================================
PM_PlaySample


===================================
*/
extern "C" void PM_PlaySample( const char *szFile, float fVolume, int iPitch, float *origin )
{ 

	//char szPath[256];
	//sprintf(szPath, "Foot-> %s", szFile);
	//gEngfuncs.Con_Printf(szPath);

	gSoundEngine.PlaySound( szFile, origin, SND_RELATIVE, CHAN_BODY, fVolume*2, iPitch );
}


/*
===================================
CL_SoundEngineReset


===================================
*/
extern "C" __declspec( dllexport ) void CL_SoundEngineReset( void )
{
	gSoundEngine.ResetEngine();
}


/*
===================================
CL_SoundPrecache


===================================
*/
extern "C" __declspec( dllexport ) void CL_SoundPrecache( void *pPath )
{
	char szPath[256];

	strcpy(szPath, "sound/");
	strcat(szPath, (char*)pPath);
	gSoundEngine.PrecacheSound(szPath);
}


/*
===================================
CL_SoundEmit


===================================
*/
extern "C" __declspec( dllexport ) void CL_EmitSound( void *pEdict, void *pPath, int iEntIndex, float flVolume, float flAttenuation, int iFlags, int iPitch, int iChannel )
{
	// Fograin92: Display sound/sentence name and pass it
	//char szPath[256];
	//sprintf(szPath, "%s", pPath);
	//gEngfuncs.Con_Printf(szPath);

	gSoundEngine.PlaySound((char *)pPath, g_vecZero, iFlags, iChannel, flVolume, iPitch, flAttenuation, (edict_t *)pEdict, iEntIndex);
}


/*
===================================
CL_SoundAmbient


===================================
*/
extern "C" __declspec( dllexport ) void CL_EmitAmbient( void *pEdict, void *pPath, int iEntIndex, float flVolume, float flAttenuation, float *pOrigin, int iFlags, int iPitch )
{
	iFlags |= SND_AMBIENT;
	gSoundEngine.PlaySound((char *)pPath, pOrigin, iFlags, CHAN_STATIC, flVolume, iPitch, flAttenuation, (edict_t *)pEdict, iEntIndex );
}