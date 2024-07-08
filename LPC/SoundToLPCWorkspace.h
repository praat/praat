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

void SoundToLPCWorkspace_initSkeleton (mutableSoundToLPCWorkspace me, constSound input, mutableLPC output);

void SoundToLPCWorkspace_init (mutableSoundToLPCWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients
);

/********************* autocorrelation method ***********************/

Thing_define (SoundToLPCAutocorrelationWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCAutocorrelationWorkspace_init (SoundToLPCAutocorrelationWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients
);

autoSoundToLPCAutocorrelationWorkspace SoundToLPCAutocorrelationWorkspace_createSkeleton (constSound input, mutableLPC output);

autoSoundToLPCAutocorrelationWorkspace SoundToLPCAutocorrelationWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/********************* covariance method ***********************/

Thing_define (SoundToLPCCovarianceWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCCovarianceWorkspace_init (SoundToLPCCovarianceWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients
);

autoSoundToLPCCovarianceWorkspace SoundToLPCCovarianceWorkspace_createSkeleton (constSound input, mutableLPC output);

autoSoundToLPCCovarianceWorkspace SoundToLPCCovarianceWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/********************* burg method ***********************/

Thing_define (SoundToLPCBurgWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCBurgWorkspace_init (SoundToLPCBurgWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients
);

autoSoundToLPCBurgWorkspace SoundToLPCBurgWorkspace_createSkeleton (constSound input, mutableLPC output);

autoSoundToLPCBurgWorkspace SoundToLPCBurgWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape
);


/********************* marple method ***********************/

Thing_define (SoundToLPCMarpleWorkspace, SoundToLPCWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCMarpleWorkspace_init (SoundToLPCMarpleWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer maxnCoefficients,
	double tol1, double tol2
);

autoSoundToLPCMarpleWorkspace SoundToLPCMarpleWorkspace_createSkeleton (constSound input, mutableLPC output);

autoSoundToLPCMarpleWorkspace SoundToLPCMarpleWorkspace_create (constSound input, mutableLPC output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double tol1, double tol2
);


/**************** SoundAndLPCToLPCRobustWorkspace *************************************/

void SoundAndLPCToLPCRobustWorkspace_init (SoundAndLPCToLPCRobustWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	integer maxnCoefficients, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

autoSoundAndLPCToLPCRobustWorkspace SoundAndLPCToLPCRobustWorkspace_createSkeleton (
	constSound input, constLPC otherInput, mutableLPC output
);

autoSoundAndLPCToLPCRobustWorkspace SoundAndLPCToLPCRobustWorkspace_create (constSound input, constLPC otherInput, mutableLPC output, 
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev,
	integer itermax, double tol, double location, bool wantlocation
);

/**************** SoundToLPCRobustWorkspace *************************************/

void SoundToLPCRobustWorkspace_init (SoundToLPCRobustWorkspace me,
	double samplingPeriod, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	integer maxnCoefficients,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

autoSoundToLPCRobustWorkspace SoundToLPCRobustWorkspace_createSkeleton (constSound input, mutableLPC output);

autoSoundToLPCRobustWorkspace SoundToLPCRobustWorkspace_create (constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

#endif /*_SoundToLPCWorkspace_h_ */
