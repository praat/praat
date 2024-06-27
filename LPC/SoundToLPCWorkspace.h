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
	If the output LPC is not present, as can be the case when SoundToLPCWorkspace is part of a 
	chain of workspaces, we need to supply the maxnCoefficients and samplingPeriod  from external sources.
*/
void SoundToLPCWorkspace_initLPCDependency (mutableSoundToLPCWorkspace me, integer maxnCoefficients, double samplingPeriod);

/*
	If input Sound not present we need to supply the sampling time
*/
inline void SoundToLPCWorkspace_initSoundDependency (mutableSoundToLPCWorkspace me, double samplingPeriod) {
	my samplingPeriod = samplingPeriod;
	if (my input)
		Melder_assert (my input -> dx == samplingPeriod);
}

void SoundToLPCWorkspace_autocorrelation_init (SoundToLPCWorkspace_autocorrelation me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCWorkspace_autocorrelation SoundToLPCWorkspace_autocorrelation_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCWorkspace_covariance SoundToLPCWorkspace_covariance_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

void SoundToLPCWorkspace_burg_init (SoundToLPCWorkspace_burg me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCWorkspace_burg SoundToLPCWorkspace_burg_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCWorkspace_marple SoundToLPCWorkspace_marple_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double tol1, double tol2
);

autoSoundAndLPCToLPCWorkspace_robust SoundAndLPCToLPCWorkspace_robust_create (
	constSound input, constLPC intermediate, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

void SoundToLPCWorkspace_robust_init (SoundToLPCWorkspace_robust me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, 
	kSound_windowShape windowShape, double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

autoSoundToLPCWorkspace_robust SoundToLPCWorkspace_robust_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

#endif /*_SoundToLPCWorkspace_h_ */
