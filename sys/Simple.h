#ifndef _Simple_h_
#define _Simple_h_
/* Simple.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1996/07/24
 * pb 2002/03/07 GPL
 */

#ifndef _Data_h_
	#include "Data.h"
#endif

#include "Simple_def.h"

#define SimpleInt_methods Data_methods
oo_CLASS_CREATE (SimpleInt, Data)

#define SimpleShort_methods Data_methods
oo_CLASS_CREATE (SimpleShort, Data)

#define SimpleLong_methods Data_methods
oo_CLASS_CREATE (SimpleLong, Data)

#define SimpleFloat_methods Data_methods
oo_CLASS_CREATE (SimpleFloat, Data)

#define SimpleDouble_methods Data_methods
oo_CLASS_CREATE (SimpleDouble, Data)

#define SimpleString_methods Data_methods
oo_CLASS_CREATE (SimpleString, Data)

SimpleInt SimpleInt_create (int number);
SimpleShort SimpleShort_create (short number);
SimpleLong SimpleLong_create (long number);
SimpleFloat SimpleFloat_create (float number);
SimpleDouble SimpleDouble_create (double number);
SimpleString SimpleString_create (const char *string);

#endif
