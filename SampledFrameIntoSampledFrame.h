#ifndef _SampledFrameIntoSampledFrame_h_
#define _SampledFrameIntoSampledFrame_h_
/* SampledFrameIntoSampledFrame.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "Sampled.h"
#include "SampledIntoSampledStatus.h"

#include "SampledFrameIntoSampledFrame_def.h"

void SampledFrameIntoSampledFrame_init (SampledFrameIntoSampledFrame me, mutableSampled output);

void SampledFrameIntoSampledFrame_initForStatusUpdates (SampledFrameIntoSampledFrame me, mutableSampledIntoSampledStatus status, bool updateStatus);

void SampledFrameIntoSampledFrame_initFrameInterval (SampledFrameIntoSampledFrame me, integer startFrame, integer endFrame);


#endif /* _SampledFrameIntoSampledFrame_h_ */

