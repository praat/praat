#ifndef _TableOfReal_extensions_h_
#define _TableOfReal_extensions_h_
/* TableOfReal_extensions.h
 *
 * Copyright (C) 1993-2012, 2014, 2015 David Weenink
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
 djmw 20020411 initial GPL
 djmw 20120914 Latest modification.
*/

#include "TableOfReal.h"
#include "Collection.h"
#include "Pattern.h"
#include "Categories.h"
#include "Strings_.h"
#include "SSCP.h"

void TableOfReal_to_Pattern_and_Categories(TableOfReal me, long fromrow, long torow, long fromcol, long tocol,
	autoPattern *p, autoCategories *c);

autoTableOfReal TableOfReal_transpose (TableOfReal me);

autoStrings TableOfReal_extractRowLabels (TableOfReal me);

autoStrings TableOfReal_extractColumnLabels (TableOfReal me);

void TableOfReal_and_Categories_setRowLabels (TableOfReal me, Categories thee);
// !!! Set rowlabels from categories because we need a Table-object with string-columns.

autoTableOfReal TableOfReal_sortOnlyByRowLabels (TableOfReal me);

long *TableOfReal_getSortedIndexFromRowLabels (TableOfReal me);

autoTableOfReal TableOfReal_sortRowsByIndex (TableOfReal me, long *index, int reverse);
// thy data[reverse ? i : index[i]][j] = my data[reverse ? index[i] : i]

autoTableOfReal TableOfReal_createIrisDataset ();

int TableOfReal_areAllCellsDefined (TableOfReal me, long rb, long re, long cb, long ce);

autoTableOfReal TableOfReal_createFromPolsData_50males (bool include_levels);

autoTableOfReal TableOfReal_createFromVanNieropData_25females (bool include_levels);

autoTableOfReal TableOfReal_createFromWeeninkData (int option); /* M W C */

void TableOfReal_getColumnExtrema (TableOfReal me, long col, double *min, double *max);

long TableOfReal_getColumnIndexAtMaximumInRow (TableOfReal me, long rowNumber);

const char32 *TableOfReal_getColumnLabelAtMaximumInRow (TableOfReal me, long rowNumber);

void TableOfReal_drawRowsAsHistogram (TableOfReal me, Graphics g, const char32 *rows, long colb, long cole,
	double ymin, double ymax, double xoffsetFraction, double interbarFraction,
	double interbarsFraction, const char32 *greys, int garnish);

void TableOfReal_drawScatterPlot (TableOfReal me, Graphics g, long icx, long icy, long rowb,
	long rowe, double xmin, double xmax, double ymin, double ymax,
	int labelSize, bool useRowLabels, const char32 *label, bool garnish);

void TableOfReal_drawAsSquares_area (TableOfReal me, Graphics g, double zmin, double zmax, double cellSizeFactor, int randomFillOrder, bool garnish);

void TableOfReal_drawScatterPlotMatrix (TableOfReal me, Graphics g, long colb, long cole, double fractionWhite);

void TableOfReal_drawBoxPlots (TableOfReal me, Graphics g, long rowmin, long rowmax, long colmin, long colmax,
	double ymin, double ymax, bool garnish);

void TableOfReal_drawVectors (TableOfReal me, Graphics g, long colx1, long coly1,
	long colx2, long coly2, double xmin, double xmax,
	double ymin, double ymax, int vectype, int labelsize, bool garnish);

void TableOfReal_drawBiplot (TableOfReal me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double sv_splitfactor, int labelsize,
	bool garnish);

void TableOfReal_drawColumnAsDistribution (TableOfReal me, Graphics g, int column, double minimum, double maximum, long nBins,
	double freqMin, double freqMax, bool cumulative, bool garnish);

long TableOfReal_getNumberOfLabelMatches (TableOfReal me, const char32 *search, int columnLabels,
	int use_regexp);
/*
	Find number of labels that match search description.
*/

int TableOfReal_equalLabels (TableOfReal me, TableOfReal thee, int rowLabels, int columnLabels);
/* return 1 when labels are equal else 0 */

void TableOfReal_copyLabels (TableOfReal me, TableOfReal thee, int rowOrigin, int columnOrigin);
/*
	rowOrigin ==  1 copy from row
	rowOrigin ==  0 do nothing
	rowOrigin == -1 copy from column

	columnOrigin ==  1 copy from column
	columnOrigin ==  0 do nothing
	columnOrigin == -1 copy from row
*/

void TableOfReal_labelsFromCollectionItemNames (TableOfReal me, Collection thee, int row, int column);

void TableOfReal_setSequentialColumnLabels (TableOfReal me, long from, long to,
	const char32 *precursor, long number, long increment);

void TableOfReal_setSequentialRowLabels (TableOfReal me, long from, long to,
	const char32 *precursor, long number, long increment);

int TableOfReal_hasRowLabels (TableOfReal me);

int TableOfReal_hasColumnLabels (TableOfReal me);

void TableOfReal_changeRowLabels (TableOfReal me, const char32 *search, const char32 *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp);

void TableOfReal_changeColumnLabels (TableOfReal me, const char32 *search, const char32 *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp);
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
/* PRECONDITION: Table must be sorted by row labels !! */

double TableOfReal_getColumnQuantile (TableOfReal me, long col, double quantile);

double TableOfReal_getRowSumByLabel (TableOfReal me, const char32 *label);

double TableOfReal_getRowSum (TableOfReal me, long index);

double TableOfReal_getColumnSumByLabel (TableOfReal me, const char32 *label);

double TableOfReal_getColumnSum (TableOfReal me, long index);

double TableOfReal_getGrandSum (TableOfReal me);

void TableOfReal_centreRows (TableOfReal me);

void TableOfReal_doubleCentre (TableOfReal me);

int TableOfReal_checkPositive (TableOfReal me);

double TableOfReal_getTableNorm (TableOfReal me);

void TableOfReal_normalizeTable (TableOfReal me, double norm);

void TableOfReal_normalizeColumns (TableOfReal me, double norm);

void TableOfReal_normalizeRows (TableOfReal me, double norm);

void TableOfReal_standardizeColumns (TableOfReal me);

void TableOfReal_standardizeRows (TableOfReal me);

autoTableOfReal TableOfReal_rankColumns (TableOfReal me);

autoTableOfReal TableOfReal_meansByRowLabels (TableOfReal me, int expand, int stats);
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

autoTableOfReal TableOfReal_choleskyDecomposition (TableOfReal me, int upper, int inverse);

autoTableOfReal TableOfReal_appendColumns (TableOfReal me, TableOfReal thee);

void TableOfReal_copyOneRowWithLabel (TableOfReal me, TableOfReal thee, long myrow, long thyrow);

double TableOfReal_normalityTest_BHEP (TableOfReal me, double *beta, double *tnb, double *lnmu, double *lnvar);

autoTableOfReal TableOfReal_and_TableOfReal_crossCorrelations (TableOfReal me, TableOfReal thee, int by_columns, int center, int normalize);

/********************* class TablesOfReal ******************************/

Collection_declare (OrderedOfTableOfReal, OrderedOf, TableOfReal);

Thing_define (TableOfRealList, OrderedOfTableOfReal) {
	structTableOfRealList () {
		our classInfo = classTableOfRealList;
	}
};

autoTableOfReal TablesOfReal_sum (TableOfRealList me);

bool TablesOfReal_haveIdenticalDimensions (TableOfRealList me);

autoTableOfReal TableOfReal_appendColumnsMany (TableOfRealList me);

#endif /* _TableOfReal_extensions_h_ */
