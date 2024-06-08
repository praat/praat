/* LPCToFormantAnalysisWorkspace.cpp
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

#include "LPCToFormantAnalysisWorkspace.h"

#include "oo_DESTROY.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "LPCToFormantAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCToFormantAnalysisWorkspace_def.h"

Thing_implement (LPCToFormantAnalysisWorkspace, SampledAnalysisWorkspace, 0);

void allocateFormantFrames (SampledAnalysisWorkspace me) {
	Melder_assert (my output != nullptr);
	Formant thee = reinterpret_cast<Formant> (my output);
	for (integer iframe = 1; iframe <= thy nx; iframe ++) {
		const Formant_Frame formantFrame = & thy frames [iframe];
		Formant_Frame_init (formantFrame, thy maxnFormants);
	}
}

void LPCToFormantAnalysisWorkspace_init (mutableLPCToFormantAnalysisWorkspace me, constLPC input, mutableFormant output) {
	SampledAnalysisWorkspace_init (me, input, output);
	my p = Polynomial_create (-1.0, 1.0, input -> maxnCoefficients);
	my roots = Roots_create (input -> maxnCoefficients);
	my allocateOutputFrames = allocateFormantFrames;
}

autoLPCToFormantAnalysisWorkspace LPCToFormantAnalysisWorkspace_create (constLPC input, mutableFormant output) {
	try {
		autoLPCToFormantAnalysisWorkspace me = Thing_new (LPCToFormantAnalysisWorkspace);
		LPCToFormantAnalysisWorkspace_init (me.get(), input, output);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LPCToFormantAnalysisWorkspace not created.");
	}
}

/* End of file LPCToFormantAnalysisWorkspace.cpp */

