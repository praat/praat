/* ERPTier_def.h
 *
 * Copyright (C) 2011,2014,2015,2017 Paul Boersma
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


#define ooSTRUCT ERPPoint
oo_DEFINE_CLASS (ERPPoint, AnyPoint)

	oo_OBJECT (Sound, 2, erp)

oo_END_CLASS (ERPPoint)
#undef ooSTRUCT


#define ooSTRUCT ERPTier
oo_DEFINE_CLASS (ERPTier, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, ERPPoint, 0)

	oo_INTEGER (numberOfChannels)
	oo_STRING_VECTOR (channelNames, numberOfChannels)

	#if oo_DECLARING
		AnyTier_METHODS

		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (ERPTier)
#undef ooSTRUCT


/* End of file ERPTier_def.h */
