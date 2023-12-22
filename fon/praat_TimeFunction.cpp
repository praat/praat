/* praat_TimeFunction.cpp
 *
 * Copyright (C) 1992-2017,2022,2023 Paul Boersma
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

#include "praat_TimeFunction.h"

// MARK: - TIMEFUNCTION

// MARK: Query

DIRECT (REAL_TimeFunction_getStartTime) {
	QUERY_ONE_FOR_REAL (Function)
		double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (REAL_TimeFunction_getEndTime) {
	QUERY_ONE_FOR_REAL (Function)
		double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (REAL_TimeFunction_getTotalDuration) {
	QUERY_ONE_FOR_REAL (Function)
		double result = my xmax - my xmin;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

// MARK: Modify

FORM (MODIFY_TimeFunction_shiftTimesBy, U"Shift times by", nullptr) {
	REAL (shift, U"Shift (s)", U"0.5")
	OK
DO
	MODIFY_EACH (Function)
		Function_shiftXBy (me, shift);
	MODIFY_EACH_END
}

FORM (MODIFY_TimeFunction_shiftTimesTo, U"Shift times to", nullptr) {
	CHOICE (shift, U"Shift", 1)
		OPTION (U"start time")
		OPTION (U"centre time")
		OPTION (U"end time")
	REAL (toTime, U"To time (s)", U"0.0")
	OK
DO
	MODIFY_EACH (Function)
		Function_shiftXTo (me, shift == 1 ? my xmin : shift == 2 ? 0.5 * (my xmin + my xmax) : my xmax, toTime);
	MODIFY_EACH_END
}

DIRECT (MODIFY_TimeFunction_shiftToZero) {
	MODIFY_EACH (Function)
		Function_shiftXTo (me, my xmin, 0.0);
	MODIFY_EACH_END
}

FORM (MODIFY_TimeFunction_scaleTimesBy, U"Scale times by", nullptr) {
	POSITIVE (factor, U"Factor", U"2.0")
	OK
DO
	MODIFY_EACH (Function)
		Function_scaleXBy (me, factor);
	MODIFY_EACH_END
}

FORM (MODIFY_TimeFunction_scaleTimesTo, U"Scale times to", nullptr) {
	REAL (newStartTime, U"New start time (s)", U"0.0")
	REAL (newEndTime, U"New end time (s)", U"1.0")
	OK
DO
	if (newStartTime >= newEndTime) Melder_throw (U"New end time should be greater than new start time.");
	MODIFY_EACH (Function)
		Function_scaleXTo (me, newStartTime, newEndTime);
	MODIFY_EACH_END
}

// MARK: - buttons

void praat_TimeFunction_query_init (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"Query time domain", nullptr, 1, nullptr);
	praat_addAction1 (klas, 1, U"Get start time || Get starting time", nullptr, 2, REAL_TimeFunction_getStartTime);
			// alternatives COMPATIBILITY <= 2006
	praat_addAction1 (klas, 1, U"Get end time || Get finishing time", nullptr, 2, REAL_TimeFunction_getEndTime);
			// alternatives COMPATIBILITY <= 2006
	praat_addAction1 (klas, 1, U"Get total duration || Get duration", nullptr, 2, REAL_TimeFunction_getTotalDuration);
			// alternatives COMPATIBILITY <= 2004
}

void praat_TimeFunction_modify_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Modify times", nullptr, 1, nullptr);
	praat_addAction1 (klas, 0, U"Shift times by...", nullptr, 2, MODIFY_TimeFunction_shiftTimesBy);
	praat_addAction1 (klas, 0, U"Shift times to...", nullptr, 2, MODIFY_TimeFunction_shiftTimesTo);
	praat_addAction1 (klas, 0,   U"Shift to zero", nullptr, GuiMenu_DEPTH_2 | GuiMenu_DEPRECATED_2008, MODIFY_TimeFunction_shiftToZero);
			// replace with "Shift times to..."
	praat_addAction1 (klas, 0, U"Scale times by...", nullptr, 2, MODIFY_TimeFunction_scaleTimesBy);
	praat_addAction1 (klas, 0, U"Scale times to... || Scale times...", nullptr, 2, MODIFY_TimeFunction_scaleTimesTo);
			// alternatives COMPATIBILITY <= 2008
}

/* End of file praat_TimeFunction.cpp */
