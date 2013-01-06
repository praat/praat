/* enums_getText.h
 *
 * Copyright (C) 2007 Paul Boersma
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

/* See enums.h */

/* For C++ files. */

#undef enums_begin
#undef enums_add
#undef enums_alt
#undef enums_end
#define enums_begin(type,minimum)  const wchar_t * type##_getText (int testValue) { return
#define enums_add(type,value,which,text)  testValue == type##_##which ? text :
#define enums_alt(type,which,text)
#define enums_end(type,maximum,def) type##_getText (type##_DEFAULT); }

/* End of file enums_getText.h */
