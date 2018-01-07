#ifndef _MDS_h_
#define _MDS_h_
/* MDS.h
 *
 * Multi Dimensional Scaling
 *
 * Copyright (C) 1993-2011, 2015-2016 David Weenink, 2015,2017 Paul Boersma
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

autoWeight Weight_create (integer numberOfPoints);

/************************** class Salience **************************************/

Thing_define (Salience, TableOfReal) {
};

autoSalience Salience_create (integer numberOfSources, integer numberOfDimensions);

void Salience_setDefaults (Salience me);

integer Salience_correctNegatives (Salience me);

void Salience_draw (Salience me, Graphics g, int xdimension, int ydimension, bool garnish);

/************************** class MDSVec ******************************/

Thing_define (MDSVec, Daata) {
	integer nProximities, nPoints;
	double *proximity;
	integer *iPoint, *jPoint;

	void v_destroy () noexcept
		override;
};

autoMDSVec MDSVec_create (integer nObjects);

/************** class MDSVecs *********************************/

Collection_define (MDSVecList, OrderedOf, MDSVec) {
};

autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, integer numberOfDimensions, int scaling);

/********************* class ProximityList *******************************/

Collection_define (ProximityList, OrderedOf, Proximity) {
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};


#pragma mark - class ConfusionList

Collection_define (ConfusionList, OrderedOf, Confusion) {
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

autoConfusion ConfusionList_sum (ConfusionList me);


#pragma mark - class DistanceList

Collection_define (DistanceList, OrderedOf, Distance) {
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};


#pragma mark - class ScalarProduct

Thing_define (ScalarProduct, TableOfReal) {
};

autoScalarProduct ScalarProduct_create (integer numberOfPoints);


#pragma mark - class ScalarProductList

Collection_define (ScalarProductList, OrderedOf, ScalarProduct) {
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};


#pragma mark - class Dissimilarity

Thing_define (Dissimilarity, Proximity) {
};

autoDissimilarity Dissimilarity_create (integer numberOfPoints);

double Dissimilarity_getAdditiveConstant (Dissimilarity me);
/*
	Get the best estimate for the additive constant:
		"distance = dissimilarity + constant"
	F. Cailliez (1983), The analytical solution of the additive constant problem, Psychometrika 48, 305-308.
*/


#pragma mark - class Transformator

Thing_define (Transformator, Thing) {
	integer numberOfPoints;
	int normalization;

	virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

void Transformator_init (Transformator me, integer numberOfPoints);

autoTransformator Transformator_create (integer numberOfPoints);

void Transformator_setNormalization (Transformator me, int normalization);

autoDistance Transformator_transform (Transformator me, MDSVec vec, Distance dist, Weight w);

Thing_define (ISplineTransformator, Transformator) {
	integer numberOfInteriorKnots, order, numberOfParameters;
	double **m, *b, *knot;

	void v_destroy () noexcept
		override;
	autoDistance v_transform (MDSVec vec, Distance dist, Weight w)
		override;
};

autoISplineTransformator ISplineTransformator_create (integer numberOfPoints, integer numberOfInteriorKnots, integer order);

Thing_define (RatioTransformator, Transformator) {
	double ratio;

	autoDistance v_transform (MDSVec vec, Distance dist, Weight w)
		override;
};

autoRatioTransformator RatioTransformator_create (integer numberOfPoints);

Thing_define (MonotoneTransformator, Transformator) {
	int tiesHandling;

	autoDistance v_transform (MDSVec vec, Distance dist, Weight w)
		override;
};

autoMonotoneTransformator MonotoneTransformator_create (integer numberPoints);

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator, int tiesHandling);


/*************** class DissimilyList ****************************/

Collection_define (DissimilarityList, OrderedOf, Dissimilarity) {
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};


/**************** class Similarity *****************************/

Thing_define (Similarity, Proximity) {
};

autoSimilarity Similarity_create (integer numberOfPoints);


/************** KRUSKAL *********************************************/

Thing_define (Kruskal, Thing) {
	int process;
	int measurementLevel;
	int conditionality;
	autoConfiguration configuration;
	autoProximityList proximities;
	int stress_formula;
	autoMDSVec vec;
	double **dx;
	autoMinimizer minimizer;

	void v_destroy () noexcept
		override;
};

autoKruskal Kruskal_create (integer numberOfpoints, integer numberOfDimensions);

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress (Dissimilarity me, Configuration conf, Weight weight, Transformator t);

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

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d, Configuration c, Transformator t, Weight w, int stressMeasure);

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure);

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure);

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure);

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w, int tiesHandling, int stressMeasure);

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d, Configuration c, Weight w, integer numberOfInteriorKnots, integer order, int stressMeasure);

void Distance_Weight_smacofNormalize (Distance d, Weight w);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf, Weight weight, Transformator t, double tolerance, integer numberOfIterations, bool showProgress, double *stress);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf, Weight w, Transformator t, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity dis, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity dis, Configuration cstart, Weight w, int tiesHandling, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w, integer numberOfInteriorKnots, integer order, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_ratio_mds (Dissimilarity dis, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);


autoConfiguration Dissimilarity_Weight_interval_mds (Dissimilarity dis, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_monotone_mds(Dissimilarity me, Weight w, integer numberOfDimensions, int tiesHandling, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight weight, integer numberOfDimensions,
	integer numberOfInteriorKnots, integer order, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, int tiesHandling, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, integer numberOfInternalKnots, integer order, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

autoDistance Dissimilarity_Configuration_Transformator_Weight_transform (Dissimilarity d, Configuration c, Transformator t, Weight w);


/******** DISTANCE & SCALARPRODUCT *******************************/

autoScalarProduct Distance_to_ScalarProduct (Distance me, bool normalize);


/************** DISTANCE & PROXIMITY ********************************/

void Proximity_Distance_drawScatterDiagram (Proximity me, Distance thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);


/********** COVARIANCE & CONFIGURATION ***************************/

autoConfiguration SSCP_to_Configuration (SSCP me, integer numberOfDimensions);

autoConfiguration Covariance_to_Configuration (Covariance me, integer numberOfDimensions);


/************ CORRELATION & CONFIGURATION ************************/

autoConfiguration Correlation_to_Configuration (Correlation me, integer numberOfDimensions);


/************** DISTANCE & CONFIGURATION **************************/

autoDistance Configuration_to_Distance (Configuration me);
/* Calculates distances between the points */

void Distance_Configuration_drawScatterDiagram (Distance me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

autoConfiguration Distance_to_Configuration_torsca (Distance me, int numberOfDimensions);


/************** DISSIMILARITY & CONFIGURATION ************************/

autoConfiguration Dissimilarity_to_Configuration_kruskal (Dissimilarity me, integer numberOfDimensions, integer metric, int tiesHandling, int stress_formula, double tolerance, integer numberOfIterations, integer numberOfRepetitions);

autoConfiguration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him, int tiesHandling, int stress_formula, double tolerance, integer numberOfIterations, integer numberOfRepetitions);

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, int tiesHandling, int stress_formula);

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);

autoDistance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dis, Configuration conf, int tiesHandling);

autoDistanceList DissimilarityList_Configuration_monotoneRegression (DissimilarityList dissims, Configuration conf, int tiesHandling);

void Dissimilarity_Configuration_drawMonotoneRegression	(Dissimilarity me, Configuration him, Graphics g, int tiesHandling, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, bool garnish);


/************** SIMILARITY & TABLESOFREAL ********************************/

autoSimilarity DistanceList_to_Similarity_cc (DistanceList me, Weight weight);

autoSimilarity ConfigurationList_to_Similarity_cc (ConfigurationList me, Weight weight);


/************** DISSIMILARITY & DISTANCE *************************************/

autoDistance Dissimilarity_to_Distance (Dissimilarity me, int scale);
/* with optional scaling with "additive constant" */

autoDissimilarity Distance_to_Dissimilarity (Distance me);

autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, int tiesHandling);


/************** DISSIMILARITY & CONFUSION ************************************/

autoDissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel);


/************** DISSIMILARITY & MDSVEC ***************************************/

autoMDSVec Dissimilarity_to_MDSVec (Dissimilarity me);

autoMDSVecList DissimilarityList_to_MDSVecList (DissimilarityList me);


/************** DISSIMILARITY & SIMILARITY ***********************************/

autoDissimilarity Similarity_to_Dissimilarity (Similarity me, double maximumDissimilarity);


/************** DISSIMILARITY & WEIGHT ***********************************/

autoWeight Dissimilarity_to_Weight (Dissimilarity me);


/************** CONFUSION & SIMILARITY ***************************************/

autoSimilarity Confusion_to_Similarity (Confusion me, bool normalize, int symmetrizeMethod);


/************** DissimilarityList & DistanceList **********************************/

autoDissimilarityList DistanceList_to_DissimilarityList (DistanceList me);

autoDistanceList DissimilarityList_to_DistanceList (DissimilarityList me, int scale);


/************** DistanceList & Configuration ************************************/

void DistanceList_to_Configuration_ytl (DistanceList me, int numberOfDimensions, int normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2);
/*
	F.W. Young, Y. Takane & R. Lewyckyj (1978), Three notes on ALSCAL,
	Psychometrika 43, 433-435.

	 w: numberOfSources x numberOfDimensions or null
*/

autoDistanceList ConfigurationList_to_DistanceList (ConfigurationList me);


/************** MDSVec(Lists)  & Distance(List) **********************************/

autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, int tiesHandlingMethod);

autoDistanceList MDSVecList_Distance_monotoneRegression (MDSVecList me, Distance thee, int tiesHandlingMethod);


/************** ScalarProduct(List) & ...... **********************************/

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *varianceExplained, double *varianceTotal);

void ScalarProductList_Configuration_Salience_vaf (ScalarProductList me, Configuration thee, Salience him, double *vaf);

autoScalarProductList DistanceList_to_ScalarProductList (DistanceList me, bool normalize);

void ScalarProductList_to_Configuration_ytl (ScalarProductList me, int numberOfDimensions, autoConfiguration *out1, autoSalience *out2);

void ScalarProductList_Configuration_Salience_indscal (ScalarProductList sp, Configuration conf, Salience weights, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);


/************** INDSCAL & ....... ***********************************/

void DissimilarityList_indscal (DissimilarityList me, integer numberOfDimensions, int tiesHandlingMethod, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_to_Configuration_indscal (DistanceList me, integer numberOfDimensions, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_indscal (DissimilarityList me, Configuration conf, int tiesHandlingMethod, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_Configuration_indscal (DistanceList dists, Configuration conf, bool normalizeScalarProducts, double tolerance, integer numberOfIterations,
	bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_Salience_indscal (DissimilarityList dissims, Configuration conf, Salience w, int tiesHandlingMethod,
	bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

autoDistanceList MDSVecList_Configuration_Salience_monotoneRegression (MDSVecList vecs, Configuration conf, Salience weights, int tiesHandlingMethod);

void DistanceList_Configuration_Salience_indscal (DistanceList dists, Configuration conf, Salience weights, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

void DistanceList_Configuration_Salience_vaf (DistanceList me, Configuration thee, Salience him, bool normalizeScalarProducts, double *vaf);

void DissimilarityList_Configuration_Salience_vaf (DissimilarityList me, Configuration thee,
	Salience him, int tiesHandlingMethod, bool normalizeScalarProducts, double *vaf);

void DistanceList_Configuration_vaf (DistanceList me, Configuration thee, bool normalizeScalarProducts, double *vaf);

void DissimilarityList_Configuration_vaf (DissimilarityList me, Configuration thee, int tiesHandlingMethod, bool normalizeScalarProducts, double *vaf);

autoSalience ScalarProductList_Configuration_to_Salience (ScalarProductList me, Configuration him);

autoSalience DistanceList_Configuration_to_Salience (DistanceList me, Configuration him, bool normalizeScalarProducts);

autoSalience DissimilarityList_Configuration_to_Salience (DissimilarityList me, Configuration him, int tiesHandling, bool normalizeScalarProducts);


/********* Conversions from & to TableOfReal *****************************/

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

void drawSplines (Graphics g, double low, double high, double ymin, double ymax, int type, integer order, char32 const *interiorKnots, bool garnish);

void drawMDSClassRelations (Graphics g);

#endif /* _MDS_h_ */
