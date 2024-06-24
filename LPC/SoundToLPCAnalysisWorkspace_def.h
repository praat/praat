/* SoundToLPCAnalysisWorkspace_def.h
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

#define ooSTRUCT SoundToLPCAnalysisWorkspace
oo_DEFINE_CLASS (SoundToLPCAnalysisWorkspace, SoundAnalysisWorkspace)

	oo_INTEGER (maxnCoefficients) 			// if output object not present!
	oo_STRUCT (LPC_Frame, outputLPCFrame)

	#if oo_DECLARING

		LPC_Frame outputLPCFrameRef = & outputLPCFrame;

		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

	#if oo_COPYING

		thy outputLPCFrameRef = outputLPCFrameRef;

	#endif

oo_END_CLASS (SoundToLPCAnalysisWorkspace)
#undef ooSTRUCT

#define ooSTRUCT SoundAndLPCToLPCAnalysisWorkspace_robust
oo_DEFINE_CLASS (SoundAndLPCToLPCAnalysisWorkspace_robust, SoundToLPCAnalysisWorkspace)

	oo_BOOLEAN (intermediateObjectPresent)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, intermediate) // read-only original
	oo_STRUCT (LPC_Frame, intermediateLPCFrame)
	
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
	oo_VEC (coefficients, intermediate -> maxnCoefficients)
	oo_VEC (covariancesw, intermediate -> maxnCoefficients)
	oo_MAT (covarmatrixw, intermediate -> maxnCoefficients, intermediate -> maxnCoefficients)
	oo_OBJECT (SVD, 1, svd)

	#if oo_DECLARING

		LPC_Frame intermediateLPCFrameRef = & intermediateLPCFrame;
		
		void getInputFrame (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

	#if oo_COPYING

		thy intermediateLPCFrameRef = intermediateLPCFrameRef;

	#endif
		
oo_END_CLASS (SoundAndLPCToLPCAnalysisWorkspace_robust)
#undef ooSTRUCT

#define ooSTRUCT SoundToLPCAnalysisWorkspace_robust
oo_DEFINE_CLASS (SoundToLPCAnalysisWorkspace_robust, SoundToLPCAnalysisWorkspace)
	/*
		We inherit from SoundToLPCAnalysisWorkspace (instead of SampledAnalysisWorkspace)
		to determine the timing because we can't do multiple inheritance
	*/
	oo_OBJECT (SoundToLPCAnalysisWorkspace, 0, soundToLPC)
	oo_OBJECT (SoundAndLPCToLPCAnalysisWorkspace_robust, 0, soundAndLPCToLPC)

	#if oo_DECLARING

		void getInputFrame (void) override;
		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

oo_END_CLASS (SoundToLPCAnalysisWorkspace_robust)
#undef ooSTRUCT

/* End of file SoundToLPCAnalysisWorkspace_def.h */
 
