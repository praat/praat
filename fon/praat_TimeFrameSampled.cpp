/* praat_TimeFrameSampled.cpp
 *
 * Copyright (C) 2016,2017,2019,2021,2022 Paul Boersma
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

// MARK: - TIMEFRAMESAMPLED

DIRECT (INTEGER_TimeFrameSampled_getNumberOfFrames) {
	QUERY_ONE_FOR_REAL (Sampled)
		integer result = my nx;
	QUERY_ONE_FOR_REAL_END (U" frames");
}

FORM (REAL_TimeFrameSampled_getFrameFromTime, U"Get frame number from time", U"Get frame number from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	QUERY_ONE_FOR_REAL (Sampled)
		double result = Sampled_xToIndex (me, time);
	QUERY_ONE_FOR_REAL_END (U" (frame as a real number)")
}

DIRECT (REAL_TimeFrameSampled_getFrameLength) {
	QUERY_ONE_FOR_REAL (Sampled)
		double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_TimeFrameSampled_getTimeFromFrame, U"Get time from frame number", U"Get time from frame number...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Sampled)
		double result = Sampled_indexToX (me, frameNumber);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (NUMVEC_TimeFrameSampled_listAllFrameTimes) {
	QUERY_ONE_FOR_REAL_VECTOR (Sampled)
		autoVEC result = Sampled_listAllXValues (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

// MARK: - buttons

void praat_TimeFrameSampled_query_init (ClassInfo klas) {
	praat_TimeFunction_query_init (klas);
	praat_addAction1 (klas, 1, U"Query time sampling", nullptr, 1, nullptr);
	praat_addAction1 (klas, 1, U"Get number of frames", nullptr, 2, INTEGER_TimeFrameSampled_getNumberOfFrames);
	praat_addAction1 (klas, 1, U"Get time step", nullptr, 2, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1,   U"Get frame length", U"*Get time step", GuiMenu_DEPTH_2 | GuiMenu_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1,   U"Get frame duration", U"*Get time step", GuiMenu_DEPTH_2 | GuiMenu_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameLength);
	praat_addAction1 (klas, 1, U"Get time from frame number...", nullptr, 2, REAL_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1,   U"Get time from frame...", U"*Get time from frame number...", GuiMenu_DEPTH_2 | GuiMenu_DEPRECATED_2004, REAL_TimeFrameSampled_getTimeFromFrame);
	praat_addAction1 (klas, 1, U"List all frame times", nullptr, 2, NUMVEC_TimeFrameSampled_listAllFrameTimes);
	praat_addAction1 (klas, 1, U"Get frame number from time...", nullptr, 2, REAL_TimeFrameSampled_getFrameFromTime);
	praat_addAction1 (klas, 1,   U"Get frame from time...", U"*Get frame number from time...", GuiMenu_DEPTH_2 | GuiMenu_DEPRECATED_2004, REAL_TimeFrameSampled_getFrameFromTime);
}

/* End of file praat_TimeFrameSampled.cpp */
