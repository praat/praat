/* Configuration_def.h
 *
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 19971207
 djmw 20020813 GPL header
*/

#define ooSTRUCT Configuration
oo_DEFINE_CLASS (Configuration, TableOfReal)

	oo_LONG (metric)
	oo_DOUBLE_VECTOR (w, my numberOfColumns)
				
oo_END_CLASS (Configuration)	
#undef ooSTRUCT

/* End of file Configuration_def.h */	
