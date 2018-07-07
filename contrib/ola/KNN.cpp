/* KNN.cpp
 *
 * Copyright (C) 2008 Ola So"der, 2010-2012,2016-2018 Paul Boersma
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
 * os 2008/05/29 initial release
 * pb 2010/06/06 removed some array-creations-on-the-stack
 * pb 2011/04/12 C++
 * pb 2011/04/13 removed several memory leaks
 * pb 2011/07/07 some exception safety
 */

#include "KNN.h"
#include "KNN_threads.h"
#include "OlaP.h"

#include "oo_DESTROY.h"
#include "KNN_def.h"
#include "oo_COPY.h"
#include "KNN_def.h"
#include "oo_EQUAL.h"
#include "KNN_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "KNN_def.h"
#include "oo_WRITE_TEXT.h"
#include "KNN_def.h"
#include "oo_WRITE_BINARY.h"
#include "KNN_def.h"
#include "oo_READ_TEXT.h"
#include "KNN_def.h"
#include "oo_READ_BINARY.h"
#include "KNN_def.h"
#include "oo_DESCRIPTION.h"
#include "KNN_def.h"


Thing_implement (KNN, Daata, 0);

/////////////////////////////////////////////////////////////////////////////////////////////
// Praat specifics                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

void structKNN :: v_info ()
{
    structDaata :: v_info ();
    MelderInfo_writeLine (U"Size of instancebase: ", nInstances);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Creation                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

autoKNN KNN_create ()
{
	try {
		autoKNN me = Thing_new (KNN);
        my nInstances = 0;
        return me;
    } catch (MelderError) {
		Melder_throw (U"KNN classifier not created.");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Learning                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

int KNN_learn
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,         // the classifier to be trained
                    //
    PatternList p,      // source pattern
                    //
    Categories c,   // target categories
                    //
    int method,     // method <- REPLACE or APPEND
                    //
    int ordering    // ordering <- SHUFFLE?
)

{
    if (c->size == p->ny)           // the number of input vectors must
    {                               // equal the number of categories.
        switch (method)
        {
        case kOla_REPLACE:          // in REPLACE mode simply
                                    // dispose of the current
            my input = Data_copy (p);   // LEAK
            my output = Data_copy (c);
            my nInstances = c->size;

            break;

        case kOla_APPEND:                   // in APPEND mode a new
                                            // instance base is formed
                                            // by merging the new and
                                            // the old.
                                            //
            if (p->nx == (my input)->nx)    // the number of features
                                            // of the old and new
                                            // instance base must
                                            // match; otherwise merging
                                            // won't be possible.
            {
				/*
				 * Create without change.
				 */
				autoMatrix matrix = Matrix_appendRows (my input.get(), p, classPatternList);
                autoPatternList tinput = matrix.static_cast_move <structPatternList>();
				autoCategories toutput = Data_copy (my output.get());
				toutput -> merge (c);

				/*
				 * Change without error.
				 */
                my input = tinput.move();
                my output = toutput.move();
                my nInstances += p->ny;
            }
            else                                    // fail
            {
                return kOla_DIMENSIONALITY_MISMATCH;
            }
            break;
        }
        if (ordering == kOla_SHUFFLE)               // shuffle the instance base
        {
            KNN_shuffleInstances(me);
        }
    }
    else                                            // fail
    {
        return kOla_PATTERN_CATEGORIES_MISMATCH;
    }

    return kOla_SUCCESS;                            // success

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Classification - To Categories                                                          //
/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
        KNN me;
        PatternList ps;
        integer * output;
        FeatureWeights fws;
        integer k;
        int dist;
        integer istart;
        integer istop;

} KNN_input_ToCategories_t;

autoCategories KNN_classifyToCategories
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    PatternList ps,     // the pattern to classify
                        //
    FeatureWeights fws, // feature weights
                        //
    integer k,          // the number of sought after neighbours
                        //
    int dist            // distance weighting
)

{
    int nthreads = KNN_getNumberOfCPUs();
    integer *outputindices = NUMvector <integer> (0, ps->ny);
    integer chunksize =  ps->ny / nthreads;

    Melder_assert(nthreads > 0); 
    Melder_assert(k > 0 && k <= my nInstances);

    if(chunksize < 1)
    {
        chunksize = 1;
        nthreads = ps->ny;
    }

    integer istart = 1;
    integer istop = chunksize;

    autoCategories output = Categories_create ();
    KNN_input_ToCategories_t ** input = (KNN_input_ToCategories_t **) malloc(nthreads * sizeof(KNN_input_ToCategories_t *));

    if (!input)
        return autoCategories();

    for (int i = 0; i < nthreads; i ++)
    {
        input [i] = (KNN_input_ToCategories_t *) malloc(sizeof(KNN_input_ToCategories_t));
        if (! input [i])
        {
            while (input [i --])
                free (input [i]);

            free (input);
            return autoCategories();
        }
    }

    for (int i = 0; i < nthreads; i ++)
    {  
        input[i]->me = me;
        input[i]->ps = ps;
        input[i]->output = outputindices;
        input[i]->fws = fws;
        input[i]->k = k;
        input[i]->dist = dist;
        input[i]->istart = istart;

        if (istop + chunksize > ps->ny)
        {  
            input[i]->istop = ps->ny; 
            break;
        }
        else
        {  
            input[i]->istop = istop;
            istart = istop + 1;
            istop += chunksize;
        }
    }
 
    enum KNN_thread_status * error = (enum KNN_thread_status *) KNN_threadDistribution(KNN_classifyToCategoriesAux, (void **) input, nthreads);
    //void *error = KNN_classifyToCategoriesAux (input [0]);
    for (int i = 0; i < nthreads; i ++)
        free (input [i]);
  
    free (input);
    if (error)           // Something went very wrong, you ought to inform the user!
    {
        free (error);
        return autoCategories();
    }

	for (integer i = 1; i <= ps -> ny; i ++) {
		output -> addItem_move (Data_copy (my output->at [outputindices [i]]));
	}
	NUMvector_free (outputindices, 0);
    return output;
}

void * KNN_classifyToCategoriesAux
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    void * input
   
)

{
    Melder_assert(((KNN_input_ToCategories_t *) input)->istart > 0 &&  
                  ((KNN_input_ToCategories_t *) input)->istop > 0 &&
                  ((KNN_input_ToCategories_t *) input)->istart <= ((KNN_input_ToCategories_t *) input)->ps->ny &&
                  ((KNN_input_ToCategories_t *) input)->istop <= ((KNN_input_ToCategories_t *) input)->ps->ny &&
                  ((KNN_input_ToCategories_t *) input)->istart <= ((KNN_input_ToCategories_t *) input)->istop);
    
    integer ncollected;
    integer ncategories;

    integer *indices = NUMvector <integer> (0, ((KNN_input_ToCategories_t *) input)->k);
    integer *freqindices = NUMvector <integer> (0, ((KNN_input_ToCategories_t *) input)->k);

    double *distances = NUMvector <double> (0, ((KNN_input_ToCategories_t *) input)->k);
    double *freqs = NUMvector <double> (0, ((KNN_input_ToCategories_t *) input)->k);
 
    for (integer y = ((KNN_input_ToCategories_t *) input)->istart; y <= ((KNN_input_ToCategories_t *) input)->istop; ++y)
    {
        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighbours
        (
            ((KNN_input_ToCategories_t *) input)->ps, 
            ((KNN_input_ToCategories_t *) input)->me->input.get(),
            ((KNN_input_ToCategories_t *) input)->fws, y, 
            ((KNN_input_ToCategories_t *) input)->k, indices, distances
        );

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances
        (
            ((KNN_input_ToCategories_t *) input)->me->output.get(),
            ((KNN_input_ToCategories_t *) input)->k, 
            indices, distances, freqs, freqindices
        );

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        switch(((KNN_input_ToCategories_t *) input)->dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (integer c = 0; c < ncategories; ++c)
                    freqs[c] *= 1 / OlaMAX(distances[c], kOla_MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (integer c = 0; c < ncategories; ++c)
                    freqs[c] *= 1 / OlaMAX(OlaSQUARE(distances[c]), kOla_MINFLOAT);
        }

        KNN_normalizeFloatArray(freqs, ncategories);
        ((KNN_input_ToCategories_t *) input)->output[y] = freqindices[KNN_max(freqs, ncategories)];
    }

	NUMvector_free (indices, 0);
	NUMvector_free (freqindices, 0);
	NUMvector_free (distances, 0);
	NUMvector_free (freqs, 0);
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Classification - To TableOfReal                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	KNN me;
	PatternList ps;
	Categories uniqueCategories;
	TableOfReal output;
	FeatureWeights fws;
	integer k;
	int dist;
	integer istart;
	integer istop;
} KNN_input_ToTableOfReal_t;

autoTableOfReal KNN_classifyToTableOfReal
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    PatternList ps,     // source PatternList
                        //
    FeatureWeights fws, // feature weights
                        //
    integer k,          // the number of sought after neighbours
                        //
    int dist            // distance weighting
)

{
    int nthreads = KNN_getNumberOfCPUs();
    integer chunksize =  ps->ny / nthreads;
    autoCategories uniqueCategories = Categories_selectUniqueItems (my output.get());
    integer ncategories = uniqueCategories->size;
   
    Melder_assert (nthreads > 0);
    Melder_assert (ncategories > 0);
    Melder_assert (k > 0 && k <= my nInstances);
 
    if (! ncategories)
        return autoTableOfReal();

    if (chunksize < 1)
    {
        chunksize = 1;
        nthreads = ps->ny;
    }

    integer istart = 1;
    integer istop = chunksize;

    KNN_input_ToTableOfReal_t ** input = (KNN_input_ToTableOfReal_t **) malloc (nthreads * sizeof (KNN_input_ToTableOfReal_t *));
    
    if(! input)
        return autoTableOfReal();

    autoTableOfReal output = TableOfReal_create(ps->ny, ncategories);

    for (integer i = 1; i <= ncategories; i ++)
        TableOfReal_setColumnLabel (output.get(), i, uniqueCategories->at [i] -> string.get());

    for (int i = 0; i < nthreads; i ++)
    {
        input [i] = (KNN_input_ToTableOfReal_t *) malloc (sizeof (KNN_input_ToTableOfReal_t));
        if (! input [i])
        {
            while (input [i --])   // ppgb FIXME: is always false
                free (input [i]);   // ppgb FIXME: the stopping condition is incorrect

            free (input);
            return autoTableOfReal();
        }
    }

    for (int i = 0; i < nthreads; i ++)
    {  
        input[i]->me = me;
        input[i]->ps = ps;
        input[i]->output = output.get();   // YUCK: reference copy
        input[i]->uniqueCategories = uniqueCategories.releaseToAmbiguousOwner();
        input[i]->fws = fws;
        input[i]->k = k;
        input[i]->dist = dist;
        input[i]->istart = istart;

        if(istop + chunksize > ps->ny)
        {  
            input[i]->istop = ps->ny; 
            break;
        }
        else
        {  
            input[i]->istop = istop;
            istart = istop + 1;
            istop += chunksize;
        }
    }
 
    enum KNN_thread_status * error = (enum KNN_thread_status *) KNN_threadDistribution (KNN_classifyToTableOfRealAux, (void **) input, nthreads);
    for (int i = 0; i < nthreads; i ++)
        free (input [i]);
  
    free (input);
    if (error)           // Something went very wrong, you ought to inform the user!
    {
        free (error);
        return autoTableOfReal();
    }
    return output;
}


void * KNN_classifyToTableOfRealAux
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    void * void_input

)

{
	KNN_input_ToTableOfReal_t *input = (KNN_input_ToTableOfReal_t *) void_input;
    integer ncategories = input -> uniqueCategories->size;
    autoNUMvector <integer> indices ((integer) 0, input -> k);
    autoNUMvector <double> distances ((integer) 0, input -> k);

	for (integer y = input -> istart; y <= input -> istop; y ++) {
		KNN_kNeighbours (input -> ps, input -> me -> input.get(), input -> fws, y, input -> k, indices.peek(), distances.peek());
		for (integer i = 0; i < input -> k; i ++) {
			for (integer j = 1; j <= ncategories; j ++) {
				if (FeatureWeights_areFriends (input -> me -> output->at [indices [i]], input -> uniqueCategories->at [j])){
					input -> output -> data [y] [j] += 1.0;
				}
			}
		}
	}
	switch (input -> dist) {
		case kOla_DISTANCE_WEIGHTED_VOTING:
			for (integer y = input -> istart; y <= input -> istop; y ++) {
				longdouble sum = 0.0;
				for (integer c = 1; c <= ncategories; c ++) {
					input -> output -> data [y] [c] *= 1.0 / OlaMAX (distances [c], kOla_MINFLOAT);
					sum += input -> output -> data [y] [c];
				}
				for (integer c = 1; c <= ncategories; c ++) {
					input -> output -> data [y] [c] /= sum;
				}
			}
			break;
		case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
			for (integer y = input -> istart; y <= input -> istop; y ++) {
				longdouble sum = 0.0;
				for (integer c = 1; c <= ncategories; c ++) {
					input -> output -> data [y] [c] *= 1.0 / OlaMAX (OlaSQUARE (distances [c]), kOla_MINFLOAT);
					sum += input -> output -> data [y] [c];
				}
				for (integer c = 1; c <= ncategories; c ++) {
					input -> output -> data [y] [c] /= sum;
				}
			}
			break;
		case kOla_FLAT_VOTING:
			for (integer y = input -> istart; y <= input -> istop; y ++) {
				longdouble sum = 0.0;
				for (integer c = 1; c <= ncategories; c ++) {
					sum += input -> output -> data [y] [c];
				}
				for (integer c = 1; c <= ncategories; c ++) {
					input -> output -> data [y] [c] /= sum;
				}
			}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Classification - Folding                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

autoCategories KNN_classifyFold
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    PatternList ps,     // source PatternList
                        //
    FeatureWeights fws, // feature weights
                        //
    integer k,          // the number of sought after neighbours
                        //
    int dist,           // distance weighting
                        //
    integer begin,      // fold start, inclusive [...
                        //
    integer end         // fold end, inclusive ...]
                        //
)

{
    Melder_assert(k > 0 && k <= ps->ny);
    Melder_assert(end > 0 && end <= ps->ny);
    Melder_assert(begin > 0 && begin <= ps->ny);

    if (begin > end) 
        OlaSWAP(integer, begin, end);

    if (k > my nInstances - (end - begin))
        k = my nInstances - (end - begin);

    integer ncollected;
    integer ncategories;
    autoNUMvector <integer> indices ((integer) 0, k);
    autoNUMvector <integer> freqindices ((integer) 0, k);
    autoNUMvector <double> distances ((integer) 0, k);
    autoNUMvector <double> freqs ((integer) 0, k);
    autoNUMvector <integer> outputindices ((integer) 0, ps->ny);
    integer noutputindices = 0;

    for (integer y = begin; y <= end; ++y)
    {
        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighboursSkipRange (ps, my input.get(), fws, y, k, indices.peek(), distances.peek(), begin, end);

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances (my output.get(), k, indices.peek(), distances.peek(), freqs.peek(), freqindices.peek());

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        switch (dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (integer c = 0; c < ncategories; c ++)
                    freqs [c] *= 1.0 / OlaMAX (distances [c], kOla_MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (integer c = 0; c < ncategories; c ++)
                    freqs [c] *= 1.0 / OlaMAX (OlaSQUARE (distances [c]), kOla_MINFLOAT);
        }

        KNN_normalizeFloatArray (freqs.peek(), ncategories);
        outputindices [noutputindices++] = freqindices [KNN_max (freqs.peek(), ncategories)];
    }

	autoCategories output = Categories_create ();
	for (integer o = 0; o < noutputindices; o ++) {
		output -> addItem_move (Data_copy (my output->at [outputindices [o]]));
	}
	return output;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Evaluation                                                                              //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_evaluate
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    FeatureWeights fws, // feature weights
                        //
    integer k,          // the number of sought after neighbours
                        //
    int dist,           // distance weighting
                        //
    int mode            // kOla_TEN_FOLD_CROSS_VALIDATION / kOla_LEAVE_ONE_OUT
                        //
)

{
    double correct = 0.0;
    integer adder;

    switch(mode)
    {
        case kOla_TEN_FOLD_CROSS_VALIDATION:
            adder = my nInstances / 10;
            break;

        case kOla_LEAVE_ONE_OUT:
            if (my nInstances > 1)
                adder = 1;
            else
                adder = 0;
            break;

        default:
            adder = 0;
    }
    
    if (adder == 0)
        return -1;

    for (integer begin = 1; begin <= my nInstances; begin += adder)
    {
        autoCategories c = KNN_classifyFold (me, my input.get(), fws, k, dist, begin, OlaMIN (begin + adder - 1, my nInstances));
        for (integer y = 1; y <= c->size; y ++)
            if (FeatureWeights_areFriends (c->at [y], my output->at [begin + y - 1]))
                correct += 1.0;
    }

    correct /= (double) my nInstances;
    return correct;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Evaluation using a separate test set                                                    //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_evaluateWithTestSet
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    PatternList p,      // The vectors of the test set
                        //
    Categories c,       // The categories of the test set
                        //
    FeatureWeights fws, // feature weights
                        //
    integer k,          // the number of sought after neighbours
                        //
    int dist            // distance weighting
                        //
)

{
    double correct = 0.0;
    autoCategories t = KNN_classifyToCategories (me, p, fws, k, dist);
	for (integer y = 1; y <= t->size; y ++)
		if (FeatureWeights_areFriends (t->at [y], c->at [y])) correct += 1.0;
	return correct / c->size;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Model search                                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

    typedef struct structsoil
    {
        double performance;
        integer dist;
        integer k;
    } soil;

double KNN_modelSearch
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    FeatureWeights fws, // feature weights
                        //
    integer * k,        // valid integer *, to hold the output value of k
                        //
    int * dist,         // valid int *, to hold the output value dist_weight
                        //
    int mode,           // evaluation mode
                        //
    double rate,        // learning rate
                        //
    integer nseeds      // the number of seeds to be used
                        //
)

{
	try {
		int dists[] = 
		{
			kOla_SQUARED_DISTANCE_WEIGHTED_VOTING, 
			kOla_DISTANCE_WEIGHTED_VOTING, 
			kOla_FLAT_VOTING
		};

		integer max = *k;
		double range = (double) max / 2.0;
		double pivot = range;

		double dpivot = 1.0;
		double drange = 1.0;
		double drate = rate / range;

		soil best = { 0, Melder_iround (dpivot), Melder_iround (dpivot) };
		autoNUMvector <soil> field ((integer) 0, nseeds - 1);

		while (range > 0)
		{
			for (integer n = 0; n < nseeds; n++)
			{
				field[n].k = Melder_iround (NUMrandomUniform (OlaMAX (pivot - range, 1), OlaMIN (pivot + range, max)));
				field[n].dist = Melder_iround (NUMrandomUniform (OlaMAX (dpivot - drange, 0), OlaMIN (dpivot + drange, 2)));
				field[n].performance = KNN_evaluate (me, fws, field[n].k, dists[field[n].dist], mode);
			}

			integer maxindex = 0;
			for (integer n = 1; n < nseeds; n ++)
				if (field [n]. performance > field [maxindex]. performance) maxindex = n;

			if (field [maxindex]. performance > best. performance)
			{
				pivot = field[maxindex].k;
				dpivot = field[maxindex].dist;

				best.performance = field[maxindex].performance;
				best.dist = field[maxindex].dist;
				best.k = field[maxindex].k;
			}

			range -= rate;
			drange -= drate;
		}

		*k = best.k;
		*dist = dists[best.dist];

		return best.performance;
	} catch (MelderError) {
		Melder_throw (me, U" & ", fws, U": model search not performed.");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Euclidean distance                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_distanceEuclidean
(
    PatternList ps,     // PatternList 1
                        //
    PatternList pt,     // PatternList 2
                        //
    FeatureWeights fws, // Feature weights
                        //
    integer rows,       // Vector index of pattern 1
                        //
    integer rowt        // Vector index of pattern 2
)

{
    double distance = 0.0;
    for (integer x = 1; x <= ps->nx; ++x)
        distance += OlaSQUARE ((ps->z[rows][x] - pt->z[rowt][x]) * fws->fweights->data[1][x]);
    return sqrt (distance);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Manhattan distance                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_distanceManhattan
(
    PatternList ps, // PatternList 1
                    //
    PatternList pt, // PatternList 2
                    //
    integer rows,   // Vector index of pattern 1
                    //
    integer rowt    // Vector index of pattern 2
                    //
)

{
    longdouble distance = 0.0;
    for (integer x = 1; x <= ps->nx; x ++)
        distance += fabs (ps->z[rows][x] - pt->z[rowt][x]);
    return (double) distance;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Find longest distance                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_max
(
    double * distances,     // an array of distances containing ...
                            //
    integer ndistances      // ndistances distances
                            //
)

{
    integer maxndx = 0;
    for (integer maxc = 1; maxc < ndistances; maxc ++)
        if (distances[maxc] > distances[maxndx]) 
            maxndx = maxc;
    return maxndx;
}


////////////////////////////////////////////////////////////////////////////////////////////
// Locate k neighbours, skip one + disposal of distance                                    //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_kNeighboursSkip
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,      // source pattern
                        //
    PatternList p,      // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    integer jy,         // source instance index
                        //
    integer k,          // the number of sought after neighbours
                        //
    integer * indices,  // memory space to contain the indices of
                        // the k neighbours
                        //
    integer skipper     // the index of the instance to be skipped
                        //
)

{
    integer maxi;
    integer dc = 0;
    integer py = 1;

    autoNUMvector <double> distances ((integer) 0, k - 1);

    Melder_assert (jy > 0 && jy <= j->ny);
    Melder_assert (k > 0 && k <= p->ny);
    Melder_assert (skipper <= p->ny);

    while (dc < k && py <= p -> ny)
    {
        if (py != jy && py != skipper)
        {
            distances [dc] = KNN_distanceEuclidean (j, p, fws, jy, py);
            indices [dc] = py;
            ++ dc;
        }
        ++ py;
    }

    maxi = KNN_max (distances.peek(), k);
    while (py <= p->ny)
    {
        if (py != jy && py != skipper)
        {
            double d = KNN_distanceEuclidean (j, p, fws, jy, py);
            if (d < distances [maxi])
            {
                distances [maxi] = d;
                indices [maxi] = py;
                maxi = KNN_max (distances.peek(), k);
            }
        }
        ++ py;
    }
    
    return OlaMIN (k, dc);

}

//////////////////////////////////////////////////////////////////////////////////
// Locate the k nearest neighbours, exclude instances within the range defined  //
// by [begin ... end]                                                           //
//////////////////////////////////////////////////////////////////////////////////

integer KNN_kNeighboursSkipRange
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,      // source-pattern (where the unknown is located)
                        //
    PatternList p,      // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    integer jy,         // the index of the unknown instance in the source pattern
                        //
    integer k,          // the number of sought after neighbours
                        //
    integer * indices,  // a pointer to a memory-space big enough for k integers
                        // representing indices to the k neighbours in the
                        // target pattern
                        //
    double * distances, // a pointer to a memory-space big enough for k
                        // doubles representing the distances to the k
                        // neighbours
                        //
    integer begin,      // an index indicating the first instance in the
                        // target pattern to be excluded from the search
                        //
    integer end         // an index indicating the last instance in the
                        // range of excluded instances in the target
                        // pattern
)

{
    ///////////////////////////////
    // Private variables         //
    ///////////////////////////////

    integer maxi;                   // index indicating the most distant neighbour
                                    // among the k nearest
                                    //
    integer dc = 0;                 // fetch counter
                                    //
    integer py = 0;                 //

    Melder_assert (jy > 0 && jy <= j->ny);
    Melder_assert (k > 0 && k <= p->ny);
    Melder_assert (end > 0 && end <= j->ny);
    Melder_assert (begin > 0 && begin <= j->ny);

    while (dc < k && (end + py) % p->ny + 1 != begin)   // the first k neighbours are the nearest found
    {                                                   // sofar

        if ((end + py) % p->ny + 1 != jy)               // no instance is its own neighbour
        {
            distances [dc] = KNN_distanceEuclidean (j, p, fws, jy, (end + py) % p->ny + 1);
            indices [dc] = (end + py) % p->ny + 1;
            ++dc;
        }
        ++py;
    }

    maxi = KNN_max(distances, k);                       // accept only those instances less distant
    while ((end + py) % p->ny + 1 != begin)             // than the least near one found this far
    {
        if ((end + py) % p->ny + 1 != jy)
        {
            double d = KNN_distanceEuclidean(j, p, fws, jy, (end + py) % p->ny + 1);
            if (d < distances[maxi])
            {
                distances [maxi] = d;
                indices [maxi] = (end + py) % p->ny + 1;
                maxi = KNN_max (distances, k);
            }
        }
        ++py;
    }

    return OlaMIN (k, dc); // return the number of found neighbours

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locate k neighbours                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_kNeighbours
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,      // source-pattern (where the unknown is located)
                        //
    PatternList p,      // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    integer jy,         // the index of the unknown instance in the source pattern
                        //
    integer k,          // the number of sought after neighbours
                        //
    integer * indices,  // a pointer to a memory-space big enough for k integers
                        // representing indices to the k neighbours in the
                        // target pattern
    double * distances  // a pointer to a memory-space big enough for k
                        // doubles representing the distances to the k
                        // neighbours
                        //
)   

{
    integer maxi;
    integer dc = 0;
    integer py = 1;

    Melder_assert (jy > 0 && jy <= j->ny);
    Melder_assert (k > 0 && k <= p->ny);
    Melder_assert (indices);
    Melder_assert (distances);

    while (dc < k && py <= p->ny)
    {
        if (py != jy)
        {
            distances[dc] = KNN_distanceEuclidean (j, p, fws, jy, py);
            indices[dc] = py;
            ++dc;
        }
        ++py;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if (py != jy)
        {
            double d = KNN_distanceEuclidean (j, p, fws, jy, py);
            if (d < distances [maxi])
            {
                distances [maxi] = d;
                indices [maxi] = py;
                maxi = KNN_max (distances, k);
            }
        }
        ++py;
    }

    integer ret = OlaMIN (k, dc);
    if (ret < 1)
    {
        indices [0] = jy;
        return 0;
    }
    else
        return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locating k (nearest) friends                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_kFriends
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,      // source pattern
                        //
    PatternList p,      // target pattern (where friends are sought for)
                        //
    Categories c,       // categories
                        //
    integer jy,         // the index of the source instance
                        //  
    integer k,          // the number of sought after friends
                        //
    integer * indices   // a pointer to a memory-space big enough for k integers
                        // representing indices to the k friends in the
                        // target pattern
)

{
    integer maxi;
    integer dc = 0;
    integer py = 1;
    autoNUMvector <double> distances ((integer) 0, k - 1);

    Melder_assert (jy <= j->ny  && k <= p->ny && k > 0);
    Melder_assert (indices);

    while (dc < k && py < p->ny)
    {
        if (jy != py && FeatureWeights_areFriends (c->at [jy], c->at [py]))
        {
            distances[dc] = KNN_distanceManhattan (j, p, jy, py);
            indices[dc] = py;
            dc++;
        }
        ++py;
    }

    maxi = KNN_max (distances.peek(), k);
    while (py <= p->ny)
    {
        if (jy != py && FeatureWeights_areFriends (c->at [jy], c->at [py]))
        {
            double d = KNN_distanceManhattan (j, p, jy, py);
            if (d < distances [maxi])
            {
                distances [maxi] = d;
                indices [maxi] = py;
                maxi = KNN_max (distances.peek(), k);
            }
        }
        ++py;
    }

    return OlaMIN (k, dc);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Computing the distance to the nearest enemy                                             //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_nearestEnemy
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,  // source-pattern
                    //
    PatternList p,  // target pattern (where friends are sought for)
                    //
    Categories c,   // categories
                    //
    integer jy      // the index of the source instance
                    //
)

{
    double distance = KNN_distanceManhattan(j, p, jy, 1);
    Melder_assert(jy > 0 && jy <= j->ny );

    for (integer y = 2; y <= p->ny; y++)
    {
        if (FeatureWeights_areEnemies (c->at [jy], c->at [y]))
        {
            double newdist = KNN_distanceManhattan(j, p, jy, y);
            if (newdist > distance) distance = newdist;
        }
    }
    return distance;

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Computing the number of friends among k neighbours                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_friendsAmongkNeighbours
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,  // source-pattern
                    //
    PatternList p,  // target pattern (where friends are sought for)
                    //
    Categories c,   // categories
                    //
    integer jy,     // the index of the source instance
                    //
    integer k       // k (!)
                    //
)

{
    autoNUMvector <double> distances ((integer) 0, k - 1);
    autoNUMvector <integer> indices ((integer) 0, k - 1);
    integer friends = 0;

    Melder_assert (jy > 0 && jy <= j->ny  && k <= p->ny && k > 0);

    autoFeatureWeights fws = FeatureWeights_create (p -> nx);
    integer ncollected = KNN_kNeighbours (j, p, fws.get(), jy, k, indices.peek(), distances.peek());

    while (ncollected--)
        if (FeatureWeights_areFriends (c->at [jy], c->at [indices [ncollected]])) friends ++;
    
    return friends;

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locating k unique (nearest) enemies                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_kUniqueEnemies
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList j,   // source-pattern
                     //
    PatternList p,   // target pattern (where friends are sought for)
                     //
    Categories c,    // categories
                     //
    integer jy,      // the index of the source instance
                     //
    integer k,       // k (!)
                     //
    integer *indices // a memory space to hold the indices of the
                     // located enemies
                     //
)

{
    integer maxi;
    integer dc = 0;
    integer py = 1;
    double *distances = NUMvector <double> (0, k - 1);

    Melder_assert (jy <= j->ny);
	Melder_assert (k <= p->ny);
	Melder_assert (k > 0);
    Melder_assert (indices);

    while (dc < k && py <= p->ny)
    {
        if (FeatureWeights_areEnemies (c->at [jy], c->at [py]))
        {
            int hasfriend = 0;
            for (integer sc = 0; sc < dc; ++sc)
                if (FeatureWeights_areFriends (c->at [py], c->at [indices [sc]])) hasfriend = 1;

            if (!hasfriend)
            {
                distances[dc] = KNN_distanceManhattan(j, p, jy, py);
                indices[dc] = py;
                ++dc;
            }
        }
        ++py;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if (FeatureWeights_areEnemies (c->at [jy], c->at [py]))
        {
            int hasfriend = 0;
            for (integer sc = 0; sc < dc; ++sc)
                if (FeatureWeights_areFriends (c->at [py], c->at [indices[sc]])) hasfriend = 1;

            if (!hasfriend)
            {
                double d = KNN_distanceManhattan(j, p, jy, py);
                if (d < distances[maxi] && FeatureWeights_areFriends (c->at [jy], c->at [py]))
                {
                    distances[maxi] = d;
                    indices[maxi] = py;
                    maxi = KNN_max(distances, k);
                }
            }
        }
        ++py;
    }
	NUMvector_free (distances, 0);
    return OlaMIN (k, dc);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute dissimilarity matrix                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

autoDissimilarity KNN_patternToDissimilarity
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList p,          // PatternList
                        //
    FeatureWeights fws  // Feature weights
                        //
)

{
    autoDissimilarity output = Dissimilarity_create (p -> ny);
	for (integer y = 1; y <= p -> ny; ++ y)
		for (integer x = 1; x <= p -> ny; ++ x)
			output -> data [y] [x] = KNN_distanceEuclidean (p, p, fws, y, x);
	return output;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute frequencies                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

integer KNN_kIndicesToFrequenciesAndDistances
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Categories c,        // Source categories
                         //
    integer k,           // k (!)
                         //
    integer * indices,   // In: indices
                         //
    double * distances,  // Out: distances
                         //
    double * freqs,      // Out: and frequencies (double, sic!)
                         //
    integer *freqindices // Out: and indices -> freqs.
)

{
    integer ncategories = 0;
    
    Melder_assert (k <= c->size && k > 0);
    Melder_assert (distances && indices && freqs && freqindices);

    for (integer y = 0; y < k; ++y)
    {
        int hasfriend = 0;
        integer ifriend = 0;

        while (ifriend < ncategories)
        {
            if (FeatureWeights_areFriends (c->at [indices [y]], c->at [freqindices [ifriend]]))
            {
                hasfriend = 1;
                break;
            }
            ++ifriend;
        }

        if (!hasfriend)
        {
            freqindices[ncategories] = indices[y];
            freqs[ncategories] = 1;
            distances[ncategories] = distances[y];
            ncategories++;
        }
        else
        {
            ++freqs[ifriend];
            distances[ifriend] += (distances[y] - distances[ifriend]) / (ncategories + 1);
        }
    }
    return(ncategories);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Normalize array                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

void KNN_normalizeFloatArray
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    double * array,     // Array to be normalized
                        //
    integer n           // The number of elements
                        // in the array
)

{
    integer c = 0;
    longdouble sum = 0.0;

    while (c < n)
        sum += array [c ++];   // this sums over array [0 .. n-1]

    while (c --)
        array [c] /= sum;   // this scales array [0 .. n-1]
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Remove instance                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

void KNN_removeInstance
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,     // Classifier
                //
    integer y   // Index of the instance to be purged
                //
)

{
    if (y == 1 && my nInstances == 1) {
        my nInstances = 0;
        my input.reset();
        my output.reset();
        return;
    }

    Melder_assert (y > 0 && y <= my nInstances);

    autoPatternList newPattern = PatternList_create (my nInstances - 1, my input -> nx);
	integer yt = 1;
	for (integer cy = 1; cy <= my nInstances; cy ++) {
		if (cy != y) {
			for (integer cx = 1; cx <= my input -> nx; cx ++)
				newPattern -> z [yt] [cx] = my input -> z [cy] [cx];
			yt ++;
		}
	}

	my input = newPattern.move();
	my output -> removeItem (y);
	my nInstances--;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Shuffle instances                                                                       //
/////////////////////////////////////////////////////////////////////////////////////////////

void KNN_shuffleInstances
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me      // Classifier whose instance
                // base is to be shuffled
)
{
    if (my nInstances < 2) 
        return;   // it takes at least two to tango

    autoPatternList new_input = PatternList_create (my nInstances, my input -> nx);
    autoCategories new_output = Categories_create ();
	integer y = 1;
	while (my nInstances)
	{
		integer pick = NUMrandomInteger (1, my nInstances);
		new_output -> addItem_move (Data_copy (my output->at [pick]));

		for (integer x = 1; x <= my input -> nx; x ++)
			new_input -> z [y] [x] = my input -> z [pick] [x];

		KNN_removeInstance (me, pick);
		y ++;
	}

	my nInstances = new_output->size;
	my input = new_input.move();
	my output = new_output.move();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// KNN to Permutation (experimental)                                                       //
/////////////////////////////////////////////////////////////////////////////////////////////
autoPermutation KNN_SA_ToPermutation
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    integer tries,      //
                        //
    integer iterations, //
                        //
    double step_size,   //
                        //
    double boltzmann_c, //
                        //
    double temp_start,  //
                        //
    double damping_f,   //
                        //
    double temp_stop    //
                        //
)

{
    gsl_rng * r;
    const gsl_rng_type * T;

    KNN_SA_t * istruct = KNN_SA_t_create (my input.get());
    autoPermutation result = Permutation_create (my nInstances);

    gsl_siman_params_t params = { (int) tries, (int) iterations, step_size, boltzmann_c, temp_start, damping_f, temp_stop};

    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    gsl_siman_solve(r, 
                    istruct, 
                    KNN_SA_t_energy, 
                    KNN_SA_t_step, 
                    KNN_SA_t_metric, 
                    nullptr,                // KNN_SA_t_print
                    KNN_SA_t_copy, 
                    KNN_SA_t_copy_construct, 
                    KNN_SA_t_destroy, 
                    0, 
                    params);

    for (integer i = 1; i <= my nInstances; ++i)
        result->p[i] = istruct->indices[i];

    KNN_SA_t_destroy(istruct);

    return result;
}


double KNN_SA_t_energy
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    void * istruct
             
)

{
    if(((KNN_SA_t *) istruct)->p->ny < 2) 
        return(0);  

    double eCost = 0;
    for (integer i = 1; i <= ((KNN_SA_t *) istruct)->p->ny; ++i)
    {
        /* fast and sloppy version */ 
       
        double jDist = 0;
        double kDist = 0;

        integer j = i - 1 > 0 ? i - 1 : ((KNN_SA_t *) istruct)->p->ny;
        integer k = i + 1 <= ((KNN_SA_t *) istruct)->p->ny ? i + 1 : 1;

        for (integer x = 1; x <= ((KNN_SA_t *) istruct)->p->nx; ++x)
        {
            jDist +=    OlaSQUARE(((KNN_SA_t *) istruct)->p->z[((KNN_SA_t *) istruct)->indices[i]][x] - 
                        ((KNN_SA_t *) istruct)->p->z[((KNN_SA_t *) istruct)->indices[j]][x]);
            
            kDist +=    OlaSQUARE(((KNN_SA_t *) istruct)->p->z[((KNN_SA_t *) istruct)->indices[i]][x] - 
                        ((KNN_SA_t *) istruct)->p->z[((KNN_SA_t *) istruct)->indices[k]][x]);
        }   
        
        eCost += ((sqrt(jDist) + sqrt(kDist)) / 2 - eCost) / i;

    }

    return(eCost);
}

double KNN_SA_t_metric
(
    void * istruct1,
    void * istruct2
)

{
    double result = 0;

    for (integer i = ((KNN_SA_t *) istruct1)->p->ny; i >= 1; --i)
        if(((KNN_SA_t *) istruct1)->indices[i] !=  ((KNN_SA_t *) istruct2)->indices[i]) 
            ++result;

    return result;
}

void KNN_SA_t_print (void * istruct) {
    Melder_casual (U"\n");
    for (integer i = 1; i <= ((KNN_SA_t *) istruct) -> p -> ny; i ++)
        Melder_casual (((KNN_SA_t *) istruct) -> indices [i]);
    Melder_casual (U"\n");
}

void KNN_SA_t_step
(
    const gsl_rng * r,
    void * istruct,
    double step_size
)

{
    integer i1 = Melder_iround ((((KNN_SA_t *) istruct) -> p -> ny - 1) * gsl_rng_uniform (r)) + 1;
    integer i2 = (i1 + Melder_iround (step_size * gsl_rng_uniform (r))) % ((KNN_SA_t *) istruct) -> p -> ny + 1;

    if (i1 == i2)
        return;

    if (i1 > i2)
        OlaSWAP (integer, i1, i2);

    integer partitions[i2 - i1 + 1];

    KNN_SA_partition(((KNN_SA_t *) istruct)->p, i1, i2, partitions);

    for (integer r, l = 1, stop = i2 - i1 + 1; l < stop; l ++)
    {
        while (l < stop && partitions [l] == 1)
            l ++;

        r = l + 1;

        while (r <= stop && partitions [r] == 2)
            r ++;

        if (r == stop)
            break;
        
        OlaSWAP (integer, ((KNN_SA_t *) istruct) -> indices [i1], ((KNN_SA_t *) istruct) -> indices [i2]);
    }
}

void KNN_SA_t_copy
(
    void * istruct_src,
    void * istruct_dest
)

{
    ((KNN_SA_t *) istruct_dest)->p = ((KNN_SA_t *) istruct_src)->p;

    for (integer i = 1; i <= ((KNN_SA_t *) istruct_dest)->p->ny; ++i)
        ((KNN_SA_t *) istruct_dest)->indices[i] = ((KNN_SA_t *) istruct_src)->indices[i];
}


void * KNN_SA_t_copy_construct
(
    void * istruct
)

{
    KNN_SA_t * result = (KNN_SA_t *) malloc(sizeof(KNN_SA_t));

    result->p = ((KNN_SA_t *) istruct)->p;
    result->indices = (integer *) malloc (sizeof(integer) * (result->p->ny + 1));

    for (integer i = 1; i <= result->p->ny; ++i)
        result->indices[i] = ((KNN_SA_t *) istruct)->indices[i];

    return((void *) result);
}

KNN_SA_t * KNN_SA_t_create
(
    PatternList p
)

{
    KNN_SA_t * result = (KNN_SA_t *) malloc(sizeof(KNN_SA_t));

    result->p = p;
    result->indices = (integer *) malloc (sizeof (integer) * (p->ny + 1));

    for (integer i = 1; i <= p->ny; ++i)
        result->indices[i] = i;

    return(result);
}

void KNN_SA_t_destroy
(
    void * istruct
)

{
    free(((KNN_SA_t *) istruct)->indices);
    free((KNN_SA_t *) istruct);
}

void KNN_SA_partition
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList p,          //
                            //
    integer i1,             // i1 < i2
                            //
    integer i2,             //
                            //
    integer * result        // [0] not used
                            //
)

{
    integer c1 = Melder_iround (NUMrandomUniform (i1, i2));   // BUG: probably incorrect (the edges have half-probability)
    integer c2 = Melder_iround (NUMrandomUniform (i1, i2));

    double *p1 = NUMvector <double> (1, p->nx); 
    double *p2 = NUMvector <double> (1, p->nx);

    for (integer x = 1; x <= p->nx; ++x)
    {
        p1[x] = p->z[c1][x];
        p2[x] = p->z[c2][x];
    }

    for (bool converging = true; converging; )
    {
        double d1, d2;
        converging = false;

        for (integer i = i1, j = 1; i <= i2; ++i)
        {
            d1 = 0.0;
            d2 = 0.0;
        
            for (integer x = 1; x <= p->nx; ++x)
            {
                d1 += OlaSQUARE(p->z[i][x] - p1[x]);
                d2 += OlaSQUARE(p->z[i][x] - p2[x]);
            }

            d1 = sqrt (d1);
            d2 = sqrt (d2);

            if (d1 < d2)
            {
                if (result [j] != 1)
                {
                    converging = true;
                    result [j] = 1;
                }
            }
            else
            { 
                if (result [j] != 2)
                {
                    converging = true;
                    result [j] = 2;
                }
            }
            j ++;
        }

        for (integer x = 1; x <= p -> nx; x ++)
        {
            p1 [x] = 0.0;
            p2 [x] = 0.0;
        }

        for (integer i = i1, j = 1, j1 = 1, j2 = 1; i <= i2; i ++)
        {
            if (result [j] == 1)
            {
                for (integer x = 1; x <= p->nx; x ++)
                    p1[x] += (p->z[i][x] - p1[x]) / j1;
                j1 ++;
            }
            else
            {
                for (integer x = 1; x <= p -> nx; x ++)
                    p2[x] += (p->z[i][x] - p2[x]) / j2;
                j2 ++;
            }
            j ++;
        }
    }
    NUMvector_free (p1, 1);
    NUMvector_free (p2, 1);
}


/* End of file KNN.cpp */
