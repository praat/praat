/* Categories.cpp
 *
 * Copyright (C) 1993-2018 David Weenink, 2015,2017,2018 Paul Boersma
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

/*
 djmw 20020315 GPL header
 djmw 20110304 Thing_new
 */

#include "Categories.h"

void structCategories :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of strings: ", our size);
	autoStringSet set = StringList_to_StringSet (this);
	MelderInfo_writeLine (U"Number of unique categories: ", set->size);
}

void structCategories :: v_readText (MelderReadText a_text, int /*formatVersion*/) {
	integer l_size = texgeti32 (a_text);
	if (l_size == 0)
		(void) 0;
	else if (l_size < 0)
		Melder_throw (U"Size cannot be negative.");
	else
		our _grow (l_size);

	for (integer i = 1; i <= l_size; i ++) {
		autoSimpleString itemi = Thing_new (SimpleString);
		itemi -> v_readText (a_text, 0);
		our addItemAtPosition_move (itemi.move(), i);
	}
}

void structCategories :: v_writeText (MelderFile file) {
	texputi32 (file, our size, U"size");
	for (integer i = 1; i <= our size; i ++) {
		SimpleString data = our at [i];
		texputintro (file, U"item [", Melder_integer (i), U"]:");
		data -> structSimpleString :: v_writeText (file);
		texexdent (file);
	}
}

Thing_implement (Categories, StringList, 0);

autoCategories Categories_create () {
	try {
		autoCategories me = Thing_new (Categories);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Categories not created.");
	}
}

autoCategories Categories_createWithSequentialNumbers (integer n) {
	try {
		autoCategories me = Thing_new (Categories);
		OrderedOfString_initWithSequentialNumbers (me.get(), n);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sequential number Categories not created.");
	}
}

autoCategories Categories_selectUniqueItems (Categories me) {
	try {
		autoStringSet set = StringList_to_StringSet (me);
		autoCategories thee = Categories_create ();
		for (integer i = 1; i <= set->size; i ++) {
			autoSimpleString item = Data_copy (set->at [i]);
			thy addItem_move (item.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no unique categories created.");
	}
}

void Categories_drawItem (Categories me, Graphics g, integer position, double xWC, double yWC) {
	if (position < 1 || position > my size) return;
	SimpleString item = my at [position];
	Graphics_text (g, xWC, yWC, item -> string.get());
}

/* TableOfReal_Rowlabels_to_Categories  ??? */
autoCategories TableOfReal_to_CategoriesRow (TableOfReal me) {
	try {
		autoCategories thee = Categories_create ();

		for (integer i = 1; i <= my numberOfRows; i ++) {
			if (my rowLabels [i]) {
				autoSimpleString s = SimpleString_create (my rowLabels [i].get());
				thy addItem_move (s.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": row labels not converted to Categories.");
	}
}

autoCategories TableOfReal_to_CategoriesColumn (TableOfReal me) {
	try {
		autoCategories thee = Categories_create ();

		for (integer i = 1; i <= my numberOfColumns; i ++) {
			if (my columnLabels [i]) {
				autoSimpleString s = SimpleString_create (my columnLabels [i].get());
				thy addItem_move (s.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": columnlabels not converted to Categories.");
	}
}

/* End of file Categories.cpp */
