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
#include "WorkvectorPool.h"

#include "SampledAnalysisWorkspace_def.h"

void SampledAnalysisWorkspace_init (mutableSampledAnalysisWorkspace me, constSampled input, mutableSampled output);

autoSampledAnalysisWorkspace SampledAnalysisWorkspace_create (constSampled input, mutableSampled output);

void SampledAnalysisWorkspace_initWorkvectorPool (mutableSampledAnalysisWorkspace me, constINTVEC const& vectorSizes);

void SampledAnalysisWorkspace_getThreadingInfo (constSampledAnalysisWorkspace me, integer maximumNumberOfThreads, integer numberOfFramesPerThread, integer *out_numberOfThreads);

void SampledAnalysisWorkspace_replaceOutput (mutableSampledAnalysisWorkspace me, mutableSampled thee);
/*
	Preconditions:
		my output -> xmin == thy xmin
		my output -> xmax == thy xmax
		my output -> nx   == thy nx
		my output -> dx   == thy dx
		my output -> x1   == thy x1
*/

void SampledAnalysisWorkspace_replaceInput (mutableSampledAnalysisWorkspace me, constSampled input);
/*
	Preconditions:
		my output -> xmin == thy xmin
		my output -> xmax == thy xmax
		my output -> nx   == thy nx
		my output -> dx   == thy dx
		my output -> x1   == thy x1
*/

void SampledAnalysisWorkspace_analyseThreaded (mutableSampledAnalysisWorkspace me);

#endif /* _SampledAnalysisWorkspace_h_ */
 
