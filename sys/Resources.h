#ifndef _Resources_h_
#define _Resources_h_
/* Resources.h
 *
 * Copyright (C) 1996-2002 Paul Boersma
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
 */

#ifndef _melder_h_
	#include "melder.h"
#endif

void Resources_addByte (const char *string, signed char *value);
void Resources_addShort (const char *string, short *value);
void Resources_addInt (const char *string, int *value);
void Resources_addLong (const char *string, long *value);
void Resources_addUbyte (const char *string, unsigned char *value);
void Resources_addUshort (const char *string, unsigned short *value);
void Resources_addUint (const char *string, unsigned int *value);
void Resources_addUlong (const char *string, unsigned long *value);
void Resources_addChar (const char *string, char *value);
void Resources_addFloat (const char *string, float *value);
void Resources_addDouble (const char *string, double *value);
void Resources_addString (const char *string, char *value);

void Resources_read (MelderFile file);
void Resources_write (MelderFile file);

/* End of file Resources.h */
#endif
