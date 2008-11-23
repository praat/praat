/* Matrix_extensions.c
 *
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 20040226 Matrix_drawAsSquares: respect the colour environment (fill with current colour).
 djmw 20041110 Matrix_drawDistribution did't draw lowest bin correctly.
 djmw 20050221 Matrix_drawDistribution would draw outside window.
 djmw 20050405 Matrix_drawDistribution crashed if minimum > data minimum5
 djmw 20080122 float -> double
*/

#include "Matrix_extensions.h"
#include "Eigen.h"
#include "NUM2.h"

void Matrix_scatterPlot (I, Any g, long icx, long icy,
    double xmin, double xmax, double ymin, double ymax,
    double size_mm, const wchar_t *mark, int garnish)
{
    iam (Matrix); 
    long i, ix = labs (icx), iy = labs (icy);
    
    if (ix < 1 || ix > my nx || iy < 1 || iy > my nx) return;
    if (xmax <= xmin)
    {
		(void) Matrix_getWindowExtrema (me, ix, ix, 1, my ny, & xmin, & xmax);
		if (xmax <= xmin)
		{
			xmin -= 0.5; xmax += 0.5;
		}
    }
    if (ymax <= ymin)
    {
		(void) Matrix_getWindowExtrema (me, iy, iy, 1, my ny, & ymin, & ymax);
		if (ymax <= ymin)
		{
			ymin -= 0.5; ymax += 0.5;
		}
    }
    Graphics_setInner (g);
    if (icx < 0) { double t = xmin; xmin = xmax; xmax = t; }
    if (icy < 0) { double t = ymin; ymin = ymax; ymax = t; }
    Graphics_setWindow (g, xmin, xmax, ymin, ymax);
    for (i = 1; i <= my ny; i++)
    {
    	if (my z[i][ix] >= xmin && my z[i][ix] <= xmax &&
    	my z[i][iy] >= ymin && my z[i][iy] <= ymax)
    		Graphics_mark (g, my z[i][ix], my z[i][iy], size_mm, mark);
    }
    Graphics_unsetInner (g);
    if (garnish)
    {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (ymin * ymax < 0.0) Graphics_markLeft (g, 0.0, 1, 1, 1, NULL);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		if (xmin * xmax < 0.0) Graphics_markBottom (g, 0.0, 1, 1, 1, NULL);
	}
}

void Matrix_drawAsSquares (I, Any g, double xmin, double xmax, double ymin, double ymax, int garnish)
{
    iam (Matrix); int colour = Graphics_inqColour (g);
    long i, j, ixmin, ixmax, iymin, iymax, nx, ny;
    double dx, dy, min, max, wAbsMax;

	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	nx = Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax);
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	ny = Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax);
	max = nx > ny ? nx : ny;
	dx = (xmax - xmin) / max; dy = (ymax - ymin) / max;
    Graphics_setInner (g);
    Graphics_setWindow (g, xmin, xmax, ymin, ymax);
    Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & min, & max);
    wAbsMax = fabs (max) > fabs (min) ? fabs (max) : fabs (min);
    for (i = iymin; i <= iymax; i++)
    {
		double y = Matrix_rowToY (me, i);
		for (j = ixmin; j <= ixmax; j++)
		{
			double x = Matrix_columnToX (me, j);
	    	double d = 0.95 * sqrt (fabs (my z[i][j]) / wAbsMax);
	    	double x1WC = x - d * dx / 2, x2WC = x + d * dx / 2;
	    	double y1WC = y - d * dy / 2, y2WC = y + d * dy / 2;
			if (my z[i][j] > 0) Graphics_setColour (g, Graphics_WHITE);
	    	Graphics_fillRectangle (g, x1WC, x2WC, y1WC, y2WC);
	    	Graphics_setColour (g, colour);
	    	Graphics_rectangle (g, x1WC, x2WC , y1WC, y2WC);
		}
    }
    Graphics_setGrey (g, 0.0);
    Graphics_unsetInner (g);
    if (garnish)
    {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (ymin * ymax < 0.0) Graphics_markLeft (g, 0.0, 1, 1, 1, NULL);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		if (xmin * xmax < 0.0) Graphics_markBottom (g, 0.0, 1, 1, 1, NULL);
    }
}

void Matrix_scale (I, int choice)
{
	iam (Matrix); double min, max, extremum;
	long i, j, nZero = 0;
	
	if (choice == 2) /* by row */
	{
		for (i = 1; i <= my ny; i++)
		{
			Matrix_getWindowExtrema (me, 1, my nx, i, i, &min, &max);
			extremum = fabs (max) > fabs (min) ? fabs (max) : fabs (min);
			if (extremum == 0.0) nZero++;
			else for (j=1; j <= my nx; j++) my z[i][j] /= extremum;
		}
	}
	else if (choice == 3) /* by col */
	{
		for (j = 1; j <= my nx; j++)
		{
			Matrix_getWindowExtrema (me, j, j, 1, my ny, &min, &max);
			extremum =  fabs (max) > fabs (min) ? fabs (max) : fabs (min);
			if (extremum == 0.0) nZero++;
			else for (i=1; i <= my ny; i++) my z[i][j] /= extremum;
		}
	}
	else if (choice == 1) /* overall */
	{
		Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, &min, &max);
		extremum =  fabs (max) > fabs (min) ? fabs (max) : fabs (min);
		if (extremum == 0.0) nZero++;
		else
		{
			for (i = 1; i <= my ny; i++)
			{
				for (j = 1; j <= my nx; j++) my z[i][j] /= extremum;
			}
		}
	}
	else
	{
		Melder_flushError ("Matrix_scale: choice must be >= 0 && < 3.");
		return;
	}
	if (nZero)  Melder_warning1 (L"Matrix_scale: extremum == 0, (part of) matrix unscaled.");
}

Any Matrix_transpose (I)
{
	iam (Matrix); 
	long i, j;
	Matrix thee = Matrix_create (my ymin, my ymax, my ny, my dy, my y1,
		my xmin, my xmax, my nx, my dx, my x1);
		
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my ny; i++)
	{
		for (j=1; j <= my nx; j++)
		{ 
			thy z[j][i] = my z[i][j];
		}
	}
	return thee;
	
}

void Matrix_drawDistribution (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double minimum, double maximum, long nBins,
	double freqMin, double freqMax, int cumulative, int garnish)
{
	iam (Matrix); 
	long i, j, ixmin, ixmax, iymin, iymax, nxy = 0, *freq = NULL;
	double binWidth, fi = 0;
	
	if (nBins <= 0) return;
	if (xmax <= xmin)
	{
		xmin = my xmin; xmax = my xmax;
	}
	if (ymax <= ymin)
	{
		ymin = my ymin; ymax = my ymax;
	}
	if (Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax) == 0 ||
		Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax) == 0) 
			return;
	if (maximum <= minimum) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, 
		iymax, & minimum, & maximum);
	if (maximum <= minimum)
	{
		minimum -= 1.0; maximum += 1.0;
	}
	
	/* 
		Count the numbers per bin and the total
	*/
	
	if (nBins < 1) nBins = 10;
	if ((freq = NUMlvector (1, nBins)) == NULL) return;
	binWidth = (maximum - minimum) / nBins;
	for (i = iymin; i <= iymax; i++) 
	{
		for (j = ixmin; j <= ixmax; j++)
		{
			long bin = 1 + floor ((my z[i][j] - minimum) / binWidth);
			if (bin <= nBins && bin > 0)
			{
				freq[bin]++; nxy ++;
			}
		}
	}
		
	if (freqMax <= freqMin)
	{
		if (cumulative)
		{
			freqMin = 0; freqMax = 1.0;
		}
		else
		{
			NUMlvector_extrema (freq, 1, nBins, & freqMin, & freqMax);
			if (freqMax <= freqMin)
			{
				freqMin = freqMin > 1 ? freqMin-1: 0; 
				freqMax += 1.0;
			}
		}
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, minimum, maximum, freqMin, freqMax);
	
	for (i = 1; i <= nBins; i++)
	{
		double ftmp = freq[i];
		fi = cumulative ? fi + freq[i] / nxy : freq[i];
		ftmp = fi;
		if (ftmp > freqMax) ftmp = freqMax;
		if (ftmp > freqMin) Graphics_rectangle (g, minimum + (i-1) * 
			binWidth, minimum + i * binWidth, freqMin, ftmp);
	}	
    Graphics_unsetInner (g);
    if (garnish)
    {
    	Graphics_drawInnerBox (g);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	if (! cumulative) Graphics_textLeft (g, 0, L"number/bin");
    }
	NUMlvector_free (freq, 1);
}

void Matrix_drawSliceY (I, Graphics g, double x, double ymin, double ymax,
	double min, double max)
{
	iam (Matrix);
	long i, ix, iymin, iymax, ny;
	double *y;
	
	if (x < my xmin || x > my xmax) return;
	ix = Matrix_xToNearestColumn (me, x);
	
	if (ymax <= ymin)
	{
		ymin = my ymin;
		ymax = my ymax;
	}
	
	ny = Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax);
	if (ny < 1) return;
	
	if (max <= min)
	{
		Matrix_getWindowExtrema (me, ix, ix, iymin, iymax, &min, &max);
	}
	if (max <= min) 
	{ 
		min -= 0.5; max += 0.5;
	}
	y = NUMdvector (iymin, iymax);
	if (y == NULL) return;
	
	Graphics_setWindow (g, ymin, ymax, min, max);
	Graphics_setInner (g);

	for (i = iymin; i <= iymax; i++)
	{
		y[i] = my z[i][ix];
	}
	Graphics_function (g, y, iymin, iymax, Matrix_rowToY (me, iymin),
		 Matrix_rowToY (me, iymax)); 
	Graphics_unsetInner (g);	
	NUMdvector_free (y, 1);	
}

Matrix Matrix_solveEquation (I, double tolerance)
{
	iam (Matrix); Matrix thee = NULL; int status = 0;
	long i, j, m = my ny, n = my nx - 1; 
	double **u = NULL, *b = NULL, *x = NULL;
	double tol = tolerance ? tolerance : NUMeps * m;

	if (n == 0) return Melder_errorp1 (L"Matrix_solveEquation: there must be at least 2 columns in the matrix.");

	if (m < n) Melder_warning1 (L"Matrix_solveEquation: solution is not unique (fewer equations than unknowns).");

	if (! (u = NUMdmatrix (1, m, 1, n)) ||
		! (b = NUMdvector (1, m)) ||
		! (x = NUMdvector (1, n)) ||
		! (thee = Matrix_create (0.5, 0.5+n, n, 1, 1, 0.5, 1.5, 1, 1, 1))) goto end;

	for (i=1; i <= m; i++)
	{
		for (j=1; j <= n; j++) u[i][j] = my z[i][j];
		b[i] = my z[i][my nx];
	}

	if (! NUMsolveEquation (u, m, n, b, tol, x)) goto end;
	for (j=1; j <= n; j++) thy z[1][j] = x[j];
	status = 1;

end:
	NUMdmatrix_free (u, 1, 1);
	NUMdvector_free (b, 1);
	NUMdvector_free (x, 1);
	if (! status) forget (thee);
	return thee;
}

/* End of file Matrix_extensions.c */
