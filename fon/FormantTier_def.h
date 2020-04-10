/* FormantTier_def.h
 *
 * Copyright (C) 1992-2006,2011,2015-2018,2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */


#define ooSTRUCT FormantPoint
oo_DEFINE_CLASS (FormantPoint, AnyPoint)

	oo_INTEGER16 (numberOfFormants)
	oo_VEC (formant, numberOfFormants)
	oo_VEC (bandwidth, numberOfFormants)

oo_END_CLASS (FormantPoint)
#undef ooSTRUCT


#define ooSTRUCT FormantTier
oo_DEFINE_CLASS (FormantTier, Function)   // a kind of AnyTier though

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, FormantPoint, 0)

	#if oo_DECLARING
		AnyTier_METHODS
	#endif

oo_END_CLASS (FormantTier)
#undef ooSTRUCT


/* End of file FormantTier_def.h */
