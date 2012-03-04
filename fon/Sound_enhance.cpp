/* Sound_enhance.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2004/11/22 simplified Sound_to_Spectrum ()
 * pb 2007/01/28 made compatible with stereo sounds (by rejecting them)
 * pb 2007/07/22 renamed the overlap-add method in such a way that it does not sound like a trademark for diphone concatenation
 * pb 2008/01/19 double
 * pb 2011/06/07 C++
 */

#include "Manipulation.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "Sound_and_Spectrum.h"

Sound Sound_lengthen_overlapAdd (Sound me, double fmin, double fmax, double factor) {
	try {
		if (my ny > 1)
			Melder_throw ("Overlap-add works only on mono sounds.");
		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.peek());
		autoPitch pitch = Sound_to_Pitch (sound.peek(), 0.8 / fmin, fmin, fmax);
		autoPointProcess pulses = Sound_Pitch_to_PointProcess_cc (sound.peek(), pitch.peek());
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch.peek());
		autoDurationTier duration = DurationTier_create (my xmin, my xmax);
		RealTier_addPoint (duration.peek(), 0.5 * (my xmin + my xmax), factor);
		autoSound thee = Sound_Point_Pitch_Duration_to_Sound (sound.peek(), pulses.peek(), pitchTier.peek(), duration.peek(), 1.5 / fmin);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not lengthened.");
	}
}

Sound Sound_deepenBandModulation (Sound me, double enhancement_dB,
	double flow, double fhigh, double slowModulation, double fastModulation, double bandSmoothing)
{
	try {
		autoSound thee = Data_copy (me);
		double maximumFactor = pow (10, enhancement_dB / 20), alpha = sqrt (log (2.0));
		double alphaslow = alpha / slowModulation, alphafast = alpha / fastModulation;

		for (long channel = 1; channel <= my ny; channel ++) {
			autoSound channelSound = Sound_extractChannel (me, channel);
			autoSpectrum orgspec = Sound_to_Spectrum (channelSound.peek(), TRUE);

			/*
			 * Keep the part of the sound that is outside the filter bank.
			 */
			autoSpectrum spec = Data_copy (orgspec.peek());
			Spectrum_stopHannBand (spec.peek(), flow, fhigh, bandSmoothing);
			autoSound filtered = Spectrum_to_Sound (spec.peek());
			long n = thy nx;
			double *amp = thy z [channel];
			for (long i = 1; i <= n; i ++) amp [i] = filtered -> z [1] [i];

			autoMelderProgress progress (L"Deepen band modulation...");
			double fmin = flow;
			while (fmin < fhigh) {
				/*
				 * Take a one-bark frequency band.
				 */
				double fmid_bark = NUMhertzToBark (fmin) + 0.5, ceiling;
				double fmax = NUMbarkToHertz (NUMhertzToBark (fmin) + 1);
				if (fmax > fhigh) fmax = fhigh;
				Melder_progress (fmin / fhigh, L"Band: ", Melder_fixed (fmin, 0), L" ... ", Melder_fixed (fmax, 0), L" Hz");
				NUMmatrix_copyElements (orgspec -> z, spec -> z, 1, 2, 1, spec -> nx);
				Spectrum_passHannBand (spec.peek(), fmin, fmax, bandSmoothing);
				autoSound band = Spectrum_to_Sound (spec.peek());
				/*
				 * Compute a relative intensity contour.
				 */		
				autoSound intensity = Data_copy (band.peek());
				n = intensity -> nx;
				amp = intensity -> z [1];
				for (long i = 1; i <= n; i ++) amp [i] = 10 * log10 (amp [i] * amp [i] + 1e-6);
				autoSpectrum intensityFilter = Sound_to_Spectrum (intensity.peek(), TRUE);
				n = intensityFilter -> nx;
				for (long i = 1; i <= n; i ++) {
					double frequency = intensityFilter -> x1 + (i - 1) * intensityFilter -> dx;
					double slow = alphaslow * frequency, fast = alphafast * frequency;
					double factor = exp (- fast * fast) - exp (- slow * slow);
					intensityFilter -> z [1] [i] *= factor;
					intensityFilter -> z [2] [i] *= factor;
				}
				intensity.reset (Spectrum_to_Sound (intensityFilter.peek()));
				n = intensity -> nx;
				amp = intensity -> z [1];
				for (long i = 1; i <= n; i ++) amp [i] = pow (10, amp [i] / 2);
				/*
				 * Clip to maximum enhancement.
				 */
				ceiling = 1 + (maximumFactor - 1.0) * (0.5 - 0.5 * cos (NUMpi * fmid_bark / 13));
				for (long i = 1; i <= n; i ++) amp [i] = 1 / (1 / amp [i] + 1 / ceiling);

				n = thy nx;
				amp = thy z [channel];
				for (long i = 1; i <= n; i ++) amp [i] += band -> z [1] [i] * intensity -> z [1] [i];

				fmin = fmax;
			}
		}
		Vector_scale (thee.peek(), 0.99);
		/* Truncate. */
		thy xmin = my xmin;
		thy xmax = my xmax;
		thy nx = my nx;
		thy x1 = my x1;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": band modulation not deepened.");
	}
}

/* End of file Sound_enhance.cpp */
