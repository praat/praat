/* Cochleagram.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2012,2015-2020 Paul Boersma
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

#include "Graphics.h"
#include "Cochleagram.h"

Thing_implement (Cochleagram, Matrix, 2);

autoCochleagram Cochleagram_create (double tmin, double tmax, integer nt, double dt, double t1, double df, integer nf) {
	try {
		autoCochleagram me = Thing_new (Cochleagram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, 0.0, nf * df, nf, df, 0.5 * df);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cochleagram with ", nt, U" times and ", nf, U" frequencies not created.");
	}
}

void Cochleagram_paint (Cochleagram me, Graphics g, double tmin, double tmax, bool garnish) {
	static double border [1 + 12]
		{ 0.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0 };
	try {
		autoCochleagram copy = Data_copy (me);
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		integer itmin, itmax;
		Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
		for (integer iy = 2; iy < my ny; iy ++)
			for (integer ix = itmin; ix <= itmax; ix ++)
				if (my z [iy] [ix] > my z [iy - 1] [ix] &&
					my z [iy] [ix] > my z [iy + 1] [ix])
				{
					copy -> z [iy - 1] [ix] += 10.0;
					copy -> z [iy] [ix] += 10.0;
					copy -> z [iy + 1] [ix] += 10.0;
				}
		Graphics_setInner (g);
		Graphics_setWindow (g, tmin, tmax, 0.0, my ny * my dy);
		Graphics_grey (g, copy -> z.part (1, my ny, itmin, itmax),
			Matrix_columnToX (me, itmin), Matrix_columnToX (me, itmax),
			0.5 * my dy, (my ny - 0.5) * my dy,
			12, border);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_textBottom (g, true, U"Time (s)");
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_textLeft (g, true, U"Place (Bark)");
			Graphics_marksLeftEvery (g, 1.0, 5.0, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

double Cochleagram_difference (Cochleagram me, Cochleagram thee, double tmin, double tmax) {
	try {
		if (my nx != thy nx || my dx != thy dx || my x1 != thy x1)
			Melder_throw (U"Unequal time samplings.");
		Melder_require (my ny == thy ny,
			U"Unequal numbers of frequencies.");
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		integer itmin, itmax;
		integer nt = Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
		Melder_require (nt > 0,
			U"Window too short.");
		longdouble diff = 0.0;
		for (integer itime = itmin; itime <= itmax; itime ++) {
			for (integer ifreq = 1; ifreq <= my ny; ifreq ++) {
				double d = my z [ifreq] [itime] - thy z [ifreq] [itime];
				diff += d * d;
			}
		}
		diff /= nt * my ny;
		return sqrt ((double) diff);
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": difference not computed.");
	}
}

autoCochleagram Matrix_to_Cochleagram (Matrix me) {
	try {
		autoCochleagram thee = Cochleagram_create (my xmin, my xmax, my nx, my dx, my x1, my dy, my ny);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Cochleagram.");
	}
}

autoMatrix Cochleagram_to_Matrix (Cochleagram me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of file Cochleagram.cpp */
