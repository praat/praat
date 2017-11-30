#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020411 initial GPL
 djmw 20131220 Latest modification.
*/

#include "TableOfReal.h"
#include "Collection.h"
#include "Categories.h"
#include "Strings_.h"
#include "SSCP.h"
#include "Table.h"

integer Table_getNumberOfRowsWhere (Table me, const char32 *formula, Interpreter interpreter);

integer *Table_findRowsMatchingCriterion (Table me, const char32 *formula, Interpreter interpreter, integer *numberOfMatches);

autoTable Table_create_petersonBarney1952 ();

autoTable Table_create_polsVanNierop1973 ();

autoTable Table_create_weenink1983 ();

autoTable Table_create_esposito2006 ();

autoTable Table_create_ganong1980 ();

double Table_getMedianAbsoluteDeviation (Table me, integer columnNumber);

// Two one-way tests for normal and non-normally distributed data, respectively.
autoTable Table_getOneWayAnalysisOfVarianceF (Table me, integer column, integer groupColumn, autoTable *means, autoTable *meansDiff, autoTable *meansDiffProbabilities);

autoTable Table_getOneWayKruskalWallis (Table me, integer column, integer factorColumn, double *p_prob, double *p_kruskalWallis, double *p_df);

autoTable Table_getTwoWayAnalysisOfVarianceF (Table me, integer column, integer groupColumnA, integer groupColumnB, autoTable *means, autoTable *factorLevelSizes);

void Table_verticalErrorBarsPlotWhere (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer yci_min, integer yci_max, double bar_mm, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_horizontalErrorBarsPlotWhere (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer xci_min, integer xci_max, double bar_mm, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_normalProbabilityPlot (Table me, Graphics g, integer column, integer numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, bool garnish);

void Table_quantileQuantilePlot (Table me, Graphics g, integer xcolumn, integer ycolumn, integer numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const char32 *label, bool garnish);

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g, integer dataColumn, integer factorColumn, const char32 *xlevel, const char32 *ylevel, integer numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const char32 *label, bool garnish);

void Table_boxPlots (Table me, Graphics g, integer dataColumn, integer factorColumn, double ymin, double ymax, bool garnish);

void Table_boxPlotsWhere (Table me, Graphics g, const char32 *dataColumns_string, integer factorColumn, double ymin, double ymax, bool garnish, const char32 *formula, Interpreter interpreter);

autoTable Table_extractRowsWhere (Table me, const char32 *formula, Interpreter interpreter);

autoTable Table_extractColumnRanges (Table me, const char32 *ranges);

autoTable Table_extractMahalanobisWhere (Table me, const char32 *columnLabels, const char32 *factorColumn, double numberOfSigmas, kMelder_number which, const char32 *formula, Interpreter interpreter);

void Table_distributionPlotWhere (Table me, Graphics g, integer dataColumn, double minimum, double maximum, integer nBins, double freqMin, double freqMax, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_barPlotWhere (Table me, Graphics g, const char32 *columnLabels, double ymin, double ymax, const char32 *labelColumn, double xoffsetFraction, double interbarFraction, double interbarsFraction, const char32 *colours, double angle, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_lineGraphWhere (Table me, Graphics g, integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax, const char32 *symbol, double angle, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_lagPlotWhere (Table me, Graphics g, integer column, integer lag, double xmin, double xmax, const char32 *symbol, int labelSize, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_drawEllipsesWhere (Table me, Graphics g, integer xcolumn, integer ycolumn, integer labelcolumn, double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, integer labelSize, bool garnish, const char32 *formula, Interpreter interpreter);

void Table_printAsAnovaTable (Table me);

void Table_printAsMeansTable (Table me);

#endif // _Table_extensions_h_
