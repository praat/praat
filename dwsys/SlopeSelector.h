#ifndef _SlopeSelector_h_
#define _SlopeSelector_h_
/* SlopeSelector.h
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


#include "NUMmedian.h"
#include "PermutationInversionCounter.h"
#include "NUM2.h"
#include "Permutation.h"

/*
    The algorithm to find the kth (or median) slope in O (n log(n)) time is described in
        Matousek (1991): Randomized optimal algorithm for slope selection.
        Information processing letters 39: 183-187.
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

#define SlopeSelector_TheilSen 1
#define SlopeSelector_LeastSquares 2
#define SlopeSelector_MedianOfMedians 3

class SlopeSelector {
private:
    integer numberOfDualLines;
    integer sampleSize;
    integer krt;
    integer maxNumberOfIntervalCrossings;
    autoINTVEC sortedRandomCrossingIndices;
    autoINTVEC currentCrossingIndices;
    autoVEC lineCrossings, intervalCrossings;
    autoPermutation lineRankingAtBeginX;
    autoPermutation lineRankingAtEndX;  // n..1
    autoPermutation lineRankingAtBeginXPrevious; // 1..n
    autoPermutation lineRankingAtEndXPrevious;  // n..1
    autoPermutation inverseOfLineRankingAtBeginX; // 1..n
    PermutationInversionCounter inversionCounter;
    constVEC ax, ay;

    void initData (constVEC const& x, constVEC const& y) {
        Melder_assert (x.size == y.size && x.size > 2);
        our ax = x;
        our ay = y;
    }

    void initTheilSen () {
        numberOfDualLines = ax.size;
        sampleSize = std::max (50_integer, Melder_iroundUp (sqrt (numberOfDualLines)));
        krt = Melder_iroundDown (1.5 * sqrt (sampleSize));
        maxNumberOfIntervalCrossings = numberOfDualLines * (numberOfDualLines - 1) / 2;
        sortedRandomCrossingIndices = raw_INTVEC (sampleSize);
        currentCrossingIndices = raw_INTVEC (sampleSize);
        lineCrossings = raw_VEC (numberOfDualLines); // used for intervals and line crossing
        intervalCrossings = raw_VEC (sampleSize); // used for intervals and line crossing
        lineRankingAtBeginX = Permutation_create (numberOfDualLines, true); // 1..n
        lineRankingAtEndX = Permutation_reverse (lineRankingAtBeginX.get(), 0, 0);  // n..1
        lineRankingAtBeginXPrevious = Permutation_create (numberOfDualLines, true); // 1..n
        lineRankingAtEndXPrevious = Permutation_reverse (lineRankingAtBeginX.get(), 0, 0);  // n..1
        inverseOfLineRankingAtBeginX = Permutation_create (numberOfDualLines, true); // 1..n
    }

    void theilSen (integer kth, double *out_kth, double *out_kp1th) {
        try {
            /*
                SampleSize = 10
                n      tNew      tOld   tOld/tNew
                10     8.821e-05 1.549-05  0.176
                50     0.00052   0.000398  0.769
                100    0.0019    0.001711  0.897
                500    0.0062    0.018     2.925
                1000   0.0149    0.074     4.976
                5000   0.221     2.247    10.186
                10000  0.866     9.673    11.168

                sampleSize = 50

                10     3.862e-05 1.549e-05  0.401
                50     0.000326  0.0003805  1.167
                100    0.000845  0.0016000  1.893
                500    0.003288  0.0182259  5.543
                1000   0.013693  0.0738728  5.395
                5000   0.225863  2.2403669  9.919
                10000  0.911006  9.9953691 10.972
            */
            bool firstTime = true;
            integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
            integer numberOfCrossingsAtBeginX = 0, kappa = kth, ilow, ihigh;

            auto getSortedSlopes = [&] (integer numberOfSlopes) {
                intervalCrossings.resize (numberOfSlopes);
                for (integer i = 1; i <= numberOfSlopes; i ++) {
                    getInversionFromIndex (numberOfDualLines, currentCrossingIndices [i], ilow, ihigh);
                    const integer ipoint = numberOfDualLines + 1 - ilow;
                    const integer jpoint = numberOfDualLines + 1 - ihigh;
                    intervalCrossings [i] = (ay [jpoint] - ay [ipoint]) / (ax [jpoint] - ax [ipoint]); // the slope
                }
                sort_VEC_inout (intervalCrossings.get());
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
                        const integer numberOfIntervalCrossings = inversionCounter.getSelectedInversionsNotInOther
                            (lineRankingAtEndX.get(), inverseOfLineRankingAtBeginX.get(), sortedRandomCrossingIndices.get(), currentCrossingIndices.get());
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
                            lineCrossings [iline] = x * ax [ipoint] - ay [ipoint]; // the dual line
                            result -> p [iline] = iline;
                        }
                        std::sort (result -> p.begin(), result -> p.end(),
                            [&] (integer& i1, integer& i2) {
                                return lineCrossings [i1] < lineCrossings [i2];
                             });
                        const integer numberOfCrossingsAtX = Permutation_getNumberOfInversions (result);
                        return numberOfCrossingsAtX;
                    };

                    numberOfCrossingsAtBeginX = getLineCrossingsAtX (beginX, lineRankingAtBeginX.get());
                    if (numberOfCrossingsAtBeginX < kth) {
                        const integer numberOfCrossingsAtEndX = getLineCrossingsAtX (endX, lineRankingAtEndX.get());
                        if (kth < numberOfCrossingsAtEndX + 1) { // we also need k+1;
                            currentNumberOfIntervalCrossings = numberOfCrossingsAtEndX - numberOfCrossingsAtBeginX;
                            Permutation_invert_into (lineRankingAtBeginX.get(), inverseOfLineRankingAtBeginX.get());
                            lineRankingAtEndXPrevious -> p.get()  <<=  lineRankingAtEndX -> p.get();
                            firstTime = false;
                        } else
                            lineRankingAtEndX -> p.get()  <<=  lineRankingAtEndXPrevious -> p.get();
                    } // else is covered by inverseOfLineRankingAtBeginX
                } else { // we are done: currentNumberOfIntervalCrossings <= sampleSize
                    sortedRandomCrossingIndices.resize (currentNumberOfIntervalCrossings);
                    currentCrossingIndices.resize (currentNumberOfIntervalCrossings);
                    for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
                        sortedRandomCrossingIndices [i] = i;
                    (void) inversionCounter.getSelectedInversionsNotInOther
                        (lineRankingAtEndX.get(), inverseOfLineRankingAtBeginX.get(), sortedRandomCrossingIndices.get(), currentCrossingIndices.get());
                    Melder_assert (currentNumberOfIntervalCrossings == inversionCounter.getNumberOfInversionsRegistered ());
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

    double intercept (double slope) {
        for (integer i = 1; i <= numberOfDualLines; i ++)
            lineCrossings [i] = ay [i] - slope * ax [i];
        return num::NUMquantile (lineCrossings.get(), 0.5);
    }

public:

    SlopeSelector () {}

    SlopeSelector (constVEC const& x, constVEC const& y, bool theilSen) {
        initData (x, y);
        if (theilSen)
            initTheilSen ();
    }

    double quantile_theilSen (double factor) {
        double kleft, kright;
        const integer numberOfLines = ax.size * (ax.size - 1) / 2;
        const double place = factor * numberOfLines + 0.5;
        const integer left = Melder_clipped (1_integer, Melder_ifloor (place), numberOfLines);
        Melder_assert (numberOfLines - 1 >= 1);
        theilSen (left, & kleft, & kright);
        const double dif = kright - kleft;
        if (dif == 0.0)
            return kleft;
        return kleft + (place - left) * dif;
    }

    // only for ax.size < 50
    double quantile_orderNSquared (double factor) {
        const integer numberOfLines = ax.size * (ax.size - 1) / 2;
        autoVEC slopes = raw_VEC (numberOfLines);
        integer index = 0;
        for (integer i = 1; i < ax.size; i ++)
            for (integer j = i + 1; j <= ax.size; j ++)
                slopes [++ index] = (ay [j] - ay [i]) / (ax [j] - ax [i]);
        Melder_assert (index == numberOfLines);
        sort_VEC_inout (slopes.get());
        const double medianSlope = NUMquantile (slopes.get(), factor);
        return medianSlope;
    }

    double slopeQuantile (double factor) {
        const integer split = 50;
        return ( ax.size < split ? quantile_orderNSquared (factor) : quantile_theilSen (factor) );
    }

    double slopeQuantile (double factor, double *out_intercept) {
        const double slope = slopeQuantile (factor);
        if (out_intercept)
            *out_intercept = intercept (slope);
        return slope;
    }


    double leastSquares (double *out_intercept) {
        const double xsum = NUMsum (ax);
        const double xmean = xsum / ax.size;
        longdouble variance = 0.0, m = 0.0;
        for (integer i = 1; i <= ax.size; i ++) {
            const double t = ax [i] - xmean;
            variance += t * t;
            m += t * ay [i];
        }
        // y = m*x + b
        m /= variance;
        if (out_intercept) {
            const double ysum = NUMsum (ay);
            *out_intercept = (ysum - m * xsum) / ax.size;
        }
        return m;
    }

};

void timeSlopeSelection ();

#endif / * _SlopeSelector_h_ */
