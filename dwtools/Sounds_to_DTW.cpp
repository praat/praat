/* Sounds_to_DTW.cpp
 *
 * Copyright (C) 2012-2019 David Weenink
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

#include "Sound_to_MFCC.h"
#include "Sounds_to_DTW.h"
#include "CCs_to_DTW.h"

autoDTW Sounds_to_DTW (Sound me, Sound thee, double analysisWidth, double dt, double band, int slope) {
	try {
		constexpr integer numberOfCoefficients = 12;
		constexpr double fmin_mel = 100.0, df_mel = 100.0, fmax_mel = 0.0;
//TRACE
trace(1);
		autoMFCC mfcc_me = Sound_to_MFCC (me, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
trace(2);
		autoMFCC mfcc_thee = Sound_to_MFCC (thee, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
trace(3);
		constexpr double wc = 1.0, wle = 0.0, wr = 0.0, wer = 0.0, dtr = 0.0;
		autoDTW him = CCs_to_DTW (mfcc_me.get(), mfcc_thee.get(), wc, wle, wr, wer, dtr);
trace(4);
		autoPolygon p = DTW_to_Polygon (him.get(), band, slope);
trace(5);
		DTW_Polygon_findPathInside (him.get(), p.get(), slope, 0);
trace(6);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no DTW created.");
	}
}

/* End of file Sounds_to_DTW.cpp */
