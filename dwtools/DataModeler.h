#ifndef _DataModeler_h_
#define _DataModeler_h_
/* DataModeler.h
 *
 * Copyright (C) 2014 David Weenink
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

#define FormantModeler_DATA_FROM_LOWER 2
#define FormantModeler_DATA_FROM_UPPER 3

#include "Collection.h"
#include "Pitch.h"
#include "Sound_to_Formant.h"
#include "SSCP.h"
#include "Table.h"

#include "DataModeler_def.h"
oo_CLASS_CREATE (DataModeler, Function);
oo_CLASS_CREATE (FormantModeler, Function);

Thing_define (PitchModeler, DataModeler) {
	// overridden methods:
	//	virtual void v_info ();
};

void  DataModeler_init (DataModeler me, double xmin, double xmax, long numberOfDataPoints, long numberOfParameters, int type);

DataModeler DataModeler_create (double xmin, double xmax, long numberOfDataPoints, long numberOfParameters, int type);

void DataModeler_setBasisFunctions (DataModeler me, int type);

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, int connectPoints, double barWidth_mm, double horizontalOffset_mm, int drawDots);

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm);

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, int errorbars, double barWidth_mm, double horizontalOffset_mm, int garnish);

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, double horizontalOffset_mm, int garnish);

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int estimated, long numberOfParameters, double horizontalOffset_mm);

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, int useSigmaY, double numberOfSigmas, wchar_t *mark, int marksFontSize, double horizontalOffset_mm);

/* Get the y-value of the fitted function at x */

void DataModeler_setTolerance (DataModeler me, double tolerance);

void DataModeler_fit (DataModeler me);

// sigmaY used in fit or not.
void DataModeler_setDataWeighing (DataModeler me, int useSigmaY);
long DataModeler_getNumberOfFixedParameters (DataModeler me);
void DataModeler_setParameterValueFixed (DataModeler me, long index, double value);
void DataModeler_setParametersFree (DataModeler me, long fromIndex, long toIndex);
double DataModeler_getParameterValue (DataModeler me, long index);
int DataModeler_getParameterStatus (DataModeler me, long index);
double DataModeler_getParameterStandardDeviation (DataModeler me, long index);
double DataModeler_getVarianceOfParameters (DataModeler me, long fromIndex, long toIndex, long *numberOfFreeParameters);
void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas);
double DataModeler_estimateSigmaY (DataModeler me);

void DataModeler_getExtremaY (DataModeler me, double *ymin, double *ymax);
double DataModeler_getModelValueAtX (DataModeler me, double x);
double DataModeler_getWeightedMean (DataModeler me);

long DataModeler_getNumberOfInvalidDataPoints (DataModeler me);
double DataModeler_getDataPointValue (DataModeler me, long index);
void DataModeler_setDataPointValue (DataModeler me, long index, double value);
int DataModeler_getDataPointStatus (DataModeler me, long index);
void DataModeler_setDataPointStatus (DataModeler me, long index, int status);
void DataModeler_setDataPointSigma (DataModeler me, long index, double sigma);
double DataModeler_getDataPointSigma (DataModeler me, long index);
double DataModeler_getResidualSumOfSquares (DataModeler me, long *numberOfDataPoints);

double *DataModeler_getZScores (DataModeler me, int useSigmaY);
double DataModeler_getDegreesOfFreedom (DataModeler me);
double DataModeler_getChiSquaredQ (DataModeler me, int useSigmaY, double *probability, double *ndf);
double DataModeler_getCoefficientOfDetermination (DataModeler me, double *ssreg, double *sstot);

Formant Formant_extractPart (Formant me, double tmin, double tmax);

Covariance DataModeler_to_Covariance_parameters (DataModeler me);
Table DataModeler_to_Table_zscores (DataModeler me, int useSigmaY);

FormantModeler FormantModeler_create (double tmin, double tmax, long numberOfFormants, long numberOfDataPoints, long numberOfParameters);
void FormantModeler_fit (FormantModeler me);
void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
 	long iformant, long iterm, bool scaled, long numberOfPoints, int garnish);

void FormantModeler_setDataWeighing (FormantModeler me, long fromFormant, long toFormant, int useSigmaY);

void FormantModeler_setParameterValueFixed (FormantModeler me, long iformant, long index, double value);
void FormantModeler_setParametersFree (FormantModeler me, long fromFormant, long toFormant, long fromIndex, long toIndex);
void FormantModeler_setParameterValuesToZero (FormantModeler me, long fromFormant, long toFormant, double numberOfSigmas);

void FormantModeler_setTolerance (FormantModeler me, double tolerance);

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	long fromTrack, long toTrack, int estimated, long numberOfParameters, int errorBars, double barWidth_mm, double horizontalOffset_mm, int garnish);

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, long fromTrack, long toTrack, 
	 int estimated, long numberOfParameters, double horizontalOffset_mm, int garnish);

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, long fromTrack, long toTrack, double numberOfSigmas, int useSigmaY, wchar_t *mark, int marksFontSize, double horizontalOffset_mm, int garnish);

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, long iformant, int useSigmaY, long numberOfQuantiles, double numberOfSigmas, int labelSize, const wchar_t *label, int garnish);

Table FormantModeler_to_Table_zscores (FormantModeler me, int useSigmaY);

Covariance FormantModeler_to_Covariance_parameters (FormantModeler me, long iformant);

double FormantModeler_getChiSquaredQ (FormantModeler me, long fromFormant, long toFormant, int useSigmaY, double *probability, double *ndf);

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, long fromFormant, long toFormant);
double FormantModeler_getStandardDeviation (FormantModeler me, long iformant);
double FormantModeler_getResidualSumOfSquares (FormantModeler me, long iformant, long *numberOfDataPoints);
double FormantModeler_getEstimatedValueAtTime (FormantModeler me, long iformant, double time);

long FormantModeler_getNumberOfParameters (FormantModeler me, long iformant);
long FormantModeler_getNumberOfFixedParameters (FormantModeler me, long iformant);

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, long iformant, long index);
double FormantModeler_getVarianceOfParameters (FormantModeler me, long fromFormant, long toFormant, long fromIndex, long toIndex, long *numberOfFreeParameters);
int FormantModeler_getParameterStatus (FormantModeler me, long iformant, long index);

long FormantModeler_getNumberOfDataPoints (FormantModeler me, long iformant);
long FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, long iformant);

void FormantModeler_setDataPointStatus (FormantModeler me, long iformant, long index, int status);
int FormantModeler_getDataPointStatus (FormantModeler me, long iformant, long index);
double FormantModeler_getDataPointValue (FormantModeler me, long iformant, long index);
void FormantModeler_setDataPointValue (FormantModeler me, long iformant, long index, double value);
double FormantModeler_getDataPointSigma (FormantModeler me, long iformant, long index);
void FormantModeler_setDataPointSigma (FormantModeler me, long iformant, long index, double sigma);

double FormantModeler_getDegreesOfFreedom (FormantModeler me, long iformant);
long FormantModeler_getNumberOfTracks (FormantModeler me);

double FormantModeler_getModelValueAtTime (FormantModeler me, long iformant, double time);
double FormantModeler_getWeightedMean (FormantModeler me, long iformant);

double FormantModeler_getParameterValue (FormantModeler me, long iformant, long iparameter);

FormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, long numberOfFormants, long numberOfParametersPerTrack, int bandwidthEstimatesSigma);
Formant FormantModeler_to_Formant (FormantModeler me, int estimate, int estimateUndefined);

FormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas, int useSigmaY);
double FormantModeler_getSmoothnessValue (FormantModeler me, long fromFormant, long toFormant, long numberOfParametersPerTrack, double power);
double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, long track1, long track2, int type);

long Formants_getSmoothestInInterval (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, int useConstraints, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3);

Formant Formants_extractSmoothestPart (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, double numberOfSigmas, double power);
Formant Formants_extractSmoothestPart_withFormantsConstraints (Collection me, double tmin, double tmax, long numberOfFormantTracks, long numberOfParametersPerTrack, int useBandWidthsForTrackEstimation, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

DataModeler FormantModeler_extractDataModeler (FormantModeler me, long iformant);

PitchModeler Pitch_to_PitchModeler (Pitch me, double tmin, double tmax, long numberOfParameters);

void PitchModeler_draw (PitchModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax, long numberOfParameters, int garnish);

DataModeler Table_to_DataModeler (Table me, double xmin, double xmax, long xcolumn, long ycolumn, long scolumn, long numberOfParameters, int type);

Formant Sound_to_Formant_interval (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, long numberOfFrequencySteps, double preemphasisFrequency, long numberOfFormantTracks, long numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *ceiling);

Formant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime, double windowLength, double timeStep, double minFreq, double maxFreq, long numberOfFrequencySteps, double preemphasisFrequency, long numberOfFormantTracks, long numberOfParametersPerTrack, int weighData, double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3, double *ceiling);
#endif /* _DataModeler_h_ */
