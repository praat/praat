#ifndef _praat_TimeFunction_h_
#define _praat_TimeFunction_h_
/* praat_TimeFunction.h
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

void praat_TimeFunction_query_init (ClassInfo klas);   // Query buttons for time-based subclasses of Function.
void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time-based subclasses of Function.

/* End of file praat_TimeFunction.h */
#endif
