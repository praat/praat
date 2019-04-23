#ifndef _PCA_h_
#define _PCA_h_
/* PCA.h
 *
 * Principal Component Analysis
 * 
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/


#include "Matrix.h"
#include "TableOfReal.h"
#include "Configuration.h"
#include "Eigen.h"

#include "PCA_def.h"

autoPCA PCA_create (integer numberOfComponents, integer dimension);

void PCA_setNumberOfObservations (PCA me, integer numberOfObservations);

integer PCA_getNumberOfObservations (PCA me);

autoPCA TableOfReal_to_PCA_byRows (TableOfReal me);

autoEigen PCA_to_Eigen (PCA me);

/* Calculate PCA of M'M */

autoPCA Matrix_to_PCA_byRows (Matrix me);
autoPCA Matrix_to_PCA_byColumns (Matrix me);
/* Calculate PCA of M'M */

void PCA_getEqualityOfEigenvalues (PCA me, integer from, integer to, int conservative, double *out_prob, double *out_chisq, double *out_df);
/* Morrison, Multivariate statistical methods, page 336 */

autoTableOfReal PCA_TableOfReal_to_TableOfReal_projectRows (PCA me, TableOfReal thee, integer numberOfDimensionsToKeep);
autoConfiguration PCA_TableOfReal_to_Configuration (PCA me, TableOfReal thee, integer numberOfDimensions);

autoTableOfReal PCA_TableOfReal_to_TableOfReal_zscores (PCA me, TableOfReal thee, integer numberOfDimensions);

double PCA_TableOfReal_getFractionVariance (PCA me, TableOfReal thee, integer from, integer to);
/*	Get fraction variance of the table projected in the pca-space.
	Shorthand for projecting the Covariance of the TableOfReal on the PCA-space
	and quering the projected Covariance for 'fraction variance'.
*/

autoTableOfReal PCA_Configuration_to_TableOfReal_reconstruct (PCA me, Configuration thee);
/* Reconstruct the original TableOfReal from the PCA and the Configuration */

autoTableOfReal PCA_to_TableOfReal_reconstruct1 (PCA me, conststring32 numstring);

#endif /* _PCA_h_ */

