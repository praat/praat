/* Sound_and_Spectrum.c
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
 * pb 2002/07/16 GPL
 * pb 2003/03/09 shorter sounds from Hann band filtering
 * pb 2003/05/15 replaced memcof with NUMburg
 * pb 2003/07/02 checks on NUMrealft
 * pb 2004/04/21 Sound_to_Spectrum_dft
 * pb 2004/10/18 explicit Fourier tables
 * pb 2004/11/22 single Sound_to_Spectrum procedure
 */

#include "Sound_and_Spectrum.h"
#include "NUM2.h"

Spectrum Sound_to_Spectrum (Sound me, int fast) {
	Spectrum thee = NULL;
	long numberOfSamples = my nx, i, numberOfFrequencies;
	float *data = NULL, scaling;
	float *re, *im;
	struct NUMfft_Table_f fourierTable_struct;
	NUMfft_Table_f fourierTable = & fourierTable_struct;
	fourierTable -> trigcache = NULL;
	fourierTable -> splitcache = NULL;

	if (fast) {
		numberOfSamples = 2;
		while (numberOfSamples < my nx) numberOfSamples *= 2;
	}
	numberOfFrequencies = numberOfSamples / 2 + 1;   /* 4 samples -> cos0 cos1 sin1 cos2; 5 samples -> cos0 cos1 sin1 cos2 sin2 */
	data = NUMfvector (1, numberOfSamples); cherror
	NUMfft_Table_init_f (fourierTable, numberOfSamples); cherror

	for (i = 1; i <= my nx; i ++) data [i] = my z [1] [i];
	NUMfft_forward_f (fourierTable, data); cherror
	thee = Spectrum_create (0.5 / my dx, numberOfFrequencies); cherror
	thy dx = 1.0 / (my dx * numberOfSamples);   /* Override. */
	re = thy z [1]; im = thy z [2];
	scaling = my dx;
	re [1] = data [1] * scaling;
	im [1] = 0.0;
	for (i = 2; i < numberOfFrequencies; i ++) {
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
end:
	NUMfvector_free (data, 1);
	NUMfft_Table_free_f (fourierTable);
	iferror forget (thee);
	return thee;
}

Sound Spectrum_to_Sound (Spectrum me) {
	Sound thee = NULL;
	long numberOfSamples, i;
	float *amp, scaling, *re = my z [1], *im = my z [2];
	double lastFrequency = my x1 + (my nx - 1) * my dx;
	int originalNumberOfSamplesProbablyOdd = im [my nx] != 0.0 || my xmax - lastFrequency > 0.25 * my dx;
	if (my x1 != 0.0) {
		Melder_error ("A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not %.8g Hz.", my x1);
		goto end;
	}
	numberOfSamples = 2 * my nx - ( originalNumberOfSamplesProbablyOdd ? 1 : 2 );
	thee = Sound_createSimple (1 / my dx, numberOfSamples * my dx); cherror
	amp = thy z [1];
	scaling = my dx;
	amp [1] = re [1] * scaling;
	for (i = 2; i < my nx; i ++) {
		amp [i + i - 1] = re [i] * scaling;
		amp [i + i] = im [i] * scaling;
	}
	if (originalNumberOfSamplesProbablyOdd) {
		amp [numberOfSamples] = re [my nx] * scaling;
		if (numberOfSamples > 1) amp [2] = im [my nx] * scaling;
	} else {
		amp [2] = re [my nx] * scaling;
	}
	NUMrealft (amp, numberOfSamples, -1); cherror
end:
	iferror forget (thee);
	return thee;
}

Spectrum Spectrum_lpcSmoothing (Spectrum me, int numberOfPeaks, double preemphasisFrequency) {
	Sound sound = NULL;
	Spectrum thee = NULL;
	float gain, a [100], *data = NULL, *re, *im;
	long i, numberOfCoefficients = 2 * numberOfPeaks, nfft, halfnfft, ndata;
	double scale;

	sound = Spectrum_to_Sound (me); cherror
	NUMpreemphasize_f (sound -> z [1], sound -> nx, sound -> dx, preemphasisFrequency);	 	
	
	NUMburg (sound -> z [1], sound -> nx, a, numberOfCoefficients, & gain);
	for (i = 1; i <= numberOfCoefficients; i ++) a [i] = - a [i];
	thee = Data_copy (me); cherror

	nfft = 2 * (thy nx - 1);
	ndata = numberOfCoefficients < nfft ? numberOfCoefficients : nfft - 1;
	scale = 10 * (gain > 0 ? sqrt (gain) : 1) / numberOfCoefficients;
	data = NUMfvector (1, nfft); cherror
	data [1] = 1;
	for (i = 1; i <= ndata; i ++)
		data [i + 1] = a [i];
	NUMrealft (data, nfft, 1); cherror
	re = thy z [1], im = thy z [2];
	re [1] = scale / data [1];
	im [1] = 0.0;
	halfnfft = nfft / 2;
	for (i = 2; i <= halfnfft; i ++) {
		double real = data [i + i - 1], imag = data [i + i];
		re [i] = scale / sqrt (real * real + imag * imag) / (1 + thy dx * (i - 1) / preemphasisFrequency);
		im [i] = 0;
	}
	re [halfnfft + 1] = scale / data [2] / (1 + thy dx * halfnfft / preemphasisFrequency);
	im [halfnfft + 1] = 0.0;
end:
	forget (sound);
	NUMfvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

Sound Sound_filter_formula (Sound me, const char *formula) {
	Sound filtered = NULL, result = NULL;
	Spectrum spec;
	long i;
	spec = Sound_to_Spectrum (me, TRUE); cherror
	Matrix_formula ((Matrix) spec, formula, 0); cherror
	filtered = Spectrum_to_Sound (spec); cherror
	/* The filtered signal may be much longer than the original, so: */
	result = Data_copy (me); cherror
	for (i = 1; i <= my nx; i ++)
		result -> z [1] [i] = filtered -> z [1] [i];
end:
	forget (filtered);
	forget (spec);
	iferror forget (result);
	return result;
}

Sound Sound_filter_passHannBand (Sound me, double fmin, double fmax, double smooth) {
	Spectrum spec = NULL;
	Sound thee = Data_copy (me), him;
	spec = Sound_to_Spectrum (me, TRUE); cherror
	Spectrum_passHannBand (spec, fmin, fmax, smooth);
	him = Spectrum_to_Sound (spec); cherror
	NUMfvector_copyElements (his z [1], thy z [1], 1, thy nx);
end:
	forget (spec);
	forget (him);
	iferror forget (thee);
	return thee;
}

Sound Sound_filter_stopHannBand (Sound me, double fmin, double fmax, double smooth) {
	Spectrum spec = NULL;
	Sound thee = Data_copy (me), him;
	spec = Sound_to_Spectrum (me, TRUE); cherror
	Spectrum_stopHannBand (spec, fmin, fmax, smooth);
	him = Spectrum_to_Sound (spec); cherror
end:
	NUMfvector_copyElements (his z [1], thy z [1], 1, thy nx);
	forget (spec);
	forget (him);
	iferror forget (thee);
	return thee;
}

/* End of file Sound_and_Spectrum.c */
