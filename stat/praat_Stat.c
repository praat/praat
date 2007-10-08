/* praat_Stat.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/12
 */

#include "praat.h"

#include "Distributions_and_Strings.h"
#include "Matrix.h"
#include "PairDistribution.h"
#include "Table.h"
#include "TableEditor.h"
#include "Regression.h"

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
	if (! praat_new (Distributions_addMany (me), "added")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (Distributionses_getMeanAbsoluteDifference, "Get mean difference", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Distributions me = NULL, thee = NULL;
	WHERE (SELECTED) { if (me) thee = OBJECT; else me = OBJECT; }
	Melder_informationReal (Distributionses_getMeanAbsoluteDifference (me, thee, GET_INTEGER ("Column number")), NULL);
END

FORM (Distributions_getProbability, "Get probability", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("String", "")
	OK
DO
	Melder_informationReal (Distributions_getProbability (ONLY_OBJECT,
		GET_STRINGW (L"String"), GET_INTEGER ("Column number")), NULL);
END

DIRECT (Distributions_help) Melder_help (L"Distributions"); END

FORM (Distributions_to_Strings, "To Strings", 0)
	NATURAL ("Column number", "1")
	NATURAL ("Number of strings", "1000")
	OK
DO
	EVERY_TO (Distributions_to_Strings (OBJECT, GET_INTEGER ("Column number"), GET_INTEGER ("Number of strings")))
END

FORM (Distributions_to_Strings_exact, "To Strings (exact)", 0)
	NATURAL ("Column number", "1")
	OK
DO
	EVERY_TO (Distributions_to_Strings_exact (OBJECT, GET_INTEGER ("Column number")))
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

FORM (PairDistribution_getString1, "Get string1", 0)
	NATURAL ("Pair number", "1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER ("Pair number");
	if (ipair > my pairs -> size) {
		return Melder_error5 (L"Pair number (", Melder_integer (ipair), L") cannot be greater than number of pairs (", Melder_integer (my pairs -> size), L").");
	}
	PairProbability prob = my pairs -> item [ipair];
	Melder_information1 (prob -> string1);
END

FORM (PairDistribution_getString2, "Get string2", 0)
	NATURAL ("Pair number", "1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER ("Pair number");
	if (ipair > my pairs -> size) {
		return Melder_error5 (L"Pair number (", Melder_integer (ipair), L") cannot be greater than number of pairs (", Melder_integer (my pairs -> size), L").");
	}
	PairProbability prob = my pairs -> item [ipair];
	Melder_information1 (prob -> string2);
END

FORM (PairDistribution_getWeight, "Get weight", 0)
	NATURAL ("Pair number", "1")
	OK
DO
	PairDistribution me = ONLY_OBJECT;
	long ipair = GET_INTEGER ("Pair number");
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

FORM (PairDistribution_to_Stringses, "Generate two Strings objects", 0)
	NATURAL ("Number", "1000")
	SENTENCE ("Name of first Strings", "input")
	SENTENCE ("Name of second Strings", "output")
	OK
DO
	Strings strings1, strings2;
	if (! PairDistribution_to_Stringses (ONLY (classPairDistribution), GET_INTEGER ("Number"), & strings1, & strings2)) return 0;
	if (! praat_new (strings1, "%s", GET_STRING ("Name of first Strings"))) { forget (strings2); return 0; }
	if (! praat_new (strings2, "%s", GET_STRING ("Name of second Strings"))) return 0;
END

DIRECT (PairDistribution_to_Table)
	EVERY_TO (PairDistribution_to_Table (OBJECT))
END

/***** PAIRDISTRIBUTION & DISTRIBUTIONS *****/

FORM (PairDistribution_Distributions_getFractionCorrect, "PairDistribution & Distributions: Get fraction correct", 0)
	NATURAL ("Column", "1")
	OK
DO
	Melder_informationReal (PairDistribution_Distributions_getFractionCorrect
		(ONLY (classPairDistribution), ONLY (classDistributions), GET_INTEGER ("Column")), NULL);
END

/***** TABLE *****/

FORM (Table_appendColumn, "Table: Append column", 0)
	WORD ("Label", "newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_appendColumn (OBJECT, GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_appendDifferenceColumn, "Table: Append difference column", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	WORD ("Label", "diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendDifferenceColumn (OBJECT, icol, jcol, GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_appendProductColumn, "Table: Append product column", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	WORD ("Label", "diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendProductColumn (OBJECT, icol, jcol, GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_appendQuotientColumn, "Table: Append quotient column", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	WORD ("Label", "diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendQuotientColumn (OBJECT, icol, jcol, GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_appendSumColumn, "Table: Append sum column", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	WORD ("Label", "diff")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendSumColumn (OBJECT, icol, jcol, GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

DIRECT (Table_appendRow)
	WHERE (SELECTED) {
		Table_appendRow (OBJECT);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_collapseRows, "Table: Collapse rows", 0)
	LABEL ("", "Columns with factors (independent variables):")
	TEXTFIELD ("factors", "speaker dialect age vowel")
	LABEL ("", "Columns to sum:")
	TEXTFIELD ("columnsToSum", "number cost")
	LABEL ("", "Columns to average:")
	TEXTFIELD ("columnsToAverage", "price")
	LABEL ("", "Columns to medianize:")
	TEXTFIELD ("columnsToMedianize", "vot")
	LABEL ("", "Columns to average logarithmically:")
	TEXTFIELD ("columnsToAverageLogarithmically", "duration")
	LABEL ("", "Columns to medianize logarithmically:")
	TEXTFIELD ("columnsToMedianizeLogarithmically", "F0 F1 F2 F3")
	LABEL ("", "Columns not mentioned above will be ignored.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Table_collapseRows (OBJECT,
			GET_STRINGW (L"factors"), GET_STRINGW (L"columnsToSum"),
			GET_STRINGW (L"columnsToAverage"), GET_STRINGW (L"columnsToMedianize"),
			GET_STRINGW (L"columnsToAverageLogarithmically"), GET_STRINGW (L"columnsToMedianizeLogarithmically")),
			"%s_pooled", NAME)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_createWithColumnNames, "Create Table with column names", 0)
	WORD ("Name", "table")
	INTEGER ("Number of rows", "10")
	LABEL ("", "Column names:")
	TEXTFIELD ("columnNames", "speaker dialect age vowel F0 F1 F2")
	OK
DO
	if (! praat_new (Table_createWithColumnNames
		(GET_INTEGER ("Number of rows"), GET_STRINGW (L"columnNames")),
		GET_STRING ("Name"))) return 0;
END

FORM (Table_createWithoutColumnNames, "Create Table without column names", 0)
	WORD ("Name", "table")
	INTEGER ("Number of rows", "10")
	NATURAL ("Number of columns", "3")
	OK
DO
	if (! praat_new (Table_createWithoutColumnNames
		(GET_INTEGER ("Number of rows"), GET_INTEGER ("Number of columns")),
		GET_STRING ("Name"))) return 0;
END

FORM (Table_drawEllipse, "Draw ellipse (standard deviation)", 0)
	WORD ("Horizontal column", "")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0 (= auto)")
	WORD ("Vertical column", "")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0 (= auto)")
	POSITIVE ("Number of sigmas", "2.0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Horizontal column"));
		long ycolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Vertical column"));
		Table_drawEllipse (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
			GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
			GET_REAL ("Number of sigmas"), GET_INTEGER ("Garnish"));
	}
	praat_picture_close ();
	return 1;
END

DIRECT (Table_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error ("Cannot edit a Table from batch.");
	} else {
		WHERE (SELECTED) {
			TableEditor editor = TableEditor_create (theCurrentPraat -> topShell, FULL_NAMEW, ONLY_OBJECT);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
		}
	}
END

FORM (Table_extractRowsWhereColumn_number, "Table: Extract rows where column (number)", 0)
	WORD ("Extract all rows where column...", "")
	RADIO ("...is...", 1)
	RADIOBUTTONS_ENUMW (Melder_NUMBER_text_adjective (itext), Melder_NUMBER_min, Melder_NUMBER_max)
	REAL ("...the number", "0.0")
	OK
DO
	double value = GET_REAL ("...the number");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Extract all rows where column..."));
		if (icol == 0) return Melder_error ("No such column.");
		if (! praat_new (Table_extractRowsWhereColumn_number (OBJECT,
			icol, GET_INTEGER ("...is...") - 1 + Melder_NUMBER_min, value),
			"%s_%ls_%ld", NAME, Table_messageColumn (OBJECT, icol),
			(long) floor (value+0.5))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_extractRowsWhereColumn_text, "Table: Extract rows where column (text)", 0)
	WORD ("Extract all rows where column...", "")
	OPTIONMENU ("...", 1)
	OPTIONS_ENUMW (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "hi")
	OK
DO
	const wchar_t *value = GET_STRINGW (L"...the text");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Extract all rows where column..."));
		if (icol == 0) return Melder_error ("No such column.");
		if (! praat_new (Table_extractRowsWhereColumn_string (OBJECT,
			icol, GET_INTEGER ("...") - 1 + Melder_STRING_min, value),
			"%s_%ls", NAME, value)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_formula, "Table: Formula", "Table: Formula...")
	WORD ("Column label", "")
	TEXTFIELD ("formula", "abs (self)")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		if (! Table_formula (OBJECT, icol, GET_STRINGW (L"formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_formula_columnRange, "Table: Formula (column range)", "Table: Formula...")
	WORD ("From column label", "")
	WORD ("To column label", "")
	TEXTFIELD ("formula", "log10 (self)")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol1 = Table_columnLabelToIndex (me, GET_STRINGW (L"From column label"));
		if (icol1 == 0) return Melder_error ("No such column.");
		long icol2 = Table_columnLabelToIndex (me, GET_STRINGW (L"To column label"));
		if (icol2 == 0) return Melder_error ("No such column.");
		if (! Table_formula_columnRange (OBJECT, icol1, icol2, GET_STRINGW (L"formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_getColumnIndex, "Table: Get column index", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Melder_information1 (Melder_integer (Table_columnLabelToIndex (ONLY_OBJECT, GET_STRINGW (L"Column label"))));
END
	
FORM (Table_getColumnLabel, "Table: Get column label", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = GET_INTEGER ("Column number");
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information1 (my columnHeaders [icol]. label);
END

FORM (Table_getGroupMean, "Table: Get group mean", 0)
	WORD ("Column label", "salary")
	WORD ("Group column", "gender")
	SENTENCE ("Group", "F")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	if (column == 0) return Melder_error ("No such column.");
	long groupColumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Group column"));
	if (groupColumn == 0) return Melder_error ("No such column.");
	Melder_information1 (Melder_double (Table_getGroupMean (ONLY_OBJECT, column, groupColumn, GET_STRINGW (L"Group"))));
END

FORM (Table_getMean, "Table: Get mean", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information1 (Melder_double (Table_getMean (ONLY_OBJECT, icol)));
END
	
FORM (Table_getQuantile, "Table: Get quantile", 0)
	SENTENCE ("Column label", "")
	POSITIVE ("Quantile", "0.50 (= median)")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information1 (Melder_double (Table_getQuantile (ONLY_OBJECT, icol, GET_REAL ("Quantile"))));
END
	
FORM (Table_getStandardDeviation, "Table: Get standard deviation", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information1 (Melder_double (Table_getStdev (ONLY_OBJECT, icol)));
END
	
DIRECT (Table_to_LinearRegression)
	EVERY_TO (Table_to_LinearRegression (OBJECT))
END

DIRECT (Table_to_LogisticRegression)
	EVERY_TO (Table_to_LogisticRegression (OBJECT))
END

DIRECT (Table_getNumberOfColumns)
	Melder_information1 (Melder_integer (((Table) ONLY_OBJECT) -> numberOfColumns));
END

DIRECT (Table_getNumberOfRows)
	Melder_information1 (Melder_integer (((Table) ONLY_OBJECT) -> rows -> size));
END

FORM (Table_getValue, "Table: Get value", 0)
	NATURAL ("Row number", "1")
	WORD ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long irow = GET_INTEGER ("Row number");
	long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	if (icol == 0) return Melder_error ("No such column.");
	REQUIRE (irow >= 1 && irow <= my rows -> size, "Row number out of range.")
	REQUIRE (icol >= 1 && icol <= my numberOfColumns, "Column number out of range.")
	Melder_information1 (((TableRow) my rows -> item [irow]) -> cells [icol]. string);
END

DIRECT (Table_help) Melder_help (L"Table"); END

FORM (Table_insertColumn, "Table: Insert column", 0)
	NATURAL ("Position", "1")
	WORD ("Label", "newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_insertColumn (OBJECT, GET_INTEGER ("Position"), GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_insertRow, "Table: Insert row", 0)
	NATURAL ("Position", "1")
	OK
DO
	WHERE (SELECTED) {
		Table_insertRow (OBJECT, GET_INTEGER ("Position"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_list, "Table: List", 0)
	BOOLEAN ("Include row numbers", true)
	OK
DO
	WHERE (SELECTED) {
		Table_list (OBJECT, GET_INTEGER ("Include row numbers"));
	}
END

FORM_READ (Table_readFromTableFile, "Read Table from table file", 0)
	if (! praat_new1 (Table_readFromTableFile (file), MelderFile_name (file))) return 0;
END

FORM_READ (Table_readFromCommaSeparatedFile, "Read Table from comma-separated file", 0)
	if (! praat_new1 (Table_readFromCharacterSeparatedTextFile (file, ','), MelderFile_name (file))) return 0;
END

FORM_READ (Table_readFromTabSeparatedFile, "Read Table from tab-separated file", 0)
	if (! praat_new1 (Table_readFromCharacterSeparatedTextFile (file, '\t'), MelderFile_name (file))) return 0;
END

FORM (Table_removeColumn, "Table: Remove column", 0)
	WORD ("Column label", "")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		Table_removeColumn (me, icol);
		praat_dataChanged (me);
		iferror return 0;
	}
END

FORM (Table_removeRow, "Table: Remove row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	WHERE (SELECTED) {
		Table_removeRow (OBJECT, GET_INTEGER ("Row number"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_reportCorrelation_kendallTau, "Report correlation (Kendall tau)", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double correlation, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	correlation = Table_getCorrelation_kendallTau (me, column1, column2, significanceLevel,
		& significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Correlation between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine3 (L"Correlation = ", Melder_double (correlation), L" (Kendall's tau-b)");
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest tau that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest tau that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END

FORM (Table_reportCorrelation_pearsonR, "Report correlation (Pearson r)", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double correlation, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	correlation = Table_getCorrelation_pearsonR (me, column1, column2, significanceLevel,
		& significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Correlation between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine3 (L"Correlation = ", Melder_double (correlation), L" (Pearson's r)");
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest r that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest r that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END
	
FORM (Table_reportDifference_studentT, "Report difference (Student t)", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_columnLabelToIndex (me, GET_STRINGW (L"left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRINGW (L"right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double difference, t, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	difference = Table_getDifference_studentT (me, column1, column2, significanceLevel,
		& t, & significance, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine5 (L"Difference between column ", Table_messageColumn (me, column1),
		L" and column ", Table_messageColumn (me, column2), L":");
	MelderInfo_writeLine2 (L"Difference = ", Melder_double (difference));
	MelderInfo_writeLine2 (L"Student's t = ", Melder_double (t));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significance), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END
	
FORM (Table_reportGroupDifference_studentT, "Report group difference (Student t)", 0)
	WORD ("Column", "salary")
	WORD ("Group column", "gender")
	SENTENCE ("Group 1", "F")
	SENTENCE ("Group 2", "M")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_columnLabelToIndex (me, GET_STRINGW (L"Column"));
	if (column == 0) return Melder_error ("No such column.");
	long groupColumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Group column"));
	if (groupColumn == 0) return Melder_error ("No such column.");
	double significanceLevel = GET_REAL ("Significance level");
	wchar_t *group1 = GET_STRINGW (L"Group 1"), *group2 = GET_STRINGW (L"Group 2");
	double mean, tFromZero, significanceFromZero, lowerLimit, upperLimit;
	mean = Table_getGroupDifference_studentT (me, column, groupColumn, group1, group2, significanceLevel,
		& tFromZero, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_write4 (L"Difference in column ", Table_messageColumn (me, column), L" between groups ", group1);
	MelderInfo_writeLine5 (L" and ", group2, L" of column ", Table_messageColumn (me, groupColumn), L":");
	MelderInfo_writeLine2 (L"Difference = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t = ", Melder_double (tFromZero));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END

FORM (Table_reportGroupMean_studentT, "Report group mean (Student t)", 0)
	WORD ("Column", "salary")
	WORD ("Group column", "gender")
	SENTENCE ("Group", "F")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_columnLabelToIndex (me, GET_STRINGW (L"Column"));
	if (column == 0) return Melder_error ("No such column.");
	long groupColumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Group column"));
	if (groupColumn == 0) return Melder_error ("No such column.");
	double significanceLevel = GET_REAL ("Significance level");
	wchar_t *group = GET_STRINGW (L"Group");
	double mean, tFromZero, significanceFromZero, lowerLimit, upperLimit;
	mean = Table_getGroupMean_studentT (me, column, groupColumn, group, significanceLevel,
		& tFromZero, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_write4 (L"Mean in column ", Table_messageColumn (me, column), L" of group ", group);
	MelderInfo_writeLine3 (L" of column ", Table_messageColumn (me, groupColumn), L":");
	MelderInfo_writeLine2 (L"Mean = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t from zero = ", Melder_double (tFromZero));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest difference that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END

FORM (Table_reportMean_studentT, "Report mean (Student t)", 0)
	WORD ("Column", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_columnLabelToIndex (me, GET_STRINGW (L"Column"));
	double significanceLevel = GET_REAL ("Significance level");
	double mean, tFromZero, significanceFromZero, lowerLimit, upperLimit;
	if (column == 0) return Melder_error ("No such column.");
	mean = Table_getMean_studentT (me, column, significanceLevel,
		& tFromZero, & significanceFromZero, & lowerLimit, & upperLimit);
	MelderInfo_open ();
	MelderInfo_writeLine3 (L"Mean of column ", Table_messageColumn (me, column), L":");
	MelderInfo_writeLine2 (L"Mean = ", Melder_double (mean));
	MelderInfo_writeLine2 (L"Student's t from zero = ", Melder_double (tFromZero));
	MelderInfo_writeLine3 (L"Significance from zero = ", Melder_double (significanceFromZero), L" (one-tailed)");
	MelderInfo_writeLine3 (L"Confidence interval (", Melder_double (100 * (1.0 - 2.0 * significanceLevel)), L"%):");
	MelderInfo_writeLine5 (L"   Lower limit = ", Melder_double (lowerLimit),
		L" (lowest value that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_writeLine5 (L"   Upper limit = ", Melder_double (upperLimit),
		L" (highest value that cannot be rejected with p = ", Melder_double (significanceLevel), L")");
	MelderInfo_close ();
END

FORM (Table_rowsToColumns, "Table: Rows to columns", 0)
	LABEL ("", "Columns with factors (independent variables):")
	TEXTFIELD ("factors", "dialect gender speaker")
	WORD ("Column to transpose", "vowel")
	LABEL ("", "Columns to expand:")
	TEXTFIELD ("columnsToExpand", "duration F0 F1 F2 F3")
	LABEL ("", "Columns not mentioned above will be ignored.")
	OK
DO
	const wchar_t *columnLabel = GET_STRINGW (L"Column to transpose");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, columnLabel);
		if (icol == 0) return Melder_error3 (L"No such column: ", columnLabel, L".");
		if (! praat_new (Table_rowsToColumns (OBJECT,
			GET_STRINGW (L"factors"), icol, GET_STRINGW (L"columnsToExpand")),
			"%s_nested", NAME)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_scatterPlot, "Scatter plot", 0)
	WORD ("Horizontal column", "")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0 (= auto)")
	WORD ("Vertical column", "")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0 (= auto)")
	WORD ("Column with marks", "")
	NATURAL ("Font size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Horizontal column"));
		long ycolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Vertical column"));
		long markColumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Column with marks"));
		if (xcolumn == 0 || ycolumn == 0 || markColumn == 0) return Melder_error ("No such column.");
		Table_scatterPlot (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
			GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
			markColumn, GET_INTEGER ("Font size"), GET_INTEGER ("Garnish"));
	}
	praat_picture_close ();
	return 1;
END

FORM (Table_scatterPlot_mark, "Scatter plot (marks)", 0)
	WORD ("Horizontal column", "")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0 (= auto)")
	WORD ("Vertical column", "")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0 (= auto)")
	POSITIVE ("Mark size (mm)", "1.0")
	BOOLEAN ("Garnish", 1)
	SENTENCE ("Mark string (+xo.)", "+")
	OK
DO
	praat_picture_open ();
	WHERE (SELECTED) {
		Table me = OBJECT;
		long xcolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Horizontal column"));
		long ycolumn = Table_columnLabelToIndex (me, GET_STRINGW (L"Vertical column"));
		Table_scatterPlot_mark (me, GRAPHICS, xcolumn, ycolumn,
			GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
			GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
			GET_REAL ("Mark size"), GET_STRINGW (L"Mark string"), GET_INTEGER ("Garnish"));
	}
	praat_picture_close ();
	return 1;
END

FORM (Table_searchColumn, "Table: Search column", 0)
	WORD ("Column label", "")
	WORD ("Value", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
	if (icol == 0) return Melder_error ("No such column.");
	Melder_information1 (Melder_integer (Table_searchColumn (me, icol, GET_STRINGW (L"Value"))));
END
	
FORM (Table_setColumnLabel_index, "Set column label", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		Table_setColumnLabel (OBJECT, GET_INTEGER ("Column number"), GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_setColumnLabel_label, "Set column label", 0)
	SENTENCE ("Old label", "")
	SENTENCE ("New label", "")
	OK
DO
	WHERE (SELECTED) {
		Table_setColumnLabel (OBJECT, Table_columnLabelToIndex (OBJECT, GET_STRINGW (L"Old label")), GET_STRINGW (L"New label"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (Table_setNumericValue, "Table: Set numeric value", 0)
	NATURAL ("Row number", "1")
	WORD ("Column label", "")
	REAL ("Numeric value", "1.5")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		Table_setNumericValue (me, GET_INTEGER ("Row number"), icol, GET_REAL ("Numeric value"));
		praat_dataChanged (me);
		iferror return 0;
	}
END

FORM (Table_setStringValue, "Table: Set string value", 0)
	NATURAL ("Row number", "1")
	WORD ("Column label", "")
	WORD ("String value", "xx")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		Table_setStringValue (me, GET_INTEGER ("Row number"), icol, GET_STRINGW (L"String value"));
		praat_dataChanged (me);
		iferror return 0;
	}
END

FORM (Table_sortRows, "Table: Sort rows", 0)
	LABEL ("", "One or more column labels for sorting:")
	TEXTFIELD ("columnLabels", "dialect gender name")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		if (! Table_sortRows_string (me, GET_STRINGW (L"columnLabels"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_to_TableOfReal, "Table: Down to TableOfReal", 0)
	WORD ("Column for row labels", "")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRINGW (L"Column for row labels"));
		if (! praat_new (Table_to_TableOfReal (OBJECT, icol), NAME)) return 0;
	}
END

FORM_WRITE (Table_writeToTableFile, "Write Table to table file", 0, "Table")
	if (! Table_writeToTableFile (ONLY_OBJECT, file)) return 0;
END

/***** TABLEOFREAL *****/

DIRECT (TablesOfReal_append)
	Collection me = Collection_create (classTableOfReal, 10);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new (TablesOfReal_appendMany (me), "appended")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

FORM (TableOfReal_create, "Create TableOfReal", 0)
	WORD ("Name", "table")
	NATURAL ("Number of rows", "10")
	NATURAL ("Number of columns", "3")
	OK
DO
	if (! praat_new (TableOfReal_create (GET_INTEGER ("Number of rows"), GET_INTEGER ("Number of columns")),
		GET_STRING ("Name"))) return 0;
END

FORM (TableOfReal_drawAsNumbers, "Draw as numbers", 0)
	NATURAL ("From row", "1")
	INTEGER ("To row", "0 (= all)")
	RADIO ("Format", 3)
	RADIOBUTTON ("decimal")
	RADIOBUTTON ("exponential")
	RADIOBUTTON ("free")
	RADIOBUTTON ("rational")
	NATURAL ("Precision", "5")
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsNumbers (OBJECT, GRAPHICS,
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("Format"), GET_INTEGER ("Precision")))
END

FORM (TableOfReal_drawAsNumbers_if, "Draw as numbers if...", 0)
	NATURAL ("From row", "1")
	INTEGER ("To row", "0 (= all)")
	RADIO ("Format", 3)
	RADIOBUTTON ("decimal")
	RADIOBUTTON ("exponential")
	RADIOBUTTON ("free")
	RADIOBUTTON ("rational")
	NATURAL ("Precision", "5")
	LABEL ("", "Condition:")
	TEXTFIELD ("condition", "self <> 0")
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsNumbers_if (OBJECT, GRAPHICS,
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("Format"), GET_INTEGER ("Precision"), GET_STRINGW (L"condition")))
END

FORM (TableOfReal_drawAsSquares, "Draw table as squares", 0)
	INTEGER ("From row", "1")
	INTEGER ("To row", "0")
	INTEGER ("From column", "1")
	INTEGER ("To column", "0")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TableOfReal_drawAsSquares (OBJECT, GRAPHICS, 
		GET_INTEGER ("From row"), GET_INTEGER ("To row"),
		GET_INTEGER ("From column"), GET_INTEGER ("To column"),
		GET_INTEGER ("Garnish")))
END

FORM (TableOfReal_drawHorizontalLines, "Draw horizontal lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawHorizontalLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawLeftAndRightLines, "Draw left and right lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawLeftAndRightLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawTopAndBottomLines, "Draw top and bottom lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawTopAndBottomLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END
FORM (TableOfReal_drawVerticalLines, "Draw vertical lines", 0)
	NATURAL ("From row", "1") INTEGER ("To row", "0 (= all)") OK DO
	EVERY_DRAW (TableOfReal_drawVerticalLines (OBJECT, GRAPHICS, GET_INTEGER ("From row"), GET_INTEGER ("To row"))) END

DIRECT (TableOfReal_extractColumnLabelsAsStrings)
	EVERY_TO (TableOfReal_extractColumnLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractColumnRanges, "Extract column ranges", 0)
	LABEL ("", "Create a new TableOfReal from the following columns:")
	TEXTFIELD ("ranges", "1 2")
	LABEL ("", "To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnRanges (OBJECT, GET_STRINGW (L"ranges")), "%s_cols", NAME)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhere, "Extract columns where", 0)
	LABEL ("", "Extract all columns with at least one cell where:")
	TEXTFIELD ("condition", "col mod 3 = 0 ; this example extracts every third column")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhere (OBJECT, GET_STRINGW (L"condition")), "%s_cols", NAME)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhereLabel, "Extract column where label", 0)
	OPTIONMENU ("Extract all columns whose label...", 1)
	OPTIONS_ENUMW (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "a")
	OK
DO
	const wchar_t *text = GET_STRINGW (L"...the text");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhereLabel (OBJECT,
			GET_INTEGER ("Extract all columns whose label...") - 1 + Melder_STRING_min, text),
			"%s_%ls", NAME, text)) return 0;
	}
END

FORM (TableOfReal_extractColumnsWhereRow, "Extract columns where row", 0)
	NATURAL ("Extract all columns where row...", "1")
	OPTIONMENU ("...is...", 1)
	OPTIONS_ENUMW (Melder_NUMBER_text_adjective (itext), Melder_NUMBER_min, Melder_NUMBER_max)
	REAL ("...the value", "0.0")
	OK
DO
	long row = GET_INTEGER ("Extract all columns where row...");
	double value = GET_REAL ("...the value");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractColumnsWhereRow (OBJECT,
			row, GET_INTEGER ("...is...") - 1 + Melder_NUMBER_min, value),
			"%s_%ld_%ld", NAME, row, (long) floor (value+0.5))) return 0;
	}
END

DIRECT (TableOfReal_extractRowLabelsAsStrings)
	EVERY_TO (TableOfReal_extractRowLabelsAsStrings (OBJECT))
END

FORM (TableOfReal_extractRowRanges, "Extract row ranges", 0)
	LABEL ("", "Create a new TableOfReal from the following rows:")
	TEXTFIELD ("ranges", "1 2")
	LABEL ("", "To supply rising or falling ranges, use e.g. 2:6 or 5:3.")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowRanges (OBJECT, GET_STRINGW (L"ranges")), "%s_rows", NAME)) return 0;
	}
END

FORM (TableOfReal_extractRowsWhere, "Extract rows where", 0)
	LABEL ("", "Extract all rows with at least one cell where:")
	TEXTFIELD ("condition", "row mod 3 = 0 ; this example extracts every third row")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhere (OBJECT, GET_STRINGW (L"condition")), "%s_rows", NAME)) return 0;
	}
END

FORM (TableOfReal_extractRowsWhereColumn, "Extract rows where column", 0)
	NATURAL ("Extract all rows where column...", "1")
	OPTIONMENU ("...is...", 1)
	OPTIONS_ENUMW (Melder_NUMBER_text_adjective (itext), Melder_NUMBER_min, Melder_NUMBER_max)
	REAL ("...the value", "0.0")
	OK
DO
	long column = GET_INTEGER ("Extract all rows where column...");
	double value = GET_REAL ("...the value");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhereColumn (OBJECT,
			column, GET_INTEGER ("...is...") - 1 + Melder_NUMBER_min, value),
			"%s_%ld_%ld", NAME, column, (long) floor (value+0.5))) return 0;
	}
END

FORM (TableOfReal_extractRowsWhereLabel, "Extract rows where label", 0)
	OPTIONMENU ("Extract all rows whose label...", 1)
	OPTIONS_ENUMW (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "a")
	OK
DO
	const wchar_t *text = GET_STRINGW (L"...the text");
	WHERE (SELECTED) {
		if (! praat_new (TableOfReal_extractRowsWhereLabel (OBJECT,
			GET_INTEGER ("Extract all rows whose label...") - 1 + Melder_STRING_min, text),
			"%s_%ls", NAME, text)) return 0;
	}
END

FORM (TableOfReal_formula, "TableOfReal: Formula", "Formula...")
	LABEL ("", "for row from 1 to nrow do for col from 1 to ncol do self [row, col] = ...")
	TEXTFIELD ("formula", "if col = 5 then self + self [6] else self fi")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_formula (OBJECT, GET_STRINGW (L"formula"), NULL)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_getColumnIndex, "Get column index", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Melder_information1 (Melder_integer (TableOfReal_columnLabelToIndex (ONLY_OBJECT, GET_STRINGW (L"Column label"))));
END
	
FORM (TableOfReal_getColumnLabel, "Get column label", 0)
	NATURAL ("Column number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER ("Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information1 (table -> columnLabels == NULL ? L"" : table -> columnLabels [columnNumber]);
END
	
FORM (TableOfReal_getColumnMean_index, "Get column mean", 0)
	NATURAL ("Column number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = GET_INTEGER ("Column number");
	REQUIRE (columnNumber <= table -> numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnMean_label, "Get column mean", 0)
	SENTENCE ("Column label", "")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRINGW (L"Column label"));
	REQUIRE (columnNumber > 0, "Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnMean (table, columnNumber), NULL);
END
	
FORM (TableOfReal_getColumnStdev_index, "Get column standard deviation", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Melder_informationReal (TableOfReal_getColumnStdev (ONLY_OBJECT, GET_INTEGER ("Column number")), NULL);
END
	
FORM (TableOfReal_getColumnStdev_label, "Get column standard deviation", 0)
	SENTENCE ("Column label", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long columnNumber = TableOfReal_columnLabelToIndex (table, GET_STRINGW (L"Column label"));
	REQUIRE (columnNumber > 0, "Column label does not exist.")
	Melder_informationReal (TableOfReal_getColumnStdev (table, columnNumber), NULL);
END

DIRECT (TableOfReal_getNumberOfColumns) TableOfReal me = ONLY_OBJECT; Melder_information1 (Melder_integer (my numberOfColumns)); END
DIRECT (TableOfReal_getNumberOfRows) TableOfReal me = ONLY_OBJECT; Melder_information1 (Melder_integer (my numberOfRows)); END

FORM (TableOfReal_getRowIndex, "Get row index", 0)
	SENTENCE ("Row label", "")
	OK
DO
	Melder_information1 (Melder_integer (TableOfReal_rowLabelToIndex (ONLY_OBJECT, GET_STRINGW (L"Row label"))));
END
	
FORM (TableOfReal_getRowLabel, "Get row label", 0)
	NATURAL ("Row number", "1")
	OK
DO
	TableOfReal table = ONLY_OBJECT;
	long rowNumber = GET_INTEGER ("Row number");
	REQUIRE (rowNumber <= table -> numberOfRows, "Row number must not be greater than number of rows.")
	Melder_information1 (table -> rowLabels == NULL ? L"" : table -> rowLabels [rowNumber]);
END

FORM (TableOfReal_getValue, "Get value", 0)
	NATURAL ("Row number", "1") NATURAL ("Column number", "1") OK DO TableOfReal me = ONLY_OBJECT;
	long row = GET_INTEGER ("Row number"), column = GET_INTEGER ("Column number");
	REQUIRE (row <= my numberOfRows, "Row number must not exceed number of rows.")
	REQUIRE (column <= my numberOfColumns, "Column number must not exceed number of columns.")
	Melder_informationReal (my data [row] [column], NULL); END

DIRECT (TableOfReal_help) Melder_help (L"TableOfReal"); END

FORM (TableOfReal_insertColumn, "Insert column", 0)
	NATURAL ("Column number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertColumn (OBJECT, GET_INTEGER ("Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_insertRow, "Insert row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_insertRow (OBJECT, GET_INTEGER ("Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM_READ (TableOfReal_readFromHeaderlessSpreadsheetFile, "Read TableOfReal from headerless spreadsheet file", 0)
	if (! praat_new1 (TableOfReal_readFromHeaderlessSpreadsheetFile (file), MelderFile_name (file))) return 0;
END

FORM (TableOfReal_removeColumn, "Remove column", 0)
	NATURAL ("Column number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeColumn (OBJECT, GET_INTEGER ("Column number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_removeRow, "Remove row", 0)
	NATURAL ("Row number", "1")
	OK
DO
	WHERE (SELECTED) {
		if (! TableOfReal_removeRow (OBJECT, GET_INTEGER ("Row number"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_index, "Set column label", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, GET_INTEGER ("Column number"), GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setColumnLabel_label, "Set column label", 0)
	SENTENCE ("Old label", "")
	SENTENCE ("New label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setColumnLabel (OBJECT, TableOfReal_columnLabelToIndex (OBJECT, GET_STRINGW (L"Old label")),
			GET_STRINGW (L"New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setRowLabel_index, "Set row label", 0)
	NATURAL ("Row number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, GET_INTEGER ("Row number"), GET_STRINGW (L"Label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_setValue, "Set value", "TableOfReal: Set value...")
	NATURAL ("Row number", "1")
	NATURAL ("Column number", "1")
	REAL ("New value", "0.0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal me = OBJECT;
		long irow = GET_INTEGER ("Row number"), icol = GET_INTEGER ("Column number");
		REQUIRE (irow <= my numberOfRows, "Row number too large.")
		REQUIRE (icol <= my numberOfColumns, "Column number too large.")
		my data [irow] [icol] = GET_REAL ("New value");
		praat_dataChanged (me);
	}
END

FORM (TableOfReal_setRowLabel_label, "Set row label", 0)
	SENTENCE ("Old label", "")
	SENTENCE ("New label", "")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_setRowLabel (OBJECT, TableOfReal_rowLabelToIndex (OBJECT, GET_STRINGW (L"Old label")),
			GET_STRINGW (L"New label"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByColumn, "Sort rows by column", 0)
	INTEGER ("Column", "1")
	INTEGER ("Secondary column", "0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByColumn (OBJECT, GET_INTEGER ("Column"), GET_INTEGER ("Secondary column"));
		praat_dataChanged (OBJECT);
	}
END

FORM (TableOfReal_sortByLabel, "Sort rows by label", 0)
	LABEL ("", "Secondary sorting keys:")
	INTEGER ("Column1", "1")
	INTEGER ("Column2", "0")
	OK
DO
	WHERE (SELECTED) {
		TableOfReal_sortByLabel (OBJECT, GET_INTEGER ("Column1"), GET_INTEGER ("Column2"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TableOfReal_to_Matrix)
	EVERY_TO (TableOfReal_to_Matrix (OBJECT))
END

FORM (TableOfReal_to_Table, "TableOfReal: To Table", 0)
	SENTENCE ("Label of first column", "rowLabel")
	OK
DO
	EVERY_TO (TableOfReal_to_Table (OBJECT, GET_STRINGW (L"Label of first column")))
END

FORM_WRITE (TableOfReal_writeToHeaderlessSpreadsheetFile, "Write TableOfReal to spreadsheet", 0, "txt")
	if (! TableOfReal_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END


DIRECT (StatisticsTutorial) Melder_help (L"Statistics"); END

static Any tabSeparatedFileRecognizer (int nread, const char *header, MelderFile file) {
	/*
	 * A table is recognized if it has at least one tab symbol,
	 * which must be before the first newline symbol (if any).
	 */
	const char *tab = strchr (header, '\t'), *newline = strchr (header, '\n');
	(void) nread;
	if (newline == NULL) newline = strchr (header, '\r');
	if (tab == NULL || (newline != NULL && newline - tab < 0)) return NULL;
	return Table_readFromCharacterSeparatedTextFile (file, '\t');
}

void praat_TableOfReal_init (void *klas);   /* Buttons for TableOfReal and for its subclasses. */
void praat_TableOfReal_init (void *klas) {
	praat_addAction1 (klas, 1, "Write to headerless spreadsheet file...", 0, 0, DO_TableOfReal_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (klas, 0, "Draw -                 ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Draw as numbers...", 0, 1, DO_TableOfReal_drawAsNumbers);
		praat_addAction1 (klas, 0, "Draw as numbers if...", 0, 1, DO_TableOfReal_drawAsNumbers_if);
		praat_addAction1 (klas, 0, "Draw as squares...", 0, 1, DO_TableOfReal_drawAsSquares);	
		praat_addAction1 (klas, 0, "-- draw lines --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Draw vertical lines...", 0, 1, DO_TableOfReal_drawVerticalLines);
		praat_addAction1 (klas, 0, "Draw horizontal lines...", 0, 1, DO_TableOfReal_drawHorizontalLines);
		praat_addAction1 (klas, 0, "Draw left and right lines...", 0, 1, DO_TableOfReal_drawLeftAndRightLines);
		praat_addAction1 (klas, 0, "Draw top and bottom lines...", 0, 1, DO_TableOfReal_drawTopAndBottomLines);
	praat_addAction1 (klas, 0, "Query -                ", 0, 0, 0);
		praat_addAction1 (klas, 1, "Get number of rows", 0, 1, DO_TableOfReal_getNumberOfRows);
		praat_addAction1 (klas, 1, "Get number of columns", 0, 1, DO_TableOfReal_getNumberOfColumns);
		praat_addAction1 (klas, 1, "Get row label...", 0, 1, DO_TableOfReal_getRowLabel);
		praat_addAction1 (klas, 1, "Get column label...", 0, 1, DO_TableOfReal_getColumnLabel);
		praat_addAction1 (klas, 1, "Get row index...", 0, 1, DO_TableOfReal_getRowIndex);
		praat_addAction1 (klas, 1, "Get column index...", 0, 1, DO_TableOfReal_getColumnIndex);
		praat_addAction1 (klas, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (klas, 1, "Get value...", 0, 1, DO_TableOfReal_getValue);
		if (klas == classTableOfReal) {
			praat_addAction1 (klas, 1, "-- get statistics --", 0, 1, 0);
			praat_addAction1 (klas, 1, "Get column mean (index)...", 0, 1, DO_TableOfReal_getColumnMean_index);
			praat_addAction1 (klas, 1, "Get column mean (label)...", 0, 1, DO_TableOfReal_getColumnMean_label);
			praat_addAction1 (klas, 1, "Get column stdev (index)...", 0, 1, DO_TableOfReal_getColumnStdev_index);
			praat_addAction1 (klas, 1, "Get column stdev (label)...", 0, 1, DO_TableOfReal_getColumnStdev_label);
		}
	praat_addAction1 (klas, 0, "Modify -               ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Formula...", 0, 1, DO_TableOfReal_formula);
		praat_addAction1 (klas, 0, "Set value...", 0, 1, DO_TableOfReal_setValue);
		praat_addAction1 (klas, 0, "Sort by label...", 0, 1, DO_TableOfReal_sortByLabel);
		praat_addAction1 (klas, 0, "Sort by column...", 0, 1, DO_TableOfReal_sortByColumn);
		praat_addAction1 (klas, 0, "-- structure --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Remove row (index)...", 0, 1, DO_TableOfReal_removeRow);
		praat_addAction1 (klas, 0, "Remove column (index)...", 0, 1, DO_TableOfReal_removeColumn);
		praat_addAction1 (klas, 0, "Insert row (index)...", 0, 1, DO_TableOfReal_insertRow);
		praat_addAction1 (klas, 0, "Insert column (index)...", 0, 1, DO_TableOfReal_insertColumn);
		praat_addAction1 (klas, 0, "-- set --", 0, 1, 0);
		praat_addAction1 (klas, 0, "Set row label (index)...", 0, 1, DO_TableOfReal_setRowLabel_index);
		praat_addAction1 (klas, 0, "Set row label (label)...", 0, 1, DO_TableOfReal_setRowLabel_label);
		praat_addAction1 (klas, 0, "Set column label (index)...", 0, 1, DO_TableOfReal_setColumnLabel_index);
		praat_addAction1 (klas, 0, "Set column label (label)...", 0, 1, DO_TableOfReal_setColumnLabel_label);
	praat_addAction1 (klas, 0, "Synthesize -     ", 0, 0, 0);
		praat_addAction1 (klas, 0, "Append", 0, 1, DO_TablesOfReal_append);
	praat_addAction1 (klas, 0, "Extract part -", 0, 0, 0);
		praat_addAction1 (klas, 0, "Extract row ranges...", 0, 1, DO_TableOfReal_extractRowRanges);
		praat_addAction1 (klas, 0, "Extract rows where column...", 0, 1, DO_TableOfReal_extractRowsWhereColumn);
		praat_addAction1 (klas, 0, "Extract rows where label...", 0, 1, DO_TableOfReal_extractRowsWhereLabel);
		praat_addAction1 (klas, 0, "Extract rows where...", 0, 1, DO_TableOfReal_extractRowsWhere);
		praat_addAction1 (klas, 0, "Extract column ranges...", 0, 1, DO_TableOfReal_extractColumnRanges);
		praat_addAction1 (klas, 0, "Extract columns where row...", 0, 1, DO_TableOfReal_extractColumnsWhereRow);
		praat_addAction1 (klas, 0, "Extract columns where label...", 0, 1, DO_TableOfReal_extractColumnsWhereLabel);
		praat_addAction1 (klas, 0, "Extract columns where...", 0, 1, DO_TableOfReal_extractColumnsWhere);
	praat_addAction1 (klas, 0, "Extract -", 0, 0, 0);
		praat_addAction1 (klas, 0, "Extract row labels as Strings", 0, 1, DO_TableOfReal_extractRowLabelsAsStrings);
		praat_addAction1 (klas, 0, "Extract column labels as Strings", 0, 1, DO_TableOfReal_extractColumnLabelsAsStrings);
	praat_addAction1 (klas, 0, "Convert -     ", 0, 0, 0);
		praat_addAction1 (klas, 0, "To Table...", 0, 1, DO_TableOfReal_to_Table);
		praat_addAction1 (klas, 0, "To Matrix", 0, 1, DO_TableOfReal_to_Matrix);
}

void praat_uvafon_Stat_init (void);
void praat_uvafon_Stat_init (void) {

	Thing_recognizeClassesByName (classTableOfReal, classDistributions, classPairDistribution,
		classTable, classLinearRegression, classLogisticRegression, NULL);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	praat_addMenuCommand ("Objects", "New", "Tables", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "Create Table with column names...", 0, 1, DO_Table_createWithColumnNames);
		praat_addMenuCommand ("Objects", "New", "Create Table without column names...", 0, 1, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand ("Objects", "New", "Create Table...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_createWithoutColumnNames);
		praat_addMenuCommand ("Objects", "New", "Create TableOfReal...", 0, 1, DO_TableOfReal_create);

	praat_addMenuCommand ("Objects", "Read", "Read TableOfReal from headerless spreadsheet file...", 0, 0, DO_TableOfReal_readFromHeaderlessSpreadsheetFile);
	praat_addMenuCommand ("Objects", "Read", "Read Table from table file...", 0, 0, DO_Table_readFromTableFile);
	praat_addMenuCommand ("Objects", "Read", "Read Table from comma-separated file...", 0, 0, DO_Table_readFromCommaSeparatedFile);
	praat_addMenuCommand ("Objects", "Read", "Read Table from tab-separated file...", 0, 0, DO_Table_readFromTabSeparatedFile);

	praat_addAction1 (classDistributions, 0, "Distributions help", 0, 0, DO_Distributions_help);
	praat_TableOfReal_init (classDistributions);
	praat_addAction1 (classDistributions, 1, "Get probability (label)...", "Get value...", 1, DO_Distributions_getProbability);
	praat_addAction1 (classDistributions, 0, "-- get from two --", "Get probability (label)...", 1, 0);
	praat_addAction1 (classDistributions, 2, "Get mean absolute difference...", "-- get from two --", 1, DO_Distributionses_getMeanAbsoluteDifference);
	praat_addAction1 (classDistributions, 0, "-- add --", "Append", 1, 0);
	praat_addAction1 (classDistributions, 0, "Add", "-- add --", 1, DO_Distributionses_add);
	praat_addAction1 (classDistributions, 0, "Generate", 0, 0, 0);
		praat_addAction1 (classDistributions, 0, "To Strings...", 0, 0, DO_Distributions_to_Strings);
		praat_addAction1 (classDistributions, 0, "To Strings (exact)...", 0, 0, DO_Distributions_to_Strings_exact);

	praat_addAction1 (classPairDistribution, 0, "PairDistribution help", 0, 0, DO_PairDistribution_help);
	praat_addAction1 (classPairDistribution, 0, "To Table", 0, 0, DO_PairDistribution_to_Table);
	praat_addAction1 (classPairDistribution, 1, "To Stringses...", 0, 0, DO_PairDistribution_to_Stringses);
	praat_addAction1 (classPairDistribution, 0, "Query -          ", 0, 0, 0);
		praat_addAction1 (classPairDistribution, 1, "Get number of pairs", 0, 1, DO_PairDistribution_getNumberOfPairs);
		praat_addAction1 (classPairDistribution, 1, "Get string1...", 0, 1, DO_PairDistribution_getString1);
		praat_addAction1 (classPairDistribution, 1, "Get string2...", 0, 1, DO_PairDistribution_getString2);
		praat_addAction1 (classPairDistribution, 1, "Get weight...", 0, 1, DO_PairDistribution_getWeight);
		praat_addAction1 (classPairDistribution, 1, "-- get fraction correct --", 0, 1, 0);
		praat_addAction1 (classPairDistribution, 1, "Get fraction correct (maximum likelihood)", 0, 1, DO_PairDistribution_getFractionCorrect_maximumLikelihood);
		praat_addAction1 (classPairDistribution, 1, "Get fraction correct (probability matching)", 0, 1, DO_PairDistribution_getFractionCorrect_probabilityMatching);
	praat_addAction1 (classPairDistribution, 0, "Modify -          ", 0, 0, 0);
	praat_addAction1 (classPairDistribution, 1, "Remove zero weights", 0, 0, DO_PairDistribution_removeZeroWeights);

	praat_addAction1 (classTable, 0, "Table help", 0, 0, DO_Table_help);
	praat_addAction1 (classTable, 1, "Write to table file...", 0, 0, DO_Table_writeToTableFile);
	praat_addAction1 (classTable, 1, "Edit", 0, 0, DO_Table_edit);
	praat_addAction1 (classTable, 0, "Draw -                ", 0, 0, 0);
		praat_addAction1 (classTable, 0, "Scatter plot...", 0, 1, DO_Table_scatterPlot);
		praat_addAction1 (classTable, 0, "Scatter plot (mark)...", 0, 1, DO_Table_scatterPlot_mark);
		praat_addAction1 (classTable, 0, "Draw ellipse (standard deviation)...", 0, 1, DO_Table_drawEllipse);
	praat_addAction1 (classTable, 0, "Query -                ", 0, 0, 0);
		praat_addAction1 (classTable, 1, "List...", 0, 1, DO_Table_list);
		praat_addAction1 (classTable, 1, "-- get structure --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Get number of rows", 0, 1, DO_Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, "Get number of columns", 0, 1, DO_Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, "Get column label...", 0, 1, DO_Table_getColumnLabel);
		praat_addAction1 (classTable, 1, "Get column index...", 0, 1, DO_Table_getColumnIndex);
		praat_addAction1 (classTable, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Get value...", 0, 1, DO_Table_getValue);
		praat_addAction1 (classTable, 1, "Search column...", 0, 1, DO_Table_searchColumn);
		praat_addAction1 (classTable, 1, "-- statistics --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Statistics tutorial", 0, 1, DO_StatisticsTutorial);
		praat_addAction1 (classTable, 1, "-- get stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Get quantile...", 0, 1, DO_Table_getQuantile);
		praat_addAction1 (classTable, 1, "Get mean...", 0, 1, DO_Table_getMean);
		praat_addAction1 (classTable, 1, "Get group mean...", 0, 1, DO_Table_getGroupMean);
		praat_addAction1 (classTable, 1, "Get standard deviation...", 0, 1, DO_Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, "-- report stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Report mean (Student t)...", 0, 1, DO_Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, "Report standard deviation...", 0, 1, DO_Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, "Report difference (Student t)...", 0, 1, DO_Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, "Report group mean (Student t)...", 0, 1, DO_Table_reportGroupMean_studentT);
		praat_addAction1 (classTable, 1, "Report group difference (Student t)...", 0, 1, DO_Table_reportGroupDifference_studentT);
		praat_addAction1 (classTable, 1, "Report correlation (Pearson r)...", 0, 1, DO_Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, "Report correlation (Kendall tau)...", 0, 1, DO_Table_reportCorrelation_kendallTau);
		praat_addAction1 (classTable, 1, "-- to regression --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "To linear regression", 0, 1, DO_Table_to_LinearRegression);
		praat_addAction1 (classTable, 1, "To logistic regression", 0, 1, DO_Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, "Modify -        ", 0, 0, 0);
		praat_addAction1 (classTable, 0, "Set string value...", 0, 1, DO_Table_setStringValue);
		praat_addAction1 (classTable, 0, "Set numeric value...", 0, 1, DO_Table_setNumericValue);
		praat_addAction1 (classTable, 0, "Formula...", 0, 1, DO_Table_formula);
		praat_addAction1 (classTable, 0, "Formula (column range)...", 0, 1, DO_Table_formula_columnRange);
		praat_addAction1 (classTable, 0, "Sort rows...", 0, 1, DO_Table_sortRows);
		praat_addAction1 (classTable, 0, "-- structure --", 0, 1, 0);
		praat_addAction1 (classTable, 0, "Append row", 0, 1, DO_Table_appendRow);
		praat_addAction1 (classTable, 0, "Append column...", 0, 1, DO_Table_appendColumn);
		praat_addAction1 (classTable, 0, "Append sum column...", 0, 1, DO_Table_appendSumColumn);
		praat_addAction1 (classTable, 0, "Append difference column...", 0, 1, DO_Table_appendDifferenceColumn);
		praat_addAction1 (classTable, 0, "Append product column...", 0, 1, DO_Table_appendProductColumn);
		praat_addAction1 (classTable, 0, "Append quotient column...", 0, 1, DO_Table_appendQuotientColumn);
		praat_addAction1 (classTable, 0, "Remove row...", 0, 1, DO_Table_removeRow);
		praat_addAction1 (classTable, 0, "Remove column...", 0, 1, DO_Table_removeColumn);
		praat_addAction1 (classTable, 0, "Insert row...", 0, 1, DO_Table_insertRow);
		praat_addAction1 (classTable, 0, "Insert column...", 0, 1, DO_Table_insertColumn);
		praat_addAction1 (classTable, 0, "-- set --", 0, 1, 0);
		praat_addAction1 (classTable, 0, "Set column label (index)...", 0, 1, DO_Table_setColumnLabel_index);
		praat_addAction1 (classTable, 0, "Set column label (label)...", 0, 1, DO_Table_setColumnLabel_label);
	praat_addAction1 (classTable, 0, "Extract -     ", 0, 0, 0);
		praat_addAction1 (classTable, 0, "Extract rows where column (number)...", 0, 1, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, "Extract rows where column...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, "Select rows where column...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Table_extractRowsWhereColumn_number);
		praat_addAction1 (classTable, 0, "Extract rows where column (text)...", 0, 1, DO_Table_extractRowsWhereColumn_text);
		praat_addAction1 (classTable, 0, "Collapse rows...", 0, 1, DO_Table_collapseRows);
		praat_addAction1 (classTable, 0, "Rows to columns...", 0, 1, DO_Table_rowsToColumns);
	praat_addAction1 (classTable, 0, "Down to TableOfReal...", 0, 0, DO_Table_to_TableOfReal);

	praat_addAction1 (classTableOfReal, 0, "TableOfReal help", 0, 0, DO_TableOfReal_help);
	praat_TableOfReal_init (classTableOfReal);

	praat_addAction2 (classPairDistribution, 1, classDistributions, 1, "Get fraction correct...", 0, 0, DO_PairDistribution_Distributions_getFractionCorrect);
}

/* End of file praat_Stat.c */
