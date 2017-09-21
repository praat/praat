/* ArbitrarilySampled.cpp
 *
 * Copyright (C) 2017 David Weenink
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

#include "melder.h"
#include "ArbitrarilySampled.h"
#include "TableOfReal_extensions.h"

#include "oo_DESTROY.h"
#include "ArbitrarilySampled_def.h"
#include "oo_COPY.h"
#include "ArbitrarilySampled_def.h"
#include "oo_EQUAL.h"
#include "ArbitrarilySampled_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ArbitrarilySampled_def.h"
#include "oo_WRITE_TEXT.h"
#include "ArbitrarilySampled_def.h"
#include "oo_WRITE_BINARY.h"
#include "ArbitrarilySampled_def.h"
#include "oo_READ_TEXT.h"
#include "ArbitrarilySampled_def.h"
#include "oo_READ_BINARY.h"
#include "ArbitrarilySampled_def.h"
#include "oo_DESCRIPTION.h"
#include "ArbitrarilySampled_def.h"

Thing_implement (ArbitrarilySampled, Daata, 0);

autonumvec ArbitrarilySampled_columnToNumvec (ArbitrarilySampled me, long icol) {
	if (icol < 1 || icol > my numberOfSamples) {
		Melder_throw (me, U": Column number must be in the range 1..", my numberOfSamples, U".");
	}
	autonumvec v (my numberOfSamples, false);
	for (long irow = 1; irow <= my numberOfSamples; irow ++) {
		v [irow] = my samples -> data [irow] [icol];
	}
	return v;
}

double getTensionFactor (double tension) {
	double tensionFactor = undefined;
	if (tension > 0.0 && tension < 1.0) {
		tensionFactor = sqrt (tension / (1.0 - tension));
	}
	return tensionFactor;
}

void ArbitrarilySampled_biharmonicSplines_getWeights (ArbitrarilySampled me, double tension, numvec weights) {
	Melder_assert (weights.size == my numberOfSamples);
	double tensionFactor = getTensionFactor (tension);
	/*
		1. Calculate the Green matrix G = greensFunction (distance (point i, pount j))
		2. Solve z = G.w for w
	*/
	autonummat g (my numberOfSamples, my numberOfSamples, false);
	for (long i = 1; i <= my numberOfSamples; i ++) {
		for (long j = i + 1; j <= my numberOfSamples; j ++) {
			double distance = my v_getEuclideanDistance (i, j);
			g [i] [j] = g [j] [i] = my v_greensFunction (distance, tensionFactor);
		}
		g [i] [i] = 0.0;
	}
	autonumvec z = ArbitrarilySampled_columnToNumvec (me, my numberOfSamples);
	NUMsolveEquation (g.at, my numberOfSamples, my numberOfSamples, z.at, 0.0, weights.at);
}

double ArbitrarilySampled_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, numvec position) {
	real80 result = 0.0;
	double tensionFactor = getTensionFactor (tension);
	for (long i = 1; i <= my numberOfSamples; i ++) {
		double radialDistance = my v_getEuclideanDistance_pos (i, position);
		result += weights [i] * my v_greensFunction (radialDistance, tensionFactor);
	}
	return (double) result;
}

void structArbitrarilySampled :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of samples: ", numberOfSamples);
	MelderInfo_writeLine (U"Number of dimensions: ", numberOfDimensions);
	for (long i = 1; i <= numberOfDimensions; i ++) {
		const char32 *dimensionText = i == 1 ? U"X" : (i == 2 ? U"Y" : U"Z");
		MelderInfo_writeLine (U"Minimum ", dimensionText, U": ", domain [i] [1]);
		MelderInfo_writeLine (U"Maximum ", dimensionText, U": ", domain [i] [2]);
	}
}

void ArbitrarilySampled_init (ArbitrarilySampled me, long numberOfSamples, long numberOfDimensions) {
	try {
		my numberOfSamples = numberOfSamples;
		my numberOfDimensions = numberOfDimensions;
		autoTableOfReal samples = TableOfReal_create (numberOfSamples, numberOfDimensions + 1);
		autonummat domain (numberOfDimensions, 2, true);
		my samples = samples.move();
		my domain = domain.at;
		domain.at = nullptr;
	} catch (MelderError) {
		Melder_throw (U"ArbitrarilySampled not created.");
	}
}

double ArbitrarilySampled_getEuclideanDistance (ArbitrarilySampled me, long ipoint, long jpoint) {
	return ipoint <= my numberOfSamples && jpoint <= my numberOfSamples ? 
		my v_getEuclideanDistance (ipoint, jpoint) : undefined;
}

double ArbitrarilySampled_getEuclideanDistance_pos (ArbitrarilySampled me, long ipoint, numvec position) {
	return ipoint <= my numberOfSamples ? 
		my v_getEuclideanDistance_pos (ipoint, position) : undefined;
}

Thing_implement (ArbitrarilySampled1D, ArbitrarilySampled, 0);

double structArbitrarilySampled1D :: v_getEuclideanDistance (long ipoint, long jpoint) {
	return fabs (samples -> data [ipoint][1] - samples -> data [jpoint][1]);
}

double structArbitrarilySampled1D :: v_getEuclideanDistance_pos (long ipoint, numvec position) {
	return fabs (samples -> data [ipoint][1] - position [1]);
}

double structArbitrarilySampled1D :: v_greensFunction (double radialDistance, double p) {
	return isundef (p) ? radialDistance * radialDistance * radialDistance : 
		exp (- p * radialDistance) + p * radialDistance - 1.0;
}

void ArbitrarilySampled1D_setDomain (ArbitrarilySampled1D me, double xmin, double xmax) {
	if (xmax <= xmin) {
		TableOfReal_getColumnExtrema (my samples.get(), 1, & xmin, & xmax);
	}
	my domain [1][1] = xmin;
	my domain [1][2] = xmax;
}

autoArbitrarilySampled1D ArbitrarilySampled1D_create (long numberOfSamples, double xmin, double xmax) {
	try {
		autoArbitrarilySampled1D me = Thing_new (ArbitrarilySampled1D);
		ArbitrarilySampled_init (me.get(), numberOfSamples, 1);
		my domain [1][1] = xmin;
		my domain [1][2] = xmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"ArbitrarilySampled not created.");
	}
}

double ArbitrarilySampled1D_biharmonicInterpolation (ArbitrarilySampled me, double tension, numvec weights, double x)
{
	autonumvec position (1, false);
	position [1] = x;
	return ArbitrarilySampled_biharmonicSplines_interpolate (me, tension, weights, position.get());
}

Thing_implement (ArbitrarilySampled2D, ArbitrarilySampled, 0);

double structArbitrarilySampled2D :: v_getEuclideanDistance (long ipoint, long jpoint) {
	double dx = samples -> data [ipoint][1] - samples -> data [jpoint][1];
	double dy = samples -> data [ipoint][2] - samples -> data [jpoint][2];
	return sqrt (dx * dx + dy * dy);
}

double structArbitrarilySampled2D :: v_getEuclideanDistance_pos (long ipoint, numvec position) {
	double dx = samples -> data [ipoint][1] - position [1];
	double dy = samples -> data [ipoint][2] - position [2];
	return sqrt (dx * dx + dy * dy);
}

double structArbitrarilySampled2D :: v_greensFunction (double radialDistance, double p) {
	return isundef (p) ? radialDistance * radialDistance * log (radialDistance) : 
		NUMbesselK (0, p * radialDistance) + log (p * radialDistance);
}

void ArbitrarilySampled2D_setDomain (ArbitrarilySampled2D me, double xmin, double xmax, double ymin, double ymax) {
	if (xmax <= xmin) {
		TableOfReal_getColumnExtrema (my samples.get(), 1, & xmin, & xmax);
	}
	my domain [1][1] = xmin;
	my domain [1][2] = xmax;
	if (ymax <= ymin) {
		TableOfReal_getColumnExtrema (my samples.get(), 2, & ymin, & ymax);
	}
	my domain [2][1] = ymin;
	my domain [2][2] = ymax;
}

autoArbitrarilySampled2D ArbitrarilySampled2D_create (long numberOfSamples, double xmin, double xmax, double ymin, double ymax) {
	try {
		autoArbitrarilySampled2D me = Thing_new (ArbitrarilySampled2D);
		ArbitrarilySampled_init (me.get(), numberOfSamples, 2);
		my domain [1] [1] = xmin;
		my domain [1] [2] = xmax;
		my domain [2] [1] = ymin;
		my domain [2] [2] = ymax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"ArbitrarilySampled not created.");
	}
}

double ArbitrarilySampled2D_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, double x, double y) {
	autonumvec position (2, false);
	position [1] = x;
	position [2] = y;
	return ArbitrarilySampled_biharmonicSplines_interpolate (me, tension, weights, position.get());
}

Thing_implement (ArbitrarilySampled3D, ArbitrarilySampled, 0);

double structArbitrarilySampled3D :: v_getEuclideanDistance (long ipoint, long jpoint) {
	double dx = samples -> data [ipoint][1] - samples -> data [jpoint][1];
	double dy = samples -> data [ipoint][2] - samples -> data [jpoint][2];
	double dz = samples -> data [ipoint][3] - samples -> data [jpoint][3];
	return sqrt (dx * dx + dy * dy + dz * dz);
}

double structArbitrarilySampled3D :: v_getEuclideanDistance_pos (long ipoint, numvec position) {
	double dx = samples -> data [ipoint][1] - position [1];
	double dy = samples -> data [ipoint][2] - position [2];
	double dz = samples -> data [ipoint][3] - position [3];
	return sqrt (dx * dx + dy * dy + dz * dz);
}

double structArbitrarilySampled3D :: v_greensFunction (double radialDistance, double p) {
	return isundef (p) ? radialDistance : (exp (p * radialDistance) - 1.0) / (p * radialDistance) + 1.0;
}

void ArbitrarilySampled3D_setDomain (ArbitrarilySampled3D me, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) {
	if (xmax <= xmin) {
		TableOfReal_getColumnExtrema (my samples.get(), 1, & xmin, & xmax);
	}
	my domain [1][1] = xmin;
	my domain [1][2] = xmax;
	if (ymax <= ymin) {
		TableOfReal_getColumnExtrema (my samples.get(), 2, & ymin, & ymax);
	}
	my domain [2][1] = ymin;
	my domain [2][2] = ymax;
	if (zmax <= zmin) {
		TableOfReal_getColumnExtrema (my samples.get(), 3, & zmin, & zmax);
	}
	my domain [3][1] = zmin;
	my domain [3][2] = zmax;
}

autoArbitrarilySampled3D ArbitrarilySampled3D_create (long numberOfSamples, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) {
	try {
		autoArbitrarilySampled3D me = Thing_new (ArbitrarilySampled3D);
		ArbitrarilySampled_init (me.get(), numberOfSamples, 3);
		my domain [1] [1] = xmin;
		my domain [1] [2] = xmax;
		my domain [2] [1] = ymin;
		my domain [2] [2] = ymax;
		my domain [3] [1] = zmin;
		my domain [3] [2] = zmax;

		return me;
	} catch (MelderError) {
		Melder_throw (U"ArbitrarilySampled not created.");
	}
}

double ArbitrarilySampled3D_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, double x, double y, double z) {
	autonumvec position (3, false);
	position [1] = x;
	position [2] = y;
	position [3] = z;
	return ArbitrarilySampled_biharmonicSplines_interpolate (me, tension, weights, position.get());
}

#define ArbitrarilySampled_SCALING_NONE 0
#define ArbitrarilySampled_SCALING_EXTREMES 1
#define ArbitrarilySampled_SCALING_NORMALIZE 2
#define ArbitrarilySampled_SCALING_MAXRADIUS 3

void ArbitrarilySampled_vectorizeColumn (ArbitrarilySampled me, long icol, numvec x) {
	if (x.size <  my numberOfSamples) {
		Melder_throw (U"Numvec dimensions must at least be equal to the number of samples.");
	}
	for (long i = 1; i <= my numberOfSamples; i ++) {
		x [i] = my samples -> data [i] [icol];
	}
}

double ArbitrarilySampled_getMaximumEuclideanDistance (ArbitrarilySampled me) {
	double dmax = undefined;
	if (my numberOfDimensions == 2) { // 1D data
		double xmin, xmax;
		TableOfReal_getColumnExtrema (my samples.get(), 1, & xmin, & xmax);
		dmax = fabs (xmax - xmin);
	} else {
		dmax = 0.0;
		for (long i = 1; i <= my numberOfSamples; i ++) {
			for (long j = i + 1; j <= my numberOfSamples; j ++) {
				double distance = my v_getEuclideanDistance (i, j);
				if (distance > dmax) {
					dmax = distance;
				}
			}
		}
	}
	return dmax;
}


autoArbitrarilySampled ArbitrarilySampled_scaleByExtremes (ArbitrarilySampled me, nummat *p_extremes) {
	try {
		autoArbitrarilySampled thee = Data_copy (me);
		bool save_extremes = p_extremes && p_extremes -> ncol >= my numberOfDimensions && p_extremes -> nrow >= 2;
		for (long icol = 1; icol < my numberOfDimensions; icol ++) {
			double cmin, cmax;
			TableOfReal_getColumnExtrema (my samples.get(), icol, & cmin, & cmax);
			if (cmax > cmin) {
				for (long i = 1; i <= my numberOfSamples; i ++) {
					thy samples -> data [i] [icol] = (thy samples -> data [i][icol] - cmin) / (cmax -cmin);
				}
			}
			if (save_extremes) {
				*p_extremes [1] [icol] = cmin;
				*p_extremes [2] [icol] = cmax;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not scaled.");
	}
}

autoArbitrarilySampled ArbitrarilySampled_scaleByMaxRadius (ArbitrarilySampled me, double *p_maxRadius) {
	try {
		autoArbitrarilySampled thee = Data_copy (me);
		double maxRadius = ArbitrarilySampled_getMaximumEuclideanDistance (me);
		for (long icol = 1; icol < my numberOfDimensions; icol ++) {
			if (maxRadius > 0.0) {
				for (long i = 1; i <= my numberOfSamples; i ++) {
					thy samples -> data [i][icol] /= maxRadius;
				}
			}
		}
		if (p_maxRadius) {
			*p_maxRadius = maxRadius;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not scaled.");
	}
}


void ArbitrarilySampled_scaleData (ArbitrarilySampled me, int scaling) {
	
}


autoMatrix ArbitrarilySampled2D_to_Matrix_biharmonicSplinesInterpolation (ArbitrarilySampled me, double tension, double xmin, double xmax, long nx, double ymin, double ymax, long ny) {
	try {
		if (xmax <= xmin) { // autoscaling
			TableOfReal_getColumnExtrema (my samples.get(), 1, & xmin, & xmax);
		}
		if (ymax <= ymin) { // autoscaling
			TableOfReal_getColumnExtrema (my samples.get(), 2, & ymin, & ymax);
		}
		numvec weights (my numberOfSamples, false), position (2, false);
		ArbitrarilySampled_biharmonicSplines_getWeights (me, tension, weights);
		double dx = (xmax - xmin) / nx, dy = (ymax - ymin) / ny; 
		autoMatrix thee = Matrix_create (xmin, xmax, nx, dx, xmin + 0.5 * dx, ymin, ymax, ny, dy, ymin + 0.5 * dy);
		for (long irow = 1; irow <= ny; irow ++) {
			position [2] = thy y1 + (irow - 1) * dy;
			for (long icol = 1; icol <= nx; icol ++) {
				position [1] = thy x1 + (icol - 1) * dx;
				double z = ArbitrarilySampled_biharmonicSplines_interpolate (me, tension, weights, position);
				thy z [irow] [icol] = z;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": interpolation not finished.");
	}
}

autoArbitrarilySampled1D TableOfReal_to_ArbitrarilySampled1D (TableOfReal me, long xcol, long ycol) {
	try {
		if (xcol < 1 || xcol > my numberOfColumns || ycol < 1 || ycol > my numberOfColumns) {
			Melder_throw (U"The column numbers must in the range 1..", my numberOfColumns, U".");
		}
		autoArbitrarilySampled1D thee = ArbitrarilySampled1D_create (my numberOfRows, 0.0, 1.0);
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			thy samples -> data [irow] [1] = my data [irow] [xcol];
			thy samples -> data [irow] [2] = my data [irow] [ycol];
		}
		ArbitrarilySampled1D_setDomain (thee.get(), 0.0, 0.0); // auto domain
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoArbitrarilySampled2D TableOfReal_to_ArbitrarilySampled2D (TableOfReal me, long xcol, long ycol, long zcol) {
	try {
		if (xcol < 1 || xcol > my numberOfColumns || ycol < 1 || ycol > my numberOfColumns || zcol < 1 || zcol > my numberOfColumns) {
			Melder_throw (U"The column numbers must in the range 1..", my numberOfColumns, U".");
		}
		autoArbitrarilySampled2D thee = ArbitrarilySampled2D_create (my numberOfRows, 0.0, 1.0, 0.0, 1.0); // don'tknow domain yet
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			thy samples -> data [irow] [1] = my data [irow] [xcol];
			thy samples -> data [irow] [2] = my data [irow] [ycol];
			thy samples -> data [irow] [3] = my data [irow] [zcol];
		}
		ArbitrarilySampled2D_setDomain (thee.get(), 0.0, 0.0, 0.0, 0.0); // auto
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoArbitrarilySampled3D TableOfReal_to_ArbitrarilySampled3D (TableOfReal me, long xcol, long ycol, long zcol, long vcol) {
	try {
		if (xcol < 1 || xcol > my numberOfColumns || ycol < 1 || ycol > my numberOfColumns || zcol < 1 || zcol > my numberOfColumns || vcol < 1 || vcol > my numberOfColumns) {
			Melder_throw (U"The column numbers must in the range 1..", my numberOfColumns, U".");
		}
		autoArbitrarilySampled3D thee = ArbitrarilySampled3D_create (my numberOfRows, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			thy samples -> data [irow] [1] = my data [irow] [xcol];
			thy samples -> data [irow] [2] = my data [irow] [ycol];
			thy samples -> data [irow] [3] = my data [irow] [zcol];
			thy samples -> data [irow] [4] = my data [irow] [vcol];
		}
		ArbitrarilySampled3D_setDomain (thee.get(), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0); // auto domain
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

/* End of file ArbitrarilySampled.cpp */
