#ifndef _ICA_h_
#define _ICA_h_
/* ICA.h
 *
 * Copyright (C) 2010-2020 David Weenink, 2015 Paul Boersma
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
  djmw 20101202 Initial version
  djmw 20120406 Latest modification.
*/

#include "Covariance.h"
#include "Sound_and_MixingMatrix.h"


Thing_define (Diagonalizer, TableOfReal) {
};

Thing_define (CrossCorrelationTable, SSCP) {
	void v_info ()
		override;
};

Collection_define (CrossCorrelationTableList, OrderedOf, CrossCorrelationTable) {
	void v_info ()
		override;
};

/*
	Cell [i,j] of a CrossCorrelationTable contains the cross-correlation between signal i and signal j (for one particular lag time).
	For example, the CrossCorrelation of an n-channel sound is a nxn table where cell [i,j] contains the
	cross-correlation of channel i with channel j for a particlular lag time tau.
	In the statistical literature sometimes the cross-correlation between signals is also called "covariance".
	However, the only thing a Covariance has in common with a CrossCorrelationTable is that both are symmetric
	matrices. Differences between a CrossCorrelationTable and a Covariance:
	1. a Covariance matrix is always positive definite, for a cross-correlation table this is only guaranteed for
	  lag time tau = 0.
	2. The elements c[i][j] in a Covariance always satisfy |c[i][j]/sqrt(c[i][i]*c[j][j])| <= 1, this is
	  in general not the case for cross-correlations.
*/

autoCrossCorrelationTable CrossCorrelationTable_create (integer dimension);

autoCrossCorrelationTable CrossCorrelationTable_createSimple (conststring32 covars, conststring32 centroid, integer numberOfSamples);

/* (sum(i,j=1..dimension, i!=j; C[i][j]^2))/(dimension*(dimension-1)) */
double CrossCorrelationTable_getDiagonalityMeasure (CrossCorrelationTable me);

autoCrossCorrelationTable CrossCorrelationTable_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee);

double CrossCorrelationTableList_getDiagonalityMeasure (CrossCorrelationTableList me, double *w, integer start, integer end);

double CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTableList me, Diagonalizer thee, double *w, integer start, integer end);

autoCrossCorrelationTableList CrossCorrelationTableList_createTestSet (integer dimension, integer n, int firstPositiveDefinite, double sigma);

autoDiagonalizer Diagonalizer_create (integer dimension);

autoSound Sound_to_Sound_BSS (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double delta_w, int method);

autoSound Sound_whitenChannels (Sound me, double varianceFraction);
autoSound Sound_Covariance_whitenChannels (Sound me, Covariance thee, double varianceFraction);

void MixingMatrix_CrossCorrelationTableList_improveUnmixing (MixingMatrix me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double tol, int method);

void MixingMatrix_Sound_improveUnmixing (MixingMatrix me, Sound thee,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method);
/*
	Determine the matrix that diagonalizes a series of CrossCorrelationTables as well as possible.
*/
autoDiagonalizer CrossCorrelationTableList_to_Diagonalizer (CrossCorrelationTableList me, integer maxNumberOfIterations, double tol, int method);

void Diagonalizer_CrossCorrelationTableList_improveDiagonality (Diagonalizer me, CrossCorrelationTableList thee,
	integer maxNumberOfIterations, double tol, int method);

autoCrossCorrelationTableList CrossCorrelationTables_to_CrossCorrelationTableList (OrderedOf<structCrossCorrelationTable> *me);
/*
	Determine V*C[k]*V' for k=1..n, where V is the diagonalizer matrix and C[k} the k-th CrossCorrelationTable.
*/
autoCrossCorrelationTableList CrossCorrelationTableList_Diagonalizer_diagonalize (CrossCorrelationTableList me, Diagonalizer thee);


autoDiagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me);

autoMixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me);

/*
	For multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples; (z[i][k] - mean[i])(z[j][k + lag] - mean[j])) / (nsamples - 1).
*/
autoCrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me,
	double startTime, double endTime, double lagStep);

autoCrossCorrelationTable Sounds_to_CrossCorrelationTable_combined (Sound me, Sound thee,
	double relativeStartTime, double relativeEndTime, double lagStep);

// The covariance is the cross-correlation with lag 0.
autoCovariance Sound_to_Covariance_channels (Sound me, double startTime, double endTime);
/*
	Determine a CrossCorrelationTable for lags (k-1)*lagStep, where k = 1...n.
*/
autoCrossCorrelationTableList Sound_to_CrossCorrelationTableList (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep);

autoMixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me);

autoMixingMatrix Sound_to_MixingMatrix (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method);

#endif /*_ICA_h_ */
