/* Sound_to_SPINET.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20020813 GPL header
*/

#include "Sound_to_SPINET.h"
#include "NUM2.h"

static double fgamma (double x, long n)
{
	double x2p1 = 1 + x * x, d = x2p1; long i;
	for (i=2; i <= n; i++) d *= x2p1;
	return 1 / d;
}

/*
	precondition:
	0 < minimumFrequencyHz < maximumFrequencyHz
*/

SPINET Sound_to_SPINET (Sound me, double timeStep, double windowDuration,
	double minimumFrequencyHz, double maximumFrequencyHz, long nFilters,
	double excitationErbProportion, double inhibitionErbProportion)
{
	Sound window = NULL, frame = NULL; SPINET thee = NULL;
	long i, j, k, numberOfFrames;
	double firstTime, b = 1.02, samplingFrequency = 1 / my dx;
	double *f = NULL, *bw = NULL, *aex = NULL, *ain = NULL;
	
	if (timeStep < my dx) timeStep = my dx;
	if (maximumFrequencyHz > samplingFrequency / 2) maximumFrequencyHz = samplingFrequency / 2;
			
	if (! Sampled_shortTermAnalysis (me, windowDuration, timeStep, &numberOfFrames, &firstTime) ||
		! (thee = SPINET_create (my xmin, my xmax, numberOfFrames, timeStep, firstTime,
			minimumFrequencyHz, maximumFrequencyHz, nFilters,
			excitationErbProportion, inhibitionErbProportion)) ||
		! (window = Sound_createGaussian (windowDuration, samplingFrequency)) ||
		! (frame = Sound_createSimple (windowDuration, samplingFrequency)) ||
		! (f = NUMdvector (1, nFilters)) || ! (bw = NUMdvector (1, nFilters)) ||
		! (aex = NUMdvector (1, nFilters)) || ! (ain = NUMdvector (1, nFilters))) goto cleanup;
				
	/*
		Cochlear filterbank: gammatone
	*/
	
	for (i=1; i <= nFilters; i++)
	{
		f[i] = NUMerbToHertz (thy y1 + (i - 1) * thy dy);
		bw[i] = 2 * NUMpi * b * (f[i] * (6.23e-6 * f[i] + 93.39e-3) + 28.52);
	}
	
	Melder_progress (0.0, "SPINET analysis");
	
	for (i=1; i <= nFilters; i++)
	{
		Sound gammaTone = NULL, filtered = NULL;
		/* Contribution of outer & middle ear and phase locking */
		double bb = (f[i] / 1000) * exp (- f[i] / 1000);
		/* Time where gammafunction envelope has its maximum */
		double tgammaMax = (thy gamma - 1) / bw[i];
		/* Amplitude at tgammaMax */
		double gammaMaxAmplitude = pow ((thy gamma - 1) / (NUMe * bw[i]), (thy gamma - 1));
		double timeCorrection = tgammaMax - windowDuration / 2;
		if (! (gammaTone = Sound_createGammaTone (0, 0.1, samplingFrequency,
				thy gamma, b, f[i], 0, 0, 0)) ||
			/* filtering can be made 30% faster by taking Spectrum(me) outside the loop */
			! (filtered = Sounds_convolve (me, gammaTone))) { forget (gammaTone); goto cleanup; }
		/*
			To energy measure: weigh with broad-band transfer function  
		*/
		for (j=1; j <= numberOfFrames; j++)
		{
			Sound_into_Sound (filtered, frame, Sampled_indexToX (thee, j) + timeCorrection);
			Sounds_multiply (frame, window);
			thy y[i][j] = Sound_power (frame) * bb / gammaMaxAmplitude;
		}
		forget (filtered); forget (gammaTone);
		if (! Melder_progress ((double)i / nFilters, "SPINET: filter %ld from %ld",
			i, nFilters)) goto cleanup;		
	}

	/*
		Excitatory and inhibitory area functions
	*/

	for (i=1; i <= nFilters; i++)
	{
		for (k=1; k <= nFilters; k++)
		{
			double fr = (f[k] - f[i]) / bw[i];
			aex[i] += fgamma (fr / thy excitationErbProportion, thy gamma);	
			ain[i] += fgamma (fr / thy inhibitionErbProportion, thy gamma);
		}
	}
	
	/*
		On-center off-surround interactions
	*/

	for (j=1; j <= numberOfFrames; j++)
		for (i=1; i <= nFilters; i++)
		{
			double a = 0;
			for (k=1; k <= nFilters; k++)
			{
				double fr = (f[k] - f[i]) / bw[i];
				double hexsq = fgamma (fr / thy excitationErbProportion, thy gamma);
				double hinsq = fgamma (fr / thy inhibitionErbProportion, thy gamma);
				a += thy y[k][j] * (hexsq / aex[i] - hinsq / ain[i]);
			}
			thy s[i][j] = a > 0 ? a : 0;
		}
	Melder_progress (1.0, NULL);
cleanup:
	NUMdvector_free (aex, 1); NUMdvector_free (ain, 1);
	NUMdvector_free (f, 1); NUMdvector_free (bw, 1);
	forget (window); forget (frame);
	if (! Melder_hasError()) return thee;
	forget (thee);
	return Melder_errorp ("Sound_to_SPINET: not performed.");
}

/* End of file Sound_to_SPINET.c */
