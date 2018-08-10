/* enums_getText.h
 *
 * Copyright (C) 2007,2015,2017 Paul Boersma
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
#define enums_begin(kType,minimum)  conststring32 kType##_getText (kType testValue) { return
#define enums_add(kType,value,which,text)  testValue == kType::which ? text :
#define enums_alt(kType,which,text)
#define enums_end(kType,maximum,def) kType##_getText (kType::DEFAULT); }

/* End of file enums_getText.h */
