/* Table.cpp
 *
 * Copyright (C) 2002-2012 Paul Boersma
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
 * pb 2010/03/04 Wilcoxon rank sum
 * pb 2010/06/23 report number of degrees of freedom in t-tests
 * pb 2011/03/15 C++
 * pb 2011/04/15 C++
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

Thing_implement (TableRow, Data, 0);

Thing_implement (Table, Data, 0);

void structTable :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (rows -> size));
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (numberOfColumns));
}

const wchar_t * structTable :: v_getColStr (long columnNumber) {
	if (columnNumber < 1 || columnNumber > numberOfColumns) return NULL;
	return columnHeaders [columnNumber]. label ? columnHeaders [columnNumber]. label : L"";
}

double structTable :: v_getMatrix (long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > rows -> size) return NUMundefined;
	if (columnNumber < 1 || columnNumber > numberOfColumns) return NUMundefined;
	wchar_t *stringValue = ((TableRow) rows -> item [rowNumber]) -> cells [columnNumber]. string;
	return stringValue == NULL ? NUMundefined : Melder_atof (stringValue);
}

const wchar_t * structTable :: v_getMatrixStr (long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > rows -> size) return L"";
	if (columnNumber < 1 || columnNumber > numberOfColumns) return L"";
	wchar_t *stringValue = ((TableRow) rows -> item [rowNumber]) -> cells [columnNumber]. string;
	return stringValue == NULL ? L"" : stringValue;
}

double structTable :: v_getColIndex (const wchar_t *columnLabel) {
	return Table_findColumnIndexFromColumnLabel (this, columnLabel);
}

static TableRow TableRow_create (long numberOfColumns) {
	autoTableRow me = Thing_new (TableRow);
	my numberOfColumns = numberOfColumns;
	my cells = NUMvector <structTableCell> (1, numberOfColumns);
	return me.transfer();
}

void Table_initWithoutColumnNames (I, long numberOfRows, long numberOfColumns) {
	iam (Table);
	if (numberOfColumns < 1)
		Melder_throw ("Cannot create table without columns.");
	my numberOfColumns = numberOfColumns;
	my columnHeaders = NUMvector <structTableColumnHeader> (1, numberOfColumns);
	my rows = Ordered_create ();
	for (long irow = 1; irow <= numberOfRows; irow ++) {
		Table_appendRow (me);
	}
}

Table Table_createWithoutColumnNames (long numberOfRows, long numberOfColumns) {
	try {
		autoTable me = Thing_new (Table);
		Table_initWithoutColumnNames (me.peek(), numberOfRows, numberOfColumns);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Table not created.");
	}
}

void Table_initWithColumnNames (I, long numberOfRows, const wchar_t *columnNames) {
	iam (Table);
	Table_initWithoutColumnNames (me, numberOfRows, Melder_countTokens (columnNames));
	long icol = 0;
	for (wchar_t *columnName = Melder_firstToken (columnNames); columnName != NULL; columnName = Melder_nextToken ()) {
		icol ++;
		Table_setColumnLabel (me, icol, columnName);
	}
}

Table Table_createWithColumnNames (long numberOfRows, const wchar_t *columnNames) {
	try {
		autoTable me = Thing_new (Table);
		Table_initWithColumnNames (me.peek(), numberOfRows, columnNames);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Table not created.");
	}
}

void Table_appendRow (Table me) {
	try {
		autoTableRow row = TableRow_create (my numberOfColumns);
		Collection_addItem (my rows, row.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": row not appended.");
	}
}

void Table_appendColumn (Table me, const wchar_t *label) {
	try {
		Table_insertColumn (me, my numberOfColumns + 1, label);
	} catch (MelderError) {
		Melder_throw (me, ": column \"", label, "\" not appended.");
	}
}

void Table_checkSpecifiedRowNumberWithinRange (Table me, long rowNumber) {
	if (rowNumber < 1)
		Melder_throw (me, ": the specified row number is ", rowNumber, ", but should be at least 1.");
	if (rowNumber > my rows -> size)
		Melder_throw (me, ": the specified row number (", rowNumber, ") exceeds my number of rows (", my rows -> size, ").");	
}

void Table_removeRow (Table me, long rowNumber) {
	try {
		if (my rows -> size == 1)
			Melder_throw (me, ": cannot remove my only row.");
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Collection_removeItem (my rows, rowNumber);
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = FALSE;
	} catch (MelderError) {
		Melder_throw (me, ": row ", rowNumber, " not removed.");
	}
}

void Table_checkSpecifiedColumnNumberWithinRange (Table me, long columnNumber) {
	if (columnNumber < 1)
		Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at least 1.");
	if (columnNumber > my numberOfColumns)
		Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at most my number of columns (", my numberOfColumns, ").");	
}

void Table_removeColumn (Table me, long columnNumber) {
	try {
		if (my numberOfColumns == 1)
			Melder_throw (me, ": cannot remove my only column.");
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		/*
		 * Changes without error.
		 */
		Melder_free (my columnHeaders [columnNumber]. label);
		for (long icol = columnNumber; icol < my numberOfColumns; icol ++)
			my columnHeaders [icol] = my columnHeaders [icol + 1];
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			Melder_free (row -> cells [columnNumber]. string);
			for (long icol = columnNumber; icol < row -> numberOfColumns; icol ++)
				row -> cells [icol] = row -> cells [icol + 1];
			row -> numberOfColumns --;
		}
		my numberOfColumns --;
	} catch (MelderError) {
		Melder_throw (me, ": column ", columnNumber, " not removed.");
	}
}

void Table_insertRow (Table me, long rowNumber) {
	try {
		/*
		 * Check without changes.
		 */
		if (rowNumber < 1)
			Melder_throw (me, ": the specified row number is ", rowNumber, ", but should be at least 1.");
		if (rowNumber > my rows -> size + 1)
			Melder_throw (me, ": the specified row number is ", rowNumber, ", but should be at most my number of rows (", my rows -> size, ") plus 1.");	
		autoTableRow row = TableRow_create (my numberOfColumns);
		/*
		 * Safe change.
		 */
		Ordered_addItemPos (my rows, row.transfer(), rowNumber);
		/*
		 * Changes without error.
		 */
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = FALSE;
	} catch (MelderError) {
		Melder_throw (me, ": row ", rowNumber, " not inserted.");
	}
}

void Table_insertColumn (Table me, long columnNumber, const wchar_t *label) {
	try {
		/*
		 * Check without changes.
		 */
		if (columnNumber < 1)
			Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at least 1.");
		if (columnNumber > my numberOfColumns + 1)
			Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at most my number of columns (", my numberOfColumns, ") plus 1.");	
		autoTable thee = Table_createWithoutColumnNames (my rows -> size, my numberOfColumns + 1);
		autostring newLabel = Melder_wcsdup (label);
		/*
		 * Changes without error.
		 */
		/*
		 * Transfer column headers to larger structure.
		 */
		for (long icol = 1; icol < columnNumber; icol ++) {
			Melder_assert (thy columnHeaders [icol]. label == NULL);   // make room...
			thy columnHeaders [icol] = my columnHeaders [icol];   // ...fill in and dangle...
			my columnHeaders [icol]. label = NULL;   // ...undangle
		}
		thy columnHeaders [columnNumber]. label = newLabel.transfer();
		thy columnHeaders [columnNumber]. numericized = false;
		for (long icol = my numberOfColumns + 1; icol > columnNumber; icol --) {
			Melder_assert (thy columnHeaders [icol]. label == NULL);   // make room...
			thy columnHeaders [icol] = my columnHeaders [icol - 1];   // ...fill in and dangle...
			my columnHeaders [icol - 1]. label = NULL;   // ...undangle
		}
		/*
		 * Transfer rows to larger structure.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]), thyRow = static_cast <TableRow> (thy rows -> item [irow]);
			for (long icol = 1; icol < columnNumber; icol ++) {
				Melder_assert (thyRow -> cells [icol]. string == NULL);   // make room...
				thyRow -> cells [icol] = myRow -> cells [icol];   // ...fill in and dangle...
				myRow -> cells [icol]. string = NULL;   // ...undangle
			}
			Melder_assert (thyRow -> cells [columnNumber]. string == NULL);
			Melder_assert (thyRow -> cells [columnNumber]. number == 0.0);
			for (long icol = myRow -> numberOfColumns + 1; icol > columnNumber; icol --) {
				Melder_assert (thyRow -> cells [icol]. string == NULL);   // make room...
				thyRow -> cells [icol] = myRow -> cells [icol - 1];   // ...fill in and dangle...
				myRow -> cells [icol - 1]. string = NULL;   // ...undangle
			}
		}
		/*
		 * Transfer larger structure with column headers to me.
		 */
		NUMvector_free <structTableColumnHeader> (my columnHeaders, 1);   // make room...
		my columnHeaders = thy columnHeaders;   // ...fill in and dangle...
		thy columnHeaders = NULL;   // ...undangle
		/*
		 * Transfer larger structure with rows to me.
		 */
		forget (my rows);   // make room...
		my rows = thy rows;   // ...fill in and dangle...
		thy rows = NULL;   // ...undangle
		/*
		 * Update my state.
		 */
		my numberOfColumns ++;
	} catch (MelderError) {
		Melder_throw (me, ": column not inserted.");
	}
}

void Table_setColumnLabel (Table me, long columnNumber, const wchar_t *label) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring newLabel = Melder_wcsdup (label);
		/*
		 * Changes without error.
		 */
		Melder_free (my columnHeaders [columnNumber]. label);
		my columnHeaders [columnNumber]. label = newLabel.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": column label not set.");
	}
}

long Table_findColumnIndexFromColumnLabel (Table me, const wchar_t *label) {
	for (long icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnHeaders [icol]. label && wcsequ (my columnHeaders [icol]. label, label))
			return icol;
	return 0;
}

long Table_getColumnIndexFromColumnLabel (Table me, const wchar_t *columnLabel) {
	long columnNumber = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	if (columnNumber == 0)
		Melder_throw (me, ": there is no column named \"", columnLabel, "\".");
	return columnNumber;
}

long * Table_getColumnIndicesFromColumnLabelString (Table me, const wchar_t *string, long *numberOfTokens) {
	*numberOfTokens = 0;
	autoMelderTokens tokens (string, numberOfTokens);
	if (*numberOfTokens < 1)
		Melder_throw (me, ": you specified an empty list of columns.");
	autoNUMvector <long> columns (1, *numberOfTokens);
	for (long icol = 1; icol <= *numberOfTokens; icol ++) {
		columns [icol] = Table_getColumnIndexFromColumnLabel (me, tokens [icol]);
	}
	return columns.transfer();
}

long Table_searchColumn (Table me, long columnNumber, const wchar_t *value) {
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		if (row -> cells [columnNumber]. string != NULL && wcsequ (row -> cells [columnNumber]. string, value))
			return irow;
	}
	return 0;
}

void Table_setStringValue (Table me, long rowNumber, long columnNumber, const wchar_t *value) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring newValue = Melder_wcsdup (value);
		/*
		 * Change without errors.
		 */
		TableRow row = static_cast <TableRow> (my rows -> item [rowNumber]);
		Melder_free (row -> cells [columnNumber]. string);
		row -> cells [columnNumber]. string = newValue.transfer();
		my columnHeaders [columnNumber]. numericized = FALSE;
	} catch (MelderError) {
		Melder_throw (me, ": string value not set.");
	}
}

void Table_setNumericValue (Table me, long rowNumber, long columnNumber, double value) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring newValue = Melder_wcsdup (Melder_double (value));
		/*
		 * Change without errors.
		 */
		TableRow row = static_cast <TableRow> (my rows -> item [rowNumber]);
		Melder_free (row -> cells [columnNumber]. string);
		row -> cells [columnNumber]. string = newValue.transfer();
		my columnHeaders [columnNumber]. numericized = FALSE;
	} catch (MelderError) {
		Melder_throw (me, ": numeric value not set.");
	}
}

bool Table_isCellNumeric_ErrorFalse (Table me, long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > my rows -> size) return false;
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return false;
	TableRow row = static_cast <TableRow> (my rows -> item [rowNumber]);
	const wchar_t *cell = row -> cells [columnNumber]. string;
	if (cell == NULL) return true;   // the value --undefined--
	/*
	 * Skip leading white space, in order to separately detect "?" and "--undefined--".
	 */
	while (*cell == ' ' || *cell == '\t' || *cell == '\n' || *cell == '\r') cell ++;
	if (cell [0] == '\0') return true;   // only white space: the value --undefined--
	if (cell [0] == '?' || wcsnequ (cell, L"--undefined--", 13)) {
		/*
		 * See whether there is anything else besides "?" or "--undefined--" and white space.
		 */
		cell += ( cell [0] == '?' ) ? 1 : 13;
		while (*cell == ' ' || *cell == '\t' || *cell == '\n' || *cell == '\r') cell ++;
		return *cell == '\0';   // only white space after the "?" or "--undefined--"
	}
	return Melder_isStringNumeric_nothrow (cell);
}

bool Table_isColumnNumeric_ErrorFalse (Table me, long columnNumber) {
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return false;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		if (! Table_isCellNumeric_ErrorFalse (me, irow, columnNumber)) return false;
	}
	return true;
}

static long stringCompare_column;

static int stringCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	wchar_t *firstString = my cells [stringCompare_column]. string;
	wchar_t *secondString = thy cells [stringCompare_column]. string;
	return wcscmp (firstString ? firstString : L"", secondString ? secondString : L"");
}

static void sortRowsByStrings_Assert (Table me, long columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	stringCompare_column = columnNumber;
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), stringCompare_NoError);
}

static int indexCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	if (my sortingIndex < thy sortingIndex) return -1;
	if (my sortingIndex > thy sortingIndex) return +1;
	return 0;
}

static void sortRowsByIndex_NoError (Table me) {
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), indexCompare_NoError);
}

void Table_numericize_Assert (Table me, long columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	if (my columnHeaders [columnNumber]. numericized) return;
	if (Table_isColumnNumeric_ErrorFalse (me, columnNumber)) {
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			const wchar_t *string = row -> cells [columnNumber]. string;
			row -> cells [columnNumber]. number =
				string == NULL || string [0] == '\0' || (string [0] == '?' && string [1] == '\0') ? NUMundefined :
				Melder_atof (string);
		}
	} else {
		long iunique = 0;
		const wchar_t *previousString = NULL;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			row -> sortingIndex = irow;
		}
		sortRowsByStrings_Assert (me, columnNumber);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			const wchar_t *string = row -> cells [columnNumber]. string;
			if (string == NULL) string = L"";
			if (previousString == NULL || ! wcsequ (string, previousString)) {
				iunique ++;
			}
			row -> cells [columnNumber]. number = iunique;
			previousString = string;
		}
		sortRowsByIndex_NoError (me);
	}
	my columnHeaders [columnNumber]. numericized = TRUE;
}

static void Table_numericize_checkDefined (Table me, long columnNumber) {
	Table_numericize_Assert (me, columnNumber);
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		if (row -> cells [columnNumber]. number == NUMundefined)
			Melder_throw (me, ": the cell in row ", irow,
				" of column \"", my columnHeaders [columnNumber]. label ? my columnHeaders [columnNumber]. label : Melder_integer (columnNumber),
				" is undefined.");
	}
}

const wchar_t * Table_getStringValue_Assert (Table me, long rowNumber, long columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows -> size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	TableRow row = static_cast <TableRow> (my rows -> item [rowNumber]);
	return row -> cells [columnNumber]. string ? row -> cells [columnNumber]. string : L"";
}

double Table_getNumericValue_Assert (Table me, long rowNumber, long columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows -> size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	TableRow row = static_cast <TableRow> (my rows -> item [rowNumber]);
	Table_numericize_Assert (me, columnNumber);
	return row -> cells [columnNumber]. number;
}

double Table_getMean (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows -> size < 1)
			return NUMundefined;
		double sum = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			sum += row -> cells [columnNumber]. number;
		}
		return sum / my rows -> size;
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute mean of column ", columnNumber, ".");
	}
}

double Table_getMaximum (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows -> size < 1)
			return NUMundefined;
		TableRow firstRow = static_cast <TableRow> (my rows -> item [1]);
		double maximum = firstRow -> cells [columnNumber]. number;
		for (long irow = 2; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			if (row -> cells [columnNumber]. number > maximum)
				maximum = row -> cells [columnNumber]. number;
		}
		return maximum;
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute maximum of column ", columnNumber, ".");
	}
}

double Table_getMinimum (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows -> size < 1)
			return NUMundefined;
		TableRow firstRow = static_cast <TableRow> (my rows -> item [1]);
		double minimum = firstRow -> cells [columnNumber]. number;
		for (long irow = 2; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			if (row -> cells [columnNumber]. number < minimum)
				minimum = row -> cells [columnNumber]. number;
		}
		return minimum;
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute minimum of column ", columnNumber, ".");
	}
}

double Table_getGroupMean (Table me, long columnNumber, long groupColumnNumber, const wchar_t *group) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		long n = 0;
		double sum = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			if (Melder_wcsequ (row -> cells [groupColumnNumber]. string, group)) {
				n += 1;
				sum += row -> cells [columnNumber]. number;
			}
		}
		if (n < 1) return NUMundefined;
		double mean = sum / n;
		return mean;
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute mean of column ", columnNumber, " for group \"", group, "\" of column ", groupColumnNumber, ".");
	}
}

double Table_getQuantile (Table me, long columnNumber, double quantile) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows -> size < 1)
			return NUMundefined;
		autoNUMvector <double> sortingColumn (1, my rows -> size);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			sortingColumn [irow] = row -> cells [columnNumber]. number;
		}
		NUMsort_d (my rows -> size, sortingColumn.peek());
		return NUMquantile (my rows -> size, sortingColumn.peek(), quantile);
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute the ", quantile, " quantile of column ", columnNumber, ".");
	}
}

double Table_getStdev (Table me, long columnNumber) {
	try {
		double mean = Table_getMean (me, columnNumber);   // already checks for columnNumber and undefined cells
		if (my rows -> size < 2)
			return NUMundefined;
		double sum = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			double d = row -> cells [columnNumber]. number - mean;
			sum += d * d;
		}
		return sqrt (sum / (my rows -> size - 1));
	} catch (MelderError) {
		Melder_throw (me, ": cannot compute the standard deviation of column ", columnNumber, ".");
	}
}

long Table_drawRowFromDistribution (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows -> size < 1)
			Melder_throw (me, ": no rows.");
		double total = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			total += row -> cells [columnNumber]. number;
		}
		if (total <= 0.0)
			Melder_throw (me, ": the total weight of column ", columnNumber, " is not positive.");
		long irow;
		do {
			double rand = NUMrandomUniform (0, total), sum = 0.0;
			for (irow = 1; irow <= my rows -> size; irow ++) {
				TableRow row = static_cast <TableRow> (my rows -> item [irow]);
				sum += row -> cells [columnNumber]. number;
				if (rand <= sum) break;
			}
		} while (irow > my rows -> size);   /* Guard against rounding errors. */
		return irow;
	} catch (MelderError) {
		Melder_throw (me, ": cannot draw a row from the distribution of column ", columnNumber, ".");
	}
}

Table Table_extractRowsWhereColumn_number (Table me, long columnNumber, int which_Melder_NUMBER, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_Assert (me, columnNumber);   // extraction should work even if cells are not defined
		autoTable thee = Table_create (0, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			thy columnHeaders [icol]. label = Melder_wcsdup (my columnHeaders [icol]. label);
		}
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which_Melder_NUMBER, criterion)) {
				autoTableRow newRow = Data_copy (row);
				Collection_addItem (thy rows, newRow.transfer());
			}
		}
		if (thy rows -> size == 0) {
			Melder_warning (L"No row matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rows not extracted.");
	}
}

Table Table_extractRowsWhereColumn_string (Table me, long columnNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autoTable thee = Table_create (0, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			autostring newLabel = Melder_wcsdup (my columnHeaders [icol]. label);
			thy columnHeaders [icol]. label = newLabel.transfer();
		}
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string, which_Melder_STRING, criterion)) {
				autoTableRow newRow = Data_copy (row);
				Collection_addItem (thy rows, newRow.transfer());
			}
		}
		if (thy rows -> size == 0) {
			Melder_warning (L"No row matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rows not extracted.");
	}
}

static void Table_columns_checkExist (Table me, wchar_t **columnNames, long n) {
	for (long i = 1; i <= n; i ++) {
		if (Table_findColumnIndexFromColumnLabel (me, columnNames [i]) == 0)
			Melder_throw (me, ": column \"", columnNames [i], "\" does not exist.");
	}
}

static void Table_columns_checkCrossSectionEmpty (wchar_t **factors, long nfactors, wchar_t **vars, long nvars) {
	for (long ifactor = 1; ifactor <= nfactors; ifactor ++) {
		for (long ivar = 1; ivar <= nvars; ivar ++) {
			if (wcsequ (factors [ifactor], vars [ivar]))
				Melder_throw ("Factor \"", factors [ifactor], "\" is also used as dependent variable.");
		}
	}
}

Table Table_collapseRows (Table me, const wchar_t *factors_string, const wchar_t *columnsToSum_string,
	const wchar_t *columnsToAverage_string, const wchar_t *columnsToMedianize_string,
	const wchar_t *columnsToAverageLogarithmically_string, const wchar_t *columnsToMedianizeLogarithmically_string)
{
	bool originalChanged = false;
	try {
		Melder_assert (factors_string != NULL);

		/*
		 * Parse the six strings of tokens.
		 */
		long numberOfFactors;
		autoMelderTokens factors (factors_string, & numberOfFactors);
		if (numberOfFactors < 1)
			Melder_throw ("In order to pool table data, you must supply at least one independent variable.");
		Table_columns_checkExist (me, factors.peek(), numberOfFactors);

		long numberToSum;
		autoMelderTokens columnsToSum;
		if (columnsToSum_string) {
			columnsToSum.reset (columnsToSum_string, & numberToSum);
			Table_columns_checkExist (me, columnsToSum.peek(), numberToSum);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToSum.peek(), numberToSum);
		}
		long numberToAverage;
		autoMelderTokens columnsToAverage;
		if (columnsToAverage_string) {
			columnsToAverage.reset (columnsToAverage_string, & numberToAverage);
			Table_columns_checkExist (me, columnsToAverage.peek(), numberToAverage);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToAverage.peek(), numberToAverage);
		}
		long numberToMedianize;
		autoMelderTokens columnsToMedianize;
		if (columnsToMedianize_string) {
			columnsToMedianize.reset (columnsToMedianize_string, & numberToMedianize);
			Table_columns_checkExist (me, columnsToMedianize.peek(), numberToMedianize);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToMedianize.peek(), numberToMedianize);
		}
		long numberToAverageLogarithmically;
		autoMelderTokens columnsToAverageLogarithmically;
		if (columnsToAverageLogarithmically_string) {
			columnsToAverageLogarithmically.reset (columnsToAverageLogarithmically_string, & numberToAverageLogarithmically);
			Table_columns_checkExist (me, columnsToAverageLogarithmically.peek(), numberToAverageLogarithmically);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToAverageLogarithmically.peek(), numberToAverageLogarithmically);
		}
		long numberToMedianizeLogarithmically;
		autoMelderTokens columnsToMedianizeLogarithmically;
		if (columnsToMedianizeLogarithmically_string) {
			columnsToMedianizeLogarithmically.reset (columnsToMedianizeLogarithmically_string, & numberToMedianizeLogarithmically);
			Table_columns_checkExist (me, columnsToMedianizeLogarithmically.peek(), numberToMedianizeLogarithmically);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToMedianizeLogarithmically.peek(), numberToMedianizeLogarithmically);
		}

		autoTable thee = Table_createWithoutColumnNames (0,
			numberOfFactors + numberToSum + numberToAverage + numberToMedianize + numberToAverageLogarithmically + numberToMedianizeLogarithmically);
		Melder_assert (thy numberOfColumns > 0);

		autoNUMvector <double> sortingColumn;
		if (numberToMedianize > 0 || numberToMedianizeLogarithmically > 0) {
			sortingColumn.reset (1, my rows -> size);
		}
		/*
		 * Set the column names. Within the dependent variables, the same name may occur more than once.
		 */
		autoNUMvector <long> columns (1, thy numberOfColumns);
		{
			long icol = 0;
			for (long i = 1; i <= numberOfFactors; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, factors [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, factors [i]);
			}
			for (long i = 1; i <= numberToSum; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, columnsToSum [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToSum [i]);
			}
			for (long i = 1; i <= numberToAverage; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, columnsToAverage [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverage [i]);
			}
			for (long i = 1; i <= numberToMedianize; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, columnsToMedianize [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianize [i]);
			}
			for (long i = 1; i <= numberToAverageLogarithmically; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, columnsToAverageLogarithmically [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverageLogarithmically [i]);
			}
			for (long i = 1; i <= numberToMedianizeLogarithmically; i ++) {
				Table_setColumnLabel (thee.peek(), ++ icol, columnsToMedianizeLogarithmically [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianizeLogarithmically [i]);
			}
			Melder_assert (icol == thy numberOfColumns);
		}
		/*
		 * Make sure that all the columns in the original table that we will use in the pooled table are defined.
		 */
		for (long icol = 1; icol <= thy numberOfColumns; icol ++) {
			Table_numericize_checkDefined (me, columns [icol]);
		}
		/*
		 * Remember the present sorting of the original table.
		 * (This is safe: the sorting index may change only vacuously when numericizing.)
		 * But this cannot be done before the previous block!
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			row -> sortingIndex = irow;
		}
		/*
		 * We will now sort the original table temporarily, by the factors (independent variables) only.
		 */
		Table_sortRows_Assert (me, columns.peek(), numberOfFactors);   /* This works only because the factors come first. */
		originalChanged = true;
		/*
		 * Find stretches of identical factors.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			long rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows -> size) break;
				for (long icol = 1; icol <= numberOfFactors; icol ++) {
					if (((TableRow) my rows -> item [rowmax]) -> cells [columns [icol]]. number !=
						((TableRow) my rows -> item [rowmin]) -> cells [columns [icol]]. number)
					{
						identical = false;
						break;
					}
				}
				if (! identical) break;
			}
			rowmax --;
			/*
			 * We have the stretch.
			 */
			Table_insertRow (thee.peek(), thy rows -> size + 1);
			{
				long icol = 0;
				for (long i = 1; i <= numberOfFactors; i ++) {
					++ icol;
					Table_setStringValue (thee.peek(), thy rows -> size, icol,
						((TableRow) my rows -> item [rowmin]) -> cells [columns [icol]]. string);
				}
				for (long i = 1; i <= numberToSum; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sum += ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
					}
					Table_setNumericValue (thee.peek(), thy rows -> size, icol, sum);
				}
				for (long i = 1; i <= numberToAverage; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sum += ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
					}
					Table_setNumericValue (thee.peek(), thy rows -> size, icol, sum / (rowmax - rowmin + 1));
				}
				for (long i = 1; i <= numberToMedianize; i ++) {
					++ icol;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sortingColumn [jrow] = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
					}
					NUMsort_d (rowmax - rowmin + 1, & sortingColumn [rowmin - 1]);
					double median = NUMquantile (rowmax - rowmin + 1, & sortingColumn [rowmin - 1], 0.5);
					Table_setNumericValue (thee.peek(), thy rows -> size, icol, median);
				}
				for (long i = 1; i <= numberToAverageLogarithmically; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						double value = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
						if (value <= 0.0)
							Melder_throw (
								"The cell in column \"", columnsToAverageLogarithmically [i],
								"\" of row ", jrow, " of ", me,
								" is not positive.\nCannot average logarithmically.");
						sum += log (value);
					}
					Table_setNumericValue (thee.peek(), thy rows -> size, icol, exp (sum / (rowmax - rowmin + 1)));
				}
				for (long i = 1; i <= numberToMedianizeLogarithmically; i ++) {
					++ icol;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						double value = ((TableRow) my rows -> item [jrow]) -> cells [columns [icol]]. number;
						if (value <= 0.0)
							Melder_throw (
								"The cell in column \"", columnsToMedianizeLogarithmically [i],
								"\" of row ", jrow, " of ", me,
								" is not positive.\nCannot medianize logarithmically.");
						sortingColumn [jrow] = log (value);
					}
					NUMsort_d (rowmax - rowmin + 1, & sortingColumn [rowmin - 1]);
					double median = NUMquantile (rowmax - rowmin + 1, & sortingColumn [rowmin - 1], 0.5);
					Table_setNumericValue (thee.peek(), thy rows -> size, icol, exp (median));
				}
				Melder_assert (icol == thy numberOfColumns);
			}
			irow = rowmax;
		}
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table
		return thee.transfer();
	} catch (MelderError) {
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
		return NULL;
	}
}

static wchar_t ** _Table_getLevels (Table me, long column, long *numberOfLevels) {
	try {
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			row -> sortingIndex = irow;
		}
		long columns [2] = { 0, column };
		Table_sortRows_Assert (me, columns, 1);
		*numberOfLevels = 0;
		long irow = 1;
		while (irow <= my rows -> size) {
			double value = ((TableRow) my rows -> item [irow]) -> cells [column]. number;
			(*numberOfLevels) ++;
			while (++ irow <= my rows -> size && ((TableRow) my rows -> item [irow]) -> cells [column]. number == value) { }
		}
		autostringvector result (1, *numberOfLevels);
		*numberOfLevels = 0;
		irow = 1;
		while (irow <= my rows -> size) {
			double value = ((TableRow) my rows -> item [irow]) -> cells [column]. number;
			result [++ *numberOfLevels] = Melder_wcsdup (Table_getStringValue_Assert (me, irow, column));
			while (++ irow <= my rows -> size && ((TableRow) my rows -> item [irow]) -> cells [column]. number == value) { }
		}
		sortRowsByIndex_NoError (me);   // unsort the original table
		return result.transfer();
	} catch (MelderError) {
		sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
		return NULL;
	}
}

Table Table_rowsToColumns (Table me, const wchar_t *factors_string, long columnToTranspose, const wchar_t *columnsToExpand_string) {
	bool originalChanged = false;
	try {
		Melder_assert (factors_string != NULL);

		long numberOfFactors = 0, numberToExpand = 0, numberOfLevels = 0;
		bool warned = false;
		/*
		 * Parse the two strings of tokens.
		 */
		autoMelderTokens factors_names (factors_string, & numberOfFactors);
		if (numberOfFactors < 1)
			Melder_throw ("In order to nest table data, you must supply at least one independent variable.");
		Table_columns_checkExist (me, factors_names.peek(), numberOfFactors);
		autoMelderTokens columnsToExpand_names (columnsToExpand_string, & numberToExpand);
		if (numberToExpand < 1)
			Melder_throw ("In order to nest table data, you must supply at least one dependent variable (to expand).");
		Table_columns_checkExist (me, columnsToExpand_names.peek(), numberToExpand);
		Table_columns_checkCrossSectionEmpty (factors_names.peek(), numberOfFactors, columnsToExpand_names.peek(), numberToExpand);
		wchar_t ** dummy = _Table_getLevels (me, columnToTranspose, & numberOfLevels);
		autostringvector levels_names (dummy, 1, numberOfLevels);
		/*
		 * Get the column numbers for the factors.
		 */
		autoNUMvector <long> factorColumns (1, numberOfFactors);
		for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
			factorColumns [ifactor] = Table_findColumnIndexFromColumnLabel (me, factors_names [ifactor]);
			/*
			 * Make sure that all the columns in the original table that we will use in the nested table are defined.
			 */
			Table_numericize_checkDefined (me, factorColumns [ifactor]);
		}
		/*
		 * Get the column numbers for the expandable variables.
		 */
		autoNUMvector <long> columnsToExpand (1, numberToExpand);
		for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			columnsToExpand [iexpand] = Table_findColumnIndexFromColumnLabel (me, columnsToExpand_names [iexpand]);
			Table_numericize_checkDefined (me, columnsToExpand [iexpand]);
		}
		/*
		 * Create the new table, with column names.
		 */
		autoTable thee = Table_createWithoutColumnNames (0, numberOfFactors + (numberOfLevels * numberToExpand));
		Melder_assert (thy numberOfColumns > 0);
		for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
			Table_setColumnLabel (thee.peek(), ifactor, factors_names [ifactor]);
		}
		autoMelderString columnLabel;
		for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			for (long ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				MelderString_copy (& columnLabel, columnsToExpand_names [iexpand]);
				MelderString_appendCharacter (& columnLabel, '.');
				MelderString_append (& columnLabel, levels_names [ilevel]);
				//Melder_casual ("Number of factors: %ld", numberOfFactors);
				//Melder_casual ("Level: %ld out of %ld", ilevel, numberOfLevels);
				long columnNumber = numberOfFactors + (iexpand - 1) * numberOfLevels + ilevel;
				//Melder_casual ("Column number: %ld", columnNumber);
				Table_setColumnLabel (thee.peek(), columnNumber, columnLabel.string);
			}
		}
		/*
		 * Remember the present sorting of the original table.
		 * (This is safe: the sorting index may change only vacuously when numericizing.)
		 * But this cannot be done before the previous blocks that numericize!
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			row -> sortingIndex = irow;
		}
		/*
		 * We will now sort the original table temporarily, by the factors (independent variables) only.
		 */
		Table_sortRows_Assert (me, factorColumns.peek(), numberOfFactors);
		originalChanged = true;
		/*
		 * Find stretches of identical factors.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			long rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows -> size) break;
				for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
					if (((TableRow) my rows -> item [rowmax]) -> cells [factorColumns [ifactor]]. number !=
						((TableRow) my rows -> item [rowmin]) -> cells [factorColumns [ifactor]]. number)
					{
						identical = false;
						break;
					}
				}
				if (! identical) break;
			}
			#if 0
			if (rowmax - rowmin > numberOfLevels && ! warned) {
				Melder_warning (L"Some rows of the original table have not been included in the new table. "
					"You could perhaps add more factors.");
				warned = true;
			}
			#endif
			rowmax --;
			/*
			 * We have the stretch.
			 */
			Table_insertRow (thee.peek(), thy rows -> size + 1);
			TableRow thyRow = static_cast <TableRow> (thy rows -> item [thy rows -> size]);
			for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
				Table_setStringValue (thee.peek(), thy rows -> size, ifactor,
					((TableRow) my rows -> item [rowmin]) -> cells [factorColumns [ifactor]]. string);
			}
			for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
				for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
					TableRow myRow = static_cast <TableRow> (my rows -> item [jrow]);
					double value = myRow -> cells [columnsToExpand [iexpand]]. number;
					long level = myRow -> cells [columnToTranspose]. number;
					long thyColumn = numberOfFactors + (iexpand - 1) * numberOfLevels + level;
					if (thyRow -> cells [thyColumn]. string != NULL && ! warned) {
						Melder_warning (L"Some information from the original table has not been included in the new table. "
							"You could perhaps add more factors.");
						warned = true;
					}
					Table_setNumericValue (thee.peek(), thy rows -> size, thyColumn, value);
				}
			}
			irow = rowmax;
		}
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table
		return thee.transfer();
	} catch (MelderError) {
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
		return NULL;
	}
}

Table Table_transpose (Table me) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my numberOfColumns, 1 + my rows -> size);
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				Table_setStringValue (thee.peek(), icol, 1, my columnHeaders [icol]. label);
			}
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				Table_setStringValue (thee.peek(), icol, 1 + irow, Table_getStringValue_Assert (me, irow, icol));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not transposed.");
	}
}

static long *cellCompare_columns, cellCompare_numberOfColumns;

static int cellCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	for (long icol = 1; icol <= cellCompare_numberOfColumns; icol ++) {
		if (my cells [cellCompare_columns [icol]]. number < thy cells [cellCompare_columns [icol]]. number) return -1;
		if (my cells [cellCompare_columns [icol]]. number > thy cells [cellCompare_columns [icol]]. number) return +1;
	}
	return 0;
}

void Table_sortRows_Assert (Table me, long *columns, long numberOfColumns) {
	for (long icol = 1; icol <= numberOfColumns; icol ++) {
		Table_numericize_Assert (me, columns [icol]);
	}
	cellCompare_columns = columns;
	cellCompare_numberOfColumns = numberOfColumns;
	qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), cellCompare_NoError);
}

void Table_sortRows_string (Table me, const wchar_t *columns_string) {
	try {
		long numberOfColumns;
		autoMelderTokens columns_tokens (columns_string, & numberOfColumns);
		if (numberOfColumns < 1)
			Melder_throw (me, ": you specified an empty list of columns.");
		autoNUMvector <long> columns (1, numberOfColumns);
		for (long icol = 1; icol <= numberOfColumns; icol ++) {
			columns [icol] = Table_findColumnIndexFromColumnLabel (me, columns_tokens [icol]);
			if (columns [icol] == 0)
				Melder_throw ("Column \"", columns_tokens [icol], L"\" does not exist.");
		}
		Table_sortRows_Assert (me, columns.peek(), numberOfColumns);
	} catch (MelderError) {
		Melder_throw (me, ": rows not sorted.");
	}
}

void Table_randomizeRows (Table me) {
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		long jrow = NUMrandomInteger (irow, my rows -> size);
		TableRow tmp = static_cast <TableRow> (my rows -> item [irow]);
		my rows -> item [irow] = my rows -> item [jrow];
		my rows -> item [jrow] = tmp;
	}
}

void Table_reflectRows (Table me) {
	for (long irow = 1; irow <= my rows -> size / 2; irow ++) {
		long jrow = my rows -> size + 1 - irow;
		TableRow tmp = static_cast <TableRow> (my rows -> item [irow]);
		my rows -> item [irow] = my rows -> item [jrow];
		my rows -> item [jrow] = tmp;
	}
}

Table Tables_append (Collection me) {
	try {
		if (my size == 0) Melder_throw ("Cannot add zero tables.");
		Table thee = static_cast <Table> (my item [1]);
		long nrow = thy rows -> size;
		long ncol = thy numberOfColumns;
		Table firstTable = thee;
		for (long itab = 2; itab <= my size; itab ++) {
			thee = static_cast <Table> (my item [itab]);
			nrow += thy rows -> size;
			if (thy numberOfColumns != ncol)
				Melder_throw ("Numbers of columns do not match.");
			for (long icol = 1; icol <= ncol; icol ++) {
				if (! Melder_wcsequ (thy columnHeaders [icol]. label, firstTable -> columnHeaders [icol]. label))
					Melder_throw ("The label of column ", icol, " of ", thee,
						" (", thy columnHeaders [icol]. label, L") does not match the label of column ", icol,
						" of ", firstTable, " (", firstTable -> columnHeaders [icol]. label, ").");
			}
		}
		autoTable him = Table_createWithoutColumnNames (nrow, ncol);
		for (long icol = 1; icol <= ncol; icol ++) {
			Table_setColumnLabel (him.peek(), icol, thy columnHeaders [icol]. label);
		}
		nrow = 0;
		for (long itab = 1; itab <= my size; itab ++) {
			thee = static_cast <Table> (my item [itab]);
			for (long irow = 1; irow <= thy rows -> size; irow ++) {
				nrow ++;
				for (long icol = 1; icol <= ncol; icol ++) {
					Table_setStringValue (him.peek(), nrow, icol, Table_getStringValue_Assert (thee, irow, icol));
				}
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Table objects not appended.");
	}
}

void Table_appendSumColumn (Table me, long column1, long column2, const wchar_t *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows -> size, 1);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			Table_setNumericValue (thee.peek(), irow, 1, myRow -> cells [column1]. number + myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			TableRow thyRow = static_cast <TableRow> (thy rows -> item [irow]);
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (myCell -> string == NULL);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = NULL;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, ": sum column not appended.");
	}
}

void Table_appendDifferenceColumn (Table me, long column1, long column2, const wchar_t *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows -> size, 1);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			Table_setNumericValue (thee.peek(), irow, 1, myRow -> cells [column1]. number - myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			TableRow thyRow = static_cast <TableRow> (thy rows -> item [irow]);
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (myCell -> string == NULL);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = NULL;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, ": difference column not appended.");
	}
}

void Table_appendProductColumn (Table me, long column1, long column2, const wchar_t *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows -> size, 1);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			Table_setNumericValue (thee.peek(), irow, 1, myRow -> cells [column1]. number * myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			TableRow thyRow = static_cast <TableRow> (thy rows -> item [irow]);
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (myCell -> string == NULL);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = NULL;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, ": product column not appended.");
	}
}

void Table_appendQuotientColumn (Table me, long column1, long column2, const wchar_t *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows -> size, 1);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			double value = myRow -> cells [column2]. number == 0.0 ? NUMundefined :
				myRow -> cells [column1]. number / myRow -> cells [column2]. number;
			Table_setNumericValue (thee.peek(), irow, 1, value);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow myRow = static_cast <TableRow> (my rows -> item [irow]);
			TableRow thyRow = static_cast <TableRow> (thy rows -> item [irow]);
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (myCell -> string == NULL);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = NULL;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, ": quotient column not appended.");
	}
}

void Table_formula_columnRange (Table me, long fromColumn, long toColumn, const wchar_t *expression, Interpreter interpreter) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, fromColumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, toColumn);
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, TRUE);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			for (long icol = fromColumn; icol <= toColumn; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING) {
					Table_setStringValue (me, irow, icol, result. result.stringResult);
					Melder_free (result. result.stringResult);
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
					Table_setNumericValue (me, irow, icol, result. result.numericResult);
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_ARRAY) {
					Melder_throw (me, ": cannot put arrays into cells.");
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING_ARRAY) {
					Melder_throw (me, ": cannot put arrays into cells.");
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": application of formula not completed.");
	}
}

void Table_formula (Table me, long icol, const wchar_t *expression, Interpreter interpreter) {
	Table_formula_columnRange (me, icol, icol, expression, interpreter);
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
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		sum1 += row -> cells [column1]. number;
		sum2 += row -> cells [column2]. number;
	}
	mean1 = sum1 / n;
	mean2 = sum2 / n;
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (irow = 1; irow < n; irow ++) {
		TableRow rowi = static_cast <TableRow> (my rows -> item [irow]);
		for (jrow = irow + 1; jrow <= n; jrow ++) {
			TableRow rowj = static_cast <TableRow> (my rows -> item [jrow]);
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
	double *out_t, double *out_numberOfDegreesOfFreedom, double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_t) *out_t = NUMundefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = NUMundefined;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	long n = my rows -> size;
	if (n < 1) return NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	double sum = 0.0;
	for (long irow = 1; irow <= n; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		sum += row -> cells [column1]. number - row -> cells [column2]. number;
	}
	double meanDifference = sum / n;
	long degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= n; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	double mean = 0.0, var = 0.0, standardError;
	long n = my rows -> size;
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (n < 1) return NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	long degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	Table_numericize_Assert (me, column);
	for (long irow = 1; irow <= n; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		mean += row -> cells [column]. number;
	}
	mean /= n;
	if (n >= 2 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		for (long irow = 1; irow <= n; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			double diff = row -> cells [column]. number - mean;
			var += diff * diff;
		}
		standardError = sqrt (var / degreesOfFreedom / n);
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

double Table_getGroupMean_studentT (Table me, long column, long groupColumn, const wchar_t *group, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	Table_numericize_Assert (me, column);
	long n = 0;
	double sum = 0.0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = NUMundefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns) return NUMundefined;
	Table_numericize_Assert (me, column);
	long n1 = 0, n2 = 0;
	double sum1 = 0.0, sum2 = 0.0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	long degreesOfFreedom = n1 + n2 - 2;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	double mean1 = sum1 / n1;
	double mean2 = sum2 / n2;
	double difference = mean1 - mean2;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		double sumOfSquares = 0.0;
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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

double Table_getGroupDifference_wilcoxonRankSum (Table me, long column, long groupColumn, const wchar_t *group1, const wchar_t *group2,
	double *out_rankSum, double *out_significanceFromZero)
{
	if (out_rankSum) *out_rankSum = NUMundefined;
	if (out_significanceFromZero) *out_significanceFromZero = NUMundefined;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns) return NUMundefined;
	Table_numericize_Assert (me, column);
	long n1 = 0, n2 = 0;
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		if (row -> cells [groupColumn]. string != NULL) {
			if (wcsequ (row -> cells [groupColumn]. string, group1)) {
				n1 ++;
			} else if (wcsequ (row -> cells [groupColumn]. string, group2)) {
				n2 ++;
			}
		}
	}
	long n = n1 + n2;
	if (n1 < 1 || n2 < 1 || n < 3) return NUMundefined;
	Table ranks = Table_createWithoutColumnNames (n, 3);   // column 1 = group, 2 = value, 3 = rank
	for (long irow = 1, jrow = 0; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		if (row -> cells [groupColumn]. string != NULL) {
			if (wcsequ (row -> cells [groupColumn]. string, group1)) {
				Table_setNumericValue (ranks, ++ jrow, 1, 1.0);
				Table_setNumericValue (ranks, jrow, 2, row -> cells [column]. number);
			} else if (wcsequ (row -> cells [groupColumn]. string, group2)) {
				Table_setNumericValue (ranks, ++ jrow, 1, 2.0);
				Table_setNumericValue (ranks, jrow, 2, row -> cells [column]. number);
			}
		}
	}
	Table_numericize_Assert (ranks, 1);
	Table_numericize_Assert (ranks, 2);
	Table_numericize_Assert (ranks, 3);
	long columns [1+1] = { 0, 2 };   // we're gonna sort by column 2
	Table_sortRows_Assert (ranks, columns, 1);   // we sort by one column only
	double totalNumberOfTies3 = 0.0;
	for (long irow = 1; irow <= ranks -> rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (ranks -> rows -> item [irow]);
		double value = row -> cells [2]. number;
		long rowOfLastTie = irow + 1;
		for (; rowOfLastTie <= ranks -> rows -> size; rowOfLastTie ++) {
			TableRow row2 = static_cast <TableRow> (ranks -> rows -> item [rowOfLastTie]);
			double value2 = row2 -> cells [2]. number;
			if (value2 != value) break;
		}
		rowOfLastTie --;
		double averageRank = 0.5 * ((double) irow + (double) rowOfLastTie);
		for (long jrow = irow; jrow <= rowOfLastTie; jrow ++) {
			Table_setNumericValue (ranks, jrow, 3, averageRank);
		}
		long numberOfTies = rowOfLastTie - irow + 1;
		totalNumberOfTies3 += (double) (numberOfTies - 1) * (double) numberOfTies * (double) (numberOfTies + 1);
	}
	Table_numericize_Assert (ranks, 3);
	double maximumRankSum = (double) n1 * (double) n2, rankSum = 0.0;
	for (long irow = 1; irow <= ranks -> rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (ranks -> rows -> item [irow]);
		if (row -> cells [1]. number == 1.0) rankSum += row -> cells [3]. number;
	}
	rankSum -= 0.5 * (double) n1 * ((double) n1 + 1.0);
	double stdev = sqrt (maximumRankSum * ((double) n + 1.0 - totalNumberOfTies3 / n / (n - 1)) / 12.0);
	if (out_rankSum) *out_rankSum = rankSum;
	if (out_significanceFromZero) *out_significanceFromZero = NUMgaussQ (fabs (rankSum - 0.5 * maximumRankSum) / stdev);
	forget (ranks);
	return rankSum / maximumRankSum;
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
	Table_numericize_Assert (me, icol);
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
	Table_numericize_Assert (me, xcolumn);
	Table_numericize_Assert (me, ycolumn);
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
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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
	long n = my rows -> size;
	int saveFontSize = Graphics_inqFontSize (g);
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
	Table_numericize_Assert (me, xcolumn);
	Table_numericize_Assert (me, ycolumn);
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
	for (long irow = 1; irow <= n; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
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

void Table_drawEllipse_e (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int garnish)
{
	try {
		if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
		Table_numericize_Assert (me, xcolumn);
		Table_numericize_Assert (me, ycolumn);
		if (xmin == xmax) {
			if (! Table_getExtrema (me, xcolumn, & xmin, & xmax)) return;
			if (xmin == xmax) xmin -= 0.5, xmax += 0.5;
		}
		if (ymin == ymax) {
			if (! Table_getExtrema (me, ycolumn, & ymin, & ymax)) return;
			if (ymin == ymax) ymin -= 0.5, ymax += 0.5;
		}
		autoTableOfReal tableOfReal = TableOfReal_create (my rows -> size, 2);
		for (long irow = 1; irow <= my rows -> size; irow ++) {
			tableOfReal -> data [irow] [1] = Table_getNumericValue_Assert (me, irow, xcolumn);
			tableOfReal -> data [irow] [2] = Table_getNumericValue_Assert (me, irow, ycolumn);
		}
		autoSSCP sscp = TableOfReal_to_SSCP (tableOfReal.peek(), 0, 0, 0, 0);
		SSCP_drawConcentrationEllipse (sscp.peek(), g, numberOfSigmas, 0, 1, 2, xmin, xmax, ymin, ymax, garnish);
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

static const wchar_t *visibleString (const wchar_t *s) {
	return s != NULL && s [0] != '\0' ? s : L"?";
}

void Table_list (Table me, bool includeRowNumbers) {
	MelderInfo_open ();
	if (includeRowNumbers) {
		MelderInfo_write (L"row");
		if (my numberOfColumns > 0) MelderInfo_write (L"\t");
	}
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol > 1) MelderInfo_write (L"\t");
		MelderInfo_write (visibleString (my columnHeaders [icol]. label));
	}
	MelderInfo_write (L"\n");
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		if (includeRowNumbers) {
			MelderInfo_write (Melder_integer (irow));
			if (my numberOfColumns > 0) MelderInfo_write (L"\t");
		}
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol > 1) MelderInfo_write (L"\t");
			MelderInfo_write (visibleString (row -> cells [icol]. string));
		}
		MelderInfo_write (L"\n");
	}
	MelderInfo_close ();
}

static void _Table_writeToCharacterSeparatedFile (Table me, MelderFile file, wchar_t kar) {
	autoMelderString buffer;
	for (long icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol != 1) MelderString_appendCharacter (& buffer, kar);
		wchar_t *s = my columnHeaders [icol]. label;
		MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
	}
	MelderString_appendCharacter (& buffer, '\n');
	for (long irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = static_cast <TableRow> (my rows -> item [irow]);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol != 1) MelderString_appendCharacter (& buffer, kar);
			wchar_t *s = row -> cells [icol]. string;
			MelderString_append (& buffer, s != NULL && s [0] != '\0' ? s : L"?");
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
}

void Table_writeToTabSeparatedFile (Table me, MelderFile file) {
	try {
		_Table_writeToCharacterSeparatedFile (me, file, '\t');
	} catch (MelderError) {
		Melder_throw (me, ": not written to tab-separated file.");
	}
}

void Table_writeToCommaSeparatedFile (Table me, MelderFile file) {
	try {
		_Table_writeToCharacterSeparatedFile (me, file, ',');
	} catch (MelderError) {
		Melder_throw (me, ": not written to comma-separated file.");
	}
}

Table Table_readFromTableFile (MelderFile file) {
	Table me = NULL;
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
		if (nelements == 0 || nelements % ncol != 0)
			Melder_throw ("The number of elements (", nelements, ") is not a multiple of the number of columns (", ncol, ").");

		/*
		 * Create empty table.
		 */
		nrow = nelements / ncol - 1;
		me = Table_create (nrow, ncol);

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
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			for (long icol = 1; icol <= ncol; icol ++) {
				while (*p == ' ' || *p == '\t' || *p == '\n') { Melder_assert (*p != '\0'); p ++; }
				static MelderString buffer = { 0 };
				MelderString_empty (& buffer);
				while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0') { MelderString_appendCharacter (& buffer, *p); p ++; }
				row -> cells [icol]. string = Melder_wcsdup_f (buffer.string);
				MelderString_empty (& buffer);
			}
		}
		return me;
	} catch (MelderError) {
		forget (me);
		Melder_throw ("Table object not read from space-separated text file ", file, ".");
	}
}

Table Table_readFromCharacterSeparatedTextFile (MelderFile file, wchar_t separator) {
	try {
		autostring string = MelderFile_readText (file);

		/*
		 * Kill final new-line symbols.
		 */
		for (long length = wcslen (string.peek()); length > 0 && string [length - 1] == '\n'; length = wcslen (string.peek())) string [length - 1] = '\0';

		/*
		 * Count columns.
		 */
		long ncol = 1;
		const wchar_t *p = & string [0];
		for (;;) {
			wchar_t kar = *p++;
			if (kar == '\0') Melder_throw (L"No rows.");
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
		autoTable me = Table_create (nrow, ncol);

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
			Table_setColumnLabel (me.peek(), icol, buffer.string);
			MelderString_empty (& buffer);
		}

		/*
		 * Read cells.
		 */
		for (long irow = 1; irow <= nrow; irow ++) {
			TableRow row = static_cast <TableRow> (my rows -> item [irow]);
			for (long icol = 1; icol <= ncol; icol ++) {
				static MelderString buffer = { 0 };
				MelderString_empty (& buffer);
				while (*p != separator && *p != '\n' && *p != '\0') {
					MelderString_appendCharacter (& buffer, *p);
					p ++;
				}
				if (*p == '\0') {
					if (irow != nrow) Melder_fatal ("irow %ld, nrow %ld, icol %ld, ncol %ld", irow, nrow, icol, ncol);
					if (icol != ncol) Melder_throw ("Last row incomplete.");
				} else if (*p == '\n') {
					if (icol != ncol) Melder_throw ("Row ", irow, " incomplete.");
					p ++;
				} else {
					Melder_assert (*p == separator);
					p ++;
				}
				row -> cells [icol]. string = Melder_wcsdup (buffer.string);
				MelderString_empty (& buffer);
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Table object not read from character-separated text file ", file, ".");
	}
}

/* End of file Table.cpp */
