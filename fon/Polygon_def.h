/* Polygon_def.h
 *
 * Copyright (C) 1992-2005,2008,2011,2015-2018 Paul Boersma
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


#define ooSTRUCT Polygon
oo_DEFINE_CLASS (Polygon, Daata)

	oo_INTEGER (numberOfPoints)

	#if oo_READING
		oo_VERSION_UNTIL (1)
			oo_obsoleteVEC32 (x, numberOfPoints)
			oo_obsoleteVEC32 (y, numberOfPoints)
		oo_VERSION_ELSE
			oo_VEC (x, numberOfPoints)
			oo_VEC (y, numberOfPoints)
		oo_VERSION_END
	#else
		oo_VEC (x, numberOfPoints)
		oo_VEC (y, numberOfPoints)
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Polygon)
#undef ooSTRUCT


/* End of file Polygon_def.h */
