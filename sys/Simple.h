#ifndef _Simple_h_
#define _Simple_h_
/* Simple.h
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

#include "Data.h"

#include "Simple_def.h"
oo_CLASS_CREATE (SimpleInt, Data);
oo_CLASS_CREATE (SimpleLong, Data);
oo_CLASS_CREATE (SimpleDouble, Data);
oo_CLASS_CREATE (SimpleString, Data);

SimpleInt SimpleInt_create (int number);
SimpleLong SimpleLong_create (long number);
SimpleDouble SimpleDouble_create (double number);
SimpleString SimpleString_create (const wchar_t *string);

/* End of file Simple.h */
#endif
