/* NoulliGrid_def.h
 *
 * Copyright (C) 2018 Paul Boersma
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


#define ooSTRUCT NoulliPoint
oo_DEFINE_CLASS (NoulliPoint, Function)

	oo_INTEGER (numberOfCategories)
	oo_VEC (probabilities, numberOfCategories)

oo_END_CLASS (NoulliPoint)
#undef ooSTRUCT


#define ooSTRUCT NoulliTier
oo_DEFINE_CLASS (NoulliTier, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, NoulliPoint, 0)

oo_END_CLASS (NoulliTier)
#undef ooSTRUCT


#define ooSTRUCT NoulliGrid
oo_DEFINE_CLASS (NoulliGrid, Function)

	oo_INTEGER (numberOfCategories)
	oo_STRING_VECTOR (categoryNames, numberOfCategories)
	oo_COLLECTION_OF (OrderedOf, tiers, NoulliTier, 0)

oo_END_CLASS (NoulliGrid)
#undef ooSTRUCT


/* End of file NoulliGrid_def.h */
