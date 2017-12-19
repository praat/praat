/* Simple_extensions.cpp
 *
 * Copyright (C) 1994-2011, 2015-2017 David Weenink
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
 djmw 20020812 GPL header
 djmw & pb wchar
*/

#include "Simple_extensions.h"
#include "longchar.h"

void SimpleString_init (SimpleString me, const char32 *string) {
	my string = Melder_dup (string);
}

const char32 *SimpleString_c (SimpleString me) {
	return my string;
}

void SimpleString_append (SimpleString me, SimpleString thee) {
	SimpleString_append_c (me, thy string);
}

void SimpleString_append_c (SimpleString me, const char32 *str) {
	if (! str) {
		return;
	}
	integer myLength = str32len (my string);
	my string = (char32 *) Melder_realloc (my string, (myLength + str32len (str) + 1) * (integer) sizeof (char32));
	str32cpy (& my string[myLength], str);
}

autoSimpleString SimpleString_concat (SimpleString me, SimpleString thee) {
	autoSimpleString him = Data_copy (me);
	SimpleString_append_c (him.get(), thy string);
	return him;
}

autoSimpleString SimpleString_concat_c (SimpleString me, const char32 *str) {
	autoSimpleString him = Data_copy (me);
	SimpleString_append_c (him.get(), str);
	return him;
}

void SimpleString_replace_c (SimpleString me, const char32 *str) {
	char32 *ptr = Melder_dup (str);
	Melder_free (my string);
	my string = ptr;
}

integer SimpleString_length (SimpleString me) {
	return str32len (my string);
}

void SimpleString_draw (SimpleString me, Graphics g, double xWC, double yWC) {
	Graphics_text (g, xWC, yWC, my string);
}

const char32 *SimpleString_nativize_c (SimpleString me, int educateQuotes) {
	autoSimpleString thee = Data_copy (me);
	Longchar_nativize32 (thy string, my string, educateQuotes);
	return my string;
}

const char32 *SimpleString_genericize_c (SimpleString me) {
	autoSimpleString thee = Data_copy (me);
	my string = (char32 *) Melder_realloc (my string, (3 * str32len (my string) + 1) * (integer) sizeof (char32));
	Longchar_genericize32 (thy string, my string);
	return my string;
}

/* End of file Simple_extensions.cpp */
