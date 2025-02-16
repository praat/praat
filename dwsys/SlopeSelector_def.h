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
    oo_VEC (crossings, sampleSize)

	#if oo_DECLARING
		constVEC ax;	// the data points
        constVEC ay;

        void init (constVEC const& x, constVEC const& y, integer sampleSize);

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
	oo_INTEGER (split)
	oo_OBJECT (Permutation, 0, lineRankingAtBeginX)
	oo_OBJECT (Permutation, 0, lineRankingAtEndX)
	oo_OBJECT (Permutation, 0, lineRankingAtBeginXPrevious)
	oo_OBJECT (Permutation, 0, lineRankingAtEndXPrevious)
	oo_OBJECT (Permutation, 0, inverseOfLineRankingAtBeginX)
	oo_INTVEC (sortedRandomCrossingIndices, sampleSize)
	oo_INTVEC (currentCrossingIndices, sampleSize)
	oo_VEC (lineCrossings, numberOfDualLines)
    oo_INTEGER (numberOfLinesIfSplit)
    oo_VEC (slopes, numberOfLinesIfSplit)
	oo_OBJECT (PermutationInversionCounter, 0, inversionCounter)

	#if oo_DECLARING

        //void setSplit (integer split);  // from O(n²) to O(n log(n))

        void getKth (integer k, double& kth, double& kp1th);

        double slopeQuantile_theilSen (double factor);

        double slopeQuantile_orderNSquared (double factor);

        double getSlope () override;

        double getIntercept (double slope) override;

        // if numberOfDualLines > split
        double slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer);

        double slopeQuantile (double factor) {
            return ( numberOfDualLines < split ? slopeQuantile_orderNSquared (factor) : slopeQuantile_theilSen (factor) );
        }

        void slopeByLeastSquares (double &slope, double& intercept);

    #endif

oo_END_CLASS (SlopeSelectorTheilSen)
#undef ooSTRUCT

/* End of file SlopeSelector_def */
