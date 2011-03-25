/* Label.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/13 wchar_t
 */

#include "Label.h"

static int copy (I, thou) {
	iam (Autosegment); thouart (Autosegment);
	if (! inherited (Autosegment) copy (me, thee)) return 0;
	return my name == NULL || (Thing_setName (thee, my name), my name != NULL);
}

static bool equal (I, thou) {
	iam (Autosegment); thouart (Autosegment);
	if (! inherited (Autosegment) equal (me, thee)) return 0;
	if (my name == NULL && thy name == NULL) return 1;   /* Shortcut: no names. */
	if (my name == NULL || thy name == NULL) return 0;
	return wcsequ (my name, thy name);
}

static struct structData_Description description [] = {
	{ L"Autosegment", inheritwa, 0, sizeof (struct structAutosegment), L"Autosegment", & theStructFunction. description },
	{ L"name", stringwa, (int) & ((Autosegment) 0) -> name, sizeof (wchar_t *) },
	{ 0 } };

class_methods (Autosegment, Function)
	class_method (copy)
	class_method (equal)
	class_method (description)
class_methods_end

Any Autosegment_create (double tmin, double tmax, const wchar_t *label) {
	Autosegment me = Thing_new (Autosegment);
	if (! me || ! Function_init (me, tmin, tmax) || (label != NULL && (Thing_setName (me, label), my name == NULL)))
		forget (me);
	return me;
}

static int compare (I, thou) {
	iam (Function); thouart (Function);
	if (my xmin < thy xmin) return -1;
	if (my xmin > thy xmin) return 1;
	if (my xmax < thy xmax) return -1;
	if (my xmax > thy xmax) return 1;
	return 0;
}

class_methods (Tier, Sorted)
	class_method (compare)
class_methods_end

int Tier_init (I, long initialCapacity) {
	iam (Tier);
	Autosegment autosegment = NULL;
	if (! Sorted_init (me, classAutosegment, initialCapacity) ||
			! (autosegment = Autosegment_create (-1e30, 1e30, NULL)) ||
			! Collection_addItem (me, autosegment)) {
		forget (autosegment);
		return 0;
	}
	return 1;
}

Tier Tier_create (long initialCapacity) {
	Tier me = Thing_new (Tier);
	if (! Tier_init (me, initialCapacity)) forget (me);
	return me;
}

long Tier_timeToIndex (Tier me, double t) {
	long i; 
	for (i = 1; i <= my size; i ++) {
		Autosegment interval = my item [i];
		if (t >= interval -> xmin && t < interval -> xmax)
			return i;
	}
	return 0;   /* Empty tier or very large t. */
}

class_methods (Label, Ordered)
class_methods_end

int Label_init (I, long initialNumberOfTiers) {
	iam (Label);
	int i;
	if (! Ordered_init (me, classTier, initialNumberOfTiers)) return 0;
	for (i = 1; i <= initialNumberOfTiers; i ++) {
		Tier tier;
		if (! (tier = Tier_create (10)) || ! Collection_addItem (me, tier))
			{ forget (tier); return 0; }
	}
	return 1;
}

Label Label_create (long initialNumberOfTiers) {
	Label me = Thing_new (Label);
	if (! Label_init (me, initialNumberOfTiers)) forget (me);
	return me;
}

int Label_addTier (Label me) {
	Tier tier = Tier_create (10);
	if (! tier) return 0;
	if (! Collection_addItem (me, tier)) return 0;
	return 1;
}

void Label_suggestDomain (Label me, double *tmin, double *tmax) {
	int itier;
	*tmin = 0.0;
	*tmax = 0.0;
	for (itier = 1; itier <= my size; itier ++) {
		Tier tier = my item [itier];
		if (tier -> size) {
			Autosegment seg = tier -> item [1];
			if (seg -> xmin <= *tmin) {
				if (seg -> name && seg -> name [0])
					*tmin = seg -> xmin - 1.0;
				else
					*tmin = seg -> xmin;
			}
			seg = tier -> item [tier -> size];
			if (seg -> xmax >= *tmax)
				*tmax = seg -> xmax;
		}
	}
	*tmax += 1.0;
}

/* End of file Label.c */
