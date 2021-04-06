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
#include "Sound.h"
#include "melder.h"
#include "MultiSampledSpectrogram_def.h"

Thing_define (ConstantQLogFSpectrogram, MultiSampledSpectrogram) {
};

void FrequencyBin_init (FrequencyBin me, double xmin, double xmax, integer nx, double dx, double x1);

void MultiSampledSpectrogram_init (MultiSampledSpectrogram me, double fmin, double fmax, integer numberOfFrequencies, double df, double f1);

void MultiSampledSpectrogram_draw (MultiSampledSpectrogram me, double tmin, double tmax, double fmin, double fmax, bool garnish);

void MultiSampledSpectrogram_paint (MultiSampledSpectrogram me, double tmin, double tmax, double fmin, double fmax, bool garnish);

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double f1, integer numberOfStepsPerOctave, integer numberOfSteps);

autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram (Sound me, double f1, double q, integer numberOfStepsPerOctave, integer numberOfSteps, double timeOversamplingFactor);

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish);

#endif /* MultiSampledSpectrogram_h_ */
