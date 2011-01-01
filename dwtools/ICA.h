#ifndef _ICA_h_
#define _ICA_h_
/* ICA.h
 *
 * Copyright (C) 2010 David Weenink
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
*/
#ifndef _SSCP_h_
	#include "SSCP.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#define MixingMatrix_members TableOfReal_members
#define MixingMatrix_methods TableOfReal_methods
class_create (MixingMatrix, TableOfReal);

#define Diagonalizer_members TableOfReal_members
#define Diagonalizer_methods TableOfReal_methods
class_create (Diagonalizer, TableOfReal);

#define CrossCorrelationTable_members SSCP_members
#define CrossCorrelationTable_methods SSCP_methods
class_create (CrossCorrelationTable, SSCP);

#define CrossCorrelationTables_members Ordered_members
#define CrossCorrelationTables_methods Ordered_methods
class_create (CrossCorrelationTables, Ordered);

/*
	Cell [i,j] of a CrossCorrelationTable contains the cross-correlation between signal i and signal j.
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
CrossCorrelationTable CrossCorrelationTable_create (long dimension);
CrossCorrelationTable CrossCorrelationTable_createSimple (wchar_t *covars, wchar_t *centroid, long numberOfSamples);

/* (sum(i,j=1..dimension, i!=j; C[i][j]^2))/(dimension*(dimension-1)) */
double CrossCorrelationTable_getDiagonalitymeasure (CrossCorrelationTable me);
CrossCorrelationTable CrossCorrelationTable_and_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee);

CrossCorrelationTables CrossCorrelationTables_create (void);
double CrossCorrelationTables_getDiagonalityMeasure (CrossCorrelationTables me, double *w, long start, long end);
double CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTables me, Diagonalizer thee, double *w, long start, long end);
CrossCorrelationTables CrossCorrelationTables_createTestSet (long dimension, long n, int firstPositiveDefinite, double sigma);

MixingMatrix MixingMatrix_create (long numberOfChannels, long numberOfComponents);
MixingMatrix MixingMatrix_createSimple (long numberOfChannels, long numberOfComponents, wchar_t *elements);
void MixingMatrix_initializeRandom (MixingMatrix me);

Diagonalizer Diagonalizer_create (long dimension);

Sound Sound_and_MixingMatrix_mix (Sound me, MixingMatrix thee);
Sound Sound_and_MixingMatrix_unmix (Sound me, MixingMatrix thee);

MixingMatrix Sound_to_MixingMatrix (Sound me, double startTime, double endTime, long ncovars, double lagTime, long maxNumberOfIterations, double delta_w, int method);

Sound Sound_to_Sound_BSS (Sound me, double startTime, double endTime, long ncovars, double lagTime, long maxNumberOfIterations, double delta_w, int method);
Sound Sound_and_PCA_to_Sound_pc (Sound me, PCA thee, long numberOfComponents, int whiten);
PCA Sound_to_PCA (Sound me, double startTime, double endTime);

int MixingMatrix_and_CrossCorrelationTables_improveUnmixing (MixingMatrix me, CrossCorrelationTables thee, long maxNumberOfIterations, double tol, int method);

/*
	Determine the matrix that diagonalizes a series of CrossCorrelationTables as well as possible.
*/
Diagonalizer CrossCorrelationTables_to_Diagonalizer (CrossCorrelationTables me, long maxNumberOfIterations, double tol, int method);
int Diagonalizer_and_CrossCorrelationTables_improveDiagonality (Diagonalizer me, CrossCorrelationTables thee, long maxNumberOfIterations, double tol, int method);

/*
	Determine V*C[k]*V' for k=1..n, where V is the diagonalizer matrix and C[k} the k-th CrossCorrelationTable.
*/
CrossCorrelationTables CrossCorrelationTables_and_Diagonalizer_diagonalize (CrossCorrelationTables me, Diagonalizer thee);


Diagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me);
MixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me);

/*
	For multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples; (z[i][k] - mean[i])(z[j][k + lag] - mean[j])) / (nsamples - 1).
*/
CrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me, double startTime, double endTime, double lagTime);

/*
	Determine a CrossCorrelationTable for lags (k-1)*lagTime, where k = 1...n.
*/
CrossCorrelationTables Sound_to_CrossCorrelationTables (Sound me, double startTime, double endTime, double lagTime, long n);

MixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me);

#endif /*_ICA_h_ */
