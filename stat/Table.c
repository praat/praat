/* Table.c
 *
 * Copyright (C) 2002-2009 Paul Boersma
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
 * pb 2003/05/19 Melder_atof
 * pb 2004/07/25 Melder_double
 * pb 2004/10/16 struct tags
 * pb 2005/03/04 Melder_NUMBER
 * pb 2005/04/25 Table_getLogisticRegression
 * pb 2005/04/25 Table_to_Matrix
 * pb 2005/06/16 enums -> ints
 * pb 2005/09/13 Table_readFromCharacterSeparatedTextFile
 * pb 2005/09/26 sorting by string now also works
 * pb 2006/01/24 getMatrixStr
 * pb 2006/01/26 Table_extractRowsWhereColumn_string
 * pb 2006/04/16 Table_createWithColumnNames
 * pb 2006/04/16 moved Melder_isStringNumeric to melder_atof.c
 * pb 2006/04/17 getColStr
 * pb 2007/04/19 Table_pool
 * pb 2006/04/24 Table_scatterPlot
 * pb 2006/08/27 Table_drawEllipse
 * pb 2006/10/29 TableOfReal_to_Table
 * pb 2006/11/25 Table_getGroupDifference_studentT
 * pb 2006/12/10 MelderInfo
 * pb 2007/03/17 exported Table_numericize for optimizers
 * pb 2007/05/07 renamed Table_pool to Table_collapseRows
 * pb 2007/05/07 Table_rowsToColumns
 * pb 2007/06/28 Table_getGroupMean_studentT, Table_getGroupMean
 * pb 2007/10/01 can write as encoding
 * pb 2007/10/13 made Table_getExtrema global
 * pb 2007/11/18 refactoring
 * pb 2008/01/02 Table_drawRowFromDistribution
 * pb 2008/04/30 new Formula API
 * pb 2009/01/18 Interpreter argument in formula
 * pb 2009/10/21 Table_randomizeRows
 */

#include <ctype.h>
#include "Table.h"
#include "NUM2.h"
#include "Formula.h"
#include "SSCP.h"

#include "oo_DESTROY.h"
#include "Table_def.h"
#include "oo_COPY.h"
#include "Table_def.h"
#include "oo_EQUAL.h"
#include "Table_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Table_def.h"
#include "oo_WRITE_TEXT.h"
#include "Table_def.h"
#include "oo_WRITE_BINARY.h"
#include "Table_def.h"
#include "oo_READ_TEXT.h"
#include "Table_def.h"
#include "oo_READ_BINARY.h"
#include "Table_def.h"
#include "oo_DESCRIPTION.h"
#include "Table_def.h"

class_methods (TableRow, Data) {
	class_method_local (TableRow, destroy)
	class_method_local (TableRow, description)
	class_method_local (TableRow, copy)
	class_method_local (TableRow, equal)
	class_method_local (TableRow, canWriteAsEncoding)
	class_method_local (TableRow, writeText)
	class_method_local (TableRow, writeBinary)
	class_method_local (TableRow, readText)
	class_method_local (TableRow, readBinary)
	class_methods_end
}

static void info (I) {
	iam (Table);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of rows: ", Melder_integer (my rows -> size));
	MelderInfo_writeLine2 (L"Number of columns: ", Melder_integer (my numberOfColumns));
}

static double getNrow (I) { iam (Table); return my rows -> size; }
static double getNcol (I) { iam (Table); return my numberOfColumns; }
static wchar_t * getColStr (I, long icol) {
	iam (Table);
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	return my columnHeaders [icol]. label ? my columnHeaders [icol]. label : L"";
}
static double getMatrix (I, long irow, long icol) {
	iam (Table);
	wchar_t *stringValue;
	if (irow < 1 || irow > my rows -> size) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	stringValue = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	return stringValue == NULL ? NUMundefined : Melder_atof (stringValue);
}
static wchar_t * getMatrixStr (I, long irow, long icol) {
	iam (Table);
	wchar_t *stringValue;
	if (irow < 1 || irow > my rows -> size) return L"";
	if (icol < 1 || icol > my numberOfColumns) return L"";
	stringValue = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	return stringValue == NULL ? L"" : stringValue;
}
static double getColumnIndex (I, const wchar_t *columnLabel) {
	iam (Table);
	return Table_findColumnIndexFromColumnLabel (me, columnLabel);
}

class_methods (Table, Data) {
	class_method_local (Table, destroy)
	class_method_local (Table, description)
	class_method_local (Table, copy)
	class_method_local (Table, equal)
	class_method_local (Table, canWriteAsEncoding)
	class_method_local (Table, writeText)
	class_method_local (Table, writeBinary)
	class_method_local (Table, readText)
	class_method_local (Table, readBinary)
	class_method (info)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getColStr)
	class_method (getMatrix)
	class_method (getMatrixStr)
	class_method (getColumnIndex)
	class_methods_end
}

static TableRow TableRow_create (long numberOfColumns) {
	TableRow me = new (TableRow); cherror
	my numberOfColumns = numberOfColumns;
	my cells = NUMstructvector (TableCell, 1, numberOfColumns); cherror
end:
	iferror forget (me);
	return me;
}

int Table_initWithoutColumnNames (I, long numberOfRows, long numberOfColumns) {
	iam (Table);
	long irow;
	if (numberOfColumns < 1)
		return Melder_error1 (L"Cannot create table without columns.");
	my numberOfColumns = numberOfColumns;
	if (! (my columnHeaders = NUMstructvector (TableColumnHeader, 1, numberOfColumns))) return 0;
	if (! (my rows = Ordered_create ())) return 0;
	for (irow = 1; irow <= numberOfRows; irow ++) {
		Table_appendRow (me); iferror return 0;
	}
	return 1;
}

Table Table_createWithoutColumnNames (long numberOfRows, long numberOfColumns) {
	Table me = new (Table);
	if (! me || ! Table_initWithoutColumnNames (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

int Table_initWithColumnNames (I, long numberOfRows, const wchar_t *columnNames) {
	iam (Table);
	wchar_t *columnName;
	long icol = 0;
	Table_initWithoutColumnNames (me, numberOfRows, Melder_countTokens (columnNames)); cherror
	for (columnName = Melder_firstToken (columnNames); columnName != NULL; columnName = Melder_nextToken ()) {
		icol ++;
		Table_setColumnLabel (me, icol, columnName); cherror
	}
end:
	iferror return 0;
	return 1;
}

Table Table_createWithColumnNames (long numberOfRows, const wchar_t *columnNames) {
	Table me = new (Table);
	if (! me || ! Table_initWithColumnNames (me, numberOfRows, columnNames)) forget (me);
	return me;
}

int Table_appendRow (Table me) {
	TableRow row = TableRow_create (my numberOfColumns); cherror
	Collection_addItem (my rows, row); cherror
end:
	iferror return 0;
	return 1;
}

int Table_appendColumn (Table me, const wchar_t *label) {
	return Table_insertColumn (me, my numberOfColumns + 1, label);
}

int Table_removeRow (Table me, long irow) {
	if (my rows -> size == 1) error1 (L"Cannot remove the only row.")
	if (irow < 1 || irow > my rows -> size) error3 (L"No row ", Melder_integer (irow), L".")
	Collection_removeItem (my rows, irow);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) my columnHeaders [icol]. numericized = FALSE;
end:
	iferror return Melder_error1 (L"(Table_removeRow:) Not performed.");
	return 1;
}

int Table_removeColumn (Table me, long icol) {
	if (my numberOfColumns == 1) error1 (L"Cannot remove the only column.")
	if (icol < 1 || icol > my numberOfColumns) error3 (L"No column ", Melder_integer (icol), L".")
	Melder_free (my columnHeaders [icol]. label);
	for (long jcol = icol; jcol < my numberOfColumns; jcol ++)
		my columnHeaders [jcol] = my columnHeaders [jcol + 1];
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Melder_free (row -> cells [icol]. string);
		for (long jcol = icol; jcol < row -> numberOfColumns; jcol ++)
			row -> cells [jcol] = row -> cells [jcol + 1];
		row -> numberOfColumns --;
	}
	my numberOfColumns --;
end:
	iferror return Melder_error1 (L"(Table_removeColumn:) Not performed.");
	return 1;
}

int Table_insertRow (Table me, long irow) {
	TableRow row = TableRow_create (my numberOfColumns); cherror
	if (irow < 1 || irow > my rows -> size + 1) error3 (L"Cannot create row ", Melder_integer (irow), L".")
	Ordered_addItemPos (my rows, row, irow);
	for (long icol = 1; icol <= my numberOfColumns; icol ++) my columnHeaders [icol]. numericized = FALSE;
end:
	iferror return Melder_error1 (L"(Table_insertRow:) Not performed.");
	return 1;
}

int Table_insertColumn (Table me, long icol, const wchar_t *label) {
	struct structTableColumnHeader *columnHeaders = NULL;
	if (icol < 1 || icol > my numberOfColumns + 1) error3 (L"Cannot create column ", Melder_integer (icol), L".")
	columnHeaders = NUMstructvector (TableColumnHeader, 1, my numberOfColumns + 1); cherror
	for (long jcol = 1; jcol < icol; jcol ++)
		columnHeaders [jcol] = my columnHeaders [jcol];   /* Shallow copy of strings. */
	for (long jcol = my numberOfColumns + 1; jcol > icol; jcol --)
		columnHeaders [jcol] = my columnHeaders [jcol - 1];   /* Shallow copy of strings. */
	columnHeaders [icol]. label = Melder_wcsdup (label); cherror
	NUMstructvector_free (TableColumnHeader, my columnHeaders, 1);
	my columnHeaders = columnHeaders;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		struct structTableCell *cells = NUMstructvector (TableCell, 1, row -> numberOfColumns + 1); cherror
		for (long jcol = 1; jcol < icol; jcol ++)
			cells [jcol] = row -> cells [jcol];   /* Shallow copy of strings. */
		for (long jcol = row -> numberOfColumns + 1; jcol > icol; jcol --)
			cells [jcol] = row -> cells [jcol - 1];   /* Shallow copy of strings. */
		NUMstructvector_free (TableCell, row -> cells, 1);
		row -> cells = cells;
		row -> numberOfColumns ++;
	}
	my numberOfColumns ++;
end:
	iferror return Melder_error1 (L"(Table_insertColumn:) Not performed.");
	return 1;
}

void Table_setColumnLabel (Table me, long icol, const wchar_t *label) {
	if (icol < 1 || icol > my numberOfColumns || label == my columnHeaders [icol]. label) return;
	Melder_free (my columnHeaders [icol]. label);
	my columnHeaders [icol]. label = Melder_wcsdup (label);
}

long Table_findColumnIndexFromColumnLabel (Table me, const wchar_t *label) {
	for (long icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnHeaders [icol]. label && wcsequ (my columnHeaders [icol]. label, label))
			return icol;
	return 0;
}

long Table_getColumnIndexFromColumnLabel (Table me, const wchar_t *columnLabel) {
	long columnIndex = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	if (columnIndex == 0)
		error4 (Thing_messageName (me), L" does not contain a column named \"", columnLabel, L"\".")
end:
	iferror return 0;
	return columnIndex;
}

long * Table_getColumnIndicesFromColumnLabelString (Table me, const wchar_t *string, long *numberOfTokens) {
	wchar_t **tokens = NULL;
	long *columns = NULL;
	*numberOfTokens = 0;

	tokens = Melder_getTokens (string, numberOfTokens); cherror
	if (*numberOfTokens < 1)
		error1 (L"Empty list of columns.")
	columns = NUMlvector (1, *numberOfTokens); cherror
	for (long icol = 1; icol <= *numberOfTokens; icol ++) {
		columns [icol] = Table_getColumnIndexFromColumnLabel (me, tokens [icol]); cherror
	}
end:
	Melder_freeTokens (& tokens);
	iferror return NULL;
	return columns;
}

long Table_searchColumn (Table me, long icol, const wchar_t *value) {
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. string != NULL && wcsequ (row -> cells [icol]. string, value))
			return irow;
	}
	return 0;
}

int Table_setStringValue (Table me, long irow, long icol, const wchar_t *value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error3 (L"Row number ", Melder_integer (irow), L" out of range.");
	if (icol < 1 || icol > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (icol), L" out of range.");
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_wcsdup (value);
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

int Table_setNumericValue (Table me, long irow, long icol, double value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error3 (L"Row number ", Melder_integer (irow), L" out of range.");
	if (icol < 1 || icol > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (icol), L" out of range.");
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_wcsdup (Melder_double (value));
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

static int Table_isCellNumeric (Table me, long irow, long icol) {
	TableRow row;
	const wchar_t *cell;
	if (irow < 1 || irow > my rows -> size) return FALSE;
	if (icol < 1 || icol > my numberOfColumns) return FALSE;
	row = my rows -> item [irow];
	cell = row -> cells [icol]. string;
	if (cell == NULL) return TRUE;   /* The value --undefined-- */
	/*
	 * Skip leading white space, in order to separately detect "?" and "--undefined--".
	 */
	while (*cell == ' ' || *cell == '\t' || *cell == '\n' || *cell == '\r') cell ++;
	if (cell [0] == '\0') return TRUE;   /* Only white space: the value --undefined-- */
	if (cell [0] == '?' || wcsnequ (cell, L"--undefined--", 13)) {
		/*
		 * See whether there is anything else besides "?" or "--undefined--" and white space.
		 */
		cell += cell [0] == '?' ? 1 : 13;
		while (*cell == ' ' || *cell == '\t' || *cell == '\n' || *cell == '\r') cell ++;
		return *cell == '\0';   /* Only white space after the "?" or "--undefined--". */
	}
	return Melder_isStringNumeric (cell);
}

static int Table_isColumnNumeric (Table me, long icol) {
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return FALSE;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		if (! Table_isCellNumeric (me, irow, icol)) return FALSE;
	}
	return TRUE;
}

static long stringCompare_column;

static int stringCompare (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	wchar_t *firstString = my cells [stringCompare_column]. string;
	wchar_t *secondString = thy cells [stringCompare_column]. string;
	return wcscmp (firstString ? firstString : L"", secondString ? secondString : L"");
}

static void sortRowsByStrings (Table me, long icol) {
	Melder_assert (icol >= 1 && icol <= my numberOfColumns);
	stringCompare_column = icol;
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), stringCompare);
}

static int indexCompare (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	if (my sortingIndex < thy sortingIndex) return -1;
	if (my sortingIndex > thy sortingIndex) return +1;
	return 0;
}

static void sortRowsByIndex (Table me) {
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), indexCompare);
}

void Table_numericize (Table me, long icol) {
	long irow;
	Melder_assert (icol >= 1);
	Melder_assert (icol <= my numberOfColumns);
	if (my columnHeaders [icol]. numericized) return;
	if (Table_isColumnNumeric (me, icol)) {
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			wchar_t *string = row -> cells [icol]. string;
			row -> cells [icol]. number =
				string == NULL || string [0] == '\0' || (string [0] == '?' && string [1] == '\0') ? NUMundefined :
				Melder_atof (string);
		}
	} else {
		long iunique = 0;
		const wchar_t *previousString = NULL;
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			row -> sortingIndex = irow;
		}
		sortRowsByStrings (me, icol);
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			wchar_t *string = row -> cells [icol]. string;
			if (string == NULL) string = L"";
			if (previousString == NULL || ! wcsequ (string, previousString)) {
				iunique ++;
			}
			row -> cells [icol]. number = iunique;
			previousString = string;
		}
		sortRowsByIndex (me);
	}
	my columnHeaders [icol]. numericized = TRUE;
}

static int Table_numericize_checkDefined (Table me, long icol) {
	Table_numericize (me, icol);
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. number == NUMundefined) {
			return Melder_error7 (
				L"The cell in row ", Melder_integer (irow),
				L" of column \"", my columnHeaders [icol]. label ? my columnHeaders [icol]. label : Melder_integer (icol),
				L"\" of Table \"", my name ? my name : L"(noname)", L"\" is undefined.");
		}
	}
	return 1;
}

const wchar_t * Table_getStringValue (Table me, long irow, long icol) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return NULL;
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	row = my rows -> item [irow];
	return row -> cells [icol]. string ? row -> cells [icol]. string : L"";
}

double Table_getNumericValue (Table me, long irow, long icol) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	row = my rows -> item [irow];
	Table_numericize (me, icol); cherror
end:
	iferror return NUMundefined;   /* BUG */
	return row -> cells [icol]. number;
}

double Table_getMean_e (Table me, long icol) {
	double sum = 0.0;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error1 (L"Cannot compute mean.");
		return NUMundefined;
	}
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		sum += row -> cells [icol]. number;
	}
	return sum / my rows -> size;
}

double Table_getMaximum_e (Table me, long icol) {
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error1 (L"Cannot compute maximum.");
		return NUMundefined;
	}
	TableRow firstRow = my rows -> item [1];
	double maximum = firstRow -> cells [icol]. number;
	for (long irow = 2; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. number > maximum)
			maximum = row -> cells [icol]. number;
	}
	return maximum;
}

double Table_getMinimum_e (Table me, long icol) {
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error1 (L"Cannot compute minimum.");
		return NUMundefined;
	}
	TableRow firstRow = my rows -> item [1];
	double minimum = firstRow -> cells [icol]. number;
	for (long irow = 2; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. number < minimum)
			minimum = row -> cells [icol]. number;
	}
	return minimum;
}

double Table_getGroupMean (Table me, long column, long groupColumn, const wchar_t *group) {
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column);
	long n = 0;
	double sum = 0.0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [groupColumn]. string != NULL) {
			if (wcsequ (row -> cells [groupColumn]. string, group)) {
				n += 1;
				sum += row -> cells [column]. number;
			}
		}
	}
	if (n < 1) return NUMundefined;
	double mean = sum / n;
	return mean;
}

double Table_getQuantile_e (Table me, long icol, double quantile) {
	double *sortingColumn = NULL;
	double result = NUMundefined;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error1 (L"Cannot compute quantile.");
		return NUMundefined;
	}
	sortingColumn = NUMdvector (1, my rows -> size); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		sortingColumn [irow] = ((TableRow) my rows -> item [irow]) -> cells [icol]. number;
	}
	NUMsort_d (my rows -> size, sortingColumn);
	result = NUMquantile (my rows -> size, sortingColumn, quantile);
end:
	NUMdvector_free (sortingColumn, 1);
	return result;
}

double Table_getStdev_e (Table me, long icol) {
	double mean = Table_getMean_e (me, icol), sum = 0.0;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 2) return NUMundefined;
	if (mean == NUMundefined) return NUMundefined;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		double d = row -> cells [icol]. number - mean;
		sum += d * d;
	}
	return sqrt (sum / (my rows -> size - 1));
}

long Table_drawRowFromDistribution (Table me, long column) {
	long irow = 0;
	if (column < 1 || column > my numberOfColumns)
		error3 (L"No column ", Melder_integer (column), L".")
	Table_numericize (me, column);
	double total = 0.0;
	if (my rows -> size < 1)
		error1 (L"No candidates.")
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		total += row -> cells [column]. number;
	}
	if (total <= 0.0)
		error1 (L"Column total not positive.")
	do {
		double rand = NUMrandomUniform (0, total), sum = 0.0;
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			sum += row -> cells [column]. number;
			if (rand <= sum) break;
		}
	} while (irow > my rows -> size);   /* Guard against rounding errors. */
end:
	iferror return Melder_error1 (L"(Table_drawRowFromDistribution:) Not performed.");
	return irow;
}

Table Table_extractRowsWhereColumn_number (Table me, long column, int which_Melder_NUMBER, double criterion) {
	Table thee = NULL;
	if (column < 1 || column > my numberOfColumns)
		error3 (L"No column ", Melder_integer (column), L".")
	Table_numericize (me, column);
	thee = Table_create (0, my numberOfColumns); cherror
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnHeaders [icol]. label = Melder_wcsdup (my columnHeaders [icol]. label); cherror
	}
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (Melder_numberMatchesCriterion (row -> cells [column]. number, which_Melder_NUMBER, criterion)) {
			TableRow newRow = Data_copy (row);
			Collection_addItem (thy rows, newRow);
		}
	}
	if (thy rows -> size == 0) {
		Melder_warning1 (L"No row matches criterion.");
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"(Table_selectRowsWhereColumn:) Not performed.");
	}
	return thee;
}

Table Table_extractRowsWhereColumn_string (Table me, long column, int which_Melder_STRING, const wchar_t *criterion) {
	Table thee = NULL;
	if (column < 1 || column > my numberOfColumns)
		error3 (L"No column ", Melder_integer (column), L".")
	Table_numericize (me, column);
	thee = Table_create (0, my numberOfColumns); cherror
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnHeaders [icol]. label = Melder_wcsdup (my columnHeaders [icol]. label); cherror
	}
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (Melder_stringMatchesCriterion (row -> cells [column]. string, which_Melder_STRING, criterion)) {
			TableRow newRow = Data_copy (row);
			Collection_addItem (thy rows, newRow);
		}
	}
	if (thy rows -> size == 0) {
		Melder_warning1 (L"No row matches criterion.");
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"(Table_selectRowsWhereColumn:) Not performed.");
	}
	return thee;
}

static int _Table_columnsExist_check (Table me, wchar_t **columnNames, long n) {
	for (long i = 1; i <= n; i ++) {
		if (Table_findColumnIndexFromColumnLabel (me, columnNames [i]) == 0) {
			return Melder_error3 (L"Column \"", columnNames [i], L"\" does not exist.");
		}
	}
	return TRUE;
}

static int _columns_crossSectionIsEmpty_check (wchar_t **factors, long nfactors, wchar_t **vars, long nvars) {
	for (long ifactor = 1; ifactor <= nfactors; ifactor ++) {
		for (long ivar = 1; ivar <= nvars; ivar ++) {
			if (wcsequ (factors [ifactor], vars [ivar])) {
				return Melder_error3 (L"Factor \"", factors [ifactor], L"\" is also used as dependent variable.");
			}
		}
	}
	return TRUE;
}

Table Table_collapseRows (Table me, const wchar_t *factors_string, const wchar_t *columnsToSum_string,
	const wchar_t *columnsToAverage_string, const wchar_t *columnsToMedianize_string,
	const wchar_t *columnsToAverageLogarithmically_string, const wchar_t *columnsToMedianizeLogarithmically_string)
{
	Table thee = NULL;
	wchar_t **factors = NULL, **columnsToSum = NULL,
		**columnsToAverage = NULL, **columnsToMedianize = NULL,
		**columnsToAverageLogarithmically = NULL, **columnsToMedianizeLogarithmically = NULL;
	long numberOfFactors = 0, numberToSum = 0,
		numberToAverage = 0, numberToMedianize = 0,
		numberToAverageLogarithmically = 0, numberToMedianizeLogarithmically = 0;
	long i, irow, icol, *columns = NULL;
	double *sortingColumn = NULL;
	Melder_assert (factors_string != NULL);
	/*
	 * Remember the present sorting of the original table.
	 * (This is safe: the sorting index may change only vacuously when numericizing.)
	 */
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		row -> sortingIndex = irow;
	}
	/*
	 * Parse the six strings of tokens.
	 */
	factors = Melder_getTokens (factors_string, & numberOfFactors); cherror
	if (numberOfFactors < 1)
		error1 (L"In order to pool table data, you must supply at least one independent variable.")
	_Table_columnsExist_check (me, factors, numberOfFactors); cherror
	if (columnsToSum_string) {
		columnsToSum = Melder_getTokens (columnsToSum_string, & numberToSum); cherror
		_Table_columnsExist_check (me, columnsToSum, numberToSum); cherror
	}
	if (columnsToAverage_string) {
		columnsToAverage = Melder_getTokens (columnsToAverage_string, & numberToAverage); cherror
		_Table_columnsExist_check (me, columnsToAverage, numberToAverage); cherror
	}
	if (columnsToMedianize_string) {
		columnsToMedianize = Melder_getTokens (columnsToMedianize_string, & numberToMedianize); cherror
		_Table_columnsExist_check (me, columnsToMedianize, numberToMedianize); cherror
	}
	if (columnsToAverageLogarithmically_string) {
		columnsToAverageLogarithmically = Melder_getTokens (columnsToAverageLogarithmically_string, & numberToAverageLogarithmically); cherror
		_Table_columnsExist_check (me, columnsToAverageLogarithmically, numberToAverageLogarithmically); cherror
	}
	if (columnsToMedianizeLogarithmically_string) {
		columnsToMedianizeLogarithmically = Melder_getTokens (columnsToMedianizeLogarithmically_string, & numberToMedianizeLogarithmically); cherror
		_Table_columnsExist_check (me, columnsToMedianizeLogarithmically, numberToMedianizeLogarithmically); cherror
	}
	_columns_crossSectionIsEmpty_check (factors, numberOfFactors, columnsToSum, numberToSum); cherror
	_columns_crossSectionIsEmpty_check (factors, numberOfFactors, columnsToAverage, numberToAverage); cherror
	_columns_crossSectionIsEmpty_check (factors, numberOfFactors, columnsToMedianize, numberToMedianize); cherror
	_columns_crossSectionIsEmpty_check (factors, numberOfFactors, columnsToAverageLogarithmically, numberToAverageLogarithmically); cherror
	_columns_crossSectionIsEmpty_check (factors, numberOfFactors, columnsToMedianizeLogarithmically, numberToMedianizeLogarithmically); cherror
	thee = Table_createWithoutColumnNames (0, numberOfFactors + numberToSum + numberToAverage + numberToMedianize + numberToAverageLogarithmically + numberToMedianizeLogarithmically); cherror
	Melder_assert (thy numberOfColumns > 0);
	columns = NUMlvector (1, thy numberOfColumns); cherror
	if (numberToMedianize > 0 || numberToMedianizeLogarithmically > 0) {
		sortingColumn = NUMdvector (1, my rows -> size); cherror
	}
	/*
	 * Set the column names. Within the dependent variables, the same name may occur more than once.
	 */
	icol = 0;
	for (i = 1; i <= numberOfFactors; i ++) {
		Table_setColumnLabel (thee, ++ icol, factors [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, factors [i]);
	}
	for (i = 1; i <= numberToSum; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToSum [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToSum [i]);
	}
	for (i = 1; i <= numberToAverage; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToAverage [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverage [i]);
	}
	for (i = 1; i <= numberToMedianize; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToMedianize [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianize [i]);
	}
	for (i = 1; i <= numberToAverageLogarithmically; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToAverageLogarithmically [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverageLogarithmically [i]);
	}
	for (i = 1; i <= numberToMedianizeLogarithmically; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToMedianizeLogarithmically [i]);
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianizeLogarithmically [i]);
	}
	Melder_assert (icol == thy numberOfColumns);
	/*
	 * Make sure that all the columns in the original table that we will use in the pooled table are defined.
	 */
	for (icol = 1; icol <= thy numberOfColumns; icol ++) {
		Table_numericize_checkDefined (me, columns [icol]); cherror
	}
	/*
	 * We will now sort the original table temporarily, by the factors (independent variables) only.
	 */
	Table_sortRows (me, columns, numberOfFactors);   /* This works only because the factors come first. */
	/*
	 * Find stretches of identical factors.
	 */
	for (irow = 1; irow <= my rows -> size; irow ++) {
		long rowmin = irow, rowmax = irow;
		for (;;) {
			int identical = TRUE;
			if (++ rowmax > my rows -> size) break;
			for (icol = 1; icol <= numberOfFactors; icol ++) {
				if (((TableRow) my rows -> item [rowmax]) -> cells [columns [icol]]. number !=
				    ((TableRow) my rows -> item [rowmin]) -> cells [columns [icol]]. number)
				{
					identical = FALSE;
					break;
				}
			}
			if (! identical) break;
		}
		rowmax --;
		/*
		 * We have the stretch.
		 */
		Table_insertRow (thee, thy rows -> size + 1);
		icol = 0;
		for (i = 1; i <= numberOfFactors; i ++) {
			++ icol;
			Table_setStringValue (thee, thy rows -> size, icol,
				((TableRow) my rows -> item [rowmin]) -> cells [columns [icol]]. string);
		}
		for (i = 1; i <= numberToSum; i ++) {
			double sum = 0.0;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				sum += ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
			}
			Table_setNumericValue (thee, thy rows -> size, icol, sum);
		}
		for (i = 1; i <= numberToAverage; i ++) {
			double sum = 0.0;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				sum += ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
			}
			Table_setNumericValue (thee, thy rows -> size, icol, sum / (rowmax - rowmin + 1));
		}
		for (i = 1; i <= numberToMedianize; i ++) {
			double median;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				sortingColumn [jrow] = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
			}
			NUMsort_d (rowmax - rowmin + 1, sortingColumn + rowmin - 1);
			median = NUMquantile (rowmax - rowmin + 1, sortingColumn + rowmin - 1, 0.5);
			Table_setNumericValue (thee, thy rows -> size, icol, median);
		}
		for (i = 1; i <= numberToAverageLogarithmically; i ++) {
			double sum = 0.0;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				double value = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
				if (value <= 0.0)
					error7 (
						L"The cell in column \"", columnsToAverageLogarithmically [i],
						L"\" of row ", Melder_integer (jrow),
						L" of Table \"", my name ? my name : L"(untitled)",
						L"\" is not positive.\nCannot average logarithmically.")
				sum += log (value);
			}
			Table_setNumericValue (thee, thy rows -> size, icol, exp (sum / (rowmax - rowmin + 1)));
		}
		for (i = 1; i <= numberToMedianizeLogarithmically; i ++) {
			double median;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				double value = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
				if (value <= 0.0)
					error7 (
						L"The cell in column \"", columnsToMedianizeLogarithmically [i],
						L"\" of row ", Melder_integer (jrow),
						L" of Table \"", my name ? my name : L"(untitled)",
						L"\" is not positive.\nCannot medianize logarithmically.")
				sortingColumn [jrow] = log (value);
			}
			NUMsort_d (rowmax - rowmin + 1, sortingColumn + rowmin - 1);
			median = NUMquantile (rowmax - rowmin + 1, sortingColumn + rowmin - 1, 0.5);
			Table_setNumericValue (thee, thy rows -> size, icol, exp (median));
		}
		irow = rowmax;
	}
end:
	sortRowsByIndex (me);   /* Unsort the original table. */
	Melder_freeTokens (& factors);
	Melder_assert (factors == NULL);
	Melder_freeTokens (& columnsToSum);
	Melder_freeTokens (& columnsToAverage);
	Melder_freeTokens (& columnsToMedianize);
	Melder_freeTokens (& columnsToAverageLogarithmically);
	Melder_freeTokens (& columnsToMedianizeLogarithmically);
	NUMlvector_free (columns, 1);
	NUMdvector_free (sortingColumn, 1);
	iferror forget (thee);
	return thee;
}

static wchar_t ** _Table_getLevels (Table me, long column, long *numberOfLevels) {
	wchar_t **result = NULL;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		row -> sortingIndex = irow;
	}
	long columns [2] = { 0, column };
	Table_sortRows (me, columns, 1);
	*numberOfLevels = 0;
	long irow = 1;
	while (irow <= my rows -> size) {
		double value = ((TableRow) my rows -> item [irow]) -> cells [column]. number;
		(*numberOfLevels) ++;
		while (++ irow <= my rows -> size && ((TableRow) my rows -> item [irow]) -> cells [column]. number == value) { }
	}
	result = (wchar_t **) NUMpvector (1, *numberOfLevels); cherror
	*numberOfLevels = 0;
	irow = 1;
	while (irow <= my rows -> size) {
		double value = ((TableRow) my rows -> item [irow]) -> cells [column]. number;
		result [++ *numberOfLevels] = Melder_wcsdup (Table_getStringValue (me, irow, column)); cherror
		while (++ irow <= my rows -> size && ((TableRow) my rows -> item [irow]) -> cells [column]. number == value) { }
	}
	sortRowsByIndex (me);   /* Unsort the original table. */
end:
	iferror { NUMpvector_free ((void **) result, 1), result = NULL; }
	return result;
}

Table Table_rowsToColumns (Table me, const wchar_t *factors_string, long columnToTranspose, const wchar_t *columnsToExpand_string) {
	Table thee = NULL;
	wchar_t **factors_names = NULL, **columnsToExpand_names = NULL, **levels_names = NULL;
	long numberOfFactors = 0, numberToExpand = 0, numberOfLevels;
	long *factorColumns = NULL, *columnsToExpand = NULL;
	double *sortingColumn = NULL;
	MelderString columnLabel = { 0 };
	bool warned = false;
	Melder_assert (factors_string != NULL);
	/*
	 * Remember the present sorting of the original table.
	 * (This is safe: the sorting index may change only vacuously when numericizing.)
	 */
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		row -> sortingIndex = irow;
	}
	/*
	 * Parse the two strings of tokens.
	 */
	factors_names = Melder_getTokens (factors_string, & numberOfFactors); cherror
	if (numberOfFactors < 1)
		error1 (L"In order to nest table data, you must supply at least one independent variable.")
	_Table_columnsExist_check (me, factors_names, numberOfFactors); cherror
	columnsToExpand_names = Melder_getTokens (columnsToExpand_string, & numberToExpand); cherror
	if (numberToExpand < 1)
		error1 (L"In order to nest table data, you must supply at least one dependent variable (to expand).")
	_Table_columnsExist_check (me, columnsToExpand_names, numberToExpand); cherror
	_columns_crossSectionIsEmpty_check (factors_names, numberOfFactors, columnsToExpand_names, numberToExpand); cherror
	levels_names = _Table_getLevels (me, columnToTranspose, & numberOfLevels); cherror
	/*
	 * Get the column numbers for the factors.
	 */
	factorColumns = NUMlvector (1, numberOfFactors); cherror
	for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
		factorColumns [ifactor] = Table_findColumnIndexFromColumnLabel (me, factors_names [ifactor]);
		/*
		 * Make sure that all the columns in the original table that we will use in the nested table are defined.
		 */
		Table_numericize_checkDefined (me, factorColumns [ifactor]); cherror
	}
	/*
	 * Get the column numbers for the expandable variables.
	 */
	columnsToExpand = NUMlvector (1, numberToExpand); cherror
	for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
		columnsToExpand [iexpand] = Table_findColumnIndexFromColumnLabel (me, columnsToExpand_names [iexpand]);
		Table_numericize_checkDefined (me, columnsToExpand [iexpand]); cherror
	}
	/*
	 * Create the new table, with column names.
	 */
	thee = Table_createWithoutColumnNames (0, numberOfFactors + (numberOfLevels * numberToExpand)); cherror
	Melder_assert (thy numberOfColumns > 0);
	for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
		Table_setColumnLabel (thee, ifactor, factors_names [ifactor]);
	}
	for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
		for (long ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
			MelderString_copy (& columnLabel, columnsToExpand_names [iexpand]);
			MelderString_appendCharacter (& columnLabel, '.');
			MelderString_append (& columnLabel, levels_names [ilevel]);
			Table_setColumnLabel (thee, numberOfFactors + (iexpand - 1) * numberOfLevels + ilevel, columnLabel.string);
		}
	}
	/*
	 * We will now sort the original table temporarily, by the factors (independent variables) only.
	 */
	Table_sortRows (me, factorColumns, numberOfFactors);
	/*
	 * Find stretches of identical factors.
	 */
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		long rowmin = irow, rowmax = irow;
		for (;;) {
			int identical = TRUE;
			if (++ rowmax > my rows -> size) break;
			for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
				if (((TableRow) my rows -> item [rowmax]) -> cells [factorColumns [ifactor]]. number !=
				    ((TableRow) my rows -> item [rowmin]) -> cells [factorColumns [ifactor]]. number)
				{
					identical = FALSE;
					break;
				}
			}
			if (! identical) break;
		}
		#if 0
		if (rowmax - rowmin > numberOfLevels && ! warned) {
			Melder_warning1 (L"Some rows of the original table have not been included in the new table. "
				"You could perhaps add more factors.");
			warned = true;
		}
		#endif
		rowmax --;
		/*
		 * We have the stretch.
		 */
		Table_insertRow (thee, thy rows -> size + 1);
		TableRow thyRow = thy rows -> item [thy rows -> size];
		for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
			Table_setStringValue (thee, thy rows -> size, ifactor,
				((TableRow) my rows -> item [rowmin]) -> cells [factorColumns [ifactor]]. string);
		}
		for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
				TableRow myRow = my rows -> item [jrow];
				double value = myRow -> cells [columnsToExpand [iexpand]]. number;
				long level = myRow -> cells [columnToTranspose]. number;
				long thyColumn = numberOfFactors + (iexpand - 1) * numberOfLevels + level;
				if (thyRow -> cells [thyColumn]. string != NULL && ! warned) {
					Melder_warning1 (L"Some information from the original table has not been included in the new table. "
						"You could perhaps add more factors.");
					warned = true;
				}
				Table_setNumericValue (thee, thy rows -> size, thyColumn, value);
			}
		}
		irow = rowmax;
	}
end:
	sortRowsByIndex (me);   /* Unsort the original table. */
	Melder_freeTokens (& factors_names);
	Melder_assert (factors_names == NULL);
	Melder_freeTokens (& columnsToExpand_names);
	MelderString_free (& columnLabel);
	NUMpvector_free ((void **) levels_names, 1);
	NUMlvector_free (factorColumns, 1);
	NUMlvector_free (columnsToExpand, 1);
	NUMdvector_free (sortingColumn, 1);
	iferror forget (thee);
	return thee;
}

static long *cellCompare_columns, cellCompare_numberOfColumns;

static int cellCompare (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	for (long icol = 1; icol <= cellCompare_numberOfColumns; icol ++) {
		if (my cells [cellCompare_columns [icol]]. number < thy cells [cellCompare_columns [icol]]. number) return -1;
		if (my cells [cellCompare_columns [icol]]. number > thy cells [cellCompare_columns [icol]]. number) return +1;
	}
	return 0;
}

void Table_sortRows (Table me, long *columns, long numberOfColumns) {
	for (long icol = 1; icol <= numberOfColumns; icol ++) {
		Melder_assert (columns [icol] >= 1 && columns [icol] <= my numberOfColumns);
		Table_numericize (me, columns [icol]);
	}
	cellCompare_columns = columns;
	cellCompare_numberOfColumns = numberOfColumns;
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), cellCompare);
}

int Table_sortRows_string (Table me, const wchar_t *columns_string) {
	long numberOfColumns, *columns = NULL;
	wchar_t **columns_tokens = Melder_getTokens (columns_string, & numberOfColumns); cherror
	if (numberOfColumns < 1)
		error1 (L"Empty list of columns. Cannot sort.")
	columns = NUMlvector (1, numberOfColumns); cherror
	for (long icol = 1; icol <= numberOfColumns; icol ++) {
		columns [icol] = Table_findColumnIndexFromColumnLabel (me, columns_tokens [icol]);
		if (columns [icol] == 0)
			error3 (L"Column \"", columns_tokens [icol], L"\" does not exist.")
	}
	Table_sortRows (me, columns, numberOfColumns);
end:
	Melder_freeTokens (& columns_tokens);
	NUMlvector_free (columns, 1);
	iferror return 0;
	return 1;
}

void Table_randomizeRows (Table me) {
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		long jrow = NUMrandomInteger (irow, my rows -> size);
		TableRow tmp = my rows -> item [irow];
		my rows -> item [irow] = my rows -> item [jrow];
		my rows -> item [jrow] = tmp;
	}
}

Table Tables_append (Collection me) {
	Table him = NULL, thee;
	if (my size == 0) return Melder_errorp1 (L"Cannot add zero tables.");
	thee = my item [1];
	long nrow = thy rows -> size;
	long ncol = thy numberOfColumns;
	Table firstTable = thee;
	for (long itab = 2; itab <= my size; itab ++) {
		thee = my item [itab];
		nrow += thy rows -> size;
		if (thy numberOfColumns != ncol) error1 (L"Numbers of columns do not match.")
		for (long icol = 1; icol <= ncol; icol ++) {
			if (! Melder_wcsequ (thy columnHeaders [icol]. label, firstTable -> columnHeaders [icol]. label)) {
				error13 (L"The label of column ", Melder_integer (icol), L" of ", Thing_messageName (thee),
					L" (", thy columnHeaders [icol]. label, L") does not match the label of column ", Melder_integer (icol),
					L" of ", Thing_messageName (firstTable), L" (", firstTable -> columnHeaders [icol]. label, L").")
			}
		}
	}
	him = Table_createWithoutColumnNames (nrow, ncol); cherror
	for (long icol = 1; icol <= ncol; icol ++) {
		Table_setColumnLabel (him, icol, thy columnHeaders [icol]. label); cherror
	}
	nrow = 0;
	for (long itab = 1; itab <= my size; itab ++) {
		thee = my item [itab];
		for (long irow = 1; irow <= thy rows -> size; irow ++) {
			nrow ++;
			for (long icol = 1; icol <= ncol; icol ++) {
				Table_setStringValue (him, nrow, icol, Table_getStringValue (thee, irow, icol));
			}
		}
	}
end:
	iferror { forget (him); Melder_error1 (L"Tables not appended."); }
	return him;
}

int Table_appendSumColumn (Table me, long column1, long column2, const wchar_t *label) {
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column1), L" out of range.");
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column2), L" out of range.");
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number + row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendDifferenceColumn (Table me, long column1, long column2, const wchar_t *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column1), L" out of range.");
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column2), L" out of range.");
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number - row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendProductColumn (Table me, long column1, long column2, const wchar_t *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column1), L" out of range.");
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column2), L" out of range.");
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number * row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendQuotientColumn (Table me, long column1, long column2, const wchar_t *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column1), L" out of range.");
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error3 (L"Column number ", Melder_integer (column2), L" out of range.");
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns, row -> cells [column2]. number == 0.0 ? NUMundefined :
			row -> cells [column1]. number / row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_formula_columnRange (Table me, long icol1, long icol2, const wchar_t *expression, Interpreter interpreter) {
	if (icol1 < 1 || icol1 > my numberOfColumns) return Melder_error3 (L"No column ", Melder_integer (icol1), L".");
	if (icol2 < 1 || icol2 > my numberOfColumns) return Melder_error3 (L"No column ", Melder_integer (icol2), L".");
	Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, TRUE); cherror
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		for (long icol = icol1; icol <= icol2; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING) {
				Table_setStringValue (me, irow, icol, result. result.stringResult);
				Melder_free (result. result.stringResult);
			} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
				Table_setNumericValue (me, irow, icol, result. result.numericResult);
			} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_ARRAY) {
				error1 (L"Table_formula_columnRange: cannot put arrays into cells.")
			} else if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING_ARRAY) {
				error1 (L"Table_formula_columnRange: cannot put arrays into cells.")
			}
		}
	}
end:
	iferror return 0;
	return 1;
}

int Table_formula (Table me, long icol, const wchar_t *expression, Interpreter interpreter) {
	return Table_formula_columnRange (me, icol, icol, expression, interpreter);
}

double Table_getCorrelation_pearsonR (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	long n = my rows -> size, irow;
	double correlation;
	double sum1 = 0.0, sum2 = 0.0, sum12 = 0.0, sum11 = 0.0, sum22 = 0.0, mean1, mean2;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	if (n < 2) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		sum1 += row -> cells [column1]. number;
		sum2 += row -> cells [column2]. number;
	}
	mean1 = sum1 / n;
	mean2 = sum2 / n;
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		double d1 = row -> cells [column1]. number - mean1, d2 = row -> cells [column2]. number - mean2;
		sum12 += d1 * d2;
		sum11 += d1 * d1;
		sum22 += d2 * d2;
	}
	correlation = sum11 == 0.0 || sum22 == 0.0 ? NUMundefined : sum12 / sqrt (sum11 * sum22);
	if (out_significance && NUMdefined (correlation) && n >= 3)
		*out_significance = fabs (correlation) == 1.0 ? 0.0 :
			/* One-sided: */
			NUMstudentQ (fabs (correlation) * sqrt ((n - 2) / (1 - correlation * correlation)), n - 2);
	if ((out_lowerLimit || out_upperLimit) && NUMdefined (correlation) && n >= 4) {
		if (fabs (correlation) == 1.0) {
			if (out_lowerLimit) *out_lowerLimit = correlation;
			if (out_upperLimit) *out_upperLimit = correlation;
		} else {
			double z = 0.5 * log ((1.0 + correlation) / (1.0 - correlation));
			double dz = NUMinvGaussQ (significanceLevel) / sqrt (n - 3);
			if (out_lowerLimit)
				*out_lowerLimit = tanh (z - dz);
			if (out_upperLimit)
				*out_upperLimit = tanh (z + dz);
		}
	}
	return correlation;
}

double Table_getCorrelation_kendallTau (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	long n = my rows -> size, irow, jrow;
	double correlation, denominator;
	long numberOfConcordants = 0, numberOfDiscordants = 0, numberOfExtra1 = 0, numberOfExtra2 = 0;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow < n; irow ++) {
		TableRow rowi = my rows -> item [irow];
		for (jrow = irow + 1; jrow <= n; jrow ++) {
			TableRow rowj = my rows -> item [jrow];
			double diff1 = rowi -> cells [column1]. number - rowj -> cells [column1]. number;
			double diff2 = rowi -> cells [column2]. number - rowj -> cells [column2]. number;
			double concord = diff1 * diff2;
			if (concord > 0.0) {
				numberOfConcordants ++;
			} else if (concord < 0.0) {
				numberOfDiscordants ++;
			} else if (diff1 != 0.0) {
				numberOfExtra1 ++;
			} else {
				numberOfExtra2 ++;
			}
		}
	}
	denominator = sqrt ((numberOfConcordants + numberOfDiscordants + numberOfExtra1) *
		(numberOfConcordants + numberOfDiscordants + numberOfExtra2));
	correlation = denominator == 0.0 ? NUMundefined : (numberOfConcordants - numberOfDiscordants) / denominator;
	if ((out_significance || out_lowerLimit || out_upperLimit) && NUMdefined (correlation) && n >= 2) {
		double standardError = sqrt ((4 * n + 10.0) / (9 * n * (n - 1)));
		if (out_significance)
			*out_significance = NUMgaussQ (fabs (correlation) / standardError);   /* One-sided. */
		if (out_lowerLimit)
			*out_lowerLimit = correlation - NUMinvGaussQ (significanceLevel) * standardError;
		if (out_upperLimit)
			*out_upperLimit = correlation + NUMinvGaussQ (significanceLevel) * standardError;
	}
	return correlation;
}

double Table_getDifference_studentT (Table me, long column1, long column2, double significanceLevel,
	double *out_t, double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_t) *out_t = NUMundefined;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	long n = my rows -> size;
	if (n < 1) return NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	double sum = 0.0;
	for (long irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		sum += row -> cells [column1]. number - row -> cells [column2]. number;
	}
	double meanDifference = sum / n;
	long degreesOfFreedom = n - 1;
	if (degreesOfFreedom >= 1 && (out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= n; irow ++) {
			TableRow row = my rows -> item [irow];
			double diff = (row -> cells [column1]. number - row -> cells [column2]. number) - meanDifference;
			sumOfSquares += diff * diff;
		}
		double standardError = sqrt (sumOfSquares / degreesOfFreedom / n);
		if (out_t && standardError != 0.0 ) *out_t = meanDifference / standardError;
		if (out_significance) *out_significance =
			standardError == 0.0 ? 0.0 : NUMstudentQ (fabs (meanDifference) / standardError, degreesOfFreedom);
		if (out_lowerLimit) *out_lowerLimit =
			meanDifference - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit) *out_upperLimit =
			meanDifference + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return meanDifference;
}

double Table_getMean_studentT (Table me, long column, double significanceLevel,
	double *out_tFromZero, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	double mean = 0.0, var = 0.0, standardError;
	long n = my rows -> size, irow;
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (n < 1) return NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		mean += row -> cells [column]. number;
	}
	mean /= n;
	if (n >= 2 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		for (irow = 1; irow <= n; irow ++) {
			TableRow row = my rows -> item [irow];
			double diff = row -> cells [column]. number - mean;
			var += diff * diff;
		}
		standardError = sqrt (var / (n - 1) / n);
		if (out_tFromZero && standardError != 0.0 ) *out_tFromZero = mean / standardError;
		if (out_significanceFromZero) *out_significanceFromZero =
			standardError == 0.0 ? 0.0 : NUMstudentQ (fabs (mean) / standardError, n - 1);
		if (out_lowerLimit) *out_lowerLimit =
			mean - standardError * NUMinvStudentQ (significanceLevel, n - 1);
		if (out_upperLimit) *out_upperLimit =
			mean + standardError * NUMinvStudentQ (significanceLevel, n - 1);
	}
	return mean;
}

double Table_getGroupMean_studentT (Table me, long column, long groupColumn, const wchar_t *group, double significanceLevel,
	double *out_tFromZero, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column);
	long n = 0;
	double sum = 0.0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [groupColumn]. string != NULL) {
			if (wcsequ (row -> cells [groupColumn]. string, group)) {
				n += 1;
				sum += row -> cells [column]. number;
			}
		}
	}
	if (n < 1) return NUMundefined;
	double mean = sum / n;
	long degreesOfFreedom = n - 1;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			if (row -> cells [groupColumn]. string != NULL) {
				if (wcsequ (row -> cells [groupColumn]. string, group)) {
					double diff = row -> cells [column]. number - mean;
					sumOfSquares += diff * diff;
				}
			}
		}
		double standardError = sqrt (sumOfSquares / degreesOfFreedom / n);
		if (out_tFromZero && standardError != 0.0 ) *out_tFromZero = mean / standardError;
		if (out_significanceFromZero) *out_significanceFromZero =
			standardError == 0.0 ? 0.0 : NUMstudentQ (fabs (mean) / standardError, degreesOfFreedom);
		if (out_lowerLimit) *out_lowerLimit =
			mean - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit) *out_upperLimit =
			mean + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return mean;
}

double Table_getGroupDifference_studentT (Table me, long column, long groupColumn, const wchar_t *group1, const wchar_t *group2, double significanceLevel,
	double *out_tFromZero, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column);
	long n1 = 0, n2 = 0;
	double sum1 = 0.0, sum2 = 0.0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [groupColumn]. string != NULL) {
			if (wcsequ (row -> cells [groupColumn]. string, group1)) {
				n1 ++;
				sum1 += row -> cells [column]. number;
			} else if (wcsequ (row -> cells [groupColumn]. string, group2)) {
				n2 ++;
				sum2 += row -> cells [column]. number;
			}
		}
	}
	if (n1 < 1 || n2 < 1) return NUMundefined;
	double mean1 = sum1 / n1;
	double mean2 = sum2 / n2;
	double difference = mean1 - mean2;
	long degreesOfFreedom = n1 + n2 - 2;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			if (row -> cells [groupColumn]. string != NULL) {
				if (wcsequ (row -> cells [groupColumn]. string, group1)) {
					double diff = row -> cells [column]. number - mean1;
					sumOfSquares += diff * diff;
				} else if (wcsequ (row -> cells [groupColumn]. string, group2)) {
					double diff = row -> cells [column]. number - mean2;
					sumOfSquares += diff * diff;
				}
			}
		}
		double standardError = sqrt (sumOfSquares / degreesOfFreedom * (1.0 / n1 + 1.0 / n2));
		if (out_tFromZero && standardError != 0.0 ) *out_tFromZero = difference / standardError;
		if (out_significanceFromZero) *out_significanceFromZero =
			standardError == 0.0 ? 0.0 : NUMstudentQ (fabs (difference) / standardError, degreesOfFreedom);
		if (out_lowerLimit) *out_lowerLimit =
			difference - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit) *out_upperLimit =
			difference + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return difference;
}

double Table_getFisherF (Table me, long col1, long col2);
double Table_getOneWayAnovaSignificance (Table me, long col1, long col2);
double Table_getFisherFLowerLimit (Table me, long col1, long col2, double significanceLevel);
double Table_getFisherFUpperLimit (Table me, long col1, long col2, double significanceLevel);

bool Table_getExtrema (Table me, long icol, double *minimum, double *maximum) {
	long n = my rows -> size, irow;
	if (icol < 1 || icol > my numberOfColumns || n == 0) {
		*minimum = *maximum = NUMundefined;
		return false;
	}
	Table_numericize (me, icol);
	*minimum = *maximum = ((TableRow) my rows -> item [1]) -> cells [icol]. number;
	for (irow = 2; irow <= n; irow ++) {
		double value = ((TableRow) my rows -> item [irow]) -> cells [icol]. number;
		if (value < *minimum) *minimum = value;
		if (value > *maximum) *maximum = value;
	}
	return true;
}

void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const wchar_t *mark, int garnish)
{
	long n = my rows -> size, irow;
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
	Table_numericize (me, xcolumn);
	Table_numericize (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax)) return;
		if (xmin == xmax) xmin -= 0.5, xmax += 0.5;
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax)) return;
		if (ymin == ymax) ymin -= 0.5, ymax += 0.5;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		Graphics_mark (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, markSize_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, TRUE, my columnHeaders [xcolumn]. label);
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, TRUE, my columnHeaders [ycolumn]. label);
	}
}

void Table_scatterPlot (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long markColumn, int fontSize, int garnish)
{
	long n = my rows -> size, irow;
	int saveFontSize = Graphics_inqFontSize (g);
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
	Table_numericize (me, xcolumn);
	Table_numericize (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax)) return;
		if (xmin == xmax) xmin -= 0.5, xmax += 0.5;
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax)) return;
		if (ymin == ymax) ymin -= 0.5, ymax += 0.5;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, fontSize);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		const wchar_t *mark = row -> cells [markColumn]. string;
		if (mark)
			Graphics_text (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, mark);
	}
	Graphics_setFontSize (g, saveFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, TRUE, my columnHeaders [xcolumn]. label);
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, TRUE, my columnHeaders [ycolumn]. label);
	}
}

void Table_drawEllipse (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int garnish)
{
	TableOfReal tableOfReal = NULL;
	SSCP sscp = NULL;
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
	Table_numericize (me, xcolumn);
	Table_numericize (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax)) return;
		if (xmin == xmax) xmin -= 0.5, xmax += 0.5;
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax)) return;
		if (ymin == ymax) ymin -= 0.5, ymax += 0.5;
	}
	tableOfReal = Table_to_TableOfReal (me, 0); cherror
	sscp = TableOfReal_to_SSCP (tableOfReal, 0, 0, 0, 0); cherror
	SSCP_drawConcentrationEllipse (sscp, g, numberOfSigmas, 0,
		xcolumn, ycolumn, xmin, xmax, ymin, ymax, garnish);
end:
	forget (tableOfReal);
	forget (sscp);
	iferror Melder_clearError ();
}

static const wchar_t *visibleString (const wchar_t *s) {
	return s != NULL && s [0] != '\0' ? s : L"?";
}

void Table_list (Table me, bool includeRowNumbers) {
	MelderInfo_open ();
	if (includeRowNumbers) {
		MelderInfo_write1 (L"row");
		if (my numberOfColumns > 0) MelderInfo_write1 (L"\t");
	}
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol > 1) MelderInfo_write1 (L"\t");
		MelderInfo_write1 (visibleString (my columnHeaders [icol]. label));
	}
	MelderInfo_write1 (L"\n");
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		if (includeRowNumbers) {
			MelderInfo_write1 (Melder_integer (irow));
			if (my numberOfColumns > 0) MelderInfo_write1 (L"\t");
		}
		TableRow row = my rows -> item [irow];
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol > 1) MelderInfo_write1 (L"\t");
			MelderInfo_write1 (visibleString (row -> cells [icol]. string));
		}
		MelderInfo_write1 (L"\n");
	}
	MelderInfo_close ();
}

int Table_writeToTableFile (Table me, MelderFile file) {
	MelderString buffer = { 0 };
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol != 1) MelderString_appendCharacter (& buffer, '\t');
		wchar_t *s = my columnHeaders [icol]. label;
		MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
	}
	MelderString_appendCharacter (& buffer, '\n');
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol != 1) MelderString_appendCharacter (& buffer, '\t');
			wchar_t *s = row -> cells [icol]. string;
			MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	MelderFile_writeText (file, buffer.string);
end:
	MelderString_free (& buffer);
	iferror return 0;
	return 1;
}

Table Table_readFromTableFile (MelderFile file) {
	Table me = NULL;
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
	if (nelements == 0 || nelements % ncol != 0)
		error5 (L"The number of elements (", Melder_integer (nelements), L") is not a multiple of the number of columns (", Melder_integer (ncol), L").")

	/*
	 * Create empty table.
	 */
	nrow = nelements / ncol - 1;
	me = Table_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */
	p = & string [0];
	for (long icol = 1; icol <= ncol; icol ++) {
		while (*p == ' ' || *p == '\t') { Melder_assert (*p != '\0'); p ++; }
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		while (*p != ' ' && *p != '\t' && *p != '\n') { MelderString_appendCharacter (& buffer, *p); p ++; }
		Table_setColumnLabel (me, icol, buffer.string);
		MelderString_empty (& buffer);
	}
	for (long irow = 1; irow <= nrow; irow ++) {
		TableRow row = my rows -> item [irow];
		for (long icol = 1; icol <= ncol; icol ++) {
			while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') { MelderString_appendCharacter (& buffer, *p); p ++; }
			row -> cells [icol]. string = Melder_wcsdup (buffer.string);
			MelderString_empty (& buffer);
		}
	}

end:
	iferror {
		forget (me);
		Melder_error3 (L"(Table_readFromTableFile:) File ", MelderFile_messageName (file), L" not read.");
		return NULL;
	}
	return me;
}

Table Table_readFromCharacterSeparatedTextFile (MelderFile file, wchar_t separator) {
	Table me = NULL;
	wchar_t *string = MelderFile_readText (file); cherror

	/*
	 * Kill final new-line symbols.
	 */
	for (long length = wcslen (string); length > 0 && string [length - 1] == '\n'; length = wcslen (string)) string [length - 1] = '\0';

	/*
	 * Count columns.
	 */
	long ncol = 1;
	wchar_t *p = & string [0];
	for (;;) {
		wchar_t kar = *p++;
		if (kar == '\0') error1 (L"No rows.")
		if (kar == '\n') break;
		if (kar == separator) ncol ++;
	}

	/*
	 * Count rows.
	 */
	long nrow = 1;
	for (;;) {
		wchar_t kar = *p++;
		if (kar == '\0') break;
		if (kar == '\n') nrow ++;
	}

	/*
	 * Create empty table.
	 */
	me = Table_create (nrow, ncol); cherror

	/*
	 * Read column names.
	 */
	p = & string [0];
	for (long icol = 1; icol <= ncol; icol ++) {
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		while (*p != separator && *p != '\n') {
			Melder_assert (*p != '\0');
			MelderString_appendCharacter (& buffer, *p);
			p ++;
		}
		p ++;
		Table_setColumnLabel (me, icol, buffer.string);
		MelderString_empty (& buffer);
	}

	/*
	 * Read cells.
	 */
	for (long irow = 1; irow <= nrow; irow ++) {
		TableRow row = my rows -> item [irow];
		for (long icol = 1; icol <= ncol; icol ++) {
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			while (*p != separator && *p != '\n' && *p != '\0') {
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			if (*p == '\0') {
				if (irow != nrow) Melder_fatal ("irow %ld, nrow %ld, icol %ld, ncol %ld", irow, nrow, icol, ncol);
				if (icol != ncol) error1 (L"Last row incomplete.")
			} else if (*p == '\n') {
				if (icol != ncol) error3 (L"Row ", Melder_integer (irow), L" incomplete.")
				p ++;
			} else {
				Melder_assert (*p == separator);
				p ++;
			}
			row -> cells [icol]. string = Melder_wcsdup (buffer.string);
			MelderString_empty (& buffer);
		}
	}

end:
	iferror {
		forget (me);
		Melder_error3 (L"(Table_readFromCharacterSeparatedTextFile:) File ", MelderFile_messageName (file), L" not read.");
		return NULL;
	}
	return me;
}

/* End of file Table.c */
