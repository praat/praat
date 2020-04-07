#ifndef _Correlation_h_
#define _Correlation_h_
/* Correlation.h
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

#include "SSCP.h"
#include "TableOfReal_extensions.h"

Thing_define (Correlation, SSCP) {
};

autoCorrelation TableOfReal_to_Correlation (TableOfReal me);

autoCorrelation TableOfReal_to_Correlation_rank (TableOfReal me);

autoCorrelation Correlation_create (integer dimension);

autoCorrelation Correlation_createSimple (conststring32 s_correlations, conststring32 s_centroid, integer numberOfObservations);

autoTableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, integer numberOfTests, int method);
/*
	if (method == 1)
		Confidence intervals by Ruben's approximation
	if (method == 2)
		Obtain large-sample conservative multiple tests and intervals by the
		Bonferroni inequality and the Fisher z transformation.

	Put upper value of confidence intervals in upper matrix and lower
	values of confidence intervals in lower part of resulting table.
	Diagonal values are 1 (and represent both upper and lower c.i.).
*/

void Correlation_testDiagonality_bartlett (Correlation me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df);
/* Test if a Correlation matrix is diagonal, Morrison pp. 116-118 */

autoCorrelation SSCP_to_Correlation (SSCP me);

#endif  /* _Correlation_h_ */
