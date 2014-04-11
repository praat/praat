/* Sound_to_Pitch2.c
 *
 * Copyright (C) 1993-2007 David Weenink
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
 djmw 20021106 Latest modification
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20070103 Sound interface changes
*/

#include "Sound_to_Pitch2.h"
#include "Pitch_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_SPINET.h"
#include "SPINET_to_Pitch.h"
#include "NUM2.h"

static int spec_enhance_SHS (double a[], long n)
{
	long i, j, nmax = 0, *posmax;
	if (n < 2 || ! (posmax = NUMlvector (1, (n + 1) / 2))) return 0;
	if (a[1] > a[2]) posmax[++nmax] = 1;
	for (i=2; i <= n-1; i++) if (a[i] > a[i-1] && a[i] >= a[i+1]) posmax[++nmax] = i;
	if (a[n] > a[n-1]) posmax[++nmax] = n;
	if (nmax == 1)
	{
		for (j=1; j <= posmax[1]-3; j++) a[j] = 0;
		for (j=posmax[1]+3; j <= n; j++) a[j] = 0;
	}
	else for (i=2; i <= nmax; i++) for (j=posmax[i-1]+3; j <= posmax[i]-3; j++) a[j] = 0;
	NUMlvector_free (posmax, 1);
	return 1;
}

static void spec_smoooth_SHS (double a[], long n)
{
	double ai, aim1 = 0; long i;
	for (i=1; i <= n-1; i++)
	{
		ai = a[i]; a[i] = (aim1 + 2 * ai + a[i+1]) / 4; aim1 = ai;
	}	
}

Pitch Sound_to_Pitch_shs (Sound me, double timeStep, double minimumPitch,
	double maximumFrequency, double ceiling, long maxnSubharmonics, long maxnCandidates,
	double compressionFactor, long nPointsPerOctave)
{
	Sound sound = NULL, frame = NULL, hamming = NULL; Pitch thee = NULL;
	double *specAmp = NULL, firstTime, newSamplingFrequency = 2 * maximumFrequency;
	double df, frameDuration, windowDuration = 2 / minimumPitch, halfWindow = windowDuration / 2;
	/* Linear separated frequencies from FFT-spectrum on an octave scale */
	double *fl2 = NULL;
	/* Number of frequency points, domain, point distance, frequencies & amplitudes of */
	/* interpolated spectrum on an octave scale */
	long nFrequencyPoints;
	double fmaxl2, fminl2, dfl2, *y2 = NULL, *al2 = NULL;
	/* Scale factor & sensitivity of auditory system */
	double atans = nPointsPerOctave * NUMlog2 (65.0 / 50.0) - 1, *arctg = NULL; 
	/* correlation between successive pitch periods */
	double *cc = NULL; 
	double vuvCriterium = 0.52, globalMean, globalPeak;
	long i, j, k, m, numberOfFrames, nfft = 1, nfft2;
	/* Number of speech samples in the downsampled signal in each frame: */
	/* 100 for windowDuration == 0.04 and newSamplingFrequency == 2500 */
	long nx = floor (windowDuration * newSamplingFrequency + 0.5);
	
	/*
		The minimum number of points for the fft is 256.
	*/
	
	while ((nfft *= 2) < nx || nfft <= 128) ;
	nfft2 = nfft / 2 + 1;
	frameDuration = nfft / newSamplingFrequency;
	df = newSamplingFrequency / nfft;
	
	/*
		The number of points on the octave scale
	*/
	
	fminl2 = NUMlog2 (minimumPitch); fmaxl2 = NUMlog2 (maximumFrequency);
	nFrequencyPoints = (fmaxl2 - fminl2) * nPointsPerOctave;
	dfl2 = (fmaxl2 - fminl2) / (nFrequencyPoints - 1);
	
	if (! (sound = Sound_resample (me, newSamplingFrequency, 50)) ||
		! Sampled_shortTermAnalysis (sound, windowDuration, timeStep, &numberOfFrames, &firstTime) ||
		! (frame = Sound_createSimple (1, frameDuration, newSamplingFrequency)) ||
		! (hamming = Sound_createHamming (nx / newSamplingFrequency, newSamplingFrequency)) ||
		! (thee = Pitch_create (my xmin, my xmax, numberOfFrames, timeStep, firstTime,
				ceiling, maxnCandidates)) ||
		! (cc = NUMdvector (1, numberOfFrames)) || ! (specAmp = NUMdvector (1, nfft2)) ||
		! (fl2 = NUMdvector (1, nfft2)) || ! (y2 = NUMdvector (1, nfft2)) ||
		! (arctg = NUMdvector (1, nFrequencyPoints)) ||
		! (al2 = NUMdvector (1, nFrequencyPoints))) goto cleanup;

	Melder_assert (frame->nx >= nx);
	Melder_assert (hamming->nx == nx);

	/*
		Compute the absolute value of the globally largest amplitude w.r.t. the global mean.
	*/

	Sound_localMean (sound, sound->xmin, sound->xmax, &globalMean);
	Sound_localPeak (sound, sound->xmin, sound->xmax, globalMean, &globalPeak);

	/*
		For the cubic spline interpolation we need the frequencies on an octave
		scale, i.e., a log2 scale. All frequencies must be DIFFERENT, otherwise 
		the cubic spline interpolation will give corrupt results.
		Because log2(f==0) is not defined, we use the heuristic: f[2]-f[1] == f[3]-f[2].
	*/

	for (i=2; i <= nfft2; i++) fl2[i] = NUMlog2 ((i-1) * df);
	fl2[1] = 2 * fl2[2] - fl2[3];

	/*
		Calculate frequencies regularly spaced on a log2-scale and
		the frequency weighting function.
	*/

	for (i=1; i <= nFrequencyPoints; i++)
	{
		arctg[i] = 0.5 + atan (3 * (i - atans) / nPointsPerOctave) / NUMpi;
	} 

	/*
		Perform the analysis on all frames.
	*/
	
	for (i=1; i <= numberOfFrames; i++)
	{
		Pitch_Frame pitchFrame = &thy frame[i]; Spectrum spec = NULL;
		double hm = 1, f0, strength, localMean, localPeak, *sumspec = NULL;
		double tmid = Sampled_indexToX (thee, i); /* The center of this frame */
		long nx_tmp = frame -> nx;
		
		/*
			Copy a frame from the sound, apply a hamming window.
			Get local 'intensity'
		*/
		
		frame -> nx = nx; /*begin vies */
		Sound_into_Sound (sound, frame, tmid - halfWindow);
		Sounds_multiply (frame, hamming);
		Sound_localMean (sound, tmid - 3 * halfWindow, tmid + 3 * halfWindow,
				&localMean);
		Sound_localPeak (sound, tmid - halfWindow, tmid + halfWindow,
				localMean, &localPeak); 
		pitchFrame->intensity = localPeak > globalPeak ? 1 : localPeak / globalPeak;
		frame -> nx = nx_tmp; /* einde vies */
		
		/*
			Get the Fourier spectrum.
		*/
		
		if (! (spec = Sound_to_Spectrum (frame, TRUE))) goto cleanup;
		Melder_assert (spec->nx == nfft2);
		
		/*
			From complex spectrum to amplitude spectrum.
		*/
		
		for (j=1; j <= nfft2; j++)
		{
			double rs = spec->z[1][j], is = spec->z[2][j];
			specAmp[j] = sqrt (rs * rs + is * is);
		}
		
		/*
			Enhance the peaks in the spectrum.
		*/
		
		if (! spec_enhance_SHS (specAmp, nfft2)) { forget (spec); goto cleanup; };
		
		/*
			Smooth the enhanced spectrum.
		*/
		
		spec_smoooth_SHS (specAmp, nfft2);
		
		/*
			Go to a logarithmic scale and perform cubic spline interpolation to get 
			spectral values for the increased number of frequency points.
		*/
		
		if (! NUMspline_d (fl2, specAmp, nfft2, 1e30, 1e30, y2))
			{ forget (spec); goto cleanup; };
		for (j=1; j <= nFrequencyPoints; j++)
		{
			double f = fminl2 + (j-1) * dfl2;
			NUMsplint_d (fl2, specAmp, y2, nfft2, f, &al2[j]);
		}
		
		/*
			Multiply by frequency selectivity of the auditory system.
		*/
		
		for (j=1; j <= nFrequencyPoints; j++) al2[j] = al2[j] > 0 ?
			al2[j] * arctg[j] : 0;
		
		/*
			The subharmonic summation.
			Shift spectra in octaves and sum. 
		*/
		
		if (! Pitch_Frame_init (pitchFrame, maxnCandidates) ||
			! (sumspec = NUMdvector (1, nFrequencyPoints))) { forget (spec); goto cleanup; }
		pitchFrame->nCandidates = 0; /* !!!!! */
		
		for (m=1; m <= maxnSubharmonics+1; m++)
		{
			long kb = 1 + floor (nPointsPerOctave * NUMlog2 (m));
			for (k=kb; k <= nFrequencyPoints; k++) 
			{
				sumspec[k-kb+1] += al2[k] * hm;
			}
			hm *= compressionFactor;
		}
		
		/*
			First register the voiceless candidate (always present).
		*/
		
		Pitch_Frame_addPitch (pitchFrame, 0, 0, maxnCandidates);
		
		/*
			Get the best local estimates for the pitch as the maxima of the
			subharmonic sum spectrum by parabolic interpolation on three points:
			The formula for a parabole with a maximum is:
				y(x) = a - b (x - c)^2 with a, b, c >= 0
			The three points are (-x, y1), (0, y2) and (x, y3).
			The solution for a (the maximum) and c (the position) is:
			a = (2 y1 (4 y2 + y3) - y1^2 - (y3 - 4 y2)^2)/( 8 (y1 - 2 y2 + y3)
			c = dx (y1 - y3) / (2 (y1 - 2 y2 + y3))
			(b = (2 y2 - y1 - y3) / (2 dx^2) )
   		*/
   		
		for (k=2; k <= nFrequencyPoints-1; k++)
		{
			double y1 = sumspec[k-1], y2 = sumspec[k], y3 = sumspec[k+1];
			if (y2 > y1 && y2 >= y3)
			{
				double denum = y1 - 2 * y2 + y3, tmp = y3 - 4 * y2;
				double x =  dfl2 * (y1 - y3) / (2 * denum);
				double f = pow (2, fminl2 + (k - 1) * dfl2 + x);
				double strength = (2 * y1 * (4 * y2 + y3) - y1 * y1 - tmp * tmp) / (8 * denum);
				Pitch_Frame_addPitch (pitchFrame, f, strength, maxnCandidates);
			}	
		}
		
		/*
			Check whether f0 corresponds to an actual periodicity T = 1 / f0:
			correlate two signal periods of duration T, one starting at the 
			middle of the interval and one starting T seconds before.
			If there is periodicity the correlation coefficient should be high.
			
			However, some sounds do not show any regularity, or very low 
			frequency and regularity, and nevertheless have a definite
			pitch, e.g. Shepard sounds.
		*/
		
		Pitch_Frame_getPitch (pitchFrame, &f0, &strength);
		if (f0 > 0) cc[i] = Sound_correlateParts (sound, tmid - 1.0 / f0, tmid, 1.0 / f0);
				
		forget (spec); NUMdvector_free (sumspec, 1);
	}
	
	/*
		Base V/UV decision on correlation coefficients.
		Resize the pitch strengths w.r.t. the cc.
	*/
	
	for (i=1; i <= numberOfFrames; i++)
		Pitch_Frame_resizeStrengths (& thy frame[i], cc[i], vuvCriterium);

cleanup:
	NUMdvector_free (fl2, 1); NUMdvector_free (specAmp, 1);
	NUMdvector_free (y2, 1); NUMdvector_free (al2, 1);
	NUMdvector_free (cc, 1); NUMdvector_free (arctg, 1);
	forget (sound); forget (hamming); forget (frame);
	if (! Melder_hasError()) return thee;
	forget (thee);
	return Melder_errorp ("Sound_to_Pitch_shs: not performed.");
}

Pitch Sound_to_Pitch_SPINET (Sound me, double timeStep, double windowDuration,
	double minimumFrequencyHz, double maximumFrequencyHz, long nFilters,
	double ceiling, int maxnCandidates)
{
	SPINET him; Pitch thee;
	if (! (him = Sound_to_SPINET (me, timeStep, windowDuration, minimumFrequencyHz,
		maximumFrequencyHz, nFilters, 0.4, 0.6))) return NULL;
	thee = SPINET_to_Pitch (him, 0.15, ceiling, maxnCandidates);
	forget (him);
	if (! Melder_hasError()) return thee;
	forget (thee);
	return NULL;
}

/* End of file Sound_to_Pitch2.c */
