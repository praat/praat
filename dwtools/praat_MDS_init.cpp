/* praat_MDS_init.cpp
 *
 * Copyright (C) 1992-2012, 2015-2016 David Weenink
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
 djmw 20071011 REQUIRE requires U"".
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

void praat_TableOfReal_init2 (ClassInfo klas);

static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *DRAW_BUTTON    = U"Draw -";
static const char32 *ANALYSE_BUTTON = U"Analyse -";
static const char32 *CONFIGURATION_BUTTON = U"To Configuration -";

/* Tests */

/*
	Sort row 1 ascending and store in row 3
	Sort row 1 and move row 2 along and store in rows 4 and 5 respectively
	Make an index for row 1 and store in row 6
*/
static void TabelOfReal_testSorting (TableOfReal me, long rowtoindex) {
	try {
		long  nc = my numberOfColumns;

		autoNUMvector<long> index (1, nc);
		if (my numberOfRows < 6) {
			Melder_throw (U"TabelOfReal_sort2: we want at least 6 rows!!");
		}
		if (rowtoindex < 1 || rowtoindex > 2) {
			Melder_throw (U"TabelOfReal_sort2: rowtoindex <= 2");
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
		Melder_throw (me, U": sorting test not ok.");
	}
}

#undef iam
#define iam iam_LOOP

FORM (MODIFY_TabelOfReal_testSorting, U"TabelOfReal: Sort and index", U"") {
	NATURALVAR (rowIndex, U"Row to index", U"1")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TabelOfReal_testSorting (me, rowIndex);
	}
END }

/************************* examples ***************************************/

FORM (NEW_Dissimilarity_createLetterRExample, U"Create letter R example", U"Create letter R example...") {
	LABEL (U"", U"For the monotone transformation on the distances")
	REALVAR (noiseRange, U"Noise range", U"32.5")
	OK
DO
	praat_new (Dissimilarity_createLetterRExample (noiseRange), U"R");
END }

FORM (NEWMANY_INDSCAL_createCarrollWishExample, U"Create INDSCAL Carroll & Wish example...", U"Create INDSCAL Carroll & Wish example...") {
	REALVAR (noiseStandardDeviation, U"Noise standard deviation", U"0.0")
	OK
DO
	praat_new (INDSCAL_createCarrollWishExample (noiseStandardDeviation), U""); 
END }

FORM (NEW_Configuration_create, U"Create Configuration", U"Create Configuration...") {
	WORDVAR (name, U"Name", U"uniform")
	NATURALVAR (numberOfPoints, U"Number of points", U"10")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	LABEL (U"", U"Formula:")
	TEXTVAR (formula, U"formula", U"randomUniform(-1.5, 1.5)")
	OK
DO
	autoConfiguration me = Configuration_create (numberOfPoints, numberOfDimensions);
	TableOfReal_formula (me.get(), formula, interpreter, nullptr);
	praat_new (me.move(), name);
END }

FORM (GRAPHICS_drawSplines, U"Draw splines", U"spline") {
	REALVAR (xmin, U"left Horizontal range", U"0.0")
	REALVAR (xmax, U"right Horizontal range", U"1.0")
	REALVAR (ymin, U"left Vertical range", U"0.0")
	REALVAR (ymax, U"right Vertical range", U"20.0")
	RADIOVAR (splineType, U"Spline type", 1)
		RADIOBUTTON (U"M-spline")
		RADIOBUTTON (U"I-spline")
	INTEGERVAR (order, U"Order", U"3")
	SENTENCEVAR (interiorKnots_string, U"Interior knots", U"0.3 0.5 0.6")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	if (xmax <= xmin or ymax <= ymin) {
		Melder_throw (U"Required: xmin < xmax and ymin < ymax.");
	}
	autoPraatPicture picture;
	drawSplines (GRAPHICS, xmin, xmax, ymin, ymax, splineType,order, interiorKnots_string, garnish);
END }

DIRECT (GRAPHICS_drawMDSClassRelations) {
	autoPraatPicture picture;
	drawMDSClassRelations (GRAPHICS);
END }


/***************** AffineTransform ***************************************/


DIRECT (HELP_AffineTransform_help) {
	Melder_help (U"AffineTransform");
END }

DIRECT (NEW_AffineTransform_invert) {
	LOOP {
		iam (AffineTransform);
		praat_new (AffineTransform_invert (me), NAME, U"_inv");
	}
END }

FORM (REAL_AffineTransform_getTransformationElement, U"AffineTransform: Get transformation element", U"Procrustes") {
	NATURALVAR (row, U"Row number", U"1")
	NATURALVAR (column, U"Column number", U"1")
	OK
DO
	LOOP {
		iam (AffineTransform);
		if (row > my n) {
			Melder_throw (U"Row number must not exceed number of rows.");
		}
		if (column > my n) {
			Melder_throw (U"Column number must not exceed number of columns.");
		}
		Melder_information (my r [row] [column]);
	}
END }

FORM (REAL_AffineTransform_getTranslationElement, U"AffineTransform: Get translation element", U"Procrustes") {
	NATURALVAR (index, U"Index", U"1")
	OK
DO
	LOOP {
		iam (AffineTransform);
		if (index > my n) {
			Melder_throw (U"Index must not exceed number of elements.");
		}
		Melder_information (my t [index]);
	}
END }

DIRECT (NEW_AffineTransform_extractMatrix) {
	LOOP {
		iam (AffineTransform);
		autoTableOfReal thee = AffineTransform_extractMatrix (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_AffineTransform_extractTranslationVector) {
	LOOP {
		iam (AffineTransform);
		autoTableOfReal thee = AffineTransform_extractTranslationVector (me);
		praat_new (thee.move(), my name);
	}
END }

/***************** Configuration ***************************************/

DIRECT (HELP_Configuration_help) {
	Melder_help (U"Configuration");
END }

#define praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax) \
	NATURALVAR (horizontalDimension, U"Horizontal dimension", U"1") \
	NATURALVAR (verticalDimension, U"Vertical dimension", U"2") \
	REALVAR (xmin, U"left Horizontal range", U"0.0") \
	REALVAR (xmax, U"right Horizontal range", U"0.0") \
	REALVAR (ymin, U"left Vertical range", U"0.0") \
	REALVAR (ymax, U"right Vertical range", U"0.0")

FORM (GRAPHICS_Configuration_draw, U"Configuration: Draw", U"Configuration: Draw...") {
	praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax)
	NATURALVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (useRowLables, U"Use row labels", false)
	WORDVAR (label, U"Label", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_draw (me, GRAPHICS, horizontalDimension, verticalDimension, xmin, xmax, ymin, ymax, labelSize, useRowLables, label, garnish);
	}
END }

FORM (GRAPHICS_Configuration_drawSigmaEllipses, U"Configuration: Draw sigma ellipses", U"Configuration: Draw sigma ellipses...") {
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax)
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas, false,   // confidence (i.e. the previous argument is a number of sigmas, not a confidence level)
		nullptr, horizontalDimension, verticalDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	}
END }

FORM (GRAPHICS_Configuration_drawOneSigmaEllipse, U"Configuration: Draw one sigma ellipse", U"Configuration: Draw sigma ellipses...") {
	SENTENCEVAR (label, U"Label", U"")
	POSITIVEVAR (numberOfSigmas, U"Number of sigmas", U"1.0")
	praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax)
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, numberOfSigmas,false, label, horizontalDimension, verticalDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	}
END }


FORM (GRAPHICS_Configuration_drawConfidenceEllipses, U"Configuration: Draw confidence ellipses", nullptr) {
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax)
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true /* confidence */, nullptr, horizontalDimension, verticalDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	}
END }

FORM (GRAPHICS_Configuration_drawOneConfidenceEllipse, U"Configuration: Draw one confidence ellipse", nullptr) {
	SENTENCEVAR (label, U"Label", U"")
	POSITIVEVAR (confidenceLevel, U"Confidence level (0-1)", U"0.95")
	praat_Configuration_draw_commonFields(horizontalDimension,verticalDimension,xmin,xmax,ymin,ymax)
	INTEGERVAR (labelSize, U"Label size", U"12")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Configuration);
		Configuration_drawConcentrationEllipses (me, GRAPHICS, confidenceLevel, true, label, horizontalDimension, verticalDimension, xmin, xmax, ymin, ymax, labelSize, garnish);
	}
END }

DIRECT (MODIFY_Configuration_randomize) {
	LOOP {
		iam (Configuration);
		Configuration_randomize (me);
	}
END }

FORM (MODIFY_Configuration_normalize, U"Configuration: Normalize", U"Configuration: Normalize...") {
	REALVAR (sumOfSquares, U"Sum of squares", U"0.0")
	LABEL (U"", U"On (INDSCAL), Off (Kruskal)")
	BOOLEANVAR (separateDimensions, U"Each dimension separately", true)
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_normalize (me, sumOfSquares, separateDimensions);
	}
END }

DIRECT (MODIFY_Configuration_centralize) {
	LOOP {
		iam (Configuration);
		TableOfReal_centreColumns (me);
	}
END }

FORM (MODIFY_Configuration_rotate, U"Configuration: Rotate", U"Configuration: Rotate...") {
	NATURALVAR (dimension1, U"Dimension 1", U"1")
	NATURALVAR (dimension2, U"Dimension 2", U"2")
	REALVAR (angle_degrees, U"Angle (degrees)", U"60.0")
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_rotate (me, dimension1, dimension2, angle_degrees);
	}
END }

DIRECT (MODIFY_Configuration_rotateToPrincipalDirections) {
	LOOP {
		iam (Configuration);
		Configuration_rotateToPrincipalDirections (me);
	}
END }

FORM (MODIFY_Configuration_invertDimension, U"Configuration: Invert dimension", U"Configuration: Invert dimension...") {
	NATURALVAR (dimension, U"Dimension", U"1")
	OK
DO
	LOOP {
		iam (Configuration);
		Configuration_invertDimension (me, dimension);
	}
END }

DIRECT (NEW_Configuration_to_Distance) {
	LOOP {
		iam (Configuration);
		praat_new (Configuration_to_Distance (me), my name);
	}
END }

FORM (NEW_Configuration_varimax, U"Configuration: To Configuration (varimax)", U"Configuration: To Configuration (varimax)...") {
	BOOLEANVAR (normalizeRows, U"Normalize rows", true)
	BOOLEANVAR (useQuartimax, U"Quartimax", false)
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"50")
	POSITIVEVAR (tolerance, U"Tolerance", U"1e-6")
	OK
DO
	LOOP {
		iam (Configuration);
		autoConfiguration result = Configuration_varimax (me, normalizeRows, useQuartimax, maximumNumberOfIterations, tolerance);
		praat_new (result.move(), my name, U"_varimax");
	}
END }

DIRECT (NEW_Configurations_to_Similarity_cc) {
	autoConfigurationList list = ConfigurationList_create ();
	LOOP {
		iam (Configuration);
		list -> addItem_ref (me);
	}
	Weight nullWeight = nullptr;
	autoSimilarity result = ConfigurationList_to_Similarity_cc (list.get(), nullWeight);
	praat_new (result.move(), U"congruence");
END }

FORM (NEW1_Configurations_to_Procrustes, U"Configuration & Configuration: To Procrustes", U"Configuration & Configuration: To Procrustes...") {
	BOOLEANVAR (useOrthogonalTransform, U"Use orthogonal transform", false)
	OK
DO
	Configuration configuration1 = nullptr, configuration2 = nullptr;
	LOOP {
		iam (Configuration);
		(configuration1 ? configuration2 : configuration1) = me;
	}
	Melder_assert (configuration1 && configuration2);
	autoProcrustes result = Configurations_to_Procrustes (configuration1, configuration2, useOrthogonalTransform);
	praat_new (result.move(), Thing_getName (configuration2), U"_to_", Thing_getName (configuration1));
END }

FORM (NEW1_Configurations_to_AffineTransform_congruence, U"Configurations: To AffineTransform (congruence)", U"Configurations: To AffineTransform (congruence)...") {
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"50")
	POSITIVEVAR (tolerance, U"Tolerance", U"1e-6")
	OK
DO
	Configuration configuration1 = nullptr;
	Configuration configuration2 = nullptr;
	LOOP {
		iam (Configuration);
		( configuration1 ? configuration2 : configuration1 ) = me;
	}
	Melder_assert (configuration1 && configuration2);
	autoAffineTransform result = Configurations_to_AffineTransform_congruence (configuration1, configuration2, maximumNumberOfIterations, tolerance);
	praat_new (result.move(), configuration1 -> name, U"_", configuration2 -> name);
END }

DIRECT (NEW1_Configuration_Weight_to_Similarity_cc) {
	autoConfigurationList configurations = ConfigurationList_create ();
	Weight weight = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classConfiguration) {
			configurations -> addItem_ref ((Configuration) me);
		} else if (CLASS == classWeight) {
			weight = (Weight) me;
		}
	}
	Melder_assert (configurations->size > 0 && weight);
	autoSimilarity result = ConfigurationList_to_Similarity_cc (configurations.get(), weight);
	praat_new (result.move(), U"congruence");
END }

DIRECT (NEW1_Configuration_and_AffineTransform_to_Configuration) {
	Configuration me = FIRST (Configuration);
	AffineTransform thee = FIRST_GENERIC (AffineTransform);
	autoConfiguration result = Configuration_and_AffineTransform_to_Configuration (me, thee);
	praat_new (result.move(), my name, U"_", thy name);
END }

/*************** Confusion *********************************/

FORM (NEW_Confusion_to_Dissimilarity_pdf, U"Confusion: To Dissimilarity (pdf)", U"Confusion: To Dissimilarity (pdf)...") {
	POSITIVEVAR (minimumConfusionLevel, U"Minimum confusion level", U"0.5")
	OK
DO
	LOOP {
		iam (Confusion);
		autoDissimilarity result = Confusion_to_Dissimilarity_pdf (me, minimumConfusionLevel);
		praat_new (result.move(), my name, U"_pdf");
	}
END }

FORM (NEW_Confusion_to_Similarity, U"Confusion: To Similarity", U"Confusion: To Similarity...") {
	BOOLEANVAR (normalize, U"Normalize", true)
	RADIOVAR (symmetrizeMethod, U"Symmetrization", 1)
		RADIOBUTTON (U"No symmetrization")
		RADIOBUTTON (U"Average (s[i][j] = (c[i][j]+c[j][i])/2)")
		RADIOBUTTON (U"Houtgast (s[i][j]= sum (min(c[i][k],c[j][k])))")
	OK
DO
	LOOP {
		iam (Confusion);
		autoSimilarity result = Confusion_to_Similarity (me, normalize, symmetrizeMethod);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW1_Confusions_sum) {
	autoConfusionList confusions = ConfusionList_create ();
	LOOP {
		iam (Confusion);
		confusions -> addItem_ref (me);
	}
	autoConfusion result = ConfusionList_sum (confusions.get());
	praat_new (result.move(), U"sum");
END }

DIRECT (NEW_Confusion_to_ContingencyTable) {
	LOOP {
		iam (Confusion);
		autoContingencyTable result = Confusion_to_ContingencyTable (me);
		praat_new (result.move(), my name);
	}
END }

/*************** ContingencyTable *********************************/


FORM (NEW_ContingencyTable_to_Configuration_ca, U"ContingencyTable: To Configuration (ca)", U"ContingencyTable: To Configuration (ca)...") {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	RADIOVAR (scalingType, U"Scaling of final configuration", 3)
		RADIOBUTTON (U"Row points in centre of gravity of column points")
		RADIOBUTTON (U"Column points in centre of gravity of row points")
		RADIOBUTTON (U"Row points and column points symmetric")
	OK
DO
	LOOP {
		iam (ContingencyTable);
		praat_new (ContingencyTable_to_Configuration_ca (me, numberOfDimensions, scalingType), my name);
	}
END }

DIRECT (REAL_ContingencyTable_chisqProbability) {
	LOOP {
		iam (ContingencyTable);
		Melder_information (ContingencyTable_chisqProbability (me));
	}
END }

DIRECT (REAL_ContingencyTable_cramersStatistic) {
	LOOP {
		iam (ContingencyTable);
		Melder_information (ContingencyTable_cramersStatistic (me));
	}
END }

DIRECT (REAL_ContingencyTable_contingencyCoefficient) {
	LOOP {
		iam (ContingencyTable);
		Melder_information (ContingencyTable_contingencyCoefficient (me));
	}
END }

/************************* Correlation ***********************************/

FORM (NEW_Correlation_to_Configuration, U"Correlation: To Configuration", nullptr) {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	OK
DO
	LOOP {
		iam (Correlation);
		autoConfiguration result = Correlation_to_Configuration (me, numberOfDimensions);
		praat_new (result.move(), my name);
	}
END }


/************************* Similarity ***************************************/

DIRECT (HELP_Similarity_help) {
	Melder_help (U"Similarity");
END }

FORM (NEW_Similarity_to_Dissimilarity, U"Similarity: To Dissimilarity", U"Similarity: To Dissimilarity...") {
	REALVAR (maximumDissimilarity, U"Maximum dissimilarity", U"0.0 (= from data)")
	OK
DO
	LOOP {
		iam (Similarity);
		autoDissimilarity result = Similarity_to_Dissimilarity (me, maximumDissimilarity);
		praat_new (result.move(), my name);
	}
END }

/**************** Dissimilarity ***************************************/

#define praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions) \
	LABEL (U"", U"Minimization parameters") \
	REALVAR (tolerance, U"Tolerance", U"1e-5") \
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"50 (= each repetition)") \
	NATURALVAR (numberOfRepetitions, U"Number of repetitions", U"1")


#define Dissimilarity_and_Configuration_getStress_addCommonFields(stressMeasure) \
	RADIOVAR (stressMeasure, U"Stress measure", 1) \
		RADIOBUTTON (U"Normalized") \
		RADIOBUTTON (U"Kruskal's stress-1") \
		RADIOBUTTON (U"Kruskal's stress-2") \
		RADIOBUTTON (U"Raw")

#define praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish) \
	REALVAR (fromProximity, U"left Proximity range", U"0.0") \
	REALVAR (toProximity, U"right Proximity range", U"0.0") \
	REALVAR (fromDistance, U"left Distance range", U"0.0") \
	REALVAR (toDistance, U"right Distance range", U"0.0") \
	POSITIVEVAR (markSize, U"Mark size (mm)", U"1.0") \
	SENTENCEVAR (markString, U"Mark string (+xo.)", U"+") \
	BOOLEANVAR (garnish, U"Garnish", true)

DIRECT (HELP_Dissimilarity_help) {
	Melder_help (U"Dissimilarity");
END }

DIRECT (REAL_Dissimilarity_getAdditiveConstant) {
	LOOP {
		iam (Dissimilarity);
		double c = Dissimilarity_getAdditiveConstant (me);
		Melder_information (c);
	}
END }

FORM (NEW1_Dissimilarity_Configuration_kruskal, U"Dissimilarity & Configuration: To Configuration (kruskal)", U"Dissimilarity & Configuration: To Configuration (kruskal)...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	RADIOVAR (stressFormula, U"Stress calculation", 1)
		RADIOBUTTON (U"Formula1")
		RADIOBUTTON (U"Formula2")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	autoConfiguration result = Dissimilarity_Configuration_kruskal (me, thee, tiesHandling, stressFormula, tolerance, maximumNumberOfIterations, numberOfRepetitions);
	praat_new (result.move(), my name, U"_kruskal");
END }

FORM (NEW1_Dissimilarity_Configuration_absolute_mds, U"Dissimilarity & Configuration: To Configuration (absolute mds)", U"Dissimilarity & Configuration: To Configuration (absolute mds)...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_absolute_mds (me, thee, nullptr, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), my name, U"_absolute");
END }

FORM (NEW1_Dissimilarity_Configuration_ratio_mds, U"Dissimilarity & Configuration: To Configuration (ratio mds)", U"Dissimilarity & Configuration: To Configuration (ratio mds)...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_ratio_mds (me, thee, nullptr, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), my name, U"_ratio");
END }

FORM (NEW1_Dissimilarity_Configuration_interval_mds, U"Dissimilarity & Configuration: To Configuration (interval mds)", U"Dissimilarity & Configuration: To Configuration (interval mds)...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_interval_mds (me, thee, nullptr, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), my name, U"_interval");
END }

FORM (NEW1_Dissimilarity_Configuration_monotone_mds, U"Dissimilarity & Configuration: To Configuration (monotone mds)", U"Dissimilarity & Configuration: To Configuration (monotone mds)...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_monotone_mds (me, thee, nullptr, tiesHandling, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), my name, U"_monotone");
END }

FORM (NEW1_Dissimilarity_Configuration_ispline_mds, U"Dissimilarity & Configuration: To Configuration (i-spline mds)", U"Dissimilarity & Configuration: To Configuration (i-spline mds)...") {
	LABEL (U"", U"Spline smoothing")
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"1")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_ispline_mds (me, thee, nullptr, numberOfInteriorKnots, order, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), my name, U"_ispline");
END }

FORM (NEW1_Dissimilarity_Configuration_Weight_absolute_mds, U"Dissimilarity & Configuration & Weight: To Configuration (absolute mds)", U"Dissimilarity & Configuration & Weight: To Configuration...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_absolute_mds (dissimilarity, configuration, weight, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), dissimilarity -> name, U"_w_absolute");
END }

FORM (NEW1_Dissimilarity_Configuration_Weight_ratio_mds, U"Dissimilarity & Configuration & Weight: To Configuration (ratio mds)", U"Dissimilarity & Configuration & Weight: To Configuration...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_ratio_mds (dissimilarity, configuration, weight, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), dissimilarity -> name, U"_w_ratio");
END }

FORM (NEW1_Dissimilarity_Configuration_Weight_interval_mds, U"Dissimilarity & Configuration & Weight: To Configuration (interval mds)", U"Dissimilarity & Configuration & Weight: To Configuration...") {
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	bool showProgress = true;
	autoConfiguration result = Dissimilarity_Configuration_Weight_interval_mds (dissimilarity, configuration, weight, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress);
	praat_new (result.move(), dissimilarity -> name, U"_w_interval");
END }

FORM (NEW1_Dissimilarity_Configuration_Weight_monotone_mds, U"Dissimilarity & Configuration & Weight: To Configuration (monotone mds)", U"Dissimilarity & Configuration & Weight: To Configuration...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	bool showProgress = true;
	praat_new (Dissimilarity_Configuration_Weight_monotone_mds (dissimilarity, configuration, weight, tiesHandling, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), dissimilarity -> name, U"_sw_monotone");
END }

FORM (NEW1_Dissimilarity_Configuration_Weight_ispline_mds,  U"Dissimilarity & Configuration & Weight: To Configuration (i-spline mds)", U"Dissimilarity & Configuration & Weight: To Configuration...") {
	LABEL (U"", U"Spline smoothing")
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"1")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance,maximumNumberOfIterations,numberOfRepetitions)
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	bool showProgress = true;
	praat_new (Dissimilarity_Configuration_Weight_ispline_mds (dissimilarity, configuration, weight, numberOfInteriorKnots, order, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), dissimilarity -> name, U"_sw_ispline");
END }

FORM (REAL_Dissimilarity_Configuration_getStress, U"Dissimilarity & Configuration: Get stress",  U"Dissimilarity & Configuration: get stress") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	RADIOVAR (stressFormula, U"Stress calculation", 1)
		RADIOBUTTON (U"Formula1")
		RADIOBUTTON (U"Formula2")
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_getStress (me, thee, tiesHandling, stressFormula));
END }

FORM (REAL_Dissimilarity_Configuration_absolute_stress, U"Dissimilarity & Configuration: Get stress (absolute mds)", U"Dissimilarity & Configuration: Get stress (absolute mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_Weight_absolute_stress (me, thee, nullptr,stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_ratio_stress, U"Dissimilarity & Configuration: Get stress (ratio mds)", U"Dissimilarity & Configuration: Get stress (ratio mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_Weight_ratio_stress (me, thee, nullptr, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_interval_stress, U"Dissimilarity & Configuration: Get stress (interval mds)", U"Dissimilarity & Configuration: Get stress (interval mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_Weight_interval_stress (me, thee, nullptr, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_monotone_stress, U"Dissimilarity & Configuration: Get stress (monotone mds)", U"Dissimilarity & Configuration: Get stress (monotone mds)...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_Weight_monotone_stress (me, thee, nullptr, tiesHandling,stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_ispline_stress, U"Dissimilarity & Configuration: Get stress (i-spline mds)", U"Dissimilarity & Configuration: Get stress (i-spline mds)...") {
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Melder_information (Dissimilarity_Configuration_Weight_ispline_stress (me, thee, nullptr, numberOfInteriorKnots, order, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_Weight_absolute_stress, U"Dissimilarity & Configuration & Weight: Get stress (absolute mds)", U"Dissimilarity & Configuration & Weight: Get stress (absolute mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	Melder_information (Dissimilarity_Configuration_Weight_absolute_stress (dissimilarity, configuration, weight, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_Weight_ratio_stress, U"Dissimilarity & Configuration & Weight: Get stress (ratio mds)", U"Dissimilarity & Configuration & Weight: Get stress (ratio mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	Melder_information (Dissimilarity_Configuration_Weight_ratio_stress (dissimilarity, configuration, weight, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_Weight_interval_stress, U"Dissimilarity & Configuration & Weight: Get stress (interval mds)", U"Dissimilarity & Configuration & Weight: Get stress (interval mds)...") {
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	Melder_information (Dissimilarity_Configuration_Weight_interval_stress (dissimilarity, configuration, weight, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_Weight_monotone_stress, U"Dissimilarity & Configuration & Weight: Get stress (monotone mds)", U"Dissimilarity & Configuration & Weight: Get stress (monotone mds)...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach)")
		RADIOBUTTON (U"Secondary approach")
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	Melder_information (Dissimilarity_Configuration_Weight_monotone_stress (dissimilarity, configuration, weight, tiesHandling, stressMeasure));
END }

FORM (REAL_Dissimilarity_Configuration_Weight_ispline_stress, U"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)", U"Dissimilarity & Configuration & Weight: Get stress (i-spline mds)...") {
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"3")
	Dissimilarity_and_Configuration_getStress_addCommonFields (stressMeasure);
	OK
DO
	Dissimilarity dissimilarity = FIRST (Dissimilarity);
	Configuration configuration = FIRST (Configuration);
	Weight weight = FIRST (Weight);
	Melder_information (Dissimilarity_Configuration_Weight_ispline_stress (dissimilarity, configuration, weight, numberOfInteriorKnots, order, stressMeasure));
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawShepardDiagram, U"Dissimilarity & Configuration: Draw Shepard diagram", U"Dissimilarity & Configuration: Draw Shepard diagram...") {
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_drawShepardDiagram (me, thee, GRAPHICS, fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawAbsoluteRegression, U"Dissimilarity & Configuration: Draw regression (absolute mds)", U"Dissimilarity & Configuration: Draw regression (absolute mds)...") {
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawAbsoluteRegression (me, thee, nullptr, GRAPHICS, fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawRatioRegression, U"Dissimilarity & Configuration: Draw regression (ratio mds)", U"Dissimilarity & Configuration: Draw regression (ratio mds)...") {
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawRatioRegression (me, thee, nullptr, GRAPHICS, fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawIntervalRegression, U"Dissimilarity & Configuration: Draw regression (interval mds)", U"Dissimilarity & Configuration: Draw regression (interval mds)...") {
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawIntervalRegression (me, thee, nullptr, GRAPHICS, fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawMonotoneRegression, U"Dissimilarity & Configuration: Draw regression (monotone mds)", U"Dissimilarity & Configuration: Draw regression (monotone mds)...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach)")
		RADIOBUTTON (U"Secondary approach")
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawMonotoneRegression (me, thee, nullptr, GRAPHICS, tiesHandling, fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (GRAPHICS_Dissimilarity_Configuration_drawISplineRegression, U"Dissimilarity & Configuration: Draw regression (i-spline mds)", U"Dissimilarity & Configuration: Draw regression (i-spline mds)...") {
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"3")
	praat_Dissimilarity_Configuration_drawing_commonFields(fromProximity,toProximity,fromDistance,toDistance,markSize,markString,garnish)	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Configuration thee = FIRST (Configuration);
	Dissimilarity_Configuration_Weight_drawISplineRegression (me, thee, nullptr, GRAPHICS, numberOfInteriorKnots, order,fromProximity, toProximity, fromDistance, toDistance, markSize, markString, garnish);
END }

FORM (NEW_Dissimilarity_to_Configuration_kruskal, U"Dissimilarity: To Configuration (kruskal)", U"Dissimilarity: To Configuration (kruskal)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	NATURALVAR (distanceMetric, U"Distance metric", U"2 (= Euclidean)")
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	RADIOVAR (stressFormula, U"Stress calculation", 1)
		RADIOBUTTON (U"Formula1")
		RADIOBUTTON (U"Formula2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_to_Configuration_kruskal (me, numberOfDimensions, distanceMetric, tiesHandling, stressFormula, tolerance, maximumNumberOfIterations, numberOfRepetitions), my name);
	}
END }

FORM (NEW_Dissimilarity_to_Configuration_absolute_mds, U"Dissimilarity: To Configuration (absolute mds)", U"Dissimilarity: To Configuration (absolute mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_absolute_mds (me, nullptr, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_absolute");
	}
END }

FORM (NEW_Dissimilarity_to_Configuration_ratio_mds, U"Dissimilarity: To Configuration (ratio mds)", U"Dissimilarity: To Configuration (ratio mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_ratio_mds (me, nullptr, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_ratio");
	}
END }

FORM (NEW_Dissimilarity_to_Configuration_interval_mds, U"Dissimilarity: To Configuration (interval mds)", U"Dissimilarity: To Configuration (interval mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_interval_mds (me, nullptr, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_interval");
	}
END }

FORM (NEW_Dissimilarity_to_Configuration_monotone_mds, U"Dissimilarity: To Configuration (monotone mds)", U"Dissimilarity: To Configuration (monotone mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	praat_Dissimilarity_to_Configuration_commonFields (tolerance, maximumNumberOfIterations, numberOfRepetitions)	
	OK
DO
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_monotone_mds (me, nullptr, numberOfDimensions, tiesHandling, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_monotone");
	}
END }

FORM (NEW_Dissimilarity_to_Configuration_ispline_mds, U"Dissimilarity: To Configuration (i-spline mds)", U"Dissimilarity: To Configuration (i-spline mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	LABEL (U"", U"Spline smoothing")
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"1")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	if (not (order > 0 || numberOfInteriorKnots > 0)) {
		Melder_throw (U"Order-zero spline must at least have 1 interior knot.");
	}
	LOOP {
		iam (Dissimilarity);
		int showProgress = 1;
		praat_new (Dissimilarity_Weight_ispline_mds (me, nullptr, numberOfDimensions, numberOfInteriorKnots, order, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_ispline");
	}
END }

FORM (NEW1_Dissimilarity_Weight_ispline_mds, U"Dissimilarity & Weight: To Configuration (i-spline mds)", U"Dissimilarity & Weight: To Configuration (i-spline mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	LABEL (U"", U"Spline smoothing")
	INTEGERVAR (numberOfInteriorKnots, U"Number of interior knots", U"1")
	INTEGERVAR (order, U"Order of I-spline", U"1")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	if (not (order > 0 || numberOfInteriorKnots > 0)) {
		Melder_throw (U"Order-zero spline must at least have 1 interior knot.");
	}
	praat_new (Dissimilarity_Weight_ispline_mds (me, w, numberOfDimensions, numberOfInteriorKnots, order, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_ispline");
END }

FORM (NEW1_Dissimilarity_Weight_absolute_mds, U"Dissimilarity & Weight: To Configuration (absolute mds)", U"Dissimilarity & Weight: To Configuration (absolute mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_absolute_mds (me, w, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_absolute");
END }

FORM (NEW1_Dissimilarity_Weight_ratio_mds, U"Dissimilarity & Weight: To Configuration (ratio mds)", U"Dissimilarity & Weight: To Configuration (ratio mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_ratio_mds (me, w, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_absolute");
END }

FORM (NEW1_Dissimilarity_Weight_interval_mds, U"Dissimilarity & Weight: To Configuration (interval mds)", U"Dissimilarity & Weight: To Configuration (interval mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_interval_mds (me, w, numberOfDimensions, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_absolute");
END }

FORM (NEW1_Dissimilarity_Weight_monotone_mds, U"Dissimilarity & Weight: To Configuration (monotone mds)", U"Dissimilarity & Weight: To Configuration (monotone mds)...") {
	LABEL (U"", U"Configuration")
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	praat_Dissimilarity_to_Configuration_commonFields(tolerance,maximumNumberOfIterations,numberOfRepetitions)	
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Weight w = FIRST (Weight);
	int showProgress = 1;
	praat_new (Dissimilarity_Weight_monotone_mds (me, w, numberOfDimensions, tiesHandling, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgress), my name, U"_monotone");
END }

FORM (NEW_Dissimilarity_to_Distance, U"Dissimilarity: To Distance", U"Dissimilarity: To Distance...") {
	BOOLEANVAR (scale, U"Scale (additive constant)", true)
	OK
DO
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_to_Distance (me, scale), my name);
	}
END }

DIRECT (NEW_Dissimilarity_to_Weight) {
	LOOP {
		iam (Dissimilarity);
		praat_new (Dissimilarity_to_Weight (me), my name);
	}
END }

/************************* Distance(s) ***************************************/

FORM (NEW_Distance_to_ScalarProduct, U"Distance: To ScalarProduct", U"Distance: To ScalarProduct...") {
	BOOLEANVAR (scaleSumOfSquares, U"Make sum of squares equal 1.0", true)
	OK
DO
	LOOP {
		iam (Distance);
		praat_new (Distance_to_ScalarProduct (me, scaleSumOfSquares), my name);
	}
END }

DIRECT (NEW_Distance_to_Dissimilarity) {
	LOOP {
		iam (Distance);
		praat_new (Distance_to_Dissimilarity (me), my name);
	}
END }

FORM (NEWMANY_old_Distances_to_Configuration_indscal, U"Distance: To Configuration (indscal)", U"Distance: To Configuration (indscal)...") {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	LABEL (U"", U"Minimization parameters")
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100 (= each repetition)")
	NATURALVAR (numberOfRepetitions, U"Number of repetitions", U"1")
	OK
DO
	autoDistanceList distances = DistanceList_create ();
	LOOP {
		iam (Distance);
		distances -> addItem_ref (me);
	}
	autoConfiguration configurationResult;
	autoSalience salienceResult;
	DistanceList_to_Configuration_indscal (distances.get(), numberOfDimensions, normalizeScalarProducts, tolerance, maximumNumberOfIterations, numberOfRepetitions,true /* showProgress */, & configurationResult, & salienceResult);
	praat_new (configurationResult.move(), U"indscal");
	praat_new (salienceResult.move(), U"indscal");
END }

FORM (NEWMANY_Distances_to_Configuration_indscal, U"Distance: To Configuration (indscal)", U"Distance: To Configuration (indscal)...") {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	LABEL (U"", U"Minimization parameters")
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100 (= each repetition)")
	NATURALVAR (numberOfRepetitions, U"Number of repetitions", U"1")
	BOOLEANVAR (wantSalience, U"Want Salience", true)
	BOOLEANVAR (showProgressInfo, U"Show progress info", false)
	OK
DO_ALTERNATIVE (NEWMANY_old_Distances_to_Configuration_indscal)
	autoDistanceList distances = DistanceList_create ();
	LOOP {
		iam (Distance);
		distances -> addItem_ref (me);
	}
	autoConfiguration configurationResult;
	autoSalience salienceResult;
	DistanceList_to_Configuration_indscal (distances.get(), numberOfDimensions, normalizeScalarProducts, tolerance, maximumNumberOfIterations, numberOfRepetitions, showProgressInfo, & configurationResult, (wantSalience ? & salienceResult: nullptr));
	praat_new (configurationResult.move(), U"indscal");
	if (wantSalience) {
		praat_new (salienceResult.move(), U"indscal");
	}
END }

FORM (GRAPHICS_Distance_and_Configuration_drawScatterDiagram, U"Distance & Configuration: Draw scatter diagram", U"Distance & Configuration: Draw scatter diagram...") {
	REALVAR (xmin, U"Minimum x-distance", U"0.0")
	REALVAR (xmax, U"Maximum x-distance", U"0.0")
	REALVAR (ymin, U"Minimum y-distance", U"0.0")
	REALVAR (ymax, U"Maximum y-distance", U"0.0")
	POSITIVEVAR (markSize, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (markString, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	Distance me = FIRST (Distance);
	Configuration thee = FIRST (Configuration);
	Distance_and_Configuration_drawScatterDiagram (me, thee, GRAPHICS, xmin, xmax, ymin, ymax, markSize, markString, garnish);
END }

FORM (NEWMANY_Distance_Configuration_indscal, U"Distance & Configuration: To Configuration (indscal)", U"Distance & Configuration: To Configuration (indscal)...") {
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	LABEL (U"", U"Minimization parameters")
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100 (= each repetition)")
	OK
DO
	autoDistanceList distances = DistanceList_create ();
	Configuration configuration = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classDistance) {
			distances -> addItem_ref ((Distance) me);
		} else if (CLASS == classConfiguration) {
			configuration = (Configuration) me;
		}
	}
	Melder_assert (distances->size > 0 && configuration);
	autoConfiguration configurationResult;
	autoSalience salienceResult;
	DistanceList_Configuration_indscal (distances.get(), configuration, normalizeScalarProducts, tolerance, maximumNumberOfIterations,		true /* showProgress */, & configurationResult, & salienceResult);
	praat_new (configurationResult.move(), U"indscal");
	praat_new (salienceResult.move(), U"indscal");
END }

FORM (REAL_Distance_Configuration_vaf, U"Distance & Configuration: Get VAF", U"Distance & Configuration: Get VAF...") {
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	OK
DO
	autoDistanceList distances = DistanceList_create ();
	Configuration configuration = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classDistance) {
			distances -> addItem_ref ((Distance) me);
		} else if (CLASS == classConfiguration) {
			configuration = (Configuration) me;
		}
	}
	Melder_assert (distances->size > 0 && configuration);
	double varianceAccountedFor;
	DistanceList_Configuration_vaf (distances.get(), configuration, normalizeScalarProducts, & varianceAccountedFor);
	Melder_information (varianceAccountedFor);
END }

FORM (REAL_Distance_Configuration_Salience_vaf, U"Distance & Configuration & Salience: Get VAF", U"Distance & Configuration & Salience: Get VAF...") {
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	OK
DO
	autoDistanceList distances = DistanceList_create ();
	Configuration configuration = nullptr;
	Salience salience = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classDistance) {
			distances -> addItem_ref ((Distance) me);
		} else if (CLASS == classConfiguration) {
			configuration = (Configuration) me;
		} else if (CLASS == classSalience) {
			salience = (Salience) me;
		}
	}
	Melder_assert (distances -> size > 0 && configuration && salience);
	double varianceAccountedFor;
	DistanceList_Configuration_Salience_vaf (distances.get(), configuration, salience, normalizeScalarProducts, & varianceAccountedFor);
	Melder_information (varianceAccountedFor);
END }

FORM (REAL_Dissimilarity_Configuration_Salience_vaf, U"Dissimilarity & Configuration & Salience: Get VAF", U"Dissimilarity & Configuration & Salience: Get VAF...") {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	OK
DO
	autoDissimilarityList dissimilarities = DissimilarityList_create ();
	Configuration configuration = nullptr;
	Salience salience = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classDissimilarity) {
			dissimilarities -> addItem_ref ((Dissimilarity) me);
		} else if (CLASS == classConfiguration) {
			configuration = (Configuration) me;
		} else if (CLASS == classSalience) {
			salience = (Salience) me;
		}
	}
	Melder_assert (dissimilarities->size > 0 && configuration && salience);
	double varianceAccountedFor;
	DissimilarityList_Configuration_Salience_vaf (dissimilarities.get(), configuration, salience ,tiesHandling, normalizeScalarProducts, & varianceAccountedFor);
	Melder_information (varianceAccountedFor);
END }

FORM (NEWMANY_Distance_Configuration_Salience_indscal, U"Distance & Configuration & Salience: To Configuration (indscal)", U"Distance & Configuration & Salience: To Configuration (indscal)...") {
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	LABEL (U"", U"Minimization parameters")
	REALVAR (tolerance, U"Tolerance", U"1e-5")
	NATURALVAR (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	OK
DO
	autoDistanceList thee = DistanceList_create ();
	Configuration configuration = nullptr;
	Salience salience = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classDistance) {
			thy addItem_ref ((Distance) me);
		} else if (CLASS == classConfiguration) {
			configuration = (Configuration) me;
		} else if (CLASS == classSalience) {
			salience = (Salience) me;
		}
	}
	Melder_assert (thy size > 0 && configuration && salience);
	double varianceAccountedFor;
	autoConfiguration configurationResult;
	autoSalience salienceResult;
	DistanceList_Configuration_Salience_indscal (thee.get(), configuration, salience, normalizeScalarProducts, tolerance, maximumNumberOfIterations, true, & configurationResult, & salienceResult, & varianceAccountedFor);
	praat_new (configurationResult.move(), U"indscal");
	praat_new (salienceResult.move(), U"indscal");
END }

FORM (NEWMANY_Distances_to_Configuration_ytl, U"Distance: To Configuration (ytl)", U"Distance: To Configuration (ytl)...") {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	BOOLEANVAR (normalizeScalarProducts, U"Normalize scalar products", true)
	BOOLEANVAR (wantSalienceObject, U"Salience object", false)
	OK
DO
	autoDistanceList distances = DistanceList_create ();
	LOOP {
		iam (Distance);
		distances -> addItem_ref (me);
	}
	autoConfiguration configurationResult;
	autoSalience salienceResult;
	DistanceList_to_Configuration_ytl (distances.get(), numberOfDimensions, normalizeScalarProducts, & configurationResult, & salienceResult);
	praat_new (configurationResult.move(), U"ytl");
	if (wantSalienceObject) {
		praat_new (salienceResult.move(), U"ytl");
	}
END }

FORM (NEW1_Dissimilarity_Distance_monotoneRegression, U"Dissimilarity & Distance: Monotone regression", nullptr) {
	RADIOVAR (tiesHandling, U"Handling of ties", 1)
		RADIOBUTTON (U"Primary approach")
		RADIOBUTTON (U"Secondary approach")
	OK
DO
	Dissimilarity me = FIRST (Dissimilarity);
	Distance thee = FIRST (Distance);
	praat_new (Dissimilarity_Distance_monotoneRegression (me, thee, tiesHandling), my name);
END }

FORM (GRAPHICS_Distance_Dissimilarity_drawShepardDiagram, U"Distance & Dissimilarity: Draw Shepard diagram", nullptr) {
	REALVAR (fromDissimilarity, U"left dissimilarity range", U"0.0")
	REALVAR (toDissimilarity, U"right dissimilarity range", U"0.0")
	REALVAR (fromDistance, U"left Distance range", U"0.0")
	REALVAR (toDistance, U"right Distance range", U"0.0")
	POSITIVEVAR (markSize, U"Mark size (mm)", U"1.0")
	SENTENCEVAR (markString, U"Mark string (+xo.)", U"+")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	Dissimilarity me = FIRST (Dissimilarity);
	Distance thee = FIRST (Distance);
	Proximity_Distance_drawScatterDiagram (me, thee, GRAPHICS, fromDissimilarity, toDissimilarity, fromDistance, toDistance, markSize, markString, garnish);
END }

DIRECT (HELP_MDS_help) {
	Melder_help (U"Multidimensional scaling");
END }


/************************* Salience ***************************************/


FORM (GRAPHICS_Salience_draw, U"Salience: Draw", nullptr) {
	NATURALVAR (horizontalDimension, U"Horizontal dimension", U"1")
	NATURALVAR (verticalDimension, U"Vertical dimension", U"2")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Salience);
		Salience_draw (me, GRAPHICS, horizontalDimension, verticalDimension, garnish);
	}
END }

/************************* COVARIANCE & CONFIGURATION  ********************/

FORM (NEW_Covariance_to_Configuration, U"Covariance: To Configuration", nullptr) {
	NATURALVAR (numberOfDimensions, U"Number of dimensions", U"2")
	OK
DO
	LOOP {
		iam (Covariance);
		autoConfiguration result = Covariance_to_Configuration (me, numberOfDimensions);
		praat_new (result.move(), my name);
	}
END }

/********* Procrustes ***************************/

DIRECT (HELP_Procrustes_help) {
	Melder_help (U"Procrustes");
END }

DIRECT (REAL_Procrustes_getScale) {
	LOOP {
		iam (Procrustes);
		Melder_information (my s);
	}
END }

/********* Casts from & to TableOfReal ***************************/

DIRECT (NEW_TableOfReal_to_Dissimilarity) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Dissimilarity (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_Similarity) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Similarity (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_Distance) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Distance (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_Salience) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Salience (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_Weight) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_Weight (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_ScalarProduct) {
	LOOP {
		iam (TableOfReal);
		praat_new (TableOfReal_to_ScalarProduct (me), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_Configuration) {
	LOOP {
		iam (TableOfReal);
		autoConfiguration result = TableOfReal_to_Configuration (me);
		praat_new (result.move(), my name);
	}
END }

DIRECT (NEW_TableOfReal_to_ContingencyTable) {
	LOOP {
		iam (TableOfReal);
		autoContingencyTable result = TableOfReal_to_ContingencyTable (me);
		praat_new (result.move(), my name);
	}
END }

/********************** TableOfReal ***************************************/

DIRECT (REAL_TableOfReal_getTableNorm) {
	LOOP {
		iam (TableOfReal);
		Melder_information (TableOfReal_getTableNorm (me));
	}
END }

FORM (MODIFY_TableOfReal_normalizeTable, U"TableOfReal: Normalize table", U"TableOfReal: Normalize table...") {
	POSITIVEVAR (norm, U"Norm", U"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeTable (me, norm);
	}
END }

FORM (MODIFY_TableOfReal_normalizeRows, U"TableOfReal: Normalize rows", U"TableOfReal: Normalize rows...") {
	POSITIVEVAR (norm, U"Norm", U"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeRows (me, norm);
	}
END }

FORM (MODIFY_TableOfReal_normalizeColumns, U"TableOfReal: Normalize columns", U"TableOfReal: Normalize columns...") {
	POSITIVEVAR (norm, U"Norm", U"1.0")
	OK
DO
	LOOP {
		iam (TableOfReal);
		TableOfReal_normalizeColumns (me, norm);
	}
END }

DIRECT (MODIFY_TableOfReal_centreRows) {
	LOOP {
		iam (TableOfReal);
		TableOfReal_centreRows (me);
	}
END }

DIRECT (MODIFY_TableOfReal_centreColumns) {
	LOOP {
		iam (TableOfReal);
		TableOfReal_centreColumns (me);
	}
END }

DIRECT (MODIFY_TableOfReal_doubleCentre) {
	LOOP {
		iam (TableOfReal);
		TableOfReal_doubleCentre (me);
	}
END }

DIRECT (MODIFY_TableOfReal_standardizeRows) {
	LOOP {
		iam (TableOfReal);
		TableOfReal_standardizeRows (me);
	}
END }

DIRECT (MODIFY_TableOfReal_standardizeColumns) {
	LOOP {
		iam (TableOfReal);
		TableOfReal_standardizeColumns (me);
	}
END }

DIRECT (NEW_TableOfReal_to_Confusion) {
	LOOP {
		iam (TableOfReal);
		autoConfusion result = TableOfReal_to_Confusion (me);
		praat_new (result.move(), my name);
	}
END }

static void praat_AffineTransform_init (ClassInfo klas) {
	praat_addAction1 (klas, 0, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (klas, 1, U"Get transformation element...", QUERY_BUTTON, 1, REAL_AffineTransform_getTransformationElement);
	praat_addAction1 (klas, 1, U"Get translation element...", QUERY_BUTTON, 1, REAL_AffineTransform_getTranslationElement);
	praat_addAction1 (klas, 0, U"Invert", nullptr, 0, NEW_AffineTransform_invert);
}

static void praat_Configuration_and_AffineTransform_init (ClassInfo transform) {
	praat_addAction2 (classConfiguration, 1, transform, 1, U"To Configuration", nullptr, 0, NEW1_Configuration_and_AffineTransform_to_Configuration);
}

void praat_TableOfReal_extras (ClassInfo klas);
void praat_TableOfReal_extras (ClassInfo klas) {
	praat_addAction1 (klas, 1, U"-- get additional --", U"Get value...", 1, nullptr);
	praat_addAction1 (klas, 1, U"Get table norm", U"-- get additional --", 1, REAL_TableOfReal_getTableNorm);
	praat_addAction1 (klas, 0, U"-- set additional --", U"Set column label (label)...", 1, nullptr);
	praat_addAction1 (klas, 0, U"Normalize rows...", U"-- set additional --", 1, MODIFY_TableOfReal_normalizeRows);
	praat_addAction1 (klas, 0, U"Normalize columns...", U"Normalize rows...", 1, MODIFY_TableOfReal_normalizeColumns);
	praat_addAction1 (klas, 0, U"Normalize table...", U"Normalize columns...", 1, MODIFY_TableOfReal_normalizeTable);
	praat_addAction1 (klas, 0, U"Standardize rows", U"Normalize table...", 1, MODIFY_TableOfReal_standardizeRows);
	praat_addAction1 (klas, 0, U"Standardize columns", U"Standardize rows", 1, MODIFY_TableOfReal_standardizeColumns);
	praat_addAction1 (klas, 0, U"Test sorting...", U"Standardize columns", praat_DEPTH_1 + praat_HIDDEN + praat_NO_API, MODIFY_TabelOfReal_testSorting);
}

void praat_uvafon_MDS_init ();
void praat_uvafon_MDS_init () {
	Thing_recognizeClassesByName (classAffineTransform, classProcrustes, classContingencyTable, classDissimilarity,
		classSimilarity, classConfiguration, classDistance, classSalience, classScalarProduct, classWeight, nullptr);
	Thing_recognizeClassByOtherName (classProcrustes, U"Procrustus");

	praat_addMenuCommand (U"Objects", U"New", U"Multidimensional scaling", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"MDS tutorial", nullptr, praat_DEPTH_1 | praat_NO_API, HELP_MDS_help);
	praat_addMenuCommand (U"Objects", U"New", U"-- MDS --", nullptr, 1, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create letter R example...", nullptr, 1, NEW_Dissimilarity_createLetterRExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create INDSCAL Carroll Wish example...", nullptr, 1, NEWMANY_INDSCAL_createCarrollWishExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create Configuration...", nullptr, 1, NEW_Configuration_create);
	praat_addMenuCommand (U"Objects", U"New", U"Draw splines...", nullptr, 1, GRAPHICS_drawSplines);
	praat_addMenuCommand (U"Objects", U"New", U"Draw MDS class relations", nullptr, 1, GRAPHICS_drawMDSClassRelations);

	/****** 1 class ********************************************************/

	praat_addAction1 (classAffineTransform, 0, U"AffineTransform help", nullptr, 0, HELP_AffineTransform_help);
	praat_AffineTransform_init (classAffineTransform);


	praat_addAction1 (classConfiguration, 0, U"Configuration help", nullptr, 0, HELP_Configuration_help);
	praat_TableOfReal_init2 (classConfiguration);
	praat_TableOfReal_extras (classConfiguration);
	(void) praat_removeAction (classConfiguration, nullptr, nullptr, U"Insert column (index)...");
	(void) praat_removeAction (classConfiguration, nullptr, nullptr, U"Remove column (index)...");
	(void) praat_removeAction (classConfiguration, nullptr, nullptr, U"Append");
	praat_addAction1 (classConfiguration, 0, U"Draw...", DRAW_BUTTON, 1, GRAPHICS_Configuration_draw);
	praat_addAction1 (classConfiguration, 0, U"Draw sigma ellipses...", U"Draw...", 1, GRAPHICS_Configuration_drawSigmaEllipses);
	praat_addAction1 (classConfiguration, 0, U"Draw one sigma ellipse...", U"Draw...", 1, GRAPHICS_Configuration_drawOneSigmaEllipse);
	praat_addAction1 (classConfiguration, 0, U"Draw confidence ellipses...", U"Draw sigma ellipses...", 1, GRAPHICS_Configuration_drawConfidenceEllipses);
	praat_addAction1 (classConfiguration, 0, U"Draw one confidence ellipse...", U"Draw sigma ellipses...", 1, GRAPHICS_Configuration_drawOneConfidenceEllipse);

	praat_addAction1 (classConfiguration, 0, U"Randomize", U"Normalize table...", 1, MODIFY_Configuration_randomize);
	praat_addAction1 (classConfiguration, 0, U"Normalize...", U"Randomize", 1, MODIFY_Configuration_normalize);
	praat_addAction1 (classConfiguration, 0, U"Centralize", U"Randomize", 1, MODIFY_Configuration_centralize);
	praat_addAction1 (classConfiguration, 1, U"-- set rotations & reflections --", U"Centralize", 1, nullptr);

	praat_addAction1 (classConfiguration, 0, U"Rotate...", U"-- set rotations & reflections --", 1, MODIFY_Configuration_rotate);
	praat_addAction1 (classConfiguration, 0, U"Rotate (pc)", U"Rotate...", 1, MODIFY_Configuration_rotateToPrincipalDirections);
	praat_addAction1 (classConfiguration, 0, U"Invert dimension...", U"Rotate (pc)", 1, MODIFY_Configuration_invertDimension);
	praat_addAction1 (classConfiguration, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classConfiguration, 0, U"To Distance", nullptr, 0, NEW_Configuration_to_Distance);
	praat_addAction1 (classConfiguration, 0, U"To Configuration (varimax)...", nullptr, 0, NEW_Configuration_varimax);
	praat_addAction1 (classConfiguration, 0, U"To Similarity (cc)", nullptr, 0, NEW_Configurations_to_Similarity_cc);

	praat_addAction1 (classConfiguration, 0, U"Match configurations -", nullptr, 0, nullptr);
	praat_addAction1 (classConfiguration, 2, U"To Procrustes...", nullptr, 1, NEW1_Configurations_to_Procrustes);
	praat_addAction1 (classConfiguration, 2, U"To AffineTransform (congruence)...", nullptr, 1, NEW1_Configurations_to_AffineTransform_congruence);

	praat_addAction1 (classConfusion, 0, U"To ContingencyTable", U"To Matrix", 0, NEW_Confusion_to_ContingencyTable);
	praat_addAction1 (classConfusion, 0, U"To Proximity -", U"Analyse", 0, nullptr);
	praat_addAction1 (classConfusion, 0, U"To Dissimilarity (pdf)...", U"To Proximity -", 1, NEW_Confusion_to_Dissimilarity_pdf);
	praat_addAction1 (classConfusion, 0, U"To Similarity...", U"To Proximity -", 1, NEW_Confusion_to_Similarity);
	praat_addAction1 (classConfusion, 0, U"Sum", U"Synthesize -", 1, NEW1_Confusions_sum);


	praat_TableOfReal_init2 (classContingencyTable);
	praat_addAction1 (classContingencyTable, 1, U"-- statistics --", U"Get value...", 1, nullptr);
	praat_addAction1 (classContingencyTable, 1, U"Get chi squared probability", U"-- statistics --", 1, REAL_ContingencyTable_chisqProbability);
	praat_addAction1 (classContingencyTable, 1, U"Get Cramer's statistic", U"Get chi squared probability", 1, REAL_ContingencyTable_cramersStatistic);
	praat_addAction1 (classContingencyTable, 1, U"Get contingency coefficient", U"Get Cramer's statistic", 1, REAL_ContingencyTable_contingencyCoefficient);
	praat_addAction1 (classContingencyTable, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classContingencyTable, 1, U"To Configuration (ca)...", nullptr, 0, NEW_ContingencyTable_to_Configuration_ca);


	praat_addAction1 (classCorrelation, 0, U"To Configuration...", nullptr, 0, NEW_Correlation_to_Configuration);

	praat_addAction1 (classDissimilarity, 0, U"Dissimilarity help", nullptr, 0, HELP_Dissimilarity_help);
	praat_TableOfReal_init2 (classDissimilarity);
	praat_TableOfReal_extras (classDissimilarity);
	praat_addAction1 (classDissimilarity, 0, U"Get additive constant", U"Get table norm", 1, REAL_Dissimilarity_getAdditiveConstant);
	praat_addAction1 (classDissimilarity, 0, CONFIGURATION_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (monotone mds)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_monotone_mds);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (i-spline mds)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_ispline_mds);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (interval mds)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_interval_mds);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (ratio mds)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_ratio_mds);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (absolute mds)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_absolute_mds);
	praat_addAction1 (classDissimilarity, 1, U"To Configuration (kruskal)...", nullptr, 1, NEW_Dissimilarity_to_Configuration_kruskal);
	praat_addAction1 (classDissimilarity, 0, U"To Distance...", nullptr, 0, NEW_Dissimilarity_to_Distance);
	praat_addAction1 (classDissimilarity, 0, U"To Weight", nullptr, 0, NEW_Dissimilarity_to_Weight);


	praat_addAction1 (classCovariance, 0, U"To Configuration...", nullptr, 0, NEW_Covariance_to_Configuration);


	praat_TableOfReal_init2 (classDistance);
	praat_TableOfReal_extras (classDistance);
	praat_addAction1 (classDistance, 0, U"Analyse -", nullptr, 0, nullptr);
	praat_addAction1 (classDistance, 0, CONFIGURATION_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classDistance, 0, U"To Configuration (indscal)...", nullptr, 1, NEWMANY_Distances_to_Configuration_indscal);
	praat_addAction1 (classDistance, 0, U"-- linear scaling --", nullptr, 1, nullptr);
	praat_addAction1 (classDistance, 0, U"To Configuration (ytl)...", nullptr, 1, NEWMANY_Distances_to_Configuration_ytl);
	praat_addAction1 (classDistance, 0, U"To Dissimilarity", nullptr, 0, NEW_Distance_to_Dissimilarity);
	praat_addAction1 (classDistance, 0, U"To ScalarProduct...", nullptr, 0, NEW_Distance_to_ScalarProduct);


	praat_addAction1 (classProcrustes, 0, U"Procrustes help", nullptr, 0, HELP_Procrustes_help);
	praat_AffineTransform_init (classProcrustes);
	praat_addAction1 (classProcrustes, 1, U"Get scale", QUERY_BUTTON, 1, REAL_Procrustes_getScale);
	praat_addAction1 (classProcrustes, 0, U"Extract transformation matrix", nullptr, 0, NEW_AffineTransform_extractMatrix);
	praat_addAction1 (classProcrustes, 0, U"Extract translation vector", nullptr, 0, NEW_AffineTransform_extractTranslationVector);

	praat_TableOfReal_init2 (classSalience);
	praat_TableOfReal_extras (classSalience);
	praat_addAction1 (classSalience, 0, U"Draw...", DRAW_BUTTON, 1, GRAPHICS_Salience_draw);


	praat_addAction1 (classSimilarity, 0, U"Similarity help", nullptr, 0, HELP_Similarity_help);
	praat_TableOfReal_init2 (classSimilarity);
	praat_TableOfReal_extras (classSimilarity);
	praat_addAction1 (classSimilarity, 0, U"Analyse -", nullptr, 0, nullptr);
	praat_addAction1 (classSimilarity, 0, U"To Dissimilarity...", nullptr, 0, NEW_Similarity_to_Dissimilarity);


	praat_TableOfReal_init2 (classScalarProduct);
	praat_TableOfReal_extras (classScalarProduct);

	praat_TableOfReal_extras (classTableOfReal);
	praat_addAction1 (classTableOfReal, 1, U"Centre rows", U"Normalize table...", 1, MODIFY_TableOfReal_centreRows);
	praat_addAction1 (classTableOfReal, 1, U"Centre columns", U"Centre rows", 1, MODIFY_TableOfReal_centreColumns);
	praat_addAction1 (classTableOfReal, 1, U"Double centre", U"Centre columns", 1, MODIFY_TableOfReal_doubleCentre);
	praat_addAction1 (classTableOfReal, 0, U"Cast -", nullptr, 0, nullptr);
	praat_addAction1 (classTableOfReal, 0, U"To Confusion", nullptr, 1, NEW_TableOfReal_to_Confusion);
	praat_addAction1 (classTableOfReal, 0, U"To Dissimilarity", nullptr, 1, NEW_TableOfReal_to_Dissimilarity);
	praat_addAction1 (classTableOfReal, 0, U"To Similarity", nullptr, 1, NEW_TableOfReal_to_Similarity);
	praat_addAction1 (classTableOfReal, 0, U"To Distance", nullptr, 1, NEW_TableOfReal_to_Distance);
	praat_addAction1 (classTableOfReal, 0, U"To Salience", nullptr, 1, NEW_TableOfReal_to_Salience);
	praat_addAction1 (classTableOfReal, 0, U"To Weight", nullptr, 1, NEW_TableOfReal_to_Weight);
	praat_addAction1 (classTableOfReal, 0, U"To ScalarProduct", nullptr, 1, NEW_TableOfReal_to_ScalarProduct);
	praat_addAction1 (classTableOfReal, 0, U"To Configuration", nullptr, 1, NEW_TableOfReal_to_Configuration);
	praat_addAction1 (classTableOfReal, 0, U"To ContingencyTable", nullptr, 1, NEW_TableOfReal_to_ContingencyTable);

	praat_TableOfReal_init2 (classWeight);

	/****** 2 classes ********************************************************/

	praat_Configuration_and_AffineTransform_init (classAffineTransform);
	praat_Configuration_and_AffineTransform_init (classProcrustes);

	praat_addAction2 (classConfiguration, 0, classWeight, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction2 (classConfiguration, 0, classWeight, 1, U"To Similarity (cc)", nullptr, 0, NEW1_Configuration_Weight_to_Similarity_cc);

	praat_addAction2 (classDissimilarity, 1, classWeight, 1, ANALYSE_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, U"To Configuration (monotone mds)...", nullptr, 1, NEW1_Dissimilarity_Weight_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, U"To Configuration (i-spline mds)...", nullptr, 1, NEW1_Dissimilarity_Weight_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, U"To Configuration (interval mds)...", nullptr, 1, NEW1_Dissimilarity_Weight_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, U"To Configuration (ratio mds)...", nullptr, 1, NEW1_Dissimilarity_Weight_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classWeight, 1, U"To Configuration (absolute mds)...", nullptr, 1, NEW1_Dissimilarity_Weight_absolute_mds);


	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw Shepard diagram...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"-- draw regressions --", nullptr, 1, nullptr);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw monotone regression...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawMonotoneRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw i-spline regression...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawISplineRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw interval regression...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawIntervalRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw ratio regression...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawRatioRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Draw absolute regression...", nullptr, 1, GRAPHICS_Dissimilarity_Configuration_drawAbsoluteRegression);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress (monotone mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_monotone_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress (i-spline mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_ispline_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress (interval mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_interval_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress (ratio mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_ratio_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress (absolute mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_absolute_stress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"Get stress...", nullptr, 1, REAL_Dissimilarity_Configuration_getStress);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, ANALYSE_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (monotone mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_monotone_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (i-spline mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_ispline_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (interval mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_interval_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (ratio mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_ratio_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (absolute mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_absolute_mds);
	praat_addAction2 (classDissimilarity, 1, classConfiguration, 1, U"To Configuration (kruskal)...", nullptr, 1, NEW1_Dissimilarity_Configuration_kruskal);

	praat_addAction2 (classDistance, 1, classConfiguration, 1, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, U"Draw scatter diagram...", nullptr, 0, GRAPHICS_Distance_and_Configuration_drawScatterDiagram);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, U"Get VAF...", nullptr, 0, REAL_Distance_Configuration_vaf);
	praat_addAction2 (classDistance, 1, classConfiguration, 1, ANALYSE_BUTTON, nullptr, 0, nullptr);
	praat_addAction2 (classDistance, 0, classConfiguration, 1, U"To Configuration (indscal)...", nullptr, 1, NEWMANY_Distance_Configuration_indscal);

	praat_addAction2 (classDistance, 1, classDissimilarity, 1, U"Draw Shepard diagram...", nullptr, 0, GRAPHICS_Distance_Dissimilarity_drawShepardDiagram);
	praat_addAction2 (classDissimilarity, 1, classDistance, 1, U"Monotone regression...", nullptr, 0, NEW1_Dissimilarity_Distance_monotoneRegression);

	/****** 3 classes ********************************************************/

	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, classSalience, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction3 (classDissimilarity, 0, classConfiguration, 1, classSalience, 1, U"Get VAF...", nullptr, 1, REAL_Dissimilarity_Configuration_Salience_vaf);

	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"Get stress (monotone mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_Weight_monotone_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"Get stress (i-spline mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_Weight_ispline_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"Get stress (interval mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_Weight_interval_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"Get stress (ratio mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_Weight_ratio_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"Get stress (absolute mds)...", nullptr, 1, REAL_Dissimilarity_Configuration_Weight_absolute_stress);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, ANALYSE_BUTTON, nullptr, 0, nullptr);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"To Configuration (monotone mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_Weight_monotone_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"To Configuration (i-spline mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_Weight_ispline_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"To Configuration (interval mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_Weight_interval_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"To Configuration (ratio mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_Weight_ratio_mds);
	praat_addAction3 (classDissimilarity, 1, classConfiguration, 1, classWeight, 1, U"To Configuration (absolute mds)...", nullptr, 1, NEW1_Dissimilarity_Configuration_Weight_absolute_mds);


	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, QUERY_BUTTON, nullptr, 0, nullptr);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, U"Get VAF...", nullptr, 1, REAL_Distance_Configuration_Salience_vaf);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction3 (classDistance, 0, classConfiguration, 1, classSalience, 1, U"To Configuration (indscal)...", nullptr, 0, NEWMANY_Distance_Configuration_Salience_indscal);

	INCLUDE_MANPAGES (manual_MDS_init)
}

/* End of file praat_MDS_init.c 2010*/
