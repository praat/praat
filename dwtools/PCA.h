#ifndef _PCA_h_
#define _PCA_h_
/* PCA.h
 *
 * Principal Component Analysis
 * 
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/


#ifndef _Data_h_
	#include "Data.h"
#endif

#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif

#ifndef _Configuration_h_
	#include "Configuration.h"
#endif

#ifndef _Eigen_h_
	#include "Eigen.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#define PCA_members Eigen_members \
	long numberOfObservations;			\
	wchar_t **labels;						\
	double *centroid;
#define PCA_methods Eigen_methods
class_create (PCA, Eigen);

PCA PCA_create (long numberOfComponents, long dimension);

void PCA_setNumberOfObservations (PCA me, long numberOfObservations);
long PCA_getNumberOfObservations (PCA me);

PCA TableOfReal_to_PCA (I);
/* Calculate PCA of M'M */

void PCA_getEqualityOfEigenvalues (PCA me, long from, long to, int conservative,
	double *probability, double *chisq, long *ndf);
/* Morrison, Multivariate statistical methods, page 336 */

Configuration PCA_and_TableOfReal_to_Configuration (PCA me, thou, long numberOfDimensions);

double PCA_and_TableOfReal_getFractionVariance (PCA me, thou, long from, long to);
/*	Get fraction variance of the table projected in the pca-space.
	Shorthand for projecting the Covariance of the TableOfReal on the PCA-space
	and quering the projected Covariance for 'fraction variance'.
*/

TableOfReal PCA_and_Configuration_to_TableOfReal_reconstruct (PCA me, thou);
/* Reconstruct the original TableOfReal from the PCA and the Configuration */

TableOfReal PCA_to_TableOfReal_reconstruct1 (PCA me, wchar_t *numstring);

#ifdef __cplusplus
	}
#endif

#endif /* _PCA_h_ */

