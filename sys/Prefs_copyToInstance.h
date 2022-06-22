/* Prefs_copyToInstance.h
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

/* for C++ files; see Preferences.h */

#undef  Prefs_begin
#define Prefs_begin(Klas) \
	void struct##Klas :: v1_copyPreferencesToInstance () { \
		Klas##_Parent :: v1_copyPreferencesToInstance ();

#undef  ClassPrefs_addInt
#define ClassPrefs_addInt(Klas,name,version,default)
#undef  ClassPrefs_overrideInt
#define ClassPrefs_overrideInt(Klas,name,version,default)
#undef  InstancePrefs_addInt
#define InstancePrefs_addInt(Klas,name,version,default)  our _instancePref_##name = our _v_classPref2_##name();
#undef  InstancePrefs_overrideInt
#define InstancePrefs_overrideInt(Klas,name,version,default)

#undef  ClassPrefs_addInteger
#define ClassPrefs_addInteger(Klas,name,version,default)
#undef  ClassPrefs_overrideInteger
#define ClassPrefs_overrideInteger(Klas,name,version,default)
#undef  InstancePrefs_addInteger
#define InstancePrefs_addInteger(Klas,name,version,default)  our _instancePref_##name = our _v_classPref2_##name();
#undef  InstancePrefs_overrideInteger
#define InstancePrefs_overrideInteger(Klas,name,version,default)

#undef  ClassPrefs_addDouble
#define ClassPrefs_addDouble(Klas,name,version,default)
#undef  ClassPrefs_overrideDouble
#define ClassPrefs_overrideDouble(Klas,name,version,default)
#undef  InstancePrefs_addDouble
#define InstancePrefs_addDouble(Klas,name,version,default)  our _instancePref_##name = our _v_classPref2_##name();
#undef  InstancePrefs_overrideDouble
#define InstancePrefs_overrideDouble(Klas,name,version,default)

#undef  ClassPrefs_addBool
#define ClassPrefs_addBool(Klas,name,version,default)
#undef  ClassPrefs_overrideBool
#define ClassPrefs_overrideBool(Klas,name,version,default)
#undef  InstancePrefs_addBool
#define InstancePrefs_addBool(Klas,name,version,default)  our _instancePref_##name = our _v_classPref2_##name();
#undef  InstancePrefs_overrideBool
#define InstancePrefs_overrideBool(Klas,name,version,default)

#undef  ClassPrefs_addEnum
#define ClassPrefs_addEnum(Klas,name,version,kEnumerated,default)
#undef  ClassPrefs_overrideEnum
#define ClassPrefs_overrideEnum(Klas,name,version,kEnumerated,default)
#undef  InstancePrefs_addEnum
#define InstancePrefs_addEnum(Klas,name,version,kEnumerated,default)  our _instancePref_##name = our _v_classPref2_##name();
#undef  InstancePrefs_overrideEnum
#define InstancePrefs_overrideEnum(Klas,name,version,kEnumerated,default)

#undef  ClassPrefs_addString
#define ClassPrefs_addString(Klas,name,version,default)
#undef  ClassPrefs_overrideString
#define ClassPrefs_overrideString(Klas,name,version,default)
#undef  InstancePrefs_addString
#define InstancePrefs_addString(Klas,name,version,default)  str32cpy (& our _instancePref_##name [0], our _v_classPref2_##name());
#undef  InstancePrefs_overrideString
#define InstancePrefs_overrideString(Klas,name,version,default)

#undef  Prefs_end
#define Prefs_end(Klas) \
	}

/* End of file Prefs_copyToInstance.h */
