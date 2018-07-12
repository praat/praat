/* prefs_define.h
 *
 * Copyright (C) 2013,2015-2018 Paul Boersma
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

/* for C++ files; see prefs.h */

#undef prefs_begin
#undef prefs_add_int
#undef prefs_add_int_with_data
#undef prefs_override_int
#undef prefs_add_integer
#undef prefs_add_integer_with_data
#undef prefs_override_integer
#undef prefs_add_bool
#undef prefs_add_bool_with_data
#undef prefs_override_bool
#undef prefs_add_double
#undef prefs_add_double_with_data
#undef prefs_override_double
#undef prefs_add_enum
#undef prefs_add_enum_with_data
#undef prefs_override_enum
#undef prefs_add_string
#undef prefs_add_string_with_data
#undef prefs_override_string
#undef prefs_end

#define prefs_begin(Klas)

#define prefs_add_int(Klas,name,version,default) \
	int struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;
#define prefs_add_int_with_data(Klas,name,version,default)  prefs_add_int (Klas, name, version, default)
#define prefs_override_int(Klas,name,version,default) \
	int struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;

#define prefs_add_integer(Klas,name,version,default) \
	integer struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;
#define prefs_add_integer_with_data(Klas,name,version,default)  prefs_add_integer (Klas, name, version, default)
#define prefs_override_integer(Klas,name,version,default) \
	integer struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;

#define prefs_add_bool(Klas,name,version,default) \
	bool struct##Klas :: s_##name; \
	bool struct##Klas :: sdefault_##name = default;
#define prefs_add_bool_with_data(Klas,name,version,default)  prefs_add_bool (Klas, name, version, default)
#define prefs_override_bool(Klas,name,version,default) \
	bool struct##Klas :: s_##name; \
	bool struct##Klas :: sdefault_##name = default;

#define prefs_add_double(Klas,name,version,default) \
	double struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;
#define prefs_add_double_with_data(Klas,name,version,default)  prefs_add_double (Klas, name, version, default)
#define prefs_override_double(Klas,name,version,default) \
	double struct##Klas :: s_##name; \
	conststring32 struct##Klas :: sdefault_##name = default;

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	enum enumerated struct##Klas :: s_##name; \
	enum enumerated struct##Klas :: sdefault_##name = enumerated :: default;
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  prefs_add_enum (Klas, name, version, enumerated, default)
#define prefs_override_enum(Klas,name,version,enumerated,default) \
	enum enumerated struct##Klas :: s_##name; \
	enum enumerated struct##Klas :: sdefault_##name = enumerated :: default;

#define prefs_add_string(Klas,name,version,default) \
	char32 struct##Klas :: s_##name [Preferences_STRING_BUFFER_SIZE]; \
	conststring32 struct##Klas :: sdefault_##name = default;
#define prefs_add_string_with_data(Klas,name,version,default)  prefs_add_string (Klas, name, version, default)
#define prefs_override_string(Klas,name,version,default) \
	char32 struct##Klas :: s_##name [Preferences_STRING_BUFFER_SIZE]; \
	conststring32 struct##Klas :: sdefault_##name = default;

#define prefs_end(Klas)

/* End of file prefs_define.h */
