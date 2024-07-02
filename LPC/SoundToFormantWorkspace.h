#ifndef _SoundToFormantWorkspace_h_
#define _SoundToFormantWorkspace_h_
/* SoundToFormantWorkspace_def.h
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

#include "Formant.h"
#include "LPC.h"
#include "Sound.h"
#include "LPCToFormantWorkspace.h"
#include "SoundToLPCWorkspace.h"

#include "SoundToFormantWorkspace_def.h"

/*
	Workspace to calculate formant frequencies from intermediate LPC coefficients.
	To specify the SoundToFormantWorkspace completely we need both the input object and the output object.
		input  : supplies the samplingPeriod
		output : supplies the maxnFormants from which the maxnCoefficients (= 2 * maxnFormants) for the LPC are calulated.
*/

Thing_define (SoundToFormantWorkspace_burg, SoundToFormantWorkspace) {
};

Thing_define (SoundToFormantWorkspace_robust, SoundToFormantWorkspace) {
};

void SoundToFormantWorkspace_initPartialOutputDependency_sampling (SoundToFormantWorkspace me, double samplingPeriod);

void SoundToFormantWorkspace_initPartialOutputDependency_number (SoundToFormantWorkspace me, integer maxnFormants);

/* *********************** burg ********************************************** */

void SoundToFormantWorkspace_burg_initInputDependency (SoundToFormantWorkspace me, double samplingPeriod);

void SoundToFormantWorkspace_burg_initOutputDependency (SoundToFormantWorkspace me, integer maxnFormants);

void SoundToFormantWorkspace_burg_initInputAndOutputDependency (SoundToFormantWorkspace me, double samplingPeriod, integer maxnFormants);

autoSoundToFormantWorkspace_burg SoundToFormantWorkspace_burg_create (constSound input,
	mutableFormant output, double effectiveAnalysisWidth, kSound_windowShape windowShape, double margin
);

/* *********************** robust ********************************************** */

void SoundToFormantWorkspace_robust_initInputDependency (SoundToFormantWorkspace me, double samplingPeriod);

void SoundToFormantWorkspace_robust_initOutputDependency (SoundToFormantWorkspace me, integer maxnFormants);

void SoundToFormantWorkspace_robust_initInputAndOutputDependency (SoundToFormantWorkspace me, double samplingPeriod, integer maxnFormants);

autoSoundToFormantWorkspace_robust SoundToFormantWorkspace_robust_create (constSound input,
	mutableFormant output, double effectiveAnalysisWidth, kSound_windowShape windowShape, double margin,
	double k_stdev, integer itermax, double tol
);

#endif /*_SoundToFormantWorkspace_h_ */

