#ifndef _Simple_h_
#define _Simple_h_
/* Simple.h
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

#include "Data.h"

#include "Simple_def.h"

autoSimpleInteger SimpleInteger_create (integer number);
autoSimpleDouble SimpleDouble_create (double number);
autoSimpleString SimpleString_create (conststring32 string);

int SimpleString_compare (SimpleString me, SimpleString thee) noexcept;

/* End of file Simple.h */
#endif
