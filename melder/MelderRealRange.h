#pragma once
/* MelderRealRange.h
 *
 * Copyright (C) 2018 Paul Boersma
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

inline MelderRealRange NUMextrema (const constVEC& vec) {
	if (NUMisEmpty (vec)) return { undefined, undefined };
	double minimum = vec [1], maximum = minimum;
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value < minimum) minimum = value;
		if (value > maximum) maximum = value;
	}
	return { minimum, maximum };
}

/* End of file MelderRealRange.h */

