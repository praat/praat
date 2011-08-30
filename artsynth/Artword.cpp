/* Artword.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

Thing_implement (Artword, Data, 0);

Artword Artword_create (double totalTime) {
	Artword me = Thing_new (Artword);
	if (! me) return NULL;
	my totalTime = totalTime;
	for (int i = 1; i <= kArt_muscle_MAX; i ++)
		Artword_setDefault (me, i);
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
		int insert = 1;
		if (time < 0.0) time = 0.0;
		if (time > my totalTime) time = my totalTime;
		while (insert <= f -> numberOfTargets && f -> times [insert] < time)
			insert ++;
		Melder_assert (insert <= f -> numberOfTargets);   // can never insert past totalTime
		if (f -> times [insert] != time) {
			long numberOfTargets = f -> numberOfTargets;
			NUMvector_insert <double> (& f -> times, 1, & numberOfTargets, insert);
			numberOfTargets = f -> numberOfTargets;
			NUMvector_insert <double> (& f -> targets, 1, & numberOfTargets, insert);
			f -> numberOfTargets ++;
		}
		f -> targets [insert] = target;
		f -> times [insert] = time;
	} catch (MelderError) {
		Melder_throw (me, ": target not set.");
	}
}

double Artword_getTarget (Artword me, int feature, double time) {
	ArtwordData f = & my data [feature];
	double *times = f -> times, *targets = f -> targets;
	int iTarget = f -> _iTarget;
	if (! iTarget) iTarget = 1;
	while (time > times [iTarget + 1] && iTarget < f -> numberOfTargets - 1)
		iTarget ++;
	while (time < times [iTarget] && iTarget > 1)
		iTarget --;
	f -> _iTarget = iTarget;
	Melder_assert (iTarget > 0 && iTarget < f -> numberOfTargets);
	return targets [iTarget] + (time - times [iTarget]) *
		(targets [iTarget + 1] - targets [iTarget]) /
		(times [iTarget + 1] - times [iTarget]);
}

void Artword_removeTarget (Artword me, int feature, int iTarget) {
	ArtwordData f = & my data [feature];
	Melder_assert (iTarget >= 1);
	Melder_assert (iTarget <= f -> numberOfTargets);
	if (iTarget == 1)
		f -> targets [iTarget] = 0.0;
	else if (iTarget == f -> numberOfTargets)
		f -> targets [f -> numberOfTargets] = 0.0;
	else {
		int i;
		for (i = iTarget; i < f -> numberOfTargets; i ++) {
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

void Artword_draw (Artword me, Graphics g, int feature, int garnish) {
	long numberOfTargets = my data [feature]. numberOfTargets;
	if (numberOfTargets > 0) {
		autoNUMvector <double> x (1, numberOfTargets);
		autoNUMvector <double> y (1, numberOfTargets);
		Graphics_setInner (g);
		Graphics_setWindow (g, 0, my totalTime, -1, 1);
		for (int i = 1; i <= numberOfTargets; i ++) {
			x [i] = my data [feature]. times [i];
			y [i] = my data [feature]. targets [i];
		}
		Graphics_polyline (g, numberOfTargets, & x [1], & y [1]);         
		Graphics_unsetInner (g);
	}

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		Graphics_marksLeft (g, 3, TRUE, TRUE, TRUE);
		Graphics_textTop (g, FALSE, kArt_muscle_getText (feature));
		Graphics_textBottom (g, TRUE, L"Time (s)");
	}
}

/* End of file Artword.cpp */
