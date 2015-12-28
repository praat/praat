#ifndef _Label_h_
#define _Label_h_
/* Label.h
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

#include "Collection.h"
#include "Function.h"

Thing_define (Autosegment, Function) {
	void v_copy (Daata data_to)
		override;
	bool v_equal (Daata otherData)
		override;
	static Data_Description s_description;
	Data_Description v_description ()
		override { return s_description; }
};

autoAutosegment Autosegment_create (double tmin, double tmax, const char32 *label);
/*
	Function:
		create a new instance of Autosegment.
	Return value:
		the new Autosegment.
	Preconditions:
		tmax > tmin;
		label may be null;
	Postconditions:
		result -> tmin == tmin;
		result -> tmax == tmax;
		if (label)
			result -> name == nullptr;
		else
			result -> name [] == label [];   // 'label' copied into 'name'
*/

Collection_declare (SortedOfAutosegment, SortedOf, Autosegment);

Thing_define (Tier, SortedOfAutosegment) {

	/**
		Initialize a new Tier containing one Autosegment from -1e30 to 1e30.
		@post
			this->size() == 1;
			(*this) [1] -> classInfo == classAutosegment;
			(*this) [1] -> xmin == -1e30;
			(*this) [1] -> xmax == 1e30;
			(*this) [1] -> name == nullptr;
	*/
	structTier () {
		our classInfo = classTier;
		our addItem_move (Autosegment_create (-1e30, 1e30, nullptr));
	}

	static int compareHook (Autosegment me, Autosegment thee) {
		if (my xmin < thy xmin) return -1;
		if (my xmin > thy xmin) return 1;
		if (my xmax < thy xmax) return -1;
		if (my xmax > thy xmax) return 1;
		return 0;
	}
	SortedOf<structAutosegment>::CompareHook v_getCompareHook () override { return compareHook; }
};

/**
	@return
		index, or 0 if the tier is empty or t is very large.
	@post
		result == 0 || my item [i] -> xmin <= result < my item [i] -> xmax;
*/
long Tier_timeToIndex (Tier me, double t);

Collection_declare (OrderedOfTier, OrderedOf, Tier);

Thing_define (Label, OrderedOfTier) {
	structLabel () {
		our classInfo = classLabel;
	}
};

void Label_addTier (Label me);

void Label_suggestDomain (Label me, double *tmin, double *tmax);

#endif
/* End of file Label.h */
