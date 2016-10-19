#ifndef _praat_FFNet_h_
#define _praat_FFNet_h_
/* praat_FFNet.h
 *
 * Copyright (C) 2016 David Weenink & Paul Boersma
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
	Interface functions for time-based subclasses of Function.
*/

#include "FFNet.h"
#include "praat.h"

void praat_uvafon_FFNet_init ();

/*
	Special request from contrib/Ola, who wants to include these commands in the KNN menu.
*/
extern "C" void NEW1_PatternList_create (UiCallback_ARGS);
extern "C" void NEW1_Categories_create (UiCallback_ARGS);

/* End of file praat_FFNet.h */
#endif
