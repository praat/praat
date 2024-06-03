#ifndef _SampledAnalysisWorkspace_h_
#define _SampledAnalysisWorkspace_h_
/* SampledAnalysisWorkspace.h
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

#include "SampledAnalysisWorkspace_def.h"

autoWorkvectorPool WorkvectorPool_create (INTVEC const& vectorSizes, bool reusable);

void SampledAnalysisWorkspace_init (SampledAnalysisWorkspace me, Sampled input, Sampled output);

autoSampledAnalysisWorkspace SampledAnalysisWorkspace_create (Sampled input, Sampled output);

void SampledAnalysisWorkspace_initWorkvectorPool (SampledAnalysisWorkspace me, INTVEC const& vectorSizes);

void SampledAnalysisWorkspace_getThreadingInfo (SampledAnalysisWorkspace me, integer maximumNumberOfThreads, integer numberOfFramesPerThread, integer *out_numberOfThreads);

void SampledAnalysisWorkspace_replaceOutput (SampledAnalysisWorkspace me, Sampled thee);
/*
	Preconditions:
		my output -> xmin == thy xmin
		my output -> xmax == thy xmax
		my output -> nx   == thy nx
		my output -> dx   == thy dx
		my output -> x1   == thy x1
*/

void SampledAnalysisWorkspace_replaceInput (SampledAnalysisWorkspace me, Sampled input);
/*
	Preconditions:
		my output -> xmin == thy xmin
		my output -> xmax == thy xmax
		my output -> nx   == thy nx
		my output -> dx   == thy dx
		my output -> x1   == thy x1
*/

void SampledAnalysisWorkspace_analyseThreaded (SampledAnalysisWorkspace me);

#endif /* _SampledAnalysisWorkspace_h_ */
 
