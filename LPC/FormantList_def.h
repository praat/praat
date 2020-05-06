/* FormantList_def.h
 *
 * Copyright (C) 2020 David Weenink
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

#define ooSTRUCT FormantList
oo_DEFINE_CLASS (FormantList, Function)
	oo_INTEGER (numberOfFormantObjects)
	oo_INTEGER (defaultFormantObject)
	oo_COLLECTION_OF (OrderedOf, formants, Formant, 2)
	oo_STRING_VECTOR (identification, numberOfFormantObjects)
	
	#if oo_DECLARING
		virtual void v_info ();
	#endif

oo_END_CLASS (FormantList)
#undef ooSTRUCT

/* End of FormantList_def.h */
