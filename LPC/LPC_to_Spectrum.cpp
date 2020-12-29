/* LPC_to_Spectrum.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
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

/*
 djmw 20020529 GPL header
 djmw 20020529 Changed NUMrealft to NUMforwardRealFastFourierTransform
 djmw 20030708 Added NUM2.h
 djmw 20080122 float -> double
*/

#include "LPC_to_Spectrum.h"
#include "NUM2.h"

/*
	PSD(f) = (sigma^2 T) /|1 + Sum (k=1..p, a [k] exp(-2 pi i f k T))|^2,
	where sigma^2 == gain, T is samplinginterval

	LPC-spectrum is approximately 20 dB too high (w.r.t. 25 ms spectrum from Sound)
*/

void LPC_Frame_into_Spectrum (LPC_Frame me, Spectrum thee, double bandwidthReduction, double deEmphasisFrequency) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	if (my nCoefficients == 0) {
		thy z.all()  <<=  0.0;
		return;
	}
	/*
		When deEmphasisFrequency is effective we need 1 extra position in the fftbuffer.
	*/
	const integer nfft = 2 * (thy nx - 1);
	double ndata = my nCoefficients + 1;
	double scale = 1.0 / sqrt (2.0 * thy xmax * thy dx);
	if (ndata >= nfft - 1 && (deEmphasisFrequency < thy xmax || ndata > nfft))
		Melder_throw (U"Spectrum size not large enough.");

	autoVEC fftbuffer = zero_VEC (nfft);
	/*
		Copy 1.0, a [1], ... a [p] into fftbuffer
	*/
	fftbuffer [1] = 1.0;
	fftbuffer.part (2, ndata) <<= my a.get();

	if (deEmphasisFrequency < thy xmax) {
		/*
			Multiply (1, a [1] z^-1, ... a [p] z^-p) by (1 - b z^-1)
		*/
		const double b = exp (- NUM2pi * deEmphasisFrequency / thy xmax);
		ndata ++;
		for (integer i = ndata; i > 1; i--)
			fftbuffer [i] -= b * fftbuffer [i - 1];
	}
	/*
		Calculate sum (k=0..ndata; a [k] (z)^-k) along a contour with radius r:
		sum (k=0..ndata; a [k] (rz)^-k) = sum (k=0..ndata; (a [k]r^-k) z^-k)
	*/
	const double g = exp (NUMpi * bandwidthReduction / (thy dx * nfft)); /* r = 1/g */
	for (integer i = 2; i <= ndata; i ++)
		fftbuffer [i] *= pow (g, i - 1);
	/*
		Perform the fft.
		The LPC spectrum is obtained by inverting this spectrum.
		The imaginary parts of the frequencies 0 and Nyquist are 0.
	*/
	NUMforwardRealFastFourierTransform (fftbuffer.get());
	if (my gain > 0.0)
		scale *= sqrt (my gain);
	thy z [1] [1] = scale / fftbuffer [1];
	thy z [2] [1] = 0.0;
	for (integer i = 2; i <= nfft / 2; i ++) {
		/*
			We use: 1 / (a + ib) = (a - ib) / (a^2 + b^2)
		*/
		const double re = fftbuffer [i + i - 1], im = fftbuffer [i + i];
		const double invSquared = scale / (re * re + im * im);
		thy z [1] [i] =  re * invSquared;
		thy z [2] [i] = -im * invSquared;
	}
	thy z [1] [thy nx] = scale / fftbuffer [2];
	thy z [2] [thy nx] = 0.0;
}

autoSpectrum LPC_to_Spectrum (LPC me, double t, double dfMin, double bandwidthReduction, double deEmphasisFrequency) {
	try {
		const double samplingFrequency = 1.0 / my samplingPeriod;
		integer nfft = 2, index = Sampled_xToNearestIndex (me, t);
		if (index < 1)
			index = 1;
		if (index > my nx)
			index = my nx;
		if (dfMin <= 0) {
			nfft = 512;
			dfMin = samplingFrequency / nfft;
		}
		while (samplingFrequency / nfft > dfMin || nfft <= my d_frames [index].nCoefficients)
			nfft *= 2;

		autoSpectrum thee = Spectrum_create (samplingFrequency / 2.0, nfft / 2 + 1);
		LPC_Frame_into_Spectrum (& my d_frames [index], thee.get(), bandwidthReduction, deEmphasisFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum created.");
	}
}

/* End of file LPC_to_Spectrum.cpp */
