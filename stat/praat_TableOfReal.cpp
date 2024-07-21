/* praat_TableOfReal.cpp
 *
 * Copyright (C) 1992-2018,2021-2024 Paul Boersma
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

#include "Matrix.h"
#include "praat_TableOfReal.h"

// MARK: - TABLEOFREAL

// MARK: Help

DIRECT (HELP_TableOfReal_help) {
	HELP (U"TableOfReal")
}

// MARK: Draw

FORM (GRAPHICS_TableOfReal_drawAsNumbers, U"Draw as numbers", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	CHOICE (format, U"Format", 3)
		OPTION (U"decimal")
		OPTION (U"exponential")
		OPTION (U"free")
		OPTION (U"rational")
	NATURAL (precision, U"Precision", U"5")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsNumbers (me, GRAPHICS, fromRow, toRow, format, precision);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawAsNumbers_if, U"Draw as numbers if...", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	CHOICE (format, U"Format", 3)
		OPTION (U"decimal")
		OPTION (U"exponential")
		OPTION (U"free")
		OPTION (U"rational")
	NATURAL (precision, U"Precision", U"5")
	FORMULA (condition, U"Condition", U"self <> 0")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsNumbers_if (me, GRAPHICS, fromRow, toRow, format, precision, condition, interpreter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawAsSquares, U"Draw table as squares", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	NATURAL (fromColumn, U"From column", U"1")
	INTEGER (toColumn, U"To column", U"0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawAsSquares (me, GRAPHICS, fromRow, toRow, fromColumn, toColumn, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawHorizontalLines, U"Draw horizontal lines", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawHorizontalLines (me, GRAPHICS, fromRow, toRow);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawLeftAndRightLines, U"Draw left and right lines", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawLeftAndRightLines (me, GRAPHICS, fromRow, toRow);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawTopAndBottomLines, U"Draw top and bottom lines", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawTopAndBottomLines (me, GRAPHICS, fromRow, toRow);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_TableOfReal_drawVerticalLines, U"Draw vertical lines", nullptr) {
	NATURAL (fromRow, U"From row", U"1")
	INTEGER (toRow, U"To row", U"0 (= all)")
	OK
DO
	GRAPHICS_EACH (TableOfReal)
		TableOfReal_drawVerticalLines (me, GRAPHICS, fromRow, toRow);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (INTEGER_TableOfReal_getColumnIndex, U"Get column index", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer result = TableOfReal_columnLabelToIndex (me, columnLabel);
	QUERY_ONE_FOR_REAL_END (U" (index of column ", columnLabel, U")")
}
	
FORM (STRING_TableOfReal_getColumnLabel, U"Get column label", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (TableOfReal)
		if (columnNumber > my numberOfColumns)
			Melder_throw (me, U": your column number should not be greater than the number of columns.");
		conststring32 result = my columnLabels ? my columnLabels [columnNumber].get() : U"";
	QUERY_ONE_FOR_STRING_END
}
	
FORM (REAL_TableOfReal_getColumnMean_index, U"Get column mean", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		if (columnNumber > my numberOfColumns)
			Melder_throw (me, U": your column number should not be greater than the number of columns.");
		const double result = TableOfReal_getColumnMean (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (mean of column ", columnNumber, U")")
}

FORM (REAL_TableOfReal_getColumnMean_label, U"Get column mean", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer columnNumber = TableOfReal_columnLabelToIndex (me, columnLabel);
		if (columnNumber == 0)
			Melder_throw (me, U": your column label does not exist.");
		const double result = TableOfReal_getColumnMean (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (mean of column ", columnLabel, U")")
}

FORM (REAL_TableOfReal_getColumnStdev_index, U"Get column standard deviation", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		if (columnNumber > my numberOfColumns)
			Melder_throw (me, U": your column number should not be greater than the number of columns.");
		const double result = TableOfReal_getColumnStdev (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (standard deviation of column ", columnNumber, U")")
}

FORM (REAL_TableOfReal_getColumnStdev_label, U"Get column standard deviation", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer columnNumber = TableOfReal_columnLabelToIndex (me, columnLabel);
		if (columnNumber == 0)
			Melder_throw (me, U": column label does not exist.");
		double result = TableOfReal_getColumnStdev (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (standard deviation of column ", columnLabel, U")")
}

DIRECT (INTEGER_TableOfReal_getNumberOfColumns) {
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer result = my numberOfColumns;
	QUERY_ONE_FOR_REAL_END (U" columns")
}

DIRECT (INTEGER_TableOfReal_getNumberOfRows) {
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer result = my numberOfRows;
	QUERY_ONE_FOR_REAL_END (U" rows")
}

FORM (INTEGER_TableOfReal_getRowIndex, U"Get row index", nullptr) {
	SENTENCE (rowLabel, U"Row label", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		const integer result = TableOfReal_rowLabelToIndex (me, rowLabel);
	QUERY_ONE_FOR_REAL_END (U" (index of row ", rowLabel, U")")
}

FORM (STRING_TableOfReal_getRowLabel, U"Get row label", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (TableOfReal)
		if (rowNumber > my numberOfRows)
			Melder_throw (me, U": your row number should not be greater than the number of rows.");
		conststring32 result = my rowLabels ? my rowLabels [rowNumber].get() : U"";
	QUERY_ONE_FOR_STRING_END
}

FORM (REAL_TableOfReal_getValue, U"Get value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (TableOfReal)
		TableOfReal_checkRowNumberWithinRange (me, rowNumber);
		TableOfReal_checkColumnNumberWithinRange (me, columnNumber);
		const double result = my data [rowNumber] [columnNumber];
	QUERY_ONE_FOR_REAL_END (U" (value in column ", columnNumber, U" of row ", rowNumber, U")")
}

// MARK: Modify

FORM (MODIFY_TableOfReal_formula, U"TableOfReal: Formula", U"Formula...") {
	COMMENT (U"for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	FORMULA (formula, U"Formula", U"if col = 5 then self + self [6] else self fi")
	OK
DO
	MODIFY_EACH_WEAK (TableOfReal)
		TableOfReal_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_TableOfReal_insertColumn, U"Insert column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_insertColumn (me, columnNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_insertRow, U"Insert row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_insertRow (me, rowNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_removeColumn, U"Remove column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_removeColumn (me, columnNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_removeRow, U"Remove row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_removeRow (me, rowNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_setColumnLabel_index, U"Set column label", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	SENTENCE (label, U"Label", U"")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_setColumnLabel (me, columnNumber, label);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_setColumnLabel_label, U"Set column label", nullptr) {
	SENTENCE (oldLabel, U"Old label", U"")
	SENTENCE (newLabel, U"New label", U"")
	OK
DO
	MODIFY_EACH (TableOfReal)
		const integer columnNumber = TableOfReal_columnLabelToIndex (me, oldLabel);
		TableOfReal_setColumnLabel (me, columnNumber, newLabel);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_setRowLabel_index, U"Set row label", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (label, U"Label", U"")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_setRowLabel (me, rowNumber, label);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_setValue, U"Set value", U"TableOfReal: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL_OR_UNDEFINED (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_checkRowNumberWithinRange (me, rowNumber);
		TableOfReal_checkColumnNumberWithinRange (me, columnNumber);
		my data [rowNumber] [columnNumber] = newValue;
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_setRowLabel_label, U"Set row label", nullptr) {
	SENTENCE (oldLabel, U"Old label", U"")
	WORD (newLabel, U"New label", U"")
	OK
DO
	MODIFY_EACH (TableOfReal)
		const integer rowNumber = TableOfReal_rowLabelToIndex (me, oldLabel);
		TableOfReal_setRowLabel (me, rowNumber, newLabel);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_sortByColumn, U"Sort rows by column", nullptr) {
	INTEGER (column, U"Column", U"1")
	INTEGER (secondaryColumn, U"Secondary column", U"0")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_sortByColumn (me, column, secondaryColumn);
	MODIFY_EACH_END
}

FORM (MODIFY_TableOfReal_sortByLabel, U"Sort rows by label", nullptr) {
	COMMENT (U"Secondary sorting keys:")
	INTEGER (column1, U"Column1", U"1")
	INTEGER (column2, U"Column2", U"0")
	OK
DO
	MODIFY_EACH (TableOfReal)
		TableOfReal_sortByLabel (me, column1, column2);
	MODIFY_EACH_END
}

// MARK: Extract

DIRECT (NEW_TableOfReal_extractColumnLabelsAsStrings) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoStrings result = TableOfReal_extractColumnLabelsAsStrings (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_TableOfReal_extractColumnsByNumber, U"Extract columns by number", nullptr) {
	COMMENT (U"Create a new TableOfReal from the following existing columns.")
	NATURALVECTOR (columnNumbers, U"Column numbers", RANGES_, U"1 2")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractColumnsByNumber (me, columnNumbers);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cols")
}

FORM (NEW_TableOfReal_extractColumnsWhere, U"Extract columns where", nullptr) {
	COMMENT (U"Extract all columns with at least one cell where the following condition holds.")
	FORMULA (condition, U"Condition", U"col mod 3 = 0 ; this example extracts every third column")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractColumnsWhere (me, condition, interpreter);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cols")
}

FORM (NEW_TableOfReal_extractColumnsWhoseLabel, U"Extract columns whose label...", nullptr) {
	OPTIONMENU_ENUM (kMelder_string, extractAllColumnsWhoseLabel,
			U"Extract all columns whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"a")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractColumnsWhoseLabel (me, extractAllColumnsWhoseLabel, ___theText);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TableOfReal_extractColumnsWhereRow, U"Extract columns where row", nullptr) {
	NATURAL (extractAllColumnsWhereRow, U"Extract all columns where row...", U"1")
	OPTIONMENU_ENUM (kMelder_number, ___is___, U"...is...", kMelder_number::DEFAULT)
	REAL (___theValue, U"...the value", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractColumnsWhereRow (me, extractAllColumnsWhereRow, ___is___, ___theValue);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", extractAllColumnsWhereRow, U"_", Melder_iround (___theValue))
}

DIRECT (NEW_TableOfReal_extractRowLabelsAsStrings) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoStrings result = TableOfReal_extractRowLabelsAsStrings (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_TableOfReal_extractRowsByNumber, U"Extract rows by number", nullptr) {
	COMMENT (U"Create a new TableOfReal from the following existing rows.")
	NATURALVECTOR (rowNumbers, U"Row numbers", RANGES_, U"1 2")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractRowsByNumber (me, rowNumbers);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rows")
}

FORM (NEW_TableOfReal_extractRowsWhere, U"Extract rows where", nullptr) {
	COMMENT (U"Extract all rows with at least one cell where the following condition holds.")
	FORMULA (condition, U"Condition", U"row mod 3 = 0 ; this example extracts every third row")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractRowsWhere (me, condition, interpreter);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rows")
}

FORM (NEW_TableOfReal_extractRowsWhereColumn, U"Extract rows where column", nullptr) {
	NATURAL (extractAllRowsWhereColumn, U"Extract all rows where column...", U"1")
	OPTIONMENU_ENUM (kMelder_number, ___is___, U"...is...", kMelder_number::DEFAULT)
	REAL (___theValue, U"...the value", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractRowsWhereColumn (me, extractAllRowsWhereColumn, ___is___, ___theValue);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", extractAllRowsWhereColumn, U"_", Melder_iround (___theValue))
}

FORM (NEW_TableOfReal_extractRowsWhoseLabel, U"Extract rows whose label", nullptr) {
	OPTIONMENU_ENUM (kMelder_string, extractAllRowsWhoseLabel,
			U"Extract all rows whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"a")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTableOfReal result = TableOfReal_extractRowsWhoseLabel (me, extractAllRowsWhoseLabel, ___theText);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", ___theText)
}

// MARK: Convert

DIRECT (NEW1_TablesOfReal_append) {
	COMBINE_ALL_TO_ONE (TableOfReal)
		autoTableOfReal result = TablesOfReal_appendMany (& list);
	COMBINE_ALL_TO_ONE_END (U"appended")
}

DIRECT (NEW_TableOfReal_to_Matrix) {
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoMatrix result = TableOfReal_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_TableOfReal_to_Table, U"TableOfReal: To Table", nullptr) {
	SENTENCE (labelOfFirstColumn, U"Label of first column", U"rowLabel")
	OK
DO
	CONVERT_EACH_TO_ONE (TableOfReal)
		autoTable result = TableOfReal_to_Table (me, labelOfFirstColumn);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM_SAVE (SAVE_TableOfReal_writeToHeaderlessSpreadsheetFile, U"Save TableOfReal as spreadsheet", 0, U"txt") {
	SAVE_ONE (TableOfReal)
		TableOfReal_writeToHeaderlessSpreadsheetFile (me, file);
	SAVE_ONE_END
}

void praat_TableOfReal_init (ClassInfo klas) {
	if (klas == classTableOfReal)
		praat_addAction1 (classTableOfReal, 0, U"TableOfReal help", nullptr, 0, HELP_TableOfReal_help);
	praat_addAction1 (klas, 1, U"Save as headerless spreadsheet file... || Write to headerless spreadsheet file...",
			nullptr, 0, SAVE_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Draw as numbers...",
				nullptr, 1, GRAPHICS_TableOfReal_drawAsNumbers);
		praat_addAction1 (klas, 0, U"Draw as numbers if...",
				nullptr, 1, GRAPHICS_TableOfReal_drawAsNumbers_if);
		praat_addAction1 (klas, 0, U"Draw as squares...",
				nullptr, 1, GRAPHICS_TableOfReal_drawAsSquares);
		praat_addAction1 (klas, 0, U"-- draw lines --",
				nullptr, 1, nullptr);
		praat_addAction1 (klas, 0, U"Draw vertical lines...",
				nullptr, 1, GRAPHICS_TableOfReal_drawVerticalLines);
		praat_addAction1 (klas, 0, U"Draw horizontal lines...",
				nullptr, 1, GRAPHICS_TableOfReal_drawHorizontalLines);
		praat_addAction1 (klas, 0, U"Draw left and right lines...",
				nullptr, 1, GRAPHICS_TableOfReal_drawLeftAndRightLines);
		praat_addAction1 (klas, 0, U"Draw top and bottom lines...",
				nullptr, 1, GRAPHICS_TableOfReal_drawTopAndBottomLines);
	praat_addAction1 (klas, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 1, U"Get number of rows",
				nullptr, 1, INTEGER_TableOfReal_getNumberOfRows);
		praat_addAction1 (klas, 1, U"Get number of columns",
				nullptr, 1, INTEGER_TableOfReal_getNumberOfColumns);
		praat_addAction1 (klas, 1, U"Get row label...",
				nullptr, 1, STRING_TableOfReal_getRowLabel);
		praat_addAction1 (klas, 1, U"Get column label...",
				nullptr, 1, STRING_TableOfReal_getColumnLabel);
		praat_addAction1 (klas, 1, U"Get row index...",
				nullptr, 1, INTEGER_TableOfReal_getRowIndex);
		praat_addAction1 (klas, 1, U"Get column index...",
				nullptr, 1, INTEGER_TableOfReal_getColumnIndex);
		praat_addAction1 (klas, 1, U"-- get value --",
				nullptr, 1, nullptr);
		praat_addAction1 (klas, 1, U"Get value...",
				nullptr, 1, REAL_TableOfReal_getValue);
	if (klas == classTableOfReal) {
		praat_addAction1 (klas, 1, U"-- get statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (klas, 1, U"Get column mean (index)...",
				nullptr, 1, REAL_TableOfReal_getColumnMean_index);
		praat_addAction1 (klas, 1, U"Get column mean (label)...",
				nullptr, 1, REAL_TableOfReal_getColumnMean_label);
		praat_addAction1 (klas, 1, U"Get column stdev (index)...",
				nullptr, 1, REAL_TableOfReal_getColumnStdev_index);
		praat_addAction1 (klas, 1, U"Get column stdev (label)...",
				nullptr, 1, REAL_TableOfReal_getColumnStdev_label);
	}
	praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Formula...",
				nullptr, 1, MODIFY_TableOfReal_formula);
		praat_addAction1 (klas, 0, U"Set value...",
				nullptr, 1, MODIFY_TableOfReal_setValue);
		praat_addAction1 (klas, 0, U"Sort by label...",
				nullptr, 1, MODIFY_TableOfReal_sortByLabel);
		praat_addAction1 (klas, 0, U"Sort by column...",
				nullptr, 1, MODIFY_TableOfReal_sortByColumn);
		praat_addAction1 (klas, 0, U"-- structure --",
				nullptr, 1, nullptr);
		praat_addAction1 (klas, 0, U"Remove row (index)...",
				nullptr, 1, MODIFY_TableOfReal_removeRow);
		praat_addAction1 (klas, 0, U"Remove column (index)...",
				nullptr, 1, MODIFY_TableOfReal_removeColumn);
		praat_addAction1 (klas, 0, U"Insert row (index)...",
				nullptr, 1, MODIFY_TableOfReal_insertRow);
		praat_addAction1 (klas, 0, U"Insert column (index)...",
				nullptr, 1, MODIFY_TableOfReal_insertColumn);
		praat_addAction1 (klas, 0, U"-- set --",
				nullptr, 1, nullptr);
		praat_addAction1 (klas, 0, U"Set row label (index)...",
				nullptr, 1, MODIFY_TableOfReal_setRowLabel_index);
		praat_addAction1 (klas, 0, U"Set row label (label)...",
				nullptr, 1, MODIFY_TableOfReal_setRowLabel_label);
		praat_addAction1 (klas, 0, U"Set column label (index)...",
				nullptr, 1, MODIFY_TableOfReal_setColumnLabel_index);
		praat_addAction1 (klas, 0, U"Set column label (label)...",
				nullptr, 1, MODIFY_TableOfReal_setColumnLabel_label);
	praat_addAction1 (klas, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Append",
				nullptr, 1, NEW1_TablesOfReal_append);
	praat_addAction1 (klas, 0, U"Extract part -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Extract rows by number... || Extract row ranges...",
				nullptr, 1, NEW_TableOfReal_extractRowsByNumber);
		praat_addAction1 (klas, 0, U"Extract rows where column...",
				nullptr, 1, NEW_TableOfReal_extractRowsWhereColumn);
		praat_addAction1 (klas, 0, U"Extract rows whose label... || Extract rows where label...",
				nullptr, 1, NEW_TableOfReal_extractRowsWhoseLabel);
		praat_addAction1 (klas, 0, U"Extract rows where...",
				nullptr, 1, NEW_TableOfReal_extractRowsWhere);
		praat_addAction1 (klas, 0, U"Extract columns by number... || Extract column ranges...",
				nullptr, 1, NEW_TableOfReal_extractColumnsByNumber);
		praat_addAction1 (klas, 0, U"Extract columns where row...",
				nullptr, 1, NEW_TableOfReal_extractColumnsWhereRow);
		praat_addAction1 (klas, 0, U"Extract columns whose label... || Extract columns where label...",
				nullptr, 1, NEW_TableOfReal_extractColumnsWhoseLabel);   // alternative GuiMenu_DEPRECATED_2021
		praat_addAction1 (klas, 0, U"Extract columns where...",
				nullptr, 1, NEW_TableOfReal_extractColumnsWhere);
	praat_addAction1 (klas, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Extract row labels as Strings",
				nullptr, 1, NEW_TableOfReal_extractRowLabelsAsStrings);
		praat_addAction1 (klas, 0, U"Extract column labels as Strings",
				nullptr, 1, NEW_TableOfReal_extractColumnLabelsAsStrings);
	praat_addAction1 (klas, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"To Table...",
				nullptr, 1, NEW_TableOfReal_to_Table);
		praat_addAction1 (klas, 0, U"To Matrix",
				nullptr, 1, NEW_TableOfReal_to_Matrix);
}

/* End of file praat_TableOfReal.cpp */
