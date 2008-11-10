/* TableOfReal.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
		if (! (my columnLabels = NUMvector (sizeof (wchar_t *), 1, my numberOfColumns))) return 0;
		for (long i = 1; i <= my numberOfColumns; i ++)
			if (! (my columnLabels [i] = texgetw2 (text))) return 0;
	}
	my numberOfRows = texgeti4 (text);
	if (my numberOfRows >= 1) {
		if (! (my rowLabels = NUMvector (sizeof (wchar_t *), 1, my numberOfRows))) return 0;
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
static wchar_t * getRowStr (I, long irow) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows) return NULL;
	return my rowLabels [irow] ? my rowLabels [irow] : L"";
}
static wchar_t * getColStr (I, long icol) {
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
	if (! (my rowLabels = NUMvector (sizeof (wchar_t *), 1, numberOfRows))) return 0;
	if (! (my columnLabels = NUMvector (sizeof (wchar_t *), 1, numberOfColumns))) return 0;
	if (! (my data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns))) return 0;
	return 1;
}

TableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns) {
	TableOfReal me = new (TableOfReal);
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

int TableOfReal_removeRow (I, long irow) {
	iam (TableOfReal);
	if (my numberOfRows == 1)
		error2 (Thing_messageName (me), L" has only one row, and a TableOfReal without rows cannot exist.")
	if (irow < 1 || irow > my numberOfRows) error3 (L"No row ", Melder_integer (irow), L".")
	double **data = NUMdmatrix (1, my numberOfRows - 1, 1, my numberOfColumns); cherror
	for (long i = 1; i <= my numberOfColumns; i ++) {
		for (long j = 1; j < irow; j ++) data [j] [i] = my data [j] [i];
		for (long j = irow; j < my numberOfRows; j ++) data [j] [i] = my data [j + 1] [i];
	}
	Melder_free (my rowLabels [irow]);
	for (long j = irow; j < my numberOfRows; j ++) my rowLabels [j] = my rowLabels [j + 1];
	NUMdmatrix_free (my data, 1, 1);
	my data = data;
	my numberOfRows --;
end:
	iferror return Melder_error5 (L"Row ", Melder_integer (irow), L" of ", Thing_messageName (me), L" not removed.");
	return 1;
}

int TableOfReal_insertRow (I, long irow) {
	iam (TableOfReal);
	double **data = NULL;
	wchar_t **rowLabels = NULL;
	if (irow < 1 || irow > my numberOfRows + 1) error3 (L"Cannot create row ", Melder_integer (irow), L".")
	data = NUMdmatrix (1, my numberOfRows + 1, 1, my numberOfColumns); cherror
	rowLabels = NUMpvector (1, my numberOfRows + 1);
	for (long j = 1; j < irow; j ++)	{
		rowLabels [j] = my rowLabels [j];
		for (long i = 1; i <= my numberOfColumns; i ++) data [j] [i] = my data [j] [i];
	}
	for (long j = my numberOfRows + 1; j > irow; j --) {
		rowLabels [j] = my rowLabels [j - 1];
		for (long i = 1; i <= my numberOfColumns; i ++) data [j] [i] = my data [j - 1] [i];
	}
	NUMdmatrix_free (my data, 1, 1);
	NUMpvector_free (my rowLabels, 1);
	my data = data;
	my rowLabels = rowLabels;
	my numberOfRows ++;
end:
	iferror return Melder_error1 (L"(TableOfReal_insertRow:) Not performed.");
	return 1;
}

int TableOfReal_removeColumn (I, long icol) {
	iam (TableOfReal);
	double **data = NULL;
	if (my numberOfColumns == 1) error1 (L"Cannot remove the only column.")
	if (icol < 1 || icol > my numberOfColumns) error3 (L"No column ", Melder_integer (icol), L".")
	data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns - 1); cherror
	for (long i = 1; i <= my numberOfRows; i ++) {
		for (long j = 1; j < icol; j ++) data [i] [j] = my data [i] [j];
		for (long j = icol; j < my numberOfColumns; j ++) data [i] [j] = my data [i] [j + 1];
	}
	Melder_free (my columnLabels [icol]);
	for (long j = icol; j < my numberOfColumns; j ++) my columnLabels [j] = my columnLabels [j + 1];
	NUMdmatrix_free (my data, 1, 1);
	my data = data;
	my numberOfColumns --;
end:
	iferror return Melder_error1 (L"(TableOfReal_removeColumn:) Not performed.");
	return 1;
}

int TableOfReal_insertColumn (I, long icol) {
	iam (TableOfReal);
	double **data = NULL;
	wchar_t **columnLabels = NULL;
	if (icol < 1 || icol > my numberOfColumns + 1) error3 (L"Cannot create column ", Melder_integer (icol), L".")
	data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns + 1); cherror
	columnLabels = NUMpvector (1, my numberOfColumns + 1); cherror
	for (long j = 1; j < icol; j ++) {
		columnLabels [j] = my columnLabels [j];
		for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j];
	}
	for (long j = my numberOfColumns + 1; j > icol; j --) {
		columnLabels [j] = my columnLabels [j - 1];
		for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j - 1];
	}
	NUMdmatrix_free (my data, 1, 1);
	NUMpvector_free (my columnLabels, 1);
	my data = data;
	my columnLabels = columnLabels;
	my numberOfColumns ++;
end:
	iferror return Melder_error1 (L"(TableOfReal_insertColumn:) Not performed.");
	return 1;
}

void TableOfReal_setRowLabel (I, long irow, const wchar_t *label) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows || label == my rowLabels [irow]) return;
	Melder_free (my rowLabels [irow]);
	my rowLabels [irow] = Melder_wcsdup (label);
}

void TableOfReal_setColumnLabel (I, long icol, const wchar_t *label) {
	iam (TableOfReal);
	if (icol < 1 || icol > my numberOfColumns || label == my columnLabels [icol]) return;
	Melder_free (my columnLabels [icol]);
	my columnLabels [icol] = Melder_wcsdup (label);
}

int TableOfReal_formula (I, const wchar_t *expression, thou) {
	iam (TableOfReal);
	thouart (TableOfReal);
	Formula_compile (NULL, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
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
		thy rowLabels [irow] = Melder_wcsdup (my rowLabels [irow]); iferror return;
	}
}

static void copyColumnLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnLabels [icol] = Melder_wcsdup (my columnLabels [icol]); iferror return;
	}
}

static void copyRow (TableOfReal me, long myRow, TableOfReal thee, long thyRow) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	thy rowLabels [thyRow] = Melder_wcsdup (my rowLabels [myRow]); iferror return;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy data [thyRow] [icol] = my data [myRow] [icol];
	}
}

static void copyColumn (TableOfReal me, long myCol, TableOfReal thee, long thyCol) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	thy columnLabels [thyCol] = Melder_wcsdup (my columnLabels [myCol]); iferror return;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		thy data [irow] [thyCol] = my data [irow] [myCol];
	}
}

TableOfReal TableOfReal_extractRowsWhereColumn (I, long column, int which_Melder_NUMBER, double criterion) {
	iam (TableOfReal);
	/*
	 * init
	 */
	TableOfReal thee = NULL;
	/*
	 * check input
	 */
	if (column < 1 || column > my numberOfColumns) error3 (L"No such column: ", Melder_integer (column), L".")

	long n = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		if (Melder_numberMatchesCriterion (my data [irow] [column], which_Melder_NUMBER, criterion)) {
			n ++;
		}
	}
	if (n == 0) error1 (L"No row matches this criterion.")
	thee = TableOfReal_create (n, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	n = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		if (Melder_numberMatchesCriterion (my data [irow] [column], which_Melder_NUMBER, criterion)) {
			copyRow (me, irow, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
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
	/*
	 * init
	 */
	TableOfReal thee = NULL;
	/*
	 * check input
	 */
	if (row < 1 || row > my numberOfRows) error3 (L"No such row: ", Melder_integer (row), L".")

	long n = 0;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (Melder_numberMatchesCriterion (my data [row] [icol], which_Melder_NUMBER, criterion)) {
			n ++;
		}
	}
	if (n == 0) error1 (L"No column matches this criterion.")

	thee = TableOfReal_create (my numberOfRows, n);
	copyRowLabels (me, thee); cherror
	n = 0;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (Melder_numberMatchesCriterion (my data [row] [icol], which_Melder_NUMBER, criterion)) {
			copyColumn (me, icol, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
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
	const wchar_t *p;
	long previousElement, currentElement, *elements, ielement;
	/*
	 * Count the elements.
	 */
	previousElement = 0;
	*numberOfElements = 0;
	p = & ranges [0];
	for (;;) {
		while (*p == ' ' || *p == '\t') p ++;
		if (*p == '\0') break;
		if (isdigit (*p)) {
			currentElement = wcstol (p, NULL, 10);
			if (currentElement == 0) {
				Melder_error3 (L"No such ", elementType, L": 0 (minimum is 1).");
				return NULL;
			}
			if (currentElement > maximumElement) {
				Melder_error7 (L"No such ", elementType, L": ", Melder_integer (currentElement), L" (maximum is ", Melder_integer (maximumElement), L").");
				return NULL;
			}
			*numberOfElements += 1;
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else if (*p == ':') {
			if (previousElement == 0) {
				return Melder_errorp ("Cannot start range with colon.");
			}
			do { p ++; } while (*p == ' ' || *p == '\t');
			if (*p == '\0') {
				return Melder_errorp ("Cannot end range with colon.");
			}
			if (! isdigit (*p)) {
				return Melder_errorp ("End of range should be a positive whole number.");
			}
			currentElement = wcstol (p, NULL, 10);
			if (currentElement == 0) {
				Melder_error3 (L"No such ", elementType, L": 0 (minimum is 1).");
				return NULL;
			}
			if (currentElement > maximumElement) {
				Melder_error7 (L"No such ", elementType, L": ", Melder_integer (currentElement), L" (maximum is ", Melder_integer (maximumElement), L").");
				return NULL;
			}
			if (currentElement > previousElement) {
				*numberOfElements += currentElement - previousElement;
			} else {
				*numberOfElements += previousElement - currentElement;
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else {
			return Melder_errorp ("Start of range should be a positive whole number.");
		}
	}
	/*
	 * Create room for the elements.
	 */
	elements = NUMlvector (1, *numberOfElements);
	if (elements == NULL) return NULL;
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
			currentElement = wcstol (p, NULL, 10);
			elements [++ *numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else if (*p == ':') {
			do { p ++; } while (*p == ' ' || *p == '\t');
			currentElement = wcstol (p, NULL, 10);
			if (currentElement > previousElement) {
				for (ielement = previousElement + 1; ielement <= currentElement; ielement ++) {
					elements [++ *numberOfElements] = ielement;
				}
			} else {
				for (ielement = previousElement - 1; ielement >= currentElement; ielement --) {
					elements [++ *numberOfElements] = ielement;
				}
			}
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		}
	}
	return elements;
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

TableOfReal TableOfReal_extractRowsWhere (I, const wchar_t *condition) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	Formula_compile (NULL, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	/*
	 * Count the new number of rows.
	 */
	long numberOfElements = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			if (result. result.numericResult != 0.0) {
				numberOfElements ++;
				break;
			}
		}
	}
	if (numberOfElements < 1) error1 (L"No rows match this condition.")

	/*
	 * Create room for the result.
	 */	
	thee = TableOfReal_create (numberOfElements, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	/*
	 * Store the result.
	 */
	numberOfElements = 0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result);
			if (result. result.numericResult != 0.0) {
				copyRow (me, irow, thee, ++ numberOfElements); cherror
				break;
			}
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractColumnsWhere (I, const wchar_t *condition) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, icol, numberOfElements;
	Formula_compile (NULL, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
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
	Strings thee = new (Strings); cherror
	thy strings = NUMpvector (1, my numberOfRows); cherror
	thy numberOfStrings = my numberOfRows;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		thy strings [irow] = Melder_wcsdup (my rowLabels [irow] ? my rowLabels [irow] : L""); cherror
	}
end:
	iferror forget (thee);
	return thee;	
}

Strings TableOfReal_extractColumnLabelsAsStrings (I) {
	iam (TableOfReal);
	long icol;
	Strings thee = new (Strings); cherror
	thy strings = NUMpvector (1, my numberOfColumns); cherror
	thy numberOfStrings = my numberOfColumns;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy strings [icol] = Melder_wcsdup (my columnLabels [icol] ? my columnLabels [icol] : L""); cherror
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
	const wchar_t *conditionFormula)
{
	iam (TableOfReal);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long row, col;
	Matrix original = TableOfReal_to_Matrix (me), conditions = Data_copy (original);
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfColumns + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);
	if (! Matrix_formula (original, conditionFormula, conditions))
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
	int colour = Graphics_inqColour (g);
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
	/*
	 * init
	 */
	TableOfReal him = NULL;
	/*
	 * check input
	 */
	if (thy numberOfColumns != my numberOfColumns) error1 (L"Numbers of columns do not match.")

	him = Thing_new (my methods); cherror
	TableOfReal_init (him, my numberOfRows + thy numberOfRows, my numberOfColumns); cherror
	/* Unsafe: new attributes not initialized. */
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		TableOfReal_setColumnLabel (him, icol, my columnLabels [icol]); cherror
	}
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		TableOfReal_setRowLabel (him, irow, my rowLabels [irow]); cherror
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			his data [irow] [icol] = my data [irow] [icol];
	}
	for (long irow = 1; irow <= thy numberOfRows; irow ++) {
		long hisRow = irow + my numberOfRows;
		TableOfReal_setRowLabel (him, hisRow, thy rowLabels [irow]); cherror
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			his data [hisRow] [icol] = thy data [irow] [icol];
	}
end:
	iferror { forget (him); Melder_error1 (L"(TablesOfReal_append:) Not performed."); }
	return him;
}

Any TablesOfReal_appendMany (Collection me) {
	TableOfReal him = NULL, thee;
	long itab, irow, icol, nrow, ncol;
	if (my size == 0) return Melder_errorp1 (L"Cannot add zero tables.");
	thee = my item [1];
	nrow = thy numberOfRows;
	ncol = thy numberOfColumns;
	for (itab = 2; itab <= my size; itab ++) {
		thee = my item [itab];
		nrow += thy numberOfRows;
		if (thy numberOfColumns != ncol) error1 (L"Numbers of columns do not match.")
	}
	him = Thing_new (thy methods); cherror
	TableOfReal_init (him, nrow, ncol); cherror
	/* Unsafe: new attributes not initialized. */
	for (icol = 1; icol <= ncol; icol ++) {
		TableOfReal_setColumnLabel (him, icol, thy columnLabels [icol]); cherror
	}
	nrow = 0;
	for (itab = 1; itab <= my size; itab ++) {
		thee = my item [itab];
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
	long i, j;
	for (i = 1; i < my numberOfRows; i ++) for (j = i + 1; j <= my numberOfRows; j ++) {
		wchar_t *tmpString;
		long column;
		if (useLabels) {
			if (my rowLabels [i] != NULL) {
				if (my rowLabels [j] != NULL) {
					int compare = wcscmp (my rowLabels [i], my rowLabels [j]);
					if (compare < 0) continue;
					if (compare > 0) goto swap;
				} else goto swap;
			} else if (my rowLabels [j] != NULL) continue;
		}
		/*
		 * If we arrive here, the two labels are equal or both NULL (or useLabels is FALSE).
		 */
		if (column1 > 0 && column1 <= my numberOfColumns) {
			if (my data [i] [column1] < my data [j] [column1]) continue;
			if (my data [i] [column1] > my data [j] [column1]) goto swap;
		}
		if (column2 > 0 && column2 <= my numberOfColumns) {
			if (my data [i] [column2] < my data [j] [column2]) continue;
			if (my data [i] [column2] > my data [j] [column2]) goto swap;
		}
		/*
		 * If we arrive here, everything is equal.
		 */
		continue;
	swap:
		tmpString = my rowLabels [i];
		my rowLabels [i] = my rowLabels [j];
		my rowLabels [j] = tmpString;
		for (column = 1; column <= my numberOfColumns; column ++) {
			double tmpValue = my data [i] [column];
			my data [i] [column] = my data [j] [column];
			my data [j] [column] = tmpValue;
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
		Table_numericize (me, icol);
	}
	if (labelColumn) {
		for (icol = 1; icol < labelColumn; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol - 1, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			wchar_t *string = row -> cells [labelColumn]. string;
			TableOfReal_setRowLabel (thee, irow, string ? string : L"");
			for (icol = 1; icol < labelColumn; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol] = Table_getNumericValue (me, irow, icol);
			}
			for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol - 1] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol - 1] = Table_getNumericValue (me, irow, icol);
			}
		}
	} else {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			for (icol = 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
				//thy data [irow] [icol] = Table_getNumericValue (me, irow, icol);
			}
		}
	}
end:
	iferror return NULL;
	return thee;
}

Table TableOfReal_to_Table (TableOfReal me, const wchar_t *labelOfFirstColumn) {
	Table thee = Table_createWithoutColumnNames (my numberOfRows, my numberOfColumns + 1); cherror
	Table_setColumnLabel (thee, 1, labelOfFirstColumn); cherror
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		wchar_t *columnLabel = my columnLabels [icol];
		thy columnHeaders [icol + 1]. label = Melder_wcsdup (columnLabel && columnLabel [0] ? columnLabel : L"?"); cherror
	}
	for (long irow = 1; irow <= thy rows -> size; irow ++) {
		wchar_t *stringValue = my rowLabels [irow];
		TableRow row = thy rows -> item [irow];
		row -> cells [1]. string = Melder_wcsdup (stringValue && stringValue [0] ? stringValue : L"?"); cherror
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			double numericValue = my data [irow] [icol];
			row -> cells [icol + 1]. string = Melder_wcsdup (Melder_double (numericValue)); cherror
		}
	}
end:
	iferror return NULL;
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
	TableOfReal me = NULL;
	wchar_t *string = MelderFile_readText (file); cherror
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
	if (ncol < 1) error1 (L"No columns.")

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
		error5 (L"The number of elements (", Melder_integer (nelements), L") is not a multiple of the number of columns plus 1 (", Melder_integer (ncol + 1), L").")

	/*
	 * Create empty table.
	 */
	nrow = nelements / (ncol + 1) - 1;
	me = TableOfReal_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */
	p = & string [0];
	while (*p == ' ' || *p == '\t') { Melder_assert (*p != '\0'); p ++; }
	while (*p != ' ' && *p != '\t') { Melder_assert (*p != '\0'); p ++; }   // Ignore the header of the zeroth column ("rowLabel" perhaps).
	for (long icol = 1; icol <= ncol; icol ++) {
		while (*p == ' ' || *p == '\t') { Melder_assert (*p != '\0'); p ++; }
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		while (*p != ' ' && *p != '\t' && *p != '\n') { MelderString_appendCharacter (& buffer, *p); p ++; }
		TableOfReal_setColumnLabel (me, icol, buffer.string);
		MelderString_empty (& buffer);
	}
	for (long irow = 1; irow <= nrow; irow ++) {
		while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		while (*p != ' ' && *p != '\t') { MelderString_appendCharacter (& buffer, *p); p ++; }
		TableOfReal_setRowLabel (me, irow, buffer.string);
		MelderString_empty (& buffer);
		for (long icol = 1; icol <= ncol; icol ++) {
			while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
			MelderString_empty (& buffer);
			while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') { MelderString_appendCharacter (& buffer, *p); p ++; }
			my data [irow] [icol] = Melder_atof (buffer.string);   /* If cell contains a string, this will be 0. */
			MelderString_empty (& buffer);
		}
	}

end:
	iferror {
		forget (me);
		Melder_error3 (L"(TableOfReal_readFromHeaderlessSpreadsheetFile:) File ", MelderFile_messageName (file), L" not read.");
		return NULL;
	}
	return me;
}

/* End of file TableOfReal.c */
