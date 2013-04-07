/* SoundRecorder.cpp
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
/* Linux code originally by Darryl Purnell, Pretoria */
/* GTK conversion includes work by Franz Brauße */

/* This source file describes interactive sound recorders for the following systems:
 *     MacOS
 *     Linux
 *     Windows
 * Because the behaviour of these sound recorders is partly similar, partly different,
 * this would seem a good candidate for object-oriented programming
 * (one audio manager and several audio drivers).
 * However, the places where sound recorders are similar and where they are different,
 * are hard to predict. For this reason, everything is done with system #ifdefs.
 */

#include <errno.h>
#include "SoundRecorder.h"
#include "Sound_and_Spectrum.h"
#include "machine.h"
#include "EditorM.h"
#if defined (macintosh)
	#include "pa_mac_core.h"
#endif

#include "enums_getText.h"
#include "SoundRecorder_enums.h"
#include "enums_getValue.h"
#include "SoundRecorder_enums.h"

Thing_implement (SoundRecorder, Editor, 0);

#include "prefs_define.h"
#include "SoundRecorder_prefs.h"
#include "prefs_install.h"
#include "SoundRecorder_prefs.h"
#include "prefs_copyToInstance.h"
#include "SoundRecorder_prefs.h"

static struct {
	int bufferSizeInMegabytes;
} preferences;

void SoundRecorder_preferences () {
	Preferences_addInt (L"SoundRecorder.bufferSizeInMegabytes", & preferences.bufferSizeInMegabytes, 60);
}

int SoundRecorder_getBufferSizePref_MB () { return preferences.bufferSizeInMegabytes; }
void SoundRecorder_setBufferSizePref_MB (int size) { preferences.bufferSizeInMegabytes = size < 1 ? 1 : size > 1000 ? 1000: size; }

#define step 1000

/* For those systems that do not have a pollable audio control panel, */
/* the settings are saved only here, so that they are remembered across */
/* subsequent creations of a SoundRecorder. Also, this is then the way */
/* in which two simultaneously open SoundRecorders would communicate. */

static struct {
	int inputSource;   // 1 = microphone, 2 = line, 3 = digital
	int leftGain, rightGain;   // 0..255
	double sampleRate;
} theControlPanel =
#if defined (linux)
	{ 1, 200, 200, 44100 };
#elif defined (macintosh)
	{ 1, 26, 26, 44100 };
#else
	{ 1, 26, 26, 44100 };
#endif

/********** ERROR HANDLING **********/

#if defined (_WIN32)
static void win_fillFormat (SoundRecorder me) {
	my waveFormat. nSamplesPerSec = (int) theControlPanel. sampleRate;
	my waveFormat. nChannels = my numberOfChannels;
	my waveFormat. wFormatTag = WAVE_FORMAT_PCM;
	my waveFormat. wBitsPerSample = 16;
	my waveFormat. nBlockAlign = my waveFormat. nChannels * my waveFormat. wBitsPerSample / 8;
	my waveFormat. nAvgBytesPerSec = my waveFormat. nBlockAlign * my waveFormat. nSamplesPerSec;
	my waveFormat. cbSize = 0;
}
static void win_fillHeader (SoundRecorder me, int which) {
	my waveHeader [which]. dwFlags = 0;
	my waveHeader [which]. lpData = which == 0 ? (char *) my buffer : which == 1 ? (char *) my buffertje1: (char *) my buffertje2;
	my waveHeader [which]. dwBufferLength = which == 0 ? my nmax * my waveFormat. nChannels * 2 : 1000 * my waveFormat. nChannels * 2;
	my waveHeader [which]. dwLoops = 0;
	my waveHeader [which]. lpNext = NULL;
	my waveHeader [which]. reserved = 0;
}
static void win_waveInCheck (SoundRecorder me) {
	wchar_t messageText [MAXERRORLENGTH];
	MMRESULT err;
	if (my err == MMSYSERR_NOERROR) return;
	err = waveInGetErrorText (my err, messageText, MAXERRORLENGTH);
	if (err == MMSYSERR_NOERROR) Melder_throw (messageText);
	else if (err == MMSYSERR_BADERRNUM) Melder_throw ("Error number ", my err, " out of range.");
	else if (err == MMSYSERR_NODRIVER) Melder_throw ("No sound driver present.");
	else if (err == MMSYSERR_NOMEM) Melder_throw ("Out of memory.");
	else Melder_throw ("Unknown sound error.");
}
static void win_waveInOpen (SoundRecorder me) {
	try {
		my err = waveInOpen (& my hWaveIn, WAVE_MAPPER, & my waveFormat, 0, 0, CALLBACK_NULL);
		win_waveInCheck (me);
		if (Melder_debug != 8) waveInReset (my hWaveIn);
	} catch (MelderError) {
		Melder_throw ("Audio input not opened.");
	}
}
static void win_waveInPrepareHeader (SoundRecorder me, int which) {
	try {
		my err = waveInPrepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input: cannot prepare header.\nQuit some other programs or go to \"Sound input prefs\" in the Preferences menu.");
	}
}
static void win_waveInAddBuffer (SoundRecorder me, int which) {
	try {
		my err = waveInAddBuffer (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input: cannot add buffer.");
	}
}
static void win_waveInStart (SoundRecorder me) {
	try {
		my err = waveInStart (my hWaveIn);   // asynchronous
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input not started.");
	}
}
static void win_waveInStop (SoundRecorder me) {
	try {
		my err = waveInStop (my hWaveIn);
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input not stopped.");
	}
}
static void win_waveInReset (SoundRecorder me) {
	try {
		my err = waveInReset (my hWaveIn);
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input not reset.");
	}
}
static void win_waveInUnprepareHeader (SoundRecorder me, int which) {
	try {
		my err = waveInUnprepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input: cannot unprepare header.");
	}
}
static void win_waveInClose (SoundRecorder me) {
	try {
		my err = waveInClose (my hWaveIn);
		my hWaveIn = 0;
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw ("Audio input not closed.");
	}
}
#endif

static void stopRecording (SoundRecorder me) {	
	if (! my recording) return;
	try {
		my recording = false;
		if (! my synchronous) {
			if (my inputUsesPortAudio) {
				Pa_StopStream (my portaudioStream);
				Pa_CloseStream (my portaudioStream);
				my portaudioStream = NULL;
			} else {
				#if defined (_WIN32)
					/*
					 * On newer systems, waveInStop waits until the buffer is full.
					 * Wrong behaviour!
					 * Therefore, we call waveInReset instead.
					 * But on these same newer systems, waveInReset causes the dwBytesRecorded
					 * attribute to go to zero, so we cannot do
					 * my nsamp = my waveHeader [0]. dwBytesRecorded / (sizeof (short) * my numberOfChannels);
					 */
					MMTIME mmtime;
					mmtime. wType = TIME_BYTES;
					my nsamp = 0;
					if (waveInGetPosition (my hWaveIn, & mmtime, sizeof (MMTIME)) == MMSYSERR_NOERROR)
						my nsamp = mmtime. u.cb / (sizeof (short) * my numberOfChannels);
					win_waveInReset (me);
					if (my nsamp == 0)
						my nsamp = my waveHeader [0]. dwBytesRecorded / (sizeof (short) * my numberOfChannels);
					if (my nsamp > my nmax)
						my nsamp = my nmax;
					win_waveInUnprepareHeader (me, 0);
					win_waveInClose (me);
				#endif
			}
		}
	} catch (MelderError) {
		Melder_flushError ("Cannot stop recording.");
	}
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
}

void structSoundRecorder :: v_destroy () {
	stopRecording (this);   // must occur before freeing my buffer
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // must also occur before freeing my buffer
	#if gtk
		g_idle_remove_by_data (this);
	#elif motif
		if (workProcId) XtRemoveWorkProc (workProcId);
	#endif
	NUMvector_free <short> (buffer, 0);

	if (inputUsesPortAudio) {
		if (portaudioStream) Pa_StopStream (portaudioStream);
		if (portaudioStream) Pa_CloseStream (portaudioStream);
	} else {
		#if defined (_WIN32)
			if (hWaveIn != 0) {
				waveInReset (hWaveIn);
				waveInUnprepareHeader (hWaveIn, & waveHeader [0], sizeof (WAVEHDR));
				waveInClose (hWaveIn);
			}
		#elif defined (macintosh)
		#elif defined (UNIX)
			if (fd != -1) close (fd);
		#endif
	}
	forget (graphics);
	SoundRecorder_Parent :: v_destroy ();
}

static void showMaximum (SoundRecorder me, int channel, double maximum) {
	maximum /= 32768.0;
	Graphics_setWindow (my graphics,
		my numberOfChannels == 1 || channel == 1 ? -0.1 : -2.1,
		my numberOfChannels == 1 || channel == 2 ? 1.1 : 3.1,
		-0.1, 1.1);
	Graphics_setGrey (my graphics, 0.9);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, maximum, 1.0);
	Graphics_setColour (my graphics, Graphics_GREEN);
	if (maximum < 0.75) {
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, maximum);
	} else {
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 0.75);
		Graphics_setColour (my graphics, Graphics_YELLOW);
		if (maximum < 0.92) {
			Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.75, maximum);
		} else {
			Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.75, 0.92);
			Graphics_setColour (my graphics, Graphics_RED);
			Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.92, maximum);
		}
	}
}

static void showMeter (SoundRecorder me, short *buffer, long nsamp) {
	Melder_assert (my graphics != NULL);
	if (nsamp < 1) {
		Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
		#if defined (macintosh)
			Graphics_setColour (my graphics, Graphics_WHITE);
			Graphics_fillRectangle (my graphics, 0.2, 0.8, 0.3, 0.7);
		#endif
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_text (my graphics, 0.5, 0.5, L"Not recording.");
		return;
	}
	if (my p_meter_which == kSoundRecorder_meter_INTENSITY) {
		short leftMaximum = 0, rightMaximum = 0;
		if (my numberOfChannels == 1) {
			for (long i = 0; i < nsamp; i ++) {
				short value = buffer [i];
				if (abs (value) > leftMaximum) leftMaximum = abs (value);
			}
		} else {
			for (long i = 0; i < nsamp; i ++) {
				long left = buffer [i+i], right = buffer [i+i+1];
				if (abs (left) > leftMaximum) leftMaximum = abs (left);
				if (abs (right) > rightMaximum) rightMaximum = abs (right);
			}
		}
		if (my lastLeftMaximum > 30000) {
			int leak = my lastLeftMaximum - 2000000 / theControlPanel. sampleRate;
			if (leftMaximum < leak) leftMaximum = leak;
		}
		showMaximum (me, 1, leftMaximum);
		my lastLeftMaximum = leftMaximum;
		if (my numberOfChannels == 2) {
			if (my lastRightMaximum > 30000) {
				int leak = my lastRightMaximum - 2000000 / theControlPanel. sampleRate;
				if (rightMaximum < leak) rightMaximum = leak;
			}
			showMaximum (me, 2, rightMaximum);
			my lastRightMaximum = rightMaximum;
		}
	} else if (my p_meter_which == kSoundRecorder_meter_CENTRE_OF_GRAVITY_VERSUS_INTENSITY) {
		autoSound sound = Sound_create (my numberOfChannels,
			0.0, nsamp / theControlPanel. sampleRate,
			nsamp, 1.0 / theControlPanel. sampleRate, 0.5 / theControlPanel. sampleRate);
		short *p = & buffer [0];
		for (long isamp = 1; isamp <= nsamp; isamp ++) {
			for (long ichan = 1; ichan <= my numberOfChannels; ichan ++) {
				sound -> z [ichan] [isamp] = * (p ++) / 32768.0;
			}
		}
		Sound_multiplyByWindow (sound.peek(), kSound_windowShape_KAISER_2);
		double intensity = Sound_getIntensity_dB (sound.peek());
		autoSpectrum spectrum = Sound_to_Spectrum (sound.peek(), true);
		double centreOfGravity = Spectrum_getCentreOfGravity (spectrum.peek(), 1.0);
		trace ("%ld samples, intensity %f dB, centre of gravity %f Hz", nsamp, intensity, centreOfGravity);
		Graphics_setWindow (my graphics,
			my p_meter_centreOfGravity_minimum, my p_meter_centreOfGravity_maximum,
			my p_meter_intensity_minimum, my p_meter_intensity_maximum);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_fillRectangle (my graphics,
			my p_meter_centreOfGravity_minimum, my p_meter_centreOfGravity_maximum,
			my p_meter_intensity_minimum, my p_meter_intensity_maximum);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_fillCircle_mm (my graphics, centreOfGravity, intensity, 3.0);
	}
}

static bool tooManySamplesInBufferToReturnToGui (SoundRecorder me) {
	return false;
}

static long getMyNsamp (SoundRecorder me) {
	volatile long nsamp = my nsamp;   // Prevent inlining.
	return nsamp;
}

#if gtk
static gboolean workProc (void *void_me) {
#else
static bool workProc (void *void_me) {
#endif
	iam (SoundRecorder);
	try {
		short buffertje [step*2];
		int stepje = 0;

		#if defined (linux)
			#define min(a,b) a > b ? b : a
		#endif

		/* Determine global audio parameters (may have been changed by an external control panel):
		 *   1. input source;
		 *   2. left and right gain;
		 *   3. sampling frequency.
		 */
		if (my inputUsesPortAudio) {
		} else {
		}

		/* Set the buttons according to the audio parameters. */

		if (my recordButton) my recordButton -> f_setSensitive (! my recording);
		if (my stopButton)   my stopButton   -> f_setSensitive (  my recording);
		if (my playButton)   my playButton   -> f_setSensitive (! my recording && my nsamp > 0);
		if (my applyButton)  my applyButton  -> f_setSensitive (! my recording && my nsamp > 0);
		if (my okButton)     my okButton     -> f_setSensitive (! my recording && my nsamp > 0);
		if (my monoButton   && my numberOfChannels == 1) my monoButton   -> f_set ();
		if (my stereoButton && my numberOfChannels == 2) my stereoButton -> f_set ();
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button && theControlPanel. sampleRate == my fsamp_ [i]. fsamp)
				my fsamp_ [i]. button -> f_set ();
		if (my device_ [theControlPanel. inputSource]. button)
			my device_ [theControlPanel. inputSource]. button -> f_set ();
		if (my monoButton)   my monoButton   -> f_setSensitive (! my recording);
		if (my stereoButton) my stereoButton -> f_setSensitive (! my recording);
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button) {
				my fsamp_ [i]. button -> f_setSensitive (! my recording);
			}
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
			if (my device_ [i]. button)
				my device_ [i]. button -> f_setSensitive (! my recording);

		/*Graphics_setGrey (my graphics, 0.9);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 32768.0);
		Graphics_setGrey (my graphics, 0.9);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 32768.0);*/

		if (my synchronous) {
			/*
			 * Read some samples into 'buffertje'.
			 */
			do {
				if (my inputUsesPortAudio) {
					/*
					 * Asynchronous recording: do nothing.
					 */
				} else {
					#if defined (macintosh) || defined (_WIN32)
						/*
						 * Asynchronous recording on these systems: do nothing.
						 */
					#else
						// linux
						if (my fd != -1)
							stepje = read (my fd, (void *) buffertje, step * (sizeof (short) * my numberOfChannels)) / (sizeof (short) * my numberOfChannels);
					#endif
				}

				if (my recording) {
					memcpy (my buffer + my nsamp * my numberOfChannels, buffertje, stepje * (sizeof (short) * my numberOfChannels));
				}
				showMeter (me, buffertje, stepje);
				if (my recording) {
					my nsamp += stepje;
					if (my nsamp > my nmax - step) my recording = false;
					my progressScale -> f_setValue (1000.0 * ((double) my nsamp / (double) my nmax));
				}
			} while (my recording && tooManySamplesInBufferToReturnToGui (me));
		} else {
			if (my recording) {
				/*
				 * We have to know how far the buffer has been filled.
				 * However, the buffer may be filled at interrupt time,
				 * so that the buffer may be being filled during this workproc.
				 * So we ask for the buffer filling just once, namely here at the beginning.
				 */
				long lastSample = 0;
				if (my inputUsesPortAudio) {
					 /*
					  * The buffer filling is contained in my nsamp,
					  * which has been set during interrupt time and may again be updated behind our backs during this workproc.
					  * So we do it in such a way that the compiler cannot ask for my nsamp twice.
					  */
					lastSample = getMyNsamp (me);
					Pa_Sleep (10);
				} else {
					#if defined (_WIN32)
						MMTIME mmtime;
						mmtime. wType = TIME_BYTES;
						if (waveInGetPosition (my hWaveIn, & mmtime, sizeof (MMTIME)) == MMSYSERR_NOERROR)
							lastSample = mmtime. u.cb / (sizeof (short) * my numberOfChannels);
					#elif defined (macintosh)
					#endif
				}
				long firstSample = lastSample - 3000;
				if (firstSample < 0) firstSample = 0;
				showMeter (me, my buffer + firstSample * my numberOfChannels, lastSample - firstSample);
				my progressScale -> f_setValue (1000.0 * ((double) lastSample / (double) my nmax));
			} else {
				showMeter (me, NULL, 0);
			}
		}
		#if gtk
			return true;
		#else
			return false;
		#endif
	} catch (MelderError) {
		Melder_flushError (NULL);
		return false;
	}
}

static int portaudioStreamCallback (
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *void_me)
{
	/*
	 * This procedure may be called at interrupt time.
	 * It therefore accesses only data that is constant during recording,
	 * namely me, my buffer, my numberOfChannels, and my nmax.
	 * The only thing it changes is my nsamp;
	 * the workProc will therefore have to take some care in accessing my nsamp (see there).
	 */
	iam (SoundRecorder);
	(void) output;
	(void) timeInfo;
	(void) statusFlags;
	if (Melder_debug == 20) Melder_casual ("The PortAudio stream callback receives %ld frames.", frameCount);
	Melder_assert (my nsamp <= my nmax);
	unsigned long samplesLeft = my nmax - my nsamp;
	if (samplesLeft > 0) {
		unsigned long dsamples = samplesLeft > frameCount ? frameCount : samplesLeft;
		if (Melder_debug == 20) Melder_casual ("play %ls %ls", Melder_integer (dsamples),
			Melder_double (Pa_GetStreamCpuLoad (my portaudioStream)));
		memcpy (my buffer + my nsamp * my numberOfChannels, input, 2 * dsamples * my numberOfChannels);
		my nsamp += dsamples;
		if (my nsamp >= my nmax) return paComplete;
	} else /*if (my nsamp >= my nmax)*/ {
		my nsamp = my nmax;
		return paComplete;
	}
	return paContinue;
}

static void gui_button_cb_record (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	try {
		if (my recording) return;
		my nsamp = 0;
		my recording = true;
		my lastLeftMaximum = 0;
		my lastRightMaximum = 0;
		if (! my synchronous) {
			if (my inputUsesPortAudio) {
				PaStreamParameters streamParameters = { 0 };
				streamParameters. device = my deviceIndices [theControlPanel. inputSource];
				streamParameters. channelCount = my numberOfChannels;
				streamParameters. sampleFormat = paInt16;
				streamParameters. suggestedLatency = my deviceInfos [theControlPanel. inputSource] -> defaultLowInputLatency;
				#if defined (macintosh)
					PaMacCoreStreamInfo macCoreStreamInfo = { 0 };
					macCoreStreamInfo. size = sizeof (PaMacCoreStreamInfo);
					macCoreStreamInfo. hostApiType = paCoreAudio;
					macCoreStreamInfo. version = 0x01;
					macCoreStreamInfo. flags = paMacCoreChangeDeviceParameters | paMacCoreFailIfConversionRequired;
					streamParameters. hostApiSpecificStreamInfo = & macCoreStreamInfo;
				#endif
				if (Melder_debug == 20) Melder_casual ("Before Pa_OpenStream");
				PaError err = Pa_OpenStream (& my portaudioStream, & streamParameters, NULL,
					theControlPanel. sampleRate, 0, paNoFlag, portaudioStreamCallback, (void *) me);
				if (Melder_debug == 20) Melder_casual ("Pa_OpenStream returns %d", err);
				if (err)
					Melder_throw ("open ", Melder_peekUtf8ToWcs (Pa_GetErrorText (err)));
				Pa_StartStream (my portaudioStream);
				if (Melder_debug == 20) Melder_casual ("Pa_StartStream returns %d", err);
				if (err)
					Melder_throw ("start ", Melder_peekUtf8ToWcs (Pa_GetErrorText (err)));
			} else {
				#if defined (_WIN32)
					win_fillFormat (me);
					win_fillHeader (me, 0);
					win_waveInOpen (me);
					win_waveInPrepareHeader (me, 0);
					win_waveInAddBuffer (me, 0);
					win_waveInStart (me);
				#elif defined (macintosh)
				#endif
			}
		}
		Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	} catch (MelderError) {
		Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
		my recording = false;
		Melder_flushError ("Cannot record.");
	}
}

static void gui_button_cb_stop (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	stopRecording (me);
}

static void gui_button_cb_play (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	if (my recording || my nsamp == 0) return;
	MelderAudio_play16 (my buffer, theControlPanel. sampleRate, my fakeMono ? my nsamp / 2 : my nsamp, my fakeMono ? 2 : my numberOfChannels, NULL, NULL);
}

static void publish (SoundRecorder me) {
	autoSound sound;
	long nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	if (my nsamp == 0) return;
	double fsamp = theControlPanel. sampleRate;
	if (fsamp <= 0.0) fsamp = 44100.0;   // safe
	try {
		sound.reset (Sound_createSimple (my numberOfChannels, (double) nsamp / fsamp, fsamp));
	} catch (MelderError) {
		Melder_flushError ("You can still save to file.");
		return;
	}
	if (my fakeMono) {
		for (long i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = (my buffer [i + i - 2] + my buffer [i + i - 1]) * (1.0 / 65536);
	} else if (my numberOfChannels == 1) {
		for (long i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = my buffer [i - 1] * (1.0 / 32768);
	} else {
		for (long i = 1; i <= nsamp; i ++) {
			sound -> z [1] [i] = my buffer [i + i - 2] * (1.0 / 32768);
			sound -> z [2] [i] = my buffer [i + i - 1] * (1.0 / 32768);
		}
	}
	if (my soundName) {
		autostring name = my soundName -> f_getString ();
		Thing_setName (sound.peek(), name.peek());
	}
	my broadcastPublication (sound.transfer());
}

static void gui_button_cb_cancel (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	stopRecording (me);
	forget (me);
}

static void gui_button_cb_apply (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	stopRecording (me);
	publish (me);
}

static void gui_button_cb_ok (I, GuiButtonEvent event) {
	(void) event;
	iam (SoundRecorder);
	stopRecording (me);
	publish (me);
	forget (me);
}

static void initialize (SoundRecorder me) {
	try {
		if (my inputUsesPortAudio) {
			#if defined (macintosh)
				my fsamp_ [SoundRecorder_IFSAMP_8000]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_11025]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_12000]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_16000]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_22050]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_24000]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_32000]. canDo = false;
				my fsamp_ [SoundRecorder_IFSAMP_64000]. canDo = false;
			#else
				// Accept all standard sample rates.
				(void) me;
			#endif
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				(void) me;
			#elif defined (linux)
				int sampleRate = (int) theControlPanel. sampleRate, sampleSize = 16;
				int channels = my numberOfChannels, stereo = ( my numberOfChannels == 2 ), val;
				#if __BYTE_ORDER == __BIG_ENDIAN
					int format = AFMT_S16_BE;
				#else
					int format = AFMT_S16_LE;
				#endif
				int fd_mixer;
				my fd = open ("/dev/dsp", O_RDONLY);
				if (my fd == -1) {
					if (errno == EBUSY)
						Melder_throw ("Audio device already in use.");
					else
						Melder_throw ("Cannot open audio device.\n"
							"Please switch on PortAudio in the Sound Recording Preferences.");
				}
				ioctl (my fd, SNDCTL_DSP_RESET, NULL);
				ioctl (my fd, SNDCTL_DSP_SPEED, & sampleRate);
				ioctl (my fd, SNDCTL_DSP_SAMPLESIZE, & sampleSize);
				ioctl (my fd, SNDCTL_DSP_CHANNELS, (val = channels, & val));
				if (channels == 1 && val == 2) {
					close (my fd);
					Melder_throw ("This sound card does not support mono.");
				}
				ioctl (my fd, SNDCTL_DSP_STEREO, & stereo);
				ioctl (my fd, SNDCTL_DSP_SETFMT, & format);
				fd_mixer = open ("/dev/mixer", O_WRONLY);		
				if (fd_mixer == -1) {
					Melder_throw ("Cannot open /dev/mixer.");
				} else {
					int dev_mask = theControlPanel. inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
					if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1) {
						close (fd_mixer);
						Melder_throw ("Can't set recording device in mixer.");
					}
					close (fd_mixer);
				}
			#endif
		}
	} catch (MelderError) {
		Melder_throw ("16-bit audio recording not initialized.");
	}
}

static void gui_radiobutton_cb_input (I, GuiRadioButtonEvent event) {
	iam (SoundRecorder);
	Melder_casual ("SoundRecorder: setting the input source from %ld to %ld.", (long) theControlPanel. inputSource, (long) event -> position);
	theControlPanel. inputSource = event -> position;

	/* Set system's input source. */
	if (my inputUsesPortAudio) {
		// deferred to the start of recording
	} else {
		#if defined (_WIN32)
			// deferred to the start of recording
		#elif defined (macintosh)
			//SPBCloseDevice (my refNum);
			try {
				initialize (me);
			} catch (MelderError) {
				Melder_flushError (NULL);
			}
		#elif defined (linux)
			int fd_mixer = open ("/dev/mixer", O_WRONLY);		
			if (fd_mixer == -1) {
				Melder_flushError ("(Sound_record:) Cannot open /dev/mixer.");
			}
			int dev_mask = theControlPanel.inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
			if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
				Melder_flushError ("(Sound_record:) Can't set recording device in mixer");		
			close (fd_mixer);
		#endif
	}
}

static void gui_radiobutton_cb_fsamp (I, GuiRadioButtonEvent event) {
	iam (SoundRecorder);
	if (my recording) return;
	try {
		double fsamp = NUMundefined;
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (event -> toggle == my fsamp_ [i]. button)
				fsamp = my fsamp_ [i]. fsamp;
		Melder_assert (NUMdefined (fsamp));
		/*
		 * If we push the 48000 button while the sampling frequency is 22050,
		 * we first get a message that the 22050 button has changed,
		 * and then we get a message that the 48000 button has changed.
		 * So the following will work (it used to be different with old Motif versions on Linux):
		 */
		Melder_casual ("SoundRecorder: setting the sample rate from %ld to %ld Hz.", (long) theControlPanel. sampleRate, (long) fsamp);
		if (fsamp == theControlPanel. sampleRate) return;
		/*
		 * Now we know, hopefully, that the message is from the button that was clicked,
		 * not the one that was unset by the radio box, so we can take action.
		 */
		theControlPanel. sampleRate = fsamp;
		/*
		 * Set the system's sampling frequency.
		 * On some systems, we cannot do this without closing the audio device,
		 * and reopening it with a new sampling frequency.
		 */
		if (my inputUsesPortAudio) {
			// deferred to the start of recording
		} else {
			#if defined (_WIN32)
				// deferred to the start of recording
			#elif defined (macintosh)
				//SPBCloseDevice (my refNum);
				initialize (me);
			#elif defined (linux)		
				close (my fd);
				initialize (me);
			#endif
		}
	} catch (MelderError) {
		Melder_throw ("Sampling frequency not changed.");
	}
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (SoundRecorder);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	Graphics_setWsViewport (my graphics, 0, event -> width, 0, event -> height);
	Graphics_setWsWindow (my graphics, 0, event -> width, 0, event -> height);
	Graphics_setViewport (my graphics, 0, event -> width, 0, event -> height);
	Graphics_updateWs (my graphics);
}

void structSoundRecorder :: v_createChildren ()
{
	/* Channels */

	long y = 20 + Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, 10, 160, y, y + Gui_LABEL_HEIGHT, L"Channels:", 0);

	GuiRadioGroup_begin ();
	y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
	monoButton = GuiRadioButton_createShown (d_windowForm, 20, 170, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Mono", NULL, NULL, 0);
	y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
	stereoButton = GuiRadioButton_createShown (d_windowForm, 20, 170, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Stereo", NULL, NULL, 0);
	GuiRadioGroup_end ();

	/* Input source */
	
	y = 140 + Machine_getMenuBarHeight ();
	#if defined (_WIN32)
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, L"(use Windows mixer", 0);
		y += Gui_LABEL_HEIGHT + 10;
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, L"   without meters)", 0);
	#else
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, L"Input source:", 0);
		GuiRadioGroup_begin ();
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
			if (device_ [i]. canDo) {
				y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
				device_ [i]. button = GuiRadioButton_createShown (d_windowForm, 20, 170, y, y + Gui_RADIOBUTTON_HEIGHT,
					device_ [i]. name, gui_radiobutton_cb_input, this, 0);
			}
		}
		GuiRadioGroup_end ();
	#endif
	
	/* Meter box */
	
	y = 20 + Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, 170, -170, y, y + Gui_LABEL_HEIGHT, L"Meter", GuiLabel_CENTRE);
	y += Gui_LABEL_HEIGHT;
	meter = GuiDrawingArea_createShown (d_windowForm, 170, -170, y, -150,
		NULL, NULL, NULL, gui_drawingarea_cb_resize, this, GuiDrawingArea_BORDER);

	/* Sampling frequency */

	y = 20 + Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, -160, -10, y, y + Gui_LABEL_HEIGHT, L"Sampling frequency:", 0);
	GuiRadioGroup_begin ();
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
		if (fsamp_ [i]. canDo) {
			double fsamp = fsamp_ [i]. fsamp;
			wchar_t title [40];
			swprintf (title, 40, L"%ls Hz", fsamp == floor (fsamp) ? Melder_integer ((long) fsamp) : Melder_fixed (fsamp, 5));
			y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
			fsamp_ [i]. button = GuiRadioButton_createShown (d_windowForm,
				-150, -10, y, y + Gui_RADIOBUTTON_HEIGHT,
				title, gui_radiobutton_cb_fsamp, this, fsamp == theControlPanel. sampleRate ? GuiRadioButton_SET : 0);
		}
	}
	GuiRadioGroup_end ();

	progressScale = GuiScale_createShown (d_windowForm,
		10, 350, -130, -90,
		0, 1000, 0, 0);

	y = 60;
	recordButton = GuiButton_createShown (d_windowForm, 20, 90, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		L"Record", gui_button_cb_record, this, 0);
	stopButton = GuiButton_createShown (d_windowForm, 100, 170, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		L"Stop", gui_button_cb_stop, this, 0);
	if (inputUsesPortAudio) {
		playButton = GuiButton_createShown (d_windowForm, 180, 250, -y - Gui_PUSHBUTTON_HEIGHT, -y,
			L"Play", gui_button_cb_play, this, 0);
	} else {
		#if defined (_WIN32) || defined (macintosh)
			playButton = GuiButton_createShown (d_windowForm, 180, 250, -y - Gui_PUSHBUTTON_HEIGHT, -y,
				L"Play", gui_button_cb_play, this, 0);
		#endif
	}
	
	GuiLabel_createShown (d_windowForm, -200, -130, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2, L"Name:", GuiLabel_RIGHT);
	soundName = GuiText_createShown (d_windowForm, -120, -20, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2, 0);
	soundName -> f_setString (L"untitled");

	y = 20;
	cancelButton = GuiButton_createShown (d_windowForm, -350, -280, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		L"Close", gui_button_cb_cancel, this, 0);
	applyButton = GuiButton_createShown (d_windowForm, -270, -170, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		L"Save to list", gui_button_cb_apply, this, GuiButton_DEFAULT);
	okButton = GuiButton_createShown (d_windowForm, -160, -20, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		L"Save to list & Close", gui_button_cb_ok, this, 0);
}

static void writeFakeMonoFile (SoundRecorder me, MelderFile file, int audioFileType) {
	long nsamp = my nsamp / 2;
	autoMelderFile mfile = MelderFile_create (file);
	MelderFile_writeAudioFileHeader (file, audioFileType, theControlPanel. sampleRate, nsamp, 1, 16);
	if (Melder_defaultAudioFileEncoding (audioFileType, 16) == Melder_LINEAR_16_BIG_ENDIAN) {
		for (long i = 0; i < nsamp; i ++)
			binputi2 ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
	} else {
		for (long i = 0; i < nsamp; i ++)
			binputi2LE ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
	}
	MelderFile_writeAudioFileTrailer (file, audioFileType, theControlPanel. sampleRate, nsamp, 1, 16);
	mfile.close ();
}

static void writeAudioFile (SoundRecorder me, MelderFile file, int audioFileType) {
	try {
		if (my fakeMono) {
			writeFakeMonoFile (me, file, audioFileType);
		} else {
			MelderFile_writeAudioFile (file, audioFileType, my buffer, theControlPanel. sampleRate, my nsamp, my numberOfChannels, 16);
		}
	} catch (MelderError) {
		Melder_throw ("Audio file not written.");
	}
}

static void menu_cb_writeWav (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as WAV file", 0)
		wchar_t *name = my soundName -> f_getString ();
		swprintf (defaultName, 300, L"%ls.wav", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_WAV);
	EDITOR_END
}

static void menu_cb_writeAifc (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as AIFC file", 0)
		wchar_t *name = my soundName -> f_getString ();
		swprintf (defaultName, 300, L"%ls.aifc", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_AIFC);
	EDITOR_END
}

static void menu_cb_writeNextSun (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as NeXT/Sun file", 0)
		wchar_t *name = my soundName -> f_getString ();
		swprintf (defaultName, 300, L"%ls.au", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NEXT_SUN);
	EDITOR_END
}

static void menu_cb_writeNist (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as NIST file", 0)
		wchar_t *name = my soundName -> f_getString ();
		swprintf (defaultName, 300, L"%ls.nist", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NIST);
	EDITOR_END
}

static void updateMenus (SoundRecorder me) {
	my d_meterIntensityButton -> f_check (my p_meter_which == kSoundRecorder_meter_INTENSITY);
	my d_meterCentreOfGravityVersusIntensityButton -> f_check (my p_meter_which == kSoundRecorder_meter_CENTRE_OF_GRAVITY_VERSUS_INTENSITY);
}

static void menu_cb_intensity (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	my pref_meter_which () = my p_meter_which = kSoundRecorder_meter_INTENSITY;
	updateMenus (me);
}
static void menu_cb_centreOfGravityVersusIntensity (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	my pref_meter_which () = my p_meter_which = kSoundRecorder_meter_CENTRE_OF_GRAVITY_VERSUS_INTENSITY;
	updateMenus (me);
}

static void menu_cb_SoundRecorder_help (EDITOR_ARGS) { EDITOR_IAM (SoundRecorder); Melder_help (L"SoundRecorder"); }

void structSoundRecorder :: v_createMenus () {
	SoundRecorder_Parent :: v_createMenus ();
	Editor_addCommand (this, L"File", L"Save as WAV file...", 0, menu_cb_writeWav);
	Editor_addCommand (this, L"File", L"Save as AIFC file...", 0, menu_cb_writeAifc);
	Editor_addCommand (this, L"File", L"Save as NeXT/Sun file...", 0, menu_cb_writeNextSun);
	Editor_addCommand (this, L"File", L"Save as NIST file...", 0, menu_cb_writeNist);
	Editor_addCommand (this, L"File", L"-- write --", 0, 0);
	Editor_addMenu (this, L"Meter", 0);
	d_meterIntensityButton =
		Editor_addCommand (this, L"Meter", L"Intensity", GuiMenu_RADIO_FIRST, menu_cb_intensity);
	d_meterCentreOfGravityVersusIntensityButton =
		Editor_addCommand (this, L"Meter", L"Centre of gravity ~ intensity", GuiMenu_RADIO_NEXT, menu_cb_centreOfGravityVersusIntensity);
}

void structSoundRecorder :: v_createHelpMenuItems (EditorMenu menu) {
	SoundRecorder_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"SoundRecorder help", '?', menu_cb_SoundRecorder_help);
}

SoundRecorder SoundRecorder_create (int numberOfChannels) {
	try {
		autoSoundRecorder me = Thing_new (SoundRecorder);
		my inputUsesPortAudio = MelderAudio_getInputUsesPortAudio ();

		if (my inputUsesPortAudio) {
		} else {
			#if defined (_WIN32)
				UINT numberOfDevices = waveInGetNumDevs (), i;
				WAVEINCAPS caps;
				MMRESULT err;
				if (numberOfDevices == 0)
					Melder_throw ("No sound input devices available.");
				err = waveInGetDevCaps (WAVE_MAPPER, & caps, sizeof (WAVEINCAPS));
				if (numberOfChannels == 2 && caps. wChannels < 2)
					Melder_throw ("Your computer does not support stereo sound input.");
				/* BUG: should we ask whether 16 bit is supported? */
				for (i = 0; i < numberOfDevices; i ++) {
					waveInGetDevCaps (i, & caps, sizeof (WAVEINCAPS));
					/*Melder_casual ("Name of device %d: %s", i, caps. szPname);*/
				}
			#elif defined (macintosh)
				SInt32 soundFeatures;
				if (Gestalt (gestaltSoundAttr, & soundFeatures) ||
						! (soundFeatures & (1 << gestaltSoundIOMgrPresent)) ||
						! (soundFeatures & (1 << gestaltBuiltInSoundInput)) ||
						! (soundFeatures & (1 << gestaltHasSoundInputDevice)))
					Melder_throw ("Your computer does not support sound input.");
				if (! (soundFeatures & (1 << gestalt16BitSoundIO)) ||   /* Hardware. */
					! (soundFeatures & (1 << gestaltStereoInput)) ||   /* Hardware. */
					! (soundFeatures & (1 << gestalt16BitAudioSupport)))   /* Software. */
					Melder_throw ("Your computer does not support stereo sound input.");
			#endif
		}
		my numberOfChannels = numberOfChannels;
		if (sizeof (short) != 2)
			Melder_throw ("Long shorts!!!!!");
		if (my inputUsesPortAudio) {
			my synchronous = false;
		} else {
			#if defined (macintosh) || defined (_WIN32)
				my synchronous = false;
			#else
				my synchronous = true;
			#endif
		}
		/*
		 * Allocate the maximum buffer.
		 */
		if (preferences.bufferSizeInMegabytes < 1) preferences.bufferSizeInMegabytes = 1;   /* Validate preferences. */
		if (preferences.bufferSizeInMegabytes > 1000) preferences.bufferSizeInMegabytes = 1000;
		if (my buffer == NULL) {
			long nmax_bytes_pref = preferences.bufferSizeInMegabytes * 1000000;
			long nmax_bytes = my inputUsesPortAudio ? nmax_bytes_pref :
				#if defined (_WIN32)
					66150000;   /* The maximum physical buffer on Windows XP; shorter than in Windows 98, alas. */
				#else
					nmax_bytes_pref;
				#endif
			my nmax = nmax_bytes / (sizeof (short) * numberOfChannels);
			for (;;) {
				try {
					my buffer = NUMvector <short> (0, my nmax * numberOfChannels - 1);
					break;   // success
				} catch (MelderError) {
					if (my nmax < 100000) {
						throw MelderError ();   // failure, with error message
					} else {
						Melder_clearError ();
						my nmax /= 2;   // retry with less application memory
					}
				}
			}
		}
		Melder_assert (my buffer != NULL);

		/*
		 * Count the number of input devices and sources.
		 */
		if (my inputUsesPortAudio) {
			static bool paInitialized = false;
			if (! paInitialized) {
				PaError err = Pa_Initialize ();
				if (Melder_debug == 20) Melder_casual ("init %s", Pa_GetErrorText (err));
				paInitialized = true;
				if (Melder_debug == 20) {
					PaHostApiIndex hostApiCount = Pa_GetHostApiCount ();
					Melder_casual ("host API count %ls", Melder_integer (hostApiCount));
					for (PaHostApiIndex iHostApi = 0; iHostApi < hostApiCount; iHostApi ++) {
						const PaHostApiInfo *hostApiInfo = Pa_GetHostApiInfo (iHostApi);
						PaHostApiTypeId type = hostApiInfo -> type;
						Melder_casual ("host API %ls: %ls, \"%s\" %ls", Melder_integer (iHostApi), Melder_integer (type), hostApiInfo -> name, Melder_integer (hostApiInfo -> deviceCount));
					}
					PaHostApiIndex defaultHostApi = Pa_GetDefaultHostApi ();
					Melder_casual ("default host API %ls", Melder_integer (defaultHostApi));
					PaDeviceIndex deviceCount = Pa_GetDeviceCount ();
					Melder_casual ("device count %ls", Melder_integer (deviceCount));
				}
			}
			PaDeviceIndex deviceCount = Pa_GetDeviceCount ();
			for (PaDeviceIndex idevice = 0; idevice < deviceCount; idevice ++) {
				const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo (idevice);
				if (Melder_debug == 20) Melder_casual ("Device \"%s\", input %d, output %d, sample rate %lf", deviceInfo -> name,
					deviceInfo -> maxInputChannels, deviceInfo -> maxOutputChannels, deviceInfo -> defaultSampleRate);
				if (deviceInfo -> maxInputChannels > 0 && my numberOfInputDevices < SoundRecorder_IDEVICE_MAX) {
					my device_ [++ my numberOfInputDevices]. canDo = true;
					wcsncpy (my device_ [my numberOfInputDevices]. name, Melder_peekUtf8ToWcs (deviceInfo -> name), 40);
					my device_ [my numberOfInputDevices]. name [40] = '\0';
					my deviceInfos [my numberOfInputDevices] = deviceInfo;
					my deviceIndices [my numberOfInputDevices] = idevice;
				}
			}
			if (my numberOfInputDevices == 0)
				Melder_throw ("No input devices available.");
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				// No device info: use Windows mixer.
			#else
				my device_ [1]. canDo = true;
				wcscpy (my device_ [1]. name, L"Microphone");
				my device_ [2]. canDo = true;
				wcscpy (my device_ [2]. name, L"Line");
			#endif
		}

		/*
		 * Sampling frequency constants.
		 */
		my fsamp_ [SoundRecorder_IFSAMP_8000]. fsamp = 8000.0;
		my fsamp_ [SoundRecorder_IFSAMP_9800]. fsamp = 9800.0;
		my fsamp_ [SoundRecorder_IFSAMP_11025]. fsamp = 11025.0;
		my fsamp_ [SoundRecorder_IFSAMP_12000]. fsamp = 12000.0;
		my fsamp_ [SoundRecorder_IFSAMP_16000]. fsamp = 16000.0;
		my fsamp_ [SoundRecorder_IFSAMP_22050]. fsamp = 22050.0;
		my fsamp_ [SoundRecorder_IFSAMP_22254]. fsamp = 22254.54545;
		my fsamp_ [SoundRecorder_IFSAMP_24000]. fsamp = 24000.0;
		my fsamp_ [SoundRecorder_IFSAMP_32000]. fsamp = 32000.0;
		my fsamp_ [SoundRecorder_IFSAMP_44100]. fsamp = 44100.0;
		my fsamp_ [SoundRecorder_IFSAMP_48000]. fsamp = 48000.0;
		my fsamp_ [SoundRecorder_IFSAMP_64000]. fsamp = 64000.0;
		my fsamp_ [SoundRecorder_IFSAMP_96000]. fsamp = 96000.0;
		my fsamp_ [SoundRecorder_IFSAMP_192000]. fsamp = 192000.0;

		/*
		 * The default set of possible sampling frequencies, to be modified in the initialize () procedure.
		 */
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) my fsamp_ [i]. canDo = true;   // optimistic: can do all, except two:
		my fsamp_ [SoundRecorder_IFSAMP_9800]. canDo = false;   // sgi only
		my fsamp_ [SoundRecorder_IFSAMP_22254]. canDo = false;   // old Mac only

		/*
		 * Initialize system-dependent structures.
		 * On all systems: stereo 16-bit linear encoding.
		 * Some systems take initial values from the system control panel
		 * (automatically in the workProc), other systems from theControlPanel.
		 */
		initialize (me.peek());

		Editor_init (me.peek(), 100, 100, 600, 500, L"SoundRecorder", NULL);
		my graphics = Graphics_create_xmdrawingarea (my meter);
		Melder_assert (my graphics);
		Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);

struct structGuiDrawingAreaResizeEvent event = { my meter, 0 };
event. width  = my meter -> f_getWidth  ();
event. height = my meter -> f_getHeight ();
gui_drawingarea_cb_resize (me.peek(), & event);

		#if gtk
			g_idle_add (workProc, me.peek());
		#elif motif
			my workProcId = GuiAddWorkProc (workProc, me.peek());
		#endif
		updateMenus (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SoundRecorder not created.");
	}
}

/* End of file SoundRecorder.cpp */
