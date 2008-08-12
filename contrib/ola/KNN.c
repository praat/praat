/* KNN.c
 *
 * Copyright (C) 2008 Ola Söder
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

/* $URL: svn://pegasos.dyndns.biz/praat/trunk/kNN/KNN.c $
 * $Rev: 137 $
 * $Author: stix $
 * $Date: 2008-08-10 19:34:07 +0200 (Sun, 10 Aug 2008) $
 * $Id: KNN.c 137 2008-08-10 17:34:07Z stix $
 */

/*
 * os 20080529 Initial release
 */

#include "KNN.h"

#include "oo_DESTROY.h"
#include "KNN_def.h"
#include "oo_COPY.h"
#include "KNN_def.h"
#include "oo_EQUAL.h"
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


/////////////////////////////////////////////////////////////////////////////////////////////
// Praat specifics                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

static void info (I)
{
    iam (KNN);
    classData -> info (me);
    MelderInfo_writeLine2 (L"Size of instancebase: ", Melder_integer (my nInstances));
}

class_methods (KNN, Data)
class_method_local (KNN, destroy)
class_method_local (KNN, copy)
class_method_local (KNN, equal)
class_method_local (KNN, writeText)
class_method_local (KNN, writeBinary)
class_method_local (KNN, readText)
class_method_local (KNN, readBinary)
class_method_local (KNN, description)
class_method (info)
class_methods_end

/////////////////////////////////////////////////////////////////////////////////////////////
// Creation                                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

KNN KNN_create ()// a near-dummy function
{
    KNN me = new (KNN);
    if (!me)
    {
        return(NULL);
    }
    else
    {
        my nInstances = 0;
        return(me);
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
    Pattern p,      // source pattern
                    //
    Categories c,   // target categories
                    //
    int method,     // method <- REPLACE or APPEND
                    //
    int ordering    // ordering <- SHUFFLE?
)

{
    if (c->size == p->ny)           // the number of input vectors must
    {                               // equal the number of categories
        switch (method)
        {
        case kOla_REPLACE:          // in REPLACE mode simply
                                    // dispose of the current
            if (my nInstances > 0)  // instance base and store
            {                       // the new one
                forget(my input);
                forget(my output);
            }

            my input = Data_copy(p);
            my output = Data_copy(c);
            my nInstances = c->size;

            break;

        case kOla_APPEND:                   // in APPEND mode a new
                                            // instance base is formed
                                            // by merging the new and
                                            // the old
                                            //
            if (p->nx == (my input)->nx)    // the number of features
                                            // of the old and new
                                            // instance base must
                                            // match otherwise merging
                                            // wont be possible
            {

                Matrix tinput = Matrix_appendRows(my input, p);
                Categories toutput = Collections_merge(my output, c);

                forget(my input);
                forget(my output);

                my input = (Pattern) tinput;
                my output = toutput;
                my nInstances += p->ny;
            }
            else                            // fail
            {
                return(kOla_DIMENSIONALITY_MISMATCH);
            }
            break;
        }
        if (ordering == kOla_SHUFFLE)            // shuffle the instance base
        {
            KNN_shuffleInstances(me);
        }
    }
    else                                    // fail
    {
        return(kOla_PATTERN_CATEGORIES_MISMATCH);
    }
    return(kOla_SUCCESS);                        // success
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Classification - To Categories                                                          //
/////////////////////////////////////////////////////////////////////////////////////////////

Categories KNN_classifyToCategories
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    Pattern ps,         // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long k,             // the number of sought after neighbours
                        //
    int dist            // distance weighting
)

{
    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    long ncollected;
    long ncategories;
    long indices[k];
    long freqindices[k];
    double distances[k];
    double freqs[k];
    long outputindices[ps->ny];
    long noutputindices = 0;

    for (long y = 1; y <= ps->ny; y++)
    {
        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighbours(ps, my input, fws, y, k, indices, distances);

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances(my output, k, indices, distances, freqs, freqindices);

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        switch (dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(distances[c], MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(SQUARE(distances[c]), MINFLOAT);
        }

        KNN_normalizeFloatArray(freqs, ncategories);
        outputindices[noutputindices++] = freqindices[KNN_max(freqs, ncategories)];
    }

    Categories output = Categories_create();
    if (output)
    {
        Categories_init(output, 0);
        for (long o = 0; o < noutputindices; o++)
        {
            Collection_addItem(output, Data_copy((my output)->item[outputindices[o]]));
        }
        return(output);
    }
    return(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Classification - To TableOfReal                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

TableOfReal KNN_classifyToTableOfReal
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    Pattern ps,         // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long k,             // the number of sought after neighbours
                        //
    int dist            // distance weighting
)

{

    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    long ncollected;
    long ncategories;
    long indices[k];
    long freqindices[k];
    double distances[k];
    double freqs[k];
    long outputindices[ps->ny];
    double tabledata[ps->ny * 4];
    long noutputindices = 0;

    for (long y = 1; y <= ps->ny; y++)
    {

        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighbours(ps, my input, fws, y, k, indices, distances);

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances(my output, k, indices, distances, freqs, freqindices);

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        double freqb[ncategories];
        for (long c = 0; c < ncategories; c++)
            freqb[c] = freqs[c];

        switch (dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(distances[c], MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(SQUARE(distances[c]), MINFLOAT);
        }

        KNN_normalizeFloatArray(freqs, ncategories);
        long max = KNN_max(freqs, ncategories);

        outputindices[noutputindices] = freqindices[max];
        tabledata[noutputindices * 4] = freqb[max];
        tabledata[noutputindices * 4 + 1] = distances[max];
        tabledata[noutputindices * 4 + 2] = freqs[max];
        tabledata[noutputindices * 4 + 3] = freqb[max] / ncollected;

        noutputindices++;
    }

    TableOfReal output = TableOfReal_create(noutputindices, 4);
    if (output)
    {
        TableOfReal_setColumnLabel(output, 1, L"n neighbours");
        TableOfReal_setColumnLabel(output, 2, L"mean distance");
        TableOfReal_setColumnLabel(output, 3, L"p(w)");
        TableOfReal_setColumnLabel(output, 4, L"p");

        for (long yq = 0; yq < noutputindices; yq++)
        {
            TableOfReal_setRowLabel(output, yq + 1, SimpleString_c((my output)->item[outputindices[yq]]));
            for (long xq = 0; xq < 4; xq++)
                output->data[yq + 1][xq + 1] = tabledata[yq * 4 + xq];
        }
        return(output);
    }
    return(NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Classification - To TableOfReal, all candidates                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

TableOfReal KNN_classifyToTableOfRealAll
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    Pattern ps,         // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long k,             // the number of sought after neighbours
                        //
    int dist            // distance weighting
)

{

    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    long ncollected;
    long ncategories;
    long indices[k];
    long freqindices[k];
    double distances[k];
    double freqs[k];
    long outputindices[k * ps->ny];
    double tabledata[k * ps->ny * 4];
    long noutputindices = 0;

    for (long y = 1; y <= ps->ny; y++)
    {

        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighbours(ps, my input, fws, y, k, indices, distances);

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances(my output, k, indices, distances, freqs, freqindices);

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        for (long q = 0; q < ncategories; q++)
        {
            tabledata[(q + noutputindices) * 4] = freqs[q];
            tabledata[(q + noutputindices) * 4 + 1] = distances[q];
            tabledata[(q + noutputindices) * 4 + 3] = freqs[q] / ncollected;
        }

        switch (dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(distances[c], MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(SQUARE(distances[c]), MINFLOAT);
        }

        KNN_normalizeFloatArray(freqs, ncategories);

        for (long q = 0; q < ncategories; q++)
        {
            outputindices[q + noutputindices] = freqindices[q];
            tabledata[(q + noutputindices) * 4 + 2] = freqs[q];
        }
        noutputindices += ncategories;
    }

    TableOfReal output = TableOfReal_create(noutputindices, 4);
    if (output)
    {
        TableOfReal_setColumnLabel(output, 1, L"n neighbours");
        TableOfReal_setColumnLabel(output, 2, L"mean distance");
        TableOfReal_setColumnLabel(output, 3, L"p(w)");
        TableOfReal_setColumnLabel(output, 4, L"p");

        for (long yq = 0; yq < noutputindices; yq++)
        {
            TableOfReal_setRowLabel(output, yq + 1, SimpleString_c((my output)->item[outputindices[yq]]));
            for (long xq = 0; xq < 4; xq++)
                output->data[yq + 1][xq + 1] = tabledata[yq * 4 + xq];
        }
        return(output);
    }
    return(NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Classification - Folding                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////

Categories KNN_classifyFold
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    Pattern ps,         // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long k,             // the number of sought after neighbours
                        //
    int dist,           // distance weighting
                        //
    long begin,         // fold start, inclusive [...
                        //
    long end            // fold end, inclusive ...]
                        //
)

{
    if (begin > end) LONGSWAP(begin, end);// safety-belts
    if (begin < 1) begin = 1;
    if (end > ps->ny) end = ps->ny;
    if (k < 1) k = 1;
    if (k > my nInstances - (end - begin))
        k = my nInstances - (end - begin);

    long ncollected;
    long ncategories;
    long indices[k];
    long freqindices[k];
    double distances[k];
    double freqs[k];
    long outputindices[ps->ny];
    long noutputindices = 0;

    for (long y = begin; y <= end; y++)
    {
        /////////////////////////////////////////
        // Localizing the k nearest neighbours //
        /////////////////////////////////////////

        ncollected = KNN_kNeighboursSkipRange(ps, my input, fws, y, k, indices, distances, begin, end);

        /////////////////////////////////////////////////
        // Computing frequencies and average distances //
        /////////////////////////////////////////////////

        ncategories = KNN_kIndicesToFrequenciesAndDistances(my output, k, indices, distances, freqs, freqindices);

        ////////////////////////
        // Distance weighting //
        ////////////////////////

        switch (dist)
        {
            case kOla_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(distances[c], MINFLOAT);
                break;

            case kOla_SQUARED_DISTANCE_WEIGHTED_VOTING:
                for (long c = 0; c < ncategories; c++)
                    freqs[c] *= 1 / MAX(SQUARE(distances[c]), MINFLOAT);
        }

        KNN_normalizeFloatArray(freqs, ncategories);
        outputindices[noutputindices++] = freqindices[KNN_max(freqs, ncategories)];
    }

    Categories output = Categories_create();
    if (output)
    {
        Categories_init(output, 0);
        for (long o = 0; o < noutputindices; o++)
        {
            Collection_addItem(output, Data_copy((my output)->item[outputindices[o]]));
        }
        return(output);
    }
    return(NULL);
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
    long k,             // the number of sought after neighbours
                        //
    int dist,           // distance weighting
                        //
    int mode            // TEN_FOLD_CROSS_VALIDATION / LEAVE_ONE_OUT
                        //
)

{
    if (fws->fweights->numberOfColumns != (my input)->nx) return(kOla_FWEIGHTS_MISMATCH);

    double correct = 0;
    long adder = 1;

    if (mode == kOla_TEN_FOLD_CROSS_VALIDATION) adder = my nInstances / 10;

    for (long begin = 1; begin <= my nInstances; begin += adder)
    {
        Categories c = KNN_classifyFold(me, my input, fws, k, dist, begin, MIN(begin + adder - 1, my nInstances));
        for (long y = 1; y <= c->size; y++)
            if (FRIENDS(c->item[y], (my output)->item[begin + y - 1])) correct++;
        forget(c);
    }
    correct /= (double) my nInstances;
    return(correct);
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
    Pattern p,          // The vectors of the test set
                        //
    Categories c,       // The categories of the test set
                        //
    FeatureWeights fws, // feature weights
                        //
    long k,             // the number of sought after neighbours
                        //
    int dist            // distance weighting
                        //
)

{
    if (fws->fweights->numberOfColumns != p->nx) return(kOla_FWEIGHTS_MISMATCH);
    if (p->ny != c->size) return(kOla_PATTERN_CATEGORIES_MISMATCH);
    if (p->nx != (my input)->nx) return(kOla_DIMENSIONALITY_MISMATCH);
    if (p->ny == 0) return(0);

    double correct = 0;
    Categories t = KNN_classifyToCategories(me, p, fws, k, dist);

    if (t)
    {
        for (long y = 1; y <= t->size; y++)
            if (FRIENDS(t->item[y], c->item[y])) correct++;
        forget(t);
        return(correct / c->size);
    }
    else
    {
        return(0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Model search                                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_modelSearch
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,             // the classifier being used
                        //
    FeatureWeights fws, // feature weights
                        //
    long * k,           // valid long *, to hold the output value of k
                        //
    int * dist,         // valid int *, to hold the output value dist_weight
                        //
    int mode,           // evaluation mode
                        //
    double rate,        // learning rate
                        //
    long nseeds         // the number of seeds to be used
                        //
)

{
    int dists[] = {kOla_SQUARED_DISTANCE_WEIGHTED_VOTING, kOla_DISTANCE_WEIGHTED_VOTING, kOla_FLAT_VOTING};
    long max = *k;
    double range = (double) max / 2;
    double pivot = range;

    double dpivot = 1;
    double drange = 1;
    double drate = rate / range;

    typedef struct
    {
        double performance;
        long dist;
        long k;
    } soil;

    soil best = {0, lround(dpivot), lround(dpivot)};
    soil field[nseeds];

    while (range > 0)
    {
        for (long n = 0; n < nseeds; n++)
        {
            field[n].k = lround(NUMrandomUniform(MAX(pivot - range, 1), MIN(pivot + range, max)));
            field[n].dist = lround(NUMrandomUniform(MAX(dpivot - drange, 0), MIN(dpivot + drange, 2)));
            field[n].performance = KNN_evaluate(me, fws, field[n].k, dists[field[n].dist], mode);
        }

        long maxindex = 0;
        for (long n = 1; n < nseeds; n++)
            if (field[n].performance > field[maxindex].performance) maxindex = n;

        if (field[maxindex].performance > best.performance)
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

    return(best.performance);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Euclidean distance                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_distanceEuclidean
(
    Pattern ps,         // Pattern 1
                        //
    Pattern pt,         // Pattern 2
                        //
    FeatureWeights fws, // Feature weights
                        //
    long rows,          // Vector index of pattern 1
                        //
    long rowt           // Vector index of pattern 2
)

{
    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    double distance = 0;
    for (long x = 1; x <= ps->nx; x++)
        distance += SQUARE((ps->z[rows][x] - pt->z[rowt][x]) * fws->fweights->data[1][x]);

    return(sqrt(distance));
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Manhattan distance                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_distanceManhattan
(
    Pattern ps,     // Pattern 1
                    //
    Pattern pt,     // Pattern 2
                    //
    long rows,      // Vector index of pattern 1
                    //
    long rowt       // Vector index of pattern 2
                    //
)

{
    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    double distance = 0;
    for (long x = 1; x <= ps->nx; x++)
        distance += fabs(ps->z[rows][x] - pt->z[rowt][x]);

    return(distance);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Find longest distance                                                                   //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_max
(
    double * distances,     // an array of distances containing ...
                            //
    long ndistances         // ndistances distances
                            //
)

{
    /////////////////////////////////////////////////////////////
    // No checks are made, the validity of the parameters must //
    // be ensured by the calling function....                  //
    /////////////////////////////////////////////////////////////

    long maxc = 1;
    long maxndx = 0;

    while (maxc < ndistances)
    {
        if (distances[maxc] > distances[maxndx]) maxndx = maxc;
        maxc++;
    }

    return(maxndx);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locate k neighbours, skip one + disposal of distance                                    //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_kNeighboursSkip
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,          // source pattern
                        //
    Pattern p,          // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long jy,            // source instance index
                        //
    long k,             // the number of sought after neighbours
                        //
    long * indices,     // memory space to contain the indices of
                        // the k neighbours
                        //
    long skipper        // the index of the instance to be skipped
                        //
)

{
    long maxi;
    long dc = 0;
    long py = 1;

    double distances[k];

    if (jy > j->ny) jy = j->ny;// safety belt
    if (k > p->ny) k = p->ny;
    if (k < 1) k = 1;

    while (dc < k && py <= p->ny)
    {
        if ((py != jy) && (py != skipper))
        {

            distances[dc] = KNN_distanceEuclidean(j, p, fws, jy, py);
            indices[dc] = py;
            dc++;
        }
        py++;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if ((py != jy) && (py != skipper))
        {

            double d = KNN_distanceEuclidean(j, p, fws, jy, py);
            if (d < distances[maxi])
            {
                distances[maxi] = d;
                indices[maxi] = py;
                maxi = KNN_max(distances, k);
            }
        }
        py++;
    }
    return(MIN(k, dc));
}

//////////////////////////////////////////////////////////////////////////////////
// Locate the k nearest neighbours, exclude instances within the range defined  //
// by [begin ... end]//
//////////////////////////////////////////////////////////////////////////////////

long KNN_kNeighboursSkipRange
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,          // source-pattern (where the unknown is located)
                        //
    Pattern p,          // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long jy,            // the index of the unknown instance in the source pattern
                        //
    long k,             // the number of sought after neighbours
                        //
    long * indices,     // a pointer to a memory-space big enough for k longs
                        // representing indices to the k neighbours in the
                        // target pattern
                        //
    double * distances, // a pointer to a memory-space big enough for k
                        // doubles representing the distances to the k
                        // neighbours
                        //
    long begin,         // an index indicating the first instance in the
                        // target pattern to be excluded from the search
                        //
    long end            // an index indicating the last instance in the
                        // range of excluded instances in the target
                        // pattern
)

{
    ///////////////////////////////
    // Private variables         //
    ///////////////////////////////

    long maxi;                      // index indicating the most distant neighbour
                                    // among the k nearest
                                    //
    long dc = 0;                    // fetch counter
                                    //
    long py = 0;                    //

    if (jy > j->ny) jy = j->ny;     // safety belts
    if (end > j->ny) end = j->ny;   //
    if (begin < 1) begin = 1;       //
    if (k > p->ny) k = p->ny;       //
    if (k < 1) k = 1;               //

    while (dc < k && (end + py) % p->ny + 1 != begin)   // the first k neighbours are the nearest found
    {                                                   // sofar

        if ((end + py) % p->ny + 1 != jy)               // no instance is its own neighbour
        {
            distances[dc] = KNN_distanceEuclidean(j, p, fws, jy, (end + py) % p->ny + 1);
            indices[dc] = (end + py) % p->ny + 1;
            dc++;
        }
        py++;
    }

    maxi = KNN_max(distances, k);                       // accept only those instances less distant
    while ((end + py) % p->ny + 1 != begin)             // than the least near one found this far
    {
        if ((end + py) % p->ny + 1 != jy)
        {
            double d = KNN_distanceEuclidean(j, p, fws, jy, (end + py) % p->ny + 1);
            if (d < distances[maxi])
            {
                distances[maxi] = d;
                indices[maxi] = (end + py) % p->ny + 1;
                maxi = KNN_max(distances, k);
            }
        }
        py++;
    }
    return(MIN(k, dc));// return the number of found neighbours
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locate k neighbours                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_kNeighbours
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,          // source-pattern (where the unknown is located)
                        //
    Pattern p,          // target pattern (where neighbours are sought for)
                        //
    FeatureWeights fws, // feature weights
                        //
    long jy,            // the index of the unknown instance in the source pattern
                        //
    long k,             // the number of sought after neighbours
                        //
    long * indices,     // a pointer to a memory-space big enough for k longs
                        // representing indices to the k neighbours in the
                        // target pattern
    double * distances  // a pointer to a memory-space big enough for k
                        // doubles representing the distances to the k
                        // neighbours
                        //
)

{
    long maxi;
    long dc = 0;
    long py = 1;

    if (jy > j->ny) jy = j->ny;// safety belt
    if (k > p->ny) k = p->ny;
    if (k < 1) k = 1;

    while (dc < k && py <= p->ny)
    {
        if (py != jy)
        {
            distances[dc] = KNN_distanceEuclidean(j, p, fws, jy, py);
            indices[dc] = py;
            dc++;
        }
        py++;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if (py != jy)
        {
            double d = KNN_distanceEuclidean(j, p, fws, jy, py);
            if (d < distances[maxi])
            {
                distances[maxi] = d;
                indices[maxi] = py;
                maxi = KNN_max(distances, k);
            }
        }
        py++;
    }

    long ret = MIN(k, dc);
    if (ret < 1)
    {
        indices[0] = jy;
        return(0);
    }
    else
    {
        return(ret);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locating k (nearest) friends                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_kFriends
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,          // source-pattern
                        //
    Pattern p,          // target pattern (where friends are sought for)
                        //
    Categories c,       // categories
                        //
    long jy,            // the index of the source instance
                        //  
    long k,             // the number of sought after friends
                        //
    long * indices      // a pointer to a memory-space big enough for k longs
                        // representing indices to the k friends in the
                        // target pattern
)

{
    if (jy > j->ny) jy = j->ny;// safety belt
    if (k > p->ny) k = p->ny;
    if (k < 1) k = 1;

    long maxi;
    long dc = 0;
    long py = 1;
    double distances[k];

    while (dc < k && py < p->ny)
    {
        if (jy != py && FRIENDS(c->item[jy], c->item[py]))
        {
            distances[dc] = KNN_distanceManhattan(j, p, jy, py);
            indices[dc] = py;
            dc++;
        }
        py++;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if (jy != py && FRIENDS(c->item[jy],c->item[py]))
        {
            double d = KNN_distanceManhattan(j, p, jy, py);
            if (d < distances[maxi])
            {
                distances[maxi] = d;
                indices[maxi] = py;
                maxi = KNN_max(distances, k);
            }
        }
        py++;
    }
    return(MIN(k,dc));
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Computing the distance to the nearest enemy                                             //
/////////////////////////////////////////////////////////////////////////////////////////////

double KNN_nearestEnemy
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,      // source-pattern
                    //
    Pattern p,      // target pattern (where friends are sought for)
                    //
    Categories c,   // categories
                    //
    long jy         // the index of the source instance
                    //
)

{
    if (jy > j->ny) jy = j->ny;// safety belt

    double distance = KNN_distanceManhattan(j, p, jy, 1);

    for (long y = 2; y <= p->ny; y++)
    {
        if (ENEMIES(c->item[jy], c->item[y]))
        {
            double newdist = KNN_distanceManhattan(j, p, jy, y);
            if (newdist > distance) distance = newdist;
        }
    }
    return(distance);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Computing the number of friends among k neighbours                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_friendsAmongkNeighbours
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,      // source-pattern
                    //
    Pattern p,      // target pattern (where friends are sought for)
                    //
    Categories c,   // categories
                    //
    long jy,        // the index of the source instance
                    //
    long k          // k (!)
                    //
)

{
    if (jy > j->ny) jy = j->ny;// safety belt
    if (k > p->ny) k = p->ny;
    if (k < 1) k = 1;

    double distances[k];
    long indices[k];
    long friends = 0;

    FeatureWeights fws = FeatureWeights_create(p->nx);
    long ncollected = KNN_kNeighbours(j, p, fws, jy, k, indices, distances);
    forget(fws);

    while (ncollected--)
    {
        if (FRIENDS(c->item[jy], c->item[indices[ncollected]])) friends++;
    }
    return(friends);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Locating k unique (nearest) enemies                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_kUniqueEnemies
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern j,      // source-pattern
                    //
    Pattern p,      // target pattern (where friends are sought for)
                    //
    Categories c,   // categories
                    //
    long jy,        // the index of the source instance
                    //
    long k,         // k (!)
                    //
    long * indices  // a memory space to hold the indices of the
                    // located enemies
                    //
)

{
    if (jy > j->ny) jy = j->ny;// safety belt
    if (k > p->ny) k = p->ny;
    if (k < 1) k = 1;

    long maxi;
    long dc = 0;
    long py = 1;
    double distances[k];

    while (dc < k && py <= p->ny)
    {
        if (ENEMIES(c->item[jy], c->item[py]))
        {
            int hasfriend = 0;
            for (long sc = 0; sc < dc; sc++)
                if (FRIENDS(c->item[py], c->item[indices[sc]])) hasfriend = 1;

            if (!hasfriend)
            {
                distances[dc] = KNN_distanceManhattan(j, p, jy, py);
                indices[dc] = py;
                dc++;
            }
        }
        py++;
    }

    maxi = KNN_max(distances, k);
    while (py <= p->ny)
    {
        if (ENEMIES(c->item[jy], c->item[py]))
        {
            int hasfriend = 0;
            for (long sc = 0; sc < dc; sc++)
                if (FRIENDS(c->item[py], c->item[indices[sc]])) hasfriend = 1;

            if (!hasfriend)
            {
                double d = KNN_distanceManhattan(j, p, jy, py);
                if (d < distances[maxi] && FRIENDS(c->item[jy],c->item[py]))
                {
                    distances[maxi] = d;
                    indices[maxi] = py;
                    maxi = KNN_max(distances, k);
                }
            }
        }
        py++;
    }
    return(MIN(k,dc));
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute dissimilarity matrix                                                            //
/////////////////////////////////////////////////////////////////////////////////////////////

Dissimilarity KNN_patternToDissimilarity
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern p,          // Pattern
                        //
    FeatureWeights fws  // Feature weights
                        //
)

{
    Dissimilarity output = Dissimilarity_create(p->ny);

    if (output)
    {
        for (long y = 1; y <= p->ny; y++)
        {
            for (long x = 1; x <= p->ny; x++)
            {
                output->data[y][x] = KNN_distanceEuclidean(p, p, fws, y, x);
            }
        }
        return(output);
    }
    else
    {
        return(NULL);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute frequencies                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

long KNN_kIndicesToFrequenciesAndDistances
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Categories c,       // Source categories
                        //
    long k,             // k (!)
                        //
    long * indices,     // In: indices
                        //
    double * distances, // Out: distances
                        //
    double * freqs,     // Out: and frequencies (double, sic!)
                        //
    long *freqindices   // Out: and indices -> freqs.
)

{
    if (k > c->size) k = c->size;
    if (k < 1) k = 1;

    long ncategories = 0;

    for (long y = 0; y < k; y++)
    {
        int hasfriend = 0;
        long friend = 0;

        while (friend < ncategories)
        {
            if (FRIENDS(c->item[indices[y]], c->item[freqindices[friend]]))
            {
                hasfriend = 1;
                break;
            }
            friend++;
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
            freqs[friend]++;
            distances[friend] += (distances[y] - distances[friend]) / (ncategories + 1);
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
    long n              // The number of elements
                        // in the array
)

{
    double sum = 0;
    for (long c = 0; c < n; c++)
        sum += array[c];
    for (long c = 0; c < n; c++)
        array[c] /= sum;
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
    long y      // Index of the instance to be purged
                //
)

{
    if (y == 1 && my nInstances == 1)
    {
        my nInstances = 0;
        forget(my input);
        forget(my output);
        return;
    }

    if (y > my nInstances || y < 1) return;// safety belt

    Pattern new = Pattern_create(my nInstances - 1, (my input)->nx);

    if (new)
    {
        long yt = 1;
        for (long cy = 1; cy <= my nInstances; cy++)
            if (cy != y)
            {
                for (long cx = 1; cx <= (my input)->nx; cx++)
                    new->z[yt][cx] = (my input)->z[cy][cx];
                yt++;
            }
        forget(my input);
        my input = new;
        Collection_removeItem(my output, y);
        my nInstances--;
    }
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

    if (my nInstances < 2) return;  // It takes atleast two to tango

    Pattern new_input = Pattern_create(my nInstances, (my input)->nx);
    Categories new_output = Categories_create();

    if (new_input && new_output)
    {
        long y = 1;
        Categories_init(new_output, 0);
        while (my nInstances)
        {
            long pick = (long) lround(NUMrandomUniform(1, my nInstances));
            Collection_addItem(new_output, Data_copy((my output)->item[pick]));
            for (long x = 1;x <= (my input)->nx; x++)
                new_input->z[y][x] = (my input)->z[pick][x];
            KNN_removeInstance(me, pick);
            y++;
        }
        forget(my input);
        forget(my output);
        my input = new_input;
        my output = new_output;
        my nInstances  = new_output->size;
    }
    else
    {
        forget(new_input);
        forget(new_output);
    }
}

/* End of file KNN.c */
