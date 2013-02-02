/* prefs_install.h
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

#define prefs_begin(Klas) \
	void struct##Klas :: f_preferences () {

#define prefs_add_int(Klas,name,version,default) \
	Preferences_addInt (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name, Melder_atof (sdefault_##name));
#define prefs_add_int_with_data(Klas,name,version,default)  prefs_add_int (Klas, name, version, default)

#define prefs_add_long(Klas,name,version,default) \
	Preferences_addLong (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name, Melder_atof (sdefault_##name));
#define prefs_add_long_with_data(Klas,name,version,default)  prefs_add_long (Klas, name, version, default)

#define prefs_add_bool(Klas,name,version,default) \
	Preferences_addBool (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name, default);
#define prefs_add_bool_with_data(Klas,name,version,default)  prefs_add_bool (Klas, name, version, default)

#define prefs_add_double(Klas,name,version,default) \
	Preferences_addDouble (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name, Melder_atof (sdefault_##name));
#define prefs_add_double_with_data(Klas,name,version,default)  prefs_add_double (Klas, name, version, default)

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	Preferences_addEnum (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name, enumerated, enumerated##_##default);
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  prefs_add_enum (Klas, name, version, enumerated, default)

#define prefs_add_string(Klas,name,version,default) \
	Preferences_addString (Melder_wcscat (L"" #Klas "." #name, version >= 2 ? L"." #version : L""), & s_##name [0], default);
#define prefs_add_string_with_data(Klas,name,version,default)  prefs_add_string (Klas, name, version, default)

#define prefs_end(Klas) \
	}

/* End of file prefs_install.h */
