#ifndef _Articulation_h_
#define _Articulation_h_
/* Articulation.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

/* Art = Articulation */
/* Members represent muscle activities for speech production. */
/* All members have values from 0 (no activity) to 1 (maximum activity). */

#include "Data.h"

#include "Articulation_enums.h"

#include "Articulation_def.h"
oo_CLASS_CREATE (Art, Data);

Art Art_create (void);
/*
	Return value:
		an array of double, with indices from enum Art.
	Postconditions:
		result -> art [kArt_muscle_LUNGS] == 0.0;
		...
		result -> art [kArt_muscle_BUCCINATOR] = 0.0;
*/

/* End of file Articulation.h */
#endif
