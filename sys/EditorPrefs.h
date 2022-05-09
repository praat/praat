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

#define EditorPrefs_addAny_default_(DefaultType,name) \
	private: \
		static DefaultType _classDefault_##name; \
		virtual DefaultType _v_default_##name() { return our _classDefault_##name; } \
	public: \
		DefaultType default_##name() { return our _v_default_##name(); }
#define EditorPrefs_overrideAny_default_(DefaultType,name) \
	private: \
		static DefaultType _classDefault_##name; \
		DefaultType _v_default_##name () override { return our _classDefault_##name; }

using EditorPrefsString = char32 [Preferences_STRING_BUFFER_SIZE];
#define EditorPref_copyPlain(from,to) \
	to = from;
#define EditorPref_copyString(from,to) \
	str32ncpy (to, from, Preferences_STRING_BUFFER_SIZE); \
	to [Preferences_STRING_BUFFER_SIZE - 1] = U'\0';

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

#define EditorClassPrefs_addAny_(StorageType,ArgumentType,DefaultType,name,CopyMethod) \
	EditorPrefs_addAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		virtual StorageType & _v_classPref1_##name() { return our _classPref_##name; } \
	public: \
		ArgumentType classPref_##name() { return our _v_classPref1_##name(); } \
		void setClassPref_##name (ArgumentType newValue) { \
			EditorPref_copy##CopyMethod (newValue, our _v_classPref1_##name()) \
		}
#define EditorClassPrefs_overrideAny_(StorageType,ArgumentType,DefaultType,name) \
	EditorPrefs_overrideAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		StorageType & _v_classPref1_##name () override { return our _classPref_##name; }
#define EditorInstancePrefs_addAny_(StorageType,ArgumentType,DefaultType,name,CopyMethod) \
	EditorPrefs_addAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		virtual StorageType & _v_classPref2_##name () { return our _classPref_##name; } \
		StorageType _instancePref_##name; \
	public: \
		ArgumentType instancePref_##name () { return our _instancePref_##name; } \
		void setInstancePref_##name (ArgumentType newValue) { \
			EditorPref_copy##CopyMethod (newValue, our _v_classPref2_##name()) \
			EditorPref_copy##CopyMethod (newValue, our _instancePref_##name) \
		}
#define EditorInstancePrefs_overrideAny_(StorageType,ArgumentType,DefaultType,name) \
	EditorPrefs_overrideAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		StorageType & _v_classPref2_##name () override { return our _classPref_##name; }

#define EditorClassPrefs_addInt(Klas,name,version,default)              EditorClassPrefs_addAny_         (int, int, conststring32, name, Plain)
#define EditorClassPrefs_overrideInt(Klas,name,version,default)         EditorClassPrefs_overrideAny_    (int, int, conststring32, name)
#define EditorInstancePrefs_addInt(Klas,name,version,default)           EditorInstancePrefs_addAny_      (int, int, conststring32, name, Plain)
#define EditorInstancePrefs_overrideInt(Klas,name,version,default)      EditorInstancePrefs_overrideAny_ (int, int, conststring32, name)

#define EditorClassPrefs_addInteger(Klas,name,version,default)          EditorClassPrefs_addAny_         (integer, integer, conststring32, name, Plain)
#define EditorClassPrefs_overrideInteger(Klas,name,version,default)     EditorClassPrefs_overrideAny_    (integer, integer, conststring32, name)
#define EditorInstancePrefs_addInteger(Klas,name,version,default)       EditorInstancePrefs_addAny_      (integer, integer, conststring32, name, Plain)
#define EditorInstancePrefs_overrideInteger(Klas,name,version,default)  EditorInstancePrefs_overrideAny_ (integer, integer, conststring32, name)

#define EditorClassPrefs_addDouble(Klas,name,version,default)           EditorClassPrefs_addAny_         (double, double, conststring32, name, Plain)
#define EditorClassPrefs_overrideDouble(Klas,name,version,default)      EditorClassPrefs_overrideAny_    (double, double, conststring32, name)
#define EditorInstancePrefs_addDouble(Klas,name,version,default)        EditorInstancePrefs_addAny_      (double, double, conststring32, name, Plain)
#define EditorInstancePrefs_overrideDouble(Klas,name,version,default)   EditorInstancePrefs_overrideAny_ (double, double, conststring32, name)

#define EditorClassPrefs_addBool(Klas,name,version,default)             EditorClassPrefs_addAny_         (bool, bool, bool, name, Plain)
#define EditorClassPrefs_overrideBool(Klas,name,version,default)        EditorClassPrefs_overrideAny_    (bool, bool, bool, name)
#define EditorInstancePrefs_addBool(Klas,name,version,default)          EditorInstancePrefs_addAny_      (bool, bool, bool, name, Plain)
#define EditorInstancePrefs_overrideBool(Klas,name,version,default)     EditorInstancePrefs_overrideAny_ (bool, bool, bool, name)

#define EditorClassPrefs_addEnum(Klas,name,version,kEnumerated,default)          EditorClassPrefs_addAny_         (enum kEnumerated, enum kEnumerated, enum kEnumerated, name, Plain)
#define EditorClassPrefs_overrideEnum(Klas,name,version,kEnumerated,default)     EditorClassPrefs_overrideAny_    (enum kEnumerated, enum kEnumerated, enum kEnumerated, name)
#define EditorInstancePrefs_addEnum(Klas,name,version,kEnumerated,default)       EditorInstancePrefs_addAny_      (enum kEnumerated, enum kEnumerated, enum kEnumerated, name, Plain)
#define EditorInstancePrefs_overrideEnum(Klas,name,version,kEnumerated,default)  EditorInstancePrefs_overrideAny_ (enum kEnumerated, enum kEnumerated, enum kEnumerated, name)

#define EditorClassPrefs_addString(Klas,name,version,default)             EditorClassPrefs_addAny_         (EditorPrefsString, conststring32, conststring32, name, String)
#define EditorClassPrefs_overrideString(Klas,name,version,default)        EditorClassPrefs_overrideAny_    (EditorPrefsString, conststring32, conststring32, name)
#define EditorInstancePrefs_addString(Klas,name,version,default)          EditorInstancePrefs_addAny_      (EditorPrefsString, conststring32, conststring32, name, String)
#define EditorInstancePrefs_overrideString(Klas,name,version,default)     EditorInstancePrefs_overrideAny_ (EditorPrefsString, conststring32, conststring32, name)

#define EditorPrefs_end(Klas) \
	public:

/* End of file EditorPrefs.h */
#endif
