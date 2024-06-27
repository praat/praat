#ifndef _SoundToSampledWorkspace_h_
#define _SoundToSampledWorkspace_h_
/* SoundToSampledWorkspace.h
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

#include "Sound.h"
#include "SampledToSampledWorkspace.h"

#include "SoundToSampledWorkspace_def.h"

void SoundToSampledWorkspace_init (mutableSoundToSampledWorkspace me, constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

/*
	If the input == nullptr ad we need to initialize the input soundFrame.
*/
void SoundToSampledWorkspace_initSoundFrame (mutableSoundToSampledWorkspace me, double sound_dx);

autoSoundToSampledWorkspace SoundToSampledWorkspace_create (constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

void SoundToSampledWorkspace_analyseThreaded (mutableSoundToSampledWorkspace me, constSound thee, double preEmphasisFrequency);

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape);

#endif /* _SoundToSampledWorkspace_h_ */
 
