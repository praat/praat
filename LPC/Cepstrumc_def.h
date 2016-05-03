/* Cepstrumc_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
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

/*
	djmw 20020812 GPL header
	djmw 20080122 Version 1: float -> double
*/

#define ooSTRUCT Cepstrumc_Frame
oo_DEFINE_STRUCT (Cepstrumc_Frame)

	oo_INT (nCoefficients)

	#if oo_READING_BINARY
		if (formatVersion == 0) {
			oo_FLOAT_VECTOR_FROM (c, 0, nCoefficients)
		} else {
			oo_DOUBLE_VECTOR_FROM (c, 0, nCoefficients)
		}
	#else
		oo_DOUBLE_VECTOR_FROM (c, 0, nCoefficients)
	#endif
	
oo_END_STRUCT (Cepstrumc_Frame)
#undef ooSTRUCT


#define ooSTRUCT Cepstrumc
oo_DEFINE_CLASS (Cepstrumc, Sampled)

	oo_DOUBLE (samplingFrequency)   // from Sound
	oo_INT (maxnCoefficients)
	oo_STRUCT_VECTOR (Cepstrumc_Frame, frame, nx)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Cepstrumc)
#undef ooSTRUCT


/* End of file Cepstrumc_def.h */
