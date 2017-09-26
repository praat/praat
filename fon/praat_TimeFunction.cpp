/* praat_TimeFunction.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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
	NUMBER_ONE (Function)
		double result = my xmin;
	NUMBER_ONE_END (U" seconds")
}

DIRECT (REAL_TimeFunction_getEndTime) {
	NUMBER_ONE (Function)
		double result = my xmax;
	NUMBER_ONE_END (U" seconds")
}

DIRECT (REAL_TimeFunction_getTotalDuration) {
	NUMBER_ONE (Function)
		double result = my xmax - my xmin;
	NUMBER_ONE_END (U" seconds")
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
	RADIO (shift, U"Shift", 1)
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
	praat_addAction1 (klas, 1, U"Get start time", nullptr, 2, REAL_TimeFunction_getStartTime);
	praat_addAction1 (klas, 1,   U"Get starting time", U"*Get start time", praat_DEPTH_2 | praat_DEPRECATED_2006, REAL_TimeFunction_getStartTime);
	praat_addAction1 (klas, 1, U"Get end time", nullptr, 2, REAL_TimeFunction_getEndTime);
	praat_addAction1 (klas, 1,   U"Get finishing time", U"*Get end time", praat_DEPTH_2 | praat_DEPRECATED_2006, REAL_TimeFunction_getEndTime);
	praat_addAction1 (klas, 1, U"Get total duration", nullptr, 2, REAL_TimeFunction_getTotalDuration);
	praat_addAction1 (klas, 1,   U"Get duration", U"*Get total duration", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_TimeFunction_getTotalDuration);
}

void praat_TimeFunction_modify_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Modify times", nullptr, 1, nullptr);
	praat_addAction1 (klas, 0, U"Shift times by...", nullptr, 2, MODIFY_TimeFunction_shiftTimesBy);
	praat_addAction1 (klas, 0, U"Shift times to...", nullptr, 2, MODIFY_TimeFunction_shiftTimesTo);
	praat_addAction1 (klas, 0,   U"Shift to zero", U"*Shift times to...", praat_DEPTH_2 | praat_DEPRECATED_2008, MODIFY_TimeFunction_shiftToZero);
	praat_addAction1 (klas, 0, U"Scale times by...", nullptr, 2, MODIFY_TimeFunction_scaleTimesBy);
	praat_addAction1 (klas, 0, U"Scale times to...", nullptr, 2, MODIFY_TimeFunction_scaleTimesTo);
	praat_addAction1 (klas, 0,   U"Scale times...", U"*Scale times to...", praat_DEPTH_2 | praat_DEPRECATED_2008, MODIFY_TimeFunction_scaleTimesTo);
}

/* End of file praat_TimeFunction.cpp */
