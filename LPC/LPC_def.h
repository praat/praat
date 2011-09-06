/* LPC_def.h
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


#define ooSTRUCT LPC_Frame
oo_DEFINE_STRUCT (LPC_Frame)

	oo_INT (nCoefficients)
	#if oo_READING_BINARY
		if (localVersion == 0)
		{
			oo_FLOAT_VECTOR (a, nCoefficients)
			oo_FLOAT (gain)
		}
		else
		{
			oo_DOUBLE_VECTOR (a, nCoefficients)
			oo_DOUBLE (gain)
		}
	#else
		oo_DOUBLE_VECTOR (a, nCoefficients)
		oo_DOUBLE (gain)
	#endif

oo_END_STRUCT (LPC_Frame)
#undef ooSTRUCT


#define ooSTRUCT LPC
oo_DEFINE_CLASS (LPC, Sampled)

	/* samplingPeriod */
	oo_DOUBLE (samplingPeriod) /* from Sound */
	oo_INT (maxnCoefficients)
	oo_STRUCT_VECTOR (LPC_Frame, d_frames, nx)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (LPC)
#undef ooSTRUCT


/* End of file LPC_def.h */
