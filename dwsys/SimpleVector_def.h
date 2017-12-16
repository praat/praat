/* SimpleVector_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020606 GPL header
*/

#define ooSTRUCT DoubleVector
oo_DEFINE_CLASS (DoubleVector, Daata)

	oo_INTEGER (min)
	oo_INTEGER (max)
	oo_DOUBLE_VECTOR_FROM (v, min, max)
	
oo_END_CLASS (DoubleVector)	
#undef ooSTRUCT

#define ooSTRUCT ComplexVector
oo_DEFINE_CLASS (ComplexVector, Daata)

	oo_INTEGER (min)
	oo_INTEGER (max)
	oo_COMPLEX_VECTOR_FROM (v, min, max)
	
oo_END_CLASS (ComplexVector)	
#undef ooSTRUCT


/* End of file SimpleVector_def.h */	
