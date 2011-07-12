/* KNN_prune.cpp
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2011 Paul Boersma
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

long KNN_prune_prune
(
    KNN me,     // the classifier to be pruned
    double n,   // pruning degree: noise, 0 <= n <= 1
    double r,   // pruning redundancy: noise, 0 <= n <= 1
    long k      // k(!)
)
{
	autoCategories uniqueCategories = Categories_selectUniqueItems (my output, 1);
	if (Categories_getSize (uniqueCategories.peek()) == my nInstances)
		return 0;
	long removals = 0;
	long ncandidates = 0;
	autoNUMvector <long> candidates (0L, my nInstances - 1);
	if (my nInstances <= 1)
		return 0;
	for (long y = 1; y <= my nInstances; y ++) {
		if (KNN_prune_noisy (my input, my output, y, k)) {
			if (n == 1 || NUMrandomUniform (0, 1) <= n) {
				KNN_removeInstance (me, y);
				++ removals;
			}
		}
	}
	for (long y = 1; y <= my nInstances; ++ y) {
		if (KNN_prune_superfluous (my input, my output, y, k, 0) && ! KNN_prune_critical (my input, my output, y, k))
			candidates [ncandidates ++] = y;
	}
	KNN_prune_sort (my input, my output, k, candidates.peek(), ncandidates);
	for (long y = 0; y < ncandidates; ++ y) {
		if (KNN_prune_superfluous (my input, my output, candidates [y], k, 0) && ! KNN_prune_critical (my input, my output, candidates [y], k)) {
			if (r == 1 || NUMrandomUniform (0, 1) <= r) {
				KNN_removeInstance (me, candidates[y]);
				for (long i = y + 1; i < ncandidates; ++ i) {
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
    Pattern p,      // source
    Categories c,   // source
    long k,         // k(!)
    long * indices, // indices of instances to be sorted
    long nindices   // the number of instances to be sorted
)
{
	long n = nindices;
	autoNUMvector <long> h (0L, nindices - 1);
	for (long cc = 0; cc < nindices; ++ cc)
		h [cc] = KNN_friendsAmongkNeighbours (p, p, c, indices [cc], k);
	while (-- n) {   // insertion-sort, is heap-sort worth the effort?
		for (long m = n; m < nindices - 1; m ++) {
			if (h [m - 1] > h[m]) 
				break;
			if (h [m - 1] < h[m]) {
				OlaSWAP (long, indices [m - 1], indices [m]);
			} else {
				if (KNN_nearestEnemy (p, p, c, indices [m - 1]) < KNN_nearestEnemy (p, p, c, indices [m])) {
					OlaSWAP (long, indices [m - 1], indices [m]);
				} else {
					if (NUMrandomUniform (0, 1) > 0.5) {
						OlaSWAP (long, indices [m - 1], indices [m]);
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// k-coverage                                                                              //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_prune_kCoverage
(
    Pattern p,      // source
    Categories c,   // source
    long y,         // source instance index
    long k,         // k(!)
    long * indices  // Out: kCoverage set
)
{
	Melder_assert (y <= p->ny);
	Melder_assert (k > 0 && k <= p->ny);
	long cc = 0;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <long> tempindices (0L, p -> ny - 1);
	for (long yy = 1; yy <= p -> ny; yy++) {
		if (y != yy && FeatureWeights_areFriends ((SimpleString) c -> item [y], (SimpleString) c -> item [yy])) {
			long n = KNN_kNeighboursSkip (p, p, fws.peek(), yy, k, tempindices.peek(), y);
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
    Pattern p,      // source
    Categories c,   // source
    long y,         // source instance index
    long k,         // k(!)
    long skipper    // Skipping instance skipper
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <long> indices (0L, k - 1);
	autoNUMvector <long> freqindices (0L, k - 1);
	autoNUMvector <double> distances (0L, k - 1);
	autoNUMvector <double> freqs (0L, k - 1);
	if (! KNN_kNeighboursSkip (p, p, fws.peek(), y, k, indices.peek(), skipper)) return 0;
	long ncategories = KNN_kIndicesToFrequenciesAndDistances (c, k, indices.peek(), distances.peek(), freqs.peek(), freqindices.peek());
	int result = FeatureWeights_areFriends ((SimpleString) c -> item [y], (SimpleString) c -> item [freqindices [KNN_max (freqs.peek(), ncategories)]]);
	if (result)
		return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// testing for criticalness                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

int KNN_prune_critical
(
    Pattern p,      // source
    Categories c,   // source
    long y,         // source instance index
    long k          // k(!)
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <long> indices (0L, k - 1);
	long ncollected = KNN_kNeighboursSkip (p, p, fws.peek(), y, k, indices.peek(), y);
	for (long ic = 0; ic < ncollected; ic ++) {
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
    Pattern p,      // source
    Categories c,   // source
    long y,         // source instance index
    long k          // k(!)
)
{
	if (y > p -> ny) y = p -> ny;   // safety belt
	if (k > p -> ny) k = p -> ny;
	autoFeatureWeights fws = FeatureWeights_create (p -> nx);
	autoNUMvector <long> indices (0L, p->ny - 1);    // the coverage is not bounded by k but by n
	long reachability = KNN_kNeighboursSkip (p, p, fws.peek(), y, k, indices.peek(), y); 
	long coverage = KNN_prune_kCoverage (p, c, y, k, indices.peek());
	if (! KNN_prune_superfluous (p, c, y, k, 0) && reachability > coverage)
		return 1;
	return 0;
}

/* End of file KNN_prune.cpp */
