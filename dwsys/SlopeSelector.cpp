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

void structSlopeSelector :: init (constVEC const& x, constVEC const& y, integer crossingCapacity) {
    numberOfPoints  = x.size;
    our crossingCapacity = crossingCapacity;
    crossings = raw_VEC (crossingCapacity);
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

static void randomInteger_INTVEC_inout (INTVEC iv, integer ilow, integer ihigh) {
    for (integer i = 1; i <= iv.size; i ++)
        iv [i] = NUMrandomInteger (ilow, ihigh);
}

void structSlopeSelectorTheilSen :: getKth (integer k, double& kth, double& kp1th) {
    try {
        bool firstTime = true;
        numberOfTries = 0;
        const integer krt = Melder_iroundDown (1.5 * sqrt (sampleSize));
        integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
        integer numberOfCrossingsAtLowX = 0, numberOfCrossingsAtLowXPrevious = 0;
        integer numberOfCrossingsAtHighX = maxNumberOfIntervalCrossings, numberOfCrossingsAtHighXPrevious = maxNumberOfIntervalCrossings;
        integer kappa = k, ilow, ihigh;

        auto getSortedSlopes = [&] (integer numberOfSlopes) {
            Melder_assert (numberOfSlopes <= maximumContractionSize);
            crossings.resize (numberOfSlopes);
            for (integer i = 1; i <= numberOfSlopes; i ++) {
                getInversionFromCode (currentCrossingCodes [i], ilow, ihigh);
                const integer ipoint = numberOfDualLines + 1 - ilow;
                const integer jpoint = numberOfDualLines + 1 - ihigh;
                crossings [i] = (ay [jpoint] - ay [ipoint]) / (ax [jpoint] - ax [ipoint]); // the slope
            }
            sort_VEC_inout (crossings.get());
        };

        auto getPermutationAtX = [&](double x, mutablePermutation result) {
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

        while (currentNumberOfIntervalCrossings > std::min (maximumContractionSize, maxNumberOfIntervalCrossings)) { // sampleSize*10

            /*
                Pick a random sample of size sampleSize from the intersections in the interval (lowX, highX).
                The first time we have the maximum number of crossings in our interval (-inf, +inf) and
                therefore the current crossing indices can be chosen directly.
                In the other cases we have n' < n(n-1)/2 crossings and pick random numbers from the interval [1,n']
                and we have to search the corresponding crossings in O(n log(n)) time.
            */
            if (firstTime)
                randomInteger_INTVEC_inout (currentCrossingCodes.get(), 1_integer, currentNumberOfIntervalCrossings);
            else {
                randomInteger_INTVEC_inout (sortedRandomCrossingCodes.get(), 1_integer, currentNumberOfIntervalCrossings);
                sort_INTVEC_inout (sortedRandomCrossingCodes.get());
                const integer numberOfIntervalCrossings = inversionCounter -> getSelectedInversionsNotInOther (
                    lineRankingAtHighX.get(), inverseOfLineRankingAtLowX.get(), sortedRandomCrossingCodes.get(), currentCrossingCodes.get()
                );
                Melder_assert (numberOfIntervalCrossings > 0);
            }

            getSortedSlopes (sampleSize);

            const double kappar = ((double) sampleSize) / currentNumberOfIntervalCrossings * (k - numberOfCrossingsAtLowX);
            kappa = std::max (1_integer, Melder_iroundDown (kappar));
            const integer kb = std::max (1_integer, kappa - krt);
            const integer ke = std::min (sampleSize, kappa + krt);
            const double lowX = crossings [kb];
            const double highX = crossings [ke];
            Melder_assert (lowX <= highX);
            /*
                We one of the following three situations for k, where lp & hp are the previous interval
                and lc & hc the current interval borders
                   |        |           |        |
                   lp       lc          hc      hp
                       k         k          k
                      (1)       (2)        (3)
            */
            numberOfCrossingsAtLowX = getPermutationAtX (lowX, lineRankingAtLowX.get());
            if (k < numberOfCrossingsAtLowX) { // (1) interval lx, lx
                Permutations_swap (lineRankingAtHighX.get(), lineRankingAtLowX.get()); // order of the two swaps is important here!
                Permutations_swap (lineRankingAtLowX.get(), lineRankingAtLowXPrevious.get());
                // lineRankingAtHighX -> p.get()  <<=  lineRankingAtLowX -> p.get(); // order of coying is important here!
                // lineRankingAtLowX -> p.get()  <<=  lineRankingAtLowXPrevious -> p.get(); // TODO via move swap!!
                numberOfCrossingsAtHighX = numberOfCrossingsAtLowX;
                numberOfCrossingsAtLowX = numberOfCrossingsAtLowXPrevious;
                // inverseOfLineRankingAtLowX is still valid!!
            } else { // interval lc, hx
                numberOfCrossingsAtHighX = getPermutationAtX (highX, lineRankingAtHighX.get());
                if (k < numberOfCrossingsAtHighX) { // (2) interval lc,hc
                    //lineRankingAtHighXPrevious -> p.get()  <<=  lineRankingAtHighX -> p.get();
                   // lineRankingAtLowXPrevious -> p.get()  <<=  lineRankingAtLowX -> p.get();
                } else { // (3) interval hc, hx
                    Permutations_swap (lineRankingAtLowX.get(), lineRankingAtHighX.get());
                    Permutations_swap (lineRankingAtHighX.get(), lineRankingAtHighXPrevious.get());
                    numberOfCrossingsAtLowX = numberOfCrossingsAtHighX;
                    numberOfCrossingsAtHighX = numberOfCrossingsAtHighXPrevious;
                }
                Permutation_invert_into (lineRankingAtLowX.get(), inverseOfLineRankingAtLowX.get());
            }
            lineRankingAtHighXPrevious -> p.get()  <<=  lineRankingAtHighX -> p.get();
            lineRankingAtLowXPrevious -> p.get()  <<=  lineRankingAtLowX -> p.get();
            currentNumberOfIntervalCrossings = numberOfCrossingsAtHighX - numberOfCrossingsAtLowX;
            numberOfCrossingsAtLowXPrevious = numberOfCrossingsAtLowX;
            numberOfCrossingsAtHighXPrevious = numberOfCrossingsAtHighX;
            firstTime = false;
            ++ numberOfTries;
        }
        /* Last round */
        sortedRandomCrossingCodes.resize (currentNumberOfIntervalCrossings);
        currentCrossingCodes.resize (currentNumberOfIntervalCrossings);
        for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
            sortedRandomCrossingCodes [i] = i;
        (void) inversionCounter -> getSelectedInversionsNotInOther (lineRankingAtHighX.get(), 
            inverseOfLineRankingAtLowX.get(), sortedRandomCrossingCodes.get(), currentCrossingCodes.get()
        );
        Melder_assert (currentNumberOfIntervalCrossings == inversionCounter -> numberOfInversionsRegistered);
        crossings.resize (currentNumberOfIntervalCrossings);
        getSortedSlopes (currentNumberOfIntervalCrossings);
        kappa = k - numberOfCrossingsAtLowX;
        kth = crossings [kappa];
        kp1th = crossings [kappa + 1];
 
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
    my maxNumberOfIntervalCrossings = x.size * (x.size - 1) / 2;
    my sampleSize = my numberOfDualLines;
    my maximumContractionSize = 10 * my sampleSize;
    my init (x, y, my maximumContractionSize);

    my sortedRandomCrossingCodes = raw_INTVEC (my maximumContractionSize);
    my currentCrossingCodes = raw_INTVEC (my maximumContractionSize);
    my lineCrossings = raw_VEC (my numberOfDualLines);
    my crossings = raw_VEC (my maximumContractionSize); // used for intervals and line crossing
    my lineRankingAtLowX = Permutation_create (my numberOfDualLines, true); // 1..n
    my lineRankingAtHighX = Permutation_reverse (my lineRankingAtLowX.get(), 0, 0);  // n..1
    my lineRankingAtLowXPrevious = Permutation_create (my numberOfDualLines, true); // 1..n
    my lineRankingAtHighXPrevious = Permutation_reverse (my lineRankingAtLowX.get(), 0, 0);  // n..1
    my inverseOfLineRankingAtLowX = Permutation_create (my numberOfDualLines, true); // 1..n
    my inversionCounter = PermutationInversionCounter_create (my numberOfDualLines);
}

autoSlopeSelectorTheilSen SlopeSelectorTheilSen_create (constVEC const& x, constVEC const& y) {
	Melder_assert (x.size == y.size);
	Melder_assert (x.size > 1);
	autoSlopeSelectorTheilSen me = Thing_new (SlopeSelectorTheilSen);
    SlopeSelectorTheilSen_init (me.get(), x, y);
    return me;
}

void SlopeSelectorTheilSen_getMedianSlope (SlopeSelectorTheilSen me, double& slope, double& intercept) {
    slope = my slopeQuantile (0.5);
    intercept = my getIntercept (slope);
}

void SlopeSelectorTheilSen_getKth (SlopeSelectorTheilSen me, integer k, double& kth, double& kp1th) {
    my getKth (k, kth, kp1th);
}

void SlopeSelectorTheilSen_getSlopeQuantile (SlopeSelectorTheilSen me, double quantile, double &slope, double &intercept);

/*
    17/2/2025
    Old: n² slopes, sort, NUMquantile(0.5)
    New: Matoušek (1991) O(n log(n)) c=10, samplesize == numberOfDualLines
    n ntries tTS tSiegel tOld tOld/tTS tOld/tSiegel SlopeOld/Siegel
    10 0 5.0067e-06 1.1444e-05 3.8146e-06 *0.4375* 1.3125 0.9995
    50 1 4.6253e-05 9.5605e-05 6.7710e-05 *0.4837* 0.6830 1.0017
    100 3 0.0001 0.0005 0.0002 *0.2035* 0.5351 1.0020
    500 4 0.0027 0.0053 0.0052 *0.5269* 0.5352 0.9992
    1000 4 0.0093 0.0157 0.0197 *0.5942* 0.4739 0.9992
    5000 5 0.1952 0.3612 0.4345 *0.5403* 0.4493 1.0001
    10000 4 0.7977 1.0587 1.7057 *0.7534* 0.4676 1.0001
*/
void timeSlopeSelection () {
    try {
        Melder_clearInfo ();
        autoINTVEC sizes {10_integer, 50_integer, 100_integer, 500_integer, 1000_integer, 5000_integer, 10000_integer};
        MelderInfo_write (U"Old: n² slopes, sort, NUMquantile(0.5)\n"
            "New: Matoušek (1991) O(n log(n))\n"
        );
        MelderInfo_writeLine (U"n ntries tTS tSiegel tOld tOld/tTS tOld/tSiegel SlopeOld/Siegel");
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
            const double slope3 = siegel -> getSlope ();
            const double t3 = Melder_stopwatch ();
            const integer ntries = sl -> numberOfTries;
            MelderInfo_writeLine (n, U" ", ntries, U" ", t1, U" ", t2, U" ", t3, U" *", t1 / t2, U"*", U" ", t1 / t3, U" ", slope1/slope3);
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

/* End of file SlopeSelector.cpp */
