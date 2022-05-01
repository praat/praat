/* EditorPrefs_install.h
 *
 * Copyright (C) 2013,2015-2018,2021,2022 Paul Boersma
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

/* for C++ files; see EditorPrefs.h */

#undef EditorPrefs_begin
#undef prefs_add_int
#undef prefs_add_int_with_data
#undef prefs_override_int
#undef prefs_add_integer
#undef prefs_add_integer_with_data
#undef prefs_override_integer
#undef prefs_add_bool
#undef prefs_add_bool_with_data
#undef prefs_override_bool
#undef EditorClassPrefs_addDouble
#undef EditorClassPrefs_overrideDouble
#undef EditorInstancePrefs_addDouble
#undef EditorInstancePrefs_overrideDouble
#undef prefs_add_enum
#undef prefs_add_enum_with_data
#undef prefs_override_enum
#undef prefs_add_string
#undef prefs_add_string_with_data
#undef prefs_override_string
#undef EditorPrefs_end

#define EditorPrefs_begin(Klas) \
	void struct##Klas :: f_preferences () {

#define prefs_add_int(Klas,name,version,default) \
	Preferences_addInt (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, (int) Melder_atoi (sdefault_##name));
#define prefs_add_int_with_data(Klas,name,version,default)  prefs_add_int (Klas, name, version, default)
#define prefs_override_int(Klas,name,version,default) \
	Preferences_addInt (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, (int) Melder_atoi (sdefault_##name));

#define prefs_add_integer(Klas,name,version,default) \
	Preferences_addInteger (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, Melder_atoi (sdefault_##name));
#define prefs_add_integer_with_data(Klas,name,version,default)  prefs_add_integer (Klas, name, version, default)
#define prefs_override_integer(Klas,name,version,default) \
	Preferences_addInteger (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, Melder_atoi (sdefault_##name));

#define prefs_add_bool(Klas,name,version,default) \
	Preferences_addBool (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, default);
#define prefs_add_bool_with_data(Klas,name,version,default)  prefs_add_bool (Klas, name, version, default)
#define prefs_override_bool(Klas,name,version,default) \
	Preferences_addBool (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, default);

#define EditorClassPrefs_addDouble(Klas,name,version,default) \
	Preferences_addDouble (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _staticClassPref_##name, Melder_atof (sdefault_##name));
#define EditorClassPrefs_overrideDouble(Klas,name,version,default)  EditorClassPrefs_addDouble(Klas,name,version,default)
#define EditorInstancePrefs_addDouble(Klas,name,version,default)  EditorClassPrefs_addDouble(Klas,name,version,default)
#define EditorInstancePrefs_overrideDouble(Klas,name,version,default)  EditorInstancePrefs_addDouble(Klas,name,version,default)

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	Preferences_addEnum (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, enumerated, enumerated :: default);
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  prefs_add_enum (Klas, name, version, enumerated, default)
#define prefs_override_enum(Klas,name,version,enumerated,default) \
	Preferences_addEnum (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name, enumerated, enumerated :: default);

#define prefs_add_string(Klas,name,version,default) \
	Preferences_addString (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name [0], default);
#define prefs_add_string_with_data(Klas,name,version,default)  prefs_add_string (Klas, name, version, default)
#define prefs_override_string(Klas,name,version,default) \
	Preferences_addString (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name [0], default);

#define EditorPrefs_end(Klas) \
	}

/* End of file EditorPrefs_install.h */
