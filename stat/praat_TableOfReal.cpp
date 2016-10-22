/* praat_TableOfReal.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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

#undef iam
#define iam iam_LOOP

/***** TABLEOFREAL *****/
#pragma mark -
#pragma mark TABLEOFREAL

DIRECT2 (TablesOfReal_append) {
	OrderedOf<structTableOfReal> list;
	LOOP {
		iam (TableOfReal);
		list. addItem_ref (me);
	}
	autoTableOfReal thee = TablesOfReal_appendMany (& list);
	praat_new (thee.move(), U"appended");
END2 }

FORM (TableOfReal_drawAsNumbers, U"Draw as numbers", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	RADIO (U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"5")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsNumbers (me, GRAPHICS,
			GET_INTEGER (U"From row"), GET_INTEGER (U"To row"),
			GET_INTEGER (U"Format"), GET_INTEGER (U"Precision"));
	}
END2 }

FORM (TableOfReal_drawAsNumbers_if, U"Draw as numbers if...", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	RADIO (U"Format", 3)
		RADIOBUTTON (U"decimal")
		RADIOBUTTON (U"exponential")
		RADIOBUTTON (U"free")
		RADIOBUTTON (U"rational")
	NATURAL (U"Precision", U"5")
	LABEL (U"", U"Condition:")
	TEXTFIELD (U"condition", U"self <> 0")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsNumbers_if (me, GRAPHICS,
			GET_INTEGER (U"From row"), GET_INTEGER (U"To row"),
			GET_INTEGER (U"Format"), GET_INTEGER (U"Precision"), GET_STRING (U"condition"), interpreter);
	}
END2 }

FORM (TableOfReal_drawAsSquares, U"Draw table as squares", nullptr) {
	INTEGER (U"From row", U"1")
	INTEGER (U"To row", U"0")
	INTEGER (U"From column", U"1")
	INTEGER (U"To column", U"0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsSquares (me, GRAPHICS, 
			GET_INTEGER (U"From row"), GET_INTEGER (U"To row"),
			GET_INTEGER (U"From column"), GET_INTEGER (U"To column"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (TableOfReal_drawHorizontalLines, U"Draw horizontal lines", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawHorizontalLines (me, GRAPHICS, GET_INTEGER (U"From row"), GET_INTEGER (U"To row"));
	}
END2 }

FORM (TableOfReal_drawLeftAndRightLines, U"Draw left and right lines", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawLeftAndRightLines (me, GRAPHICS, GET_INTEGER (U"From row"), GET_INTEGER (U"To row"));
	}
END2 }

FORM (TableOfReal_drawTopAndBottomLines, U"Draw top and bottom lines", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawTopAndBottomLines (me, GRAPHICS, GET_INTEGER (U"From row"), GET_INTEGER (U"To row"));
	}
END2 }

FORM (TableOfReal_drawVerticalLines, U"Draw vertical lines", nullptr) {
	NATURAL (U"From row", U"1")
	INTEGER (U"To row", U"0 (= all)")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawVerticalLines (me, GRAPHICS, GET_INTEGER (U"From row"), GET_INTEGER (U"To row"));
	}
END2 }

DIRECT2 (TableOfReal_extractColumnLabelsAsStrings) {
	LOOP {
		iam (TableOfReal);
		autoStrings thee = TableOfReal_extractColumnLabelsAsStrings (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TableOfReal_extractColumnRanges, U"Extract column ranges", nullptr) {
	LABEL (U"", U"Create a new TableOfReal from the following columns:")
	TEXTFIELD (U"ranges", U"1 2")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnRanges (me, GET_STRING (U"ranges"));
		praat_new (thee.move(), my name, U"_cols");
	}
END2 }

FORM (TableOfReal_extractColumnsWhere, U"Extract columns where", nullptr) {
	LABEL (U"", U"Extract all columns with at least one cell where:")
	TEXTFIELD (U"condition", U"col mod 3 = 0 ; this example extracts every third column")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhere (me, GET_STRING (U"condition"), interpreter);
		praat_new (thee.move(), my name, U"_cols");
	}
END2 }

FORM (TableOfReal_extractColumnsWhereLabel, U"Extract column where label", nullptr) {
	OPTIONMENU_ENUM (U"Extract all columns whose label...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"a")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhereLabel (me, GET_ENUM (kMelder_string, U"Extract all columns whose label..."), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TableOfReal_extractColumnsWhereRow, U"Extract columns where row", nullptr) {
	NATURAL (U"Extract all columns where row...", U"1")
	OPTIONMENU_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the value", U"0.0")
	OK2
DO
	long row = GET_INTEGER (U"Extract all columns where row...");
	double value = GET_REAL (U"...the value");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhereRow (me, row, GET_ENUM (kMelder_number, U"...is..."), value);
		praat_new (thee.move(), my name, U"_", row, U"_", lround (value));
	}
END2 }

DIRECT2 (TableOfReal_extractRowLabelsAsStrings) {
	LOOP {
		iam (TableOfReal);
		autoStrings thee = TableOfReal_extractRowLabelsAsStrings (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TableOfReal_extractRowRanges, U"Extract row ranges", nullptr) {
	LABEL (U"", U"Create a new TableOfReal from the following rows:")
	TEXTFIELD (U"ranges", U"1 2")
	LABEL (U"", U"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowRanges (me, GET_STRING (U"ranges"));
		praat_new (thee.move(), my name, U"_rows");
	}
END2 }

FORM (TableOfReal_extractRowsWhere, U"Extract rows where", nullptr) {
	LABEL (U"", U"Extract all rows with at least one cell where:")
	TEXTFIELD (U"condition", U"row mod 3 = 0 ; this example extracts every third row")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhere (me, GET_STRING (U"condition"), interpreter);
		praat_new (thee.move(), my name, U"_rows");
	}
END2 }

FORM (TableOfReal_extractRowsWhereColumn, U"Extract rows where column", nullptr) {
	NATURAL (U"Extract all rows where column...", U"1")
	OPTIONMENU_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the value", U"0.0")
	OK2
DO
	long column = GET_INTEGER (U"Extract all rows where column...");
	double value = GET_REAL (U"...the value");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhereColumn (me,
			column, GET_ENUM (kMelder_number, U"...is..."), value);
		praat_new (thee.move(), my name, U"_", column, U"_", lround (value));
	}
END2 }

FORM (TableOfReal_extractRowsWhereLabel, U"Extract rows where label", nullptr) {
	OPTIONMENU_ENUM (U"Extract all rows whose label...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"a")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhereLabel (me, GET_ENUM (kMelder_string, U"Extract all rows whose label..."), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TableOfReal_formula, U"TableOfReal: Formula", U"Formula...") {
	LABEL (U"", U"for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	TEXTFIELD (U"formula", U"if col = 5 then self + self [6] else self fi")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		try {
			TableOfReal_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END2 }

FORM (TableOfReal_getColumnIndex, U"Get column index", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (U"Column label"));
		Melder_information (columnNumber);
	}
END2 }
	
FORM (TableOfReal_getColumnLabel, U"Get column label", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (U"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, U": column number must not be greater than number of columns.");
		Melder_information (my columnLabels ? my columnLabels [columnNumber] : U"");
	}
END2 }
	
FORM (TableOfReal_getColumnMean_index, U"Get column mean", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (U"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, U": column number must not be greater than number of columns.");
		double columnMean = TableOfReal_getColumnMean (me, columnNumber);
		Melder_informationReal (columnMean, nullptr);
	}
END2 }
	
FORM (TableOfReal_getColumnMean_label, U"Get column mean", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (U"Column label"));
		if (columnNumber == 0) Melder_throw (me, U": column label does not exist.");
		double columnMean = TableOfReal_getColumnMean (me, columnNumber);
		Melder_informationReal (columnMean, nullptr);
	}
END2 }
	
FORM (TableOfReal_getColumnStdev_index, U"Get column standard deviation", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (U"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, U": column number must not be greater than number of columns.");
		double stdev = TableOfReal_getColumnStdev (me, columnNumber);
		Melder_informationReal (stdev, nullptr);
	}
END2 }
	
FORM (TableOfReal_getColumnStdev_label, U"Get column standard deviation", nullptr) {
	SENTENCE (U"Column label", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (U"Column label"));
		if (columnNumber == 0) Melder_throw (me, U": column label does not exist.");
		double stdev = TableOfReal_getColumnStdev (me, columnNumber);
		Melder_informationReal (stdev, nullptr);
	}
END2 }

DIRECT2 (TableOfReal_getNumberOfColumns) {
	LOOP {
		iam (TableOfReal);
		Melder_information (my numberOfColumns);
	}
END2 }

DIRECT2 (TableOfReal_getNumberOfRows) {
	LOOP {
		iam (TableOfReal);
		Melder_information (my numberOfRows);
	}
END2 }

FORM (TableOfReal_getRowIndex, U"Get row index", nullptr) {
	SENTENCE (U"Row label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = TableOfReal_rowLabelToIndex (me, GET_STRING (U"Row label"));
		Melder_information (rowNumber);
	}
END2 }
	
FORM (TableOfReal_getRowLabel, U"Get row label", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (U"Row number");
		if (rowNumber > my numberOfRows) Melder_throw (me, U": row number must not be greater than number of rows.");
		Melder_information (my rowLabels ? my rowLabels [rowNumber] : U"");
	}
END2 }

FORM (TableOfReal_getValue, U"Get value", nullptr) {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (U"Row number"), columnNumber = GET_INTEGER (U"Column number");
		if (rowNumber > my numberOfRows) Melder_throw (me, U": row number must not exceed number of rows.");
		if (columnNumber > my numberOfColumns) Melder_throw (me, U": column number must not exceed number of columns.");
		Melder_informationReal (my data [rowNumber] [columnNumber], nullptr);
	}
END2 }

DIRECT2 (TableOfReal_help) {
	Melder_help (U"TableOfReal");
END2 }

FORM (TableOfReal_insertColumn, U"Insert column", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_insertColumn (me, GET_INTEGER (U"Column number"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_insertRow, U"Insert row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_insertRow (me, GET_INTEGER (U"Row number"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_removeColumn, U"Remove column", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_removeColumn (me, GET_INTEGER (U"Column number"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_removeRow, U"Remove row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_removeRow (me, GET_INTEGER (U"Row number"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_setColumnLabel_index, U"Set column label", nullptr) {
	NATURAL (U"Column number", U"1")
	SENTENCE (U"Label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setColumnLabel (me, GET_INTEGER (U"Column number"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_setColumnLabel_label, U"Set column label", nullptr) {
	SENTENCE (U"Old label", U"")
	SENTENCE (U"New label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (U"Old label"));
		TableOfReal_setColumnLabel (me, columnNumber, GET_STRING (U"New label"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_setRowLabel_index, U"Set row label", nullptr) {
	NATURAL (U"Row number", U"1")
	SENTENCE (U"Label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setRowLabel (me, GET_INTEGER (U"Row number"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_setValue, U"Set value", U"TableOfReal: Set value...") {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	REAL_OR_UNDEFINED (U"New value", U"0.0")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (U"Row number"), columnNumber = GET_INTEGER (U"Column number");
		if (rowNumber > my numberOfRows) Melder_throw (me, U": row number too large.");
		if (columnNumber > my numberOfColumns) Melder_throw (me, U": column number too large.");
		my data [rowNumber] [columnNumber] = GET_REAL (U"New value");
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_setRowLabel_label, U"Set row label", nullptr) {
	SENTENCE (U"Old label", U"")
	SENTENCE (U"New label", U"")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = TableOfReal_rowLabelToIndex (me, GET_STRING (U"Old label"));
		TableOfReal_setRowLabel (me, rowNumber, GET_STRING (U"New label"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_sortByColumn, U"Sort rows by column", nullptr) {
	INTEGER (U"Column", U"1")
	INTEGER (U"Secondary column", U"0")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_sortByColumn (me, GET_INTEGER (U"Column"), GET_INTEGER (U"Secondary column"));
		praat_dataChanged (me);
	}
END2 }

FORM (TableOfReal_sortByLabel, U"Sort rows by label", nullptr) {
	LABEL (U"", U"Secondary sorting keys:")
	INTEGER (U"Column1", U"1")
	INTEGER (U"Column2", U"0")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_sortByLabel (me, GET_INTEGER (U"Column1"), GET_INTEGER (U"Column2"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (TableOfReal_to_Matrix) {
	LOOP {
		iam (TableOfReal);
		autoMatrix thee = TableOfReal_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TableOfReal_to_Table, U"TableOfReal: To Table", nullptr) {
	SENTENCE (U"Label of first column", U"rowLabel")
	OK2
DO
	LOOP {
		iam (TableOfReal);
		autoTable thee = TableOfReal_to_Table (me, GET_STRING (U"Label of first column"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM_SAVE (SAVE_TableOfReal_writeToHeaderlessSpreadsheetFile, U"Save TableOfReal as spreadsheet", 0, U"txt") {
	LOOP {
		iam (TableOfReal);
		TableOfReal_writeToHeaderlessSpreadsheetFile (me, file);
	}
END2 }

void praat_TableOfReal_init (ClassInfo klas) {
	if (klas == classTableOfReal) {
		praat_addAction1 (classTableOfReal, 0, U"TableOfReal help", nullptr, 0, DO_TableOfReal_help);
	}
	praat_addAction1 (klas, 1, U"Save as headerless spreadsheet file...", nullptr, 0, SAVE_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 1,   U"Write to headerless spreadsheet file...", U"*Save as headerless spreadsheet file...", praat_DEPRECATED_2011, SAVE_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Draw as numbers...", nullptr, 1, DO_TableOfReal_drawAsNumbers);
		praat_addAction1 (klas, 0, U"Draw as numbers if...", nullptr, 1, DO_TableOfReal_drawAsNumbers_if);
		praat_addAction1 (klas, 0, U"Draw as squares...", nullptr, 1, DO_TableOfReal_drawAsSquares);
		praat_addAction1 (klas, 0, U"-- draw lines --", nullptr, 1, 0);
		praat_addAction1 (klas, 0, U"Draw vertical lines...", nullptr, 1, DO_TableOfReal_drawVerticalLines);
		praat_addAction1 (klas, 0, U"Draw horizontal lines...", nullptr, 1, DO_TableOfReal_drawHorizontalLines);
		praat_addAction1 (klas, 0, U"Draw left and right lines...", nullptr, 1, DO_TableOfReal_drawLeftAndRightLines);
		praat_addAction1 (klas, 0, U"Draw top and bottom lines...", nullptr, 1, DO_TableOfReal_drawTopAndBottomLines);
	praat_addAction1 (klas, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 1, U"Get number of rows", nullptr, 1, DO_TableOfReal_getNumberOfRows);
		praat_addAction1 (klas, 1, U"Get number of columns", nullptr, 1, DO_TableOfReal_getNumberOfColumns);
		praat_addAction1 (klas, 1, U"Get row label...", nullptr, 1, DO_TableOfReal_getRowLabel);
		praat_addAction1 (klas, 1, U"Get column label...", nullptr, 1, DO_TableOfReal_getColumnLabel);
		praat_addAction1 (klas, 1, U"Get row index...", nullptr, 1, DO_TableOfReal_getRowIndex);
		praat_addAction1 (klas, 1, U"Get column index...", nullptr, 1, DO_TableOfReal_getColumnIndex);
		praat_addAction1 (klas, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (klas, 1, U"Get value...", nullptr, 1, DO_TableOfReal_getValue);
		if (klas == classTableOfReal) {
			praat_addAction1 (klas, 1, U"-- get statistics --", nullptr, 1, nullptr);
			praat_addAction1 (klas, 1, U"Get column mean (index)...", nullptr, 1, DO_TableOfReal_getColumnMean_index);
			praat_addAction1 (klas, 1, U"Get column mean (label)...", nullptr, 1, DO_TableOfReal_getColumnMean_label);
			praat_addAction1 (klas, 1, U"Get column stdev (index)...", nullptr, 1, DO_TableOfReal_getColumnStdev_index);
			praat_addAction1 (klas, 1, U"Get column stdev (label)...", nullptr, 1, DO_TableOfReal_getColumnStdev_label);
		}
	praat_addAction1 (klas, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Formula...", nullptr, 1, DO_TableOfReal_formula);
		praat_addAction1 (klas, 0, U"Set value...", nullptr, 1, DO_TableOfReal_setValue);
		praat_addAction1 (klas, 0, U"Sort by label...", nullptr, 1, DO_TableOfReal_sortByLabel);
		praat_addAction1 (klas, 0, U"Sort by column...", nullptr, 1, DO_TableOfReal_sortByColumn);
		praat_addAction1 (klas, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (klas, 0, U"Remove row (index)...", nullptr, 1, DO_TableOfReal_removeRow);
		praat_addAction1 (klas, 0, U"Remove column (index)...", nullptr, 1, DO_TableOfReal_removeColumn);
		praat_addAction1 (klas, 0, U"Insert row (index)...", nullptr, 1, DO_TableOfReal_insertRow);
		praat_addAction1 (klas, 0, U"Insert column (index)...", nullptr, 1, DO_TableOfReal_insertColumn);
		praat_addAction1 (klas, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (klas, 0, U"Set row label (index)...", nullptr, 1, DO_TableOfReal_setRowLabel_index);
		praat_addAction1 (klas, 0, U"Set row label (label)...", nullptr, 1, DO_TableOfReal_setRowLabel_label);
		praat_addAction1 (klas, 0, U"Set column label (index)...", nullptr, 1, DO_TableOfReal_setColumnLabel_index);
		praat_addAction1 (klas, 0, U"Set column label (label)...", nullptr, 1, DO_TableOfReal_setColumnLabel_label);
	praat_addAction1 (klas, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Append", nullptr, 1, DO_TablesOfReal_append);
	praat_addAction1 (klas, 0, U"Extract part -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Extract row ranges...", nullptr, 1, DO_TableOfReal_extractRowRanges);
		praat_addAction1 (klas, 0, U"Extract rows where column...", nullptr, 1, DO_TableOfReal_extractRowsWhereColumn);
		praat_addAction1 (klas, 0, U"Extract rows where label...", nullptr, 1, DO_TableOfReal_extractRowsWhereLabel);
		praat_addAction1 (klas, 0, U"Extract rows where...", nullptr, 1, DO_TableOfReal_extractRowsWhere);
		praat_addAction1 (klas, 0, U"Extract column ranges...", nullptr, 1, DO_TableOfReal_extractColumnRanges);
		praat_addAction1 (klas, 0, U"Extract columns where row...", nullptr, 1, DO_TableOfReal_extractColumnsWhereRow);
		praat_addAction1 (klas, 0, U"Extract columns where label...", nullptr, 1, DO_TableOfReal_extractColumnsWhereLabel);
		praat_addAction1 (klas, 0, U"Extract columns where...", nullptr, 1, DO_TableOfReal_extractColumnsWhere);
	praat_addAction1 (klas, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"Extract row labels as Strings", nullptr, 1, DO_TableOfReal_extractRowLabelsAsStrings);
		praat_addAction1 (klas, 0, U"Extract column labels as Strings", nullptr, 1, DO_TableOfReal_extractColumnLabelsAsStrings);
	praat_addAction1 (klas, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (klas, 0, U"To Table...", nullptr, 1, DO_TableOfReal_to_Table);
		praat_addAction1 (klas, 0, U"To Matrix", nullptr, 1, DO_TableOfReal_to_Matrix);
}

/* End of file praat_TableOfReal.cpp */
