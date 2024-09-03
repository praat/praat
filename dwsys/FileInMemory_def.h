/* FileInMemory_def.h
 *
 * Copyright (C) 2017-2020 David Weenink, 2024 Paul Boersma
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
oo_DEFINE_CLASS (FileInMemory, SimpleString)

	oo_INTEGER (d_numberOfBytes)   // not including any final null byte
	oo_INTEGER (d_position)
	oo_INT16 (d_errno)
	oo_INT16 (d_eof)
	oo_INT32 (ungetChar)

	/*
		The order of the following two statements doesn't matter!
		This is because d_data autodestroys with the FileInMemory *after* this code block.
	*/
	#if oo_DESTROYING
		if (_dontOwnData)
			our d_data.cells = nullptr;   // prevent autodestruction
	#endif
	oo_BYTEVEC (d_data, d_numberOfBytes + 1)   // final null byte possible

	oo_UBYTE (isOpen)

	#if oo_DECLARING || oo_DESCRIBING
		oo_UBYTE (_dontOwnData)
	#endif

	#if oo_DECLARING
		void v1_info ()
			override;
	#endif
	
oo_END_CLASS (FileInMemory)
#undef ooSTRUCT

/* End of file FileInMemory_def.h */
