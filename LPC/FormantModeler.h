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
#include "Covariance.h"
#include "DataModeler.h"
#include "Formant.h"
#include "OptimalCeilingTier.h"
#include "Pitch.h"
#include "Sound_to_Formant.h"
#include "Table.h"

#include "FormantModeler_enums.h"

#include "FormantModeler_def.h"

autoFormant Formant_extractPart (Formant me, double tmin, double tmax);

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfDataPoints, integer numberOfTracks, integer numberOfParameters);

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfDataPoints, constINTVEC const& numberOfParameters);

double FormantModeler_indexToTime (FormantModeler me, integer index);

void FormantModeler_fit (FormantModeler me);

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	integer itrack, integer iterm, bool scaled, integer numberOfPoints, bool garnish);

void FormantModeler_setDataWeighing (FormantModeler me, integer fromTrack, integer toTrack, kFormantModelerWeights weighFormants);

void FormantModeler_setParameterValueFixed (FormantModeler me, integer itrack, integer index, double value);

void FormantModeler_setParametersFree (FormantModeler me, integer fromTrack, integer toTrack, integer fromIndex, integer toIndex);

void FormantModeler_setParameterValuesToZero (FormantModeler me, integer fromTrack, integer toTrack, double numberOfSigmas);

void FormantModeler_setTolerance (FormantModeler me, double tolerance);

void FormantModeler_drawModel_inside (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, MelderColour oddTracks, MelderColour evenTracks, integer numberOfPoints);

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, bool errorBars,
	MelderColour oddTracks, MelderColour evenTracks, bool garnish);

void FormantModeler_speckle_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, bool errorBars, MelderColour oddTracks, MelderColour evenTracks);

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, MelderColour oddTracks, MelderColour evenTracks, bool garnish);

void FormantModeler_drawTracks_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax, integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, MelderColour oddTracks, MelderColour evenTracks, bool garnish);

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack, double numberOfSigmas, conststring32 mark, double marksFontSize, MelderColour oddTracks, MelderColour evenTracks, bool garnish);

void FormantModeler_drawCumulativeChiScores (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish);

void FormantModeler_drawVariancesOfShiftedTracks (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	kFormantModelerTrackShift shiftDirection, integer fromTrack, integer toTrack, bool garnish);

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, integer itrack, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish);

autoTable FormantModeler_to_Table_zscores (FormantModeler me);

autoCovariance FormantModeler_to_Covariance_parameters (FormantModeler me, integer itrack);


/*
	Precondition: fromFormant >= 1
	toFormant == 0 signals 'all' data
*/
double FormantModeler_getChiSquaredQ (FormantModeler me, integer fromTrack, integer toTrack, double *probability, double *ndf);

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, integer fromTrack, integer toTrack);

double FormantModeler_getStandardDeviation (FormantModeler me, integer itrack);

double FormantModeler_getResidualSumOfSquares (FormantModeler me, integer itrack, integer *numberOfDataPoints);

double FormantModeler_getEstimatedValueAtTime (FormantModeler me, integer itrack, double time);

integer FormantModeler_getNumberOfParameters (FormantModeler me, integer itrack);

integer FormantModeler_getNumberOfFixedParameters (FormantModeler me, integer itrack);

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, integer itrack, integer index);

/*
	Precondition: fromIndex >= 1 && fromFormant >= 1
	toFormant == 0 && toIndex == 0 signal 'all' data
*/
double FormantModeler_getVarianceOfParameters (FormantModeler me, integer fromTrack, integer toTrack, integer fromIndex, integer toIndex, integer *numberOfFreeParameters);

kDataModelerParameter FormantModeler_getParameterStatus (FormantModeler me, integer itrack, integer index);

integer FormantModeler_getNumberOfDataPoints (FormantModeler me);

integer FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, integer itrack);

void FormantModeler_setDataPointStatus (FormantModeler me, integer itrack, integer index, kDataModelerData status);

kDataModelerData FormantModeler_getDataPointStatus (FormantModeler me, integer itrack, integer index);

double FormantModeler_getDataPointValue (FormantModeler me, integer itrack, integer index);

void FormantModeler_setDataPointValue (FormantModeler me, integer itrack, integer index, double value);

double FormantModeler_getDataPointSigma (FormantModeler me, integer itrack, integer index);

void FormantModeler_setDataPointSigma (FormantModeler me, integer itrack, integer index, double sigma);

double FormantModeler_getDegreesOfFreedom (FormantModeler me, integer itrack);

integer FormantModeler_getNumberOfTracks (FormantModeler me);

double FormantModeler_getModelValueAtTime (FormantModeler me, integer itrack, double time);

double FormantModeler_getModelValueAtIndex (FormantModeler me, integer itrack, integer index);

double FormantModeler_getWeightedMean (FormantModeler me, integer itrack);

double FormantModeler_getParameterValue (FormantModeler me, integer itrack, integer iparameter);

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, integer numberOfTracks,
	integer numberOfParametersPerTrack);

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, constINTVEC const& numberOfParametersPerTrack);

autoFormant FormantModeler_to_Formant (FormantModeler me, bool estimate, bool estimateUndefined);

autoFormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas);

/*
	Get roughness criterion value according to Weenink's (2015) measure
	W = (var/k)^t * (chi^2/d), where
	var is the sum of all variances of all parameters of all modelled formants,
	k is the total number of parameters to mode all tracks, chi^2 is the combined chi-squared of all
	the modelled tracks, d is the combined degrees of freedom, and t is a number that if chosen
	larger than 1 guarantees that for tracks that only differ in bandwidth the one with the largest
	bandwidth obtains a higher value for W.
	A lower value for W means a smoother track.
	
	The routine returns log10 (W).
*/
double FormantModeler_getStress (FormantModeler me, integer fromTrack, integer toTrack,
	integer numberOfParametersPerTrack, double power);

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, integer track1, integer track2, int type);

void FormantModeler_reportChiSquared (FormantModeler me);

integer Formants_getSmoothestInInterval (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, bool useConstraints, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoFormant Formants_extractSmoothestPart (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power);

autoFormant Formants_extractSmoothestPart_withFormantsConstraints (CollectionOf<structFormant>* me, double tmin, double tmax, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3);

autoDataModeler FormantModeler_extractDataModeler (FormantModeler me, integer itrack);

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
