#ifndef _Pitch_h_
#define _Pitch_h_
/* Pitch.h
 *
 * Copyright (C) 1992-2007,2009,2011,2012,2014-2020 Paul Boersma
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

#include "Sampled.h"
#include "Graphics.h"
#include "Table.h"
Thing_declare (Interpreter);

#include "Pitch_enums.h"
#include "Pitch_def.h"

autoPitch Pitch_create (double tmin, double tmax, integer nt, double dt, double t1,
	double ceiling, integer maxnCandidates);
/*
	Function:
		create an empty pitch contour (voiceless).
	Preconditions:
		tmax > tmin;
		nt >= 1;
		dt > 0.0;
		maxnCandidates >= 2;
	Postconditions:
		my xmin == tmin;
		my xmax == tmax;
		my nx == nt;
		my dx == dt;
		my x1 == t1;
		my ceiling == ceiling;
		my maxnCandidates == maxnCandidates;
		my frame [1..nt]. nCandidates == 1;
		my frame [1..nt]. candidate [1]. frequency == 0.0; // unvoiced
		my frame [1..nt]. candidate [1]. strength == 0.0; // aperiodic
		my frame [1..nt]. intensity == 0.0; // silent
*/

void Pitch_Frame_init (Pitch_Frame me, integer nCandidates);
/*
	Function:
		create space for a number of candidates; space already there is disposed of.
	Preconditions:
		nCandidates >= 1;
	Postconditions:
		my nCandidates == nCandidates;
		my candidate [1..nCandidates]. frequency == 0.0; // unvoiced
		my candidate [1..nCandidates]. strength == 0.0; // aperiodic
		my intensity == 0.0; // silent
*/

inline bool Pitch_util_frequencyIsVoiced (double f, double ceiling) {
	return f > 0.0 && f < ceiling;   // note: return false is f is NaN
}

bool Pitch_isVoiced_i (Pitch me, integer index);
/*
	Is the frame 'index' voiced?
	A frame is considered voiced if the frequency of its first candidate
	is greater than 0.0 but less than my ceiling.
	Precondition:
		index >= 1 && index <= my nx;
*/

bool Pitch_isVoiced_t (Pitch me, double t);
/*
	Are you voiced at time `t`?
	The answer is `true` iff `t` lies within a voiced frame.
*/

#define Pitch_LEVEL_FREQUENCY  1
#define Pitch_LEVEL_STRENGTH  2

#define Pitch_STRENGTH_UNIT_min  0
#define Pitch_STRENGTH_UNIT_AUTOCORRELATION  0
#define Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO  1
#define Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB  2
#define Pitch_STRENGTH_UNIT_max  2

#define Pitch_NEAREST  0
#define Pitch_LINEAR  1

double Pitch_getValueAtTime (Pitch me, double time, kPitch_unit unit, bool interpolate);
double Pitch_getStrengthAtTime (Pitch me, double time, kPitch_unit unit, bool interpolate);

integer Pitch_countVoicedFrames (Pitch me);

double Pitch_getMean (Pitch me, double tmin, double tmax, kPitch_unit unit);
double Pitch_getMeanStrength (Pitch me, double tmin, double tmax, int strengthUnit);
double Pitch_getQuantile (Pitch me, double tmin, double tmax, double quantile, kPitch_unit unit);
double Pitch_getStandardDeviation (Pitch me, double tmin, double tmax, kPitch_unit unit);
void Pitch_getMaximumAndTime (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate,
	double *return_maximum, double *return_timeOfMaximum);
double Pitch_getMaximum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate);
double Pitch_getTimeOfMaximum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate);
void Pitch_getMinimumAndTime (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate,
	double *return_minimum, double *return_timeOfMinimum);
double Pitch_getMinimum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate);
double Pitch_getTimeOfMinimum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate);

integer Pitch_getMaxnCandidates (Pitch me);
/*
	Returns the largest number of candidates actually attested in a frame.
*/

void Pitch_setCeiling (Pitch me, double ceiling);
/*
	Postcondition:
		my ceiling = ceiling;
*/

autoMAT Pitch_Frame_getAllCandidates (Pitch_Frame me);
autoMAT Pitch_getAllCandidatesInFrame (Pitch me, integer frameNumber);

void Pitch_pathFinder (Pitch me, double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost,
	double ceiling, int pullFormants);

/* Drawing methods. */
#define Pitch_speckle_NO  false
#define Pitch_speckle_YES  true
void Pitch_drawInside (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	bool speckle, kPitch_unit yscale);
void Pitch_draw (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish,
	bool speckle, kPitch_unit yscale);
/*
	draw a pitch contour into a Graphics.
	If tmax <= tmin, draw whole time domain.
*/

void Pitch_difference (Pitch me, Pitch thee);
/* give information about frames that are different in me and thee. */

integer Pitch_getMeanAbsSlope_hertz (Pitch me, double *slope);
integer Pitch_getMeanAbsSlope_mel (Pitch me, double *slope);
integer Pitch_getMeanAbsSlope_semitones (Pitch me, double *slope);
integer Pitch_getMeanAbsSlope_erb (Pitch me, double *slope);
integer Pitch_getMeanAbsSlope_noOctave (Pitch me, double *slope);
/*
   The value returned is the number of voiced frames (nVoiced);
   this signals if the values are valid:
   'value', 'minimum', 'maximum', and 'mean' are valid if nVoiced >= 1;
   'variance' and 'slope' are valid if nVoiced >= 2.
   Invalid variables are always set to 0.0.
   'minimum', 'maximum', 'mean', and 'variance' may be null.
*/

MelderFraction Pitch_getFractionOfLocallyVoicedFrames (Pitch me, double tmin, double tmax,
	double ceiling, double silenceThreshold, double voicingThreshold);
MelderFraction Pitch_getFractionOfLocallyUnvoicedFrames (Pitch me, double tmin, double tmax,
	double ceiling, double silenceThreshold, double voicingThreshold);

autoPitch Pitch_killOctaveJumps (Pitch me);
/* Add octave jumps so that every pitch step,
   including those across unvoiced frames,
   does not exceed 1/2 octave.
   Postcondition:
      result -> ceiling = my ceiling * 2;
*/

autoPitch Pitch_interpolate (Pitch me);
/* Interpolate the pitch values of unvoiced frames. */
/* No extrapolation beyond first and last voiced frames. */

autoPitch Pitch_subtractLinearFit (Pitch me, kPitch_unit unit);

autoPitch Pitch_smooth (Pitch me, double bandWidth);
/* Smoothing by convolution with Gaussian curve.
   Time domain: exp (- (pi t bandWidth) ^ 2)
      down to 8.5 % for t = +- 0.5/bandWidth
   Frequency domain: exp (- (f / bandWidth) ^ 2)
      down to 1/e for f = +- bandWidth
   Example:
      if bandWidth = 10 Hz,
      then the Gaussian curve has a 8.5% duration of 0.1 s,
      and a 1/e low-pass point of 10 Hz.
   Algorithm:
      Interpolation of pitch at internal unvoiced parts.
      Zeroth-degree extrapolation at edges (duration triples).
      FFT; multiply by Gaussian; inverse FFT.
      Cut back to normal duration.
      Undo interpolation.
*/

void Pitch_step (Pitch me, double step, double precision, double tmin, double tmax);
/*
	Instead of the currently chosen candidate,
	choose the candidate with another ("target") frequency, determined by 'step'.
	E.g., for an upward octave jump, 'step' is 2.
	Only consider frequencies between (1 - precision) * targetFrequency
	and (1 - precision) * targetFrequency.
	Take the candidate nearest to targetFrequency,
	as long as that candidate is in between 0 and my ceiling.
*/

void Pitch_formula (Pitch me, conststring32 formula, Interpreter interpreter);

autoVEC Pitch_listValuesInAllFrames (Pitch me);

autoTable Pitch_tabulateCandidatesInFrame (Pitch me, integer frameNumber);
autoTable Pitch_tabulateCandidates (Pitch me);

/* End of file Pitch.h */
#endif
