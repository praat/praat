#ifndef _ConstantQLogFSpectrogram_h_
#define _ConstantQLogFSpectrogram_h_
/* ConstantQLogFSpectrogram.h
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

#include "Graphics.h"
#include "Matrix.h"
#include "MultiSampledSpectrogram.h"
#include "melder.h"

Thing_define (ConstantQLogFSpectrogram, MultiSampledSpectrogram) {
	void v_info ()
		override;
	double v_getValueAtSample (integer ifreq, integer iframe, int unit)
		override;
	double v_myFrequencyUnitToHertz (double log2_f)
		override;
	double v_hertzToMyFrequencyUnit (double f_hz)
		override;	
};

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double tmin, double tmax, double f1, double fmax, integer numberOfBinsPerOctave, double frequencyResolutionInBins);

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, double dBRange, bool garnish);

double ConstantQLogFSpectrogram_getQualityFactor (ConstantQLogFSpectrogram me);

void ConstantQLogFSpectrogram_formula (ConstantQLogFSpectrogram me, conststring32 formula, Interpreter interpreter);

void ConstantQLogFSpectrogram_formula_part (ConstantQLogFSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter);

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_translateSpectrum (ConstantQLogFSpectrogram me, double fromTime, double toTime, double fromFrequency, double shiftNumberOfBins);

#endif /* MultiSampledSpectrogram_h_ */
