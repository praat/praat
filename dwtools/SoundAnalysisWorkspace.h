#ifndef _SoundAnalysisWorkspace_h_
#define _SoundAnalysisWorkspace_h_
/* SoundAnalysisWorkspace.h
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
#include "SampledAnalysisWorkspace.h"

#include "SoundAnalysisWorkspace_def.h"

void SoundAnalysisWorkspace_init (mutableSoundAnalysisWorkspace me, constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

/*
	If the input == nullptr ad we need to initialize the input soundFrame.
*/
void SoundAnalysisWorkspace_initSoundFrame (mutableSoundAnalysisWorkspace me, double sound_dx);

autoSoundAnalysisWorkspace SoundAnalysisWorkspace_create (constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

void SoundAnalysisWorkspace_analyseThreaded (mutableSoundAnalysisWorkspace me, constSound thee, double preEmphasisFrequency);

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape);

#endif /* _SoundAnalysisWorkspace_h_ */
 
