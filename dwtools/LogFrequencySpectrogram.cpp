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


autoLogFrequencySpectrogram LogFrequencySpectrogram_create (double xmin, double xmax, integer nx, double dx, double x1, double fmin_hz, double fmax_hz, integer numberOfStepsPerOctave, double f1_hz) {
	try {
		autoLogFrequencySpectrogram me = Thing_new (LogFrequencySpectrogram);
		const double ymax = log2 (fmax_hz), ymin = log2 (fmin_hz), y1 = log2 (f1_hz);
		const integer ny = Melder_iround (numberOfStepsPerOctave * log2 (fmax_hz / fmin_hz));
		const double dy = pow (2.0, 1.0 / numberOfStepsPerOctave) - 1.0;
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create LogFrequencySpectrogram.");
	}
}

autoLogFrequencySpectrogram Sound_to_LogFrequencySpectrogram_constantQ (Sound me, double timeStep, double fmin_hz, double fmax_hz, integer numberOfStepsPerOctave, double f1_hz) {
	try {
		const integer nx = (my xmax - my xmin) / timeStep;
		Melder_require (nx > 0,
			U"Either your Sound is too short or your time step is too large.");
		autoLogFrequencySpectrogram thee = LogFrequencySpectrogram_create (my xmin, my xmax, nx, timeStep, timeStep, fmin_hz, fmax_hz,  numberOfStepsPerOctave, f1_hz);
		const double baseQ = 1.0 / (pow (2.0, 1.0 / 24.0) - 1.0);
		const double samplingFrequency = 1.0 / my dx, alpha = 25.0/46.0, oneMinusAlpha = 1.0 - alpha;
		const integer Nkmax = samplingFrequency * baseQ / thy ymin;
		autoVEC windowedSin = raw_VEC (Nkmax), windowedCos = raw_VEC (Nkmax);
		for (integer ifreq = 1; ifreq <= thy ny; ifreq ++) {
			const double f = thy ymin * pow (2.0, ifreq - 1.0);
			const double q = f < 1569.0 ? baseQ : 2.0 * baseQ;
			const integer numberOfWindowSamples = Melder_iroundDown (samplingFrequency * q / f);
			const integer numberOfHalfWindowSamples = numberOfWindowSamples / 2;
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				const double window = alpha + oneMinusAlpha * cos (NUM2pi *i / numberOfWindowSamples);
				windowedSin [i] = - window * sin (NUM2pi * f * i);
				windowedCos [i] =   window * cos (NUM2pi * f * i);
			}
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				const double time = Sampled_indexToX (thee.get(), iframe);
				const integer midSample = Sampled_xToLowIndex (me, time);
				const integer lowIndexSound = midSample - numberOfHalfWindowSamples;
				const integer highIndexSound = lowIndexSound + numberOfWindowSamples - ( numberOfWindowSamples % 2 == 0 ? 0 : 1 );
				const integer startSample = std::max (1_integer, lowIndexSound);
				const integer endSample = std::min (highIndexSound, my nx);
				const integer lowIndexWindow = startSample - lowIndexSound;
				const integer highIndexWindow = numberOfWindowSamples - (endSample - highIndexSound);
				Melder_assert (highIndexWindow - lowIndexWindow == endSample - startSample);
				const double realPart = NUMinner (my z.row(1).part (startSample, endSample), windowedCos.part (lowIndexWindow, highIndexWindow));
				const double imagPart = NUMinner (my z.row(1).part (startSample, endSample), windowedSin.part (lowIndexWindow, highIndexWindow));
				thy z [1] [iframe] = realPart * realPart + imagPart * imagPart;
			}
		}
		return thee;
		
	} catch (MelderError) {
		Melder_throw (me, U": cannot create LogFrequencySpectrogram from Sound.");
	}
	
}

/* End of file LogFrequencySpectrogram.cpp */
