#ifndef _Preferences_h_
#define _Preferences_h_
/* Preferences.h
 *
 * Copyright (C) 1996-2008,2011,2013,2015-2018 Paul Boersma
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
 * All strings added with Preferences_addString should have the following buffer size,
 * which conveniently equals the size of the path buffer in MelderFile so that
 * file paths can be used as preferences.
 */
#define Preferences_STRING_BUFFER_SIZE 1+kMelder_MAXPATH

#define pref_str32cpy(to, from) \
	str32ncpy (to, from, Preferences_STRING_BUFFER_SIZE); \
	to [Preferences_STRING_BUFFER_SIZE - 1] = U'\0';

#define pref_str32cpy2(to2, to1, from) \
	str32ncpy (to1, from, Preferences_STRING_BUFFER_SIZE); \
	to1 [Preferences_STRING_BUFFER_SIZE - 1] = U'\0'; \
	str32cpy (to2, to1);

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

/* End of file Preferences.h */
#endif
