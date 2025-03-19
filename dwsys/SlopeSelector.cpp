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

#include <limits>
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

#include "enums_getText.h"
#include "SlopeSelector_enums.h"
#include "enums_getValue.h"
#include "SlopeSelector_enums.h"

static void randomInteger_INTVEC_inout (INTVEC iv, integer ilow, integer ihigh) {
    for (integer i = 1; i <= iv.size; i ++)
        iv [i] = NUMrandomInteger (ilow, ihigh);
}

Thing_implement (SlopeSelector, Daata, 1);

void structSlopeSelector :: newDataPoints (constVEC const& x, constVEC const& y) {
    Melder_assert (x.size == y.size);
    Melder_assert (x.size == numberOfPoints);
    our xp = x;
    our yp = y;
}

void structSlopeSelector :: getSlopeAndIntercept_leastSquares (double &slope, double& intercept) {
    const double xsum = NUMsum (xp);
    const double xmean = xsum / numberOfPoints;
    longdouble variance = 0.0;
	slope = 0;
    for (integer i = 1; i <= numberOfPoints; i ++) {
        const double ti = xp [i] - xmean;
        variance += ti * ti;
        slope += ti * yp [i];
    }
    slope /= variance;
	const double ysum  = NUMsum (yp);
	intercept = (ysum - xsum * slope) / numberOfPoints;
}

double structSlopeSelector :: getIntercept (double slope) {
    for (integer i = 1; i <= numberOfPoints; i ++)
        xcrossings [i] = yp [i] - slope * xp [i];
    return num::NUMquantile (xcrossings.get(), 0.5);
}

double structSlopeSelector :: getSlope_Siegel () {
	slopes.resize (slopes._capacity);
	VEC siegelSlopes = slopes.part (1, numberOfPoints - 1);
	VEC medians = slopes.part (numberOfPoints, numberOfPoints + numberOfPoints - 1);
    integer numberOfMedians = 0;
    for (integer i = 1; i <= numberOfPoints; i ++) {
        integer iline = 0;
        for (integer j = 1; j <= numberOfPoints; j ++) {
            if (i != j)
                siegelSlopes [++ iline] = (yp [i] - yp [j]) / (xp [i] - xp [j]);
        }
        Melder_assert (iline == numberOfPoints - 1);
        medians [++ numberOfMedians] = num::NUMquantile (siegelSlopes, 0.5);
    }
    Melder_assert (numberOfMedians == numberOfPoints);
    return num::NUMquantile (medians, 0.5);
}

static inline bool areEqual (double x, double y) {
	return ( x == 0.0 ? fabs (y) < 1e-14 : fabs ((x - y) / x) < 1e-14 );
}

void structSlopeSelector :: getKth_TheilSen (integer k, double& kth, double& kp1th) {
    try {
        numberOfTries = 0;
		const integer maxNumberOfIntervalCrossings = numberOfPoints * (numberOfPoints - 1) / 2;
        const integer krt = Melder_iroundDown (/* 1.5 */ sqrt (sampleSize)); // smaller than in the paper
        integer currentNumberOfIntervalCrossings = maxNumberOfIntervalCrossings;
        integer numberOfCrossingsAtLowX = 0, numberOfCrossingsAtLowXPrevious = 0;
        integer numberOfCrossingsAtHighX = maxNumberOfIntervalCrossings, numberOfCrossingsAtHighXPrevious = maxNumberOfIntervalCrossings;
        integer kappa = k, ilow, ihigh;
		double lowXPrevious, lowX = - std::numeric_limits<double>::infinity(), delta = 0.1;

        auto getSlopes = [&] (integer numberOfSlopes) {
            Melder_assert (numberOfSlopes <= maximumContractionSize);
            slopes.resize (numberOfSlopes);
            for (integer j = 1, i = 1; i <= numberOfSlopes; i ++, j += 2) {
                const integer ipoint = numberOfPoints + 1 - currentInversions [j];		// ilow
                const integer jpoint = numberOfPoints + 1 - currentInversions [j + 1];	// ihigh;
                slopes [i] = (yp [jpoint] - yp [ipoint]) / (xp [jpoint] - xp [ipoint]);	// the slope
            }
        };

        auto getPermutationAtX = [&](double x, mutablePermutation p) {
            for (integer iline = 1; iline <= numberOfPoints; iline ++) {
                const integer ipoint = numberOfPoints + 1 - iline;
                xcrossings [iline] = x * xp [ipoint] - yp [ipoint]; // the dual line's y-value
                p -> p [iline] = iline;
            }
            std::sort (p -> p.begin(), p -> p.end(),
                [&] (integer& i1, integer& i2) {
                    return xcrossings [i1] < xcrossings [i2];
				});
            return inversionCounter -> getNumberOfInversions (p);
        };

		/*
			In Matoušek's paper the following loop has to be run until the currentNumberOfIntervalCrossings <= samplingSize.
			We changed this to a number 10 times larger to reduce the number of iterations of this loop because
			the selection of a random sample of the inversions in the interval [xlow,xHigh] with the function
			'getSelectedInversionsNotInOther' is by far the most computational expensive part in the loop. 
			Reducing the number of iterations of this loop therefore saves more time than calculating somewhat more slopes.
		*/
        while (currentNumberOfIntervalCrossings > std::min (maximumContractionSize, maxNumberOfIntervalCrossings)) { // sampleSize*10

            /*
                Pick a random sample of size sampleSize from the intersections in the interval (lowX, highX)
                and store these in the vector 'currentInversions'.
                The first time we have the maximum number of slopes in our interval (-inf, +inf) and
                therefore the current inversions can be chosen directly.
                In the other cases we have n' < n(n-1)/2 slopes and pick random numbers from the interval [1,n']
                from which we have to search the corresponding slopes in O(n log(n)) time.
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
			num::NUMselect_inplace (slopes.get(), kb);
			lowXPrevious = lowX;
			lowX = slopes [kb];
			num::NUMselect_inplace (slopes.get(), ke);
			double highX = slopes [ke];
			trace (U"lowX:", lowX, U" highX:", highX);
            /*
                We have one of the following five situations for k, when lpx & hpx are the previous interval borders,
                lowX & highX the current interval borders and nlow and nhigh the number of inversions at the
                current borders.
                   |                               |
				  lpx                             hpx
							|             |
						   lowX (?==?)  highX
						   nlow         nhigh
                       k    k      k      k    k
                      (1)  (2)    (3)    (4)  (5)
				(1) k < nlow, we make lowX the new upper border
				(2) k == nlow, we know that kth == lowX
				(3) nlow < k < nhigh , next
				(4) k == nhigh
				(5) k > nhigh, make highX the new lowX
			*/
			if (numberOfTries > 0) { // for the first try the history is already there
				numberOfCrossingsAtLowXPrevious = numberOfCrossingsAtLowX;
				lineRankingAtLowXPrevious -> p.get()  <<=  lineRankingAtLowX -> p.get();
			}
            numberOfCrossingsAtLowX = getPermutationAtX (lowX, lineRankingAtLowX.get());
			trace (U"k:", k, U" kb:", kb, U" ke:", ke, U" ss:", sampleSize, U" ", numberOfCrossingsAtLowX, U" tries:", numberOfTries);
			if (k < numberOfCrossingsAtLowX) { // (1) interval lp, lc
                Permutations_swap (lineRankingAtHighX.get(), lineRankingAtLowX.get());			// first set lineRankingAtHighX
                numberOfCrossingsAtHighX = numberOfCrossingsAtLowX;
				trace (U"k<l ", lowX, U" ", highX, U" " );
				if (areEqual (lowX, highX)) { // we need a new lower border!
					if (k > numberOfCrossingsAtLowXPrevious) { // we are done
						kth = kp1th = lowX;
						return;
					}
					lowX -= ( fabs (lowX) > 0 ? fabs (lowX) * delta : delta );
					numberOfCrossingsAtLowX = getPermutationAtX (lowX, lineRankingAtLowX.get());
					Permutation_invert_into (lineRankingAtLowX.get(), inverseOfLineRankingAtLowX.get());
				} else {
					Permutations_swap (lineRankingAtLowX.get(), lineRankingAtLowXPrevious.get());	// then lineRankingAtLowX
					numberOfCrossingsAtHighX = numberOfCrossingsAtLowX;
					numberOfCrossingsAtLowX = numberOfCrossingsAtLowXPrevious;
					// inverseOfLineRankingAtLowX is still valid!!
				}
				trace (U"k<l ", lowX, U" ", highX, U" ", numberOfCrossingsAtLowX);
			} else if (k == numberOfCrossingsAtLowX) {
				kth = lowX;
				const integer kbp1 = kb + 1;
				num::NUMselect_inplace (slopes.get(), kbp1);
				highX = slopes [kbp1];
				trace (U"highX1:", highX);
				if (areEqual (lowX, highX)) {
					kp1th = lowX;
					return;
				}
				numberOfCrossingsAtHighX = getPermutationAtX (highX, lineRankingAtHighX.get());
				trace (U"k=l ", lowX, U" ", highX, U" ", numberOfCrossingsAtHighX);
			} else { // (3,4,5}) k > numberOfCrossingsAtLowX)
				if (numberOfTries > 0) {
					numberOfCrossingsAtHighXPrevious = numberOfCrossingsAtHighX;
					lineRankingAtHighXPrevious -> p.get()  <<=  lineRankingAtHighX -> p.get();
				}
				if (areEqual(lowX, highX)) { // we need a new high border!
					highX += ( fabs (highX) > 0 ? fabs (highX) * delta : delta );
				}
				numberOfCrossingsAtHighX = getPermutationAtX (highX, lineRankingAtHighX.get());
				if (k < numberOfCrossingsAtHighX) { // (3)
					trace (U"k<h ");
					// ok
				} else if (k == numberOfCrossingsAtHighX) { // (4)
					kth = highX;
					const integer kep1 = ke + 1;
					if (kep1 <= sampleSize) {
						num::NUMselect_inplace (slopes.get(), kep1);
						highX = slopes [kep1];
						if (areEqual (kth, highX)) {
							kp1th = kth;
							return;
						}
						trace (U"highX2:", highX);
					} else {
						Permutations_swap (lineRankingAtLowX.get(), lineRankingAtHighX.get());
						numberOfCrossingsAtLowX = numberOfCrossingsAtHighX;
						highX += ( fabs (highX) > 0 ? fabs (highX) * delta : delta );
					}
					trace (U"highX3:", highX);
					numberOfCrossingsAtHighX = getPermutationAtX (highX, lineRankingAtHighX.get());
					trace (U"k=h ", numberOfCrossingsAtHighX);
				} else { // (5) k > numberOfCrossingsAtHighX
					Permutations_swap (lineRankingAtLowX.get(), lineRankingAtHighX.get());
					numberOfCrossingsAtLowX = numberOfCrossingsAtHighX;
					Permutations_swap (lineRankingAtHighX.get(), lineRankingAtHighXPrevious.get());
					numberOfCrossingsAtHighX = numberOfCrossingsAtHighXPrevious;
					trace (U"k>h");
				}
				Permutation_invert_into (lineRankingAtLowX.get(), inverseOfLineRankingAtLowX.get());
			}
            currentNumberOfIntervalCrossings = numberOfCrossingsAtHighX - numberOfCrossingsAtLowX;
            ++ numberOfTries;
        }
        /*
            Finish with the remaining inversions. (We don't need the xcrossings anymore.)
         */
        sortedRandomCrossingCodes.resize (currentNumberOfIntervalCrossings);
        currentInversions.resize (2*currentNumberOfIntervalCrossings);
        for (integer i = 1; i <= currentNumberOfIntervalCrossings; i ++)
            sortedRandomCrossingCodes [i] = i;
        (void) inversionCounter -> getSelectedInversionsNotInOther (lineRankingAtHighX.get(),
            inverseOfLineRankingAtLowX.get(), sortedRandomCrossingCodes.get(), currentInversions.get()
        );
        Melder_assert (currentNumberOfIntervalCrossings == inversionCounter -> numberOfInversionsRegistered);
        getSlopes (currentNumberOfIntervalCrossings);
        kappa = k - numberOfCrossingsAtLowX;
		if (kappa > 0) { // or should we avoid this situation
			num::NUMselect_inplace (slopes.get(), kappa);
			kth = slopes [kappa];
		} else {
			kp1th = NUMmin_e (slopes.part (kappa + 1, currentNumberOfIntervalCrossings));
		}
		trace (kappa, U" ", numberOfCrossingsAtLowX, U" ", currentNumberOfIntervalCrossings);
		kp1th = NUMmin_e (slopes.part (kappa + 1, currentNumberOfIntervalCrossings));
    } catch (MelderError) {
        Melder_throw (U"kth slope could not be selected.");
    }
}

double structSlopeSelector :: slopeQuantile_TheilSen (double factor) {
    double kleft, kright;
    const integer numberOfLines = xp.size * (xp.size - 1) / 2;
    const double place = factor * numberOfLines + 0.5;
    const integer left = Melder_clipped (1_integer, Melder_ifloor (place), numberOfLines);
    Melder_assert (numberOfLines - 1 >= 1);
    getKth_TheilSen (left, kleft, kright);
    const double dif = kright - kleft;
    if (dif == 0.0)
        return kleft;
    return kleft + (place - left) * dif;
}

double structSlopeSelector :: getSlope_TheilSen () {
    return slopeQuantile_TheilSen (0.5);
}

// buffer.size == xp.size * (xp.size - 1) / 2
double structSlopeSelector :: slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer) {
    const integer numberOfLines = xp.size * (xp.size - 1) / 2;
    Melder_assert (numberOfLines == buffer.size);
    integer index = 0;
    for (integer i = 1; i < xp.size; i ++)
        for (integer j = i + 1; j <= xp.size; j ++)
            buffer [++ index] = (yp [j] - yp [i]) / (xp [j] - xp [i]);
    Melder_assert (index == numberOfLines);
    const double medianSlope = num::NUMquantile (buffer, factor);
    return medianSlope;
}

void SlopeSelector_init (SlopeSelector me, integer numberOfPoints) {
	my numberOfPoints = numberOfPoints;
    my sampleSize = my numberOfPoints;
    my maximumContractionSize = 5 * my numberOfPoints;

    my sortedRandomCrossingCodes = raw_INTVEC (my maximumContractionSize);
	my inversionsSize = 2 * my maximumContractionSize;
    my currentInversions = raw_INTVEC (my inversionsSize);
    my slopes = raw_VEC (my maximumContractionSize); // used for intervals and line crossing
	my xcrossings = raw_VEC (my numberOfPoints);
    my lineRankingAtLowX = Permutation_create (my numberOfPoints, true); // 1..n
    my lineRankingAtHighX = Permutation_reverse (my lineRankingAtLowX.get(), 0, 0);  // n..1
    my lineRankingAtLowXPrevious = Permutation_create (my numberOfPoints, true); // 1..n
    my lineRankingAtHighXPrevious = Permutation_reverse (my lineRankingAtLowX.get(), 0, 0);  // n..1
    my inverseOfLineRankingAtLowX = Permutation_create (my numberOfPoints, true); // 1..n
    my inversionCounter = PermutationInversionCounter_create (my numberOfPoints);
}

autoSlopeSelector SlopeSelector_create (integer numberOfPoints) {
	try {
		autoSlopeSelector me = Thing_new (SlopeSelector);
		SlopeSelector_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SlopeSelector.");
	}
}

autoSlopeSelector SlopeSelector_create (constVEC const& x, constVEC const& y) {
	try {
		Melder_assert (x.size == y.size);
		Melder_assert (x.size > 1);
		autoSlopeSelector me = Thing_new (SlopeSelector);
		SlopeSelector_init (me.get(), x.size);
		my newDataPoints (x, y);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create SlopeSelector from data points.");
	}
}

void SlopeSelectorTheilSen_getMedianSlope (SlopeSelector me, double& slope, double& intercept) {
    slope = my slopeQuantile (0.5);
    intercept = my getIntercept (slope);
}

void SlopeSelector_getSlopeAndIntercept (SlopeSelector me, double &slope, double &intercept, kSlopeSelector_method method) {
	if (method == kSlopeSelector_method :: THEILSEN) {
		slope = my getSlope_TheilSen ();
		intercept = my getIntercept (slope);
	} else if (method == kSlopeSelector_method :: SIEGEL) {
		slope = my getSlope_Siegel ();
		intercept = my getIntercept (slope);
	} else if (method == kSlopeSelector_method :: LEAST_SQUARES) {
		my getSlopeAndIntercept_leastSquares (slope, intercept);
	}
}

/*
    22/2/2025
    Old: n² slopes, sort, NUMquantile(0.5)
	New: Matoušek (1991) O(n log(n)) ?
	n  ntries   tTS      tSiegel      tOld    tOld/tTS  tOld/tSiegel SlopeOld/Siegel
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
			autoSlopeSelector sls =  SlopeSelector_create (x.get(), y.get());
            autoVEC buffer = raw_VEC (maxNumberOfLines);
            Melder_stopwatch ();
            const double slope1 = sls -> slopeQuantile_orderNSquaredWithBuffer (factor, buffer.get());
            const double t1 = Melder_stopwatch ();
            const double slope2 = sls -> slopeQuantile_TheilSen (factor);
            const double t2 = Melder_stopwatch ();
            const integer ntries = sls -> numberOfTries;
            Melder_assert (slope1 == slope2);
            Melder_stopwatch ();
            const double slope3 = sls -> getSlope_Siegel ();
            const double t3 = Melder_stopwatch ();
            MelderInfo_writeLine (n, U" ", ntries, U" ", t1, U" ", t2, U" ", t3, U" *", t1 / t2, U"*", U" ", t1 / t3, U" ", slope1/slope3);
			/* corner cases with many equal slopes */
			for (integer i = 1; i <= 5; i ++) {
				const double slope = NUMrandomUniform (0.1, 10.0);
				for (integer i = 1; i <= n; i ++)
					y[i] = slope * x[i];
				for (integer j = 1; j <= n / 10; j ++) {
					const integer index = NUMrandomInteger (1, n);
					y [index] += NUMrandomUniform  (-20.0,20.0);
				}
				sls -> newDataPoints (x.get(), y.get());
				const double slope4 = sls -> slopeQuantile_TheilSen (factor);
				Melder_assert (fabs ((slope4 - slope) / slope) < 1e-12);
			}
			
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

/* End of file SlopeSelector.cpp */
