#ifndef _Proximity_and_Distance_h_
#define _Proximity_and_Distance_h_
/* Proximity_and_Distance.h
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

#include "Proximity.h"
#include "Distance.h"
#include "MDS_enums.h"

autoDistance Dissimilarity_to_Distance (Dissimilarity me, kMDS_AnalysisScale scale); 

autoDissimilarity Distance_to_Dissimilarity (Distance me);

autoDistanceList DissimilarityList_to_DistanceList (DissimilarityList me, kMDS_AnalysisScale scale);

#endif /* Proximity_and_Distance_h */
