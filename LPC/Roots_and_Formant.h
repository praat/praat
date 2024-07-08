#ifndef _Roots_and_Formant_h_
#define _Roots_and_Formant_h_
/* Roots_and_Formant.h
 *
 * Copyright (C) 1994-2024 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Formant.h"
#include "Roots.h"

void Roots_into_Formant_Frame (constRoots me, Formant_Frame thee, double samplingFrequency, double margin);

#endif /* _Roots_and_Formant_h_ */
