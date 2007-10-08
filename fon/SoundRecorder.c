/* SoundRecorder.c
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
/* Linux code originally by Darryl Purnell, Pretoria */

/*
 * pb 2002/07/16 GPL
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
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/30 stereo
 * pb 2007/01/03 CoreAudio (PortAudio)
 * pb 2007/01/03 flexible drawing area
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
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

//#define USE_PORTAUDIO  1
#ifndef USE_PORTAUDIO
	// Default: use PortAudio on Mac only.
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
#endif

struct SoundRecorder_Device {
	char name [1+40];
	bool canDo;
	Widget button;
};
#define SoundRecorder_IDEVICE_MAX  8

struct SoundRecorder_Fsamp {
	double fsamp;
	bool canDo;
	Widget button;
};
#define SoundRecorder_IFSAMP_8000  1
#define SoundRecorder_IFSAMP_9800  2
#define SoundRecorder_IFSAMP_11025  3
#define SoundRecorder_IFSAMP_12000  4
#define SoundRecorder_IFSAMP_16000  5
#define SoundRecorder_IFSAMP_22050  6
#define SoundRecorder_IFSAMP_22254  7
#define SoundRecorder_IFSAMP_24000  8
#define SoundRecorder_IFSAMP_32000  9
#define SoundRecorder_IFSAMP_44100  10
#define SoundRecorder_IFSAMP_48000  11
#define SoundRecorder_IFSAMP_64000  12
#define SoundRecorder_IFSAMP_96000  13
#define SoundRecorder_IFSAMP_MAX  13

#define CommonSoundRecorder_members Editor_members \
	int numberOfChannels, fakeMono; \
	XtWorkProcId workProcId; \
	long nsamp, nmax; \
	int synchronous, recording, lastLeftMaximum, lastRightMaximum, coupled; \
	long numberOfInputDevices; \
	struct SoundRecorder_Device device [1+SoundRecorder_IDEVICE_MAX]; \
	struct SoundRecorder_Fsamp fsamp [1+SoundRecorder_IFSAMP_MAX]; \
	short *buffer; \
	Widget monoButton, stereoButton, meter; \
	Widget progressScale, recordButton, stopButton, playButton; \
	Widget soundName, cancelButton, applyButton, okButton; \
	Widget leftGainScale, rightGainScale; \
	Widget coupleButton; \
	Graphics graphics;

/* Class definition of SoundRecorder. */

#if USE_PORTAUDIO
	#define SoundRecorder_members CommonSoundRecorder_members \
		const PaDeviceInfo *deviceInfos [1+SoundRecorder_IDEVICE_MAX]; \
		PaDeviceIndex deviceIndices [1+SoundRecorder_IDEVICE_MAX]; \
		PaStream *portaudioStream;
#elif defined (sgi)
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
	#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
	#define SoundRecorder_members CommonSoundRecorder_members \
		Handle temporaryMemoryHandle; \
		short macSource [1+8]; \
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
class_create_opaque (SoundRecorder, Editor);

static int theCouplePreference = 1;
static int nmaxMB_pref = 4;

void SoundRecorder_prefs (void) {
	Resources_addInt (L"SoundRecorder.bufferSize_MB", & nmaxMB_pref);
	Resources_addInt (L"SoundRecorder.coupleSliders", & theCouplePreference);
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
	{ 1, 200, 200, 44100 };
#elif defined (macintosh)
	{ 1, 26, 26, 44100 };
#else
	{ 1, 26, 26, 44100 };
#endif

/********** ERROR HANDLING **********/

#if defined (_WIN32) && ! USE_PORTAUDIO
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
	wchar_t messageText [MAXERRORLENGTH];
	MMRESULT err;
	if (my err == MMSYSERR_NOERROR) return 1;
	err = waveInGetErrorText (my err, messageText, MAXERRORLENGTH);
	if (err == MMSYSERR_NOERROR) Melder_error1 (messageText);
	else if (err == MMSYSERR_BADERRNUM) Melder_error3 (L"Error number ", Melder_integer (my err), L" out of range.");
	else if (err == MMSYSERR_NODRIVER) Melder_error1 (L"No sound driver present.");
	else if (err == MMSYSERR_NOMEM) Melder_error1 (L"Out of memory.");
	else Melder_error1 (L"Unknown sound error.");
	return 0;
}
static int win_waveInOpen (SoundRecorder me) {
	my err = waveInOpen (& my hWaveIn, WAVE_MAPPER, & my waveFormat, 0, 0, CALLBACK_NULL);
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input not opened.");
	if (Melder_debug != 8) waveInReset (my hWaveIn);
	return 1;
}
static int win_waveInPrepareHeader (SoundRecorder me, int which) {
	my err = waveInPrepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input: cannot prepare header.\n"
		"Quit some other programs or go to \"Sound input prefs\" in the Preferences menu.");
	return 1;
}
static int win_waveInAddBuffer (SoundRecorder me, int which) {
	my err = waveInAddBuffer (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input: cannot add buffer.");
	return 1;
}
static int win_waveInStart (SoundRecorder me) {
	my err = waveInStart (my hWaveIn);   /* Asynchronous. */
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input not started.");
	return 1;
}
static int win_waveInStop (SoundRecorder me) {
	my err = waveInStop (my hWaveIn);
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input not stopped.");
	return 1;
}
static int win_waveInReset (SoundRecorder me) {
	my err = waveInReset (my hWaveIn);
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input not reset.");
	return 1;
}
static int win_waveInUnprepareHeader (SoundRecorder me, int which) {
	my err = waveInUnprepareHeader (my hWaveIn, & my waveHeader [which], sizeof (WAVEHDR));
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input: cannot unprepare header.");
	return 1;
}
static int win_waveInClose (SoundRecorder me) {
	my err = waveInClose (my hWaveIn);
	my hWaveIn = 0;
	if (! win_waveInCheck (me)) return Melder_error1 (L"Audio input not closed.");
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
		#if USE_PORTAUDIO
			Pa_StopStream (my portaudioStream);
			Pa_CloseStream (my portaudioStream);
			my portaudioStream = NULL;
		#elif defined (_WIN32)
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
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
}

static void destroy (I) {
	iam (SoundRecorder);
	stopRecording (me);   /* Must occur before freeing my buffer. */
	Melder_stopPlaying (Melder_IMPLICIT);   /* Must also occur before freeing my buffer. */
	if (my workProcId) XtRemoveWorkProc (my workProcId);
	#if defined (macintosh) && ! USE_PORTAUDIO
		if (my temporaryMemoryHandle) {
			DisposeHandle (my temporaryMemoryHandle);
		} else {
			NUMsvector_free (my buffer, 0);
		}
	#else
		NUMsvector_free (my buffer, 0);
	#endif

	#if USE_PORTAUDIO
		if (my portaudioStream) Pa_StopStream (my portaudioStream);
		if (my portaudioStream) Pa_CloseStream (my portaudioStream);
	#elif defined (_WIN32)
		if (my hWaveIn != 0) {
			waveInReset (my hWaveIn);
			waveInUnprepareHeader (my hWaveIn, & my waveHeader [0], sizeof (WAVEHDR));
			waveInClose (my hWaveIn);
		}
	#elif defined (macintosh)
		if (my refNum) SPBCloseDevice (my refNum);
	#elif defined (sgi)
		if (my port) ALcloseport (my port);
		if (my audio) ALfreeconfig (my audio);
	#elif defined (UNIX) || defined (HPUX)
		if (my fd != -1) close (my fd);
	#endif
	forget (my graphics);
	inherited (SoundRecorder) destroy (me);
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
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	//#ifndef _WIN32
	//	Graphics_setColour (my graphics, Graphics_WHITE);
	//	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	//#endif
	Graphics_setColour (my graphics, Graphics_BLACK);
	if (nsamp < 1) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
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

static int tooManySamplesInBufferToReturnToGui (SoundRecorder me) {
	#if defined (sgi)
		return ALgetfilled (my port) > step;
	#else
		(void) me;
		return FALSE;
	#endif
}

static long getMyNsamp (SoundRecorder me) {
	volatile long nsamp = my nsamp;   // Prevent inlining.
	return nsamp;
}

static Boolean workProc (XtPointer void_me) {
	iam (SoundRecorder);
	short buffertje [step*2];
	int leftMaximum = 0, rightMaximum = 0, stepje = 0;

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
		#if USE_PORTAUDIO
		#else
			OSErr err;
			short macSource, isource;
			Str255 pdeviceName;
			err = SPBGetDeviceInfo (my refNum, siDeviceName, & pdeviceName);
			if (err != noErr) { onceError ("SPBGetDeviceInfo (deviceName)", err); return False; }
			err = SPBGetDeviceInfo (my refNum, siInputSource, & macSource);
			if (err != noErr) { onceError ("SPBGetDeviceInfo (inputSource)", err); return False; }
			for (isource = 1; isource <= my numberOfInputDevices; isource ++) {
				if (strequ ((const char *) & pdeviceName, (const char *) my hybridDeviceNames [isource]) &&
						macSource == my macSource [isource]) {
					theControlPanel. inputSource = isource;
					break;
				}
			}
		#endif
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
	#endif

	/* Set the buttons according to the audio parameters. */

	if (my recordButton) XtSetSensitive (my recordButton, my recording == FALSE);
	if (my stopButton) XtSetSensitive (my stopButton, my recording == TRUE);
	if (my playButton) XtSetSensitive (my playButton, my recording == FALSE && my nsamp > 0);
	if (my applyButton) XtSetSensitive (my applyButton, my recording == FALSE && my nsamp > 0);
	if (my okButton) XtSetSensitive (my okButton, my recording == FALSE && my nsamp > 0);
	if (my monoButton) XmToggleButtonSetState (my monoButton, my numberOfChannels == 1, False);
	if (my stereoButton) XmToggleButtonSetState (my stereoButton, my numberOfChannels == 2, False);
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
		if (my fsamp [i]. button)
			XmToggleButtonSetState (my fsamp [i]. button, theControlPanel. sampleRate == my fsamp [i]. fsamp, False);
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
		if (my device [i]. button)
			XmToggleButtonSetState (my device [i]. button, theControlPanel. inputSource == i, False);
	if (my monoButton) XtSetSensitive (my monoButton, ! my recording);
	if (my stereoButton) XtSetSensitive (my stereoButton, ! my recording);
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
		if (my fsamp [i]. button) XtSetSensitive (my fsamp [i]. button, ! my recording);
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++)
		if (my device [i]. button)
			XtSetSensitive (my device [i]. button, ! my recording);
	if (my leftGainScale) XmScaleSetValue (my leftGainScale, theControlPanel. leftGain);
	if (my rightGainScale) XmScaleSetValue (my rightGainScale, theControlPanel. rightGain);

	/*Graphics_setGrey (my graphics, 0.9);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 32768.0);
	Graphics_setGrey (my graphics, 0.9);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 32768.0);*/

	if (my synchronous) {
		/*
		 * Read some samples into 'buffertje'.
		 */
		do {
			#if USE_PORTAUDIO || defined (macintosh) || defined (_WIN32)
				/*
				 * Asynchronous recording on these systems: do nothing.
				 */
			#elif defined (sgi)
				ALreadsamps (my port, buffertje, step * my numberOfChannels);
				stepje = step;
			#else
				// linux, sun, HPUX
				stepje = read (my fd, (void *) buffertje, step * (sizeof (short) * my numberOfChannels)) / (sizeof (short) * my numberOfChannels);
			#endif

			if (my recording) {
				memcpy (my buffer + my nsamp * my numberOfChannels, buffertje, stepje * (sizeof (short) * my numberOfChannels));
			}
			showMeter (me, buffertje, stepje);
			if (my recording) {
				my nsamp += stepje;
				if (my nsamp > my nmax - step) my recording = FALSE;
				XmScaleSetValue (my progressScale,
					(int) (1000.0f * ((float) my nsamp / (float) my nmax)));
			}
		} while (my recording && tooManySamplesInBufferToReturnToGui (me));
	} else {
		if (my recording) {
			/*
			 * We have to know how far the buffer has been filled.
			 * However, the buffer may be filled at interrupt time,
			 * so that the buffer may be filled during this workproc.
			 * So we ask for the buffer filling just once, namely here at the beginning.
			 */
			#if USE_PORTAUDIO
				 /*
				  * The buffer filling is contained in my nsamp,
				  * which has been set during interrupt time and may again be updated behind our backs during this workproc.
				  * So we do it in such a way that the compiler cannot ask for my nsamp twice.
				  */
				long lastSample = getMyNsamp (me);
			#elif defined (_WIN32)
				MMTIME mmtime;
				mmtime. wType = TIME_BYTES;
				long lastSample = 0;
				if (waveInGetPosition (my hWaveIn, & mmtime, sizeof (MMTIME)) == MMSYSERR_NOERROR)
					lastSample = mmtime. u.cb / (sizeof (short) * my numberOfChannels);
			#elif defined (macintosh)
				OSErr err;
				short recordingStatus, meterLevel;
				unsigned long totalSamplesToRecord, numberOfSamplesRecorded, totalMsecsToRecord, numberOfMsecsRecorded;
				err = SPBGetRecordingStatus (my refNum, & recordingStatus, & meterLevel,
						& totalSamplesToRecord, & numberOfSamplesRecorded,
						& totalMsecsToRecord, & numberOfMsecsRecorded);
				if (err != noErr) { onceError ("SPBGetRecordingStatus", err); return FALSE; }
				if (totalSamplesToRecord == 0)
					my nsamp = my nmax;
				else
					my nsamp = numberOfSamplesRecorded / (sizeof (short) * my numberOfChannels);
				long lastSample = my nsamp;
			#else
				long lastSample = 0;   // Will not occur.
			#endif
			long firstSample = lastSample - 1000;
			if (firstSample < 0) firstSample = 0;
			showMeter (me, my buffer + firstSample * my numberOfChannels, lastSample - firstSample);
			XmScaleSetValue (my progressScale, (int) (1000.0f * ((float) lastSample / (float) my nmax)));
		} else {
			showMeter (me, NULL, 0);
		}
	}
	iferror Melder_flushError (NULL);
	return False;
}

#if USE_PORTAUDIO
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
#endif

static void cb_record (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	if (my recording) return;
	my nsamp = 0;
	my recording = TRUE;
	my lastLeftMaximum = 0;
	my lastRightMaximum = 0;
	if (! my synchronous) {
		#if USE_PORTAUDIO
			PaStreamParameters streamParameters = { 0 };
			streamParameters. device = my deviceIndices [theControlPanel. inputSource];
			streamParameters. channelCount = my numberOfChannels;
			streamParameters. sampleFormat = paInt16;
			streamParameters. suggestedLatency = my deviceInfos [theControlPanel. inputSource] -> defaultLowInputLatency;
			#if defined (macintosh)
				struct paMacCoreStreamInfo macCoreStreamInfo = { 0 };
				macCoreStreamInfo. size = sizeof (paMacCoreStreamInfo);
				macCoreStreamInfo. hostApiType = paCoreAudio;
				macCoreStreamInfo. version = 0x01;
				macCoreStreamInfo. flags = paMacCore_ChangeDeviceParameters | paMacCore_FailIfConversionRequired;
				streamParameters. hostApiSpecificStreamInfo = & macCoreStreamInfo;
			#endif
			if (Melder_debug == 20) Melder_casual ("Before Pa_OpenStream");
			PaError err = Pa_OpenStream (& my portaudioStream, & streamParameters, NULL,
				theControlPanel. sampleRate, 0, paNoFlag, portaudioStreamCallback, (void *) me);
			if (Melder_debug == 20) Melder_casual ("Pa_OpenStream returns %d", err);
			if (err) { Melder_error ("open %s", Pa_GetErrorText (err)); goto end; }
			Pa_StartStream (my portaudioStream);
			if (Melder_debug == 20) Melder_casual ("Pa_StartStream returns %d", err);
			if (err) { Melder_error ("start %s", Pa_GetErrorText (err)); goto end; }
		#elif defined (_WIN32)
			win_fillFormat (me);
			win_fillHeader (me, 0);
			win_waveInOpen (me); cherror
			win_waveInPrepareHeader (me, 0); cherror
			win_waveInAddBuffer (me, 0); cherror
			win_waveInStart (me); cherror
		#elif defined (macintosh)
			OSErr err;
			err = SPBStopRecording (my refNum);
			if (err != noErr) { onceError ("SPBStopRecording", err); return; }
			for (;;) {
				my spb. bufferPtr = (char *) my buffer;
				my spb. bufferLength = my spb. count = my nmax * (sizeof (short) * my numberOfChannels);
				err = SPBRecord (& my spb, true);   /* Asynchronous. */
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
		#endif
	}
end:
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	iferror { my recording = FALSE; Melder_flushError ("Cannot record."); }
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

static void publish (SoundRecorder me) {
	Sound sound = NULL;
	long i, nsamp = my fakeMono ? my nsamp / 2 : my nsamp;
	double fsamp = theControlPanel. sampleRate;
	if (my nsamp == 0) return;
	if (fsamp <= 0) fsamp = 48000.0;   /* Safe. */
	sound = Sound_createSimple (my numberOfChannels, (double) nsamp / fsamp, fsamp);
	if (Melder_hasError ()) { Melder_flushError ("You can still save to file."); return; }
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
		wchar_t *name = GuiText_getStringW (my soundName);
		Thing_setNameW (sound, name);
		Melder_free (name);
	}
	if (my publishCallback)
		my publishCallback (me, my publishClosure, sound);
}

static void cb_cancel (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	stopRecording (me);
	forget (me);
}

static void cb_apply (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	stopRecording (me);
	publish (me);
}

static void cb_ok (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	(void) w;
	(void) call;
	stopRecording (me);
	publish (me);
	forget (me);
}

static void setSystemInputGains (SoundRecorder me) {
	#if USE_PORTAUDIO
		(void) me;
	#elif defined (sgi)
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

static int initialize (SoundRecorder me) {
	#if USE_PORTAUDIO
		#if defined (macintosh)
			my fsamp [SoundRecorder_IFSAMP_8000]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_11025]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_12000]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_16000]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_22050]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_24000]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_32000]. canDo = false;
			my fsamp [SoundRecorder_IFSAMP_64000]. canDo = false;
		#else
			// Accept all standard sample rates.
			(void) me;
		#endif
	#elif defined (sgi)
		my audio = ALnewconfig ();
		if (! my audio)
			return Melder_error ("Unexpected error: cannot create audio config...");
		ALsetchannels (my audio, my numberOfChannels == 1 ? AL_MONO : AL_STEREO);
		ALsetwidth (my audio, AL_SAMPLE_16);
		if (! (my port = ALopenport ("SoundRecorder", "r", my audio)))
			return Melder_error ("Cannot open audio port (too many open already).");
		my can9800 = TRUE;
	#elif defined (macintosh)
		unsigned long sampleRate_uf = theControlPanel. sampleRate * 65536L;
		short numberOfChannels = my numberOfChannels, continuous = TRUE, sampleSize = 16, async;
		char levelMeterOnOff = 1;
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
			In order to accommodate sample rates greater than 32 kHz, the most significant bit is not treated as a sign bit;
			instead, that bit is interpreted as having the value 32,768."
		 */
		SPBGetDeviceInfo (my refNum, siSampleRateAvailable, & sampleRateInfo. number);
		if (sampleRateInfo. number == 0) {
		} else {
			for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
				my fsamp [i]. canDo = false;
			}
			for (irate = 1; irate <= sampleRateInfo. number; irate ++) {
				Fixed rate_fixed = (* (Fixed **) sampleRateInfo. handle) [irate - 1];
				unsigned short rate_ushort = * (unsigned short *) & rate_fixed;
				switch (rate_ushort) {
					case 0: my fsamp [SoundRecorder_IFSAMP_44100]. canDo = true,
					        my fsamp [SoundRecorder_IFSAMP_48000]. canDo = true; break;   /* BUG */
					case 8000: my fsamp [SoundRecorder_IFSAMP_8000]. canDo = true; break;
					case 11025: my fsamp [SoundRecorder_IFSAMP_11025]. canDo = true; break;
					case 12000: my fsamp [SoundRecorder_IFSAMP_12000]. canDo = true; break;
					case 16000: my fsamp [SoundRecorder_IFSAMP_16000]. canDo = true; break;
					case 22050: my fsamp [SoundRecorder_IFSAMP_22050]. canDo = true; break;
					case 22254: my fsamp [SoundRecorder_IFSAMP_22254]. canDo = true; break;
					case 24000: my fsamp [SoundRecorder_IFSAMP_24000]. canDo = true; break;
					case 32000: my fsamp [SoundRecorder_IFSAMP_32000]. canDo = true; break;
					case 44100: my fsamp [SoundRecorder_IFSAMP_44100]. canDo = true; break;
					case 48000: my fsamp [SoundRecorder_IFSAMP_48000]. canDo = true; break;
					case 64000: my fsamp [SoundRecorder_IFSAMP_64000]. canDo = true; break;
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
	#elif defined (sun)
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
	#elif defined (HPUX)
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
	#elif defined (_WIN32)
		(void) me;
	#elif defined (linux)
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
	#endif
	return 1;
}

static void cb_input (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	theControlPanel. inputSource = 1;   // Default.
	Melder_assert (w != NULL);
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
		if (w == my device [i]. button) {
			theControlPanel. inputSource = i;
		}
	}
	(void) call;

	/* Set system's input source. */
	#if USE_PORTAUDIO || defined (_WIN32)
	    // Deferred to the start of recording.
	#elif defined (sgi)
		my info [0] = AL_INPUT_SOURCE;
		my info [1] =
			theControlPanel. inputSource == 1 ? AL_INPUT_MIC :
			theControlPanel. inputSource == 2 ? AL_INPUT_LINE : AL_INPUT_DIGITAL;
		ALsetparams (AL_DEFAULT_DEVICE, my info, 2);
	#elif defined (macintosh)
		SPBCloseDevice (my refNum);
		if (! initialize (me)) Melder_flushError (NULL);
	#elif defined (sun)
		AUDIO_INITINFO (& my info);
		my info. record. port =
			theControlPanel. inputSource == 1 ? AUDIO_MICROPHONE : AUDIO_LINE_IN;
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#elif defined (HPUX)
		ioctl (my fd, AUDIO_SET_INPUT,
			theControlPanel. inputSource == 1 ? AUDIO_IN_MIKE : AUDIO_IN_LINE);
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

static void cb_fsamp (Widget w, XtPointer void_me, XtPointer call) {
	iam (SoundRecorder);
	if (my recording) return;
	double fsamp = NUMundefined;
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++)
		if (w == my fsamp [i]. button)
			fsamp = my fsamp [i]. fsamp;
	Melder_assert (NUMdefined (fsamp));
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
	#if USE_PORTAUDIO || defined (_WIN32)
	    // Deferred to the start of recording.
	#elif defined (sgi)
		my info [0] = AL_INPUT_RATE;
		my info [1] = (int) theControlPanel. sampleRate;
		ALsetparams (AL_DEFAULT_DEVICE, my info, 2);
	#elif defined (macintosh)
		SPBCloseDevice (my refNum);
		if (! initialize (me)) Melder_flushError (NULL);
	#elif defined (sun)
		AUDIO_INITINFO (& my info);
		my info. record. sample_rate = (int) theControlPanel. sampleRate;
		ioctl (my fd, AUDIO_SETINFO, & my info);
	#elif defined (HPUX)
		close (my fd);
		sleep (1);
		if (! initialize (me)) Melder_flushError (NULL);
	#elif defined (linux)		
		close (my fd);
		if (! initialize (me)) Melder_flushError (NULL);
	#endif
#ifdef _WIN32
end:
	iferror Melder_flushError ("Cannot change sampling frequency.");
#endif
}

MOTIF_CALLBACK (cb_resize)
	iam (SoundRecorder);
	Dimension width, height, marginWidth = 10, marginHeight = 10;
	XtVaGetValues (w, XmNwidth, & width, XmNheight, & height,
		XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	Graphics_setWsViewport (my graphics, marginWidth, width - marginWidth, marginHeight, height - marginHeight);
	width = width - marginWidth - marginWidth;
	height = height - marginHeight - marginHeight;
	Graphics_setWsWindow (my graphics, 0, width, 0, height);
	Graphics_setViewport (my graphics, 0, width, 0, height);
	Graphics_updateWs (my graphics);
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (SoundRecorder);
	Widget form = XmCreateForm (my dialog, "form", NULL, 0);
	XtVaSetValues (form,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
		NULL);

	XtVaCreateManagedWidget ("Channels:", xmLabelWidgetClass, form,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 20,
		NULL);
	Widget channels = XmCreateRadioBox (form, "channels", NULL, 0);
	XtVaSetValues (channels,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 45,
		XmNwidth, 150,
		NULL);
	my monoButton = XmCreateToggleButton (channels, "Mono", NULL, 0);
	XtManageChild (my monoButton);
	my stereoButton = XmCreateToggleButton (channels, "Stereo", NULL, 0);
	XtManageChild (my stereoButton);
	XtManageChild (channels);

	long y = 110, dy = 25;
	XtVaCreateManagedWidget ("Input source:", xmLabelWidgetClass, form,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y,
		NULL);
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
		if (my device [i]. canDo) {
			y += dy;
			my device [i]. button = XtVaCreateManagedWidget (my device [i]. name, 
			xmToggleButtonWidgetClass, form, XmNindicatorType, XmONE_OF_MANY,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y,
			NULL);
		}
	}
	#if defined (_WIN32)
		XtVaCreateManagedWidget ("(use Windows mixer", xmLabelWidgetClass, form,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y,
			NULL);
		XtVaCreateManagedWidget ("   without meters)", xmLabelWidgetClass, form,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, y + dy,
			NULL);
	#endif
	for (long i = 1; i <= SoundRecorder_IDEVICE_MAX; i ++) {
		if (my device [i]. button) {
			XtAddCallback (my device [i]. button, XmNvalueChangedCallback, cb_input, me);
			XtManageChild (my device [i]. button);
		}
	}

	XtVaCreateManagedWidget ("Meter", xmLabelWidgetClass, form,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 170,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 20,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 170,
		XmNalignment, XmALIGNMENT_CENTER,
		NULL);
	my meter = XmCreateDrawingArea (form, "meter", NULL, 0);
	XtVaSetValues (my meter,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 170,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 45,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 170,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 150,
		XmNborderWidth, 1, NULL);
	XtManageChild (my meter);

	#if defined (UNIX) && 0
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
	#endif

	XtVaCreateManagedWidget ("Sampling frequency:", xmLabelWidgetClass, form,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 10,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 20,
		XmNwidth, 150,
		NULL);
	Widget fsampBox = XmCreateRadioBox (form, "fsamp", NULL, 0);
	XtVaSetValues (fsampBox,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 10,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 45,
		XmNwidth, 150,
		NULL);
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) {
		if (my fsamp [i]. canDo) {
			double fsamp = my fsamp [i]. fsamp;
			wchar_t title [40];
			swprintf (title, 40, L"%ls Hz", fsamp == floor (fsamp) ? Melder_integer ((long) fsamp) : Melder_fixed (fsamp, 5));
			my fsamp [i]. button = XmCreateToggleButton (fsampBox, Melder_peekWcsToUtf8 (title), NULL, 0);
			XtAddCallback (my fsamp [i]. button, XmNvalueChangedCallback, cb_fsamp, me);
			XtManageChild (my fsamp [i]. button);
		}
	}
	XtManageChild (fsampBox);

	my progressScale = XmCreateScale (form, "scale", NULL, 0);
	XtVaSetValues (my progressScale, XmNorientation, XmHORIZONTAL,
		XmNminimum, 0, XmNmaximum, 1000,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 10,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 90,
		XmNwidth, 250,
		#ifdef macintosh
			XmNscaleWidth, 340,
		#endif
		NULL);
	XtManageChild (my progressScale);

	y = 60;
	my recordButton = XmCreatePushButton (form, "Record", NULL, 0);
	XtAddCallback (my recordButton, XmNactivateCallback, cb_record, (XtPointer) me);
	XtVaSetValues (my recordButton,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 20,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNwidth, 70,
		NULL);
	XtManageChild (my recordButton);
	my stopButton = XmCreatePushButton (form, "Stop", NULL, 0);
	XtAddCallback (my stopButton, XmNactivateCallback, cb_stop, (XtPointer) me);
	XtVaSetValues (my stopButton,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 100,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNwidth, 70,
		NULL);
	XtManageChild (my stopButton);
	#if defined (sgi) || defined (_WIN32) || defined (macintosh)
		my playButton = XmCreatePushButton (form, "Play", NULL, 0);
		XtAddCallback (my playButton, XmNactivateCallback, cb_play, (XtPointer) me);
		XtVaSetValues (my playButton,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 180,
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
			XmNwidth, 70,
			NULL);
		XtManageChild (my playButton);
	#endif
	XtVaCreateManagedWidget ("Name:", xmLabelWidgetClass, form,
		XmNrightAttachment, XmATTACH_FORM,
		#if defined (macintosh)
			XmNrightOffset, 100,
		#else
			XmNrightOffset, 130,
		#endif
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNalignment, XmALIGNMENT_END,
		NULL);
	my soundName = XtVaCreateManagedWidget ("name", xmTextFieldWidgetClass, form, XmNcolumns, 10,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 20,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		NULL);
	XtAddCallback (my soundName, XmNactivateCallback, cb_apply, (XtPointer) me);
	XmTextFieldSetString (my soundName, "untitled");

	y = 20;

	my cancelButton = XmCreatePushButton (form, "Close", NULL, 0);
	XtAddCallback (my cancelButton, XmNactivateCallback, cb_cancel, (XtPointer) me);
	XtVaSetValues (my cancelButton,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 280,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNwidth, 70,
		NULL);
	XtManageChild (my cancelButton);

	my applyButton = XmCreatePushButton (form, "Save to list", NULL, 0);
	XtAddCallback (my applyButton, XmNactivateCallback, cb_apply, (XtPointer) me);
	XtVaSetValues (my applyButton,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 170,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNwidth, 100,
		NULL);
	XtManageChild (my applyButton);

	my okButton = XmCreatePushButton (form, "Save to list & Close", NULL, 0);
	XtAddCallback (my okButton, XmNactivateCallback, cb_ok, (XtPointer) me);
	XtVaSetValues (my okButton,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 20,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, y,
		XmNwidth, 140,
		NULL);
	XtManageChild (my okButton);

	XtManageChild (form);
}

static int writeAudioFile (SoundRecorder me, MelderFile file, int audioFileType) {
	if (my fakeMono) {
		long nsamp = my nsamp / 2;
		MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
		if (file -> filePointer) {
			MelderFile_writeAudioFileHeader16 (file, audioFileType, theControlPanel. sampleRate, nsamp, 1);
			if (Melder_defaultAudioFileEncoding16 (audioFileType) == Melder_LINEAR_16_BIG_ENDIAN) {
				for (long i = 0; i < nsamp; i ++)
					binputi2 ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
			} else {
				for (long i = 0; i < nsamp; i ++)
					binputi2LE ((my buffer [i + i - 2] + my buffer [i + i - 1]) / 2, file -> filePointer);
			}
		}
		MelderFile_close (file);
	} else {
		MelderFile_writeAudioFile16 (file, audioFileType, my buffer, theControlPanel. sampleRate, my nsamp, my numberOfChannels);
	}
	iferror return Melder_error ("Audio file not written.");
	return 1;
}

FORM_WRITE (SoundRecorder, cb_writeWav, "Write to WAV file", 0)
	wchar_t *name = GuiText_getStringW (my soundName);
	swprintf (defaultName, 300, L"%ls.wav", name);
	Melder_free (name);
DO_WRITE
	if (! writeAudioFile (me, file, Melder_WAV)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeAifc, "Write to AIFC file", 0)
	wchar_t *name = GuiText_getStringW (my soundName);
	swprintf (defaultName, 300, L"%ls.aifc", name);
	Melder_free (name);
DO_WRITE
	if (! writeAudioFile (me, file, Melder_AIFC)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeNextSun, "Write to NeXT/Sun file", 0)
	wchar_t *name = GuiText_getStringW (my soundName);
	swprintf (defaultName, 300, L"%ls.au", name);
	Melder_free (name);
DO_WRITE
	if (! writeAudioFile (me, file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (SoundRecorder, cb_writeNist, "Write to NIST file", 0)
	wchar_t *name = GuiText_getStringW (my soundName);
	swprintf (defaultName, 300, L"%ls.nist", name);
	Melder_free (name);
DO_WRITE
	if (! writeAudioFile (me, file, Melder_NIST)) return 0;
END

DIRECT (SoundRecorder, cb_SoundRecorder_help) Melder_help (L"SoundRecorder"); END

static void createMenus (I) {
	iam (SoundRecorder);
	inherited (SoundRecorder) createMenus (me);
	Editor_addCommand (me, L"File", L"Write to WAV file...", 0, cb_writeWav);
	Editor_addCommand (me, L"File", L"Write to AIFC file...", 0, cb_writeAifc);
	Editor_addCommand (me, L"File", L"Write to NeXT/Sun file...", 0, cb_writeNextSun);
	Editor_addCommand (me, L"File", L"Write to NIST file...", 0, cb_writeNist);
	Editor_addCommand (me, L"File", L"-- write --", 0, 0);
	Editor_addCommand (me, L"Help", L"SoundRecorder help", '?', cb_SoundRecorder_help);
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
	#if USE_PORTAUDIO || defined (macintosh) || defined (_WIN32)
		my synchronous = FALSE;
	#else
		my synchronous = TRUE;
	#endif
	/*
	 * Allocate the maximum buffer.
	 */
	if (nmaxMB_pref < 1) nmaxMB_pref = 1;   /* Validate preferences. */
	if (nmaxMB_pref > 1000) nmaxMB_pref = 1000;
	if (my buffer == NULL) {
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
	#if USE_PORTAUDIO
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
				my device [++ my numberOfInputDevices]. canDo = true;
				strncpy (my device [my numberOfInputDevices]. name, deviceInfo -> name, 40);
				my device [my numberOfInputDevices]. name [40] = '\0';
				my deviceInfos [my numberOfInputDevices] = deviceInfo;
				my deviceIndices [my numberOfInputDevices] = idevice;
			}
		}
	#elif defined (macintosh)
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
						my device [++ my numberOfInputDevices]. canDo = true;
						strcpy ((char *) my hybridDeviceNames [my numberOfInputDevices], (const char *) hybridDeviceName);
						my macSource [my numberOfInputDevices] = deviceSource;
						strncpy (my device [my numberOfInputDevices]. name, plength + 1, *plength < 40 ? *plength : 40);
						my device [my numberOfInputDevices]. name [*plength < 40 ? *plength : 40] = '\0';
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
		my device [1]. canDo = true;
		strcpy (my device [1]. name, "Microphone");
		my device [2]. canDo = true;
		strcpy (my device [2]. name, "Line");
		#if defined (sgi)
			my device [3]. canDo = true;
			strcpy (my device [3]. name, "Digital");
		#endif
	#endif

	/*
	 * Sampling frequency constants.
	 */
	my fsamp [SoundRecorder_IFSAMP_8000]. fsamp = 8000.0;
	my fsamp [SoundRecorder_IFSAMP_9800]. fsamp = 9800.0;
	my fsamp [SoundRecorder_IFSAMP_11025]. fsamp = 11025.0;
	my fsamp [SoundRecorder_IFSAMP_12000]. fsamp = 12000.0;
	my fsamp [SoundRecorder_IFSAMP_16000]. fsamp = 16000.0;
	my fsamp [SoundRecorder_IFSAMP_22050]. fsamp = 22050.0;
	my fsamp [SoundRecorder_IFSAMP_22254]. fsamp = 22254.54545;
	my fsamp [SoundRecorder_IFSAMP_24000]. fsamp = 24000.0;
	my fsamp [SoundRecorder_IFSAMP_32000]. fsamp = 32000.0;
	my fsamp [SoundRecorder_IFSAMP_44100]. fsamp = 44100.0;
	my fsamp [SoundRecorder_IFSAMP_48000]. fsamp = 48000.0;
	my fsamp [SoundRecorder_IFSAMP_64000]. fsamp = 64000.0;
	my fsamp [SoundRecorder_IFSAMP_96000]. fsamp = 96000.0;

	/*
	 * The default set of possible sampling frequencies, to be modified in the initialize () procedure.
	 */
	for (long i = 1; i <= SoundRecorder_IFSAMP_MAX; i ++) my fsamp [i]. canDo = true;   // optimistic: can do all, except two:
	my fsamp [SoundRecorder_IFSAMP_9800]. canDo = false;   // sgi only
	my fsamp [SoundRecorder_IFSAMP_22254]. canDo = false;   // old Mac only

	/*
	 * Initialize system-dependent structures.
	 * On all systems: stereo 16-bit linear encoding.
	 * Some systems take initial values from the system control panel
	 * (automatically in the workProc), other systems from theControlPanel.
	 */
	if (! initialize (me)) goto error;

	if (! Editor_init (me, parent, 100, 100, 600, 500, L"SoundRecorder", NULL)) goto error;
	Melder_assert (XtWindow (my meter));
	my graphics = Graphics_create_xmdrawingarea (my meter);
	Melder_assert (my graphics);
	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, 0.0, 1.0);
	XtAddCallback (my meter, XmNresizeCallback, cb_resize, (XtPointer) me);
cb_resize (my meter, (XtPointer) me, 0);
	my workProcId = XtAppAddWorkProc (context, workProc, (XtPointer) me);
	return me;
error:
	forget (me);
	return Melder_errorp ("SoundRecorder not created.");
}

/* End of file SoundRecorder.c */
