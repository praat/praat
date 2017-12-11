/* Confusion.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20010628
 djmw 20020813 GPL header
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20070620 Latest modification.
 djmw 20080521 +Confusion_drawAsNumbers
 djmw 20110304 Thing_new
 djmw 20111110 Use autostringvector
*/

#include "Confusion.h"
#include "Polygon_extensions.h"
#include "Matrix_extensions.h"
#include "TableOfReal_extensions.h"
#include "Collection_extensions.h"
#include "Distributions_and_Strings.h"
#include "NUM2.h"

Thing_implement (Confusion, TableOfReal, 0);

void structConfusion :: v_info () {
	double h, hx, hy, hygx, hxgy, uygx, uxgy, uxy, frac;
	integer nCorrect;

	Confusion_getEntropies (this, & h, & hx, & hy, & hygx, & hxgy, & uygx, & uxgy, & uxy);
	Confusion_getFractionCorrect (this, & frac, & nCorrect);
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of colums: ", numberOfColumns);
	MelderInfo_writeLine (U"Entropies (y is row variable):");
	MelderInfo_writeLine (U"  Total: ", h);
	MelderInfo_writeLine (U"  Y: ", hy);
	MelderInfo_writeLine (U"  X: ", hx);
	MelderInfo_writeLine (U"  Y given x: ", hygx);
	MelderInfo_writeLine (U"  X given y: ", hxgy);
	MelderInfo_writeLine (U"  Dependency of y on x; ", uygx);
	MelderInfo_writeLine (U"  Dependency of x on y: ", uxgy);
	MelderInfo_writeLine (U"  Symmetrical dependency: ", uxy);
	MelderInfo_writeLine (U"  Total number of entries: ", Confusion_getNumberOfEntries (this));
	MelderInfo_writeLine (U" Fraction correct: ", frac);
}

autoConfusion Confusion_createFromStringses (Strings me, Strings thee) {
	try {
		Melder_require (my numberOfStrings > 0 && thy numberOfStrings > 0, U"Both Strings should not be empty.");
		
		autoConfusion him = Confusion_create (my numberOfStrings, thy numberOfStrings);
		for (integer irow = 1; irow <= my numberOfStrings; irow ++) {
			const char32 *label = my strings [irow];
			TableOfReal_setRowLabel (him.get(), irow, label);
		}
		for (integer icol = 1; icol <= thy numberOfStrings; icol ++) {
			const char32 *label = thy strings [icol];
			TableOfReal_setColumnLabel (him.get(), icol, label);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": could not create Confusion with ", thee);
	}
}

autoConfusion Confusion_create (integer numberOfStimuli, integer numberOfResponses) {
	try {
		autoConfusion me = Thing_new (Confusion);
		TableOfReal_init (me.get(), numberOfStimuli, numberOfResponses);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Confusion not created.");
	}
}

autoConfusion Confusion_createSimple (const char32 *labels) {
	try {
		integer numberOfLabels = Melder_countTokens (labels);
		Melder_require (numberOfLabels > 0, U"There should be at least one label.");
		
		autoConfusion me = Confusion_create (numberOfLabels, numberOfLabels);
		integer ilabel = 1;
		for (char32 *token = Melder_firstToken (labels); token != 0; token = Melder_nextToken ()) {
			for (integer i = 1; i <= ilabel - 1; i ++) {
				if (Melder_cmp (token, my rowLabels [i]) == 0) {
					Melder_throw (U"Label ", i, U" and ", ilabel, U" should not be equal.");
				}
			}
			TableOfReal_setRowLabel (me.get(), ilabel, token);
			TableOfReal_setColumnLabel (me.get(), ilabel, token);
			ilabel++;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Confusion not created.");
	}
}

autoConfusion Categories_to_Confusion (Categories me, Categories thee) {
	try {
		Melder_require (my size == thy size, U"Both Categories should have the same number of items.");

		autoCategories ul1 = Categories_selectUniqueItems (me);
		autoCategories ul2 = Categories_selectUniqueItems (thee);
		autoConfusion him = Confusion_create (ul1->size, ul2->size);

		for (integer i = 1; i <= ul1->size; i ++) {
			SimpleString s = ul1->at [i];
			TableOfReal_setRowLabel (him.get(), i, s -> string);
		}
		for (integer i = 1; i <= ul2->size; i ++) {
			SimpleString s = ul2->at [i];
			TableOfReal_setColumnLabel (him.get(), i, s -> string);
		}
		for (integer i = 1; i <= my size; i ++) {
			SimpleString myi = my at [i], thyi = thy at [i];
			Confusion_increase (him.get(), SimpleString_c (myi), SimpleString_c (thyi));
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Confusion created.");
	}
}

#define TINY 1.0e-30

void Confusion_getEntropies (Confusion me, double *p_h, double *p_hx, double *p_hy, double *p_hygx, double *p_hxgy, double *p_uygx, double *p_uxgy, double *p_uxy) {
	double h = 0.0, hx = 0.0, hy = 0.0, hxgy = 0.0, hygx = 0.0, uygx = 0.0, uxgy = 0.0, uxy = 0.0;

	autoNUMvector<double> rowSum (1, my numberOfRows);
	autoNUMvector<double> colSum (1, my numberOfColumns);

	double sum = 0.0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			rowSum [i] += my data [i] [j];
			colSum [j] += my data [i] [j];
			sum += my data [i] [j];
		}
	}
	for (integer i = 1; i <= my numberOfRows; i ++) {
		if (rowSum [i] > 0.0) {
			hy -= rowSum [i] / sum * NUMlog2 (rowSum [i] / sum);
		}
	}
	for (integer j = 1; j <= my numberOfColumns; j ++) {
		if (colSum [j] > 0.0) {
			hx -= colSum [j] / sum * NUMlog2 (colSum [j] / sum);
		}
	}
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			if (my data [i] [j] > 0.0) {
				h -= my data [i] [j] / sum * NUMlog2 (my data [i] [j] / sum);
			}
		}
	}

	hygx = h - hx;
	hxgy = h - hy;
	uygx = (hy - hygx) / (hy + TINY);
	uxgy = (hx - hxgy) / (hx + TINY);
	uxy = 2.0 * (hx + hy - h) / (hx + hy + TINY);
	if (p_h) {
		*p_h  = h;
	}
	if (p_hx) {
		*p_hx  = hx;
	}
	if (p_hy) {
		*p_hy  = hy;
	}
	if (p_hygx) {
		*p_hygx  = hygx;
	}
	if (p_hxgy) {
		*p_hxgy  = hxgy;
	}
	if (p_uygx) {
		*p_uygx  = uygx;
	}
	if (p_uxgy) {
		*p_uxgy  = uxgy;
	}
	if (p_uxy) {
		*p_uxy  = uxy;
	}
}

void Confusion_increase (Confusion me, const char32 *stim, const char32 *resp) {
	try {
		integer stimIndex = TableOfReal_rowLabelToIndex (me, stim);
		Melder_require (stimIndex > 0, U"The stimulus name should be valid.");
		
		integer respIndex = TableOfReal_columnLabelToIndex (me, resp);
		Melder_require (respIndex > 0, U"The response name should be valid.");

		my data [stimIndex] [respIndex] += 1.0;
	} catch (MelderError) {
		Melder_throw (me, U": not increased.");
	}
}

double Confusion_getValue (Confusion me, const char32 *stim, const char32 *resp) {
	integer stimIndex = TableOfReal_rowLabelToIndex (me, stim);
	
	Melder_require (stimIndex > 0, U"The stimulus should be valid.");
	
	integer respIndex = TableOfReal_columnLabelToIndex (me, resp);
	
	Melder_require (respIndex > 0, U"The response should be valid.");
	
	return my data [stimIndex] [respIndex];
}

void Confusion_getFractionCorrect (Confusion me, double *p_fraction, integer *p_numberOfCorrect) {
	double fraction = undefined;
	integer numberOfCorrect = -1;

	double c = 0.0, ct = 0.0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			if (! my rowLabels [i] || ! my columnLabels [j]) {
				return;
			}
			ct += my data[i][j];
			if (str32cmp (my rowLabels [i], my columnLabels [j]) == 0) {
				c += my data [i] [j];
			}
		}
	}

	if (ct != 0.0) {
		fraction = c / ct;
	}
	if (p_fraction) {
		*p_fraction = fraction;
	}
	numberOfCorrect = Melder_ifloor (c);
	if (p_numberOfCorrect) {
		*p_numberOfCorrect = numberOfCorrect;
	}
}

/*************** Confusion_Matrix_draw ****************************************/

#define NPOINTS 6

static autoPolygon Polygon_createPointer () {
	try {
		double x [NPOINTS + 1] = { 0.0, 0.0, 0.9, 1.0, 0.9, 0.0, 0.0 };
		double y [NPOINTS + 1] = { 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.0 };
		autoPolygon me = Polygon_create (NPOINTS);
		for (integer i = 1; i <= NPOINTS; i ++) {
			my x [i] = x [i]; my y [i] = y [i];
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polygon not created.");
	}
}

static void Polygon_drawInside (Polygon me, Graphics g) {
	Graphics_polyline (g, my numberOfPoints, & my x[1], & my y[1]);
}

void Confusion_Matrix_draw (Confusion me, Matrix thee, Graphics g, integer index, double lowerPercentage, double xmin, double xmax, double ymin, double ymax, int garnish) {
	integer ib = 1, ie = my numberOfRows;
	if (index > 0 && index <= my numberOfColumns) {
		ib = ie = index;
	}
	Melder_require (thy ny == my numberOfRows, U"The number of stimuli should equal the number of rows in the matrix.");

	if (xmax <= xmin) {
		(void) Matrix_getWindowExtrema (thee, 1, 1, 1, thy ny, &xmin, &xmax);
	}

	if (xmax <= xmin) {
		return;
	}

	if (ymax <= ymin) {
		(void) Matrix_getWindowExtrema (thee, 2, 2, 1, thy ny, &ymin, &ymax);
	}

	if (ymax <= ymin) {
		return;
	}
	double rmax = fabs (xmax - xmin) / 10.0;
	double rmin = rmax / 10;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin - rmax, xmax + rmax, ymin - rmax, ymax + rmax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (integer i = 1; i <= my numberOfRows; i ++) {
		Graphics_text (g, thy z [i] [1], thy z [i] [2], my rowLabels[i]);
	}
	for (integer i = ib; i <= ie; i ++) {
		double xSum = 0.0;
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			xSum += my data [i] [j];
		}

		if (xSum <= 0.0) {
			continue;    /* no confusions */
		}

		double x1 = thy z [i] [1];
		double y1 = thy z [i] [2];
		double r = rmax * my data [i] [i] / xSum;

		Graphics_circle (g, x1, y1, r > rmin ? r : rmin);

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			double x2 = thy z [j] [1], y2 = thy z [j] [2];
			double perc =  100.0 * my data [i] [j] / xSum;
			double dx = x2 - x1, dy = y2 - y1;
			double alpha = atan2 (dy, dx);

			if (perc == 0.0 || perc < lowerPercentage || j == i) {
				continue;
			}

			xmin = x1; xmax = x2;
			if (x2 < x1) {
				xmin = x2; xmax = x1;
			}
			ymin = y1; xmax = y2;
			if (y2 < y1) {
				ymin = y2; ymax = y1;
			}
			autoPolygon p = Polygon_createPointer();
			double xs = sqrt (dx * dx + dy * dy) - 2.2 * r;
			if (xs < 0.0) {
				xs = 0.0;
			}
			double ys = perc * rmax / 100.0;
			Polygon_scale (p.get(), xs, ys);
			Polygon_translate (p.get(), x1, y1 - ys / 2);
			Polygon_rotate (p.get(), alpha, x1, y1);
			Polygon_translate (p.get(), 1.1 * r * cos (alpha), 1.1 * r * sin (alpha));
			Polygon_drawInside (p.get(), g);
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		}
		Graphics_marksLeft (g, 2, true, true, false);
		if (xmin * xmax < 0.0) {
			Graphics_markBottom (g, 0.0, true, true, true, nullptr);
		}
	}
}

autoMatrix Confusion_difference (Confusion me, Confusion thee) {
	try {
		/* categories must be the same too*/
		Melder_require (my numberOfColumns == thy numberOfColumns && my numberOfRows == thy numberOfRows,
			U"The dimensions should be equal.");

		autoMatrix him = Matrix_create (0.5, my numberOfColumns + 0.5, my numberOfColumns, 1.0, 1.0, 0.5, my numberOfRows + 0.5, my numberOfRows, 1.0, 1.0);

		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = 1; j <= my numberOfColumns; j ++) {
				his z [i] [j] = my data [i] [j] - thy data [i] [j];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Matrix not created from two Confusions.");
	}
}

integer Confusion_getNumberOfEntries (Confusion me) {
	double total = 0.0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			total += my data [i] [j];
		}
	}
	return Melder_ifloor (total);
}

static void create_index (char32 **s, integer sb, integer se, char32 **ref, integer rb, integer re, integer *index) {
	for (integer i = sb; i <= se; i ++) {
		integer indxj = 0;
		for (integer j = rb; j <= re; j ++) {
			if (str32equ (s [i], ref [j])) {
				indxj = j; break;
			}
		}
		index [i] = indxj;
	}
}

autoConfusion Confusion_condense (Confusion me, const char32 *search, const char32 *replace,
	integer maximumNumberOfReplaces, bool use_regexp) {
	try {
		integer nmatches, nstringmatches;
		Melder_require (my rowLabels != nullptr && my columnLabels != nullptr, U"Both row and column labels should be present.");
		
		autostring32vector rowLabels (strs_replace (my rowLabels, 1, my numberOfRows, search, replace,
			maximumNumberOfReplaces, & nmatches, & nstringmatches, use_regexp), 1, my numberOfRows);

		autostring32vector columnLabels (strs_replace (my columnLabels, 1, my numberOfColumns,  search, replace,
			 maximumNumberOfReplaces, & nmatches, & nstringmatches, use_regexp), 1, my numberOfColumns);

		autoStrings srow = Thing_new (Strings);
		srow -> numberOfStrings = my numberOfRows;
		srow -> strings = rowLabels.transfer();

		autoStrings scol = Thing_new (Strings);
		scol -> numberOfStrings = my numberOfColumns;
		scol -> strings = columnLabels.transfer();

		/* Find dimension of new Confusion */
		autoDistributions dcol = Strings_to_Distributions (scol.get());
		integer nresp = dcol -> numberOfRows;

		autoDistributions drow = Strings_to_Distributions (srow.get());
		integer nstim = drow -> numberOfRows;

		autoConfusion thee = Confusion_create (nstim, nresp);

		NUMstrings_copyElements (drow -> rowLabels, thy rowLabels, 1, nstim);
		NUMstrings_copyElements (dcol -> rowLabels, thy columnLabels, 1, nresp);

		autoNUMvector<integer> rowIndex (1, my numberOfRows);
		create_index (srow -> strings, 1, my numberOfRows, drow -> rowLabels, 1, nstim, rowIndex.peek());
		autoNUMvector<integer> columnIndex (1, my numberOfColumns);
		create_index (scol -> strings, 1, my numberOfColumns, dcol -> rowLabels, 1, nresp, columnIndex.peek());

		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = 1; j <= my numberOfColumns; j ++) {
				thy data [rowIndex [i]] [columnIndex [j]] += my data [i] [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not condensed.");
	}
}

autoConfusion TableOfReal_to_Confusion (TableOfReal me) {
	try {
		Melder_require (TableOfReal_checkPositive (me), U"Elements should not be negative.");
		
		autoConfusion thee = Thing_new (Confusion);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Confusion.");
	}
}

autoConfusion Confusion_group (Confusion me, const char32 *labels, const char32 *newLabel, integer newpos) {
	try {
		autoConfusion stim = Confusion_groupStimuli (me, labels, newLabel, newpos);
		autoConfusion thee = Confusion_groupResponses (stim.get(), labels, newLabel, newpos);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not grouped.");
	}
}

autoConfusion Confusion_groupStimuli (Confusion me, const char32 *labels, const char32 *newLabel, integer newpos) {
	try {
		integer ncondense = Melder_countTokens (labels);
		autoNUMvector<integer> irow (1, my numberOfRows);

		for (integer i = 1; i <= my numberOfRows; i ++) {
			irow [i] = i;
		}

		for (char32 *token = Melder_firstToken (labels); token != nullptr; token = Melder_nextToken ()) {
			for (integer i = 1; i <= my numberOfRows; i ++) {
				if (Melder_equ (token, my rowLabels [i])) {
					irow [i] = 0;
					break;
				}
			}
		}
		integer nfound = 0;
		for (integer i = 1; i <= my numberOfRows; i ++) {
			if (irow [i] == 0) {
				nfound ++;
			}
		}
		Melder_require (nfound > 0, U"The stimulus labels are invalid.");
		
		if (nfound != ncondense) {
			Melder_warning (U"One or more of the given stimulus labels are suspect.");
		}
		integer newnstim = my numberOfRows - nfound + 1;
		if (newpos < 1) {
			newpos = 1;
		}
		if (newpos > newnstim) {
			newpos = newnstim;
		}
		autoConfusion thee = Confusion_create (newnstim, my numberOfColumns);
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);

		TableOfReal_setRowLabel (thee.get(), newpos, newLabel);
		integer inewrow = 1;
		for (integer i = 1; i <= my numberOfRows; i ++) {
			integer rowpos = newpos;
			if (irow [i] > 0) {
				if (inewrow == newpos) {
					inewrow++;
				}
				rowpos = inewrow;
				inewrow++;
				TableOfReal_setRowLabel (thee.get(), rowpos, my rowLabels[i]);
			}
			for (integer j = 1; j <= my numberOfColumns; j ++) {
				thy data [rowpos] [j] += my data [i] [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": stimuli not grouped.");
	}
}

autoConfusion Confusion_groupResponses (Confusion me, const char32 *labels, const char32 *newLabel, integer newpos) {
	try {
		integer ncondense = Melder_countTokens (labels);
		autoNUMvector<integer> icol (1, my numberOfColumns);

		for (integer i = 1; i <= my numberOfColumns; i ++) {
			icol [i] = i;
		}

		for (char32 *token = Melder_firstToken (labels); token != 0; token = Melder_nextToken ()) {
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				if (Melder_equ (token, my columnLabels [i])) {
					icol [i] = 0;
					break;
				}
			}
		}
		integer nfound = 0;
		for (integer i = 1; i <= my numberOfColumns; i ++) {
			if (icol [i] == 0) {
				nfound ++;
			}
		}
		
		Melder_require (nfound > 0, U"The response labels are invalid.");
		
		if (nfound != ncondense) {
			Melder_warning (U"One or more of the given response labels are suspect.");
		}
		integer newnresp = my numberOfColumns - nfound + 1;
		if (newpos < 1) {
			newpos = 1;
		}
		if (newpos > newnresp) {
			newpos = newnresp;
		}
		autoConfusion thee = Confusion_create (my numberOfRows, newnresp);
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, my numberOfRows);
		TableOfReal_setColumnLabel (thee.get(), newpos, newLabel);
		integer inewcol = 1;
		for (integer i = 1; i <= my numberOfColumns; i ++) {
			integer colpos = newpos;
			if (icol [i] > 0) {
				if (inewcol == newpos) {
					inewcol ++;
				}
				colpos = inewcol;
				inewcol ++;
				TableOfReal_setColumnLabel (thee.get(), colpos, my columnLabels [i]);
			}
			for (integer j = 1; j <= my numberOfRows; j ++) {
				thy data [j] [colpos] += my data [j] [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": responses not grouped.");
	}
}

autoTableOfReal Confusion_to_TableOfReal_marginals (Confusion me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows + 1, my numberOfColumns + 1);

		double total = 0.0;
		for (integer i = 1; i <= my numberOfRows; i ++) {
			double rowsum = 0.0;
			for (integer j = 1; j <= my numberOfColumns; j ++) {
				thy data [i] [j] = my data [i] [j];
				rowsum += my data [i] [j];
			}
			thy data [i] [my numberOfColumns + 1] = rowsum;
			total += rowsum;
		}

		thy data [my numberOfRows + 1] [my numberOfColumns + 1] = total;

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			double colsum = 0;
			for (integer i = 1; i <= my numberOfRows; i ++) {
				colsum += my data [i] [j];
			}
			thy data [my numberOfRows + 1] [j] = colsum;
		}

		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": table with marginals not created.");
	}
}

void Confusion_drawAsNumbers (Confusion me, Graphics g, bool marginals, int iformat, int precision) {
	TableOfReal thee = me;
	autoTableOfReal athee;
	if (marginals) {
		athee = Confusion_to_TableOfReal_marginals (me);
		thee = athee.get();
	}
	TableOfReal_drawAsNumbers (thee, g, 1, thy numberOfRows, iformat, precision);
}


/* End of file Confusion.cpp */
