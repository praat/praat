/* Pitch.cpp
 *
 * Copyright (C) 1992-2009,2011,2012,2014-2020 Paul Boersma
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

#include "Pitch.h"
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

int structPitch :: v_getMinimumUnit (integer level) {
	return level == Pitch_LEVEL_FREQUENCY ? (int) kPitch_unit::MIN : Pitch_STRENGTH_UNIT_min;
}

int structPitch :: v_getMaximumUnit (integer level) {
	return level == Pitch_LEVEL_FREQUENCY ? (int) kPitch_unit::MAX : Pitch_STRENGTH_UNIT_max;
}

conststring32 structPitch :: v_getUnitText (integer level, int unit, uint32 flags) {
	if (level == Pitch_LEVEL_FREQUENCY) {
		return
			unit == (int) kPitch_unit::HERTZ ?
				flags & Function_UNIT_TEXT_MENU ? U"Hertz" : U"Hz" :
			unit == (int) kPitch_unit::HERTZ_LOGARITHMIC ?
				flags & Function_UNIT_TEXT_MENU ? U"Hertz (logarithmic)" : (flags & Function_UNIT_TEXT_SHORT) && (flags & Function_UNIT_TEXT_GRAPHICAL) ? U"%%Hz%" : U"Hz" :
			unit == (int) kPitch_unit::MEL ? U"mel" :
			unit == (int) kPitch_unit::LOG_HERTZ ?
				flags & Function_UNIT_TEXT_MENU ? U"logHertz" : U"logHz" :
			unit == (int) kPitch_unit::SEMITONES_1 ?
				flags & Function_UNIT_TEXT_SHORT ? U"st__1_" : flags & Function_UNIT_TEXT_GRAPHICAL ? U"semitones %%re% 1 Hz" : U"semitones re 1 Hz" :
			unit == (int) kPitch_unit::SEMITONES_100 ?
				flags & Function_UNIT_TEXT_SHORT ? U"st__100_" : flags & Function_UNIT_TEXT_GRAPHICAL ? U"semitones %%re% 100 Hz" : U"semitones re 100 Hz" :
			unit == (int) kPitch_unit::SEMITONES_200 ?
				flags & Function_UNIT_TEXT_SHORT ? U"st__200_" : flags & Function_UNIT_TEXT_GRAPHICAL ? U"semitones %%re% 200 Hz" : U"semitones re 200 Hz" :
			unit == (int) kPitch_unit::SEMITONES_440 ?
				flags & Function_UNIT_TEXT_SHORT ? U"st__a_" : flags & Function_UNIT_TEXT_GRAPHICAL ? U"semitones %%re% 440 Hz" : U"semitones re 440 Hz" :
			unit == (int) kPitch_unit::ERB ?
				flags & Function_UNIT_TEXT_SHORT ? U"erb" : U"ERB" :
			U"";
	} else if (level == Pitch_LEVEL_STRENGTH) {
		return
			unit == Pitch_STRENGTH_UNIT_AUTOCORRELATION ? U"" :
			unit == Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO ? U"" :
			unit == Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB ? U"dB" :
			U"";
	}
	return U"unknown";
}

bool structPitch :: v_isUnitLogarithmic (integer level, int unit) {
	return level == Pitch_LEVEL_FREQUENCY && unit == (int) kPitch_unit::HERTZ_LOGARITHMIC;
}

double structPitch :: v_convertStandardToSpecialUnit (double value, integer level, int unit) {
	if (level == Pitch_LEVEL_FREQUENCY) {
		return
			unit == (int) kPitch_unit::HERTZ ? value :
			unit == (int) kPitch_unit::HERTZ_LOGARITHMIC ? value <= 0.0 ? undefined : log10 (value) :
			unit == (int) kPitch_unit::MEL ? NUMhertzToMel (value) :
			unit == (int) kPitch_unit::LOG_HERTZ ? value <= 0.0 ? undefined : log10 (value) :
			unit == (int) kPitch_unit::SEMITONES_1 ? value <= 0.0 ? undefined : (12.0 / NUMln2) * log (value / 1.0) :
			unit == (int) kPitch_unit::SEMITONES_100 ? value <= 0.0 ? undefined : (12.0 / NUMln2) * log (value / 100.0) :
			unit == (int) kPitch_unit::SEMITONES_200 ? value <= 0.0 ? undefined : (12.0 / NUMln2) * log (value / 200.0) :
			unit == (int) kPitch_unit::SEMITONES_440 ? value <= 0.0 ? undefined : (12.0 / NUMln2) * log (value / 440.0) :
			unit == (int) kPitch_unit::ERB ? NUMhertzToErb (value) :
			undefined;
	} else {
		return
			unit == Pitch_STRENGTH_UNIT_AUTOCORRELATION ? value :
			unit == Pitch_STRENGTH_UNIT_NOISE_HARMONICS_RATIO ?
				value <= 1e-15 ? 1e15 : value > 1.0 - 1e-15 ? 1e-15 : (1.0 - value) / value :   // before losing precision
			unit == Pitch_STRENGTH_UNIT_HARMONICS_NOISE_DB ?
				value <= 1e-15 ? -150.0 : value > 1.0 - 1e-15 ? 150.0 : 10.0 * log10 (value / (1.0 - value)) :   // before losing precision
			undefined;
	}
}

double structPitch :: v_convertSpecialToStandardUnit (double value, integer level, int unit) {
	if (level == Pitch_LEVEL_FREQUENCY) {
		return
			unit == (int) kPitch_unit::HERTZ ? value :
			unit == (int) kPitch_unit::HERTZ_LOGARITHMIC ? pow (10.0, value) :
			unit == (int) kPitch_unit::MEL ? NUMmelToHertz (value) :
			unit == (int) kPitch_unit::LOG_HERTZ ? pow (10.0, value) :
			unit == (int) kPitch_unit::SEMITONES_1 ? 1.0 * exp (value * (NUMln2 / 12.0)):
			unit == (int) kPitch_unit::SEMITONES_100 ? 100.0 * exp (value * (NUMln2 / 12.0)):
			unit == (int) kPitch_unit::SEMITONES_200 ? 200.0 * exp (value * (NUMln2 / 12.0)):
			unit == (int) kPitch_unit::SEMITONES_440 ? 440.0 * exp (value * (NUMln2 / 12.0)):
			unit == (int) kPitch_unit::ERB ? NUMerbToHertz (value) :
			undefined;
	} else {
		return undefined;
	}
}

#define doesUnitAllowNegativeValues(unit)  \
	( (unit) == kPitch_unit::HERTZ_LOGARITHMIC || (unit) == kPitch_unit::LOG_HERTZ ||  \
	  (unit) == kPitch_unit::SEMITONES_1 || (unit) == kPitch_unit::SEMITONES_100 ||  \
	  (unit) == kPitch_unit::SEMITONES_200 || (unit) == kPitch_unit::SEMITONES_440 )

double structPitch :: v_getValueAtSample (integer iframe, integer ilevel, int unit) {
	const double f = our frames [iframe]. candidates [1]. frequency;
	if (! Pitch_util_frequencyIsVoiced (f, our ceiling))
		return undefined;
	return v_convertStandardToSpecialUnit (ilevel == Pitch_LEVEL_FREQUENCY ? f :
			our frames [iframe]. candidates [1]. strength, ilevel, unit);
}

bool Pitch_isVoiced_i (Pitch me, integer iframe) {
	return isdefined (Sampled_getValueAtSample (me, iframe, Pitch_LEVEL_FREQUENCY, (int) kPitch_unit::HERTZ));
}

bool Pitch_isVoiced_t (Pitch me, double time) {
	return isdefined (Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, (int) kPitch_unit::HERTZ, false));
}

double Pitch_getValueAtTime (Pitch me, double time, kPitch_unit unit, bool interpolate) {
	return Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, (int) unit, interpolate);
}

double Pitch_getStrengthAtTime (Pitch me, double time, kPitch_unit unit, bool interpolate) {
	return Sampled_getValueAtX (me, time, Pitch_LEVEL_STRENGTH, (int) unit, interpolate);
}

integer Pitch_countVoicedFrames (Pitch me) {
	return Sampled_countDefinedSamples (me, 0.0, 0.0, Pitch_LEVEL_FREQUENCY, (int) kPitch_unit::HERTZ);
}

double Pitch_getMean (Pitch me, double tmin, double tmax, kPitch_unit unit) {
	return Sampled_getMean (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, (int) unit, true);
}

double Pitch_getMeanStrength (Pitch me, double tmin, double tmax, int strengthUnit) {
	return Sampled_getMean (me, tmin, tmax, Pitch_LEVEL_STRENGTH, strengthUnit, true);
}

double Pitch_getQuantile (Pitch me, double tmin, double tmax, double quantile, kPitch_unit unit) {
	double value = Sampled_getQuantile (me, tmin, tmax, quantile, Pitch_LEVEL_FREQUENCY, (int) unit);
	if (value <= 0.0 && ! doesUnitAllowNegativeValues (unit))
		value = undefined;
	return value;
}

double Pitch_getStandardDeviation (Pitch me, double tmin, double tmax, kPitch_unit unit) {
	return Sampled_getStandardDeviation (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, (int) unit, true);
}

void Pitch_getMaximumAndTime (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate,
	double *return_maximum, double *return_timeOfMaximum)
{
	Sampled_getMaximumAndX (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, (int) unit, interpolate, return_maximum, return_timeOfMaximum);
	if (! doesUnitAllowNegativeValues (unit) && return_maximum && *return_maximum <= 0.0)
	{
		*return_maximum = undefined;   // unlikely
	}
}

double Pitch_getMaximum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate) {
	double maximum;
	Pitch_getMaximumAndTime (me, tmin, tmax, unit, interpolate, & maximum, nullptr);
	return maximum;
}

double Pitch_getTimeOfMaximum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate) {
	double time;
	Pitch_getMaximumAndTime (me, tmin, tmax, unit, interpolate, nullptr, & time);
	return time;
}

void Pitch_getMinimumAndTime (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate,
	double *return_minimum, double *return_timeOfMinimum)
{
	Sampled_getMinimumAndX (me, tmin, tmax, Pitch_LEVEL_FREQUENCY, (int) unit, interpolate, return_minimum, return_timeOfMinimum);
	if (! doesUnitAllowNegativeValues (unit) && return_minimum && *return_minimum <= 0.0)
	{
		*return_minimum = undefined;   // not so unlikely
	}
}

double Pitch_getMinimum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate) {
	double minimum;
	Pitch_getMinimumAndTime (me, tmin, tmax, unit, interpolate, & minimum, nullptr);
	return minimum;
}

double Pitch_getTimeOfMinimum (Pitch me, double tmin, double tmax, kPitch_unit unit, bool interpolate) {
	double time;
	Pitch_getMinimumAndTime (me, tmin, tmax, unit, interpolate, nullptr, & time);
	return time;
}

static integer Pitch_getMeanAbsoluteSlope (Pitch me,
	double *out_hertz, double *out_mel, double *out_semitones, double *out_erb, double *out_withoutOctaveJumps)
{
	integer firstVoicedFrame = 0, lastVoicedFrame = 0, numberOfVoicedFrames = 0;
	autoVEC frequencies = raw_VEC (my nx);
	for (integer i = 1; i <= my nx; i ++) {
		const double frequency = my frames [i]. candidates [1]. frequency;
		if (Pitch_util_frequencyIsVoiced (frequency, my ceiling)) {
			frequencies [i] = frequency;
			numberOfVoicedFrames ++;
		} else {
			frequencies [i] = 0.0;
		}
	}
	for (integer i = 1; i <= my nx; i ++)   // look for first voiced frame
		if (frequencies [i] != 0.0) {
			firstVoicedFrame = i;
			break;
		}
	for (integer i = my nx; i >= 1; i --)   // look for last voiced frame
		if (frequencies [i] != 0.0) {
			lastVoicedFrame = i;
			break;
		}
	if (numberOfVoicedFrames > 1) {
		integer ilast = firstVoicedFrame;
		double flast = frequencies [ilast];
		const double span = (lastVoicedFrame - firstVoicedFrame) * my dx;
		longdouble slopeHz = 0.0, slopeMel = 0.0, slopeSemitones = 0.0, slopeErb = 0.0, slopeRobust = 0.0;
		for (integer i = firstVoicedFrame + 1; i <= lastVoicedFrame; i ++) if (frequencies [i] != 0.0) {
			double localStepSemitones = fabs (NUMhertzToSemitones (frequencies [i]) - NUMhertzToSemitones (flast));
			slopeHz += fabs (frequencies [i] - flast);
			slopeMel += fabs (NUMhertzToMel (frequencies [i]) - NUMhertzToMel (flast));
			slopeSemitones += localStepSemitones;
			slopeErb += fabs (NUMhertzToErb (frequencies [i]) - NUMhertzToErb (flast));
			while (localStepSemitones >= 12.0)
				localStepSemitones -= 12.0;   // kill octave jumps
			if (localStepSemitones > 6.0)
				localStepSemitones = 12.0 - localStepSemitones;
			slopeRobust += localStepSemitones;
			ilast = i;
			flast = frequencies [ilast];
		}
		if (out_hertz)
			*out_hertz = double (slopeHz / span);
		if (out_mel)
			*out_mel = double (slopeMel / span);
		if (out_semitones)
			*out_semitones = double (slopeSemitones / span);
		if (out_erb)
			*out_erb = double (slopeErb / span);
		if (out_withoutOctaveJumps)
			*out_withoutOctaveJumps = double (slopeRobust / span);
	} else {
		if (out_hertz)
			*out_hertz = undefined;
		if (out_mel)
			*out_mel = undefined;
		if (out_semitones)
			*out_semitones = undefined;
		if (out_erb)
			*out_erb = undefined;
		if (out_withoutOctaveJumps)
			*out_withoutOctaveJumps = undefined;
	}
	return numberOfVoicedFrames;
}

integer Pitch_getMeanAbsSlope_hertz (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, slope, nullptr, nullptr, nullptr, nullptr);
}

integer Pitch_getMeanAbsSlope_mel (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, nullptr, slope, nullptr, nullptr, nullptr);
}

integer Pitch_getMeanAbsSlope_semitones (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, nullptr, nullptr, slope, nullptr, nullptr);
}

integer Pitch_getMeanAbsSlope_erb (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, nullptr, nullptr, nullptr, slope, nullptr);
}

integer Pitch_getMeanAbsSlope_noOctave (Pitch me, double *slope) {
	return Pitch_getMeanAbsoluteSlope (me, nullptr, nullptr, nullptr, nullptr, slope);
}

MelderFraction Pitch_getFractionOfLocallyVoicedFrames (
	Pitch me, double tmin, double tmax, double ceiling, double silenceThreshold, double voicingThreshold)
{
	MelderFraction result;
	integer imin, imax;
	result.denominator = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	for (integer i = imin; i <= imax; i ++) {
		const Pitch_Frame frame = & my frames [i];
		if (frame -> intensity >= silenceThreshold) {
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++) {
				const Pitch_Candidate cand = & frame -> candidates [icand];
				if (cand -> frequency > 0.0 && cand -> frequency < ceiling && cand -> strength >= voicingThreshold) {
					result.numerator += 1.0;
					break;   // next frame
				}
			}
		}
	}
	return result;
}

MelderFraction Pitch_getFractionOfLocallyUnvoicedFrames (
	Pitch me, double tmin, double tmax, double ceiling, double silenceThreshold, double voicingThreshold)
{
	MelderFraction fraction = Pitch_getFractionOfLocallyVoicedFrames (me, tmin, tmax, ceiling, silenceThreshold, voicingThreshold);
	fraction.numerator = fraction.denominator - fraction.numerator;
	return fraction;
}

void structPitch :: v_info () {
	autoVEC frequencies = Sampled_getSortedValues (this, 0.0, 0.0, Pitch_LEVEL_FREQUENCY, (int) kPitch_unit::HERTZ);
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", our nx, U" (", frequencies.size, U" voiced)");
	MelderInfo_writeLine (U"   Time step: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", our x1, U" seconds");
	MelderInfo_writeLine (U"Ceiling at: ", our ceiling, U" Hz");

	if (frequencies.size > 0) {   // quantiles
		double quantile10 = NUMquantile (frequencies.get(), 0.10);
		double quantile16 = NUMquantile (frequencies.get(), 0.16);
		double quantile50 = NUMquantile (frequencies.get(), 0.50);   // median
		double quantile84 = NUMquantile (frequencies.get(), 0.84);
		double quantile90 = NUMquantile (frequencies.get(), 0.90);
		MelderInfo_writeLine (U"\nEstimated quantiles:");
		MelderInfo_writeLine (U"   10% = ",
				Melder_single (quantile10), U" Hz = ",
				Melder_single (NUMhertzToMel (quantile10)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (quantile10)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (quantile10)), U" ERB");
		MelderInfo_writeLine (U"   16% = ",
				Melder_single (quantile16), U" Hz = ",
				Melder_single (NUMhertzToMel (quantile16)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (quantile16)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (quantile16)), U" ERB");
		MelderInfo_writeLine (U"   50% = ",
				Melder_single (quantile50), U" Hz = ",
				Melder_single (NUMhertzToMel (quantile50)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (quantile50)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (quantile50)), U" ERB");
		MelderInfo_writeLine (U"   84% = ",
				Melder_single (quantile84), U" Hz = ",
				Melder_single (NUMhertzToMel (quantile84)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (quantile84)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (quantile84)), U" ERB");
		MelderInfo_write (U"   90% = ",
				Melder_single (quantile90), U" Hz = ",
				Melder_single (NUMhertzToMel (quantile90)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (quantile90)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (quantile90)), U" ERB");
		if (frequencies.size > 1) {
			double correction = sqrt (frequencies.size / (frequencies.size - 1.0));
			MelderInfo_writeLine (U"\nEstimated spreading:");
			MelderInfo_writeLine (U"   84%-median = ",
					Melder_half ((quantile84 - quantile50) * correction), U" Hz = ",
					Melder_half ((NUMhertzToMel (quantile84) - NUMhertzToMel (quantile50)) * correction), U" Mel = ",
					Melder_half ((NUMhertzToSemitones (quantile84) - NUMhertzToSemitones (quantile50)) * correction), U" semitones = ",
					Melder_half ((NUMhertzToErb (quantile84) - NUMhertzToErb (quantile50)) * correction), U" ERB");
			MelderInfo_writeLine (U"   median-16% = ",
					Melder_half ((quantile50 - quantile16) * correction), U" Hz = ",
					Melder_half ((NUMhertzToMel (quantile50) - NUMhertzToMel (quantile16)) * correction), U" Mel = ",
					Melder_half ((NUMhertzToSemitones (quantile50) - NUMhertzToSemitones (quantile16)) * correction), U" semitones = ",
					Melder_half ((NUMhertzToErb (quantile50) - NUMhertzToErb (quantile16)) * correction), U" ERB");
			MelderInfo_writeLine (U"   90%-10% = ",
					Melder_half ((quantile90 - quantile10) * correction), U" Hz = ",
					Melder_half ((NUMhertzToMel (quantile90) - NUMhertzToMel (quantile10)) * correction), U" Mel = ",
					Melder_half ((NUMhertzToSemitones (quantile90) - NUMhertzToSemitones (quantile10)) * correction), U" semitones = ",
					Melder_half ((NUMhertzToErb (quantile90) - NUMhertzToErb (quantile10)) * correction), U" ERB");
		}
	}
	if (frequencies.size > 0) {   // extrema, range, mean and standard deviation
		double minimum = Pitch_getMinimum (this, xmin, xmax, kPitch_unit::HERTZ, false);
		double maximum = Pitch_getMaximum (this, xmin, xmax, kPitch_unit::HERTZ, false);
		MelderInfo_writeLine (U"\nMinimum ",
				Melder_single (minimum), U" Hz = ",
				Melder_single (NUMhertzToMel (minimum)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (minimum)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (minimum)), U" ERB");
		MelderInfo_writeLine (U"Maximum ",
				Melder_single (maximum), U" Hz = ",
				Melder_single (NUMhertzToMel (maximum)), U" Mel = ",
				Melder_single (NUMhertzToSemitones (maximum)), U" semitones above 100 Hz = ",
				Melder_single (NUMhertzToErb (maximum)), U" ERB");
		MelderInfo_writeLine (U"Range ",
				Melder_half (maximum - minimum), U" Hz = ",
				Melder_single (NUMhertzToMel (maximum) - NUMhertzToMel (minimum)), U" Mel = ",
				Melder_half (NUMhertzToSemitones (maximum) - NUMhertzToSemitones (minimum)), U" semitones = ",
				Melder_half (NUMhertzToErb (maximum) - NUMhertzToErb (minimum)), U" ERB");
		double meanHertz = Pitch_getMean (this, 0, 0, kPitch_unit::HERTZ);
		double meanMel = Pitch_getMean (this, 0, 0, kPitch_unit::MEL);
		double meanSemitones = Pitch_getMean (this, 0, 0, kPitch_unit::SEMITONES_100);
		double meanErb = Pitch_getMean (this, 0, 0, kPitch_unit::ERB);
		MelderInfo_writeLine (U"Average: ",
				Melder_single (meanHertz), U" Hz = ",
				Melder_single (meanMel), U" Mel = ",
				Melder_single (meanSemitones), U" semitones above 100 Hz = ",
				Melder_single (meanErb), U" ERB");
		if (frequencies.size > 1) {
			double stdevHertz = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit::HERTZ);
			double stdevMel = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit::MEL);
			double stdevSemitones = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit::SEMITONES_100);
			double stdevErb = Pitch_getStandardDeviation (this, 0, 0, kPitch_unit::ERB);
			MelderInfo_writeLine (U"Standard deviation: ",
					Melder_half (stdevHertz), U" Hz = ",
					Melder_half (stdevMel), U" Mel = ",
					Melder_half (stdevSemitones), U" semitones = ",
					Melder_half (stdevErb), U" ERB");
		}
	}
	if (frequencies.size > 0) {   // variability: mean absolute slope
		double slopeHertz, slopeMel, slopeSemitones, slopeErb, slopeWithoutOctaveJumps;
		Pitch_getMeanAbsoluteSlope (this, & slopeHertz, & slopeMel, & slopeSemitones, & slopeErb, & slopeWithoutOctaveJumps);
		MelderInfo_writeLine (U"\nMean absolute slope: ",
				Melder_half (slopeHertz), U" Hz/s = ",
				Melder_half (slopeMel), U" Mel/s = ",
				Melder_half (slopeSemitones), U" semitones/s = ",
				Melder_half (slopeErb), U" ERB/s");
		MelderInfo_writeLine (U"Mean absolute slope without octave jumps: ", Melder_half (slopeWithoutOctaveJumps), U" semitones/s");
	}
}

void Pitch_Frame_init (Pitch_Frame me, integer numberOfCandidates) {
	my candidates = newvectorzero <structPitch_Candidate> (numberOfCandidates);
	my nCandidates = numberOfCandidates;   // maintain invariant
}

autoPitch Pitch_create (double tmin, double tmax, integer nt, double dt, double t1,
	double ceiling, integer maxnCandidates)
{
	try {
		autoPitch me = Thing_new (Pitch);
		Sampled_init (me.get(), tmin, tmax, nt, dt, t1);
		my ceiling = ceiling;
		my maxnCandidates = maxnCandidates;
		my frames = newvectorzero <structPitch_Frame> (nt);

		/*
			Put one candidate in every frame (unvoiced, silent).
		*/
		for (integer it = 1; it <= nt; it ++)
			Pitch_Frame_init (& my frames [it], 1);

		return me;
	} catch (MelderError) {
		Melder_throw (U"Pitch not created.");
	}
}

void Pitch_setCeiling (Pitch me, double ceiling) {
	my ceiling = ceiling;
}

integer Pitch_getMaxnCandidates (Pitch me) {
	integer result = 0;
	for (integer i = 1; i <= my nx; i ++) {
		integer numberOfCandidates = my frames [i]. nCandidates;
		if (numberOfCandidates > result)
			result = numberOfCandidates;
	}
	return result;
}

autoMAT Pitch_Frame_getAllCandidates (Pitch_Frame me) {
	integer numberOfCandidates = my nCandidates;
	autoMAT candidates = raw_MAT (2, numberOfCandidates);
	for (integer icand = 1; icand <= numberOfCandidates; icand ++) {
		candidates [1] [icand] = my candidates [icand]. frequency;
		candidates [2] [icand] = my candidates [icand]. strength;
	}
	return candidates;
}

autoMAT Pitch_getAllCandidatesInFrame (Pitch me, integer frameNumber) {
	try {
    	my checkIndex (frameNumber);
    	const Pitch_Frame frame = & my frames [frameNumber];
    	return Pitch_Frame_getAllCandidates (frame);
	} catch (MelderError) {
		Melder_throw (U"Pitch candidate matrix not created.");
	}
}

void Pitch_pathFinder (Pitch me, double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost,
	double ceiling, int pullFormants)
{
	if (Melder_debug == 33)
		Melder_casual (U"Pitch path finder:"
			U"\nSilence threshold = ", silenceThreshold,
			U"\nVoicing threshold = ", voicingThreshold,
			U"\nOctave cost = ", octaveCost,
			U"\nOctave jump cost = ", octaveJumpCost,
			U"\nVoiced/unvoiced cost = ", voicedUnvoicedCost,
			U"\nCeiling = ", ceiling,
			U"\nPull formants = ", pullFormants);
	try {
		const integer maxnCandidates = Pitch_getMaxnCandidates (me);
		integer place;
		volatile double maximum, value;
		const double ceiling2 = ( pullFormants ? 2.0 * ceiling : ceiling );
		/* Next three lines 20011015 */
		const double timeStepCorrection = 0.01 / my dx;
		octaveJumpCost *= timeStepCorrection;
		voicedUnvoicedCost *= timeStepCorrection;

		my ceiling = ceiling;
		autoMAT delta = zero_MAT (my nx, maxnCandidates);
		autoINTMAT psi = zero_INTMAT (my nx, maxnCandidates);

		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Pitch_Frame frame = & my frames [iframe];
			double unvoicedStrength = ( silenceThreshold <= 0 ? 0.0 :
				2.0 - frame -> intensity / (silenceThreshold / (1.0 + voicingThreshold)) );
			unvoicedStrength = voicingThreshold + std::max (0.0, unvoicedStrength);
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++) {
				const Pitch_Candidate candidate = & frame -> candidates [icand];
				const bool voiceless = ! Pitch_util_frequencyIsVoiced (candidate -> frequency, ceiling2);
				delta [iframe] [icand] = ( voiceless ? unvoicedStrength :
					candidate -> strength - octaveCost * NUMlog2 (ceiling / candidate -> frequency) );
			}
		}

		/* Look for the most probable path through the maxima. */
		/* There is a cost for the voiced/unvoiced transition, */
		/* and a cost for a frequency jump. */

		for (integer iframe = 2; iframe <= my nx; iframe ++) {
			const Pitch_Frame prevFrame = & my frames [iframe - 1], curFrame = & my frames [iframe];
			const constVEC prevDelta = delta [iframe - 1];
			const VEC curDelta = delta [iframe];
			const INTVEC curPsi = psi [iframe];
			for (integer icand2 = 1; icand2 <= curFrame -> nCandidates; icand2 ++) {
				const double f2 = curFrame -> candidates [icand2]. frequency;
				maximum = -1e30;
				place = 0;
				for (integer icand1 = 1; icand1 <= prevFrame -> nCandidates; icand1 ++) {
					double f1 = prevFrame -> candidates [icand1]. frequency;
					double transitionCost;
					const bool previousVoiceless = ! Pitch_util_frequencyIsVoiced (f1, ceiling2);
					const bool currentVoiceless = ! Pitch_util_frequencyIsVoiced (f2, ceiling2);
					if (currentVoiceless) {
						if (previousVoiceless) {
							transitionCost = 0.0;   // both voiceless
						} else {
							transitionCost = voicedUnvoicedCost;   // voiced-to-unvoiced transition
						}
					} else {
						if (previousVoiceless) {
							transitionCost = voicedUnvoicedCost;   // unvoiced-to-voiced transition
							if (Melder_debug == 30) {
								/*
									Try to take into account a frequency jump across a voiceless stretch.
								*/
								integer place1 = icand1;
								for (integer jframe = iframe - 2; jframe >= 1; jframe --) {
									place1 = psi [jframe + 1] [place1];
									f1 = my frames [jframe]. candidates [place1]. frequency;
									if (Pitch_util_frequencyIsVoiced (f1, ceiling)) {
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
					//if (Melder_debug == 33) Melder_casual (U"Frame ", iframe, U", current candidate ", icand2,
					//  U" (delta ", curDelta [icand2], U"), previous candidate ", icand1, U" (delta ", prevDelta [icand1], U"), ",
					//	U"transition cost ", transitionCost, U", value ", value, U", maximum ", maximum);
					if (value > maximum) {
						maximum = value;
						place = icand1;
					} else if (value == maximum) {
						if (Melder_debug == 33)
							Melder_casual (
								U"A tie in frame ", iframe,
								U", current candidate ", icand2,
								U", previous candidate ", icand1
							);
					}
				}
				curDelta [icand2] = maximum;
				curPsi [icand2] = place;
			}
		}

		/* Find the end of the most probable path. */

		place = 1;
		maximum = delta [my nx] [place];
		for (integer icand = 2; icand <= my frames [my nx]. nCandidates; icand ++) {
			if (delta [my nx] [icand] > maximum) {
				place = icand;
				maximum = delta [my nx] [place];
			}
		}

		/* Backtracking: follow the path backwards. */

		for (integer iframe = my nx; iframe >= 1; iframe --) {
			if (Melder_debug == 33)
				Melder_casual (
					U"Frame ", iframe, U":",
					U" swapping candidates 1 and ", place
				);
			const Pitch_Frame frame = & my frames [iframe];
			std::swap (frame -> candidates [1], frame -> candidates [place]);
			place = psi [iframe] [place];
		}

		/* Pull formants: devoice frames with frequencies between ceiling and ceiling2. */

		if (ceiling2 > ceiling) {
			if (Melder_debug == 33)
				Melder_casual (U"Pulling formants...");
			for (integer iframe = my nx; iframe >= 1; iframe --) {
				const Pitch_Frame frame = & my frames [iframe];
				const Pitch_Candidate winner = & frame -> candidates [1];
				const double f = winner -> frequency;
				if (f > ceiling && f < ceiling2) {
					for (integer icand = 2; icand <= frame -> nCandidates; icand ++) {
						const Pitch_Candidate loser = & frame -> candidates [icand];
						if (loser -> frequency == 0.0) {
							std::swap (* winner, * loser);
							break;
						}
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": path not found.");
	}
}

void Pitch_drawInside (Pitch me, Graphics g, double xmin, double xmax, double fmin, double fmax, bool speckle, kPitch_unit unit) {
	Sampled_drawInside (me, g, xmin, xmax, fmin, fmax, speckle, Pitch_LEVEL_FREQUENCY, (int) unit);
}

void Pitch_draw (Pitch me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish, bool speckle, kPitch_unit unit) {
	Graphics_setInner (g);
	Pitch_drawInside (me, g, tmin, tmax, fmin, fmax, speckle, unit);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, Melder_cat (U"Pitch (", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, Function_UNIT_TEXT_GRAPHICAL), U")"));
		if (Function_isUnitLogarithmic (me, Pitch_LEVEL_FREQUENCY, (int) unit)) {
			Graphics_marksLeftLogarithmic (g, 6, true, true, false);
		} else {
			Graphics_marksLeft (g, 2, true, true, false);
		}
	}
}

void Pitch_difference (Pitch me, Pitch thee) {
	integer nuvtov = 0, nvtouv = 0, ndfdown = 0, ndfup = 0;
	if (my nx != thy nx || my dx != thy dx || my x1 != thy x1) {
		Melder_flushError (U"Pitch_difference: these Pitches are not aligned.");
		return;
	}
	for (integer i = 1; i <= my nx; i ++) {
		const double myf = my frames [i]. candidates [1]. frequency, thyf = thy frames [i]. candidates [1]. frequency;
		const bool myUnvoiced = ! Pitch_util_frequencyIsVoiced (myf, my ceiling);
		const bool thyUnvoiced = ! Pitch_util_frequencyIsVoiced (thyf, thy ceiling);
		const double t = Sampled_indexToX (me, i);
		if (myUnvoiced && ! thyUnvoiced) {
			Melder_casual (
				U"Frame ", i,
				U" time ", t,
				U": unvoiced to voiced."
			);
			nuvtov ++;
		} else if (! myUnvoiced && thyUnvoiced) {
			Melder_casual (
				U"Frame ", i,
				U" time ", t,
				U": voiced to unvoiced."
			);
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
	MelderInfo_writeLine (U"Difference between two Pitches:");
	MelderInfo_writeLine (U"Unvoiced to voiced: ", nuvtov, U" frames.");
	MelderInfo_writeLine (U"Voiced to unvoiced: ", nvtouv, U" frames.");
	MelderInfo_writeLine (U"Downward frequency jump: ", ndfdown, U" frames.");
	MelderInfo_writeLine (U"Upward frequency jump: ", ndfup, U" frames.");
	MelderInfo_close ();
}

autoPitch Pitch_killOctaveJumps (Pitch me) {
	try {
		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, my ceiling, 2);
		integer nVoiced = 0, nUp = 0;
		double lastFrequency = 0.0;
		for (integer i = 1; i <= my nx; i ++) {
			double f = my frames [i]. candidates [1]. frequency;
			thy frames [i]. candidates [1]. strength = my frames [i]. candidates [1]. strength;
			if (Pitch_util_frequencyIsVoiced (f, my ceiling)) {
				nVoiced ++;
				if (lastFrequency != 0.0) {
					const double fmin = lastFrequency * 0.7071, fmax = 2.0 * fmin;
					while (f < fmin) {
						f *= 2.0;
						nUp ++;
					}
					while (f > fmax) {
						f *= 0.5;
						nUp --;
					}
				}
				lastFrequency = thy frames [i]. candidates [1]. frequency = f;
			}
		}
		thy ceiling *= 2.0;   // make room for some octave jumps
		while (nUp > nVoiced / 2) {
			for (integer i = 1; i <= thy nx; i ++)
				thy frames [i]. candidates [1]. frequency *= 0.5;
			nUp -= nVoiced;
		}
		while (nUp < - nVoiced / 2) {
			for (integer i = 1; i <= thy nx; i ++)
				thy frames [i]. candidates [1]. frequency *= 2.0;
			nUp += nVoiced;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": octave jumps not killed.");
	}
}

autoPitch Pitch_interpolate (Pitch me) {
	try {
		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, my ceiling, 2);
		for (integer i = 1; i <= my nx; i ++) {
			const double f = my frames [i]. candidates [1]. frequency;
			thy frames [i]. candidates [1]. strength = 0.9;
			if (Pitch_util_frequencyIsVoiced (f, my ceiling)) {
				thy frames [i]. candidates [1]. frequency = f;
			} else {
				integer left, right;
				double fleft = 0.0, fright = 0.0;
				for (left = i - 1; left >= 1 && fleft == 0.0; left --) {
					fleft = my frames [left]. candidates [1]. frequency;
					if (fleft >= my ceiling)
						fleft = 0.0;
				}
				for (right = i + 1; right <= my nx && fright == 0.0; right ++) {
					fright = my frames [right]. candidates [1]. frequency;
					if (fright >= my ceiling)
						fright = 0.0;
				}
				if (fleft != 0.0 && fright != 0.0)
					thy frames [i]. candidates [1]. frequency =
						((i - left) * fright + (right - i) * fleft) / (right - left);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not interpolated.");
	}
}

autoPitch Pitch_subtractLinearFit (Pitch me, kPitch_unit unit) {
	try {
		autoPitch thee = Pitch_interpolate (me);
		/*
			Find the first and last voiced frame.
		*/
		integer imin = thy nx + 1, imax = 0;
		for (integer i = 1; i <= my nx; i ++)
			if (Pitch_isVoiced_i (thee.get(), i)) {
				imin = i;
				break;
			}
		for (integer i = imin + 1; i <= my nx; i ++)
			if (! Pitch_isVoiced_i (thee.get(), i)) {
				imax = i - 1;
				break;
			}
		integer n = imax - imin + 1;
		if (n <= 1) {
			/*
				If there is only one point, there is no slope to subtract.
				The result is the original frequency if the original was voiced, else 0.0 Hz.
				The interpolated Pitch already has these properties set correctly.
			*/
			return thee;
		}
		/*
			Compute average pitch and time.
		*/
		longdouble sum = 0.0;
		for (integer i = imin; i <= imax; i ++)
			sum += Sampled_getValueAtSample (thee.get(), i, Pitch_LEVEL_FREQUENCY, (int) unit);
		double fmean = double (sum) / n;
		double tmean = thy x1 + (0.5 * (imin + imax) - 1) * thy dx;
		/*
			Compute slope.
		*/
		double numerator = 0.0, denominator = 0.0;
		for (integer i = imin; i <= imax; i ++) {
			const double t = thy x1 + (i - 1) * thy dx - tmean;
			const double f = Sampled_getValueAtSample (thee.get(), i, Pitch_LEVEL_FREQUENCY, (int) unit)
					- fmean;
			numerator += f * t;
			denominator += t * t;
		}
		double slope = numerator / denominator;
		/*
			Modify frequencies.
		*/
		for (integer i = imin; i <= imax; i ++) {
			const Pitch_Frame myFrame = & my frames [i], thyFrame = & thy frames [i];
			const Pitch_Candidate myCandidate = & myFrame -> candidates [1], thyCandidate = & thyFrame -> candidates [1];
			const double t = thy x1 + (i - 1) * thy dx - tmean, myFreq = myCandidate -> frequency;
			const double f = Sampled_getValueAtSample (thee.get(), i, Pitch_LEVEL_FREQUENCY, (int) unit)
					- slope * t;
			if (Pitch_util_frequencyIsVoiced (myFreq, my ceiling))
				thyCandidate -> frequency = Function_convertSpecialToStandardUnit (me, f, Pitch_LEVEL_FREQUENCY, (int) unit);
			else
				thyCandidate -> frequency = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": linear fit not subtracted.");
	}
}

autoPitch Pitch_smooth (Pitch me, double bandWidth) {
	try {
		autoPitch interp = Pitch_interpolate (me);
		autoMatrix matrix1 = Pitch_to_Matrix (interp.get());
		const double logicalDuration = matrix1 -> xmax - matrix1 -> xmin;
		const double physicalDuration = matrix1 -> nx * matrix1 -> dx;
		autoSound sound1 = Sound_create (
			1,   // one channel
			matrix1 -> xmin - logicalDuration,   // triply wide logical time domain (ignored)
			matrix1 -> xmax + logicalDuration,   // triply wide logical time domain (ignored)
			3 * matrix1 -> nx,   // triply wide physical time domain
			matrix1 -> dx,
			matrix1 -> x1 - physicalDuration   // (ignored)
		);

		integer firstVoiced = 0, lastVoiced = 0;
		for (integer i = 1; i <= matrix1 -> nx; i ++) {
			const double f = matrix1 -> z [1] [i];
			if (f != 0.0) {
				if (! firstVoiced)
					firstVoiced = i;
				lastVoiced = i;
				sound1 -> z [1] [i + matrix1 -> nx] = f;
			}
		}

		/* Extrapolate. */
		double fextrap = matrix1 -> z [1] [firstVoiced];
		firstVoiced += matrix1 -> nx;
		for (integer i = 1; i < firstVoiced; i ++)
			sound1 -> z [1] [i] = fextrap;
		fextrap = matrix1 -> z [1] [lastVoiced];
		lastVoiced += matrix1 -> nx;
		for (integer i = lastVoiced + 1; i <= sound1 -> nx; i ++)
			sound1 -> z [1] [i] = fextrap;

		/* Smooth. */
		autoSpectrum spectrum = Sound_to_Spectrum (sound1.get(), true);
		for (integer i = 1; i <= spectrum -> nx; i ++) {
			double f = (i - 1) * spectrum -> dx, fT = f / bandWidth, factor = exp (- fT * fT);
			spectrum -> z [1] [i] *= factor;
			spectrum -> z [2] [i] *= factor;
		}
		autoSound sound2 = Spectrum_to_Sound (spectrum.get());

		autoMatrix matrix2 = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer i = 1; i <= my nx; i ++) {
			const double originalF0 = my frames [i]. candidates [1]. frequency;
			matrix2 -> z [1] [i] = ( Pitch_util_frequencyIsVoiced (originalF0, my ceiling) ?
				sound2 -> z [1] [i + matrix2 -> nx] : 0.0 );
		}
		autoPitch thee = Matrix_to_Pitch (matrix2.get());
		thy ceiling = my ceiling;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

void Pitch_step (Pitch me, double step, double precision, double tmin, double tmax) {
	Melder_assert (precision >= 0.0 && precision < 1.0);
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax))
		return;
	for (integer i = imin; i <= imax; i ++) {
		const Pitch_Frame frame = & my frames [i];
		const double currentFrequency = frame -> candidates [1]. frequency;
		if (Pitch_util_frequencyIsVoiced (currentFrequency, my ceiling)) {
			const double targetFrequency = currentFrequency * step;
			const double fmin = (1.0 - precision) * targetFrequency;
			const double fmax = std::min ((1.0 + precision) * targetFrequency, my ceiling);
			int nearestCandidate = 0;
			double nearestDistance = my ceiling;
			for (int icand = 2; icand <= frame -> nCandidates; icand ++) {
				const double f = frame -> candidates [icand]. frequency;
				if (f > fmin && f < fmax) {
					double localDistance = fabs (f - targetFrequency);
					if (localDistance < nearestDistance) {
						nearestCandidate = icand;
						nearestDistance = localDistance;
					}
				}
			}
			if (nearestCandidate)
				std::swap (frame -> candidates [nearestCandidate], frame -> candidates [1]);
		}
	}
}

static autoTable Pitch_Frame_tabulateCandidates (Pitch_Frame me) {
	autoTable you = Table_createWithColumnNames (my nCandidates, U"frequency strength");
	for (integer icand = 1; icand <= my nCandidates; icand ++) {
		const Pitch_Candidate candidate = & my candidates [icand];
		Table_setNumericValue (you.get(), icand, 1, candidate -> frequency);
		Table_setNumericValue (you.get(), icand, 2, candidate -> strength);
	}
	return you;
}

autoTable Pitch_tabulateCandidatesInFrame (Pitch me, integer frameNumber) {
	my checkIndex (frameNumber);
	const Pitch_Frame frame = & my frames [frameNumber];
	return Pitch_Frame_tabulateCandidates (frame);
}

autoTable Pitch_tabulateCandidates (Pitch me) {
	integer totalNumberOfCandidates = 0;
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		const Pitch_Frame frame = & my frames [iframe];
		totalNumberOfCandidates += frame -> nCandidates;
	}
	autoTable result = Table_createWithColumnNames (totalNumberOfCandidates, U"frame frequency strength");
	integer rowNumber = 0;
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		const Pitch_Frame frame = & my frames [iframe];
		for (integer icand = 1; icand <= frame -> nCandidates; icand ++) {
			const Pitch_Candidate candidate = & frame -> candidates [icand];
			Table_setNumericValue (result.get(), ++ rowNumber, 1, double (iframe));
			Table_setNumericValue (result.get(), rowNumber, 2, candidate -> frequency);
			Table_setNumericValue (result.get(), rowNumber, 3, candidate -> strength);
		}
	}
	return result;
}

/* End of file Pitch.cpp */
