/* Graphics_extensions.cpp
 *
 * Copyright (C) 2012-2020 David Weenink
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

#include "NUM2.h"
#include "Permutation.h"
#include "Graphics_extensions.h"

#include "enums_getText.h"
#include "Graphics_extensions_enums.h"
#include "enums_getValue.h"
#include "Graphics_extensions_enums.h"

/*
	Draw a box plot of x[1..x.size]. The vertical center line of the plot
	is at position 'x'. The rectangle box is 2*w wide, the whisker 2*r.
	All drawing outside [ymin, ymax] is clipped.
*/

void Graphics_boxAndWhiskerPlot (Graphics g, constVEC data, double x, double r, double w, double ymin, double ymax) {
	const int lineType = Graphics_inqLineType (g);

	Melder_assert (r > 0.0 && w > 0.0);
	if (data.size < 3)
		return;
	/*
		Sort the data (ascending: x[1] <= ... <= x[ndata]).
		Get the median (q50) and the upper and lower quartile points
		(q25 and q75).
		Now q25 and q75 are the lower and upper hinges, respectively.
		The fences can be calculated from q25 and q75.
		The spread is defined as the interquartile range or midrange
		|q75 - q25|.
		The fences are defined as:
		(lower/upper) innerfence = (lower/upper) hinge +/- 1.5 hspread
		(lower/upper) outerfence = (lower/upper) hinge +/- 3.0 hspread
	*/
	
	autoVEC sorted = sort_VEC (data);

	if (ymax <= ymin) {
		ymin = sorted [1];
		ymax = sorted [sorted.size];
	}
	if (sorted [1] > ymax || sorted [sorted.size] < ymin)
		return;

	const double mean = NUMmean (sorted.get());
	const double q25 = NUMquantile (sorted.get(), 0.25);
	const double q50 = NUMquantile (sorted.get(), 0.5);
	const double q75 = NUMquantile (sorted.get(), 0.75);

	const double hspread = fabs (q75 - q25);
	const double lowerOuterFence = q25 - 3.0 * hspread;
	const double lowerInnerFence = q25 - 1.5 * hspread;
	const double upperInnerFence = q75 + 1.5 * hspread;
	const double upperOuterFence = q75 + 3.0 * hspread;
	/*
		Decide whether there are outliers that have to be drawn.
		First process sorted from below.
	*/
	integer i = 1;
	while (i <= sorted.size && sorted [i] < ymin)
		i ++;
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	while (i <= sorted.size && sorted [i] < lowerOuterFence)
		Graphics_text (g, x, sorted [i ++], U"o");

	while (i <= sorted.size && sorted[i] < lowerInnerFence)
		Graphics_text (g, x, sorted [i ++], U"*");

	const double lowerWhisker = sorted [i] < q25 ? sorted [i] : lowerInnerFence;
	if (lowerWhisker > ymax)
		return;
	/*
		Next process data from above.
	*/
	const integer ilow = i;
	i = sorted.size;
	while (i >= ilow && sorted [i] > ymax)
		i --;
	while (i >= ilow && sorted [i] > upperOuterFence)
		Graphics_text (g, x, sorted [i --], U"o");

	while (i >= ilow && sorted [i] > upperInnerFence)
		Graphics_text (g, x, sorted [i --], U"*");

	const double upperWhisker = sorted [i] > q75 ? sorted [i] : upperInnerFence;
	if (upperWhisker < ymin)
		return;
	/*
		Determine what parts of the "box" have to be drawn within the
		range [ymin, ymax].
		Horizontal lines first.
	*/
	double y1 = lowerWhisker;
	if (ymax > y1 && ymin < y1)
		Graphics_line (g, x - r, y1, x + r, y1);
	y1 = q25;
	if (ymax > y1 && ymin < y1)
		Graphics_line (g, x - w, y1, x + w, y1);
	y1 = q50;
	if (ymax > y1 && ymin < y1)
		Graphics_line (g, x - w, y1, x + w, y1);
	y1 = q75;
	if (ymax > y1 && ymin < y1)
		Graphics_line (g, x - w, y1, x + w, y1);
	y1 = upperWhisker;
	if (ymax > y1 && ymin < y1)
		Graphics_line (g, x - r, y1, x + r, y1);
	/*
		Extension: draw the mean too.
	*/
	y1 = mean;
	if (ymax > y1 && ymin < y1) {
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, x - w, y1, x + w, y1);
		Graphics_setLineType (g, lineType);
	}
	/*
		Vertical lines.
	*/
	y1 = lowerWhisker;
	double y2 = q25;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x, y1, x, y2);
	}
	y1 = q25;
	y2 = q75;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x - w, y1, x - w, y2);
		Graphics_line (g, x + w, y1, x + w, y2);
	}
	y1 = q75;
	y2 = upperWhisker;
	if (ymax > y1 && ymin < y2) {
		y1 = y1 > ymin ? y1 : ymin;
		y2 = y2 < ymax ? y2 : ymax;
		Graphics_line (g, x, y1, x, y2);
	}
}

void Graphics_quantileQuantilePlot (Graphics g, integer numberOfQuantiles, constVEC x, constVEC y, 
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 plotLabel) {
	const double fontSize = Graphics_inqFontSize (g);

	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	autoVEC xsorted = sort_VEC (x);
	autoVEC ysorted = sort_VEC (y);

	const integer numberOfData = std::min (x.size, y.size);
	numberOfQuantiles = std::min (numberOfData, numberOfQuantiles);
	const double un = pow (0.5, 1.0 / numberOfQuantiles);
	const double u1 = 1.0 - un;
	if (xmin == xmax) {
		xmin = NUMquantile (xsorted.get(), u1);
		xmax = NUMquantile (xsorted.get(), un);
	}
	if (ymin == ymax) {
		ymin = NUMquantile (ysorted.get(), u1);
		ymax = NUMquantile (ysorted.get(), un);
	}
	for (integer i = 1; i <= numberOfQuantiles; i++) {
		const double ui = i == 1 ? u1 : (i == numberOfQuantiles ? un : (i - 0.3175) / (numberOfQuantiles + 0.365));
		const double qx = NUMquantile (xsorted.get(), ui);
		const double qy = NUMquantile (ysorted.get(), ui);
		if (qx < xmin || qx > xmax || qy < ymin || qy > ymax)
			continue; // outside area
		Graphics_text (g, qx, qy, plotLabel);
	}
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, xmin, ymin, xmax, ymax);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setFontSize (g, fontSize);
}

void Graphics_lagPlot (Graphics g, constVEC data, double xmin, double xmax, integer lag, double labelSize, conststring32 plotLabel)
{
	if (lag < 0 || lag >= data.size)
		return;
	const double fontSize = Graphics_inqFontSize (g);
	Graphics_setFontSize (g, labelSize);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	/*
		Plot x [i] vertically and x [i-lag] horizontally
	*/
	for (integer i = 1; i <= data.size - lag; i ++) {
		const double x = data [i + lag], y = data [i];
		if (x >= xmin && x <= xmax && y >= xmin && y <= xmax)
			Graphics_text (g, x, y, plotLabel);
	}
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setFontSize (g, fontSize);
}

/* End of file Graphics_extensions.cpp */
