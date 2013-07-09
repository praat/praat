/* praat_Stat.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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

static wchar_t formatBuffer [32] [40];
static int formatIndex = 0;
static wchar_t * Table_messageColumn (Table me, long column) {
	if (++ formatIndex == 32) formatIndex = 0;
	if (my columnHeaders [column]. label != NULL && my columnHeaders [column]. label [0] != '\0')
		swprintf (formatBuffer [formatIndex], 40, L"\"%.39ls\"", my columnHeaders [column]. label);
	else
		swprintf (formatBuffer [formatIndex], 40, L"%ld", column);
	return formatBuffer [formatIndex];
}

/***** DISTRIBUTIONS *****/

DIRECT (Distributionses_add)
	autoCollection me = praat_getSelectedObjects ();
	autoDistributions thee = Distributions_addMany (me.peek());
	praat_new (thee.transfer(), L"added");
END

FORM (Distributionses_getMeanAbsoluteDifference, L"Get mean difference", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Distributions me = NULL, thee = NULL;
	LOOP {
		(me ? thee : me) = (Distributions) OBJECT;
	}
	Melder_informationReal (Distributionses_getMeanAbsoluteDifference (me, thee, GET_INTEGER (L"Column number")), NULL);
END

FORM (Distributions_getProbability, L"Get probability", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"String", L"")
	OK
DO
	LOOP {
		iam (Distributions);
		double probability = Distributions_getProbability (me, GET_STRING (L"String"), GET_INTEGER (L"Column number"));
		Melder_informationReal (probability, NULL);
	}
END

DIRECT (Distributions_help)
	Melder_help (L"Distributions");
END

FORM (Distributions_to_Strings, L"To Strings", 0)
	NATURAL (L"Column number", L"1")
	NATURAL (L"Number of strings", L"1000")
	OK
DO
	LOOP {
		iam (Distributions);
		autoStrings thee = Distributions_to_Strings (me, GET_INTEGER (L"Column number"), GET_INTEGER (L"Number of strings"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Distributions_to_Strings_exact, L"To Strings (exact)", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (Distributions);
		autoStrings thee = Distributions_to_Strings_exact (me, GET_INTEGER (L"Column number"));
		praat_new (thee.transfer(), my name);
	}
END

/***** LOGISTICREGRESSION *****/

FORM (LogisticRegression_drawBoundary, L"LogisticRegression: Draw boundary", 0)
	WORD (L"Horizontal factor", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical factor", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (LogisticRegression);
		long xfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (L"Horizontal factor"));
		long yfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (L"Vertical factor"));
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			yfactor, GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Garnish"));
	}
END

/***** PAIRDISTRIBUTION *****/

DIRECT (PairDistribution_getFractionCorrect_maximumLikelihood)
	LOOP {
		iam (PairDistribution);
		double fractionCorrect = PairDistribution_getFractionCorrect_maximumLikelihood (me);
		Melder_informationReal (fractionCorrect, NULL);
	}
END

DIRECT (PairDistribution_getFractionCorrect_probabilityMatching)
	LOOP {
		iam (PairDistribution);
		double fractionCorrect = PairDistribution_getFractionCorrect_probabilityMatching (me);
		Melder_informationReal (fractionCorrect, NULL);
	}
END

DIRECT (PairDistribution_getNumberOfPairs)
	LOOP {
		iam (PairDistribution);
		Melder_information (Melder_integer (my pairs -> size));
	}
END

FORM (PairDistribution_getString1, L"Get string1", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	LOOP {
		iam (PairDistribution);
		const wchar_t *string1 = PairDistribution_getString1 (me, GET_INTEGER (L"Pair number"));
		Melder_information (string1);
	}
END

FORM (PairDistribution_getString2, L"Get string2", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	LOOP {
		iam (PairDistribution);
		const wchar_t *string2 = PairDistribution_getString2 (me, GET_INTEGER (L"Pair number"));
		Melder_information (string2);
	}
END

FORM (PairDistribution_getWeight, L"Get weight", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	LOOP {
		iam (PairDistribution);
		double weight = PairDistribution_getWeight (me, GET_INTEGER (L"Pair number"));
		Melder_information (Melder_double (weight));
	}
END

DIRECT (PairDistribution_help) Melder_help (L"PairDistribution"); END

DIRECT (PairDistribution_removeZeroWeights)
	LOOP {
		iam (PairDistribution);
		PairDistribution_removeZeroWeights (me);
		praat_dataChanged (me);
	}
END

DIRECT (PairDistribution_swapInputsAndOutputs)
	LOOP {
		iam (PairDistribution);
		my f_swapInputsAndOutputs ();
		praat_dataChanged (me);
	}
END

FORM (PairDistribution_to_Stringses, L"Generate two Strings objects", 0)
	NATURAL (L"Number", L"1000")
	SENTENCE (L"Name of first Strings", L"input")
	SENTENCE (L"Name of second Strings", L"output")
	OK
DO
	LOOP {
		iam (PairDistribution);
		Strings strings1_, strings2_;
		PairDistribution_to_Stringses (me, GET_INTEGER (L"Number"), & strings1_, & strings2_);
		autoStrings strings1 = strings1_, strings2 = strings2_;   // UGLY
		praat_new (strings1.transfer(), GET_STRING (L"Name of first Strings"));
		praat_new (strings2.transfer(), GET_STRING (L"Name of second Strings"));
	}
END

DIRECT (PairDistribution_to_Table)
	LOOP {
		iam (PairDistribution);
		autoTable thee = PairDistribution_to_Table (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** PAIRDISTRIBUTION & DISTRIBUTIONS *****/

FORM (PairDistribution_Distributions_getFractionCorrect, L"PairDistribution & Distributions: Get fraction correct", 0)
	NATURAL (L"Column", L"1")
	OK
DO
	PairDistribution me = NULL;
	Distributions thee = NULL;
	LOOP {
		if (CLASS == classPairDistribution) me = (PairDistribution) OBJECT;
		if (CLASS == classDistributions) thee = (Distributions) OBJECT;
	}
	double fractionCorrect = PairDistribution_Distributions_getFractionCorrect (me, thee, GET_INTEGER (L"Column"));
	Melder_informationReal (fractionCorrect, NULL);
END

/***** TABLE *****/

DIRECT (Tables_append)
	autoCollection collection = Collection_create (classTable, 10);
	Collection_dontOwnItems (collection.peek());
	LOOP {
		iam (Table);
		Collection_addItem (collection.peek(), me);
	}
	autoTable thee = Tables_append (collection.peek());
	praat_new (thee.transfer(), L"appended");
END

FORM (Table_appendColumn, L"Table: Append column", 0)
	WORD (L"Label", L"newcolumn")
	OK
DO
	LOOP {
		iam (Table);
		Table_appendColumn (me, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_appendDifferenceColumn, L"Table: Append difference column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		Table_appendDifferenceColumn (me, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (Table_appendProductColumn, L"Table: Append product column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		Table_appendProductColumn (me, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (Table_appendQuotientColumn, L"Table: Append quotient column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		Table_appendQuotientColumn (me, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (Table_appendSumColumn, L"Table: Append sum column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		Table_appendSumColumn (me, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

DIRECT (Table_appendRow)
	LOOP {
		iam (Table);
		Table_appendRow (me);
		praat_dataChanged (me);
	}
END

FORM (Table_collapseRows, L"Table: Collapse rows", 0)
	LABEL (L"", L"Columns with factors (independent variables):")
	TEXTFIELD (L"factors", L"speaker dialect age vowel")
	LABEL (L"", L"Columns to sum:")
	TEXTFIELD (L"columnsToSum", L"number cost")
	LABEL (L"", L"Columns to average:")
	TEXTFIELD (L"columnsToAverage", L"price")
	LABEL (L"", L"Columns to medianize:")
	TEXTFIELD (L"columnsToMedianize", L"vot")
	LABEL (L"", L"Columns to average logarithmically:")
	TEXTFIELD (L"columnsToAverageLogarithmically", L"duration")
	LABEL (L"", L"Columns to medianize logarithmically:")
	TEXTFIELD (L"columnsToMedianizeLogarithmically", L"F0 F1 F2 F3")
	LABEL (L"", L"Columns not mentioned above will be ignored.")
	OK
DO
	LOOP {
		iam (Table);
		autoTable thee = Table_collapseRows (me,
			GET_STRING (L"factors"), GET_STRING (L"columnsToSum"),
			GET_STRING (L"columnsToAverage"), GET_STRING (L"columnsToMedianize"),
			GET_STRING (L"columnsToAverageLogarithmically"), GET_STRING (L"columnsToMedianizeLogarithmically"));
		praat_new (thee.transfer(), my name, L"_pooled");
	}
END

FORM (Table_createWithColumnNames, L"Create Table with column names", 0)
	WORD (L"Name", L"table")
	INTEGER (L"Number of rows", L"10")
	LABEL (L"", L"Column names:")
	TEXTFIELD (L"columnNames", L"speaker dialect age vowel F0 F1 F2")
	OK
DO
	autoTable me = Table_createWithColumnNames (GET_INTEGER (L"Number of rows"), GET_STRING (L"columnNames"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Table_createWithoutColumnNames, L"Create Table without column names", 0)
	WORD (L"Name", L"table")
	INTEGER (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"3")
	OK
DO
	autoTable me = Table_createWithoutColumnNames (GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Table_drawEllipse, L"Draw ellipse (standard deviation)", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	POSITIVE (L"Number of sigmas", L"2.0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		Table_drawEllipse_e (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_drawRowFromDistribution, L"Table: Draw row from distribution", 0)
	WORD (L"Column with distribution", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with distribution"));
		long row = Table_drawRowFromDistribution (me, icol);
		Melder_information (Melder_integer (row));
	}
END

DIRECT (Table_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot edit a Table from batch.");
	LOOP {
		iam (Table);
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (Table_extractRowsWhereColumn_number, L"Table: Extract rows where column (number)", 0)
	WORD (L"Extract all rows where column...", L"")
	RADIO_ENUM (L"...is...", kMelder_number, DEFAULT)
	REAL (L"...the number", L"0.0")
	OK
DO
	double value = GET_REAL (L"...the number");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_number (me, icol, GET_ENUM (kMelder_number, L"...is..."), value);
		praat_new (thee.transfer(), my name, L"_", Table_messageColumn (static_cast <Table> OBJECT, icol), L"_", NUMdefined (value) ? Melder_integer ((long) round (value)) : L"undefined");
		praat_dataChanged (me);   // WHY?
	}
END

FORM (Table_extractRowsWhereColumn_text, L"Table: Extract rows where column (text)", 0)
	WORD (L"Extract all rows where column...", L"")
	OPTIONMENU_ENUM (L"...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	const wchar_t *value = GET_STRING (L"...the text");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Extract all rows where column..."));
		autoTable thee = Table_extractRowsWhereColumn_string (me, icol, GET_ENUM (kMelder_string, L"..."), value);
		praat_new (thee.transfer(), my name, L"_", value);
		praat_dataChanged (me);   // WHY?
	}
END

FORM (Table_formula, L"Table: Formula", L"Table: Formula...")
	WORD (L"Column label", L"")
	TEXTFIELD (L"formula", L"abs (self)")
	OK
DO
	LOOP {
		iam (Table);
		try {
			long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
			Table_formula (me, icol, GET_STRING (L"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Table may have partially changed
			throw;
		}
	}
END

FORM (Table_formula_columnRange, L"Table: Formula (column range)", L"Table: Formula...")
	WORD (L"From column label", L"")
	WORD (L"To column label", L"")
	TEXTFIELD (L"formula", L"log10 (self)")
	OK
DO
	LOOP {
		iam (Table);
		try {
			long icol1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"From column label"));
			long icol2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"To column label"));
			Table_formula_columnRange (me, icol1, icol2, GET_STRING (L"formula"), interpreter);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Table may have partially changed
			throw;
		}
	}
END

FORM (Table_getColumnIndex, L"Table: Get column index", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		Melder_information (Melder_integer (Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"))));
	}
END

FORM (Table_getColumnLabel, L"Table: Get column label", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (Table);
		long icol = GET_INTEGER (L"Column number");
		if (icol > my numberOfColumns) Melder_throw ("Column number must not be greater than number of columns.");
		Melder_information (my columnHeaders [icol]. label);
	}
END

FORM (Table_getGroupMean, L"Table: Get group mean", 0)
	WORD (L"Column label", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group", L"F")
	OK
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column"));
		Melder_information (Melder_double (Table_getGroupMean (static_cast <Table> ONLY_OBJECT, column, groupColumn, GET_STRING (L"Group"))));
	}
END

FORM (Table_getMaximum, L"Table: Get maximum", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double maximum = Table_getMaximum (me, icol);
		Melder_information (Melder_double (maximum));
	}
END

FORM (Table_getMean, L"Table: Get mean", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double mean = Table_getMean (me, icol);
		Melder_information (Melder_double (mean));
	}
END

FORM (Table_getMinimum, L"Table: Get minimum", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double minimum = Table_getMinimum (me, icol);
		Melder_information (Melder_double (minimum));
	}
END

FORM (Table_getQuantile, L"Table: Get quantile", 0)
	SENTENCE (L"Column label", L"")
	POSITIVE (L"Quantile", L"0.50 (= median)")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double quantile = Table_getQuantile (me, icol, GET_REAL (L"Quantile"));
		Melder_information (Melder_double (quantile));
	}
END

FORM (Table_getStandardDeviation, L"Table: Get standard deviation", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		double stdev = Table_getStdev (me, icol);
		Melder_information (Melder_double (stdev));
	}
END

DIRECT (Table_getNumberOfColumns)
	LOOP {
		iam (Table);
		Melder_information (Melder_integer (my numberOfColumns));
	}
END

DIRECT (Table_getNumberOfRows)
	LOOP {
		iam (Table);
		Melder_information (Melder_integer (my rows -> size));
	}
END

FORM (Table_getValue, L"Table: Get value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long rowNumber = GET_INTEGER (L"Row number");
		Table_checkSpecifiedRowNumberWithinRange (me, rowNumber);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		Melder_information (((TableRow) my rows -> item [rowNumber]) -> cells [icol]. string);
	}
END

DIRECT (Table_help) Melder_help (L"Table"); END

FORM (Table_insertColumn, L"Table: Insert column", 0)
	NATURAL (L"Position", L"1")
	WORD (L"Label", L"newcolumn")
	OK
DO
	LOOP {
		iam (Table);
		Table_insertColumn (me, GET_INTEGER (L"Position"), GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (Table_insertRow, L"Table: Insert row", 0)
	NATURAL (L"Position", L"1")
	OK
DO
	LOOP {
		iam (Table);
		Table_insertRow (me, GET_INTEGER (L"Position"));
		praat_dataChanged (me);
	}
END

FORM (Table_list, L"Table: List", 0)
	BOOLEAN (L"Include row numbers", true)
	OK
DO
	LOOP {
		iam (Table);
		Table_list (me, GET_INTEGER (L"Include row numbers"));
	}
END

FORM_READ (Table_readFromTableFile, L"Read Table from table file", 0, true)
	praat_newWithFile (Table_readFromTableFile (file), MelderFile_name (file), file);
END

FORM_READ (Table_readFromCommaSeparatedFile, L"Read Table from comma-separated file", 0, true)
	praat_newWithFile (Table_readFromCharacterSeparatedTextFile (file, ','), MelderFile_name (file), file);
END

FORM_READ (Table_readFromTabSeparatedFile, L"Read Table from tab-separated file", 0, true)
	praat_newWithFile (Table_readFromCharacterSeparatedTextFile (file, '\t'), MelderFile_name (file), file);
END

FORM (Table_removeColumn, L"Table: Remove column", 0)
	WORD (L"Column label", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		Table_removeColumn (me, icol);
		praat_dataChanged (me);
	}
END

FORM (Table_removeRow, L"Table: Remove row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	LOOP {
		iam (Table);
		Table_removeRow (me, GET_INTEGER (L"Row number"));
		praat_dataChanged (me);
	}
END

FORM (Table_reportCorrelation_kendallTau, L"Report correlation (Kendall tau)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_kendallTau (me, column1, column2, unconfidence,
			& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Correlation between column ", Table_messageColumn (me, column1),
			L" and column ", Table_messageColumn (me, column2), L":");
		MelderInfo_writeLine (L"Correlation = ", Melder_double (correlation), L" (Kendall's tau-b)");
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END

FORM (Table_reportCorrelation_pearsonR, L"Report correlation (Pearson r)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		double correlation, significance, lowerLimit, upperLimit;
		correlation = Table_getCorrelation_pearsonR (me, column1, column2, unconfidence,
			& significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Correlation between column ", Table_messageColumn (me, column1),
			L" and column ", Table_messageColumn (me, column2), L":");
		MelderInfo_writeLine (L"Correlation = ", Melder_double (correlation), L" (Pearson's r)");
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_integer (my rows -> size - 2));
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END
	
FORM (Table_reportDifference_studentT, L"Report difference (Student t)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns"));
		long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		double difference, t, numberOfDegreesOfFreedom, significance, lowerLimit, upperLimit;
		difference = Table_getDifference_studentT (me, column1, column2, unconfidence,
			& t, & numberOfDegreesOfFreedom, & significance, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Difference between column ", Table_messageColumn (me, column1),
			L" and column ", Table_messageColumn (me, column2), L":");
		MelderInfo_writeLine (L"Difference = ", Melder_double (difference));
		MelderInfo_writeLine (L"Student's t = ", Melder_double (t));
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END
	
FORM (Table_reportGroupDifference_studentT, L"Report group difference (Student t)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group 1", L"F")
	SENTENCE (L"Group 2", L"M")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		wchar_t *group1 = GET_STRING (L"Group 1"), *group2 = GET_STRING (L"Group 2");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupDifference_studentT (me, column, groupColumn, group1, group2, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (L"Difference in column ", Table_messageColumn (me, column), L" between groups ", group1);
		MelderInfo_writeLine (L" and ", group2, L" of column ", Table_messageColumn (me, groupColumn), L":");
		MelderInfo_writeLine (L"Difference = ", Melder_double (mean));
		MelderInfo_writeLine (L"Student's t = ", Melder_double (tFromZero));
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END

FORM (Table_reportGroupDifference_wilcoxonRankSum, L"Report group difference (Wilcoxon rank sum)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group 1", L"F")
	SENTENCE (L"Group 2", L"M")
	OK
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column"));
		wchar_t *group1 = GET_STRING (L"Group 1"), *group2 = GET_STRING (L"Group 2");
		double areaUnderCurve, rankSum, significanceFromZero;
		areaUnderCurve = Table_getGroupDifference_wilcoxonRankSum (me, column, groupColumn, group1, group2,
			& rankSum, & significanceFromZero);
		MelderInfo_open ();
		MelderInfo_write (L"Difference in column ", Table_messageColumn (me, column), L" between groups ", group1);
		MelderInfo_writeLine (L" and ", group2, L" of column ", Table_messageColumn (me, groupColumn), L":");
		MelderInfo_writeLine (L"Larger: ", areaUnderCurve < 0.5 ? group1 : areaUnderCurve > 0.5 ? group2 : L"(both equal)");
		MelderInfo_writeLine (L"Area under curve: ", Melder_double (areaUnderCurve));
		MelderInfo_writeLine (L"Rank sum: ", Melder_double (rankSum));
		MelderInfo_writeLine (L"Significance from zero: ", Melder_double (significanceFromZero), L" (one-tailed)");
		MelderInfo_close ();
	}
END

FORM (Table_reportGroupMean_studentT, L"Report group mean (Student t)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group", L"F")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		wchar_t *group = GET_STRING (L"Group");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getGroupMean_studentT (me, column, groupColumn, group, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_write (L"Mean in column ", Table_messageColumn (me, column), L" of group ", group);
		MelderInfo_writeLine (L" of column ", Table_messageColumn (me, groupColumn), L":");
		MelderInfo_writeLine (L"Mean = ", Melder_double (mean));
		MelderInfo_writeLine (L"Student's t from zero = ", Melder_double (tFromZero));
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END

FORM (Table_reportMean_studentT, L"Report mean (Student t)", 0)
	WORD (L"Column", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	LOOP {
		iam (Table);
		long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column"));
		double unconfidence = GET_REAL (L"One-tailed unconfidence");
		double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
		mean = Table_getMean_studentT (me, column, unconfidence,
			& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
		MelderInfo_open ();
		MelderInfo_writeLine (L"Mean of column ", Table_messageColumn (me, column), L":");
		MelderInfo_writeLine (L"Mean = ", Melder_double (mean));
		MelderInfo_writeLine (L"Student's t from zero = ", Melder_double (tFromZero));
		MelderInfo_writeLine (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
		MelderInfo_writeLine (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
		MelderInfo_writeLine (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
		MelderInfo_writeLine (L"   Lower limit = ", Melder_double (lowerLimit),
			L" (lowest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_writeLine (L"   Upper limit = ", Melder_double (upperLimit),
			L" (highest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
		MelderInfo_close ();
	}
END

FORM (Table_rowsToColumns, L"Table: Rows to columns", 0)
	LABEL (L"", L"Columns with factors (independent variables):")
	TEXTFIELD (L"factors", L"dialect gender speaker")
	WORD (L"Column to transpose", L"vowel")
	LABEL (L"", L"Columns to expand:")
	TEXTFIELD (L"columnsToExpand", L"duration F0 F1 F2 F3")
	LABEL (L"", L"Columns not mentioned above will be ignored.")
	OK
DO
	const wchar_t *columnLabel = GET_STRING (L"Column to transpose");
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel);
		autoTable thee = Table_rowsToColumns (me, GET_STRING (L"factors"), icol, GET_STRING (L"columnsToExpand"));
		praat_new (thee.transfer(), NAME, L"_nested");
	}
END

FORM (Table_scatterPlot, L"Scatter plot", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	WORD (L"Column with marks", L"")
	NATURAL (L"Font size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		long markColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with marks"));
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			markColumn, GET_INTEGER (L"Font size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_scatterPlot_mark, L"Scatter plot (marks)", 0)
	WORD (L"Horizontal column", L"")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0 (= auto)")
	WORD (L"Vertical column", L"")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	POSITIVE (L"Mark size (mm)", L"1.0")
	BOOLEAN (L"Garnish", 1)
	SENTENCE (L"Mark string (+xo.)", L"+")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Table);
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column"));
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column"));
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Table_searchColumn, L"Table: Search column", 0)
	WORD (L"Column label", L"")
	WORD (L"Value", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		Melder_information (Melder_integer (Table_searchColumn (me, icol, GET_STRING (L"Value"))));
	}
END
	
FORM (Table_setColumnLabel_index, L"Set column label", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, GET_INTEGER (L"Column number"), GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (Table_setColumnLabel_label, L"Set column label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	LOOP {
		iam (Table);
		Table_setColumnLabel (me, Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Old label")), GET_STRING (L"New label"));
		praat_dataChanged (me);
	}
END

FORM (Table_setNumericValue, L"Table: Set numeric value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	REAL_OR_UNDEFINED (L"Numeric value", L"1.5")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		Table_setNumericValue (me, GET_INTEGER (L"Row number"), icol, GET_REAL (L"Numeric value"));
		praat_dataChanged (me);
	}
END

FORM (Table_setStringValue, L"Table: Set string value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	SENTENCE (L"String value", L"xx")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label"));
		Table_setStringValue (me, GET_INTEGER (L"Row number"), icol, GET_STRING (L"String value"));
		praat_dataChanged (me);
	}
END

DIRECT (Table_randomizeRows)
	LOOP {
		iam (Table);
		Table_randomizeRows (me);
		praat_dataChanged (me);
	}
END

DIRECT (Table_reflectRows)
	LOOP {
		iam (Table);
		Table_reflectRows (me);
		praat_dataChanged (me);
	}
END

FORM (Table_sortRows, L"Table: Sort rows", 0)
	LABEL (L"", L"One or more column labels for sorting:")
	TEXTFIELD (L"columnLabels", L"dialect gender name")
	OK
DO
	LOOP {
		iam (Table);
		Table_sortRows_string (me, GET_STRING (L"columnLabels"));
		praat_dataChanged (me);
	}
END

DIRECT (Table_to_LinearRegression)
	LOOP {
		iam (Table);
		autoLinearRegression thee = Table_to_LinearRegression (me);
		praat_new (thee.transfer(), NAME);
	}
END

FORM (Table_to_LogisticRegression, L"Table: To LogisticRegression", 0)
	LABEL (L"", L"Factors (column names):")
	TEXTFIELD (L"factors", L"F0 F1 duration")
	WORD (L"Dependent 1 (column name)", L"e")
	WORD (L"Dependent 2 (column name)", L"i")
	OK
DO
	LOOP {
		iam (Table);
		autoLogisticRegression thee = Table_to_LogisticRegression (me, GET_STRING (L"factors"), GET_STRING (L"Dependent 1"), GET_STRING (L"Dependent 2"));
		praat_new (thee.transfer(), NAME);
	}
END

FORM (Table_to_TableOfReal, L"Table: Down to TableOfReal", 0)
	WORD (L"Column for row labels", L"")
	OK
DO
	LOOP {
		iam (Table);
		long icol = Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Column for row labels"));
		autoTableOfReal thee = Table_to_TableOfReal (me, icol);
		praat_new (thee.transfer(), NAME);
	}
END

DIRECT (Table_transpose)
	LOOP {
		iam (Table);
		autoTable thee = Table_transpose (me);
		praat_new (thee.transfer(), NAME, L"_transposed");
	}
END

FORM_WRITE (Table_writeToCommaSeparatedFile, L"Save Table as comma-separated file", 0, L"Table")
	LOOP {
		iam (Table);
		Table_writeToCommaSeparatedFile (me, file);
	}
END

FORM_WRITE (Table_writeToTabSeparatedFile, L"Save Table as tab-separated file", 0, L"Table")
	LOOP {
		iam (Table);
		Table_writeToTabSeparatedFile (me, file);
	}
END

/***** TABLEOFREAL *****/

DIRECT (TablesOfReal_append)
	autoCollection tables = Collection_create (classTableOfReal, 10);
	Collection_dontOwnItems (tables.peek());
	LOOP {
		iam (TableOfReal);
		Collection_addItem (tables.peek(), me);
	}
	autoTableOfReal thee = static_cast <TableOfReal> (TablesOfReal_appendMany (tables.peek()));
	praat_new (thee.transfer(), L"appended");
END

FORM (TableOfReal_create, L"Create TableOfReal", 0)
	WORD (L"Name", L"table")
	NATURAL (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"3")
	OK
DO
	autoTableOfReal me = TableOfReal_create (GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (TableOfReal_drawAsNumbers, L"Draw as numbers", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	RADIO (L"Format", 3)
		RADIOBUTTON (L"decimal")
		RADIOBUTTON (L"exponential")
		RADIOBUTTON (L"free")
		RADIOBUTTON (L"rational")
	NATURAL (L"Precision", L"5")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsNumbers (me, GRAPHICS,
			GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
			GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"));
	}
END

FORM (TableOfReal_drawAsNumbers_if, L"Draw as numbers if...", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	RADIO (L"Format", 3)
		RADIOBUTTON (L"decimal")
		RADIOBUTTON (L"exponential")
		RADIOBUTTON (L"free")
		RADIOBUTTON (L"rational")
	NATURAL (L"Precision", L"5")
	LABEL (L"", L"Condition:")
	TEXTFIELD (L"condition", L"self <> 0")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsNumbers_if (me, GRAPHICS,
			GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
			GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"), GET_STRING (L"condition"), interpreter);
	}
END

FORM (TableOfReal_drawAsSquares, L"Draw table as squares", 0)
	INTEGER (L"From row", L"1")
	INTEGER (L"To row", L"0")
	INTEGER (L"From column", L"1")
	INTEGER (L"To column", L"0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawAsSquares (me, GRAPHICS, 
			GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
			GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
			GET_INTEGER (L"Garnish"));
		}
END

FORM (TableOfReal_drawHorizontalLines, L"Draw horizontal lines", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawHorizontalLines (me, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"));
	}
END

FORM (TableOfReal_drawLeftAndRightLines, L"Draw left and right lines", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawLeftAndRightLines (me, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"));
	}
END

FORM (TableOfReal_drawTopAndBottomLines, L"Draw top and bottom lines", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawTopAndBottomLines (me, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"));
	}
END

FORM (TableOfReal_drawVerticalLines, L"Draw vertical lines", 0)
	NATURAL (L"From row", L"1")
	INTEGER (L"To row", L"0 (= all)")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TableOfReal);
		TableOfReal_drawVerticalLines (me, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"));
	}
END

DIRECT (TableOfReal_extractColumnLabelsAsStrings)
	LOOP {
		iam (TableOfReal);
		autoStrings thee = TableOfReal_extractColumnLabelsAsStrings (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (TableOfReal_extractColumnRanges, L"Extract column ranges", 0)
	LABEL (L"", L"Create a new TableOfReal from the following columns:")
	TEXTFIELD (L"ranges", L"1 2")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnRanges (me, GET_STRING (L"ranges"));
		praat_new (thee.transfer(), my name, L"_cols");
	}
END

FORM (TableOfReal_extractColumnsWhere, L"Extract columns where", 0)
	LABEL (L"", L"Extract all columns with at least one cell where:")
	TEXTFIELD (L"condition", L"col mod 3 = 0 ; this example extracts every third column")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhere (me, GET_STRING (L"condition"), interpreter);
		praat_new (thee.transfer(), my name, L"_cols");
	}
END

FORM (TableOfReal_extractColumnsWhereLabel, L"Extract column where label", 0)
	OPTIONMENU_ENUM (L"Extract all columns whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"a")
	OK
DO
	const wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhereLabel (me, GET_ENUM (kMelder_string, L"Extract all columns whose label..."), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TableOfReal_extractColumnsWhereRow, L"Extract columns where row", 0)
	NATURAL (L"Extract all columns where row...", L"1")
	OPTIONMENU_ENUM (L"...is...", kMelder_number, DEFAULT)
	REAL (L"...the value", L"0.0")
	OK
DO
	long row = GET_INTEGER (L"Extract all columns where row...");
	double value = GET_REAL (L"...the value");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractColumnsWhereRow (me, row, GET_ENUM (kMelder_number, L"...is..."), value);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (row), L"_", Melder_integer (round (value)));
	}
END

DIRECT (TableOfReal_extractRowLabelsAsStrings)
	LOOP {
		iam (TableOfReal);
		autoStrings thee = TableOfReal_extractRowLabelsAsStrings (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (TableOfReal_extractRowRanges, L"Extract row ranges", 0)
	LABEL (L"", L"Create a new TableOfReal from the following rows:")
	TEXTFIELD (L"ranges", L"1 2")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowRanges (me, GET_STRING (L"ranges"));
		praat_new (thee.transfer(), my name, L"_rows");
	}
END

FORM (TableOfReal_extractRowsWhere, L"Extract rows where", 0)
	LABEL (L"", L"Extract all rows with at least one cell where:")
	TEXTFIELD (L"condition", L"row mod 3 = 0 ; this example extracts every third row")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhere (me, GET_STRING (L"condition"), interpreter);
		praat_new (thee.transfer(), my name, L"_rows");
	}
END

FORM (TableOfReal_extractRowsWhereColumn, L"Extract rows where column", 0)
	NATURAL (L"Extract all rows where column...", L"1")
	OPTIONMENU_ENUM (L"...is...", kMelder_number, DEFAULT)
	REAL (L"...the value", L"0.0")
	OK
DO
	long column = GET_INTEGER (L"Extract all rows where column...");
	double value = GET_REAL (L"...the value");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhereColumn (me,
			column, GET_ENUM (kMelder_number, L"...is..."), value);
		praat_new (thee.transfer(), my name, L"_", Melder_integer (column), L"_", Melder_integer (round (value)));
	}
END

FORM (TableOfReal_extractRowsWhereLabel, L"Extract rows where label", 0)
	OPTIONMENU_ENUM (L"Extract all rows whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"a")
	OK
DO
	const wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TableOfReal);
		autoTableOfReal thee = TableOfReal_extractRowsWhereLabel (me, GET_ENUM (kMelder_string, L"Extract all rows whose label..."), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TableOfReal_formula, L"TableOfReal: Formula", L"Formula...")
	LABEL (L"", L"for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	TEXTFIELD (L"formula", L"if col = 5 then self + self [6] else self fi")
	OK
DO
	LOOP {
		iam (TableOfReal);
		try {
			TableOfReal_formula (me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (TableOfReal_getColumnIndex, L"Get column index", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (L"Column label"));
		Melder_information (Melder_integer (columnNumber));
	}
END
	
FORM (TableOfReal_getColumnLabel, L"Get column label", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (L"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, ": column number must not be greater than number of columns.");
		Melder_information (my columnLabels == NULL ? L"" : my columnLabels [columnNumber]);
	}
END
	
FORM (TableOfReal_getColumnMean_index, L"Get column mean", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (L"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, ": column number must not be greater than number of columns.");
		double columnMean = TableOfReal_getColumnMean (me, columnNumber);
		Melder_informationReal (columnMean, NULL);
	}
END
	
FORM (TableOfReal_getColumnMean_label, L"Get column mean", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (L"Column label"));
		if (columnNumber == 0) Melder_throw (me, ": column label does not exist.");
		double columnMean = TableOfReal_getColumnMean (me, columnNumber);
		Melder_informationReal (columnMean, NULL);
	}
END
	
FORM (TableOfReal_getColumnStdev_index, L"Get column standard deviation", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = GET_INTEGER (L"Column number");
		if (columnNumber > my numberOfColumns) Melder_throw (me, ": column number must not be greater than number of columns.");
		double stdev = TableOfReal_getColumnStdev (me, columnNumber);
		Melder_informationReal (stdev, NULL);
	}
END
	
FORM (TableOfReal_getColumnStdev_label, L"Get column standard deviation", 0)
	SENTENCE (L"Column label", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (L"Column label"));
		if (columnNumber == 0) Melder_throw (me, ": column label does not exist.");
		double stdev = TableOfReal_getColumnStdev (me, columnNumber);
		Melder_informationReal (stdev, NULL);
	}
END

DIRECT (TableOfReal_getNumberOfColumns)
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_integer (my numberOfColumns));
	}
END

DIRECT (TableOfReal_getNumberOfRows)
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_integer (my numberOfRows));
	}
END

FORM (TableOfReal_getRowIndex, L"Get row index", 0)
	SENTENCE (L"Row label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = TableOfReal_rowLabelToIndex (me, GET_STRING (L"Row label"));
		Melder_information (Melder_integer (rowNumber));
	}
END
	
FORM (TableOfReal_getRowLabel, L"Get row label", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (L"Row number");
		if (rowNumber > my numberOfRows) Melder_throw (me, ": row number must not be greater than number of rows.");
		Melder_information (my rowLabels == NULL ? L"" : my rowLabels [rowNumber]);
	}
END

FORM (TableOfReal_getValue, L"Get value", 0)
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (L"Row number"), columnNumber = GET_INTEGER (L"Column number");
		if (rowNumber > my numberOfRows) Melder_throw (me, ": row number must not exceed number of rows.");
		if (columnNumber > my numberOfColumns) Melder_throw (me, ": column number must not exceed number of columns.");
		Melder_informationReal (my data [rowNumber] [columnNumber], NULL);
	}
END

DIRECT (TableOfReal_help) Melder_help (L"TableOfReal"); END

FORM (TableOfReal_insertColumn, L"Insert column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_insertColumn (me, GET_INTEGER (L"Column number"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_insertRow, L"Insert row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_insertRow (me, GET_INTEGER (L"Row number"));
		praat_dataChanged (me);
	}
END

FORM_READ (TableOfReal_readFromHeaderlessSpreadsheetFile, L"Read TableOfReal from headerless spreadsheet file", 0, true)
	praat_newWithFile (TableOfReal_readFromHeaderlessSpreadsheetFile (file), MelderFile_name (file), file);
END

FORM (TableOfReal_removeColumn, L"Remove column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_removeColumn (me, GET_INTEGER (L"Column number"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_removeRow, L"Remove row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_removeRow (me, GET_INTEGER (L"Row number"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setColumnLabel_index, L"Set column label", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setColumnLabel (me, GET_INTEGER (L"Column number"), GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setColumnLabel_label, L"Set column label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long columnNumber = TableOfReal_columnLabelToIndex (me, GET_STRING (L"Old label"));
		TableOfReal_setColumnLabel (me, columnNumber, GET_STRING (L"New label"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setRowLabel_index, L"Set row label", 0)
	NATURAL (L"Row number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_setRowLabel (me, GET_INTEGER (L"Row number"), GET_STRING (L"Label"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setValue, L"Set value", L"TableOfReal: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL_OR_UNDEFINED (L"New value", L"0.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = GET_INTEGER (L"Row number"), columnNumber = GET_INTEGER (L"Column number");
		if (rowNumber > my numberOfRows) Melder_throw (me, ": row number too large.");
		if (columnNumber > my numberOfColumns) Melder_throw (me, ": column number too large.");
		my data [rowNumber] [columnNumber] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setRowLabel_label, L"Set row label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	LOOP {
		iam (TableOfReal);
		long rowNumber = TableOfReal_rowLabelToIndex (me, GET_STRING (L"Old label"));
		TableOfReal_setRowLabel (me, rowNumber, GET_STRING (L"New label"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_sortByColumn, L"Sort rows by column", 0)
	INTEGER (L"Column", L"1")
	INTEGER (L"Secondary column", L"0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_sortByColumn (me, GET_INTEGER (L"Column"), GET_INTEGER (L"Secondary column"));
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_sortByLabel, L"Sort rows by label", 0)
	LABEL (L"", L"Secondary sorting keys:")
	INTEGER (L"Column1", L"1")
	INTEGER (L"Column2", L"0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_sortByLabel (me, GET_INTEGER (L"Column1"), GET_INTEGER (L"Column2"));
		praat_dataChanged (me);
	}
END

DIRECT (TableOfReal_to_Matrix)
	LOOP {
		iam (TableOfReal);
		autoMatrix thee = TableOfReal_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (TableOfReal_to_Table, L"TableOfReal: To Table", 0)
	SENTENCE (L"Label of first column", L"rowLabel")
	OK
DO
	LOOP {
		iam (TableOfReal);
		autoTable thee = TableOfReal_to_Table (me, GET_STRING (L"Label of first column"));
		praat_new (thee.transfer(), my name);
	}
END

FORM_WRITE (TableOfReal_writeToHeaderlessSpreadsheetFile, L"Save TableOfReal as spreadsheet", 0, L"txt")
	LOOP {
		iam (TableOfReal);
		TableOfReal_writeToHeaderlessSpreadsheetFile (me, file);
	}
END


DIRECT (StatisticsTutorial) Melder_help (L"Statistics"); END

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

static Any tabSeparatedFileRecognizer (int nread, const char *header, MelderFile file) {
	/*
	 * A table is recognized if it has at least one tab symbol,
	 * which must be before the first newline symbol (if any).
	 */
	unsigned char *uheader = (unsigned char *) header;
	bool isTabSeparated =
		uheader [0] == 0xef && uheader [1] == 0xff ? isTabSeparated_utf16be (nread, header) :
		uheader [0] == 0xff && uheader [1] == 0xef ? isTabSeparated_utf16le (nread, header) :
		isTabSeparated_8bit (nread, header);
	if (! isTabSeparated) return NULL;
	return Table_readFromCharacterSeparatedTextFile (file, '\t');
}

void praat_TableOfReal_init (ClassInfo klas);   /* Buttons for TableOfReal and for its subclasses. */
void praat_TableOfReal_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"Save as headerless spreadsheet file...", 0, 0, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 1, L"Write to headerless spreadsheet file...", 0, praat_HIDDEN, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"Draw as numbers...", 0, 1, DO_TableOfReal_drawAsNumbers);
		praat_addAction1 (klas, 0, L"Draw as numbers if...", 0, 1, DO_TableOfReal_drawAsNumbers_if);
		praat_addAction1 (klas, 0, L"Draw as squares...", 0, 1, DO_TableOfReal_drawAsSquares);	
		praat_addAction1 (klas, 0, L"-- draw lines --", 0, 1, 0);
		praat_addAction1 (klas, 0, L"Draw vertical lines...", 0, 1, DO_TableOfReal_drawVerticalLines);
		praat_addAction1 (klas, 0, L"Draw horizontal lines...", 0, 1, DO_TableOfReal_drawHorizontalLines);
		praat_addAction1 (klas, 0, L"Draw left and right lines...", 0, 1, DO_TableOfReal_drawLeftAndRightLines);
		praat_addAction1 (klas, 0, L"Draw top and bottom lines...", 0, 1, DO_TableOfReal_drawTopAndBottomLines);
	praat_addAction1 (klas, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (klas, 1, L"Get number of rows", 0, 1, DO_TableOfReal_getNumberOfRows);
		praat_addAction1 (klas, 1, L"Get number of columns", 0, 1, DO_TableOfReal_getNumberOfColumns);
		praat_addAction1 (klas, 1, L"Get row label...", 0, 1, DO_TableOfReal_getRowLabel);
		praat_addAction1 (klas, 1, L"Get column label...", 0, 1, DO_TableOfReal_getColumnLabel);
		praat_addAction1 (klas, 1, L"Get row index...", 0, 1, DO_TableOfReal_getRowIndex);
		praat_addAction1 (klas, 1, L"Get column index...", 0, 1, DO_TableOfReal_getColumnIndex);
		praat_addAction1 (klas, 1, L"-- get value --", 0, 1, 0);
		praat_addAction1 (klas, 1, L"Get value...", 0, 1, DO_TableOfReal_getValue);
		if (klas == classTableOfReal) {
			praat_addAction1 (klas, 1, L"-- get statistics --", 0, 1, 0);
			praat_addAction1 (klas, 1, L"Get column mean (index)...", 0, 1, DO_TableOfReal_getColumnMean_index);
			praat_addAction1 (klas, 1, L"Get column mean (label)...", 0, 1, DO_TableOfReal_getColumnMean_label);
			praat_addAction1 (klas, 1, L"Get column stdev (index)...", 0, 1, DO_TableOfReal_getColumnStdev_index);
			praat_addAction1 (klas, 1, L"Get column stdev (label)...", 0, 1, DO_TableOfReal_getColumnStdev_label);
		}
	praat_addAction1 (klas, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"Formula...", 0, 1, DO_TableOfReal_formula);
		praat_addAction1 (klas, 0, L"Set value...", 0, 1, DO_TableOfReal_setValue);
		praat_addAction1 (klas, 0, L"Sort by label...", 0, 1, DO_TableOfReal_sortByLabel);
		praat_addAction1 (klas, 0, L"Sort by column...", 0, 1, DO_TableOfReal_sortByColumn);
		praat_addAction1 (klas, 0, L"-- structure --", 0, 1, 0);
		praat_addAction1 (klas, 0, L"Remove row (index)...", 0, 1, DO_TableOfReal_removeRow);
		praat_addAction1 (klas, 0, L"Remove column (index)...", 0, 1, DO_TableOfReal_removeColumn);
		praat_addAction1 (klas, 0, L"Insert row (index)...", 0, 1, DO_TableOfReal_insertRow);
		praat_addAction1 (klas, 0, L"Insert column (index)...", 0, 1, DO_TableOfReal_insertColumn);
		praat_addAction1 (klas, 0, L"-- set --", 0, 1, 0);
		praat_addAction1 (klas, 0, L"Set row label (index)...", 0, 1, DO_TableOfReal_setRowLabel_index);
		praat_addAction1 (klas, 0, L"Set row label (label)...", 0, 1, DO_TableOfReal_setRowLabel_label);
		praat_addAction1 (klas, 0, L"Set column label (index)...", 0, 1, DO_TableOfReal_setColumnLabel_index);
		praat_addAction1 (klas, 0, L"Set column label (label)...", 0, 1, DO_TableOfReal_setColumnLabel_label);
	praat_addAction1 (klas, 0, L"Synthesize -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"Append", 0, 1, DO_TablesOfReal_append);
	praat_addAction1 (klas, 0, L"Extract part -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"Extract row ranges...", 0, 1, DO_TableOfReal_extractRowRanges);
		praat_addAction1 (klas, 0, L"Extract rows where column...", 0, 1, DO_TableOfReal_extractRowsWhereColumn);
		praat_addAction1 (klas, 0, L"Extract rows where label...", 0, 1, DO_TableOfReal_extractRowsWhereLabel);
		praat_addAction1 (klas, 0, L"Extract rows where...", 0, 1, DO_TableOfReal_extractRowsWhere);
		praat_addAction1 (klas, 0, L"Extract column ranges...", 0, 1, DO_TableOfReal_extractColumnRanges);
		praat_addAction1 (klas, 0, L"Extract columns where row...", 0, 1, DO_TableOfReal_extractColumnsWhereRow);
		praat_addAction1 (klas, 0, L"Extract columns where label...", 0, 1, DO_TableOfReal_extractColumnsWhereLabel);
		praat_addAction1 (klas, 0, L"Extract columns where...", 0, 1, DO_TableOfReal_extractColumnsWhere);
	praat_addAction1 (klas, 0, L"Extract -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"Extract row labels as Strings", 0, 1, DO_TableOfReal_extractRowLabelsAsStrings);
		praat_addAction1 (klas, 0, L"Extract column labels as Strings", 0, 1, DO_TableOfReal_extractColumnLabelsAsStrings);
	praat_addAction1 (klas, 0, L"Convert -", 0, 0, 0);
		praat_addAction1 (klas, 0, L"To Table...", 0, 1, DO_TableOfReal_to_Table);
		praat_addAction1 (klas, 0, L"To Matrix", 0, 1, DO_TableOfReal_to_Matrix);
}

void praat_uvafon_stat_init ();
void praat_uvafon_stat_init () {

	Thing_recognizeClassesByName (classTableOfReal, classDistributions, classPairDistribution,
		classTable, classLinearRegression, classLogisticRegression, NULL);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	praat_addMenuCommand (L"Objects", L"New", L"Tables", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table with column names...", 0, 1, DO_Table_createWithColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table without column names...", 0, 1, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal...", 0, 1, DO_TableOfReal_create);

	praat_addMenuCommand (L"Objects", L"Open", L"Read TableOfReal from headerless spreadsheet file...", 0, 0, DO_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from tab-separated file...", 0, 0, DO_Table_readFromTabSeparatedFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from comma-separated file...", 0, 0, DO_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from whitespace-separated file...", 0, 0, DO_Table_readFromTableFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from table file...", 0, praat_HIDDEN, DO_Table_readFromTableFile);

	praat_addAction1 (classDistributions, 0, L"Distributions help", 0, 0, DO_Distributions_help);
	praat_TableOfReal_init (classDistributions);
	praat_addAction1 (classDistributions, 1, L"Get probability (label)...", L"Get value...", 1, DO_Distributions_getProbability);
	praat_addAction1 (classDistributions, 0, L"-- get from two --", L"Get probability (label)...", 1, 0);
	praat_addAction1 (classDistributions, 2, L"Get mean absolute difference...", L"-- get from two --", 1, DO_Distributionses_getMeanAbsoluteDifference);
	praat_addAction1 (classDistributions, 0, L"-- add --", L"Append", 1, 0);
	praat_addAction1 (classDistributions, 0, L"Add", L"-- add --", 1, DO_Distributionses_add);
	praat_addAction1 (classDistributions, 0, L"Generate", 0, 0, 0);
		praat_addAction1 (classDistributions, 0, L"To Strings...", 0, 0, DO_Distributions_to_Strings);
		praat_addAction1 (classDistributions, 0, L"To Strings (exact)...", 0, 0, DO_Distributions_to_Strings_exact);

	praat_addAction1 (classLogisticRegression, 0, L"Draw boundary...", 0, 0, DO_LogisticRegression_drawBoundary);

	praat_addAction1 (classPairDistribution, 0, L"PairDistribution help", 0, 0, DO_PairDistribution_help);
	praat_addAction1 (classPairDistribution, 0, L"To Table", 0, 0, DO_PairDistribution_to_Table);
	praat_addAction1 (classPairDistribution, 1, L"To Stringses...", 0, 0, DO_PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (classPairDistribution, 1, L"Get number of pairs", 0, 1, DO_PairDistribution_getNumberOfPairs);
		praat_addAction1 (classPairDistribution, 1, L"Get string1...", 0, 1, DO_PairDistribution_getString1);
		praat_addAction1 (classPairDistribution, 1, L"Get string2...", 0, 1, DO_PairDistribution_getString2);
		praat_addAction1 (classPairDistribution, 1, L"Get weight...", 0, 1, DO_PairDistribution_getWeight);
		praat_addAction1 (classPairDistribution, 1, L"-- get fraction correct --", 0, 1, 0);
		praat_addAction1 (classPairDistribution, 1, L"Get fraction correct (maximum likelihood)", 0, 1, DO_PairDistribution_getFractionCorrect_maximumLikelihood);
		praat_addAction1 (classPairDistribution, 1, L"Get fraction correct (probability matching)", 0, 1, DO_PairDistribution_getFractionCorrect_probabilityMatching);
	praat_addAction1 (classPairDistribution, 0, L"Modify -", 0, 0, 0);
	praat_addAction1 (classPairDistribution, 0, L"Remove zero weights", 0, 1, DO_PairDistribution_removeZeroWeights);
	praat_addAction1 (classPairDistribution, 0, L"Swap inputs and outputs", 0, 1, DO_PairDistribution_swapInputsAndOutputs);

	praat_addAction1 (classTable, 0, L"Table help", 0, 0, DO_Table_help);
	praat_addAction1 (classTable, 1, L"Save as tab-separated file...", 0, 0, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, L"Save as comma-separated file...", 0, 0, DO_Table_writeToCommaSeparatedFile);
	praat_addAction1 (classTable, 1, L"Save as table file...", 0, praat_HIDDEN, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, L"Write to table file...", 0, praat_HIDDEN, DO_Table_writeToTabSeparatedFile);
	praat_addAction1 (classTable, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Table_edit);
	praat_addAction1 (classTable, 1, L"Edit", 0, praat_HIDDEN, DO_Table_edit);
	praat_addAction1 (classTable, 0, L"Draw -", 0, 0, 0);
		praat_addAction1 (classTable, 0, L"Scatter plot...", 0, 1, DO_Table_scatterPlot);
		praat_addAction1 (classTable, 0, L"Scatter plot (mark)...", 0, 1, DO_Table_scatterPlot_mark);
		praat_addAction1 (classTable, 0, L"Draw ellipse (standard deviation)...", 0, 1, DO_Table_drawEllipse);
	praat_addAction1 (classTable, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (classTable, 1, L"List...", 0, 1, DO_Table_list);
		praat_addAction1 (classTable, 1, L"-- get structure --", 0, 1, 0);
		praat_addAction1 (classTable, 1, L"Get number of rows", 0, 1, DO_Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, L"Get number of columns", 0, 1, DO_Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, L"Get column label...", 0, 1, DO_Table_getColumnLabel);
		praat_addAction1 (classTable, 1, L"Get column index...", 0, 1, DO_Table_getColumnIndex);
		praat_addAction1 (classTable, 1, L"-- get value --", 0, 1, 0);
		praat_addAction1 (classTable, 1, L"Get value...", 0, 1, DO_Table_getValue);
		praat_addAction1 (classTable, 1, L"Search column...", 0, 1, DO_Table_searchColumn);
		praat_addAction1 (classTable, 1, L"-- statistics --", 0, 1, 0);
		praat_addAction1 (classTable, 1, L"Statistics tutorial", 0, 1, DO_StatisticsTutorial);
		praat_addAction1 (classTable, 1, L"-- get stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, L"Get quantile...", 0, 1, DO_Table_getQuantile);
		praat_addAction1 (classTable, 1, L"Get minimum...", 0, 1, DO_Table_getMinimum);
		praat_addAction1 (classTable, 1, L"Get maximum...", 0, 1, DO_Table_getMaximum);
		praat_addAction1 (classTable, 1, L"Get mean...", 0, 1, DO_Table_getMean);
		praat_addAction1 (classTable, 1, L"Get group mean...", 0, 1, DO_Table_getGroupMean);
		praat_addAction1 (classTable, 1, L"Get standard deviation...", 0, 1, DO_Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, L"-- report stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, L"Report mean (Student t)...", 0, 1, DO_Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, L"Report standard deviation...", 0, 1, DO_Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, L"Report difference (Student t)...", 0, 1, DO_Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, L"Report group mean (Student t)...", 0, 1, DO_Table_reportGroupMean_studentT);
		praat_addAction1 (classTable, 1, L"Report group difference (Student t)...", 0, 1, DO_Table_reportGroupDifference_studentT);
		praat_addAction1 (classTable, 1, L"Report group difference (Wilcoxon rank sum)...", 0, 1, DO_Table_reportGroupDifference_wilcoxonRankSum);
		praat_addAction1 (classTable, 1, L"Report correlation (Pearson r)...", 0, 1, DO_Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, L"Report correlation (Kendall tau)...", 0, 1, DO_Table_reportCorrelation_kendallTau);
	praat_addAction1 (classTable, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classTable, 0, L"Set string value...", 0, 1, DO_Table_setStringValue);
		praat_addAction1 (classTable, 0, L"Set numeric value...", 0, 1, DO_Table_setNumericValue);
		praat_addAction1 (classTable, 0, L"Formula...", 0, 1, DO_Table_formula);
		praat_addAction1 (classTable, 0, L"Formula (column range)...", 0, 1, DO_Table_formula_columnRange);
		praat_addAction1 (classTable, 0, L"Sort rows...", 0, 1, DO_Table_sortRows);
		praat_addAction1 (classTable, 0, L"Randomize rows", 0, 1, DO_Table_randomizeRows);
		praat_addAction1 (classTable, 0, L"Reflect rows", 0, 1, DO_Table_reflectRows);
		praat_addAction1 (classTable, 0, L"-- structure --", 0, 1, 0);
		praat_addAction1 (classTable, 0, L"Append row", 0, 1, DO_Table_appendRow);
		praat_addAction1 (classTable, 0, L"Append column...", 0, 1, DO_Table_appendColumn);
		praat_addAction1 (classTable, 0, L"Append sum column...", 0, 1, DO_Table_appendSumColumn);
		praat_addAction1 (classTable, 0, L"Append difference column...", 0, 1, DO_Table_appendDifferenceColumn);
		praat_addAction1 (classTable, 0, L"Append product column...", 0, 1, DO_Table_appendProductColumn);
		praat_addAction1 (classTable, 0, L"Append quotient column...", 0, 1, DO_Table_appendQuotientColumn);
		praat_addAction1 (classTable, 0, L"Remove row...", 0, 1, DO_Table_removeRow);
		praat_addAction1 (classTable, 0, L"Remove column...", 0, 1, DO_Table_removeColumn);
		praat_addAction1 (classTable, 0, L"Insert row...", 0, 1, DO_Table_insertRow);
		praat_addAction1 (classTable, 0, L"Insert column...", 0, 1, DO_Table_insertColumn);
		praat_addAction1 (classTable, 0, L"-- set --", 0, 1, 0);
		praat_addAction1 (classTable, 0, L"Set column label (index)...", 0, 1, DO_Table_setColumnLabel_index);
		praat_addAction1 (classTable, 0, L"Set column label (label)...", 0, 1, DO_Table_setColumnLabel_label);
	praat_addAction1 (classTable, 0, L"Analyse -", 0, 0, 0);
		praat_addAction1 (classTable, 0, L"To linear regression", 0, 1, DO_Table_to_LinearRegression);
		praat_addAction1 (classTable, 0, L"To logistic regression...", 0, 1, DO_Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, L"Synthesize -", 0, 0, 0);
		praat_addAction1 (classTable, 0, L"Append", 0, 1, DO_Tables_append);
	praat_addAction1 (classTable, 0, L"Generate -", 0, 0, 0);
		praat_addAction1 (classTable, 1, L"Draw row from distribution...", 0, 1, DO_Table_drawRowFromDistribution);
	praat_addAction1 (classTable, 0, L"Extract -", 0, 0, 0);
		praat_addAction1 (classTable, 0, L"Extract rows where column (number)...", 0, 1, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, L"Extract rows where column...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, L"Select rows where column...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, L"Extract rows where column (text)...", 0, 1, DO_Table_extractRowsWhereColumn_text);
		praat_addAction1 (classTable, 0, L"Transpose", 0, 1, DO_Table_transpose);
		praat_addAction1 (classTable, 0, L"Collapse rows...", 0, 1, DO_Table_collapseRows);
		praat_addAction1 (classTable, 0, L"Rows to columns...", 0, 1, DO_Table_rowsToColumns);
	praat_addAction1 (classTable, 0, L"Down to TableOfReal...", 0, 0, DO_Table_to_TableOfReal);

	praat_addAction1 (classTableOfReal, 0, L"TableOfReal help", 0, 0, DO_TableOfReal_help);
	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, L"Get fraction correct...", 0, 0, DO_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.cpp */
