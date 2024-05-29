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


#define ooSTRUCT SoundToRobustLPCAnalysisWorkspace
oo_DEFINE_CLASS (SoundToRobustLPCAnalysisWorkspace, SoundAnalysisWorkspace)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, original) // read-only original
	oo_INTEGER (computeSVDworksize)
	oo_DOUBLE (k_stdev)
	oo_INTEGER (localPredictionOrder) // can change from frame to frame
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

oo_END_CLASS (SoundToRobustLPCAnalysisWorkspace)
#undef ooSTRUCT

/* End of file SoundToLPCAnalysisWorkspace_def.h */
 
