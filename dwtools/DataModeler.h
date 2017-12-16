#ifndef _DataModeler_h_
#define _DataModeler_h_
/* DataModeler.h
 *
 * Copyright (C) 2014-2016 David Weenink
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

#define DataModeler_TYPE_POLYNOMIAL 0
#define DataModeler_TYPE_LEGENDRE 1

#define DataModeler_PARAMETER_FREE 0
#define DataModeler_PARAMETER_FIXED 1
#define DataModeler_PARAMETER_UNDEFINED -1

#define DataModeler_DATA_WEIGH_EQUAL 0
#define DataModeler_DATA_WEIGH_SIGMA 1
#define DataModeler_DATA_WEIGH_RELATIVE 2
#define DataModeler_DATA_WEIGH_SQRT 3
#define DataModeler_DATA_WEIGH_SQRT 3

#define DataModeler_DATA_VALID 0
#define DataModeler_DATA_INVALID -1
#define DataModeler_DATA_FROM_FIT 1
#define DataModeler_DATA_FROM_COPY1 2
#define DataModeler_DATA_FROM_COPY2 3
#define DataModeler_DATA_FROM_COPY3 4
#define DataModeler_DATA_FROM_COPY4 5

#define FormantModeler_NOSHIFT_TRACKS 0
#define FormantModeler_UPSHIFT_TRACKS 1
#define FormantModeler_DOWNSHIFT_TRACKS 2

#define FormantModeler_DATA_FROM_LOWER 12
#define FormantModeler_DATA_FROM_UPPER 13

#include "Collection.h"
#include "Pitch.h"
#include "OptimalCeilingTier.h"
#include "Sound_to_Formant.h"
#include "SSCP.h"
#include "Table.h"

#include "DataModeler_def.h"

Thing_define (PitchModeler, DataModeler) {
	// void v_info ()
	//     override;
};

void  DataModeler_init (DataModeler me, double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, int type);

autoDataModeler DataModeler_create (double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, int type);

autoDataModeler DataModeler_createSimple (double xmin, double xmax, integer numberOfDataPoints, char32 *parameters, double gaussianNoiseStd, int type);

void DataModeler_setBasisFunctions (DataModeler me, int type);

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, int connectPoints, double barWidth_mm, double horizontalOffset_mm, int drawDots);

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm);

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm, int garnish);

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, double horizontalOffset_mm, int garnish);

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, integer numberOfParameters, double horizontalOffset_mm);

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, int useSigmaY, char32 *mark, int marksFontSize, double horizontalOffset_mm);

/* Get the y-value of the fitted function at x */

void DataModeler_setTolerance (DataModeler me, double tolerance);

void DataModeler_fit (DataModeler me);

// sigmaY used in fit or not.
void DataModeler_setDataWeighing (DataModeler me, int useSigmaY);

integer DataModeler_getNumberOfFixedParameters (DataModeler me);

void DataModeler_setParameterValue (DataModeler me, integer index, double value, int status);

void DataModeler_setParameterValueFixed (DataModeler me, integer index, double value);

void DataModeler_setParametersFree (DataModeler me, integer fromIndex, integer toIndex);

double DataModeler_getParameterValue (DataModeler me, integer index);

int DataModeler_getParameterStatus (DataModeler me, integer index);

double DataModeler_getParameterStandardDeviation (DataModeler me, integer index);

double DataModeler_getVarianceOfParameters (DataModeler me, integer fromIndex, integer toIndex, integer *numberOfFreeParameters);

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas);

double DataModeler_estimateSigmaY (DataModeler me);

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

int DataModeler_getDataPointStatus (DataModeler me, integer index);

void DataModeler_setDataPointStatus (DataModeler me, integer index, int status);

void DataModeler_setDataPointYSigma (DataModeler me, integer index, double sigma);

double DataModeler_getDataPointYSigma (DataModeler me, integer index);
double DataModeler_getResidualSumOfSquares (DataModeler me, integer *numberOfDataPoints);

void DataModeler_getZScores (DataModeler me, int useSigmaY, double zscores[]);

double DataModeler_getDegreesOfFreedom (DataModeler me);

double DataModeler_getChiSquaredQ (DataModeler me, int useSigmaY, double *probability, double *ndf);

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *ssreg, double *sstot);

void DataModeler_reportChiSquared (DataModeler me, int weighDataType);

autoFormant Formant_extractPart (Formant me, double tmin, double tmax);

autoCovariance DataModeler_to_Covariance_parameters (DataModeler me);

autoTable DataModeler_to_Table_zscores (DataModeler me, int useSigmaY);

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfFormants, integer numberOfDataPoints, integer numberOfParameters);

double FormantModeler_indexToTime (FormantModeler me, integer index);

void FormantModeler_fit (FormantModeler me);

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax, integer iformant, integer iterm, bool scaled, integer numberOfPoints, bool garnish);

void FormantModeler_setDataWeighing (FormantModeler me, integer fromFormant, integer toFormant, int useSigmaY);

void FormantModeler_setParameterValueFixed (FormantModeler me, integer iformant, integer index, double value);

void FormantModeler_setParametersFree (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex);

void FormantModeler_setParameterValuesToZero (FormantModeler me, integer fromFormant, integer toFormant, double numberOfSigmas);

void FormantModeler_setTolerance (FormantModeler me, double tolerance);

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm, int garnish);

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, int estimated, integer numberOfParameters, double horizontalOffset_mm, int garnish);

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, double numberOfSigmas, int useSigmaY, char32 *mark, int marksFontSize, double horizontalOffset_mm, int garnish);

void FormantModeler_drawCumulativeChiScores (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int useSigmaY, int garnish);

void FormantModeler_drawVariancesOfShiftedTracks (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int shiftDirection, integer fromFormant, integer toFormant, bool garnish);

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, integer iformant, int useSigmaY, integer numberOfQuantiles, double numberOfSigmas, int labelSize, const char32 *label, bool garnish);

autoTable FormantModeler_to_Table_zscores (FormantModeler me, int useSigmaY);

autoCovariance FormantModeler_to_Covariance_parameters (FormantModeler me, integer iformant);

double FormantModeler_getChiSquaredQ (FormantModeler me, integer fromFormant, integer toFormant, int useSigmaY, double *probability, double *ndf);

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, integer fromFormant, integer toFormant);

double FormantModeler_getStandardDeviation (FormantModeler me, integer iformant);

double FormantModeler_getResidualSumOfSquares (FormantModeler me, integer iformant, integer *numberOfDataPoints);

double FormantModeler_getEstimatedValueAtTime (FormantModeler me, integer iformant, double time);

integer FormantModeler_getNumberOfParameters (FormantModeler me, integer iformant);

integer FormantModeler_getNumberOfFixedParameters (FormantModeler me, integer iformant);

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, integer iformant, integer index);

double FormantModeler_getVarianceOfParameters (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex, integer *numberOfFreeParameters);

int FormantModeler_getParameterStatus (FormantModeler me, integer iformant, integer index);

integer FormantModeler_getNumberOfDataPoints (FormantModeler me);

integer FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, integer iformant);

void FormantModeler_setDataPointStatus (FormantModeler me, integer iformant, integer index, int status);

int FormantModeler_getDataPointStatus (FormantModeler me, integer iformant, integer index);

double FormantModeler_getDataPointValue (FormantModeler me, integer iformant, integer index);

void FormantModeler_setDataPointValue (FormantModeler me, integer iformant, integer index, double value);

double FormantModeler_getDataPointSigma (FormantModeler me, integer iformant, integer index);

void FormantModeler_setDataPointSigma (FormantModeler me, integer iformant, integer index, double sigma);

double FormantModeler_getDegreesOfFreedom (FormantModeler me, integer iformant);

integer FormantModeler_getNumberOfTracks (FormantModeler me);

double FormantModeler_getModelValueAtTime (FormantModeler me, integer iformant, double time);

double FormantModeler_getModelValueAtIndex (FormantModeler me, integer iformant, integer index);

double FormantModeler_getWeightedMean (FormantModeler me, integer iformant);

double FormantModeler_getParameterValue (FormantModeler me, integer iformant, integer iparameter);

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, integer numberOfFormants, integer numberOfParametersPerTrack, int bandwidthEstimatesSigma);

autoFormant FormantModeler_to_Formant (FormantModeler me, int estimate, int estimateUndefined);

autoFormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas, int useSigmaY);

double FormantModeler_getSmoothnessValue (FormantModeler me, integer fromFormant, integer toFormant, integer numberOfParametersPerTrack, double power);

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, integer track1, integer track2, int type);

void FormantModeler_reportChiSquared (FormantModeler me, int weighDataType);

integer Formants_getSmoothestInInterval (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, int useConstraints, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoFormant Formants_extractSmoothestPart (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, double numberOfSigmas, double power);

autoFormant Formants_extractSmoothestPart_withFormantsConstraints (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoDataModeler FormantModeler_extractDataModeler (FormantModeler me, integer iformant);

autoPitchModeler Pitch_to_PitchModeler (Pitch me, double tmin, double tmax, integer numberOfParameters);

void PitchModeler_draw (PitchModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax, integer numberOfParameters, int garnish);

autoDataModeler Table_to_DataModeler (Table me, double xmin, double xmax, integer xcolumn, integer ycolumn, integer scolumn, integer numberOfParameters, int type);

autoFormant Sound_to_Formant_interval (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *optimalCeiling);

autoFormant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *optimalCeiling);

double Sound_getOptimalFormantCeiling (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps, double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power);

autoOptimalCeilingTier Sound_to_OptimalCeilingTier (Sound me, double windowLength, double timeStep, double minCeiling, double maxCeiling, integer numberOfFrequencySteps, double preemphasisFrequency, double smoothingWindow, integer numberOfFormantTracks, integer numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power);

#endif /* _DataModeler_h_ */
