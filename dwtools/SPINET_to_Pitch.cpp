/* SPINET_to_Pitch.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19970408
 djmw 20020813 GPL header
 djmw 20021106 Latest modification
*/

#include "SPINET_to_Pitch.h"
#include "Pitch_extensions.h"
#include "NUM2.h"

/*
	from Erb-scale to log2-scale
*/

autoPitch SPINET_to_Pitch (SPINET me, double harmonicFallOffSlope, double ceiling, integer maxnCandidates) {
	try {
		integer nPointsPerOctave = 48;
		double fmin = NUMerbToHertz (SampledXY_indexToY (me, 1L));
		double fmax = NUMerbToHertz (SampledXY_indexToY (me, my ny));
		double fminl2 = NUMlog2 (fmin), fmaxl2 = NUMlog2 (fmax);
		double points = (fmaxl2 - fminl2) * nPointsPerOctave;
		double dfl2 = (fmaxl2 - fminl2) / (points - 1);
		integer nFrequencyPoints = Melder_ifloor (points);
		integer maxHarmonic = Melder_ifloor (fmax / fmin);
		double maxStrength = 0.0, unvoicedCriterium = 0.45, maxPower = 0.0;

		Melder_require (nFrequencyPoints > 1, U"Frequency range too small.");
		Melder_require (fmin < ceiling, U"The centre frequency of the lowest filter should be smaller than the ceiling.");

		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, ceiling, maxnCandidates);
		autoNUMvector<double> power (1, my nx);
		autoNUMvector<double> pitch (1, nFrequencyPoints);
		autoNUMvector<double> sumspec (1, nFrequencyPoints);
		autoNUMvector<double> y (1, my ny);
		autoNUMvector<double> yv2 (1, my ny);
		autoNUMvector<double> fl2 (1, my ny);

		// From ERB's to log (f)

		for (integer i = 1; i <= my ny; i ++) {
			double f = NUMerbToHertz (my y1 + (i - 1) * my dy);
			fl2 [i] = NUMlog2 (f);
		}

		// Determine global maximum power in frame

		for (integer j = 1; j <= my nx; j ++) {
			double p = 0.0;
			for (integer i = 1; i <= my ny; i ++) {
				p += my s [i] [j];
			}
			if (p > maxPower) {
				maxPower = p;
			}
			power [j] = p;
		}
		Melder_require (maxPower != 0.0, U"The sound should not have all amplitudes equal to zero.");

		for (integer j = 1; j <= my nx; j ++) {
			Pitch_Frame pitchFrame = &thy frame [j];

			pitchFrame -> intensity = power [j] / maxPower;
			for (integer i = 1; i <= my ny; i ++) {
				y [i] = my s [i] [j];
			}
			NUMcubicSplineInterpolation_getSecondDerivatives (fl2.peek(), y.peek(), my ny, 1e30, 1e30, yv2.peek());
			for (integer k = 1; k <= nFrequencyPoints; k ++) {
				double f = fminl2 + (k - 1) * dfl2;
				pitch [k] = NUMcubicSplineInterpolation (fl2.peek(), y.peek(), yv2.peek(), my ny, f);
				sumspec [k] = 0.0;
			}

			// Formula (8): weighted harmonic summation.

			for (integer m = 1; m <= maxHarmonic; m ++) {
				double hm = 1 - harmonicFallOffSlope * NUMlog2 (m);
				integer kb = 1 + Melder_ifloor (nPointsPerOctave * NUMlog2 (m));
				for (integer k = kb; k <= nFrequencyPoints; k ++) {
					if (pitch [k] > 0.0) {
						sumspec [k - kb + 1] += pitch [k] * hm;
					}
				}
			}

			// into Pitch object

			Pitch_Frame_init (pitchFrame, maxnCandidates);
			pitchFrame -> nCandidates = 0; /* !!!!! */
			Pitch_Frame_addPitch (pitchFrame, 0, 0, maxnCandidates); /* unvoiced */

			for (integer k = 2; k <= nFrequencyPoints - 1; k ++) {
				double y1 = sumspec [k - 1], y2 = sumspec [k], y3 = sumspec [k + 1];
				if (y2 > y1 && y2 >= y3) {
					double denum = y1 - 2.0 * y2 + y3, tmp = y3 - 4.0 * y2;
					double x = dfl2 * (y1 - y3) / (2 * denum);
					double f = pow (2.0, fminl2 + (k - 1) * dfl2 + x);
					double strength = (2.0 * y1 * (4.0 * y2 + y3) - y1 * y1 - tmp * tmp) / (8.0 * denum);
					if (strength > maxStrength) {
						maxStrength = strength;
					}
					Pitch_Frame_addPitch (pitchFrame, f, strength, maxnCandidates);
				}
			}
		}

		// Scale the pitch strengths

		for (integer j = 1; j <= my nx; j ++) {
			double f0, localStrength;
			Pitch_Frame_getPitch (&thy frame [j], &f0, &localStrength);
			Pitch_Frame_resizeStrengths (&thy frame [j], localStrength / maxStrength, unvoicedCriterium);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Pitch created.");
	}
}

/* End of file SPINET_to_Pitch.cpp */
