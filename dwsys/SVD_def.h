/* SVD_def.h
 *
 * Copyright (C) 1994-2018 David Weenink
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

#define ooSTRUCT SVD
oo_DEFINE_CLASS (SVD, Daata)

	oo_DOUBLE (tolerance)
	oo_INTEGER (numberOfRows)
	oo_INTEGER (numberOfColumns) // new invariant: numberOfRows >= numberOfColumns!
	oo_FROM (1)
		oo_QUESTION (isTransposed)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			if (our numberOfRows < our numberOfColumns) {
				integer tmp = our numberOfRows;
				our numberOfRows = our numberOfColumns;
				our numberOfColumns = tmp;
				our isTransposed = true;
				oo_MAT (v, numberOfColumns, numberOfColumns)
				oo_MAT (u, numberOfRows, numberOfColumns)
			} else {
				our isTransposed = false;
				oo_MAT (u, numberOfRows, numberOfColumns)
				oo_MAT (v, numberOfColumns, numberOfColumns)
			}
		oo_VERSION_ELSE
			oo_MAT (u, numberOfRows, numberOfColumns)
			oo_MAT (v, numberOfColumns, numberOfColumns)
		oo_VERSION_END
	#else
		oo_MAT (u, numberOfRows, numberOfColumns)
		oo_MAT (v, numberOfColumns, numberOfColumns)
	#endif
	oo_VEC (d, numberOfColumns)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (SVD)
#undef ooSTRUCT


#define ooSTRUCT GSVD
oo_DEFINE_CLASS (GSVD, Daata)

	oo_DOUBLE (tolerance)
	oo_INTEGER (numberOfColumns)
	oo_MAT ( q, numberOfColumns, numberOfColumns)
	oo_MAT ( r, numberOfColumns, numberOfColumns)
	oo_VEC (d1, numberOfColumns)
	oo_VEC (d2, numberOfColumns)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (GSVD)
#undef ooSTRUCT


/* End of file SVD_def.h */
