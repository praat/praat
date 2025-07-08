/* SampledFrameIntoMatrixFrame_def.h
 *
 * Copyright (C) 2025 David Weenink
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
	A separate deep copy of the SampledFrameIntoMatrixFrame is needed for each thread
*/

#define ooSTRUCT SampledFrameIntoMatrixFrame
oo_DEFINE_CLASS (SampledFrameIntoMatrixFrame, SampledFrameIntoSampledFrame)

	/*
		Because each frame of the output Matrix is in a column, the memory access is spread.
		For each thread we might need local memory.
	*/
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Matrix, matrix)
	oo_MAT (localOutput, matrix -> ny, maximumNumberOfFrames)

	#if oo_DECLARING

		void allocateMemoryAfterThreadsAreKnown () override;

	#endif

oo_END_CLASS (SampledFrameIntoMatrixFrame)
#undef ooSTRUCT

/* End of file SampledFrameIntoMatrixFrame_def.h */
 
