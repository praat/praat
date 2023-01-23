/* ConstantQLog2FSpectrogram.cpp
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

#include "Formula.h"
#include "ConstantQSpectrograms.h"

Thing_implement (ConstantQLog2FSpectrogram, MultiSampledSpectrogram, 0);

void structConstantQLog2FSpectrogram :: v1_info () {
	ConstantQLog2FSpectrogram_Parent :: v1_info ();
	MelderInfo_writeLine (U"Frequency resolution in bins: ", frequencyResolutionInBins);
	MelderInfo_writeLine (U"Quality factor Q: ", ConstantQLog2FSpectrogram_getQualityFactor (this));
}

double structConstantQLog2FSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) const {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structConstantQLog2FSpectrogram :: v_myFrequencyUnitToHertz (double log2_f) const {
	return exp2 (log2_f);
}

double structConstantQLog2FSpectrogram :: v_hertzToMyFrequencyUnit (double f_hz) const {
	return ( f_hz > 0.0 ? log2 (f_hz) : undefined );
}

autoConstantQLog2FSpectrogram ConstantQLog2FSpectrogram_create (double tmin, double tmax, double f1, double fmax, integer numberOfBinsPerOctave, double frequencyResolutionInBins) {
	try {
		autoConstantQLog2FSpectrogram me = Thing_new (ConstantQLog2FSpectrogram);
		const double log2_dy = 1.0 / numberOfBinsPerOctave;
		const integer numberOfBins = Melder_iroundDown (log2 (fmax / f1) * numberOfBinsPerOctave);
		Melder_require (numberOfBins > 1,
			U"The number of bins should be larger than 1.");
		const double log2_ymin = log2 (f1);
		const double log2_f1 = log2_ymin + 0.5 * log2_dy, log2_ymax = log2 (fmax);		
		MultiSampledSpectrogram_init (me.get(), tmin, tmax, log2_ymin, log2_ymax, numberOfBins, log2_dy, log2_f1, frequencyResolutionInBins);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create ConstantQLog2FSpectrogram.");
	}
}

double ConstantQLog2FSpectrogram_getQualityFactor (ConstantQLog2FSpectrogram me) {
	const double a = exp2 (my frequencyResolutionInBins * my dx);
	return 1.0 / (a - 1.0 / a);
}

static void MultiSampledSpectrogram_setFrequencyTicks (MultiSampledSpectrogram me, Graphics g, double fmin, double fmax, double df) {
	double f = fmin;
	while (f <= fmax) {
		const double f_hz = my v_myFrequencyUnitToHertz (f);
		conststring32 f_string = Melder_fixed (f_hz, 1);
		Graphics_markLeft (g, f, false, true, false, f_string);
		f += df;
	}
}

void ConstantQLog2FSpectrogram_paint (ConstantQLog2FSpectrogram me, Graphics g, double tmin, double tmax, double fmin_hz, double fmax_hz, double dBRange, bool garnish) {
	Graphics_setInner (g);
	double fmin, fmax;	
	if (fmin_hz == 0.0 && fmax_hz == 0.0) {
		fmin = my xmin;
		fmax = my xmax;
	} else {
		fmin = ( fmin_hz > 0.0 ? my v_hertzToMyFrequencyUnit (fmin_hz) : my xmin );
		fmax = ( fmax_hz > 0.0 ? my v_hertzToMyFrequencyUnit (fmax_hz) : my xmin );
	}
	if (! Function_intersectRangeWithDomain (me, & fmin, & fmax))
		return;
	MultiSampledSpectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, dBRange);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_inqWindow (g, & tmin, & tmax, & fmin, & fmax);
		MultiSampledSpectrogram_setFrequencyTicks (me, g, fmin, fmax, 1.0);
		Graphics_textLeft (g, true, U"Frequency (log__2_Hz)");
	}
}

autoConstantQLog2FSpectrogram ConstantQLog2FSpectrogram_translateSpectrum (ConstantQLog2FSpectrogram me, double fromTime, double toTime, double fromFrequency, double shiftNumberOfBins) {
	try {
		autoConstantQLog2FSpectrogram thee = Data_copy (me);
		if (shiftNumberOfBins == 0.0)
			return thee;
		Melder_require (fabs (shiftNumberOfBins) < my nx,
			U"The shift should not be larger than the number of frequency bins (", my nx, U").");
		fromFrequency = ( fromFrequency <= 0.0 ? (shiftNumberOfBins > 0.0 ? my x1 : my xmax) : my v_hertzToMyFrequencyUnit (fromFrequency) );
		if (shiftNumberOfBins > 0.0) {
			// start at the highest frequency and work down.
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
							to_bin -> z [1] [index] = newValue;
					}
				}
			}
		}
		
		return thee;	
	} catch (MelderError) {
		Melder_throw (me, U": shift not completed.");
	}
}

Thing_implement (GaborSpectrogram, MultiSampledSpectrogram, 0);

void structGaborSpectrogram :: v1_info () {
	MultiSampledSpectrogram_Parent :: v1_info ();
	MelderInfo_writeLine (U"Frequency resolution in bins: ", frequencyResolutionInBins);
}

void GaborSpectrogram_paint (GaborSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish) {
	Function_bidirectionalAutowindow (me, & fmin, & fmax);
	if (! Function_intersectRangeWithDomain (me, & fmin, & fmax))
		return;
	Graphics_setInner (g);
	MultiSampledSpectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, dBRange);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_inqWindow (g, & tmin, & tmax, & fmin, & fmax);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (Hz)");
	}
}

autoGaborSpectrogram GaborSpectrogram_create (double tmin, double tmax, double fmax, double filterBandwidth, double df) {
	try {
		autoGaborSpectrogram me = Thing_new (GaborSpectrogram);
		Melder_assert (filterBandwidth > 0.0);
		Melder_assert (df > 0.0);
		Melder_require (filterBandwidth <= fmax,
			U"The filter bandwidth should not exceed ", fmax, U".");
		const integer numberOfFrequencyBins = Melder_ifloor ((fmax - filterBandwidth) / df) + 1;
		const double f1 = 0.5 * (fmax - (numberOfFrequencyBins - 1) * df);
		const double frequencyResolutionBins = 0.5 * filterBandwidth / df;
		MultiSampledSpectrogram_init (me.get(), tmin, tmax, 0.0, fmax, numberOfFrequencyBins, df, f1, frequencyResolutionBins);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create GaborSpectrogram.");
	}
}

/* End of file ConstantQSpectrograms.cpp */
