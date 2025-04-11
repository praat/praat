#ifndef _SoundFrameIntoSampledFrame_h_
#define _SoundFrameIntoSampledFrame_h_
/* SoundFrameIntoSampledFrame.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "Sound.h"
#include "SampledFrameIntoSampledFrame.h"

#include "SoundFrameIntoSampledFrame_def.h"

void SoundFrameIntoSampledFrame_init (mutableSoundFrameIntoSampledFrame me, constSound input, mutableSampled output, double effectiveAnalysisWidth, kSound_windowShape windowShape);

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape);

integer getSoundFrameSize (double approximatePhysicalAnalysisWidth, double samplingPeriod);

#endif /* _SoundFrameIntoSampledFrame_h_ */
 
