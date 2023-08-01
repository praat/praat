#ifndef _TableOfReal_extensions_h_
#define _TableOfReal_extensions_h_
/* TableOfReal_extensions.h
 *
 * Copyright (C) 1993-2021 David Weenink
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

#include "Categories.h"
#include "Collection.h"
#include "Graphics_extensions.h"
#include "Matrix_extensions.h"
#include "PatternList.h"
#include "Strings_.h"
#include "TableOfReal.h"

void TableOfReal_to_PatternList_and_Categories(TableOfReal me, integer fromrow, integer torow, integer fromcol, integer tocol, autoPatternList *out_p, autoCategories *out_c);

autoTableOfReal TableOfReal_transpose (TableOfReal me);

autoStrings TableOfReal_extractRowLabels (TableOfReal me);

autoStrings TableOfReal_extractColumnLabels (TableOfReal me);

void TableOfReal_Categories_setRowLabels (TableOfReal me, Categories thee);
// !!! Set rowlabels from categories because we need a Table-object with string-columns.

autoTableOfReal TableOfReal_sortOnlyByRowLabels (TableOfReal me);

autoINTVEC TableOfReal_getSortedIndexFromRowLabels (TableOfReal me);

autoTableOfReal TableOfReal_sortRowsByIndex (TableOfReal me, constINTVEC index, bool reverse);
// thy data [reverse ? i : index [i]] [j] = my data [reverse ? index [i] : i]

autoTableOfReal TableOfReal_createIrisDataset ();

autoTableOfReal TableOfReal_create_pols1973 (bool include_levels);

autoTableOfReal TableOfReal_create_vanNierop1973 (bool include_levels);

autoTableOfReal TableOfReal_create_weenink1983 (int option); /* M W C */

/* The data for Fig. 2 in Sandwell (1987) */
autoTableOfReal TableOfReal_create_sandwell1987 ();

integer TableOfReal_getColumnIndexAtMaximumInRow (TableOfReal me, integer rowNumber);

conststring32 TableOfReal_getColumnLabelAtMaximumInRow (TableOfReal me, integer rowNumber);

void TableOfReal_drawRowsAsHistogram (TableOfReal me, Graphics g, constINTVECVU const& rowNumbers, integer colb, integer cole,
	double ymin, double ymax, double xoffsetFraction, double interbarFraction,
	double interbarsFraction, constVECVU const& greys, bool garnish
);

void TableOfReal_drawScatterPlot (TableOfReal me, Graphics g, integer icx, integer icy, integer rowb,
	integer rowe, double xmin, double xmax, double ymin, double ymax,
	integer labelSize, bool useRowLabels, conststring32 label, bool garnish
);

void TableOfReal_drawAsScalableSquares (TableOfReal me, Graphics g, integer rowmin, integer rowmax, integer colmin, integer colmax, kGraphicsMatrixOrigin origin, double cellSizeFactor, kGraphicsMatrixCellDrawingOrder fillOrder, bool garnish);

void TableOfReal_drawScatterPlotMatrix (TableOfReal me, Graphics g, integer colb, integer cole, double fractionWhite);

void TableOfReal_drawBoxPlots (TableOfReal me, Graphics g, integer rowmin, integer rowmax, integer colmin, integer colmax,
	double ymin, double ymax, bool garnish);

void TableOfReal_drawVectors (TableOfReal me, Graphics g, integer colx1, integer coly1,
	integer colx2, integer coly2, double xmin, double xmax,
	double ymin, double ymax, int vectype, int labelsize, bool garnish);

void TableOfReal_drawBiplot (TableOfReal me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double sv_splitfactor, int labelsize,
	bool garnish);

void TableOfReal_drawColumnAsDistribution (TableOfReal me, Graphics g, integer column, double minimum, double maximum, integer nBins,
	double freqMin, double freqMax, bool cumulative, bool garnish);

integer TableOfReal_getNumberOfLabelMatches (TableOfReal me, conststring32 search, bool columnLabels,
	bool use_regexp);
/*
	Find number of labels that match search description.
*/

void TableOfReal_copyLabels (TableOfReal me, TableOfReal thee, int rowOrigin, int columnOrigin);
/*
	rowOrigin ==  1 copy from row
	rowOrigin ==  0 do nothing
	rowOrigin == -1 copy from column

	columnOrigin ==  1 copy from column
	columnOrigin ==  0 do nothing
	columnOrigin == -1 copy from row
*/

void TableOfReal_setLabelsFromCollectionItemNames (TableOfReal me, Collection thee, bool setRowLabels, bool setColumnLabels);

void TableOfReal_setSequentialColumnLabels (TableOfReal me, integer from, integer to, conststring32 precursor, integer number, integer increment);

void TableOfReal_setSequentialRowLabels (TableOfReal me, integer from, integer to, conststring32 precursor, integer number, integer increment);

bool TableOfReal_hasRowLabels (TableOfReal me);

bool TableOfReal_hasColumnLabels (TableOfReal me);

void TableOfReal_changeRowLabels (TableOfReal me, conststring32 search, conststring32 replace,
	integer maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp);

void TableOfReal_changeColumnLabels (TableOfReal me, conststring32 search, conststring32 replace,
	integer maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp);
/*
	Change all row/column labels. The 'search' and 'replace' string are
	interpreted as regular expressions when 'use_regexp' != 0.
	'maximumNumberOfReplaces' is the maximum number of replaces in EACH string
	in the array of strings (you can replace ALL occurrences by making this
	number <= 0)
	The totalnumber of matches found is returned in 'nmatches'.
	The number of strings with at least one match is returned in
	'nstringmatches'.
*/

void TableOfReal_centreColumns (TableOfReal me);

void TableOfReal_centreColumns_byRowLabel (TableOfReal me);
/* PRECONDITION: Table should be sorted by row labels !! */

double TableOfReal_getColumnQuantile (TableOfReal me, integer col, double quantile);

double TableOfReal_getRowSumByLabel (TableOfReal me, conststring32 label);

double TableOfReal_getRowSum (TableOfReal me, integer index);

double TableOfReal_getColumnSumByLabel (TableOfReal me, conststring32 label);

double TableOfReal_getColumnSum (TableOfReal me, integer index);

double TableOfReal_getGrandSum (TableOfReal me);

void TableOfReal_centreRows (TableOfReal me);

void TableOfReal_doubleCentre (TableOfReal me);

bool TableOfReal_isNonNegative (TableOfReal me);

double TableOfReal_getTableNorm (TableOfReal me);

void TableOfReal_normalizeTable (TableOfReal me, double norm);

void TableOfReal_normalizeColumns (TableOfReal me, double norm);

void TableOfReal_normalizeRows (TableOfReal me, double norm);

void TableOfReal_standardizeColumns (TableOfReal me);

void TableOfReal_standardizeRows (TableOfReal me);

autoTableOfReal TableOfReal_rankColumns (TableOfReal me, integer fromColumn, integer toColumn);

autoTableOfReal TableOfReal_meansByRowLabels (TableOfReal me, bool expand, bool useMedians);
/*
	stats == 0? averages : medians
	For a table with n rows and m different labels (m <= n):
	if (expand=1) { output has n rows, substitute each array by the average value for that row label}
	else output a table with m rows, the averages for the m labels.
*/

autoTableOfReal TableOfReal_bootstrap (TableOfReal me);
// Produce new table with the same number of entries, but randomly selected with replacement.

autoTableOfReal TableOfReal_randomizeRows (TableOfReal me);
/* Produce new table with randomized rows */

/* For the inheritors */
autoTableOfReal TableOfReal_to_TableOfReal (TableOfReal me);

autoTableOfReal TableOfReal_choleskyDecomposition (TableOfReal me, bool upper, bool inverse);

autoTableOfReal TableOfReal_appendColumns (TableOfReal me, TableOfReal thee);

void TableOfReal_copyOneRowWithLabel (TableOfReal me, TableOfReal thee, integer myrow, integer thyrow);

double TableOfReal_normalityTest_BHEP (TableOfReal me, double *h, double *out_tnb, double *out_lnmu, double *out_lnvar, bool *out_singularCovariance);
/*
	Henze & Wagner (1997), A new approach to the BHEP tests for multivariate normality, Journal of Multivariate Analysis 62, 1-23.
*/

double TableOfReal_testSphericityOfCovariance (TableOfReal me, integer numberOfPermutations, bool useCorrelation);
/*
	Sphericity test of covariance (correlation) matrix
	This test is a combination of two tests:
	1. H0: Sigma = sigma^2 I vs. H1: Sigma = diag (sigma[1]^2, sigma-2[^2, ... sigma[p]^2)
	2. H0: Sigma = diag (sigma[1]^2, sigma-2[^2, ... sigma[p]^2) vs. H1: Sigma != diag (sigma[1]^2, sigma-2[^2, ... sigma[p]^2)
	This is test 1 in:
	L. Wu, C. Weng, X. Wang, K. Wang & X. Liu (2018): Test of covariance and correlation matrices,
	arXiv: 1812.01172v1 [start.ME] 4 Dec 2018.
*/

double TableOfReal_testCovarianceEqualsIdentityMatrix (TableOfReal me, integer numberOfPermutations, bool useCorrelation);
/*
	Test if the covariance or correlation equals the identity matrix I.
	H0: Sigma = I vs H1: Sigma != I
	This is test2 in:
		L. Wu, C. Weng, X. Wang, K. Wang & X. Liu (2018): Test of covariance and correlation matrices,
		arXiv: 1812.01172v1 [start.ME] 4 Dec 2018.
*/	

double TableOfReal_testCovarianceCompoundSymmetry (TableOfReal me, integer numberOfPermutations, bool useCorrelation);
/*
	H0: Sigma = sigma^2 (1-rho) * I + rho * J,
	where I is identity matrix and J is square matrix of ones znd rho > 0 (intra-class correlation coefficient
	This is test3 in:
		L. Wu, C. Weng, X. Wang, K. Wang & X. Liu (2018): Test of covariance and correlation matrices,
		arXiv: 1812.01172v1 [start.ME] 4 Dec 2018.

*/

double TableOfReal_testEqualityOfCovariances (TableOfReal me, TableOfReal thee, integer numberOfPermutations, bool useCorrelation);
/*
	Test the equality of two covariance or correlation matrices.
	H0: Sigma[1] = Sigma[2] vs H1: Sigma[1] != Sigma[2]
	This is test4 in:
		L. Wu, C. Weng, X. Wang, K. Wang & X. Liu (2018): Test of covariance and correlation matrices,
		arXiv: 1812.01172v1 [start.ME] 4 Dec 2018.
*/	

autoTableOfReal TableOfReal_TableOfReal_crossCorrelations (TableOfReal me, TableOfReal thee, bool by_columns, bool center, bool normalize);


#pragma mark - class TableOfRealList

autoTableOfReal TableOfRealList_sum (TableOfRealList me);

bool TableOfRealList_haveIdenticalDimensions (TableOfRealList me);

autoTableOfReal TableOfRealList_appendColumnsMany (TableOfRealList me);

autoMatrix TableOfReal_to_Matrix_interpolateOnRectangularGrid (TableOfReal me, double xmin, double xmax, double nx, double ymin, double ymax, integer ny, int method);

#endif /* _TableOfReal_extensions_h_ */
