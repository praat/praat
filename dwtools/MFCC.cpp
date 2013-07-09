/* MFCC.cpp
 *
 * Mel Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2013 David Weenink
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
 * djmw 20020813 GPL header
 * djmw 20110304 Thing_new
*/

#include "MelFilter_and_MFCC.h"
#include "NUM2.h"


Thing_implement (MFCC, CC, 1);

MFCC MFCC_create (double tmin, double tmax, long nt, double dt, double t1,
                  long maximumNumberOfCoefficients, double fmin_mel, double fmax_mel) {
	try {
		autoMFCC me = Thing_new (MFCC);
		CC_init (me.peek(), tmin, tmax, nt, dt, t1, maximumNumberOfCoefficients, fmin_mel, fmax_mel);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("MFCC not created.");
	}
}

void MFCC_lifter (MFCC me, long lifter) {
	try {
		Melder_assert (lifter > 0);
		autoNUMvector<double> c (1, my maximumNumberOfCoefficients);
		for (long i = 1; i <= my maximumNumberOfCoefficients; i++) {
			c[i] = (1 + lifter / 2 * sin (NUMpi * i / lifter));
		}

		for (long frame = 1; frame <= my nx; frame++) {
			CC_Frame cf = (CC_Frame) & my frame[frame];
			for (long i = 1; i <= cf -> numberOfCoefficients; i++) {
				cf -> c[i] *= c[i];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not lifted.");
	}
}

TableOfReal MFCC_to_TableOfReal (MFCC me, bool includeC0) {
	try {
		long numberOfColumns = my maximumNumberOfCoefficients + (includeC0 ? 1 : 0);
		autoTableOfReal thee = TableOfReal_create (my nx, numberOfColumns);
		autoMelderString columnLabel;
		for (long i = 1; i <= numberOfColumns; i++) {
			MelderString_append (&columnLabel, L"c", Melder_integer (includeC0 ? i - 1 : i));
			TableOfReal_setColumnLabel (thee.peek(), i, columnLabel.string);
			MelderString_empty (&columnLabel);
		}
		long offset = includeC0 ? 1 : 0;
		for (long iframe = 1; iframe <= my nx; iframe++) {
			CC_Frame cf = (CC_Frame) & my frame[iframe];
			for (long j = 1; j <= cf -> numberOfCoefficients; j++) {
				thy data[iframe][j + offset] = cf -> c[j];
			}
			if (includeC0) {
				thy data[iframe][1] = cf -> c0;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TabelOfReal.");
	}
}

// as_Sound not to_Sound
Sound MFCC_to_Sound (MFCC me) {
	try {
		autoSound thee = Sound_create (my maximumNumberOfCoefficients, my xmin, my xmax, my nx, my dx, my x1);
		for (long iframe = 1; iframe <= my nx; iframe++) {
			CC_Frame cf = (CC_Frame) & my frame[iframe];
			for (long j = 1; j <= my maximumNumberOfCoefficients; j++) {
				thy z[j][iframe] = cf -> c[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not represented as Sound.");
	}
}

Sound MFCCs_crossCorrelate (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		if (my dx != thy dx) {
			Melder_throw ("The samplings of the two MFCC's have to be equal.");
		}
		if (my maximumNumberOfCoefficients != thy maximumNumberOfCoefficients) {
			Melder_throw ("The number of coefficients in the two MFCC's have to be equal.");
		}
		autoSound target = MFCC_to_Sound (me);
		autoSound source = MFCC_to_Sound (thee);
		autoSound cc = Sounds_crossCorrelate (target.peek(), source.peek(), scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw ("No cross-correlation between ", me, " and ", thee, " calculated.");
	}
}

Sound MFCCs_convolve (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		if (my dx != thy dx) {
			Melder_throw ("The samplings of the two MFCC's have to be equal.");
		}
		if (my maximumNumberOfCoefficients != thy maximumNumberOfCoefficients) {
			Melder_throw ("The number of coefficients in the two MFCC's have to be equal.");
		}
		autoSound target = MFCC_to_Sound (me);
		autoSound source = MFCC_to_Sound (thee);
		autoSound cc = Sounds_convolve (target.peek(), source.peek(), scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not convolved.");
	}
}

static double CC_Frames_distance (CC_Frame me, CC_Frame thee, bool includeEnergy) {
	double dist = 0;
	if (includeEnergy) {
		double d0 = my c0 - thy c0;
		dist += d0 * d0;
	}
	for (long i = 1; i <= my numberOfCoefficients; i++) {
		double di = my c[i] - thy c[i];
		dist += di * di;
	}
	return sqrt (dist);
}

/* 1: cepstral difference function (d)
 * 2: spectral stability (dstab)
 * 3: spectral center of gravity (gs)
 * 4: stable internal duration
 */
Matrix MFCC_to_Matrix_features (MFCC me, double windowLength, bool includeEnergy) {
	try {
		long nw = windowLength / my dx / 2;
		autoMelFilter him = MFCC_to_MelFilter (me, 0, 0);
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 4, 4, 1, 1);
		thy z[1][1] = thy z[1][my nx] = 0;  // first & last frame
		for (long iframe = 2; iframe <= my nx - 1; iframe++) {
			CC_Frame cfi = (CC_Frame) & my frame[iframe];
			// 1. cepstral difference
			long nwi = iframe > nw ? nw : iframe - 1;
			nwi = iframe < my nx - nwi ? nwi : my nx - iframe;
			double numer = 0;
			for (long j = 1; j <= nwi; j++) {
				numer += j * j;
			}
			numer *= 2;
			double dsq = 0;
			if (includeEnergy) {
				double sumj = 0;
				for (long j = 1; j <= nwi; j++) {
					CC_Frame cfp = (CC_Frame) & my frame[iframe + j];
					CC_Frame cfm = (CC_Frame) & my frame[iframe - j];
					sumj += j * (cfp -> c0 - cfm -> c0);
				}
				sumj /= numer;
				dsq += sumj * sumj;
			}
			for (long i = 1; i <= my maximumNumberOfCoefficients; i++) {
				double sumj = 0;
				for (long j = 1; j <= nwi; j++) {
					CC_Frame cfp = (CC_Frame) & my frame[iframe + j];
					CC_Frame cfm = (CC_Frame) & my frame[iframe - j];
					sumj += j * (cfp -> c[j] - cfm -> c[j]);
				}
				sumj /= numer;
				dsq += sumj * sumj;
			}
			thy z[1][iframe] = dsq;
			
			// 2: spectral stability (dstab)
			CC_Frame cfp = (CC_Frame) & my frame[iframe + 1];
			CC_Frame cfm = (CC_Frame) & my frame[iframe - 1];
			double dim1 = CC_Frames_distance (cfi, cfm, includeEnergy);
			double dip1 = CC_Frames_distance (cfi, cfp, includeEnergy);
			thy z[2][iframe] = (dim1 + dip1) / 2;
			
			// 3: spectral centere of gravity (gs)
			double msm = 0, sm = 0;
			for (long j = 1; j <= his ny; j++) {
				sm += his z[j][iframe];
				msm += j * his z[j][iframe];
			}
			double gs = sm == 0 ? 0 : msm / sm;
			thy z[3][iframe] = gs;
			
			// 4: stable internal duration
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no features calculated.");
	}

}

/* End of file MFCC.cpp */
