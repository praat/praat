/* praat_MDS_init.c
 *
 * Copyright (C) 1992-2007 David Weenink
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
*/

#include <math.h>
#include "NUM2.h"
#include "praat.h"
#include "MDS.h"
#include "TableOfReal_extensions.h"
#include "Configuration_and_Procrustes.h"
#include "Configuration_AffineTransform.h"
#include "Confusion.h"
#include "Formula.h"

void praat_TableOfReal_init (void *klas);

static wchar_t *QUERY_BUTTON   = L"Query -                ";
static wchar_t *DRAW_BUTTON    = L"Draw -                 ";
static wchar_t *ANALYSE_BUTTON = L"Analyse -              ";
static wchar_t *CONFIGURATION_BUTTON = L"To Configuration -     ";
extern void praat_TableOfReal_init2  (void *klas);

/* Tests */

/*
	Sort row 1 ascending and store in row 3 
	Sort row 1 and move row 2 along and store in rows 4 and 5 respectively
	Make an index for row 1 and store in row 6
*/
static int TabelOfReal_testSorting (I, long rowtoindex)
{
	iam (TableOfReal);
	long i, nr = my numberOfRows, nc = my numberOfColumns;
	long *index = NUMlvector (1, nc);
	
	if (index == NULL) return 0;
	if (nr < 6) return Melder_error1 (L"TabelOfReal_sort2: we want at least 6 rows!!");
	if (rowtoindex < 1 || rowtoindex > 2) 
		return Melder_error1 (L"TabelOfReal_sort2: rowtoindex <= 2");

	/* Copy 1->3 and sort 3 inplace */	
	NUMdvector_copyElements (my data[1], my data[3], 1, nc);
	NUMsort_d (nc, my data[3]);

	/* Copy 1->4 and 2->5, sort 4+5 in parallel */	
	NUMdvector_copyElements (my data[1], my data[4], 1, nc);	
	NUMdvector_copyElements (my data[2], my data[5], 1, nc);	
	NUMsort2_dd (nc, my data[4], my data[5]);

	/* make index */
	NUMindexx_d (my data[rowtoindex], nc, index);
	for (i = 1; i <= nc; i++) my data[6][i] = index[i];
	NUMlvector_free (index, 1);
	return 1;
}

FORM (TabelOfReal_testSorting, L"TabelOfReal: Sort and index", L"")
	NATURAL (L"Row to index", L"1")
	OK
DO
	if (! TabelOfReal_testSorting (ONLY_OBJECT, GET_INTEGER (L"Row to index"))) return 0;
END
	
/************************* examples ***************************************/

FORM (Dissimilarity_createLetterRExample, L"Create letter R example", L"Create letter R example...")
	LABEL (L"", L"For the monotone transformation on the distances")
	REAL (L"Noise range", L"32.5")
	OK
DO
	(void) praat_new1 (Dissimilarity_createLetterRExample 
		(GET_REAL (L"Noise range")), NULL);
END

FORM (INDSCAL_createCarrollWishExample, 
	L"Create INDSCAL Carroll & Wish example...",
	L"Create INDSCAL Carroll & Wish example...")
	REAL (L"Noise standard deviation", L"0.0")
	OK
DO
	(void) praat_new1 (INDSCAL_createCarrollWishExample 
		(GET_REAL (L"Noise standard deviation")), NULL);
END

FORM (Configuration_create, L"Create Configuration", L"Create Configuration...")
	WORD (L"Name", L"uniform")
	NATURAL (L"Number of points", L"10")
	NATURAL (L"Number of dimensions", L"2")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"randomUniform(-1.5, 1.5)")
	OK
DO
	Configuration c = Configuration_create (GET_INTEGER (L"Number of points"),
		GET_INTEGER (L"Number of dimensions"));
	if (c == NULL || ! praat_new1 (c, GET_STRING (L"Name"))) return 0;
	if (! TableOfReal_formula (c, GET_STRING (L"formula"), NULL)) return 0;
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
	REQUIRE (xmin < xmax && ymin < ymax, L"Required: xmin < xmax and ymin < ymax.")
	praat_picture_open ();
	drawSplines (GRAPHICS, xmin, xmax, ymin, ymax, GET_INTEGER (L"Spline type"),
		GET_INTEGER (L"Order"), GET_STRING (L"Interior knots"), 
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

DIRECT (drawMDSClassRelations)
	praat_picture_open ();
	drawMDSClassRelations (GRAPHICS);
	praat_picture_close ();
END


/***************** AffineTransform ***************************************/


DIRECT (AffineTransform_help)
	Melder_help (L"AffineTransform"); 
END

DIRECT (AffineTransform_invert)
	EVERY_CHECK (praat_new2 (AffineTransform_invert (OBJECT), NAMEW, L"_inverse"))	 
END

FORM (AffineTransform_getTransformationElement, L"AffineTransform: Get transformation element", L"Procrustes")
	NATURAL (L"Row number", L"1")
	NATURAL (L"Column number", L"1")
	OK
DO
	AffineTransform me = ONLY_OBJECT;
	long row = GET_INTEGER (L"Row number");
	long column = GET_INTEGER (L"Column number");
	REQUIRE (row <= my n, L"Row number must not exceed number of rows.")
	REQUIRE (column <= my n, L"Column number must not exceed number of columns.")
	Melder_information1 (Melder_double (my r [row] [column]));
END

FORM (AffineTransform_getTranslationElement, L"AffineTransform: Get translation element", L"Procrustes")
	NATURAL (L"Index", L"1")
	OK
DO
	AffineTransform me = ONLY_OBJECT;
	long number = GET_INTEGER (L"Index");
	REQUIRE (number <= my n, L"Index must not exceed number of elements.")
	Melder_information1 (Melder_double (my t [number]));
END

DIRECT (AffineTransform_extractMatrix)
	EVERY_TO (AffineTransform_extractMatrix (OBJECT))
END

DIRECT (AffineTransform_extractTranslationVector)
	EVERY_TO (AffineTransform_extractTranslationVector (OBJECT))
END

/***************** Configuration ***************************************/

DIRECT (Configuration_help)
	Melder_help (L"Configuration"); 
END

static void Configuration_draw_addCommonFields (void *dia)
{
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
	EVERY_DRAW (Configuration_draw (OBJECT, GRAPHICS,
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
		GET_REAL (L"right Vertical range"), GET_INTEGER (L"Label size"), 
		GET_INTEGER (L"Use row labels"), GET_STRING (L"Label"),
		GET_INTEGER (L"Garnish")))
END

FORM (Configuration_drawSigmaEllipses, L"Configuration: Draw sigma ellipses", L"Configuration: Draw sigma ellipses...")
	POSITIVE (L"Number of sigmas", L"1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Number of sigmas"), 0, NULL,
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
END

FORM (Configuration_drawOneSigmaEllipse, L"Configuration: Draw one sigma ellipse", L"Configuration: Draw sigma ellipses...")
	SENTENCE (L"Label", L"")
	POSITIVE (L"Number of sigmas", L"1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Number of sigmas"), 0, GET_STRING (L"Label"),
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
END


FORM (Configuration_drawConfidenceEllipses, L"Configuration: Draw confidence ellipses", 0)
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, NULL,
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
END

FORM (Configuration_drawOneConfidenceEllipse, L"Configuration: Draw one confidence ellipse", 0)
	SENTENCE (L"Label", L"")
	POSITIVE (L"Confidence level (0-1)", L"0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER (L"Label size", L"12")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL (L"Confidence level"), 1, GET_STRING (L"Label"),
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_REAL (L"left Horizontal range"), GET_REAL (L"right Horizontal range"),
		GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"),
		GET_INTEGER (L"Label size"), GET_INTEGER (L"Garnish")))
END

DIRECT (Configuration_randomize)
	EVERY (Configuration_randomize (OBJECT))
END

FORM (Configuration_normalize, L"Configuration: Normalize", L"Configuration: Normalize...")
	REAL (L"Sum of squares", L"0.0")
	LABEL (L"", L"On (INDSCAL), Off (Kruskal)")
	BOOLEAN (L"Each dimension separately", 1)
	OK
DO
	EVERY (Configuration_normalize (OBJECT, GET_REAL (L"Sum of squares"),
		GET_INTEGER (L"Each dimension separately")))
END

DIRECT (Configuration_centralize)
	EVERY (TableOfReal_centreColumns (OBJECT))
END

FORM (Configuration_rotate, L"Configuration: Rotate", L"Configuration: Rotate...")
	NATURAL (L"Dimension 1", L"1")
	NATURAL (L"Dimension 2", L"2")
	REAL (L"Angle (degrees)", L"60.0")
	OK
DO
	EVERY (Configuration_rotate (OBJECT, GET_INTEGER (L"Dimension 1"),
		GET_INTEGER (L"Dimension 2"), GET_REAL (L"Angle")))
END

DIRECT (Configuration_rotateToPrincipalDirections)
	EVERY (Configuration_rotateToPrincipalDirections (OBJECT))
END

FORM (Configuration_invertDimension, L"Configuration: Invert dimension", L"Configuration: Invert dimension...")
	NATURAL (L"Dimension", L"1")
	OK
DO
	EVERY (Configuration_invertDimension (OBJECT, GET_INTEGER (L"Dimension")))
END

DIRECT (Configuration_to_Distance)
	EVERY_TO (Configuration_to_Distance (OBJECT))
END

FORM (Configuration_varimax, L"Configuration: To Configuration (varimax)", L"Configuration: To Configuration (varimax)...")
	BOOLEAN (L"Normalize rows", 1)
	BOOLEAN (L"Quartimax", 0)
	NATURAL (L"Maximum number of iterations", L"50")
	POSITIVE (L"Tolerance", L"1e-6")
	OK
DO
	EVERY_TO (Configuration_varimax (OBJECT, GET_INTEGER (L"Normalize rows"),
		GET_INTEGER (L"Quartimax"),
		GET_INTEGER (L"Maximum number of iterations"), GET_REAL (L"Tolerance")))
END

DIRECT (Configuration_to_Similarity_cc)
	Configurations cs = Configurations_create();
	Similarity s = NULL;
	WHERE (SELECTED) (void) Collection_addItem (cs, OBJECT);
	s = Configurations_to_Similarity_cc (cs, NULL);
	cs -> size = 0; forget (cs);
	if (! praat_new1 (s, L"congruence")) return 0;
END

FORM (Configurations_to_Procrustes, L"Configuration & Configuration: To Procrustes", L"Configuration & Configuration: To Procrustes...")
	BOOLEAN (L"Orthogonal transform", 0)
	OK
DO
	Configuration c1 = NULL, c2 = NULL;
	WHERE (SELECTED) { if (c1) c2 = OBJECT; else c1 = OBJECT; }	
	if (! praat_new3 (Configurations_to_Procrustes (c1, c2, GET_INTEGER (L"Orthogonal transform")), 
		Thing_getName (c2), L"_to_", Thing_getName (c1))) return 0;
END

FORM (Configurations_to_AffineTransform_congruence, L"Configurations: To AffineTransform (congruence)", L"Configurations: To AffineTransform (congruence)...")
	NATURAL (L"Maximum number of iterations", L"50")
	POSITIVE (L"Tolerance", L"1e-6")
	OK
DO
	Configuration c1 = NULL, c2 = NULL;
	WHERE (SELECTED && CLASS == classConfiguration)
	{
		if (c1) c2 = OBJECT; 
		else c1 = OBJECT;
	}	
	NEW (Configurations_to_AffineTransform_congruence (c1, c2,
		GET_INTEGER (L"Maximum number of iterations"),
		GET_REAL (L"Tolerance")))
END

DIRECT (Configuration_Weight_to_Similarity_cc)
	Configurations cs = Configurations_create();
	Similarity s = NULL; Weight w = ONLY (classWeight);
	WHERE (SELECTED && CLASS == classConfiguration)
	{
		(void) Collection_addItem (cs, OBJECT);
	}
	s = Configurations_to_Similarity_cc (cs, w);
	cs -> size = 0; forget (cs);
	if (! praat_new1 (s, L"congruence")) return 0;
END

DIRECT (Configuration_and_AffineTransform_to_Configuration)
	NEW (Configuration_and_AffineTransform_to_Configuration 
		(ONLY (classConfiguration), ONLY_GENERIC (classAffineTransform)))
END

/*************** Confusion *********************************/

FORM (Confusion_to_Dissimilarity_pdf, L"Confusion: To Dissimilarity (pdf)", L"Confusion: To Dissimilarity (pdf)...")
	POSITIVE (L"Minimum confusion level", L"0.5")
	OK
DO
	WHERE (SELECTED)
	{
		Confusion c = OBJECT;
		if (! praat_new2 (Confusion_to_Dissimilarity_pdf (OBJECT,
			GET_REAL (L"Minimum confusion level")), c -> name, L"_pdf"))
				return 0;
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
	EVERY_TO (Confusion_to_Similarity (OBJECT, GET_INTEGER (L"Normalize"),
		GET_INTEGER (L"Symmetrization")))
END

DIRECT (Confusions_sum)
	Confusions me = Confusions_create(); Confusion conf = NULL;
	WHERE (SELECTED) (void) Collection_addItem (me, OBJECT);
	conf = Confusions_sum (me);
	my size = 0; forget (me);
	if (! praat_new1 (conf, L"untitled_sum")) return 0;
END

DIRECT (Confusion_to_ContingencyTable)
	EVERY_TO (Confusion_to_ContingencyTable (OBJECT))
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
	EVERY_TO (ContingencyTable_to_Configuration_ca (OBJECT, 
		GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Scaling of final configuration")))
END

DIRECT (ContingencyTable_chisqProbability)
	Melder_information1 (Melder_double (ContingencyTable_chisqProbability (ONLY_OBJECT)));
END

DIRECT (ContingencyTable_cramersStatistic)
	Melder_information1 (Melder_double (ContingencyTable_cramersStatistic (ONLY_OBJECT)));
END

DIRECT (ContingencyTable_contingencyCoefficient)
	Melder_information1 (Melder_double (ContingencyTable_contingencyCoefficient (ONLY_OBJECT)));
END

/************************* Correlation ***********************************/

FORM (Correlation_to_Configuration, L"Correlation: To Configuration", 0)
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	EVERY_TO (Correlation_to_Configuration (OBJECT, 
		GET_INTEGER (L"Number of dimensions")))
END


/************************* Similarity ***************************************/

DIRECT (Similarity_help)
	Melder_help (L"Similarity"); 
END

FORM (Similarity_to_Dissimilarity, L"Similarity: To Dissimilarity", L"Similarity: To Dissimilarity...")
	REAL (L"Maximum dissimilarity", L"0.0 (=from data)")
	OK
DO
	EVERY_TO (Similarity_to_Dissimilarity (OBJECT, GET_REAL (L"Maximum dissimilarity")))
END

/**************** Dissimilarity ***************************************/

static void Dissimilarity_to_Configuration_addCommonFields (void *dia)
{
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"50 (= each repetition)")
	NATURAL (L"Number of repetitions", L"1")
}

static void Dissimilarity_and_Configuration_getStress_addCommonFields (void *dia, void *radio)
{
	RADIO (L"Stress measure", 1)
	RADIOBUTTON (L"Normalized")
	RADIOBUTTON (L"Kruskal's stress-1")
	RADIOBUTTON (L"Kruskal's stress-2")
	RADIOBUTTON (L"Raw")
}

static void Dissimilarity_Configuration_drawDiagram_addCommonFields (void *dia)
{
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
	double c;
	Dissimilarity_getAdditiveConstant(ONLY_OBJECT, &c);
	Melder_information1 (Melder_double (c));
END

FORM (Dissimilarity_Configuration_kruskal, L"Dissimilarity & Configuration: To Configuration (kruskal)", L"Dissimilarity & Configuration: To Configuration (kruskal)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON (L"Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k]-dbar)^2))")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Configuration conf = ONLY (classConfiguration);
	if (! praat_new2 (Dissimilarity_Configuration_kruskal 
		(ONLY (classDissimilarity), conf,
		GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress calculation"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions")), conf -> name, L"_s_kruskal")) return 0;
END

FORM (Dissimilarity_Configuration_absolute_mds, L"Dissimilarity & Configuration: To Configuration (absolute mds)", L"Dissimilarity & Configuration: To Configuration (absolute mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_absolute_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_s_absolute")) return 0;
END

FORM (Dissimilarity_Configuration_ratio_mds, L"Dissimilarity & Configuration: To Configuration (ratio mds)", L"Dissimilarity & Configuration: To Configuration (ratio mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_ratio_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_s_ratio")) return 0;
END

FORM (Dissimilarity_Configuration_interval_mds, L"Dissimilarity & Configuration: To Configuration (interval mds)", L"Dissimilarity & Configuration: To Configuration (interval mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_interval_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_s_interval")) return 0;
END

FORM (Dissimilarity_Configuration_monotone_mds, L"Dissimilarity & Configuration: To Configuration (monotone mds)", L"Dissimilarity & Configuration: To Configuration (monotone mds)...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_monotone_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_s_monotone")) return 0;
END

FORM (Dissimilarity_Configuration_ispline_mds, L"Dissimilarity & Configuration: To Configuration (i-spline mds)", L"Dissimilarity & Configuration: To Configuration (i-spline mds)...")
	LABEL (L"", L"Spline smoothing")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_ispline_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_INTEGER (L"Number of interior knots"), 
		GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_s_ispline")) return 0;
END

FORM (Dissimilarity_Configuration_Weight_absolute_mds, L"Dissimilarity & Configuration & Weight: To Configuration (absolute mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_absolute_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_sw_absolute")) return 0;
END

FORM (Dissimilarity_Configuration_Weight_ratio_mds, L"Dissimilarity & Configuration & Weight: To Configuration (ratio mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_ratio_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_sw_ratio")) return 0;
END

FORM (Dissimilarity_Configuration_Weight_interval_mds, L"Dissimilarity & Configuration & Weight: To Configuration (interval mds)", L"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_interval_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_sw_interval")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_monotone_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_sw_monotone")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Configuration_Weight_ispline_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_INTEGER (L"Number of interior knots"), 
		GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_sw_ispline")) return 0;
END

FORM (Dissimilarity_Configuration_getStress, L"Dissimilarity & Configuration: Get stress", 
	L"Dissimilarity & Configuration: get stress")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON (L"Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k]-dbar)^2))")
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_getStress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		GET_INTEGER (L"Handling of ties"), GET_INTEGER (L"Stress calculation"))));
END

FORM (Dissimilarity_Configuration_absolute_stress, 
	L"Dissimilarity & Configuration: Get stress (absolute mds)",
	L"Dissimilarity & Configuration: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_ratio_stress, L"Dissimilarity & Configuration: Get stress (ratio mds)",
	L"Dissimilarity & Configuration: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_interval_stress, 
	L"Dissimilarity & Configuration: Get stress (interval mds)",
	L"Dissimilarity & Configuration: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_interval_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
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
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
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
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL,
		GET_INTEGER (L"Number of interior knots"), 
		GET_INTEGER (L"Order of I-spline"), 
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_absolute_stress, 
	L"Dissimilarity & Configuration & Weight: Get stress (absolute mds)",
	L"Dissimilarity & Configuration & Weight: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_ratio_stress, 
	L"Dissimilarity & Configuration & Weight: Get stress (ratio mds)",
	L"Dissimilarity & Configuration & Weight: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_interval_stress, 
	L"Dissimilarity & Configuration & Weight: Get stress (interval mds)",
	L"Dissimilarity & Configuration & Weight: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_interval_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER (L"Stress measure"))));
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
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER (L"Handling of ties"), 
		GET_INTEGER (L"Stress measure"))));
END


FORM (Dissimilarity_Configuration_Weight_ispline_stress, 
	L"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)",
	L"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)...")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL,
		GET_INTEGER (L"Number of interior knots"), 
		GET_INTEGER (L"Order of I-spline"), 
		GET_INTEGER (L"Stress measure"))));
END

FORM (Dissimilarity_Configuration_drawShepardDiagram, L"Dissimilarity & Configuration: Draw Shepard diagram", 	
	L"Dissimilarity & Configuration: Draw Shepard diagram...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_drawShepardDiagram (ONLY (classDissimilarity),
		ONLY (classConfiguration), GRAPHICS, 
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"),
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawAbsoluteRegression, 
	L"Dissimilarity & Configuration: Draw regression (absolute mds)",
	L"Dissimilarity & Configuration: Draw regression (absolute mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawAbsoluteRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), 
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawRatioRegression, 
	L"Dissimilarity & Configuration: Draw regression (ratio mds)",
	L"Dissimilarity & Configuration: Draw regression (ratio mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawRatioRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawIntervalRegression, 
	L"Dissimilarity & Configuration: Draw regression (interval mds)",
	L"Dissimilarity & Configuration: Draw regression (interval mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawIntervalRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	praat_picture_close ();
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
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawMonotoneRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GRAPHICS, GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawISplineRegression, 
	L"Dissimilarity & Configuration: Draw regression (i-spline mds)",
	L"Dissimilarity & Configuration: Draw regression (i-spline mds)...")
	INTEGER (L"Number of interior knots", L"1")
	INTEGER (L"Order of I-spline", L"3")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawISplineRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS, 
		GET_INTEGER (L"Number of interior knots"), 
		GET_INTEGER (L"Order of I-spline"),
		GET_REAL (L"left Proximity range"), GET_REAL (L"right Proximity range"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), 
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_kruskal, L"Dissimilarity: To Configuration (kruskal)", 
	L"Dissimilarity: To Configuration (kruskal)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	NATURAL (L"Distance metric", L"2 (=Euclidean)")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	RADIO (L"Stress calculation", 1)
	RADIOBUTTON (L"Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON (L"Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k]-dbar)^2))")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	if (! praat_new1 (Dissimilarity_kruskal (dissimilarity,
		 GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Distance metric"), GET_INTEGER (L"Handling of ties"),
		GET_INTEGER (L"Stress calculation"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), 
		GET_INTEGER (L"Number of repetitions")),
		dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_absolute_mds, L"Dissimilarity: To Configuration (absolute mds)", 
	L"Dissimilarity: To Configuration (absolute mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_absolute_mds (dissimilarity, NULL, 
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_absolute")) return 0;
END

FORM (Dissimilarity_ratio_mds, L"Dissimilarity: To Configuration (ratio mds)", 
	L"Dissimilarity: To Configuration (ratio mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_ratio_mds (dissimilarity, NULL,
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_ratio")) return 0;
END

FORM (Dissimilarity_interval_mds, L"Dissimilarity: To Configuration (interval mds)", 
	L"Dissimilarity: To Configuration (interval mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_interval_mds (dissimilarity, NULL,
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_interval")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_monotone_mds (dissimilarity, NULL,
		GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_monotone")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	long niknots = GET_INTEGER (L"Number of interior knots");
	long order = GET_INTEGER (L"Order of I-spline");
	REQUIRE (order > 0 || niknots > 0, 
		L"Order-zero spline must at least have 1 interior knot.")
	if (! praat_new2 (Dissimilarity_Weight_ispline_mds (dissimilarity, NULL,
		GET_INTEGER (L"Number of dimensions"),
		niknots, order,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_ispline")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	long niknots = GET_INTEGER (L"Number of interior knots");
	long order = GET_INTEGER (L"Order of I-spline");
	REQUIRE (order > 0 || niknots > 0, 
		L"Order-zero spline must at least have 1 interior knot.")
	if (! praat_new2 (Dissimilarity_Weight_ispline_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER (L"Number of dimensions"), niknots, order,
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_ispline")) return 0;
END

FORM (Dissimilarity_Weight_absolute_mds, L"Dissimilarity & Weight: To Configuration (absolute mds)", 
	L"Dissimilarity & Weight: To Configuration (absolute mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_absolute_mds (dissimilarity,
		ONLY (classWeight),
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_absolute")) return 0;
END

FORM (Dissimilarity_Weight_ratio_mds, L"Dissimilarity & Weight: To Configuration (ratio mds)", 
	L"Dissimilarity & Weight: To Configuration (ratio mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_ratio_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_absolute")) return 0;
END

FORM (Dissimilarity_Weight_interval_mds, L"Dissimilarity & Weight: To Configuration (interval mds)", 
	L"Dissimilarity & Weight: To Configuration (interval mds)...")
	LABEL (L"", L"Configuration")
	NATURAL (L"Number of dimensions", L"2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_interval_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER (L"Number of dimensions"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_absolute")) return 0;
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
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new2 (Dissimilarity_Weight_monotone_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Handling of ties"),
		GET_REAL (L"Tolerance"), GET_INTEGER (L"Maximum number of iterations"),
		GET_INTEGER (L"Number of repetitions"), showProgress),
		dissimilarity -> name, L"_monotone")) return 0;
END

FORM (Dissimilarity_to_Distance, L"Dissimilarity: To Distance", L"Dissimilarity: To Distance...")
	BOOLEAN (L"Scale (additive constant)", 1)
	OK
DO
	EVERY_TO (Dissimilarity_to_Distance (OBJECT, GET_INTEGER (L"Scale")))
END

DIRECT (Dissimilarity_to_Weight)
	EVERY_TO (Dissimilarity_to_Weight (OBJECT))
END

/************************* Distance(s) ***************************************/

FORM (Distance_to_ScalarProduct, L"Distance: To ScalarProduct", L"Distance: To ScalarProduct...")
	BOOLEAN (L"Make sum of squares equal 1.0", 1)
	OK
DO
	EVERY_TO (Distance_to_ScalarProduct (OBJECT, 
		GET_INTEGER (L"Make sum of squares equal 1.0")))
END

DIRECT (Distance_to_Dissimilarity)
	EVERY_TO (Distance_to_Dissimilarity (OBJECT))
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
	Distances distances = Distances_create();
	Configuration cresult; Salience wresult;
	WHERE (SELECTED) (void) Collection_addItem (distances, OBJECT);
	Distances_indscal (distances, GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Normalize scalar products"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), 
		GET_INTEGER (L"Number of repetitions"),
		1, &cresult, &wresult);
	distances -> size = 0; forget (distances);
	if (! praat_new1 (cresult, L"indscal") ||
		! praat_new1 (wresult, L"indscal")) return 0;
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
	praat_picture_open ();
	Distance_and_Configuration_drawScatterDiagram (ONLY (classDistance),
		ONLY (classConfiguration), GRAPHICS, GET_REAL (L"Minimum x-distance"), 
		GET_REAL (L"Maximum x-distance"),
		GET_REAL (L"Minimum y-distance"), GET_REAL (L"Maximum y-distance"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), 
		GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (Distance_Configuration_indscal, L"Distance & Configuration: To Configuration (indscal)",
	L"Distance & Configuration: To Configuration (indscal)...")
	BOOLEAN (L"Normalize scalar products", 1)
	LABEL (L"", L"Minimization parameters")
	REAL (L"Tolerance", L"1e-5")
	NATURAL (L"Maximum number of iterations", L"100 (= each repetition)")
	OK
DO
	Distances distances = Distances_create();
	Configuration cresult; Salience wresult;
	Configuration configuration = ONLY (classConfiguration);
	WHERE (SELECTED)
	{
		if (CLASS == classDistance)
		{
			(void) Collection_addItem (distances, OBJECT);
		}
	}
	Distances_Configuration_indscal (distances, configuration,
		GET_INTEGER (L"Normalize scalar products"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), 1, &cresult, &wresult);
	distances -> size = 0; forget (distances);
	if (! praat_new1 (cresult, L"indscal") ||
		! praat_new1 (wresult, L"indscal")) return 0;
END

FORM (Distance_Configuration_vaf, L"Distance & Configuration: Get VAF", L"Distance & Configuration: Get VAF...")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	Distances distances = Distances_create();
	double vaf;
	Configuration configuration = ONLY (classConfiguration);
	WHERE (SELECTED)
	{
		if (CLASS == classDistance)
		{
			(void) Collection_addItem (distances, OBJECT);
		}
	}
	Distances_Configuration_vaf (distances, configuration, 
		GET_INTEGER (L"Normalize scalar products"), &vaf);
	distances -> size = 0;
	forget (distances);
	Melder_information1 (Melder_double (vaf));
END

FORM (Distance_Configuration_Salience_vaf, L"Distance & Configuration & Salience: Get VAF", L"Distance & Configuration & Salience: Get VAF...")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	Distances distances = Distances_create();
	double vaf;
	WHERE (SELECTED)
	{
		if (CLASS == classDistance)
		{
			(void) Collection_addItem (distances, OBJECT);
		}
	}
	Distances_Configuration_Salience_vaf (distances, ONLY (classConfiguration),
		ONLY (classSalience), GET_INTEGER (L"Normalize scalar products"), &vaf);
	Melder_information1 (Melder_double (vaf));
	distances -> size = 0; 
	forget (distances);
END

FORM (Dissimilarity_Configuration_Salience_vaf, L"Dissimilarity & Configuration & Salience: Get VAF",
	L"Dissimilarity & Configuration & Salience: Get VAF...")
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	BOOLEAN (L"Normalize scalar products", 1)
	OK
DO
	Dissimilarities dissimilarities = Dissimilarities_create(); 
	double vaf;
	WHERE (SELECTED)
	{
		if (CLASS == classDissimilarity)
		{
			(void) Collection_addItem (dissimilarities, OBJECT);
		}
	}
	Dissimilarities_Configuration_Salience_vaf (dissimilarities, 
		ONLY (classConfiguration),
		ONLY (classSalience), GET_INTEGER (L"Handling of ties"),
		GET_INTEGER (L"Normalize scalar products"), &vaf);
	Melder_information1 (Melder_double (vaf));
	dissimilarities -> size = 0; 
	forget (dissimilarities);
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
	Distances distances = Distances_create(); 
	int showprocess = 1; 
	double vaf;
	Configuration cresult; 
	Salience wresult;
	WHERE (SELECTED)
	{
		if (CLASS == classDistance)
		{
			(void) Collection_addItem (distances, OBJECT);
		}
	}
	Distances_Configuration_Salience_indscal (distances, 
		ONLY (classConfiguration), ONLY (classSalience),
		GET_INTEGER (L"Normalize scalar products"), GET_REAL (L"Tolerance"),
		GET_INTEGER (L"Maximum number of iterations"), showprocess, &cresult,
		&wresult, &vaf);
	distances -> size = 0;
	forget (distances);
	if (! praat_new1 (cresult, L"indscal") ||
		! praat_new1 (wresult, L"indscal")) return 0;
END


FORM (Distances_to_Configuration_ytl, L"Distance: To Configuration (ytl)", L"Distance: To Configuration (ytl)...")
	NATURAL (L"Number of dimensions", L"2")
	BOOLEAN (L"Normalize scalar products", 1)
	BOOLEAN (L"Salience object", 0)
	OK
DO
	Distances me = Distances_create();
	Configuration cresult; Salience wresult;
	WHERE (SELECTED) (void) Collection_addItem (me, OBJECT);
	Distances_to_Configuration_ytl (me, GET_INTEGER (L"Number of dimensions"),
		GET_INTEGER (L"Normalize scalar products"), &cresult, &wresult);
	my size = 0; 
	forget (me);
	if (! praat_new1 (cresult, NULL)) return 0;
	if (GET_INTEGER (L"Salience object"))
	{
		if (! praat_new1 (wresult, NULL)) return 0;
	}
	else forget (wresult);
END

FORM (Dissimilarity_Distance_monotoneRegression, L"Dissimilarity & Distance: Monotone regression", 0)
	RADIO (L"Handling of ties", 1)
	RADIOBUTTON (L"Primary approach")
	RADIOBUTTON (L"Secondary approach")
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	if (! praat_new1 (Dissimilarity_Distance_monotoneRegression (dissimilarity, 
		ONLY (classDistance),
		GET_INTEGER (L"Handling of ties")), dissimilarity -> name)) return 0;
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
	EVERY_DRAW (Proximity_Distance_drawScatterDiagram 
		(ONLY (classDissimilarity), ONLY (classDistance), GRAPHICS,
		GET_REAL (L"Minimum dissimilarity"), GET_REAL (L"Maximum dissimilarity"),
		GET_REAL (L"left Distance range"), GET_REAL (L"right Distance range"),
		GET_REAL (L"Mark size"), GET_STRING (L"Mark string"), 
		GET_INTEGER (L"Garnish")))
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
	EVERY_DRAW (Salience_draw (OBJECT, GRAPHICS,
		GET_INTEGER (L"Horizontal dimension"), GET_INTEGER (L"Vertical dimension"),
		GET_INTEGER (L"Garnish")))
END

/************************* COVARIANCE & CONFIGURATION  ********************/

FORM (Covariance_to_Configuration, L"Covariance: To Configuration", 0)
	NATURAL (L"Number of dimensions", L"2")
	OK
DO
	EVERY_TO (Covariance_to_Configuration (OBJECT, 
		GET_INTEGER (L"Number of dimensions")))
END

/********* Procrustes ***************************/

DIRECT (Procrustes_help)
	Melder_help (L"Procrustes"); 
END

DIRECT (Procrustes_getScale)
	Procrustes me = ONLY_OBJECT;
	Melder_information1 (Melder_double (my s));
END

/********* Casts from & to TableOfReal ***************************/

DIRECT (TableOfReal_to_Dissimilarity)
	EVERY_TO (TableOfReal_to_Dissimilarity (OBJECT))
END

DIRECT (TableOfReal_to_Similarity)
	EVERY_TO (TableOfReal_to_Similarity (OBJECT))
END

DIRECT (TableOfReal_to_Distance) 
	EVERY_TO (TableOfReal_to_Distance (OBJECT)) 
END

DIRECT (TableOfReal_to_Salience) 
	EVERY_TO (TableOfReal_to_Salience (OBJECT)) 
END

DIRECT (TableOfReal_to_Weight) 
	EVERY_TO (TableOfReal_to_Weight (OBJECT)) 
END

DIRECT (TableOfReal_to_ScalarProduct) 
	EVERY_TO (TableOfReal_to_ScalarProduct (OBJECT)) 
END

DIRECT (TableOfReal_to_Configuration) 
	EVERY_TO (TableOfReal_to_Configuration (OBJECT)) 
END

DIRECT (TableOfReal_to_ContingencyTable) 
EVERY_TO (TableOfReal_to_ContingencyTable (OBJECT)) 
END

/********************** TableOfReal ***************************************/

DIRECT (TableOfReal_getTableNorm)
	Melder_information1 (Melder_double (TableOfReal_getTableNorm (ONLY_OBJECT)));
END

FORM (TableOfReal_normalizeTable, L"TableOfReal: Normalize table", L"TableOfReal: Normalize table...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	EVERY (TableOfReal_normalizeTable (OBJECT, GET_REAL (L"Norm")))
END

FORM (TableOfReal_normalizeRows, L"TableOfReal: Normalize rows", L"TableOfReal: Normalize rows...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	EVERY (TableOfReal_normalizeRows (OBJECT, GET_REAL (L"Norm")))
END

FORM (TableOfReal_normalizeColumns, L"TableOfReal: Normalize columns", L"TableOfReal: Normalize columns...")
	POSITIVE (L"Norm", L"1.0")
	OK
DO
	EVERY (TableOfReal_normalizeColumns (OBJECT, GET_REAL (L"Norm")))
END

DIRECT (TableOfReal_centreRows)
	EVERY (TableOfReal_centreRows (OBJECT))
END

DIRECT (TableOfReal_centreColumns)
	EVERY (TableOfReal_centreColumns (OBJECT))
END

DIRECT (TableOfReal_doubleCentre)
	EVERY (TableOfReal_doubleCentre (OBJECT))
END

DIRECT (TableOfReal_standardizeRows)
	EVERY (TableOfReal_standardizeRows (OBJECT))
END

DIRECT (TableOfReal_standardizeColumns)
	EVERY (TableOfReal_standardizeColumns (OBJECT))
END

DIRECT (TableOfReal_to_Confusion)
	EVERY_TO (TableOfReal_to_Confusion (OBJECT))
END

static void praat_AffineTransform_init (void *klas)
{
	praat_addAction1 (klas, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (klas, 1, L"Get transformation element...", QUERY_BUTTON, 1, DO_AffineTransform_getTransformationElement);
	praat_addAction1 (klas, 1, L"Get translation element...", QUERY_BUTTON, 1, DO_AffineTransform_getTranslationElement);
	praat_addAction1 (klas, 0, L"Invert", 0, 0, DO_AffineTransform_invert);
}

static void praat_Configuration_and_AffineTransform_init (void *transform)
{
	praat_addAction2 (classConfiguration, 1, transform, 1, L"To Configuration", 0, 0, DO_Configuration_and_AffineTransform_to_Configuration);
}

static void praat_TableOfReal_extras (void *klas)
{
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

void praat_uvafon_MDS_init (void);
void praat_uvafon_MDS_init (void)
{
	Thing_recognizeClassesByName (classProcrustes,
		classContingencyTable, classDissimilarity,
		classSimilarity, classConfiguration, classDistance,
		classSalience, NULL);
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
	praat_addAction1 (classConfiguration, 0, L"To Similarity (cc)", 0, 0, DO_Configuration_to_Similarity_cc);

	praat_addAction1 (classConfiguration, 0, L"Match configurations - ", 0, 0, 0);
	praat_addAction1 (classConfiguration, 2, L"To Procrustes...", 0, 1, DO_Configurations_to_Procrustes);	
	praat_addAction1 (classConfiguration, 2, L"To AffineTransform (congruence)...", 0, 1, DO_Configurations_to_AffineTransform_congruence);

	praat_addAction1 (classConfusion, 0, L"To ContingencyTable", L"To Matrix", 0, DO_Confusion_to_ContingencyTable);
	praat_addAction1 (classConfusion, 0, L"To Proximity -       ", L"Analyse", 0, 0);
		praat_addAction1 (classConfusion, 0, L"To Dissimilarity (pdf)...", L"To Proximity -       ", 1, DO_Confusion_to_Dissimilarity_pdf);
		praat_addAction1 (classConfusion, 0, L"To Similarity...", L"To Proximity -       ", 1, DO_Confusion_to_Similarity);
		praat_addAction1 (classConfusion, 0, L"Sum", L"Synthesize -     ", 1, DO_Confusions_sum);


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
	praat_addAction1 (classDissimilarity, 0, L"Analyse", 0, 0, 0);
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
	praat_addAction1 (classDistance, 0, L"Analyse", 0, 0, 0);
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
	praat_addAction1 (classSimilarity, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classSimilarity, 0, L"To Dissimilarity...", 0, 0, DO_Similarity_to_Dissimilarity);


	praat_TableOfReal_init2 (classScalarProduct);
	praat_TableOfReal_extras (classScalarProduct);
   
	praat_TableOfReal_extras (classTableOfReal);
	praat_addAction1 (classTableOfReal, 1, L"Centre rows", L"Normalize table...", 1, DO_TableOfReal_centreRows);
	praat_addAction1 (classTableOfReal, 1, L"Centre columns", L"Centre rows", 1, DO_TableOfReal_centreColumns);
	praat_addAction1 (classTableOfReal, 1, L"Double centre", L"Centre columns", 1, DO_TableOfReal_doubleCentre);
	praat_addAction1 (classTableOfReal, 0, L"Cast -         ", 0, 0, 0);
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

/* End of file praat_MDS_init.c */
