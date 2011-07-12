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

/*
 * pb 2011/06/02
 */

#include "Collection.h"
#include "Function.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (Autosegment);

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

Thing_declare1cpp (Tier);

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

Thing_declare1cpp (Label);

Label Label_create (long initialNumberOfTiers);

void Label_init (I, long initialNumberOfTiers);

void Label_addTier (Label me);

void Label_suggestDomain (Label me, double *tmin, double *tmax);

#ifdef __cplusplus
	}

	struct structAutosegment : public structFunction {
	};
	#define Autosegment__methods(klas) Function__methods(klas)
	Thing_declare2cpp (Autosegment, Function);

	struct structTier : public structSorted {
	};
	#define Tier__methods(klas) Sorted__methods(klas)
	Thing_declare2cpp (Tier, Sorted);

	struct structLabel : public structOrdered {
	};
	#define Label__methods(klas) Ordered__methods(klas)
	Thing_declare2cpp (Label, Ordered);

#endif

#endif
/* End of file Label.h */
