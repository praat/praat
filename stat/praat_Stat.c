/* praat_Stat.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2010/06/23
 */

#include "praat.h"

#include "Distributions_and_Strings.h"
#include "LogisticRegression.h"
#include "Matrix.h"
#include "PairDistribution.h"
#include "Table.h"
#include "TableEditor.h"
#include "UnicodeData.h"

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
	Collection me = Collection_create (classDistributions, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new1 (Distributions_addMany (me), L"added")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (Distributionses_getMeanAbsoluteDifference, L"Get mean difference", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Distributions me = NULL, thee = NULL;
	WHERE (SELECTED) { if (me) thee = OBJECT; else me = OBJECT; }
	Melder_informationReal (Distributionses_getMeanAbsoluteDifference (me, thee, GET_INTEGER (L"Column number")), NULL);
END

FORM (Distributions_getProbability, L"Get probability", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"String", L"")
	OK
DO
	Melder_informationReal (Distributions_getProbability (ONLY_OBJECT,
		GET_STRING (L"String"), GET_INTEGER (L"Column number")), NULL);
END

DIRECT (Distributions_help) Melder_help (L"Distributions"); END

FORM (Distributions_to_Strings, L"To Strings", 0)
	NATURAL (L"Column number", L"1")
	NATURAL (L"Number of strings", L"1000")
	OK
DO
	EVERY_TO (Distributions_to_Strings (OBJECT, GET_INTEGER (L"Column number"), GET_INTEGER (L"Number of strings")))
END

FORM (Distributions_to_Strings_exact, L"To Strings (exact)", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	EVERY_TO (Distributions_to_Strings_exact (OBJECT, GET_INTEGER (L"Column number")))
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
	praat_picture_open ();
	WHERE (SELECTED) {
		LogisticRegression me = OBJECT;
		long xfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (L"Horizontal factor")); cherror
		long yfactor = Regression_getFactorIndexFromFactorName_e (me, GET_STRING (L"Vertical factor")); cherror
		LogisticRegression_drawBoundary (me, GRAPHICS,
			xfactor, GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			yfactor, GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Garnish"));
	}
	praat_picture_close ();
	return 1;
end:
END

/***** PAIRDISTRIBUTION *****/

DIRECT (PairDistribution_getFractionCorrect_maximumLikelihood)
	Melder_informationReal (PairDistribution_getFractionCorrect_maximumLikelihood (ONLY_OBJECT), NULL);
	iferror return 0;
END

DIRECT (PairDistribution_getFractionCorrect_probabilityMatching)
	Melder_informationReal (PairDistribution_getFractionCorrect_probabilityMatching (ONLY_OBJECT), NULL);
	iferror return 0;
END

DIRECT (PairDistribution_getNumberOfPairs)
	PairDistribution me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my pairs -> size));
END

FORM (PairDistribution_getString1, L"Get string1", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER (L"Pair number");
	if (ipair > my pairs -> size) {
		return Melder_error5 (L"Pair number (", Melder_integer (ipair), L") cannot be greater than number of pairs (", Melder_integer (my pairs -> size), L").");
	}
	PairProbability prob = my pairs -> item [ipair];
	Melder_information1 (prob -> string1);
END

FORM (PairDistribution_getString2, L"Get string2", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER (L"Pair number");
	if (ipair > my pairs -> size) {
		return Melder_error5 (L"Pair number (", Melder_integer (ipair), L") cannot be greater than number of pairs (", Melder_integer (my pairs -> size), L").");
	}
	PairProbability prob = my pairs -> item [ipair];
	Melder_information1 (prob -> string2);
END

FORM (PairDistribution_getWeight, L"Get weight", 0)
	NATURAL (L"Pair number", L"1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER (L"Pair number");
	if (ipair > my pairs -> size) {
		return Melder_error5 (L"Pair number (", Melder_integer (ipair), L") cannot be greater than number of pairs (", Melder_integer (my pairs -> size), L").");
	}
	PairProbability prob = my pairs -> item [ipair];
	Melder_information1 (Melder_double (prob -> weight));
END

DIRECT (PairDistribution_help) Melder_help (L"PairDistribution"); END

DIRECT (PairDistribution_removeZeroWeights)
	EVERY (PairDistribution_removeZeroWeights (OBJECT))
END

FORM (PairDistribution_to_Stringses, L"Generate two Strings objects", 0)
	NATURAL (L"Number", L"1000")
	SENTENCE (L"Name of first Strings", L"input")
	SENTENCE (L"Name of second Strings", L"output")
	OK
DO
	Strings strings1, strings2;
	if (! PairDistribution_to_Stringses (ONLY (classPairDistribution), GET_INTEGER (L"Number"), & strings1, & strings2)) return 0;
	if (! praat_new1 (strings1, GET_STRING (L"Name of first Strings"))) { forget (strings2); return 0; }
	if (! praat_new1 (strings2, GET_STRING (L"Name of second Strings"))) return 0;
END

DIRECT (PairDistribution_to_Table)
	EVERY_TO (PairDistribution_to_Table (OBJECT))
END

/***** PAIRDISTRIBUTION & DISTRIBUTIONS *****/

FORM (PairDistribution_Distributions_getFractionCorrect, L"PairDistribution & Distributions: Get fraction correct", 0)
	NATURAL (L"Column", L"1")
	OK
DO
	Melder_informationReal (PairDistribution_Distributions_getFractionCorrect
		(ONLY (classPairDistribution), ONLY (classDistributions), GET_INTEGER (L"Column")), NULL);
END

/***** TABLE *****/

DIRECT (Tables_append)
	Collection me = Collection_create (classTable, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new1 (Tables_append (me), L"appended")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (Table_appendColumn, L"Table: Append column", 0)
	WORD (L"Label", L"newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_appendColumn (OBJECT, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_appendDifferenceColumn, L"Table: Append difference column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
		Table_appendDifferenceColumn (OBJECT, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
end:
END

FORM (Table_appendProductColumn, L"Table: Append product column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
		Table_appendProductColumn (OBJECT, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
end:
END

FORM (Table_appendQuotientColumn, L"Table: Append quotient column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
		Table_appendQuotientColumn (OBJECT, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
end:
END

FORM (Table_appendSumColumn, L"Table: Append sum column", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	WORD (L"Label", L"diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
		long jcol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
		Table_appendSumColumn (OBJECT, icol, jcol, GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
end:
END

DIRECT (Table_appendRow)
	WHERE (SELECTED) {
		Table_appendRow (OBJECT);
		praat_dataChanged (OBJECT);   // BUG
		iferror return 0;   // BUG (occurs often in this file): if append fails, it should restore the previous overt state.
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
	WHERE (SELECTED) {
		if (! praat_new2 (Table_collapseRows (OBJECT,
			GET_STRING (L"factors"), GET_STRING (L"columnsToSum"),
			GET_STRING (L"columnsToAverage"), GET_STRING (L"columnsToMedianize"),
			GET_STRING (L"columnsToAverageLogarithmically"), GET_STRING (L"columnsToMedianizeLogarithmically")),
			NAME, L"_pooled")) return 0;
		praat_dataChanged (OBJECT);   // BUG: collapseRows should not change the original table overtly.
	}
END

FORM (Table_createWithColumnNames, L"Create Table with column names", 0)
	WORD (L"Name", L"table")
	INTEGER (L"Number of rows", L"10")
	LABEL (L"", L"Column names:")
	TEXTFIELD (L"columnNames", L"speaker dialect age vowel F0 F1 F2")
	OK
DO
	if (! praat_new1 (Table_createWithColumnNames
		(GET_INTEGER (L"Number of rows"), GET_STRING (L"columnNames")),
		GET_STRING (L"Name"))) return 0;
END

FORM (Table_createWithoutColumnNames, L"Create Table without column names", 0)
	WORD (L"Name", L"table")
	INTEGER (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"3")
	OK
DO
	if (! praat_new1 (Table_createWithoutColumnNames
		(GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns")),
		GET_STRING (L"Name"))) return 0;
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
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column")); cherror
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column")); cherror
		Table_drawEllipse (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Number of sigmas"), GET_INTEGER (L"Garnish"));
	}
	praat_picture_close ();
	return 1;
end:
END

FORM (Table_drawRowFromDistribution, L"Table: Draw row from distribution", 0)
	WORD (L"Column with distribution", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with distribution")); cherror
	long row = Table_drawRowFromDistribution (me, icol); cherror
	Melder_information1 (Melder_integer (row));
end:
END

DIRECT (Table_edit)
	if (theCurrentPraatApplication -> batch) {
		return Melder_error1 (L"Cannot edit a Table from batch.");
	} else {
		WHERE (SELECTED) {
			TableEditor editor = TableEditor_create (theCurrentPraatApplication -> topShell, ID_AND_FULL_NAME, ONLY_OBJECT);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
		}
	}
END

FORM (Table_extractRowsWhereColumn_number, L"Table: Extract rows where column (number)", 0)
	WORD (L"Extract all rows where column...", L"")
	RADIO_ENUM (L"...is...", kMelder_number, DEFAULT)
	REAL (L"...the number", L"0.0")
	OK
DO
	double value = GET_REAL (L"...the number");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Extract all rows where column...")); cherror
		if (! praat_new5 (Table_extractRowsWhereColumn_number (OBJECT,
			icol, GET_ENUM (kMelder_number, L"...is..."), value),
			NAME, L"_", Table_messageColumn (OBJECT, icol), L"_", NUMdefined (value) ? Melder_integer ((long) floor (value+0.5)) : L"undefined")) return 0;
		praat_dataChanged (OBJECT);
	}
end:
END

FORM (Table_extractRowsWhereColumn_text, L"Table: Extract rows where column (text)", 0)
	WORD (L"Extract all rows where column...", L"")
	OPTIONMENU_ENUM (L"...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	const wchar_t *value = GET_STRING (L"...the text");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Extract all rows where column...")); cherror
		if (! praat_new3 (Table_extractRowsWhereColumn_string (OBJECT,
			icol, GET_ENUM (kMelder_string, L"..."), value),
			NAME, L"_", value)) return 0;
		praat_dataChanged (OBJECT);
	}
end:
END

FORM (Table_formula, L"Table: Formula", L"Table: Formula...")
	WORD (L"Column label", L"")
	TEXTFIELD (L"formula", L"abs (self)")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
		if (! Table_formula (OBJECT, icol, GET_STRING (L"formula"), interpreter)) return 0;
		praat_dataChanged (OBJECT);
	}
end:
END

FORM (Table_formula_columnRange, L"Table: Formula (column range)", L"Table: Formula...")
	WORD (L"From column label", L"")
	WORD (L"To column label", L"")
	TEXTFIELD (L"formula", L"log10 (self)")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"From column label")); cherror
		long icol2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"To column label")); cherror
		if (! Table_formula_columnRange (OBJECT, icol1, icol2, GET_STRING (L"formula"), interpreter)) return 0;
		praat_dataChanged (OBJECT);
	}
end:
END

FORM (Table_getColumnIndex, L"Table: Get column index", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Melder_information1 (Melder_integer (Table_findColumnIndexFromColumnLabel (ONLY_OBJECT, GET_STRING (L"Column label"))));
END
	
FORM (Table_getColumnLabel, L"Table: Get column label", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = GET_INTEGER (L"Column number");
	REQUIRE (icol <= my numberOfColumns, L"Column number must not be greater than number of columns.")
	Melder_information1 (my columnHeaders [icol]. label);
END

FORM (Table_getGroupMean, L"Table: Get group mean", 0)
	WORD (L"Column label", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group", L"F")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column")); cherror
	Melder_information1 (Melder_double (Table_getGroupMean (ONLY_OBJECT, column, groupColumn, GET_STRING (L"Group"))));
end:
END

FORM (Table_getMaximum, L"Table: Get maximum", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	double maximum = Table_getMaximum_e (me, icol); cherror
	Melder_information1 (Melder_double (maximum));
end:
END
	
FORM (Table_getMean, L"Table: Get mean", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	double mean = Table_getMean_e (me, icol); cherror
	Melder_information1 (Melder_double (mean));
end:
END
	
FORM (Table_getMinimum, L"Table: Get minimum", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	double minimum = Table_getMinimum_e (me, icol); cherror
	Melder_information1 (Melder_double (minimum));
end:
END
	
FORM (Table_getQuantile, L"Table: Get quantile", 0)
	SENTENCE (L"Column label", L"")
	POSITIVE (L"Quantile", L"0.50 (= median)")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	double quantile = Table_getQuantile_e (me, icol, GET_REAL (L"Quantile"));
	Melder_information1 (Melder_double (quantile));
end:
END
	
FORM (Table_getStandardDeviation, L"Table: Get standard deviation", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	double stdev = Table_getStdev_e (me, icol);
	Melder_information1 (Melder_double (stdev));
end:
END
	
DIRECT (Table_getNumberOfColumns)
	Melder_information1 (Melder_integer (((Table) ONLY_OBJECT) -> numberOfColumns));
END

DIRECT (Table_getNumberOfRows)
	Melder_information1 (Melder_integer (((Table) ONLY_OBJECT) -> rows -> size));
END

FORM (Table_getValue, L"Table: Get value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long irow = GET_INTEGER (L"Row number");
	REQUIRE (irow >= 1 && irow <= my rows -> size, L"Row number out of range.")
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	Melder_information1 (((TableRow) my rows -> item [irow]) -> cells [icol]. string);
end:
END

DIRECT (Table_help) Melder_help (L"Table"); END

FORM (Table_insertColumn, L"Table: Insert column", 0)
	NATURAL (L"Position", L"1")
	WORD (L"Label", L"newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_insertColumn (OBJECT, GET_INTEGER (L"Position"), GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_insertRow, L"Table: Insert row", 0)
	NATURAL (L"Position", L"1")
	OK
DO
	WHERE (SELECTED) {
		Table_insertRow (OBJECT, GET_INTEGER (L"Position"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_list, L"Table: List", 0)
	BOOLEAN (L"Include row numbers", true)
	OK
DO
	WHERE (SELECTED) {
		Table_list (OBJECT, GET_INTEGER (L"Include row numbers"));
	}
END

FORM_READ (Table_readFromTableFile, L"Read Table from table file", 0, true)
	if (! praat_new1 (Table_readFromTableFile (file), MelderFile_name (file))) return 0;
END

FORM_READ (Table_readFromCommaSeparatedFile, L"Read Table from comma-separated file", 0, true)
	if (! praat_new1 (Table_readFromCharacterSeparatedTextFile (file, ','), MelderFile_name (file))) return 0;
END

FORM_READ (Table_readFromTabSeparatedFile, L"Read Table from tab-separated file", 0, true)
	if (! praat_new1 (Table_readFromCharacterSeparatedTextFile (file, '\t'), MelderFile_name (file))) return 0;
END

FORM (Table_removeColumn, L"Table: Remove column", 0)
	WORD (L"Column label", L"")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
		Table_removeColumn (me, icol);
		praat_dataChanged (me);
		iferror return 0;
	}
end:
END

FORM (Table_removeRow, L"Table: Remove row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	WHERE (SELECTED) {
		Table_removeRow (OBJECT, GET_INTEGER (L"Row number"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_reportCorrelation_kendallTau, L"Report correlation (Kendall tau)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
	long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	double correlation, significance, lowerLimit, upperLimit;
	correlation = Table_getCorrelation_kendallTau (me, column1, column2, unconfidence,
		& significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Correlation between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine3 (L"Correlation = ", Melder_double (correlation), L" (Kendall's tau-b)");
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest tau that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
END

FORM (Table_reportCorrelation_pearsonR, L"Report correlation (Pearson r)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
	long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	double correlation, significance, lowerLimit, upperLimit;
	correlation = Table_getCorrelation_pearsonR (me, column1, column2, unconfidence,
		& significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Correlation between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine3 (L"Correlation = ", Melder_double (correlation), L" (Pearson's r)");
	MelderInfo_writeLine2 (L"Number of degrees of freedom = ", Melder_integer (my rows -> size - 2));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest r that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
END
	
FORM (Table_reportDifference_studentT, L"Report difference (Student t)", 0)
	WORD (L"left Columns", L"")
	WORD (L"right Columns", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"left Columns")); cherror
	long column2 = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"right Columns")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	double difference, t, numberOfDegreesOfFreedom, significance, lowerLimit, upperLimit;
	difference = Table_getDifference_studentT (me, column1, column2, unconfidence,
		& t, & numberOfDegreesOfFreedom, & significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Difference between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine2 (L"Difference = ", Melder_double (difference));
	MelderInfo_writeLine2 (L"Student's t = ", Melder_double (t));
	MelderInfo_writeLine2 (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
END
	
FORM (Table_reportGroupDifference_studentT, L"Report group difference (Student t)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group 1", L"F")
	SENTENCE (L"Group 2", L"M")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column")); cherror
	long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	wchar_t *group1 = GET_STRING (L"Group 1"), *group2 = GET_STRING (L"Group 2");
	double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
	mean = Table_getGroupDifference_studentT (me, column, groupColumn, group1, group2, unconfidence,
		& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_write4 (L"Difference in column ", Table_messageColumn (me, column), L" between groups ", group1);
	MelderInfo_writeLine5 (L" and ", group2, L" of column ", Table_messageColumn (me, groupColumn), L":");
	MelderInfo_writeLine2 (L"Difference = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t = ", Melder_double (tFromZero));
	MelderInfo_writeLine2 (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
END

FORM (Table_reportGroupDifference_wilcoxonRankSum, L"Report group difference (Wilcoxon rank sum)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group 1", L"F")
	SENTENCE (L"Group 2", L"M")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column")); cherror
	long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column")); cherror
	wchar_t *group1 = GET_STRING (L"Group 1"), *group2 = GET_STRING (L"Group 2");
	double areaUnderCurve, rankSum, significanceFromZero;
	areaUnderCurve = Table_getGroupDifference_wilcoxonRankSum (me, column, groupColumn, group1, group2,
		& rankSum, & significanceFromZero);
	MelderInfo_open ();
	MelderInfo_write4 (L"Difference in column ", Table_messageColumn (me, column), L" between groups ", group1);
	MelderInfo_writeLine5 (L" and ", group2, L" of column ", Table_messageColumn (me, groupColumn), L":");
	MelderInfo_writeLine2 (L"Larger: ", areaUnderCurve < 0.5 ? group1 : areaUnderCurve > 0.5 ? group2 : L"(both equal)");
	MelderInfo_writeLine2 (L"Area under curve: ", Melder_double (areaUnderCurve));
	MelderInfo_writeLine2 (L"Rank sum: ", Melder_double (rankSum));
	MelderInfo_writeLine3 (L"Significance from zero: ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_close ();
end:
END

FORM (Table_reportGroupMean_studentT, L"Report group mean (Student t)", 0)
	WORD (L"Column", L"salary")
	WORD (L"Group column", L"gender")
	SENTENCE (L"Group", L"F")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column")); cherror
	long groupColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Group column")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	wchar_t *group = GET_STRING (L"Group");
	double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
	mean = Table_getGroupMean_studentT (me, column, groupColumn, group, unconfidence,
		& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_write4 (L"Mean in column ", Table_messageColumn (me, column), L" of group ", group);
	MelderInfo_writeLine3 (L" of column ", Table_messageColumn (me, groupColumn), L":");
	MelderInfo_writeLine2 (L"Mean = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t from zero = ", Melder_double (tFromZero));
	MelderInfo_writeLine2 (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
END

FORM (Table_reportMean_studentT, L"Report mean (Student t)", 0)
	WORD (L"Column", L"")
	POSITIVE (L"One-tailed unconfidence", L"0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column")); cherror
	double unconfidence = GET_REAL (L"One-tailed unconfidence");
	double mean, tFromZero, numberOfDegreesOfFreedom, significanceFromZero, lowerLimit, upperLimit;
	mean = Table_getMean_studentT (me, column, unconfidence,
		& tFromZero, & numberOfDegreesOfFreedom, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine3 (L"Mean of column ", Table_messageColumn (me, column), L":");
	MelderInfo_writeLine2 (L"Mean = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t from zero = ", Melder_double (tFromZero));
	MelderInfo_writeLine2 (L"Number of degrees of freedom = ", Melder_double (numberOfDegreesOfFreedom));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * unconfidence)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest value that cannot be rejected with " UNITEXT_GREEK_SMALL_LETTER_ALPHA " = ", Melder_double (unconfidence), L")");
	MelderInfo_close ();
end:
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
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, columnLabel); cherror
		if (! praat_new2 (Table_rowsToColumns (OBJECT,
			GET_STRING (L"factors"), icol, GET_STRING (L"columnsToExpand")),
			NAME, L"_nested")) return 0;
		praat_dataChanged (OBJECT);
	}
end:
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
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column")); cherror
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column")); cherror
		long markColumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column with marks")); cherror
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			markColumn, GET_INTEGER (L"Font size"), GET_INTEGER (L"Garnish"));
	}
	praat_picture_close ();
	return 1;
end:
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
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Horizontal column")); cherror
		long ycolumn = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Vertical column")); cherror
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	}
	praat_picture_close ();
	return 1;
end:
END

FORM (Table_searchColumn, L"Table: Search column", 0)
	WORD (L"Column label", L"")
	WORD (L"Value", L"")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
	Melder_information1 (Melder_integer (Table_searchColumn (me, icol, GET_STRING (L"Value"))));
end:
END
	
FORM (Table_setColumnLabel_index, L"Set column label", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	WHERE (SELECTED) {
		Table_setColumnLabel (OBJECT, GET_INTEGER (L"Column number"), GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_setColumnLabel_label, L"Set column label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	WHERE (SELECTED) {
		Table_setColumnLabel (OBJECT, Table_findColumnIndexFromColumnLabel (OBJECT, GET_STRING (L"Old label")), GET_STRING (L"New label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_setNumericValue, L"Table: Set numeric value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	REAL_OR_UNDEFINED (L"Numeric value", L"1.5")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
		Table_setNumericValue (me, GET_INTEGER (L"Row number"), icol, GET_REAL (L"Numeric value"));
		praat_dataChanged (me);
		iferror return 0;
	}
end:
END

FORM (Table_setStringValue, L"Table: Set string value", 0)
	NATURAL (L"Row number", L"1")
	WORD (L"Column label", L"")
	SENTENCE (L"String value", L"xx")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_getColumnIndexFromColumnLabel (me, GET_STRING (L"Column label")); cherror
		Table_setStringValue (me, GET_INTEGER (L"Row number"), icol, GET_STRING (L"String value"));
		praat_dataChanged (me);
		iferror return 0;
	}
end:
END

DIRECT (Table_randomizeRows)
	WHERE (SELECTED) {
		Table_randomizeRows (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_sortRows, L"Table: Sort rows", 0)
	LABEL (L"", L"One or more column labels for sorting:")
	TEXTFIELD (L"columnLabels", L"dialect gender name")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		if (! Table_sortRows_string (me, GET_STRING (L"columnLabels"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Table_to_LinearRegression)
	EVERY_TO (Table_to_LinearRegression (OBJECT))
END

FORM (Table_to_LogisticRegression, L"Table: To LogisticRegression", 0)
	LABEL (L"", L"Factors (column names):")
	TEXTFIELD (L"factors", L"F0 F1 duration")
	WORD (L"Dependent 1 (column name)", L"e")
	WORD (L"Dependent 2 (column name)", L"i")
	OK
DO
	EVERY_TO (Table_to_LogisticRegression (OBJECT, GET_STRING (L"factors"), GET_STRING (L"Dependent 1"), GET_STRING (L"Dependent 2")))
END

FORM (Table_to_TableOfReal, L"Table: Down to TableOfReal", 0)
	WORD (L"Column for row labels", L"")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_findColumnIndexFromColumnLabel (me, GET_STRING (L"Column for row labels"));
		if (! praat_new1 (Table_to_TableOfReal (OBJECT, icol), NAME)) return 0;
	}
END

FORM_WRITE (Table_writeToTableFile, L"Write Table to table file", 0, L"Table")
	if (! Table_writeToTableFile (ONLY_OBJECT, file)) return 0;
END

/***** TABLEOFREAL *****/

DIRECT (TablesOfReal_append)
	Collection me = Collection_create (classTableOfReal, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new1 (TablesOfReal_appendMany (me), L"appended")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (TableOfReal_create, L"Create TableOfReal", 0)
	WORD (L"Name", L"table")
	NATURAL (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"3")
	OK
DO
	if (! praat_new1 (TableOfReal_create (GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns")),
		GET_STRING (L"Name"))) return 0;
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
	EVERY_DRAW (TableOfReal_drawAsNumbers (OBJECT, GRAPHICS,
		GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
		GET_INTEGER (L"Format"), GET_INTEGER (L"Precision")))
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
	EVERY_DRAW (TableOfReal_drawAsNumbers_if (OBJECT, GRAPHICS,
		GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
		GET_INTEGER (L"Format"), GET_INTEGER (L"Precision"), GET_STRING (L"condition"), interpreter))
END

FORM (TableOfReal_drawAsSquares, L"Draw table as squares", 0)
	INTEGER (L"From row", L"1")
	INTEGER (L"To row", L"0")
	INTEGER (L"From column", L"1")
	INTEGER (L"To column", L"0")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsSquares (OBJECT, GRAPHICS, 
		GET_INTEGER (L"From row"), GET_INTEGER (L"To row"),
		GET_INTEGER (L"From column"), GET_INTEGER (L"To column"),
		GET_INTEGER (L"Garnish")))
END

FORM (TableOfReal_drawHorizontalLines, L"Draw horizontal lines", 0)
	NATURAL (L"From row", L"1") INTEGER (L"To row", L"0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawHorizontalLines (OBJECT, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"))) END
FORM (TableOfReal_drawLeftAndRightLines, L"Draw left and right lines", 0)
	NATURAL (L"From row", L"1") INTEGER (L"To row", L"0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawLeftAndRightLines (OBJECT, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"))) END
FORM (TableOfReal_drawTopAndBottomLines, L"Draw top and bottom lines", 0)
	NATURAL (L"From row", L"1") INTEGER (L"To row", L"0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawTopAndBottomLines (OBJECT, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"))) END
FORM (TableOfReal_drawVerticalLines, L"Draw vertical lines", 0)
	NATURAL (L"From row", L"1") INTEGER (L"To row", L"0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawVerticalLines (OBJECT, GRAPHICS, GET_INTEGER (L"From row"), GET_INTEGER (L"To row"))) END

DIRECT (TableOfReal_extractColumnLabelsAsStrings)
	EVERY_TO (TableOfReal_extractColumnLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractColumnRanges, L"Extract column ranges", 0)
	LABEL (L"", L"Create a new TableOfReal from the following columns:")
	TEXTFIELD (L"ranges", L"1 2")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (TableOfReal_extractColumnRanges (OBJECT, GET_STRING (L"ranges")), NAME, L"_cols")) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhere, L"Extract columns where", 0)
	LABEL (L"", L"Extract all columns with at least one cell where:")
	TEXTFIELD (L"condition", L"col mod 3 = 0 ; this example extracts every third column")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (TableOfReal_extractColumnsWhere (OBJECT, GET_STRING (L"condition"), interpreter), NAME, L"_cols")) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhereLabel, L"Extract column where label", 0)
	OPTIONMENU_ENUM (L"Extract all columns whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"a")
	OK
DO
	const wchar_t *text = GET_STRING (L"...the text");
	WHERE (SELECTED) {
		if (! praat_new3 (TableOfReal_extractColumnsWhereLabel (OBJECT,
			GET_ENUM (kMelder_string, L"Extract all columns whose label..."), text),
			NAME, L"_", text)) return 0;
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
	WHERE (SELECTED) {
		if (! praat_new5 (TableOfReal_extractColumnsWhereRow (OBJECT,
			row, GET_ENUM (kMelder_number, L"...is..."), value),
			NAME, L"_", Melder_integer (row), L"_", Melder_integer ((long) floor (value+0.5)))) return 0;
	}
END

DIRECT (TableOfReal_extractRowLabelsAsStrings)
	EVERY_TO (TableOfReal_extractRowLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractRowRanges, L"Extract row ranges", 0)
	LABEL (L"", L"Create a new TableOfReal from the following rows:")
	TEXTFIELD (L"ranges", L"1 2")
	LABEL (L"", L"To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (TableOfReal_extractRowRanges (OBJECT, GET_STRING (L"ranges")), NAME, L"_rows")) return 0;
	}
END

FORM (TableOfReal_extractRowsWhere, L"Extract rows where", 0)
	LABEL (L"", L"Extract all rows with at least one cell where:")
	TEXTFIELD (L"condition", L"row mod 3 = 0 ; this example extracts every third row")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (TableOfReal_extractRowsWhere (OBJECT, GET_STRING (L"condition"), interpreter), NAME, L"_rows")) return 0;
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
	WHERE (SELECTED) {
		if (! praat_new5 (TableOfReal_extractRowsWhereColumn (OBJECT,
			column, GET_ENUM (kMelder_number, L"...is..."), value),
			NAME, L"_", Melder_integer (column), L"_", Melder_integer ((long) floor (value+0.5)))) return 0;
	}
END

FORM (TableOfReal_extractRowsWhereLabel, L"Extract rows where label", 0)
	OPTIONMENU_ENUM (L"Extract all rows whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"a")
	OK
DO
	const wchar_t *text = GET_STRING (L"...the text");
	WHERE (SELECTED) {
		if (! praat_new3 (TableOfReal_extractRowsWhereLabel (OBJECT,
			GET_ENUM (kMelder_string, L"Extract all rows whose label..."), text),
			NAME, L"_", text)) return 0;
	}
END

FORM (TableOfReal_formula, L"TableOfReal: Formula", L"Formula...")
	LABEL (L"", L"for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	TEXTFIELD (L"formula", L"if col = 5 then self + self [6] else self fi")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_formula (OBJECT, GET_STRING (L"formula"), interpreter, NULL)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_getColumnIndex, L"Get column index", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	Melder_information1 (Melder_integer (TableOfReal_columnLabelToIndex (ONLY_OBJECT, GET_STRING (L"Column label"))));
END
	
FORM (TableOfReal_getColumnLabel, L"Get column label", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER (L"Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, L"Column number must not be greater than number of columns.")
	Melder_information1 (table -> columnLabels == NULL ? L"" : table -> columnLabels [columnNumber]);
END
	
FORM (TableOfReal_getColumnMean_index, L"Get column mean", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER (L"Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, L"Column number must not be greater than number of columns.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnMean_label, L"Get column mean", 0)
	SENTENCE (L"Column label", L"")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRING (L"Column label"));
	REQUIRE (columnNumber > 0, L"Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnStdev_index, L"Get column standard deviation", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	Melder_informationReal (TableOfReal_getColumnStdev (ONLY_OBJECT, GET_INTEGER (L"Column number")), NULL);
END
	
FORM (TableOfReal_getColumnStdev_label, L"Get column standard deviation", 0)
	SENTENCE (L"Column label", L"1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRING (L"Column label"));
	REQUIRE (columnNumber > 0, L"Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnStdev (table, columnNumber), NULL);
END

DIRECT (TableOfReal_getNumberOfColumns) TableOfReal me = ONLY_OBJECT; Melder_information1 (Melder_integer (my numberOfColumns)); END
DIRECT (TableOfReal_getNumberOfRows) TableOfReal me = ONLY_OBJECT; Melder_information1 (Melder_integer (my numberOfRows)); END

FORM (TableOfReal_getRowIndex, L"Get row index", 0)
	SENTENCE (L"Row label", L"")
	OK
DO
	Melder_information1 (Melder_integer (TableOfReal_rowLabelToIndex (ONLY_OBJECT, GET_STRING (L"Row label"))));
END
	
FORM (TableOfReal_getRowLabel, L"Get row label", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long rowNumber = GET_INTEGER (L"Row number");
	REQUIRE (rowNumber <= table -> numberOfRows, L"Row number must not be greater than number of rows.")
	Melder_information1 (table -> rowLabels == NULL ? L"" : table -> rowLabels [rowNumber]);
END

FORM (TableOfReal_getValue, L"Get value", 0)
	NATURAL (L"Row number", L"1") NATURAL (L"Column number", L"1") OK DO TableOfReal me = ONLY_OBJECT;
	long row = GET_INTEGER (L"Row number"), column = GET_INTEGER (L"Column number");
	REQUIRE (row <= my numberOfRows, L"Row number must not exceed number of rows.")
	REQUIRE (column <= my numberOfColumns, L"Column number must not exceed number of columns.")
	Melder_informationReal (my data [row] [column], NULL); END

DIRECT (TableOfReal_help) Melder_help (L"TableOfReal"); END

FORM (TableOfReal_insertColumn, L"Insert column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertColumn (OBJECT, GET_INTEGER (L"Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_insertRow, L"Insert row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertRow (OBJECT, GET_INTEGER (L"Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM_READ (TableOfReal_readFromHeaderlessSpreadsheetFile, L"Read TableOfReal from headerless spreadsheet file", 0, true)
	if (! praat_new1 (TableOfReal_readFromHeaderlessSpreadsheetFile (file), MelderFile_name (file))) return 0;
END

FORM (TableOfReal_removeColumn, L"Remove column", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeColumn (OBJECT, GET_INTEGER (L"Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_removeRow, L"Remove row", 0)
	NATURAL (L"Row number", L"1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeRow (OBJECT, GET_INTEGER (L"Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_index, L"Set column label", 0)
	NATURAL (L"Column number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, GET_INTEGER (L"Column number"), GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_label, L"Set column label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, TableOfReal_columnLabelToIndex (OBJECT, GET_STRING (L"Old label")),
			GET_STRING (L"New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setRowLabel_index, L"Set row label", 0)
	NATURAL (L"Row number", L"1")
	SENTENCE (L"Label", L"")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, GET_INTEGER (L"Row number"), GET_STRING (L"Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setValue, L"Set value", L"TableOfReal: Set value...")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	REAL_OR_UNDEFINED (L"New value", L"0.0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal me = OBJECT;
		long irow = GET_INTEGER (L"Row number"), icol = GET_INTEGER (L"Column number");
		REQUIRE (irow <= my numberOfRows, L"Row number too large.")
		REQUIRE (icol <= my numberOfColumns, L"Column number too large.")
		my data [irow] [icol] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setRowLabel_label, L"Set row label", 0)
	SENTENCE (L"Old label", L"")
	SENTENCE (L"New label", L"")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, TableOfReal_rowLabelToIndex (OBJECT, GET_STRING (L"Old label")),
			GET_STRING (L"New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByColumn, L"Sort rows by column", 0)
	INTEGER (L"Column", L"1")
	INTEGER (L"Secondary column", L"0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByColumn (OBJECT, GET_INTEGER (L"Column"), GET_INTEGER (L"Secondary column"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByLabel, L"Sort rows by label", 0)
	LABEL (L"", L"Secondary sorting keys:")
	INTEGER (L"Column1", L"1")
	INTEGER (L"Column2", L"0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByLabel (OBJECT, GET_INTEGER (L"Column1"), GET_INTEGER (L"Column2"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TableOfReal_to_Matrix)
	EVERY_TO (TableOfReal_to_Matrix (OBJECT))
END

FORM (TableOfReal_to_Table, L"TableOfReal: To Table", 0)
	SENTENCE (L"Label of first column", L"rowLabel")
	OK
DO
	EVERY_TO (TableOfReal_to_Table (OBJECT, GET_STRING (L"Label of first column")))
END

FORM_WRITE (TableOfReal_writeToHeaderlessSpreadsheetFile, L"Write TableOfReal to spreadsheet", 0, L"txt")
	if (! TableOfReal_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
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

void praat_TableOfReal_init (void *klas);   /* Buttons for TableOfReal and for its subclasses. */
void praat_TableOfReal_init (void *klas) {
	praat_addAction1 (klas, 1, L"Write to headerless spreadsheet file...", 0, 0, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
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

void praat_uvafon_Stat_init (void);
void praat_uvafon_Stat_init (void) {

	Thing_recognizeClassesByName (classTableOfReal, classDistributions, classPairDistribution,
		classTable, classLinearRegression, classLogisticRegression, NULL);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	praat_addMenuCommand (L"Objects", L"New", L"Tables", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table with column names...", 0, 1, DO_Table_createWithColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table without column names...", 0, 1, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create Table...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand (L"Objects", L"New", L"Create TableOfReal...", 0, 1, DO_TableOfReal_create);

	praat_addMenuCommand (L"Objects", L"Open", L"Read TableOfReal from headerless spreadsheet file...", 0, 0, DO_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from table file...", 0, 0, DO_Table_readFromTableFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from comma-separated file...", 0, 0, DO_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read Table from tab-separated file...", 0, 0, DO_Table_readFromTabSeparatedFile);

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
	praat_addAction1 (classPairDistribution, 1, L"Remove zero weights", 0, 0, DO_PairDistribution_removeZeroWeights);

	praat_addAction1 (classTable, 0, L"Table help", 0, 0, DO_Table_help);
	praat_addAction1 (classTable, 1, L"Write to table file...", 0, 0, DO_Table_writeToTableFile);
	praat_addAction1 (classTable, 1, L"Edit", 0, 0, DO_Table_edit);
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
		praat_addAction1 (classTable, 0, L"Collapse rows...", 0, 1, DO_Table_collapseRows);
		praat_addAction1 (classTable, 0, L"Rows to columns...", 0, 1, DO_Table_rowsToColumns);
	praat_addAction1 (classTable, 0, L"Down to TableOfReal...", 0, 0, DO_Table_to_TableOfReal);

	praat_addAction1 (classTableOfReal, 0, L"TableOfReal help", 0, 0, DO_TableOfReal_help);
	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, L"Get fraction correct...", 0, 0, DO_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.c */
