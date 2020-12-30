/* Table.cpp
 *
 * Copyright (C) 2002-2020 Paul Boersma
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

conststring32 structTable :: v_getColStr (integer columnNumber) {
	if (columnNumber < 1 || columnNumber > our numberOfColumns)
		return nullptr;
	return our columnHeaders [columnNumber]. label ? our columnHeaders [columnNumber]. label.get() : U"";
}

double structTable :: v_getMatrix (integer rowNumber, integer columnNumber) {
	if (rowNumber < 1 || rowNumber > our rows.size)
		return undefined;
	if (columnNumber < 1 || columnNumber > our numberOfColumns)
		return undefined;
	conststring32 stringValue = our rows.at [rowNumber] -> cells [columnNumber]. string.get();
	return stringValue ? Melder_atof (stringValue) : undefined;
}

conststring32 structTable :: v_getMatrixStr (integer rowNumber, integer columnNumber) {
	if (rowNumber < 1 || rowNumber > our rows.size)
		return U"";
	if (columnNumber < 1 || columnNumber > our numberOfColumns)
		return U"";
	conststring32 stringValue = our rows.at [rowNumber] -> cells [columnNumber]. string.get();
	return stringValue ? stringValue : U"";
}

double structTable :: v_getColIndex (conststring32 columnLabel) {
	return Table_findColumnIndexFromColumnLabel (this, columnLabel);
}

static autoTableRow TableRow_create (integer numberOfColumns) {
	autoTableRow me = Thing_new (TableRow);
	my numberOfColumns = numberOfColumns;
	my cells = newvectorzero <structTableCell> (numberOfColumns);
	return me;
}

void Table_initWithoutColumnNames (Table me, integer numberOfRows, integer numberOfColumns) {
	if (numberOfColumns < 1)
		Melder_throw (U"Cannot create table without columns.");
	my numberOfColumns = numberOfColumns;
	my columnHeaders = newvectorzero <structTableColumnHeader> (numberOfColumns);
	for (integer irow = 1; irow <= numberOfRows; irow ++)
		Table_appendRow (me);
}

autoTable Table_createWithoutColumnNames (integer numberOfRows, integer numberOfColumns) {
	try {
		autoTable me = Thing_new (Table);
		Table_initWithoutColumnNames (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}
}

conststring32 Table_messageColumn (Table me, integer column) {
	if (column >= 1 && column <= my numberOfColumns && my columnHeaders [column]. label && my columnHeaders [column]. label [0] != U'\0')
		return Melder_cat (U"\"", my columnHeaders [column]. label.get(), U"\"");
	else
		return Melder_integer (column);
}

void Table_initWithColumnNames (Table me, integer numberOfRows, conststring32 columnNames_string) {
	autoSTRVEC columnNames = splitByWhitespace_STRVEC (columnNames_string);
	Table_initWithoutColumnNames (me, numberOfRows, columnNames.size);
	for (integer icol = 1; icol <= columnNames.size; icol ++)
		Table_setColumnLabel (me, icol, columnNames [icol].get());
}

autoTable Table_createWithColumnNames (integer numberOfRows, conststring32 columnNames) {
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

void Table_appendColumn (Table me, conststring32 label) {
	try {
		Table_insertColumn (me, my numberOfColumns + 1, label);
	} catch (MelderError) {
		Melder_throw (me, U": column \"", label, U"\" not appended.");
	}
}

void Table_checkSpecifiedRowNumberWithinRange (Table me, integer rowNumber) {
	Melder_require (rowNumber >= 1,
		me, U": the requested row number is ", rowNumber, U", but should be at least 1.");
	Melder_require (rowNumber <= my rows.size,
		me, U": the requested row number (", rowNumber, U") exceeds my number of rows (", my rows.size, U").");
}

void Table_removeRow (Table me, integer rowNumber) {
	try {
		if (my rows.size == 1)
			Melder_throw (me, U": cannot remove my only row.");
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		my rows. removeItem (rowNumber);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": row ", rowNumber, U" not removed.");
	}
}

void Table_checkSpecifiedColumnNumberWithinRange (Table me, integer columnNumber) {
	Melder_require (columnNumber >= 1,
		me, U": the requested column number is ", columnNumber, U", but should be at least 1.");
	Melder_require (columnNumber <= my numberOfColumns,
		me, U": the requested column number is ", columnNumber, U", but should be at most my number of columns (", my numberOfColumns, U").");
}

void Table_removeColumn (Table me, integer columnNumber) {
	try {
		Melder_require (my numberOfColumns > 1,
			me, U": cannot remove my only column.");
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		/*
			Changes without error.
		*/
		my columnHeaders [columnNumber]. destroy ();
		my columnHeaders. remove (columnNumber);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> cells [columnNumber]. destroy ();
			row -> cells. remove (columnNumber);
			row -> numberOfColumns --;   // maintain invariant
		}
		my numberOfColumns --;   // maintain invariant
	} catch (MelderError) {
		Melder_throw (me, U": column ", Table_messageColumn (me, columnNumber), U" not removed.");
	}
}

void Table_insertRow (Table me, integer rowNumber) {
	try {
		/*
			Strong exception safety, step 1: check and create without changing me.
		*/
		Melder_require (rowNumber >= 1,
			me, U": the requested row number is ", rowNumber, U", but should be at least 1.");
		Melder_require (rowNumber <= my rows.size + 1,
			me, U": the requested row number is ", rowNumber, U", but should be at most my number of rows (", my rows.size, U") plus 1.");
		autoTableRow row = TableRow_create (my numberOfColumns);
		/*
			Strong exception safety, recursive intermediate step:
			call at most one function that has strong exception safety itself.
			The following function call is indeed a safe change,
			i.e., in case of an error, me is not changed.
		*/
		my rows. addItemAtPosition_move (row.move(), rowNumber);
		/*
			Strong exception safety, step 2: perform changes to me without any risk of error.
		*/
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			my columnHeaders [icol]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": row ", rowNumber, U" not inserted.");
	}
}

void Table_insertColumn (Table me, integer columnNumber, conststring32 label /* cattable */) {
	try {
		/*
			Strong exception safety, step 1: check and create without changing me.
		*/
		Melder_require (columnNumber >= 1,
			me, U": the requested column number is ", columnNumber, U", but should be at least 1.");
		Melder_require (columnNumber <= my numberOfColumns + 1,
			me, U": the requested column number is ", columnNumber, U", but should be at most my number of columns (", my numberOfColumns, U") plus 1.");
		autostring32 newLabel = Melder_dup (label);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, my numberOfColumns + 1);
		/*
			Changes without error.
		*/
		/*
			Transfer column headers to larger structure.
		*/
		for (integer icol = 1; icol < columnNumber; icol ++)
			thy columnHeaders [icol] = std::move (my columnHeaders [icol]);
		thy columnHeaders [columnNumber]. label = newLabel.move();
		thy columnHeaders [columnNumber]. numericized = false;
		for (integer icol = my numberOfColumns + 1; icol > columnNumber; icol --)
			thy columnHeaders [icol] = std::move (my columnHeaders [icol - 1]);
		/*
			Transfer rows to larger structure.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			TableRow thyRow = thy rows.at [irow];
			for (integer icol = 1; icol < columnNumber; icol ++)
				thyRow -> cells [icol] = std::move (myRow -> cells [icol]);
			Melder_assert (! thyRow -> cells [columnNumber]. string);
			Melder_assert (thyRow -> cells [columnNumber]. number == 0.0);
			for (integer icol = myRow -> numberOfColumns + 1; icol > columnNumber; icol --)
				thyRow -> cells [icol] = std::move (myRow -> cells [icol - 1]);
		}
		/*
			Strong exception safety, step 2: perform changes to me without any risk of error:
			transfer column headers and rows from the larger structure to me.
		*/
		my columnHeaders = thy columnHeaders.move();
		my rows = thy rows.move();
		my numberOfColumns ++;   // maintain invariant
	} catch (MelderError) {
		Melder_throw (me, U": column not inserted.");
	}
}

void Table_setColumnLabel (Table me, integer columnNumber, conststring32 label /* cattable */) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		my columnHeaders [columnNumber]. label = Melder_dup (label);
	} catch (MelderError) {
		Melder_throw (me, U": column label not set.");
	}
}

integer Table_findColumnIndexFromColumnLabel (Table me, conststring32 label) noexcept {
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (my columnHeaders [icol]. label && str32equ (my columnHeaders [icol]. label.get(), label))
			return icol;
	}
	return 0;
}

integer Table_getColumnIndexFromColumnLabel (Table me, conststring32 columnLabel) {
	integer columnNumber = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	if (columnNumber == 0)
		Melder_throw (me, U": there is no column named \"", columnLabel, U"\".");
	return columnNumber;
}

autoINTVEC Table_getColumnIndicesFromColumnLabelString (Table me, conststring32 columnLabels_string) {
	autoSTRVEC columnLabels = splitByWhitespace_STRVEC (columnLabels_string);
	if (columnLabels.size < 1)
		Melder_throw (me, U": you specified an empty list of columns.");
	autoINTVEC columns = raw_INTVEC (columnLabels.size);
	for (integer icol = 1; icol <= columnLabels.size; icol ++)
		columns [icol] = Table_getColumnIndexFromColumnLabel (me, columnLabels [icol].get());
	return columns;
}

integer Table_searchColumn (Table me, integer columnNumber, conststring32 value) noexcept {
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [columnNumber]. string && str32equ (row -> cells [columnNumber]. string.get(), value))
			return irow;
	}
	return 0;
}

void Table_setStringValue (Table me, integer rowNumber, integer columnNumber, conststring32 value /* cattable */) {
	try {
		/*
			Check without changes.
		*/
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring32 newLabel = Melder_dup (value);
		/*
			Change without errors.
		*/
		TableRow row = my rows.at [rowNumber];
		row -> cells [columnNumber]. string = newLabel.move();
		my columnHeaders [columnNumber]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": string value not set.");
	}
}

void Table_setNumericValue (Table me, integer rowNumber, integer columnNumber, double value) {
	try {
		/*
			Check without changes.
		*/
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autostring32 newLabel = Melder_dup (Melder_double (value));
		/*
			Change without errors.
		*/
		TableRow row = my rows.at [rowNumber];
		row -> cells [columnNumber]. string = newLabel.move();
		my columnHeaders [columnNumber]. numericized = false;
	} catch (MelderError) {
		Melder_throw (me, U": numeric value not set.");
	}
}

bool Table_isCellNumeric_ErrorFalse (Table me, integer rowNumber, integer columnNumber) {
	if (rowNumber < 1 || rowNumber > my rows.size)
		return false;
	if (columnNumber < 1 || columnNumber > my numberOfColumns)
		return false;
	const TableRow row = my rows.at [rowNumber];
	conststring32 cell = row -> cells [columnNumber]. string.get();
	if (! cell)
		return true;   // namely the value --undefined--
	/*
		Skip leading white space, in order to separately detect "?" and "--undefined--".
	*/
	Melder_skipHorizontalOrVerticalSpace (& cell);
	if (cell [0] == U'\0')
		return true;   // only white space: the value --undefined--
	if (cell [0] == U'?' || str32nequ (cell, U"--undefined--", 13)) {
		/*
			See whether there is anything else besides "?" or "--undefined--" and white space.
		*/
		cell += ( cell [0] == U'?' ) ? 1 : 13;
		Melder_skipHorizontalOrVerticalSpace (& cell);
		return *cell == U'\0';   // only white space after the "?" or "--undefined--"
	}
	return Melder_isStringNumeric (cell);
}

bool Table_isColumnNumeric_ErrorFalse (Table me, integer columnNumber) {
	if (columnNumber < 1 || columnNumber > my numberOfColumns)
		return false;
	for (integer irow = 1; irow <= my rows.size; irow ++)
		if (! Table_isCellNumeric_ErrorFalse (me, irow, columnNumber))
			return false;
	return true;
}

static integer stringCompare_column;

static int stringCompare_NoError (const void *first, const void *second) {
	const TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	const conststring32 firstString = my cells [stringCompare_column]. string.get();
	const conststring32 secondString = thy cells [stringCompare_column]. string.get();
	return str32cmp (firstString ? firstString : U"", secondString ? secondString : U"");
}

static void sortRowsByStrings_Assert (Table me, integer columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	stringCompare_column = columnNumber;
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), stringCompare_NoError);
}

static int indexCompare_NoError (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	if (my sortingIndex < thy sortingIndex)
		return -1;
	if (my sortingIndex > thy sortingIndex)
		return +1;
	return 0;
}

static void sortRowsByIndex_NoError (Table me) {
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), indexCompare_NoError);
}

void Table_numericize_Assert (Table me, integer columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	if (my columnHeaders [columnNumber]. numericized)
		return;
	if (Table_isColumnNumeric_ErrorFalse (me, columnNumber)) {
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			const conststring32 string = row -> cells [columnNumber]. string.get();
			row -> cells [columnNumber]. number =
					! string || string [0] == U'\0' || (string [0] == U'?' && string [1] == U'\0') ? undefined :
					Melder_atof (string);
		}
	} else {
		integer iunique = 0;
		conststring32 previousString = nullptr;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		sortRowsByStrings_Assert (me, columnNumber);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			conststring32 string = row -> cells [columnNumber]. string.get();
			if (! string)
				string = U"";
			if (! previousString || ! str32equ (string, previousString))
				iunique ++;
			row -> cells [columnNumber]. number = iunique;
			previousString = string;
		}
		sortRowsByIndex_NoError (me);
	}
	my columnHeaders [columnNumber]. numericized = true;
}

static void Table_numericize_checkDefined (Table me, integer columnNumber) {
	Table_numericize_Assert (me, columnNumber);
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		const TableRow row = my rows.at [irow];
		if (isundef (row -> cells [columnNumber]. number)) {
			Melder_throw (me, U": the cell in row ", irow,
				U" of column \"", my columnHeaders [columnNumber]. label ? my columnHeaders [columnNumber]. label.get() : Melder_integer (columnNumber),
				U"\" is undefined."
			);
		}
	}
}

conststring32 Table_getStringValue_Assert (Table me, integer rowNumber, integer columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	const TableRow row = my rows.at [rowNumber];
	return row -> cells [columnNumber]. string ? row -> cells [columnNumber]. string.get() : U"";
}

double Table_getNumericValue_Assert (Table me, integer rowNumber, integer columnNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= my rows.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= my numberOfColumns);
	const TableRow row = my rows.at [rowNumber];
	Table_numericize_Assert (me, columnNumber);
	return row -> cells [columnNumber]. number;
}

double Table_getMean (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		longdouble sum = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow row = my rows.at [irow];
			sum += row -> cells [columnNumber]. number;
		}
		return double (sum) / my rows.size;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute mean of column ", columnNumber, U".");
	}
}

double Table_getMaximum (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		TableRow firstRow = my rows.at [1];
		double maximum = firstRow -> cells [columnNumber]. number;
		for (integer irow = 2; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [columnNumber]. number > maximum)
				maximum = row -> cells [columnNumber]. number;
		}
		return maximum;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute maximum of column ", columnNumber, U".");
	}
}

double Table_getMinimum (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		TableRow firstRow = my rows.at [1];
		double minimum = firstRow -> cells [columnNumber]. number;
		for (integer irow = 2; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [columnNumber]. number < minimum)
				minimum = row -> cells [columnNumber]. number;
		}
		return minimum;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute minimum of column ", columnNumber, U".");
	}
}

double Table_getGroupMean (Table me, integer columnNumber, integer groupColumnNumber, conststring32 group) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		integer n = 0;
		longdouble sum = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_equ (row -> cells [groupColumnNumber]. string.get(), group)) {
				n += 1;
				sum += row -> cells [columnNumber]. number;
			}
		}
		if (n < 1)
			return undefined;
		double mean = double (sum) / n;
		return mean;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute mean of column ", columnNumber, U" for group \"", group, U"\" of column ", groupColumnNumber, U".");
	}
}

double Table_getQuantile (Table me, integer columnNumber, double quantile) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			return undefined;
		autoVEC sortingColumn = raw_VEC (my rows.size);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow row = my rows.at [irow];
			sortingColumn [irow] = row -> cells [columnNumber]. number;
		}
		sort_VEC_inout (sortingColumn.get());
		return NUMquantile (sortingColumn.get(), quantile);
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute the ", quantile, U" quantile of column ", columnNumber, U".");
	}
}

double Table_getStdev (Table me, integer columnNumber) {
	try {
		const double mean = Table_getMean (me, columnNumber);   // already checks for columnNumber and undefined cells
		if (my rows.size < 2)
			return undefined;
		longdouble sum = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			double d = row -> cells [columnNumber]. number - mean;
			sum += d * d;
		}
		return sqrt (double (sum) / (my rows.size - 1));
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute the standard deviation of column ", columnNumber, U".");
	}
}

integer Table_drawRowFromDistribution (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_checkDefined (me, columnNumber);
		if (my rows.size < 1)
			Melder_throw (me, U": no rows.");
		longdouble total = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			total += row -> cells [columnNumber]. number;
		}
		if (total <= 0.0)
			Melder_throw (me, U": the total weight of column ", columnNumber, U" is not positive.");
		integer irow;
		do {
			double rand = NUMrandomUniform (0.0, double (total));
			longdouble sum = 0.0;
			for (irow = 1; irow <= my rows.size; irow ++) {
				TableRow row = my rows.at [irow];
				sum += row -> cells [columnNumber]. number;
				if (rand <= sum)
					break;
			}
		} while (irow > my rows.size);   // guard against rounding errors
		return irow;
	} catch (MelderError) {
		Melder_throw (me, U": cannot draw a row from the distribution of column ", Table_messageColumn (me, columnNumber), U".");
	}
}

autoTable Table_extractRowsWhereColumn_number (Table me, integer columnNumber, kMelder_number which, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_Assert (me, columnNumber);   // extraction should work even if cells are not defined
		autoTable thee = Table_create (0, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			thy columnHeaders [icol]. label = Melder_dup (my columnHeaders [icol]. label.get());
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which, criterion)) {
				autoTableRow newRow = Data_copy (row);
				thy rows. addItem_move (newRow.move());
			}
		}
		if (thy rows.size == 0)
			Melder_warning (U"No row matches criterion.");
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTable Table_extractRowsWhereColumn_string (Table me, integer columnNumber, kMelder_string which, conststring32 criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autoTable thee = Table_create (0, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			thy columnHeaders [icol]. label = Melder_dup (my columnHeaders [icol]. label.get());
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string.get(), which, criterion, true)) {
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

static void Table_columns_checkExist (Table me, constSTRVEC columnNames) {
	for (integer i = 1; i <= columnNames.size; i ++)
		if (Table_findColumnIndexFromColumnLabel (me, columnNames [i]) == 0)
			Melder_throw (me, U": column \"", columnNames [i], U"\" does not exist.");
}

static void Table_columns_checkCrossSectionEmpty (constSTRVEC factors, constSTRVEC vars) {
	for (integer ifactor = 1; ifactor <= factors.size; ifactor ++)
		for (integer ivar = 1; ivar <= vars.size; ivar ++)
			if (str32equ (factors [ifactor], vars [ivar]))
				Melder_throw (U"Factor \"", factors [ifactor], U"\" is also used as dependent variable.");
}

autoTable Table_collapseRows (Table me, conststring32 factors_string, conststring32 columnsToSum_string,
	conststring32 columnsToAverage_string, conststring32 columnsToMedianize_string,
	conststring32 columnsToAverageLogarithmically_string, conststring32 columnsToMedianizeLogarithmically_string)
{
	bool originalChanged = false;
	try {
		Melder_assert (factors_string);

		/*
			Parse the six strings of tokens.
		*/
		autoSTRVEC factors = splitByWhitespace_STRVEC (factors_string);
		if (factors.size < 1)
			Melder_throw (U"In order to pool table data, you must supply at least one independent variable.");
		Table_columns_checkExist (me, factors.get());

		autoSTRVEC columnsToSum = splitByWhitespace_STRVEC (columnsToSum_string);
		Table_columns_checkExist (me, columnsToSum.get());
		Table_columns_checkCrossSectionEmpty (factors.get(), columnsToSum.get());

		autoSTRVEC columnsToAverage = splitByWhitespace_STRVEC (columnsToAverage_string);
		Table_columns_checkExist (me, columnsToAverage.get());
		Table_columns_checkCrossSectionEmpty (factors.get(), columnsToAverage.get());

		autoSTRVEC columnsToMedianize = splitByWhitespace_STRVEC (columnsToMedianize_string);
		Table_columns_checkExist (me, columnsToMedianize.get());
		Table_columns_checkCrossSectionEmpty (factors.get(), columnsToMedianize.get());

		autoSTRVEC columnsToAverageLogarithmically = splitByWhitespace_STRVEC (columnsToAverageLogarithmically_string);
		Table_columns_checkExist (me, columnsToAverageLogarithmically.get());
		Table_columns_checkCrossSectionEmpty (factors.get(), columnsToAverageLogarithmically.get());

		autoSTRVEC columnsToMedianizeLogarithmically = splitByWhitespace_STRVEC (columnsToMedianizeLogarithmically_string);
		Table_columns_checkExist (me, columnsToMedianizeLogarithmically.get());
		Table_columns_checkCrossSectionEmpty (factors.get(), columnsToMedianizeLogarithmically.get());

		autoTable thee = Table_createWithoutColumnNames (0,
				factors.size + columnsToSum.size + columnsToAverage.size + columnsToMedianize.size +
				columnsToAverageLogarithmically.size + columnsToMedianizeLogarithmically.size);
		Melder_assert (thy numberOfColumns > 0);

		autoVEC sortingColumn;
		if (columnsToMedianize.size > 0 || columnsToMedianizeLogarithmically.size > 0)
			sortingColumn = zero_VEC (my rows.size);
		/*
			Set the column names. Within the dependent variables, the same name may occur more than once.
		*/
		autoINTVEC columns = zero_INTVEC (thy numberOfColumns);
		{
			integer icol = 0;
			for (integer i = 1; i <= factors.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, factors [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, factors [i].get());
			}
			for (integer i = 1; i <= columnsToSum.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToSum [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToSum [i].get());
			}
			for (integer i = 1; i <= columnsToAverage.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToAverage [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverage [i].get());
			}
			for (integer i = 1; i <= columnsToMedianize.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToMedianize [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianize [i].get());
			}
			for (integer i = 1; i <= columnsToAverageLogarithmically.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToAverageLogarithmically [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToAverageLogarithmically [i].get());
			}
			for (integer i = 1; i <= columnsToMedianizeLogarithmically.size; i ++) {
				Table_setColumnLabel (thee.get(), ++ icol, columnsToMedianizeLogarithmically [i].get());
				columns [icol] = Table_findColumnIndexFromColumnLabel (me, columnsToMedianizeLogarithmically [i].get());
			}
			Melder_assert (icol == thy numberOfColumns);
		}
		/*
			Make sure that all the columns in the original table that we will use in the pooled table are defined.
		*/
		for (integer icol = 1; icol <= thy numberOfColumns; icol ++) {
			Table_numericize_checkDefined (me, columns [icol]);
		}
		/*
			Remember the present sorting of the original table.
			(This is safe: the sorting index may change only vacuously when numericizing.)
			But this cannot be done before the previous block!
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		/*
			We will now sort the original table temporarily, by the factors (independent variables) only.
		*/
		Table_sortRows_Assert (me, constINTVEC (columns.cells, factors.size));   /* This works only because the factors come first. */
		originalChanged = true;
		/*
			Find stretches of identical factors.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			integer rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows.size)
					break;
				for (integer icol = 1; icol <= factors.size; icol ++) {
					if (my rows.at [rowmax] -> cells [columns [icol]]. number !=
						my rows.at [rowmin] -> cells [columns [icol]]. number)
					{
						identical = false;
						break;
					}
				}
				if (! identical)
					break;
			}
			rowmax --;
			/*
				We have the stretch.
			*/
			Table_insertRow (thee.get(), thy rows.size + 1);
			{// scope
				integer icol = 0;
				for (integer i = 1; i <= factors.size; i ++) {
					++ icol;
					Table_setStringValue (thee.get(), thy rows.size, icol,
						my rows.at [rowmin] -> cells [columns [icol]]. string.get());
				}
				for (integer i = 1; i <= columnsToSum.size; i ++) {
					++ icol;
					longdouble sum = 0.0;
					for (integer jrow = rowmin; jrow <= rowmax; jrow ++)
						sum += my rows.at [jrow] -> cells [columns [icol]]. number;
					Table_setNumericValue (thee.get(), thy rows.size, icol, double (sum));
				}
				for (integer i = 1; i <= columnsToAverage.size; i ++) {
					++ icol;
					longdouble sum = 0.0;
					for (integer jrow = rowmin; jrow <= rowmax; jrow ++)
						sum += my rows.at [jrow] -> cells [columns [icol]]. number;
					Table_setNumericValue (thee.get(), thy rows.size, icol, double (sum) / (rowmax - rowmin + 1));
				}
				for (integer i = 1; i <= columnsToMedianize.size; i ++) {
					++ icol;
					for (integer jrow = rowmin; jrow <= rowmax; jrow ++)
						sortingColumn [jrow] = my rows.at [jrow] -> cells [columns [icol]]. number;
					const VEC part = sortingColumn.part (rowmin, rowmax);
					sort_VEC_inout (part);
					const double median = NUMquantile (part, 0.5);
					Table_setNumericValue (thee.get(), thy rows.size, icol, median);
				}
				for (integer i = 1; i <= columnsToAverageLogarithmically.size; i ++) {
					++ icol;
					longdouble sum = 0.0;
					for (integer jrow = rowmin; jrow <= rowmax; jrow ++) {
						const double value = my rows.at [jrow] -> cells [columns [icol]]. number;
						if (value <= 0.0) {
							Melder_throw (
								U"The cell in column \"", columnsToAverageLogarithmically [i].get(),
								U"\" of row ", jrow, U" of ", me,
								U" is not positive.\nCannot average logarithmically."
							);
						}
						sum += log (value);
					}
					Table_setNumericValue (thee.get(), thy rows.size, icol, exp (double (sum / (rowmax - rowmin + 1))));
				}
				for (integer i = 1; i <= columnsToMedianizeLogarithmically.size; i ++) {
					++ icol;
					for (integer jrow = rowmin; jrow <= rowmax; jrow ++) {
						const double value = my rows.at [jrow] -> cells [columns [icol]]. number;
						if (value <= 0.0) {
							Melder_throw (
								U"The cell in column \"", columnsToMedianizeLogarithmically [i].get(),
								U"\" of row ", jrow, U" of ", me,
								U" is not positive.\nCannot medianize logarithmically."
							);
						}
						sortingColumn [jrow] = log (value);
					}
					const VEC part = sortingColumn.part (rowmin, rowmax);
					sort_VEC_inout (part);
					const double median = NUMquantile (part, 0.5);
					Table_setNumericValue (thee.get(), thy rows.size, icol, exp (median));
				}
				Melder_assert (icol == thy numberOfColumns);
			}
			irow = rowmax;
		}
		if (originalChanged)
			sortRowsByIndex_NoError (me);   // unsort the original table
		return thee;
	} catch (MelderError) {
		if (originalChanged)
			sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
	}
}

static autoSTRVEC Table_getLevels_ (Table me, integer column) {
	try {
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		integer columns [1] = { column };
		Table_sortRows_Assert (me, constINTVEC (columns, 1));
		integer numberOfLevels = 0;
		integer irow = 1;
		while (irow <= my rows.size) {
			const double value = my rows.at [irow] -> cells [column]. number;
			numberOfLevels ++;
			while (++ irow <= my rows.size && my rows.at [irow] -> cells [column]. number == value) {
				;
			}
		}
		autoSTRVEC result (numberOfLevels);
		numberOfLevels = 0;
		irow = 1;
		while (irow <= my rows.size) {
			const double value = my rows.at [irow] -> cells [column]. number;
			result [++ numberOfLevels] = Melder_dup (Table_getStringValue_Assert (me, irow, column));
			while (++ irow <= my rows.size && my rows.at [irow] -> cells [column]. number == value) { }
		}
		sortRowsByIndex_NoError (me);   // unsort the original table
		return result;
	} catch (MelderError) {
		sortRowsByIndex_NoError (me);   // unsort the original table   // UGLY
		throw;
	}
}

autoTable Table_rowsToColumns (Table me, conststring32 factors_string, integer columnToTranspose, conststring32 columnsToExpand_string) {
	bool originalChanged = false;
	try {
		Melder_assert (factors_string);

		bool warned = false;
		/*
			Parse the two strings of tokens.
		*/
		autoSTRVEC factors_names = splitByWhitespace_STRVEC (factors_string);
		const integer numberOfFactors = factors_names.size;
		if (numberOfFactors < 1)
			Melder_throw (U"In order to nest table data, you should supply at least one independent variable.");
		Table_columns_checkExist (me, factors_names.get());
		autoSTRVEC columnsToExpand_names = splitByWhitespace_STRVEC (columnsToExpand_string);
		const integer numberToExpand = columnsToExpand_names.size;
		if (numberToExpand < 1)
			Melder_throw (U"In order to nest table data, you should supply at least one dependent variable (to expand).");
		Table_columns_checkExist (me, columnsToExpand_names.get());
		Table_columns_checkCrossSectionEmpty (factors_names.get(), columnsToExpand_names.get());
		autoSTRVEC levels_names = Table_getLevels_ (me, columnToTranspose);
		const integer numberOfLevels = levels_names.size;
		/*
			Get the column numbers for the factors.
		*/
		autoINTVEC factorColumns = zero_INTVEC (numberOfFactors);
		for (integer ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
			factorColumns [ifactor] = Table_findColumnIndexFromColumnLabel (me, factors_names [ifactor].get());
			/*
				Make sure that all the columns in the original table that we will use in the nested table are defined.
			*/
			Table_numericize_checkDefined (me, factorColumns [ifactor]);
		}
		/*
			Get the column numbers for the expandable variables.
		*/
		autoINTVEC columnsToExpand = zero_INTVEC (numberToExpand);
		for (integer iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			columnsToExpand [iexpand] = Table_findColumnIndexFromColumnLabel (me, columnsToExpand_names [iexpand].get());
			Table_numericize_checkDefined (me, columnsToExpand [iexpand]);
		}
		/*
			Create the new table, with column names.
		*/
		autoTable thee = Table_createWithoutColumnNames (0, numberOfFactors + (numberOfLevels * numberToExpand));
		Melder_assert (thy numberOfColumns > 0);
		for (integer ifactor = 1; ifactor <= numberOfFactors; ifactor ++)
			Table_setColumnLabel (thee.get(), ifactor, factors_names [ifactor].get());
		for (integer iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
			for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				//Melder_casual (U"Number of factors: ", numberOfFactors);
				//Melder_casual (U"Level: ", ilevel, U" out of ", numberOfLevels);
				integer columnNumber = numberOfFactors + (iexpand - 1) * numberOfLevels + ilevel;
				//Melder_casual (U"Column number: ", columnNumber);
				Table_setColumnLabel (thee.get(), columnNumber,
					Melder_cat (columnsToExpand_names [iexpand].get(), U".", levels_names [ilevel].get()));
			}
		}
		/*
			Remember the present sorting of the original table.
			(This is safe: the sorting index may change only vacuously when numericizing.)
			But this cannot be done before the previous blocks that numericize!
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			row -> sortingIndex = irow;
		}
		/*
			We will now sort the original table temporarily, by the factors (independent variables) only.
		*/
		Table_sortRows_Assert (me, factorColumns.get());
		originalChanged = true;
		/*
			Find stretches of identical factors.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			integer rowmin = irow, rowmax = irow;
			for (;;) {
				bool identical = true;
				if (++ rowmax > my rows.size)
					break;
				for (integer ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
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
				We have the stretch.
			*/
			Table_insertRow (thee.get(), thy rows.size + 1);
			TableRow thyRow = thy rows.at [thy rows.size];
			for (integer ifactor = 1; ifactor <= numberOfFactors; ifactor ++) {
				Table_setStringValue (thee.get(), thy rows.size, ifactor,
					my rows.at [rowmin] -> cells [factorColumns [ifactor]]. string.get());
			}
			for (integer iexpand = 1; iexpand <= numberToExpand; iexpand ++) {
				for (integer jrow = rowmin; jrow <= rowmax; jrow ++) {
					TableRow myRow = my rows.at [jrow];
					const double value = myRow -> cells [columnsToExpand [iexpand]]. number;
					const integer level = Melder_iround (myRow -> cells [columnToTranspose]. number);
					const integer thyColumn = numberOfFactors + (iexpand - 1) * numberOfLevels + level;
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
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			Table_setStringValue (thee.get(), icol, 1, my columnHeaders [icol]. label.get());
		for (integer irow = 1; irow <= my rows.size; irow ++)
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				Table_setStringValue (thee.get(), icol, 1 + irow, Table_getStringValue_Assert (me, irow, icol));
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not transposed.");
	}
}

static constINTVEC *cellCompare_columns;

static int cellCompare (const void *first, const void *second) {
	const TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	const integer ncol = cellCompare_columns->size;
	for (integer icol = 1; icol <= ncol; icol ++) {
		const integer cellNumber = (*cellCompare_columns) [icol];
		if (my cells [cellNumber]. number < thy cells [cellNumber]. number)
			return -1;
		if (my cells [cellNumber]. number > thy cells [cellNumber]. number)
			return +1;
	}
	return 0;
}

void Table_sortRows_Assert (Table me, constINTVEC columns) {
	for (integer icol = 1; icol <= columns.size; icol ++)
		Table_numericize_Assert (me, columns [icol]);
	cellCompare_columns = & columns;
	qsort (& my rows.at [1], (unsigned long) my rows.size, sizeof (TableRow), cellCompare);
}

void Table_sortRows_string (Table me, conststring32 columns_string) {
	try {
		autoSTRVEC columns_tokens = splitByWhitespace_STRVEC (columns_string);
		integer numberOfColumns = columns_tokens.size;
		if (numberOfColumns < 1)
			Melder_throw (me, U": you specified an empty list of columns.");
		autoINTVEC columns = raw_INTVEC (numberOfColumns);
		for (integer icol = 1; icol <= numberOfColumns; icol ++) {
			columns [icol] = Table_findColumnIndexFromColumnLabel (me, columns_tokens [icol].get());
			if (columns [icol] == 0)
				Melder_throw (U"Column \"", columns_tokens [icol].get(), U"\" does not exist.");
		}
		Table_sortRows_Assert (me, columns.get());
	} catch (MelderError) {
		Melder_throw (me, U": rows not sorted.");
	}
}

void Table_randomizeRows (Table me) noexcept {
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		integer jrow = NUMrandomInteger (irow, my rows.size);
		TableRow tmp = my rows.at [irow];
		my rows.at [irow] = my rows.at [jrow];
		my rows.at [jrow] = tmp;
	}
}

void Table_reflectRows (Table me) noexcept {
	for (integer irow = 1; irow <= my rows.size / 2; irow ++) {
		integer jrow = my rows.size + 1 - irow;
		TableRow tmp = my rows.at [irow];
		my rows.at [irow] = my rows.at [jrow];
		my rows.at [jrow] = tmp;
	}
}

autoTable Tables_append (OrderedOf<structTable>* me) {
	try {
		if (my size == 0)
			Melder_throw (U"Cannot add zero tables.");
		Table thee = my at [1];
		integer nrow = thy rows.size;
		const integer ncol = thy numberOfColumns;
		const Table firstTable = thee;
		for (integer itab = 2; itab <= my size; itab ++) {
			thee = my at [itab];
			nrow += thy rows.size;
			if (thy numberOfColumns != ncol)
				Melder_throw (U"Numbers of columns do not match.");
			for (integer icol = 1; icol <= ncol; icol ++) {
				if (! Melder_equ (thy columnHeaders [icol]. label.get(), firstTable -> columnHeaders [icol]. label.get())) {
					Melder_throw (U"The label of column ", icol, U" of ", thee,
						U" (", thy columnHeaders [icol]. label.get(), U") does not match the label of column ", icol,
						U" of ", firstTable, U" (", firstTable -> columnHeaders [icol]. label.get(), U")."
					);
				}
			}
		}
		autoTable him = Table_createWithoutColumnNames (nrow, ncol);
		for (integer icol = 1; icol <= ncol; icol ++)
			Table_setColumnLabel (him.get(), icol, thy columnHeaders [icol]. label.get());
		nrow = 0;
		for (integer itab = 1; itab <= my size; itab ++) {
			thee = my at [itab];
			for (integer irow = 1; irow <= thy rows.size; irow ++) {
				nrow ++;
				for (integer icol = 1; icol <= ncol; icol ++)
					Table_setStringValue (him.get(), nrow, icol, Table_getStringValue_Assert (thee, irow, icol));
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Table objects not appended.");
	}
}

void Table_appendSumColumn (Table me, integer column1, integer column2, conststring32 label) {   // safe
	try {
		/*
			Check without change.
		*/
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1,
					myRow -> cells [column1]. number + myRow -> cells [column2]. number);
		}
		/*
			Safe change.
		*/
		Table_appendColumn (me, label);
		/*
			Change without error.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			const TableRow thyRow = thy rows.at [irow];
			const TableCell myCell = & myRow -> cells [my numberOfColumns];
			const TableCell thyCell = & thyRow -> cells [1];
			myCell -> string = thyCell -> string. move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": sum column not appended.");
	}
}

void Table_appendDifferenceColumn (Table me, integer column1, integer column2, conststring32 label) {   // safe
	try {
		/*
			Check without change.
		*/
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1,
					myRow -> cells [column1]. number - myRow -> cells [column2]. number);
		}
		/*
			Safe change.
		*/
		Table_appendColumn (me, label);
		/*
			Change without error.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			const TableRow thyRow = thy rows.at [irow];
			const TableCell myCell = & myRow -> cells [my numberOfColumns];
			const TableCell thyCell = & thyRow -> cells [1];
			myCell -> string = thyCell -> string. move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": difference column not appended.");
	}
}

void Table_appendProductColumn (Table me, integer column1, integer column2, conststring32 label) {   // safe
	try {
		/*
			Check without change.
		*/
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			Table_setNumericValue (thee.get(), irow, 1,
					myRow -> cells [column1]. number * myRow -> cells [column2]. number);
		}
		/*
			Safe change.
		*/
		Table_appendColumn (me, label);
		/*
			Change without error.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			const TableRow thyRow = thy rows.at [irow];
			const TableCell myCell = & myRow -> cells [my numberOfColumns];
			const TableCell thyCell = & thyRow -> cells [1];
			myCell -> string = thyCell -> string. move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": product column not appended.");
	}
}

void Table_appendQuotientColumn (Table me, integer column1, integer column2, conststring32 label) {   // safe
	try {
		/*
			Check without change.
		*/
		Table_checkSpecifiedColumnNumberWithinRange (me, column1);
		Table_checkSpecifiedColumnNumberWithinRange (me, column2);
		Table_numericize_checkDefined (me, column1);
		Table_numericize_checkDefined (me, column2);
		autoTable thee = Table_createWithoutColumnNames (my rows.size, 1);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			const double value =
					myRow -> cells [column2]. number == 0.0 ? undefined :
					myRow -> cells [column1]. number / myRow -> cells [column2]. number;
			Table_setNumericValue (thee.get(), irow, 1, value);
		}
		/*
			Safe change.
		*/
		Table_appendColumn (me, label);
		/*
			Change without error.
		*/
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow myRow = my rows.at [irow];
			const TableRow thyRow = thy rows.at [irow];
			const TableCell myCell = & myRow -> cells [my numberOfColumns];
			const TableCell thyCell = & thyRow -> cells [1];
			myCell -> string = thyCell -> string. move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": quotient column not appended.");
	}
}

void Table_formula_columnRange (Table me, integer fromColumn, integer toColumn, conststring32 expression, Interpreter interpreter) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, fromColumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, toColumn);
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_UNKNOWN, true);
		Formula_Result result;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			for (integer icol = fromColumn; icol <= toColumn; icol ++) {
				Formula_run (irow, icol, & result);
				if (result. expressionType == kFormula_EXPRESSION_TYPE_STRING) {
					Table_setStringValue (me, irow, icol, result. stringResult.get());
				} else if (result. expressionType == kFormula_EXPRESSION_TYPE_NUMERIC) {
					Table_setNumericValue (me, irow, icol, result. numericResult);
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

void Table_formula (Table me, integer icol, conststring32 expression, Interpreter interpreter) {
	Table_formula_columnRange (me, icol, icol, expression, interpreter);
}

double Table_getCorrelation_pearsonR (Table me, integer column1, integer column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	const integer n = my rows.size;
	double correlation;
	longdouble sum1 = 0.0, sum2 = 0.0, sum12 = 0.0, sum11 = 0.0, sum22 = 0.0;
	if (out_significance) *out_significance = undefined;
	if (out_lowerLimit)   *out_lowerLimit   = undefined;
	if (out_upperLimit)   *out_upperLimit   = undefined;
	if (column1 < 1 || column1 > my numberOfColumns)
		return undefined;
	if (column2 < 1 || column2 > my numberOfColumns)
		return undefined;
	if (n < 2)
		return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (integer irow = 1; irow <= n; irow ++) {
		const TableRow row = my rows.at [irow];
		sum1 += row -> cells [column1]. number;
		sum2 += row -> cells [column2]. number;
	}
	double mean1 = double (sum1) / n;
	double mean2 = double (sum2) / n;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		const double d1 = row -> cells [column1]. number - mean1, d2 = row -> cells [column2]. number - mean2;
		sum12 += d1 * d2;
		sum11 += d1 * d1;
		sum22 += d2 * d2;
	}
	correlation = ( sum11 == 0.0 || sum22 == 0.0 ? undefined :
			double (sum12) / sqrt (double (sum11) * double (sum22)) );
	if (out_significance && isdefined (correlation) && n >= 3)
		*out_significance = fabs (correlation) == 1.0 ? 0.0 :
			/* One-sided: */
			NUMstudentQ (fabs (correlation) * sqrt ((n - 2) / (1 - correlation * correlation)), n - 2);
	if ((out_lowerLimit || out_upperLimit) && isdefined (correlation) && n >= 4) {
		if (fabs (correlation) == 1.0) {
			if (out_lowerLimit)
				*out_lowerLimit = correlation;
			if (out_upperLimit)
				*out_upperLimit = correlation;
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

double Table_getCorrelation_kendallTau (Table me, integer column1, integer column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	const integer n = my rows.size;
	integer numberOfConcordants = 0, numberOfDiscordants = 0, numberOfExtra1 = 0, numberOfExtra2 = 0;
	if (out_significance) *out_significance = undefined;
	if (out_lowerLimit)   *out_lowerLimit   = undefined;
	if (out_upperLimit)   *out_upperLimit   = undefined;
	if (column1 < 1 || column1 > my numberOfColumns)
		return undefined;
	if (column2 < 1 || column2 > my numberOfColumns)
		return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	for (integer irow = 1; irow < n; irow ++) {
		TableRow rowi = my rows.at [irow];
		for (integer jrow = irow + 1; jrow <= n; jrow ++) {
			TableRow rowj = my rows.at [jrow];
			const double diff1 = rowi -> cells [column1]. number - rowj -> cells [column1]. number;
			const double diff2 = rowi -> cells [column2]. number - rowj -> cells [column2]. number;
			const double concord = diff1 * diff2;
			if (concord > 0.0)
				numberOfConcordants ++;
			else if (concord < 0.0)
				numberOfDiscordants ++;
			else if (diff1 != 0.0)
				numberOfExtra1 ++;
			else
				numberOfExtra2 ++;
		}
	}
	const double denominator = sqrt ((numberOfConcordants + numberOfDiscordants + numberOfExtra1) *
			(numberOfConcordants + numberOfDiscordants + numberOfExtra2));
	const double correlation = ( denominator == 0.0 ? undefined :
			(numberOfConcordants - numberOfDiscordants) / denominator );
	if ((out_significance || out_lowerLimit || out_upperLimit) && isdefined (correlation) && n >= 2) {
		const double standardError = sqrt ((4 * n + 10.0) / (9 * n * (n - 1)));
		if (out_significance)
			*out_significance = NUMgaussQ (fabs (correlation) / standardError);   // one-sided
		if (out_lowerLimit)
			*out_lowerLimit = correlation - NUMinvGaussQ (significanceLevel) * standardError;
		if (out_upperLimit)
			*out_upperLimit = correlation + NUMinvGaussQ (significanceLevel) * standardError;
	}
	return correlation;
}

double Table_getDifference_studentT (Table me, integer column1, integer column2, double significanceLevel,
	double *out_t, double *out_numberOfDegreesOfFreedom, double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_t)                        *out_t                        = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significance)             *out_significance             = undefined;
	if (out_lowerLimit)               *out_lowerLimit               = undefined;
	if (out_upperLimit)               *out_upperLimit               = undefined;
	const integer n = my rows.size;
	if (n < 1)
		return undefined;
	if (column1 < 1 || column1 > my numberOfColumns)
		return undefined;
	if (column2 < 1 || column2 > my numberOfColumns)
		return undefined;
	Table_numericize_Assert (me, column1);
	Table_numericize_Assert (me, column2);
	longdouble sum = 0.0;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		sum += row -> cells [column1]. number - row -> cells [column2]. number;
	}
	const double meanDifference = double (sum) / n;
	const integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom)
		*out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		longdouble sumOfSquares = 0.0;
		for (integer irow = 1; irow <= n; irow ++) {
			TableRow row = my rows.at [irow];
			double diff = (row -> cells [column1]. number - row -> cells [column2]. number) - meanDifference;
			sumOfSquares += diff * diff;
		}
		const double standardError = sqrt (double (sumOfSquares) / degreesOfFreedom / n);
		if (out_t && standardError != 0.0)
			*out_t = meanDifference / standardError;
		if (out_significance)
			*out_significance = ( standardError == 0.0 ? 0.0 :
					NUMstudentQ (fabs (meanDifference) / standardError, degreesOfFreedom) );
		if (out_lowerLimit)
			*out_lowerLimit = meanDifference - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit)
			*out_upperLimit = meanDifference + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return meanDifference;
}

double Table_getMean_studentT (Table me, integer column, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	const integer n = my rows.size;
	if (out_tFromZero)                *out_tFromZero                = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero)     *out_significanceFromZero     = undefined;
	if (out_lowerLimit)               *out_lowerLimit               = undefined;
	if (out_upperLimit)               *out_upperLimit               = undefined;
	if (n < 1)
		return undefined;
	if (column < 1 || column > my numberOfColumns)
		return undefined;
	const integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom)
		*out_numberOfDegreesOfFreedom = degreesOfFreedom;
	Table_numericize_Assert (me, column);
	longdouble sum = 0.0;
	for (integer irow = 1; irow <= n; irow ++) {
		TableRow row = my rows.at [irow];
		sum += row -> cells [column]. number;
	}
	double mean = double (sum) / n;
	if (n >= 2 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		longdouble sumOfSquares = 0.0;
		for (integer irow = 1; irow <= n; irow ++) {
			const TableRow row = my rows.at [irow];
			const double diff = row -> cells [column]. number - mean;
			sumOfSquares += diff * diff;
		}
		const double standardError = sqrt (double (sumOfSquares) / degreesOfFreedom / n);
		if (out_tFromZero && standardError != 0.0)
			*out_tFromZero = mean / standardError;
		if (out_significanceFromZero)
			*out_significanceFromZero = ( standardError == 0.0 ? 0.0 :
					NUMstudentQ (fabs (mean) / standardError, degreesOfFreedom) );
		if (out_lowerLimit)
			*out_lowerLimit = mean - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit)
			*out_upperLimit = mean + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return mean;
}

double Table_getGroupMean_studentT (Table me, integer column, integer groupColumn, conststring32 group, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero)                *out_tFromZero                = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero)     *out_significanceFromZero     = undefined;
	if (out_lowerLimit)               *out_lowerLimit               = undefined;
	if (out_upperLimit)               *out_upperLimit               = undefined;
	if (column < 1 || column > my numberOfColumns)
		return undefined;
	Table_numericize_Assert (me, column);
	integer n = 0;
	longdouble sum = 0.0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string.get(), group)) {
				n += 1;
				sum += row -> cells [column]. number;
			}
		}
	}
	if (n < 1)
		return undefined;
	double mean = double (sum) / n;
	integer degreesOfFreedom = n - 1;
	if (out_numberOfDegreesOfFreedom)
		*out_numberOfDegreesOfFreedom = degreesOfFreedom;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		longdouble sumOfSquares = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow row = my rows.at [irow];
			if (row -> cells [groupColumn]. string) {
				if (str32equ (row -> cells [groupColumn]. string.get(), group)) {
					const double diff = row -> cells [column]. number - mean;
					sumOfSquares += diff * diff;
				}
			}
		}
		const double standardError = sqrt (double (sumOfSquares) / degreesOfFreedom / n);
		if (out_tFromZero && standardError != 0.0)
			*out_tFromZero = mean / standardError;
		if (out_significanceFromZero)
			*out_significanceFromZero = ( standardError == 0.0 ? 0.0 :
					NUMstudentQ (fabs (mean) / standardError, degreesOfFreedom) );
		if (out_lowerLimit)
			*out_lowerLimit = mean - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit)
			*out_upperLimit = mean + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return mean;
}

double Table_getGroupDifference_studentT (Table me, integer column, integer groupColumn, conststring32 group1, conststring32 group2, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit)
{
	if (out_tFromZero)                *out_tFromZero                = undefined;
	if (out_numberOfDegreesOfFreedom) *out_numberOfDegreesOfFreedom = undefined;
	if (out_significanceFromZero)     *out_significanceFromZero     = undefined;
	if (out_lowerLimit)               *out_lowerLimit               = undefined;
	if (out_upperLimit)               *out_upperLimit               = undefined;
	if (column < 1 || column > my numberOfColumns)
		return undefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns)
		return undefined;
	Table_numericize_Assert (me, column);
	integer n1 = 0, n2 = 0;
	longdouble sum1 = 0.0, sum2 = 0.0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string.get(), group1)) {
				n1 ++;
				sum1 += row -> cells [column]. number;
			} else if (str32equ (row -> cells [groupColumn]. string.get(), group2)) {
				n2 ++;
				sum2 += row -> cells [column]. number;
			}
		}
	}
	if (n1 < 1 || n2 < 1)
		return undefined;
	const integer degreesOfFreedom = n1 + n2 - 2;
	if (out_numberOfDegreesOfFreedom)
		*out_numberOfDegreesOfFreedom = degreesOfFreedom;
	const double mean1 = double (sum1) / n1;
	const double mean2 = double (sum2) / n2;
	const double difference = mean1 - mean2;
	if (degreesOfFreedom >= 1 && (out_tFromZero || out_significanceFromZero || out_lowerLimit || out_upperLimit)) {
		longdouble sumOfSquares = 0.0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			if (row -> cells [groupColumn]. string) {
				if (str32equ (row -> cells [groupColumn]. string.get(), group1)) {
					const double diff = row -> cells [column]. number - mean1;
					sumOfSquares += diff * diff;
				} else if (str32equ (row -> cells [groupColumn]. string.get(), group2)) {
					const double diff = row -> cells [column]. number - mean2;
					sumOfSquares += diff * diff;
				}
			}
		}
		const double standardError = sqrt (double (sumOfSquares) / degreesOfFreedom * (1.0 / n1 + 1.0 / n2));
		if (out_tFromZero && standardError != 0.0)
			*out_tFromZero = difference / standardError;
		if (out_significanceFromZero)
			*out_significanceFromZero = ( standardError == 0.0 ? 0.0 :
					NUMstudentQ (fabs (difference) / standardError, degreesOfFreedom) );
		if (out_lowerLimit)
			*out_lowerLimit = difference - standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
		if (out_upperLimit)
			*out_upperLimit = difference + standardError * NUMinvStudentQ (significanceLevel, degreesOfFreedom);
	}
	return difference;
}

double Table_getGroupDifference_wilcoxonRankSum (Table me, integer column, integer groupColumn, conststring32 group1, conststring32 group2,
	double *out_rankSum, double *out_significanceFromZero)
{
	if (out_rankSum)              *out_rankSum              = undefined;
	if (out_significanceFromZero) *out_significanceFromZero = undefined;
	if (column < 1 || column > my numberOfColumns)
		return undefined;
	if (groupColumn < 1 || groupColumn > my numberOfColumns)
		return undefined;
	Table_numericize_Assert (me, column);
	integer n1 = 0, n2 = 0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		const TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string.get(), group1))
				n1 ++;
			else if (str32equ (row -> cells [groupColumn]. string.get(), group2))
				n2 ++;
		}
	}
	const integer n = n1 + n2;
	if (n1 < 1 || n2 < 1 || n < 3)
		return undefined;
	autoTable ranks = Table_createWithoutColumnNames (n, 3);   // column 1 = group, 2 = value, 3 = rank
	for (integer irow = 1, jrow = 0; irow <= my rows.size; irow ++) {
		const TableRow row = my rows.at [irow];
		if (row -> cells [groupColumn]. string) {
			if (str32equ (row -> cells [groupColumn]. string.get(), group1)) {
				Table_setNumericValue (ranks.get(), ++ jrow, 1, 1.0);
				Table_setNumericValue (ranks.get(), jrow, 2, row -> cells [column]. number);
			} else if (str32equ (row -> cells [groupColumn]. string.get(), group2)) {
				Table_setNumericValue (ranks.get(), ++ jrow, 1, 2.0);
				Table_setNumericValue (ranks.get(), jrow, 2, row -> cells [column]. number);
			}
		}
	}
	Table_numericize_Assert (ranks.get(), 1);
	Table_numericize_Assert (ranks.get(), 2);
	Table_numericize_Assert (ranks.get(), 3);
	integer columns [1] = { 2 };   // we're gonna sort by column 2
	Table_sortRows_Assert (ranks.get(), constINTVEC (columns, 1));   // we sort by one column only
	double totalNumberOfTies3 = 0.0;
	for (integer irow = 1; irow <= ranks -> rows.size; irow ++) {
		TableRow row = ranks -> rows.at [irow];
		const double value = row -> cells [2]. number;
		integer rowOfLastTie = irow + 1;
		for (; rowOfLastTie <= ranks -> rows.size; rowOfLastTie ++) {
			const TableRow row2 = ranks -> rows.at [rowOfLastTie];
			const double value2 = row2 -> cells [2]. number;
			if (value2 != value)
				break;
		}
		rowOfLastTie --;
		const double averageRank = 0.5 * (double (irow) + double (rowOfLastTie));
		for (integer jrow = irow; jrow <= rowOfLastTie; jrow ++)
			Table_setNumericValue (ranks.get(), jrow, 3, averageRank);
		const integer numberOfTies = rowOfLastTie - irow + 1;
		totalNumberOfTies3 += double (numberOfTies - 1) * double (numberOfTies) * double (numberOfTies + 1);
	}
	Table_numericize_Assert (ranks.get(), 3);
	const double maximumRankSum = double (n1) * double (n2);
	longdouble rankSum = 0.0;
	for (integer irow = 1; irow <= ranks -> rows.size; irow ++) {
		const TableRow row = ranks -> rows.at [irow];
		if (row -> cells [1]. number == 1.0)
			rankSum += row -> cells [3]. number;
	}
	rankSum -= 0.5 * double (n1) * (double (n1) + 1.0);
	const double stdev = sqrt (maximumRankSum * (double (n) + 1.0 - totalNumberOfTies3 / n / (n - 1)) / 12.0);
	if (out_rankSum)
		*out_rankSum = double (rankSum);
	if (out_significanceFromZero)
		*out_significanceFromZero = NUMgaussQ (fabs (double (rankSum) - 0.5 * maximumRankSum) / stdev);
	return double (rankSum) / maximumRankSum;
}

double Table_getFisherF (Table me, integer col1, integer col2);
double Table_getOneWayAnovaSignificance (Table me, integer col1, integer col2);
double Table_getFisherFLowerLimit (Table me, integer col1, integer col2, double significanceLevel);
double Table_getFisherFUpperLimit (Table me, integer col1, integer col2, double significanceLevel);

bool Table_getExtrema (Table me, integer icol, double *minimum, double *maximum) {
	const integer n = my rows.size;
	if (icol < 1 || icol > my numberOfColumns || n == 0) {
		*minimum = *maximum = undefined;
		return false;
	}
	Table_numericize_Assert (me, icol);
	MelderExtremaWithInit extrema;
	for (integer irow = 1; irow <= n; irow ++) {
		const double value = my rows.at [irow] -> cells [icol]. number;
		extrema.update (value);
	}
	*minimum = extrema.min;
	*maximum = extrema.max;
	return true;
}

/*@praat
	#
	# Paul Boersma 2002-05-02
	#
	# bug reported by Michelle Porter on 2002-05-01
	#

	Create Table with column names: "table", 4, "A B"
	Set numeric value: 1, "A", 1
	Set numeric value: 2, "A", 2
	Set numeric value: 3, "A", 3
	Set numeric value: 4, "A", 4
	Set numeric value: 1, "B", 4
	Set numeric value: 2, "B", 3
	Set numeric value: 3, "B", 2
	Set numeric value: 4, "B", 1
	Scatter plot (mark): "A", 0, 0, "B", 0, 0, 1, "yes", "+"
	Remove
@*/
void Table_scatterPlot_mark (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, conststring32 mark, bool garnish)
{
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns)
		return;
	Table_numericize_Assert (me, xcolumn);
	Table_numericize_Assert (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax))
			return;
		if (xmin == xmax) {
			xmin -= 0.5;
			xmax += 0.5;
		}
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax))
			return;
		if (ymin == ymax) {
			ymin -= 0.5;
			ymax += 0.5;
		}
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
			Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label.get());
		Graphics_marksLeft (g, 2, true, true, false);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label.get());
	}
}

void Table_scatterPlot (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, integer markColumn, double fontSize, bool garnish)
{
	const double saveFontSize = Graphics_inqFontSize (g);
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns)
		return;
	Table_numericize_Assert (me, xcolumn);
	Table_numericize_Assert (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax))
			return;
		if (xmin == xmax) {
			xmin -= 0.5;
			xmax += 0.5;
		}
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax))
			return;
		if (ymin == ymax) {
			ymin -= 0.5;
			ymax += 0.5;
		}
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, fontSize);
	const integer n = my rows.size;
	for (integer irow = 1; irow <= n; irow ++) {
		const TableRow row = my rows.at [irow];
		conststring32 mark = row -> cells [markColumn]. string.get();
		if (mark)
			Graphics_text (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, mark);
	}
	Graphics_setFontSize (g, saveFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label.get());
		Graphics_marksLeft (g, 2, true, true, false);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label.get());
	}
}

void Table_drawEllipse_e (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, bool garnish)
{
	try {
		if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns)
			return;
		Table_numericize_Assert (me, xcolumn);
		Table_numericize_Assert (me, ycolumn);
		if (xmin == xmax) {
			if (! Table_getExtrema (me, xcolumn, & xmin, & xmax))
				return;
			if (xmin == xmax) {
				xmin -= 0.5;
				xmax += 0.5;
			}
		}
		if (ymin == ymax) {
			if (! Table_getExtrema (me, ycolumn, & ymin, & ymax))
				return;
			if (ymin == ymax) {
				ymin -= 0.5;
				ymax += 0.5;
			}
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

static conststring32 visibleString (conststring32 s) {
	return ( s && s [0] != U'\0' ? s : U"?" );
}

void Table_list (Table me, bool includeRowNumbers) {
	MelderInfo_open ();
	if (includeRowNumbers) {
		MelderInfo_write (U"row");
		if (my numberOfColumns > 0)
			MelderInfo_write (U"\t");
	}
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol > 1)
			MelderInfo_write (U"\t");
		MelderInfo_write (visibleString (my columnHeaders [icol]. label.get()));
	}
	MelderInfo_write (U"\n");
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		if (includeRowNumbers) {
			MelderInfo_write (irow);
			if (my numberOfColumns > 0)
				MelderInfo_write (U"\t");
		}
		TableRow row = my rows.at [irow];
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol > 1)
				MelderInfo_write (U"\t");
			MelderInfo_write (visibleString (row -> cells [icol]. string.get()));
		}
		MelderInfo_write (U"\n");
	}
	MelderInfo_close ();
}

static void writeToCharacterSeparatedFile (Table me, MelderFile file, char32 separator, bool interpretQuotes) {
	autoMelderString buffer;
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (icol != 1)
			MelderString_appendCharacter (& buffer, separator);
		conststring32 s = my columnHeaders [icol]. label.get();
		MelderString_append (& buffer, ( s && s [0] != U'\0' ? s : U"?" ));
	}
	MelderString_appendCharacter (& buffer, U'\n');
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		TableRow row = my rows.at [irow];
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			if (icol != 1)
				MelderString_appendCharacter (& buffer, separator);
			conststring32 s = row -> cells [icol]. string.get();
			if (! s)
				s = U"";
			if (s [0] == U'\0') {
				bool separatorIsInvisible = ( separator == U'\t' );
				bool emptyStringsWillBeVisibleEnough = ! separatorIsInvisible;   // it's fine to have ",,,,,," in a comma environment
				bool replaceEmptyStringsWithSomethingVisible = ! emptyStringsWillBeVisibleEnough;
				if (replaceEmptyStringsWithSomethingVisible) {
					MelderString_appendCharacter (& buffer, U'?');
				}
			} else if (str32chr (s, separator)) {
				if (interpretQuotes) {
					MelderString_appendCharacter (& buffer, U'\"');
					MelderString_append (& buffer, s);
					MelderString_appendCharacter (& buffer, U'\"');
				} else {
					conststring32 separatorText =
						separator == U'\t' ? U"a separating tab" :
						separator == U',' ? U"a separating comma" :
						separator == U';' ? U"a separating semicolon" :
						U"a separator symbol";
					Melder_throw (U"Row ", irow, U" contains ", separatorText, U" inside a cell without providing the possiblity of quoting.");
				}
			} else {
				MelderString_append (& buffer, s);
			}
		}
		MelderString_appendCharacter (& buffer, U'\n');
	}
	MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
}

void Table_writeToTabSeparatedFile (Table me, MelderFile file) {
	try {
		writeToCharacterSeparatedFile (me, file, U'\t', false);
	} catch (MelderError) {
		Melder_throw (me, U": not written to tab-separated file.");
	}
}

void Table_writeToCommaSeparatedFile (Table me, MelderFile file) {
	try {
		writeToCharacterSeparatedFile (me, file, U',', true);
	} catch (MelderError) {
		Melder_throw (me, U": not written to comma-separated file.");
	}
}

void Table_writeToSemicolonSeparatedFile (Table me, MelderFile file) {
	try {
		writeToCharacterSeparatedFile (me, file, U';', true);
	} catch (MelderError) {
		Melder_throw (me, U": not written to comma-separated file.");
	}
}

autoTable Table_readFromTableFile (MelderFile file) {
	try {
		autostring32 string = MelderFile_readText (file);
		/*
			Count columns.
		*/
		integer numberOfColumns = 0;
		char32 *p = & string [0];
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\n' || kar == U'\0')
				break;
			if (kar == U' ' || kar == U'\t')
				continue;
			numberOfColumns ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\n' || kar == U'\0')
				break;
		}
		if (numberOfColumns < 1)
			Melder_throw (U"No columns.");

		/*
			Count elements.
		*/
		p = & string [0];
		integer numberOfElements = 0;
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0')
				break;
			if (kar == U' ' || kar == U'\t' || kar == U'\n')
				continue;
			numberOfElements ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\0')
				break;
		}

		/*
			Check if all columns are complete.
		*/
		if (numberOfElements == 0 || numberOfElements % numberOfColumns != 0)
			Melder_throw (U"The number of elements (", numberOfElements, U") is not a multiple of the number of columns (", numberOfColumns, U").");

		/*
			Create empty table.
		*/
		const integer numberOfRows = numberOfElements / numberOfColumns - 1;
		autoTable me = Table_create (numberOfRows, numberOfColumns);

		/*
			Read elements.
		*/
		p = & string [0];
		for (integer icol = 1; icol <= numberOfColumns; icol ++) {
			while (*p == U' ' || *p == U'\t') {
				Melder_assert (*p != U'\0');
				p ++;
			}
			static MelderString buffer;
			MelderString_empty (& buffer);
			while (*p != U' ' && *p != U'\t' && *p != U'\n') {
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			Table_setColumnLabel (me.get(), icol, buffer.string);
			MelderString_empty (& buffer);
		}
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= numberOfColumns; icol ++) {
				while (*p == U' ' || *p == U'\t' || *p == U'\n') { Melder_assert (*p != U'\0'); p ++; }
				static MelderString buffer;
				MelderString_empty (& buffer);
				while (*p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\0') {
					MelderString_appendCharacter (& buffer, *p);
					p ++;
				}
				row -> cells [icol]. string = Melder_dup_f (buffer.string);
				MelderString_empty (& buffer);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Table object not read from space-separated text file ", file, U".");
	}
}

autoTable Table_readFromCharacterSeparatedTextFile (MelderFile file, char32 separator, bool interpretQuotes) {
	try {
		autostring32 string = MelderFile_readText (file);

		/*
			Kill final new-line symbols.
	 	*/
		for (int64 length = str32len (string.get());
		     length > 0 && string [length - 1] == U'\n';
			 length = str32len (string.get()))
		{
			string [length - 1] = U'\0';
		}

		/*
			Count columns.
 		*/
		integer numberOfColumns = 1;
		const char32 *p = & string [0];
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0') Melder_throw (U"No rows.");
			if (kar == U'\n') break;
			if (kar == separator) numberOfColumns ++;
		}

		/*
			Count rows.
	 	*/
		integer numberOfRows = 1;
	 	{// scope
			bool withinQuotes = false;
			for (;;) {
				char32 kar = *p++;
				if (interpretQuotes && kar == U'\"')
					withinQuotes = ! withinQuotes;
				if (! withinQuotes) {
					if (kar == U'\0') break;
					if (kar == U'\n') numberOfRows ++;
				}
			}
		}

		/*
			Create empty table.
		*/
		autoTable me = Table_create (numberOfRows, numberOfColumns);

		/*
			Read column names.
	 	*/
		autoMelderString buffer;
		p = & string [0];
		for (integer icol = 1; icol <= numberOfColumns; icol ++) {
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
			Read cells.
	 	*/
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= numberOfColumns; icol ++) {
				MelderString_empty (& buffer);
				bool withinQuotes = false;
				while (*p != U'\0' && (*p != separator && *p != U'\n' || withinQuotes)) {
					if (interpretQuotes && *p == U'\"') {
						withinQuotes = ! withinQuotes;
					} else {
						MelderString_appendCharacter (& buffer, *p);
					}
					p ++;
				}
				if (*p == U'\0') {
					if (irow != numberOfRows)
						Melder_fatal (U"irow ", irow, U", nrow ", numberOfRows, U", icol ", icol, U", ncol ", numberOfColumns);
					if (icol != numberOfColumns)
						Melder_throw (U"Last row incomplete.");
					if (withinQuotes) {
						if (str32chr (buffer.string, U'\n'))
							Melder_warning (U"The last cell contains an unmatched double-quote (\") and also multiple lines, "
									"so perhaps multiple lines were unintentionally combined into one cell. "
									"The problem may be in row ", irow, U".");
						else
							Melder_warning (U"The last cell contains an unmatched double-quote (\"), "
									"so perhaps multiple cells were unintentionally combined. "
									"The problem is in row ", irow, U".");
					}
				} else if (*p == U'\n') {
					if (icol != numberOfColumns)
						Melder_throw (U"Row ", irow, U" incomplete.");
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
