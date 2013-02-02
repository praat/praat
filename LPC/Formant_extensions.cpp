/* Formant_extensions.c
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
*/

#include "Formant_extensions.h"
#include "NUM2.h"

void Formant_formula (Formant me, double tmin, double tmax, long formantmin, long formantmax, Interpreter interpreter, wchar_t *expression) {
	try {
		long numberOfPossibleFormants = my maxnFormants;
		if (tmax <= tmin) {
			tmin = my xmin; tmax = my xmax;
		}
		if (formantmax >= formantmin) {
			formantmin = 1; formantmax = numberOfPossibleFormants;
		}
		formantmin = formantmin < 1 ? 1 : formantmin;
		formantmax = formantmax > numberOfPossibleFormants ? numberOfPossibleFormants : formantmax;

		autoMatrix fb = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 2 * numberOfPossibleFormants, 2 * numberOfPossibleFormants, 1.0, 1.0);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			int numberOfFormants = frame -> nFormants < numberOfPossibleFormants ? frame -> nFormants : numberOfPossibleFormants;
			for (long iformant = 1; iformant <= numberOfFormants; iformant++) {
				if (iformant <= frame -> nFormants) {
					fb -> z[2 * iformant - 1][iframe] = frame -> formant[iformant].frequency;
					fb -> z[2 * iformant    ][iframe] = frame -> formant[iformant].bandwidth;
				}
			}
		}
		// Apply formula
		double ymin = 2 * formantmin - 1, ymax = 2 * formantmax;
		Matrix_formula_part (fb.peek(), tmin, tmax, ymin, ymax, expression, interpreter, NULL);
		// Put results back in Formant
		long ixmin, ixmax, iymin, iymax;
		(void) Matrix_getWindowSamplesX (fb.peek(), tmin, tmax, & ixmin, & ixmax);
		(void) Matrix_getWindowSamplesY (fb.peek(), ymin, ymax, & iymin, & iymax);

		for (long iframe = ixmin; iframe <= ixmax; iframe++) {
			// if some of the formant frequencies are set to zero => remove the formant
			Formant_Frame frame = & my d_frames [iframe];
			int numberOfFormants = frame -> nFormants < formantmax ? frame -> nFormants : formantmax;
			int iformantto = formantmin > 1 ? formantmin - 1 : 0;
			for (long iformant = formantmin; iformant <= numberOfFormants; iformant++) {
				double frequency = fb -> z[2 * iformant - 1][iframe];
				double bandWidth = fb -> z[2 * iformant    ][iframe];
				if (frequency > 0 && bandWidth > 0) {
					iformantto++;
					frame -> formant[iformantto].frequency = frequency;
					frame -> formant[iformantto].bandwidth = bandWidth;
				} else {
					frame -> formant[iformant].frequency = frame -> formant[iformant].bandwidth = 0;
				}
			}
			// shift the (higher) formants down if necessary.
			for (long iformant = formantmax + 1; iformant <= frame -> nFormants; iformant++) {
				double frequency = fb -> z[2 * iformant - 1][iframe];
				double bandWidth = fb -> z[2 * iformant    ][iframe];
				if (frequency > 0 && bandWidth > 0) {
					iformantto++;
					frame -> formant[iformantto].frequency = frequency;
					frame -> formant[iformantto].bandwidth = bandWidth;
				} else {
					frame -> formant[iformant].frequency = frame -> formant[iformant].bandwidth = 0;
				}
			}
			frame ->  nFormants = iformantto;
		}
	} catch (MelderError) {
		Melder_throw (me, ": not filtered.");
	}
}

IntensityTier Formant_and_Spectrogram_to_IntensityTier (Formant me, Spectrogram thee, long iformant) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("The start and end times of the Formant and the Spectrogram must be equal.");
		}
		if (iformant < 1 || iformant > my maxnFormants) {
			Melder_throw ("Formant number not in range [1, ", my maxnFormants, "].");
		}
		autoIntensityTier him = IntensityTier_create (my xmin, my xmax);
		double previousValue = -80000; // can never occur
		double previousTime = my xmin;
		for (long iframe = 1; iframe <= my nx; iframe++) {
			Formant_Frame frame = & my d_frames [iframe];
			long numberOfFormants = frame -> nFormants;
			double time = Sampled_indexToX (me, iframe);
			double value = 0;
			if (iformant <= numberOfFormants) {
				double f = frame -> formant[iformant].frequency;
				value = Matrix_getValueAtXY (thee, time, f);
				value = value == NUMundefined ? 0.0 : value;
			}
			value = 10.0 * log10 ((value + 1e-30) / 4.0e-10); /* dB / Hz */
			if (value != previousValue) {
				if (iframe > 1 && previousTime < time - 1.5 * my dx) { // mark the end of the same interval
					RealTier_addPoint (him.peek(), time - my dx, previousValue);
				}
				RealTier_addPoint (him.peek(), time, value);
				previousTime = time;
			}
			previousValue = value;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("IntensityTier not created from ", me, " and ", thee, ".");
	}
}

/* End of file Formant_extensions.cpp */
