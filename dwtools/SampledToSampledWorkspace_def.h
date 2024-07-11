/* SampledToSampledWorkspace_def.h
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

/*
	A separate deep copy of the SampledToSampledWorkspace is needed for each thread
*/
#define ooSTRUCT SampledToSampledWorkspace
oo_DEFINE_CLASS (SampledToSampledWorkspace, Daata)

	#if oo_DECLARING

		/*
			Only a reference to the Sampled input and output objects are needed because
			each thread only accesses disjoint parts of it. And only the reference needs to be copied.
			The domains of input and output should be equal, the sampling can be different.
		*/
		constSampled input;
		Sampled output;

	#endif

	#if oo_COPYING

		thy input = input;
		thy output = output;

	#endif
		
	oo_BOOLEAN (inputObjectPresent)
	oo_BOOLEAN (outputObjectPresent)
	
	oo_BOOLEAN (useMultiThreading)
	oo_INTEGER (minimumNumberOfFramesPerThread) // default 40
	oo_INTEGER (maximumNumberOfThreads)
	
	oo_INTEGER (currentFrame)				// the frame we are working on
	oo_INTEGER (frameAnalysisInfo)			// signals different "error" conditions etc in a frame analysis
	oo_BOOLEAN (frameAnalysisIsOK)			// signals whether the analysis is OK or not on the basis of the frameAnalysisInfo
	oo_INTEGER (globalFrameErrorCount)		// the number of frames where some error occured
	
	/*
		For approximations we need tolerances
	*/
	oo_DOUBLE (tol1)
	oo_DOUBLE (tol2)
		
	/*
		For all temporary work vectors in a frame analysis
	*/
	oo_OBJECT (WorkvectorPool, 0, workvectorPool)
	
	#if oo_DECLARING

		virtual void getInputFrame (void);
		
		virtual bool inputFrameToOutputFrame (void); // sets the frameAnalysisInfo and also frameAnalysisIsOK
		
		virtual void saveOutputFrame (void);
		
		virtual void inputFramesToOutputFrames (integer fromFrame, integer toFrame); // sets currentFrame

		virtual void allocateOutputFrames (void);

	#endif

oo_END_CLASS (SampledToSampledWorkspace)
#undef ooSTRUCT

/* End of file SampledToSampledWorkspace_def.h */
 
