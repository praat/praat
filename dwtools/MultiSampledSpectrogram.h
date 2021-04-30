#ifndef _MultiSampledSpectrogram_h_
#define _MultiSampledSpectrogram_h_
/* MultiSampledSpectrogram.h
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

#include "Function.h"
#include "Graphics.h"
#include "Sampled.h"
#include "Matrix.h"
#include "MultiSampledSpectrogram_def.h"
#include "Vector.h"

void FrequencyBin_init (FrequencyBin me, double xmin, double xmax, integer nx, double dx, double x1);
autoFrequencyBin FrequencyBin_create (double xmin, double xmax, integer nx, double dx, double x1);

void FrequencyBin_formula (FrequencyBin me, conststring32 formula, Interpreter interpreter);

double FrequencyBin_getValueAtX (FrequencyBin me, double x, kVector_valueInterpolation valueInterpolationType);

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double fmin, double fmax, integer numberOfFrequencies, double df, double f1);

void MultiSampledSpectrogram_draw (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish);

void MultiSampledSpectrogram_paint (MultiSampledSpectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish);

integer MultiSampledSpectrogram_getNumberOfFrames (MultiSampledSpectrogram me);

#endif /* MultiSampledSpectrogram_h_ */
