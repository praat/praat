/* Excitation_to_Formant.cpp
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
 * pb 1995/08/23
 * pb 2002/07/16 GPL
 * pb 2008/01/19 double
 * pb 2011/05/24 C++
 */

#include "Excitation_to_Formant.h"

Formant Excitation_to_Formant (Excitation me, int maxnFormants) {
	try {
		long nfreq = my nx, nform = 0;
		double *p = my z [1];

		autoFormant thee = Formant_create (0, 1, 1, 1, 0.5, maxnFormants);
		thy d_frames [1]. formant = NUMvector <structFormant_Formant> (1, maxnFormants);
		for (long i = 2; i < nfreq; i ++)
			if (p [i] > p [i - 1] && p [i] >= p [i + 1]) {
				double min3phon, left, right;
				double firstDerivative = p [i+1] - p [i-1], secondDerivative = 2 * p [i] - p [i-1] - p [i+1];
				long j;
				Formant_Formant formant = & thy d_frames [1]. formant [++ nform];
				formant -> frequency = Excitation_barkToHertz (
					my x1 + my dx * (i - 1 + 0.5 * firstDerivative / secondDerivative));
				min3phon = p [i] + 0.125 * firstDerivative * firstDerivative / secondDerivative - 3.0;
				/* Search left. */
				j = i - 1; while (p [j] > min3phon && j > 1) j --;
				left = Excitation_barkToHertz (
					p [j] > min3phon ? my xmin : my x1 + my dx * (j - 1 + (min3phon - p [j]) / (p [j + 1] - p [j])));
				 /* Search right. */
				j = i + 1; while (p [j] > min3phon && j < nfreq) j ++;
				right = Excitation_barkToHertz (
					p [j] > min3phon ? my xmax : my x1 + my dx * (j - 1 - (min3phon - p [j]) / (p [j - 1] - p [j])));
				formant -> bandwidth = right - left;
				if (nform == thy maxnFormants) break;
			}
		thy d_frames [1]. nFormants = nform;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Formant.");
	}
}

/* End of file Excitation_to_Formant.cpp */
