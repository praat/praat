/* EditDistanceTable.c
 *
 * Copyright (C) 2012 David Weenink
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
EditCostsTable EditCostsTable_createDefault ();

/* The insertion, deletion and substitution costs are specified in a TableOfReal
 * 1..n-2 target symbols/alphabet
 * 1..m-2 source symbols/alphabet
 * row n-1 and col m-1 specify nomatch symbols
 * cells [n][1..m-2] specify insertion costs
 * cells [1..n-1][m] specify deletion costs
 * cell [n-1][m-1] if nomatch target == nomatch source
 * cell [n][m] if nomatch target != nomatch source
 */

Thing_implement (WarpingPath, Data, 0);

WarpingPath WarpingPath_create (long length) {
	try {
		autoWarpingPath me = Thing_new (WarpingPath);
		my d_path = NUMvector<structPairOfInteger> (1, length);
		my _capacity = my d_pathLength = length;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("WarpPath not created.");
	}
}

static void WarpingPath_reset (WarpingPath me) {
	for (long i = 1; i <= my _capacity; i++) {
		my d_path[i].x = my d_path[i].y = 0;
	}
	my d_pathLength = my _capacity;
}

static void WarpingPath_getPath (WarpingPath me, short **psi, long iy, long ix) { // psi[0..nrows-1][0..ncols-1]
	long index = my d_pathLength;
	my d_path[index].x = ix;
	my d_path[index].y = iy;
	while (!(ix == 0 && iy == 0)) {
		if (psi[iy][ix] == WARPING_fromLeft) {
			ix--;
		} else if (psi[iy][ix] == WARPING_fromBelow) {
			iy--;
		} else { // WARPING_fromDiag
			ix--; iy--;
		}
		my d_path[--index].x = ix;
		my d_path[index].y = iy;
	}
	if (index > 1) {
		long k = 1;
		for (long i = index; i <= my d_pathLength; i++) {
			my d_path[k++] = my d_path[i];
			my d_path[i].x = my d_path[i].y = 0;
		}
		my d_pathLength = k - 1;
	}
}

static void WarpingPath_shiftPathByOne (WarpingPath me) {
	for (long i = 1; i <= my d_pathLength; i++) {
		(my d_path[i].x)++; (my d_path[i].y)++;
	}
}

long WarpingPath_getColumnsFromRowIndex (WarpingPath me, long iy, long *ix1, long *ix2) {
	if (iy <= 0) {
		return 0;
	}
	*ix1 = 0; *ix2 = 0;
	for (long i = 1; i <= my d_pathLength; i++) {
		if (my d_path[i].y < iy) {
			continue;
		} else if (my d_path[i].y == iy) {
			if (*ix1 == 0) *ix1 = my d_path[i].x;
			*ix2 = my d_path[i].x;
		} else {
			break;
		}
	}
	return *ix2 - *ix1 + 1;
}

long WarpingPath_getRowsFromColumnIndex (WarpingPath me, long ix, long *iy1, long *iy2) {
	if (ix <= 0) {
		return 0;
	}
	*iy1 = 0; *iy2 = 0;
	for (long i = 1; i <= my d_pathLength; i++) {
		if (my d_path[i].x < ix) {
			continue;
		} else if (my d_path[i].x == ix) {
			if (*iy1 == 0) *iy1 = my d_path[i].y;
			*iy2 = my d_path[i].y;
		} else {
			break;
		}
	}
	return *iy2 - *iy1 + 1;
}

Thing_implement (EditCostsTable, TableOfReal, 0);

void structEditCostsTable :: v_info () {
	EditDistanceTable_Parent :: v_info ();
	MelderInfo_writeLine (L"Target:", Melder_integer (numberOfRows - 2), L" symbols.");
	MelderInfo_writeLine (L"Source:", Melder_integer (numberOfColumns - 2), L" symbols.");
}

bool structEditCostsTable :: v_matchTargetSymbol (const wchar_t *targetSymbol, const wchar_t *symbol) {
	return Melder_wcsequ (targetSymbol, symbol);
}

bool structEditCostsTable :: v_matchSourceSymbol (const wchar_t *sourceSymbol, const wchar_t *symbol) {
	return Melder_wcsequ (sourceSymbol, symbol);
}

bool structEditCostsTable :: v_matchTargetWithSourceSymbol (const wchar_t *targetSymbol, const wchar_t *sourceSymbol) {
	return Melder_wcsequ (targetSymbol, sourceSymbol);
}

EditCostsTable EditCostsTable_create (long targetAlphabetSize, long sourceAlphabetSize) {
	try{
		autoEditCostsTable me = Thing_new (EditCostsTable);
		TableOfReal_init (me.peek(), targetAlphabetSize + 2, sourceAlphabetSize + 2);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EditCostsTable not created.");
	}
}

EditCostsTable EditCostsTable_createDefault () {
	try {
		autoEditCostsTable me = EditCostsTable_create (0, 0);
		my data[1][1] = 0; // default substitution cost (nomatch == nomatch)
		my data[2][2] = 2; // default substitution cost (nomatch != nomatch)
		my data[2][1] = 1; // default insertion cost
		my data[1][2] = 1; // default deletion cost
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Default EditCostsTable not created.");
	}
}

void EditCostsTable_setDefaultCosts (EditCostsTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	my data[my numberOfRows - 1][my numberOfColumns - 1] = 0;
	my data[my numberOfRows][my numberOfColumns] = substitutionCosts;
	my data[my numberOfRows][my numberOfColumns - 1] = deletionCosts;
	my data[my numberOfRows - 1][my numberOfColumns] = insertionCosts;
}

long EditCostsTable_getTargetIndex (EditCostsTable me, const wchar_t *symbol) {
	for (long i = 1; i <= my numberOfRows - 2; i++) {
		if (my v_matchTargetSymbol (my rowLabels[i], symbol)) {
			return i;
		}
	}
	return 0;
}

long EditCostsTable_getSourceIndex (EditCostsTable me, const wchar_t *symbol) {
	for (long j = 1; j <= my numberOfColumns - 2; j++) {
		if (my v_matchSourceSymbol (my columnLabels[j], symbol)) {
			return j;
		}
	}
	return 0;
}

void EditCostsTable_setInsertionCosts (EditCostsTable me, wchar_t *targets, double cost) {
	for (wchar_t *token = Melder_firstToken (targets); token != 0; token = Melder_nextToken ()) {
		long irow = EditCostsTable_getTargetIndex (me, token);
		irow = irow > 0 ? irow : my numberOfRows - 1; // nomatch condition to penultimate row
		my data[irow][my numberOfColumns] = cost;
	}
}

void EditCostsTable_setDeletionCosts (EditCostsTable me, wchar_t *sources, double cost) {
	for (wchar_t *token = Melder_firstToken (sources); token != 0; token = Melder_nextToken ()) {
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

void EditCostsTable_setSubstitutionCosts (EditCostsTable me, wchar_t *targets, wchar_t *sources, double cost) {
	try {
		autoNUMvector<long> targetIndex (1, my numberOfRows);
		autoNUMvector<long> sourceIndex (1, my numberOfRows);
		long numberOfTargetSymbols = 0;
		for (wchar_t *token = Melder_firstToken (targets); token != 0; token = Melder_nextToken ()) {
			long index = EditCostsTable_getTargetIndex (me, token);
			if (index > 0) {
				targetIndex[++numberOfTargetSymbols] = index;
			}
		}
		if (numberOfTargetSymbols == 0) {
			targetIndex[++numberOfTargetSymbols] = my numberOfRows - 1;
		}
		long numberOfSourceSymbols = 0;
		for (wchar_t *token = Melder_firstToken (sources); token != 0; token = Melder_nextToken ()) {
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
		Melder_throw (me, ": substitution costs not set.");
	}
}

double EditCostsTable_getInsertionCost (EditCostsTable me, const wchar_t *symbol) {
	long irow = EditCostsTable_getTargetIndex (me, symbol);
	irow = irow == 0 ? my numberOfRows - 1 : irow; // others is penultimate row
	return my data[irow][my numberOfColumns];
}

double EditCostsTable_getDeletionCost (EditCostsTable me, const wchar_t *sourceSymbol) {
	long icol = EditCostsTable_getSourceIndex (me, sourceSymbol);
	icol = icol == 0 ? my numberOfColumns - 1 : icol; // others is penultimate column
	return my data[my numberOfRows][icol];
}

double EditCostsTable_getSubstitutionCost (EditCostsTable me, const wchar_t *symbol, const wchar *replacement) {
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

TableOfReal EditCostsTable_to_TableOfReal (EditCostsTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (long j = 1; j <= my numberOfColumns; j++) {
			thy columnLabels[j] = Melder_wcsdup (my columnLabels[j]);
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			thy rowLabels[i] = Melder_wcsdup (my rowLabels[i]);
		}
		NUMmatrix_copyElements<double> (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee.transfer();

	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

Thing_implement (EditDistanceTable, TableOfReal, 0);

void structEditDistanceTable :: v_info () {
	EditDistanceTable_Parent :: v_info ();
	MelderInfo_writeLine (L"Target:", Melder_integer (numberOfRows), L" symbols.");
	MelderInfo_writeLine (L"Source:", Melder_integer (numberOfColumns), L" symbols.");
}

EditDistanceTable EditDistanceTable_create (Strings target, Strings source) {
	try {
		autoEditDistanceTable me = Thing_new (EditDistanceTable);
		long numberOfSourceSymbols = source -> numberOfStrings, numberOfTargetSymbols = target -> numberOfStrings;
		TableOfReal_init (me.peek(), numberOfTargetSymbols + 1, numberOfSourceSymbols + 1);
		TableOfReal_setColumnLabel (me.peek(), 1, L"");
		for (long j = 1; j <= numberOfSourceSymbols; j++) {
			my columnLabels[j + 1] = Melder_wcsdup (source -> strings[j]);
		}
		TableOfReal_setRowLabel (me.peek(), 1, L"");
		for (long i = 1; i <= numberOfTargetSymbols; i++) {
			my rowLabels[i + 1] = Melder_wcsdup (target -> strings[i]);
		}
		my d_warpingPath = WarpingPath_create (numberOfTargetSymbols + numberOfSourceSymbols + 1);
		my d_editCostsTable = EditCostsTable_createDefault ();
		EditDistanceTable_findPath (me.peek(), 0);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EditDistanceTable not created.");
	}
}

void EditDistanceTable_setEditCosts (EditDistanceTable me, EditCostsTable thee) {
	try {
		forget (my d_editCostsTable);
		autoEditCostsTable ect = (EditCostsTable) Data_copy (thee);
		my d_editCostsTable = ect.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": edit costs not set.");
	}
}

EditDistanceTable EditDistanceTable_createFromCharacterStrings (const wchar_t *chars1, const wchar_t *chars2) {
	try {
		autoStrings s1 = Strings_createAsCharacters (chars1);
		autoStrings s2 = Strings_createAsCharacters (chars2);
		autoEditDistanceTable me = EditDistanceTable_create (s1.peek(), s2.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EditDistanceTable not created from character strings.");
	}
}

static void NUMrationalize (double x, long *numerator, long *denominator) {
	double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator, rounded = floor (numerator_d + 0.5);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = rounded;
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

static void print4 (wchar_t *buffer, double value, int iformat, int width, int precision) {
	wchar_t formatString [40];
	if (value == NUMundefined) wcscpy (buffer, L"undefined");
	else if (iformat == 4) {
		long numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			swprintf (buffer, 40, L"0");
		else if (denominator > 1)
			swprintf (buffer, 40, L"%ld/%ld", numerator, denominator);
		else
			swprintf (buffer, 40, L"%.7g", value);
	} else {
		swprintf (formatString, 40, L"%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		swprintf (buffer, 40, formatString, value);
	}
}

static double getMaxRowLabelWidth (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	double maxWidth = 0.0;
	if (! my rowLabels) return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (long irow = rowmin; irow <= rowmax; irow ++) if (my rowLabels [irow] && my rowLabels [irow] [0]) {
		double textWidth = Graphics_textWidth_ps (graphics, my rowLabels [irow], TRUE);   /* SILIPA is bigger than XIPA */
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
	for (long i = 1; i <= my d_warpingPath -> d_pathLength; i++) {
		structPairOfInteger poi = my d_warpingPath -> d_path[i];
		onPath[poi.y] [poi.x] = true;
	}

	for (long irow = my numberOfRows; irow > 0; irow --) {
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
			Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow]);
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			wchar_t text [40];
			print4 (text, my data [irow] [icol], iformat, 0, precision);
			Graphics_setBold (graphics, onPath[irow][icol]);
			Graphics_text (graphics, icol, y, text);
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
	const wchar_t *oinsertion = L"i", *insertion = L"*", *odeletion = L"d", *deletion = L"*", *osubstitution = L"s", *oequal = L"";
	Graphics_setWindow (graphics, 0.5, my d_warpingPath -> d_pathLength - 0.5, 0, 1); // pathLength-1 symbols
	double lineSpacing = getLineSpacing (graphics);
	double ytarget = 1 - lineSpacing, ysource = ytarget - 2 * lineSpacing, yoper = ysource - lineSpacing;
	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
	for (long i = 2; i <= my d_warpingPath -> d_pathLength; i++) {
		structPairOfInteger p = my d_warpingPath -> d_path[i], p1 = my d_warpingPath -> d_path[i - 1];
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
			Graphics_text (graphics, x, yoper, (Melder_wcsequ (my rowLabels[p.y], my columnLabels[p.x]) ? oequal : osubstitution));
		}
		Graphics_line (graphics, x, ysource + lineSpacing, x, ytarget - 0.1 * lineSpacing);
	}
}

void EditDistanceTable_setDefaultCosts (EditDistanceTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	EditCostsTable_setDefaultCosts (my d_editCostsTable, insertionCosts, deletionCosts, substitutionCosts);
	EditDistanceTable_findPath (me, 0);
}

TableOfReal EditDistanceTable_to_TableOfReal_directions (EditDistanceTable me) {
		TableOfReal tor;
		EditDistanceTable_findPath (me, &tor);
		return tor;
}

void EditDistanceTable_findPath (EditDistanceTable me, TableOfReal *directions) {
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
			delta[0][j] = delta[0][j - 1] + EditCostsTable_getDeletionCost (my d_editCostsTable, my columnLabels[j+1]);
			psi[0][j] = WARPING_fromLeft;
		}
		for (long i = 1; i <= numberOfTargets; i++) {
			delta[i][0] = delta[i - 1][0] + EditCostsTable_getInsertionCost (my d_editCostsTable, my rowLabels[i+1]);
			psi[i][0] = WARPING_fromBelow;
		}
		for (long j = 1; j <= numberOfSources; j++) {
			for (long i = 1; i <= numberOfTargets; i++) {
				// the substitution, deletion and insertion costs.
				double left = delta[i][j - 1] + EditCostsTable_getInsertionCost (my d_editCostsTable, my rowLabels[i+1]);
				double bottom = delta[i - 1][j] + EditCostsTable_getDeletionCost (my d_editCostsTable, my columnLabels[j+1]);
				double mindist = delta[i - 1][j - 1] + EditCostsTable_getSubstitutionCost (my d_editCostsTable, my rowLabels[i+1], my columnLabels[j+1]); // diag
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

		WarpingPath_reset (my d_warpingPath);

		WarpingPath_getPath (my d_warpingPath, psi.peek(), iy, ix);

		WarpingPath_shiftPathByOne (my d_warpingPath);

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
			*directions = him.transfer();
		}
	} catch (MelderError) {
		Melder_throw (me, ": minimum path not found.");
	}
}

TableOfReal EditDistanceTable_to_TableOfReal (EditDistanceTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		for (long j = 1; j <= my numberOfColumns; j++) {
			thy columnLabels[j] = Melder_wcsdup (my columnLabels[j]);
		}
		for (long i = 1; i <= my numberOfRows; i++) {
			thy rowLabels[i] = Melder_wcsdup (my rowLabels[i]);
		}
		NUMmatrix_copyElements<double> (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no TableOfReal created.");
	}
}

/* End of file EditDistanceTable.cpp */
