/* Label.cpp
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2018 Paul Boersma
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

#include "Label.h"

Thing_implement (Autosegment, Function, 0);

void structAutosegment :: v_copy (Daata thee_Daata) {
	Autosegment thee = static_cast <Autosegment> (thee_Daata);
	Autosegment_Parent :: v_copy (thee);
	if (name) Thing_setName (thee, name.get());
}

bool structAutosegment :: v_equal (Daata thee_Daata) {
	Autosegment thee = static_cast <Autosegment> (thee_Daata);
	if (! Autosegment_Parent :: v_equal (thee)) return false;
	if (! our name && ! thy name) return true;   // shortcut: no names
	if (! our name || ! thy name) return false;
	return str32equ (our name.get(), thy name.get());
}

static struct structData_Description theAutosegment_description [] = {
	{ U"Autosegment", inheritwa, 0, sizeof (struct structAutosegment), U"Autosegment", & theClassInfo_Function, 0, nullptr, nullptr, nullptr, nullptr },
	{ U"name", stringwa, Melder_offsetof (Autosegment, name), sizeof (char32 *), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
	{ } };
Data_Description structAutosegment :: s_description = & theAutosegment_description [0];

autoAutosegment Autosegment_create (double tmin, double tmax, conststring32 label) {
	try {
		autoAutosegment me = Thing_new (Autosegment);
		Function_init (me.get(), tmin, tmax);
		if (label) {
			Thing_setName (me.get(), label);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Autosegment not created.");
	}
}

/********** class Tier **********/

Thing_implement (Tier, Sorted, 0);

integer Tier_timeToIndex (Tier me, double time) {
	for (integer i = 1; i <= my size; i ++) {
		Autosegment interval = my at [i];
		if (time >= interval -> xmin && time < interval -> xmax)
			return i;
	}
	return 0;   // empty tier or very large time
}

Thing_implement (Label, Ordered, 0);

void Label_addTier (Label me) {
	autoTier tier = Tier_create ();
	my addItem_move (tier.move());
}

void Label_suggestDomain (Label me, double *tmin, double *tmax) {
	*tmin = 0.0;
	*tmax = 0.0;
	for (int itier = 1; itier <= my size; itier ++) {
		Tier tier = my at [itier];
		if (tier->size > 0) {
			Autosegment seg = tier->at [1];
			if (seg -> xmin <= *tmin) {
				if (seg -> name && seg -> name [0])
					*tmin = seg -> xmin - 1.0;
				else
					*tmin = seg -> xmin;
			}
			seg = tier->at [tier->size];
			if (seg -> xmax >= *tmax)
				*tmax = seg -> xmax;
		}
	}
	*tmax += 1.0;
}

/* End of file Label.cpp */
