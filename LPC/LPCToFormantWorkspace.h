#ifndef _LPCToFormantWorkspace_h_
#define _LPCToFormantWorkspace_h_
/* LPCToFormantWorkspace.h
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

#include "Formant.h"
#include "LPC.h"
#include "Polynomial.h"
#include "Roots.h"
#include "LPCToSampledWorkspace.h"
#include "LPCToFormantWorkspace_def.h"

void LPCToFormantWorkspace_init (LPCToFormantWorkspace me, double samplingPeriod, integer maxnCoefficients, double margin);

autoLPCToFormantWorkspace LPCToFormantWorkspace_createSkeleton (constLPC input, mutableFormant output);

autoLPCToFormantWorkspace LPCToFormantWorkspace_create (constLPC input, mutableFormant output, double margin);

inline integer numberOfFormantsFromNumberOfCoefficients (integer maxnCoefficients, double margin) {
	return ( margin == 0.0 ? maxnCoefficients : (maxnCoefficients + 1) / 2 );
}

#endif /*_LPCToFormantWorkspace_h_ */
