/* Harmonicity.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2015-2020 Paul Boersma
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

#include "Graphics.h"
#include "Harmonicity.h"

Thing_implement (Harmonicity, Vector, 2);

static autoVEC Harmonicity_getSoundingValues (Harmonicity me, double tmin, double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer imin, imax;
	integer numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (numberOfFrames < 1)
		return autoVEC();
	autoVEC soundingValues = raw_VEC (numberOfFrames);
	integer numberOfSoundingFrames = 0;
	for (integer iframe = imin; iframe <= imax; iframe ++)
		if (my z [1] [iframe] != -200.0)
			soundingValues [++ numberOfSoundingFrames] = my z [1] [iframe];
	if (numberOfSoundingFrames < 1)
		return autoVEC();
	soundingValues.size = numberOfSoundingFrames;   // shrink (without reallocation)
	return soundingValues;
}

double Harmonicity_getMean (Harmonicity me, double tmin, double tmax) {
	autoVEC soundingValues = Harmonicity_getSoundingValues (me, tmin, tmax);
	return NUMmean (soundingValues.get());
}

double Harmonicity_getStandardDeviation (Harmonicity me, double tmin, double tmax) {
	autoVEC soundingValues = Harmonicity_getSoundingValues (me, tmin, tmax);
	return NUMstdev (soundingValues.get());
}

double Harmonicity_getQuantile (Harmonicity me, double quantile) {
	autoVEC soundingValues = Harmonicity_getSoundingValues (me, 0.0, 0.0);
	sort_VEC_inout (soundingValues.get());
	return NUMquantile (soundingValues.get(), quantile);
}

void structHarmonicity :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	autoVEC soundingValues = Harmonicity_getSoundingValues (this, 0.0, 0.0);
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", our nx, U" (", soundingValues.size, U" sounding)");
	MelderInfo_writeLine (U"   Time step: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", our x1, U" seconds");
	if (soundingValues.size > 0) {
		MelderInfo_writeLine (U"Periodicity-to-noise ratios of sounding frames:");
		sort_VEC_inout (soundingValues.get());
		MelderInfo_writeLine (U"   Median ", Melder_single (NUMquantile (soundingValues.get(), 0.50)), U" dB");
		MelderInfo_writeLine (U"   10 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.10)), U" dB   90 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.90)), U" dB");
		MelderInfo_writeLine (U"   16 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.16)), U" dB   84 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.84)), U" dB");
		MelderInfo_writeLine (U"   25 % = ", Melder_single (NUMquantile (soundingValues.get(), 0.25)), U" dB   75 %% = ",
				Melder_single (NUMquantile (soundingValues.get(), 0.75)), U" dB");
		MelderInfo_writeLine (U"Minimum: ", Melder_single (soundingValues [1]), U" dB");
		MelderInfo_writeLine (U"Maximum: ", Melder_single (soundingValues [soundingValues.size]), U" dB");
		MelderGaussianStats stats = NUMmeanStdev (soundingValues.all());
		MelderInfo_writeLine (U"Average: ", Melder_single (stats.mean), U" dB");
		if (soundingValues.size > 1)
			MelderInfo_writeLine (U"Standard deviation: ", Melder_single (stats.stdev), U" dB");
	}
}

autoHarmonicity Harmonicity_create (double tmin, double tmax, integer nt, double dt, double t1) {
	try {
		autoHarmonicity me = Thing_new (Harmonicity);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Harmonicity not created.");
	}
}

autoMatrix Harmonicity_to_Matrix (Harmonicity me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to Matrix.");
	}
}

autoHarmonicity Matrix_to_Harmonicity (Matrix me) {
	try {
		autoHarmonicity thee = Harmonicity_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"not converted to Harmonicity.");
	}
}

/* End of file Harmonicity.cpp */
