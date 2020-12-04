/* Sound_enhance.cpp
 *
 * Copyright (C) 1992-2012,2015-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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

autoSound Sound_lengthen_overlapAdd (Sound me, double fmin, double fmax, double factor) {
	try {
		if (my ny > 1)
			Melder_throw (U"Overlap-add works only on mono sounds.");
		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.get());
		autoPitch pitch = Sound_to_Pitch (sound.get(), 0.8 / fmin, fmin, fmax);
		autoPointProcess pulses = Sound_Pitch_to_PointProcess_cc (sound.get(), pitch.get());
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch.get());
		autoDurationTier duration = DurationTier_create (my xmin, my xmax);
		RealTier_addPoint (duration.get(), 0.5 * (my xmin + my xmax), factor);
		autoSound thee = Sound_Point_Pitch_Duration_to_Sound (sound.get(), pulses.get(), pitchTier.get(), duration.get(), 1.5 / fmin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not lengthened.");
	}
}

autoSound Sound_deepenBandModulation (Sound me, double enhancement_dB,
	double flow, double fhigh, double slowModulation, double fastModulation, double bandSmoothing)
{
	try {
		autoSound thee = Data_copy (me);
		double maximumFactor = pow (10.0, enhancement_dB / 20.0), alpha = sqrt (log (2.0));
		double alphaslow = alpha / slowModulation, alphafast = alpha / fastModulation;

		for (integer channel = 1; channel <= my ny; channel ++) {
			autoSound channelSound = Sound_extractChannel (me, channel);
			autoSpectrum orgspec = Sound_to_Spectrum (channelSound.get(), true);

			/*
				Keep the part of the sound that is outside the filter bank.
			*/
			autoSpectrum spec = Data_copy (orgspec.get());
			Spectrum_stopHannBand (spec.get(), flow, fhigh, bandSmoothing);
			autoSound filtered = Spectrum_to_Sound (spec.get());
			integer n = thy nx;
			VEC amp = thy z.row (channel);
			for (integer i = 1; i <= n; i ++)
				amp [i] = filtered -> z [1] [i];

			autoMelderProgress progress (U"Deepen band modulation...");
			double fmin = flow;
			while (fmin < fhigh) {
				/*
					Take a one-bark frequency band.
				*/
				double fmid_bark = NUMhertzToBark (fmin) + 0.5;
				double fmax = NUMbarkToHertz (NUMhertzToBark (fmin) + 1.0);
				if (fmax > fhigh) fmax = fhigh;
				Melder_progress (fmin / fhigh, U"Band: ", Melder_fixed (fmin, 0), U" ... ", Melder_fixed (fmax, 0), U" Hz");
				spec -> z.all()  <<=  orgspec -> z.all();
				Spectrum_passHannBand (spec.get(), fmin, fmax, bandSmoothing);
				autoSound band = Spectrum_to_Sound (spec.get());
				/*
					Compute a relative intensity contour.
				*/
				autoSound intensity = Data_copy (band.get());
				n = intensity -> nx;
				amp = intensity -> z.row (1);
				for (integer i = 1; i <= n; i ++)
					amp [i] = 10.0 * log10 (amp [i] * amp [i] + 1e-6);
				autoSpectrum intensityFilter = Sound_to_Spectrum (intensity.get(), true);
				n = intensityFilter -> nx;
				for (integer i = 1; i <= n; i ++) {
					double frequency = intensityFilter -> x1 + (i - 1) * intensityFilter -> dx;
					double slow = alphaslow * frequency, fast = alphafast * frequency;
					double factor = exp (- fast * fast) - exp (- slow * slow);
					intensityFilter -> z [1] [i] *= factor;
					intensityFilter -> z [2] [i] *= factor;
				}
				intensity = Spectrum_to_Sound (intensityFilter.get());
				n = intensity -> nx;
				amp = intensity -> z.row (1);
				for (integer i = 1; i <= n; i ++)
					amp [i] = pow (10.0, amp [i] / 2.0);
				/*
					Clip to maximum enhancement.
				*/
				const double ceiling = 1.0 + (maximumFactor - 1.0) * (0.5 - 0.5 * cos (NUMpi * fmid_bark / 13.0));
				for (integer i = 1; i <= n; i ++)
					amp [i] = 1.0 / (1.0 / amp [i] + 1.0 / ceiling);

				n = thy nx;
				amp = thy z.row (channel);
				for (integer i = 1; i <= n; i ++)
					amp [i] += band -> z [1] [i] * intensity -> z [1] [i];

				fmin = fmax;
			}
		}
		Vector_scale (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": band modulation not deepened.");
	}
}

/* End of file Sound_enhance.cpp */
