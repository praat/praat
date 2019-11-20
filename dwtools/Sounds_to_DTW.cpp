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

/*
 djmw 20120221
*/

#include "Sound_to_MFCC.h"
#include "Sounds_to_DTW.h"
#include "CCs_to_DTW.h"

autoDTW Sounds_to_DTW (Sound me, Sound thee, double analysisWidth, double dt, double band, int slope) {
	try {
        constexpr integer numberOfCoefficients = 12;
        const double fmin_mel = 100, df_mel = 100, fmax_mel = 0.0;
		autoMFCC mfcc_me = Sound_to_MFCC (me, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
		autoMFCC mfcc_thee = Sound_to_MFCC (thee, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
        constexpr double wc = 1.0, wle = 0.0, wr = 0.0, wer = 0.0, dtr = 0.0;
        autoDTW him = CCs_to_DTW (mfcc_me.get(), mfcc_thee.get(), wc, wle, wr, wer, dtr);
        autoPolygon p = DTW_to_Polygon (him.get(), band, slope);
        DTW_Polygon_findPathInside (him.get(), p.get(), slope, 0);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no DTW created.");
	}
}

/* End of file Sounds_to_DTW.cpp */
