/* Simple.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

Thing_implement (SimpleInt, Data, 0);

SimpleInt SimpleInt_create (int number) {
	autoSimpleInt me = Thing_new (SimpleInt);
	my number = number;
	return me.transfer();
}

Thing_implement (SimpleLong, Data, 0);

SimpleLong SimpleLong_create (long number) {
	autoSimpleLong me = Thing_new (SimpleLong);
	my number = number;
	return me.transfer();
}

Thing_implement (SimpleDouble, Data, 0);

SimpleDouble SimpleDouble_create (double number) {
	autoSimpleDouble me = Thing_new (SimpleDouble);
	my number = number;
	return me.transfer();
}

Thing_implement (SimpleString, Data, 0);

SimpleString SimpleString_create (const wchar_t *string) {
	autoSimpleString me = Thing_new (SimpleString);
	my string = Melder_wcsdup (string);
	return me.transfer();
}

/* End of file Simple.cpp */
