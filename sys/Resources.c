/* Resources.c
 *
 * Copyright (C) 1996-2003 Paul Boersma
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
 * pb 1999/11/09
 * pb 2002/03/07 GPL
 * pb 2002/11/24 Melder_double
 * pb 2003/12/09 guard against Preference file that has been copied from one platform to another
 */

#include "Resources.h"
#include "Collection.h"

/*
 * Though Resource inherits from Data,
 * we will use SimpleString routines with it.
 */

#define Resource_members Data_members \
	char *string; \
	int type; \
	void *value;
#define Resource_methods Data_methods
class_create (Resource, Data)

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

static void Resources_add (const char *string, int type, void *value) {
	Resource me = new (Resource);
	my string = Melder_strdup (string);
	my type = type;
	my value = value;
	if (! theResources) theResources = SortedSetOfString_create ();
	Collection_addItem (theResources, me);
}

void Resources_addByte (const char *string, signed char *value)
	{ Resources_add (string, bytewa, value); }

void Resources_addShort (const char *string, short *value)
	{ Resources_add (string, shortwa, value); }

void Resources_addInt (const char *string, int *value)
	{ Resources_add (string, intwa, value); }

void Resources_addLong (const char *string, long *value)
	{ Resources_add (string, longwa, value); }

void Resources_addUbyte (const char *string, unsigned char *value)
	{ Resources_add (string, ubytewa, value); }

void Resources_addUshort (const char *string, unsigned short *value)
	{ Resources_add (string, ushortwa, value); }

void Resources_addUint (const char *string, unsigned int *value)
	{ Resources_add (string, uintwa, value); }

void Resources_addUlong (const char *string, unsigned long *value)
	{ Resources_add (string, ulongwa, value); }

void Resources_addChar (const char *string, char *value)
	{ Resources_add (string, charwa, value); }

void Resources_addFloat (const char *string, float *value)
	{ Resources_add (string, floatwa, value); }

void Resources_addDouble (const char *string, double *value)
	{ Resources_add (string, doublewa, value); }

void Resources_addString (const char *string, char *value)
	{ Resources_add (string, stringwa, value); }

void Resources_read (MelderFile file) {
	/*
	 * It is possible (see praat.c) that this routine is called
	 * before any resources have been registered.
	 * In that case, do nothing.
	 */
	if (! theResources) return;
	MelderFile_open (file);
	for (;;) {
		char *line = MelderFile_readLine (file), *value;
		long iresource;
		Resource resource;
		if (! line) goto end;
		if ((value = strstr (line, ": ")) == NULL) goto end;
		*value = '\0', value += 2;
		iresource = SortedSetOfString_lookUp (theResources, line);
		if (! iresource) continue;   /* Ignore unrecognized resources. */
		resource = theResources -> item [iresource];
		switch (resource -> type) {
			case bytewa: * (signed char *) resource -> value = atoi (value); break;
			case shortwa: * (short *) resource -> value = atoi (value); break;
			case intwa: * (int *) resource -> value = atoi (value); break;
			case longwa: * (long *) resource -> value = atol (value); break;
			case ubytewa: * (unsigned char *) resource -> value = strtoul (value, NULL, 10); break;
			case ushortwa: * (unsigned short *) resource -> value = strtoul (value, NULL, 10); break;
			case uintwa: * (unsigned int *) resource -> value = strtoul (value, NULL, 10); break;
			case ulongwa: * (unsigned long *) resource -> value = strtoul (value, NULL, 10); break;
			case charwa: * (char *) resource -> value = value [0]; break;
			case floatwa: * (float *) resource -> value = atof (value); break;
			case doublewa: * (double *) resource -> value = atof (value); break;
			case stringwa: strcpy (resource -> value, value); break;
		}
	}
end:
	MelderFile_close (file);
	Melder_clearError ();
}

void Resources_write (MelderFile file) {
	FILE *f;
	long iresource;
	if (! theResources || theResources -> size == 0) return;
	f = Melder_fopen (file, "w");
	if (! f) { Melder_clearError (); return; }
	for (iresource = 1; iresource <= theResources -> size; iresource ++) {
		Resource resource = theResources -> item [iresource];
		fprintf (f, "%s: ", resource -> string);
		switch (resource -> type) {
			case bytewa: fprintf (f, "%d\n", * (signed char *) resource -> value); break;
			case shortwa: fprintf (f, "%d\n", * (short *) resource -> value); break;
			case intwa: fprintf (f, "%d\n", * (int *) resource -> value); break;
			case longwa: fprintf (f, "%ld\n", * (long *) resource -> value); break;
			case ubytewa: fprintf (f, "%u\n", * (unsigned char *) resource -> value); break;
			case ushortwa: fprintf (f, "%u\n", * (unsigned short *) resource -> value); break;
			case uintwa: fprintf (f, "%u\n", * (unsigned int *) resource -> value); break;
			case ulongwa: fprintf (f, "%lu\n", * (unsigned long *) resource -> value); break;
			case charwa: fprintf (f, "%c\n", * (char *) resource -> value); break;
			case floatwa: fprintf (f, "%.9g\n", * (float *) resource -> value); break;
			case doublewa: fprintf (f, "%s\n", Melder_double (* (double *) resource -> value)); break;
			case stringwa: fprintf (f, "%s\n", resource -> value); break;
		}
	}
	fclose (f);
}

/* End of file Resources.c */
