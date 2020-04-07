#ifndef _MDS_h_
#define _MDS_h_
/* MDS.h
 *
 * Multi Dimensional Scaling
 *
 * Copyright (C) 1993-2020 David Weenink, 2015,2017,2018 Paul Boersma
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

#include "Covariance.h"
#include "Correlation.h"
#include "Graphics.h"
#include "Minimizers.h"
#include "Confusion.h"
#include "ContingencyTable.h"
#include "MDSVec.h"
#include "TableOfReal_extensions.h"
#include "Proximity.h"
#include "Distance.h"
#include "Configuration.h"
#include "MDS_enums.h"

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

void Salience_draw (Salience me, Graphics g, integer xdimension, integer ydimension, bool garnish);

autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, integer numberOfDimensions, integer scaling);

#pragma mark - class ConfusionList

Collection_define (ConfusionList, OrderedOf, Confusion) {
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

autoConfusion ConfusionList_sum (ConfusionList me);

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



#pragma mark - class Transformator

Thing_define (Transformator, Thing) {
	integer numberOfPoints;
	integer normalization;

	virtual autoDistance v_transform (MDSVec vec, Distance dist, Weight w);
};

void Transformator_init (Transformator me, integer numberOfPoints);

autoTransformator Transformator_create (integer numberOfPoints);

void Transformator_setNormalization (Transformator me, integer normalization);

autoDistance Transformator_transform (Transformator me, MDSVec vec, Distance dist, Weight w);

Thing_define (ISplineTransformator, Transformator) {
	integer numberOfInteriorKnots, order, numberOfParameters;
	autoMAT m;
	autoVEC b, knot;

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
	kMDS_TiesHandling tiesHandling;

	autoDistance v_transform (MDSVec vec, Distance dist, Weight w)
		override;
};

autoMonotoneTransformator MonotoneTransformator_create (integer numberPoints);

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator, kMDS_TiesHandling tiesHandling);

/************** Kruskal *********************************************/

Thing_define (Kruskal, Thing) {
	autoConfiguration configuration;
	autoProximityList proximities;
	kMDS_TiesHandling tiesHandling;
	kMDS_KruskalStress stress_formula;
	autoMDSVec vec;
	autoMAT dx;
	autoMinimizer minimizer;
};

autoKruskal Kruskal_create (integer numberOfPoints, integer numberOfDimensions);

autoConfiguration Dissimilarity_to_Configuration_kruskal (Dissimilarity me,
	integer numberOfDimensions, integer metric, kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions);

autoConfiguration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him,
	kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions);

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula);

/******** end Kruskal **************************************************/

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress (Dissimilarity me, Configuration conf, Weight weight, Transformator t);

double Distance_Weight_stress (Distance fit, Distance c, Weight w, kMDS_stressMeasure type);
/*
	Calculates stress.
	type is one of (MDS_NORMALIZED_STRESS, MDS_STRESS_1,
		MDS_STRESS_2, MDS_RAW_STRESS)
*/

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w);
/*
	Congruence coefficient B&G page 350.
*/

void Distance_Weight_rawStressComponents (Distance fit, Distance conf, Weight weight, double *out_eta_fit, double *out_eta_conf, double *out_rho);
/*
	Computes
		eta_fit = sum (i<j,i=1..n; w[i][j] * dfit[i][j]^2)
		eta_conf = sum (i<j,i=1..n; w[i][j] * conf[i][j]^2)
		rho   = sum (i<j,i=1..n; w[i][j] * dfit[i][j] * conf[i][j];
	where,
		dfit[i][j] = transformation (dissimilarity[i][j])
		conf[i][j] = distance between x[i] and x[j] (in the configuration)
*/

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d, Configuration c, Transformator t, Weight w, kMDS_stressMeasure stressMeasure);

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w,
	kMDS_stressMeasure stressMeasure);

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w,
	kMDS_stressMeasure stressMeasure);

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w,
	kMDS_stressMeasure stressMeasure);

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w,
	kMDS_TiesHandling tiesHandling, kMDS_stressMeasure stressMeasure);

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d, Configuration c, Weight w,
	integer numberOfInteriorKnots, integer order, kMDS_stressMeasure stressMeasure);

void Distance_Weight_smacofNormalize (Distance d, Weight w);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf, Weight weight, Transformator t,
	double tolerance, integer numberOfIterations, bool showProgress, double *out_stress);

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf, Weight w, Transformator t,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity dis, Configuration cstart, Weight w,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity dis, Configuration cstart, Weight w,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity dis, Configuration cstart, Weight w,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity dis, Configuration cstart, Weight w, kMDS_TiesHandling tiesHandling,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w,
	integer numberOfInteriorKnots, integer order,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress
);

autoConfiguration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_ratio_mds (Dissimilarity dis, Weight w,
	integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_interval_mds (Dissimilarity dis, Weight w,
	integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress);

autoConfiguration Dissimilarity_Weight_monotone_mds(Dissimilarity me, Weight w,
	integer numberOfDimensions, kMDS_TiesHandling tiesHandling,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress
);

autoConfiguration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight weight, integer numberOfDimensions,
	integer numberOfInteriorKnots, integer order,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress
);

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	kMDS_TiesHandling tiesHandling, double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	integer numberOfInternalKnots, integer order, double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);

autoDistance Dissimilarity_Configuration_Transformator_Weight_transform (Dissimilarity d, Configuration c, Transformator t, Weight w);


/******** DISTANCE & SCALARPRODUCT *******************************/

autoScalarProduct Distance_to_ScalarProduct (Distance me, bool normalize);


/************** DISTANCE & PROXIMITY ********************************/

void Proximity_Distance_drawScatterDiagram (Proximity me, Distance thee, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish
);


/********** COVARIANCE & CONFIGURATION ***************************/

autoConfiguration SSCP_to_Configuration (SSCP me, integer numberOfDimensions);

autoConfiguration Covariance_to_Configuration (Covariance me, integer numberOfDimensions);


/************ CORRELATION & CONFIGURATION ************************/

autoConfiguration Correlation_to_Configuration (Correlation me, integer numberOfDimensions);


/************** DISTANCE & CONFIGURATION **************************/

autoDistance Configuration_to_Distance (Configuration me);
/* Calculates distances between the points */

void Distance_Configuration_drawScatterDiagram (Distance me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish);

autoConfiguration Distance_to_Configuration_torsca (Distance me, integer numberOfDimensions);


/************** DISSIMILARITY & CONFIGURATION ************************/

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, Configuration him, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish);

autoDistance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dis, Configuration conf, kMDS_TiesHandling tiesHandling);

autoDistanceList DissimilarityList_Configuration_monotoneRegression (DissimilarityList dissims, Configuration conf, kMDS_TiesHandling tiesHandling);

void Dissimilarity_Configuration_drawMonotoneRegression	(Dissimilarity me, Configuration him, Graphics g, kMDS_TiesHandling tiesHandling,	double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish);


/************** SIMILARITY & TABLESOFREAL ********************************/

autoSimilarity DistanceList_to_Similarity_cc (DistanceList me, Weight weight);

autoSimilarity ConfigurationList_to_Similarity_cc (ConfigurationList me, Weight weight);


/************** DISSIMILARITY & DISTANCE *************************************/

autoDistance Dissimilarity_to_Distance (Dissimilarity me, integer scale);
/* with optional scaling with "additive constant" */

autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, kMDS_TiesHandling tiesHandling);


/************** DISSIMILARITY & CONFUSION ************************************/

autoDissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel);


/************** DISSIMILARITY & SIMILARITY ***********************************/

autoDissimilarity Similarity_to_Dissimilarity (Similarity me, double maximumDissimilarity);


/************** DISSIMILARITY & WEIGHT ***********************************/

autoWeight Dissimilarity_to_Weight (Dissimilarity me);


/************** CONFUSION & SIMILARITY ***************************************/

autoSimilarity Confusion_to_Similarity (Confusion me, bool normalize, integer symmetrizeMethod);


/************** DissimilarityList & DistanceList **********************************/

autoDissimilarityList DistanceList_to_DissimilarityList (DistanceList me);



/************** DistanceList & Configuration ************************************/

void DistanceList_to_Configuration_ytl (DistanceList me,
	integer numberOfDimensions, integer normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2);
/*
	F.W. Young, Y. Takane & R. Lewyckyj (1978), Three notes on ALSCAL,
	Psychometrika 43, 433-435.

	 w: numberOfSources x numberOfDimensions or null
*/

autoDistanceList ConfigurationList_to_DistanceList (ConfigurationList me);


/************** MDSVec(Lists)  & Distance(List) **********************************/

autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, kMDS_TiesHandling tiesHandling);

autoDistanceList MDSVecList_Distance_monotoneRegression (MDSVecList me, Distance thee, kMDS_TiesHandling tiesHandling);


/************** ScalarProduct(List) & ...... **********************************/

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *varianceExplained, double *varianceTotal);

void ScalarProductList_Configuration_Salience_vaf (ScalarProductList me, Configuration thee, Salience him, double *out_varianceAccountedFor);

autoScalarProductList DistanceList_to_ScalarProductList (DistanceList me, bool normalize);

void ScalarProductList_to_Configuration_ytl (ScalarProductList me, integer numberOfDimensions, autoConfiguration *out1, autoSalience *out2);

void ScalarProductList_Configuration_Salience_indscal (ScalarProductList sp, Configuration conf, Salience weights,
	double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *out_varianceAccountedFor);


/************** INDSCAL & ....... ***********************************/

void DissimilarityList_indscal (DissimilarityList me, integer numberOfDimensions, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_to_Configuration_indscal (DistanceList me, integer numberOfDimensions, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_indscal (DissimilarityList me, Configuration conf, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DistanceList_Configuration_indscal (DistanceList dists, Configuration conf, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2);

void DissimilarityList_Configuration_Salience_indscal (DissimilarityList dissims, Configuration conf, Salience w,
	kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *out_varianceAccountedFor);

autoDistanceList MDSVecList_Configuration_Salience_monotoneRegression (MDSVecList vecs, Configuration conf, Salience weights, kMDS_TiesHandling tiesHandling);

void DistanceList_Configuration_Salience_indscal (DistanceList dists, Configuration conf, Salience weights, bool normalizeScalarProducts,
	double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *out_varianceAccountedFor);

void DistanceList_Configuration_Salience_vaf (DistanceList me, Configuration thee, Salience him, bool normalizeScalarProducts, double *out_varianceAccountedFor);

void DissimilarityList_Configuration_Salience_vaf (DissimilarityList me, Configuration thee,
	Salience him, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double *out_varianceAccountedFor);

void DistanceList_Configuration_vaf (DistanceList me, Configuration thee, bool normalizeScalarProducts, double *out_varianceAccountedFor);

void DissimilarityList_Configuration_vaf (DissimilarityList me, Configuration thee,
	kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double *out_varianceAccountedFor);

autoSalience ScalarProductList_Configuration_to_Salience (ScalarProductList me, Configuration him);

autoSalience DistanceList_Configuration_to_Salience (DistanceList me, Configuration him, bool normalizeScalarProducts);

autoSalience DissimilarityList_Configuration_to_Salience (DissimilarityList me, Configuration him,
	kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts);


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

void drawSplines (Graphics g, double low, double high, double ymin, double ymax, kMDS_splineType splineType, integer order, char32 const *interiorKnots, bool garnish);

void drawMDSClassRelations (Graphics g);

#endif /* _MDS_h_ */
