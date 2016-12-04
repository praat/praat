/* praat_Stat.cpp
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

#include "Distributions_and_Strings.h"
#include "LogisticRegression.h"
#include "Matrix.h"
#include "PairDistribution.h"
#include "Table.h"
#include "TableEditor.h"
#include "UnicodeData.h"

#include "praat_TableOfReal.h"

#undef iam
#define iam iam_LOOP

static const char32 * Table_messageColumn (Table me, long column) {
	if (my columnHeaders [column]. label && my columnHeaders [column]. label [0] != U'\0')
		return Melder_cat (U"\"", my columnHeaders [column]. label, U"\"");
	else
		return Melder_integer (column);
}

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
	NATURAL4 (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_COUPLE (Distributions)
		double result = Distributionses_getMeanAbsoluteDifference (me, you, columnNumber);
	NUMBER_COUPLE_END (U" (mean absolute difference between columns ", columnNumber, U")")
}

FORM (REAL_Distributions_getProbability, U"Get probability", nullptr) {
	NATURAL4 (columnNumber, U"Column number", U"1")
	SENTENCE4 (string, U"String", U"")
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
	NATURAL4 (columnNumber, U"Column number", U"1")
	NATURAL4 (numberOfStrings, U"Number of strings", U"1000")
	OK
DO
	CONVERT_EACH (Distributions)
		autoStrings result = Distributions_to_Strings (me, columnNumber, numberOfStrings);
	CONVERT_EACH_END (my name)
}

FORM (NEW_Distributions_to_Strings_exact, U"To Strings (exact)", nullptr) {
	NATURAL4 (columnNumber, U"Column number", U"1")
	OK
DO
	CONVERT_EACH (Distributions)
		autoStrings result = Distributions_to_Strings_exact (me, columnNumber);
	CONVERT_EACH_END (my name)
}

// MARK: - LOGISTICREGRESSION

// MARK: Draw

FORM (GRAPHICS_LogisticRegression_drawBoundary, U"LogisticRegression: Draw boundary", nullptr) {
	WORD4 (horizontalFactor, U"Horizontal factor", U"")
	REAL4 (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL4 (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	WORD4 (verticalFactor, U"Vertical factor", U"")
	REAL4 (fromVertical, U"left Vertical range", U"0.0")
	REAL4 (toVertical, U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LogisticRegression)
		long xfactor = Regression_getFactorIndexFromFactorName_e (me, horizontalFactor);
		long yfactor = Regression_getFactorIndexFromFactorName_e (me, verticalFactor);
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, fromHorizontal, toHorizontal,
			yfactor, fromVertical, toVertical,
			garnish);
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
		long result = my pairs.size;
	NUMBER_ONE_END (U" pairs")
}

FORM (STRING_PairDistribution_getString1, U"Get string1", nullptr) {
	NATURAL4 (pairNumber, U"Pair number", U"1")
	OK
DO
	STRING_ONE (PairDistribution)
		const char32 *result = PairDistribution_getString1 (me, pairNumber);
	STRING_ONE_END
}

FORM (STRING_PairDistribution_getString2, U"Get string2", nullptr) {
	NATURAL4 (pairNumber, U"Pair number", U"1")
	OK
DO
	STRING_ONE (PairDistribution)
		const char32 *result = PairDistribution_getString2 (me, pairNumber);
	STRING_ONE_END
}

FORM (REAL_PairDistribution_getWeight, U"Get weight", nullptr) {
	NATURAL4 (pairNumber, U"Pair number", U"1")
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
	NATURAL4 (number, U"Number", U"1000")
	SENTENCE4 (nameOfFirstStrings, U"Name of first Strings", U"input")
	SENTENCE4 (nameOfSecondStrings, U"Name of second Strings", U"output")
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
	LOOP {
		iam (PairDistribution);
		autoTable thee = PairDistribution_to_Table (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - PAIRDISTRIBUTION & DISTRIBUTIONS

FORM (REAL_PairDistribution_Distributions_getFractionCorrect, U"PairDistribution & Distributions: Get fraction correct", nullptr) {
	NATURAL4 (columnNumber, U"Column number", U"1")
	OK
DO
	NUMBER_TWO (PairDistribution, Distributions)
		double result = PairDistribution_Distributions_getFractionCorrect (me, you, columnNumber);
	NUMBER_TWO_END (U" (fraction correct)")
}

// MARK: - TABLE

// MARK: New

FORM (NEW1_Table_createWithColumnNames, U"Create Table with column names", nullptr) {
	WORD4 (name, U"Name", U"table")
	INTEGER4 (numberOfRows, U"Number of rows", U"10")
	LABEL (U"", U"Column names:")
	TEXTFIELD4 (columnNames, U"columnNames", U"speaker dialect age vowel F0 F1 F2")
	OK
DO
	CREATE_ONE
		autoTable result = Table_createWithColumnNames (numberOfRows, columnNames);
	CREATE_ONE_END (name)
}

FORM (NEW1_Table_createWithoutColumnNames, U"Create Table without column names", nullptr) {
	WORD4 (name, U"Name", U"table")
	INTEGER4 (numberOfRows, U"Number of rows", U"10")
	NATURAL4 (numberOfColumns, U"Number of columns", U"3")
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
		autoTable result = Table_readFromCharacterSeparatedTextFile (file, U',');
	READ_ONE_END
}

FORM_READ (READ1_Table_readFromTabSeparatedFile, U"Read Table from tab-separated file", nullptr, true) {
	READ_ONE
		autoTable result = Table_readFromCharacterSeparatedTextFile (file, U'\t');
	READ_ONE_END
}

// MARK: Save

FORM_SAVE (SAVE_Table_writeToCommaSeparatedFile, U"Save Table as comma-separated file", 0, U"Table") {
	SAVE_ONE (Table)
		Table_writeToCommaSeparatedFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Table_writeToTabSeparatedFile, U"Save Table as tab-separated file", 0, U"Table") {
	SAVE_ONE (Table)
		Table_writeToTabSeparatedFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_StatisticsTutorial) {
	HELP (U"Statistics")
}

// MARK: View & Edit

DIRECT (WINDOW_Table_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a Table from batch.");
	LOOP {
		iam (Table);
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: Tabulate

FORM (LIST_Table_list, U"Table: List", nullptr) {
	BOOLEAN4 (includeRowNumbers, U"Include row numbers", true)
	OK
DO
	INFO_ONE (Table)
		Table_list (me, includeRowNumbers);
	INFO_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_Table_scatterPlot, U"Scatter plot", nullptr) {
	WORD4 (horizontalColumn, U"Horizontal column", U"")
	REAL4 (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL4 (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	WORD4 (verticalColumn, U"Vertical column", U"")
	REAL4 (fromVertical, U"left Vertical range", U"0.0")
	REAL4 (toVertical, U"right Vertical range", U"0.0 (= auto)")
	WORD4 (columnWithMarks, U"Column with marks", U"")
	NATURAL4 (fontSize, U"Font size", U"12")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		long markColumn = Table_getColumnIndexFromColumnLabel (me, columnWithMarks);
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
			fromHorizontal, toHorizontal, fromVertical, toVertical, markColumn, fontSize, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_scatterPlot_mark, U"Scatter plot (marks)", nullptr) {
	WORD4 (horizontalColumn, U"Horizontal column", U"")
	REAL4 (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL4 (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	WORD4 (verticalColumn, U"Vertical column", U"")
	REAL4 (fromVertical, U"left Vertical range", U"0.0")
	REAL4 (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE4 (markSize, U"Mark size (mm)", U"1.0")
	BOOLEAN4 (garnish, U"Garnish", true)
	SENTENCE4 (markString, U"Mark string (+xo.)", U"+")
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			fromHorizontal, toHorizontal, fromVertical, toVertical,
			markSize, markString, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Table_drawEllipse, U"Draw ellipse (standard deviation)", nullptr) {
	WORD4 (horizontalColumn, U"Horizontal column", U"")
	REAL4 (fromHorizontal, U"left Horizontal range", U"0.0")
	REAL4 (toHorizontal, U"right Horizontal range", U"0.0 (= auto)")
	WORD4 (verticalColumn, U"Vertical column", U"")
	REAL4 (fromVertical, U"left Vertical range", U"0.0")
	REAL4 (toVertical, U"right Vertical range", U"0.0 (= auto)")
	POSITIVE4 (numberOfSigmas, U"Number of sigmas", U"2.0")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Table)
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, horizontalColumn);
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, verticalColumn);
		Table_drawEllipse_e (me, GRAPHICS, xcolumn, ycolumn,
			fromHorizontal, toHorizontal, fromVertical, toVertical, numberOfSigmas, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (INTEGER_Table_drawRowFromDistribution, U"Table: Draw row from distribution", nullptr) {
	WORD4 (columnWithDistribution, U"Column with distribution", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnWithDistribution);
		long result = Table_drawRowFromDistribution (me, columnNumber);
	NUMBER_ONE_END (U" (random row number)")
}

FORM (INTEGER_Table_getColumnIndex, U"Table: Get column index", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long result = Table_findColumnIndexFromColumnLabel (me, columnLabel);
	NUMBER_ONE_END (U" (index of column ", columnLabel, U")")
}

FORM (STRING_Table_getColumnLabel, U"Table: Get column label", nullptr) {
	NATURAL4 (columnNumber, U"Column number", U"1")
	OK
DO
	STRING_ONE (Table)
		if (columnNumber > my numberOfColumns)
			Melder_throw (U"Your column number should not be greater than the number of columns.");
		const char32 *result = my columnHeaders [columnNumber]. label;
	STRING_ONE_END
}

FORM (REAL_Table_getGroupMean, U"Table: Get group mean", nullptr) {
	WORD4 (columnLabel, U"Column label", U"salary")
	WORD4 (groupColumnLabel, U"Group column label", U"gender")
	SENTENCE4 (group, U"Group", U"F")
	OK
DO
	NUMBER_ONE (Table)
		long column = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, groupColumnLabel);
		double result = Table_getGroupMean (me, column, groupColumn, group);
	NUMBER_ONE_END (U" (mean of ", columnLabel, U" in group ", group, U")")
}

FORM (REAL_Table_getMaximum, U"Table: Get maximum", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getMaximum (me, columnNumber);
	NUMBER_ONE_END (U" (maximum of ", columnLabel, U")")
}

FORM (REAL_Table_getMean, U"Table: Get mean", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getMean (me, columnNumber);
	NUMBER_ONE_END (U" (mean of ", columnLabel, U")")
}

FORM (REAL_Table_getMinimum, U"Table: Get minimum", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getMinimum (me, columnNumber);
	NUMBER_ONE_END (U" (minimum of ", columnLabel, U")")
}

FORM (REAL_Table_getQuantile, U"Table: Get quantile", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	POSITIVE4 (quantile, U"Quantile", U"0.50 (= median)")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getQuantile (me, columnNumber, quantile);
	NUMBER_ONE_END (U" (", quantile, U" quantile of ", columnLabel, U")")
}

FORM (REAL_Table_getStandardDeviation, U"Table: Get standard deviation", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		double result = Table_getStdev (me, columnNumber);
	NUMBER_ONE_END (U" (standard deviation of ", columnLabel, U")")
}

DIRECT (INTEGER_Table_getNumberOfColumns) {
	NUMBER_ONE (Table)
		long result = my numberOfColumns;
	NUMBER_ONE_END (U" columns")
}

DIRECT (INTEGER_Table_getNumberOfRows) {
	NUMBER_ONE (Table)
		long result = my rows.size;
	NUMBER_ONE_END (U" rows")
}

FORM (STRING_Table_getValue, U"Table: Get value", nullptr) {
	NATURAL4 (rowNumber, U"Row number", U"1")
	SENTENCE4 (columnLabel, U"Column label", U"")
	OK
DO
	STRING_ONE (Table)
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		const char32 *result = my rows.at [rowNumber] -> cells [columnNumber]. string;
	STRING_ONE_END
}

FORM (INTEGER_Table_searchColumn, U"Table: Search column", nullptr) {
	SENTENCE4 (columnLabel, U"Column label", U"")
	SENTENCE4 (value, U"Value", U"")
	OK
DO
	NUMBER_ONE (Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		long result = Table_searchColumn (me, columnNumber, value);
	NUMBER_ONE_END (U" (first row in which ", columnLabel, U" is ", value)
}
	
// MARK: Statistics

FORM (INFO_Table_reportCorrelation_kendallTau, U"Report correlation (Kendall tau)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

FORM (INFO_Table_reportCorrelation_pearsonR, U"Report correlation (Pearson r)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
		MelderInfo_writeLine (U"Number of degrees of freedom = ", my rows.size - 2);
		MelderInfo_writeLine (U"Significance from zero = ", significance, U" (one-tailed)");
		MelderInfo_writeLine (U"Confidence interval (", 100.0 * (1.0 - 2.0 * unconfidence), U"%):");
		MelderInfo_writeLine (U"   Lower limit = ", lowerLimit,
			U" (lowest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_writeLine (U"   Upper limit = ", upperLimit,
			U" (highest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", unconfidence, U")");
		MelderInfo_close ();
	INFO_ONE_END
}

FORM (INFO_Table_reportDifference_studentT, U"Report difference (Student t)", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

FORM (INFO_Table_reportGroupDifference_studentT, U"Report group difference (Student t)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group 1", U"F")
	SENTENCE (U"Group 2", U"M")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

FORM (INFO_Table_reportGroupDifference_wilcoxonRankSum, U"Report group difference (Wilcoxon rank sum)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group 1", U"F")
	SENTENCE (U"Group 2", U"M")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

FORM (INFO_Table_reportGroupMean_studentT, U"Report group mean (Student t)", nullptr) {
	WORD (U"Column", U"salary")
	WORD (U"Group column", U"gender")
	SENTENCE (U"Group", U"F")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

FORM (INFO_Table_reportMean_studentT, U"Report mean (Student t)", nullptr) {
	WORD (U"Column", U"")
	POSITIVE (U"One-tailed unconfidence", U"0.025")
	OK
DO
	INFO_ONE (Table)
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
	INFO_ONE_END
}

// MARK: Modify

FORM (MODIFY_Table_appendColumn, U"Table: Append column", nullptr) {
	WORD (U"Label", U"newcolumn")
	OK
DO
	LOOP {
		iam (Table);
		Table_appendColumn (me, GET_STRING (U"Label"));
		praat_dataChanged (OBJECT);
	}
END }

FORM (MODIFY_Table_appendDifferenceColumn, U"Table: Append difference column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendDifferenceColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_appendProductColumn, U"Table: Append product column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendProductColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_appendQuotientColumn, U"Table: Append quotient column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendQuotientColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_appendSumColumn, U"Table: Append sum column", nullptr) {
	WORD (U"left Columns", U"")
	WORD (U"right Columns", U"")
	WORD (U"Label", U"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"right Columns"));
		Table_appendSumColumn (me, icol, jcol, GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Table_appendRow) {
	LOOP {
		iam (Table);
		Table_appendRow (me);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_formula, U"Table: Formula", U"Table: Formula...") {
	WORD (U"Column label", U"")
	TEXTFIELD (U"formula", U"abs (self)")
	OK
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
END }

FORM (MODIFY_Table_formula_columnRange, U"Table: Formula (column range)", U"Table: Formula...") {
	WORD (U"From column label", U"")
	WORD (U"To column label", U"")
	TEXTFIELD (U"formula", U"log10 (self)")
	OK
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
END }

FORM (MODIFY_Table_insertColumn, U"Table: Insert column", nullptr) {
	NATURAL (U"Position", U"1")
	WORD (U"Label", U"newcolumn")
	OK
DO
	LOOP {
		iam (Table);
		Table_insertColumn (me, GET_INTEGER (U"Position"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_insertRow, U"Table: Insert row", nullptr) {
	NATURAL (U"Position", U"1")
	OK
DO
	LOOP {
		iam (Table);
		Table_insertRow (me, GET_INTEGER (U"Position"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_removeColumn, U"Table: Remove column", nullptr) {
	WORD (U"Column label", U"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_removeColumn (me, icol);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_removeRow, U"Table: Remove row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK
DO
	LOOP {
		iam (Table);
		Table_removeRow (me, GET_INTEGER (U"Row number"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_setColumnLabel_index, U"Set column label", nullptr) {
	NATURAL (U"Column number", U"1")
	SENTENCE (U"Label", U"")
	OK
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, GET_INTEGER (U"Column number"), GET_STRING (U"Label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_setColumnLabel_label, U"Set column label", nullptr) {
	SENTENCE (U"Old label", U"")
	SENTENCE (U"New label", U"")
	OK
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Old label")), GET_STRING (U"New label"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_setNumericValue, U"Table: Set numeric value", nullptr) {
	NATURAL (U"Row number", U"1")
	WORD (U"Column label", U"")
	REAL_OR_UNDEFINED (U"Numeric value", U"1.5")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_setNumericValue (me, GET_INTEGER (U"Row number"), icol, GET_REAL (U"Numeric value"));
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_setStringValue, U"Table: Set string value", nullptr) {
	NATURAL (U"Row number", U"1")
	WORD (U"Column label", U"")
	SENTENCE (U"String value", U"xx")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Column label"));
		Table_setStringValue (me, GET_INTEGER (U"Row number"), icol, GET_STRING (U"String value"));
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Table_randomizeRows) {
	LOOP {
		iam (Table);
		Table_randomizeRows (me);
		praat_dataChanged (me);
	}
END }

DIRECT (MODIFY_Table_reflectRows) {
	LOOP {
		iam (Table);
		Table_reflectRows (me);
		praat_dataChanged (me);
	}
END }

FORM (MODIFY_Table_sortRows, U"Table: Sort rows", nullptr) {
	LABEL (U"", U"One or more column labels for sorting:")
	TEXTFIELD (U"columnLabels", U"dialect gender name")
	OK
DO
	LOOP {
		iam (Table);
		Table_sortRows_string (me, GET_STRING (U"columnLabels"));
		praat_dataChanged (me);
	}
END }

// MARK: Convert

FORM (NEW_Table_collapseRows, U"Table: Collapse rows", nullptr) {
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
	OK
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_collapseRows (me,
			GET_STRING (U"factors"), GET_STRING (U"columnsToSum"),
			GET_STRING (U"columnsToAverage"), GET_STRING (U"columnsToMedianize"),
			GET_STRING (U"columnsToAverageLogarithmically"), GET_STRING (U"columnsToMedianizeLogarithmically"));
		praat_new (thee.move(), my name, U"_pooled");
	}
END }

DIRECT (NEW1_Tables_append) {
	OrderedOf<structTable> list;
	LOOP {
		iam (Table);
		list. addItem_ref (me);
	}
	autoTable thee = Tables_append (& list);
	praat_new (thee.move(), U"appended");
END }

FORM (NEW_Table_extractRowsWhereColumn_number, U"Table: Extract rows where column (number)", nullptr) {
	WORD (U"Extract all rows where column...", U"")
	RADIO_ENUM (U"...is...", kMelder_number, DEFAULT)
	REAL (U"...the number", U"0.0")
	OK
DO
	double value = GET_REAL (U"...the number");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_number (me, icol, GET_ENUM (kMelder_number, U"...is..."), value);
		praat_new (thee.move(), my name, U"_", Table_messageColumn (static_cast <Table> OBJECT, icol), U"_", NUMdefined (value) ? Melder_integer (lround (value)) : U"undefined");
		praat_dataChanged (me);   // WHY?
	}
END }

FORM (NEW_Table_extractRowsWhereColumn_text, U"Table: Extract rows where column (text)", nullptr) {
	WORD (U"Extract all rows where column...", U"")
	OPTIONMENU_ENUM (U"...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK
DO
	const char32 *value = GET_STRING (U"...the text");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (U"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_string (me, icol, GET_ENUM (kMelder_string, U"..."), value);
		praat_new (thee.move(), my name, U"_", value);
		praat_dataChanged (me);   // WHY?
	}
END }

DIRECT (NEW_Table_transpose) {
	LOOP {
		iam (Table);
		autoTable thee = Table_transpose (me);
		praat_new (thee.move(), NAME, U"_transposed");
	}
END }

FORM (NEW_Table_rowsToColumns, U"Table: Rows to columns", nullptr) {
	LABEL (U"", U"Columns with factors (independent variables):")
	TEXTFIELD (U"factors", U"dialect gender speaker")
	WORD (U"Column to transpose", U"vowel")
	LABEL (U"", U"Columns to expand:")
	TEXTFIELD (U"columnsToExpand", U"duration F0 F1 F2 F3")
	LABEL (U"", U"Columns not mentioned above will be ignored.")
	OK
DO
	const char32 *columnLabel = GET_STRING (U"Column to transpose");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoTable thee = Table_rowsToColumns (me, GET_STRING (U"factors"), icol, GET_STRING (U"columnsToExpand"));
		praat_new (thee.move(), NAME, U"_nested");
	}
END }

DIRECT (NEW_Table_to_LinearRegression) {
	LOOP {
		iam (Table);
		autoLinearRegression thee = Table_to_LinearRegression (me);
		praat_new (thee.move(), NAME);
	}
END }

FORM (NEW_Table_to_LogisticRegression, U"Table: To LogisticRegression", nullptr) {
	LABEL (U"", U"Factors (column names):")
	TEXTFIELD (U"factors", U"F0 F1 duration")
	WORD (U"Dependent 1 (column name)", U"e")
	WORD (U"Dependent 2 (column name)", U"i")
	OK
DO
	LOOP {
		iam (Table);
		autoLogisticRegression thee = Table_to_LogisticRegression (me, GET_STRING (U"factors"), GET_STRING (U"Dependent 1"), GET_STRING (U"Dependent 2"));
		praat_new (thee.move(), NAME);
	}
END }

FORM (NEW_Table_to_TableOfReal, U"Table: Down to TableOfReal", nullptr) {
	WORD (U"Column for row labels", U"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_findColumnIndexFromColumnLabel (me, GET_STRING (U"Column for row labels"));
		autoTableOfReal thee = Table_to_TableOfReal (me, icol);
		praat_new (thee.move(), NAME);
	}
END }

FORM (NEW1_TableOfReal_create, U"Create TableOfReal", nullptr) {
	WORD (U"Name", U"table")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"3")
	OK
DO
	autoTableOfReal me = TableOfReal_create (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	praat_new (me.move(), GET_STRING (U"Name"));
END }

FORM_READ (READ1_TableOfReal_readFromHeaderlessSpreadsheetFile, U"Read TableOfReal from headerless spreadsheet file", nullptr, true) {
	READ_ONE
		autoTableOfReal result = TableOfReal_readFromHeaderlessSpreadsheetFile (file);
	READ_ONE_END
}

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
	praat_addAction1 (classTable, 0, U"Down to TableOfReal...", nullptr, 0, NEW_Table_to_TableOfReal);

	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0, REAL_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.cpp */
