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
	To specify the SoundToFormantWorkspace completely we need both the input object and the output object and the numberOfPoles.
		input  : supplies the samplingPeriod
		output : supplies the maxnFormants
		numberOfPoles  : specifies the maxnCoefficients (= numberOfPoles) (we can input e.g numberOfFormants = 5.5)
*/

void SoundToFormantWorkspace_initSkeleton (SoundToFormantWorkspace me, constSound input, mutableFormant output);

void SoundToFormantWorkspace_init (SoundToFormantWorkspace me,
	double samplingTime, double effectiveAnalysisWidth, kSound_windowShape windowShape, integer numberOfPoles, double margin
);

inline integer numberOfPolesFromNumberOfFormants (double numberOfFormants) {
	return Melder_ifloor (2.0 * numberOfFormants);
}

/* *********************** Autocorrelation ********************************************** */

Thing_define (SoundToFormantAutocorrelationWorkspace, SoundToFormantWorkspace) {
};

void SoundToFormantAutocorrelationWorkspace_init (SoundToFormantAutocorrelationWorkspace me, double samplingPeriod, integer numberOfPoles, double margin);

autoSoundToFormantAutocorrelationWorkspace SoundToFormantAutocorrelationWorkspace_createSkeleton (constSound input, mutableFormant output);

autoSoundToFormantAutocorrelationWorkspace SoundToFormantAutocorrelationWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, integer numberOfPoles, double margin
);


/* *********************** Covariance ********************************************** */

Thing_define (SoundToFormantCovarianceWorkspace, SoundToFormantWorkspace) {
};

void SoundToFormantCovarianceWorkspace_init (SoundToFormantCovarianceWorkspace me, double samplingPeriod, integer numberOfPoles, double margin);

autoSoundToFormantCovarianceWorkspace SoundToFormantCovarianceWorkspace_createSkeleton (constSound input, mutableFormant output);

autoSoundToFormantCovarianceWorkspace SoundToFormantCovarianceWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, integer numberOfPoles, double margin
);

/* *********************** Burg ********************************************** */

Thing_define (SoundToFormantBurgWorkspace, SoundToFormantWorkspace) {
};

void SoundToFormantBurgWorkspace_init (SoundToFormantBurgWorkspace me, double samplingPeriod, integer numberOfPoles, double margin);

autoSoundToFormantBurgWorkspace SoundToFormantBurgWorkspace_createSkeleton (constSound input, mutableFormant output);

autoSoundToFormantBurgWorkspace SoundToFormantBurgWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, integer numberOfPoles, double margin
);

/* *********************** Marple ********************************************** */

Thing_define (SoundToFormantMarpleWorkspace, SoundToFormantWorkspace) {
};

void SoundToFormantMarpleWorkspace_init (SoundToFormantMarpleWorkspace me, double samplingPeriod, double tol1, double tol2, integer numberOfPoles, double margin);

autoSoundToFormantMarpleWorkspace SoundToFormantMarpleWorkspace_createSkeleton (constSound input, mutableFormant output);

autoSoundToFormantMarpleWorkspace SoundToFormantMarpleWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double tol1, double tol2, integer numberOfPoles, double margin
);

/* *********************** robust ********************************************** */

Thing_define (SoundToFormantRobustWorkspace, SoundToFormantWorkspace) {
};

void SoundToFormantRobustWorkspace_init (SoundToFormantRobustWorkspace me, double samplingPeriod,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax,
	double tol, double location, bool wantlocation, integer numberOfPoles, double margin
);

autoSoundToFormantRobustWorkspace SoundToFormantRobustWorkspace_createSkeleton (constSound input, mutableFormant output);

autoSoundToFormantRobustWorkspace SoundToFormantRobustWorkspace_create (constSound input, mutableFormant output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape, double k_stdev, integer itermax, double tol,
	double location, bool wantlocation, integer numberOfPoles, double margin
);

#endif /*_SoundToFormantWorkspace_h_ */

