#ifndef _Matrix_extensions_h_
#define _Matrix_extensions_h_
/* Matrix_extensions.h
 *
 * Copyright (C) 1993-2020 David Weenink
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
 djmw 20020813 GPL header
 djmw 20121110 Latest modification
*/

#include "Matrix.h"
#include "Eigen.h"
#include "Graphics.h"
#include "Graphics_extensions.h"
#include "SVD.h"

autoMatrix Matrix_readFromIDXFormatFile (MelderFile file);
autoDaata IDXFormattedMatrixFileRecognizer (integer numberOfBytesRead, const char *header, MelderFile file);

void Matrix_scatterPlot (Matrix me, Graphics g, integer icx, integer icy,
    double xmin, double xmax, double ymin, double ymax,
    double size_mm, conststring32 mark, bool garnish);
/* Draw my columns ix and iy as a scatterplot (with squares)				*/

void Matrix_drawAsSquares_inside (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax, kGraphicsMatrixOrigin origin, double cellAreaScaleFactor, kGraphicsMatrixCellDrawingOrder drawingOrder);

void Matrix_drawAsSquares (Matrix me, Graphics graphics, double xmin, double xmax, double ymin, double ymax, bool garnished);
/* Draw a Matrix as small squares whose area correspond to the matrix element */
/* The square is filled with black if the weights are negative					*/

void Matrix_drawRowsAsLineSegments (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	double minimum, double maximum, int connect);
/* draw a row as segments */

void Matrix_scale (Matrix me, int choice);
/* choice = 1 :divide each elmnt by the maximum (abs) */
/* choice = 2 :rows, divide each row elmnt by the maximum (abs) of that row	*/
/* choice = 3 :columns, divide each col elmnt by the maximum of that col	*/

autoMatrix Matrix_transpose (Matrix me);

void Matrix_drawDistribution (Matrix me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double minimum, double maximum, integer nBins,
	double freqMin, double freqmax, bool cumulative, bool garnish);

void Matrix_drawSliceY (Matrix me, Graphics g, double x, double ymin, double ymax,
	double min, double max);

int Matrix_fitPolynomial (Matrix me, integer maxDegree);

autoMatrix Matrix_solveEquation (Matrix me, double tolerance);

autoMatrix Matrix_solveEquation (Matrix me, Matrix thee, double tolerance); // Me * X = Thee

double Matrix_getMean (Matrix me, double xmin, double xmax, double ymin, double ymax);

double Matrix_getStandardDeviation (Matrix me, double xmin, double xmax, double ymin, double ymax);

autoEigen Matrix_to_Eigen (Matrix me);
/* Symmetric matrix */

autoMatrix SVD_to_Matrix (SVD me, integer from, integer to);

autoCOMPVEC Matrix_listEigenvalues (Matrix me);
void Matrix_Eigen_complex (Matrix me, autoMatrix *out_eigenvectors, autoMatrix *out_eigenvalues);
/* General square matrix */

#endif /* _Matrix_extensions_h_ */
