/* Strings_def.h
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2018,2020 Paul Boersma
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


#define ooSTRUCT Strings
oo_DEFINE_CLASS (Strings, Daata)

	oo_INTEGER (numberOfStrings)
	oo_STRING_VECTOR (strings, numberOfStrings)

	#if oo_DECLARING
		void v_info ()
			override;
		void v_assertInvariants () override {
			our structDaata :: v_assertInvariants ();
			Melder_assert (our strings.size == our numberOfStrings);
		}
		void maintainInvariants () {
			our numberOfStrings = our strings.size;
		}
		void checkStringNumber (integer stringNumber) {
			Melder_require (stringNumber >= 1,
				U"The element number should be at least 1, not ", stringNumber, U".");
			Melder_require (stringNumber <= our numberOfStrings,
				U"The element number should be at most the number of elements (", our numberOfStrings, U"), not", stringNumber, U".");
		}
		bool v_hasGetVectorStr ()
			override { return true; }
		conststring32 v_getVectorStr (integer icol)
			override;
	#endif

oo_END_CLASS (Strings)
#undef ooSTRUCT


/* End of file Strings_def.h */
