#ifndef _praat_TimeTier_h_
#define _praat_TimeTier_h_
/* praat_TimeTier.h
 *
 * Copyright (C) 2016 Paul Boersma
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

/*
	Interface functions for time-based subclasses of AnyTier.
*/

#include "AnyTier.h"
#include "praat_TimeFunction.h"

/*
	Action buttons.
*/
void praat_TimeTier_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of AnyTier.
void praat_TimeTier_modify_init (ClassInfo klas);   // Modification buttons for time-based subclasses of AnyTier.

/* End of file praat_TimeTier.h */
#endif
