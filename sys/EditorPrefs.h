#ifndef _EditorPrefs_h_
#define _EditorPrefs_h_
/* EditorPrefs.h
 *
 * Copyright (C) 2013,2015-2018,2022 Paul Boersma
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

#include "Preferences.h"

#define EditorPrefs_begin(Klas) \
	public: static void f_preferences (); \
	void v_copyPreferencesToInstance () override;

#define prefs_add_int(Klas,name,version,default) \
	private: static int s_##name; public: virtual int & pref_##name () { return s_##name; } \
	private: static conststring32 sdefault_##name; public: virtual conststring32 default_##name () { return sdefault_##name; }
#define prefs_add_int_with_data(Klas,name,version,default)  public: int p_##name; prefs_add_int (Klas, name, version, default)
#define prefs_override_int(Klas,name,version,default) \
	private: static int s_##name; public: int & pref_##name () override { return s_##name; } \
	private: static conststring32 sdefault_##name; public: conststring32 default_##name () override { return sdefault_##name; }

#define prefs_add_integer(Klas,name,version,default) \
	private: static integer s_##name; public: virtual integer & pref_##name () { return s_##name; } \
	private: static conststring32 sdefault_##name; public: virtual conststring32 default_##name () { return sdefault_##name; }
#define prefs_add_integer_with_data(Klas,name,version,default)  public: integer p_##name; prefs_add_integer (Klas, name, version, default)
#define prefs_override_integer(Klas,name,version,default) \
	private: static integer s_##name; public: integer & pref_##name () override { return s_##name; } \
	private: static conststring32 sdefault_##name; public: conststring32 default_##name () override { return sdefault_##name; }

#define prefs_add_bool(Klas,name,version,default) \
	private: static bool s_##name; public: virtual bool & pref_##name () { return s_##name; } \
	private: static bool sdefault_##name; public: virtual bool default_##name () { return sdefault_##name; }
#define prefs_add_bool_with_data(Klas,name,version,default)  public: bool p_##name; prefs_add_bool (Klas, name, version, default)
#define prefs_override_bool(Klas,name,version,default) \
	private: static bool s_##name; public: bool & pref_##name () override { return s_##name; } \
	private: static bool sdefault_##name; public: bool default_##name () override { return sdefault_##name; }

#define EditorClassPrefs_addDouble(Klas,name,version,default) \
	private: static double _staticClassPref_##name; private: virtual double & _dynamicClassPref1_##name () { return _staticClassPref_##name; } \
	private: static conststring32 sdefault_##name; public: virtual conststring32 default_##name () { return sdefault_##name; } \
	public: double classPref_##name () { return our _dynamicClassPref1_##name(); } \
	public: void setClassPref_##name (double newValue) { our _dynamicClassPref1_##name() = newValue; }
#define EditorClassPrefs_overrideDouble(Klas,name,version,default) \
	private: static double _staticClassPref_##name; public: double & _dynamicClassPref1_##name () override { return _staticClassPref_##name; } \
	private: static conststring32 sdefault_##name; public: conststring32 default_##name () override { return sdefault_##name; }
#define EditorInstancePrefs_addDouble(Klas,name,version,default) \
	private: double _instancePref_##name; \
	private: static double _staticClassPref_##name; private: virtual double & _dynamicClassPref2_##name () { return _staticClassPref_##name; } \
	private: static conststring32 sdefault_##name; public: virtual conststring32 default_##name () { return sdefault_##name; } \
	public: double instancePref_##name () { return our _instancePref_##name; } \
	public: void setInstancePref_##name (double newValue) { our _dynamicClassPref2_##name() = our _instancePref_##name = newValue; } \
	private: void _copyPrefToInstance_##name () { our _instancePref_##name = our _dynamicClassPref2_##name(); }
#define EditorInstancePrefs_overrideDouble(Klas,name,version,default) \
	private: static double _staticClassPref_##name; public: double & _dynamicClassPref2_##name () override { return _staticClassPref_##name; } \
	private: static conststring32 sdefault_##name; public: conststring32 default_##name () override { return sdefault_##name; }

#define prefs_add_enum(Klas,name,version,enumerated,default) \
	private: static enum enumerated s_##name; public: virtual enum enumerated & pref_##name () { return s_##name; } \
	private: static enum enumerated sdefault_##name; public: virtual enum enumerated default_##name () { return sdefault_##name; }
#define prefs_add_enum_with_data(Klas,name,version,enumerated,default)  public: enumerated p_##name; prefs_add_enum (Klas, name, version, enumerated, default)
#define prefs_override_enum(Klas,name,version,enumerated,default) \
	private: static enum enumerated s_##name; public: enum enumerated & pref_##name () override { return s_##name; } \
	private: static enum enumerated sdefault_##name; public: enum enumerated default_##name () override { return sdefault_##name; }

#define prefs_add_string(Klas,name,version,default) \
	private: static char32 s_##name [Preferences_STRING_BUFFER_SIZE]; public: virtual char32 * pref_##name () { return & s_##name [0]; } \
	private: static conststring32 sdefault_##name; public: virtual conststring32 default_##name () { return sdefault_##name; }
#define prefs_add_string_with_data(Klas,name,version,default)  public: char32 p_##name [Preferences_STRING_BUFFER_SIZE]; prefs_add_string (Klas, name, version, default)
#define prefs_override_string(Klas,name,version,default) \
	private: static char32 s_##name [Preferences_STRING_BUFFER_SIZE]; public: char32 * pref_##name () override { return & s_##name [0]; } \
	private: static conststring32 sdefault_##name; public: conststring32 default_##name () override{ return sdefault_##name; }

#define EditorPrefs_end(Klas) \
	public:

/* End of file EditorPrefs.h */
#endif
