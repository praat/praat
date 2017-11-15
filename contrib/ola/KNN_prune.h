#ifndef _KNN_prune_h_
#define _KNN_prune_h_

/* KNN_prune.h
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
 * pb 2011/03/08 C++
 */

/////////////////////////////////////////////////////
// Prune auxs                                      //
/////////////////////////////////////////////////////

#include "KNN.h"
#include "FeatureWeights.h"
#include "OlaP.h"

/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////

// Prune
integer KNN_prune_prune
(
    KNN me,             // the classifier to be pruned
    double n,           // pruning degree: noise, 0 <= n <= 1
    double r,           // pruning redundancy: noise, 0 <= n <= 1
    integer k           // k(!)
);

// sort indices according to pruning order defined by rule 2
void KNN_prune_sort
(
    PatternList p,      // source
    Categories c,       // source
    integer k,          // k(!)
    integer *indices,   // indices of instances to be sorted
    integer nindices    // the number of instances to be sorted
);

// k-coverage
integer KNN_prune_kCoverage
(
    PatternList p,      // source
    Categories c,       // source
    integer y,          // source instance index
    integer k,          // k(!)
    integer * indices   // Out: kCoverage set
);

// testing for superfluousness
int KNN_prune_superfluous
(
    PatternList p,      // source
    Categories c,       // source
    integer y,          // source instance index
    integer k,          // k(!)
    integer skipper     // Skipping instance skipper
);

// testing for criticalness
int KNN_prune_critical
(
    PatternList p,      // source
    Categories c,       // source
    integer y,          // source instance index
    integer k           // k(!)
);

// testing for noisyness
int KNN_prune_noisy
(
    PatternList p,      // source
    Categories c,       // source
    integer y,          // source instance index
    integer k           // k(!)
);

/* End of file KNN_prune.h */
#endif
