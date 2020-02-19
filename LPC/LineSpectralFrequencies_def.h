/* LineSpectralFrequencies_def.h
 *
 * Copyright (C) 2016-2020 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#define ooSTRUCT LineSpectralFrequencies_Frame
oo_DEFINE_STRUCT (LineSpectralFrequencies_Frame)

	oo_INT (numberOfFrequencies)
	oo_VEC (frequencies, numberOfFrequencies)
	
oo_END_STRUCT (LineSpectralFrequencies_Frame)
#undef ooSTRUCT


#define ooSTRUCT LineSpectralFrequencies
oo_DEFINE_CLASS (LineSpectralFrequencies, Sampled)

	oo_DOUBLE (maximumFrequency)
	oo_INT (maximumNumberOfFrequencies)
	oo_STRUCTVEC (LineSpectralFrequencies_Frame, d_frames, nx)

	#if oo_DECLARING
		void v_info ()
			override;
		conststring32 v_getIndexText () const
			override { return U"frame number"; }
		conststring32 v_getNxText () const
			override { return U"the number of frames"; }
	#endif

oo_END_CLASS (LineSpectralFrequencies)
#undef ooSTRUCT


/* End of file LineSpectralFrequencies_def.h */
