/* Sampled_def.h
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
 * pb 1995/08/24
 * pb 2002/07/16 GPL
 */


#define ooSTRUCT Sampled
oo_DEFINE_CLASS (Sampled, Function)

	oo_LONG (nx)
	oo_DOUBLE (dx)
	oo_DOUBLE (x1)

oo_END_CLASS (Sampled)
#undef ooSTRUCT


/* End of file Sampled_def.h */
