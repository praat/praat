/* Simple_def.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/12
 */

#define ooSTRUCT SimpleInt
oo_DEFINE_CLASS (SimpleInt, Data)
	oo_INT (number)
oo_END_CLASS (SimpleInt)
#undef ooSTRUCT

#define ooSTRUCT SimpleShort
oo_DEFINE_CLASS (SimpleShort, Data)
	oo_SHORT (number)
oo_END_CLASS (SimpleShort)
#undef ooSTRUCT

#define ooSTRUCT SimpleLong
oo_DEFINE_CLASS (SimpleLong, Data)
	oo_LONG (number)
oo_END_CLASS (SimpleLong)
#undef ooSTRUCT

#define ooSTRUCT SimpleFloat
oo_DEFINE_CLASS (SimpleFloat, Data)
	oo_FLOAT (number)
oo_END_CLASS (SimpleFloat)
#undef ooSTRUCT

#define ooSTRUCT SimpleDouble
oo_DEFINE_CLASS (SimpleDouble, Data)
	oo_DOUBLE (number)
oo_END_CLASS (SimpleDouble)
#undef ooSTRUCT

#define ooSTRUCT SimpleString
oo_DEFINE_CLASS (SimpleString, Data)
	oo_STRINGW (string)
oo_END_CLASS (SimpleString)
#undef ooSTRUCT

/* End of file Simple_def.h */
