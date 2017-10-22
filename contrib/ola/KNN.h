#ifndef _KNN_h_
#define _KNN_h_
/* KNN.h
 *
 * Copyright (C) 2007-2008 Ola Söder, 2017 Paul Boersma
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
 * os 20080529 Initial release
 * pb 2011/03/08 C++
 */

/////////////////////////////////////////////////////
// Praat datatypes                                 //
/////////////////////////////////////////////////////

#include "Data.h"
#include "PatternList.h"
#include "Categories.h"
#include "TableOfReal.h"
#include "Permutation.h"
#include "MDS.h"

/////////////////////////////////////////////////////
// KNN miscs                                       //
/////////////////////////////////////////////////////

#include "OlaP.h"
#include "FeatureWeights.h"
#include "gsl_siman.h"

/////////////////////////////////////////////////////
// Praat specifics                                 //
/////////////////////////////////////////////////////

#include "KNN_def.h"

/////////////////////////////////////////////////////
// Private definitions and macros                  //
/////////////////////////////////////////////////////

#define kOla_TEN_FOLD_CROSS_VALIDATION 1
#define kOla_LEAVE_ONE_OUT 2

#define kOla_SQUARED_DISTANCE_WEIGHTED_VOTING 8
#define kOla_DISTANCE_WEIGHTED_VOTING 16
#define kOla_FLAT_VOTING 32

#define kOla_ERROR 0
#define kOla_SUCCESS 303

#define kOla_APPEND 1
#define kOla_REPLACE 2

#define kOla_SHUFFLE 1
#define kOla_SEQUENTIAL 2

#define kOla_PATTERN_CATEGORIES_MISMATCH 111
#define kOla_DIMENSIONALITY_MISMATCH 222
#define kOla_FWEIGHTS_MISMATCH 333


/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////

// a near-dummy function
autoKNN KNN_create ();

// Learning
int KNN_learn
(
    KNN me,             // the classifier to be trained
    PatternList p,      // source pattern
    Categories c,       // target categories
    int method,         // method <- REPLACE or APPEND
    int ordering        // ordering <- SHUFFLE?
);

// Classification - To Categories
autoCategories KNN_classifyToCategories
(
    KNN me,             // the classifier being used
    PatternList ps,     // target pattern (where neighbours are sought for)
    FeatureWeights fws, // feature weights
    integer k,          // the number of sought after neighbours
    int dist            // distance weighting
);

// Classification - To Categories, threading aux
void * KNN_classifyToCategoriesAux
(
    void * input
);

// Classification - To TableOfReal
autoTableOfReal KNN_classifyToTableOfReal
(
    KNN me, PatternList ps, FeatureWeights fws, integer k, int dist
);

// Classification - To TableOfReal, threading aux
void * KNN_classifyToTableOfRealAux
(
    void * input
);

// Classification - To TableOfReal, all candidates
autoTableOfReal KNN_classifyToTableOfRealAll
(
    KNN me,             // the classifier being used
    PatternList ps,     // target pattern (where neighbours are sought for)
    FeatureWeights fws, // feature weights
    integer k,          // the number of sought after neighbours
    int dist            // distance weighting
);

// Classification - Folding
autoCategories KNN_classifyFold
(
    KNN me,             // the classifier being used
    PatternList ps,     // target pattern (where neighbours are sought for)
    FeatureWeights fws, // feature weights
    integer k,          // the number of sought after neighbours
    int dist,           // distance weighting
    integer begin,      // fold start, inclusive [...
    integer end        // fold end, inclusive ...]
);

// Evaluation
double KNN_evaluate
(
    KNN me,             // the classifier being used
    FeatureWeights fws, // feature weights
    integer k,          // the number of sought after neighbours
    int dist,           // distance weighting
    int mode            // TEN_FOLD_CROSS_VALIDATION / LEAVE_ONE_OUT
);

// Evaluation using a separate test set
double KNN_evaluateWithTestSet
(
    KNN me,             // the classifier being used
    PatternList p,      // the vectors of the test set
    Categories c,       // the categories of the test set
    FeatureWeights fws, // feature weights
    integer k,          // the number of sought after neighbours
    int dist            // distance weighting
);

// Model search
double KNN_modelSearch
(
    KNN me,             // the classifier being used
    FeatureWeights fws, // feature weights
    integer *k,         // valid integer *, to hold the output value of k
    int *dist,          // valid int *, to hold the output value dist_weight
    int mode,           // evaluation mode
    double rate,        // learning rate
    integer nseeds      // the number of seeds to be used
);

// Euclidean distance
double KNN_distanceEuclidean
(
    PatternList ps, PatternList pt, FeatureWeights fws, integer rows, integer rowt
);

// Manhattan distance
double KNN_distanceManhattan
(
    PatternList ps, PatternList pt, integer rows, integer rowt
);

// Find longest distance
integer KNN_max
(
    double * distances, // an array of distances containing ...
    integer ndistances     // ndistances distances
);

// Locate k neighbours, skip one + disposal of distance
integer KNN_kNeighboursSkip
(
    PatternList j, PatternList p, FeatureWeights fws, integer jy, integer k, integer *indices, integer skipper
);

// Locate the k nearest neighbours, exclude instances within the range defined
// by [begin ... end]
integer KNN_kNeighboursSkipRange
(
    PatternList j, PatternList p, FeatureWeights fws, integer jy, integer k, integer *indices, double *distances, integer begin, integer end
                        // range of excluded instances in the target
                        // pattern
);

// Locate k neighbours
integer KNN_kNeighbours
(
    PatternList j,      // source-pattern (where the unknown is located)
    PatternList p,      // target pattern (where neighbours are sought for)
    FeatureWeights fws, // feature weights
    integer jy,         // the index of the unknown instance in the source pattern
    integer k,          // the number of sought after neighbours
    integer * indices,  // a pointer to a memory-space big enough for k integers
                        // representing indices to the k neighbours in the
                        // target pattern
    double * distances  // a pointer to a memory-space big enough for k
                        // doubles representing the distances to the k
                        // neighbours
);

// Locating k (nearest) friends
integer KNN_kFriends
(
    PatternList j, PatternList p, Categories c, integer jy, integer k, integer *indices
                        // representing indices to the k friends in the
                        // target pattern
);

// Computing the distance to the nearest enemy
double KNN_nearestEnemy
(
    PatternList j, PatternList p, Categories c, integer jy
);

// Computing the number of friends among k neighbours
integer KNN_friendsAmongkNeighbours
(
    PatternList j, PatternList p, Categories c, integer jy, integer k
);

// Locating k unique (nearest) enemies
integer KNN_kUniqueEnemies
(
    PatternList j, PatternList p, Categories c, integer jy, integer k, integer *indices
                        // located enemies
);

// Compute dissimilarity matrix
autoDissimilarity KNN_patternToDissimilarity
(
    PatternList p,       // PatternList
    FeatureWeights fws   // Feature weights
);

// Compute frequencies
integer KNN_kIndicesToFrequenciesAndDistances
(
    Categories c,        // Source categories
    integer k,           // k (!)
    integer *indices,    // In: indices
    double *distances,   // Out: distances
    double *freqs,       // Out: and frequencies (double, sic!)
    integer *freqindices // Out: and indices -> freqs.
);

// Normalize array
void KNN_normalizeFloatArray
(
    double * array,     // Array to be normalized
    integer n           // The number of elements
                        // in the array
);

// Remove instance
void KNN_removeInstance
(
    KNN me,             // Classifier
    integer y           // Index of the instance to be purged
                        //
);

// Shuffle instances
void KNN_shuffleInstances
(
    KNN me              // Classifier whose instance
                        // base is to be shuffled
);

// Experimental code
autoPermutation KNN_SA_ToPermutation
(
    KNN me,             // the classifier being used
    integer tries,      //
    integer iterations, //
    double step_size,   //
    double boltzmann_c, //
    double temp_start,  //
    double damping_f,   //
    double temp_stop    //
                        //
);

// Experimental code
typedef struct
{
    PatternList p;
    integer *indices;
} KNN_SA_t;
    
// Experimental code
double KNN_SA_t_energy
(
    void * istruct
);

// Experimental code
double KNN_SA_t_metric
(
    void * istruct1,
    void * istruct2
);

// Experimental code
void KNN_SA_t_print
(
    void * istruct
);

// Experimental code
void KNN_SA_t_step
(
    const gsl_rng * r,
    void * istruct,
    double step_size
);

// Experimental code
void KNN_SA_t_copy
(
    void * istruct_src,
    void * istruct_dest
);

// Experimental code
void * KNN_SA_t_copy_construct
(
    void * istruct
);

// Experimental code
KNN_SA_t * KNN_SA_t_create
(
    PatternList p
);

// Experimental code
void KNN_SA_t_destroy
(
    void * istruct
);

// Experimental code
void KNN_SA_partition
(
    PatternList p,               
    integer i1,
    integer i2,
    integer *result
);

// Compute feature weights (wrapper), evaluate using folding
autoFeatureWeights FeatureWeights_computeWrapperInt
(
    KNN me,                 // Classifier
    integer k,              // k(!)
    int d,                  // distance weighting
    integer nseeds,         // the number of seeds
    double alfa,            // shrinkage factor
    double stop,            // stop at
    int mode,               // mode (co/serial)
    int emode               // evaluation mode (10-fold/L1O)
);

// Compute feature weights (wrapper), evaluate using separate test set
autoFeatureWeights FeatureWeights_computeWrapperExt
(
    KNN nn,                 // Classifier
    PatternList pp,         // test pattern
    Categories c,           // test categories
    integer k,              // k(!)
    int d,                  // distance weighting
    integer nseeds,         // the number of seeds
    double alfa,            // shrinkage factor
    double stop,            // stop at
    int mode                // mode (co/serial)
);

// Evaluate feature weights, wrapper aux.
double FeatureWeights_evaluate
(
    FeatureWeights fws,     // Weights to evaluate
    KNN nn,                 // Classifier
    PatternList pp,         // test pattern
    Categories c,           // test categories
    integer k,              // k(!)
    int d                   // distance weighting
);

/* End of file KNN.h */
#endif
