/* Cepstrum.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
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
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrum.h"
#include "NUM2.h"
#include "Vector.h"

#include "enums_getText.h"
#include "Cepstrum_enums.h"
#include "enums_getValue.h"
#include "Cepstrum_enums.h"

Thing_implement (Cepstrum, Matrix, 2);

double structCepstrum :: v_getValueAtSample (integer isamp, integer which, int units) {
	if (which == 1) {
		if (units == 0)
			return z [1] [isamp];
		else
			return 20.0 * log10 (fabs (z [1] [isamp]) + 1e-30); // dB's
	}
	return undefined;
}

autoCepstrum Cepstrum_create (double qmax, integer nq) {
	try {
		autoCepstrum me = Thing_new (Cepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrum not created.");
	}
}

void Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, bool power, bool garnish) {
	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin;
		qmax = my xmax;
	}

	integer imin, imax;
	integer numberOfSelected = Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax);
	if (numberOfSelected == 0)
		return;
	autoVEC y = raw_VEC (numberOfSelected);
	for (integer i = 1; i <= numberOfSelected; i ++)
		y [i] = my v_getValueAtSample (imin + i - 1, 1, (power ? 1 : 0));

	if (minimum >= maximum) // autoscaling
		NUMextrema (y.get(), & minimum, & maximum);
	else
		VECclip_inplace (minimum, y.get(), maximum);
	if (maximum == minimum) {
		maximum += 1.0;
		minimum -= 1.0;
	}
	Graphics_setWindow (g, qmin, qmax, minimum, maximum);
	Graphics_function (g, y.asArgumentToFunctionThatExpectsOneBasedArray(), 1, numberOfSelected, Matrix_columnToX (me, imin), Matrix_columnToX (me, imax));

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Quefrency (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, power ? U"Amplitude (dB)" : U"Amplitude");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, bool garnish) {
	Cepstrum_draw (me, g, qmin, qmax, minimum, maximum, false, garnish);
}

/* End of file Cepstrum.cpp */
