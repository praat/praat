#ifndef _Preferences_h_
#define _Preferences_h_
/* Preferences.h
 *
 * Copyright (C) 1996-2011,2013 Paul Boersma
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

#include "melder.h"

/*
 * All strings added with Preferences_addString should have the following buffer size,
 * which conveniently equals the size of the path buffer in MelderFile so that
 * file paths can be used as preferences.
 */
#define Preferences_STRING_BUFFER_SIZE 1+kMelder_MAXPATH

#define pref_wcscpy(to,from) \
	wcsncpy (to, from, Preferences_STRING_BUFFER_SIZE); \
	to [Preferences_STRING_BUFFER_SIZE - 1] = '\0';

#define pref_wcscpy2(to2,to1,from) \
	wcsncpy (to1, from, Preferences_STRING_BUFFER_SIZE); \
	to1 [Preferences_STRING_BUFFER_SIZE - 1] = '\0'; \
	wcscpy (to2, to1);

enum kPreferences_dummy { dummy1 = 1, dummy2 = 2 };

void Preferences_addByte (const wchar_t *string, signed char *value, signed char defaultValue);
void Preferences_addShort (const wchar_t *string, short *value, short defaultValue);
void Preferences_addInt (const wchar_t *string, int *value, int defaultValue);
void Preferences_addLong (const wchar_t *string, long *value, long defaultValue);
void Preferences_addUbyte (const wchar_t *string, unsigned char *value, unsigned char defaultValue);
void Preferences_addUshort (const wchar_t *string, unsigned short *value, unsigned short defaultValue);
void Preferences_addUint (const wchar_t *string, unsigned int *value, unsigned int defaultValue);
void Preferences_addUlong (const wchar_t *string, unsigned long *value, unsigned long defaultValue);
void Preferences_addBool (const wchar_t *string, bool *value, bool defaultValue);
void Preferences_addDouble (const wchar_t *string, double *value, double defaultValue);
void Preferences_addString (const wchar_t *string, wchar_t *value, const wchar_t *defaultValue);
void _Preferences_addEnum (const wchar_t *string, enum kPreferences_dummy *value, int min, int max,
	const wchar_t *(*getText) (int value), int (*getValue) (const wchar_t *text), enum kPreferences_dummy defaultValue);
#define Preferences_addEnum(string,value,enumerated,defaultValue) \
	_Preferences_addEnum (string, (enum kPreferences_dummy *) value, enumerated##_MIN, enumerated##_MAX, \
	enumerated##_getText, enumerated##_getValue, (enum kPreferences_dummy) defaultValue)

void Preferences_read (MelderFile file);
void Preferences_write (MelderFile file);

/* End of file Preferences.h */
#endif
