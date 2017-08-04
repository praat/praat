#ifndef _Preferences_h_
#define _Preferences_h_
/* Preferences.h
 *
 * Copyright (C) 1996-2011,2013,2015,2017 Paul Boersma
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

#define pref_str32cpy(to,from) \
	str32ncpy (to, from, Preferences_STRING_BUFFER_SIZE); \
	to [Preferences_STRING_BUFFER_SIZE - 1] = U'\0';

#define pref_str32cpy2(to2,to1,from) \
	str32ncpy (to1, from, Preferences_STRING_BUFFER_SIZE); \
	to1 [Preferences_STRING_BUFFER_SIZE - 1] = U'\0'; \
	str32cpy (to2, to1);

enum kPreferences_dummy { dummy1 = 1, dummy2 = 2 };

void Preferences_addByte   (const char32 *string /* cattable */, signed char *value, signed char defaultValue);
void Preferences_addShort  (const char32 *string /* cattable */, short *value, short defaultValue);
void Preferences_addInt16  (const char32 *string /* cattable */, int *value, int defaultValue);
void Preferences_addInt    (const char32 *string /* cattable */, int *value, int defaultValue);
void Preferences_addLong   (const char32 *string /* cattable */, long *value, long defaultValue);
void Preferences_addUbyte  (const char32 *string /* cattable */, unsigned char *value, unsigned char defaultValue);
void Preferences_addUshort (const char32 *string /* cattable */, unsigned short *value, unsigned short defaultValue);
void Preferences_addUint   (const char32 *string /* cattable */, unsigned int *value, unsigned int defaultValue);
void Preferences_addUlong  (const char32 *string /* cattable */, unsigned long *value, unsigned long defaultValue);
void Preferences_addBool   (const char32 *string /* cattable */, bool *value, bool defaultValue);
void Preferences_addDouble (const char32 *string /* cattable */, double *value, double defaultValue);
void Preferences_addString (const char32 *string /* cattable */, char32 *value, const char32 *defaultValue);
void _Preferences_addEnum  (const char32 *string /* cattable */, enum kPreferences_dummy *value, int min, int max,
	const char32 *(*getText) (int value), int (*getValue) (const char32 *text), enum kPreferences_dummy defaultValue);
#define Preferences_addEnum(string,value,enumerated,defaultValue) \
	_Preferences_addEnum (string, (enum kPreferences_dummy *) value, enumerated##_MIN, enumerated##_MAX, \
	enumerated##_getText, enumerated##_getValue, (enum kPreferences_dummy) defaultValue)

void Preferences_read (MelderFile file);
void Preferences_write (MelderFile file);

/* End of file Preferences.h */
#endif
