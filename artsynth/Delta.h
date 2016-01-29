#ifndef _Delta_h_
#define _Delta_h_
/* Delta.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Thing.h"

typedef struct structDelta_Tube *Delta_Tube;
struct structDelta_Tube
{
	/* Structure: static. */

	Delta_Tube left1;   /* If null: closed at left edge. */
	Delta_Tube left2;   /* If not null: two merging streams. */
	Delta_Tube right1;  /* If null: radiation at right edge. */
	Delta_Tube right2;  /* If not null: a stream splitting into two. */
	long parallel;   /* Parallel subdivision. */

	/* Controlled by articulation: quasistatic. */

	double Dxeq, Dyeq, Dzeq;
	double mass, k1, k3, Brel, s1, s3, dy;
	double k1left1, k1left2, k1right1, k1right2;   /* Linear coupling factors. */
	double k3left1, k3left2, k3right1, k3right2;   /* Cubic coupling factors. */

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
	int numberOfTubes;              // >= 1
	struct structDelta_Tube *tube;  // tube [1..numberOfTubes]

	void v_destroy () noexcept
		override;
};

void Delta_init (Delta me, int numberOfTubes);

autoDelta Delta_create (int numberOfTubes);
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
