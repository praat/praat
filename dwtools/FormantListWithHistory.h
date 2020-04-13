#ifndef _FormantListWithHistory_h_
#define _FormantListWithHistory_h_
/* FormantListWithHistory.h
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
#include "LPC.h"
#include "Sound.h"

#include "FormantListWithHistory_def.h"

/*
	A FormantListWithHistory is an ordered collection of Formants, where
	all Formants in the list have the same domain and the same sampling.
	They have all been analysed with the same parameters except 'Maximum formant (Hz)'
*/

autoFormantListWithHistory Sound_to_FormantListWithHistory_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double minimumCeiling, double maximumCeiling, integer numberOfCeilings, double tol1, double tol2, double huberNumberOfStdDev, double tol, integer maximumNumberOfIterations);

#endif /* _FormantListWithHistory_h_ */
