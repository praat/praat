/* TableOfReal.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
#include "oo_WRITE_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_READ_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_DESCRIPTION.h"
#include "TableOfReal_def.h"

static void fprintquotedstring (FILE *f, const char *s) {
	fputc ('\"', f);
	if (s) { char c; while ((c = *s ++) != '\0') { fputc (c, f); if (c == '\"') fputc (c, f); } }
	fputc ('\"', f);
}

static int writeAscii (I, FILE *f) {
	iam (TableOfReal);
	long i, j;
	ascputi4 (my numberOfColumns, f, "numberOfColumns");
	fprintf (f, "\ncolumnLabels []: %s\n", my numberOfColumns >= 1 ? "" : "(empty)");
	for (i = 1; i <= my numberOfColumns; i ++) {
		fprintquotedstring (f, my columnLabels [i]);
		fputc ('\t', f);
	}
	ascputi4 (my numberOfRows, f, "numberOfRows");
	for (i = 1; i <= my numberOfRows; i ++) {
		fprintf (f, "\nrow [%ld]: ", i);
		fprintquotedstring (f, my rowLabels [i]);
		for (j = 1; j <= my numberOfColumns; j ++) {
			double x = my data [i] [j];
			fprintf (f, "\t%s", Melder_double (x));
		}
	}
	return 1;
}

static int readAscii (I, FILE *f) {
	iam (TableOfReal);
	long i, j;
	my numberOfColumns = ascgeti4 (f);
	if (my numberOfColumns >= 1) {
		if (! (my columnLabels = NUMvector (sizeof (char *), 1, my numberOfColumns))) return 0;
		for (i = 1; i <= my numberOfColumns; i ++)
			if (! (my columnLabels [i] = ascgets2 (f))) return 0;
	}
	my numberOfRows = ascgeti4 (f);
	if (my numberOfRows >= 1) {
		if (! (my rowLabels = NUMvector (sizeof (char *), 1, my numberOfRows))) return 0;
	}
	if (my numberOfRows >= 1 && my numberOfColumns >= 1) {
		if (! (my data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns))) return 0;
		for (i = 1; i <= my numberOfRows; i ++) {
			if (! (my rowLabels [i] = ascgets2 (f))) return 0;
			for (j = 1; j <= my numberOfColumns; j ++)
				my data [i] [j] = ascgetr8 (f);
		}
	}
	return 1;
}

static void info (I) {
	iam (TableOfReal);
	Melder_information ("Number of rows = %ld\nNumber of columns = %ld",
		my numberOfRows, my numberOfColumns);
}

static double getNrow (I) { iam (TableOfReal); return my numberOfRows; }
static double getNcol (I) { iam (TableOfReal); return my numberOfColumns; }
static double getMatrix (I, long irow, long icol) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	return my data [irow] [icol];
}
static double getRowIndex (I, const char *rowLabel) {
	iam (TableOfReal);
	return TableOfReal_rowLabelToIndex (me, rowLabel);
}
static double getColumnIndex (I, const char *columnLabel) {
	iam (TableOfReal);
	return TableOfReal_columnLabelToIndex (me, columnLabel);
}

class_methods (TableOfReal, Data)
	class_method_local (TableOfReal, destroy)
	class_method_local (TableOfReal, description)
	class_method_local (TableOfReal, copy)
	class_method_local (TableOfReal, equal)
	class_method (writeAscii)
	class_method (readAscii)
	class_method_local (TableOfReal, writeBinary)
	class_method_local (TableOfReal, readBinary)
	class_method (info)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getMatrix)
	class_method (getRowIndex)
	class_method (getColumnIndex)
class_methods_end

int TableOfReal_init (I, long numberOfRows, long numberOfColumns) {
	iam (TableOfReal);
	if (numberOfRows < 1 || numberOfColumns < 1)
		return Melder_error ("(TableOfReal_init:) Cannot create cell-less table.");
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! (my rowLabels = NUMvector (sizeof (char *), 1, numberOfRows))) return 0;
	if (! (my columnLabels = NUMvector (sizeof (char *), 1, numberOfColumns))) return 0;
	if (! (my data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns))) return 0;
	return 1;
}

TableOfReal TableOfReal_create (long numberOfRows, long numberOfColumns) {
	TableOfReal me = new (TableOfReal);
	if (! me || ! TableOfReal_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

/***** QUERY *****/

long TableOfReal_rowLabelToIndex (I, const char *label) {
	iam (TableOfReal);
	long irow;
	for (irow = 1; irow <= my numberOfRows; irow ++)
		if (my rowLabels [irow] && strequ (my rowLabels [irow], label))
			return irow;
	return 0;
}

long TableOfReal_columnLabelToIndex (I, const char *label) {
	iam (TableOfReal);
	long icol;
	for (icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnLabels [icol] && strequ (my columnLabels [icol], label))
			return icol;
	return 0;
}

double TableOfReal_getColumnMean (I, long icol) {
	iam (TableOfReal);
	double sum = 0.0;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 1) return NUMundefined;
	for (irow = 1; irow <= my numberOfRows; irow ++)
		sum += my data [irow] [icol];
	return sum / my numberOfRows;
}

double TableOfReal_getColumnStdev (I, long icol) {
	iam (TableOfReal);
	double mean = TableOfReal_getColumnMean (me, icol), sum = 0.0, d;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my numberOfRows < 2) return NUMundefined;
	for (irow = 1; irow <= my numberOfRows; irow ++)
		sum += ( d = my data [irow] [icol] - mean, d * d );
	return sqrt (sum / (my numberOfRows - 1));
}

/***** MODIFY *****/

int TableOfReal_removeRow (I, long irow) {
	iam (TableOfReal);
	double **data = NULL;
	long i, j;
	if (my numberOfRows == 1)
		{ Melder_error ("Cannot remove the only row."); goto end; }
	if (irow < 1 || irow > my numberOfRows)
		{ Melder_error ("No row %ld.", irow); goto end; }
	data = NUMdmatrix (1, my numberOfRows - 1, 1, my numberOfColumns); cherror
	for (i = 1; i <= my numberOfColumns; i ++) {
		for (j = 1; j < irow; j ++) data [j] [i] = my data [j] [i];
		for (j = irow; j < my numberOfRows; j ++) data [j] [i] = my data [j + 1] [i];
	}
	Melder_free (my rowLabels [irow]);
	for (j = irow; j < my numberOfRows; j ++) my rowLabels [j] = my rowLabels [j + 1];
	NUMdmatrix_free (my data, 1, 1);
	my data = data;
	my numberOfRows --;
end:
	iferror return Melder_error ("(TableOfReal_removeRow:) Not performed.");
	return 1;
}

int TableOfReal_insertRow (I, long irow) {
	iam (TableOfReal);
	double **data = NULL;
	char **rowLabels = NULL;
	long i, j;
	if (irow < 1 || irow > my numberOfRows + 1)
		{ Melder_error ("Cannot create row %ld.", irow); goto end; }
	data = NUMdmatrix (1, my numberOfRows + 1, 1, my numberOfColumns); cherror
	rowLabels = NUMpvector (1, my numberOfRows + 1);
	for (j = 1; j < irow; j ++)	{
		rowLabels [j] = my rowLabels [j];
		for (i = 1; i <= my numberOfColumns; i ++) data [j] [i] = my data [j] [i];
	}
	for (j = my numberOfRows + 1; j > irow; j --) {
		rowLabels [j] = my rowLabels [j - 1];
		for (i = 1; i <= my numberOfColumns; i ++) data [j] [i] = my data [j - 1] [i];
	}
	NUMdmatrix_free (my data, 1, 1);
	NUMpvector_free (my rowLabels, 1);
	my data = data;
	my rowLabels = rowLabels;
	my numberOfRows ++;
end:
	iferror return Melder_error ("(TableOfReal_insertRow:) Not performed.");
	return 1;
}

int TableOfReal_removeColumn (I, long icol) {
	iam (TableOfReal);
	double **data = NULL;
	long i, j;
	if (my numberOfColumns == 1)
		{ Melder_error ("Cannot remove the only column."); goto end; }
	if (icol < 1 || icol > my numberOfColumns)
		{ Melder_error ("No column %ld.", icol); goto end; }
	data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns - 1); cherror
	for (i = 1; i <= my numberOfRows; i ++) {
		for (j = 1; j < icol; j ++) data [i] [j] = my data [i] [j];
		for (j = icol; j < my numberOfColumns; j ++) data [i] [j] = my data [i] [j + 1];
	}
	Melder_free (my columnLabels [icol]);
	for (j = icol; j < my numberOfColumns; j ++) my columnLabels [j] = my columnLabels [j + 1];
	NUMdmatrix_free (my data, 1, 1);
	my data = data;
	my numberOfColumns --;
end:
	iferror return Melder_error ("(TableOfReal_removeColumn:) Not performed.");
	return 1;
}

int TableOfReal_insertColumn (I, long icol) {
	iam (TableOfReal);
	double **data = NULL;
	char **columnLabels = NULL;
	long i, j;
	if (icol < 1 || icol > my numberOfColumns + 1)
		{ Melder_error ("Cannot create column %ld.", icol); goto end; }
	data = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns + 1); cherror
	columnLabels = NUMpvector (1, my numberOfColumns + 1); cherror
	for (j = 1; j < icol; j ++) {
		columnLabels [j] = my columnLabels [j];
		for (i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j];
	}
	for (j = my numberOfColumns + 1; j > icol; j --) {
		columnLabels [j] = my columnLabels [j - 1];
		for (i = 1; i <= my numberOfRows; i ++) data [i] [j] = my data [i] [j - 1];
	}
	NUMdmatrix_free (my data, 1, 1);
	NUMpvector_free (my columnLabels, 1);
	my data = data;
	my columnLabels = columnLabels;
	my numberOfColumns ++;
end:
	iferror return Melder_error ("(TableOfReal_insertColumn:) Not performed.");
	return 1;
}

void TableOfReal_setRowLabel (I, long irow, const char *label) {
	iam (TableOfReal);
	if (irow < 1 || irow > my numberOfRows || label == my rowLabels [irow]) return;
	Melder_free (my rowLabels [irow]);
	my rowLabels [irow] = Melder_strdup (label);
}

void TableOfReal_setColumnLabel (I, long icol, const char *label) {
	iam (TableOfReal);
	if (icol < 1 || icol > my numberOfColumns || label == my columnLabels [icol]) return;
	Melder_free (my columnLabels [icol]);
	my columnLabels [icol] = Melder_strdup (label);
}

int TableOfReal_formula (I, const char *expression, thou) {
	iam (TableOfReal);
	thouart (TableOfReal);
	long irow, icol;
	if (! Formula_compile (NULL, me, expression, FALSE, TRUE)) return 0;
	if (thee == NULL) thee = me;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			double result;
			if (! Formula_run (irow, icol, & result, NULL)) return 0;
			thy data [irow] [icol] = result;
		}
	}
	return 1;
}

/***** EXTRACT PART *****/

static void copyRowLabels (TableOfReal me, TableOfReal thee) {
	long irow;
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		thy rowLabels [irow] = Melder_strdup (my rowLabels [irow]); iferror return;
	}
}

static void copyColumnLabels (TableOfReal me, TableOfReal thee) {
	long icol;
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnLabels [icol] = Melder_strdup (my columnLabels [icol]); iferror return;
	}
}

static void copyRow (TableOfReal me, long myRow, TableOfReal thee, long thyRow) {
	long icol;
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	thy rowLabels [thyRow] = Melder_strdup (my rowLabels [myRow]); iferror return;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy data [thyRow] [icol] = my data [myRow] [icol];
	}
}

static void copyColumn (TableOfReal me, long myCol, TableOfReal thee, long thyCol) {
	long irow;
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	thy columnLabels [thyCol] = Melder_strdup (my columnLabels [myCol]); iferror return;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		thy data [irow] [thyCol] = my data [irow] [myCol];
	}
}

static doesNumericValueMatchCriterion (double value, int which, double criterion) {
	return
		which == TableOfReal_EQUAL_TO && value == criterion ||
		which == TableOfReal_NOT_EQUAL_TO && value != criterion ||
		which == TableOfReal_LESS_THAN && value < criterion ||
		which == TableOfReal_LESS_THAN_OR_EQUAL_TO && value <= criterion ||
		which == TableOfReal_GREATER_THAN && value > criterion ||
		which == TableOfReal_GREATER_THAN_OR_EQUAL_TO && value >= criterion;
}

static int doesStringValueMatchCriterion (const char *value, int which, const char *criterion) {
	if (value == NULL) {
		value = "";   /* Regard null strings as empty strings, as is usual in Praat. */
	}
	if (which <= TableOfReal_NOT_EQUAL_TO) {
		int matchPositiveCriterion = strequ (value, criterion);
		return (which == TableOfReal_EQUAL_TO) == matchPositiveCriterion;
	}
	if (which <= TableOfReal_DOES_NOT_CONTAIN) {
		int matchPositiveCriterion = strstr (value, criterion) != NULL;
		return (which == TableOfReal_CONTAINS) == matchPositiveCriterion;
	}
	if (which <= TableOfReal_DOES_NOT_START_WITH) {
		int matchPositiveCriterion = strnequ (value, criterion, strlen (criterion));
		return (which == TableOfReal_STARTS_WITH) == matchPositiveCriterion;
	}
	if (which <= TableOfReal_DOES_NOT_END_WITH) {
		int criterionLength = strlen (criterion), valueLength = strlen (value);
		int matchPositiveCriterion = criterionLength <= valueLength && strequ (value + valueLength - criterionLength, criterion);
		return (which == TableOfReal_ENDS_WITH) == matchPositiveCriterion;
	}
	return 0;   /* Should not occur. */
}

TableOfReal TableOfReal_extractRowsWhereColumn (I, long column, int which, double criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, n = 0;
	if (column < 1 || column > my numberOfColumns) {
		Melder_error ("No such column: %ld.", column);
		goto end;
	}
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		if (doesNumericValueMatchCriterion (my data [irow] [column], which, criterion)) {
			n ++;
		}
	}
	if (n == 0) {
		Melder_error ("No row matches this criterion.");
		goto end;
	}
	thee = TableOfReal_create (n, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	n = 0;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		if (doesNumericValueMatchCriterion (my data [irow] [column], which, criterion)) {
			copyRow (me, irow, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractRowsWhereLabel (I, int which, const char *criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, n = 0;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		if (doesStringValueMatchCriterion (my rowLabels [irow], which, criterion)) {
			n ++;
		}
	}
	if (n == 0) {
		Melder_error ("No row matches this criterion.");
		goto end;
	}
	thee = TableOfReal_create (n, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	n = 0;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		if (doesStringValueMatchCriterion (my rowLabels [irow], which, criterion)) {
			copyRow (me, irow, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractColumnsWhereRow (I, long row, int which, double criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long icol, n = 0;
	if (row < 1 || row > my numberOfRows) {
		Melder_error ("No such row: %ld.", row);
		goto end;
	}
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		if (doesNumericValueMatchCriterion (my data [row] [icol], which, criterion)) {
			n ++;
		}
	}
	if (n == 0) {
		Melder_error ("No column matches this criterion.");
		goto end;
	}
	thee = TableOfReal_create (my numberOfRows, n);
	copyRowLabels (me, thee); cherror
	n = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		if (doesNumericValueMatchCriterion (my data [row] [icol], which, criterion)) {
			copyColumn (me, icol, thee, ++ n); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractColumnsWhereLabel (I, int which, const char *criterion) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long icol, n = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		if (doesStringValueMatchCriterion (my columnLabels [icol], which, criterion)) {
			n ++;
		}
	}
	if (n == 0) {
		Melder_error ("No column matches this criterion.");
		goto end;
	}
	thee = TableOfReal_create (my numberOfRows, n);
	copyRowLabels (me, thee); cherror
	n = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		if (doesStringValueMatchCriterion (my columnLabels [icol], which, criterion)) {
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
static long *getElementsOfRanges (const char *ranges, long maximumElement, long *numberOfElements, const char *elementType)
{
	const char *p;
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
			currentElement = atol (p);
			if (currentElement == 0) {
				return Melder_errorp ("No such %s: 0 (minimum is 1).", elementType);
			}
			if (currentElement > maximumElement) {
				return Melder_errorp ("No such %s: %ld (maximum is %ld).", elementType, currentElement, maximumElement);
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
			currentElement = atol (p);
			if (currentElement == 0) {
				return Melder_errorp ("No such %s: 0 (minimum is 1).", elementType);
			}
			if (currentElement > maximumElement) {
				return Melder_errorp ("No such %s: %ld (maximum is %ld).", elementType, currentElement, maximumElement);
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
			currentElement = atol (p);
			elements [++ *numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (isdigit (*p));
		} else if (*p == ':') {
			do { p ++; } while (*p == ' ' || *p == '\t');
			currentElement = atol (p);
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

TableOfReal TableOfReal_extractRowRanges (I, const char *ranges) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long *elements = NULL, numberOfElements, ielement;
	elements = getElementsOfRanges (ranges, my numberOfRows, & numberOfElements, "row"); cherror
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

TableOfReal TableOfReal_extractColumnRanges (I, const char *ranges) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long *elements = NULL, numberOfElements, ielement;
	elements = getElementsOfRanges (ranges, my numberOfColumns, & numberOfElements, "column"); cherror
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

TableOfReal TableOfReal_extractRowsWhere (I, const char *condition) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, icol, numberOfElements;
	Formula_compile (NULL, me, condition, FALSE, TRUE); cherror
	/*
	 * Count the new number of rows.
	 */
	numberOfElements = 0;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			double numericResult;
			Formula_run (irow, icol, & numericResult, NULL); cherror
			if (numericResult != 0.0) {
				numberOfElements ++;
				break;
			}
		}
	}
	if (numberOfElements < 1) {
		Melder_error ("No rows match this condition.");
		goto end;
	}
	/*
	 * Create room for the result.
	 */	
	thee = TableOfReal_create (numberOfElements, my numberOfColumns); cherror
	copyColumnLabels (me, thee); cherror
	/*
	 * Store the result.
	 */
	numberOfElements = 0;
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			double numericResult;
			Formula_run (irow, icol, & numericResult, NULL);
			if (numericResult != 0.0) {
				copyRow (me, irow, thee, ++ numberOfElements); cherror
				break;
			}
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TableOfReal_extractColumnsWhere (I, const char *condition) {
	iam (TableOfReal);
	TableOfReal thee = NULL;
	long irow, icol, numberOfElements;
	Formula_compile (NULL, me, condition, FALSE, TRUE); cherror
	/*
	 * Count the new number of columns.
	 */
	numberOfElements = 0;
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			double numericResult;
			Formula_run (irow, icol, & numericResult, NULL); cherror
			if (numericResult != 0.0) {
				numberOfElements ++;
				break;
			}
		}
	}
	if (numberOfElements < 1) {
		Melder_error ("No columns match this condition.");
		goto end;
	}
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
			double numericResult;
			Formula_run (irow, icol, & numericResult, NULL);
			if (numericResult != 0.0) {
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
		thy strings [irow] = Melder_strdup (my rowLabels [irow] ? my rowLabels [irow] : ""); cherror
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
		thy strings [icol] = Melder_strdup (my columnLabels [icol] ? my columnLabels [icol] : ""); cherror
	}
end:
	iferror forget (thee);
	return thee;	
}

Matrix TableOfReal_to_Matrix (I) {
	iam (TableOfReal);
	long i, j;
	Matrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns); cherror
	for (i = 1; i <= my numberOfRows; i ++) for (j = 1; j <= my numberOfColumns; j ++)
		thy z [i] [j] = my data [i] [j];
end:
	iferror return NULL;
	return thee;
}

TableOfReal Matrix_to_TableOfReal (I) {
	iam (Matrix);
	long i, j;
	TableOfReal thee = TableOfReal_create (my ny, my nx); cherror
	for (i = 1; i <= my ny; i ++) for (j = 1; j <= my nx; j ++)
		thy data [i] [j] = my z [i] [j];
end:
	iferror return NULL;
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

static void print4 (char *buffer, double value, int iformat, int width, int precision) {
	char formatString [40];
	if (value == NUMundefined) strcpy (buffer, "undefined");
	else if (iformat == 4) {
		long numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			sprintf (buffer, "0");
		else if (denominator > 1)
			sprintf (buffer, "%ld/%ld", numerator, denominator);
		else
			sprintf (buffer, "%.7g", value);
	} else {
		sprintf (formatString, "%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		sprintf (buffer, formatString, value);
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
		double textWidth = Graphics_textWidth_ps (g, my rowLabels [row]);
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
			char text [40];
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
	const char *conditionFormula)
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
			char text [40];
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
	TableOfReal him = NULL;
	long irow, icol;
	if (thy numberOfColumns != my numberOfColumns)
		{ Melder_error ("Numbers of columns do not match."); goto end; }
	him = Thing_new (my methods); cherror
	TableOfReal_init (him, my numberOfRows + thy numberOfRows, my numberOfColumns); cherror
	/* Unsafe: new attributes not initialized. */
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		TableOfReal_setColumnLabel (him, icol, my columnLabels [icol]); cherror
	}
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		TableOfReal_setRowLabel (him, irow, my rowLabels [irow]); cherror
		for (icol = 1; icol <= my numberOfColumns; icol ++)
			his data [irow] [icol] = my data [irow] [icol];
	}
	for (irow = 1; irow <= thy numberOfRows; irow ++) {
		long hisRow = irow + my numberOfRows;
		TableOfReal_setRowLabel (him, hisRow, thy rowLabels [irow]); cherror
		for (icol = 1; icol <= my numberOfColumns; icol ++)
			his data [hisRow] [icol] = thy data [irow] [icol];
	}
end:
	iferror { forget (him); Melder_error ("(TablesOfReal_append:) Not performed."); }
	return him;
}

Any TablesOfReal_appendMany (Collection me) {
	TableOfReal him = NULL, thee;
	long itab, irow, icol, nrow, ncol;
	if (my size == 0) return Melder_errorp ("Cannot add zero tables.");
	thee = my item [1];
	nrow = thy numberOfRows;
	ncol = thy numberOfColumns;
	for (itab = 2; itab <= my size; itab ++) {
		thee = my item [itab];
		nrow += thy numberOfRows;
		if (thy numberOfColumns != ncol)
			{ Melder_error ("Numbers of columns do not match."); goto end; }
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
	iferror { forget (him); Melder_error ("(TablesOfReal_appendMany:) Not performed."); }
	return him;
}

static void TableOfReal_sort (TableOfReal me, int useLabels, long column1, long column2) {
	long i, j;
	for (i = 1; i < my numberOfRows; i ++) for (j = i + 1; j <= my numberOfRows; j ++) {
		char *tmpString;
		long column;
		if (useLabels) {
			if (my rowLabels [i] != NULL) {
				if (my rowLabels [j] != NULL) {
					int compare = strcmp (my rowLabels [i], my rowLabels [j]);
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

int TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file) {
	FILE *f = Melder_fopen (file, "w");
	long i, j;
	if (! f) return 0;
	fprintf (f, "rowLabel");
	for (i = 1; i <= my numberOfColumns; i ++) {
		fprintf (f, "\t%s", my columnLabels [i] && my columnLabels [i] [0] ? my columnLabels [i] : "?");
	}
	for (i = 1; i <= my numberOfRows; i ++) {
		fprintf (f, "\n%s", my rowLabels [i] && my rowLabels [i] [0] ? my rowLabels [i] : "?");
		for (j = 1; j <= my numberOfColumns; j ++) {
			double x = my data [i] [j];
			fprintf (f, "\t%s", Melder_double (x));
		}
	}
	if (! Melder_fclose (file, f)) return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

TableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file) {
	FILE *f = NULL;
	TableOfReal me = NULL;
	char element [101];
	long nrow, ncol, nelements, irow, icol;

	f = Melder_fopen (file, "rb");
	if (! f) goto end;

	/*
	 * Count number of columns.
	 */
	ncol = 0;
	for (;;) {
		int kar = fgetc (f);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
		if (kar == ' ' || kar == '\t') continue;
		ncol ++;
		do {
			kar = fgetc (f);
		} while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\r' && kar != EOF);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
	}
	ncol --;
	if (ncol < 1) { Melder_error ("No columns."); goto end; }

	/*
	 * Count number of elements.
	 */
	rewind (f);
	nelements = 0;
	for (;;) {
		if (fscanf (f, "%s", & element) < 1) break;   /* Zero or end-of-file. */
		nelements ++;
	}

	/*
	 * Check if all columns are complete.
	 */
	if (! nelements || nelements % (ncol + 1)) {
		Melder_error ("The number of elements (%ld) is not a multiple of the number of columns plus 1 (%ld).",
			nelements, ncol + 1);
		goto end;
	}

	/*
	 * Create empty table.
	 */

	nrow = nelements / (ncol + 1) - 1;
	me = TableOfReal_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */

	rewind (f);
	fscanf (f, "%s", element);   /* Ignore header of column with labels. */
	for (icol = 1; icol <= ncol; icol ++) {
		fscanf (f, "%s", element);
		TableOfReal_setColumnLabel (me, icol, element);
	}
	for (irow = 1; irow <= nrow; irow ++) {
		fscanf (f, "%s", element);
		TableOfReal_setRowLabel (me, irow, element);
		for (icol = 1; icol <= ncol; icol ++) {
			fscanf (f, "%s", element);   /* Because some columns may contain strings. */
			my data [irow] [icol] = atof (element);   /* If cell contains a string, this will be 0. */
		}
	}

end:
	Melder_fclose (file, f);
	iferror { forget (me); return Melder_errorp (
		"(TableOfReal_readFromHeaderlessSpreadsheetFile:) File %s not read.", MelderFile_messageName (file)); }
	return me;
}

/* End of file TableOfReal.c */
