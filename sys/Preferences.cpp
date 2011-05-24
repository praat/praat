/* Preferences.cpp
 *
 * Copyright (C) 1996-2011 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2002/11/24 Melder_double
 * pb 2003/12/09 guard against Preference file that has been copied from one platform to another
 * pb 2005/03/04 guard against hand-edited Preference files that contain strings longer than (Preferences_STRING_BUFFER_SIZE - 1) bytes
 * pb 2007/08/12 wchar_t
 * pb 2007/09/01 bool
 * pb 2007/11/02 enum
 * pb 2007/11/14 removed swprintf(%ls) because of bug on Mac
 * pb 2007/11/30 Preferences_write forgets thePreferences
 * pb 2007/12/09 removed "resources"
 * pb 2008/01/19 removed float
 * pb 2009/03/21 modern enums
 * pb 2011/05/14 removed charwa
 * pb 2011/05/14 C++
 */

#include "Preferences.h"
#include "Collection.h"

/*
 * Though Preference inherits from Data,
 * we will use SimpleString routines with it.
 */

#define Preference_members Data_members \
	wchar_t *string; \
	int type; \
	void *value; \
	int min, max; \
	const wchar_t * (*getText) (int value); \
	int (*getValue) (const wchar_t *text);
#define Preference_methods Data_methods
class_create (Preference, Data);

/* Warning: copy methods etc. not implemented. */

static void destroy (I) {
	iam (Preference);
	Melder_free (my string);
	inherited (Preference) destroy (me);
}

class_methods (Preference, Data)
	class_method (destroy)
class_methods_end

static SortedSetOfString thePreferences;

static void Preferences_add (const wchar_t *string, int type, void *value, int min, int max, const wchar_t *(*getText) (int value), int (*getValue) (const wchar_t *text)) {
	Preference me = Thing_new (Preference);
	my string = Melder_wcsdup_f (string);
	my type = type;
	my value = value;
	my min = min;
	my max = max;
	my getText = getText;
	my getValue = getValue;
	if (! thePreferences) thePreferences = SortedSetOfString_create ();
	Collection_addItem (thePreferences, me);
}

void Preferences_addByte (const wchar_t *string, signed char *value, signed char defaultValue)
	{ *value = defaultValue; Preferences_add (string, bytewa, value, 0, 0, NULL, NULL); }

void Preferences_addShort (const wchar_t *string, short *value, short defaultValue)
	{ *value = defaultValue; Preferences_add (string, shortwa, value, 0, 0, NULL, NULL); }

void Preferences_addInt (const wchar_t *string, int *value, int defaultValue)
	{ *value = defaultValue; Preferences_add (string, intwa, value, 0, 0, NULL, NULL); }

void Preferences_addLong (const wchar_t *string, long *value, long defaultValue)
	{ *value = defaultValue; Preferences_add (string, longwa, value, 0, 0, NULL, NULL); }

void Preferences_addUbyte (const wchar_t *string, unsigned char *value, unsigned char defaultValue)
	{ *value = defaultValue; Preferences_add (string, ubytewa, value, 0, 0, NULL, NULL); }

void Preferences_addUshort (const wchar_t *string, unsigned short *value, unsigned short defaultValue)
	{ *value = defaultValue; Preferences_add (string, ushortwa, value, 0, 0, NULL, NULL); }

void Preferences_addUint (const wchar_t *string, unsigned int *value, unsigned int defaultValue)
	{ *value = defaultValue; Preferences_add (string, uintwa, value, 0, 0, NULL, NULL); }

void Preferences_addUlong (const wchar_t *string, unsigned long *value, unsigned long defaultValue)
	{ *value = defaultValue; Preferences_add (string, ulongwa, value, 0, 0, NULL, NULL); }

void Preferences_addBool (const wchar_t *string, bool *value, bool defaultValue)
	{ *value = defaultValue; Preferences_add (string, boolwa, value, 0, 0, NULL, NULL); }

void Preferences_addDouble (const wchar_t *string, double *value, double defaultValue)
	{ *value = defaultValue; Preferences_add (string, doublewa, value, 0, 0, NULL, NULL); }

void Preferences_addString (const wchar_t *string, wchar_t *value, const wchar_t *defaultValue)
	{ wcscpy (value, defaultValue); Preferences_add (string, stringwa, value, 0, 0, NULL, NULL); }

void _Preferences_addEnum (const wchar_t *string, enum kPreferences_dummy *value, int min, int max,
	const wchar_t *(*getText) (int value), int (*getValue) (const wchar_t *text), enum kPreferences_dummy defaultValue)
{
	{ *value = defaultValue; Preferences_add (string, enumwa, value, min, max, getText, getValue); }
}

void Preferences_read (MelderFile file) {
	/*
	 * It is possible (see praat.c) that this routine is called
	 * before any preferences have been registered.
	 * In that case, do nothing.
	 */
	if (! thePreferences) return;
	MelderReadText text = MelderReadText_createFromFile (file); cherror
	for (;;) {
		wchar_t *line = MelderReadText_readLine (text), *value;
		if (line == NULL) goto end;
		if ((value = wcsstr (line, L": ")) == NULL) goto end;
		*value = '\0', value += 2;
		long ipref = SortedSetOfString_lookUp (thePreferences, line);
		if (! ipref) continue;   /* Ignore unrecognized preferences. */
		Preference pref = (Preference) thePreferences -> item [ipref];
		switch (pref -> type) {
			case bytewa: * (signed char *) pref -> value = wcstol (value, NULL, 10); break;
			case shortwa: * (short *) pref -> value = wcstol (value, NULL, 10); break;
			case intwa: * (int *) pref -> value = wcstol (value, NULL, 10); break;
			case longwa: * (long *) pref -> value = wcstol (value, NULL, 10); break;
			case ubytewa: * (unsigned char *) pref -> value = wcstoul (value, NULL, 10); break;
			case ushortwa: * (unsigned short *) pref -> value = wcstoul (value, NULL, 10); break;
			case uintwa: * (unsigned int *) pref -> value = wcstoul (value, NULL, 10); break;
			case ulongwa: * (unsigned long *) pref -> value = wcstoul (value, NULL, 10); break;
			case boolwa: * (bool *) pref -> value =
				wcsnequ (value, L"yes", 3) ? true :
				wcsnequ (value, L"no", 2) ? false :
				wcstol (value, NULL, 10) != 0; break;
			case doublewa: * (double *) pref -> value = Melder_atof (value); break;
			case stringwa: {
				wcsncpy ((wchar_t *) pref -> value, value, Preferences_STRING_BUFFER_SIZE);
				((wchar_t *) pref -> value) [Preferences_STRING_BUFFER_SIZE - 1] = '\0'; break;
			}
			case enumwa: {
				int intValue = pref -> getValue (value);
				if (intValue < 0)
					intValue = pref -> getValue (L"\t");   // look for the default
				* (enum kPreferences_dummy *) pref -> value = (enum kPreferences_dummy) intValue; break;
			}
		}
	}
end:
	Melder_clearError ();
	MelderReadText_delete (text);
}

void Preferences_write (MelderFile file) {
	if (! thePreferences || thePreferences -> size == 0) return;
	static MelderString buffer = { 0 };
	for (long ipref = 1; ipref <= thePreferences -> size; ipref ++) {
		Preference pref = (Preference) thePreferences -> item [ipref];
		MelderString_append2 (& buffer, pref -> string, L": ");
		switch (pref -> type) {
			case bytewa: MelderString_append1 (& buffer, Melder_integer (* (signed char *) pref -> value)); break;
			case shortwa: MelderString_append1 (& buffer, Melder_integer (* (short *) pref -> value)); break;
			case intwa: MelderString_append1 (& buffer, Melder_integer (* (int *) pref -> value)); break;
			case longwa: MelderString_append1 (& buffer, Melder_integer (* (long *) pref -> value)); break;
			case ubytewa: MelderString_append1 (& buffer, Melder_integer (* (unsigned char *) pref -> value)); break;
			case ushortwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned short *) pref -> value)); break;
			case uintwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned int *) pref -> value)); break;
			case ulongwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned long *) pref -> value)); break;
			case boolwa: MelderString_append1 (& buffer, Melder_boolean (* (bool *) pref -> value)); break;
			case doublewa: MelderString_append1 (& buffer, Melder_double (* (double *) pref -> value)); break;
			case stringwa: MelderString_append1 (& buffer, (const wchar *) pref -> value); break;
			case enumwa: MelderString_append1 (& buffer, pref -> getText (* (enum kPreferences_dummy *) pref -> value)); break;
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	MelderFile_writeText (file, buffer.string);
	Melder_clearError ();
	forget (thePreferences);	
}

/* End of file Preferences.cpp */
