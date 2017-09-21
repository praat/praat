#ifndef _ArbitrarilySampled_h_
#define _ArbitrarilySampled_h_
/* ArbitrarilySampled.h
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

#include "Matrix.h"
#include "melder.h"
#include "NUM2.h"
#include "TableOfReal.h"

#include "ArbitrarilySampled_def.h"

/* Interpretation:
	1D data:
	column 1: x_values of samples
	column 2: value at sample point (x)
	2D data
	column 1: x_value of samples
	column 2: y_value of samples
	column 3: data value at sample point (x,y)
	3D data
	column 1: x_value of samples
	column 2: y_value of samples
	column 3: z_value of samples
	column 4: data value at sample point(x,y,z)
*/

void ArbitrarilySampled_init (long numberOfSamples, long numberOfDimensions);

double ArbitrarilySampled_getEuclideanDistance (ArbitrarilySampled me, long ipoint, long jpoint);

double ArbitrarilySampled_getEuclideanDistance_pos (ArbitrarilySampled me, long ipoint, numvec position);

/*
	In biharmonic interpolation the value s(X) at a new point X (1, 2 or 3 dimensional) is a linear combination 
	of Green's functions defined at each datapoint (X[j]).
		v = Sum (j=1, n, c[j] g(r[j]), 
	where n is the number of data points, g(r[j]) is Green's function, r[j] is the distance from X to point X[j] and 
	c[j] is the corresponding multiplication factor.
	The c[j] are determined by having the s(X) pass exactly to each of the n datapoints X[j]). 
	This gives s(X[i]) = Sum (j=1, n, c[j] g(distance(X[i],X[j]).
*/
void ArbitrarilySampled_biharmonicSplines_getWeights (ArbitrarilySampled me, double tension, numvec weights);

double ArbitrarilySampled_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, numvec position);

autoTableOfReal ArbitrarilySampled_to_TableOfReal (ArbitrarilySampled me);

/**************** ArbitrarilySampled1D ******************/

Thing_define (ArbitrarilySampled1D, ArbitrarilySampled) {
	double v_getEuclideanDistance (long ipoint, long jpoint) override;
	double v_getEuclideanDistance_pos (long ipoint, numvec position) override;
	double v_greensFunction (double radialDistance, double tension) override;
};

void ArbitrarilySampled1D_setDomain (ArbitrarilySampled1D me, double xmin, double xmax);

autoArbitrarilySampled1D ArbitrarilySampled1D_create (long numberOfDataPoints, double xmin, double xmax);

autoArbitrarilySampled1D TableOfReal_to_ArbitrarilySampled1D (TableOfReal me, long xcol, long ycol);

double ArbitrarilySampled1D_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, double x);

autoArbitrarilySampled1D TableOfReal_to_ArbitrarilySampled1D (TableOfReal me, long icolx, long icoly);

autoMatrix ArbitrarilySampled1D_to_Matrix_biharmonicSplinesInterpolation (ArbitrarilySampled me, double tension, double xmin, double xmax, long nx);

/**************** ArbitrarilySampled2D ******************/

Thing_define (ArbitrarilySampled2D, ArbitrarilySampled) {
	double v_getEuclideanDistance (long ipoint, long jpoint) override;
	double v_getEuclideanDistance_pos (long ipoint, numvec position) override;
	double v_greensFunction (double radialDistance, double tension) override;
};

void ArbitrarilySampled2D_setDomain (ArbitrarilySampled1D me, double xmin, double xmax, double ymin, double ymax);

autoArbitrarilySampled2D ArbitrarilySampled2D_create (long numberOfDataPoints, double xmin, double xmax, double ymin, double ymax);

autoArbitrarilySampled2D TableOfReal_to_ArbitrarilySampled2D (TableOfReal me, long xcol, long ycol, long zcol);

double ArbitrarilySampled2D_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, double x, double y);

autoArbitrarilySampled2D TableOfReal_to_ArbitrarilySampled2D (TableOfReal me, long icolx, long icoly, long icolz);

autoMatrix ArbitrarilySampled2D_to_Matrix_biharmonicSplinesInterpolation (ArbitrarilySampled me, double tension, double xmin, double xmax, long nx, double ymin, double ymax, long ny);

/**************** ArbitrarilySampled3D ******************/

Thing_define (ArbitrarilySampled3D, ArbitrarilySampled) {
	double v_getEuclideanDistance (long ipoint, long jpoint) override;
	double v_getEuclideanDistance_pos (long ipoint, numvec position) override;
	double v_greensFunction (double radialDistance, double tension) override;
};

void ArbitrarilySampled3D_setDomain (ArbitrarilySampled1D me, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

autoArbitrarilySampled3D ArbitrarilySampled3D_create (long numberOfDataPoints, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

autoArbitrarilySampled3D TableOfReal_to_ArbitrarilySampled3D (TableOfReal me, long xcol, long ycol, long zcol, long vcol);

double ArbitrarilySampled3D_biharmonicSplines_interpolate (ArbitrarilySampled me, double tension, numvec weights, double x, double y, double z);

autoArbitrarilySampled3D TableOfReal_to_ArbitrarilySampled3D (TableOfReal me, long icolx, long icoly, long icolz, long icoldata);


/* End of file ArbitrarilySampled.h */
#endif
