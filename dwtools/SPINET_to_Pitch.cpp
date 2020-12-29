/* SPINET_to_Pitch.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
*/

#include "SPINET_to_Pitch.h"
#include "Pitch_extensions.h"
#include "NUM2.h"

/*
	from Erb-scale to log2-scale
*/

autoPitch SPINET_to_Pitch (SPINET me, double harmonicFallOffSlope, double ceiling, integer maxnCandidates) {
	try {
		constexpr integer nPointsPerOctave = 48;
		const double fmin = NUMerbToHertz (SampledXY_indexToY (me, 1L));
		const double fmax = NUMerbToHertz (SampledXY_indexToY (me, my ny));
		const double fminl2 = NUMlog2 (fmin), fmaxl2 = NUMlog2 (fmax);
		const double points = (fmaxl2 - fminl2) * nPointsPerOctave;
		const double dfl2 = (fmaxl2 - fminl2) / (points - 1);
		const integer numberOfFrequencyPoints = Melder_ifloor (points);
		const integer maxHarmonic = Melder_ifloor (fmax / fmin);
		const double unvoicedCriterium = 0.45;

		Melder_require (numberOfFrequencyPoints > 1,
			U"Frequency range too small.");
		Melder_require (fmin < ceiling,
			U"The centre frequency of the lowest filter should be smaller than the ceiling.");

		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, ceiling, maxnCandidates);
		autoVEC power = raw_VEC (my nx);
		autoVEC pitch = raw_VEC (numberOfFrequencyPoints);
		autoVEC sumspec = raw_VEC (numberOfFrequencyPoints);
		autoVEC y = raw_VEC (my ny);
		autoVEC yv2 = raw_VEC (my ny);
		autoVEC fl2 = raw_VEC (my ny);
		/*
			From ERB's to log (f)
		*/
		for (integer i = 1; i <= my ny; i ++) {
			const double f = NUMerbToHertz (my y1 + (i - 1) * my dy);
			fl2 [i] = NUMlog2 (f);
		}
		/*
			Determine global maximum power in frame
		*/
		double maxStrength = 0.0, maxPower = 0.0;
		for (integer j = 1; j <= my nx; j ++) {
			const double p = NUMsum (my s.column (j));
			if (p > maxPower)
				maxPower = p;
			power [j] = p;
		}
		Melder_require (maxPower != 0.0,
			U"The sound should not have all amplitudes equal to zero.");

		for (integer j = 1; j <= my nx; j ++) {
			const Pitch_Frame pitchFrame = & thy frames [j];

			pitchFrame -> intensity = power [j] / maxPower;
			y.all() <<= my s.column (j);
			
			NUMcubicSplineInterpolation_getSecondDerivatives (yv2.get(), fl2.get(), y.get(), 1e30, 1e30);
			for (integer k = 1; k <= numberOfFrequencyPoints; k ++) {
				const double f = fminl2 + (k - 1) * dfl2;
				pitch [k] = NUMcubicSplineInterpolation (fl2.get(), y.get(), yv2.get(), f);
				sumspec [k] = 0.0;
			}
			/*
				Formula (8): weighted harmonic summation.
			*/
			for (integer m = 1; m <= maxHarmonic; m ++) {
				const double hm = 1 - harmonicFallOffSlope * NUMlog2 (m);
				const integer kb = 1 + Melder_ifloor (nPointsPerOctave * NUMlog2 (m));   // TODO: what is kb?
				for (integer k = kb; k <= numberOfFrequencyPoints; k ++)
					if (pitch [k] > 0.0)
						sumspec [k - kb + 1] += pitch [k] * hm;
			}
			/*
				Into Pitch object
			*/
			Pitch_Frame_init (pitchFrame, maxnCandidates);
			pitchFrame -> candidates. resize (pitchFrame -> nCandidates = 0);   // !!!!!
			Pitch_Frame_addPitch (pitchFrame, 0, 0, maxnCandidates);   // unvoiced

			for (integer k = 2; k <= numberOfFrequencyPoints - 1; k ++) {
				const double y1 = sumspec [k - 1], y2 = sumspec [k], y3 = sumspec [k + 1];
				if (y2 > y1 && y2 >= y3) {
					const double denum = y1 - 2.0 * y2 + y3, tmp = y3 - 4.0 * y2;
					const double x = dfl2 * (y1 - y3) / (2 * denum);
					const double f = pow (2.0, fminl2 + (k - 1) * dfl2 + x);
					const double strength = (2.0 * y1 * (4.0 * y2 + y3) - y1 * y1 - tmp * tmp) / (8.0 * denum);
					if (strength > maxStrength)
						maxStrength = strength;
					Pitch_Frame_addPitch (pitchFrame, f, strength, maxnCandidates);
				}
			}
		}

		// Scale the pitch strengths

		for (integer j = 1; j <= my nx; j ++) {
			double f0, localStrength;
			Pitch_Frame_getPitch (& thy frames [j], & f0, & localStrength);
			Pitch_Frame_resizeStrengths (& thy frames [j], localStrength / maxStrength, unvoicedCriterium);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Pitch created.");
	}
}

/* End of file SPINET_to_Pitch.cpp */
