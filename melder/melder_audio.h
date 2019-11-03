#ifndef _melder_audio_h_
#define _melder_audio_h_
/* melder_audio.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

void MelderAudio_setInputSoundSystem (enum kMelder_inputSoundSystem inputSoundSystem);
enum kMelder_inputSoundSystem MelderAudio_getInputSoundSystem ();

void MelderAudio_setOutputSoundSystem (enum kMelder_outputSoundSystem outputSoundSystem);
enum kMelder_outputSoundSystem MelderAudio_getOutputSoundSystem ();

#if defined (_WIN32)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.1
		// in order to get rid of the click on some cards
#elif defined (macintosh)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
		// in order to switch off the BOING caused by the automatic gain control
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.0
		// in order to reduce the BOING caused by the automatic gain control when the user replays immediately after a sound has finished
#elif defined (linux)
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.1
		// in order to get rid of double playing of a sounding buffer (?)
#else
	#define kMelderAudio_outputSilenceBefore_DEFAULT  0.0
	#define kMelderAudio_outputSilenceAfter_DEFAULT  0.0
#endif
void MelderAudio_setOutputSilenceBefore (double silenceBefore);
double MelderAudio_getOutputSilenceBefore ();
void MelderAudio_setOutputSilenceAfter (double silenceAfter);
double MelderAudio_getOutputSilenceAfter ();

void MelderAudio_setUseInternalSpeaker (bool useInternalSpeaker);   // for HP-UX and Sun
bool MelderAudio_getUseInternalSpeaker ();
integer MelderAudio_getOutputBestSampleRate (integer fsamp);

extern bool MelderAudio_isPlaying;
extern bool MelderAudio_hasBeenInitialized;
void MelderAudio_play16 (int16 *buffer, integer sampleRate, integer numberOfSamples, integer numberOfChannels,
	bool (*playCallback) (void *playClosure, integer numberOfSamplesPlayed),   // return true to continue, false to stop
	void *playClosure);
bool MelderAudio_stopPlaying (bool isExplicit);   // returns true if sound was playing
#define MelderAudio_IMPLICIT  false
#define MelderAudio_EXPLICIT  true
integer MelderAudio_getSamplesPlayed ();
bool MelderAudio_stopWasExplicit ();

void Melder_audio_prefs ();   // in init file

#pragma mark - ASYNCHRONICITY

void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity);
enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity ();

class autoMelderAudioSaveMaximumAsynchronicity {
	bool _disowned;
	enum kMelder_asynchronicityLevel _savedAsynchronicity;
public:
	autoMelderAudioSaveMaximumAsynchronicity () {
		our _savedAsynchronicity = MelderAudio_getOutputMaximumAsynchronicity ();
		trace (U"value was ", (int) our _savedAsynchronicity);
		our _disowned = false;
	}
	~autoMelderAudioSaveMaximumAsynchronicity () {
		MelderAudio_setOutputMaximumAsynchronicity (our _savedAsynchronicity);
		trace (U"value set to ", (int) our _savedAsynchronicity);
	}
	/*
		Disable copying.
	*/
	autoMelderAudioSaveMaximumAsynchronicity (const autoMelderAudioSaveMaximumAsynchronicity&) = delete;   // disable copy constructor
	autoMelderAudioSaveMaximumAsynchronicity& operator= (const autoMelderAudioSaveMaximumAsynchronicity&) = delete;   // disable copy assignment
	/*
		Enable moving.
	*/
	autoMelderAudioSaveMaximumAsynchronicity (autoMelderAudioSaveMaximumAsynchronicity&& other) noexcept {   // enable move constructor
		our _disowned = other._disowned;
		our _savedAsynchronicity = other._savedAsynchronicity;
		other._disowned = true;
	}
	autoMelderAudioSaveMaximumAsynchronicity& operator= (autoMelderAudioSaveMaximumAsynchronicity&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our _disowned = other._disowned;
			our _savedAsynchronicity = other._savedAsynchronicity;
			other._disowned = true;   // needed only if you insist on keeping the source in a valid state
		}
		return *this;
	}
	autoMelderAudioSaveMaximumAsynchronicity&& move () noexcept { return static_cast <autoMelderAudioSaveMaximumAsynchronicity&&> (*this); }
	void releaseToAmbiguousOwner () {
		our _disowned = true;
	}
};

/*
	True if specified by the "asynchronous" directive in a script.
	TODO: change to inline variable once C++17 is implemented completely on all platforms.
*/
extern bool Melder_asynchronous;

class autoMelderAsynchronous {
	bool _disowned;
	bool _savedAsynchronicity;
public:
	autoMelderAsynchronous () {
		our _savedAsynchronicity = Melder_asynchronous;
		Melder_asynchronous = true;
		our _disowned = false;
	}
	~autoMelderAsynchronous () {
		if (! _disowned) {
			Melder_asynchronous = _savedAsynchronicity;
		}
	}
	/*
		Disable copying.
	*/
	autoMelderAsynchronous (const autoMelderAsynchronous&) = delete;   // disable copy constructor
	autoMelderAsynchronous& operator= (const autoMelderAsynchronous&) = delete;   // disable copy assignment
	/*
		Enable moving.
	*/
	autoMelderAsynchronous (autoMelderAsynchronous&& other) noexcept {   // enable move constructor
		our _disowned = other._disowned;
		our _savedAsynchronicity = other._savedAsynchronicity;
		other._disowned = true;
	}
	autoMelderAsynchronous& operator= (autoMelderAsynchronous&& other) noexcept {   // enable move assignment
		if (& other != this) {
			our _disowned = other._disowned;
			our _savedAsynchronicity = other._savedAsynchronicity;
			other._disowned = true;   // needed only if you insist on keeping the source in a valid state
		}
		return *this;
	}
	autoMelderAsynchronous&& move () noexcept { return static_cast <autoMelderAsynchronous&&> (*this); }
	void releaseToAmbiguousOwner () {
		our _disowned = true;
	}
};

/* End of file melder_audio.h */
#endif
