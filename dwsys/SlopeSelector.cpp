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

#include "SlopeSelector.h"
#include "melder.h"

void timeSlopeSelection () {
    try {
        Melder_clearInfo ();
        autoINTVEC sizes {10_integer, 50_integer, 100_integer, 500_integer, 1000_integer, 5000_integer, 10000_integer};
        MelderInfo_writeLine (U"n tNew tOld tOld/tNew");
        for (integer isize = 1; isize <= sizes.size; isize ++) {
            double slope = 1.0, b = 4.0, stddev = 0.1, factor = 0.5;
            const integer n = sizes [isize];
            autoVEC x = from_to_count_VEC (0.0, 10.0, n);
            autoVEC y = randomGauss_VEC (n, 0.0, 0.1);
            for (integer i = 1; i <= n; i ++)
                y[i] = slope * x[i] + NUMrandomGauss (b, stddev);
            SlopeSelector sl (x.get(), y.get());

            Melder_stopwatch ();
            const double slope1 = sl.quantile_theilSen (factor);
            const double t1 = Melder_stopwatch ();
            const double slope2 = sl.quantile_orderNSquared (factor);
            Melder_assert (slope1 == slope2);
            const double t2 = Melder_stopwatch ();
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" ", t1 / t2);
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"");
    }
}

/* End of file SlopeSelector.cpp */
