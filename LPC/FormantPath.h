#ifndef _FormantPath_h_
#define _FormantPath_h_
/* FormantPath.h
 *
 * Copyright (C) 2020-2022 David Weenink
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
#include "Formant.h"
#include "Function.h"
#include "LPC.h"
#include "Sound.h"
#include "Table.h"

#include "FormantPath_def.h"

/*
	A FormantPath inherits from Sampled and contains an ordered collection of Formant objects.
	All Formant objects have the same domain and sampling as the Formant path.
	The ceilings [1:size of Formant collection] vector contains the formant ceiling used for the corresponding Formant in the collection.
	The path vector [1:numberOfFrames], selects a frame from one of the Formant objects.
*/

autoFormantPath FormantPath_create (double xmin, double xmax, integer nx, double dx, double x1, integer numberOfCandidates);

integer FormantPath_getNumberOfFormantTracks (FormantPath me);

autoTable FormantPath_downTo_Table_optimalInterval (FormantPath me, double tmin, double tmax, 
	constINTVEC const& parameters, double powerf, bool includeFrameNumber, bool includeTime, integer numberOfTimeDecimals,
	bool includeIntensity, integer numberOfIntensityDecimals, bool includeNumberOfFormants, integer numberOfFrequencyDecimals,
	bool includeBandwidths, bool includeOptimalCeiling, bool includeMinimumStress);

autoTable FormantPath_downTo_Table_stresses (FormantPath me, double tmin, double tmax, constINTVEC const& parameters,
	double powerf, integer numberOfStressDecimals, bool includeIntervalTimes, integer numberOfTimeDecimals);

autoFormant FormantPath_extractFormant (FormantPath me);

autoMatrix FormantPath_to_Matrix_qSums (FormantPath me, integer numberOfTracks);
autoMatrix FormantPath_to_Matrix_transition (FormantPath me, integer numberOfTracks, bool maximumCosts);
autoMatrix FormantPath_to_Matrix_stress (FormantPath me, double windowLength, constINTVEC const& parameters, double powerf);

double FormantPath_getStressOfCandidate (FormantPath me, double tmin, double tmax, integer fromFormant, integer toFormant,
	constINTVEC const& parameters, double powerf, integer candidate);

autoVEC FormantPath_getStressOfCandidates (FormantPath me, double tmin, double tmax, integer fromFormant, integer toFormant,
	constINTVEC const& parameters, double powerf);

double FormantPath_getOptimalCeiling (FormantPath me, double tmin, double tmax, constINTVEC const& parameters, double powerf);

autoINTVEC FormantPath_getOptimumPath (FormantPath me, double qWeight, double frequencyChangeWeight, double stressWeight,
	double ceilingChangeWeight,	double intensityModulationStepSize, double windowLength, constINTVEC const& parameters,
	double powerf, autoMatrix *out_delta);

void FormantPath_setPath (FormantPath me, double tmin, double tmax, integer selectedCandidate);
void FormantPath_setOptimalPath (FormantPath me, double tmin, double tmax, constINTVEC const& parameters, double powerf);

void FormantPath_pathFinder (FormantPath me, double qWeight, double frequencyChangeWeight, double stressWeight, 
	double ceilingChangeWeight, double intensityModulationStepSize, double windowLength, constINTVEC const& parameters,
	double powerf);

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants,
	double formantCeiling, double analysisWidth, double preemphasisFrequency, double ceilingExtensionFraction,
	integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev,
	double huber_tol, integer huber_maximumNumberOfIterations, autoSound *out_sourcesMultiChannel
);

static inline autoFormantPath Sound_to_FormantPath_burg (Sound me, double timeStep, double maximumNumberOfFormants,
	double formantCeiling, double analysisWidth, double preemphasisFrequency, double ceilingExtensionFraction, integer numberOfStepsToACeiling)
{
	return Sound_to_FormantPath_any (me, kLPC_Analysis::BURG, timeStep, maximumNumberOfFormants,
			formantCeiling, analysisWidth, preemphasisFrequency, ceilingExtensionFraction, numberOfStepsToACeiling, 1e-6, 1e-6, 1.5, 1e-6, 5, nullptr);
}

void FormantPath_drawAsGrid (FormantPath me, Graphics g, double tmin, double tmax, double fmax, 
	integer fromFormant, integer toFormant, bool showBandwidths, MelderColour oddNumberedFormants, MelderColour evenNumberedFormants,
	integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y, double yGridLineEvery_Hz,
	double xCursor, double yCursor, MelderColour selected, constINTVEC const& parameters,
	bool markCandidatesWithinPath, bool showStress, double powerf, bool showEstimatedModels, bool garnish);

void FormantPath_drawAsGrid_inside (FormantPath me, Graphics g, double tmin, double tmax, double fmax,
	integer fromFormant, integer toFormant, bool showBandwidths, MelderColour oddNumberedFormants, MelderColour evenNumberedFormants,
	integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y, double yGridLineEvery_Hz,
	double xCursor, double yCursor, MelderColour selected, constINTVEC const& parameters,
	bool markCandidatesWithinPath, bool showStress, double powerf, bool showEstimatedModels, bool garnish);

#endif /* _FormantPath_h_ */
