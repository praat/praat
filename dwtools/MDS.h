#ifndef _MDS_h_
#define _MDS_h_
/* MDS.h
 *
 * Multi Dimensional Scaling
 *
 * Copyright (C) 1993-2011,2015 David Weenink, 2015 Paul Boersma
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
	long nProximities, nPoints;
	double *proximity;
	long *iPoint, *jPoint;

	void v_destroy ()
		override;
};

autoMDSVec MDSVec_create (long nObjects);

/************** class MDSVecs *********************************/

Collection_declare (OrderedOfMDSVec, OrderedOf, MDSVec);

Thing_define (MDSVecList, OrderedOfMDSVec) {
	structMDSVecList () {
		our classInfo = classMDSVecList;
	}
};

inline static autoMDSVecList MDSVecList_create () {
	return Thing_new (MDSVecList);
}

autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, long numberOfDimensions, int scaling);

/********************* class Proximities *******************************/

Collection_declare (OrderedOfProximity, OrderedOf, Proximity);

Thing_define (ProximityList, OrderedOfProximity) {
	structProximityList () {
		our classInfo = classProximityList;
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

inline static autoProximityList ProximityList_create () {
	return Thing_new (ProximityList);
}

/****************** class Confusions **********************************/

Collection_declare (OrderedOfConfusion, OrderedOf, Confusion);

Thing_define (ConfusionList, OrderedOfConfusion) {
	structConfusionList () {
		our classInfo = classConfusionList;
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

autoConfusion ConfusionList_sum (ConfusionList me);

/************* class Distances **************************************/

Collection_declare (OrderedOfDistance, OrderedOf, Distance);

Thing_define (DistanceList, OrderedOfDistance) {
	structDistanceList () {
		our classInfo = classDistanceList;
	}
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

inline static autoDistanceList DistanceList_create () {
	return Thing_new (DistanceList);
}

//Collection_implement (Distances, Proximities, 0);

/**************** class ScalarProduct **************************************/

Thing_define (ScalarProduct, TableOfReal) {
};

autoScalarProduct ScalarProduct_create (long numberOfPoints);

/************** class ScalarProducts ********************************/

Collection_declare (OrderedOfScalarProduct, OrderedOf, ScalarProduct);

Thing_define (ScalarProductList, OrderedOfScalarProduct) {
	structScalarProductList () {
		our classInfo = classScalarProductList;
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

inline static autoScalarProductList ScalarProductList_create () {
	return Thing_new (ScalarProductList);
}

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
	int tiesProcessing;

	autoDistance v_transform (MDSVec vec, Distance dist, Weight w)
		override;
};

autoMonotoneTransformator MonotoneTransformator_create (long numberPoints);

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator,
	int tiesProcessing);


/*************** class Dissimilarities ****************************/

Collection_declare (OrderedOfDissimilarity, OrderedOf, Dissimilarity);

Thing_define (DissimilarityList, OrderedOfDissimilarity) {
	structDissimilarityList () {
		our classInfo = classDissimilarityList;
	}
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};
inline static autoDissimilarityList DissimilarityList_create () {
	return Thing_new (DissimilarityList);
}


/**************** class Similarity *****************************/

Thing_define (Similarity, Proximity) {
};

autoSimilarity Similarity_create (long numberOfPoints);


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

	void v_destroy ()
		override;
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

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf, Weight w, Transformator t, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress);

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

autoScalarProduct Distance_to_ScalarProduct (Distance me, bool normalize);


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

autoDistanceList DissimilarityList_Configuration_monotoneRegression (DissimilarityList dissims, Configuration conf, int processTies);

void Dissimilarity_Configuration_drawMonotoneRegression	(Dissimilarity me, Configuration him, Graphics g, int processTies, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish);


/************** SIMILARITY & TABLESOFREAL ********************************/

autoSimilarity DistanceList_to_Similarity_cc (DistanceList me, Weight weight);

autoSimilarity ConfigurationList_to_Similarity_cc (ConfigurationList me, Weight weight);


/************** DISSIMILARITY & DISTANCE *************************************/

autoDistance Dissimilarity_to_Distance (Dissimilarity me, int scale);
/* with optional scaling with "additive constant" */

autoDissimilarity Distance_to_Dissimilarity (Distance me);

autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, int tiesProcessingMethod);


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


/************** DISSIMILARITIES & DISTANCES **********************************/

autoDissimilarityList DistanceList_to_DissimilarityList (DistanceList me);

autoDistanceList DissimilarityList_to_DistanceList (DissimilarityList me, int scale);


/************** DISTANCES & CONFIGURATION ************************************/

void DistanceList_to_Configuration_ytl (DistanceList me, int numberOfDimensions, int normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2);
/*
	F.W. Young, Y. Takane & R. Lewyckyj (1978), Three notes on ALSCAL,
	Psychometrika 43, 433-435.

	 w: numberOfSources x numberOfDimensions or null
*/

autoDistanceList ConfigurationList_to_DistanceList (ConfigurationList me);


/************** MDSVEC(S) & DISTANCE(S) **********************************/

autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, int tiesProcessingMethod);

autoDistanceList MDSVecList_Distance_monotoneRegression (MDSVecList me, Distance thee, int tiesProcessingMethod);


/************** SCALARPRODUCT(S) & ...... **********************************/

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *varianceExplained, double *varianceTotal);

void ScalarProductList_Configuration_Salience_vaf (ScalarProductList me, Configuration thee, Salience him, double *vaf);

autoScalarProductList DistanceList_to_ScalarProductList (DistanceList me, bool normalize);

void ScalarProductList_to_Configuration_ytl (ScalarProductList me, int numberOfDimensions, autoConfiguration *out1, autoSalience *out2);

void ScalarProductList_Configuration_Salience_indscal (ScalarProductList sp, Configuration conf, Salience weights, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);


/************** INDSCAL & ....... ***********************************/

void DissimilarityList_indscal (DissimilarityList me, long numberOfDimensions, int tiesProcessingMethod, bool normalizeScalarProducts, double tolerance,
	long numberOfIterations, long numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_indscal (DistanceList me, long numberOfDimensions, bool normalizeScalarProducts, double tolerance, long numberOfIterations,
	long numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_indscal (DissimilarityList me, Configuration conf, int tiesProcessingMethod, bool normalizeScalarProducts,
	double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_Configuration_indscal (DistanceList dists, Configuration conf, bool normalizeScalarProducts, double tolerance, long numberOfIterations,
	bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_Salience_indscal (DissimilarityList dissims, Configuration conf, Salience w, int tiesProcessingMethod,
	bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

autoDistanceList MDSVecList_Configuration_Salience_monotoneRegression (MDSVecList vecs, Configuration conf, Salience weights, int tiesProcessingMethod);

void DistanceList_Configuration_Salience_indscal (DistanceList dists, Configuration conf, Salience weights, bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf);

void DistanceList_Configuration_Salience_vaf (DistanceList me, Configuration thee, Salience him, bool normalizeScalarProducts, double *vaf);

void DissimilarityList_Configuration_Salience_vaf (DissimilarityList me, Configuration thee,
	Salience him, int tiesProcessingMethod, bool normalizeScalarProducts, double *vaf);

void DistanceList_Configuration_vaf (DistanceList me, Configuration thee, bool normalizeScalarProducts, double *vaf);

void DissimilarityList_Configuration_vaf (DissimilarityList me, Configuration thee, int tiesProcessingMethod, bool normalizeScalarProducts, double *vaf);

autoSalience ScalarProductList_Configuration_to_Salience (ScalarProductList me, Configuration him);

autoSalience DistanceList_Configuration_to_Salience (DistanceList me, Configuration him, bool normalizeScalarProducts);

autoSalience DissimilarityList_Configuration_to_Salience (DissimilarityList me, Configuration him, int processTies, bool normalizeScalarProducts);


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

void drawSplines (Graphics g, double low, double high, double ymin, double ymax, int type, long order, char32 const *interiorKnots, int garnish);

void drawMDSClassRelations (Graphics g);

#endif /* _MDS_h_ */
