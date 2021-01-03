#ifndef _Sound_extensions_h_
#define _Sound_extensions_h_
/* Sound_extensions.h
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Sound.h"
#include "Pitch.h"
#include "Collection.h"
#include "PointProcess.h"
#include "TextGrid.h"
#include "Sound_extensions_enums.h"

Thing_declare (Interpreter);

int Sound_writeToNistAudioFile (Sound me, MelderFile file);

autoSound Sound_readFromCmuAudioFile (MelderFile file);

/* only 16 bit signed has been tested !! */
autoSound Sound_readFromRawFile (MelderFile file, const char *format, int nBitsCoding,
	bool littleEndian, bool unSigned, integer skipNBytes, double samplingFrequency);
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

autoSound Sound_readFromDialogicADPCMFile (MelderFile file, double sampleRate);
/*
*/

autoSound Sound_readFromOggVorbisFile (MelderFile file);
autoSound Sound_readFromOggOpusFile (MelderFile file);

void Sound_writeToRawFile (Sound me, MelderFile file, const char *format, bool littleEndian, int nBitsCoding, bool unSigned);

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

autoSound Sound_createGaussian (double windowDuration, double samplingFrequency);

autoSound Sound_createHamming (double windowDuration, double samplingFrequency);

autoSound Sound_createSimpleToneComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, integer numberOfComponents, double frequencyDistance, bool scaleAmplitudes);

autoSound Sound_createMistunedHarmonicComplex (double minimumTime, double maximumTime, double samplingFrequency,
	double firstFrequency, integer numberOfComponents, integer mistunedComponent, double mistuningFraction, bool scaleAmplitudes);

autoSound Sound_createGammaTone (double minimumTime, double maximumTime, double samplingFrequency,
	double gamma, double frequency, double bandwidth, double initialPhase, double addition, bool scaleAmplitudes);

autoSound Sound_createShepardTone (double minimumTime, double maximumTime, double samplingFrequency,
	double lowestFrequency, integer numberOfComponents, double frequencyChange, double amplitudeRange);

autoSound Sound_createShepardToneComplex (double minimumTime, double maximumTime,
	double samplingFrequency, double lowestFrequency, integer numberOfComponents,
	double frequencyChange_st, double amplitudeRange, double octaveShiftFraction);

autoSound Sound_createPattersonWightmanTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyShiftRatio, integer numberOfComponents);

autoSound Sound_createPlompTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyFraction, integer m);

autoSound Sound_createFromWindowFunction (double effectiveTime, double samplingFrequency, int windowType);
/* 1; rect 2:hamming 3: bartlet 4: welch 5: hanning 6:gaussian */

autoSound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth);

autoSound Sound_filterByGammaToneFilter (Sound me, double centre_frequency, double bandwidth, double gamma, double initialPhase);

void Sounds_multiply (Sound me, Sound thee);
/* precondition: my nx == thy nx */

double Sound_correlateParts (Sound me, double t1, double t2, double duration);
/*
	Correlate part (t1, t1+duration) with (t2, t2+duration)
*/

double Sound_localMean (Sound me, double fromTime, double toTime);
double Sound_localPeak (Sound me, double fromTime, double toTime, double reference);

double Sound_getNearestLevelCrossing (Sound me, integer channel, double position, double level, kSoundSearchDirection searchDirection);

autoSound Sound_localAverage (Sound me, double averaginginterval, int windowType);
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

void Sound_fade (Sound me, int channel, double t, double fadeTime, bool fadeOut, bool fadeGlobal);
/* if inout <= 0 fade in with (1-cos)/2  else fade out with (1+cos)/2
	channel = 0 (all), 1 (left), 2 (right).
*/

void Sound_draw_btlr (Sound me, Graphics g, double tmin, double tmax, double amin, double amax, kSoundDrawingDirection drawingDirection, bool garnish);
/* direction is one of the macros's FROM_LEFT_TO_RIGHT... */

void Sound_drawWhere (Sound me, Graphics g, double tmin, double tmax, double minimum, double maximum,
	bool garnish, conststring32 method, integer numberOfBisections, conststring32 formula, Interpreter interpreter);

void Sound_paintWhere (Sound me, Graphics g, MelderColour colour, double tmin, double tmax,
	double minimum, double maximum, double level, bool garnish,
	integer numberOfBisections, conststring32 formula, Interpreter interpreter
);

void Sounds_paintEnclosed (Sound me, Sound thee, Graphics g, MelderColour colour, double tmin, double tmax,
	double minimum, double maximum, bool garnish);

autoSound Sound_changeGender (Sound me, double pitchMin, double pitchMax, double pitchRatio, double formantFrequenciesRatio, double durationRatio);

autoSound Sound_Pitch_changeGender (Sound me, Pitch him, double pitchRatio, double formantFrequenciesRatio, double durationRatio);

autoSound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio, double new_pitch, double pitchRangeFactor, double durationFactor);

autoSound Sound_Pitch_changeGender_old (Sound me, Pitch him, double formantRatio, double new_pitch, double pitchRangeFactor, double durationFactor);

autoPointProcess Sound_to_PointProcess_getJumps (Sound me, integer channel, double minimumJump, double maximumDuration);
/*
	Marks jumps in the signal where the amplitude changes more than 'minimumJump'
	within a time interval of maximumDuration
*/

autoSound Sound_changeSpeaker (Sound me, double pitchMin, double pitchMax,
	double formantMultiplier, // > 0
	double pitchMultiplier, // > 0
	double pitchRangeMultiplier, // any number
	double durationMultiplier // > 0
);

autoSound Sound_Pitch_changeSpeaker (Sound me, Pitch him,
	double formantMultiplier, // > 0
	double pitchMultiplier, // > 0
	double pitchRangeMultiplier, // any number
	double durationMultiplier // > 0
);

/* Outphased */
autoSound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio,
	double new_pitch, double pitchRangeFactor, double durationFactor);

autoTextGrid Sound_to_TextGrid_detectSilences (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration,
	conststring32 silentLabel, conststring32 soundingLabel);

void Sound_getStartAndEndTimesOfSounding (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double *out_t1, double *out_t2);

autoSound Sound_IntervalTier_cutPartsMatchingLabel (Sound me, IntervalTier thee, conststring32 match);
/* Cut intervals that match the label from the sound. The starting time of the new sound is
 * (1) my xmin if the first interval is not matching
 * (2) the end time of the first interval if matching
 */

autoSound Sound_trimSilencesAtStartAndEnd (Sound me, double trimDuration, double minPitch, double timeStep,	double silenceThreshold, 
double minSilenceDuration, double minSoundingDuration, double *startTimeOfSounding, double *endTimeOfSounding);

autoSound Sound_trimSilences (Sound me, double trimDuration, bool onlyAtStartAndEnd, double minPitch, double timeStep,
    double silenceThreshold, double minSilenceDuration, double minSoundingDuration, autoTextGrid *tg, conststring32 trimLabel);

autoSound Sound_copyChannelRanges (Sound me, conststring32 ranges);

autoSound Sound_removeNoise (Sound me, double noiseStart, double noiseEnd, double windowLength, double minBandFilterFrequency, double maxBandFilterFrequency, double smoothing, kSoundNoiseReductionMethod method);

autoSound Sound_reduceNoise (Sound me, double noiseStart, double noiseEnd, double windowLength, double minBandFilterFrequency, double maxBandFilterFrequency, double smoothing, double noiseReduction_dB, kSoundNoiseReductionMethod method);

void Sound_playAsFrequencyShifted (Sound me, double shiftBy, double newSamplingFrequency, integer precision);

#endif /* _Sound_extensions_h_ */
