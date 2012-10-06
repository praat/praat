/* Confusion.cpp
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
	long nCorrect;

	Confusion_getEntropies (this, & h, & hx, & hy, & hygx, & hxgy, & uygx,
	                        & uxgy, & uxy);
	Confusion_getFractionCorrect (this, & frac, & nCorrect);
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (numberOfRows));
	MelderInfo_writeLine (L"Number of colums: ", Melder_integer (numberOfColumns));
	MelderInfo_writeLine (L"Entropies (y is row variable):");
	MelderInfo_writeLine (L"  Total: ", Melder_double (h));
	MelderInfo_writeLine (L"  Y: ", Melder_double (hy));
	MelderInfo_writeLine (L"  X: ", Melder_double (hx));
	MelderInfo_writeLine (L"  Y given x: ", Melder_double (hygx));
	MelderInfo_writeLine (L"  X given y: ", Melder_double (hxgy));
	MelderInfo_writeLine (L"  Dependency of y on x; ", Melder_double (uygx));
	MelderInfo_writeLine (L"  Dependency of x on y: ", Melder_double (uxgy));
	MelderInfo_writeLine (L"  Symmetrical dependency: ", Melder_double (uxy));
	MelderInfo_writeLine (L"  Total number of entries: ",
	                       Melder_integer (Confusion_getNumberOfEntries (this)));
	MelderInfo_writeLine (L" Fraction correct: ", Melder_double (frac));
}

Confusion Confusion_create (long numberOfStimuli, long numberOfResponses) {
	try {
		autoConfusion me = Thing_new (Confusion);
		TableOfReal_init (me.peek(), numberOfStimuli, numberOfResponses);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Confusion not created.");
	}
}

Confusion Confusion_createSimple (const wchar_t *labels) {
	try {
		long numberOfLabels = Melder_countTokens (labels);
		if (numberOfLabels < 1) {
			Melder_throw ("Not enough labels.");
		}
		autoConfusion me = Confusion_create (numberOfLabels, numberOfLabels);
		long ilabel = 1;
		for (wchar_t *token = Melder_firstToken (labels); token != 0; token = Melder_nextToken ()) {
			for (long i = 1; i <= ilabel - 1; i++) {
				if (Melder_wcscmp (token, my rowLabels[i]) == 0) {
					Melder_throw ("Label ", i, "and ", ilabel, "may not be equal.");
				}
			}
			TableOfReal_setRowLabel (me.peek(), ilabel, token);
			TableOfReal_setColumnLabel (me.peek(), ilabel, token);
			ilabel++;
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Simple Confusion not created.");
	}
}

Confusion Categories_to_Confusion (Categories me, Categories thee) {
	try {
		if (my size != thy size) {
			Melder_throw ("Categories_to_Confusion: dimensions do not agree.");
		}

		autoCategories ul1 = Categories_selectUniqueItems (me, 1);
		autoCategories ul2 = Categories_selectUniqueItems (thee, 1);
		autoConfusion him = Confusion_create (ul1 -> size, ul2 -> size);

		for (long i = 1; i <= ul1 -> size; i++) {
			SimpleString s = (SimpleString) ul1 -> item[i];
			TableOfReal_setRowLabel (him.peek(), i, s -> string);
		}
		for (long i = 1; i <= ul2 -> size; i++) {
			SimpleString s = (SimpleString) ul2 -> item[i];
			TableOfReal_setColumnLabel (him.peek(), i, s -> string);
		}
		for (long i = 1; i <= my size; i++) {
			SimpleString myi = (SimpleString) my item[i], thyi = (SimpleString) thy item[i];
			Confusion_increase (him.peek(), SimpleString_c (myi), SimpleString_c (thyi));
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Confusion created.");
	}
}

#define TINY 1.0e-30

void Confusion_getEntropies (Confusion me, double *h, double *hx, double *hy,
                             double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy) {
	*h = *hx = *hy = *hxgy = *hygx = *uygx = *uxgy = *uxy = 0;

	autoNUMvector<double> rowSum (1, my numberOfRows);
	autoNUMvector<double> colSum (1, my numberOfColumns);

	double sum = 0.0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			rowSum[i] += my data[i][j];
			colSum[j] += my data[i][j];
			sum += my data[i][j];
		}
	}
	for (long i = 1; i <= my numberOfRows; i++) {
		if (rowSum[i] > 0) {
			*hy -= rowSum[i] / sum * NUMlog2 (rowSum[i] / sum);
		}
	}
	for (long j = 1; j <= my numberOfColumns; j++) {
		if (colSum[j] > 0) {
			*hx -= colSum[j] / sum * NUMlog2 (colSum[j] / sum);
		}
	}
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (my data[i][j] > 0) {
				*h -= my data[i][j] / sum * NUMlog2 (my data[i][j] / sum);
			}
		}
	}

	*hygx = *h - *hx;
	*hxgy = *h - *hy;
	*uygx = (*hy - *hygx) / (*hy + TINY);
	*uxgy = (*hx - *hxgy) / (*hx + TINY);
	*uxy = 2.0 * (*hx + *hy - *h) / (*hx + *hy + TINY);
}

void Confusion_increase (Confusion me, const wchar_t *stim, const wchar_t *resp) {
	try {
		long stimIndex = TableOfReal_rowLabelToIndex (me, stim);
		if (stimIndex < 1) {
			Melder_throw ("Stimulus not valid.");
		}
		long respIndex = TableOfReal_columnLabelToIndex (me, resp);
		if (respIndex < 1) {
			Melder_throw ("Response not valid.");
		}

		my data[stimIndex][respIndex] += 1;
	} catch (MelderError) {
		Melder_throw (me, ": not increased.");
	}
}

double Confusion_getValue (Confusion me, const wchar_t *stim, const wchar_t *resp) {
	long stimIndex = TableOfReal_rowLabelToIndex (me, stim);
	if (stimIndex < 1) {
		Melder_throw ("Stimulus not valid.");
	}
	long respIndex = TableOfReal_columnLabelToIndex (me, resp);
	if (respIndex < 1) {
		Melder_throw ("Response not valid.");
	}
	return my data[stimIndex][respIndex];
}

void Confusion_getFractionCorrect (Confusion me, double *fraction, long *numberOfCorrect) {
	*fraction = NUMundefined;
	*numberOfCorrect = -1;

	double c = 0, ct = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (my rowLabels[i] == 0 || my columnLabels[j] == 0) {
				return;
			}
			ct += my data[i][j];
			if (wcscmp (my rowLabels[i], my columnLabels[j]) == 0) {
				c += my data[i][j];
			}
		}
	}

	if (ct != 0) {
		*fraction = c / ct;
	}
	*numberOfCorrect = c;
}

/*************** Confusion_Matrix_draw ****************************************/

#define NPOINTS 6

static Polygon Polygon_createPointer () {
	try {
		double x[NPOINTS + 1] = { 0, 0, 0.9, 1, 0.9, 0, 0 };
		double y[NPOINTS + 1] = { 0, 0, 0, 0.5,   1, 1, 0 };
		autoPolygon me = Polygon_create (NPOINTS);
		for (long i = 1; i <= NPOINTS; i++) {
			my x[i] = x[i]; my y[i] = y[i];
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Polygon not created.");
	}
}

static void Polygon_drawInside (I, Graphics g) {
	iam (Polygon);
	Graphics_polyline (g, my numberOfPoints, & my x[1], & my y[1]);
}

void Confusion_Matrix_draw (Confusion me, Matrix thee, Graphics g, long index, double lowerPercentage,
                            double xmin, double xmax, double ymin, double ymax, int garnish) {
	long ib = 1, ie = my numberOfRows;
	if (index > 0 && index <= my numberOfColumns) {
		ib = ie = index;
	}

	if (thy ny != my numberOfRows) {
		Melder_throw ("Wrong number of positions.");
	}

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
	double rmax = fabs (xmax - xmin) / 10;
	double rmin = rmax / 10;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin - rmax, xmax + rmax, ymin - rmax, ymax + rmax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (long i = 1; i <= my numberOfRows; i++) {
		Graphics_text (g, thy z[i][1], thy z[i][2], my rowLabels[i]);
	}
	for (long i = ib; i <= ie; i++) {
		double xSum = 0;
		for (long j = 1; j <= my numberOfColumns; j++) {
			xSum += my data[i][j];
		}

		if (xSum <= 0) {
			continue;    /* no confusions */
		}

		double x1 = thy z[i][1];
		double y1 = thy z[i][2];
		double r = rmax * my data[i][i] / xSum;

		Graphics_circle (g, x1, y1, r > rmin ? r : rmin);

		for (long j = 1; j <= my numberOfColumns; j++) {
			double x2 = thy z[j][1], y2 = thy z[j][2];
			double perc =  100 * my data[i][j] / xSum;
			double dx = x2 - x1, dy = y2 - y1;
			double alpha = atan2 (dy, dx);

			if (perc == 0 || perc < lowerPercentage || j == i) {
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
			if (xs < 0) {
				xs = 0;
			}
			double ys = perc * rmax / 100;
			Polygon_scale (p.peek(), xs, ys);
			Polygon_translate (p.peek(), x1, y1 - ys / 2);
			Polygon_rotate (p.peek(), alpha, x1, y1);
			Polygon_translate (p.peek(), 1.1 * r * cos (alpha), 1.1 * r * sin (alpha));
			Polygon_drawInside (p.peek(), g);
		}
	}

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (g, 0.0, 1, 1, 1, NULL);
		}
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (xmin * xmax < 0.0) {
			Graphics_markBottom (g, 0.0, 1, 1, 1, NULL);
		}
	}
}

Matrix Confusion_difference (Confusion me, Confusion thee) {
	try {
		/* categories must be the same too*/
		if (my numberOfColumns != thy numberOfColumns || my numberOfRows != thy numberOfRows) Melder_throw
			("Dimensions not equal.");

		autoMatrix him = Matrix_create (0.5, my numberOfColumns + 0.5, my numberOfColumns,
		                                1, 1, 0.5, my numberOfRows + 0.5, my numberOfRows, 1, 1);

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				his z[i][j] = my data[i][j] - thy data[i][j];
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix not created from two Confusions.");
	}
}

long Confusion_getNumberOfEntries (Confusion me) {
	double total = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			total += my data[i][j];
		}
	}
	return total;
}

static void create_index (wchar_t **s, long sb, long se, wchar_t **ref, long rb, long re, long *index) {
	for (long i = sb; i <= se; i++) {
		long indxj = 0;
		for (long j = rb; j <= re; j++) {
			if (wcsequ (s[i], ref[j])) {
				indxj = j; break;
			}
		}
		index[i] = indxj;
	}
}

Confusion Confusion_condense (Confusion me, const wchar_t *search, const wchar_t *replace,
	long maximumNumberOfReplaces, int use_regexp) {
	try {
		long nmatches, nstringmatches;

		if (my rowLabels == 0 || my columnLabels == 0) {
			Melder_throw ("No row or column labels.");
		}
		autostringvector rowLabels (strs_replace (my rowLabels, 1, my numberOfRows, search, replace,
			maximumNumberOfReplaces, &nmatches, &nstringmatches, use_regexp), 1, my numberOfRows);

		autostringvector columnLabels (strs_replace (my columnLabels, 1, my numberOfColumns,  search, replace,
			 maximumNumberOfReplaces, &nmatches, &nstringmatches, use_regexp), 1, my numberOfColumns);

		autoStrings srow = Thing_new (Strings);
		srow -> numberOfStrings = my numberOfRows;
		srow -> strings = rowLabels.transfer();

		autoStrings scol = Thing_new (Strings);
		scol -> numberOfStrings = my numberOfColumns;
		scol -> strings = columnLabels.transfer();

		/* Find dimension of new Confusion */
		autoDistributions dcol = Strings_to_Distributions (scol.peek());
		long nresp = dcol -> numberOfRows;

		autoDistributions drow = Strings_to_Distributions (srow.peek());
		long nstim = drow -> numberOfRows;

		autoConfusion thee = Confusion_create (nstim, nresp);

		NUMstrings_copyElements (drow -> rowLabels, thy rowLabels, 1, nstim);
		NUMstrings_copyElements (dcol -> rowLabels, thy columnLabels, 1, nresp);

		autoNUMvector<long> rowIndex (1, my numberOfRows);
		create_index (srow -> strings, 1, my numberOfRows, drow -> rowLabels, 1, nstim, rowIndex.peek());
		autoNUMvector<long> columnIndex (1, my numberOfColumns);
		create_index (scol -> strings, 1, my numberOfColumns, dcol -> rowLabels, 1, nresp, columnIndex.peek());

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				thy data [rowIndex [i]][columnIndex[j]] += my data[i][j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not condensed.");
	}
}

Confusion TableOfReal_to_Confusion (I) {
	iam (TableOfReal);
	try {
		if (! TableOfReal_checkPositive (me)) {
			Melder_throw ("Elements may not be less than zero.");
		}
		autoConfusion thee = Thing_new (Confusion);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Confusion.");
	}
}

Confusion Confusion_group (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos) {
	try {
		autoConfusion stim = Confusion_groupStimuli (me, labels, newLabel, newpos);
		autoConfusion thee = Confusion_groupResponses (stim.peek(), labels, newLabel, newpos);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not grouped.");
	}
}

Confusion Confusion_groupStimuli (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos) {
	try {
		long ncondense = Melder_countTokens (labels);
		autoNUMvector<long> irow (1, my numberOfRows);

		for (long i = 1; i <= my numberOfRows; i++) {
			irow[i] = i;
		}

		for (wchar_t *token = Melder_firstToken (labels); token != 0; token = Melder_nextToken ()) {
			for (long i = 1; i <= my numberOfRows; i++) {
				if (Melder_wcsequ (token, my rowLabels[i])) {
					irow[i] = 0;
					break;
				}
			}
		}
		long nfound = 0;
		for (long i = 1; i <= my numberOfRows; i++) {
			if (irow[i] == 0) {
				nfound ++;
			}
		}
		if (nfound == 0) {
			Melder_throw ("Invalid stimulus labels.");
		}
		if (nfound != ncondense) {
			Melder_warning (L"One or more of the given stimulus labels are suspect.");
		}
		long newnstim = my numberOfRows - nfound + 1;
		if (newpos < 1) {
			newpos = 1;
		}
		if (newpos > newnstim) {
			newpos = newnstim;
		}
		autoConfusion thee = Confusion_create (newnstim, my numberOfColumns);
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);

		TableOfReal_setRowLabel (thee.peek(), newpos, newLabel);
		long inewrow = 1;
		for (long i = 1; i <= my numberOfRows; i++) {
			long rowpos = newpos;
			if (irow[i] > 0) {
				if (inewrow == newpos) {
					inewrow++;
				}
				rowpos = inewrow;
				inewrow++;
				TableOfReal_setRowLabel (thee.peek(), rowpos, my rowLabels[i]);
			}
			for (long j = 1; j <= my numberOfColumns; j++) {
				thy data[rowpos][j] += my data[i][j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": stimuli not grouped.");
	}
}

Confusion Confusion_groupResponses (Confusion me, const wchar_t *labels, const wchar_t *newLabel, long newpos) {
	try {
		long ncondense = Melder_countTokens (labels);
		autoNUMvector<long> icol (1, my numberOfColumns);

		for (long i = 1; i <= my numberOfColumns; i++) {
			icol[i] = i;
		}

		for (wchar_t *token = Melder_firstToken (labels); token != 0; token = Melder_nextToken ()) {
			for (long i = 1; i <= my numberOfColumns; i++) {
				if (Melder_wcsequ (token, my columnLabels[i])) {
					icol[i] = 0;
					break;
				}
			}
		}
		long nfound = 0;
		for (long i = 1; i <= my numberOfColumns; i++) {
			if (icol[i] == 0) {
				nfound ++;
			}
		}
		if (nfound == 0) {
			Melder_throw ("Invalid response labels.");
		}
		if (nfound != ncondense) {
			Melder_warning (L"One or more of the given response labels are suspect.");
		}
		long newnresp = my numberOfColumns - nfound + 1;
		if (newpos < 1) {
			newpos = 1;
		}
		if (newpos > newnresp) {
			newpos = newnresp;
		}
		autoConfusion thee = Confusion_create (my numberOfRows, newnresp);
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, my numberOfRows);
		TableOfReal_setColumnLabel (thee.peek(), newpos, newLabel);
		long inewcol = 1;
		for (long i = 1; i <= my numberOfColumns; i++) {
			long colpos = newpos;
			if (icol[i] > 0) {
				if (inewcol == newpos) {
					inewcol++;
				}
				colpos = inewcol;
				inewcol++;
				TableOfReal_setColumnLabel (thee.peek(), colpos, my columnLabels[i]);
			}
			for (long j = 1; j <= my numberOfRows; j++) {
				thy data[j][colpos] += my data[j][i];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": responses not grouped.");
	}
}

TableOfReal Confusion_to_TableOfReal_marginals (I) {
	iam (Confusion);
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows + 1, my numberOfColumns + 1);

		double total = 0;
		for (long i = 1; i <= my numberOfRows; i++) {
			double rowsum = 0;
			for (long j = 1; j <= my numberOfColumns; j++) {
				thy data[i][j] = my data[i][j];
				rowsum += my data[i][j];
			}
			thy data[i][my numberOfColumns + 1] = rowsum;
			total += rowsum;
		}

		thy data[my numberOfRows + 1][my numberOfColumns + 1] = total;

		for (long j = 1; j <= my numberOfColumns; j++) {
			double colsum = 0;
			for (long i = 1; i <= my numberOfRows; i++) {
				colsum += my data[i][j];
			}
			thy data[my numberOfRows + 1][j] = colsum;
		}

		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, my numberOfRows);
		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": table with marginals not created.");
	}
}

void Confusion_drawAsNumbers (I, Graphics g, int marginals, int iformat, int precision) {
	iam (Confusion);
	TableOfReal thee = (TableOfReal) me;
	autoTableOfReal athee = 0;
	if (marginals) {
		athee.reset (Confusion_to_TableOfReal_marginals (me));
		thee = athee.peek();
	}
	TableOfReal_drawAsNumbers (thee, g, 1, thy numberOfRows, iformat, precision);
}


/* End of file Confusion.cpp */
