#ifndef _SoundRecorder_h_
#define _SoundRecorder_h_
/* SoundRecorder.h
 *
 * Copyright (C) 1992-2005,2008-2013,2015-2017,2020,2022,2023 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/* An editor-like object that allows the user to record sounds. */

#include "ScriptEditor.h"
#include "Sound.h"

#include "SoundRecorder_enums.h"

#include "../external/portaudio/portaudio.h"
#if defined (_WIN32)
#elif defined (macintosh)
#elif defined (linux)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
	#if ! defined (NO_AUDIO)
		#if defined (__OpenBSD__) || defined (__NetBSD__)
			#include <soundcard.h>
		#else
			#include <sys/soundcard.h>
		#endif
	#endif
#endif

struct SoundRecorder_Device {
	char32 name [1+40];
	bool canDo;
	GuiRadioButton button;
};

struct SoundRecorder_Fsamp {
	double fsamp;
	bool canDo;
	GuiRadioButton button;
};

#define SoundRecorder_IDEVICE_MAX  20

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
#define SoundRecorder_IFSAMP_192000  14
#define SoundRecorder_IFSAMP_MAX  14

Thing_define (SoundRecorder, Editor) {
	int numberOfChannels;
	integer nsamp, nmax;
	integer firstSample, lastSample;   // for the meter
	bool synchronous, recording;
	int lastLeftMaximum, lastRightMaximum;
	integer numberOfInputDevices;
	struct SoundRecorder_Device devices [1+SoundRecorder_IDEVICE_MAX];
	struct SoundRecorder_Fsamp fsamps [1+SoundRecorder_IFSAMP_MAX];
	autovector <short> recordBuffer;
	GuiRadioButton monoButton, stereoButton;
	GuiDrawingArea meter;
	GuiScale progressScale;
	GuiButton recordButton, stopButton, playButton;
	GuiText soundName;
	GuiButton cancelButton, applyButton, okButton;
	GuiMenuItem meterIntensityButton, meterCentreOfGravityVersusIntensityButton;
	autoGraphics graphics;
	bool inputUsesPortAudio;

	const PaDeviceInfo *deviceInfos [1+SoundRecorder_IDEVICE_MAX];
	PaDeviceIndex deviceIndices [1+SoundRecorder_IDEVICE_MAX];
	PaStream *portaudioStream;

	#if cocoa
		CFRunLoopTimerRef d_cocoaTimer;
	#elif motif
		XtWorkProcId workProcId;
	#endif
	#if defined (_WIN32)
		HWAVEIN hWaveIn;
		WAVEFORMATEX waveFormat;
		WAVEHDR waveHeader [3];
		MMRESULT err;
		short buffertje1 [1000*2], buffertje2 [1000*2];
	#elif defined (macintosh)
		short macSource [1+8];
		Str255 hybridDeviceNames [1+8];
		int32 refNum;
	#elif defined (linux)
		int fd;
	#else
		int fd;
	#endif

	void v9_destroy () noexcept
		override;
	bool v_hasEditMenu ()
		override { return false; }
	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;

	#include "SoundRecorder_prefs.h"
};

autoSoundRecorder SoundRecorder_create (int numberOfChannels);
/*
	Function:
		create a SoundRecorder, which is an interactive window
		for recording in 16-bit mono or stereo.
*/

void SoundRecorder_preferences ();
integer SoundRecorder_getBufferSizePref_MB ();
void SoundRecorder_setBufferSizePref_MB (integer size);

/* End of file SoundRecorder.h */
#endif
