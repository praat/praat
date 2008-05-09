/* Matrix.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/04/03 Matrix_getValueAtXY
 * pb 2003/06/19 Eigen
 * pb 2003/08/28 Matrix_writeToHeaderlessSpreadsheetFile
 * pb 2005/06/16 units
 * pb 2007/03/18 moved table stuff here
 * pb 2007/05/26 wchar_t
 * pb 2007/06/21 tex
 * pb 2007/10/01 can write as encoding
 * pb 2008/01/19 double
 * pb 2008/04/30 new Formula API
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

static int readText (I, MelderReadString *text) {
	iam (Matrix);
	if (Thing_version < 0) {
		my xmin = texgetr8 (text);
		my xmax = texgetr8 (text);
		my ymin = texgetr8 (text);
		my ymax = texgetr8 (text);
		my nx = texgeti4 (text);
		my ny = texgeti4 (text);
		my dx = texgetr8 (text);
		my dy = texgetr8 (text);
		my x1 = texgetr8 (text);
		my y1 = texgetr8 (text);
	} else {
		inherited (Matrix) readText (me, text);
		my ymin = texgetr8 (text);
		my ymax = texgetr8 (text);
		my ny = texgeti4 (text);
		my dy = texgetr8 (text);
		my y1 = texgetr8 (text);
	}
	if (my xmin > my xmax || my ymin > my ymax)
		return Melder_error1 (L"(Matrix::readText:) xmin should <= xmax and ymin <= ymax.");
	if (my nx < 1 || my ny < 1)
		return Melder_error1 (L"(Matrix::readText:) nx should >= 1 and ny >= 1.");
	if (my dx <= 0 || my dy <= 0)
		return Melder_error1 (L"(Matrix::readText:) dx should > 0 and dy > 0.");
	if (! (my z = NUMdmatrix_readText_r8 (1, my ny, 1, my nx, text, "z"))) return 0;
	return 1;
}

static void info (I) {
	iam (Matrix);
	double minimum = 0, maximum = 0;
	classData -> info (me);
	Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & minimum, & maximum);
	MelderInfo_writeLine2 (L"xmin: ", Melder_double (my xmin));
	MelderInfo_writeLine2 (L"xmax: ", Melder_double (my xmax));
	MelderInfo_writeLine2 (L"Number of columns: ", Melder_integer (my nx));
	MelderInfo_writeLine5 (L"dx: ", Melder_double (my dx), L" (-> sampling rate ", Melder_double (1.0 / my dx), L" )");
	MelderInfo_writeLine2 (L"x1: ", Melder_double (my x1));
	MelderInfo_writeLine2 (L"ymin: ", Melder_double (my ymin));
	MelderInfo_writeLine2 (L"ymax: ", Melder_double (my ymax));
	MelderInfo_writeLine2 (L"Number of rows: ", Melder_integer (my ny));
	MelderInfo_writeLine5 (L"dy: ", Melder_double (my dy), L" (-> sampling rate ", Melder_double (1.0 / my dy), L" )");
	MelderInfo_writeLine2 (L"y1: ", Melder_double (my y1));
	MelderInfo_writeLine2 (L"Minimum value: ", Melder_single (minimum));
	MelderInfo_writeLine2 (L"Maximum value: ", Melder_single (maximum));
}

static double getValueAtSample (I, long isamp, long ilevel, int unit) {
	iam (Matrix);
	double value = my z [ilevel] [isamp];
	return NUMdefined (value) ? our convertStandardToSpecialUnit (me, value, ilevel, unit) : NUMundefined;
}

static double getNrow (I) { iam (Matrix); return my ny; }
static double getNcol (I) { iam (Matrix); return my nx; }
static double getYmin (I) { iam (Matrix); return my ymin; }
static double getYmax (I) { iam (Matrix); return my ymax; }
static double getNy (I) { iam (Matrix); return my ny; }
static double getDy (I) { iam (Matrix); return my dy; }
static double getY (I, long iy) { iam (Matrix); return my y1 + (iy - 1) * my dy; }

static double getMatrix (I, long irow, long icol) {
	iam (Matrix);
	if (irow < 1 || irow > my ny) return 0.0;
	if (icol < 1 || icol > my nx) return 0.0;
	return my z [irow] [icol];
}

static double getFunction2 (I, double x, double y) {
	iam (Matrix);
	double rrow = (y - my y1) / my dy + 1.0;
	double rcol = (x - my x1) / my dx + 1.0;
	long irow = floor (rrow), icol = floor (rcol);
	double drow = rrow - irow, dcol = rcol - icol;
	double z1 = irow < 1 || irow > my ny || icol < 1 || icol > my nx ? 0.0 : my z [irow] [icol];
	double z2 = irow < 0 || irow >= my ny || icol < 1 || icol > my nx ? 0.0 : my z [irow + 1] [icol];
	double z3 = irow < 1 || irow > my ny || icol < 0 || icol >= my nx ? 0.0 : my z [irow] [icol + 1];
	double z4 = irow < 0 || irow >= my ny || icol < 0 || icol >= my nx ? 0.0 : my z [irow + 1] [icol + 1];
	return (1.0 - drow) * (1.0 - dcol) * z1 + drow * (1.0 - dcol) * z2 + (1.0 - drow) * dcol * z3 + drow * dcol * z4;
}

class_methods (Matrix, Sampled) {
	us -> version = 2;
	class_method_local (Matrix, destroy)
	class_method_local (Matrix, description)
	class_method_local (Matrix, copy)
	class_method_local (Matrix, equal)
	class_method_local (Matrix, canWriteAsEncoding)
	class_method_local (Matrix, writeText)
	class_method (readText)
	class_method_local (Matrix, writeBinary)
	class_method_local (Matrix, readBinary)
	class_method (info)
	class_method (getValueAtSample)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getYmin)
	class_method (getYmax)
	class_method (getNy)
	class_method (getDy)
	class_method (getY)
	class_method (getMatrix)
	class_method (getFunction2)
	class_methods_end
}

int Matrix_init
	(I, double xmin, double xmax, long nx, double dx, double x1,
		 double ymin, double ymax, long ny, double dy, double y1)
{
	iam (Matrix);
	if (! Sampled_init (me, xmin, xmax, nx, dx, x1)) return 0;
	my ymin = ymin; my ymax = ymax; my ny = ny; my dy = dy; my y1 = y1;
	if (! (my z = NUMdmatrix (1, my ny, 1, my nx))) return 0;
	return 1;
}

Matrix Matrix_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1)
{
	Matrix me = new (Matrix);
	if (! me || ! Matrix_init (me, xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1)) forget (me);
	return me;
}

Matrix Matrix_createSimple (long numberOfRows, long numberOfColumns) {
	Matrix me = new (Matrix);
	if (! me || ! Matrix_init (me, 0.5, numberOfColumns + 0.5, numberOfColumns, 1, 1,
		0.5, numberOfRows + 0.5, numberOfRows, 1, 1)) forget (me);
	return me;
}

double Matrix_columnToX (I, double column) { iam (Matrix); return my x1 + (column - 1) * my dx; }

double Matrix_rowToY (I, double row) { iam (Matrix); return my y1 + (row - 1) * my dy; }

double Matrix_xToColumn (I, double x) { iam (Matrix); return (x - my x1) / my dx + 1; }

long Matrix_xToLowColumn (I, double x) { iam (Matrix); return (long) floor (Matrix_xToColumn (me, x)); }

long Matrix_xToHighColumn (I, double x) { iam (Matrix); return (long) ceil (Matrix_xToColumn (me, x)); }

long Matrix_xToNearestColumn (I, double x) { iam (Matrix); return (long) floor (Matrix_xToColumn (me, x) + 0.5); }

double Matrix_yToRow (I, double y) { iam (Matrix); return (y - my y1) / my dy + 1; }

long Matrix_yToLowRow (I, double y) { iam (Matrix); return (long) floor (Matrix_yToRow (me, y)); }

long Matrix_yToHighRow (I, double y) { iam (Matrix); return (long) ceil (Matrix_yToRow (me, y)); }

long Matrix_yToNearestRow (I, double y) { iam (Matrix); return (long) floor (Matrix_yToRow (me, y) + 0.5); }

long Matrix_getWindowSamplesX (I, double xmin, double xmax, long *ixmin, long *ixmax) {
	iam (Matrix);
	*ixmin = 1 + (long) ceil  ((xmin - my x1) / my dx);
	*ixmax = 1 + (long) floor ((xmax - my x1) / my dx);
	if (*ixmin < 1) *ixmin = 1;
	if (*ixmax > my nx) *ixmax = my nx;
	if (*ixmin > *ixmax) return 0;
	return *ixmax - *ixmin + 1;
}

long Matrix_getWindowSamplesY (I, double ymin, double ymax, long *iymin, long *iymax) {
	iam (Matrix);
	*iymin = 1 + (long) ceil  ((ymin - my y1) / my dy);
	*iymax = 1 + (long) floor ((ymax - my y1) / my dy);
	if (*iymin < 1) *iymin = 1;
	if (*iymax > my ny) *iymax = my ny;
	if (*iymin > *iymax) return 0;
	return *iymax - *iymin + 1;
}

long Matrix_getWindowExtrema (I, long ixmin, long ixmax, long iymin, long iymax,
	double *minimum, double *maximum)
{
	iam (Matrix);
	long iy, ix;
	if (ixmin == 0) ixmin = 1;
	if (ixmax == 0) ixmax = my nx;
	if (iymin == 0) iymin = 1;
	if (iymax == 0) iymax = my ny;
	if (ixmin > ixmax || iymin > iymax) return 0;
	*minimum = *maximum = my z [iymin] [ixmin];
	for (iy = iymin; iy <= iymax; iy ++)
		for (ix = ixmin; ix <= ixmax; ix ++)
		{
			if (my z [iy] [ix] < *minimum) *minimum = my z [iy] [ix];
			if (my z [iy] [ix] > *maximum) *maximum = my z [iy] [ix];
		}
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

double Matrix_getValueAtXY (I, double x, double y) {
	iam (Matrix);
	long bottomRow, leftCol, topRow, rightCol;
	double drow, dcol;
	double row_real = (y - my y1) / my dy + 1.0;
	double col_real = (x - my x1) / my dx + 1.0;
	/*
	 * We imagine a unit square around every (xi, yi) point in the matrix.
	 * For (x, y) values outside the union of these squares, the z value is undefined.
	 */
	if (row_real < 0.5 || row_real > my ny + 0.5) return NUMundefined;
	if (col_real < 0.5 || col_real > my nx + 0.5) return NUMundefined;
	/*
	 * Determine the four nearest (xi, yi) points.
	 */
	bottomRow = floor (row_real);   /* 0 <= bottomRow <= my ny */
	topRow = bottomRow + 1;         /* 1 <= topRow <= my ny + 1 */
	leftCol = floor (col_real);     /* 0 <= leftCol <= my nx */
	rightCol = leftCol + 1;         /* 1 <= rightCol <= my nx + 1 */
	drow = row_real - bottomRow;    /* 0.0 <= drow < 1.0 */
	dcol = col_real - leftCol;      /* 0.0 <= dcol < 1.0 */
	/*
	 * If adjacent points exist
	 * (i.e., both row numbers are between 1 and my ny,
	 *  or both column numbers are between 1 and my nx),
	 * we do linear interpolation.
	 * If not, we do constant extrapolation,
	 * which can be simulated by an interpolation between equal z values.
	 */
	if (bottomRow < 1) bottomRow = 1;         /* 1 <= bottomRow <= my ny */
	if (topRow > my ny) topRow = my ny;       /* 1 <= topRow <= my ny */
	if (leftCol < 1) leftCol = 1;             /* 1 <= leftCol <= my nx */
	if (rightCol > my nx) rightCol = my nx;   /* 1 <= rightCol <= my nx */
	return (1.0 - drow) * (1.0 - dcol) * my z [bottomRow] [leftCol] +
	       drow * (1.0 - dcol) * my z [topRow] [leftCol] +
	       (1.0 - drow) * dcol * my z [bottomRow] [rightCol] +
	       drow * dcol * my z [topRow] [rightCol];
}

double Matrix_getSum (I) {
	iam (Matrix);
	double sum = 0.0;
	long row, col;
	for (row = 1; row <= my ny; row ++)
		for (col = 1; col <= my nx; col ++)
			sum += my z [row] [col];
	return sum;
}

void Matrix_drawRows (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	iam (Matrix);
	long ixmin, ixmax, iymin, iymax, iy;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	if (xmin >= xmax) return;
	Graphics_setInner (g);
	for (iy = iymin; iy <= iymax; iy ++)
	{
		Graphics_setWindow (g, xmin, xmax,
			minimum - (iy - iymin) * (maximum - minimum),
			maximum + (iymax - iy) * (maximum - minimum));
		Graphics_function (g, my z [iy], ixmin, ixmax,
					Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
	if (iymin < iymax)
		Graphics_setWindow (g, xmin, xmax, my y1 + (iymin - 1.5) * my dy, my y1 + (iymax - 0.5) * my dy);
}

void Matrix_drawOneContour (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double height)
{
	iam (Matrix);
	long ixmin, ixmax, iymin, iymax;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_contour (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		height);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_drawContours (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	iam (Matrix);
	double border [1 + 8];
	long ixmin, ixmax, iymin, iymax, iborder;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (iborder = 1; iborder <= 8; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (8 + 1);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_altitude (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		8, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintContours (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	iam (Matrix);
	double border [1 + 30];
	long ixmin, ixmax, iymin, iymax, iborder;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	for (iborder = 1; iborder <= 30; iborder ++)
		border [iborder] = minimum + iborder * (maximum - minimum) / (30 + 1);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_grey (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		30, border);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

static void cellArrayOrImage (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, int interpolate)
{
	iam (Matrix);
	long ixmin, ixmax, iymin, iymax;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
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
		Graphics_image (g, my z,
			ixmin, ixmax, Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
			iymin, iymax, Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5),
			minimum, maximum);
	else
		Graphics_cellArray (g, my z,
			ixmin, ixmax, Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
			iymin, iymax, Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5),
			minimum, maximum);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void Matrix_paintImage (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (void_me, g, xmin, xmax, ymin, ymax, minimum, maximum, TRUE);
}

void Matrix_paintCells (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum)
{
	cellArrayOrImage (void_me, g, xmin, xmax, ymin, ymax, minimum, maximum, FALSE);
}

void Matrix_paintSurface (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, double elevation, double azimuth)
{
	iam (Matrix);
	long ixmin, ixmax, iymin, iymax;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	(void) Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 1.0; maximum += 1.0; }
	Graphics_setInner (g);
	Graphics_setWindow (g, -1, 1, minimum, maximum);
	Graphics_surface (g, my z,
		ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax),
		iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax),
		minimum, maximum, elevation, azimuth);
	Graphics_unsetInner (g);
}

extern int sginap (long);
void Matrix_movie (I, Graphics g) {
	iam (Matrix);
	double minimum = 0, maximum = 1;
	double *column = NUMdvector (1, my ny);
	Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & minimum, & maximum);
	Graphics_setViewport (g, 0, 1, 0, 1);
	Graphics_setWindow (g, my ymin, my ymax, minimum, maximum);
	for (long icol = 1; icol <= my nx; icol ++) {
		long irow;
		for (irow = 1; irow <= my ny; irow ++)
			column [irow] = my z [irow] [icol];
		Graphics_clearWs (g);
		Graphics_function (g, column, 1, my ny, my ymin, my ymax);
		Graphics_flushWs (g);
		#ifdef sgi
			sginap (2);
		#endif
	}
	NUMdvector_free (column, 1);
}

Matrix Matrix_readAP (MelderFile fs) {
	Matrix me;
	short header [256];
	long i, j;
	double samplingFrequency;
	FILE *f = Melder_fopen (fs, "rb");
	if (! f) return 0;
	for (i = 0; i < 256; i ++)
		header [i] = bingeti2LE (f);
	samplingFrequency = header [100];
	Melder_casual ("Sampling frequency %.10g.", samplingFrequency);
	me = Matrix_create (0, header [34], header [34] /* Number of frames. */, 1, 0.5,
			0, header [35], header [35] /* Number of words per frame. */, 1, 0.5);
	if (! me) { fclose (f); return NULL; }
        /*Mat := MATRIX_create (Buffer.I2 [36], (* Number of words per frame. *)
                           Buffer.I2 [35], (* Number of frames. *)
                           1.0,
                           Buffer.I2 [111] / (* Samples per frame. *)
                           Buffer.I2 [101]); (* Sampling frequency. *)*/
	Melder_casual ("... Loading %d frames of %d words ...", header [34], header [35]);
        for (i = 1; i <= my nx; i ++) for (j = 1; j <= my ny; j ++)
		my z [j] [i] = bingeti2LE (f);

	/* Get pitch frequencies.
	 */
	for (i = 1; i <= my nx; i ++) if (my z [1] [i] != 0.0)
		my z [1] [i] = - samplingFrequency / my z [1] [i];

	fclose (f);
	return me;
}

Matrix Matrix_appendRows (I, thou) {
	iam (Matrix); thouart (Matrix);
	long irow, icol;
	Matrix him;
	him = Matrix_create (my xmin < thy xmin ? my xmin : thy xmin,
		my xmax > thy xmax ? my xmax : thy xmax,
		my nx > thy nx ? my nx : thy nx, my dx, my x1 < thy x1 ? my x1 : thy x1,
		my ymin, my ymax + (thy ymax - thy ymin), my ny + thy ny, my dy, my y1);
	if (! him) return NULL;
	if (our _size == classMatrix -> _size) Thing_overrideClass (him, my methods);
	for (irow = 1; irow <= my ny; irow ++)
		for (icol = 1; icol <= my nx; icol ++)
			his z [irow] [icol] = my z [irow] [icol];
	for (irow = 1; irow <= thy ny; irow ++)
		for (icol = 1; icol <= thy nx; icol ++)
			his z [irow + my ny] [icol] = thy z [irow] [icol];
	return him;
}

Matrix Matrix_readFromRawTextFile (MelderFile fs) {   // BUG: not Unicode-compatible
	FILE *f = NULL;
	Matrix me = NULL;
	long nrow, ncol, nelements, irow, icol;

	f = Melder_fopen (fs, "rb");
	if (! f) goto end;

	/*
	 * Count number of columns.
	 */
	ncol = 0;
	for (;;) {
		int kar = fgetc (f);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
		if (kar == ' ' || kar == '\t') continue;
		ncol ++;
		do {
			kar = fgetc (f);
		} while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\r' && kar != EOF);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
	}
	if (! ncol) error1 (L"File empty")

	/*
	 * Count number of elements.
	 */
	rewind (f);
	nelements = 0;
	for (;;) {
		double element;
		if (fscanf (f, "%lf", & element) < 1) break;   /* Zero or end-of-file. */
		nelements ++;
	}

	/*
	 * Check if all columns are complete.
	 */
	if (! nelements || nelements % ncol)
		error5 (L"The number of elements (", Melder_integer (nelements), L") is not a multiple of the number of columns (", Melder_integer (ncol), L").")

	/*
	 * Create simple matrix.
	 */

	nrow = nelements / ncol;
	me = Matrix_createSimple (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */

	rewind (f);
	for (irow = 1; irow <= nrow; irow ++)
		for (icol = 1; icol <= ncol; icol ++)
			fscanf (f, "%lf", & my z [irow] [icol]);

end:
	Melder_fclose (fs, f);
	iferror { forget (me); return Melder_errorp (
		"(Matrix_readFromRawTextFile:) File %s not read.", MelderFile_messageName (fs)); }
	return me;
}

int Matrix_eigen (I, Matrix *eigenvectors, Matrix *eigenvalues) {
	iam (Matrix);
	Eigen eigen = NULL;
	long i, j;

	*eigenvectors = NULL, *eigenvalues = NULL;
	if (my nx != my ny) return Melder_error1 (L"(Matrix_eigen:) Matrix not square.");
	return Melder_error1 (L"(Matrix_eigen:) Not implemented. Write to the authors.");
	//if ((eigen = Eigen_createFromSymmetricMatrix_f (eigen, my z, my nx)) == NULL) goto end;   // BUG

	*eigenvectors = Data_copy (me); cherror
	*eigenvalues = Matrix_create (1, 1, 1, 1, 1, my ymin, my ymax, my ny, my dy, my y1);
	for (i = 1; i <= my nx; i ++) {
		(*eigenvalues) -> z [i] [1] = eigen -> eigenvalues [i];
		for (j = 1; j <= my nx; j ++)
			(*eigenvectors) -> z [i] [j] = eigen -> eigenvectors [j] [i];
	}
end:
	forget (eigen);
	if (Melder_hasError ()) {
		_Thing_forget ((Thing *) eigenvectors); *eigenvectors = NULL;
		_Thing_forget ((Thing *) eigenvalues); *eigenvalues = NULL;
		return 0;
	}
	return 1;
}

Matrix Matrix_power (I, long power) {
	iam (Matrix);
	long ipow;
	Matrix thee = NULL, him = NULL;
	if (my nx != my ny) return Melder_errorp ("(Matrix_power:) Matrix not square.");
	thee = Data_copy (me);
	him = Data_copy (me);
	if (! thee || ! him) goto end;
	for (ipow = 2; ipow <= power; ipow ++) {
		long irow, icol;
		Matrix tmp;
		tmp = him; him = thee; thee = tmp;
		for (irow = 1; irow <= my ny; irow ++) for (icol = 1; icol <= my nx; icol ++) {
			long i;
			thy z [irow] [icol] = 0.0;
			for (i = 1; i <= my nx; i ++)
				thy z [irow] [icol] += his z [irow] [i] * my z [i] [icol];
		}
	}
end:
	forget (him);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

int Matrix_writeToMatrixTextFile (Matrix me, MelderFile fs) {
	FILE *f = Melder_fopen (fs, "w");
	long i, j;
	if (! f) return 0;
	fprintf (f, "\"ooTextFile\"\n\"Matrix\"\n%.17g %.17g %ld %.17g %.17g\n%.17g %.17g %ld %.17g %.17g\n",
		my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
	for (i = 1; i <= my ny; i ++) {
		for (j = 1; j <= my nx; j ++) {
			if (j > 1) fprintf (f, " ");
			fprintf (f, "%.17g", my z [i] [j]);
		}
		fprintf (f, "\n");
	}
	if (! Melder_fclose (fs, f)) return 0;
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

int Matrix_writeToHeaderlessSpreadsheetFile (Matrix me, MelderFile fs) {
	FILE *f = Melder_fopen (fs, "w");
	long i, j;
	if (! f) return 0;
	for (i = 1; i <= my ny; i ++) {
		for (j = 1; j <= my nx; j ++) {
			if (j > 1) fprintf (f, "\t");
			fprintf (f, "%ls", Melder_single (my z [i] [j]));
		}
		fprintf (f, "\n");
	}
	if (! Melder_fclose (fs, f)) return 0;
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

int Matrix_formula (Matrix me, const wchar_t *expression, Matrix target) {
	struct Formula_Result result;
	Formula_compile (NULL, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	if (target == NULL) target = me;
	for (long irow = 1; irow <= my ny; irow ++) {
		for (long icol = 1; icol <= my nx; icol ++) {
			Formula_run (irow, icol, & result); cherror
			target -> z [irow] [icol] = result. result.numericResult;
		}
	}
end:
	iferror return 0;
	return 1;
}

void Matrix_scaleAbsoluteExtremum (I, double scale) { iam (Matrix);
	double extremum = 0.0;
	for (long i = 1; i <= my ny; i ++) {
		for (long j = 1; j <= my nx; j ++) {
			if (fabs (my z [i] [j]) > extremum) {
				extremum = fabs (my z [i] [j]);
			}
		}
	}
	if (extremum != 0.0) {
		double factor = scale / extremum;
		for (long i = 1; i <= my ny; i ++) {
			for (long j = 1; j <= my nx; j ++) {
				my z [i] [j] *= factor;
			}
		}
	}
}

Matrix TableOfReal_to_Matrix (I) {
	iam (TableOfReal);
	long i, j;
	Matrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns); cherror
	for (i = 1; i <= my numberOfRows; i ++) for (j = 1; j <= my numberOfColumns; j ++)
		thy z [i] [j] = my data [i] [j];
end:
	iferror return NULL;
	return thee;
}

TableOfReal Matrix_to_TableOfReal (I) {
	iam (Matrix);
	long i, j;
	TableOfReal thee = TableOfReal_create (my ny, my nx); cherror
	for (i = 1; i <= my ny; i ++) for (j = 1; j <= my nx; j ++)
		thy data [i] [j] = my z [i] [j];
end:
	iferror return NULL;
	return thee;
}

Matrix Table_to_Matrix (Table me) {
	long irow, icol;
	Matrix thee = Matrix_createSimple (my rows -> size, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		Table_numericize (me, icol);
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			thy z [irow] [icol] = row -> cells [icol]. number;
		}
	}
end:
	iferror return NULL;
	return thee;
}

/* End of file Matrix.c */
