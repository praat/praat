#ifndef _MDS_h_
#define _MDS_h_
/* MDS.h
 *
 * Multi Dimensional Scaling
 *
 * Copyright (C) 1993-2011 David Weenink
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

Weight Weight_create (long numberOfPoints);

/************************** class Salience **************************************/

Thing_define (Salience, TableOfReal) {
};

Salience Salience_create (long numberOfSources, long numberOfDimensions);

void Salience_setDefaults (Salience me);

long Salience_correctNegatives (Salience me);

void Salience_draw (Salience me, Graphics g, int xdimension, int ydimension,
	int garnish);

/************************** class MDSVec ******************************/

Thing_define (MDSVec, Data) {
	// new data:
	public:
		long nProximities, nPoints;
		double *proximity;
		long *iPoint, *jPoint;
	// overridden methods:
	protected:
		virtual void v_destroy ();
};

MDSVec MDSVec_create (long nObjects);

/************** class MDSVecs *********************************/

Thing_define (MDSVecs, Ordered) {
};

MDSVecs MDSVecs_create ();


Configuration ContingencyTable_to_Configuration_ca (ContingencyTable me,
	long numberOfDimensions, int scaling);

/********************* class Proximities *******************************/

Thing_define (Proximities, TablesOfReal) {
};

void Proximities_init (I, ClassInfo klas);

Proximities Proximities_create ();

/****************** class Confusions **********************************/

Thing_define (Confusions, Proximities) {
};

Confusions Confusions_create ();

Confusion Confusions_sum (Confusions me);

/************* class Distances **************************************/

Thing_define (Distances, Proximities) {
};

Distances Distances_create ();

/**************** class ScalarProduct **************************************/

Thing_define (ScalarProduct, TableOfReal) {
};

ScalarProduct ScalarProduct_create (long numberOfPoints);

/************** class ScalarProducts ********************************/

Thing_define (ScalarProducts, TablesOfReal) {
};

ScalarProducts ScalarProducts_create ();


/************* class Dissimilarity *********************************/

Thing_define (Dissimilarity, Proximity) {
};

Dissimilarity Dissimilarity_create (long numberOfPoints);

int Dissimilarity_getAdditiveConstant (I, double *c);
/*
	Get the best estimate for the additive constant:
		"distance = dissimilarity + constant"
	F. Cailliez (1983), The analytical solution of the additive
		constant problem,
	Psychometrika 48, 305-308.
*/

/****************** class Transformator *******************************/

Thing_define (Transformator, Thing) {
	// new data:
	public:
		long numberOfPoints;
		int normalization;
	// new methods:
		virtual Distance v_transform (MDSVec vec, Distance dist, Weight w);
};

void Transformator_init (I, long numberOfPoints);

Transformator Transformator_create (long numberOfPoints);

void Transformator_setNormalization (I, int normalization);

Distance Transformator_transform (I, MDSVec vec, Distance dist, Weight w);

Thing_define (ISplineTransformator, Transformator) {
	// new data:
	public:
		long numberOfInteriorKnots, order, numberOfParameters;
		double **m, *b, *knot;
	// overridden methods:
		virtual void v_destroy ();
		virtual Distance v_transform (MDSVec vec, Distance dist, Weight w);
};

ISplineTransformator ISplineTransformator_create (long numberOfPoints, long numberOfInteriorKnots,
	long order);

Thing_define (RatioTransformator, Transformator) {
	// new data:
	public:
		double ratio;
	// overridden methods:
		virtual Distance v_transform (MDSVec vec, Distance dist, Weight w);
};

RatioTransformator RatioTransformator_create (long numberOfPoints);

Thing_define (MonotoneTransformator, Transformator) {
	// new data:
	public:
		int tiesProcessing;
	// overridden methods:
		virtual Distance v_transform (MDSVec vec, Distance dist, Weight w);
};

MonotoneTransformator MonotoneTransformator_create (long numberPoints);

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator,
	int tiesProcessing);


/*************** class Dissimilarities ****************************/

Thing_define (Dissimilarities, Proximities) {
};

Dissimilarities Dissimilarities_create ();

/**************** class Similarity *****************************/

Thing_define (Similarity, Proximity) {
};

Similarity Similarity_create (long numberOfPoints);


/************** KRUSKAL *********************************************/

Thing_define (Kruskal, Thing) {
	// new data:
	public:
		int process;
		int measurementLevel;
		int conditionality;
		Configuration configuration;
		Proximities proximities;
		int stress_formula;
		MDSVec vec;
		double **dx;
		Minimizer minimizer;
	// overridden methods:
		virtual void v_destroy ();
};

Kruskal Kruskal_create (long numberOfpoints, long numberOfDimensions);

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

void Distance_Weight_rawStressComponents (Distance fit, Distance conf,
	Weight weight, double *eta_fit, double *eta_conf, double *rho);
/*
	Computes
		eta_fit = sum (i<j,i=1..n; w[i][j] * dfit[i][j]^2)
		eta_conf = sum (i<j,i=1..n; w[i][j] * conf[i][j]^2)
		rho   = sum (i<j,i=1..n; w[i][j] * dfit[i][j] * conf[i][j];
	where,
		dfit[i][j] = transformation (dissimilarity[i][j])
		conf[i][j] = distance between x[i] and x[j] (in the configuration)
*/

double Dissimilarity_Configuration_Transformator_Weight_stress
	(Dissimilarity d, Configuration c, Any t, Weight w, int type);

double Dissimilarity_Configuration_Weight_absolute_stress
	(Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_ratio_stress
	(Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_interval_stress
	(Dissimilarity d, Configuration c, Weight w, int type);

double Dissimilarity_Configuration_Weight_monotone_stress
	(Dissimilarity d, Configuration c, Weight w, int tiesProcessing, int type);

double Dissimilarity_Configuration_Weight_ispline_stress
	(Dissimilarity d, Configuration c, Weight w, long numberOfInteriorKnots,
	long order, int type);

void Distance_Weight_smacofNormalize (Distance d, Weight w);

Configuration Dissimilarity_Configuration_Weight_Transformator_smacof
	(Dissimilarity me, Configuration conf, Weight weight, Any transformator,
	double tolerance, long numberOfIterations, int showProgress,
	double *stress);

Configuration Dissimilarity_Configuration_Weight_Transformator_multiSmacof
	(Dissimilarity me, Configuration conf, Weight w, Any transformator,
	double tolerance, long numberOfIterations, long numberOfRepetitions,
	int showProgress);


Configuration Dissimilarity_Configuration_Weight_absolute_mds
	(Dissimilarity dis, Configuration cstart, Weight w, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Configuration_Weight_ratio_mds
	(Dissimilarity dis, Configuration cstart, Weight w, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Configuration_Weight_interval_mds
	(Dissimilarity dis, Configuration cstart, Weight w, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Configuration_Weight_monotone_mds
	(Dissimilarity dis, Configuration cstart, Weight w, int tiesProcessing,
	double tolerance, long numberOfIterations, long numberOfRepetitions,
	int showProgress);

Configuration Dissimilarity_Configuration_Weight_ispline_mds
	(Dissimilarity me, Configuration cstart, Weight w,
	long numberOfInteriorKnots, long order, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Weight_absolute_mds
	(Dissimilarity me, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Weight_ratio_mds
	(Dissimilarity dis, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);


Configuration Dissimilarity_Weight_interval_mds
	(Dissimilarity dis, Weight w, long numberOfDimensions, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

Configuration Dissimilarity_Weight_monotone_mds
	(Dissimilarity me, Weight w, long numberOfDimensions, int tiesProcessing,
	double tolerance, long numberOfIterations, long numberOfRepetitions,
	int showProgress);

Configuration Dissimilarity_Weight_ispline_mds
	(Dissimilarity me, Weight weight, long numberOfDimensions,
	long numberOfInteriorKnots, long order, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress);

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression
	(Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin,
	double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawRatioRegression
	(Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawIntervalRegression
	(Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawMonotoneRegression
	(Dissimilarity d, Configuration c, Weight w, Graphics g,
	int tiesProcessing, double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish);

void Dissimilarity_Configuration_Weight_drawISplineRegression
	(Dissimilarity d, Configuration c, Weight w, Graphics g,
	long numberOfInternalKnots, long order, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish);

Distance Dissimilarity_Configuration_Transformator_Weight_transform
	(Dissimilarity d, Configuration c, Any t, Weight w);

/******** DISTANCE & SCALARPRODUCT *******************************/


ScalarProduct Distance_to_ScalarProduct (Distance me, int normalize);


/************** DISTANCE & PROXIMITY ********************************/


void Proximity_Distance_drawScatterDiagram (I, Distance thee, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double size_mm,
	const wchar_t *mark, int garnish);


/********** COVARIANCE & CONFIGURATION ***************************/


Configuration Covariance_to_Configuration (Covariance me,
	long numberOfDimensions);


/************ CORRELATION & CONFIGURATION ************************/


Configuration Correlation_to_Configuration (Correlation me,
	long numberOfDimensions);


/************** DISTANCE & CONFIGURATION **************************/


Distance Configuration_to_Distance (Configuration me);
/* Calculates distances between the points */

void Distance_and_Configuration_drawScatterDiagram
	(Distance me, Configuration him, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish);

Configuration Distance_to_Configuration_torsca (Distance me,
	int numberOfDimensions);


/************** DISSIMILARITY & CONFIGURATION ************************/


Configuration Dissimilarity_kruskal (Dissimilarity me, long numberOfDimensions,
	long metric, int processTies, int stress_formula, double tolerance,
	long numberOfIterations, long numberOfRepetitions);

Configuration Dissimilarity_Configuration_kruskal
	(Dissimilarity me, Configuration him, int processTies, int stress_formula,
	double tolerance, long numberOfIterations, long numberOfRepetitions);

double Dissimilarity_Configuration_getStress
	(Dissimilarity me, Configuration him, int processTies, int stress_formula);

void Dissimilarity_Configuration_drawShepardDiagram
	(Dissimilarity me, Configuration him, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish);

Distance Dissimilarity_Configuration_monotoneRegression
	(Dissimilarity dis, Configuration conf, int processTies);

Distances Dissimilarities_Configuration_monotoneRegression
	(Dissimilarities dissims, Configuration conf, int processTies);

void Dissimilarity_Configuration_drawMonotoneRegression
	(Dissimilarity me, Configuration him, Graphics g, int processTies,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish);

/************** SIMILARITY & TABLESOFREAL ********************************/


Similarity Distances_to_Similarity_cc (Distances me, Weight weight);

Similarity Configurations_to_Similarity_cc (Configurations me, Weight weight);


/************** DISSIMILARITY & DISTANCE *************************************/


Distance Dissimilarity_to_Distance (Dissimilarity me, int scale);
/* with optional scaling with "additive constant" */

Dissimilarity Distance_to_Dissimilarity (Distance me);

Distance Dissimilarity_Distance_monotoneRegression
	(Dissimilarity me, Distance thee, int processTies);


/************** DISSIMILARITY & CONFUSION ************************************/


Dissimilarity Confusion_to_Dissimilarity_pdf
	(Confusion me, double minimumConfusionLevel);


/************** DISSIMILARITY & MDSVEC ***************************************/


MDSVec Dissimilarity_to_MDSVec (Dissimilarity me);

MDSVecs Dissimilarities_to_MDSVecs (Dissimilarities me);


/************** DISSIMILARITY & SIMILARITY ***********************************/


Dissimilarity Similarity_to_Dissimilarity (Similarity me,
	double maximumDissimilarity);


/************** DISSIMILARITY & WEIGHT ***********************************/



Weight Dissimilarity_to_Weight (Dissimilarity me);


/************** CONFUSION & SIMILARITY ***************************************/


Similarity Confusion_to_Similarity (Confusion me, int normalize,
	int symmetrizeMethod);


/************** CONFUSION & DISTANCE *****************************************/


Distance Confusion_to_Distance_pdf (Confusion me, int symmetrizeBefore,
	double maximumDistance);


/************** DISSIMILARITIES & DISTANCES **********************************/


Dissimilarities Distances_to_Dissimilarities (Distances me);

Distances Dissimilarities_to_Distances (Dissimilarities me, int scale);


/************** DISTANCES & CONFIGURATION ************************************/


int Distances_to_Configuration_ytl (Distances me, int numberOfDimensions,
	int normalizeScalarProducts, Configuration *out1, Salience *out2);
/*
	F.W. Young, Y. Takane & R. Lewyckyj (1978), Three notes on ALSCAL,
	Psychometrika 43, 433-435.

	 w: numberOfSources x numberOfDimensions or NULL
*/

Distances Configurations_to_Distances (Configurations me);


/************** MDSVEC(S) & DISTANCE(S) **********************************/


Distance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee,
	int processTies);

Distances MDSVecs_Distance_monotoneRegression (MDSVecs me, Distance thee,
	int processTies);


/************** SCALARPRODUCT(S) & ...... **********************************/

void ScalarProduct_Configuration_getVariances (ScalarProduct me,
	Configuration thee, double *varianceExplained, double *varianceTotal);

void ScalarProducts_Configuration_Salience_vaf (ScalarProducts me,
	Configuration thee, Salience him, double *vaf);

ScalarProducts Distances_to_ScalarProducts (Distances me, int normalize);

void ScalarProducts_to_Configuration_ytl (ScalarProducts me,
	int numberOfDimensions, Configuration *out1, Salience *out2);

void ScalarProducts_Configuration_Salience_indscal (ScalarProducts sp,
	Configuration conf, Salience weights, double tolerance,
	long numberOfIterations, int showProgress,
	Configuration *out1, Salience *out2, double *vaf);

/************** INDSCAL & ....... ***********************************/


void Dissimilarities_indscal (Dissimilarities me, long numberOfDimensions,
	int processTies, int normalizeScalarProducts, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress,
	Configuration *out1, Salience *out2);

void Distances_indscal (Distances me, long numberOfDimensions,
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	long numberOfRepetitions, int showProgress,
	Configuration *out1, Salience *out2);

void Dissimilarities_Configuration_indscal (Dissimilarities me,
	Configuration conf, int processTies, int normalizeScalarProducts,
	double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2);

void Distances_Configuration_indscal (Distances dists, Configuration conf,
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2);

void Dissimilarities_Configuration_Salience_indscal (Dissimilarities dissims,
	Configuration conf, Salience w, int processTies,
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2, double *vaf);

Distances MDSVecs_Configuration_Salience_monotoneRegression (MDSVecs vecs,
	Configuration conf, Salience weights, int processTies);

void Distances_Configuration_Salience_indscal (Distances dists,
	Configuration conf, Salience weights,
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2, double *vaf);

void Distances_Configuration_Salience_vaf (Distances me, Configuration thee,
	Salience him, int normalizeScalarProducts, double *vaf);

void Dissimilarities_Configuration_Salience_vaf (Dissimilarities me, Configuration thee,
	Salience him, int processTies, int normalizeScalarProducts, double *vaf);

void Distances_Configuration_vaf (Distances me, Configuration thee,
	int normalizeScalarProducts, double *vaf);

void Dissimilarities_Configuration_vaf (Dissimilarities me, Configuration thee,
	int processTies, int normalizeScalarProducts, double *vaf);

Salience ScalarProducts_Configuration_to_Salience (ScalarProducts me,
	Configuration him);

Salience Distances_Configuration_to_Salience (Distances me, Configuration him,
	int normalizeScalarProducts);

Salience Dissimilarities_Configuration_to_Salience (Dissimilarities me,
	Configuration him, int processTies, int normalizeScalarProducts);


/********* Casts from & to TableOfReal *****************************/

Dissimilarity TableOfReal_to_Dissimilarity (I);

Similarity TableOfReal_to_Similarity (I);

Distance TableOfReal_to_Distance (I);

Salience TableOfReal_to_Salience (I);

Weight TableOfReal_to_Weight (I);

ScalarProduct TableOfReal_to_ScalarProduct (I);

/**************  EXAMPLES  ***************************************************/


Dissimilarity Dissimilarity_createLetterRExample (double noiseRange);

Collection INDSCAL_createCarrollWishExample (double noiseStd);

Salience Salience_createCarrollWishExample ();

void drawSplines (Graphics g, double low, double high, double ymin, double ymax,
	int type, long order, wchar_t const *interiorKnots, int garnish);

void drawMDSClassRelations (Graphics g);

#endif /* _MDS_h_ */
