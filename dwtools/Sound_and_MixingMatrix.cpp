/* Sound_and_MixingMatrix.cpp
 *
 * Copyright (C) 2010-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Interpreter.h"
#include "NUM2.h"
#include "Sound_and_MixingMatrix.h"

void Sound_and_MixingMatrix_playPart (Sound me, MixingMatrix thee, double fromTime, double toTime, Sound_PlayCallback callback, Thing boss) {
	try {
		autoSound part = Sound_extractPart (me, fromTime, toTime, kSound_windowShape_RECTANGULAR, 1.0, true);
		autoSound mix = Sound_and_MixingMatrix_mix (part.get(), thee);
		Sound_playPart (mix.get(), fromTime, toTime, callback, boss);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void Sound_and_MixingMatrix_play (Sound me, MixingMatrix thee, Sound_PlayCallback callback, Thing boss) {
	Sound_and_MixingMatrix_playPart (me, thee, my xmin, my xmax, callback, boss);
}

autoSound Sound_and_MixingMatrix_mix (Sound me, MixingMatrix thee) {
	try {
		if (my ny != thy numberOfColumns) {
			Melder_throw (U"The number of inputs in the MixingMatrix and the number of channels in the Sound must be equal.");
		}
		autoSound him = Sound_create (thy numberOfRows, my xmin, my xmax, my nx, my dx, my x1);
		for (long i = 1; i <= thy numberOfRows; i++) {
			for (long j = 1; j <= my nx; j++) {
				double mix = 0;
				for (long k = 1; k <= my ny; k++) {
					mix += thy data[i][k] * my z[k][j];
				}
				his z[i][j] = mix;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not mixed.");
	}
}

autoSound Sound_and_MixingMatrix_unmix (Sound me, MixingMatrix thee) {
	try {
		if (my ny != thy numberOfRows) {
			Melder_throw (U"The MixingMatrix and the Sound must have the same number of channels.");
		}

		autoNUMmatrix<double> minv (1, thy numberOfColumns, 1, thy numberOfRows);
		NUMpseudoInverse (thy data, thy numberOfRows, thy numberOfColumns, minv.peek(), 0);
		autoSound him = Sound_create (thy numberOfColumns, my xmin, my xmax, my nx, my dx, my x1);
		for (long i = 1; i <= thy numberOfColumns; i++) {
			for (long j = 1; j <= my nx; j++) {
				double s = 0;
				for (long k = 1; k <= my ny; k++) {
					s += minv[i][k] * my z[k][j];
				}
				his z[i][j] = s;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not unmixed.");
	}
}

/* End of file Sound_and_MixingMatrix.cpp */
