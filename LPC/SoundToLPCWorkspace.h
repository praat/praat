#ifndef _SoundToLPCAnalysisWorkspace_h_
#define _SoundToLPCAnalysisWorkspace_h_
/* SoundToLPCAnalysisWorkspace_def.h
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

#include "SoundToLPCAnalysisWorkspace_def.h"

Thing_define (SoundToLPCAnalysisWorkspace_autocorrelation, SoundToLPCAnalysisWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCAnalysisWorkspace_covariance, SoundToLPCAnalysisWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCAnalysisWorkspace_burg, SoundToLPCAnalysisWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

Thing_define (SoundToLPCAnalysisWorkspace_marple, SoundToLPCAnalysisWorkspace) {
	bool inputFrameToOutputFrame (void) override;
};

void SoundToLPCAnalysisWorkspace_init (mutableSoundToLPCAnalysisWorkspace me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

/*
	If the output LPC is not present, as can be the case when SoundToLPCAnalysisWorkspace is part of a 
	chain of workspaces, we need to supply the maxnCoefficients and samplingPeriod  from external sources.
*/
void SoundToLPCAnalysisWorkspace_initLPCDependency (mutableSoundToLPCAnalysisWorkspace me, integer maxnCoefficients, double samplingPeriod);

/*
	If input Sound not present we need to supply the sampling time
*/
inline void SoundToLPCAnalysisWorkspace_initSoundDependency (mutableSoundToLPCAnalysisWorkspace me, double samplingPeriod) {
	my samplingPeriod = samplingPeriod;
	if (my input)
		Melder_assert (my input -> dx == samplingPeriod);
}

void SoundToLPCAnalysisWorkspace_autocorrelation_init (SoundToLPCAnalysisWorkspace_autocorrelation me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCAnalysisWorkspace_autocorrelation SoundToLPCAnalysisWorkspace_autocorrelation_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCAnalysisWorkspace_covariance SoundToLPCAnalysisWorkspace_covariance_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

void SoundToLPCAnalysisWorkspace_burg_init (SoundToLPCAnalysisWorkspace_burg me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCAnalysisWorkspace_burg SoundToLPCAnalysisWorkspace_burg_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape
);

autoSoundToLPCAnalysisWorkspace_marple SoundToLPCAnalysisWorkspace_marple_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth, kSound_windowShape windowShape,
	double tol1, double tol2
);

autoSoundAndLPCToLPCAnalysisWorkspace_robust SoundAndLPCToLPCAnalysisWorkspace_robust_create (
	constSound input, constLPC intermediate, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double k_stdev, integer itermax, double tol, double location, bool wantlocation
);

void SoundToLPCAnalysisWorkspace_robust_init (SoundToLPCAnalysisWorkspace_robust me,
	constSound input, mutableLPC output, double effectiveAnalysisWidth, 
	kSound_windowShape windowShape, double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

autoSoundToLPCAnalysisWorkspace_robust SoundToLPCAnalysisWorkspace_robust_create (
	constSound input, mutableLPC output, double effectiveAnalysisWidth,
	kSound_windowShape windowShape,	double k_stdev,	integer itermax, double tol, double location, bool wantlocation
);

#endif /*_SoundToLPCAnalysisWorkspace_h_ */
