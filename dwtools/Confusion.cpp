/* Confusion.cpp
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
			conststring32 label = my strings [irow].get();
			TableOfReal_setRowLabel (him.get(), irow, label);
		}
		for (integer icol = 1; icol <= thy numberOfStrings; icol ++) {
			conststring32 label = thy strings [icol].get();
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

autoConfusion Confusion_createSimple (conststring32 labels_string) {
	try {
		autoSTRVEC labels = splitByWhitespace_STRVEC (labels_string);
		Melder_require (labels.size > 0, U"There should be at least one label.");
		
		autoConfusion me = Confusion_create (labels.size, labels.size);
		integer ilabel = 1;
		for (integer itoken = 1; itoken <= labels.size; itoken ++) {
			const conststring32 token = labels [itoken].get();
			for (integer i = 1; i <= ilabel - 1; i ++) {
				if (Melder_equ (token, my rowLabels [i].get())) {
					Melder_throw (U"Label ", i, U" and ", ilabel, U" should not be equal.");
				}
			}
			TableOfReal_setRowLabel (me.get(), ilabel, token);
			TableOfReal_setColumnLabel (me.get(), ilabel, token);
			ilabel ++;
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
			const SimpleString s = ul1->at [i];
			TableOfReal_setRowLabel (him.get(), i, s -> string.get());
		}
		for (integer i = 1; i <= ul2->size; i ++) {
			const SimpleString s = ul2->at [i];
			TableOfReal_setColumnLabel (him.get(), i, s -> string.get());
		}
		for (integer i = 1; i <= my size; i ++) {
			const SimpleString myi = my at [i], thyi = thy at [i];
			Confusion_increase (him.get(), myi -> string.get(), thyi -> string.get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Confusion created.");
	}
}

void Confusion_getEntropies (Confusion me, double *out_h, double *out_hx, double *out_hy,
	double *out_hygx, double *out_hxgy, double *out_uygx, double *out_uxgy, double *out_uxy)
{
	NUMgetEntropies (my data.get(), out_h, out_hx,
			out_hy,	out_hygx, out_hxgy, out_uygx, out_uxgy, out_uxy);
}

void Confusion_increase (Confusion me, conststring32 stimulus, conststring32 response) {
	try {
		const integer stimulusIndex = TableOfReal_rowLabelToIndex (me, stimulus);
		Melder_require (stimulusIndex > 0,
			U"The stimulus name should be valid.");
		
		const integer responseIndex = TableOfReal_columnLabelToIndex (me, response);
		Melder_require (responseIndex > 0,
			U"The response name should be valid.");

		my data [stimulusIndex] [responseIndex] += 1.0;
	} catch (MelderError) {
		Melder_throw (me, U": not increased.");
	}
}

double Confusion_getValue (Confusion me, conststring32 stimulus, conststring32 response) {
	const integer stimulusIndex = TableOfReal_rowLabelToIndex (me, stimulus);
	Melder_require (stimulusIndex > 0,
		U"The stimulus name should be valid.");
	
	const integer responseIndex = TableOfReal_columnLabelToIndex (me, response);
	Melder_require (responseIndex > 0,
		U"The response name should be valid.");

	return my data [stimulusIndex] [responseIndex];
}

void Confusion_getFractionCorrect (Confusion me, double *out_fraction, integer *out_numberOfCorrect) {
	double fraction = undefined;
	integer numberOfCorrect = -1;

	double c = 0.0, ct = 0.0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			if (! my rowLabels [i] || ! my columnLabels [j])
				return;
			ct += my data [i] [j];
			if (str32equ (my rowLabels [i].get(), my columnLabels [j].get()))
				c += my data [i] [j];
		}
	}
	if (ct != 0.0)
		fraction = c / ct;
	if (out_fraction)
		*out_fraction = fraction;
	numberOfCorrect = Melder_ifloor (c);
	if (out_numberOfCorrect)
		*out_numberOfCorrect = numberOfCorrect;
}

/*************** Confusion_Matrix_draw ****************************************/

static autoPolygon Polygon_createPointer () {
	try {
		constexpr integer numberOfPoints = 6;
		double x [numberOfPoints + 1] = { 0.0, 0.0, 0.9, 1.0, 0.9, 0.0, 0.0 };
		double y [numberOfPoints + 1] = { 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.0 };
		autoPolygon me = Polygon_create (numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			my x [i] = x [i];
			my y [i] = y [i];
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polygon not created.");
	}
}

static void Polygon_drawInside (Polygon me, Graphics g) {
	Graphics_polyline (g, my numberOfPoints, & my x[1], & my y[1]);
}

void Confusion_Matrix_draw (Confusion me, Matrix thee, Graphics g, integer index, double lowerPercentage, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	integer ib = 1, ie = my numberOfRows;
	if (index > 0 && index <= my numberOfColumns) {
		ib = ie = index;
	}
	Melder_require (thy ny == my numberOfRows,
		U"The number of stimuli should equal the number of rows in the matrix.");

	if (xmax <= xmin)
		(void) Matrix_getWindowExtrema (thee, 1, 1, 1, thy ny, & xmin, & xmax);
	if (xmax <= xmin)
		return;
	if (ymax <= ymin)
		(void) Matrix_getWindowExtrema (thee, 2, 2, 1, thy ny, & ymin, & ymax);
	if (ymax <= ymin)
		return;

	const double rmax = fabs (xmax - xmin) / 10.0;
	const double rmin = rmax / 10;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin - rmax, xmax + rmax, ymin - rmax, ymax + rmax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	for (integer i = 1; i <= my numberOfRows; i ++) {
		Graphics_text (g, thy z [i] [1], thy z [i] [2], my rowLabels [i].get());
	}
	for (integer i = ib; i <= ie; i ++) {
		const double xSum = NUMsum (my data.row (i));
		if (xSum <= 0.0)
			continue;    /* no confusions */
		const double x1 = thy z [i] [1];
		const double y1 = thy z [i] [2];
		const double r = rmax * my data [i] [i] / xSum;

		Graphics_circle (g, x1, y1, ( r > rmin ? r : rmin ));

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			const double x2 = thy z [j] [1], y2 = thy z [j] [2];
			const double perc =  100.0 * my data [i] [j] / xSum;
			const double dx = x2 - x1, dy = y2 - y1;
			const double alpha = atan2 (dy, dx);

			if (perc == 0.0 || perc < lowerPercentage || j == i)
				continue;
			xmin = x1;
			xmax = x2;
			if (x2 < x1) {
				xmin = x2;
				xmax = x1;
			}
			ymin = y1;
			xmax = y2;
			if (y2 < y1) {
				ymin = y2;
				ymax = y1;
			}
			autoPolygon p = Polygon_createPointer();
			double xs = sqrt (dx * dx + dy * dy) - 2.2 * r;
			if (xs < 0.0)
				xs = 0.0;
			const double ys = perc * rmax / 100.0;
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

		his z.all()  <<=  my data.all()  -  thy data.all();
		
		return him;
	} catch (MelderError) {
		Melder_throw (U"Matrix not created from two Confusions.");
	}
}

integer Confusion_getNumberOfEntries (Confusion me) {
	const double total = NUMsum (my data.get());
	return Melder_ifloor (total);
}

static autoINTVEC create_index (constSTRVEC const& s, constSTRVEC const& ref) {
	autoINTVEC index = raw_INTVEC (s.size);
	for (integer i = 1; i <= s.size; i ++) {
		integer indxj = 0;
		for (integer j = 1; j <= ref.size; j ++) {
			if (str32equ (s [i], ref [j])) {
				indxj = j;
				break;
			}
		}
		index [i] = indxj;
	}
	return index;
}

autoConfusion Confusion_condense (Confusion me, conststring32 search, conststring32 replace,
	integer maximumNumberOfReplaces, bool use_regexp) {
	try {
		integer nmatches, nstringmatches;
		Melder_require (my rowLabels && my columnLabels,
			U"Both row and column labels should be present.");
		
		autoSTRVEC rowLabels = string32vector_searchAndReplace (my rowLabels.get(),
				search, replace, maximumNumberOfReplaces, & nmatches, & nstringmatches, use_regexp);

		autoSTRVEC columnLabels = string32vector_searchAndReplace (my columnLabels.get(),
				search, replace, maximumNumberOfReplaces, & nmatches, & nstringmatches, use_regexp);

		autoStrings srow = Thing_new (Strings);
		srow -> numberOfStrings = my numberOfRows;
		srow -> strings = std::move (rowLabels);

		autoStrings scol = Thing_new (Strings);
		scol -> numberOfStrings = my numberOfColumns;
		scol -> strings = std::move (columnLabels);

		/* Find dimension of new Confusion */
		autoDistributions dcol = Strings_to_Distributions (scol.get());
		const integer nresp = dcol -> numberOfRows;

		autoDistributions drow = Strings_to_Distributions (srow.get());
		const integer nstim = drow -> numberOfRows;

		autoConfusion thee = Confusion_create (nstim, nresp);

		thy rowLabels.all() <<= drow -> rowLabels.all();
		thy columnLabels.all() <<= dcol -> rowLabels.all();

		autoINTVEC rowIndex = create_index (srow -> strings.get(), drow -> rowLabels.get());
		autoINTVEC columnIndex = create_index (scol -> strings.get(), dcol -> rowLabels.get());

		for (integer i = 1; i <= my numberOfRows; i ++)
			for (integer j = 1; j <= my numberOfColumns; j ++)
				thy data [rowIndex [i]] [columnIndex [j]] += my data [i] [j];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not condensed.");
	}
}

autoConfusion TableOfReal_to_Confusion (TableOfReal me) {
	try {
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoConfusion thee = Thing_new (Confusion);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Confusion.");
	}
}

autoConfusion Confusion_group (Confusion me, conststring32 labels, conststring32 newLabel, integer newpos) {
	try {
		autoConfusion stim = Confusion_groupStimuli (me, labels, newLabel, newpos);
		autoConfusion thee = Confusion_groupResponses (stim.get(), labels, newLabel, newpos);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not grouped.");
	}
}

autoConfusion Confusion_groupStimuli (Confusion me, conststring32 labels_string, conststring32 newLabel, integer newpos) {
	try {
		autoSTRVEC labels = splitByWhitespace_STRVEC (labels_string);
		const integer ncondense = labels.size;
		autoINTVEC irow = to_INTVEC (my numberOfRows);

		for (integer itoken = 1; itoken <= labels.size; itoken ++) {
			conststring32 token = labels [itoken].get();
			for (integer i = 1; i <= my numberOfRows; i ++) {
				if (Melder_equ (token, my rowLabels [i].get())) {
					irow [i] = 0;
					break;
				}
			}
		}
		integer nfound = 0;
		for (integer i = 1; i <= my numberOfRows; i ++) {
			if (irow [i] == 0)
				nfound ++;
		}
		Melder_require (nfound > 0,
			U"The stimulus labels are invalid.");
		
		if (nfound != ncondense)
			Melder_warning (U"One or more of the given stimulus labels are suspect.");
		const integer newnstim = my numberOfRows - nfound + 1;
		if (newpos < 1)
			newpos = 1;
		if (newpos > newnstim)
			newpos = newnstim;
		autoConfusion thee = Confusion_create (newnstim, my numberOfColumns);
		thy columnLabels.all() <<= my columnLabels.all();
		TableOfReal_setRowLabel (thee.get(), newpos, newLabel);
		integer inewrow = 1;
		for (integer i = 1; i <= my numberOfRows; i ++) {
			integer rowpos = newpos;
			if (irow [i] > 0) {
				if (inewrow == newpos)
					inewrow ++;
				rowpos = inewrow;
				inewrow ++;
				TableOfReal_setRowLabel (thee.get(), rowpos, my rowLabels [i].get());
			}
			for (integer j = 1; j <= my numberOfColumns; j ++)
				thy data [rowpos] [j] += my data [i] [j];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": stimuli not grouped.");
	}
}

autoConfusion Confusion_groupResponses (Confusion me, conststring32 labels_string, conststring32 newLabel, integer newpos) {
	try {
		autoSTRVEC labels = splitByWhitespace_STRVEC (labels_string);
		const integer ncondense = labels.size;
		autoINTVEC icol = to_INTVEC (my numberOfColumns);

		for (integer itoken = 1; itoken <= labels.size; itoken ++) {
			const conststring32 token = labels [itoken].get();
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				if (Melder_equ (token, my columnLabels [i].get())) {
					icol [i] = 0;
					break;
				}
			}
		}
		integer nfound = 0;
		for (integer i = 1; i <= my numberOfColumns; i ++) {
			if (icol [i] == 0)
				nfound ++;
		}
		Melder_require (nfound > 0,
			U"The response labels are invalid.");
		
		if (nfound != ncondense)
			Melder_warning (U"One or more of the given response labels are suspect.");
		const integer newnresp = my numberOfColumns - nfound + 1;
		if (newpos < 1)
			newpos = 1;
		if (newpos > newnresp)
			newpos = newnresp;
		autoConfusion thee = Confusion_create (my numberOfRows, newnresp);
		thy rowLabels.all() <<= my rowLabels.all();
		TableOfReal_setColumnLabel (thee.get(), newpos, newLabel);
		integer inewcol = 1;
		for (integer i = 1; i <= my numberOfColumns; i ++) {
			integer colpos = newpos;
			if (icol [i] > 0) {
				if (inewcol == newpos)
					inewcol ++;
				colpos = inewcol;
				inewcol ++;
				TableOfReal_setColumnLabel (thee.get(), colpos, my columnLabels [i].get());
			}
			thy data.column (colpos)  +=  my data.column (i);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": responses not grouped.");
	}
}

autoTableOfReal Confusion_to_TableOfReal_marginals (Confusion me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows + 1, my numberOfColumns + 1);

		thy data.part(1, my numberOfRows, 1, my numberOfColumns) <<= my data.get();
		autoVEC columnSums = columnSums_VEC (my data.get());
		thy data.row (my numberOfRows + 1).part (1, my numberOfColumns) <<= columnSums.get();
		autoVEC rowSums = rowSums_VEC (my data.get());
		thy data.column (my numberOfColumns + 1).part (1, my numberOfRows) <<= rowSums.get();
		
		thy data [my numberOfRows + 1] [my numberOfColumns + 1] = NUMsum (rowSums.get());
		
		thy rowLabels.part (1, my numberOfRows) <<= my rowLabels.all();
		thy columnLabels.part (1, my numberOfColumns) <<= my columnLabels.all();
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
