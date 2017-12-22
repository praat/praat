/* Configuration.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20010920 +Configuration_getDilationFactor
 djmw 20020315 GPL header
 djmw 20030513 applied change in numeric label generation
 djmw 20030801 Configuration_drawConcentrationEllipses extra argument
 djmw 20040303 Moved containsPrintableCharacter to NUM2.c
 djmw 20041026 Removed non-used code.
 djmw 20050314 Configuration_draw crashed when rowlabel==nullptr
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071009 wchar
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
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of points: ", numberOfRows);
	MelderInfo_writeLine (U"Number of dimensions: ", numberOfColumns);
	MelderInfo_writeLine (U"Metric: ", metric);
}

autoConfiguration Configuration_create (integer numberOfPoints, integer numberOfDimensions) {
	try {
		autoConfiguration me = Thing_new (Configuration);
		TableOfReal_init (me.get(), numberOfPoints, numberOfDimensions);
		my w = NUMvector<double> (1, numberOfDimensions);
		TableOfReal_setSequentialRowLabels (me.get(), 0, 0, nullptr, 1, 1);
		TableOfReal_setSequentialColumnLabels (me.get(), 0, 0, U"dimension ", 1, 1);

		my metric = 2;
		Configuration_setDefaultWeights (me.get());
		Configuration_randomize (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created.");
	}
}

void Configuration_setMetric (Configuration me, integer metric) {
	my metric = metric;
}

void Configuration_setDefaultWeights (Configuration me) {
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		my w [i] = 1;
	}
}

void Configuration_setSqWeights (Configuration me, const double weight[]) {
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		my w [i] = sqrt (weight [i]);
	}
}

void Configuration_normalize (Configuration me, double sumsq, bool columns) {
	TableOfReal_centreColumns (me);
	if (columns) {
		sumsq = sumsq <= 0.0 ? 1.0 : sqrt (sumsq);
		NUMnormalizeColumns (my data, my numberOfRows, my numberOfColumns, sumsq);
	} else {
		if (sumsq <= 0.0) {
			sumsq = my numberOfRows;
		}
		NUMnormalize (my data, my numberOfRows, my numberOfColumns, sqrt (sumsq));
	}
}

void Configuration_randomize (Configuration me) {
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my data [i] [j] = NUMrandomUniform (-1.0, 1.0);
		}
	}
}

void Configuration_rotate (Configuration me, integer dimension1, integer dimension2, double angle_degrees) {
	double f = NUMpi * (2.0 - angle_degrees / 180.0);
	double cosa = cos (f), sina = sin (f);

	if (dimension1 == dimension2 || angle_degrees == 0) {
		return;
	}

	if (dimension1 > dimension2) {
		integer dt = dimension1; dimension1 = dimension2; dimension2 = dt;
	}
	if (dimension1 < 1 || dimension2 > my numberOfColumns) {
		return;
	}
	for (integer i = 1; i <= my numberOfRows; i ++) {
		double x1 = my data [i] [dimension1];
		double x2 = my data [i] [dimension2];
		my data [i] [dimension1] =   cosa * x1 + sina * x2;
		my data [i] [dimension2] = - sina * x1 + cosa * x2;
	}
}

void Configuration_invertDimension (Configuration me, int dimension) {
	if (dimension < 1 || dimension > my numberOfColumns) {
		return;
	}

	for (integer i = 1; i <= my numberOfRows; i ++) {
		my data [i] [dimension] = - my data [i] [dimension];
	}
}


static double NUMsquaredVariance (double **a, integer nr, integer nc, bool rawPowers) {
	double v4 = 0.0;

	for (integer j = 1; j <= nc; j ++) {
		double sum4 = 0.0, mean = 0.0;
		for (integer i = 1; i <= nr; i ++) {
			double sq = a [i] [j] * a [i] [j];
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
static void NUMvarimax (double **xm, double **ym, integer nr, integer nc, bool normalizeRows, bool quartimax, integer maximumNumberOfIterations, double tolerance) {
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
		for (integer i = 1; i <= nr; i ++) {
			for (integer j = 1; j <= nc; j ++) {
				norm [i] += ym [i] [j] * ym [i] [j];
			}
			if (norm [i] <= 0.0) {
				continue;
			}
			norm [i] = sqrt (norm [i]);
			for (integer j = 1; j <= nc; j ++) {
				ym [i] [j] /= norm [i];
			}
		}
	}

	// Initial squared "variance".

	double varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);
	if (varianceSq == 0.0) {
		return;
	}

	// Treat columns pairwise.

	double varianceSq_old;
	integer numberOfIterations = 0;
	do {
		for (integer c1 = 1; c1 <= nc; c1 ++) {
			for (integer c2 = c1 + 1; c2 <= nc; c2 ++) {
				double um = 0.0, vm = 0.0;
				for (integer i = 1; i <= nr; i ++) {
					double x = ym [i] [c1];
					double y = ym [i] [c2];
					u [i] = x * x - y * y;
					um += u [i];
					v [i] = 2.0 * x * y;
					vm += v [i];
				}
				um /= nr; vm /= nr;
				if (quartimax || nr == 1) {
					um = vm = 0.0;
				}

				/*
					In the paper just before equation (1):
					a = 2n u'v, b = n(u'u-v'v), c = sqrt(a^2+b^2)
					w = -sign(a) sqrt((b+c) / 2c)
					Tricks: multiplication with n drops out!
						a's multiplication by 2 outside the loop.
				*/
				double a = 0.0, b = 0.0;
				for (integer i = 1; i <= nr; i ++) {
					double ui = u [i] - um;
					double vi = v [i] - vm;
					a += ui * vi;
					b += ui * ui - vi * vi;
				}
				double c = sqrt (4.0 * a * a + b * b);
				double w = sqrt ( (c + b) / (2.0 * c));
				if (a > 0.0) {
					w = -w;
				}
				double cost = sqrt (0.5 + 0.5 * w);
				double sint = sqrt (0.5 - 0.5 * w);
				double t22 = cost;
				double t11 = cost;
				double t12 = sint;
				double t21 = -sint;

				// Prevent permutations: when w < 0, i.e., a > 0, swap columns of T:/

				if (w < 0.0) {
					t11 = sint; t12 = t21 = cost; t22 = -sint;
				}

				// Rotate in the plane spanned by c1 and c2.

				for (integer i = 1; i <= nr; i ++) {
					double *xt = ym [i], xtc1 = xt [c1];
					xt [c1] = xtc1 * t11 + xt [c2] * t21;
					xt [c2] = xtc1 * t12 + xt [c2] * t22;
				}
			}
		}

		numberOfIterations++;
		varianceSq_old = varianceSq;
		varianceSq = NUMsquaredVariance (ym, nr, nc, quartimax);

	} while (fabs (varianceSq_old - varianceSq) / varianceSq_old > tolerance &&
	         numberOfIterations < maximumNumberOfIterations);

	if (normalizeRows) {
		for (integer i = 1; i <= nr; i ++) {
			for (integer j = 1; j <= nc; j ++) {
				ym [i] [j] *= norm [i];
			}
		}
	}
}

autoConfiguration Configuration_varimax (Configuration me, bool normalizeRows, bool quartimax, integer maximumNumberOfIterations, double tolerance) {
	try {
		autoConfiguration thee = Data_copy (me);
		NUMvarimax (my data, thy data, my numberOfRows, my numberOfColumns, normalizeRows, quartimax, maximumNumberOfIterations, tolerance);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": varimax rotation not performed.");
	}
}

autoConfiguration Configuration_congruenceRotation (Configuration me, Configuration thee, integer maximumNumberOfIterations, double tolerance) {
	try {
		autoAffineTransform at = Configurations_to_AffineTransform_congruence (me, thee, maximumNumberOfIterations, tolerance);
		autoConfiguration him = Configuration_AffineTransform_to_Configuration (me, at.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": congruence rotation not performed.");
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
		Melder_throw (me, U": not rotated to principal directions.");
	}
}

void Configuration_draw (Configuration me, Graphics g, int xCoordinate, int yCoordinate, double xmin, double xmax, double ymin, double ymax, int labelSize, bool useRowLabels, const char32 *label, bool garnish)
{
	integer nPoints = my numberOfRows, numberOfDimensions = my numberOfColumns;

	if (numberOfDimensions > 1 && (xCoordinate > numberOfDimensions || yCoordinate > numberOfDimensions)) {
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

	for (integer i = 1; i <= nPoints; i ++) {
		x [i] = my data [i] [xCoordinate] * my w [xCoordinate];
		y [i] = numberOfDimensions > 1 ? my data [i] [yCoordinate] * my w [yCoordinate] : 0.0;
	}
	if (xmax <= xmin) {
		NUMvector_extrema (x.peek(), 1, nPoints, &xmin, &xmax);
	}
	if (xmax <= xmin) {
		xmax += 1.0;
		xmin -= 1.0;
	}
	if (ymax <= ymin) {
		NUMvector_extrema (y.peek(), 1, nPoints, &ymin, &ymax);
	}
	if (ymax <= ymin) {
		ymax += 1.0;
		ymin -= 1.0;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFontSize (g, labelSize);
	for (integer i = 1; i <= my numberOfRows; i ++) {
		if (x [i] >= xmin && x [i] <= xmax && y [i] >= ymin && y [i] <= ymax) {
			const char32 *plotLabel = ( useRowLabels ? my rowLabels [i] : label );
			if (NUMstring_containsPrintableCharacter (plotLabel)) {
				Graphics_text (g, x [i], y [i], plotLabel);
			} else {
				noLabel ++;
			}
		}
	}
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		if (numberOfDimensions > 1) {
			Graphics_marksLeft (g, 2, true, true, false);
			if (my columnLabels[xCoordinate]) {
				Graphics_textBottom (g, true, my columnLabels[xCoordinate]);
			}
			if (my columnLabels[yCoordinate]) {
				Graphics_textLeft (g, true, my columnLabels[yCoordinate]);
			}
		}
	}

	if (noLabel > 0) {
		Melder_warning (U"Configuration_draw: ", noLabel, U" from ", my numberOfRows, U" labels are not visible because they are empty or they contain only spaces or they contain only non-printable characters");
	}
}

void Configuration_drawConcentrationEllipses (Configuration me, Graphics g, double scale, bool confidence, const char32 *label, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, bool garnish) {
	autoSSCPList sscps = TableOfReal_to_SSCPList_byLabel (me);
	SSCPList_drawConcentrationEllipses (sscps.get(), g, scale, confidence, label, d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
}

autoConfiguration TableOfReal_to_Configuration (TableOfReal me) {
	try {
		autoConfiguration thee = Configuration_create (my numberOfRows, my numberOfColumns);

		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted.");
	}
}

autoConfiguration TableOfReal_to_Configuration_pca (TableOfReal me, integer numberOfDimensions) {
	try {
		if (numberOfDimensions < 1 || numberOfDimensions > my numberOfColumns) {
			numberOfDimensions = my numberOfColumns;
		}

		autoPCA pca = TableOfReal_to_PCA_byRows (me);
		autoConfiguration thee = PCA_TableOfReal_to_Configuration (pca.get(), me, numberOfDimensions);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": pca not performed.");
	}
}

/********************** Examples *********************************************/

autoConfiguration Configuration_createLetterRExample (int choice) {
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
		0.62832258520372286,   0.78548335015622228,   1.0610707888793069 };
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
		-0.35920781945385832, -0.62766325578928184, -0.60389363590825562 };
	try {
		double *x, *y;
		autoConfiguration me = Configuration_create (32, 2);

		if (choice == 2) {
			x = x2; y = y2;
			Thing_setName (me.get(), U"R_fit");
		} else {
			x = x1; y = y1;
			Thing_setName (me.get(), U"R");
		}

		for (integer i = 1; i <= 32; i ++) {
			char32 s [20];
			Melder_sprint (s, 20, i);
			TableOfReal_setRowLabel (me.get(), i, s);
			my data [i] [1] = x [i];
			my data [i] [2] = y [i];
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Letter R Configuration not created.");
	}
}

autoConfiguration Configuration_createCarrollWishExample () {
	double  x [10] = {0.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, -1.0,  0.0,  1.0};
	double  y [10] = {0.0,  1.0, 1.0, 1.0,  0.0, 0.0, 0.0, -1.0, -1.0, -1.0};
	char32 const *label[] = { U"", U"A", U"B", U"C", U"D", U"E", U"F", U"G", U"H", U"I"};
	try {
		integer nObjects = 9;
		autoConfiguration me = Configuration_create (nObjects, 2);
		for (integer i = 1; i <= nObjects; i ++) {
			my data [i] [1] = x [i];
			my data [i] [2] = y [i];
			TableOfReal_setRowLabel (me.get(), i, label[i]);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Carroll Wish Configuration not created.");
	}
}

/************ CONFIGURATIONS **************************************/

Thing_implement (ConfigurationList, TableOfRealList, 0);

/* End of file Configuration.cpp */
