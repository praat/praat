#ifndef _Table_h_
#define _Table_h_
/* Table.h
 *
 * Copyright (C) 2002-2011,2012,2014,2015,2017 Paul Boersma
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

void Table_initWithColumnNames (Table me, integer numberOfRows, conststring32 columnNames);
autoTable Table_createWithColumnNames (integer numberOfRows, conststring32 columnNames);
void Table_initWithoutColumnNames (Table me, integer numberOfRows, integer numberOfColumns);
autoTable Table_createWithoutColumnNames (integer numberOfRows, integer numberOfColumns);
#define Table_create Table_createWithoutColumnNames

autoTable Tables_append (OrderedOf<structTable>* me);
void Table_appendRow (Table me);
void Table_appendColumn (Table me, conststring32 label);
void Table_appendSumColumn (Table me, integer column1, integer column2, conststring32 label);
void Table_appendDifferenceColumn (Table me, integer column1, integer column2, conststring32 label);
void Table_appendProductColumn (Table me, integer column1, integer column2, conststring32 label);
void Table_appendQuotientColumn (Table me, integer column1, integer column2, conststring32 label);
void Table_removeRow (Table me, integer row);
void Table_removeColumn (Table me, integer column);
void Table_insertRow (Table me, integer row);
void Table_insertColumn (Table me, integer column, conststring32 label /* cattable */);
void Table_setColumnLabel (Table me, integer column, conststring32 label /* cattable */);
integer Table_findColumnIndexFromColumnLabel (Table me, conststring32 label) noexcept;
integer Table_getColumnIndexFromColumnLabel (Table me, conststring32 columnLabel);
autoINTVEC Table_getColumnIndicesFromColumnLabelString (Table me, conststring32 string);
integer Table_searchColumn (Table me, integer column, conststring32 value) noexcept;

/*
 * Procedure for reading strings or numbers from table cells:
 * use the following two calls exclusively.
 */
conststring32 Table_getStringValue_Assert (Table me, integer row, integer column);
double Table_getNumericValue_Assert (Table me, integer row, integer column);

/*
 * Procedure for writing strings or numbers into table cells:
 * use the following two calls exclusively.
 */
void Table_setStringValue (Table me, integer rowNumber, integer columnNumber, conststring32 value /* cattable */);
void Table_setNumericValue (Table me, integer row, integer column, double value);

/* For optimizations only (e.g. conversion to Matrix or TableOfReal). */
void Table_numericize_Assert (Table me, integer columnNumber);

double Table_getQuantile (Table me, integer column, double quantile);
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

void Table_sortRows_Assert (Table me, constINTVEC columns);
void Table_sortRows_string (Table me, conststring32 columns_string);
void Table_randomizeRows (Table me) noexcept;
void Table_reflectRows (Table me) noexcept;

void Table_scatterPlot (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, integer markColumn, double fontSize, bool garnish);
void Table_scatterPlot_mark (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, conststring32 mark, bool garnish);
void Table_drawEllipse_e (Table me, Graphics g, integer xcolumn, integer ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, bool garnish);

void Table_list (Table me, bool includeRowNumbers);
void Table_writeToTabSeparatedFile (Table me, MelderFile file);
void Table_writeToCommaSeparatedFile (Table me, MelderFile file);
void Table_writeToSemicolonSeparatedFile (Table me, MelderFile file);
autoTable Table_readFromTableFile (MelderFile file);
autoTable Table_readFromCharacterSeparatedTextFile (MelderFile file, char32 separator, bool interpretQuotes);

autoTable Table_extractRowsWhereColumn_number (Table me, integer column, kMelder_number which, double criterion);
autoTable Table_extractRowsWhereColumn_string (Table me, integer column, kMelder_string which, conststring32 criterion);
autoTable Table_collapseRows (Table me, conststring32 factors_string, conststring32 columnsToSum_string,
	conststring32 columnsToAverage_string, conststring32 columnsToMedianize_string,
	conststring32 columnsToAverageLogarithmically_string, conststring32 columnsToMedianizeLogarithmically_string);
autoTable Table_rowsToColumns (Table me, conststring32 factors_string, integer columnToTranspose, conststring32 columnsToExpand_string);
autoTable Table_transpose (Table me);

void Table_checkSpecifiedRowNumberWithinRange (Table me, integer rowNumber);
void Table_checkSpecifiedColumnNumberWithinRange (Table me, integer columnNumber);
bool Table_isCellNumeric_ErrorFalse (Table me, integer rowNumber, integer columnNumber);
bool Table_isColumnNumeric_ErrorFalse (Table me, integer columnNumber);

conststring32 Table_messageColumn (Table me, integer column);

/* End of file Table.h */
#endif
