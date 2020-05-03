#ifndef _melder_real_h_
#define _melder_real_h_
/* melder_real.h
 *
 * Copyright (C) 1992-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	The following is checked in praat.h.
*/
using longdouble = long double;   // typically 80 bits ("extended") precision, but stored in 96 or 128 bits; on some platforms only 64 bits

template <typename T>
constexpr T sqr (T x) {
	return x * x;
}

struct MelderPoint {
	double x, y;
};

struct MelderRealRange {
	double min, max;
	//bool isEmpty () { return ! (max > min); }   // note edge case: will return true if min or max is NaN
	//double size () {
	//	double result = max - min;
	//	return std::max (result, 0.0);
	//}
};

struct MelderExtremaWithInit {
	double min = std::numeric_limits<double>::max();
	double max = std::numeric_limits<double>::lowest();
	void update (double val) {
		if (val < min)
			min = val;
		if (val > max)
			max = val;
	}
	bool isValid () const {
		return min <= max;
	}
};

struct MelderGaussianStats {
	double mean, stdev;
};

/* End of file melder_real.h */
#endif
