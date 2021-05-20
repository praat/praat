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

Thing_implement (ConstantQLogFSpectrogram, MultiSampledSpectrogram, 0);

void structConstantQLogFSpectrogram :: v_info () {
	structMultiSampledSpectrogram :: v_info ();
	MelderInfo_writeLine (U"Frequency resolution in bins: ", frequencyResolutionInBins);
	MelderInfo_writeLine (U"Quality factor Q: ", ConstantQLogFSpectrogram_getQualityFactor (this));
}

double structConstantQLogFSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structConstantQLogFSpectrogram :: v_myFrequencyUnitToHertz (double log2_f) {
	return exp2 (log2_f);
}

double structConstantQLogFSpectrogram :: v_hertzToMyFrequencyUnit (double f_hz) {
	return ( f_hz > 0.0 ? log2 (f_hz) : undefined );
}

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double tmin, double tmax, double f1, double fmax, integer numberOfBinsPerOctave, double frequencyResolutionInBins) {
	try {
		const double dy = 1.0 / numberOfBinsPerOctave;
		const integer numberOfBins = Melder_iroundDown (log2 (fmax / f1) * numberOfBinsPerOctave);
		Melder_require (numberOfBins > 1,
			U"The number of bins should be larger than 1.");
		const double log2_f1 = log2 (f1);
		const double ymin = log2_f1 - 0.5 * dy, ymax = log2 (fmax);		
		autoConstantQLogFSpectrogram me = Thing_new (ConstantQLogFSpectrogram);
		MultiSampledSpectrogram_init (me.get(), tmin, tmax, ymin, ymax, numberOfBins, dy, log2_f1, frequencyResolutionInBins);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create ConstantQLogFSpectrogram.");
	}
}

double ConstantQLogFSpectrogram_getQualityFactor (ConstantQLogFSpectrogram me) {
	const double a = exp2 (my frequencyResolutionInBins * my dx);
	return 1.0 / (a - 1.0 / a);
}

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish) {
	Graphics_setInner (g);
	MultiSampledSpectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, dBRange);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_inqWindow (g, & tmin, & tmax, & fmin, & fmax);
		double f = my x1; // TODO Can the ticks be generalized into MultiSampledSpectrogram_paint ??
		while (f <= my xmax ) {
			if (f >= fmin) {
				const double f_hz = my v_myFrequencyUnitToHertz (f);
				conststring32 f_string = Melder_fixed (f_hz, 1);
				Graphics_markLeft (g, f, false, true, false, f_string);
			}
			f += 1.0;
		}
		Graphics_textLeft (g, true, U"Frequency (log__2_Hz)");
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

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_translateSpectrum (ConstantQLogFSpectrogram me, double fromTime, double toTime, double fromFrequency, double shiftNumberOfBins) {
	try {
		autoConstantQLogFSpectrogram thee = Data_copy (me);
		if (shiftNumberOfBins == 0.0)
			return thee;
		Melder_require (fabs (shiftNumberOfBins) < my nx,
			U"The shift should not be larger than the number of frequency bins (", my nx, U").");
		fromFrequency = ( fromFrequency <= 0.0 ? (shiftNumberOfBins > 0.0 ? my x1 : my xmax) : my v_hertzToMyFrequencyUnit (fromFrequency) );
		if (shiftNumberOfBins > 0.0) {
			// start at the hihest frequency and work down.
			const integer ifreqFrom = Sampled_xToHighIndex (me, fromFrequency);
			const integer int_shiftNumberofBins = shiftNumberOfBins;
			const integer lowestBin = std::max (1_integer, ifreqFrom - int_shiftNumberofBins);
			for (integer ifreq = my nx; ifreq >= lowestBin; ifreq --) {
				FrequencyBin to_bin = thy frequencyBins.at [ifreq];
				const integer from_index = ifreq - int_shiftNumberofBins;
				integer itmin, itmax;
				if (Sampled_getWindowSamples (to_bin, fromTime, toTime, & itmin, & itmax) > 0 && from_index > 0) {
					FrequencyBin from_bin = my frequencyBins.at [from_index];
					for (integer index = itmin; index <= itmax; index ++) {
						const double time = Sampled_indexToX (to_bin, index);
						const double newValue = FrequencyBin_getValueAtX (from_bin, time, kVector_valueInterpolation::SINC70);
						if (isdefined (newValue))
							to_bin -> z[1] [index] = newValue;
					}
				}
			}
		}
		
		return thee;	
	} catch (MelderError) {
		Melder_throw (me, U": shift not completed.");
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
