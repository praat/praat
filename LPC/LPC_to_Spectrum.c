/* LPC_to_Spectrum.c
 *
 * Copyright (C) 1994-2002 David Weenink
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
 djmw 20020529 GPL header
 djmw 20020529 Changed NUMrealft to NUMforwardRealFastFourierTransform_f
 djmw 20030708 Added NUM2.h
*/

#include "LPC_to_Spectrum.h"
#include "NUM2.h"

/*
	PSD(f) = (sigma^2 T) /|1 + Sum (k=1..p, a[k] exp(-2 pi i f k T))|^2,
	where sigma^2 == gain, T is samplinginterval
	
	LPC-spectrum is approximately 20 dB too high (w.r.t. 25 ms spectrum from Sound)
*/
	
int LPC_Frame_into_Spectrum (LPC_Frame me, Spectrum thee, double bandwidthReduction,
	double deEmphasisFrequency)
{
	long i, nfft = 2 * (thy nx - 1), ndata = my nCoefficients + 1;
	float scale = 1.0 / sqrt (2 * thy xmax * thy dx), *fftbuffer;
	
	if (my nCoefficients == 0)
	{
		for (i=1; i <= thy nx; i++)
		{
			thy z[1][i] = thy z[2][i] = 0;
		}
		return 1;
	}
	
	/*
		When deEmphasisFrequency is effective we need 1 extra position in the fftbuffer.
	*/
	
	if (ndata >= nfft - 1 && (deEmphasisFrequency < thy xmax || ndata > nfft))
	{
		return Melder_error ("LPC_Frame_into_Spectrum: Spectrum size not large enough.");
	}
	
	if ((fftbuffer = NUMfvector (1, nfft)) == NULL) return 0;
	
	/*
		Copy 1, a[1], ... a[p] into fftbuffer
	*/
	
	fftbuffer[1] = 1;
	for (i=2; i <= ndata; i++)
	{
		fftbuffer[i] = my a[i-1];
	}
	
	if (deEmphasisFrequency < thy xmax)
	{
		/*
			Multiply (1, a[1] z^-1, ... a[p] z^-p) by (1 - b z^-1)
		*/
	
		double b = exp (- 2.0 * NUMpi * deEmphasisFrequency / thy xmax);
		ndata ++;
		for (i=ndata; i > 1; i--)
		{
			fftbuffer[i] -= b * fftbuffer[i-1];
		}
	}
	
	if (bandwidthReduction > 0)
	{
		/*
			Calculate sum (k=0..ndata; a[k] (z)^-k) along a contour with radius r:
			sum (k=0..ndata; a[k] (rz)^-k) = sum (k=0..ndata; (a[k]r^-k) z^-k)
		*/
	
		double g = exp (NUMpi * bandwidthReduction / (thy dx * nfft)); /* r = 1/g */
		for (i=2; i <= ndata; i++)
		{
			fftbuffer[i] *= pow (g, i - 1);
		}
	}
	
	/*
		Perform the fft.
		The LPC spectrum is obtained by inverting this spectrum.
		The imaginary parts of the frequencies 0 and Nyquist are 0.
	*/
	
	NUMforwardRealFastFourierTransform_f (fftbuffer, nfft);
	if (my gain > 0) scale *= sqrt (my gain);
	thy z[1][1] = scale / fftbuffer[1];
	thy z[2][1] = 0;
	for (i=2; i <= nfft/2; i++)
	{
		/*
			We use: 1 / (a + ib) = (a - ib) / (a^2 + b^2)
		*/
		float re = fftbuffer[i+i-1], im = fftbuffer[i+i];
		float invSquared = scale / (re * re + im * im);
		thy z[1][i] =  re * invSquared;
		thy z[2][i] = -im * invSquared;
	}
	thy z[1][thy nx] = scale / fftbuffer[2];
	thy z[2][thy nx] = 0;
	
	NUMfvector_free (fftbuffer, 1);
	return 1;
}

Spectrum LPC_to_Spectrum (LPC me, double t, double dfMin, double bandwidthReduction,
	double deEmphasisFrequency)
{
	Spectrum thee; double samplingFrequency = 1.0 / my samplingPeriod;
	long nfft = 2, index = Sampled_xToNearestIndex (me, t);
	
	if (index < 1) index = 1;
	if (index > my nx) index = my nx;
	if (dfMin <= 0)
	{
		nfft = 512; dfMin = samplingFrequency / nfft;
	}
	while (samplingFrequency / nfft > dfMin || nfft <= my frame[index].nCoefficients)
	{
		nfft *= 2;
	}
	if (((thee = Spectrum_create (samplingFrequency / 2, nfft / 2 + 1)) == NULL) ||
		! LPC_Frame_into_Spectrum (& my frame[index], thee, bandwidthReduction, deEmphasisFrequency)) forget (thee);

	return thee;
}

/* End of file LPC_to_Spectrum.c */
