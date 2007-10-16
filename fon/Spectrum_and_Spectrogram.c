/* Spectrum_and_Spectrogram.c
 *
 * Copyright (C) 1992-2002 David Weenink & Paul Boersma
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
 * pb 1998/04/18
 * pb 2002/07/16 GPL
 */

#include "Spectrum_and_Spectrogram.h"

Spectrum Spectrogram_to_Spectrum (I, double tim) {
	iam (Spectrogram);
	Spectrum thee = Spectrum_create (my ymax, my ny);
	long itime, ifreq;
	if (! thee) goto error;
	/* Override stupid Spectrum values. */
	thy xmin = my ymin;
	thy xmax = my ymax;
	thy x1 = my y1;   /* Centre of first band, instead of 0 (makes it unFFTable). */
	thy dx = my dy;   /* Frequency step. */
	itime = Sampled_xToIndex (me, tim);
	if (itime < 1 ) itime = 1;
	if (itime > my nx) itime = my nx;
	for (ifreq = 1; ifreq <= my ny; ifreq ++) {
		double value = my z [ifreq] [itime];
		if (value < 0.0) { Melder_error1 (L"Negative values in spectrogram."); goto error; }
		thy z [1] [ifreq] = sqrt (value);
		thy z [2] [ifreq] = 0.0;
	}
	return thee;
error:
	forget (thee);
	return Melder_errorp ("(Spectrogram_to_Spectrum:) Not performed.");
}

Spectrogram Spectrum_to_Spectrogram (I) {
	iam (Spectrum);
	long i;
	Spectrogram thee = Spectrogram_create (0, 1, 1, 1, 0.5, my xmin, my xmax, my nx, my dx, my x1);
	if (! thee) return NULL;
	for (i = 1; i <= my nx; i ++)
		thy z [i] [1] = my z [1] [i] * my z [1] [i] + my z [2] [i] * my z [2] [i];
	return thee;
}

/* End of file Spectrum_and_Spectrogram.c */
