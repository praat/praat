/* Pitch.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Pitch.h"
#include <ctype.h>
#include "Sound_and_Spectrum.h"
#include "Matrix_and_Pitch.h"

#include "oo_DESTROY.h"
#include "Pitch_def.h"
#include "oo_COPY.h"
#include "Pitch_def.h"
#include "oo_EQUAL.h"
#include "Pitch_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Pitch_def.h"
#include "oo_WRITE_TEXT.h"
#include "Pitch_def.h"
#include "oo_READ_TEXT.h"
#include "Pitch_def.h"
#include "oo_WRITE_BINARY.h"
#include "Pitch_def.h"
#include "oo_READ_BINARY.h"
#include "Pitch_def.h"
#include "oo_DESCRIPTION.h"
#include "Pitch_def.h"

#include "enums_getText.h"
#include "Pitch_enums.h"
#include "enums_getValue.h"
#include "Pitch_enums.h"

Thing_implement (Pitch, Sampled, 1);

#define FREQUENCY(frame)  ((frame) -> candidate [1]. frequency)
#define STRENGTH(frame)  ((frame) -> candidate [1]. strength)
#define NOT_VOICED(f)  ((f) <= 0.0 || (f) >= my ceiling)   /* This includes NUMundefined! */

int structPitch :: v_getMinimumUnit (long ilevel) {
	return ilevel == Pitch_LEVEL_FREQUENCY ? kPitch_unit_MIN : Pitch_STRENGTH_UNIT_min;
}

int structPitch :: v_getMaximumUnit (long ilevel) {
	return ilevel == Pitch_LEVEL_FREQUENCY ? kPitch_unit_MAX : Pitch_STRENGTH_UNIT_max;
}

const wchar_t * structPitch :: v_getUnitText (long ilevel, int unit, unsigned long flags) {
	if (ilevel == Pitch_LEVEL_FREQUENCY) {
		return
			unit == kPitch_unit_HERTZ ?
				flags & Function_UNIT_TEXT_MENU ? L"Hertz" : L"Hz" :
			unit == kPitch_unit_HERTZ_LOGARITHMIC ?
				flags & Function_UNIT_TEXT_MENU ? L"Hertz (logarithmic)" : (flags & Function_UNIT_TEXT_SHORT) && (flags & Function_UNIT_TEXT_GRAPHICAL) ? L"%%Hz%" : L"Hz" :
			unit == kPitch_unit_MEL ? L"mel" :
			unit == kPitch_unit_LOG_HERTZ ?
				flags & Function_UNIT_TEXT_MENU ? L"logHertz" : L"logHz" :
			unit == kPitch_unit_SEMITONES_1 ?
				flags & Function_UNIT_TEXT_SHORT ? L"st__1_" : flags & Function_UNIT_TEXT_GRAPHICAL ? L"semitones %%re% 1 Hz" : L"semitones re 1 Hz" :
			unit == kPitch_unit_SEMITONES_100 ?
				flags & Function_UNIT_TEXT_SHORT ? L"st__100_" : flags & Function_UNIT_TEXT_GRAPHICAL ? L"semitones %%re% 100 Hz" : L"semitones re 100 Hz" :
			unit == kPitch_unit_SEMITONES_200 ?
				flags & Function_UNIT_TEXT_SHORT ? L"st__200_" : flags & Function_UNIT_TEXT_GRAPHICAL ? L"semitones %%re% 200 Hz" : L"semitones re 200 Hz" :
			unit == kPitch_unit_SEMITONES_440 ?
				flags & Function_UNIT_TEXT_SHORT ? L"st__a_" : flags & Function_UNIT_TEXT_GRAPHICAL ? L"semitones %%re% 440 Hz" : L"semitones re 440 Hz" :
			unit == kPitch_unit_ERB ?
				flags & Function_UNIT_TEXT_SHORT ? L"erb" : L"ERB" :
			L"";
	} else if (ilevel == Pitch_LEVEL_STRENGTH) {
		return
			unit == Pitch_STRENGTH_UNIT_AUTOCORRELATION ? L"" :
			unit == Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO ? L"" :
			unit == Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB ? L"dB" :
			L"";
	}
	return L"unknown";
}

bool structPitch :: v_isUnitLogarithmic (long ilevel, int unit) {
	return ilevel == Pitch_LEVEL_FREQUENCY && unit == kPitch_unit_HERTZ_LOGARITHMIC;
}

double structPitch :: v_convertStandardToSpecialUnit (double value, long ilevel, int unit) {
	if (ilevel == Pitch_LEVEL_FREQUENCY) {
		return
			unit == kPitch_unit_HERTZ ? value :
			unit == kPitch_unit_HERTZ_LOGARITHMIC ? value <= 0.0 ? NUMundefined : log10 (value) :
			unit == kPitch_unit_MEL ? NUMhertzToMel (value) :
			unit == kPitch_unit_LOG_HERTZ ? value <= 0.0 ? NUMundefined : log10 (value) :
			unit == kPitch_unit_SEMITONES_1 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 1.0) / NUMln2 :
			unit == kPitch_unit_SEMITONES_100 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 100.0) / NUMln2 :
			unit == kPitch_unit_SEMITONES_200 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 200.0) / NUMln2 :
			unit == kPitch_unit_SEMITONES_440 ? value <= 0.0 ? NUMundefined : 12.0 * log (value / 440.0) / NUMln2 :
			unit == kPitch_unit_ERB ? NUMhertzToErb (value) :
			NUMundefined;
	} else {
		return
			unit == Pitch_STRENGTH_UNIT_AUTOCORRELATION ? value :
			unit == Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO ?
				value <= 1e-15 ? 1e15 : value > 1.0 - 1e-15 ? 1e-15 : (1.0 - value) / value :   /* Before losing precision. */
			unit == Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB ?
				value <= 1e-15 ? -150.0 : value > 1.0 - 1e-15 ? 150.0 : 10 * log10 (value / (1.0 - value)) :   /* Before losing precision. */
			NUMundefined;
	}
}

double structPitch :: v_convertSpecialToStandardUnit (double value, long ilevel, int unit) {
	if (ilevel == Pitch_LEVEL_FREQUENCY) {
		return
			unit == kPitch_unit_HERTZ ? value :
			unit == kPitch_unit_HERTZ_LOGARITHMIC ? pow (10.0, value) :
			unit == kPitch_unit_MEL ? NUMmelToHertz (value) :
			unit == kPitch_unit_LOG_HERTZ ? pow (10.0, value) :
			unit == kPitch_unit_SEMITONES_1 ? 1.0 * exp (value * (NUMln2 / 12.0)):
			unit == kPitch_unit_SEMITONES_100 ? 100.0 * exp (value * (NUMln2 / 12.0)):
			unit == kPitch_unit_SEMITONES_200 ? 200.0 * exp (value * (NUMln2 / 12.0)):
			unit == kPitch_unit_SEMITONES_440 ? 440.0 * exp (value * (NUMln2 / 12.0)):
			unit == kPitch_unit_ERB ? NUMerbToHertz (value) :
			NUMundefined;
	} else {
		return NUMundefined;
	}
}

#define doesUnitAllowNegativeValues(unit)  \
	( (unit) == kPitch_unit_HERTZ_LOGARITHMIC || (unit) == kPitch_unit_LOG_HERTZ ||  \
	  (unit) == kPitch_unit_SEMITONES_1 || (unit) == kPitch_unit_SEMITONES_100 ||  \
	  (unit) == kPitch_unit_SEMITONES_200 || (unit) == kPitch_unit_SEMITONES_440 )

double structPitch :: v_getValueAtSample (long iframe, long ilevel, int unit) {
	double f = frame [iframe]. candidate [1]. frequency;
	if (f <= 0.0 || f >= ceiling) return NUMundefined;   // frequency out of range (or NUMundefined)? Voiceless
	return v_convertStandardToSpecialUnit (ilevel == Pitch_LEVEL_FREQUENCY ? f : frame [iframe]. candidate [1]. strength, ilevel, unit);
}

bool Pitch_isVoiced_i (Pitch me, long iframe) {
	return NUMdefined (Sampled_getValueAtSample (me, iframe, Pitch_LEVEL_FREQUENCY, kPitch_unit_HERTZ));
}

bool Pitch_isVoiced_t (Pitch me, double time) {
	return NUMdefined (Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, kPitch_unit_HERTZ, FALSE));
}

double Pitch_getValueAtTime (Pitch me, double time, int unit, int interpolate) {
	return Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, unit, interpolate);
}

double Pitch_getStrengthAtTime (Pitch me, double time, int unit, int interpolate) {
	return Sampled_getValueAtX (me, time, Pitch_LEVEL_STRENGTH, unit, interpolate);
}

long Pitch_countVoicedFrames (Pitch me) {
	return Sampled_countDefinedSamples (me, Pitch_LEVEL_FREQUENCY, kPitch_unit_HERTZ);
}

double Pitch_getMean (Pitch me, double tmin, double tmax, int unit) {
	return Sampled_getMean (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, unit, TRUE);
}

double Pitch_getMeanStrength (Pitch me, double tmin, double tmax, int unit) {
	return Sampled_getMean (me, tmin, tmax, Pitch_LEVEL_STRENGTH, unit, TRUE);
}

double Pitch_getQuantile (Pitch me, double tmin, double tmax, double quantile, int unit) {
	double value = Sampled_getQuantile (me, tmin, tmax, quantile, Pitch_LEVEL_FREQUENCY, unit);
	if (value <= 0.0 && ! doesUnitAllowNegativeValues (unit)) {
		value = NUMundefined;
	}
	return value;
}

double Pitch_getStandardDeviation (Pitch me, double tmin, double tmax, int unit) {
	return Sampled_getStandardDeviation (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, unit, TRUE);
}

#define MEL(f)  NUMhertzToMel (f)
#define SEMITONES(f)  NUMhertzToSemitones (f)
#define ERB(f)  NUMhertzToErb (f)

void Pitch_getMaximumAndTime (Pitch me, double tmin, double tmax, int unit, int interpolate,
	double *return_maximum, double *return_timeOfMaximum)
{
	Sampled_getMaximumAndX (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, unit, interpolate, return_maximum, return_timeOfMaximum);
	if (! doesUnitAllowNegativeValues (unit) && return_maximum && *return_maximum <= 0.0)
	{
		*return_maximum = NUMundefined;   /* Unlikely. */
	}
}

double Pitch_getMaximum (Pitch me, double tmin, double tmax, int unit, int interpolate) {
	double maximum;
	Pitch_getMaximumAndTime (me, tmin, tmax, unit, interpolate, & maximum, NULL);
	return maximum;
}

double Pitch_getTimeOfMaximum (Pitch me, double tmin, double tmax, int unit, int interpolate) {
	double time;
	Pitch_getMaximumAndTime (me, tmin, tmax, unit, interpolate, NULL, & time);
	return time;
}

void Pitch_getMinimumAndTime (Pitch me, double tmin, double tmax, int unit, int interpolate,
	double *return_minimum, double *return_timeOfMinimum)
{
	Sampled_getMinimumAndX (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, unit, interpolate, return_minimum, return_timeOfMinimum);
	if (! doesUnitAllowNegativeValues (unit) && return_minimum && *return_minimum <= 0.0)
	{
		*return_minimum = NUMundefined;   /* Not so unlikely. */
	}
}

double Pitch_getMinimum (Pitch me, double tmin, double tmax, int unit, int interpolate) {
	double minimum;
	Pitch_getMinimumAndTime (me, tmin, tmax, unit, interpolate, & minimum, NULL);
	return minimum;
}

double Pitch_getTimeOfMinimum (Pitch me, double tmin, double tmax, int unit, int interpolate) {
	double time;
	Pitch_getMinimumAndTime (me, tmin, tmax, unit, interpolate, NULL, & time);
	return time;
}

static long Pitch_getMeanAbsoluteSlope (Pitch me,
	double *out_hertz, double *out_mel, double *out_semitones, double *out_erb, double *out_withoutOctaveJumps)
{
	long firstVoicedFrame = 0, lastVoicedFrame = 0, nVoiced = 0, i;
	double *frequencies = NUMvector <double> (1, my nx);
	for (i = 1; i <= my nx; i ++) {
		double frequency = my frame [i]. candidate [1]. frequency;
		frequencies [i] = frequency > 0.0 && frequency < my ceiling ? frequency : 0.0;
		if (frequencies [i]) nVoiced ++;
	}
	for (i = 1; i <= my nx; i ++)   /* Look for first voiced frame. */
		if (frequencies [i] != 0.0) { firstVoicedFrame = i; break; }
	for (i = my nx; i >= 1; i --)   /* Look for last voiced frame. */
		if (frequencies [i] != 0.0) { lastVoicedFrame = i; break; }
	if (nVoiced > 1) {
		int ilast = firstVoicedFrame;
		double span = (lastVoicedFrame - firstVoicedFrame) * my dx, flast = frequencies [ilast];
		double slopeHz = 0, slopeMel = 0, slopeSemitones = 0, slopeErb = 0, slopeRobust = 0;
		for (i = firstVoicedFrame + 1; i <= lastVoicedFrame; i ++) if (frequencies [i] != 0.0) {
			double localStepSemitones = fabs (SEMITONES (frequencies [i]) - SEMITONES (flast));
			slopeHz += fabs (frequencies [i] - flast);
			slopeMel += fabs (MEL (frequencies [i]) - MEL (flast));
			slopeSemitones += localStepSemitones;
			slopeErb += fabs (ERB (frequencies [i]) - ERB (flast));
			while (localStepSemitones >= 12.0) localStepSemitones -= 12.0;   /* Kill octave jumps. */
			if (localStepSemitones > 6.0) localStepSemitones = 12.0 - localStepSemitones;
			slopeRobust += localStepSemitones;
			ilast = i;
			flast = frequencies [i];
		}
		if (out_hertz) *out_hertz = slopeHz / span;
		if (out_mel) *out_mel = slopeMel / span;
		if (out_semitones) *out_semitones = slopeSemitones / span;
		if (out_erb) *out_erb = slopeErb / span;
		if (out_withoutOctaveJumps) *out_withoutOctaveJumps = slopeRobust / span;
	} else {
		if (out_hertz) *out_hertz = NUMundefined;
		if (out_mel) *out_mel = NUMundefined;
		if (out_semitones) *out_semitones = NUMundefined;
		if (out_erb) *out_erb = NUMundefined;
		if (out_withoutOctaveJumps) *out_withoutOctaveJumps = NUMundefined;
	}
	NUMvector_free (frequencies, 1);
	return nVoiced;
}

long Pitch_getMeanAbsSlope_hertz (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, slope, NULL, NULL, NULL, NULL);
}

long Pitch_getMeanAbsSlope_mel (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, NULL, slope, NULL, NULL, NULL);
}

long Pitch_getMeanAbsSlope_semitones (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, NULL, NULL, slope, NULL, NULL);
}

long Pitch_getMeanAbsSlope_erb (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, NULL, NULL, NULL, slope, NULL);
}

long Pitch_getMeanAbsSlope_noOctave (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, NULL, NULL, NULL, NULL, slope);
}

void structPitch :: v_info () {
	long nVoiced;
	double *frequencies = Sampled_getSortedValues (this, Pitch_LEVEL_FREQUENCY, kPitch_unit_HERTZ, & nVoiced);
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"Time sampling:");
	MelderInfo_writeLine (L"   Number of frames: ", Melder_integer (nx), L" (", Melder_integer (nVoiced), L" voiced)");
	MelderInfo_writeLine (L"   Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"   First frame centred at: ", Melder_double (x1), L" seconds");
	MelderInfo_writeLine (L"Ceiling at: ", Melder_double (ceiling), L" Hz");

	if (nVoiced >= 1) {   /* Quantiles. */
		double quantile10, quantile16, quantile50, quantile84, quantile90;
		quantile10 = NUMquantile (nVoiced, frequencies, 0.10);
		quantile16 = NUMquantile (nVoiced, frequencies, 0.16);
		quantile50 = NUMquantile (nVoiced, frequencies, 0.50);   /* Median. */
		quantile84 = NUMquantile (nVoiced, frequencies, 0.84);
		quantile90 = NUMquantile (nVoiced, frequencies, 0.90);
		MelderInfo_writeLine (L"\nEstimated quantiles:");
		MelderInfo_write (L"   10% = ", Melder_single (quantile10), L" Hz = ", Melder_single (MEL (quantile10)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (quantile10)), L" semitones above 100 Hz = ", Melder_single (ERB (quantile10)), L" ERB");
		MelderInfo_write (L"   16% = ", Melder_single (quantile16), L" Hz = ", Melder_single (MEL (quantile16)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (quantile16)), L" semitones above 100 Hz = ", Melder_single (ERB (quantile16)), L" ERB");
		MelderInfo_write (L"   50% = ", Melder_single (quantile50), L" Hz = ", Melder_single (MEL (quantile50)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (quantile50)), L" semitones above 100 Hz = ", Melder_single (ERB (quantile50)), L" ERB");
		MelderInfo_write (L"   84% = ", Melder_single (quantile84), L" Hz = ", Melder_single (MEL (quantile84)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (quantile84)), L" semitones above 100 Hz = ", Melder_single (ERB (quantile84)), L" ERB");
		MelderInfo_write (L"   90% = ", Melder_single (quantile90), L" Hz = ", Melder_single (MEL (quantile90)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (quantile90)), L" semitones above 100 Hz = ", Melder_single (ERB (quantile90)), L" ERB");
		if (nVoiced > 1) {
			double corr = sqrt (nVoiced / (nVoiced - 1.0));
			MelderInfo_writeLine (L"\nEstimated spreading:");
			MelderInfo_write (L"   84%-median = ", Melder_half ((quantile84 - quantile50) * corr), L" Hz = ", Melder_half ((MEL (quantile84) - MEL (quantile50)) * corr), L" Mel = ");
			MelderInfo_writeLine (Melder_half ((SEMITONES (quantile84) - SEMITONES (quantile50)) * corr), L" semitones = ", Melder_half ((ERB (quantile84) - ERB (quantile50)) * corr), L" ERB");
			MelderInfo_write (L"   median-16% = ", Melder_half ((quantile50 - quantile16) * corr), L" Hz = ", Melder_half ((MEL (quantile50) - MEL (quantile16)) * corr), L" Mel = ");
			MelderInfo_writeLine (Melder_half ((SEMITONES (quantile50) - SEMITONES (quantile16)) * corr), L" semitones = ", Melder_half ((ERB (quantile50) - ERB (quantile16)) * corr), L" ERB");
			MelderInfo_write (L"   90%-10% = ", Melder_half ((quantile90 - quantile10) * corr), L" Hz = ", Melder_half ((MEL (quantile90) - MEL (quantile10)) * corr), L" Mel = ");
			MelderInfo_writeLine (Melder_half ((SEMITONES (quantile90) - SEMITONES (quantile10)) * corr), L" semitones = ", Melder_half ((ERB (quantile90) - ERB (quantile10)) * corr), L" ERB");
		}
	}
	if (nVoiced >= 1) {   /* Extrema, range, mean and standard deviation. */
		double minimum = Pitch_getMinimum (this, xmin, xmax, kPitch_unit_HERTZ, FALSE);
		double maximum = Pitch_getMaximum (this, xmin, xmax, kPitch_unit_HERTZ, FALSE);
		double meanHertz, meanMel, meanSemitones, meanErb;
		MelderInfo_write (L"\nMinimum ", Melder_single (minimum), L" Hz = ", Melder_single (MEL (minimum)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (minimum)), L" semitones above 100 Hz = ", Melder_single (ERB (minimum)), L" ERB");
		MelderInfo_write (L"Maximum ", Melder_single (maximum), L" Hz = ", Melder_single (MEL (maximum)), L" Mel = ");
		MelderInfo_writeLine (Melder_single (SEMITONES (maximum)), L" semitones above 100 Hz = ", Melder_single (ERB (maximum)), L" ERB");
		MelderInfo_write (L"Range ", Melder_half (maximum - minimum), L" Hz = ", Melder_single (MEL (maximum) - MEL (minimum)), L" Mel = ");
		MelderInfo_writeLine (Melder_half (SEMITONES (maximum) - SEMITONES (minimum)), L" semitones = ", Melder_half (ERB (maximum) - ERB (minimum)), L" ERB");
		meanHertz = Pitch_getMean (this, 0, 0, kPitch_unit_HERTZ);
		meanMel = Pitch_getMean (this, 0, 0, kPitch_unit_MEL);
		meanSemitones = Pitch_getMean (this, 0, 0, kPitch_unit_SEMITONES_100);
		meanErb = Pitch_getMean (this, 0, 0, kPitch_unit_ERB);
		MelderInfo_write (L"Average: ", Melder_single (meanHertz), L" Hz = ", Melder_single (meanMel), L" Mel = ");
		MelderInfo_writeLine (Melder_single (meanSemitones), L" semitones above 100 Hz = ", Melder_single (meanErb), L" ERB");
		if (nVoiced >= 2) {
			double stdevHertz = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit_HERTZ);
			double stdevMel = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit_MEL);
			double stdevSemitones = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit_SEMITONES_100);
			double stdevErb = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit_ERB);
			MelderInfo_write (L"Standard deviation: ", Melder_half (stdevHertz), L" Hz = ", Melder_half (stdevMel), L" Mel = ");
			MelderInfo_writeLine (Melder_half (stdevSemitones), L" semitones = ", Melder_half (stdevErb), L" ERB");
		}
	}
	NUMvector_free (frequencies, 1);
	if (nVoiced > 1) {   /* Variability: mean absolute slope. */
		double slopeHertz, slopeMel, slopeSemitones, slopeErb, slopeWithoutOctaveJumps;
		Pitch_getMeanAbsoluteSlope (this, & slopeHertz, & slopeMel, & slopeSemitones, & slopeErb, & slopeWithoutOctaveJumps);
		MelderInfo_write (L"\nMean absolute slope: ", Melder_half (slopeHertz), L" Hz/s = ", Melder_half (slopeMel), L" Mel/s = ");
		MelderInfo_writeLine (Melder_half (slopeSemitones), L" semitones/s = ", Melder_half (slopeErb), L" ERB/s");
		MelderInfo_writeLine (L"Mean absolute slope without octave jumps: ", Melder_half (slopeWithoutOctaveJumps), L" semitones/s");
	}
}

void Pitch_Frame_init (Pitch_Frame me, int nCandidates) {
	/*
	 * Create without change.
	 */
	autoNUMvector <structPitch_Candidate> candidate (1, nCandidates);
	/*
	 * Change without error.
	 */
	NUMvector_free (my candidate, 1);
	my candidate = candidate.transfer();
	my nCandidates = nCandidates;
}

Pitch Pitch_create (double tmin, double tmax, long nt, double dt, double t1,
	double ceiling, int maxnCandidates)
{
	try {
		autoPitch me = Thing_new (Pitch);
		Sampled_init (me.peek(), tmin, tmax, nt, dt, t1);
		my ceiling = ceiling;
		my maxnCandidates = maxnCandidates;
		my frame = NUMvector <structPitch_Frame> (1, nt);

		/* Put one candidate in every frame (unvoiced, silent). */
		for (long it = 1; it <= nt; it ++) {
			Pitch_Frame_init (& my frame [it], 1);
		}

		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Pitch not created.");
	}
}

void Pitch_setCeiling (Pitch me, double ceiling) {
	my ceiling = ceiling;
}

int Pitch_getMaxnCandidates (Pitch me) {
	int result = 0;
	for (long i = 1; i <= my nx; i ++) {
		int nCandidates = my frame [i]. nCandidates;
		if (nCandidates > result) result = nCandidates;
	}
	return result;
}

void Pitch_pathFinder (Pitch me, double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost,
	double ceiling, int pullFormants)
{
	if (Melder_debug == 33) Melder_casual ("Pitch path finder:\nSilence threshold = %g\nVoicing threshold = %g\nOctave cost = %g\nOctave jump cost = %g\n"
		"Voiced/unvoiced cost = %g\nCeiling = %g\nPull formants = %d", silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost,
		ceiling, pullFormants);
	try {
		long maxnCandidates = Pitch_getMaxnCandidates (me);
		long place;
		volatile double maximum, value;
		double ceiling2 = pullFormants ? 2 * ceiling : ceiling;
		/* Next three lines 20011015 */
		double timeStepCorrection = 0.01 / my dx;
		octaveJumpCost *= timeStepCorrection;
		voicedUnvoicedCost *= timeStepCorrection;

		my ceiling = ceiling;
		autoNUMmatrix <double> delta (1, my nx, 1, maxnCandidates);
		autoNUMmatrix <long> psi (1, my nx, 1, maxnCandidates);

		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Pitch_Frame frame = & my frame [iframe];
			double unvoicedStrength = silenceThreshold <= 0 ? 0 :
				2 - frame->intensity / (silenceThreshold / (1 + voicingThreshold));
			unvoicedStrength = voicingThreshold + (unvoicedStrength > 0 ? unvoicedStrength : 0);
			for (long icand = 1; icand <= frame->nCandidates; icand ++) {
				Pitch_Candidate candidate = & frame->candidate [icand];
				int voiceless = candidate->frequency == 0 || candidate->frequency > ceiling2;
				delta [iframe] [icand] = voiceless ? unvoicedStrength :
					candidate->strength - octaveCost * NUMlog2 (ceiling / candidate->frequency);
			}
		}

		/* Look for the most probable path through the maxima. */
		/* There is a cost for the voiced/unvoiced transition, */
		/* and a cost for a frequency jump. */

		for (long iframe = 2; iframe <= my nx; iframe ++) {
			Pitch_Frame prevFrame = & my frame [iframe - 1], curFrame = & my frame [iframe];
			double *prevDelta = delta [iframe - 1], *curDelta = delta [iframe];
			long *curPsi = psi [iframe];
			for (long icand2 = 1; icand2 <= curFrame -> nCandidates; icand2 ++) {
				double f2 = curFrame -> candidate [icand2]. frequency;
				maximum = -1e30;
				place = 0;
				for (long icand1 = 1; icand1 <= prevFrame -> nCandidates; icand1 ++) {
					double f1 = prevFrame -> candidate [icand1]. frequency;
					double transitionCost;
					bool previousVoiceless = f1 <= 0 || f1 >= ceiling2;
					bool currentVoiceless = f2 <= 0 || f2 >= ceiling2;
					if (currentVoiceless) {
						if (previousVoiceless) {
							transitionCost = 0;   // both voiceless
						} else {
							transitionCost = voicedUnvoicedCost;   // voiced-to-unvoiced transition
						}
					} else {
						if (previousVoiceless) {
							transitionCost = voicedUnvoicedCost;   // unvoiced-to-voiced transition
							if (Melder_debug == 30) {
								/*
								 * Try to take into account a frequency jump across a voiceless stretch.
								 */
								long place1 = icand1;
								for (long jframe = iframe - 2; jframe >= 1; jframe --) {
									place1 = psi [jframe + 1] [place1];
									f1 = my frame [jframe]. candidate [place1]. frequency;
									if (f1 > 0 && f1 < ceiling) {
										transitionCost += octaveJumpCost * fabs (NUMlog2 (f1 / f2)) / (iframe - jframe);
										break;
									}
								}
							}
						} else {
							transitionCost = octaveJumpCost * fabs (NUMlog2 (f1 / f2));   // both voiced
						}
					}
					value = prevDelta [icand1] - transitionCost + curDelta [icand2];
					//if (Melder_debug == 33) Melder_casual ("Frame %ld, current candidate %ld (delta %g), previous candidate %ld (delta %g), "
					//	"transition cost %g, value %g, maximum %g", iframe, icand2, curDelta [icand2], icand1, prevDelta [icand1], transitionCost, value, maximum);
					if (value > maximum) {
						maximum = value;
						place = icand1;
					} else if (value == maximum) {
						if (Melder_debug == 33) Melder_casual ("A tie in frame %ld, current candidate %ld, previous candidate %ld", iframe, icand2, icand1);
					}
				}
				curDelta [icand2] = maximum;
				curPsi [icand2] = place;
			}
		}

		/* Find the end of the most probable path. */

		place = 1;
		maximum = delta [my nx] [place];
		for (long icand = 2; icand <= my frame [my nx]. nCandidates; icand ++) {
			if (delta [my nx] [icand] > maximum) {
				place = icand;
				maximum = delta [my nx] [place];
			}
		}

		/* Backtracking: follow the path backwards. */

		for (long iframe = my nx; iframe >= 1; iframe --) {
			if (Melder_debug == 33) Melder_casual ("Frame %ld: swapping candidates 1 and %ld", iframe, place);
			Pitch_Frame frame = & my frame [iframe];
			structPitch_Candidate help = frame -> candidate [1];
			frame -> candidate [1] = frame -> candidate [place];
			frame -> candidate [place] = help;
			place = psi [iframe] [place];   // This assignment is challenging to CodeWarrior 11.
		}

		/* Pull formants: devoice frames with frequencies between ceiling and ceiling2. */

		if (ceiling2 > ceiling) {
			if (Melder_debug == 33) Melder_casual ("Pulling formants...");
			for (long iframe = my nx; iframe >= 1; iframe --) {
				Pitch_Frame frame = & my frame [iframe];
				Pitch_Candidate winner = & frame -> candidate [1];
				double f = winner -> frequency;
				if (f > ceiling && f <= ceiling2) {
					for (long icand = 2; icand <= frame -> nCandidates; icand ++) {
						Pitch_Candidate loser = & frame -> candidate [icand];
						if (loser -> frequency == 0.0) {
							structPitch_Candidate help = * winner;
							* winner = * loser;
							* loser = help;
							break;
						}
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": path not found.");
	}
}

void Pitch_drawInside (Pitch me, Graphics g, double xmin, double xmax, double fmin, double fmax, int speckle, int unit) {
	Sampled_drawInside (me, g, xmin, xmax, fmin, fmax, speckle, Pitch_LEVEL_FREQUENCY, unit);
}

void Pitch_draw (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax, int garnish, int speckle, int unit) {
	Graphics_setInner (g);
	Pitch_drawInside (me, g, tmin, tmax, fmin, fmax, speckle, unit);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, TRUE, L"Time (s)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		MelderString_append (& buffer, L"Pitch (", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, unit, Function_UNIT_TEXT_GRAPHICAL), L")");
		Graphics_textLeft (g, TRUE, buffer.string);
		if (Function_isUnitLogarithmic (me, Pitch_LEVEL_FREQUENCY, unit)) {
			Graphics_marksLeftLogarithmic (g, 6, TRUE, TRUE, FALSE);
		} else {
			Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		}
	}
}

void Pitch_difference (Pitch me, Pitch thee) {
	long nuvtov = 0, nvtouv = 0, ndfdown = 0, ndfup = 0;
	if (my nx != thy nx || my dx != thy dx || my x1 != thy x1) {
		Melder_flushError ("Pitch_difference: these Pitches are not aligned.");
		return;
	}
	for (long i = 1; i <= my nx; i ++) {
		double myf = my frame [i]. candidate [1]. frequency, thyf = thy frame [i]. candidate [1]. frequency;
		int myUnvoiced = myf == 0 || myf > my ceiling;
		int thyUnvoiced = thyf == 0 || thyf > thy ceiling;
		double t = Sampled_indexToX (me, i);
		if (myUnvoiced && ! thyUnvoiced) {
			Melder_casual ("Frame %ld time %f: unvoiced to voiced.", i, t);
			nuvtov ++;
		} else if (! myUnvoiced && thyUnvoiced) {
			Melder_casual ("Frame %ld time %f: voiced to unvoiced.", i, t);
			nvtouv ++;
		} else if (! myUnvoiced && ! thyUnvoiced) {
			if (myf > thyf) {
				//Melder_casual ("Frame %ld time %f: downward frequency jump from %.5g Hz to %.5g Hz.", i, t, myf, thyf);
				ndfdown ++;
			} else if (myf < thyf) {
				//Melder_casual ("Frame %ld time %f: upward frequency jump from %.5g Hz to %.5g Hz.", i, t, myf, thyf);
				ndfup ++;
			}
		}
	}
	MelderInfo_open ();
	MelderInfo_writeLine (L"Difference between two Pitches:");
	MelderInfo_writeLine (L"Unvoiced to voiced: ", Melder_integer (nuvtov), L" frames.");
	MelderInfo_writeLine (L"Voiced to unvoiced: ", Melder_integer (nvtouv), L" frames.");
	MelderInfo_writeLine (L"Downward frequency jump: ", Melder_integer (ndfdown), L" frames.");
	MelderInfo_writeLine (L"Upward frequency jump: ", Melder_integer (ndfup), L" frames.");
	MelderInfo_close ();
}

Pitch Pitch_killOctaveJumps (Pitch me) {
	try {
		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, my ceiling, 2);
		long nVoiced = 0, nUp = 0;
		double lastFrequency = 0.0;
		for (long i = 1; i <= my nx; i ++) {
			double f = my frame [i]. candidate [1]. frequency;
			thy frame [i]. candidate [1]. strength = my frame [i]. candidate [1]. strength;
			if (f > 0.0 && f < my ceiling) {
				nVoiced ++;
				if (lastFrequency != 0.0) {
					double fmin = lastFrequency * 0.7071, fmax = 2.0 * fmin;
					while (f < fmin) { f *= 2.0; nUp ++; }
					while (f > fmax) { f *= 0.5; nUp --; }
				}
				lastFrequency = thy frame [i]. candidate [1]. frequency = f;
			}
		}
		thy ceiling *= 2;   /* Make room for some octave jumps. */
		while (nUp > nVoiced / 2) {
			for (long i = 1; i <= thy nx; i ++)
				thy frame [i]. candidate [1]. frequency *= 0.5;
			nUp -= nVoiced;
		}
		while (nUp < - nVoiced / 2) {
			for (long i = 1; i <= thy nx; i ++)
				thy frame [i]. candidate [1]. frequency *= 2.0;
			nUp += nVoiced;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": octave jumps not killed.");
	}
}

Pitch Pitch_interpolate (Pitch me) {
	try {
		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, my ceiling, 2);
		for (long i = 1; i <= my nx; i ++) {
			double f = my frame [i]. candidate [1]. frequency;
			thy frame [i]. candidate [1]. strength = 0.9;
			if (f > 0.0 && f < my ceiling) {
				thy frame [i]. candidate [1]. frequency = f;
			} else {
				long left, right;
				double fleft = 0.0, fright = 0.0;
				for (left = i - 1; left >= 1 && fleft == 0.0; left --) {
					fleft = my frame [left]. candidate [1]. frequency;
					if (fleft >= my ceiling) fleft = 0.0;
				}
				for (right = i + 1; right <= my nx && fright == 0.0; right ++) {
					fright = my frame [right]. candidate [1]. frequency;
					if (fright >= my ceiling) fright = 0.0;
				}
				if (fleft && fright)
					thy frame [i]. candidate [1]. frequency =
						((i - left) * fright + (right - i) * fleft) / (right - left);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not interpolated.");
	}
}

Pitch Pitch_subtractLinearFit (Pitch me, int unit) {
	try {
		autoPitch thee = Pitch_interpolate (me);
		/*
		 * Find the first and last voiced frame.
		 */
		long imin = thy nx + 1, imax = 0;
		for (long i = 1; i <= my nx; i ++)
			if (Pitch_isVoiced_i (thee.peek(), i)) { imin = i; break; }
		for (long i = imin + 1; i <= my nx; i ++)
			if (! Pitch_isVoiced_i (thee.peek(), i)) { imax = i - 1; break; }
		long n = imax - imin + 1;
		if (n < 3) return thee.transfer();
		/*
		 * Compute average pitch and time.
		 */
		double sum = 0.0;
		for (long i = imin; i <= imax; i ++) {
			sum += Sampled_getValueAtSample (thee.peek(), i, Pitch_LEVEL_FREQUENCY, unit);
		}
		double fmean = sum / n;
		double tmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
		 * Compute slope.
		 */
		double numerator = 0.0, denominator = 0.0;
		for (long i = imin; i <= imax; i ++) {
			double t = thy x1 + (i - 1) * thy dx - tmean;
			double f = Sampled_getValueAtSample (thee.peek(), i, Pitch_LEVEL_FREQUENCY, unit) - fmean;
			numerator += f * t;
			denominator += t * t;
		}
		double slope = numerator / denominator;
		/*
		 * Modify frequencies.
		 */
		for (long i = imin; i <= imax; i ++) {
			Pitch_Frame myFrame = & my frame [i], thyFrame = & thy frame [i];
			double t = thy x1 + (i - 1) * thy dx - tmean, myFreq = FREQUENCY (myFrame);
			double f = Sampled_getValueAtSample (thee.peek(), i, Pitch_LEVEL_FREQUENCY, unit);
			f -= slope * t;
			if (NOT_VOICED (myFreq))
				FREQUENCY (thyFrame) = 0.0;
			else
				FREQUENCY (thyFrame) = Function_convertSpecialToStandardUnit (me, f, Pitch_LEVEL_FREQUENCY, unit);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": linear fit not subtracted.");
	}
}

Pitch Pitch_smooth (Pitch me, double bandWidth) {
	try {
		autoPitch interp = Pitch_interpolate (me);
		autoMatrix matrix1 = Pitch_to_Matrix (interp.peek());
		autoSound sound1 = Sound_create (1, 2 * matrix1->xmin - matrix1->xmax, 2 * matrix1->xmax - matrix1->xmin,
			3 * matrix1->nx, matrix1->dx, matrix1->x1 - 2 * matrix1->nx * matrix1->dx);

		long firstVoiced = 0, lastVoiced = 0;
		for (long i = 1; i <= matrix1 -> nx; i ++) {
			double f = matrix1 -> z [1] [i];
			if (f) {
				if (! firstVoiced) firstVoiced = i;
				lastVoiced = i;
				sound1 -> z [1] [i + matrix1 -> nx] = f;
			}
		}

		/* Extrapolate. */
		double fextrap = matrix1 -> z [1] [firstVoiced];
		firstVoiced += matrix1 -> nx;
		for (long i = 1; i < firstVoiced; i ++)
			sound1 -> z [1] [i] = fextrap;
		fextrap = matrix1 -> z [1] [lastVoiced];
		lastVoiced += matrix1 -> nx;
		for (long i = lastVoiced + 1; i <= sound1 -> nx; i ++)
			sound1 -> z [1] [i] = fextrap;

		/* Smooth. */
		autoSpectrum spectrum = Sound_to_Spectrum (sound1.peek(), TRUE);
		for (long i = 1; i <= spectrum -> nx; i ++) {
			double f = (i - 1) * spectrum -> dx, fT = f / bandWidth, factor = exp (- fT * fT);
			spectrum -> z [1] [i] *= factor;
			spectrum -> z [2] [i] *= factor;
		}
		autoSound sound2 = Spectrum_to_Sound (spectrum.peek());

		autoMatrix matrix2 = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 1, 1, 1, 1);
		for (long i = 1; i <= my nx; i ++) {
			double originalF0 = my frame [i]. candidate [1]. frequency;
			matrix2 -> z [1] [i] = originalF0 > 0.0 && originalF0 < my ceiling ?
				sound2 -> z [1] [i + matrix2 -> nx] : 0.0;
		}
		autoPitch thee = Matrix_to_Pitch (matrix2.peek());
		thy ceiling = my ceiling;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

void Pitch_step (Pitch me, double step, double precision, double tmin, double tmax) {
	Melder_assert (precision >= 0.0 && precision < 1.0);
	long imin, imax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax)) return;
	for (long i = imin; i <= imax; i ++) {
		Pitch_Frame frame = & my frame [i];
		double currentFrequency = frame -> candidate [1]. frequency;
		if (currentFrequency > 0.0 && currentFrequency < my ceiling) {
			double targetFrequency = currentFrequency * step;
			double fmin = (1 - precision) * targetFrequency;
			double fmax = (1 + precision) * targetFrequency;
			int icand, nearestCandidate = 0;
			double nearestDistance = my ceiling;
			if (fmax > my ceiling) fmax = my ceiling;
			for (icand = 2; icand <= frame -> nCandidates; icand ++) {
				double f = frame -> candidate [icand]. frequency;
				if (f > fmin && f < fmax) {
					double localDistance = fabs (f - targetFrequency);
					if (localDistance < nearestDistance) {
						nearestCandidate = icand;
						nearestDistance = localDistance;
					}
				}
			}
			if (nearestCandidate) {   /* Swap candidates. */
				struct structPitch_Candidate candidate = frame -> candidate [nearestCandidate];
				frame -> candidate [nearestCandidate] = frame -> candidate [1];
				frame -> candidate [1] = candidate;
			}
		}
	}
}

/* End of file Pitch.cpp */
