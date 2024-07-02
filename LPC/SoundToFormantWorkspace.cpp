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

static void SoundToFormantWorkspace_initInputDependency (SoundToFormantWorkspace me, double samplingPeriod) {
	SoundToLPCWorkspace_initInputDependency (my soundToLPC.get(), samplingPeriod);
}

void SoundToFormantWorkspace_initPartialOutputDependency_sampling (SoundToFormantWorkspace me, double samplingPeriod) {
	my soundToLPC -> samplingPeriod = samplingPeriod;
}

void SoundToFormantWorkspace_initPartialOutputDependency_number (SoundToFormantWorkspace me, integer maxnFormants) {
	const integer maxnCoefficients = Melder_ifloor (2 * maxnFormants);
	my soundToLPC -> maxnCoefficients = maxnCoefficients;
}

Thing_implement (SoundToFormantWorkspace_burg, SoundToFormantWorkspace, 0);

void SoundToFormantWorkspace_burg_initInputDependency (SoundToFormantWorkspace_burg me, double samplingPeriod) {
	SoundToLPCWorkspace_initInputDependency (my soundToLPC.get(), samplingPeriod);
}

void SoundToFormantWorkspace_burg_initOutputDependency (SoundToFormantWorkspace_burg me, integer maxnFormants) {
	SoundToFormantWorkspace_initPartialOutputDependency_number (me, maxnFormants);
}

void SoundToFormantWorkspace_burg_initInputAndOutputDependency (SoundToFormantWorkspace_burg me, double samplingPeriod, integer maxnFormants) {
	SoundToFormantWorkspace_burg_initInputDependency (me, samplingPeriod);
	const integer maxnCoefficients = Melder_ifloor (2 * maxnFormants);
	SoundToLPCWorkspace_burg thee = reinterpret_cast<SoundToLPCWorkspace_burg> (my soundToLPC.get());
	SoundToLPCWorkspace_burg_initOutputDependency (thee, samplingPeriod, maxnCoefficients);
}

autoSoundToFormantWorkspace_burg SoundToFormantWorkspace_burg_create (
	constSound input, mutableFormant output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double margin)
{
	try {
		autoSoundToFormantWorkspace_burg me = Thing_new (SoundToFormantWorkspace_burg);
		SoundToSampledWorkspace_init (me.get(), input, output, effectiveAnalysisWidth, windowShape);
		/*
			Because we inherit from SoundToFormant which inherits from SoundToSampledWorkspace there is not need 
			to start SoundToLPCWorkspace_burg_create with the input Sound.
		*/
		my soundToLPC = SoundToLPCWorkspace_burg_create (nullptr, nullptr, effectiveAnalysisWidth, windowShape);
		/*
			LPCToFormantWorkspace knows how to allocate Formant frames, it therefore needs the output Formant.
		*/
		my lpcToFormant = LPCToFormantWorkspace_create (nullptr, output, margin);
		/*
			We need the input and the output objects together to define the complete workspace.
			If both are not present we can only partially initialize!!
		*/
		if (input && output)
			SoundToFormantWorkspace_burg_initInputAndOutputDependency (me.get(), input -> dx, output -> maxnFormants);
		else if (input)
			SoundToFormantWorkspace_burg_initInputDependency (me.get(), input -> dx);
		else if (output)
			SoundToFormantWorkspace_initPartialOutputDependency_number (me.get(), output -> maxnFormants);
		// else : minimal initialisation
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantWorkspace_burg not created.");
	}
}

Thing_implement (SoundToFormantWorkspace_robust, SoundToFormantWorkspace, 0);

void SoundToFormantWorkspace_robust_initInputDependency (SoundToFormantWorkspace_robust me, double samplingPeriod) {
	SoundToFormantWorkspace_initInputDependency (me, samplingPeriod);
}

void SoundToFormantWorkspace_robust_initOutputDependency (SoundToFormantWorkspace_robust me, integer maxnFormants) {
	const integer maxnCoefficients = Melder_ifloor (2 *  maxnFormants);
	SoundToFormantWorkspace thee = reinterpret_cast<SoundToFormantWorkspace> (me);
	SoundToFormantWorkspace_initPartialOutputDependency_number (thee, maxnFormants);
}

void SoundToFormantWorkspace_robust_initInputAndOutputDependency (SoundToFormantWorkspace_robust me, double samplingPeriod, integer maxnFormants) {
	SoundToFormantWorkspace_robust_initInputDependency (me, samplingPeriod);
	SoundToFormantWorkspace_robust_initOutputDependency (me, maxnFormants);
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
		my lpcToFormant = LPCToFormantWorkspace_create (nullptr, output, margin);
		/*
			Complete initialisation only when both input && output are present
		*/
		if (input && output)
			SoundToFormantWorkspace_robust_initInputAndOutputDependency (me.get(), input -> dx, output -> maxnFormants);
		else if (input)
			SoundToFormantWorkspace_initInputDependency (me.get(), input -> dx);
		else // output
			SoundToFormantWorkspace_robust_initOutputDependency (me.get(), output ->  maxnFormants);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToFormantWorkspace_robust not created.");
	}
}

/* End of file SoundToFormantWorkspace.cpp */
