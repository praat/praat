#ifndef _Preferences_h_
#define _Preferences_h_
/* Preferences.h
 *
 * Copyright (C) 1996-2008,2011,2013,2015-2018,2022,2023 Paul Boersma
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

#include "melder.h"

/*
	All strings added with Preferences_addString should have the following buffer size,
	which conveniently equals the size of the path buffer in MelderFile so that
	file paths can be used as preferences.
*/
#define Preferences_STRING_BUFFER_SIZE 1+kMelder_MAXPATH
using PrefsString = char32 [Preferences_STRING_BUFFER_SIZE];

void Preferences_addByte     (conststring32 string /* cattable */, signed char *value, signed char defaultValue);
void Preferences_addShort    (conststring32 string /* cattable */, short *value, short defaultValue);
void Preferences_addInt16    (conststring32 string /* cattable */, int *value, int defaultValue);
void Preferences_addInt      (conststring32 string /* cattable */, int *value, int defaultValue);
void Preferences_addInteger  (conststring32 string /* cattable */, integer *value, integer defaultValue);
void Preferences_addUbyte    (conststring32 string /* cattable */, unsigned char *value, unsigned char defaultValue);
void Preferences_addUshort   (conststring32 string /* cattable */, unsigned short *value, unsigned short defaultValue);
void Preferences_addUint     (conststring32 string /* cattable */, unsigned int *value, unsigned int defaultValue);
void Preferences_addUinteger (conststring32 string /* cattable */, uinteger *value, uinteger defaultValue);
void Preferences_addBool     (conststring32 string /* cattable */, bool *value, bool defaultValue);
void Preferences_addDouble   (conststring32 string /* cattable */, double *value, double defaultValue);
void Preferences_addString   (conststring32 string /* cattable */, char32 *value, conststring32 defaultValue);
void _Preferences_addEnum    (conststring32 string /* cattable */, int *value, int min, int max,
	conststring32 (*getText) (int value), int (*getValue) (conststring32 text), int defaultValue);
#define Preferences_addEnum(string, value, enumerated, defaultValue) \
	_Preferences_addEnum (string, (int *) value, (int) enumerated::MIN, (int) enumerated::MAX, \
	(conststring32 (*) (int)) enumerated##_getText, (enum_generic_getValue) enumerated##_getValue, (int) defaultValue)

void Preferences_read (MelderFile file);
void Preferences_write (MelderFile file);

void Preferences_exit_optimizeByLeaking ();

/*
	Macros for preferences that are included in Things.
*/

#define Prefs_begin(Klas) \
	public: static void f_preferences (); \
	void v1_copyPreferencesToInstance () override;

#define Prefs_addAny_default_(DefaultType,name) \
	private: \
		static DefaultType _classDefault_##name; \
		virtual DefaultType _v_default_##name() const { return our _classDefault_##name; } \
	public: \
		DefaultType default_##name() const { return our _v_default_##name(); }
#define Prefs_overrideAny_default_(DefaultType,name) \
	private: \
		static DefaultType _classDefault_##name; \
		DefaultType _v_default_##name () const override { return our _classDefault_##name; }

#define Pref_copyPlain(from,to) \
	to = from
inline void Pref_copyString (conststring32 from, mutablestring32 to) {
	str32ncpy (to, from, Preferences_STRING_BUFFER_SIZE - 1); \
	to [Preferences_STRING_BUFFER_SIZE - 1] = U'\0';
}

/*
	The use of one _classPref_##name both for classes and for instances prevents the following two declarations in a single class:
		ClassPrefs_addDouble (foo)
		InstancePrefs_addDouble (foo)
	The differential use of _v_classPref1_##name for classes and _v_classPref2_##name for instances prevents the use of
		ClassPrefs_addDouble (foo)
	in a base class and
		InstancePrefs_overrideDouble (foo)
	in a derived class (or the reverse).
 */

#define ClassPrefs_addAny_(StorageType,ArgumentType,DefaultType,name,CopyMethod) \
	Prefs_addAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		virtual StorageType & _v_classPref1_##name() const { return our _classPref_##name; } \
	public: \
		ArgumentType classPref_##name() const { return our _v_classPref1_##name(); } \
		void setClassPref_##name (ArgumentType newValue) { \
			Pref_copy##CopyMethod (newValue, our _v_classPref1_##name()); \
		}
#define ClassPrefs_overrideAny_(StorageType,ArgumentType,DefaultType,name) \
	Prefs_overrideAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		StorageType & _v_classPref1_##name () const override { return our _classPref_##name; }
#define InstancePrefs_addAny_(StorageType,ArgumentType,DefaultType,name,CopyMethod) \
	Prefs_addAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		virtual StorageType & _v_classPref2_##name () const { return our _classPref_##name; } \
		StorageType _instancePref_##name; \
	public: \
		ArgumentType instancePref_##name () const { return our _instancePref_##name; } \
		void setInstancePref_##name (ArgumentType newValue) { \
			Pref_copy##CopyMethod (newValue, our _v_classPref2_##name()); \
			Pref_copy##CopyMethod (newValue, our _instancePref_##name); \
		}
#define InstancePrefs_overrideAny_(StorageType,ArgumentType,DefaultType,name) \
	Prefs_overrideAny_default_ (DefaultType, name) \
	private: \
		static StorageType _classPref_##name; \
		StorageType & _v_classPref2_##name () const override { return our _classPref_##name; }

#define ClassPrefs_addInt(Klas,name,version,default)              ClassPrefs_addAny_         (int, int, conststring32, name, Plain)
#define ClassPrefs_overrideInt(Klas,name,version,default)         ClassPrefs_overrideAny_    (int, int, conststring32, name)
#define InstancePrefs_addInt(Klas,name,version,default)           InstancePrefs_addAny_      (int, int, conststring32, name, Plain)
#define InstancePrefs_overrideInt(Klas,name,version,default)      InstancePrefs_overrideAny_ (int, int, conststring32, name)

#define ClassPrefs_addInteger(Klas,name,version,default)          ClassPrefs_addAny_         (integer, integer, conststring32, name, Plain)
#define ClassPrefs_overrideInteger(Klas,name,version,default)     ClassPrefs_overrideAny_    (integer, integer, conststring32, name)
#define InstancePrefs_addInteger(Klas,name,version,default)       InstancePrefs_addAny_      (integer, integer, conststring32, name, Plain)
#define InstancePrefs_overrideInteger(Klas,name,version,default)  InstancePrefs_overrideAny_ (integer, integer, conststring32, name)

#define ClassPrefs_addDouble(Klas,name,version,default)           ClassPrefs_addAny_         (double, double, conststring32, name, Plain)
#define ClassPrefs_overrideDouble(Klas,name,version,default)      ClassPrefs_overrideAny_    (double, double, conststring32, name)
#define InstancePrefs_addDouble(Klas,name,version,default)        InstancePrefs_addAny_      (double, double, conststring32, name, Plain)
#define InstancePrefs_overrideDouble(Klas,name,version,default)   InstancePrefs_overrideAny_ (double, double, conststring32, name)

#define ClassPrefs_addBool(Klas,name,version,default)             ClassPrefs_addAny_         (bool, bool, bool, name, Plain)
#define ClassPrefs_overrideBool(Klas,name,version,default)        ClassPrefs_overrideAny_    (bool, bool, bool, name)
#define InstancePrefs_addBool(Klas,name,version,default)          InstancePrefs_addAny_      (bool, bool, bool, name, Plain)
#define InstancePrefs_overrideBool(Klas,name,version,default)     InstancePrefs_overrideAny_ (bool, bool, bool, name)

#define ClassPrefs_addEnum(Klas,name,version,kEnumerated,default)          ClassPrefs_addAny_         (enum kEnumerated, enum kEnumerated, enum kEnumerated, name, Plain)
#define ClassPrefs_overrideEnum(Klas,name,version,kEnumerated,default)     ClassPrefs_overrideAny_    (enum kEnumerated, enum kEnumerated, enum kEnumerated, name)
#define InstancePrefs_addEnum(Klas,name,version,kEnumerated,default)       InstancePrefs_addAny_      (enum kEnumerated, enum kEnumerated, enum kEnumerated, name, Plain)
#define InstancePrefs_overrideEnum(Klas,name,version,kEnumerated,default)  InstancePrefs_overrideAny_ (enum kEnumerated, enum kEnumerated, enum kEnumerated, name)

#define ClassPrefs_addString(Klas,name,version,default)             ClassPrefs_addAny_         (PrefsString, conststring32, conststring32, name, String)
#define ClassPrefs_overrideString(Klas,name,version,default)        ClassPrefs_overrideAny_    (PrefsString, conststring32, conststring32, name)
#define InstancePrefs_addString(Klas,name,version,default)          InstancePrefs_addAny_      (PrefsString, conststring32, conststring32, name, String)
#define InstancePrefs_overrideString(Klas,name,version,default)     InstancePrefs_overrideAny_ (PrefsString, conststring32, conststring32, name)

#define Prefs_end(Klas) \
	public:

/* End of file Preferences.h */
#endif
