/* Sequence_def.h
 *
 * Copyright (C) 1994-2002 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 19970507
 djmw 20020812 GPL header
*/

#define ooSTRUCT Sequence
oo_DEFINE_CLASS (Sequence, Data)
	oo_INT (action)
	oo_LONG (n)
	oo_LONG (seed)
	oo_LONG (iy)
	oo_LONG (ntab)
	oo_LONG_VECTOR (tab, my ntab)
	oo_LONG_VECTOR (p, my n)
			
oo_END_CLASS(Sequence)	
#undef ooSTRUCT

/* End of file Sequence_def.h */	
