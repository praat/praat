/* FileInMemory_def.h
 *
 * Copyright (C) 2017-2020 David Weenink
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

#define ooSTRUCT FileInMemory
oo_DEFINE_CLASS (FileInMemory, Daata)

	oo_LSTRING (d_path)
	oo_LSTRING (d_id)
	oo_INTEGER (d_numberOfBytes)
	oo_INTEGER (d_position)
	oo_INTEGER (d_errno)
	oo_INT32 (ungetChar)
	#if oo_DESTROYING
		if (! _dontOwnData) {
			oo_BYTEVEC (d_data, d_numberOfBytes + 1)
		}
	#else
		oo_BYTEVEC (d_data, d_numberOfBytes + 1) // final null byte possible
	#endif
	oo_UBYTE (writable)

	#if oo_DECLARING || oo_DESCRIBING
		oo_UBYTE (_dontOwnData)
	#endif

	#if oo_DECLARING
		void v_info () override; 
	#endif
	
oo_END_CLASS (FileInMemory)
#undef ooSTRUCT

/* End of file FileInMemory_def.h */
