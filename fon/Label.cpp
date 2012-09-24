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

#include "Label.h"

Thing_implement (Autosegment, Function, 0);

void structAutosegment :: v_copy (thou) {
	thouart (Autosegment);
	Autosegment_Parent :: v_copy (thee);
	if (name) Thing_setName (thee, name);
}

bool structAutosegment :: v_equal (thou) {
	thouart (Autosegment);
	if (! Autosegment_Parent :: v_equal (thee)) return false;
	if (name == NULL && thy name == NULL) return true;   // shortcut: no names
	if (name == NULL || thy name == NULL) return false;
	return wcsequ (name, thy name);
}

static struct structData_Description theAutosegment_description [] = {
	{ L"Autosegment", inheritwa, 0, sizeof (struct structAutosegment), L"Autosegment", & theClassInfo_Function },
	{ L"name", stringwa, Melder_offsetof (Autosegment, name), sizeof (wchar_t *) },
	{ 0 } };
Data_Description structAutosegment :: s_description = & theAutosegment_description [0];

Autosegment Autosegment_create (double tmin, double tmax, const wchar_t *label) {
	try {
		autoAutosegment me = Thing_new (Autosegment);
		Function_init (me.peek(), tmin, tmax);
		if (label != NULL) {
			Thing_setName (me.peek(), label);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Autosegment not created.");
	}
}

int structTier :: compare (I, thou) {
	iam (Function); thouart (Function);
	if (my xmin < thy xmin) return -1;
	if (my xmin > thy xmin) return 1;
	if (my xmax < thy xmax) return -1;
	if (my xmax > thy xmax) return 1;
	return 0;
}

Thing_implement (Tier, Sorted, 0);

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

Thing_implement (Label, Ordered, 0);

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
