/* Artword.cpp
 *
 * Copyright (C) 1992-2011,2015,2016,2017 Paul Boersma
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
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		Artword_setDefault (me.get(), i);
	return me;
}

void Artword_setDefault (Artword me, int feature) {
	ArtwordData f = & my data [feature];
	NUMvector_free <double> (f -> times, 1);
	NUMvector_free <double> (f -> targets, 1);
	f -> times = NUMvector <double> (1, 2);
	f -> targets = NUMvector <double> (1, 2);
	f -> numberOfTargets = 2;
	f -> times [1] = 0.0;
	f -> targets [1] = 0.0;
	f -> times [2] = my totalTime;
	f -> targets [2] = 0.0;
	f -> _iTarget = 1;
}

void Artword_setTarget (Artword me, int feature, double time, double target) {
	try {
		Melder_assert (feature >= 1);
		Melder_assert (feature <= kArt_muscle_MAX);
		ArtwordData f = & my data [feature];
		Melder_assert (f -> numberOfTargets >= 2);
		int32 insertionPosition = 1;   // should be able to go up to 32768
		if (time < 0.0) time = 0.0;
		if (time > my totalTime) time = my totalTime;
		while (insertionPosition <= f -> numberOfTargets && f -> times [insertionPosition] < time)
			insertionPosition ++;
		Melder_assert (insertionPosition <= f -> numberOfTargets);   // can never insert past totalTime
		if (f -> times [insertionPosition] != time) {
			if (f -> numberOfTargets == INT16_MAX)
				Melder_throw (U"An Artword cannot have more than ", INT16_MAX, U" targets.");
			long numberOfTargets = f -> numberOfTargets;
			NUMvector_insert <double> (& f -> times, 1, & numberOfTargets, insertionPosition);
			numberOfTargets = f -> numberOfTargets;
			NUMvector_insert <double> (& f -> targets, 1, & numberOfTargets, insertionPosition);
			f -> numberOfTargets ++;
		}
		f -> targets [insertionPosition] = target;
		f -> times [insertionPosition] = time;
	} catch (MelderError) {
		Melder_throw (me, U": target not set.");
	}
}

double Artword_getTarget (Artword me, int feature, double time) {
	ArtwordData f = & my data [feature];
	double *times = f -> times, *targets = f -> targets;
	int16 targetNumber = f -> _iTarget;
	if (! targetNumber) targetNumber = 1;
	while (time > times [targetNumber + 1] && targetNumber < f -> numberOfTargets - 1)
		targetNumber ++;
	while (time < times [targetNumber] && targetNumber > 1)
		targetNumber --;
	f -> _iTarget = targetNumber;
	Melder_assert (targetNumber > 0 && targetNumber < f -> numberOfTargets);
	return targets [targetNumber] + (time - times [targetNumber]) *
		(targets [targetNumber + 1] - targets [targetNumber]) /
		(times [targetNumber + 1] - times [targetNumber]);
}

void Artword_removeTarget (Artword me, int feature, int16 targetNumber) {
	ArtwordData f = & my data [feature];
	Melder_assert (targetNumber >= 1);
	Melder_assert (targetNumber <= f -> numberOfTargets);
	if (targetNumber == 1) {
		f -> targets [targetNumber] = 0.0;
	} else if (targetNumber == f -> numberOfTargets) {
		f -> targets [f -> numberOfTargets] = 0.0;
	} else {
		for (int16 i = targetNumber; i < f -> numberOfTargets; i ++) {
			f -> times [i] = f -> times [i + 1];
			f -> targets [i] = f -> targets [i + 1];
		}
		f -> numberOfTargets --;
	}
	f -> _iTarget = 1;
}

void Artword_intoArt (Artword me, Art art, double time) {
	for (int feature = 1; feature <= kArt_muscle_MAX; feature ++) {
		art -> art [feature] = Artword_getTarget (me, feature, time);
	}
}

void Artword_draw (Artword me, Graphics g, int feature, bool garnish) {
	int16 numberOfTargets = my data [feature]. numberOfTargets;
	if (numberOfTargets > 0) {
		autoNUMvector <double> x (1, numberOfTargets);
		autoNUMvector <double> y (1, numberOfTargets);
		Graphics_setInner (g);
		Graphics_setWindow (g, 0, my totalTime, -1.0, 1.0);
		for (int16 i = 1; i <= numberOfTargets; i ++) {
			x [i] = my data [feature]. times [i];
			y [i] = my data [feature]. targets [i];
		}
		Graphics_polyline (g, numberOfTargets, & x [1], & y [1]);         
		Graphics_unsetInner (g);
	}

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 3, true, true, true);
		Graphics_textTop (g, false, kArt_muscle_getText (feature));
		Graphics_textBottom (g, true, U"Time (s)");
	}
}

/* End of file Artword.cpp */
