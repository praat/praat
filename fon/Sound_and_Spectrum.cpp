/* Sound_and_Spectrum.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2003/03/09 shorter sounds from Hann band filtering
 * pb 2003/05/15 replaced memcof with NUMburg
 * pb 2003/07/02 checks on NUMrealft
 * pb 2004/04/21 Sound_to_Spectrum_dft
 * pb 2004/10/18 explicit Fourier tables
 * pb 2004/11/22 single Sound_to_Spectrum procedure
 * pb 2006/12/30 new Sound_create API
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2009/01/18 Interpreter argument to formula
 * pb 2011/06/06 C++
 * pb 2017/06/07
 */

#include "Sound_and_Spectrum.h"
#include "NUM2.h"

autoSpectrum Sound_to_Spectrum (Sound me, int fast) {
	try {
		integer numberOfSamples = my nx;
		const integer numberOfChannels = my ny;
		if (fast) {
			numberOfSamples = 2;
			while (numberOfSamples < my nx) numberOfSamples *= 2;
		}
		integer numberOfFrequencies = numberOfSamples / 2 + 1;   // 4 samples -> cos0 cos1 sin1 cos2; 5 samples -> cos0 cos1 sin1 cos2 sin2

		autoNUMvector <double> data (1, numberOfSamples);
		if (numberOfChannels == 1) {
			const double *channel = my z [1];
			for (integer i = 1; i <= my nx; i ++) {
				data [i] = channel [i];
			}
			/*
				All samples from `my nx + 1` through `numberOfSamples`
				should be set to zero, but they are already zero.
			*/
			// so do nothing
		} else {
			for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
				const double *channel = my z [ichan];
				for (integer i = 1; i <= my nx; i ++) {
					data [i] += channel [i];
				}
			}
			for (integer i = 1; i <= my nx; i ++) {
				data [i] /= numberOfChannels;
			}
		}

		autoNUMfft_Table fourierTable;
		NUMfft_Table_init (& fourierTable, numberOfSamples);
		NUMfft_forward (& fourierTable, data.peek());

		autoSpectrum thee = Spectrum_create (0.5 / my dx, numberOfFrequencies);
		thy dx = 1.0 / (my dx * numberOfSamples);   // override
		double *re = thy z [1];
		double *im = thy z [2];
		double scaling = my dx;
		re [1] = data [1] * scaling;
		im [1] = 0.0;
		for (integer i = 2; i < numberOfFrequencies; i ++) {
			re [i] = data [i + i - 2] * scaling;   // data [2], data [4], ...
			im [i] = data [i + i - 1] * scaling;   // data [3], data [5], ...
		}
		if ((numberOfSamples & 1) != 0) {
			if (numberOfSamples > 1) {
				re [numberOfFrequencies] = data [numberOfSamples - 1] * scaling;
				im [numberOfFrequencies] = data [numberOfSamples] * scaling;
			}
		} else {
			re [numberOfFrequencies] = data [numberOfSamples] * scaling;
			im [numberOfFrequencies] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrum.");
	}
}

autoSound Spectrum_to_Sound (Spectrum me) {
	try {
		double *re = my z [1], *im = my z [2];
		double lastFrequency = my x1 + (my nx - 1) * my dx;
		bool originalNumberOfSamplesProbablyOdd = ( im [my nx] != 0.0 || my xmax - lastFrequency > 0.25 * my dx );
		if (my x1 != 0.0)
			Melder_throw (U"A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my x1, U" Hz.");
		integer numberOfSamples = 2 * my nx - ( originalNumberOfSamplesProbablyOdd ? 1 : 2 );
		autoSound thee = Sound_createSimple (1, 1.0 / my dx, numberOfSamples * my dx);
		double *amp = thy z [1];
		double scaling = my dx;
		amp [1] = re [1] * scaling;
		for (integer i = 2; i < my nx; i ++) {
			amp [i + i - 1] = re [i] * scaling;
			amp [i + i] = im [i] * scaling;
		}
		if (originalNumberOfSamplesProbablyOdd) {
			amp [numberOfSamples] = re [my nx] * scaling;
			if (numberOfSamples > 1) amp [2] = im [my nx] * scaling;
		} else {
			amp [2] = re [my nx] * scaling;
		}
		NUMrealft (amp, numberOfSamples, -1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

autoSpectrum Spectrum_lpcSmoothing (Spectrum me, int numberOfPeaks, double preemphasisFrequency) {
	try {
		double gain, a [100];
		integer numberOfCoefficients = 2 * numberOfPeaks;

		autoSound sound = Spectrum_to_Sound (me);
		NUMpreemphasize_f (sound -> z [1], sound -> nx, sound -> dx, preemphasisFrequency);	 	
		
		NUMburg (sound -> z [1], sound -> nx, a, numberOfCoefficients, & gain);
		for (integer i = 1; i <= numberOfCoefficients; i ++) a [i] = - a [i];
		autoSpectrum thee = Data_copy (me);

		integer nfft = 2 * (thy nx - 1);
		integer ndata = numberOfCoefficients < nfft ? numberOfCoefficients : nfft - 1;
		double scale = 10.0 * (gain > 0.0 ? sqrt (gain) : 1.0) / numberOfCoefficients;
		autoNUMvector <double> data (1, nfft);
		data [1] = 1.0;
		for (integer i = 1; i <= ndata; i ++)
			data [i + 1] = a [i];
		NUMrealft (data.peek(), nfft, 1);
		double *re = thy z [1];
		double *im = thy z [2];
		re [1] = scale / data [1];
		im [1] = 0.0;
		integer halfnfft = nfft / 2;
		for (integer i = 2; i <= halfnfft; i ++) {
			double realPart = data [i + i - 1], imaginaryPart = data [i + i];
			re [i] = scale / sqrt (realPart * realPart + imaginaryPart * imaginaryPart) / (1.0 + thy dx * (i - 1) / preemphasisFrequency);
			im [i] = 0.0;
		}
		re [halfnfft + 1] = scale / data [2] / (1.0 + thy dx * halfnfft / preemphasisFrequency);
		im [halfnfft + 1] = 0.0;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

autoSound Sound_filter_formula (Sound me, const char32 *formula, Interpreter interpreter) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, true);
			Matrix_formula (spec.get(), formula, interpreter, nullptr);
			autoSound him = Spectrum_to_Sound (spec.get());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
				Matrix_formula (spec.get(), formula, interpreter, nullptr);
				autoSound him = Spectrum_to_Sound (spec.get());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (with formula).");
	}
}

autoSound Sound_filter_passHannBand (Sound me, double fmin, double fmax, double smooth) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, true);
			Spectrum_passHannBand (spec.get(), fmin, fmax, smooth);
			autoSound him = Spectrum_to_Sound (spec.get());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
				Spectrum_passHannBand (spec.get(), fmin, fmax, smooth);
				autoSound him = Spectrum_to_Sound (spec.get());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (pass Hann band).");
	}
}

autoSound Sound_filter_stopHannBand (Sound me, double fmin, double fmax, double smooth) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, true);
			Spectrum_stopHannBand (spec.get(), fmin, fmax, smooth);
			autoSound him = Spectrum_to_Sound (spec.get());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
				Spectrum_stopHannBand (spec.get(), fmin, fmax, smooth);
				autoSound him = Spectrum_to_Sound (spec.get());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (stop Hann band).");
	}
}

/* End of file Sound_and_Spectrum.cpp */
