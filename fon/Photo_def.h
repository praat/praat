/* Photo_def.h
 *
 * Copyright (C) 2013,2014 Paul Boersma
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


#define ooSTRUCT Photo
oo_DEFINE_CLASS (Photo, SampledXY)

	oo_OBJECT (Matrix, 2, d_red)
	oo_OBJECT (Matrix, 2, d_green)
	oo_OBJECT (Matrix, 2, d_blue)
	oo_OBJECT (Matrix, 2, d_transparency)

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
		// functions:
			void f_init (double xmin, double xmax, long nx, double dx, double x1,
			             double ymin, double ymax, long ny, double dy, double y1);

			double_rgbt f_getValueAtXY (double x, double y);
			/*
				Linear interpolation between matrix points,
				constant extrapolation in cells on the edge,
				NUMundefined outside the union of the unit squares around the points.
			*/

			void f_replaceRed (Matrix red);
			void f_replaceGreen (Matrix green);
			void f_replaceBlue (Matrix blue);
			void f_replaceTransparency (Matrix transparency);

			void f_paintImage (Graphics g, double xmin, double xmax, double ymin, double ymax);

			void f_paintCells (Graphics g, double xmin, double xmax, double ymin, double ymax);
			/*
				Every sample is drawn as a rectangle.
			*/

			void f_movie (Graphics g);
			void f_saveAsPNG               (MelderFile file);
			void f_saveAsTIFF              (MelderFile file);
			void f_saveAsGIF               (MelderFile file);
			void f_saveAsWindowsBitmapFile (MelderFile file);
			void f_saveAsJPEG              (MelderFile file);
			void f_saveAsJPEG2000          (MelderFile file);
			void f_saveAsAppleIconFile     (MelderFile file);
			void f_saveAsWindowsIconFile   (MelderFile file);
		// helpers:
			#if defined (_WIN32)
				void _win_saveAsImageFile (MelderFile file, const wchar_t *which);
			#elif defined (macintosh)
				void _mac_saveAsImageFile (MelderFile file, const void *which);
			#endif
	#endif

oo_END_CLASS (Photo)
#undef ooSTRUCT


/* End of file Matrix_def.h */
