/* Function_def.h
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
 * pb 2007/10/15
 */


#define ooSTRUCT Function
oo_DEFINE_CLASS (Function, Data)

	oo_DOUBLE (xmin)
	oo_DOUBLE (xmax)

	#if oo_READING
		if (my xmin > my xmax)
			return Melder_error5 (L"Wrong xmin ", Melder_double (my xmin), L" and xmax ", Melder_double (my xmax), L".");
	#endif

oo_END_CLASS (Function)
#undef ooSTRUCT


/* End of file Function_def.h */
