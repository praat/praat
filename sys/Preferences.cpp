/* Preferences.cpp
 *
 * Copyright (C) 1996-2012,2013 Paul Boersma
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

#include "Preferences.h"
#include "Collection.h"

Thing_define (Preference, SimpleString) {
	// new data:
	public:
		int type;
		void *value;
		int min, max;
		const wchar_t * (*getText) (int value);
		int (*getValue) (const wchar_t *text);
	// overridden methods:
		void v_destroy ();
		/* Warning: copy methods etc. not implemented. */
};
Thing_implement (Preference, SimpleString, 0);

void structPreference :: v_destroy () {
	Melder_free (string);
	Preference_Parent :: v_destroy ();
}

static SortedSetOfString thePreferences;

static void Preferences_add (const wchar_t *string, int type, void *value, int min, int max, const wchar_t * (*getText) (int value), int (*getValue) (const wchar_t *text)) {
	Preference me = Thing_new (Preference);
	my string = Melder_wcsdup_f (string);
	for (wchar_t *p = & my string [0]; *p != '\0'; p ++) if (*p == '_') *p = '.';
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

void Preferences_addInt (const wchar_t *string, int *value, int defaultValue)
	{ *value = defaultValue; Preferences_add (string, intwa, value, 0, 0, NULL, NULL); }

void Preferences_addLong (const wchar_t *string, long *value, long defaultValue)
	{ *value = defaultValue; Preferences_add (string, longwa, value, 0, 0, NULL, NULL); }

void Preferences_addUbyte (const wchar_t *string, unsigned char *value, unsigned char defaultValue)
	{ *value = defaultValue; Preferences_add (string, ubytewa, value, 0, 0, NULL, NULL); }

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
	 * It is possible (see praat.cpp) that this routine is called
	 * before any preferences have been registered.
	 * In that case, do nothing.
	 */
	if (! thePreferences) return;
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);
		for (;;) {
			wchar_t *line = MelderReadText_readLine (text.peek());
			if (line == NULL)
				return;   // OK: we have read past the last line
			wchar_t *value = wcsstr (line, L": ");
			if (value == NULL)
				return;   // OK: we have read past the last key-value pair
			*value = '\0', value += 2;
			long ipref = SortedSetOfString_lookUp (thePreferences, line);
			if (! ipref) {
				/*
				 * Recognize some preference names that went obsolete in February 2013.
				 */
				if (Melder_wcsnequ (line, L"FunctionEditor.", 15))
					ipref = SortedSetOfString_lookUp (thePreferences,
						Melder_wcscat (L"TimeSoundAnalysisEditor.", line + 15));
			}
			if (! ipref) continue;   // skip unrecognized keys
			Preference pref = (Preference) thePreferences -> item [ipref];
			switch (pref -> type) {
				case bytewa: * (signed char *) pref -> value = wcstol (value, NULL, 10); break;
				case intwa: * (int *) pref -> value = wcstol (value, NULL, 10); break;
				case longwa: * (long *) pref -> value = wcstol (value, NULL, 10); break;
				case ubytewa: * (unsigned char *) pref -> value = wcstoul (value, NULL, 10); break;
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
	} catch (MelderError) {
		Melder_clearError ();   // this is done during start-up, so it should never fail
	}
}

void Preferences_write (MelderFile file) {
	if (! thePreferences || thePreferences -> size == 0) return;
	static MelderString buffer = { 0 };
	for (long ipref = 1; ipref <= thePreferences -> size; ipref ++) {
		Preference pref = (Preference) thePreferences -> item [ipref];
		MelderString_append (& buffer, pref -> string, L": ");
		switch (pref -> type) {
			case bytewa:   MelderString_append (& buffer, Melder_integer (* (signed char *)    pref -> value)); break;
			case intwa:    MelderString_append (& buffer, Melder_integer (* (int *)            pref -> value)); break;
			case longwa:   MelderString_append (& buffer, Melder_integer (* (long *)           pref -> value)); break;
			case ubytewa:  MelderString_append (& buffer, Melder_integer (* (unsigned char *)  pref -> value)); break;
			case uintwa:   MelderString_append (& buffer, Melder_integer (* (unsigned int *)   pref -> value)); break;
			case ulongwa:  MelderString_append (& buffer, Melder_integer (* (unsigned long *)  pref -> value)); break;
			case boolwa:   MelderString_append (& buffer, Melder_boolean (* (bool *)           pref -> value)); break;
			case doublewa: MelderString_append (& buffer, Melder_double  (* (double *)         pref -> value)); break;
			case stringwa: MelderString_append (& buffer, (const wchar_t *) pref -> value); break;
			case enumwa:   MelderString_append (& buffer, pref -> getText (* (enum kPreferences_dummy *) pref -> value)); break;
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	try {
		MelderFile_writeText (file, buffer.string, kMelder_textOutputEncoding_ASCII_THEN_UTF16);
	} catch (MelderError) {
		Melder_clearError ();
	}
	forget (thePreferences);	
}

/* End of file Preferences.cpp */
