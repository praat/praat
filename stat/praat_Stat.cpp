/* praat_Stat.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
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

#include "praat.h"

#include "Distributions_and_Strings.h"
#include "LogisticRegression.h"
#include "Matrix.h"
#include "PairDistribution.h"
#include "Table.h"
#include "TableEditor.h"
#include "UnicodeData.h"

#undef iam
#define iam iam_LOOP

static const char32 * Table_messageColumn (Table me, long column) {
	if (my columnHeaders [column]. label && my columnHeaders [column]. label [0] != U'\0')
		return Melder_cat (U"\"", my columnHeaders [column]. label, U"\"");
	else
		return Melder_integer (column);
}

/***** DISTRIBUTIONS *****/
#pragma mark -
#pragma mark DISTRIBUTIONS

DIRECT2 (Distributionses_add) {
	autoCollection me = praat_getSelectedObjects ();
	autoDistributions thee = Distributions_addMany (me.peek());
	praat_new (thee.move(), U"added");
END2 }

FORM (Distributionses_getMeanAbsoluteDifference, U"Get mean difference", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	Distributions me = nullptr, thee = nullptr;
	LOOP {
		(me ? thee : me) = (Distributions) OBJECT;
	}
	Melder_informationReal (Distributionses_getMeanAbsoluteDifference (me, thee, GET_INTEGER (U"Column number")), nullptr);
END2 }

FORM (Distributions_getProbability, U"Get probability", nullptr) {
	NATURAL (U"Column number", U"1")
	SENTENCE (U"String", U"")
	OK2
DO
	LOOP {
		iam (Distributions);
		double probability = Distributions_getProbability (me, GET_STRING (U"String"), GET_INTEGER (U"Column number"));
		Melder_informationReal (probability, nullptr);
	}
END2 }

DIRECT2 (Distributions_help) {
	Melder_help (U"Distributions");
END2 }

FORM (Distributions_to_Strings, U"To Strings", nullptr) {
	NATURAL (U"Column number", U"1")
	NATURAL (U"Number of strings", U"1000")
	OK2
DO
	LOOP {
		iam (Distributions);
		autoStrings thee = Distributions_to_Strings (me, GET_INTEGER (U"Column number"), GET_INTEGER (U"Number of strings"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Distributions_to_Strings_exact, U"To Strings (exact)", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (Distributions);
		autoStrings thee = Distributions_to_Strings_exact (me, GET_INTEGER (U"Column number"));
		praat_new (thee.move(), my name);
	}
END2 }

/***** LOGISTICREGRESSION *****/
#pragma mark -
#pragma mark LOGISTICREGRESSION

FORM (LogisticRegression_drawBoundary, U"LogisticRegression: Draw boundary", nullptr) {
	WORD (U"Horizontal factor", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical factor", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (LogisticRegression);
		long xfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (U"Horizontal factor"));
		long yfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (U"Vertical factor"));
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			yfactor, GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_INTEGER (U"Garnish"));
	}
END2 }

/***** PAIRDISTRIBUTION *****/
#pragma mark -
#pragma mark PAIRDISTRIBUTION

DIRECT2 (PairDistribution_getFractionCorrect_maximumLikelihood) {
	LOOP {
		iam (PairDistribution);
		double fractionCorrect = PairDistribution_getFractionCorrect_maximumLikelihood (me);
		Melder_informationReal (fractionCorrect, nullptr);
	}
END2 }

DIRECT2 (PairDistribution_getFractionCorrect_probabilityMatching) {
	LOOP {
		iam (PairDistribution);
		double fractionCorrect = PairDistribution_getFractionCorrect_probabilityMatching (me);
		Melder_informationReal (fractionCorrect, nullptr);
	}
END2 }

DIRECT2 (PairDistribution_getNumberOfPairs) {
	LOOP {
		iam (PairDistribution);
		Melder_information (my pairs.size);
	}
END2 }

FORM (PairDistribution_getString1, U"Get string1", nullptr) {
	NATURAL (U"Pair number", U"1")
	OK2
DO
	LOOP {
		iam (PairDistribution);
		const char32 *string1 = PairDistribution_getString1 (me, GET_INTEGER (U"Pair number"));
		Melder_information (string1);
	}
END2 }

FORM (PairDistribution_getString2, U"Get string2", nullptr) {
	NATURAL (U"Pair number", U"1")
	OK2
DO
	LOOP {
		iam (PairDistribution);
		const char32 *string2 = PairDistribution_getString2 (me, GET_INTEGER (U"Pair number"));
		Melder_information (string2);
	}
END2 }

FORM (PairDistribution_getWeight, U"Get weight", nullptr) {
	NATURAL (U"Pair number", U"1")
	OK2
DO
	LOOP {
		iam (PairDistribution);
		double weight = PairDistribution_getWeight (me, GET_INTEGER (U"Pair number"));
		Melder_information (weight);
	}
END2 }

DIRECT2 (PairDistribution_help) {
	Melder_help (U"PairDistribution");
END2 }

DIRECT2 (PairDistribution_removeZeroWeights) {
	LOOP {
		iam (PairDistribution);
		PairDistribution_removeZeroWeights (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (PairDistribution_swapInputsAndOutputs) {
	LOOP {
		iam (PairDistribution);
		PairDistribution_swapInputsAndOutputs (me);
		praat_dataChanged (me);
	}
END2 }

FORM (PairDistribution_to_Stringses, U"Generate two Strings objects", nullptr) {
	NATURAL (U"Number", U"1000")
	SENTENCE (U"Name of first Strings", U"input")
	SENTENCE (U"Name of second Strings", U"output")
	OK2
DO
	LOOP {
		iam (PairDistribution);
		autoStrings strings1, strings2;
		PairDistribution_to_Stringses (me, GET_INTEGER (U"Number"), & strings1, & strings2);
		praat_new (strings1.move(), GET_STRING (U"Name of first Strings"));
		praat_new (strings2.move(), GET_STRING (U"Name of second Strings"));
	}
END2 }

DIRECT2 (PairDistribution_to_Table) {
	LOOP {
		iam (PairDistribution);
		autoTable thee = PairDistribution_to_Table (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** PAIRDISTRIBUTION & DISTRIBUTIONS *****/
#pragma mark -
#pragma mark PAIRDISTRIBUTION & DISTRIBUTIONS

FORM (PairDistribution_Distributions_getFractionCorrect, U"PairDistribution & Distributions: Get fraction correct", nullptr) {
	NATURAL (U"Column", U"1")
	OK2
DO
	PairDistribution me = nullptr;
	Distributions thee = nullptr;
	LOOP {
		if (CLASS == classPairDistribution) me = (PairDistribution) OBJECT;
		if (CLASS == classDistributions) thee = (Distributions) OBJECT;
	}
	double fractionCorrect = PairDistribution_Distributions_getFractionCorrect (me, thee, GET_INTEGER (U"Column"));
	Melder_informationReal (fractionCorrect, nullptr);
END2 }

/***** TABLE *****/
#pragma mark -
#pragma mark TABLE

DIRECT2 (Tables_append) {
	autoCollection collection = Collection_create (10);
	LOOP {
		iam (Table);
		Collection_addItem_ref (collection.peek(), me);
	}
	autoTable thee = Tables_append (collection.peek());
	praat_new (thee.move(), U"appended");
END2 }

FORM (Table_appendColumn, U"Table: Append column", nullptr) {
	WORD (U"Label", U"newcolumn")
	OK2
DO
	LOOP {
		iam (Table);
		Table_appendColumn (me, GET_STRING (U"Label"));
		praat_dataChanged (OBJECT);
	}
END2 }

FORM (Table_appendDifferenceColumn, U"Table: Append difference column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendDifferenceColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_appendProductColumn, U"Table: Append product column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendProductColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_appendQuotientColumn, U"Table: Append quotient column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendQuotientColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_appendSumColumn, U"Table: Append sum column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendSumColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Table_appendRow) {
	LOOP {
		iam (Table);
		Table_appendRow (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Table_collapseRows, U"Table: Collapse rows", nullptr) {
	LABEL (U"", U"Columns with factors (independent variables):")
	TEXTFIELD (U"factors", U"speaker dialect age vowel")
	LABEL (U"", U"Columns to sum:")
	TEXTFIELD (U"columnsToSum", U"number cost")
	LABEL (U"", U"Columns to average:")
	TEXTFIELD (U"columnsToAverage", U"price")
	LABEL (U"", U"Columns to medianize:")
	TEXTFIELD (U"columnsToMedianize", U"vot")
	LABEL (U"", U"Columns to average logarithmically:")
	TEXTFIELD (U"columnsToAverageLogarithmically", U"duration")
	LABEL (U"", U"Columns to medianize logarithmically:")
	TEXTFIELD (U"columnsToMedianizeLogarithmically", U"F0 F1 F2 F3")
	LABEL (U"", U"Columns not mentioned above will be ignored.")
	OK2
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_collapseRows (me,
			GET_STRING (U"factors"), GET_STRING (U"columnsToSum"),
			GET_STRING (U"columnsToAverage"), GET_STRING (U"columnsToMedianize"),
			GET_STRING (U"columnsToAverageLogarithmically"), GET_STRING (U"columnsToMedianizeLogarithmically"));
		praat_new (thee.move(), my name, U"_pooled");
	}
END2 }

FORM (Table_createWithColumnNames, U"Create Table with column names", nullptr) {
	WORD (U"Name", U"table")
	INTEGER (U"Number of rows", U"10")
	LABEL (U"", U"Column names:")
	TEXTFIELD (U"columnNames", U"speaker dialect age vowel F0 F1 F2")
	OK2
DO
	autoTable me = Table_createWithColumnNames (GET_INTEGER (U"Number of rows"), GET_STRING (U"columnNames"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Table_createWithoutColumnNames, U"Create Table without column names", nullptr) {
	WORD (U"Name", U"table")
	INTEGER (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"3")
	OK2
DO
	autoTable me = Table_createWithoutColumnNames (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Table_drawEllipse, U"Draw ellipse (standard deviation)", nullptr) {
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (U"Number of sigmas", U"2.0")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		Table_drawEllipse_e (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Number of sigmas"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Table_drawRowFromDistribution, U"Table: Draw row from distribution", nullptr) {
	WORD (U"Column with distribution", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column with distribution"));
		long row = Table_drawRowFromDistribution (me, icol);
		Melder_information (row);
	}
END2 }

DIRECT2 (Table_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a Table from batch.");
	LOOP {
		iam (Table);
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (Table_extractRowsWhereColumn_number, U"Table: Extract rows where column (number)", nullptr) {
	WORD (U"Extract all rows where column...", U"")
	RADIO_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the number", U"0.0")
	OK2
DO
	double value = GET_REAL (U"...the number");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_number (me, icol, GET_ENUM (kMelder_number, U"...is..."), value);
		praat_new (thee.move(), my name, U"_", Table_messageColumn (static_cast <Table> OBJECT, icol), U"_", NUMdefined (value) ? Melder_integer (lround (value)) : U"undefined");
		praat_dataChanged (me);   // WHY?
	}
END2 }

FORM (Table_extractRowsWhereColumn_text, U"Table: Extract rows where column (text)", nullptr) {
	WORD (U"Extract all rows where column...", U"")
	OPTIONMENU_ENUM (U"...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *value = GET_STRING (U"...the text");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_string (me, icol, GET_ENUM (kMelder_string, U"..."), value);
		praat_new (thee.move(), my name, U"_", value);
		praat_dataChanged (me);   // WHY?
	}
END2 }

FORM (Table_formula, U"Table: Formula", U"Table: Formula...") {
	WORD (U"Column label", U"")
	TEXTFIELD (U"formula", U"abs (self)")
	OK2
DO
	LOOP {
		iam (Table);
		try {
			long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
			Table_formula (me, icol, GET_STRING (U"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Table may have partially changed
			throw;
		}
	}
END2 }

FORM (Table_formula_columnRange, U"Table: Formula (column range)", U"Table: Formula...") {
	WORD (U"From column label", U"")
	WORD (U"To column label", U"")
	TEXTFIELD (U"formula", U"log10 (self)")
	OK2
DO
	LOOP {
		iam (Table);
		try {
			long icol1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"From column label"));
			long icol2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"To column label"));
			Table_formula_columnRange (me, icol1, icol2, GET_STRING (U"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Table may have partially changed
			throw;
		}
	}
END2 }

FORM (Table_getColumnIndex, U"Table: Get column index", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		Melder_information (Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Column label")));
	}
END2 }

FORM (Table_getColumnLabel, U"Table: Get column label", nullptr) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = GET_INTEGER (U"Column number");
		if (icol > my numberOfColumns) Melder_throw (U"Column number must not be greater than number of columns.");
		Melder_information (my columnHeaders [icol]. label);
	}
END2 }

FORM (Table_getGroupMean, U"Table: Get group mean", nullptr) {
	WORD (U"Column label", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group", U"F")
	OK2
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Group column"));
		Melder_information (Table_getGroupMean (static_cast <Table> ONLY_OBJECT, column, groupColumn, GET_STRING (U"Group")));
	}
END2 }

FORM (Table_getMaximum, U"Table: Get maximum", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double maximum = Table_getMaximum (me, icol);
		Melder_information (maximum);
	}
END2 }

FORM (Table_getMean, U"Table: Get mean", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double mean = Table_getMean (me, icol);
		Melder_information (mean);
	}
END2 }

FORM (Table_getMinimum, U"Table: Get minimum", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double minimum = Table_getMinimum (me, icol);
		Melder_information (minimum);
	}
END2 }

FORM (Table_getQuantile, U"Table: Get quantile", nullptr) {
	SENTENCE (U"Column label", U"")
	POSITIVE (U"Quantile", U"0.50 (= median)")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double quantile = Table_getQuantile (me, icol, GET_REAL (U"Quantile"));
		Melder_information (quantile);
	}
END2 }

FORM (Table_getStandardDeviation, U"Table: Get standard deviation", nullptr) {
	SENTENCE (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		double stdev = Table_getStdev (me, icol);
		Melder_information (stdev);
	}
END2 }

DIRECT2 (Table_getNumberOfColumns) {
	LOOP {
		iam (Table);
		Melder_information (my numberOfColumns);
	}
END2 }

DIRECT2 (Table_getNumberOfRows) {
	LOOP {
		iam (Table);
		Melder_information (my rows -> size);
	}
END2 }

FORM (Table_getValue, U"Table: Get value", nullptr) {
	NATURAL (U"Row number", U"1")
	WORD (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long rowNumber = GET_INTEGER (U"Row number");
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Melder_information (((TableRow) my rows -> item [rowNumber]) -> cells [icol]. string);
	}
END2 }

DIRECT2 (Table_help) {
	Melder_help (U"Table");
END2 }

FORM (Table_insertColumn, U"Table: Insert column", nullptr) {
	NATURAL (U"Position", U"1")
	WORD (U"Label", U"newcolumn")
	OK2
DO
	LOOP {
		iam (Table);
		Table_insertColumn (me, GET_INTEGER (U"Position"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_insertRow, U"Table: Insert row", nullptr) {
	NATURAL (U"Position", U"1")
	OK2
DO
	LOOP {
		iam (Table);
		Table_insertRow (me, GET_INTEGER (U"Position"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_list, U"Table: List", nullptr) {
	BOOLEAN (U"Include row numbers", true)
	OK2
DO
	LOOP {
		iam (Table);
		Table_list (me, GET_INTEGER (U"Include row numbers"));
	}
END2 }

FORM_READ2 (Table_readFromTableFile, U"Read Table from table file", nullptr, true) {
	autoTable me = Table_readFromTableFile (file);
	praat_newWithFile (me.move(), file, MelderFile_name (file));
END2 }

FORM_READ2 (Table_readFromCommaSeparatedFile, U"Read Table from comma-separated file", nullptr, true) {
	autoTable me = Table_readFromCharacterSeparatedTextFile (file, U',');
	praat_newWithFile (me.move(), file, MelderFile_name (file));
END2 }

FORM_READ2 (Table_readFromTabSeparatedFile, U"Read Table from tab-separated file", nullptr, true) {
	autoTable me = Table_readFromCharacterSeparatedTextFile (file, U'\t');
	praat_newWithFile (me.move(), file, MelderFile_name (file));
END2 }

FORM (Table_removeColumn, U"Table: Remove column", nullptr) {
	WORD (U"Column label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_removeColumn (me, icol);
		praat_dataChanged (me);
	}
END2 }

FORM (Table_removeRow, U"Table: Remove row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	LOOP {
		iam (Table);
		Table_removeRow (me, GET_INTEGER (U"Row number"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_reportCorrelation_kendallTau, U"Report correlation (Kendall tau)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_kendallTau (me, column1, column2, unconfidence,
			& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Correlation between column ", Table_messageColumn (me, column1),
			U" and column ", Table_messageColumn (me, column2), U":");
		MelderInfo_writeLine (U"Correlation = ", correlation, U" (Kendall's tau-b)");
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }

FORM (Table_reportCorrelation_pearsonR, U"Report correlation (Pearson r)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_pearsonR (me, column1, column2, unconfidence,
			& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Correlation between column ", Table_messageColumn (me, column1),
			U" and column ", Table_messageColumn (me, column2), U":");
		MelderInfo_writeLine (U"Correlation = ", correlation, U" (Pearson's r)");
		MelderInfo_writeLine (U"Number of degrees of freedom = ", my rows -> size - 2);
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }
	
FORM (Table_reportDifference_studentT, U"Report difference (Student t)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		double difference, t, numberOfDegreesOfFreedom, significance, lowerLimit, upperLimit;
		difference = Table_getDifference_studentT (me, column1, column2, unconfidence,
			& t, & numberOfDegreesOfFreedom, & significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Difference between column ", Table_messageColumn (me, column1),
			U" and column ", Table_messageColumn (me, column2), U":");
		MelderInfo_writeLine (U"Difference = ", difference);
		MelderInfo_writeLine (U"Student's t = ", t);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }
	
FORM (Table_reportGroupDifference_studentT, U"Report group difference (Student t)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group 1", U"F")
	SENTENCE (U"Group 2", U"M")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Group column"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		const char32 *group1 = GET_STRING (U"Group 1"), *group2 = GET_STRING (U"Group 2");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupDifference_studentT (me, column, groupColumn, group1, group2, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (U"Difference in column ", Table_messageColumn (me, column), U" between groups ", group1);
		MelderInfo_writeLine (U" and ", group2, U" of column ", Table_messageColumn (me, groupColumn), U":");
		MelderInfo_writeLine (U"Difference = ", mean);
		MelderInfo_writeLine (U"Student's t = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }

FORM (Table_reportGroupDifference_wilcoxonRankSum, U"Report group difference (Wilcoxon rank sum)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group 1", U"F")
	SENTENCE (U"Group 2", U"M")
	OK2
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Group column"));
		const char32 *group1 = GET_STRING (U"Group 1"), *group2 = GET_STRING (U"Group 2");
		double areaUnderCurve, rankSum, significanceFromZero;
		areaUnderCurve = Table_getGroupDifference_wilcoxonRankSum (me, column, groupColumn, group1, group2,
			& rankSum, & significanceFromZero);
		MelderInfo_open ();
		MelderInfo_write (U"Difference in column ", Table_messageColumn (me, column), U" between groups ", group1);
		MelderInfo_writeLine (U" and ", group2, U" of column ", Table_messageColumn (me, groupColumn), U":");
		MelderInfo_writeLine (U"Larger: ", areaUnderCurve < 0.5 ? group1 : areaUnderCurve > 0.5 ? group2 : U"(both equal)");
		MelderInfo_writeLine (U"Area under curve: ", areaUnderCurve);
		MelderInfo_writeLine (U"Rank sum: ", rankSum);
		MelderInfo_writeLine (U"Significance from zero: ", significanceFromZero, U" (one-tailed)");
		MelderInfo_close ();
	}
END2 }

FORM (Table_reportGroupMean_studentT, U"Report group mean (Student t)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group", U"F")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Group column"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		const char32 *group = GET_STRING (U"Group");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupMean_studentT (me, column, groupColumn, group, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (U"Mean in column ", Table_messageColumn (me, column), U" of group ", group);
		MelderInfo_writeLine (U" of column ", Table_messageColumn (me, groupColumn), U":");
		MelderInfo_writeLine (U"Mean = ", mean);
		MelderInfo_writeLine (U"Student's t from zero = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }

FORM (Table_reportMean_studentT, U"Report mean (Student t)", nullptr) {
	WORD (U"Column", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK2
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column"));
		double unconfidence = GET_REAL (U"One-tailed unconfidence");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getMean_studentT (me, column, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Mean of column ", Table_messageColumn (me, column), U":");
		MelderInfo_writeLine (U"Mean = ", mean);
		MelderInfo_writeLine (U"Student's t from zero = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	}
END2 }

FORM (Table_rowsToColumns, U"Table: Rows to columns", nullptr) {
	LABEL (U"", U"Columns with factors (independent variables):")
	TEXTFIELD (U"factors", U"dialect gender speaker")
	WORD (U"Column to transpose", U"vowel")
	LABEL (U"", U"Columns to expand:")
	TEXTFIELD (U"columnsToExpand", U"duration F0 F1 F2 F3")
	LABEL (U"", U"Columns not mentioned above will be ignored.")
	OK2
DO
	const char32 *columnLabel = GET_STRING (U"Column to transpose");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoTable thee = Table_rowsToColumns (me, GET_STRING (U"factors"), icol, GET_STRING (U"columnsToExpand"));
		praat_new (thee.move(), NAME, U"_nested");
	}
END2 }

FORM (Table_scatterPlot, U"Scatter plot", nullptr) {
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	WORD (U"Column with marks", U"")
	NATURAL (U"Font size", U"12")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		long markColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column with marks"));
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			markColumn, GET_INTEGER (U"Font size"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Table_scatterPlot_mark, U"Scatter plot (marks)", nullptr) {
	WORD (U"Horizontal column", U"")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0 (= auto)")
	WORD (U"Vertical column", U"")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (U"Mark size (mm)", U"1.0")
	BOOLEAN (U"Garnish", true)
	SENTENCE (U"Mark string (+xo.)", U"+")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Vertical column"));
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (U"left Horizontal range"), GET_REAL (U"right Horizontal range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"),
			GET_REAL (U"Mark size"), GET_STRING (U"Mark string"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (Table_searchColumn, U"Table: Search column", nullptr) {
	WORD (U"Column label", U"")
	WORD (U"Value", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Melder_information (Table_searchColumn (me, icol, GET_STRING (U"Value")));
	}
END2 }
	
FORM (Table_setColumnLabel_index, U"Set column label", nullptr) {
	NATURAL (U"Column number", U"1")
	SENTENCE (U"Label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, GET_INTEGER (U"Column number"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_setColumnLabel_label, U"Set column label", nullptr) {
	SENTENCE (U"Old label", U"")
	SENTENCE (U"New label", U"")
	OK2
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Old label")), GET_STRING (U"New label"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_setNumericValue, U"Table: Set numeric value", nullptr) {
	NATURAL (U"Row number", U"1")
	WORD (U"Column label", U"")
	REAL_OR_UNDEFINED (U"Numeric value", U"1.5")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_setNumericValue (me, GET_INTEGER (U"Row number"), icol, GET_REAL (U"Numeric value"));
		praat_dataChanged (me);
	}
END2 }

FORM (Table_setStringValue, U"Table: Set string value", nullptr) {
	NATURAL (U"Row number", U"1")
	WORD (U"Column label", U"")
	SENTENCE (U"String value", U"xx")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_setStringValue (me, GET_INTEGER (U"Row number"), icol, GET_STRING (U"String value"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Table_randomizeRows) {
	LOOP {
		iam (Table);
		Table_randomizeRows (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Table_reflectRows) {
	LOOP {
		iam (Table);
		Table_reflectRows (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Table_sortRows, U"Table: Sort rows", nullptr) {
	LABEL (U"", U"One or more column labels for sorting:")
	TEXTFIELD (U"columnLabels", U"dialect gender name")
	OK2
DO
	LOOP {
		iam (Table);
		Table_sortRows_string (me, GET_STRING (U"columnLabels"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Table_to_LinearRegression) {
	LOOP {
		iam (Table);
		autoLinearRegression thee = Table_to_LinearRegression (me);
		praat_new (thee.move(), NAME);
	}
END2 }

FORM (Table_to_LogisticRegression, U"Table: To LogisticRegression", nullptr) {
	LABEL (U"", U"Factors (column names):")
	TEXTFIELD (U"factors", U"F0 F1 duration")
	WORD (U"Dependent 1 (column name)", U"e")
	WORD (U"Dependent 2 (column name)", U"i")
	OK2
DO
	LOOP {
		iam (Table);
		autoLogisticRegression thee = Table_to_LogisticRegression (me, GET_STRING (U"factors"), GET_STRING (U"Dependent 1"), GET_STRING (U"Dependent 2"));
		praat_new (thee.move(), NAME);
	}
END2 }

FORM (Table_to_TableOfReal, U"Table: Down to TableOfReal", nullptr) {
	WORD (U"Column for row labels", U"")
	OK2
DO
	LOOP {
		iam (Table);
		long icol = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Column for row labels"));
		autoTableOfReal thee = Table_to_TableOfReal (me, icol);
		praat_new (thee.move(), NAME);
	}
END2 }

DIRECT2 (Table_transpose) {
	LOOP {
		iam (Table);
		autoTable thee = Table_transpose (me);
		praat_new (thee.move(), NAME, U"_transposed");
	}
END2 }

FORM_WRITE2 (Table_writeToCommaSeparatedFile, U"Save Table as comma-separated file", 0, U"Table") {
	LOOP {
		iam (Table);
		Table_writeToCommaSeparatedFile (me, file);
	}
END2 }

FORM_WRITE2 (Table_writeToTabSeparatedFile, U"Save Table as tab-separated file", 0, U"Table") {
	LOOP {
		iam (Table);
		Table_writeToTabSeparatedFile (me, file);
	}
END2 }

/***** TABLEOFREAL *****/
#pragma mark -
#pragma mark TABLEOFREAL

DIRECT2 (TablesOfReal_append) {
	autoCollection tables = Collection_create (10);
	LOOP {
		iam (TableOfReal);
		Collection_addItem_ref (tables.peek(), me);
	}
	autoTableOfReal thee = TablesOfReal_appendMany (tables.peek());
	praat_new (thee.move(), U"appended");
END2 }

FORM (TableOfReal_create, U"Create TableOfReal", nullptr) {
	WORD (U"Name", U"table")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"3")
	OK2
DO
	autoTableOfReal me = TableOfReal_create (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	praat_new (me.move(), GET_STRING (U"Name"));
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

FORM_READ2 (TableOfReal_readFromHeaderlessSpreadsheetFile, U"Read TableOfReal from headerless spreadsheet file", nullptr, true) {
	autoTableOfReal me = TableOfReal_readFromHeaderlessSpreadsheetFile (file);
	praat_newWithFile (me.move(), file, MelderFile_name (file));
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

FORM_WRITE2 (TableOfReal_writeToHeaderlessSpreadsheetFile, U"Save TableOfReal as spreadsheet", 0, U"txt") {
	LOOP {
		iam (TableOfReal);
		TableOfReal_writeToHeaderlessSpreadsheetFile (me, file);
	}
END2 }


DIRECT2 (StatisticsTutorial) {
	Melder_help (U"Statistics");
END2 }

static bool isTabSeparated_8bit (int nread, const char *header) {
	for (long i = 0; i < nread; i ++) {
		if (header [i] == '\t') return true;
		if (header [i] == '\n' || header [i] == '\r') return false;
	}
	return false;
}

static bool isTabSeparated_utf16be (int nread, const char *header) {
	for (long i = 2; i < nread; i += 2) {
		if (header [i] == '\0' && header [i + 1] == '\t') return true;
		if (header [i] == '\0' && (header [i + 1] == '\n' || header [i + 1] == '\r')) return false;
	}
	return false;
}

static bool isTabSeparated_utf16le (int nread, const char *header) {
	for (long i = 2; i < nread; i += 2) {
		if (header [i + 1] == '\0' && header [i] == '\t') return true;
		if (header [i + 1] == '\0' && (header [i] == '\n' || header [i] == '\r')) return false;
	}
	return false;
}

static autoDaata tabSeparatedFileRecognizer (int nread, const char *header, MelderFile file) {
	/*
	 * A table is recognized if it has at least one tab symbol,
	 * which must be before the first newline symbol (if any).
	 */
	unsigned char *uheader = (unsigned char *) header;
	bool isTabSeparated =
		uheader [0] == 0xef && uheader [1] == 0xff ? isTabSeparated_utf16be (nread, header) :
		uheader [0] == 0xff && uheader [1] == 0xef ? isTabSeparated_utf16le (nread, header) :
		isTabSeparated_8bit (nread, header);
	if (! isTabSeparated) return autoDaata ();
	return Table_readFromCharacterSeparatedTextFile (file, '\t');
}

void praat_TableOfReal_init (ClassInfo klas);   // buttons for TableOfReal and for its subclasses
void praat_TableOfReal_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Save as headerless spreadsheet file...", nullptr, 0, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 1, U"Write to headerless spreadsheet file...", nullptr, praat_HIDDEN, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
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

void praat_uvafon_stat_init ();
void praat_uvafon_stat_init () {

	Thing_recognizeClassesByName (classTableOfReal, classDistributions, classPairDistribution,
		classTable, classLinearRegression, classLogisticRegression, nullptr);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	praat_addMenuCommand (U"Objects", U"New", U"Tables", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table with column names...", nullptr, 1, DO_Table_createWithColumnNames);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table without column names...", nullptr, 1, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal...", nullptr, 1, DO_TableOfReal_create);

	praat_addMenuCommand (U"Objects", U"Open", U"Read TableOfReal from headerless spreadsheet file...", nullptr, 0, DO_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from tab-separated file...", nullptr, 0, DO_Table_readFromTabSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from comma-separated file...", nullptr, 0, DO_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from whitespace-separated file...", nullptr, 0, DO_Table_readFromTableFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from table file...", nullptr, praat_HIDDEN, DO_Table_readFromTableFile);

	praat_addAction1 (classDistributions, 0, U"Distributions help", nullptr, 0, DO_Distributions_help);
	praat_TableOfReal_init (classDistributions);
	praat_addAction1 (classDistributions, 1, U"Get probability (label)...", U"Get value...", 1, DO_Distributions_getProbability);
	praat_addAction1 (classDistributions, 0, U"-- get from two --", U"Get probability (label)...", 1, nullptr);
	praat_addAction1 (classDistributions, 2, U"Get mean absolute difference...", U"-- get from two --", 1, DO_Distributionses_getMeanAbsoluteDifference);
	praat_addAction1 (classDistributions, 0, U"-- add --", U"Append", 1, nullptr);
	praat_addAction1 (classDistributions, 0, U"Add", U"-- add --", 1, DO_Distributionses_add);
	praat_addAction1 (classDistributions, 0, U"Generate", nullptr, 0, nullptr);   // FIXME no hyphen?
		praat_addAction1 (classDistributions, 0, U"To Strings...", nullptr, 0, DO_Distributions_to_Strings);
		praat_addAction1 (classDistributions, 0, U"To Strings (exact)...", nullptr, 0, DO_Distributions_to_Strings_exact);

	praat_addAction1 (classLogisticRegression, 0, U"Draw boundary...", nullptr, 0, DO_LogisticRegression_drawBoundary);

	praat_addAction1 (classPairDistribution, 0, U"PairDistribution help", nullptr, 0, DO_PairDistribution_help);
	praat_addAction1 (classPairDistribution, 0, U"To Table", nullptr, 0, DO_PairDistribution_to_Table);
	praat_addAction1 (classPairDistribution, 1, U"To Stringses...", nullptr, 0, DO_PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get number of pairs", nullptr, 1, DO_PairDistribution_getNumberOfPairs);
		praat_addAction1 (classPairDistribution, 1, U"Get string1...", nullptr, 1, DO_PairDistribution_getString1);
		praat_addAction1 (classPairDistribution, 1, U"Get string2...", nullptr, 1, DO_PairDistribution_getString2);
		praat_addAction1 (classPairDistribution, 1, U"Get weight...", nullptr, 1, DO_PairDistribution_getWeight);
		praat_addAction1 (classPairDistribution, 1, U"-- get fraction correct --", nullptr, 1, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (maximum likelihood)", nullptr, 1, DO_PairDistribution_getFractionCorrect_maximumLikelihood);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (probability matching)", nullptr, 1, DO_PairDistribution_getFractionCorrect_probabilityMatching);
	praat_addAction1 (classPairDistribution, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPairDistribution, 0, U"Remove zero weights", nullptr, 1, DO_PairDistribution_removeZeroWeights);
	praat_addAction1 (classPairDistribution, 0, U"Swap inputs and outputs", nullptr, 1, DO_PairDistribution_swapInputsAndOutputs);

	praat_addAction1 (classTable, 0, U"Table help", nullptr, 0, DO_Table_help);
	praat_addAction1 (classTable, 1, U"Save as tab-separated file...", nullptr, 0, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as comma-separated file...", nullptr, 0, DO_Table_writeToCommaSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as table file...", nullptr, praat_HIDDEN, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"Write to table file...", nullptr, praat_HIDDEN, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_Table_edit);
	praat_addAction1 (classTable, 1, U"Edit", nullptr, praat_HIDDEN, DO_Table_edit);
	praat_addAction1 (classTable, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Scatter plot...", nullptr, 1, DO_Table_scatterPlot);
		praat_addAction1 (classTable, 0, U"Scatter plot (mark)...", nullptr, 1, DO_Table_scatterPlot_mark);
		praat_addAction1 (classTable, 0, U"Draw ellipse (standard deviation)...", nullptr, 1, DO_Table_drawEllipse);
	praat_addAction1 (classTable, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"List...", nullptr, 1, DO_Table_list);
		praat_addAction1 (classTable, 1, U"-- get structure --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get number of rows", nullptr, 1, DO_Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, U"Get number of columns", nullptr, 1, DO_Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, U"Get column label...", nullptr, 1, DO_Table_getColumnLabel);
		praat_addAction1 (classTable, 1, U"Get column index...", nullptr, 1, DO_Table_getColumnIndex);
		praat_addAction1 (classTable, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get value...", nullptr, 1, DO_Table_getValue);
		praat_addAction1 (classTable, 1, U"Search column...", nullptr, 1, DO_Table_searchColumn);
		praat_addAction1 (classTable, 1, U"-- statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Statistics tutorial", nullptr, 1, DO_StatisticsTutorial);
		praat_addAction1 (classTable, 1, U"-- get stats --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get quantile...", nullptr, 1, DO_Table_getQuantile);
		praat_addAction1 (classTable, 1, U"Get minimum...", nullptr, 1, DO_Table_getMinimum);
		praat_addAction1 (classTable, 1, U"Get maximum...", nullptr, 1, DO_Table_getMaximum);
		praat_addAction1 (classTable, 1, U"Get mean...", nullptr, 1, DO_Table_getMean);
		praat_addAction1 (classTable, 1, U"Get group mean...", nullptr, 1, DO_Table_getGroupMean);
		praat_addAction1 (classTable, 1, U"Get standard deviation...", nullptr, 1, DO_Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, U"-- report stats --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Report mean (Student t)...", nullptr, 1, DO_Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, U"Report standard deviation...", nullptr, 1, DO_Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, U"Report difference (Student t)...", nullptr, 1, DO_Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group mean (Student t)...", nullptr, 1, DO_Table_reportGroupMean_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Student t)...", nullptr, 1, DO_Table_reportGroupDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Wilcoxon rank sum)...", nullptr, 1, DO_Table_reportGroupDifference_wilcoxonRankSum);
		praat_addAction1 (classTable, 1, U"Report correlation (Pearson r)...", nullptr, 1, DO_Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, U"Report correlation (Kendall tau)...", nullptr, 1, DO_Table_reportCorrelation_kendallTau);
	praat_addAction1 (classTable, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Set string value...", nullptr, 1, DO_Table_setStringValue);
		praat_addAction1 (classTable, 0, U"Set numeric value...", nullptr, 1, DO_Table_setNumericValue);
		praat_addAction1 (classTable, 0, U"Formula...", nullptr, 1, DO_Table_formula);
		praat_addAction1 (classTable, 0, U"Formula (column range)...", nullptr, 1, DO_Table_formula_columnRange);
		praat_addAction1 (classTable, 0, U"Sort rows...", nullptr, 1, DO_Table_sortRows);
		praat_addAction1 (classTable, 0, U"Randomize rows", nullptr, 1, DO_Table_randomizeRows);
		praat_addAction1 (classTable, 0, U"Reflect rows", nullptr, 1, DO_Table_reflectRows);
		praat_addAction1 (classTable, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Append row", nullptr, 1, DO_Table_appendRow);
		praat_addAction1 (classTable, 0, U"Append column...", nullptr, 1, DO_Table_appendColumn);
		praat_addAction1 (classTable, 0, U"Append sum column...", nullptr, 1, DO_Table_appendSumColumn);
		praat_addAction1 (classTable, 0, U"Append difference column...", nullptr, 1, DO_Table_appendDifferenceColumn);
		praat_addAction1 (classTable, 0, U"Append product column...", nullptr, 1, DO_Table_appendProductColumn);
		praat_addAction1 (classTable, 0, U"Append quotient column...", nullptr, 1, DO_Table_appendQuotientColumn);
		praat_addAction1 (classTable, 0, U"Remove row...", nullptr, 1, DO_Table_removeRow);
		praat_addAction1 (classTable, 0, U"Remove column...", nullptr, 1, DO_Table_removeColumn);
		praat_addAction1 (classTable, 0, U"Insert row...", nullptr, 1, DO_Table_insertRow);
		praat_addAction1 (classTable, 0, U"Insert column...", nullptr, 1, DO_Table_insertColumn);
		praat_addAction1 (classTable, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Set column label (index)...", nullptr, 1, DO_Table_setColumnLabel_index);
		praat_addAction1 (classTable, 0, U"Set column label (label)...", nullptr, 1, DO_Table_setColumnLabel_label);
	praat_addAction1 (classTable, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"To linear regression", nullptr, 1, DO_Table_to_LinearRegression);
		praat_addAction1 (classTable, 0, U"To logistic regression...", nullptr, 1, DO_Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Append", nullptr, 1, DO_Tables_append);
	praat_addAction1 (classTable, 0, U"Generate -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"Draw row from distribution...", nullptr, 1, DO_Table_drawRowFromDistribution);
	praat_addAction1 (classTable, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Extract rows where column (number)...", nullptr, 1, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, U"Extract rows where column...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, U"Select rows where column...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, U"Extract rows where column (text)...", nullptr, 1, DO_Table_extractRowsWhereColumn_text);
		praat_addAction1 (classTable, 0, U"Transpose", nullptr, 1, DO_Table_transpose);
		praat_addAction1 (classTable, 0, U"Collapse rows...", nullptr, 1, DO_Table_collapseRows);
		praat_addAction1 (classTable, 0, U"Rows to columns...", nullptr, 1, DO_Table_rowsToColumns);
	praat_addAction1 (classTable, 0, U"Down to TableOfReal...", nullptr, 0, DO_Table_to_TableOfReal);

	praat_addAction1 (classTableOfReal, 0, U"TableOfReal help", nullptr, 0, DO_TableOfReal_help);
	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0, DO_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.cpp */
