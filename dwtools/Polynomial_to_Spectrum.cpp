/* Polynomial_to_Spectrum.h
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 */

#include "Polynomial_to_Spectrum.h"

static void Polynomial_evaluate_z_cart (Polynomial me, double r, double phi, double *out_re, double *out_im) {
	longdouble rn = 1.0;

	longdouble re = my coefficients [1];
	longdouble im = 0.0;
	if (r == 0.0)
		return;
	for (integer i = 2; i <= my numberOfCoefficients; i ++) {
		rn *= r;
		const longdouble arg = (i - 1) * phi;
		re += my coefficients [i] * rn * cos (arg);
		im += my coefficients [i] * rn * sin (arg);
	}
	if (out_re)
		*out_re = (double) re;
	if (out_im)
		*out_im = (double) im;
}

/* Can be speeded up by doing a FFT */
autoSpectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency, integer numberOfFrequencies, double radius) {
	try {
		Melder_require (numberOfFrequencies > 1,
			U"Number of frequencies should be greater than 1.");
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfFrequencies);

		const double phi = NUMpi / (numberOfFrequencies - 1);
		for (integer i = 1; i <= numberOfFrequencies; i ++) {
			double re, im;
			Polynomial_evaluate_z_cart (me, radius, (i - 1) * phi, & re, & im);
			thy z [1] [i] = re;
			thy z [2] [i] = im;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum calculated.");
	}
}

/* End of file Polynomial_to_Spectrum.cpp */
