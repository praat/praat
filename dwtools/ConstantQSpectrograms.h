#ifndef _ConstantQSpectrograms_h_
#define _ConstantQSpectrograms_h_
/* ConstantQSpectrograms.h
 * 
 * Copyright (C) 2021-2022 David Weenink
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

#include "Graphics.h"
#include "Matrix.h"
#include "MultiSampledSpectrogram.h"
#include "melder.h"

Thing_define (ConstantQLog2FSpectrogram, MultiSampledSpectrogram) {
	void v1_info ()
		override;
	double v_getValueAtSample (integer ifreq, integer iframe, int unit) const
		override;
	double v_myFrequencyUnitToHertz (double log2_f) const
		override;
	double v_hertzToMyFrequencyUnit (double f_hz) const
		override;
};

autoConstantQLog2FSpectrogram ConstantQLog2FSpectrogram_create (double tmin, double tmax, double f1, double fmax, integer numberOfBinsPerOctave, double frequencyResolutionInBins);

void ConstantQLog2FSpectrogram_paint (ConstantQLog2FSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish);

double ConstantQLog2FSpectrogram_getQualityFactor (ConstantQLog2FSpectrogram me);

void ConstantQLog2FSpectrogram_formula (ConstantQLog2FSpectrogram me, conststring32 formula, Interpreter interpreter);

void ConstantQLog2FSpectrogram_formula_part (ConstantQLog2FSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter);

autoConstantQLog2FSpectrogram ConstantQLog2FSpectrogram_translateSpectrum (ConstantQLog2FSpectrogram me, double fromTime, double toTime, double fromFrequency, double shiftNumberOfBins);

Thing_define (GaborSpectrogram, MultiSampledSpectrogram) {
	void v1_info ()
		override;
	double v_myFrequencyUnitToHertz (double f) const
		override { return f; }
	double v_hertzToMyFrequencyUnit (double f) const
		override { return f; }
};

void GaborSpectrogram_paint (GaborSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish);

autoGaborSpectrogram GaborSpectrogram_create (double tmin, double tmax, double fmax, double frequencyResolution, double df);

#endif /* ConstantQSpectrograms_h_ */
