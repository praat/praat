/* praat_Stat.cpp
 *
 * Copyright (C) 1992-2019,2021-2023 Paul Boersma
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

#include "Distributions_and_Strings.h"
#include "LogisticRegression.h"
#include "Matrix.h"
#include "PairDistribution.h"
#include "Table.h"
#include "TableEditor.h"
#include "RealTier.h"
#include "../kar/UnicodeData.h"

#include "praat_TableOfReal.h"

// MARK: - DISTRIBUTIONS

// MARK: Help

DIRECT (HELP__Distributions_help) {
	HELP (U"Distributions")
}

DIRECT (HELP__Table_help) {
	HELP (U"Table")
}

// MARK: Query

FORM (QUERY_ONE_FOR_REAL__Distributionses_getMeanAbsoluteDifference, U"Get mean difference", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_TWO_FOR_REAL (Distributions)
		const double result = Distributionses_getMeanAbsoluteDifference (me, you, columnNumber);
	QUERY_TWO_FOR_REAL_END (U" (mean absolute difference between columns ", columnNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__Distributions_getProbability, U"Get probability", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	SENTENCE (string, U"String", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Distributions)
		const double result = Distributions_getProbability (me, string, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (probability)")
}

// MARK: Modify

DIRECT (COMBINE_ALL_TO_ONE__Distributionses_add) {
	COMBINE_ALL_TO_ONE (Distributions)
		autoDistributions result = Distributions_addMany (& list);
	COMBINE_ALL_TO_ONE_END (U"added")
}

// MARK: Generate

FORM (CONVERT_EACH_TO_ONE__Distributions_to_Strings, U"To Strings", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	NATURAL (numberOfStrings, U"Number of strings", U"1000")
	OK
DO
	CONVERT_EACH_TO_ONE (Distributions)
		autoStrings result = Distributions_to_Strings (me, columnNumber, numberOfStrings);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Distributions_to_Strings_exact, U"To Strings (exact)", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Distributions)
		autoStrings result = Distributions_to_Strings_exact (me, columnNumber);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - LOGISTICREGRESSION

// MARK: Draw

FORM (GRAPHICS_EACH__LogisticRegression_drawBoundary, U"LogisticRegression: Draw boundary", nullptr) {
	SENTENCE (horizontalFactor, U"Horizontal factor", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalFactor, U"Vertical factor", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LogisticRegression)
		const integer xfactor = Regression_getFactorIndexFromFactorName_e (me, horizontalFactor);
		const integer yfactor = Regression_getFactorIndexFromFactorName_e (me, verticalFactor);
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, fromHorizontal, toHorizontal,
			yfactor, fromVertical, toVertical,
			garnish
		);
	GRAPHICS_EACH_END
}

// MARK: - PAIRDISTRIBUTION

// MARK: Help

DIRECT (HELP__PairDistribution_help) {
	HELP (U"PairDistribution")
}

// MARK: Query

DIRECT (QUERY_ONE_FOR_REAL__PairDistribution_getFractionCorrect_maximumLikelihood) {
	QUERY_ONE_FOR_REAL (PairDistribution)
		const double result = PairDistribution_getFractionCorrect_maximumLikelihood (me);
	QUERY_ONE_FOR_REAL_END (U" (fraction correct)")
}

DIRECT (QUERY_ONE_FOR_REAL__PairDistribution_getFractionCorrect_probabilityMatching) {
	QUERY_ONE_FOR_REAL (PairDistribution)
		const double result = PairDistribution_getFractionCorrect_probabilityMatching (me);
	QUERY_ONE_FOR_REAL_END (U" (fraction correct)")
}

DIRECT (QUERY_ONE_FOR_INTEGER__PairDistribution_getNumberOfPairs) {
	QUERY_ONE_FOR_INTEGER (PairDistribution)
		const integer result = my pairs.size;
	QUERY_ONE_FOR_INTEGER_END (U" pairs")
}

FORM (QUERY_ONE_FOR_STRING__PairDistribution_getString1, U"Get string1", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (PairDistribution)
		const conststring32 result = PairDistribution_getString1 (me, pairNumber);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_STRING__PairDistribution_getString2, U"Get string2", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (PairDistribution)
		const conststring32 result = PairDistribution_getString2 (me, pairNumber);
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_REAL__PairDistribution_getWeight, U"Get weight", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (PairDistribution)
		const double result = PairDistribution_getWeight (me, pairNumber);
	QUERY_ONE_FOR_REAL_END (U" (weight of pair ", pairNumber, U")")
}

// MARK: Modify

DIRECT (MODIFY_EACH__PairDistribution_removeZeroWeights) {
	MODIFY_EACH (PairDistribution)
		PairDistribution_removeZeroWeights (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__PairDistribution_swapInputsAndOutputs) {
	MODIFY_EACH (PairDistribution)
		PairDistribution_swapInputsAndOutputs (me);
	MODIFY_EACH_END
}

// MARK: Generate

FORM (CONVERT_ONE_TO_MULTIPLE__PairDistribution_to_Stringses, U"Generate two Strings objects", nullptr) {
	NATURAL (number, U"Number", U"1000")
	SENTENCE (nameOfFirstStrings, U"Name of first Strings", U"input")
	SENTENCE (nameOfSecondStrings, U"Name of second Strings", U"output")
	OK
DO
	CONVERT_ONE_TO_MULTIPLE (PairDistribution)
		autoStrings strings1, strings2;
		PairDistribution_to_Stringses (me, number, & strings1, & strings2);
		praat_new (strings1.move(), nameOfFirstStrings);
		praat_new (strings2.move(), nameOfSecondStrings);
	CONVERT_ONE_TO_MULTIPLE_END
}

DIRECT (CONVERT_EACH_TO_ONE__PairDistribution_to_Table) {
	CONVERT_EACH_TO_ONE (PairDistribution)
		autoTable result = PairDistribution_to_Table (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - PAIRDISTRIBUTION & DISTRIBUTIONS

FORM (QUERY_ONE_AND_ONE_FOR_REAL__PairDistribution_Distributions_getFractionCorrect, U"PairDistribution & Distributions: Get fraction correct", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_AND_ONE_FOR_REAL (PairDistribution, Distributions)
		const double result = PairDistribution_Distributions_getFractionCorrect (me, you, columnNumber);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" (fraction correct)")
}

// MARK: - TABLE

// MARK: New

FORM (NEW1_Table_createWithColumnNames, U"Create Table with column names", nullptr) {
	WORD (name, U"Name", U"table")
	INTEGER (numberOfRows, U"Number of rows", U"10")
	STRINGARRAY (columnNames, U"Column names", { U"speaker", U"dialect", U"age_yr", U"vowel", U"F0_Hz", U"F1_Hz", U"F2_Hz" })
	OK
DO
	CREATE_ONE
		autoTable result = Table_createWithColumnNames (numberOfRows, columnNames);
	CREATE_ONE_END (name)
}

FORM (NEW1_Table_createWithoutColumnNames, U"Create Table without column names", nullptr) {
	WORD (name, U"Name", U"table")
	INTEGER (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"3")
	OK
DO
	CREATE_ONE
		autoTable result = Table_createWithoutColumnNames (numberOfRows, numberOfColumns);
	CREATE_ONE_END (name)
}

// MARK: Open

FORM_READ (READ1_Table_readFromTableFile, U"Read Table from table file", nullptr, true) {
	READ_ONE
		autoTable result = Table_readFromTableFile (file);
	READ_ONE_END
}

FORM_READ (READ1_Table_readFromCommaSeparatedFile, U"Read Table from comma-separated file", nullptr, true) {
	READ_ONE
		autoTable result = Table_readFromCharacterSeparatedTextFile (file, U',', true);
	READ_ONE_END
}

FORM_READ (READ1_Table_readFromSemicolonSeparatedFile, U"Read Table from semicolon-separated file", nullptr, true) {
	READ_ONE
		autoTable result = Table_readFromCharacterSeparatedTextFile (file, U';', true);
	READ_ONE_END
}

FORM_READ (READ1_Table_readFromTabSeparatedFile, U"Read Table from tab-separated file", nullptr, true) {
	READ_ONE
		autoTable result = Table_readFromCharacterSeparatedTextFile (file, U'\t', false);
	READ_ONE_END
}

// MARK: Save

FORM_SAVE (SAVE_ONE__Table_writeToTabSeparatedFile, U"Save Table as tab-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToTabSeparatedFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Table_writeToCommaSeparatedFile, U"Save Table as comma-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToCommaSeparatedFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Table_writeToSemicolonSeparatedFile, U"Save Table as semicolon-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToSemicolonSeparatedFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP__StatisticsTutorial) {
	HELP (U"Statistics")
}

// MARK: View & Edit

DIRECT (EDITOR_ONE__Table_viewAndEdit) {
	EDITOR_ONE (a,Table)
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: Tabulate

FORM (INFO_ONE__Table_list, U"Table: List", nullptr) {
	BOOLEAN (includeRowNumbers, U"Include row numbers", true)
	OK
DO
	INFO_ONE (Table)
		Table_list (me, includeRowNumbers);
	INFO_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_EACH__Table_scatterPlot, U"Scatter plot", nullptr) {
	SENTENCE (horizontalColumnName, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumnName, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (marksColumnName, U"Column with marks", U"")
	POSITIVE (fontSize, U"Font size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer horizontalColumnNumber = Table_columnNameToNumber_e (me, horizontalColumnName);
		const integer verticalColumnNumber   = Table_columnNameToNumber_e (me, verticalColumnName);
		const integer marksColumnNumber      = Table_columnNameToNumber_e (me, marksColumnName);
		Table_scatterPlot (me, GRAPHICS, horizontalColumnNumber, verticalColumnNumber,
				fromHorizontal, toHorizontal, fromVertical, toVertical, marksColumnNumber, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_scatterPlot_mark, U"Scatter plot (marks)", nullptr) {
	SENTENCE (horizontalColumnName, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumnName, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (markSize, U"Mark size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	SENTENCE (markString, U"Mark string (+xo.)", U"+")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer horizontalColumnNumber = Table_columnNameToNumber_e (me, horizontalColumnName);
		const integer verticalColumnNumber   = Table_columnNameToNumber_e (me, verticalColumnName);
		Table_scatterPlot_mark (me, GRAPHICS, horizontalColumnNumber, verticalColumnNumber,
			fromHorizontal, toHorizontal, fromVertical, toVertical,
			markSize, markString, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__Table_drawEllipse, U"Draw ellipse (standard deviation)", nullptr) {
	SENTENCE (horizontalColumnName, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumnName, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer horizontalColumnNumber = Table_columnNameToNumber_e (me, horizontalColumnName);
		const integer verticalColumnNumber   = Table_columnNameToNumber_e (me, verticalColumnName);
		Table_drawEllipse_e (me, GRAPHICS, horizontalColumnNumber, verticalColumnNumber,
				fromHorizontal, toHorizontal, fromVertical, toVertical, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (QUERY_ONE_FOR_INTEGER__Table_drawRowFromDistribution, U"Table: Draw row from distribution", nullptr) {
	SENTENCE (columnWithDistribution, U"Column with distribution", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnWithDistribution);
		const integer result = Table_drawRowFromDistribution (me, columnNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (random row number)")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__Table_getAllNumbersInColumn, U"Table: Get all numbers in column", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnLabel);
		autoVEC result = Table_getAllNumbersInColumn (me, columnNumber);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_INTEGER__Table_getColumnIndex, U"Table: Get column index", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Table)
		const integer result = Table_columnNameToNumber_0 (me, columnLabel);
	QUERY_ONE_FOR_INTEGER_END (U" (index of column ", columnLabel, U")")
}

FORM (QUERY_ONE_FOR_STRING__Table_getColumnLabel, U"Table: Get column label", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (Table)
		if (columnNumber > my numberOfColumns)
			Melder_throw (U"Your column number should not be greater than the number of columns.");
		conststring32 result = my columnHeaders [columnNumber]. label.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_REAL__Table_getGroupMean, U"Table: Get group mean", nullptr) {
	SENTENCE (columnName, U"Column to average over", U"salary")
	SENTENCE (groupColumnName, U"Group column", U"gender")
	SENTENCE (group, U"Group", U"F")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber      = Table_columnNameToNumber_e (me, columnName);
		const integer groupColumnNumber = Table_columnNameToNumber_e (me, groupColumnName);
		const double result = Table_getGroupMean (me, columnNumber, groupColumnNumber, group);
	QUERY_ONE_FOR_REAL_END (U" (mean of ", columnName, U" in group ", group, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getMaximum, U"Table: Get maximum", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getMaximum (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (maximum of ", columnName, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getMean, U"Table: Get mean", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getMean (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (mean of ", columnName, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getMinimum, U"Table: Get minimum", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getMinimum (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (minimum of ", columnName, U")")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Table_getNumberOfColumns) {
	QUERY_ONE_FOR_INTEGER (Table)
		const integer result = my numberOfColumns;
	QUERY_ONE_FOR_INTEGER_END (U" columns")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Table_getNumberOfRows) {
	QUERY_ONE_FOR_INTEGER (Table)
		const integer result = my rows.size;
	QUERY_ONE_FOR_INTEGER_END (U" rows")
}

FORM (QUERY_ONE_FOR_REAL__Table_getQuantile, U"Table: Get quantile", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	POSITIVE (quantile, U"Quantile", U"0.50 (= median)")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getQuantile (me, columnNumber, quantile);
	QUERY_ONE_FOR_REAL_END (U" (", quantile, U" quantile of ", columnName, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getStandardDeviation, U"Table: Get standard deviation", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getStdev (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (standard deviation of ", columnName, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getSum, U"Table: Get sum", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const double result = Table_getSum (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (sum of ", columnName, U")")
}

FORM (QUERY_ONE_FOR_REAL__Table_getValue, U"Table: Get value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	QUERY_ONE_FOR_REAL (Table)
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		conststring32 result = my rows.at [rowNumber] -> cells [columnNumber]. string.get();
		/*
			The following lines are a hack:
			we are returning a number in a string.
		*/
		if (interpreter)
			interpreter -> returnType = kInterpreter_ReturnType::REAL_;
		Melder_information (result);
	END_NO_NEW_DATA   // SMELL
}

FORM (QUERY_ONE_FOR_INTEGER__Table_searchColumn, U"Table: Search column", nullptr) {
	SENTENCE (columnName, U"Column label", U"")
	SENTENCE (value, U"Value", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		const integer result = Table_searchColumn (me, columnNumber, value);
	QUERY_ONE_FOR_INTEGER_END (U" (first row in which ", columnName, U" is ", value, U")")
}
	
// MARK: Statistics

FORM (INFO_ONE__Table_reportCorrelation_kendallTau, U"Report correlation (Kendall tau)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		double significance, lowerLimit, upperLimit;
		const double correlation = Table_getCorrelation_kendallTau (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
				& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Correlation between column ", Table_messageColumn (me, columnNumber1),
				U" and column ", Table_messageColumn (me, columnNumber2), U":");
		MelderInfo_writeLine (U"Correlation = ", correlation, U" (Kendall's tau-b)");
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportCorrelation_pearsonR, U"Report correlation (Pearson r)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		double significance, lowerLimit, upperLimit;
		const double correlation = Table_getCorrelation_pearsonR (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
				& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Correlation between column ", Table_messageColumn (me, columnNumber1),
				U" and column ", Table_messageColumn (me, columnNumber2), U":");
		MelderInfo_writeLine (U"Correlation = ", correlation, U" (Pearson's r)");
		MelderInfo_writeLine (U"Number of degrees of freedom = ", my rows.size - 2);
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportDifference_studentT, U"Report difference (Student t)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		double t, numberOfDegreesOfFreedom, significance, lowerLimit, upperLimit;
		const double difference = Table_getDifference_studentT (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
				& t, & numberOfDegreesOfFreedom, & significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Difference between column ", Table_messageColumn (me, columnNumber1),
				U" and column ", Table_messageColumn (me, columnNumber2), U":");
		MelderInfo_writeLine (U"Difference = ", difference);
		MelderInfo_writeLine (U"Student's t = ", t);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportGroupDifference_studentT, U"Report group difference (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group1, U"Group 1", U"F")
	SENTENCE (group2, U"Group 2", U"M")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, column);
		const integer groupColumnNumber = Table_columnNameToNumber_e (me, groupColumn);
		double tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		const double mean = Table_getGroupDifference_studentT (me, columnNumber, groupColumnNumber, group1, group2, oneTailedUnconfidence,
				& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (U"Difference in column ", Table_messageColumn (me, columnNumber), U" between groups ", group1);
		MelderInfo_writeLine (U" and ", group2, U" of column ", Table_messageColumn (me, groupColumnNumber), U":");
		MelderInfo_writeLine (U"Difference = ", mean);
		MelderInfo_writeLine (U"Student's t = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportGroupDifference_wilcoxonRankSum, U"Report group difference (Wilcoxon rank sum)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group1, U"Group 1", U"F")
	SENTENCE (group2, U"Group 2", U"M")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, column);
		const integer groupColumnNumber = Table_columnNameToNumber_e (me, groupColumn);
		double areaUnderCurve, rankSum, significanceFromZero;
		areaUnderCurve = Table_getGroupDifference_wilcoxonRankSum (me, columnNumber, groupColumnNumber, group1, group2,
				& rankSum, & significanceFromZero);
		MelderInfo_open ();
		MelderInfo_write (U"Difference in column ", Table_messageColumn (me, columnNumber), U" between groups ", group1);
		MelderInfo_writeLine (U" and ", group2, U" of column ", Table_messageColumn (me, groupColumnNumber), U":");
		MelderInfo_writeLine (U"Larger: ", areaUnderCurve < 0.5 ? group1 : areaUnderCurve > 0.5 ? group2 : U"(both equal)");
		MelderInfo_writeLine (U"Area under curve: ", areaUnderCurve);
		MelderInfo_writeLine (U"Rank sum: ", rankSum);
		MelderInfo_writeLine (U"Significance from zero: ", significanceFromZero, U" (one-tailed)");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportGroupMean_studentT, U"Report group mean (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group, U"Group", U"F")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, column);
		const integer groupColumnNumber = Table_columnNameToNumber_e (me, groupColumn);
		double tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		const double mean = Table_getGroupMean_studentT (me, columnNumber, groupColumnNumber, group, oneTailedUnconfidence,
				& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (U"Mean in column ", Table_messageColumn (me, columnNumber), U" of group ", group);
		MelderInfo_writeLine (U" of column ", Table_messageColumn (me, groupColumnNumber), U":");
		MelderInfo_writeLine (U"Mean = ", mean);
		MelderInfo_writeLine (U"Student's t from zero = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_ONE__Table_reportMean_studentT, U"Report mean (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, column);
		double tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		const double mean = Table_getMean_studentT (me, columnNumber, oneTailedUnconfidence,
				& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Mean of column ", Table_messageColumn (me, columnNumber), U":");
		MelderInfo_writeLine (U"Mean = ", mean);
		MelderInfo_writeLine (U"Student's t from zero = ", tFromZero);
		MelderInfo_writeLine (U"Number of degrees of freedom = ", numberOfDegreesOfFreedom);
		MelderInfo_writeLine (U"Significance from zero = ", significanceFromZero, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * oneTailedUnconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
				U" (lowest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
				U" (highest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", oneTailedUnconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

// MARK: Modify

FORM (MODIFY_EACH__Table_appendColumn, U"Table: Append column", nullptr) {
	SENTENCE (name, U"Name", U"newcolumn")
	OK
DO
	MODIFY_EACH (Table)
		Table_appendColumn (me, name);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_appendDifferenceColumn, U"Table: Append difference column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"diff")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		Table_appendDifferenceColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_appendProductColumn, U"Table: Append product column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"prod")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		Table_appendProductColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_appendQuotientColumn, U"Table: Append quotient column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"quot")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		Table_appendQuotientColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_appendSumColumn, U"Table: Append sum column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"sum")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, column1);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, column2);
		Table_appendSumColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Table_appendRow) {
	MODIFY_EACH (Table)
		Table_appendRow (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH_WEAK__Table_formula, U"Table: Formula", U"Table: Formula...") {
	SENTENCE (columnName, U"Column (name)", U"")
	FORMULA (formula, U"Formula", U"abs (self)")
	OK
DO
	MODIFY_EACH_WEAK (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		Table_formula (me, columnNumber, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_EACH_WEAK__Table_formula_columnRange, U"Table: Formula (column range)", U"Table: Formula...") {
	SENTENCE (fromColumn, U"From column (name)", U"")
	SENTENCE (toColumn, U"To column (name)", U"")
	FORMULA (formula, U"Formula", U"log10 (self)")
	OK
DO
	MODIFY_EACH_WEAK (Table)
		const integer columnNumber1 = Table_columnNameToNumber_e (me, fromColumn);
		const integer columnNumber2 = Table_columnNameToNumber_e (me, toColumn);
		Table_formula_columnRange (me, columnNumber1, columnNumber2, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_EACH__Table_insertColumn, U"Table: Insert column", nullptr) {
	NATURAL (position, U"Position", U"1")
	SENTENCE (name, U"Name", U"newcolumn")
	OK
DO
	MODIFY_EACH (Table)
		Table_insertColumn (me, position, name);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_insertRow, U"Table: Insert row", nullptr) {
	NATURAL (position, U"Position", U"1")
	OK
DO
	MODIFY_EACH (Table)
		Table_insertRow (me, position);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_removeColumn, U"Table: Remove column", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		Table_removeColumn (me, columnNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_removeRow, U"Table: Remove row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_EACH (Table)
		Table_removeRow (me, rowNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_renameColumn_byNumber, U"Rename column (by column number)", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	SENTENCE (newName, U"New name", U"")
	OK
DO
	MODIFY_EACH (Table)
		Table_renameColumn_e (me, columnNumber, newName);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_renameColumn, U"Rename column", nullptr) {
	SENTENCE (oldName, U"Old name", U"")
	SENTENCE (newName, U"New name", U"")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, oldName);
		Table_renameColumn_e (me, columnNumber, newName);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_setNumericValue, U"Table: Set numeric value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnName, U"Column name", U"")
	REAL_OR_UNDEFINED (numericValue, U"Numeric value", U"1.5")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		Table_setNumericValue (me, rowNumber, columnNumber, numericValue);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_setStringValue, U"Table: Set string value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnName, U"Column name", U"")
	SENTENCE (stringValue, U"String value", U"xx")
	OK
DO
	MODIFY_EACH (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, columnName);
		Table_setStringValue (me, rowNumber, columnNumber, stringValue);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Table_randomizeRows) {
	MODIFY_EACH (Table)
		Table_randomizeRows (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Table_reflectRows) {
	MODIFY_EACH (Table)
		Table_reflectRows (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Table_sortRows, U"Table: Sort rows", nullptr) {
	STRINGARRAY (columnNames, U"One or more column names for sorting", { U"dialect", U"gender", U"name" })
	OK
DO
	MODIFY_EACH (Table)
		Table_sortRows (me, columnNames);
	MODIFY_EACH_END
}

// MARK: Convert

FORM (CONVERT_EACH_TO_ONE__Table_collapseRows, U"Table: Collapse rows", nullptr) {
	STRINGARRAY_LINES (3, factors, U"Columns with factors (independent variables)", { U"speaker", U"dialect", U"age", U"vowel" })
	STRINGARRAY_LINES (3, columnsToSum, U"Columns to sum", { U"number", U"cost" })
	STRINGARRAY_LINES (3, columnsToAverage, U"Columns to average", { U"price" })
	STRINGARRAY_LINES (3, columnsToMedianize, U"Columns to medianize", { U"vot" })
	STRINGARRAY_LINES (3, columnsToAverageLogarithmically, U"Columns to average logarithmically", { U"duration" })
	STRINGARRAY_LINES (3, columnsToMedianizeLogarithmically, U"Columns to medianize logarithmically", { U"F0", U"F1", U"F2", U"F3" })
	LABEL (U"Columns not mentioned above will be ignored.")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoTable result = Table_collapseRows (me, factors, columnsToSum, columnsToAverage,
				columnsToMedianize, columnsToAverageLogarithmically, columnsToMedianizeLogarithmically);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_pooled")
}

DIRECT (COMBINE_ALL_TO_ONE__Tables_append) {
	COMBINE_ALL_TO_ONE (Table)
		autoTable result = Tables_append (& list);
	COMBINE_ALL_TO_ONE_END (U"appended")
}

FORM (CONVERT_EACH_TO_ONE__Table_extractRowsWhereColumn_number, U"Table: Extract rows where column (number)", nullptr) {
	SENTENCE (extractAllRowsWhereColumn___, U"Extract all rows where column...", U"")
	CHOICE_ENUM (kMelder_number, ___is___, U"...is...", kMelder_number::DEFAULT)
	REAL (___theNumber, U"...the number", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, extractAllRowsWhereColumn___);
		autoTable result = Table_extractRowsWhereColumn_number (me, columnNumber, (kMelder_number) ___is___, ___theNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Table_messageColumn (me, columnNumber), U"_",
			isdefined (___theNumber) ? Melder_integer (Melder_iround (___theNumber)) : U"undefined")
}

FORM (CONVERT_EACH_TO_ONE__Table_extractRowsWhereColumn_text, U"Table: Extract rows where column (text)", nullptr) {
	SENTENCE (extractAllRowsWhereColumn___, U"Extract all rows where column...", U"")
	OPTIONMENU_ENUM (kMelder_string, ___, U"...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_e (me, extractAllRowsWhereColumn___);
		autoTable result = Table_extractRowsWhereColumn_string (me, columnNumber, ___, ___theText);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", ___theText)
}

DIRECT (CONVERT_EACH_TO_ONE__Table_transpose) {
	CONVERT_EACH_TO_ONE (Table)
		autoTable result = Table_transpose (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_transposed");
}

FORM (CONVERT_EACH_TO_ONE__Table_rowsToColumns, U"Table: Rows to columns", nullptr) {
	STRINGARRAY (factors, U"Columns with factors (independent variables)", { U"dialect", U"gender", U"speaker" })
	SENTENCE (columnToTranspose, U"Column to transpose", U"vowel")
	STRINGARRAY (columnsToExpand, U"Columns to expand", { U"duration", U"F0", U"F1", U"F2", U"F3" })
	LABEL (U"Columns not mentioned above will be ignored.")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoTable result = Table_rowsToColumns (me, factors, columnToTranspose, columnsToExpand);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_nested")
}

DIRECT (CONVERT_EACH_TO_ONE__Table_to_LinearRegression) {
	CONVERT_EACH_TO_ONE (Table)
		autoLinearRegression result = Table_to_LinearRegression (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Table_to_LogisticRegression, U"Table: To LogisticRegression", nullptr) {
	STRINGARRAY (factors, U"Factors (column names)", { U"F0", U"F1", U"duration" })
	SENTENCE (dependent1, U"Dependent 1 (column name)", U"e")
	SENTENCE (dependent2, U"Dependent 2 (column name)", U"i")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		autoLogisticRegression result = Table_to_LogisticRegression (me, factors, dependent1, dependent2);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Table_downto_TableOfReal, U"Table: Down to TableOfReal", nullptr) {
	SENTENCE (columnForRowLabels, U"Column for row labels", U"")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		const integer columnNumber = Table_columnNameToNumber_0 (me, columnForRowLabels);
		autoTableOfReal result = Table_to_TableOfReal (me, columnNumber);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Table_downto_Matrix) {
	CONVERT_EACH_TO_ONE (Table)
		autoMatrix result = Table_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Table_to_RealTier, U"Table: To RealTier", nullptr) {
	SENTENCE (columnWithTimes, U"Column with times", U"")
	SENTENCE (columnWithValues, U"Column with values", U"")
	REAL_OR_UNDEFINED (startTime, U"Start time (s)", U"0.0")
	REAL_OR_UNDEFINED (endTime, U"End time (s)", U"undefined")
	OK
DO
	CONVERT_EACH_TO_ONE (Table)
		const integer timeColumn = Table_columnNameToNumber_0 (me, columnWithTimes);
		const integer valueColumn = Table_columnNameToNumber_0 (me, columnWithValues);
		autoRealTier result = Table_to_RealTier (me, timeColumn, valueColumn, startTime, endTime);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW1_TableOfReal_create, U"Create TableOfReal", nullptr) {
	WORD (name, U"Name", U"table")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"3")
	OK
DO
	CREATE_ONE
		autoTableOfReal result = TableOfReal_create (numberOfRows, numberOfColumns);
	CREATE_ONE_END (name)
}

FORM_READ (READ1_TableOfReal_readFromHeaderlessSpreadsheetFile, U"Read TableOfReal from headerless spreadsheet file", nullptr, true) {
	READ_ONE
		autoTableOfReal result = TableOfReal_readFromHeaderlessSpreadsheetFile (file);
	READ_ONE_END
}

static bool isTabSeparated_8bit (integer nread, const char *header) {
	for (integer i = 0; i < nread; i ++) {
		if (header [i] == '\t')
			return true;
		if (header [i] == '\n' || header [i] == '\r')
			return false;
	}
	return false;
}

static bool isTabSeparated_utf16be (integer nread, const char *header) {
	for (integer i = 2; i < nread; i += 2) {
		if (header [i] == '\0' && header [i + 1] == '\t')
			return true;
		if (header [i] == '\0' && (header [i + 1] == '\n' || header [i + 1] == '\r'))
			return false;
	}
	return false;
}

static bool isTabSeparated_utf16le (integer nread, const char *header) {
	for (integer i = 2; i < nread; i += 2) {
		if (header [i + 1] == '\0' && header [i] == '\t')
			return true;
		if (header [i + 1] == '\0' && (header [i] == '\n' || header [i] == '\r'))
			return false;
	}
	return false;
}

static autoDaata tabSeparatedFileRecognizer (integer nread, const char *header, MelderFile file) {
	/*
	 * A table is recognized if it has at least one tab symbol,
	 * which must be before the first newline symbol (if any).
	 */
	unsigned char *uheader = (unsigned char *) header;
	const bool isTabSeparated =
		uheader [0] == 0xef && uheader [1] == 0xff ? isTabSeparated_utf16be (nread, header) :
		uheader [0] == 0xff && uheader [1] == 0xef ? isTabSeparated_utf16le (nread, header) :
		isTabSeparated_8bit (nread, header)
	;
	if (! isTabSeparated)
		return autoDaata ();
	return Table_readFromCharacterSeparatedTextFile (file, U'\t', false);
}

void praat_uvafon_stat_init ();
void praat_uvafon_stat_init () {

	Thing_recognizeClassesByName (classTableOfReal, classDistributions, classPairDistribution,
			classTable, classLinearRegression, classLogisticRegression, nullptr);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	structTableEditor :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"New", U"Tables", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table with column names...",
				nullptr, 1, NEW1_Table_createWithColumnNames);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table without column names... || Create Table...",
				nullptr, 1, NEW1_Table_createWithoutColumnNames);   // alternative GuiMenu_DEPRECATED_2006
		praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal...",
				nullptr, 1, NEW1_TableOfReal_create);

	praat_addMenuCommand (U"Objects", U"Open", U"-- open table --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from tab-separated file...",
			nullptr, 0, READ1_Table_readFromTabSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from comma-separated file...",
			nullptr, 0, READ1_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from semicolon-separated file...",
			nullptr, 0, READ1_Table_readFromSemicolonSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from whitespace-separated file... || Read Table from table file...",
			nullptr, 0, READ1_Table_readFromTableFile);   // alternative GuiMenu_DEPRECATED_2011
	praat_addMenuCommand (U"Objects", U"Open", U"Read TableOfReal from headerless spreadsheet file...",
			nullptr, 0, READ1_TableOfReal_readFromHeaderlessSpreadsheetFile);

	praat_addAction1 (classDistributions, 0, U"Distributions help", nullptr, 0,
			HELP__Distributions_help);
	praat_TableOfReal_init (classDistributions);
		praat_addAction1 (classDistributions, 1, U"Get probability (label)...",
				U"Get value...", 1, QUERY_ONE_FOR_REAL__Distributions_getProbability);
		praat_addAction1 (classDistributions, 0, U"-- get from two --",
				U"Get probability (label)...", 1, nullptr);
		praat_addAction1 (classDistributions, 2, U"Get mean absolute difference...",
				U"-- get from two --", 1, QUERY_ONE_FOR_REAL__Distributionses_getMeanAbsoluteDifference);
		praat_addAction1 (classDistributions, 0, U"-- add --",
				U"Append", 1, nullptr);
		praat_addAction1 (classDistributions, 0, U"Add",
				U"-- add --", 1, COMBINE_ALL_TO_ONE__Distributionses_add);
praat_addAction1 (classDistributions, 0, U"Generate", nullptr, 0, nullptr);
	praat_addAction1 (classDistributions, 0, U"To Strings...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Distributions_to_Strings);
	praat_addAction1 (classDistributions, 0, U"To Strings (exact)...",
			nullptr, 0, CONVERT_EACH_TO_ONE__Distributions_to_Strings_exact);

	praat_addAction1 (classLogisticRegression, 0, U"Draw boundary...",
			nullptr, 0, GRAPHICS_EACH__LogisticRegression_drawBoundary);

	praat_addAction1 (classPairDistribution, 0, U"PairDistribution help",
			nullptr, 0, HELP__PairDistribution_help);
	praat_addAction1 (classPairDistribution, 0, U"To Table",
			nullptr, 0, CONVERT_EACH_TO_ONE__PairDistribution_to_Table);
	praat_addAction1 (classPairDistribution, 1, U"To Stringses...",
			nullptr, 0, CONVERT_ONE_TO_MULTIPLE__PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get number of pairs",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__PairDistribution_getNumberOfPairs);
		praat_addAction1 (classPairDistribution, 1, U"Get string1...",
				nullptr, 1, QUERY_ONE_FOR_STRING__PairDistribution_getString1);
		praat_addAction1 (classPairDistribution, 1, U"Get string2...",
				nullptr, 1, QUERY_ONE_FOR_STRING__PairDistribution_getString2);
		praat_addAction1 (classPairDistribution, 1, U"Get weight...",
				nullptr, 1, QUERY_ONE_FOR_REAL__PairDistribution_getWeight);
		praat_addAction1 (classPairDistribution, 1, U"-- get fraction correct --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (maximum likelihood)",
				nullptr, 1, QUERY_ONE_FOR_REAL__PairDistribution_getFractionCorrect_maximumLikelihood);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (probability matching)",
				nullptr, 1, QUERY_ONE_FOR_REAL__PairDistribution_getFractionCorrect_probabilityMatching);
	praat_addAction1 (classPairDistribution, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPairDistribution, 0, U"Remove zero weights",
				nullptr, 1, MODIFY_EACH__PairDistribution_removeZeroWeights);
		praat_addAction1 (classPairDistribution, 0, U"Swap inputs and outputs",
				nullptr, 1, MODIFY_EACH__PairDistribution_swapInputsAndOutputs);

	praat_addAction1 (classTable, 0, U"Table help",
			nullptr, 0, HELP__Table_help);
	praat_addAction1 (classTable, 1, U"Save as tab-separated file... || Save as table file... || Write to table file...",
			nullptr, 0, SAVE_ONE__Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as comma-separated file...",
			nullptr, 0, SAVE_ONE__Table_writeToCommaSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as semicolon-separated file...",
			nullptr, 0, SAVE_ONE__Table_writeToSemicolonSeparatedFile);
	praat_addAction1 (classTable, 1, U"View & Edit || Edit",
			nullptr, GuiMenu_ATTRACTIVE | GuiMenu_NO_API, EDITOR_ONE__Table_viewAndEdit);
	praat_addAction1 (classTable, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Scatter plot...",
				nullptr, 1, GRAPHICS_EACH__Table_scatterPlot);
		praat_addAction1 (classTable, 0, U"Scatter plot (mark)...",
				nullptr, 1, GRAPHICS_EACH__Table_scatterPlot_mark);
		praat_addAction1 (classTable, 0, U"Draw ellipse (standard deviation)...",
				nullptr, 1, GRAPHICS_EACH__Table_drawEllipse);
	praat_addAction1 (classTable, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"List...",
				nullptr, 1, INFO_ONE__Table_list);
		praat_addAction1 (classTable, 1, U"-- get structure --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get number of rows",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, U"Get number of columns",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, U"Get column label...",
				nullptr, 1, QUERY_ONE_FOR_STRING__Table_getColumnLabel);
		praat_addAction1 (classTable, 1, U"Get column index...",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Table_getColumnIndex);
		praat_addAction1 (classTable, 1, U"-- get value --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get value...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getValue);
		praat_addAction1 (classTable, 1, U"Get all numbers in column...",
				nullptr, 1, QUERY_ONE_FOR_REAL_VECTOR__Table_getAllNumbersInColumn);
		praat_addAction1 (classTable, 1, U"Search column...",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Table_searchColumn);
		praat_addAction1 (classTable, 1, U"-- statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Statistics tutorial",
				nullptr, 1, HELP__StatisticsTutorial);
		praat_addAction1 (classTable, 1, U"-- get stats --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get quantile...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getQuantile);
		praat_addAction1 (classTable, 1, U"Get minimum...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getMinimum);
		praat_addAction1 (classTable, 1, U"Get maximum...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getMaximum);
		praat_addAction1 (classTable, 1, U"Get sum...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getSum);
		praat_addAction1 (classTable, 1, U"Get mean...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getMean);
		praat_addAction1 (classTable, 1, U"Get group mean...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getGroupMean);
		praat_addAction1 (classTable, 1, U"Get standard deviation...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, U"-- report stats --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Report mean (Student t)...",
				nullptr, 1, INFO_ONE__Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, U"Report standard deviation...",
				nullptr, 1, INFO_ONE__Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, U"Report difference (Student t)...",
				nullptr, 1, INFO_ONE__Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group mean (Student t)...",
				nullptr, 1, INFO_ONE__Table_reportGroupMean_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Student t)...",
				nullptr, 1, INFO_ONE__Table_reportGroupDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Wilcoxon rank sum)...",
				nullptr, 1, INFO_ONE__Table_reportGroupDifference_wilcoxonRankSum);
		praat_addAction1 (classTable, 1, U"Report correlation (Pearson r)...",
				nullptr, 1, INFO_ONE__Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, U"Report correlation (Kendall tau)...",
				nullptr, 1, INFO_ONE__Table_reportCorrelation_kendallTau);
	praat_addAction1 (classTable, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Set string value...",
				nullptr, 1, MODIFY_EACH__Table_setStringValue);
		praat_addAction1 (classTable, 0, U"Set numeric value...",
				nullptr, 1, MODIFY_EACH__Table_setNumericValue);
		praat_addAction1 (classTable, 0, U"Formula...",
				nullptr, 1, MODIFY_EACH_WEAK__Table_formula);
		praat_addAction1 (classTable, 0, U"Formula (column range)...",
				nullptr, 1, MODIFY_EACH_WEAK__Table_formula_columnRange);
		praat_addAction1 (classTable, 0, U"Sort rows...",
				nullptr, 1, MODIFY_EACH__Table_sortRows);
		praat_addAction1 (classTable, 0, U"Randomize rows",
				nullptr, 1, MODIFY_EACH__Table_randomizeRows);
		praat_addAction1 (classTable, 0, U"Reflect rows",
				nullptr, 1, MODIFY_EACH__Table_reflectRows);
		praat_addAction1 (classTable, 0, U"-- structure --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Append row",
				nullptr, 1, MODIFY_EACH__Table_appendRow);
		praat_addAction1 (classTable, 0, U"Append column...",
				nullptr, 1, MODIFY_EACH__Table_appendColumn);
		praat_addAction1 (classTable, 0, U"Append sum column...",
				nullptr, 1, MODIFY_EACH__Table_appendSumColumn);
		praat_addAction1 (classTable, 0, U"Append difference column...",
				nullptr, 1, MODIFY_EACH__Table_appendDifferenceColumn);
		praat_addAction1 (classTable, 0, U"Append product column...",
				nullptr, 1, MODIFY_EACH__Table_appendProductColumn);
		praat_addAction1 (classTable, 0, U"Append quotient column...",
				nullptr, 1, MODIFY_EACH__Table_appendQuotientColumn);
		praat_addAction1 (classTable, 0, U"Remove row...",
				nullptr, 1, MODIFY_EACH__Table_removeRow);
		praat_addAction1 (classTable, 0, U"Remove column...",
				nullptr, 1, MODIFY_EACH__Table_removeColumn);
		praat_addAction1 (classTable, 0, U"Insert row...",
				nullptr, 1, MODIFY_EACH__Table_insertRow);
		praat_addAction1 (classTable, 0, U"Insert column...",
				nullptr, 1, MODIFY_EACH__Table_insertColumn);
		praat_addAction1 (classTable, 0, U"-- set --",
				nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Rename column... || Set column label (label)...",   // alternative deprecated 2023
				nullptr, 1, MODIFY_EACH__Table_renameColumn);
		praat_addAction1 (classTable, 0, U"Rename column (by number)... || Set column label (index)...",   // alternative deprecated 2023
				nullptr, 1, MODIFY_EACH__Table_renameColumn_byNumber);
	praat_addAction1 (classTable, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"To linear regression",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_to_LinearRegression);
		praat_addAction1 (classTable, 0, U"To logistic regression...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Append",
				nullptr, 1, COMBINE_ALL_TO_ONE__Tables_append);
	praat_addAction1 (classTable, 0, U"Generate -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"Draw row from distribution...",
				nullptr, 1, QUERY_ONE_FOR_INTEGER__Table_drawRowFromDistribution);
	praat_addAction1 (classTable, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Extract rows where column (number)... || Extract rows where column... || Select rows where column...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_extractRowsWhereColumn_number);   // alternative GuiMenu_DEPRECATED_2006/2004
		praat_addAction1 (classTable, 0, U"Extract rows where column (text)...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_extractRowsWhereColumn_text);
		praat_addAction1 (classTable, 0, U"Transpose",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_transpose);
		praat_addAction1 (classTable, 0, U"Collapse rows...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_collapseRows);
		praat_addAction1 (classTable, 0, U"Rows to columns...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_rowsToColumns);
	praat_addAction1 (classTable, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"To RealTier...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_to_RealTier);
		praat_addAction1 (classTable, 0, U"Down to TableOfReal...",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_downto_TableOfReal);
		praat_addAction1 (classTable, 0, U"Down to Matrix",
				nullptr, 1, CONVERT_EACH_TO_ONE__Table_downto_Matrix);

	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, U"Get fraction correct...",
			nullptr, 0, QUERY_ONE_AND_ONE_FOR_REAL__PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.cpp */
