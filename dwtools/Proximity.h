#ifndef _Proximity_h_
#define _Proximity_h_
/* Proximity.h
 *
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/

#include "TableOfReal.h"

Thing_define (Proximity, TableOfReal) {
};

void Proximity_init (Proximity me, integer numberOfPoints);

Thing_define (Dissimilarity, Proximity) {
};

autoDissimilarity Dissimilarity_create (integer numberOfPoints);

autoDissimilarity Dissimilarity_createLetterRExample (double noiseStd);

double Dissimilarity_getAdditiveConstant (Dissimilarity me);
/*
	Get the best estimate for the additive constant:
		"distance = dissimilarity + constant"
	F. Cailliez (1983), The analytical solution of the additive constant problem, Psychometrika 48, 305-308.
*/

Thing_define (Similarity, Proximity) {
};

autoSimilarity Similarity_create (integer numberOfPoints);

Collection_define (ProximityList, OrderedOf, Proximity) {
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

Collection_define (DissimilarityList, OrderedOf, Dissimilarity) {
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

#endif /* _Proximity_h_ */
