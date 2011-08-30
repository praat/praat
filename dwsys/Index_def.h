/* Index_def.h
 *
 * Copyright (C) 2005-2006 David Weenink
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
 djmw 20050724
 djmw 20060529 Added object version numbers.
*/

#define ooSTRUCT Index
oo_DEFINE_CLASS (Index, Data)
	oo_OBJECT (Ordered, 0, classes)
	oo_LONG (numberOfElements)
	oo_LONG_VECTOR (classIndex, numberOfElements)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS(Index)	
#undef ooSTRUCT

#define ooSTRUCT StringsIndex
oo_DEFINE_CLASS (StringsIndex, Index)
oo_END_CLASS(StringsIndex)
#undef ooSTRUCT

/* End of file Index_def.h */	
