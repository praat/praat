/* Discriminant_def.h
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
 djmw 1998
 djmw 20020813 GPL header
*/

#define ooSTRUCT Discriminant
oo_DEFINE_CLASS (Discriminant, Eigen)
	
	oo_LONG (numberOfGroups)
	oo_OBJECT (SSCPs, groups)
	oo_OBJECT (SSCP, total)
	oo_DOUBLE_VECTOR (aprioriProbabilities, my numberOfGroups)
	oo_DOUBLE_MATRIX (costs, my numberOfGroups, my numberOfGroups)

oo_END_CLASS (Discriminant)	
#undef ooSTRUCT

/* End of file Discriminant_def.h */	
