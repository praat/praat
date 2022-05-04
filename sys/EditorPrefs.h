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

#define prefs_add_bool(Klas,name,version,default) \
	private: static bool s_##name; public: virtual bool & pref_##name () { return s_##name; } \
	private: static bool sdefault_##name; public: virtual bool default_##name () { return sdefault_##name; }
#define prefs_add_bool_with_data(Klas,name,version,default)  public: bool p_##name; prefs_add_bool (Klas, name, version, default)
#define prefs_override_bool(Klas,name,version,default) \
	private: static bool s_##name; public: bool & pref_##name () override { return s_##name; } \
	private: static bool sdefault_##name; public: bool default_##name () override { return sdefault_##name; }

/*
	The use of one _classPref_##name both for classes and for instances prevents the following two declarations in a single class:
		EditorClassPrefs_addDouble (foo)
		EditorInstancePrefs_addDouble (foo)
	The differential use of _v_classPref1_##name for classes and _v_classPref2_##name for instances prevents the use of
		EditorClassPrefs_addDouble (foo)
	in a base class and
		EditorInstancePrefs_overrideDouble (foo)
	in a derived class (or the reverse).
 */

#define EditorClassPrefs_addAny(Type,name) \
	private: \
		static conststring32 _classDefault_##name; \
		virtual conststring32 _v_default_##name() { return our _classDefault_##name; } \
		static Type _classPref_##name; \
		virtual Type & _v_classPref1_##name() { return our _classPref_##name; } \
	public: \
		virtual conststring32 default_##name() { return our _v_default_##name(); } \
		Type classPref_##name() { return our _v_classPref1_##name(); } \
		void setClassPref_##name (Type newValue) { our _v_classPref1_##name() = newValue; }
#define EditorClassPrefs_overrideAny(Type,name) \
	private: \
		static conststring32 _classDefault_##name; \
		conststring32 _v_default_##name () override { return our _classDefault_##name; } \
		static Type _classPref_##name; \
		Type & _v_classPref1_##name () override { return our _classPref_##name; }
#define EditorInstancePrefs_addAny(Type,name) \
	private: \
		static conststring32 _classDefault_##name; \
		virtual conststring32 _v_default_##name () { return our _classDefault_##name; } \
		static Type _classPref_##name; \
		virtual Type & _v_classPref2_##name () { return our _classPref_##name; } \
		Type _instancePref_##name; \
		void _copyPrefToInstance_##name () { our _instancePref_##name = our _v_classPref2_##name(); } \
	public: \
		virtual conststring32 default_##name() { return our _v_default_##name(); } \
		Type instancePref_##name () { return our _instancePref_##name; } \
		void setInstancePref_##name (Type newValue) { our _v_classPref2_##name() = our _instancePref_##name = newValue; }
#define EditorInstancePrefs_overrideAny(Type,name) \
	private: \
		static conststring32 _classDefault_##name; \
		conststring32 _v_default_##name () override { return our _classDefault_##name; } \
		static Type _classPref_##name; \
		Type & _v_classPref2_##name () override { return our _classPref_##name; }

#define EditorClassPrefs_addInt(Klas,name,version,default)  EditorClassPrefs_addAny (int, name)
#define EditorClassPrefs_overrideInt(Klas,name,version,default)  EditorClassPrefs_overrideAny (int, name)
#define EditorInstancePrefs_addInt(Klas,name,version,default)  EditorInstancePrefs_addAny (int, name)
#define EditorInstancePrefs_overrideInt(Klas,name,version,default)  EditorInstancePrefs_overrideAny (int, name)

#define EditorClassPrefs_addInteger(Klas,name,version,default)  EditorClassPrefs_addAny (integer, name)
#define EditorClassPrefs_overrideInteger(Klas,name,version,default)  EditorClassPrefs_overrideAny (integer, name)
#define EditorInstancePrefs_addInteger(Klas,name,version,default)  EditorInstancePrefs_addAny (integer, name)
#define EditorInstancePrefs_overrideInteger(Klas,name,version,default)  EditorInstancePrefs_overrideAny (integer, name)

#define EditorClassPrefs_addBool(Klas,name,version,default)  EditorClassPrefs_addAny (bool, name)
#define EditorClassPrefs_overrideBool(Klas,name,version,default)  EditorClassPrefs_overrideAny (bool, name)
#define EditorInstancePrefs_addBool(Klas,name,version,default)  EditorInstancePrefs_addAny (bool, name)
#define EditorInstancePrefs_overrideBool(Klas,name,version,default)  EditorInstancePrefs_overrideAny (bool, name)

#define EditorClassPrefs_addDouble(Klas,name,version,default)  EditorClassPrefs_addAny (double, name)
#define EditorClassPrefs_overrideDouble(Klas,name,version,default)  EditorClassPrefs_overrideAny (double, name)
#define EditorInstancePrefs_addDouble(Klas,name,version,default)  EditorInstancePrefs_addAny (double, name)
#define EditorInstancePrefs_overrideDouble(Klas,name,version,default)  EditorInstancePrefs_overrideAny (double, name)

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
