/* praat_Stat.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/04/28
 */

#include "praat.h"

#include "Table.h"
#include "TableEditor.h"
#include "Regression.h"

static char formatBuffer [32] [40];
static int formatIndex = 0;
static char * Table_messageColumn (Table me, long column) {
	if (++ formatIndex == 32) formatIndex = 0;
	if (my columnHeaders [column]. label != NULL && my columnHeaders [column]. label [0] != '\0')
		sprintf (formatBuffer [formatIndex], "\"%.39s\"", my columnHeaders [column]. label);
	else
		sprintf (formatBuffer [formatIndex], "%ld", column);
	return formatBuffer [formatIndex];
}

/***** TABLE *****/

FORM (Table_appendColumn, "Table: Append column", 0)
	WORD ("Label", "newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_appendColumn (OBJECT, GET_STRING ("Label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendDifferenceColumn (OBJECT, icol, jcol, GET_STRING ("Label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendProductColumn (OBJECT, icol, jcol, GET_STRING ("Label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendQuotientColumn (OBJECT, icol, jcol, GET_STRING ("Label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
		long jcol = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
		if (icol == 0 || jcol == 0) return Melder_error ("No such column.");
		Table_appendSumColumn (OBJECT, icol, jcol, GET_STRING ("Label"));
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

DIRECT (Table_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a Table from batch.");
	} else {
		WHERE (SELECTED) {
			TableEditor editor = TableEditor_create (praat.topShell, FULL_NAME, ONLY_OBJECT);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
		}
	}
END

FORM (Table_extractRowsWhereColumn_number, "Table: Extract rows where column (number)", 0)
	WORD ("Extract all rows where column...", "")
	RADIO ("...is...", 1)
	RADIOBUTTONS_ENUM (Melder_NUMBER_text_adjective (itext), Melder_NUMBER_min, Melder_NUMBER_max)
	REAL ("...the number", "0.0")
	OK
DO
	double value = GET_REAL ("...the number");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Extract all rows where column..."));
		if (icol == 0) return Melder_error ("No such column.");
		if (! praat_new (Table_extractRowsWhereColumn_number (OBJECT,
			icol, GET_INTEGER ("...is...") - 1 + Melder_NUMBER_min, value),
			"%s_%s_%ld", NAME, Table_messageColumn (OBJECT, icol),
			(long) floor (value+0.5))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_extractRowsWhereColumn_text, "Table: Extract rows where column (text)", 0)
	WORD ("Extract all rows where column...", "")
	OPTIONMENU ("...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "hi")
	OK
DO
	const char *value = GET_STRING ("...the text");
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Extract all rows where column..."));
		if (icol == 0) return Melder_error ("No such column.");
		if (! praat_new (Table_extractRowsWhereColumn_string (OBJECT,
			icol, GET_INTEGER ("...") - 1 + Melder_STRING_min, value),
			"%s_%s", NAME, value)) return 0;
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		if (! Table_formula (OBJECT, icol, GET_STRING ("formula"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_getColumnIndex, "Table: Get column index", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Melder_information ("%ld", Table_columnLabelToIndex (ONLY_OBJECT, GET_STRING ("Column label")));
END
	
FORM (Table_getColumnLabel, "Table: Get column label", 0)
	NATURAL ("Column number", "1")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = GET_INTEGER ("Column number");
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information ("%s", my columnHeaders [icol]. label == NULL ? "" : my columnHeaders [icol]. label);
END

FORM (Table_getMean, "Table: Get mean", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information ("%s", Melder_double (Table_getMean (ONLY_OBJECT, icol)));
END
	
FORM (Table_getQuantile, "Table: Get quantile", 0)
	SENTENCE ("Column label", "")
	POSITIVE ("Quantile", "0.50 (= median)")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information ("%s", Melder_double (Table_getQuantile (ONLY_OBJECT, icol, GET_REAL ("Quantile"))));
END
	
FORM (Table_getStandardDeviation, "Table: Get standard deviation", 0)
	SENTENCE ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
	REQUIRE (icol > 0, "No such column.")
	REQUIRE (icol <= my numberOfColumns, "Column number must not be greater than number of columns.")
	Melder_information ("%s", Melder_double (Table_getStdev (ONLY_OBJECT, icol)));
END
	
DIRECT (Table_to_LinearRegression)
	EVERY_TO (Table_to_LinearRegression (OBJECT))
END

DIRECT (Table_to_LogisticRegression)
	EVERY_TO (Table_to_LogisticRegression (OBJECT))
END

DIRECT (Table_getNumberOfColumns)
	Melder_information ("%ld", ((Table) ONLY_OBJECT) -> numberOfColumns);
END

DIRECT (Table_getNumberOfRows)
	Melder_information ("%ld", ((Table) ONLY_OBJECT) -> rows -> size);
END

FORM (Table_getValue, "Table: Get value", 0)
	NATURAL ("Row number", "1")
	WORD ("Column label", "")
	OK
DO
	Table me = ONLY_OBJECT;
	long irow = GET_INTEGER ("Row number");
	long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
	char *value;
	if (icol == 0) return Melder_error ("No such column.");
	REQUIRE (irow >= 1 && irow <= my rows -> size, "Row number out of range.")
	REQUIRE (icol >= 1 && icol <= my numberOfColumns, "Column number out of range.")
	value = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	Melder_information ("%s", value == NULL ? "" : value);
END

DIRECT (Table_help) Melder_help ("Table"); END

FORM (Table_insertColumn, "Table: Insert column", 0)
	NATURAL ("Position", "1")
	WORD ("Label", "newcolumn")
	OK
DO
	WHERE (SELECTED) {
		Table_insertColumn (OBJECT, GET_INTEGER ("Position"), GET_STRING ("Label"));
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

FORM (Table_pool, "Table: Pool", 0)
	LABEL ("", "Columns with factors (independent variables):")
	TEXTFIELD ("independentVariables", "speaker dialect age vowel")
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
		if (! praat_new (Table_pool (OBJECT,
			GET_STRING ("independentVariables"), GET_STRING ("columnsToSum"),
			GET_STRING ("columnsToAverage"), GET_STRING ("columnsToMedianize"),
			GET_STRING ("columnsToAverageLogarithmically"), GET_STRING ("columnsToMedianizeLogarithmically")),
			"%s_pooled", NAME)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (Table_removeColumn, "Table: Remove column", 0)
	WORD ("Column label", "")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
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
	long column1 = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double correlation, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	correlation = Table_getCorrelation_kendallTau (me, column1, column2, significanceLevel,
		& significance, & lowerLimit, & upperLimit);
	Melder_information ("Correlation between column %s and column %s:\n"
		"Correlation = %s (Kendall's tau-b)\n"
		"Significance from zero = %s (one-tailed)\n"
		"Confidence interval (%f%%):\n"
		"   Lower limit = %s (lowest tau that cannot be rejected with p = %f)\n"
		"   Upper limit = %s (highest tau that cannot be rejected with p = %f)\n",
		Table_messageColumn (me, column1), Table_messageColumn (me, column2),
		Melder_double (correlation), Melder_double (significance),
		100 * (1.0 - 2.0 * significanceLevel),
		Melder_double (lowerLimit), significanceLevel,
		Melder_double (upperLimit), significanceLevel);
END

FORM (Table_reportCorrelation_pearsonR, "Report correlation (Pearson r)", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double correlation, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	correlation = Table_getCorrelation_pearsonR (me, column1, column2, significanceLevel,
		& significance, & lowerLimit, & upperLimit);
	Melder_information ("Correlation between column %s and column %s:\n"
		"Correlation = %s (Pearson's r)\n"
		"Significance from zero = %s (one-tailed)\n"
		"Confidence interval (%f%%):\n"
		"   Lower limit = %s (lowest r that cannot be rejected with p = %f)\n"
		"   Upper limit = %s (highest r that cannot be rejected with p = %f)\n",
		Table_messageColumn (me, column1), Table_messageColumn (me, column2),
		Melder_double (correlation), Melder_double (significance),
		100 * (1.0 - 2.0 * significanceLevel),
		Melder_double (lowerLimit), significanceLevel,
		Melder_double (upperLimit), significanceLevel);
END
	
FORM (Table_reportDifference_studentT, "Report difference (Student t)", 0)
	WORD ("left Columns", "")
	WORD ("right Columns", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column1 = Table_columnLabelToIndex (me, GET_STRING ("left Columns"));
	long column2 = Table_columnLabelToIndex (me, GET_STRING ("right Columns"));
	double significanceLevel = GET_REAL ("Significance level");
	double difference, t, significance, lowerLimit, upperLimit;
	if (column1 == 0 || column2 == 0) return Melder_error ("No such column.");
	difference = Table_getDifference_studentT (me, column1, column2, significanceLevel,
		& t, & significance, & lowerLimit, & upperLimit);
	Melder_information ("Difference between column %s and column %s:\n"
		"Difference = %s\n"
		"Student's t = %s\n"
		"Significance from zero = %s (one-tailed)\n"
		"Confidence interval (%f%%):\n"
		"   Lower limit = %s (lowest difference that cannot be rejected with p = %f)\n"
		"   Upper limit = %s (highest difference that cannot be rejected with p = %f)\n",
		Table_messageColumn (me, column1), Table_messageColumn (me, column2),
		Melder_double (difference), Melder_double (t), Melder_double (significance),
		100 * (1.0 - 2.0 * significanceLevel),
		Melder_double (lowerLimit), significanceLevel,
		Melder_double (upperLimit), significanceLevel);
END
	
FORM (Table_reportMean_studentT, "Report mean (Student t)", 0)
	WORD ("Column", "")
	POSITIVE ("Significance level", "0.025")
	OK
DO
	Table me = ONLY_OBJECT;
	long column = Table_columnLabelToIndex (me, GET_STRING ("Column"));
	double significanceLevel = GET_REAL ("Significance level");
	double mean, tFromZero, significanceFromZero, lowerLimit, upperLimit;
	if (column == 0) return Melder_error ("No such column.");
	mean = Table_getMean_studentT (me, column, significanceLevel,
		& tFromZero, & significanceFromZero, & lowerLimit, & upperLimit);
	Melder_information ("Mean of column %s:\n"
		"Mean = %s\n"
		"Student's t from zero = %s\n"
		"Significance from zero = %s (one-tailed)\n"
		"Confidence interval (%f%%):\n"
		"   Lower limit = %s (lowest difference that cannot be rejected with p = %f)\n"
		"   Upper limit = %s (highest difference that cannot be rejected with p = %f)\n",
		Table_messageColumn (me, column),
		Melder_double (mean), Melder_double (tFromZero), Melder_double (significanceFromZero),
		100 * (1.0 - 2.0 * significanceLevel),
		Melder_double (lowerLimit), significanceLevel,
		Melder_double (upperLimit), significanceLevel);
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
		Table me = ONLY_OBJECT;
		long xcolumn = Table_columnLabelToIndex (me, GET_STRING ("Horizontal column"));
		long ycolumn = Table_columnLabelToIndex (me, GET_STRING ("Vertical column"));
		long markColumn = Table_columnLabelToIndex (me, GET_STRING ("Column with marks"));
		if (xcolumn == 0 || ycolumn == 0 || markColumn == 0) return Melder_error ("No such column.");
		Table_scatterPlot (OBJECT, GRAPHICS, xcolumn, ycolumn,
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
		Table me = ONLY_OBJECT;
		long xcolumn = Table_columnLabelToIndex (me, GET_STRING ("Horizontal column"));
		long ycolumn = Table_columnLabelToIndex (me, GET_STRING ("Vertical column"));
		Table_scatterPlot_mark (OBJECT, GRAPHICS, xcolumn, ycolumn,
			GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
			GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
			GET_REAL ("Mark size"), GET_STRING ("Mark string"), GET_INTEGER ("Garnish"));
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
	long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
	if (icol == 0) return Melder_error ("No such column.");
	Melder_information ("%ld", Table_searchColumn (me, icol, GET_STRING ("Value")));
END
	
FORM (Table_setColumnLabel_index, "Set column label", 0)
	NATURAL ("Column number", "1")
	SENTENCE ("Label", "")
	OK
DO
	WHERE (SELECTED) {
		Table_setColumnLabel (OBJECT, GET_INTEGER ("Column number"), GET_STRING ("Label"));
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
		Table_setColumnLabel (OBJECT, Table_columnLabelToIndex (OBJECT, GET_STRING ("Old label")), GET_STRING ("New label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
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
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Column label"));
		if (icol == 0) return Melder_error ("No such column.");
		Table_setStringValue (me, GET_INTEGER ("Row number"), icol, GET_STRING ("String value"));
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
		if (! Table_sortRows_string (me, GET_STRING ("columnLabels"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Table_to_Matrix)
	EVERY_TO (Table_to_Matrix (OBJECT))
END

FORM (Table_to_TableOfReal, "Table: Down to TableOfReal", 0)
	WORD ("Column for row labels", "")
	OK
DO
	WHERE (SELECTED) {
		Table me = OBJECT;
		long icol = Table_columnLabelToIndex (me, GET_STRING ("Column for row labels"));
		if (! praat_new (Table_to_TableOfReal (OBJECT, icol), NAME)) return 0;
	}
END

FORM_WRITE (Table_writeToTableFile, "Write Table to table file", 0, "txt")
	if (! Table_writeToTableFile (ONLY_OBJECT, file)) return 0;
END

DIRECT (StatisticsTutorial) Melder_help ("Statistics"); END

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

void praat_uvafon_Stat_init (void);
void praat_uvafon_Stat_init (void) {

	Thing_recognizeClassesByName (classTable, classLinearRegression, classLogisticRegression, NULL);

	Data_recognizeFileType (tabSeparatedFileRecognizer);

	praat_addAction1 (classTable, 0, "Table help", 0, 0, DO_Table_help);
	praat_addAction1 (classTable, 1, "Write to table file...", 0, 0, DO_Table_writeToTableFile);
	praat_addAction1 (classTable, 1, "Edit", 0, 0, DO_Table_edit);
	praat_addAction1 (classTable, 0, "Draw -                ", 0, 0, 0);
		praat_addAction1 (classTable, 0, "Scatter plot...", 0, 1, DO_Table_scatterPlot);
		praat_addAction1 (classTable, 0, "Scatter plot (mark)...", 0, 1, DO_Table_scatterPlot_mark);
	praat_addAction1 (classTable, 0, "Query -                ", 0, 0, 0);
		praat_addAction1 (classTable, 1, "Get number of rows", 0, 1, DO_Table_getNumberOfRows);
		praat_addAction1 (classTable, 1, "Get number of columns", 0, 1, DO_Table_getNumberOfColumns);
		praat_addAction1 (classTable, 1, "Get column label...", 0, 1, DO_Table_getColumnLabel);
		praat_addAction1 (classTable, 1, "Get column index...", 0, 1, DO_Table_getColumnIndex);
		praat_addAction1 (classTable, 1, "Search column...", 0, 1, DO_Table_searchColumn);
		praat_addAction1 (classTable, 1, "-- get value --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Get value...", 0, 1, DO_Table_getValue);
	praat_addAction1 (classTable, 0, "Statistics -    ", 0, 0, 0);
		praat_addAction1 (classTable, 1, "Statistics tutorial", 0, 1, DO_StatisticsTutorial);
		praat_addAction1 (classTable, 1, "-- get stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Get quantile...", 0, 1, DO_Table_getQuantile);
		praat_addAction1 (classTable, 1, "Get mean...", 0, 1, DO_Table_getMean);
		praat_addAction1 (classTable, 1, "Get standard deviation...", 0, 1, DO_Table_getStandardDeviation);
		praat_addAction1 (classTable, 1, "-- report stats --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "Report mean (Student t)...", 0, 1, DO_Table_reportMean_studentT);
		/*praat_addAction1 (classTable, 1, "Report standard deviation...", 0, 1, DO_Table_reportStandardDeviation);*/
		praat_addAction1 (classTable, 1, "Report difference (Student t)...", 0, 1, DO_Table_reportDifference_studentT);
		praat_addAction1 (classTable, 1, "Report correlation (Pearson r)...", 0, 1, DO_Table_reportCorrelation_pearsonR);
		praat_addAction1 (classTable, 1, "Report correlation (Kendall tau)...", 0, 1, DO_Table_reportCorrelation_kendallTau);
		praat_addAction1 (classTable, 1, "-- to regression --", 0, 1, 0);
		praat_addAction1 (classTable, 1, "To linear regression", 0, 1, DO_Table_to_LinearRegression);
		praat_addAction1 (classTable, 1, "To logistic regression", 0, 1, DO_Table_to_LogisticRegression);
	praat_addAction1 (classTable, 0, "Modify -        ", 0, 0, 0);
		praat_addAction1 (classTable, 0, "Set string value...", 0, 1, DO_Table_setStringValue);
		praat_addAction1 (classTable, 0, "Set numeric value...", 0, 1, DO_Table_setNumericValue);
		praat_addAction1 (classTable, 0, "Formula...", 0, 1, DO_Table_formula);
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
		praat_addAction1 (classTable, 0, "Pool...", 0, 1, DO_Table_pool);
	praat_addAction1 (classTable, 0, "Down to Matrix", 0, 0, DO_Table_to_Matrix);
	praat_addAction1 (classTable, 0, "Down to TableOfReal...", 0, 0, DO_Table_to_TableOfReal);
}

/* End of file praat_Stat.c */
