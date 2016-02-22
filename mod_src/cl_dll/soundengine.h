// Following work is released under LGPL license,
// shipped with this source code.

#if !defined ( SOUNDENGINE_H )
#define SOUNDENGINE_H
#if defined( _WIN32 )
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#undef LoadImage

#include <iostream>
#include <vector>

#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"

#include "ref_params.h"
#include "parsemsg.h"
#include "cl_entity.h"
#include "r_efx.h"

extern Vector g_vecZero;

#define MAX_ACTIVE_SOUNDS		128
#define	MAX_CACHED_SOUNDS		4096
#define MAX_ACTIVE_TEMP_SOUNDS	5
#define MAX_SENTENCES			8192	//4096

// Default rolloff factors
#define MIN_DISTANCE	32
#define MAX_DISTANCE	1024

// Extended sound defines
#define SND_STOP			(1<<5)
#define SND_CHANGE_VOL		(1<<6)
#define SND_CHANGE_PITCH	(1<<7)
#define SND_SPAWNING		(1<<8)
#define SND_LOOP			(1<<9)
#define SND_AMBIENT			(1<<10)
#define SND_SENTENCE		(1<<11)
#define SND_2D				(1<<12)
#define SND_HUD				(1<<13)
#define SND_MUSIC			(1<<14)
#define SND_TEMPENT			(1<<15)
#define SND_RELATIVE		(1<<16)

// Colliding temp entity sounds

#ifndef BOUNCE_GLASS
#define BOUNCE_GLASS		0x01
#define	BOUNCE_METAL		0x02
#define BOUNCE_FLESH		0x04
#define BOUNCE_WOOD			0x08
#define BOUNCE_SHRAP		0x10
#define BOUNCE_SHELL		0x20
#define	BOUNCE_CONCRETE		0x40
#define BOUNCE_SHOTSHELL	0x80
#endif

// Channel defines
#define CHAN_AUTO				0			// low priority, common sound
#define CHAN_WEAPON				1			// highest priority, weapon sound
#define	CHAN_VOICE				2			// highest priority, sentence sound
#define CHAN_ITEM				3			// low priority
#define	CHAN_BODY				4			// highest priority, replacement of snd_2d
#define CHAN_STREAM				5			// allocate stream channel from the static or dynamic area
#define CHAN_STATIC				6			// allocate channel from the static area 
#define CHAN_NETWORKVOICE_BASE	7			// voice data coming across the network
#define CHAN_NETWORKVOICE_END	500			// network voice data reserves slots (CHAN_NETWORKVOICE_BASE through CHAN_NETWORKVOICE_END).

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;

typedef INT8 int8_t;
typedef UINT8 uint8_t;
typedef INT16 int16_t;
typedef UINT16 uint16_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;

typedef int16_t Int16BigEndian;
typedef uint16_t UInt16LittleEndian;
typedef int32_t Int32BigEndian;
typedef uint32_t UInt32LittleEndian;

struct scache_t
{
	byte *pFile;
	int	  iSize;
	char  szFile[256];

	int iLoopStart;
	int iLoopEnd;
};

struct soption_t
{
	char szFile[64];
	float flDelay;

	int iPitch; // Frequency
	int iStart; // Start sound at iStart % of whole length
	int iEnd; // Stop sound at iEnd % of whole length
	int iVolume; // Play sound at iVolume % of default folume (1.0)
	int iTime; // Delay between words (0 = full delay, 100 = no delay)
};

struct sentence_t
{
	char szID[64];
	char szParentDir[64];
	std::vector<soption_t> pOptions;
};

struct sound_t
{
	FMOD_SOUND*		pSound;
	FMOD_CHANNEL*	pChannel;

	scache_t*		pCache;
	edict_t*		pEdict;
	TEMPENTITY*		pTemp;

	vec3_t			vOrigin;

	int				iFlags;
	int				iPitch;
	int				iEntIndex;
	int				iChannel;

	float			flVolume;
	float			flRadius;
	float			flFrequency;

	sentence_t*		pSentence;
	int				iCurChunk;

	int				iStopPos;

	bool			bStereo;
};

/*
====================
VectorCopyFM

====================
*/
inline void VectorCopyFM( float *vecIn, FMOD_VECTOR &vecOut )
{
	vecOut.x = vecIn[0];
	vecOut.y = vecIn[1];
	vecOut.z = vecIn[2];
}

/*
====================
ScaleByte

====================
*/
inline void ScaleByte( DWORD *nInput )
{
	if(*nInput % 2 != 0)
	{
		*nInput += 1;
	}
}

/*
====================
FFT

====================
*/
inline short FFT(short int dir,long m,double *x,double *y)
{
   long n,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   n = 1;
   for (i=0;i<m;i++) 
      n *= 2;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0; 
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0; 
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<n;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1; 
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1) 
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<n;i++) {
         x[i] /= n;
         y[i] /= n;
      }
   }
   
   return(TRUE);
}

//FFT defines
inline double GetFrequencyIntensity(double re, double im)
{
    return sqrt((re*re)+(im*im));
}
#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))
#define Amplitude(re,im,len) (GetFrequencyIntensity(re,im)/(len))
#define AmplitudeScaled(re,im,len,scale) ((int)Amplitude(re,im,len)%scale)
//FFT defines

/*
====================
CSoundEngine

====================
*/
class CSoundEngine
{
public:
	void	GetDllPointers( void );

	void	Init( void );
	void	Shutdown( void );

	void	ResetEngine( void );
	void	VidInit( void );

	void	ErrorCheck( bool bInit = false );
	void	FatalError( char *pszFormat, ... );

	void	SetupFrame( ref_params_t *pparams );
	void    SetupListener( void );
	void	SetupSounds( void );
	void	SetupMusic( void );
	void	SetupGeometry( void );
	void	SetupReverbation( void );

	sound_t* PrepareSound( const char *szFile, int iEntIndex, int iFlags, int iChannel );
	void	 PlaySound( const char *szFile, vec3_t vOrigin = g_vecZero, int iFlags = NULL, int iChannel = CHAN_AUTO, float fVolume = VOL_NORM, int iPitch = PITCH_NORM, float flAttenuation = ATTN_NORM, edict_t *pEdict = NULL, int iEntIndex = NULL );
	void	 StopSound( int iEntIndex, int iChannel );
	void	 PlaySentenceChunk( sound_t *pSound, sentence_t *pSentence, soption_t *pChunk );
	void	 PlayMusic( char *pszFile );

	void		GetLoopPoint( scache_t *pCache );
	scache_t*	PrecacheSound( char *szFile );
	scache_t*	GetSoundFromCache( char *szFile );

	void	TempEntPlaySound( TEMPENTITY *pTemp, float flVolume );
	void	LoadSentences( void );
	int		MsgFunc_PlayAudio( const char *pszName, int iSize, void *pBuf );

private:
	HMODULE				m_hFmodDll;

	FMOD_SYSTEM*		m_pSystem;
	FMOD_RESULT			m_hResult;
	FMOD_CAPS			m_sCaps;
	FMOD_SPEAKERMODE	m_sSpeakerMode;

	FMOD_SOUND*			m_pMusicSound;
	FMOD_CHANNEL*		m_pMusicChannel;

	FMOD_GEOMETRY*		m_pGeometry;

	unsigned int		m_iVersion;
	int					m_iNumDrivers;
	int					m_iCurrentRoomType;
	int					m_iViewEntity;

	bool				m_bPlayingMusic;
	bool				m_bReloaded;

	char				m_szDeviceName[256];

	sound_t	    		m_sSounds[MAX_ACTIVE_SOUNDS];
	int					m_iNumActiveSounds;

	scache_t			m_sSoundCache[MAX_CACHED_SOUNDS];
	int					m_iNumCachedSounds;

	sentence_t			m_sSentences[MAX_SENTENCES];
	int					m_iNumSentences;

	cvar_t*				m_pCvarRoomType;

	vec3_t				m_vViewOrigin;
	vec3_t				m_vViewAngles;

private:
	FMOD_RESULT			(_stdcall *_FMOD_System_Create) ( FMOD_SYSTEM **system );
	FMOD_RESULT			(_stdcall *_FMOD_System_GetVersion) ( FMOD_SYSTEM *system, unsigned int *  version );
	FMOD_RESULT			(_stdcall *_FMOD_System_GetNumDrivers) ( FMOD_SYSTEM *system, int *numdrivers );
	FMOD_RESULT			(_stdcall *_FMOD_System_SetOutput) ( FMOD_SYSTEM *system, FMOD_OUTPUTTYPE output );
	FMOD_RESULT			(_stdcall *_FMOD_System_GetDriverCaps) ( FMOD_SYSTEM *system, int id, FMOD_CAPS *caps, int *minfrequency, int *maxfrequency, FMOD_SPEAKERMODE *controlpanelspeakermode );
	FMOD_RESULT			(_stdcall *_FMOD_System_SetSpeakerMode) ( FMOD_SYSTEM *system, FMOD_SPEAKERMODE speakermode );
	FMOD_RESULT			(_stdcall *_FMOD_System_SetDSPBufferSize) ( FMOD_SYSTEM *system, unsigned int bufferlength, int numbuffers );
	FMOD_RESULT			(_stdcall *_FMOD_System_GetDriverInfo) ( FMOD_SYSTEM *system, int id, char *name, int namelen, FMOD_GUID *guid );
	FMOD_RESULT			(_stdcall *_FMOD_System_SetSoftwareFormat) ( FMOD_SYSTEM *system, int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod );
	FMOD_RESULT			(_stdcall *_FMOD_System_Init) ( FMOD_SYSTEM *system, int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata );
	FMOD_RESULT			(_stdcall *_FMOD_System_Set3DSettings) ( FMOD_SYSTEM *system, float dopplerscale, float distancefactor, float rolloffscale );
	FMOD_RESULT			(_stdcall *_FMOD_System_Release) ( FMOD_SYSTEM *system );
	FMOD_RESULT	        (_stdcall *_FMOD_Channel_Stop) ( FMOD_CHANNEL *channel );
	FMOD_RESULT			(_stdcall *_FMOD_Sound_Release) ( FMOD_SOUND *sound );
	FMOD_RESULT			(_stdcall *_FMOD_System_SetReverbAmbientProperties) ( FMOD_SYSTEM *system, FMOD_REVERB_PROPERTIES *prop );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_SetPaused) ( FMOD_CHANNEL *channel, FMOD_BOOL paused );
	FMOD_RESULT			(_stdcall *_FMOD_System_Update) ( FMOD_SYSTEM *system );
	FMOD_RESULT			(_stdcall *_FMOD_System_Set3DListenerAttributes) ( FMOD_SYSTEM *system, int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_IsPlaying) ( FMOD_CHANNEL *channel, FMOD_BOOL *isplaying );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_GetPaused) ( FMOD_CHANNEL *channel, FMOD_BOOL *paused );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_Set3DAttributes) ( FMOD_CHANNEL *channel, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_GetPosition) ( FMOD_CHANNEL *channel, unsigned int *position, FMOD_TIMEUNIT postype );

	FMOD_RESULT			(_stdcall *_FMOD_Channel_SetVolume) ( FMOD_CHANNEL *channel, float volume );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_SetFrequency) ( FMOD_CHANNEL *channel, float frequency );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_GetFrequency) ( FMOD_CHANNEL *channel, float *frequency );
	FMOD_RESULT			(_stdcall *_FMOD_System_CreateSound) ( FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound );
	FMOD_RESULT			(_stdcall *_FMOD_System_PlaySound) ( FMOD_SYSTEM *system, FMOD_CHANNELINDEX channelid, FMOD_SOUND *sound, FMOD_BOOL paused, FMOD_CHANNEL **channel );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_Set3DMinMaxDistance) ( FMOD_CHANNEL *channel, float mindistance, float maxdistance );
	FMOD_RESULT			(_stdcall *_FMOD_System_CreateStream) ( FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound );
	FMOD_RESULT			(_stdcall *_FMOD_Channel_SetDelay) ( FMOD_CHANNEL *  channel, FMOD_DELAYTYPE  delaytype,   unsigned int  delayhi,   unsigned int  delaylo);
	FMOD_RESULT			(_stdcall *_FMOD_Channel_SetPosition) ( FMOD_CHANNEL *  channel,unsigned int  position,   FMOD_TIMEUNIT  postype);
	FMOD_RESULT			(_stdcall *_FMOD_Sound_GetFormat) (FMOD_SOUND *  sound,FMOD_SOUND_TYPE *  type, FMOD_SOUND_FORMAT *  format, int *  channels, int *  bits);
};

extern CSoundEngine gSoundEngine;

#endif