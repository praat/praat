#ifndef _SoundRecorder_h_
#define _SoundRecorder_h_
/* SoundRecorder.h
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

/* An editor-like object that allows the user to record sounds. */

#include "Editor.h"
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
	#if defined (__OpenBSD__) || defined (__NetBSD__)
		#include <soundcard.h>
	#else
		#include <sys/soundcard.h>
	#endif
#endif

struct SoundRecorder_Device {
	wchar_t name [1+40];
	bool canDo;
	GuiRadioButton button;
};

struct SoundRecorder_Fsamp {
	double fsamp;
	bool canDo;
	GuiRadioButton button;
};

#define SoundRecorder_IDEVICE_MAX  8

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
	// new data:
	public:
		int numberOfChannels;
		long nsamp, nmax;
		bool fakeMono, synchronous, recording;
		int lastLeftMaximum, lastRightMaximum;
		long numberOfInputDevices;
		struct SoundRecorder_Device device_ [1+SoundRecorder_IDEVICE_MAX];
		struct SoundRecorder_Fsamp fsamp_ [1+SoundRecorder_IFSAMP_MAX];
		short *buffer;
		GuiRadioButton monoButton, stereoButton;
		GuiDrawingArea meter;
		GuiScale progressScale;
		GuiButton recordButton, stopButton, playButton;
		GuiText soundName;
		GuiButton cancelButton, applyButton, okButton;
		GuiMenuItem d_meterIntensityButton, d_meterCentreOfGravityVersusIntensityButton;
		Graphics graphics;
		bool inputUsesPortAudio;
		const PaDeviceInfo *deviceInfos [1+SoundRecorder_IDEVICE_MAX];
		PaDeviceIndex deviceIndices [1+SoundRecorder_IDEVICE_MAX];
		PaStream *portaudioStream;
		#if motif
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
			long refNum;
		#elif defined (linux)
			int fd;
		#else
			int fd;
		#endif
	// overridden methods:
		virtual void v_destroy ();
		virtual bool v_editable () { return false; }
		virtual bool v_scriptable () { return false; }
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
	// preferences:
		#include "SoundRecorder_prefs.h"
};

SoundRecorder SoundRecorder_create (int numberOfChannels);
/*
	Function:
		create a SoundRecorder, which is an interactive window
		for recording in 16-bit mono or stereo.
*/

void SoundRecorder_preferences ();
int SoundRecorder_getBufferSizePref_MB ();
void SoundRecorder_setBufferSizePref_MB (int size);

/* End of file SoundRecorder.h */
#endif
