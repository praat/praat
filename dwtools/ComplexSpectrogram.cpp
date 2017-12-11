/* ComplexSpectrogram.cpp
 * 
 * Copyright (C) 2014-2015 David Weenink
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
	double t1, double fmin, double fmax, integer nf, double df, double f1) {
	try {
		autoComplexSpectrogram me = Thing_new (ComplexSpectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		my phase = NUMmatrix <double> (1, my ny, 1, my nx);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ComplexSpectrogram not created.");
	}
}

autoComplexSpectrogram Sound_to_ComplexSpectrogram (Sound me, double windowLength, double timeStep) {
	try {
		double samplingFrequency = 1.0 / my dx, myDuration = my xmax - my xmin, t1;
		Melder_require (windowLength <= myDuration,
			U"Your sound is too short:\nit should be at least as long as one window length.");
		
		integer nsamp_window = Melder_ifloor (windowLength / my dx);
		integer halfnsamp_window = nsamp_window / 2 - 1;
		nsamp_window = halfnsamp_window * 2;
		
		Melder_require (nsamp_window > 1,
			U"There should be atleast two samples in the window.");
		
		integer numberOfFrames;
		Sampled_shortTermAnalysis (me, windowLength, timeStep, & numberOfFrames, & t1);

		// Compute sampling of the spectrum

		integer numberOfFrequencies = halfnsamp_window + 1;
		double df = samplingFrequency / (numberOfFrequencies - 1);
		
		autoComplexSpectrogram thee = ComplexSpectrogram_create (my xmin, my xmax, numberOfFrames, timeStep, t1, 0.0, 0.5 * samplingFrequency, numberOfFrequencies, df, 0.0);
		// 
		autoSound analysisWindow = Sound_create (1, 0.0, nsamp_window * my dx, nsamp_window, my dx, 0.5 * my dx);
		
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			double t = Sampled_indexToX (thee.get(), iframe);
			integer leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
			integer startSample = rightSample - halfnsamp_window;
			integer endSample = leftSample + halfnsamp_window;
			Melder_assert (startSample >= 1);
			Melder_assert (endSample <= my nx);
			
			for (integer j = 1; j <= nsamp_window; j ++) {
				analysisWindow -> z [1] [j] = my z [1] [startSample - 1 + j];
			}
			// window ?
			autoSpectrum spec = Sound_to_Spectrum (analysisWindow.get(), 0);
			
			thy z [1] [iframe] = spec -> z [1] [1] * spec -> z [1] [1];
			thy phase[1][iframe] = 0.0;
			for (integer ifreq = 2; ifreq <= numberOfFrequencies - 1; ifreq ++) {
				double x = spec -> z [1] [ifreq], y = spec -> z [2] [ifreq];
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
		/* original number of samples is odd: imaginary part of last spectral value is zero -> 
		 * phase is either zero or +/-pi
		 */
		double pi = atan2 (0.0, - 0.5);
		double samplingFrequency = 2.0 * my ymax;
		double lastFrequency = my y1 + (my ny - 1) * my dy, lastPhase = my phase[my ny][1];
		int originalNumberOfSamplesProbablyOdd = (lastPhase != 0.0 && lastPhase != pi && lastPhase != -pi) || 
			my ymax - lastFrequency > 0.25 * my dx;
		Melder_require (my y1 == 0.0, 
			U"A Fourier-transformable ComplexSpectrogram should have a first frequency of 0 Hz, not ", my y1, U" Hz.");
		
		integer nsamp_window = 2 * my ny - (originalNumberOfSamplesProbablyOdd ? 1 : 2 );
		integer halfnsamp_window = nsamp_window / 2;
		double synthesisWindowDuration = nsamp_window / samplingFrequency;
		autoSpectrum spectrum = Spectrum_create (my ymax, my ny);
		autoSound synthesisWindow = Sound_createSimple (1, synthesisWindowDuration, samplingFrequency);
		double newDuration = (my xmax - my xmin) * stretchFactor;
		autoSound thee = Sound_createSimple (1, newDuration, samplingFrequency); //TODO
		double thyStartTime;
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			// "original" sound :
			double tmid = Sampled_indexToX (me, iframe);
			integer leftSample = Sampled_xToLowIndex (thee.get(), tmid);
			integer rightSample = leftSample + 1;
			integer startSample = rightSample - halfnsamp_window;
			double startTime = Sampled_indexToX (thee.get(), startSample);
			if (iframe == 1) {
				thyStartTime = Sampled_indexToX (thee.get(), startSample);
			}
			//integer endSample = leftSample + halfnsamp_window;
			// New Sound with stretch
			integer thyStartSample = Sampled_xToLowIndex (thee.get(),thyStartTime);
			double thyEndTime = thyStartTime + my dx * stretchFactor;
			integer thyEndSample = Sampled_xToLowIndex (thee.get(), thyEndTime);
			integer stretchedStepSizeSamples = thyEndSample - thyStartSample + 1;
			//double extraTime = (thyStartSample - startSample + 1) * thy dx;
			double extraTime = (thyStartTime - startTime);
			spectrum -> z[1][1] = sqrt (my z[1][iframe]);
			for (integer ifreq = 2; ifreq <= my ny; ifreq ++) {
				double f = my y1 + (ifreq - 1) * my dy;
				double a = sqrt (my z [ifreq] [iframe]);
				double phi = my phase [ifreq] [iframe], intPart;
				double extraPhase = 2.0 * pi * modf (extraTime * f, & intPart); // fractional part
				phi += extraPhase;
				spectrum -> z [1] [ifreq] = a * cos (phi);
				spectrum -> z [2] [ifreq] = a * sin (phi);
			}

			autoSound synthesis = Spectrum_to_Sound (spectrum.get());

			// Where should the sound be placed?

			integer thyEndSampleP = Melder_ifloor (fmin (thyStartSample + synthesis -> nx - 1, thyStartSample + stretchedStepSizeSamples - 1)); // guard against extreme stretches
			if (iframe == my nx) {
				thyEndSampleP = Melder_ifloor (fmin (thy nx, thyStartSample + synthesis -> nx - 1));   // ppgb: waarom naar beneden afgerond?
			}
			for (integer j = thyStartSample; j <= thyEndSampleP; j++) {
				thy z [1] [j] = synthesis -> z [1] [j - thyStartSample + 1];
			}
			thyStartTime += my dx * stretchFactor;
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
			spectrum -> z[1][1] = sqrt (my z[1][iframe]);
			for (integer ifreq = 2; ifreq <= my ny; ifreq++) {
				double f = my y1 + (ifreq - 1) * my dy;
				double a = sqrt (my z[ifreq][iframe]);
				double phi = my phase[ifreq][iframe];
				double extraPhase = 2.0 * pi * (stretchFactor - 1.0) * my dx * f;
				phi += extraPhase;
				spectrum -> z[1][ifreq] = a * cos (phi);
				spectrum -> z[2][ifreq] = a * sin (phi);
			}
			autoSound synthesis = Spectrum_to_Sound (spectrum.get());
			for (integer j = istart; j <= iend; j++) {
				thy z[1][j] = synthesis -> z[1][j - istart + 1];
			}
			istart = iend + 1; iend = istart + stepSizeSamples - 1;
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
		NUMmatrix_copyElements<double> (my z, thy z, 1, my ny, 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrogram.");
	}
}

autoSpectrum ComplexSpectrogram_to_Spectrum (ComplexSpectrogram me, double time) {
	try {
		integer iframe = Sampled_xToLowIndex (me, time);   // ppgb: geen Sampled_xToIndex gebruiken voor integers (afrondingen altijd expliciet maken)
		iframe = iframe < 1 ? 1 : (iframe > my nx ? my nx : iframe);
		autoSpectrum thee = Spectrum_create (my ymax, my ny);
		for (integer ifreq = 1; ifreq <= my ny; ifreq ++) {
			double a = sqrt (my z [ifreq] [iframe]);
			double phi = my phase [ifreq] [iframe];
			thy z [1] [ifreq] = a * cos (phi);
			thy z [2] [ifreq] = a * sin (phi);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum created.");
	}
}
/* End of file ComplexSpectrogram.cpp */
