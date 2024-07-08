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

void structSoundToFormantWorkspace :: allocateOutputFrames () {
	lpcToFormant -> allocateOutputFrames ();
}

void structSoundToFormantWorkspace :: getInputFrame (void) {
	SoundToFormantWorkspace_Parent :: getInputFrame ();
}

bool structSoundToFormantWorkspace :: inputFrameToOutputFrame () {
	soundToLPC -> currentFrame = currentFrame;
	soundToLPC -> soundFrameVEC = soundFrameVEC;
	bool step1 = soundToLPC -> inputFrameToOutputFrame ();
	lpcToFormant -> currentFrame = currentFrame;
	lpcToFormant -> lpcFrameRef = soundToLPC -> outputLPCFrameRef;
	bool step2 = lpcToFormant -> inputFrameToOutputFrame ();
	return step1 && step2;
}

void structSoundToFormantWorkspace :: saveOutputFrame () {
	lpcToFormant -> saveOutputFrame ();
}

void SoundToFormantWorkspace_initSkeleton (SoundToFormantWorkspace me, constSound input, mutableFormant output) {
	SoundToSampledWorkspace_initSkeleton (me, input, output);
}

void SoundToFormantWorkspace_init (SoundToFormantWorkspace me, double samplingPeriod, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, integer numberOfPoles, double margin)
{
	SoundToSampledWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape);
	LPCToFormantWorkspace_init (my lpcToFormant.get(), samplingPeriod, numberOfPoles, margin);
}

Thing_implement (SoundToFormantBurgWorkspace, SoundToFormantWorkspace, 0);

void SoundToFormantBurgWorkspace_init (SoundToFormantBurgWorkspace me, double samplingPeriod, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, integer numberOfPoles, double margin)
{
	SoundToFormantWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles, margin);
	SoundToLPCBurgWorkspace stlb = reinterpret_cast<SoundToLPCBurgWorkspace> (my soundToLPC.get());
	SoundToLPCBurgWorkspace_init (stlb, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles);
}

autoSoundToFormantBurgWorkspace SoundToFormantBurgWorkspace_createSkeleton (constSound input, mutableFormant output) {
	autoSoundToFormantBurgWorkspace me = Thing_new (SoundToFormantBurgWorkspace);
	SoundToFormantWorkspace_initSkeleton (me.get(), input, output);
	my soundToLPC = SoundToLPCBurgWorkspace_createSkeleton (nullptr, nullptr);
	my lpcToFormant = LPCToFormantWorkspace_createSkeleton (nullptr, output);
	return me;
}

autoSoundToFormantBurgWorkspace SoundToFormantBurgWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, integer numberOfPoles, double margin)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToFormantBurgWorkspace me = SoundToFormantBurgWorkspace_createSkeleton (input, output);
		SoundToFormantBurgWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape, numberOfPoles, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantBurgWorkspace not created.");
	}
}

Thing_implement (SoundToFormantRobustWorkspace, SoundToFormantWorkspace, 0);

void SoundToFormantRobustWorkspace_init (SoundToFormantRobustWorkspace me, double samplingPeriod,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax,
	double tol, double location, bool wantlocation, integer numberOfPoles, double margin)
{
	SoundToLPCRobustWorkspace stlr = reinterpret_cast<SoundToLPCRobustWorkspace> (my soundToLPC.get());
	SoundToLPCRobustWorkspace_init (stlr, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles,
		k_stdev, itermax, tol, location, wantlocation);
	LPCToFormantWorkspace_init (my lpcToFormant.get(), samplingPeriod, numberOfPoles, margin);
	SoundToFormantWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles, margin);
}

autoSoundToFormantRobustWorkspace SoundToFormantRobustWorkspace_createSkeleton (constSound input, mutableFormant output)
{
	autoSoundToFormantRobustWorkspace me = Thing_new (SoundToFormantRobustWorkspace);
	SoundToFormantWorkspace_initSkeleton (me.get(), input, output);
	my soundToLPC = SoundToLPCRobustWorkspace_createSkeleton (nullptr, nullptr);
	my lpcToFormant = LPCToFormantWorkspace_createSkeleton (nullptr, output);
	return me;
}

autoSoundToFormantRobustWorkspace SoundToFormantRobustWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax,
	double tol, double location, bool wantlocation, integer numberOfPoles, double margin)
{
	try {
		Sampled_assertEqualDomains (input, output);
		autoSoundToFormantRobustWorkspace me = SoundToFormantRobustWorkspace_createSkeleton (input, output);
		SoundToFormantRobustWorkspace_init (me.get(), input -> dx, effectiveAnalysisWidth,
			windowShape, k_stdev, itermax, tol, location, wantlocation, numberOfPoles, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantRobustWorkspace not created.");
	}
}

/* End of file SoundToFormantWorkspace.cpp */
