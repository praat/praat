/* Matrix_def.h
 *
 * Copyright (C) 1992-2005,2008,2011-2013,2015-2018 Paul Boersma
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


#define ooSTRUCT Matrix
oo_DEFINE_CLASS (Matrix, SampledXY)

	#if oo_READING
		if (Melder_debug == 45)
			Melder_casual (U"structMatrix :: read:"
				U" Going to read ", our ny, U" rows"
				U" of ", our nx, U" columns.");
		oo_VERSION_UNTIL (2)
			oo_obsoleteMAT32 (z, ny, nx)
		oo_VERSION_ELSE
			oo_MAT (z, ny, nx)
		oo_VERSION_END
	#else
		oo_MAT (z, ny, nx)
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
		bool v_hasGetNrow ()
			override { return true; }
		double v_getNrow ()
			override { return ny; }
		bool v_hasGetNcol ()
			override { return true; }
		double v_getNcol ()
			override { return nx; }
		bool v_hasGetMatrix ()
			override { return true; }
		double v_getMatrix (integer irow, integer icol)
			override;
		bool v_hasGetFunction2 ()
			override { return true; }
		double v_getFunction2 (double x, double y)
			override;
		double v_getValueAtSample (integer sampleNumber, integer level, int unit)
			override;
	#endif

oo_END_CLASS (Matrix)
#undef ooSTRUCT


/* End of file Matrix_def.h */
