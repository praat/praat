/* Label.cpp
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

/*
 * pb 2002/07/16 GPL
 * pb 2004/10/16 C++ compatible structs
 * pb 2007/08/13 wchar
 * pb 2011/06/02 C++
 * pb 2011/07/02 C++
 */

#include "Label.h"

static void copy (I, thou) {
	iam (Autosegment); thouart (Autosegment);
	inherited (Autosegment) copy (me, thee);
	if (my name) Thing_setName (thee, my name);
}

static bool equal (I, thou) {
	iam (Autosegment); thouart (Autosegment);
	if (! inherited (Autosegment) equal (me, thee)) return false;
	if (my name == NULL && thy name == NULL) return true;   // shortcut: no names
	if (my name == NULL || thy name == NULL) return false;
	return wcsequ (my name, thy name);
}

static struct structData_Description description [] = {
	{ L"Autosegment", inheritwa, 0, sizeof (struct structAutosegment), L"Autosegment", & theStructFunction. description },
	{ L"name", stringwa, (char *) & ((Autosegment) & Melder_debug) -> name - (char *) & Melder_debug, sizeof (wchar *) },
	{ 0 } };

class_methods (Autosegment, Function) {
	class_method (copy)
	class_method (equal)
	class_method (description)
	class_methods_end
}

Autosegment Autosegment_create (double tmin, double tmax, const wchar *label) {
	try {
		autoAutosegment me = Thing_new (Autosegment);
		Function_init (me.peek(), tmin, tmax);
		if (label != NULL) {
			Thing_setName (me.peek(), label); therror
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Autosegment not created.");
	}
}

static int compare (I, thou) {
	iam (Function); thouart (Function);
	if (my xmin < thy xmin) return -1;
	if (my xmin > thy xmin) return 1;
	if (my xmax < thy xmax) return -1;
	if (my xmax > thy xmax) return 1;
	return 0;
}

class_methods (Tier, Sorted) {
	class_method (compare)
	class_methods_end
}

void Tier_init (I, long initialCapacity) {
	iam (Tier);
	Sorted_init (me, classAutosegment, initialCapacity);
	Collection_addItem (me, Autosegment_create (-1e30, 1e30, NULL));
}

Tier Tier_create (long initialCapacity) {
	try {
		autoTier me = Thing_new (Tier);
		Tier_init (me.peek(), initialCapacity);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Tier not created.");
	}
}

long Tier_timeToIndex (Tier me, double t) {
	for (long i = 1; i <= my size; i ++) {
		Autosegment interval = (Autosegment) my item [i];
		if (t >= interval -> xmin && t < interval -> xmax)
			return i;
	}
	return 0;   /* Empty tier or very large t. */
}

class_methods (Label, Ordered) {
	class_methods_end
}

void Label_init (I, long initialNumberOfTiers) {
	iam (Label);
	Ordered_init (me, classTier, initialNumberOfTiers);
	for (long i = 1; i <= initialNumberOfTiers; i ++) {
		Collection_addItem (me, Tier_create (10));
	}
}

Label Label_create (long initialNumberOfTiers) {
	try {
		autoLabel me = Thing_new (Label);
		Label_init (me.peek(), initialNumberOfTiers);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Label not created.");
	}
}

void Label_addTier (Label me) {
	Collection_addItem (me, Tier_create (10));
}

void Label_suggestDomain (Label me, double *tmin, double *tmax) {
	*tmin = 0.0;
	*tmax = 0.0;
	for (int itier = 1; itier <= my size; itier ++) {
		Tier tier = (Tier) my item [itier];
		if (tier -> size) {
			Autosegment seg = (Autosegment) tier -> item [1];
			if (seg -> xmin <= *tmin) {
				if (seg -> name && seg -> name [0])
					*tmin = seg -> xmin - 1.0;
				else
					*tmin = seg -> xmin;
			}
			seg = (Autosegment) tier -> item [tier -> size];
			if (seg -> xmax >= *tmax)
				*tmax = seg -> xmax;
		}
	}
	*tmax += 1.0;
}

/* End of file Label.cpp */
