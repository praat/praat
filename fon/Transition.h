#ifndef _Transition_h_
#define _Transition_h_
/* Transition.h
 *
 * Copyright (C) 1992-2011,2012,2015 Paul Boersma
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

#include "Matrix.h"
#include "Graphics.h"

#include "Transition_def.h"
oo_CLASS_CREATE (Transition, Daata);

void Transition_init (Transition me, long numberOfStates);
autoTransition Transition_create (long numberOfStates);

void Transition_formula (Transition me, const char32 *formula);
void Transition_drawAsNumbers (Transition me, Graphics g, int iformat, int precision);

void Transition_eigen (Transition me, autoMatrix *eigenvectors, autoMatrix *eigenvalues);
autoTransition Transition_power (Transition me, long power);

autoMatrix Transition_to_Matrix (Transition me);
autoTransition Matrix_to_Transition (Matrix me);

/* End of file Transition.h */
#endif
