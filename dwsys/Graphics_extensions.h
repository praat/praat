#ifndef _Graphics_extensions_h_
#define _Graphics_extensions_h_
/* Graphics_extensions.h
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20120914 Latest modification.
*/

#include "Graphics.h"

/*
	Draw a box plot of data[1..ndata]. The vertical center line of the plot
	is at position 'x'. The rectangle box is 2*w wide, the whisker 2*r.
	All drawing outside [ymin, ymax] is clipped.
*/

void Graphics_boxAndWhiskerPlot (Graphics g, double data[], long ndata, double x, double r, double w, double ymin, double ymax);

void Graphics_quantileQuantilePlot (Graphics g, long numberOfQuantiles, double *xdata, long xnumberOfData, double *ydata, long ynumberOfData, double xmin, double xmax, double ymin, double ymax, int labelSize, const wchar_t *plotLabel);

void Graphics_matrixAsSquares (Graphics g, double **matrix, long numberOfRows, long numberOfColumns, double zmin, double zmax, double cellSizeFactor, int randomFillOrder);

#endif /* _Graphics_extensions_h_ */
