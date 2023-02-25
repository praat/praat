#ifndef _FormantPath_h_
#define _FormantPath_h_
/* FormantPath.h
 *
 * Copyright (C) 2020-2023 David Weenink
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
#include "TextGrid.h"

#include "FormantPath_def.h"

/*
	A FormantPath inherits from Sampled and contains:
	
	1. An ordered collection of N Formant objects.
	2. A vector of size N with ceiling frequencies in hertz. Ceiling [i] contains the formant ceiling frequency
	   that was used to calculate the corresponding Formant from the sound.
	3. A TextGrid in which an intervalTier determines a path.
	   Each interval is labeled with an integer number that	refers to one of the Formant objects (an integer number 
	   was chosen instead of the ceiling frequency wich is a real number, because integers can be more easily compared 
	   for equivalence than real numbers.)
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
	integer numberOfStepsUpDown, double marple_tol1, double marple_tol2, double huber_numberOfStdDev,
	double huber_tol, integer huber_maximumNumberOfIterations, autoSound *out_sourcesMultiChannel
);

static inline autoFormantPath Sound_to_FormantPath_burg (Sound me, double timeStep, double maximumNumberOfFormants,
	double formantCeiling, double analysisWidth, double preemphasisFrequency, double ceilingExtensionFraction, integer numberOfStepsUpDown)
{
	return Sound_to_FormantPath_any (me, kLPC_Analysis::BURG, timeStep, maximumNumberOfFormants,
			formantCeiling, analysisWidth, preemphasisFrequency, ceilingExtensionFraction, numberOfStepsUpDown, 1e-6, 1e-6, 1.5, 1e-6, 5, nullptr);
}

static inline autoFormantPath Sound_to_FormantPath_robust (Sound me, double timeStep, double maximumNumberOfFormants,
	double formantCeiling, double analysisWidth, double preemphasisFrequency, double numberOfStandardDeviations, 
	integer maximumNumberOfIterations, double tolerance, double ceilingExtensionFraction, integer numberOfStepsUpDown)
{
	return Sound_to_FormantPath_any (me, kLPC_Analysis::ROBUST, timeStep, maximumNumberOfFormants,
			formantCeiling, analysisWidth, preemphasisFrequency, ceilingExtensionFraction, numberOfStepsUpDown, 1e-6, 1e-6, numberOfStandardDeviations, tolerance, maximumNumberOfIterations, nullptr);
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

void FormantPath_getCandidateAtTime (FormantPath me, double time, double *out_tmin, double *out_tmax, integer *out_candidate);

integer FormantPath_getCandidateInFrame (FormantPath me, integer iframe);

integer FormantPath_getUniqueCandidateInInterval (FormantPath me, double tmin, double tmax);

inline conststring32 FormantPath_getCeilingFrequency_string (FormantPath me, integer candidate) {
	Melder_assert (candidate > 0 && candidate <= my ceilings.size);
	const double ceilingFrequency = my ceilings [candidate];
	return Melder_double (ceilingFrequency);
}

/*
	Only needed to convert from version 0 
*/
autoTextGrid FormantPath_to_TextGrid_version0 (FormantPath me, INTVEC const& path);

#endif /* _FormantPath_h_ */
