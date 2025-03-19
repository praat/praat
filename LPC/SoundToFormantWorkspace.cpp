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

#define SoundToFormant_ANYWORKSPACE(name) \
Thing_implement (SoundToFormant##name##Workspace, SoundToFormantWorkspace, 0);\
void SoundToFormant##name##Workspace_init (SoundToFormant##name##Workspace me, double samplingPeriod, double effectiveAnalysisWidth,\
	kSound_windowShape windowShape, SoundToFormant_EXTRAS_TYPED integer numberOfPoles, double margin) \
{\
	SoundToFormantWorkspace_init (me, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles, margin);\
	SoundToLPC##name##Workspace ws = reinterpret_cast<SoundToLPC##name##Workspace> (my soundToLPC.get());\
	SoundToLPC##name##Workspace_init (ws, samplingPeriod, effectiveAnalysisWidth, windowShape, numberOfPoles SoundToFormant_EXTRAS);\
}\
\
autoSoundToFormant##name##Workspace SoundToFormant##name##Workspace_createSkeleton (constSound input, mutableFormant output) {\
	autoSoundToFormant##name##Workspace me = Thing_new (SoundToFormant##name##Workspace);\
	SoundToFormantWorkspace_initSkeleton (me.get(), input, output);\
	my soundToLPC = SoundToLPC##name##Workspace_createSkeleton (nullptr, nullptr);\
	my lpcToFormant = LPCToFormantWorkspace_createSkeleton (nullptr, output);\
	return me;\
}\
\
autoSoundToFormant##name##Workspace SoundToFormant##name##Workspace_create (constSound input, mutableFormant output,\
	double effectiveAnalysisWidth, kSound_windowShape windowShape,  SoundToFormant_EXTRAS_TYPED integer numberOfPoles, double margin)\
{\
	try {\
		Sampled_assertEqualDomains (input, output);\
		autoSoundToFormant##name##Workspace me = SoundToFormant##name##Workspace_createSkeleton (input, output);\
		SoundToFormant##name##Workspace_init (me.get(), input -> dx, effectiveAnalysisWidth, windowShape SoundToFormant_EXTRAS, numberOfPoles, margin);\
		return me;\
	} catch (MelderError) {\
		Melder_throw (U"SoundToFormant##name##Workspace not created.");\
	}\
}

#define SoundToFormant_EXTRAS_TYPED
#define SoundToFormant_EXTRAS

SoundToFormant_ANYWORKSPACE(Autocorrelation)
SoundToFormant_ANYWORKSPACE(Covariance)
SoundToFormant_ANYWORKSPACE(Burg)
#undef SoundToFormant_EXTRAS_TYPED
#undef SoundToFormant_EXTRAS

#define SoundToFormant_EXTRAS_TYPED double tol1, double tol2,
#define SoundToFormant_EXTRAS ,tol1, tol2
SoundToFormant_ANYWORKSPACE(Marple)
#undef SoundToFormant_EXTRAS_TYPED
#undef SoundToFormant_EXTRAS

#define SoundToFormant_EXTRAS_TYPED double k_stdev, integer itermax, double tol, double location, bool wantlocation,
#define SoundToFormant_EXTRAS ,k_stdev, itermax, tol, location, wantlocation
SoundToFormant_ANYWORKSPACE(Robust)
#undef SoundToFormant_EXTRAS_TYPED
#undef SoundToFormant_EXTRAS

#undef SoundToFormant_ANYWORKSPACE

/* End of file SoundToFormantWorkspace.cpp */
