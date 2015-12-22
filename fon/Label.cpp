/* Label.cpp
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

void structAutosegment :: v_copy (Daata thee_Daata) {
	Autosegment thee = static_cast <Autosegment> (thee_Daata);
	Autosegment_Parent :: v_copy (thee);
	if (name) Thing_setName (thee, name);
}

bool structAutosegment :: v_equal (Daata thee_Daata) {
	Autosegment thee = static_cast <Autosegment> (thee_Daata);
	if (! Autosegment_Parent :: v_equal (thee)) return false;
	if (! our name && ! thy name) return true;   // shortcut: no names
	if (! our name || ! thy name) return false;
	return str32equ (name, thy name);
}

static struct structData_Description theAutosegment_description [] = {
	{ U"Autosegment", inheritwa, 0, sizeof (struct structAutosegment), U"Autosegment", & theClassInfo_Function },
	{ U"name", stringwa, Melder_offsetof (Autosegment, name), sizeof (char32 *) },
	{ 0 } };
Data_Description structAutosegment :: s_description = & theAutosegment_description [0];

autoAutosegment Autosegment_create (double tmin, double tmax, const char32 *label) {
	try {
		autoAutosegment me = Thing_new (Autosegment);
		Function_init (me.peek(), tmin, tmax);
		if (label) {
			Thing_setName (me.peek(), label);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Autosegment not created.");
	}
}

int structTier :: compareHook (Autosegment me, Autosegment thee) {
	if (my xmin < thy xmin) return -1;
	if (my xmin > thy xmin) return 1;
	if (my xmax < thy xmax) return -1;
	if (my xmax > thy xmax) return 1;
	return 0;
}

Thing_implement (Tier, Sorted, 0);

void Tier_init (Tier me, long initialCapacity) {
	Sorted_init (me, initialCapacity);
	Collection_addItem_move (me, Autosegment_create (-1e30, 1e30, nullptr));
}

autoTier Tier_create (long initialCapacity) {
	try {
		autoTier me = Thing_new (Tier);
		Tier_init (me.peek(), initialCapacity);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Tier not created.");
	}
}

long Tier_timeToIndex (Tier me, double time) {
	for (long i = 1; i <= my size; i ++) {
		Autosegment interval = (Autosegment) my item [i];
		if (time >= interval -> xmin && time < interval -> xmax)
			return i;
	}
	return 0;   // empty tier or very large time
}

Thing_implement (Label, Ordered, 0);

void Label_init (Label me, long initialNumberOfTiers) {
	Ordered_init (me, initialNumberOfTiers);
	for (long i = 1; i <= initialNumberOfTiers; i ++) {
		Collection_addItem_move (me, Tier_create (10));
	}
}

autoLabel Label_create (long initialNumberOfTiers) {
	try {
		autoLabel me = Thing_new (Label);
		Label_init (me.peek(), initialNumberOfTiers);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Label not created.");
	}
}

void Label_addTier (Label me) {
	Collection_addItem_move (me, Tier_create (10));
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
