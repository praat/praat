/* Matrix.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

void structMatrix :: v1_info () {
	structDaata :: v1_info ();
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

void structMatrix :: v1_readText (MelderReadText text, int formatVersion) {
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
		Matrix_Parent :: v1_readText (text, formatVersion);
	}
	Melder_require (our xmin <= our xmax, U"xmin should be less than or equal to xmax.");
	Melder_require (our ymin <= our ymax, U"ymin should be less than or equal to ymax.");
	Melder_require (our nx >= 1, U"nx should be at least 1.");
	Melder_require (our ny >= 1, U"ny should be at least 1.");
	Melder_require (our dx > 0.0, U"dx should be greater than 0.0.");
	Melder_require (our dy > 0.0, U"dy should be greater than 0.0.");
	our z = matrix_readText_r64 (our ny, our nx, text, "z");
}

double structMatrix :: v_getValueAtSample (integer isamp, integer ilevel, int unit) const {
	const double value = our z [ilevel] [isamp];
	return ( isdefined (value) ? our v_convertStandardToSpecialUnit (value, ilevel, unit) : undefined );
}

double structMatrix :: v_getMatrix (integer irow, integer icol) const {
	if (irow < 1 || irow > our ny)
		return 0.0;
	if (icol < 1 || icol > our nx)
		return 0.0;
	return z [irow] [icol];
}

double structMatrix :: v_getFunction2 (double x, double y) const {
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

void Matrix_init (const mutableMatrix me,
	const double xmin, const double xmax, const integer nx, const double dx, const double x1,
	const double ymin, const double ymax, const integer ny, const double dy, const double y1
) {
	Sampled_init (me, xmin, xmax, nx, dx, x1);
	my ymin = ymin;
	my ymax = ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
	my z = zero_MAT (my ny, my nx);
}

autoMatrix Matrix_create (
	const double xmin, const double xmax, const integer nx, const double dx, const double x1,
	const double ymin, const double ymax, const integer ny, const double dy, const double y1
) {
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not created.");
	}
}

autoMatrix Matrix_createSimple (const integer numberOfRows, const integer numberOfColumns) {
	try {
		autoMatrix me = Thing_new (Matrix);
		Matrix_init (me.get(),
			0.5, numberOfColumns + 0.5, numberOfColumns, 1.0, 1.0,
			0.5, numberOfRows    + 0.5, numberOfRows   , 1.0, 1.0
		);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Matrix object not created.");
	}
}

double Matrix_columnToX (const constMatrix me, const double column) { return my x1 + (column - 1.0) * my dx; }   // FIXME inline and use Sampled

double Matrix_rowToY (const constMatrix me, const double row) { return my y1 + (row - 1.0) * my dy; }

double Matrix_xToColumn (const constMatrix me, const double x) { return (x - my x1) / my dx + 1.0; }

integer Matrix_xToLowColumn (const constMatrix me, const double x) { return Melder_ifloor (Matrix_xToColumn (me, x)); }

integer Matrix_xToHighColumn (const constMatrix me, const double x) { return Melder_iceiling (Matrix_xToColumn (me, x)); }

integer Matrix_xToNearestColumn (const constMatrix me, const double x) { return Melder_iround (Matrix_xToColumn (me, x)); }

double Matrix_yToRow (const constMatrix me, const double y) { return (y - my y1) / my dy + 1.0; }

integer Matrix_yToLowRow (const constMatrix me, const double y) { return Melder_ifloor (Matrix_yToRow (me, y)); }

integer Matrix_yToHighRow (const constMatrix me, const double y) { return Melder_iceiling (Matrix_yToRow (me, y)); }

integer Matrix_yToNearestRow (const constMatrix me, const double y) { return Melder_iround (Matrix_yToRow (me, y)); }

integer Matrix_getWindowSamplesX (const constMatrix me,
	const double xmin, const double xmax, integer *const ixmin, integer *const ixmax
) {
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

integer Matrix_getWindowSamplesY (const constMatrix me,
	const double ymin, const double ymax,
	integer *const iymin, integer *const iymax
) {
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

integer Matrix_getWindowExtrema (const constMatrix me,
	integer ixmin, integer ixmax,
	integer iymin, integer iymax,
	double *const minimum, double *const maximum)
{
	if (ixmin == 0)   // default = all
		ixmin = 1;
	if (ixmax == 0)   // default = all
		ixmax = my nx;
	if (iymin == 0)   // default = all
		iymin = 1;
	if (iymax == 0)   // default = all
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

double Matrix_getValueAtXY (const constMatrix me, const double x, const double y) {
	const double row_real = (y - my y1) / my dy + 1.0;
	const double col_real = (x - my x1) / my dx + 1.0;
	/*
		We imagine a unit square around every (xi, yi) point in the matrix.
		For (x, y) values outside the union of these squares, the z value is undefined.
	*/
	if (row_real < 0.5 || row_real > my ny + 0.5)
		return undefined;
	if (col_real < 0.5 || col_real > my nx + 0.5)
		return undefined;
	/*
		Determine the four nearest (xi, yi) points.
	*/
	integer bottomRow = Melder_ifloor (row_real);   // 0 <= bottomRow <= my ny
	integer topRow = bottomRow + 1;         // 1 <= topRow <= my ny + 1
	integer leftCol = Melder_ifloor (col_real);     // 0 <= leftCol <= my nx
	integer rightCol = leftCol + 1;         // 1 <= rightCol <= my nx + 1
	const double drow = row_real - bottomRow;    // 0.0 <= drow < 1.0
	const double dcol = col_real - leftCol;      // 0.0 <= dcol < 1.0
	/*
		If adjacent points exist
		(i.e., both row numbers are between 1 and my ny,
		 or both column numbers are between 1 and my nx),
		we do linear interpolation.
		If not, we do constant extrapolation,
		which can be simulated by an interpolation between equal z values.
	*/
	Melder_clipLeft (1_integer, & bottomRow);   // 1 <= bottomRow <= my ny
	Melder_clipRight (& topRow, my ny);         // 1 <= topRow <= my ny
	Melder_clipLeft (1_integer, & leftCol);     // 1 <= leftCol <= my nx
	Melder_clipRight (& rightCol, my nx);       // 1 <= rightCol <= my nx
	return (1.0 - drow) * (1.0 - dcol) * my z [bottomRow] [leftCol] +
		drow * (1.0 - dcol) * my z [topRow] [leftCol] +
		(1.0 - drow) * dcol * my z [bottomRow] [rightCol] +
		drow * dcol * my z [topRow] [rightCol];
}

double Matrix_getSum (const constMatrix me) {
	return NUMsum (my z.all());
}

double Matrix_getNorm (const constMatrix me) {
	return NUMnorm (my z.get(), 2.0);
}

void Matrix_drawRows (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	/* mutable autoscaling */ double minimum, /* mutable autoscaling */ double maximum
) {
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_unidirectionalAutowindowY (me, & ymin, & ymax);
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
			maximum + (iymax - iy) * (maximum - minimum)
		);
		Graphics_function (g, & my z [iy] [0], ixmin, ixmax,
				Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
	if (iymin < iymax)
		Graphics_setWindow (g, xmin, xmax, my y1 + (iymin - 1.5) * my dy, my y1 + (iymax - 0.5) * my dy);
}

void Matrix_drawOneContour (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	const double height
) {
	Function_bidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_bidirectionalAutowindowY (me, & ymin, & ymax);
	const bool xreversed = xmin > xmax;
	if (xreversed)
		std::swap (xmin, xmax);
	const bool yreversed = ymin > ymax;
	if (yreversed)
		std::swap (ymin, ymax);
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (xmin == xmax || ymin == ymax)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g,
		xreversed ? xmax : xmin,
		xreversed ? xmin : xmax,
		yreversed ? ymax : ymin,
		yreversed ? ymin : ymax
	);
	Graphics_contour (g,
		my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin),
		Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin),
		Matrix_rowToY (me, iymax),
		height
	);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_drawContours (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	/* mutable autoscaling */ double minimum, /* mutable autoscaling */ double maximum
) {
	Function_bidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_bidirectionalAutowindowY (me, & ymin, & ymax);
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	double border [1 + 8];
	for (integer iborder = 1; iborder <= 8; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (8 + 1);
	if (xmin == xmax || ymin == ymax)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_altitude (g,
		my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin),
		Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin),
		Matrix_rowToY (me, iymax),
		8,
		border
	);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintContours (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	/* mutable autoscaling */ double minimum, /* mutable autoscaling */ double maximum
) {
	double border [1 + 30];
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_unidirectionalAutowindowY (me, & ymin, & ymax);
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	for (integer iborder = 1; iborder <= 30; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (30 + 1);
	if (xmin >= xmax || ymin >= ymax)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_grey (g,
		my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin),
		Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin),
		Matrix_rowToY (me, iymax),
		30,
		border
	);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

static void cellArrayOrImage (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	/* mutable autoscaling */ double minimum, /* mutable autoscaling */ double maximum,
	const bool interpolate
) {
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_unidirectionalAutowindowY (me, & ymin, & ymax);
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx,
		& ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy,
		& iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	if (interpolate)
		Graphics_image (g,
			my z.part (iymin, iymax, ixmin, ixmax),
			Matrix_columnToX (me, ixmin - 0.5),
			Matrix_columnToX (me, ixmax + 0.5),
			Matrix_rowToY (me, iymin - 0.5),
			Matrix_rowToY (me, iymax + 0.5),
			minimum,
			maximum
		);
	else
		Graphics_cellArray (g,
			my z.part (iymin, iymax, ixmin, ixmax),
			Matrix_columnToX (me, ixmin - 0.5),
			Matrix_columnToX (me, ixmax + 0.5),
			Matrix_rowToY (me, iymin - 0.5),
			Matrix_rowToY (me, iymax + 0.5),
			minimum,
			maximum
		);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintImage (const constMatrix me, Graphics g,
	const double xmin, const double xmax,
	const double ymin, const double ymax,
	const double minimum, const double maximum
) {
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, true);
}

void Matrix_paintCells (const constMatrix me, Graphics g,
	const double xmin, const double xmax,
	const double ymin, const double ymax,
	const double minimum, const double maximum
) {
	cellArrayOrImage (me, g, xmin, xmax, ymin, ymax, minimum, maximum, false);
}

void Matrix_paintSurface (const constMatrix me, Graphics g,
	/* mutable autowindow */ double xmin, /* mutable autowindow */ double xmax,
	/* mutable autowindow */ double ymin, /* mutable autowindow */ double ymax,
	/* mutable autoscaling */ double minimum, /* mutable autoscaling */ double maximum,
	const double elevation, const double azimuth
) {
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_unidirectionalAutowindowY (me, & ymin, & ymax);
	integer ixmin, ixmax, iymin, iymax;
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, -1.0, 1.0, minimum, maximum);
	Graphics_surface (g,
		my z.part (iymin, iymax, ixmin, ixmax),
		Matrix_columnToX (me, ixmin),
		Matrix_columnToX (me, ixmax),
		Matrix_rowToY (me, iymin),
		Matrix_rowToY (me, iymax),
		minimum,
		maximum,
		elevation,
		azimuth
	);
	Graphics_unsetInner (g);
}

void Matrix_playMovie (const constMatrix me, Graphics g) {
	Melder_require (my ny >= 2,
		me, U": cannot play a movie for a Matrix with less than 2 rows.");
	if (my xmin == my xmax || my ymin == my ymax)
		return;
	autoVEC column = raw_VEC (my ny);
	/* mutable init */ double minimum = 0.0, maximum = 1.0;
	Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & minimum, & maximum);
	if (minimum == maximum) {
		minimum -= 0.5;
		maximum += 0.5;
	}
	for (integer icol = 1; icol <= my nx; icol ++) {
		column.all()  <<=  my z.column (icol);
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

autoMatrix Matrix_appendRows (const constMatrix me, const constMatrix thee, ClassInfo klas) {
	try {
		autoMatrix him = Thing_newFromClass (klas).static_cast_move <structMatrix>();
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

static bool isSymmetric (const constMatrix me) {
	for (integer irow = 1; irow <= my ny - 1; irow ++)
		for (integer icol = irow + 1; icol <= my nx; icol ++)
			if (my z [irow] [icol] != my z [icol] [irow])
				return false;
	return true;
}

void Matrix_eigen (const constMatrix me, autoMatrix *out_eigenvectors, autoMatrix *out_eigenvalues) {
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

autoMatrix Matrix_power (const constMatrix me, const integer power) {
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

void Matrix_writeToMatrixTextFile (const constMatrix me, MelderFile file) {
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

void Matrix_writeToHeaderlessSpreadsheetFile (const constMatrix me, MelderFile file) {
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

void Matrix_formula (const mutableMatrix me,
	conststring32 expression, Interpreter interpreter, /* mutable default */ mutableMatrix target)
{
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

void Matrix_formula_part (const mutableMatrix me, double xmin, double xmax, double ymin, double ymax,
	conststring32 expression, Interpreter interpreter, /* mutable default */ mutableMatrix target)
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

void Matrix_scaleAbsoluteExtremum (const mutableMatrix me, const double scale) {
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

autoMatrix TableOfReal_to_Matrix (const constTableOfReal me) {
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

autoTableOfReal Matrix_to_TableOfReal (const constMatrix me) {
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
			Table_numericize_a (me, icol);
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
