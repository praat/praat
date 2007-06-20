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

static char *QUERY_BUTTON   = "Query -                ";
static char *CONVERT_BUTTON   = "Convert -              ";
static char *DRAW_BUTTON    = "Draw -                 ";
static char *MODIFY_BUTTON  = "Modify -               ";
static char *EXTRACT_BUTTON = "Extract - ";
static char *ANALYSE_BUTTON = "Analyse -              ";
static char *CONFIGURATION_BUTTON = "To Configuration -     ";
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
	if (nr < 6) return Melder_error ("TabelOfReal_sort2: we want at least 6 rows!!");
	if (rowtoindex < 1 || rowtoindex > 2) 
		return Melder_error	("TabelOfReal_sort2: rowtoindex <= 2");

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

FORM (TabelOfReal_testSorting, "TabelOfReal: Sort and index", "")
	NATURAL ("Row to index", "1")
	OK
DO
	if (! TabelOfReal_testSorting (ONLY_OBJECT, GET_INTEGER ("Row to index"))) return 0;
END
	
/************************* examples ***************************************/

FORM (Dissimilarity_createLetterRExample, "Create letter R example", 
	"Create letter R example...")
	LABEL ("", "For the monotone transformation on the distances")
	REAL("Noise range", "32.5")
	OK
DO
	(void) praat_new (Dissimilarity_createLetterRExample 
		(GET_REAL("Noise range")), NULL);
END

FORM (INDSCAL_createCarrollWishExample, 
	"Create INDSCAL Carroll & Wish example...",
	"Create INDSCAL Carroll & Wish example...")
	REAL("Noise standard deviation", "0.0")
	OK
DO
	(void) praat_new (INDSCAL_createCarrollWishExample 
		(GET_REAL("Noise standard deviation")), NULL);
END

FORM (Configuration_create, "Create Configuration", "Create Configuration...")
	WORD ("Name", "uniform")
	NATURAL ("Number of points", "10")
	NATURAL ("Number of dimensions", "2")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "randomUniform(-1.5, 1.5)")
	OK
DO
	Configuration c = Configuration_create (GET_INTEGER ("Number of points"),
		GET_INTEGER ("Number of dimensions"));
	if (! c || ! praat_new (c, GET_STRING ("Name"))) return 0;
	if (! TableOfReal_formula (c, GET_STRING ("formula"), NULL)) return 0;
END

FORM (drawSplines, "Draw splines", "spline")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "1.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "20.0")
	RADIO ("Spline type", 1)
	RADIOBUTTON ("M-spline")
	RADIOBUTTON ("I-spline")
	INTEGER ("Order", "3")
	SENTENCE ("Interior knots", "0.3 0.5 0.6")
	BOOLEAN ("Garnish", 1)
	OK
DO
	double xmin = GET_REAL ("left Horizontal range"), xmax = GET_REAL ("right Horizontal range");
	double ymin = GET_REAL ("left Vertical range"), ymax = GET_REAL ("right Vertical range");
	REQUIRE (xmin < xmax && ymin < ymax, 
		"required: xmin < xmax and ymin < ymax")
	praat_picture_open ();
	drawSplines (GRAPHICS,xmin, xmax, ymin, ymax, GET_INTEGER ("Spline type"),
		GET_INTEGER ("Order"), GET_STRING ("Interior knots"), 
		GET_INTEGER ("Garnish"));
	praat_picture_close ();
END

DIRECT (drawMDSClassRelations)
	praat_picture_open ();
	drawMDSClassRelations (GRAPHICS);
	praat_picture_close ();
END


/***************** AffineTransform ***************************************/


DIRECT (AffineTransform_help)
	Melder_help ("AffineTransform"); 
END

DIRECT (AffineTransform_invert)
	EVERY_CHECK (praat_new (AffineTransform_invert (OBJECT), 
		"%s_inverse", NAME))	 
END

FORM (AffineTransform_getTransformationElement, "AffineTransform: Get transformation element", "Procrustes")
	NATURAL ("Row number", "1")
	NATURAL ("Column number", "1")
	OK
DO
	AffineTransform me = ONLY_OBJECT;
	long row = GET_INTEGER ("Row number");
	long column = GET_INTEGER ("Column number");
	REQUIRE (row <= my n, "Row number must not exceed number of rows.")
	REQUIRE (column <= my n, "Column number must not exceed number of columns.")
	Melder_information1 (Melder_double (my r [row] [column]));
END

FORM (AffineTransform_getTranslationElement, "AffineTransform: Get translation element", "Procrustes")
	NATURAL ("Index", "1")
	OK
DO
	AffineTransform me = ONLY_OBJECT;
	long number = GET_INTEGER ("Index");
	REQUIRE (number <= my n, "Index must not exceed number of elements.")
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
	Melder_help ("Configuration"); 
END

static void Configuration_draw_addCommonFields (void *dia)
{
	NATURAL ("Horizontal dimension", "1")
	NATURAL ("Vertical dimension", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
}

FORM (Configuration_draw, "Configuration: Draw", "Configuration: Draw...")
	Configuration_draw_addCommonFields (dia);
	NATURAL ("Label size", "12")
	BOOLEAN ("Use row labels", 0)
	WORD ("Label", "+")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_draw (OBJECT, GRAPHICS,
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"), GET_REAL ("left Vertical range"),
		GET_REAL ("right Vertical range"), GET_INTEGER ("Label size"), 
		GET_INTEGER ("Use row labels"), GET_STRING ("Label"),
		GET_INTEGER("Garnish")))
END

FORM (Configuration_drawSigmaEllipses, "Configuration: Draw sigma ellipses",
	"Configuration: Draw sigma ellipses...")
	POSITIVE ("Number of sigmas", "1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Number of sigmas"), 0, NULL,
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

FORM (Configuration_drawOneSigmaEllipse, "Configuration: Draw one sigma ellipse",
	"Configuration: Draw sigma ellipses...")
	SENTENCE ("Label", "")
	POSITIVE ("Number of sigmas", "1.0")
	Configuration_draw_addCommonFields (dia);
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Number of sigmas"), 0, GET_STRING ("Label"),
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END


FORM (Configuration_drawConfidenceEllipses, "Configuration: Draw confidence ellipses", 0)
	POSITIVE ("Confidence level (0-1)", "0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Confidence level"), 1, NULL,
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

FORM (Configuration_drawOneConfidenceEllipse, "Configuration: Draw one confidence ellipse", 0)
	SENTENCE ("Label", "")
	POSITIVE ("Confidence level (0-1)", "0.95")
	Configuration_draw_addCommonFields (dia);
	INTEGER ("Label size", "12")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Configuration_drawConcentrationEllipses (OBJECT, GRAPHICS,
		GET_REAL ("Confidence level"), 1, GET_STRING ("Label"),
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_REAL ("left Horizontal range"), GET_REAL ("right Horizontal range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"),
		GET_INTEGER ("Label size"), GET_INTEGER ("Garnish")))
END

DIRECT (Configuration_randomize)
	EVERY (Configuration_randomize (OBJECT))
END

FORM (Configuration_normalize, "Configuration: Normalize", 
	"Configuration: Normalize...")
	REAL ("Sum of squares", "0.0")
	LABEL ("", "On (INDSCAL), Off (Kruskal)")
	BOOLEAN ("Each dimension separately", 1)
	OK
DO
	EVERY (Configuration_normalize (OBJECT, GET_REAL ("Sum of squares"),
		GET_INTEGER ("Each dimension separately")))
END

DIRECT (Configuration_centralize)
	EVERY (TableOfReal_centreColumns (OBJECT))
END

FORM (Configuration_rotate, "Configuration: Rotate", 
	"Configuration: Rotate...")
	NATURAL ("Dimension 1", "1")
	NATURAL ("Dimension 2", "2")
	REAL ("Angle (degrees)", "60.0")
	OK
DO
	EVERY (Configuration_rotate (OBJECT, GET_INTEGER ("Dimension 1"),
		GET_INTEGER ("Dimension 2"), GET_REAL ("Angle")))
END

DIRECT (Configuration_rotateToPrincipalDirections)
	EVERY (Configuration_rotateToPrincipalDirections (OBJECT))
END

FORM (Configuration_invertDimension, "Configuration: Invert dimension", 
	"Configuration: Invert dimension...")
	NATURAL ("Dimension", "1")
	OK
DO
	EVERY (Configuration_invertDimension (OBJECT, GET_INTEGER ("Dimension")))
END

DIRECT (Configuration_to_Distance)
	EVERY_TO (Configuration_to_Distance (OBJECT))
END

FORM (Configuration_varimax, "Configuration: To Configuration (varimax)", 
	"Configuration: To Configuration (varimax)...")
	BOOLEAN ("Normalize rows", 1)
	BOOLEAN ("Quartimax", 0)
	NATURAL ("Maximum number of iterations", "50")
	POSITIVE ("Tolerance", "1e-6")
	OK
DO
	EVERY_TO (Configuration_varimax (OBJECT, GET_INTEGER ("Normalize rows"),
		GET_INTEGER ("Quartimax"),
		GET_INTEGER ("Maximum number of iterations"), GET_REAL ("Tolerance")))
END

DIRECT (Configuration_to_Similarity_cc)
	Configurations cs = Configurations_create();
	Similarity s = NULL;
	WHERE (SELECTED) (void) Collection_addItem (cs, OBJECT);
	s = Configurations_to_Similarity_cc (cs, NULL);
	cs -> size = 0; forget (cs);
	if (! praat_new (s, "congruence")) return 0;
END

FORM (Configurations_to_Procrustes, "Configuration & Configuration: To Procrustes",
	"Configuration & Configuration: To Procrustes...")
	BOOLEAN ("Orthogonal transform", 0)
	OK
DO
	Configuration c1 = NULL, c2 = NULL;
	WHERE (SELECTED) { if (c1) c2 = OBJECT; else c1 = OBJECT; }	
	if (! praat_new (Configurations_to_Procrustes (c1, c2, GET_INTEGER ("Orthogonal transform")), 
		"%s_to_%s", Thing_getName (c2), Thing_getName (c1))) return 0;
END

FORM (Configurations_to_AffineTransform_congruence, "Configurations: To AffineTransform (congruence)",
	"Configurations: To AffineTransform (congruence)...")
	NATURAL ("Maximum number of iterations", "50")
	POSITIVE ("Tolerance", "1e-6")
	OK
DO
	Configuration c1 = NULL, c2 = NULL;
	WHERE (SELECTED && CLASS == classConfiguration)
	{
		if (c1) c2 = OBJECT; 
		else c1 = OBJECT;
	}	
	NEW (Configurations_to_AffineTransform_congruence (c1, c2,
		GET_INTEGER ("Maximum number of iterations"),
		GET_REAL ("Tolerance")))
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
	if (! praat_new (s, "congruence")) return 0;
END

DIRECT (Configuration_and_AffineTransform_to_Configuration)
	NEW (Configuration_and_AffineTransform_to_Configuration 
		(ONLY (classConfiguration), ONLY_GENERIC (classAffineTransform)))
END

/*************** Confusion *********************************/

FORM (Confusion_to_Dissimilarity_pdf, "Confusion: To Dissimilarity (pdf)", 
	"Confusion: To Dissimilarity (pdf)...")
	POSITIVE ("Minimum confusion level", "0.5")
	OK
DO
	WHERE (SELECTED)
	{
		Confusion c = OBJECT;
		if (! praat_new (Confusion_to_Dissimilarity_pdf (OBJECT,
			GET_REAL ("Minimum confusion level")), "%s_pdf", c -> name))
				return 0;
	}
END

FORM (Confusion_to_Similarity, "Confusion: To Similarity", "Confusion: To Similarity...")
	BOOLEAN ("Normalize", 1)
	RADIO ("Symmetrization", 1)
	RADIOBUTTON ("No symmetrization")
	RADIOBUTTON ("Average (s[i][j] = (c[i][j]+c[j][i])/2)")
	RADIOBUTTON ("Houtgast (s[i][j]= sum (min(c[i][k],c[j][k])))")
	OK
DO
	EVERY_TO (Confusion_to_Similarity (OBJECT, GET_INTEGER ("Normalize"),
		GET_INTEGER ("Symmetrization")))
END

DIRECT (Confusions_sum)
	Confusions me = Confusions_create(); Confusion conf = NULL;
	WHERE (SELECTED) (void) Collection_addItem (me, OBJECT);
	conf = Confusions_sum (me);
	my size = 0; forget (me);
	if (! praat_new (conf, "untitled_sum")) return 0;
END

DIRECT (Confusion_to_ContingencyTable)
	EVERY_TO (Confusion_to_ContingencyTable (OBJECT))
END

/*************** ContingencyTable *********************************/


FORM (ContingencyTable_to_Configuration_ca, "ContingencyTable: To Configuration (ca)",
	"ContingencyTable: To Configuration (ca)...")
	NATURAL ("Number of dimensions", "2")
	RADIO ("Scaling of final configuration", 3)
	RADIOBUTTON ("Row points in centre of gravity of column points")
	RADIOBUTTON ("Column points in centre of gravity of row points")
	RADIOBUTTON ("Row points and column points symmetric")
	OK
DO
	EVERY_TO (ContingencyTable_to_Configuration_ca (OBJECT, 
		GET_INTEGER ("Number of dimensions"),
		GET_INTEGER ("Scaling of final configuration")))
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

FORM (Correlation_to_Configuration, "Correlation: To Configuration", 0)
	NATURAL ("Number of dimensions", "2")
	OK
DO
	EVERY_TO (Correlation_to_Configuration (OBJECT, 
		GET_INTEGER ("Number of dimensions")))
END


/************************* Similarity ***************************************/

DIRECT (Similarity_help)
	Melder_help ("Similarity"); 
END

FORM (Similarity_to_Dissimilarity, "Similarity: To Dissimilarity", 
	"Similarity: To Dissimilarity...")
	REAL ("Maximum dissimilarity", "0.0 (=from data)")
	OK
DO
	EVERY_TO (Similarity_to_Dissimilarity (OBJECT, GET_REAL ("Maximum dissimilarity")))
END

/**************** Dissimilarity ***************************************/

static void Dissimilarity_to_Configuration_addCommonFields (void *dia)
{
	LABEL("", "Minimization parameters")
	REAL ("Tolerance", "1e-5")
	NATURAL ("Maximum number of iterations", "50 (= each repetition)")
	NATURAL ("Number of repetitions", "1")
}

static void Dissimilarity_and_Configuration_getStress_addCommonFields (void *dia, void *radio)
{
	RADIO ("Stress measure", 1)
	RADIOBUTTON ("Normalized")
	RADIOBUTTON ("Kruskal's stress-1")
	RADIOBUTTON ("Kruskal's stress-2")
	RADIOBUTTON ("Raw")
}

static void Dissimilarity_Configuration_drawDiagram_addCommonFields (void *dia)
{
	REAL ("left Proximity range", "0.0")
	REAL ("right Proximity range", "0.0")
	REAL ("left Distance range", "0.0")
	REAL ("right Distance range", "0.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Garnish", 1)
}

DIRECT (Dissimilarity_help)
	Melder_help ("Dissimilarity");
END

DIRECT (Dissimilarity_getAdditiveConstant)
	double c;
	Dissimilarity_getAdditiveConstant(ONLY_OBJECT, &c);
	Melder_information1 (Melder_double (c));
END

FORM (Dissimilarity_Configuration_kruskal, "Dissimilarity & Configuration: To Configuration (kruskal)", 
	"Dissimilarity & Configuration: To Configuration (kruskal)...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	RADIO ("Stress calculation", 1)
	RADIOBUTTON ("Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON ("Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / "
		"SUM((dist[k]-dbar)^2))")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Configuration conf = ONLY (classConfiguration);
	if (! praat_new (Dissimilarity_Configuration_kruskal 
		(ONLY (classDissimilarity), conf,
		GET_INTEGER ("Handling of ties"), GET_INTEGER ("Stress calculation"),
		GET_REAL ("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions")), 
		"%s_s_kruskal", conf -> name)) return 0;
END

FORM (Dissimilarity_Configuration_absolute_mds, 
	"Dissimilarity & Configuration: To Configuration (absolute mds)",
	"Dissimilarity & Configuration: To Configuration (absolute mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_absolute_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_s_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_ratio_mds, "Dissimilarity & Configuration: To Configuration (ratio mds)",
	"Dissimilarity & Configuration: To Configuration (ratio mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_ratio_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_s_ratio", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_interval_mds, "Dissimilarity & Configuration: To Configuration (interval mds)",
	"Dissimilarity & Configuration: To Configuration (interval mds)...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_interval_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_s_interval", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_monotone_mds, 
	"Dissimilarity & Configuration: To Configuration (monotone mds)",
	"Dissimilarity & Configuration: To Configuration (monotone mds)...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_monotone_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Handling of ties"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_s_monotone", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_ispline_mds, 
	"Dissimilarity & Configuration: To Configuration (i-spline mds)",
	"Dissimilarity & Configuration: To Configuration (i-spline mds)...")
	LABEL ("", "Spline smoothing")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_ispline_mds 
		(dissimilarity, ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Number of interior knots"), 
		GET_INTEGER ("Order of I-spline"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_s_ispline", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_Weight_absolute_mds, 
	"Dissimilarity & Configuration & Weight: To Configuration (absolute mds)",
	"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_absolute_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_sw_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_Weight_ratio_mds, 
	"Dissimilarity & Configuration & Weight: To Configuration (ratio mds)",
	"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_ratio_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_sw_ratio", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_Weight_interval_mds, 
	"Dissimilarity & Configuration & Weight: To Configuration (interval mds)",
	"Dissimilarity & Configuration & Weight: To Configuration...")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_interval_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_sw_interval", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_Weight_monotone_mds, 
	"Dissimilarity & Configuration & Weight: To Configuration (monotone mds)",
	"Dissimilarity & Configuration & Weight: To Configuration...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_monotone_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_INTEGER ("Handling of ties"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_sw_monotone", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_Weight_ispline_mds, 
	"Dissimilarity & Configuration & Weight: To Configuration (i-spline mds)",
	"Dissimilarity & Configuration & Weight: To Configuration...")
	LABEL ("", "Spline smoothing")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Configuration_Weight_ispline_mds 
		(dissimilarity, ONLY (classConfiguration), ONLY (classWeight), 
		GET_INTEGER ("Number of interior knots"), 
		GET_INTEGER ("Order of I-spline"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_sw_ispline", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Configuration_getStress, "Dissimilarity & Configuration: Get stress", 
	"Dissimilarity & Configuration: get stress")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	RADIO ("Stress calculation", 1)
	RADIOBUTTON ("Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON ("Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / "
		"SUM((dist[k]-dbar)^2))")
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_getStress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		GET_INTEGER ("Handling of ties"), GET_INTEGER ("Stress calculation"))));
END

FORM (Dissimilarity_Configuration_absolute_stress, 
	"Dissimilarity & Configuration: Get stress (absolute mds)",
	"Dissimilarity & Configuration: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_ratio_stress, "Dissimilarity & Configuration: Get stress (ratio mds)",
	"Dissimilarity & Configuration: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_interval_stress, 
	"Dissimilarity & Configuration: Get stress (interval mds)",
	"Dissimilarity & Configuration: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_interval_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_monotone_stress, 
	"Dissimilarity & Configuration: Get stress (monotone mds)",
	"Dissimilarity & Configuration: Get stress (monotone mds)...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GET_INTEGER ("Handling of ties"), GET_INTEGER ("Stress measure"))));
END


FORM (Dissimilarity_Configuration_ispline_stress, 
	"Dissimilarity & Configuration: Get stress (i-spline mds)",
	"Dissimilarity & Configuration: Get stress (i-spline mds)...")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL,
		GET_INTEGER ("Number of interior knots"), 
		GET_INTEGER ("Order of I-spline"), 
		GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_absolute_stress, 
	"Dissimilarity & Configuration & Weight: Get stress (absolute mds)",
	"Dissimilarity & Configuration & Weight: Get stress (absolute mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_absolute_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_ratio_stress, 
	"Dissimilarity & Configuration & Weight: Get stress (ratio mds)",
	"Dissimilarity & Configuration & Weight: Get stress (ratio mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ratio_stress
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_interval_stress, 
	"Dissimilarity & Configuration & Weight: Get stress (interval mds)",
	"Dissimilarity & Configuration & Weight: Get stress (interval mds)...")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_interval_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_Weight_monotone_stress, 
	"Dissimilarity & Configuration & Weight: Get stress (monotone mds)",
	"Dissimilarity & Configuration & Weight: Get stress (monotone mds)...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach)")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_monotone_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), 
		ONLY (classWeight), GET_INTEGER ("Handling of ties"), 
		GET_INTEGER ("Stress measure"))));
END


FORM (Dissimilarity_Configuration_Weight_ispline_stress, 
	"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)",
	"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)...")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (dia, radio);
	OK
DO
	Melder_information1 (Melder_double (Dissimilarity_Configuration_Weight_ispline_stress 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL,
		GET_INTEGER ("Number of interior knots"), 
		GET_INTEGER ("Order of I-spline"), 
		GET_INTEGER ("Stress measure"))));
END

FORM (Dissimilarity_Configuration_drawShepardDiagram, "Dissimilarity & Configuration: Draw Shepard diagram", 	
	"Dissimilarity & Configuration: Draw Shepard diagram...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_drawShepardDiagram (ONLY (classDissimilarity),
		ONLY (classConfiguration), GRAPHICS, 
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"),
		GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawAbsoluteRegression, 
	"Dissimilarity & Configuration: Draw regression (absolute mds)",
	"Dissimilarity & Configuration: Draw regression (absolute mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawAbsoluteRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), 
		GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawRatioRegression, 
	"Dissimilarity & Configuration: Draw regression (ratio mds)",
	"Dissimilarity & Configuration: Draw regression (ratio mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawRatioRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawIntervalRegression, 
	"Dissimilarity & Configuration: Draw regression (interval mds)",
	"Dissimilarity & Configuration: Draw regression (interval mds)...")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawIntervalRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS,
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawMonotoneRegression, 
	"Dissimilarity & Configuration: Draw regression (monotone mds)",
	"Dissimilarity & Configuration: Draw regression (monotone mds)...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach)")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawMonotoneRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, 
		GRAPHICS, GET_INTEGER ("Handling of ties"),
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_Configuration_drawISplineRegression, 
	"Dissimilarity & Configuration: Draw regression (i-spline mds)",
	"Dissimilarity & Configuration: Draw regression (i-spline mds)...")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "3")
	Dissimilarity_Configuration_drawDiagram_addCommonFields (dia);
	OK
DO
	praat_picture_open ();
	Dissimilarity_Configuration_Weight_drawISplineRegression 
		(ONLY (classDissimilarity), ONLY (classConfiguration), NULL, GRAPHICS, 
		GET_INTEGER ("Number of interior knots"), 
		GET_INTEGER ("Order of I-spline"),
		GET_REAL("left Proximity range"), GET_REAL("right Proximity range"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), 
		GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Dissimilarity_kruskal, "Dissimilarity: To Configuration (kruskal)", 
	"Dissimilarity: To Configuration (kruskal)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	NATURAL ("Distance metric", "2 (=Euclidean)")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	RADIO ("Stress calculation", 1)
	RADIOBUTTON ("Formula1 (sqrt (SUM((dist[k]-fit[k])^2) / SUM((dist[k])^2))")
	RADIOBUTTON ("Formula2 (sqrt (SUM((dist[k]-fit[k])^2) / "
		"SUM((dist[k]-dbar)^2))")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	if (! praat_new (Dissimilarity_kruskal (dissimilarity,
		 GET_INTEGER ("Number of dimensions"),
		GET_INTEGER ("Distance metric"), GET_INTEGER ("Handling of ties"),
		GET_INTEGER ("Stress calculation"), GET_REAL("Tolerance"),
		GET_INTEGER ("Maximum number of iterations"), 
		GET_INTEGER ("Number of repetitions")),
		dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_absolute_mds, "Dissimilarity: To Configuration (absolute mds)", 
	"Dissimilarity: To Configuration (absolute mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_absolute_mds (dissimilarity, NULL, 
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_ratio_mds, "Dissimilarity: To Configuration (ratio mds)", 
	"Dissimilarity: To Configuration (ratio mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_ratio_mds (dissimilarity, NULL,
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_ratio", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_interval_mds, "Dissimilarity: To Configuration (interval mds)", 
	"Dissimilarity: To Configuration (interval mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_interval_mds (dissimilarity, NULL,
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_interval", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_monotone_mds, "Dissimilarity: To Configuration (monotone mds)", 
	"Dissimilarity: To Configuration (monotone mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_monotone_mds (dissimilarity, NULL,
		GET_INTEGER ("Number of dimensions"),
		GET_INTEGER ("Handling of ties"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_monotone", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_ispline_mds, "Dissimilarity: To Configuration (i-spline mds)", 
	"Dissimilarity: To Configuration (i-spline mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	LABEL ("", "Spline smoothing")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	long niknots = GET_INTEGER ("Number of interior knots");
	long order = GET_INTEGER ("Order of I-spline");
	REQUIRE (order > 0 || niknots > 0, 
		"Order-zero spline must at least have 1 interior knot.")
	if (! praat_new (Dissimilarity_Weight_ispline_mds (dissimilarity, NULL,
		GET_INTEGER ("Number of dimensions"),
		niknots, order,
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_ispline", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Weight_ispline_mds, "Dissimilarity & Weight: To Configuration (i-spline mds)", 
	"Dissimilarity & Weight: To Configuration (i-spline mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	LABEL ("", "Spline smoothing")
	INTEGER ("Number of interior knots", "1")
	INTEGER ("Order of I-spline", "1")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	long niknots = GET_INTEGER ("Number of interior knots");
	long order = GET_INTEGER ("Order of I-spline");
	REQUIRE (order > 0 || niknots > 0, 
		"Order-zero spline must at least have 1 interior knot.")
	if (! praat_new (Dissimilarity_Weight_ispline_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER ("Number of dimensions"), niknots, order,
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_ispline", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Weight_absolute_mds, "Dissimilarity & Weight: To Configuration (absolute mds)", 
	"Dissimilarity & Weight: To Configuration (absolute mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_absolute_mds (dissimilarity,
		ONLY (classWeight),
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Weight_ratio_mds, "Dissimilarity & Weight: To Configuration (ratio mds)", 
	"Dissimilarity & Weight: To Configuration (ratio mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_ratio_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Weight_interval_mds, "Dissimilarity & Weight: To Configuration (interval mds)", 
	"Dissimilarity & Weight: To Configuration (interval mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_interval_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER ("Number of dimensions"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_absolute", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_Weight_monotone_mds, "Dissimilarity & Weight: To Configuration (monotone mds)", 
	"Dissimilarity & Weight: To Configuration (monotone mds)...")
	LABEL ("", "Configuration")
	NATURAL ("Number of dimensions", "2")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	Dissimilarity_to_Configuration_addCommonFields (dia);
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity); 
	int showProgress = 1;
	if (! praat_new (Dissimilarity_Weight_monotone_mds (dissimilarity, 
		ONLY (classWeight),
		GET_INTEGER ("Number of dimensions"),
		GET_INTEGER ("Handling of ties"),
		GET_REAL("Tolerance"), GET_INTEGER ("Maximum number of iterations"),
		GET_INTEGER ("Number of repetitions"), showProgress),
		"%s_monotone", dissimilarity -> name)) return 0;
END

FORM (Dissimilarity_to_Distance, "Dissimilarity: To Distance", "Dissimilarity: To Distance...")
	BOOLEAN ("Scale (additive constant)", 1)
	OK
DO
	EVERY_TO (Dissimilarity_to_Distance (OBJECT, GET_INTEGER ("Scale")))
END

DIRECT (Dissimilarity_to_Weight)
	EVERY_TO (Dissimilarity_to_Weight (OBJECT))
END

/************************* Distance(s) ***************************************/

FORM (Distance_to_ScalarProduct, "Distance: To ScalarProduct", 
	"Distance: To ScalarProduct...")
	BOOLEAN ("Make sum of squares equal 1.0", 1)
	OK
DO
	EVERY_TO (Distance_to_ScalarProduct (OBJECT, 
		GET_INTEGER ("Make sum of squares equal 1.0")))
END

DIRECT (Distance_to_Dissimilarity)
	EVERY_TO (Distance_to_Dissimilarity (OBJECT))
END

FORM (Distances_indscal, "Distance: To Configuration (indscal)", 
	"Distance: To Configuration (indscal)...")
	NATURAL ("Number of dimensions", "2")
	BOOLEAN ("Normalize scalar products", 1)
	LABEL("", "Minimization parameters")
	REAL ("Tolerance", "1e-5")
	NATURAL ("Maximum number of iterations", "100 (= each repetition)")
	NATURAL ("Number of repetitions", "1")
	OK
DO
	Distances distances = Distances_create();
	Configuration cresult; Salience wresult;
	WHERE (SELECTED) (void) Collection_addItem (distances, OBJECT);
	Distances_indscal (distances, GET_INTEGER("Number of dimensions"),
		GET_INTEGER("Normalize scalar products"), GET_REAL("Tolerance"),
		GET_INTEGER("Maximum number of iterations"), 
		GET_INTEGER("Number of repetitions"),
		1, &cresult, &wresult);
	distances -> size = 0; forget (distances);
	if (! praat_new (cresult, "indscal") ||
		! praat_new (wresult, "indscal")) return 0;
END

FORM (Distance_and_Configuration_drawScatterDiagram, "Distance & Configuration: Draw scatter diagram",
	"Distance & Configuration: Draw scatter diagram...")
	REAL ("Minimum x-distance", "0.0")
	REAL ("Maximum x-distance", "0.0")
	REAL ("Minimum y-distance", "0.0")
	REAL ("Maximum y-distance", "0.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	Distance_and_Configuration_drawScatterDiagram (ONLY (classDistance),
		ONLY (classConfiguration), GRAPHICS, GET_REAL("Minimum x-distance"), 
		GET_REAL("Maximum x-distance"),
		GET_REAL("Minimum y-distance"), GET_REAL("Maximum y-distance"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), 
		GET_INTEGER("Garnish"));
	praat_picture_close ();
END

FORM (Distance_Configuration_indscal, "Distance & Configuration: To Configuration (indscal)",
	"Distance & Configuration: To Configuration (indscal)...")
	BOOLEAN ("Normalize scalar products", 1)
	LABEL("", "Minimization parameters")
	REAL ("Tolerance", "1e-5")
	NATURAL ("Maximum number of iterations", "100 (= each repetition)")
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
		GET_INTEGER ("Normalize scalar products"), GET_REAL ("Tolerance"),
		GET_INTEGER ("Maximum number of iterations"), 1, &cresult, &wresult);
	distances -> size = 0; forget (distances);
	if (! praat_new (cresult, "indscal") ||
		! praat_new (wresult, "indscal")) return 0;
END

FORM (Distance_Configuration_vaf, "Distance & Configuration: Get VAF", 
	"Distance & Configuration: Get VAF...")
	BOOLEAN ("Normalize scalar products", 1)
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
		GET_INTEGER ("Normalize scalar products"), &vaf);
	distances -> size = 0;
	forget (distances);
	Melder_information1 (Melder_double (vaf));
END

FORM (Distance_Configuration_Salience_vaf, "Distance & Configuration & Salience: Get VAF", 
	"Distance & Configuration & Salience: Get VAF...")
	BOOLEAN ("Normalize scalar products", 1)
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
		ONLY (classSalience), GET_INTEGER ("Normalize scalar products"), &vaf);
	Melder_information1 (Melder_double (vaf));
	distances -> size = 0; 
	forget (distances);
END

FORM (Dissimilarity_Configuration_Salience_vaf, "Dissimilarity & Configuration & Salience: Get VAF",
	"Dissimilarity & Configuration & Salience: Get VAF...")
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	BOOLEAN ("Normalize scalar products", 1)
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
		ONLY (classSalience), GET_INTEGER ("Handling of ties"),
		GET_INTEGER ("Normalize scalar products"), &vaf);
	Melder_information1 (Melder_double (vaf));
	dissimilarities -> size = 0; 
	forget (dissimilarities);
END

FORM (Distance_Configuration_Salience_indscal,
	"Distance & Configuration & Salience: To Configuration (indscal)", 
	"Distance & Configuration & Salience: To Configuration (indscal)...")
	BOOLEAN ("Normalize scalar products", 1)
	LABEL("", "Minimization parameters")
	REAL ("Tolerance", "1e-5")
	NATURAL ("Maximum number of iterations", "100")
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
		GET_INTEGER ("Normalize scalar products"), GET_REAL ("Tolerance"),
		GET_INTEGER ("Maximum number of iterations"), showprocess, &cresult,
		&wresult, &vaf);
	distances -> size = 0;
	forget (distances);
	if (! praat_new (cresult, "indscal") ||
		! praat_new (wresult, "indscal")) return 0;
END


FORM (Distances_to_Configuration_ytl, 
	"Distance: To Configuration (ytl)",
	"Distance: To Configuration (ytl)...")
	NATURAL ("Number of dimensions", "2")
	BOOLEAN ("Normalize scalar products", 1)
	BOOLEAN ("Salience object", 0)
	OK
DO
	Distances me = Distances_create();
	Configuration cresult; Salience wresult;
	WHERE (SELECTED) (void) Collection_addItem (me, OBJECT);
	Distances_to_Configuration_ytl (me, GET_INTEGER("Number of dimensions"),
		GET_INTEGER("Normalize scalar products"), &cresult, &wresult);
	my size = 0; 
	forget (me);
	if (! praat_new (cresult, NULL)) return 0;
	if (GET_INTEGER ("Salience object"))
	{
		if (! praat_new (wresult, NULL)) return 0;
	}
	else forget (wresult);
END

FORM (Dissimilarity_Distance_monotoneRegression, "Dissimilarity & Distance: Monotone regression", 0)
	RADIO ("Handling of ties", 1)
	RADIOBUTTON ("Primary approach")
	RADIOBUTTON ("Secondary approach")
	OK
DO
	Dissimilarity dissimilarity = ONLY (classDissimilarity);
	if (! praat_new (Dissimilarity_Distance_monotoneRegression (dissimilarity, 
		ONLY (classDistance),
		GET_INTEGER ("Handling of ties")), dissimilarity -> name)) return 0;
END

FORM (Distance_Dissimilarity_drawShepardDiagram, 
	"Distance & Dissimilarity: Draw Shepard diagram", 0)
	REAL ("Minimum dissimilarity", "0.0")
	REAL ("Maximum dissimilarity", "0.0")
	REAL ("left Distance range", "0.0")
	REAL ("right Distance range", "0.0")
	POSITIVE("Mark size (mm)", "1.0")
	SENTENCE ("Mark string (+xo.)", "+")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Proximity_Distance_drawScatterDiagram 
		(ONLY (classDissimilarity), ONLY (classDistance), GRAPHICS,
		GET_REAL("Minimum dissimilarity"), GET_REAL("Maximum dissimilarity"),
		GET_REAL("left Distance range"), GET_REAL("right Distance range"),
		GET_REAL ("Mark size"), GET_STRING ("Mark string"), 
		GET_INTEGER("Garnish")))
END
	
DIRECT (MDS_help)
	Melder_help ("Multidimensional scaling");
END
	

/************************* Salience ***************************************/


FORM (Salience_draw, "Salience: Draw", 0)
	NATURAL ("Horizontal dimension", "1")
	NATURAL ("Vertical dimension", "2")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Salience_draw (OBJECT, GRAPHICS,
		GET_INTEGER ("Horizontal dimension"), GET_INTEGER ("Vertical dimension"),
		GET_INTEGER("Garnish")))
END

/************************* COVARIANCE & CONFIGURATION  ********************/

FORM (Covariance_to_Configuration, "Covariance: To Configuration", 0)
	NATURAL ("Number of dimensions", "2")
	OK
DO
	EVERY_TO (Covariance_to_Configuration (OBJECT, 
		GET_INTEGER ("Number of dimensions")))
END

/********* Procrustes ***************************/

DIRECT (Procrustes_help)
	Melder_help ("Procrustes"); 
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

FORM (TableOfReal_normalizeTable, "TableOfReal: Normalize table", 
	"TableOfReal: Normalize table...")
	POSITIVE ("Norm", "1.0")
	OK
DO
	EVERY (TableOfReal_normalizeTable (OBJECT, GET_REAL ("Norm")))
END

FORM (TableOfReal_normalizeRows, "TableOfReal: Normalize rows", 
	"TableOfReal: Normalize rows...")
	POSITIVE ("Norm", "1.0")
	OK
DO
	EVERY (TableOfReal_normalizeRows (OBJECT, GET_REAL ("Norm")))
END

FORM (TableOfReal_normalizeColumns, "TableOfReal: Normalize columns", 
	"TableOfReal: Normalize columns...")
	POSITIVE ("Norm", "1.0")
	OK
DO
	EVERY (TableOfReal_normalizeColumns (OBJECT, GET_REAL ("Norm")))
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
	praat_addAction1 (klas, 1, "Get transformation element...", QUERY_BUTTON, 
		1, DO_AffineTransform_getTransformationElement);
	praat_addAction1 (klas, 1, "Get translation element...", QUERY_BUTTON, 1, DO_AffineTransform_getTranslationElement);
	praat_addAction1 (klas, 0, "Invert", 0, 0, DO_AffineTransform_invert);
}

static void praat_Configuration_and_AffineTransform_init (void *transform)
{
	praat_addAction2 (classConfiguration, 1, transform, 1, "To Configuration",
		0, 0, DO_Configuration_and_AffineTransform_to_Configuration);
}

static void praat_TableOfReal_extras (void *klas)
{
	praat_addAction1 (klas, 1, "-- get additional --", "Get value...", 1, 0);
	praat_addAction1 (klas, 1, "Get table norm", "-- get additional --", 1,
		DO_TableOfReal_getTableNorm);
	praat_addAction1 (klas, 1, "-- set additional --", 
		"Set column label (label)...", 1, 0);
	praat_addAction1 (klas, 1, "Normalize rows...", "-- set additional --", 1, 
		DO_TableOfReal_normalizeRows);
	praat_addAction1 (klas, 1, "Normalize columns...", "Normalize rows...", 1, 
		DO_TableOfReal_normalizeColumns);
	praat_addAction1 (klas, 1, "Normalize table...", "Normalize columns...", 1, 
		DO_TableOfReal_normalizeTable);
	praat_addAction1 (klas, 1, "Standardize rows", "Normalize table...", 1, 
		DO_TableOfReal_standardizeRows);
	praat_addAction1 (klas, 1, "Standardize columns", "Standardize rows", 1, 
		DO_TableOfReal_standardizeColumns);
	praat_addAction1 (klas, 1, "Test sorting...", "Standardize columns", 
		praat_DEPTH_1 + praat_HIDDEN, DO_TabelOfReal_testSorting);
}

void praat_uvafon_MDS_init (void);
void praat_uvafon_MDS_init (void)
{
	Thing_recognizeClassesByName (classProcrustes,
		classContingencyTable, classDissimilarity,
		classSimilarity, classConfiguration, classDistance,
		classSalience, NULL);
	Thing_recognizeClassByOtherName (classProcrustes, "Procrustus");
		
    praat_addMenuCommand ("Objects", "New", "Multidimensional scaling",
		0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "MDS tutorial", 0, 1, DO_MDS_help);
    praat_addMenuCommand ("Objects", "New", "-- MDS --", 0, 1, 0);		
    praat_addMenuCommand ("Objects", "New", "Create letter R example...", 0, 1,
    	DO_Dissimilarity_createLetterRExample);
    praat_addMenuCommand ("Objects", "New", "Create INDSCAL Carroll Wish "
		"example...", 0, 1, DO_INDSCAL_createCarrollWishExample);
    praat_addMenuCommand ("Objects", "New", "Create Configuration...",
		0, 1, DO_Configuration_create);
    praat_addMenuCommand ("Objects", "New", "Draw splines...", 0, 1,
    	DO_drawSplines);
    praat_addMenuCommand ("Objects", "New", "Draw MDS class relations", 0, 1,
    	DO_drawMDSClassRelations);

/****** 1 class ********************************************************/

	praat_addAction1 (classAffineTransform, 0, "AffineTransform help", 0, 0,
		DO_AffineTransform_help);
	praat_AffineTransform_init (classAffineTransform);


	praat_addAction1 (classConfiguration, 0, "Configuration help", 0, 0,
		DO_Configuration_help);
	praat_TableOfReal_init2 (classConfiguration);
	praat_TableOfReal_extras (classConfiguration);
	(void) praat_removeAction (classConfiguration, NULL, NULL,
		L"Insert column (index)...");
	(void) praat_removeAction (classConfiguration, NULL, NULL,
		L"Remove column (index)...");
	(void) praat_removeAction (classConfiguration, NULL, NULL, L"Append");
	praat_addAction1 (classConfiguration, 0, "Draw...", DRAW_BUTTON,
		1, DO_Configuration_draw);
	praat_addAction1 (classConfiguration, 0, "Draw sigma ellipses...",
		"Draw...", 1, DO_Configuration_drawSigmaEllipses);
	praat_addAction1 (classConfiguration, 0, "Draw one sigma ellipse...",
		"Draw...", 1, DO_Configuration_drawOneSigmaEllipse);
	praat_addAction1 (classConfiguration, 0, "Draw confidence ellipses...",
		"Draw sigma ellipses...", 1, DO_Configuration_drawConfidenceEllipses);
	praat_addAction1 (classConfiguration, 0, "Draw one confidence ellipse...",
		"Draw sigma ellipses...", 1, DO_Configuration_drawOneConfidenceEllipse);
		
		praat_addAction1 (classConfiguration, 0, "Randomize", 
			"Normalize table...", 1, DO_Configuration_randomize);
		praat_addAction1 (classConfiguration, 0, "Normalize...", "Randomize", 1,
			DO_Configuration_normalize);
		praat_addAction1 (classConfiguration, 0, "Centralize", "Randomize", 1,
			DO_Configuration_centralize);
		praat_addAction1 (classConfiguration, 1, 
			"-- set rotations & reflections --", "Centralize", 1, 0);

		praat_addAction1 (classConfiguration, 0, "Rotate...", 
			"-- set rotations & reflections --", 1, DO_Configuration_rotate);
		praat_addAction1 (classConfiguration, 0, "Rotate (pc)", "Rotate...", 1,
			DO_Configuration_rotateToPrincipalDirections);
		praat_addAction1 (classConfiguration, 0, "Invert dimension...", 
			"Rotate (pc)", 1, DO_Configuration_invertDimension);
	praat_addAction1 (classConfiguration, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classConfiguration, 0, "To Distance", 0, 0,
		DO_Configuration_to_Distance);
	praat_addAction1 (classConfiguration, 0, "To Configuration (varimax)...", 
		0, 0, DO_Configuration_varimax);
	praat_addAction1 (classConfiguration, 0, "To Similarity (cc)", 0, 0,
		DO_Configuration_to_Similarity_cc);

	praat_addAction1 (classConfiguration, 0, "Match configurations - ", 
		0, 0, 0);
	praat_addAction1 (classConfiguration, 2, "To Procrustes...", 
		0, 1, DO_Configurations_to_Procrustes);	
	praat_addAction1 (classConfiguration, 2, 
		"To AffineTransform (congruence)...", 
		0, 1, DO_Configurations_to_AffineTransform_congruence);

	praat_addAction1 (classConfusion, 0, "To ContingencyTable", "To Matrix", 0,
		DO_Confusion_to_ContingencyTable);
	praat_addAction1 (classConfusion, 0, "To Proximity -       ", "Analyse", 
		0, 0);
		praat_addAction1 (classConfusion, 0, "To Dissimilarity (pdf)...",
			"To Proximity -       ", 1, DO_Confusion_to_Dissimilarity_pdf);
		praat_addAction1 (classConfusion, 0, "To Similarity...",
			"To Proximity -       ", 1, DO_Confusion_to_Similarity);
		praat_addAction1 (classConfusion, 0, "Sum", "Synthesize -     ", 1,
			DO_Confusions_sum);


	praat_TableOfReal_init2 (classContingencyTable);
	praat_addAction1 (classContingencyTable, 1, "-- statistics --",
		"Get value...", 1, 0);
	praat_addAction1 (classContingencyTable, 1, "Get chi squared probability", 
		"-- statistics --", 1, DO_ContingencyTable_chisqProbability);
	praat_addAction1 (classContingencyTable, 1, "Get Cramer's statistic",
		"Get chi squared probability", 1, DO_ContingencyTable_cramersStatistic);
	praat_addAction1 (classContingencyTable, 1, "Get contingency coefficient", 
		"Get Cramer's statistic", 1, 
		DO_ContingencyTable_contingencyCoefficient);
	praat_addAction1 (classContingencyTable, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classContingencyTable, 1, "To Configuration (ca)...", 
		0, 0, DO_ContingencyTable_to_Configuration_ca);


	praat_addAction1 (classCorrelation, 0, "To Configuration...", 0, 0,
		DO_Correlation_to_Configuration);

	praat_addAction1 (classDissimilarity, 0, "Dissimilarity help", 0, 0,
		DO_Dissimilarity_help);
	praat_TableOfReal_init2 (classDissimilarity);
	praat_TableOfReal_extras (classDissimilarity);
	praat_addAction1 (classDissimilarity, 0, "Get additive constant",
		"Get table norm", 1, DO_Dissimilarity_getAdditiveConstant);
	praat_addAction1 (classDissimilarity, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classDissimilarity, 0, CONFIGURATION_BUTTON,
		0, 0, 0);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(monotone mds)...", 0, 1, DO_Dissimilarity_monotone_mds);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(i-spline mds)...", 0, 1, DO_Dissimilarity_ispline_mds);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(interval mds)...", 0, 1, DO_Dissimilarity_interval_mds);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(ratio mds)...", 0, 1, DO_Dissimilarity_ratio_mds);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(absolute mds)...", 0, 1, DO_Dissimilarity_absolute_mds);
		praat_addAction1 (classDissimilarity, 1, "To Configuration "
			"(kruskal)...", 0, 1, DO_Dissimilarity_kruskal);
	praat_addAction1 (classDissimilarity, 0, "To Distance...", 0, 0,
		 DO_Dissimilarity_to_Distance);
	praat_addAction1 (classDissimilarity, 0, "To Weight", 0, 0,
		 DO_Dissimilarity_to_Weight);


	praat_addAction1 (classCovariance, 0, "To Configuration...", 0, 0,
		DO_Covariance_to_Configuration);


	praat_TableOfReal_init2 (classDistance);
	praat_TableOfReal_extras (classDistance);
	praat_addAction1 (classDistance, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classDistance, 0, CONFIGURATION_BUTTON, 0, 0, 0);
		praat_addAction1 (classDistance, 0, "To Configuration (indscal)...", 
			0, 1, DO_Distances_indscal);
		praat_addAction1 (classDistance, 0, "-- linear scaling --", 0, 1, 0);
		praat_addAction1 (classDistance, 0, "To Configuration (ytl)...", 0, 1, 
			DO_Distances_to_Configuration_ytl);
	praat_addAction1 (classDistance, 0, "To Dissimilarity", 0, 0, 
		DO_Distance_to_Dissimilarity);
	praat_addAction1 (classDistance, 0, "To ScalarProduct...", 0, 0, 
		DO_Distance_to_ScalarProduct);


	praat_addAction1 (classProcrustes, 0, "Procrustes help", 0, 0,
		DO_Procrustes_help);
	praat_AffineTransform_init (classProcrustes);
	praat_addAction1 (classProcrustes, 1, "Get scale", QUERY_BUTTON , 
		1, DO_Procrustes_getScale);
	praat_addAction1 (classProcrustes, 0, "Extract transformation matrix", 0, 0,
		DO_AffineTransform_extractMatrix);
	praat_addAction1 (classProcrustes, 0, "Extract translation vector", 0, 0,
		DO_AffineTransform_extractTranslationVector);

	praat_TableOfReal_init2 (classSalience);
	praat_TableOfReal_extras (classSalience);
	praat_addAction1 (classSalience, 0, "Draw...", DRAW_BUTTON, 1, 
		DO_Salience_draw);


	praat_addAction1 (classSimilarity, 0, "Similarity help", 0, 0, 
		DO_Similarity_help);
	praat_TableOfReal_init2 (classSimilarity);
	praat_TableOfReal_extras (classSimilarity);
	praat_addAction1 (classSimilarity, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classSimilarity, 0, "To Dissimilarity...", 0, 0, 
		DO_Similarity_to_Dissimilarity);


	praat_TableOfReal_init2 (classScalarProduct);
	praat_TableOfReal_extras (classScalarProduct);
   
	praat_TableOfReal_extras (classTableOfReal);
	praat_addAction1 (classTableOfReal, 1, "Centre rows", "Normalize table...",
		1, DO_TableOfReal_centreRows);
	praat_addAction1 (classTableOfReal, 1, "Centre columns", "Centre rows",
		1, DO_TableOfReal_centreColumns);
	praat_addAction1 (classTableOfReal, 1, "Double centre", "Centre columns",
		1, DO_TableOfReal_doubleCentre);
	praat_addAction1 (classTableOfReal, 0, "Cast -         ", 0, 0, 0);
		praat_addAction1 (classTableOfReal, 0, "To Confusion",
			0, 1, DO_TableOfReal_to_Confusion);
		praat_addAction1 (classTableOfReal, 0, "To Dissimilarity",
			0, 1, DO_TableOfReal_to_Dissimilarity);
		praat_addAction1 (classTableOfReal, 0, "To Similarity", 0, 1,
		 	DO_TableOfReal_to_Similarity);
		praat_addAction1 (classTableOfReal, 0, "To Distance", 0, 1,
			DO_TableOfReal_to_Distance);
		praat_addAction1 (classTableOfReal, 0, "To Salience", 0, 1,
			DO_TableOfReal_to_Salience);
		praat_addAction1 (classTableOfReal, 0, "To Weight", 0, 1,
			DO_TableOfReal_to_Weight);
		praat_addAction1 (classTableOfReal, 0, "To ScalarProduct", 0, 1,
			DO_TableOfReal_to_ScalarProduct);
		praat_addAction1 (classTableOfReal, 0, "To Configuration", 0, 1,
			DO_TableOfReal_to_Configuration);
		praat_addAction1 (classTableOfReal, 0, "To ContingencyTable", 0, 1,
			DO_TableOfReal_to_ContingencyTable);

	praat_TableOfReal_init2 (classWeight);
	
			
/****** 2 classes ********************************************************/

	praat_Configuration_and_AffineTransform_init (classAffineTransform);
	praat_Configuration_and_AffineTransform_init (classProcrustes);

	praat_addAction2 (classConfiguration, 0, classWeight, 1, "Analyse", 
		0, 0, 0);
	praat_addAction2 (classConfiguration, 0, classWeight, 1, 
		"To Similarity (cc)", 0, 0, DO_Configuration_Weight_to_Similarity_cc);

	praat_addAction2 (classDissimilarity, 1, classWeight, 1, ANALYSE_BUTTON,
		0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, "To Configuration "
		"(monotone mds)...", 0, 1, DO_Dissimilarity_Weight_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, "To Configuration "
		"(i-spline mds)...", 0, 1, DO_Dissimilarity_Weight_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, "To Configuration "
		"(interval mds)...", 0, 1, DO_Dissimilarity_Weight_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, "To Configuration "
		"(ratio mds)...", 0, 1, DO_Dissimilarity_Weight_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, "To Configuration "
		"(absolute mds)...", 0, 1, DO_Dissimilarity_Weight_absolute_mds);


	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, DRAW_BUTTON,
		0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Draw Shepard diagram...", 0, 1,
		DO_Dissimilarity_Configuration_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, "-- draw "
		"regressions --", 0, 1, 0);	
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		"Draw monotone regression...",
		0, 1, DO_Dissimilarity_Configuration_drawMonotoneRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Draw i-spline regression...",
		0, 1, DO_Dissimilarity_Configuration_drawISplineRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		"Draw interval regression...",
		0, 1, DO_Dissimilarity_Configuration_drawIntervalRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Draw ratio regression...",
		0, 1, DO_Dissimilarity_Configuration_drawRatioRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Draw absolute regression...",
		0, 1, DO_Dissimilarity_Configuration_drawAbsoluteRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		QUERY_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		"Get stress...",
		0, 1, DO_Dissimilarity_Configuration_getStress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Get stress (monotone mds)...",
		0, 1, DO_Dissimilarity_Configuration_monotone_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Get stress (i-spline mds)...",
		0, 1, DO_Dissimilarity_Configuration_ispline_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Get stress (interval mds)...",
		0, 1, DO_Dissimilarity_Configuration_interval_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Get stress (ratio mds)...",
		0, 1, DO_Dissimilarity_Configuration_ratio_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"Get stress (absolute mds)...",
		0, 1, DO_Dissimilarity_Configuration_absolute_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		ANALYSE_BUTTON, 0, 0, 0);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		"To Configuration (monotone mds)...",
		0, 1, DO_Dissimilarity_Configuration_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"To Configuration (i-spline mds)...",
		0, 1, DO_Dissimilarity_Configuration_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"To Configuration (interval mds)...",
		0, 1, DO_Dissimilarity_Configuration_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"To Configuration (ratio mds)...",
		0, 1, DO_Dissimilarity_Configuration_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1,
		"To Configuration (absolute mds)...",
		0, 1, DO_Dissimilarity_Configuration_absolute_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, 
		"To Configuration (kruskal)...",
		0, 1, DO_Dissimilarity_Configuration_kruskal);

	praat_addAction2 (classDistance, 1, classConfiguration, 1, DRAW_BUTTON,
		0, 0, 0);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, "Draw scatter "
		"diagram...", 0, 0, DO_Distance_and_Configuration_drawScatterDiagram);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, QUERY_BUTTON,
		0, 0, 0);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, "Get VAF...", 
		0, 0, DO_Distance_Configuration_vaf);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, ANALYSE_BUTTON,
		0, 0, 0);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, 
		"To Configuration (indscal)...", 0, 1,
		DO_Distance_Configuration_indscal);

	praat_addAction2 (classDistance, 1, classDissimilarity, 1,
		"Draw Shepard diagram...", 0, 0,
		 DO_Distance_Dissimilarity_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classDistance, 1, 
		"Monotone regression...", 0, 0,
		 DO_Dissimilarity_Distance_monotoneRegression);
		
/****** 3 classes ********************************************************/


	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, 
		classSalience, 1, QUERY_BUTTON, 0, 0, 0); 
	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, 
		classSalience, 1, "Get VAF...", 0, 1, 
		DO_Dissimilarity_Configuration_Salience_vaf); 

	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight,
		1, QUERY_BUTTON, 0, 0, 0); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "Get stress (monotone mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_monotone_stress); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "Get stress (i-spline mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_ispline_stress); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "Get stress (interval mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_interval_stress); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "Get stress (ratio mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_ratio_stress); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "Get stress (absolute mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_absolute_stress); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, ANALYSE_BUTTON, 0, 0, 0); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "To Configuration (monotone mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_monotone_mds); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "To Configuration (i-spline mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_ispline_mds); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "To Configuration (interval mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_interval_mds); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "To Configuration (ratio mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_ratio_mds); 
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 
		1, "To Configuration (absolute mds)...",
		0, 1, DO_Dissimilarity_Configuration_Weight_absolute_mds); 


	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1,
		QUERY_BUTTON, 0, 0, 0); 
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1,
		"Get VAF...", 0, 1, DO_Distance_Configuration_Salience_vaf); 
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1,
		 "Analyse", 0, 0, 0); 
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1,
		"To Configuration (indscal)...", 0, 0,
		 DO_Distance_Configuration_Salience_indscal); 


	INCLUDE_MANPAGES (manual_MDS_init)
}

/* End of file praat_MDS_init.c */
