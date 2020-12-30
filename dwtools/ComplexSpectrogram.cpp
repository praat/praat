/* ComplexSpectrogram.cpp
 *
 * Copyright (C) 2014-2019 David Weenink
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

#include "ComplexSpectrogram.h"
#include "Sound_and_Spectrum.h"

#include "oo_DESTROY.h"
#include "ComplexSpectrogram_def.h"
#include "oo_COPY.h"
#include "ComplexSpectrogram_def.h"
#include "oo_EQUAL.h"
#include "ComplexSpectrogram_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ComplexSpectrogram_def.h"
#include "oo_WRITE_TEXT.h"
#include "ComplexSpectrogram_def.h"
#include "oo_WRITE_BINARY.h"
#include "ComplexSpectrogram_def.h"
#include "oo_READ_TEXT.h"
#include "ComplexSpectrogram_def.h"
#include "oo_READ_BINARY.h"
#include "ComplexSpectrogram_def.h"
#include "oo_DESCRIPTION.h"
#include "ComplexSpectrogram_def.h"

Thing_implement (ComplexSpectrogram, Matrix, 2);


autoComplexSpectrogram ComplexSpectrogram_create (double tmin, double tmax, integer nt, double dt,
	double t1, double fmin, double fmax, integer nf, double df, double f1)
{
	try {
		autoComplexSpectrogram me = Thing_new (ComplexSpectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		my phase = zero_MAT (my ny, my nx);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ComplexSpectrogram not created.");
	}
}

autoVEC newVEChanning (integer size) {
	autoVEC window = raw_VEC (size);
	for (integer i = 1; i <= size; i ++)
		window [i] = 0.5 * (1.0 - cos (NUM2pi * i / size));
	return window;
}

autoComplexSpectrogram Sound_to_ComplexSpectrogram (Sound me, double windowLength, double maximumFrequency ) {
	try {
		Melder_require (my ny == 1,
			U"The sound should have onely one channel.");
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 / my dx;
		const double myDuration = my xmax - my xmin;
		Melder_require (windowLength <= myDuration,
			U"The sound is too short: it should be at least as long as one window length.");
		if (maximumFrequency == 0.0)
			maximumFrequency = nyquistFrequency;
		Melder_require (maximumFrequency <= nyquistFrequency,
			U"The maximum frequency should not exceed ", nyquistFrequency, U" Hz.");
		integer nsamp_window = Melder_ifloor (windowLength / my dx);
		autoVEC window = newVEChanning (nsamp_window);
		const integer halfnsamp_window = nsamp_window / 2 - 1;
		nsamp_window = halfnsamp_window * 2;
		
		Melder_require (nsamp_window > 1,
			U"There should be at least two samples in the window.");
		
		integer numberOfFrames;
		const double timeStep = 0.5 * windowLength;
		double t1;
		Sampled_shortTermAnalysis (me, windowLength, timeStep, & numberOfFrames, & t1);

		// Compute sampling of the spectrum

		const integer numberOfFrequencies = halfnsamp_window + 1;
		const double df = samplingFrequency / (numberOfFrequencies - 1);
		
		autoComplexSpectrogram thee = ComplexSpectrogram_create (my xmin, my xmax, numberOfFrames, timeStep, t1, 0.0, maximumFrequency, numberOfFrequencies, df, 0.0);

		autoSound analysisWindow = Sound_create (1, 0.0, nsamp_window * my dx, nsamp_window, my dx, 0.5 * my dx);
		
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double tmid = Sampled_indexToX (thee.get(), iframe);
			const integer leftSample = Sampled_xToLowIndex (me, tmid);
			double rightSample = leftSample + 1;
			const integer startSample = rightSample - halfnsamp_window;
			const integer endSample = startSample + nsamp_window - 1;
			Melder_assert (startSample >= 1);
			Melder_assert (endSample <= my nx);
			
			analysisWindow -> z.row (1) <<= my z.row (1).part (startSample, endSample);
			
			analysisWindow -> z.row (1)  *=  window.part (1, nsamp_window);
			autoSpectrum spec = Sound_to_Spectrum (analysisWindow.get(), false);
			
			thy z [1] [iframe] = spec -> z [1] [1] * spec -> z [1] [1];
			thy phase [1] [iframe] = 0.0;
			for (integer ifreq = 2; ifreq <= numberOfFrequencies - 1; ifreq ++) {
				const double x = spec -> z [1] [ifreq], y = spec -> z [2] [ifreq];
				thy z [ifreq] [iframe] = x * x + y * y; // power
				thy phase [ifreq] [iframe] = atan2 (y, x); // phase [-pi,+pi]
			}
			// even number of samples
			thy z [numberOfFrequencies] [iframe] = spec -> z [1] [numberOfFrequencies] * spec -> z [1][numberOfFrequencies];
			thy phase [numberOfFrequencies] [iframe] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ComplexSpectrogram created.");
	}
}

autoSound ComplexSpectrogram_to_Sound (ComplexSpectrogram me, double stretchFactor) {
	try {
		/*
			original number of samples is odd: imaginary part of last spectral value is zero ->
			phase is either zero or +/-pi
		 */
		const double samplingFrequency = 2.0 * my ymax;
		const double lastFrequency = my y1 + (my ny - 1) * my dy, lastPhase = my phase [my ny] [1];
		const bool originalNumberOfSamplesProbablyOdd = ( lastPhase != 0.0 && lastPhase != NUMpi && lastPhase != -NUMpi ||
				my ymax - lastFrequency > 0.25 * my dx );
		Melder_require (my y1 == 0.0,
			U"A Fourier-transformable ComplexSpectrogram should have a first frequency of 0 Hz, not ", my y1, U" Hz.");
		
		const integer nsamp_window = 2 * my ny - ( originalNumberOfSamplesProbablyOdd ? 1 : 2 );
		const integer halfnsamp_window = nsamp_window / 2;
		const double synthesisWindowDuration = nsamp_window / samplingFrequency;
		autoSpectrum spectrum = Spectrum_create (my ymax, my ny);
		autoSound synthesisWindow = Sound_createSimple (1, synthesisWindowDuration, samplingFrequency);
		const double newDuration = (my xmax - my xmin) * stretchFactor;
		autoSound thee = Sound_createSimple (1, newDuration, samplingFrequency); //TODO
		//double thyStartTime;
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			// "original" sound :
			const double tmid = Sampled_indexToX (me, iframe);
			const integer leftSample = Sampled_xToLowIndex (thee.get(), tmid);
			const integer rightSample = leftSample + 1;
			const integer startSample = rightSample - halfnsamp_window;
			const integer endSample = std::min (startSample + nsamp_window - 1, thy nx);
			
			//const double startTime = Sampled_indexToX (thee.get(), startSample);
			//if (iframe == 1)
			//	thyStartTime = Sampled_indexToX (thee.get(), startSample);
			//integer endSample = leftSample + halfnsamp_window;
			// New Sound with stretch
			//const integer thyStartSample = Sampled_xToLowIndex (thee.get(), thyStartTime);
			//const double thyEndTime = thyStartTime + my dx * stretchFactor;
			//const integer thyEndSample = Sampled_xToLowIndex (thee.get(), thyEndTime);
			//double extraTime = (thyStartSample - startSample + 1) * thy dx;
			//const double extraTime = thyStartTime - startTime;
			spectrum -> z [1] [1] = sqrt (my z [1] [iframe]);
			for (integer ifreq = 2; ifreq <= my ny; ifreq ++) {
				//const double f = my y1 + (ifreq - 1) * my dy;
				const double a = sqrt (my z [ifreq] [iframe]);
				//double dummy;
				const double phi = my phase [ifreq] [iframe]; // + extraPhase;
				spectrum -> z [1] [ifreq] = a * cos (phi);
				spectrum -> z [2] [ifreq] = a * sin (phi);
			}
			autoSound synthesis = Spectrum_to_Sound (spectrum.get());
			
			thy z.row (1).part (startSample, endSample)  +=  0.5 * synthesis -> z.row (1).part (1, nsamp_window);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

#if 0
static autoSound ComplexSpectrogram_to_Sound2 (ComplexSpectrogram me, double stretchFactor) {
	try {
		/* original number of samples is odd: imaginary part of last spectral value is zero -> 
		 * phase is either zero or pi
		 */
		double pi = atan2 (0.0, - 0.5);
		double samplingFrequency = 2.0 * my ymax;
		double lastFrequency = my y1 + (my ny - 1) * my dy;
		int originalNumberOfSamplesProbablyOdd = (my phase [my ny] [1] != 0.0 && my phase[my ny] [1] != pi) || my ymax - lastFrequency > 0.25 * my dx;
		if (my y1 != 0.0) {
			Melder_throw (U"A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my y1, U" Hz.");
		}
		integer numberOfSamples = 2 * my ny - (originalNumberOfSamplesProbablyOdd ? 1 : 2 );
		double synthesisWindowDuration = numberOfSamples / samplingFrequency;
		autoSpectrum spectrum = Spectrum_create (my ymax, my ny);
		autoSound synthesisWindow = Sound_createSimple (1, synthesisWindowDuration, samplingFrequency);
		integer stepSizeSamples = my dx * samplingFrequency * stretchFactor;
		double newDuration = (my xmax - my xmin) * stretchFactor + 0.05;
		autoSound thee = Sound_createSimple (1, newDuration, samplingFrequency); //TODO
		integer istart = 1, iend = istart + stepSizeSamples - 1;
		for (integer iframe = 1; iframe <= my nx; iframe++) {
			spectrum -> z [1] [1] = sqrt (my z [1] [iframe]);
			for (integer ifreq = 2; ifreq <= my ny; ifreq ++) {
				const double f = my y1 + (ifreq - 1) * my dy;
				const double a = sqrt (my z [ifreq] [iframe]);
				const double extraPhase = 2.0 * pi * (stretchFactor - 1.0) * my dx * f;
				const double phi = my phase [ifreq] [iframe] + extraPhase;
				spectrum -> z [1] [ifreq] = a * cos (phi);
				spectrum -> z [2] [ifreq] = a * sin (phi);
			}
			autoSound synthesis = Spectrum_to_Sound (spectrum.get());
			for (integer j = istart; j <= iend; j ++)
				thy z [1] [j] = synthesis -> z [1] [j - istart + 1];
			istart = iend + 1;
			iend = istart + stepSizeSamples - 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}
#endif

autoSpectrogram ComplexSpectrogram_to_Spectrogram (ComplexSpectrogram me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all() <<= my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrogram.");
	}
}

void ComplexSpectrogram_Spectrogram_replaceAmplitudes (ComplexSpectrogram me, Spectrogram thee) {
	Melder_require (my nx == thy nx && my ny == thy ny,
		U"The numbers of cells in the ComplexSpectrogram and Spectrogram should be equal.");
	my z.all() <<= thy z.all();
}

autoSpectrum ComplexSpectrogram_to_Spectrum (ComplexSpectrogram me, double time) {
	try {
		integer iframe = Sampled_xToLowIndex (me, time);
		if (iframe < 1)
			iframe = 1;
		if (iframe > my nx)
			iframe = my nx;
		autoSpectrum thee = Spectrum_create (my ymax, my ny);
		for (integer ifreq = 1; ifreq <= my ny; ifreq ++) {
			const double a = sqrt (my z [ifreq] [iframe]);
			const double phi = my phase [ifreq] [iframe];
			thy z [1] [ifreq] = a * cos (phi);
			thy z [2] [ifreq] = a * sin (phi);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum created.");
	}
}

/* End of file ComplexSpectrogram.cpp */
