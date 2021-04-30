/* MultiSampledSpectrogram.cpp
 * 
 * Copyright (C) 2021 David Weenink
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

double structFrequencyBin :: v_getValueAtSample (integer iframe, integer which , int unit) {
	if (unit == 0) {
		return ( which == 1 ? z [1] [iframe] : undefined );
	}
	double power = sqr (z [1] [iframe]);
	return ( unit == 1 ? power : unit == 2 ? 10.0 * log10 (power / 4e-10) : undefined );
}

Thing_implement (MultiSampledSpectrogram, Sampled, 0);

void structMultiSampledSpectrogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Minimum frequency (Hz): ", exp2 (xmin), U" Hz");
	MelderInfo_writeLine (U"Maximum frequency (Hz): ", exp2 (xmax), U" Hz");
	MelderInfo_writeLine (U"First frequency (Hz): ", exp2 (x1), U" Hz");
	const integer numberOfFrequencies = frequencyBins.size;
	MelderInfo_writeLine (U"Number of frequencies: ", numberOfFrequencies);
	MelderInfo_writeLine (U"Number of frames in frequency bin 1: ", frequencyBins.at [1] -> nx);
	MelderInfo_writeLine (U"Number of frames in frequency bin ", numberOfFrequencies, U": ", frequencyBins.at [numberOfFrequencies] -> nx);
	MelderInfo_writeLine (U"Number of frames in all bins: ", MultiSampledSpectrogram_getNumberOfFrames (this));
}

double structMultiSampledSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structMultiSampledSpectrogram :: v_myFrequencyToHertz (double log2_f) {
	return exp2 (log2_f);
}

double structMultiSampledSpectrogram :: v_hertzToMyFrequency (double f_hz) {
	return log2 (f_hz);
}

autoFrequencyBin FrequencyBin_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoFrequencyBin me = Thing_new (FrequencyBin);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 0.5, 1.5, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FrequencyBin not created.");
	}
}

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double fmin, double fmax, integer numberOfFrequencies, double df, double f1) {
	Sampled_init (me, fmin, fmax, numberOfFrequencies, df, f1);
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

double FrequencyBin_getValueAtX (FrequencyBin me, double x, kVector_valueInterpolation valueInterpolationType) {
	const double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge)
		return undefined;
	const integer interpolationDepth = kVector_valueInterpolation_to_interpolationDepth (valueInterpolationType);
	const double index_real = (x - my x1) / my dx + 1.0;
	return NUM_interpolate_sinc (my z.row (1), index_real, interpolationDepth);
}

integer MultiSampledSpectrogram_getNumberOfFrames (MultiSampledSpectrogram me) {
	double numberOfFrames = 0;
	for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins . at [ifreq];
		numberOfFrames += frequencyBin -> nx;
	}
	return numberOfFrames;
}

void MultiSampledSpectrogram_draw (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}

void MultiSampledSpectrogram_paint_inside (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}
void MultiSampledSpectrogram_paint (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	MultiSampledSpectrogram_paint_inside (me, g, tmin, tmax, fmin, fmax, garnish);
}

/* End of file  MultiSampledSpectrogram.cpp */
