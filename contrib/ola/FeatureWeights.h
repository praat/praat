#ifndef _FeatureWeights_h_
#define _FeatureWeights_h_
/* FeatureWeights.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * os 20080529 Initial release
 * pb 2011/03/08
 */

/////////////////////////////////////////////////////
// Praat datatypes                                 //
/////////////////////////////////////////////////////

#include "Data.h"
#include "TableOfReal.h"
#include "Pattern.h"
#include "Categories.h"

/////////////////////////////////////////////////////
// Miscs                                           //
/////////////////////////////////////////////////////

#include "OlaP.h"

/////////////////////////////////////////////////////
// Praat specifics                                 //
/////////////////////////////////////////////////////

#include "FeatureWeights_def.h"

/////////////////////////////////////////////////////
// Private definitions and macros                  //
/////////////////////////////////////////////////////

#define FeatureWeights_areFriends(x,y)  ! SimpleString_compare (x,y)
#define FeatureWeights_areEnemies(x,y)  SimpleString_compare (x,y)

/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////

// Create
autoFeatureWeights FeatureWeights_create
(
    long nweights           // number of weights
);

// Compute prior probabilities
long FeatureWeights_computePriors
(
    Categories c,           // source categories
    long * indices,         // Out: instances indices ..
    double * priors         // Out: .. and their prior probabilities
);

// Compute feature weights (obsolete)
autoFeatureWeights FeatureWeights_compute
(
    Pattern pp,             // Source pattern
    Categories c,           // Source categories
    long k                  // k(!)
);

// Compute feature weights according to the RELIEF-F algorithm
autoFeatureWeights FeatureWeights_computeRELIEF
(
    Pattern pp,             // source pattern
    Categories c,           // source categories
    long k                  // k(!)
);

/* End of file FeatureWeights.h */
#endif
