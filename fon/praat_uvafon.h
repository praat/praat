#ifndef _praat_uvafon_h_
#define _praat_uvafon_h_
/* praat_uvafon.h
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

#include "praat.h"

void praat_uvafon_init ();
void praat_uvafon_Sound_init ();
void praat_uvafon_TextGrid_init ();

/*
	Functions that make fields in Praat's forms look similarly in all packages.
*/
int praat_get_frequencyRange (UiForm dia, double *fmin, double *fmax);

void praat_TableOfReal_init (ClassInfo klas);   // Buttons for TableOfReal and for its subclasses.

void praat_TimeFrameSampled_query_init (ClassInfo klas);   // Query buttons for frame-based time-based subclasses of Sampled.

#endif
/* End of file praat_uvafon.h */
