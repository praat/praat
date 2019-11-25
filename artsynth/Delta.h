#ifndef _Delta_h_
#define _Delta_h_
/* Delta.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015-2017,2019 Paul Boersma
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

#include "Thing.h"

typedef struct structDelta_Tube *Delta_Tube;
struct structDelta_Tube
{
	/* Structure: static. */

	Delta_Tube left1;   // if null: closed at left edge
	Delta_Tube left2;   // if not null: two merging streams
	Delta_Tube right1;  // if null: radiation at right edge
	Delta_Tube right2;  // if not null: a stream splitting into two
	integer parallel;   // parallel subdivision

	/* Controlled by articulation: quasistatic. */

	double Dxeq, Dyeq, Dzeq;
	double mass, k1, k3, Brel, s1, s3, dy;
	double k1left1, k1left2, k1right1, k1right2;   // linear coupling factors
	double k3left1, k3left2, k3right1, k3right2;   // cubic coupling factors

	/* Dynamic. */

	double Jhalf, Jleft, Jleftnew, Jright, Jrightnew;
	double Qhalf, Qleft, Qleftnew, Qright, Qrightnew;
	double Dx, Dxnew, dDxdt, dDxdtnew, Dxhalf;
	double Dy, Dynew, dDydt, dDydtnew;
	double Dz;
	double A, Ahalf, Anew, V, Vnew;
	double e, ehalf, eleft, eleftnew, eright, erightnew, ehalfold;
	double p, phalf, pleft, pleftnew, pright, prightnew;
	double Kleft, Kleftnew, Kright, Krightnew, Pturbright, Pturbrightnew;
	double B, r, R, DeltaP, v;
};

Thing_define (Delta, Thing) {
	integer numberOfTubes;           // >= 1
	autovector <structDelta_Tube> tubes;
};

void Delta_init (Delta me, integer numberOfTubes);

autoDelta Delta_create (integer numberOfTubes);
/*
	Function:
		return a new Delta.
	Preconditions:
		numberOfTubes >= 1;
	Postconditions:
		result -> numberOfTubes = numberOfTubes;
		all members of result -> tube [1..numberOfTubes] are zero or null,
		except 'parallel', which is 1.
*/

/* End of file Delta.h */
#endif
