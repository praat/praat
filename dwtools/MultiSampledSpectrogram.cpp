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

void windowShape_VEC_preallocated (VEC const& target, kSound_windowShape windowShape) {
	const integer n = target.size;
	const double imid = 0.5 * (double) (n + 1);
	switch (windowShape) {
		case kSound_windowShape::RECTANGULAR: {
			target  <<=  1.0; 
		} break; case kSound_windowShape::TRIANGULAR: {  // "Bartlett"
			for (integer i = 1; i <= n; i ++) {
				const double phase = (double) i / n;   // 0..1  TODO No window is not symmetric
				target [i] = 1.0 - fabs ((2.0 * phase - 1.0));
			} 
		} break; case kSound_windowShape::PARABOLIC: {  // "Welch"
			for (integer i = 1; i <= n; i ++) { 
				const double phase = (double) i / n;
				target [i] = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
			}
		} break; case kSound_windowShape::HANNING: {
			for (integer i = 1; i <= n; i ++) {
				const double phase = (double) i / n;
				target [i] = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
			}
		} break; case kSound_windowShape::HAMMING: {
			for (integer i = 1; i <= n; i ++) { 
				const double phase = (double) i / n;
				target [i] = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
			}
		} break; case kSound_windowShape::GAUSSIAN_1: {
			const double edge = exp (-3.0), onebyedge1 = 1.0 / (1.0 - edge);   // -0.5..+0.5
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-12.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_2: {
			const double edge = exp (-12.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-48.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_3: {
			const double edge = exp (-27.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-108.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_4: {
			const double edge = exp (-48.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-192.0 * phase * phase) - edge) * onebyedge1; 
			}
		} break; case kSound_windowShape::GAUSSIAN_5: {
			const double edge = exp (-75.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-300.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::KAISER_1: {
			const double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2.0 * NUMpi * sqrt (root));
			}
		} break; case kSound_windowShape::KAISER_2: {
			const double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi * NUMpi + 0.5);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * sqrt (root)); 
			}
		} break; default: {
			target  <<=  1.0;
		}
	}
}

autoVEC windowShape_VEC (integer n, kSound_windowShape windowShape) {
	autoVEC result = raw_VEC (n);
	windowShape_VEC_preallocated (result.get(), windowShape);
	return result;
}

Thing_implement (FrequencyBin, Sampled, 0);

double structFrequencyBin :: v_getValueAtSample (integer iframe, integer which , int unit) {
	if (unit == 0) {
		return ( which == 1 ? z [1] [iframe] : which == 2 ? z [2] [iframe] : undefined );
	}
	double power = sqr (z [1] [iframe]) + sqr (z [2] [iframe]);
	return ( unit == 1 ? power : unit == 2 ? 10.0 * log10 (power / 4e-10) : undefined );
}

Thing_implement (MultiSampledSpectrogram, Sampled, 0);

Thing_implement (ConstantQLogFSpectrogram, MultiSampledSpectrogram, 0);

void structMultiSampledSpectrogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Minimum frequency (Hz): ", exp2 (xmin), U" Hz");
	MelderInfo_writeLine (U"Maximum frequency (Hz): ", exp2 (xmax), U" Hz");
	MelderInfo_writeLine (U"First frequency (Hz): ", exp2 (x1), U" Hz");
	const integer numberOfFrequencies = frequencyBins.size;
	MelderInfo_writeLine (U"Number of frequencies: ", numberOfFrequencies);
	MelderInfo_writeLine (U"Number of frames in frequency bin 1: ", frequencyBins.at [1] -> nx);
	MelderInfo_writeLine (U"Number of frames in frequency bin ", numberOfFrequencies, U": ", frequencyBins.at [numberOfFrequencies] -> nx);
}

double structMultiSampledSpectrogram :: v_getValueAtSample (integer ifreq, integer iframe , int unit) {
	FrequencyBin bin = frequencyBins.at [ifreq];
	const double value = bin -> v_getValueAtSample (iframe, 1, unit);
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, iframe, unit) : undefined );
}

double structMultiSampledSpectrogram :: v_getFrequencyHz (double log2_f) {
	return exp2 (log2_f);
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
	if (fmin >= fmax) {
		fmin = my xmin;
		fmax = my xmax;
	} else {
		fmin = ( fmin > 0 ? log2 (fmin) : 0.0 );
		fmax = ( fmax > my v_getFrequencyHz (my xmax) ? my xmax : log2 (fmax) );
	}
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
		double f = my x1;
		while (f <= my xmax ) {
			if (f >= fmin) {
				const double f_hz = my v_getFrequencyHz (f);
				conststring32 f_string = Melder_fixed (f_hz, 1);
				Graphics_markLeft (g, f, false, true, false, f_string);
			}
			f += 1.0;
		}
		Graphics_textLeft (g, true, U"Frequency (log__2_Hz)");
	}
}

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double f1, integer numberOfStepsPerOctave, integer numberOfSteps) {
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
		
		autoConstantQLogFSpectrogram thee = ConstantQLogFSpectrogram_create (lowestFrequency, numberOfStepsPerOctave, numberOfSteps);
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 * samplingFrequency;
		Melder_require (thy v_getFrequencyHz (thy xmax) <= nyquistFrequency,
			U"The number of steps you want result in a maximum frequency which is above the Nyquist frequency of the sound. "
			"The maximum number of steps should not exceed ", Melder_iroundDown (numberOfStepsPerOctave * log2 (nyquistFrequency / lowestFrequency)), U".");
		if (timeOversamplingFactor < 1.0)
			timeOversamplingFactor = 4.0; // default oversampling
		const integer maximumNumberOfAnalysisSamples = samplingFrequency * q / lowestFrequency + 1;
		autoVEC window = raw_VEC (maximumNumberOfAnalysisSamples);
		autoMAT windowedExp = raw_MAT (2, maximumNumberOfAnalysisSamples);
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
				window [i] = NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * root);
			}
			window.resize (numberOfWindowSamples);
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				windowedCos [i] =   window [i] * cos (NUM2pi * q * (i - 1) / numberOfWindowSamples);
				windowedSin [i] = - window [i] * sin (NUM2pi * q * (i - 1) / numberOfWindowSamples);
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
				const double windowSq = NUMsum2 (window.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample));
				const double scaleFactor = 1.0 / (actualNumberOfSamples * sqrt (windowSq)) ;
				VEC amplitudePart = my z.row(1).part (leftSample, rightSample);
				frequencyBin -> z [1] [iframe] = NUMinner (amplitudePart, windowedCos.part (windowFromSoundOffset + leftSample,
					windowFromSoundOffset + rightSample)) * scaleFactor;
				frequencyBin -> z [2] [iframe] = NUMinner (amplitudePart, windowedSin.part (windowFromSoundOffset + leftSample, 
					windowFromSoundOffset + rightSample)) * scaleFactor;
			}
			thy frequencyBins.addItem_move (frequencyBin.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLogFSpectrogram.");
	}
}

/* End of file  MultiSampledSpectrogram.cpp */
