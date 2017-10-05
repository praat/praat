#ifndef _FeatureWeights_h_
#define _FeatureWeights_h_
/* FeatureWeights.h
 *
 * Copyright (C) 2007-2008 Ola Söder, 2011,2017 Paul Boersma
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
 * os 20080529 Initial release
 * pb 2011/03/08
 */

/////////////////////////////////////////////////////
// Praat datatypes                                 //
/////////////////////////////////////////////////////

#include "Data.h"
#include "TableOfReal.h"
#include "PatternList.h"
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
    integer nweights        // number of weights
);

// Compute prior probabilities
integer FeatureWeights_computePriors
(
    Categories c,           // source categories
    integer * indices,      // Out: instances indices ..
    double * priors         // Out: .. and their prior probabilities
);

// Compute feature weights (obsolete)
autoFeatureWeights FeatureWeights_compute
(
    PatternList pp,         // Source pattern
    Categories c,           // Source categories
    integer k               // k(!)
);

// Compute feature weights according to the RELIEF-F algorithm
autoFeatureWeights FeatureWeights_computeRELIEF
(
    PatternList pp,         // source pattern
    Categories c,           // source categories
    integer k              // k(!)
);

/* End of file FeatureWeights.h */
#endif
