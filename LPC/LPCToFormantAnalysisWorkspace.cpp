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
#include "Roots_and_Formant.h"

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

Thing_implement (LPCToFormantAnalysisWorkspace, LPCAnalysisWorkspace, 0);

static void Formant_Frame_init (Formant_Frame me, integer numberOfFormants) {
	if (numberOfFormants > 0)
		my formant = newvectorzero <structFormant_Formant> (numberOfFormants);
	my numberOfFormants = my formant.size; // maintain invariant
}

static void LPC_Frame_into_Polynomial (constLPC_Frame me, mutablePolynomial p) {
	Melder_assert (my nCoefficients  == my a.size); // check invariant

	p -> coefficients.resize (my nCoefficients + 1);
	for (integer icof = 1; icof <= my nCoefficients; icof ++)
		p -> coefficients [icof] = my a [my nCoefficients + 1 - icof];
	p -> coefficients [my nCoefficients + 1] = 1.0;
	p -> numberOfCoefficients = p -> coefficients.size; // maintain invariant
}

void structLPCToFormantAnalysisWorkspace :: allocateOutputFrames () {
	Melder_assert (output != nullptr);
	Formant thee = reinterpret_cast<Formant> (output);
	for (integer iframe = 1; iframe <= thy nx; iframe ++) {
		const Formant_Frame formantFrame = & thy frames [iframe];
		Formant_Frame_init (formantFrame, thy maxnFormants);
	}
}

void structLPCToFormantAnalysisWorkspace :: getInputFrame (integer iframe) {
	constLPC me = reinterpret_cast<constLPC> (input);
	my d_frames [iframe].copy (& lpcFrame);
	
}

bool structLPCToFormantAnalysisWorkspace :: inputFrameToOutputFrame () {
	constLPC lpc = reinterpret_cast<constLPC> (input);
	
	formantFrame. intensity = lpcFrame. gain;
	if (lpcFrame. nCoefficients == 0) {
		formantFrame. numberOfFormants = 0;
		formantFrame. formant.resize (formantFrame. numberOfFormants); // maintain invariant
		frameAnalysisInfo = 1;	
		return true;
	}
	frameAnalysisInfo = 0;
	const double samplingFrequency = 1.0 / lpc -> samplingPeriod;
	LPC_Frame_into_Polynomial (& lpcFrame, p.get());
	Polynomial_into_Roots (p.get(), roots.get(), workvectorPool.get());
	Roots_fixIntoUnitCircle (roots.get());
	Roots_into_Formant_Frame (roots.get(), & formantFrame, samplingFrequency, margin);
	return true;
}

void structLPCToFormantAnalysisWorkspace :: saveOutputFrame () {
	mutableFormant me = reinterpret_cast<mutableFormant> (output);
	formantFrame.copy (& my frames [currentFrame]);
}

autoLPCToFormantAnalysisWorkspace LPCToFormantAnalysisWorkspace_create (constLPC input, mutableFormant output, double margin) {
	try {
		Sampled_assertEqualDomainsAndSampling (input, output);
		autoLPCToFormantAnalysisWorkspace me = Thing_new (LPCToFormantAnalysisWorkspace);
		const integer maxnCoefficients = input -> maxnCoefficients;
		LPCAnalysisWorkspace_init (me.get(), input, output, maxnCoefficients);
		Formant_Frame_init (& my formantFrame, output -> maxnFormants);
		autoINTVEC sizes {maxnCoefficients * maxnCoefficients, maxnCoefficients, maxnCoefficients, 11 * maxnCoefficients};
		my workvectorPool = WorkvectorPool_create (sizes.get(), true);		
		my p = Polynomial_create (-1.0, 1.0, maxnCoefficients);
		my roots = Roots_create (maxnCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LPCToFormantAnalysisWorkspace not created.");
	}
}

/* End of file LPCToFormantAnalysisWorkspace.cpp */

