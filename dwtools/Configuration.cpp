/* Configuration.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20010920 +Configuration_getDilationFactor
 djmw 20020315 GPL header
 djmw 20030513 applied change in numeric label generation
 djmw 20030801 Configuration_drawConcentrationEllipses extra argument
 djmw 20040303 Moved containsPrintableCharacter to NUM2.c
 djmw 20041026 Removed non-used code.
 djmw 20050314 Configuration_draw crashed when rowlabel==NULL
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071009 wchar_t
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20100302 Extra test in Configuration_rotate
 djmw 20110304 Thing_new
 */

#include <ctype.h>
#include "SVD.h"
#include "PCA.h"
#include "Configuration.h"
#include "Configuration_AffineTransform.h"
#include "TableOfReal_extensions.h"
#include "SSCP.h"

#include "oo_DESTROY.h"
#include "Configuration_def.h"
#include "oo_COPY.h"
#include "Configuration_def.h"
#include "oo_EQUAL.h"
#include "Configuration_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Configuration_def.h"
#include "oo_WRITE_TEXT.h"
#include "Configuration_def.h"
#include "oo_WRITE_BINARY.h"
#include "Configuration_def.h"
#include "oo_READ_TEXT.h"
#include "Configuration_def.h"
#include "oo_READ_BINARY.h"
#include "Configuration_def.h"
#include "oo_DESCRIPTION.h"
#include "Configuration_def.h"

Thing_implement (Configuration, TableOfReal, 0);

void structConfiguration :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of points: ", Melder_integer (numberOfRows));
	MelderInfo_writeLine (L"Number of dimensions: ", Melder_integer (numberOfColumns));
	MelderInfo_writeLine (L"Metric: ", Melder_integer (metric));
}

Configuration Configuration_create (long numberOfPoints,
                                    long numberOfDimensions) {
	try {
		autoConfiguration me = Thing_new (Configuration);
		TableOfReal_init (me.peek(), numberOfPoints, numberOfDimensions);
		my w = NUMvector<double> (1, numberOfDimensions);
		TableOfReal_setSequentialRowLabels (me.peek(), 0, 0, NULL, 1, 1);
		TableOfReal_setSequentialColumnLabels (me.peek(), 0, 0, L"dimension ", 1, 1);

		my metric = 2;
		Configuration_setDefaultWeights (me.peek());
		Configuration_randomize (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Configuration not created.");
	}
}

void Configuration_setMetric (Configuration me, long metric) {
	my metric = metric;
}

void Configuration_setDefaultWeights (Configuration me) {
	for (long i = 1; i <= my numberOfColumns; i++) {
		my w[i] = 1;
	}
}

void Configuration_setSqWeights (Configuration me, const double weight[]) {
	for (long i = 1; i <= my numberOfColumns; i++) {
		my w[i] = sqrt (weight[i]);
	}
}

void Configuration_normalize (Configuration me, double sumsq, int columns) {
	TableOfReal_centreColumns (me);
	if (columns) {
		sumsq = sumsq <= 0 ? 1.0 : sqrt (sumsq);
		NUMnormalizeColumns (my data, my numberOfRows, my numberOfColumns, sumsq);
	} else {
		if (sumsq <= 0) {
			sumsq = my numberOfRows;
		}
		NUMnormalize (my data, my numberOfRows, my numberOfColumns,
		              sqrt (sumsq));
	}
}

void Configuration_randomize (Configuration me) {
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			my data[i][j] = NUMrandomUniform (-1, 1);
		}
	}
}

void Configuration_rotate (Configuration me, long dimension1, long dimension2, double angle_degrees) {
	double f = NUMpi * (2 - angle_degrees / 180);
	double cosa = cos (f), sina = sin (f);

	if (dimension1 == dimension2 || angle_degrees == 0) {
		return;
	}

	if (dimension1 > dimension2) {
		long dt = dimension1; dimension1 = dimension2; dimension2 = dt;
	}
	if (dimension1 < 1 || dimension2 > my numberOfColumns) {
		return;
	}
	for (long i = 1; i <= my numberOfRows; i++) {
		double x1 = my data[i][dimension1], x2 = my data[i][dimension2];
		my data[i][dimension1] =   cosa * x1 + sina * x2;
		my data[i][dimension2] = - sina * x1 + cosa * x2;
	}
}

void Configuration_invertDimension (Configuration me, int dimension) {
	if (dimension < 1 || dimension > my numberOfColumns) {
		return;
	}

	for (long i = 1; i <= my numberOfRows; i++) {
		my data[i][dimension] = - my data[i][dimension];
	}
}


static double NUMsquaredVariance (double **a, long nr, long nc, int rawPowers) {
	double v4 = 0;

	for (long j = 1; j <= nc; j++) {
		double sum4 = 0, mean = 0;
		for (long i = 1; i <= nr; i++) {
			double sq = a[i][j] * a[i][j];
			sum4 += sq * sq;
			mean += sq;
		}
		v4 += sum4;
		if (! rawPowers) {
			v4 -= mean * mean / nr;
		}
	}
	return v4;
}

/*
	Varimax rotation, implementation according to:
		Jos Ten Berge (1995), "Suppressing permutations or rigid
		planar rotations: a remedy against nonoptimal varimax rotations",
		Psychometrika 60, 437-446.
*/
static void NUMvarimax (double **xm, double **ym, long nr, long nc, int normalizeRows, int quartimax,
                        long maximumNumberOfIterations, double tolerance) {
	Melder_assert (nr > 0 && nc > 0);

	NUMmatrix_copyElements (xm, ym, 1, nr, 1, nc);

	if (nc == 1) {
		return;
	}
	if (nc == 2) {
		maximumNumberOfIterations = 1;
	}

	autoNUMvector<double> u (1, nr);
	autoNUMvector<double> v (1, nr);
	autoNUMvector<double> norm;

	// Normalize sum of squares of each row to one.
	// After rotation we have to rescale.

	if (normalizeRows) {
		norm.reset (1, nr);
		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				norm[i] += ym[i][j] * ym[i][j];
			}
			if (norm[i] <= 0) {
				continue;
			}
			norm[i] = sqrt (norm[i]);
			for (long j = 1; j <= nc; j++) {
				ym[i][j] /= norm[i];
			}
		}
	}

	// Initial squared "variance".

	double varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);
	if (varianceSq == 0) {
		return;
	}

	// Treat columns pairwise.

	double varianceSq_old;
	long numberOfIterations = 0;
	do {
		for (long c1 = 1; c1 <= nc; c1++) {
			for (long c2 = c1 + 1; c2 <= nc; c2++) {
				double um = 0, vm = 0;
				for (long i = 1; i <= nr; i++) {
					double x = ym[i][c1], y = ym[i][c2];
					u[i] = x * x - y * y;
					um += u[i];
					v[i] = 2 * x * y;
					vm += v[i];
				}
				um /= nr; vm /= nr;
				if (quartimax || nr == 1) {
					um = vm = 0;
				}

				/*
					In the paper just before equation (1):
					a = 2n u'v, b = n(u'u-v'v), c = sqrt(a^2+b^2)
					w = -sign(a) sqrt((b+c) / 2c)
					Tricks: multiplication with n drops out!
						a's multiplication by 2 outside the loop.
				*/
				double a = 0, b = 0;
				for (long i = 1; i <= nr; i++) {
					double ui = u[i] - um, vi = v[i] - vm;
					a += ui * vi;
					b += ui * ui - vi * vi;
				}
				double c = sqrt (4 * a * a + b * b);
				double w = sqrt ( (c + b) / (2 * c));
				if (a > 0) {
					w = -w;
				}
				double cost = sqrt (0.5 + 0.5 * w);
				double sint = sqrt (0.5 - 0.5 * w);
				double t22 = cost;
				double t11 = cost;
				double t12 = sint;
				double t21 = -sint;

				// Prevent permutations: when w < 0, i.e., a > 0, swap columns of T:/

				if (w < 0) {
					t11 = sint; t12 = t21 = cost; t22 = -sint;
				}

				// Rotate in the plane spanned by c1 and c2.

				for (long i = 1; i <= nr; i++) {
					double *xt = ym[i], xtc1 = xt[c1];
					xt[c1] = xtc1 * t11 + xt[c2] * t21;
					xt[c2] = xtc1 * t12 + xt[c2] * t22;
				}
			}
		}

		numberOfIterations++;
		varianceSq_old = varianceSq;
		varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);

	} while (fabs (varianceSq_old - varianceSq) / varianceSq_old > tolerance &&
	         numberOfIterations < maximumNumberOfIterations);

	if (normalizeRows) {
		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				ym[i][j] *= norm[i];
			}
		}
	}
}

Configuration Configuration_varimax (Configuration me, int normalizeRows,
                                     int quartimax, long maximumNumberOfIterations, double tolerance) {
	try {
		autoConfiguration thee = Data_copy (me);
		NUMvarimax (my data, thy data, my numberOfRows, my numberOfColumns, normalizeRows, quartimax,
		            maximumNumberOfIterations, tolerance);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": varimax rotation not performed.");
	}
}

Configuration Configuration_congruenceRotation (Configuration me, Configuration thee, long maximumNumberOfIterations, double tolerance) {
	try {
		autoAffineTransform at = Configurations_to_AffineTransform_congruence (me, thee, maximumNumberOfIterations, tolerance);
		autoConfiguration him = Configuration_and_AffineTransform_to_Configuration (me, at.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": congruence rotation not performed.");
	}
}

/* Replace by TableOfReal_to_Configuration_pca ??? */

void Configuration_rotateToPrincipalDirections (Configuration me) {
	try {
		autoNUMmatrix<double> m (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfColumns), 1, 1);

		NUMdmatrix_into_principalComponents (my data, my numberOfRows, my numberOfColumns, my numberOfColumns, m.peek());
		NUMvector_free (my data, 1);
		my data = m.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not rotated to principal directions.");
	}
}

void Configuration_draw (Configuration me, Graphics g, int xCoordinate, int yCoordinate, double xmin, double xmax,
                         double ymin, double ymax, int labelSize, int useRowLabels, const wchar_t *label, int garnish) {
	long nPoints = my numberOfRows, numberOfDimensions = my numberOfColumns;

	if (numberOfDimensions > 1 && (xCoordinate > numberOfDimensions ||
	                               yCoordinate > numberOfDimensions)) {
		return;
	}
	if (numberOfDimensions == 1) {
		xCoordinate = 1;
	}
	int fontSize = Graphics_inqFontSize (g), noLabel = 0;
	if (labelSize == 0) {
		labelSize = fontSize;
	}
	autoNUMvector<double> x (1, nPoints);
	autoNUMvector<double> y (1, nPoints);

	for (long i = 1; i <= nPoints; i++) {
		x[i] = my data[i][xCoordinate] * my w[xCoordinate];
		y[i] = numberOfDimensions > 1 ? my data[i][yCoordinate] * my w[yCoordinate] : 0;
	}
	if (xmax <= xmin) {
		NUMvector_extrema (x.peek(), 1, nPoints, &xmin, &xmax);
	}
	if (xmax <= xmin) {
		xmax += 1;
		xmin -= 1;
	}
	if (ymax <= ymin) {
		NUMvector_extrema (y.peek(), 1, nPoints, &ymin, &ymax);
	}
	if (ymax <= ymin) {
		ymax += 1;
		ymin -= 1;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	for (long i = 1; i <= my numberOfRows; i++) {
		if (x[i] >= xmin && x[i] <= xmax && y[i] >= ymin && y[i] <= ymax) {
			wchar_t const *plotLabel = useRowLabels ? my rowLabels[i] : label;
			if (NUMstring_containsPrintableCharacter (plotLabel)) {
				Graphics_text (g, x[i], y[i], plotLabel);
			} else {
				noLabel++;
			}
		}
	}
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		if (numberOfDimensions > 1) {
			Graphics_marksLeft (g, 2, 1, 1, 0);
			if (my columnLabels[xCoordinate]) {
				Graphics_textBottom (g, 1, my columnLabels[xCoordinate]);
			}
			if (my columnLabels[yCoordinate]) {
				Graphics_textLeft (g, 1, my columnLabels[yCoordinate]);
			}
		}
	}

	if (noLabel > 0) Melder_warning (L"Configuration_draw: ", Melder_integer (noLabel), L" from ", Melder_integer (my numberOfRows),
		                                 L" labels are not visible because they are empty or they contain only spaces or they contain only non-printable characters");
}

void Configuration_drawConcentrationEllipses (Configuration me, Graphics g,
        double scale, int confidence, const wchar_t *label, long d1, long d2, double xmin, double xmax,
        double ymin, double ymax, int fontSize, int garnish) {
	autoSSCPs sscps = TableOfReal_to_SSCPs_byLabel (me);
	SSCPs_drawConcentrationEllipses (sscps.peek(), g, scale, confidence, label,
	                                 d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
}

Configuration TableOfReal_to_Configuration (I) {
	iam (TableOfReal);
	try {
		autoConfiguration thee = Configuration_create (my numberOfRows, my numberOfColumns);

		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted.");
	}
}

Configuration TableOfReal_to_Configuration_pca (TableOfReal me, long numberOfDimensions) {
	try {
		if (numberOfDimensions < 1 || numberOfDimensions > my numberOfColumns) {
			numberOfDimensions = my numberOfColumns;
		}

		autoPCA pca = TableOfReal_to_PCA (me);
		autoConfiguration thee = PCA_and_TableOfReal_to_Configuration (pca.peek(), me, numberOfDimensions);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": pca not performed.");
	}
}

/********************** Examples *********************************************/

Configuration Configuration_createLetterRExample (int choice) {
	double x1[33] = { 0,
	                  -5, -5, -5, -5, -5, -5, -5,   -5, -5, -5,
	                  -5, -4, -3, -2, -1,  0,  1, 2.25,  3,  3,
	                  2.25,  1,  0, -1, -2, -3, -4,   -1,  0,  1, 2, 3
	                };
	double y1[33] = { 0,
	                  -6, -5, -4, -3, -2, -1, 0,   1,  2,  3,
	                  4,  4,  4,  4,  4,  4, 4, 3.5,  2,  1,
	                  -0.5, -1, -1, -1, -1, -1, -1, -2, -3, -4, -5, -6
	                };
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
	                 0.62832258520372286,   0.78548335015622228,   1.0610707888793069
	                };
	double y2[33] = {0, 0.49630791172076621, 0.53320347382055022,
	                 0.62384637225470441,  0.47592708487655661,  0.50364353255684202,
	                 0.55311720162084443,  0.55118713773007066,  0.50007736370068601,
	                 0.40432332354648709,  0.49817059660482677,  0.49803436631629411,
	                 0.33213829258059019,  0.14585700576425648, -0.022110500334692869,
	                 -0.1752555003289698,  -0.29448744336706828, -0.45639468287493545,
	                 -0.59177815505008013, -0.74980550818568981, -0.78095916436791279,
	                 -0.64447562732895125, -0.49526830813007033, -0.22443396573313243,
	                 -0.066378148077667398, -0.03498490725857361,  0.16196028200653381,
	                 0.30633527000982519, -0.14894460651161745, -0.30808798640907431,
	                 -0.35920781945385832, -0.62766325578928184, -0.60389363590825562
	                };
	try {
		double *x, *y;
		autoConfiguration me = Configuration_create (32, 2);

		if (choice == 2) {
			x = x2; y = y2;
			Thing_setName (me.peek(), L"R_fit");
		} else {
			x = x1; y = y1;
			Thing_setName (me.peek(), L"R");
		}

		for (long i = 1; i <= 32; i++) {
			wchar_t s[20];
			swprintf (s, 20, L"%ld", i);
			TableOfReal_setRowLabel (me.peek(), i, s);
			my data [i][1] = x[i];
			my data [i][2] = y[i];
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Letter R Configuration not created.");
	}
}

Configuration Configuration_createCarrollWishExample () {
	double  x[10] = {0, -1, 0, 1, -1, 0, 1, -1,  0,  1};
	double  y[10] = {0,  1, 1, 1,  0, 0, 0, -1, -1, -1};
	wchar_t const *label[] = { L"", L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I"};
	try {
		long nObjects = 9;
		autoConfiguration me = Configuration_create (nObjects, 2);
		for (long i = 1; i <= nObjects; i++) {
			my data[i][1] = x[i];
			my data[i][2] = y[i];
			TableOfReal_setRowLabel (me.peek(), i, label[i]);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Carroll Wish Configuration not created.");
	}
}

/************ CONFIGURATIONS **************************************/

Thing_implement (Configurations, Ordered, 0);

Configurations Configurations_create () {
	try {
		autoConfigurations me = Thing_new (Configurations);
		Ordered_init (me.peek(), classConfiguration, 10);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Configurations not created.");
	}
}

/* End of file Configuration.cpp */
