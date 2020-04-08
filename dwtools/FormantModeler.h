#ifndef _FormantModeler_h_
#define _FormantModeler_h_
/* FormantModeler.h
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

#include "Collection.h"
#include "Pitch.h"
#include "OptimalCeilingTier.h"
#include "Sound_to_Formant.h"
#include "SSCP.h"
#include "Table.h"

#include "FormantModeler_enums.h"

#include "FormantModeler_def.h"

autoFormant Formant_extractPart (Formant me, double tmin, double tmax);

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfFormants, integer numberOfDataPoints, integer numberOfParameters);

double FormantModeler_indexToTime (FormantModeler me, integer index);

void FormantModeler_fit (FormantModeler me);

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	integer iformant, integer iterm, bool scaled, integer numberOfPoints, bool garnish);

void FormantModeler_setDataWeighing (FormantModeler me, integer fromFormant, integer toFormant, kFormantModelerWeights weighFormants);

void FormantModeler_setParameterValueFixed (FormantModeler me, integer iformant, integer index, double value);

void FormantModeler_setParametersFree (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex);

void FormantModeler_setParameterValuesToZero (FormantModeler me, integer fromFormant, integer toFormant, double numberOfSigmas);

void FormantModeler_setTolerance (FormantModeler me, double tolerance);

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, bool estimated, integer numberOfParameters,
	bool errorBars, double barWidth_mm, double horizontalOffset_mm, bool garnish);

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, bool estimated, integer numberOfParameters, double horizontalOffset_mm, bool garnish);

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack,
	double numberOfSigmas, conststring32 mark, double marksFontSize, double horizontalOffset_mm, bool garnish);

void FormantModeler_drawCumulativeChiScores (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish);

void FormantModeler_drawVariancesOfShiftedTracks (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	kFormantModelerTrackShift shiftDirection, integer fromFormant, integer toFormant, bool garnish);

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, integer iformant, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish);

autoTable FormantModeler_to_Table_zscores (FormantModeler me);

autoCovariance FormantModeler_to_Covariance_parameters (FormantModeler me, integer iformant);

double FormantModeler_getChiSquaredQ (FormantModeler me, integer fromFormant, integer toFormant, double *probability, double *ndf);

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, integer fromFormant, integer toFormant);

double FormantModeler_getStandardDeviation (FormantModeler me, integer iformant);

double FormantModeler_getResidualSumOfSquares (FormantModeler me, integer iformant, integer *numberOfDataPoints);

double FormantModeler_getEstimatedValueAtTime (FormantModeler me, integer iformant, double time);

integer FormantModeler_getNumberOfParameters (FormantModeler me, integer iformant);

integer FormantModeler_getNumberOfFixedParameters (FormantModeler me, integer iformant);

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, integer iformant, integer index);

double FormantModeler_getVarianceOfParameters (FormantModeler me, integer fromFormant, integer toFormant, integer fromIndex, integer toIndex, integer *numberOfFreeParameters);

kDataModelerParameter FormantModeler_getParameterStatus (FormantModeler me, integer iformant, integer index);

integer FormantModeler_getNumberOfDataPoints (FormantModeler me);

integer FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, integer iformant);

void FormantModeler_setDataPointStatus (FormantModeler me, integer iformant, integer index, kDataModelerData status);

kDataModelerData FormantModeler_getDataPointStatus (FormantModeler me, integer iformant, integer index);

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

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, integer numberOfFormants,
	integer numberOfParametersPerTrack);

autoFormant FormantModeler_to_Formant (FormantModeler me, bool estimate, bool estimateUndefined);

autoFormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas);

double FormantModeler_getSmoothnessValue (FormantModeler me, integer fromFormant, integer toFormant,
	integer numberOfParametersPerTrack, double power);

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, integer track1, integer track2, int type);

void FormantModeler_reportChiSquared (FormantModeler me);

integer Formants_getSmoothestInInterval (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, bool useConstraints, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoFormant Formants_extractSmoothestPart (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power);

autoFormant Formants_extractSmoothestPart_withFormantsConstraints (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoDataModeler FormantModeler_extractDataModeler (FormantModeler me, integer iformant);

autoFormant Sound_to_Formant_interval (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3,
	double *out_optimalCeiling
);

autoFormant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3,
	double *out_optimalCeiling
);

double Sound_getOptimalFormantCeiling (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power
);

autoOptimalCeilingTier Sound_to_OptimalCeilingTier (Sound me,
	double windowLength, double timeStep, double minCeiling, double maxCeiling, integer numberOfFrequencySteps,
	double preemphasisFrequency, double smoothingWindow, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power
);

#endif /* _FormantModeler_h_ */
