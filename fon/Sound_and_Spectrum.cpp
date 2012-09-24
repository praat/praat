/* Sound_and_Spectrum.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
 */

#include "Sound_and_Spectrum.h"
#include "NUM2.h"

Spectrum Sound_to_Spectrum (Sound me, int fast) {
	try {
		long numberOfSamples = my nx;
		if (fast) {
			numberOfSamples = 2;
			while (numberOfSamples < my nx) numberOfSamples *= 2;
		}
		long numberOfFrequencies = numberOfSamples / 2 + 1;   // 4 samples -> cos0 cos1 sin1 cos2; 5 samples -> cos0 cos1 sin1 cos2 sin2
		autoNUMvector <double> data (1, numberOfSamples);
		autoNUMfft_Table fourierTable;
		NUMfft_Table_init (& fourierTable, numberOfSamples);

		for (long i = 1; i <= my nx; i ++)
			data [i] = my ny == 1 ? my z [1] [i] : 0.5 * (my z [1] [i] + my z [2] [i]);
		NUMfft_forward (& fourierTable, data.peek());
		autoSpectrum thee = Spectrum_create (0.5 / my dx, numberOfFrequencies);
		thy dx = 1.0 / (my dx * numberOfSamples);   // override
		double *re = thy z [1];
		double *im = thy z [2];
		double scaling = my dx;
		re [1] = data [1] * scaling;
		im [1] = 0.0;
		for (long i = 2; i < numberOfFrequencies; i ++) {
			re [i] = data [i + i - 2] * scaling;
			im [i] = data [i + i - 1] * scaling;
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Spectrum.");
	}
}

Sound Spectrum_to_Sound (Spectrum me) {
	try {
		double *re = my z [1], *im = my z [2];
		double lastFrequency = my x1 + (my nx - 1) * my dx;
		int originalNumberOfSamplesProbablyOdd = im [my nx] != 0.0 || my xmax - lastFrequency > 0.25 * my dx;
		if (my x1 != 0.0)
			Melder_throw ("A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my x1, L" Hz.");
		long numberOfSamples = 2 * my nx - ( originalNumberOfSamplesProbablyOdd ? 1 : 2 );
		autoSound thee = Sound_createSimple (1, 1 / my dx, numberOfSamples * my dx);
		double *amp = thy z [1];
		double scaling = my dx;
		amp [1] = re [1] * scaling;
		for (long i = 2; i < my nx; i ++) {
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

Spectrum Spectrum_lpcSmoothing (Spectrum me, int numberOfPeaks, double preemphasisFrequency) {
	try {
		double gain, a [100];
		long numberOfCoefficients = 2 * numberOfPeaks;

		autoSound sound = Spectrum_to_Sound (me);
		NUMpreemphasize_f (sound -> z [1], sound -> nx, sound -> dx, preemphasisFrequency);	 	
		
		NUMburg (sound -> z [1], sound -> nx, a, numberOfCoefficients, & gain);
		for (long i = 1; i <= numberOfCoefficients; i ++) a [i] = - a [i];
		autoSpectrum thee = Data_copy (me);

		long nfft = 2 * (thy nx - 1);
		long ndata = numberOfCoefficients < nfft ? numberOfCoefficients : nfft - 1;
		double scale = 10 * (gain > 0 ? sqrt (gain) : 1) / numberOfCoefficients;
		autoNUMvector <double> data (1, nfft);
		data [1] = 1;
		for (long i = 1; i <= ndata; i ++)
			data [i + 1] = a [i];
		NUMrealft (data.peek(), nfft, 1);
		double *re = thy z [1];
		double *im = thy z [2];
		re [1] = scale / data [1];
		im [1] = 0.0;
		long halfnfft = nfft / 2;
		for (long i = 2; i <= halfnfft; i ++) {
			double real = data [i + i - 1], imag = data [i + i];
			re [i] = scale / sqrt (real * real + imag * imag) / (1 + thy dx * (i - 1) / preemphasisFrequency);
			im [i] = 0;
		}
		re [halfnfft + 1] = scale / data [2] / (1 + thy dx * halfnfft / preemphasisFrequency);
		im [halfnfft + 1] = 0.0;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

Sound Sound_filter_formula (Sound me, const wchar_t *formula, Interpreter interpreter) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, TRUE);
			Matrix_formula ((Matrix) spec.peek(), formula, interpreter, NULL);
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (long ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
				Matrix_formula ((Matrix) spec.peek(), formula, interpreter, NULL);
				autoSound him = Spectrum_to_Sound (spec.peek());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered (with formula).");
	}
}

Sound Sound_filter_passHannBand (Sound me, double fmin, double fmax, double smooth) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, TRUE);
			Spectrum_passHannBand (spec.peek(), fmin, fmax, smooth);
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (long ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
				Spectrum_passHannBand (spec.peek(), fmin, fmax, smooth);
				autoSound him = Spectrum_to_Sound (spec.peek());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered (pass Hann band).");
	}
}

Sound Sound_filter_stopHannBand (Sound me, double fmin, double fmax, double smooth) {
	try {
		autoSound thee = Data_copy (me);
		if (my ny == 1) {
			autoSpectrum spec = Sound_to_Spectrum (me, TRUE);
			Spectrum_stopHannBand (spec.peek(), fmin, fmax, smooth);
			autoSound him = Spectrum_to_Sound (spec.peek());
			NUMvector_copyElements (his z [1], thy z [1], 1, thy nx);
		} else {
			for (long ichan = 1; ichan <= my ny; ichan ++) {
				autoSound channel = Sound_extractChannel (me, ichan);
				autoSpectrum spec = Sound_to_Spectrum (channel.peek(), TRUE);
				Spectrum_stopHannBand (spec.peek(), fmin, fmax, smooth);
				autoSound him = Spectrum_to_Sound (spec.peek());
				NUMvector_copyElements (his z [1], thy z [ichan], 1, thy nx);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered (stop Hann band).");
	}
}

/* End of file Sound_and_Spectrum.cpp */
