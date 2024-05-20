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

#include "Sampled.h"
#include "Sound.h"
#include "Sound_extensions.h"

#include "SoundAnalysisWorkspace_def.h"

void SoundAnalysisWorkspace_init (SoundAnalysisWorkspace me, Sound thee, Sampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

void SoundAnalysisWorkspace_replaceSound (SoundAnalysisWorkspace me, Sound thee);
/*
	Preconditions: 
		my sound->xmin/xmax = thy xmin/xmax
		my sound->nx = thy nx
		my sound->dx = thy dx
		my sound->x1 = thy x1
*/

void SoundAnalysisWorkspace_analyseThreaded (SoundAnalysisWorkspace me, Sound thee, double preEmphasisFrequency);

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape);

inline void SoundAnalysisWorkspace_subtractLocalMean (SoundAnalysisWorkspace me, bool subtractLocalMean) {
		my subtractLocalMean = subtractLocalMean;
}

#endif /* _SoundAnalysisWorkspace_h_ */
 
