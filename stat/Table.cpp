/* Table.cpp
 *
 * Copyright (C) 2002-2012,2013,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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

Thing_implement (TableRow, Daata, 0);

Thing_implement (Table, Daata, 0);

void structTable :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of rows: ", our rows.size);
	MelderInfo_writeLine (U"Number of columns: ", our numberOfColumns);
}

const char32 * structTable :: v_getColStr (long columnNumber) {
	if (columnNumber < 1 || columnNumber > our numberOfColumns) return nullptr;
	return our columnHeaders [columnNumber]. label ? our columnHeaders [columnNumber]. label : U"";
}

double structTable :: v_getMatrix (long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > our rows.size) return undefined;
	if (columnNumber < 1 || columnNumber > our numberOfColumns) return undefined;
	char32 *stringValue = our rows.at [rowNumber] -> cells [columnNumber]. string;
	return stringValue ? Melder_atof (stringValue) : undefined;
}

const char32 * structTable :: v_getMatrixStr (long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > our rows.size) return U"";
	if (columnNumber < 1 || columnNumber > our numberOfColumns) return U"";
	char32 *stringValue = our rows.at [rowNumber] -> cells [columnNumber]. string;
	return stringValue ? stringValue : U"";
}

double structTable :: v_getColIndex (const char32 *columnLabel) {
	return Table_findColumnIndexFromColumnLabel (this, columnLabel);
}

static autoTableRow TableRow_create (long numberOfColumns) {
	autoTableRow me = Thing_new (TableRow);
	my numberOfColumns = numberOfColumns;
	my cells = NUMvector <structTableCell> (1, numberOfColumns);
	return me;
}

void Table_initWithoutColumnNames (Table me, long numberOfRows, long numberOfColumns) {
	if (numberOfColumns < 1)
		Melder_throw (U"Cannot create table without columns.");
	my numberOfColumns = numberOfColumns;
	my columnHeaders = NUMvector <structTableColumnHeader> (1, numberOfColumns);
	for (long irow = 1; irow <= numberOfRows; irow ++) {
		Table_appendRow (me);
	}
}

autoTable Table_createWithoutColumnNames (long numberOfRows, long numberOfColumns) {
	try {
		autoTable me = Thing_new (Table);
		Table_initWithoutColumnNames (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}
}

void Table_initWithColumnNames (Table me, long numberOfRows, const char32 *columnNames) {
	Table_initWithoutColumnNames (me, numberOfRows, Melder_countTokens (columnNames));
	long icol = 0;
	for (char32 *columnName = Melder_firstToken (columnNames); columnName; columnName = Melder_nextToken ()) {
		icol ++;
		Table_setColumnLabel (me, icol, columnName);
	}
}

autoTable Table_createWithColumnNames (long numberOfRows, const char32 *columnNames) {
	try {
		autoTable me = Thing_new (Table);
		Table_initWithColumnNames (me.get(), numberOfRows, columnNames);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}
}

void Table_appendRow (Table me) {
	try {
		autoTableRow row = TableRow_create (my numberOfColumns);
		my rows. addItem_move (row.move());
	} catch (MelderError) {
		Melder_throw (me, U": row not appended.");
	}
}

void Table_appendColumn (Table me, const char32 *label) {
	try {
		Table_insertColumn (me, my numberOfColumns + 1, label);
	} catch (MelderError) {
		Melder_throw (me, U": column \"", label, U"\" not appended.");
	}
}

void Table_checkSpecifiedRowNumberWithinRange (Table me, long rowNumber) {
	if (rowNumber < 1)
		Melder_throw (me, U": the specified row number is ", rowNumber, U", but should be at least 1.");
	if (rowNumber > my rows.size)
		Melder_throw (me, U": the specified row number (", rowNumber, U") exceeds my number of rows (", my rows.size, U").");
}

void Table_removeRow (Table me, long rowNumber) {
	try {
		if (my rows.size == 1)
			Melder_throw (me, U": cannot remove my only row.");
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		my rows. removeItem (rowNumber);
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": row ", rowNumber, U" not removed.");
	}
}

void Table_checkSpecifiedColumnNumberWithinRange (Table me, long columnNumber) {
	if (columnNumber < 1)
		Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at least 1.");
	if (columnNumber > my numberOfColumns)
		Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at most my number of columns (", my numberOfColumns, U").");
}

void Table_removeColumn (Table me, long columnNumber) {
	try {
		if (my numberOfColumns == 1)
			Melder_throw (me, U": cannot remove my only column.");
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		/*
		 * Changes without error.
		 */
		Melder_free (my columnHeaders [columnNumber]. label);
		for (long icol = columnNumber; icol < my numberOfColumns; icol ++)
			my columnHeaders [icol] = my columnHeaders [icol + 1];
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			Melder_free (row -> cells [columnNumber]. string);
			for (long icol = columnNumber; icol < row -> numberOfColumns; icol ++)
				row -> cells [icol] = row -> cells [icol + 1];
			row -> numberOfColumns --;
		}
		my numberOfColumns --;
	} catch (MelderError) {
		Melder_throw (me, U": column ", columnNumber, U" not removed.");
	}
}

void Table_insertRow (Table me, long rowNumber) {
	try {
		/*
		 * Check without changes.
		 */
		if (rowNumber < 1)
			Melder_throw (me, U": the specified row number is ", rowNumber, U", but should be at least 1.");
		if (rowNumber > my rows.size + 1)
			Melder_throw (me, U": the specified row number is ", rowNumber, U", but should be at most my number of rows (", my rows.size, U") plus 1.");
		autoTableRow row = TableRow_create (my numberOfColumns);
		/*
		 * Safe change.
		 */
		my rows. addItemAtPosition_move (row.move(), rowNumber);
		/*
		 * Changes without error.
		 */
		for (long icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": row ", rowNumber, U" not inserted.");
	}
}

void Table_insertColumn (Table me, long columnNumber, const char32 *label /* cattable */) {
	try {
		/*
		 * Check without changes.
		 */
		if (columnNumber < 1)
			Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at least 1.");
		if (columnNumber > my numberOfColumns + 1)
			Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at most my number of columns (", my numberOfColumns, U") plus 1.");
		autostring32 newLabel = Melder_dup (label);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, my numberOfColumns + 1);
		/*
		 * Changes without error.
		 */
		/*
		 * Transfer column headers to larger structure.
		 */
		for (long icol = 1; icol < columnNumber; icol ++) {
			Melder_assert (! thy columnHeaders [icol]. label);   // make room...
			thy columnHeaders [icol] = my columnHeaders [icol];   // ...fill in and dangle...
			my columnHeaders [icol]. label = nullptr;   // ...undangle
		}
		thy columnHeaders [columnNumber]. label = newLabel.transfer();
		thy columnHeaders [columnNumber]. numericized = false;
		for (long icol = my numberOfColumns + 1; icol > columnNumber; icol --) {
			Melder_assert (! thy columnHeaders [icol]. label);   // make room...
			thy columnHeaders [icol] = my columnHeaders [icol - 1];   // ...fill in and dangle...
			my columnHeaders [icol - 1]. label = nullptr;   // ...undangle
		}
		/*
		 * Transfer rows to larger structure.
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			for (long icol = 1; icol < columnNumber; icol ++) {
				Melder_assert (! thyRow -> cells [icol]. string);   // make room...
				thyRow -> cells [icol] = myRow -> cells [icol];   // ...fill in and dangle...
				myRow -> cells [icol]. string = nullptr;   // ...undangle
			}
			Melder_assert (! thyRow -> cells [columnNumber]. string);
			Melder_assert (thyRow -> cells [columnNumber]. number == 0.0);
			for (long icol = myRow -> numberOfColumns + 1; icol > columnNumber; icol --) {
				Melder_assert (! thyRow -> cells [icol]. string);   // make room...
				thyRow -> cells [icol] = myRow -> cells [icol - 1];   // ...fill in and dangle...
				myRow -> cells [icol - 1]. string = nullptr;   // ...undangle
			}
		}
		/*
		 * Transfer larger structure with column headers to me.
		 */
		NUMvector_free <structTableColumnHeader> (my columnHeaders, 1);   // make room...
		my columnHeaders = thy columnHeaders;   // ...fill in and dangle...
		thy columnHeaders = nullptr;   // ...undangle
		/*
		 * Transfer larger structure with rows to me.
		 */
		my rows = thy rows.move();
		/*
		 * Update my state.
		 */
		my numberOfColumns ++;
	} catch (MelderError) {
		Melder_throw (me, U": column not inserted.");
	}
}

void Table_setColumnLabel (Table me, long columnNumber, const char32 *label /* cattable */) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring32 newLabel = Melder_dup (label);
		/*
		 * Changes without error.
		 */
		Melder_free (my columnHeaders [columnNumber]. label);
		my columnHeaders [columnNumber]. label = newLabel.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": column label not set.");
	}
}

long Table_findColumnIndexFromColumnLabel (Table me, const char32 *label) noexcept {
	for (long icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnHeaders [icol]. label && str32equ (my columnHeaders [icol]. label, label))
			return icol;
	return 0;
}

long Table_getColumnIndexFromColumnLabel (Table me, const char32 *columnLabel) {
	long columnNumber = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	if (columnNumber == 0)
		Melder_throw (me, U": there is no column named \"", columnLabel, U"\".");
	return columnNumber;
}

long * Table_getColumnIndicesFromColumnLabelString (Table me, const char32 *string, long *ptr_numberOfTokens) {
	autoMelderTokens tokens (string);
	if (tokens.count() < 1)
		Melder_throw (me, U": you specified an empty list of columns.");
	autoNUMvector <long> columns (1, tokens.count());
	for (long icol = 1; icol <= tokens.count(); icol ++) {
		columns [icol] = Table_getColumnIndexFromColumnLabel (me, tokens [icol]);
	}
	*ptr_numberOfTokens = tokens.count();
	return columns.transfer();
}

long Table_searchColumn (Table me, long columnNumber, const char32 *value) noexcept {
	for (long irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [columnNumber]. string && str32equ (row -> cells [columnNumber]. string, value))
			return irow;
	}
	return 0;
}

void Table_setStringValue (Table me, long rowNumber, long columnNumber, const char32 *value /* cattable */) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring32 newValue = Melder_dup (value);
		/*
		 * Change without errors.
		 */
		TableRow row = my rows.at [rowNumber];
		Melder_free (row -> cells [columnNumber]. string);
		row -> cells [columnNumber]. string = newValue.transfer();
		my columnHeaders [columnNumber]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": string value not set.");
	}
}

void Table_setNumericValue (Table me, long rowNumber, long columnNumber, double value) {
	try {
		/*
		 * Check without changes.
		 */
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring32 newValue = Melder_dup (Melder_double (value));
		/*
		 * Change without errors.
		 */
		TableRow row = my rows.at [rowNumber];
		Melder_free (row -> cells [columnNumber]. string);
		row -> cells [columnNumber]. string = newValue.transfer();
		my columnHeaders [columnNumber]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": numeric value not set.");
	}
}

bool Table_isCellNumeric_ErrorFalse (Table me, long rowNumber, long columnNumber) {
	if (rowNumber < 1 || rowNumber > my rows.size) return false;
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return false;
	TableRow row = my rows.at [rowNumber];
	const char32 *cell = row -> cells [columnNumber]. string;
	if (! cell) return true;   // the value --undefined--
	/*
	 * Skip leading white space, in order to separately detect "?" and "--undefined--".
	 */
	while (*cell == U' ' || *cell == U'\t' || *cell == U'\n' || *cell == U'\r') cell ++;
	if (cell [0] == U'\0') return true;   // only white space: the value --undefined--
	if (cell [0] == U'?' || str32nequ (cell, U"--undefined--", 13)) {
		/*
		 * See whether there is anything else besides "?" or "--undefined--" and white space.
		 */
		cell += ( cell [0] == U'?' ) ? 1 : 13;
		while (*cell == U' ' || *cell == U'\t' || *cell == U'\n' || *cell == U'\r') cell ++;
		return *cell == U'\0';   // only white space after the "?" or "--undefined--"
	}
	return Melder_isStringNumeric_nothrow (cell);
}

bool Table_isColumnNumeric_ErrorFalse (Table me, long columnNumber) {
	if (columnNumber < 1 || columnNumber > my numberOfColumns) return false;
	for (long irow = 1; irow <= my rows.size; irow ++) {
		if (! Table_isCellNumeric_ErrorFalse (me, irow, columnNumber)) return false;
	}
	return true;
}

static long stringCompare_column;

static int stringCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	char32 *firstString = my cells [stringCompare_column]. string;
	char32 *secondString = thy cells [stringCompare_column]. string;
	return str32cmp (firstString ? firstString : U"", secondString ? secondString : U"");
}

static void sortRowsByStrings_Assert (Table me, long columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	stringCompare_column = columnNumber;
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), stringCompare_NoError);
}

static int indexCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	if (my sortingIndex < thy sortingIndex) return -1;
	if (my sortingIndex > thy sortingIndex) return +1;
	return 0;
}

static void sortRowsByIndex_NoError (Table me) {
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), indexCompare_NoError);
}

void Table_numericize_Assert (Table me, long columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	if (my columnHeaders [columnNumber]. numericized) return;
	if (Table_isColumnNumeric_ErrorFalse (me, columnNumber)) {
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			const char32 *string = row -> cells [columnNumber]. string;
			row -> cells [columnNumber]. number =
				! string || string [0] == U'\0' || (string [0] == U'?' && string [1] == U'\0') ? undefined :
				Melder_atof (string);
		}
	} else {
		long iunique = 0;
		const char32 *previousString = nullptr;
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		sortRowsByStrings_Assert (me, columnNumber);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			const char32 *string = row -> cells [columnNumber]. string;
			if (! string) string = U"";
			if (! previousString || ! str32equ (string, previousString)) {
				iunique ++;
			}
			row -> cells [columnNumber]. number = iunique;
			previousString = string;
		}
		sortRowsByIndex_NoError (me);
	}
	my columnHeaders [columnNumber]. numericized = true;
}

static void Table_numericize_checkDefined (Table me, long columnNumber) {
	Table_numericize_Assert (me, columnNumber);
	for (long irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (isundef (row -> cells [columnNumber]. number))
			Melder_throw (me, U": the cell in row ", irow,
				U" of column \"", my columnHeaders [columnNumber]. label ? my columnHeaders [columnNumber]. label : Melder_integer (columnNumber),
				U"\" is undefined.");
	}
}

const char32 * Table_getStringValue_Assert (Table me, long rowNumber, long columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	TableRow row = my rows.at [rowNumber];
	return row -> cells [columnNumber]. string ? row -> cells [columnNumber]. string : U"";
}

double Table_getNumericValue_Assert (Table me, long rowNumber, long columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	TableRow row = my rows.at [rowNumber];
	Table_numericize_Assert (me, columnNumber);
	return row -> cells [columnNumber]. number;
}

double Table_getMean (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		real80 sum = 0.0;
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			sum += row -> cells [columnNumber]. number;
		}
		return (real) sum / my rows.size;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute mean of column ", columnNumber, U".");
	}
}

double Table_getMaximum (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		TableRow firstRow = my rows.at [1];
		double maximum = firstRow -> cells [columnNumber]. number;
		for (long irow = 2; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [columnNumber]. number > maximum)
				maximum = row -> cells [columnNumber]. number;
		}
		return maximum;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute maximum of column ", columnNumber, U".");
	}
}

double Table_getMinimum (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		TableRow firstRow = my rows.at [1];
		double minimum = firstRow -> cells [columnNumber]. number;
		for (long irow = 2; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [columnNumber]. number < minimum)
				minimum = row -> cells [columnNumber]. number;
		}
		return minimum;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute minimum of column ", columnNumber, U".");
	}
}

double Table_getGroupMean (Table me, long columnNumber, long groupColumnNumber, const char32 *group) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		long n = 0;
		real80 sum = 0.0;
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_equ (row -> cells [groupColumnNumber]. string, group)) {
				n += 1;
				sum += row -> cells [columnNumber]. number;
			}
		}
		if (n < 1) return undefined;
		real mean = (real) sum / n;
		return mean;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute mean of column ", columnNumber, U" for group \"", group, U"\" of column ", groupColumnNumber, U".");
	}
}

double Table_getQuantile (Table me, long columnNumber, double quantile) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		autoNUMvector <double> sortingColumn (1, my rows.size);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			sortingColumn [irow] = row -> cells [columnNumber]. number;
		}
		NUMsort_d (my rows.size, sortingColumn.peek());
		return NUMquantile (my rows.size, sortingColumn.peek(), quantile);
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute the ", quantile, U" quantile of column ", columnNumber, U".");
	}
}

double Table_getStdev (Table me, long columnNumber) {
	try {
		double mean = Table_getMean (me, columnNumber);   // already checks for columnNumber and undefined cells
		if (my rows.size < 2)
			return undefined;
		real80 sum = 0.0;
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			double d = row -> cells [columnNumber]. number - mean;
			sum += d * d;
		}
		return sqrt ((real) sum / (my rows.size - 1));
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute the standard deviation of column ", columnNumber, U".");
	}
}

long Table_drawRowFromDistribution (Table me, long columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			Melder_throw (me, U": no rows.");
		real80 total = 0.0;
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			total += row -> cells [columnNumber]. number;
		}
		if (total <= 0.0)
			Melder_throw (me, U": the total weight of column ", columnNumber, U" is not positive.");
		long irow;
		do {
			double rand = NUMrandomUniform (0, (real) total);
			real80 sum = 0.0;
			for (irow = 1; irow <= my rows.size; irow ++) {
				TableRow row = my rows.at [irow];
				sum += row -> cells [columnNumber]. number;
				if (rand <= sum) break;
			}
		} while (irow > my rows.size);   // guard against rounding errors
		return irow;
	} catch (MelderError) {
		Melder_throw (me, U": cannot draw a row from the distribution of column ", columnNumber, U".");
	}
}

autoTable Table_extractRowsWhereColumn_number (Table me, long columnNumber, int which_Melder_NUMBER, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_Assert (me, columnNumber);   // extraction should work even if cells are not defined
		autoTable thee = Table_create (0, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			thy columnHeaders [icol]. label = Melder_dup (my columnHeaders [icol]. label);
		}
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which_Melder_NUMBER, criterion)) {
				autoTableRow newRow = Data_copy (row);
				thy rows. addItem_move (newRow.move());
			}
		}
		if (thy rows.size == 0) {
			Melder_warning (U"No row matches criterion.");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTable Table_extractRowsWhereColumn_string (Table me, long columnNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autoTable thee = Table_create (0, my numberOfColumns);
		for (long icol = 1; icol <= my numberOfColumns; icol ++) {
			autostring32 newLabel = Melder_dup (my columnHeaders [icol]. label);
			thy columnHeaders [icol]. label = newLabel.transfer();
		}
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string, which_Melder_STRING, criterion)) {
				autoTableRow newRow = Data_copy (row);
				thy rows. addItem_move (newRow.move());
			}
		}
		if (thy rows.size == 0) {
			Melder_warning (U"No row matches criterion.");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

static void Table_columns_checkExist (Table me, char32 **columnNames, long n) {
	for (long i = 1; i <= n; i ++) {
		if (Table_findColumnIndexFromColumnLabel (me, columnNames [i]) == 0)
			Melder_throw (me, U": column \"", columnNames [i], U"\" does not exist.");
	}
}

static void Table_columns_checkCrossSectionEmpty (char32 **factors, long nfactors, char32 **vars, long nvars) {
	for (long ifactor = 1; ifactor <= nfactors; ifactor ++) {
		for (long ivar = 1; ivar <= nvars; ivar ++) {
			if (str32equ (factors [ifactor], vars [ivar]))
				Melder_throw (U"Factor \"", factors [ifactor], U"\" is also used as dependent variable.");
		}
	}
}

autoTable Table_collapseRows (Table me, const char32 *factors_string, const char32 *columnsToSum_string,
	const char32 *columnsToAverage_string, const char32 *columnsToMedianize_string,
	const char32 *columnsToAverageLogarithmically_string, const char32 *columnsToMedianizeLogarithmically_string)
{
	bool originalChanged = false;
	try {
		Melder_assert (factors_string);

		/*
		 * Parse the six strings of tokens.
		 */
		autoMelderTokens factors (factors_string);
		long numberOfFactors = factors.count();
		if (numberOfFactors < 1)
			Melder_throw (U"In order to pool table data, you must supply at least one independent variable.");
		Table_columns_checkExist (me, factors.peek(), numberOfFactors);

		autoMelderTokens columnsToSum;
		long numberToSum = 0;
		if (columnsToSum_string) {
			columnsToSum.reset (columnsToSum_string);
			numberToSum = columnsToSum.count();
			Table_columns_checkExist (me, columnsToSum.peek(), numberToSum);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToSum.peek(), numberToSum);
		}
		autoMelderTokens columnsToAverage;
		long numberToAverage = 0;
		if (columnsToAverage_string) {
			columnsToAverage.reset (columnsToAverage_string);
			numberToAverage = columnsToAverage.count();
			Table_columns_checkExist (me, columnsToAverage.peek(), numberToAverage);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToAverage.peek(), numberToAverage);
		}
		autoMelderTokens columnsToMedianize;
		long numberToMedianize = 0;
		if (columnsToMedianize_string) {
			columnsToMedianize.reset (columnsToMedianize_string);
			numberToMedianize = columnsToMedianize.count();
			Table_columns_checkExist (me, columnsToMedianize.peek(), numberToMedianize);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToMedianize.peek(), numberToMedianize);
		}
		autoMelderTokens columnsToAverageLogarithmically;
		long numberToAverageLogarithmically = 0;
		if (columnsToAverageLogarithmically_string) {
			columnsToAverageLogarithmically.reset (columnsToAverageLogarithmically_string);
			numberToAverageLogarithmically = columnsToAverageLogarithmically.count();
			Table_columns_checkExist (me, columnsToAverageLogarithmically.peek(), numberToAverageLogarithmically);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToAverageLogarithmically.peek(), numberToAverageLogarithmically);
		}
		autoMelderTokens columnsToMedianizeLogarithmically;
		long numberToMedianizeLogarithmically = 0;
		if (columnsToMedianizeLogarithmically_string) {
			columnsToMedianizeLogarithmically.reset (columnsToMedianizeLogarithmically_string);
			numberToMedianizeLogarithmically = columnsToMedianizeLogarithmically.count();
			Table_columns_checkExist (me, columnsToMedianizeLogarithmically.peek(), numberToMedianizeLogarithmically);
			Table_columns_checkCrossSectionEmpty (factors.peek(), numberOfFactors, columnsToMedianizeLogarithmically.peek(), numberToMedianizeLogarithmically);
		}

		autoTable thee = Table_createWithoutColumnNames (0,
			numberOfFactors + numberToSum + numberToAverage + numberToMedianize + numberToAverageLogarithmically + numberToMedianizeLogarithmically);
		Melder_assert (thy numberOfColumns > 0);

		autoNUMvector <double> sortingColumn;
		if (numberToMedianize > 0 || numberToMedianizeLogarithmically > 0) {
			sortingColumn.reset (1, my rows.size);
		}
		/*
		 * Set the column names. Within the dependent variables, the same name may occur more than once.
		 */
		autoNUMvector <long> columns (1, thy numberOfColumns);
		{
			long icol = 0;
			for (long i = 1; i <= numberOfFactors; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, factors [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, factors [i]);
			}
			for (long i = 1; i <= numberToSum; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToSum [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToSum [i]);
			}
			for (long i = 1; i <= numberToAverage; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToAverage [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverage [i]);
			}
			for (long i = 1; i <= numberToMedianize; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToMedianize [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianize [i]);
			}
			for (long i = 1; i <= numberToAverageLogarithmically; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToAverageLogarithmically [i]);
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverageLogarithmically [i]);
			}
			for (long i = 1; i <= numberToMedianizeLogarithmically; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToMedianizeLogarithmically [i]);
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
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
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
		for (long irow = 1; irow <= my rows.size; irow ++) {
			long rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows.size) break;
				for (long icol = 1; icol <= numberOfFactors; icol ++) {
					if (my rows.at [rowmax] -> cells [columns [icol]]. number !=
						my rows.at [rowmin] -> cells [columns [icol]]. number)
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
			Table_insertRow (thee.get(), thy rows.size + 1);
			{
				long icol = 0;
				for (long i = 1; i <= numberOfFactors; i ++) {
					++ icol;
					Table_setStringValue (thee.get(), thy rows.size, icol,
						my rows.at [rowmin] -> cells [columns [icol]]. string);
				}
				for (long i = 1; i <= numberToSum; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sum += my rows.at [jrow] -> cells [columns [icol]]. number;
					}
					Table_setNumericValue (thee.get(), thy rows.size, icol, sum);
				}
				for (long i = 1; i <= numberToAverage; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sum += my rows.at [jrow] -> cells [columns [icol]]. number;
					}
					Table_setNumericValue (thee.get(), thy rows.size, icol, sum / (rowmax - rowmin + 1));
				}
				for (long i = 1; i <= numberToMedianize; i ++) {
					++ icol;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						sortingColumn [jrow] = my rows.at [jrow] -> cells [columns [icol]]. number;
					}
					NUMsort_d (rowmax - rowmin + 1, & sortingColumn [rowmin - 1]);
					double median = NUMquantile (rowmax - rowmin + 1, & sortingColumn [rowmin - 1], 0.5);
					Table_setNumericValue (thee.get(), thy rows.size, icol, median);
				}
				for (long i = 1; i <= numberToAverageLogarithmically; i ++) {
					++ icol;
					double sum = 0.0;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						double value = my rows.at [jrow] -> cells [columns [icol]]. number;
						if (value <= 0.0)
							Melder_throw (
								U"The cell in column \"", columnsToAverageLogarithmically [i],
								U"\" of row ", jrow, U" of ", me,
								U" is not positive.\nCannot average logarithmically.");
						sum += log (value);
					}
					Table_setNumericValue (thee.get(), thy rows.size, icol, exp (sum / (rowmax - rowmin + 1)));
				}
				for (long i = 1; i <= numberToMedianizeLogarithmically; i ++) {
					++ icol;
					for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
						double value = my rows.at [jrow] -> cells [columns [icol]]. number;
						if (value <= 0.0)
							Melder_throw (
								U"The cell in column \"", columnsToMedianizeLogarithmically [i],
								U"\" of row ", jrow, U" of ", me,
								U" is not positive.\nCannot medianize logarithmically.");
						sortingColumn [jrow] = log (value);
					}
					NUMsort_d (rowmax - rowmin + 1, & sortingColumn [rowmin - 1]);
					double median = NUMquantile (rowmax - rowmin + 1, & sortingColumn [rowmin - 1], 0.5);
					Table_setNumericValue (thee.get(), thy rows.size, icol, exp (median));
				}
				Melder_assert (icol == thy numberOfColumns);
			}
			irow = rowmax;
		}
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table
		return thee;
	} catch (MelderError) {
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
	}
}

static char32 ** _Table_getLevels (Table me, long column, long *numberOfLevels) {
	try {
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		long columns [2] = { 0, column };
		Table_sortRows_Assert (me, columns, 1);
		*numberOfLevels = 0;
		long irow = 1;
		while (irow <= my rows.size) {
			double value = my rows.at [irow] -> cells [column]. number;
			(*numberOfLevels) ++;
			while (++ irow <= my rows.size && my rows.at [irow] -> cells [column]. number == value) { }
		}
		autostring32vector result (1, *numberOfLevels);
		*numberOfLevels = 0;
		irow = 1;
		while (irow <= my rows.size) {
			double value = my rows.at [irow] -> cells [column]. number;
			result [++ *numberOfLevels] = Melder_dup (Table_getStringValue_Assert (me, irow, column));
			while (++ irow <= my rows.size && my rows.at [irow] -> cells [column]. number == value) { }
		}
		sortRowsByIndex_NoError (me);   // unsort the original table
		return result.transfer();
	} catch (MelderError) {
		sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
	}
}

autoTable Table_rowsToColumns (Table me, const char32 *factors_string, long columnToTranspose, const char32 *columnsToExpand_string) {
	bool originalChanged = false;
	try {
		Melder_assert (factors_string);

		bool warned = false;
		/*
		 * Parse the two strings of tokens.
		 */
		autoMelderTokens factors_names (factors_string);
		long numberOfFactors = factors_names.count();
		if (numberOfFactors < 1)
			Melder_throw (U"In order to nest table data, you must supply at least one independent variable.");
		Table_columns_checkExist (me, factors_names.peek(), numberOfFactors);
		autoMelderTokens columnsToExpand_names (columnsToExpand_string);
		long numberToExpand = columnsToExpand_names.count();
		if (numberToExpand < 1)
			Melder_throw (U"In order to nest table data, you must supply at least one dependent variable (to expand).");
		Table_columns_checkExist (me, columnsToExpand_names.peek(), numberToExpand);
		Table_columns_checkCrossSectionEmpty (factors_names.peek(), numberOfFactors, columnsToExpand_names.peek(), numberToExpand);
		long numberOfLevels = 0;
		char32 ** dummy = _Table_getLevels (me, columnToTranspose, & numberOfLevels);
		autostring32vector levels_names (dummy, 1, numberOfLevels);
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
			Table_setColumnLabel (thee.get(), ifactor, factors_names [ifactor]);
		}
		for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			for (long ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				//Melder_casual (U"Number of factors: ", numberOfFactors);
				//Melder_casual (U"Level: ", ilevel, U" out of ", numberOfLevels);
				long columnNumber = numberOfFactors + (iexpand - 1) * numberOfLevels + ilevel;
				//Melder_casual (U"Column number: ", columnNumber);
				Table_setColumnLabel (thee.get(), columnNumber, Melder_cat (columnsToExpand_names [iexpand], U".", levels_names [ilevel]));
			}
		}
		/*
		 * Remember the present sorting of the original table.
		 * (This is safe: the sorting index may change only vacuously when numericizing.)
		 * But this cannot be done before the previous blocks that numericize!
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
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
		for (long irow = 1; irow <= my rows.size; irow ++) {
			long rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows.size) break;
				for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
					if (my rows.at [rowmax] -> cells [factorColumns [ifactor]]. number !=
						my rows.at [rowmin] -> cells [factorColumns [ifactor]]. number)
					{
						identical = false;
						break;
					}
				}
				if (! identical) break;
			}
			#if 0
			if (rowmax - rowmin > numberOfLevels && ! warned) {
				Melder_warning (U"Some rows of the original table have not been included in the new table. "
					U"You could perhaps add more factors.");
				warned = true;
			}
			#endif
			rowmax --;
			/*
			 * We have the stretch.
			 */
			Table_insertRow (thee.get(), thy rows.size + 1);
			TableRow thyRow = thy rows.at [thy rows.size];
			for (long ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
				Table_setStringValue (thee.get(), thy rows.size, ifactor,
					my rows.at [rowmin] -> cells [factorColumns [ifactor]]. string);
			}
			for (long iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
				for (long jrow = rowmin; jrow <= rowmax; jrow ++) {
					TableRow myRow = my rows.at [jrow];
					double value = myRow -> cells [columnsToExpand [iexpand]]. number;
					long level = lround (myRow -> cells [columnToTranspose]. number);
					long thyColumn = numberOfFactors + (iexpand - 1) * numberOfLevels + level;
					if (thyRow -> cells [thyColumn]. string && ! warned) {
						Melder_warning (U"Some information from the original table has not been included in the new table. "
							U"You could perhaps add more factors.");
						warned = true;
					}
					Table_setNumericValue (thee.get(), thy rows.size, thyColumn, value);
				}
			}
			irow = rowmax;
		}
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table
		return thee;
	} catch (MelderError) {
		if (originalChanged) sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
	}
}

autoTable Table_transpose (Table me) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my numberOfColumns, 1 + my rows.size);
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				Table_setStringValue (thee.get(), icol, 1, my columnHeaders [icol]. label);
			}
		for (long irow = 1; irow <= my rows.size; irow ++) {
			for (long icol = 1; icol <= my numberOfColumns; icol ++) {
				Table_setStringValue (thee.get(), icol, 1 + irow, Table_getStringValue_Assert (me, irow, icol));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not transposed.");
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
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), cellCompare_NoError);
}

void Table_sortRows_string (Table me, const char32 *columns_string) {
	try {
		autoMelderTokens columns_tokens (columns_string);
		long numberOfColumns = columns_tokens.count();
		if (numberOfColumns < 1)
			Melder_throw (me, U": you specified an empty list of columns.");
		autoNUMvector <long> columns (1, numberOfColumns);
		for (long icol = 1; icol <= numberOfColumns; icol ++) {
			columns [icol] = Table_findColumnIndexFromColumnLabel (me, columns_tokens [icol]);
			if (columns [icol] == 0)
				Melder_throw (U"Column \"", columns_tokens [icol], U"\" does not exist.");
		}
		Table_sortRows_Assert (me, columns.peek(), numberOfColumns);
	} catch (MelderError) {
		Melder_throw (me, U": rows not sorted.");
	}
}

void Table_randomizeRows (Table me) noexcept {
	for (long irow = 1; irow <= my rows.size; irow ++) {
		long jrow = NUMrandomInteger (irow, my rows.size);
		TableRow tmp = my rows.at [irow];
		my rows.at [irow] = my rows.at [jrow];
		my rows.at [jrow] = tmp;
	}
}

void Table_reflectRows (Table me) noexcept {
	for (long irow = 1; irow <= my rows.size / 2; irow ++) {
		long jrow = my rows.size + 1 - irow;
		TableRow tmp = my rows.at [irow];
		my rows.at [irow] = my rows.at [jrow];
		my rows.at [jrow] = tmp;
	}
}

autoTable Tables_append (OrderedOf<structTable>* me) {
	try {
		if (my size == 0) Melder_throw (U"Cannot add zero tables.");
		Table thee = my at [1];
		long nrow = thy rows.size;
		long ncol = thy numberOfColumns;
		Table firstTable = thee;
		for (long itab = 2; itab <= my size; itab ++) {
			thee = my at [itab];
			nrow += thy rows.size;
			if (thy numberOfColumns != ncol)
				Melder_throw (U"Numbers of columns do not match.");
			for (long icol = 1; icol <= ncol; icol ++) {
				if (! Melder_equ (thy columnHeaders [icol]. label, firstTable -> columnHeaders [icol]. label))
					Melder_throw (U"The label of column ", icol, U" of ", thee,
						U" (", thy columnHeaders [icol]. label, U") does not match the label of column ", icol,
						U" of ", firstTable, U" (", firstTable -> columnHeaders [icol]. label, U").");
			}
		}
		autoTable him = Table_createWithoutColumnNames (nrow, ncol);
		for (long icol = 1; icol <= ncol; icol ++) {
			Table_setColumnLabel (him.get(), icol, thy columnHeaders [icol]. label);
		}
		nrow = 0;
		for (long itab = 1; itab <= my size; itab ++) {
			thee = my at [itab];
			for (long irow = 1; irow <= thy rows.size; irow ++) {
				nrow ++;
				for (long icol = 1; icol <= ncol; icol ++) {
					Table_setStringValue (him.get(), nrow, icol, Table_getStringValue_Assert (thee, irow, icol));
				}
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Table objects not appended.");
	}
}

void Table_appendSumColumn (Table me, long column1, long column2, const char32 *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1, myRow -> cells [column1]. number + myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (! myCell -> string);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = nullptr;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, U": sum column not appended.");
	}
}

void Table_appendDifferenceColumn (Table me, long column1, long column2, const char32 *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1, myRow -> cells [column1]. number - myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (! myCell -> string);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = nullptr;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, U": difference column not appended.");
	}
}

void Table_appendProductColumn (Table me, long column1, long column2, const char32 *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1, myRow -> cells [column1]. number * myRow -> cells [column2]. number);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (! myCell -> string);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = nullptr;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, U": product column not appended.");
	}
}

void Table_appendQuotientColumn (Table me, long column1, long column2, const char32 *label) {   // safe
	try {
		/*
		 * Check without change.
		 */
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			double value = myRow -> cells [column2]. number == 0.0 ? undefined :
				myRow -> cells [column1]. number / myRow -> cells [column2]. number;
			Table_setNumericValue (thee.get(), irow, 1, value);
		}
		/*
		 * Safe change.
		 */
		Table_appendColumn (me, label);
		/*
		 * Change without error.
		 */
		for (long irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			TableCell myCell = & myRow -> cells [my numberOfColumns];
			TableCell thyCell = & thyRow -> cells [1];
			Melder_assert (! myCell -> string);   // make room...
			myCell -> string = thyCell -> string;   // ...fill in and dangle...
			thyCell -> string = nullptr;   // ...undangle
		}
	} catch (MelderError) {
		Melder_throw (me, U": quotient column not appended.");
	}
}

void Table_formula_columnRange (Table me, long fromColumn, long toColumn, const char32 *expression, Interpreter interpreter) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, fromColumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, toColumn);
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, true);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			for (integer icol = fromColumn; icol <= toColumn; icol ++) {
				Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING) {
					Table_setStringValue (me, irow, icol, result. result.stringResult);
					Melder_free (result. result.stringResult);
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
					Table_setNumericValue (me, irow, icol, result. result.numericResult);
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
					Melder_throw (me, U": cannot put vectors into cells.");
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
					Melder_throw (me, U": cannot put matrices into cells.");
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING_ARRAY) {
					Melder_throw (me, U": cannot put string arrays into cells.");
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": application of formula not completed.");
	}
}

void Table_formula (Table me, long icol, const char32 *expression, Interpreter interpreter) {
	Table_formula_columnRange (me, icol, icol, expression, interpreter);
}

double Table_getCorrelation_pearsonR (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	long n = my rows.size, irow;
	double correlation;
	double sum1 = 0.0, sum2 = 0.0, sum12 = 0.0, sum11 = 0.0, sum22 = 0.0, mean1, mean2;
	if (out_significance) *out_significance = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	if (column1 < 1 || column1 > my numberOfColumns) return undefined;
	if (column2 < 1 || column2 > my numberOfColumns) return undefined;
	if (n < 2) return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		sum1 += row -> cells [column1]. number;
		sum2 += row -> cells [column2]. number;
	}
	mean1 = sum1 / n;
	mean2 = sum2 / n;
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		double d1 = row -> cells [column1]. number - mean1, d2 = row -> cells [column2]. number - mean2;
		sum12 += d1 * d2;
		sum11 += d1 * d1;
		sum22 += d2 * d2;
	}
	correlation = sum11 == 0.0 || sum22 == 0.0 ? undefined : sum12 / sqrt (sum11 * sum22);
	if (out_significance && isdefined (correlation) && n >= 3)
		*out_significance = fabs (correlation) == 1.0 ? 0.0 :
			/* One-sided: */
			NUMstudentQ (fabs (correlation) * sqrt ((n - 2) / (1 - correlation * correlation)), n - 2);
	if ((out_lowerLimit || out_upperLimit) && isdefined (correlation) && n >= 4) {
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
	long n = my rows.size, irow, jrow;
	double correlation, denominator;
	long numberOfConcordants = 0, numberOfDiscordants = 0, numberOfExtra1 = 0, numberOfExtra2 = 0;
	if (out_significance) *out_significance = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	if (column1 < 1 || column1 > my numberOfColumns) return undefined;
	if (column2 < 1 || column2 > my numberOfColumns) return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (irow = 1; irow < n; irow ++) {
		TableRow rowi = my rows.at [irow];
		for (jrow = irow + 1; jrow <= n; jrow ++) {
			TableRow rowj = my rows.at [jrow];
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
	correlation = denominator == 0.0 ? undefined : (numberOfConcordants - numberOfDiscordants) / denominator;
	if ((out_significance || out_lowerLimit || out_upperLimit) && isdefined (correlation) && n >= 2) {
		double standardError = sqrt ((4 * n + 10.0) / (9 * n * (n - 1)));
		if (out_significance)
			*out_significance = NUMgaussQ (fabs (correlation) / standardError);   // one-sided
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
	if (out_t) *out_t = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significance) *out_significance = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	integer n = my rows.size;
	if (n < 1) return undefined;
	if (column1 < 1 || column1 > my numberOfColumns) return undefined;
	if (column2 < 1 || column2 > my numberOfColumns) return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	real80 sum = 0.0;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		sum += row -> cells [column1]. number - row -> cells [column2]. number;
	}
	real meanDifference = (real) sum / n;
	integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		real80 sumOfSquares = 0.0;
		for (integer irow = 1; irow <= n; irow ++) {
			TableRow row = my rows.at [irow];
			real diff = (row -> cells [column1]. number - row -> cells [column2]. number) - meanDifference;
			sumOfSquares += diff * diff;
		}
		real standardError = sqrt ((real) sumOfSquares / degreesOfFreedom / n);
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
	integer n = my rows.size;
	if (out_tFromZero) *out_tFromZero = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero) *out_significanceFromZero = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	if (n < 1) return undefined;
	if (column < 1 || column > my numberOfColumns) return undefined;
	integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	Table_numericize_Assert (me, column);
	real80 sum = 0.0;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		sum += row -> cells [column]. number;
	}
	real mean = real (sum / n);
	if (n >= 2 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		real80 sumOfSquares = 0.0;
		for (integer irow = 1; irow <= n; irow ++) {
			TableRow row = my rows.at [irow];
			real diff = row -> cells [column]. number - mean;
			sumOfSquares += diff * diff;
		}
		real standardError = sqrt ((real) sumOfSquares / degreesOfFreedom / n);
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

double Table_getGroupMean_studentT (Table me, long column, long groupColumn, const char32 *group, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero) *out_significanceFromZero = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	if (column < 1 || column > my numberOfColumns) return undefined;
	Table_numericize_Assert (me, column);
	integer n = 0;
	real80 sum = 0.0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string, group)) {
				n += 1;
				sum += row -> cells [column]. number;
			}
		}
	}
	if (n < 1) return undefined;
	real mean = (real) sum / n;
	integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		real80 sumOfSquares = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [groupColumn]. string) {
				if (str32equ (row -> cells [groupColumn]. string, group)) {
					real diff = row -> cells [column]. number - mean;
					sumOfSquares += diff * diff;
				}
			}
		}
		real standardError = sqrt ((real) sumOfSquares / degreesOfFreedom / n);
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

double Table_getGroupDifference_studentT (Table me, long column, long groupColumn, const char32 *group1, const char32 *group2, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero) *out_tFromZero = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero) *out_significanceFromZero = undefined;
	if (out_lowerLimit) *out_lowerLimit = undefined;
	if (out_upperLimit) *out_upperLimit = undefined;
	if (column < 1 || column > my numberOfColumns) return undefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns) return undefined;
	Table_numericize_Assert (me, column);
	integer n1 = 0, n2 = 0;
	real80 sum1 = 0.0, sum2 = 0.0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string, group1)) {
				n1 ++;
				sum1 += row -> cells [column]. number;
			} else if (str32equ (row -> cells [groupColumn]. string, group2)) {
				n2 ++;
				sum2 += row -> cells [column]. number;
			}
		}
	}
	if (n1 < 1 || n2 < 1) return undefined;
	integer degreesOfFreedom = n1 + n2 - 2;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = degreesOfFreedom;
	real mean1 = (real) sum1 / n1;
	real mean2 = (real) sum2 / n2;
	real difference = mean1 - mean2;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		real80 sumOfSquares = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [groupColumn]. string) {
				if (str32equ (row -> cells [groupColumn]. string, group1)) {
					real diff = row -> cells [column]. number - mean1;
					sumOfSquares += diff * diff;
				} else if (str32equ (row -> cells [groupColumn]. string, group2)) {
					real diff = row -> cells [column]. number - mean2;
					sumOfSquares += diff * diff;
				}
			}
		}
		real standardError = sqrt ((real) sumOfSquares / degreesOfFreedom * (1.0 / n1 + 1.0 / n2));
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

double Table_getGroupDifference_wilcoxonRankSum (Table me, long column, long groupColumn, const char32 *group1, const char32 *group2,
	double *out_rankSum, double *out_significanceFromZero)
{
	if (out_rankSum) *out_rankSum = undefined;
	if (out_significanceFromZero) *out_significanceFromZero = undefined;
	if (column < 1 || column > my numberOfColumns) return undefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns) return undefined;
	Table_numericize_Assert (me, column);
	integer n1 = 0, n2 = 0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string, group1)) {
				n1 ++;
			} else if (str32equ (row -> cells [groupColumn]. string, group2)) {
				n2 ++;
			}
		}
	}
	integer n = n1 + n2;
	if (n1 < 1 || n2 < 1 || n < 3) return undefined;
	autoTable ranks = Table_createWithoutColumnNames (n, 3);   // column 1 = group, 2 = value, 3 = rank
	for (integer irow = 1, jrow = 0; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string, group1)) {
				Table_setNumericValue (ranks.get(), ++ jrow, 1, 1.0);
				Table_setNumericValue (ranks.get(), jrow, 2, row -> cells [column]. number);
			} else if (str32equ (row -> cells [groupColumn]. string, group2)) {
				Table_setNumericValue (ranks.get(), ++ jrow, 1, 2.0);
				Table_setNumericValue (ranks.get(), jrow, 2, row -> cells [column]. number);
			}
		}
	}
	Table_numericize_Assert (ranks.get(), 1);
	Table_numericize_Assert (ranks.get(), 2);
	Table_numericize_Assert (ranks.get(), 3);
	integer columns [1+1] = { 0, 2 };   // we're gonna sort by column 2
	Table_sortRows_Assert (ranks.get(), columns, 1);   // we sort by one column only
	double totalNumberOfTies3 = 0.0;
	for (integer irow = 1; irow <= ranks -> rows.size; irow ++) {
		TableRow row = ranks -> rows.at [irow];
		double value = row -> cells [2]. number;
		long rowOfLastTie = irow + 1;
		for (; rowOfLastTie <= ranks -> rows.size; rowOfLastTie ++) {
			TableRow row2 = ranks -> rows.at [rowOfLastTie];
			double value2 = row2 -> cells [2]. number;
			if (value2 != value) break;
		}
		rowOfLastTie --;
		double averageRank = 0.5 * ((double) irow + (double) rowOfLastTie);
		for (integer jrow = irow; jrow <= rowOfLastTie; jrow ++) {
			Table_setNumericValue (ranks.get(), jrow, 3, averageRank);
		}
		long numberOfTies = rowOfLastTie - irow + 1;
		totalNumberOfTies3 += (double) (numberOfTies - 1) * (double) numberOfTies * (double) (numberOfTies + 1);
	}
	Table_numericize_Assert (ranks.get(), 3);
	double maximumRankSum = (double) n1 * (double) n2;
	real80 rankSum = 0.0;
	for (integer irow = 1; irow <= ranks -> rows.size; irow ++) {
		TableRow row = ranks -> rows.at [irow];
		if (row -> cells [1]. number == 1.0) rankSum += row -> cells [3]. number;
	}
	rankSum -= 0.5 * (double) n1 * ((double) n1 + 1.0);
	double stdev = sqrt (maximumRankSum * ((double) n + 1.0 - totalNumberOfTies3 / n / (n - 1)) / 12.0);
	if (out_rankSum) *out_rankSum = (real) rankSum;
	if (out_significanceFromZero) *out_significanceFromZero = NUMgaussQ (fabs ((real) rankSum - 0.5 * maximumRankSum) / stdev);
	return (real) rankSum / maximumRankSum;
}

double Table_getFisherF (Table me, long col1, long col2);
double Table_getOneWayAnovaSignificance (Table me, long col1, long col2);
double Table_getFisherFLowerLimit (Table me, long col1, long col2, double significanceLevel);
double Table_getFisherFUpperLimit (Table me, long col1, long col2, double significanceLevel);

bool Table_getExtrema (Table me, long icol, double *minimum, double *maximum) {
	integer n = my rows.size;
	if (icol < 1 || icol > my numberOfColumns || n == 0) {
		*minimum = *maximum = undefined;
		return false;
	}
	Table_numericize_Assert (me, icol);
	*minimum = *maximum = my rows.at [1] -> cells [icol]. number;
	for (integer irow = 2; irow <= n; irow ++) {
		double value = my rows.at [irow] -> cells [icol]. number;
		if (value < *minimum) *minimum = value;
		if (value > *maximum) *maximum = value;
	}
	return true;
}

void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const char32 *mark, int garnish)
{
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
	integer n = my rows.size;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		Graphics_mark (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, markSize_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label);
		Graphics_marksLeft (g, 2, true, true, false);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label);
	}
}

void Table_scatterPlot (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long markColumn, int fontSize, int garnish)
{
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
	integer n = my rows.size;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		const char32 *mark = row -> cells [markColumn]. string;
		if (mark)
			Graphics_text (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, mark);
	}
	Graphics_setFontSize (g, saveFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label);
		Graphics_marksLeft (g, 2, true, true, false);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label);
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
		autoTableOfReal tableOfReal = TableOfReal_create (my rows.size, 2);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			tableOfReal -> data [irow] [1] = Table_getNumericValue_Assert (me, irow, xcolumn);
			tableOfReal -> data [irow] [2] = Table_getNumericValue_Assert (me, irow, ycolumn);
		}
		autoSSCP sscp = TableOfReal_to_SSCP (tableOfReal.get(), 0, 0, 0, 0);
		SSCP_drawConcentrationEllipse (sscp.get(), g, numberOfSigmas, 0, 1, 2, xmin, xmax, ymin, ymax, garnish);
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

static const char32 *visibleString (const char32 *s) {
	return ( s && s [0] != U'\0' ? s : U"?" );
}

void Table_list (Table me, bool includeRowNumbers) {
	MelderInfo_open ();
	if (includeRowNumbers) {
		MelderInfo_write (U"row");
		if (my numberOfColumns > 0) MelderInfo_write (U"\t");
	}
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol > 1) MelderInfo_write (U"\t");
		MelderInfo_write (visibleString (my columnHeaders [icol]. label));
	}
	MelderInfo_write (U"\n");
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		if (includeRowNumbers) {
			MelderInfo_write (irow);
			if (my numberOfColumns > 0) MelderInfo_write (U"\t");
		}
		TableRow row = my rows.at [irow];
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol > 1) MelderInfo_write (U"\t");
			MelderInfo_write (visibleString (row -> cells [icol]. string));
		}
		MelderInfo_write (U"\n");
	}
	MelderInfo_close ();
}

static void _Table_writeToCharacterSeparatedFile (Table me, MelderFile file, char32 kar) {
	autoMelderString buffer;
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol != 1) MelderString_appendCharacter (& buffer, kar);
		char32 *s = my columnHeaders [icol]. label;
		MelderString_append (& buffer, ( s && s [0] != U'\0' ? s : U"?" ));
	}
	MelderString_appendCharacter (& buffer, U'\n');
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol != 1) MelderString_appendCharacter (& buffer, kar);
			char32 *s = row -> cells [icol]. string;
			MelderString_append (& buffer, ( s && s [0] != U'\0' ? s : U"?" ));
		}
		MelderString_appendCharacter (& buffer, U'\n');
	}
	MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
}

void Table_writeToTabSeparatedFile (Table me, MelderFile file) {
	try {
		_Table_writeToCharacterSeparatedFile (me, file, U'\t');
	} catch (MelderError) {
		Melder_throw (me, U": not written to tab-separated file.");
	}
}

void Table_writeToCommaSeparatedFile (Table me, MelderFile file) {
	try {
		_Table_writeToCharacterSeparatedFile (me, file, U',');
	} catch (MelderError) {
		Melder_throw (me, U": not written to comma-separated file.");
	}
}

autoTable Table_readFromTableFile (MelderFile file) {
	try {
		autostring32 string = MelderFile_readText (file);
		/*
		 * Count columns.
		 */
		integer ncol = 0;
		char32 *p = & string [0];
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\n' || kar == U'\0') break;
			if (kar == U' ' || kar == U'\t') continue;
			ncol ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\n' || kar == U'\0') break;
		}
		if (ncol < 1) Melder_throw (U"No columns.");

		/*
		 * Count elements.
		 */
		p = & string [0];
		integer nelements = 0;
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0') break;
			if (kar == U' ' || kar == U'\t' || kar == U'\n') continue;
			nelements ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\0') break;
		}

		/*
		 * Check if all columns are complete.
		 */
		if (nelements == 0 || nelements % ncol != 0)
			Melder_throw (U"The number of elements (", nelements, U") is not a multiple of the number of columns (", ncol, U").");

		/*
		 * Create empty table.
		 */
		integer nrow = nelements / ncol - 1;
		autoTable me = Table_create (nrow, ncol);

		/*
		 * Read elements.
		 */
		p = & string [0];
		for (integer icol = 1; icol <= ncol; icol ++) {
			while (*p == U' ' || *p == U'\t') { Melder_assert (*p != U'\0'); p ++; }
			static MelderString buffer { };
			MelderString_empty (& buffer);
			while (*p != U' ' && *p != U'\t' && *p != U'\n') { MelderString_appendCharacter (& buffer, *p); p ++; }
			Table_setColumnLabel (me.get(), icol, buffer.string);
			MelderString_empty (& buffer);
		}
		for (integer irow = 1; irow <= nrow; irow ++) {
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= ncol; icol ++) {
				while (*p == U' ' || *p == U'\t' || *p == U'\n') { Melder_assert (*p != U'\0'); p ++; }
				static MelderString buffer { };
				MelderString_empty (& buffer);
				while (*p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\0') { MelderString_appendCharacter (& buffer, *p); p ++; }
				row -> cells [icol]. string = Melder_dup_f (buffer.string);
				MelderString_empty (& buffer);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table object not read from space-separated text file ", file, U".");
	}
}

autoTable Table_readFromCharacterSeparatedTextFile (MelderFile file, char32 separator) {
	try {
		autostring32 string = MelderFile_readText (file);

		/*
		 * Kill final new-line symbols.
		 */
		for (int64 length = str32len (string.peek());
		     length > 0 && string [length - 1] == U'\n';
			 length = str32len (string.peek()))
		{
			string [length - 1] = U'\0';
		}

		/*
		 * Count columns.
		 */
		integer ncol = 1;
		const char32 *p = & string [0];
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0') Melder_throw (U"No rows.");
			if (kar == U'\n') break;
			if (kar == separator) ncol ++;
		}

		/*
		 * Count rows.
		 */
		integer nrow = 1;
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0') break;
			if (kar == U'\n') nrow ++;
		}

		/*
		 * Create empty table.
		 */
		autoTable me = Table_create (nrow, ncol);

		/*
		 * Read column names.
		 */
		autoMelderString buffer;
		p = & string [0];
		for (integer icol = 1; icol <= ncol; icol ++) {
			MelderString_empty (& buffer);
			while (*p != separator && *p != U'\n') {
				Melder_assert (*p != U'\0');
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			p ++;
			Table_setColumnLabel (me.get(), icol, buffer.string);
		}

		/*
		 * Read cells.
		 */
		for (integer irow = 1; irow <= nrow; irow ++) {
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= ncol; icol ++) {
				MelderString_empty (& buffer);
				while (*p != separator && *p != U'\n' && *p != U'\0') {
					MelderString_appendCharacter (& buffer, *p);
					p ++;
				}
				if (*p == U'\0') {
					if (irow != nrow) Melder_fatal (U"irow ", irow, U", nrow ", nrow, U", icol ", icol, U", ncol ", ncol);
					if (icol != ncol) Melder_throw (U"Last row incomplete.");
				} else if (*p == U'\n') {
					if (icol != ncol) Melder_throw (U"Row ", irow, U" incomplete.");
					p ++;
				} else {
					Melder_assert (*p == separator);
					p ++;
				}
				row -> cells [icol]. string = Melder_dup (buffer.string);   // BUG? could be null
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table object not read from character-separated text file ", file, U".");
	}
}

/* End of file Table.cpp */
