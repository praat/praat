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

#define ooSTRUCT SlopeSelectorTheilSen
oo_DEFINE_CLASS (SlopeSelectorTheilSen, Daata)
	oo_INTEGER (numberOfDualLines)
    oo_INTEGER (maxNumberOfIntervalCrossings)
	oo_INTEGER (sampleSize)
	oo_INTEGER (split)
	oo_OBJECT (Permutation, 0, lineRankingAtBeginX)
	oo_OBJECT (Permutation, 0, lineRankingAtEndX)
	oo_OBJECT (Permutation, 0, lineRankingAtBeginXPrevious)
	oo_OBJECT (Permutation, 0, lineRankingAtEndXPrevious)
	oo_OBJECT (Permutation, 0, inverseOfLineRankingAtBeginX)
	oo_INTVEC (sortedRandomCrossingIndices, sampleSize)
	oo_INTVEC (currentCrossingIndices, sampleSize)
	oo_VEC (lineCrossings, numberOfDualLines)
	oo_VEC (intervalCrossings, sampleSize)
    oo_INTEGER (numberOfLinesIfSplit)
    oo_VEC (slopes, numberOfLinesIfSplit)
	oo_OBJECT (PermutationInversionCounter, 0, inversionCounter)

	#if oo_DECLARING
		constVEC ax;	// the data points
        constVEC ay;

        void newDataPoints (constVEC const& x, constVEC const& y);

        void setSplit (integer split);  // from O(n²) to O(n log(n))

        void getKth (integer k, double& kth, double& kp1th);

        double slopeQuantile_theilSen (double factor);

        double slopeQuantile_orderNSquared (double factor);

        // if numberOfDualLines > split
        double slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer);

        double slopeQuantile (double factor) {
            return ( numberOfDualLines < split ? slopeQuantile_orderNSquared (factor) : slopeQuantile_theilSen (factor) );
        }

        double getIntercept (double slope);

        void slopeByLeastSquares (double &slope, double& intercept);

    #endif
	#if oo_COPYING
		thy ax = ax;
		thy ay = ay;
	#endif
oo_END_CLASS (SlopeSelectorTheilSen)
#undef ooSTRUCT

/* End of file SlopeSelector_def */
