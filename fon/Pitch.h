#ifndef _Pitch_h_
#define _Pitch_h_
/* Pitch.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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

/*
 * pb 2004/11/09
 */

#ifndef _Sampled_h_
	#include "Sampled.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#include "Pitch_def.h"
#define Pitch_methods Sampled_methods
oo_CLASS_CREATE (Pitch, Sampled)

Pitch Pitch_create (double tmin, double tmax, long nt, double dt, double t1,
	double ceiling, int maxnCandidates);
/*
	Function:
		create an empty pitch contour (voiceless), or NULL if out of memory.
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

int Pitch_Frame_init (Pitch_Frame me, int nCandidates);
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

/* Implemented methods:

	Pitch::info (I)
		gives statistical information about the periodic frames:
		estimates of the 10%, 16%, 50%, 84%, and 90% points of the distribution;
		estimates of the distances from the 50% point to the 16% and 84% points;
		estimates of the mean and standard deviation of the distribution on a Hertz scale;
		minimum and maximum values of the measured set.
	Reading failures:
		Finishing time should be greater than starting time.
		Number of frames should be positive.
		Time step should be greater than 0.0.
		Maximum number of candidates should be positive.
		Number of candidates at time 'x1+(frame-1)*dx' should be positive.
		The number of frequency-strength pairs at time 'x1+(frame-1)*dx' should equal 'nCandidates [frame]'.
*/

int Pitch_isVoiced_i (Pitch me, long index);
/*
	Is the frame 'index' voiced?
	A frame is considered voiced if the frequency of its first candidate
	is greater than 0.0 but less than my ceiling.
	Precondition:
		index >= 1 && index <= my nx;
*/

int Pitch_isVoiced_t (Pitch me, double t);
/*
	Are you voiced at time 't'?
	The answer is TRUE iff 't' lies within a voiced frame.
*/

#define Pitch_HERTZ  0
#define Pitch_MEL  1
#define Pitch_SEMITONES  2
#define Pitch_ERB  3

#define Pitch_STRENGTH_UNIT_AUTOCORRELATION  0
#define Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO  1
#define Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB  2

#define Pitch_NEAREST  0
#define Pitch_LINEAR  1

/* The following routines return NUMundefined if the answer cannot be determined. */

double Pitch_getValueInFrame (Pitch me, long frameNumber, int units);
double Pitch_getStrengthInFrame (Pitch me, long frameNumber, int units);
/*
	Return NUMundefined if the frame 'index' is unvoiced, else the frequency or strength of its first candidate.
*/
double Pitch_getValueAtTime (Pitch me, double t, int units, int interpolation);
double Pitch_getStrengthAtTime (Pitch me, double t, int units, int interpolation);
/*
	Return NUMundefined if 't' is not within a voiced frame.
	Otherwise, the result is linearly interpolated between the centres of two adjacent voiced frames,
	or equal to the centre frequency or strength of a frame at the edge of a sequence of voiced frames. 
*/

double Pitch_getQuantile (Pitch me, double tmin, double tmax, double quantile, int units);
double Pitch_getMean (Pitch me, double tmin, double tmax, int units);
double Pitch_getMeanStrength (Pitch me, double tmin, double tmax, int units);
double Pitch_getStandardDeviation (Pitch me, double tmin, double tmax, int units);

void Pitch_getMaximumAndTime (Pitch me, double tmin, double tmax, int units, int interpolate,
	double *return_maximum, double *return_timeOfMaximum);
double Pitch_getMaximum (Pitch me, double tmin, double tmax, int units, int interpolate);
double Pitch_getTimeOfMaximum (Pitch me, double tmin, double tmax, int units, int interpolate);
void Pitch_getMinimumAndTime (Pitch me, double tmin, double tmax, int units, int interpolate,
	double *return_minimum, double *return_timeOfMinimum);
double Pitch_getMinimum (Pitch me, double tmin, double tmax, int units, int interpolate);
double Pitch_getTimeOfMinimum (Pitch me, double tmin, double tmax, int units, int interpolate);


long Pitch_countVoicedFrames (Pitch me);

int Pitch_getMaxnCandidates (Pitch me);
/*
	Returns the largest number of candidates actually attested in a frame.
*/

void Pitch_setCeiling (Pitch me, double ceiling);
/*
	Postcondition:
		my ceiling = ceiling;
*/

void Pitch_pathFinder (Pitch me, double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost,
	double ceiling, int pullFormants);

/* Drawing methods. */
#define Pitch_speckle_NO  FALSE
#define Pitch_speckle_YES  TRUE
#define Pitch_yscale_LINEAR  1
#define Pitch_yscale_LOGARITHMIC  2
#define Pitch_yscale_SEMITONES  3
#define Pitch_yscale_MEL  4
#define Pitch_yscale_ERB  5
void Pitch_convertYscale (double *fmin, double *fmax, int yscale);
double Pitch_convertFrequency (double f_Hz, int yscale);
const char * Pitch_yscaleText (int yscale);
const char * Pitch_shortUnitText (int yscale);
const char * Pitch_longUnitText (int yscale);
int Pitch_yscaleToUnits (int yscale);
void Pitch_drawInside (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	int speckle, int yscale);
void Pitch_draw (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax, int garnish,
	int speckle, int yscale);
/*
	draw a pitch contour into a Graphics.
	If tmax <= tmin, draw whole time domain.
*/

void Pitch_difference (Pitch me, Pitch thee);
/* give information about frames that are different in me and thee. */

long Pitch_getMeanAbsSlope_hertz (Pitch me, double *slope);
long Pitch_getMeanAbsSlope_mel (Pitch me, double *slope);
long Pitch_getMeanAbsSlope_semitones (Pitch me, double *slope);
long Pitch_getMeanAbsSlope_erb (Pitch me, double *slope);
long Pitch_getMeanAbsSlope_noOctave (Pitch me, double *slope);
/*
   The value returned is the number of voiced frames (nVoiced);
   this signals if the values are valid:
   'value', 'minimum', 'maximum', and 'mean' are valid if nVoiced >= 1;
   'variance' and 'slope' are valid if nVoiced >= 2.
   Invalid variables are always set to 0.0.
   'minimum', 'maximum', 'mean', and 'variance' may be NULL.
*/

Pitch Pitch_killOctaveJumps (Pitch me);
/* Add octave jumps so that every pitch step,
   including those across unvoiced frames,
   does not exceed 1/2 octave.
   Postcondition:
      result -> ceiling = my ceiling * 2;
*/

Pitch Pitch_interpolate (Pitch me);
/* Interpolate the pitch values of unvoiced frames. */
/* No extrapolation beyond first and last voiced frames. */

Pitch Pitch_subtractLinearFit (Pitch me, int units);

Pitch Pitch_smooth (Pitch me, double bandWidth);
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

int Pitch_formula (Pitch me, const char *formula);

/* End of file Pitch.h */
#endif
