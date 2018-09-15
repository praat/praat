#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
 *
 * Copyright (C) 1993-2018 David Weenink
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

integer Table_getNumberOfRowsWhere (Table me, conststring32 formula, Interpreter interpreter);

autoINTVEC Table_findRowsMatchingCriterion (Table me, conststring32 formula, Interpreter interpreter);

autoTable Table_create_petersonBarney1952 ();

autoTable Table_create_polsVanNierop1973 ();

autoTable Table_create_weenink1983 ();

autoTable Table_create_esposito2006 ();

autoTable Table_create_ganong1980 ();

double Table_getMedianAbsoluteDeviation (Table me, integer columnNumber);

// Two one-way tests for normal and non-normally distributed data, respectively.
autoTable Table_getOneWayAnalysisOfVarianceF (Table me,
	integer column, integer groupColumn, autoTable *means, autoTable *meansDiff, autoTable *meansDiffProbabilities);

autoTable Table_getOneWayKruskalWallis (Table me,
	integer column, integer factorColumn, double *p_prob, double *p_kruskalWallis, double *p_df);

autoTable Table_getTwoWayAnalysisOfVarianceF (Table me,
	integer column, integer groupColumnA, integer groupColumnB, autoTable *means, autoTable *factorLevelSizes);

void Table_verticalErrorBarsPlotWhere (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer yci_min, integer yci_max, double bar_mm, bool garnish, conststring32 formula, Interpreter interpreter);

void Table_horizontalErrorBarsPlotWhere (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer xci_min, integer xci_max, double bar_mm, bool garnish, conststring32 formula, Interpreter interpreter);

void Table_normalProbabilityPlot (Table me, Graphics g,
	integer column, integer numberOfQuantiles, double numberOfSigmas, int labelSize, conststring32 label, bool garnish);

void Table_quantileQuantilePlot (Table me, Graphics g,
	integer xcolumn, integer ycolumn, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, int labelSize, conststring32 label, bool garnish
);

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g,
	integer dataColumn, integer factorColumn, conststring32 xlevel, conststring32 ylevel, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, int labelSize, conststring32 label, bool garnish
);

void Table_boxPlots (Table me, Graphics g,
	integer dataColumn, integer factorColumn, double ymin, double ymax, bool garnish);

void Table_boxPlotsWhere (Table me, Graphics g,
	conststring32 dataColumns_string, integer factorColumn,
	double ymin, double ymax, bool garnish, conststring32 formula, Interpreter interpreter
);

autoTable Table_extractRowsWhere (Table me, conststring32 formula, Interpreter interpreter);

autoTable Table_extractColumnRanges (Table me, conststring32 ranges);

autoTable Table_extractMahalanobisWhere (Table me,
	conststring32 columnLabels, conststring32 factorColumn, double numberOfSigmas,
	kMelder_number which, conststring32 formula, Interpreter interpreter
);

void Table_distributionPlotWhere (Table me, Graphics g,
	integer dataColumn, double minimum, double maximum, integer nBins,
	double freqMin, double freqMax, bool garnish, conststring32 formula, Interpreter interpreter
);

void Table_barPlotWhere (Table me, Graphics g,
	conststring32 columnLabels, double ymin, double ymax, conststring32 labelColumn,
	double xoffsetFraction, double interbarFraction, double interbarsFraction, conststring32 colours,
	double angle, bool garnish, conststring32 formula, Interpreter interpreter
);

void Table_lineGraphWhere (Table me, Graphics g,
	integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax,
	conststring32 symbol, double angle, bool garnish, conststring32 formula, Interpreter interpreter
);

void Table_lagPlotWhere (Table me, Graphics g,
	integer column, integer lag, double xmin, double xmax,
	conststring32 symbol, int labelSize, bool garnish, conststring32 formula, Interpreter interpreter
);

void Table_drawEllipsesWhere (Table me, Graphics g,
	integer xcolumn, integer ycolumn, integer labelcolumn, double xmin, double xmax, double ymin, double ymax,
	double numberOfSigmas, integer labelSize, bool garnish, conststring32 formula, Interpreter interpreter);

void Table_printAsAnovaTable (Table me);

void Table_printAsMeansTable (Table me);

#endif // _Table_extensions_h_
