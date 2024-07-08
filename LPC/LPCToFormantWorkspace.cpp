/* LPCToFormantWorkspace.cpp
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

#include "LPCToFormantWorkspace.h"
#include "Roots_and_Formant.h"

#include "oo_DESTROY.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_COPY.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_EQUAL.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "LPCToFormantWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCToFormantWorkspace_def.h"

Thing_implement (LPCToFormantWorkspace, LPCToSampledWorkspace, 0);

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

void structLPCToFormantWorkspace :: allocateOutputFrames () {
	if (! outputObjectPresent)
		return;
	Formant thee = reinterpret_cast<Formant> (output);
	for (integer iframe = 1; iframe <= thy nx; iframe ++) {
		const Formant_Frame formantFrame = & thy frames [iframe];
		Formant_Frame_init (formantFrame, thy maxnFormants);
	}
}

void structLPCToFormantWorkspace :: getInputFrame () {
	if (! inputObjectPresent)
		return;
	constLPC me = reinterpret_cast<constLPC> (input);
	my d_frames [currentFrame].copy (& lpcFrame);
}

bool structLPCToFormantWorkspace :: inputFrameToOutputFrame () {
	formantFrameRef -> intensity = lpcFrameRef -> gain;
	if (lpcFrameRef -> nCoefficients == 0) {
		formantFrameRef -> numberOfFormants = 0;
		formantFrameRef -> formant.resize (formantFrameRef -> numberOfFormants); // maintain invariant
		frameAnalysisInfo = 1;	
		return true;
	}
	frameAnalysisInfo = 0;
	const double samplingFrequency = 1.0 / samplingPeriod;
	LPC_Frame_into_Polynomial (lpcFrameRef, p.get());
	Polynomial_into_Roots (p.get(), roots.get(), workvectorPool.get());
	Roots_fixIntoUnitCircle (roots.get());
	Roots_into_Formant_Frame (roots.get(), formantFrameRef, samplingFrequency, margin);
	return true;
}

void structLPCToFormantWorkspace :: saveOutputFrame () {
	if (! outputObjectPresent)
		return;
	mutableFormant me = reinterpret_cast<mutableFormant> (output);
	formantFrame.copy (& my frames [currentFrame]);
}

void LPCToFormantWorkspace_init (LPCToFormantWorkspace me, double samplingPeriod, integer maxnCoefficients, double margin) {
	LPCToSampledWorkspace_init (me, samplingPeriod, maxnCoefficients);
	my margin = margin;
	const integer maxnFormants = numberOfFormantsFromNumberOfCoefficients (maxnCoefficients, margin);
	if (my output) {
		Formant f = reinterpret_cast<Formant> (my output);
		Melder_assert (f -> maxnFormants == maxnFormants);
	}
	my maxnFormants = maxnFormants;
	Formant_Frame_init (& my formantFrame, maxnFormants);
	/*
		upperHessenberg (n*n), real part (n), imaginary part (n), dhseqr workspace (<= 11*n)
	*/
	autoINTVEC sizes {maxnCoefficients * maxnCoefficients, maxnCoefficients, maxnCoefficients, 11 * maxnCoefficients};
	my workvectorPool = WorkvectorPool_create (sizes.get(), true);		
	my p = Polynomial_create (-1.0, 1.0, maxnCoefficients);
	my roots = Roots_create (maxnCoefficients);
}

autoLPCToFormantWorkspace LPCToFormantWorkspace_createSkeleton (constLPC input, mutableFormant output) {
	autoLPCToFormantWorkspace me = Thing_new (LPCToFormantWorkspace);
	LPCToSampledWorkspace_initSkeleton (me.get(), input, output);
	return me;
}

autoLPCToFormantWorkspace LPCToFormantWorkspace_create (constLPC input, mutableFormant output, double margin) {
	try {
		Sampled_assertEqualDomainsAndSampling (input, output);
		autoLPCToFormantWorkspace me = LPCToFormantWorkspace_createSkeleton (input, output);
		LPCToFormantWorkspace_init (me.get(), input -> samplingPeriod, input -> maxnCoefficients, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LPCToFormantWorkspace not created.");
	}
}

/* End of file LPCToFormantWorkspace.cpp */

