/* LPC_def.h
 *
 * Copyright (C) 1994-2018 David Weenink
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


#define ooSTRUCT LPC_Frame
oo_DEFINE_STRUCT (LPC_Frame)

	oo_INT (nCoefficients)
	#if oo_READING_BINARY
		oo_VERSION_UNTIL (1)
			oo_obsoleteVEC32 (a, nCoefficients)
			oo_FLOAT (gain)
		oo_VERSION_ELSE
			oo_VEC (a, nCoefficients)
			oo_DOUBLE (gain)
		oo_VERSION_END
	#else
		oo_VEC (a, nCoefficients)
		oo_DOUBLE (gain)
	#endif

oo_END_STRUCT (LPC_Frame)
#undef ooSTRUCT


#define ooSTRUCT LPC
oo_DEFINE_CLASS (LPC, Sampled)

	oo_DOUBLE (samplingPeriod)   // from Sound
	oo_INT (maxnCoefficients)
	oo_STRUCTVEC (LPC_Frame, d_frames, nx)

	#if oo_DECLARING
		void v_info ()
			override;
		conststring32 v_getIndexText () const
			override { return U"frame number"; }
		conststring32 v_getNxText () const
			override { return U"the number of frames"; }
	#endif

oo_END_CLASS (LPC)
#undef ooSTRUCT


/* End of file LPC_def.h */
