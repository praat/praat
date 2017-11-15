/* KNN_prune.cpp
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2011,2015,2016,2017 Paul Boersma
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
 * os 2007/05/29 Initial release
 * os 2009/01/23 Bugfix: Rejects classifiers containing 0 or 1 instances
 * pb 2010/06/06 removed some array-creations-on-the-stack
 * pb 2011/04/14 C++
 * pb 2011/04/16 removed memory leaks
 */

#include "KNN_prune.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Prune                                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_prune_prune
(
    KNN me,     // the classifier to be pruned
    double n,   // pruning degree: noise, 0 <= n <= 1
    double r,   // pruning redundancy: noise, 0 <= n <= 1
    integer k   // k(!)
)
{
	autoCategories uniqueCategories = Categories_selectUniqueItems (my output.get());
	if (Categories_getSize (uniqueCategories.get()) == my nInstances)
		return 0;
	integer removals = 0;
	integer ncandidates = 0;
	autoNUMvector <integer> candidates ((integer) 0, my nInstances - 1);
	if (my nInstances <= 1)
		return 0;
	for (integer y = 1; y <= my nInstances; y ++) {
		if (KNN_prune_noisy (my input.get(), my output.get(), y, k)) {
			if (n == 1 || NUMrandomUniform (0, 1) <= n) {
				KNN_removeInstance (me, y);
				++ removals;
			}
		}
	}
	for (integer y = 1; y <= my nInstances; ++ y) {
		if (KNN_prune_superfluous (my input.get(), my output.get(), y, k, 0) && ! KNN_prune_critical (my input.get(), my output.get(), y, k))
			candidates [ncandidates ++] = y;
	}
	KNN_prune_sort (my input.get(), my output.get(), k, candidates.peek(), ncandidates);
	for (integer y = 0; y < ncandidates; ++ y) {
		if (KNN_prune_superfluous (my input.get(), my output.get(), candidates [y], k, 0) && ! KNN_prune_critical (my input.get(), my output.get(), candidates [y], k)) {
			if (r == 1.0 || NUMrandomUniform (0.0, 1.0) <= r) {
				KNN_removeInstance (me, candidates[y]);
				for (integer i = y + 1; i < ncandidates; ++ i) {
					if(candidates[i] > candidates[y])
						-- candidates[i];
				}
				++ removals;
			}
		}
	}
	return removals;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// sort indices according to pruning order defined by rule 2                               //
/////////////////////////////////////////////////////////////////////////////////////////////

void KNN_prune_sort
(
    PatternList p,     // source
    Categories c,      // source
    integer k,         // k(!)
    integer * indices, // indices of instances to be sorted
    integer nindices   // the number of instances to be sorted
)
{
	integer n = nindices;
	autoNUMvector <integer> h ((integer) 0, nindices - 1);
	for (integer cc = 0; cc < nindices; ++ cc)
		h [cc] = KNN_friendsAmongkNeighbours (p, p, c, indices [cc], k);
	while (-- n) {   // insertion-sort, is heap-sort worth the effort?
		for (integer m = n; m < nindices - 1; m ++) {
			if (h [m - 1] > h[m]) 
				break;
			if (h [m - 1] < h[m]) {
				OlaSWAP (integer, indices [m - 1], indices [m]);
			} else {
				if (KNN_nearestEnemy (p, p, c, indices [m - 1]) < KNN_nearestEnemy (p, p, c, indices [m])) {
					OlaSWAP (integer, indices [m - 1], indices [m]);
				} else {
					if (NUMrandomUniform (0, 1) > 0.5) {
						OlaSWAP (integer, indices [m - 1], indices [m]);
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// k-coverage                                                                              //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_prune_kCoverage
(
    PatternList p,     // source
    Categories c,      // source
    integer y,         // source instance index
    integer k,         // k(!)
    integer * indices  // Out: kCoverage set
)
{
	Melder_assert (y <= p->ny);
	Melder_assert (k > 0 && k <= p->ny);
	integer cc = 0;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <integer> tempindices ((integer) 0, p -> ny - 1);
	for (integer yy = 1; yy <= p -> ny; yy ++) {
		if (y != yy && FeatureWeights_areFriends (c->at [y], c->at [yy])) {
			integer n = KNN_kNeighboursSkip (p, p, fws.get(), yy, k, tempindices.peek(), y);
			while (n) {
				Melder_assert (n <= p -> ny);
				if (tempindices [-- n] == y) {
					indices [cc ++] = yy;
					break;
				}
			}
		}
	}
	return cc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// testing for superfluousness                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////

int KNN_prune_superfluous
(
    PatternList p,   // source
    Categories c,    // source
    integer y,       // source instance index
    integer k,       // k(!)
    integer skipper  // Skipping instance skipper
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <integer> indices ((integer) 0, k - 1);
	autoNUMvector <integer> freqindices ((integer) 0, k - 1);
	autoNUMvector <double> distances ((integer) 0, k - 1);
	autoNUMvector <double> freqs ((integer) 0, k - 1);
	if (! KNN_kNeighboursSkip (p, p, fws.get(), y, k, indices.peek(), skipper)) return 0;
	integer ncategories = KNN_kIndicesToFrequenciesAndDistances (c, k, indices.peek(), distances.peek(), freqs.peek(), freqindices.peek());
	int result = FeatureWeights_areFriends (c->at [y], c->at [freqindices [KNN_max (freqs.peek(), ncategories)]]);
	if (result)
		return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// testing for criticalness                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

int KNN_prune_critical
(
    PatternList p,  // source
    Categories c,   // source
    integer y,      // source instance index
    integer k       // k(!)
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <integer> indices ((integer) 0, k - 1);
	integer ncollected = KNN_kNeighboursSkip (p, p, fws.get(), y, k, indices.peek(), y);
	for (integer ic = 0; ic < ncollected; ic ++) {
		if (! KNN_prune_superfluous (p, c, indices [ic], k, 0) || ! KNN_prune_superfluous (p, c, indices [ic], k, y)) {
			return 1;
		}
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// testing for noisyness                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////////

int KNN_prune_noisy
(
    PatternList p,  // source
    Categories c,   // source
    integer y,      // source instance index
    integer k       // k(!)
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <integer> indices ((integer) 0, p->ny - 1);    // the coverage is not bounded by k but by n
	integer reachability = KNN_kNeighboursSkip (p, p, fws.get(), y, k, indices.peek(), y);
	integer coverage = KNN_prune_kCoverage (p, c, y, k, indices.peek());
	if (! KNN_prune_superfluous (p, c, y, k, 0) && reachability > coverage)
		return 1;
	return 0;
}

/* End of file KNN_prune.cpp */
