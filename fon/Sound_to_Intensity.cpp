/* Sound_to_Intensity.cpp
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
 * pb 2003/05/20 default time step is four times oversampling
 * pb 2003/07/10 NUMbessel_i0_f
 * pb 2003/11/19 Sound_to_Intensity veryAccurate
 * pb 2003/12/15 removed bug introduced by previous change
 * pb 2004/10/27 subtractMean
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2007/01/27 for stereo sounds, add channel energies
 * pb 2007/02/14 honoured precondition of Sampled_shortTermAnalysis (by checking whether minimumPitch is defined)
 * pb 2008/01/19 double
 * pb 2011/03/04 C++
 * pb 2011/03/28 C++
 */

#include "Sound_to_Intensity.h"

static Intensity Sound_to_Intensity_ (Sound me, double minimumPitch, double timeStep, int subtractMeanPressure) {
	try {
		/*
		 * Preconditions.
		 */
		if (! NUMdefined (minimumPitch)) Melder_throw ("(Sound-to-Intensity:) Minimum pitch undefined.");
		if (! NUMdefined (timeStep)) Melder_throw ("(Sound-to-Intensity:) Time step undefined.");
		if (timeStep < 0.0) Melder_throw ("(Sound-to-Intensity:) Time step should be zero or positive instead of ", timeStep, ".");
		if (my dx <= 0.0) Melder_throw ("(Sound-to-Intensity:) The Sound's time step should be positive.");
		if (minimumPitch <= 0.0) Melder_throw ("(Sound-to-Intensity:) Minimum pitch should be positive.");
		/*
		 * Defaults.
		 */
		if (timeStep == 0.0) timeStep = 0.8 / minimumPitch;   // default: four times oversampling Hanning-wise

		double windowDuration = 6.4 / minimumPitch;
		Melder_assert (windowDuration > 0.0);
		double halfWindowDuration = 0.5 * windowDuration;
		long halfWindowSamples = halfWindowDuration / my dx;
		autoNUMvector <double> amplitude (- halfWindowSamples, halfWindowSamples);
		autoNUMvector <double> window (- halfWindowSamples, halfWindowSamples);

		for (long i = - halfWindowSamples; i <= halfWindowSamples; i ++) {
			double x = i * my dx / halfWindowDuration, root = 1 - x * x;
			window [i] = root <= 0.0 ? 0.0 : NUMbessel_i0_f ((2 * NUMpi * NUMpi + 0.5) * sqrt (root));
		}

		long numberOfFrames;
		double thyFirstTime;
		try {
			Sampled_shortTermAnalysis (me, windowDuration, timeStep, & numberOfFrames, & thyFirstTime); therror
		} catch (...) {
			Melder_throw ("The duration of the sound in an intensity analysis should be at least 6.4 divided by the minimum pitch (", minimumPitch, " Hz), "
				"i.e. at least ", 6.4 / minimumPitch, " s, instead of ", my xmax - my xmin, " s.");
		}
		autoIntensity thee = Intensity_create (my xmin, my xmax, numberOfFrames, timeStep, thyFirstTime);
		for (long iframe = 1; iframe <= numberOfFrames; iframe ++) {
			double midTime = Sampled_indexToX (thee.peek(), iframe);
			long midSample = Sampled_xToNearestIndex (me, midTime);
			long leftSample = midSample - halfWindowSamples, rightSample = midSample + halfWindowSamples;
			double sumxw = 0.0, sumw = 0.0, intensity;
			if (leftSample < 1) leftSample = 1;
			if (rightSample > my nx) rightSample = my nx;

			for (long channel = 1; channel <= my ny; channel ++) {
				for (long i = leftSample; i <= rightSample; i ++) {
					amplitude [i - midSample] = my z [channel] [i];
				}
				if (subtractMeanPressure) {
					double sum = 0.0;
					for (long i = leftSample; i <= rightSample; i ++) {
						sum += amplitude [i - midSample];
					}
					double mean = sum / (rightSample - leftSample + 1);
					for (long i = leftSample; i <= rightSample; i ++) {
						amplitude [i - midSample] -= mean;
					}
				}
				for (long i = leftSample; i <= rightSample; i ++) {
					sumxw += amplitude [i - midSample] * amplitude [i - midSample] * window [i - midSample];
					sumw += window [i - midSample];
				}
			}
			intensity = sumxw / sumw;
			if (intensity != 0.0) intensity /= 4e-10;
			thy z [1] [iframe] = intensity < 1e-30 ? -300 : 10 * log10 (intensity);
		}
		return thee.transfer();
	} catch (...) {
		rethrowmzero (me, ": intensity analysis not performed.");
	}
}

Intensity Sound_to_Intensity (Sound me, double minimumPitch, double timeStep, int subtractMeanPressure) {
	try {
		bool veryAccurate = false;
		if (veryAccurate) {
			autoSound up = Sound_upsample (me);   // because squaring doubles the frequency content, i.e. you get super-Nyquist components
			autoIntensity thee = Sound_to_Intensity_ (up.peek(), minimumPitch, timeStep, subtractMeanPressure);
			return thee.transfer();
		} else {
			autoIntensity thee = Sound_to_Intensity_ (me, minimumPitch, timeStep, subtractMeanPressure);
			return thee.transfer();
		}
	} catch (...) {
		rethrowzero;
	}
}

#define PLAYGROUND  0

#if PLAYGROUND
	struct structIntensity2 : structIntensity {
		structIntensity2 () { }
	};
	typedef structIntensity2 *Intensity2;
	_THING_DECLARE_AUTO (Intensity2)
#endif

IntensityTier Sound_to_IntensityTier (Sound me, double minimumPitch, double timeStep, int subtractMean) {
	try {
		autoIntensity intensity = Sound_to_Intensity (me, minimumPitch, timeStep, subtractMean);
		#if PLAYGROUND
			autoIntensity i2;   // compiler prevents this
			autoIntensity i3 = intensity;   // compiler prevents this
			i2 = intensity;   // compiler prevents this
			Intensity i4 = intensity.peek();   // compiler allows this
			Intensity i10 = intensity;   // but not  this
			Intensity i8 = i2.peek();   // compiler allows this
			Intensity i9 = i2;   // but not this
			i4 = intensity;   // or this
			Intensity2 i5 = NULL;
			i4 = i5;   // base-class object becomes derived-class object
			autoIntensity i6 = i5;
			autoIntensity2 i7;
			Intensity_downto_IntensityTier (i7.peek());   // a conversion that works
			Intensity_downto_IntensityTier (i7);   // and one that doesn't
			autoIntensity i11 = i2.peek();   // this is wrong but not detected...
			autoIntensity i22 = i2.transfer();   // ... and this is correct
			structTable table = { 0 };
			Table_initWithoutColumnNames (& table, 10, 10);
			Table theenew = new structTable (table);
			Melder_casual("rows %ld, columns %ld", theenew -> rows -> size, theenew -> numberOfColumns);
		#endif
		autoIntensityTier thee = Intensity_downto_IntensityTier (intensity.peek());
		return thee.transfer();
	} catch (...) {
		rethrowmzero (me, ": no IntensityTier created.");
	}
}

/* End of file Sound_to_Intensity.cpp */
