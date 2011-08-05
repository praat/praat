/* SoundRecorder.cpp
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
/* Linux code originally by Darryl Purnell, Pretoria */
/* GTK conversion includes work by fb */

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
#include "Editor.h"
#include "machine.h"
#include "EditorM.h"
#include "Preferences.h"
#if defined (macintosh)
	#include "pa_mac_core.h"
	#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
#endif

Thing_implement (SoundRecorder, Editor, 0);

static struct {
	int bufferSize_MB;
} preferences;

void SoundRecorder_prefs (void) {
	Preferences_addInt (L"SoundRecorder.bufferSize_MB", & preferences.bufferSize_MB, 20);
}

int SoundRecorder_getBufferSizePref_MB (void) { return preferences.bufferSize_MB; }
void SoundRecorder_setBufferSizePref_MB (int size) { preferences.bufferSize_MB = size < 1 ? 1 : size > 1000 ? 1000: size; }

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
	wchar messageText [MAXERRORLENGTH];
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

#if defined (macintosh)
static const char *errString (long err) {
	switch (err) {
		/* -54 */ case permErr: return "Attempt to open locked file for writing.";
		/* -128 */ case userCanceledErr: return "User cancelled the operation.";
		/* -200 */ case noHardwareErr: return "No such sound hardware.";
		/* -220 */ case siNoSoundInHardware: return "No sound input hardware available.";
		/* -221 */ case siBadSoundInDevice: return "Invalid sound input device.";
		/* -222 */ case siNoBufferSpecified: return "No buffer specified for synchronous recording.";
		/* -223 */ case siInvalidCompression: return "Invalid compression type.";
		/* -224 */ case siHardDriveTooSlow: return "Hard drive too slow to record.";
		/* -227 */ case siDeviceBusyErr: return "Sound input device is busy.";
		/* -228 */ case siBadDeviceName: return "Invalid device name.";
		/* -229 */ case siBadRefNum: return "Invalid reference number.";
		/* -231 */ case siUnknownInfoType: return "Unknown type of information.";
		/* -232 */ case siUnknownQuality: return "Unknown quality.";
		default: return NULL; break;
	}
	return NULL;
}
static void onceError (const char *routine, long err) {
	const char *string = errString (err);
	if (string) Melder_throw ("(", routine, ":) ", string);
	else Melder_throw ("(", routine, ":) Error ", err);
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
					win_waveInReset (me); therror
					if (my nsamp == 0)
						my nsamp = my waveHeader [0]. dwBytesRecorded / (sizeof (short) * my numberOfChannels);
					if (my nsamp > my nmax)
						my nsamp = my nmax;
					win_waveInUnprepareHeader (me, 0); therror
					win_waveInClose (me); therror
				#elif defined (macintosh)
					OSErr err;
					short recordingStatus, meterLevel;
					unsigned long totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
					err = SPBGetRecordingStatus (my refNum, & recordingStatus, & meterLevel,
							& totalSamplesToRecord, & numberOfSamplesRecorded,
							& totalMsecsToRecord, & numberOfMsecsRecorded);
					if (err != noErr)
						Melder_throw ("SPBGetRecordingStatus ", err);
					/* Melder_assert (meterLevel >= 0); Melder_assert (meterLevel <= 255); */
					if (totalSamplesToRecord == 0)
						my nsamp = my nmax;
					else
						my nsamp = numberOfSamplesRecorded / (sizeof (short) * my numberOfChannels);   /* From Mac "samples" to Mac "frames" (our "samples"). */
					err = SPBStopRecording (my refNum);
					if (err != noErr)
						Melder_throw ("SPBStopRecording ", err);
					my spb. bufferPtr = NULL;
					err = SPBRecord (& my spb, true);
					if (err != noErr)
						Melder_throw ("SPBRecord ", err);
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
			if (refNum) SPBCloseDevice (refNum);
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
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	//#ifndef _WIN32
	//	Graphics_setColour (my graphics, Graphics_WHITE);
	//	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	//#endif
	Graphics_setColour (my graphics, Graphics_BLACK);
	if (nsamp < 1) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		#if defined (macintosh)
			Graphics_setColour (my graphics, Graphics_WHITE);
			Graphics_fillRectangle (my graphics, 0.2, 0.8, 0.3, 0.7);
			Graphics_setColour (my graphics, Graphics_BLACK);
		#endif
		Graphics_text (my graphics, 0.5, 0.5, L"Not recording.");
		return;
	}
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
}

static bool tooManySamplesInBufferToReturnToGui (SoundRecorder me) {
	return false;
}

static long getMyNsamp (SoundRecorder me) {
	volatile long nsamp = my nsamp;   // Prevent inlining.
	return nsamp;
}

static Boolean workProc (XtPointer void_me) {
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
			#if defined (macintosh)
				OSErr err;
				short macSource, isource;
				Str255 pdeviceName;
				err = SPBGetDeviceInfo (my refNum, siDeviceName, & pdeviceName);
				if (err != noErr)
					onceError ("SPBGetDeviceInfo (deviceName)", err);
				err = SPBGetDeviceInfo (my refNum, siInputSource, & macSource);
					onceError ("SPBGetDeviceInfo (inputSource)", err);
				for (isource = 1; isource <= my numberOfInputDevices; isource ++) {
					if (strequ ((const char *) & pdeviceName, (const char *) my hybridDeviceNames [isource]) &&
							macSource == my macSource [isource]) {
						theControlPanel. inputSource = isource;
						break;
					}
				}
			#endif
		}

		/* Set the buttons according to the audio parameters. */

		if (my recordButton) GuiObject_setSensitive (my recordButton, ! my recording);
		if (my stopButton) GuiObject_setSensitive (my stopButton, my recording);
		if (my playButton) GuiObject_setSensitive (my playButton, ! my recording && my nsamp > 0);
		if (my applyButton) GuiObject_setSensitive (my applyButton, ! my recording && my nsamp > 0);
		if (my okButton) GuiObject_setSensitive (my okButton, ! my recording && my nsamp > 0);
		if (my monoButton) GuiRadioButton_setValue (my monoButton, my numberOfChannels == 1);
		if (my stereoButton) GuiRadioButton_setValue (my stereoButton, my numberOfChannels == 2);
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button)
				GuiRadioButton_setValue (my fsamp_ [i]. button, theControlPanel. sampleRate == my fsamp_ [i]. fsamp);
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
			if (my device_ [i]. button)
				GuiRadioButton_setValue (my device_ [i]. button, theControlPanel. inputSource == i);
		if (my monoButton) GuiObject_setSensitive (my monoButton, ! my recording);
		if (my stereoButton) GuiObject_setSensitive (my stereoButton, ! my recording);
		for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
			if (my fsamp_ [i]. button)
				GuiObject_setSensitive (my fsamp_ [i]. button, ! my recording);
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
			if (my device_ [i]. button)
				GuiObject_setSensitive (my device_ [i]. button, ! my recording);

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
					#if gtk
						gtk_range_set_value (GTK_RANGE (my progressScale), (1000.0 * ((double) my nsamp / (double) my nmax)));
					#elif motif
						XmScaleSetValue (my progressScale, (int) (1000.0f * ((float) my nsamp / (float) my nmax)));
					#endif
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
						OSErr err;
						short recordingStatus, meterLevel;
						unsigned long totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
						err = SPBGetRecordingStatus (my refNum, & recordingStatus, & meterLevel,
								& totalSamplesToRecord, & numberOfSamplesRecorded,
								& totalMsecsToRecord, & numberOfMsecsRecorded);
						if (err != noErr)
							onceError ("SPBGetRecordingStatus", err);
						if (totalSamplesToRecord == 0)
							my nsamp = my nmax;
						else
							my nsamp = numberOfSamplesRecorded / (sizeof (short) * my numberOfChannels);
						lastSample = my nsamp;
					#endif
				}
				long firstSample = lastSample - 1000;
				if (firstSample < 0) firstSample = 0;
				showMeter (me, my buffer + firstSample * my numberOfChannels, lastSample - firstSample);
				#if gtk
					gtk_range_set_value (GTK_RANGE (my progressScale), (1000.0 * ((double) lastSample / (double) my nmax)));
				#elif motif
					XmScaleSetValue (my progressScale, (int) (1000.0f * ((float) lastSample / (float) my nmax)));
				#endif
			} else {
				showMeter (me, NULL, 0);
			}
		}
		#if gtk
		 return TRUE;
		#else
		 return False;
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
					OSErr err;
					err = SPBStopRecording (my refNum);
					if (err != noErr) { onceError ("SPBStopRecording", err); return; }
					my spb. bufferPtr = (char *) my buffer;
					my spb. bufferLength = my spb. count = my nmax * (sizeof (short) * my numberOfChannels);
					err = SPBRecord (& my spb, true);   /* Asynchronous. */
					if (err == notEnoughMemoryErr) {
						Melder_flushError ("Out of memory. Quit other programs."); return;
					} else if (err != noErr) { onceError ("SPBRecord", err); return; }
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
	Sound sound = NULL;
	long i, nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	double fsamp = theControlPanel. sampleRate;
	if (my nsamp == 0) return;
	if (fsamp <= 0) fsamp = 48000.0;   /* Safe. */
	try {
		sound = Sound_createSimple (my numberOfChannels, (double) nsamp / fsamp, fsamp);
	} catch (MelderError) {
		Melder_flushError ("You can still save to file.");
		return;
	}
	if (my fakeMono) {
		for (i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = (my buffer [i + i - 2] + my buffer [i + i - 1]) * (1.0 / 65536);
	} else if (my numberOfChannels == 1) {
		for (i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = my buffer [i - 1] * (1.0 / 32768);
	} else {
		for (i = 1; i <= nsamp; i ++) {
			sound -> z [1] [i] = my buffer [i + i - 2] * (1.0 / 32768);
			sound -> z [2] [i] = my buffer [i + i - 1] * (1.0 / 32768);
		}
	}
	if (my soundName) {
		wchar *name = GuiText_getString (my soundName);
		Thing_setName (sound, name);
		Melder_free (name);
	}
	if (my publishCallback)
		my publishCallback (me, my publishClosure, sound);
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
static void gui_cb_apply (GuiObject widget, XtPointer void_me, XtPointer call) {
	(void) widget;
	(void) call;
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
				unsigned long sampleRate_uf = theControlPanel. sampleRate * 65536L;
				short numberOfChannels = my numberOfChannels, continuous = TRUE, sampleSize = 16, async;
				char levelMeterOnOff = 1;
				short inputSource = theControlPanel. inputSource, irate;
				OSType compressionType = 'NONE';
				struct { Handle dummy1; short dummy2, number; Handle handle; } sampleRateInfo;   // make sure that number is adjacent to handle
				if (SPBOpenDevice (my hybridDeviceNames [inputSource], siWritePermission, & my refNum) != noErr)
					Melder_throw ("Cannot open audio input device.");
				/*
					From Apple:
					"Get the range of sample rates this device can produce.
					The infoData  parameter points to an integer, which is the number of sample rates the device supports, followed by a handle.
					The handle references a list of sample rates, each of type Fixed .
					If the device can record a range of sample rates, the number of sample rates is set to 0 and the handle contains two rates,
					the minimum and the maximum of the range of sample rates.
					Otherwise, a list is returned that contains the sample rates supported.
					In order to accommodate sample rates greater than 32 kHz, the most significant bit is not treated as a sign bit;
					instead, that bit is interpreted as having the value 32,768."
				 */
				SPBGetDeviceInfo (my refNum, siSampleRateAvailable, & sampleRateInfo. number);
				if (sampleRateInfo. number == 0) {
				} else {
					for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
						my fsamp_ [i]. canDo = false;
					}
					for (irate = 1; irate <= sampleRateInfo. number; irate ++) {
						Fixed rate_fixed = (* (Fixed **) sampleRateInfo. handle) [irate - 1];
						unsigned short rate_ushort = * (unsigned short *) & rate_fixed;
						switch (rate_ushort) {
							case 0: my fsamp_ [SoundRecorder_IFSAMP_44100]. canDo = true,
									my fsamp_ [SoundRecorder_IFSAMP_48000]. canDo = true; break;   /* BUG */
							case 8000: my fsamp_ [SoundRecorder_IFSAMP_8000]. canDo = true; break;
							case 11025: my fsamp_ [SoundRecorder_IFSAMP_11025]. canDo = true; break;
							case 12000: my fsamp_ [SoundRecorder_IFSAMP_12000]. canDo = true; break;
							case 16000: my fsamp_ [SoundRecorder_IFSAMP_16000]. canDo = true; break;
							case 22050: my fsamp_ [SoundRecorder_IFSAMP_22050]. canDo = true; break;
							case 22254: my fsamp_ [SoundRecorder_IFSAMP_22254]. canDo = true; break;
							case 24000: my fsamp_ [SoundRecorder_IFSAMP_24000]. canDo = true; break;
							case 32000: my fsamp_ [SoundRecorder_IFSAMP_32000]. canDo = true; break;
							case 44100: my fsamp_ [SoundRecorder_IFSAMP_44100]. canDo = true; break;
							case 48000: my fsamp_ [SoundRecorder_IFSAMP_48000]. canDo = true; break;
							case 64000: my fsamp_ [SoundRecorder_IFSAMP_64000]. canDo = true; break;
							default: Melder_warning3 (L"Your computer seems to support a sampling frequency of ", Melder_integer (rate_ushort), L" Hz. "
								"Contact the author (paul.boersma@uva.nl) to make this frequency available to you.");
						}
					}
				}
				if (SPBSetDeviceInfo (my refNum, siInputSource, & my macSource [inputSource]) != noErr)
					Melder_warning ("Cannot change input source.");
				if (SPBSetDeviceInfo (my refNum, siSampleRate, & sampleRate_uf) != noErr) {
					Melder_warning ("Cannot set sampling frequency to %.5f Hz.", theControlPanel. sampleRate);
					theControlPanel. sampleRate = 44100;
				}
				if (SPBSetDeviceInfo (my refNum, siNumberChannels, & numberOfChannels) != noErr) {
					if (my numberOfChannels == 1) {
						my fakeMono = true;
					} else {
						Melder_throw ("Cannot set to stereo.");
					}
				}
				if (SPBSetDeviceInfo (my refNum, siCompressionType, & compressionType) != noErr)
					Melder_throw ("Cannot set sample format to linear.");
				if (SPBSetDeviceInfo (my refNum, siSampleSize, & sampleSize) != noErr)
					Melder_throw ("Cannot set to ", sampleSize, L"-bit.");
				if (SPBSetDeviceInfo (my refNum, siLevelMeterOnOff, & levelMeterOnOff) != noErr)
					Melder_throw ("Cannot set level meter to ON.");
				if (! my synchronous && (SPBGetDeviceInfo (my refNum, siAsync, & async) != noErr || ! async)) {
					static bool warned = false;
					my synchronous = true;
					if (! warned) { Melder_warning1 (L"Recording must and will be synchronous on this computer."); warned = true; }
				}
				if (my synchronous && SPBSetDeviceInfo (my refNum, siContinuous, & continuous) != noErr)
					Melder_throw ("Cannot set continuous recording.");
				my spb. inRefNum = my refNum;
				if (! my synchronous) {
					OSErr err;
					if (my recording) {
						my spb. bufferPtr = (char *) my buffer;
						my spb. bufferLength = my spb. count = my nmax * (sizeof (short) * my numberOfChannels);
					} else {
						my spb. bufferPtr = NULL;
					}
					err = SPBRecord (& my spb, true);
					if (err != noErr)
						onceError ("SPBRecord", err);
				}
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
	theControlPanel. inputSource = 1;   // default
	Melder_assert (event -> toggle != NULL);
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
		if (event -> toggle == my device_ [i]. button) {
			theControlPanel. inputSource = i;
		}
	}

	/* Set system's input source. */
	if (my inputUsesPortAudio) {
		// deferred to the start of recording
	} else {
		#if defined (_WIN32)
			// deferred to the start of recording
		#elif defined (macintosh)
			SPBCloseDevice (my refNum);
			try {
				initialize (me); therror
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
				SPBCloseDevice (my refNum);
				initialize (me); therror
			#elif defined (linux)		
				close (my fd);
				initialize (me); therror
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

void structSoundRecorder :: v_createChildren () {
	GuiObject form, channels, inputSources, meterBox, recstopplayBox, nameBox, fsampBox, dlgCtrlBox;
	
	#if gtk
		form = dialog;
		GuiObject hbox1 = gtk_hbox_new (FALSE, 3);		// contains {Channels, Input source}, Meter, Sampling freq
		GuiObject hbox2 = gtk_hbox_new (TRUE, 3); 		// contains {slider, {Record, Stop}}, {Name, label}
		gtk_box_pack_start (GTK_BOX (form), hbox1, TRUE, TRUE, 3);
		gtk_box_pack_start (GTK_BOX (form), hbox2, FALSE, FALSE, 3);
	#elif motif
		/* TODO */
		form = XmCreateForm (dialog, "form", NULL, 0);
		XtVaSetValues (form,
			XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
			XmNbottomAttachment, XmATTACH_FORM,
			XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
			NULL);
		
		meterBox = form;
	#endif
	
	#if gtk
		GuiObject h1vbox = gtk_vbox_new (FALSE, 3);
		gtk_box_pack_start (GTK_BOX (hbox1), h1vbox, FALSE, FALSE, 3);
		GuiObject channels_frame = gtk_frame_new ("Channels");
		gtk_box_pack_start (GTK_BOX (h1vbox), channels_frame, FALSE, FALSE, 3);
		channels = gtk_vbox_new (TRUE, 3);
		gtk_container_add (GTK_CONTAINER (channels_frame), channels);
	#elif motif
		GuiLabel_createShown (form, 10, 160, 20, Gui_AUTOMATIC, L"Channels:", 0);
		channels = XmCreateRadioBox (form, "channels", NULL, 0);
		XtVaSetValues (channels,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 45,
			XmNwidth, 150,
			NULL);
		inputSources = form;
	#endif
	
	monoButton = GuiRadioButton_createShown (channels, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC,
		L"Mono", NULL, NULL, 0);
	stereoButton = GuiRadioButton_createShown (channels, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC,
		L"Stereo", NULL, NULL, 0);
	GuiObject_show (channels);
	
	#if gtk
		GuiRadioButton_setGroup (stereoButton, GuiRadioButton_getGroup (monoButton));
		
		GuiObject input_sources_frame = gtk_frame_new ("Input source");
		gtk_box_pack_start (GTK_BOX (h1vbox), input_sources_frame, FALSE, FALSE, 3);
		inputSources = gtk_vbox_new (TRUE, 3);
		gtk_container_add (GTK_CONTAINER (input_sources_frame), inputSources);
	#endif
	
	long y = 110, dy = 25;
	#if defined (_WIN32)
		GuiLabel_createShown (inputSources, 10, 160, y, Gui_AUTOMATIC, L"(use Windows mixer", 0);
		GuiLabel_createShown (inputSources, 10, 160, y + dy, Gui_AUTOMATIC, L"   without meters)", 0);
	#else
		#if gtk
		GSList *input_radio_list = NULL;
		#else
		GuiLabel_createShown (inputSources, 10, 160, y, Gui_AUTOMATIC, L"Input source:", 0);
		#endif
		for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
			if (device_ [i]. canDo) {
				y += dy;
				device_ [i]. button = GuiRadioButton_createShown (inputSources, 10, 160, y, Gui_AUTOMATIC,
					device_ [i]. name, gui_radiobutton_cb_input, this, 0);
				#if gtk
				if (input_radio_list)
					GuiRadioButton_setGroup (device_ [i]. button, input_radio_list);
				input_radio_list = (GSList *) GuiRadioButton_getGroup (device_ [i]. button);
				#endif
			}
		}
	#endif
	
	#if gtk
		meterBox = gtk_vbox_new (FALSE, 3);
		gtk_box_pack_start (GTK_BOX (hbox1), meterBox, TRUE, TRUE, 3);
	#endif
	
	GuiLabel_createShown (meterBox, 170, -170, 20, Gui_AUTOMATIC, L"Meter", GuiLabel_CENTRE);
	meter = GuiDrawingArea_createShown (meterBox, 170, -170, 45, -150,
		NULL, NULL, NULL, gui_drawingarea_cb_resize, this, GuiDrawingArea_BORDER);

	#if gtk
		gtk_widget_set_double_buffered (meter, FALSE);
		
		GuiObject h1vbox2 = gtk_vbox_new (FALSE, 3);
		GuiObject fsampBox_frame = gtk_frame_new ("Sampling frequency");
		fsampBox = gtk_vbox_new (TRUE, 3);
		GSList *fsamp_radio_list = NULL;
		
		gtk_box_pack_start (GTK_BOX (hbox1), h1vbox2, FALSE, FALSE, 3);
		gtk_box_pack_start (GTK_BOX (h1vbox2), fsampBox_frame, FALSE, FALSE, 3);
		gtk_container_add (GTK_CONTAINER (fsampBox_frame), fsampBox);
	#elif motif
		GuiLabel_createShown (form, -160, -10, 20, Gui_AUTOMATIC, L"Sampling frequency:", 0);
		fsampBox = XmCreateRadioBox (form, "fsamp", NULL, 0);
		XtVaSetValues (fsampBox,
			XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 10,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 45,
			XmNwidth, 150,
			NULL);
	#endif
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
		if (fsamp_ [i]. canDo) {
			double fsamp = fsamp_ [i]. fsamp;
			wchar title [40];
			swprintf (title, 40, L"%ls Hz", fsamp == floor (fsamp) ? Melder_integer ((long) fsamp) : Melder_fixed (fsamp, 5));
			fsamp_ [i]. button = GuiRadioButton_createShown (fsampBox,
				Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC,
				title, gui_radiobutton_cb_fsamp, this, 0);
			#if gtk
			if (fsamp_radio_list)
				GuiRadioButton_setGroup (fsamp_ [i]. button, fsamp_radio_list);
			fsamp_radio_list = (GSList *) GuiRadioButton_getGroup (fsamp_ [i]. button);
			#endif
		}
	}
	GuiObject_show (fsampBox);
	
	#if gtk
		GuiObject h2vbox = gtk_vbox_new (FALSE, 3);
		gtk_box_pack_start (GTK_BOX (hbox2), h2vbox, TRUE, TRUE, 3);
		
		progressScale = gtk_hscrollbar_new (NULL);
		gtk_range_set_range (GTK_RANGE (progressScale), 0, 1000);
		
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (progressScale));
		adj -> page_size = 150;
		gtk_adjustment_changed (adj);
		gtk_box_pack_start (GTK_BOX (h2vbox), progressScale, TRUE, TRUE, 3);
	#elif motif
		progressScale = XmCreateScale (form, "scale", NULL, 0);
		XtVaSetValues (progressScale, XmNorientation, XmHORIZONTAL,
			XmNminimum, 0, XmNmaximum, 1000,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 90,
			XmNwidth, 250,
			#ifdef macintosh
				XmNscaleWidth, 340,
			#endif
			NULL);
	#endif
	GuiObject_show (progressScale);

	#if gtk
		recstopplayBox = gtk_hbutton_box_new ();
		gtk_button_box_set_layout (GTK_BUTTON_BOX (recstopplayBox), GTK_BUTTONBOX_START);
		gtk_box_set_spacing (GTK_BOX (recstopplayBox), 3);
		gtk_container_add (GTK_CONTAINER (h2vbox), recstopplayBox);
	#else
		recstopplayBox = form;
	#endif
	y = 60;
	recordButton = GuiButton_createShown (recstopplayBox, 20, 90, Gui_AUTOMATIC, -y,
		L"Record", gui_button_cb_record, this, 0);
	stopButton = GuiButton_createShown (recstopplayBox, 100, 170, Gui_AUTOMATIC, -y,
		L"Stop", gui_button_cb_stop, this, 0);
	if (inputUsesPortAudio) {
		playButton = GuiButton_createShown (recstopplayBox, 180, 250, Gui_AUTOMATIC, -y,
			L"Play", gui_button_cb_play, this, 0);
	} else {
		#if defined (_WIN32) || defined (macintosh)
			playButton = GuiButton_createShown (recstopplayBox, 180, 250, Gui_AUTOMATIC, -y,
				L"Play", gui_button_cb_play, this, 0);
		#endif
	}
	
	#if gtk
		nameBox = gtk_hbox_new(FALSE, 3);
		gtk_container_add(GTK_CONTAINER(hbox2), nameBox);
	#else
		nameBox = form;
	#endif
	
	GuiLabel_createShown (nameBox, -200, -130, Gui_AUTOMATIC, -y - 2, L"Name:", GuiLabel_RIGHT);
	soundName = GuiText_createShown (nameBox, -120, -20, Gui_AUTOMATIC, -y, 0);
	#if motif	
	XtAddCallback (soundName, XmNactivateCallback, gui_cb_apply, (XtPointer) this);
	#endif
	GuiText_setString (soundName, L"untitled");

	#if gtk
		dlgCtrlBox = gtk_hbutton_box_new ();		// contains buttons
		gtk_button_box_set_layout (GTK_BUTTON_BOX (dlgCtrlBox), GTK_BUTTONBOX_END);
		gtk_box_set_spacing (GTK_BOX (dlgCtrlBox), 3);
		gtk_box_pack_end (GTK_BOX (form), dlgCtrlBox, FALSE, FALSE, 3);
	#else
		dlgCtrlBox = form;
	#endif
	
	y = 20;
	cancelButton = GuiButton_createShown (dlgCtrlBox, -350, -280, Gui_AUTOMATIC, -y,
		L"Close", gui_button_cb_cancel, this, 0);
	applyButton = GuiButton_createShown (dlgCtrlBox, -270, -170, Gui_AUTOMATIC, -y,
		L"Save to list", gui_button_cb_apply, this, 0);
	okButton = GuiButton_createShown (dlgCtrlBox, -160, -20, Gui_AUTOMATIC, -y,
		L"Save to list & Close", gui_button_cb_ok, this, 0);

	#if gtk
		gtk_widget_show_all (form);
	#else
		GuiObject_show (form);
	#endif
}

static void writeFakeMonoFile (SoundRecorder me, MelderFile file, int audioFileType) {
	long nsamp = my nsamp / 2;
	autoMelderFile mfile = MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
	MelderFile_writeAudioFileHeader16 (file, audioFileType, theControlPanel. sampleRate, nsamp, 1); therror
	if (Melder_defaultAudioFileEncoding16 (audioFileType) == Melder_LINEAR_16_BIG_ENDIAN) {
		for (long i = 0; i < nsamp; i ++)
			binputi2 ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
	} else {
		for (long i = 0; i < nsamp; i ++)
			binputi2LE ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
	}
	mfile.close ();
}

static void writeAudioFile (SoundRecorder me, MelderFile file, int audioFileType) {
	try {
		if (my fakeMono) {
			writeFakeMonoFile (me, file, audioFileType);
		} else {
			MelderFile_writeAudioFile16 (file, audioFileType, my buffer, theControlPanel. sampleRate, my nsamp, my numberOfChannels); therror
		}
	} catch (MelderError) {
		Melder_throw ("Audio file not written.");
	}
}

static int menu_cb_writeWav (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as WAV file", 0)
		wchar *name = GuiText_getString (my soundName);
		swprintf (defaultName, 300, L"%ls.wav", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_WAV);
	EDITOR_END
}

static int menu_cb_writeAifc (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as AIFC file", 0)
		wchar *name = GuiText_getString (my soundName);
		swprintf (defaultName, 300, L"%ls.aifc", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_AIFC);
	EDITOR_END
}

static int menu_cb_writeNextSun (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as NeXT/Sun file", 0)
		wchar *name = GuiText_getString (my soundName);
		swprintf (defaultName, 300, L"%ls.au", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NEXT_SUN);
	EDITOR_END
}

static int menu_cb_writeNist (EDITOR_ARGS) {
	EDITOR_IAM (SoundRecorder);
	EDITOR_FORM_WRITE (L"Save as NIST file", 0)
		wchar *name = GuiText_getString (my soundName);
		swprintf (defaultName, 300, L"%ls.nist", name);
		Melder_free (name);
	EDITOR_DO_WRITE
		writeAudioFile (me, file, Melder_NIST);
	EDITOR_END
}

static int menu_cb_SoundRecorder_help (EDITOR_ARGS) { EDITOR_IAM (SoundRecorder); Melder_help (L"SoundRecorder"); return 1; }

void structSoundRecorder :: v_createMenus () {
	SoundRecorder_Parent :: v_createMenus ();
	Editor_addCommand (this, L"File", L"Save as WAV file...", 0, menu_cb_writeWav);
	Editor_addCommand (this, L"File", L"Save as AIFC file...", 0, menu_cb_writeAifc);
	Editor_addCommand (this, L"File", L"Save as NeXT/Sun file...", 0, menu_cb_writeNextSun);
	Editor_addCommand (this, L"File", L"Save as NIST file...", 0, menu_cb_writeNist);
	Editor_addCommand (this, L"File", L"-- write --", 0, 0);
}

void structSoundRecorder :: v_createHelpMenuItems (EditorMenu menu) {
	SoundRecorder_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"SoundRecorder help", '?', menu_cb_SoundRecorder_help);
}

SoundRecorder SoundRecorder_create (GuiObject parent, int numberOfChannels) {
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
				long soundFeatures;
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
		if (preferences.bufferSize_MB < 1) preferences.bufferSize_MB = 1;   /* Validate preferences. */
		if (preferences.bufferSize_MB > 1000) preferences.bufferSize_MB = 1000;
		if (my buffer == NULL) {
			long nmax_bytes_pref = preferences.bufferSize_MB * 1000000;
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
				for (long idevice = 1; idevice <= 8; idevice ++) {
					Str255 hybridDeviceName;
					OSErr err = SPBGetIndexedDevice (idevice, & hybridDeviceName [0], NULL);
					if (err == siBadSoundInDevice) break;
					(void) PtoCstr (hybridDeviceName);
					if (SPBOpenDevice (hybridDeviceName, siWritePermission, & my refNum) == noErr) {
						Handle handle;
						if (SPBGetDeviceInfo (my refNum, siInputSourceNames, & handle) == noErr) {
							char *data = *handle, *plength;
							int numberOfDeviceSources = * (short *) data, deviceSource;
							/*HLock (handle);*/
							plength = & data [2];
							for (deviceSource = 1; deviceSource <= numberOfDeviceSources; deviceSource ++) {
								if (my numberOfInputDevices == SoundRecorder_IDEVICE_MAX) break;
								my device_ [++ my numberOfInputDevices]. canDo = true;
								strcpy ((char *) my hybridDeviceNames [my numberOfInputDevices], (const char *) hybridDeviceName);
								my macSource [my numberOfInputDevices] = deviceSource;
								plength [40] = '\0';
								wcsncpy (my device_ [my numberOfInputDevices]. name, Melder_peekUtf8ToWcs (plength + 1), *plength < 40 ? *plength : 40);
								my device_ [my numberOfInputDevices]. name [*plength < 40 ? *plength : 40] = '\0';
								plength += *plength + 1;
							}
							DisposeHandle (handle);
						}
						SPBCloseDevice (my refNum);
					}
				}
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
		initialize (me.peek()); therror

		Editor_init (me.peek(), parent, 100, 100, 600, 500, L"SoundRecorder", NULL);
		#if motif
		Melder_assert (XtWindow (my meter));
		#endif
		my graphics = Graphics_create_xmdrawingarea (my meter);
		Melder_assert (my graphics);
		Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);

struct structGuiDrawingAreaResizeEvent event = { my meter, 0 };
event. width = GuiObject_getWidth (my meter);
event. height = GuiObject_getHeight (my meter);
gui_drawingarea_cb_resize (me.peek(), & event);

		#if gtk
			g_idle_add (workProc, me.peek());
		#elif motif
			my workProcId = GuiAddWorkProc (workProc, (XtPointer) me.peek());
		#endif
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SoundRecorder not created.");
	}
}

/* End of file SoundRecorder.cpp */
