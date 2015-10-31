/* praat_contrib_Ola_KNN.cpp
 *
 * Copyright (C) 2007-2009 Ola Söder, 2010-2011,2015 Paul Boersma
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
 * os 2007/05/29 Initial release?
 * os 2009/01/23 Bugfix: Removed MUX:ing (KNN_learn) incompatible with the scripting engine. Thanks to Paul Boersma for spotting this problem.
 * pb 2010/12/28 in messages: typos, English, interpunction
 * pb 2011/07/12 C++ and removed several errors
 * pb 2011/09/18 made a start with handling dataChanged at all
 */

#include "KNN.h"
#include "KNN_threads.h"
#include "KNN_prune.h"
#include "Pattern_to_Categories_cluster.h"
#include "FeatureWeights.h"
#include "praat.h"

static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *MODIFY_BUTTON  = U"Modify -";
static const char32 *EXTRACT_BUTTON = U"Extract -";

/////////////////////////////////////////////////////////////////////////////////////////
// KNN creations                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_create, U"Create kNN Classifier", U"kNN classifiers 1. What is a kNN classifier?") {
	WORD (U"Name", U"Classifier")
	OK2
DO
	autoKNN knn = KNN_create ();
	praat_new (knn.move(), GET_STRING (U"Name"));
END2 }

FORM (KNN_Pattern_Categories_to_KNN, U"Create kNN classifier", U"kNN classifiers 1. What is a kNN classifier?") {
	WORD (U"Name", U"Classifier")
	RADIO (U"Ordering", 1)
	RADIOBUTTON (U"Random")
	RADIOBUTTON (U"Sequential")
	OK2
DO
	iam_ONLY (Pattern);
	thouart_ONLY (Categories);
	int ordering = GET_INTEGER (U"Ordering");
	autoKNN knn = KNN_create ();
	switch (ordering) {
		case 1:
			ordering = kOla_SHUFFLE;
			break;
		case 2:
			ordering = kOla_SEQUENTIAL;
	}
	int result = KNN_learn (knn.peek(), me, thee, kOla_REPLACE, ordering);
	switch (result) {
		case kOla_PATTERN_CATEGORIES_MISMATCH:
			Melder_throw (U"The number of Categories should be equal to the number of rows in Pattern.");
		case kOla_DIMENSIONALITY_MISMATCH:
			Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
		default:
			praat_new (knn.move(), GET_STRING(U"Name"));
	}
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// KNN extractions, queries and modifications                                         //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT2 (KNN_getNumberOfInstances) {
    iam_ONLY (KNN);
    Melder_information (my nInstances, U" units");
END2 }

FORM (KNN_getOptimumModel, U"kNN model selection", U"kNN classifiers 1.1.2. Model selection") {
	RADIO (U"Evaluation method", 1)
	RADIOBUTTON (U"Leave one out")
	RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (U"k max", U"50")
	INTEGER (U"Number of seeds", U"10")
	POSITIVE (U"Learning rate", U"0.2")
	OK2
DO
	iam_ONLY (KNN);
	long k = GET_INTEGER (U"k max");
	double lrate = GET_REAL (U"Learning rate");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k max such that 0 < k max < ", my nInstances + 1, U".");
	long nseeds = GET_INTEGER (U"Number of seeds");
	if (nseeds < 1)
		Melder_throw (U"The number of seeds should exceed 1.");
	int mode = GET_INTEGER (U"Evaluation method");
	switch (mode) {
		case 2:
			mode = kOla_TEN_FOLD_CROSS_VALIDATION;
			break;
		case 1:
			mode = kOla_LEAVE_ONE_OUT;
			break;
	}
	autoFeatureWeights fws = FeatureWeights_create ((my input) -> nx);
	int dist;
	KNN_modelSearch (me, fws.peek(), &k, &dist, mode, lrate, nseeds);
	switch (dist) {
		case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
			Melder_information (U"Vote weighting: Inversed squared distance\n", U"k: ", k);
			break;
		case kOla_DISTANCE_WEIGHTED_VOTING:
			Melder_information (U"Vote weighting: Inversed distance\n", U"k: ", k);
			break;
		case kOla_FLAT_VOTING:
			Melder_information (U"Vote weighting: Flat\n", U"k: ", k);
			break;
	}
END2 }

FORM (KNN_evaluate, U"Evaluation", U"KNN: Get accuracy estimate...") {
	RADIO (U"Evaluation method", 1)
	RADIOBUTTON (U"Leave one out")
	RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances < 1)
		Melder_throw (U"Instance base is empty.");
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	int mode = GET_INTEGER (U"Evaluation method");
	switch (mode) {
		case 2:
			mode = kOla_TEN_FOLD_CROSS_VALIDATION;
			break;
		case 1: 
			mode = kOla_LEAVE_ONE_OUT;
			break;
	}
	autoFeatureWeights fws = FeatureWeights_create (my input -> nx);
	double result = KNN_evaluate (me, fws.peek(), k, vt, mode);
	if (lround (result) == kOla_FWEIGHTS_MISMATCH)
		Melder_throw (U"The number of feature weights should be equal to the dimensionality of the Pattern.");
	Melder_information (100 * result, U" percent of the instances correctly classified.");   // BUG: use Melder_percent
END2 }

FORM (KNN_evaluateWithFeatureWeights, U"Evaluation", U"KNN & FeatureWeights: Get accuracy estimate...") {
	RADIO (U"Evaluation method", 1)
	RADIOBUTTON (U"Leave one out")
	RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances < 1)
		Melder_throw (U"Instance base is empty");
	thouart_ONLY (FeatureWeights);
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}   
	int mode = GET_INTEGER (U"Evaluation method");
	switch (mode) {
		case 2:
			mode = kOla_TEN_FOLD_CROSS_VALIDATION;
			break;
		case 1:
			mode = kOla_LEAVE_ONE_OUT;
			break;
	}
	double result = KNN_evaluate (me, thee, k, vt, mode);
	if (lround (result) == kOla_FWEIGHTS_MISMATCH)
		Melder_throw (U"The number of feature weights should be equal to the dimensionality of the Pattern.");
	Melder_information (100 * result, U" percent of the instances correctly classified.");
END2 }

DIRECT2 (KNN_extractInputPatterns) {
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	autoPattern input = Data_copy (my input);
	praat_new (input.move(), U"Input Patterns");
END2 }

DIRECT2 (KNN_extractOutputCategories) {
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	autoCategories output = Data_copy (my output);
	praat_new (output.move(), U"Output Categories");
END2 }

FORM (KNN_reset, U"Reset", U"KNN: Reset...") {
    LABEL (U"", U"Warning: this command destroys all previous learning.")
    OK2
DO
	iam_ONLY (KNN);
	forget (my input);
	forget (my output);
	my nInstances = 0;
	praat_dataChanged (me);   // BUG: this should be inserted much more often
END2 }

DIRECT2 (KNN_shuffle) {
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	KNN_shuffleInstances (me);
END2 }

FORM (KNN_prune, U"Pruning", U"KNN: Prune...") {
	POSITIVE (U"Noise pruning degree", U"1")
	POSITIVE (U"Redundancy pruning degree", U"1")
	INTEGER (U"k neighbours", U"1")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	long oldn = my nInstances;   // save before it changes!
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	double n = GET_REAL (U"Noise pruning degree");
	double r = GET_REAL (U"Redundancy pruning degree");
	if (n <= 0 || n > 1 || r <= 0 || r > 1)
		Melder_throw (U"Please select a pruning degree d such that 0 < d <= 1.");
	long npruned = KNN_prune_prune (me, n, r, k);
	Melder_information (npruned, U" instances discarded. \n", U"Size of new instance base: ", oldn - npruned);
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Learning                                                                            //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_learn, U"Learning", U"kNN classifiers 1. What is a kNN classifier?") {
	RADIO (U"Learning method", 1)
	RADIOBUTTON (U"Append new information")
	RADIOBUTTON (U"Replace current intancebase")
	RADIO (U"Ordering", 1)
	RADIOBUTTON (U"Random")
	RADIOBUTTON (U"Sequential")
	OK2
DO
	iam_ONLY (KNN);
	thouart_ONLY (Pattern);
	heis_ONLY (Categories);
	int ordering = GET_INTEGER (U"Ordering");
	switch (ordering) {
		case 1:
			ordering = kOla_SHUFFLE;
			break;
		case 2:
			ordering = kOla_SEQUENTIAL;
	}
	int method = GET_INTEGER (U"Learning method");
	int result = kOla_ERROR;
	switch (method) {
		case 1:
			result = KNN_learn (me, thee, him, my nInstances == 0 ? kOla_REPLACE : kOla_APPEND, ordering);
			break;
		case 2:
			result = KNN_learn (me, thee, him, kOla_REPLACE, ordering);
			break;
	}
	switch (result) {
		case kOla_PATTERN_CATEGORIES_MISMATCH:  
			Melder_throw (U"The number of Categories should be equal to the number of rows in Pattern.");
		case kOla_DIMENSIONALITY_MISMATCH:
			Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
	}
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Evaluation                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_evaluateWithTestSet, U"Evaluation", U"KNN & Pattern & Categories: Evaluate...") {
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty");
	thouart_ONLY (Pattern);
	heis_ONLY (Categories);
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	if (thy ny != his size)
		Melder_throw (U"The number of Categories should be equal to the number of rows in Pattern.");
	if (thy nx != (my input)->nx)
		Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
	autoFeatureWeights fws = FeatureWeights_create (thy nx);
	double result = KNN_evaluateWithTestSet (me, thee, him, fws.peek(), k, vt);
	Melder_information (100 * result, U" percent of the instances correctly classified.");
END2 }

FORM (KNN_evaluateWithTestSetAndFeatureWeights, U"Evaluation", U"KNN & Pattern & Categories & FeatureWeights: Evaluate...") {
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty");
	Pattern p = (Pattern) ONLY (classPattern);
	Categories c = (Categories) ONLY (classCategories);
	FeatureWeights fws = (FeatureWeights) ONLY (classFeatureWeights);
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	if (p -> ny != c -> size)
		Melder_throw (U"The number of Categories should be equal to the number of rows in Pattern.");
	if (p -> nx != my input -> nx)
		Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
	if (p->nx != fws -> fweights -> numberOfColumns)
		Melder_throw (U"The number of feature weights should be equal to the dimensionality of the Pattern.");
	double result = KNN_evaluateWithTestSet (me, p, c, fws, k, vt);
	Melder_information (100 * result, U" percent of the instances correctly classified.");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Classification                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_toCategories, U"Classification", U"KNN & Pattern: To Categories...") {
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	thouart_ONLY (Pattern);
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	if (thy nx != my input -> nx)
		Melder_throw (U"The dimensionality of Pattern should match that of the instance base.");
	autoFeatureWeights fws = FeatureWeights_create (thy nx);
	autoCategories result = KNN_classifyToCategories (me, thee, fws.peek(), k, vt);
	praat_new (result.move(), U"Output");
END2 }

FORM (KNN_toTableOfReal, U"Classification", U"KNN & Pattern: To TabelOfReal...") {
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	thouart_ONLY (Pattern);
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	autoFeatureWeights fws = FeatureWeights_create(thy nx);
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	if (thy nx != my input -> nx)
		Melder_throw (U"The dimensionality of Pattern should match that of the instance base.");
	autoTableOfReal result = KNN_classifyToTableOfReal (me, thee, fws.peek(), k, vt);
	praat_new (result.move(), U"Output");
END2 }

FORM (KNN_toCategoriesWithFeatureWeights, U"Classification", U"KNN & Pattern & FeatureWeights: To Categories...") {
	INTEGER (U"k neighbours", U"KNN & Pattern & FeatureWeights: To Categories...")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")  
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	thouart_ONLY (Pattern);
	heis_ONLY (FeatureWeights);
	int vt = GET_INTEGER (U"Vote weighting");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;  
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	if (thy nx != (my input)->nx)
		Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
	if (thy nx != his fweights -> numberOfColumns)
		Melder_throw (U"The number of feature weights should be equal to the dimensionality of the Pattern.");
	autoCategories result = KNN_classifyToCategories (me, thee, him, k, vt);
	praat_new (result.move(), U"Output");
END2 }

FORM (KNN_toTableOfRealWithFeatureWeights, U"Classification", U"KNN & Pattern & FeatureWeights: To TableOfReal...") {
	INTEGER (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 1)
	RADIOBUTTON (U"Inversed squared distance")
	RADIOBUTTON (U"Inversed distance")
	RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty.");
	thouart_ONLY (Pattern);
	heis_ONLY (FeatureWeights);
	long k = GET_INTEGER (U"k neighbours");
	int vt = GET_INTEGER (U"Vote weighting");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U"\n");
	if (thy nx != his fweights -> numberOfColumns)
		Melder_throw (U"The number of features and the number of feature weights should be equal.");
	switch (vt) {
		case 1:
			vt = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			vt = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			vt = kOla_FLAT_VOTING;
			break;
	}
	autoTableOfReal result = KNN_classifyToTableOfReal (me, thee, him, k, vt);
	praat_new (result.move(), U"Output");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Clustering                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (Pattern_to_Categories_cluster, U"k-means clustering", U"Pattern: To Categories...") {
	INTEGER (U"k clusters", U"1")
	POSITIVE (U"Cluster size ratio constraint", U"0.0000001");
	INTEGER (U"Maximum number of reseeds", U"1000")
	OK2
DO
	iam_ONLY (Pattern);
	if (my nx > 0 && my ny > 0) {
		long k = GET_INTEGER (U"k clusters");
		if (k < 1 || k > my ny)
			Melder_throw (U"Please select a value of k such that 0 < k <= ", my ny, U".");
		long rs =  GET_INTEGER (U"Maximum number of reseeds");
		if (rs < 0)
			Melder_throw (U"The maximum number of reseeds should not be negative.");
		double rc =  GET_REAL (U"Cluster size ratio constraint");
		if (rc > 1 || rc <= 0)
			Melder_throw (U"Please select a value of the cluster size ratio constraint c such that 0 < c <= 1.");
		autoFeatureWeights fws = FeatureWeights_create (my nx);
		autoCategories result = Pattern_to_Categories_cluster (me, fws.peek(), k, rc, rs);
		praat_new (result.move(), U"Output");
	} else {
		Melder_throw (U"Pattern is empty.");
	}
END2 }

FORM (Pattern_to_Categories_clusterWithFeatureWeights, U"k-means clustering", U"Pattern & FeatureWeights: To Categories...") {
	INTEGER (U"k clusters", U"1")
	POSITIVE (U"Cluster size ratio constraint", U"0.0000001");
	INTEGER (U"Maximum number of reseeds", U"1000")
	OK2
DO
	iam_ONLY (Pattern);
	if (my nx > 0 && my ny > 0) {
		thouart_ONLY (FeatureWeights); 
		if (my nx != thy fweights -> numberOfColumns)
			Melder_throw (U"The number of features and the number of feature weights should be equal.");
		long k = GET_INTEGER(U"k clusters");
		if (k < 1 || k > my ny)
			Melder_throw (U"Please select a value of k such that 0 < k <= ", my ny, U".");
		long rs =  GET_INTEGER(U"Maximum number of reseeds");
		if (rs < 0)
			Melder_throw (U"The maximum number of reseeds should not be negative.");
		double rc =  GET_REAL(U"Cluster size ratio constraint");
		if (rc > 1 || rc <= 0)
			Melder_throw (U"Please select a value of the cluster size ratio constraint c such that 0 < c <= 1.");
		autoCategories result = Pattern_to_Categories_cluster (me, thee, k, rc, rs);
		praat_new (result.move(), U"Output");
	} else {
		Melder_throw (U"Pattern is empty.");
	}
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Dissimilarity computations                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT2 (KNN_patternToDissimilarity) {
	iam_ONLY (Pattern);
	autoFeatureWeights fws = FeatureWeights_create (my nx);
	autoDissimilarity result = KNN_patternToDissimilarity (me, fws.peek());
	praat_new (result.move(), U"Output");
END2 }

DIRECT2 (KNN_patternToDissimilarityWithFeatureWeights) {
	iam_ONLY (Pattern);
	thouart_ONLY (FeatureWeights);  
	if (my nx != thy fweights -> numberOfColumns)
		Melder_throw (U"The number of features and the number of feature weights should be equal.");
	autoDissimilarity result = KNN_patternToDissimilarity (me, thee);
	praat_new (result.move(), U"Output");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Computation of permutation                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_SA_computePermutation, U"To Permutation...", U"Pattern & Categories: To FeatureWeights...") {
	NATURAL (U"Tries per step", U"200")
	NATURAL (U"Iterations", U"10")
	POSITIVE (U"Step size", U"10")
	POSITIVE (U"Boltzmann constant", U"1.0")
	POSITIVE (U"Initial temperature", U"0.002")
	POSITIVE (U"Damping factor", U"1.005")
	POSITIVE (U"Final temperature", U"0.000002")
	OK2
DO
	iam_ONLY (KNN);
	long tries = GET_INTEGER (U"Tries per step");
	long iterations = GET_INTEGER (U"Iterations");
	double step_size = GET_REAL (U"Step size");
	double bolzmann_c = GET_REAL (U"Boltzmann constant");
	double temp_start = GET_REAL (U"Initial temperature");
	double temp_damp = GET_REAL (U"Damping factor");
	double temp_stop = GET_REAL (U"Final temperature");
	autoPermutation result = KNN_SA_ToPermutation (me, tries, iterations, step_size, bolzmann_c, temp_start, temp_damp, temp_stop);
	praat_new (result.move(), U"Output");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Computation of feature weights                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (FeatureWeights_computeRELIEF, U"Feature weights", U"Pattern & Categories: To FeatureWeights...") {
	INTEGER (U"Number of neighbours", U"1")
	OK2
DO
	iam_ONLY (Pattern);
	thouart_ONLY (Categories);
	if (my ny < 2)
		Melder_throw (U"The Pattern object should contain at least 2 rows.");
	if (my ny != thy size)
		Melder_throw (U"The number of rows in the Pattern object should equal the number of categories in the Categories object.");
	autoFeatureWeights result = FeatureWeights_compute (me, thee, GET_INTEGER (U"Number of neighbours"));
	praat_new (result.move(), U"Output");
END2 }

FORM (FeatureWeights_computeWrapperExt, U"Feature weights", U"KNN & Pattern & Categories: To FeatureWeights..") {
	POSITIVE (U"Learning rate", U"0.02")
	NATURAL (U"Number of seeds", U"20")
	POSITIVE (U"Stop at", U"1")
	RADIO (U"Optimization", 1)
		RADIOBUTTON (U"Co-optimization")
		RADIOBUTTON (U"Single feature")
	NATURAL (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 3)
		RADIOBUTTON (U"Inversed squared distance")
		RADIOBUTTON (U"Inversed distance")
		RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances <= 0)
		Melder_throw (U"Instance base is empty");
	thouart_ONLY (Pattern);
	heis_ONLY (Categories);
	int mode = GET_INTEGER (U"Vote weighting");
	switch (mode) {
		case 1:
			mode = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			mode = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			mode = kOla_FLAT_VOTING;
			break;
	}
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	if (thy nx != my input -> nx)
		Melder_throw (U"The dimensionality of Pattern should be equal to that of the instance base.");
	autoFeatureWeights result = FeatureWeights_computeWrapperExt (me, thee, him, k, mode, GET_INTEGER (U"Number of seeds"),
		GET_REAL (U"Learning rate"), GET_REAL (U"Stop at"), (int) GET_INTEGER (U"Optimization"));
	praat_new (result.move(), U"Output");
END2 }

FORM (FeatureWeights_computeWrapperInt, U"Feature weights", U"KNN: To FeatureWeights...") {
	POSITIVE (U"Learning rate", U"0.02")
	NATURAL (U"Number of seeds", U"10")
	POSITIVE (U"Stop at", U"1")
	RADIO (U"Optimization", 1)
		RADIOBUTTON (U"Co-optimization")
		RADIOBUTTON (U"Single feature")
	RADIO (U"Evaluation method", 1)
		RADIOBUTTON (U"Leave one out")
		RADIOBUTTON (U"10-fold cross-validation")
	NATURAL (U"k neighbours", U"1")
	RADIO (U"Vote weighting", 3)
		RADIOBUTTON (U"Inversed squared distance")
		RADIOBUTTON (U"Inversed distance")
		RADIOBUTTON (U"Flat")
	OK2
DO
	iam_ONLY (KNN);
	if (my nInstances < 1)
		Melder_throw (U"Instance base is empty");
	int emode = GET_INTEGER (U"Evaluation method");
	switch (emode) {
		case 2:
			emode = kOla_TEN_FOLD_CROSS_VALIDATION;
			break;
		case 1:
			emode = kOla_LEAVE_ONE_OUT;
			break;
	}
	int mode = GET_INTEGER (U"Vote weighting");
	switch (mode) {
		case 1:
			mode = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			mode = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			mode = kOla_FLAT_VOTING;
			break;
	}
	long k = GET_INTEGER (U"k neighbours");
	if (k < 1 || k > my nInstances)
		Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
	autoFeatureWeights result = FeatureWeights_computeWrapperInt (me, k, mode, GET_INTEGER (U"Number of seeds"), GET_REAL (U"Learning rate"),
		GET_REAL (U"Stop at"), (int) GET_INTEGER (U"Optimization"), emode);
	praat_new (result.move(), U"Output");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Creation and processing of auxiliary datatypes                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (Pattern_create, U"Create Pattern", 0) {
	WORD (U"Name", U"1x1")
	NATURAL (U"Dimension of a pattern", U"1")
	NATURAL (U"Number of patterns", U"1")
	OK2
DO
	autoPattern result = Pattern_create (GET_INTEGER (U"Number of patterns"), GET_INTEGER (U"Dimension of a pattern"));
	praat_new (result.move(), GET_STRING (U"Name"));
END2 }

FORM (Categories_create, U"Create Categories", 0) {
	WORD (U"Name", U"empty")
	OK2
DO
	autoCategories result = Categories_create ();
	praat_new (result.move(), GET_STRING (U"Name"));
END2 }

FORM (FeatureWeights_create, U"Create FeatureWeights", 0) {
	WORD (U"Name", U"empty")
	NATURAL (U"Number of weights", U"1")
	OK2
DO
	autoFeatureWeights result = FeatureWeights_create (GET_INTEGER (U"Number of weights"));
	praat_new (result.move(), GET_STRING (U"Name"));
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG                                                                               //
/////////////////////////////////////////////////////////////////////////////////////////

// Disabled
/*
#ifdef _DEBUG

DIRECT (KNN_debug_KNN_SA_partition)
    Pattern p = ONLY (classPattern);
    autoPattern output = Pattern_create (p->ny, p->nx);
    autoNUMvector <long> result (0, p->ny);
    KNN_SA_partition (p, 1, p->ny, result);

    for (long k = 1, c = 1; k <= output->ny; ++k, ++c)
        for (long i = 1; i <= p->ny && k <= output->ny; ++i)
            if(result [i] == c)
            {
                for(long j = 1; j <= output->nx; ++j)
                    output->z[k][j] = p->z[i][j];
                ++k;
            }

    praat_new (output.move(), U"Output");

END

DIRECT (KNN_debug_KNN_getNumberOfCPUs)
    Melder_information (KNN_getNumberOfCPUs(), U" CPUs available");
END

DIRECT (KNN_debug_KNN_threadTest)
    KNN_threadTest();
END

#endif
*/

/////////////////////////////////////////////////////////////////////////////////////////
// Help                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT2 (KNN_help) {
	Melder_help (U"KNN classifiers");
END2 }

DIRECT2 (hint_KNN_and_FeatureWeights_evaluate) {
	Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN and a FeatureWeights object and choosing \"Evaluate...\".");
END2 }

DIRECT2 (hint_KNN_and_Pattern_classify) {
	Melder_information (U"You can use the KNN as a classifier by selecting a KNN and a Pattern and choosing \"To Categories...\" or \"To TableOfReal...\".");
END2 }

DIRECT2 (hint_KNN_and_Pattern_and_FeatureWeights_classify) {
	Melder_information (U"You can use the KNN as a classifier by selecting a KNN, a Pattern and an FeatureWeights object and choosing \"To Categories...\" or \"To TableOfReal...\".");
END2 }

DIRECT2 (hint_KNN_and_Pattern_and_Categories_learn) {
	Melder_information (U"You can train a KNN by selecting a KNN, a Pattern and a Categories object together and choosing \"Learn...\".");
END2 }

DIRECT2 (hint_KNN_and_Pattern_and_Categories_evaluate) {
	Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN, a test Pattern and the corresponding Categories object and choosing \"Evaluate...\".");
END2 }

DIRECT2 (hint_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate) {
	Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN, a test Pattern, an FeatureWeights object, and the corresponding Categories object and choosing \"Evaluate...\".");
END2 }

DIRECT2 (hint_Pattern_and_FeatureWeights_to_Categories) {
	Melder_information (U"A Pattern object and a FeatureWeights object can be used to compute a fixed number of clusters using the k-means clustering clustering algorithm.");
END2 }

DIRECT2 (hint_Pattern_and_FeatureWeights_to_Dissimilarity) {
	Melder_information (U"A Dissimilarity matrix can be generated from a Pattern and a FeatureWeights object.");
END2 }

/////////////////////////////////////////////////////////////////////////////////////////
// Setting callbacks                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////

void praat_contrib_Ola_KNN_init ();
void praat_contrib_Ola_KNN_init ()
{
    Thing_recognizeClassesByName (classKNN, NULL);
    Thing_recognizeClassesByName (classFeatureWeights, NULL);

//////////
// Menu //
//////////

    praat_addMenuCommand (U"Objects", U"New", U"kNN classifiers", 0, 0, 0);

    praat_addMenuCommand (U"Objects", U"New", U"kNN classifiers", 0, 1, DO_KNN_help);
    praat_addMenuCommand (U"Objects", U"New", U"-- KNN --", 0, 1, 0);

    praat_addMenuCommand (U"Objects", U"New", U"Create kNN classifier...", 0, 1, DO_KNN_create);

    praat_addMenuCommand (U"Objects", U"New", U"Advanced", 0, 1, 0);
    praat_addMenuCommand (U"Objects", U"New", U"Create Pattern...", 0, 2, DO_Pattern_create);
    praat_addMenuCommand (U"Objects", U"New", U"Create Categories...", 0, 2, DO_Categories_create);
    praat_addMenuCommand (U"Objects", U"New", U"Create FeatureWeights...", 0, 2, DO_FeatureWeights_create);

/////////////
// Actions //
/////////////

    praat_addAction1 (classKNN, 0, U"kNN help", 0, 0, DO_KNN_help);
    praat_addAction1 (classKNN, 0, QUERY_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 1, U"Get optimized parameters...", 0, 2, DO_KNN_getOptimumModel);
    praat_addAction1 (classKNN, 1, U"Get accuracy estimate...", 0, 2, DO_KNN_evaluate);
    praat_addAction1 (classKNN, 1, U"Get size of instancebase", 0, 2, DO_KNN_getNumberOfInstances);

    praat_addAction1 (classKNN, 0, MODIFY_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 1, U"Shuffle", 0, 1, DO_KNN_shuffle);
    praat_addAction1 (classKNN, 1, U"Prune...", 0, 1, DO_KNN_prune);
    praat_addAction1 (classKNN, 1, U"Reset...", 0, 1, DO_KNN_reset);
    praat_addAction1 (classKNN, 0, EXTRACT_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 0, U"Extract input Patterns", 0, 1, DO_KNN_extractInputPatterns);
    praat_addAction1 (classKNN, 0, U"Extract output Categories", 0, 1, DO_KNN_extractOutputCategories);

    praat_addAction1 (classKNN, 0, U"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeWrapperInt);

 // praat_addAction1 (classKNN, 0, U"To Permutation...", 0, 0, DO_KNN_SA_computePermutation);
 // praat_addAction2 (classKNN, 1, classFeatureWeights, 1, U"To Permutation...", 0, 0, DO_KNN_evaluateWithFeatureWeights);

    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, U"Learn...", 0, 0, DO_KNN_learn);
    praat_addAction2 (classKNN, 1, classFeatureWeights, 1, U"Evaluate...", 0, 0, DO_KNN_evaluateWithFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, U"Evaluate...", 0, 0, DO_KNN_evaluateWithTestSet);
    praat_addAction4 (classKNN, 1, classPattern, 1, classCategories, 1, classFeatureWeights, 1, U"Evaluate...", 0, 0, DO_KNN_evaluateWithTestSetAndFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, U"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeWrapperExt);
    praat_addAction2 (classKNN, 1, classPattern, 1, U"To Categories...", 0, 0, DO_KNN_toCategories);
    praat_addAction2 (classKNN, 1, classPattern, 1, U"To TableOfReal...", 0, 0, DO_KNN_toTableOfReal);

    praat_addAction (classKNN, 1, classPattern, 1, classFeatureWeights, 1, U"To Categories...", 0, 0, DO_KNN_toCategoriesWithFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classFeatureWeights, 1, U"To TableOfReal...", 0, 0, DO_KNN_toTableOfRealWithFeatureWeights);

    praat_addAction1 (classPattern, 1, U"To Dissimilarity", 0, 1, DO_KNN_patternToDissimilarity);
    praat_addAction1 (classPattern, 1, U"To Categories...", 0, 1, DO_Pattern_to_Categories_cluster);
    praat_addAction2 (classPattern, 1, classFeatureWeights, 1, U"To Dissimilarity", 0, 0, DO_KNN_patternToDissimilarityWithFeatureWeights);
    praat_addAction2 (classPattern, 1, classFeatureWeights, 1, U"To Categories...", 0, 0, DO_Pattern_to_Categories_clusterWithFeatureWeights);

    praat_addAction2 (classPattern, 1, classCategories, 1, U"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeRELIEF);
    praat_addAction2 (classPattern, 1, classCategories, 1, U"To KNN Classifier...", 0, 0, DO_KNN_Pattern_Categories_to_KNN);

///////////
// DEBUG //
///////////

/*
#ifdef _DEBUG

    praat_addAction1 (classKNN, 0, U"_DEBUG: KNN_getNumberOfCPUs", 0, 0, DO_KNN_debug_KNN_getNumberOfCPUs);
    praat_addAction1 (classKNN, 0, U"_DEBUG: KNN_threadTest", 0, 0, DO_KNN_debug_KNN_threadTest);
    praat_addAction1 (classPattern, 1, U"_DEBUG: KNN_SA_partition", 0, 1, DO_KNN_debug_KNN_SA_partition);

#endif
*/

///////////
// Hints //
///////////

    praat_addAction1 (classPattern, 0, U"& FeatureWeights: To Categories?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Categories);
    praat_addAction1 (classPattern, 0, U"& FeatureWeights: To Dissimilarity?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Dissimilarity);

    praat_addAction1 (classKNN, 0, U"& FeatureWeights: Evaluate?", 0, 0, DO_hint_KNN_and_FeatureWeights_evaluate);
//  praat_addAction1 (classKNN, 0, U"& FeatureWeights: To Permutation?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Dissimilarity);
    praat_addAction1 (classKNN, 0, U"& Pattern: Classify?", 0, 0, DO_hint_KNN_and_Pattern_classify);
    praat_addAction1 (classKNN, 0, U"& Pattern & FeatureWeights: Classify?", 0, 0, DO_hint_KNN_and_Pattern_and_FeatureWeights_classify);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories: Learn?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_learn);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories: Evaluate?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_evaluate);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories & FeatureWeights: Evaluate?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate);

    INCLUDE_MANPAGES (manual_KNN_init)
}

/* End of file praat_contrib_Ola_KNN.cpp */
