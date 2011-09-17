/* Harmonics_def.h
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


#define ooSTRUCT Harmonics
oo_DEFINE_CLASS (Harmonics, Data)

	oo_LONG (numberOfHarmonics)

	oo_DOUBLE_VECTOR (harmonics, numberOfHarmonics)

	#if oo_DECLARING
		// overridden methods:
		public:
			virtual void v_info ();
	#endif

oo_END_CLASS (Harmonics)
#undef ooSTRUCT


/* End of file Harmonics_def.h */
