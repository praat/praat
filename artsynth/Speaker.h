#ifndef _Speaker_h_
#define _Speaker_h_
/* Speaker.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1995/10/15
 * pb 2002/07/16 GPL
 */

#ifndef _Data_h_
	#include "Data.h"
#endif

#include "Speaker_def.h"
#define Speaker_methods Data_methods
oo_CLASS_CREATE (Speaker, Data);

Speaker Speaker_create (char *kindOfSpeaker, int numberOfVocalCordMasses);
	/* Preconditions:								*/
	/*    1 <= numberOfVocalCordMasses <= 2;					*/
	/* Failures:									*/
	/*    Kind of speaker is not one of "Female", "Male", or "Child".		*/

/* End of file Speaker.h */
#endif
