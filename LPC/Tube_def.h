/* Tube_def.h
 *
 * Copyright (C) 1994-2011 David Weenink
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


#define ooSTRUCT Tube_Frame
oo_DEFINE_STRUCT (Tube_Frame)

	oo_INT (nSegments)
	oo_DOUBLE (length)
	oo_DOUBLE_VECTOR (c, nSegments)
	
oo_END_STRUCT (Tube_Frame)
#undef ooSTRUCT


#define ooSTRUCT Tube
oo_DEFINE_CLASS (Tube, Sampled)
	
	oo_INT (maxnSegments)
	oo_STRUCT_VECTOR (Tube_Frame, frame, nx)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (Tube)
#undef ooSTRUCT


/* End of file Tube_def.h */
