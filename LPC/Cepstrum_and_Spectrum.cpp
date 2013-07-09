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
		autoCepstrum thee = Cepstrum_create (x -> xmax - x -> xmin, x -> nx);
		NUMvector_copyElements (x -> z[1], thy z[1], 1, x -> nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrum created.");
	}
}

PowerCepstrum Spectrum_to_PowerCepstrum (Spectrum me) {
	try {
		autoSpectrum dBspectrum = Data_copy (me);
		double *re = dBspectrum -> z[1], *im = dBspectrum -> z[2];
		for (long i = 1; i <= dBspectrum -> nx; i ++) {
			re[i] = log (re[i] * re[i] + im[i] * im[i] + 1e-300);
			im[i] = 0.0;
		}
		autoSound cepstrum = Spectrum_to_Sound (dBspectrum.peek());
		autoPowerCepstrum thee = PowerCepstrum_create (0.5 / my dx, my nx);
		for (long i = 1; i <= thy nx; i++) {
			double val = cepstrum -> z[1][i];
			thy z[1][i] = val * val;
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

Cepstrum Spectrum_to_Cepstrum (Spectrum me) {
	try {
		autoSpectrum dBspectrum = Data_copy (me);
		double *re = dBspectrum -> z[1], *im = dBspectrum -> z[2];
		for (long i = 1; i <= dBspectrum -> nx; i++) {
			re[i] = log (re[i] * re[i] + im[i] * im[i] + 1e-300);
			im[i] = 0.0;
		}
		autoSound cepstrum = Spectrum_to_Sound (dBspectrum.peek());
		autoCepstrum thee = Cepstrum_create (0.5 / my dx, my nx);
		for (long i = 1; i <= thy nx; i++) {
			double val = cepstrum -> z[1][i];
			thy z[1][i] = val;
		}
		return thee.transfer ();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

Spectrum Cepstrum_to_Spectrum (Cepstrum me) { //TODO power cepstrum
	try {
		autoCepstrum cepstrum = Data_copy (me);
		cepstrum ->  z[1][1] = my z[1][1];
		for (long i = 2; i <= cepstrum -> nx; i++) {
			cepstrum -> z[1][i] = 2 * my z[1][i];
		}
		autoSpectrum thee = Sound_to_Spectrum ((Sound) cepstrum.peek(), 1);

		double *re = thy z[1], *im = thy z[2];
		for (long i = 1; i <= thy nx; i ++) {
			re[i] =  exp (0.5 * re[i]);   // i.e., sqrt (exp(re [i]))
			im[i] = 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Spectrum created.");
	}
}

Cepstrum Spectrum_to_Cepstrum2 (Spectrum me) {
	try {
		autoNUMfft_Table fftTable;
		// originalNumberOfSamplesProbablyOdd irrelevant
		if (my x1 != 0.0) {
			Melder_throw ("A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my x1, L" Hz.");
		}
		long numberOfSamples = 2 * my nx - 2;
		autoCepstrum thee = Cepstrum_create (0.5 / my dx, my nx);
		// my dx = 1 / (dT * N) = 1 / (duration of sound)
		thy dx = 1 / (my dx * numberOfSamples); // Cepstrum is on [-T/2, T/2] !
		NUMfft_Table_init (&fftTable, numberOfSamples);
		autoNUMvector<double> fftbuf (1, numberOfSamples);

		fftbuf[1] = my v_getValueAtSample (1, 0, 2);
		for (long i = 2; i < my nx; i++) {
			fftbuf [i + i - 2] = my v_getValueAtSample (i, 0, 2);
			fftbuf [i + i - 1] = 0;
		}
		fftbuf [numberOfSamples] = my v_getValueAtSample (my nx, 0, 2);
		NUMfft_backward (&fftTable, fftbuf.peek());
		for (long i = 1; i <= my nx; i++) {
			double val = fftbuf[i] / numberOfSamples; // scaling 1/n because ifft(fft(1))= n;
			thy z[1][i] = val * val; // power cepstrum
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Cepstrum.");
	}
}


Spectrum Cepstrum_to_Spectrum2 (Cepstrum me) { //TODO power cepstrum
	try {
		autoNUMfft_Table fftTable;
		long numberOfSamples = 2 * my nx - 2;

		autoNUMvector<double> fftbuf (1, numberOfSamples);
		autoSpectrum thee = Spectrum_create (0.5 / my dx, my nx);
		fftbuf[1] = sqrt (my z[1][1]);
		for (long i = 2; i <= my nx; i++) {
			fftbuf[i] = 2 * sqrt (my z[1][i]);
		}
		// fftbuf[my nx+1 ... numberOfSamples] = 0
		NUMfft_Table_init (&fftTable, numberOfSamples);
		NUMfft_forward (&fftTable, fftbuf.peek());
		
		thy z[1][1] = fabs (fftbuf[1]);
		for (long i = 2; i < my nx; i++) {
			double br = fftbuf[i + i - 2], bi = fftbuf[i + i - 1];
			thy z[1][i] = sqrt (br * br + bi * bi);
		}
		thy z[1][my nx] = fabs (fftbuf[numberOfSamples]);
		for (long i = 1; i <= my nx; i++) {
			thy z[1][i] = exp (NUMln10 * thy z[1][i] / 20) * 2e-5 / sqrt (2 * thy dx);
			thy z[2][i] = 0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Spectrum created.");
	}
}

/* End of file Cepstrum_and_Spectrum.cpp  */
