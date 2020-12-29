/* Sound_and_MixingMatrix.cpp
 *
 * Copyright (C) 2010-2019 David Weenink
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

void Sound_MixingMatrix_playPart (Sound me, MixingMatrix thee, double fromTime, double toTime, Sound_PlayCallback callback, Thing boss) {
	try {
		autoSound mix = Sound_MixingMatrix_mixPart (me, thee, fromTime, toTime);
		Sound_playPart (mix.get(), fromTime, toTime, callback, boss);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void Sound_MixingMatrix_play (Sound me, MixingMatrix thee, Sound_PlayCallback callback, Thing boss) {
	Sound_MixingMatrix_playPart (me, thee, my xmin, my xmax, callback, boss);
}

autoSound Sound_MixingMatrix_mix (Sound me, MixingMatrix thee) {
	return Sound_MixingMatrix_mixPart (me, thee, my xmin, my xmax);
}

autoSound Sound_MixingMatrix_mixPart (Sound me, MixingMatrix thee, double fromTime, double toTime) {
	try {
		Melder_require (my ny == thy numberOfColumns,
			U"The number of inputs in the MixingMatrix and the number of channels in the Sound should be equal.");
		
		if (fromTime == toTime) {
			fromTime = my xmin;
			toTime = my xmax;
		}
		/*
			Determine index range.
			We use all the real or virtual samples that fit within [fromTime..toTime].
		*/
		const integer ix1 = 1 + Melder_iceiling ((fromTime - my x1) / my dx);
		const integer ix2 = 1 + Melder_ifloor ((toTime - my x1) / my dx);
		Melder_require (ix1 <= ix2,
			U"Mixed Sound would contain no samples.");

		autoSound him = Sound_create (thy numberOfRows, fromTime, toTime, ix2 - ix1 + 1, my dx, my x1 + (ix1 - 1) * my dx);
		/*
		       1          nx                             1          nx
		       |..........|                              |..........|                 (me)
		   |-----------|---|                                |-----|----------|        (index in me)
		  ix1         ix2  ix2                              ix1   ix2        ix2
		  1           'nx' 'nx'
		  Example:   (1)     (2)                                  (3)        (4)
		  New sound: him_nx = ix2 - ix1 + 1
		  Example: nx = 12
		  (1) copy from [1,  ix2] to [2-ix1, 1 - ix1 + ix2]
		 		ix1=-3, ix2=8 [1,8] -> [5,12] 
		  (2) copy from [1,   nx] to [2-ix1, 1 - ix1 +  nx]
		  		ix1=-3, ix2=13 [1,12] -> [5,16] 
		  (3) copy from [ix1,ix2] to [1    , ix2 - ix1 + 1]
		  		ix1=4, ix2=10 [4,10] -> [1,7]
		  (4) copy from [ix1, nx] to [1    , nx - ix1 + 1]
		  		ix1=4, ix2=21 [4,12] -> [1,9]
		*/
		if (! (toTime < my xmin || fromTime > my xmax)) {
			integer off1 = 0, off2 = 0;
			integer to_i1 = 1, to_i2 = his nx;
			if (ix1 < 1) { // (1) + (2)
				off1 = 1 - ix1;
				to_i1 = 1 - ix1;
				to_i2 = to_i1 + my nx; // (2)
				if (ix2 < my nx) // (1)
					to_i2 = 1 - ix1 + ix2;
			} else { // (3) + (4)
				off2 = ix1 - 1;
				to_i2 = to_i1 + ix2 - ix1; // (3)
				if (ix2 > my nx) // (4)
					to_i2 = his nx;
			}
			for (integer i = 1; i <= thy numberOfRows; i ++) {
				for (integer ichan = 1; ichan <= my ny; ichan ++) {
					const double mixingCoeffient = thy data [i] [ichan];
					if (mixingCoeffient != 0.0) {
						for (integer j = 1; j <= to_i2 - to_i1 + 1; j ++) {
							//to [j] += mixingCoeffient * from [j];
							his z [i] [j + off1] += mixingCoeffient * my z [ichan] [j + off2];
						}
					}
				}
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not mixed.");
	}
}

autoSound Sound_MixingMatrix_unmix (Sound me, MixingMatrix thee) {
	try {
		Melder_require (my ny == thy numberOfColumns,
			U"The number of inputs in the MixingMatrix and the number of channels in the Sound should be equal.");

		autoMAT minv = newMATpseudoInverse (thy data.get(), 0.0);
		autoSound him = Sound_create (thy numberOfColumns, my xmin, my xmax, my nx, my dx, my x1);
		mul_MAT_out (his z.get(), minv.get(), my z.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not unmixed.");
	}
}

/* End of file Sound_and_MixingMatrix.cpp */
