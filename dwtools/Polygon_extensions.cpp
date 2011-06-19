/* Polygon_extensions.c
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20020813 GPL header
 djmw 20080122 float -> double
 djmw 20100407 Sound_to_Polygon, Sounds_to_Polygon_encloseds
 djmw 20100418 Polygon_rotate. bug: angle was in degrees now in radians
*/

#include "Matrix.h"
#include "NUM2.h"
#include "Polygon_extensions.h"
#include "Vector.h"

void Polygon_translate (I, double xt, double yt)
{
	iam (Polygon);

	for (long i = 1; i <= my numberOfPoints; i++)
	{
		my x[i] += xt;
		my y[i] += yt;
	}
}

/* rotate counterclockwise w.r.t. (xc,yc) */
void Polygon_rotate (I, double alpha, double xc, double yc)
{
	iam (Polygon);
	double f = alpha * NUMpi / 180, cosa = cos (f), sina = sin (f);

	Polygon_translate (me, -xc, -yc);
	for (long i = 1; i <= my numberOfPoints; i++)
	{
		double x = my x[i];
		my x[i] = cosa * my x[i] - sina * my y[i];
		my y[i] = sina * x       + cosa * my y[i];
	}
	Polygon_translate (me, xc, yc);
}

void Polygon_scale (I, double xs, double ys)
{
	iam (Polygon);

	for (long i = 1; i <= my numberOfPoints; i++)
	{
		my x[i] *= xs;
		my y[i] *= ys;
	}
}

void Polygon_reverseX (I)
{
	iam (Polygon);

	for (long i = 1; i <= my numberOfPoints; i++)
	{
		my x[i] = -my x[i];
	}

}

void Polygon_reverseY (I)
{
	iam (Polygon);

	for (long i = 1; i <= my numberOfPoints; i++)
	{
		my y[i] = -my y[i];
	}

}

void Polygon_Categories_draw (I, thou, Any graphics, double xmin, double xmax,
	double ymin, double ymax, int garnish)
{
    iam (Polygon); thouart (Categories);
    double min, max, tmp;

    if (my numberOfPoints != thy size) return;

    if (xmax == xmin)
    {
		NUMdvector_extrema (my x, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		xmin = min - tmp; xmax = max + tmp;
    }

    if (ymax == ymin)
    {
		NUMdvector_extrema (my y, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		ymin = min - tmp; ymax = max + tmp;
    }

    Graphics_setInner (graphics);
    Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
    Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);

    for (long i = 1; i <= my numberOfPoints; i++)
	{
		OrderedOfString_drawItem (thee, graphics, i, my x[i], my y[i]);
	}
    Graphics_unsetInner (graphics);
    if (garnish)
    {
		Graphics_drawInnerBox (graphics);
		Graphics_marksLeft (graphics, 2, 1, 1, 0);
		if (ymin * ymax < 0.0)
		{
			Graphics_markLeft (graphics, 0.0, 1, 1, 1, NULL);
		}
		Graphics_marksBottom (graphics, 2, 1, 1, 0);
		if (xmin * xmax < 0.0)
		{
			Graphics_markBottom (graphics, 0.0, 1, 1, 1, NULL);
		}
	}
}

static void setWindow (Polygon me, Any graphics,
	double xmin, double xmax, double ymin, double ymax)
{
	Melder_assert (me);

	if (xmax <= xmin)   /* Autoscaling along x axis. */
	{
		xmax = xmin = my x [1];
		for (long i = 2; i <= my numberOfPoints; i ++)
		{
			if (my x [i] < xmin) xmin = my x [i];
			if (my x [i] > xmax) xmax = my x [i];
		}
		if (xmin == xmax)
		{
			xmin -= 1.0;
			xmax += 1.0;
		}
	}
	if (ymax <= ymin)   /* Autoscaling along y axis. */
	{
		ymax = ymin = my y [1];
		for (long i = 2; i <= my numberOfPoints; i ++)
		{
			if (my y [i] < ymin) ymin = my y [i];
			if (my y [i] > ymax) ymax = my y [i];
		}
		if (ymin == ymax)
		{
			ymin -= 1.0;
			ymax += 1.0;
		}
	}
	Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
}

void Polygon_drawMarks (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark)
{
	iam (Polygon);

	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= my numberOfPoints; i++)
	{
		Graphics_mark (g, my x[i], my y[i], size_mm, mark);
	}
	Graphics_unsetInner (g);
}

#define CLIP_Y(y,ymin,ymax) (clip ? ((y) > (ymax) ? (ymax) : (y) < (ymin) ? (ymin) : (y)) : y)

Polygon Sound_to_Polygon (Sound me, int channel, double tmin, double tmax, double ymin, double ymax, double level)
{
	try {
		bool clip = ymin < ymax;
		if (channel < 1 || channel > my ny) Melder_throw ("Channel does not exist.");
		if (tmin >= tmax)
		{
			tmin = my xmin;
			tmax = my xmax;
		}
		if (tmin < my xmin) tmin = my xmin;
		if (tmax > my xmax) tmax = my xmax;
		if (tmin >= my xmax || tmax < my xmin) Melder_throw ("Invalid domain.");
		long k = 1, i1 = Sampled_xToHighIndex (me, tmin);
		long i2 = Sampled_xToLowIndex (me, tmax);
		long numberOfPoints = i2 - i1 + 1 + 2 + 2 + 1; // begin + endpoint + level + connection
		autoPolygon him = Polygon_create (numberOfPoints);

		/*
			In Vector_getValueAtX the interpolation only returns defined values between the
			left and right edges that are calculated as
				left = x1 - 0.5 * dx; right = left + my nx * dx.
			Given a sound, for example on the domain [0,...], the value of 'left' with the above formula might
			not return exactly xmin but instead a very small deviation (due to the imprecise
			representation of real numbers in a computer).
			Querying for the value at xmin which is outside the interpolation domain then produces an 'undefined'.
			We try to avoid this with the following workaround.
		*/
		double xmin = my x1 - 0.5 * my dx;
		double xmax = xmin + my nx * my dx;
		tmin = tmin < xmin ? xmin : tmin;
		tmax = tmax > xmax ? xmax : tmax;
		// End of workaround
		his x[k] = tmin;
		his y[k++] = CLIP_Y(level,ymin,ymax);
		his x[k] = tmin;
		double y = Vector_getValueAtX (me, tmin, channel, Vector_VALUE_INTERPOLATION_LINEAR);
		his y[k++] = CLIP_Y(y,ymin,ymax);
		for (long i = i1; i <= i2; i++)
		{
			y = my z[channel][i];
			his x[k] = my x1 + (i - 1) * my dx;
			his y[k++] = CLIP_Y(y,ymin,ymax);
		}
		his x[k] = tmax;
		y = Vector_getValueAtX (me, tmax, channel, Vector_VALUE_INTERPOLATION_LINEAR);
		his y[k++] = CLIP_Y(y,ymin,ymax);
		his x[k] = tmax;
		his y[k++] = CLIP_Y(level,ymin,ymax);
		his x[k] = his x[1]; // Close the path
		his y[k++] = his y[1];
		return him.transfer();
	} catch (MelderError) { rethrowmzero (me, ":no Polygon created."); }
}

/* Area inbetween */

Polygon Sounds_to_Polygon_enclosed (Sound me, Sound thee, int channel, double tmin, double tmax, double ymin, double ymax)
{
	try {
		bool clip = ymin < ymax;
		if (my ny > 1 && thy ny > 1 && my ny != thy ny) Melder_throw ("The numbers of channels of the two sounds have to be equal or 1.");

		long numberOfChannels = my ny > thy ny ? my ny : thy ny;

		if (channel < 1 || channel > numberOfChannels) Melder_throw ("Channel does not exist.");
		// find overlap in the domains  with xmin workaround as in Sound_to_Polygon
		double xmin1 = my x1 - 0.5 * my dx, xmin2 = thy x1 - 0.5 * thy dx ;
		double xmin = my xmin > thy xmin ? xmin1 : xmin2;
		double xmax = my xmax < thy xmax ? xmin1 + my nx * my dx : xmin2 + thy nx * thy dx;
		if (xmax <= xmin) Melder_throw ("Domains must overlap.");
		if (tmin >= tmax)
		{
			tmin = xmin;
			tmax = xmax;
		}
		if (tmin < xmin) tmin = xmin;
		if (tmax > xmax) tmax = xmax;
		if (tmin >= xmax || tmax < xmin) Melder_throw ("Invalid domain.");

		long k = 1;
		long ib1 = Sampled_xToHighIndex (me, tmin);
		long ie1 = Sampled_xToLowIndex (me, tmax);
		long n1 = ie1 - ib1 + 1;
		long ib2 = Sampled_xToHighIndex (thee, tmin);
		long ie2 = Sampled_xToLowIndex (thee, tmax);
		long n2 = ie2 - ib2 + 1;
		long numberOfPoints = n1 + n2 + 5; // me + thee + begin + endpoint + closing

		autoPolygon him = Polygon_create (numberOfPoints);
		
		// my starting point at tmin
		
		double y = Vector_getValueAtX (me, tmin, (my ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmin;
		his y[k++] = CLIP_Y(y,ymin,ymax);
		
		// my samples
		
		for (long i = ib1; i <= ie1; i++)
		{
			double t = my x1 + (i - 1) * my dx;
			y = my z[my ny == 1 ? 1 : channel][i];
			his x[k] = t;
			his y[k++] = CLIP_Y(y,ymin,ymax);
		}
		
		// my end point at tmax
		
		y = Vector_getValueAtX (me, tmax, (my ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmax;
		his y[k++] = y;
		
		// thy starting point at tmax
		
		y = Vector_getValueAtX (thee, tmax, (thy ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmax;
		his y[k++] = y;
		
		// thy samples
		
		for (long i = ie2; i >= ib2; i--)
		{
			double t = thy x1 + (i - 1) * thy dx;
			y = thy z[thy ny == 1 ? 1 : channel][i];
			his x[k] = t;
			his y[k++] = CLIP_Y(y,ymin,ymax);
		}
		
		// thy end point at tmin
		
		y = Vector_getValueAtX (thee, tmin, (thy ny == 1 ? 1 : channel), Vector_VALUE_INTERPOLATION_LINEAR);
		his x[k] = tmin;
		his y[k++] = y;
		
		// connect
		
		his x[k] = his x[1];
		his y[k] = his y[1];
		Melder_assert (k == numberOfPoints);
		return him.transfer();
	} catch (MelderError) { rethrowmzero (me, ": no encloded Polygon created."); }
}

#define AREA2(x1,y1,x2,y2,x3,y3) ((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))

/* End of file Polygon_extensions.cpp */
