/* Cepstrum_and_Spectrum.cpp
 *
 * Copyright (C) 1994-2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020812 GPL header
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
 djmw 20121015
*/

#include "Cepstrum_and_Spectrum.h"
#include "NUM2.h"
#include "Spectrum_extensions.h"
#include "Sound_and_Spectrum.h"

Cepstrum Spectrum_to_Cepstrum_cmplx (Spectrum me) {
	try {
		autoMatrix unwrap = Spectrum_unwrap (me);
		autoSpectrum sx = Data_copy (me);

		// Copy magnitude-squared and unwrapped phase.

		for (long i = 1; i <= my nx; i ++) {
			double xa = unwrap -> z[1][i];
			sx -> z[1][i] = xa > 0 ? 0.5 * log (xa) : -300;
			sx -> z[2][i] = unwrap -> z[2][i];
		}

		// Compute complex cepstrum x.

		autoSound x = Spectrum_to_Sound (sx.peek());
		autoCepstrum thee = Cepstrum_create (0, x -> xmax - x -> xmin, x -> nx);
		NUMvector_copyElements (x -> z[1], thy z[1], 1, x -> nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrum created.");
	}
}

Cepstrum Spectrum_to_Cepstrum (Spectrum me) {
	try {
		autoNUMfft_Table fftTable;
		// originalNumberOfSamplesProbablyOdd irrelevant
		if (my x1 != 0.0) {
			Melder_throw ("A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my x1, L" Hz.");
		}
		long numberOfSamples = 2 * my nx - 2;
		autoCepstrum thee = Cepstrum_create (0, 0.5 / my dx, my nx);
		NUMfft_Table_init (&fftTable, numberOfSamples);
		autoNUMvector<double> amp (1, numberOfSamples);
		amp[1] = my v_getValueAtSample (1, 0, 2);
		for (long i = 2; i < my nx; i ++) {
			double pow_dB = my v_getValueAtSample (i, 0, 2);
			amp [i + i - 2] = pow_dB / 10;
			amp [i + i - 1] = 0;
		}
		amp [numberOfSamples] = my v_getValueAtSample (my nx, 0, 2);
		NUMfft_backward (&fftTable, amp.peek());
		for (long i = 1; i <= my nx; i++) {
			thy z[1][i] = fabs (amp[i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

Spectrum Cepstrum_to_Spectrum (Cepstrum me) {
	try {
		autoSound tmp = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		NUMvector_copyElements	(my z[1], tmp -> z[1], 1, my nx);
		autoSpectrum thee = Sound_to_Spectrum (tmp.peek(), TRUE);

		double *x = thy z[1], *y = thy z[2];
		double scaling = tmp -> dx;
		for (long i = 1; i <= thy nx; i++) {
			double amp = sqrt (x[i] * x[i] + y [i] * y[i]) / scaling;
			//amp = pow (10, amp / 10); //
			// we have lost phase
			thy z[1][i] = amp * 2.0e-5 ;
			thy z[2][i] = 0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Spectrum created.");
	}
}

/* End of file Cepstrum_and_Spectrum.cpp  */
