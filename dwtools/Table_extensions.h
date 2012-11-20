#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20120724 Latest modification.
*/

#include "TableOfReal.h"
#include "Collection.h"
#include "Pattern.h"
#include "Categories.h"
#include "Strings_.h"
#include "SSCP.h"
#include "Table.h"

Table Table_createFromPetersonBarneyData ();
Table Table_createFromPolsVanNieropData ();
Table Table_createFromWeeninkData ();

double Table_getMedianAbsoluteDeviation (Table me, long columnNumber);

// Two one-way tests for normal and non-normally distributed data, respectively.
Table Table_getOneWayAnalysisOfVarianceF (Table me, long column, long groupColumn, Table *means, Table *meansDiff, Table *meansDiffProbabilities);
Table Table_getOneWayKruskalWallis (Table me, long column, long groupColumn, double *degreesOfFreedom, double *kruskalWallis, double *probability);

Table Table_getTwoWayAnalysisOfVarianceF (Table me, long column, long groupColumnA, long groupColumnB, Table *means, Table *factorLevelSizes);

void Table_scatterPlotWithConfidenceIntervals (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long xci_min, long xci_max,
	long yci_min, long yci_max, double bar_mm, int garnish);

void Table_normalProbabilityPlot (Table me, Graphics g, long column, long numberOfQuantiles, double numberOfSigmas, int labelSize, const wchar_t *label, int garnish);

void Table_quantileQuantilePlot (Table me, Graphics g, long xcolumn, long ycolumn, long numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const wchar_t *label, int garnish);

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g, long dataColumn, long factorColumn, wchar_t *xlevel, wchar_t *ylevel, long numberOfQuantiles, double xmin, double xmax, double ymin, double ymax, int labelSize, const wchar_t *label, int garnish);

void Table_boxPlots (Table me, Graphics g, long dataColumn, long factorColumn, double ymin, double ymax, int garnish);

void Table_printAsAnovaTable (Table me);
void Table_printAsMeansTable (Table me);
#endif // _Table_extensions_h_
