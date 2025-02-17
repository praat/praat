/* SlopeSelector_def.h
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

#define ooSTRUCT SlopeSelector
oo_DEFINE_CLASS (SlopeSelector, Daata)
	oo_INTEGER (numberOfPoints)
    oo_INTEGER (sampleSize)
    oo_INTEGER (crossingCapacity)
    oo_VEC (crossings, crossingCapacity)

	#if oo_DECLARING
		constVEC ax;	// the data points
        constVEC ay;

        void init (constVEC const& x, constVEC const& y, integer crossingCapacity);

        void newDataPoints (constVEC const& x, constVEC const& y);

        virtual double getSlope ();

        virtual double getIntercept (double slope);

        void getSlopeAndIntercept (double &slope, double& intercept);

    #endif
	#if oo_COPYING
		thy ax = ax;
		thy ay = ay;
	#endif
oo_END_CLASS (SlopeSelector)
#undef ooSTRUCT

#define ooSTRUCT SlopeSelectorSiegel
oo_DEFINE_CLASS (SlopeSelectorSiegel, SlopeSelector)

    oo_VEC (medians, numberOfPoints)

	#if oo_DECLARING

        double getSlope () override;

        double getIntercept (double slope) override;

    #endif

oo_END_CLASS (SlopeSelectorSiegel)
#undef ooSTRUCT

#define ooSTRUCT SlopeSelectorTheilSen
oo_DEFINE_CLASS (SlopeSelectorTheilSen, SlopeSelector)
	oo_INTEGER (numberOfDualLines)
    oo_INTEGER (maxNumberOfIntervalCrossings)
    oo_INTEGER (numberOfTries)
    oo_INTEGER (maximumContractionSize) // (factor>=1)*sampleSize
	oo_OBJECT (Permutation, 0, lineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtLowXPrevious)
	oo_OBJECT (Permutation, 0, inverseOfLineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighXPrevious)
	oo_INTVEC (sortedRandomCrossingCodes, sampleSize)
	oo_INTVEC (currentCrossingCodes, sampleSize)
	oo_VEC (lineCrossings, numberOfDualLines)
	oo_OBJECT (PermutationInversionCounter, 0, inversionCounter)

	#if oo_DECLARING

        void getKth (integer k, double& kth, double& kp1th);

        double slopeQuantile_theilSen (double factor);

        double slopeQuantile_orderNSquared (double factor);

        double getSlope () override;

        double getIntercept (double slope) override;

        double slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer);

        double slopeQuantile (double factor) {
            return slopeQuantile_theilSen (factor);
        }

        void slopeByLeastSquares (double &slope, double& intercept);

    #endif

oo_END_CLASS (SlopeSelectorTheilSen)
#undef ooSTRUCT

/* End of file SlopeSelector_def */
