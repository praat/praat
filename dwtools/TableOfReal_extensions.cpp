/* TableOfReal_extensions.cpp
 *
 * Copyright (C) 1993-2012, 2014, 2015, 2017 David Weenink, 2017 Paul Boersma
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
 djmw 20000202 17 typos in F1-3,L1-3 table corrected
 djmw 20030707 Changed TableOfReal_drawVectors interface.
 djmw 20031030 Added TableOfReal_appendColumns
 djmw 20031216 Interface change in TableOfReal_choleskyDecomposition
 djmw 20040108 Corrected a memory leak in TableOfReal_drawBoxPlots
 djmw 20040211 Modified TableOfReal_copyLabels behaviour: copy NULL-labels too.
 djmw 20040511 Removed TableOfReal_extractRowsByRowLabel,TableOfReal_selectColumnsWhereRow
 djmw 20040617 Removed column selection bug in TableOfReal_drawRowsAsHistogram
 djmw 20041105 Added TableOfReal_createFromVanNieropData_25females
 djmw 20041115 TableOfReal_drawScatterPlot: plotting a NULL-label crashed Praat.
 djmw 20041213 Added TableOfReal_createFromWeeninkData.
 djmw 20050221 TableOfReal_meansByRowLabels, extra reduce parameter.
 djmw 20050222 TableOfReal_drawVectors didn't draw rowlabels.
 djmw 20050512 TableOfReal TableOfReal_meansByRowLabels crashed if first label in sorted was NULL.
 djmw 20051116 TableOfReal_drawScatterPlot draw reverse permited by choosing xmin > xmax and/or ymin>ymax
 djmw 20060301 TableOfReal_meansByRowLabels extra medianize
 djmw 20060626 Extra NULL argument for ExecRE.
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20070822 wchar
 djmw 20070902 Better error messages (object type and name feedback)
 djmw 20070614 updated to version 1.30 of regular expressions.
 djmw 20071202 Melder_warning<n>
 djmw 20080122 float -> double
 djmw 20081119 +TableOfReal_areAllCellsDefined
 djmw 20090506 +setInner for _drawScatterPlotMatrix
 djmw 20091009 +TableOfReal_drawColumnAsDistribution
 djmw 20100222 Corrected a bug in TableOfReal_copyOneRowWithLabel which caused label corruption if
               from and to table were equal and rows were equal too.
 djmw 20111110 Use autostringvector
 djmw 20111123 Always use Melder_wcscmp
*/

#include <ctype.h>
#include "Graphics_extensions.h"
#include "SSCP.h"
#include "Matrix_extensions.h"
#include "NUMclapack.h"
#include "NUM2.h"
#include "SVD.h"
#include "Table_extensions.h"
#include "TableOfReal_extensions.h"
#include "TableOfReal_and_Permutation.h"
#include "regularExp.h"
#include "Formula.h"

#define EMPTY_STRING(s) (! (s) || s[0] == '\0')
#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

#define Graphics_ARROW 1
#define Graphics_TWOWAYARROW 2
#define Graphics_LINE 3

static autoTableOfReal TableOfReal_and_TableOfReal_columnCorrelations (TableOfReal me, TableOfReal thee, int center, int normalize);
static autoTableOfReal TableOfReal_and_TableOfReal_rowCorrelations (TableOfReal me, TableOfReal thee, int center, int normalize);

long TableOfReal_getColumnIndexAtMaximumInRow (TableOfReal me, long rowNumber) {
	long columnNumber = 0;
	if (rowNumber > 0 && rowNumber <= my numberOfRows) {
		double max = my data [rowNumber] [1];
		columnNumber = 1;
		for (long icol = 2; icol <= my numberOfColumns; icol ++) {
			if (my data [rowNumber] [icol] > max) {
				max = my data [rowNumber] [icol];
				columnNumber = icol;
			}
		}
	}
	return columnNumber;
}

const char32 *TableOfReal_getColumnLabelAtMaximumInRow (TableOfReal me, long rowNumber) {
	long columnNumber = TableOfReal_getColumnIndexAtMaximumInRow (me, rowNumber);
	return my v_getColStr (columnNumber);
}

void TableOfReal_copyOneRowWithLabel (TableOfReal me, TableOfReal thee, long myrow, long thyrow) {
	try {
		if (me == thee && myrow == thyrow) {
			return;
		}

		if (myrow < 1 ||  myrow > my  numberOfRows || thyrow < 1 || thyrow > thy numberOfRows || my numberOfColumns != thy numberOfColumns) {
			Melder_throw (U"The dimensions do not fit.");
		}

		Melder_free (thy rowLabels[thyrow]);
		thy rowLabels[thyrow] = Melder_dup (my rowLabels[myrow]);

		if (my data[myrow] != thy data[thyrow]) {
			NUMvector_copyElements (my data[myrow], thy data[thyrow], 1, my numberOfColumns);
		}
	} catch (MelderError) {
		Melder_throw (me, U": row ", myrow, U" not copied to ", thee);
	}
}

int TableOfReal_hasRowLabels (TableOfReal me) {
	if (! my rowLabels) {
		return 0;
	}
	for (long i = 1; i <= my numberOfRows; i++) {
		if (EMPTY_STRING (my rowLabels[i])) {
			return 0;
		}
	}
	return 1;
}

int TableOfReal_hasColumnLabels (TableOfReal me) {
	if (! my columnLabels) {
		return 0;
	}
	for (long i = 1; i <= my numberOfColumns; i++) {
		if (EMPTY_STRING (my columnLabels[i])) {
			return 0;
		}
	}
	return 1;
}

autoTableOfReal TableOfReal_createIrisDataset () {
	double iris[150][4] = {
		{5.1, 3.5, 1.4, 0.2}, {4.9, 3.0, 1.4, 0.2}, {4.7, 3.2, 1.3, 0.2}, {4.6, 3.1, 1.5, 0.2}, {5.0, 3.6, 1.4, 0.2},
		{5.4, 3.9, 1.7, 0.4}, {4.6, 3.4, 1.4, 0.3}, {5.0, 3.4, 1.5, 0.2}, {4.4, 2.9, 1.4, 0.2}, {4.9, 3.1, 1.5, 0.1},
		{5.4, 3.7, 1.5, 0.2}, {4.8, 3.4, 1.6, 0.2}, {4.8, 3.0, 1.4, 0.1}, {4.3, 3.0, 1.1, 0.1}, {5.8, 4.0, 1.2, 0.2},
		{5.7, 4.4, 1.5, 0.4}, {5.4, 3.9, 1.3, 0.4}, {5.1, 3.5, 1.4, 0.3}, {5.7, 3.8, 1.7, 0.3}, {5.1, 3.8, 1.5, 0.3},
		{5.4, 3.4, 1.7, 0.2}, {5.1, 3.7, 1.5, 0.4}, {4.6, 3.6, 1.0, 0.2}, {5.1, 3.3, 1.7, 0.5}, {4.8, 3.4, 1.9, 0.2},
		{5.0, 3.0, 1.6, 0.2}, {5.0, 3.4, 1.6, 0.4}, {5.2, 3.5, 1.5, 0.2}, {5.2, 3.4, 1.4, 0.2}, {4.7, 3.2, 1.6, 0.2},
		{4.8, 3.1, 1.6, 0.2}, {5.4, 3.4, 1.5, 0.4}, {5.2, 4.1, 1.5, 0.1}, {5.5, 4.2, 1.4, 0.2}, {4.9, 3.1, 1.5, 0.2},
		{5.0, 3.2, 1.2, 0.2}, {5.5, 3.5, 1.3, 0.2}, {4.9, 3.6, 1.4, 0.1}, {4.4, 3.0, 1.3, 0.2}, {5.1, 3.4, 1.5, 0.2},
		{5.0, 3.5, 1.3, 0.3}, {4.5, 2.3, 1.3, 0.3}, {4.4, 3.2, 1.3, 0.2}, {5.0, 3.5, 1.6, 0.6}, {5.1, 3.8, 1.9, 0.4},
		{4.8, 3.0, 1.4, 0.3}, {5.1, 3.8, 1.6, 0.2}, {4.6, 3.2, 1.4, 0.2}, {5.3, 3.7, 1.5, 0.2}, {5.0, 3.3, 1.4, 0.2},
		{7.0, 3.2, 4.7, 1.4}, {6.4, 3.2, 4.5, 1.5}, {6.9, 3.1, 4.9, 1.5}, {5.5, 2.3, 4.0, 1.3}, {6.5, 2.8, 4.6, 1.5},
		{5.7, 2.8, 4.5, 1.3}, {6.3, 3.3, 4.7, 1.6}, {4.9, 2.4, 3.3, 1.0}, {6.6, 2.9, 4.6, 1.3}, {5.2, 2.7, 3.9, 1.4},
		{5.0, 2.0, 3.5, 1.0}, {5.9, 3.0, 4.2, 1.5}, {6.0, 2.2, 4.0, 1.0}, {6.1, 2.9, 4.7, 1.4}, {5.6, 2.9, 3.6, 1.3},
		{6.7, 3.1, 4.4, 1.4}, {5.6, 3.0, 4.5, 1.5}, {5.8, 2.7, 4.1, 1.0}, {6.2, 2.2, 4.5, 1.5}, {5.6, 2.5, 3.9, 1.1},
		{5.9, 3.2, 4.8, 1.8}, {6.1, 2.8, 4.0, 1.3}, {6.3, 2.5, 4.9, 1.5}, {6.1, 2.8, 4.7, 1.2}, {6.4, 2.9, 4.3, 1.3},
		{6.6, 3.0, 4.4, 1.4}, {6.8, 2.8, 4.8, 1.4}, {6.7, 3.0, 5.0, 1.7}, {6.0, 2.9, 4.5, 1.5}, {5.7, 2.6, 3.5, 1.0},
		{5.5, 2.4, 3.8, 1.1}, {5.5, 2.4, 3.7, 1.0}, {5.8, 2.7, 3.9, 1.2}, {6.0, 2.7, 5.1, 1.6}, {5.4, 3.0, 4.5, 1.5},
		{6.0, 3.4, 4.5, 1.6}, {6.7, 3.1, 4.7, 1.5}, {6.3, 2.3, 4.4, 1.3}, {5.6, 3.0, 4.1, 1.3}, {5.5, 2.5, 4.0, 1.3},
		{5.5, 2.6, 4.4, 1.2}, {6.1, 3.0, 4.6, 1.4}, {5.8, 2.6, 4.0, 1.2}, {5.0, 2.3, 3.3, 1.0}, {5.6, 2.7, 4.2, 1.3},
		{5.7, 3.0, 4.2, 1.2}, {5.7, 2.9, 4.2, 1.3}, {6.2, 2.9, 4.3, 1.3}, {5.1, 2.5, 3.0, 1.1}, {5.7, 2.8, 4.1, 1.3},
		{6.3, 3.3, 6.0, 2.5}, {5.8, 2.7, 5.1, 1.9}, {7.1, 3.0, 5.9, 2.1}, {6.3, 2.9, 5.6, 1.8}, {6.5, 3.0, 5.8, 2.2},
		{7.6, 3.0, 6.6, 2.1}, {4.9, 2.5, 4.5, 1.7}, {7.3, 2.9, 6.3, 1.8}, {6.7, 2.5, 5.8, 1.8}, {7.2, 3.6, 6.1, 2.5},
		{6.5, 3.2, 5.1, 2.0}, {6.4, 2.7, 5.3, 1.9}, {6.8, 3.0, 5.5, 2.1}, {5.7, 2.5, 5.0, 2.0}, {5.8, 2.8, 5.1, 2.4},
		{6.4, 3.2, 5.3, 2.3}, {6.5, 3.0, 5.5, 1.8}, {7.7, 3.8, 6.7, 2.2}, {7.7, 2.6, 6.9, 2.3}, {6.0, 2.2, 5.0, 1.5},
		{6.9, 3.2, 5.7, 2.3}, {5.6, 2.8, 4.9, 2.0}, {7.7, 2.8, 6.7, 2.0}, {6.3, 2.7, 4.9, 1.8}, {6.7, 3.3, 5.7, 2.1},
		{7.2, 3.2, 6.0, 1.8}, {6.2, 2.8, 4.8, 1.8}, {6.1, 3.0, 4.9, 1.8}, {6.4, 2.8, 5.6, 2.1}, {7.2, 3.0, 5.8, 1.6},
		{7.4, 2.8, 6.1, 1.9}, {7.9, 3.8, 6.4, 2.0}, {6.4, 2.8, 5.6, 2.2}, {6.3, 2.8, 5.1, 1.5}, {6.1, 2.6, 5.6, 1.4},
		{7.7, 3.0, 6.1, 2.3}, {6.3, 3.4, 5.6, 2.4}, {6.4, 3.1, 5.5, 1.8}, {6.0, 3.0, 4.8, 1.8}, {6.9, 3.1, 5.4, 2.1},
		{6.7, 3.1, 5.6, 2.4}, {6.9, 3.1, 5.1, 2.3}, {5.8, 2.7, 5.1, 1.9}, {6.8, 3.2, 5.9, 2.3}, {6.7, 3.3, 5.7, 2.5},
		{6.7, 3.0, 5.2, 2.3}, {6.3, 2.5, 5.0, 1.9}, {6.5, 3.0, 5.2, 2.0}, {6.2, 3.4, 5.4, 2.3}, {5.9, 3.0, 5.1, 1.8}
	};

	try {
		autoTableOfReal me = TableOfReal_create (150, 4);

		TableOfReal_setColumnLabel (me.get(), 1, U"sl");
		TableOfReal_setColumnLabel (me.get(), 2, U"sw");
		TableOfReal_setColumnLabel (me.get(), 3, U"pl");
		TableOfReal_setColumnLabel (me.get(), 4, U"pw");
		for (long i = 1; i <= 150; i++) {
			int kind = (i - 1) / 50 + 1;
			char32 const *label = kind == 1 ? U"1" : kind == 2 ? U"2" : U"3";
			for (long j = 1; j <= 4; j++) {
				my data[i][j] = iris[i - 1][j - 1];
			}
			TableOfReal_setRowLabel (me.get(), i, label);
		}
		Thing_setName (me.get(), U"iris");
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal from iris data not created.");
	}
}

autoStrings TableOfReal_extractRowLabels (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);

		if (my numberOfRows > 0) {
			thy strings = NUMvector<char32 *> (1, my numberOfRows);

			thy numberOfStrings = my numberOfRows;

			for (long i = 1; i <= my numberOfRows; i++) {
				const char32 *label = my rowLabels[i] ? my rowLabels[i] : U"?";
				thy strings[i] = Melder_dup (label);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": row labels not extracted.");
	}
}

autoStrings TableOfReal_extractColumnLabels (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);

		if (my numberOfColumns > 0) {
			thy strings = NUMvector<char32 *> (1, my numberOfColumns);
			thy numberOfStrings = my numberOfColumns;

			for (long i = 1; i <= my numberOfColumns; i++) {
				char32 const *label = my columnLabels[i] ? my columnLabels[i] : U"?";
				thy strings[i] = Melder_dup (label);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": column labels not extracted.");
	}
}

autoTableOfReal TableOfReal_transpose (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfColumns, my numberOfRows);

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				thy data[j][i] = my data[i][j];
			}
		}
		NUMstrings_copyElements (my rowLabels, thy columnLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, thy rowLabels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not transposed.");
	}
}

void TableOfReal_to_PatternList_and_Categories (TableOfReal me, long fromrow, long torow, long fromcol, long tocol,
	autoPatternList *p, autoCategories *c)
{
	try {
		long ncol = my numberOfColumns, nrow = my numberOfRows;

		if (fromrow == torow && fromrow == 0) {
			fromrow = 1; torow = nrow;
		} else if (fromrow > 0 && fromrow <= nrow && torow == 0) {
			torow = nrow;
		} else if (! (fromrow > 0 && torow <= nrow && fromrow <= torow)) {
			Melder_throw (U"Invalid row selection.");
		}
		if (fromcol == tocol && fromcol == 0) {
			fromcol = 1; tocol = ncol;
		} else if (fromcol > 0 && fromcol <= ncol && tocol == 0) {
			tocol = ncol;
		} else if (! (fromcol > 0 && tocol <= ncol && fromcol <= tocol)) {
			Melder_throw (U"Invalid column selection.");
		}

		nrow = torow - fromrow + 1;
		ncol = tocol - fromcol + 1;
		autoPatternList ap = PatternList_create (nrow, ncol);
		autoCategories ac = Categories_create ();

		long row = 1;
		for (long i = fromrow; i <= torow; i ++, row ++) {
			char32 const *s = my rowLabels [i] ? my rowLabels [i] : U"?";
			autoSimpleString item = SimpleString_create (s);
			ac -> addItem_move (item.move());
			long col = 1;
			for (long j = fromcol; j <= tocol; j ++, col ++) {
				ap -> z [row] [col] = my data [i] [j];
			}
		}
		*p = ap.move();
		*c = ac.move();
	} catch (MelderError) {
		Melder_throw (U"PatternList and Categories not created from TableOfReal.");
	}
}

void TableOfReal_getColumnExtrema (TableOfReal me, long col, double *p_min, double *p_max) {
	double min = undefined, max = undefined;
	if (col < 1 || col > my numberOfColumns) {
		Melder_throw (U"Invalid column number.");
	}
	min = max = my data[1][col];
	for (long i = 2; i <= my numberOfRows; i++) {
		if (my data[i][col] > max) {
			max = my data[i][col];
		} else if (my data[i][col] < min) {
			min = my data[i][col];
		}
	}
	if (p_min) {
		*p_min = min;
	}
	if (p_max) {
		*p_max = max;
	}
}

void TableOfReal_drawRowsAsHistogram (TableOfReal me, Graphics g, const char32 *rows, long colb, long cole, double ymin,
	double ymax, double xoffsetFraction, double interbarFraction, double interbarsFraction, const char32 *greys, bool garnish)
{
	if (colb >= cole) {
		colb = 1; cole = my numberOfColumns;
	}
	if (colb <= cole && (colb < 1 || cole > my numberOfColumns)) {
		Melder_throw (U"Invalid columns");
	}

	long nrows;
	autoNUMvector<double> irows (NUMstring_to_numbers (rows, &nrows), 1);
	for (long i = 1; i <= nrows; i++) {
		long irow = (long) floor (irows[i]);
		if (irow < 0 || irow > my numberOfRows) {
			Melder_throw (U"Invalid row (", irow, U").");
		}
		if (ymin >= ymax) {
			double min, max;
			NUMvector_extrema (my data[irow], colb, cole, &min, &max);
			if (i > 1) {
				if (min < ymin) {
					ymin = min;
				}
				if (max > ymax) {
					ymax = max;
				}
			} else {
				ymin = min; ymax = max;
			}
		}
	}
	long ngreys;
	autoNUMvector<double> igreys (NUMstring_to_numbers (greys, &ngreys), 1);

	Graphics_setWindow (g, 0.0, 1.0, ymin, ymax);
	Graphics_setInner (g);

	long ncols = cole - colb + 1;
	double bar_width = 1.0 / (ncols * nrows + 2.0 * xoffsetFraction + (ncols - 1) * interbarsFraction + ncols * (nrows - 1) * interbarFraction);
	double dx = (interbarsFraction + nrows + (nrows - 1) * interbarFraction) * bar_width;

	for (long i = 1; i <= nrows; i++) {
		long irow = (long) floor (irows[i]);
		double xb = xoffsetFraction * bar_width + (i - 1) * (1.0 + interbarFraction) * bar_width;

		double x1 = xb;
		double grey = i <= ngreys ? igreys[i] : igreys[ngreys];
		for (long j = colb; j <= cole; j++) {
			double x2 = x1 + bar_width;
			double y1 = ymin, y2 = my data[irow][j];
			if (y2 > ymin) {
				if (y2 > ymax) {
					y2 = ymax;
				}
				Graphics_setGrey (g, grey);
				Graphics_fillRectangle (g, x1, x2, y1, y2);
				Graphics_setGrey (g, 0.0);   // black
				Graphics_rectangle (g, x1, x2, y1, y2);
			}
			x1 += dx;
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		double xb = (xoffsetFraction + 0.5 * (nrows + (nrows - 1) * interbarFraction)) * bar_width;
		for (long j = colb; j <= cole; j ++) {
			if (my columnLabels [j]) {
				Graphics_markBottom (g, xb, false, false, false, my columnLabels [j]);
			}
			xb += dx;
		}
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void TableOfReal_drawBiplot (TableOfReal me, Graphics g, double xmin, double xmax, double ymin, double ymax, double sv_splitfactor, int labelsize, bool garnish) {
	long nr = my numberOfRows, nc = my numberOfColumns, nPoints = nr + nc;
	int fontsize = Graphics_inqFontSize (g);

	autoSVD svd = SVD_create (nr, nc);

	NUMmatrix_copyElements (my data, svd -> u, 1, nr, 1, nc);
	NUMcentreColumns (svd -> u, 1, nr, 1, nc, nullptr);

	SVD_compute (svd.get());
	long numberOfZeroed = SVD_zeroSmallSingularValues (svd.get(), 0.0);

	long nmin = MIN (nr, nc) - numberOfZeroed;
	if (nmin < 2) {
		Melder_throw (U"There must be at least two (independent) columns in the table.");
	}
	autoNUMvector<double> x (1, nPoints);
	autoNUMvector<double> y (1, nPoints);

	double lambda1 = pow (svd -> d[1], sv_splitfactor);
	double lambda2 = pow (svd -> d[2], sv_splitfactor);
	for (long i = 1; i <= nr; i++) {
		x[i] = svd -> u[i][1] * lambda1;
		y[i] = svd -> u[i][2] * lambda2;
	}
	lambda1 = svd -> d[1] / lambda1;
	lambda2 = svd -> d[2] / lambda2;
	for (long i = 1; i <= nc; i++) {
		x[nr + i] = svd -> v[i][1] * lambda1;
		y[nr + i] = svd -> v[i][2] * lambda2;
	}

	if (xmax <= xmin) {
		NUMvector_extrema (x.peek(), 1, nPoints, &xmin, &xmax);
	}
	if (xmax <= xmin) {
		xmax += 1; xmin -= 1;
	}
	if (ymax <= ymin) {
		NUMvector_extrema (y.peek(), 1, nPoints, &ymin, &ymax);
	}
	if (ymax <= ymin) {
		ymax += 1.0;
		ymin -= 1.0;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	if (labelsize > 0) {
		Graphics_setFontSize (g, labelsize);
	}
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= nPoints; i ++) {
		char32 const *label;
		if (i <= nr) {
			label = my rowLabels [i];
			if (! label) {
				label = U"?__r_";
			}
		} else {
			label = my columnLabels [i - nr];
			if (! label) {
				label = U"?__c_";
			}
		}
		Graphics_text (g, x[i], y[i], label);
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_marksBottom (g, 2, true, true, false);
	}

	if (labelsize > 0) {
		Graphics_setFontSize (g, fontsize);
	}
}

void TableOfReal_drawBoxPlots (TableOfReal me, Graphics g, long rowmin, long rowmax, long colmin, long colmax, double ymin, double ymax, bool garnish) {
	if (rowmax < rowmin || rowmax < 1) {
		rowmin = 1; rowmax = my numberOfRows;
	}
	if (rowmin < 1) {
		rowmin = 1;
	}
	if (rowmax > my numberOfRows) {
		rowmax = my numberOfRows;
	}
	long numberOfRows = rowmax - rowmin + 1;
	if (colmax < colmin || colmax < 1) {
		colmin = 1; colmax = my numberOfColumns;
	}
	if (colmin < 1) {
		colmin = 1;
	}
	if (colmax > my numberOfColumns) {
		colmax = my numberOfColumns;
	}
	if (ymax <= ymin) {
		NUMmatrix_extrema (my data, rowmin, rowmax, colmin, colmax, &ymin, &ymax);
	}
	autoNUMvector<double> data (1, numberOfRows);

	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, ymin, ymax);
	Graphics_setInner (g);

	for (long j = colmin; j <= colmax; j ++) {
		double x = j, r = 0.05, w = 0.2, t;
		long ndata = 0;

		for (long i = 1; i <= numberOfRows; i++) {
			if (isdefined (t = my data [rowmin + i - 1] [j])) {
				data [++ ndata] = t;
			}
		}
		Graphics_boxAndWhiskerPlot (g, data.peek(), ndata, x, r, w, ymin, ymax);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		for (long j = colmin; j <= colmax; j++) {
			if (my columnLabels && my columnLabels[j] && my columnLabels[j][0]) {
				Graphics_markBottom (g, j, false, true, false, my columnLabels [j]);
			}
		}
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

int TableOfReal_equalLabels (TableOfReal me, TableOfReal thee, int rowLabels, int columnLabels) {
	Melder_assert (rowLabels || columnLabels);
	if (rowLabels) {
		if (my numberOfRows != thy numberOfRows) {
			return 0;
		}
		if (my rowLabels == thy rowLabels) {
			return 1;
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			if (Melder_cmp (my rowLabels[i], thy rowLabels[i])) {
				return 0;
			}
		}
	}
	if (columnLabels) {
		if (my numberOfColumns != thy numberOfColumns) {
			return 0;
		}
		if (my columnLabels == thy columnLabels) {
			return 1;
		}
		for (long i = 1; i <= my numberOfColumns; i++) {
			if (Melder_cmp (my columnLabels[i], thy columnLabels[i]) != 0) {
				return 0;
			}
		}
	}
	return 1;
}

void TableOfReal_copyLabels (TableOfReal me, TableOfReal thee, int rowOrigin, int columnOrigin) {
	if (rowOrigin == 1) {
		if (my numberOfRows != thy numberOfRows) {
			Melder_throw (U"#rows1 must equal #rows2");
		}
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, thy numberOfRows);
	} else if (rowOrigin == -1) {
		if (my numberOfColumns != thy numberOfRows) {
			Melder_throw (U"#columns1 must equal #rows2.");
		}
		NUMstrings_copyElements (my columnLabels, thy rowLabels, 1, thy numberOfRows);
	}
	if (columnOrigin == 1) {
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw (U"#columns1 must equal #columns2.");
		}
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, thy numberOfColumns);
	} else if (columnOrigin == -1) {
		if (my numberOfRows != thy numberOfColumns) {
			Melder_throw (U"#rows1 must equal #columns2");
		}
		NUMstrings_copyElements (my rowLabels, thy columnLabels, 1, thy numberOfColumns);
	}
}

void TableOfReal_labelsFromCollectionItemNames (TableOfReal me, Collection thee, int row, int column) {
	try {
		if (row) {
			Melder_assert (my numberOfRows == thy size);
			for (long i = 1; i <= my numberOfRows; i ++) {
				const char32 *name = Thing_getName (thy at [i]);
				TableOfReal_setRowLabel (me, i, name);
			}
		}
		if (column) {
			Melder_assert (my numberOfColumns == thy size);
			for (long i = 1; i <= my numberOfColumns; i ++) {
				const char32 *name = Thing_getName (thy at [i]);
				TableOfReal_setColumnLabel (me, i, name);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

void TableOfReal_centreColumns (TableOfReal me) {
	NUMcentreColumns (my data, 1, my numberOfRows, 1, my numberOfColumns, nullptr);
}

void TableOfReal_and_Categories_setRowLabels (TableOfReal me, Categories thee) {
	try {

		if (my numberOfRows != thy size) {
			Melder_throw (U"The number of items in both objects must be equal.");
		}

		/*
			If anything goes wrong we must leave the Table intact. We first copy the Categories, swap the labels
				and then delete the newly created categories.
		*/

		autoCategories categories_copy = Data_copy (thee);

		for (long i = 1; i <= my numberOfRows; i ++) {
			/*
				The modification of my rowLabels is implemented as a swap
				with the contents of the SimpleStrings in the copied Categories.
			*/
			char32 *tmp = categories_copy->at [i] -> string;
			categories_copy->at [i] -> string = my rowLabels [i];
			my rowLabels [i] = tmp;
		}
	} catch (MelderError) {
		Melder_throw (me, U": row labels not set from categories.");
	}
}

void TableOfReal_centreColumns_byRowLabel (TableOfReal me) {
	char32 *label = my rowLabels [1];
	long index = 1;

	for (long i = 2; i <= my numberOfRows; i ++) {
		char32 *li = my rowLabels [i];
		if (! Melder_equ (li, label)) {
			NUMcentreColumns (my data, index, i - 1, 1, my numberOfColumns, 0);
			label = li;
			index = i;
		}
	}
	NUMcentreColumns (my data, index, my numberOfRows, 1, my numberOfColumns, nullptr);
}

double TableOfReal_getRowSum (TableOfReal me, long index) {
	if (index < 1 || index > my numberOfRows) {
		return undefined;
	}

	double sum = 0.0;
	for (long j = 1; j <= my numberOfColumns; j ++) {
		sum += my data [index] [j];
	}
	return sum;
}

double TableOfReal_getColumnSumByLabel (TableOfReal me, const char32 *label) {
	long index = TableOfReal_columnLabelToIndex (me, label);
	if (index < 1) {
		Melder_throw (U"There is no \"", label, U"\" column label.");
	}
	return TableOfReal_getColumnSum (me, index);
}

double TableOfReal_getRowSumByLabel (TableOfReal me, const char32 *label) {
	long index = TableOfReal_rowLabelToIndex (me, label);
	if (index < 1) {
		Melder_throw (U"There is no \"", label, U"\" row label.");
	}
	return TableOfReal_getRowSum (me, index);
}

double TableOfReal_getColumnSum (TableOfReal me, long index) {
	if (index < 1 || index > my numberOfColumns) {
		return undefined;
	}

	double sum = 0.0;
	for (long i = 1; i <= my numberOfRows; i ++) {
		sum += my data [i] [index];
	}
	return sum;
}

double TableOfReal_getGrandSum (TableOfReal me) {
	double sum = 0.0;
	for (long i = 1; i <= my numberOfRows; i ++) {
		for (long j = 1; j <= my numberOfColumns; j ++) {
			sum += my data [i] [j];
		}
	}
	return sum;
}

void TableOfReal_centreRows (TableOfReal me) {
	NUMcentreRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_doubleCentre (TableOfReal me) {
	NUMdoubleCentre (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeColumns (TableOfReal me, double norm) {
	NUMnormalizeColumns (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_normalizeRows (TableOfReal me, double norm) {
	NUMnormalizeRows (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_standardizeColumns (TableOfReal me) {
	NUMstandardizeColumns (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_standardizeRows (TableOfReal me) {
	NUMstandardizeRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeTable (TableOfReal me, double norm) {
	NUMnormalize (my data, my numberOfRows, my numberOfColumns, norm);
}

double TableOfReal_getTableNorm (TableOfReal me) {
	double sumsq = 0.0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			sumsq += my data[i][j] * my data[i][j];
		}
	}
	return sqrt (sumsq);
}

int TableOfReal_checkPositive (TableOfReal me) {
	long negative = 0;

	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (my data[i][j] < 0.0) {
				negative ++; break;
			}
		}
	}
	return negative == 0;
}

/* undefined ??? */
void NUMdmatrix_getColumnExtrema (double **a, long rowb, long rowe, long icol, double *min, double *max);
void NUMdmatrix_getColumnExtrema (double **a, long rowb, long rowe, long icol, double *min, double *max) {
	*min = *max = a[rowb][icol];
	for (long i = rowb + 1; i <= rowe; i++) {
		double t = a[i][icol];
		if (t > *max) {
			*max = t;
		} else if (t < *min) {
			*min = t;
		}
	}
}

void TableOfReal_drawScatterPlotMatrix (TableOfReal me, Graphics g, long colb, long cole, double fractionWhite) {
	long m = my numberOfRows;

	if (colb == 0 && cole == 0) {
		colb = 1; cole = my numberOfColumns;
	} else if (cole < colb || colb < 1 || cole > my numberOfColumns) {
		return;
	}

	long n = cole - colb + 1;
	if (n == 1) {
		return;
	}
	autoNUMvector<double> xmin (colb, cole);
	autoNUMvector<double> xmax (colb, cole);

	for (long j = colb; j <= cole; j++) {
		xmin[j] = xmax[j] = my data[1][j];
	}
	for (long i = 2; i <= m; i++) {
		for (long j = colb; j <= cole; j++) {
			if (my data[i][j] > xmax[j]) {
				xmax[j] = my data[i][j];
			} else if (my data[i][j] < xmin[j]) {
				xmin[j] = my data[i][j];
			}
		}
	}
	for (long j = colb; j <= cole; j++) {
		double extra = fractionWhite * fabs (xmax[j] - xmin[j]);
		if (extra == 0) {
			extra = 0.5;
		}
		xmin[j] -= extra; xmax[j] += extra;
	}

	Graphics_setWindow (g, 0.0, n, 0.0, n);
	Graphics_setInner (g);
	Graphics_line (g, 0.0, n, n, n);
	Graphics_line (g, 0.0, 0.0, 0.0, n);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= n; i++) {
		long xcol, ycol = colb + i - 1;
		const char32 *mark;
		char32 label[40];
		Graphics_line (g, 0.0, n - i, n, n - i);
		Graphics_line (g, i, n, i, 0.0);
		for (long j = 1; j <= n; j++) {
			xcol = colb + j - 1;
			if (i == j) {
				mark = my columnLabels[xcol];
				if (! mark) {
					Melder_sprint (label,40, U"Column ", xcol);   // ppgb: 20 chars is niet genoeg voor een long
					mark = label;
				}
				Graphics_text (g, j - 0.5, n - i + 0.5, mark);
			} else {
				for (long k = 1; k <= m; k++) {
					double x = j - 1 + (my data[k][xcol] - xmin[xcol]) / (xmax[xcol] - xmin[xcol]);
					double y = n - i + (my data[k][ycol] - xmin[ycol]) / (xmax[ycol] - xmin[ycol]);
					mark = EMPTY_STRING (my rowLabels[k]) ? U"+" : my rowLabels[k];
					Graphics_text (g, x, y, mark);
				}
			}
		}
	}
	Graphics_unsetInner (g);
}

void TableOfReal_drawAsScalableSquares (TableOfReal me, Graphics g, double zmin, double zmax, double cellSizeFactor, int randomFillOrder, bool garnish) {
	try {
		cellSizeFactor = cellSizeFactor <= 0.0 ? 1.0 : cellSizeFactor;
		if (zmin == 0 && zmax == 0) {
			NUMmatrix_extrema<double> (my data, 1, my numberOfRows, 1, my numberOfColumns, &zmin, &zmax);
		}
		double xmin = 0.0, xmax = my numberOfColumns + 1.0, ymin = 0.0, ymax = my numberOfRows + 1.0;
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		Graphics_matrixAsSquares (g, my data, my numberOfRows, my numberOfColumns, zmin, zmax, cellSizeFactor, randomFillOrder);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottomEvery (g, 1.0, 1.0, false, true, false);
			Graphics_marksLeftEvery (g, 1.0, 1.0, false, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void TableOfReal_drawScatterPlot (TableOfReal me, Graphics g, long icx, long icy, long rowb, long rowe, double xmin, double xmax, double ymin, double ymax, int labelSize, bool useRowLabels, const char32 *label, bool garnish) {
	double m = my numberOfRows, n = my numberOfColumns;
	int fontSize = Graphics_inqFontSize (g);

	if (icx < 1 || icx > n || icy < 1 || icy > n) {
		return;
	}
	if (rowb < 1) {
		rowb = 1;
	}
	if (rowe > m) {
		rowe = (long) floor (m);
	}
	if (rowe <= rowb) {
		rowb = 1; rowe = (long) floor (m);
	}

	if (xmax == xmin) {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icx, & xmin, & xmax);
		double tmp = xmax - xmin == 0.0 ? 0.5 : 0.0;
		xmin -= tmp; xmax += tmp;
	}
	if (ymax == ymin) {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icy, & ymin, & ymax);
		double tmp = ymax - ymin == 0.0 ? 0.5 : 0.0;
		ymin -= tmp; ymax += tmp;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);

	long noLabel = 0;
	for (long i = rowb; i <= rowe; i++) {
		double x = my data[i][icx], y = my data[i][icy];

		if (((xmin < xmax && x >= xmin && x <= xmax) || (xmin > xmax && x <= xmin && x >= xmax)) &&
		        ((ymin < ymax && y >= ymin && y <= ymax) || (ymin > ymax && y <= ymin && y >= ymax))) {
			const char32 *plotLabel = useRowLabels ? my rowLabels[i] : label;
			if (! NUMstring_containsPrintableCharacter (plotLabel)) {
				noLabel++;
				continue;
			}
			Graphics_text (g, x, y, plotLabel);
		}
	}

	Graphics_setFontSize (g, fontSize);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		if (ymin < ymax) {
			if (my columnLabels[icx]) {
				Graphics_textBottom (g, true, my columnLabels[icx]);
			}
			Graphics_marksBottom (g, 2, true, true, false);
		} else {
			if (my columnLabels[icx]) {
				Graphics_textTop (g, true, my columnLabels[icx]);
			}
			Graphics_marksTop (g, 2, true, true, false);
		}
		if (xmin < xmax) {
			if (my columnLabels[icy]) {
				Graphics_textLeft (g, true, my columnLabels[icy]);
			}
			Graphics_marksLeft (g, 2, true, true, false);
		} else {
			if (my columnLabels[icy]) {
				Graphics_textRight (g, true, my columnLabels[icy]);
			}
			Graphics_marksRight (g, 2, true, true, false);
		}
	}
	if (noLabel > 0) {
		Melder_warning (noLabel, U" from ", my numberOfRows, U" labels are "
		                U"not visible because they are empty or they contain only spaces or non-printable characters");
	}
}


#pragma mark - class TableOfRealList

autoTableOfReal TableOfRealList_sum (TableOfRealList me) {
	try {
		if (my size <= 0) {
			return autoTableOfReal();
		}
		autoTableOfReal thee = Data_copy (my at [1]);

		for (long i = 2; i <= my size; i ++) {
			TableOfReal him = my at [i];
			if (thy numberOfRows != his numberOfRows || thy numberOfColumns != his numberOfColumns || ! TableOfReal_equalLabels (thee.get(), him, 1, 1)) {
				Melder_throw (U"Dimensions or labels differ for table 1 and ", i, U".");
			}
			for (long j = 1; j <= thy numberOfRows; j ++) {
				for (long k = 1; k <= thy numberOfColumns; k ++) {
					thy data [j] [k] += his data [j] [k];
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": sum not created.");
	}
}

bool TableOfRealList_haveIdenticalDimensions (TableOfRealList me) {
	if (my size < 2) {
		return true;
	}
	TableOfReal t1 = my at [1];
	for (long i = 2; i <= my size; i ++) {
		TableOfReal t = my at [i];
		if (t -> numberOfColumns != t1 -> numberOfColumns || t -> numberOfRows != t1 -> numberOfRows) {
			return false;
		}
	}
	return true;
}

double TableOfReal_getColumnQuantile (TableOfReal me, long col, double quantile) {
	try {
		if (col < 1 || col > my numberOfColumns) {
			return undefined;
		}
		autoNUMvector<double> values (1, my numberOfRows);

		for (long i = 1; i <= my numberOfRows; i++) {
			values[i] = my data[i][col];
		}

		NUMsort_d (my numberOfRows, values.peek());
		double r = NUMquantile (my numberOfRows, values.peek(), quantile);
		return r;
	} catch (MelderError) {
		return undefined;
	}
}

static autoTableOfReal TableOfReal_createPolsVanNieropData (int choice, bool include_levels) {
	try {
		autoTable table = Table_create_polsVanNierop1973 ();

		// Default: Pols 50 males, first part of the table.

		long nrows = 50 * 12;
		long ncols = include_levels ? 6 : 3;
		long ib = 1;

		if (choice == 2) { /* Van Nierop 25 females */
			ib = nrows + 1;
			nrows = 25 * 12;
		}

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);

		for (long i = 1; i <= nrows; i ++) {
			TableRow row = table -> rows.at [ib + i - 1];
			TableOfReal_setRowLabel (thee.get(), i, row -> cells [4]. string);
			for (long j = 1; j <= 3; j++) {
				thy data [i] [j] = Melder_atof (row -> cells [4 + j]. string);
				if (include_levels) {
					thy data [i] [3 + j] = Melder_atof (row -> cells [7 + j]. string);
				}
			}
		}
		for (long j = 1; j <= 3; j ++) {
			const char32 *label = table -> columnHeaders [4 + j]. label;
			TableOfReal_setColumnLabel (thee.get(), j, label);
			if (include_levels) {
				label = table -> columnHeaders[7 + j].label;
				TableOfReal_setColumnLabel (thee.get(), 3 + j, label);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal from Pols & Van Nierop data not created.");
	}
}

autoTableOfReal TableOfReal_create_pols1973 (bool include_levels) {
	return TableOfReal_createPolsVanNieropData (1, include_levels);
}

autoTableOfReal TableOfReal_create_vanNierop1973 (bool include_levels) {
	return TableOfReal_createPolsVanNieropData (2, include_levels);
}

autoTableOfReal TableOfReal_create_weenink1983 (int option) {
	try {
		long nvowels = 12, ncols = 3, nrows = 10 * nvowels;

		autoTable table = Table_create_weenink1983 ();

		long ib = ( option == 1 ? 1 : option == 2 ? 11 : 21 ); /* m f c*/
		ib = (ib - 1) * nvowels + 1;

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);

		for (long i = 1; i <= nrows; i ++) {
			TableRow row = table -> rows.at [ib + i - 1];
			TableOfReal_setRowLabel (thee.get(), i, row -> cells [5]. string);
			for (long j = 1; j <= 3; j ++) {
				thy data [i] [j] = Melder_atof (row -> cells [6 + j]. string); /* Skip F0 */
			}
		}
		for (long j = 1; j <= 3; j++)  {
			const char32 *label = table -> columnHeaders [6 + j]. label;
			TableOfReal_setColumnLabel (thee.get(), j, label);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal from Weenink data not created.");
	}
}

autoTableOfReal TableOfReal_randomizeRows (TableOfReal me) {
	try {
		autoPermutation p = Permutation_create (my numberOfRows);
		Permutation_permuteRandomly_inline (p.get(), 0, 0);
		autoTableOfReal thee = TableOfReal_and_Permutation_permuteRows (me, p.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": randomized rows not created");
	}
}

autoTableOfReal TableOfReal_bootstrap (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);

		// Copy column labels.

		for (long i = 1; i <= my numberOfColumns; i++) {
			if (my columnLabels[i]) {
				TableOfReal_setColumnLabel (thee.get(), i, my columnLabels[i]);
			}
		}

		/*
			Select randomly from table with replacement. Because of replacement
			you do not get back the original data set. A random fraction,
			typically 1/e (37%) are replaced by duplicates.
		*/

		for (long i = 1; i <= my numberOfRows; i++) {
			long p = NUMrandomInteger (1, my numberOfRows);
			NUMvector_copyElements (my data[p], thy data[i], 1, my numberOfColumns);
			if (my rowLabels[p]) {
				TableOfReal_setRowLabel (thee.get(), i, my rowLabels[p]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": bootstrapped data not created.");
	}
}

void TableOfReal_changeRowLabels (TableOfReal me, const char32 *search, const char32 *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	try {
		autostring32vector rowLabels (strs_replace (my rowLabels, 1, my numberOfRows, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp), 1, my numberOfRows);
		NUMstrings_free (my rowLabels, 1, my numberOfRows);
		my rowLabels = rowLabels.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": row labels not changed.");
	}
}

void TableOfReal_changeColumnLabels (TableOfReal me, const char32 *search, const char32 *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	try {
		autostring32vector columnLabels (strs_replace (my columnLabels, 1, my numberOfColumns, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp), 1, my numberOfColumns);
		NUMstrings_free (my columnLabels, 1, my numberOfColumns);
		my columnLabels = columnLabels.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": column labels not changed.");
	}
}

long TableOfReal_getNumberOfLabelMatches (TableOfReal me, const char32 *search, int columnLabels, int use_regexp) {
	long nmatches = 0, numberOfLabels = my numberOfRows;
	char32 **labels = my rowLabels;
	regexp *compiled_regexp = nullptr;

	if (! search || str32len (search) == 0) {
		return 0;
	}
	if (columnLabels) {
		numberOfLabels = my numberOfColumns;
		labels = my columnLabels;
	}
	if (use_regexp) {
		compiled_regexp = CompileRE_throwable (search, 0);
	}
	for (long i = 1; i <= numberOfLabels; i++) {
		if (! labels[i]) {
			continue;
		}
		if (use_regexp) {
			if (ExecRE (compiled_regexp, 0, labels[i], nullptr, 0, U'\0', U'\0', 0, 0, 0)) {
				nmatches++;
			}
		} else if (str32equ (labels[i], search)) {
			nmatches++;
		}
	}
	if (use_regexp) {
		free (compiled_regexp);
	}
	return nmatches;
}

void TableOfReal_drawVectors (TableOfReal me, Graphics g, long colx1, long coly1, long colx2, long coly2, double xmin, double xmax, double ymin, double ymax, int vectype, int labelsize, bool garnish) {
	long nx = my numberOfColumns, ny = my numberOfRows;
	int fontsize = Graphics_inqFontSize (g);

	if (colx1 < 1 || colx1 > nx || coly1 < 1 || coly1 > nx) {
		Melder_warning (U"The index in the \"From\" column(s) must be in range [1, ", nx, U"].");
		return;
	}
	if (colx2 < 1 || colx2 > nx || coly2 < 1 || coly2 > nx) {
		Melder_warning (U"The index in the \"To\" column(s) must be in range [1, ", nx, U"].");
		return;
	}

	double min, max;
	if (xmin >= xmax) {
		NUMmatrix_extrema (my data, 1, ny, colx1, colx1, &min, &max);
		NUMmatrix_extrema (my data, 1, ny, colx2, colx2, &xmin, &xmax);
		if (min < xmin) {
			xmin = min;
		}
		if (max > xmax) {
			xmax = max;
		}
	}
	if (ymin >= ymax) {
		NUMmatrix_extrema (my data, 1, ny, coly1, coly1, &min, &max);
		NUMmatrix_extrema (my data, 1, ny, coly2, coly2, &ymin, &ymax);
		if (min < ymin) {
			ymin = min;
		}
		if (max > ymax) {
			ymax = max;
		}
	}
	if (xmin == xmax) {
		if (ymin == ymax) {
			return;
		}
		xmin -= 0.5;
		xmax += 0.5;
	}
	if (ymin == ymax) {
		ymin -= 0.5;
		ymax += 0.5;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	if (labelsize > 0) {
		Graphics_setFontSize (g, labelsize);
	}
	for (long i = 1; i <= ny; i++) {
		double x1 = my data[i][colx1];
		double y1 = my data[i][coly1];
		double x2 = my data[i][colx2];
		double y2 = my data[i][coly2];
		const char32 *mark = EMPTY_STRING (my rowLabels[i]) ? U"" : my rowLabels[i];
		if (vectype == Graphics_LINE) {
			Graphics_line (g, x1, y1, x2, y2);
		} else if (vectype == Graphics_TWOWAYARROW) {
			Graphics_arrow (g, x1, y1, x2, y2);
			Graphics_arrow (g, x2, y2, x1, y1);
		} else { /*if (vectype == Graphics_ARROW) */
			Graphics_arrow (g, x1, y1, x2, y2);
		}
		if (labelsize > 0) {
			Graphics_text (g, x1, y1, mark);
		}
	}
	if (labelsize > 0) {
		Graphics_setFontSize (g, fontsize);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

void TableOfReal_drawColumnAsDistribution (TableOfReal me, Graphics g, int column, double minimum, double maximum, long nBins, double freqMin, double freqMax, bool cumulative, bool garnish) {
	if (column < 1 || column > my numberOfColumns) {
		return;
	}
	autoMatrix thee = TableOfReal_to_Matrix (me);
	Matrix_drawDistribution (thee.get(), g, column - 0.5, column + 0.5, 0.0, 0.0, minimum, maximum, nBins, freqMin, freqMax, cumulative, garnish);
	if (garnish && my columnLabels[column] != 0) {
		Graphics_textBottom (g, true, my columnLabels[column]);
	}
}

autoTableOfReal TableOfReal_sortRowsByIndex (TableOfReal me, long index[], int reverse) {
	try {
		if (my rowLabels == 0) {
			Melder_throw (U"No labels to sort");
		}

		double min, max;
		NUMvector_extrema (index, 1, my numberOfRows, &min, &max);
		if (min < 1 || max > my numberOfRows) {
			Melder_throw (U"One or more indices out of range [1, ", my numberOfRows, U"].");
		}
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);

		for (long i = 1; i <= my numberOfRows; i++) {
			long myindex = reverse ? i : index[i];
			long thyindex = reverse ? index[i] : i;
			const char32 *mylabel = my rowLabels[myindex];
			double *mydata = my data[myindex];
			double *thydata = thy data[thyindex];

			// Copy the row label

			thy rowLabels[i] = Melder_dup (mylabel);

			// Copy the row values

			for (long j = 1; j <= my numberOfColumns; j++) {
				thydata[j] = mydata[j];
			}
		}

		// Copy column labels.

		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not sorted by row index.");
	}
}

long *TableOfReal_getSortedIndexFromRowLabels (TableOfReal me) {
	try {
		autoNUMvector<long> index (1, my numberOfRows);
		NUMindexx_s (my rowLabels, my numberOfRows, index.peek());
		return index.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": no sorted index created.");
	}
}

autoTableOfReal TableOfReal_sortOnlyByRowLabels (TableOfReal me) {
	try {
		autoPermutation index = TableOfReal_to_Permutation_sortRowLabels (me);
		autoTableOfReal thee = TableOfReal_and_Permutation_permuteRows (me, index.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not sorted by row labels.");
	}
}

static void NUMmedianizeColumns (double **a, long rb, long re, long cb, long ce) {
	long n = re - rb + 1;

	if (n < 2) {
		return;
	}
	autoNUMvector<double> tmp (1, n);
	for (long j = cb; j <= ce; j++) {
		long k = 1;
		for (long i = rb; i <= re; i++, k++) {
			tmp[k] = a[i][j];
		}
		NUMsort_d (n, tmp.peek());
		double median = NUMquantile (n, tmp.peek(), 0.5);
		for (long i = rb; i <= re; i++) {
			a[i][j] = median;
		}
	}
}

static void NUMstatsColumns (double **a, long rb, long re, long cb, long ce, int stats) {
	if (stats == 0) {
		NUMaverageColumns (a, rb, re, cb, ce);
	} else {
		NUMmedianizeColumns (a, rb, re, cb, ce);
	}
}

autoTableOfReal TableOfReal_meansByRowLabels (TableOfReal me, int expand, int stats) {
	try {
		autoTableOfReal thee;
		autoNUMvector<long> index (TableOfReal_getSortedIndexFromRowLabels (me), 1);
		autoTableOfReal sorted = TableOfReal_sortRowsByIndex (me, index.peek(), 0);

		long indexi = 1, indexr = 0;
		const char32 *label = sorted -> rowLabels[1];
		for (long i = 2; i <= my numberOfRows; i++) {
			const char32 *li = sorted -> rowLabels[i];
			if (Melder_cmp (li, label) != 0) {
				NUMstatsColumns (sorted -> data, indexi, i - 1, 1, my numberOfColumns, stats);

				if (expand == 0) {
					indexr++;
					TableOfReal_copyOneRowWithLabel (sorted.get(), sorted.get(), indexi, indexr);
				}
				label = li; indexi = i;
			}
		}

		NUMstatsColumns (sorted -> data, indexi, my numberOfRows, 1, my numberOfColumns, stats);

		if (expand != 0) {
			// Now invert the table.

			char32 **tmp = sorted -> rowLabels; sorted -> rowLabels = my rowLabels;
			thee = TableOfReal_sortRowsByIndex (sorted.get(), index.peek(), 1);
			sorted -> rowLabels = tmp;
		} else {
			indexr++;
			TableOfReal_copyOneRowWithLabel (sorted.get(), sorted.get(), indexi, indexr);
			thee = TableOfReal_create (indexr, my numberOfColumns);
			for (long i = 1; i <= indexr; i++) {
				TableOfReal_copyOneRowWithLabel (sorted.get(), thee.get(), i, i);
			}
			NUMstrings_copyElements (sorted -> columnLabels, thy columnLabels, 1, my numberOfColumns);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": means by row labels not created.");
	}
}

autoTableOfReal TableOfReal_rankColumns (TableOfReal me) {
	try {
		autoTableOfReal thee = Data_copy (me);
		NUMrankColumns (thy data, 1, thy numberOfRows, 1, thy numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": column ranks not created.");
	}
}

void TableOfReal_setSequentialColumnLabels (TableOfReal me, long from, long to, const char32 *precursor, long number, long increment) {
	if (from == 0) {
		from = 1;
	}
	if (to == 0) {
		to = my numberOfColumns;
	}
	if (from < 1 || from > my numberOfColumns || to < from || to > my numberOfColumns) {
		Melder_throw (U"Wrong column indices.");
	}
	NUMstrings_setSequentialNumbering (my columnLabels, from, to, precursor, number, increment, (int) 0);
}

void TableOfReal_setSequentialRowLabels (TableOfReal me, long from, long to, const char32 *precursor, long number, long increment) {
	if (from == 0) {
		from = 1;
	}
	if (to == 0) {
		to = my numberOfRows;
	}
	if (from < 1 || from > my numberOfRows || to < from || to > my numberOfRows) {
		Melder_throw (U"Wrong row indices.");
	}
	NUMstrings_setSequentialNumbering (my rowLabels, from, to, precursor, number, increment, (int) 0);
}

/* For the inheritors */
autoTableOfReal TableOfReal_to_TableOfReal (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not copied.");
	}
}

autoTableOfReal TableOfReal_choleskyDecomposition (TableOfReal me, int upper, int inverse) {
	try {
		char diag = 'N';
		long n = my numberOfColumns, lda = my numberOfRows, info;

		if (n != lda) {
			Melder_throw (U"The table must be a square symmetric table.");
		}
		autoTableOfReal thee = Data_copy (me);

		if (upper) {
			for (long i = 2; i <= n; i++) for (long j = 1; j < i; j++) {
					thy data[i][j] = 0.0;
				}
		} else {
			for (long i = 1; i < n; i++) for (long j = i + 1; j <= n; j++) {
					thy data[i][j] = 0.0;
				}
		}
		char uplo = upper ? 'L' : 'U';
		NUMlapack_dpotf2 (&uplo, &n, &thy data[1][1], &lda, &info);
		if (info != 0) {
			Melder_throw (U"dpotf2 fails");
		}

		if (inverse) {
			NUMlapack_dtrtri (&uplo, &diag, &n, &thy data[1][1], &lda, &info);
			if (info != 0) {
				Melder_throw (U"dtrtri fails");
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Cholesky decomposition not performed.");
	}
}

autoTableOfReal TableOfReal_appendColumns (TableOfReal me, TableOfReal thee) {
	try {
		long ncols = my numberOfColumns + thy numberOfColumns;
		long labeldiffs = 0;

		if (my numberOfRows != thy numberOfRows) {
			Melder_throw (U"Number of rows must be equal.");
		}
		/* Stricter label checking???
			append only if
			(my rowLabels[i] == thy rowlabels[i], i=1..my numberOfRows) or
			(my rowLabels[i] == 'empty', i=1..my numberOfRows)  or
			(thy rowLabels[i] == 'empty', i=1..my numberOfRows);
			'empty':  nullptr or \w*
		*/
		autoTableOfReal him = TableOfReal_create (my numberOfRows, ncols);
		NUMstrings_copyElements (my rowLabels, his rowLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, his columnLabels,  1, my numberOfColumns);
		NUMstrings_copyElements (thy columnLabels, &his columnLabels[my numberOfColumns], 1, thy numberOfColumns);
		for (long i = 1; i <= my numberOfRows; i++) {
			if (Melder_cmp (my rowLabels[i], thy rowLabels[i]) != 0) {
				labeldiffs++;
			}
			NUMvector_copyElements (my data[i], his data[i], 1, my numberOfColumns);
			NUMvector_copyElements (thy data[i], &his data[i][my numberOfColumns], 1, thy numberOfColumns);
		}
		if (labeldiffs > 0) {
			Melder_warning (labeldiffs, U" row labels differed.");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with appended columns not created.");
	}
}

autoTableOfReal TableOfRealList_appendColumnsMany (TableOfRealList me) {
	try {
		if (my size == 0) {
			Melder_throw (U"No tables selected.");
		}
		TableOfReal thee = my at [1];
		long nrow = thy numberOfRows;
		long ncol = thy numberOfColumns;
		for (long itab = 2; itab <= my size; itab ++) {
			thee = my at [itab];
			ncol += thy numberOfColumns;
			if (thy numberOfRows != nrow) {
				Melder_throw (U"Numbers of rows in item ", itab, U" differs from previous.");
			}
		}
		autoTableOfReal him = TableOfReal_create (nrow, ncol);
		/* Unsafe: new attributes not initialized. */
		for (long irow = 1; irow <= nrow; irow ++) {
			TableOfReal_setRowLabel (him.get(), irow, thy rowLabels [irow]);
		}
		ncol = 0;
		for (long itab = 1; itab <= my size; itab ++) {
			thee = my at [itab];
			for (long icol = 1; icol <= thy numberOfColumns; icol ++) {
				ncol++;
				TableOfReal_setColumnLabel (him.get(), ncol, thy columnLabels [icol]);
				for (long irow = 1; irow <= nrow; irow ++) {
					his data [irow] [ncol] = thy data [irow] [icol];
				}
			}
		}
		Melder_assert (ncol == his numberOfColumns);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with appended columns not created.");
	}
}

double TableOfReal_normalityTest_BHEP (TableOfReal me, double *h, double *p_tnb, double *p_lnmu, double *p_lnvar) {
	try {
		/* Henze & Wagner (1997), A new approach to the BHEP tests for multivariate normality, 
		 *  Journal of Multivariate Analysis 62, 1-23.
		 */

		long n = my numberOfRows, p = my numberOfColumns;
		double beta = *h > 0 ? NUMsqrt1_2 / *h : NUMsqrt1_2 * pow ( (1.0 + 2 * p) / 4, 1.0 / (p + 4)) * pow (n, 1.0 / (p + 4));
		double p2 = p / 2.0;
		double beta2 = beta * beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
		double gamma = 1 + 2 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
		double delta = 1.0 + beta2 * (4 + 3 * beta2), delta2 = delta * delta;
		double prob = undefined;

		if (*h <= 0) {
			*h = NUMsqrt1_2 / beta;
		}

		double tnb = undefined, lnmu = undefined, lnvar = undefined;

		if (n < 2 || p < 1) {
			return undefined;
		}

		autoCovariance thee = TableOfReal_to_Covariance (me);
		try {
			SSCP_expandLowerCholesky (thee.get());
		} catch (MelderError) {
			tnb = 4.0 * n;
		}
		{
			double djk, djj, sumjk = 0.0, sumj = 0.0;
			double b1 = beta2 / 2.0, b2 = b1 / (1.0 + beta2);
			/* Heinze & Wagner (1997), page 3
				We use d[j][k] = ||Y[j]-Y[k]||^2 = (Y[j]-Y[k])'S^(-1)(Y[j]-Y[k])
				So d[j][k]= d[k][j] and d[j][j] = 0
			*/
			for (long j = 1; j <= n; j++) {
				for (long k = 1; k < j; k++) {
					djk = NUMmahalanobisDistance_chi (thy lowerCholesky, my data[j], my data[k], p, p);
					sumjk += 2.0 * exp (-b1 * djk); // factor 2 because d[j][k] == d[k][j]
				}
				sumjk += 1; // for k == j
				djj = NUMmahalanobisDistance_chi (thy lowerCholesky, my data[j], thy centroid, p, p);
				sumj += exp (-b2 * djj);
			}
			tnb = (1.0 / n) * sumjk - 2.0 * pow (1.0 + beta2, - p2) * sumj + n * pow (gamma, - p2); // n *
		}
		double mu = 1.0 - pow (gamma, -p2) * (1.0 + p * beta2 / gamma + p * (p + 2) * beta4 / (2.0 * gamma2));
		double var = 2.0 * pow (1 + 4 * beta2, -p2)
			+ 2.0 * pow (gamma,  -p) * (1.0 + 2 * p * beta4 / gamma2  + 3 * p * (p + 2) * beta8 / (4.0 * gamma4))
			- 4.0 * pow (delta, -p2) * (1.0 + 3 * p * beta4 / (2 * delta) + p * (p + 2) * beta8 / (2.0 * delta2));
		double mu2 = mu * mu;
		lnmu = 0.5 * log (mu2 * mu2 / (mu2 + var)); //log (sqrt (mu2 * mu2 /(mu2 + var)));
		lnvar = sqrt (log ( (mu2 + var) / mu2));
		prob = NUMlogNormalQ (tnb, lnmu, lnvar);
		if (p_tnb) {
			*p_tnb = tnb;
		}
		if (p_lnmu) {
			*p_lnmu = lnmu;
		}
		if (p_lnvar) {
			*p_lnvar = lnvar;
		}
		return prob;
	} catch (MelderError) {
		Melder_throw (me, U": cannot determine normality.");
	}
}

autoTableOfReal TableOfReal_and_TableOfReal_crossCorrelations (TableOfReal me, TableOfReal thee, int by_columns, int center, int normalize) {
	return by_columns ? TableOfReal_and_TableOfReal_columnCorrelations (me, thee, center, normalize) :
	       TableOfReal_and_TableOfReal_rowCorrelations (me, thee, center, normalize);
}

autoTableOfReal TableOfReal_and_TableOfReal_rowCorrelations (TableOfReal me, TableOfReal thee, int center, int normalize) {
	try {
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw (U"Both tables must have the same number of columns.");
		}

		autoTableOfReal him = TableOfReal_create (my numberOfRows, thy numberOfRows);
		autoNUMmatrix<double> my_data (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfColumns), 1, 1);
		autoNUMmatrix<double> thy_data (NUMmatrix_copy (thy data, 1, thy numberOfRows, 1, thy numberOfColumns), 1, 1);
		if (center) {
			NUMcentreRows (my_data.peek(), 1, my numberOfRows, 1, my numberOfColumns);
			NUMcentreRows (thy_data.peek(), 1, thy numberOfRows, 1, thy numberOfColumns);
		}
		if (normalize) {
			NUMnormalizeRows (my_data.peek(), my numberOfRows, my numberOfColumns, 1);
			NUMnormalizeRows (thy_data.peek(), thy numberOfRows, thy numberOfColumns, 1);
		}
		NUMstrings_copyElements (my rowLabels, his rowLabels, 1, his numberOfRows);
		NUMstrings_copyElements (thy rowLabels, his columnLabels, 1, his numberOfColumns);
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long k = 1; k <= thy numberOfRows; k++) {
				double ctmp = 0;
				for (long j = 1; j <= my numberOfColumns; j++) {
					ctmp += my_data[i][j] * thy_data[k][j];
				}
				his data[i][k] = ctmp;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with row correlations not created.");
	}
}

autoTableOfReal TableOfReal_and_TableOfReal_columnCorrelations (TableOfReal me, TableOfReal thee, int center, int normalize) {
	try {
		if (my numberOfRows != thy numberOfRows) {
			Melder_throw (U"Both tables must have the same number of rows.");
		}

		autoTableOfReal him = TableOfReal_create (my numberOfColumns, thy numberOfColumns);
		autoNUMmatrix<double> my_data (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfColumns), 1, 1);
		autoNUMmatrix<double> thy_data (NUMmatrix_copy (thy data, 1, thy numberOfRows, 1, thy numberOfColumns), 1, 1);
		if (center) {
			NUMcentreColumns (my_data.peek(), 1, my numberOfRows, 1, my numberOfColumns, nullptr);
			NUMcentreColumns (thy_data.peek(), 1, thy numberOfRows, 1, thy numberOfColumns, nullptr);
		}
		if (normalize) {
			NUMnormalizeColumns (my_data.peek(), my numberOfRows, my numberOfColumns, 1);
			NUMnormalizeColumns (thy_data.peek(), thy numberOfRows, thy numberOfColumns, 1);
		}
		NUMstrings_copyElements (my columnLabels, his rowLabels, 1, his numberOfRows);
		NUMstrings_copyElements (thy columnLabels, his columnLabels, 1, his numberOfColumns);

		for (long j = 1; j <= my numberOfColumns; j++) {
			for (long k = 1; k <= thy numberOfColumns; k++) {
				double ctmp = 0.0;
				for (long i = 1; i <= my numberOfRows; i++) {
					ctmp += my_data[i][j] * thy_data[i][k];
				}
				his data[j][k] = ctmp;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with column correlations not created.");
	}
}

#undef EMPTY_STRING
#undef MAX
#undef MIN

/* End of file TableOfReal_extensions.c 1869*/
