/* Cochleagram.c
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
 * pb 1995/07/24
 * pb 2002/07/16 GPL
 * pb 2007/03/17 domain quantity
 */

#include "Graphics.h"
#include "Cochleagram.h"

class_methods (Cochleagram, Matrix)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

Cochleagram Cochleagram_create (double tmin, double tmax, long nt, double dt, double t1,
	double df, long nf)
{
	Cochleagram me = new (Cochleagram);
	if (! me || ! Matrix_init (me, tmin, tmax, nt, dt, t1,
		0.0, nf * df, nf, df, 0.5 * df)) forget (me);
	return me;
}

void Cochleagram_paint (Cochleagram me, Graphics g, double tmin, double tmax, int garnish)
{
	static float border [1 + 12] =
		{ 0, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };
	Cochleagram copy = Data_copy (me);
	long iy, ix, itmin, itmax;
	if (! copy) return;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
	for (iy = 2; iy < my ny; iy ++)
		for (ix = itmin; ix <= itmax; ix ++)
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
	forget (copy);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "Place (Bark)");
		Graphics_marksLeftEvery (g, 1.0, 5.0, 1, 1, 0);
	}
}

double Cochleagram_difference (Cochleagram me, Cochleagram thee, double tmin, double tmax)
{
	long itime, ifreq, itmin, itmax, nt;
	double diff = 0.0;
	if (my nx != thy nx || my dx != thy dx || my x1 != thy x1 || my ny != thy ny)
		return Melder_error ("Cochleagram_difference: unequal time sampling or number of frequencies.");
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! (nt = Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax)))
		return Melder_error ("Cochleagram_difference: window too short.");
	for (itime = itmin; itime <= itmax; itime ++)
		for (ifreq = 1; ifreq <= my ny; ifreq ++)
		{
			double d = my z [ifreq] [itime] - thy z [ifreq] [itime];
			diff += d * d;
		}
	diff /= nt * my ny;
	return sqrt (diff);
}

Cochleagram Matrix_to_Cochleagram (Matrix me)
{
	Cochleagram thee = Cochleagram_create (my xmin, my xmax, my nx, my dx, my x1,
		my dy, my ny);
	if (! thee) return NULL;
	NUMfmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	return thee;
}

Matrix Cochleagram_to_Matrix (Cochleagram me)
{
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
					my ymin, my ymax, my ny, my dy, my y1);
	if (! thee) return NULL;
	NUMfmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	return thee;
}

/* End of file Cochleagram.c */
