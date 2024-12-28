#ifndef _Table_h_
#define _Table_h_
/* Table.h
 *
 * Copyright (C) 2002-2012,2014-2019,2021-2024 Paul Boersma
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

#include "Collection.h"
#include "Graphics.h"
Thing_declare (Interpreter);

#include "Table_def.h"

void Table_initWithColumnNames (Table me, integer numberOfRows, constSTRVEC columnNames);
autoTable Table_createWithColumnNames (integer numberOfRows, constSTRVEC columnNames);
void Table_initWithoutColumnNames (Table me, integer numberOfRows, integer numberOfColumns);
autoTable Table_createWithoutColumnNames (integer numberOfRows, integer numberOfColumns);
#define Table_create Table_createWithoutColumnNames

void Table_checkSpecifiedColumnNumbersWithinRange (Table me, constINTVECVU const& columnNumbers);

autoTable Tables_append (OrderedOf<structTable>* me);
void Table_appendRow (Table me);
void Table_appendColumn (Table me, conststring32 columnName);
void Table_appendSumColumn (Table me, integer column1, integer column2, conststring32 columnName);
void Table_appendDifferenceColumn (Table me, integer column1, integer column2, conststring32 columnName);
void Table_appendProductColumn (Table me, integer column1, integer column2, conststring32 columnName);
void Table_appendQuotientColumn (Table me, integer column1, integer column2, conststring32 columnName);
void Table_removeRow (Table me, integer rowNumber);
void Table_removeColumn (Table me, integer columnNumber);
void Table_insertRow (Table me, integer rowPosition);
void Table_insertColumn (Table me, integer columnPosition, conststring32 columnName /* cattable */);
void Table_renameColumn_e (Table me, integer columnNumber, conststring32 columnName /* cattable */);
integer Table_columnNameToNumber_0 (constTable me, conststring32 columnName) noexcept;
integer Table_columnNameToNumber_e (constTable me, conststring32 columnName);
autoINTVEC Table_columnNamesToNumbers (constTable me, constSTRVEC const& columnNames);
autoINTVEC Table_getColumnIndicesFromColumnLabelString (constTable me, conststring32 string);
integer Table_searchColumn (constTable me, integer column, conststring32 value) noexcept;

/*
 * Procedure for reading strings or numbers from table cells:
 * use the following two calls exclusively.
 */
conststring32 Table_getStringValue_a (Table me, integer row, integer column);
double Table_getNumericValue_a (Table me, integer row, integer column);

/*
 * Procedure for writing strings or numbers into table cells:
 * use the following two calls exclusively.
 */
void Table_setStringValue (Table me, integer rowNumber, integer columnNumber, conststring32 value /* cattable */);
void Table_setNumericValue (Table me, integer row, integer column, double value);

/* For optimizations only (e.g. conversion to Matrix or TableOfReal). */
void Table_numericize_a (Table me, integer columnNumber);

autoVEC Table_getAllNumbersInColumn (Table me, integer columnNumber);
autoSTRVEC Table_getAllTextsInColumn (Table me, integer columnNumber);

double Table_getQuantile (Table me, integer column, double quantile);
double Table_getSum (Table me, integer column);
double Table_getMean (Table me, integer column);
double Table_getMaximum (Table me, integer icol);
double Table_getMinimum (Table me, integer icol);
double Table_getGroupMean (Table me, integer column, integer groupColumn, conststring32 group);
double Table_getStdev (Table me, integer column);
integer Table_drawRowFromDistribution (Table me, integer column);
double Table_getCorrelation_pearsonR (Table me, integer column1, integer column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getCorrelation_kendallTau (Table me, integer column1, integer column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getMean_studentT (Table me, integer column, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getDifference_studentT (Table me, integer column1, integer column2, double significanceLevel,
	double *out_t, double *out_numberOfDegreesOfFreedom, double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupMean_studentT (Table me, integer column, integer groupColumn, conststring32 group1, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupDifference_studentT (Table me, integer column, integer groupColumn, conststring32 group1, conststring32 group2, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupDifference_wilcoxonRankSum (Table me, integer column, integer groupColumn, conststring32 group1, conststring32 group2,
	double *out_rankSum, double *out_significanceFromZero);
double Table_getVarianceRatio (Table me, integer column1, integer column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
bool Table_getExtrema (Table me, integer icol, double *minimum, double *maximum);

void Table_formula (Table me, integer column, conststring32 formula, Interpreter interpreter);
void Table_formula_columnRange (Table me, integer column1, integer column2, conststring32 expression, Interpreter interpreter);

void Table_sortRows_a (Table me, constINTVECVU const& columns);
void Table_sortRows (Table me, constSTRVEC columns);
void Table_randomizeRows (Table me) noexcept;
void Table_reflectRows (Table me) noexcept;

void Table_scatterPlot (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, integer markColumn, double fontSize, bool garnish);
void Table_scatterPlot_mark (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, conststring32 mark, bool garnish);
void Table_drawEllipse_e (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, bool garnish);

void Table_list (constTable me, bool includeRowNumbers);
void Table_writeToTabSeparatedFile (Table me, MelderFile file);
void Table_writeToCommaSeparatedFile (Table me, MelderFile file);
void Table_writeToSemicolonSeparatedFile (Table me, MelderFile file);
autoTable Table_readFromTableFile (MelderFile file);
autoTable Table_readFromCharacterSeparatedTextFile (MelderFile file, char32 separator, bool interpretQuotes);

autoTable Table_extractRowsWhereColumn_number (Table me, integer column, kMelder_number which, double criterion);
autoTable Table_extractRowsWhereColumn_string (Table me, integer column, kMelder_string which, conststring32 criterion);
autoTable Table_collapseRows (Table me, constSTRVEC factors, constSTRVEC columnsToSum,
	constSTRVEC columnsToAverage, constSTRVEC columnsToMedianize,
	constSTRVEC columnsToAverageLogarithmically, constSTRVEC columnsToMedianizeLogarithmically);
autoTable Table_rowsToColumns (Table me, constSTRVEC const& factors_names, conststring32 columnToTranspose, constSTRVEC const& columnsToExpand_names);
autoTable Table_transpose (Table me);

void Table_columns_checkExist (Table me, constSTRVEC columnNames);
void Table_checkSpecifiedRowNumberWithinRange (Table me, integer rowNumber);
void Table_checkSpecifiedColumnNumberWithinRange (Table me, integer columnNumber);
bool Table_isCellNumeric_ErrorFalse (Table me, integer rowNumber, integer columnNumber);
bool Table_isColumnNumeric_ErrorFalse (Table me, integer columnNumber);

conststring32 Table_messageColumn (Table me, integer column);

/* End of file Table.h */
#endif
