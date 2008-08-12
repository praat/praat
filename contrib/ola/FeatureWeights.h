#ifndef _FeatureWeights_h_
#define _FeatureWeights_h_

/* FeatureWeights.h
 *
 * Copyright (C) 2007-2008 Ola Söder
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

/* $URL: svn://pegasos.dyndns.biz/praat/trunk/kNN/FeatureWeights.h $
 * $Rev: 137 $
 * $Author: stix $
 * $Date: 2008-08-10 19:34:07 +0200 (Sun, 10 Aug 2008) $
 * $Id: FeatureWeights.h 137 2008-08-10 17:34:07Z stix $
 */

/*
 * os 20080529 Initial release
 */

/////////////////////////////////////////////////////
// Praat datatypes                                 //
/////////////////////////////////////////////////////

#include "Data.h"
#include "TableOfReal.h"
#include "Pattern.h"
#include "Categories.h"

/////////////////////////////////////////////////////
// Praat specifics                                 //
/////////////////////////////////////////////////////

#include "FeatureWeights_def.h"
#define FeatureWeights_methods Data_methods
oo_CLASS_CREATE (FeatureWeights, Data);

/////////////////////////////////////////////////////
// Miscs                                           //
/////////////////////////////////////////////////////

#include "KNN.h"
#include "OlaP.h"

/////////////////////////////////////////////////////
// Private definitions and macros                  //
/////////////////////////////////////////////////////

#define FRIENDS(x,y) !SimpleString_compare(x,y)
#define ENEMIES(x,y) SimpleString_compare(x,y)

/////////////////////////////////////////////////////
// Prototypes                                      //
/////////////////////////////////////////////////////


// Create
FeatureWeights FeatureWeights_create
(
    long nweights           // number of weights
);

// Compute prior probabilities
long FeatureWeights_computePriors
(
    Categories c,           // source categories
    long * indices,         // Out: instances indices ..
    double * priors         // Out: .. and their prior probabilities
);

// Compute feature weights (obsolete)
FeatureWeights FeatureWeights_compute
(
    Pattern pp,             // Source pattern
    Categories c,           // Source categories
    long k                  // k(!)
);

// Compute feature weights (wrapper), evaluate using folding
FeatureWeights FeatureWeights_computeWrapperInt
(
    KNN me,                 // Classifier
    long k,                 // k(!)
    int d,                  // distance weighting
    long nseeds,            // the number of seeds
    double alfa,            // shrinkage factor
    double stop,            // stop at
    int mode,               // mode (co/serial)
    int emode               // evaluation mode (10-fold/L1O)
);

// Compute feature weights (wrapper), evaluate using separate test set
FeatureWeights FeatureWeights_computeWrapperExt
(
    KNN nn,                 // Classifier
    Pattern pp,             // test pattern
    Categories c,           // test categories
    long k,                 // k(!)
    int d,                  // distance weighting
    long nseeds,            // the number of seeds
    double alfa,            // shrinkage factor
    double stop,            // stop at
    int mode                // mode (co/serial)
);

// Evaluate feature weights, wrapper aux.
double FeatureWeights_evaluate
(
    FeatureWeights fws,     // Weights to evaluate
    KNN nn,                 // Classifier
    Pattern pp,             // test pattern
    Categories c,           // test categories
    long k,                 // k(!)
    int d                   // distance weighting
);

// Compute feature weights according to the RELIEF-F algorithm
FeatureWeights FeatureWeights_computeRELIEF
(
    Pattern pp,             // source pattern
    Categories c,           // source categories
    long k                  // k(!)
);

#endif /* _FeatureWeights_h_ */
