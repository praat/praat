/* Graphics_surface.cpp
 *
 * Copyright (C) 1992-2005,2008,2011,2016-2019 Paul Boersma
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

#include "Graphics.h"

void Graphics_surface (Graphics me, constMATVU const& z,
	double x1, double x2, double y1, double y2,
	double minimum, double maximum,
	double /* elevation */, double /* azimuth */)
{
	/*
		`sum` is the running sum of the x and y indices of the back corner of each tetragon.
		The x and y indices of the back corner of the backmost tetragon are z.ncol and z.nrow,
		and the x and y indices of the front corner of the frontmost tetragon are 1 and 1,
		so that the x and y indices of its back corner are 1 + 1 and 1 + 1.
	*/
	integer const maxsum = z.ncol + z.nrow, minsum = (1 + 1) + (1 + 1);
	if (z.ncol <= 1 || z.nrow <= 1) return;
	double const dx = (x2 - x1) / (z.ncol - 1);
	double const dy = (y2 - y1) / (z.nrow - 1);

	/*
		We start at the back of the surface plot.
		This part of the picture may be overdrawn by points more forward.
	*/
	for (integer sum = maxsum; sum >= minsum; sum --) {

		/*
			We are going to cycle over a diagonal sequence of points.
			Compute the row boundaries of this sequence.
		*/
		integer iymin = 1 + 1, iymax = z.nrow;
		if (iymin < sum - z.nrow) iymin = sum - z.nrow;
		if (iymax > sum - (1 + 1)) iymax = sum - (1 + 1);
		for (integer iy = iymin; iy <= iymax; iy ++) {

			/*
				Compute the indices of all four points.
			*/
			integer const ix = sum - iy;
			integer const ixback = ix, ixfront = ix - 1, ixleft = ix - 1, ixright = ix;
			integer const iyback = iy, iyfront = iy - 1, iyleft = iy, iyright = iy - 1;

			/*
				Compute the world coordinates of all four points.
			*/
			double const xback = x1 + (ixback - 1) * dx, xright = xback;
			double const xfront = x1 + (ixfront - 1) * dx, xleft = xfront;
			double const yback = y1 + (iyback - 1) * dy, yleft = yback;
			double const yfront = y1 + (iyfront - 1) * dy, yright = yfront;
			double const zback = z [iyback] [ixback], zfront = z [iyfront] [ixfront];
			double const zleft = z [iyleft] [ixleft], zright = z [iyright] [ixright];

			/*
				The Graphics library uses a two-dimensional world, so we have to convert
				to 2-D world coordinates, which we call x [0..3] and y [0..3].
				We suppose that world coordinate `x` = 0 is in the centre of the figure,
				and that the left and right borders of the figure have world coordinates -1.0 and +1.0.
				Also, we suppose that the bottom and top are around `minimum` and `maximum`.
			*/
			double x [5], y [5];

			/*
				BUG: elevation and azimuth are fixed.
			*/
			double const up = 0.3 * (maximum - minimum), xscale = 1.0 / (x2 - x1), yscale = 1.0 / (y2 - y1);

			/*
				The back point.
			*/
			x [0] = (xback - x1) * xscale - (yback - y1) * yscale;
			y [0] = up * ((xback - x1) * xscale + (yback - y1) * yscale) + zback;

			/*
				The right point.
			*/
			x [1] = (xright - x1) * xscale - (yright - y1) * yscale;
			y [1] = up * ((xright - x1) * xscale + (yright - y1) * yscale) + zright;

			/*
				The front point.
			*/
			x [2] = (xfront - x1) * xscale - (yfront - y1) * yscale;
			y [2] = up * ((xfront - x1) * xscale + (yfront - y1) * yscale) + zfront;

			/*
				The left point.
			*/
			x [3] = (xleft - x1) * xscale - (yleft - y1) * yscale;
			y [3] = up * ((xleft - x1) * xscale + (yleft - y1) * yscale) + zleft;

			/*
				Paint the tetragon in the average grey value, white at the top.
				This gives the idea of height.
			*/
			Graphics_setGrey (me, (0.25 * (zback + zright + zfront + zleft) - minimum) / (maximum - minimum));
			Graphics_fillArea (me, 4, & x [0], & y [0]);

			/*
				Draw the borders of the tetragon in black.
				This gives the idea of steepness and viewing angle.
			*/
			Graphics_setGrey (me, 0.0);
			x [4] = x [0];   // close polygon
			y [4] = y [0];   // close polygon
			Graphics_polyline (me, 5, & x [0], & y [0]);
		}
	}
}

/* End of file Graphics_surface.cpp */
