#ifndef _LPCToSampledWorkspace_h_
#define _LPCToSampledWorkspace_h_
/* LPCToSampledWorkspace.h
 *
 * Copyright (C) 2024 David Weenink
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

#include "LPC.h"
#include "SampledToSampledWorkspace.h"
#include "LPCToSampledWorkspace_def.h"

void LPCToSampledWorkspace_init (mutableLPCToSampledWorkspace me, double samplingPeriod, integer maxnCoefficients);

void LPCToSampledWorkspace_initSkeleton (mutableLPCToSampledWorkspace me, constLPC input, mutableSampled output);

#endif /* _LPCToSampledWorkspace_h_ */
