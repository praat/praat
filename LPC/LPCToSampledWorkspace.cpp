/* LPCToSampledWorkspace.cpp
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

#include "LPCToSampledWorkspace.h"

#include "oo_DESTROY.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_COPY.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_EQUAL.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "LPCToSampledWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCToSampledWorkspace_def.h"


Thing_implement (LPCToSampledWorkspace, SampledToSampledWorkspace, 0);

void LPCToSampledWorkspace_init (mutableLPCToSampledWorkspace me, double samplingPeriod, integer maxnCoefficients) {
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = maxnCoefficients;
	LPC_Frame_init (& my lpcFrame, my maxnCoefficients);
}

void LPCToSampledWorkspace_initSkeleton (mutableLPCToSampledWorkspace me, constLPC input, mutableSampled output) {
	SampledToSampledWorkspace_init (me, input, output);
}

/* End of file LPCToSampledWorkspace.cpp */
