/* LPC_to_Spectrogram.c
 *
 * Copyright (C) 1994-2008 David Weenink
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
 djmw 20080122 float -> double
*/

#include "LPC_to_Spectrogram.h"

Spectrogram LPC_to_Spectrogram (LPC me, double dfMin, double bandwidthReduction,
	double deEmphasisFrequency)
{
	Spectrogram thee = NULL; long i, j, nfft = 2;
	double freqStep, samplingFrequency = 1.0 / my samplingPeriod;
	
	if (dfMin <= 0)
	{
		nfft = 512; dfMin = samplingFrequency / nfft;
	}
	while (samplingFrequency / nfft > dfMin || nfft <= my maxnCoefficients) nfft *= 2;
	freqStep = samplingFrequency / nfft;
	
	if ((thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1,
			0, samplingFrequency / 2, nfft / 2 + 1, freqStep, 0)) == NULL) return NULL;
		
	for (i=1; i <= my nx; i++)
	{
		double t = Sampled_indexToX (me, i);
		Spectrum spec = LPC_to_Spectrum (me, t, dfMin, bandwidthReduction, deEmphasisFrequency);
		if (spec == NULL) goto end;
		for (j=1; j <= spec -> nx; j++)
		{
			double re = spec -> z[1][j], im = spec -> z[2][j];
			thy z[j][i] =  re * re + im * im;
		}
		forget (spec);
	}	
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

/* End of file LPC_to_Spectrogram.c */
