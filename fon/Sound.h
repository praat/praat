#ifndef _Sound_h_
#define _Sound_h_
/* Sound.h
 *
 * Copyright (C) 1992-2005,2006-2008,2010-2019 Paul Boersma
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

/* Sound inherits from Vector */
/* A Sound is a sampled signal, not quantized. */
#include "Vector.h"
#include "Collection.h"

#include "Sound_enums.h"

Thing_define (Sound, Vector) {
	void v_info ()
		override;
	bool v_hasGetMatrix ()
		override { return true; }
	double v_getMatrix (integer irow, integer icol)
		override;
	bool v_hasGetFunction2 ()
		override { return true; }
	double v_getFunction2 (double x, double y)
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

/* Attributes:
	xmin              // Start time (seconds).
	xmax              // End time (seconds).
	nx                // Number of samples.
	dx                // Sampling period (seconds).
	x1                // Time of first sample (seconds).
	ymin == 1         // Left or only channel.
	ymax              // Right or only channels.
	ny                // Number of channels.
	dy == 1; y1 == 1  // y is channel number (1 = left or mono; 2 = right).
	z [i] [...]       // Amplitude.
	z may be replaced (e.g., in pasting).
*/

autoSound Sound_create (integer numberOfChannels, double xmin, double xmax, integer nx, double dx, double x1);
/*
	Function:
		return a new silent Sound.
	Preconditions:
		xmax > xmin;
		nx >= 1;
		dx > 0.0;
	Postconditions:
		thy xmin == xmin;
		thy xmax == xmax;
		thy nx == nx;
		thy dx == dx;
		thy x1 == x1;
		thy ymin = 1.0;
		thy ymax = numberOfChannels;
		thy ny = numberOfChannels;
		thy dx = 1.0;
		thy x1 = 1.0;
		thy z [i] [1..nx] == 0.0;
*/

autoSound Sound_createSimple (integer numberOfChannels, double duration, double samplingFrequency);
/*
	Function:
		return a new silent Sound.
	Preconditions:
		duration > 0.0;
		samplingFrequency > 0.0;
	Postconditions:
		thy xmin == 0.0;
		thy xmax == duration;
		thy nx == Melder_iround (duration * samplingFrequency);
		thy dx == 1 / samplingFrequency;
		thy x1 == 0.5 * thy dx;		// Centre of first sampling period.
		thy ymin = 1.0;
		thy ymax = numberOfChannels;
		thy ny = numberOfChannels;
		thy dx = 1.0;
		thy x1 = 1.0;
		thy z [i] [1..nx] == 0.0;
*/

autoSound Sound_convertToMono (Sound me);
autoSound Sound_convertToStereo (Sound me);
autoSound Sound_extractChannel (Sound me, integer ichannel);
autoSound Sound_extractChannels (Sound me, constVECVU const& channelNumbers);
autoSound Sounds_combineToStereo (OrderedOf<structSound>* me);

/* Levels for Sampled_getValueAtSample (me, index, level, unit) */
#define Sound_LEVEL_MONO  0
#define Sound_LEVEL_LEFT  1
#define Sound_LEVEL_RIGHT  2

autoSound Sound_upsample (Sound me);   /* By a factor 2. */

autoSound Sound_resample (Sound me, double samplingFrequency, integer precision);
/*
	Method:
		precision <= 1: linear interpolation.
		precision >= 2: sinx/x interpolation with maximum depth equal to 'precision'.
*/

autoSound Sounds_append (Sound me, double silenceDuration, Sound thee);
/*
	Function:
		append two Sounds.
	Failure:
		my dx != thy dx: "Sampling frequencies do not match."
	Postconditions:
		result -> xmin == 0;
		result -> xmax == result -> nx * my dx;
		result -> nx == my nx + thy nx + Melder_iround (silenceDuration / my dx);
		result -> dx == my dx;
		result -> x1 == 0.5 * my dx;
		for (i = 1..my nx)
			result -> z [1] [i] == my z [1] [i]
		for (i = 1..thy nx)
		result -> z [1] [i + my nx + Melder_iround (silenceDuration / my dx)] == thy z [1] [i]
*/
 
autoSound Sounds_convolve (Sound me, Sound thee, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);
/*
	Function:
		convolve two Sounds.
	Failure:
		my dx != thy dx: "Sampling frequencies do not match."
	Postconditions:
		result -> xmin == my xmin + thy xmin;
		result -> xmax == my xmax + thy xmax;
		result -> nx == my nx + thy nx - 1;
		result -> dx == my dx;
		result -> x1 == my x1 + thy x1;
		for (i = 1..result -> nx)
			result -> z [1] [i] == result -> dx *
				sum (j = 1..i, my z [1] [j] * thy z [1] [i - j + 1])
*/
autoSound Sounds_crossCorrelate (Sound me, Sound thee, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);
autoSound Sounds_crossCorrelate_short (Sound me, Sound thee, double tmin, double tmax, bool normalize);
autoSound Sound_autoCorrelate (Sound me, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);

double Sound_getRootMeanSquare (Sound me, double xmin, double xmax);
double Sound_getEnergy (Sound me, double xmin, double xmax);
double Sound_getPower (Sound me, double xmin, double xmax);
double Sound_getEnergyInAir (Sound me);
double Sound_getPowerInAir (Sound me);
double Sound_getIntensity_dB (Sound me);

double Sound_getNearestZeroCrossing (Sound me, double position, integer ichannel);
void Sound_setZero (Sound me, double tmin, double tmax, bool roundTimesToNearestZeroCrossing);

autoSound Sound_createAsPureTone (integer numberOfChannels, double startingTime, double endTime,
	double sampleRate, double frequency, double amplitude, double fadeInDuration, double fadeOutDuration);
autoSound Sound_createAsToneComplex (double startingTime, double endTime,
	double sampleRate, int phase, double frequencyStep,
	double firstFrequency, double ceiling, integer numberOfComponents);
/* Values for `phase' parameter: */
#define Sound_TONE_COMPLEX_SINE  0
#define Sound_TONE_COMPLEX_COSINE  1

autoSound Sounds_concatenate (OrderedOf<structSound>& list, double overlapTime);
void Sound_multiplyByWindow (Sound me, kSound_windowShape windowShape);
void Sound_scaleIntensity (Sound me, double newAverageIntensity);
void Sound_overrideSamplingFrequency (Sound me, double newSamplingFrequency);
autoSound Sound_extractPart (Sound me, double t1, double t2, kSound_windowShape windowShape, double relativeWidth, bool preserveTimes);
autoSound Sound_extractPartForOverlap (Sound me, double t1, double t2, double overlap);
void Sound_filterWithFormants (Sound me, double tmin, double tmax,
	int numberOfFormants, double formant [], double bandwidth []);
autoSound Sound_filter_oneFormant (Sound me, double frequency, double bandwidth);
void Sound_filterWithOneFormantInplace (Sound me, double frequency, double bandwidth);
autoSound Sound_filter_preemphasis (Sound me, double frequency);
autoSound Sound_filter_deemphasis (Sound me, double frequency);

void Sound_reverse (Sound me, double tmin, double tmax);

void Sound_draw (Sound me, Graphics g,
	double tmin, double tmax, double minimum, double maximum, bool garnish, conststring32 method);
/* For method, see Vector_draw. */

autoMatrix Sound_to_Matrix (Sound me);
/*
	Create a Matrix from a Sound,
	with deep copy of all its Matrix attributes, except class information and methods.
*/

autoSound Matrix_to_Sound (Matrix me);

autoSound Matrix_to_Sound_mono (Matrix me, integer row);
/*
	Function:
		create a Sound from one row of a Matrix.
		"row" is forced inside the segment [1, my ny];
		negative values count from the last row;
		e.g., if "row" is -1, the last row is taken.
	Postconditions:
		thy xmin == my xmin;
		thy xmax == my xmax;
		thy nx == my nx;
		thy dx == my dx;
		thy x1 == my x1;
		thy ymin ymax ny dy y1 == 1;
		thy z [1] [...] == my z [row] [...];
*/

extern autoSound Sound_clipboard;

/********** Sound_audio.cpp **********/

autoSound Sound_record_fixedTime (int inputSource,
	double gain, double balance, double samplingFrequency, double duration);
	/*
		Function:
			record a sound without user interaction, synchronously (i.e., return when recording finsishes).
		Arguments:
			inputSource:
				<=0 = do not change (may be set by Audio Control Panel).
				1 = microphone.
				2 = line.
				3 = digital.
				If the input source selection fails, the microphone is used.
			gain:
				<0.0 = do not change.
				0.0 = minimum.
				1.0 = maximum.
			balance:
				<0.0 = do not change.
				0.0 = left channel only.
				0.5 = mid balance.
				1.0 = right channel only.
			samplingFrequency:
				<=0.0 = do not change.
				must be one of the sampling frequencies supported by the hardware.
			duration:
				positive time in seconds.
		Return value:
			the resulting Sound.
		Failures:
			Hardware does not support sampling frequency.
			Duration must be positive.
			Duration too long (out of memory).
		Usage:
			batch; in an interactive environment, it is better to use SoundRecorder_create,
			and wait for broadcastPublication().
	*/

typedef MelderCallback <int, structThing /* boss */, int /* phase */, double /* tmin */, double /* tmax */, double /* t */> Sound_PlayCallback;

void Sound_playPart (Sound me, double tmin, double tmax, Sound_PlayCallback playCallback, Thing playBoss);
/*
 * Play a sound. The playing can be interrupted with the Escape key (also Command-period on the Mac).
 * If playCallback is not null, Sound_play will call it repeatedly, with five parameters:
 *    1. playBoss: the same value as was supplied as the last argument to Sound_playPart.
 *    2. phase: 1 at the start, 2 while playing, 3 at the end.
 *    3. tmin: the same tmin that was supplied as the second argument to Sound_playPart.
 *    4. tmax: the same tmax that was supplied as the second argument to Sound_playPart.
 *    5. t: the time (probably between tmin and tmax) at which the sound is playing.
 * The usage of playCallback is as follows. Suppose we are an editor that wants to show a moving
 * cursor while we play a sound:
 *    Sound_playPart (my sound, my startSelection, my endSelection, thePlayCallback, me);
 * We gave ourselves as the playClosure, so that the message will arrive with us:
 *    int thePlayCallback (SoundEditor me, int phase, double tmin, double tmax, double t) {
 *       if (phase == 1) {
 *          Melder_assert (t == tmin);
 *          drawPlayCursor (me, my playCursor = t);
 *       } else if (phase == 2) {
 *          undrawPlayCursor (me, my playCursor);
 *          drawPlayCursor (me, my playCursor = t);
 *       } else {
 *          undrawPlayCursor (me, t);
 *          moveCursor (me, t);
 *          if (t < tmax) { Melder_casual (U"Sound play interrupted."); }
 *       }
 *       return 1;
 *    }
 * The playCallback procedure usually returns 1, because the sound should continue playing.
 * If playCallback returns 0 instead, Sound_play will interrupt the play and return.
 * Sound_playPart returns the time at which playing stopped. If playing was not interrupted, it returns tmax.
 *
 * Sound_playPart () usually runs asynchronously, and kills an already playing sound.
 */
void Sound_play (Sound me, Sound_PlayCallback playCallback, Thing playBoss);
	/* The same as Sound_playPart (me, my xmin, my xmax, playCallback, playBoss); */

/********** Sound_files.cpp **********/

/* To avoid clipping, keep the absolute amplitude below 1.000. */
/* All are mono or stereo PCM. */
void Sound_saveAsAudioFile (Sound me, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint);
void Sound_saveAsKayFile (Sound me, MelderFile file);   // 16-bit
void Sound_saveAsSesamFile (Sound me, MelderFile file);   // 12-bit SESAM/LVS

autoSound Sound_readFromSoundFile (MelderFile file);   // AIFF, WAV, NeXT/Sun, or NIST
autoSound Sound_readFromKayFile (MelderFile file);   // 16-bit
autoSound Sound_readFromSesamFile (MelderFile file);   // 12-bit SESAM/LVS
autoSound Sound_readFromBellLabsFile (MelderFile file);   // 16-bit
autoSound Sound_readFromRawAlawFile (MelderFile file);
autoSound Sound_readFromMovieFile (MelderFile file);

autoSound Sound_readFromRawSoundFile (MelderFile file, int encoding, int numberOfChannels, double sampleRate);
/*
	'encoding' is any of the following:
		Melder_LINEAR_8_SIGNED
		Melder_LINEAR_8_UNSIGNED
		Melder_LINEAR_16_BIG_ENDIAN
		Melder_LINEAR_16_LITTLE_ENDIAN
		Melder_MULAW
		Melder_ALAW
	'numberOfChannels' is 1 (mono) or 2 (stereo)
	'sampleRate' is in hertz
*/
void Sound_saveAsRawSoundFile (Sound me, MelderFile file, int encoding);
/*
	'encoding' is any of the following:
		Melder_LINEAR_8_SIGNED
		Melder_LINEAR_8_UNSIGNED
		Melder_LINEAR_16_BIG_ENDIAN
		Melder_LINEAR_16_LITTLE_ENDIAN
	'me' must exist
*/

/********** Sound_enhance.cpp **********/

autoSound Sound_lengthen_overlapAdd (Sound me, double fmin, double fmax, double factor);
autoSound Sound_deepenBandModulation (Sound me, double enhancement_dB,
	double flow, double fhigh, double slowModulation, double fastModulation, double bandSmoothing);

/*
	Abstract.
*/
Collection_define (SoundList, OrderedOf, Sound) {
};

/* End of file Sound.h */
#endif
