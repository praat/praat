/* LogFrequencySpectrogram.cpp
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


#include "LogFrequencySpectrogram.h"
#include "NUM2.h"

Thing_implement (LogFrequencySpectrogram, Matrix, 2);

void structLogFrequencySpectrogram :: v_info () {
	structMatrix :: v_info ();
}

double log2yToHz (double log2y) {
	return pow (2.0, log2y);
}
double hzToY (double f_hz) {
	return log2 (f_hz);
}

autoLogFrequencySpectrogram LogFrequencySpectrogram_create (double tmin, double tmax, integer nx, double dx, double x1, double f1, integer numberOfStepsPerOctave, integer numberOfSteps) {
	try {
		autoLogFrequencySpectrogram me = Thing_new (LogFrequencySpectrogram);
		const double fmax_hz = f1 * pow (2.0, numberOfSteps / numberOfStepsPerOctave);
		const double log2_fmax = log2 (fmax_hz), log2_f1 = log2 (f1);
		const double dy = (log2_fmax - log2_f1) / (numberOfSteps - 1);
		const double ymin = log2_f1 - 0.5 * dy, ymax = log2_fmax + 0.5 * dy;
		Matrix_init (me.get(), tmin, tmax, nx, dx, x1, ymin, ymax, numberOfSteps, dy, log2_f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create LogFrequencySpectrogram.");
	}
}

void LogFrequencySpectrogram_paint (LogFrequencySpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double minimum, double maximum, bool garnish) {
	if (fmin >= fmax) {
	}
	autoLogFrequencySpectrogram thee = Data_copy (me);
	for (integer irow = 1; irow <= thy z.nrow; irow ++)
		for (integer icol =1; icol <= thy z.ncol; icol ++)
			thy z [irow] [icol] = 10 * log (my z [irow] [icol] / 4e-10);
	Matrix_paintCells (thee.get(), g, tmin, tmax, fmax, fmin, minimum, maximum);
	if (garnish) {
		
	}
}

autoLogFrequencySpectrogram Sound_to_LogFrequencySpectrogram_constantQ (Sound me, double timeStep, double f1_hz, integer numberOfStepsPerOctave, integer numberOfSteps) {
	try {
		if (timeStep <= 0.0)
			timeStep = 0.01;
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 * samplingFrequency;
		Melder_require (f1_hz * pow (2.0, numberOfSteps / numberOfStepsPerOctave) <= nyquistFrequency,
			U"The number of steps you want result in a maximum frequency which is above the Nyquist frequency of the sound. "
			"The maximum number of steps should not exceed ", Melder_iroundDown (numberOfStepsPerOctave * log2 (nyquistFrequency / f1_hz)), U".");
		const integer nx = (my xmax - my xmin - 0.5 * timeStep) / timeStep;
		Melder_require (nx > 0,
			U"Either your Sound is too short or your time step is too large.");
		autoLogFrequencySpectrogram thee = LogFrequencySpectrogram_create (my xmin, my xmax, nx, timeStep, timeStep, f1_hz, numberOfStepsPerOctave, numberOfSteps);
		const double baseQ = 1.0 / (pow (2.0, 1.0 / numberOfStepsPerOctave) - 1.0);
		const double alpha = 25.0/46.0, oneMinusAlpha = 1.0 - alpha;
		const integer Nkmax = samplingFrequency * baseQ / f1_hz;
		autoVEC windowedSin = raw_VEC (Nkmax), windowedCos = raw_VEC (Nkmax);
		for (integer ifreq = 1; ifreq <= thy ny; ifreq ++) {
			const double log2y = SampledXY_indexToY (thee.get(), ifreq);
			const double f = pow (2.0, log2y);
			const double q = f < 1569.0 ? baseQ : 2.0 * baseQ;
			const integer numberOfWindowSamples = Melder_iroundDown (samplingFrequency * q / f);
			const integer numberOfHalfWindowSamples = numberOfWindowSamples / 2;
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				const double window = alpha + oneMinusAlpha * cos (NUM2pi * (i - 1) / numberOfWindowSamples);
				windowedSin [i] = - window * sin (NUM2pi * q * (i - 1) / numberOfWindowSamples);
				windowedCos [i] =   window * cos (NUM2pi * q * (i - 1) / numberOfWindowSamples);
			}
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				const double time = Sampled_indexToX (thee.get(), iframe);
				const integer midSample = Sampled_xToLowIndex (me, time);
				const integer lowIndexSound = midSample - numberOfHalfWindowSamples;
				const integer highIndexSound = lowIndexSound + numberOfWindowSamples - 1;
				const integer startSample = std::max (1_integer, lowIndexSound);
				const integer endSample = std::min (highIndexSound, my nx);
				const integer actualNumberOfSamples = endSample - startSample + 1;
				const integer lowIndexWindow = startSample - lowIndexSound + 1;
				const integer highIndexWindow = lowIndexWindow + actualNumberOfSamples - 1 ;
				Melder_assert (highIndexWindow - lowIndexWindow == endSample - startSample);
				const double realPart = NUMinner (my z.row(1).part (startSample, endSample), windowedCos.part (lowIndexWindow, highIndexWindow));
				const double imagPart = NUMinner (my z.row(1).part (startSample, endSample), windowedSin.part (lowIndexWindow, highIndexWindow));
				thy z [ifreq] [iframe] = (realPart * realPart + imagPart * imagPart) / (actualNumberOfSamples * actualNumberOfSamples);
			}
		}
		return thee;
		
	} catch (MelderError) {
		Melder_throw (me, U": cannot create LogFrequencySpectrogram from Sound.");
	}
	
}

/* End of file LogFrequencySpectrogram.cpp */
