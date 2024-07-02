#ifndef _SoundToLPCWorkspace_h_
#define _SoundToLPCWorkspace_h_
/* SoundToLPCWorkspace_def.h
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

#include "LPC.h"
#include "Sound.h"
#include "SVD.h"
#include "SoundToSampledWorkspace.h"

#include "SoundToLPCWorkspace_def.h"

Thing_define (SoundToLPCWorkspace_autocorrelation, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCWorkspace_covariance, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCWorkspace_burg, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCWorkspace_marple, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCWorkspace_init (mutableSoundToLPCWorkspace me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/*
	If the input Sound is not present we need to supply the sampling time
*/
void SoundToLPCWorkspace_initInputDependency (mutableSoundToLPCWorkspace me, double samplingPeriod);

/*
	If the output LPC is not present, as can be the case when SoundToLPCWorkspace is part of a 
	chain of workspaces, we need to supply the maxnCoefficients and samplingPeriod  from external sources.
*/
void SoundToLPCWorkspace_initOutputDependency (mutableSoundToLPCWorkspace me, double samplingPeriod, integer maxnCoefficients);

/*
	If both input and output are not present:
*/
void SoundToLPCWorkspace_initInputAndOutputDependency (mutableSoundToLPCWorkspace me, double samplingPeriod, integer maxnCoefficients);

/* ******************** autocorrelation method ***********************/

#define SoundToLPCWorkspace_autocorrelation_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCWorkspace_autocorrelation_initOutputDependency (SoundToLPCWorkspace_autocorrelation me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_autocorrelation_initInputAndOutputDependency (SoundToLPCWorkspace_autocorrelation me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_autocorrelation_initOutputDependency (SoundToLPCWorkspace_autocorrelation me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCWorkspace_autocorrelation SoundToLPCWorkspace_autocorrelation_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** covariance method ***********************/

#define SoundToLPCWorkspace_covariance_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCWorkspace_covariance_initOutputDependency (SoundToLPCWorkspace_covariance me, 
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_covariance_initInputAndOutputDependency (SoundToLPCWorkspace_covariance me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCWorkspace_covariance SoundToLPCWorkspace_covariance_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** burg method ***********************/

#define SoundToLPCWorkspace_burg_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCWorkspace_burg_initOutputDependency (SoundToLPCWorkspace_burg me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_burg_initInputAndOutputDependency (SoundToLPCWorkspace_burg me, 
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCWorkspace_burg SoundToLPCWorkspace_burg_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** marple method ***********************/

#define SoundToLPCWorkspace_burg_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCWorkspace_marple_initOutputDependency (SoundToLPCWorkspace_marple me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_marple_initInputAndOutputDependency (SoundToLPCWorkspace_marple me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCWorkspace_marple SoundToLPCWorkspace_marple_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double tol1, double tol2
);

/**************** SoundAndLPCToLPCWorkspace_robust *************************************/


void SoundAndLPCToLPCWorkspace_robust_initInputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod
);

void SoundAndLPCToLPCWorkspace_robust_initOtherInputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCWorkspace_robust_initOutputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCWorkspace_robust_initInputAndOtherInputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCWorkspace_robust_initInputAndOutputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCWorkspace_robust_initOtherInputAndOutputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCWorkspace_robust_initInputAndOtherInputAndOutputDependency (SoundAndLPCToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundAndLPCToLPCWorkspace_robust SoundAndLPCToLPCWorkspace_robust_create (
	constSound input, constLPC otherInput, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

/**************** SoundToLPCWorkspace_robust *************************************/

void SoundToLPCWorkspace_robust_initInputDependency (SoundToLPCWorkspace_robust me,
	double samplingPeriod
);

void SoundToLPCWorkspace_robust_initOutputDependency (SoundToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCWorkspace_robust_initInputAndOutputDependency (SoundToLPCWorkspace_robust me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCWorkspace_robust SoundToLPCWorkspace_robust_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

#endif /*_SoundToLPCWorkspace_h_ */
