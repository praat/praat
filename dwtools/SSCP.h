#ifndef _SSCP_h_
#define _SSCP_h_
/* SSCP.h
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "PCA.h"
#include "TableOfReal_extensions.h"

#include "SSCP_def.h"

#include "SSCP_enums.h"

/*
	Ordered collection of SSCP's
	All SSCP's must have the same dimensions and labels.
*/
Collection_define (SSCPList, OrderedOf, SSCP) {
};

void SSCP_init (SSCP me, integer dimension, kSSCPstorage storage);

autoSSCP SSCP_create (integer dimension);

void SSCP_reset (SSCP me);

void SSCP_drawTwoDimensionalEllipse_inside (SSCP me, Graphics g, double scale, conststring32 label, double fontSize);

double SSCP_getEllipseScalefactor (SSCP me, double scale, bool confidence);

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, bool garnish);

void SSCP_setNumberOfObservations (SSCP me, double numberOfObservations);

void SSCP_setCentroid (SSCP me, integer component, double value); // only SSCP & Covariance

void SSCP_setValue (SSCP me, integer row, integer col, double value); // only SSCP & Covariance

double SSCP_getNumberOfObservations (SSCP me);

double SSCP_getDegreesOfFreedom (SSCP me);

double SSCP_getTotalVariance (SSCP me);

double SSCP_getCumulativeContributionOfComponents (SSCP me, integer from, integer to);

double SSCP_getLnDeterminant (SSCP me);

double SSCP_getConcentrationEllipseArea(SSCP me, double scale, bool confidence, integer d1, integer d2);

double SSCP_getFractionVariation (SSCP me, integer from, integer to);

autoSSCP TableOfReal_to_SSCP (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole);
autoSSCP TableOfReal_to_SSCP_rowWeights (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole, integer weightColumnNumber);

autoTableOfReal SSCP_TableOfReal_extractDistanceQuantileRange (SSCP me, TableOfReal thee, double qlow, double qhigh);

autoTableOfReal SSCP_to_TableOfReal (SSCP me);

autoTableOfReal SSCP_extractCentroid (SSCP me);

autoSSCPList TableOfReal_to_SSCPList_byLabel (TableOfReal me);

autoPCA SSCP_to_PCA (SSCP me);

void SSCP_expandPCA (SSCP me);

void SSCP_unExpandPCA (SSCP me);

void SSCP_getDiagonality_bartlett (SSCP me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df);

autoSSCP SSCPList_to_SSCP_sum (SSCPList me);
/* Sum the sscp's and weigh each means with it's numberOfObservations. */

autoSSCP SSCPList_to_SSCP_pool (SSCPList me);

void SSCPList_getHomegeneityOfCovariances_box (SSCPList me, double *out_probability, double *out_chisq, double *out_df);

autoSSCP SSCP_toTwoDimensions (SSCP me, constVECVU const& v1, constVECVU const& v2);

autoSSCPList SSCPList_toTwoDimensions (SSCPList me, constVECVU const& v1, constVECVU const& v2);

autoSSCPList SSCPList_extractTwoDimensions (SSCPList me, integer d1, integer d2);

/* For inheritors */

void SSCPList_drawConcentrationEllipses (SSCPList me, Graphics g, double scale,
	bool confidence, conststring32 label, integer d1, integer d2, double xmin, double xmax,
	double ymin, double ymax, double fontSize, bool garnish);

void SSCPList_getEllipsesBoundingBoxCoordinates (SSCPList me, double scale, bool confidence,
	double *xmin, double *xmax, double *ymin, double *ymax);

void SSCP_expand (SSCP me);
/*
	Expand a reduced storage SSCP. For efficiency reasons, the expanded matrix is kept in memory.
	Successive calls to SSCP_expand don't change anything unless
	Before using one of the Covariance functions defined here on a reduced matrix we
	first have to expand it to normal size.

	Covariance me = Covariance_create_reduceStorage (dimension, kSSCPstorage::Diagonal); // diagonal only
	...
	SSCP_expand (me);
	PCA thee = SSCP_to_PCA (me);
*/

void SSCP_unExpand (SSCP me);
/* Use only if the memory is really needed! */

void SSCP_expandLowerCholeskyInverse (SSCP me); // create lower square root of covariance matrix

void SSCP_unExpandLowerCholesky (SSCP me);

/* End of file SSCP.h */
#endif
