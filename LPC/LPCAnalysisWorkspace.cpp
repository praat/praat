/* LPCAnalysisWorkspace.cpp
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

#include "LPCAnalysisWorkspace.h"

#include "oo_DESTROY.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "LPCAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCAnalysisWorkspace_def.h"


Thing_implement (LPCAnalysisWorkspace, SampledAnalysisWorkspace, 0);

void LPCAnalysisWorkspace_init (mutableLPCAnalysisWorkspace me, constLPC input, mutableSampled output, integer numberOfCoefficients) {
	SampledAnalysisWorkspace_init (me, input, output);
	if (! my inputObjectPresent)
		return;
	Melder_assert (input -> maxnCoefficients == numberOfCoefficients);
	LPC_Frame_init (& my lpcFrame, numberOfCoefficients);
}

/* End of file LPCAnalysisWorkspace.cpp */
