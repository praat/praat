/* SampledAnalysisWorkspace_def.h
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

#define ooSTRUCT WorkvectorPool
oo_DEFINE_CLASS (WorkvectorPool, Daata)

	oo_INTEGER (poolMemorySize)
	oo_INTEGER (numberOfVectors)
	oo_INTVEC (vectorSizes, numberOfVectors)
	oo_INTVEC (vectorStart, numberOfVectors)
	oo_VEC (memoryPool, poolMemorySize)
	oo_BOOLEAN (reusable)
	oo_BOOLVEC (inuse, numberOfVectors)
	
	#if oo_DECLARING
	
		VEC getRawVEC (integer vectorIndex, integer size) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			Melder_assert (size <= vectorSizes [vectorIndex]);
			Melder_assert (reusable || (! reusable && ! inuse [vectorIndex]));
			inuse [vectorIndex] = true;
			return memoryPool.part (vectorStart [vectorIndex], size);
		}
		
		VEC getZeroVEC (integer vectorIndex, integer size) {
			VEC result = getRawVEC (vectorIndex, size);
			result  <<=  0.0;
			return result;
		}
		
		MAT getRawMAT (integer vectorIndex, integer nrow, integer ncol) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			const integer wantedSize = nrow * ncol;
			Melder_assert (wantedSize <= vectorSizes [vectorIndex]);
			inuse [vectorIndex] = true;
			VEC pool = memoryPool.part (vectorStart [vectorIndex], wantedSize);
			return pool.asmatrix (nrow, ncol);
		}
		
		MAT getZeroMAT (integer vectorIndex, integer nrow, integer ncol) {
			MAT result = getRawMAT (vectorIndex, nrow, ncol);
			result  <<=  0.0;
			return result;
		}
		
		void freeVEC (integer vectorIndex) {
			Melder_assert (vectorIndex > 0 && vectorIndex <= numberOfVectors);
			Melder_assert (inuse [vectorIndex]);
			inuse [vectorIndex] = false;
		}
		
	#endif

oo_END_CLASS (WorkvectorPool)
#undef ooSTRUCT

/*
	A separate deep copy of the SampledAnalysisWorkspace is needed for each thread
*/
#define ooSTRUCT SampledAnalysisWorkspace
oo_DEFINE_CLASS (SampledAnalysisWorkspace, Daata)

	/*
		Only a reference to the Sampled that is analysed is needed in a thread because
		each thread only accesses disjoint parts of it.
	*/
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Sampled, input)

	/*
		Each thread accesses the same disjoint parts in the result.
		Precondition:
			input -> xmin == output -> xmin;
			input -> xmax == output -> xmax;
	*/
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Sampled, output)
	
	oo_BOOLEAN (useMultiThreading)
	oo_INTEGER (minimumNumberOfFramesPerThread) // 40 ?
	oo_INTEGER (maximumNumberOfThreads)
	
	oo_INTEGER (frameAnalysisInfo)			// signals different error conditions etc in an analysis
	oo_BOOLEAN (frameAnalysisIsOK)			// determines on the basis of the frameAnalysisInfo whether the analysis is OK or not
	oo_INTEGER (globalFrameErrorCount)		// the number of frames where some error occured
	oo_INTEGER (currentFrame)				// the frame we are working on
	
	/*
		For approximations we need tolerances
	*/
	oo_DOUBLE (tol1)
	oo_DOUBLE (tol2)
		
	/*
		For all temporary work vectors in the analysis
	*/
	oo_OBJECT (WorkvectorPool, 0, workvectorPool)
	
	#if oo_DECLARING
		void (*getInputFrame) (SampledAnalysisWorkspace me, integer iframe);
		void (*analyseOneInputFrame) (SampledAnalysisWorkspace me); // has to set frameAnalysisIsOK to true or false
		void (*allocateOutputFrames) (SampledAnalysisWorkspace me);
	#endif
		
	#if oo_COPYING
		thy getInputFrame = getInputFrame;
		thy analyseOneInputFrame = our analyseOneInputFrame;
		thy allocateOutputFrames = our allocateOutputFrames;
	#endif	
		
	#if oo_DECLARING
		
		void analyseManyInputFrames (SampledAnalysisWorkspace me, integer fromFrame, integer toFrame);
		
	#endif

oo_END_CLASS (SampledAnalysisWorkspace)
#undef ooSTRUCT

/* End of file SampledAnalysisWorkspace_def.h */
 
