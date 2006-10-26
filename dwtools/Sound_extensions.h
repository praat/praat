#ifndef _Sound_extensions_h_
#define _Sound_extensions_h_
/* Sound_extensions.h
 *
 * Copyright (C) 1993-2005 David Weenink
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
 djmw 20061025 Latest modification
*/

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _PointProcess_h_
	#include "PointProcess.h"
#endif
#ifndef _TextGrid_h_
	#include "TextGrid.h"
#endif

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

int Sound_writeToRawFile (Sound me, MelderFile file, const char *format, int littleEndian, 
	int nBitsCoding, int unSigned);

void Sound_into_Sound (Sound me, Sound to, double startTime);
/* precondition: my dx == to->dx (equal sampling times */

int Sound_overwritePart (Sound me, double t1, double t2, Sound thee, double t3);
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
	
Sound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth);

void Sounds_multiply (Sound me, Sound thee);
/* precondition: my nx = thy nx */

double Sound_correlateParts (Sound me, double t1, double t2, double duration);
/*
	Correlate part (t1, t1+duration) with (t2, t2+duration)
*/

void Sound_localMean (Sound me, double fromTime, double toTime, double *mean);
void Sound_localPeak (Sound me, double fromTime, double toTime, double ref, double *peak);

double Sound_power (Sound me);

void Sound_scale_dB (Sound me, double level_dB);
/*
	Scales the amplitude of a Sound to a certain dB level.
	The reference value is an amplitude of 1.
	All amplitudes are multiplied by a scale factor which is
		10^(level_dB/10) / extremum,
	where extremum is the maximum of the absolute values the signal values. 
*/

Sound Sound_changeGender (Sound me, double pitchMin, double pitchMax, double pitchRatio, 
	double formantFrequenciesRatio, double durationRatio);
Sound Sound_and_Pitch_changeGender (Sound me, Pitch him, double pitchMin, double pitchRatio, 
	double formantFrequenciesRatio, double durationRatio);

Sound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor);

Sound Sound_and_Pitch_changeGender_old (Sound me, Pitch him, double fmin, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor);

PointProcess Sound_to_PointProcess_getJumps (Sound me, double minimumJump, double dt);
/*
	Marks jumps in the signal where the amplitude changes more than 'minimumJump'
	within time dt
*/

int Sound_filter_part_formula (Sound me, double t1, double t2, const char *formula);

/* Outphased */
Sound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor);

Sound Sound_and_Pitch_changeGender_old (Sound me, Pitch him, double fmin, double formantRatio, 
	double new_pitch, double pitchRangeFactor, double durationFactor);

IntervalTier Sound_to_IntervalTier_detectSilence (Sound me, double silenceThreshold,
	double minSilenceDuration, double minNonSilenceDuration, char *silenceLabel);
/*
	Marks "silence" intervals in a sound.
	silenceThreshold: silence-to-speech and speech-to-silence threshold as a fraction of the intensity
		range (dB's). 
		We have speech if:
			local_intensity >= max_intensity - (1-silenceThreshold)*(max_intensity-min_intensity).
		Silence if:
			local_intensity < max_intensity - (1-silenceThreshold)*(max_intensity-min_intensity).
	If minSilenceDuration > 0 then only intervals with a duration > minSilenceDuration will be labelled
	as silences in the IntervalTier.
	
*/
#endif /* _Sound_extensions_h_ */
