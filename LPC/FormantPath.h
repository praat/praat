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

#include "FormantPath_def.h"

/*
	A FormantPath has an ordered collection of Formants and an interval tier.
	All Formants and the IntervalTier have the same domain.
	All Formant have the same sampling.
*/
autoFormantPath FormantPath_create (double fromTime, double toTime, integer numberOfFormantObjects);

integer FormantPath_identifyFormantIndexByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive);

Formant FormantPath_identifyFormantByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive);

void Formant_and_FormantPath_replaceFrames (Formant target, FormantPath sourceList, double fromTime, double toTime, integer sourceIndex);

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumFormantFrequency, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel);

#endif /* _FormantPath_h_ */
