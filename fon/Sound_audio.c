/* Sound_audio.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/09/14 Sound_recordFixedTime records in stereo if mono is not available
 * pb 2003/12/06 use sys/soundcard.h instead of linux/soundcard.h for FreeBSD compatibility
 * pb 2005/04/24 Sound_recordFixedTime: Firewire Solo 1264
 * pb 2005/06/16 removed previous change (System Preferences handles this)
 * pb 2005/10/13 edition for OpenBSD
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/20 Sound_playPart and Sound_play allow stereo
 * pb 2006/12/30 Sound_playPart and Sound_play allow better stereo
 * pb 2007/01/07 PortAudio
 * Erez Volk
 */

#include <errno.h>

#ifdef linux
	#define DEV_AUDIO  "/dev/dsp"
#else
	#define DEV_AUDIO  "/dev/audio"
#endif

/*
 * In the new Mac SDK/ CodeWarrior, the following must be included before <Sound.h> and <Resources.h>...???
 */
#include "Sound.h"
#include "Preferences.h"

//#define USE_PORTAUDIO  1
#ifndef USE_PORTAUDIO
	#if defined (macintosh)
		#define USE_PORTAUDIO  1
	#else
		#define USE_PORTAUDIO  0
	#endif
#endif

#if USE_PORTAUDIO
	#include "portaudio.h"
	#if defined (macintosh)
		#include "pa_mac_core.h"
	#endif
#elif defined (sgi)
	#include <audio.h>
	#include <unistd.h>   /* sginap (): nap while waiting for a sound to finish playing. */
#elif defined (macintosh)
	#include "macport_on.h"
	#include <Carbon/Carbon.h>
	#include "macport_off.h"
	#ifndef __MWERKS__
		typedef SndListPtr *SndListHandle;
	#endif
#elif defined (sun)
	#include <fcntl.h>
	#include <stropts.h>
	#include <unistd.h>   /* ioctl open write close */
	#if defined (sun4)
		#include <ctype.h>
		#include <sun/audioio.h>
	#else
		#include <sys/audioio.h>
	#endif
#elif defined (HPUX)
	#include <fcntl.h>
	#include <ctype.h>
	#include <sys/audio.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
#elif defined (_WIN32)
	#include <windows.h>
	#include <mmsystem.h>
#elif defined (linux)
	#include <fcntl.h>
	#if defined (__OpenBSD__) || defined (__NetBSD__)
		#include <soundcard.h>
	#else
		#include <sys/soundcard.h>
	#endif
	#include <sys/ioctl.h>   /* ioctl */
	#include <unistd.h>   /* open write close read */
#else
	#include <fcntl.h>
#endif

static int ulaw2linear [] = 
      { -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
        -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
        -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
        -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
         -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
         -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
         -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
         -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
         -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
         -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
          -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
          -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
          -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
          -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
          -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
           -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
         32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
         23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
         15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
         11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
          7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
          5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
          3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
          2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
          1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
          1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
           876,    844,    812,    780,    748,    716,    684,    652,
           620,    588,    556,    524,    492,    460,    428,    396,
           372,    356,    340,    324,    308,    292,    276,    260,
           244,    228,    212,    196,    180,    164,    148,    132,
           120,    112,    104,     96,     88,     80,     72,     64,
            56,     48,     40,     32,     24,     16,      8,      0
       };

#if USE_PORTAUDIO
struct Sound_recordFixedTime_Info {
	long numberOfSamples, numberOfSamplesRead;
	short *buffer;
};
static long getNumberOfSamplesRead (volatile struct Sound_recordFixedTime_Info *info) {
	volatile long numberOfSamplesRead = info -> numberOfSamplesRead;
	return numberOfSamplesRead;
}
static int portaudioStreamCallback (
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *void_info)
{
	(void) output;
	(void) timeInfo;
	(void) statusFlags;
	struct Sound_recordFixedTime_Info *info = (struct Sound_recordFixedTime_Info *) void_info;
	unsigned long samplesLeft = info -> numberOfSamples - info -> numberOfSamplesRead;
	if (samplesLeft > 0) {
		unsigned long dsamples = samplesLeft > frameCount ? frameCount : samplesLeft;
		memcpy (info -> buffer + 1 + info -> numberOfSamplesRead, input, 2 * dsamples);
		info -> numberOfSamplesRead += dsamples;
		if (info -> numberOfSamplesRead >= info -> numberOfSamples) return paComplete;
	} else /*if (info -> numberOfSamplesRead >= info -> numberOfSamples)*/ {
		info -> numberOfSamplesRead = info -> numberOfSamples;
		return paComplete;
	}
	return paContinue;
}
#endif

Sound Sound_recordFixedTime (int inputSource, double gain, double balance, double sampleRate, double duration) {
	Sound me = NULL;
	long numberOfSamples, i;
	short *buffer = NULL;
	int mulaw = FALSE;
	int can16bit = TRUE;
	int fakeMonoByStereo = FALSE;   /* Will be set to TRUE for systems (like MacOS X) that do not allow direct mono recording. */

	/* Declare system-dependent data structures. */

	#if USE_PORTAUDIO
		static bool paInitialized = false;
		PaStream *portaudioStream = NULL;
		volatile struct Sound_recordFixedTime_Info info = { 0 };
		PaStreamParameters streamParameters = { 0 };
	#elif defined (sgi)
		ALconfig config;
		ALport port;
	#elif defined (macintosh)
		SPB spb;
		long refNum, soundFeatures;
		(void) gain;
		(void) balance;
		can16bit = Gestalt (gestaltSoundAttr, & soundFeatures) == 0 &&
			(soundFeatures & (1 << gestalt16BitAudioSupport)) != 0;
	#elif defined (_WIN32)
		HWAVEIN hWaveIn = 0;
		WAVEFORMATEX waveFormat;
		WAVEHDR waveHeader;
		MMRESULT err;
		(void) inputSource;
		(void) gain;
		(void) balance;
	#else
		int fd = -1;   /* Other systems use stream I/O with a file descriptor. */
		#if defined (sun)
			struct audio_info info;
		#elif defined (HPUX)
			struct audio_describe info;
			struct audio_gains gains;
			struct audio_limits limits;
                #elif defined (linux)
#define min(a,b) a > b ? b : a
			int dev_mask;
			int fd_mixer;
			int val;
		#endif
	#endif

	/* Check representation of shorts. */

	if (sizeof (short) != 2)
		return Melder_errorp ("(Sound_record:) Cannot record a sound on this machine.");

	/* Check sampling frequency. */

	#if USE_PORTAUDIO && defined (macintosh)
		if (sampleRate != 44100 && sampleRate != 48000 && sampleRate != 96000)
	#elif defined (sgi)
		if (sampleRate != 8000 && sampleRate != 9800 && sampleRate != 11025 &&
		    	sampleRate != 16000 && sampleRate != 22050 &&
		    	sampleRate != 32000 && sampleRate != 44100 &&
		    	sampleRate != 48000 && sampleRate != 0)
	#elif defined (macintosh)
		if (sampleRate != 11025 && sampleRate != 22050 && sampleRate != 44100)
	#elif defined (sun) || defined (linux)
		if (sampleRate != 8000 && sampleRate != 11025 &&
		    	sampleRate != 16000 && sampleRate != 22050 &&
		    	sampleRate != 32000 && sampleRate != 44100 &&
		    	sampleRate != 48000)
	#elif defined (HPUX)
		if (sampleRate != 8000 && sampleRate != 11025 &&
		    	sampleRate != 16000 && sampleRate != 22050 &&
		    	sampleRate != 32000 && sampleRate != 44100 &&
		    	sampleRate != 48000 && sampleRate != 5512)
	#elif defined (_WIN32)
		if (sampleRate != 8000 && sampleRate != 11025 &&
		    	sampleRate != 16000 && sampleRate != 22050 &&
		    	sampleRate != 32000 && sampleRate != 44100 &&
		    	sampleRate != 48000 && sampleRate != 96000)
	#endif
		return Melder_errorp ("(Sound_record:) Audio hardware does not support sampling frequency %.8g.", sampleRate);

	/*
	 * Open phase 1.
	 * On some systems, the info is filled in before the audio port is opened.
	 * On other systems, the info is filled in after the port is opened.
	 */
	#if USE_PORTAUDIO
		if (! paInitialized) {
			PaError err = Pa_Initialize ();
			if (err) {
				Melder_error2 (L"Pa_Initialize: ", Melder_peekUtf8ToWcs (Pa_GetErrorText (err)));
				goto error;
			}
			paInitialized = true;
		}
	#elif defined (sgi)
		config = ALnewconfig ();
		if (! config) return Melder_errorp
			("(Sound_record:) Do not know how to record a sound on this machine.");
		/* We do not open the port yet, because the info is an argument to opening the port. */
	#elif defined (macintosh)
		if (SPBOpenDevice (NULL, siWritePermission, & refNum) != noErr)
			return Melder_errorp ("Cannot open audio input device.\n"
				"Perhaps somebody else is recording on your computer.");
	#elif defined (_WIN32)
	#else
		/* We must open the port now, because we use an ioctl to set the info to an open port. */
		fd = open (DEV_AUDIO, O_RDONLY);
		if (fd == -1) {
			if (errno == EBUSY)
				Melder_error1 (L"(Sound_record:) Audio device in use by another program.");
			else
				#ifdef linux
					Melder_error1 (L"(Sound_record:) Cannot open audio device.\n"
						"Consult /usr/doc/HOWTO/Sound-HOWTO.");
				#else
					Melder_error1 (L"(Sound_record:) Cannot open audio device.");
				#endif
			goto error;
		}
		/* The device immediately started recording into its buffer, but probably at the wrong rate etc. */
		/* Pause and flush this rubbish. */
		#if defined (sun)
			AUDIO_INITINFO (& info);
			info. record. pause = 1;
			ioctl (fd, AUDIO_SETINFO, & info);   /* Pause! */
			ioctl (fd, I_FLUSH, FLUSHR);   /* Discard buffers! */
		#elif defined (HPUX)
			ioctl (fd, AUDIO_RESET, RESET_RX_BUF | RESET_RX_OVF);
			ioctl (fd, AUDIO_PAUSE, AUDIO_RECEIVE);
		#elif defined (linux)
			ioctl (fd, SNDCTL_DSP_RESET, NULL);
		#endif
	#endif

	/* Set the input source; the default is the microphone. */

	#if USE_PORTAUDIO
		if (inputSource < 1 || inputSource > Pa_GetDeviceCount ()) {
			Melder_error3 (L"Unknown device #", Melder_integer (inputSource), L".");
			goto error;
		}
		streamParameters. device = inputSource - 1;
	#elif defined (sgi)
		if (inputSource == 0) {
			/* Do not change input source: e.g., use the one set by the Audio Control Panel. */
		} else {
			long params [2];
			params [0] = AL_INPUT_SOURCE;
			params [1] = inputSource == 2 ? AL_INPUT_LINE :
				inputSource == 3 ? AL_INPUT_DIGITAL : AL_INPUT_MIC;
			ALsetparams (AL_DEFAULT_DEVICE, params, 2);
		}
	#elif defined (macintosh)
	{
		short macInputSource = inputSource;
		OSErr err = SPBSetDeviceInfo (refNum, siInputSource, & macInputSource);
		if (err != noErr) {
			static int notified = FALSE;
			if (! notified) Melder_warning ("(Sound_record:) "
				"Cannot set input source to %d. Error #%d.", inputSource, err);
			notified = TRUE;
		}
	}
	#elif defined (sun)
		AUDIO_INITINFO (& info);
		info. record. port = inputSource == 2 ? AUDIO_LINE_IN : AUDIO_MICROPHONE;   /* No digital. */
		if (ioctl (fd, AUDIO_SETINFO, & info) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set port.");
			goto error;
		}
	#elif defined (HPUX)
		if (ioctl (fd, AUDIO_SET_INPUT, inputSource == 2 ? AUDIO_IN_LINE : AUDIO_IN_MIKE) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set input source.");
			goto error;
		}
	#elif defined (linux)
		fd_mixer = open("/dev/mixer", O_WRONLY);		
		if (fd_mixer == -1) {
			Melder_error1 (L"(Sound_record:) Cannot open /dev/mixer.");
			goto error;
		}
		

		if (ioctl(fd_mixer, SOUND_MIXER_READ_RECMASK, &dev_mask) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot access /dev/mixer.");
			goto error;
		}
		/*		printf("%d %d %d\n", dev_mask, (dev_mask&SOUND_MASK_LINE), (dev_mask&SOUND_MASK_MIC));*/
		if (inputSource == 2) {
			/*  AUDIO_LINE_IN */
			if (dev_mask&SOUND_MASK_LINE) {
				dev_mask = SOUND_MASK_LINE;
			} else {
				Melder_error1 (L"(Sound_record:) Can't set LINE as recording device");
				goto error;
			}
			
		} else {
			/*  AUDIO_MICROPHONE */
			if (dev_mask&SOUND_MASK_MIC) {
				dev_mask = SOUND_MASK_MIC;
			} else {
				Melder_error1 (L"(Sound_record:) Can't set MIC as recording device");
				goto error;
			}

		}
		/*		printf("%d\n", dev_mask);*/
		if (ioctl(fd_mixer, SOUND_MIXER_WRITE_RECSRC, &dev_mask) == -1) {
			Melder_error1 (L"(Sound_record:) Can't set recording device in mixer");		
		}
		if (ioctl(fd_mixer, SOUND_MIXER_READ_RECSRC, &dev_mask) == -1) {
			Melder_error1 (L"(Sound_record:) Can't read recording device from mixer");		
		} else {
			/*			printf("%x\n",dev_mask);*/
			if (dev_mask&SOUND_MASK_MIC) {
				inputSource = 1;				
			} else if (dev_mask&SOUND_MASK_LINE) {
				inputSource = 2;
				
			}
		} 
	#endif

	/* Set gain and balance. */

	#if USE_PORTAUDIO || defined (sgi) || defined (HPUX) || defined (macintosh) || defined (_WIN32)
		/* Taken from Audio Control Panel. */
	#elif defined (sun)
		AUDIO_INITINFO (& info);
		info. record. gain = gain <= 0.0 ? 0 : gain >= 1.0 ? 255 : floor (gain * 255 + 0.5);
		info. record. balance = balance <= 0.0 ? 0 : balance >= 1.0 ? 64 : floor (balance * 64 + 0.5);
		if (ioctl (fd, AUDIO_SETINFO, & info) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set gain and balance.");
			goto error;
		}
	#elif defined (linux)
		val = (gain <= 0.0 ? 0 : gain >= 1.0 ? 100 : floor (gain * 100 + 0.5));  
		balance = balance <= 0 ? 0 : balance >= 1 ? 1 : balance;
		if (balance >= 0.5) {
			val = (int)(((int)(val*balance/(1-balance)) << 8) | val);
		} else {
			val = (int)(val | ((int)(val*(1-balance)/balance) << 8));
		}
		val = (int)((min(2-2*balance,1))*val) | ((int)((min(2*balance,1))*val) << 8);
		if (inputSource == 1) {			
			/* MIC */		       
			if (ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_MIC), &val) == -1) {
				Melder_error1 (L"(Sound_record:) Cannot set gain and balance.");
				goto error;				
			}
		} else {
			/* LINE */
			if (ioctl(fd_mixer, MIXER_WRITE(SOUND_MIXER_LINE), &val) == -1) {
				Melder_error1 (L"(Sound_record:) Cannot set gain and balance.");
				goto error;				
			}
		}
		close(fd_mixer);
	#endif

	/* Set the sampling frequency. */

	#if USE_PORTAUDIO
		// Set while opening.
	#elif defined (sgi)
		if (sampleRate == 0.0) {
			/* Do not change sampling frequency. Get it from audio panel. */
			long params [2];
			params [0] = AL_INPUT_RATE;
			ALgetparams (AL_DEFAULT_DEVICE, params, 2);
			switch (params [1]) {
				case AL_RATE_8000:  sampleRate = 8000; break;
				case 9800: sampleRate = 9800; break;
				case AL_RATE_11025: sampleRate = 11025; break;
				case AL_RATE_16000: sampleRate = 16000; break;
				case AL_RATE_22050: sampleRate = 22050; break;
				case AL_RATE_32000: sampleRate = 32000; break;
				case AL_RATE_44100: sampleRate = 44100; break;
				case AL_RATE_48000: sampleRate = 48000; break;
				default:
					return Melder_errorp ("(Sound_recordFixedTime:) Wrong sampling frequency.");
			}
		} else {
			/* Set sampling frequency. */
			long params [2];
			params [0] = AL_INPUT_RATE;
			params [1] = sampleRate;
			ALsetparams (AL_DEFAULT_DEVICE, params, 2);
		}
	#elif defined (macintosh)
		unsigned long sampleRate_uf = sampleRate * 65536.0;
		if (SPBSetDeviceInfo (refNum, siSampleRate, & sampleRate_uf) != noErr) {
			Melder_error3 (L"(Sound_record:) Cannot set sampling frequency to ", Melder_integer (sampleRate), L" Hertz.");
			goto error;
		}
	#elif defined (sun)
		AUDIO_INITINFO (& info);
		info. record. sample_rate = sampleRate;
		if (ioctl (fd, AUDIO_SETINFO, & info) == -1) {
			Melder_error3 (L"(Sound_record:) Cannot set sampling frequency to ", Melder_double (sampleRate), L" Hertz.");
			goto error;
		}
	#elif defined (HPUX)
		if (ioctl (fd, AUDIO_SET_SAMPLE_RATE, (int) sampleRate) == -1) {
			Melder_error3 (L"(Sound_record:) Cannot set sampling frequency to ", Melder_integer (sampleRate), L" Hertz.");
			goto error;
		}
	#elif defined (linux)
		if (ioctl (fd, SNDCTL_DSP_SPEED, & sampleRate) == -1) {
			Melder_error3 (L"(Sound_record:) Cannot set sampling frequency to ", Melder_integer (sampleRate), L" Hertz.");
			goto error;
		}
	#elif defined (_WIN32)
		waveFormat. nSamplesPerSec = sampleRate;
	#endif

	/* Set the number of channels to 1 (mono), if possible. */

	#if USE_PORTAUDIO
		streamParameters. channelCount = 1;
	#elif defined (sgi)
		ALsetchannels (config, AL_MONO);
	#elif defined (macintosh)
	{
		short numberOfChannels = 1;
		if (SPBSetDeviceInfo (refNum, siNumberChannels, & numberOfChannels) != noErr)
			fakeMonoByStereo = TRUE;
	}
	#elif defined (sun)
		AUDIO_INITINFO (& info);
		info. record. channels = 1;
		if (ioctl (fd, AUDIO_SETINFO, & info) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set to mono.");
			goto error;
		}
	#elif defined (HPUX)
		if (ioctl (fd, AUDIO_SET_CHANNELS, 1) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set to mono.");
			goto error;
		}
	#elif defined (linux)
		val = 1;
		if (ioctl (fd, SNDCTL_DSP_CHANNELS, & val) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set to mono.");
			goto error;
		}
	#elif defined (_WIN32)
		waveFormat. nChannels = 1;
	#endif

	/* Set the encoding to 16-bit linear (or to 8-bit linear, if 16-bit is not available). */

	#if USE_PORTAUDIO
		streamParameters. sampleFormat = paInt16;
	#elif defined (sgi)
		ALsetwidth (config, AL_SAMPLE_16);
	#elif defined (macintosh)
	{
		OSType compressionType = 'NONE';
		short sampleSize = can16bit ? 16 : 8;
		if (SPBSetDeviceInfo (refNum, siCompressionType, & compressionType) != noErr) {
			Melder_error1 (L"(Sound_record:) Cannot set to linear.");
			goto error;
		}
		if (SPBSetDeviceInfo (refNum, siSampleSize, & sampleSize) != noErr) {
			Melder_error3 (L"(Sound_record:) Cannot set to ", Melder_integer (sampleSize), L"-bit.");
			goto error;
		}
	}
	#elif defined (sun)
		AUDIO_INITINFO (& info);
		info. record. precision = 16;
		info. record. encoding = AUDIO_ENCODING_LINEAR;
		if (ioctl (fd, AUDIO_SETINFO, & info) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set to 16-bit linear.");
			goto error;
		}
	#elif defined (HPUX)
		if (ioctl (fd, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set 16-bit linear.");
			goto error;
		}
	#elif defined (linux)
		val = AFMT_S16_LE;
		if (ioctl (fd, SNDCTL_DSP_SETFMT, &val) == -1) {
			Melder_error1 (L"(Sound_record:) Cannot set 16-bit linear.");
			goto error;
		}
	#elif defined (_WIN32)
		waveFormat. wFormatTag = WAVE_FORMAT_PCM;
		waveFormat. wBitsPerSample = 16;
		waveFormat. nBlockAlign = waveFormat. nChannels * waveFormat. wBitsPerSample / 8;
		waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
	#endif

	/* Create a buffer for recording, and the resulting sound. */

	numberOfSamples = floor (sampleRate * duration + 0.5);
	if (numberOfSamples < 1)
		return Melder_errorp ("(Sound_record:) Duration too short.");
	if (! (buffer = NUMsvector (1, numberOfSamples * (fakeMonoByStereo ? 2 : 1)))) return NULL;
	if (! (me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate))) {   // STEREO BUG
		NUMsvector_free (buffer, 1);
		return NULL;
	}
	Melder_assert (my nx == numberOfSamples);

	/*
	 * Open phase 2.
	 * This starts recording now.
	 */

	#if USE_PORTAUDIO
		streamParameters. suggestedLatency = Pa_GetDeviceInfo (inputSource - 1) -> defaultLowInputLatency;
		#if defined (macintosh)
			struct paMacCoreStreamInfo macCoreStreamInfo = { 0 };
			macCoreStreamInfo. size = sizeof (paMacCoreStreamInfo);
			macCoreStreamInfo. hostApiType = paCoreAudio;
			macCoreStreamInfo. version = 0x01;
			macCoreStreamInfo. flags = paMacCore_ChangeDeviceParameters | paMacCore_FailIfConversionRequired;
			streamParameters. hostApiSpecificStreamInfo = & macCoreStreamInfo;
		#endif
		info. numberOfSamples = numberOfSamples;
		info. numberOfSamplesRead = 0;
		info. buffer = buffer;
		PaError err = Pa_OpenStream (& portaudioStream, & streamParameters, NULL,
			sampleRate, 0, paNoFlag, portaudioStreamCallback, (void *) & info);
		if (err) { Melder_error2 (L"open ", Melder_peekUtf8ToWcs (Pa_GetErrorText (err))); goto error; }
		Pa_StartStream (portaudioStream);
		if (err) { Melder_error2 (L"start ", Melder_peekUtf8ToWcs (Pa_GetErrorText (err))); goto error; }
	#elif defined (sgi)
		port = ALopenport ("Sound_record", "r", config);
		if (! port) {
			return Melder_errorp ("(Sound_recordFixedTime:) Cannot open audio port.");
			goto error;
		}
	#elif defined (macintosh)
		spb. inRefNum = refNum;
		spb. bufferLength = spb. count = numberOfSamples * (can16bit ? 2 : 1) * (fakeMonoByStereo ? 2 : 1);
		spb. milliseconds = 0;
		spb. bufferPtr = (void *) & buffer [1];
		spb. completionRoutine = NULL;
		spb. interruptRoutine = NULL;
		spb. userLong = 0;
		spb. error = noErr;
		spb. unused1 = 0;
		if (SPBRecord (& spb, false) != noErr) {
			Melder_errorp ("(Sound_recordFixedTime:) Cannot create audio buffer.");
			goto error;
		}
	#elif defined (sun)
		if (info. record. encoding == AUDIO_ENCODING_ULAW) {
			mulaw = TRUE;
			sampleRate = info. record. sample_rate;
			numberOfSamples = floor (sampleRate * duration + 0.5);
			Melder_casual ("Recording with mulaw encoding at sampling frequency %f.", sampleRate);
			forget (me);
			me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate);
		}
		AUDIO_INITINFO (& info);
		info. record. pause = 0;   /* Resume. */
		ioctl (fd, AUDIO_SETINFO, & info);
	#elif defined (HPUX)
		ioctl (fd, AUDIO_GET_LIMITS, & limits);
		ioctl (fd, AUDIO_SET_RXBUFSIZE, limits. max_receive_buffer_size);
		ioctl (fd, AUDIO_RESUME, AUDIO_RECEIVE);
	#elif defined (_WIN32)
		waveFormat. cbSize = 0;
		err = waveInOpen (& hWaveIn, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL);
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while opening."); goto error; }
	#endif

	/* Read the sound into the buffer. */

	#if USE_PORTAUDIO
		// The callback will do this. Just wait.
		while (/*getNumberOfSamplesRead (& info)*/ info. numberOfSamplesRead < numberOfSamples) {
			//Pa_Sleep (1);
			//Melder_casual ("filled %ld/%ld", getNumberOfSamplesRead (& info), numberOfSamples);
		}
	#elif defined (sgi)
		ALreadsamps (port, & buffer [1], numberOfSamples);
	#elif defined (macintosh)
	#elif defined (_WIN32)
		waveHeader. dwFlags = 0;
		waveHeader. lpData = (char *) & buffer [1];
		waveHeader. dwBufferLength = numberOfSamples * 2;
		waveHeader. dwLoops = 0;
		waveHeader. lpNext = NULL;
		waveHeader. reserved = 0;
		err = waveInPrepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while preparing header."); goto error; }
		err = waveInAddBuffer (hWaveIn, & waveHeader, sizeof (WAVEHDR));
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while listening."); goto error; }
		err = waveInStart (hWaveIn);
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while starting."); goto error; }
      		while (! (waveHeader. dwFlags & WHDR_DONE)) { (void) clock (); }
		err = waveInUnprepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while unpreparing header."); goto error; }
	#else
		if (mulaw)
			read (fd, & ((char *) buffer) [1], numberOfSamples);
		else {
			long bytesLeft = 2 * numberOfSamples, dbytes, bytesRead = 0;
			while (bytesLeft) {
				dbytes = read (fd, & ((char *) buffer) [2 + bytesRead], bytesLeft > 4000 ? 4000 : bytesLeft);
				if (dbytes <= 0) break;
				bytesLeft -= dbytes;
				bytesRead += dbytes;
			};
		}
	#endif

	/* Copy the buffered data to the sound object, and discard the buffer. */

	if (fakeMonoByStereo)
		for (i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = ((long) buffer [i + i - 1] + buffer [i + i]) * (1.0 / 65536);
	else if (mulaw)
		for (i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = ulaw2linear [((unsigned char *) buffer) [i]] * (1.0 / 32768);
	else if (can16bit)
		for (i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = buffer [i] * (1.0 / 32768);
	else
		for (i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = ((int) ((unsigned char *) buffer) [i + 1] - 128) * (1.0 / 128);
	NUMsvector_free (buffer, 1);

	/* Close the audio device. */

	#if USE_PORTAUDIO
		Pa_StopStream (portaudioStream);
		Pa_CloseStream (portaudioStream);
	#elif defined (sgi)
		ALcloseport (port);
		ALfreeconfig (config);
	#elif defined (macintosh)
		SPBCloseDevice (refNum);
	#elif defined (_WIN32)
		err = waveInClose (hWaveIn);
		if (err != MMSYSERR_NOERROR) { Melder_error3 (L"(Sound_record:) Error ", Melder_integer (err), L" while closing."); goto error; }
	#else
		close (fd);
	#endif

	/* Hand the resulting sound to the caller. */

	return me;
error:
	forget (me);
	NUMsvector_free (buffer, 1);
	#if USE_PORTAUDIO
		if (portaudioStream) Pa_StopStream (portaudioStream);
		if (portaudioStream) Pa_CloseStream (portaudioStream);
	#elif defined (sgi)
		if (port) ALcloseport (port);
		if (config) ALfreeconfig (config);
	#elif defined (macintosh)
		SPBCloseDevice (refNum);
	#elif defined (_WIN32)
		if (hWaveIn != 0) waveInClose (hWaveIn);
	#else
		if (fd != -1) close (fd);
	#endif
	return Melder_errorp ("Sound not recorded.");
}

/********** PLAYING A SOUND **********/

static struct SoundPlay {
	long numberOfSamples, i1, i2, zeroPadding;
	double tmin, tmax, dt, t1;
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t);
	void *closure;
	short *buffer;
} thePlayingSound;

static int melderPlayCallback (void *closure, long samplesPlayed) {
	struct SoundPlay *me = (struct SoundPlay *) closure;
	int phase = 2;
	double t = samplesPlayed <= my zeroPadding ? my tmin :
		samplesPlayed >= my zeroPadding + my numberOfSamples ? my tmax :
		my t1 + (my i1 - 1.5 + samplesPlayed - my zeroPadding) * my dt;
	if (! Melder_isPlaying) {
		NUMsvector_free (my buffer, 1), my buffer = 0;
		phase = 3;
	}
	if (my callback)
		return my callback (my closure, phase, my tmin, my tmax, t);
	return 1;
}

int Sound_playPart (Sound me, double tmin, double tmax,
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	long ifsamp = floor (1.0 / my dx + 0.5), bestSampleRate = Melder_getBestSampleRate (ifsamp);
	if (ifsamp == bestSampleRate) {
		struct SoundPlay *thee = (struct SoundPlay *) & thePlayingSound;
		float *fromLeft = my z [1], *fromRight = my ny > 1 ? my z [2] : NULL;
		Melder_stopPlaying (Melder_IMPLICIT);
		long i1, i2;
		if ((thy numberOfSamples = Matrix_getWindowSamplesX (me, tmin, tmax, & i1, & i2)) < 1) goto end;
		thy tmin = tmin;
		thy tmax = tmax;
		thy dt = my dx;
		thy t1 = my x1;
		thy callback = callback;
		thy closure = closure;
		thy zeroPadding = (long) (ifsamp * Melder_getZeroPadding ());
		int numberOfChannels = my ny > 1 ? 2 : 1;
		thy buffer = NUMsvector (1, (i2 - i1 + 1 + 2 * thy zeroPadding) * numberOfChannels); cherror
		thy i1 = i1;
		thy i2 = i2;
		short *to = thy buffer;
		if (numberOfChannels == 2) {
			for (long i = i1; i <= i2; i ++) {
				long valueLeft = (long) floor (fromLeft [i] * 32768.0 + 0.5);
				* ++ to = valueLeft < -32768 ? -32768 : valueLeft > 32767 ? 32767 : valueLeft;
				long valueRight = (long) floor (fromRight [i] * 32768.0 + 0.5);
				* ++ to = valueRight < -32768 ? -32768 : valueRight > 32767 ? 32767 : valueRight;
			}
		} else {
			for (long i = i1; i <= i2; i ++) {
				long value = (long) floor (fromLeft [i] * 32768.0 + 0.5);
				* ++ to = value < -32768 ? -32768 : value > 32767 ? 32767 : value;
			}
		}
		if (thy callback) thy callback (thy closure, 1, tmin, tmax, tmin);
		if (! Melder_play16 (thy buffer + 1, ifsamp,
			thy zeroPadding + thy numberOfSamples + thy zeroPadding, numberOfChannels, melderPlayCallback, thee))
			Melder_flushError (NULL);
	} else {
		Sound resampled = Sound_resample (me, bestSampleRate, 1);
		Sound_playPart (resampled, tmin, tmax, callback, closure);   /* Recursively. */
		forget (resampled);
	}
end:
	iferror return 0;
	return 1;
}

int Sound_play (Sound me,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure)
{
	return Sound_playPart (me, my xmin, my xmax, playCallback, playClosure);
}

/* End of file Sound_audio.c */
