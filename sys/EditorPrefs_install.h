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

#undef prefs_add_string
#undef prefs_add_string_with_data
#undef prefs_override_string

#undef  EditorPrefs_begin
#define EditorPrefs_begin(Klas) \
	void struct##Klas :: f_preferences () {

#undef  EditorClassPrefs_addInt
#define EditorClassPrefs_addInt(Klas,name,version,default) \
	Preferences_addInt (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _classPref_##name, (int) Melder_atoi (_classDefault_##name));
#undef  EditorClassPrefs_overrideInt
#define EditorClassPrefs_overrideInt(Klas,name,version,default)     EditorClassPrefs_addInt(Klas,name,version,default)
#undef  EditorInstancePrefs_addInt
#define EditorInstancePrefs_addInt(Klas,name,version,default)       EditorClassPrefs_addInt(Klas,name,version,default)
#undef  EditorInstancePrefs_overrideInt
#define EditorInstancePrefs_overrideInt(Klas,name,version,default)  EditorInstancePrefs_addInt(Klas,name,version,default)

#undef  EditorClassPrefs_addInteger
#define EditorClassPrefs_addInteger(Klas,name,version,default) \
	Preferences_addInteger (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _classPref_##name, Melder_atoi (_classDefault_##name));
#undef  EditorClassPrefs_overrideInteger
#define EditorClassPrefs_overrideInteger(Klas,name,version,default)     EditorClassPrefs_addInteger(Klas,name,version,default)
#undef  EditorInstancePrefs_addInteger
#define EditorInstancePrefs_addInteger(Klas,name,version,default)       EditorClassPrefs_addInteger(Klas,name,version,default)
#undef  EditorInstancePrefs_overrideInteger
#define EditorInstancePrefs_overrideInteger(Klas,name,version,default)  EditorInstancePrefs_addInteger(Klas,name,version,default)

#undef  EditorClassPrefs_addDouble
#define EditorClassPrefs_addDouble(Klas,name,version,default) \
	Preferences_addDouble (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _classPref_##name, Melder_atof (_classDefault_##name));
#undef  EditorClassPrefs_overrideDouble
#define EditorClassPrefs_overrideDouble(Klas,name,version,default)     EditorClassPrefs_addDouble(Klas,name,version,default)
#undef  EditorInstancePrefs_addDouble
#define EditorInstancePrefs_addDouble(Klas,name,version,default)       EditorClassPrefs_addDouble(Klas,name,version,default)
#undef  EditorInstancePrefs_overrideDouble
#define EditorInstancePrefs_overrideDouble(Klas,name,version,default)  EditorInstancePrefs_addDouble(Klas,name,version,default)

#undef  EditorClassPrefs_addBool
#define EditorClassPrefs_addBool(Klas,name,version,default) \
	Preferences_addBool (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _classPref_##name, default);
#undef  EditorClassPrefs_overrideBool
#define EditorClassPrefs_overrideBool(Klas,name,version,default)     EditorClassPrefs_addBool(Klas,name,version,default)
#undef  EditorInstancePrefs_addBool
#define EditorInstancePrefs_addBool(Klas,name,version,default)       EditorClassPrefs_addBool(Klas,name,version,default)
#undef  EditorInstancePrefs_overrideBool
#define EditorInstancePrefs_overrideBool(Klas,name,version,default)  EditorInstancePrefs_addBool(Klas,name,version,default)

#undef  EditorClassPrefs_addEnum
#define EditorClassPrefs_addEnum(Klas,name,version,kEnumerated,default) \
	Preferences_addEnum (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & _classPref_##name, kEnumerated, kEnumerated :: default);
#undef  EditorClassPrefs_overrideEnum
#define EditorClassPrefs_overrideEnum(Klas,name,version,kEnumerated,default)     EditorClassPrefs_addEnum(Klas,name,version,kEnumerated,default)
#undef  EditorInstancePrefs_addEnum
#define EditorInstancePrefs_addEnum(Klas,name,version,kEnumerated,default)       EditorClassPrefs_addEnum(Klas,name,version,kEnumerated,default)
#undef  EditorInstancePrefs_overrideEnum
#define EditorInstancePrefs_overrideEnum(Klas,name,version,kEnumerated,default)  EditorInstancePrefs_addEnum(Klas,name,version,kEnumerated,default)

#define prefs_add_string(Klas,name,version,default) \
	Preferences_addString (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name [0], default);
#define prefs_add_string_with_data(Klas,name,version,default)  prefs_add_string (Klas, name, version, default)
#define prefs_override_string(Klas,name,version,default) \
	Preferences_addString (Melder_cat (U"" #Klas U"." #name, version >= 2 ? U"." #version : U""), & s_##name [0], default);

#undef  EditorPrefs_end
#define EditorPrefs_end(Klas) \
	}

/* End of file EditorPrefs_install.h */
