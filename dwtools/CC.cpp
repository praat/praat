/* CC.cpp
 *
 * Copyright (C) 1993-2012, 2014-2017 David Weenink
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
 djmw 20011016 removed some causes for compiler warnings
 djmw 20020315 GPL header
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: oo_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 float -> double
 djmw 20080513 CC_getValue
 */

#include "CC.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "CC_def.h"
#include "oo_COPY.h"
#include "CC_def.h"
#include "oo_EQUAL.h"
#include "CC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "CC_def.h"
#include "oo_WRITE_TEXT.h"
#include "CC_def.h"
#include "oo_WRITE_BINARY.h"
#include "CC_def.h"
#include "oo_READ_TEXT.h"
#include "CC_def.h"
#include "oo_READ_BINARY.h"
#include "CC_def.h"
#include "oo_DESCRIPTION.h"
#include "CC_def.h"

Thing_implement (CC, Sampled, 1);

static integer CC_getMaximumNumberOfCoefficientsUsed (CC me) {
	integer numberOfCoefficients = 0;
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		CC_Frame cf = (CC_Frame) & my frame [iframe];
		integer numberOfCoefficients_iframe = cf -> numberOfCoefficients;
		if (numberOfCoefficients_iframe > numberOfCoefficients) {
			numberOfCoefficients = numberOfCoefficients_iframe;
		}
	}
	return numberOfCoefficients;
}

void structCC :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:", xmin, U" to ", xmax, U" seconds");
	MelderInfo_writeLine (U"Number of frames: ", nx);
	MelderInfo_writeLine (U"Time step: ", dx, U" seconds");
	MelderInfo_writeLine (U"First frame at: ", x1, U" seconds");
	MelderInfo_writeLine (U"Maximum number of coefficients possible: ", maximumNumberOfCoefficients);
	MelderInfo_writeLine (U"Maximum number of coefficients used: ", CC_getMaximumNumberOfCoefficientsUsed (this));
}

void CC_Frame_init (CC_Frame me, integer numberOfCoefficients) {
	my c = NUMvector<double> (1, numberOfCoefficients);
	my numberOfCoefficients = numberOfCoefficients;
}

void CC_init (CC me, double tmin, double tmax, integer nt, double dt, double t1, integer maximumNumberOfCoefficients, double fmin, double fmax) {
	my fmin = fmin;
	my fmax = fmax;
	my maximumNumberOfCoefficients = maximumNumberOfCoefficients;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my frame = NUMvector<structCC_Frame> (1, nt);
}

autoMatrix CC_to_Matrix (CC me) {
	try {
		// find number of coefficients by quering all frames.
		// We cannot use maximumNumberOfCoefficiennts because this number is only used to calculate the inverse
		integer numberOfCoefficients = 0;
		for (integer i = 1; i <= my nx; i ++) {
			CC_Frame cf = & my frame [i];
			if (cf -> numberOfCoefficients > numberOfCoefficients) {
				numberOfCoefficients = cf -> numberOfCoefficients;
			}
		}
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, numberOfCoefficients, numberOfCoefficients, 1.0, 1.0);
		
		for (integer i = 1; i <= my nx; i ++) {
			CC_Frame cf = & my frame [i];
			for (integer j = 1; j <= cf -> numberOfCoefficients; j ++) {
				thy z [j] [i] = cf -> c [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

void CC_paint (CC me, Graphics g, double xmin, double xmax, integer cmin, integer cmax, double minimum, double maximum, int garnish) {
	autoMatrix thee = CC_to_Matrix (me);

	Matrix_paintCells (thee.get(), g, xmin, xmax, cmin, cmax, minimum, maximum);

	if (garnish) {
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Coefficients");
	}
}

void CC_drawC0 (CC me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish) {
	(void) garnish;

	if (xmin >= xmax) {
		xmin = my xmin; xmax = my xmax;
	}

	integer bframe, eframe;
	(void) Sampled_getWindowSamples (me, xmin, xmax, & bframe, & eframe);
	autoNUMvector <double> c (bframe, eframe);
	for (integer i = bframe; i <= eframe; i ++) {
		CC_Frame cf = & my frame [i];
		c [i] = cf -> c0;
	}
	if (ymin >= ymax) {
		NUMvector_extrema (c.peek(), bframe, eframe, &ymin, &ymax);
		if (ymax <= ymin) {
			ymin -= 1.0;
			ymax += 1.0;
		}
	} else {
		NUMvector_clip (c.peek(), bframe, eframe, ymin, ymax);
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, c.peek(), bframe, eframe, xmin, xmax);
	Graphics_unsetInner (g);
}

void CC_getNumberOfCoefficients_extrema (CC me, integer startframe, integer endframe, integer *p_min, integer *p_max) {

	Melder_assert (startframe <= endframe);

	if (startframe == 0 && endframe == 0) {
		startframe = 1; endframe = my nx;
	}
	if (startframe < 1) {
		startframe = 1;
	}
	if (endframe > my nx) {
		endframe = my nx;
	}

	integer min = my maximumNumberOfCoefficients;
	integer max = 0;

	for (integer i = startframe; i <= endframe; i ++) {
		CC_Frame f = & my frame [i];
		integer nc = f -> numberOfCoefficients;

		if (nc < min) {
			min = nc;
		} else if (nc > max) {
			max = nc;
		}
	}
	if (p_min) {
		*p_min = min;
	}
	if (p_max) {
		*p_max = max;
	}
}

integer CC_getMinimumNumberOfCoefficients (CC me, integer startframe, integer endframe) {
	integer min, max;

	CC_getNumberOfCoefficients_extrema (me, startframe, endframe, & min, & max);

	return min;
}

integer CC_getMaximumNumberOfCoefficients (CC me, integer startframe, integer endframe) {
	integer min, max;

	CC_getNumberOfCoefficients_extrema (me, startframe, endframe, & min, & max);

	return max;
}

integer CC_getNumberOfCoefficients (CC me, integer iframe) {
	if (iframe < 1 || iframe > my nx) {
		return 0;
	}
	CC_Frame cf = & me -> frame[iframe];
	return cf -> numberOfCoefficients;
}


double CC_getValueInFrame (CC me, integer iframe, integer index) {
	if (iframe < 1 || iframe > my nx) {
		return undefined;
	}
	CC_Frame cf = & me -> frame [iframe];
	return index > cf -> numberOfCoefficients ? undefined : cf -> c [index];
}

double CC_getValue (CC me, double t, integer index) {
	integer iframe = Sampled_xToNearestIndex (me, t);
	if (iframe < 1 || iframe > my nx) {
		return undefined;
	}
	CC_Frame cf = & me -> frame [iframe];
	return index > cf -> numberOfCoefficients ? undefined : cf -> c [index];
}

double CC_getC0ValueInFrame (CC me, integer iframe) {
	if (iframe < 1 || iframe > my nx) {
		return undefined;
	}
	CC_Frame cf = & me -> frame [iframe];
	return cf -> c0;
}

#if 0
double CC_getC0ValueAtTime (CC me, double t) {
	integer iframe = Sampled_xToNearestIndex (me, t);
	return CC_getC0ValueInFrame (me, iframe);
}
#endif

/* End of file CC.cpp */
