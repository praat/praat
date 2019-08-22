/* praat_Stat.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
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
#include "../kar/UnicodeData.h"

#include "praat_TableOfReal.h"

// MARK: - DISTRIBUTIONS

// MARK: Help

DIRECT (HELP_Distributions_help) {
	HELP (U"Distributions")
}

DIRECT (HELP_Table_help) {
	HELP (U"Table")
}

// MARK: Query

FORM (REAL_Distributionses_getMeanAbsoluteDifference, U"Get mean difference", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_COUPLE (Distributions)
		double result = Distributionses_getMeanAbsoluteDifference (me, you, columnNumber);
	NUMBER_COUPLE_END (U" (mean absolute difference between columns ", columnNumber, U")")
}

FORM (REAL_Distributions_getProbability, U"Get probability", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	SENTENCE (string, U"String", U"")
	OK
DO
	NUMBER_ONE (Distributions)
		double result = Distributions_getProbability (me, string, columnNumber);
	NUMBER_ONE_END (U" (probability)")
}

// MARK: Modify

DIRECT (NEW1_Distributionses_add) {
	CONVERT_LIST (Distributions)
		autoDistributions result = Distributions_addMany (& list);
	CONVERT_LIST_END (U"added")
}

// MARK: Generate

FORM (NEW_Distributions_to_Strings, U"To Strings", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	NATURAL (numberOfStrings, U"Number of strings", U"1000")
	OK
DO
	CONVERT_EACH (Distributions)
		autoStrings result = Distributions_to_Strings (me, columnNumber, numberOfStrings);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Distributions_to_Strings_exact, U"To Strings (exact)", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_EACH (Distributions)
		autoStrings result = Distributions_to_Strings_exact (me, columnNumber);
	CONVERT_EACH_END (my name.get())
}

// MARK: - LOGISTICREGRESSION

// MARK: Draw

FORM (GRAPHICS_LogisticRegression_drawBoundary, U"LogisticRegression: Draw boundary", nullptr) {
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
		integer xfactor = Regression_getFactorIndexFromFactorName_e (me, horizontalFactor);
		integer yfactor = Regression_getFactorIndexFromFactorName_e (me, verticalFactor);
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, fromHorizontal, toHorizontal,
			yfactor, fromVertical, toVertical,
			garnish
		);
	GRAPHICS_EACH_END
}

// MARK: - PAIRDISTRIBUTION

// MARK: Help

DIRECT (HELP_PairDistribution_help) {
	HELP (U"PairDistribution")
}

// MARK: Query

DIRECT (REAL_PairDistribution_getFractionCorrect_maximumLikelihood) {
	NUMBER_ONE (PairDistribution)
		double result = PairDistribution_getFractionCorrect_maximumLikelihood (me);
	NUMBER_ONE_END (U" (fraction correct)")
}

DIRECT (REAL_PairDistribution_getFractionCorrect_probabilityMatching) {
	NUMBER_ONE (PairDistribution)
		double result = PairDistribution_getFractionCorrect_probabilityMatching (me);
	NUMBER_ONE_END (U" (fraction correct)")
}

DIRECT (INTEGER_PairDistribution_getNumberOfPairs) {
	NUMBER_ONE (PairDistribution)
		integer result = my pairs.size;
	NUMBER_ONE_END (U" pairs")
}

FORM (STRING_PairDistribution_getString1, U"Get string1", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	STRING_ONE (PairDistribution)
		conststring32 result = PairDistribution_getString1 (me, pairNumber);
	STRING_ONE_END
}

FORM (STRING_PairDistribution_getString2, U"Get string2", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	STRING_ONE (PairDistribution)
		conststring32 result = PairDistribution_getString2 (me, pairNumber);
	STRING_ONE_END
}

FORM (REAL_PairDistribution_getWeight, U"Get weight", nullptr) {
	NATURAL (pairNumber, U"Pair number", U"1")
	OK
DO
	NUMBER_ONE (PairDistribution)
		double result = PairDistribution_getWeight (me, pairNumber);
	NUMBER_ONE_END (U" (weight of pair ", pairNumber, U")")
}

// MARK: Modify

DIRECT (MODIFY_PairDistribution_removeZeroWeights) {
	MODIFY_EACH (PairDistribution)
		PairDistribution_removeZeroWeights (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_PairDistribution_swapInputsAndOutputs) {
	MODIFY_EACH (PairDistribution)
		PairDistribution_swapInputsAndOutputs (me);
	MODIFY_EACH_END
}

// MARK: Generate

FORM (NEW2_PairDistribution_to_Stringses, U"Generate two Strings objects", nullptr) {
	NATURAL (number, U"Number", U"1000")
	SENTENCE (nameOfFirstStrings, U"Name of first Strings", U"input")
	SENTENCE (nameOfSecondStrings, U"Name of second Strings", U"output")
	OK
DO
	FIND_ONE (PairDistribution)
		autoStrings strings1, strings2;
		PairDistribution_to_Stringses (me, number, & strings1, & strings2);
		praat_new (strings1.move(), nameOfFirstStrings);
		praat_new (strings2.move(), nameOfSecondStrings);
	END
}

DIRECT (NEW_PairDistribution_to_Table) {
	CONVERT_EACH (PairDistribution)
		autoTable result = PairDistribution_to_Table (me);
	CONVERT_EACH_END (my name.get())
}

// MARK: - PAIRDISTRIBUTION & DISTRIBUTIONS

FORM (REAL_PairDistribution_Distributions_getFractionCorrect, U"PairDistribution & Distributions: Get fraction correct", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_TWO (PairDistribution, Distributions)
		const double result = PairDistribution_Distributions_getFractionCorrect (me, you, columnNumber);
	NUMBER_TWO_END (U" (fraction correct)")
}

// MARK: - TABLE

// MARK: New

FORM (NEW1_Table_createWithColumnNames, U"Create Table with column names", nullptr) {
	WORD (name, U"Name", U"table")
	INTEGER (numberOfRows, U"Number of rows", U"10")
	TEXTFIELD (columnNames, U"Column names:", U"speaker dialect age vowel F0 F1 F2")
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

FORM_SAVE (SAVE_Table_writeToTabSeparatedFile, U"Save Table as tab-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToTabSeparatedFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Table_writeToCommaSeparatedFile, U"Save Table as comma-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToCommaSeparatedFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Table_writeToSemicolonSeparatedFile, U"Save Table as semicolon-separated file", nullptr, U"Table") {
	SAVE_ONE (Table)
		Table_writeToSemicolonSeparatedFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_StatisticsTutorial) {
	HELP (U"Statistics")
}

// MARK: View & Edit

DIRECT (WINDOW_Table_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a Table from batch.");
	FIND_ONE_WITH_IOBJECT (Table)
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Tabulate

FORM (LIST_Table_list, U"Table: List", nullptr) {
	BOOLEAN (includeRowNumbers, U"Include row numbers", true)
	OK
DO
	INFO_ONE (Table)
		Table_list (me, includeRowNumbers);
	INFO_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_Table_scatterPlot, U"Scatter plot", nullptr) {
	SENTENCE (horizontalColumn, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumn, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	SENTENCE (columnWithMarks, U"Column with marks", U"")
	POSITIVE (fontSize, U"Font size", U"12")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		const integer ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		const integer markColumn = Table_getColumnIndexFromColumnLabel (me, columnWithMarks);
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
				fromHorizontal, toHorizontal, fromVertical, toVertical, markColumn, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_scatterPlot_mark, U"Scatter plot (marks)", nullptr) {
	SENTENCE (horizontalColumn, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumn, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (markSize, U"Mark size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	SENTENCE (markString, U"Mark string (+xo.)", U"+")
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		const integer ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			fromHorizontal, toHorizontal, fromVertical, toVertical,
			markSize, markString, garnish
		);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipse, U"Draw ellipse (standard deviation)", nullptr) {
	SENTENCE (horizontalColumn, U"Horizontal column", U"")
	REAL (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	SENTENCE (verticalColumn, U"Vertical column", U"")
	REAL (fromVertical, U"left Vertical range", U"0.0")
	REAL (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		const integer xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		const integer ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		Table_drawEllipse_e (me, GRAPHICS, xcolumn, ycolumn,
				fromHorizontal, toHorizontal, fromVertical, toVertical, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (INTEGER_Table_drawRowFromDistribution, U"Table: Draw row from distribution", nullptr) {
	SENTENCE (columnWithDistribution, U"Column with distribution", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnWithDistribution);
		const integer result = Table_drawRowFromDistribution (me, columnNumber);
	NUMBER_ONE_END (U" (random row number)")
}

FORM (INTEGER_Table_getColumnIndex, U"Table: Get column index", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer result = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	NUMBER_ONE_END (U" (index of column ", columnLabel, U")")
}

FORM (STRING_Table_getColumnLabel, U"Table: Get column label", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	STRING_ONE (Table)
		if (columnNumber > my numberOfColumns)
			Melder_throw (U"Your column number should not be greater than the number of columns.");
		conststring32 result = my columnHeaders [columnNumber]. label.get();
	STRING_ONE_END
}

FORM (REAL_Table_getGroupMean, U"Table: Get group mean", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"salary")
	SENTENCE (groupColumnLabel, U"Group column label", U"gender")
	SENTENCE (group, U"Group", U"F")
	OK
DO
	NUMBER_ONE (Table)
		const integer column = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const integer groupColumn = Table_getColumnIndexFromColumnLabel (me, groupColumnLabel);
		const double result = Table_getGroupMean (me, column, groupColumn, group);
	NUMBER_ONE_END (U" (mean of ", columnLabel, U" in group ", group, U")")
}

FORM (REAL_Table_getMaximum, U"Table: Get maximum", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const double result = Table_getMaximum (me, columnNumber);
	NUMBER_ONE_END (U" (maximum of ", columnLabel, U")")
}

FORM (REAL_Table_getMean, U"Table: Get mean", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const double result = Table_getMean (me, columnNumber);
	NUMBER_ONE_END (U" (mean of ", columnLabel, U")")
}

FORM (REAL_Table_getMinimum, U"Table: Get minimum", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const double result = Table_getMinimum (me, columnNumber);
	NUMBER_ONE_END (U" (minimum of ", columnLabel, U")")
}

FORM (REAL_Table_getQuantile, U"Table: Get quantile", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	POSITIVE (quantile, U"Quantile", U"0.50 (= median)")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const double result = Table_getQuantile (me, columnNumber, quantile);
	NUMBER_ONE_END (U" (", quantile, U" quantile of ", columnLabel, U")")
}

FORM (REAL_Table_getStandardDeviation, U"Table: Get standard deviation", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		const integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const double result = Table_getStdev (me, columnNumber);
	NUMBER_ONE_END (U" (standard deviation of ", columnLabel, U")")
}

DIRECT (INTEGER_Table_getNumberOfColumns) {
	NUMBER_ONE (Table)
		const integer result = my numberOfColumns;
	NUMBER_ONE_END (U" columns")
}

DIRECT (INTEGER_Table_getNumberOfRows) {
	NUMBER_ONE (Table)
		const integer result = my rows.size;
	NUMBER_ONE_END (U" rows")
}

FORM (STRING_Table_getValue, U"Table: Get value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	STRING_ONE (Table)
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		conststring32 result = my rows.at [rowNumber] -> cells [columnNumber]. string.get();
	STRING_ONE_END
}

FORM (INTEGER_Table_searchColumn, U"Table: Search column", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	SENTENCE (value, U"Value", U"")
	OK
DO
	NUMBER_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		integer result = Table_searchColumn (me, columnNumber, value);
	NUMBER_ONE_END (U" (first row in which ", columnLabel, U" is ", value)
}
	
// MARK: Statistics

FORM (INFO_Table_reportCorrelation_kendallTau, U"Report correlation (Kendall tau)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_kendallTau (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
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

FORM (INFO_Table_reportCorrelation_pearsonR, U"Report correlation (Pearson r)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_pearsonR (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
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

FORM (INFO_Table_reportDifference_studentT, U"Report difference (Student t)", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		double difference, t, numberOfDegreesOfFreedom, significance, lowerLimit, upperLimit;
		difference = Table_getDifference_studentT (me, columnNumber1, columnNumber2, oneTailedUnconfidence,
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

FORM (INFO_Table_reportGroupDifference_studentT, U"Report group difference (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group1, U"Group 1", U"F")
	SENTENCE (group2, U"Group 2", U"M")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, column);
		integer groupColumnNumber = Table_getColumnIndexFromColumnLabel (me, groupColumn);
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupDifference_studentT (me, columnNumber, groupColumnNumber, group1, group2, oneTailedUnconfidence,
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

FORM (INFO_Table_reportGroupDifference_wilcoxonRankSum, U"Report group difference (Wilcoxon rank sum)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group1, U"Group 1", U"F")
	SENTENCE (group2, U"Group 2", U"M")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, column);
		integer groupColumnNumber = Table_getColumnIndexFromColumnLabel (me, groupColumn);
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

FORM (INFO_Table_reportGroupMean_studentT, U"Report group mean (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"salary")
	SENTENCE (groupColumn, U"Group column", U"gender")
	SENTENCE (group, U"Group", U"F")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, column);
		integer groupColumnNumber = Table_getColumnIndexFromColumnLabel (me, groupColumn);
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupMean_studentT (me, columnNumber, groupColumnNumber, group, oneTailedUnconfidence,
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

FORM (INFO_Table_reportMean_studentT, U"Report mean (Student t)", nullptr) {
	SENTENCE (column, U"Column", U"")
	POSITIVE (oneTailedUnconfidence, U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, column);
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getMean_studentT (me, columnNumber, oneTailedUnconfidence,
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

FORM (MODIFY_Table_appendColumn, U"Table: Append column", nullptr) {
	SENTENCE (label, U"Label", U"newcolumn")
	OK
DO
	MODIFY_EACH (Table)
		Table_appendColumn (me, label);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_appendDifferenceColumn, U"Table: Append difference column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"diff")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		Table_appendDifferenceColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_appendProductColumn, U"Table: Append product column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"prod")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		Table_appendProductColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_appendQuotientColumn, U"Table: Append quotient column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"quot")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		Table_appendQuotientColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_appendSumColumn, U"Table: Append sum column", nullptr) {
	SENTENCE (column1, U"left Columns", U"")
	SENTENCE (column2, U"right Columns", U"")
	SENTENCE (label, U"Label", U"sum")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, column1);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, column2);
		Table_appendSumColumn (me, columnNumber1, columnNumber2, label);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Table_appendRow) {
	MODIFY_EACH (Table)
		Table_appendRow (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_formula, U"Table: Formula", U"Table: Formula...") {
	SENTENCE (columnLabel, U"Column (label)", U"")
	TEXTFIELD (formula, U"Formula:", U"abs (self)")
	OK
DO
	MODIFY_EACH_WEAK (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		Table_formula (me, columnNumber, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Table_formula_columnRange, U"Table: Formula (column range)", U"Table: Formula...") {
	SENTENCE (fromColumn, U"From column (label)", U"")
	SENTENCE (toColumn, U"To column (label)", U"")
	TEXTFIELD (formula, U"Formula:", U"log10 (self)")
	OK
DO
	MODIFY_EACH_WEAK (Table)
		integer columnNumber1 = Table_getColumnIndexFromColumnLabel (me, fromColumn);
		integer columnNumber2 = Table_getColumnIndexFromColumnLabel (me, toColumn);
		Table_formula_columnRange (me, columnNumber1, columnNumber2, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Table_insertColumn, U"Table: Insert column", nullptr) {
	NATURAL (position, U"Position", U"1")
	SENTENCE (label, U"Label", U"newcolumn")
	OK
DO
	MODIFY_EACH (Table)
		Table_insertColumn (me, position, label);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_insertRow, U"Table: Insert row", nullptr) {
	NATURAL (position, U"Position", U"1")
	OK
DO
	MODIFY_EACH (Table)
		Table_insertRow (me, position);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_removeColumn, U"Table: Remove column", nullptr) {
	SENTENCE (columnLabel, U"Column label", U"")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		Table_removeColumn (me, columnNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_removeRow, U"Table: Remove row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_EACH (Table)
		Table_removeRow (me, rowNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_setColumnLabel_index, U"Set column label", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	SENTENCE (newLabel, U"New label", U"")
	OK
DO
	MODIFY_EACH (Table)
		Table_setColumnLabel (me, columnNumber, newLabel);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_setColumnLabel_label, U"Set column label", nullptr) {
	SENTENCE (oldLabel, U"Old label", U"")
	SENTENCE (newLabel, U"New label", U"")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, oldLabel);
		Table_setColumnLabel (me, columnNumber, newLabel);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_setNumericValue, U"Table: Set numeric value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnLabel, U"Column label", U"")
	REAL_OR_UNDEFINED (numericValue, U"Numeric value", U"1.5")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		Table_setNumericValue (me, rowNumber, columnNumber, numericValue);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_setStringValue, U"Table: Set string value", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	SENTENCE (columnLabel, U"Column label", U"")
	SENTENCE (stringValue, U"String value", U"xx")
	OK
DO
	MODIFY_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		Table_setStringValue (me, rowNumber, columnNumber, stringValue);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Table_randomizeRows) {
	MODIFY_EACH (Table)
		Table_randomizeRows (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Table_reflectRows) {
	MODIFY_EACH (Table)
		Table_reflectRows (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Table_sortRows, U"Table: Sort rows", nullptr) {
	TEXTFIELD (columnLabels, U"One or more column labels for sorting:", U"dialect gender name")
	OK
DO
	MODIFY_EACH (Table)
		Table_sortRows_string (me, columnLabels);
	MODIFY_EACH_END
}

// MARK: Convert

FORM (NEW_Table_collapseRows, U"Table: Collapse rows", nullptr) {
	TEXTFIELD (factors, U"Columns with factors (independent variables):", U"speaker dialect age vowel")
	TEXTFIELD (columnsToSum, U"Columns to sum:", U"number cost")
	TEXTFIELD (columnsToAverage, U"Columns to average:", U"price")
	TEXTFIELD (columnsToMedianize, U"Columns to medianize:", U"vot")
	TEXTFIELD (columnsToAverageLogarithmically, U"Columns to average logarithmically:", U"duration")
	TEXTFIELD (columnsToMedianizeLogarithmically, U"Columns to medianize logarithmically:", U"F0 F1 F2 F3")
	LABEL (U"Columns not mentioned above will be ignored.")
	OK
DO
	CONVERT_EACH (Table)
		autoTable result = Table_collapseRows (me, factors, columnsToSum, columnsToAverage,
			columnsToMedianize, columnsToAverageLogarithmically, columnsToMedianizeLogarithmically);
	CONVERT_EACH_END (my name.get(), U"_pooled")
}

DIRECT (NEW1_Tables_append) {
	CONVERT_LIST (Table)
		autoTable result = Tables_append (& list);
	CONVERT_LIST_END (U"appended")
}

FORM (NEW_Table_extractRowsWhereColumn_number, U"Table: Extract rows where column (number)", nullptr) {
	SENTENCE (extractAllRowsWhereColumn___, U"Extract all rows where column...", U"")
	RADIO_ENUM (kMelder_number, ___is___, U"...is...", kMelder_number::DEFAULT)
	REAL (___theNumber, U"...the number", U"0.0")
	OK
DO
	CONVERT_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, extractAllRowsWhereColumn___);
		autoTable result = Table_extractRowsWhereColumn_number (me, columnNumber, (kMelder_number) ___is___, ___theNumber);
	CONVERT_EACH_END (my name.get(), U"_", Table_messageColumn (me, columnNumber), U"_",
			isdefined (___theNumber) ? Melder_integer (Melder_iround (___theNumber)) : U"undefined")
}

FORM (NEW_Table_extractRowsWhereColumn_text, U"Table: Extract rows where column (text)", nullptr) {
	SENTENCE (extractAllRowsWhereColumn___, U"Extract all rows where column...", U"")
	OPTIONMENU_ENUM (kMelder_string, ___, U"...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, extractAllRowsWhereColumn___);
		autoTable result = Table_extractRowsWhereColumn_string (me, columnNumber, ___, ___theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

DIRECT (NEW_Table_transpose) {
	CONVERT_EACH (Table)
		autoTable result = Table_transpose (me);
	CONVERT_EACH_END (my name.get(), U"_transposed");
}

FORM (NEW_Table_rowsToColumns, U"Table: Rows to columns", nullptr) {
	TEXTFIELD (factors, U"Columns with factors (independent variables):", U"dialect gender speaker")
	SENTENCE (columnToTranspose, U"Column to transpose", U"vowel")
	TEXTFIELD (columnsToExpand, U"Columns to expand:", U"duration F0 F1 F2 F3")
	LABEL (U"Columns not mentioned above will be ignored.")
	OK
DO
	CONVERT_EACH (Table)
		integer columnNumber = Table_getColumnIndexFromColumnLabel (me, columnToTranspose);
		autoTable result = Table_rowsToColumns (me, factors, columnNumber, columnsToExpand);
	CONVERT_EACH_END (my name.get(), U"_nested")
}

DIRECT (NEW_Table_to_LinearRegression) {
	CONVERT_EACH (Table)
		autoLinearRegression result = Table_to_LinearRegression (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Table_to_LogisticRegression, U"Table: To LogisticRegression", nullptr) {
	TEXTFIELD (factors, U"Factors (column names):", U"F0 F1 duration")
	SENTENCE (dependent1, U"Dependent 1 (column name)", U"e")
	SENTENCE (dependent2, U"Dependent 2 (column name)", U"i")
	OK
DO
	CONVERT_EACH (Table)
		autoLogisticRegression result = Table_to_LogisticRegression (me, factors, dependent1, dependent2);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Table_downto_TableOfReal, U"Table: Down to TableOfReal", nullptr) {
	SENTENCE (columnForRowLabels, U"Column for row labels", U"")
	OK
DO
	CONVERT_EACH (Table)
		integer columnNumber = Table_findColumnIndexFromColumnLabel (me, columnForRowLabels);
		autoTableOfReal result = Table_to_TableOfReal (me, columnNumber);
	CONVERT_EACH_END (my name.get())
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
	bool isTabSeparated =
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
		praat_addMenuCommand (U"Objects", U"New", U"Create Table with column names...", nullptr, 1, NEW1_Table_createWithColumnNames);
		praat_addMenuCommand (U"Objects", U"New", U"Create Table without column names...", nullptr, 1, NEW1_Table_createWithoutColumnNames);
		praat_addMenuCommand (U"Objects", U"New",   U"Create Table...", U"*Create Table without column names...", praat_DEPTH_1 | praat_DEPRECATED_2006, NEW1_Table_createWithoutColumnNames);
	praat_addMenuCommand (U"Objects", U"New", U"Create TableOfReal...", nullptr, 1, NEW1_TableOfReal_create);

	praat_addMenuCommand (U"Objects", U"Open", U"Read TableOfReal from headerless spreadsheet file...", nullptr, 0, READ1_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from tab-separated file...", nullptr, 0, READ1_Table_readFromTabSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from comma-separated file...", nullptr, 0, READ1_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from semicolon-separated file...", nullptr, 0, READ1_Table_readFromSemicolonSeparatedFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Table from whitespace-separated file...", nullptr, 0, READ1_Table_readFromTableFile);
	praat_addMenuCommand (U"Objects", U"Open",   U"Read Table from table file...", U"*Read Table from whitespace-separated file...", praat_DEPRECATED_2011, READ1_Table_readFromTableFile);

	praat_addAction1 (classDistributions, 0, U"Distributions help", nullptr, 0, HELP_Distributions_help);
	praat_TableOfReal_init (classDistributions);
	praat_addAction1 (classDistributions, 1, U"Get probability (label)...", U"Get value...", 1, REAL_Distributions_getProbability);
	praat_addAction1 (classDistributions, 0, U"-- get from two --", U"Get probability (label)...", 1, nullptr);
	praat_addAction1 (classDistributions, 2, U"Get mean absolute difference...", U"-- get from two --", 1, REAL_Distributionses_getMeanAbsoluteDifference);
	praat_addAction1 (classDistributions, 0, U"-- add --", U"Append", 1, nullptr);
	praat_addAction1 (classDistributions, 0, U"Add", U"-- add --", 1, NEW1_Distributionses_add);
	praat_addAction1 (classDistributions, 0, U"Generate", nullptr, 0, nullptr);   // FIXME no hyphen?
		praat_addAction1 (classDistributions, 0, U"To Strings...", nullptr, 0, NEW_Distributions_to_Strings);
		praat_addAction1 (classDistributions, 0, U"To Strings (exact)...", nullptr, 0, NEW_Distributions_to_Strings_exact);

	praat_addAction1 (classLogisticRegression, 0, U"Draw boundary...", nullptr, 0, GRAPHICS_LogisticRegression_drawBoundary);

	praat_addAction1 (classPairDistribution, 0, U"PairDistribution help", nullptr, 0, HELP_PairDistribution_help);
	praat_addAction1 (classPairDistribution, 0, U"To Table", nullptr, 0, NEW_PairDistribution_to_Table);
	praat_addAction1 (classPairDistribution, 1, U"To Stringses...", nullptr, 0, NEW2_PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get number of pairs", nullptr, 1, INTEGER_PairDistribution_getNumberOfPairs);
		praat_addAction1 (classPairDistribution, 1, U"Get string1...", nullptr, 1, STRING_PairDistribution_getString1);
		praat_addAction1 (classPairDistribution, 1, U"Get string2...", nullptr, 1, STRING_PairDistribution_getString2);
		praat_addAction1 (classPairDistribution, 1, U"Get weight...", nullptr, 1, REAL_PairDistribution_getWeight);
		praat_addAction1 (classPairDistribution, 1, U"-- get fraction correct --", nullptr, 1, nullptr);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (maximum likelihood)", nullptr, 1, REAL_PairDistribution_getFractionCorrect_maximumLikelihood);
		praat_addAction1 (classPairDistribution, 1, U"Get fraction correct (probability matching)", nullptr, 1, REAL_PairDistribution_getFractionCorrect_probabilityMatching);
	praat_addAction1 (classPairDistribution, 0, U"Modify -", nullptr, 0, nullptr);
	praat_addAction1 (classPairDistribution, 0, U"Remove zero weights", nullptr, 1, MODIFY_PairDistribution_removeZeroWeights);
	praat_addAction1 (classPairDistribution, 0, U"Swap inputs and outputs", nullptr, 1, MODIFY_PairDistribution_swapInputsAndOutputs);

	praat_addAction1 (classTable, 0, U"Table help", nullptr, 0, HELP_Table_help);
	praat_addAction1 (classTable, 1, U"Save as tab-separated file...", nullptr, 0, SAVE_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as table file...", nullptr, praat_DEPRECATED_2011, SAVE_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1,   U"Write to table file...", nullptr, praat_DEPRECATED_2011, SAVE_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as comma-separated file...", nullptr, 0, SAVE_Table_writeToCommaSeparatedFile);
	praat_addAction1 (classTable, 1, U"Save as semicolon-separated file...", nullptr, 0, SAVE_Table_writeToSemicolonSeparatedFile);
	praat_addAction1 (classTable, 1, U"View & Edit", nullptr, praat_ATTRACTIVE | praat_NO_API, WINDOW_Table_viewAndEdit);
	praat_addAction1 (classTable, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011 | praat_NO_API, WINDOW_Table_viewAndEdit);
	praat_addAction1 (classTable, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Scatter plot...", nullptr, 1, GRAPHICS_Table_scatterPlot);
		praat_addAction1 (classTable, 0, U"Scatter plot (mark)...", nullptr, 1, GRAPHICS_Table_scatterPlot_mark);
		praat_addAction1 (classTable, 0, U"Draw ellipse (standard deviation)...", nullptr, 1, GRAPHICS_Table_drawEllipse);
	praat_addAction1 (classTable, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"List...", nullptr, 1, LIST_Table_list);
		praat_addAction1 (classTable, 1, U"-- get structure --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get number of rows", nullptr, 1, INTEGER_Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, U"Get number of columns", nullptr, 1, INTEGER_Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, U"Get column label...", nullptr, 1, STRING_Table_getColumnLabel);
		praat_addAction1 (classTable, 1, U"Get column index...", nullptr, 1, INTEGER_Table_getColumnIndex);
		praat_addAction1 (classTable, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get value...", nullptr, 1, STRING_Table_getValue);
		praat_addAction1 (classTable, 1, U"Search column...", nullptr, 1, INTEGER_Table_searchColumn);
		praat_addAction1 (classTable, 1, U"-- statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Statistics tutorial", nullptr, 1, HELP_StatisticsTutorial);
		praat_addAction1 (classTable, 1, U"-- get stats --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Get quantile...", nullptr, 1, REAL_Table_getQuantile);
		praat_addAction1 (classTable, 1, U"Get minimum...", nullptr, 1, REAL_Table_getMinimum);
		praat_addAction1 (classTable, 1, U"Get maximum...", nullptr, 1, REAL_Table_getMaximum);
		praat_addAction1 (classTable, 1, U"Get mean...", nullptr, 1, REAL_Table_getMean);
		praat_addAction1 (classTable, 1, U"Get group mean...", nullptr, 1, REAL_Table_getGroupMean);
		praat_addAction1 (classTable, 1, U"Get standard deviation...", nullptr, 1, REAL_Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, U"-- report stats --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 1, U"Report mean (Student t)...", nullptr, 1, INFO_Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, U"Report standard deviation...", nullptr, 1, DO_Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, U"Report difference (Student t)...", nullptr, 1, INFO_Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group mean (Student t)...", nullptr, 1, INFO_Table_reportGroupMean_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Student t)...", nullptr, 1, INFO_Table_reportGroupDifference_studentT);
		praat_addAction1 (classTable, 1, U"Report group difference (Wilcoxon rank sum)...", nullptr, 1, INFO_Table_reportGroupDifference_wilcoxonRankSum);
		praat_addAction1 (classTable, 1, U"Report correlation (Pearson r)...", nullptr, 1, INFO_Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, U"Report correlation (Kendall tau)...", nullptr, 1, INFO_Table_reportCorrelation_kendallTau);
	praat_addAction1 (classTable, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Set string value...", nullptr, 1, MODIFY_Table_setStringValue);
		praat_addAction1 (classTable, 0, U"Set numeric value...", nullptr, 1, MODIFY_Table_setNumericValue);
		praat_addAction1 (classTable, 0, U"Formula...", nullptr, 1, MODIFY_Table_formula);
		praat_addAction1 (classTable, 0, U"Formula (column range)...", nullptr, 1, MODIFY_Table_formula_columnRange);
		praat_addAction1 (classTable, 0, U"Sort rows...", nullptr, 1, MODIFY_Table_sortRows);
		praat_addAction1 (classTable, 0, U"Randomize rows", nullptr, 1, MODIFY_Table_randomizeRows);
		praat_addAction1 (classTable, 0, U"Reflect rows", nullptr, 1, MODIFY_Table_reflectRows);
		praat_addAction1 (classTable, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Append row", nullptr, 1, MODIFY_Table_appendRow);
		praat_addAction1 (classTable, 0, U"Append column...", nullptr, 1, MODIFY_Table_appendColumn);
		praat_addAction1 (classTable, 0, U"Append sum column...", nullptr, 1, MODIFY_Table_appendSumColumn);
		praat_addAction1 (classTable, 0, U"Append difference column...", nullptr, 1, MODIFY_Table_appendDifferenceColumn);
		praat_addAction1 (classTable, 0, U"Append product column...", nullptr, 1, MODIFY_Table_appendProductColumn);
		praat_addAction1 (classTable, 0, U"Append quotient column...", nullptr, 1, MODIFY_Table_appendQuotientColumn);
		praat_addAction1 (classTable, 0, U"Remove row...", nullptr, 1, MODIFY_Table_removeRow);
		praat_addAction1 (classTable, 0, U"Remove column...", nullptr, 1, MODIFY_Table_removeColumn);
		praat_addAction1 (classTable, 0, U"Insert row...", nullptr, 1, MODIFY_Table_insertRow);
		praat_addAction1 (classTable, 0, U"Insert column...", nullptr, 1, MODIFY_Table_insertColumn);
		praat_addAction1 (classTable, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (classTable, 0, U"Set column label (index)...", nullptr, 1, MODIFY_Table_setColumnLabel_index);
		praat_addAction1 (classTable, 0, U"Set column label (label)...", nullptr, 1, MODIFY_Table_setColumnLabel_label);
	praat_addAction1 (classTable, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"To linear regression", nullptr, 1, NEW_Table_to_LinearRegression);
		praat_addAction1 (classTable, 0, U"To logistic regression...", nullptr, 1, NEW_Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Append", nullptr, 1, NEW1_Tables_append);
	praat_addAction1 (classTable, 0, U"Generate -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 1, U"Draw row from distribution...", nullptr, 1, INTEGER_Table_drawRowFromDistribution);
	praat_addAction1 (classTable, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (classTable, 0, U"Extract rows where column (number)...", nullptr, 1, NEW_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0,   U"Extract rows where column...", U"*Extract rows where column (number)...", praat_DEPTH_1 | praat_DEPRECATED_2006, NEW_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0,   U"Select rows where column...", U"*Extract rows where column (number)...", praat_DEPTH_1 | praat_DEPRECATED_2004, NEW_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, U"Extract rows where column (text)...", nullptr, 1, NEW_Table_extractRowsWhereColumn_text);
		praat_addAction1 (classTable, 0, U"Transpose", nullptr, 1, NEW_Table_transpose);
		praat_addAction1 (classTable, 0, U"Collapse rows...", nullptr, 1, NEW_Table_collapseRows);
		praat_addAction1 (classTable, 0, U"Rows to columns...", nullptr, 1, NEW_Table_rowsToColumns);
	praat_addAction1 (classTable, 0, U"Down to TableOfReal...", nullptr, 0, NEW_Table_downto_TableOfReal);

	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0, REAL_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.cpp */
