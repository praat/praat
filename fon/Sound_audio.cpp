/* Sound_audio.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * Stefan de Konink 2007/12/02 big-endian Linux
 * pb 2008/01/19 double
 * pb 2008/07/07 split zero padding between silenceBefore and silenceAfter
 * fb 2010/02/26 fix resource leak fd_mixer in case of error during init
 * pb 2010/04/20 Sound_recordFixedTime for Linux: repair
 * pb 2010/11/02 Sound_recordFixedTime for Linux: repair bug from 1998
 * pb 2011/06/07 C++
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
#include "../external/portaudio/portaudio.h"

#if defined (macintosh)
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
	#include "pa_mac_core.h"
	#include "macport_off.h"
#elif defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include <mmsystem.h>
	#include "winport_off.h"
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
		short *input2 = (short*) input;
		trace ("read %d samples: %d, %d, %d...", (int) dsamples, (int) input2 [0], (int) input2 [1], (int) input2 [3]);
		if (info -> numberOfSamplesRead >= info -> numberOfSamples) return paComplete;
	} else /*if (info -> numberOfSamplesRead >= info -> numberOfSamples)*/ {
		info -> numberOfSamplesRead = info -> numberOfSamples;
		return paComplete;
	}
	return paContinue;
}

Sound Sound_recordFixedTime (int inputSource, double gain, double balance, double sampleRate, double duration) {
	bool inputUsesPortAudio = MelderAudio_getInputUsesPortAudio ();
	PaStream *portaudioStream = NULL;
	#if defined (macintosh)
	#elif defined (_WIN32)
		HWAVEIN hWaveIn = 0;
	#else
		int fd = -1;   /* Other systems use stream I/O with a file descriptor. */
		int fd_mixer = -1;
	#endif
	try {
		long numberOfSamples, i;
		int mulaw = FALSE;
		int can16bit = TRUE;
		int fakeMonoByStereo = FALSE;   /* Will be set to TRUE for systems (like MacOS X) that do not allow direct mono recording. */

		/* Declare system-dependent data structures. */

		static bool paInitialized = false;
		volatile struct Sound_recordFixedTime_Info info = { 0 };
		PaStreamParameters streamParameters = { 0 };
		#if defined (macintosh)
			(void) gain;
			(void) balance;
		#elif defined (_WIN32)
			WAVEFORMATEX waveFormat;
			WAVEHDR waveHeader;
			MMRESULT err;
			(void) inputSource;
			(void) gain;
			(void) balance;
		#elif defined (linux)
			#define min(a,b) a > b ? b : a
			int dev_mask;
			int fd_mixer = -1;
			int val;
		#endif

		/* Check representation of shorts. */

		if (sizeof (short) != 2)
			Melder_throw ("Cannot record a sound on this computer.");

		/* Check sampling frequency. */

		bool supportsSamplingFrequency = true;
		if (inputUsesPortAudio) {
			#if defined (macintosh)
				if (sampleRate != 44100 && sampleRate != 48000 && sampleRate != 96000) supportsSamplingFrequency = false;
			#endif
		} else {
			#if defined (macintosh)
				if (sampleRate != 44100) supportsSamplingFrequency = false;
			#elif defined (linux)
				if (sampleRate != 8000 && sampleRate != 11025 &&
						sampleRate != 16000 && sampleRate != 22050 &&
						sampleRate != 32000 && sampleRate != 44100 &&
						sampleRate != 48000) supportsSamplingFrequency = false;
			#elif defined (_WIN32)
				if (sampleRate != 8000 && sampleRate != 11025 &&
						sampleRate != 16000 && sampleRate != 22050 &&
						sampleRate != 32000 && sampleRate != 44100 &&
						sampleRate != 48000 && sampleRate != 96000) supportsSamplingFrequency = false;
			#endif
		}
		if (! supportsSamplingFrequency)
			Melder_throw ("Your audio hardware does not support a sampling frequency of ", sampleRate, " Hz.");

		/*
		 * Open phase 1.
		 * On some systems, the info is filled in before the audio port is opened.
		 * On other systems, the info is filled in after the port is opened.
		 */
		if (inputUsesPortAudio) {
			if (! paInitialized) {
				PaError err = Pa_Initialize ();
				if (err)
					Melder_throw ("Pa_Initialize: ", Pa_GetErrorText (err));
				paInitialized = true;
			}
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
			#else
				/* We must open the port now, because we use an ioctl to set the info to an open port. */
				fd = open (DEV_AUDIO, O_RDONLY);
				if (fd == -1) {
					if (errno == EBUSY)
						Melder_throw ("Audio device in use by another program.");
					else
						#ifdef linux
							Melder_throw ("Cannot open audio device.\nPlease switch on PortAudio in the Sound Recording Preferences.");
						#else
							Melder_throw ("Cannot open audio device.");
						#endif
				}
				/* The device immediately started recording into its buffer, but probably at the wrong rate etc. */
				/* Pause and flush this rubbish. */
				#if defined (linux)
					ioctl (fd, SNDCTL_DSP_RESET, NULL);
				#endif
			#endif
		}

		/* Set the input source; the default is the microphone. */

		if (inputUsesPortAudio) {
			if (inputSource < 1 || inputSource > Pa_GetDeviceCount ())
				Melder_throw ("Unknown device #", inputSource, ".");
			streamParameters. device = inputSource - 1;
		} else {
			#if defined (macintosh)
			#elif defined (linux)
				fd_mixer = open ("/dev/mixer", O_WRONLY);		
				if (fd_mixer == -1)
					Melder_throw ("Cannot open /dev/mixer.");
				dev_mask = inputSource == 1 ? SOUND_MASK_MIC : SOUND_MASK_LINE;
				if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
					Melder_throw ("Cannot set recording device in mixer");		
			#endif
		}

		/* Set gain and balance. */

		if (inputUsesPortAudio) {
			/* Taken from Audio Control Panel. */
		} else {
			#if defined (macintosh) || defined (_WIN32)
				/* Taken from Audio Control Panel. */
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
					if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_MIC), & val) == -1)
						Melder_throw ("Cannot set gain and balance.");
				} else {
					/* LINE */
					if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_LINE), & val) == -1)
						Melder_throw ("Cannot set gain and balance.");
				}
				close (fd_mixer);
				fd_mixer = -1;
			#endif
		}

		/* Set the sampling frequency. */

		if (inputUsesPortAudio) {
			// Set while opening.
		} else {
			#if defined (macintosh)
			#elif defined (linux)
				int sampleRate_int = (int) sampleRate;
				if (ioctl (fd, SNDCTL_DSP_SPEED, & sampleRate_int) == -1)
					Melder_throw ("Cannot set sampling frequency to ", sampleRate, " Hz.");
			#elif defined (_WIN32)
				waveFormat. nSamplesPerSec = sampleRate;
			#endif
		}

		/* Set the number of channels to 1 (mono), if possible. */

		if (inputUsesPortAudio) {
			streamParameters. channelCount = 1;
		} else {
			#if defined (macintosh)
			#elif defined (linux)
				val = 1;
				if (ioctl (fd, SNDCTL_DSP_CHANNELS, & val) == -1)
					Melder_throw ("Cannot set to mono.");
			#elif defined (_WIN32)
				waveFormat. nChannels = 1;
			#endif
		}

		/* Set the encoding to 16-bit linear (or to 8-bit linear, if 16-bit is not available). */

		if (inputUsesPortAudio) {
			streamParameters. sampleFormat = paInt16;
		} else {
			#if defined (macintosh)
			#elif defined (linux)
				#if __BYTE_ORDER == __BIG_ENDIAN
					val = AFMT_S16_BE;
				#else
					val = AFMT_S16_LE;
				#endif
				if (ioctl (fd, SNDCTL_DSP_SETFMT, & val) == -1)
					Melder_throw ("Cannot set 16-bit linear.");
			#elif defined (_WIN32)
				waveFormat. wFormatTag = WAVE_FORMAT_PCM;
				waveFormat. wBitsPerSample = 16;
				waveFormat. nBlockAlign = waveFormat. nChannels * waveFormat. wBitsPerSample / 8;
				waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
			#endif
		}

		/* Create a buffer for recording, and the resulting sound. */

		numberOfSamples = floor (sampleRate * duration + 0.5);
		if (numberOfSamples < 1)
			Melder_throw ("Duration too short.");
		autoNUMvector <short> buffer (1, numberOfSamples * (fakeMonoByStereo ? 2 : 1));
		autoSound me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate);   // STEREO BUG
		Melder_assert (my nx == numberOfSamples);

		/*
		 * Open phase 2.
		 * This starts recording now.
		 */

		if (inputUsesPortAudio) {
			streamParameters. suggestedLatency = Pa_GetDeviceInfo (inputSource - 1) -> defaultLowInputLatency;
			#if defined (macintosh)
				PaMacCoreStreamInfo macCoreStreamInfo = { 0 };
				macCoreStreamInfo. size = sizeof (PaMacCoreStreamInfo);
				macCoreStreamInfo. hostApiType = paCoreAudio;
				macCoreStreamInfo. version = 0x01;
				macCoreStreamInfo. flags = paMacCoreChangeDeviceParameters | paMacCoreFailIfConversionRequired;
				streamParameters. hostApiSpecificStreamInfo = & macCoreStreamInfo;
			#endif
			info. numberOfSamples = numberOfSamples;
			info. numberOfSamplesRead = 0;
			info. buffer = buffer.peek();
			PaError err = Pa_OpenStream (& portaudioStream, & streamParameters, NULL,
				sampleRate, 0, paNoFlag, portaudioStreamCallback, (void *) & info);
			if (err)
				Melder_throw ("open ", Pa_GetErrorText (err));
			Pa_StartStream (portaudioStream);
			if (err)
				Melder_throw ("start ", Pa_GetErrorText (err));
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				waveFormat. cbSize = 0;
				err = waveInOpen (& hWaveIn, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL);
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while opening.");
			#endif
		}
for (i = 1; i <= numberOfSamples; i ++) trace ("Started %d", (int) buffer [i]);

		/* Read the sound into the buffer. */

		if (inputUsesPortAudio) {
			// The callback will do this. Just wait.
			while (/*getNumberOfSamplesRead (& info)*/ info. numberOfSamplesRead < numberOfSamples) {
				//Pa_Sleep (1);
				//Melder_casual ("filled %ld/%ld", getNumberOfSamplesRead (& info), numberOfSamples);
			}
for (i = 1; i <= numberOfSamples; i ++) trace ("Recorded %d", (int) buffer [i]);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				waveHeader. dwFlags = 0;
				waveHeader. lpData = (char *) & buffer [1];
				waveHeader. dwBufferLength = numberOfSamples * 2;
				waveHeader. dwLoops = 0;
				waveHeader. lpNext = NULL;
				waveHeader. reserved = 0;
				err = waveInPrepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while preparing header.");
				err = waveInAddBuffer (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while listening.");
				err = waveInStart (hWaveIn);
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while starting.");
					while (! (waveHeader. dwFlags & WHDR_DONE)) { Pa_Sleep (1); }
				err = waveInUnprepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while unpreparing header.");
			#else
				if (mulaw)
					read (fd, (char *) & buffer [1], numberOfSamples);
				else {
					long bytesLeft = 2 * numberOfSamples, dbytes, bytesRead = 0;
					while (bytesLeft) {
						//Melder_casual ("Reading %ld bytes", bytesLeft > 4000 ? 4000 : bytesLeft);
						dbytes = read (fd, & ((char *) buffer.peek()) [2 + bytesRead], bytesLeft > 4000 ? 4000 : bytesLeft);
						//Melder_casual("Read %ld bytes", dbytes);
						if (dbytes <= 0) break;
						bytesLeft -= dbytes;
						bytesRead += dbytes;
					};
				}
			#endif
		}

		/* Copy the buffered data to the sound object, and discard the buffer. */

		if (fakeMonoByStereo)
			for (i = 1; i <= numberOfSamples; i ++)
				my z [1] [i] = ((long) buffer [i + i - 1] + buffer [i + i]) * (1.0 / 65536);
		else if (mulaw)
			for (i = 1; i <= numberOfSamples; i ++)
				my z [1] [i] = ulaw2linear [((unsigned char *) buffer.peek()) [i]] * (1.0 / 32768);
		else if (can16bit)
			for (i = 1; i <= numberOfSamples; i ++)
				my z [1] [i] = buffer [i] * (1.0 / 32768);
		else
			for (i = 1; i <= numberOfSamples; i ++)
				my z [1] [i] = ((int) ((unsigned char *) buffer.peek()) [i + 1] - 128) * (1.0 / 128);

		/* Close the audio device. */

		if (inputUsesPortAudio) {
			Pa_StopStream (portaudioStream);
			Pa_CloseStream (portaudioStream);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				err = waveInClose (hWaveIn);
				if (err != MMSYSERR_NOERROR)
					Melder_throw ("Error ", err, " while closing.");
			#else
				close (fd);
			#endif
		}

		/* Hand the resulting sound to the caller. */

		return me.transfer();
	} catch (MelderError) {
		if (inputUsesPortAudio) {
			if (portaudioStream) Pa_StopStream (portaudioStream);
			if (portaudioStream) Pa_CloseStream (portaudioStream);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				if (hWaveIn != 0) waveInClose (hWaveIn);
			#else
				if (fd_mixer != -1) close (fd_mixer);
				if (fd != -1) close (fd);
			#endif
		}
		Melder_throw ("Sound not recorded.");
	}
}

/********** PLAYING A SOUND **********/

static struct SoundPlay {
	long numberOfSamples, i1, i2, silenceBefore, silenceAfter;
	double tmin, tmax, dt, t1;
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t);
	void *closure;
	short *buffer;
} thePlayingSound;

static bool melderPlayCallback (void *closure, long samplesPlayed) {
	struct SoundPlay *me = (struct SoundPlay *) closure;
	int phase = 2;
	double t = samplesPlayed <= my silenceBefore ? my tmin :
		samplesPlayed >= my silenceBefore + my numberOfSamples ? my tmax :
		my t1 + (my i1 - 1.5 + samplesPlayed - my silenceBefore) * my dt;
	if (! MelderAudio_isPlaying) {
		NUMvector_free (my buffer, 1), my buffer = NULL;
		phase = 3;
	}
	if (my callback)
		return my callback (my closure, phase, my tmin, my tmax, t);
	return true;
}

void Sound_playPart (Sound me, double tmin, double tmax,
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	try {
		long ifsamp = floor (1.0 / my dx + 0.5), bestSampleRate = MelderAudio_getOutputBestSampleRate (ifsamp);
		if (ifsamp == bestSampleRate) {
			struct SoundPlay *thee = (struct SoundPlay *) & thePlayingSound;
			double *fromLeft = my z [1], *fromRight = my ny > 1 ? my z [2] : NULL;
			MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
			long i1, i2;
			if ((thy numberOfSamples = Matrix_getWindowSamplesX (me, tmin, tmax, & i1, & i2)) < 1) return;
			thy tmin = tmin;
			thy tmax = tmax;
			thy dt = my dx;
			thy t1 = my x1;
			thy callback = callback;
			thy closure = closure;
			thy silenceBefore = (long) (ifsamp * MelderAudio_getOutputSilenceBefore ());
			thy silenceAfter = (long) (ifsamp * MelderAudio_getOutputSilenceAfter ());
			int numberOfChannels = my ny > 1 ? 2 : 1;
			NUMvector_free (thy buffer, 1);   // just in case
			thy buffer = NUMvector <short> (1, (i2 - i1 + 1 + thy silenceBefore + thy silenceAfter) * numberOfChannels);
			thy i1 = i1;
			thy i2 = i2;
			short *to = thy buffer + thy silenceBefore * numberOfChannels;
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
			MelderAudio_play16 (thy buffer + 1, ifsamp,
				thy silenceBefore + thy numberOfSamples + thy silenceAfter, numberOfChannels, melderPlayCallback, thee);
		} else {
			autoSound resampled = Sound_resample (me, bestSampleRate, 1);
			Sound_playPart (resampled.peek(), tmin, tmax, callback, closure);   // recursively
		}
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

void Sound_play (Sound me,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure)
{
	Sound_playPart (me, my xmin, my xmax, playCallback, playClosure);
}

/* End of file Sound_audio.cpp */
