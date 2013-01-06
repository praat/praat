/* prefs_define.h
 *
 * Copyright (C) 2013 Paul Boersma
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

/* for C++ files; see prefs.h */

#undef prefs_begin
#undef prefs_add_int
#undef prefs_add_int_with_data
#undef prefs_add_long
#undef prefs_add_long_with_data
#undef prefs_add_bool
#undef prefs_add_bool_with_data
#undef prefs_add_double
#undef prefs_add_double_with_data
#undef prefs_add_enum
#undef prefs_add_enum_with_data
#undef prefs_add_string
#undef prefs_add_string_with_data
#undef prefs_end

#define prefs_begin(Klas)

#define prefs_add_int(Klas,name,version,default) \
	int struct##Klas :: s_##name; \
	const wchar_t * struct##Klas :: sdefault_##name = default;
#define prefs_add_int_with_data(Klas,name,version,default)  prefs_add_int (Klas, name, version, default)

#define prefs_add_long(Klas,name,version,default) \
	long struct##Klas :: s_##name; \
	const wchar_t * struct##Klas :: sdefault_##name = default;
#define prefs_add_long_with_data(Klas,name,version,default)  prefs_add_long (Klas, name, version, default)

#define prefs_add_bool(Klas,name,version,default) \
	bool struct##Klas :: s_##name; \
	bool struct##Klas :: sdefault_##name = default;
#define prefs_add_bool_with_data(Klas,name,version,default)  prefs_add_bool (Klas, name, version, default)

#define prefs_add_double(Klas,name,version,default) \
	double struct##Klas :: s_##name; \
	const wchar_t * struct##Klas :: sdefault_##name = default;
#define prefs_add_double_with_data(Klas,name,version,default)  prefs_add_double (Klas, name, version, default)

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	enum enumerated struct##Klas :: s_##name; \
	enum enumerated struct##Klas :: sdefault_##name = enumerated##_##default;
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  prefs_add_enum (Klas, name, version, enumerated, default)

#define prefs_end(Klas)

#define prefs_add_string(Klas,name,version,default) \
	wchar_t struct##Klas :: s_##name [Preferences_STRING_BUFFER_SIZE]; \
	const wchar_t * struct##Klas :: sdefault_##name = default;
#define prefs_add_string_with_data(Klas,name,version,default)  prefs_add_string (Klas, name, version, default)

/* End of file prefs_define.h */
