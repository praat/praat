/* Configuration.c
 * 
 * Copyright (C) 1993-2004 David Weenink
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
 djmw 20010920 adapted NUMprocrustus
 djmw 20010920 +Configuration_getDilationFactor
 djmw 20020315 GPL header
 djmw 20030513 applied change in numeric label generation
 djmw 20030801 Configuration_drawConcentrationEllipses extra argument
 djmw 20040303 Moved containsPrintableCharacter to NUM2.c
 djmw 20041026 Removed non-used code.
 */

#include <ctype.h>
#include "SVD.h"
#include "PCA.h"
#include "Configuration.h"
#include "Configuration_AffineTransform.h"
#include "TableOfReal_extensions.h"
#include "SSCP.h"
#include "Procrustus.h"

#include "oo_DESTROY.h"
#include "Configuration_def.h"
#include "oo_COPY.h"
#include "Configuration_def.h"
#include "oo_EQUAL.h"
#include "Configuration_def.h"
#include "oo_WRITE_ASCII.h"
#include "Configuration_def.h"
#include "oo_WRITE_BINARY.h"
#include "Configuration_def.h"
#include "oo_READ_ASCII.h"
#include "Configuration_def.h"
#include "oo_READ_BINARY.h"
#include "Configuration_def.h"
#include "oo_DESCRIPTION.h"
#include "Configuration_def.h"

static void classConfiguration_info (I)
{
	iam (Configuration);
	Melder_information ("Number of points = %ld\n"
		"Number of dimensions = %ld\n"
		"Metric = %ld\n",
		my numberOfRows, my numberOfColumns, my metric);
}

class_methods (Configuration, TableOfReal)
	class_method_local (Configuration, destroy)
	class_method_local (Configuration, equal)
	class_method_local (Configuration, copy)
	class_method_local (Configuration, readAscii)
	class_method_local (Configuration, readBinary)
	class_method_local (Configuration, writeAscii)
	class_method_local (Configuration, writeBinary)
	class_method_local (Configuration, description)
	class_method_local (Configuration, info)
class_methods_end


Configuration Configuration_create (long numberOfPoints, 
	long numberOfDimensions)
{
	Configuration me = new (Configuration);
	
	if (me == NULL) return NULL;
	
	if (! TableOfReal_init (me, numberOfPoints, numberOfDimensions) ||
		! (my w = NUMdvector (1, numberOfDimensions)) ||
		! TableOfReal_setSequentialRowLabels (me, 0, 0, NULL, 1, 1) ||
		! TableOfReal_setSequentialColumnLabels (me, 0, 0, "dimension ", 1, 1))
			goto end;

	my metric = 2;

	Configuration_setDefaultWeights (me);
	Configuration_randomize (me);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

void Configuration_setMetric (Configuration me, long metric)
{
	my metric = metric;
}

void Configuration_setDefaultWeights (Configuration me)
{
	long i;
	for (i = 1; i <= my numberOfColumns; i++)
	{
		my w[i] = 1;
	}
}

void Configuration_setSqWeights (Configuration me, const double weight[])
{
	long i;
	for (i = 1; i <= my numberOfColumns; i++)
	{
		my w[i] = sqrt (weight[i]);
	}
}

void Configuration_normalize (Configuration me, double sumsq, int columns)
{
	TableOfReal_centreColumns (me);
	if (columns)
	{
		sumsq = sumsq <= 0 ? 1.0 : sqrt (sumsq);
		NUMnormalizeColumns_d (my data, my numberOfRows, my numberOfColumns,
			sumsq);
	}
	else
	{
		if (sumsq <= 0) sumsq = my numberOfRows;
		NUMnormalize_d (my data, my numberOfRows, my numberOfColumns, 
			sqrt (sumsq));
	}
}

void Configuration_randomize (Configuration me)
{
	long i, j;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			my data[i][j] = NUMrandomUniform (-1, 1);
		}
	}
}

void Configuration_rotate (Configuration me, long dimension1, long dimension2, 
	double angle_degrees)
{
	long i; 
	double f = NUMpi * (2 - angle_degrees / 180);
	double cosa = cos (f), sina = sin (f);

	if (dimension1 == dimension2) return;
	
	if (dimension1 > dimension2)
	{
		long dt = dimension1; dimension1 = dimension2; dimension2 = dt;
	}
	if (dimension1 < 1 || dimension2 > my numberOfColumns) return;
	for (i = 1; i <= my numberOfRows; i++)
	{
		double x1 = my data[i][dimension1], x2 = my data[i][dimension2];
		my data[i][dimension1] =   cosa * x1 + sina * x2;
		my data[i][dimension2] = - sina * x1 + cosa * x2;
	}
}

void Configuration_invertDimension (Configuration me, int dimension)
{
	long i;
	
	if (dimension < 1 || dimension > my numberOfColumns) return;
	
	for (i=1; i <= my numberOfRows; i++)
	{
		my data[i][dimension] = - my data[i][dimension];
	}
}


static double NUMsquaredVariance (double **a, long nr, long nc, int rawPowers)
{
	double sum4, mean, v4; 
	long i, j;
	
	for (v4 = 0, j = 1; j <= nc; j++)
	{
		for (sum4 = 0, mean = 0, i = 1; i <= nr; i++)
		{
			double sq = a[i][j] * a[i][j];
			sum4 += sq * sq;
			mean += sq;
		}
		v4 += sum4;
		if (! rawPowers) v4 -= mean * mean / nr;
	}
	return v4;
}

/*
	Varimax rotation, implementation according to:
		Jos Ten Berge (1995), "Suppressing permutations or rigid
		planar rotations: a remedy against nonoptimal varimax rotations",
		Psychometrika 60, 437-446.
*/
static int NUMvarimax (double **xm, double **ym, long nr, long nc, 
	int normalizeRows, int quartimax, long maximumNumberOfIterations, 
	double tolerance)
{
	long numberOfIterations = 0, i, j, c1, c2;
	double varianceSq, varianceSq_old, *u = NULL, *v = NULL, *norm = NULL;

	Melder_assert (nr > 0 && nc > 0);
	
	NUMdmatrix_copyElements (xm, ym, 1, nr, 1, nc);
	
	if (nc == 1) return 1;
	if (nc == 2) maximumNumberOfIterations = 1;
	
	if (! (u = NUMdvector (1, nr)) ||
		! (v = NUMdvector (1, nr))) goto end;
	
	/*
		Normalize sum of squares of each row to one.
		After rotation we have to rescale. 
	*/
		
	if (normalizeRows)
	{
		if (! (norm = NUMdvector (1, nr))) goto end;
		for (i = 1; i <= nr; i++)
		{
			for (j = 1; j <= nc; j++)
			{
				norm[i] += ym[i][j] * ym[i][j];
			}
			if (norm[i] <= 0) continue;
			norm[i] = sqrt (norm[i]);
			for (j = 1; j <= nc; j++)
			{
				ym[i][j] /= norm[i];
			}
		}
	}
	
	/*
		Initial squared "variance".
	*/
	
	varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);
	
	if (varianceSq == 0) goto end;
	
	/*
		Treat columns pairwise.
	*/
	
	do 
	{	
		for (c1 = 1; c1 <= nc; c1++)
		{
			for (c2 = c1 + 1; c2 <= nc; c2++)
			{
				double a, b, c, w, um, vm, t11, t12, t21, t22, cost, sint;
				for (um = vm = 0, i = 1; i <= nr; i++)
				{
					double x = ym[i][c1], y = ym[i][c2];
					u[i] = x * x - y * y;
					um += u[i];
					v[i] = 2 * x * y;
					vm += v[i];
				}	
				um /= nr; vm /= nr;
				if (quartimax || nr == 1) um = vm = 0;
				
				/*
					In the paper just before equation (1):
					a = 2n u'v, b = n(u'u-v'v), c = sqrt(a^2+b^2)
					w = -sign(a) sqrt((b+c) / 2c)
					Tricks: multiplication with n drops out!
						a's multiplication by 2 outside the loop.			
				*/
				
				for (a = b = 0, i = 1; i <= nr; i++)
				{
					double ui = u[i] - um, vi = v[i] - vm;
					a += ui * vi;
					b += ui * ui - vi * vi;
				}
				c = sqrt (4 * a * a + b * b);
				w = sqrt ((c + b) / (2 * c));
				if (a > 0) w = -w;
				t11 = t22 = cost = sqrt (0.5 + 0.5 * w);
				t12 = sint = sqrt (0.5 - 0.5 * w);
				t21 = -sint;
				
				/*
					Prevent permutations: when w < 0, i.e., a > 0, 
						swap columns of T:
				*/
				
				if (w < 0)
				{
					t11 = sint; t12 = t21 = cost; t22 = -sint;
				}
				
				/*
					Rotate in the plane spanned by c1 and c2.
				*/

				for (i = 1; i <= nr; i++)
				{
					double *xt = ym[i], xtc1 = xt[c1];
					xt[c1] = xtc1 * t11 + xt[c2] * t21;
					xt[c2] = xtc1 * t12 + xt[c2] * t22;
				}
			}
		}
		
		numberOfIterations++;
		varianceSq_old = varianceSq;
		varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);
		
	} while (fabs(varianceSq_old - varianceSq) / varianceSq_old > tolerance &&
		numberOfIterations < maximumNumberOfIterations);
		
	if (normalizeRows)
	{
		for (i = 1; i <= nr; i++)
		{
			for (j = 1; j <= nc; j++) ym[i][j] *= norm[i];
		}
	}
	
end:

	if (normalizeRows) NUMdvector_free (norm, 1);
	NUMdvector_free (u, 1);
	NUMdvector_free (v, 1);
	return !Melder_hasError();
}

Configuration Configuration_varimax (Configuration me, int normalizeRows,
	int quartimax, long maximumNumberOfIterations, double tolerance)
{
	Configuration thee = Data_copy (me);
	
	if (! thee) return NULL;
	
	if (! NUMvarimax (my data, thy data, my numberOfRows, my numberOfColumns, 
		normalizeRows, quartimax, maximumNumberOfIterations, tolerance))
			forget (thee);
	
	return thee;
}



Configuration Configuration_congruenceRotation (Configuration me, 
	Configuration thee, long maximumNumberOfIterations, double tolerance)
{
	Configuration him = NULL;
	AffineTransform at = NULL;

	at = Configurations_to_AffineTransform_congruence (me, thee,
		maximumNumberOfIterations, tolerance);
	if (at == NULL) return NULL;
	
	him = Configuration_and_AffineTransform_to_Configuration (me, at);
	
	forget (at);
	if (Melder_hasError ()) forget (him);
	return him;
}

/* Replace by TableOfReal_to_Configuration_pca ??? */

int Configuration_rotateToPrincipalDirections (Configuration me)
{
	double **m = NUMdmatrix_copy (my data, 1, my numberOfRows, 
		1, my numberOfColumns);
	if (m == NULL) return 0;
	
	if (! NUMdmatrix_into_principalComponents (my data, my numberOfRows, 
		my numberOfColumns, my numberOfColumns, m))
	{
		 NUMdmatrix_free (m, 1, 1); return 0;
	}
	NUMdmatrix_free (my data, 1, 1);
	my data = m;
	return 1;
}

void Configuration_draw (Configuration me, Graphics g, int xCoordinate, 
	int yCoordinate, double xmin, double xmax, double ymin, double ymax, 
	int labelSize, int useRowLabels, char *label, int garnish)
{
	long i, nPoints = my numberOfRows, numberOfDimensions = my numberOfColumns;
	double *x = NULL, *y = NULL;
	int fontSize = Graphics_inqFontSize (g), noLabel = 0;
	
	if (numberOfDimensions > 1 && (xCoordinate > numberOfDimensions || 
		yCoordinate > numberOfDimensions)) return;
	if (numberOfDimensions == 1) xCoordinate = 1;
	if (labelSize == 0) labelSize = fontSize;
	if (! (x = NUMdvector (1, nPoints)) || 
		! (y = NUMdvector (1, nPoints))) goto end;
		
	for (i = 1; i <= nPoints; i++)
	{
		x[i] = my data[i][xCoordinate] * my w[xCoordinate];
		y[i] = numberOfDimensions > 1 ? 
			my data[i][yCoordinate] * my w[yCoordinate] : 0;
	}
	if (xmax <= xmin) NUMdvector_extrema (x, 1, nPoints, &xmin, &xmax);
	if (xmax <= xmin) { xmax += 1; xmin -= 1; }
	if (ymax <= ymin) NUMdvector_extrema (y, 1, nPoints, &ymin, &ymax);
	if (ymax <= ymin) { ymax += 1; ymin -= 1; }
    Graphics_setWindow (g, xmin, xmax, ymin, ymax);	
    Graphics_setInner (g);
    Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	for (i = 1; i <= my numberOfRows; i++)
	{
		if (x[i] >= xmin && x[i] <= xmax && y[i] >= ymin && y[i] <= ymax)
		{
			char *plotLabel = useRowLabels ? my rowLabels[i] : label;
			if (! NUMstring_containsPrintableCharacter (plotLabel)) noLabel++;
			Graphics_text (g, x[i], y[i], plotLabel);
		}
	}
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
    	if (numberOfDimensions > 1)
    	{
    		Graphics_marksLeft (g, 2, 1, 1, 0);
    		if (my columnLabels[xCoordinate])
    			Graphics_textBottom (g, 1, my columnLabels[xCoordinate]);
			if (my columnLabels[yCoordinate])
				Graphics_textLeft (g, 1, my columnLabels[yCoordinate]);
    	}
	}
	
	if (noLabel > 0) Melder_warning ("Configuration_draw: %d from %d labels are "
		"not visible because they are empty or they contain only spaces or "
		"they contain only non-printable characters", noLabel, my numberOfRows);
	
end:

	NUMdvector_free (x, 1);
	NUMdvector_free (y, 1);
}


void Configuration_drawConcentrationEllipses (Configuration me, Graphics g, 
	double scale, int confidence, char *label, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int garnish)
{
	SSCPs sscps = TableOfReal_to_SSCPs_byLabel (me);

	if (sscps == NULL) return;
	
	SSCPs_drawConcentrationEllipses (sscps, g, scale, confidence, label,
			d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);

	forget (sscps);
}

Configuration TableOfReal_to_Configuration (I)
{
	iam (TableOfReal); 
	Configuration thee;

	thee = Configuration_create (my numberOfRows, my numberOfColumns);
	if (thee == NULL) return NULL;

	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 
		1, my numberOfColumns);
	if (! TableOfReal_copyLabels (me, thee, 1, 1)) forget (thee);
	return thee;
}

Configuration TableOfReal_to_Configuration_pca (TableOfReal me, 
	long numberOfDimensions)
{
	Configuration thee = NULL; 
	PCA pca= NULL; 

	if (numberOfDimensions < 1 || numberOfDimensions > my numberOfColumns)
		numberOfDimensions = my numberOfColumns;
		
	if ((pca = TableOfReal_to_PCA (me)) == NULL) return NULL;

	thee = PCA_and_TableOfReal_to_Configuration (pca, me, numberOfDimensions);
	forget (pca);
	return thee;
}

/********************** Examples *********************************************/

Configuration Configuration_createLetterRExample (int choice)
{
	long i;
	double x1[33] = { 0,
		  -5, -5, -5, -5, -5, -5, -5,   -5, -5, -5,
		  -5, -4, -3, -2, -1,  0,  1, 2.25,  3,  3,
		2.25,  1,  0, -1, -2, -3, -4,   -1,  0,  1, 2, 3 };
	double y1[33] = { 0,
		  -6, -5, -4, -3, -2, -1, 0,   1,  2,  3,
		   4,  4,  4,  4,  4,  4, 4, 3.5,  2,  1,
		-0.5, -1, -1, -1, -1, -1, -1, -2, -3, -4, -5, -6 };
	double x2[33] = {0, 0.94756043346272423, 0.73504466902509913,
		 0.4528453515175927,    0.46311499024105723,   0.30345454816993439,
		 0.075184942115601547, -0.090010071904764719, -0.19630977381424003,
		-0.36341509807865086,  -0.54216996409132612,  -0.68704678013309872,
		-0.67370169194623086,  -0.69336494336440502,  -0.67809065144478664,
		-0.61382610572366281,  -0.68656530656078996,  -0.57704879646736551,
		-0.63417502349009069,  -0.37153350651419026,  -0.091809666009009777,
		 0.054833807442559397,  0.1445593164362155,    0.055587230806920782,
		 0.18201798315035453,   0.048445620192953162,  0.081595930742961439,
		 0.20063623749033621,   0.28546520751183313,   0.39384438699721991,
		 0.62832258520372286,   0.78548335015622228,   1.0610707888793069};
	double y2[33] = {0, 0.49630791172076621, 0.53320347382055022,
		 0.62384637225470441,  0.47592708487655661,  0.50364353255684202,
		 0.55311720162084443,  0.55118713773007066,  0.50007736370068601,
		 0.40432332354648709,  0.49817059660482677,  0.49803436631629411,
		 0.33213829258059019,  0.14585700576425648, -0.022110500334692869,
		-0.1752555003289698,  -0.29448744336706828, -0.45639468287493545,
		-0.59177815505008013, -0.74980550818568981, -0.78095916436791279,
		-0.64447562732895125, -0.49526830813007033, -0.22443396573313243,
		-0.066378148077667398,-0.03498490725857361,  0.16196028200653381,
		 0.30633527000982519, -0.14894460651161745, -0.30808798640907431,
		-0.35920781945385832, -0.62766325578928184, -0.60389363590825562};
	double *x, *y;
	Configuration me = Configuration_create (32, 2);
		
	if (me == NULL) return NULL;

	if (choice == 2)
	{
		x = x2; y = y2;
		Thing_setName (me, "R_fit");
	}
	else
	{
		x = x1; y = y1;
		Thing_setName (me, "R");
	}
		
	for (i = 1; i <= 32; i++)
	{
		char s[20];
		sprintf (s, "%d", i);
		TableOfReal_setRowLabel (me, i, s);
		my data [i][1] = x[i];
		my data [i][2] = y[i];
	}
	return me;
}

Configuration Configuration_createCarrollWishExample (void)
{
	long i, nObjects = 9;
	double  x[10] = {0, -1, 0, 1, -1, 0, 1, -1,  0,  1};
	double  y[10] = {0,  1, 1, 1,  0, 0, 0, -1, -1, -1};
	char *label[] = { "", "A", "B", "C", "D", "E", "F", "G", "H", "I"};
	Configuration me = Configuration_create (nObjects, 2);
	
	if (me == NULL) return NULL;
	
	for (i = 1; i <= nObjects; i++)
	{
		my data[i][1] = x[i]; 
		my data[i][2] = y[i];
		TableOfReal_setRowLabel (me, i, label[i]);
	}
	return me;
}

/************ CONFIGURATIONS **************************************/

class_methods (Configurations, Ordered)
class_methods_end

Configurations Configurations_create (void)
{
	Configurations me = new (Configurations);
	if (! me || ! Ordered_init (me, classConfiguration, 10)) forget (me);
	return me;
}

/*
	19980404 djmw added Configuration_varimax
	
	BUGS:
		19980319 djmw Configuration_to_TableOfReal: corrected memory leak
		19981026 djmw Configuration_draw: hor. and. vert. dotted line through
			origin were drawn other way around.
*/

/* End of file Configuration.c */
