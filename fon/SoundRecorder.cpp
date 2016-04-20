/* SoundRecorder.cpp
 *
 * Copyright (C) 1992-2011,2012,2013,2014,2015,2016 Paul Boersma
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
 * are hard to predict. For this reason, everything is done with platform #ifdefs.
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
#include "NUM2.h"	// We need this for str_replace_literal()

// A function prototype so we can use it in gui_button_cb_stop(), but define it later on.
static void do_split_recording(SoundRecorder me, bool finalRecording);
// A function prototype so we can use it in do_split_recording(), but define it later on.
static void writeAudioFile(SoundRecorder me, MelderFile file, int audioFileType);
// A function prototype so we can use it in the apply and ok callbacks, but define it later on.
static void do_auto_save(SoundRecorder me, int64 recNum);

static struct {
	int bufferSizeInMegabytes;
} preferences;

void SoundRecorder_preferences () {
	Preferences_addInt (U"SoundRecorder.bufferSizeInMegabytes", & preferences.bufferSizeInMegabytes, 60);
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
	my waveHeader [which]. lpNext = nullptr;
	my waveHeader [which]. reserved = 0;
}
static void win_waveInCheck (SoundRecorder me) {
	char32 messageText [MAXERRORLENGTH];
	MMRESULT err;
	if (my err == MMSYSERR_NOERROR) return;
	err = waveInGetErrorText (my err, Melder_32toW (messageText), MAXERRORLENGTH);
	if (err == MMSYSERR_NOERROR) Melder_throw (messageText);
	else if (err == MMSYSERR_BADERRNUM) Melder_throw (U"Error number ", my err, U" out of range.");
	else if (err == MMSYSERR_NODRIVER) Melder_throw (U"No sound driver present.");
	else if (err == MMSYSERR_NOMEM) Melder_throw (U"Out of memory.");
	else Melder_throw (U"Unknown sound error.");
}
static void win_waveInOpen (SoundRecorder me) {
	try {
		my err = waveInOpen (& my hWaveIn, WAVE_MAPPER, & my waveFormat, 0, 0, CALLBACK_NULL);
		win_waveInCheck (me);
		if (Melder_debug != 8) waveInReset (my hWaveIn);
	} catch (MelderError) {
		Melder_throw (U"Audio input not opened.");
	}
}
static void win_waveInPrepareHeader (SoundRecorder me, int which) {
	try {
		my err = waveInPrepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input: cannot prepare header.\nQuit some other programs or go to \"Sound input prefs\" in the Preferences menu.");
	}
}
static void win_waveInAddBuffer (SoundRecorder me, int which) {
	try {
		my err = waveInAddBuffer (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input: cannot add buffer.");
	}
}
static void win_waveInStart (SoundRecorder me) {
	try {
		my err = waveInStart (my hWaveIn);   // asynchronous
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input not started.");
	}
}
static void win_waveInStop (SoundRecorder me) {
	try {
		my err = waveInStop (my hWaveIn);
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input not stopped.");
	}
}
static void win_waveInReset (SoundRecorder me) {
	try {
		my err = waveInReset (my hWaveIn);
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input not reset.");
	}
}
static void win_waveInUnprepareHeader (SoundRecorder me, int which) {
	try {
		my err = waveInUnprepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input: cannot unprepare header.");
	}
}
static void win_waveInClose (SoundRecorder me) {
	try {
		my err = waveInClose (my hWaveIn);
		my hWaveIn = 0;
		win_waveInCheck (me);
	} catch (MelderError) {
		Melder_throw (U"Audio input not closed.");
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
				my portaudioStream = nullptr;
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
		Melder_flushError (U"Cannot stop recording.");
	}
	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Graphics_WHITE);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
}

void structSoundRecorder :: v_destroy () noexcept {
	stopRecording (this);   // must occur before freeing my buffer
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // must also occur before freeing my buffer
	#if cocoa
		if (our d_cocoaTimer) CFRunLoopTimerInvalidate (our d_cocoaTimer);
	#elif gtk
		g_idle_remove_by_data (this);
	#elif motif
		if (our workProcId) XtRemoveWorkProc (our workProcId);
	#endif
	NUMvector_free (buffer, 0);

	if (our inputUsesPortAudio) {
		if (our portaudioStream) Pa_StopStream (our portaudioStream);
		if (our portaudioStream) Pa_CloseStream (our portaudioStream);
	} else {
		#if defined (_WIN32)
			if (our hWaveIn != 0) {
				waveInReset (our hWaveIn);
				waveInUnprepareHeader (our hWaveIn, & our waveHeader [0], sizeof (WAVEHDR));
				waveInClose (our hWaveIn);
			}
		#elif defined (macintosh)
		#elif defined (UNIX)
			if (our fd != -1) close (our fd);
		#endif
	}
	SoundRecorder_Parent :: v_destroy ();
}

static void showMaximum (SoundRecorder me, int channel, double maximum) {
	maximum /= 32768.0;
	Graphics_setWindow (my graphics.get(),
		my numberOfChannels == 1 || channel == 1 ? 0.0 : -1.0,
		my numberOfChannels == 1 || channel == 2 ? 1.0 : 2.0,
		-0.1, 1.1);
	Graphics_setGrey (my graphics.get(), 0.9);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, maximum, 1.0);
	Graphics_setColour (my graphics.get(), Graphics_GREEN);
	if (maximum < 0.75) {
		Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, maximum);
	} else {
		Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 0.75);
		Graphics_setColour (my graphics.get(), Graphics_YELLOW);
		if (maximum < 0.92) {
			Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.75, maximum);
		} else {
			Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.75, 0.92);
			Graphics_setColour (my graphics.get(), Graphics_RED);
			Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.92, maximum);
		}
	}
}

static void showMeter (SoundRecorder me, short *buffer, long nsamp) {
	Melder_assert (my graphics);
	if (nsamp < 1) {
		Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		#if defined (macintosh)
			Graphics_setColour (my graphics.get(), Graphics_WHITE);
			Graphics_fillRectangle (my graphics.get(), 0.2, 0.8, 0.3, 0.7);
		#endif
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
		Graphics_setColour (my graphics.get(), Graphics_BLACK);
		Graphics_text (my graphics.get(), 0.5, 0.5, U"Not recording.");
		return;
	}
	if (my p_meter_which == kSoundRecorder_meter_INTENSITY) {
		int leftMaximum = 0, rightMaximum = 0;
		if (my numberOfChannels == 1) {
			for (long i = 0; i < nsamp; i ++) {
				int value = buffer [i];
				if (abs (value) > leftMaximum) leftMaximum = abs (value);
			}
		} else {
			for (long i = 0; i < nsamp; i ++) {
				int left = buffer [i+i], right = buffer [i+i+1];
				if (abs (left) > leftMaximum) leftMaximum = abs (left);
				if (abs (right) > rightMaximum) rightMaximum = abs (right);
			}
		}
		if (my lastLeftMaximum > 30000) {
			int leak = my lastLeftMaximum - (int) floor (2000000 / theControlPanel. sampleRate);
			if (leftMaximum < leak) leftMaximum = leak;
		}
		showMaximum (me, 1, leftMaximum);
		my lastLeftMaximum = leftMaximum;
		if (my numberOfChannels == 2) {
			if (my lastRightMaximum > 30000) {
				int leak = my lastRightMaximum - (int) floor (2000000 / theControlPanel. sampleRate);
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
		Sound_multiplyByWindow (sound.get(), kSound_windowShape_KAISER_2);
		double intensity = Sound_getIntensity_dB (sound.get());
		autoSpectrum spectrum = Sound_to_Spectrum (sound.get(), true);
		double centreOfGravity = Spectrum_getCentreOfGravity (spectrum.get(), 1.0);
		trace (nsamp, U" samples, intensity ", intensity, U" dB, centre of gravity ", centreOfGravity, U" Hz");
		Graphics_setWindow (my graphics.get(),
			my p_meter_centreOfGravity_minimum, my p_meter_centreOfGravity_maximum,
			my p_meter_intensity_minimum, my p_meter_intensity_maximum);
		Graphics_setColour (my graphics.get(), Graphics_WHITE);
		Graphics_fillRectangle (my graphics.get(),
			my p_meter_centreOfGravity_minimum, my p_meter_centreOfGravity_maximum,
			my p_meter_intensity_minimum, my p_meter_intensity_maximum);
		Graphics_setColour (my graphics.get(), Graphics_BLACK);
		Graphics_fillCircle_mm (my graphics.get(), centreOfGravity, intensity, 3.0);
	}
	Graphics_flushWs (my graphics.get());
}

static bool tooManySamplesInBufferToReturnToGui (SoundRecorder me) {
	(void) me;
	return false;
}

static long getMyNsamp (SoundRecorder me) {
	volatile long nsamp = my nsamp;   // Prevent inlining.
	return nsamp;
}

#if cocoa
	#define WORKPROC_RETURN  void
	#define WORKPROC_ARGS  CFRunLoopTimerRef /*timer*/, void *void_SoundRecorder
#elif gtk
	#define WORKPROC_RETURN  gboolean
	#define WORKPROC_ARGS  void *void_SoundRecorder
#else
	#define WORKPROC_RETURN  bool
	#define WORKPROC_ARGS  void *void_SoundRecorder
#endif

static WORKPROC_RETURN workProc (WORKPROC_ARGS) {
	SoundRecorder me = static_cast <SoundRecorder> (void_SoundRecorder);
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

		if (my recordButton) GuiThing_setSensitive (my recordButton, ! my recording);
		if (my stopButton)   GuiThing_setSensitive (my stopButton,     my recording);
		if (my playButton)   GuiThing_setSensitive (my playButton,   ! my recording && my nsamp > 0);
		// Also disable the apply and ok button when split recording is enabled, because we'll auto save the objects.
		if (my applyButton)  GuiThing_setSensitive (my applyButton,  ! my recording && my nsamp > 0 && ! my bSplitRec);
		if (my okButton)     GuiThing_setSensitive (my okButton,     ! my recording && my nsamp > 0 && ! my bSplitRec);
		if (my monoButton   && my numberOfChannels == 1) GuiRadioButton_set (my monoButton);
		if (my stereoButton && my numberOfChannels == 2) GuiRadioButton_set (my stereoButton);
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button && theControlPanel. sampleRate == my fsamp_ [i]. fsamp)
				GuiRadioButton_set (my fsamp_ [i]. button);
		if (my device_ [theControlPanel. inputSource]. button)
			GuiRadioButton_set (my device_ [theControlPanel. inputSource]. button);
		if (my monoButton)   GuiThing_setSensitive (my monoButton,   ! my recording);
		if (my stereoButton) GuiThing_setSensitive (my stereoButton, ! my recording);
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button) {
				GuiThing_setSensitive (my fsamp_ [i]. button, ! my recording);
			}
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
			if (my device_ [i]. button)
				GuiThing_setSensitive (my device_ [i]. button, ! my recording);

		if (my splitButton)  GuiThing_setSensitive (my splitButton,    my recording && my bSplitRec);
		// Since setSenstive doesn't seem to work correctly for text fields, we just hide the split recording options
		// altogether when split recording is disabled.
		if (my bSplitRec) {
			// If we're not yet showing the split recording options, show them now.
			if ( ! my bShowingSplitRecOpts ) {
				my bShowingSplitRecOpts = true;
				if (my recNumLbl)      GuiThing_show (my recNumLbl);
				if (my recNumText)     GuiThing_show (my recNumText);
				if (my separatorLbl)   GuiThing_show (my separatorLbl);
				if (my separatorText)  GuiThing_show (my separatorText);
			}
		} else {
			// If the split recording options weren't hidden yet, hide them now.
			if ( my bShowingSplitRecOpts ) {
				my bShowingSplitRecOpts = false;
				if (my recNumLbl)      GuiThing_hide (my recNumLbl);
				if (my recNumText)     GuiThing_hide (my recNumText);
				if (my separatorLbl)   GuiThing_hide (my separatorLbl);
				if (my separatorText)  GuiThing_hide (my separatorText);
			}
		}
		if (my bAutoSave) {
			if ( ! my bShowingAutoSaveOpts ) {
				my bShowingAutoSaveOpts = true;
				if (my autoSaveLbl)    GuiThing_show (my autoSaveLbl);
				if (my pathButton)     GuiThing_show (my pathButton);
				if (my savePathText)   GuiThing_show (my savePathText);
				if (my subdirCheckbox) GuiThing_show (my subdirCheckbox);
			}
		} else {
			if ( my bShowingAutoSaveOpts ) {
				my bShowingAutoSaveOpts = false;
				if (my autoSaveLbl)    GuiThing_hide (my autoSaveLbl);
				if (my pathButton)     GuiThing_hide (my pathButton);
				if (my savePathText)   GuiThing_hide (my savePathText);
				if (my subdirCheckbox) GuiThing_hide (my subdirCheckbox);
			}
		}

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
					GuiScale_setValue (my progressScale, 1000.0 * ((double) my nsamp / (double) my nmax));
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
				GuiScale_setValue (my progressScale, 1000.0 * ((double) lastSample / (double) my nmax));
			} else {
				showMeter (me, nullptr, 0);
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
	#if cocoa
		return;
	#elif gtk
		return true;
	#else
		return false;
	#endif
}

static int portaudioStreamCallback (
    const void *input, void * /* output */,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* /* timeInfo */,
    PaStreamCallbackFlags /* statusFlags */,
    void *void_SoundRecorder)
{
	/*
	 * This procedure may be called at interrupt time.
	 * It therefore accesses only data that is constant during recording,
	 * namely me, my buffer, my numberOfChannels, and my nmax.
	 * The only thing it changes is my nsamp;
	 * the workProc will therefore have to take some care in accessing my nsamp (see there).
	 */
	SoundRecorder me = static_cast <SoundRecorder> (void_SoundRecorder);
	if (Melder_debug == 20)
		Melder_casual (U"The PortAudio stream callback receives ", frameCount, U" frames.");
	Melder_assert (my nsamp <= my nmax);
	unsigned long samplesLeft = my nmax - my nsamp;
	if (samplesLeft > 0) {
		unsigned long dsamples = samplesLeft > frameCount ? frameCount : samplesLeft;
		if (Melder_debug == 20)
			Melder_casual (U"play ", dsamples, U" ", Pa_GetStreamCpuLoad (my portaudioStream));
		memcpy (my buffer + my nsamp * my numberOfChannels, input, 2 * dsamples * my numberOfChannels);
		my nsamp += dsamples;
		if (my nsamp >= my nmax) return paComplete;
	} else /*if (my nsamp >= my nmax)*/ {
		my nsamp = my nmax;
		return paComplete;
	}
	return paContinue;
}

static void gui_button_cb_record (SoundRecorder me, GuiButtonEvent /* event */) {
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
				if (Melder_debug == 20)
					Melder_casual (U"Before Pa_OpenStream");
				PaError err = Pa_OpenStream (& my portaudioStream, & streamParameters, nullptr,
					theControlPanel. sampleRate, 0, paNoFlag, portaudioStreamCallback, (void *) me);
				if (Melder_debug == 20)
					Melder_casual (U"Pa_OpenStream returns ", (int) err);
				if (err)
					Melder_throw (U"open ", Melder_peek8to32 (Pa_GetErrorText (err)));
				Pa_StartStream (my portaudioStream);
				if (Melder_debug == 20)
					Melder_casual (U"Pa_StartStream returns ", (int) err);
				if (err)
					Melder_throw (U"start ", Melder_peek8to32 (Pa_GetErrorText (err)));
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
		Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics.get(), Graphics_WHITE);
		Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	} catch (MelderError) {
		Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics.get(), Graphics_WHITE);
		Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		my recording = false;
		Melder_flushError (U"Cannot record.");
	}
}

static void gui_button_cb_stop (SoundRecorder me, GuiButtonEvent /* event */) {
	// If split recording is enabled, we want do all the split recording stuff and not
	// only stop the recording.
	if ( my bSplitRec ) {
		do_split_recording(me, true);
	} else {
		stopRecording(me);
	}
}

static void gui_button_cb_play (SoundRecorder me, GuiButtonEvent /* event */) {
	if (my recording || my nsamp == 0) return;
	MelderAudio_play16 (my buffer, theControlPanel. sampleRate, my fakeMono ? my nsamp / 2 : my nsamp, my fakeMono ? 2 : my numberOfChannels, nullptr, nullptr);
}

static void publish_with_recnum (SoundRecorder me, int64 currentRecNumber) {
	autoSound sound;
	long nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	if (my nsamp == 0) return;
	double fsamp = theControlPanel. sampleRate;
	if (fsamp <= 0.0) fsamp = 44100.0;   // safe
	try {
		sound = Sound_createSimple (my numberOfChannels, (double) nsamp / fsamp, fsamp);
	} catch (MelderError) {
		Melder_flushError (U"You can still save to file.");
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
		autostring32 name = GuiText_getString (my soundName);
		// If split recording is enabled, append the current recording number to the name (with the separator).
		if ( my bSplitRec && currentRecNumber >= 0 ) {
			autostring32 separator = GuiText_getString(my separatorText);
			autoMelderString defaultName;
			MelderString_append(&defaultName, name.peek(), separator.peek(), currentRecNumber);
			Thing_setName(sound.get(), defaultName.string);
		} else {
			// Otherwise just save the sound object with its normal name.
			Thing_setName (sound.get(), name.peek());
		}
	}
	Editor_broadcastPublication (me, sound.move());
}

static void publish (SoundRecorder me) {
	publish_with_recnum(me, -1);
}

static void gui_button_cb_cancel (SoundRecorder me, GuiButtonEvent /* event */) {
	stopRecording (me);
	forget (me);
}

static void gui_button_cb_apply (SoundRecorder me, GuiButtonEvent /* event */) {
	stopRecording (me);
	publish (me);
	// Also save the file to disk if auto saving to disk is enabled (and split recording is disabled).
	// Note: If split recording is enabled, this callback should never get called, since the save button is disabled in that case.
	if ( my bAutoSave && ! my bSplitRec ) {
		do_auto_save(me, -1);
	}
}

static void gui_button_cb_ok (SoundRecorder me, GuiButtonEvent /* event */) {
	stopRecording (me);
	publish (me);
	// Also save the file to disk if auto saving to disk is enabled (and split recording is disabled).
	// Note: If split recording is enabled, this callback should never get called, since the save button is disabled in that case.
	if ( my bAutoSave && ! my bSplitRec ) {
		do_auto_save(me, -1);
	}
	forget (me);
}

/**	@summary Saves a recording as a wav file to disk.
 *	@param me The SoundRecorder object. This is needed to access its text fields and checkboxes.
 *	@param recNum The recording number to append to the filename (together with the separator character). Only used when split recording
 *	is enabled, otherwise no number and separator character are added to the filename.
 */
static void do_auto_save(SoundRecorder me, int64 recNum) {
	autostring32 savePathStr = GuiText_getString(my savePathText);
	autostring32 separatorStr = GuiText_getString(my separatorText);
	autostring32 nameStr = GuiText_getString(my soundName);
	autoMelderString filePathStr;
#if defined (_WIN32)
	const char32* pathSeparatorChar = U"\\";
#else
	const char32* pathSeparatorChar = U"/";
#endif

	// Make sure we use the correct path separators.
	long numberOfMatches; // Note: We don't actually use this variable.
#if defined (_WIN32)
	autostring32 correctedPath = str_replace_literal(savePathStr.peek(), U"/", U"\\", 0, &numberOfMatches);
#else
	autostring32 correctedPath = str_replace_literal(savePathStr.peek(), U"\\", U"/", 0, &numberOfMatches);
#endif
	// And also make sure it ends with a slash.
	if ( !Melder_stringMatchesCriterion(correctedPath.peek(), kMelder_string_ENDS_WITH, pathSeparatorChar) ) {
		MelderString_append(&filePathStr, correctedPath.peek(), pathSeparatorChar);
	}
// --- FIXME >>> This causes Praat to crash the second time we call this function from do_split_recording() (without closing the SoundRecorder). Does
// --- FIXME --- GuiText_setString() have issues with updating while we're in a callback?
//	// If the corrected path isn't the same as the one we retrieved from the auto save path field, we'll update that field.
//	if ( str32cmp(filePathStr.string, savePathStr.peek()) != 0 ) {
//		GuiText_setString(my savePathText, filePathStr.string);
//	}
// --- FIXME <<<
	// Append the name as a subdirectory, if needed.
	if ( my bUseSubdir ) {
		MelderString_append(&filePathStr, nameStr.peek(), pathSeparatorChar);
	}

	// Use mkdir to make sure the path exists before we try to write to it.
	autoMelderString mkdirCmdStr;
#if defined (_WIN32)
	MelderString_append(&mkdirCmdStr, U"mkdir \"", filePathStr.string, U"\"");
#else
	MelderString_append(&mkdirCmdStr, U"mkdir -p \"", filePathStr.string, U"\"");
#endif

	try {
		Melder_system(mkdirCmdStr.string);
	} catch ( MelderError ) {
		Melder_throw(U"System command \"", mkdirCmdStr.string, U"\" failed to create the specified directory");
	}

	// Append the file name to the path.
	if ( my bSplitRec ) {
		MelderString_append(&filePathStr, nameStr.peek(), separatorStr.peek(), recNum);
	} else {
		// Don't append the separator and recording number when split recording isn't enabled (i.e. when save or save&close was used).
		MelderString_append(&filePathStr, nameStr.peek());
	}

	// To prevent overwriting existing files we first check if the file already exists. If it does, we'll
	// append "(<retry_count>)" to the file name and check again. We'll repeat this until we find a file name
	// that doesn't exists (or until we reach 256 retries, in which case we just give up).
	int fileDuplicateNum = 1;
	autoMelderString tmpFilePathStr;
	MelderString_copy(&tmpFilePathStr, filePathStr.string, U".wav");
	structMelderFile outFile = { 0 };
	while ( fileDuplicateNum < 256) {
		Melder_relativePathToFile(tmpFilePathStr.string, &outFile);
		if ( MelderFile_exists(&outFile) ) {
			MelderString_copy(&tmpFilePathStr, filePathStr.string, U"(", fileDuplicateNum, U").wav");
		} else {
			// If the file doesn't exist yet, then it's safe to use this name to write the audio file.
			break;
		}
		++fileDuplicateNum;
	}
	if ( fileDuplicateNum == 256 ) {
		Melder_throw(U"Tried 255 different file names, but all existed. Giving up.");
	}

	// And finally write the audio file as a WAV.
	writeAudioFile(me, &outFile, Melder_WAV);
	// --- DEBUG >>>
	//MelderInfo_write(U"Should be saving file to output path: ", filePathStr.string, "\n");
	//MelderInfo_drain();
	// --- DEBUG <<<
}

/**	@summary Splits a recording.
 *
 *	This basically stops the current recording, save the requested things, and starts a
 *	new recording (unless @p last was @c true).
 *	@param me The SoundRecorder object.
 *	@param finalRecording When set to @c true no new recording will be started. This should be the
 *	case when the last fragment was recorded.
 */
static void do_split_recording(SoundRecorder me, bool finalRecording) {
	// Stop the current recording.
	stopRecording(me);

	int64 recNum = 1;
	// Determine the current recording number.
	if ( my recNumText ) {
		// Retrieve the current recording number or use 1 if it's not a number.
		autostring32 recNumStr = GuiText_getString(my recNumText);
		if ( Melder_isStringNumeric_nothrow(recNumStr.peek()) ) {
			recNum = Melder_atoi(recNumStr.peek());
			// Only allow positive numbers.
			if ( recNum < 0 ) {
				recNum = 1;
			}
		}
		// Update the recNumText's with the incremented value.
		GuiText_setString(my recNumText, Melder_integer(recNum + 1));
	}

// --- TODO >>> Do we want to make sure the separator isn't empty?
//	if ( my separatorText ) {
//		autostring32 separator = GuiText_getString(my separatorText);
//		if ( separator.peek()[0] == U'\0' ) {
//			separator = U"-";
//			// See FIXME below about updating while in callback.
//			GuiText_setString(my separatorText, separator.peek());
//		}
//	}
// --- TODO <<<

	// Save the file to the output path if auto saving is enabled.
	if ( my bAutoSave ) {
		do_auto_save(me, recNum);
	}

	// Always add the soundobject to the list when we're split recording.
	publish_with_recnum(me, recNum);

	// Start the next recording (unless we were told to stop).
	if ( !finalRecording ) {
		gui_button_cb_record(me, 0);
	}
}

static void gui_button_cb_split(SoundRecorder me, GuiButtonEvent /* event */) {
	do_split_recording(me, false);
}

static void gui_checkbutton_cb_split(SoundRecorder me, GuiCheckButtonEvent event) {
	// Update bSplitRec to the current value of the checkbox.
	my bSplitRec = GuiCheckButton_getValue(event->toggle);
}

static void gui_checkbutton_cb_autosave(SoundRecorder me, GuiCheckButtonEvent event) {
	// Update bAutoSave to the current value of the checkbox.
	my bAutoSave = GuiCheckButton_getValue(event->toggle);
}

static void gui_button_cb_path(SoundRecorder me, GuiButtonEvent /* event */) {
	autostring32 dirPath = GuiFileSelect_getDirectoryName(nullptr, U"Choose output directory");
	// If no string was returned (i.e. the user canceled), then don't update the path.
	if ( !dirPath.peek() ) {
		return;
	}
	GuiText_setString(my savePathText, dirPath.peek());
}

static void gui_checkbutton_cb_subdir(SoundRecorder me, GuiCheckButtonEvent event) {
	// Update bUseSubDir to the current value of the checkbox.
	my bUseSubdir = GuiCheckButton_getValue(event->toggle);
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
						Melder_throw (U"Audio device already in use.");
					else
						Melder_throw (U"Cannot open audio device.\n"
							U"Please switch on PortAudio in the Sound Recording Preferences.");
				}
				ioctl (my fd, SNDCTL_DSP_RESET, nullptr);
				ioctl (my fd, SNDCTL_DSP_SPEED, & sampleRate);
				ioctl (my fd, SNDCTL_DSP_SAMPLESIZE, & sampleSize);
				ioctl (my fd, SNDCTL_DSP_CHANNELS, (val = channels, & val));
				if (channels == 1 && val == 2) {
					close (my fd);
					Melder_throw (U"This sound card does not support mono.");
				}
				ioctl (my fd, SNDCTL_DSP_STEREO, & stereo);
				ioctl (my fd, SNDCTL_DSP_SETFMT, & format);
				fd_mixer = open ("/dev/mixer", O_WRONLY);		
				if (fd_mixer == -1) {
					Melder_throw (U"Cannot open /dev/mixer.");
				} else {
					int dev_mask = theControlPanel. inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
					if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1) {
						close (fd_mixer);
						Melder_throw (U"Can't set recording device in mixer.");
					}
					close (fd_mixer);
				}
			#endif
		}
	} catch (MelderError) {
		Melder_throw (U"16-bit audio recording not initialized.");
	}
}

static void gui_radiobutton_cb_input (SoundRecorder me, GuiRadioButtonEvent event) {
	Melder_casual (U"SoundRecorder:"
		U" setting the input source from ", theControlPanel. inputSource,
		U" to ", event -> position, U".");
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
				Melder_flushError ();
			}
		#elif defined (linux)
			int fd_mixer = open ("/dev/mixer", O_WRONLY);		
			if (fd_mixer == -1) {
				Melder_flushError (U"(Sound_record:) Cannot open /dev/mixer.");
			}
			int dev_mask = theControlPanel.inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
			if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
				Melder_flushError (U"(Sound_record:) Can't set recording device in mixer");
			close (fd_mixer);
		#endif
	}
}

static void gui_radiobutton_cb_fsamp (SoundRecorder me, GuiRadioButtonEvent event) {
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
		Melder_casual (U"SoundRecorder:"
			U" setting the sample rate from ", (long) theControlPanel. sampleRate,
			U" to ", (long) fsamp, U" Hz.");
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
		Melder_throw (U"Sampling frequency not changed.");
	}
}

static void gui_drawingarea_cb_resize (SoundRecorder me, GuiDrawingArea_ResizeEvent event) {
	if (! my graphics) return;   // could be the case in the very beginning
	Graphics_setWsViewport (my graphics.get(), 0, event -> width, 0, event -> height);
	Graphics_setWsWindow (my graphics.get(), 0, event -> width, 0, event -> height);
	Graphics_setViewport (my graphics.get(), 0, event -> width, 0, event -> height);
	Graphics_updateWs (my graphics.get());
}

void structSoundRecorder :: v_createChildren ()
{
	/* Channels */

	long y = 20 + Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, 10, 160, y, y + Gui_LABEL_HEIGHT, U"Channels:", 0);

	GuiRadioGroup_begin ();
	y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
	monoButton = GuiRadioButton_createShown (d_windowForm, 20, 170, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Mono", nullptr, nullptr, 0);
	y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
	stereoButton = GuiRadioButton_createShown (d_windowForm, 20, 170, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Stereo", nullptr, nullptr, 0);
	GuiRadioGroup_end ();

	/* Input source */
	
	y = 140 + Machine_getMenuBarHeight ();
	#if defined (_WIN32)
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, U"(use Windows mixer", 0);
		y += Gui_LABEL_HEIGHT + 10;
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, U"   without meters)", 0);
	#else
		GuiLabel_createShown (d_windowForm, 10, 170, y, y + Gui_LABEL_HEIGHT, U"Input source:", 0);
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
	GuiLabel_createShown (d_windowForm, 170, -170, y, y + Gui_LABEL_HEIGHT, U"Meter", GuiLabel_CENTRE);
	y += Gui_LABEL_HEIGHT;
	meter = GuiDrawingArea_createShown (d_windowForm, 170, -170, y, -200,
		nullptr, nullptr, nullptr, gui_drawingarea_cb_resize, this, GuiDrawingArea_BORDER);

	/* Sampling frequency */

	y = 20 + Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, -160, -10, y, y + Gui_LABEL_HEIGHT, U"Sampling frequency:", 0);
	GuiRadioGroup_begin ();
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
		if (fsamp_ [i]. canDo) {
			double fsamp = fsamp_ [i]. fsamp;
			y += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING;
			fsamp_ [i]. button = GuiRadioButton_createShown (d_windowForm,
				-150, -10, y, y + Gui_RADIOBUTTON_HEIGHT,
				Melder_cat (fsamp == floor (fsamp) ? Melder_integer ((long) fsamp) : Melder_fixed (fsamp, 5), U" Hz"),
				gui_radiobutton_cb_fsamp, this, fsamp == theControlPanel. sampleRate ? GuiRadioButton_SET : 0);
		}
	}
	GuiRadioGroup_end ();

	progressScale = GuiScale_createShown (d_windowForm,
		10, 350, -180, -140,
		0, 1000, 0, 0);

	y = 110;
	recordButton = GuiButton_createShown (d_windowForm, 20, 90, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Record", gui_button_cb_record, this, 0);
	splitButton = GuiButton_createShown (d_windowForm, 100, 170, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Split rec.", gui_button_cb_split, this, 0);
	stopButton = GuiButton_createShown (d_windowForm, 180, 250, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Stop", gui_button_cb_stop, this, 0);
	if (inputUsesPortAudio) {
		playButton = GuiButton_createShown (d_windowForm, 260, 330, -y - Gui_PUSHBUTTON_HEIGHT, -y,
			U"Play", gui_button_cb_play, this, 0);
	} else {
		#if defined (_WIN32) || defined (macintosh)
			playButton = GuiButton_createShown (d_windowForm, 260, 330, -y - Gui_PUSHBUTTON_HEIGHT, -y,
				U"Play", gui_button_cb_play, this, 0);
		#endif
	}
	
	GuiLabel_createShown (d_windowForm, -200, -130, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2, U"Name:", GuiLabel_RIGHT);
	soundName = GuiText_createShown (d_windowForm, -120, -20, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2, 0);
	GuiText_setString (soundName, U"untitled");

	// By default set the check boxes to be "checked".
	bSplitRec = bAutoSave = bUseSubdir = true;
	bShowingSplitRecOpts = bSplitRec;
	bShowingAutoSaveOpts = bAutoSave;

	y = 80;
	enableSplitRecCheckbox = GuiCheckButton_createShown (d_windowForm, 20, -245, -y - Gui_TEXTFIELD_HEIGHT, -y,
		U"Enable split recording (Auto saves to Object list)", gui_checkbutton_cb_split, this, bSplitRec ? GuiCheckButton_SET : 0);
	GuiCheckButton_setValue(enableSplitRecCheckbox, bSplitRec);
	recNumLbl = GuiLabel_createShown (d_windowForm, -240, -150, -y - Gui_TEXTFIELD_HEIGHT, -y, U"Start number:", GuiLabel_RIGHT);
	recNumText = GuiText_createShown (d_windowForm, -148, -123, -y - Gui_TEXTFIELD_HEIGHT, -y, 0);
	GuiText_setString (recNumText, U"1");
	separatorLbl = GuiLabel_createShown (d_windowForm, -122, -47, -y - Gui_TEXTFIELD_HEIGHT, -y, U"Separator:", GuiLabel_RIGHT);
	separatorText = GuiText_createShown (d_windowForm, -45, -20, -y - Gui_TEXTFIELD_HEIGHT, -y, 0);
	GuiText_setString (separatorText, U"-");

	y = 50;
	autoSaveCheckbox = GuiCheckButton_createShown (d_windowForm, 20, 117, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2,
		U"Auto save files", gui_checkbutton_cb_autosave, this, bAutoSave ? GuiCheckButton_SET : 0);
	GuiCheckButton_setValue(autoSaveCheckbox, bAutoSave);
	autoSaveLbl = GuiLabel_createShown (d_windowForm, 117, 140, -y + 2 - Gui_TEXTFIELD_HEIGHT, -y + 2, U"to:", 0);
	savePathText = GuiText_createShown (d_windowForm, 140, -230, -y - Gui_TEXTFIELD_HEIGHT, -y, 0);
	structMelderDir homeDir = { { 0 } };
	Melder_getHomeDir(&homeDir);
	char32* saveDirectory = Melder_dirToPath(&homeDir);
	GuiText_setString (savePathText, saveDirectory);

	pathButton = GuiButton_createShown (d_windowForm, -220, -190, -y - 1 - Gui_TEXTFIELD_HEIGHT, -y - 1,
		U"...", gui_button_cb_path, this, 0);

	subdirCheckbox = GuiCheckButton_createShown (d_windowForm, -180, -20, -y - 2 - Gui_TEXTFIELD_HEIGHT, -y - 2,
		U"Create subdirectories", gui_checkbutton_cb_subdir, this, bUseSubdir ? GuiCheckButton_SET : 0);
	GuiCheckButton_setValue(subdirCheckbox, bUseSubdir);

	y = 20;
	cancelButton = GuiButton_createShown (d_windowForm, -350, -280, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Close", gui_button_cb_cancel, this, 0);
	applyButton = GuiButton_createShown (d_windowForm, -270, -170, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Save to list", gui_button_cb_apply, this, GuiButton_DEFAULT);
	okButton = GuiButton_createShown (d_windowForm, -160, -20, -y - Gui_PUSHBUTTON_HEIGHT, -y,
		U"Save to list & Close", gui_button_cb_ok, this, 0);
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
	MelderFile_writeAudioFileTrailer (file, audioFileType, lround (theControlPanel. sampleRate), nsamp, 1, 16);
	mfile.close ();
}

static void writeAudioFile (SoundRecorder me, MelderFile file, int audioFileType) {
	try {
		if (my fakeMono) {
			writeFakeMonoFile (me, file, audioFileType);
		} else {
			MelderFile_writeAudioFile (file, audioFileType, my buffer, lround (theControlPanel. sampleRate), my nsamp, my numberOfChannels, 16);
		}
	} catch (MelderError) {
		Melder_throw (U"Audio file not written.");
	}
}

static void menu_cb_writeWav (SoundRecorder me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_WRITE (U"Save as WAV file", nullptr)
		char32 *name = GuiText_getString (my soundName);
		Melder_sprint (defaultName,300, name, U".wav");
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_WAV);
	EDITOR_END
}

static void menu_cb_writeAifc (SoundRecorder me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_WRITE (U"Save as AIFC file", nullptr)
		char32 *name = GuiText_getString (my soundName);
		Melder_sprint (defaultName,300, name, U".aifc");
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_AIFC);
	EDITOR_END
}

static void menu_cb_writeNextSun (SoundRecorder me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_WRITE (U"Save as NeXT/Sun file", nullptr)
		char32 *name = GuiText_getString (my soundName);
		Melder_sprint (defaultName,300, name, U".au");
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NEXT_SUN);
	EDITOR_END
}

static void menu_cb_writeNist (SoundRecorder me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_WRITE (U"Save as NIST file", nullptr)
		char32 *name = GuiText_getString (my soundName);
		Melder_sprint (defaultName,300, name, U".nist");
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NIST);
	EDITOR_END
}

static void updateMenus (SoundRecorder me) {
	GuiMenuItem_check (my d_meterIntensityButton,
		my p_meter_which == kSoundRecorder_meter_INTENSITY);
	GuiMenuItem_check (my d_meterCentreOfGravityVersusIntensityButton,
		my p_meter_which == kSoundRecorder_meter_CENTRE_OF_GRAVITY_VERSUS_INTENSITY);
}

static void menu_cb_intensity (SoundRecorder me, EDITOR_ARGS_DIRECT) {
	my pref_meter_which () = my p_meter_which = kSoundRecorder_meter_INTENSITY;
	updateMenus (me);
}
static void menu_cb_centreOfGravityVersusIntensity (SoundRecorder me, EDITOR_ARGS_DIRECT) {
	my pref_meter_which () = my p_meter_which = kSoundRecorder_meter_CENTRE_OF_GRAVITY_VERSUS_INTENSITY;
	updateMenus (me);
}

static void menu_cb_SoundRecorder_help (SoundRecorder, EDITOR_ARGS_DIRECT) { Melder_help (U"SoundRecorder"); }

void structSoundRecorder :: v_createMenus () {
	SoundRecorder_Parent :: v_createMenus ();
	Editor_addCommand (this, U"File", U"Save as WAV file...", 0, menu_cb_writeWav);
	Editor_addCommand (this, U"File", U"Save as AIFC file...", 0, menu_cb_writeAifc);
	Editor_addCommand (this, U"File", U"Save as NeXT/Sun file...", 0, menu_cb_writeNextSun);
	Editor_addCommand (this, U"File", U"Save as NIST file...", 0, menu_cb_writeNist);
	Editor_addCommand (this, U"File", U"-- write --", 0, 0);
	Editor_addMenu (this, U"Meter", 0);
	d_meterIntensityButton =
		Editor_addCommand (this, U"Meter", U"Intensity", GuiMenu_RADIO_FIRST, menu_cb_intensity);
	d_meterCentreOfGravityVersusIntensityButton =
		Editor_addCommand (this, U"Meter", U"Centre of gravity ~ intensity", GuiMenu_RADIO_NEXT, menu_cb_centreOfGravityVersusIntensity);
}

void structSoundRecorder :: v_createHelpMenuItems (EditorMenu menu) {
	SoundRecorder_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"SoundRecorder help", '?', menu_cb_SoundRecorder_help);
}

autoSoundRecorder SoundRecorder_create (int numberOfChannels) {
	try {
		autoSoundRecorder me = Thing_new (SoundRecorder);
		my inputUsesPortAudio =
			#if defined (_WIN32)
				MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem_MME_VIA_PORTAUDIO;
			#elif defined (macintosh)
				MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem_COREAUDIO_VIA_PORTAUDIO;
			#else
				MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem_ALSA_VIA_PORTAUDIO;
			#endif

		if (my inputUsesPortAudio) {
		} else {
			#if defined (_WIN32)
				UINT numberOfDevices = waveInGetNumDevs (), i;
				WAVEINCAPS caps;
				MMRESULT err;
				if (numberOfDevices == 0)
					Melder_throw (U"No sound input devices available.");
				err = waveInGetDevCaps (WAVE_MAPPER, & caps, sizeof (WAVEINCAPS));
				if (numberOfChannels == 2 && caps. wChannels < 2)
					Melder_throw (U"Your computer does not support stereo sound input.");
				/* BUG: should we ask whether 16 bit is supported? */
				for (i = 0; i < numberOfDevices; i ++) {
					waveInGetDevCaps (i, & caps, sizeof (WAVEINCAPS));
					/*Melder_casual (U"Name of device ", i, U": ", Melder_peek16to32 (aps. szPname));*/
				}
			#elif defined (macintosh)
				SInt32 soundFeatures;
				if (Gestalt (gestaltSoundAttr, & soundFeatures) ||
						! (soundFeatures & (1 << gestaltSoundIOMgrPresent)) ||
						! (soundFeatures & (1 << gestaltBuiltInSoundInput)) ||
						! (soundFeatures & (1 << gestaltHasSoundInputDevice)))
					Melder_throw (U"Your computer does not support sound input.");
				if (! (soundFeatures & (1 << gestalt16BitSoundIO)) ||   // hardware
					! (soundFeatures & (1 << gestaltStereoInput)) ||   // hardware
					! (soundFeatures & (1 << gestalt16BitAudioSupport)))   // software
					Melder_throw (U"Your computer does not support stereo sound input.");
			#endif
		}
		my numberOfChannels = numberOfChannels;
		if (sizeof (short) != 2)
			Melder_throw (U"Long shorts!!!!!");
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
		if (preferences.bufferSizeInMegabytes < 1) preferences.bufferSizeInMegabytes = 1;   // validate preferences
		if (preferences.bufferSizeInMegabytes > 1000) preferences.bufferSizeInMegabytes = 1000;
		if (! my buffer) {
			long nmax_bytes_pref = preferences.bufferSizeInMegabytes * 1000000;
			long nmax_bytes = my inputUsesPortAudio ? nmax_bytes_pref :
				#if defined (_WIN32)
					66150000;   // the maximum physical buffer on Windows XP; shorter than in Windows 98, alas.
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
		Melder_assert (my buffer);

		/*
		 * Count the number of input devices and sources.
		 */
		if (my inputUsesPortAudio) {
			static bool paInitialized = false;
			if (! paInitialized) {
				PaError err = Pa_Initialize ();
				if (Melder_debug == 20)
					Melder_casual (U"init ", Melder_peek8to32 (Pa_GetErrorText (err)));
				paInitialized = true;
				if (Melder_debug == 20) {
					PaHostApiIndex hostApiCount = Pa_GetHostApiCount ();
					Melder_casual (U"host API count ", hostApiCount);
					for (PaHostApiIndex iHostApi = 0; iHostApi < hostApiCount; iHostApi ++) {
						const PaHostApiInfo *hostApiInfo = Pa_GetHostApiInfo (iHostApi);
						PaHostApiTypeId type = hostApiInfo -> type;
						Melder_casual (U"host API ", iHostApi, U": ", type, U", \"", Melder_peek8to32 (hostApiInfo -> name), U"\" ", hostApiInfo -> deviceCount);
					}
					PaHostApiIndex defaultHostApi = Pa_GetDefaultHostApi ();
					Melder_casual (U"default host API ", defaultHostApi);
					PaDeviceIndex deviceCount = Pa_GetDeviceCount ();
					Melder_casual (U"device count ", deviceCount);
				}
			}
			PaDeviceIndex deviceCount = Pa_GetDeviceCount ();
			for (PaDeviceIndex idevice = 0; idevice < deviceCount; idevice ++) {
				const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo (idevice);
				if (Melder_debug == 20)
					Melder_casual (
						U"Device \"", Melder_peek8to32 (deviceInfo -> name),
						U"\", input ", deviceInfo -> maxInputChannels,
						U", output ", deviceInfo -> maxOutputChannels,
						U", sample rate ", deviceInfo -> defaultSampleRate
					);
				if (deviceInfo -> maxInputChannels > 0 && my numberOfInputDevices < SoundRecorder_IDEVICE_MAX) {
					my device_ [++ my numberOfInputDevices]. canDo = true;
					str32ncpy (my device_ [my numberOfInputDevices]. name, Melder_peek8to32 (deviceInfo -> name), 40);
					my device_ [my numberOfInputDevices]. name [40] = U'\0';
					my deviceInfos [my numberOfInputDevices] = deviceInfo;
					my deviceIndices [my numberOfInputDevices] = idevice;
				}
			}
			if (my numberOfInputDevices == 0)
				Melder_throw (U"No input devices available.");
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				// No device info: use Windows mixer.
			#else
				my device_ [1]. canDo = true;
				str32cpy (my device_ [1]. name, U"Microphone");
				my device_ [2]. canDo = true;
				str32cpy (my device_ [2]. name, U"Line");
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
		initialize (me.get());

        Editor_init (me.get(), 100, 100, 600, 550, U"SoundRecorder", nullptr);
		my graphics = Graphics_create_xmdrawingarea (my meter);
		Melder_assert (my graphics);
		Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics.get(), Graphics_WHITE);
		Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);

struct structGuiDrawingArea_ResizeEvent event { my meter, 0 };
event. width  = GuiControl_getWidth  (my meter);
event. height = GuiControl_getHeight (my meter);
gui_drawingarea_cb_resize (me.get(), & event);

		#if cocoa
			CFRunLoopTimerContext context = { 0, me.get(), nullptr, nullptr, nullptr };
			my d_cocoaTimer = CFRunLoopTimerCreate (nullptr, CFAbsoluteTimeGetCurrent () + 0.02,
				0.02, 0, 0, workProc, & context);
			CFRunLoopAddTimer (CFRunLoopGetCurrent (), my d_cocoaTimer, kCFRunLoopCommonModes);
		#elif gtk
			g_idle_add (workProc, me.get());
		#elif motif
			my workProcId = GuiAddWorkProc (workProc, me.get());
		#endif
		updateMenus (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundRecorder not created.");
	}
}

/* End of file SoundRecorder.cpp */
