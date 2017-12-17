/* Graphics_extensions.cpp
 *
 * Copyright (C) 2012-2015 David Weenink
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
 djmw 20120727 latest modification
*/

#include "Graphics_extensions.h"
#include "NUM2.h"
#include "Permutation.h"

/*
	Draw a box plot of data[1..ndata]. The vertical center line of the plot
	is at position 'x'. The rectangle box is 2*w wide, the whisker 2*r.
	All drawing outside [ymin, ymax] is clipped.
*/

void Graphics_boxAndWhiskerPlot (Graphics g, double data[], integer ndata, double x, double r, double w, double ymin, double ymax) {
	int lineType = Graphics_inqLineType (g);

	Melder_assert (r > 0.0 && w > 0.0);
	if (ndata < 3) {
		return;
	}
	/*
		Sort the data (ascending: data[1] <= ... <= data[ndata]).
		Get the median (q50) and the upper and lower quartile points
		(q25 and q75).
		Now q25 and q75 are the lower and upper hinges, respectively.
		The fances can be calcultaed from q25 and q75.
		The spread is defined as the interquartile range or midrange
		|q75 - q25|.
		The fences are defined as:
		(lower/upper) innerfence = (lower/upper) hinge +/- 1.5 hspread
		(lower/upper) outerfence = (lower/upper) hinge +/- 3.0 hspread
	*/

	NUMsort_d (ndata, data);

	if (ymax <= ymin) {
		ymin = data[1]; ymax = data[ndata];
	}
	if (data[1] > ymax || data[ndata] < ymin) {
		return;
	}

	double mean = 0.0;
	for (integer i = 1; i <= ndata; i++) {
		mean += data[i];
	}
	mean /= ndata;

	double q25 = NUMquantile (ndata, data, 0.25);
	double q50 = NUMquantile (ndata, data, 0.5);
	double q75 = NUMquantile (ndata, data, 0.75);

	double hspread = fabs (q75 - q25);
	double lowerOuterFence = q25 - 3.0 * hspread;
	double lowerInnerFence = q25 - 1.5 * hspread;
	double upperInnerFence = q75 + 1.5 * hspread;
	double upperOuterFence = q75 + 3.0 * hspread;

	/*
		Decide whether there are outliers that have to be drawn.
		First process data from below (data are sorted).
	*/

	integer i = 1, ie = ndata;
	while (i <= ie && data[i] < ymin) {
		i++;
	}
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	while (i <= ie && data[i] < lowerOuterFence) {
		Graphics_text (g, x, data[i], U"o"); i++;
	}
	while (i <= ie && data[i] < lowerInnerFence) {
		Graphics_text (g, x, data[i], U"*"); i++;
	}
	double lowerWhisker = data[i] < q25 ? data[i] : lowerInnerFence;
	if (lowerWhisker > ymax) {
		return;
	}

	// Next process data from above.

	i = ndata; ie = i;
	while (i >= ie && data[i] > ymax) {
		i--;
	}
	while (i >= ie && data[i] > upperOuterFence) {
		Graphics_text (g, x, data[i], U"o"); i--;
	}
	while (i >= ie && data[i] > upperInnerFence) {
		Graphics_text (g, x, data[i], U"*"); i--;
	}
	double upperWhisker = data[i] > q75 ? data[i] : upperInnerFence;
	if (upperWhisker < ymin) {
		return;
	}

	/*
		Determine what parts of the "box" have to be drawn within the
		range [ymin, ymax].
		Horizontal lines first.
	*/

	double y1 = lowerWhisker;
	if (ymax > y1 && ymin < y1) {
		Graphics_line (g, x - r, y1, x + r, y1);
	}
	y1 = q25;
	if (ymax > y1 && ymin < y1) {
		Graphics_line (g, x - w, y1, x + w, y1);
	}
	y1 = q50;
	if (ymax > y1 && ymin < y1) {
		Graphics_line (g, x - w, y1, x + w, y1);
	}
	y1 = q75;
	if (ymax > y1 && ymin < y1) {
		Graphics_line (g, x - w, y1, x + w, y1);
	}
	y1 = upperWhisker;
	if (ymax > y1 && ymin < y1) {
		Graphics_line (g, x - r, y1, x + r, y1);
	}

	// Extension: draw the mean too.

	y1 = mean;
	if (ymax > y1 && ymin < y1) {
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, x - w, y1, x + w, y1);
		Graphics_setLineType (g, lineType);
	}

	// Now process the vertical lines.

	y1 = lowerWhisker;
	double y2 = q25;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x, y1, x, y2);
	}
	y1 = q25; y2 = q75;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x - w, y1, x - w, y2);
		Graphics_line (g, x + w, y1, x + w, y2);
	}
	y1 = q75; y2 = upperWhisker;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x, y1, x, y2);
	}
}

void Graphics_quantileQuantilePlot (Graphics g, integer numberOfQuantiles, double xdata[], integer xnumberOfData, double ydata[], integer ynumberOfData, double xmin, double xmax, double ymin, double ymax, int labelSize, const char32 *plotLabel) {
	int fontSize = Graphics_inqFontSize (g);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	autoNUMvector<double> xsorted (NUMvector_copy<double> (xdata, 1, xnumberOfData), 1);
	autoNUMvector<double> ysorted (NUMvector_copy<double> (ydata, 1, ynumberOfData), 1);
	NUMsort_d (xnumberOfData, xsorted.peek());
	NUMsort_d (ynumberOfData, ysorted.peek());

	integer numberOfData = xnumberOfData < ynumberOfData ? xnumberOfData : ynumberOfData;
	numberOfQuantiles = numberOfData < numberOfQuantiles ? numberOfData : numberOfQuantiles;
	double un = pow (0.5, 1.0 / numberOfQuantiles);
	double u1 = 1.0 - un;
	if (xmin == xmax) {
		xmin = NUMquantile (xnumberOfData, xsorted.peek(), u1);
		xmax = NUMquantile (xnumberOfData, xsorted.peek(), un);
	}
	if (ymin == ymax) {
		ymin = NUMquantile (ynumberOfData, ysorted.peek(), u1);
		ymax = NUMquantile (ynumberOfData, ysorted.peek(), un);
	}
	for (integer i = 1; i <= numberOfQuantiles; i++) {
		double ui = i == 1 ? u1 : (i == numberOfQuantiles ? un : (i - 0.3175) / (numberOfQuantiles + 0.365));
		double qx = NUMquantile (xnumberOfData, xsorted.peek(), ui);
		double qy = NUMquantile (ynumberOfData, ysorted.peek(), ui);
		if (qx < xmin || qx > xmax || qy < ymin || qy > ymax) continue; // outside area
		Graphics_text (g, qx, qy, plotLabel);
	}
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, xmin, ymin, xmax, ymax);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setFontSize (g, fontSize);
}

void Graphics_matrixAsSquares (Graphics g, double **matrix, integer numberOfRows, integer numberOfColumns, double zmin, double zmax, double cellSizeFactor, bool randomFillOrder) {
	integer numberOfCells = numberOfRows * numberOfColumns;
	autoPermutation p = Permutation_create (numberOfCells);
	if (randomFillOrder) {
		Permutation_permuteRandomly_inplace (p.get(), 1, numberOfCells);
	}
	double zAbsMax = fabs (zmax) > fabs (zmin) ? fabs (zmax) : fabs (zmin);
	Graphics_Colour colour = Graphics_inqColour (g);
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (g, &x1WC, &x2WC, &y1WC, &y2WC);
	double dx = fabs (x2WC - x1WC) / numberOfColumns;
	double dy = fabs (y2WC - y1WC) / numberOfRows;
	for (integer i = 1; i <= numberOfCells; i++) {
		integer index = Permutation_getValueAtIndex (p.get(), i);
		integer irow = (index - 1) / numberOfColumns + 1;
		integer icol = (index - 1) % numberOfColumns + 1;
		double z = matrix[irow][icol];
		z = z < zmin ? zmin : z;
		z = z > zmax ? zmax : z;
		double zweight = sqrt (fabs (z) / zAbsMax); // Area length^2)
		double xcenter = (icol - 0.5) * dx;
		double ycenter = (irow - 0.5) * dy;
		double x1 = x1WC + xcenter - zweight * 0.5 * dx * cellSizeFactor;
		x1 = x1 < x1WC ? x1WC : x1;
		double x2 = x1WC + xcenter + zweight * 0.5 * dx * cellSizeFactor;
		x2 = x2 > x2WC ? x2WC : x2;
		double y1 = y1WC + ycenter - zweight * 0.5 * dy * cellSizeFactor;
		y1 = y1 < y1WC ? y1WC : y1;
		double y2 = y1WC + ycenter + zweight * 0.5 * dy * cellSizeFactor;
		y2 = y2 > y2WC ? y2WC : y2;
		if (z > 0.0) {
			Graphics_setColour (g, Graphics_WHITE);
		}
		Graphics_fillRectangle (g, x1, x2, y1, y2);
		Graphics_setColour (g, colour);
		Graphics_rectangle (g, x1, x2 , y1, y2);
	}
}

void Graphics_lagPlot (Graphics g, double data[], integer numberOfData, double xmin, double xmax, integer lag, int labelSize, const char32 *plotLabel) {
	if (lag < 0 || lag >= numberOfData) {
		return;
	}
	int fontSize = Graphics_inqFontSize (g);
	Graphics_setFontSize (g, labelSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	// plot x[i] vertically and x[i-lag] horizontally
	for (integer i = 1; i <= numberOfData - lag; i++) {
		double x = data[i + lag], y = data[i];
		if (x >= xmin && x <= xmax && y >= xmin && y <= xmax) {
			Graphics_text (g, x, y, plotLabel);
		}
	}
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setFontSize (g, fontSize);
}

/* End of file Graphics_extensions.cpp */
