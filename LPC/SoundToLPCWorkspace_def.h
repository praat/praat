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

	oo_DOUBLE (samplingPeriod)				// if input && output object not present
	oo_INTEGER (maxnCoefficients) 			// if output object not present!
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

#define ooSTRUCT SoundAndLPCToLPCRobustWorkspace
oo_DEFINE_CLASS (SoundAndLPCToLPCRobustWorkspace, SoundToLPCWorkspace)

	oo_STRUCT (LPC_Frame, otherInputLPCFrame)
	
	#if oo_DECLARING

		constLPC otherInput;
		LPC_Frame otherInputLPCFrameRef = & otherInputLPCFrame;

	#endif

	#if oo_COPYING

		thy otherInput = otherInput;
		thy otherInputLPCFrameRef = & thy otherInputLPCFrame;

	#endif
		

	oo_BOOLEAN (otherInputObjectPresent)
	oo_INTEGER (currentPredictionOrder)
	
	oo_DOUBLE (k_stdev)
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
	oo_INTEGER (computeSVDworksize)
	oo_OBJECT (SVD, 1, svd)

	#if oo_DECLARING

		void getInputFrame (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

oo_END_CLASS (SoundAndLPCToLPCRobustWorkspace)
#undef ooSTRUCT

#define ooSTRUCT SoundToLPCRobustWorkspace
oo_DEFINE_CLASS (SoundToLPCRobustWorkspace, SoundToLPCWorkspace)
	/*
		We inherit from SoundToLPCWorkspace (instead of SampledToSampledWorkspace)
		to determine the time info (redundant because we can't do multiple inheritance)
	*/
	oo_OBJECT (SoundToLPCWorkspace, 0, soundToLPC)
	oo_OBJECT (SoundAndLPCToLPCRobustWorkspace, 0, soundAndLPCToLPC)

	#if oo_DECLARING

		void getInputFrame (void) override;
		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

oo_END_CLASS (SoundToLPCRobustWorkspace)
#undef ooSTRUCT

/* End of file SoundToLPCWorkspace_def.h */
 
