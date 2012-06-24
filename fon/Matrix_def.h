/* Matrix_def.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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


#define ooSTRUCT Matrix
oo_DEFINE_CLASS (Matrix, Sampled)

	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_LONG (ny)
	oo_DOUBLE (dy)
	oo_DOUBLE (y1)
	#if oo_READING
		if (Melder_debug == 45)
			Melder_casual ("structMatrix :: read: Going to read %ld rows of %ld columns.", ny, nx);
		if (localVersion >= 2) {
			oo_DOUBLE_MATRIX (z, ny, nx)
		} else {
			oo_FLOAT_MATRIX (z, ny, nx)
		}
	#else
		oo_DOUBLE_MATRIX (z, ny, nx)
	#endif

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
			virtual bool v_hasGetNrow      () { return true; }   virtual double v_getNrow      ()        { return ny; }
			virtual bool v_hasGetNcol      () { return true; }   virtual double v_getNcol      ()        { return nx; }
			virtual bool v_hasGetYmin      () { return true; }   virtual double v_getYmin      ()        { return ymin; }
			virtual bool v_hasGetYmax      () { return true; }   virtual double v_getYmax      ()        { return ymax; }
			virtual bool v_hasGetNy        () { return true; }   virtual double v_getNy        ()        { return ny; }
			virtual bool v_hasGetDy        () { return true; }   virtual double v_getDy        ()        { return dy; }
			virtual bool v_hasGetY         () { return true; }   virtual double v_getY         (long iy) { return y1 + (iy - 1) * dy; }
			virtual bool v_hasGetMatrix    () { return true; }   virtual double v_getMatrix    (long irow, long icol);
			virtual bool v_hasGetFunction2 () { return true; }   virtual double v_getFunction2 (double x, double y);
			virtual double v_getValueAtSample (long isamp, long ilevel, int unit);
	#endif

oo_END_CLASS (Matrix)
#undef ooSTRUCT


/* End of file Matrix_def.h */
