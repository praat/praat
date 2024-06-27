/* SoundToFormantWorkspace.cpp
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

#include "SoundToFormantWorkspace.h"

#include "oo_DESTROY.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundToFormantWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundToFormantWorkspace_def.h"


Thing_implement (SoundToFormantWorkspace, SoundToSampledWorkspace, 0);

void structSoundToFormantWorkspace :: getInputFrame (void) {
	/*
		Get the sound frame and pass its reference to the other objects
		and make sure they all refer to the same frame.
	*/
	SoundToFormantWorkspace_Parent :: getInputFrame ();
	soundToLPC -> currentFrame = currentFrame;
	soundToLPC -> soundFrameVEC = soundFrameVEC;
	lpcToFormant -> currentFrame = currentFrame;
}

void structSoundToFormantWorkspace :: allocateOutputFrames () {
	lpcToFormant -> allocateOutputFrames ();
}

void structSoundToFormantWorkspace :: saveOutputFrame () {
	lpcToFormant -> saveOutputFrame ();
}

bool structSoundToFormantWorkspace :: inputFrameToOutputFrame () {
	bool step1 = soundToLPC -> inputFrameToOutputFrame ();
	lpcToFormant -> lpcFrameRef = soundToLPC -> outputLPCFrameRef;
	bool step2 = lpcToFormant -> inputFrameToOutputFrame ();
	return step1 && step2;
}

void SoundToFormantWorkspace_initSoundDependency (SoundToFormantWorkspace me, double samplingPeriod) {
	my lpcToFormant -> samplingPeriod = samplingPeriod;
	if (my soundToLPC -> outputObjectPresent)
		Melder_assert (my soundToLPC -> samplingPeriod == samplingPeriod);
}
	
void SoundToFormantWorkspace_initFormantDependency (SoundToFormantWorkspace me, integer maxnFormants, double margin)
{
	Formant output = reinterpret_cast<Formant> (my output);
	my lpcToFormant = LPCToFormantWorkspace_create (nullptr, nullptr, margin);
	my lpcToFormant -> maxnFormants = maxnFormants;
}

void SoundToFormantWorkspace_initLPCDependency (SoundToFormantWorkspace me, integer maxnCoefficients, double samplingPeriod, double margin) {
	SoundToLPCWorkspace_initLPCDependency (my soundToLPC.get(), maxnCoefficients, samplingPeriod);
	Formant output = reinterpret_cast<Formant> (my output);
	
	my lpcToFormant = LPCToFormantWorkspace_create (nullptr, nullptr, margin);
	if (! my outputObjectPresent)
		my lpcToFormant -> samplingPeriod = samplingPeriod;
}

Thing_implement (SoundToFormantWorkspace_burg, SoundToFormantWorkspace, 0);

autoSoundToFormantWorkspace_burg SoundToFormantWorkspace_burg_create (
	constSound input, mutableFormant output, integer numberOfFormants, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double margin)
{
	try {
		Melder_assert (input);
		if (output)
			Sampled_assertEqualDomains (input, output);
		autoSoundToFormantWorkspace_burg me = Thing_new (SoundToFormantWorkspace_burg);
		SoundToSampledWorkspace_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my soundToLPC = SoundToLPCWorkspace_burg_create (nullptr, nullptr,
			effectiveAnalysisWidth, windowShape);
		const integer maxnCoefficients = Melder_ifloor (2 * numberOfFormants);
		if (input && output)
			SoundToFormantWorkspace_initLPCDependency (me.get(), maxnCoefficients, input -> dx, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantWorkspace_burg not created.");
	}
}

Thing_implement (SoundToFormantWorkspace_robust, SoundToFormantWorkspace, 0);

void SoundToFormantWorkspace_robust_initFormantDependency (mutableSoundToFormantWorkspace, integer maxnFormants) {
	
}

autoSoundToFormantWorkspace_robust SoundToFormantWorkspace_robust_create (
	constSound input, mutableFormant output, double effectiveAnalysisWidth, kSound_windowShape windowShape, double margin, double k_stdev, integer itermax, double tol)
{
	try {
		Melder_assert (input); // to get the samplingPeriod;
		autoSoundToFormantWorkspace_robust me = Thing_new (SoundToFormantWorkspace_robust);
		SoundToSampledWorkspace_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		my soundToLPC = SoundToLPCWorkspace_robust_create (nullptr, nullptr,
			effectiveAnalysisWidth, windowShape, k_stdev, itermax, tol, 0.0, true
		);
		
		if (input && output) {
			const integer maxnCoefficients = Melder_ifloor (2 * output -> maxnFormants);
			SoundToFormantWorkspace_initLPCDependency (me.get(), maxnCoefficients, input -> dx, margin);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantWorkspace_robust not created.");
	}
}

/* End of file SoundToFormantWorkspace.cpp */
