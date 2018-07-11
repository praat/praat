#ifndef _Speaker_h_
#define _Speaker_h_
/* Speaker.h
 *
 * Copyright (C) 1992-2005,2007,2011,2012,2015-2018 Paul Boersma
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

#include "Data.h"

#include "Speaker_def.h"

autoSpeaker Speaker_create (conststring32 kindOfSpeaker, int16 numberOfVocalCordMasses);
	/* Preconditions:								*/
	/*    1 <= numberOfVocalCordMasses <= 2;					*/
	/* Failures:									*/
	/*    Kind of speaker is not one of "Female", "Male", or "Child".		*/

/* End of file Speaker.h */
#endif
