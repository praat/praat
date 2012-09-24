#ifndef _Label_h_
#define _Label_h_
/* Label.h
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

#include "Collection.h"
#include "Function.h"

Thing_define (Autosegment, Function) {
	// overridden methods:
	protected:
		virtual void v_copy (Any data_to);
		virtual bool v_equal (Any otherData);
		static Data_Description s_description;
		virtual Data_Description v_description () { return s_description; }
};

Autosegment Autosegment_create (double tmin, double tmax, const wchar_t *label);
/*
	Function:
		create a new instance of Autosegment.
	Return value:
		the new Autosegment, or NULL if out of memory.
	Preconditions:
		tmax > tmin;
		label may be NULL;
	Postconditions:
		result -> tmin == tmin;
		result -> tmax == tmax;
		if (label != NULL)
			result -> name == NULL;
		else
			result -> name [] == label [];   // 'label' copied into 'name'
*/

Thing_define (Tier, Sorted) {
	// overridden methods:
		static int compare (Any data1, Any data2);
		virtual int (*v_getCompareFunction ()) (Any data1, Any data2) { return compare; }
};

void Tier_init (I, long initialCapacity);

Tier Tier_create (long initialCapacity);
/*
	Function:
		create a new Tier containing one Autosegment from -1e30 to 1e30.
	Return value:
		the new Tier, or NULL if out of memory.
	Postconditions:
		my size == 1;
		my item [1] -> methods == classAutosegment;
		my item [1] -> xmin == -1e30;
		my item [1] -> xmax == 1e30;
		my item [1] -> name == NULL;
*/

long Tier_timeToIndex (Tier me, double t);
/*
	Return value:
		index, or 0 if the tier is empty or t is very large.
	Postconditions:
		result == 0 || my item [i] -> xmin <= result < my item [i] -> xmax;
*/

void Tier_init (I, long initialCapacity);

Thing_define (Label, Ordered) {
};

Label Label_create (long initialNumberOfTiers);

void Label_init (I, long initialNumberOfTiers);

void Label_addTier (Label me);

void Label_suggestDomain (Label me, double *tmin, double *tmax);

#endif
/* End of file Label.h */
