/* WordList_def.h
 *
 * Copyright (C) 1999-2011,2017 Paul Boersma
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


#define ooSTRUCT WordList
oo_DEFINE_CLASS (WordList, Daata)

	#if !oo_DESCRIBING
		oo_LSTRING (string)
	#endif

	#if oo_DECLARING
		oo_INTEGER (length)

		void v_info ()
			override;
	#endif

oo_END_CLASS (WordList)
#undef ooSTRUCT


/* End of file WordList_def.h */
