/* Categories.cpp
 *
 * Copyright (C) 1993-2013, 2015 David Weenink, 2015 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020315 GPL header
 djmw 20110304 Thing_new
 */

#include "Categories.h"

void structCategories :: v_readText (MelderReadText a_text, int /*formatVersion*/) {
	long l_size = texgeti4 (a_text);
	if (l_size == 0) {
		(void) 0;
	} else if (l_size < 0) {
		Melder_throw (U"Size cannot be negative.");
	} else {
		our _grow (l_size);
	}
	for (long i = 1; i <= l_size; i ++) {
		autoSimpleString itemi = Thing_new (SimpleString);
		itemi -> v_readText (a_text, 0);
		our addItemAtPosition_move (itemi.move(), i);
	}
}

void structCategories :: v_writeText (MelderFile file) {
	texputi4 (file, our size(), U"size", nullptr, nullptr, nullptr, nullptr, nullptr);
	for (long i = 1; i <= our size(); i ++) {
		SimpleString data = our _item [i];
		texputintro (file, U"item" " [", Melder_integer (i), U"]:", nullptr, nullptr, nullptr);
		data -> structSimpleString :: v_writeText (file);
		texexdent (file);
	}
}

Thing_implement (Categories, OrderedOfString, 0);

autoCategories Categories_create () {
	try {
		autoCategories me = Thing_new (Categories);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Categories not created.");
	}
}

autoCategories Categories_sequentialNumbers (long n) {
	try {
		autoCategories me = Thing_new (Categories);
		OrderedOfString_sequentialNumbers (me.peek(), n);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sequential number Categories not created.");
	}
}

autoCategories Categories_selectUniqueItems (Categories me) {
	try {
		autoOrderedOfString s = OrderedOfString_selectUniqueItems (me);
		autoCategories thee = OrderedOfString_to_Categories (s.peek());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no unique categories created.");
	}
}

void Categories_drawItem (Categories me, Graphics g, long position, double xWC, double yWC) {
	if (position < 1 || position > my size()) {
		return;
	}
	SimpleString_draw (my _item [position], g, xWC, yWC);
}

autoCategories OrderedOfString_to_Categories (OrderedOfString me) {
	try {
		autoCategories thee = Categories_create();

		for (long i = 1; i <= my size(); i ++) {
			autoSimpleString item = Data_copy (my _item [i]);
			thy addItem_move (item.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Categories.");
	}
}

long Categories_getSize (Categories me) {
	return my size();
}

/* TableOfReal_Rowlabels_to_Categories  ??? */
autoCategories TableOfReal_to_CategoriesRow (TableOfReal me) {
	try {
		autoCategories thee = Categories_create ();

		for (long i = 1; i <= my numberOfRows; i ++) {
			if (my rowLabels[i]) {
				autoSimpleString s = SimpleString_create (my rowLabels [i]);
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

		for (long i = 1; i <= my numberOfColumns; i ++) {
			if (my columnLabels[i]) {
				autoSimpleString s = SimpleString_create (my columnLabels [i]);
				thy addItem_move (s.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": columnlabels not converted to Categories.");
	}
}

/* End of file Categories.cpp */
