/* TableOfReal_extensions.cpp
 *
 * Copyright (C) 1993-2020 David Weenink, 2017 Paul Boersma
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

#include "Covariance.h"
#include "Graphics_extensions.h"
#include "Matrix_extensions.h"
#include "NUMlapack.h"
#include "NUM2.h"
#include "SVD.h"
#include "Table_extensions.h"
#include "TableOfReal_extensions.h"
#include "TableOfReal_and_Permutation.h"
#include "regularExp.h"
#include "Formula.h"

#define EMPTY_STRING(s) (! (s) || s [0] == '\0')

#define Graphics_ARROW 1
#define Graphics_TWOWAYARROW 2
#define Graphics_LINE 3

static autoTableOfReal TableOfReal_TableOfReal_columnCorrelations (TableOfReal me, TableOfReal thee, bool center, bool normalize);
static autoTableOfReal TableOfReal_TableOfReal_rowCorrelations (TableOfReal me, TableOfReal thee, bool center, bool normalize);

integer TableOfReal_getColumnIndexAtMaximumInRow (TableOfReal me, integer rowNumber) {
	return ( rowNumber > 0 && rowNumber <= my numberOfRows ?
		NUMmaxPos (my data.row (rowNumber)) : 0 );
}

conststring32 TableOfReal_getColumnLabelAtMaximumInRow (TableOfReal me, integer rowNumber) {
	integer columnNumber = TableOfReal_getColumnIndexAtMaximumInRow (me, rowNumber);
	return my v_getColStr (columnNumber);
}

void TableOfReal_copyOneRowWithLabel (TableOfReal me, TableOfReal thee, integer myrow, integer thyrow) {
	try {
		if (me == thee && myrow == thyrow)
			return;
		Melder_require (myrow > 0 && myrow <= my numberOfRows && thyrow > 0 && thyrow <= thy numberOfRows && my numberOfColumns == thy numberOfColumns,
			U"The dimensions do not fit.");
		
		thy rowLabels [thyrow] = Melder_dup (my rowLabels [myrow].get());
		thy data.row (thyrow) <<= my data.row (myrow); 
	} catch (MelderError) {
		Melder_throw (me, U": row ", myrow, U" not copied to ", thee);
	}
}

bool TableOfReal_hasRowLabels (TableOfReal me) {
	if (! my rowLabels)
		return false;
	for (integer i = 1; i <= my numberOfRows; i ++)
		if (EMPTY_STRING (my rowLabels [i]))
			return false;
	return true;
}

bool TableOfReal_hasColumnLabels (TableOfReal me) {
	if (! my columnLabels)
		return false;
	for (integer i = 1; i <= my numberOfColumns; i ++)
		if (EMPTY_STRING (my columnLabels [i]))
			return false;
	return true;
}

autoTableOfReal TableOfReal_createIrisDataset () {
	const double iris [150] [4] = {
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
		for (integer i = 1; i <= 150; i ++) {
			const integer kind = (i - 1) / 50 + 1;
			char32 const *label = kind == 1 ? U"1" : kind == 2 ? U"2" : U"3";
			for (integer j = 1; j <= 4; j ++)
				my data [i] [j] = iris [i - 1] [j - 1];
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
			thy strings = autoSTRVEC (my numberOfRows);
			thy numberOfStrings = my numberOfRows;
			for (integer i = 1; i <= my numberOfRows; i ++) {
				const conststring32 label = my rowLabels [i] ? my rowLabels [i].get() : U"?";
				thy strings [i] = Melder_dup (label);
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
			thy strings = autoSTRVEC (my numberOfColumns);
			thy numberOfStrings = my numberOfColumns;

			for (integer i = 1; i <= my numberOfColumns; i ++) {
				const conststring32 label = my columnLabels [i] ? my columnLabels [i].get() : U"?";
				thy strings [i] = Melder_dup (label);
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
		thy data.all() <<= my data.transpose();
		thy columnLabels.all() <<= my rowLabels.all();
		thy rowLabels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not transposed.");
	}
}

void TableOfReal_to_PatternList_and_Categories (TableOfReal me, integer fromrow, integer torow, integer fromcol, integer tocol,
	autoPatternList *out_p, autoCategories *out_c)
{
	try {
		integer ncols = my numberOfColumns, nrows = my numberOfRows;
		fromrow = fromrow == 0 ? 1 : fromrow;
		torow = torow == 0 ? nrows : torow;
		Melder_require (fromrow > 0 && fromrow <= torow && torow <= nrows,
			U"Invalid row selection.");
		fromcol = fromcol == 0 ? 1 : fromcol;
		tocol = tocol == 0 ? ncols : tocol;
		
		Melder_require (fromcol > 0 && fromcol <= tocol && tocol <= ncols,
			U"Invalid column selection."); 

		nrows = torow - fromrow + 1;
		ncols = tocol - fromcol + 1;
		autoPatternList ap = PatternList_create (nrows, ncols);
		autoCategories ac = Categories_create ();

		integer row = 1;
		for (integer i = fromrow; i <= torow; i ++, row ++) {
			const char32 *s = my rowLabels [i] ? my rowLabels [i].get() : U"?";
			autoSimpleString item = SimpleString_create (s);
			ac -> addItem_move (item.move());
			integer col = 1;
			for (integer j = fromcol; j <= tocol; j ++, col ++)
				ap -> z [row] [col] = my data [i] [j];
		}
		if (out_p)
			*out_p = ap.move();
		if (out_c)
			*out_c = ac.move();
	} catch (MelderError) {
		Melder_throw (U"PatternList and Categories not created from TableOfReal.");
	}
}

void TableOfReal_getColumnExtrema (TableOfReal me, integer col, double *out_min, double *out_max) {
	Melder_require (col > 0 && col <= my numberOfColumns,
		U"Invalid column number.");
	if (out_min)
		*out_min = NUMmin (my data.column (col));
	if (out_max)
		*out_max = NUMmax (my data.column (col));
}

void TableOfReal_drawRowsAsHistogram (TableOfReal me, Graphics g, conststring32 rows, integer colb, integer cole, double ymin,
	double ymax, double xoffsetFraction, double interbarFraction, double interbarsFraction, conststring32 greys, bool garnish) {
	colb = colb == 0 ? 1 : colb;
	cole = cole == 0 ? my numberOfColumns : cole;

	Melder_require (colb > 0 && colb <= cole && cole <= my numberOfColumns,
		U"Invalid columns");

	autoVEC irows = newVECfromString (rows);
	for (integer i = 1; i <= irows.size; i ++) {
		const integer irow = Melder_ifloor (irows [i]);
		Melder_require (irow > 0 && irow <= my numberOfRows,
			U"Invalid row (", irow, U").");
		if (ymin >= ymax) {
			double min, max;
			NUMextrema (my data.row (irow).part (colb, cole), & min, & max);
			if (i > 1) {
				if (min < ymin)
					ymin = min;
				if (max > ymax)
					ymax = max;
			} else {
				ymin = min;
				ymax = max;
			}
		}
	}

	autoVEC igreys = newVECfromString (greys);

	Graphics_setWindow (g, 0.0, 1.0, ymin, ymax);
	Graphics_setInner (g);

	const integer ncols = cole - colb + 1, nrows = irows.size;
	const double bar_width = 1.0 / (ncols * nrows + 2.0 * xoffsetFraction + (ncols - 1) * interbarsFraction + ncols * (nrows - 1) * interbarFraction);
	const double dx = (interbarsFraction + nrows + (nrows - 1) * interbarFraction) * bar_width;

	for (integer i = 1; i <= nrows; i ++) {
		const integer irow = Melder_ifloor (irows [i]);
		const double xb = xoffsetFraction * bar_width + (i - 1) * (1.0 + interbarFraction) * bar_width;

		const double grey = i <= igreys.size ? igreys [i] : igreys [igreys.size];
		double x1 = xb;
		for (integer j = colb; j <= cole; j ++) {
			const double x2 = x1 + bar_width;
			double y2 = my data [irow] [j];
			if (y2 > ymin) {
				if (y2 > ymax)
					y2 = ymax;
				Graphics_setGrey (g, grey);
				Graphics_fillRectangle (g, x1, x2, ymin, y2);
				Graphics_setGrey (g, 0.0);   // black
				Graphics_rectangle (g, x1, x2, ymin, y2);
			}
			x1 += dx;
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		double xb = (xoffsetFraction + 0.5 * (nrows + (nrows - 1) * interbarFraction)) * bar_width;
		for (integer j = colb; j <= cole; j ++) {
			if (my columnLabels [j])
				Graphics_markBottom (g, xb, false, false, false, my columnLabels [j].get());
			xb += dx;
		}
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void TableOfReal_drawBiplot (TableOfReal me, Graphics g, double xmin, double xmax, double ymin, double ymax, double sv_splitfactor, int labelsize, bool garnish) {
	const integer nr = my numberOfRows, nc = my numberOfColumns, nPoints = nr + nc;
	const double fontsize = Graphics_inqFontSize (g);

	autoSVD svd = SVD_create (nr, nc);

	svd -> u.all() <<= my data.all();
	centreEachColumn_MAT_inout (svd -> u.get());

	SVD_compute (svd.get());
	const integer numberOfZeroed = SVD_zeroSmallSingularValues (svd.get(), 0.0);

	const integer nmin = std::min (nr, nc) - numberOfZeroed;
	Melder_require (nmin > 1,
		U"There should be at least two (independent) columns in the table.");

	autoVEC x = raw_VEC (nPoints);
	autoVEC y = raw_VEC (nPoints);

	const double lambda1 = pow (svd -> d [1], sv_splitfactor);
	const double lambda2 = pow (svd -> d [2], sv_splitfactor);
	x.part (1, nr) <<= svd -> u.column (1)  *  lambda1;
	y.part (1, nr) <<= svd -> u.column (2)  *  lambda2;
	
	const double lambda3 = svd -> d [1] / lambda1;
	const double lambda4 = svd -> d [2] / lambda2;
	x.part (nr + 1, nPoints) <<= svd -> v.column (1)  *  lambda3;
	y.part (nr + 1, nPoints) <<= svd -> v.column (2)  *  lambda4;
	
	if (xmax <= xmin)
		NUMextrema (x.get(), & xmin, & xmax);

	if (xmax <= xmin) {
		xmax += 1.0;
		xmin -= 1.0;
	}
	if (ymax <= ymin)
		NUMextrema (y.get(), & ymin, & ymax);
	if (ymax <= ymin) {
		ymax += 1.0;
		ymin -= 1.0;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	if (labelsize > 0)
		Graphics_setFontSize (g, labelsize);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);

	for (integer i = 1; i <= nPoints; i ++) {
		char32 const *label;
		if (i <= nr) {
			label = my rowLabels [i].get();
			if (! label)
				label = U"?__r_";
		} else {
			label = my columnLabels [i - nr].get();
			if (! label)
				label = U"?__c_";
		}
		Graphics_text (g, x [i], y [i], label);
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_marksBottom (g, 2, true, true, false);
	}

	if (labelsize > 0)
		Graphics_setFontSize (g, fontsize);
}

void TableOfReal_drawBoxPlots (TableOfReal me, Graphics g, integer rowmin, integer rowmax, integer colmin, integer colmax, double ymin, double ymax, bool garnish) {
	if (rowmax < rowmin || rowmax < 1) {
		rowmin = 1;
		rowmax = my numberOfRows;
	}
	if (rowmin < 1)
		rowmin = 1;
	if (rowmax > my numberOfRows)
		rowmax = my numberOfRows;

	const integer numberOfRows = rowmax - rowmin + 1;
	if (colmax < colmin || colmax < 1) {
		colmin = 1;
		colmax = my numberOfColumns;
	}
	if (colmin < 1)
		colmin = 1;
	if (colmax > my numberOfColumns)
		colmax = my numberOfColumns;

	if (ymax <= ymin) {
		MelderRealRange yrange = NUMextrema (my data.part (rowmin, rowmax, colmin, colmax));
		ymin = yrange.min;
		ymax = yrange.max;
	}

	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, ymin, ymax);
	Graphics_setInner (g);

	autoVEC data = raw_VEC (numberOfRows);
	for (integer j = colmin; j <= colmax; j ++) {
		const double x = j, r = 0.05, w = 0.2;
		integer ndata = 0;
		
		for (integer i = 1; i <= numberOfRows; i ++) {
			if (isdefined (my data [rowmin + i - 1] [j]))
				data [ ++ ndata] = my data [rowmin + i - 1] [j];
		}
		Graphics_boxAndWhiskerPlot (g, data.get(), x, r, w, ymin, ymax);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		for (integer j = colmin; j <= colmax; j ++) {
			if (my columnLabels && my columnLabels [j] && my columnLabels [j] [0])
				Graphics_markBottom (g, j, false, true, false, my columnLabels [j].get());
		}
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void TableOfReal_copyLabels (TableOfReal me, TableOfReal thee, int rowOrigin, int columnOrigin) {
	if (rowOrigin == 1) {
		Melder_require (my numberOfRows == thy numberOfRows,
			U"Both tables must have the same number of rows.");
		thy rowLabels.all() <<= my rowLabels.all();
	} else if (rowOrigin == -1) {
		Melder_require (my numberOfColumns == thy numberOfRows,
			U"Both tables must have the same number of columns.");
		thy rowLabels.all() <<= my columnLabels.all();
	}
	if (columnOrigin == 1) {
		Melder_require (my numberOfColumns == thy numberOfColumns,
			U"Both tables must have the same number of columns.");
		thy columnLabels.all() <<= my columnLabels.all();
	} else if (columnOrigin == -1) {
		Melder_require (my numberOfRows == thy numberOfColumns,
			U"Both tables must have the same number of rows.");
		thy columnLabels.all() <<= my rowLabels.all();
	}
}

void TableOfReal_setLabelsFromCollectionItemNames (TableOfReal me, Collection thee, bool setRowLabels, bool setColumnLabels) {
	try {
		if (setRowLabels) {
			Melder_assert (my numberOfRows == thy size);
			for (integer i = 1; i <= my numberOfRows; i ++) {
				const conststring32 name = Thing_getName (thy at [i]);
				TableOfReal_setRowLabel (me, i, name);
			}
		}
		if (setColumnLabels) {
			Melder_assert (my numberOfColumns == thy size);
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				const conststring32 name = Thing_getName (thy at [i]);
				TableOfReal_setColumnLabel (me, i, name);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

void TableOfReal_centreColumns (TableOfReal me) {
	centreEachColumn_MAT_inout (my data.get());
}

void TableOfReal_Categories_setRowLabels (TableOfReal me, Categories thee) {
	try {
		Melder_require (my numberOfRows == thy size,
			U"The number of items in both objects should be equal.");
		/*
			Create without change.
		*/
		autoCategories categories_copy = Data_copy (thee);
		/*
			Change without error.
		*/
		for (integer i = 1; i <= my numberOfRows; i ++)
			my rowLabels [i] = categories_copy->at [i] -> string.move();
	} catch (MelderError) {
		Melder_throw (me, U": row labels not set from categories.");
	}
}

void TableOfReal_centreColumns_byRowLabel (TableOfReal me) {
	conststring32 label = my rowLabels [1].get();
	integer index = 1;
	for (integer i = 2; i <= my numberOfRows; i ++) {
		const conststring32 li = my rowLabels [i].get();
		if (! Melder_equ (li, label)) {
			centreEachColumn_MAT_inout (my data.horizontalBand (index, i - 1));
			label = li;
			index = i;
		}
	}
	centreEachColumn_MAT_inout (my data.horizontalBand (index, my numberOfRows));
}

double TableOfReal_getRowSum (TableOfReal me, integer rowNumber) {
	Melder_require (rowNumber > 0 && rowNumber <= my numberOfRows,
		U"Row number not in valid range.");
	return NUMsum (my data.row (rowNumber));
}

double TableOfReal_getColumnSum (TableOfReal me, integer columnNumber) {
	Melder_require (columnNumber > 0 && columnNumber <= my numberOfColumns,
		U"Column number not in valid range.");
	return NUMsum (my data.column (columnNumber));
}

double TableOfReal_getRowSumByLabel (TableOfReal me, conststring32 rowLabel) {
	const integer rowNumber = TableOfReal_rowLabelToIndex (me, rowLabel);
	Melder_require (rowNumber > 0,
		U"There is no \"", rowLabel, U"\" row label.");
	return TableOfReal_getRowSum (me, rowNumber);
}

double TableOfReal_getColumnSumByLabel (TableOfReal me, conststring32 columnLabel) {
	const integer columnNumber = TableOfReal_columnLabelToIndex (me, columnLabel);
	Melder_require (columnNumber > 0,
		U"There is no \"", columnLabel, U"\" column label.");
	return TableOfReal_getColumnSum (me, columnNumber);
}

double TableOfReal_getGrandSum (TableOfReal me) {
	return NUMsum (my data.all());
}

void TableOfReal_centreRows (TableOfReal me) {
	centreEachRow_MAT_inout (my data.get());
}

void TableOfReal_doubleCentre (TableOfReal me) {
	doubleCentre_MAT_inout (my data.get());
}

void TableOfReal_normalizeColumns (TableOfReal me, double norm) {
		MATnormalizeRows_inplace (my data.transpose(), 2.0, norm);
}

void TableOfReal_normalizeRows (TableOfReal me, double norm) {
	MATnormalizeRows_inplace (my data.get(), 2.0, norm);
}

void TableOfReal_standardizeColumns (TableOfReal me) {
	if (my numberOfRows <= 1) {
		my data.all()  <<=  0.0;
		return;
	}
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		MelderGaussianStats stats = NUMmeanStdev (my data.column (icol));
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			my data [irow] [icol] = (my data [irow] [icol] - stats.mean) / stats.stdev;
	}
}

void TableOfReal_standardizeRows (TableOfReal me) {
	if (my numberOfColumns <= 1) {
		my data.all()  <<=  0.0;
		return;
	}
	for (integer irow = 1; irow <= my numberOfRows; irow ++) {
		MelderGaussianStats stats = NUMmeanStdev (my data.row (irow));
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			my data [irow] [icol] = (my data [irow] [icol] - stats.mean) / stats.stdev;
	}
}

void TableOfReal_normalizeTable (TableOfReal me, double norm) {
	MATnormalize_inplace (my data.get(), 2.0, norm);
}

double TableOfReal_getTableNorm (TableOfReal me) {
	return NUMnorm (my data.get(), 2.0);
}

bool TableOfReal_isNonNegative (TableOfReal me) {
	return NUMisNonNegative (my data.all());
}

void TableOfReal_drawScatterPlotMatrix (TableOfReal me, Graphics g, integer colb, integer cole, double fractionWhite) {

	if (colb == 0 && cole == 0) {
		colb = 1;
		cole = my numberOfColumns;
	} else if (cole < colb || colb < 1 || cole > my numberOfColumns)
		return;

	const integer numberOfColumns = cole - colb + 1;
	if (numberOfColumns == 1)
		return;
	autoVEC colmin = raw_VEC (numberOfColumns);
	autoVEC colmax = raw_VEC (numberOfColumns);

	for (integer j = 1; j <= numberOfColumns; j ++) {
		colmin [j] = NUMmin (my data.column (colb + j - 1));
		colmax [j] = NUMmax (my data.column (colb + j - 1));
	}

	for (integer j = 1; j <= numberOfColumns; j ++) {
		double extra = fractionWhite * fabs (colmax [j] - colmin [j]);
		if (extra == 0.0)
			extra = 0.5;
		colmin [j] -= extra;
		colmax [j] += extra;
	}

	Graphics_setWindow (g, 0.0, numberOfColumns, 0.0, numberOfColumns);
	Graphics_setInner (g);
	Graphics_line (g, 0.0, numberOfColumns, numberOfColumns, numberOfColumns);
	Graphics_line (g, 0.0, 0.0, 0.0, numberOfColumns);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);

	for (integer icol = 1; icol <= numberOfColumns; icol ++) {
		const integer ycol = colb + icol - 1;
		Graphics_line (g, 0.0, numberOfColumns - icol, numberOfColumns, numberOfColumns - icol);
		Graphics_line (g, icol, numberOfColumns, icol, 0.0);
		for (integer irow = 1; irow <= numberOfColumns; irow ++) {
			const integer xcol = colb + irow - 1;
			if (icol == irow) {
				conststring32 mark = my columnLabels [xcol].get();
				char32 label [40];
				if (! mark) {
					Melder_sprint (label, 40, U"Column ", xcol);
					mark = label;
				}
				Graphics_text (g, irow - 0.5, numberOfColumns - icol + 0.5, mark);
			} else {
				for (integer k = 1; k <= my numberOfRows; k ++) {
					const double x = irow - 1 + (my data [k] [xcol] - colmin [irow]) / (colmax [irow] - colmin [irow]);
					const double y = numberOfColumns - icol + (my data [k] [ycol] - colmin [icol]) / (colmax [icol] - colmin [icol]);
					conststring32 mark = EMPTY_STRING (my rowLabels [k]) ? U"+" : my rowLabels [k].get();
					Graphics_text (g, x, y, mark);
				}
			}
		}
	}
	Graphics_unsetInner (g);
}

void TableOfReal_drawAsScalableSquares (TableOfReal me, Graphics g, integer rowmin, integer rowmax, integer colmin, integer colmax, kGraphicsMatrixOrigin origin, double cellSizeFactor, kGraphicsMatrixCellDrawingOrder fillOrder, bool garnish) {
	try {
		NUMfixIndicesInRange (1, my numberOfRows, & rowmin, & rowmax);
		NUMfixIndicesInRange (1, my numberOfColumns, & colmin, & colmax);
		autoMatrix thee = TableOfReal_to_Matrix (me);
		Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5);
		Graphics_setInner (g);
		Matrix_drawAsSquares_inside (thee.get(), g, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5, origin, cellSizeFactor, fillOrder);
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

void TableOfReal_drawScatterPlot (TableOfReal me, Graphics g, integer icx, integer icy, integer rowb, integer rowe, double xmin, double xmax, double ymin, double ymax,
	integer labelSize, bool useRowLabels, conststring32 label, bool garnish)
{
	const double fontSize = Graphics_inqFontSize (g);

	Melder_require (icx >= 1 && icx <= my numberOfColumns,
		U"The horizontal column number should be in the range from 1 to ", my numberOfColumns, U".");
	Melder_require (icy >= 1 && icy <= my numberOfColumns,
		U"The vertical column number should be in the range from 1 to ", my numberOfColumns, U".");
	if (rowb < 1)
		rowb = 1;
	if (rowe > my numberOfRows)
		rowe = my numberOfRows;
	if (rowe <= rowb) {
		rowb = 1;
		rowe = my numberOfRows;
	}
	if (xmax == xmin) {
		MelderRealRange xrange = NUMextrema (my data.part (rowb, rowe, icx, icx));
		const double tmp = ( xrange.max == xrange.min ? 0.5 : 0.0 );
		xmin = xrange.min - tmp;
		xmax = xrange.max + tmp;
	}
	if (ymax == ymin) {
		MelderRealRange yrange = NUMextrema (my data.part (rowb, rowe, icy, icy));
		const double tmp = ( yrange.max == yrange.min ? 0.5 : 0.0 );
		ymin = yrange.min - tmp;
		ymax = yrange.max + tmp;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);

	for (integer i = rowb; i <= rowe; i ++) {
		const double x = my data [i] [icx], y = my data [i] [icy];
		if (((xmin < xmax && x >= xmin && x <= xmax) || (xmin > xmax && x <= xmin && x >= xmax)) &&
		    ((ymin < ymax && y >= ymin && y <= ymax) || (ymin > ymax && y <= ymin && y >= ymax))) {
				conststring32 mark = useRowLabels ? my rowLabels [i].get() : label;
				if (mark)
					Graphics_text (g, x, y, mark);
		}
	}

	Graphics_setFontSize (g, fontSize);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		if (ymin < ymax) {
			if (my columnLabels [icx])
				Graphics_textBottom (g, true, my columnLabels [icx].get());
			Graphics_marksBottom (g, 2, true, true, false);
		} else {
			if (my columnLabels [icx])
				Graphics_textTop (g, true, my columnLabels [icx].get());
			Graphics_marksTop (g, 2, true, true, false);
		}
		if (xmin < xmax) {
			if (my columnLabels [icy])
				Graphics_textLeft (g, true, my columnLabels [icy].get());
			Graphics_marksLeft (g, 2, true, true, false);
		} else {
			if (my columnLabels [icy])
				Graphics_textRight (g, true, my columnLabels [icy].get());
			Graphics_marksRight (g, 2, true, true, false);
		}
	}
}


#pragma mark - class TableOfRealList

autoTableOfReal TableOfRealList_sum (TableOfRealList me) {
	try {
		if (my size <= 0)
			return autoTableOfReal();

		autoTableOfReal thee = Data_copy (my at [1]);

		for (integer i = 2; i <= my size; i ++) {
			TableOfReal him = my at [i];
			Melder_require (thy numberOfRows == his numberOfRows && thy numberOfColumns == his numberOfColumns
					&& NUMequal (thy rowLabels.get(), his rowLabels.get())
					&& NUMequal (thy columnLabels.get(), his columnLabels.get()),
				U"Dimensions or labels differ for table ", i, U".");
			thy data.get()  +=  his data.get();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": sum not created.");
	}
}

bool TableOfRealList_haveIdenticalDimensions (TableOfRealList me) {
	if (my size < 2)
		return true;
	const TableOfReal t1 = my at [1];
	for (integer i = 2; i <= my size; i ++) {
		const TableOfReal t = my at [i];
		if (t -> numberOfColumns != t1 -> numberOfColumns || t -> numberOfRows != t1 -> numberOfRows)
			return false;
	}
	return true;
}

double TableOfReal_getColumnQuantile (TableOfReal me, integer columnNumber, double quantile) {
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns)
			return undefined;
		autoVEC values = column_VEC (my data.get(), columnNumber);
		sort_VEC_inout (values.get());
		return NUMquantile (values.get(), quantile);
	} catch (MelderError) {
		return undefined;
	}
}

autoTableOfReal TableOfReal_create_sandwell1987 () {
	try {
		/*
			The following numbers are the 21 (approximate) data points in Fig, 2 in Sandwell (1987).
			They were measured by djmw from a printed picture blown up by 800%.
			The following numbers are in cm measured from the left (x) and the bottom (y) of the figure.
			Vertical scale: 8.25 cm in the picture is 12 units, y [1] is at y = 0.0.
			Horizontal scale: 17.75 cm is 10 units, x [1] is at x = 0.0.
		*/
		const double x [22] = { 0.0, 0.9, 2.15, 3.5, 4.75, 5.3, 6.15, 7.15, 7.95, 8.85, 9.95, 10.15, 10.3, 11.5, 12.4, 13.3, 14.2, 15.15, 16.0, 16.85, 17.25, 18.15 };
		const double y [22] = { 0.0, 4.2, 3.5, 4.2, 5.65, 10.1, 8.5, 7.8, 7.1, 6.4, 5.65, 0.6, 5.65, 4.2, 5.65, 7.1, 6.75, 6.35, 4.2,  2.05, 4.95, 4.25 };
		const integer numberOfSamples = 21;
		autoTableOfReal thee = TableOfReal_create (numberOfSamples, 2);
		for (integer isample = 1; isample <= numberOfSamples; isample ++) {
			thy data [isample] [1] = (x [isample] - x [1]) * 10.0 / 17.75;
			thy data [isample] [2] = (y [isample] - y [1]) * 12.0 / 8.25;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sandwell (1987) table not created.");
	}
}

static autoTableOfReal TableOfReal_createPolsVanNieropData (int choice, bool include_levels) {
	try {
		autoTable table = Table_create_polsVanNierop1973 ();
		/*
			Default: Pols 50 males, first part of the table.
		*/
		const integer ncols = include_levels ? 6 : 3;
		integer ib = 1,nrows = 50 * 12;
		if (choice == 2) { /* Van Nierop 25 females */
			ib = nrows + 1;
			nrows = 25 * 12;
		}

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);

		for (integer i = 1; i <= nrows; i ++) {
			const TableRow row = table -> rows.at [ib + i - 1];
			TableOfReal_setRowLabel (thee.get(), i, row -> cells [4]. string.get());
			for (integer j = 1; j <= 3; j ++) {
				thy data [i] [j] = Melder_atof (row -> cells [4 + j]. string.get());
				if (include_levels)
					thy data [i] [3 + j] = Melder_atof (row -> cells [7 + j]. string.get());
			}
		}
		for (integer j = 1; j <= 3; j ++) {
			conststring32 label = table -> columnHeaders [4 + j]. label.get();
			TableOfReal_setColumnLabel (thee.get(), j, label);
			if (include_levels) {
				label = table -> columnHeaders [7 + j]. label.get();
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
		const integer nvowels = 12, ncols = 3, nrows = 10 * nvowels;

		autoTable table = Table_create_weenink1983 ();

		integer ib = ( option == 1 ? 1 : option == 2 ? 11 : 21 ); /* m f c*/
		ib = (ib - 1) * nvowels + 1;

		autoTableOfReal thee = TableOfReal_create (nrows, ncols);
		for (integer i = 1; i <= nrows; i ++) {
			const TableRow row = table -> rows.at [ib + i - 1];
			TableOfReal_setRowLabel (thee.get(), i, row -> cells [5]. string.get());
			for (integer j = 1; j <= 3; j ++)
				thy data [i] [j] = Melder_atof (row -> cells [6 + j]. string.get()); /* Skip F0 */
		}
		for (integer j = 1; j <= 3; j ++)  {
			const conststring32 label = table -> columnHeaders [6 + j]. label.get();
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
		Permutation_permuteRandomly_inplace (p.get(), 0, 0);
		autoTableOfReal thee = TableOfReal_Permutation_permuteRows (me, p.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": randomized rows not created");
	}
}

autoTableOfReal TableOfReal_bootstrap (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			TableOfReal_setColumnLabel (thee.get(), icol, my columnLabels [icol].get());
		/*
			Select randomly from table with replacement. Because of replacement
			you do not get back the original data set. A random fraction,
			typically 1/e (37%), are replaced by duplicates.
		*/
		for (integer thyRow = 1; thyRow <= thy numberOfRows; thyRow ++) {
			const integer myRow = NUMrandomInteger (1, my numberOfRows);
			thy data.row (thyRow) <<= my data.row (myRow);
			TableOfReal_setRowLabel (thee.get(), thyRow, my rowLabels [myRow].get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": bootstrapped data not created.");
	}
}

void TableOfReal_changeRowLabels (TableOfReal me,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp) {
	try {
		autoSTRVEC rowLabels = string32vector_searchAndReplace (my rowLabels.get(),
			search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp);
		my rowLabels = std::move (rowLabels);
	} catch (MelderError) {
		Melder_throw (me, U": row labels not changed.");
	}
}

void TableOfReal_changeColumnLabels (TableOfReal me,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp) {
	try {
		autoSTRVEC columnLabels = string32vector_searchAndReplace (my columnLabels.get(),
			search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp);
		my columnLabels = std::move (columnLabels);
	} catch (MelderError) {
		Melder_throw (me, U": column labels not changed.");
	}
}

integer TableOfReal_getNumberOfLabelMatches (TableOfReal me, conststring32 search, bool columnLabels, bool use_regexp) {
	integer nmatches = 0, numberOfLabels = my numberOfRows;
	char32 **labels = my rowLabels.peek2();
	regexp *compiled_regexp = nullptr;

	if (! search || search [0] == U'\0')
		return 0;
	if (columnLabels) {
		numberOfLabels = my numberOfColumns;
		labels = my columnLabels.peek2();
	}
	try {
		if (use_regexp)
			compiled_regexp = CompileRE_throwable (search, 0);
		for (integer i = 1; i <= numberOfLabels; i ++) {
			if (! labels [i])
				continue;
			if (use_regexp) {
				if (ExecRE (compiled_regexp, nullptr, labels [i], nullptr, false, U'\0', U'\0', nullptr, nullptr))
					nmatches ++;
			} else if (str32equ (labels [i], search))
				nmatches ++;
		}
		if (use_regexp)
			free (compiled_regexp);
	} catch (MelderError) {
		if (use_regexp)
			free (compiled_regexp);
		Melder_throw (me, U"Can't get number of matches.");
	}
	return nmatches;
}

void TableOfReal_drawVectors (TableOfReal me, Graphics g, integer colx1, integer coly1, integer colx2, integer coly2, double xmin, double xmax, double ymin, double ymax, int vectype, int labelsize, bool garnish) {
	const integer nx = my numberOfColumns, ny = my numberOfRows;
	const double fontsize = Graphics_inqFontSize (g);

	Melder_require (colx1 > 0 && colx1 <= nx && coly1 > 0 && coly1 <= nx,
		U"The index in the \"From\" column(s) should be in range [1, ", nx, U"].");
	Melder_require (colx2 > 0 && colx2 <= nx && coly2 > 0 && coly2 <= nx,
		U"The index in the \"To\" column(s) should be in range [1, ", nx, U"].");

	if (xmin >= xmax) {
		MelderRealRange x1 = NUMextrema (my data.column (colx1));
		MelderRealRange x2 = NUMextrema (my data.column (colx2));
		xmin = std::min (x1.min, x2.min);
		xmax = std::max (x1.max, x2.max);
	}
	if (ymin >= ymax) {
		MelderRealRange y1 = NUMextrema (my data.column (coly1));
		MelderRealRange y2 = NUMextrema (my data.column (coly2));
		ymin = std::min (y1.min, y2.min);
		ymax = std::max (y1.max, y2.max);
	}
	if (xmin == xmax) {
		if (ymin == ymax) return;
		xmin -= 0.5;
		xmax += 0.5;
	}
	if (ymin == ymax) {
		ymin -= 0.5;
		ymax += 0.5;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);

	if (labelsize > 0)
		Graphics_setFontSize (g, labelsize);

	for (integer i = 1; i <= ny; i ++) {
		const double x1 = my data [i] [colx1];
		const double y1 = my data [i] [coly1];
		const double x2 = my data [i] [colx2];
		const double y2 = my data [i] [coly2];
		const conststring32 mark = EMPTY_STRING (my rowLabels [i]) ? U"" : my rowLabels [i].get();
		if (vectype == Graphics_LINE)
			Graphics_line (g, x1, y1, x2, y2);
		else if (vectype == Graphics_TWOWAYARROW) {
			Graphics_arrow (g, x1, y1, x2, y2);
			Graphics_arrow (g, x2, y2, x1, y1);
		} else /*if (vectype == Graphics_ARROW) */
			Graphics_arrow (g, x1, y1, x2, y2);
		if (labelsize > 0)
			Graphics_text (g, x1, y1, mark);
	}
	if (labelsize > 0)
		Graphics_setFontSize (g, fontsize);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

void TableOfReal_drawColumnAsDistribution (TableOfReal me, Graphics g, integer column, double minimum, double maximum, integer nBins, double freqMin, double freqMax, bool cumulative, bool garnish) {
	if (column < 1 || column > my numberOfColumns)
		return;
	autoMatrix thee = TableOfReal_to_Matrix (me);
	Matrix_drawDistribution (thee.get(), g, column - 0.5, column + 0.5, 0.0, 0.0, minimum, maximum, nBins, freqMin, freqMax, cumulative, garnish);
	if (garnish && my columnLabels [column])
		Graphics_textBottom (g, true, my columnLabels [column].get());
}

autoTableOfReal TableOfReal_sortRowsByIndex (TableOfReal me, constINTVEC index, bool reverse) {
	try {
		Melder_require (my rowLabels,
			U"No labels to sort");
		MelderIntegerRange range = NUMextrema (index);
		Melder_require (range.first > 0 && range.first <= my numberOfRows && range.last > 0 && range.last <= my numberOfRows,
			U"One or more indices out of range [1, ", my numberOfRows, U"].");
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (integer i = 1; i <= my numberOfRows; i ++) {
			const integer myindex = reverse ? i : index [i];
			const integer thyindex = reverse ? index [i] : i;
			thy rowLabels [thyindex] = Melder_dup (my rowLabels [myindex].get());
			thy data.row (thyindex) <<= my data.row (myindex);
		}
		thy columnLabels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not sorted by row index.");
	}
}

autoINTVEC TableOfReal_getSortedIndexFromRowLabels (TableOfReal me) {
	try {
		return newINTVECindex (my rowLabels.get());
	} catch (MelderError) {
		Melder_throw (me, U": no sorted index created.");
	}
}

autoTableOfReal TableOfReal_sortOnlyByRowLabels (TableOfReal me) {
	try {
		autoPermutation index = TableOfReal_to_Permutation_sortRowLabels (me);
		autoTableOfReal thee = TableOfReal_Permutation_permuteRows (me, index.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not sorted by row labels.");
	}
}

static void NUMaverageBlock_byColumns_inplace (MAT a, integer rb, integer re, integer cb, integer ce, bool medians) {
	Melder_assert (rb > 0 && rb <= a.nrow);
	Melder_assert (rb <= re && re <= a.nrow);
	Melder_assert (cb > 0 && cb <= a.ncol);
	Melder_assert (cb <= ce && ce <= a.ncol);
	const integer n = re - rb + 1;
	if (n < 2)
		return;
	autoVEC tmp = raw_VEC (n);
	for (integer j = cb; j <= ce; j ++) {
		integer k = 1;
		for (integer i = rb; i <= re; i ++, k ++)
			tmp [k] = a [i] [j];
		double average;
		if (medians) {
			sort_VEC_inout (tmp.get());
			average = NUMquantile (tmp.get(), 0.5);
		} else {
			average = NUMmean (tmp.get());
		}
		for (integer i = rb; i <= re; i ++)
			a [i] [j] = average;
	}
}

autoTableOfReal TableOfReal_meansByRowLabels (TableOfReal me, bool expand, bool useMedians) {
	try {
		autoTableOfReal thee;
		autoINTVEC index = TableOfReal_getSortedIndexFromRowLabels (me);
		autoTableOfReal sorted = TableOfReal_sortRowsByIndex (me, index.get(), false);

		integer indexi = 1, indexr = 0;
		conststring32 label = sorted -> rowLabels [1].get();
		for (integer i = 2; i <= my numberOfRows; i ++) {
			const conststring32 li = sorted -> rowLabels [i].get();
			if (Melder_cmp (li, label) != 0) {
				NUMaverageBlock_byColumns_inplace (sorted -> data.get(), indexi, i - 1, 1, my numberOfColumns, useMedians);
				if (! expand) {
					indexr ++;
					TableOfReal_copyOneRowWithLabel (sorted.get(), sorted.get(), indexi, indexr);
				}
				label = li;
				indexi = i;
			}
		}

		NUMaverageBlock_byColumns_inplace (sorted -> data.get(), indexi, my numberOfRows, 1, my numberOfColumns, useMedians);

		if (expand) {
			/*
				Now revert the sorting
			*/
			thee = TableOfReal_sortRowsByIndex (sorted.get(), index.get(), true);
		} else {
			indexr ++;
			TableOfReal_copyOneRowWithLabel (sorted.get(), sorted.get(), indexi, indexr);
			thee = TableOfReal_create (indexr, my numberOfColumns);
			for (integer i = 1; i <= indexr; i ++)
				TableOfReal_copyOneRowWithLabel (sorted.get(), thee.get(), i, i);
			thy columnLabels.all() <<= sorted -> columnLabels.all();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": means by row labels not created.");
	}
}

autoTableOfReal TableOfReal_rankColumns (TableOfReal me, integer fromColumn, integer toColumn) {
	try {
		fixAndCheckColumnRange (& fromColumn, & toColumn, my data.get(), 1);
		autoTableOfReal thee = Data_copy (me);
		MATrankColumns (thy data.get(), fromColumn, toColumn);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": column ranks not created.");
	}
}

/*
	s[lo]   = precursor<number>
	s[lo+1] = precursor<number+1>
	...
	s[hi]   = precursor<number+hi-lo>
*/
void TableOfReal_setSequentialColumnLabels (TableOfReal me, integer from, integer to, conststring32 precursor, integer number, integer increment) {
	from = ( from == 0 ? 1 : from );
	to = ( to == 0 ? my numberOfColumns : to );
	Melder_require (from > 0 && from <= to && to <= my numberOfColumns,
		U"Wrong column indices.");
	for (integer i = from; i <= to; i ++, number += increment)
		my columnLabels [i] = Melder_dup (Melder_cat (precursor, number));
}

void TableOfReal_setSequentialRowLabels (TableOfReal me, integer from, integer to, conststring32 precursor, integer number, integer increment) {
	from = ( from == 0 ? 1 : from );
	to = ( to == 0 ? my numberOfRows : to );
	Melder_require (from > 0 && from <= to && to <= my numberOfRows,
		U"Wrong row indices.");
	for (integer i = from; i <= to; i ++, number += increment)
		my rowLabels [i] = Melder_dup (Melder_cat (precursor, number));
}

/* For the inheritors */
autoTableOfReal TableOfReal_to_TableOfReal (TableOfReal me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		thy data.all() <<= my data.all();
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not copied.");
	}
}

autoTableOfReal TableOfReal_choleskyDecomposition (TableOfReal me, bool upper, bool inverse) {
	try {
		integer n = my numberOfColumns, lda = my numberOfRows, info;

		Melder_require (n == lda,
			U"The table should be a square symmetric table.");
		
		autoTableOfReal thee = Data_copy (me);

		if (upper) {
			for (integer i = 2; i <= n; i ++)
				for (integer j = 1; j < i; j ++)
					thy data [i] [j] = 0.0;
		} else {
			for (integer i = 1; i < n; i ++)
				for (integer j = i + 1; j <= n; j ++)
					thy data [i] [j] = 0.0;
		}
		const char *uplo = ( upper ? "L" : "U" );
		NUMlapack_dpotf2_ (uplo, n, & thy data [1] [1], lda, & info);
		Melder_require (info == 0,
			U"dpotf2 fails");
		
		if (inverse) {
			NUMlapack_dtrtri_ (uplo, "N", n, & thy data [1] [1], lda, &info);
			Melder_require (info == 0,
				U"dtrtri fails");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Cholesky decomposition not performed.");
	}
}

autoTableOfReal TableOfReal_appendColumns (TableOfReal me, TableOfReal thee) {
	try {
		const integer ncols = my numberOfColumns + thy numberOfColumns;
		Melder_require (my numberOfRows == thy numberOfRows,
			U"The numbers of rows should be equal.");
		/*
			Stricter label checking???
			append only if
			(my rowLabels [i] == thy rowlabels [i], i=1..my numberOfRows) or
			(my rowLabels [i] == 'empty', i=1..my numberOfRows)  or
			(thy rowLabels [i] == 'empty', i=1..my numberOfRows);
			'empty':  nullptr or \w*
		*/
		autoTableOfReal him = TableOfReal_create (my numberOfRows, ncols);
		his rowLabels.all() <<= my rowLabels.all();
		his columnLabels.part (1, my numberOfColumns) <<= my columnLabels.all();
		his columnLabels.part (my numberOfColumns + 1, ncols) <<= thy columnLabels.all();
		his data.verticalBand (1, my numberOfColumns) <<= my data.all();
		his data.verticalBand (my numberOfColumns + 1, ncols) <<= thy data.all();

		integer labeldiffs = 0;
		for (integer i = 1; i <= my numberOfRows; i ++)
			if (! Melder_equ (my rowLabels [i].get(), thy rowLabels [i].get()))
				labeldiffs ++;
		if (labeldiffs > 0)
			Melder_warning (labeldiffs, U" row labels differed.");
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with appended columns not created.");
	}
}

autoTableOfReal TableOfRealList_appendColumnsMany (TableOfRealList me) {
	try {
		Melder_require (my size > 0,
			U"No tables selected.");
		const TableOfReal first = my at [1];
		const integer nrows = first -> numberOfRows;
		integer ncols = first -> numberOfColumns;
		for (integer itab = 2; itab <= my size; itab ++) {
			const TableOfReal next = my at [itab];
			ncols += next -> numberOfColumns;
			Melder_require (next -> numberOfRows == nrows,
				U"Numbers of rows in item ", itab, U" differs from previous.");
		}
		autoTableOfReal him = TableOfReal_create (nrows, ncols);
		/* Unsafe: new attributes not initialized. */
		his rowLabels.all() <<= first -> rowLabels.all();
		
		integer hisColumnIndex = 0;
		for (integer itab = 1; itab <= my size; itab ++) {
			const TableOfReal table = my at [itab];
			for (integer icol = 1; icol <= table -> numberOfColumns; icol ++) {
				hisColumnIndex ++;
				TableOfReal_setColumnLabel (him.get(), hisColumnIndex, table -> columnLabels [icol].get());
				his data.column (hisColumnIndex) <<= table -> data.column (icol);
			}
		}
		Melder_assert (hisColumnIndex == his numberOfColumns);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with appended columns not created.");
	}
}

double TableOfReal_normalityTest_BHEP (TableOfReal me, double *h, double *out_tnb, double *out_lnmu, double *out_lnvar, bool *out_singularCovariance) {
	try {
		/*
			Henze & Wagner (1997), A new approach to the BHEP tests for multivariate normality, 
			Journal of Multivariate Analysis 62, 1-23.
		 */
		autoCovariance thee = TableOfReal_to_Covariance (me);
		autoVEC weights;
		const double probability = Covariance_TableOfReal_normalityTest_BHEP (thee.get(), me, weights.get(), h, out_tnb, out_lnmu, out_lnvar, out_singularCovariance);
		return probability;
	} catch (MelderError) {
		Melder_throw (me, U": cannot determine normality.");
	}
}

autoTableOfReal TableOfReal_TableOfReal_crossCorrelations (TableOfReal me, TableOfReal thee, bool by_columns, bool center, bool normalize) {
	return by_columns ? TableOfReal_TableOfReal_columnCorrelations (me, thee, center, normalize) :
	       TableOfReal_TableOfReal_rowCorrelations (me, thee, center, normalize);
}

autoTableOfReal TableOfReal_TableOfReal_rowCorrelations (TableOfReal me, TableOfReal thee, bool centre, bool normalize) {
	try {
		Melder_require (my numberOfColumns == thy numberOfColumns,
			U"Both tables should have the same number of columns.");
		autoTableOfReal him = TableOfReal_create (my numberOfRows, thy numberOfRows);
		autoMAT my_data = copy_MAT (my data.get());
		autoMAT thy_data = copy_MAT (thy data.get());
		if (centre) {
			centreEachRow_MAT_inout (my_data.get());
			centreEachRow_MAT_inout (thy_data.get());
		}
		if (normalize) {
			MATnormalizeRows_inplace (my_data.get(), 2.0, 1.0);
			MATnormalizeRows_inplace (thy_data.get(), 2.0, 1.0);
		}
		his rowLabels.all() <<= my rowLabels.all();
		his columnLabels.all() <<= thy rowLabels.all();
		mul_MAT_out (his data.get(), my_data.get(), thy_data.transpose());
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with row correlations not created.");
	}
}

autoTableOfReal TableOfReal_TableOfReal_columnCorrelations (TableOfReal me, TableOfReal thee, bool center, bool normalize) {
	try {
		Melder_require (my numberOfRows == thy numberOfRows,
			U"Both tables should have the same number of rows.");
		autoTableOfReal him = TableOfReal_create (my numberOfColumns, thy numberOfColumns);
		autoMAT my_data = copy_MAT (my data.get());
		autoMAT thy_data = copy_MAT (thy data.get());
		if (center) {
			centreEachColumn_MAT_inout (my_data.get());
			centreEachColumn_MAT_inout (thy_data.get());
		}
		if (normalize) {
			MATnormalizeColumns_inplace (my_data.get(), 2.0, 1.0);
			MATnormalizeColumns_inplace (thy_data.get(), 2.0, 1.0);
		}
		his rowLabels.all() <<= my columnLabels.all();
		his columnLabels.all() <<= thy columnLabels.all();
		mul_MAT_out (his data.get(), my_data.transpose(), thy_data.get()); 
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with column correlations not created.");
	}
}

autoMatrix TableOfReal_to_Matrix_interpolateOnRectangularGrid (TableOfReal me, double xmin, double xmax, double nx, double ymin, double ymax, integer ny, int /* method */) {
	try {
		Melder_require (my numberOfColumns > 2 && my numberOfColumns > 2, 
			U"There should be at least three colums and three rows.");
		autoVEC weights = newVECbiharmonic2DSplineInterpolation_getWeights (my data.column (1), my data.column (2), my data.column (3));
		const double dx = (xmax - xmin) / nx, dy = (ymax - ymin) / ny; 
		autoMatrix thee = Matrix_create (xmin, xmax, nx, dx, xmin + 0.5 * dx,
			ymin, ymax, ny, dy, ymin + 0.5 * dy);
		for (integer irow = 1; irow <= ny; irow ++) {
			const double yp = thy y1 + (irow - 1) * dy;
			for (integer icol = 1; icol <= nx; icol ++) {
				const double xp = thy x1 + (icol - 1) * dx;
				thy z [irow] [icol] = NUMbiharmonic2DSplineInterpolation (my data.column (1), my data.column (2), weights.get(), xp, yp);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": interpolation not finished.");
	}
}

#undef EMPTY_STRING

/* End of file TableOfReal_extensions.c */
