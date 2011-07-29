/* Pattern_to_Categories_cluster.h
 *
 * Copyright (C) 2007-2008 Ola Söder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#include "Pattern.h"
#include "Categories.h"

/////////////////////////////////////////////////////
// Miscs                                           //
/////////////////////////////////////////////////////

#include "FeatureWeights.h"

/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////

// Pattern_to_Categories_cluster                                                                            
Categories Pattern_to_Categories_cluster
(
    Pattern p,              // source
    FeatureWeights fws,     // feature weights
    long k,                 // k(!)
    double s,               // clustersize constraint 0 < s <= 1
    long m                  // reseed maximum
);

/* End of file Pattern_to_Categories_cluster.h */
