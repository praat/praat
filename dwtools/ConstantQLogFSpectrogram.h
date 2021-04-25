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

#include "ConstantQLogFSpectrogram_def.h"

autoConstantQLogFSpectrogram ConstantQLogFSpectrogram_create (double f1, double fmax, integer numberOfBinsPerOctave, double qualityFactor);

void ConstantQLogFSpectrogram_formula (ConstantQLogFSpectrogram me, conststring32 formula, Interpreter interpreter);

void ConstantQLogFSpectrogram_formula_part (ConstantQLogFSpectrogram me, double fromTime, double toTime, double fromFrequency, double toFrequency, conststring32 formula, Interpreter interpreter);

void ConstantQLogFSpectrogram_paint (ConstantQLogFSpectrogram me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dBRange, bool garnish);

#endif /* MultiSampledSpectrogram_h_ */
