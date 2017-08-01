/* enums_getValue.h
 *
 * Copyright (C) 2007-2009,2015,2017 Paul Boersma
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

/* See enums.h */

/* For C++ files. */

#undef enums_begin
#undef enums_add
#undef enums_alt
#undef enums_end
#define enums_begin(type,minimum)  int type##_getValue (const char32 *testText) {
#define enums_add(type,value,which,text)  if (Melder_equ_firstCharacterCaseInsensitive (testText, text)) return type##_##which;
#define enums_alt(type,which,text)  if (Melder_equ_firstCharacterCaseInsensitive (testText, text)) return type##_##which;
#define enums_end(type,maximum,def) \
	if (str32equ (testText, U"\t")) return type##_DEFAULT; \
	if (str32equ (testText, U"\n")) return maximum; \
	return -1; }

/* End of file enums_getValue.h */
