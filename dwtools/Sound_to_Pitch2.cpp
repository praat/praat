/* Sound_to_Pitch2.c
 *
 * Copyright (C) 1993-2019 David Weenink
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

static void spec_enhance_SHS (VEC const & a) {
	Melder_assert (a.size >= 2);

	autoINTVEC posmax = raw_INTVEC ((a.size + 1) / 2);
	integer nmax = 0;
	if (a [1] > a [2])
		posmax [++ nmax] = 1;

	for (integer i = 2; i <= a.size - 1; i ++)
		if (a [i] > a [i - 1] && a [i] >= a [i + 1])
			posmax [++ nmax] = i;
		
	if (a [a.size] > a [a.size - 1])
		posmax [++ nmax] = a.size;

	if (nmax == 1) {
		a.part (1, posmax [1] - 3) <<= 0.0;
		a.part (posmax [1] + 3, a.size) <<= 0.0;
	} else {
		for (integer i = 2; i <= nmax; i ++)
			a.part (posmax [i - 1] + 3, posmax [i] - 3) <<= 0.0;
	}
}

static void spec_smoooth_SHS (VEC const& a) {
	/*
		Convolve in-place with the small symmetric moving-average
		kernel { 0.25, 0.5, 0.25 }, aligned around its second element.
		The basic equation for an output element a_new [i] is:
			a_new [i] := 0.25 * (a [i - 1] + 2.0 * a [i] + a [i + 1])

		The procedure is performed in place, i.e., the vector a_new[]
		appears as the new version of the vector a[], so that care has
		to be taken to timely save elements that will be overwritten.
		At the edges we perform "same" convolution, meaning that
		the output vector has the same number of elements as the
		input vector (this is a natural situation in case of in-place
		filtering). The elements just beyond the edges of the vector,
		namely a [0] and a [a.size + 1], are assumed to be zero.
	*/
	double a_i_minus_1 = 0.0;   // save a [i - 1], for i == 1
	for (integer i = 1; i <= a.size - 1; i ++) {
		const double a_i = a [i];   // save a [i]
		a [i] = 0.25 * (a_i_minus_1 + 2.0 * a_i + a [i + 1]);
		a_i_minus_1 = a_i;
	}
	a [a.size] = 0.25 * (a_i_minus_1 + 2.0 * a [a.size]);
}

autoPitch Sound_to_Pitch_shs (Sound me, double timeStep, double minimumPitch, double maximumFrequency,
	double ceiling, integer maxnSubharmonics, integer maxnCandidates, double compressionFactor, integer numberOfPointsPerOctave)
{
	try {
		const double newSamplingFrequency = 2.0 * maximumFrequency;
		const double windowDuration = 2.0 / minimumPitch, halfWindow = 0.5 * windowDuration;
		const double atans = numberOfPointsPerOctave * NUMlog2 (65.0 / 50.0) - 1.0;
		/*
			Number of speech samples in the downsampled signal in each frame:
			100 for windowDuration == 0.04 and newSamplingFrequency == 2500
		*/
		const integer numberOfSamples = Melder_iround (windowDuration * newSamplingFrequency);
		const double frameDuration = numberOfSamples / newSamplingFrequency;
		
		integer nfft = 256; // the minimum number of points for the FFT
		while (nfft < numberOfSamples)
			nfft *= 2;
		const integer nfft2 = nfft / 2 + 1;
		const double fftframeDuration = nfft / newSamplingFrequency;
		const double df = newSamplingFrequency / nfft;
		/*
			The number of points on the octave scale.
		*/
		const double fminl2 = NUMlog2 (minimumPitch), fmaxl2 = NUMlog2 (maximumFrequency);
		const integer numberOfFrequencyPoints = Melder_ifloor ((fmaxl2 - fminl2) * numberOfPointsPerOctave);
		const double dfl2 = (fmaxl2 - fminl2) / (numberOfFrequencyPoints - 1);

		autoSound sound = Sound_resample (me, newSamplingFrequency, 50);
		integer numberOfFrames;
		double firstTime;
		Sampled_shortTermAnalysis (sound.get(), windowDuration, timeStep, & numberOfFrames, & firstTime);
		autoSound fftframe = Sound_createSimple (1, fftframeDuration, newSamplingFrequency);
		autoSound analysisframe = Sound_createSimple (1, frameDuration, newSamplingFrequency);
		autoSound hamming = Sound_createHamming (frameDuration, newSamplingFrequency);
		autoPitch thee = Pitch_create (my xmin, my xmax, numberOfFrames, timeStep, firstTime, ceiling, maxnCandidates);
		autoVEC cc = zero_VEC (numberOfFrames);
		autoVEC specAmp = raw_VEC (nfft2);
		autoVEC fl2 = raw_VEC (nfft2);
		autoVEC yv2 = raw_VEC (nfft2);
		autoVEC arctg = raw_VEC (numberOfFrequencyPoints);
		autoVEC al2 = raw_VEC (numberOfFrequencyPoints);

		Melder_assert (fftframe -> nx >= numberOfSamples);
		Melder_assert (hamming -> nx == numberOfSamples);
		Melder_assert (analysisframe -> nx == numberOfSamples);
		/*
			Compute the absolute value of the globally largest amplitude w.r.t. the global mean.
		*/
		const double globalMean = Sound_localMean (sound.get(), sound -> xmin, sound -> xmax);
		const double globalPeak = Sound_localPeak (sound.get(), sound -> xmin, sound -> xmax, globalMean);
		/*
			For the cubic spline interpolation we need the frequencies on an octave
			scale, i.e., a log2 scale. All frequencies should be DIFFERENT, otherwise
			the cubic spline interpolation will give corrupt results.
			Because log2(f==0) is not defined, we use the heuristic: f [2] - f [1] == f [3] - f [2].
		*/
		for (integer i = 2; i <= nfft2; i ++)
			fl2 [i] = NUMlog2 ((i - 1) * df);
		fl2 [1] = 2.0 * fl2 [2] - fl2 [3];
		/*
			Calculate frequencies regularly spaced on a log2-scale and the frequency weighting function.
		*/
		for (integer i = 1; i <= numberOfFrequencyPoints; i ++)
			arctg [i] = 0.5 + atan (3.0 * (i - atans) / numberOfPointsPerOctave) / NUMpi;
		/*
			Perform the analysis on all frames.
		*/
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const Pitch_Frame pitchFrame = & thy frames [iframe];
			const double tmid = Sampled_indexToX (thee.get(), iframe); // The center of this frame
			/*
				Copy a frame from the sound, apply a hamming window. Get local 'intensity'
			*/
			Sound_into_Sound (sound.get(), analysisframe.get(), tmid - halfWindow);
			Sounds_multiply (analysisframe.get(), hamming.get());
			const double localMean = Sound_localMean (sound.get(), tmid - 3.0 * halfWindow, tmid + 3.0 * halfWindow);
			const double localPeak = Sound_localPeak (sound.get(), tmid - halfWindow, tmid + halfWindow, localMean);
			pitchFrame -> intensity = localPeak > globalPeak ? 1.0 : localPeak / globalPeak;
			/*
				Get the Fourier spectrum.
			*/
			fftframe -> z[1].part (1, analysisframe -> nx) <<= analysisframe -> z [1]; // 
			autoSpectrum spec = Sound_to_Spectrum (fftframe.get(), true);
			Melder_assert (spec -> nx == nfft2);
			/*
				From complex spectrum to amplitude spectrum.
			*/
			for (integer j = 1; j <= nfft2; j ++) {
				const double rs = spec -> z [1] [j], is = spec -> z [2] [j];
				specAmp [j] = sqrt (rs * rs + is * is);
			}
			/*
				Enhance the peaks in the spectrum.
			*/
			spec_enhance_SHS (specAmp.get());
			/*
				Smooth the enhanced spectrum.
			*/
			spec_smoooth_SHS (specAmp.get());
			/*
				Go to a logarithmic scale and perform cubic spline interpolation to get
				spectral values for the increased number of frequency points.
			*/
			NUMcubicSplineInterpolation_getSecondDerivatives (yv2.get(), fl2.get(), specAmp.get(), 1e30, 1e30);
			for (integer j = 1; j <= numberOfFrequencyPoints; j ++) {
				const double f = fminl2 + (j - 1) * dfl2;
				al2 [j] = NUMcubicSplineInterpolation (fl2.get(), specAmp.get(), yv2.get(), f);
			}
			/*
				Multiply by frequency selectivity of the auditory system.
			*/
			for (integer j = 1; j <= numberOfFrequencyPoints; j ++)
				al2 [j] = ( al2 [j] > 0.0 ? al2 [j] * arctg [j] : 0.0 );
			/*
				The subharmonic summation. Shift spectra in octaves and sum.
			*/
			Pitch_Frame_init (pitchFrame, maxnCandidates);
			autoVEC sumspec = zero_VEC (numberOfFrequencyPoints);
			pitchFrame -> candidates. resize (pitchFrame -> nCandidates = 0);   // !!!!!

			double hm = 1.0;
			for (integer m = 1; m <= maxnSubharmonics + 1; m ++) {
				const integer kb = 1 + Melder_ifloor (numberOfPointsPerOctave * NUMlog2 (m));
				for (integer k = kb; k <= numberOfFrequencyPoints; k ++)
					sumspec [k - kb + 1] += al2 [k] * hm;
				hm *= compressionFactor;
			}
			/*
				First register the voiceless candidate (always present).
			*/
			Pitch_Frame_addPitch (pitchFrame, 0.0, 0.0, maxnCandidates);
			/*
				Get the best local estimates for the pitch as the maxima of the
				subharmonic sum spectrum by parabolic interpolation on three points:
				The formula for a parabola with a maximum is:
					y(x) = a - b (x - c)^2 with a, b, c >= 0
				The three points are (-x, y1), (0, y2) and (x, y3).
				The solution for a (the maximum) and c (the position) is:
				a = (2 y1 (4 y2 + y3) - y1^2 - (y3 - 4 y2)^2)/( 8 (y1 - 2 y2 + y3)
				c = dx (y1 - y3) / (2 (y1 - 2 y2 + y3))
				(b = (2 y2 - y1 - y3) / (2 dx^2) )
			*/
			for (integer k = 2; k <= numberOfFrequencyPoints - 1; k ++) {
				const double y1 = sumspec [k - 1], y2 = sumspec [k], y3 = sumspec [k + 1];
				if (y2 > y1 && y2 >= y3) {
					const double denum = y1 - 2.0 * y2 + y3, tmp = y3 - 4.0 * y2;
					const double x = dfl2 * (y1 - y3) / (2.0 * denum);
					const double f = pow (2.0, fminl2 + (k - 1) * dfl2 + x);
					const double strength = (2.0 * y1 * (4.0 * y2 + y3) - y1 * y1 - tmp * tmp) / (8.0 * denum);
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
			double pitch_strength, f0;
			Pitch_Frame_getPitch (pitchFrame, & f0, & pitch_strength);
			if (f0 > 0.0)
				cc [iframe] = Sound_correlateParts (sound.get(), tmid - 1.0 / f0, tmid, 1.0 / f0);
		}
		/*
			Base V/UV decision on correlation coefficients.
			Resize the pitch strengths w.r.t. the cc.
		*/
		constexpr double vuvCriterion = 0.52;
		for (integer i = 1; i <= numberOfFrames; i ++)
			Pitch_Frame_resizeStrengths (& thy frames [i], cc [i], vuvCriterion);
		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Pitch (shs) created.");
	}
}

autoPitch Sound_to_Pitch_SPINET (Sound me, double timeStep, double windowDuration, double minimumFrequencyHz, double maximumFrequencyHz, integer nFilters, double ceiling, int maxnCandidates) {
	try {
		autoSPINET him = Sound_to_SPINET (me, timeStep, windowDuration, minimumFrequencyHz, maximumFrequencyHz, nFilters, 0.4, 0.6);
		autoPitch thee = SPINET_to_Pitch (him.get(), 0.15, ceiling, maxnCandidates);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Pitch (SPINET) created.");
	}
}

/* End of file Sound_to_Pitch2.cpp */
