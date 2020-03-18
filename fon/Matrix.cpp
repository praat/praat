/* Matrix.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
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
#include "NUM2.h"
#include "Formula.h"
#include "Eigen.h"

#include "oo_DESTROY.h"
#include "Matrix_def.h"
#include "oo_COPY.h"
#include "Matrix_def.h"
#include "oo_EQUAL.h"
#include "Matrix_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Matrix_def.h"
#include "oo_WRITE_TEXT.h"
#include "Matrix_def.h"
#include "oo_WRITE_BINARY.h"
#include "Matrix_def.h"
#include "oo_READ_BINARY.h"
#include "Matrix_def.h"
#include "oo_DESCRIPTION.h"
#include "Matrix_def.h"

Thing_implement (Matrix, SampledXY, 2);

void structMatrix :: v_info () {
	structDaata :: v_info ();
	double minimum = 0.0, maximum = 0.0;
	Matrix_getWindowExtrema (this, 1, our nx, 1, our ny, & minimum, & maximum);
	MelderInfo_writeLine (U"xmin: ", our xmin);
	MelderInfo_writeLine (U"xmax: ", our xmax);
	MelderInfo_writeLine (U"Number of columns: ", our nx);
	MelderInfo_writeLine (U"dx: ", our dx, U" (-> sampling rate ", 1.0 / our dx, U" )");
	MelderInfo_writeLine (U"x1: ", our x1);
	MelderInfo_writeLine (U"ymin: ", our ymin);
	MelderInfo_writeLine (U"ymax: ", our ymax);
	MelderInfo_writeLine (U"Number of rows: ", our ny);
	MelderInfo_writeLine (U"dy: ", our dy, U" (-> sampling rate ", 1.0 / our dy, U" )");
	MelderInfo_writeLine (U"y1: ", our y1);
	MelderInfo_writeLine (U"Minimum value: ", minimum);
	MelderInfo_writeLine (U"Maximum value: ", maximum);
}

void structMatrix :: v_readText (MelderReadText text, int formatVersion) {
	if (formatVersion < 0) {
		our xmin = texgetr64 (text);
		our xmax = texgetr64 (text);
		our ymin = texgetr64 (text);
		our ymax = texgetr64 (text);
		our nx = texgeti32 (text);
		our ny = texgeti32 (text);
		our dx = texgetr64 (text);
		our dy = texgetr64 (text);
		our x1 = texgetr64 (text);
		our y1 = texgetr64 (text);
	} else {
		Matrix_Parent :: v_readText (text, formatVersion);
	}
	Melder_require (our xmin <= our xmax, U"xmin should be less than or equal to xmax.");
	Melder_require (our ymin <= our ymax, U"ymin should be less than or equal to ymax.");
	Melder_require (our nx >= 1, U"nx should be at least 1.");
	Melder_require (our ny >= 1, U"ny should be at least 1.");
	Melder_require (our dx > 0.0, U"dx should be greater than 0.0.");
	Melder_require (our dy > 0.0, U"dy should be greater than 0.0.");
	our z = matrix_readText_r64 (our ny, our nx, text, "z");
}

double structMatrix :: v_getValueAtSample (integer isamp, integer ilevel, int unit) {
	const double value = our z [ilevel] [isamp];
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, ilevel, unit) : undefined );
}

double structMatrix :: v_getMatrix (integer irow, integer icol) {
	if (irow < 1 || irow > our ny)
		return 0.0;
	if (icol < 1 || icol > our nx)
		return 0.0;
	return z [irow] [icol];
}

double structMatrix :: v_getFunction2 (double x, double y) {
	const double rrow = (y - our y1) / our dy + 1.0;
	const double rcol = (x - our x1) / our dx + 1.0;
	const integer irow = Melder_ifloor (rrow), icol = Melder_ifloor (rcol);
	const double drow = rrow - irow, dcol = rcol - icol;
	const double z1 = irow < 1 || irow >  our ny || icol < 1 || icol >  our nx ? 0.0 : z [irow]     [icol];
	const double z2 = irow < 0 || irow >= our ny || icol < 1 || icol >  our nx ? 0.0 : z [irow + 1] [icol];
	const double z3 = irow < 1 || irow >  our ny || icol < 0 || icol >= our nx ? 0.0 : z [irow]     [icol + 1];
	const double z4 = irow < 0 || irow >= our ny || icol < 0 || icol >= our nx ? 0.0 : z [irow + 1] [icol + 1];
	return (1.0 - drow) * (1.0 - dcol) * z1 + drow * (1.0 - dcol) * z2 + (1.0 - drow) * dcol * z3 + drow * dcol * z4;
}

void Matrix_init
	(Matrix me, double xmin, double xmax, integer nx, double dx, double x1,
	            double ymin, double ymax, integer ny, double dy, double y1)
{
	Sampled_init (me, xmin, xmax, nx, dx, x1);
	my ymin = ymin;
	my ymax = ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
	my z = newMATzero (my ny, my nx);
}

autoMatrix Matrix_create
	(double xmin, double xmax, integer nx, double dx, double x1,
	 double ymin, double ymax, integer ny, double dy, double y1)
{
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not created.");
	}
}

autoMatrix Matrix_createSimple (integer numberOfRows, integer numberOfColumns) {
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.get(),
				0.5, numberOfColumns + 0.5, numberOfColumns, 1, 1,
				0.5, numberOfRows    + 0.5, numberOfRows   , 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not created.");
	}
}

double Matrix_columnToX (Matrix me, double column) { return my x1 + (column - 1.0) * my dx; }   // FIXME inline and use Sampled

double Matrix_rowToY (Matrix me, double row) { return my y1 + (row - 1.0) * my dy; }

double Matrix_xToColumn (Matrix me, double x) { return (x - my x1) / my dx + 1.0; }

integer Matrix_xToLowColumn (Matrix me, double x) { return Melder_ifloor (Matrix_xToColumn (me, x)); }

integer Matrix_xToHighColumn (Matrix me, double x) { return Melder_iceiling (Matrix_xToColumn (me, x)); }

integer Matrix_xToNearestColumn (Matrix me, double x) { return Melder_iround (Matrix_xToColumn (me, x)); }

double Matrix_yToRow (Matrix me, double y) { return (y - my y1) / my dy + 1.0; }

integer Matrix_yToLowRow (Matrix me, double y) { return Melder_ifloor (Matrix_yToRow (me, y)); }

integer Matrix_yToHighRow (Matrix me, double y) { return Melder_iceiling (Matrix_yToRow (me, y)); }

integer Matrix_yToNearestRow (Matrix me, double y) { return Melder_iround (Matrix_yToRow (me, y)); }

integer Matrix_getWindowSamplesX (Matrix me, double xmin, double xmax, integer *ixmin, integer *ixmax) {
	*ixmin = 1 + Melder_iceiling ((xmin - my x1) / my dx);
	*ixmax = 1 + Melder_ifloor   ((xmax - my x1) / my dx);
	if (*ixmin < 1)
		*ixmin = 1;
	if (*ixmax > my nx)
		*ixmax = my nx;
	if (*ixmin > *ixmax)
		return 0;
	return *ixmax - *ixmin + 1;
}

integer Matrix_getWindowSamplesY (Matrix me, double ymin, double ymax, integer *iymin, integer *iymax) {
	*iymin = 1 + Melder_iceiling ((ymin - my y1) / my dy);
	*iymax = 1 + Melder_ifloor   ((ymax - my y1) / my dy);
	if (*iymin < 1)
		*iymin = 1;
	if (*iymax > my ny)
		*iymax = my ny;
	if (*iymin > *iymax)
		return 0;
	return *iymax - *iymin + 1;
}

integer Matrix_getWindowExtrema (Matrix me, integer ixmin, integer ixmax, integer iymin, integer iymax,
	double *minimum, double *maximum)
{
	if (ixmin == 0)
		ixmin = 1;
	if (ixmax == 0)
		ixmax = my nx;
	if (iymin == 0)
		iymin = 1;
	if (iymax == 0)
		iymax = my ny;
	if (ixmin > ixmax || iymin > iymax)
		return 0;
	MelderExtremaWithInit extrema;
	for (integer iy = iymin; iy <= iymax; iy ++)
		for (integer ix = ixmin; ix <= ixmax; ix ++)
			extrema.update (my z [iy] [ix]);
	*minimum = extrema.min;
	*maximum = extrema.max;
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

double Matrix_getValueAtXY (Matrix me, double x, double y) {
	const double row_real = (y - my y1) / my dy + 1.0;
	const double col_real = (x - my x1) / my dx + 1.0;
	/*
	 * We imagine a unit square around every (xi, yi) point in the matrix.
	 * For (x, y) values outside the union of these squares, the z value is undefined.
	 */
	if (row_real < 0.5 || row_real > my ny + 0.5)
		return undefined;
	if (col_real < 0.5 || col_real > my nx + 0.5)
		return undefined;
	/*
	 * Determine the four nearest (xi, yi) points.
	 */
	integer bottomRow = Melder_ifloor (row_real);   // 0 <= bottomRow <= my ny
	integer topRow = bottomRow + 1;         // 1 <= topRow <= my ny + 1
	integer leftCol = Melder_ifloor (col_real);     // 0 <= leftCol <= my nx
	integer rightCol = leftCol + 1;         // 1 <= rightCol <= my nx + 1
	double drow = row_real - bottomRow;    // 0.0 <= drow < 1.0
	double dcol = col_real - leftCol;      // 0.0 <= dcol < 1.0
	/*
	 * If adjacent points exist
	 * (i.e., both row numbers are between 1 and my ny,
	 *  or both column numbers are between 1 and my nx),
	 * we do linear interpolation.
	 * If not, we do constant extrapolation,
	 * which can be simulated by an interpolation between equal z values.
	 */
	if (bottomRow < 1)
		bottomRow = 1;         // 1 <= bottomRow <= my ny
	if (topRow > my ny)
		topRow = my ny;        // 1 <= topRow <= my ny
	if (leftCol < 1)
		leftCol = 1;           // 1 <= leftCol <= my nx
	if (rightCol > my nx)
		rightCol = my nx;      // 1 <= rightCol <= my nx
	return (1.0 - drow) * (1.0 - dcol) * my z [bottomRow] [leftCol] +
		drow * (1.0 - dcol) * my z [topRow] [leftCol] +
		(1.0 - drow) * dcol * my z [bottomRow] [rightCol] +
		drow * dcol * my z [topRow] [rightCol];
}

double Matrix_getSum (Matrix me) {
	return NUMsum (my z.all());
}

double Matrix_getNorm (Matrix me) {
	return NUMnorm (my z.get(), 2.0);
}

void Matrix_drawRows (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) {
		ymin = my ymin;
		ymax = my ymax;
	}
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	if (xmin >= xmax)
		return;
	Graphics_setInner (g);
	for (integer iy = iymin; iy <= iymax; iy ++) {
		Graphics_setWindow (g, xmin, xmax,
				minimum - (iy - iymin) * (maximum - minimum),
				maximum + (iymax - iy) * (maximum - minimum));
		Graphics_function (g, & my z [iy] [0], ixmin, ixmax,
				Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
	if (iymin < iymax)
		Graphics_setWindow (g, xmin, xmax, my y1 + (iymin - 1.5) * my dy, my y1 + (iymax - 0.5) * my dy);
}

void Matrix_drawOneContour (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double height)
{
	bool xreversed = xmin > xmax, yreversed = ymin > ymax;
	if (xmax == xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax == ymin) { ymin = my ymin; ymax = my ymax; }
	if (xreversed) { double temp = xmin; xmin = xmax; xmax = temp; }
	if (yreversed) { double temp = ymin; ymin = ymax; ymax = temp; }
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (xmin == xmax || ymin == ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xreversed ? xmax : xmin, xreversed ? xmin : xmax, yreversed ? ymax : ymin, yreversed ? ymin : ymax);
	Graphics_contour (g, my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		height);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_drawContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	double border [1 + 8];
	if (xmax == xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax == ymin) { ymin = my ymin; ymax = my ymax; }
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (integer iborder = 1; iborder <= 8; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (8 + 1);
	if (xmin == xmax || ymin == ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_altitude (g, my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		8, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintContours (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	double border [1 + 30];
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (integer iborder = 1; iborder <= 30; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (30 + 1);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_grey (g, my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		30, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

static void cellArrayOrImage (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, bool interpolate)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx,
		& ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy,
		& iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	if (interpolate)
		Graphics_image (g, my z.part (iymin, iymax, ixmin, ixmax),
			Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);
	else
		Graphics_cellArray (g, my z.part (iymin, iymax, ixmin, ixmax),
			Sampled_indexToX   (me, ixmin - 0.5), Sampled_indexToX   (me, ixmax + 0.5),
			SampledXY_indexToY (me, iymin - 0.5), SampledXY_indexToY (me, iymax + 0.5),
			minimum, maximum);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintImage (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, true);
}

void Matrix_paintCells (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, false);
}

void Matrix_paintSurface (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, double elevation, double azimuth)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	Graphics_setInner (g);
	Graphics_setWindow (g, -1.0, 1.0, minimum, maximum);
	Graphics_surface (g, my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		minimum, maximum, elevation, azimuth);
	Graphics_unsetInner (g);
}

void Matrix_playMovie (Matrix me, Graphics g) {
	Melder_require (my ny >= 2,
		me, U": cannot play a movie for a Matrix with less than 2 rows.");
	autoVEC column = newVECraw (my ny);
	double minimum = 0.0, maximum = 1.0;
	Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & minimum, & maximum);
	if (minimum == maximum) {
		minimum -= 0.5;
		maximum += 0.5;
	}
	for (integer icol = 1; icol <= my nx; icol ++) {
		column.all() <<= my z.column (icol);
		Graphics_beginMovieFrame (g, & Melder_WHITE);
		Graphics_setWindow (g, my ymin, my ymax, minimum, maximum);
		Graphics_function (g, column.asArgumentToFunctionThatExpectsOneBasedArray(), 1, my ny, my ymin, my ymax);
		Graphics_endMovieFrame (g, 0.03);
	}
}

autoMatrix Matrix_readAP (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		int16 header [256];
		for (integer i = 0; i < 256; i ++)
			header [i] = bingeti16LE (f);
		const integer numberOfFrames = header [34];
		const integer numberOfWordsPerFrame = header [35];
		const double samplingFrequency = double (header [100]);   // converting up (from 16 to 54 bits)
		//const integer numberOfSamplesPerFrame = header [110];
		Melder_casual (U"Sampling frequency ", samplingFrequency);
		autoMatrix me = Matrix_create (0.0, (double) numberOfFrames, numberOfFrames, 1.0, 0.5,
				0.0, (double) numberOfWordsPerFrame, numberOfWordsPerFrame, 1.0, 0.5);
		Melder_casual (U"... Loading ", numberOfFrames, U" frames",
			U" of ", numberOfWordsPerFrame, U" words each ...");
		for (integer i = 1; i <= my nx; i ++)
			for (integer j = 1; j <= my ny; j ++)
				my z [j] [i] = bingeti16LE (f);   // converting up (from 16 to 54 bits)

		/*
			Get pitch frequencies.
		*/
		for (integer i = 1; i <= my nx; i ++)
			if (my z [1] [i] != 0.0)
				my z [1] [i] = - samplingFrequency / my z [1] [i];

		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not read from AP file ", file);
	}
}

autoMatrix Matrix_appendRows (Matrix me, Matrix thee, ClassInfo klas) {
	try {
		autoMatrix him = Thing_newFromClass (klas).static_cast_move<structMatrix>();
		Matrix_init (him.get(),
			std::min (my xmin, thy xmin),
			std::max (my xmax, thy xmax),
			std::max (my nx, thy nx),
			my dx,
			std::min (my x1, thy x1),
			my ymin, my ymax + (thy ymax - thy ymin), my ny + thy ny, my dy, my y1
		);
		for (integer irow = 1; irow <= my ny; irow ++)
			for (integer icol = 1; icol <= my nx; icol ++)
				his z [irow] [icol] = my z [irow] [icol];
		for (integer irow = 1; irow <= thy ny; irow ++)
			for (integer icol = 1; icol <= thy nx; icol ++)
				his z [irow + my ny] [icol] = thy z [irow] [icol];
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": rows not appended.");
	}
}

autoMatrix Matrix_readFromRawTextFile (MelderFile file) {   // BUG: not Unicode-compatible (use of fscanf)
	try {
		autofile f = Melder_fopen (file, "rb");

		/*
			Count columns.
		*/
		integer numberOfColumns = 0;
		for (;;) {
			int kar = fgetc (f);
			if (kar == EOF || Melder_isVerticalSpace ((char32) kar))
				break;
			if (Melder_isHorizontalSpace ((char32) kar))
				continue;
			numberOfColumns ++;
			do {
				kar = fgetc (f);
			} while (kar != EOF && ! Melder_isHorizontalOrVerticalSpace ((char32) kar));
			if (kar == EOF || Melder_isVerticalSpace ((char32) kar))
				break;
		}
		if (numberOfColumns == 0)
			Melder_throw (U"File empty");

		/*
			Count elements.
		*/
		rewind (f);
		integer numberOfElements = 0;
		for (;;) {
			double element;
			if (fscanf (f, "%lf", & element) < 1)
				break;   // zero or end-of-file
			numberOfElements ++;
		}

		/*
			Check if all columns are complete.
		*/
		if (numberOfElements == 0 || numberOfElements % numberOfColumns != 0)
			Melder_throw (U"The number of elements (", numberOfElements, U") is not a multiple of the number of columns (", numberOfColumns, U").");

		/*
			Create simple matrix.
		*/
		integer numberOfRows = numberOfElements / numberOfColumns;
		autoMatrix me = Matrix_createSimple (numberOfRows, numberOfColumns);

		/*
			Read elements.
		*/
		rewind (f);
		for (integer irow = 1; irow <= numberOfRows; irow ++)
			for (integer icol = 1; icol <= numberOfColumns; icol ++)
				fscanf (f, "%lf", & my z [irow] [icol]);

		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not read from raw text file ", file, U".");
	}
}

static bool isSymmetric (Matrix me) {
	for (integer irow = 1; irow <= my ny - 1; irow ++)
		for (integer icol = irow + 1; icol <= my nx; icol ++)
			if (my z [irow] [icol] != my z [icol][irow])
				return false;
	return true;
}

void Matrix_eigen (Matrix me, autoMatrix *out_eigenvectors, autoMatrix *out_eigenvalues) {
	try {
		Melder_require (my nx == my ny, 
			U"The number of rows (here ", my ny, U") should be equal to the number of columns (here ", my nx, U").");
		Melder_require (isSymmetric (me),
			U"The matrix should be symmetric.");
		autoEigen eigen = Thing_new (Eigen);
		Eigen_initFromSymmetricMatrix (eigen.get(), my z.get());
		autoMatrix eigenvectors = Data_copy (me);
		autoMatrix eigenvalues = Matrix_create (1.0, 1.0, 1, 1.0, 1.0, my ymin, my ymax, my ny, my dy, my y1);
		for (integer i = 1; i <= my nx; i ++) {
			eigenvalues -> z [i] [1] = eigen -> eigenvalues [i];
			for (integer j = 1; j <= my nx; j ++)
				eigenvectors -> z [i] [j] = eigen -> eigenvectors [j] [i];
		}
		*out_eigenvectors = eigenvectors.move();
		*out_eigenvalues = eigenvalues.move();
	} catch (MelderError) {
		Melder_throw (me, U": eigenstructure not computed.");
	}
}

autoMatrix Matrix_power (Matrix me, integer power) {
	try {
		Melder_require (my nx == my ny,
			U"The number of rows (here ", my ny, U") should be equal to the number of columns (here ", my nx, U").");
		autoMatrix thee = Data_copy (me);
		autoMatrix him = Data_copy (me);
		for (integer ipow = 2; ipow <= power; ipow ++) {
			std::swap (his z, thy z);
			for (integer irow = 1; irow <= my ny; irow ++) {
				for (integer icol = 1; icol <= my nx; icol ++) {
					thy z [irow] [icol] = 0.0;
					for (integer i = 1; i <= my nx; i ++)
						thy z [irow] [icol] += his z [irow] [i] * my z [i] [icol];
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": power not computed.");
	}
}

void Matrix_writeToMatrixTextFile (Matrix me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "\"ooTextFile\"\n\"Matrix\"\n%s %s %s %s %s\n%s %s %s %s %s\n",
			Melder8_double (my xmin), Melder8_double (my xmax), Melder8_integer (my nx),
				Melder8_double (my dx), Melder8_double (my x1),
			Melder8_double (my ymin), Melder8_double (my ymax), Melder8_integer (my ny),
				Melder8_double (my dy), Melder8_double (my y1));
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++) {
				if (j > 1)
					fprintf (f, " ");
				fprintf (f, "%s", Melder8_double (my z [i] [j]));
			}
			fprintf (f, "\n");
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not written to Matrix text file.");
	}
}

void Matrix_writeToHeaderlessSpreadsheetFile (Matrix me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++) {
				if (j > 1)
					fprintf (f, "\t");
				fprintf (f, "%s", Melder8_single (my z [i] [j]));
			}
			fprintf (f, "\n");
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not saved as tab-separated file ", file);
	}
}

void Matrix_formula (Matrix me, conststring32 expression, Interpreter interpreter, Matrix target) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! target)
			target = me;
		for (integer irow = 1; irow <= my ny; irow ++) {
			for (integer icol = 1; icol <= my nx; icol ++) {
				Formula_run (irow, icol, & result);
				target -> z [irow] [icol] = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

void Matrix_formula_part (Matrix me, double xmin, double xmax, double ymin, double ymax,
	conststring32 expression, Interpreter interpreter, Matrix target)
{
	try {
		Function_unidirectionalAutowindow (me, & xmin, & xmax);
		if (ymax <= ymin) {
			ymin = my ymin;
			ymax = my ymax;
		}
		integer ixmin, ixmax, iymin, iymax;
		(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
		(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! target)
			target = me;
		for (integer irow = iymin; irow <= iymax; irow ++) {
			for (integer icol = ixmin; icol <= ixmax; icol ++) {
				Formula_run (irow, icol, & result);
				target -> z [irow] [icol] = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

void Matrix_scaleAbsoluteExtremum (Matrix me, double scale) {
	double extremum = 0.0;
	for (integer i = 1; i <= my ny; i ++)
		for (integer j = 1; j <= my nx; j ++)
			if (fabs (my z [i] [j]) > extremum)
				extremum = fabs (my z [i] [j]);
	if (extremum != 0.0) {
		double factor = scale / extremum;
		for (integer i = 1; i <= my ny; i ++)
			for (integer j = 1; j <= my nx; j ++)
				my z [i] [j] *= factor;
	}
}

autoMatrix TableOfReal_to_Matrix (TableOfReal me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns);
		for (integer i = 1; i <= my numberOfRows; i ++)
			for (integer j = 1; j <= my numberOfColumns; j ++)
				thy z [i] [j] = my data [i] [j];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoTableOfReal Matrix_to_TableOfReal (Matrix me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my ny, my nx);
		for (integer i = 1; i <= my ny; i ++)
			for (integer j = 1; j <= my nx; j ++)
				thy data [i] [j] = my z [i] [j];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoMatrix Table_to_Matrix (Table me) {
	try {
		autoMatrix thee = Matrix_createSimple (my rows.size, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			Table_numericize_Assert (me, icol);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				thy z [irow] [icol] = row -> cells [icol]. number;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of file Matrix.cpp */
