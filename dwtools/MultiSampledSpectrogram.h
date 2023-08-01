#ifndef _MultiSampledSpectrogram_h_
#define _MultiSampledSpectrogram_h_
/* MultiSampledSpectrogram.h
 * 
 * Copyright (C) 2021-2023 David Weenink
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

#include "AnalyticSound.h"
#include "Graphics.h"


Thing_define (FrequencyBin, AnalyticSound) {
	double v_getValueAtSample (integer isample, integer which, int unit) const
		override;
};

#include "MultiSampledSpectrogram_def.h"

/*
	A MultiSampledSpectrogram is a spectrogram that is evenly sampled in the frequency domain (on some possibly non-linear frequency scale).
	Each "sample" is a FrequencyBin with real and imaginary values.
	The original sound can be reconstructed from the data in the frequency bins.
*/

autoFrequencyBin FrequencyBin_create (double xmin, double xmax, integer nx, double dx, double x1);

void FrequencyBin_formula (FrequencyBin me, conststring32 formula, Interpreter interpreter);

double FrequencyBin_getValueAtX (FrequencyBin me, double x, kVector_valueInterpolation valueInterpolationType);

autoSound FrequencyBin_to_Sound (FrequencyBin me);

autoAnalyticSound FrequencyBin_to_AnalyticSound (FrequencyBin me);

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double tmin, double tmax, double fmin, double fmax, 
	integer numberOfFrequencies, double df, double f1, double frequencyResolutionInBins);

void MultiSampledSpectrogram_formula (MultiSampledSpectrogram me, conststring32 formula, Interpreter interpreter);

void MultiSampledSpectrogram_formula_part (MultiSampledSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter);

void MultiSampledSpectrogram_paintInside (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin_hz, double fmax_hz, double dBRange);

integer MultiSampledSpectrogram_getNumberOfFrames (MultiSampledSpectrogram me);

void MultiSampledSpectrogram_getFrequencyBand (MultiSampledSpectrogram me, integer index, double *out_flow, double *out_fhigh);

#endif /* MultiSampledSpectrogram_h_ */
