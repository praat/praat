/* praat_contrib_Ola_KNN.cpp
 *
 * Copyright (C) 2007-2009 Ola Söder, 2010-2011,2015,2016 Paul Boersma
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
 * os 2007/05/29 Initial release?
 * os 2009/01/23 Bugfix: Removed MUX:ing (KNN_learn) incompatible with the scripting engine. Thanks to Paul Boersma for spotting this problem.
 * pb 2010/12/28 in messages: typos, English, interpunction
 * pb 2011/07/12 C++ and removed several errors
 * pb 2011/09/18 made a start with handling dataChanged at all
 * pb 2016/10/19 loops, dataChanged, Praat idiom for names, English
 */

#include "KNN.h"
#include "KNN_threads.h"
#include "KNN_prune.h"
#include "Pattern_to_Categories_cluster.h"
#include "FeatureWeights.h"

#include "praat_FFNet.h"

#undef iam
#define iam iam_LOOP

static const char32 *QUERY_BUTTON   = U"Query -";
static const char32 *MODIFY_BUTTON  = U"Modify -";
static const char32 *EXTRACT_BUTTON = U"Extract -";

/////////////////////////////////////////////////////////////////////////////////////////
// KNN creations                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW1_KNN_create, U"Create kNN Classifier", U"kNN classifiers 1. What is a kNN classifier?") {
	WORD (name, U"Name", U"Classifier")
	OK
DO
	CREATE_ONE
		autoKNN result = KNN_create ();
	CREATE_ONE_END (name)
}

FORM (NEW1_PatternList_Categories_to_KNN, U"Create kNN classifier", U"kNN classifiers 1. What is a kNN classifier?") {
	WORD (name, U"Name", U"Classifier")
	RADIOx (ordering, U"Ordering", 1, 1)
		RADIOBUTTON (U"Random")
		RADIOBUTTON (U"Sequential")
	OK
DO
	CONVERT_TWO (PatternList, Categories)
		autoKNN result = KNN_create ();
		switch (ordering) {
			case 1:
				ordering = kOla_SHUFFLE;
				break;
			case 2:
				ordering = kOla_SEQUENTIAL;
		}
		int status = KNN_learn (result.get(), me, you, kOla_REPLACE, ordering);
		if (status == kOla_PATTERN_CATEGORIES_MISMATCH)
			Melder_throw (U"The number of Categories should be equal to the number of rows in PatternList.");
		if (status == kOla_DIMENSIONALITY_MISMATCH)
			Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
	CONVERT_TWO_END (name)
}

/////////////////////////////////////////////////////////////////////////////////////////
// KNN extractions, queries and modifications                                         //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (INTEGER_KNN_getNumberOfInstances) {
    NUMBER_ONE (KNN)
		integer result = my nInstances;
	NUMBER_ONE_END (U" units")
}

FORM (INTEGER_KNN_getOptimumModel, U"kNN model selection", U"kNN classifiers 1.1.2. Model selection") {
	RADIOx (evaluationMethod, U"Evaluation method", 1, 1)
		RADIOBUTTON (U"Leave one out")
		RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (kmax, U"k max", U"50")
	INTEGER (numberOfSeeds, U"Number of seeds", U"10")
	POSITIVE (learningRate, U"Learning rate", U"0.2")
	OK
DO
	INFO_ONE (KNN)
		if (kmax < 1 || kmax > my nInstances)
			Melder_throw (U"Please select a value of k max such that 0 < k max < ", my nInstances + 1, U".");
		if (numberOfSeeds < 1)
			Melder_throw (U"The number of seeds should exceed 1.");   // BUG
		switch (evaluationMethod) {
			case 1:
				evaluationMethod = kOla_LEAVE_ONE_OUT;
				break;
			case 2:
				evaluationMethod = kOla_TEN_FOLD_CROSS_VALIDATION;
				break;
		}
		autoFeatureWeights fws = FeatureWeights_create (my input -> nx);
		int dist;
		KNN_modelSearch (me, fws.get(), & kmax, & dist, evaluationMethod, learningRate, numberOfSeeds);
		switch (dist) {
			case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
				Melder_information (kmax, U" (vote weighting: inverse squared distance)");
				break;
			case kOla_DISTANCE_WEIGHTED_VOTING:
				Melder_information (kmax, U" (vote weighting: inverse distance)");
				break;
			case kOla_FLAT_VOTING:
				Melder_information (kmax, U" (vote weighting: flat)");
				break;
		}
	INFO_ONE_END
}

FORM (REAL_KNN_evaluate, U"Evaluation", U"KNN: Get accuracy estimate...") {
	RADIOx (evaluationMethod, U"Evaluation method", 1, 1)
		RADIOBUTTON (U"Leave one out")
		RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	FIND_ONE (KNN)
		if (my nInstances < 1)
			Melder_throw (U"Instance base is empty.");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		switch (evaluationMethod) {
			case 2:
				evaluationMethod = kOla_TEN_FOLD_CROSS_VALIDATION;
				break;
			case 1: 
				evaluationMethod = kOla_LEAVE_ONE_OUT;
				break;
		}
		autoFeatureWeights fws = FeatureWeights_create (my input -> nx);
		double result = KNN_evaluate (me, fws.get(), kNeighbours, voteWeighting, evaluationMethod);
		if (Melder_iround (result) == kOla_FWEIGHTS_MISMATCH)
			Melder_throw (U"The number of feature weights should be equal to the dimensionality of the PatternList.");
		Melder_information (100 * result, U" percent of the instances correctly classified.");   // BUG: use Melder_percent
	END
}

FORM (REAL_KNN_FeatureWeights_evaluate, U"Evaluation", U"KNN & FeatureWeights: Get accuracy estimate...") {
	RADIOx (evaluationMethod, U"Evaluation method", 1, 1)
		RADIOBUTTON (U"Leave one out")
		RADIOBUTTON (U"10-fold cross-validation")
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	FIND_TWO (KNN, FeatureWeights)
		if (my nInstances < 1)
			Melder_throw (U"Instance base is empty");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}   
		switch (evaluationMethod) {
			case 2:
				evaluationMethod = kOla_TEN_FOLD_CROSS_VALIDATION;
				break;
			case 1:
				evaluationMethod = kOla_LEAVE_ONE_OUT;
				break;
		}
		double result = KNN_evaluate (me, you, kNeighbours, voteWeighting, evaluationMethod);
		if (Melder_iround (result) == kOla_FWEIGHTS_MISMATCH)
			Melder_throw (U"The number of feature weights should be equal to the dimensionality of the PatternList.");
		Melder_information (100 * result, U" percent of the instances correctly classified.");   // BUG: never report a percentage; always report a fraction
	END
}

DIRECT (NEW_KNN_extractInputPatterns) {
	CONVERT_EACH (KNN)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		autoPatternList result = Data_copy (my input.get());
	CONVERT_EACH_END (my name, U"_input")
}

DIRECT (NEW_KNN_extractOutputCategories) {
	CONVERT_EACH (KNN)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		autoCategories result = Data_copy (my output.get());
	CONVERT_EACH_END (my name, U"_output");
}

FORM (MODIFY_KNN_reset, U"Reset", U"KNN: Reset...") {
    LABEL (U"Warning: this command destroys all previous learning.")
    OK
DO
	MODIFY_EACH (KNN)
		my input.reset();
		my output.reset();
		my nInstances = 0;
	MODIFY_EACH_END
}

DIRECT (MODIFY_KNN_shuffle) {
	MODIFY_EACH (KNN)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		KNN_shuffleInstances (me);
		praat_dataChanged (me);
	MODIFY_EACH_END
}

FORM (INFO_MODIFY_KNN_prune, U"Pruning", U"KNN: Prune...") {
	POSITIVE (noisePruningDegree, U"Noise pruning degree", U"1")
	POSITIVE (redundancyPruningDegree, U"Redundancy pruning degree", U"1")
	INTEGER (kNeighbours, U"k neighbours", U"1")
	OK
DO
	FIND_ONE (KNN)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		integer oldn = my nInstances;   // save before it changes!
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		if (noisePruningDegree <= 0.0 || noisePruningDegree > 1.0)
			Melder_throw (U"The noise pruning degree should be between 0.0 (excluded) and 1.0 (included).");
		if (redundancyPruningDegree <= 0.0 || redundancyPruningDegree > 1.0)
			Melder_throw (U"The redundancy pruning degree should be between 0.0 (excluded) and 1.0 (included).");
		integer npruned = KNN_prune_prune (me, noisePruningDegree, redundancyPruningDegree, kNeighbours);   // BUG: the KNN is changed
		Melder_information (npruned, U" instances discarded. \n", U"Size of new instance base: ", oldn - npruned);
	END
}

/////////////////////////////////////////////////////////////////////////////////////////
// Learning                                                                            //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (MODIFY_KNN_PatternList_Categories_learn, U"Learning", U"kNN classifiers 1. What is a kNN classifier?") {
	RADIOx (learningMethod, U"Learning method", 1, 1)
		RADIOBUTTON (U"Append new information")
		RADIOBUTTON (U"Replace current instance base")
	RADIOx (ordering, U"Ordering", 1, 1)
		RADIOBUTTON (U"Random")
		RADIOBUTTON (U"Sequential")
	OK
DO
	FIND_THREE (KNN, PatternList, Categories)
		switch (ordering) {
			case 1:
				ordering = kOla_SHUFFLE;
				break;
			case 2:
				ordering = kOla_SEQUENTIAL;
		}
		int result = kOla_ERROR;
		switch (learningMethod) {
			case 1:
				result = KNN_learn (me, you, him, my nInstances == 0 ? kOla_REPLACE : kOla_APPEND, ordering);
				break;
			case 2:
				result = KNN_learn (me, you, him, kOla_REPLACE, ordering);
				break;
		}
		switch (result) {
			case kOla_PATTERN_CATEGORIES_MISMATCH:  
				Melder_throw (U"The number of Categories should be equal to the number of rows in PatternList.");
			case kOla_DIMENSIONALITY_MISMATCH:
				Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
		}
	END
}

/////////////////////////////////////////////////////////////////////////////////////////
// Evaluation                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (BUG_KNN_evaluateWithTestSet, U"Evaluation", U"KNN & PatternList & Categories: Evaluate...") {
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	FIND_THREE (KNN, PatternList, Categories)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		if (your ny != his size)
			Melder_throw (U"The number of Categories should be equal to the number of rows in PatternList.");
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
		autoFeatureWeights fws = FeatureWeights_create (your nx);
		double result = KNN_evaluateWithTestSet (me, you, him, fws.get(), kNeighbours, voteWeighting);
		Melder_information (100 * result, U" percent of the instances correctly classified.");
	END
}

FORM (BUG_KNN_evaluateWithTestSetAndFeatureWeights, U"Evaluation", U"KNN & PatternList & Categories & FeatureWeights: Evaluate...") {
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIO (voteWeighting, U"Vote weighting", 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	FIND_FOUR (KNN, PatternList, Categories, FeatureWeights)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		Melder_require (your ny == his size,
			U"Your number of Categories (", your ny, U") should be equal to the number of rows in PatternList (", his size, U").");
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
		if (your nx != her fweights -> numberOfColumns)
			Melder_throw (U"The number of feature weights should be equal to the dimensionality of the PatternList.");
		double result = KNN_evaluateWithTestSet (me, you, him, she, kNeighbours, voteWeighting);
		Melder_information (100 * result, U" percent of the instances correctly classified.");
	END
}

/////////////////////////////////////////////////////////////////////////////////////////
// Classification                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW1_KNN_PatternList_to_Categories, U"Classification", U"KNN & PatternList: To Categories...") {
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	CONVERT_TWO (KNN, PatternList)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should match that of the instance base.");
		autoFeatureWeights fws = FeatureWeights_create (your nx);
		autoCategories result = KNN_classifyToCategories (me, you, fws.get(), kNeighbours, voteWeighting);
	CONVERT_TWO_END (my name, U"_", your name)
}

FORM (NEW1_KNN_PatternList_to_TableOfReal, U"Classification", U"KNN & PatternList: To TabelOfReal...") {
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	CONVERT_TWO (KNN, PatternList)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		autoFeatureWeights fws = FeatureWeights_create (your nx);
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should match that of the instance base.");
		autoTableOfReal result = KNN_classifyToTableOfReal (me, you, fws.get(), kNeighbours, voteWeighting);
	CONVERT_TWO_END (U"Output")
}

FORM (NEW1_KNN_PatternList_FeatureWeights_to_Categories, U"Classification", U"KNN & PatternList & FeatureWeights: To Categories...") {
	INTEGER (kNeighbours, U"k neighbours", U"KNN & PatternList & FeatureWeights: To Categories...")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	CONVERT_THREE (KNN, PatternList, FeatureWeights)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
		if (your nx != his fweights -> numberOfColumns)
			Melder_throw (U"The number of feature weights should be equal to the dimensionality of the PatternList.");
		autoCategories result = KNN_classifyToCategories (me, you, him, kNeighbours, voteWeighting);
	CONVERT_THREE_END (U"Output")
}

FORM (NEW1_KNN_PatternList_FeatureWeights_to_TableOfReal, U"Classification", U"KNN & PatternList & FeatureWeights: To TableOfReal...") {
	INTEGER (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 1, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	CONVERT_THREE (KNN, PatternList, FeatureWeights)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty.");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U"\n");
		if (your nx != his fweights -> numberOfColumns)
			Melder_throw (U"The number of features and the number of feature weights should be equal.");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		autoTableOfReal result = KNN_classifyToTableOfReal (me, you, him, kNeighbours, voteWeighting);
	CONVERT_THREE_END (U"Output")
}

/////////////////////////////////////////////////////////////////////////////////////////
// Clustering                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW_PatternList_to_Categories_cluster, U"k-means clustering", U"PatternList: To Categories...") {
	INTEGER (kClusters, U"k clusters", U"1")
	POSITIVE (clusterSizeRatioConstraint, U"Cluster size ratio constraint", U"1e-7");
	INTEGER (maximumNumberOfReseeds, U"Maximum number of reseeds", U"1000")
	OK
DO
	CONVERT_EACH (PatternList)
		if (my nx <= 0 || my ny <= 0)
			Melder_throw (U"PatternList is empty.");
		if (kClusters < 1 || kClusters > my ny)
			Melder_throw (U"Please select a value of k such that 0 < k <= ", my ny, U".");
		if (maximumNumberOfReseeds < 0)
			Melder_throw (U"The maximum number of reseeds should not be negative.");
		Melder_require (clusterSizeRatioConstraint > 0.0 && clusterSizeRatioConstraint <= 1.0,
			U"The cluster size ratio constraint should be between 0.0 (exclusive) and 1.0 (inclusive).");
		autoFeatureWeights fws = FeatureWeights_create (my nx);
		autoCategories result = PatternList_to_Categories_cluster (me, fws.get(), kClusters, clusterSizeRatioConstraint, maximumNumberOfReseeds);
	CONVERT_EACH_END (U"Output")
}

FORM (NEW1_PatternList_FeatureWeights_to_Categories_cluster, U"k-means clustering", U"PatternList & FeatureWeights: To Categories...") {
	INTEGER (kClusters, U"k clusters", U"1")
	POSITIVE (clusterSizeRatioConstraint, U"Cluster size ratio constraint", U"1e-7");
	INTEGER (maximumNumberOfReseeds, U"Maximum number of reseeds", U"1000")
	OK
DO
	CONVERT_TWO (PatternList, FeatureWeights)
		if (my nx <= 0 || my ny <= 0)
			Melder_throw (U"PatternList is empty.");
		if (my nx != your fweights -> numberOfColumns)
			Melder_throw (U"The number of features and the number of feature weights should be equal.");
		if (kClusters < 1 || kClusters > my ny)
			Melder_throw (U"Please select a value of k such that 0 < k <= ", my ny, U".");
		Melder_require (maximumNumberOfReseeds >= 0,
			U"The maximum number of reseeds should be 0 or positive.");
		Melder_require (clusterSizeRatioConstraint > 0.0 && clusterSizeRatioConstraint <= 1.0,
			U"The cluster size ratio constraint should be between 0.0 (exclusive) and 1.0 (inclusive).");
		autoCategories result = PatternList_to_Categories_cluster (me, you, kClusters, clusterSizeRatioConstraint, maximumNumberOfReseeds);
	CONVERT_TWO_END (U"Output")
}

/////////////////////////////////////////////////////////////////////////////////////////
// Dissimilarity computations                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (NEW_PatternList_to_Dissimilarity) {
	CONVERT_EACH (PatternList)
		autoFeatureWeights fws = FeatureWeights_create (my nx);
		autoDissimilarity result = KNN_patternToDissimilarity (me, fws.get());
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_PatternList_FeatureWeights_to_Dissimilarity) {
	CONVERT_TWO (PatternList, FeatureWeights)
		if (my nx != your fweights -> numberOfColumns)
			Melder_throw (U"The number of features and the number of feature weights should be equal.");
		autoDissimilarity result = KNN_patternToDissimilarity (me, you);
	CONVERT_TWO_END (U"Output")
}

/////////////////////////////////////////////////////////////////////////////////////////
// Computation of permutation                                                          //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW_KNN_to_Permutation_annealing, U"KNN: To Permutation", U"PatternList & Categories: To FeatureWeights...") {
	NATURAL (numberOfTriesPerStep, U"Number of tries per step", U"200")
	NATURAL (numberOfIterations, U"Number of iterations", U"10")
	POSITIVE (stepSize, U"Step size", U"10")
	POSITIVE (boltzmannConstant, U"Boltzmann constant", U"1.0")
	POSITIVE (initialTemperature, U"Initial temperature", U"0.002")
	POSITIVE (dampingFactor, U"Damping factor", U"1.005")
	POSITIVE (finalTemperature, U"Final temperature", U"0.000002")
	OK
DO
	CONVERT_EACH (KNN)
		autoPermutation result = KNN_SA_ToPermutation (me, numberOfTriesPerStep, numberOfIterations,
			stepSize, boltzmannConstant, initialTemperature, dampingFactor, finalTemperature);
	CONVERT_EACH_END (my name)
}

/////////////////////////////////////////////////////////////////////////////////////////
// Computation of feature weights                                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW1_PatternList_Categories_to_FeatureWeights_relief, U"Feature weights", U"PatternList & Categories: To FeatureWeights...") {
	INTEGER (numberOfNeighbours, U"Number of neighbours", U"1")
	OK
DO
	CONVERT_TWO (PatternList, Categories)
		if (my ny < 2)
			Melder_throw (U"The PatternList object should contain at least two rows.");
		if (my ny != your size)
			Melder_throw (U"The number of rows in the PatternList object should equal the number of categories in the Categories object.");
		autoFeatureWeights result = FeatureWeights_compute (me, you, numberOfNeighbours);
	CONVERT_TWO_END (U"Output")
}

FORM (NEW1_KNN_PatternList_Categories_to_FeatureWeights_wrapperExt, U"Feature weights", U"KNN & PatternList & Categories: To FeatureWeights..") {
	POSITIVE (learningRate, U"Learning rate", U"0.02")
	NATURAL (numberOfSeeds, U"Number of seeds", U"20")
	POSITIVE (stopAt, U"Stop at", U"1.0")
	RADIOx (optimization, U"Optimization", 1, 1)
		RADIOBUTTON (U"Co-optimization")
		RADIOBUTTON (U"Single feature")
	NATURAL (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 3, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	CONVERT_THREE (KNN, PatternList, Categories)
		if (my nInstances <= 0)
			Melder_throw (U"Instance base is empty");
		switch (voteWeighting) {
			case 1:
				voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
				break;
			case 2:
				voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
				break;
			case 3:
				voteWeighting = kOla_FLAT_VOTING;
				break;
		}
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		if (your nx != my input -> nx)
			Melder_throw (U"The dimensionality of PatternList should be equal to that of the instance base.");
		autoFeatureWeights result = FeatureWeights_computeWrapperExt (me, you, him, kNeighbours, voteWeighting, numberOfSeeds,
			learningRate, stopAt, optimization);
	CONVERT_THREE_END (U"Output")
}

FORM (NEW_KNN_to_FeatureWeights_wrapperInt, U"Feature weights", U"KNN: To FeatureWeights...") {
	POSITIVE (learningRate, U"Learning rate", U"0.02")
	NATURAL (numberOfSeeds, U"Number of seeds", U"10")
	POSITIVE (stopAt, U"Stop at", U"1.0")
	RADIOx (optimization, U"Optimization", 1, 1)
		RADIOBUTTON (U"Co-optimization")
		RADIOBUTTON (U"Single feature")
	RADIOx (evaluationMethod, U"Evaluation method", 1, 1)
		RADIOBUTTON (U"Leave one out")
		RADIOBUTTON (U"10-fold cross-validation")
	NATURAL (kNeighbours, U"k neighbours", U"1")
	RADIOx (voteWeighting, U"Vote weighting", 3, 1)
		RADIOBUTTON (U"Inverse squared distance")
		RADIOBUTTON (U"Inverse distance")
		RADIOBUTTON (U"Flat")
	OK
DO
	switch (evaluationMethod) {
		case 1:
			evaluationMethod = kOla_LEAVE_ONE_OUT;
			break;
		case 2:
			evaluationMethod = kOla_TEN_FOLD_CROSS_VALIDATION;
			break;
	}
	switch (voteWeighting) {
		case 1:
			voteWeighting = kOla_SQUARED_DISTANCE_WEIGHTED_VOTING;
			break;
		case 2:
			voteWeighting = kOla_DISTANCE_WEIGHTED_VOTING;
			break;
		case 3:
			voteWeighting = kOla_FLAT_VOTING;
			break;
	}
	CONVERT_EACH (KNN)
		if (my nInstances < 1)
			Melder_throw (U"Instance base is empty");
		if (kNeighbours < 1 || kNeighbours > my nInstances)
			Melder_throw (U"Please select a value of k such that 0 < k < ", my nInstances + 1, U".");
		autoFeatureWeights result = FeatureWeights_computeWrapperInt (me, kNeighbours, voteWeighting, numberOfSeeds, learningRate,
			stopAt, optimization, evaluationMethod);
	CONVERT_EACH_END (my name, U"_output")
}

/////////////////////////////////////////////////////////////////////////////////////////
// Creation and processing of auxiliary datatypes                                      //
/////////////////////////////////////////////////////////////////////////////////////////

FORM (NEW1_FeatureWeights_create, U"Create FeatureWeights", nullptr) {
	WORD (name, U"Name", U"empty")
	NATURAL (numberOfWeights, U"Number of weights", U"1")
	OK
DO
	CREATE_ONE
		autoFeatureWeights result = FeatureWeights_create (numberOfWeights);
	CREATE_ONE_END (name)
}

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG                                                                               //
/////////////////////////////////////////////////////////////////////////////////////////

// Disabled
/*
#ifdef _DEBUG

DIRECT (KNN_debug_KNN_SA_partition) {
    FIND_ONE (PatternList)
		autoPatternList output = PatternList_create (my ny, my nx);
		autoNUMvector <integer> result (0, my ny);
		KNN_SA_partition (me, 1, my ny, result);

		for (integer k = 1, c = 1; k <= output -> ny; k ++, c ++)
			for (integer i = 1; i <= my ny && k <= output -> ny; i ++)
				if (result [i] == c) {
					for(integer j = 1; j <= output -> nx; ++j)
						output -> z [k] [j] = my z [i] [j];
					k ++;
				}
		praat_new (output.move(), U"Output");
	END
}

DIRECT (KNN_debug_KNN_getNumberOfCPUs) {
    Melder_information (KNN_getNumberOfCPUs(), U" CPUs available");
END }

DIRECT (KNN_debug_KNN_threadTest) {
    KNN_threadTest();
END }

#endif
*/

/////////////////////////////////////////////////////////////////////////////////////////
// Help                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////

DIRECT (HELP_KNN_help) {
	HELP (U"KNN classifiers")
}

DIRECT (HINT_KNN_and_FeatureWeights_evaluate) {
	INFO_NONE
		Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN and a FeatureWeights object and choosing \"Evaluate...\".");
	INFO_NONE_END
}

DIRECT (HINT_KNN_and_Pattern_classify) {
	INFO_NONE
		Melder_information (U"You can use the KNN as a classifier by selecting a KNN and a PatternList and choosing \"To Categories...\" or \"To TableOfReal...\".");
	INFO_NONE_END
}

DIRECT (HINT_KNN_and_Pattern_and_FeatureWeights_classify) {
	INFO_NONE
		Melder_information (U"You can use the KNN as a classifier by selecting a KNN, a PatternList and an FeatureWeights object and choosing \"To Categories...\" or \"To TableOfReal...\".");
	INFO_NONE_END
}

DIRECT (HINT_KNN_and_Pattern_and_Categories_learn) {
	INFO_NONE
		Melder_information (U"You can train a KNN by selecting a KNN, a PatternList and a Categories object together and choosing \"Learn...\".");
	INFO_NONE_END
}

DIRECT (HINT_KNN_and_Pattern_and_Categories_evaluate) {
	INFO_NONE
		Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN, a test PatternList and the corresponding Categories object and choosing \"Evaluate...\".");
	INFO_NONE_END
}

DIRECT (HINT_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate) {
	INFO_NONE
		Melder_information (U"The accuracy of a KNN can be estimated by selecting a KNN, a test PatternList, an FeatureWeights object, and the corresponding Categories object and choosing \"Evaluate...\".");
	INFO_NONE_END
}

DIRECT (HINT_Pattern_and_FeatureWeights_to_Categories) {
	INFO_NONE
		Melder_information (U"A PatternList object and a FeatureWeights object can be used to compute a fixed number of clusters using the k-means clustering clustering algorithm.");
	INFO_NONE_END
}

DIRECT (HINT_Pattern_and_FeatureWeights_to_Dissimilarity) {
	INFO_NONE
		Melder_information (U"A Dissimilarity matrix can be generated from a PatternList and a FeatureWeights object.");
	INFO_NONE_END
}

/////////////////////////////////////////////////////////////////////////////////////////
// Setting callbacks                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////

void praat_contrib_Ola_KNN_init ();
void praat_contrib_Ola_KNN_init ()
{
    Thing_recognizeClassesByName (classKNN, classFeatureWeights, nullptr);

//////////
// Menu //
//////////

    praat_addMenuCommand (U"Objects", U"New", U"kNN classifiers", nullptr, 0, nullptr);

    praat_addMenuCommand (U"Objects", U"New", U"kNN classifiers", nullptr, praat_DEPTH_1, HELP_KNN_help);
    praat_addMenuCommand (U"Objects", U"New", U"-- KNN --", nullptr, 1, nullptr);

    praat_addMenuCommand (U"Objects", U"New", U"Create kNN classifier...", nullptr, 1, NEW1_KNN_create);

    praat_addMenuCommand (U"Objects", U"New", U"Advanced", nullptr, 1, nullptr);
    praat_addMenuCommand (U"Objects", U"New", U"Create PatternList...", nullptr, 2, NEW1_PatternList_create);
    praat_addMenuCommand (U"Objects", U"New", U"Create Categories...", nullptr, 2, NEW1_Categories_create);
    praat_addMenuCommand (U"Objects", U"New", U"Create FeatureWeights...", nullptr, 2, NEW1_FeatureWeights_create);

/////////////
// Actions //
/////////////

    praat_addAction1 (classKNN, 0, U"kNN help", nullptr, 0, HELP_KNN_help);
    praat_addAction1 (classKNN, 0, QUERY_BUTTON, nullptr, 0, nullptr);
    praat_addAction1 (classKNN, 1, U"Get optimized parameters...", nullptr, 2, INTEGER_KNN_getOptimumModel);
    praat_addAction1 (classKNN, 1, U"Get accuracy estimate...", nullptr, 2, REAL_KNN_evaluate);
    praat_addAction1 (classKNN, 1, U"Get size of instancebase", nullptr, 2, INTEGER_KNN_getNumberOfInstances);

    praat_addAction1 (classKNN, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
    praat_addAction1 (classKNN, 0, U"Shuffle", nullptr, 1, MODIFY_KNN_shuffle);
    praat_addAction1 (classKNN, 1, U"Prune...", nullptr, 1, INFO_MODIFY_KNN_prune);
    praat_addAction1 (classKNN, 0, U"Reset...", nullptr, 1, MODIFY_KNN_reset);
    praat_addAction1 (classKNN, 0, EXTRACT_BUTTON, nullptr, 0, nullptr);
    praat_addAction1 (classKNN, 0, U"Extract input Patterns", nullptr, 1, NEW_KNN_extractInputPatterns);
    praat_addAction1 (classKNN, 0, U"Extract output Categories", nullptr, 1, NEW_KNN_extractOutputCategories);

    praat_addAction1 (classKNN, 0, U"To FeatureWeights...", nullptr, 0, NEW_KNN_to_FeatureWeights_wrapperInt);

 // praat_addAction1 (classKNN, 0, U"To Permutation (annealing)...", nullptr, 0, NEW_KNN_to_Permutation_annealing);
 // praat_addAction2 (classKNN, 1, classFeatureWeights, 1, U"To Permutation...", nullptr, 0, NEW1_KNN_FeatureWeights_to_Permutation);

    praat_addAction3 (classKNN, 1, classPatternList, 1, classCategories, 1, U"Learn...", nullptr, 0, MODIFY_KNN_PatternList_Categories_learn);
    praat_addAction2 (classKNN, 1, classFeatureWeights, 1, U"Evaluate...", nullptr, 0, REAL_KNN_FeatureWeights_evaluate);
    praat_addAction3 (classKNN, 1, classPatternList, 1, classCategories, 1, U"Evaluate...", nullptr, 0, BUG_KNN_evaluateWithTestSet);
    praat_addAction4 (classKNN, 1, classPatternList, 1, classCategories, 1, classFeatureWeights, 1, U"Evaluate...", nullptr, 0, BUG_KNN_evaluateWithTestSetAndFeatureWeights);
    praat_addAction3 (classKNN, 1, classPatternList, 1, classCategories, 1, U"To FeatureWeights...", nullptr, 0,
		NEW1_KNN_PatternList_Categories_to_FeatureWeights_wrapperExt);
    praat_addAction2 (classKNN, 1, classPatternList, 1, U"To Categories...", nullptr, 0, NEW1_KNN_PatternList_to_Categories);
    praat_addAction2 (classKNN, 1, classPatternList, 1, U"To TableOfReal...", nullptr, 0, NEW1_KNN_PatternList_to_TableOfReal);

    praat_addAction3 (classKNN, 1, classPatternList, 1, classFeatureWeights, 1, U"To Categories...", nullptr, 0,
		NEW1_KNN_PatternList_FeatureWeights_to_Categories);
    praat_addAction3 (classKNN, 1, classPatternList, 1, classFeatureWeights, 1, U"To TableOfReal...", nullptr, 0,
		NEW1_KNN_PatternList_FeatureWeights_to_TableOfReal);

    praat_addAction1 (classPatternList, 0, U"To Dissimilarity", nullptr, 1, NEW_PatternList_to_Dissimilarity);
    praat_addAction1 (classPatternList, 0, U"To Categories (cluster)...", nullptr, 1, NEW_PatternList_to_Categories_cluster);
    praat_addAction2 (classPatternList, 1, classFeatureWeights, 1, U"To Dissimilarity", nullptr, 0, NEW1_PatternList_FeatureWeights_to_Dissimilarity);
    praat_addAction2 (classPatternList, 1, classFeatureWeights, 1, U"To Categories (cluster)...", nullptr, 0, NEW1_PatternList_FeatureWeights_to_Categories_cluster);

    praat_addAction2 (classPatternList, 1, classCategories, 1, U"To FeatureWeights (relief)...", nullptr, 0, NEW1_PatternList_Categories_to_FeatureWeights_relief);
    praat_addAction2 (classPatternList, 1, classCategories, 1, U"To KNN Classifier...", nullptr, 0, NEW1_PatternList_Categories_to_KNN);

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

    praat_addAction1 (classPatternList, 0, U"& FeatureWeights: To Categories?", nullptr, 0, HINT_Pattern_and_FeatureWeights_to_Categories);
    praat_addAction1 (classPatternList, 0, U"& FeatureWeights: To Dissimilarity?", nullptr, 0, HINT_Pattern_and_FeatureWeights_to_Dissimilarity);

    praat_addAction1 (classKNN, 0, U"& FeatureWeights: Evaluate?", nullptr, 0, HINT_KNN_and_FeatureWeights_evaluate);
//  praat_addAction1 (classKNN, 0, U"& FeatureWeights: To Permutation?", nullptr, 0, HINT_Pattern_and_FeatureWeights_to_Dissimilarity);
    praat_addAction1 (classKNN, 0, U"& Pattern: Classify?", nullptr, 0, HINT_KNN_and_Pattern_classify);
    praat_addAction1 (classKNN, 0, U"& Pattern & FeatureWeights: Classify?", nullptr, 0, HINT_KNN_and_Pattern_and_FeatureWeights_classify);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories: Learn?", nullptr, 0, HINT_KNN_and_Pattern_and_Categories_learn);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories: Evaluate?", nullptr, 0, HINT_KNN_and_Pattern_and_Categories_evaluate);
    praat_addAction1 (classKNN, 0, U"& Pattern & Categories & FeatureWeights: Evaluate?", nullptr, 0, HINT_KNN_and_Pattern_and_Categories_and_FeatureWeights_evaluate);

    INCLUDE_MANPAGES (manual_KNN_init)
}

/* End of file praat_contrib_Ola_KNN.cpp */
