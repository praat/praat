#ifndef _NUMlinprog_h_
#define _NUMlinprog_h_
/* NUMlinprog.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

/********** Linear programming **********/

typedef struct structNUMlinprog *NUMlinprog;
void NUMlinprog_delete (NUMlinprog me);
NUMlinprog NUMlinprog_new (bool maximize);
void NUMlinprog_addVariable (NUMlinprog me, double lowerBound, double upperBound, double coeff);
void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound);
void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient);
void NUMlinprog_run (NUMlinprog me);
double NUMlinprog_getPrimalValue (NUMlinprog me, integer ivar);

/* End of file NUMlinprog.h */
#endif
