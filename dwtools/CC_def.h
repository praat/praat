/* CC_def.h
 * 
 * Copyright (C) 1993-2018 David Weenink
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


#define ooSTRUCT CC_Frame
oo_DEFINE_STRUCT (CC_Frame)

	oo_INTEGER (numberOfCoefficients)
	#if oo_READING_BINARY
		oo_VERSION_UNTIL (1)
			oo_FLOAT (c0)
			oo_obsoleteVEC32 (c, numberOfCoefficients)
		oo_VERSION_ELSE
			oo_DOUBLE (c0)
			oo_VEC (c, numberOfCoefficients)
		oo_VERSION_END
	#else
		oo_DOUBLE (c0)
		oo_VEC (c, numberOfCoefficients)
	#endif

oo_END_STRUCT (CC_Frame)
#undef ooSTRUCT


#define ooSTRUCT CC
oo_DEFINE_CLASS (CC, Sampled)

	oo_DOUBLE (fmin)
	oo_DOUBLE (fmax)
	// c[0]..c[maximumNumberOfCoefficients] ; needed for inverse transform
	oo_INTEGER (maximumNumberOfCoefficients)
	oo_STRUCTVEC (CC_Frame, frame, nx)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (CC)
#undef ooSTRUCT


/* End of file CC_def.h */
