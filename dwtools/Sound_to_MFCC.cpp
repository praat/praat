/* Sound_to_MFCC.cpp
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
 djmw 20010410
 djmw 20020813 GPL header
*/

#include "Sound_to_MFCC.h"
#include "Sound_and_Spectrogram_extensions.h"

autoMFCC Sound_to_MFCC (Sound me, integer numberOfCoefficients, double analysisWidth, double dt, double f1_mel, double fmax_mel, double df_mel) {
	try {
		autoMelSpectrogram mf = Sound_to_MelSpectrogram (me, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
		autoMFCC mfcc = MelSpectrogram_to_MFCC (mf.get(), numberOfCoefficients);
		return mfcc;
	} catch (MelderError) {
		Melder_throw (me, U": no MFCC created.");
	}
}

/* End of file Sound_to_MFCC.cpp */
