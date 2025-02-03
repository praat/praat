/* slopeSelection.cpp
 *
 * Copyright (C) 2025 David Weenink
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


#include "median_common.h"
#include "NUMinversionCounter.h"
#include "NUM2.h"
#include "Permutation.h"

/*
    Given points p[i] = (ax[i], ay[i]), i = 1..N, with ax[i] < ax[i+1] for i = 1..N-1.
    Find the line with the k-th largest slope of all the N(N-1)/2 lines connecting
    p[i] = (ax[i],ay[i]) with p[j] = (ax[j],ay[j]) where i = 1..N,and j = i+1..N.
    We consider this problem in the dual space where we have N lines line[l] where
        line[l] is y = ax[N-l+1] * x - ay[N-l+1].
    The ordering of the lines is such that when x = -inf, line [1] has the lowest y-value and
    line [N] the highest. Therefore, the correspondence between line number 'l' and point number 'i' is
    l = N-i+1, for i = 1..N.
    The x coordinate of the intersection of line[i] and line[j] is at x = (ay[N-i+1]-ay[N-j+1])/(ax[N-i+1]-ax[N-j+1]),
    which equals the slope of the line connecting p[i] and p[j].
    In the dual space (x,y) we then search for the intersection with the k-th largest x-coordinate.
    For x = -inf the y-values of line[1]..line[N] are y[1] > y[2] > .. y[N], and
    for x = +inf the y-values of line[1]..line[N] are in reverse order y[N] > y[N-1] > .. y[1].
    If we sort the N crossings of the lines at x = a according to line number this will be a permutation
    of the numbers 1..N. The number of inversions in this permutation has to be equal to the number of intersections
    to the left of x=a (an inversion occurs when i < j && perm[i] > perm[j]).
    The number of inversions in a permutation of size N can be counted in O(N log(N)).
 */

static void slopeSelectionTheil (constVEC const& ax, constVEC const& ay, integer kth, double *out_kth, double *out_kp1th) {
    try {
        Melder_assert (ax.size == ay.size);
        const integer numberOfDualLines = ax.size;
        const integer sampleSize = std::max (50_integer, Melder_iroundUp (sqrt (numberOfDualLines)));
        const integer krt = Melder_iroundDown (1.5 * sqrt (sampleSize));
        const integer maxNumberOfIntervalCrossings = numberOfDualLines * (numberOfDualLines - 1) / 2;
        autoINTVEC sortedRandomCrossingIndices = raw_INTVEC (sampleSize);
        autoINTVEC currentCrossingIndices = raw_INTVEC (sampleSize);
        autoVEC intervalCrossings = raw_VEC (sampleSize);
        autovector<IndexedDouble> lineCrossingsAtX = newvectorraw<IndexedDouble> (numberOfDualLines);
        autoPermutation lineRankingAtBeginX = Permutation_create (numberOfDualLines, true); // 1..n
        autoPermutation lineRankingAtEndX = Permutation_reverse (lineRankingAtBeginX.get(), 0, 0);  // n..1
        autoPermutation lineRankingAtBeginXPrevious = Permutation_create (numberOfDualLines, true); // 1..n
        autoPermutation lineRankingAtEndXPrevious = Permutation_reverse (lineRankingAtBeginX.get(), 0, 0);  // n..1
        autoPermutation positionInRankingAtBeginX = Permutation_create (numberOfDualLines, true); // 1..n
        autovector<IndexedInteger> indexedLineRankingAtEndX = newvectorraw<IndexedInteger> (numberOfDualLines);
        struct InversionCounter<double> inversionCounter (false);
        struct InversionCounter<integer> permutationCounter (false);
        bool firstTime = true;
        integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
        integer numberOfCrossingsAtBeginX = 0, kappa = kth, ilow, ihigh;

        auto getSortedSlopes = [&] (integer numberOfSlopes) {
            for (integer i = 1; i <= numberOfSlopes; i ++) {
                inversionCounter.getInversionFromIndex (currentCrossingIndices [i], & ilow, & ihigh);
                const integer ipoint = numberOfDualLines + 1 - ilow;
                const integer jpoint = numberOfDualLines + 1 - ihigh;
                intervalCrossings [i] = (ay [jpoint] - ay [ipoint]) / (ax [jpoint] - ax [ipoint]); // the slope
            }
            sort_e_VEC_inout (intervalCrossings.get());
        };

        while (true) {
            if (currentNumberOfIntervalCrossings > sampleSize) {

                /*
                    Pick a random sample of size sampleSize from the intersections in the interval (beginX, endX).
                    The first time we have the maximum number of crossings in our interval (-inf, +inf) and
                    the current crossing indices can be chosen directly.
                    In the other cases we have n' < n(n-1)/2 crossings and pick random numbers from the interval [1,n']
                    and we have to search the crossings in O(n log(n)) time.
                */
                if (firstTime) {
                    for (integer i = 1; i <= sampleSize; i ++)
                        currentCrossingIndices [i] = NUMrandomInteger (1, currentNumberOfIntervalCrossings);
                } else {
                    for (integer i = 1; i <= sampleSize; i ++)
                        sortedRandomCrossingIndices [i] = NUMrandomInteger (1, currentNumberOfIntervalCrossings);
                    sort_INTVEC_inout (sortedRandomCrossingIndices.get());
                    permutationCounter.convertToIndexedNumberVector_inplace (lineRankingAtEndX -> p.get(), indexedLineRankingAtEndX.get());
                    const integer numberOfIntervalCrossings = permutationCounter.getSelectedInversionsNotInOtherbySorting
                        (indexedLineRankingAtEndX.get(), sortedRandomCrossingIndices.get(), positionInRankingAtBeginX -> p.get(), currentCrossingIndices.get());
                    Melder_assert (numberOfIntervalCrossings > 0);
                }

                getSortedSlopes (sampleSize);

                const double kappar = ((double) sampleSize) / currentNumberOfIntervalCrossings * (kth - numberOfCrossingsAtBeginX);
                kappa = std::max (1_integer, Melder_iroundDown (kappar));
                const integer kb = std::max (1_integer, kappa - krt);
                const integer ke = std::min (sampleSize, kappa + krt);
                const double beginX = intervalCrossings [kb];
                const double endX = intervalCrossings [ke];
                Melder_assert (beginX <= endX);

                auto getLineCrossingsAtX = [&](double x, mutablePermutation result) {
                    for (integer iline = 1; iline <= numberOfDualLines; iline ++) {
                        const integer ipoint = numberOfDualLines + 1 - iline;
                        lineCrossingsAtX [iline].number = x * ax [ipoint] - ay [ipoint]; // the dual line
                        lineCrossingsAtX [iline].index = iline;
                    }
                    const integer numberOfCrossingsAtX = inversionCounter.getNumberOfInversionsbySorting (lineCrossingsAtX.get());
                    for (integer i = 1; i <= numberOfDualLines; i ++)
                        result -> p [i] = lineCrossingsAtX [i].index;
                    return numberOfCrossingsAtX;
                };

                numberOfCrossingsAtBeginX = getLineCrossingsAtX (beginX, lineRankingAtBeginX.get());
                if (numberOfCrossingsAtBeginX < kth) {
                    const integer numberOfCrossingsAtEndX = getLineCrossingsAtX (endX, lineRankingAtEndX.get());
                    if (kth < numberOfCrossingsAtEndX + 1) { // we also need k+1;
                        currentNumberOfIntervalCrossings = numberOfCrossingsAtEndX - numberOfCrossingsAtBeginX;
                        Permutation_invert_into (lineRankingAtBeginX.get(), positionInRankingAtBeginX.get());
                        lineRankingAtEndXPrevious -> p.get()  <<=  lineRankingAtEndX -> p.get();
                        firstTime = false;
                    } else
                        lineRankingAtEndX -> p.get()  <<=  lineRankingAtEndXPrevious -> p.get();
                } // else is covered by positionInRankingAtBeginX
            } else { // currentNumberOfIntervalCrossings <= sampleSize
                sortedRandomCrossingIndices.resize (currentNumberOfIntervalCrossings);
                currentCrossingIndices.resize (currentNumberOfIntervalCrossings);
                for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
                    sortedRandomCrossingIndices [i] = i;
                permutationCounter.convertToIndexedNumberVector_inplace (lineRankingAtEndX -> p.get(), indexedLineRankingAtEndX.get());
                (void) permutationCounter.getSelectedInversionsNotInOtherbySorting
                    (indexedLineRankingAtEndX.get(), sortedRandomCrossingIndices.get(), positionInRankingAtBeginX -> p.get(), currentCrossingIndices.get());
                Melder_assert (currentNumberOfIntervalCrossings == permutationCounter.getNumberOfInversionsRegistered ());
                intervalCrossings.resize (currentNumberOfIntervalCrossings);
                getSortedSlopes (currentNumberOfIntervalCrossings);
                kappa = kth - numberOfCrossingsAtBeginX;
                if (out_kth)
                    *out_kth = intervalCrossings [kappa];
                if (out_kp1th)
                    *out_kp1th = intervalCrossings [kappa + 1];
                return;
            }
        }
    } catch (MelderError) {
        Melder_throw (U"Slope could not be selected.");
    }
}

static double slopeQuantile_new (constVEC const& x, constVEC const& y, double factor) {
    if (x.size < 2)
        return undefined;
    double kleft, kright;
    const integer numberOfLines = x.size * (x.size - 1) / 2;
    const double place = factor * numberOfLines + 0.5;
    const integer left = Melder_clipped (1_integer, Melder_ifloor (place), numberOfLines);
    Melder_assert (numberOfLines - 1 >= 1);
    slopeSelectionTheil (x, y, left, & kleft, & kright);
    const double dif = kright - kleft;
    if (dif == 0.0)
        return kleft;
    return kleft + (place - left) * dif;
}

static double slopeQuantile_classic (constVEC const& x, constVEC const& y, double factor) {
    if (x.size < 2)
        return undefined;
    const integer numberOfLines = x.size * (x.size - 1) / 2;
    autoVEC slopes = raw_VEC (numberOfLines);
    integer index = 0;
    for (integer i = 1; i < x.size; i ++)
        for (integer j = i + 1; j <= x.size; j ++)
            slopes [++ index] = (y [j] - y [i]) / (x [j] - x [i]);
    Melder_assert (index == numberOfLines);
    sort_e_VEC_inout (slopes.get());
    const double medianSlope = NUMquantile (slopes.get(), factor);
    return medianSlope;
}

double NUMslopeQuantile (constVEC const& x, constVEC const& y, double factor) {
    Melder_assert (x.size == y.size);
    if (x.size < 1)
        return undefined;
    const integer split = 50;
    return ( x.size < split ? slopeQuantile_classic (x, y, factor) : slopeQuantile_new (x, y, factor) );
}

void timeSlopeSelection () {
    try {
        Melder_clearInfo ();
        autoINTVEC sizes {10_integer, 50_integer, 100_integer, 500_integer, 1000_integer, 5000_integer, 10000_integer};
        MelderInfo_writeLine (U"n tNew tOld tOld/tNew");
        for (integer isize = 1; isize <= sizes.size; isize ++) {
            double slope = 1.0, b = 4.0, stddev = 0.1, factor = 0.5;
            const integer n = sizes [isize];
            autoVEC x = from_to_count_VEC (0.0, 10.0, n);
            autoVEC y = randomGauss_VEC (n, 0.0, 0.1);
            for (integer i = 1; i <= n; i ++)
                y[i] = slope * x[i] + NUMrandomGauss (b, stddev);
            Melder_stopwatch ();
            const double slope1 = slopeQuantile_classic (x.get(), y.get(), factor);
            const double t1 = Melder_stopwatch ();
            const double slope2 = slopeQuantile_new (x.get(), y.get(), factor);
            Melder_assert (slope1 == slope2);
            const double t2 = Melder_stopwatch ();
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" ", t1 / t2);
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

/* End of file slopeSelection.cpp */
