/* NUMmedian.cpp
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
#include "median_of_ninthers.h"
#include "Permutation.h"


structExtendedReal num::NUMmin_e (vector<structExtendedReal> v) {
	if (v.size == 0)
		Melder_throw (U"min_e: cannot determine the minimum of an empty ExtendedReal vector.");
	if (v.size == 1)
		return v [1];
	ExtendedReal t = & v [1];
	for (integer i = 2; i <=v.size; i ++ )
		if (lessThan (v [i], *t))
			t = & v [i];
	return *t;
}

structExtendedReal num::NUMmax_e (vector<structExtendedReal> v) {
	if (v.size == 0)
		Melder_throw (U"max_e: cannot determine the maximum of an empty ExtendedReal vector.");
	if (v.size == 1)
		return v [1];
	ExtendedReal t = & v [1];
	for (integer i = 2; i <=v.size; i ++ )
		if (greaterThan (v [i], *t))
			t = & v [i];
	return *t;
}

structExtendedReal num::NUMquantile (vector<structExtendedReal> const& v, double factor) {
	if (v.size < 1)
		return {undefined, 0};
	if (v.size == 1)
		return v [1];
	const double place = factor * v.size + 0.5;
	const integer left = Melder_clipped (1_integer, Melder_ifloor (place), v.size - 1);
	trace (U"left:", left, U" size:", v.size);
	num::NUMselect_inplace (v, left);
	vector<structExtendedReal> highPart = v.part (left + 1, v.size);
	const structExtendedReal min = num::NUMmin_e (highPart);
	const double slope = min.real - v [left].real;
	if (slope == 0.0)
		return v [left];   // or a [left + 1], which is the same
	return { v [left].real + (place - left) * slope, v [left].extension }; // ???
}

void timeMedian (void) {
    try {
        Melder_clearInfo ();
        autoINTVEC numberOfElements {11_integer, 101_integer, 1001_integer, 10001_integer, 100001_integer};
		MelderInfo_writeLine (U"Old: O(n log(n)); sort, NUMquantile(0.5)\n"
			"New: Alexandrescu (2017) O(n)\n"
		);
        MelderInfo_writeLine (U"n tNew tOld tOld/tNew info");
        for (integer index = 1; index <= numberOfElements.size; index ++) {
            /*
                median of increasing values
             */
            const integer n = numberOfElements [index];
            autoVEC p = to_VEC (n);
            autoVEC pc = copy_VEC (p.get());
            Melder_stopwatch ();
            sort_VEC_inout (pc.get());
            double  median1 = NUMquantile (pc.get(), 0.5);
            double t1 = Melder_stopwatch ();
            double median2 = num::NUMquantile (p.get(), 0.5);
            double t2 = Melder_stopwatch ();
            Melder_assert (median1 == median2);
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" *", t1 / t2, U"* increasing 1..n");
            /*
                median of decreasing values
            */
            for (integer i = 1; i <= n; i ++) {
                p [i] = n + 1 - i;
            }
            pc = copy_VEC (p.get());
            Melder_stopwatch ();
            sort_VEC_inout (pc.get());
            median1 = NUMquantile (pc.get(), 0.5);
            t1 = Melder_stopwatch ();
            median2 = num::NUMquantile (p.get(), 0.5);
            t2 = Melder_stopwatch ();
            Melder_assert (median1 == median2);
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" *", t1 / t2, U"* decreasing n..1");
            /*
                median of increasing values followed by decreasing values
            */
            for (integer i = 1; i <= n/2; i ++) {
                p [i] = i;
                p [n/2 + i] = n/2 + 1 - i;
            }
            pc = copy_VEC (p.get());
            Melder_stopwatch ();
            sort_VEC_inout (pc.get());
            median1 = NUMquantile (pc.get(), 0.5);
            t1 = Melder_stopwatch ();
            median2 = num::NUMquantile (p.get(), 0.5);
            t2 = Melder_stopwatch ();
            Melder_assert (median1 == median2);
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" *", t1 / t2, U"* inc-dec");
            /*
                median of uniform random values
             */
            for (integer i = 1; i <= n; i ++) {
                p [i] = NUMrandomUniform (-100.0, 100.0);
            }
            pc = copy_VEC (p.get());
            Melder_stopwatch ();
            sort_VEC_inout (pc.get());
            median1 = NUMquantile (pc.get(), 0.5);
            t1 = Melder_stopwatch ();
            median2 = num::NUMquantile (p.get(), 0.5);
            t2 = Melder_stopwatch ();
            Melder_assert (median1 == median2);
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" *", t1 / t2, U"* uniform(-100,100)");
            /*
                median of Gaussian random values
             */
            for (integer i = 1; i <= n; i ++) {
                p [i] = NUMrandomGauss (0.0, 1.0);
            }
            pc = copy_VEC (p.get());
            Melder_stopwatch ();
            sort_VEC_inout (pc.get());
            median1 = NUMquantile (pc.get(), 0.5);
            t1 = Melder_stopwatch ();
            median2 = num::NUMquantile (p.get(), 0.5);
            t2 = Melder_stopwatch ();
            Melder_assert (median1 == median2);
            MelderInfo_writeLine (n, U" ", t2, U" ", t1, U" *", t1 / t2, U"* gauss(0,1)");
        }
        MelderInfo_close ();
    } catch (MelderError) {
        Melder_throw (U"Could not perform timing of medians.");
    }
}

 /*  End of file NUMmedian.cpp */
