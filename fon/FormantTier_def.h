/* FormantTier_def.h
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


#define ooSTRUCT FormantPoint
oo_DEFINE_CLASS (FormantPoint, Data)

	oo_DOUBLE (time)   /* AnyPoint */
	oo_INT (numberOfFormants)
	oo_DOUBLE_ARRAY (formant, 10, numberOfFormants)
	oo_DOUBLE_ARRAY (bandwidth, 10, numberOfFormants)

oo_END_CLASS (FormantPoint)
#undef ooSTRUCT


#define ooSTRUCT FormantTier
oo_DEFINE_CLASS (FormantTier, Function)

	oo_COLLECTION (SortedSetOfDouble, points, FormantPoint, 0)

oo_END_CLASS (FormantTier)
#undef ooSTRUCT


/* End of file FormantTier_def.h */
