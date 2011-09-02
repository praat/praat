/* Formant_def.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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


#define ooSTRUCT Formant_Formant
oo_DEFINE_STRUCT (Formant_Formant)

	#if oo_READING
		if (localVersion <= 1) {
			oo_FLOAT (frequency)
			oo_FLOAT (bandwidth)
		} else {
			oo_DOUBLE (frequency)
			oo_DOUBLE (bandwidth)
		}
	#else
		oo_DOUBLE (frequency)
		oo_DOUBLE (bandwidth)
	#endif

oo_END_STRUCT (Formant_Formant)
#undef ooSTRUCT



#define ooSTRUCT Formant_Frame
oo_DEFINE_STRUCT (Formant_Frame)

	oo_FROM (1)
		oo_DOUBLE (intensity)
	oo_ENDFROM

	oo_INT (nFormants)
	oo_STRUCT_VECTOR (Formant_Formant, formant, nFormants)

oo_END_STRUCT (Formant_Frame)
#undef ooSTRUCT



#define ooSTRUCT Formant
oo_DEFINE_CLASS (Formant, Sampled)

	oo_INT (maxnFormants)
	oo_STRUCT_VECTOR (Formant_Frame, d_frames, nx)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
			virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
			virtual double v_getValueAtSample (long iframe, long which, int units);
	#endif

oo_END_CLASS (Formant)
#undef ooSTRUCT


/* End of file Formant_def.h */
