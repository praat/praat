/* praat_MDS_init.cpp
 *
 * Copyright (C) 1992-2012 David Weenink
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
 djmw 20020408 GPL
 djmw 20020408 Added MDS-tutorial
 djmw 20020603 Changes due to TableOfReal dynamic menu changes.
 djmw 20040415 Forms texts.
 djmw 20040513 More forms text changes
 djmw 20041027 Orhogonal transform parameter for Configurations_to_Procrustes
 djmw 20050406 classProcrustus -> classProcrustes.
 djmw 20050426 Removed "Procrustus.h"
 djmw 20050630 Better name of Procrustes object after Configurations_to_Procrustes.
 djmw 20061218 Introduction of Melder_information<12...9>
 djmw 20070902 Melder_new<1...>
 djmw 20071011 REQUIRE requires L"".
 djmw 20090818 Thing_recognizeClassesByName: added classAffineTransform, classScalarProduct, classWeight
*/

#include <math.h>
#include "NUM2.h"
#include "praat.h"
#include "MDS.h"
#include "ContingencyTable.h"
#include "TableOfReal_extensions.h"
#include "Configuration_and_Procrustes.h"
#include "Configuration_AffineTransform.h"
#include "Confusion.h"
#include "Formula.h"

void praat_TableOfReal_init (ClassInfo klas);
void praat_TableOfReal_init2 (ClassInfo klas);

static const wchar_t *QUERY_BUTTON   = L"Query -";
static const wchar_t *DRAW_BUTTON    = L"Draw -";
static const wchar_t *ANALYSE_BUTTON = L"Analyse -";
static const wchar_t *CONFIGURATION_BUTTON = L"To Configuration -";

/* Tests */

/*
	Sort row 1 ascending and store in row 3
	Sort row 1 and move row 2 along and store in rows 4 and 5 respectively
	Make an index for row 1 and store in row 6
*/
static void TabelOfReal_testSorting (I, long rowtoindex) {
	iam (TableOfReal);
	try {
		long  nc = my numberOfColumns;

		autoNUMvector<long> index (1, nc);
		if (my numberOfRows < 6) {
			Melder_throw ("TabelOfReal_sort2: we want at least 6 rows!!");
		}
		if (rowtoindex < 1 || rowtoindex > 2) {
			Melder_throw ("TabelOfReal_sort2: rowtoindex <= 2");
		}

		// Copy 1->3 and sort 3 inplace
		NUMvector_copyElements (my data[1], my data[3], 1, nc);
		NUMsort_d (nc, my data[3]);

		// Copy 1->4 and 2->5, sort 4+5 in parallel
		NUMvector_copyElements (my data[1], my data[4], 1, nc);
		NUMvector_copyElements (my data[2], my data[5], 1, nc);
		NUMsort2 (nc, my data[4], my data[5]);

		NUMindexx (my data[rowtoindex], nc, index.peek());
		for (long i = 1; i <= nc; i++) {
			my data[6][i] = index[i];
		}
	} catch (MelderError) {
		Melder_throw (me, ": sorting test not ok.");
	}
}

#undef iam
#define iam iam_LOOP

FORM (TabelOfReal_testSorting, L"TabelOfReal: Sort and index", L"")
	NATURAL (L"Row to index", L"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TabelOfReal_testSorting (me, GET_INTEGER (L"Row to index"));
	}
END

/************************* examples ***************************************/

FORM (Dissimilarity_createLetterRExample, L"Create letter R example", L"Create letter R example...")
	LABEL (L"", L"For the monotone transformation on the distances")
	REAL (L"Noise range", L"32.5")
	OK
DO
	praat_new (Dissimilarity_createLetterRExample (GET_REAL (L"Noise range")), 0);
END

FORM (INDSCAL_createCarrollWishExample,
      L"Create INDSCAL Carroll & Wish example...",
      L"Create INDSCAL Carroll & Wish example...")
	REAL (L"Noise standard deviation", L"0.0")
OK
	DO
	praat_new (INDSCAL_createCarrollWishExample (GET_REAL (L"Noise standard deviation")), 0);
END

FORM (Configuration_create, L"Create Configuration", L"Create Configuration...")
	WORD (L"Name", L"uniform")
	NATURAL (L"Number of points", L"10")
	NATURAL (L"Number of dimensions", L"2")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"randomUniform(-1.5, 1.5)")
	OK
DO
	autoConfiguration me = Configuration_create (GET_INTEGER (L"Number of points"), GET_INTEGER (L"Number of dimensions"));
	TableOfReal_formula (me.peek(), GET_STRING (L"formula"), interpreter, 0);
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (drawSplines, L"Draw splines", L"spline")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"1.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"20.0")
	RADIO (L"Spline type", 1)
	RADIOBUTTON (L"M-spline")
	RADIOBUTTON (L"I-spline")
	INTEGER (L"Order", L"3")
	SENTENCE (L"Interior knots", L"0.3 0.5 0.6")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	double xmin = GET_REAL (L"left Horizontal range"), xmax = GET_REAL (L"right Horizontal range");
	double ymin = GET_REAL (L"left Vertical range"), ymax = GET_REAL (L"right Vertical range");
	if (xmax <= xmin or ymax <= ymin) {
		Melder_throw ("Required: xmin < xmax and ymin < ymax.");
	}
	autoPraatPicture picture;
	drawSplines (GRAPHICS, xmin, xmax, ymin, ymax, GET_INTEGER (L"Spline type"),
		GET_INTEGER (L"Order"), GET_STRING (L"Interior knots"), GET_INTEGER (L"Garnish"));
END

DIRECT (drawMDSClassRelations)
	autoPraatPicture picture;
	drawMDSClassRelations (GRAPHICS);
END


/***************** AffineTransform ***************************************/


DIRECT (AffineTransform_help)
	Melder_help (L"AffineTransform");
END

DIRECT (AffineTransform_invert)
	LOOP {
		iam (AffineTransform);
		praat_new ( (AffineTransform) AffineTransform_invert (me), NAME, L"_inv");
	}
END

FORM (AffineTransform_getTransformationElement, L"AffineTransform: Get transformation element", L"Procrustes")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	OK
DO
	long row = GET_INTEGER (L"Row number");
	long column = GET_INTEGER (L"Column number");
	LOOP {
		iam (AffineTransform);
		if (row > my n) {
			Melder_throw ("Row number must not exceed number of rows.");
		}
		if (column > my n) {
			Melder_throw ("Column number must not exceed number of columns.");
		}
		Melder_information (Melder_double (my r [row] [column]));
	}
END

FORM (AffineTransform_getTranslationElement, L"AffineTransform: Get translation element", L"Procrustes")
	NATURAL (L"Index", L"1")
	OK
DO
	long number = GET_INTEGER (L"Index");
	LOOP {
		iam (AffineTransform);
		if (number > my n) {
			Melder_throw ("Index must not exceed number of elements.");
		}
		Melder_information (Melder_double (my t [number]));
	}
END

DIRECT (AffineTransform_extractMatrix)
	LOOP {
		iam (AffineTransform);
		praat_new (AffineTransform_extractMatrix (me), my name);
	}
END

DIRECT (AffineTransform_extractTranslationVector)
	LOOP {
		iam (AffineTransform);
		praat_new (AffineTransform_extractTranslationVector (me), my name);
	}
END

/***************** Configuration ***************************************/

DIRECT (Configuration_help)
	Melder_help (L"Configuration");
END

static void Configuration_draw_addCommonFields (void *dia) {
	NATURAL (L"Horizontal dimension", L"1")
	NATURAL (L"Vertical dimension", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
}

FORM (Configuration_draw, L"Configuration: Draw", L"Configuration: Draw...")
	Configuration_draw_addCommonFields (dia);
	NATURAL (L"Label size", L"12")
	BOOLEAN (L"Use row labels", 0)
	WORD (L"Label", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_draw (me, GRAPHICS, GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
			GET_REAL (L"right Vertical range"), GET_INTEGER (L"Label size"),
			GET_INTEGER (L"Use row labels"), GET_STRING (L"Label"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Configuration_drawSigmaEllipses, L"Configuration: Draw sigma ellipses", L"Configuration: Draw sigma ellipses...")
	POSITIVE (L"Number of sigmas", L"1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, GET_REAL (L"Number of sigmas"), 0, 0,
			GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Configuration_drawOneSigmaEllipse, L"Configuration: Draw one sigma ellipse", L"Configuration: Draw sigma ellipses...")
	SENTENCE (L"Label", L"")
	POSITIVE (L"Number of sigmas", L"1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, GET_REAL (L"Number of sigmas"), 0, GET_STRING (L"Label"),
			GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END


FORM (Configuration_drawConfidenceEllipses, L"Configuration: Draw confidence ellipses", 0)
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, GET_REAL (L"Confidence level"), 1, 0,
			GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

FORM (Configuration_drawOneConfidenceEllipse, L"Configuration: Draw one confidence ellipse", 0)
	SENTENCE (L"Label", L"")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS,
			GET_REAL (L"Confidence level"), 1, GET_STRING (L"Label"),
			GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
			GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
			GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish"));
	}
END

DIRECT (Configuration_randomize)
	LOOP {
		iam (Configuration);
		Configuration_randomize (me);
	}
END

FORM (Configuration_normalize, L"Configuration: Normalize", L"Configuration: Normalize...")
	REAL (L"Sum of squares", L"0.0")
	LABEL (L"", L"On (INDSCAL), Off (Kruskal)")
	BOOLEAN (L"Each dimension separately", 1)
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_normalize (me, GET_REAL (L"Sum of squares"), GET_INTEGER (L"Each dimension separately"));
	}
END

DIRECT (Configuration_centralize)
	LOOP {
		iam (Configuration);
		TableOfReal_centreColumns (me);
	}
END

FORM (Configuration_rotate, L"Configuration: Rotate", L"Configuration: Rotate...")
	NATURAL (L"Dimension 1", L"1")
	NATURAL (L"Dimension 2", L"2")
	REAL (L"Angle (degrees)", L"60.0")
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_rotate (me, GET_INTEGER (L"Dimension 1"), GET_INTEGER (L"Dimension 2"), GET_REAL (L"Angle"));
	}
END

DIRECT (Configuration_rotateToPrincipalDirections)
	LOOP {
		iam (Configuration);
		Configuration_rotateToPrincipalDirections (me);
	}
END

FORM (Configuration_invertDimension, L"Configuration: Invert dimension", L"Configuration: Invert dimension...")
	NATURAL (L"Dimension", L"1")
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_invertDimension (me, GET_INTEGER (L"Dimension"));
	}
END

DIRECT (Configuration_to_Distance)
	LOOP {
		iam (Configuration);
		praat_new (Configuration_to_Distance (me), my name);
	}
END

FORM (Configuration_varimax, L"Configuration: To Configuration (varimax)", L"Configuration: To Configuration (varimax)...")
	BOOLEAN (L"Normalize rows", 1)
	BOOLEAN (L"Quartimax", 0)
	NATURAL (L"Maximum number of iterations", L"50")
	POSITIVE (L"Tolerance", L"1e-6")
	OK
DO
	LOOP {
		iam (Configuration);
		praat_new (Configuration_varimax (me, GET_INTEGER (L"Normalize rows"), GET_INTEGER (L"Quartimax"),
			GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance")), my name, L"_varimax");
	}
END

DIRECT (Configurations_to_Similarity_cc)
	autoConfigurations set = (Configurations) praat_getSelectedObjects ();
	praat_new (Configurations_to_Similarity_cc (set.peek(), 0), L"congruence");
END

FORM (Configurations_to_Procrustes, L"Configuration & Configuration: To Procrustes", L"Configuration & Configuration: To Procrustes...")
	BOOLEAN (L"Orthogonal transform", 0)
	OK
DO
	Configuration c1 = 0, c2 = 0;
	LOOP {
		iam (Configuration);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (Configurations_to_Procrustes (c1, c2, GET_INTEGER (L"Orthogonal transform")),
		Thing_getName (c2), L"_to_", Thing_getName (c1));
END

FORM (Configurations_to_AffineTransform_congruence, L"Configurations: To AffineTransform (congruence)", L"Configurations: To AffineTransform (congruence)...")
	NATURAL (L"Maximum number of iterations", L"50")
	POSITIVE (L"Tolerance", L"1e-6")
	OK
DO
	Configuration c1 = 0, c2 = 0;
	LOOP {
		iam (Configuration);
		(c1 ? c2 : c1) = me;
	}
	Melder_assert (c1 && c2);
	praat_new (Configurations_to_AffineTransform_congruence (c1, c2, GET_INTEGER (L"Maximum number of iterations"),
		GET_REAL (L"Tolerance")), c1 -> name, L"_", c2 -> name);
END

DIRECT (Configuration_Weight_to_Similarity_cc)
	autoConfigurations thee = Configurations_create ();
	Collection_dontOwnItems (thee.peek());
	Weight w = 0;
	LOOP {
		iam (Data);
		if (CLASS == classConfiguration) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classWeight) {
			w = (Weight) me;
		}
	}
	Melder_assert (thy size > 0 && w);
	praat_new (Configurations_to_Similarity_cc (thee.peek(), w), L"congruence");
END

DIRECT (Configuration_and_AffineTransform_to_Configuration)
	Configuration me = FIRST (Configuration);
	AffineTransform at = FIRST_GENERIC (AffineTransform);
	praat_new (Configuration_and_AffineTransform_to_Configuration (me, at), my name, L"_", at -> name);
END

/*************** Confusion *********************************/

FORM (Confusion_to_Dissimilarity_pdf, L"Confusion: To Dissimilarity (pdf)", L"Confusion: To Dissimilarity (pdf)...")
	POSITIVE (L"Minimum confusion level", L"0.5")
	OK
DO
	LOOP {
		iam (Confusion);
		praat_new (Confusion_to_Dissimilarity_pdf (me, GET_REAL (L"Minimum confusion level")), my name, L"_pdf");
	}
END

FORM (Confusion_to_Similarity, L"Confusion: To Similarity", L"Confusion: To Similarity...")
	BOOLEAN (L"Normalize", 1)
	RADIO (L"Symmetrization", 1)
	RADIOBUTTON (L"No symmetrization")
	RADIOBUTTON (L"Average (s[i][j] = (c[i][j]+c[j][i])/2)")
	RADIOBUTTON (L"Houtgast (s[i][j]= sum (min(c[i][k],c[j][k])))")
	OK
DO
	LOOP {
		iam (Confusion);
		praat_new (Confusion_to_Similarity (me, GET_INTEGER (L"Normalize"), GET_INTEGER (L"Symmetrization")), my name);
	}
END

DIRECT (Confusions_sum)
	autoConfusions me = (Confusions) praat_getSelectedObjects ();
	praat_new (Confusions_sum (me.peek()), L"sum");
END

DIRECT (Confusion_to_ContingencyTable)
	LOOP {
		iam (Confusion);
		praat_new (Confusion_to_ContingencyTable (me), my name);
	}
END

/*************** ContingencyTable *********************************/


FORM (ContingencyTable_to_Configuration_ca, L"ContingencyTable: To Configuration (ca)", L"ContingencyTable: To Configuration (ca)...")
	NATURAL (L"Number of dimensions", L"2")
	RADIO (L"Scaling of final configuration", 3)
	RADIOBUTTON (L"Row points in centre of gravity of column points")
	RADIOBUTTON (L"Column points in centre of gravity of row points")
	RADIOBUTTON (L"Row points and column points symmetric")
	OK
DO
	LOOP {
		iam (ContingencyTable);
		praat_new (ContingencyTable_to_Configuration_ca (me, GET_INTEGER (L"Number of dimensions"),
			GET_INTEGER (L"Scaling of final configuration")), my name);
	}
END

DIRECT (ContingencyTable_chisqProbability)
	LOOP {
		iam (ContingencyTable);
		Melder_information (Melder_double (ContingencyTable_chisqProbability (me)));
	}
END

DIRECT (ContingencyTable_cramersStatistic)
	LOOP {
		iam (ContingencyTable);
		Melder_information (Melder_double (ContingencyTable_cramersStatistic (me)));
	}
END

DIRECT (ContingencyTable_contingencyCoefficient)
	LOOP {
		iam (ContingencyTable);
		Melder_information (Melder_double (ContingencyTable_contingencyCoefficient (me)));
	}
END

/************************* Correlation ***********************************/

FORM (Correlation_to_Configuration, L"Correlation: To Configuration", 0)
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	LOOP {
		iam (Correlation);
		praat_new (Correlation_to_Configuration (me, GET_INTEGER (L"Number of dimensions")), my name);
	}
END


/************************* Similarity ***************************************/

DIRECT (Similarity_help)
	Melder_help (L"Similarity");
END

FORM (Similarity_to_Dissimilarity, L"Similarity: To Dissimilarity", L"Similarity: To Dissimilarity...")
	REAL (L"Maximum dissimilarity", L"0.0 (=from data)")
	OK
DO
	LOOP {
		iam (Similarity);
		praat_new (Similarity_to_Dissimilarity (me, GET_REAL (L"Maximum dissimilarity")), my name);
	}
END

/**************** Dissimilarity ***************************************/

static void Dissimilarity_to_Configuration_addCommonFields (void *dia) {
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"50 (= each repetition)")
	NATURAL (L"Number of repetitions", L"1")
}

static void Dissimilarity_and_Configuration_getStress_addCommonFields (void *dia, void *radio) {
	RADIO (L"Stress measure", 1)
	RADIOBUTTON (L"Normalized")
	RADIOBUTTON (L"Kruskal's stress-1")
	RADIOBUTTON (L"Kruskal's stress-2")
	RADIOBUTTON (L"Raw")
}

static void Dissimilarity_Configuration_drawDiagram_addCommonFields (void *dia) {
	REAL (L"left Proximity range", L"0.0")
	REAL (L"right Proximity range", L"0.0")
	REAL (L"left Distance range", L"0.0")
	REAL (L"right Distance range", L"0.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Garnish", 1)
}

DIRECT (Dissimilarity_help)
	Melder_help (L"Dissimilarity");
END

DIRECT (Dissimilarity_getAdditiveConstant)
	LOOP {
		iam (Dissimilarity);
		double c;
		Dissimilarity_getAdditiveConstant (me, &c);
		Melder_information (Melder_double (c));
	}
END

FORM (Dissimilarity_Configuration_kruskal, L"Dissimilarity & Configuration: To Configuration (kruskal)", L"Dissimilarity & Configuration: To Configuration (kruskal)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1")
	RADIOBUTTON (L"Formula2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	praat_new (Dissimilarity_Configuration_kruskal (me, c, GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress calculation"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions")), my name, L"_kruskal");
END

FORM (Dissimilarity_Configuration_absolute_mds, L"Dissimilarity & Configuration: To Configuration (absolute mds)", L"Dissimilarity & Configuration: To Configuration (absolute mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_absolute_mds (me, c, 0,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_absolute");
END

FORM (Dissimilarity_Configuration_ratio_mds, L"Dissimilarity & Configuration: To Configuration (ratio mds)", L"Dissimilarity & Configuration: To Configuration (ratio mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_ratio_mds (me, c, 0,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_ratio");
END

FORM (Dissimilarity_Configuration_interval_mds, L"Dissimilarity & Configuration: To Configuration (interval mds)", L"Dissimilarity & Configuration: To Configuration (interval mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_interval_mds (me, c, 0,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_interval");
END

FORM (Dissimilarity_Configuration_monotone_mds, L"Dissimilarity & Configuration: To Configuration (monotone mds)", L"Dissimilarity & Configuration: To Configuration (monotone mds)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_monotone_mds (me, c, 0,
		GET_INTEGER (L"Handling of ties"), GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_monotone");
END

FORM (Dissimilarity_Configuration_ispline_mds, L"Dissimilarity & Configuration: To Configuration (i-spline mds)", L"Dissimilarity & Configuration: To Configuration (i-spline mds)...")
	LABEL (L"", L"Spline smoothing")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_ispline_mds (me, c, 0,
		GET_INTEGER (L"Number of interior knots"), GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_ispline");
END

FORM (Dissimilarity_Configuration_Weight_absolute_mds, L"Dissimilarity & Configuration & Weight: To Configuration (absolute mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_absolute_mds (me, c, w,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_w_absolute");
END

FORM (Dissimilarity_Configuration_Weight_ratio_mds, L"Dissimilarity & Configuration & Weight: To Configuration (ratio mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_ratio_mds (me, c, w,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_w_ratio");
END

FORM (Dissimilarity_Configuration_Weight_interval_mds, L"Dissimilarity & Configuration & Weight: To Configuration (interval mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_interval_mds (me, c, w,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_w_interval");
END

FORM (Dissimilarity_Configuration_Weight_monotone_mds,
      L"Dissimilarity & Configuration & Weight: To Configuration (monotone mds)",
      L"Dissimilarity & Configuration & Weight: To Configuration...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_monotone_mds (me, c, w,
		GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_sw_monotone");
END

FORM (Dissimilarity_Configuration_Weight_ispline_mds,
      L"Dissimilarity & Configuration & Weight: To Configuration (i-spline mds)",
      L"Dissimilarity & Configuration & Weight: To Configuration...")
	LABEL (L"", L"Spline smoothing")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Configuration_Weight_ispline_mds (me, c, w,
		GET_INTEGER (L"Number of interior knots"), GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_sw_ispline");
END

FORM (Dissimilarity_Configuration_getStress, L"Dissimilarity & Configuration: Get stress",
      L"Dissimilarity & Configuration: get stress")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1")
	RADIOBUTTON (L"Formula2")
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_getStress (me, c,
		GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress calculation"))));
END

FORM (Dissimilarity_Configuration_absolute_stress,
      L"Dissimilarity & Configuration: Get stress (absolute mds)",
      L"Dissimilarity & Configuration: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress (me, c, 0,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_ratio_stress, L"Dissimilarity & Configuration: Get stress (ratio mds)",
      L"Dissimilarity & Configuration: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress (me, c, 0,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_interval_stress,
      L"Dissimilarity & Configuration: Get stress (interval mds)",
      L"Dissimilarity & Configuration: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_interval_stress (me, c, 0,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_monotone_stress,
      L"Dissimilarity & Configuration: Get stress (monotone mds)",
      L"Dissimilarity & Configuration: Get stress (monotone mds)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress (me, c, 0,
		GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_ispline_stress,
      L"Dissimilarity & Configuration: Get stress (i-spline mds)",
      L"Dissimilarity & Configuration: Get stress (i-spline mds)...")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress (me, c, 0,
		GET_INTEGER (L"Number of interior knots"), GET_INTEGER (L"Order of I-spline"), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_absolute_stress,
      L"Dissimilarity & Configuration & Weight: Get stress (absolute mds)",
      L"Dissimilarity & Configuration & Weight: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress (me, c, w,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_ratio_stress,
      L"Dissimilarity & Configuration & Weight: Get stress (ratio mds)",
      L"Dissimilarity & Configuration & Weight: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress (me, c, w,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_interval_stress,
      L"Dissimilarity & Configuration & Weight: Get stress (interval mds)",
      L"Dissimilarity & Configuration & Weight: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_interval_stress (me, c, w,
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_monotone_stress,
      L"Dissimilarity & Configuration & Weight: Get stress (monotone mds)",
      L"Dissimilarity & Configuration & Weight: Get stress (monotone mds)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach)")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress (me, c, w,
		GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_ispline_stress,
      L"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)",
      L"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)...")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Weight w = FIRST (Weight);
	Melder_information (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress (me, c, w,
		GET_INTEGER (L"Number of interior knots"), GET_INTEGER (L"Order of I-spline"), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_drawShepardDiagram, L"Dissimilarity & Configuration: Draw Shepard diagram",
      L"Dissimilarity & Configuration: Draw Shepard diagram...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_drawShepardDiagram (me, c, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_Configuration_drawAbsoluteRegression,
      L"Dissimilarity & Configuration: Draw regression (absolute mds)",
      L"Dissimilarity & Configuration: Draw regression (absolute mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawAbsoluteRegression (me, c, 0, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_Configuration_drawRatioRegression,
      L"Dissimilarity & Configuration: Draw regression (ratio mds)",
      L"Dissimilarity & Configuration: Draw regression (ratio mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawRatioRegression (me, c, 0, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_Configuration_drawIntervalRegression,
      L"Dissimilarity & Configuration: Draw regression (interval mds)",
      L"Dissimilarity & Configuration: Draw regression (interval mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawIntervalRegression (me, c, 0, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_Configuration_drawMonotoneRegression,
      L"Dissimilarity & Configuration: Draw regression (monotone mds)",
      L"Dissimilarity & Configuration: Draw regression (monotone mds)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach)")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawMonotoneRegression (me, c, 0, GRAPHICS,
		GET_INTEGER (L"Handling of ties"), GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_Configuration_drawISplineRegression,
      L"Dissimilarity & Configuration: Draw regression (i-spline mds)",
      L"Dissimilarity & Configuration: Draw regression (i-spline mds)...")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"3")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration c = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawISplineRegression (me, c, 0, GRAPHICS,
		GET_INTEGER (L"Number of interior knots"), GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
END

FORM (Dissimilarity_kruskal, L"Dissimilarity: To Configuration (kruskal)", L"Dissimilarity: To Configuration (kruskal)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	NATURAL (L"Distance metric", L"2 (=Euclidean)")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1")
	RADIOBUTTON (L"Formula2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_kruskal (me, GET_INTEGER (L"Number of dimensions"),
			GET_INTEGER (L"Distance metric"), GET_INTEGER (L"Handling of ties"),
			GET_INTEGER (L"Stress calculation"), GET_REAL (L"Tolerance"),
			GET_INTEGER (L"Maximum number of iterations"), GET_INTEGER (L"Number of repetitions")), my name);
	}
END

FORM (Dissimilarity_absolute_mds, L"Dissimilarity: To Configuration (absolute mds)",
      L"Dissimilarity: To Configuration (absolute mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_absolute_mds (me, NULL, GET_INTEGER (L"Number of dimensions"),
			GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
			GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_absolute");
	}
END

FORM (Dissimilarity_ratio_mds, L"Dissimilarity: To Configuration (ratio mds)",
      L"Dissimilarity: To Configuration (ratio mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_ratio_mds (me, 0, GET_INTEGER (L"Number of dimensions"),
			GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
			GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_ratio");
	}
END

FORM (Dissimilarity_interval_mds, L"Dissimilarity: To Configuration (interval mds)",
      L"Dissimilarity: To Configuration (interval mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_interval_mds (me, 0, GET_INTEGER (L"Number of dimensions"),
			GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
			GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_interval");
	}
END

FORM (Dissimilarity_monotone_mds, L"Dissimilarity: To Configuration (monotone mds)",
      L"Dissimilarity: To Configuration (monotone mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_monotone_mds (me, 0, GET_INTEGER (L"Number of dimensions"),
			GET_INTEGER (L"Handling of ties"),
			GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
			GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_monotone");
	}
END

FORM (Dissimilarity_ispline_mds, L"Dissimilarity: To Configuration (i-spline mds)",
      L"Dissimilarity: To Configuration (i-spline mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	LABEL (L"", L"Spline smoothing")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	long niknots = GET_INTEGER (L"Number of interior knots");
	long order = GET_INTEGER (L"Order of I-spline");
	if (not (order > 0 || niknots > 0)) {
		Melder_throw ("Order-zero spline must at least have 1 interior knot.");
	}
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_ispline_mds (me, 0, GET_INTEGER (L"Number of dimensions"), niknots, order,
			GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
			GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_ispline");
	}
END

FORM (Dissimilarity_Weight_ispline_mds, L"Dissimilarity & Weight: To Configuration (i-spline mds)",
      L"Dissimilarity & Weight: To Configuration (i-spline mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	LABEL (L"", L"Spline smoothing")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	long niknots = GET_INTEGER (L"Number of interior knots");
	long order = GET_INTEGER (L"Order of I-spline");
	if (not (order > 0 || niknots > 0)) {
		Melder_throw ("Order-zero spline must at least have 1 interior knot.");
	}
	praat_new (Dissimilarity_Weight_ispline_mds (me, w, GET_INTEGER (L"Number of dimensions"), niknots, order,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_ispline");
END

FORM (Dissimilarity_Weight_absolute_mds, L"Dissimilarity & Weight: To Configuration (absolute mds)",
      L"Dissimilarity & Weight: To Configuration (absolute mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_absolute_mds (me, w, GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_absolute");
END

FORM (Dissimilarity_Weight_ratio_mds, L"Dissimilarity & Weight: To Configuration (ratio mds)",
      L"Dissimilarity & Weight: To Configuration (ratio mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_ratio_mds (me, w, GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_absolute");
END

FORM (Dissimilarity_Weight_interval_mds, L"Dissimilarity & Weight: To Configuration (interval mds)",
      L"Dissimilarity & Weight: To Configuration (interval mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_interval_mds (me, w, GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_absolute");
END

FORM (Dissimilarity_Weight_monotone_mds, L"Dissimilarity & Weight: To Configuration (monotone mds)",
      L"Dissimilarity & Weight: To Configuration (monotone mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_monotone_mds (me, w, GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Handling of ties"), GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress), my name, L"_monotone");
END

FORM (Dissimilarity_to_Distance, L"Dissimilarity: To Distance", L"Dissimilarity: To Distance...")
	BOOLEAN (L"Scale (additive constant)", 1)
	OK
DO
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_to_Distance (me, GET_INTEGER (L"Scale")), my name);
	}
END

DIRECT (Dissimilarity_to_Weight)
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_to_Weight (me), my name);
	}
END

/************************* Distance(s) ***************************************/

FORM (Distance_to_ScalarProduct, L"Distance: To ScalarProduct", L"Distance: To ScalarProduct...")
	BOOLEAN (L"Make sum of squares equal 1.0", 1)
	OK
DO
	LOOP {
		iam (Distance);
		praat_new (Distance_to_ScalarProduct (me, GET_INTEGER (L"Make sum of squares equal 1.0")), my name);
	}
END

DIRECT (Distance_to_Dissimilarity)
	LOOP {
		iam (Distance);
		praat_new (Distance_to_Dissimilarity (me), my name);
	}
END

FORM (Distances_indscal, L"Distance: To Configuration (indscal)", L"Distance: To Configuration (indscal)...")
	NATURAL (L"Number of dimensions", L"2")
	BOOLEAN (L"Normalize scalar products", 1)
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"100 (= each repetition)")
	NATURAL (L"Number of repetitions", L"1")
	OK
DO
	autoDistances me = (Distances) praat_getSelectedObjects ();
	Configuration c; Salience s;
	Distances_indscal (me.peek(), GET_INTEGER (L"Number of dimensions"), GET_INTEGER (L"Normalize scalar products"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"), GET_INTEGER (L"Number of repetitions"),
		1, &c, &s);
	autoConfiguration ac = c; autoSalience as = s;
	praat_new (ac.transfer(), L"indscal");
	praat_new (as.transfer(), L"indscal");
END

FORM (Distance_and_Configuration_drawScatterDiagram, L"Distance & Configuration: Draw scatter diagram",
      L"Distance & Configuration: Draw scatter diagram...")
	REAL (L"Minimum x-distance", L"0.0")
	REAL (L"Maximum x-distance", L"0.0")
	REAL (L"Minimum y-distance", L"0.0")
	REAL (L"Maximum y-distance", L"0.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	Distance me = FIRST (Distance);
	Configuration c = FIRST (Configuration);
	Distance_and_Configuration_drawScatterDiagram (me, c, GRAPHICS, GET_REAL (L"Minimum x-distance"),
		GET_REAL (L"Maximum x-distance"), GET_REAL (L"Minimum y-distance"), GET_REAL (L"Maximum y-distance"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
END

FORM (Distance_Configuration_indscal, L"Distance & Configuration: To Configuration (indscal)",
      L"Distance & Configuration: To Configuration (indscal)...")
	BOOLEAN (L"Normalize scalar products", 1)
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"100 (= each repetition)")
	OK
DO
	autoDistances thee = Distances_create ();
	Collection_dontOwnItems (thee.peek());
	Configuration c = 0;
	LOOP {
		iam (Data);
		if (CLASS == classDistance) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classConfiguration) {
			c = (Configuration) me;
		}
	}
	Melder_assert (thy size > 0 && c);
	Configuration cr; Salience sr;
	Distances_Configuration_indscal (thee.peek(), c,
		GET_INTEGER (L"Normalize scalar products"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), 1, &cr, &sr);
	autoConfiguration ac = cr; autoSalience as = sr;
	praat_new (ac.transfer(), L"indscal");
	praat_new (as.transfer(), L"indscal");
END

FORM (Distance_Configuration_vaf, L"Distance & Configuration: Get VAF", L"Distance & Configuration: Get VAF...")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	autoDistances thee = Distances_create ();
	Collection_dontOwnItems (thee.peek());
	Configuration c = 0;
	LOOP {
		iam (Data);
		if (CLASS == classDistance) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classConfiguration) {
			c = (Configuration) me;
		}
	}
	Melder_assert (thy size > 0 && c);
	double vaf;
	Distances_Configuration_vaf (thee.peek(), c, GET_INTEGER (L"Normalize scalar products"), &vaf);
	Melder_information (Melder_double (vaf));
END

FORM (Distance_Configuration_Salience_vaf, L"Distance & Configuration & Salience: Get VAF", L"Distance & Configuration & Salience: Get VAF...")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	autoDistances thee = Distances_create ();
	Collection_dontOwnItems (thee.peek());
	Configuration c = 0; Salience s = 0;
	LOOP {
		iam (Data);
		if (CLASS == classDistance) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classConfiguration) {
			c = (Configuration) me;
		} else if (CLASS == classSalience) {
			s = (Salience) me;
		}
	}
	Melder_assert (thy size > 0 && c && s);
	double vaf;
	Distances_Configuration_Salience_vaf (thee.peek(), c, s, GET_INTEGER (L"Normalize scalar products"), &vaf);
	Melder_information (Melder_double (vaf));
END

FORM (Dissimilarity_Configuration_Salience_vaf, L"Dissimilarity & Configuration & Salience: Get VAF",
      L"Dissimilarity & Configuration & Salience: Get VAF...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	autoDissimilarities thee = Dissimilarities_create ();
	Collection_dontOwnItems (thee.peek());
	Configuration c = 0; Salience s = 0;
	LOOP {
		iam (Data);
		if (CLASS == classDissimilarity) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classConfiguration) {
			c = (Configuration) me;
		} else if (CLASS == classSalience) {
			s = (Salience) me;
		}
	}
	Melder_assert (thy size > 0 && c && s);
	double vaf;
	Dissimilarities_Configuration_Salience_vaf (thee.peek(), c, s, GET_INTEGER (L"Handling of ties"),
		GET_INTEGER (L"Normalize scalar products"), &vaf);
	Melder_information (Melder_double (vaf));
END

FORM (Distance_Configuration_Salience_indscal,
      L"Distance & Configuration & Salience: To Configuration (indscal)",
      L"Distance & Configuration & Salience: To Configuration (indscal)...")
	BOOLEAN (L"Normalize scalar products", 1)
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"100")
	OK
DO
	autoDistances thee = Distances_create ();
	Collection_dontOwnItems (thee.peek());
	Configuration c = 0; Salience s = 0;
	LOOP {
		iam (Data);
		if (CLASS == classDistance) {
			Collection_addItem (thee.peek(), me);
		} else if (CLASS == classConfiguration) {
			c = (Configuration) me;
		} else if (CLASS == classSalience) {
			s = (Salience) me;
		}
	}
	Melder_assert (thy size > 0 && c && s);
	double vaf;
	Configuration cr; Salience sr;
	Distances_Configuration_Salience_indscal (thee.peek(), c, s,
		GET_INTEGER (L"Normalize scalar products"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), 1, &cr, &sr, &vaf);
	autoConfiguration ac = cr; autoSalience as = sr;
	praat_new (ac.transfer(), L"indscal");
	praat_new (as.transfer(), L"indscal");
END

FORM (Distances_to_Configuration_ytl, L"Distance: To Configuration (ytl)", L"Distance: To Configuration (ytl)...")
	NATURAL (L"Number of dimensions", L"2")
	BOOLEAN (L"Normalize scalar products", 1)
	BOOLEAN (L"Salience object", 0)
	OK
DO
	autoDistances me = (Distances) praat_getSelectedObjects ();
	Configuration cr; Salience sr;
	Distances_to_Configuration_ytl (me.peek(), GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Normalize scalar products"), &cr, &sr);
	autoConfiguration ac = cr; autoSalience as = sr;
	praat_new (ac.transfer(), L"ytl");
	if (GET_INTEGER (L"Salience object")) {
		praat_new (as.transfer(), L"ytl");
	}
END

FORM (Dissimilarity_Distance_monotoneRegression, L"Dissimilarity & Distance: Monotone regression", 0)
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Distance d = FIRST (Distance);
	praat_new (Dissimilarity_Distance_monotoneRegression (me, d, GET_INTEGER (L"Handling of ties")), my name);
END

FORM (Distance_Dissimilarity_drawShepardDiagram, L"Distance & Dissimilarity: Draw Shepard diagram", L"")
	REAL (L"Minimum dissimilarity", L"0.0")
	REAL (L"Maximum dissimilarity", L"0.0")
	REAL (L"left Distance range", L"0.0")
	REAL (L"right Distance range", L"0.0")
	POSITIVE (L"Mark size (mm)", L"1.0")
	SENTENCE (L"Mark string (+xo.)", L"+")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Distance d = FIRST (Distance);
	Proximity_Distance_drawScatterDiagram (me, d, GRAPHICS,
		GET_REAL (L"Minimum dissimilarity"), GET_REAL (L"Maximum dissimilarity"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish"));
END

DIRECT (MDS_help)
	Melder_help (L"Multidimensional scaling");
END


/************************* Salience ***************************************/


FORM (Salience_draw, L"Salience: Draw", 0)
	NATURAL (L"Horizontal dimension", L"1")
	NATURAL (L"Vertical dimension", L"2")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	LOOP {
		iam (Salience);
		Salience_draw (me, GRAPHICS, GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_INTEGER (L"Garnish"));
	}
END

/************************* COVARIANCE & CONFIGURATION  ********************/

FORM (Covariance_to_Configuration, L"Covariance: To Configuration", 0)
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	LOOP {
		iam (Covariance);
		praat_new (Covariance_to_Configuration (me, GET_INTEGER (L"Number of dimensions")), my name);
	}
END

/********* Procrustes ***************************/

DIRECT (Procrustes_help)
	Melder_help (L"Procrustes");
END

DIRECT (Procrustes_getScale)
	LOOP {
		iam (Procrustes);
		Melder_information (Melder_double (my s));
	}
END

/********* Casts from & to TableOfReal ***************************/

DIRECT (TableOfReal_to_Dissimilarity)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Dissimilarity (me), my name);
	}
END

DIRECT (TableOfReal_to_Similarity)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Similarity (me), my name);
	}
END

DIRECT (TableOfReal_to_Distance)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Distance (me), my name);
	}
END

DIRECT (TableOfReal_to_Salience)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Salience (me), my name);
	}
END

DIRECT (TableOfReal_to_Weight)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Weight (me), my name);
	}
END

DIRECT (TableOfReal_to_ScalarProduct)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_ScalarProduct (me), my name);
	}
END

DIRECT (TableOfReal_to_Configuration)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Configuration (me), my name);
	}
END

DIRECT (TableOfReal_to_ContingencyTable)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_ContingencyTable (me), my name);
	}
END

/********************** TableOfReal ***************************************/

DIRECT (TableOfReal_getTableNorm)
	LOOP {
		iam (TableOfReal);
		Melder_information (Melder_double (TableOfReal_getTableNorm (me)));
	}
END

FORM (TableOfReal_normalizeTable, L"TableOfReal: Normalize table", L"TableOfReal: Normalize table...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeTable (me, GET_REAL (L"Norm"));
	}
END

FORM (TableOfReal_normalizeRows, L"TableOfReal: Normalize rows", L"TableOfReal: Normalize rows...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeRows (me, GET_REAL (L"Norm"));
	}
END

FORM (TableOfReal_normalizeColumns, L"TableOfReal: Normalize columns", L"TableOfReal: Normalize columns...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeColumns (me, GET_REAL (L"Norm"));
	}
END

DIRECT (TableOfReal_centreRows)
	LOOP {
		iam (TableOfReal);
		TableOfReal_centreRows (me);
	}
END

DIRECT (TableOfReal_centreColumns)
	LOOP {
		iam (TableOfReal);
		TableOfReal_centreColumns (me);
	}
END

DIRECT (TableOfReal_doubleCentre)
	LOOP {
		iam (TableOfReal);
		TableOfReal_doubleCentre (me);
	}
END

DIRECT (TableOfReal_standardizeRows)
	LOOP {
		iam (TableOfReal);
		TableOfReal_standardizeRows (me);
	}
END

DIRECT (TableOfReal_standardizeColumns)
	LOOP {
		iam (TableOfReal);
		TableOfReal_standardizeColumns (me);
	}
END

DIRECT (TableOfReal_to_Confusion)
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Confusion (me), my name);
	}
END

static void praat_AffineTransform_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 1, L"Get transformation element...", QUERY_BUTTON, 1, DO_AffineTransform_getTransformationElement);
	praat_addAction1 (klas, 1, L"Get translation element...", QUERY_BUTTON, 1, DO_AffineTransform_getTranslationElement);
	praat_addAction1 (klas, 0, L"Invert", 0, 0, DO_AffineTransform_invert);
}

static void praat_Configuration_and_AffineTransform_init (ClassInfo transform) {
	praat_addAction2 (classConfiguration, 1, transform, 1, L"To Configuration", 0, 0, DO_Configuration_and_AffineTransform_to_Configuration);
}

void praat_TableOfReal_extras (ClassInfo klas);
void praat_TableOfReal_extras (ClassInfo klas) {
	praat_addAction1 (klas, 1, L"-- get additional --", L"Get value...", 1, 0);
	praat_addAction1 (klas, 1, L"Get table norm", L"-- get additional --", 1, DO_TableOfReal_getTableNorm);
	praat_addAction1 (klas, 1, L"-- set additional --", L"Set column label (label)...", 1, 0);
	praat_addAction1 (klas, 1, L"Normalize rows...", L"-- set additional --", 1, DO_TableOfReal_normalizeRows);
	praat_addAction1 (klas, 1, L"Normalize columns...", L"Normalize rows...", 1, DO_TableOfReal_normalizeColumns);
	praat_addAction1 (klas, 1, L"Normalize table...", L"Normalize columns...", 1, DO_TableOfReal_normalizeTable);
	praat_addAction1 (klas, 1, L"Standardize rows", L"Normalize table...", 1, DO_TableOfReal_standardizeRows);
	praat_addAction1 (klas, 1, L"Standardize columns", L"Standardize rows", 1, DO_TableOfReal_standardizeColumns);
	praat_addAction1 (klas, 1, L"Test sorting...", L"Standardize columns", praat_DEPTH_1 + praat_HIDDEN, DO_TabelOfReal_testSorting);
}

void praat_uvafon_MDS_init ();
void praat_uvafon_MDS_init () {
	Thing_recognizeClassesByName (classAffineTransform, classProcrustes,
	                              classContingencyTable, classDissimilarity,
	                              classSimilarity, classConfiguration, classDistance,
	                              classSalience, classScalarProduct, classWeight, NULL);
	Thing_recognizeClassByOtherName (classProcrustes, L"Procrustus");

	praat_addMenuCommand (L"Objects", L"New", L"Multidimensional scaling", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"MDS tutorial", 0, 1, DO_MDS_help);
	praat_addMenuCommand (L"Objects", L"New", L"-- MDS --", 0, 1, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Create letter R example...", 0, 1, DO_Dissimilarity_createLetterRExample);
	praat_addMenuCommand (L"Objects", L"New", L"Create INDSCAL Carroll Wish example...", 0, 1, DO_INDSCAL_createCarrollWishExample);
	praat_addMenuCommand (L"Objects", L"New", L"Create Configuration...", 0, 1, DO_Configuration_create);
	praat_addMenuCommand (L"Objects", L"New", L"Draw splines...", 0, 1, DO_drawSplines);
	praat_addMenuCommand (L"Objects", L"New", L"Draw MDS class relations", 0, 1, DO_drawMDSClassRelations);

	/****** 1 class ********************************************************/

	praat_addAction1 (classAffineTransform, 0, L"AffineTransform help", 0, 0, DO_AffineTransform_help);
	praat_AffineTransform_init (classAffineTransform);


	praat_addAction1 (classConfiguration, 0, L"Configuration help", 0, 0, DO_Configuration_help);
	praat_TableOfReal_init2 (classConfiguration);
	praat_TableOfReal_extras (classConfiguration);
	(void) praat_removeAction (classConfiguration, NULL, NULL, L"Insert column (index)...");
	(void) praat_removeAction (classConfiguration, NULL, NULL, L"Remove column (index)...");
	(void) praat_removeAction (classConfiguration, NULL, NULL, L"Append");
	praat_addAction1 (classConfiguration, 0, L"Draw...", DRAW_BUTTON, 1, DO_Configuration_draw);
	praat_addAction1 (classConfiguration, 0, L"Draw sigma ellipses...", L"Draw...", 1, DO_Configuration_drawSigmaEllipses);
	praat_addAction1 (classConfiguration, 0, L"Draw one sigma ellipse...", L"Draw...", 1, DO_Configuration_drawOneSigmaEllipse);
	praat_addAction1 (classConfiguration, 0, L"Draw confidence ellipses...", L"Draw sigma ellipses...", 1, DO_Configuration_drawConfidenceEllipses);
	praat_addAction1 (classConfiguration, 0, L"Draw one confidence ellipse...", L"Draw sigma ellipses...", 1, DO_Configuration_drawOneConfidenceEllipse);

	praat_addAction1 (classConfiguration, 0, L"Randomize", L"Normalize table...", 1, DO_Configuration_randomize);
	praat_addAction1 (classConfiguration, 0, L"Normalize...", L"Randomize", 1, DO_Configuration_normalize);
	praat_addAction1 (classConfiguration, 0, L"Centralize", L"Randomize", 1, DO_Configuration_centralize);
	praat_addAction1 (classConfiguration, 1, L"-- set rotations & reflections --", L"Centralize", 1, 0);

	praat_addAction1 (classConfiguration, 0, L"Rotate...", L"-- set rotations & reflections --", 1, DO_Configuration_rotate);
	praat_addAction1 (classConfiguration, 0, L"Rotate (pc)", L"Rotate...", 1, DO_Configuration_rotateToPrincipalDirections);
	praat_addAction1 (classConfiguration, 0, L"Invert dimension...", L"Rotate (pc)", 1, DO_Configuration_invertDimension);
	praat_addAction1 (classConfiguration, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classConfiguration, 0, L"To Distance", 0, 0, DO_Configuration_to_Distance);
	praat_addAction1 (classConfiguration, 0, L"To Configuration (varimax)...", 0, 0, DO_Configuration_varimax);
	praat_addAction1 (classConfiguration, 0, L"To Similarity (cc)", 0, 0, DO_Configurations_to_Similarity_cc);

	praat_addAction1 (classConfiguration, 0, L"Match configurations -", 0, 0, 0);
	praat_addAction1 (classConfiguration, 2, L"To Procrustes...", 0, 1, DO_Configurations_to_Procrustes);
	praat_addAction1 (classConfiguration, 2, L"To AffineTransform (congruence)...", 0, 1, DO_Configurations_to_AffineTransform_congruence);

	praat_addAction1 (classConfusion, 0, L"To ContingencyTable", L"To Matrix", 0, DO_Confusion_to_ContingencyTable);
	praat_addAction1 (classConfusion, 0, L"To Proximity -", L"Analyse", 0, 0);
	praat_addAction1 (classConfusion, 0, L"To Dissimilarity (pdf)...", L"To Proximity -", 1, DO_Confusion_to_Dissimilarity_pdf);
	praat_addAction1 (classConfusion, 0, L"To Similarity...", L"To Proximity -", 1, DO_Confusion_to_Similarity);
	praat_addAction1 (classConfusion, 0, L"Sum", L"Synthesize -", 1, DO_Confusions_sum);


	praat_TableOfReal_init2 (classContingencyTable);
	praat_addAction1 (classContingencyTable, 1, L"-- statistics --", L"Get value...", 1, 0);
	praat_addAction1 (classContingencyTable, 1, L"Get chi squared probability", L"-- statistics --", 1, DO_ContingencyTable_chisqProbability);
	praat_addAction1 (classContingencyTable, 1, L"Get Cramer's statistic", L"Get chi squared probability", 1, DO_ContingencyTable_cramersStatistic);
	praat_addAction1 (classContingencyTable, 1, L"Get contingency coefficient", L"Get Cramer's statistic", 1,
	                  DO_ContingencyTable_contingencyCoefficient);
	praat_addAction1 (classContingencyTable, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classContingencyTable, 1, L"To Configuration (ca)...", 0, 0, DO_ContingencyTable_to_Configuration_ca);


	praat_addAction1 (classCorrelation, 0, L"To Configuration...", 0, 0, DO_Correlation_to_Configuration);

	praat_addAction1 (classDissimilarity, 0, L"Dissimilarity help", 0, 0, DO_Dissimilarity_help);
	praat_TableOfReal_init2 (classDissimilarity);
	praat_TableOfReal_extras (classDissimilarity);
	praat_addAction1 (classDissimilarity, 0, L"Get additive constant", L"Get table norm", 1, DO_Dissimilarity_getAdditiveConstant);
	praat_addAction1 (classDissimilarity, 0, CONFIGURATION_BUTTON, 0, 0, 0);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (monotone mds)...", 0, 1, DO_Dissimilarity_monotone_mds);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (i-spline mds)...", 0, 1, DO_Dissimilarity_ispline_mds);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (interval mds)...", 0, 1, DO_Dissimilarity_interval_mds);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (ratio mds)...", 0, 1, DO_Dissimilarity_ratio_mds);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (absolute mds)...", 0, 1, DO_Dissimilarity_absolute_mds);
	praat_addAction1 (classDissimilarity, 1, L"To Configuration (kruskal)...", 0, 1, DO_Dissimilarity_kruskal);
	praat_addAction1 (classDissimilarity, 0, L"To Distance...", 0, 0, DO_Dissimilarity_to_Distance);
	praat_addAction1 (classDissimilarity, 0, L"To Weight", 0, 0, DO_Dissimilarity_to_Weight);


	praat_addAction1 (classCovariance, 0, L"To Configuration...", 0, 0, DO_Covariance_to_Configuration);


	praat_TableOfReal_init2 (classDistance);
	praat_TableOfReal_extras (classDistance);
	praat_addAction1 (classDistance, 0, L"Analyse -", 0, 0, 0);
	praat_addAction1 (classDistance, 0, CONFIGURATION_BUTTON, 0, 0, 0);
	praat_addAction1 (classDistance, 0, L"To Configuration (indscal)...", 0, 1, DO_Distances_indscal);
	praat_addAction1 (classDistance, 0, L"-- linear scaling --", 0, 1, 0);
	praat_addAction1 (classDistance, 0, L"To Configuration (ytl)...", 0, 1, DO_Distances_to_Configuration_ytl);
	praat_addAction1 (classDistance, 0, L"To Dissimilarity", 0, 0, DO_Distance_to_Dissimilarity);
	praat_addAction1 (classDistance, 0, L"To ScalarProduct...", 0, 0, DO_Distance_to_ScalarProduct);


	praat_addAction1 (classProcrustes, 0, L"Procrustes help", 0, 0, DO_Procrustes_help);
	praat_AffineTransform_init (classProcrustes);
	praat_addAction1 (classProcrustes, 1, L"Get scale", QUERY_BUTTON, 1, DO_Procrustes_getScale);
	praat_addAction1 (classProcrustes, 0, L"Extract transformation matrix", 0, 0, DO_AffineTransform_extractMatrix);
	praat_addAction1 (classProcrustes, 0, L"Extract translation vector", 0, 0, DO_AffineTransform_extractTranslationVector);

	praat_TableOfReal_init2 (classSalience);
	praat_TableOfReal_extras (classSalience);
	praat_addAction1 (classSalience, 0, L"Draw...", DRAW_BUTTON, 1, DO_Salience_draw);


	praat_addAction1 (classSimilarity, 0, L"Similarity help", 0, 0, DO_Similarity_help);
	praat_TableOfReal_init2 (classSimilarity);
	praat_TableOfReal_extras (classSimilarity);
	praat_addAction1 (classSimilarity, 0, L"Analyse -", 0, 0, 0);
	praat_addAction1 (classSimilarity, 0, L"To Dissimilarity...", 0, 0, DO_Similarity_to_Dissimilarity);


	praat_TableOfReal_init2 (classScalarProduct);
	praat_TableOfReal_extras (classScalarProduct);

	praat_TableOfReal_extras (classTableOfReal);
	praat_addAction1 (classTableOfReal, 1, L"Centre rows", L"Normalize table...", 1, DO_TableOfReal_centreRows);
	praat_addAction1 (classTableOfReal, 1, L"Centre columns", L"Centre rows", 1, DO_TableOfReal_centreColumns);
	praat_addAction1 (classTableOfReal, 1, L"Double centre", L"Centre columns", 1, DO_TableOfReal_doubleCentre);
	praat_addAction1 (classTableOfReal, 0, L"Cast -", 0, 0, 0);
	praat_addAction1 (classTableOfReal, 0, L"To Confusion", 0, 1, DO_TableOfReal_to_Confusion);
	praat_addAction1 (classTableOfReal, 0, L"To Dissimilarity", 0, 1, DO_TableOfReal_to_Dissimilarity);
	praat_addAction1 (classTableOfReal, 0, L"To Similarity", 0, 1, DO_TableOfReal_to_Similarity);
	praat_addAction1 (classTableOfReal, 0, L"To Distance", 0, 1, DO_TableOfReal_to_Distance);
	praat_addAction1 (classTableOfReal, 0, L"To Salience", 0, 1, DO_TableOfReal_to_Salience);
	praat_addAction1 (classTableOfReal, 0, L"To Weight", 0, 1, DO_TableOfReal_to_Weight);
	praat_addAction1 (classTableOfReal, 0, L"To ScalarProduct", 0, 1, DO_TableOfReal_to_ScalarProduct);
	praat_addAction1 (classTableOfReal, 0, L"To Configuration", 0, 1, DO_TableOfReal_to_Configuration);
	praat_addAction1 (classTableOfReal, 0, L"To ContingencyTable", 0, 1, DO_TableOfReal_to_ContingencyTable);

	praat_TableOfReal_init2 (classWeight);


	/****** 2 classes ********************************************************/

	praat_Configuration_and_AffineTransform_init (classAffineTransform);
	praat_Configuration_and_AffineTransform_init (classProcrustes);

	praat_addAction2 (classConfiguration, 0, classWeight, 1, L"Analyse", 0, 0, 0);
	praat_addAction2 (classConfiguration, 0, classWeight, 1, L"To Similarity (cc)", 0, 0, DO_Configuration_Weight_to_Similarity_cc);

	praat_addAction2 (classDissimilarity, 1, classWeight, 1, ANALYSE_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, L"To Configuration (monotone mds)...", 0, 1, DO_Dissimilarity_Weight_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, L"To Configuration (i-spline mds)...", 0, 1, DO_Dissimilarity_Weight_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, L"To Configuration (interval mds)...", 0, 1, DO_Dissimilarity_Weight_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, L"To Configuration (ratio mds)...", 0, 1, DO_Dissimilarity_Weight_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, L"To Configuration (absolute mds)...", 0, 1, DO_Dissimilarity_Weight_absolute_mds);


	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, DRAW_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw Shepard diagram...", 0, 1, DO_Dissimilarity_Configuration_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"-- draw regressions --", 0, 1, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw monotone regression...", 0, 1, DO_Dissimilarity_Configuration_drawMonotoneRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw i-spline regression...", 0, 1, DO_Dissimilarity_Configuration_drawISplineRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw interval regression...", 0, 1, DO_Dissimilarity_Configuration_drawIntervalRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw ratio regression...", 0, 1, DO_Dissimilarity_Configuration_drawRatioRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Draw absolute regression...", 0, 1, DO_Dissimilarity_Configuration_drawAbsoluteRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress...", 0, 1, DO_Dissimilarity_Configuration_getStress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress (monotone mds)...", 0, 1, DO_Dissimilarity_Configuration_monotone_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress (i-spline mds)...", 0, 1, DO_Dissimilarity_Configuration_ispline_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress (interval mds)...", 0, 1, DO_Dissimilarity_Configuration_interval_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress (ratio mds)...", 0, 1, DO_Dissimilarity_Configuration_ratio_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"Get stress (absolute mds)...", 0, 1, DO_Dissimilarity_Configuration_absolute_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, ANALYSE_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (monotone mds)...", 0, 1, DO_Dissimilarity_Configuration_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (i-spline mds)...", 0, 1, DO_Dissimilarity_Configuration_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (interval mds)...", 0, 1, DO_Dissimilarity_Configuration_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (ratio mds)...", 0, 1, DO_Dissimilarity_Configuration_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (absolute mds)...", 0, 1, DO_Dissimilarity_Configuration_absolute_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, L"To Configuration (kruskal)...", 0, 1, DO_Dissimilarity_Configuration_kruskal);

	praat_addAction2 (classDistance, 1, classConfiguration, 1, DRAW_BUTTON, 0, 0, 0);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, L"Draw scatter diagram...", 0, 0, DO_Distance_and_Configuration_drawScatterDiagram);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, L"Get VAF...", 0, 0, DO_Distance_Configuration_vaf);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, ANALYSE_BUTTON, 0, 0, 0);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, L"To Configuration (indscal)...", 0, 1, DO_Distance_Configuration_indscal);

	praat_addAction2 (classDistance, 1, classDissimilarity, 1, L"Draw Shepard diagram...", 0, 0, DO_Distance_Dissimilarity_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classDistance, 1, L"Monotone regression...", 0, 0, DO_Dissimilarity_Distance_monotoneRegression);

	/****** 3 classes ********************************************************/


	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, classSalience, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, classSalience, 1, L"Get VAF...", 0, 1, DO_Dissimilarity_Configuration_Salience_vaf);

	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"Get stress (monotone mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_monotone_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"Get stress (i-spline mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_ispline_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"Get stress (interval mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_interval_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"Get stress (ratio mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_ratio_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"Get stress (absolute mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_absolute_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, ANALYSE_BUTTON, 0, 0, 0);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"To Configuration (monotone mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_monotone_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"To Configuration (i-spline mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_ispline_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"To Configuration (interval mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_interval_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"To Configuration (ratio mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_ratio_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, L"To Configuration (absolute mds)...", 0, 1, DO_Dissimilarity_Configuration_Weight_absolute_mds);


	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, QUERY_BUTTON, 0, 0, 0);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, L"Get VAF...", 0, 1, DO_Distance_Configuration_Salience_vaf);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, L"Analyse", 0, 0, 0);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, L"To Configuration (indscal)...", 0, 0, DO_Distance_Configuration_Salience_indscal);


	INCLUDE_MANPAGES (manual_MDS_init)
}

/* End of file praat_MDS_init.c 1775*/
