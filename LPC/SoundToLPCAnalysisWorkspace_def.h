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
	
	oo_OBJECT (Daata, 0, extraData)  // e.g. for robust analysis
	
	#if oo_DECLARING
		
		virtual void getAutocorrelations (SoundToLPCAnalysisWorkspace /* me */, VEC) {};
		
	#endif

oo_END_CLASS (SoundToLPCAnalysisWorkspace)
#undef ooSTRUCT

#define ooSTRUCT RobustAnalysisExtraData
oo_DEFINE_CLASS (RobustAnalysisExtraData, Daata)

	oo_INTEGER (inverseFilteringCapacity)
	oo_VEC (work_inverseFiltering, inverseFilteringCapacity)
	oo_INTEGER (computeSVDCapacity)
	oo_VEC (work_computeSVD, computeSVDCapacity)
	oo_INTEGER (svdSolveCapacity)
	oo_VEC (work_svdSolve, svdSolveCapacity)
	oo_INTEGER (huberCapacity)
	oo_VEC (work_huber, huberCapacity)
	
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, original) // read-only original
	
	oo_DOUBLE (k_stdev)
	oo_INTEGER (localPredictionOrder) // can change from frame to frame
	oo_INTEGER (iter)
	oo_INTEGER (itermax)
	oo_INTEGER (huber_iterations) // = 5;
	oo_BOOLEAN (wantlocation)
	oo_BOOLEAN (wantscale)
	oo_DOUBLE (location)
	oo_DOUBLE (scale)
	oo_INTEGER (analysisFrameSize)
	oo_VEC (error, analysisFrameSize)
	oo_VEC (sampleWeights, analysisFrameSize)
	oo_VEC (coefficients, original -> maxnCoefficients)
	oo_VEC (covariancesw, original -> maxnCoefficients)
	oo_MAT (covarmatrixw, original -> maxnCoefficients, original -> maxnCoefficients)
	oo_OBJECT (SVD, 1, svd)

oo_END_CLASS (RobustAnalysisExtraData)
#undef ooSTRUCT

/* End of file SoundToLPCAnalysisWorkspace_def.h */
 
