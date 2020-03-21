/* Preferences.cpp
 *
 * Copyright (C) 1996-2013,2015-2020 Paul Boersma
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
#include "Collection.h"

Thing_define (Preference, SimpleString) {
	int type;
	void *value;
	int min, max;
	conststring32 (*getText) (int value);
	int (*getValue) (conststring32 text);

	void v_destroy () noexcept
		override;
	/* Warning: copy methods etc. not implemented. */
};
Thing_implement (Preference, SimpleString, 0);

void structPreference :: v_destroy () noexcept {
	Preference_Parent :: v_destroy ();
}

static SortedSetOfStringOf <structPreference> thePreferences;

void Preferences_exit_optimizeByLeaking () {
	thePreferences. _ownItems = false;
}

static void Preferences_add (conststring32 string, int type, void *value, int min, int max,
	conststring32 (*getText) (int value), int (*getValue) (conststring32 text))
{
	autoPreference me = Thing_new (Preference);
	my string = Melder_dup (string);
	for (char32 *p = & my string [0]; *p != U'\0'; p ++)
		if (*p == U'_') *p = U'.';
	my type = type;
	my value = value;
	my min = min;
	my max = max;
	my getText = getText;
	my getValue = getValue;
	thePreferences. addItem_move (me.move());
}

void Preferences_addByte (conststring32 string, signed char *value, signed char defaultValue)
	{ *value = defaultValue; Preferences_add (string, bytewa, value, 0, 0, nullptr, nullptr); }

void Preferences_addInt16 (conststring32 string, int *value, int defaultValue)
	{ *value = defaultValue; Preferences_add (string, int16wa, value, 0, 0, nullptr, nullptr); }

void Preferences_addInt (conststring32 string, int *value, int defaultValue)
	{ *value = defaultValue; Preferences_add (string, intwa, value, 0, 0, nullptr, nullptr); }

void Preferences_addInteger (conststring32 string, integer *value, integer defaultValue)
	{ *value = defaultValue; Preferences_add (string, integerwa, value, 0, 0, nullptr, nullptr); }

void Preferences_addUbyte (conststring32 string, unsigned char *value, unsigned char defaultValue)
	{ *value = defaultValue; Preferences_add (string, ubytewa, value, 0, 0, nullptr, nullptr); }

void Preferences_addUint (conststring32 string, unsigned int *value, unsigned int defaultValue)
	{ *value = defaultValue; Preferences_add (string, uintwa, value, 0, 0, nullptr, nullptr); }

void Preferences_addUinteger (conststring32 string, uinteger *value, uinteger defaultValue)
	{ *value = defaultValue; Preferences_add (string, uintegerwa, value, 0, 0, nullptr, nullptr); }

void Preferences_addBool (conststring32 string, bool *value, bool defaultValue)
	{ *value = defaultValue; Preferences_add (string, questionwa, value, 0, 0, nullptr, nullptr); }

void Preferences_addDouble (conststring32 string, double *value, double defaultValue)
	{ *value = defaultValue; Preferences_add (string, doublewa, value, 0, 0, nullptr, nullptr); }

void Preferences_addString (conststring32 string, char32 *value, conststring32 defaultValue)
	{ str32cpy (value, defaultValue); Preferences_add (string, stringwa, value, 0, 0, nullptr, nullptr); }

void _Preferences_addEnum (conststring32 string, int *value, int min, int max,
	conststring32 (*getText) (int value), int (*getValue) (conststring32 text), int defaultValue)
{
	{ *value = defaultValue; Preferences_add (string, enumwa, value, min, max, getText, getValue); }
}

void Preferences_read (MelderFile file) {
	/*
	 * It is possible (see praat.cpp) that this routine is called
	 * before any preferences have been registered.
	 * In that case, do nothing.
	 */
	if (thePreferences.size == 0) return;
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);
		for (;;) {
			const mutablestring32 line = MelderReadText_readLine (text.get());
			if (! line)
				return;   // OK: we have read past the last line
			char32 *value = str32str (line, U": ");
			if (! value)
				return;   // OK: we have read past the last key-value pair
			*value = U'\0';
			value += 2;
			integer ipref = thePreferences. lookUp (line);
			if (ipref == 0) {
				/*
					Recognize some preference names that went obsolete in February 2013.
				*/
				if (Melder_nequ (line, U"FunctionEditor.", 15))
					ipref = thePreferences. lookUp (Melder_cat (U"TimeSoundAnalysisEditor.", line + 15));
			}
			if (ipref == 0) continue;   // skip unrecognized keys
			Preference pref = thePreferences.at [ipref];
			switch (pref -> type) {
				case bytewa: * (signed char *) pref -> value =
						(int8) strtol (Melder_peek32to8 (value), nullptr, 10);
				break;
				case int16wa: * (int16 *) pref -> value =
						(int16) strtol (Melder_peek32to8 (value), nullptr, 10);
				break;
				case intwa: * (int *) pref -> value =
						strtol (Melder_peek32to8 (value), nullptr, 10);
				break;
				case integerwa: * (integer *) pref -> value =
						strtol (Melder_peek32to8 (value), nullptr, 10);
				break;
				case ubytewa: * (unsigned char *) pref -> value =
						(uint8) strtoul (Melder_peek32to8 (value), nullptr, 10);
				break;
				case uintwa: * (unsigned int *) pref -> value =
						strtoul (Melder_peek32to8 (value), nullptr, 10);
				break;
				case uintegerwa: * (uinteger *) pref -> value =
						strtoul (Melder_peek32to8 (value), nullptr, 10);
				break;
				case doublewa: * (double *) pref -> value =
						Melder_a8tof (Melder_peek32to8 (value));
				break;
				case stringwa: {
					str32ncpy ((char32 *) pref -> value, value, Preferences_STRING_BUFFER_SIZE);
					((char32 *) pref -> value) [Preferences_STRING_BUFFER_SIZE - 1] = U'\0';
				}
				break;
				case enumwa: {
					int intValue = pref -> getValue (value);
					if (intValue < 0)
						intValue = pref -> getValue (U"\t");   // look for the default
					* (int *) pref -> value = intValue;
				}
				break;
				case questionwa: * (bool *) pref -> value =
						( str32nequ (value, U"yes", 3) ? true :
						  str32nequ (value, U"no", 2) ? false :
						  strtol (Melder_peek32to8 (value), nullptr, 10) != 0 );
				break;
			}
		}
	} catch (MelderError) {
		Melder_clearError ();   // this is done during start-up, so it should never fail
	}
}

void Preferences_write (MelderFile file) {
	if (thePreferences.size == 0)
		return;
	static MelderString buffer;
	for (integer ipref = 1; ipref <= thePreferences.size; ipref ++) {
		Preference pref = thePreferences.at [ipref];
		MelderString_append (& buffer, pref -> string.get(), U": ");
		switch (pref -> type) {
			case bytewa:     MelderString_append (& buffer, (int) (* (signed char *)    pref -> value)); break;
			case int16wa:    MelderString_append (& buffer,       (* (int16 *)          pref -> value)); break;
			case intwa:      MelderString_append (& buffer,       (* (int *)            pref -> value)); break;
			case integerwa:  MelderString_append (& buffer,       (* (integer *)        pref -> value)); break;
			case ubytewa:    MelderString_append (& buffer, (int) (* (unsigned char *)  pref -> value)); break;
			case uintwa:     MelderString_append (& buffer,       (* (unsigned int *)   pref -> value)); break;
			case uintegerwa: MelderString_append (& buffer,       (* (uinteger *)       pref -> value)); break;
			case doublewa:   MelderString_append (& buffer,       (* (double *)         pref -> value)); break;
			case stringwa:   MelderString_append (& buffer,         ((conststring32)    pref -> value)); break;
			case enumwa:     MelderString_append (& buffer,  pref -> getText (* (int *) pref -> value)); break;
			case questionwa: MelderString_append (& buffer,       (* (bool *)           pref -> value)); break;
		}
		MelderString_appendCharacter (& buffer, U'\n');
	}
	try {
		MelderFile_writeText (file, buffer.string, kMelder_textOutputEncoding::ASCII_THEN_UTF16);
	} catch (MelderError) {
		Melder_clearError ();
	}
}

/* End of file Preferences.cpp */
