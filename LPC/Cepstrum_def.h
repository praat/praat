/* Cepstrum_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
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

/*
 djmw 19981207
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
*/

#define ooSTRUCT Cepstrum_Frame
oo_DEFINE_STRUCT (Cepstrum_Frame)

	oo_INT (nCoefficients)
	#if oo_READING_BINARY
		if (localVersion == 0)
		{
			oo_FLOAT_VECTOR_FROM (c, 0, my nCoefficients)
		}
		else
		{
			oo_DOUBLE_VECTOR_FROM (c, 0, my nCoefficients)
		}
	#else
		oo_DOUBLE_VECTOR_FROM (c, 0, my nCoefficients)
	#endif
	
oo_END_STRUCT (Cepstrum_Frame)
#undef ooSTRUCT

#define ooSTRUCT Cepstrum
oo_DEFINE_CLASS (Cepstrum, Sampled)

	oo_DOUBLE (samplingFrequency) /* from Sound */
	oo_INT (maxnCoefficients)
	oo_STRUCT_VECTOR (Cepstrum_Frame, frame, my nx)

oo_END_CLASS (Cepstrum)
#undef ooSTRUCT

/* End of file Cepstrum_def.h */
