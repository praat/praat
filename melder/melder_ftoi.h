#ifndef _melder_ftoi_h_
#define _melder_ftoi_h_
/* melder_ftoi.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

#pragma mark - REAL TO INTEGER CONVERSION

inline double Melder_roundDown (double x) {
	return floor (x);
}

inline integer Melder_iroundDown (double x) {
	double xround = Melder_roundDown (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,   // this formulation handles NaN correctly
		U"When rounding down the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_ifloor  Melder_iroundDown

inline double Melder_roundUp (double x) {
	return ceil (x);
}

inline integer Melder_iroundUp (double x) {
	double xround = Melder_roundUp (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding up the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_iceiling  Melder_iroundUp

inline double Melder_roundTowardsZero (double x) {
	return x >= 0.0 ? Melder_roundDown (x) : Melder_roundUp (x);
}

inline integer Melder_iroundTowardsZero (double x) {
	Melder_require (x >= (double) INTEGER_MIN && x <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U" towards zero, the result cannot be represented in an integer.");
	return (integer) x;
}

inline double Melder_roundAwayFromZero (double x) {
	return x >= 0.0 ? Melder_roundUp (x) : Melder_roundDown (x);
}

inline integer Melder_iroundAwayFromZero (double x) {
	double xround = Melder_roundAwayFromZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U" away from zero, the result cannot be represented in an integer.");
	return (integer) xround;
}

inline double Melder_round_tieUp (double x) {
	return Melder_roundDown (x + 0.5);
}

inline integer Melder_iround_tieUp (double x) {
	double xround = Melder_round_tieUp (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}
#define Melder_iround  Melder_iround_tieUp

inline double Melder_round_tieDown (double x) {
	return Melder_roundUp (x - 0.5);
}

inline integer Melder_iround_tieDown (double x) {
	double xround = Melder_round_tieDown (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

inline double Melder_round_tieTowardsZero (double x) {
	return x >= 0.0 ? Melder_round_tieDown (x) : Melder_round_tieUp (x);
}

inline integer Melder_iround_tieTowardsZero (double x) {
	double xround = Melder_round_tieTowardsZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

inline double Melder_round_tieAwayFromZero (double x) {
	return x >= 0.0 ? Melder_round_tieUp (x) : Melder_round_tieDown (x);
}

inline integer Melder_iround_tieAwayFromZero (double x) {
	double xround = Melder_round_tieAwayFromZero (x);
	Melder_require (xround >= (double) INTEGER_MIN && xround <= (double) INTEGER_MAX,
		U"When rounding the real value ", x, U", the result cannot be represented in an integer.");
	return (integer) xround;
}

/* End of file melder_ftoi.h */
#endif
