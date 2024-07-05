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

Thing_define (SoundToLPCAutocorrelationWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCCovarianceWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCBurgWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCMarpleWorkspace, SoundToLPCWorkspace) {
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

#define SoundToLPCAutocorrelationWorkspace_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCAutocorrelationWorkspace_initOutputDependency (SoundToLPCAutocorrelationWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCAutocorrelationWorkspace_initInputAndOutputDependency (SoundToLPCAutocorrelationWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCAutocorrelationWorkspace_initOutputDependency (SoundToLPCAutocorrelationWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCAutocorrelationWorkspace SoundToLPCAutocorrelationWorkspace_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** covariance method ***********************/

#define SoundToLPCCovarianceWorkspace_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCCovarianceWorkspace_initOutputDependency (SoundToLPCCovarianceWorkspace me, 
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCCovarianceWorkspace_initInputAndOutputDependency (SoundToLPCCovarianceWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCCovarianceWorkspace SoundToLPCCovarianceWorkspace_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** burg method ***********************/

#define SoundToLPCBurgWorkspace_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCBurgWorkspace_initOutputDependency (SoundToLPCBurgWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCBurgWorkspace_initInputAndOutputDependency (SoundToLPCBurgWorkspace me, 
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCBurgWorkspace SoundToLPCBurgWorkspace_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/* ******************** marple method ***********************/

#define SoundToLPCBurgWorkspace_initInputDependency SoundToLPCWorkspace_initInputDependency

void SoundToLPCMarpleWorkspace_initOutputDependency (SoundToLPCMarpleWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCMarpleWorkspace_initInputAndOutputDependency (SoundToLPCMarpleWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCMarpleWorkspace SoundToLPCMarpleWorkspace_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double tol1, double tol2
);

/**************** SoundAndLPCToLPCRobustWorkspace *************************************/


void SoundAndLPCToLPCRobustWorkspace_initInputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod
);

void SoundAndLPCToLPCRobustWorkspace_initOtherInputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCRobustWorkspace_initOutputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCRobustWorkspace_initInputAndOtherInputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCRobustWorkspace_initInputAndOutputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCRobustWorkspace_initOtherInputAndOutputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundAndLPCToLPCRobustWorkspace_initInputAndOtherInputAndOutputDependency (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundAndLPCToLPCRobustWorkspace SoundAndLPCToLPCRobustWorkspace_create (
	constSound input, constLPC otherInput, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

/**************** SoundToLPCRobustWorkspace *************************************/

void SoundToLPCRobustWorkspace_initInputDependency (SoundToLPCRobustWorkspace me,
	double samplingPeriod
);

void SoundToLPCRobustWorkspace_initOutputDependency (SoundToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

void SoundToLPCRobustWorkspace_initInputAndOutputDependency (SoundToLPCRobustWorkspace me,
	double samplingPeriod, integer maxnCoefficients
);

autoSoundToLPCRobustWorkspace SoundToLPCRobustWorkspace_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

#endif /*_SoundToLPCWorkspace_h_ */
