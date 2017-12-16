/* Simple_def.h
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

#define ooSTRUCT SimpleInteger
oo_DEFINE_CLASS (SimpleInteger, Daata)
	oo_INTEGER (number)
oo_END_CLASS (SimpleInteger)
#undef ooSTRUCT

#define ooSTRUCT SimpleDouble
oo_DEFINE_CLASS (SimpleDouble, Daata)
	oo_DOUBLE (number)
oo_END_CLASS (SimpleDouble)
#undef ooSTRUCT

#define ooSTRUCT SimpleString
oo_DEFINE_CLASS (SimpleString, Daata)
	oo_STRING (string)
oo_END_CLASS (SimpleString)
#undef ooSTRUCT

/* End of file Simple_def.h */
