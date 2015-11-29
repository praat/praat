#ifndef _MDS_h_
#define _MDS_h_
/* MDS.h
 *
 * Multi Dimensional Scaling
 *
 * Copyright (C) 1993-2011, 2015 David Weenink
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
 djmw 20110306 Latest modification.
*/

#define KRUSKAL_MAXDIMENSION 10

/* process */
#define MDS_DISCRETE 0
#define MDS_CONTINUOUS 1

/* measurement conditionality */
#define MDS_UNCONDITIONAL 0
#define MDS_MATRIXCONDITIONAL 1
#define MDS_ROWCONDITIONAL 2

/* analysis level */
#define MDS_ABSOLUTE 0
#define MDS_RATIO	1
#define MDS_INTERVAL 2
#define MDS_SPLINE 3
#define MDS_ORDINAL 4
#define MDS_NOMINAL 5

/* normalization */
#define CONFIGURATION_COLUMNS 1
#define CONFIGURATION_MATRIX 2

/* ties processing */
#define MDS_PRIMARY_APPROACH 1
#define MDS_SECONDARY_APPROACH 2

/* stress */
#define MDS_NORMALIZED_STRESS 1
#define MDS_STRESS_1 2
#define MDS_STRESS_2 3
#define MDS_RAW_STRESS 4

#define MDS_MSPLINE 1
#define MDS_ISPLINE 2

#include "Graphics.h"
#include "Minimizers.h"
#include "Confusion.h"
#include "ContingencyTable.h"
#include "TableOfReal_extensions.h"
#include "Proximity.h"
#include "Distance.h"
#include "Configuration.h"
#include "SSCP.h"

/************************** class Weight **************************************/

Thing_define (Weight, TableOfReal) {
};

autoWeight Weight_create (long numberOfPoints);

/************************** class Salience **************************************/

Thing_define (Salience, TableOfReal) {
};

autoSalience Salience_create (long numberOfSources, long numberOfDimensions);

void Salience_setDefaults (Salience me);

long Salience_correctNegatives (Salience me);

void Salience_draw (Salience me, Graphics g, int xdimension, int ydimension, int garnish);

/************************** class MDSVec ******************************/

Thing_define (MDSVec, Daata) {
	// new data:
	public:
		long nProximities, nPoints;
		double *proximity;
		long *iPoint, *jPoint;
	// overridden methods:
	protected:
		virtual void v_destroy ();
};

autoMDSVec MDSVec_create (long nObjects);

/************** class MDSVecs *********************************/

Thing_define (MDSVecs, Ordered) {
};

autoMDSVecs MDSVecs_create ();


autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, long numberOfDimensions, int scaling);

/********************* class Proximities *******************************/

Thing_define (Proximities, TablesOfReal) {
};

void Proximities_init (Proximities me, ClassInfo klas);

autoProximities Proximities_create ();

/****************** class Confusions **********************************/

Thing_define (Confusions, Proximities) {
};

autoConfusions Confusions_create ();

autoConfusion Confusions_sum (Confusions me);

/************* class Distances **************************************/

Thing_define (Distances, Proximities) {
};

autoDistances Distances_create ();

/**************** class ScalarProduct **************************************/

Thing_define (ScalarProduct, TableOfReal) {
};

autoScalarProduct ScalarProduct_create (long numberOfPoints);

/************** class ScalarProducts ********************************/

Thing_define (ScalarProducts, TablesOfReal) {
};

autoScalarProducts ScalarProducts_create ();


/************* class Dissimilarity *********************************/

Thing_define (Dissimilarity, Proximity) {
};

autoDissimilarity Dissimilarity_create (long numberOfPoints);

double Dissimilarity_getAdditiveConstant (Dissimilarity me);
/*
	Get the best estimate for the additive constant:
		"distance = dissimilarity + constant"
	F. Cailliez (1983), The analytical solution of the additive constant problem, Psychometrika 48, 305-308.
*/

/****************** class Transformator *******************************/

Thing_define (Transformator, Thing) {
	// new data:
	public:
		long numberOfPoints;
		int normalization;
	// new methods:
		virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

void Transformator_init (Transformator me, long numberOfPoints);

autoTransformator Transformator_create (long numberOfPoints);

void Transformator_setNormalization (Transformator me, int normalization);

autoDistance Transformator_transform (Transformator me, MDSVec vec, Distance dist, Weight w);

Thing_define (ISplineTransformator, Transformator) {
	// new data:
	public:
		long numberOfInteriorKnots, order, numberOfParameters;
		double **m, *b, *knot;
	// overridden methods:
		virtual void v_destroy ();
		virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

autoISplineTransformator ISplineTransformator_create (long numberOfPoints, long numberOfInteriorKnots, long order);

Thing_define (RatioTransformator, Transformator) {
	// new data:
	public:
		double ratio;
	// overridden methods:
		virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

autoRatioTransformator RatioTransformator_create (long numberOfPoints);

Thing_define (MonotoneTransformator, Transformator) {
	// new data:
	public:
		int tiesProcessing;
	// overridden methods:
		virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

autoMonotoneTransformator MonotoneTransformator_create (long numberPoints);

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator,
	int tiesProcessing);


/*************** class Dissimilarities ****************************/

Thing_define (Dissimilarities, Proximities) {
};

autoDissimilarities Dissimilarities_create ();

/**************** class Similarity *****************************/

Thing_define (Similarity, Proximity) {
};

autoSimilarity Similarity_create (long numberOfPoints);


/************** KRUSKAL *********************************************/

Thing_define (Kruskal, Thing) {
	// new data:
	public:
		int process;
		int measurementLevel;
		int conditionality;
		autoConfiguration configuration;
		autoProximities proximities;
		int stress_formula;
		autoMDSVec vec;
		double **dx;
		autoMinimizer minimizer;
	// overridden methods:
		virtual void v_destroy ();
};

autoKruskal Kruskal_create (long numberOfpoints, long numberOfDimensions);

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress
	(Dissimilarity me, Configuration conf, Weight weight, Transformator t);

double Distance_Weight_stress (Distance fit, Distance c, Weight w, int type);
/*
	Calculates stress.
	type is one of (MDS_NORMALIZED_STRESS, MDS_STRESS_1,
		MDS_STRESS_2, MDS_RAW_STRESS)
*/

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w);
/*
	Congruence coefficient B&G page 350.
*/

void Distance_Weight_rawStressComponents (Distance fit, Distance conf, Weight weight, double *eta_fit, double *eta_conf, double *rho);
/*
	Computes
		eta_fit = sum (i<j,i=1..n; w[i][j] * dfit[i][j]^2)
		eta_conf = sum (i<j,i=1..n; w[i][j] * conf[i][j]^2)
		rho   = sum (i<j,i=1..n; w[i][j] * dfit[i][j] * conf[i][j];
	where,
		dfit[i][j] = transformation (dissimilarity[i][j])
		conf[i][j] = distance between x[i] and x[j] (in the configuration)
*/

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d, Configuration c, Transformator t, Weight w, int type);

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w, int tiesProcessing, int type);

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d, Configuration c, Weight w, long numberOfInteriorKnots, long order, int type);

void Distance_Weight_smacofNormalize (Distance d, Weight w);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf, Weight weight, Transformator t, double tolerance, long numberOfIterations, bool showProgress, double *stress);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf, Weight w, Transformator t,	double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);


autoConfiguration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity dis, Configuration cstart, Weight w, int tiesProcessing, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w,
	long numberOfInteriorKnots, long order, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_ratio_mds (Dissimilarity dis, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress);


autoConfiguration Dissimilarity_Weight_interval_mds (Dissimilarity dis, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_monotone_mds (Dissimilarity me, Weight w, long numberOfDimensions, int tiesProcessing,
	double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight weight, long numberOfDimensions,
	long numberOfInteriorKnots, long order, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin,
	double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	int tiesProcessing, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	long numberOfInternalKnots, long order, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

autoDistance Dissimilarity_Configuration_Transformator_Weight_transform (Dissimilarity d, Configuration c, Transformator t, Weight w);

/******** DISTANCE & SCALARPRODUCT *******************************/


autoScalarProduct Distance_to_ScalarProduct (Distance me, int normalize);


/************** DISTANCE & PROXIMITY ********************************/


void Proximity_Distance_drawScatterDiagram (Proximity me, Distance thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);


/********** COVARIANCE & CONFIGURATION ***************************/

autoConfiguration SSCP_to_Configuration (SSCP me, long numberOfDimensions);

autoConfiguration Covariance_to_Configuration (Covariance me, long numberOfDimensions);


/************ CORRELATION & CONFIGURATION ************************/


autoConfiguration Correlation_to_Configuration (Correlation me, long numberOfDimensions);


/************** DISTANCE & CONFIGURATION **************************/


autoDistance Configuration_to_Distance (Configuration me);
/* Calculates distances between the points */

void Distance_and_Configuration_drawScatterDiagram (Distance me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

autoConfiguration Distance_to_Configuration_torsca (Distance me, int numberOfDimensions);


/************** DISSIMILARITY & CONFIGURATION ************************/


autoConfiguration Dissimilarity_kruskal (Dissimilarity me, long numberOfDimensions, long metric, int processTies, int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions);

autoConfiguration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him, int processTies, int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions);

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, int processTies, int stress_formula);

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

autoDistance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dis, Configuration conf, int processTies);

autoDistances Dissimilarities_Configuration_monotoneRegression (Dissimilarities dissims, Configuration conf, int processTies);

void Dissimilarity_Configuration_drawMonotoneRegression	(Dissimilarity me, Configuration him, Graphics g, int processTies, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);

/************** SIMILARITY & TABLESOFREAL ********************************/


autoSimilarity Distances_to_Similarity_cc (Distances me, Weight weight);

autoSimilarity Configurations_to_Similarity_cc (Configurations me, Weight weight);


/************** DISSIMILARITY & DISTANCE *************************************/


autoDistance Dissimilarity_to_Distance (Dissimilarity me, int scale);
/* with optional scaling with "additive constant" */

autoDissimilarity Distance_to_Dissimilarity (Distance me);

autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, int processTies);


/************** DISSIMILARITY & CONFUSION ************************************/


autoDissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel);


/************** DISSIMILARITY & MDSVEC ***************************************/


autoMDSVec Dissimilarity_to_MDSVec (Dissimilarity me);

autoMDSVecs Dissimilarities_to_MDSVecs (Dissimilarities me);


/************** DISSIMILARITY & SIMILARITY ***********************************/


autoDissimilarity Similarity_to_Dissimilarity (Similarity me, double maximumDissimilarity);


/************** DISSIMILARITY & WEIGHT ***********************************/



autoWeight Dissimilarity_to_Weight (Dissimilarity me);


/************** CONFUSION & SIMILARITY ***************************************/


autoSimilarity Confusion_to_Similarity (Confusion me, int normalize, int symmetrizeMethod);


/************** CONFUSION & DISTANCE *****************************************/


autoDistance Confusion_to_Distance_pdf (Confusion me, int symmetrizeBefore, double maximumDistance);


/************** DISSIMILARITIES & DISTANCES **********************************/


autoDissimilarities Distances_to_Dissimilarities (Distances me);

autoDistances Dissimilarities_to_Distances (Dissimilarities me, int scale);


/************** DISTANCES & CONFIGURATION ************************************/


void Distances_to_Configuration_ytl (Distances me, int numberOfDimensions, int normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2);
/*
	F.W. Young, Y. Takane & R. Lewyckyj (1978), Three notes on ALSCAL,
	Psychometrika 43, 433-435.

	 w: numberOfSources x numberOfDimensions or null
*/

autoDistances Configurations_to_Distances (Configurations me);


/************** MDSVEC(S) & DISTANCE(S) **********************************/


autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, int processTies);

autoDistances MDSVecs_Distance_monotoneRegression (MDSVecs me, Distance thee, int processTies);


/************** SCALARPRODUCT(S) & ...... **********************************/

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *varianceExplained, double *varianceTotal);

void ScalarProducts_Configuration_Salience_vaf (ScalarProducts me, Configuration thee, Salience him, double *vaf);

autoScalarProducts Distances_to_ScalarProducts (Distances me, int normalize);

void ScalarProducts_to_Configuration_ytl (ScalarProducts me, int numberOfDimensions, autoConfiguration *out1, autoSalience *out2);

void ScalarProducts_Configuration_Salience_indscal (ScalarProducts sp, Configuration conf, Salience weights, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

/************** INDSCAL & ....... ***********************************/


void Dissimilarities_indscal (Dissimilarities me, long numberOfDimensions, int processTies, int normalizeScalarProducts, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void Distances_indscal (Distances me, long numberOfDimensions, int normalizeScalarProducts, double tolerance, long numberOfIterations,
	long numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void Dissimilarities_Configuration_indscal (Dissimilarities me, Configuration conf, int processTies, int normalizeScalarProducts,
	double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void Distances_Configuration_indscal (Distances dists, Configuration conf, int normalizeScalarProducts, double tolerance, long numberOfIterations,
	bool showProgress, autoConfiguration *out1, autoSalience *out2);

void Dissimilarities_Configuration_Salience_indscal (Dissimilarities dissims, Configuration conf, Salience w, int processTies,
	int normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

autoDistances MDSVecs_Configuration_Salience_monotoneRegression (MDSVecs vecs, Configuration conf, Salience weights, int processTies);

void Distances_Configuration_Salience_indscal (Distances dists, Configuration conf, Salience weights, int normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

void Distances_Configuration_Salience_vaf (Distances me, Configuration thee, Salience him, int normalizeScalarProducts, double *vaf);

void Dissimilarities_Configuration_Salience_vaf (Dissimilarities me, Configuration thee,
	Salience him, int processTies, int normalizeScalarProducts, double *vaf);

void Distances_Configuration_vaf (Distances me, Configuration thee, int normalizeScalarProducts, double *vaf);

void Dissimilarities_Configuration_vaf (Dissimilarities me, Configuration thee, int processTies, int normalizeScalarProducts, double *vaf);

autoSalience ScalarProducts_Configuration_to_Salience (ScalarProducts me, Configuration him);

autoSalience Distances_Configuration_to_Salience (Distances me, Configuration him, int normalizeScalarProducts);

autoSalience Dissimilarities_Configuration_to_Salience (Dissimilarities me, Configuration him, int processTies, int normalizeScalarProducts);


/********* Casts from & to TableOfReal *****************************/

autoDissimilarity TableOfReal_to_Dissimilarity (TableOfReal me);

autoSimilarity TableOfReal_to_Similarity (TableOfReal me);

autoDistance TableOfReal_to_Distance (TableOfReal me);

autoSalience TableOfReal_to_Salience (TableOfReal me);

autoWeight TableOfReal_to_Weight (TableOfReal me);

autoScalarProduct TableOfReal_to_ScalarProduct (TableOfReal me);

/**************  EXAMPLES  ***************************************************/


autoDissimilarity Dissimilarity_createLetterRExample (double noiseRange);

autoCollection INDSCAL_createCarrollWishExample (double noiseStd);

autoSalience Salience_createCarrollWishExample ();

void drawSplines (Graphics g, double low, double high, double ymin, double ymax, int type, long order, char32 const *interiorKnots, int garnish);

void drawMDSClassRelations (Graphics g);

#endif /* _MDS_h_ */
