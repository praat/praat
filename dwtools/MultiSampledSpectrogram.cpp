/* MultiSampledSpectrogram.cpp
 * 
 * Copyright (C) 2021 David Weenink
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

#include "Collection.h"
#include "MultiSampledSpectrogram.h"

#include "oo_DESTROY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_COPY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_EQUAL.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_WRITE_TEXT.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_READ_TEXT.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_WRITE_BINARY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_READ_BINARY.h"
#include "MultiSampledSpectrogram_def.h"
#include "oo_DESCRIPTION.h"
#include "MultiSampledSpectrogram_def.h"


Thing_implement (FrequencyBin, Sampled, 0);

Thing_implement (MultiSampledSpectrogram, Sampled, 0);

Thing_implement (ConstantQLogFSpectrogram, MultiSampledSpectrogram, 0);

void structMultiSampledSpectrogram :: v_info () {
	
}

autoFrequencyBin FrequencyBin_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoFrequencyBin me = Thing_new (FrequencyBin);
		Sampled_init (me.get(), xmin, xmax, nx, dx, x1);
		my z = zero_MAT (2, my nx); 
		return me;
	} catch (MelderError) {
		Melder_throw (U"FrequencyBin not created.");
	}
}

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double fmin, double fmax, integer numberOfFrequencies, double df, double f1) {
	Sampled_init (me, fmin, fmax, numberOfFrequencies, df, f1);
}

void MultiSampledSpectrogram_draw (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}

void MultiSampledSpectrogram_paint_inside (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}
void MultiSampledSpectrogram_paint (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	MultiSampledSpectrogram_paint_inside (me, g, tmin, tmax, fmin, fmax, garnish);
}

void ConstantQLogFSpectrogram_paintInside (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double log2_fmin, double log2_fmax, double minimum, double maximum) {
	integer ixmin, ixmax, ifmin, ifmax;
	if (Sampled_getWindowSamples (me, log2_fmin, log2_fmax, & ifmin, & ifmax) == 0)
		return;
	Graphics_setWindow (g, tmin, tmax, log2_fmin, log2_fmax);
	integer numberOfFrames = Sampled_getWindowSamples (my frequencyBins.at [ifmax], tmin, tmax, & ixmin, & ixmax);
	autoMAT p = raw_MAT (1, numberOfFrames);
	
	if (minimum >= maximum) {
		/*
			(autoscaling)
			Find maximum power. No need for logarithm in the test
		*/
		double power_max = 0.0;
		for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
			FrequencyBin frequencyBin = my frequencyBins . at [ifreq];
			if ((numberOfFrames = Sampled_getWindowSamples (frequencyBin, tmin, tmax, & ixmin, & ixmax)) == 0)
				continue;
			for (integer iframe = ixmin; iframe <= ixmax; iframe ++) {
				double power = sqr (frequencyBin -> z [1] [iframe]) + sqr (frequencyBin -> z [2] [iframe]);
				if (power > power_max)
					power_max = power;
			}
		}
		if (power_max == 0.0)
			return; // empty
		maximum = 10.0 * log10 (power_max / 4e-10);
		minimum = maximum - 50.0; // 50 dB dynamic range
	}
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
		double xmin1, xmax1 ;
		const double dx = frequencyBin -> dx;
		const double log2_freq = Sampled_indexToX (me, ifreq);
		if ((numberOfFrames = Sampled_getWindowSamples (frequencyBin, tmin - 0.4999 * dx, tmax + 0.4999 * dx, & ixmin, & ixmax)) == 0)
			continue;
		p.resize (1, numberOfFrames);
		MAT z = frequencyBin -> z.get();
		integer index = 0;
		for (integer iframe = ixmin; iframe <= ixmax; iframe ++) {
			double power = sqr (z [1] [iframe]) + sqr (z [2] [iframe]);
			p [1] [ ++ index] = 10.0 * log10 (power / 4e-10);
		}
		double xmin = Sampled_indexToX (frequencyBin, ixmin) - 0.5 * dx;
		double xmax = Sampled_indexToX (frequencyBin, ixmax) + 0.5 * dx;
		double ymin = log2_freq - 0.5 * my dx; 
		double ymax = log2_freq + 0.5 * my dx;
		if (ifreq > 1) {
			Melder_clipRight (& xmin, xmin1);
			Melder_clipLeft (xmax1, & xmax);
		}
		Graphics_image (g, p.get(), xmin, xmax, ymin, ymax, minimum, maximum);
		xmin1 = xmin;
		xmax1 = xmax;
	}
}

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double minimum, double maximum, bool garnish) {
	Function_bidirectionalAutowindow (me, & fmin, & fmax);
	FrequencyBin frequencyBin = my frequencyBins.at [1];
	if (tmin >= tmax) {
		tmin = frequencyBin -> xmin;
		tmax = frequencyBin -> xmax;
	} 
	Graphics_setInner (g);
	ConstantQLogFSpectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (log2Hz)");
	}
	
}

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double tmin, double tmax, double f1, integer numberOfStepsPerOctave, integer numberOfSteps) {
	try {
		Melder_require (numberOfSteps > 1,
			U"The number of steps should be larger than 1.");
		autoConstantQLogFSpectrogram me = Thing_new (ConstantQLogFSpectrogram);
		const double fmax = f1 * exp2 (((double) numberOfSteps) / numberOfStepsPerOctave);
		const double log2_fmax = log2 (fmax), log2_f1 = log2 (f1); // * NUMlog10_2 = 0.3010299 ??  log10
		const double dy = (log2_fmax - log2_f1) / (numberOfSteps - 1);
		const double ymin = log2_f1 - 0.5 * dy, ymax = log2_fmax + 0.5 * dy;
		MultiSampledSpectrogram_init (me.get(), ymin, ymax, numberOfSteps, dy, log2_f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create ConstantQLogFSpectrogram.");
	}
}

autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram (Sound me, double lowestFrequency, double q, integer numberOfStepsPerOctave, integer numberOfSteps, double timeOversamplingFactor) {
	try {
		
		autoConstantQLogFSpectrogram thee = ConstantQLogFSpectrogram_create (my xmin, my xmax, lowestFrequency, numberOfStepsPerOctave, numberOfSteps);
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 * samplingFrequency;
		Melder_require (thy xmax <= nyquistFrequency,
			U"The number of steps you want result in a maximum frequency which is above the Nyquist frequency of the sound. "
			"The maximum number of steps should not exceed ", Melder_iroundDown (numberOfStepsPerOctave * log2 (nyquistFrequency / lowestFrequency)), U".");
		if (timeOversamplingFactor < 1.0)
			timeOversamplingFactor = 4.0; // default oversampling
		const integer maximumNumberOfAnalysisSamples = samplingFrequency * q / lowestFrequency;
		autoMAT windowedExp = raw_MAT (2, maximumNumberOfAnalysisSamples + 1);
		VEC windowedCos = windowedExp.row(1), windowedSin = windowedExp.row(2);
		for (integer ifreq = 1; ifreq <= numberOfSteps; ifreq ++) {
			const double frequency =  exp2 (Sampled_indexToX (thee.get(), ifreq));
			const integer halfNumberOfWindowSamples = Melder_iroundDown (0.5 * samplingFrequency * q / frequency);
			const integer numberOfWindowSamples = 2 * halfNumberOfWindowSamples + 1;
			double windowLength = numberOfWindowSamples * my dx;
			Melder_clipRight (& windowLength, my nx * my dx);
			const double dt = windowLength / timeOversamplingFactor;
			integer numberOfFrames;
			double t1;
			Sampled_shortTermAnalysis (me, windowLength, dt, & numberOfFrames, & t1);
			autoFrequencyBin frequencyBin = FrequencyBin_create (my xmin, my xmax, numberOfFrames, dt, t1);
			const integer windowCentreSampleNumber = halfNumberOfWindowSamples + 1;
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				const double x = (i - windowCentreSampleNumber) * my dx / (0.5 * windowLength);
				const double root = sqrt (Melder_clippedLeft (0.0, 1.0 - sqr (x)));   // clipping should be rare
				const double window = NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * root);
				windowedCos [i] =   window * cos (NUM2pi * q * (i - 1) / numberOfWindowSamples);
				windowedSin [i] = - window * sin (NUM2pi * q * (i - 1) / numberOfWindowSamples);
			}
			for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
				const double midTime = Sampled_indexToX (frequencyBin.get(), iframe);
				const integer soundCentreSampleNumber = Sampled_xToNearestIndex (me, midTime);   // time accuracy is half a sampling period

				integer leftSample = soundCentreSampleNumber - halfNumberOfWindowSamples;
				integer rightSample = soundCentreSampleNumber + halfNumberOfWindowSamples;
				/*
					Catch some edge cases, which are uncommon because Sampled_shortTermAnalysis() filtered out most problems.
				*/
				Melder_clipLeft (1_integer, & leftSample);
				Melder_clipRight (& rightSample, my nx);
				Melder_require (rightSample >= leftSample,
					U"Unexpected edge case: right sample (", rightSample, U") less than left sample (", leftSample, U").");
				const integer actualNumberOfSamples = rightSample - leftSample + 1;
				const integer windowFromSoundOffset = windowCentreSampleNumber - soundCentreSampleNumber;
				VEC amplitudePart = my z.row(1).part (leftSample, rightSample);
				frequencyBin -> z [1] [iframe] = NUMinner (amplitudePart, windowedCos.part (windowFromSoundOffset + leftSample,
					windowFromSoundOffset + rightSample)) / actualNumberOfSamples;
				frequencyBin -> z [2] [iframe] = NUMinner (amplitudePart, windowedSin.part (windowFromSoundOffset + leftSample, 
					windowFromSoundOffset + rightSample)) / actualNumberOfSamples;
			}
			thy frequencyBins.addItem_move (frequencyBin.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLogFSpectrogram.");
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
