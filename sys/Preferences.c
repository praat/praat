/* Preferences.c
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
 * pb 2002/03/07 GPL
 * pb 2002/11/24 Melder_double
 * pb 2003/12/09 guard against Preference file that has been copied from one platform to another
 * pb 2005/03/04 guard against hand-edited Preference files that contain strings longer than (Resources_STRING_BUFFER_SIZE - 1) bytes
 * pb 2007/08/12 wchar_t
 * pb 2007/09/01 bool
 * pb 2007/11/02 enum
 * pb 2007/11/14 removed swprintf(%ls) because of bug on Mac
 * pb 2007/11/30 Resources_write forgets theResources
 */

#include "Preferences.h"
#include "Collection.h"

/*
 * Though Resource inherits from Data,
 * we will use SimpleString routines with it.
 */

#define Resource_members Data_members \
	wchar_t *string; \
	int type; \
	void *value; \
	int min, max; \
	wchar_t * (*getText) (int value); \
	int (*getValue) (wchar_t *text);
#define Resource_methods Data_methods
class_create (Resource, Data);

/* Warning: copy methods etc. not implemented. */

static void destroy (I) {
	iam (Resource);
	Melder_free (my string);
	inherited (Resource) destroy (me);
}

class_methods (Resource, Data)
	class_method (destroy)
class_methods_end

static SortedSetOfString theResources;

static void Resources_add (const wchar_t *string, int type, void *value, int min, int max, wchar_t *(*getText) (int value), int (*getValue) (wchar_t *text)) {
	Resource me = new (Resource);
	my string = Melder_wcsdup (string);
	my type = type;
	my value = value;
	my min = min;
	my max = max;
	my getText = getText;
	my getValue = getValue;
	if (! theResources) theResources = SortedSetOfString_create ();
	Collection_addItem (theResources, me);
}

void Resources_addByte (const wchar_t *string, signed char *value)
	{ Resources_add (string, bytewa, value, 0, 0, NULL, NULL); }

void Preferences_addByte (const wchar_t *string, signed char *value, signed char defaultValue)
	{ *value = defaultValue; Resources_add (string, bytewa, value, 0, 0, NULL, NULL); }

void Resources_addShort (const wchar_t *string, short *value)
	{ Resources_add (string, shortwa, value, 0, 0, NULL, NULL); }

void Preferences_addShort (const wchar_t *string, short *value, short defaultValue)
	{ *value = defaultValue; Resources_add (string, shortwa, value, 0, 0, NULL, NULL); }

void Resources_addInt (const wchar_t *string, int *value)
	{ Resources_add (string, intwa, value, 0, 0, NULL, NULL); }

void Preferences_addInt (const wchar_t *string, int *value, int defaultValue)
	{ *value = defaultValue; Resources_add (string, intwa, value, 0, 0, NULL, NULL); }

void Resources_addLong (const wchar_t *string, long *value)
	{ Resources_add (string, longwa, value, 0, 0, NULL, NULL); }

void Preferences_addLong (const wchar_t *string, long *value, long defaultValue)
	{ *value = defaultValue; Resources_add (string, longwa, value, 0, 0, NULL, NULL); }

void Resources_addUbyte (const wchar_t *string, unsigned char *value)
	{ Resources_add (string, ubytewa, value, 0, 0, NULL, NULL); }

void Preferences_addUbyte (const wchar_t *string, unsigned char *value, unsigned char defaultValue)
	{ *value = defaultValue; Resources_add (string, ubytewa, value, 0, 0, NULL, NULL); }

void Resources_addUshort (const wchar_t *string, unsigned short *value)
	{ Resources_add (string, ushortwa, value, 0, 0, NULL, NULL); }

void Preferences_addUshort (const wchar_t *string, unsigned short *value, unsigned short defaultValue)
	{ *value = defaultValue; Resources_add (string, ushortwa, value, 0, 0, NULL, NULL); }

void Resources_addUint (const wchar_t *string, unsigned int *value)
	{ Resources_add (string, uintwa, value, 0, 0, NULL, NULL); }

void Preferences_addUint (const wchar_t *string, unsigned int *value, unsigned int defaultValue)
	{ *value = defaultValue; Resources_add (string, uintwa, value, 0, 0, NULL, NULL); }

void Resources_addUlong (const wchar_t *string, unsigned long *value)
	{ Resources_add (string, ulongwa, value, 0, 0, NULL, NULL); }

void Preferences_addUlong (const wchar_t *string, unsigned long *value, unsigned long defaultValue)
	{ *value = defaultValue; Resources_add (string, ulongwa, value, 0, 0, NULL, NULL); }

void Resources_addBool (const wchar_t *string, bool *value)
	{ Resources_add (string, boolwa, value, 0, 0, NULL, NULL); }

void Preferences_addBool (const wchar_t *string, bool *value, bool defaultValue)
	{ *value = defaultValue; Resources_add (string, boolwa, value, 0, 0, NULL, NULL); }

void Resources_addChar (const wchar_t *string, wchar_t *value)
	{ Resources_add (string, charwa, value, 0, 0, NULL, NULL); }

void Preferences_addChar (const wchar_t *string, wchar_t *value, wchar_t defaultValue)
	{ *value = defaultValue; Resources_add (string, charwa, value, 0, 0, NULL, NULL); }

void Resources_addFloat (const wchar_t *string, float *value)
	{ Resources_add (string, floatwa, value, 0, 0, NULL, NULL); }

void Preferences_addFloat (const wchar_t *string, float *value, float defaultValue)
	{ *value = defaultValue; Resources_add (string, floatwa, value, 0, 0, NULL, NULL); }

void Resources_addDouble (const wchar_t *string, double *value)
	{ Resources_add (string, doublewa, value, 0, 0, NULL, NULL); }

void Preferences_addDouble (const wchar_t *string, double *value, double defaultValue)
	{ *value = defaultValue; Resources_add (string, doublewa, value, 0, 0, NULL, NULL); }

void Resources_addString (const wchar_t *string, wchar_t *value)
	{ Resources_add (string, stringwwa, value, 0, 0, NULL, NULL); }

void Preferences_addString (const wchar_t *string, wchar_t *value, const wchar_t *defaultValue)
	{ wcscpy (value, defaultValue); Resources_add (string, stringwwa, value, 0, 0, NULL, NULL); }

void _Preferences_addEnum (const wchar_t *string, int *value, int min, int max,
	wchar_t *(*getText) (int value), int (*getValue) (wchar_t *text), int defaultValue)
{
	{ *value = defaultValue; Resources_add (string, enumwa, value, min, max, getText, getValue); }
}

void Resources_read (MelderFile file) {
	/*
	 * It is possible (see praat.c) that this routine is called
	 * before any resources have been registered.
	 * In that case, do nothing.
	 */
	if (! theResources) return;
	wchar_t *string = MelderFile_readText (file); cherror
	MelderReadString buffer = { string, string };
	for (;;) {
		wchar_t *line = MelderReadString_readLine (& buffer), *value;
		long iresource;
		Resource resource;
		if (! line) goto end;
		if ((value = wcsstr (line, L": ")) == NULL) goto end;
		*value = '\0', value += 2;
		iresource = SortedSetOfString_lookUp (theResources, line);
		if (! iresource) continue;   /* Ignore unrecognized resources. */
		resource = theResources -> item [iresource];
		switch (resource -> type) {
			case bytewa: * (signed char *) resource -> value = wcstol (value, NULL, 10); break;
			case shortwa: * (short *) resource -> value = wcstol (value, NULL, 10); break;
			case intwa: * (int *) resource -> value = wcstol (value, NULL, 10); break;
			case longwa: * (long *) resource -> value = wcstol (value, NULL, 10); break;
			case ubytewa: * (unsigned char *) resource -> value = wcstoul (value, NULL, 10); break;
			case ushortwa: * (unsigned short *) resource -> value = wcstoul (value, NULL, 10); break;
			case uintwa: * (unsigned int *) resource -> value = wcstoul (value, NULL, 10); break;
			case ulongwa: * (unsigned long *) resource -> value = wcstoul (value, NULL, 10); break;
			case boolwa: * (bool *) resource -> value =
				wcsnequ (value, L"yes", 3) ? true :
				wcsnequ (value, L"no", 2) ? false :
				wcstol (value, NULL, 10) != 0; break;
			case charwa: * (wchar_t *) resource -> value = value [0]; break;
			case floatwa: * (float *) resource -> value = wcstod (value, NULL); break;
			case doublewa: * (double *) resource -> value = wcstod (value, NULL); break;
			case stringwwa: {
				wcsncpy ((wchar_t *) resource -> value, value, Resources_STRING_BUFFER_SIZE);
				((wchar_t *) resource -> value) [Resources_STRING_BUFFER_SIZE - 1] = '\0'; break;
			}
			case enumwa: * (int *) resource -> value = resource -> getValue (value); break;
		}
	}
end:
	Melder_clearError ();
	Melder_free (string);
}

void Resources_write (MelderFile file) {
	if (! theResources || theResources -> size == 0) return;
	static MelderString buffer = { 0 };
	for (long iresource = 1; iresource <= theResources -> size; iresource ++) {
		Resource resource = theResources -> item [iresource];
		MelderString_append2 (& buffer, resource -> string, L": ");
		switch (resource -> type) {
			case bytewa: MelderString_append1 (& buffer, Melder_integer (* (signed char *) resource -> value)); break;
			case shortwa: MelderString_append1 (& buffer, Melder_integer (* (short *) resource -> value)); break;
			case intwa: MelderString_append1 (& buffer, Melder_integer (* (int *) resource -> value)); break;
			case longwa: MelderString_append1 (& buffer, Melder_integer (* (long *) resource -> value)); break;
			case ubytewa: MelderString_append1 (& buffer, Melder_integer (* (unsigned char *) resource -> value)); break;
			case ushortwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned short *) resource -> value)); break;
			case uintwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned int *) resource -> value)); break;
			case ulongwa: MelderString_append1 (& buffer, Melder_integer (* (unsigned long *) resource -> value)); break;
			case boolwa: MelderString_append1 (& buffer, * (bool *) resource -> value ? L"yes" : L"no"); break;
			case charwa: MelderString_appendCharacter (& buffer, * (char *) resource -> value); break;
			case floatwa: MelderString_append1 (& buffer, Melder_single (* (float *) resource -> value)); break;
			case doublewa: MelderString_append1 (& buffer, Melder_double (* (double *) resource -> value)); break;
			case stringwwa: MelderString_append1 (& buffer, resource -> value); break;
			case enumwa: MelderString_append1 (& buffer, resource -> getText (* (int *) resource -> value)); break;
		}
		MelderString_appendCharacter (& buffer, '\n');
	}
	MelderFile_writeText (file, buffer.string);
	Melder_clearError ();
	forget (theResources);	
}

/* End of file Preferences.c */
