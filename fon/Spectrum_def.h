/* Spectrum_def.h
 *
 * Copyright (C) 2002 Paul Boersma
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

/*
 * pb 2002/05/22
 * pb 2002/07/16 GPL
 */


#define ooSTRUCT Spectrum
oo_DEFINE_CLASS (Spectrum, Matrix)

	#if oo_READING
		if (localVersion < 1) {
			long i;
			for (i = 1; i <= my nx; i ++)
				my z [2] [i] = - my z [2] [i];
		}
	#endif

oo_END_CLASS (Spectrum)
#undef ooSTRUCT


/* End of file Spectrum_def.h */
