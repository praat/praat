#ifndef _CCA_h_
#define _CCA_h_
/* CCA.h
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

/*
 djmw 2001
 djmw 20020423 GPL header
 djmw 20110306 Latest modification.
*/

#include "Eigen.h"
#include "SSCP.h"
#include "TableOfReal.h"

#include "CCA_def.h"

/*
	Class CCA represents the Canonical Correlation Analysis of two datasets
	(two tables with multivariate data, Table 1 was N rows x p columns,
	Table 2 was N rows x q columns, and p <= q).

	Interpretation:

	The eigenvectors v1[i] en v2[i] have the property that for the linear
	compounds

	c1[1] = v1[1]' . Table1   c2[1]= v2[1]' . Table2
	..............................................
	c1[p] = v1[p]' . Table1   c2[p]= v2[p]' . Table2

	the sample correlation of c1[1] and c2[1] is greatest, the sample
	correlation of c1[2] and c2[2] is greatest amoung all linear compounds
	uncorrelated with c1[1] and c2[1], and so on, for all p possible pairs.
*/

autoCCA CCA_create (integer numberOfCoefficients, integer ny, integer nx);

void CCA_drawEigenvector (CCA me, Graphics g, int x_or_y, integer ivec, integer first, integer last,
	double ymin, double ymax, int weigh, double size_mm, conststring32 mark, int connect, bool garnish);

double CCA_getEigenvectorElement (CCA me, int x_or_y, integer ivec, integer element);

autoCCA TableOfReal_to_CCA (TableOfReal me, integer ny);
/*
	Solves the canonical correlation analysis equations:

	(S12*inv(S22)*S12' - lambda S11)X1 = 0 (1)
	(S12'*inv(S11)*S12 - lambda S22)X2 = 0 (2)

	Where S12 = T1' * T2, S11 = T1' * T1 and S22 = T2' * T2.
	Given the following svd's:

  	svd (T1) = U1 D1 V1'
    svd (T2) = U2 D2 V2'

	We can write down:

	inv(S11) = V1 * D1^-2 * V1' and inv(S22) = V2 * D2^-2 * V2',
	and S12*inv(S22)*S12' simplifies to: V1*D1*U1'*U2 * U2'*U1*D1*V1'

	and (1) becomes:

	(V1*D1*U1'*U2 * U2'*U1*D1*V1' -lambda V1*D1 * D1*V1')X1 = 0

	This can be written as:

	(V1*D1*U1'*U2 * U2'*U1 -lambda V1*D1) D1*V1'*X1 = 0

	multiplying from the left with: D1^-1*V1' results in

	(U1'*U2 * U2'*U1 -lambda) D1*V1'*X1 = 0

	Taking the svd(U2'*U1) = U D V' we get:

	(D^2 -lambda)V'*D1*V1'*X1 = 0

	The eigenvectors X1 can be formally written as:

	X1 = V1*inv(D1)*V

	Equation (2) results in:

	X2 = V2*inv(D2)*U
*/

autoTableOfReal CCA_TableOfReal_scores (CCA me, TableOfReal thee, integer numberOfFactors);
/*
	Return the factors in a table with 2*numberOfFactors columns.
	The first 'numberOfFactors' columns are the scores for the dependent part
	of the table the following 'numberOfFactors' columns are for the
	independent part.
*/

autoTableOfReal CCA_TableOfReal_factorLoadings (CCA me, TableOfReal thee);
/*
	Get the canonical factor loadings (also structure correlation coefficients),
	the correlation of a canonical variable with an original variable.
*/

double CCA_getCorrelationCoefficient (CCA me, integer index);

void CCA_getZeroCorrelationProbability (CCA me, integer index, double *out_prob, double *out_chisq, double *out_df);

autoTableOfReal CCA_TableOfReal_predict (CCA me, TableOfReal thee, integer from);
/*
	Given independent table, predict the dependent one, on the basis of
	the canonical correlations.
*/

autoCCA SSCP_to_CCA (SSCP me, integer ny);

#endif /* CCA.h */
