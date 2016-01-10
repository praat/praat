#ifndef _Table_h_
#define _Table_h_
/* Table.h
 *
 * Copyright (C) 2002-2011,2012,2014,2015 Paul Boersma
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

#include "Collection.h"
#include "Graphics.h"
Thing_declare (Interpreter);

#include "Table_def.h"

void Table_initWithColumnNames (Table me, long numberOfRows, const char32 *columnNames);
autoTable Table_createWithColumnNames (long numberOfRows, const char32 *columnNames);
void Table_initWithoutColumnNames (Table me, long numberOfRows, long numberOfColumns);
autoTable Table_createWithoutColumnNames (long numberOfRows, long numberOfColumns);
#define Table_create Table_createWithoutColumnNames

autoTable Tables_append (OrderedOf<structTable>* me);
void Table_appendRow (Table me);
void Table_appendColumn (Table me, const char32 *label);
void Table_appendSumColumn (Table me, long column1, long column2, const char32 *label);
void Table_appendDifferenceColumn (Table me, long column1, long column2, const char32 *label);
void Table_appendProductColumn (Table me, long column1, long column2, const char32 *label);
void Table_appendQuotientColumn (Table me, long column1, long column2, const char32 *label);
void Table_removeRow (Table me, long row);
void Table_removeColumn (Table me, long column);
void Table_insertRow (Table me, long row);
void Table_insertColumn (Table me, long column, const char32 *label /* cattable */);
void Table_setColumnLabel (Table me, long column, const char32 *label /* cattable */);
long Table_findColumnIndexFromColumnLabel (Table me, const char32 *label) noexcept;
long Table_getColumnIndexFromColumnLabel (Table me, const char32 *columnLabel);
long * Table_getColumnIndicesFromColumnLabelString (Table me, const char32 *string, long *numberOfTokens);
long Table_searchColumn (Table me, long column, const char32 *value) noexcept;

/*
 * Procedure for reading strings or numbers from table cells:
 * use the following two calls exclusively.
 */
const char32 * Table_getStringValue_Assert (Table me, long row, long column);
double Table_getNumericValue_Assert (Table me, long row, long column);

/*
 * Procedure for writing strings or numbers into table cells:
 * use the following two calls exclusively.
 */
void Table_setStringValue (Table me, long rowNumber, long columnNumber, const char32 *value /* cattable */);
void Table_setNumericValue (Table me, long row, long column, double value);

/* For optimizations only (e.g. conversion to Matrix or TableOfReal). */
void Table_numericize_Assert (Table me, long columnNumber);

double Table_getQuantile (Table me, long column, double quantile);
double Table_getMean (Table me, long column);
double Table_getMaximum (Table me, long icol);
double Table_getMinimum (Table me, long icol);
double Table_getGroupMean (Table me, long column, long groupColumn, const char32 *group);
double Table_getStdev (Table me, long column);
long Table_drawRowFromDistribution (Table me, long column);
double Table_getCorrelation_pearsonR (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getCorrelation_kendallTau (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getMean_studentT (Table me, long column, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getDifference_studentT (Table me, long column1, long column2, double significanceLevel,
	double *out_t, double *out_numberOfDegreesOfFreedom, double *out_significance, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupMean_studentT (Table me, long column, long groupColumn, const char32 *group1, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupDifference_studentT (Table me, long column, long groupColumn, const char32 *group1, const char32 *group2, double significanceLevel,
	double *out_tFromZero, double *out_numberOfDegreesOfFreedom, double *out_significanceFromZero, double *out_lowerLimit, double *out_upperLimit);
double Table_getGroupDifference_wilcoxonRankSum (Table me, long column, long groupColumn, const char32 *group1, const char32 *group2,
	double *out_rankSum, double *out_significanceFromZero);
double Table_getVarianceRatio (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit);
bool Table_getExtrema (Table me, long icol, double *minimum, double *maximum);

void Table_formula (Table me, long column, const char32 *formula, Interpreter interpreter);
void Table_formula_columnRange (Table me, long column1, long column2, const char32 *expression, Interpreter interpreter);

void Table_sortRows_Assert (Table me, long *columns, long numberOfColumns);
void Table_sortRows_string (Table me, const char32 *columns_string);
void Table_randomizeRows (Table me) noexcept;
void Table_reflectRows (Table me) noexcept;

void Table_scatterPlot (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long markColumn, int fontSize, int garnish);
void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const char32 *mark, int garnish);
void Table_drawEllipse_e (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int garnish);

void Table_list (Table me, bool includeRowNumbers);
void Table_writeToTabSeparatedFile (Table me, MelderFile file);
void Table_writeToCommaSeparatedFile (Table me, MelderFile file);
autoTable Table_readFromTableFile (MelderFile file);
autoTable Table_readFromCharacterSeparatedTextFile (MelderFile file, char32 separator);

autoTable Table_extractRowsWhereColumn_number (Table me, long column, int which_Melder_NUMBER, double criterion);
autoTable Table_extractRowsWhereColumn_string (Table me, long column, int which_Melder_STRING, const char32 *criterion);
autoTable Table_collapseRows (Table me, const char32 *factors_string, const char32 *columnsToSum_string,
	const char32 *columnsToAverage_string, const char32 *columnsToMedianize_string,
	const char32 *columnsToAverageLogarithmically_string, const char32 *columnsToMedianizeLogarithmically_string);
autoTable Table_rowsToColumns (Table me, const char32 *factors_string, long columnToTranspose, const char32 *columnsToExpand_string);
autoTable Table_transpose (Table me);

void Table_checkSpecifiedRowNumberWithinRange (Table me, long rowNumber);
void Table_checkSpecifiedColumnNumberWithinRange (Table me, long columnNumber);
bool Table_isCellNumeric_ErrorFalse (Table me, long rowNumber, long columnNumber);
bool Table_isColumnNumeric_ErrorFalse (Table me, long columnNumber);

/* End of file Table.h */
#endif
