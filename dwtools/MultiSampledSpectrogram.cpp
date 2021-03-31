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



Thing_implement (FunctionXSampledY, Function, 0);

//void structFunctionXSampledY :: v_info () {
//}

Thing_implement (FrequencyBin, Sampled, 0);

Thing_implement (MultiSampledSpectrogram, FunctionXSampledY, 0);

Thing_implement (ConstantQLogFSpectrogram, MultiSampledSpectrogram, 0);

void structMultiSampledSpectrogram :: v_info () {
	
}

void FunctionXSampledY_init ( FunctionXSampledY me, double xmin, double xmax, double ymin, double ymax, integer ny, double dy, double y1) {
	Function_init (me, xmin, xmax);
	my ymin = ymin;
	my ymax = my ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
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

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double tmin, double tmax, double fmin, double fmax, integer numberOfFrequencies, double df, double f1) {
	FunctionXSampledY_init (me, tmin, tmax, fmin, fmax, numberOfFrequencies, df, f1);
}

void MultiSampledSpectrogram_draw (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}

void MultiSampledSpectrogram_paint_inside (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	
}
void MultiSampledSpectrogram_paint (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	MultiSampledSpectrogram_paint_inside (me, g, tmin, tmax, fmin, fmax, garnish);
}

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double minimum, double maximum, bool garnish) {
	double fmin_log2 = log2 (2);
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
		MultiSampledSpectrogram_init (me.get(), tmin, tmax, ymin, ymax, numberOfSteps, dy, log2_f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create ConstantQLogFSpectrogram.");
	}
}

autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram (Sound me, double lowestFrequency, integer numberOfStepsPerOctave, integer numberOfSteps, double timeOversamplingFactor, double frequencyOversamplingFactor) {
	try {
		autoConstantQLogFSpectrogram thee = ConstantQLogFSpectrogram_create (my xmin, my xmax, lowestFrequency, numberOfStepsPerOctave, numberOfSteps);
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 * samplingFrequency;
		const double q = 1.0 / (pow (2.0, 1.0 / numberOfStepsPerOctave) - 1.0);
		Melder_require (thy ymax <= nyquistFrequency,
			U"The number of steps you want result in a maximum frequency which is above the Nyquist frequency of the sound. "
			"The maximum number of steps should not exceed ", Melder_iroundDown (numberOfStepsPerOctave * log2 (nyquistFrequency / lowestFrequency)), U".");
		if (timeOversamplingFactor < 1.0)
			timeOversamplingFactor = 4.0; // default oversampling
		const double alpha = 25.0/46.0, oneMinusAlpha = 1.0 - alpha;
		const integer maximumNumberOfAnalysisSamples = samplingFrequency * q / lowestFrequency;
		autoMAT windowedExp = raw_MAT (2, maximumNumberOfAnalysisSamples + 1);
		VEC windowedCos = windowedExp.row(1), windowedSin = windowedExp.row(2);
		for (integer ifreq = 1; ifreq <= numberOfSteps; ifreq ++) {
			const double frequency =  exp2 (thy v_getY (ifreq));
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
				frequencyBin -> z [1] [iframe] = NUMinner (amplitudePart, windowedCos.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample)) / actualNumberOfSamples;
				frequencyBin -> z [2] [iframe] = NUMinner (amplitudePart, windowedSin.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample)) / actualNumberOfSamples;
			}
			thy frequencyBins.addItem_move (frequencyBin.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLogFSpectrogram.");
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
