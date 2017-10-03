/* Polygon_def.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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
		if (formatVersion >= 1) {
			oo_DOUBLE_VECTOR (x, numberOfPoints)
			oo_DOUBLE_VECTOR (y, numberOfPoints)
		} else {
			oo_FLOAT_VECTOR (x, numberOfPoints)
			oo_FLOAT_VECTOR (y, numberOfPoints)
		}
	#else
		oo_DOUBLE_VECTOR (x, numberOfPoints)
		oo_DOUBLE_VECTOR (y, numberOfPoints)
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Polygon)
#undef ooSTRUCT


/* End of file Polygon_def.h */
