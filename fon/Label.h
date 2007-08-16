#ifndef _Label_h_
#define _Label_h_
/* Label.h
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
 * pb 2007/08/13
 */

#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Function_h_
	#include "Function.h"
#endif

#define Autosegment_members Function_members
#define Autosegment_methods Function_methods
class_create (Autosegment, Function);

Any Autosegment_create (double tmin, double tmax, const wchar_t *label);
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

#define Tier_members Sorted_members
#define Tier_methods Sorted_methods
class_create (Tier, Sorted);

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

int Tier_init (I, long initialCapacity);

#define Label_members Ordered_members
#define Label_methods Ordered_methods
class_create (Label, Ordered);

Label Label_create (long initialNumberOfTiers);

int Label_init (I, long initialNumberOfTiers);

int Label_addTier (Label me);

void Label_suggestDomain (Label me, double *tmin, double *tmax);

#endif
/* End of file Label.h */
