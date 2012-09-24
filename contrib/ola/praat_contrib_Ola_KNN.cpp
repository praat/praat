/* praat_contrib_Ola_KNN.cpp
 *
 * Copyright (C) 2007-2009 Ola Söder, 2010-2011 Paul Boersma
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

static const wchar_t *QUERY_BUTTON   = L"Query -";
static const wchar_t *MODIFY_BUTTON  = L"Modify -";
static const wchar_t *EXTRACT_BUTTON = L"Extract -";


/////////////////////////////////////////////////////////////////////////////////////////
// KNN creations                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_create, L"Create kNN Classifier", L"kNN classifiers 1. What is a kNN classifier?")
    WORD (L"Name", L"Classifier")
    OK  
DO
    autoKNN knn = KNN_create ();
    praat_new (knn.transfer(), GET_STRING (L"Name"));
END

FORM (KNN_Pattern_Categories_to_KNN, L"Create kNN classifier", L"kNN classifiers 1. What is a kNN classifier?" )
    WORD (L"Name", L"Classifier")
    RADIO (L"Ordering", 1)
    RADIOBUTTON (L"Random")
    RADIOBUTTON (L"Sequential")
    OK
DO
	iam_ONLY (Pattern);
	thouart_ONLY (Categories);
    int ordering = GET_INTEGER (L"Ordering");
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
			Melder_throw ("The number of Categories should be equal to the number of rows in Pattern.");
		case kOla_DIMENSIONALITY_MISMATCH:
			Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
		default:
			praat_new (knn.transfer(), GET_STRING(L"Name"));
	}
END





/////////////////////////////////////////////////////////////////////////////////////////
// KNN extractions, queries and modifications                                         //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (KNN_getNumberOfInstances)
    iam_ONLY (KNN);
    Melder_information (Melder_integer (my nInstances), L" units");
END

FORM (KNN_getOptimumModel, L"kNN model selection", L"kNN classifiers 1.1.2. Model selection")
    RADIO (L"Evaluation method", 1)
    RADIOBUTTON (L"Leave one out")
    RADIOBUTTON (L"10-fold cross-validation")
    INTEGER (L"k max", L"50")
    INTEGER (L"Number of seeds", L"10")
    POSITIVE (L"Learning rate", L"0.2")
    OK
DO
    iam_ONLY (KNN);
    long k = GET_INTEGER (L"k max");
    double lrate = GET_REAL (L"Learning rate");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k max such that 0 < k max < ", my nInstances + 1, ".");
    long nseeds = GET_INTEGER (L"Number of seeds");
    if (nseeds < 1)
        Melder_throw ("The number of seeds should exceed 1.");
    int mode = GET_INTEGER (L"Evaluation method");
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
            Melder_information (L"Vote weighting: Inversed squared distance\n", L"k: ", Melder_integer(k));
            break;
        case kOla_DISTANCE_WEIGHTED_VOTING:
            Melder_information (L"Vote weighting: Inversed distance\n", L"k: ", Melder_integer(k));
            break;
        case kOla_FLAT_VOTING:
            Melder_information (L"Vote weighting: Flat\n", L"k: ", Melder_integer(k));
            break;
    }
END

FORM (KNN_evaluate, L"Evaluation", L"KNN: Get accuracy estimate...")
    RADIO (L"Evaluation method", 1)
    RADIOBUTTON (L"Leave one out")
    RADIOBUTTON (L"10-fold cross-validation")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK

DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    int vt = GET_INTEGER (L"Vote weighting");
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
    int mode = GET_INTEGER (L"Evaluation method");
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
		Melder_throw ("The number of feature weights should be equal to the dimensionality of the Pattern.");
    Melder_information (Melder_double (100 * result), L" percent of the instances correctly classified.");   // BUG: use Melder_percent
END

FORM (KNN_evaluateWithFeatureWeights, L"Evaluation", L"KNN & FeatureWeights: Get accuracy estimate...")
    RADIO (L"Evaluation method", 1)
    RADIOBUTTON (L"Leave one out")
    RADIOBUTTON (L"10-fold cross-validation")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK

DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
		Melder_throw ("Instance base is empty");
    thouart_ONLY (FeatureWeights);
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    int vt = GET_INTEGER (L"Vote weighting");
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
	int mode = GET_INTEGER (L"Evaluation method");
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
        Melder_throw ("The number of feature weights should be equal to the dimensionality of the Pattern.");
    Melder_information (Melder_double (100 * result), L" percent of the instances correctly classified.");
END


DIRECT  (KNN_extractInputPatterns)
    iam_ONLY (KNN);
    if (my nInstances > 0) {
        praat_new (Data_copy (my input), L"Input Patterns");
    } else {
        Melder_throw ("Instance base is empty.");
	}
END

DIRECT  (KNN_extractOutputCategories)
    iam_ONLY (KNN);
    if (my nInstances > 0) {
        praat_new (Data_copy (my output), L"Output Categories");
    } else {
        Melder_throw ("Instance base is empty.");
	}
END

FORM (KNN_reset, L"Reset", L"KNN: Reset...")
    LABEL (L"", L"Warning: this command destroys all previous learning.")
    OK
DO
    iam_ONLY (KNN);
    forget (my input);
    forget (my output);
    my nInstances = 0;
	praat_dataChanged (me);   // BUG: this should be inserted much more often
END

DIRECT  (KNN_shuffle)
    iam_ONLY (KNN);
    if (my nInstances > 0)  
        KNN_shuffleInstances (me);
    else
        Melder_throw ("Instance base is empty.");
END

FORM (KNN_prune, L"Pruning", L"KNN: Prune...")
    POSITIVE (L"Noise pruning degree", L"1")
    POSITIVE (L"Redundancy pruning degree", L"1")
    INTEGER (L"k neighbours", L"1")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    long oldn = my nInstances;   // save before it changes!
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    double n = GET_REAL (L"Noise pruning degree");
    double r = GET_REAL (L"Redundancy pruning degree");
    if (n <= 0 || n > 1 || r <= 0 || r > 1)
        Melder_throw ("Please select a pruning degree d such that 0 < d <= 1.");
    long npruned = KNN_prune_prune (me, n, r, k);
    Melder_information (Melder_integer (npruned), L" instances discarded. \n", L"Size of new instance base: ", Melder_integer (oldn - npruned));
END





/////////////////////////////////////////////////////////////////////////////////////////
// Learning                                                                            //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_learn, L"Learning", L"kNN classifiers 1. What is a kNN classifier?")
    RADIO (L"Learning method", 1)
    RADIOBUTTON (L"Append new information")
    RADIOBUTTON (L"Replace current intancebase")
    RADIO (L"Ordering", 1)
    RADIOBUTTON (L"Random")
    RADIOBUTTON (L"Sequential")
    OK
DO
    iam_ONLY (KNN);
    thouart_ONLY (Pattern);
    heis_ONLY (Categories);
    int ordering = GET_INTEGER (L"Ordering");
    switch (ordering) {
        case 1:
            ordering = kOla_SHUFFLE;
            break;
        case 2:
            ordering = kOla_SEQUENTIAL;
    }
    int method = GET_INTEGER (L"Learning method");
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
            Melder_throw ("The number of Categories should be equal to the number of rows in Pattern.");
        case kOla_DIMENSIONALITY_MISMATCH:
            Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
    }
END




/////////////////////////////////////////////////////////////////////////////////////////
// Evaluation                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////


FORM (KNN_evaluateWithTestSet, L"Evaluation", L"KNN & Pattern & Categories: Evaluate...")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty");
    thouart_ONLY (Pattern);
    heis_ONLY (Categories);
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    int vt = GET_INTEGER (L"Vote weighting");
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
        Melder_throw ("The number of Categories should be equal to the number of rows in Pattern.");
    if (thy nx != (my input)->nx)
        Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
    autoFeatureWeights fws = FeatureWeights_create (thy nx);
    double result = KNN_evaluateWithTestSet (me, thee, him, fws.peek(), k, vt);
    Melder_information (Melder_double (100 * result), L" percent of the instances correctly classified.");
END

FORM (KNN_evaluateWithTestSetAndFeatureWeights, L"Evaluation", L"KNN & Pattern & Categories & FeatureWeights: Evaluate...")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty");
    Pattern p = (Pattern) ONLY (classPattern);
    Categories c = (Categories) ONLY (classCategories);
    FeatureWeights fws = (FeatureWeights) ONLY (classFeatureWeights);
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    int vt = GET_INTEGER (L"Vote weighting");
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
        Melder_throw ("The number of Categories should be equal to the number of rows in Pattern.");
    if (p -> nx != my input -> nx)
        Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
    if (p->nx != fws -> fweights -> numberOfColumns)
        Melder_throw ("The number of feature weights should be equal to the dimensionality of the Pattern.");
    double result = KNN_evaluateWithTestSet (me, p, c, fws, k, vt);
    Melder_information (Melder_double (100 * result), L" percent of the instances correctly classified.");
END




/////////////////////////////////////////////////////////////////////////////////////////
// Classification                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (KNN_toCategories, L"Classification", L"KNN & Pattern: To Categories...")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    thouart_ONLY (Pattern);
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    int vt = GET_INTEGER (L"Vote weighting");
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
        Melder_throw ("The dimensionality of Pattern should match that of the instance base.");
    autoFeatureWeights fws = FeatureWeights_create (thy nx);
    praat_new (KNN_classifyToCategories (me, thee, fws.peek(), k, vt), L"Output");
END

FORM (KNN_toTableOfReal, L"Classification", L"KNN & Pattern: To TabelOfReal...")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    thouart_ONLY (Pattern);
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    autoFeatureWeights fws = FeatureWeights_create(thy nx);
    int vt = GET_INTEGER (L"Vote weighting");
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
        Melder_throw ("The dimensionality of Pattern should match that of the instance base.");
    praat_new (KNN_classifyToTableOfReal (me, thee, fws.peek(), k, vt), L"Output");
END

FORM (KNN_toCategoriesWithFeatureWeights, L"Classification", L"KNN & Pattern & FeatureWeights: To Categories...")
    INTEGER (L"k neighbours", L"KNN & Pattern & FeatureWeights: To Categories...")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")  
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    thouart_ONLY (Pattern);
    heis_ONLY (FeatureWeights);
    int vt = GET_INTEGER (L"Vote weighting");
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
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    if (thy nx != (my input)->nx)
        Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
    if (thy nx != his fweights -> numberOfColumns)
        Melder_throw ("The number of feature weights should be equal to the dimensionality of the Pattern.");
	praat_new (KNN_classifyToCategories (me, thee, him, k, vt), L"Output");
END

FORM (KNN_toTableOfRealWithFeatureWeights, L"Classification", L"KNN & Pattern & FeatureWeights: To TableOfReal...")
    INTEGER (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 1)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK
DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty.");
    thouart_ONLY (Pattern);
    heis_ONLY (FeatureWeights);
    long k = GET_INTEGER (L"k neighbours");
    int vt = GET_INTEGER (L"Vote weighting");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, "\n");
    if (thy nx != his fweights -> numberOfColumns)
        Melder_throw ("The number of features and the number of feature weights should be equal.");
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
    praat_new (KNN_classifyToTableOfReal (me, thee, him, k, vt), L"Output");
END






/////////////////////////////////////////////////////////////////////////////////////////
// Clustering                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (Pattern_to_Categories_cluster, L"k-means clustering", L"Pattern: To Categories...")
    INTEGER (L"k clusters", L"1")
    POSITIVE (L"Cluster size ratio constraint", L"0.0000001");
    INTEGER (L"Maximum number of reseeds", L"1000")
    OK
DO
    iam_ONLY (Pattern);
    if (my nx > 0 && my ny > 0) {
        long k = GET_INTEGER (L"k clusters");
        if (k < 1 || k > my ny)
            Melder_throw ("Please select a value of k such that 0 < k <= ", my ny, ".");
        long rs =  GET_INTEGER (L"Maximum number of reseeds");
        if (rs < 0)
            Melder_throw ("The maximum number of reseeds should not be negative.");
        double rc =  GET_REAL (L"Cluster size ratio constraint");
		if (rc > 1 || rc <= 0)
            Melder_throw ("Please select a value of the cluster size ratio constraint c such that 0 < c <= 1.");
        autoFeatureWeights fws = FeatureWeights_create (my nx);
        praat_new (Pattern_to_Categories_cluster (me, fws.peek(), k, rc, rs), L"Output");
    } else {
        Melder_throw ("Pattern is empty.");
	}
END

FORM (Pattern_to_Categories_clusterWithFeatureWeights, L"k-means clustering", L"Pattern & FeatureWeights: To Categories...")
    INTEGER (L"k clusters", L"1")
    POSITIVE (L"Cluster size ratio constraint", L"0.0000001");
    INTEGER (L"Maximum number of reseeds", L"1000")
    OK
DO
    iam_ONLY (Pattern);
    if (my nx > 0 && my ny > 0) {
        thouart_ONLY (FeatureWeights); 
        if (my nx != thy fweights -> numberOfColumns)
            Melder_throw ("The number of features and the number of feature weights should be equal.");
		long k = GET_INTEGER(L"k clusters");
        if (k < 1 || k > my ny)
            Melder_throw ("Please select a value of k such that 0 < k <= ", my ny, ".");
        long rs =  GET_INTEGER(L"Maximum number of reseeds");
        if (rs < 0)
            Melder_throw ("The maximum number of reseeds should not be negative.");
        double rc =  GET_REAL(L"Cluster size ratio constraint");
		if (rc > 1 || rc <= 0)
            Melder_throw ("Please select a value of the cluster size ratio constraint c such that 0 < c <= 1.");
        praat_new (Pattern_to_Categories_cluster (me, thee, k, rc, rs), L"Output");
    } else {
        Melder_throw ("Pattern is empty.");
	}
END






/////////////////////////////////////////////////////////////////////////////////////////
// Dissimilarity computations                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (KNN_patternToDissimilarity)
    iam_ONLY (Pattern);
    autoFeatureWeights fws = FeatureWeights_create (my nx);
    praat_new (KNN_patternToDissimilarity (me, fws.peek()), L"Output");
END

DIRECT (KNN_patternToDissimilarityWithFeatureWeights)
    iam_ONLY (Pattern);
    thouart_ONLY (FeatureWeights);  
    if (my nx != thy fweights -> numberOfColumns)
        Melder_throw ("The number of features and the number of feature weights should be equal.");
	praat_new (KNN_patternToDissimilarity (me, thee), L"Output");
END


/////////////////////////////////////////////////////////////////////////////////////////
// Computation of permutation                                                          //
/////////////////////////////////////////////////////////////////////////////////////////


FORM (KNN_SA_computePermutation, L"To Permutation...", L"Pattern & Categories: To FeatureWeights...")
    
    NATURAL(L"Tries per step", L"200")
    NATURAL(L"Iterations", L"10")
    POSITIVE(L"Step size", L"10")
    POSITIVE(L"Boltzmann constant", L"1.0")
    POSITIVE(L"Initial temperature", L"0.002")
    POSITIVE(L"Damping factor", L"1.005")
    POSITIVE(L"Final temperature", L"0.000002")
    OK
DO
    iam_ONLY (KNN);
    long tries = GET_INTEGER (L"Tries per step");
    long iterations = GET_INTEGER (L"Iterations");
    double step_size = GET_REAL (L"Step size");
    double bolzmann_c = GET_REAL (L"Boltzmann constant");
    double temp_start = GET_REAL (L"Initial temperature");
    double temp_damp = GET_REAL (L"Damping factor");
    double temp_stop = GET_REAL (L"Final temperature");
	praat_new (KNN_SA_ToPermutation (me, tries, iterations, step_size, bolzmann_c, temp_start, temp_damp, temp_stop), L"Output");
END



/////////////////////////////////////////////////////////////////////////////////////////
// Computation of feature weights                                                      //
/////////////////////////////////////////////////////////////////////////////////////////


FORM (FeatureWeights_computeRELIEF, L"Feature weights", L"Pattern & Categories: To FeatureWeights...")
    INTEGER (L"Number of neighbours", L"1")
    OK
DO
    iam_ONLY (Pattern);
    thouart_ONLY (Categories);
    if (my ny < 2)
        Melder_throw ("The Pattern object should contain at least 2 rows.");
    if (my ny != thy size)
        Melder_throw ("The number of rows in the Pattern object should equal the number of categories in the Categories object.");
    praat_new (FeatureWeights_compute (me, thee, GET_INTEGER (L"Number of neighbours")), L"Output");
END

FORM (FeatureWeights_computeWrapperExt, L"Feature weights", L"KNN & Pattern & Categories: To FeatureWeights..")
    POSITIVE(L"Learning rate", L"0.02")
    NATURAL(L"Number of seeds", L"20")
    POSITIVE(L"Stop at", L"1")
    RADIO (L"Optimization", 1)
    RADIOBUTTON (L"Co-optimization")
    RADIOBUTTON (L"Single feature")

    NATURAL (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 3)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")  
    RADIOBUTTON (L"Flat")
    OK

DO
    iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty");
    thouart_ONLY (Pattern);
    heis_ONLY (Categories);
    int mode = GET_INTEGER (L"Vote weighting");
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
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    if (thy nx != my input -> nx)
        Melder_throw ("The dimensionality of Pattern should be equal to that of the instance base.");
    praat_new (FeatureWeights_computeWrapperExt (me, thee, him, k, mode, GET_INTEGER (L"Number of seeds"),
		GET_REAL (L"Learning rate"), GET_REAL (L"Stop at"), (int) GET_INTEGER (L"Optimization")), L"Output");
END

FORM (FeatureWeights_computeWrapperInt, L"Feature weights", L"KNN: To FeatureWeights...")
    POSITIVE(L"Learning rate", L"0.02")
    NATURAL(L"Number of seeds", L"10")
    POSITIVE(L"Stop at", L"1")
    RADIO (L"Optimization", 1)
    RADIOBUTTON (L"Co-optimization")
    RADIOBUTTON (L"Single feature")
    RADIO (L"Evaluation method", 1)
    RADIOBUTTON (L"Leave one out")
    RADIOBUTTON (L"10-fold cross-validation")
    NATURAL (L"k neighbours", L"1")
    RADIO (L"Vote weighting", 3)
    RADIOBUTTON (L"Inversed squared distance")
    RADIOBUTTON (L"Inversed distance")
    RADIOBUTTON (L"Flat")
    OK

DO
	iam_ONLY (KNN);
    if (my nInstances < 1)
        Melder_throw ("Instance base is empty");
    int emode = GET_INTEGER (L"Evaluation method");
    switch (emode) {
        case 2:
            emode = kOla_TEN_FOLD_CROSS_VALIDATION;
            break;
        case 1:
            emode = kOla_LEAVE_ONE_OUT;
            break;
    }
    int mode = GET_INTEGER (L"Vote weighting");
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
    long k = GET_INTEGER (L"k neighbours");
    if (k < 1 || k > my nInstances)
        Melder_throw ("Please select a value of k such that 0 < k < ", my nInstances + 1, ".");
    praat_new (FeatureWeights_computeWrapperInt (me, k, mode, GET_INTEGER (L"Number of seeds"), GET_REAL (L"Learning rate"),
		GET_REAL (L"Stop at"), (int) GET_INTEGER (L"Optimization"), emode), L"Output");
END






/////////////////////////////////////////////////////////////////////////////////////////
// Creation and processing of auxiliary datatypes                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (Pattern_create, L"Create Pattern", 0)
    WORD (L"Name", L"1x1")
    NATURAL (L"Dimension of a pattern", L"1")
    NATURAL (L"Number of patterns", L"1")
    OK
DO
    praat_new (Pattern_create (GET_INTEGER (L"Number of patterns"), GET_INTEGER (L"Dimension of a pattern")), GET_STRING (L"Name"));
END

FORM (Categories_create, L"Create Categories", 0)
    WORD (L"Name", L"empty")
    OK
DO
    praat_new (Categories_create (), GET_STRING (L"Name"));
END

FORM (FeatureWeights_create, L"Create FeatureWeights", 0)
    WORD (L"Name", L"empty")
NATURAL (L"Number of weights", L"1")
OK
DO
    praat_new (FeatureWeights_create (GET_INTEGER (L"Number of weights")), GET_STRING (L"Name"));
END


/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG                                                                               //
/////////////////////////////////////////////////////////////////////////////////////////

// Disabled
/*
#ifdef _DEBUG

DIRECT (KNN_debug_KNN_SA_partition)
    Pattern p = ONLY (classPattern);
    Pattern output = Pattern_create (p->ny, p->nx);
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

    praat_new (output, L"Output");

END

DIRECT (KNN_debug_KNN_getNumberOfCPUs)
    Melder_information (Melder_integer(KNN_getNumberOfCPUs()), L" CPUs available");
END

DIRECT (KNN_debug_KNN_threadTest)
    KNN_threadTest();
END

#endif
*/




/////////////////////////////////////////////////////////////////////////////////////////
// Help                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (KNN_help)
    Melder_help (L"KNN classifiers");
END

DIRECT (hint_KNN_and_FeatureWeights_evaluate)
    Melder_information (L"The accuracy of a KNN can be estimated by selecting a KNN and a FeatureWeights object and choosing \"Evaluate...\".");
END

DIRECT (hint_KNN_and_Pattern_classify)
    Melder_information (L"You can use the KNN as a classifier by selecting a KNN and a Pattern and choosing \"To Categories...\" or \"To TableOfReal...\".");
END

DIRECT (hint_KNN_and_Pattern_and_FeatureWeights_classify)
    Melder_information (L"You can use the KNN as a classifier by selecting a KNN, a Pattern and an FeatureWeights object and choosing \"To Categories...\" or \"To TableOfReal...\".");
END

DIRECT (hint_KNN_and_Pattern_and_Categories_learn)
    Melder_information (L"You can train a KNN by selecting a KNN, a Pattern and a Categories object together and choosing \"Learn...\".");
END

DIRECT (hint_KNN_and_Pattern_and_Categories_evaluate)
    Melder_information (L"The accuracy of a KNN can be estimated by selecting a KNN, a test Pattern and the corresponding Categories object and choosing \"Evaluate...\".");
END

DIRECT (hint_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate)
    Melder_information (L"The accuracy of a KNN can be estimated by selecting a KNN, a test Pattern, an FeatureWeights object, and the corresponding Categories object and choosing \"Evaluate...\".");
END

DIRECT (hint_Pattern_and_FeatureWeights_to_Categories)
    Melder_information (L"A Pattern object and a FeatureWeights object can be used to compute a fixed number of clusters using the k-means clustering clustering algorithm.");
END

DIRECT (hint_Pattern_and_FeatureWeights_to_Dissimilarity)
    Melder_information (L"A Dissimilarity matrix can be generated from a Pattern and a FeatureWeights object.");
END







/////////////////////////////////////////////////////////////////////////////////////////
// Setting callbacks                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////

void praat_contrib_Ola_KNN_init (void);
void praat_contrib_Ola_KNN_init (void)
{

    Thing_recognizeClassesByName (classKNN, NULL);
    Thing_recognizeClassesByName (classFeatureWeights, NULL);

//////////
// Menu //
//////////

    praat_addMenuCommand (L"Objects", L"New", L"kNN classifiers", 0, 0, 0);

    praat_addMenuCommand (L"Objects", L"New", L"kNN classifiers", 0, 1, DO_KNN_help);
    praat_addMenuCommand (L"Objects", L"New", L"-- KNN --", 0, 1, 0);

    praat_addMenuCommand (L"Objects", L"New", L"Create kNN classifier...", 0, 1, DO_KNN_create);

    praat_addMenuCommand (L"Objects", L"New", L"Advanced", 0, 1, 0);
    praat_addMenuCommand (L"Objects", L"New", L"Create Pattern...", 0, 2, DO_Pattern_create);
    praat_addMenuCommand (L"Objects", L"New", L"Create Categories...", 0, 2, DO_Categories_create);
    praat_addMenuCommand (L"Objects", L"New", L"Create FeatureWeights...", 0, 2, DO_FeatureWeights_create);


/////////////
// Actions //
/////////////

    praat_addAction1 (classKNN, 0, L"kNN help", 0, 0, DO_KNN_help);
    praat_addAction1 (classKNN, 0, QUERY_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 1, L"Get optimized parameters...", 0, 2, DO_KNN_getOptimumModel);
    praat_addAction1 (classKNN, 1, L"Get accuracy estimate...", 0, 2, DO_KNN_evaluate);
    praat_addAction1 (classKNN, 1, L"Get size of instancebase", 0, 2, DO_KNN_getNumberOfInstances);

    praat_addAction1 (classKNN, 0, MODIFY_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 1, L"Shuffle", 0, 1, DO_KNN_shuffle);
    praat_addAction1 (classKNN, 1, L"Prune...", 0, 1, DO_KNN_prune);
    praat_addAction1 (classKNN, 1, L"Reset...", 0, 1, DO_KNN_reset);
    praat_addAction1 (classKNN, 0, EXTRACT_BUTTON, 0, 0, 0);
    praat_addAction1 (classKNN, 0, L"Extract input Patterns", 0, 1, DO_KNN_extractInputPatterns);
    praat_addAction1 (classKNN, 0, L"Extract output Categories", 0, 1, DO_KNN_extractOutputCategories);

    praat_addAction1 (classKNN, 0, L"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeWrapperInt);

 // praat_addAction1 (classKNN, 0, L"To Permutation...", 0, 0, DO_KNN_SA_computePermutation);
 // praat_addAction2 (classKNN, 1, classFeatureWeights, 1, L"To Permutation...", 0, 0, DO_KNN_evaluateWithFeatureWeights);

    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, L"Learn...", 0, 0, DO_KNN_learn);
    praat_addAction2 (classKNN, 1, classFeatureWeights, 1, L"Evaluate...", 0, 0, DO_KNN_evaluateWithFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, L"Evaluate...", 0, 0, DO_KNN_evaluateWithTestSet);
    praat_addAction4 (classKNN, 1, classPattern, 1, classCategories, 1, classFeatureWeights, 1, L"Evaluate...", 0, 0, DO_KNN_evaluateWithTestSetAndFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classCategories, 1, L"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeWrapperExt);
    praat_addAction2 (classKNN, 1, classPattern, 1, L"To Categories...", 0, 0, DO_KNN_toCategories);
    praat_addAction2 (classKNN, 1, classPattern, 1, L"To TableOfReal...", 0, 0, DO_KNN_toTableOfReal);

    praat_addAction (classKNN, 1, classPattern, 1, classFeatureWeights, 1, L"To Categories...", 0, 0, DO_KNN_toCategoriesWithFeatureWeights);
    praat_addAction (classKNN, 1, classPattern, 1, classFeatureWeights, 1, L"To TableOfReal...", 0, 0, DO_KNN_toTableOfRealWithFeatureWeights);

    praat_addAction1 (classPattern, 1, L"To Dissimilarity", 0, 1, DO_KNN_patternToDissimilarity);
    praat_addAction1 (classPattern, 1, L"To Categories...", 0, 1, DO_Pattern_to_Categories_cluster);
    praat_addAction2 (classPattern, 1, classFeatureWeights, 1, L"To Dissimilarity", 0, 0, DO_KNN_patternToDissimilarityWithFeatureWeights);
    praat_addAction2 (classPattern, 1, classFeatureWeights, 1, L"To Categories...", 0, 0, DO_Pattern_to_Categories_clusterWithFeatureWeights);

    praat_addAction2 (classPattern, 1, classCategories, 1, L"To FeatureWeights...", 0, 0, DO_FeatureWeights_computeRELIEF);
    praat_addAction2 (classPattern, 1, classCategories, 1, L"To KNN Classifier...", 0, 0, DO_KNN_Pattern_Categories_to_KNN);

///////////
// DEBUG //
///////////

/*
#ifdef _DEBUG

    praat_addAction1 (classKNN, 0, L"_DEBUG: KNN_getNumberOfCPUs", 0, 0, DO_KNN_debug_KNN_getNumberOfCPUs);
    praat_addAction1 (classKNN, 0, L"_DEBUG: KNN_threadTest", 0, 0, DO_KNN_debug_KNN_threadTest);
    praat_addAction1 (classPattern, 1, L"_DEBUG: KNN_SA_partition", 0, 1, DO_KNN_debug_KNN_SA_partition);

#endif
*/

///////////
// Hints //
///////////

    praat_addAction1 (classPattern, 0, L"& FeatureWeights: To Categories?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Categories);
    praat_addAction1 (classPattern, 0, L"& FeatureWeights: To Dissimilarity?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Dissimilarity);

    praat_addAction1 (classKNN, 0, L"& FeatureWeights: Evaluate?", 0, 0, DO_hint_KNN_and_FeatureWeights_evaluate);
//  praat_addAction1 (classKNN, 0, L"& FeatureWeights: To Permutation?", 0, 0, DO_hint_Pattern_and_FeatureWeights_to_Dissimilarity);
    praat_addAction1 (classKNN, 0, L"& Pattern: Classify?", 0, 0, DO_hint_KNN_and_Pattern_classify);
    praat_addAction1 (classKNN, 0, L"& Pattern & FeatureWeights: Classify?", 0, 0, DO_hint_KNN_and_Pattern_and_FeatureWeights_classify);
    praat_addAction1 (classKNN, 0, L"& Pattern & Categories: Learn?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_learn);
    praat_addAction1 (classKNN, 0, L"& Pattern & Categories: Evaluate?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_evaluate);
    praat_addAction1 (classKNN, 0, L"& Pattern & Categories & FeatureWeights: Evaluate?", 0, 0, DO_hint_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate);


    INCLUDE_MANPAGES (manual_KNN_init)
}
