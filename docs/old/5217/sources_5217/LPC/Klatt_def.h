/* Klatt_def.h
 *
 * Copyright (C) 2008 David Weenink
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
 djmw 20080124
*/

#define ooSTRUCT GlottisControlPoint
oo_DEFINE_CLASS (GlottisControlPoint, Data)

	oo_DOUBLE (time)   /* AnyPoint */
	oo_DOUBLE (amplitude) /* [0,1] */
	oo_DOUBLE (openPhase) /* [0,1] */
	oo_DOUBLE (power1) /* t^2 */
	oo_DOUBLE (power2) /* t^3 */

oo_END_CLASS (GlottisControlPoint)
#undef ooSTRUCT

#define ooSTRUCT GlottisControlTier
oo_DEFINE_CLASS (GlottisControlTier, Function)

	oo_COLLECTION (SortedSetOfDouble, points, GlottisControlPoint, 0)

oo_END_CLASS (GlottisControlTier)
#undef ooSTRUCT

#define ooSTRUCT Klatt
oo_DEFINE_CLASS (Klatt, Function)
	
	oo_INT (followsReferenceImplementation)
	oo_OBJECT (GlottisControlTier, 0, gct)
	oo_OBJECT (FormantTier, 0, ft)
	oo_OBJECT (IntensityTier, 0, it)
	
oo_END_CLASS (Klatt)
#undef ooSTRUCT

/* End of file LPC_def.h */

