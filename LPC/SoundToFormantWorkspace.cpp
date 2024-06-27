/* SoundToFormantAnalysisWorkspace.cpp
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SoundToFormantAnalysisWorkspace.h"

#include "oo_DESTROY.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundToFormantAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundToFormantAnalysisWorkspace_def.h"


Thing_implement (SoundToFormantAnalysisWorkspace, SoundToSampledWorkspace, 0);

void structSoundToFormantAnalysisWorkspace :: getInputFrame (void) {
	/*
		Get the sound frame and pass its reference to the other objects
		and make sure they all refer to the same frame.
	*/
	SoundToFormantAnalysisWorkspace_Parent :: getInputFrame ();
	soundToLPC -> currentFrame = currentFrame;
	soundToLPC -> soundFrameVEC = soundFrameVEC;
	lpcToFormant -> currentFrame = currentFrame;
}

void structSoundToFormantAnalysisWorkspace :: allocateOutputFrames () {
	lpcToFormant -> allocateOutputFrames ();
}

void structSoundToFormantAnalysisWorkspace :: saveOutputFrame () {
	lpcToFormant -> saveOutputFrame ();
}

bool structSoundToFormantAnalysisWorkspace :: inputFrameToOutputFrame () {
	bool step1 = soundToLPC -> inputFrameToOutputFrame ();
	lpcToFormant -> lpcFrameRef = soundToLPC -> outputLPCFrameRef;
	bool step2 = lpcToFormant -> inputFrameToOutputFrame ();
	return step1 && step2;
}

void SoundToFormantAnalysisWorkspace_initFormantDependency (SoundToFormantAnalysisWorkspace me, integer maxnFormants) {
	my lpcToFormant -> maxnFormants = maxnFormants;
	my soundToLPC -> maxnCoefficients = 2 * maxnFormants;
}
void SoundToFormantAnalysisWorkspace_initSoundDependency (SoundToFormantAnalysisWorkspace me, double samplingPeriod);

Thing_implement (SoundToFormantAnalysisWorkspace_burg, SoundToFormantAnalysisWorkspace, 0);

autoSoundToFormantAnalysisWorkspace_burg SoundToFormantAnalysisWorkspace_burg_create (
	constSound input, mutableFormant output, integer numberOfFormants, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double margin)
{
	try {
		Melder_assert (input);
		if (output)
			Sampled_assertEqualDomains (input, output);
		autoSoundToFormantAnalysisWorkspace_burg me = Thing_new (SoundToFormantAnalysisWorkspace_burg);
		SoundToSampledWorkspace_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my soundToLPC = SoundToLPCWorkspace_burg_create (nullptr, nullptr,
			effectiveAnalysisWidth, windowShape);
		const integer maxnCoefficients = Melder_ifloor (2 * numberOfFormants);
		SoundToLPCWorkspace_initLPCDependency (my soundToLPC.get(), maxnCoefficients, input -> dx);
		my lpcToFormant = LPCToFormantWorkspace_create (nullptr, output, margin);
		if (output)
			LPCToFormantWorkspace_initFormantDependency (my lpcToFormant.get(), output -> maxnFormants);
		else {
			const integer maxnFormants = Melder_iround ((maxnCoefficients + 1) / 2);
			LPCToFormantWorkspace_initFormantDependency (my lpcToFormant.get(), maxnFormants);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantAnalysisWorkspace_burg not created.");
	}
}

Thing_implement (SoundToFormantAnalysisWorkspace_robust, SoundToFormantAnalysisWorkspace, 0);

autoSoundToFormantAnalysisWorkspace_robust SoundToFormantAnalysisWorkspace_robust_create (
	constSound input, mutableFormant output, integer numberOfFormants,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double margin, double k_stdev, integer itermax, double tol)
{
	try {
		Melder_assert (input); // to get the samplingPeriod;
		autoSoundToFormantAnalysisWorkspace_robust me = Thing_new (SoundToFormantAnalysisWorkspace_robust);
		const integer maxnCoefficients = Melder_ifloor (2 * numberOfFormants);
		SoundToSampledWorkspace_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my soundToLPC = SoundToLPCWorkspace_robust_create (nullptr, nullptr, effectiveAnalysisWidth, windowShape, k_stdev, itermax, tol, 0.0, true);
		
		my lpcToFormant = LPCToFormantWorkspace_create (nullptr, output, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantAnalysisWorkspace_robust not created.");
	}
}

/* End of file SoundToFormantAnalysisWorkspace.cpp */
