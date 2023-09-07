/* CubeGrid_def.h
 *
 * Copyright (C) 2023 Paul Boersma
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


#define ooSTRUCT CubePoint
oo_DEFINE_CLASS (CubePoint, Function)

	oo_DOUBLE (red)
	oo_DOUBLE (green)
	oo_DOUBLE (blue)

oo_END_CLASS (CubePoint)
#undef ooSTRUCT


#define ooSTRUCT CubeTier
oo_DEFINE_CLASS (CubeTier, Function)

	oo_COLLECTION_OF (SortedSetOfDoubleOf, points, CubePoint, 0)

oo_END_CLASS (CubeTier)
#undef ooSTRUCT


#define ooSTRUCT CubeGrid
oo_DEFINE_CLASS (CubeGrid, Function)

	oo_STRING (redName)
	oo_STRING (greenName)
	oo_STRING (blueName)
	oo_COLLECTION_OF (OrderedOf, tiers, CubeTier, 0)

oo_END_CLASS (CubeGrid)
#undef ooSTRUCT


/* End of file CubeGrid_def.h */
