/* SoundRecorder.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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

/*
 * pb 2003/02/09 better layout on MacOS X
 * pb 2003/04/07 larger default buffer
 * pb 2003/08/22 use getenv ("AUDIODEV") on Sun (thanks to Michel Scheffers)
 * pb 2003/09/12 more MelderFile
 * pb 2003/10/22 check for mono refusal on Linux
 * pb 2003/12/06 use sys/soundcard.h instead of linux/soundcard.h for FreeBSD compatibility
 * pb 2004/11/26 fake Mono on MacOS X
 * pb 2004/11/26 check available sampling frequencies on MacOS X
 * pb 2005/02/13 defended against weird meter levels
 * pb 2005/04/25 made 24 kHz available for Mac
 * pb 2005/08/22 removed reference to Control menu from message
 * pb 2005/09/28 made 12 and 64 kHz available for Mac
 * pb 2005/10/13 edition for OpenBSD
 * pb 2006/04/01 corrections for Intel Mac
 * pb 2006/08/09 acknowledge the 67 MB buffer limit on Windows XP
 */

/* This source file describes interactive sound recorders for the following systems:
 *     SGI
 *     MacOS & Mach
 *     SunOS
 *     HP
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
#ifdef macintosh
	#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
#endif

#define CommonSoundRecorder_members Editor_members \
	int numberOfChannels, fakeMono; \
	XtWorkProcId workProcId; \
	long nsamp, nmax; \
	int synchronous, recording, lastLeftMaximum, lastRightMaximum, coupled; \
	int can8000, can9800, can11025, can12000, can16000, can22050, can22254, can24000, can32000, can44100, can48000, can64000; \
	short *buffer; \
	Widget progressScale, recordButton, stopButton, playButton, closeButton; \
	Widget publishLeftButton, publishRightButton, leftName, rightName; \
	Widget button8000, button9800, button11025, button12000, button16000, button22050, button22254, button24000, \
		button32000, button44100, button48000, button64000, leftMeter, rightMeter; \
	Widget microphoneButton, lineButton, digitalButton, in4Button, in5Button, in6Button, in7Button, in8Button; \
	Widget leftGainScale, rightGainScale; \
	Widget coupleButton; \
	Graphics leftGraphics, rightGraphics;

/* Class definition of SoundRecorder. */

#if defined (sgi)
	#include <audio.h>
	#define SoundRecorder_members CommonSoundRecorder_members \
		ALconfig audio; \
		ALport port; \
		long info [10];
#elif defined (_WIN32)
	#define SoundRecorder_members CommonSoundRecorder_members \
	HWAVEIN hWaveIn; \
	WAVEFORMATEX waveFormat; \
	WAVEHDR waveHeader [3]; \
	MMRESULT err; \
	short buffertje1 [1000*2], buffertje2 [1000*2];
#elif defined (macintosh)
	#include "macport_on.h"
	#include <Sound.h>
	#ifndef __MACH__
		#include <SoundInput.h>
	#endif
	#include <Gestalt.h>
	#include "macport_off.h"
	#define SoundRecorder_members CommonSoundRecorder_members \
		Handle temporaryMemoryHandle; \
		int numberOfMacSources; \
		short macSource [1+8]; \
		char macSourceTitle [1+8] [100]; \
		Str255 hybridDeviceNames [1+8]; \
		SPB spb; \
		long refNum;
#elif defined (sun)
	#include <fcntl.h>
	#include <stropts.h>
	#include <unistd.h>
	#if defined (sun4)
		#include <sun/audioio.h>
	#else
		#include <sys/audioio.h>
	#endif
	#define SoundRecorder_members CommonSoundRecorder_members \
		int fd; \
		struct audio_info info;
#elif defined (HPUX)
	#include <fcntl.h>
	#include <ctype.h>
	#include <unistd.h>
	#include <sys/audio.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#define SoundRecorder_members CommonSoundRecorder_members \
		int fd; \
		struct audio_describe info; \
		int hpInputSource; \
		struct audio_gain hpGains;
#elif defined (linux)
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
	#define SoundRecorder_members CommonSoundRecorder_members \
		int fd;
#else
	#define SoundRecorder_members CommonSoundRecorder_members \
		int fd;
#endif

#define SoundRecorder_methods Editor_methods
class_create_opaque (SoundRecorder, Editor)

static int theCouplePreference = 1;
static int nmaxMB_pref = 4;
static char macDefaultDevice [Resources_STRING_BUFFER_SIZE];
static char macDefaultSource [Resources_STRING_BUFFER_SIZE];

void SoundRecorder_prefs (void) {
	Resources_addInt ("SoundRecorder.bufferSize_MB", & nmaxMB_pref);
	Resources_addInt ("SoundRecorder.coupleSliders", & theCouplePreference);
	Resources_addString ("SoundRecorder.mac.defaultDevice", & macDefaultDevice [0]);
	Resources_addString ("SoundRecorder.mac.defaultSource", & macDefaultSource [0]);
}

int SoundRecorder_getBufferSizePref_MB (void) { return nmaxMB_pref; }
void SoundRecorder_setBufferSizePref_MB (int size) { nmaxMB_pref = size < 1 ? 1 : size > 1000 ? 1000: size; }

#define step 1000

/* For those systems that do not have a pollable audio control panel, */
/* the settings are saved only here, so that they are remembered across */
/* subsequent creations of a SoundRecorder. Also, this is then the way */
/* in which two simultaneously open SoundRecorders would communicate. */

static struct {
	int inputSource;   /* 1 = microphone, 2 = line, 3 = digital. */
	int leftGain, rightGain;   /* 0..255. */
	double sampleRate;
} theControlPanel =
#if defined (sgi) || defined (HPUX) || defined (linux)
	{ 1, 200, 200, 22050 };
#elif defined (macintosh)
	{ 1, 26, 26, 44100 };
#else
	{ 1, 26, 26, 22050 };
#endif

/********** ERROR HANDLING **********/

#ifdef _WIN32
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
static int win_waveInCheck (SoundRecorder me) {
	char messageText [MAXERRORLENGTH];
	MMRESULT err;
	if (my err == MMSYSERR_NOERROR) return 1;
	err = waveInGetErrorText (my err, messageText, MAXERRORLENGTH);
	if (err == MMSYSERR_NOERROR) Melder_error ("%s", messageText);
	else if (err == MMSYSERR_BADERRNUM) Melder_error ("Error number %d out of range.", my err);
	else if (err == MMSYSERR_NODRIVER) Melder_error ("No sound driver present.");
	else if (err == MMSYSERR_NOMEM) Melder_error ("Out of memory.");
	else Melder_error ("Unknown sound error.");
	return 0;
}
static int win_waveInOpen (SoundRecorder me) {
	my err = waveInOpen (& my hWaveIn, WAVE_MAPPER, & my waveFormat, 0, 0, CALLBACK_NULL);
	if (! win_waveInCheck (me)) return Melder_error ("Audio input not opened.");
	if (Melder_debug != 8) waveInReset (my hWaveIn);
	return 1;
}
static int win_waveInPrepareHeader (SoundRecorder me, int which) {
	my err = waveInPrepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error ("Audio input: cannot prepare header.\n"
		"Quit some other programs or go to \"Sound input prefs\" in the Preferences menu.");
	return 1;
}
static int win_waveInAddBuffer (SoundRecorder me, int which) {
	my err = waveInAddBuffer (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error ("Audio input: cannot add buffer.");
	return 1;
}
static int win_waveInStart (SoundRecorder me) {
	my err = waveInStart (my hWaveIn);   /* Asynchronous. */
	if (! win_waveInCheck (me)) return Melder_error ("Audio input not started.");
	return 1;
}
static int win_waveInStop (SoundRecorder me) {
	my err = waveInStop (my hWaveIn);
	if (! win_waveInCheck (me)) return Melder_error ("Audio input not stopped.");
	return 1;
}
static int win_waveInReset (SoundRecorder me) {
	my err = waveInReset (my hWaveIn);
	if (! win_waveInCheck (me)) return Melder_error ("Audio input not reset.");
	return 1;
}
static int win_waveInUnprepareHeader (SoundRecorder me, int which) {
	my err = waveInUnprepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error ("Audio input: cannot unprepare header.");
	return 1;
}
static int win_waveInClose (SoundRecorder me) {
	my err = waveInClose (my hWaveIn);
	my hWaveIn = 0;
	if (! win_waveInCheck (me)) return Melder_error ("Audio input not closed.");
	return 1;
}
#endif

#ifdef macintosh
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
	static long notified = FALSE;
	const char *string;
	if (notified) return;
	string = errString (err);
	if (string) Melder_flushError ("(%s:) %s", routine, string);
	else Melder_flushError ("(%s:) Error %ld", routine, err);
	notified = TRUE;
}
#endif

static void stopRecording (SoundRecorder me) {	
	if (! my recording) return;
	my recording = FALSE;
	if (! my synchronous) {
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
			win_waveInReset (me); cherror
			if (my nsamp == 0)
				my nsamp = my waveHeader [0]. dwBytesRecorded / (sizeof (short) * my numberOfChannels);
			if (my nsamp > my nmax)
				my nsamp = my nmax;
			win_waveInUnprepareHeader (me, 0); cherror
			win_waveInClose (me); cherror
		#elif defined (macintosh)
			OSErr err;
			short recordingStatus, meterLevel;
			unsigned long totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
			err = SPBGetRecordingStatus (my refNum, & recordingStatus, & meterLevel,
					& totalSamplesToRecord, & numberOfSamplesRecorded,
					& totalMsecsToRecord, & numberOfMsecsRecorded);
			if (err != noErr) { onceError ("SPBGetRecordingStatus", err); return; }
			/* Melder_assert (meterLevel >= 0); Melder_assert (meterLevel <= 255); */
			if (totalSamplesToRecord == 0)
				my nsamp = my nmax;
			else
				my nsamp = numberOfSamplesRecorded / (sizeof (short) * my numberOfChannels);   /* From Mac "samples" to Mac "frames" (our "samples"). */
			err = SPBStopRecording (my refNum);
			if (err != noErr) { onceError ("SPBStopRecording", err); return; }
			my spb. bufferPtr = NULL;
			err = SPBRecord (& my spb, true);
			if (err != noErr) onceError ("SPBRecord", err);
		#endif
	}
#ifdef _WIN32
end:
	iferror Melder_flushError ("Cannot stop recording.");
#endif
}

static void destroy (I) {
	iam (SoundRecorder);
	stopRecording (me);   /* Must occur before freeing my buffer. */
	Melder_stopPlaying (Melder_IMPLICIT);   /* Must also occur before freeing my buffer. */
	if (my workProcId) XtRemoveWorkProc (my workProcId);
	#if defined (macintosh)
		if (my temporaryMemoryHandle) {
			DisposeHandle (my temporaryMemoryHandle);
		} else {
			NUMsvector_free (my buffer, 0);
		}
	#else
		NUMsvector_free (my buffer, 0);
	#endif

	#if defined (sgi)
		if (my port) ALcloseport (my port);
		if (my audio) ALfreeconfig (my audio);
	#elif defined (_WIN32)
		if (my hWaveIn != 0) {
			waveInReset (my hWaveIn);
			waveInUnprepareHeader (my hWaveIn, & my waveHeader [0], sizeof (WAVEHDR));
			waveInClose (my hWaveIn);
		}
	#elif defined (macintosh)
		if (my refNum) SPBCloseDevice (my refNum);
	#elif defined (UNIX) || defined (HPUX)
		if (my fd != -1) close (my fd);
	#else
	#endif
	forget (my leftGraphics);
	forget (my rightGraphics);
	inherited (SoundRecorder) destroy (me);
}

static void showMaximum (Graphics graphics, double maximum) {
	Graphics_setGrey (graphics, 0.9);
	Graphics_fillRectangle (graphics, 0.0, 1.0, maximum, 32768.0);
	Graphics_setColour (graphics, Graphics_GREEN);
	if (maximum < 23000) {
		Graphics_fillRectangle (graphics, 0.0, 1.0, 0.0, maximum);
	} else {
		Graphics_fillRectangle (graphics, 0.0, 1.0, 0.0, 23000.0);
		Graphics_setColour (graphics, Graphics_YELLOW);
		if (maximum < 30000) {
			Graphics_fillRectangle (graphics, 0.0, 1.0, 23000.0, maximum);
		} else {
			Graphics_fillRectangle (graphics, 0.0, 1.0, 23000.0, 30000.0);
			Graphics_setColour (graphics, Graphics_RED);
			Graphics_fillRectangle (graphics, 0.0, 1.0, 30000.0, maximum);
		}
	}
}

static void showMaxima (SoundRecorder me, int leftMaximum, int rightMaximum) {
	if (my lastLeftMaximum > 30000) {
		int leak = my lastLeftMaximum - 2000000 /
			(theControlPanel. inputSource == 3 ? 48000 : theControlPanel. sampleRate);
		if (leftMaximum < leak) leftMaximum = leak;
	}
	showMaximum (my leftGraphics, leftMaximum);
	my lastLeftMaximum = leftMaximum;
	if (my numberOfChannels == 2) {
		if (my lastRightMaximum > 30000) {
			int leak = my lastRightMaximum - 2000000 /
				(theControlPanel. inputSource == 3 ? 48000 : theControlPanel. sampleRate);
			if (rightMaximum < leak) rightMaximum = leak;
		}
		showMaximum (my rightGraphics, rightMaximum);
		my lastRightMaximum = rightMaximum;
	}
}

static int tooManySamplesInBufferToReturnToGui (SoundRecorder me) {
	#if defined (sgi)
		return ALgetfilled (my port) > step;
	#else
		(void) me;
		return FALSE;
	#endif
}

static Boolean workProc (XtPointer void_me) {
	iam (SoundRecorder);
	short buffertje [step*2];
	int leftMaximum = 0, rightMaximum = 0, i, stepje = 0;

	#if defined (linux)
		#define min(a,b) a > b ? b : a
	#endif

	/* Determine global audio parameters (may have been changed by an external control panel):
	 *   1. input source;
	 *   2. left and right gain;
	 *   3. sampling frequency.
	 */
	#if defined (sgi)
		my info [0] = AL_INPUT_RATE;
		my info [2] = AL_INPUT_SOURCE;
		my info [4] = AL_LEFT_INPUT_ATTEN;
		my info [6] = AL_RIGHT_INPUT_ATTEN;
		my info [8] = AL_DIGITAL_INPUT_RATE;
		ALgetparams (AL_DEFAULT_DEVICE, my info, 10);
		theControlPanel. inputSource = my info [3] == AL_INPUT_MIC ? 1 : my info [3] == AL_INPUT_LINE ? 2 : 3;
		theControlPanel. leftGain = 255 - my info [5];
		theControlPanel. rightGain = 255 - my info [7];
		theControlPanel. sampleRate = my info [3] == AL_INPUT_DIGITAL ? my info [9] : my info [1];
	#elif defined (macintosh)
	{
		OSErr err;
		short macSource, isource;
		Str255 pdeviceName;
		err = SPBGetDeviceInfo (my refNum, siDeviceName, & pdeviceName);
		if (err != noErr) { onceError ("SPBGetDeviceInfo (deviceName)", err); return False; }
		err = SPBGetDeviceInfo (my refNum, siInputSource, & macSource);
		if (err != noErr) { onceError ("SPBGetDeviceInfo (inputSource)", err); return False; }
		for (isource = 1; isource <= my numberOfMacSources; isource ++) {
			if (strequ ((const char *) & pdeviceName, (const char *) my hybridDeviceNames [isource]) &&
					macSource == my macSource [isource]) {
				theControlPanel. inputSource = isource;
				break;
			}
		}
	}
	#elif defined (sun)
		ioctl (my fd, AUDIO_GETINFO, & my info);
		theControlPanel. inputSource =
			my info. record. port == AUDIO_MICROPHONE ? 1 : 2;
		theControlPanel. leftGain = my info. record. balance <= 32 ? my info. record. gain:
			my info. record. gain * (64 - my info. record. balance) / 32;
		theControlPanel. rightGain = my info. record. balance >= 32 ? my info. record. gain:
			my info. record. gain * my info. record. balance / 32;
		theControlPanel. sampleRate = my info. record. sample_rate;
		if (my info. record. channels != my numberOfChannels)
			Melder_casual ("(SoundRecorder:) Not %s.", my numberOfChannels == 1 ? "mono" : "stereo");
		if (my info. record. precision != 16)
			Melder_casual ("(SoundRecorder:) Not 16-bit.");
		if (my info. record. encoding != AUDIO_ENCODING_LINEAR)
			Melder_casual ("(SoundRecorder:) Not linear.");
	#elif defined (HPUX)
	{
		int leftGain = my hpGains. cgain [0]. receive_gain * 11;
		int rightGain = my hpGains. cgain [1]. receive_gain * 11;
		int sampleRate;
		ioctl (my fd, AUDIO_GET_INPUT, & my hpInputSource);
		theControlPanel. inputSource = (my hpInputSource & AUDIO_IN_MIKE) ? 1 : 2;
		ioctl (my fd, AUDIO_GET_GAINS, & my hpGains);
		if (leftGain < theControlPanel. leftGain - 10 || leftGain > theControlPanel. leftGain)
			theControlPanel. leftGain = leftGain;
		if (rightGain < theControlPanel. rightGain - 10 || rightGain > theControlPanel. rightGain)
			theControlPanel. rightGain = rightGain;
		ioctl (my fd, AUDIO_GET_SAMPLE_RATE, & sampleRate);
		theControlPanel. sampleRate = sampleRate;
	}
	#endif

	/* Set the buttons according to the audio parameters. */

	if (my recordButton) XtSetSensitive (my recordButton, my recording == FALSE);
	if (my stopButton) XtSetSensitive (my stopButton, my recording == TRUE);
	if (my playButton) XtSetSensitive (my playButton, my recording == FALSE && my nsamp > 0);
	if (my publishLeftButton) XtSetSensitive (my publishLeftButton, my recording == FALSE && my nsamp > 0);
	if (my publishRightButton) XtSetSensitive (my publishRightButton, my recording == FALSE && my nsamp > 0);
	if (my button8000) XmToggleButtonSetState (my button8000, theControlPanel. sampleRate == 8000, False);
	if (my button9800) XmToggleButtonSetState (my button9800, theControlPanel. sampleRate == 9800, False);
	if (my button11025) XmToggleButtonSetState (my button11025, theControlPanel. sampleRate == 11025, False);
	if (my button12000) XmToggleButtonSetState (my button12000, theControlPanel. sampleRate == 12000, False);
	if (my button16000) XmToggleButtonSetState (my button16000, theControlPanel. sampleRate == 16000, False);
	if (my button22050) XmToggleButtonSetState (my button22050, theControlPanel. sampleRate == 22050, False);
	if (my button22254) XmToggleButtonSetState (my button22254, theControlPanel. sampleRate == 22254.54545, False);
	if (my button24000) XmToggleButtonSetState (my button24000, theControlPanel. sampleRate == 24000, False);
	if (my button32000) XmToggleButtonSetState (my button32000, theControlPanel. sampleRate == 32000, False);
	if (my button44100) XmToggleButtonSetState (my button44100, theControlPanel. sampleRate == 44100, False);
	if (my button48000) XmToggleButtonSetState (my button48000, theControlPanel. sampleRate == 48000, False);
	if (my button64000) XmToggleButtonSetState (my button64000, theControlPanel. sampleRate == 64000, False);
	if (my button8000) XtSetSensitive (my button8000, ! my recording);
	if (my button9800) XtSetSensitive (my button9800, ! my recording);
	if (my button11025) XtSetSensitive (my button11025, ! my recording);
	if (my button12000) XtSetSensitive (my button12000, ! my recording);
	if (my button16000) XtSetSensitive (my button16000, ! my recording);
	if (my button22050) XtSetSensitive (my button22050, ! my recording);
	if (my button22254) XtSetSensitive (my button22254, ! my recording);
	if (my button24000) XtSetSensitive (my button24000, ! my recording);
	if (my button32000) XtSetSensitive (my button32000, ! my recording);
	if (my button44100) XtSetSensitive (my button44100, ! my recording);
	if (my button48000) XtSetSensitive (my button48000, ! my recording);
	if (my button64000) XtSetSensitive (my button64000, ! my recording);
	if (my microphoneButton) XmToggleButtonSetState (my microphoneButton, theControlPanel. inputSource == 1, False);
	if (my lineButton) XmToggleButtonSetState (my lineButton, theControlPanel. inputSource == 2, False);
	if (my digitalButton) XmToggleButtonSetState (my digitalButton, theControlPanel. inputSource == 3, False);
	if (my in4Button) XmToggleButtonSetState (my in4Button, theControlPanel. inputSource == 4, False);
	if (my in5Button) XmToggleButtonSetState (my in5Button, theControlPanel. inputSource == 5, False);
	if (my in6Button) XmToggleButtonSetState (my in6Button, theControlPanel. inputSource == 6, False);
	if (my in7Button) XmToggleButtonSetState (my in7Button, theControlPanel. inputSource == 7, False);
	if (my in8Button) XmToggleButtonSetState (my in8Button, theControlPanel. inputSource == 8, False);
	if (my leftGainScale) XmScaleSetValue (my leftGainScale, theControlPanel. leftGain);
	if (my rightGainScale) XmScaleSetValue (my rightGainScale, theControlPanel. rightGain);

	/*Graphics_setGrey (my leftGraphics, 0.9);
	Graphics_fillRectangle (my leftGraphics, 0.0, 1.0, 0.0, 32768.0);
	Graphics_setGrey (my rightGraphics, 0.9);
	Graphics_fillRectangle (my rightGraphics, 0.0, 1.0, 0.0, 32768.0);*/

	if (my synchronous) {
		/*
		 * Read some samples into 'buffertje'.
		 */
		do {
			#if defined (sgi)
				ALreadsamps (my port, buffertje, step * my numberOfChannels);
				stepje = step;
			#elif defined (macintosh)
				my spb. bufferPtr = (char *) buffertje;
				my spb. bufferLength = my spb. count = step * (sizeof (short) * my numberOfChannels);
				SPBRecord (& my spb, false);   /* Synchronous. */
				stepje = step;
			#elif defined (_WIN32)
			#else
				stepje = read (my fd, (void *) buffertje, step * (sizeof (short) * my numberOfChannels)) / (sizeof (short) * my numberOfChannels);
			#endif

			if (my numberOfChannels == 1) {
				for (i = 0; i < stepje; i ++) {
					short left = buffertje [i];
					if (abs (left) > leftMaximum) leftMaximum = abs (left);
				}
			} else {
				for (i = 0; i < stepje; i ++) {
					short left = buffertje [i+i], right = buffertje [i+i+1];
					if (abs (left) > leftMaximum) leftMaximum = abs (left);
					if (abs (right) > rightMaximum) rightMaximum = abs (right);
				}
			}
			if (my recording)
				memcpy (my buffer + my nsamp * my numberOfChannels, buffertje, stepje * (sizeof (short) * my numberOfChannels));
			showMaxima (me, leftMaximum, rightMaximum);
			if (my recording) {
				my nsamp += stepje;
				if (my nsamp > my nmax - step) my recording = FALSE;
				XmScaleSetValue (my progressScale,
					(int) (1000.0f * ((float) my nsamp / (float) my nmax)));
			}
		} while (my recording && tooManySamplesInBufferToReturnToGui (me));
	} else {
		#if defined (_WIN32)
			if (my recording) {
				MMTIME mmtime;
				long firstSample = 0, lastSample = 0, i;
				mmtime. wType = TIME_BYTES;
				if (waveInGetPosition (my hWaveIn, & mmtime, sizeof (MMTIME)) == MMSYSERR_NOERROR)
					lastSample = mmtime. u.cb / (sizeof (short) * my numberOfChannels);
				firstSample = lastSample - 1000;
				if (firstSample < 0) firstSample = 0;
				if (my numberOfChannels == 1) {
					for (i = firstSample; i < lastSample; i ++) {
						long left = my buffer [i];
						if (abs (left) > leftMaximum) leftMaximum = abs (left);
					}
				} else {
					for (i = firstSample; i < lastSample; i ++) {
						long left = my buffer [i+i], right = my buffer [i+i+1];
						if (abs (left) > leftMaximum) leftMaximum = abs (left);
						if (abs (right) > rightMaximum) rightMaximum = abs (right);
					}
				}
				XmScaleSetValue (my progressScale, (int) (1000.0f * ((float) lastSample / (float) my nmax)));
				showMaxima (me, leftMaximum, rightMaximum);
			}
		#elif defined (macintosh)
			OSErr err;
			short recordingStatus, meterLevel;
			unsigned long totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
			err = SPBGetRecordingStatus (my refNum, & recordingStatus, & meterLevel,
					& totalSamplesToRecord, & numberOfSamplesRecorded,
					& totalMsecsToRecord, & numberOfMsecsRecorded);
			if (err != noErr) { onceError ("SPBGetRecordingStatus", err); return FALSE; }
			Melder_casual ("Meter level %ld", meterLevel);
			if (meterLevel < 0) meterLevel = 0;   /* Should not occur. */
			if (meterLevel > 255) meterLevel = 255;   /* Should not occur. */
			leftMaximum = rightMaximum = 128 * meterLevel;
			if (my recording) {
				if (totalSamplesToRecord == 0)
					my nsamp = my nmax;
				else
					my nsamp = numberOfSamplesRecorded / (sizeof (short) * my numberOfChannels);
				XmScaleSetValue (my progressScale, (int) (1000.0f * ((float) my nsamp / (float) my nmax)));
			}
			showMaxima (me, leftMaximum, rightMaximum);
		#else
			showMaxima (me, leftMaximum, rightMaximum);
		#endif
	}
	iferror Melder_flushError (NULL);
	return False;
}

static void cb_record (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	my nsamp = 0;
	my recording = TRUE;
	if (! my synchronous) {
		#if defined (_WIN32)
			win_fillFormat (me);
			win_fillHeader (me, 0);
			win_waveInOpen (me); cherror
			win_waveInPrepareHeader (me, 0); cherror
			win_waveInAddBuffer (me, 0); cherror
			win_waveInStart (me); cherror
		#elif defined (macintosh)
			OSErr err;
			/*FILE *f = fopen ("Harde schijf:Desktop Folder:praatlongsoundkanweg.rawww", "wb");
			if (! f) return;*/
			err = SPBStopRecording (my refNum);
			if (err != noErr) { onceError ("SPBStopRecording", err); return; }
			for (;;) {
				my spb. bufferPtr = (char *) my buffer;
				my spb. bufferLength = my spb. count = my nmax * (sizeof (short) * my numberOfChannels);
				err = SPBRecord (& my spb, true);   /* Asynchronous. */
				/* err = SPBRecordToFile (f -> handle, & my spb, true);*/
				if (err == noErr) break;   /* Success. */
				if (err == notEnoughMemoryErr) {
					if (my temporaryMemoryHandle) {
						if (my nmax < 50000) { Melder_flushError ("Out of memory. Quit other programs."); return; }
						my nmax /= 2;   /* Retry with less temporary memory. */
						SetHandleSize (my temporaryMemoryHandle, my nmax * (sizeof (short) * my numberOfChannels));
						HLock ((Handle) my temporaryMemoryHandle);
					} else { Melder_flushError ("Out of memory. Quit other programs."); return; }
				} else if (err != noErr) { onceError ("SPBRecord", err); return; }
			}
			/*fclose (f);*/
		#endif
	}
#ifdef _WIN32
end:
	iferror Melder_flushError ("Cannot record.");
#endif
}

static void cb_stop (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	stopRecording (me);
}

#if defined (sgi) || defined (macintosh) || defined (_WIN32)
static void cb_play (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	if (my recording || my nsamp == 0) return;
	if (! Melder_play16 (my buffer, theControlPanel. sampleRate, my fakeMono ? my nsamp / 2 : my nsamp, my fakeMono ? 2 : my numberOfChannels, NULL, NULL))
		Melder_flushError (NULL);
}
#endif

static void cb_publishLeft (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	Sound sound = NULL;
	long i, nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	double fsamp = theControlPanel. sampleRate;
	(void) w;
	(void) call;
	if (my nsamp == 0) return;
	if (fsamp <= 0) fsamp = 48000.0;   /* Safe. */
	sound = Sound_createSimple ((double) nsamp / fsamp, fsamp);
	if (Melder_hasError ()) { Melder_flushError ("You can still save to file."); return; }
	if (my fakeMono) {
		for (i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = (my buffer [i + i - 2] + my buffer [i + i - 1]) * (1.0 / 65536);
	} else if (my numberOfChannels == 1) {
		for (i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = my buffer [i - 1] * (1.0 / 32768);
	} else {
		for (i = 1; i <= nsamp; i ++)
			sound -> z [1] [i] = my buffer [i + i - 2] * (1.0 / 32768);
	}
	if (my leftName) {
		char *name = XmTextFieldGetString (my leftName);
		Thing_setName (sound, name);
		XtFree (name);
	}
	if (my publishCallback)
		my publishCallback (me, my publishClosure, sound);
}

static void cb_publishRight (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	Sound sound = NULL;
	long i;
	double fsamp = theControlPanel. sampleRate;
	(void) w;
	(void) call;
	if (my nsamp == 0) return;
	if (fsamp <= 0) fsamp = 48000.0;   /* Safe. */
	sound = Sound_createSimple ((double) my nsamp / fsamp, fsamp);
	if (Melder_hasError ()) { Melder_flushError ("You can still save to file."); return; }
	for (i = 1; i <= my nsamp; i ++)
		sound -> z [1] [i] = my buffer [i + i - 1] * (1.0 / 32768);
	if (my rightName) {
		char *name = XmTextFieldGetString (my rightName);
		Thing_setName (sound, name);
		XtFree (name);
	}
	if (my publishCallback)
		my publishCallback (me, my publishClosure, sound);
}

static void cb_close (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	stopRecording (me);
	forget (me);
}

static void setSystemInputGains (SoundRecorder me) {
	#if defined (sgi)
		my info [0] = AL_LEFT_INPUT_ATTEN;
		my info [1] = 255 - theControlPanel. leftGain;
		my info [2] = AL_RIGHT_INPUT_ATTEN;
		my info [3] = 255 - theControlPanel. rightGain;
		ALsetparams (AL_DEFAULT_DEVICE, my info, 4);
	#elif defined (sun)
		AUDIO_INITINFO (& my info);
		my info. record. gain = theControlPanel. leftGain > theControlPanel. rightGain ?
			theControlPanel. leftGain : theControlPanel. rightGain;
		my info. record. balance = theControlPanel. leftGain == theControlPanel. rightGain ? 32 :
			theControlPanel. leftGain > theControlPanel. rightGain ?
			32 * theControlPanel. rightGain / theControlPanel. leftGain :
			64 - 32 * theControlPanel. leftGain / theControlPanel. rightGain;
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#elif defined (HPUX)
		my hpGains. cgain [0]. receive_gain = theControlPanel. leftGain / 11;
		my hpGains. cgain [1]. receive_gain = theControlPanel. rightGain / 11;
		my hpGains. channel_mask = AUDIO_CHANNEL_LEFT | AUDIO_CHANNEL_RIGHT;
		ioctl (my fd, AUDIO_SET_GAINS, & my hpGains);
	#elif defined (linux)
	{
		int val, fd_mixer = open ("/dev/mixer", O_WRONLY);	
		(void) me;	
		if (fd_mixer == -1) {
			Melder_flushError ("(Sound_record:) Cannot open /dev/mixer.");
		}

		val = (int)(100*((float)theControlPanel.leftGain/255)) | ((int)((100*((float)theControlPanel.rightGain/255))) << 8);			      
		if (theControlPanel.inputSource == 1) {			
			if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_MIC), & val) == -1)
				Melder_flushError ("(Sound_record:) Cannot set gain and balance.");
		} else {
			if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_LINE), & val) == -1)
				Melder_flushError ("(Sound_record:) Cannot set gain and balance.");
		}
		close (fd_mixer);
	}
	#else
		(void) me;
	#endif
}

static void cb_leftGain (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	Melder_assert (w == my leftGainScale);
	(void) call;
	XmScaleGetValue (my leftGainScale, & theControlPanel. leftGain);
	if (my coupled) {
		theControlPanel. rightGain = theControlPanel. leftGain;
		XmScaleSetValue (my rightGainScale, theControlPanel. rightGain);
	}
	setSystemInputGains (me);
}

static void cb_rightGain (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	Melder_assert (w == my rightGainScale);
	(void) call;
	XmScaleGetValue (my rightGainScale, & theControlPanel. rightGain);
	if (my coupled) {
		theControlPanel. leftGain = theControlPanel. rightGain;
		XmScaleSetValue (my leftGainScale, theControlPanel. leftGain);
	}
	setSystemInputGains (me);
}

static void cb_couple (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	my coupled = 1 - my coupled;
	if (my coupled) {
		theControlPanel. leftGain = theControlPanel. rightGain =
			(theControlPanel. leftGain + theControlPanel. rightGain) / 2;
		XmScaleSetValue (my leftGainScale, theControlPanel. leftGain);
		XmScaleSetValue (my rightGainScale, theControlPanel. rightGain);
		setSystemInputGains (me);
	}
	theCouplePreference = my coupled;
}

#ifdef sgi
static int open_sgi (SoundRecorder me) {
	my audio = ALnewconfig ();
	if (! my audio)
		return Melder_error ("Unexpected error: cannot create audio config...");
	ALsetchannels (my audio, my numberOfChannels == 1 ? AL_MONO : AL_STEREO);
	ALsetwidth (my audio, AL_SAMPLE_16);
	if (! (my port = ALopenport ("SoundRecorder", "r", my audio)))
		return Melder_error ("Cannot open audio port (too many open already).");
	my can9800 = TRUE;
	return 1;
}
#endif

#ifdef _WIN32
static int open_win (SoundRecorder me) {
	(void) me;
	return 1;
}
#endif

#ifdef macintosh
static int open_mac (SoundRecorder me) {
	unsigned long sampleRate_uf = theControlPanel. sampleRate * 65536L;
	short numberOfChannels = my numberOfChannels, continuous = TRUE, sampleSize = 16, levelMeterOnOff = 1, async;
	short inputSource = theControlPanel. inputSource, irate;
	OSType compressionType = 'NONE';
	struct { Handle dummy1; short dummy2, number; Handle handle; } sampleRateInfo;   /* Make sure that number is adjacent to handle. */
	if (SPBOpenDevice (my hybridDeviceNames [inputSource], siWritePermission, & my refNum) != noErr)
		Melder_flushError ("(Sound_record:) Cannot open audio input device.");
	/*
	 	From Apple:
	 	"Get the range of sample rates this device can produce.
	 	The infoData  parameter points to an integer, which is the number of sample rates the device supports, followed by a handle.
	 	The handle references a list of sample rates, each of type Fixed .
	 	If the device can record a range of sample rates, the number of sample rates is set to 0 and the handle contains two rates,
	 	the minimum and the maximum of the range of sample rates.
	 	Otherwise, a list is returned that contains the sample rates supported.
	 	In order to accommodate sample rates greater than 32ÊkHz, the most significant bit is not treated as a sign bit;
	 	instead, that bit is interpreted as having the value 32,768."
	 */
	SPBGetDeviceInfo (my refNum, siSampleRateAvailable, & sampleRateInfo. number);
	if (sampleRateInfo. number == 0) {
	} else {
		my can8000 = FALSE;
		my can11025 = FALSE;
		my can12000 = FALSE;
		my can16000 = FALSE;
		my can22050 = FALSE;
		my can24000 = FALSE;
		my can32000 = FALSE;
		my can44100 = FALSE;
		my can48000 = FALSE;
		my can64000 = FALSE;
		for (irate = 1; irate <= sampleRateInfo. number; irate ++) {
			Fixed rate_fixed = (* (Fixed **) sampleRateInfo. handle) [irate - 1];
			unsigned short rate_ushort = * (unsigned short *) & rate_fixed;
			switch (rate_ushort) {
				case 0: my can44100 = TRUE, my can48000 = TRUE; break;   /* BUG */
				case 8000: my can8000 = TRUE; break;
				case 11025: my can11025 = TRUE; break;
				case 12000: my can12000 = TRUE; break;
				case 16000: my can16000 = TRUE; break;
				case 22050: my can22050 = TRUE; break;
				case 22254: my can22254 = TRUE; break;
				case 24000: my can24000 = TRUE; break;
				case 32000: my can32000 = TRUE; break;
				case 44100: my can44100 = TRUE; break;
				case 48000: my can48000 = TRUE; break;
				case 64000: my can64000 = TRUE; break;
				default: Melder_warning ("Your computer seems to support a sampling frequency of %d Hz. "
					"Contact the author (paul.boersma@uva.nl) to make this frequency available to you.", rate_ushort);
			}
		}
	}
	if (SPBSetDeviceInfo (my refNum, siInputSource, & my macSource [inputSource]) != noErr)
		Melder_flushError ("(Sound_record:) Cannot change input source.");
	/*if (SPBOpenDevice (NULL, siWritePermission, & my refNum) != noErr)
		return Melder_error ("Cannot open audio input device.");*/
	if (SPBSetDeviceInfo (my refNum, siSampleRate, & sampleRate_uf) != noErr) {
		Melder_flushError ("Cannot set sampling frequency to %.5f Hz.", theControlPanel. sampleRate);
		theControlPanel. sampleRate = 44100;
	}
	if (SPBSetDeviceInfo (my refNum, siNumberChannels, & numberOfChannels) != noErr) {
		if (my numberOfChannels == 1) {
			my fakeMono = TRUE;
		} else {
			return Melder_error ("(Sound_record:) Cannot set to stereo.");
		}
	}
	if (SPBSetDeviceInfo (my refNum, siCompressionType, & compressionType) != noErr)
		return Melder_error ("(Sound_record:) Cannot set to linear.");
	if (SPBSetDeviceInfo (my refNum, siSampleSize, & sampleSize) != noErr)
		return Melder_error ("(Sound_record:) Cannot set to %d-bit.", sampleSize);
	if (SPBSetDeviceInfo (my refNum, siLevelMeterOnOff, & levelMeterOnOff) != noErr)
		return Melder_error ("(Sound_record:) Cannot set level meter to ON.");
	if (! my synchronous && (SPBGetDeviceInfo (my refNum, siAsync, & async) != noErr || ! async)) {
		static int warned = FALSE;
		my synchronous = TRUE;
		if (! warned) { Melder_warning ("Recording must and will be synchronous on this machine."); warned = TRUE; }
	}
	if (my synchronous && SPBSetDeviceInfo (my refNum, siContinuous, & continuous) != noErr)
		return Melder_error ("(Sound_record:) Cannot set continuous recording.");
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
		if (err != noErr) { onceError ("SPBRecord", err); return 1; }
	}
	return 1;
}
#endif

#ifdef sun
static int open_sun (SoundRecorder me) {
	my fd = open (getenv ("AUDIODEV") ? getenv ("AUDIODEV") : "/dev/audio", O_RDONLY);
	if (my fd == -1) {
		if (errno == EBUSY)
			return Melder_error ("(SoundRecorder:) Audio device already in use.");
		else
			return Melder_error ("(SoundRecorder:) Cannot open audio device.");
	}
	AUDIO_INITINFO (& my info);
	my info. record. pause = 1;
	ioctl (my fd, AUDIO_SETINFO, & my info);   /* Pause! */
	ioctl (my fd, I_FLUSH, FLUSHR);   /* Discard buffers! */

	#ifndef sun4
		AUDIO_INITINFO (& my info);
		my info. record. buffer_size = 8176;   /* Maximum. */
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#endif

	AUDIO_INITINFO (& my info);
	my info. monitor_gain = 0;   /* Not rondzingen. */
	ioctl (my fd, AUDIO_SETINFO, & my info);

	/* Take over the saved settings. */

	AUDIO_INITINFO (& my info);
	my info. record. port = theControlPanel. inputSource == 2 ? AUDIO_LINE_IN :
		AUDIO_MICROPHONE;
	ioctl (my fd, AUDIO_SETINFO, & my info);
	AUDIO_INITINFO (& my info);
	my info. record. gain = theControlPanel. leftGain > theControlPanel. rightGain ?
		theControlPanel. leftGain : theControlPanel. rightGain;
	my info. record. balance = theControlPanel. leftGain == theControlPanel. rightGain ? 32 :
		theControlPanel. leftGain > theControlPanel. rightGain ?
		32 * theControlPanel. rightGain / theControlPanel. leftGain :
		64 - 32 * theControlPanel. leftGain / theControlPanel. rightGain;
	ioctl (my fd, AUDIO_SETINFO, & my info);
	AUDIO_INITINFO (& my info);
	my info. record. sample_rate = theControlPanel. sampleRate;
	ioctl (my fd, AUDIO_SETINFO, & my info);

	AUDIO_INITINFO (& my info);
	my info. record. precision = 16;
	my info. record. encoding = AUDIO_ENCODING_LINEAR;
	my info. record. channels = my numberOfChannels;
	ioctl (my fd, AUDIO_SETINFO, & my info);   /* 16-bit linear mono/stereo! */
	ioctl (my fd, AUDIO_GETINFO, & my info);
	if (my info. record. channels != my numberOfChannels)
		return Melder_error ("(SoundRecorder:) Cannot set to %s.", my numberOfChannels == 1 ? "mono" : "stereo");

	AUDIO_INITINFO (& my info);
	my info. record. pause = 0;
	ioctl (my fd, AUDIO_SETINFO, & my info);   /* Start listening! */
	return 1;
}
#endif

#ifdef HPUX
static int open_hp (SoundRecorder me) {
	struct audio_limits limits;
	int dataFormat, channels, wantedInput, currentInput, sampleRate, bufferSize;
	my fd = open ("/dev/audio", O_RDONLY);
	if (my fd == -1) {
		if (errno == EBUSY)
			return Melder_error ("(SoundRecorder:) Audio device already in use.");
		else
			return Melder_error ("(SoundRecorder:) Cannot open audio device.");
	}
	ioctl (my fd, AUDIO_RESET, RESET_RX_BUF | RESET_RX_OVF);
	ioctl (my fd, AUDIO_PAUSE, AUDIO_RECEIVE);

	ioctl (my fd, AUDIO_GET_DATA_FORMAT, & dataFormat);
	if (dataFormat != AUDIO_FORMAT_LINEAR16BIT && ioctl (my fd, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT) == -1)
		return Melder_error ("(SoundRecorder:) Cannot set 16-bit linear.");

	ioctl (my fd, AUDIO_GET_CHANNELS, & channels);
	if (channels != my numberOfChannels && ioctl (my fd, AUDIO_SET_CHANNELS, my numberOfChannels) == -1)
		return Melder_error ("(SoundRecorder:) Cannot set %s input.", my numberOfChannels == 1 ? "mono" : "stereo");

	wantedInput = theControlPanel. inputSource == 2 ? AUDIO_IN_LINE : AUDIO_IN_MIKE;
	ioctl (my fd, AUDIO_GET_INPUT, & currentInput);
	if (currentInput != wantedInput && ioctl (my fd, AUDIO_SET_INPUT, wantedInput) == -1)
		return Melder_error ("(SoundRecorder:) Cannot set input source.");

	ioctl (my fd, AUDIO_GET_SAMPLE_RATE, & sampleRate);
	if (sampleRate != theControlPanel. sampleRate && ioctl (my fd, AUDIO_SET_SAMPLE_RATE, (int) theControlPanel. sampleRate) == -1)
		return Melder_error ("(SoundRecorder:) Cannot set sampling frequency.");

	ioctl (my fd, AUDIO_GET_LIMITS, & limits);
	ioctl (my fd, AUDIO_GET_RXBUFSIZE, & bufferSize);
	if (bufferSize != limits. max_receive_buffer_size && ioctl (my fd, AUDIO_SET_RXBUFSIZE, limits. max_receive_buffer_size) == -1)
		return Melder_error ("(SoundRecorder:) Cannot set buffer size.");

	ioctl (my fd, AUDIO_RESUME, AUDIO_RECEIVE);
	return 1;
}
#endif

#ifdef linux
static int open_linux (SoundRecorder me) {
	int sampleRate = (int) theControlPanel. sampleRate, sampleSize = 16;
	int channels = my numberOfChannels, stereo = ( my numberOfChannels == 2 ), format = AFMT_S16_LE, val;
	int fd_mixer;
	my fd = open ("/dev/dsp", O_RDONLY);
	if (my fd == -1) {
		if (errno == EBUSY)
			return Melder_error ("(SoundRecorder:) Audio device already in use.");
		else
			return Melder_error ("(SoundRecorder:) Cannot open audio device.\n"
				"Consult /usr/doc/HOWTO/Sound-HOWTO.");
	}
	ioctl (my fd, SNDCTL_DSP_RESET, NULL);
	ioctl (my fd, SNDCTL_DSP_SPEED, & sampleRate);
	ioctl (my fd, SNDCTL_DSP_SAMPLESIZE, & sampleSize);
	ioctl (my fd, SNDCTL_DSP_CHANNELS, (val = channels, & val));
	if (channels == 1 && val == 2) {
		close (my fd);
		return Melder_error ("(SoundRecorder:) This sound card does not support mono.");
	}
	ioctl (my fd, SNDCTL_DSP_STEREO, & stereo);
	ioctl (my fd, SNDCTL_DSP_SETFMT, & format);
	fd_mixer = open ("/dev/mixer", O_WRONLY);		
	if (fd_mixer == -1) {
		return Melder_error ("(SoundRecorder:) Cannot open /dev/mixer.");
	} else {
		int dev_mask = theControlPanel. inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
		if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
			Melder_flushError ("(SoundRecorder:) Can't set recording device in mixer.");		
		close (fd_mixer);
	}
	return 1;
}
#endif

static void cb_input (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	theControlPanel. inputSource =
		w == my microphoneButton ? 1 :
		w == my lineButton ? 2 :
		w == my digitalButton ? 3 :
		w == my in4Button ? 4 :
		w == my in5Button ? 5 :
		w == my in6Button ? 6 :
		w == my in7Button ? 7 :
		w == my in8Button ? 8 : 1;
	(void) call;

	/* Set system's input source. */
	#if defined (sgi)
		my info [0] = AL_INPUT_SOURCE;
		my info [1] =
			theControlPanel. inputSource == 1 ? AL_INPUT_MIC :
			theControlPanel. inputSource == 2 ? AL_INPUT_LINE : AL_INPUT_DIGITAL;
		ALsetparams (AL_DEFAULT_DEVICE, my info, 2);
	#elif defined (macintosh)
	{
		SPBCloseDevice (my refNum);
		strcpy (macDefaultDevice, (const char *) & my hybridDeviceNames [theControlPanel. inputSource] [1]);
		strcpy (macDefaultSource, (const char *) & my macSourceTitle [theControlPanel. inputSource] [0]);
		if (! open_mac (me)) Melder_flushError (NULL);
	}
	#elif defined (sun)
		AUDIO_INITINFO (& my info);
		my info. record. port =
			theControlPanel. inputSource == 1 ? AUDIO_MICROPHONE : AUDIO_LINE_IN;
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#elif defined (HPUX)
		ioctl (my fd, AUDIO_SET_INPUT,
			theControlPanel. inputSource == 1 ? AUDIO_IN_MIKE : AUDIO_IN_LINE);
	#elif defined (linux)
	{
		int dev_mask, fd_mixer = open ("/dev/mixer", O_WRONLY);		
		if (fd_mixer == -1) {
			Melder_flushError ("(Sound_record:) Cannot open /dev/mixer.");
		}
		dev_mask = theControlPanel.inputSource == 2 ? SOUND_MASK_LINE : SOUND_MASK_MIC;
		if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
			Melder_flushError ("(Sound_record:) Can't set recording device in mixer");		
		close(fd_mixer);
	}
	#endif
}

static void cb_fsamp (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	double fsamp =
		w == my button8000 ? 8000 :
		w == my button9800 ? 9800 :
		w == my button11025 ? 11025 :
		w == my button12000 ? 12000 :
		w == my button16000 ? 16000 :
		w == my button22050 ? 22050 :
		w == my button22254 ? 22254.54545 :
		w == my button24000 ? 24000 :
		w == my button32000 ? 32000 :
		w == my button44100 ? 44100 :
		w == my button48000 ? 48000 :
		/* w == my button64000 */ 64000;
	(void) call;
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
	#if defined (sgi)
		my info [0] = AL_INPUT_RATE;
		my info [1] = (int) theControlPanel. sampleRate;
		ALsetparams (AL_DEFAULT_DEVICE, my info, 2);
	#elif defined (_WIN32)
	#elif defined (macintosh)
		SPBCloseDevice (my refNum);
		if (! open_mac (me)) Melder_flushError (NULL);
	#elif defined (sun)
		AUDIO_INITINFO (& my info);
		my info. record. sample_rate = (int) theControlPanel. sampleRate;
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#elif defined (HPUX)
		close (my fd);
		sleep (1);
		if (! open_hp (me)) Melder_flushError (NULL);
	#elif defined (linux)		
		close (my fd);
		if (! open_linux (me)) Melder_flushError (NULL);
	#endif
#ifdef _WIN32
end:
	iferror Melder_flushError ("Cannot change sampling frequency.");
#endif
}

static void createChildren (I) {
	iam (SoundRecorder);
	Widget column, row, rcInput, rcMeter, rcMeters, rcRate, rcLeft, rcRight;
	Widget fsamp, form;
	#ifndef _WIN32
		Widget input;
	#endif

	column = XmCreateRowColumn (my dialog, "column", NULL, 0);
	XtVaSetValues (column, XmNorientation, XmVERTICAL, XmNspacing, 5, XmNy, Machine_getMenuBarHeight (), NULL);

	row = XmCreateRowColumn (column, "row", NULL, 0);
	XtVaSetValues (row, XmNorientation, XmHORIZONTAL, XmNspacing, 22, NULL);

	rcInput = XmCreateRowColumn (row, "rc", NULL, 0);
	XtVaCreateManagedWidget ("Input source:", xmLabelWidgetClass, rcInput, XmNy, 30, NULL);
	#if defined (_WIN32)
		XtVaCreateManagedWidget ("(use Windows mixer", xmLabelWidgetClass, rcInput, XmNy, 60, NULL);
		XtVaCreateManagedWidget ("   without meters)", xmLabelWidgetClass, rcInput, XmNy, 85, NULL);
	#else
		input = XmCreateRadioBox (rcInput, "input", NULL, 0);
		XtVaSetValues (input, XmNy, 55, NULL);
		#ifdef macintosh
			if (my numberOfMacSources >= 1)
				my microphoneButton = XmCreateToggleButton (input, my macSourceTitle [1], NULL, 0);
			else
				my microphoneButton = XmCreateToggleButton (input, "Built-in", NULL, 0);
			if (my numberOfMacSources >= 2)
				my lineButton = XmCreateToggleButton (input, my macSourceTitle [2], NULL, 0);
			if (my numberOfMacSources >= 3)
				my digitalButton = XmCreateToggleButton (input, my macSourceTitle [3], NULL, 0);
			if (my numberOfMacSources >= 4)
				my in4Button = XmCreateToggleButton (input, my macSourceTitle [4], NULL, 0);
			if (my numberOfMacSources >= 5)
				my in5Button = XmCreateToggleButton (input, my macSourceTitle [5], NULL, 0);
			if (my numberOfMacSources >= 6)
				my in6Button = XmCreateToggleButton (input, my macSourceTitle [6], NULL, 0);
			if (my numberOfMacSources >= 7)
				my in7Button = XmCreateToggleButton (input, my macSourceTitle [7], NULL, 0);
			if (my numberOfMacSources >= 8)
				my in8Button = XmCreateToggleButton (input, my macSourceTitle [8], NULL, 0);
		#else
			my microphoneButton = XmCreateToggleButton (input, "microphone", NULL, 0);
			my lineButton = XmCreateToggleButton (input, "line", NULL, 0);
			#if defined (sgi)
				my digitalButton = XmCreateToggleButton (input, "digital", NULL, 0);
			#endif
		#endif
		if (my microphoneButton) XtAddCallback (my microphoneButton, XmNvalueChangedCallback, cb_input, me);
		if (my microphoneButton) XtManageChild (my microphoneButton);
		if (my lineButton) XtAddCallback (my lineButton, XmNvalueChangedCallback, cb_input, me);
		if (my lineButton) XtManageChild (my lineButton);
		if (my digitalButton) XtAddCallback (my digitalButton, XmNvalueChangedCallback, cb_input, me);
		if (my digitalButton) XtManageChild (my digitalButton);
		if (my in4Button) XtAddCallback (my in4Button, XmNvalueChangedCallback, cb_input, me);
		if (my in4Button) XtManageChild (my in4Button);
		if (my in5Button) XtAddCallback (my in5Button, XmNvalueChangedCallback, cb_input, me);
		if (my in5Button) XtManageChild (my in5Button);
		if (my in6Button) XtAddCallback (my in6Button, XmNvalueChangedCallback, cb_input, me);
		if (my in6Button) XtManageChild (my in6Button);
		if (my in7Button) XtAddCallback (my in7Button, XmNvalueChangedCallback, cb_input, me);
		if (my in7Button) XtManageChild (my in7Button);
		if (my in8Button) XtAddCallback (my in8Button, XmNvalueChangedCallback, cb_input, me);
		if (my in8Button) XtManageChild (my in8Button);
		XtManageChild (input);
	#endif
	XtManageChild (rcInput);

	rcMeter = XmCreateRowColumn (row, "rc", NULL, 0);
	XtVaCreateManagedWidget ("Meter", xmLabelWidgetClass, rcMeter, NULL);
	rcMeters = XmCreateRowColumn (rcMeter, "rc", NULL, 0);
	XtVaSetValues (rcMeters, XmNorientation, XmHORIZONTAL, XmNspacing, 12, NULL);
	my leftMeter = XmCreateDrawingArea (rcMeters, "meter", NULL, 0);
	XtVaSetValues (my leftMeter, XmNheight, 150, XmNy, 25, XmNwidth, 12, XmNborderWidth, 1, NULL);
	XtManageChild (my leftMeter);
	if (my numberOfChannels == 2) {
		my rightMeter = XmCreateDrawingArea (rcMeters, "meter", NULL, 0);
		XtVaSetValues (my rightMeter, XmNheight, 150, XmNy, 25, XmNwidth, 12, XmNborderWidth, 1, NULL);
		XtManageChild (my rightMeter);
	}
	XtManageChild (rcMeters);
	XtManageChild (rcMeter);

	#if defined (UNIX)
	{
		Widget rcG, rcGain, rcGains;
		rcG = XmCreateRowColumn (row, "rcG", NULL, 0);
		rcGain = XmCreateRowColumn (rcG, "rcGain", NULL, 0);
		XtVaCreateManagedWidget ("Gain:", xmLabelWidgetClass, rcGain, NULL);
		rcGains = XmCreateRowColumn (rcGain, "rc", NULL, 0);
		XtVaSetValues (rcGains, XmNorientation, XmHORIZONTAL, XmNspacing, 12, NULL);
		my leftGainScale = XmCreateScale (rcGains, "gainScale", NULL, 0);
		XtVaSetValues (my leftGainScale, XmNscaleHeight, 150, XmNy, 25,
			#ifdef macintosh
				XmNwidth, 16,
			#endif
			XmNminimum, 0, XmNmaximum, 255, NULL);
		XtAddCallback (my leftGainScale, XmNvalueChangedCallback, cb_leftGain, me);
		XtAddCallback (my leftGainScale, XmNdragCallback, cb_leftGain, me);
		XtManageChild (my leftGainScale);
		if (my numberOfChannels == 2) {
			my rightGainScale = XmCreateScale (rcGains, "gainScale", NULL, 0);
			XtVaSetValues (my rightGainScale, XmNscaleHeight, 150, XmNy, 25,
				#ifdef macintosh
					XmNwidth, 16,
				#endif
				XmNminimum, 0, XmNmaximum, 255, NULL);
			XtAddCallback (my rightGainScale, XmNvalueChangedCallback, cb_rightGain, me);
			XtAddCallback (my rightGainScale, XmNdragCallback, cb_rightGain, me);
			XtManageChild (my rightGainScale);
		}
		XtManageChild (rcGains);
		if (my numberOfChannels == 2) {
			my coupleButton = XtVaCreateManagedWidget ("Tied", xmToggleButtonWidgetClass, rcG, NULL);
			XtVaSetValues (my coupleButton, XmNwidth, 20, NULL);
			XmToggleButtonSetState (my coupleButton, my coupled = theCouplePreference, False);
			XtAddCallback (my coupleButton, XmNvalueChangedCallback, cb_couple, me);
		}
		XtManageChild (rcGain);
		XtManageChild (rcG);
	}
	#endif

	rcRate = XmCreateRowColumn (row, "rc", NULL, 0);
	XtVaCreateManagedWidget ("Sampling frequency:", xmLabelWidgetClass, rcRate, NULL);
	fsamp = XmCreateRadioBox (rcRate, "fsamp", NULL, 0);
	XtVaSetValues (fsamp, XmNy, 25, NULL);
	if (my can8000) {
		my button8000 = XmCreateToggleButton (fsamp, "8000", NULL, 0);
		XtAddCallback (my button8000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button8000);
	}
	if (my can9800) {
		my button9800 = XmCreateToggleButton (fsamp, "9800", NULL, 0);
		XtAddCallback (my button9800, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button9800);
	}
	if (my can11025) {
		my button11025 = XmCreateToggleButton (fsamp, "11025", NULL, 0);
		XtAddCallback (my button11025, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button11025);
	}
	if (my can12000) {
		my button12000 = XmCreateToggleButton (fsamp, "12000", NULL, 0);
		XtAddCallback (my button12000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button12000);
	}
	if (my can16000) {
		my button16000 = XmCreateToggleButton (fsamp, "16000", NULL, 0);
		XtAddCallback (my button16000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button16000);
	}
	if (my can22050) {
		my button22050 = XmCreateToggleButton (fsamp, "22050", NULL, 0);
		XtAddCallback (my button22050, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button22050);
	}
	if (my can22254) {
		my button22254 = XmCreateToggleButton (fsamp, "22254.54545", NULL, 0);
		XtAddCallback (my button22254, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button22254);
	}
	if (my can24000) {
		my button24000 = XmCreateToggleButton (fsamp, "24000", NULL, 0);
		XtAddCallback (my button24000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button24000);
	}
	if (my can32000) {
		my button32000 = XmCreateToggleButton (fsamp, "32000", NULL, 0);
		XtAddCallback (my button32000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button32000);
	}
	if (my can44100) {
		my button44100 = XmCreateToggleButton (fsamp, "44100", NULL, 0);
		XtAddCallback (my button44100, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button44100);
	}
	if (my can48000) {
		my button48000 = XmCreateToggleButton (fsamp, "48000", NULL, 0);
		XtAddCallback (my button48000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button48000);
	}
	if (my can64000) {
		my button64000 = XmCreateToggleButton (fsamp, "64000", NULL, 0);
		XtAddCallback (my button64000, XmNvalueChangedCallback, cb_fsamp, me);
		XtManageChild (my button64000);
	}
	XtManageChild (fsamp);
	XtManageChild (rcRate);

	XtManageChild (row);

	my progressScale = XmCreateScale (column, "scale", NULL, 0);
	XtVaSetValues (my progressScale, XmNorientation, XmHORIZONTAL,
		XmNminimum, 0, XmNmaximum, 1000,
		#ifdef macintosh
			XmNscaleWidth, 340,
		#endif
		NULL);
	XtManageChild (my progressScale);

	form = XmCreateForm (column, "form", NULL, 0);

	row = XmCreateRowColumn (form, "row", NULL, 0);
	XtVaSetValues (row, XmNorientation, XmHORIZONTAL, XmNspacing, 20, NULL);

	my recordButton = XmCreatePushButton (row, "Record", NULL, 0);
	XtAddCallback (my recordButton, XmNactivateCallback, cb_record, (XtPointer) me);
	#ifdef macintosh
		XtVaSetValues (my recordButton, XmNx, 4, NULL);
	#endif
	XtManageChild (my recordButton);
	my stopButton = XmCreatePushButton (row, "Stop", NULL, 0);
	XtAddCallback (my stopButton, XmNactivateCallback, cb_stop, (XtPointer) me);
	#ifdef macintosh
		XtVaSetValues (my stopButton, XmNx, 80, NULL);
	#endif
	XtManageChild (my stopButton);
	#if defined (sgi) || defined (_WIN32) || defined (macintosh)
		my playButton = XmCreatePushButton (row, "Play", NULL, 0);
		XtAddCallback (my playButton, XmNactivateCallback, cb_play, (XtPointer) me);
		#ifdef macintosh
			XtVaSetValues (my playButton, XmNx, 140, NULL);
		#endif
		XtManageChild (my playButton);
	#endif

	my closeButton = XmCreatePushButton (form, "Close", NULL, 0);
	XtAddCallback (my closeButton, XmNactivateCallback, cb_close, (XtPointer) me);
	XtVaSetValues (my closeButton, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset,
		#ifdef macintosh
			6,
		#else
			0,
		#endif
		NULL);
	XtManageChild (my closeButton);

	XtManageChild (row);
	XtManageChild (form);

	XtManageChild (XmCreateSeparator (column, "sep", NULL, 0));

	row = XmCreateRowColumn (column, "row", NULL, 0);
	XtVaSetValues (row, XmNorientation, XmHORIZONTAL, XmNspacing, 20, NULL);

	rcLeft = XmCreateRowColumn (row, "rc", NULL, 0);
	XtVaSetValues (rcLeft, XmNorientation, XmVERTICAL, NULL);
	my publishLeftButton = XmCreatePushButton (rcLeft,
		my numberOfChannels == 1 ? "Save to list:" : "Save left channel to list:", NULL, 0);
	XtAddCallback (my publishLeftButton, XmNactivateCallback, cb_publishLeft, (XtPointer) me);
	XtManageChild (my publishLeftButton);
	my leftName = XtVaCreateManagedWidget ("left", xmTextFieldWidgetClass, rcLeft, XmNcolumns, 10, XmNx, 20,
		#ifdef macintosh
			#if TARGET_API_MAC_CARBON
				XmNy, 32,
			#endif
		#endif
		NULL);
	XtAddCallback (my leftName, XmNactivateCallback, cb_publishLeft, (XtPointer) me);
	XmTextFieldSetString (my leftName, my numberOfChannels == 1 ? "sound" : "left");
	XtManageChild (rcLeft);

	if (my numberOfChannels == 2) {
		rcRight = XmCreateRowColumn (row, "rc", NULL, 0);
		XtVaSetValues (rcRight, XmNorientation, XmVERTICAL, NULL);
		my publishRightButton = XmCreatePushButton (rcRight, "Save right channel to list:", NULL, 0);
		XtAddCallback (my publishRightButton, XmNactivateCallback, cb_publishRight, (XtPointer) me);
		XtManageChild (my publishRightButton);
		my rightName = XtVaCreateManagedWidget ("right", xmTextFieldWidgetClass, rcRight, XmNcolumns, 10, XmNx, 20,
			#ifdef macintosh
				#if TARGET_API_MAC_CARBON
					XmNy, 32,
				#endif
			#endif
			NULL);
		XtAddCallback (my rightName, XmNactivateCallback, cb_publishRight, (XtPointer) me);
		XmTextFieldSetString (my rightName, "right");
		XtManageChild (rcRight);
	}

	XtManageChild (row);

	XtManageChild (column);
}

static int writeAudioFile_mono (SoundRecorder me, MelderFile file, int audioFileType) {
	long i, nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer) {
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, theControlPanel. sampleRate, nsamp, 1);
		if (Melder_defaultAudioFileEncoding16 (audioFileType) == Melder_LINEAR_16_BIG_ENDIAN) {
			if (my fakeMono) {
				for (i = 0; i < nsamp; i ++)
					binputi2 ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
			} else {
				for (i = 0; i < nsamp; i ++)
					binputi2 (my buffer [i], file -> filePointer);
			}
		} else {
			if (my fakeMono) {
				for (i = 0; i < nsamp; i ++)
					binputi2LE ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
			} else {
				for (i = 0; i < nsamp; i ++)
					binputi2LE (my buffer [i], file -> filePointer);
			}
		}
	}
	MelderFile_close (file);
	iferror return Melder_error ("Audio file not written.");
	return 1;
}

FORM_WRITE (SoundRecorder, cb_writeWav, "Write to WAV file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.wav", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_mono (me, file, Melder_WAV)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeAifc, "Write to AIFC file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.aifc", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_mono (me, file, Melder_AIFC)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeNextSun, "Write to NeXT/Sun file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.au", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_mono (me, file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeNist, "Write to NIST file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.nist", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_mono (me, file, Melder_NIST)) return 0;
END

static int writeAudioFile_oneChannel (SoundRecorder me, MelderFile file, int audioFileType, int channel /* 0 = left, 1 = right */) {
	long i;
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer) {
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, theControlPanel. sampleRate, my nsamp, 1);
		if (Melder_defaultAudioFileEncoding16 (audioFileType) == Melder_LINEAR_16_BIG_ENDIAN) {
			for (i = 0; i < my nsamp; i ++)
				binputi2 (my buffer [i + i + channel], file -> filePointer);
		} else {
			for (i = 0; i < my nsamp; i ++)
				binputi2LE (my buffer [i + i + channel], file -> filePointer);
		}
	}
	MelderFile_close (file);
	iferror return Melder_error ("Audio file not written.");
	return 1;
}

FORM_WRITE (SoundRecorder, cb_writeLeftWav, "Write left to WAV file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.wav", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_WAV, 0)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeRightWav, "Write right to WAV file", 0)
	char *name = XmTextFieldGetString (my rightName);
	sprintf (defaultName, "%s.wav", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_WAV, 1)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeStereoWav, "Write to stereo WAV file", 0)
	sprintf (defaultName, "stereo.wav");
DO_WRITE
	if (! MelderFile_writeAudioFile16 (file, Melder_WAV, my buffer, theControlPanel. sampleRate, my nsamp, 2)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeLeftAifc, "Write left to AIFC file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.aifc", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_AIFC, 0)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeRightAifc, "Write right to AIFC file", 0)
	char *name = XmTextFieldGetString (my rightName);
	sprintf (defaultName, "%s.aifc", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_AIFC, 1)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeStereoAifc, "Write to stereo AIFC file", 0)
	sprintf (defaultName, "stereo.aifc");
DO_WRITE
	if (! MelderFile_writeAudioFile16 (file, Melder_AIFC, my buffer, theControlPanel. sampleRate, my nsamp, 2)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeLeftNextSun, "Write left to NeXT/Sun file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.au", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_NEXT_SUN, 0)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeRightNextSun, "Write right to NeXT/Sun file", 0)
	char *name = XmTextFieldGetString (my rightName);
	sprintf (defaultName, "%s.au", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_NEXT_SUN, 1)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeStereoNextSun, "Write to stereo NeXT/Sun file", 0)
	sprintf (defaultName, "stereo.au");
DO_WRITE
	if (! MelderFile_writeAudioFile16 (file, Melder_NEXT_SUN, my buffer, theControlPanel. sampleRate, my nsamp, 2)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeLeftNist, "Write left to NIST file", 0)
	char *name = XmTextFieldGetString (my leftName);
	sprintf (defaultName, "%s.nist", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_NIST, 0)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeRightNist, "Write right to NIST file", 0)
	char *name = XmTextFieldGetString (my rightName);
	sprintf (defaultName, "%s.nist", name);
	XtFree (name);
DO_WRITE
	if (! writeAudioFile_oneChannel (me, file, Melder_NIST, 1)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeStereoNist, "Write to stereo NIST file", 0)
	sprintf (defaultName, "stereo.nist");
DO_WRITE
	if (! MelderFile_writeAudioFile16 (file, Melder_NIST, my buffer, theControlPanel. sampleRate, my nsamp, 2)) return 0;
END

DIRECT (SoundRecorder, cb_SoundRecorder_help) Melder_help ("SoundRecorder"); END

static void createMenus (I) {
	iam (SoundRecorder);
	inherited (SoundRecorder) createMenus (me);
	if (my numberOfChannels == 1) {
		Editor_addCommand (me, "File", "Write to WAV file...", 0, cb_writeWav);
		Editor_addCommand (me, "File", "Write to AIFC file...", 0, cb_writeAifc);
		Editor_addCommand (me, "File", "Write to NeXT/Sun file...", 0, cb_writeNextSun);
		Editor_addCommand (me, "File", "Write to NIST file...", 0, cb_writeNist);
		Editor_addCommand (me, "File", "-- write --", 0, 0);
	} else {
		Editor_addCommand (me, "File", "Write left channel to WAV file...", 0, cb_writeLeftWav);
		Editor_addCommand (me, "File", "Write right channel to WAV file...", 0, cb_writeRightWav);
		Editor_addCommand (me, "File", "Write to stereo WAV file...", 0, cb_writeStereoWav);
		Editor_addCommand (me, "File", "-- write wav --", 0, 0);
		Editor_addCommand (me, "File", "Write left channel to AIFC file...", 0, cb_writeLeftAifc);
		Editor_addCommand (me, "File", "Write right channel to AIFC file...", 0, cb_writeRightAifc);
		Editor_addCommand (me, "File", "Write to stereo AIFC file...", 0, cb_writeStereoAifc);
		Editor_addCommand (me, "File", "-- write aifc --", 0, 0);
		Editor_addCommand (me, "File", "Write left channel to NeXT/Sun file...", 0, cb_writeLeftNextSun);
		Editor_addCommand (me, "File", "Write right channel to NeXT/Sun file...", 0, cb_writeRightNextSun);
		Editor_addCommand (me, "File", "Write to stereo NeXT/Sun file...", 0, cb_writeStereoNextSun);
		Editor_addCommand (me, "File", "-- write next/sun --", 0, 0);
		Editor_addCommand (me, "File", "Write left channel to NIST file...", 0, cb_writeLeftNist);
		Editor_addCommand (me, "File", "Write right channel to NIST file...", 0, cb_writeRightNist);
		Editor_addCommand (me, "File", "Write to stereo NIST file...", 0, cb_writeStereoNist);
		Editor_addCommand (me, "File", "-- write nist --", 0, 0);
	}
	Editor_addCommand (me, "Help", "SoundRecorder help", '?', cb_SoundRecorder_help);
}

class_methods (SoundRecorder, Editor)
	class_method (destroy)
	class_method (createChildren)
	us -> editable = FALSE;
	us -> scriptable = FALSE;
	class_method (createMenus)
class_methods_end

SoundRecorder SoundRecorder_create (Widget parent, int numberOfChannels, XtAppContext context) {
	SoundRecorder me = new (SoundRecorder);
	#if defined (_WIN32)
		UINT numberOfDevices = waveInGetNumDevs (), i;
		WAVEINCAPS caps;
		MMRESULT err;
		if (numberOfDevices == 0) { Melder_error ("No sound input devices available."); goto error; }
		err = waveInGetDevCaps (WAVE_MAPPER, & caps, sizeof (WAVEINCAPS));
		if (numberOfChannels == 2 && caps. wChannels < 2) {
			Melder_error ("Your computer does not support stereo sound input.");
			goto error;
		}
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
		{
			Melder_error ("(SoundRecorder_create:) Your computer does not support sound input.");
			goto error;
		}
		if (! (soundFeatures & (1 << gestalt16BitSoundIO)) ||   /* Hardware. */
		    ! (soundFeatures & (1 << gestaltStereoInput)) ||   /* Hardware. */
		    ! (soundFeatures & (1 << gestalt16BitAudioSupport)))   /* Software. */
		{
			Melder_error ("Your computer does not support stereo sound input.");
			goto error;
		}
	#endif
	my numberOfChannels = numberOfChannels;
	if (sizeof (short) != 2)
		return Melder_errorp ("Long shorts!!!!!");
	#if defined (macintosh) || defined (_WIN32)
		my synchronous = FALSE;
	#else
		my synchronous = TRUE;
	#endif
	/*
	 * Allocate the maximum buffer.
	 */
	if (nmaxMB_pref < 1) nmaxMB_pref = 1;   /* Validate preferences. */
	if (nmaxMB_pref > 1000) nmaxMB_pref = 1000;
	#if defined (macintosh) && ! defined (__MACH__)
	{
		OSErr err;
		/*
		 * First try: if virtual memory is off,
		 * we try to allocate most of the remaining physical temporary memory.
		 */
		long virtualMemoryResponse;
		Gestalt (gestaltVMAttr, & virtualMemoryResponse);
		if ((virtualMemoryResponse & (1 << gestaltVMPresent)) == 0) {
			long max = (TempMaxMem (0) / 5) * 4 - 2000000;   /* Leave 80% plus 2 megabytes. */
			if (max > 100000) {
				my nmax = max / (sizeof (short) * numberOfChannels);
				my temporaryMemoryHandle = TempNewHandle (my nmax * (sizeof (short) * numberOfChannels), & err);
				if (err != noErr) { Melder_error ("Out of memory. Should not occur."); goto error; }
				HLock (my temporaryMemoryHandle);
				my buffer = (short *) * my temporaryMemoryHandle;
			}
		}
		/*
		 * If virtual memory is on, or if allocation has not succeeded,
		 * we are limited by the "maximum buffer size" preference,
		 * which is a user-settable trade-off between maximum memory use and maximum initialization speed.
		 */
		if (my buffer == NULL) {
			/*
			 * Second try: temporary memory.
			 */
			my nmax = nmaxMB_pref * 1000000 / (sizeof (short) * numberOfChannels);
			for (;;) {
				my temporaryMemoryHandle = TempNewHandle (my nmax * (sizeof (short) * numberOfChannels), & err);
				if (err == noErr) {
					HLock (my temporaryMemoryHandle);
					my buffer = (short *) * my temporaryMemoryHandle;
					break;   /* Success. */
				}
				if (my nmax < 500000) break;   /* Failure, without error message. */
				my nmax /= 2;   /* Retry with less temporary memory. */
			}
		}
	}
	#endif
	if (my buffer == NULL) {
		/*
		 * Third try: application memory.
		 */
		long nmax_bytes_pref = nmaxMB_pref * 1000000;
		long nmax_bytes =
			#if defined (_WIN32)
				66150000;   /* The maximum physical buffer on Windows XP; shorter than in Windows 98, alas. */
			#else
				nmax_bytes_pref;
			#endif
		my nmax = nmax_bytes / (sizeof (short) * numberOfChannels);
		for (;;) {
			my buffer = NUMsvector (0, my nmax * numberOfChannels - 1);
			if (my buffer) break;   /* Success. */
			if (my nmax < 100000) break;   /* Failure, with error message. */
			Melder_clearError ();
			my nmax /= 2;   /* Retry with less application memory. */
		}
	}
	if (my buffer == NULL) goto error;

	/*
	 * Count the number of input devices and sources.
	 */
	#if defined (macintosh)
	{
		int idevice;
		my numberOfMacSources = 0;
		for (idevice = 1; idevice <= 8; idevice ++) {
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
						my numberOfMacSources ++;
						if (my numberOfMacSources > 8) break;
						strcpy ((char *) my hybridDeviceNames [my numberOfMacSources], (const char *) hybridDeviceName);
						my macSource [my numberOfMacSources] = deviceSource;
						strncpy (my macSourceTitle [my numberOfMacSources], plength + 1, *plength);
						my macSourceTitle [my numberOfMacSources] [*plength] = '\0';
						if (strequ ((const char *) hybridDeviceName + 1, macDefaultDevice) &&
								strequ (my macSourceTitle [my numberOfMacSources], macDefaultSource))
							theControlPanel. inputSource = my numberOfMacSources;
						plength += *plength + 1;
					}
					DisposeHandle (handle);
				}
				SPBCloseDevice (my refNum);
			}
		}
	}
	#endif

	/*
	 * The default set of possible sampling frequencies, to be modified in the open_xxx procedures.
	 */
	my can8000 = TRUE;
	my can11025 = TRUE;
	my can12000 = TRUE;
	my can16000 = TRUE;
	my can22050 = TRUE;
	my can24000 = TRUE;
	my can32000 = TRUE;
	my can44100 = TRUE;
	my can48000 = TRUE;
	my can64000 = TRUE;

	/*
	 * Initialize system-dependent structures.
	 * On all systems: stereo 16-bit linear encoding.
	 * Some systems take initial values from the system control panel
	 * (automatically in the workProc), other systems from theControlPanel.
	 */
	#if defined (sgi)
		if (! open_sgi (me)) goto error;
	#elif defined (_WIN32)
		if (! open_win (me)) goto error;
	#elif defined (macintosh)
		if (! open_mac (me)) goto error;
	#elif defined (sun)
		if (! open_sun (me)) goto error;
	#elif defined (HPUX)
		if (! open_hp (me)) goto error;
	#elif defined (linux)
		if (! open_linux (me)) goto error;		
	#endif

#if defined (macintosh)
	if (! Editor_init (me, parent, 100, 100, 350, 100, "SoundRecorder", NULL)) goto error;
#elif defined (_WIN32)
	if (! Editor_init (me, parent, 100, 100, 300, 100, "SoundRecorder", NULL)) goto error;
#else
	if (! Editor_init (me, parent, 0, 0, 0, 0, "SoundRecorder", NULL)) goto error;
#endif
	Melder_assert (XtWindow (my leftMeter));
	my leftGraphics = Graphics_create_xmdrawingarea (my leftMeter);
	Melder_assert (my leftGraphics);
	Graphics_setWindow (my leftGraphics, 0.0, 1.0, 0.0, 32768.0);
	if (numberOfChannels == 2) {
		Melder_assert (XtWindow (my rightMeter));
		my rightGraphics = Graphics_create_xmdrawingarea (my rightMeter);
		Melder_assert (my rightGraphics);
		Graphics_setWindow (my rightGraphics, 0.0, 1.0, 0.0, 32768.0);
	}
	my workProcId = XtAppAddWorkProc (context, workProc, (XtPointer) me);
	return me;
error:
	forget (me);
	return Melder_errorp ("SoundRecorder not created.");
}

/* End of file SoundRecorder.c */
