/* TableOfReal_extensions.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20070822 wchar_t
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

#define EMPTY_STRING(s) ((s) == NULL || s[0] == '\0')
#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

#define Graphics_ARROW 1
#define Graphics_TWOWAYARROW 2
#define Graphics_LINE 3

TableOfReal TableOfReal_and_TableOfReal_columnCorrelations (I, thou, int center, int normalize);
TableOfReal TableOfReal_and_TableOfReal_rowCorrelations (I, thou, int center, int normalize);

int TableOfReal_areAllCellsDefined (I, long rb, long re, long cb, long ce) {
	iam (TableOfReal);

	if (re <= rb || rb < 1 || re > my numberOfRows) {
		rb = 1; re = my numberOfRows;
	}
	if (ce <= cb || cb < 1 || ce > my numberOfColumns) {
		cb = 1; ce = my numberOfColumns;
	}

	autoNUMvector<long> invalid_columns (1, my numberOfColumns);

	long numberOfInvalidRows = 0, numberOfInvalidColumns = 0;
	for (long i = rb; i <= re; i++) {
		for (long j = cb; j <= ce; j++) {
			long rowcount = 0;
			if (my data[i][j] == NUMundefined) {
				invalid_columns[j]++;
				rowcount++;
			}
			if (rowcount > 0) {
				numberOfInvalidRows++;
			}
		}
	}
	if (numberOfInvalidRows != 0) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (invalid_columns[j] > 0) {
				numberOfInvalidColumns++;
			}
		}
		Melder_throw (numberOfInvalidRows == 1 ? "One row contains invalid data." : (numberOfInvalidColumns == 1 ?  "One column contains invalid data." :
		              "Several rows and columns contain invalid data."));
	}
	return numberOfInvalidRows == 0 ? 1 : 0;
}

void TableOfReal_copyOneRowWithLabel (I, thou, long myrow, long thyrow) {
	iam (TableOfReal); thouart (TableOfReal);
	try {
		if (me == thee && myrow == thyrow) {
			return;
		}

		if (myrow < 1 ||  myrow > my  numberOfRows || thyrow < 1 || thyrow > thy numberOfRows || my numberOfColumns != thy numberOfColumns) {
			Melder_throw ("The dimensions do not fit.");
		}

		Melder_free (thy rowLabels[thyrow]);
		thy rowLabels[thyrow] = Melder_wcsdup (my rowLabels[myrow]);

		if (my data[myrow] != thy data[thyrow]) {
			NUMvector_copyElements (my data[myrow], thy data[thyrow], 1, my numberOfColumns);
		}
	} catch (MelderError) {
		Melder_throw (me, ": row ", myrow, " not copied to ", thee);
	}
}

int TableOfReal_hasRowLabels (I) {
	iam (TableOfReal);

	if (my rowLabels == NULL) {
		return 0;
	}
	for (long i = 1; i <= my numberOfRows; i++) {
		if (EMPTY_STRING (my rowLabels[i])) {
			return 0;
		}
	}
	return 1;
}

int TableOfReal_hasColumnLabels (I) {
	iam (TableOfReal);

	if (my columnLabels == NULL) {
		return 0;
	}
	for (long i = 1; i <= my numberOfColumns; i++) {
		if (EMPTY_STRING (my columnLabels[i])) {
			return 0;
		}
	}
	return 1;
}

TableOfReal TableOfReal_createIrisDataset () {
	float iris[150][4] = {
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

		TableOfReal_setColumnLabel (me.peek(), 1, L"sl");
		TableOfReal_setColumnLabel (me.peek(), 2, L"sw");
		TableOfReal_setColumnLabel (me.peek(), 3, L"pl");
		TableOfReal_setColumnLabel (me.peek(), 4, L"pw");
		for (long i = 1; i <= 150; i++) {
			int kind = (i - 1) / 50 + 1;
			wchar_t const *label = kind == 1 ? L"1" : kind == 2 ? L"2" : L"3";
			for (long j = 1; j <= 4; j++) {
				my data[i][j] = iris[i - 1][j - 1];
			}
			TableOfReal_setRowLabel (me.peek(), i, label);
		}
		Thing_setName (me.peek(), L"iris");
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal from iris data not created.");
	}
}

Strings TableOfReal_extractRowLabels (I) {
	iam (TableOfReal);
	try {
		autoStrings thee = Thing_new (Strings);

		if (my numberOfRows > 0) {
			thy strings = NUMvector<wchar_t *> (1, my numberOfRows);

			thy numberOfStrings = my numberOfRows;

			for (long i = 1; i <= my numberOfRows; i++) {
				wchar_t const *label = my rowLabels[i] ? my rowLabels[i] : L"?";
				thy strings[i] = Melder_wcsdup (label);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": row labels not extracted.");
	}
}

Strings TableOfReal_extractColumnLabels (I) {
	iam (TableOfReal);
	try {
		autoStrings thee = Thing_new (Strings);

		if (my numberOfColumns > 0) {
			thy strings = NUMvector<wchar_t *> (1, my numberOfColumns);
			thy numberOfStrings = my numberOfColumns;

			for (long i = 1; i <= my numberOfColumns; i++) {
				wchar_t const *label = my columnLabels[i] ? my columnLabels[i] : L"?";
				thy strings[i] = Melder_wcsdup (label);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": column labels not extracted.");
	}
}

TableOfReal TableOfReal_transpose (I) {
	iam (TableOfReal);
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfColumns, my numberOfRows);

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				thy data[j][i] = my data[i][j];
			}
		}
		NUMstrings_copyElements (my rowLabels, thy columnLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, thy rowLabels, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not transposed.");
	}
}

int TableOfReal_to_Pattern_and_Categories (I, long fromrow, long torow, long fromcol, long tocol,
        Pattern *p, Categories *c) {
	*p = 0; *c = 0;
	try {
		iam (TableOfReal);
		long ncol = my numberOfColumns, nrow = my numberOfRows;

		if (fromrow == torow && fromrow == 0) {
			fromrow = 1; torow = nrow;
		} else if (fromrow > 0 && fromrow <= nrow && torow == 0) {
			torow = nrow;
		} else if (! (fromrow > 0 && torow <= nrow && fromrow <= torow)) {
			Melder_throw ("Invalid row selection.");
		}
		if (fromcol == tocol && fromcol == 0) {
			fromcol = 1; tocol = ncol;
		} else if (fromcol > 0 && fromcol <= ncol && tocol == 0) {
			tocol = ncol;
		} else if (! (fromcol > 0 && tocol <= ncol && fromcol <= tocol)) {
			Melder_throw ("Invalid column selection.");
		}

		nrow = torow - fromrow + 1;
		ncol = tocol - fromcol + 1;
		autoPattern ap = Pattern_create (nrow, ncol);
		autoCategories ac = Categories_create ();

		long row = 1;
		for (long i = fromrow; i <= torow; i++, row++) {
			wchar_t const *s = my rowLabels[i] ? my rowLabels[i] : L"?";
			autoSimpleString item = SimpleString_create (s);
			Collection_addItem (ac.peek(), item.transfer());
			long col = 1;
			for (long j = fromcol; j <= tocol; j++, col++) {
				ap -> z[row][col] = my data[i][j];
			}
		}
		*p = ap.transfer();
		*c = ac.transfer();
		return 1;
	} catch (MelderError) {
		Melder_throw ("Pattern and Categories not created from TableOfReal.");
	}
}

void TableOfReal_getColumnExtrema (I, long col, double *min, double *max) {
	*min = NUMundefined; *max = NUMundefined;
	iam (TableOfReal);
	if (col < 1 || col > my numberOfColumns) {
		Melder_throw ("Invalid column number.");
	}
	*min = *max = my data[1][col];
	for (long i = 2; i <= my numberOfRows; i++) {
		if (my data[i][col] > *max) {
			*max = my data[i][col];
		} else if (my data[i][col] < *min) {
			*min = my data[i][col];
		}
	}
}

void TableOfReal_drawRowsAsHistogram (I, Graphics g, const wchar_t *rows, long colb, long cole, double ymin,
                                      double ymax, double xoffsetFraction, double interbarFraction, double interbarsFraction, const wchar_t *greys, int garnish) {
	iam (TableOfReal);

	if (colb >= cole) {
		colb = 1; cole = my numberOfColumns;
	}
	if (colb <= cole && (colb < 1 || cole > my numberOfColumns)) {
		Melder_throw ("Invalid columns");
	}

	long nrows;
	autoNUMvector<double> irows (NUMstring_to_numbers (rows, &nrows), 1);
	for (long i = 1; i <= nrows; i++) {
		long irow = irows[i];
		if (irow < 0 || irow > my numberOfRows) {
			Melder_throw ("Invalid row (", irow, ").");
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

	Graphics_setWindow (g, 0, 1, ymin, ymax);
	Graphics_setInner (g);

	long ncols = cole - colb + 1;
	double bar_width = 1 / (ncols * nrows + 2 * xoffsetFraction + (ncols - 1) * interbarsFraction + ncols * (nrows - 1) * interbarFraction);
	double dx = (interbarsFraction + nrows + (nrows - 1) * interbarFraction) * bar_width;

	for (long i = 1; i <= nrows; i++) {
		long irow = irows[i];
		double xb = xoffsetFraction * bar_width + (i - 1) * (1 + interbarFraction) * bar_width;

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
				Graphics_setGrey (g, 0); /* Black */
				Graphics_rectangle (g, x1, x2, y1, y2);
			}
			x1 += dx;
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		double xb = (xoffsetFraction + 0.5 * (nrows + (nrows - 1) * interbarFraction)) * bar_width;
		for (long j = colb; j <= cole; j++) {
			if (my columnLabels[j]) {
				Graphics_markBottom (g, xb, 0, 0, 0, my columnLabels[j]);
			}
			xb += dx;
		}
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

void TableOfReal_drawBiplot (I, Graphics g, double xmin, double xmax, double ymin, double ymax, double sv_splitfactor, int labelsize, int garnish) {
	iam (TableOfReal);
	long nr = my numberOfRows, nc = my numberOfColumns, nPoints = nr + nc;
	int fontsize = Graphics_inqFontSize (g);

	autoSVD svd = SVD_create (nr, nc);

	NUMmatrix_copyElements (my data, svd -> u, 1, nr, 1, nc);
	NUMcentreColumns (svd -> u, 1, nr, 1, nc, 0);

	SVD_compute (svd.peek());
	long numberOfZeroed = SVD_zeroSmallSingularValues (svd.peek(), 0);

	long nmin = MIN (nr, nc) - numberOfZeroed;
	if (nmin < 2) {
		Melder_throw ("There must be at least two (independent) columns in the table.");
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
		ymax += 1;
		ymin -= 1;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	if (labelsize > 0) {
		Graphics_setFontSize (g, labelsize);
	}
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= nPoints; i++) {
		wchar_t const *label;
		if (i <= nr) {
			label = my rowLabels[i];
			if (label == NULL) {
				label = L"?__r_";
			}
		} else {
			label = my columnLabels[i - nr];
			if (label == NULL) {
				label = L"?__c_";
			}
		}
		Graphics_text (g, x[i], y[i], label);
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}

	if (labelsize > 0) {
		Graphics_setFontSize (g, fontsize);
	}
}

void TableOfReal_drawBoxPlots (I, Graphics g, long rowmin, long rowmax, long colmin, long colmax, double ymin, double ymax, int garnish) {
	iam (TableOfReal);

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

	for (long j = colmin; j <= colmax; j++) {
		double x = j, r = 0.05, w = 0.2, t;
		long ndata = 0;

		for (long i = 1; i <= numberOfRows; i++) {
			if ( (t = my data[rowmin + i - 1][j]) != NUMundefined) {
				data[++ndata] = t;
			}
		}
		Graphics_boxAndWhiskerPlot (g, data.peek(), ndata, x, r, w, ymin, ymax);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		for (long j = colmin; j <= colmax; j++) {
			if (my columnLabels && my columnLabels[j] && my columnLabels[j][0]) {
				Graphics_markBottom (g, j, 0, 1, 0, my columnLabels [j]);
			}
		}
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

int TableOfReal_equalLabels (I, thou, int rowLabels, int columnLabels) {
	iam (TableOfReal); thouart (TableOfReal);
	Melder_assert (rowLabels || columnLabels);
	if (rowLabels) {
		if (my numberOfRows != thy numberOfRows) {
			return 0;
		}
		if (my rowLabels == thy rowLabels) {
			return 1;
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			if (Melder_wcscmp (my rowLabels[i], thy rowLabels[i])) {
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
			if (Melder_wcscmp (my columnLabels[i], thy columnLabels[i]) != 0) {
				return 0;
			}
		}
	}
	return 1;
}

void TableOfReal_copyLabels (I, thou, int rowOrigin, int columnOrigin) {
	iam (TableOfReal);
	thouart (TableOfReal);

	if (rowOrigin == 1) {
		if (my numberOfRows != thy numberOfRows) {
			Melder_throw ("#rows1 must equal #rows2");
		}
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, thy numberOfRows);
	} else if (rowOrigin == -1) {
		if (my numberOfColumns != thy numberOfRows) {
			Melder_throw ("#columns1 must equal #rows2.");
		}
		NUMstrings_copyElements (my columnLabels, thy rowLabels, 1, thy numberOfRows);
	}
	if (columnOrigin == 1) {
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw ("#columns1 must equal #columns2.");
		}
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, thy numberOfColumns);
	} else if (columnOrigin == -1) {
		if (my numberOfRows != thy numberOfColumns) {
			Melder_throw ("#rows1 must equal #columns2");
		}
		NUMstrings_copyElements (my rowLabels, thy columnLabels, 1, thy numberOfColumns);
	}
}

void TableOfReal_labelsFromCollectionItemNames (I, thou, int row, int column) {
	iam (TableOfReal);
	try {
		thouart (Collection);

		if (row) {
			Melder_assert (my numberOfRows == thy size);
			for (long i = 1; i <= my numberOfRows; i++) {
				wchar_t const *name = Thing_getName ( (Thing) thy item[i]);
				TableOfReal_setRowLabel (me, i, name);
			}
		}
		if (column) {
			Melder_assert (my numberOfColumns == thy size);
			for (long i = 1; i <= my numberOfColumns; i++) {
				wchar_t const *name = Thing_getName ( (Thing) thy item[i]);
				TableOfReal_setColumnLabel (me, i, name);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": labels not changed.");
	}
}

void TableOfReal_centreColumns (I) {
	iam (TableOfReal);
	NUMcentreColumns (my data, 1, my numberOfRows, 1, my numberOfColumns, NULL);
}

void TableOfReal_and_Categories_setRowLabels (I, Categories thee) {
	iam (TableOfReal);
	try {

		if (my numberOfRows != thy size) {
			Melder_throw ("The number of items in both objects must be equal.");
		}

		/*
			If anything goes wrong we must leave the Table intact. We first copy the Categories, swap the labels
				and then delete the newly created categories.
		*/

		autoCategories c = Data_copy (thee);

		for (long i = 1; i <= my numberOfRows; i++) {
			SimpleString s = (SimpleString) c -> item[i];
			wchar_t *t = s -> string;
			s -> string = my rowLabels[i];
			my rowLabels[i] = t;
		}
	} catch (MelderError) {
		Melder_throw (me, ": row labels not set from categories.");
	}
}

void TableOfReal_centreColumns_byRowLabel (I) {
	iam (TableOfReal);
	wchar_t *label = my rowLabels[1];
	long index = 1;

	for (long i = 2; i <= my numberOfRows; i++) {
		wchar_t *li = my rowLabels[i];
		if (Melder_wcscmp (li, label) != 0) {
			NUMcentreColumns (my data, index, i - 1, 1, my numberOfColumns, 0);
			label = li; index = i;
		}
	}
	NUMcentreColumns (my data, index, my numberOfRows, 1, my numberOfColumns, NULL);
}

double TableOfReal_getRowSum (I, long index) {
	iam (TableOfReal);

	if (index < 1 || index > my numberOfRows) {
		return NUMundefined;
	}

	double sum = 0;
	for (long j = 1; j <= my numberOfColumns; j++) {
		sum += my data[index][j];
	}
	return sum;
}

double TableOfReal_getColumnSumByLabel (I, const wchar_t *label) {
	iam (TableOfReal);
	long index = TableOfReal_columnLabelToIndex (me, label);
	if (index < 1) {
		Melder_throw ("There is no \"", label, "\" column label.");
	}
	return TableOfReal_getColumnSum (me, index);
}

double TableOfReal_getRowSumByLabel (I, const wchar_t *label) {
	iam (TableOfReal);
	long index = TableOfReal_rowLabelToIndex (me, label);
	if (index < 1) {
		Melder_throw ("There is no \"", label, "\" row label.");
	}
	return TableOfReal_getRowSum (me, index);
}

double TableOfReal_getColumnSum (I, long index) {
	iam (TableOfReal);

	if (index < 1 || index > my numberOfColumns) {
		return NUMundefined;
	}

	double sum = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		sum += my data[i][index];
	}
	return sum;
}

double TableOfReal_getGrandSum (I) {
	iam (TableOfReal);
	double sum = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			sum += my data[i][j];
		}
	}
	return sum;
}

void TableOfReal_centreRows (I) {
	iam (TableOfReal);
	NUMcentreRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_doubleCentre (I) {
	iam (TableOfReal);
	NUMdoubleCentre (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeColumns (I, double norm) {
	iam (TableOfReal);
	NUMnormalizeColumns (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_normalizeRows (I, double norm) {
	iam (TableOfReal);
	NUMnormalizeRows (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_standardizeColumns (I) {
	iam (TableOfReal);
	NUMstandardizeColumns (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_standardizeRows (I) {
	iam (TableOfReal);
	NUMstandardizeRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeTable (I, double norm) {
	iam (TableOfReal);
	NUMnormalize (my data, my numberOfRows, my numberOfColumns, norm);
}

double TableOfReal_getTableNorm (I) {
	iam (TableOfReal);
	double sumsq = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			sumsq += my data[i][j] * my data[i][j];
		}
	}
	return sqrt (sumsq);
}

int TableOfReal_checkPositive (I) {
	iam (TableOfReal);
	long negative = 0;

	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (my data[i][j] < 0) {
				negative ++; break;
			}
		}
	}
	return negative == 0;
}

/* NUMundefined ??? */
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

void TableOfReal_drawScatterPlotMatrix (I, Graphics g, long colb, long cole, double fractionWhite) {
	iam (TableOfReal);
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

	Graphics_setWindow (g, 0, n, 0, n);
	Graphics_setInner (g);
	Graphics_line (g, 0, n, n, n);
	Graphics_line (g, 0, 0, 0, n);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= n; i++) {
		long xcol, ycol = colb + i - 1;
		wchar_t const  *mark;
		wchar label[20];
		Graphics_line (g, 0, n - i, n, n - i);
		Graphics_line (g, i, n, i, 0);
		for (long j = 1; j <= n; j++) {
			xcol = colb + j - 1;
			if (i == j) {
				mark = my columnLabels[xcol];
				if (! mark) {
					swprintf (label, 20, L"Column %ld", xcol); mark = label;
				}
				Graphics_text (g, j - 0.5, n - i + 0.5, mark);
			} else {
				for (long k = 1; k <= m; k++) {
					double x = j - 1 + (my data[k][xcol] - xmin[xcol]) / (xmax[xcol] - xmin[xcol]);
					double y = n - i + (my data[k][ycol] - xmin[ycol]) / (xmax[ycol] - xmin[ycol]);
					mark = EMPTY_STRING (my rowLabels[k]) ? L"+" : my rowLabels[k];
					Graphics_text (g, x, y, mark);
				}
			}
		}
	}
	Graphics_unsetInner (g);
}

void TableOfReal_drawAsSquares_area (TableOfReal me, Graphics g, double zmin, double zmax, double cellSizeFactor, int randomFillOrder, int garnish) {
	try {
		cellSizeFactor = cellSizeFactor <= 0 ? 1 : cellSizeFactor;
		if (zmin == 0 && zmax == 0) {
			NUMmatrix_extrema<double> (my data, 1, my numberOfRows, 1, my numberOfColumns, &zmin, &zmax);
		}
		double xmin = 0, xmax = my numberOfColumns + 1, ymin = 0, ymax = my numberOfRows + 1;
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		Graphics_matrixAsSquares (g, my data, my numberOfRows, my numberOfColumns, zmin, zmax, cellSizeFactor, randomFillOrder);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottomEvery (g, 1, 1, false, true, false);
			Graphics_marksLeftEvery (g, 1, 1, false, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void TableOfReal_drawScatterPlot (I, Graphics g, long icx, long icy, long rowb, long rowe, double xmin, double xmax, double ymin, double ymax, int labelSize, int useRowLabels, const wchar_t *label, int garnish) {
	iam (TableOfReal);
	double m = my numberOfRows, n = my numberOfColumns;
	int fontSize = Graphics_inqFontSize (g);

	if (icx < 1 || icx > n || icy < 1 || icy > n) {
		return;
	}
	if (rowb < 1) {
		rowb = 1;
	}
	if (rowe > m) {
		rowe = m;
	}
	if (rowe <= rowb) {
		rowb = 1; rowe = m;
	}

	if (xmax == xmin) {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icx, & xmin, & xmax);
		double tmp = xmax - xmin == 0 ? 0.5 : 0.0;
		xmin -= tmp; xmax += tmp;
	}
	if (ymax == ymin) {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icy, & ymin, & ymax);
		double tmp = ymax - ymin == 0 ? 0.5 : 0.0;
		ymin -= tmp; ymax += tmp;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);

	long noLabel = 0;
	for (long i = rowb; i <= rowe; i++) {
		double x = my data[i][icx], y = my data[i][icy];

		if ( ( (xmin < xmax && x >= xmin && x <= xmax) || (xmin > xmax && x <= xmin && x >= xmax)) &&
		        ( (ymin < ymax && y >= ymin && y <= ymax) || (ymin > ymax && y <= ymin && y >= ymax))) {
			const wchar_t *plotLabel = useRowLabels ? my rowLabels[i] : label;
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
				Graphics_textBottom (g, 1, my columnLabels[icx]);
			}
			Graphics_marksBottom (g, 2, 1, 1, 0);
		} else {
			if (my columnLabels[icx]) {
				Graphics_textTop (g, 1, my columnLabels[icx]);
			}
			Graphics_marksTop (g, 2, 1, 1, 0);
		}
		if (xmin < xmax) {
			if (my columnLabels[icy]) {
				Graphics_textLeft (g, 1, my columnLabels[icy]);
			}
			Graphics_marksLeft (g, 2, 1, 1, 0);
		} else {
			if (my columnLabels[icy]) {
				Graphics_textRight (g, 1, my columnLabels[icy]);
			}
			Graphics_marksRight (g, 2, 1, 1, 0);
		}
	}
	if (noLabel > 0) {
		Melder_warning (Melder_integer (noLabel), L" from ", Melder_integer (my numberOfRows), L" labels are "
		                "not visible because they are empty or they contain only spaces or non-printable characters");
	}
}

/****************  TABLESOFREAL **************************************/

Thing_implement (TablesOfReal, Ordered, 0);

void TablesOfReal_init (I, ClassInfo klas) {
	iam (TablesOfReal);
	Ordered_init (me, klas, 10);
}

TablesOfReal TablesOfReal_create () {
	try {
		autoTablesOfReal me = Thing_new (TablesOfReal);
		TablesOfReal_init (me.peek(), classTableOfReal);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TablesOfReal not created.");
	}
}

TableOfReal TablesOfReal_sum (I) {
	iam (TablesOfReal);
	try {
		if (my size <= 0) {
			return 0;
		}
		autoTableOfReal thee = Data_copy ( (TableOfReal) my item[1]);

		for (long i = 2; i <= my size; i++) {
			TableOfReal him = (TableOfReal) my item[i];
			if (thy numberOfRows != his numberOfRows || thy numberOfColumns != his numberOfColumns || ! TableOfReal_equalLabels (thee.peek(), him, 1, 1)) {
				Melder_throw ("Dimensions or labels differ for table 1 and ", i, ".");
			}
			for (long j = 1; j <= thy numberOfRows; j++) {
				for (long k = 1; k <= thy numberOfColumns; k++) {
					thy data[j][k] += his data[j][k];
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": sum not created.");
	}
}

int TablesOfReal_checkDimensions (I) {
	iam (TablesOfReal);
	if (my size < 2) {
		return 1;
	}
	TableOfReal t1 = (TableOfReal) my item[1];
	for (long i = 2; i <= my size; i++) {
		TableOfReal t = (TableOfReal) my item[i];
		if (t -> numberOfColumns != t1 -> numberOfColumns ||
		        t -> numberOfRows != t1 -> numberOfRows) {
			return 0;
		}
	}
	return 1;
}

double TableOfReal_getColumnQuantile (I, long col, double quantile) {
	try {
		iam (TableOfReal);

		if (col < 1 || col > my numberOfColumns) {
			return NUMundefined;
		}
		autoNUMvector<double> values (1, my numberOfRows);

		for (long i = 1; i <= my numberOfRows; i++) {
			values[i] = my data[i][col];
		}

		NUMsort_d (my numberOfRows, values.peek());
		double r = NUMquantile (my numberOfRows, values.peek(), quantile);
		return r;
	} catch (MelderError) {
		return NUMundefined;
	}
}

static TableOfReal TableOfReal_createPolsVanNieropData (int choice, int include_levels) {
	try {
		autoTable table = Table_createFromPolsVanNieropData ();

		// Default: Pols 50 males, first part of the table.

		long nrows = 50 * 12;
		long ncols = include_levels ? 6 : 3;
		long ib = 1;

		if (choice == 2) { /* Van Nierop 25 females */
			ib = nrows + 1;
			nrows = 25 * 12;
		}

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);

		for (long i = 1; i <= nrows; i++) {
			TableRow row = (TableRow) table -> rows -> item[ib + i - 1];
			TableOfReal_setRowLabel (thee.peek(), i, row -> cells[4].string);
			for (long j = 1; j <= 3; j++) {
				thy data[i][j] = Melder_atof (row -> cells[4 + j].string);
				if (include_levels) {
					thy data[i][3 + j] = Melder_atof (row -> cells[7 + j].string);
				}
			}
		}
		for (long j = 1; j <= 3; j++) {
			wchar_t const *label = table -> columnHeaders[4 + j].label;
			TableOfReal_setColumnLabel (thee.peek(), j, label);
			if (include_levels) {
				label = table -> columnHeaders[7 + j].label;
				TableOfReal_setColumnLabel (thee.peek(), 3 + j, label);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal from Pols & Van Nierop data not created.");
	}
}

TableOfReal TableOfReal_createFromPolsData_50males (int include_levels) {
	return TableOfReal_createPolsVanNieropData (1, include_levels);
}

TableOfReal TableOfReal_createFromVanNieropData_25females (int include_levels) {
	return TableOfReal_createPolsVanNieropData (2, include_levels);
}

TableOfReal TableOfReal_createFromWeeninkData (int option) {
	try {
		long nvowels = 12, ncols = 3, nrows = 10 * nvowels;

		autoTable table = Table_createFromWeeninkData ();

		long ib = option == 1 ? 1 : option == 2 ? 11 : 21; /* m f c*/
		ib = (ib - 1) * nvowels + 1;

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);

		for (long i = 1; i <= nrows; i++) {
			TableRow row = (TableRow) table -> rows -> item[ib + i - 1];
			TableOfReal_setRowLabel (thee.peek(), i, row -> cells[5].string);
			for (long j = 1; j <= 3; j++) {
				thy data[i][j] = Melder_atof (row -> cells[6 + j].string); /* Skip F0 */
			}
		}
		for (long j = 1; j <= 3; j++)  {
			wchar_t const *label = table -> columnHeaders[6 + j].label;
			TableOfReal_setColumnLabel (thee.peek(), j, label);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal from Weenink data not created.");
	}
}

TableOfReal TableOfReal_randomizeRows (TableOfReal me) {
	try {
		autoPermutation p = Permutation_create (my numberOfRows);
		Permutation_permuteRandomly_inline (p.peek(), 0, 0);
		autoTableOfReal thee = TableOfReal_and_Permutation_permuteRows (me, p.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": randomized rows not created");
	}
}

TableOfReal TableOfReal_bootstrap (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);

		// Copy column labels.

		for (long i = 1; i <= my numberOfColumns; i++) {
			if (my columnLabels[i]) {
				TableOfReal_setColumnLabel (thee.peek(), i, my columnLabels[i]);
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
				TableOfReal_setRowLabel (thee.peek(), i, my rowLabels[p]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": bootstrapped data not created.");
	}
}

void TableOfReal_changeRowLabels (I, const wchar_t *search, const wchar_t *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	iam (TableOfReal);
	try {
		autostringvector rowLabels (strs_replace (my rowLabels, 1, my numberOfRows, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp), 1, my numberOfRows);
		NUMstrings_free (my rowLabels, 1, my numberOfRows);
		my rowLabels = rowLabels.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": row labels not changed.");
	}
}

void TableOfReal_changeColumnLabels (I, const wchar_t *search, const wchar_t *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	iam (TableOfReal);
	try {
		autostringvector columnLabels (strs_replace (my columnLabels, 1, my numberOfColumns, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp), 1, my numberOfColumns);
		NUMstrings_free (my columnLabels, 1, my numberOfColumns);
		my columnLabels = columnLabels.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": column labels not changed.");
	}
}

long TableOfReal_getNumberOfLabelMatches (I, const wchar_t *search, int columnLabels, int use_regexp) {
	iam (TableOfReal);
	long nmatches = 0, numberOfLabels = my numberOfRows;
	wchar_t **labels = my rowLabels;
	regexp *compiled_regexp = 0;

	if (search == 0 || wcslen (search) == 0) {
		return 0;
	}
	if (columnLabels) {
		numberOfLabels = my numberOfColumns;
		labels = my columnLabels;
	}
	if (use_regexp) {
		const wchar_t *compileMsg;
		compiled_regexp = CompileRE ( (regularExp_CHAR *) search, &compileMsg, 0);
		if (compiled_regexp == 0) {
			Melder_throw (compileMsg);
		}
	}
	for (long i = 1; i <= numberOfLabels; i++) {
		if (labels[i] == 0) {
			continue;
		}
		if (use_regexp) {
			if (ExecRE (compiled_regexp, 0, (regularExp_CHAR *) labels[i], NULL, 0, '\0', '\0', 0, 0, 0)) {
				nmatches++;
			}
		} else if (wcsequ (labels[i], search)) {
			nmatches++;
		}
	}
	if (use_regexp) {
		free (compiled_regexp);
	}
	return nmatches;
}

void TableOfReal_drawVectors (I, Graphics g, long colx1, long coly1, long colx2, long coly2, double xmin, double xmax,
                              double ymin, double ymax, int vectype, int labelsize, int garnish) {
	iam (TableOfReal);
	long nx = my numberOfColumns, ny = my numberOfRows;
	int fontsize = Graphics_inqFontSize (g);

	if (colx1 < 1 || colx1 > nx || coly1 < 1 || coly1 > nx) {
		Melder_warning (L"The index in the \"From\" column(s) must be in range [1, ", Melder_integer (nx), L"].");
		return;
	}
	if (colx2 < 1 || colx2 > nx || coly2 < 1 || coly2 > nx) {
		Melder_warning (L"The index in the \"To\" column(s) must be in range [1, ", Melder_integer (nx), L"].");
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
		wchar_t const *mark = EMPTY_STRING (my rowLabels[i]) ? L"" : my rowLabels[i];
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
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
}

void TableOfReal_drawColumnAsDistribution (I, Graphics g, int column, double minimum, double maximum, long nBins,
        double freqMin, double freqMax, int cumulative, int garnish) {
	iam (TableOfReal);
	if (column < 1 || column > my numberOfColumns) {
		return;
	}
	autoMatrix thee = TableOfReal_to_Matrix (me);
	Matrix_drawDistribution (thee.peek(), g,  column - 0.5, column + 0.5, 0, 0, minimum, maximum, nBins, freqMin,  freqMax,  cumulative,  garnish);
	if (garnish && my columnLabels[column] != 0) {
		Graphics_textBottom (g, 1, my columnLabels[column]);
	}
}

TableOfReal TableOfReal_sortRowsByIndex (I, long *index, int reverse) {
	iam (TableOfReal);
	try {
		if (my rowLabels == 0) {
			Melder_throw ("No labels to sort");
		}

		double min, max;
		NUMvector_extrema (index, 1, my numberOfRows, &min, &max);
		if (min < 1 || max > my numberOfRows) {
			Melder_throw ("One or more indices out of range [1, ", my numberOfRows, "].");
		}
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);

		for (long i = 1; i <= my numberOfRows; i++) {
			long    myindex = reverse ? i : index[i];
			long   thyindex = reverse ? index[i] : i;
			wchar_t   *mylabel = my rowLabels[myindex];
			double  *mydata = my data[myindex];
			double *thydata = thy data[thyindex];

			// Copy the row label

			thy rowLabels[i] = Melder_wcsdup (mylabel);

			// Copy the row values

			for (long j = 1; j <= my numberOfColumns; j++) {
				thydata[j] = mydata[j];
			}
		}

		// Copy column labels.

		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not sorted by row index.");
	}
}

long *TableOfReal_getSortedIndexFromRowLabels (I) {
	iam (TableOfReal);
	try {
		autoNUMvector<long> index (1, my numberOfRows);
		NUMindexx_s (my rowLabels, my numberOfRows, index.peek());
		return index.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no sorted index created.");
	}
}

TableOfReal TableOfReal_sortOnlyByRowLabels (I) {
	iam (TableOfReal);
	try {
		autoPermutation index = TableOfReal_to_Permutation_sortRowLabels (me);
		autoTableOfReal thee = TableOfReal_and_Permutation_permuteRows (me, index.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not sorted by row labels.");
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

TableOfReal TableOfReal_meansByRowLabels (I, int expand, int stats) {
	iam (TableOfReal);
	try {
		autoTableOfReal thee = 0;
		autoNUMvector<long> index (TableOfReal_getSortedIndexFromRowLabels (me), 1);
		autoTableOfReal sorted = TableOfReal_sortRowsByIndex (me, index.peek(), 0);

		long indexi = 1, indexr = 0;
		wchar_t const *label = sorted -> rowLabels[1];
		for (long i = 2; i <= my numberOfRows; i++) {
			wchar_t const *li = sorted -> rowLabels[i];
			if (Melder_wcscmp (li, label) != 0) {
				NUMstatsColumns (sorted -> data, indexi, i - 1, 1, my numberOfColumns, stats);

				if (expand == 0) {
					indexr++;
					TableOfReal_copyOneRowWithLabel (sorted.peek(), sorted.peek(), indexi, indexr);
				}
				label = li; indexi = i;
			}
		}

		NUMstatsColumns (sorted -> data, indexi, my numberOfRows, 1, my numberOfColumns, stats);

		if (expand != 0) {
			// Now invert the table.

			wchar_t **tmp = sorted -> rowLabels; sorted -> rowLabels = my rowLabels;
			thee.reset (TableOfReal_sortRowsByIndex (sorted.peek(), index.peek(), 1));
			sorted -> rowLabels = tmp;
		} else {
			indexr++;
			TableOfReal_copyOneRowWithLabel (sorted.peek(), sorted.peek(), indexi, indexr);
			thee.reset (TableOfReal_create (indexr, my numberOfColumns));
			for (long i = 1; i <= indexr; i++) {
				TableOfReal_copyOneRowWithLabel (sorted.peek(), thee.peek(), i, i);
			}
			NUMstrings_copyElements (sorted -> columnLabels, thy columnLabels, 1, my numberOfColumns);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": means by row labels not created.");
	}
}

TableOfReal TableOfReal_rankColumns (I) {
	iam (TableOfReal);
	try {
		autoTableOfReal thee = Data_copy (me);
		NUMrankColumns (thy data, 1, thy numberOfRows, 1, thy numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": column ranks not created.");
	}
}

void TableOfReal_setSequentialColumnLabels (I, long from, long to, const wchar_t *precursor, long number, long increment) {
	iam (TableOfReal);
	if (from == 0) {
		from = 1;
	}
	if (to == 0) {
		to = my numberOfColumns;
	}
	if (from < 1 || from > my numberOfColumns || to < from || to > my numberOfColumns) {
		Melder_throw ("Wrong column indices.");
	}
	NUMstrings_setSequentialNumbering (my columnLabels, from, to, precursor, number, increment, (int) 0);
}

void TableOfReal_setSequentialRowLabels (I, long from, long to, const wchar_t *precursor, long number, long increment) {
	iam (TableOfReal);
	if (from == 0) {
		from = 1;
	}
	if (to == 0) {
		to = my numberOfRows;
	}
	if (from < 1 || from > my numberOfRows || to < from || to > my numberOfRows) {
		Melder_throw ("Wrong row indices.");
	}
	NUMstrings_setSequentialNumbering (my rowLabels, from, to, precursor, number, increment, (int) 0);
}

/* For the inheritors */
TableOfReal TableOfReal_to_TableOfReal (I) {
	iam (TableOfReal);
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not copied.");
	}
}

TableOfReal TableOfReal_choleskyDecomposition (I, int upper, int inverse) {
	iam (TableOfReal);
	try {
		char diag = 'N';
		long n = my numberOfColumns, lda = my numberOfRows, info;

		if (n != lda) {
			Melder_throw ("The table must be a square symmetric table.");
		}
		autoTableOfReal thee = Data_copy (me);

		if (upper) {
			for (long i = 2; i <= n; i++) for (long j = 1; j < i; j++) {
					thy data[i][j] = 0;
				}
		} else {
			for (long i = 1; i < n; i++) for (long j = i + 1; j <= n; j++) {
					thy data[i][j] = 0;
				}
		}
		char uplo = upper ? 'L' : 'U';
		NUMlapack_dpotf2 (&uplo, &n, &thy data[1][1], &lda, &info);
		if (info != 0) {
			Melder_throw ("dpotf2 fails");
		}

		if (inverse) {
			NUMlapack_dtrtri (&uplo, &diag, &n, &thy data[1][1], &lda, &info);
			if (info != 0) {
				Melder_throw ("dtrtri fails");
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Cholesky decomposition not performed.");
	}
}

TableOfReal TableOfReal_appendColumns (I, thou) {
	try {
		iam (TableOfReal); thouart (TableOfReal);
		long ncols = my numberOfColumns + thy numberOfColumns;
		long labeldiffs = 0;

		if (my numberOfRows != thy numberOfRows) {
			Melder_throw ("Number of rows must be equal.");
		}
		/* Stricter label checking???
			append only if
			(my rowLabels[i] == thy rowlabels[i], i=1..my numberOfRows) or
			(my rowLabels[i] == 'empty', i=1..my numberOfRows)  or
			(thy rowLabels[i] == 'empty', i=1..my numberOfRows);
			'empty':  NULL or \w*
		*/
		autoTableOfReal him = TableOfReal_create (my numberOfRows, ncols);
		NUMstrings_copyElements (my rowLabels, his rowLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, his columnLabels,  1, my numberOfColumns);
		NUMstrings_copyElements (thy columnLabels, &his columnLabels[my numberOfColumns], 1, thy numberOfColumns);
		for (long i = 1; i <= my numberOfRows; i++) {
			if (Melder_wcscmp (my rowLabels[i], thy rowLabels[i]) != 0) {
				labeldiffs++;
			}
			NUMvector_copyElements (my data[i], his data[i], 1, my numberOfColumns);
			NUMvector_copyElements (thy data[i], &his data[i][my numberOfColumns], 1, thy numberOfColumns);
		}
		if (labeldiffs > 0) {
			Melder_warning (Melder_integer (labeldiffs), L" row labels differed.");
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal with appended columns not created.");
	}
}

TableOfReal TableOfReal_appendColumnsMany (Collection me) {
	try {
		if (my size == 0) {
			Melder_throw ("No tables selected.");
		}
		TableOfReal thee = (TableOfReal) my item [1];
		long nrow = thy numberOfRows;
		long ncol = thy numberOfColumns;
		for (long itab = 2; itab <= my size; itab++) {
			thee = (TableOfReal) my item [itab];
			ncol += thy numberOfColumns;
			if (thy numberOfRows != nrow) {
				Melder_throw ("Numbers of rows in item ", itab, " differs from previous.");
			}
		}
		autoTableOfReal him = Thing_new (TableOfReal);
		TableOfReal_init (him.peek(), nrow, ncol);
		/* Unsafe: new attributes not initialized. */
		for (long irow = 1; irow <= nrow; irow++) {
			TableOfReal_setRowLabel (him.peek(), irow, thy rowLabels [irow]);
		}
		ncol = 0;
		for (long itab = 1; itab <= my size; itab++) {
			thee = (TableOfReal) my item [itab];
			for (long icol = 1; icol <= thy numberOfColumns; icol++) {
				ncol++;
				TableOfReal_setColumnLabel (him.peek(), ncol, thy columnLabels [icol]);
				for (long irow = 1; irow <= nrow; irow++) {
					his data[irow][ncol] = thy data[irow][icol];
				}
			}
		}
		Melder_assert (ncol == his numberOfColumns);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal with appended columns not created.");
	}
}

double TableOfReal_normalityTest_BHEP (I, double *h, double *tnb, double *lnmu, double *lnvar) {
	iam (TableOfReal);
	try {
		long n = my numberOfRows, p = my numberOfColumns;
		double beta = *h > 0 ? NUMsqrt1_2 / *h : NUMsqrt1_2 * pow ( (1.0 + 2 * p) / 4, 1.0 / (p + 4)) * pow (n, 1.0 / (p + 4));
		double p2 = p / 2.0;
		double beta2 = beta * beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
		double gamma = 1 + 2 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
		double delta = 1.0 + beta2 * (4 + 3 * beta2), delta2 = delta * delta;
		double prob = NUMundefined;

		if (*h <= 0) {
			*h = NUMsqrt1_2 / beta;
		}

		*tnb = *lnmu = *lnvar = NUMundefined;

		if (n < 2 || p < 1) {
			return NUMundefined;
		}

		autoCovariance thee = TableOfReal_to_Covariance (me);
		try {
			SSCP_expandLowerCholesky (thee.peek());
		} catch (MelderError) {
			*tnb = 4 * n;
		}
		{
			double djk, djj, sumjk = 0, sumj = 0;
			double b1 = beta2 / 2, b2 = b1 / (1.0 + beta2);
			/* Heinze & Wagner (1997), page 3
				We use d[j][k] = ||Y[j]-Y[k]||^2 = (Y[j]-Y[k])'S^(-1)(Y[j]-Y[k])
				So d[j][k]= d[k][j] and d[j][j] = 0
			*/
			for (long j = 1; j <= n; j++) {
				for (long k = 1; k < j; k++) {
					djk = NUMmahalanobisDistance_chi (thy lowerCholesky, my data[j], my data[k], p, p);
					sumjk += 2 * exp (-b1 * djk); // factor 2 because d[j][k] == d[k][j]
				}
				sumjk += 1; // for k == j
				djj = NUMmahalanobisDistance_chi (thy lowerCholesky, my data[j], thy centroid, p, p);
				sumj += exp (-b2 * djj);
			}
			*tnb = (1.0 / n) * sumjk - 2.0 * pow (1.0 + beta2, - p2) * sumj + n * pow (gamma, - p2); // n *
		}
		double mu = 1.0 - pow (gamma, -p2) * (1.0 + p * beta2 / gamma + p * (p + 2) * beta4 / (2 * gamma2));
		double var = 2.0 * pow (1 + 4 * beta2, -p2)
		             + 2.0 * pow (gamma,  -p) * (1.0 + 2 * p * beta4 / gamma2  + 3 * p * (p + 2) * beta8 / (4 * gamma4))
		             - 4.0 * pow (delta, -p2) * (1.0 + 3 * p * beta4 / (2 * delta) + p * (p + 2) * beta8 / (2 * delta2));
		double mu2 = mu * mu;
		*lnmu = 0.5 * log (mu2 * mu2 / (mu2 + var)); //log (sqrt (mu2 * mu2 /(mu2 + var)));
		*lnvar = sqrt (log ( (mu2 + var) / mu2));
		prob = NUMlogNormalQ (*tnb, *lnmu, *lnvar);
		return prob;
	} catch (MelderError) {
		Melder_throw (me, ": cannot determine normality.");
	}
}

TableOfReal TableOfReal_and_TableOfReal_crossCorrelations (I, thou, int by_columns, int center, int normalize) {
	iam (TableOfReal); thouart (TableOfReal);
	return by_columns ? TableOfReal_and_TableOfReal_columnCorrelations (me, thee, center, normalize) :
	       TableOfReal_and_TableOfReal_rowCorrelations (me, thee, center, normalize);
}

TableOfReal TableOfReal_and_TableOfReal_rowCorrelations (I, thou, int center, int normalize) {
	try {
		iam (TableOfReal); thouart (TableOfReal);
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw (L"Both tables must have the same number of columns.");
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
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal with row correlations not created.");
	}
}

TableOfReal TableOfReal_and_TableOfReal_columnCorrelations (I, thou, int center, int normalize) {
	try {
		iam (TableOfReal); thouart (TableOfReal);
		if (my numberOfRows != thy numberOfRows) {
			Melder_throw (L"Both tables must have the same number of rows.");
		}

		autoTableOfReal him = TableOfReal_create (my numberOfColumns, thy numberOfColumns);
		autoNUMmatrix<double> my_data (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfColumns), 1, 1);
		autoNUMmatrix<double> thy_data (NUMmatrix_copy (thy data, 1, thy numberOfRows, 1, thy numberOfColumns), 1, 1);
		if (center) {
			NUMcentreColumns (my_data.peek(), 1, my numberOfRows, 1, my numberOfColumns, NULL);
			NUMcentreColumns (thy_data.peek(), 1, thy numberOfRows, 1, thy numberOfColumns, NULL);
		}
		if (normalize) {
			NUMnormalizeColumns (my_data.peek(), my numberOfRows, my numberOfColumns, 1);
			NUMnormalizeColumns (thy_data.peek(), thy numberOfRows, thy numberOfColumns, 1);
		}
		NUMstrings_copyElements (my columnLabels, his rowLabels, 1, his numberOfRows);
		NUMstrings_copyElements (thy columnLabels, his columnLabels, 1, his numberOfColumns);

		for (long j = 1; j <= my numberOfColumns; j++) {
			for (long k = 1; k <= thy numberOfColumns; k++) {
				double ctmp = 0;
				for (long i = 1; i <= my numberOfRows; i++) {
					ctmp += my_data[i][j] * thy_data[i][k];
				}
				his data[j][k] = ctmp;
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal with column correlations not created.");
	}
}

#undef EMPTY_STRING
#undef MAX
#undef MIN

/* End of file TableOfReal_extensions.c 1869*/
