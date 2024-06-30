/* SoundToLPCWorkspace_def.h
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

#define ooSTRUCT SoundToLPCWorkspace
oo_DEFINE_CLASS (SoundToLPCWorkspace, SoundToSampledWorkspace)

	oo_INTEGER (maxnCoefficients) 			// if output object not present!
	oo_DOUBLE (samplingPeriod)				// if input && output object not present
	oo_STRUCT (LPC_Frame, outputLPCFrame)

	#if oo_DECLARING

		LPC_Frame outputLPCFrameRef = & outputLPCFrame;

		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

	#if oo_COPYING

		thy outputLPCFrameRef = & thy outputLPCFrame;

	#endif

oo_END_CLASS (SoundToLPCWorkspace)
#undef ooSTRUCT

#define ooSTRUCT SoundAndLPCToLPCWorkspace_robust
oo_DEFINE_CLASS (SoundAndLPCToLPCWorkspace_robust, SoundToLPCWorkspace)

	oo_BOOLEAN (otherInputObjectPresent)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, otherInput)
	oo_STRUCT (LPC_Frame, otherInputLPCFrame)
	
	oo_INTEGER (computeSVDworksize)
	oo_DOUBLE (k_stdev)
	oo_INTEGER (currentPredictionOrder) // may change from frame to frame
	oo_INTEGER (iter)
	oo_INTEGER (itermax)
	oo_INTEGER (huber_iterations) // = 5;
	oo_BOOLEAN (wantlocation)
	oo_BOOLEAN (wantscale)
	oo_DOUBLE (location)
	oo_DOUBLE (scale)
	oo_VEC (error, soundFrameSize)
	oo_VEC (sampleWeights, soundFrameSize)
	oo_VEC (coefficients, otherInput -> maxnCoefficients)
	oo_VEC (covariancesw, otherInput -> maxnCoefficients)
	oo_MAT (covarmatrixw, otherInput -> maxnCoefficients, otherInput -> maxnCoefficients)
	oo_OBJECT (SVD, 1, svd)

	#if oo_DECLARING

		LPC_Frame otherInputLPCFrameRef = & otherInputLPCFrame;
		
		void getInputFrame (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

	#if oo_COPYING

		thy otherInputLPCFrameRef = & thy otherInputLPCFrame;

	#endif
		
oo_END_CLASS (SoundAndLPCToLPCWorkspace_robust)
#undef ooSTRUCT

#define ooSTRUCT SoundToLPCWorkspace_robust
oo_DEFINE_CLASS (SoundToLPCWorkspace_robust, SoundToLPCWorkspace)
	/*
		We inherit from SoundToLPCWorkspace (instead of SampledToSampledWorkspace)
		to determine the time info (redundant because we can't do multiple inheritance)
	*/
	oo_OBJECT (SoundToLPCWorkspace, 0, soundToLPC)
	oo_OBJECT (SoundAndLPCToLPCWorkspace_robust, 0, soundAndLPCToLPC)

	#if oo_DECLARING

		void getInputFrame (void) override;
		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

oo_END_CLASS (SoundToLPCWorkspace_robust)
#undef ooSTRUCT

/* End of file SoundToLPCWorkspace_def.h */
 
