/* SlopeSelector.cpp
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
#include "SlopeSelector.h"

#include "oo_DESTROY.h"
#include "SlopeSelector_def.h"
#include "oo_COPY.h"
#include "SlopeSelector_def.h"
#include "oo_EQUAL.h"
#include "SlopeSelector_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SlopeSelector_def.h"
#include "oo_WRITE_TEXT.h"
#include "SlopeSelector_def.h"
#include "oo_READ_TEXT.h"
#include "SlopeSelector_def.h"
#include "oo_WRITE_BINARY.h"
#include "SlopeSelector_def.h"
#include "oo_READ_BINARY.h"
#include "SlopeSelector_def.h"
#include "oo_DESCRIPTION.h"
#include "SlopeSelector_def.h"

Thing_implement (SlopeSelector, Daata, 1);

void structSlopeSelector :: init (constVEC const& x, constVEC const& y, integer sampleSize) {
    numberOfPoints  = x.size;
    our sampleSize = sampleSize;
    crossings = raw_VEC (sampleSize);
    our ax = x;
    our ay = y;
}

void structSlopeSelector :: newDataPoints (constVEC const& x, constVEC const& y) {
    Melder_assert (x.size == y.size);
    Melder_assert (x.size == numberOfPoints);
    our ax = x;
    our ay = y;
}

double structSlopeSelector :: getSlope () {
    const double xsum = NUMsum (ax);
    const double xmean = xsum / ax.size;
    longdouble variance = 0.0, slope = 0;
    for (integer i = 1; i <= ax.size; i ++) {
        const double ti = ax [i] - xmean;
        variance += ti * ti;
        slope += ti * ay [i];
    }
    slope /= variance;
    return slope;
}

double structSlopeSelector :: getIntercept (double slope) {
    const double xsum  = NUMsum (ax);
    const double ysum  = NUMsum (ay);
    return (ysum - xsum * slope) / numberOfPoints;
}

void structSlopeSelector :: getSlopeAndIntercept (double &slope, double& intercept) {
    slope = getSlope ();
    intercept = getIntercept (slope);
}

autoSlopeSelector SlopeSelector_create (constVEC const& x, constVEC const& y) {
    try {
        autoSlopeSelector me = Thing_new (SlopeSelector);
        my init (x, y, x.size);
        return me;
    } catch (MelderError) {
        Melder_throw (U"SlopeSelector could not be created.");
    }
}

void SlopeSelector_getSlopeAndIntercept (SlopeSelector me, double& slope, double& intercept) {
    my getSlopeAndIntercept (slope, intercept);
}

Thing_implement (SlopeSelectorSiegel, SlopeSelector, 0);

double structSlopeSelectorSiegel :: getSlope () {
    integer numberOfMedians = 0;
    for (integer i = 1; i <= numberOfPoints; i ++) {
        integer iline = 0;
        for (integer j = 1; j <= numberOfPoints; j ++) {
            if (i != j)
                crossings [++ iline] = (ay [i] - ay [j]) / (ax [i] - ax [j]);
        }
        Melder_assert (iline == numberOfPoints - 1);
        medians [++ numberOfMedians] = num::NUMquantile (crossings.get(), 0.5);
    }
    Melder_assert (numberOfMedians == numberOfPoints);
    return num::NUMquantile (medians.get(), 0.5);
}

double structSlopeSelectorSiegel :: getIntercept (double slope) {
    for (integer i = 1; i <= numberOfPoints; i ++)
        crossings [i] = ay [i] - slope * ax [i];
    return num::NUMquantile (crossings.get(), 0.5);
}

autoSlopeSelectorSiegel SlopeSelectorSiegel_create (constVEC const& x, constVEC const& y) {
    try {
        autoSlopeSelectorSiegel me = Thing_new (SlopeSelectorSiegel);
        my init (x, y, x.size - 1);
        my medians = zero_VEC (my numberOfPoints);
        return me;
    } catch (MelderError) {
        Melder_throw (U"SlopeSelectorSiegel could not be created.");
    }
}

/***************** TheilSen ********************/

Thing_implement (SlopeSelectorTheilSen, SlopeSelector, 0);

void structSlopeSelectorTheilSen :: getKth (integer k, double& kth, double& kp1th) {
    try {
        bool firstTime = true;
        numberOfTries = 0;
        const integer krt = Melder_iroundDown (1.5 * sqrt (sampleSize));
        integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
        integer numberOfCrossingsAtBeginX = 0, kappa = k, ilow, ihigh;

        auto getSortedSlopes = [&] (integer numberOfSlopes) {
            Melder_assert (numberOfSlopes <= sampleSize);
            crossings.resize (numberOfSlopes);
            for (integer i = 1; i <= numberOfSlopes; i ++) {
                getInversionFromCode (currentCrossingIndices [i], ilow, ihigh);
                const integer ipoint = numberOfDualLines + 1 - ilow;
                const integer jpoint = numberOfDualLines + 1 - ihigh;
                crossings [i] = (ay [jpoint] - ay [ipoint]) / (ax [jpoint] - ax [ipoint]); // the slope
            }
            sort_VEC_inout (crossings.get());
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
                    const integer numberOfIntervalCrossings = inversionCounter -> getSelectedInversionsNotInOther (
                        lineRankingAtEndX.get(), inverseOfLineRankingAtBeginX.get(), sortedRandomCrossingIndices.get(), currentCrossingIndices.get()
                    );
                    Melder_assert (numberOfIntervalCrossings > 0);
                }

                getSortedSlopes (sampleSize);

                const double kappar = ((double) sampleSize) / currentNumberOfIntervalCrossings * (k - numberOfCrossingsAtBeginX);
                kappa = std::max (1_integer, Melder_iroundDown (kappar));
                const integer kb = std::max (1_integer, kappa - krt);
                const integer ke = std::min (sampleSize, kappa + krt);
                const double beginX = crossings [kb];
                const double endX = crossings [ke];
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
                    const integer numberOfCrossingsAtX = inversionCounter -> getNumberOfInversions (result);
                    return numberOfCrossingsAtX;
                };

                numberOfCrossingsAtBeginX = getLineCrossingsAtX (beginX, lineRankingAtBeginX.get());
                if (numberOfCrossingsAtBeginX < k) {
                    const integer numberOfCrossingsAtEndX = getLineCrossingsAtX (endX, lineRankingAtEndX.get());
                    if (k < numberOfCrossingsAtEndX + 1) { // we also need k+1;
                        currentNumberOfIntervalCrossings = numberOfCrossingsAtEndX - numberOfCrossingsAtBeginX;
                        Permutation_invert_into (lineRankingAtBeginX.get(), inverseOfLineRankingAtBeginX.get());
                        lineRankingAtEndXPrevious -> p.get()  <<=  lineRankingAtEndX -> p.get();
                        firstTime = false;
                    } else
                        lineRankingAtEndX -> p.get()  <<=  lineRankingAtEndXPrevious -> p.get();
                } // else is covered by inverseOfLineRankingAtBeginX
                ++ numberOfTries;
            } else { // we are done: currentNumberOfIntervalCrossings <= sampleSize
                sortedRandomCrossingIndices.resize (currentNumberOfIntervalCrossings);
                currentCrossingIndices.resize (currentNumberOfIntervalCrossings);
                for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
                    sortedRandomCrossingIndices [i] = i;
                (void) inversionCounter -> getSelectedInversionsNotInOther (lineRankingAtEndX.get(), 
                    inverseOfLineRankingAtBeginX.get(), sortedRandomCrossingIndices.get(), currentCrossingIndices.get()
                );
                Melder_assert (currentNumberOfIntervalCrossings == inversionCounter -> numberOfInversionsRegistered);
                crossings.resize (currentNumberOfIntervalCrossings);
                getSortedSlopes (currentNumberOfIntervalCrossings);
                kappa = k - numberOfCrossingsAtBeginX;
                kth = crossings [kappa];
                kp1th = crossings [kappa + 1];
                return;
            }
        }
    } catch (MelderError) {
        Melder_throw (U"kth slope could not be selected.");
    }
}

double structSlopeSelectorTheilSen :: slopeQuantile_theilSen (double factor) {
    double kleft, kright;
    const integer numberOfLines = ax.size * (ax.size - 1) / 2;
    const double place = factor * numberOfLines + 0.5;
    const integer left = Melder_clipped (1_integer, Melder_ifloor (place), numberOfLines);
    Melder_assert (numberOfLines - 1 >= 1);
    getKth (left, kleft, kright);
    const double dif = kright - kleft;
    if (dif == 0.0)
        return kleft;
    return kleft + (place - left) * dif;
}

double structSlopeSelectorTheilSen :: getSlope () {
    return slopeQuantile_theilSen (0.5);
}

double structSlopeSelectorTheilSen :: getIntercept (double slope) {
    for (integer i = 1; i <= numberOfPoints; i ++)
        crossings [i] = ay [i] - slope * ax [i];
    return num::NUMquantile (crossings.get(), 0.5);
}

// only for ax.size < 50
double structSlopeSelectorTheilSen :: slopeQuantile_orderNSquared (double factor) {
    Melder_assert (ax.size <= split);
    return slopeQuantile_orderNSquaredWithBuffer (factor, slopes.get());
}

// buffer.size == ax.size * (ax.size - 1) / 2
double structSlopeSelectorTheilSen :: slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer) {
    const integer numberOfLines = ax.size * (ax.size - 1) / 2;
    Melder_assert (numberOfLines == buffer.size);
    integer index = 0;
    for (integer i = 1; i < ax.size; i ++)
        for (integer j = i + 1; j <= ax.size; j ++)
            buffer [++ index] = (ay [j] - ay [i]) / (ax [j] - ax [i]);
    Melder_assert (index == numberOfLines);
    const double medianSlope = num::NUMquantile (buffer, factor);
    return medianSlope;
}

void SlopeSelectorTheilSen_init (SlopeSelectorTheilSen me, constVEC const& x, constVEC const& y) {
	my numberOfDualLines = x.size;
    my split = 50;
    my maxNumberOfIntervalCrossings = x.size * (x.size - 1) / 2;
    if (x.size <= my split) {
        my numberOfLinesIfSplit = my maxNumberOfIntervalCrossings;
        my slopes = raw_VEC (my numberOfLinesIfSplit);
    }
    my sampleSize = my numberOfDualLines;
    my init (x, y, my sampleSize);

    my sortedRandomCrossingIndices = raw_INTVEC (my sampleSize);
    my currentCrossingIndices = raw_INTVEC (my sampleSize);
    my lineCrossings = raw_VEC (my numberOfDualLines); // used for intervals and line crossing
    my crossings = raw_VEC (my sampleSize); // used for intervals and line crossing
    my lineRankingAtBeginX = Permutation_create (my numberOfDualLines, true); // 1..n
    my lineRankingAtEndX = Permutation_reverse (my lineRankingAtBeginX.get(), 0, 0);  // n..1
    my lineRankingAtBeginXPrevious = Permutation_create (my numberOfDualLines, true); // 1..n
    my lineRankingAtEndXPrevious = Permutation_reverse (my lineRankingAtBeginX.get(), 0, 0);  // n..1
    my inverseOfLineRankingAtBeginX = Permutation_create (my numberOfDualLines, true); // 1..n
    my inversionCounter = PermutationInversionCounter_create (my numberOfDualLines);
}

autoSlopeSelectorTheilSen SlopeSelectorTheilSen_create (constVEC const& x, constVEC const& y) {
	Melder_assert (x.size == y.size);
	Melder_assert (x.size > 1);
	autoSlopeSelectorTheilSen me = Thing_new (SlopeSelectorTheilSen);
    SlopeSelectorTheilSen_init (me.get(), x, y);
    return me;
}

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

void SlopeSelectorTheilSen_getMedianSlope (SlopeSelectorTheilSen me, double& slope, double& intercept) {
    slope = my slopeQuantile (0.5);
    intercept = my getIntercept (slope);
}

void SlopeSelectorTheilSen_getKth (SlopeSelectorTheilSen me, integer k, double& kth, double& kp1th) {
    my getKth (k, kth, kp1th);
}

void SlopeSelectorTheilSen_getSlopeQuantile (SlopeSelectorTheilSen me, double quantile, double &slope, double &intercept);

void timeSlopeSelection () {
    try {
        Melder_clearInfo ();
        autoINTVEC sizes {10_integer, 50_integer, 100_integer, 500_integer, 1000_integer, 5000_integer, 10000_integer};
        MelderInfo_write (U"Old: n² slopes, sort, NUMquantile(0.5)\n"
            "New: Matoušek (1991) O(n log(n))\n"
        );
        MelderInfo_writeLine (U"n ntries tTS tSiegel tOld tOld/tTS tOld/tSiegel");
        for (integer isize = 1; isize <= sizes.size; isize ++) {
            double slope = 1.0, b = 4.0, stddev = 0.1, factor = 0.5;
            const integer n = sizes [isize];
            const integer maxNumberOfLines = n * (n - 1) / 2;
            autoVEC x = from_to_count_VEC (0.0, 10.0, n);
            autoVEC y = randomGauss_VEC (n, b, stddev);
            for (integer i = 1; i <= n; i ++)
                y[i] += slope * x[i];
           autoSlopeSelectorTheilSen sl =  SlopeSelectorTheilSen_create (x.get(), y.get());
            autoVEC buffer = raw_VEC (maxNumberOfLines);
            Melder_stopwatch ();
            const double slope1 = sl -> slopeQuantile_orderNSquaredWithBuffer (factor, buffer.get());
            const double t1 = Melder_stopwatch ();
            const double slope2 = sl -> slopeQuantile_theilSen (factor);
            const double t2 = Melder_stopwatch ();
            Melder_assert (slope1 == slope2);
            autoSlopeSelectorSiegel siegel = SlopeSelectorSiegel_create (x.get(), y.get());
            Melder_stopwatch ();
            siegel -> getSlope ();
            const double t3 = Melder_stopwatch ();
            const integer ntries = sl -> numberOfTries;
            MelderInfo_writeLine (n, U" ", ntries, U" ", t1, U" ", t2, U" ", t3, U" *", t1 / t2, U"*", U" ", t1 / t3);
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

/* End of file SlopeSelector.cpp */
