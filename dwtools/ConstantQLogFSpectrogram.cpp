/* ConstantQLogFSpectrogram.cpp
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

#include "Formula.h"
#include "ConstantQLogFSpectrogram.h"

#include "oo_DESTROY.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_COPY.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_EQUAL.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_WRITE_TEXT.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_READ_TEXT.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_WRITE_BINARY.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_READ_BINARY.h"
#include "ConstantQLogFSpectrogram_def.h"
#include "oo_DESCRIPTION.h"
#include "ConstantQLogFSpectrogram_def.h"


Thing_implement (ConstantQLogFSpectrogram, MultiSampledSpectrogram, 0);

void structConstantQLogFSpectrogram :: v_info () {
	structMultiSampledSpectrogram :: v_info ();
	MelderInfo_writeLine (U"Quality factor Q: ", qualityFactor);
}

double structConstantQLogFSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structConstantQLogFSpectrogram :: v_myFrequencyToHertz (double log2_f) {
	return exp2 (log2_f);
}

double structConstantQLogFSpectrogram :: v_hertzToMyFrequency (double f_hz) {
	return log2 (f_hz);
}



void ConstantQLogFSpectrogram_paintInside (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double log2_fmin, double log2_fmax, double dBRange) {
	integer ixmin, ixmax, ifmin, ifmax;
	if (Sampled_getWindowSamples (me, log2_fmin, log2_fmax, & ifmin, & ifmax) == 0)
		return;
	Graphics_setWindow (g, tmin, tmax, log2_fmin, log2_fmax);
	integer numberOfFrames = Sampled_getWindowSamples (my frequencyBins.at [ifmax], tmin, tmax, & ixmin, & ixmax);
	autoMAT p = raw_MAT (1, numberOfFrames);
	
	/*
		Find maximum power. No need for logarithm in the test
	*/
	MelderExtremaWithInit powerExtrema;
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins . at [ifreq];
		if ((numberOfFrames = Sampled_getWindowSamples (frequencyBin, tmin, tmax, & ixmin, & ixmax)) == 0)
			continue;
		for (integer iframe = ixmin; iframe <= ixmax; iframe ++) {
			double power = sqr (frequencyBin -> z [1] [iframe]);
			powerExtrema.update (power);
		}
	}
	if (powerExtrema.max == 0.0)
		return; // empty
	const double maximum = 10.0 * log10 (powerExtrema.max / 4e-10);
	const double minimum = std::max (maximum - dBRange, 10.0 * log10 ((powerExtrema.min + 1e-30) / 4e-10));

	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
		double xmin1, xmax1 ;
		const double dx = frequencyBin -> dx;
		const double log2_freq = Sampled_indexToX (me, ifreq);
		if ((numberOfFrames = Sampled_getWindowSamples (frequencyBin, tmin - 0.4999 * dx, tmax + 0.4999 * dx, & ixmin, & ixmax)) == 0)
			continue;
		p.resize (1, numberOfFrames);
		MAT z = frequencyBin -> z.get();
		integer index = 0;
		for (integer iframe = ixmin; iframe <= ixmax; iframe ++) {
			double power = sqr (z [1] [iframe]);
			p [1] [ ++ index] = 10.0 * log10 (power / 4e-10);
		}
		double xmin = Sampled_indexToX (frequencyBin, ixmin) - 0.5 * dx;
		double xmax = Sampled_indexToX (frequencyBin, ixmax) + 0.5 * dx;
		double ymin = log2_freq - 0.5 * my dx; 
		double ymax = log2_freq + 0.5 * my dx;
		if (ifreq > 1) {
			Melder_clipRight (& xmin, xmin1);
			Melder_clipLeft (xmax1, & xmax);
		}
		Graphics_image (g, p.get(), xmin, xmax, ymin, ymax, minimum, maximum);
		xmin1 = xmin;
		xmax1 = xmax;
	}
}

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish) {
	if (fmin >= fmax) {
		fmin = my xmin;
		fmax = my xmax;
	} else {
		fmin = ( fmin > 0 ? log2 (fmin) : 0.0 );
		fmax = ( fmax > my v_myFrequencyToHertz (my xmax) ? my xmax : log2 (fmax) );
	}
	FrequencyBin frequencyBin = my frequencyBins.at [1];
	if (tmin >= tmax) {
		tmin = frequencyBin -> xmin;
		tmax = frequencyBin -> xmax;
	}
	Graphics_setInner (g);
	ConstantQLogFSpectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, dBRange);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		double f = my x1;
		while (f <= my xmax ) {
			if (f >= fmin) {
				const double f_hz = my v_myFrequencyToHertz (f);
				conststring32 f_string = Melder_fixed (f_hz, 1);
				Graphics_markLeft (g, f, false, true, false, f_string);
			}
			f += 1.0;
		}
		Graphics_textLeft (g, true, U"Frequency (log__2_Hz)");
	}
}

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double f1, double fmax, integer numberOfBinsPerOctave, double qualityFactor) {
	try {
		
		const double ymin = 0.0, ymax = log2 (fmax);
		const double dy = 1.0 / numberOfBinsPerOctave;
		const integer numberOfBins = Melder_iroundDown (log2 (fmax / f1) * numberOfBinsPerOctave);
		Melder_require (numberOfBins > 1,
			U"The number of bins should be larger than 1.");
		autoConstantQLogFSpectrogram me = Thing_new (ConstantQLogFSpectrogram);
		my qualityFactor = qualityFactor;
		MultiSampledSpectrogram_init (me.get(), ymin, ymax, numberOfBins, dy, log2 (f1));
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create ConstantQLogFSpectrogram.");
	}
}


void ConstantQLogFSpectrogram_formula (ConstantQLogFSpectrogram me, conststring32 formula, Interpreter interpreter) {
	try {
		
		for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
			FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
			FrequencyBin_formula (frequencyBin, formula, interpreter);
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

void ConstantQLogFSpectrogram_formula_part (ConstantQLogFSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter) {
	try {
		double flow = my xmin, fhigh = my xmax;
		if (fromFrequency < toFrequency && fromFrequency >= 0.0 && toFrequency <= Melder_round_tieUp (exp2 (my xmax))) {
			flow = (fromFrequency > 0 ? log2 (fromFrequency) : 0.0 );
			fhigh = log2 (toFrequency);
		}
		integer iflow, ifhigh;
		if (Sampled_getWindowSamples (me, flow, fhigh, & iflow, & ifhigh) > 0)
			for (integer ifreq = iflow; ifreq <= ifhigh; ifreq ++) {
				FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
				Matrix_formula_part (frequencyBin, fromTime, toTime, 0.5, 1.5, formula, interpreter, nullptr);
			}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed on part.");
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
