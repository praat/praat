#ifndef _Sound_and_LPC_h_
#define _Sound_and_LPC_h_
/* Sound_and_LPC.h
 *
 * Copyright (C) 1994-2024 David Weenink
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
 djmw 19971103
 djmw 20020812 GPL header
*/

#include "LPC.h"
#include "SoundAnalysisWorkspace.h"
#include "SVD.h"

Thing_define (LPCAnalysisWorkspace, SoundAnalysisWorkspace) {
	/*
		Instead of creating a different LPCAnalysisWorkspace type for each different analysis,
		we create a generic LPCAnalysisWorkspace that can handle the auto, covar, marple and burg 
		algorithm.
	*/
	autoVEC v1, v2, v3;	// for auto, burg, marple
	autoVEC v4, v5;		// for covar
	double tolerance1, tolerance2; // for marple
	

	void getAutocorrelations (LPCAnalysisWorkspace me, VEC);
	
	virtual void allocateSampledFrames (SoundAnalysisWorkspace me) override {
		Melder_assert (my result != nullptr);
		LPC thee = reinterpret_cast<LPC> (my result);
		for (integer iframe = 1; iframe <= thy nx; iframe ++) {
			const LPC_Frame lpcFrame = & thy d_frames [iframe];
			LPC_Frame_init (lpcFrame, thy maxnCoefficients);
		}
	}
	
	void v1_copy (Daata data_to) const override {
		LPCAnalysisWorkspace thee = reinterpret_cast<LPCAnalysisWorkspace> (data_to);
		structSoundAnalysisWorkspace :: v1_copy (thee);
		thy v1 = copy_VEC (v1.get());
		thy v2 = copy_VEC (v2.get());
		thy v3 = copy_VEC (v3.get());
		thy v4 = copy_VEC (v4.get());
		thy v5 = copy_VEC (v5.get());
		thy tolerance1 = tolerance1;
		thy tolerance2 = tolerance2;
	}
};

void LPCAnalysisWorkspace_init (LPCAnalysisWorkspace me, Sound thee, LPC him, double effectiveAnalysisWidth, kSound_windowShape windowShape);

autoLPCAnalysisWorkspace LPCAnalysisWorkspace_create (Sound thee, LPC him, double effectiveAnalysisWidth, kSound_windowShape windowShape);


Thing_define (LPCRobustAnalysisWorkspace, LPCAnalysisWorkspace) {
	// workSpaces;
	autoVEC workspace_svdCompute, workspace_svdSolve, workspace_huber, workspace_inversefiltering;
	
	LPC original; 	// read-only original
	double k_stdev;	// tol1, tol2 ; tol, tol_svd;
	integer predictionOrder;
	integer iter, itermax, huber_iterations = 5;
	bool wantlocation, wantscale;
	double location, scale;
	autoVEC error;
	autoVEC sampleWeights, coefficients, covariancesw;
	autoMAT covarmatrixw;
	autoSVD svd;
	
	void allocateSampledFrames (SoundAnalysisWorkspace him) override {
		LPCRobustAnalysisWorkspace me = reinterpret_cast<LPCRobustAnalysisWorkspace> (him);
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
	
	void v1_copy (Daata data_to) const override {
		LPCRobustAnalysisWorkspace thee = reinterpret_cast<LPCRobustAnalysisWorkspace> (data_to);
		structLPCAnalysisWorkspace :: v1_copy (thee);
		thy workspace_svdCompute = copy_VEC (workspace_svdCompute.get());
		thy workspace_svdSolve = copy_VEC (workspace_svdSolve.get());
		thy workspace_huber = copy_VEC (workspace_huber.get());
		thy workspace_inversefiltering = copy_VEC (workspace_inversefiltering.get());
		thy original = original;
		thy k_stdev = k_stdev;
		thy predictionOrder = predictionOrder;
		thy iter = iter;
		thy itermax = itermax;
		thy huber_iterations = huber_iterations;
		thy wantlocation = wantlocation;
		thy wantscale = wantscale;
		thy location = location;
		thy scale = scale;
		thy error = copy_VEC (error.get());
		thy sampleWeights = copy_VEC (sampleWeights.get());
		thy coefficients = copy_VEC (coefficients.get());
		thy covariancesw = copy_VEC (covariancesw.get());
		thy covarmatrixw = copy_MAT (covarmatrixw.get());
		thy svd = Data_copy (svd.get()); // TODO maximumCapacity
	}
};

autoLPCRobustAnalysisWorkspace LPCRobustAnalysisWorkspace_create (Sound thee, LPC him, double effectiveAnalysisWidth,
	kSound_windowShape windowShape, LPC original, double k_stdev, integer itermax, double tol, double location, bool wantlocation);


autoLPC Sound_to_LPC_auto (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);
autoLPC Sound_to_LPC_covar (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);
autoLPC Sound_to_LPC_burg (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency);
autoLPC Sound_to_LPC_marple (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency, double tol1, double tol2);
autoLPC Sound_to_LPC_robust (Sound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency,
	double k_stdev,	integer itermax, double tol, bool wantlocation);
autoLPC LPC_and_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth, double preEmphasisFrequency, double k_stdev,
	integer itermax, double tol, bool wantlocation);


void Sound_into_LPC_auto (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency);
void Sound_into_LPC_covar (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency);
void Sound_into_LPC_burg (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency);
void Sound_into_LPC_marple (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency, double tol1, double tol2);
void Sound_into_LPC_robust (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency,
	double k_stdev,	integer itermax, double tol, bool wantlocation);

autoLPC LPC_createEmptyFromAnalysisSpecifications (Sound me, int predictionOrder, double physicalAnalysisWidth, double dt);

void LPCAnalysis_threaded (Sound me, double preEmphasisFrequency, LPCAnalysisWorkspace workspace);
/*
 * Function:
 *	Calculate linear prediction coefficients according to following model:
 *  Minimize E(m) = Sum(n=n0;n=n1; (x [n] + Sum(k=1;k=m; a [k]*x [n-k])))
 * Method:
 *  The minimization is carried out by solving the equations:
 *  Sum(i=1;i=m; a [i]*c [i] [k]) = -c [0] [k] for k=1,2,...,m
 *  where c [i] [k] = Sum(n=n0;n=n1;x [n-i]*x [n-k])
 *  1. Covariance:
 *		n0=m; n1 = N-1;
 *      c [i] [k] is symmetric, positive semi-definite matrix
 *  	Markel&Gray, LP of Speech, page 221;
 *  2. Autocorrelation
 *		signal is zero outside the interval;
 *      n0=-infinity; n1=infinity
 *      c [i] [k] symmetric, positive definite Toeplitz matrix
 *  	Markel&Gray, LP of Speech, page 219;
 * Preconditions:
 *	predictionOrder > 0;
 *  preEmphasisFrequency >= 0;
 *
 * Burg method: see Numerical recipes Chapter 13.
 *
 * Marple method: see Marple, L. (1980), A new autoregressive spectrum analysis
 *		algorithm, IEEE Trans. on ASSP 28, 441-453.
 *	tol1 : stop iteration when E(m) / E(0) < tol1
 *	tol2 : stop iteration when (E(m)-E(m-1)) / E(m-1) < tol2,
 */

autoSound LPC_Sound_filter (LPC me, Sound thee, bool useGain);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter:
		given e & a, determine x;
		x(n) = e(n) - Sum (k=1, m, a(k)x(n-k))
	useGain determines whether the LPC-gain is used in the synthesis.
*/

void LPC_Sound_filterWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time);

autoSound LPC_Sound_filterWithFilterAtTime (LPC me, Sound thee, integer channel, double time);

autoSound LPC_Sound_filterInverse (LPC me, Sound thee);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter inverse:
		given x & a, determine e;
		e(n) = x(n) + Sum (k=1, m, a(k)x(n-k))
*/

autoSound LPC_Sound_filterInverseWithFilterAtTime (LPC me, Sound thee, integer channel, double time);

void LPC_Sound_filterInverseWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time);

/*
	For all LPC analysis
*/
void checkLPCAnalysisParameters_e (double sound_dx, integer sound_nx, double physicalAnalysisWidth, integer predictionOrder);

#endif /* _Sound_and_LPC_h_ */
