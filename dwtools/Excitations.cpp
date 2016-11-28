/* Excitations.cpp
 *
 * Copyright (C) 1993-2011, 2015-2016 David Weenink, 2016 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Excitations.h"

Thing_implement (ExcitationList, Ordered, 0);

void ExcitationList_addItem_copy (ExcitationList me, Excitation you) {
	try {
		if (my size > 0) {
			if (your nx != my at [1] -> nx) { // may test more dx, xmin, xmax?
				Melder_throw (U"Dimension of ", you, U" differs from the rest.");
			}
		}
		autoExcitation newItem = Data_copy (you);
		my addItem_move (newItem.move());
	} catch (MelderError) {
		Melder_throw (me, U": item not added.");
	}
}

void ExcitationList_addItems (ExcitationList me, OrderedOf <structExcitation> * list) {
	for (long i = 1; i <= list -> size; i ++) {
		ExcitationList_addItem_copy (me, list -> at [i]);
	}
}

autoExcitationList Excitations_to_ExcitationList (OrderedOf <structExcitation> * me) {
	try {
		autoExcitationList you = ExcitationList_create ();
		ExcitationList_addItems (you.get(), me);
		return you;
	} catch (MelderError) {
		Melder_throw (U"No ExcitationList created from Excitation(s).");
	}
}

autoPatternList ExcitationList_to_PatternList (ExcitationList me, long join) {
	try {
		Melder_assert (my size > 0);
		Excitation excitation = my at [1];
		if (join < 1) {
			join = 1;
		}
		if (my size % join != 0) {
			Melder_throw (U"Number of rows is not a multiple of join.");
		}
		autoPatternList thee = PatternList_create (my size / join, join * excitation -> nx);
		long r = 0, c = 1;
		for (long i = 1; i <= my size; i ++) {
			double *z = my at [i] -> z [1];
			if ((i - 1) % join == 0) {
				r ++;
				c = 1;
			}
			for (long j = 1; j <= excitation -> nx; j ++) {
				thy z [r] [c ++] = z [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PatternList created.");
	}
}

autoTableOfReal ExcitationList_to_TableOfReal (ExcitationList me) {
	try {
		Melder_assert (my size > 0);
		Excitation excitation = my at [1];
		autoTableOfReal thee = TableOfReal_create (my size, excitation -> nx);
		for (long i = 1; i <= my size; i ++) {
			double *z = my at [i] -> z [1];
			for (long j = 1; j <= excitation -> nx; j ++) {
				thy data [i] [j] = z [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": TableOfReal not created.");
	}
}

autoExcitation ExcitationList_extractItem (ExcitationList me, long item) {
	try {
		if (item < 1 || item > my size) {
			Melder_throw (U"Not a valid element number.");
		}
		autoExcitation thee = Data_copy (my at [item]);
		Thing_setName (thee.get(), Thing_getName (my at [item]));
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Excitation not extracted.");
	}
}

/* End of file Excitations.cpp */
