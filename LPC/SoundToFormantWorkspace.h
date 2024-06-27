#ifndef _SoundToFormantAnalysisWorkspace_h_
#define _SoundToFormantAnalysisWorkspace_h_
/* SoundToFormantAnalysisWorkspace_def.h
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

#include "SoundToFormantAnalysisWorkspace_def.h"

Thing_define (SoundToFormantAnalysisWorkspace_burg, SoundToFormantAnalysisWorkspace) {
};

Thing_define (SoundToFormantAnalysisWorkspace_robust, SoundToFormantAnalysisWorkspace) {
};

void SoundToFormantAnalysisWorkspace_initFormantDependency (SoundToFormantAnalysisWorkspace me, integer maxnFormants);
void SoundToFormantAnalysisWorkspace_initSoundDependency (SoundToFormantAnalysisWorkspace me, double samplingPeriod);

autoSoundToFormantAnalysisWorkspace_burg SoundToFormantAnalysisWorkspace_burg_create (constSound input,
	mutableFormant output, integer numberOfFormants, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double margin
);

autoSoundToFormantAnalysisWorkspace_robust SoundToFormantAnalysisWorkspace_robust_create (constSound input,
	mutableFormant output, integer numberOfFormants, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, double margin, double k_stdev, integer itermax, double tol
);

#endif /*_SoundToFormantAnalysisWorkspace_h_ */

