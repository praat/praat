/* TableOfReal.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

void structTableOfReal :: v_writeText (MelderFile file) {
	texputi4 (file, numberOfColumns, L"numberOfColumns", 0,0,0,0,0);
	MelderFile_write (file, L"\ncolumnLabels []: ");
	if (numberOfColumns < 1) MelderFile_write (file, L"(empty)");
	MelderFile_write (file, L"\n");
	for (long i = 1; i <= numberOfColumns; i ++) {
		fprintquotedstring (file, columnLabels [i]);
		MelderFile_writeCharacter (file, '\t');
	}
	texputi4 (file, numberOfRows, L"numberOfRows", 0,0,0,0,0);
	for (long i = 1; i <= numberOfRows; i ++) {
		MelderFile_write (file, L"\nrow [", Melder_integer (i), L"]: ");
		fprintquotedstring (file, rowLabels [i]);
		for (long j = 1; j <= numberOfColumns; j ++) {
			double x = data [i] [j];
			MelderFile_write (file, L"\t", Melder_double (x));
		}
	}
}

void structTableOfReal :: v_readText (MelderReadText a_text) {
	numberOfColumns = texgeti4 (a_text);
	if (numberOfColumns >= 1) {
		columnLabels = NUMvector <wchar_t*> (1, numberOfColumns);
		for (long i = 1; i <= numberOfColumns; i ++)
			columnLabels [i] = texgetw2 (a_text);
	}
	numberOfRows = texgeti4 (a_text);
	if (numberOfRows >= 1) {
		rowLabels = NUMvector <wchar_t*> (1, numberOfRows);
	}
	if (numberOfRows >= 1 && numberOfColumns >= 1) {
		data = NUMmatrix <double> (1, numberOfRows, 1, numberOfColumns);
		for (long i = 1; i <= numberOfRows; i ++) {
			rowLabels [i] = texgetw2 (a_text);
			for (long j = 1; j <= numberOfColumns; j ++)
				data [i] [j] = texgetr8 (a_text);
		}
	}
}

void structTableOfReal :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (numberOfRows));
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (numberOfColumns));
}

const wchar_t * structTableOfReal :: v_getRowStr (long irow) {
	if (irow < 1 || irow > numberOfRows) return NULL;
	return rowLabels [irow] ? rowLabels [irow] : L"";
}
const wchar_t * structTableOfReal :: v_getColStr (long icol) {
	if (icol < 1 || icol > numberOfColumns) return NULL;
	return columnLabels [icol] ? columnLabels [icol] : L"";
}
double structTableOfReal :: v_getMatrix (long irow, long icol) {
	if (irow < 1 || irow > numberOfRows) return NUMundefined;
	if (icol < 1 || icol > numberOfColumns) return NUMundefined;
	return data [irow] [icol];
}
double structTableOfReal :: v_getRowIndex (const wchar_t *rowLabel) {
	return TableOfReal_rowLabelToIndex (this, rowLabel);
}
double structTableOfReal :: v_getColIndex (const wchar_t *columnLabel) {
	return TableOfReal_columnLabelToIndex (this, columnLabel);
}

Thing_implement (TableOfReal, Data, 0);

void TableOfReal_init (TableOfReal me, long numberOfRows, long numberOfColumns) {
	if (numberOfRows < 1 || numberOfColumns < 1)
		Melder_throw ("Cannot create cell-less table.");
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	my rowLabels = NUMvector <wchar_t*> (1, numberOfRows);
	my columnLabels = NUMvector <wchar_t*> (1, numberOfColumns);
	my data = NUMmatrix <double> (1, my numberOfRows, 1, my numberOfColumns);
}

TableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns) {
	try {
		autoTableOfReal me = Thing_new (TableOfReal);
		TableOfReal_init (me.peek(), numberOfRows, numberOfColumns);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal not created.");
	}
}

/***** QUERY *****/

long TableOfReal_rowLabelToIndex (TableOfReal me, const wchar_t *label) {
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		if (my rowLabels [irow] && wcsequ (my rowLabels [irow], label))
			return irow;
	return 0;
}

long TableOfReal_columnLabelToIndex (TableOfReal me, const wchar_t *label) {
	for (long icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnLabels [icol] && wcsequ (my columnLabels [icol], label))
			return icol;
	return 0;
}

double TableOfReal_getColumnMean (TableOfReal me, long columnNumber) {
	double sum = 0.0;
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 1) return NUMundefined;
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		sum += my data [irow] [columnNumber];
	return sum / my numberOfRows;
}

double TableOfReal_getColumnStdev (TableOfReal me, long columnNumber) {
	double mean = TableOfReal_getColumnMean (me, columnNumber), sum = 0.0, d;
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 2) return NUMundefined;
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		sum += ( d = my data [irow] [columnNumber] - mean, d * d );
	return sqrt (sum / (my numberOfRows - 1));
}

/***** MODIFY *****/

void TableOfReal_removeRow (TableOfReal me, long rowNumber) {
	try {
		if (my numberOfRows == 1)
			Melder_throw (Thing_messageName (me), " has only one row, and a TableOfReal without rows cannot exist.");
		if (rowNumber < 1 || rowNumber > my numberOfRows)
			Melder_throw ("No row ", rowNumber, ".");
		autoNUMmatrix <double> data (1, my numberOfRows - 1, 1, my numberOfColumns);
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
		NUMmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfRows --;
	} catch (MelderError) {
		Melder_throw (me, ": row ", rowNumber, " not removed.");
	}
}

void TableOfReal_insertRow (TableOfReal me, long rowNumber) {
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows + 1)
			Melder_throw ("Cannot create row ", rowNumber, ".");
		autoNUMmatrix <double> data (1, my numberOfRows + 1, 1, my numberOfColumns);
		autoNUMvector <wchar_t *> rowLabels (1, my numberOfRows + 1);   // not autostringvector...
		for (long irow = 1; irow < rowNumber; irow ++)	{
			rowLabels [irow] = my rowLabels [irow];   // ...because this is a dangling copy
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
		NUMvector_free (my rowLabels, 1);
		my rowLabels = rowLabels.transfer();
		NUMmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfRows ++;
	} catch (MelderError) {
		Melder_throw (me, ": row at position ", rowNumber, " not inserted.");
	}
}

void TableOfReal_removeColumn (TableOfReal me, long columnNumber) {
	try {
		if (my numberOfColumns == 1)
			Melder_throw ("Cannot remove the only column.");
		if (columnNumber < 1 || columnNumber > my numberOfColumns)
			Melder_throw ("No column ", columnNumber, ".");
		autoNUMmatrix <double> data (1, my numberOfRows, 1, my numberOfColumns - 1);
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
		NUMmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfColumns --;
	} catch (MelderError) {
		Melder_throw (me, ": column at position ", columnNumber, " not inserted.");
	}
}

void TableOfReal_insertColumn (TableOfReal me, long columnNumber) {
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns + 1)
			Melder_throw ("Cannot create column ", columnNumber, ".");
		autoNUMmatrix <double> data (1, my numberOfRows, 1, my numberOfColumns + 1);
		autoNUMvector <wchar_t*> columnLabels (1, my numberOfColumns + 1);   // not autostringvector...
		for (long j = 1; j < columnNumber; j ++) {
			columnLabels [j] = my columnLabels [j];   // ...because this is a dangling copy
			for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j];
		}
		for (long j = my numberOfColumns + 1; j > columnNumber; j --) {
			columnLabels [j] = my columnLabels [j - 1];
			for (long i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j - 1];
		}
		/*
		 * Change without error.
		 */
		NUMvector_free (my columnLabels, 1);
		my columnLabels = columnLabels.transfer();
		NUMmatrix_free (my data, 1, 1);
		my data = data.transfer();
		my numberOfColumns ++;
	} catch (MelderError) {
		Melder_throw (me, ": column at position ", columnNumber, " not inserted.");
	}
}

void TableOfReal_setRowLabel (TableOfReal me, long rowNumber, const wchar_t *label) {
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows) return;
		autostring newLabel = Melder_wcsdup (label);
		/*
		 * Change without error.
		 */
		Melder_free (my rowLabels [rowNumber]);
		my rowLabels [rowNumber] = newLabel.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": label of row ", rowNumber, " not set.");
	}
}

void TableOfReal_setColumnLabel (TableOfReal me, long columnNumber, const wchar_t *label) {
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns) return;
		autostring newLabel = Melder_wcsdup (label);
		/*
		 * Change without error.
		 */
		Melder_free (my columnLabels [columnNumber]);
		my columnLabels [columnNumber] = newLabel.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": label of column ", columnNumber, " not set.");
	}
}

void TableOfReal_formula (TableOfReal me, const wchar_t *expression, Interpreter interpreter, TableOfReal thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (thee == NULL) thee = me;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				thy data [irow] [icol] = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not completed.");
	}
}

/***** EXTRACT PART *****/

static void copyRowLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		thy rowLabels [irow] = Melder_wcsdup (my rowLabels [irow]);
	}
}

static void copyColumnLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnLabels [icol] = Melder_wcsdup (my columnLabels [icol]);
	}
}

static void copyRow (TableOfReal me, long myRow, TableOfReal thee, long thyRow) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	thy rowLabels [thyRow] = Melder_wcsdup (my rowLabels [myRow]);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy data [thyRow] [icol] = my data [myRow] [icol];
	}
}

static void copyColumn (TableOfReal me, long myCol, TableOfReal thee, long thyCol) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	thy columnLabels [thyCol] = Melder_wcsdup (my columnLabels [myCol]);
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		thy data [irow] [thyCol] = my data [irow] [myCol];
	}
}

TableOfReal TableOfReal_extractRowsWhereColumn (TableOfReal me, long column, int which_Melder_NUMBER, double criterion) {
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
		copyColumnLabels (me, thee.peek());
		n = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_numberMatchesCriterion (my data [irow] [column], which_Melder_NUMBER, criterion))
				copyRow (me, irow, thee.peek(), ++ n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rows not extracted.");
	}
}

TableOfReal TableOfReal_extractRowsWhereLabel (TableOfReal me, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		long n = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			if (Melder_stringMatchesCriterion (my rowLabels [irow], which_Melder_STRING, criterion)) {
				n ++;
			}
		}
		if (n == 0)
			Melder_throw (L"No row matches this criterion.");
		autoTableOfReal thee = TableOfReal_create (n, my numberOfColumns);
		copyColumnLabels (me, thee.peek());
		n = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_stringMatchesCriterion (my rowLabels [irow], which_Melder_STRING, criterion))
				copyRow (me, irow, thee.peek(), ++ n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rows not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnsWhereRow (TableOfReal me, long row, int which_Melder_NUMBER, double criterion) {
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
		copyRowLabels (me, thee.peek());
		n = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			if (Melder_numberMatchesCriterion (my data [row] [icol], which_Melder_NUMBER, criterion))
				copyColumn (me, icol, thee.peek(), ++ n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": columns not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnsWhereLabel (TableOfReal me, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		long n = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (Melder_stringMatchesCriterion (my columnLabels [icol], which_Melder_STRING, criterion)) {
				n ++;
			}
		}
		if (n == 0) Melder_throw ("No column matches this criterion.");

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, n);
		copyRowLabels (me, thee.peek());
		n = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (Melder_stringMatchesCriterion (my columnLabels [icol], which_Melder_STRING, criterion)) {
				copyColumn (me, icol, thee.peek(), ++ n);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": columns not extracted.");
	}
}

/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static long *getElementsOfRanges (const wchar_t *ranges, long maximumElement, long *numberOfElements, const wchar_t *elementType) {
	/*
	 * Count the elements.
	 */
	long previousElement = 0;
	*numberOfElements = 0;
	const wchar_t *p = & ranges [0];
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
	autoNUMvector <long> elements (1, *numberOfElements);
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
}

TableOfReal TableOfReal_extractRowRanges (TableOfReal me, const wchar_t *ranges) {
	try {
		long numberOfElements;
		autoNUMvector <long> elements (getElementsOfRanges (ranges, my numberOfRows, & numberOfElements, L"row"), 1);
		autoTableOfReal thee = TableOfReal_create (numberOfElements, my numberOfColumns);
		copyColumnLabels (me, thee.peek());
		for (long ielement = 1; ielement <= numberOfElements; ielement ++)
			copyRow (me, elements [ielement], thee.peek(), ielement);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": row ranges not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnRanges (TableOfReal me, const wchar_t *ranges) {
	try {
		long numberOfElements;
		autoNUMvector <long> elements (getElementsOfRanges (ranges, my numberOfColumns, & numberOfElements, L"column"), 1);
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, numberOfElements);
		copyRowLabels (me, thee.peek());
		for (long ielement = 1; ielement <= numberOfElements; ielement ++)
			copyColumn (me, elements [ielement], thee.peek(), ielement);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": column ranges not extracted.");
	}
}

TableOfReal TableOfReal_extractRowsWhere (TableOfReal me, const wchar_t *condition, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		/*
		 * Count the new number of rows.
		 */
		long numberOfElements = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
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
		copyColumnLabels (me, thee.peek());
		/*
		 * Store the result.
		 */
		numberOfElements = 0;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult != 0.0) {
					copyRow (me, irow, thee.peek(), ++ numberOfElements);
					break;
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rows not extracted.");
	}
}

TableOfReal TableOfReal_extractColumnsWhere (TableOfReal me, const wchar_t *condition, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		/*
		 * Count the new number of columns.
		 */
		long numberOfElements = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			for (long irow = 1; irow <= my numberOfRows; irow ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult != 0.0) {
					numberOfElements ++;
					break;
				}
			}
		}
		if (numberOfElements < 1) Melder_throw ("No columns match this condition.");

		/*
		 * Create room for the result.
		 */	
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, numberOfElements);
		copyRowLabels (me, thee.peek());
		/*
		 * Store the result.
		 */
		numberOfElements = 0;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			for (long irow = 1; irow <= my numberOfRows; irow ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult != 0.0) {
					copyColumn (me, icol, thee.peek(), ++ numberOfElements);
					break;
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": columns not extracted.");
	}
}

/***** EXTRACT *****/

Strings TableOfReal_extractRowLabelsAsStrings (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <wchar_t *> (1, my numberOfRows);
		thy numberOfStrings = my numberOfRows;
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			thy strings [irow] = Melder_wcsdup (my rowLabels [irow] ? my rowLabels [irow] : L"");
		}
		return thee.transfer();	
	} catch (MelderError) {
		Melder_throw (me, ": row labels not extracted.");
	}
}

Strings TableOfReal_extractColumnLabelsAsStrings (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <wchar_t *> (1, my numberOfColumns);
		thy numberOfStrings = my numberOfColumns;
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			thy strings [icol] = Melder_wcsdup (my columnLabels [icol] ? my columnLabels [icol] : L"");
		}
		return thee.transfer();	
	} catch (MelderError) {
		Melder_throw (me, ": column labels not extracted.");
	}
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
static double getMaxColumnLabelHeight (TableOfReal me, Graphics graphics, long colmin, long colmax) {
	double maxHeight = 0.0, lineSpacing = getLineSpacing (graphics);
	if (! my columnLabels) return 0.0;
	fixRows (me, & colmin, & colmax);
	for (long icol = colmin; icol <= colmax; icol ++) if (my columnLabels [icol] && my columnLabels [icol] [0]) {
		if (! maxHeight) maxHeight = lineSpacing;
	}
	return maxHeight;
}

void TableOfReal_drawAsNumbers (TableOfReal me, Graphics graphics, long rowmin, long rowmax, int iformat, int precision) {
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0, 1);
	double leftMargin = getLeftMargin (graphics);   // not earlier!
	double lineSpacing = getLineSpacing (graphics);   // not earlier!
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
			Graphics_text (graphics, icol, 1, my columnLabels [icol]);
	}
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		double y = 1 - lineSpacing * (irow - rowmin + 0.6);
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
			Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow]);
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			wchar_t text [40];
			print4 (text, my data [irow] [icol], iformat, 0, precision);
			Graphics_text (graphics, icol, y, text);
		}
	}
	if (maxTextHeight) {
		double left = 0.5;
		if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (graphics, left, 1, my numberOfColumns + 0.5, 1);
	}
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawAsNumbers_if (TableOfReal me, Graphics graphics, long rowmin, long rowmax, int iformat, int precision,
	const wchar_t *conditionFormula, Interpreter interpreter)
{
	try {
		autoMatrix original = TableOfReal_to_Matrix (me);
		autoMatrix conditions = original.clone ();
		fixRows (me, & rowmin, & rowmax);
		Graphics_setInner (graphics);
		Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0, 1);
		double leftMargin = getLeftMargin (graphics);   // not earlier!
		double lineSpacing = getLineSpacing (graphics);   // not earlier!
		double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
		double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);
		Matrix_formula (original.peek(), conditionFormula, interpreter, conditions.peek());

		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
				Graphics_text (graphics, icol, 1, my columnLabels [icol]);
		}
		for (long irow = rowmin; irow <= rowmax; irow ++) {
			double y = 1 - lineSpacing * (irow - rowmin + 0.6);
			Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
			if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
				Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow]);
			Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
			for (long icol = 1; icol <= my numberOfColumns; icol ++) if (conditions -> z [irow] [icol] != 0.0) {
				wchar_t text [40];
				print4 (text, my data [irow] [icol], iformat, 0, precision);
				Graphics_text (graphics, icol, y, text);
			}
		}
		if (maxTextHeight) {
			double left = 0.5;
			if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
			Graphics_line (graphics, left, 1, my numberOfColumns + 0.5, 1);
		}
		Graphics_unsetInner (graphics);
	} catch (MelderError) {
		Melder_throw (me, ": numbers not drawn.");
	}
}

void TableOfReal_drawVerticalLines (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	double lineSpacing = getLineSpacing (graphics);   // not earlier!
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	if (maxTextWidth > 0.0) colmin -= 1;
	for (long col = colmin + 1; col <= colmax; col ++)
		Graphics_line (graphics, col - 0.5, 1 + maxTextHeight, col - 0.5, 1 - lineSpacing * (rowmax - rowmin + 1));
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawLeftAndRightLines (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	double lineSpacing = getLineSpacing (graphics);
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	double right = colmax + 0.5;
	double top = 1 + maxTextHeight;
	double bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (graphics, left, top, left, bottom);
	Graphics_line (graphics, right, top, right, bottom);
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawHorizontalLines (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	double lineSpacing = getLineSpacing (graphics);
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	double top = rowmin;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	if (maxTextHeight > 0.0) rowmin -= 1;
	double right = colmax + 0.5;
	for (long irow = rowmin; irow < rowmax; irow ++) {
		double y = 1 - lineSpacing * (irow - top + 1);
		Graphics_line (graphics, left, y, right, y);
	}
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawTopAndBottomLines (TableOfReal me, Graphics graphics, long rowmin, long rowmax) {
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	double lineSpacing = getLineSpacing (graphics);
	double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	double right = colmax + 0.5;
	double top = 1 + maxTextHeight;
	double bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (graphics, left, top, right, top);
	Graphics_line (graphics, left, bottom, right, bottom);
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawAsSquares (TableOfReal me, Graphics graphics, long rowmin, long rowmax,
	long colmin, long colmax, int garnish)
{
	double dx = 1, dy = 1;
	Graphics_Colour colour = Graphics_inqColour (graphics);
	fixRows (me, & rowmin, & rowmax);
	fixColumns (me, & colmin, & colmax);
	
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5);
	double datamax = my data [rowmin] [colmin];
	for (long irow = 1; irow <= my numberOfRows; irow ++)
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			if (fabs (my data [irow] [icol]) > datamax) datamax = fabs (my data [irow] [icol]);
	
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		double y = rowmax + rowmin - irow;
		for (long icol = colmin; icol <= colmax; icol ++) {
			double x = icol;
			/* two neighbouring squares should not touch -> 0.95 */
			double d = 0.95 * sqrt (fabs (my data [irow] [icol]) / datamax);
			double x1WC = x - d * dx / 2, x2WC = x + d * dx / 2;
			double y1WC = y - d * dy / 2, y2WC = y + d * dy / 2;
			if (my data [irow] [icol] > 0) Graphics_setColour (graphics, Graphics_WHITE);
			Graphics_fillRectangle (graphics, x1WC, x2WC, y1WC, y2WC);
			Graphics_setColour (graphics, colour);
			Graphics_rectangle (graphics, x1WC, x2WC , y1WC, y2WC);
		}
	}
	Graphics_setGrey (graphics, 0.0);
	Graphics_unsetInner (graphics);
	if (garnish) {
		for (long irow = rowmin; irow <= rowmax; irow ++) if (my rowLabels [irow]) 
			Graphics_markLeft (graphics, rowmax + rowmin - irow, 0, 0, 0, my rowLabels [irow]);
		for (long icol = colmin; icol <= colmax; icol ++) if (my columnLabels [icol])
			Graphics_markTop (graphics, icol, 0, 0, 0, my columnLabels [icol]);
	}
}

Any TablesOfReal_append (TableOfReal me, TableOfReal thee) {
	try {
		if (thy numberOfColumns != my numberOfColumns)
			Melder_throw (L"Numbers of columns are ", my numberOfColumns, " and ", thy numberOfColumns, " but should be equal.");
		autoTableOfReal him = static_cast <TableOfReal> (_Thing_new (my classInfo));
		TableOfReal_init (him.peek(), my numberOfRows + thy numberOfRows, my numberOfColumns);
		/* Unsafe: new attributes not initialized. */
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (him.peek(), icol, my columnLabels [icol]);
		}
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			TableOfReal_setRowLabel (him.peek(), irow, my rowLabels [irow]);
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				his data [irow] [icol] = my data [irow] [icol];
		}
		for (long irow = 1; irow <= thy numberOfRows; irow ++) {
			long hisRow = irow + my numberOfRows;
			TableOfReal_setRowLabel (him.peek(), hisRow, thy rowLabels [irow]);
			for (long icol = 1; icol <= my numberOfColumns; icol ++)
				his data [hisRow] [icol] = thy data [irow] [icol];
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal objects not appended.");
	}
}

Any TablesOfReal_appendMany (Collection me) {
	try {
		if (my size == 0) Melder_throw ("Cannot add zero tables.");
		TableOfReal thee = static_cast <TableOfReal> (my item [1]);
		long totalNumberOfRows = thy numberOfRows;
		long numberOfColumns = thy numberOfColumns;
		for (long itab = 2; itab <= my size; itab ++) {
			thee = static_cast <TableOfReal> (my item [itab]);
			totalNumberOfRows += thy numberOfRows;
			if (thy numberOfColumns != numberOfColumns) Melder_throw ("Numbers of columns do not match.");
		}
		autoTableOfReal him = static_cast <TableOfReal> (_Thing_new (thy classInfo));
		TableOfReal_init (him.peek(), totalNumberOfRows, numberOfColumns);
		/* Unsafe: new attributes not initialized. */
		for (long icol = 1; icol <= numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (him.peek(), icol, thy columnLabels [icol]);
		}
		totalNumberOfRows = 0;
		for (long itab = 1; itab <= my size; itab ++) {
			thee = static_cast <TableOfReal> (my item [itab]);
			for (long irow = 1; irow <= thy numberOfRows; irow ++) {
				totalNumberOfRows ++;
				TableOfReal_setRowLabel (him.peek(), totalNumberOfRows, thy rowLabels [irow]);
				for (long icol = 1; icol <= numberOfColumns; icol ++)
					his data [totalNumberOfRows] [icol] = thy data [irow] [icol];
			}
		}
		Melder_assert (totalNumberOfRows == his numberOfRows);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal objects not appended.");
	}
}

static void TableOfReal_sort (TableOfReal me, bool useLabels, long column1, long column2) {
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

void TableOfReal_sortByLabel (TableOfReal me, long column1, long column2) {
	TableOfReal_sort (me, true, column1, column2);
}

void TableOfReal_sortByColumn (TableOfReal me, long column1, long column2) {
	TableOfReal_sort (me, false, column1, column2);
}

TableOfReal Table_to_TableOfReal (Table me, long labelColumn) {
	try {
		if (labelColumn < 1 || labelColumn > my numberOfColumns) labelColumn = 0;
		autoTableOfReal thee = TableOfReal_create (my rows -> size, labelColumn ? my numberOfColumns - 1 : my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			Table_numericize_Assert (me, icol);
		}
		if (labelColumn) {
			for (long icol = 1; icol < labelColumn; icol ++) {
				TableOfReal_setColumnLabel (thee.peek(), icol, my columnHeaders [icol]. label);
			}
			for (long icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
				TableOfReal_setColumnLabel (thee.peek(), icol - 1, my columnHeaders [icol]. label);
			}
			for (long irow = 1; irow <= my rows -> size; irow ++) {
				TableRow row = static_cast <TableRow> (my rows -> item [irow]);
				wchar_t *string = row -> cells [labelColumn]. string;
				TableOfReal_setRowLabel (thee.peek(), irow, string ? string : L"");
				for (long icol = 1; icol < labelColumn; icol ++) {
					thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
					//thy data [irow] [icol] = Table_getNumericValue_Assert (me, irow, icol);
				}
				for (long icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
					thy data [irow] [icol - 1] = row -> cells [icol]. number;   // Optimization.
					//thy data [irow] [icol - 1] = Table_getNumericValue_Assert (me, irow, icol);
				}
			}
		} else {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				TableOfReal_setColumnLabel (thee.peek(), icol, my columnHeaders [icol]. label);
			}
			for (long irow = 1; irow <= my rows -> size; irow ++) {
				TableRow row = static_cast <TableRow> (my rows -> item [irow]);
				for (long icol = 1; icol <= my numberOfColumns; icol ++) {
					thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
					//thy data [irow] [icol] = Table_getNumericValue_Assert (me, irow, icol);
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

Table TableOfReal_to_Table (TableOfReal me, const wchar_t *labelOfFirstColumn) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my numberOfRows, my numberOfColumns + 1);
		Table_setColumnLabel (thee.peek(), 1, labelOfFirstColumn);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			wchar_t *columnLabel = my columnLabels [icol];
			thy columnHeaders [icol + 1]. label = Melder_wcsdup (columnLabel && columnLabel [0] ? columnLabel : L"?");
		}
		for (long irow = 1; irow <= thy rows -> size; irow ++) {
			wchar_t *stringValue = my rowLabels [irow];
			TableRow row = static_cast <TableRow> (thy rows -> item [irow]);
			row -> cells [1]. string = Melder_wcsdup (stringValue && stringValue [0] ? stringValue : L"?");
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				double numericValue = my data [irow] [icol];
				row -> cells [icol + 1]. string = Melder_wcsdup (Melder_double (numericValue));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Table.");
	}
}

void TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file) {
	try {
		autoMelderString buffer;
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
		MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
	} catch (MelderError) {
		Melder_throw (me, ": not saved to tab-separated file.");
	}
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
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			TableOfReal_setColumnLabel (me.peek(), icol, buffer.string);
			MelderString_empty (& buffer);
		}
		for (long irow = 1; irow <= nrow; irow ++) {
			while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			while (*p != ' ' && *p != '\t') {
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			TableOfReal_setRowLabel (me.peek(), irow, buffer.string);
			MelderString_empty (& buffer);
			for (long icol = 1; icol <= ncol; icol ++) {
				while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
				MelderString_empty (& buffer);
				while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') {
					MelderString_appendCharacter (& buffer, *p);
					p ++;
				}
				my data [irow] [icol] = Melder_atof (buffer.string);   /* If cell contains a string, this will be 0. */
				MelderString_empty (& buffer);
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal: tab-separated file ", file, " not read.");
	}
}

/* End of file TableOfReal.cpp */
