/* praat_TimeTier.cpp
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

#include "praat_TimeTier.h"

// MARK: TIMETIER

// MARK: Query

DIRECT (INTEGER_TimeTier_getNumberOfPoints) {
	NUMBER_ONE (AnyTier)
		integer result = my points.size;
	NUMBER_ONE_END (U" points")
}

FORM (INTEGER_TimeTier_getLowIndexFromTime, U"Get low index", U"AnyTier: Get low index from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	FIND_ONE (AnyTier)
		Melder_information (my points.size == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToLowIndex (me, time)));
	END
}

FORM (INTEGER_TimeTier_getHighIndexFromTime, U"Get high index", U"AnyTier: Get high index from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	FIND_ONE (AnyTier)
		Melder_information (my points.size == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToHighIndex (me, time)));
	END
}

FORM (INTEGER_TimeTier_getNearestIndexFromTime, U"Get nearest index", U"AnyTier: Get nearest index from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	FIND_ONE (AnyTier)
		Melder_information (my points.size == 0 ? U"--undefined--" : Melder_integer (AnyTier_timeToNearestIndex (me, time)));
	END
}

FORM (REAL_TimeTier_getTimeFromIndex, U"Get time", nullptr /*"AnyTier: Get time from index..."*/) {
	NATURAL (pointNumber, U"Point number", U"10")
	OK
DO
	FIND_ONE (AnyTier)
		if (pointNumber > my points.size) Melder_information (U"--undefined--");
		else Melder_informationReal (my points.at [pointNumber] -> number, U"seconds");
	END
}

// MARK: Modify

FORM (MODIFY_TimeTier_removePoint, U"Remove one point", U"AnyTier: Remove point...") {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	MODIFY_EACH (AnyTier)
		AnyTier_removePoint (me, pointNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TimeTier_removePointNear, U"Remove one point", U"AnyTier: Remove point near...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	MODIFY_EACH (AnyTier)
		AnyTier_removePointNear (me, time);
	MODIFY_EACH_END
}

FORM (MODIFY_TimeTier_removePointsBetween, U"Remove points", U"AnyTier: Remove points between...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"1.0")
	OK
DO
	MODIFY_EACH (AnyTier)
		AnyTier_removePointsBetween (me, fromTime, toTime);
	MODIFY_EACH_END
}

// MARK: - buttons

void praat_TimeTier_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, U"Get number of points", nullptr, 1, INTEGER_TimeTier_getNumberOfPoints);
	praat_addAction1 (klas, 1, U"Get low index from time...", nullptr, 1, INTEGER_TimeTier_getLowIndexFromTime);
	praat_addAction1 (klas, 1, U"Get high index from time...", nullptr, 1, INTEGER_TimeTier_getHighIndexFromTime);
	praat_addAction1 (klas, 1, U"Get nearest index from time...", nullptr, 1, INTEGER_TimeTier_getNearestIndexFromTime);
	praat_addAction1 (klas, 1, U"Get time from index...", nullptr, 1, REAL_TimeTier_getTimeFromIndex);
}

void praat_TimeTier_modify_init (ClassInfo klas) {
	praat_TimeFunction_modify_init (klas);
	praat_addAction1 (klas, 0, U"Remove point...", nullptr, 1, MODIFY_TimeTier_removePoint);
	praat_addAction1 (klas, 0, U"Remove point near...", nullptr, 1, MODIFY_TimeTier_removePointNear);
	praat_addAction1 (klas, 0, U"Remove points between...", nullptr, 1, MODIFY_TimeTier_removePointsBetween);
}

/* End of file praat_TimeTier.cpp */
