/* TableOfReal.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/09/15 TableOfReal_readFromHeaderlessSpreadsheetFile: guard against cells with strings
 * dw 2004/02/27 drawAsSquares: fill colour same as outline colour
 * pb 2004/05/09 more Extract Part and Extract commands
 * pb 2004/10/01 Melder_double instead of %.17g
 * pb 2005/03/04 Melder_NUMBER and Melder_STRING as enums
 * pb 2005/06/16 Melder_NUMBER and Melder_STRING as ints
 * pb 2005/07/19 TableOfReal_readFromHeaderlessSpreadsheetFile: allow 30k row and column labels
 * pb 2005/09/18 SILIPA versus XIPA widths
 * pb 2006/04/17 getRowStr, getColStr
 * pb 2006/12/10 MelderInfo
 * pb 2007/03/17 moved Table stuff here
 * pb 2007/06/21 tex
 * pb 2007/10/01 can write as encoding
 * pb 2008/04/30 new Formula API
 * pb 2009/01/18 Interpreter argument to formula
 * pb 2011/03/20 C++
 */

#include <ctype.h>
#include "TableOfReal.h"
#include "NUM2.h"
#include "Matrix.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "TableOfReal_def.h"
#include "oo_COPY.h"
#include "TableOfReal_def.h"
#include "oo_EQUAL.h"
#include "TableOfReal_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "TableOfReal_def.h"
#include "oo_WRITE_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_READ_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_DESCRIPTION.h"
#include "TableOfReal_def.h"

static void fprintquotedstring (MelderFile file, const wchar_t *s) {
	MelderFile_writeCharacter (file, '\"');
	if (s) { wchar_t c; while ((c = *s ++) != '\0') { MelderFile_writeCharacter (file, c); if (c == '\"') MelderFile_writeCharacter (file, c); } }
	MelderFile_writeCharacter (file, '\"');
}

static int writeText (I, MelderFile file) {
	iam (TableOfReal);
	texputi4 (file, my numberOfColumns, L"numberOfColumns", 0,0,0,0,0);
	MelderFile_write1 (file, L"\ncolumnLabels []: ");
	if (my numberOfColumns < 1) MelderFile_write1 (file, L"(empty)");
	MelderFile_write1 (file, L"\n");
	for (long i = 1; i <= my numberOfColumns; i ++) {
		fprintquotedstring (file, my columnLabels [i]);
		MelderFile_writeCharacter (file, '\t');
	}
	texputi4 (file, my numberOfRows, L"numberOfRows", 0,0,0,0,0);
	for (long i = 1; i <= my numberOfRows; i ++) {
		MelderFile_write3 (file, L"\nrow [", Melder_integer (i), L"]: ");
		fprintquotedstring (file, my rowLabels [i]);
		for (long j = 1; j <= my numberOfColumns; j ++) {
			double x = my data [i] [j];
			MelderFile_write2 (file, L"\t", Melder_double (x));
		}
	}
	return 1;
}

static int readText (I, MelderReadText text) {
	iam (TableOfReal);
	my numberOfColumns = texgeti4 (text);
	if (my numberOfColumns >= 1) {
		if (! (my columnLabels = (wchar **) NUMvector (sizeof (wchar_t *), 1, my numberOfColumns))) return 0;
		for (long i = 1; i <= my numberOfColumns; i ++)
			if (! (my columnLabels [i] = texgetw2 (text))) return 0;
	}
	my numberOfRows = texgeti4 (text);
	if (my numberOfRows >= 1) {
		if (! (my rowLabels = (wchar **) NUMvector (sizeof (wchar_t *), 1, my numberOfRows))) return 0;
	}
	if (my numberOfRows >= 1 && my numberOfColumns >= 1) {
		if (! (my data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns))) return 0;
		for (long i = 1; i <= my numberOfRows; i ++) {
			if (! (my rowLabels [i] = texgetw2 (text))) return 0;
			for (long j = 1; j <= my numberOfColumns; j ++)
				my data [i] [j] = texgetr8 (text);
		}
	}
	return 1;
}

static void info (I) {
	iam (TableOfReal);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of rows: ", Melder_integer (my numberOfRows));
	MelderInfo_writeLine2 (L"Number of columns: ", Melder_integer (my numberOfColumns));
}

static double getNrow (I) { iam (TableOfReal); return my numberOfRows; }
static double getNcol (I) { iam (TableOfReal); return my numberOfColumns; }
static const wchar * getRowStr (I, long irow) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows) return NULL;
	return my rowLabels [irow] ? my rowLabels [irow] : L"";
}
static const wchar * getColStr (I, long icol) {
	iam (TableOfReal);
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	return my columnLabels [icol] ? my columnLabels [icol] : L"";
}
static double getMatrix (I, long irow, long icol) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	return my data [irow] [icol];
}
static double getRowIndex (I, const wchar_t *rowLabel) {
	iam (TableOfReal);
	return TableOfReal_rowLabelToIndex (me, rowLabel);
}
static double getColumnIndex (I, const wchar_t *columnLabel) {
	iam (TableOfReal);
	return TableOfReal_columnLabelToIndex (me, columnLabel);
}

class_methods (TableOfReal, Data) {
	class_method_local (TableOfReal, destroy)
	class_method_local (TableOfReal, description)
	class_method_local (TableOfReal, copy)
	class_method_local (TableOfReal, equal)
	class_method_local (TableOfReal, canWriteAsEncoding)
	class_method (writeText)
	class_method (readText)
	class_method_local (TableOfReal, writeBinary)
	class_method_local (TableOfReal, readBinary)
	class_method (info)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getRowStr)
	class_method (getColStr)
	class_method (getMatrix)
	class_method (getRowIndex)
	class_method (getColumnIndex)
	class_methods_end
}

int TableOfReal_init (I, long numberOfRows, long numberOfColumns) {
	iam (TableOfReal);
	if (numberOfRows < 1 || numberOfColumns < 1)
		return Melder_error1 (L"(TableOfReal_init:) Cannot create cell-less table.");
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! (my rowLabels = (wchar **) NUMvector (sizeof (wchar_t *), 1, numberOfRows))) return 0;
	if (! (my columnLabels = (wchar **) NUMvector (sizeof (wchar_t *), 1, numberOfColumns))) return 0;
	if (! (my data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns))) return 0;
	return 1;
}

TableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns) {
	TableOfReal me = Thing_new (TableOfReal);
	if (! me || ! TableOfReal_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

/***** QUERY *****/

long TableOfReal_rowLabelToIndex (I, const wchar_t *label) {
	iam (TableOfReal);
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		if (my rowLabels [irow] && wcsequ (my rowLabels [irow], label))
			return irow;
	return 0;
}

long TableOfReal_columnLabelToIndex (I, const wchar_t *label) {
	iam (TableOfReal);
	for (long icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnLabels [icol] && wcsequ (my columnLabels [icol], label))
			return icol;
	return 0;
}

double TableOfReal_getColumnMean (I, long icol) {
	iam (TableOfReal);
	double sum = 0.0;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 1) return NUMundefined;
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		sum += my data [irow] [icol];
	return sum / my numberOfRows;
}

double TableOfReal_getColumnStdev (I, long icol) {
	iam (TableOfReal);
	double mean = TableOfReal_getColumnMean (me, icol), sum = 0.0, d;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 2) return NUMundefined;
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		sum += ( d = my data [irow] [icol] - mean, d * d );
	return sqrt (sum / (my numberOfRows - 1));
}

/***** MODIFY *****/

int TableOfReal_removeRow (I, long rowNumber) {
	iam (TableOfReal);
	try {
		if (my numberOfRows == 1)
			Melder_throw (Thing_messageName (me), " has only one row, and a TableOfReal without rows cannot exist.");
		if (rowNumber < 1 || rowNumber > my numberOfRows)
			Melder_throw ("No row ", rowNumber, ".");
		autoNUMdmatrix data (1, my numberOfRows - 1, 1, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			for (long irow = 1; irow < rowNumber; irow ++)
				data [irow] [icol] = my data [irow] [icol];
			for (long irow = rowNumber; irow < my numberOfRows; irow ++)
				data [irow] [icol] = my data [irow + 1] [icol];
		}
		/*
		 * Change without error.
		 */
		Melder_free (my rowLabels [rowNumber]);
		for (long irow = rowNumber; irow < my numberOfRows; irow ++)
			my rowLabels [irow] = my rowLabels [irow + 1];
		NUMdmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfRows --;
		return 1;
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": row ", rowNumber, " not removed.");
	}
}

int TableOfReal_insertRow (I, long rowNumber) {
	iam (TableOfReal);
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows + 1)
			Melder_throw ("Cannot create row ", rowNumber, ".");
		autoNUMdmatrix data (1, my numberOfRows + 1, 1, my numberOfColumns);
		autoNUMwvector rowLabels (1, my numberOfRows + 1);
		for (long irow = 1; irow < rowNumber; irow ++)	{
			rowLabels [irow] = my rowLabels [irow];
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				data [irow] [icol] = my data [irow] [icol];
		}
		for (long irow = my numberOfRows + 1; irow > rowNumber; irow --) {
			rowLabels [irow] = my rowLabels [irow - 1];
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				data [irow] [icol] = my data [irow - 1] [icol];
		}
		/*
		 * Change without error.
		 */
		NUMpvector_free (my rowLabels, 1);
		my rowLabels = rowLabels.transfer();
		NUMdmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfRows ++;
		return 1;
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": row at position ", rowNumber, " not inserted.");
	}
}

int TableOfReal_removeColumn (I, long columnNumber) {
	iam (TableOfReal);
	try {
		if (my numberOfColumns == 1)
			Melder_throw ("Cannot remove the only column.");
		if (columnNumber < 1 || columnNumber > my numberOfColumns)
			Melder_throw ("No column ", columnNumber, ".");
		autoNUMdmatrix data (1, my numberOfRows, 1, my numberOfColumns - 1);
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol < columnNumber; icol ++)
				data [irow] [icol] = my data [irow] [icol];
			for (long icol = columnNumber; icol < my numberOfColumns; icol ++)
				data [irow] [icol] = my data [irow] [icol + 1];
		}
		/*
		 * Change without error.
		 */
		Melder_free (my columnLabels [columnNumber]);
		for (long icol = columnNumber; icol < my numberOfColumns; icol ++)
			my columnLabels [icol] = my columnLabels [icol + 1];
		NUMdmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfColumns --;
		return 1;
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": column at position ", columnNumber, " not inserted.");
	}
}

int TableOfReal_insertColumn (I, long columnNumber) {
	iam (TableOfReal);
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns + 1)
			Melder_throw ("Cannot create column ", columnNumber, ".");
		autoNUMdmatrix data (1, my numberOfRows, 1, my numberOfColumns + 1);
		autoNUMwvector columnLabels (1, my numberOfColumns + 1);
		for (long j = 1; j < columnNumber; j ++) {
			columnLabels [j] = my columnLabels [j];
			for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j];
		}
		for (long j = my numberOfColumns + 1; j > columnNumber; j --) {
			columnLabels [j] = my columnLabels [j - 1];
			for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j - 1];
		}
		/*
		 * Change without error.
		 */
		NUMpvector_free (my columnLabels, 1);
		my columnLabels = columnLabels.transfer();
		NUMdmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfColumns ++;
		return 1;
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": column at position ", columnNumber, " not inserted.");
	}
}

void TableOfReal_setRowLabel (I, long rowNumber, const wchar_t *label) {
	iam (TableOfReal);
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows) return;
		autostring newLabel = Melder_wcsdup_e (label);
		/*
		 * Change without error.
		 */
		Melder_free (my rowLabels [rowNumber]);
		my rowLabels [rowNumber] = newLabel.transfer();
	} catch (...) {
		rethrowm (Thing_messageName (me), ": label of row ", rowNumber, " not set.");
	}
}

void TableOfReal_setColumnLabel (I, long columnNumber, const wchar_t *label) {
	iam (TableOfReal);
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns) return;
		autostring newLabel = Melder_wcsdup_e (label);
		/*
		 * Change without error.
		 */
		Melder_free (my columnLabels [columnNumber]);
		my columnLabels [columnNumber] = newLabel.transfer();
	} catch (...) {
		rethrowm (Thing_messageName (me), ": label of column ", columnNumber, " not set.");
	}
}

int TableOfReal_formula (I, const wchar_t *expression, Interpreter interpreter, thou) {
	iam (TableOfReal);
	thouart (TableOfReal);
	Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	if (thee == NULL) thee = me;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			thy data [irow] [icol] = result. result.numericResult;
		}
	}
end:
	iferror return 0;
	return 1;
}

/***** EXTRACT PART *****/

static void copyRowLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		thy rowLabels [irow] = Melder_wcsdup_e (my rowLabels [irow]); iferror return;
	}
}

static void copyColumnLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnLabels [icol] = Melder_wcsdup_e (my columnLabels [icol]); iferror return;
	}
}

static void copyRow (TableOfReal me, long myRow, TableOfReal thee, long thyRow) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	thy rowLabels [thyRow] = Melder_wcsdup_e (my rowLabels [myRow]); iferror return;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy data [thyRow] [icol] = my data [myRow] [icol];
	}
}

static void copyColumn (TableOfReal me, long myCol, TableOfReal thee, long thyCol) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	thy columnLabels [thyCol] = Melder_wcsdup_e (my columnLabels [myCol]); iferror return;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		thy data [irow] [thyCol] = my data [irow] [myCol];
	}
}

TableOfReal TableOfReal_extractRowsWhereColumn (I, long column, int which_Melder_NUMBER, double criterion) {
	iam (TableOfReal);
	try {
		if (column < 1 || column > my numberOfColumns)
			Melder_throw ("No such column: ", column, ".");
		long n = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			if (Melder_numberMatchesCriterion (my data [irow] [column], which_Melder_NUMBER, criterion)) {
				n ++;
			}
		}
		if (n == 0) Melder_throw ("No row matches this criterion.");
		autoTableOfReal thee = TableOfReal_create (n, my numberOfColumns);
		copyColumnLabels (me, thee.peek()); therror
		n = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			if (Melder_numberMatchesCriterion (my data [irow] [column], which_Melder_NUMBER, criterion)) {
				copyRow (me, irow, thee.peek(), ++ n); therror
			}
		}
		return thee.transfer();
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": rows not extracted.");
	}
}

TableOfReal TableOfReal_extractRowsWhereLabel (I, int which_Melder_STRING, const wchar_t *criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long n = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		if (Melder_stringMatchesCriterion (my rowLabels [irow], which_Melder_STRING, criterion)) {
			n ++;
		}
	}
	if (n == 0) error1 (L"No row matches this criterion.")
	thee = TableOfReal_create (n, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	n = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		if (Melder_stringMatchesCriterion (my rowLabels [irow], which_Melder_STRING, criterion)) {
			copyRow (me, irow, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractColumnsWhereRow (I, long row, int which_Melder_NUMBER, double criterion) {
	iam (TableOfReal);
	try {
		if (row < 1 || row > my numberOfRows)
			Melder_throw ("No such row: ", row, ".");
		long n = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (Melder_numberMatchesCriterion (my data [row] [icol], which_Melder_NUMBER, criterion)) {
				n ++;
			}
		}
		if (n == 0) Melder_throw ("No column matches this criterion.");

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, n);
		copyRowLabels (me, thee.peek()); therror
		n = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (Melder_numberMatchesCriterion (my data [row] [icol], which_Melder_NUMBER, criterion)) {
				copyColumn (me, icol, thee.peek(), ++ n); therror
			}
		}
		return thee.transfer();
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": columns not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnsWhereLabel (I, int which_Melder_STRING, const wchar_t *criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long n = 0;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (Melder_stringMatchesCriterion (my columnLabels [icol], which_Melder_STRING, criterion)) {
			n ++;
		}
	}
	if (n == 0) error1 (L"No column matches this criterion.")

	thee = TableOfReal_create (my numberOfRows, n);
	copyRowLabels (me, thee); cherror
	n = 0;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (Melder_stringMatchesCriterion (my columnLabels [icol], which_Melder_STRING, criterion)) {
			copyColumn (me, icol, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static long *getElementsOfRanges (const wchar_t *ranges, long maximumElement, long *numberOfElements, const wchar_t *elementType) {
	try {
		/*
		 * Count the elements.
		 */
		long previousElement = 0;
		*numberOfElements = 0;
		const wchar *p = & ranges [0];
		for (;;) {
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0') break;
			if (isdigit (*p)) {
				long currentElement = wcstol (p, NULL, 10);
				if (currentElement == 0)
					Melder_throw ("No such ", elementType, L": 0 (minimum is 1).");
				if (currentElement > maximumElement)
					Melder_throw ("No such ", elementType, ": ", currentElement, " (maximum is ", maximumElement, ").");
				*numberOfElements += 1;
				previousElement = currentElement;
				do { p ++; } while (isdigit (*p));
			} else if (*p == ':') {
				if (previousElement == 0)
					Melder_throw ("Cannot start range with colon.");
				do { p ++; } while (*p == ' ' || *p == '\t');
				if (*p == '\0')
					Melder_throw ("Cannot end range with colon.");
				if (! isdigit (*p))
					Melder_throw ("End of range should be a positive whole number.");
				long currentElement = wcstol (p, NULL, 10);
				if (currentElement == 0)
					Melder_throw ("No such ", elementType, ": 0 (minimum is 1).");
				if (currentElement > maximumElement)
					Melder_throw ("No such ", elementType, ": ", currentElement, " (maximum is ", maximumElement, ").");
				if (currentElement > previousElement) {
					*numberOfElements += currentElement - previousElement;
				} else {
					*numberOfElements += previousElement - currentElement;
				}
				previousElement = currentElement;
				do { p ++; } while (isdigit (*p));
			} else {
				Melder_throw ("Start of range should be a positive whole number.");
			}
		}
		/*
		 * Create room for the elements.
		 */
		autoNUMlvector elements (1, *numberOfElements);
		/*
		 * Store the elements.
		 */
		previousElement = 0;
		*numberOfElements = 0;
		p = & ranges [0];
		for (;;) {
			while (*p == ' ' || *p == '\t') p ++;
			if (*p == '\0') break;
			if (isdigit (*p)) {
				long currentElement = wcstol (p, NULL, 10);
				elements [++ *numberOfElements] = currentElement;
				previousElement = currentElement;
				do { p ++; } while (isdigit (*p));
			} else if (*p == ':') {
				do { p ++; } while (*p == ' ' || *p == '\t');
				long currentElement = wcstol (p, NULL, 10);
				if (currentElement > previousElement) {
					for (long ielement = previousElement + 1; ielement <= currentElement; ielement ++) {
						elements [++ *numberOfElements] = ielement;
					}
				} else {
					for (long ielement = previousElement - 1; ielement >= currentElement; ielement --) {
						elements [++ *numberOfElements] = ielement;
					}
				}
				previousElement = currentElement;
				do { p ++; } while (isdigit (*p));
			}
		}
		return elements.transfer();
	} catch (...) {
		rethrowzero;
	}
}

TableOfReal TableOfReal_extractRowRanges (I, const wchar_t *ranges) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long *elements = NULL, numberOfElements, ielement;
	elements = getElementsOfRanges (ranges, my numberOfRows, & numberOfElements, L"row"); cherror
	thee = TableOfReal_create (numberOfElements, my numberOfColumns); cherror
	copyColumnLabels (me, thee);
	for (ielement = 1; ielement <= numberOfElements; ielement ++) {
		copyRow (me, elements [ielement], thee, ielement); cherror
	}
end:
	iferror forget (thee);
	NUMlvector_free (elements, 1);
	return thee;
}

TableOfReal TableOfReal_extractColumnRanges (I, const wchar_t *ranges) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long *elements = NULL, numberOfElements, ielement;
	elements = getElementsOfRanges (ranges, my numberOfColumns, & numberOfElements, L"column"); cherror
	thee = TableOfReal_create (my numberOfRows, numberOfElements); cherror
	copyRowLabels (me, thee);
	for (ielement = 1; ielement <= numberOfElements; ielement ++) {
		copyColumn (me, elements [ielement], thee, ielement); cherror
	}
end:
	iferror forget (thee);
	NUMlvector_free (elements, 1);
	return thee;
}

TableOfReal TableOfReal_extractRowsWhere (I, const wchar_t *condition, Interpreter interpreter) {
	iam (TableOfReal);
	try {
		Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); therror
		/*
		 * Count the new number of rows.
		 */
		long numberOfElements = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result); therror
				if (result. result.numericResult != 0.0) {
					numberOfElements ++;
					break;
				}
			}
		}
		if (numberOfElements < 1) Melder_throw ("No rows match this condition.");

		/*
		 * Create room for the result.
		 */	
		autoTableOfReal thee = TableOfReal_create (numberOfElements, my numberOfColumns);
		copyColumnLabels (me, thee.peek()); therror
		/*
		 * Store the result.
		 */
		numberOfElements = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult != 0.0) {
					copyRow (me, irow, thee.peek(), ++ numberOfElements); therror
					break;
				}
			}
		}
		return thee.transfer();
	} catch (...) {
		rethrowmzero (Thing_messageName (me), ": rows not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnsWhere (I, const wchar_t *condition, Interpreter interpreter) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, icol, numberOfElements;
	Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	/*
	 * Count the new number of columns.
	 */
	numberOfElements = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			if (result. result.numericResult != 0.0) {
				numberOfElements ++;
				break;
			}
		}
	}
	if (numberOfElements < 1) error1 (L"No columns match this condition.")

	/*
	 * Create room for the result.
	 */	
	thee = TableOfReal_create (my numberOfRows, numberOfElements); cherror
	copyRowLabels (me, thee); cherror
	/*
	 * Store the result.
	 */
	numberOfElements = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result);
			if (result. result.numericResult != 0.0) {
				copyColumn (me, icol, thee, ++ numberOfElements); cherror
				break;
			}
		}
	}
end:
	iferror forget (thee);
	return thee;
}

/***** EXTRACT *****/

Strings TableOfReal_extractRowLabelsAsStrings (I) {
	iam (TableOfReal);
	long irow;
	Strings thee = Thing_new (Strings); cherror
	thy strings = NUMwvector (1, my numberOfRows); cherror
	thy numberOfStrings = my numberOfRows;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		thy strings [irow] = Melder_wcsdup_e (my rowLabels [irow] ? my rowLabels [irow] : L""); cherror
	}
end:
	iferror forget (thee);
	return thee;	
}

Strings TableOfReal_extractColumnLabelsAsStrings (I) {
	iam (TableOfReal);
	long icol;
	Strings thee = Thing_new (Strings); cherror
	thy strings = NUMwvector (1, my numberOfColumns); cherror
	thy numberOfStrings = my numberOfColumns;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy strings [icol] = Melder_wcsdup_e (my columnLabels [icol] ? my columnLabels [icol] : L""); cherror
	}
end:
	iferror forget (thee);
	return thee;	
}

/***** DRAW *****/

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

static void fixRows (TableOfReal me, long *rowmin, long *rowmax) {
	if (*rowmax < *rowmin) { *rowmin = 1; *rowmax = my numberOfRows; }
	else if (*rowmin < 1) *rowmin = 1;
	else if (*rowmax > my numberOfRows) *rowmax = my numberOfRows;
}
static void fixColumns (TableOfReal me, long *colmin, long *colmax) {
	if (*colmax < *colmin) { *colmin = 1; *colmax = my numberOfColumns; }
	else if (*colmin < 1) *colmin = 1;
	else if (*colmax > my numberOfColumns) *colmax = my numberOfColumns;
}
static double getMaxRowLabelWidth (TableOfReal me, Graphics g, long rowmin, long rowmax) {
	double maxWidth = 0.0;
	long row;
	if (! my rowLabels) return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (row = rowmin; row <= rowmax; row ++) if (my rowLabels [row] && my rowLabels [row] [0]) {
		double textWidth = Graphics_textWidth_ps (g, my rowLabels [row], TRUE);   /* SILIPA is bigger than XIPA */
		if (textWidth > maxWidth) maxWidth = textWidth;
	}
	return maxWidth;
}
static double getLeftMargin (Graphics g) {
	return Graphics_dxMMtoWC (g, 1);
}
static double getLineSpacing (Graphics g) {
	return Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72);
}
static double getMaxColumnLabelHeight (TableOfReal me, Graphics g, long colmin, long colmax) {
	double maxHeight = 0.0, lineSpacing = getLineSpacing (g);
	long col;
	if (! my columnLabels) return 0.0;
	fixRows (me, & colmin, & colmax);
	for (col = colmin; col <= colmax; col ++) if (my columnLabels [col] && my columnLabels [col] [0]) {
		if (! maxHeight) maxHeight = lineSpacing;
	}
	return maxHeight;
}

void TableOfReal_drawAsNumbers (I, Graphics g, long rowmin, long rowmax, int iformat, int precision) {
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long row, col;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfColumns + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (col = 1; col <= my numberOfColumns; col ++) {
		if (my columnLabels && my columnLabels [col] && my columnLabels [col] [0])
			Graphics_text (g, col, 1, my columnLabels [col]);
	}
	for (row = rowmin; row <= rowmax; row ++) {
		double y = 1 - lineSpacing * (row - rowmin + 0.6);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [row] && my rowLabels [row] [0])
			Graphics_text (g, 0.5 - leftMargin, y, my rowLabels [row]);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (col = 1; col <= my numberOfColumns; col ++) {
			wchar_t text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, text);
		}
	}
	if (maxTextHeight) {
		double left = 0.5;
		if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (g, left, 1, my numberOfColumns + 0.5, 1);
	}
	Graphics_unsetInner (g);
}

void TableOfReal_drawAsNumbers_if (I, Graphics g, long rowmin, long rowmax, int iformat, int precision,
	const wchar_t *conditionFormula, Interpreter interpreter)
{
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long row, col;
	Matrix original = TableOfReal_to_Matrix (me), conditions = static_cast<Matrix> (Data_copy (original));
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfColumns + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);
	if (! Matrix_formula (original, conditionFormula, interpreter, conditions))
		{ forget (original); forget (conditions); Melder_flushError ("Numbers not drawn."); return; }

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (col = 1; col <= my numberOfColumns; col ++) {
		if (my columnLabels && my columnLabels [col] && my columnLabels [col] [0])
			Graphics_text (g, col, 1, my columnLabels [col]);
	}
	for (row = rowmin; row <= rowmax; row ++) {
		double y = 1 - lineSpacing * (row - rowmin + 0.6);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [row] && my rowLabels [row] [0])
			Graphics_text (g, 0.5 - leftMargin, y, my rowLabels [row]);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (col = 1; col <= my numberOfColumns; col ++) if (conditions -> z [row] [col] != 0.0) {
			wchar_t text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, text);
		}
	}
	if (maxTextHeight) {
		double left = 0.5;
		if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (g, left, 1, my numberOfColumns + 0.5, 1);
	}
	Graphics_unsetInner (g);
	forget (original);
	forget (conditions);
}

void TableOfReal_drawVerticalLines (I, Graphics g, long rowmin, long rowmax) {
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long colmin = 1, colmax = my numberOfColumns, col;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	if (maxTextWidth > 0.0) colmin -= 1;
	for (col = colmin + 1; col <= colmax; col ++)
		Graphics_line (g, col - 0.5, 1 + maxTextHeight, col - 0.5, 1 - lineSpacing * (rowmax - rowmin + 1));
	Graphics_unsetInner (g);
}

void TableOfReal_drawLeftAndRightLines (I, Graphics g, long rowmin, long rowmax) {
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, right, top, bottom;
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	right = colmax + 0.5;
	top = 1 + maxTextHeight;
	bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (g, left, top, left, bottom);
	Graphics_line (g, right, top, right, bottom);
	Graphics_unsetInner (g);
}

void TableOfReal_drawHorizontalLines (I, Graphics g, long rowmin, long rowmax) {
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, top, right;
	long colmin = 1, colmax = my numberOfColumns, row;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	top = rowmin;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	if (maxTextHeight > 0.0) rowmin -= 1;
	right = colmax + 0.5;
	for (row = rowmin; row < rowmax; row ++) {
		double y = 1 - lineSpacing * (row - top + 1);
		Graphics_line (g, left, y, right, y);
	}
	Graphics_unsetInner (g);
}

void TableOfReal_drawTopAndBottomLines (I, Graphics g, long rowmin, long rowmax) {
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, top, right, bottom;
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	right = colmax + 0.5;
	top = 1 + maxTextHeight;
	bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (g, left, top, right, top);
	Graphics_line (g, left, bottom, right, bottom);
	Graphics_unsetInner (g);
}

void TableOfReal_drawAsSquares (I, Graphics g, long rowmin, long rowmax,
	long colmin, long colmax, int garnish)
{
	iam (TableOfReal);
	double datamax, dx = 1, dy = 1;
	long i, j;
	Graphics_Colour colour = Graphics_inqColour (g);
	fixRows (me, & rowmin, & rowmax);
	fixColumns (me, & colmin, & colmax);
	
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5);
	datamax = my data [rowmin] [colmin];
	for (i = 1; i <= my numberOfRows; i ++) for (j = 1; j <= my numberOfColumns; j ++)
		if (fabs (my data [i] [j]) > datamax) datamax = fabs (my data [i] [j]);
	
	for (i = rowmin; i <= rowmax; i ++) {
		double y = rowmax + rowmin - i;
		for (j = colmin; j <= colmax; j ++) {
			double x = j;
			/* two neighbouring squares should not touch -> 0.95 */
			double d = 0.95 * sqrt (fabs (my data[i][j]) / datamax);
			double x1WC = x - d * dx / 2, x2WC = x + d * dx / 2;
			double y1WC = y - d * dy / 2, y2WC = y + d * dy / 2;
			if (my data [i] [j] > 0) Graphics_setColour (g, Graphics_WHITE);
			Graphics_fillRectangle (g, x1WC, x2WC, y1WC, y2WC);
			Graphics_setColour (g, colour);
			Graphics_rectangle (g, x1WC, x2WC , y1WC, y2WC);
		}
	}
	Graphics_setGrey (g, 0.0);
	Graphics_unsetInner (g);
	if (garnish) {
		for (i = rowmin; i <= rowmax; i ++) if (my rowLabels [i]) 
			Graphics_markLeft (g, rowmax + rowmin - i, 0, 0, 0, my rowLabels [i]);
		for (j = colmin; j <= colmax; j ++) if (my columnLabels [j])
			Graphics_markTop (g, j, 0, 0, 0, my columnLabels [j]);
	}
}

Any TablesOfReal_append (I, thou) {
	iam (TableOfReal); thouart (TableOfReal);
	try {
		if (thy numberOfColumns != my numberOfColumns)
			Melder_throw (L"Numbers of columns are ", my numberOfColumns, " and ", thy numberOfColumns, " but should be equal.");
		autoTableOfReal him = static_cast<TableOfReal> (_Thing_new (my methods));
		TableOfReal_init (him.peek(), my numberOfRows + thy numberOfRows, my numberOfColumns); therror
		/* Unsafe: new attributes not initialized. */
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (him.peek(), icol, my columnLabels [icol]); therror
		}
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			TableOfReal_setRowLabel (him.peek(), irow, my rowLabels [irow]); therror
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				his data [irow] [icol] = my data [irow] [icol];
		}
		for (long irow = 1; irow <= thy numberOfRows; irow ++) {
			long hisRow = irow + my numberOfRows;
			TableOfReal_setRowLabel (him.peek(), hisRow, thy rowLabels [irow]); therror
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				his data [hisRow] [icol] = thy data [irow] [icol];
		}
		return him.transfer();
	} catch (...) {
		rethrowmzero ("TableOfReal objects not appended.");
	}
}

Any TablesOfReal_appendMany (Collection me) {
	TableOfReal him = NULL, thee;
	long itab, irow, icol, nrow, ncol;
	if (my size == 0) return Melder_errorp1 (L"Cannot add zero tables.");
	thee = static_cast<TableOfReal> (my item [1]);
	nrow = thy numberOfRows;
	ncol = thy numberOfColumns;
	for (itab = 2; itab <= my size; itab ++) {
		thee = static_cast<TableOfReal> (my item [itab]);
		nrow += thy numberOfRows;
		if (thy numberOfColumns != ncol) error1 (L"Numbers of columns do not match.")
	}
	him = static_cast<TableOfReal> (_Thing_new (thy methods)); cherror
	TableOfReal_init (him, nrow, ncol); cherror
	/* Unsafe: new attributes not initialized. */
	for (icol = 1; icol <= ncol; icol ++) {
		TableOfReal_setColumnLabel (him, icol, thy columnLabels [icol]); cherror
	}
	nrow = 0;
	for (itab = 1; itab <= my size; itab ++) {
		thee = static_cast<TableOfReal> (my item [itab]);
		for (irow = 1; irow <= thy numberOfRows; irow ++) {
			nrow ++;
			TableOfReal_setRowLabel (him, nrow, thy rowLabels [irow]); cherror
			for (icol = 1; icol <= ncol; icol ++)
				his data [nrow] [icol] = thy data [irow] [icol];
		}
	}
	Melder_assert (nrow == his numberOfRows);
end:
	iferror { forget (him); Melder_error1 (L"(TablesOfReal_appendMany:) Not performed."); }
	return him;
}

static void TableOfReal_sort (TableOfReal me, int useLabels, long column1, long column2) {
	for (long irow = 1; irow < my numberOfRows; irow ++) for (long jrow = irow + 1; jrow <= my numberOfRows; jrow ++) {
		wchar_t *tmpString;
		if (useLabels) {
			if (my rowLabels [irow] != NULL) {
				if (my rowLabels [jrow] != NULL) {
					int compare = wcscmp (my rowLabels [irow], my rowLabels [jrow]);
					if (compare < 0) continue;
					if (compare > 0) goto swap;
				} else goto swap;
			} else if (my rowLabels [jrow] != NULL) continue;
		}
		/*
		 * If we arrive here, the two labels are equal or both NULL (or useLabels is FALSE).
		 */
		if (column1 > 0 && column1 <= my numberOfColumns) {
			if (my data [irow] [column1] < my data [jrow] [column1]) continue;
			if (my data [irow] [column1] > my data [jrow] [column1]) goto swap;
		}
		if (column2 > 0 && column2 <= my numberOfColumns) {
			if (my data [irow] [column2] < my data [jrow] [column2]) continue;
			if (my data [irow] [column2] > my data [jrow] [column2]) goto swap;
		}
		/*
		 * If we arrive here, everything is equal.
		 */
		continue;
	swap:
		tmpString = my rowLabels [irow];
		my rowLabels [irow] = my rowLabels [jrow];
		my rowLabels [jrow] = tmpString;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			double tmpValue = my data [irow] [icol];
			my data [irow] [icol] = my data [jrow] [icol];
			my data [jrow] [icol] = tmpValue;
		}
	}
}

void TableOfReal_sortByLabel (I, long column1, long column2) {
	iam (TableOfReal);
	TableOfReal_sort (me, TRUE, column1, column2);
}

void TableOfReal_sortByColumn (I, long column1, long column2) {
	iam (TableOfReal);
	TableOfReal_sort (me, FALSE, column1, column2);
}

TableOfReal Table_to_TableOfReal (Table me, long labelColumn) {
	long irow, icol;
	TableOfReal thee;
	if (labelColumn < 1 || labelColumn > my numberOfColumns) labelColumn = 0;
	thee = TableOfReal_create (my rows -> size, labelColumn ? my numberOfColumns - 1 : my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		Table_numericize_Assert (me, icol);
	}
	if (labelColumn) {
		for (icol = 1; icol < labelColumn; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol - 1, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast<TableRow> (my rows -> item [irow]);
			wchar_t *string = row -> cells [labelColumn]. string;
			TableOfReal_setRowLabel (thee, irow, string ? string : L"");
			for (icol = 1; icol < labelColumn; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol] = Table_getNumericValue_Assert (me, irow, icol);
			}
			for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol - 1] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol - 1] = Table_getNumericValue_Assert (me, irow, icol);
			}
		}
	} else {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast<TableRow> (my rows -> item [irow]);
			for (icol = 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol] = Table_getNumericValue_Assert (me, irow, icol);
			}
		}
	}
end:
	iferror return NULL;
	return thee;
}

Table TableOfReal_to_Table (TableOfReal me, const wchar_t *labelOfFirstColumn) {
	Table thee = NULL;
//start:
	thee = Table_createWithoutColumnNames (my numberOfRows, my numberOfColumns + 1); cherror
	Table_setColumnLabel (thee, 1, labelOfFirstColumn); cherror
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		wchar_t *columnLabel = my columnLabels [icol];
		thy columnHeaders [icol + 1]. label = Melder_wcsdup_e (columnLabel && columnLabel [0] ? columnLabel : L"?"); cherror
	}
	for (long irow = 1; irow <= thy rows -> size; irow ++) {
		wchar_t *stringValue = my rowLabels [irow];
		TableRow row = static_cast<TableRow> (thy rows -> item [irow]);
		row -> cells [1]. string = Melder_wcsdup_e (stringValue && stringValue [0] ? stringValue : L"?"); cherror
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			double numericValue = my data [irow] [icol];
			row -> cells [icol + 1]. string = Melder_wcsdup_e (Melder_double (numericValue)); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

int TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file) {
	MelderString buffer = { 0 };
	MelderString_copy (& buffer, L"rowLabel");
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		MelderString_appendCharacter (& buffer, '\t');
		wchar_t *s = my columnLabels [icol];
		MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
	}
	MelderString_appendCharacter (& buffer, '\n');
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		wchar_t *s = my rowLabels [irow];
		MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			MelderString_appendCharacter (& buffer, '\t');
			double x = my data [irow] [icol];
			MelderString_append (& buffer, Melder_double (x));
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	MelderFile_writeText (file, buffer.string);
end:
	MelderString_free (& buffer);
	iferror return 0;
	return 1;
}

TableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file) {
	try {
		autostring string = MelderFile_readText (file);
		long nrow, ncol, nelements;

		/*
		 * Count columns.
		 */
		ncol = 0;
		wchar_t *p = & string [0];
		for (;;) {
			wchar_t kar = *p++;
			if (kar == '\n' || kar == '\0') break;
			if (kar == ' ' || kar == '\t') continue;
			ncol ++;
			do { kar = *p++; } while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\0');
			if (kar == '\n' || kar == '\0') break;
		}
		ncol --;
		if (ncol < 1) Melder_throw ("No columns.");

		/*
		 * Count elements.
		 */
		p = & string [0];
		nelements = 0;
		for (;;) {
			wchar_t kar = *p++;
			if (kar == '\0') break;
			if (kar == ' ' || kar == '\t' || kar == '\n') continue;
			nelements ++;
			do { kar = *p++; } while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\0');
			if (kar == '\0') break;
		}

		/*
		 * Check if all columns are complete.
		 */
		if (nelements == 0 || nelements % (ncol + 1) != 0)
			Melder_throw ("The number of elements (", nelements, ") is not a multiple of the number of columns plus 1 (", ncol + 1, ").");

		/*
		 * Create empty table.
		 */
		nrow = nelements / (ncol + 1) - 1;
		autoTableOfReal me = TableOfReal_create (nrow, ncol);

		/*
		 * Read elements.
		 */
		p = & string [0];
		while (*p == ' ' || *p == '\t') { Melder_assert (*p != '\0'); p ++; }
		while (*p != ' ' && *p != '\t') { Melder_assert (*p != '\0'); p ++; }   // ignore the header of the zeroth column ("rowLabel" perhaps)
		for (long icol = 1; icol <= ncol; icol ++) {
			while (*p == ' ' || *p == '\t') { Melder_assert (*p != '\0'); p ++; }
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			while (*p != ' ' && *p != '\t' && *p != '\n') {
				MelderString_appendCharacter (& buffer, *p); therror
				p ++;
			}
			TableOfReal_setColumnLabel (me.peek(), icol, buffer.string); therror
			MelderString_empty (& buffer);
		}
		for (long irow = 1; irow <= nrow; irow ++) {
			while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			while (*p != ' ' && *p != '\t') {
				MelderString_appendCharacter (& buffer, *p); therror
				p ++;
			}
			TableOfReal_setRowLabel (me.peek(), irow, buffer.string);
			MelderString_empty (& buffer);
			for (long icol = 1; icol <= ncol; icol ++) {
				while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
				MelderString_empty (& buffer);
				while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') {
					MelderString_appendCharacter (& buffer, *p); therror
					p ++;
				}
				my data [irow] [icol] = Melder_atof (buffer.string);   /* If cell contains a string, this will be 0. */
				MelderString_empty (& buffer);
			}
		}
		return me.transfer();
	} catch (...) {
		rethrowmzero ("TableOfReal: headerless spreadsheet file ", MelderFile_messageName (file), " not read.");
	}
}

/* End of file TableOfReal.cpp */
