/* EditDistanceTable.c
 *
 * Copyright (C) 2012, 2014-2016 David Weenink
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
  djmw 20120407 First implementation
*/

#include "EditDistanceTable.h"

#include "oo_DESTROY.h"
#include "EditDistanceTable_def.h"
#include "oo_COPY.h"
#include "EditDistanceTable_def.h"
#include "oo_EQUAL.h"
#include "EditDistanceTable_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EditDistanceTable_def.h"
#include "oo_WRITE_TEXT.h"
#include "EditDistanceTable_def.h"
#include "oo_WRITE_BINARY.h"
#include "EditDistanceTable_def.h"
#include "oo_READ_TEXT.h"
#include "EditDistanceTable_def.h"
#include "oo_READ_BINARY.h"
#include "EditDistanceTable_def.h"
#include "oo_DESCRIPTION.h"
#include "EditDistanceTable_def.h"

// prototypes
autoEditCostsTable EditCostsTable_createDefault ();

/* The insertion, deletion and substitution costs are specified in a TableOfReal
 * 1..n-2 target symbols/alphabet
 * 1..m-2 source symbols/alphabet
 * row n-1 and col m-1 specify nomatch symbols
 * cells [n][1..m-2] specify insertion costs
 * cells [1..n-1][m] specify deletion costs
 * cell [n-1][m-1] if nomatch target == nomatch source
 * cell [n][m] if nomatch target != nomatch source
 */

Thing_implement (WarpingPath, Daata, 0);

autoWarpingPath WarpingPath_create (long length) {
	try {
		autoWarpingPath me = Thing_new (WarpingPath);
		my path = NUMvector<structPairOfInteger> (1, length);
		my _capacity = my pathLength = length;
		return me;
	} catch (MelderError) {
		Melder_throw (U"WarpPath not created.");
	}
}

static void WarpingPath_reset (WarpingPath me) {
	for (long i = 1; i <= my _capacity; i++) {
		my path[i].x = my path[i].y = 0;
	}
	my pathLength = my _capacity;
}

static void WarpingPath_getPath (WarpingPath me, short **psi, long iy, long ix) { // psi[0..nrows-1][0..ncols-1]
	long index = my pathLength;
	my path[index].x = ix;
	my path[index].y = iy;
	while (!(ix == 0 && iy == 0)) {
		if (psi[iy][ix] == WARPING_fromLeft) {
			ix--;
		} else if (psi[iy][ix] == WARPING_fromBelow) {
			iy--;
		} else { // WARPING_fromDiag
			ix--; iy--;
		}
		my path[--index].x = ix;
		my path[index].y = iy;
	}
	if (index > 1) {
		long k = 1;
		for (long i = index; i <= my pathLength; i++) {
			my path[k++] = my path[i];
			my path[i].x = my path[i].y = 0;
		}
		my pathLength = k - 1;
	}
}

static void WarpingPath_shiftPathByOne (WarpingPath me) {
	for (long i = 1; i <= my pathLength; i++) {
		(my path[i].x)++; (my path[i].y)++;
	}
}

long WarpingPath_getColumnsFromRowIndex (WarpingPath me, long iy, long *p_ix1, long *p_ix2) {
	long ix1 = 0, ix2 = 0, numberOfColumns = 0;
	if (iy > 0) {
		for (long i = 1; i <= my pathLength; i++) {
			if (my path[i].y < iy) {
				continue;
			} else if (my path[i].y == iy) {
				if (ix1 == 0) {
					ix1 = my path[i].x;
				}
				ix2 = my path[i].x;
			} else {
				break;
			}
		}
		numberOfColumns = ix2 - ix1 + 1;
	}
	if (p_ix1) {
		*p_ix1 = ix1;
	}
	if (p_ix2) {
		*p_ix2 = ix2;
	}
	return numberOfColumns;
}

long WarpingPath_getRowsFromColumnIndex (WarpingPath me, long ix, long *p_iy1, long *p_iy2) {
	if (ix <= 0) {
		return 0;
	}
	long iy1 = 0, iy2 = 0, numberOfRows = 0;
	if (ix > 0) {
		for (long i = 1; i <= my pathLength; i++) {
			if (my path[i].x < ix) {
				continue;
			} else if (my path[i].x == ix) {
				if (iy1 == 0) {
					iy1 = my path[i].y;
				}
				iy2 = my path[i].y;
			} else {
				break;
			}
		}
		numberOfRows = iy2 - iy1 + 1;
	}
	if (p_iy1) {
		*p_iy1 = iy1;
	}
	if (p_iy2) {
		*p_iy2 = iy2;
	}
	return numberOfRows;
}

Thing_implement (EditCostsTable, TableOfReal, 0);

void structEditCostsTable :: v_info () {
	EditDistanceTable_Parent :: v_info ();
	MelderInfo_writeLine (U"Target:", numberOfRows - 2, U" symbols.");
	MelderInfo_writeLine (U"Source:", numberOfColumns - 2, U" symbols.");
}

bool structEditCostsTable :: v_matchTargetSymbol (const char32 *targetSymbol, const char32 *symbol) {
	return Melder_equ (targetSymbol, symbol);
}

bool structEditCostsTable :: v_matchSourceSymbol (const char32 *sourceSymbol, const char32 *symbol) {
	return Melder_equ (sourceSymbol, symbol);
}

bool structEditCostsTable :: v_matchTargetWithSourceSymbol (const char32 *targetSymbol, const char32 *sourceSymbol) {
	return Melder_equ (targetSymbol, sourceSymbol);
}

autoEditCostsTable EditCostsTable_create (long targetAlphabetSize, long sourceAlphabetSize) {
	try{
		autoEditCostsTable me = Thing_new (EditCostsTable);
		TableOfReal_init (me.peek(), targetAlphabetSize + 2, sourceAlphabetSize + 2);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditCostsTable not created.");
	}
}

autoEditCostsTable EditCostsTable_createDefault () {
	try {
		autoEditCostsTable me = EditCostsTable_create (0, 0);
		my data[1][1] = 0; // default substitution cost (nomatch == nomatch)
		my data[2][2] = 2; // default substitution cost (nomatch != nomatch)
		my data[2][1] = 1; // default insertion cost
		my data[1][2] = 1; // default deletion cost
		return me;
	} catch (MelderError) {
		Melder_throw (U"Default EditCostsTable not created.");
	}
}

void EditCostsTable_setDefaultCosts (EditCostsTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	my data[my numberOfRows - 1][my numberOfColumns - 1] = 0;
	my data[my numberOfRows][my numberOfColumns] = substitutionCosts;
	my data[my numberOfRows][my numberOfColumns - 1] = deletionCosts;
	my data[my numberOfRows - 1][my numberOfColumns] = insertionCosts;
}

long EditCostsTable_getTargetIndex (EditCostsTable me, const char32 *symbol) {
	for (long i = 1; i <= my numberOfRows - 2; i++) {
		if (my v_matchTargetSymbol (my rowLabels[i], symbol)) {
			return i;
		}
	}
	return 0;
}

long EditCostsTable_getSourceIndex (EditCostsTable me, const char32 *symbol) {
	for (long j = 1; j <= my numberOfColumns - 2; j++) {
		if (my v_matchSourceSymbol (my columnLabels[j], symbol)) {
			return j;
		}
	}
	return 0;
}

void EditCostsTable_setInsertionCosts (EditCostsTable me, char32 *targets, double cost) {
	for (char32 *token = Melder_firstToken (targets); token != 0; token = Melder_nextToken ()) {
		long irow = EditCostsTable_getTargetIndex (me, token);
		irow = irow > 0 ? irow : my numberOfRows - 1; // nomatch condition to penultimate row
		my data[irow][my numberOfColumns] = cost;
	}
}

void EditCostsTable_setDeletionCosts (EditCostsTable me, char32 *sources, double cost) {
	for (char32 *token = Melder_firstToken (sources); token != 0; token = Melder_nextToken ()) {
		long icol = EditCostsTable_getSourceIndex (me, token);
		icol = icol > 0 ? icol : my numberOfColumns - 1; // nomatch condition to penultimate column
		my data[my numberOfRows][icol] = cost;
	}
}

void EditCostsTable_setOthersCosts (EditCostsTable me, double insertionCost, double deletionCost, double substitutionCost_equal, double substitutionCost_unequal) {
	my data[my numberOfRows - 1][my numberOfColumns] = insertionCost;
	my data[my numberOfRows][my numberOfColumns - 1] = deletionCost;
	my data[my numberOfRows - 1][my numberOfColumns - 1] = substitutionCost_unequal;
	my data[my numberOfRows][my numberOfColumns] = substitutionCost_equal;
}

double EditCostsTable_getOthersCost (EditCostsTable me, int costType) {
	return costType == 1 ? my data[my numberOfRows - 1][my numberOfColumns] : //insertion
		costType == 2 ? my data[my numberOfRows][my numberOfColumns - 1] : // deletion
		costType == 3 ? my data[my numberOfRows][my numberOfColumns] : // equality
		 my data[my numberOfRows - 1][my numberOfColumns -1]; // inequality
}

void EditCostsTable_setSubstitutionCosts (EditCostsTable me, char32 *targets, char32 *sources, double cost) {
	try {
		autoNUMvector<long> targetIndex (1, my numberOfRows);
		autoNUMvector<long> sourceIndex (1, my numberOfRows);
		long numberOfTargetSymbols = 0;
		for (char32 *token = Melder_firstToken (targets); token != 0; token = Melder_nextToken ()) {
			long index = EditCostsTable_getTargetIndex (me, token);
			if (index > 0) {
				targetIndex[++numberOfTargetSymbols] = index;
			}
		}
		if (numberOfTargetSymbols == 0) {
			targetIndex[++numberOfTargetSymbols] = my numberOfRows - 1;
		}
		long numberOfSourceSymbols = 0;
		for (char32 *token = Melder_firstToken (sources); token != 0; token = Melder_nextToken ()) {
			long index = EditCostsTable_getSourceIndex (me, token);
			if (index > 0) {
				sourceIndex[++numberOfSourceSymbols] = index;
			}
		}
		if (numberOfSourceSymbols == 0) {
			sourceIndex[++numberOfSourceSymbols] = my numberOfColumns - 1;
		}
		for (long i = 1; i <= numberOfTargetSymbols; i++) {
			long irow = targetIndex[i];
			for (long j = 1; j <= numberOfSourceSymbols; j++) {
				my data [irow][sourceIndex[j]] = cost;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": substitution costs not set.");
	}
}

double EditCostsTable_getInsertionCost (EditCostsTable me, const char32 *symbol) {
	long irow = EditCostsTable_getTargetIndex (me, symbol);
	irow = irow == 0 ? my numberOfRows - 1 : irow; // others is penultimate row
	return my data[irow][my numberOfColumns];
}

double EditCostsTable_getDeletionCost (EditCostsTable me, const char32 *sourceSymbol) {
	long icol = EditCostsTable_getSourceIndex (me, sourceSymbol);
	icol = icol == 0 ? my numberOfColumns - 1 : icol; // others is penultimate column
	return my data[my numberOfRows][icol];
}

double EditCostsTable_getSubstitutionCost (EditCostsTable me, const char32 *symbol, const char32 *replacement) {
	long irow = EditCostsTable_getTargetIndex (me, symbol);
	long icol = EditCostsTable_getSourceIndex (me, replacement);
	if (irow == 0 && icol == 0) { // nomatch
		irow = my numberOfRows;
		icol = my numberOfColumns;
		if (my v_matchTargetWithSourceSymbol (symbol, replacement)) {
			--irow;
			--icol;
		}
	} else {
		irow = irow == 0 ? my numberOfRows - 1 : irow;
		icol = icol == 0 ? my numberOfColumns - 1 : icol;
	}
	return my data[irow][icol];
}

autoTableOfReal EditCostsTable_to_TableOfReal (EditCostsTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (long j = 1; j <= my numberOfColumns; j++) {
			thy columnLabels[j] = Melder_dup (my columnLabels[j]);
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			thy rowLabels[i] = Melder_dup (my rowLabels[i]);
		}
		NUMmatrix_copyElements<double> (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee;

	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

Thing_implement (EditDistanceTable, TableOfReal, 0);

void structEditDistanceTable :: v_info () {
	EditDistanceTable_Parent :: v_info ();
	MelderInfo_writeLine (U"Target:", numberOfRows, U" symbols.");
	MelderInfo_writeLine (U"Source:", numberOfColumns, U" symbols.");
}

autoEditDistanceTable EditDistanceTable_create (Strings target, Strings source) {
	try {
		autoEditDistanceTable me = Thing_new (EditDistanceTable);
		long numberOfSourceSymbols = source -> numberOfStrings, numberOfTargetSymbols = target -> numberOfStrings;
		TableOfReal_init (me.peek(), numberOfTargetSymbols + 1, numberOfSourceSymbols + 1);
		TableOfReal_setColumnLabel (me.peek(), 1, U"");
		for (long j = 1; j <= numberOfSourceSymbols; j++) {
			my columnLabels[j + 1] = Melder_dup (source -> strings[j]);
		}
		TableOfReal_setRowLabel (me.peek(), 1, U"");
		for (long i = 1; i <= numberOfTargetSymbols; i++) {
			my rowLabels[i + 1] = Melder_dup (target -> strings[i]);
		}
		my warpingPath = WarpingPath_create (numberOfTargetSymbols + numberOfSourceSymbols + 1);
		my editCostsTable = EditCostsTable_createDefault ();
		EditDistanceTable_findPath (me.peek(), 0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditDistanceTable not created.");
	}
}

void EditDistanceTable_setEditCosts (EditDistanceTable me, EditCostsTable thee) {
	try {
		my editCostsTable = Data_copy (thee);
	} catch (MelderError) {
		Melder_throw (me, U": edit costs not set.");
	}
}

autoEditDistanceTable EditDistanceTable_createFromCharacterStrings (const char32 *chars1, const char32 *chars2) {
	try {
		autoStrings s1 = Strings_createAsCharacters (chars1);
		autoStrings s2 = Strings_createAsCharacters (chars2);
		autoEditDistanceTable me = EditDistanceTable_create (s1.peek(), s2.peek());
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditDistanceTable not created from character strings.");
	}
}

static void NUMrationalize (double x, long *numerator, long *denominator) {
	double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator, rounded = round (numerator_d);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = (long) rounded;
			return;
		}
	}
	*denominator = 0;   /* Failure. */
}

static void fixRows (TableOfReal me, long *rowmin, long *rowmax) {
	if (*rowmax < *rowmin) { *rowmin = 1; *rowmax = my numberOfRows; }
	else if (*rowmin < 1) *rowmin = 1;
	else if (*rowmax > my numberOfRows) *rowmax = my numberOfRows;
}

static void print4 (char *buffer, double value, int iformat, int width, int precision) {
	char formatString [40];
	if (iformat == 4) {
		long numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			snprintf (buffer, 40, "0");
		else if (denominator > 1)
			snprintf (buffer, 40, "%ld/%ld", numerator, denominator);
		else
			snprintf (buffer, 40, "%.7g", value);
	} else {
		snprintf (formatString, 40, "%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		snprintf (buffer, 40, formatString, value);
	}
}

static double getMaxRowLabelWidth (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	double maxWidth = 0.0;
	if (! my rowLabels) return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (long irow = rowmin; irow <= rowmax; irow ++) if (my rowLabels [irow] && my rowLabels [irow] [0]) {
		double textWidth = Graphics_textWidth_ps (graphics, my rowLabels [irow], true);   /* SILIPA is bigger than XIPA */
		if (textWidth > maxWidth) maxWidth = textWidth;
	}
	return maxWidth;
}

static double getLeftMargin (Graphics graphics) {
	return Graphics_dxMMtoWC (graphics, 1);
}

static double getLineSpacing (Graphics graphics) {
	return Graphics_dyMMtoWC (graphics, 1.5 * Graphics_inqFontSize (graphics) * 25.4 / 72);
}

void EditDistanceTable_draw (EditDistanceTable me, Graphics graphics, int iformat, int precision, double angle) {
	long rowmin = 1, rowmax = my numberOfRows;
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0, 1);
	double leftMargin = getLeftMargin (graphics);   // not earlier!
	double lineSpacing = getLineSpacing (graphics);   // not earlier!
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double y = 1 + 0.1 * lineSpacing;
	autoNUMmatrix<bool> onPath (1, my numberOfRows, 1, my numberOfColumns);
	for (long i = 1; i <= my warpingPath -> pathLength; i++) {
		structPairOfInteger poi = my warpingPath -> path[i];
		onPath[poi.y] [poi.x] = true;
	}

	for (long irow = my numberOfRows; irow > 0; irow --) {
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
			Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow]);
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			char text [40];
			print4 (text, my data [irow] [icol], iformat, 0, precision);
			Graphics_setBold (graphics, onPath[irow][icol]);
			Graphics_text (graphics, icol, y, Melder_peek8to32 (text));
			if (onPath[irow][icol]) {
				Graphics_rectangle (graphics, icol-0.5, icol+0.5, y - 0.5*lineSpacing, y + 0.5*lineSpacing);
			}
		}
		y -= lineSpacing;
		Graphics_setBold (graphics, false);
	}

	double left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
	Graphics_line (graphics, left, y, my numberOfColumns + 0.5, y);

	Graphics_setTextRotation (graphics, angle);
	if (angle < 0) {
		y -= 0.3*lineSpacing;
		Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	} else if (angle > 0) {
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		y -= 0.3*lineSpacing;
	} else {
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_TOP);
	}
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
			Graphics_text (graphics, icol, y, my columnLabels [icol]);
	}
	Graphics_setTextRotation (graphics, 0);
	y -= lineSpacing;
	Graphics_line (graphics, 0.5, y, 0.5, 1 + 0.5 * lineSpacing);
	Graphics_unsetInner (graphics);
}

void EditDistanceTable_drawEditOperations (EditDistanceTable me, Graphics graphics) {
	const char32 *oinsertion = U"i", *insertion = U"*", *odeletion = U"d", *deletion = U"*", *osubstitution = U"s", *oequal = U"";
	Graphics_setWindow (graphics, 0.5, my warpingPath -> pathLength - 0.5, 0, 1); // pathLength-1 symbols
	double lineSpacing = getLineSpacing (graphics);
	double ytarget = 1 - lineSpacing, ysource = ytarget - 2 * lineSpacing, yoper = ysource - lineSpacing;
	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
	for (long i = 2; i <= my warpingPath -> pathLength; i++) {
		structPairOfInteger p = my warpingPath -> path[i], p1 = my warpingPath -> path[i - 1];
		double x = i - 1;
		if (p.x == p1.x) { // insertion
			Graphics_text (graphics, x, ytarget, my rowLabels[p.y]);
			Graphics_text (graphics, x, ysource, deletion);
			Graphics_text (graphics, x, yoper, oinsertion);
		} else if (p.y == p1.y) { // deletion
			Graphics_text (graphics, x, ytarget, insertion);
			Graphics_text (graphics, x, ysource, my columnLabels[p.x]);
			Graphics_text (graphics, x, yoper, odeletion);
		} else { // substitution ?
			Graphics_text (graphics, x, ytarget, my rowLabels[p.y]);
			Graphics_text (graphics, x, ysource, my columnLabels[p.x]);
			Graphics_text (graphics, x, yoper, (Melder_equ (my rowLabels[p.y], my columnLabels[p.x]) ? oequal : osubstitution));
		}
		Graphics_line (graphics, x, ysource + lineSpacing, x, ytarget - 0.1 * lineSpacing);
	}
}

void EditDistanceTable_setDefaultCosts (EditDistanceTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	EditCostsTable_setDefaultCosts (my editCostsTable.peek(), insertionCosts, deletionCosts, substitutionCosts);
	EditDistanceTable_findPath (me, 0);
}

autoTableOfReal EditDistanceTable_to_TableOfReal_directions (EditDistanceTable me) {
		autoTableOfReal tor;
		EditDistanceTable_findPath (me, &tor);
		return tor;
}

void EditDistanceTable_findPath (EditDistanceTable me, autoTableOfReal *directions) {
	try {
		/* What do we have to do to source to get target?
		 * Origin [0][0] is at bottom-left corner
		 * Target vertical, source horizontal
		 * Going in the vertical direction is a deletion, horizontal is insertion, diagonal is substitution
		 */
		long numberOfSources = my numberOfColumns - 1, numberOfTargets = my numberOfRows - 1;
		autoNUMmatrix<short> psi (0, numberOfTargets, 0, numberOfSources);
		autoNUMmatrix<double> delta (0, numberOfTargets, 0, numberOfSources);

		for (long j = 1; j <= numberOfSources; j++) {
			delta[0][j] = delta[0][j - 1] + EditCostsTable_getDeletionCost (my editCostsTable.peek(), my columnLabels[j+1]);
			psi[0][j] = WARPING_fromLeft;
		}
		for (long i = 1; i <= numberOfTargets; i++) {
			delta[i][0] = delta[i - 1][0] + EditCostsTable_getInsertionCost (my editCostsTable.peek(), my rowLabels[i+1]);
			psi[i][0] = WARPING_fromBelow;
		}
		for (long j = 1; j <= numberOfSources; j++) {
			for (long i = 1; i <= numberOfTargets; i++) {
				// the substitution, deletion and insertion costs.
				double left = delta[i][j - 1] + EditCostsTable_getInsertionCost (my editCostsTable.peek(), my rowLabels[i+1]);
				double bottom = delta[i - 1][j] + EditCostsTable_getDeletionCost (my editCostsTable.peek(), my columnLabels[j+1]);
				double mindist = delta[i - 1][j - 1] + EditCostsTable_getSubstitutionCost (my editCostsTable.peek(), my rowLabels[i+1], my columnLabels[j+1]); // diag
				psi[i][j] = WARPING_fromDiag;
				if (bottom < mindist) {
					mindist = bottom;
					psi[i][j] = WARPING_fromBelow;
				}
				if (left < mindist) {
					mindist = left;
					psi[i][j] = WARPING_fromLeft;
				}
				delta[i][j] = mindist;
			}
		}
		// find minimum distance in last column
		long iy = numberOfTargets, ix = numberOfSources;

		WarpingPath_reset (my warpingPath.peek());

		WarpingPath_getPath (my warpingPath.peek(), psi.peek(), iy, ix);

		WarpingPath_shiftPathByOne (my warpingPath.peek());

		for (long i = 0; i <= numberOfTargets; i++) {
			for (long j = 0; j <= numberOfSources; j++) {
				my data[i+1][j+1] = delta[i][j];
			}
		}
		if (directions != 0) {
			autoTableOfReal him = EditDistanceTable_to_TableOfReal (me);
			for (long i = 0; i <= numberOfTargets; i++) {
				for (long j = 0; j <= numberOfSources; j++) {
					his data[i+1][j+1] = psi[i][j];
				}
			}
			*directions = him.move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": minimum path not found.");
	}
}

autoTableOfReal EditDistanceTable_to_TableOfReal (EditDistanceTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (long j = 1; j <= my numberOfColumns; j++) {
			thy columnLabels[j] = Melder_dup (my columnLabels[j]);
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			thy rowLabels[i] = Melder_dup (my rowLabels[i]);
		}
		NUMmatrix_copyElements<double> (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no TableOfReal created.");
	}
}

/* End of file EditDistanceTable.cpp */
