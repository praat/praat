#ifndef _DataModeler_h_
#define _DataModeler_h_
/* DataModeler.h
 *
 * Copyright (C) 2014-2020 David Weenink
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
 djmw 20140217
*/

#include "Collection.h"
#include "Covariance.h"
#include "Pitch.h"
#include "OptimalCeilingTier.h"
#include "Sound_to_Formant.h"
#include "Table.h"

#include "DataModeler_enums.h"

#include "DataModeler_def.h"

static inline void getAutoNaturalNumbersWithinRange (integer *from, integer *to, integer maximum, conststring32 text) {
	if (*from <= 0)
		*from = 1;
	if (*to == 0)
		*to = maximum;
	if (*to < *from) {
		*from = 1;
		*to = maximum;
	}
	if (*to > maximum)
		*to = maximum;
	Melder_require (*from <= maximum,
		U"The start index of the ", text, U" range should not be larger than ", maximum, U".");
}

static inline void getAutoNaturalNumberWithinRange (integer *number, integer maximum) {
	if (*number == 0 || *number > maximum)
		*number = maximum;
	if (*number < 0)
		*number = 1;
}

void DataModeler_init (DataModeler me, double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type);

autoDataModeler DataModeler_create (double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type);

autoDataModeler DataModeler_createSimple (double xmin, double xmax, integer numberOfDataPoints,
	conststring32 parameters, double gaussianNoiseStd, kDataModelerFunction type);

void DataModeler_setBasisFunctions (DataModeler me, kDataModelerFunction type);

integer DataModeler_drawingSpecifiers_x (DataModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax);

void DataModeler_drawBasisFunction_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	integer iterm, bool scale, integer numberOfPoints);

void DataModeler_drawModel_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, integer numberOfPoints);

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters, bool errorbars, bool connectPoints, double barWidth_mm, bool drawDots);

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters, bool errorbars, double barWidth_mm);

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool errorbars, double barWidth_mm, bool garnish);

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool garnish);

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters);

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double numberOfSigmas, conststring32 mark, double marksFontSize);

void DataModeler_normalProbabilityPlot (DataModeler me, Graphics g, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish);
/* Get the y-value of the fitted function at x */

void DataModeler_setTolerance (DataModeler me, double tolerance);

void DataModeler_fit (DataModeler me);

// sigmaY used in fit or not.
void DataModeler_setDataWeighing (DataModeler me, kDataModelerWeights weighData);

integer DataModeler_getNumberOfFixedParameters (DataModeler me);
integer DataModeler_getNumberOfFreeParameters (DataModeler me);

void DataModeler_setParameterValue (DataModeler me, integer index, double value, kDataModelerParameter status);

void DataModeler_setParameterValueFixed (DataModeler me, integer index, double value);

void DataModeler_setParametersFree (DataModeler me, integer fromIndex, integer toIndex);

double DataModeler_getParameterValue (DataModeler me, integer index);

kDataModelerParameter DataModeler_getParameterStatus (DataModeler me, integer index);

double DataModeler_getParameterStandardDeviation (DataModeler me, integer index);

double DataModeler_getVarianceOfParameters (DataModeler me, integer fromIndex, integer toIndex, integer *numberOfFreeParameters);

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas);

double DataModeler_getDataStandardDeviation (DataModeler me);

void DataModeler_getExtremaY (DataModeler me, double *ymin, double *ymax);

double DataModeler_getModelValueAtX (DataModeler me, double x);

double DataModeler_getModelValueAtIndex (DataModeler me, integer index);

double DataModeler_getWeightedMean (DataModeler me);

integer DataModeler_getNumberOfInvalidDataPoints (DataModeler me);

double DataModeler_getDataPointXValue (DataModeler me, integer index);
double DataModeler_getDataPointYValue (DataModeler me, integer index);

void DataModeler_setDataPointXValue (DataModeler me, integer index, double value);
void DataModeler_setDataPointYValue (DataModeler me, integer index, double value);
void DataModeler_setDataPointValues (DataModeler me, integer index, double xvalue, double yvalue);

kDataModelerData DataModeler_getDataPointStatus (DataModeler me, integer index);

void DataModeler_setDataPointStatus (DataModeler me, integer index, kDataModelerData status);
void DataModeler_setDataPointValueAndStatus (DataModeler me, integer index, double value, kDataModelerData dataStatus);

void DataModeler_setDataPointYSigma (DataModeler me, integer index, double sigma);

double DataModeler_getDataPointYSigma (DataModeler me, integer index);

autoVEC DataModeler_getDataPointsWeights (DataModeler me, kDataModelerWeights weighData);
/*
	Interpret the values in sigmaY as 1 / sigmay or 1 / sqrt (sigmaY) or y/sigmaY.
	If equal weighing than get sigma from the residual sum of squares between model and data.
*/

double DataModeler_getResidualSumOfSquares (DataModeler me, integer *numberOfDataPoints);

autoVEC DataModeler_getZScores (DataModeler me);

autoVEC DataModeler_getChisqScoresFromZScores (DataModeler me, constVEC zscores, bool substituteAverage);

double DataModeler_getDegreesOfFreedom (DataModeler me);

double DataModeler_getChiSquaredQ (DataModeler me, double *out_probability, double *out_ndf);

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *ssreg, double *sstot);

void DataModeler_reportChiSquared (DataModeler me);

autoCovariance DataModeler_to_Covariance_parameters (DataModeler me);

autoTable DataModeler_to_Table_zscores (DataModeler me);

autoDataModeler Table_to_DataModeler (Table me, double xmin, double xmax, integer xcolumn, integer ycolumn, integer scolumn, integer numberOfParameters, kDataModelerFunction type);

#endif /* _DataModeler_h_ */
