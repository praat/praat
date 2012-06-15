/* Cochleagram.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Graphics.h"
#include "Cochleagram.h"

Thing_implement (Cochleagram, Matrix, 2);

Cochleagram Cochleagram_create (double tmin, double tmax, long nt, double dt, double t1, double df, long nf) {
	try {
		autoCochleagram me = Thing_new (Cochleagram);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, 0.0, nf * df, nf, df, 0.5 * df);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Cochleagram with ", nt, " times and ", nf, " frequencies not created.");
	}
}

void Cochleagram_paint (Cochleagram me, Graphics g, double tmin, double tmax, int garnish) {
	static double border [1 + 12] =
		{ 0, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };
	try {
		autoCochleagram copy = Data_copy (me);
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
		long itmin, itmax;
		Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
		for (long iy = 2; iy < my ny; iy ++)
			for (long ix = itmin; ix <= itmax; ix ++)
				if (my z [iy] [ix] > my z [iy - 1] [ix] &&
					my z [iy] [ix] > my z [iy + 1] [ix])
				{
					copy -> z [iy - 1] [ix] += 10;
					copy -> z [iy] [ix] += 10;
					copy -> z [iy + 1] [ix] += 10;
				}
		Graphics_setInner (g);
		Graphics_setWindow (g, tmin, tmax, 0, my ny * my dy);
		Graphics_grey (g, copy -> z,
			itmin, itmax, Matrix_columnToX (me, itmin), Matrix_columnToX (me, itmax),
			1, my ny, 0.5 * my dy, (my ny - 0.5) * my dy,
			12, border);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_textBottom (g, 1, L"Time (s)");
			Graphics_marksBottom (g, 2, 1, 1, 0);
			Graphics_textLeft (g, 1, L"Place (Bark)");
			Graphics_marksLeftEvery (g, 1.0, 5.0, 1, 1, 0);
		}
	} catch (MelderError) {
		Melder_clearError ();   // BUG
	}
}

double Cochleagram_difference (Cochleagram me, Cochleagram thee, double tmin, double tmax) {
	try {
		if (my nx != thy nx || my dx != thy dx || my x1 != thy x1)
			Melder_throw (L"Unequal time samplings.");
		if (my ny != thy ny)
			Melder_throw (L"Unequal numbers of frequencies.");
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
		long itmin, itmax;
		long nt = Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
		if (nt == 0)
			Melder_throw ("Window too short.");
		double diff = 0.0;
		for (long itime = itmin; itime <= itmax; itime ++) {
			for (long ifreq = 1; ifreq <= my ny; ifreq ++) {
				double d = my z [ifreq] [itime] - thy z [ifreq] [itime];
				diff += d * d;
			}
		}
		diff /= nt * my ny;
		return sqrt (diff);
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": difference not computed.");
	}
}

Cochleagram Matrix_to_Cochleagram (Matrix me) {
	try {
		autoCochleagram thee = Cochleagram_create (my xmin, my xmax, my nx, my dx, my x1, my dy, my ny);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Cochleagram.");
	}
}

Matrix Cochleagram_to_Matrix (Cochleagram me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of file Cochleagram.cpp */
