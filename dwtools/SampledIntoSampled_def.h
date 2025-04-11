/* SampledIntoSampled_def.h
 *
 * Copyright (C) 2024-2025 David Weenink
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
	
/*
	A separate deep copy of the SampledIntoSampled is needed for each thread
*/

#define ooSTRUCT SampledIntoSampled
oo_DEFINE_CLASS (SampledIntoSampled, Daata)

	/*
		Only a reference to the Sampled input and output objects are needed because
		each thread only accesses disjoint parts of it. And only the reference needs to be copied.
		The domains of input and output should be equal, the samplings in general are different.
	*/
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Sampled, input)
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Sampled, output)
	/*
		Workload dispatcher 
	*/
	oo_BOOLEAN (useMultiThreading)
	oo_INTEGER (minimumNumberOfFramesPerThread) // default 40
	oo_INTEGER (maximumNumberOfFramesPerThread) // default 0 (= no limit)
	oo_INTEGER (maximumNumberOfThreads)
	/*
		For all temporary work data in a frame analysis
	*/
	oo_OBJECT (SampledFrameIntoSampledFrame, 0, frameIntoFrame)
	oo_INTEGER (globalFrameErrorCount)			// the number of frames where some error occurred

oo_END_CLASS (SampledIntoSampled)
#undef ooSTRUCT

/* End of file SampledInToSampled_def.h */
 
