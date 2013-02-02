/* melder_audio.cpp
 *
 * Copyright (C) 1992-2011,2012,2013 Paul Boersma
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
 * pb 2003/08/22 used getenv ("AUDIODEV") on Sun (thanks to Michel Scheffers)
 * pb 2003/10/22 fake mono for Linux drivers that do not support mono
 * pb 2003/12/06 use sys/soundcard.h instead of linux/soundcard.h for FreeBSD compatibility
 * pb 2004/05/07 removed motif_mac_setNullEventWaitingTime (we nowadays use 1 clock tick everywhere anyway)
 * pb 2004/08/10 fake mono for Linux drivers etc, also if not asynchronous
 * pb 2005/02/13 added O_NDELAY when opening /dev/dsp on Linux (suggestion by Rafael Laboissiere)
 * pb 2005/03/31 undid previous change (four complaints that sound stopped playing)
 * pb 2005/05/19 redid previous change (with fctrl fix suggested by Rafael Laboissiere)
 * pb 2005/10/13 edition for OpenBSD
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/16 Macintosh uses CoreAudio (via PortAudio)
 * pb 2007/01/03 best sample rate can be over 64 kHz
 * pb 2007/05/13 null pointer test for deviceInfo (thanks to Stefan de Konink)
 * pb 2007/08/12 wchar_t
 * Stefan de Konink 2007/12/02 big-endian Linux
 * pb 2007/12/04 enums
 * pb 2008/06/01 removed SPEXLAB audio server
 * pb 2008/06/10 made PortAudio and foreground playing optional
 * pb 2008/07/03 DirectSound
 * pb 2010/05/09 GTK
 * pb 2011/02/11 better message
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include "Gui.h"
#include "Preferences.h"
#include "NUM.h"
#include <time.h>
#define my  me ->

#if defined (macintosh)
	#include <sys/time.h>
	#include <math.h>
#elif defined (_WIN32)
	#include <windows.h>
	#include <math.h>
#elif defined (linux)
	#include <sys/time.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
	#if defined (__OpenBSD__) || defined (__NetBSD__)
		#include <soundcard.h>
	#else
		#include <sys/soundcard.h>
	#endif
	#include <errno.h>
#endif

#include "../external/portaudio/portaudio.h"

static struct {
	enum kMelder_asynchronicityLevel maximumAsynchronicity;
	bool useInternalSpeaker, inputUsesPortAudio, outputUsesPortAudio, outputUsesBlocking;
	double silenceBefore, silenceAfter;
} preferences;

void Melder_audio_prefs (void) {
	Preferences_addEnum (L"Audio.maximumAsynchronicity", & preferences. maximumAsynchronicity, kMelder_asynchronicityLevel, kMelder_asynchronicityLevel_DEFAULT);
	Preferences_addBool (L"Audio.useInternalSpeaker", & preferences. useInternalSpeaker, true);
	Preferences_addBool (L"Audio.outputUsesPortAudio2", & preferences. outputUsesPortAudio, kMelderAudio_outputUsesPortAudio_DEFAULT);
	Preferences_addBool (L"Audio.outputUsesBlocking2", & preferences. outputUsesBlocking, false);
	Preferences_addDouble (L"Audio.silenceBefore", & preferences. silenceBefore, kMelderAudio_outputSilenceBefore_DEFAULT);
	Preferences_addDouble (L"Audio.silenceAfter", & preferences. silenceAfter, kMelderAudio_outputSilenceAfter_DEFAULT);
	Preferences_addBool (L"Audio.inputUsesPortAudio2", & preferences. inputUsesPortAudio, kMelderAudio_inputUsesPortAudio_DEFAULT);
}

void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity) {
	//MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // BUG
	preferences. maximumAsynchronicity = maximumAsynchronicity;
}
enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity (void) { return preferences. maximumAsynchronicity; }

void MelderAudio_setInputUsesPortAudio (bool inputUsesPortAudio) {
	preferences. inputUsesPortAudio = inputUsesPortAudio;
}
bool MelderAudio_getInputUsesPortAudio (void) { return preferences. inputUsesPortAudio; }

void MelderAudio_setOutputUsesPortAudio (bool outputUsesPortAudio) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. outputUsesPortAudio = outputUsesPortAudio;
}
bool MelderAudio_getOutputUsesPortAudio (void) { return preferences. outputUsesPortAudio; }

void MelderAudio_setOutputUsesBlocking (bool outputUsesBlocking) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. outputUsesBlocking = outputUsesBlocking;
}
bool MelderAudio_getOutputUsesBlocking (void) { return preferences. outputUsesBlocking; }

void MelderAudio_setUseInternalSpeaker (bool useInternalSpeaker) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. useInternalSpeaker = useInternalSpeaker;
}
bool MelderAudio_getUseInternalSpeaker (void) { return preferences. useInternalSpeaker; }

void MelderAudio_setOutputSilenceBefore (double silenceBefore) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. silenceBefore = silenceBefore;
}
double MelderAudio_getOutputSilenceBefore (void) { return preferences. silenceBefore; }

void MelderAudio_setOutputSilenceAfter (double silenceAfter) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. silenceAfter = silenceAfter;
}
double MelderAudio_getOutputSilenceAfter (void) { return preferences. silenceAfter; }

long MelderAudio_getOutputBestSampleRate (long fsamp) {
	#if defined (macintosh)
		return fsamp;
	#elif defined (_WIN32)
		return fsamp == 8000 || fsamp == 11025 || fsamp == 16000 || fsamp == 22050 ||
			fsamp == 32000 || fsamp == 44100 || fsamp == 48000 || fsamp == 96000 ? fsamp : 44100;
	#elif defined (linux)
		return fsamp == 44100 || fsamp == 48000 || fsamp == 96000 ? fsamp : 44100;
	#else
		return 44100;
	#endif
}

bool MelderAudio_isPlaying;

static double theStartingTime = 0.0;

static struct MelderPlay {
	const int16_t *buffer;
	long sampleRate, numberOfSamples, samplesLeft, samplesSent, samplesPlayed;
	unsigned int asynchronicity;
	int numberOfChannels;
	bool explicitStop, fakeMono;
	bool (*callback) (void *closure, long samplesPlayed);
	void *closure;
	#if motif
		XtWorkProcId workProcId_motif;
	#elif gtk
		gint workProcId_gtk;
	#endif
	bool usePortAudio, blocking, supports_paComplete;
	PaStream *stream;
	double paStartingTime;
	#if defined (macintosh)
	#elif defined (linux)
		int audio_fd, val, err;
	#elif defined (_WIN32)
		HWAVEOUT hWaveOut;
		WAVEHDR waveHeader;
		MMRESULT status;
	#endif
} thePlay;

long MelderAudio_getSamplesPlayed (void) {
	return thePlay. samplesPlayed;
}

bool MelderAudio_stopWasExplicit (void) {
	return thePlay. explicitStop;
}

/*
 * The flush () procedure will always have to be invoked after normal play, i.e. in the following cases:
 * 1. After synchronous play (asynchronicity = 0, 1, or 2).
 * 2. After interruption of asynchronicity 2 by the ESCAPE key.
 * 3. After asynchronous play, by the workProc.
 * 4. After interruption of asynchronicity 3 by MelderAudio_stopPlaying ().
 */
static bool flush (void) {
	struct MelderPlay *me = & thePlay;
	if (my usePortAudio) {
		if (my stream != NULL) Pa_CloseStream (my stream), my stream = NULL;
	} else {
	#if defined (macintosh)
	#elif defined (linux)
		/*
		 * As on Sun.
		 */
		if (my audio_fd) {
			ioctl (my audio_fd, SNDCTL_DSP_RESET, (my val = 0, & my val));
			close (my audio_fd), my audio_fd = 0;
		}
	#elif defined (_WIN32)
		/*
		 * FIX: Do not reset the sound card if played to the end:
		 * the last 20 milliseconds may be truncated!
		 * This used to happen on Barbertje's Dell PC, not with SoundBlaster.
		 */
		if (my samplesPlayed != my numberOfSamples || Melder_debug == 2)
			waveOutReset (my hWaveOut);
		my status = waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
		if (/* Melder_debug == 3 && */ my status == WAVERR_STILLPLAYING) {
			waveOutReset (my hWaveOut);
			waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
		}
		waveOutClose (my hWaveOut), my hWaveOut = 0;
	#endif
	}
	if (my fakeMono) {
		NUMvector_free ((short *) my buffer, 0);
		my buffer = NULL;
	}
	MelderAudio_isPlaying = false;
	if (my samplesPlayed >= my numberOfSamples)
		my samplesPlayed = my numberOfSamples;
	if (my samplesPlayed <= 0)
		my samplesPlayed = 1;
	/*
	 * Call the callback for the last time, which is recognizable by the value of MelderAudio_isPlaying.
	 * In this way, the caller of Melder_play16 can be notified.
	 * The caller can examine the actual number of samples played by testing MelderAudio_getSamplesPlayed ().
	 */
	if (my callback)
		my callback (my closure, my samplesPlayed);
	my callback = 0;
	my closure = 0;
	return true;   /* Remove workProc if called from workProc. */
}

bool MelderAudio_stopPlaying (bool explicitStop) {
	//Melder_casual ("stop playing!");
	struct MelderPlay *me = & thePlay;
	my explicitStop = explicitStop;
	if (! MelderAudio_isPlaying || my asynchronicity < kMelder_asynchronicityLevel_ASYNCHRONOUS) return false;
	(void) flush ();
	#if motif
		XtRemoveWorkProc (thePlay. workProcId_motif);
	#elif gtk
		gtk_idle_remove (thePlay. workProcId_gtk);
	#endif
	return true;
}

static bool workProc (void *closure) {
	struct MelderPlay *me = & thePlay;
//static long n = 0;
//n ++;
//Melder_casual("workProc %ld", n);
	if (my usePortAudio) {
		if (my blocking) {
			if (my samplesLeft > 0) {
				int dsamples = my samplesLeft > 2000 ? 2000 : my samplesLeft;
				Pa_WriteStream (my stream, (void *) & my buffer [my samplesSent * my numberOfChannels], dsamples);
				my samplesLeft -= dsamples;
				my samplesSent += dsamples;
				my samplesPlayed = (Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency) * my sampleRate;
				if (my callback && ! my callback (my closure, my samplesPlayed))
					return flush ();
			} else /*if (my samplesPlayed >= my numberOfSamples)*/ {
				Pa_StopStream (my stream);
				my samplesPlayed = my numberOfSamples;
				return flush ();
			}
		} else {
			/*
			 * Not all hostApis support paComplete or wait till all buffers have been played in Pa_StopStream.
			 * Once pa_win_ds implements this, we can simply do:
			 * if (Pa_IsStreamActive (my stream)) {
			 *    if (my callback && ! my callback (my closure, my samplesPlayed))
			 *       return flush ();
			 * } else {
			 *    my samplesPlayed = my numberOfSamples;
			 *    return flush ();
			 * }
			 * But then we also have to use paComplete in the stream callback.
			 */
			double timeElapsed = Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency;
			my samplesPlayed = timeElapsed * my sampleRate;
			if (my supports_paComplete && Pa_IsStreamActive (my stream)) {
				if (my callback && ! my callback (my closure, my samplesPlayed)) {
					Pa_AbortStream (my stream);
					return flush ();
				}
			} else if (my samplesPlayed < my numberOfSamples + my sampleRate / 20) {   // allow the latency estimate to be 50 ms off.
				if (my callback && ! my callback (my closure, my samplesPlayed)) {
					Pa_AbortStream (my stream);
					return flush ();
				}
			} else {
				Pa_AbortStream (my stream);
				my samplesPlayed = my numberOfSamples;
				return flush ();
			}
			Pa_Sleep (10);
		}
	} else {
	#if defined (macintosh)
	#elif defined (linux)
		if (my samplesLeft > 0) {
			int dsamples = my samplesLeft > 500 ? 500 : my samplesLeft;
			write (my audio_fd, (char *) & my buffer [my samplesSent * my numberOfChannels], 2 * dsamples * my numberOfChannels);
			my samplesLeft -= dsamples;
			my samplesSent += dsamples;
			my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
			if (my callback && ! my callback (my closure, my samplesPlayed))
				return flush ();
		} else /*if (my samplesPlayed >= my numberOfSamples)*/ {
			close (my audio_fd), my audio_fd = 0;
			my samplesPlayed = my numberOfSamples;
			return flush ();
  		/*} else {
			my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
			if (my callback && ! my callback (my closure, my samplesPlayed))
				return flush ();*/
		}
	#elif defined (_WIN32)
  		if (my waveHeader. dwFlags & WHDR_DONE) {
			my samplesPlayed = my numberOfSamples;
			return flush ();
  		} else {
  			static long previousTime = 0;
  			unsigned long currentTime = clock ();
  			if (Melder_debug == 1) {
				my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
  			} else {
	  			MMTIME mmtime;
	  			mmtime. wType = TIME_BYTES;
	  			waveOutGetPosition (my hWaveOut, & mmtime, sizeof (MMTIME));
				my samplesPlayed = mmtime. u.cb / (2 * my numberOfChannels);
			}
			if (/* Melder_debug != 4 || */ currentTime - previousTime > CLOCKS_PER_SEC / 100) {
				previousTime = currentTime;
				if (my callback && ! my callback (my closure, my samplesPlayed))
					return flush ();
			}
			Sleep (10);
  		}
	#endif
	}
	(void) closure;
	return false;
}
#if motif
static bool workProc_motif (XtPointer closure) {
	return workProc ((void *) closure);
}
#elif gtk
static gint workProc_gtk (gpointer closure) {
	return ! workProc ((void *) closure);
}
#endif

#if defined (macintosh)
# define FloatToUnsigned(f)  \
	 ((unsigned long)(((long)((f) - 2147483648.0)) + 2147483647L + 1))
static void double2real10 (double x, unsigned char *bytes) {
	int sign, exponent;
	double fMantissa, fsMantissa;
	unsigned long highMantissa, lowMantissa;
	if (x < 0.0) { sign = 0x8000; x *= -1; }
	else sign = 0;
	if (x == 0.0) { exponent = 0; highMantissa = 0; lowMantissa = 0; }
	else {
		fMantissa = frexp (x, & exponent);
		if ((exponent > 16384) || ! (fMantissa < 1))   /* Infinity or Not-a-Number. */
			{ exponent = sign | 0x7FFF; highMantissa = 0; lowMantissa = 0; }   /* Infinity. */
		else {   /* Finite */
			exponent += 16382;   /* Add bias. */
			if (exponent < 0) {   /* Denormalized. */
				fMantissa = ldexp (fMantissa, exponent);
				exponent = 0;
			}
			exponent |= sign;
			fMantissa = ldexp (fMantissa, 32);          
			fsMantissa = floor (fMantissa); 
			highMantissa = FloatToUnsigned (fsMantissa);
			fMantissa = ldexp (fMantissa - fsMantissa, 32); 
			fsMantissa = floor (fMantissa); 
			lowMantissa = FloatToUnsigned (fsMantissa);
		}
	}
	bytes [0] = exponent >> 8;
	bytes [1] = exponent;
	bytes [2] = highMantissa >> 24;
	bytes [3] = highMantissa >> 16;
	bytes [4] = highMantissa >> 8;
	bytes [5] = highMantissa;
	bytes [6] = lowMantissa >> 24;
	bytes [7] = lowMantissa >> 16;
	bytes [8] = lowMantissa >> 8;
	bytes [9] = lowMantissa;
}
#endif

static int thePaStreamCallback (const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	(void) input;
	(void) timeInfo;
	(void) userData;
	struct MelderPlay *me = & thePlay;
	if (statusFlags & paOutputUnderflow) {
		if (Melder_debug == 20) Melder_casual ("output underflow");
	}
	if (statusFlags & paOutputOverflow) {
		if (Melder_debug == 20) Melder_casual ("output overflow");
	}
	if (my samplesLeft > 0) {
		long dsamples = my samplesLeft > (long) frameCount ? (long) frameCount : my samplesLeft;
		if (Melder_debug == 20) Melder_casual ("play %ls %ls", Melder_integer (dsamples),
			Melder_double (Pa_GetStreamCpuLoad (my stream)));
		memset (output, '\0', 2 * frameCount * my numberOfChannels);
		memcpy (output, (char *) & my buffer [my samplesSent * my numberOfChannels], 2 * dsamples * my numberOfChannels);
		my samplesLeft -= dsamples;
		my samplesSent += dsamples;
		my samplesPlayed = my samplesSent;
	} else /*if (my samplesPlayed >= my numberOfSamples)*/ {
		memset (output, '\0', 2 * frameCount * my numberOfChannels);
		my samplesPlayed = my numberOfSamples;
		return my supports_paComplete ? paComplete : paContinue;
	}
	return paContinue;
}

void MelderAudio_play16 (const int16_t *buffer, long sampleRate, long numberOfSamples, int numberOfChannels,
	bool (*playCallback) (void *playClosure, long numberOfSamplesPlayed), void *playClosure)
{
	struct MelderPlay *me = & thePlay;
	#ifdef _WIN32
		bool wasPlaying = MelderAudio_isPlaying;
	#endif
	if (MelderAudio_isPlaying) MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // otherwise, keep "explicitStop" tag
	my buffer = buffer;
	my sampleRate = sampleRate;
	my numberOfSamples = numberOfSamples;
	my numberOfChannels = numberOfChannels;
	my callback = playCallback;
	my closure = playClosure;
	my asynchronicity =
		Melder_batch ? kMelder_asynchronicityLevel_SYNCHRONOUS :
		Melder_backgrounding ? kMelder_asynchronicityLevel_INTERRUPTABLE :
		kMelder_asynchronicityLevel_ASYNCHRONOUS;
	if (my asynchronicity > preferences. maximumAsynchronicity)
		my asynchronicity = preferences. maximumAsynchronicity;
	my usePortAudio = preferences. outputUsesPortAudio;
	my blocking = preferences. outputUsesBlocking;
	my explicitStop = MelderAudio_IMPLICIT;
	my fakeMono = false;

	my samplesLeft = numberOfSamples;
	my samplesSent = 0;
	my samplesPlayed = 0;
	MelderAudio_isPlaying = true;
	if (my usePortAudio) {
		PaError err;
		static bool paInitialized = false;
		if (! paInitialized) {
			err = Pa_Initialize ();
			if (err) Melder_fatal ("PortAudio does not initialize: %s", Pa_GetErrorText (err));
			paInitialized = true;
		}
		my supports_paComplete = Pa_GetHostApiInfo (Pa_GetDefaultHostApi ()) -> type != paDirectSound;
		PaStreamParameters outputParameters = { 0 };
		outputParameters. device = Pa_GetDefaultOutputDevice ();
		const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo (outputParameters. device);
		outputParameters. channelCount = my numberOfChannels;
		outputParameters. sampleFormat = paInt16;
		if (deviceInfo != NULL) outputParameters. suggestedLatency = deviceInfo -> defaultLowOutputLatency;
		outputParameters. hostApiSpecificStreamInfo = NULL;
		err = Pa_OpenStream (& my stream, NULL, & outputParameters, my sampleRate, paFramesPerBufferUnspecified,
			paDitherOff, my blocking ? NULL : thePaStreamCallback, me);
		if (err) Melder_throw ("PortAudio cannot open sound output: ", Pa_GetErrorText (err), ".");
		theStartingTime = Melder_clock ();
		if (my blocking) {
			err = Pa_StartStream (my stream);
			if (err) Melder_throw ("PortAudio cannot start sound output: ", Pa_GetErrorText (err), ".");
			if (my asynchronicity == kMelder_asynchronicityLevel_SYNCHRONOUS) {
				Pa_WriteStream (my stream, buffer, numberOfSamples);
				Pa_StopStream (my stream);
				my samplesPlayed = my numberOfSamples;
			} else if (my asynchronicity <= kMelder_asynchronicityLevel_INTERRUPTABLE) {
				bool interrupted = false;
				while (my samplesLeft > 0 && ! interrupted) {
					long numberOfWriteSamplesAvailableWithoutBlocking = Pa_GetStreamWriteAvailable (my stream);
					if (Melder_debug == 20)
						Melder_casual ("Pa_GetStreamWriteAvailable: %ld", numberOfWriteSamplesAvailableWithoutBlocking);
					long maximumNumberOfSamplesToWrite = 1000;
					long dsamples = my samplesLeft > maximumNumberOfSamplesToWrite ? maximumNumberOfSamplesToWrite : my samplesLeft;
					Pa_WriteStream (my stream, (void *) & my buffer [my samplesSent * my numberOfChannels], dsamples);
					my samplesLeft -= dsamples;
					my samplesSent += dsamples;
					my samplesPlayed = (Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency) * my sampleRate;
					if (my callback && ! my callback (my closure, my samplesPlayed))
						interrupted = true;
					if (my asynchronicity == kMelder_asynchronicityLevel_INTERRUPTABLE && ! interrupted) {
						#if defined (macintosh)
							#if useCarbon
								EventRecord event;
								if (EventAvail (keyDownMask, & event)) {
									/*
									* Remove the event, even if it was a different key.
									* Otherwise, the key will block the future availability of the Escape key.
									*/
									FlushEvents (keyDownMask, 0);
									/*
									* Catch Escape and Command-period.
									*/
									if ((event. message & charCodeMask) == 27 ||
										((event. modifiers & cmdKey) && (event. message & charCodeMask) == '.'))
									{
										my explicitStop = MelderAudio_EXPLICIT;
										interrupted = true;
									}
								}
							#else
							#endif
						#elif defined (_WIN32)
							MSG event;
							if (PeekMessage (& event, 0, 0, 0, PM_REMOVE) && event. message == WM_KEYDOWN) {
								if (LOWORD (event. wParam) == VK_ESCAPE) {
									my explicitStop = MelderAudio_EXPLICIT;
									interrupted = true;
								}
							}
						#endif
					}				
					if (interrupted) {
						flush ();
						return;
					}
				}
				Pa_StopStream (my stream);
			} else {
				#if motif
					my workProcId_motif = GuiAddWorkProc (workProc_motif, NULL);
				#elif gtk
					my workProcId_gtk = gtk_idle_add (workProc_gtk, NULL);
				#endif
				return;
			}
		} else {
			err = Pa_StartStream (my stream);
			if (err) Melder_throw ("PortAudio cannot start sound output: ", Pa_GetErrorText (err), ".");
			my paStartingTime = Pa_GetStreamTime (my stream);
			if (my asynchronicity <= kMelder_asynchronicityLevel_INTERRUPTABLE) {
				for (;;) {
					double timeElapsed = Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency;
					my samplesPlayed = timeElapsed * my sampleRate;
					if (my samplesPlayed >= my numberOfSamples + my sampleRate / 20) {
						my samplesPlayed = my numberOfSamples;
						break;
					}
					bool interrupted = false;
					if (my asynchronicity != kMelder_asynchronicityLevel_SYNCHRONOUS && my callback &&
						! my callback (my closure, my samplesPlayed))
						interrupted = true;
					/*
					 * Safe operation: only listen to key-down events.
					 * Do this on the lowest level that will work.
					 */
					if (my asynchronicity == kMelder_asynchronicityLevel_INTERRUPTABLE && ! interrupted) {
						#if defined (macintosh)
							#if useCarbon
								EventRecord event;
								if (EventAvail (keyDownMask, & event)) {
									/*
									* Remove the event, even if it was a different key.
									* Otherwise, the key will block the future availability of the Escape key.
									*/
									FlushEvents (keyDownMask, 0);
									/*
									* Catch Escape and Command-period.
									*/
									if ((event. message & charCodeMask) == 27 ||
										((event. modifiers & cmdKey) && (event. message & charCodeMask) == '.'))
									{
										my explicitStop = MelderAudio_EXPLICIT;
										interrupted = true;
									}
								}
							#else
							#endif
						#elif defined (_WIN32)
							MSG event;
							if (PeekMessage (& event, 0, 0, 0, PM_REMOVE) && event. message == WM_KEYDOWN) {
								if (LOWORD (event. wParam) == VK_ESCAPE) {
									my explicitStop = MelderAudio_EXPLICIT;
									interrupted = true;
								}
							}
						#endif
					}
					if (interrupted) {
						flush ();
						return;
					}
					Pa_Sleep (10);
				}
				if (my samplesPlayed != my numberOfSamples) {
					Melder_fatal ("Played %ld instead of %ld samples.", my samplesPlayed, my numberOfSamples);
				}
				Pa_AbortStream (my stream);
			} else /* my asynchronicity == kMelder_asynchronicityLevel_ASYNCHRONOUS */ {
				#if motif
					my workProcId_motif = GuiAddWorkProc (workProc_motif, NULL);
				#elif gtk
					my workProcId_gtk = gtk_idle_add (workProc_gtk, NULL);
				#endif
				return;
			}
		}
		flush ();
		return;
	} else {
		#if defined (macintosh)
		#elif defined (linux)
			try {
				/* Big-endian version added by Stefan de Konink, Nov 29, 2007 */
				#if __BYTE_ORDER == __BIG_ENDIAN
					int fmt = AFMT_S16_BE;
				#else
					int fmt = AFMT_S16_LE;
				#endif
				/* O_NDELAY option added by Rafael Laboissiere, May 19, 2005 */
				if ((my audio_fd = open ("/dev/dsp", O_WRONLY | (Melder_debug == 16 ? 0 : O_NDELAY))) == -1) {
					Melder_throw (errno == EBUSY ? "Audio device already in use." :
						"Cannot open audio device.\nPlease switch on PortAudio in Praat's Sound Playing Preferences.");
				}
				fcntl (my audio_fd, F_SETFL, 0);   /* Added by Rafael Laboissiere, May 19, 2005 */
				if (ioctl (my audio_fd, SNDCTL_DSP_SETFMT,   /* Changed SND_DSP_SAMPLESIZE to SNDCTL_DSP_SETFMT; Stefan de Konink, Nov 29, 2007 */
					(my val = fmt, & my val)) == -1 ||   /* Error? */
					my val != fmt)   /* Has sound card overridden our sample size? */
				{
					Melder_throw ("Cannot set sample size to 16 bit.");
				}
				if (ioctl (my audio_fd, SNDCTL_DSP_CHANNELS, (my val = my numberOfChannels, & my val)) == -1 ||   /* Error? */
					my val != my numberOfChannels)   /* Has sound card overridden our number of channels? */
				{
					/*
					 * There is one specific case in which we can work around the current failure,
					 * namely when we are trying to play in mono but the driver of the sound card supports stereo only
					 * and notified us of this by overriding our number of channels.
					 */
					if (my numberOfChannels == 1 && my val == 2) {
						my fakeMono = true;
						int16_t *newBuffer = NUMvector <int16_t> (0, 2 * numberOfSamples - 1);
						for (long isamp = 0; isamp < numberOfSamples; isamp ++) {
							newBuffer [isamp + isamp] = newBuffer [isamp + isamp + 1] = buffer [isamp];
						}
						my buffer = (const int16_t *) newBuffer;
						my numberOfChannels = 2;
					} else {
						Melder_throw ("Cannot set number of channels to .", my numberOfChannels, ".");
					}
				}
				if (ioctl (my audio_fd, SNDCTL_DSP_SPEED, (my val = my sampleRate, & my val)) == -1 ||    // error?
					my val != my sampleRate)   /* Has sound card overridden our sampling frequency? */
				{
					Melder_throw ("Cannot set sampling frequency to ", my sampleRate, " Hz.");
				}

				theStartingTime = Melder_clock ();
				if (my asynchronicity == kMelder_asynchronicityLevel_SYNCHRONOUS) {
					if (write (my audio_fd, & my buffer [0], 2 * numberOfChannels * numberOfSamples) == -1)
						Melder_throw ("Cannot write audio output.");
					close (my audio_fd), my audio_fd = 0;   /* Drain. Set to zero in order to notify flush (). */
					my samplesPlayed = my numberOfSamples;
				} else if (my asynchronicity <= kMelder_asynchronicityLevel_INTERRUPTABLE) {
					bool interrupted = false;
					while (my samplesLeft && ! interrupted) {
						int dsamples = my samplesLeft > 500 ? 500 : my samplesLeft;
						if (write (my audio_fd, (char *) & my buffer [my samplesSent * my numberOfChannels], 2 * dsamples * my numberOfChannels) == -1)
							Melder_throw ("Cannot write audio output.");
						my samplesLeft -= dsamples;
						my samplesSent += dsamples;
						my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
						if (my callback && ! my callback (my closure, my samplesPlayed))
							interrupted = true;
					}
					if (! interrupted) {
						/*
						 * Wait for playing to end.
						 */
						close (my audio_fd), my audio_fd = 0;   // BUG: should do a loop
						my samplesPlayed = my numberOfSamples;
					}
				} else /* my asynchronicity == kMelder_asynchronicityLevel_ASYNCHRONOUS */ {
					my workProcId_gtk = gtk_idle_add (workProc_gtk, NULL);
					return;
				}
				flush ();
				return;
			} catch (MelderError) {
				struct MelderPlay *me = & thePlay;
				if (my audio_fd) close (my audio_fd), my audio_fd = 0;
				MelderAudio_isPlaying = false;
				Melder_throw ("16-bit audio not played.");
			}
		#elif defined (_WIN32)
			try {
				WAVEFORMATEX waveFormat;
				MMRESULT err;
				waveFormat. wFormatTag = WAVE_FORMAT_PCM;
				waveFormat. nChannels = my numberOfChannels;
				waveFormat. nSamplesPerSec = my sampleRate;
				waveFormat. wBitsPerSample = 16;
				waveFormat. nBlockAlign = my numberOfChannels * waveFormat. wBitsPerSample / 8;
				waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
				waveFormat. cbSize = 0;
				err = waveOutOpen (& my hWaveOut, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC);
				if (err != MMSYSERR_NOERROR) {
					MelderAudio_isPlaying = false;
					if (err == MMSYSERR_ALLOCATED)
						Melder_throw ("Previous sound is still playing? Should not occur!\n"
							"Report bug to the author: ", err, "; wasPlaying: ", wasPlaying, ".");
					if (err == MMSYSERR_BADDEVICEID)
						Melder_throw ("Cannot play a sound. Perhaps another program is playing a sound at the same time?");
					if (err == MMSYSERR_NODRIVER)
						Melder_throw ("This computer probably has no sound card.");
					if (err == MMSYSERR_NOMEM)
						Melder_throw ("Not enough free memory to play any sound at all.");
					if (err == WAVERR_BADFORMAT)
						Melder_throw ("Bad sound format? Should not occur! Report bug to the author!");
					Melder_throw ("Unknown error ", err, " while trying to play a sound? Report bug to the author!");
				}

				my waveHeader. dwFlags = 0;
				my waveHeader. lpData = (char *) my buffer;
				my waveHeader. dwBufferLength = my numberOfSamples * 2 * my numberOfChannels;
				my waveHeader. dwLoops = 1;
				my waveHeader. lpNext = NULL;
				my waveHeader. reserved = 0;
				err = waveOutPrepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
			//waveOutReset (my hWaveOut);
				if (err != MMSYSERR_NOERROR) {
					waveOutClose (my hWaveOut), my hWaveOut = 0;
					MelderAudio_isPlaying = false;
					if (err == MMSYSERR_INVALHANDLE)
						Melder_throw ("No device? Should not occur!");
					if (err == MMSYSERR_NODRIVER)
						Melder_throw ("No driver? Should not occur!");
					if (err == MMSYSERR_NOMEM)
						Melder_throw ("Not enough free memory to play this sound.\n"
							"Remove some objects, play a shorter sound, or buy more memory.");
					Melder_throw ("Unknown error ", err, " while preparing header? Should not occur!");
				}

				err = waveOutWrite (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR) {
					waveOutReset (my hWaveOut);
					waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
					waveOutClose (my hWaveOut), my hWaveOut = 0;
					MelderAudio_isPlaying = false;
					Melder_throw ("Error ", err, " while writing audio output.");   // BUG: should flush
				}

				theStartingTime = Melder_clock ();
				if (my asynchronicity == kMelder_asynchronicityLevel_SYNCHRONOUS) {
					while (! (my waveHeader. dwFlags & WHDR_DONE)) {
						Sleep (10);
					}
					my samplesPlayed = my numberOfSamples;
				} else if (my asynchronicity <= kMelder_asynchronicityLevel_INTERRUPTABLE) {
					while (! (my waveHeader. dwFlags & WHDR_DONE)) {
						MSG event;
						Sleep (10);
						my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
						if (my callback && ! my callback (my closure, my samplesPlayed))
							break;
						if (my asynchronicity == kMelder_asynchronicityLevel_INTERRUPTABLE &&
							PeekMessage (& event, 0, 0, 0, PM_REMOVE) && event. message == WM_KEYDOWN)
						{
							if (LOWORD (event. wParam) == VK_ESCAPE) {
								my explicitStop = MelderAudio_EXPLICIT;
								break;
							}
						}
					}
				} else {
					my workProcId_motif = GuiAddWorkProc (workProc_motif, NULL);
					return;
				}
				flush ();
				return;
			} catch (MelderError) {
				Melder_throw ("16-bit audio not played.");
			}
		#else
			Melder_throw ("Cannot play a sound on this computer.\n16-bit audio not played.");
		#endif
	}
}

/********** WAITING FOR SOUND INPUT **********/

void Melder_audioTrigger (void) {
}

/* End of file melder_audio.cpp */
