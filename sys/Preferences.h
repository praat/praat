#ifndef _Preferences_h_
#define _Preferences_h_
/* Preferences.h
 *
 * Copyright (C) 1996-2007 Paul Boersma
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

/*
 * pb 2007/09/03
 */

#ifndef _melder_h_
	#include "melder.h"
#endif

/*
 * All strings added with Resources_addString should have the following buffer size,
 * which conveniently equals the size of the path buffer in MelderFile so that
 * file paths can be used as preferences.
 */
#define Resources_STRING_BUFFER_SIZE 260

void Resources_addByte (const wchar_t *string, signed char *value);
void Preferences_addByte (const wchar_t *string, signed char *value, signed char defaultValue);
void Resources_addShort (const wchar_t *string, short *value);
void Preferences_addShort (const wchar_t *string, short *value, short defaultValue);
void Resources_addInt (const wchar_t *string, int *value);
void Preferences_addInt (const wchar_t *string, int *value, int defaultValue);
void Resources_addLong (const wchar_t *string, long *value);
void Preferences_addLong (const wchar_t *string, long *value, long defaultValue);
void Resources_addUbyte (const wchar_t *string, unsigned char *value);
void Preferences_addUbyte (const wchar_t *string, unsigned char *value, unsigned char defaultValue);
void Resources_addUshort (const wchar_t *string, unsigned short *value);
void Preferences_addUshort (const wchar_t *string, unsigned short *value, unsigned short defaultValue);
void Resources_addUint (const wchar_t *string, unsigned int *value);
void Preferences_addUint (const wchar_t *string, unsigned int *value, unsigned int defaultValue);
void Resources_addUlong (const wchar_t *string, unsigned long *value);
void Preferences_addUlong (const wchar_t *string, unsigned long *value, unsigned long defaultValue);
void Resources_addBool (const wchar_t *string, bool *value);
void Preferences_addBool (const wchar_t *string, bool *value, bool defaultValue);
void Resources_addChar (const wchar_t *string, wchar_t *value);
void Preferences_addChar (const wchar_t *string, wchar_t *value, wchar_t defaultValue);
void Resources_addFloat (const wchar_t *string, float *value);
void Preferences_addFloat (const wchar_t *string, float *value, float defaultValue);
void Resources_addDouble (const wchar_t *string, double *value);
void Preferences_addDouble (const wchar_t *string, double *value, double defaultValue);
void Resources_addString (const wchar_t *string, wchar_t *value);
void Preferences_addString (const wchar_t *string, wchar_t *value, const wchar_t *defaultValue);
void _Preferences_addEnum (const wchar_t *string, int *value, int min, int max,
	wchar_t *(*getText) (int value), int (*getValue) (wchar_t *text), int defaultValue);
#define Preferences_addEnum(string,value,enum,defaultValue) \
	_Preferences_addEnum (string, value, enum##_MIN, enum##_MAX, enum##_getText, enum##_getValue, enum##_##defaultValue)

void Resources_read (MelderFile file);
void Resources_write (MelderFile file);

/* End of file Preferences.h */
#endif
