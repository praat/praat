/* Spectrogram8.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 2000/09/24
 * pb 2002/07/16 GPL
 */

#include <time.h>
#include "Spectrogram8.h"
#include "Matrix.h"

static void info (I) {
	iam (Spectrogram8);
	Melder_information (
		"Number of time slices: %ld\nNumber of frequencies: %ld\n"
		"Time step: %.8g\nHighest frequency: %.8g",
		my nx, my ny, my dx, my ymax); 
}

class_methods (Spectrogram8, Image)
	class_method (info)
class_methods_end

Spectrogram8 Spectrogram8_create (double tmin, double tmax, long nt, double dt, double t1,
					double fmin, double fmax, long nf, double df, double f1)
{
	Spectrogram8 me = new (Spectrogram8);
	if (! me || ! Image_init (me, tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1))
		forget (me);
	return me;    
}

void Spectrogram8_paintInside (I, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double minimum_d, double maximum_d)
{
	iam (Spectrogram8);
	long itmin, itmax, ifmin, ifmax;
	int minimum, maximum;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (fmax <= fmin) { fmin = my ymin; fmax = my ymax; }
	if (! Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) ||
		 ! Matrix_getWindowSamplesY (me, fmin - 0.49999 * my dy, fmax + 0.49999 * my dy, & ifmin, & ifmax))
		return;
	if (minimum_d >= maximum_d) {
		minimum = 0;
		maximum = 255;
	} else {
		minimum = minimum_d * 2 + 40;
		maximum = maximum_d * 2 + 40;
		if (minimum < 0) minimum = 0; else if (minimum > 255) minimum = 255;
		if (maximum < 0) maximum = 0; else if (maximum > 255) maximum = 255;
	}
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_image8 (g, my z,
		itmin, itmax,
		Matrix_columnToX (me, itmin - 0.5),
		Matrix_columnToX (me, itmax + 0.5),
		ifmin, ifmax,
		Matrix_rowToY (me, ifmin - 0.5),
		Matrix_rowToY (me, ifmax + 0.5),
		minimum, maximum);
}

void Spectrogram8_paint (I, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	double minimum, double maximum,
	int garnish)
{
	iam (Spectrogram8);
	Graphics_setInner (g);
	Spectrogram8_paintInside (me, g, tmin, tmax, fmin, fmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "Frequency (Hz)");
	}
}

/* End of Spectrogram8.c */ 
