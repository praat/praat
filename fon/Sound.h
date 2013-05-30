#ifndef _Sound_h_
#define _Sound_h_
/* Sound.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

/* Sound inherits from Vector */
/* A Sound is a sampled signal, not quantized. */
#include "Vector.h"
#include "Collection.h"

#include "Sound_enums.h"

Thing_define (Sound, Vector) {
	// overridden methods:
		virtual void v_info ();
		virtual bool v_hasGetMatrix    () { return true; }   virtual double v_getMatrix (long irow, long icol);
		virtual bool v_hasGetFunction2 () { return true; }   virtual double v_getFunction2 (double x, double y);
		virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
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

Sound Sound_create (long numberOfChannels, double xmin, double xmax, long nx, double dx, double x1);
/*
	Function:
		return a new silent Sound, or NULL if out of memory.
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

Sound Sound_createSimple (long numberOfChannels, double duration, double samplingFrequency);
/*
	Function:
		return a new silent Sound, or NULL if out of memory.
	Preconditions:
		duration > 0.0;
		samplingFrequency > 0.0;
	Postconditions:
		thy xmin == 0.0;
		thy xmax == duration;
		thy nx == round (duration * samplingFrequency);
		thy dx == 1 / samplingFrequency;
		thy x1 == 0.5 * thy dx;		// Centre of first sampling period.
		thy ymin = 1.0;
		thy ymax = numberOfChannels;
		thy ny = numberOfChannels;
		thy dx = 1.0;
		thy x1 = 1.0;
		thy z [i] [1..nx] == 0.0;
*/

Sound Sound_convertToMono (Sound me);
Sound Sound_convertToStereo (Sound me);
Sound Sound_extractChannel (Sound me, long ichannel);
Sound Sounds_combineToStereo (Collection me);

/* Levels for Sampled_getValueAtSample (me, index, level, unit) */
#define Sound_LEVEL_MONO  0
#define Sound_LEVEL_LEFT  1
#define Sound_LEVEL_RIGHT  2

Sound Sound_upsample (Sound me);   /* By a factor 2. */

Sound Sound_resample (Sound me, double samplingFrequency, long precision);
/*
	Method:
		precision <= 1: linear interpolation.
		precision >= 2: sinx/x interpolation with maximum depth equal to 'precision'.
*/

Sound Sounds_append (Sound me, double silenceDuration, Sound thee);
/*
	Function:
		append two Sounds.
	Failure:
		my dx != thy dx: "Sampling frequencies do not match."
	Postconditions:
		result -> xmin == 0;
		result -> xmax == result -> nx * my dx;
		result -> nx == my nx + thy nx + round (silenceDuration / my dx);
		result -> dx == my dx;
		result -> x1 == 0.5 * my dx;
		for (i = 1..my nx)
			result -> z [1] [i] == my z [1] [i]
		for (i = 1..thy nx)
		result -> z [1] [i + my nx + round (silenceDuration / my dx)] == thy z [1] [i]
*/
 
Sound Sounds_convolve (Sound me, Sound thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);
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
Sound Sounds_crossCorrelate (Sound me, Sound thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);
Sound Sounds_crossCorrelate_short (Sound me, Sound thee, double tmin, double tmax, int normalize);
Sound Sound_autoCorrelate (Sound me, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);

double Sound_getRootMeanSquare (Sound me, double xmin, double xmax);
double Sound_getEnergy (Sound me, double xmin, double xmax);
double Sound_getPower (Sound me, double xmin, double xmax);
double Sound_getEnergyInAir (Sound me);
double Sound_getPowerInAir (Sound me);
double Sound_getIntensity_dB (Sound me);

double Sound_getNearestZeroCrossing (Sound me, double position, long ichannel);
void Sound_setZero (Sound me, double tmin, double tmax, int roundTimesToNearestZeroCrossing);

Sound Sound_createAsPureTone (long numberOfChannels, double startingTime, double endTime,
	double sampleRate, double frequency, double amplitude, double fadeInDuration, double fadeOutDuration);
Sound Sound_createFromToneComplex (double startingTime, double endTime,
	double sampleRate, int phase, double frequencyStep,
	double firstFrequency, double ceiling, long numberOfComponents);
/* Values for `phase' parameter: */
#define Sound_TONE_COMPLEX_SINE  0
#define Sound_TONE_COMPLEX_COSINE  1

Sound Sounds_concatenate_e (Collection me, double overlapTime);
void Sound_multiplyByWindow (Sound me, enum kSound_windowShape windowShape);
void Sound_scaleIntensity (Sound me, double newAverageIntensity);
void Sound_overrideSamplingFrequency (Sound me, double newSamplingFrequency);
Sound Sound_extractPart (Sound me, double t1, double t2, enum kSound_windowShape windowShape, double relativeWidth, bool preserveTimes);
Sound Sound_extractPartForOverlap (Sound me, double t1, double t2, double overlap);
void Sound_filterWithFormants (Sound me, double tmin, double tmax,
	int numberOfFormants, double formant [], double bandwidth []);
Sound Sound_filter_oneFormant (Sound me, double frequency, double bandwidth);
void Sound_filterWithOneFormantInline (Sound me, double frequency, double bandwidth);
Sound Sound_filter_preemphasis (Sound me, double frequency);
Sound Sound_filter_deemphasis (Sound me, double frequency);

void Sound_reverse (Sound me, double tmin, double tmax);

void Sound_draw (Sound me, Graphics g,
	double tmin, double tmax, double minimum, double maximum, bool garnish, const wchar_t *method);
/* For method, see Vector_draw. */

Matrix Sound_to_Matrix (Sound me);
/*
	Create a Matrix from a Sound,
	with deep copy of all its Matrix attributes, except class information and methods.
	Return NULL if out of memory.  
*/

Sound Matrix_to_Sound (Matrix me);

Sound Matrix_to_Sound_mono (Matrix me, long row);
/*
	Function:
		create a Sound from one row of a Matrix.
		"row" is forced inside the segment [1, my ny];
		negative values count from the last row;
		e.g., if "row" is -1, the last row is taken.
		Return NULL if out of memory.
	Postconditions:
		thy xmin == my xmin;
		thy xmax == my xmax;
		thy nx == my nx;
		thy dx == my dx;
		thy x1 == my x1;
		thy ymin ymax ny dy y1 == 1;
		thy z [1] [...] == my z [row] [...];
*/

extern Sound Sound_clipboard;

/********** Sound_audio.c **********/

Sound Sound_recordFixedTime (int inputSource,
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
				must be one of the sample rates supported by the hardware.
			duration:
				positive time in seconds.
		Return value:
			the resulting Sound; NULL in case of failure.
		Failures:
			Hardware does not support sampling frequency.
			Duration must be positive.
			Duration too long (out of memory).
		Usage:
			batch; in an interactive environment, it is better to use SoundRecorder_create,
			and wait for broadcastPublication().
	*/

void Sound_playPart (Sound me, double tmin, double tmax,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure);
/*
 * Play a sound. The playing can be interrupted with the Escape key (also Command-period on the Mac).
 * If playCallback is not NULL, Sound_play will call it repeatedly, with five parameters:
 *    1. playClosure: the same value as was supplied as the last argument to Sound_playPart.
 *    2. phase: 1 at the start, 2 while playing, 3 at the end.
 *    3. tmin: the same tmin that was supplied as the second argument to Sound_playPart.
 *    4. tmax: the same tmax that was supplied as the second argument to Sound_playPart.
 *    5. t: the time (probably between tmin and tmax) at which the sound is playing.
 * The usage of playCallback is as follows. Suppose we are an editor that wants to show a moving
 * cursor while we play a sound:
 *    Sound_playPart (my sound, my startSelection, my endSelection, thePlayCallback, me);
 * We gave ourselves as the playClosure, so that the message will arrive with us:
 *    int thePlayCallback (void *playClosure, int phase, double tmin, double tmax, double t) {
 *       SoundEditor me = (SoundEditor) playClosure;
 *       if (phase == 1) {
 *          Melder_assert (t == tmin);
 *          drawPlayCursor (me, my playCursor = t);
 *       } else if (phase == 2) {
 *          undrawPlayCursor (me, my playCursor);
 *          drawPlayCursor (me, my playCursor = t);
 *       } else {
 *          undrawPlayCursor (me, t);
 *          moveCursor (me, t);
 *          if (t < tmax) { Melder_casual ("Sound play interrupted."); }
 *       }
 *       return 1;
 *    }
 * The playCallback procedure usually returns 1, because the sound should continue playing.
 * If playCallback returns 0 instead, Sound_play will interrupt the play and return.
 * Sound_playPart returns the time at which playing stopped. If playing was not interrupted, it returns tmax.
 *
 * Sound_playPart () usually runs asynchronously, and kills an already playing sound.
 */
void Sound_play (Sound me,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure);
	/* The same as Sound_playPart (me, my xmin, my xmax, playCallback, playClosure); */

/********** Sound_files.c **********/

/* To avoid clipping, keep the absolute amplitude below 1.000. */
/* All are mono or stereo PCM. */
void Sound_writeToAudioFile (Sound me, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint);
void Sound_writeToKayFile (Sound me, MelderFile file);   /* 16 bit */
void Sound_writeToSesamFile (Sound me, MelderFile file);   /* 12-bit SESAM/LVS */

Sound Sound_readFromSoundFile (MelderFile file);   /* AIFF, WAV, NeXT/Sun, or NIST */
Sound Sound_readFromKayFile (MelderFile file);   /* 16 bit */
Sound Sound_readFromSesamFile (MelderFile file);   /* 12 bit SESAM/LVS */
Sound Sound_readFromBellLabsFile (MelderFile file);   /* 16 bit */
Sound Sound_readFromRawAlawFile (MelderFile file);
Sound Sound_readFromMovieFile (MelderFile file);

Sound Sound_readFromRawSoundFile (MelderFile file, int encoding, int numberOfChannels, double sampleRate);
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
void Sound_writeToRawSoundFile (Sound me, MelderFile file, int encoding);
/*
	'encoding' is any of the following:
		Melder_LINEAR_8_SIGNED
		Melder_LINEAR_8_UNSIGNED
		Melder_LINEAR_16_BIG_ENDIAN
		Melder_LINEAR_16_LITTLE_ENDIAN
	'me' must exist
*/

/********** Sound_enhance.c **********/

Sound Sound_lengthen_overlapAdd (Sound me, double fmin, double fmax, double factor);
Sound Sound_deepenBandModulation (Sound me, double enhancement_dB,
	double flow, double fhigh, double slowModulation, double fastModulation, double bandSmoothing);

/* End of file Sound.h */
#endif
