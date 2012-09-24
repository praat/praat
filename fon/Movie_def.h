/* Movie_def.h
 *
 * Copyright (C) 2011 Paul Boersma
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


#define ooSTRUCT Movie
oo_DEFINE_CLASS (Movie, Sampled)

	oo_OBJECT (Sound, 2, d_sound)
	oo_STRING (d_folderName)
	oo_OBJECT (Strings, 0, d_fileNames)

	#if oo_DECLARING
		// functions:
			void f_init (Sound sound, const wchar_t *folderName, Strings fileNames);
			void f_paintOneImageInside (Graphics graphics, long frameNumber, double a_xmin, double a_xmax, double a_ymin, double a_ymax);
			void f_paintOneImage (Graphics graphics, long frameNumber, double a_xmin, double a_xmax, double a_ymin, double a_ymax);
			void f_play (Graphics graphics, double tmin, double tmax, int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure);
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (Movie)
#undef ooSTRUCT


/* End of file Movie_def.h */
