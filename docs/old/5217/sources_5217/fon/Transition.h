#ifndef _Transition_h_
#define _Transition_h_
/* Transition.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/12
 */

/* Transition inherits from Data */
#ifndef _Matrix_h_
	#include "Matrix.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

/* For the inheritors. */
#define Transition_members Data_members \
	long numberOfStates; \
	wchar_t **stateLabels; \
	double **data;
#define Transition_methods Data_methods
class_create (Transition, Data);

int Transition_init (I, long numberOfStates);
Transition Transition_create (long numberOfStates);

int Transition_formula (I, const wchar_t *formula);
void Transition_drawAsNumbers (I, Graphics g, int iformat, int precision);

int Transition_eigen (Transition me, Matrix *eigenvectors, Matrix *eigenvalues);
Transition Transition_power (Transition me, long power);

Matrix Transition_to_Matrix (Transition me);
Transition Matrix_to_Transition (Matrix me);

#endif

/* End of file TableOfReal.h */
