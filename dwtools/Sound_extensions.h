#ifndef _Sound_extensions_h_
#define _Sound_extensions_h_
/* Sound_extensions.h
 *
 * Copyright (C) 1993-2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
 djmw 20121023 Latest modification
*/


#include "Sound.h"
#include "Pitch.h"
#include "Collection.h"
#include "PointProcess.h"
#include "TextGrid.h"
#include "Interpreter_decl.h"

int Sound_writeToNistAudioFile (Sound me, MelderFile file);

Sound Sound_readFromCmuAudioFile (MelderFile file);

/* only 16 bit signed has been tested !! */
Sound Sound_readFromRawFile (MelderFile file, const char *format, int nBitsCoding,
	int littleEndian, int unSigned, long skipNBytes, double samplingFrequency);
/* Reads a Sound from a raw file:
 *	fileName	: name of the file
 *	format		: "integer" (default)
 *				  "float" (this implies nBitsCoding=nBitsStorage=32, see below)
 *  nBitsCoding	: number of bits used in the coding (1..16) (default = 16 )
 *				 (the storage is assumed to be a multiple of 8 bits )
 *	littleEndian: if littleEndian != 0 then little-endian else big-endian
 *	unSigned	: if unSigned != 0 then unsigned else signed
 *	skipNBytes	: start reading after this number of bytes (skipNBytes >= 0 )
 */

Sound Sound_readFromDialogicADPCMFile (MelderFile file, double sampleRate);
/*
*/

void Sound_writeToRawFile (Sound me, MelderFile file, const char *format, int littleEndian,
	int nBitsCoding, int unSigned);

void Sound_into_Sound (Sound me, Sound to, double startTime);
/* precondition: my dx == to->dx (equal sampling times */

void Sound_overwritePart (Sound me, double t1, double t2, Sound thee, double t3);
/*
	Overwrite the part between (t1,t2) in me with samples from Sound thee,
	starting at t3 in thee.
*/

void Sound_preEmphasis (Sound me, double preEmphasisFrequency);
/* deEmphasis = exp(- 2 * NUMpi * deEmphasisFrequency * my dx); */
/* for (i=my nx; i >=2; i-- ) my z[1][i] -= preEmphasis * my z[1][i-1]; */

void Sound_deEmphasis (Sound me, double preEmphasisFrequency);
/*	for (i=2; i <= my nx; i++ ) my z[1][i] += deEmphasis * my z[1][i-1]; */

Sound Sound_createGaussian (double windowDuration, double samplingFrequency);
Sound Sound_createHamming (double windowDuration, double samplingFrequency);

Sound Sound_createSimpleToneComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, long numberOfComponents, double frequencyDistance,
	int scaleAmplitudes);

Sound Sound_createMistunedHarmonicComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, long numberOfComponents, long mistunedComponent,
	double mistuningFraction, int scaleAmplitudes);

Sound Sound_createGammaTone (double minimumTime, double maximumTime, double samplingFrequency,
	long gamma, double frequency, double bandwidth, double initialPhase, double addition,
	int scaleAmplitudes);

Sound Sound_createShepardTone (double minimumTime, double maximumTime, double samplingFrequency,
	double lowestFrequency, long numberOfComponents, double frequencyChange, double amplitudeRange);

Sound Sound_createShepardToneComplex (double minimumTime, double maximumTime,
	double samplingFrequency, double lowestFrequency, long numberOfComponents,
	double frequencyChange_st, double amplitudeRange, double octaveShiftFraction);

Sound Sound_createPattersonWightmanTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyShiftRatio, long numberOfComponents);

Sound Sound_createPlompTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyFraction, long m);

Sound Sound_createFromWindowFunction (double effectiveTime, double samplingFrequency, int windowType);
/* 1; rect 2:hamming 3: bartlet 4: welch 5: hanning 6:gaussian */

Sound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth);

void Sounds_multiply (Sound me, Sound thee);
/* precondition: my nx = thy nx */

double Sound_correlateParts (Sound me, double t1, double t2, double duration);
/*
	Correlate part (t1, t1+duration) with (t2, t2+duration)
*/

void Sound_localMean (Sound me, double fromTime, double toTime, double *mean);
void Sound_localPeak (Sound me, double fromTime, double toTime, double ref, double *peak);

Sound Sound_localAverage (Sound me, double averaginginterval, int windowType);
/* y[n] = sum(i=-n, i=n, x[n+i])/(2*n+1) */

double Sound_power (Sound me);

void Sound_scale_dB (Sound me, double level_dB);
/*
	Scales the amplitude of a Sound to a certain dB level.
	The reference value is an amplitude of 1.
	All amplitudes are multiplied by a scale factor which is
		10^(level_dB/10) / extremum,
	where extremum is the maximum of the absolute values the signal values.
*/

void Sound_fade (Sound me, int channel, double t, double fadeTime, int inout, int fadeGlobal);
/* if inout <= 0 fade in with (1-cos)/2  else fade out with (1+cos)/2
	channel = 0 (all), 1 (left), 2 (right).
*/

#define FROM_LEFT_TO_RIGHT 0
#define FROM_RIGHT_TO_LEFT 1
#define FROM_BOTTOM_TO_TOP 2
#define FROM_TOP_TO_BOTTOM 3

void Sound_draw_btlr (Sound me, Graphics g, double tmin, double tmax, double amin, double amax,
	int direction, int garnish);
/* direction is one of the macros's FROM_LEFT_TO_RIGHT... */

void Sound_drawWhere (Sound me, Graphics g, double tmin, double tmax, double minimum, double maximum,
	bool garnish, const wchar_t *method, long numberOfBisections, const wchar_t *formula, Interpreter interpreter);

void Sound_paintWhere (Sound me, Graphics g, Graphics_Colour colour, double tmin, double tmax,
	double minimum, double maximum, double level, bool garnish, long numberOfBisections, const wchar_t *formula, Interpreter interpreter);
void Sounds_paintEnclosed (Sound me, Sound thee, Graphics g, Graphics_Colour colour, double tmin, double tmax,
	double minimum, double maximum, bool garnish);

Sound Sound_changeGender (Sound me, double pitchMin, double pitchMax, double pitchRatio,
	double formantFrequenciesRatio, double durationRatio);
Sound Sound_and_Pitch_changeGender (Sound me, Pitch him, double pitchRatio,
	double formantFrequenciesRatio, double durationRatio);

Sound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio,
	double new_pitch, double pitchRangeFactor, double durationFactor);

Sound Sound_and_Pitch_changeGender_old (Sound me, Pitch him, double formantRatio,
	double new_pitch, double pitchRangeFactor, double durationFactor);

PointProcess Sound_to_PointProcess_getJumps (Sound me, double minimumJump, double dt);
/*
	Marks jumps in the signal where the amplitude changes more than 'minimumJump'
	within time dt
*/

void Sound_filter_part_formula (Sound me, double t1, double t2, const wchar_t *formula, Interpreter interpreter);

Sound Sound_changeSpeaker (Sound me, double pitchMin, double pitchMax,
	double formantMultiplier, // > 0
	double pitchMultiplier, // > 0
	double pitchRangeMultiplier, // any number
	double durationMultiplier); // > 0

Sound Sound_and_Pitch_changeSpeaker (Sound me, Pitch him,
	double formantMultiplier, // > 0
	double pitchMultiplier, // > 0
	double pitchRangeMultiplier, // any number
	double durationMultiplier); // > 0

/* Outphased */
Sound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio,
	double new_pitch, double pitchRangeFactor, double durationFactor);

TextGrid Sound_to_TextGrid_detectSilences (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration,
	const wchar_t *silentLabel, const wchar_t *soundingLabel);
void Sound_getStartAndEndTimesOfSounding (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double *t1, double *t2);

Sound Sound_and_IntervalTier_cutPartsMatchingLabel (Sound me, IntervalTier thee, const wchar_t *match);
/* Cut intervals that match the label from the sound. The starting time of the new sound is
 * (1) my xmin if the first interval is not matching
 * (2) the end time of the first interval if matching
 */

Sound Sound_trimSilencesAtStartAndEnd (Sound me, double trimDuration, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double *t1, double *t2);
Sound Sound_trimSilences (Sound me, double trimDuration, bool onlyAtStartAndEnd, double minPitch, double timeStep,
    double silenceThreshold, double minSilenceDuration, double minSoundingDuration, TextGrid *tg, const wchar_t *trimLabel);

Sound Sound_copyChannelRanges (Sound me, const wchar_t *ranges);

Sound Sound_removeNoise (Sound me, double noiseStart, double noiseEnd, double windowLength, double minBandFilterFrequency, double maxBandFilterFrequency, double smoothing, int method);

void Sound_playAsFrequencyShifted (Sound me, double shiftBy, double newSamplingFrequency, long precision);

#endif /* _Sound_extensions_h_ */
