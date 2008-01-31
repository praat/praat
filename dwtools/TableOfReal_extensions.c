/* TableOfReal_extensions.c
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
 djmw 20000202 17 typos in F1-3,L1-3 table corrected 
 djmw 20030707 Changed TableOfReal_drawVectors interface.
 djmw 20031030 Added TableOfReal_appendColumns
 djmw 20031216 Interface change in TableOfReal_choleskyDecomposition
 djmw 20040108 Corrected a memory leak in TableOfReal_drawBoxPlots
 djmw 20040211 Modified TableOfReal_copyLabels behaviour: copy NULL-labels too.
 djmw 20040511 Removed TableOfReal_extractRowsByRowLabel,TableOfReal_selectColumnsWhereRow
 djmw 20040617 Removed column selection bug in TableOfReal_drawRowsAsHistogram
 djmw 20041105 Added TableOfReal_createFromVanNieropData_25females
 djmw 20041115 TableOfReal_drawScatterPlot: plotting a NULL-label crashed Praat.
 djmw 20041213 Added TableOfReal_createFromWeeninkData.
 djmw 20050221 TableOfReal_meansByRowLabels, extra reduce parameter.
 djmw 20050222 TableOfReal_drawVectors didn't draw rowlabels.
 djmw 20050512 TableOfReal TableOfReal_meansByRowLabels crashed if first label in sorted was NULL.
 djmw 20051116 TableOfReal_drawScatterPlot draw reverse permited by choosing xmin > xmax and/or ymin>ymax
 djmw 20060301 TableOfReal_meansByRowLabels extra medianize
 djmw 20060626 Extra NULL argument for ExecRE.
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20070822 wchar_t
 djmw 20070902 Better error messages (object type and name feedback)
 djmw 20070614 updated to version 1.30 of regular expressions.
 djmw 20071202 Melder_warning<n>
 djmw 20080122 float -> double
*/

#include <ctype.h>
#include "NUMclapack.h"
#include "NUM2.h"
#include "SVD.h"
#include "TableOfReal_extensions.h"
#include "TableOfReal_and_Permutation.h"
#include "regularExp.h"
#include "Formula.h"
#include "Table_extensions.h"

#define EMPTY_STRING(s) ((s) == NULL || s[0] == '\0')
#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

#define Graphics_ARROW 1
#define Graphics_TWOWAYARROW 2
#define Graphics_LINE 3


int TableOfReal_copyOneRowWithLabel (I, thou, long myrow, long thyrow)
{
	iam (TableOfReal); thouart (TableOfReal);

	if ( myrow < 1 ||  myrow > my  numberOfRows ||
		thyrow < 1 || thyrow > thy numberOfRows ||
		my numberOfColumns != thy numberOfColumns) return 0;
	
	if (my rowLabels[myrow] != NULL && thy rowLabels[thyrow] != my rowLabels[myrow])
	{
		Melder_free (thy rowLabels[thyrow]);
		thy rowLabels[thyrow] = Melder_wcsdup (my rowLabels[myrow]);
		if (thy rowLabels[thyrow] == NULL) return 0;
	}
	if (my data[myrow] != thy data[thyrow]) NUMdvector_copyElements (my data[myrow], thy data[thyrow], 1, my numberOfColumns);
	return 1;
}

int TableOfReal_hasRowLabels (I)
{
	iam (TableOfReal);
	long i;
	
	if (my rowLabels == NULL) return 0;
	for (i = 1; i <= my numberOfRows; i++)
	{
		if (EMPTY_STRING(my rowLabels[i])) return 0;
	}
	return 1;
}

int TableOfReal_hasColumnLabels (I)
{
	iam (TableOfReal);
	long i;
	
	if (my columnLabels == NULL) return 0;
	for (i = 1; i <= my numberOfColumns; i++)
	{
		if (EMPTY_STRING (my columnLabels[i])) return 0;
	}
	return 1;
}

TableOfReal TableOfReal_createIrisDataset (void)
{
	TableOfReal me = NULL;
	long i, j; 
	float iris[150][4] = {
	5.1,3.5,1.4,0.2,4.9,3.0,1.4,0.2,4.7,3.2,1.3,0.2,4.6,3.1,1.5,0.2,5.0,3.6,1.4,0.2,
	5.4,3.9,1.7,0.4,4.6,3.4,1.4,0.3,5.0,3.4,1.5,0.2,4.4,2.9,1.4,0.2,4.9,3.1,1.5,0.1,
	5.4,3.7,1.5,0.2,4.8,3.4,1.6,0.2,4.8,3.0,1.4,0.1,4.3,3.0,1.1,0.1,5.8,4.0,1.2,0.2,
	5.7,4.4,1.5,0.4,5.4,3.9,1.3,0.4,5.1,3.5,1.4,0.3,5.7,3.8,1.7,0.3,5.1,3.8,1.5,0.3,
	5.4,3.4,1.7,0.2,5.1,3.7,1.5,0.4,4.6,3.6,1.0,0.2,5.1,3.3,1.7,0.5,4.8,3.4,1.9,0.2,
	5.0,3.0,1.6,0.2,5.0,3.4,1.6,0.4,5.2,3.5,1.5,0.2,5.2,3.4,1.4,0.2,4.7,3.2,1.6,0.2,
	4.8,3.1,1.6,0.2,5.4,3.4,1.5,0.4,5.2,4.1,1.5,0.1,5.5,4.2,1.4,0.2,4.9,3.1,1.5,0.2,
	5.0,3.2,1.2,0.2,5.5,3.5,1.3,0.2,4.9,3.6,1.4,0.1,4.4,3.0,1.3,0.2,5.1,3.4,1.5,0.2,
	5.0,3.5,1.3,0.3,4.5,2.3,1.3,0.3,4.4,3.2,1.3,0.2,5.0,3.5,1.6,0.6,5.1,3.8,1.9,0.4,
	4.8,3.0,1.4,0.3,5.1,3.8,1.6,0.2,4.6,3.2,1.4,0.2,5.3,3.7,1.5,0.2,5.0,3.3,1.4,0.2,
	7.0,3.2,4.7,1.4,6.4,3.2,4.5,1.5,6.9,3.1,4.9,1.5,5.5,2.3,4.0,1.3,6.5,2.8,4.6,1.5,
	5.7,2.8,4.5,1.3,6.3,3.3,4.7,1.6,4.9,2.4,3.3,1.0,6.6,2.9,4.6,1.3,5.2,2.7,3.9,1.4,
	5.0,2.0,3.5,1.0,5.9,3.0,4.2,1.5,6.0,2.2,4.0,1.0,6.1,2.9,4.7,1.4,5.6,2.9,3.6,1.3,
	6.7,3.1,4.4,1.4,5.6,3.0,4.5,1.5,5.8,2.7,4.1,1.0,6.2,2.2,4.5,1.5,5.6,2.5,3.9,1.1,
	5.9,3.2,4.8,1.8,6.1,2.8,4.0,1.3,6.3,2.5,4.9,1.5,6.1,2.8,4.7,1.2,6.4,2.9,4.3,1.3,
	6.6,3.0,4.4,1.4,6.8,2.8,4.8,1.4,6.7,3.0,5.0,1.7,6.0,2.9,4.5,1.5,5.7,2.6,3.5,1.0,
	5.5,2.4,3.8,1.1,5.5,2.4,3.7,1.0,5.8,2.7,3.9,1.2,6.0,2.7,5.1,1.6,5.4,3.0,4.5,1.5,
	6.0,3.4,4.5,1.6,6.7,3.1,4.7,1.5,6.3,2.3,4.4,1.3,5.6,3.0,4.1,1.3,5.5,2.5,4.0,1.3,
	5.5,2.6,4.4,1.2,6.1,3.0,4.6,1.4,5.8,2.6,4.0,1.2,5.0,2.3,3.3,1.0,5.6,2.7,4.2,1.3,
	5.7,3.0,4.2,1.2,5.7,2.9,4.2,1.3,6.2,2.9,4.3,1.3,5.1,2.5,3.0,1.1,5.7,2.8,4.1,1.3,
	6.3,3.3,6.0,2.5,5.8,2.7,5.1,1.9,7.1,3.0,5.9,2.1,6.3,2.9,5.6,1.8,6.5,3.0,5.8,2.2,
	7.6,3.0,6.6,2.1,4.9,2.5,4.5,1.7,7.3,2.9,6.3,1.8,6.7,2.5,5.8,1.8,7.2,3.6,6.1,2.5,
	6.5,3.2,5.1,2.0,6.4,2.7,5.3,1.9,6.8,3.0,5.5,2.1,5.7,2.5,5.0,2.0,5.8,2.8,5.1,2.4,
	6.4,3.2,5.3,2.3,6.5,3.0,5.5,1.8,7.7,3.8,6.7,2.2,7.7,2.6,6.9,2.3,6.0,2.2,5.0,1.5,
	6.9,3.2,5.7,2.3,5.6,2.8,4.9,2.0,7.7,2.8,6.7,2.0,6.3,2.7,4.9,1.8,6.7,3.3,5.7,2.1,
	7.2,3.2,6.0,1.8,6.2,2.8,4.8,1.8,6.1,3.0,4.9,1.8,6.4,2.8,5.6,2.1,7.2,3.0,5.8,1.6,
	7.4,2.8,6.1,1.9,7.9,3.8,6.4,2.0,6.4,2.8,5.6,2.2,6.3,2.8,5.1,1.5,6.1,2.6,5.6,1.4,
	7.7,3.0,6.1,2.3,6.3,3.4,5.6,2.4,6.4,3.1,5.5,1.8,6.0,3.0,4.8,1.8,6.9,3.1,5.4,2.1,
	6.7,3.1,5.6,2.4,6.9,3.1,5.1,2.3,5.8,2.7,5.1,1.9,6.8,3.2,5.9,2.3,6.7,3.3,5.7,2.5,
	6.7,3.0,5.2,2.3,6.3,2.5,5.0,1.9,6.5,3.0,5.2,2.0,6.2,3.4,5.4,2.3,5.9,3.0,5.1,1.8	
	};

	if (! (me = TableOfReal_create (150, 4))) return NULL;
	
	TableOfReal_setColumnLabel (me, 1, L"sl");
	TableOfReal_setColumnLabel (me, 2, L"sw");
	TableOfReal_setColumnLabel (me, 3, L"pl");
	TableOfReal_setColumnLabel (me, 4, L"pw");
	for (i=1; i <= 150; i++)
	{
		int kind = (i - 1) / 50 + 1;
		wchar_t *label = kind == 1 ? L"1" : kind == 2 ? L"2" : L"3";
		for (j=1; j <= 4; j++) my data[i][j] = iris[i-1][j-1];
		TableOfReal_setRowLabel (me, i, label);
	}
	Thing_setName (me, L"iris");
	return me;
}

Strings TableOfReal_extractRowLabels (I)
{
	iam (TableOfReal);
	Strings thee = new (Strings);
	long i, n = my numberOfRows;
	
	if (thee == NULL) return NULL;
	if (n < 1) return thee;
	
	thy strings = NUMpvector (1, n);
	if (thy strings == NULL) goto end;
	thy numberOfStrings = n;
	
	for (i = 1; i <= n; i++)
	{
		wchar_t *label = my rowLabels[i] ? my rowLabels[i] : L"?";
		thy strings[i] = Melder_wcsdup (label);
		if (thy strings[i] == NULL) goto end; 
	}

end:

	if (Melder_hasError()) forget (thee);
	return thee;	
}


Strings TableOfReal_extractColumnLabels (I)
{
	iam (TableOfReal);
	Strings thee = new (Strings);
	long i, n = my numberOfColumns;
		
	if (thee == NULL) return NULL;
	if (n < 1) return thee;
	
	thy strings = NUMpvector (1, n);
	if (thy strings == NULL) goto end;
	thy numberOfStrings = n;
	
	for (i = 1; i <= n; i++)
	{
		wchar_t *label = my columnLabels[i] ? my columnLabels[i] : L"?";
		thy strings[i] = Melder_wcsdup (label);
		if (thy strings[i] == NULL) goto end; 
	}

end:

	if (Melder_hasError()) forget (thee);
	return thee;	
}

TableOfReal TableOfReal_transpose (I)
{
	iam (TableOfReal);
	TableOfReal thee;
	long i, j;
	
	thee = TableOfReal_create (my numberOfColumns, my numberOfRows);
	if (thee == NULL) return NULL;

	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			thy data[j][i] = my data[i][j];
		}
	}
		
	if (! NUMstrings_copyElements (my rowLabels, thy columnLabels, 
		1, my numberOfRows) ||
		! NUMstrings_copyElements (my columnLabels, thy rowLabels, 
		1, my numberOfColumns)) forget (thee);
	return thee;
}

int TableOfReal_to_Pattern_and_Categories (I, long fromrow, long torow, long fromcol, long tocol,
	Pattern *p, Categories *c)
{
	iam (TableOfReal);
	long i, j, ncol = my numberOfColumns, nrow = my numberOfRows, row, col;
	int status = 1;

	if (fromrow == torow && fromrow == 0)
	{
		fromrow = 1; torow = nrow;
	}
	else if (fromrow > 0 && fromrow <= nrow && torow == 0)
	{
		torow = nrow;
	}
	else if (! (fromrow > 0 && torow <= nrow && fromrow <= torow))
	{
		return Melder_error2 (L"Illegal row selection for ", Thing_messageName(me));
	}
	if (fromcol == tocol && fromcol == 0)
	{
		fromcol = 1; tocol = ncol;
	}
	else if (fromcol > 0 && fromcol <= ncol && tocol == 0)
	{
		tocol = ncol;
	}
	else if (! (fromcol > 0 && tocol <= ncol && fromcol <= tocol))
	{
		return Melder_error2 (L"Illegal col selection for ", Thing_messageName(me));
	}
	nrow = torow - fromrow + 1;
	ncol = tocol - fromcol + 1;
	
	*c = NULL;
	if (! (*p = Pattern_create (nrow, ncol)) ||
		! (*c = Categories_create ())) goto end;
	
	for (row=1, i=fromrow; i <= torow; i++, row++)
	{
		wchar_t *s = my rowLabels[i] ? my rowLabels[i] : L"?";
		SimpleString item = SimpleString_create (s);
		if (! item || ! Collection_addItem (*c, item)) goto end;
		for (col=1, j=fromcol; j <= tocol; j++, col++)
		{
			(*p)->z[row][col] = my data[i][j];
		}
	}
	
end:

	if (Melder_hasError ())
	{
		forget (*p);
		forget (*c);
		status = 0;
	}
	return status;
}

void TableOfReal_getColumnExtrema (I, long col, double *min, double *max)
{
	iam (TableOfReal); long i;
	if (col < 1 || col > my numberOfColumns)
	{
		(void) Melder_error2 (L"Not a valid column for ", Thing_messageName(me));
		*min = NUMundefined; *max = NUMundefined; return;
	}
	*min = *max = my data[1][col];
	for (i=2; i <= my numberOfRows; i++)
	{
		if (my data[i][col] > *max) *max = my data[i][col];
		else if (my data[i][col] < *min) *min = my data[i][col];
	}
}

void TableOfReal_drawRowsAsHistogram (I, Graphics g, wchar_t *rows, long colb, long cole,
	double ymin, double ymax, double xoffsetFraction, double interbarFraction,
	double interbarsFraction, wchar_t *greys, int garnish)	
{
	iam (TableOfReal);
	long i, j, irow, nrows, ncols, ngreys;
	double *irows, *igreys, grey, bar_width = 1, xb, dx, x1, x2, y1, y2;
	
	if (colb >= cole)
	{
		colb = 1; cole = my numberOfColumns;
	}
	if (colb <= cole && (colb < 1 || cole > my numberOfColumns))
	{
		    Melder_warning1 (L"Invalid columns");
			return;
	}
	
	irows = NUMstring_to_numbers (rows, &nrows);
	if (irows == NULL)
	{
		 Melder_warning1 (L"No rows!");
		 return;
	}
	
	for (i = 1; i <= nrows; i++)
	{
		irow = irows[i];
		if (irow < 0 || irow > my numberOfRows)
		{
			 Melder_warning3 (L"Invalid row (", Melder_integer (irow), L").");
			 goto end;
		}
		if (ymin >= ymax)
		{
			double min, max;
			NUMdvector_extrema (my data[irow], colb, cole, &min, &max);
			if (i > 1)
			{
				if (min < ymin) ymin = min;
				if (max > ymax) ymax = max;
			}
			else
			{
				ymin = min; ymax = max;
			}
		}
	}
	
	igreys = NUMstring_to_numbers (greys, &ngreys);
	if (igreys == NULL)
	{
		 Melder_warning1 (L"No greys!");
		 return;
	}
 
    Graphics_setWindow (g, 0, 1, ymin, ymax);
    Graphics_setInner (g);
	
	ncols = cole - colb + 1;	
	bar_width /= ncols * nrows + 2 * xoffsetFraction + (ncols - 1) * interbarsFraction + 
		ncols * (nrows -1) * interbarFraction;
    dx = (interbarsFraction + nrows + (nrows -1) * interbarFraction) * bar_width;

	for (i = 1; i <= nrows; i++)
	{
		irow = irows[i];
		xb = xoffsetFraction * bar_width + (i - 1) * (1 + interbarFraction) * bar_width;  
	
		x1 = xb;
		grey = i <= ngreys ? igreys[i] : igreys[ngreys];
    	for (j = colb; j <= cole; j++)
    	{
			x2 = x1 + bar_width;
			y1 = ymin; y2 = my data[irow][j];
			if (y2 > ymin)
			{
				if (y2 > ymax) y2 = ymax;
				Graphics_setGrey (g, grey);
				Graphics_fillRectangle (g, x1, x2, y1, y2);
				Graphics_setGrey (g, 0); /* Black */
				Graphics_rectangle (g, x1, x2, y1, y2);
			}
			x1 += dx;
    	}
	}
	
    Graphics_unsetInner (g);
	
	if (garnish)
	{
		xb = (xoffsetFraction + 0.5 * (nrows + (nrows - 1) * interbarFraction)) * bar_width;
		for (j = colb; j <= cole; j++)
		{
			if (my columnLabels[j]) Graphics_markBottom (g, xb, 0, 0, 0, my columnLabels[j]);
			xb += dx;
		}
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}

end:
	
	NUMdvector_free (irows, 1);
}

void TableOfReal_drawBiplot (I, Graphics g, double xmin, double xmax, 
	double ymin, double ymax, double sv_splitfactor, int labelsize, int garnish)
{
	iam (TableOfReal);
	SVD svd;
	double lambda1, lambda2;
	double *x = NULL, *y = NULL;
	long i, numberOfZeroed;
	long nr = my numberOfRows, nc = my numberOfColumns, nmin;
	long nPoints = nr + nc; 
	int fontsize = Graphics_inqFontSize (g);

	svd = SVD_create (nr, nc);
	if (svd == NULL) goto end;
	
	NUMdmatrix_copyElements (my data, svd -> u, 1, nr, 1, nc);
	NUMcentreColumns (svd -> u, 1, nr, 1, nc, NULL);
	
	if (! SVD_compute (svd)) goto end; 	
	numberOfZeroed = SVD_zeroSmallSingularValues (svd, 0);
	
	nmin = MIN (nr, nc) - numberOfZeroed;
	if (nmin < 2)
	{
		Melder_warning1 (L" There must be at least two (independent) columns in the table.");
		goto end;
	}

	x = NUMdvector (1, nPoints);
	if (x == NULL) goto end;
	y = NUMdvector (1, nPoints);
	if (y == NULL) goto end;

	lambda1 = pow (svd -> d[1], sv_splitfactor);
	lambda2 = pow (svd -> d[2], sv_splitfactor);
	for (i = 1; i <= nr; i++)
	{
		x[i] = svd -> u[i][1] * lambda1;
		y[i] = svd -> u[i][2] * lambda2;
	}
	lambda1 = svd -> d[1] / lambda1;
	lambda2 = svd -> d[2] / lambda2;
	for (i = 1; i <= nc; i++)
	{
			x[nr + i] = svd -> v[i][1] * lambda1;
			y[nr + i] = svd -> v[i][2] * lambda2;
	}
		
	if (xmax <= xmin) NUMdvector_extrema (x, 1, nPoints, &xmin, &xmax);
	if (xmax <= xmin) { xmax += 1; xmin -= 1; }
	if (ymax <= ymin) NUMdvector_extrema (y, 1, nPoints, &ymin, &ymax);
	if (ymax <= ymin) { ymax += 1; ymin -= 1; }
	
    Graphics_setWindow (g, xmin, xmax, ymin, ymax);	
    Graphics_setInner (g);
	if (labelsize > 0) Graphics_setFontSize (g, labelsize);			
    Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	
	for (i = 1; i <= nPoints; i++)
	{
		wchar_t *label;
		if (i <= nr)
		{
			label = my rowLabels[i];
			if (label == NULL) label = L"?__r_";
		}
		else
		{
			label = my columnLabels[i - nr];
			if (label == NULL) label = L"?__c_";
		}
		Graphics_text (g, x[i], y[i], label);
	}
	
    Graphics_unsetInner (g);
	
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
	
	if (labelsize > 0) Graphics_setFontSize (g, fontsize);
				
end:
	forget (svd);
	NUMdvector_free (x, 1);
	NUMdvector_free (y, 1);
}

/*
	Draw a box plot of data[1..ndata]. The vertical center line of the plot
	is at position 'x'. The rectangle box is 2*w wide, the whisker 2*r.
	All drawing outside [ymin, ymax] is clipped. 
*/
static void Graphics_drawBoxPlot (Graphics g, double data[], long ndata, 
	double x, double r, double w, double ymin, double ymax)
{
	double lowerOuterFence, lowerInnerFence, mean;
	double q75, q25, q50, upperInnerFence, upperOuterFence, hspread;
	double lowerWhisker, upperWhisker, y1, y2;
	int lineType = Graphics_inqLineType (g);
	long i, ie;

	Melder_assert (r > 0 && w > 0);
	if (ndata < 3) return;
	
	/*
		Sort the data (increasing: data[1] <= ... <= data[ndata]).
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

	if (ymax <= ymin)
	{
		ymin = data[1]; ymax = data[ndata];
	}		
	if (data[1] > ymax || data[ndata] < ymin) return;

	for (mean=0, i=1; i <= ndata; i++) mean += data[i];
	mean /= ndata;
			
	q25 = NUMquantile (ndata, data, 0.25);
	q50 = NUMquantile (ndata, data, 0.5);
	q75 = NUMquantile (ndata, data, 0.75);
		
	hspread = fabs (q75 - q25);
	lowerOuterFence = q25 - 3.0 * hspread;
	lowerInnerFence = q25 - 1.5 * hspread;
	upperInnerFence = q75 + 1.5 * hspread;
	upperOuterFence = q75 + 3.0 * hspread;
		
	/*
		Decide whether there are outliers that have to be drawn.
		First process data from below (data are sorted).
	*/
	
	i = 1; ie = ndata;
	while (i <= ie && data[i] < ymin) i++;			
    Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	while (i <= ie && data[i] < lowerOuterFence)
	{
		Graphics_text (g, x, data[i], L"o"); i++;
	}
	while (i <= ie && data[i] < lowerInnerFence)
	{
		Graphics_text (g, x, data[i], L"*"); i++;
	}
	lowerWhisker = data[i] < q25 ? data[i] : lowerInnerFence;
	if (lowerWhisker > ymax) return;
		
	/*
		Next process data from above.
	*/
		
	i = ndata; ie = i;				
	while (i >= ie && data[i] > ymax) i--;
	while (i >= ie && data[i] > upperOuterFence)
	{
		Graphics_text (g, x, data[i], L"o"); i--;
	}
	while (i >= ie && data[i] > upperInnerFence)
	{
		Graphics_text (g, x, data[i], L"*"); i--;
	}
	upperWhisker = data[i] > q75 ? data[i] : upperInnerFence;
	if (upperWhisker < ymin) return;
		
	/*
		Determine what parts of the "box" have to be drawn within the 
		range [ymin, ymax].
		Horizontal lines first.
	*/
		
	y1 = lowerWhisker;
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
	if (ymax > y1 && ymin < y1)
	{
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, x - w, y1, x + w, y1);
		Graphics_setLineType (g, lineType); 
	}
		
	/*
		Now process the vertical lines.
	*/

	y1 = lowerWhisker; y2 = q25; 	
	if (ymax > y1 && ymin < y2)
	{
		y1 = MAX (y1, ymin);
		y2 = MIN (y2, ymax);
		Graphics_line (g, x, y1, x, y2);
	}
	y1 = q25; y2 = q75; 	
	if (ymax > y1 && ymin < y2)
	{
		y1 = MAX (y1, ymin);
		y2 = MIN (y2, ymax);
		Graphics_line (g, x - w, y1, x - w, y2);
		Graphics_line (g, x + w, y1, x + w, y2);
	}
	y1 = q75; y2 = upperWhisker; 	
	if (ymax > y1 && ymin < y2)
	{
		y1 = MAX (y1, ymin);
		y2 = MIN (y2, ymax);
		Graphics_line (g, x, y1, x, y2);
	}
}

void TableOfReal_drawBoxPlots (I, Graphics g, long rowmin, long rowmax, 
	long colmin, long colmax, double ymin, double ymax, int garnish)
{
	iam (TableOfReal); 
	double *data = NULL; 
	long i, j, numberOfRows;

	if (rowmax < rowmin || rowmax < 1)
	{
		rowmin = 1;
		rowmax = my numberOfRows;
	}
	if (rowmin < 1) rowmin = 1;
	if (rowmax > my numberOfRows) rowmax = my numberOfRows;
	numberOfRows = rowmax - rowmin + 1;
	if (colmax < colmin || colmax < 1)
	{
		colmin = 1;
		colmax = my numberOfColumns;
	}
	if (colmin < 1) colmin = 1; 
	if (colmax > my numberOfColumns) colmax = my numberOfColumns;
	if (ymax <= ymin) NUMdmatrix_extrema (my data, rowmin, rowmax, 
		colmin, colmax, &ymin, &ymax);
	if ((data = NUMdvector (1, numberOfRows)) == NULL) return;
	
    Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, ymin, ymax);	
    Graphics_setInner (g);
	
	for (j = colmin; j <= colmax; j++)
	{
		double x = j, r = 0.05, w = 0.2, t; 
		long ndata = 0;
		
		for (i = 1; i <= numberOfRows; i++)
		{
			if ((t = my data[rowmin+i-1][j]) != NUMundefined) data[++ndata] = t;
		}
		Graphics_drawBoxPlot (g, data, ndata, x, r, w, ymin, ymax);
	}
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		for (j = colmin; j <= colmax; j++)
		{
			if (my columnLabels && my columnLabels[j] && my columnLabels[j][0])
				Graphics_markBottom (g, j, 0, 1, 0, my columnLabels [j]);
		}
    	Graphics_marksLeft (g, 2, 1, 1, 0);
	}
	NUMdvector_free (data, 1);
}

int TableOfReal_equalLabels (I, thou, int rowLabels, int columnLabels)
{
	iam (TableOfReal); thouart (TableOfReal); long i;
	Melder_assert (rowLabels || columnLabels);
	if (rowLabels)
	{
		if (my numberOfRows != thy numberOfRows) return 0;
		if (my rowLabels == thy rowLabels) return 1;
		for (i=1; i <= my numberOfRows; i++)
		{
			if (NUMwcscmp (my rowLabels[i], thy rowLabels[i])) return 0;
		}
	}
	if (columnLabels)
	{
		if (my numberOfColumns != thy numberOfColumns) return 0;
		if (my columnLabels == thy columnLabels) return 1;
		for (i=1; i <= my numberOfColumns; i++) 
		{
			if (NUMwcscmp (my columnLabels[i], thy columnLabels[i])) return 0;
		}
	}
	return 1;
}

int TableOfReal_copyLabels (I, thou, int rowOrigin, int columnOrigin)
{
	iam (TableOfReal);
	thouart (TableOfReal);
	
	if (rowOrigin == 1)
	{
		if (my numberOfRows != thy numberOfRows ||
			! NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, thy numberOfRows)) return 0;
	}
	else if (rowOrigin == -1)
	{
		if (my numberOfColumns != thy numberOfRows ||
			! NUMstrings_copyElements (my columnLabels, thy rowLabels, 1, thy numberOfRows)) return 0;
	}
	if (columnOrigin == 1)
	{
		if (my numberOfColumns != thy numberOfColumns ||
			! NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, thy numberOfColumns)) return 0;
	}
	else if (columnOrigin == -1)
	{
		if (my numberOfRows != thy numberOfColumns ||
			! NUMstrings_copyElements (my rowLabels, thy columnLabels, 1, thy numberOfColumns)) return 0;
	}
	return 1;
}

void TableOfReal_labelsFromCollectionItemNames (I, thou, int row, int column)
{
	iam (TableOfReal); 
	thouart (Collection); 
	long i; 
	wchar_t *name;
	
	if (row)
	{
		Melder_assert (my numberOfRows == thy size);
		for (i = 1; i <= my numberOfRows; i++)
		{
			name = Thing_getName (thy item[i]);
			if (name != NULL) TableOfReal_setRowLabel (me, i, name);
		}
	}
	if (column)
	{
		Melder_assert (my numberOfColumns == thy size);
		for (i=1; i <= my numberOfColumns; i++)
		{
			name = Thing_getName (thy item[i]);
			if (name != NULL) TableOfReal_setColumnLabel (me, i, name);
		}
	}
}

void TableOfReal_centreColumns (I)
{
	iam (TableOfReal);
	NUMcentreColumns (my data, 1, my numberOfRows, 1, my numberOfColumns, NULL);
}

int TableOfReal_and_Categories_setRowLabels (I, Categories thee)
{
	iam (TableOfReal);
	Categories c = NULL;
	long i;
	if (my numberOfRows != thy size) return Melder_error5 
		(L"The number of items in ", Thing_messageName(me), L"and ", Thing_messageName(thee), L" must be equal.");
		
	/* 
		If anything goes wrong we must leave the Table intact.
		We first copy the Categories, swap the labels 
		and then delete the newly created categories.
	*/
	
	c = Data_copy (thee);
	if (c == NULL) return 0;
	
	for (i=1; i <= my numberOfRows; i++)
	{
		SimpleString s = c -> item[i];
		wchar_t *t = s -> string;
		s -> string = my rowLabels[i];
		my rowLabels[i] = t;
	}
	
	forget (c);
	return 1;
}

void TableOfReal_centreColumns_byRowLabel (I)
{
	iam (TableOfReal); wchar_t *label = my rowLabels[1];
	long i, index = 1;
		
	for (i=2; i <= my numberOfRows; i++)
	{
		wchar_t *li = my rowLabels[i];
		if (li != NULL && li != label && wcscmp (li, label))
		{
			NUMcentreColumns (my data, index, i - 1, 1, my numberOfColumns, NULL);
			label = li; index = i;
		}
	}
	NUMcentreColumns (my data, index, my numberOfRows, 1, my numberOfColumns, NULL);
}

double TableOfReal_getRowSum (I, long index)
{
	iam (TableOfReal);
	double sum = 0;
	long j;
	if (index < 1 || index > my numberOfRows)
	{
		return NUMundefined;
	}
	for (j = 1; j <= my numberOfColumns; j++)
	{
		sum += my data[index][j];
	}
	return sum;
}

double TableOfReal_getColumnSum (I, long index)
{
	iam (TableOfReal);
	double sum = 0;
	long i;
	if (index < 1 || index > my numberOfColumns)
	{
		return NUMundefined;
	}
	for (i = 1; i <= my numberOfRows; i++)
	{
		sum += my data[i][index];
	}
	return sum;
}

double TableOfReal_getGrandSum (I)
{
	iam (TableOfReal);
	double sum = 0;
	long i, j;
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			sum += my data[i][j];
		}
	}
	return sum;
}

void TableOfReal_centreRows (I)
{
	iam (TableOfReal);
	NUMcentreRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_doubleCentre (I)
{
	iam (TableOfReal);
	NUMdoubleCentre (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeColumns (I, double norm)
{
	iam (TableOfReal);
	NUMnormalizeColumns (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_normalizeRows (I, double norm)
{
	iam (TableOfReal);
	NUMnormalizeRows (my data, my numberOfRows, my numberOfColumns, norm);
}

void TableOfReal_standardizeColumns (I)
{
	iam (TableOfReal);
	NUMstandardizeColumns (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_standardizeRows (I)
{
	iam (TableOfReal);
	NUMstandardizeRows (my data, 1, my numberOfRows, 1, my numberOfColumns);
}

void TableOfReal_normalizeTable (I, double norm)
{
	iam (TableOfReal);
	NUMnormalize (my data, my numberOfRows, my numberOfColumns, norm);
}

double TableOfReal_getTableNorm (I)
{
	iam (TableOfReal); 
	double sumsq = 0; 
	long i, j;
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			sumsq += my data[i][j] * my data[i][j];
		}
	}
	return sqrt (sumsq);
}

int TableOfReal_checkPositive (I)
{
	iam (TableOfReal);
	long i, j, negative = 0;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			if (my data[i][j] < 0) { negative ++; break; }
		}
	}
	return negative == 0 ? 1 : 
		Melder_error2 (L"All matrix entries should be positive for ", Thing_messageName(me));
}

/* NUMundefined ??? */
void NUMdmatrix_getColumnExtrema (double **a, long rowb, long rowe, long icol, double *min, double *max);
void NUMdmatrix_getColumnExtrema (double **a, long rowb, long rowe, long icol, double *min, double *max)
{
	long i;
	*min = *max = a[rowb][icol];
	for (i=rowb+1; i <= rowe; i++)
	{
		double t = a[i][icol];
		if (t > *max) *max = t;
		else if (t < *min) *min = t;
	} 
}

void TableOfReal_drawScatterPlotMatrix (I, Graphics g, long colb, long cole, 
	double fractionWhite)
{
	iam (TableOfReal); 
	double *xmin = NULL, *xmax = NULL;
	long i, j, k, m = my numberOfRows, n;
	
	if (colb == 0 && cole == 0)
	{
		colb = 1; cole = my numberOfColumns;
	}
	else if (cole < colb || colb < 1 || cole > my numberOfColumns) return;
	
	n = cole - colb + 1;
	if (n == 1) return;
		
	if (! (xmin = NUMdvector (colb, cole)) ||
		! (xmax = NUMdvector (colb, cole))) goto end;
	
	for (j=colb; j <= cole; j++)
	{
		xmin[j] = xmax[j] = my data[1][j];
	}
	for (i=2; i <= m; i++)
	{
		for (j=colb; j <= cole; j++)
		{
			if (my data[i][j] > xmax[j]) xmax[j] = my data[i][j];
			else if (my data[i][j] < xmin[j]) xmin[j] = my data[i][j];
		}
	}
	for (j=colb; j <= cole; j++)
	{
		double extra = fractionWhite * fabs (xmax[j] - xmin[j]);
		if (extra == 0) extra = 0.5;
		xmin[j] -= extra; xmax[j] += extra;
	}
	
	Graphics_setWindow (g, 0, n, 0, n);
	Graphics_line (g, 0, n, n, n);
	Graphics_line (g, 0, 0, 0, n);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	
	for (i=1; i <= n; i++)
	{
		long xcol, ycol = colb + i - 1; wchar_t *mark, label[20];
		Graphics_line (g, 0, n - i, n, n - i);
		Graphics_line (g, i, n, i, 0);
		for (j=1; j <= n; j++)
		{
			xcol = colb + j -1;
			if (i == j)
			{
				mark = my columnLabels[xcol];
				if (! mark)
				{
					swprintf (label, 20, L"Column %ld", xcol); mark = label;
				}
				Graphics_text (g, j - 0.5, n - i + 0.5, mark);
			}
			else
			{
				for (k=1; k <= m; k++)
				{
					double x = j - 1 + (my data[k][xcol] - xmin[xcol]) / 
						(xmax[xcol] - xmin[xcol]);
					double y = n - i + (my data[k][ycol] - xmin[ycol]) / 
						(xmax[ycol] - xmin[ycol]); 
					mark = EMPTY_STRING (my rowLabels[k]) ? L"+" : my rowLabels[k];
					Graphics_text (g, x, y, mark);
				}
			}
		}
	}
end:
	NUMdvector_free (xmin, colb); NUMdvector_free (xmax, colb);
}

void TableOfReal_drawScatterPlot (I, Graphics g, long icx, long icy, long rowb, 
	long rowe, double xmin, double xmax, double ymin, double ymax, 
	int labelSize, int useRowLabels, wchar_t *label, int garnish)
{
    iam (TableOfReal); 
	double tmp, m = my numberOfRows, n = my numberOfColumns;
    long i, noLabel = 0;
	int fontSize = Graphics_inqFontSize (g);
    
    if (icx < 1 || icx > n || icy < 1 || icy > n) return;
    if (rowb < 1) rowb = 1;
    if (rowe > m) rowe = m;
    if (rowe <= rowb)
    {
    	rowb = 1; rowe = m;
    }
    
    if (xmax == xmin)
    {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icx, & xmin, & xmax);
		tmp = xmax - xmin == 0 ? 0.5 : 0.0;
		xmin -= tmp; xmax += tmp;
    }
    if (ymax == ymin)
    {
		NUMdmatrix_getColumnExtrema (my data, rowb, rowe, icy, & ymin, & ymax);
		tmp = ymax - ymin == 0 ? 0.5 : 0.0;
		ymin -= tmp; ymax += tmp;
    }
    
    Graphics_setInner (g);
    Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	
    for (i=rowb; i <= rowe; i++)
    {
    	double x = my data[i][icx], y = my data[i][icy];
		
		if (((xmin < xmax && x >= xmin && x <= xmax) || (xmin > xmax && x <= xmin && x >= xmax)) &&
			((ymin < ymax && y >= ymin && y <= ymax) || (ymin > ymax && y <= ymin && y >= ymax)))
		{
			wchar_t *plotLabel = useRowLabels ? my rowLabels[i] : label;
			if (! NUMstring_containsPrintableCharacter (plotLabel))
			{
				noLabel++;
				continue;
			}
			Graphics_text (g, x, y, plotLabel);
		}
	}
	
	Graphics_setFontSize (g, fontSize);
    Graphics_unsetInner (g);
	
    if (garnish)
    {
		Graphics_drawInnerBox (g);
		if (ymin < ymax)
		{
			if (my columnLabels[icx]) Graphics_textBottom (g, 1, my columnLabels[icx]);
			Graphics_marksBottom (g, 2, 1, 1, 0);
		}
		else
		{
			if (my columnLabels[icx]) Graphics_textTop (g, 1, my columnLabels[icx]);
			Graphics_marksTop (g, 2, 1, 1, 0);
		}
		if (xmin < xmax)
		{
			if (my columnLabels[icy]) Graphics_textLeft (g, 1, my columnLabels[icy]);
			Graphics_marksLeft (g, 2, 1, 1, 0);
		}
		else
		{
			if (my columnLabels[icy]) Graphics_textRight (g, 1, my columnLabels[icy]);
			Graphics_marksRight (g, 2, 1, 1, 0);
		}
	}
	if (noLabel > 0) Melder_warning4 (Melder_integer (noLabel), L" from ", Melder_integer (my numberOfRows), L" labels are "
		"not visible because they are empty or they contain only spaces or non-printable characters");
}

/****************  TABLESOFREAL **************************************/

class_methods (TablesOfReal, Ordered)
class_methods_end

int TablesOfReal_init (I, void *klas)
{
	iam (TablesOfReal);
	if (! me || ! Ordered_init (me, klas, 10)) return 0;
	return 1;
}

TablesOfReal TablesOfReal_create (void)
{
	TablesOfReal me = new (TablesOfReal);
	if (! me || ! TablesOfReal_init (me, classTableOfReal)) forget (me);
	return me;
}

TableOfReal TablesOfReal_sum (I)
{
	iam (TablesOfReal); TableOfReal thee;
	long i, j, k;
	if (my size <= 0) return NULL;
	if (! (thee = Data_copy (my item[1]))) { forget (thee); return NULL; }
	for (i=2; i <= my size; i++)
	{
		TableOfReal him = my item[i];
		if (thy numberOfRows != his numberOfRows || thy numberOfColumns != his numberOfColumns ||
			! TableOfReal_equalLabels (thee, him, 1, 1))
		{
			forget (thee);
			return Melder_errorp5 (L"TablesOfReal_sum: dimensions or labels differ for items 1 and ", Melder_integer(i), L" in ", Thing_messageName(thee), Thing_messageName(him));
		}
		for (j=1; j <= thy numberOfRows; j++)
			for (k=1; k <= thy numberOfColumns; k++) thy data[j][k] += his data[j][k]; 
	}
	return thee;
}

int TablesOfReal_checkDimensions (I)
{
	iam (TablesOfReal); TableOfReal t1; long i;
	if (my size < 2) return 1;
	t1 = my item[1];
	for (i=2; i <= my size; i++)
	{
		TableOfReal t = my item[i];
		if (t -> numberOfColumns != t1 -> numberOfColumns ||
			t -> numberOfRows != t1 -> numberOfRows) return 0;
	}
	return 1;
}

double TableOfReal_getColumnQuantile (I, long col, double quantile)
{
	iam (TableOfReal); long i, m = my numberOfRows;
	double *values, r;
	
	if (col < 1 || col > my numberOfColumns) return NUMundefined;
	
	if (! (values = NUMdvector (1, m))) return NUMundefined;
	
	for (i=1; i <= m; i++)
	{
		values[i] = my data[i][col];
	}
	
	NUMsort_d (m, values);
	r = NUMquantile (m, values, quantile);
	
	NUMdvector_free (values, 1);
	
	return r;
}

static TableOfReal TableOfReal_createPolsVanNieropData (int choice, int include_levels)
{
	Table table; 
	TableOfReal thee = NULL;
	long i, j, ib, nrows, ncols = include_levels ? 6 : 3;
	
	table = Table_createFromPolsVanNieropData ();
	if (table == NULL) return NULL;
	
	/* Default: Pols 50 males, first part of the table. */
	
	nrows = 50 * 12;
	ib = 1;
	
	if (choice == 2) /* Van Nierop 25 females */
	{
		ib = nrows + 1;
		nrows = 25 * 12;
	}
	
	thee = TableOfReal_create (nrows, ncols);
	if (thee == NULL) goto end;
	
	for (i = 1; i <= nrows; i++)
	{
		TableRow row = table -> rows -> item[ib + i - 1];
		TableOfReal_setRowLabel (thee, i, row -> cells[4].string);
		for (j = 1; j <= 3; j++)
		{
			thy data[i][j] = Melder_atof (row -> cells[4+j].string);
			if (include_levels) thy data[i][3+j] = Melder_atof (row -> cells[7+j].string);
		}
	}
	for (j = 1; j <= 3; j++)
	{
		wchar_t *label = table -> columnHeaders[4+j].label;
		TableOfReal_setColumnLabel (thee, j, label);
		if (include_levels)
		{
			label = table -> columnHeaders[7+j].label;
			TableOfReal_setColumnLabel (thee, 3+j, label);
		}
	}

end:
	forget (table);
	return thee;
}

TableOfReal TableOfReal_createFromPolsData_50males (int include_levels)
{
	return TableOfReal_createPolsVanNieropData (1, include_levels);
}

TableOfReal TableOfReal_createFromVanNieropData_25females (int include_levels)
{
	return TableOfReal_createPolsVanNieropData (2, include_levels);
}

TableOfReal TableOfReal_createFromWeeninkData (int option)
{
	Table table; 
	TableOfReal thee = NULL;
	long i, ib, j, nvowels = 12, ncols = 3, nrows = 10 * nvowels;
	
	table = Table_createFromWeeninkData ();
	if (table == NULL) return NULL;

	ib = option == 1 ? 1 : option == 2 ? 11 : 21; /* m f c*/ 
	ib = (ib -1) * nvowels + 1;
	
	thee = TableOfReal_create (nrows, ncols);
	if (thee == NULL) goto end;
	
	for (i = 1; i <= nrows; i++)
	{
		TableRow row = table -> rows -> item[ib + i - 1];
		TableOfReal_setRowLabel (thee, i, row -> cells[5].string);
		for (j = 1; j <= 3; j++)
		{
			thy data[i][j] = Melder_atof (row -> cells[6+j].string); /* Skip F0 */
		}
	}
	for (j = 1; j <= 3; j++)
	{
		wchar_t *label = table -> columnHeaders[6+j].label;
		TableOfReal_setColumnLabel (thee, j, label);
	}
end:
	forget (table);
	return thee;
}

TableOfReal TableOfReal_randomizeRows (TableOfReal me)
{
	TableOfReal thee = NULL;
	Permutation p = NULL, pp = NULL;
	
	p = Permutation_create (my numberOfRows);
	if (p == NULL) return NULL;
	pp = Permutation_permuteRandomly (p, 0, 0);
	if (pp == NULL) goto end;
	thee = TableOfReal_and_Permutation_permuteRows (me, pp);
end:
	forget (p);
	forget (pp);
	return thee;
}

TableOfReal TableOfReal_bootstrap (TableOfReal me)
{
	TableOfReal thee = NULL;
	long i;

	thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
	if (thee == NULL) return NULL;

	/*
		Copy column labels.
	*/

	for (i = 1; i <= my numberOfColumns; i++)
	{
		if (my columnLabels[i])
		{
			TableOfReal_setColumnLabel (thee, i, my columnLabels[i]);
		}
	}

	/*
		Select randomly from table with replacement. Because of replacement
		you do not get back the original data set. A random fraction, 
		typically 1/e (37%) are replaced by duplicates.
	*/
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		long p = NUMrandomInteger (1, my numberOfRows);
		NUMdvector_copyElements (my data[p], thy data[i], 
			1, my numberOfColumns);
		if (my rowLabels[p])
		{
			TableOfReal_setRowLabel (thee, i, my rowLabels[p]);
		}
	}	

	if (Melder_hasError ()) forget (thee);
	return thee;
}

int TableOfReal_changeRowLabels (I, wchar_t *search, wchar_t *replace, 
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, 
	int use_regexp)
{
	iam (TableOfReal);
	wchar_t ** rowLabels = strs_replace (my rowLabels, 1, my numberOfRows, 
		search, replace, maximumNumberOfReplaces, nmatches, 
		nstringmatches, use_regexp);
	if (rowLabels == NULL) return 0;
	NUMstrings_free (my rowLabels, 1, my numberOfRows);
	my rowLabels = rowLabels;
	return 1;
}

int TableOfReal_changeColumnLabels (I, wchar_t *search, wchar_t *replace, 
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, 
	int use_regexp)
{
	iam (TableOfReal);
	wchar_t ** columnLabels = strs_replace (my columnLabels, 1, my numberOfColumns, 
		search, replace, maximumNumberOfReplaces, nmatches, 
		nstringmatches, use_regexp);
	if (columnLabels == NULL) return 0;
	NUMstrings_free (my columnLabels, 1, my numberOfColumns);
	my columnLabels = columnLabels;
	return 1;
}

long TableOfReal_getNumberOfLabelMatches (I, wchar_t *search, int columnLabels, 
	int use_regexp)
{
	iam (TableOfReal);
	long i, nmatches = 0, numberOfLabels = my numberOfRows;
	wchar_t **labels = my rowLabels;
	regexp *compiled_regexp = NULL;
	
	if (search == NULL || wcslen (search) == 0) return 0;
	if (columnLabels)
	{
		numberOfLabels = my numberOfColumns;
		labels = my columnLabels;
	}	
	if (use_regexp)
	{
		char *compileMsg;
		compiled_regexp = CompileRE (Melder_peekWcsToUtf8 (search), &compileMsg, 0);
		if (compiled_regexp == NULL) return Melder_error1 (Melder_utf8ToWcs (compileMsg));
	}
	for (i = 1; i <= numberOfLabels; i++)
	{
		if (labels[i] == NULL) continue;
		if (use_regexp)
		{
			if (ExecRE (compiled_regexp, NULL, Melder_peekWcsToUtf8 (labels[i]), NULL, 0, 
				'\0', '\0', NULL, NULL, NULL)) nmatches++;
		}
		else if (wcsequ (labels[i], search)) nmatches++;
	}
	if (use_regexp) free (compiled_regexp);
	return nmatches;
}

void TableOfReal_drawVectors (I, Graphics g, long colx1, long coly1, 
	long colx2, long coly2, double xmin, double xmax, 
	double ymin, double ymax, int vectype, int labelsize, int garnish)
{
	iam (TableOfReal);
	long i, nx = my numberOfColumns, ny = my numberOfRows;
	double min, max;
	int fontsize = Graphics_inqFontSize (g);

	if (colx1 < 1 || colx1 > nx || coly1 < 1 || coly1 > nx)
	{
		Melder_warning3 (L"The index in the \"From\" column(s) must be in range [1, ", Melder_integer (nx), L"].");
		return;
	}
	if (colx2 < 1 || colx2 > nx || coly2 < 1 || coly2 > nx)
	{
		Melder_warning3 (L"The index in the \"To\" column(s) must be in range [1, ", Melder_integer (nx), L"].");
		return;
	}
 
	if (xmin >= xmax)
	{
		NUMdmatrix_extrema (my data, 1, ny, colx1, colx1, &min, &max);
		NUMdmatrix_extrema (my data, 1, ny, colx2, colx2, &xmin, &xmax);
		if (min < xmin) xmin = min;
		if (max > xmax) xmax = max;
	}
	if (ymin >= ymax)
	{
		NUMdmatrix_extrema (my data, 1, ny, coly1, coly1, &min, &max);
		NUMdmatrix_extrema (my data, 1, ny, coly2, coly2, &ymin, &ymax);
		if (min < ymin) ymin = min;
		if (max > ymax) ymax = max;
	}
	if (xmin == xmax)
	{
		if (ymin == ymax) return;
		xmin -= 0.5;
		xmax += 0.5;
	}
	if (ymin == ymax)
	{
		ymin -= 0.5;
		ymax += 0.5;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	if (labelsize > 0) Graphics_setFontSize (g, labelsize);			
	for (i = 1; i <= ny; i++)
	{
		float x1 = my data[i][colx1];
		float y1 = my data[i][coly1];	 
		float x2 = my data[i][colx2];
		float y2 = my data[i][coly2];
		wchar_t *mark = EMPTY_STRING (my rowLabels[i]) ? L"" : my rowLabels[i];	 
		if (vectype == Graphics_LINE)
			Graphics_line (g, x1, y1, x2, y2);
		else if (vectype == Graphics_TWOWAYARROW)
		{
			Graphics_arrow (g, x1, y1, x2, y2);
			Graphics_arrow (g, x2, y2, x1, y1);
		}
		else /*if (vectype == Graphics_ARROW) */	
			Graphics_arrow (g, x1, y1, x2, y2);
		if (labelsize <= 0) continue;
		Graphics_text (g, x1, y1, mark);

	}
	if (labelsize > 0) Graphics_setFontSize (g, fontsize);
	Graphics_unsetInner (g);
	if (garnish)
	{
	    Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
	}
}

TableOfReal TableOfReal_sortRowsByIndex (I, long *index, int reverse)
{
	iam (TableOfReal);
	TableOfReal thee = NULL;
	double min, max;
	long i, j;

	if (my rowLabels == NULL) return NULL;
	
	NUMlvector_extrema (index, 1, my numberOfRows, &min, &max);
	if (min < 1 || max > my numberOfRows) return Melder_errorp
		("TableOfReal_sortRowsByIndex: one or more indices out of range [1, %d].",
		my numberOfRows);
	
	thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		long    myindex = reverse ? i : index[i];
		long   thyindex = reverse ? index[i] : i;
		wchar_t   *mylabel = my rowLabels[myindex];
		double  *mydata = my data[myindex];
		double *thydata = thy data[thyindex];
		
		/*
			Copy the row label
		*/
		
		if (mylabel != NULL)
		{
			thy rowLabels[i] = Melder_wcsdup (mylabel);
			if (thy rowLabels[i] == NULL) goto end;
		}
		
		/*
			Copy the row values
		*/
		
		for (j = 1; j <= my numberOfColumns; j++)
		{
			thydata[j] = mydata[j];
		}
	}
	
	/*
		Copy column labels.
	*/
	
	(void) NUMstrings_copyElements (my columnLabels, thy columnLabels, 
		1, my numberOfColumns);
end:

	if (Melder_hasError()) forget (thee);
	return thee;
}

long *TableOfReal_getSortedIndexFromRowLabels (I)
{
	iam (TableOfReal); 
	long *index = NUMlvector (1, my numberOfRows);
	
	if (index == NULL) return NULL;
	NUMindexx_s (my rowLabels, my numberOfRows, index);
	return index;
}

TableOfReal TableOfReal_sortOnlyByRowLabels (I)
{
	iam (TableOfReal); 
	TableOfReal thee = NULL;
	Permutation index = NULL;
	
	index = TableOfReal_to_Permutation_sortRowLabels (me);
	if (index == NULL) return NULL;
	
	thee = TableOfReal_and_Permutation_permuteRows (me, index);

	forget (index);
	return thee;
}

static void NUMmedianizeColumns (double **a, long rb, long re, long cb, long ce)
{
	long i, j, k, n = re - rb + 1;
	double *tmp, median;
	
	if (n < 2) return;
	tmp = NUMdvector (1, n);
	if (tmp == NULL) return;
	for (j = cb; j <= ce; j++)
	{
		k = 1;
		for (i = rb; i <= re; i++, k++) tmp[k]= a[i][j];
		NUMsort_d (n, tmp);
		median = NUMquantile (n, tmp, 0.5);
		for (i = rb; i <= re; i++) a[i][j] = median;
	}
	NUMdvector_free (tmp, 1);
}

static void NUMstatsColumns (double **a, long rb, long re, long cb, long ce, int stats)
{
	if (stats == 0)
	{
		NUMaverageColumns (a, rb, re, cb, ce);
	}
	else
	{
		NUMmedianizeColumns (a, rb, re, cb, ce);
	}
}

TableOfReal TableOfReal_meansByRowLabels (I, int expand, int stats)
{
	iam (TableOfReal);
	TableOfReal thee = NULL, sorted = NULL;
	wchar_t *label, **tmp;
	long *index = NULL, indexi = 1, indexr = 0, i;

	index = TableOfReal_getSortedIndexFromRowLabels (me);
	if (index == NULL) return NULL;
	
	sorted = TableOfReal_sortRowsByIndex (me, index, 0);
	if (sorted == NULL) goto end;

	label = sorted -> rowLabels[1];
	for (i = 2; i <= my numberOfRows; i++)
	{
		wchar_t *li = sorted -> rowLabels[i];
		if (li != NULL && li != label && (label == NULL || wcscmp (li, label)))
		{
			NUMstatsColumns (sorted -> data, indexi, i - 1, 1, my numberOfColumns, stats);

			if (expand == 0)
			{
				indexr++;
				if (!TableOfReal_copyOneRowWithLabel (sorted, sorted, indexi, indexr)) goto end;
			}
			label = li; indexi = i;
		}
	}
		
	NUMstatsColumns (sorted -> data, indexi, my numberOfRows, 1, my numberOfColumns, stats);
	
	if (expand != 0)
	{
		/*
			Now inverse the table.
		*/
		tmp = sorted -> rowLabels; sorted -> rowLabels = my rowLabels;
		thee = TableOfReal_sortRowsByIndex (sorted, index, 1);
		sorted -> rowLabels = tmp;
	}
	else
	{
		indexr++;
		if (! TableOfReal_copyOneRowWithLabel (sorted, sorted, indexi, indexr)) goto end;
		thee = TableOfReal_create (indexr, my numberOfColumns);
		if (thee == NULL) goto end;
		for (i = 1; i <= indexr; i++)
		{
			if (!TableOfReal_copyOneRowWithLabel (sorted, thee, i, i)) goto end;
		}
		if (! NUMstrings_copyElements (sorted -> columnLabels, thy columnLabels, 1, my numberOfColumns)) goto end;
	}
	
end:
	forget (sorted);
	NUMlvector_free (index, 1);
	if (Melder_hasError()) forget (thee);
	return thee;
}

TableOfReal TableOfReal_rankColumns (I)
{
	iam (TableOfReal);
	TableOfReal thee = Data_copy (me);
	
	if (thee == NULL) return NULL;
	if (! NUMrankColumns (thy data, 1, thy numberOfRows, 
		1, thy numberOfColumns)) forget (thee);
	return thee;	
}
	
int TableOfReal_setSequentialColumnLabels (I, long from, long to, 
	wchar_t *precursor, long number, long increment)
{
	iam (TableOfReal);
	
	if (from == 0) from = 1;
	if (to == 0) to = my numberOfColumns;
	if (from < 1 || from > my numberOfColumns || to < from ||
		to > my numberOfColumns) return Melder_error2 
			(L"TableOfReal_setSequentialColumnLabels: wrong column indices for ", Thing_messageName(me));
	return NUMstrings_setSequentialNumbering (my columnLabels, from, to, 
		precursor, number, increment);
}
	
int TableOfReal_setSequentialRowLabels (I, long from, long to, 
	wchar_t *precursor, long number, long increment)
{
	iam (TableOfReal);
	
	if (from == 0) from = 1;
	if (to == 0) to = my numberOfRows;
	if (from < 1 || from > my numberOfRows || to < from ||
		to > my numberOfRows) return Melder_error2 
			(L"TableOfReal_setSequentialRowLabels: wrong row indices for ", Thing_messageName(me));
	return NUMstrings_setSequentialNumbering (my rowLabels, from, to, 
		precursor, number, increment);
}

/* For the inheritors */
TableOfReal TableOfReal_to_TableOfReal (I)
{
	iam (TableOfReal);
	TableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
	if (thee == NULL) return NULL;
	
	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 
		1, my numberOfColumns);
	if (! TableOfReal_copyLabels (me, thee, 1, 1)) forget (thee);
	return thee;
}

TableOfReal TableOfReal_choleskyDecomposition (I, int upper, int inverse)
{
	iam (TableOfReal);
	wchar_t *proc = L"TableOfReal_choleskyDecomposition"; char uplo = 'U', diag = 'N';
	long i, j, n = my numberOfColumns, lda = my numberOfRows, info;
	TableOfReal thee;

	if (n != lda) return Melder_errorp4 (proc, L": The matrix part of ", Thing_messageName(me), L" must be a square symmetric matrix.");
	if ((thee = Data_copy (me)) == NULL) return NULL;

	if (upper)
	{
		uplo = 'L'; /* Fortran storage */
		for (i = 2; i <= n; i++) for (j = 1; j < i; j++) thy data[i][j] = 0;
	}
	else
	{
		for (i = 1; i < n; i++) for (j = i+1; j <= n; j++) thy data[i][j] = 0;
	}
	(void) NUMlapack_dpotf2 (&uplo, &n, &thy data[1][1], &lda, &info);
	if (info != 0) goto end;

	if (inverse)
	{
		(void) NUMlapack_dtrtri (&uplo, &diag, &n, &thy data[1][1], &lda, &info);
	}

end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

TableOfReal TableOfReal_appendColumns (I, thou)
{
	iam (TableOfReal); thouart (TableOfReal);
	TableOfReal him;
	long i, ncols = my numberOfColumns + thy numberOfColumns;
	long labeldiffs = 0;

	if (my numberOfRows != thy numberOfRows) return Melder_errorp4 (Thing_messageName(me), L" and ", 
		Thing_messageName(thee), L" must have an equal number of rows.");
	/* Stricter label checking???
		append only if
		  (my rowLabels[i] == thy rowlabels[i], i=1..my numberOfRows) or
		  (my rowLabels[i] == 'empty', i=1..my numberOfRows)  or 
		  (thy rowLabels[i] == 'empty', i=1..my numberOfRows);
		'empty':  NULL or \w*
	*/
	him = TableOfReal_create (my numberOfRows, ncols);
	if (him == NULL) return NULL;
	if (! NUMstrings_copyElements (my rowLabels, his rowLabels, 1, my numberOfRows) ||
		! NUMstrings_copyElements (my columnLabels, his columnLabels,  1, my numberOfColumns) ||
		! NUMstrings_copyElements (thy columnLabels, &his columnLabels[my numberOfColumns],
			1, thy numberOfColumns)) goto end;
	for (i = 1; i <= my numberOfRows; i++)
	{
		if (NUMwcscmp (my rowLabels[i], thy rowLabels[i])) labeldiffs++;
		NUMdvector_copyElements (my data[i], his data[i], 1, my numberOfColumns);
		NUMdvector_copyElements (thy data[i], &his data[i][my numberOfColumns], 1, thy numberOfColumns);
	}
end:
	if (Melder_hasError())
	{
		forget (him);
	}
	else if (labeldiffs > 0)
	{
		Melder_warning2 (Melder_integer (labeldiffs), L" row labels differed.");
	}
	return him;
}

Any TableOfReal_appendColumnsMany (Collection me) 
{
	TableOfReal him = NULL, thee;
	long itab, irow, icol, nrow, ncol;
	
	if (my size == 0) return Melder_errorp1 (L"No tables selected.");
	thee = my item [1];
	nrow = thy numberOfRows;
	ncol = thy numberOfColumns;
	for (itab = 2; itab <= my size; itab++)
	{
		thee = my item [itab];
		ncol += thy numberOfColumns;
		if (thy numberOfRows != nrow)
		{
			Melder_error3 (L"Numbers of rows in ", Thing_messageName(thee), L" does not match the others.");
			goto end;
		}
	}
	if ((him = Thing_new (thy methods)) == NULL || 
		! TableOfReal_init (him, nrow, ncol)) goto end;
	/* Unsafe: new attributes not initialized. */
	for (irow = 1; irow <= nrow; irow++)
	{
		TableOfReal_setRowLabel (him, irow, thy rowLabels [irow]);
		if (Melder_hasError ()) goto end;
	}
	ncol = 0;
	for (itab = 1; itab <= my size; itab++)
	{
		thee = my item [itab];
		for (icol = 1; icol <= thy numberOfColumns; icol++)
		{
			ncol++;
			TableOfReal_setColumnLabel (him, ncol, thy columnLabels [icol]);
			if (Melder_hasError ()) goto end;
			for (irow = 1; irow <= nrow; irow++)
			{
				his data[irow][ncol] = thy data[irow][icol];
			}
		}
	}
	Melder_assert (ncol == his numberOfColumns);
	
end:

	if (Melder_hasError ()) forget (him);
	return him;
}

#undef EMPTY_STRING
#undef MAX
#undef MIN

/* End of file TableOfReal_extensions.c */
