#ifndef _ICA_h_
#define _ICA_h_
/* ICA.h
 *
 * Copyright (C) 2010-2014,2015 David Weenink, 2015 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
  djmw 20101202 Initial version
  djmw 20120406 Latest modification.
*/

#include "SSCP.h"
#include "Sound.h"

Thing_define (MixingMatrix, TableOfReal) {
};

Thing_define (Diagonalizer, TableOfReal) {
};

Thing_define (CrossCorrelationTable, SSCP) {
	void v_info ()
		override;
};

Collection_declare (OrderedOfCrossCorrelationTable, OrderedOf, CrossCorrelationTable);

Thing_define (CrossCorrelationTableList, OrderedOfCrossCorrelationTable) {
	structCrossCorrelationTableList () {
		our classInfo = classCrossCorrelationTableList;
	}
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

autoCrossCorrelationTable CrossCorrelationTable_create (long dimension);

autoCrossCorrelationTable CrossCorrelationTable_createSimple (char32 *covars, char32 *centroid, long numberOfSamples);

/* (sum(i,j=1..dimension, i!=j; C[i][j]^2))/(dimension*(dimension-1)) */
double CrossCorrelationTable_getDiagonalityMeasure (CrossCorrelationTable me);

autoCrossCorrelationTable CrossCorrelationTable_and_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee);

double CrossCorrelationTableList_getDiagonalityMeasure (CrossCorrelationTableList me, double *w, long start, long end);

double CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTableList me, Diagonalizer thee, double *w, long start, long end);

autoCrossCorrelationTableList CrossCorrelationTableList_createTestSet (long dimension, long n, int firstPositiveDefinite, double sigma);

autoMixingMatrix MixingMatrix_create (long numberOfChannels, long numberOfComponents);

autoMixingMatrix MixingMatrix_createSimple (long numberOfChannels, long numberOfComponents, char32 *elements);
void MixingMatrix_initializeRandom (MixingMatrix me);

autoDiagonalizer Diagonalizer_create (long dimension);

autoSound Sound_and_MixingMatrix_mix (Sound me, MixingMatrix thee);

autoSound Sound_and_MixingMatrix_unmix (Sound me, MixingMatrix thee);

autoMixingMatrix Sound_to_MixingMatrix (Sound me, double startTime, double endTime, long ncovars, double lagStep, long maxNumberOfIterations, double delta_w, int method);

autoSound Sound_to_Sound_BSS (Sound me, double startTime, double endTime, long ncovars, double lagStep, long maxNumberOfIterations, double delta_w, int method);

autoSound Sound_whitenChannels (Sound me, double varianceFraction);
autoSound Sound_and_Covariance_whitenChannels (Sound me, Covariance thee, double varianceFraction);

void MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (MixingMatrix me, CrossCorrelationTableList thee, long maxNumberOfIterations, double tol, int method);

/*
	Determine the matrix that diagonalizes a series of CrossCorrelationTables as well as possible.
*/
autoDiagonalizer CrossCorrelationTableList_to_Diagonalizer (CrossCorrelationTableList me, long maxNumberOfIterations, double tol, int method);

void Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (Diagonalizer me, CrossCorrelationTableList thee, long maxNumberOfIterations, double tol, int method);

/*
	Determine V*C[k]*V' for k=1..n, where V is the diagonalizer matrix and C[k} the k-th CrossCorrelationTable.
*/
autoCrossCorrelationTableList CrossCorrelationTableList_and_Diagonalizer_diagonalize (CrossCorrelationTableList me, Diagonalizer thee);


autoDiagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me);

autoMixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me);

/*
	For multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples; (z[i][k] - mean[i])(z[j][k + lag] - mean[j])) / (nsamples - 1).
*/
autoCrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me, double startTime, double endTime, double lagStep);

autoCrossCorrelationTable Sounds_to_CrossCorrelationTable_combined (Sound me, Sound thee, double relativeStartTime, double relativeEndTime, double lagStep);

// The covariance is the cross-correlation with lag 0.
autoCovariance Sound_to_Covariance_channels (Sound me, double startTime, double endTime);
/*
	Determine a CrossCorrelationTable for lags (k-1)*lagStep, where k = 1...n.
*/
autoCrossCorrelationTableList Sound_to_CrossCorrelationTableList (Sound me, double startTime, double endTime, double lagStep, long n);

autoMixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me);

#endif /*_ICA_h_ */
