/* Cepstrum_and_Spectrum.cpp
 *
 * Copyright (C) 1994-2017 David Weenink
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
 djmw 20020812 GPL header
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
 djmw 20121015
*/

#include "Cepstrum_and_Spectrum.h"
#include "NUM2.h"
#include "Spectrum_extensions.h"
#include "Sound_and_Spectrum.h"

autoPowerCepstrum Spectrum_to_PowerCepstrum (Spectrum me) {
	try {
		autoSpectrum dBspectrum = Data_copy (me);
		VEC re = dBspectrum -> z.row (1), im = dBspectrum -> z.row (2);
		for (integer i = 1; i <= dBspectrum -> nx; i ++) {
			re [i] = log (re [i] * re [i] + im [i] * im [i] + 1e-300);
			im [i] = 0.0;
		}
		autoSound cepstrum = Spectrum_to_Sound (dBspectrum.get());
		autoPowerCepstrum thee = PowerCepstrum_create (0.5 / my dx, my nx);
		for (integer i = 1; i <= thy nx; i ++) {
			double val = cepstrum -> z [1] [i];
			thy z [1] [i] = val * val;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

autoCepstrum Spectrum_to_Cepstrum (Spectrum me) {
	try {
		autoSpectrum dBspectrum = Data_copy (me);
		VEC re = dBspectrum -> z.row (1), im = dBspectrum -> z.row (2);
		for (integer i = 1; i <= dBspectrum -> nx; i ++) {
			re [i] = log (re [i] * re [i] + im [i] * im [i] + 1e-300);
			im [i] = 0.0;
		}
		autoSound cepstrum = Spectrum_to_Sound (dBspectrum.get());
		autoCepstrum thee = Cepstrum_create (0.5 / my dx, my nx);
		for (integer i = 1; i <= thy nx; i ++) {
			double val = cepstrum -> z [1] [i];
			thy z [1] [i] = val;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

autoSpectrum Cepstrum_to_Spectrum (Cepstrum me) { //TODO power cepstrum
	try {
		autoCepstrum cepstrum = Data_copy (me);
		cepstrum ->  z [1] [1] = my z [1] [1];
		for (integer i = 2; i <= cepstrum -> nx; i ++) {
			cepstrum -> z [1] [i] = 2 * my z [1] [i];
		}
		autoSpectrum thee = Sound_to_Spectrum ((Sound) cepstrum.get(), true);

		VEC re = thy z.row (1), im = thy z.row (2);
		for (integer i = 1; i <= thy nx; i ++) {
			re [i] =  exp (0.5 * re [i]);   // i.e., sqrt (exp(re [i]))
			im [i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum created.");
	}
}

/* End of file Cepstrum_and_Spectrum.cpp  */
