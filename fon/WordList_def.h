/* File WordList_def.h
 *
 * Copyright (C) 1999-2011 Paul Boersma
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


#define ooSTRUCT WordList
oo_DEFINE_CLASS (WordList, Data)

	#if !oo_DESCRIBING
		oo_LSTRING (string)
	#endif

	#if oo_DECLARING
		oo_LONG (length)
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (WordList)
#undef ooSTRUCT


/* End of file WordList_def.h */
