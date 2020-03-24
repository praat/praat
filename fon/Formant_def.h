/* Formant_def.h
 *
 * Copyright (C) 1992-2005,2008,2011,2015-2020 Paul Boersma
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


#define ooSTRUCT Formant_Formant
oo_DEFINE_STRUCT (Formant_Formant)

	#if oo_READING
		oo_VERSION_UNTIL (2)
			oo_FLOAT (frequency)
			oo_FLOAT (bandwidth)
		oo_VERSION_ELSE
			oo_DOUBLE (frequency)
			oo_DOUBLE (bandwidth)
		oo_VERSION_END
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

	oo_INTEGER16 (numberOfFormants)
	oo_STRUCTVEC (Formant_Formant, formant, numberOfFormants)

oo_END_STRUCT (Formant_Frame)
#undef ooSTRUCT



#define ooSTRUCT Formant
oo_DEFINE_CLASS (Formant, Sampled)

	oo_INT16 (maxnFormants)
	oo_STRUCTVEC (Formant_Frame, frames, nx)

	#if oo_DECLARING
		void v_info ()
			override;
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		double v_getValueAtSample (integer iframe, integer which, int units)
			override;
	#endif

oo_END_CLASS (Formant)
#undef ooSTRUCT


/* End of file Formant_def.h */
