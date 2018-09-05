#ifndef _MDSVec_h_
#define _MDSVec_h_
/* MDSVec.h
 *
 * Copyright (C) 2018 David Weenink
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

#include "Data.h"
#include "Collection.h"
#include "Proximity.h"

#include "MDSVec_def.h"

/*
	An MDSVec object:
	contains vectors of length numberOfPoints (numberOfPoibts - 1) /2 
	with the sorted disparities, and their corresponding row and column indices of 'numberOfPoints' objects
*/

autoMDSVec MDSVec_create (long numberOfPoints);

autoMDSVec Dissimilarity_to_MDSVec (Dissimilarity me);

Collection_define (MDSVecList, OrderedOf, MDSVec) {
};

autoMDSVecList DissimilarityList_to_MDSVecList (DissimilarityList me);

#endif /* MDSVec_def.h */
