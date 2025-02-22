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
        numberOfTries = 0;
        const integer krt = Melder_iroundDown (/* 1.5 */ sqrt (sampleSize)); // smaller than in the paper
        integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
        integer numberOfCrossingsAtLowX = 0, numberOfCrossingsAtLowXPrevious = 0;
        integer numberOfCrossingsAtHighX = maxNumberOfIntervalCrossings, numberOfCrossingsAtHighXPrevious = maxNumberOfIntervalCrossings;
        integer kappa = k, ilow, ihigh;

        auto getSlopes = [&] (integer numberOfSlopes) {
            Melder_assert (numberOfSlopes <= maximumContractionSize);
            crossings.resize (numberOfSlopes);
            for (integer j = 1, i = 1; i <= numberOfSlopes; i ++, j += 2) {
                const integer ipoint = numberOfDualLines + 1 - currentInversions [j];		// ilow
                const integer jpoint = numberOfDualLines + 1 - currentInversions [j + 1];	// ihigh;
                crossings [i] = (ay [jpoint] - ay [ipoint]) / (ax [jpoint] - ax [ipoint]);	// the slope
            }
        };

        auto getPermutationAtX = [&](double x, mutablePermutation p) {
            for (integer iline = 1; iline <= numberOfDualLines; iline ++) {
                const integer ipoint = numberOfDualLines + 1 - iline;
                crossings [iline] = x * ax [ipoint] - ay [ipoint]; // the dual line's y-value
                p -> p [iline] = iline;
            }
            std::sort (p -> p.begin(), p -> p.end(),
                [&] (integer& i1, integer& i2) {
                    return crossings [i1] < crossings [i2];
				});
            return inversionCounter -> getNumberOfInversions (p);
        };

		/*
			In Matousek's paper the following loop has to be run until the currentNumberOfIntervalCrossings <= samplingSize.
			We changed this to a number 10 times larger to reduce the number of iterations of this loop because
			the selection of a random sample of the inversions in the interval [xlow,xHigh] with 'getSelectedInversionsNotInOther'
			is by far the most expensive part in the loop. Reducing the number of iterations of this loop therefore saves more time
			than calculating somewhat more slopes.
		*/
        while (currentNumberOfIntervalCrossings > std::min (maximumContractionSize, maxNumberOfIntervalCrossings)) { // sampleSize*10

            /*
                Pick a random sample of size sampleSize from the intersections in the interval (lowX, highX).
                The first time we have the maximum number of crossings in our interval (-inf, +inf) and
                therefore the current inversions can be chosen directly.
                In the other cases we have n' < n(n-1)/2 crossings and pick random numbers from the interval [1,n']
                and we have to search the corresponding crossings in O(n log(n)) time.
            */
            if (numberOfTries == 0) {
 				for (integer i = 1; i <= sampleSize; i ++) {
					const integer irandom = NUMrandomInteger (1_integer, currentNumberOfIntervalCrossings);
					getInversionFromCode (irandom, ilow, ihigh);
					currentInversions [2 * i - 1] = ilow;
					currentInversions [2 * i    ] = ihigh;
				}
			} else {
                randomInteger_INTVEC_inout (sortedRandomCrossingCodes.get(), 1_integer, currentNumberOfIntervalCrossings);
                sort_INTVEC_inout (sortedRandomCrossingCodes.get());
                inversionCounter -> getSelectedInversionsNotInOther (
                    lineRankingAtHighX.get(), inverseOfLineRankingAtLowX.get(), sortedRandomCrossingCodes.get(), currentInversions.get()
                );
            }

            getSlopes (sampleSize);

            const double kappar = ((double) sampleSize) / currentNumberOfIntervalCrossings * (k - numberOfCrossingsAtLowX);
            kappa = std::max (1_integer, Melder_iroundDown (kappar));
            const integer kb = std::max (1_integer, kappa - krt);
            const integer ke = std::min (sampleSize, kappa + krt);
			num::NUMselect_inplace (crossings.get(), kb);
			const double lowX = crossings [kb];
			num::NUMselect_inplace (crossings.get(), ke);
			const double highX = crossings [ke];
            Melder_assert (lowX <= highX);
            /*
                We have one of the following three situations for k, where lp & hp are the previous interval borders
                and lc & hc the current interval borders
                   |                             |
                   lp       |           |        hp
                            lc          hc
                       k         k          k
                      (1)       (2)        (3)
            */
			if (numberOfTries > 0) {
				numberOfCrossingsAtLowXPrevious = numberOfCrossingsAtLowX;
				lineRankingAtLowXPrevious -> p.get()  <<=  lineRankingAtLowX -> p.get();
			}
            numberOfCrossingsAtLowX = getPermutationAtX (lowX, lineRankingAtLowX.get());
            if (k < numberOfCrossingsAtLowX) { // (1) interval lp, lc
                Permutations_swap (lineRankingAtHighX.get(), lineRankingAtLowX.get());			// first set lineRankingAtHighX
                Permutations_swap (lineRankingAtLowX.get(), lineRankingAtLowXPrevious.get());	// then lineRankingAtLowX
                numberOfCrossingsAtHighX = numberOfCrossingsAtLowX;
                numberOfCrossingsAtLowX = numberOfCrossingsAtLowXPrevious;
                // inverseOfLineRankingAtLowX is still valid!!
            } else { // (2+3) interval lc, hp
				if (numberOfTries > 0) {
					numberOfCrossingsAtHighXPrevious = numberOfCrossingsAtHighX;
					lineRankingAtHighXPrevious -> p.get()  <<=  lineRankingAtHighX -> p.get();
				}
                numberOfCrossingsAtHighX = getPermutationAtX (highX, lineRankingAtHighX.get());
                if (k < numberOfCrossingsAtHighX) { // (2) interval lc,hc
                    // ok, nothing to do
                } else { // (3) interval hc, hp
                    Permutations_swap (lineRankingAtLowX.get(), lineRankingAtHighX.get());			// first set lineRankingAtLowX
                    Permutations_swap (lineRankingAtHighX.get(), lineRankingAtHighXPrevious.get());	// then lineRankingAtHighX
                    numberOfCrossingsAtLowX = numberOfCrossingsAtHighX;
                    numberOfCrossingsAtHighX = numberOfCrossingsAtHighXPrevious;
                }
                Permutation_invert_into (lineRankingAtLowX.get(), inverseOfLineRankingAtLowX.get());
            }
            currentNumberOfIntervalCrossings = numberOfCrossingsAtHighX - numberOfCrossingsAtLowX;
            ++ numberOfTries;
        }
        /*
            Finish with the remaining inversions
         */
        sortedRandomCrossingCodes.resize (currentNumberOfIntervalCrossings);
        currentInversions.resize (2*currentNumberOfIntervalCrossings);
        for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
            sortedRandomCrossingCodes [i] = i;
        (void) inversionCounter -> getSelectedInversionsNotInOther (lineRankingAtHighX.get(),
            inverseOfLineRankingAtLowX.get(), sortedRandomCrossingCodes.get(), currentInversions.get()
        );
        Melder_assert (currentNumberOfIntervalCrossings == inversionCounter -> numberOfInversionsRegistered);
        crossings.resize (currentNumberOfIntervalCrossings);
        getSlopes (currentNumberOfIntervalCrossings);
        kappa = k - numberOfCrossingsAtLowX;
		num::NUMselect_inplace (crossings.get(), kappa);
		kth = crossings [kappa];
		kp1th = NUMmin_e (crossings.part (kappa + 1, currentNumberOfIntervalCrossings));
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
    my maximumContractionSize = 5 * my numberOfDualLines;
    my init (x, y, my maximumContractionSize);

    my sortedRandomCrossingCodes = raw_INTVEC (my maximumContractionSize);
	my inversionsSize = 2 * my maximumContractionSize;
    my currentInversions = raw_INTVEC (my inversionsSize);
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
    22/2/2025
    Old: n² slopes, sort, NUMquantile(0.5)
	New: Matoušek (1991) O(n log(n)) ?
	n ntries   tTS      tSiegel      tOld    tOld/tTS  tOld/tSiegel SlopeOld/Siegel
	10    0   8.3446e-06 1.3589e-05 9.2983e-06 *0.6140*   0.8974    1.0057
	50    2   6.9379e-05 0.0002     0.0001     *0.3115*   0.3493    1.0007
	100   4   0.0002     0.0009     0.0008     *0.2950*   0.3647    1.0021
	500   3   0.0056     0.0021     0.0091     *2.6722*   0.6178    1.0015
	1000  4   0.0075     0.0065     0.0205     *1.1525*   0.3706    1.0007
	5000  3   0.2045     0.0465     0.4115     *4.3929*   0.4970    1.0003
	10000 4   0.7233     0.2095     1.7015     *3.4526*   0.4251    0.9996
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
