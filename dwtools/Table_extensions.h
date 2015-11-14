#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
 *
 * Copyright (C) 1993-2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020411 initial GPL
 djmw 20131220 Latest modification.
*/

#include "TableOfReal.h"
#include "Collection.h"
#include "Pattern.h"
#include "Categories.h"
#include "Strings_.h"
#include "SSCP.h"
#include "Table.h"

long Table_getNumberOfRowsWhere (Table me, const char32 *formula, Interpreter interpreter);

long *Table_findRowsMatchingCriterion (Table me, const char32 *formula, Interpreter interpreter, long *numberOfMatches);

autoTable Table_createFromPetersonBarneyData ();

autoTable Table_createFromPolsVanNieropData ();

autoTable Table_createFromWeeninkData ();

autoTable Table_createFromEspositoData ();

autoTable Table_createFromGanongData ();

double Table_getMedianAbsoluteDeviation (Table me, long columnNumber);

// Two one-way tests for normal and non-normally distributed data, respectively.
autoTable Table_getOneWayAnalysisOfVarianceF (Table me, long column, long groupColumn, autoTable *means, autoTable *meansDiff, autoTable *meansDiffProbabilities);

autoTable Table_getOneWayKruskalWallis (Table me, long column, long groupColumn, double *degreesOfFreedom, double *kruskalWallis, double *probability);

autoTable Table_getTwoWayAnalysisOfVarianceF (Table me, long column, long groupColumnA, long groupColumnB, Table *means, Table *factorLevelSizes);

void Table_verticalErrorBarsPlotWhere (Table me, Graphics g, long xcolumn, long ycolumn, double xmin, double xmax, 
	double ymin, double ymax, long yci_min, long yci_max, double bar_mm, int garnish, const char32 *formula, Interpreter interpreter);

void Table_horizontalErrorBarsPlotWhere (Table me, Graphics g, long xcolumn, long ycolumn, double xmin, double xmax, 
	double ymin, double ymax, long xci_min, long xci_max, double bar_mm, int garnish, const char32 *formula, Interpreter interpreter);

void Table_normalProbabilityPlot (Table me, Graphics g, long column, long numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, int garnish);

void Table_quantileQuantilePlot (Table me, Graphics g, long xcolumn, long ycolumn, long numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const char32 *label, int garnish);

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g, long dataColumn, long factorColumn, const char32 *xlevel, const char32 *ylevel, long numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const char32 *label, int garnish);

void Table_boxPlots (Table me, Graphics g, long dataColumn, long factorColumn, double ymin, double ymax, int garnish);

void Table_boxPlotsWhere (Table me, Graphics g, char32 *dataColumns_string, long factorColumn, double ymin, double ymax, int garnish, const char32 *formula, Interpreter interpreter);

autoTable Table_extractRowsWhere (Table me, const char32 *formula, Interpreter interpreter);

autoTable Table_extractColumnRanges (Table me, char32 *ranges);

autoTable Table_extractMahalanobisWhere (Table me, const char32 *columnLabels, const char32 *factorColumn, double numberOfSigmas, int which_Melder_NUMBER, const char32 *formula, Interpreter interpreter);

void Table_distributionPlotWhere (Table me, Graphics g, long dataColumn, double minimum, double maximum, long nBins, double freqMin, double freqMax, int garnish, const char32 *formula, Interpreter interpreter);

void Table_barPlotWhere (Table me, Graphics g, const char32 *columnLabels, double ymin, double ymax, const char32 *labelColumn, double xoffsetFraction, double interbarFraction, double interbarsFraction, const char32 *colours, double angle, int garnish, const char32 *formula, Interpreter interpreter);

void Table_lineGraphWhere (Table me, Graphics g, long xcolumn, double xmin, double xmax, long ycolumn, double ymin, double ymax, const char32 *symbol, double angle, int garnish, const char32 *formula, Interpreter interpreter);

void Table_lagPlotWhere (Table me, Graphics g, long column, long lag, double xmin, double xmax, const char32 *symbol, int labelSize, int garnish, const char32 *formula, Interpreter interpreter);

void Table_drawEllipsesWhere (Table me, Graphics g, long xcolumn, long ycolumn, long labelcolumn, double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, long labelSize, int garnish, const char32 *formula, Interpreter interpreter);

void Table_printAsAnovaTable (Table me);

void Table_printAsMeansTable (Table me);

#endif // _Table_extensions_h_
