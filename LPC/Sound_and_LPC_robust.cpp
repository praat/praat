/* Sound_and_LPC_robust.cpp
 *
 * Copyright (C) 1994-2024 David Weenink
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
	djmw 20030814 First version
*/

#include "LPC_and_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound_extensions.h"

autoFormant Sound_to_Formant_robust (Sound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double halfdt_window, double preEmphasisFrequency, double safetyMargin, double k, integer itermax, double tol, bool wantlocation) {
	const double dt = dt_in > 0.0 ? dt_in : halfdt_window / 4.0;
	const double nyquist = 0.5 / my dx;
	const integer predictionOrder = Melder_ifloor (2 * numberOfFormants);
	try {
		autoSound sound;
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1.0) < 1.0e-12)
			sound = Data_copy (me);   // will be modified
		else
			sound = Sound_resample (me, maximumFrequency * 2.0, 50);

		autoLPC lpc = Sound_to_LPC_autocorrelation (sound.get(), predictionOrder, halfdt_window, dt, preEmphasisFrequency);
		autoLPC lpcRobust = LPC_and_Sound_to_LPC_robust (lpc.get(), sound.get(), halfdt_window, preEmphasisFrequency, k, itermax, tol, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcRobust.get(), safetyMargin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}

/* End of file Sound_and_LPC_robust.cpp */
