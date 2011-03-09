#ifndef _TableOfReal_extensions_h_
#define _TableOfReal_extensions_h_
/* TableOfReal_extensions.h
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
 djmw 20020411 initial GPL
 djmw 20110105 Latest modification.
*/

#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Pattern_h_
	#include "Pattern.h"
#endif
#ifndef _Categories_h_
	#include "Categories.h"
#endif
#ifndef _Strings_h_
	#include "Strings.h"
#endif
#ifndef _SSCP_h_
	#include "SSCP.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

int TableOfReal_to_Pattern_and_Categories(I, long fromrow, long torow, long fromcol, long tocol,
	Pattern *p, Categories *c);

TableOfReal TableOfReal_transpose (I);

Strings TableOfReal_extractRowLabels (I);

Strings TableOfReal_extractColumnLabels (I);

int TableOfReal_and_Categories_setRowLabels (I, Categories thee);
/* !!! Set rowlabels from categories
  Because we need a Table-object with string-columns.
*/

TableOfReal TableOfReal_sortOnlyByRowLabels (I);

long *TableOfReal_getSortedIndexFromRowLabels (I);

TableOfReal TableOfReal_sortRowsByIndex (I, long *index, int reverse);
/*
	thy data[reverse ? i : index[i]][j] = my data[reverse ? index[i] : i]
*/

TableOfReal TableOfReal_createIrisDataset (void);
int TableOfReal_areAllCellsDefined (I, long rb, long re, long cb, long ce);
TableOfReal TableOfReal_createFromPolsData_50males (int include_levels);
TableOfReal TableOfReal_createFromVanNieropData_25females (int include_levels);
TableOfReal TableOfReal_createFromWeeninkData (int option); /* M W C */

void TableOfReal_getColumnExtrema (I, long col, double *min, double *max);

void TableOfReal_drawRowsAsHistogram (I, Graphics g, wchar_t *rows, long colb, long cole,
	double ymin, double ymax, double xoffsetFraction, double interbarFraction,
	double interbarsFraction, wchar_t *greys, int garnish);

void TableOfReal_drawScatterPlot (I, Graphics g, long icx, long icy, long rowb,
	long rowe, double xmin, double xmax, double ymin, double ymax,
	int labelSize, int useRowLabels, wchar_t *label, int garnish);

void TableOfReal_drawScatterPlotMatrix (I, Graphics g, long colb, long cole, double fractionWhite);

void TableOfReal_drawBoxPlots (I, Graphics g, long rowmin, long rowmax, long colmin, long colmax,
	double ymin, double ymax, int garnish);

void TableOfReal_drawVectors (I, Graphics g, long colx1, long coly1,
	long colx2, long coly2, double xmin, double xmax,
	double ymin, double ymax, int vectype, int labelsize, int garnish);

void TableOfReal_drawBiplot (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double sv_splitfactor, int labelsize,
	int garnish);

void TableOfReal_drawColumnAsDistribution (I, Graphics g, int column, double minimum, double maximum, long nBins,
	double freqMin, double freqMax, int cumulative, int garnish);

long TableOfReal_getNumberOfLabelMatches (I, wchar_t *search, int columnLabels,
	int use_regexp);
/*
	Find number of labels that match search description.
*/

int TableOfReal_equalLabels (I, thou, int rowLabels, int columnLabels);
/* return 1 when labels are equal else 0 */

int TableOfReal_copyLabels (I, thou, int rowOrigin, int columnOrigin);
/*
	rowOrigin ==  1 copy from row
	rowOrigin ==  0 do nothing
	rowOrigin == -1 copy from column

	columnOrigin ==  1 copy from column
	columnOrigin ==  0 do nothing
	columnOrigin == -1 copy from row
*/

void TableOfReal_labelsFromCollectionItemNames (I, thou, int row, int column);

int TableOfReal_setSequentialColumnLabels (I, long from, long to,
	wchar_t *precursor, long number, long increment);
int TableOfReal_setSequentialRowLabels (I, long from, long to,
	wchar_t *precursor, long number, long increment);

int TableOfReal_hasRowLabels (I);
int TableOfReal_hasColumnLabels (I);

int TableOfReal_changeRowLabels (I, wchar_t *search, wchar_t *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches,
	int use_regexp);
int TableOfReal_changeColumnLabels (I, wchar_t *search, wchar_t *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches,
	int use_regexp);
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

void TableOfReal_centreColumns (I);
void TableOfReal_centreColumns_byRowLabel (I);
/* PRECONDITION: Table must be sorted by row labels !! */

double TableOfReal_getColumnQuantile (I, long col, double quantile);

double TableOfReal_getRowSum (I, long index);
double TableOfReal_getColumnSum (I, long index);
double TableOfReal_getGrandSum (I);

void TableOfReal_centreRows (I);

void TableOfReal_doubleCentre (I);

int TableOfReal_checkPositive (I);

double TableOfReal_getTableNorm (I);

void TableOfReal_normalizeTable (I, double norm);
void TableOfReal_normalizeColumns (I, double norm);
void TableOfReal_normalizeRows (I, double norm);

void TableOfReal_standardizeColumns (I);
void TableOfReal_standardizeRows (I);

TableOfReal TableOfReal_rankColumns (I);

TableOfReal TableOfReal_meansByRowLabels (I, int expand, int stats);
/*
	stats == 0? averages : medians
	For a table with n rows and m different labels (m <= n):
	if (expand=1) { output has n rows, substitute each array by the average value for that row label}
	else output a table with m rows, the averages for the m labels.
*/

TableOfReal TableOfReal_bootstrap (TableOfReal me);
/* Produce new table with the same number of entries, but randomly
selected with replacement. */
TableOfReal TableOfReal_randomizeRows (TableOfReal me);
/* Produce new table with randomized rows */

/* For the inheritors */
TableOfReal TableOfReal_to_TableOfReal (I);

TableOfReal TableOfReal_choleskyDecomposition (I, int upper, int inverse);

TableOfReal TableOfReal_appendColumns (I, thou);
Any TableOfReal_appendColumnsMany (Collection me);

int TableOfReal_copyOneRowWithLabel (I, thou, long myrow, long thyrow);

double TableOfReal_normalityTest_BHEP (I, double *beta, double *tnb, double *lnmu, double *lnvar);

TableOfReal TableOfReal_and_TableOfReal_crossCorrelations (I, thou, int by_columns, int center, int normalize);

/********************* class TablesOfReal ******************************/


#define TablesOfReal_members Ordered_members
#define TablesOfReal_methods Ordered_methods
class_create (TablesOfReal, Ordered);

int TablesOfReal_init (I, void *klas);

TablesOfReal TablesOfReal_create (void);

TableOfReal TablesOfReal_sum (I);

int TablesOfReal_checkDimensions (I);

#ifdef __cplusplus
	}
#endif

#endif /* _TableOfReal_extensions_h_ */
