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

void LPCToSampledWorkspace_init (mutableLPCToSampledWorkspace me, constLPC input, mutableSampled output);

/*
	If output not present
*/
inline void LPCToSampledWorkspace_initLPCDependency (mutableLPCToSampledWorkspace me, integer maxnCoefficients, double samplingPeriod) {
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = maxnCoefficients;
	LPC_Frame_init (& my lpcFrame, my maxnCoefficients);
}

#endif /* _LPCToSampledWorkspace_h_ */
