/* Matrix_extensions.cpp
 *
 * Copyright (C) 1993-2016 David Weenink
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
 djmw 20040226 Matrix_drawAsSquares: respect the colour environment (fill with current colour).
 djmw 20041110 Matrix_drawDistribution did't draw lowest bin correctly.
 djmw 20050221 Matrix_drawDistribution would draw outside window.
 djmw 20050405 Matrix_drawDistribution crashed if minimum > data minimum5
 djmw 20080122 float -> double
*/

#include "Matrix_extensions.h"
#include "Eigen.h"
#include "NUM2.h"

void Matrix_scatterPlot (Matrix me, Graphics g, long icx, long icy, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	long ix = labs (icx), iy = labs (icy);

	if (ix < 1 || ix > my nx || iy < 1 || iy > my nx) {
		return;
	}
	if (xmax <= xmin) {
		(void) Matrix_getWindowExtrema (me, ix, ix, 1, my ny, & xmin, & xmax);
		if (xmax <= xmin) {
			xmin -= 0.5; xmax += 0.5;
		}
	}
	if (ymax <= ymin) {
		(void) Matrix_getWindowExtrema (me, iy, iy, 1, my ny, & ymin, & ymax);
		if (ymax <= ymin) {
			ymin -= 0.5; ymax += 0.5;
		}
	}
	Graphics_setInner (g);
	if (icx < 0) {
		double t = xmin;
		xmin = xmax;
		xmax = t;
	}
	if (icy < 0) {
		double t = ymin;
		ymin = ymax;
		ymax = t;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= my ny; i++) {
		if (my z[i][ix] >= xmin && my z[i][ix] <= xmax && my z[i][iy] >= ymin && my z[i][iy] <= ymax) {
			Graphics_mark (g, my z[i][ix], my z[i][iy], size_mm, mark);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		}
		Graphics_marksBottom (g, 2, true, true, false);
		if (xmin * xmax < 0.0) {
			Graphics_markBottom (g, 0.0, true, true, true, nullptr);
		}
	}
}

void Matrix_drawAsSquares (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish) {
	Graphics_Colour colour = Graphics_inqColour (g);
	long ixmin, ixmax, iymin, iymax;

	if (xmax <= xmin) {
		xmin = my xmin;
		xmax = my xmax;
	}
	long nx = Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax);
	if (ymax <= ymin) {
		ymin = my ymin;
		ymax = my ymax;
	}
	long ny = Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax);
	double min, max = nx > ny ? nx : ny;
	double dx = (xmax - xmin) / max, dy = (ymax - ymin) / max;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & min, & max);
	double wAbsMax = fabs (max) > fabs (min) ? fabs (max) : fabs (min);
	for (long i = iymin; i <= iymax; i++) {
		double y = Matrix_rowToY (me, i);
		for (long j = ixmin; j <= ixmax; j++) {
			double x = Matrix_columnToX (me, j);
			double d = 0.95 * sqrt (fabs (my z[i][j]) / wAbsMax);
			if (d > 0) {
				double x1WC = x - d * dx / 2, x2WC = x + d * dx / 2;
				double y1WC = y - d * dy / 2, y2WC = y + d * dy / 2;
				if (my z[i][j] > 0) {
					Graphics_setColour (g, Graphics_WHITE);
				}
				Graphics_fillRectangle (g, x1WC, x2WC, y1WC, y2WC);
				Graphics_setColour (g, colour);
				Graphics_rectangle (g, x1WC, x2WC , y1WC, y2WC);
			}
		}
	}
	Graphics_setGrey (g, 0.0);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		}
		Graphics_marksBottom (g, 2, true, true, false);
		if (xmin * xmax < 0.0) {
			Graphics_markBottom (g, 0.0, true, true, true, nullptr);
		}
	}
}

void Matrix_scale (Matrix me, int choice) {
	double min, max, extremum;
	long nZero = 0;

	if (choice == 2) { /* by row */
		for (long i = 1; i <= my ny; i++) {
			Matrix_getWindowExtrema (me, 1, my nx, i, i, &min, &max);
			extremum = fabs (max) > fabs (min) ? fabs (max) : fabs (min);
			if (extremum == 0.0) {
				nZero++;
			} else for (long j = 1; j <= my nx; j++) {
					my z[i][j] /= extremum;
				}
		}
	} else if (choice == 3) { /* by col */
		for (long j = 1; j <= my nx; j++) {
			Matrix_getWindowExtrema (me, j, j, 1, my ny, &min, &max);
			extremum =  fabs (max) > fabs (min) ? fabs (max) : fabs (min);
			if (extremum == 0.0) {
				nZero++;
			} else for (long i = 1; i <= my ny; i++) {
					my z[i][j] /= extremum;
				}
		}
	} else if (choice == 1) { /* overall */
		Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, &min, &max);
		extremum =  fabs (max) > fabs (min) ? fabs (max) : fabs (min);
		if (extremum == 0.0) {
			nZero++;
		} else {
			for (long i = 1; i <= my ny; i++) {
				for (long j = 1; j <= my nx; j++) {
					my z[i][j] /= extremum;
				}
			}
		}
	} else {
		Melder_flushError (U"Matrix_scale: choice must be > 0 && <= 3.");
		return;
	}
	if (nZero) {
		Melder_warning (U"Matrix_scale: extremum == 0, (part of) matrix unscaled.");
	}
}

autoMatrix Matrix_transpose (Matrix me) {
	try {
		autoMatrix thee = Matrix_create (my ymin, my ymax, my ny, my dy, my y1, my xmin, my xmax, my nx, my dx, my x1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				thy z[j][i] = my z[i][j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not transposed.");
	}
}

void Matrix_drawDistribution (Matrix me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum,
	long nBins, double freqMin, double freqMax, bool cumulative, bool garnish)
{
	if (nBins <= 0) {
		return;
	}
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymax <= ymin) {
		ymin = my ymin; ymax = my ymax;
	}
	long ixmin, ixmax, iymin, iymax;
	if ((Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax) == 0) || 
		(Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax) == 0)) {
		return;
	}
	if (maximum <= minimum) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	}
	if (maximum <= minimum) {
		minimum -= 1.0; maximum += 1.0;
	}

	// Count the numbers per bin and the total

	if (nBins < 1) {
		nBins = 10;
	}
	autoNUMvector<long> freq (1, nBins);
	double binWidth = (maximum - minimum) / nBins;
	long nxy = 0;
	for (long i = iymin; i <= iymax; i++) {
		for (long j = ixmin; j <= ixmax; j++) {
			long bin = 1 + (long) floor ( (my z[i][j] - minimum) / binWidth);
			if (bin <= nBins && bin > 0) {
				freq[bin]++; nxy ++;
			}
		}
	}

	if (freqMax <= freqMin) {
		if (cumulative) {
			freqMin = 0; freqMax = 1.0;
		} else {
			NUMvector_extrema (freq.peek(), 1, nBins, & freqMin, & freqMax);
			if (freqMax <= freqMin) {
				freqMin = freqMin > 1.0 ? freqMin - 1.0 : 0.0;
				freqMax += 1.0;
			}
		}
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, minimum, maximum, freqMin, freqMax);
	double fi = 0.0;
	for (long i = 1; i <= nBins; i++) {
		double ftmp = freq[i];
		fi = cumulative ? fi + freq[i] / nxy : freq[i];
		ftmp = fi;
		if (ftmp > freqMax) {
			ftmp = freqMax;
		}
		if (ftmp > freqMin) {
			Graphics_rectangle (g, minimum + (i - 1) * binWidth, minimum + i * binWidth, freqMin, ftmp);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (! cumulative) {
			Graphics_textLeft (g, true, U"Number/bin");
		}
	}
}

void Matrix_drawSliceY (Matrix me, Graphics g, double x, double ymin, double ymax, double min, double max) {

	if (x < my xmin || x > my xmax) {
		return;
	}
	long ix = Matrix_xToNearestColumn (me, x);

	if (ymax <= ymin) {
		ymin = my ymin;
		ymax = my ymax;
	}

	long iymin, iymax;
	long ny = Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax);
	if (ny < 1) {
		return;
	}

	if (max <= min) {
		Matrix_getWindowExtrema (me, ix, ix, iymin, iymax, &min, &max);
	}
	if (max <= min) {
		min -= 0.5; max += 0.5;
	}
	autoNUMvector<double> y (iymin, iymax);

	Graphics_setWindow (g, ymin, ymax, min, max);
	Graphics_setInner (g);

	for (long i = iymin; i <= iymax; i++) {
		y[i] = my z[i][ix];
	}
	Graphics_function (g, y.peek(), iymin, iymax, Matrix_rowToY (me, iymin), Matrix_rowToY (me, iymax));
	Graphics_unsetInner (g);
}

autoMatrix Matrix_solveEquation (Matrix me, double /* tolerance */) {
	try {
		long nr = my ny, nc = my nx - 1;

		if (nc == 0) {
			Melder_throw (U"Matrix_solveEquation: there must be at least 2 columns in the matrix.");
		}
		if (nr < nc) {
			Melder_warning (U"Matrix_solveEquation: solution is not unique (fewer equations than unknowns).");
		}

		autoNUMmatrix<double> u (1, nr, 1, nc);
		autoNUMvector<double> b (1, nr);
		autoNUMvector<double> x (1, nc);
		autoMatrix thee = Matrix_create (0.5, 0.5 + nc, nc, 1, 1, 0.5, 1.5, 1, 1, 1);

		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				u[i][j] = my z[i][j];
			}
			b[i] = my z[i][my nx];
		}

		NUMsolveEquation (u.peek(), nr, nc, b.peek(), 0, x.peek());
		for (long j = 1; j <= nc; j++) {
			thy z[1][j] = x[j];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Matrix equation not solved.");
	}
}

double Matrix_getMean (Matrix me, double xmin, double xmax, double ymin, double ymax) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymax <= ymin) {
		ymin = my ymin; ymax = my ymax;
	}
	long ixmin, ixmax, iymin, iymax;
	if ((Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax) == 0) ||
		(Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax) == 0)) {
		return undefined;
	}
	double sum = 0.0;
	for (long row = iymin; row <= iymax; row++) {
		for (long col = ixmin; col <= ixmax; col++) {
			sum += my z[row][col];
		}
	}
	return sum / ((iymax - iymin + 1) * (ixmax - ixmin + 1));
}

double Matrix_getStandardDeviation (Matrix me, double xmin, double xmax, double ymin, double ymax) {
	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymax <= ymin) {
		ymin = my ymin; ymax = my ymax;
	}
	long ixmin, ixmax, iymin, iymax;
	if ((Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax) == 0) ||
		(Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax) == 0)) {
		return undefined;
	}
	long nx = ixmax - ixmin + 1, ny = iymax - iymin + 1;
	if (nx == 1 && ny == 1) {
		return undefined;
	}
	double mean = Matrix_getMean (me, xmin, xmax, ymin, ymax), sum = 0;
	for (long row = iymin; row <= iymax; row++) {
		for (long col = ixmin; col <= ixmax; col++) {
			double val = my z[row][col] - mean;
			sum += val * val;
		}
	}
	return sqrt (sum / (nx * ny - 1));
}

autoDaata IDXFormattedMatrixFileRecognizer (int numberOfBytesRead, const char *header, MelderFile file) {
	unsigned int numberOfDimensions, type, pos = 4;
	/* 
	 * 9: minumum size is 4 bytes (magic number) + 4 bytes for 1 dimension + 1 value of 1 byte
	 */
	if (numberOfBytesRead < 9 || header[0] != 0 ||  header[1] != 0 || (type = header[2]) < 8 ||
		numberOfBytesRead < 4 + (numberOfDimensions = header[3]) * 4) { // each dimension occupies 4 bytes
		return autoDaata ();
	}
	trace (U"dimensions = ", numberOfDimensions, U" type = ", type);
	/*
	 * Check if the file size (bytes) equals the number of data cells in the matrix times the size of each cell (bytes) plus thee
	 * offset of the data (4 + numberOfDimensions * 4)
	 */ 
	double numberOfCells = 1.0; // double because sizes of the dimensions could turn out to be very large if not an IDX format file
	for (long i = 1; i <= numberOfDimensions; i ++, pos += 4) {
		unsigned char b1 = header[pos], b2 = header[pos + 1], b3 = header[pos + 2], b4 = header[pos + 3];
		long size = ((uint32) b1 << 24) + ((uint32) b2 << 16) + ((uint32) b3 << 8) + (uint32) b4;
		trace (U"size = ", size, U" ", b1, U" ", b2, U" ", b3, U" ", b4);
		numberOfCells *= size;
	}
	trace (U"Number of cells =", numberOfCells);
	/*
	 * Check how many bytes each cell needs
	 */
	long cellSizeBytes = (type == 0x08 || type == 0x09) ? 1 : type == 0x0B ? 2 : (type == 0x0C || type == 0x0D) ? 4 : type == 0x0E ? 8 : 0;
	if (cellSizeBytes == 0) {
		return autoDaata ();
	}
	trace (U"Cell size =", cellSizeBytes);
	double numberOfBytes = numberOfCells * cellSizeBytes + 4 + numberOfDimensions * 4;
	trace (U"Number of bytes =", numberOfBytes);
	long numberOfBytesInFile = MelderFile_length (file);
	trace (U"File size = ", numberOfBytesInFile);
	if (numberOfBytes > numberOfBytesInFile || (long) numberOfBytes < numberOfBytesInFile) { // may occur if it is not an IDX file
		return autoDaata ();
	}
	autoMatrix thee = Matrix_readFromIDXFormatFile (file);
	return thee.move();
}


autoMatrix Matrix_readFromIDXFormatFile (MelderFile file) {
	/*
		From: http://yann.lecun.com/exdb/mnist/
		
		The IDX file format is a simple format for vectors and multidimensional matrices of various numerical types.

		The basic format is

			magic number
			size in dimension 0
			size in dimension 1
			size in dimension 2
			....
			size in dimension N
		data

		The magic number is an integer (MSB first). The first 2 bytes are always 0.

		The third byte codes the type of the data:
		0x08: unsigned byte
		0x09: signed byte
		0x0B: short (2 bytes)
		0x0C: int (4 bytes)
		0x0D: float (4 bytes)
		0x0E: double (8 bytes)

		The 4-th byte codes the number of dimensions of the vector/matrix: 1 for vectors, 2 for matrices....

		The sizes in each dimension are 4-byte integers (MSB first, big endian, like in most non-Intel processors).

		The data is stored like in a C array, i.e. the index in the last dimension changes the fastest. 

	*/
	try {
		autofile f = Melder_fopen (file, "r");
		unsigned int b1 = bingetu8 (f);   // 0
		unsigned int b2 = bingetu8 (f);   // 0
		if (b1 != 0 || b2 != 0) {
			Melder_throw (U"Starting two bytes should be zero.");
		}
		unsigned int b3 = bingetu8 (f);   // data type
		unsigned int b4 = bingetu8 (f);   // number of dimensions
		long ncols = bingeti32 (f), nrows = 1;   // ok if vector
		if (b4 > 1) {
			nrows = ncols;
			ncols = bingeti32 (f);
		}
		while (b4 > 2) {   // accumulate all other dimensions in the columns
			long n2 = bingeti32 (f);
			ncols *= n2;   // put the matrix in one row
			-- b4;
		}
		autoMatrix me = Matrix_create (0.0, ncols, ncols, 1, 0.5, 0, nrows, nrows, 1.0, 0.5);
		if (b3 == 0x08) {   // 8 bits unsigned
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingetu8 (f);
				}
			}
		} else if (b3 == 0x09) {   // 8 bits signed
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingeti8 (f);
				}
			}
		} else if (b3 == 0x0B) {   // 16 bits signed
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingeti16 (f);
				}
			}
		} else if (b3 == 0x0C) {   // 32 bits signed
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingeti32 (f);
				}
			}
		} else if (b3 == 0x0D) {   // 32-bits IEEE floating point
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingetr32 (f);
				}
			}
		} else if (b3 == 0x0E) {   // 64-bits IEEE floating point
			for (long irow = 1; irow <= nrows; irow ++) {
				for (long icol = 1; icol <= ncols; icol ++) {
					my z [irow] [icol] = bingetr64 (f);
				}
			}
		} else {
			Melder_throw (U"Not a valid data type.");
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot read from IDX format file ", MelderFile_messageName (file), U".");
	}
}

/* End of file Matrix_extensions.cpp */
