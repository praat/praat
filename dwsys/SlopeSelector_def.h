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

    oo_INTEGER (numberOfTries)
	oo_INTEGER (maximumNumberOfTries)
    oo_INTEGER (maximumContractionSize) // > sampleSize + numberOfPoints
	oo_INTEGER (inversionsSize)
	oo_OBJECT (Permutation, 0, lineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtLowXPrevious)
	oo_OBJECT (Permutation, 0, inverseOfLineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighXPrevious)
	oo_INTVEC (sortedRandomCrossingCodes, sampleSize)
	oo_INTVEC (currentInversions, inversionsSize)
	oo_VEC (slopes, maximumContractionSize)
	oo_VEC (xcrossings, numberOfPoints) // could also be shared as slopes.part (slopes._capacity - numberOfPoints + 1, slopes._capacity)
	oo_OBJECT (PermutationInversionCounter, 0, inversionCounter)

	#if oo_DECLARING
		constVEC xp;	// links to the outside world data points (by newDataPoints(x,y))
        constVEC yp;	// 

		void newDataPoints (constVEC const& x, constVEC const& y);
				
		double getSlope_Siegel ();
		
		double getSlope_TheilSen ();
		
        double getIntercept (double slope);
		
        void getSlopeAndIntercept_leastSquares (double &slope, double& intercept);
				
        void getKth_TheilSen (integer k, double& kth, double& kp1th);

        double slopeQuantile_TheilSen (double factor);

        double slopeQuantile_orderNSquared (double factor);

        double slopeQuantile_orderNSquaredWithBuffer (double factor, VEC const& buffer);

        double slopeQuantile (double factor) {
            return slopeQuantile_TheilSen (factor);
        }
    #endif
	#if oo_COPYING
		thy xp = xp; // superfluous, xp and yp need to be linked to external data (by newDataPoints)
		thy yp = yp; //
	#endif
oo_END_CLASS (SlopeSelector)
#undef ooSTRUCT

/* End of file SlopeSelector_def */
