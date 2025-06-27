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

/*
	We would have liked to inherit from the ExtendedReal struct and only overload the operator<
	but that is not possible in a _def file??
*/
#define ooSTRUCT ExtendedCrossing
oo_DEFINE_STRUCT (ExtendedCrossing)
	oo_DOUBLE (real)
	oo_INTEGER (low)
	oo_INTEGER (high)
	#if oo_DECLARING

		friend bool operator<  (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			auto approximatelyEqual = [&] (double numericalEqualityPrecision) {
				if (std::fabs (lhs.real) < numericalEqualityPrecision || std::fabs (rhs.real) < numericalEqualityPrecision) {
					return std::fabs(lhs.real - rhs.real) < numericalEqualityPrecision;
				}
				// Use relative difference otherwise
				return std::fabs(lhs.real - rhs.real) <= numericalEqualityPrecision * std::fmax(std::fabs(lhs.real), std::fabs(rhs.real));
			};
			const bool r = ( !approximatelyEqual (1e-12) ? (lhs.real < rhs.real) :
				(lhs.high != rhs.high ? (lhs.high > rhs.high) : (lhs.low < rhs.low)) );
			return r;
		}
		/*
			All derived from above comparison
		*/
		friend inline bool operator>  (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			return rhs < lhs;
		}
		friend inline bool operator<= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			return ! (lhs > rhs);
		}
		friend inline bool operator>= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			return ! (lhs < rhs);
		}
		friend inline bool operator== (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			return std::tie (lhs.real, lhs.low, lhs.high) == std::tie (rhs.real, rhs.low, rhs.high);
		}
		friend inline bool operator!= (const structExtendedCrossing& lhs, const structExtendedCrossing& rhs) {
			return ! (lhs == rhs);
		}
		
	#endif
oo_END_STRUCT (ExtendedCrossing)
#undef ooSTRUCT

#define ooSTRUCT SlopeSelector
oo_DEFINE_CLASS (SlopeSelector, Daata)
	oo_INTEGER (numberOfPoints)
    oo_INTEGER (sampleSize)

    oo_INTEGER (numberOfTries)
	oo_INTEGER (maximumNumberOfTries)
    oo_INTEGER (maximumContractionSize) // > sampleSize + numberOfPoints
	oo_OBJECT (Permutation, 0, lineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtLowXPrevious)
	oo_OBJECT (Permutation, 0, inverseOfLineRankingAtLowX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighX)
	oo_OBJECT (Permutation, 0, lineRankingAtHighXPrevious)
	oo_INTVEC (sortedRandomCrossingCodes, sampleSize)
	oo_INTEGER (inversionsSize)
	oo_INTVEC (currentInversions, inversionsSize)
	oo_STRUCTVEC(ExtendedCrossing, xslopes, maximumContractionSize)
	oo_VEC (buffer, maximumContractionSize) // for buffering and final quantile calculations
	oo_STRUCTVEC(ExtendedCrossing, xcrossings, numberOfPoints)
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
		thy xp = xp; // superfluous: xp and yp need to be linked explicitely to the external data (by newDataPoints)
		thy yp = yp; //
	#endif
oo_END_CLASS (SlopeSelector)
#undef ooSTRUCT

/* End of file SlopeSelector_def */
