#ifndef _praat_TimeVector_h_
#define _praat_TimeVector_h_
/* praat_TimeVector.h
 *
 * Copyright (C) 2016,2017 Paul Boersma
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
	Interface functions for time-based subclasses of Vector.
*/

#include "Vector.h"
#include "praat_TimeFunction.h"

/*
	Prompting for interpolated values at a time or within a time range.
*/

#define praat_TimeVector_INTERPOLATED_VALUE(time,interpolation) \
	REAL (time, U"Time (s)", U"0.5") \
	RADIOx (interpolation, U"Interpolation", 3, 0) \
		RADIOBUTTON (U"Nearest") \
		RADIOBUTTON (U"Linear") \
		RADIOBUTTON (U"Cubic") \
		RADIOBUTTON (U"Sinc70") \
		RADIOBUTTON (U"Sinc700")

#define praat_TimeVector_INTERPOLATED_EXTREMUM(fromTime,toTime,interpolation) \
	praat_TimeFunction_RANGE (fromTime, toTime) \
	RADIOx (interpolation, U"Interpolation", 2, 0) \
		RADIOBUTTON (U"None") \
		RADIOBUTTON (U"Parabolic") \
		RADIOBUTTON (U"Cubic") \
		RADIOBUTTON (U"Sinc70") \
		RADIOBUTTON (U"Sinc700")

/* End of file praat_TimeVector.h */
#endif
