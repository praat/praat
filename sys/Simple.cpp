/* Simple.cpp
 *
 * Copyright (C) 1992-2012,2015,2017 Paul Boersma
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

#include "Simple.h"

#include "oo_DESTROY.h"
#include "Simple_def.h"
#include "oo_COPY.h"
#include "Simple_def.h"
#include "oo_EQUAL.h"
#include "Simple_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Simple_def.h"
#include "oo_WRITE_TEXT.h"
#include "Simple_def.h"
#include "oo_WRITE_BINARY.h"
#include "Simple_def.h"
#include "oo_READ_TEXT.h"
#include "Simple_def.h"
#include "oo_READ_BINARY.h"
#include "Simple_def.h"
#include "oo_DESCRIPTION.h"
#include "Simple_def.h"

Thing_implement (SimpleInteger, Daata, 0);

autoSimpleInteger SimpleInteger_create (integer number) {
	autoSimpleInteger me = Thing_new (SimpleInteger);
	my number = number;
	return me;
}

Thing_implement (SimpleDouble, Daata, 0);

autoSimpleDouble SimpleDouble_create (double number) {
	autoSimpleDouble me = Thing_new (SimpleDouble);
	my number = number;
	return me;
}

Thing_implement (SimpleString, Daata, 0);

autoSimpleString SimpleString_create (conststring32 string) {
	autoSimpleString me = Thing_new (SimpleString);
	my string = Melder_dup (string);
	return me;
}

int SimpleString_compare (SimpleString me, SimpleString thee) noexcept {
	return str32cmp (my string.get(), thy string.get());
}

/* End of file Simple.cpp */
