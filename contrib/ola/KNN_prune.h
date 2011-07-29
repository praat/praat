#ifndef _KNN_prune_h_
#define _KNN_prune_h_

/* KNN_prune.h
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
long KNN_prune_prune
(
    KNN me,             // the classifier to be pruned
    double n,           // pruning degree: noise, 0 <= n <= 1
    double r,           // pruning redundancy: noise, 0 <= n <= 1
    long k              // k(!)
);

// sort indices according to pruning order defined by rule 2
void KNN_prune_sort
(
    Pattern p,          // source
    Categories c,       // source
    long k,             // k(!)
    long * indices,     // indices of instances to be sorted
    long nindices       // the number of instances to be sorted
);

// k-coverage
long KNN_prune_kCoverage
(
    Pattern p,          // source
    Categories c,       // source
    long y,             // source instance index
    long k,             // k(!)
    long * indices      // Out: kCoverage set
);

// testing for superfluousness
int KNN_prune_superfluous
(
    Pattern p,          // source
    Categories c,       // source
    long y,             // source instance index
    long k,             // k(!)
    long skipper        // Skipping instance skipper
);

// testing for criticalness
int KNN_prune_critical
(
    Pattern p,          // source
    Categories c,       // source
    long y,             // source instance index
    long k              // k(!)
);

// testing for noisyness
int KNN_prune_noisy
(
    Pattern p,          // source
    Categories c,       // source
    long y,             // source instance index
    long k              // k(!)
);

/* End of file KNN_prune.h */
#endif
