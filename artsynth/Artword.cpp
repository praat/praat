/* Artword.cpp
 *
 * Copyright (C) 1992-2009,2011,2015-2018,2020 Paul Boersma
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

#include "Artword.h"

#include "oo_DESTROY.h"
#include "Artword_def.h"
#include "oo_COPY.h"
#include "Artword_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Artword_def.h"
#include "oo_EQUAL.h"
#include "Artword_def.h"
#include "oo_WRITE_TEXT.h"
#include "Artword_def.h"
#include "oo_READ_TEXT.h"
#include "Artword_def.h"
#include "oo_WRITE_BINARY.h"
#include "Artword_def.h"
#include "oo_READ_BINARY.h"
#include "Artword_def.h"
#include "oo_DESCRIPTION.h"
#include "Artword_def.h"

Thing_implement (Artword, Daata, 0);

autoArtword Artword_create (double totalTime) {
	autoArtword me = Thing_new (Artword);
	my totalTime = totalTime;
	for (kArt_muscle muscle = (kArt_muscle) 1; muscle <= kArt_muscle::MAX; ++ muscle)
		Artword_setDefault (me.get(), muscle);
	return me;
}

void Artword_setDefault (Artword me, kArt_muscle muscle) {
	ArtwordData f = & my data [(int) muscle];
	f -> times = zero_VEC (2);
	f -> targets = zero_VEC (2);
	f -> numberOfTargets = 2;
	f -> times [1] = 0.0;
	f -> targets [1] = 0.0;
	f -> times [2] = my totalTime;
	f -> targets [2] = 0.0;
	f -> _iTarget = 1;
}

static void ArtwordData_setTarget (ArtwordData me, double time, double target) {
	Melder_assert (my numberOfTargets >= 2);
	int32 insertionPosition = 1;   // should be able to go up to 32768
	while (insertionPosition <= my numberOfTargets && my times [insertionPosition] < time)
		insertionPosition ++;
	Melder_assert (insertionPosition <= my numberOfTargets);   // can never insert past totalTime
	if (my times [insertionPosition] == time) {
		my targets [insertionPosition] = target;
		return;
	}
	if (my numberOfTargets == INT16_MAX)
		Melder_throw (U"An Artword cannot have more than ", INT16_MAX, U" targets.");
	my times. insert (insertionPosition, time);
	my targets. insert (insertionPosition, target);
	my numberOfTargets ++;   // maintain invariant
}

void Artword_setTarget (Artword me, kArt_muscle muscle, double time, double target) {
	try {
		Melder_assert ((int) muscle >= 1);
		Melder_assert ((int) muscle <= (int) kArt_muscle::MAX);
		Melder_assert (muscle <= kArt_muscle::MAX);
		if (time < 0.0) time = 0.0;
		if (time > my totalTime) time = my totalTime;
		ArtwordData_setTarget (& my data [(int) muscle], time, target);
	} catch (MelderError) {
		Melder_throw (me, U": target not set.");
	}
}

static double ArtwordData_getTarget (ArtwordData me, double time) {
	int16 targetNumber = my _iTarget;
	if (! targetNumber) targetNumber = 1;
	while (time > my times [targetNumber + 1] && targetNumber < my numberOfTargets - 1)
		targetNumber ++;
	while (time < my times [targetNumber] && targetNumber > 1)
		targetNumber --;
	my _iTarget = targetNumber;
	Melder_assert (targetNumber > 0 && targetNumber < my numberOfTargets);
	return my targets [targetNumber] + (time - my times [targetNumber]) *
		(my targets [targetNumber + 1] - my targets [targetNumber]) /
		(my times [targetNumber + 1] - my times [targetNumber]);
}

double Artword_getTarget (Artword me, kArt_muscle muscle, double time) {
	return ArtwordData_getTarget (& my data [(int) muscle], time);
}

void Artword_removeTarget (Artword me, kArt_muscle muscle, int16 targetNumber) {
	ArtwordData f = & my data [(int) muscle];
	Melder_assert (targetNumber >= 1);
	Melder_assert (targetNumber <= f -> numberOfTargets);
	if (targetNumber == 1) {
		f -> targets [targetNumber] = 0.0;
	} else if (targetNumber == f -> numberOfTargets) {
		f -> targets [f -> numberOfTargets] = 0.0;
	} else {
		f -> times. remove (targetNumber);
		f -> targets. remove (targetNumber);
		f -> numberOfTargets --;   // maintain invariant
	}
	f -> _iTarget = 1;
}

void Artword_intoArt (Artword me, Art art, double time) {
	for (int muscle = 1; muscle <= (int) kArt_muscle::MAX; muscle ++)
		art -> art [muscle] = Artword_getTarget (me, (kArt_muscle) muscle, time);
}

void Artword_draw (Artword me, Graphics g, kArt_muscle muscle, bool garnish) {
	int16 numberOfTargets = my data [(int) muscle]. numberOfTargets;
	if (numberOfTargets > 0) {
		autoVEC x = raw_VEC (numberOfTargets), y = raw_VEC (numberOfTargets);
		Graphics_setInner (g);
		Graphics_setWindow (g, 0, my totalTime, -1.0, 1.0);
		for (int16 i = 1; i <= numberOfTargets; i ++) {
			x [i] = my data [(int) muscle]. times [i];
			y [i] = my data [(int) muscle]. targets [i];
		}
		Graphics_polyline (g, numberOfTargets, & x [1], & y [1]);
		Graphics_unsetInner (g);
	}

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 3, true, true, true);
		Graphics_textTop (g, false, kArt_muscle_getText (muscle));
		Graphics_textBottom (g, true, U"Time (s)");
	}
}

/* End of file Artword.cpp */
