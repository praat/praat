#ifndef _FormantList_h_
#define _FormantList_h_
/* FormantList.h
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

#include "FormantList_def.h"

/*
	A FormantList is an ordered collection of Formants, where
	all Formants in the list have the same domain and the same sampling.
*/
autoFormantList FormantList_create (double fromTime, double toTime, integer numberOfFormantObjects);

Formant FormantList_identifyFormantByCriterion (FormantList me, kMelder_string which, conststring32 criterion, bool caseSensitive);

void Formant_and_FormantList_replaceFrames (Formant target, FormantList sourceList, double fromTime, double toTime, integer sourceIndex);

autoFormantList Sound_to_FormantList_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumFormantFrequency, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations);

#endif /* _FormantList_h_ */
