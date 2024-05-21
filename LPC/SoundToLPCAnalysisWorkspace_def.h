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
	/*
		Instead of creating a different SoundToLPCAnalysisWorkspace type for each different analysis,
		we create a generic SoundToLPCAnalysisWorkspace that can handle the auto, covar, marple and burg 
		algorithms.
	*/
	oo_INTEGER (v1workCapacity)
	oo_VEC (v1work, v1workCapacity) // for covar, auto, burg, marple, robust
	oo_INTEGER (v2workCapacity)
	oo_VEC (v2work, v2workCapacity) // for covar, auto, burg, marple, robust
	oo_INTEGER (v3workCapacity)
	oo_VEC (v3work, v3workCapacity) // for covar, auto, burg, marple, robust
	oo_INTEGER (v4workCapacity)
	oo_VEC (v4work, v4workCapacity) // for covar, robust
	oo_INTEGER (v5workCapacity)
	oo_VEC (v5work, v5workCapacity) // for covar
	oo_DOUBLE (tolerance1)		// for marple
	oo_DOUBLE (tolerance2)		// for marple
	
	#if oo_DECLARING
		
		virtual void getAutocorrelations (SoundToLPCAnalysisWorkspace /* me */, VEC) {};
		
		void allocateSampledFrames (SoundAnalysisWorkspace me) override {
			Melder_assert (my result != nullptr);
			LPC thee = reinterpret_cast<LPC> (my result);
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				const LPC_Frame lpcFrame = & thy d_frames [iframe];
				LPC_Frame_init (lpcFrame, thy maxnCoefficients);
			}
		}
		
	#endif

oo_END_CLASS (SoundToLPCAnalysisWorkspace)
#undef ooSTRUCT

#define ooSTRUCT SoundToLPCRobustAnalysisWorkspace
oo_DEFINE_CLASS (SoundToLPCRobustAnalysisWorkspace, SoundToLPCAnalysisWorkspace)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, original) // read-only original
	
	oo_DOUBLE (k_stdev)
	oo_INTEGER (predictionOrder)
	oo_INTEGER (iter)
	oo_INTEGER (itermax)
	oo_INTEGER (huber_iterations) // = 5;
	oo_BOOLEAN (wantlocation)
	oo_BOOLEAN (wantscale)
	oo_DOUBLE (location)
	oo_DOUBLE (scale)
	oo_VEC (error, analysisFrameSize)
	oo_VEC (sampleWeights, analysisFrameSize)
	oo_VEC (coefficients, original -> maxnCoefficients)
	oo_VEC (covariancesw, original -> maxnCoefficients)
	oo_MAT (covarmatrixw, original -> maxnCoefficients, original -> maxnCoefficients)
	oo_OBJECT (SVD, 1, svd)
	
	#if oo_DECLARING

		void allocateSampledFrames (SoundAnalysisWorkspace him) override {
			SoundToLPCRobustAnalysisWorkspace me = reinterpret_cast<SoundToLPCRobustAnalysisWorkspace> (him);
			Melder_assert (my result != nullptr);
			Melder_assert (my original != nullptr);
			LPC thee = reinterpret_cast<LPC> (result);
			Melder_assert (thy nx == original -> nx);
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				LPC_Frame toFrame = & thy d_frames [iframe];
				const LPC_Frame fromFrame = & my original -> d_frames [iframe];
				fromFrame -> copy (toFrame);
			}
		}

	#endif

oo_END_CLASS (SoundToLPCRobustAnalysisWorkspace)
#undef ooSTRUCT

/* End of file SoundToLPCAnalysisWorkspace_def.h */
 
