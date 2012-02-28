/* Sounds_to_DTW.cpp
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
 djmw 20120221
*/

#include "Sound_to_MFCC.h"
#include "Sounds_to_DTW.h"
#include "CCs_to_DTW.h"

DTW Sounds_to_DTW (Sound me, Sound thee, double analysisWidth, double dt, double band, int slope) {
	try {
        long numberOfCoefficients = 12;
        double fmin_mel = 100, df_mel = 100, fmax_mel = 0.0;
		autoMFCC mfcc_me = Sound_to_MFCC (me, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
		autoMFCC mfcc_thee = Sound_to_MFCC (thee, numberOfCoefficients, analysisWidth, dt, fmin_mel, fmax_mel, df_mel);
        double wc = 1, wle = 0, wr = 0, wer = 0, dtr = 0;
        autoDTW him = CCs_to_DTW (mfcc_me.peek(), mfcc_thee.peek(), wc, wle, wr, wer, dtr);
        autoPolygon p = DTW_to_Polygon (him.peek(), band, slope);
        DTW_and_Polygon_findPathInside (him.peek(), p.peek(), slope, 0);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no DTW created.");
	}
}

/* End of file Sounds_to_DTW.cpp */
