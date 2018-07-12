#ifndef _Transition_h_
#define _Transition_h_
/* Transition.h
 *
 * Copyright (C) 1992-2011,2012,2015,2017 Paul Boersma
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

#include "Matrix.h"
#include "Graphics.h"

#include "Transition_def.h"

void Transition_init (Transition me, integer numberOfStates);
autoTransition Transition_create (integer numberOfStates);

void Transition_formula (Transition me, conststring32 formula);
void Transition_drawAsNumbers (Transition me, Graphics g, int iformat, int precision);

void Transition_eigen (Transition me, autoMatrix *eigenvectors, autoMatrix *eigenvalues);
autoTransition Transition_power (Transition me, integer power);

autoMatrix Transition_to_Matrix (Transition me);
autoTransition Matrix_to_Transition (Matrix me);

/* End of file Transition.h */
#endif
