#ifndef _praat_TimeFunction_h_
#define _praat_TimeFunction_h_
/* praat_TimeFunction.h
 *
 * Copyright (C) 2016 Paul Boersma
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
	Interface functions for time-based subclasses of Function.
*/

#include "Function.h"
#include "praat.h"

/*
	Prompting for fromTime and toTime on one row with two fields.
*/
#define praat_TimeFunction_RANGE(fromTime,toTime) \
	REAL (fromTime, U"left Time range (s)", U"0.0") \
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")

/*
	Action buttons.
*/
void praat_TimeFunction_query_init (ClassInfo klas);   // Query buttons for end points and duration
void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time shifting and time scaling

/* End of file praat_TimeFunction.h */
#endif
