#ifndef _Matrix_h_
#define _Matrix_h_
/* Matrix.h
 *
 * Copyright (C) 1992-2005,2007-2019,2022,2024 Paul Boersma
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

#include "SampledXY.h"
#include "Graphics.h"
#include "../stat/Table.h"
#include "../stat/TableOfReal.h"
Thing_declare (Interpreter);

#include "Matrix_def.h"

#if 0
template <typename T, typename... Args>
	autoSomeThing <T> Thing_create (Args ... args) {
		autoSomeThing <T> me (new T);   // this `new` has to set classInfo
		my T::init (args...);
		return me;
	}

template <typename... ArgumentTypes>
autoMatrix CreateMatrix (ArgumentTypes... arguments) {
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.get(), arguments...);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not created.");
	}
}
#endif

void Matrix_init
	(Matrix me, double xmin, double xmax, integer nx, double dx, double x1,
	            double ymin, double ymax, integer ny, double dy, double y1);

autoMatrix Matrix_create
	(double xmin, double xmax, integer nx, double dx, double x1,
	 double ymin, double ymax, integer ny, double dy, double y1);
/*
	Function:
		return a new empty Matrix.
	Preconditions:
		xmin <= xmax;
		ymin <= ymax;
		nx >= 1;
		ny >= 1;
		dx > 0.0;
		dy > 0.0;
	Postconditions:
		result -> xmin == xmin;
		result -> xmax == xmax;
		result -> ymin == ymin;
		result -> ymax == ymax;
		result -> nx == nx;
		result -> ny == ny;
		result -> dx == dx;
		result -> dy == dy;
		result -> x1 == x1;
		result -> y1 == y1;
		result -> z [1..ny] [1..nx] == 0.0;
*/

autoMatrix Matrix_createSimple (integer numberOfRows, integer numberOfColumns);
/*
	Function:
		return a new empty Matrix.
	Preconditions:
		numberOfRows >= 1;
		numberOfColumns >= 1;
	Postconditions:
		result -> xmin == 0.5;
		result -> xmax == numberOfColumns + 0.5;
		result -> ymin == 0.5;
		result -> ymax == numberOfRows + 0.5;
		result -> nx == numberOfColumns;
		result -> ny == numberOfRows;
		result -> dx == 1.0;
		result -> dy == 1.0;
		result -> x1 == 1.0;
		result -> y1 == 1.0;
		result -> z [1..ny] [1..nx] == 0.0;
*/

/* An implemented method

	void structMatrix :: v1_writeText (MelderFile file);
		writes a Matrix as text to `file`.
		A sample of the format follows:
			0 5000 1 2   ! xmin xmax ymin ymax
			8193 2   ! nx ny
			0.61035156 1   ! dx dy
			0 1   ! x1 y1
 			5.1e-8   ! 0 1
			5   ! 0.61035156 1
			-3.556473   ! 1.2207031 1
			...
			90000000   ! 4998.7793 2
			3.1415927   ! 4999.3896 2
 			-5.735668e35   ! 5000 2
			The data lines (all lines after the fourth) contain: my z [iy, ix], x, y.
			things written after the "!" are mere comments:
			you cannot use them to change the meaning or order of the data.
*/

integer Matrix_getWindowSamplesX (constMatrix me,
	double xmin, double xmax,
	integer *out_ixmin, integer *out_ixmax);
/*
	Function:
		return the number of samples with x values in [xmin, xmax].
		Put the first of these samples in ixmin.
		Put the last of these samples in ixmax.
	Postconditions:
		*out_ixmin >= 1;
		*out_ixmax <= my nx;
		if (result != 0) *out_ixmin <= *out_ixmax; else *out_ixmin > *out_ixmax;
		if (result != 0) result == *out_ixmax - *out_ixmin + 1;
*/

double Matrix_getValueAtXY (constMatrix me, double x, double y);
/*
	Linear interpolation between matrix points,
	constant extrapolation in cells on the edge,
	undefined outside the union of the unit squares around the points.
*/

double Matrix_getSum (constMatrix me);
double Matrix_getNorm (constMatrix me);

double Matrix_columnToX (constMatrix me, double column);
	// return my x1 + (column - 1) * my dx

double Matrix_rowToY (constMatrix me, double row);
	// return my y1 + (row - 1) * my dy

double Matrix_xToColumn (constMatrix me, double x);
	// return (x - xmin) / my dx + 1

integer Matrix_xToLowColumn (constMatrix me, double x);
	// return Melder_ifloor (Matrix_xToColumn (me, x))

integer Matrix_xToHighColumn (constMatrix me, double x);
	// return Melder_iceiling (Matrix_xToColumn (me, x))

integer Matrix_xToNearestColumn (constMatrix me, double x);
	// return Melder_iround (Matrix_xToColumn (me, x))

double Matrix_yToRow (constMatrix me, double y);
	// return (y - ymin) / my dy + 1

integer Matrix_yToLowRow (constMatrix me, double y);
	// return Melder_ifloor (Matrix_yToRow (me, y))

integer Matrix_yToHighRow (constMatrix me, double x);
	// return Melder_iceiling (Matrix_yToRow (me, y))

integer Matrix_yToNearestRow (constMatrix me, double y);
	// return Melder_iround (Matrix_yToRow (me, y))

integer Matrix_getWindowSamplesY (constMatrix me,
	double ymin, double ymax,
	integer *out_iymin, integer *out_iymax
);

integer Matrix_getWindowExtrema (constMatrix me,
	integer ixmin, integer ixmax,
	integer iymin, integer iymax,
	double *out_minimum, double *out_maximum
);
/*
	Function:
		compute the minimum and maximum values of my z over all samples inside [ixmin, ixmax] * [iymin, iymax].
	Arguments:
		if ixmin = 0, start at first column; if ixmax = 0, end at last column (same for iymin and iymax).
	Return value:
		the number of samples inside the window.
	Postconditions:
		if result == 0, *minimum and *maximum are not changed;
*/

void Matrix_formula (mutableMatrix me, conststring32 expression, Interpreter interpreter, mutableMatrix target);
/*
	Arguments:
		"me" is the Matrix referred to as "self" or with "nx" etc. in the expression
		"target" is the Matrix whose elements will change according to:
			FOR row FROM 1 TO my ny
				FOR col FROM 1 TO my nx
					target -> z [row, col] = expression
				ENDFOR
			ENDFOR
		"expression" is the text to be compiled and interpreted.
		If "target" is null, the result will go to "me"; otherwise, to "target".
	Return value:
		0 in case of failure, otherwise 1.
*/
void Matrix_formula_part (mutableMatrix me, double xmin, double xmax, double ymin, double ymax,
	conststring32 expression, Interpreter interpreter, mutableMatrix target);

/***** Graphics routines. *****/
/*
	All of these routines show the samples of a Matrix whose x and y values
	are inside the window [xmin, xmax] * [ymin, ymax].
	The scaling of the values of these samples is determined by "minimum" and "maximum".
	All of these functions can perform automatic windowing and scaling:
	if xmax <= xmin, the window in the x direction will be set to [my xmin, my xmax];
	if ymax <= ymin, the window in the y direction will be set to [my ymin, my ymax];
	if maximum <= minimum, the windowing (scaling) in the z direction will be determined
	by the minimum and maximum values of the samples inside the window.
*/

void Matrix_drawRows (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Every row is plotted as a function of x,
	with straight lines connecting the sample points.
	The rows are stacked from bottom to top.
*/

void Matrix_drawOneContour (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double height);

void Matrix_drawContours (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/* A contour altitude plot with curves at multiple heights. */

void Matrix_paintContours (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/* A contour plot with multiple shades of grey and white (low) and black (high) paint. */

void Matrix_paintImage (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Two-dimensional interpolation of greys.
	The larger the value of the sample, the darker the greys.
*/

void Matrix_paintCells (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum);
/*
	Every sample is drawn as a grey rectangle.
	The larger the value of the sample, the darker the rectangle.
*/

void Matrix_paintSurface (constMatrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, double elevation, double azimuth);
/*
	3D surface plot. Every space between adjacent four samples is drawn as a tetragon filled with a grey value.
	'elevation' may be 30 degrees, 'azimuth' may be 45 degrees.
*/

void Matrix_playMovie (constMatrix me, Graphics g);

autoMatrix Matrix_readFromRawTextFile (MelderFile file);
autoMatrix Matrix_readAP (MelderFile file);
autoMatrix Matrix_appendRows (constMatrix me, constMatrix thee, ClassInfo klas);

void Matrix_eigen (constMatrix me, autoMatrix *eigenvectors, autoMatrix *eigenvalues);
autoMatrix Matrix_power (constMatrix me, integer power);

void Matrix_scaleAbsoluteExtremum (mutableMatrix me, double scale);

autoMatrix Table_to_Matrix (Table me);
void Matrix_writeToMatrixTextFile (constMatrix me, MelderFile file);
void Matrix_writeToHeaderlessSpreadsheetFile (constMatrix me, MelderFile file);

autoMatrix TableOfReal_to_Matrix (constTableOfReal me);
autoTableOfReal Matrix_to_TableOfReal (constMatrix me);

/* End of file Matrix.h */
#endif
