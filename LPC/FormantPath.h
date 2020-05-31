#ifndef _FormantPath_h_
#define _FormantPath_h_
/* FormantPath.h
 *
 * Copyright (C) 2020 David Weenink
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
#include "TextGrid.h"
#include "IntervalTierNavigator.h"

#include "FormantPath_def.h"

/*
	A FormantPath has an ordered collection of Formants and an IntervalTier.
	All Formants and the IntervalTier have the same domain.
	All Formant have the same sampling.
*/

autoFormantPath FormantPath_create (double fromTime, double toTime, integer numberOfFormantObjects);

integer FormantPath_identifyFormantIndexByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive);

Formant FormantPath_identifyFormantByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive);

integer FormantPath_getFormantIndexFromLabel (FormantPath me, conststring32 label);

void FormantPath_replaceFrames (FormantPath me, double fromTime, double toTime, integer formantIndex);

void FormantPath_setNavigationLabels (FormantPath me, Strings navigationLabels, integer navigationTierNumber, kMelder_string criterion);

void FormantPath_setLeftContextNavigationLabels (FormantPath me, Strings navigationLabels, kMelder_string criterion);

void FormantPath_setRightContextNavigationLabels (FormantPath me, Strings navigationLabels, kMelder_string criterion);

void FormantPath_setNavigationContext (FormantPath me,  kContextCombination criterion, bool matchContextOnly);

integer FormantPath_nextNavigationInterval (FormantPath me, integer preferedTierNumber);

integer FormantPath_identifyPathTier (FormantPath me, TextGrid thee);

void FormantPath_mergeTextGrid (FormantPath me, TextGrid thee, integer navigationTierNumber);

static inline autoFormant FormantPath_extractFormant (FormantPath me) {
	return Data_copy (my formant.get());
}

static inline autoTextGrid FormantPath_extractTextGrid (FormantPath me) {
	return Data_copy (my path.get());
}

void FormantPath_reconstructFormant (FormantPath me);

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel);

static inline autoFormantPath Sound_to_FormantPath_burg (Sound me, double timeStep, double maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling) {
	return Sound_to_FormantPath_any (me, kLPC_Analysis::BURG, timeStep, maximumNumberOfFormants, maximumFormantFrequency, windowLength, preemphasisFrequency, ceilingStep, numberOfStepsToACeiling, 1e-6, 1e-6, 1.5, 1e-6, 5, nullptr);
}

autoFormantPath Sound_and_TextGrid_to_FormantPath_any (Sound me, TextGrid thee, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel);

static inline autoFormantPath Sound_and_TextGrid_to_FormantPath_burg (Sound me, TextGrid thee, double timeStep, double maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling) {
	return Sound_and_TextGrid_to_FormantPath_any (me, thee, kLPC_Analysis::BURG, timeStep, maximumNumberOfFormants, maximumFormantFrequency, windowLength, preemphasisFrequency, ceilingStep, numberOfStepsToACeiling, 1e-6, 1e-6, 1.5, 1e-6, 5, nullptr);
}

#endif /* _FormantPath_h_ */
