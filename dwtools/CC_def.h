/* CC_def.h
 * 
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 2000
 djmw 20020315 GPL header
 */

#define ooSTRUCT CC_Frame
oo_DEFINE_STRUCT (CC_Frame)

	oo_LONG (numberOfCoefficients)
	oo_FLOAT (c0)
	oo_FLOAT_VECTOR (c, my numberOfCoefficients)

oo_END_STRUCT (CC_Frame)
#undef ooSTRUCT

#define ooSTRUCT CC
oo_DEFINE_CLASS (CC, Sampled)

	oo_DOUBLE (fmin)
	oo_DOUBLE (fmax)
	oo_LONG (maximumNumberOfCoefficients)
	oo_STRUCT_VECTOR (CC_Frame, frame, my nx)

oo_END_CLASS (CC)
#undef ooSTRUCT

/* End of file CC_def.h */
