/* Pattern_to_Categories_cluster.h
 *
 * Copyright (C) 2007-2008 Ola Söder
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
 * os 20070529 Initial release
 * pb 2011/03/08 C++
 */

/////////////////////////////////////////////////////
// DEBUG                                           //
/////////////////////////////////////////////////////

//#define CLUSTERING_DEBUG

/////////////////////////////////////////////////////
// Praat datatypes                                 //
/////////////////////////////////////////////////////

#include "PatternList.h"
#include "Categories.h"

/////////////////////////////////////////////////////
// Miscs                                           //
/////////////////////////////////////////////////////

#include "FeatureWeights.h"

/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////

// PatternList_to_Categories_cluster                                                                            
autoCategories PatternList_to_Categories_cluster
(
    PatternList p,          // source
    FeatureWeights fws,     // feature weights
    integer k,              // k(!)
    double s,               // clustersize constraint 0 < s <= 1
    integer m               // reseed maximum
);

/* End of file PatternList_to_Categories_cluster.h */
