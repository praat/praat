/* praat_TimeFrameSampled.cpp
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

#include "praat_TimeFrameSampled.h"

#undef iam
#define iam iam_LOOP

// MARK: - TIMEFRAMESAMPLED

DIRECT (INTEGER_TimeFrameSampled_getNumberOfFrames) {
	LOOP {
		iam (Sampled);
		long numberOfFrames = my nx;
		Melder_information (numberOfFrames, U" frames");
	}
END2 }

FORM3 (REAL_TimeFrameSampled_getFrameFromTime, U"Get frame number from time", U"Get frame number from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (Sampled);
		double frame = Sampled_xToIndex (me, GET_REAL (U"Time"));
		Melder_informationReal (frame, nullptr);
	}
END2 }

DIRECT (REAL_TimeFrameSampled_getFrameLength) {
	LOOP {
		iam (Sampled);
		double frameLength = my dx;
		Melder_informationReal (frameLength, U"seconds");
	}
END2 }

FORM3 (REAL_TimeFrameSampled_getTimeFromFrame, U"Get time from frame number", U"Get time from frame number...") {
	NATURAL (U"Frame number", U"1")
	OK2
DO
	LOOP {
		iam (Sampled);
		double time = Sampled_indexToX (me, GET_INTEGER (U"Frame number"));
		Melder_informationReal (time, U"seconds");
	}
END2 }

// MARK: - buttons

void praat_TimeFrameSampled_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, U"Query time sampling", nullptr, 1, nullptr);
	praat_addAction1 (klas, 1, U"Get number of frames", nullptr, 2, INTEGER_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, U"Get time step", nullptr, 2, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1,   U"Get frame length", U"*Get time step", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1,   U"Get frame duration", U"*Get time step", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, U"Get time from frame number...", nullptr, 2, REAL_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1,   U"Get time from frame...", U"*Get time from frame number...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, U"Get frame number from time...", nullptr, 2, REAL_TimeFrameSampled_getFrameFromTime);
	praat_addAction1 (klas, 1,   U"Get frame from time...", U"*Get frame number from time...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameFromTime);
}

/* End of file praat_TimeFrameSampled.cpp */
