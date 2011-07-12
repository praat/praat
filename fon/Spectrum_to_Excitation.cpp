/* Spectrum_to_Excitation.cpp
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

/*
 * pb 1995/05/20
 * pb 2002/07/16 GPL
 * pb 2011/06/10 C++
 */

#include "Spectrum_to_Excitation.h"

Excitation Spectrum_to_Excitation (I, double dbark) {
	iam (Spectrum);
	try {
		long nbark = (int) floor (25.6 / dbark + 0.5);
		double *re = my z [1], *im = my z [2]; 

		autoNUMvector <double> auditoryFilter (1, nbark);
		double filterArea = 0;
		for (long i = 1; i <= nbark; i ++) {
			double bark = dbark * (i - nbark/2) + 0.474;
			filterArea += auditoryFilter [i] = pow (10, (1.581 + 0.75 * bark - 1.75 * sqrt (1 + bark * bark)));
		}
		/*for (long i = 1; i <= nbark; i ++)
			auditoryFilter [i] /= filterArea;*/
		autoNUMvector <double> rFreqs (1, nbark + 1);
		autoNUMvector <long> iFreqs (1, nbark + 1);
		for (long i = 1; i <= nbark + 1; i ++) {
			rFreqs [i] = Excitation_barkToHertz (dbark * (i - 1));
			iFreqs [i] = Sampled_xToNearestIndex (me, rFreqs [i]);
		}
		autoNUMvector <double> inSig (1, nbark);
		for (long i = 1; i <= nbark; i ++) {
			long low = iFreqs [i], high = iFreqs [i + 1] - 1;
			if (low < 1) low = 1;
			if (high > my nx) high = my nx;
			for (long j = low; j <= high; j ++)
				inSig [i] += re [j] * re [j] + im [j] * im [j];   // Pa2 s2

			/* An anti-undersampling correction. */
			if (high >= low)
				inSig [i] *= 2 * (rFreqs [i + 1] - rFreqs [i]) / (high - low + 1) * my dx;   // Pa2: power density in this band
		}

		/* Convolution with auditory (masking) filter. */

		autoNUMvector <double> outSig (1, 2 * nbark);
		for (long i = 1; i <= nbark; i ++)
			for (long j = 1; j <= nbark; j ++)
				outSig [i + j] += inSig [i] * auditoryFilter [j];

		autoExcitation thee = Excitation_create (dbark, nbark);
		for (long i = 1; i <= nbark; i ++)
			thy z [1] [i] = Excitation_soundPressureToPhon (sqrt (outSig [i + nbark/2]), Sampled_indexToX (thee.peek(), i));

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Excitation.");
	}
}

/* End of file Spectrum_to_Excitation.cpp */
