#ifndef _Simple_h_
#define _Simple_h_
/* Simple.h
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2008/03/19
 */

#ifndef _Data_h_
	#include "Data.h"
#endif

#include "Simple_def.h"

#define SimpleInt_methods Data_methods
oo_CLASS_CREATE (SimpleInt, Data);

#define SimpleShort_methods Data_methods
oo_CLASS_CREATE (SimpleShort, Data);

#define SimpleLong_methods Data_methods
oo_CLASS_CREATE (SimpleLong, Data);

#define SimpleDouble_methods Data_methods
oo_CLASS_CREATE (SimpleDouble, Data);

#define SimpleString_methods Data_methods
oo_CLASS_CREATE (SimpleString, Data);

GLOBAL_C SimpleInt SimpleInt_create (int number);
GLOBAL_C SimpleShort SimpleShort_create (short number);
GLOBAL_C SimpleLong SimpleLong_create (long number);
GLOBAL_C SimpleDouble SimpleDouble_create (double number);
GLOBAL_C SimpleString SimpleString_create (const wchar_t *string);

#endif
