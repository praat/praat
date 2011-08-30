/* SimpleVector_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
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
 djmw 20020606 GPL header
*/

#define ooSTRUCT DoubleVector
oo_DEFINE_CLASS (DoubleVector, Data)

	oo_LONG (min)
	oo_LONG (max)
	oo_DOUBLE_VECTOR_FROM (v, min, max)
	
oo_END_CLASS (DoubleVector)	
#undef ooSTRUCT

#define ooSTRUCT ComplexVector
oo_DEFINE_CLASS (ComplexVector, Data)

	oo_LONG (min)
	oo_LONG (max)
	oo_DCOMPLEX_VECTOR_FROM (v, min, max)
	
oo_END_CLASS (ComplexVector)	
#undef ooSTRUCT


/* End of file SimpleVector_def.h */	
