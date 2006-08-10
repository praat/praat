/* Table.c
 *
 * Copyright (C) 2002-2006 Paul Boersma
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
 * pb 2006/04/24 Table_scatterPlot ()
 */

#include <ctype.h>
#include "Table.h"
#include "NUM2.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "Table_def.h"
#include "oo_COPY.h"
#include "Table_def.h"
#include "oo_EQUAL.h"
#include "Table_def.h"
#include "oo_WRITE_ASCII.h"
#include "Table_def.h"
#include "oo_WRITE_BINARY.h"
#include "Table_def.h"
#include "oo_READ_ASCII.h"
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
	class_method_local (TableRow, writeAscii)
	class_method_local (TableRow, writeBinary)
	class_method_local (TableRow, readAscii)
	class_method_local (TableRow, readBinary)
class_methods_end }

static void info (I) {
	iam (Table);
	Melder_information ("Number of rows = %ld\nNumber of columns = %ld",
		my rows -> size, my numberOfColumns);
}

static double getNrow (I) { iam (Table); return my rows -> size; }
static double getNcol (I) { iam (Table); return my numberOfColumns; }
static char * getColStr (I, long icol) {
	iam (Table);
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	return my columnHeaders [icol]. label ? my columnHeaders [icol]. label : "";
}
static double getMatrix (I, long irow, long icol) {
	iam (Table);
	char *stringValue;
	if (irow < 1 || irow > my rows -> size) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	stringValue = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	return stringValue == NULL ? NUMundefined : Melder_atof (stringValue);
}
static char * getMatrixStr (I, long irow, long icol) {
	iam (Table);
	char *stringValue;
	if (irow < 1 || irow > my rows -> size) return "";
	if (icol < 1 || icol > my numberOfColumns) return "";
	stringValue = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	return stringValue == NULL ? "" : stringValue;
}
static double getColumnIndex (I, const char *columnLabel) {
	iam (Table);
	return Table_columnLabelToIndex (me, columnLabel);
}

class_methods (Table, Data) {
	class_method_local (Table, destroy)
	class_method_local (Table, description)
	class_method_local (Table, copy)
	class_method_local (Table, equal)
	class_method_local (Table, writeAscii)
	class_method_local (Table, writeBinary)
	class_method_local (Table, readAscii)
	class_method_local (Table, readBinary)
	class_method (info)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getColStr)
	class_method (getMatrix)
	class_method (getMatrixStr)
	class_method (getColumnIndex)
class_methods_end }

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
		return Melder_error ("Cannot create table without columns.");
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

int Table_initWithColumnNames (I, long numberOfRows, const char *columnNames) {
	iam (Table);
	char *columnName;
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

Table Table_createWithColumnNames (long numberOfRows, const char *columnNames) {
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

static char ** Melder_getTokens (const char *string, long *n) {
	char **result = NULL, *token;
	long itoken = 0;
	*n = Melder_countTokens (string);
	if (*n == 0) return NULL;
	result = (char **) NUMpvector (1, *n); cherror
	for (token = Melder_firstToken (string); token != NULL; token = Melder_nextToken ()) {
		result [++ itoken] = Melder_strdup (token); cherror
	}
end:
	iferror NUMpvector_free ((void **) result, 1);
	return result;
}

static void Melder_freeTokens (char ***tokens) {
	NUMpvector_free ((void **) *tokens, 1);
	*tokens = NULL;
}

static long Melder_searchToken (const char *string, char **tokens, long n) {
	long i;
	for (i = 1; i <= n; i ++) {
		if (strequ (string, tokens [i])) return i;
	}
	return 0;
}

int Table_appendColumn (Table me, const char *label) {
	return Table_insertColumn (me, my numberOfColumns + 1, label);
}

int Table_removeRow (Table me, long irow) {
	long icol;
	if (my rows -> size == 1)
		{ Melder_error ("Cannot remove the only row."); goto end; }
	if (irow < 1 || irow > my rows -> size)
		{ Melder_error ("No row %ld.", irow); goto end; }
	Collection_removeItem (my rows, irow);
	for (icol = 1; icol <= my numberOfColumns; icol ++) my columnHeaders [icol]. numericized = FALSE;
end:
	iferror return Melder_error ("(Table_removeRow:) Not performed.");
	return 1;
}

int Table_removeColumn (Table me, long icol) {
	long irow, jcol;
	if (my numberOfColumns == 1)
		{ Melder_error ("Cannot remove the only column."); goto end; }
	if (icol < 1 || icol > my numberOfColumns)
		{ Melder_error ("No column %ld.", icol); goto end; }
	Melder_free (my columnHeaders [icol]. label);
	for (jcol = icol; jcol < my numberOfColumns; jcol ++)
		my columnHeaders [jcol] = my columnHeaders [jcol + 1];
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Melder_free (row -> cells [icol]. string);
		for (jcol = icol; jcol < row -> numberOfColumns; jcol ++)
			row -> cells [jcol] = row -> cells [jcol + 1];
		row -> numberOfColumns --;
	}
	my numberOfColumns --;
end:
	iferror return Melder_error ("(Table_removeColumn:) Not performed.");
	return 1;
}

int Table_insertRow (Table me, long irow) {
	long icol;
	TableRow row = TableRow_create (my numberOfColumns); cherror
	if (irow < 1 || irow > my rows -> size + 1)
		{ Melder_error ("Cannot create row %ld.", irow); goto end; }
	Ordered_addItemPos (my rows, row, irow);
	for (icol = 1; icol <= my numberOfColumns; icol ++) my columnHeaders [icol]. numericized = FALSE;
end:
	iferror return Melder_error ("(Table_insertRow:) Not performed.");
	return 1;
}

int Table_insertColumn (Table me, long icol, const char *label) {
	struct structTableColumnHeader *columnHeaders = NULL;
	long irow, jcol;
	if (icol < 1 || icol > my numberOfColumns + 1)
		{ Melder_error ("Cannot create column %ld.", icol); goto end; }
	columnHeaders = NUMstructvector (TableColumnHeader, 1, my numberOfColumns + 1); cherror
	for (jcol = 1; jcol < icol; jcol ++)
		columnHeaders [jcol] = my columnHeaders [jcol];   /* Shallow copy of strings. */
	for (jcol = my numberOfColumns + 1; jcol > icol; jcol --)
		columnHeaders [jcol] = my columnHeaders [jcol - 1];   /* Shallow copy of strings. */
	columnHeaders [icol]. label = Melder_strdup (label); cherror
	NUMstructvector_free (TableColumnHeader, my columnHeaders, 1);
	my columnHeaders = columnHeaders;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		struct structTableCell *cells = NUMstructvector (TableCell, 1, row -> numberOfColumns + 1); cherror
		for (jcol = 1; jcol < icol; jcol ++)
			cells [jcol] = row -> cells [jcol];   /* Shallow copy of strings. */
		for (jcol = row -> numberOfColumns + 1; jcol > icol; jcol --)
			cells [jcol] = row -> cells [jcol - 1];   /* Shallow copy of strings. */
		NUMstructvector_free (TableCell, row -> cells, 1);
		row -> cells = cells;
		row -> numberOfColumns ++;
	}
	my numberOfColumns ++;
end:
	iferror return Melder_error ("(Table_insertColumn:) Not performed.");
	return 1;
}

void Table_setColumnLabel (Table me, long icol, const char *label) {
	if (icol < 1 || icol > my numberOfColumns || label == my columnHeaders [icol]. label) return;
	Melder_free (my columnHeaders [icol]. label);
	my columnHeaders [icol]. label = Melder_strdup (label);
}

long Table_columnLabelToIndex (Table me, const char *label) {
	long icol;
	for (icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnHeaders [icol]. label && strequ (my columnHeaders [icol]. label, label))
			return icol;
	return 0;
}

long Table_searchColumn (Table me, long icol, const char *value) {
	long irow;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. string != NULL && strequ (row -> cells [icol]. string, value))
			return irow;
	}
	return 0;
}

int Table_setStringValue (Table me, long irow, long icol, const char *value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error ("Row number %ld out of range.", irow);
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("Column number %ld out of range.", icol);
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_strdup (value);
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

int Table_setNumericValue (Table me, long irow, long icol, double value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error ("Row number %ld out of range.", irow);
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("Column number %ld out of range.", icol);
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_strdup (Melder_double (value));
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

static int Table_isCellNumeric (Table me, long irow, long icol) {
	TableRow row;
	const char *cell;
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
	if (cell [0] == '?' || strnequ (cell, "--undefined--", 13)) {
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
	char *firstString = my cells [stringCompare_column]. string;
	char *secondString = thy cells [stringCompare_column]. string;
	return strcmp (firstString ? firstString : "", secondString ? secondString : "");
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

static void Table_numericize (Table me, long icol) {
	long irow;
	Melder_assert (icol >= 1);
	Melder_assert (icol <= my numberOfColumns);
	if (my columnHeaders [icol]. numericized) return;
	if (Table_isColumnNumeric (me, icol)) {
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			char *string = row -> cells [icol]. string;
			row -> cells [icol]. number =
				string == NULL || string [0] == '\0' || (string [0] == '?' && string [1] == '\0') ? NUMundefined :
				Melder_atof (string);
		}
	} else {
		long iunique = 0;
		const char *previousString = NULL;
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			row -> sortingIndex = irow;
		}
		sortRowsByStrings (me, icol);
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			char *string = row -> cells [icol]. string;
			if (string == NULL) string = "";
			if (previousString == NULL || ! strequ (string, previousString)) {
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
	long irow;
	Table_numericize (me, icol);
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. number == NUMundefined) {
			return Melder_error ("The cell in row %ld of column \"%s\" of Table \"%s\" is undefined.",
				irow,
				my columnHeaders [icol]. label ? my columnHeaders [icol]. label : Melder_integer (icol),
				my name ? my name : "(noname)");
		}
	}
	return 1;
}

const char * Table_getStringValue (Table me, long irow, long icol) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return NULL;
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	row = my rows -> item [irow];
	return row -> cells [icol]. string ? row -> cells [icol]. string : "";
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

double Table_getMean (Table me, long icol) {
	double sum = 0.0;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error ("Cannot compute mean.");
		return NUMundefined;
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		sum += row -> cells [icol]. number;
	}
	return sum / my rows -> size;
}

double Table_getQuantile (Table me, long icol, double quantile) {
	double *sortingColumn = NULL;
	double result = NUMundefined;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize_checkDefined (me, icol)) {
		Melder_error ("Cannot compute quantile.");
		return NUMundefined;
	}
	sortingColumn = NUMdvector (1, my rows -> size); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		sortingColumn [irow] = ((TableRow) my rows -> item [irow]) -> cells [icol]. number;
	}
	NUMsort_d (my rows -> size, sortingColumn);
	result = NUMquantile_d (my rows -> size, sortingColumn, quantile);
end:
	NUMdvector_free (sortingColumn, 1);
	return result;
}

double Table_getStdev (Table me, long icol) {
	double mean = Table_getMean (me, icol), sum = 0.0;
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

Table Table_extractRowsWhereColumn_number (Table me, long column, int which_Melder_NUMBER, double criterion) {
	Table thee = NULL;
	long irow, icol;
	if (column < 1 || column > my numberOfColumns)
		{ Melder_error ("No column %ld.", column); goto end; }
	Table_numericize (me, column);
	thee = Table_create (0, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnHeaders [icol]. label = Melder_strdup (my columnHeaders [icol]. label); cherror
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (Melder_numberMatchesCriterion (row -> cells [column]. number, which_Melder_NUMBER, criterion)) {
			TableRow newRow = Data_copy (row);
			Collection_addItem (thy rows, newRow);
		}
	}
	if (thy rows -> size == 0) {
		Melder_warning ("No row matches criterion.");
	}
end:
	iferror {
		forget (thee);
		Melder_error ("(Table_selectRowsWhereColumn:) Not performed.");
	}
	return thee;
}

Table Table_extractRowsWhereColumn_string (Table me, long column, int which_Melder_STRING, const char *criterion) {
	Table thee = NULL;
	long irow, icol;
	if (column < 1 || column > my numberOfColumns)
		{ Melder_error ("No column %ld.", column); goto end; }
	Table_numericize (me, column);
	thee = Table_create (0, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnHeaders [icol]. label = Melder_strdup (my columnHeaders [icol]. label); cherror
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (Melder_stringMatchesCriterion (row -> cells [column]. string, which_Melder_STRING, criterion)) {
			TableRow newRow = Data_copy (row);
			Collection_addItem (thy rows, newRow);
		}
	}
	if (thy rows -> size == 0) {
		Melder_warning ("No row matches criterion.");
	}
end:
	iferror {
		forget (thee);
		Melder_error ("(Table_selectRowsWhereColumn:) Not performed.");
	}
	return thee;
}

static int _Table_columnsExist_check (Table me, char **columnNames, long n) {
	long i;
	for (i = 1; i <= n; i ++) {
		if (Table_columnLabelToIndex (me, columnNames [i]) == 0) {
			return Melder_error ("Column \"%s\" does not exist.", columnNames [i]);
		}
	}
	return TRUE;
}

static int _columns_crossSectionIsEmpty_check (char **factors, long nfactors, char **vars, long nvars) {
	long ifactor, ivar;
	for (ifactor = 1; ifactor <= nfactors; ifactor ++) {
		for (ivar = 1; ivar <= nvars; ivar ++) {
			if (strequ (factors [ifactor], vars [ivar])) {
				return Melder_error ("Factor \"%s\" is also used as dependent variable.", factors [ifactor]);
			}
		}
	}
	return TRUE;
}

Table Table_pool (Table me, const char *factors_string, const char *columnsToSum_string,
	const char *columnsToAverage_string, const char *columnsToMedianize_string,
	const char *columnsToAverageLogarithmically_string, const char *columnsToMedianizeLogarithmically_string)
{
	Table thee = NULL;
	char **factors = NULL, **columnsToSum = NULL,
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
	if (numberOfFactors < 1) {
		Melder_error ("In order to pool table data, you must supply at least one independent variable.");
		goto end;
	}
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
		columns [icol] = Table_columnLabelToIndex (me, factors [i]);
	}
	for (i = 1; i <= numberToSum; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToSum [i]);
		columns [icol] = Table_columnLabelToIndex (me, columnsToSum [i]);
	}
	for (i = 1; i <= numberToAverage; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToAverage [i]);
		columns [icol] = Table_columnLabelToIndex (me, columnsToAverage [i]);
	}
	for (i = 1; i <= numberToMedianize; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToMedianize [i]);
		columns [icol] = Table_columnLabelToIndex (me, columnsToMedianize [i]);
	}
	for (i = 1; i <= numberToAverageLogarithmically; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToAverageLogarithmically [i]);
		columns [icol] = Table_columnLabelToIndex (me, columnsToAverageLogarithmically [i]);
	}
	for (i = 1; i <= numberToMedianizeLogarithmically; i ++) {
		Table_setColumnLabel (thee, ++ icol, columnsToMedianizeLogarithmically [i]);
		columns [icol] = Table_columnLabelToIndex (me, columnsToMedianizeLogarithmically [i]);
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
			median = NUMquantile_d (rowmax - rowmin + 1, sortingColumn + rowmin - 1, 0.5);
			Table_setNumericValue (thee, thy rows -> size, icol, median);
		}
		for (i = 1; i <= numberToAverageLogarithmically; i ++) {
			double sum = 0.0;
			long jrow;
			++ icol;
			for (jrow = rowmin; jrow <= rowmax; jrow ++) {
				double value = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
				if (value <= 0.0) {
					Melder_error ("The cell in column \"%s\" of row %ld of Table \"%s\" is not positive.\n"
						"Cannot average logarithmically.", columnsToAverageLogarithmically [i], jrow,
						my name ? my name : "(untitled)");
				}
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
				if (value <= 0.0) {
					Melder_error ("The cell in column \"%s\" of row %ld of Table \"%s\" is not positive.\n"
						"Cannot medianize logarithmically.", columnsToMedianizeLogarithmically [i], jrow,
						my name ? my name : "(untitled)");
				}
				sortingColumn [jrow] = log (value);
			}
			NUMsort_d (rowmax - rowmin + 1, sortingColumn + rowmin - 1);
			median = NUMquantile_d (rowmax - rowmin + 1, sortingColumn + rowmin - 1, 0.5);
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

static long *cellCompare_columns, cellCompare_numberOfColumns;

static int cellCompare (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	long icol;
	for (icol = 1; icol <= cellCompare_numberOfColumns; icol ++) {
		if (my cells [cellCompare_columns [icol]]. number < thy cells [cellCompare_columns [icol]]. number) return -1;
		if (my cells [cellCompare_columns [icol]]. number > thy cells [cellCompare_columns [icol]]. number) return +1;
	}
	return 0;
}

void Table_sortRows (Table me, long *columns, long numberOfColumns) {
	long icol;
	for (icol = 1; icol <= numberOfColumns; icol ++) {
		Melder_assert (columns [icol] >= 1 && columns [icol] <= my numberOfColumns);
		Table_numericize (me, columns [icol]);
	}
	cellCompare_columns = columns;
	cellCompare_numberOfColumns = numberOfColumns;
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), cellCompare);
}

int Table_sortRows_string (Table me, const char *columns_string) {
	long numberOfColumns, icol, *columns = NULL;
	char **columns_tokens = Melder_getTokens (columns_string, & numberOfColumns); cherror
	if (numberOfColumns < 1) {
		Melder_error ("Empty list of columns. Cannot sort.");
		goto end;
	}
	columns = NUMlvector (1, numberOfColumns); cherror
	for (icol = 1; icol <= numberOfColumns; icol ++) {
		columns [icol] = Table_columnLabelToIndex (me, columns_tokens [icol]);
		if (columns [icol] == 0) {
			Melder_error ("Column \"%s\" does not exist.", columns_tokens [icol]);
			goto end;
		}
	}
	Table_sortRows (me, columns, numberOfColumns);
end:
	Melder_freeTokens (& columns_tokens);
	NUMlvector_free (columns, 1);
	iferror return 0;
	return 1;
}

int Table_appendSumColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number + row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendDifferenceColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
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

int Table_appendProductColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
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

int Table_appendQuotientColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
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

int Table_formula (Table me, long icol, const char *expression) {
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("No column %ld.", icol);
	if (! Formula_compile (NULL, me, expression, 2, TRUE)) goto end;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		double numericResult;
		char *stringResult = NULL;
		if (! Formula_run (irow, icol, & numericResult, & stringResult)) goto end;
		if (stringResult) {
			Table_setStringValue (me, irow, icol, stringResult);
			Melder_free (stringResult);
		} else {
			Table_setNumericValue (me, irow, icol, numericResult);
		}
	}
end:
	iferror return 0;
	return 1;
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
	double mean1 = 0.0, mean2 = 0.0, var1 = 0.0, var2 = 0.0, covar = 0.0, standardError;
	double difference;
	long n = my rows -> size, irow;
	if (out_t) *out_t = NUMundefined;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (n < 2) return NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		mean1 += row -> cells [column1]. number;
		mean2 += row -> cells [column2]. number;
	}
	mean1 /= n;
	mean2 /= n;
	difference = mean1 - mean2;
	if ((out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		for (irow = 1; irow <= n; irow ++) {
			TableRow row = my rows -> item [irow];
			double diff1 = row -> cells [column1]. number - mean1, diff2 = row -> cells [column2]. number - mean2;
			var1 += diff1 * diff1;
			var2 += diff2 * diff2;
			covar += diff1 * diff2;
		}
		standardError = sqrt ((var1 + var2 - 2.0 * covar) / (n - 1) / n);
		if (out_t && standardError != 0.0 ) *out_t = difference / standardError;
		if (out_significance) *out_significance = standardError == 0.0 ? 0.0 : NUMstudentQ (difference / standardError, n - 1);
		if (out_lowerLimit) *out_lowerLimit = difference - standardError * NUMinvStudentQ (significanceLevel, n - 1);
		if (out_upperLimit) *out_upperLimit = difference + standardError * NUMinvStudentQ (significanceLevel, n - 1);
	}
	return difference;
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
		if (out_significanceFromZero) *out_significanceFromZero = standardError == 0.0 ? 0.0 : NUMstudentQ (mean / standardError, n - 1);
		if (out_lowerLimit) *out_lowerLimit = mean - standardError * NUMinvStudentQ (significanceLevel, n - 1);
		if (out_upperLimit) *out_upperLimit = mean + standardError * NUMinvStudentQ (significanceLevel, n - 1);
	}
	return mean;
}

Matrix Table_to_Matrix (Table me) {
	long irow, icol;
	Matrix thee = Matrix_createSimple (my rows -> size, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		Table_numericize (me, icol);
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			thy z [irow] [icol] = (float) row -> cells [icol]. number;
		}
	}
end:
	iferror return NULL;
	return thee;
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
			char *string = row -> cells [labelColumn]. string;
			TableOfReal_setRowLabel (thee, irow, string ? string : "");
			for (icol = 1; icol < labelColumn; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;
			}
			for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol - 1] = row -> cells [icol]. number;
			}
		}
	} else {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			for (icol = 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;
			}
		}
	}
end:
	iferror return NULL;
	return thee;
}

/*
Table TableOfReal_to_Table (TableOfReal me, char *) {
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
			char *string = row -> cells [labelColumn]. string;
			TableOfReal_setRowLabel (thee, irow, string ? string : "");
			for (icol = 1; icol < labelColumn; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;
			}
			for (icol = labelColumn + 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol - 1] = row -> cells [icol]. number;
			}
		}
	} else {
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			TableOfReal_setColumnLabel (thee, icol, my columnHeaders [icol]. label);
		}
		for (irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = my rows -> item [irow];
			for (icol = 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol] = row -> cells [icol]. number;
			}
		}
	}
end:
	iferror return NULL;
	return thee;
}
*/

double Table_getFisherF (Table me, long col1, long col2);
double Table_getOneWayAnovaSignificance (Table me, long col1, long col2);
double Table_getFisherFLowerLimit (Table me, long col1, long col2, double significanceLevel);
double Table_getFisherFUpperLimit (Table me, long col1, long col2, double significanceLevel);

static int Table_getExtrema (Table me, long icol, double *minimum, double *maximum) {
	long n = my rows -> size, irow;
	if (icol < 1 || icol > my numberOfColumns || n == 0) {
		*minimum = *maximum = NUMundefined;
		return 0;
	}
	Table_numericize (me, icol);
	*minimum = *maximum = ((TableRow) my rows -> item [1]) -> cells [icol]. number;
	for (irow = 2; irow <= n; irow ++) {
		double value = ((TableRow) my rows -> item [irow]) -> cells [icol]. number;
		if (value < *minimum) *minimum = value;
		if (value > *maximum) *maximum = value;
	}
	return 1;
}

void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const char *mark, int garnish)
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
		const char *mark = row -> cells [markColumn]. string;
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

int Table_writeToTableFile (Table me, MelderFile file) {
	FILE *f = Melder_fopen (file, "w");
	long i, j;
	if (! f) return 0;
	for (i = 1; i <= my numberOfColumns; i ++) {
		char *s = my columnHeaders [i]. label;
		fprintf (f, i == 1 ? "%s" : "\t%s", s != NULL && s [0] != '\0' ? s : "?");
	}
	fprintf (f, "\n");
	for (i = 1; i <= my rows -> size; i ++) {
		TableRow row = my rows -> item [i];
		for (j = 1; j <= my numberOfColumns; j ++) {
			char *s = row -> cells [j]. string;
			fprintf (f, j == 1 ? "%s" : "\t%s", s != NULL && s [0] != '\0' ? s : "?");
		}
		fprintf (f, "\n");
	}
	if (! Melder_fclose (file, f)) return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

Table Table_readFromTableFile (MelderFile file) {
	Table me = NULL;
	long nrow, ncol, nelements, irow, icol;
	FILE *f = Melder_fopen (file, "rb"); cherror

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
	if (ncol < 1) { Melder_error ("No columns."); goto end; }

	/*
	 * Count number of elements.
	 */
	rewind (f);
	nelements = 0;
	for (;;) {
		if (fscanf (f, "%s", Melder_buffer1) < 1) break;   /* Zero or end-of-file. */
		nelements ++;
	}

	/*
	 * Check if all columns are complete.
	 */
	if (nelements == 0 || nelements % ncol != 0) {
		Melder_error ("The number of elements (%ld) is not a multiple of the number of columns (%ld).",
			nelements, ncol);
		goto end;
	}

	/*
	 * Create empty table.
	 */

	nrow = nelements / ncol - 1;
	me = Table_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */

	rewind (f);
	for (icol = 1; icol <= ncol; icol ++) {
		fscanf (f, "%s", Melder_buffer1);
		Table_setColumnLabel (me, icol, Melder_buffer1);
	}
	for (irow = 1; irow <= nrow; irow ++) {
		TableRow row = my rows -> item [irow];
		for (icol = 1; icol <= ncol; icol ++) {
			fscanf (f, "%s", Melder_buffer1);
			row -> cells [icol]. string = Melder_strdup (Melder_buffer1);
		}
	}

end:
	Melder_fclose (file, f);
	iferror { forget (me); return Melder_errorp (
		"(Table_readFromTableFile:) File %s not read.", MelderFile_messageName (file)); }
	return me;
}

static long MelderString_countCharacters (const char *string, char character) {
	long n = 0;
	for (; *string != '\0'; string ++) {
		if (*string == character) {
			n ++;
		}
	}
	return n;
}

static char * MelderString_getSinglySeparatedToken (char **string, char separator) {
	char *originalString = *string, *p = originalString;
	if (*p == '\0') return NULL;
	while (*p != separator && *p != '\0') p ++;
	if (*p == '\0') {
		*string = p;
	} else {
		*p = '\0';
		*string = p + 1;
	}
	return originalString;
}

Table Table_readFromCharacterSeparatedTextFile (MelderFile file, char separator) {
	Table me = NULL;
	long nrow, ncol, irow, icol;
	char *line;
	MelderFile_open (file); cherror

	/*
	 * Count number of columns.
	 */
	line = MelderFile_readLine (file); cherror
	if (line == NULL) {
		Melder_error ("Empty file.");
		goto end;
	}
	ncol = MelderString_countCharacters (line, separator) + 1;

	/*
	 * Count and check rows.
	 */
	for (nrow = 0;; nrow ++) {
		line = MelderFile_readLine (file); cherror
		if (line == NULL) break;
		if (MelderString_countCharacters (line, separator) + 1 != ncol) {
			Melder_error ("Row %ld has wrong number of columns.", nrow + 1);
		}
	}
	if (nrow < 1) { Melder_error ("No rows."); goto end; }

	/*
	 * Create empty table.
	 */
	me = Table_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read column names.
	 */
	MelderFile_rewind (file);
	line = MelderFile_readLine (file); cherror
	for (icol = 1; icol <= ncol; icol ++) {
		char *token = MelderString_getSinglySeparatedToken (& line, separator);
		Table_setColumnLabel (me, icol, token);
	}

	/*
	 * Read cells.
	 */
	for (irow = 1; irow <= nrow; irow ++) {
		TableRow row = my rows -> item [irow];
		line = MelderFile_readLine (file); cherror
		for (icol = 1; icol <= ncol; icol ++) {
			char *token = MelderString_getSinglySeparatedToken (& line, separator);
			row -> cells [icol]. string = Melder_strdup (token);
		}
	}

end:
	MelderFile_close (file);
	iferror { forget (me); return Melder_errorp (
		"(Table_readFromCharacterSeparatedTextFile:) File %s not read.", MelderFile_messageName (file)); }
	return me;
}

/* End of file Table.c */
