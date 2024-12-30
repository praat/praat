/* MultiSampledSpectrogram.cpp
 * 
 * Copyright (C) 2021-2022 David Weenink
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

#include "Collection.h"
#include "Formula.h"
#include "MultiSampledSpectrogram.h"

#include "oo_DESTROY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_COPY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_EQUAL.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_WRITE_TEXT.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_READ_TEXT.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_WRITE_BINARY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_READ_BINARY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_DESCRIPTION.h"
#include "MultiSampledSpectrogram_def.h"

Thing_implement (FrequencyBin, Matrix, 2);

double structFrequencyBin :: v_getValueAtSample (integer iframe, integer which, int unit) const {
	if (unit == 0)
		return ( which == 1 ? z [1] [iframe] : which == 2 ? z [2] [iframe] : undefined );
	const double power = sqr (z [1] [iframe]) + sqr (z [2] [iframe]);
	return ( unit == 1 ? power : unit == 2 ? 10.0 * log10 ((power + 1e-30) / 4e-10) : undefined );
}

Thing_implement_pureVirtual (MultiSampledSpectrogram, Sampled, 0);

void structMultiSampledSpectrogram :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Minimum frequency (Hz): ", v_myFrequencyUnitToHertz (xmin), U" Hz");
	MelderInfo_writeLine (U"Maximum frequency (Hz): ", v_myFrequencyUnitToHertz (xmax), U" Hz");
	MelderInfo_writeLine (U"First frequency (Hz): ", v_myFrequencyUnitToHertz (x1), U" Hz");
	const integer numberOfFrequencies = frequencyBins.size;
	MelderInfo_writeLine (U"Number of frequencies: ", numberOfFrequencies);
	MelderInfo_writeLine (U"Number of frames in frequency bin 1: ", frequencyBins.at [1] -> nx);
	MelderInfo_writeLine (U"Number of frames in frequency bin ", numberOfFrequencies, U": ", frequencyBins.at [numberOfFrequencies] -> nx);
	MelderInfo_writeLine (U"Number of frames in all bins: ", MultiSampledSpectrogram_getNumberOfFrames (this));
}

double structMultiSampledSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) const {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structMultiSampledSpectrogram :: v_myFrequencyUnitToHertz (double f) const {
	return f;
}

double structMultiSampledSpectrogram :: v_hertzToMyFrequencyUnit (double f) const {
	return f;
}

autoFrequencyBin FrequencyBin_create (double tmin, double tmax, integer nx, double dx, double x1) {
	try {
		autoFrequencyBin me = Thing_new (FrequencyBin);
		Matrix_init (me.get(), tmin, tmax, nx, dx, x1, 0.5, 2.5, 2, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FrequencyBin not created.");
	}
}

void FrequencyBin_formula (FrequencyBin me, conststring32 formula, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		for (integer irow = 1; irow <= my ny; irow ++)
			for (integer ibin = 1; ibin <= my nx; ibin ++) {
				Formula_run (irow, ibin, & result);
				my z [irow] [ibin] = result. numericResult;
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}


// TODO:multiple rows!
double FrequencyBin_getValueAtX (FrequencyBin me, double x, kVector_valueInterpolation valueInterpolationType) {
	const double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge)
		return undefined;
	const integer interpolationDepth = kVector_valueInterpolation_to_interpolationDepth (valueInterpolationType);
	const double index_real = (x - my x1) / my dx + 1.0;
	return NUM_interpolate_sinc (my z.row (1), index_real, interpolationDepth);
}

autoSound FrequencyBin_to_Sound (FrequencyBin me) {
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1)  <<=  my z.row (1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot convert to Sound.");
	}
}

autoAnalyticSound FrequencyBin_to_AnalyticSound (FrequencyBin me) {
	try {
		autoAnalyticSound thee = AnalyticSound_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.get()  <<=  my z.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot convert to AnalyticSound.");
	}
}

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double tmin, double tmax, double fmin, double fmax, 
	integer numberOfFrequencies, double df, double f1, double frequencyResolutionInBins)
{
	my tmin = tmin;
	my tmax = tmax;
	my frequencyResolutionInBins = frequencyResolutionInBins;
	Sampled_init (me, fmin, fmax, numberOfFrequencies, df, f1);
}

void MultiSampledSpectrogram_getFrequencyBand (MultiSampledSpectrogram me, integer index, double *out_flow, double *out_fhigh) {
	const double centreFrequency = Sampled_indexToX (me, index);
	if (out_flow) {
		double flow = my v_myFrequencyUnitToHertz (centreFrequency - my frequencyResolutionInBins * my dx);
		Melder_clipLeft (0.0, & flow);
		*out_flow = flow;
	}
	if (out_fhigh) {
		const double maximumFrequency = my v_myFrequencyUnitToHertz (my xmax);
		double fhigh = my v_myFrequencyUnitToHertz (centreFrequency + my frequencyResolutionInBins * my dx);
		Melder_clipRight (& fhigh, maximumFrequency);
		*out_fhigh = fhigh;
	}
}

void MultiSampledSpectrogram_formula (MultiSampledSpectrogram me, conststring32 formula, Interpreter interpreter) {
	try {
		for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
			FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
			FrequencyBin_formula (frequencyBin, formula, interpreter);
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

void MultiSampledSpectrogram_formula_part (MultiSampledSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter) {
	try {
		const double fx1 = my v_myFrequencyUnitToHertz (my x1);
		const double fnyquist = my v_myFrequencyUnitToHertz (my xmax);
		if (fromFrequency == toFrequency && fromFrequency == 0.0) {
			fromFrequency  = fx1;
			toFrequency = fnyquist;
		}
		Melder_require (toFrequency >= fx1 && fromFrequency <= toFrequency && fromFrequency < fnyquist,
			U"At least one of the frequencies needs to be in the interval from ", fx1, U" to ", fnyquist, U" Hz.");
		Melder_clip (fx1, & fromFrequency, fnyquist);
		Melder_clip (fx1, & toFrequency, fnyquist);
		const double flow = my v_hertzToMyFrequencyUnit (fromFrequency);
		const double fhigh = my v_hertzToMyFrequencyUnit (toFrequency);
		integer iflow, ifhigh;
		if (Sampled_getWindowSamples (me, flow, fhigh, & iflow, & ifhigh) > 0)
			for (integer ifreq = iflow; ifreq <= ifhigh; ifreq ++) {
				FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
				Matrix_formula_part (frequencyBin, fromTime, toTime, 0.5, 2.5, formula, interpreter, nullptr);
			}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed on part.");
	}
}

integer MultiSampledSpectrogram_getNumberOfFrames (MultiSampledSpectrogram me) {
	double numberOfFrames = 0;
	for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
		numberOfFrames += frequencyBin -> nx;
	}
	return numberOfFrames;
}

void MultiSampledSpectrogram_paintInside (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange) {
	integer itmin, itmax, ifmin, ifmax;
	if (tmax <= tmin) {
		tmin = my tmin;
		tmax = my tmax;
	}
	if (fmax <= fmin) {
		fmax = my xmax;
		fmin = my xmin;
	}
	if (Sampled_getWindowSamples (me, fmin, fmax, & ifmin, & ifmax) == 0)
		return;
	const integer maximumNumberOfTimeFrames = Sampled_getWindowSamples (my frequencyBins.at [ifmax], tmin, tmax, & itmin, & itmax);
	if (maximumNumberOfTimeFrames == 0)
		return;	
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	autoMAT p = raw_MAT (1, maximumNumberOfTimeFrames);	
	/*
		Find maximum power. No need for logarithm in the test
	*/
	MelderExtremaWithInit powerExtrema;
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
		if (Sampled_getWindowSamples (frequencyBin, tmin, tmax, & itmin, & itmax) == 0)
			continue;
		for (integer iframe = itmin; iframe <= itmax; iframe ++) {
			double powerdB = frequencyBin -> v_getValueAtSample (iframe, 0, 2); // 10*log10 (power/..)
			powerExtrema.update (powerdB);
		}
	}
	if (powerExtrema.max == 0.0)
		return; // empty
	const double maximum = powerExtrema.max;
	const double minimum = std::max (maximum - dBRange, powerExtrema.min);
	double tmin_previousBin = undefined, tmax_previousBin = undefined;
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
		const double dx = frequencyBin -> dx;
		const integer numberOfFrames = Sampled_getWindowSamples (
			frequencyBin, tmin - 0.4999 * dx, tmax + 0.4999 * dx, & itmin, & itmax);
		if (numberOfFrames == 0)
			continue;
		p.resize (1, numberOfFrames);
		integer index = 0;
		for (integer iframe = itmin; iframe <= itmax; iframe ++)
			p [1] [ ++ index] = frequencyBin -> v_getValueAtSample (iframe, 0, 2);
		double tmin_bin = Sampled_indexToX (frequencyBin, itmin) - 0.5 * dx;
		double tmax_bin = Sampled_indexToX (frequencyBin, itmax) + 0.5 * dx;
		Melder_clipRight (& tmin_bin, tmin_previousBin); // clip against previous
		Melder_clipLeft (tmax_previousBin, & tmax_bin);
		tmin_previousBin = tmin_bin;
		tmax_previousBin = tmax_bin;
		const double freq = Sampled_indexToX (me, ifreq);
		const double ymin = freq - 0.5 * my dx, ymax = freq + 0.5 * my dx;
		Graphics_image (g, p.get(), tmin_bin, tmax_bin, ymin, ymax, minimum, maximum);
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
