#ifndef _SampledToSampledWorkspace_h_
#define _SampledToSampledWorkspace_h_
/* SampledToSampledWorkspace.h
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

#include "SampledToSampledWorkspace_def.h"

void SampledToSampledWorkspace_init (mutableSampledToSampledWorkspace me, constSampled input, mutableSampled output);

void SampledToSampledWorkspace_getThreadingInfo (constSampledToSampledWorkspace me, integer maximumNumberOfThreads, integer numberOfFramesPerThread, integer *out_numberOfThreads);

void SampledToSampledWorkspace_replaceOutput (mutableSampledToSampledWorkspace me, mutableSampled thee);
/*
	Preconditions:
		my output -> xmin == thy xmin && my output -> xmax == thy xmax
		my output -> nx   == thy nx && my output -> dx   == thy dx && my output -> x1   == thy x1
*/

void SampledToSampledWorkspace_replaceInput (mutableSampledToSampledWorkspace me, constSampled input);
/*
	Preconditions:
		my output -> xmin == thy xmin && my output -> xmax == thy xmax
		my output -> nx   == thy nx && my output -> dx   == thy dx && my output -> x1   == thy x1
*/

void SampledToSampledWorkspace_analyseThreaded (mutableSampledToSampledWorkspace me);

inline void Sampled_requireEqualSampling (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Melder_require (my x1 == thy x1 && my nx == thy nx && my dx == thy dx,
		U"The sampling of ", me, U" and ", thee, U" should be equal.");
}

inline void Sampled_assertEqualSampling (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Melder_assert (my x1 == thy x1 && my nx == thy nx && my dx == thy dx);
}

inline void Sampled_requireEqualDomains (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Melder_require (my xmin == thy xmin && my xmax == thy xmax,
		U"The domains of ", me, U" and ", thee, U" should be equal.");
}

inline void Sampled_assertEqualDomains (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Melder_assert (my xmin == thy xmin && my xmax == thy xmax);
}

inline void Sampled_requireEqualDomainsAndSampling (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Sampled_requireEqualDomains (me, thee);
	Sampled_requireEqualSampling (me, thee);
}

inline void Sampled_assertEqualDomainsAndSampling (constSampled me,  constSampled thee) {
	Melder_assert (me && thee);
	Sampled_assertEqualDomains (me, thee);
	Sampled_assertEqualSampling (me, thee);
}

#endif /* _SampledToSampledWorkspace_h_ */
 
