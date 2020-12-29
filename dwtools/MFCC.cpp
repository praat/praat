/* MFCC.cpp
 *
 * Mel Frequency Cepstral Coefficients class.
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
 * djmw 20020813 GPL header
 * djmw 20110304 Thing_new
*/

#include "MFCC.h"
#include "Spectrogram_extensions.h"
#include "NUM2.h"


Thing_implement (MFCC, CC, 1);

void structMFCC :: v_info () {
	structCC :: v_info ();
	MelderInfo_writeLine (U"Minimum frequency: ", fmin, U" mel");
	MelderInfo_writeLine (U"Maximum frequency: ", fmax, U" mel");
}

autoMFCC MFCC_create (double tmin, double tmax, integer nt, double dt, double t1, integer maximumNumberOfCoefficients, double fmin_mel, double fmax_mel) {
	try {
		autoMFCC me = Thing_new (MFCC);
		CC_init (me.get(), tmin, tmax, nt, dt, t1, maximumNumberOfCoefficients, fmin_mel, fmax_mel);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MFCC not created.");
	}
}

void MFCC_lifter (MFCC me, integer lifter) {
	try {
		Melder_assert (lifter > 0);
		autoVEC c = zero_VEC (my maximumNumberOfCoefficients);
		for (integer icoef = 1; icoef <= my maximumNumberOfCoefficients; icoef ++)
			c [icoef] = (1 + lifter / 2 * sin (NUMpi * icoef / lifter));   // BUG ?
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			CC_Frame cf = & my frame [iframe];
			for (integer icoef = 1; icoef <= cf -> numberOfCoefficients; icoef ++)
				cf -> c [icoef] *= c [icoef];
		}
	} catch (MelderError) {
		Melder_throw (me, U": not lifted.");
	}
}

autoTableOfReal MFCC_to_TableOfReal (MFCC me, bool includeC0) {
	try {
		integer numberOfColumns = my maximumNumberOfCoefficients + ( includeC0 ? 1 : 0 );
		autoTableOfReal thee = TableOfReal_create (my nx, numberOfColumns);
		for (integer i = 1; i <= numberOfColumns; i ++)
			TableOfReal_setColumnLabel (thee.get(), i, Melder_cat (U"c", ( includeC0 ? i - 1 : i )));
		const integer offset = ( includeC0 ? 1 : 0 );
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const CC_Frame cf = & my frame [iframe];
			for (integer j = 1; j <= cf -> numberOfCoefficients; j ++)
				thy data [iframe] [j + offset] = cf -> c [j];
			if (includeC0)
				thy data [iframe] [1] = cf -> c0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TabelOfReal.");
	}
}

// as_Sound not to_Sound
autoSound MFCC_to_Sound (MFCC me) {
	try {
		autoSound thee = Sound_create (my maximumNumberOfCoefficients, my xmin, my xmax, my nx, my dx, my x1);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const CC_Frame cf = & my frame [iframe];
			thy z.column (iframe) <<= cf -> c.all();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not represented as Sound.");
	}
}

autoSound MFCCs_crossCorrelate (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		Melder_require (my dx == thy dx,
			U"The samplings of the two MFCC's should be equal.");
		Melder_require (my maximumNumberOfCoefficients == thy maximumNumberOfCoefficients,
			U"The number of coefficients in the two MFCC's should be equal.");
		autoSound target = MFCC_to_Sound (me);
		autoSound source = MFCC_to_Sound (thee);
		autoSound cc = Sounds_crossCorrelate (target.get(), source.get(), scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (U"No cross-correlation between ", me, U" and ", thee, U" calculated.");
	}
}

autoSound MFCCs_convolve (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		Melder_require (my dx == thy dx,
			U"The samplings of the two MFCC's should be equal.");
		Melder_require (my maximumNumberOfCoefficients == thy maximumNumberOfCoefficients,
			U"The number of coefficients in the two MFCC's should be equal.");
		autoSound target = MFCC_to_Sound (me);
		autoSound source = MFCC_to_Sound (thee);
		autoSound cc = Sounds_convolve (target.get(), source.get(), scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not convolved.");
	}
}

static double CC_Frames_distance (CC_Frame me, CC_Frame thee, bool includeEnergy) {
	double dist = 0;
	if (includeEnergy) {
		const double d0 = my c0 - thy c0;
		dist += d0 * d0;
	}
	for (integer i = 1; i <= my numberOfCoefficients; i ++) {
		const double di = my c [i] - thy c [i];
		dist += di * di;
	}
	return sqrt (dist);
}

/*
	1: cepstral difference function (d)
	2: spectral stability (dstab)
	3: spectral center of gravity (gs)
	4: stable internal duration
 */
autoMatrix MFCC_to_Matrix_features (MFCC me, double windowLength, bool includeEnergy) {
	try {
		const integer nw = Melder_ifloor (windowLength / my dx / 2.0);
		autoMelSpectrogram him = MFCC_to_MelSpectrogram (me, 0, 0, 1);
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 4.0, 4, 1.0, 1.0);
		thy z [1] [1] = thy z [1] [my nx] = 0;  // first & last frame
		for (integer iframe = 2; iframe <= my nx - 1; iframe ++) {
			const CC_Frame cfi = & my frame [iframe];
			// 1. cepstral difference
			integer nwi = ( iframe > nw ? nw : iframe - 1 );
			if (nwi > my nx - iframe)
				nwi = my nx - iframe;
			double numer = 0.0;
			for (integer j = 1; j <= nwi; j ++)
				numer += (double) j * (double) j;
			numer *= 2.0;
			double dsq = 0.0;
			if (includeEnergy) {
				longdouble sumj = 0.0;
				for (integer j = 1; j <= nwi; j ++) {
					const CC_Frame cfp = & my frame [iframe + j];
					const CC_Frame cfm = & my frame [iframe - j];
					sumj += j * (cfp -> c0 - cfm -> c0);
				}
				sumj /= numer;
				dsq += sumj * sumj;
			}
			for (integer i = 1; i <= my maximumNumberOfCoefficients; i ++) {
				longdouble sumj = 0.0;
				for (integer j = 1; j <= nwi; j ++) {
					const CC_Frame cfp = & my frame [iframe + j];
					const CC_Frame cfm = & my frame [iframe - j];
					sumj += j * (cfp -> c [j] - cfm -> c [j]);
				}
				sumj /= numer;
				dsq += sumj * sumj;
			}
			thy z [1] [iframe] = dsq;
			
			// 2: spectral stability (dstab)
			const CC_Frame cfp = & my frame [iframe + 1];
			const CC_Frame cfm = & my frame [iframe - 1];
			const double dim1 = CC_Frames_distance (cfi, cfm, includeEnergy);
			const double dip1 = CC_Frames_distance (cfi, cfp, includeEnergy);
			thy z [2] [iframe] = (dim1 + dip1) / 2.0;
			
			// 3: spectral centere of gravity (gs)
			longdouble msm = 0.0, sm = 0.0;
			for (integer j = 1; j <= his ny; j ++) {
				sm += his z [j] [iframe];
				msm += j * his z [j] [iframe];
			}
			const double gs = ( sm == 0.0 ? 0.0 : (double) (msm / sm) );
			thy z [3] [iframe] = gs;
			
			// 4: stable internal duration
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no features calculated.");
	}
}

/* End of file MFCC.cpp */
