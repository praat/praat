/* FeatureWeights.c
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010 Paul Boersma
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
 * os 20070529 Initial release
 * pb 20100606 removed some array-creations-on-the-stack
 * pb 2011/03/08 tried to repair some of the header file chaos (several procedures here should be in KNN.c instead)
 */

#include "FeatureWeights.h"
#include "KNN.h"   // BUG

#include "oo_DESTROY.h"
#include "FeatureWeights_def.h"
#include "oo_COPY.h"
#include "FeatureWeights_def.h"
#include "oo_EQUAL.h"
#include "FeatureWeights_def.h"
#include "oo_WRITE_TEXT.h"
#include "FeatureWeights_def.h"
#include "oo_WRITE_BINARY.h"
#include "FeatureWeights_def.h"
#include "oo_READ_TEXT.h"
#include "FeatureWeights_def.h"
#include "oo_READ_BINARY.h"
#include "FeatureWeights_def.h"
#include "oo_DESCRIPTION.h"
#include "FeatureWeights_def.h"

static void info(I)
{
    iam (FeatureWeights);
    classData -> info (me);
    MelderInfo_writeLine2 (L"Number of weights: ", Melder_integer ((my fweights)->numberOfColumns));
}

class_methods(FeatureWeights, Data)
class_method_local (FeatureWeights, destroy)
class_method_local (FeatureWeights, copy)
class_method_local (FeatureWeights, equal)
class_method_local (FeatureWeights, writeText)
class_method_local (FeatureWeights, writeBinary)
class_method_local (FeatureWeights, readText)
class_method_local (FeatureWeights, readBinary)
class_method_local (FeatureWeights, description)
class_method (info)
class_methods_end


/////////////////////////////////////////////////////////////////////////////////////////////
// Creation...    //
/////////////////////////////////////////////////////////////////////////////////////////////

FeatureWeights FeatureWeights_create
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    long nweights       // number of weights
)

{
    FeatureWeights me = Thing_new (FeatureWeights);
    if (me)
    {
        my fweights = TableOfReal_create(1, nweights);
        for (long i = 1; i <= nweights; i++)
        {
            (my fweights)->data[1][i] = 1;
        }
        return(me);
    }
    else
    {
        return(NULL);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute prior probabilities                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////

long FeatureWeights_computePriors
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Categories c,       // source categories
                        //
    long * indices,     // Out: instances indices ..
                        //  
    double * priors     // Out: .. and their prior probabilities
                        //
)

{
    long nc = 0;
    for (long y = 1; y <= c->size; y++)
    {
        long friend = -1;
        for (long sc = 0; sc < nc; sc++)
            if (FRIENDS(c->item[y], c->item[indices[sc]])) friend = sc;

        if (friend < 0)
        {
            indices[nc] = y;
            priors[nc] = 1;
            nc++;
        }
        else
        {
            priors[friend]++;
        }
    }
    for (long q = 0; q < nc; q++) priors[q] /= c->size;
    return(nc);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////

FeatureWeights FeatureWeights_compute           // Obsolete
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern pp,     // Source pattern
                    //
    Categories c,   // Source categories
                    //
    long k          // k(!)
)

{
    return(FeatureWeights_computeRELIEF(pp, c, k));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights (wrapper), evaluate using folding                               //
/////////////////////////////////////////////////////////////////////////////////////////////

FeatureWeights FeatureWeights_computeWrapperInt
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,         // Classifier
                    //
    long k,         // k(!)
                    //
    int d,          // distance weighting
                    //
    long nseeds,    // the number of seeds
                    //
    double alfa,    // shrinkage factor
                    //
    double stop,    // stop at
                    //
    int mode,       // mode (co/serial)
                    //
    int emode       // evaluation mode (10-fold/L1O)
                    //
)

{
    double pivot = 0.5;
    double range = 0.5;
    double progress = 1 / range;
    double *results = NUMdvector (0, nseeds);

    if (me)
    {
        FeatureWeights *cs = NUMpvector (0, nseeds);
        for (long y = 0; y <= nseeds; y++)
        {
            cs[y] = FeatureWeights_create((my input)->nx);
            if (!cs[y])
            {
                while (y--) forget(cs[y - 1]);
                return(NULL);
            }

        }

        for (long x = 1; x <= (my input)->nx; x++)
            cs[nseeds]->fweights->data[1][x] = pivot;

        results[nseeds] = KNN_evaluate(me, cs[nseeds], k, d, emode);

        while (range > 0 && results[nseeds] < stop)
        {
            long best = nseeds;
            if (!Melder_progress1(range * progress, L"")) break;

            if (mode == 2)
            {
                for (long x = 1; x <= (my input)->nx; x++)
                {
                    for (long y = 0; y < nseeds; y++)
                    {
                        cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
                                                      OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
                        results[y] = KNN_evaluate(me, cs[y], k, d, emode);
                    }
                    for (long q = 0; q < nseeds; q++)
                        if (results[q] > results[best]) best = q;

                    if (results[best] > results[nseeds])
                    {
                        for (long x = 1; x <= (my input)->nx; x++)
                            cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
                        results[nseeds] = results[best];
                    }
                }
            }
            else
            {
                for (long y = 0; y < nseeds; y++)
                {
                    for (long x = 1; x <= (my input)->nx; x++)
                    {
                        cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
                                                      OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
                    }
                    results[y] = KNN_evaluate(me, cs[y], k, d, emode);
                }

                for (long q = 0; q < nseeds; q++)
                    if (results[q] > results[best]) best = q;

                if (results[best] > results[nseeds])
                {
                    for (long x = 1; x <= (my input)->nx; x++)
                        cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
                    results[nseeds] = results[best];
                }
            }
            range -= alfa;
        }

        for (long y = 0; y < nseeds; y++)
            forget(cs[y]);
        NUMpvector_free (cs, 0);

        Melder_progress1(1.0, NULL);
  		NUMdvector_free (results, 0);
        return(cs[nseeds]);
    }
    NUMdvector_free (results, 0);
    return(NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights (wrapper), evaluate using separate test set                     //
/////////////////////////////////////////////////////////////////////////////////////////////

FeatureWeights FeatureWeights_computeWrapperExt
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN nn,         // Classifier
                    //
    Pattern pp,     // test pattern
                    //
    Categories c,   // test categories
                    //
    long k,         // k(!)
                    //
    int d,          // distance weighting
                    //
    long nseeds,    // the number of seeds
                    //
    double alfa,    // shrinkage factor
                    //
    double stop,    // stop at
                    //
    int mode        // mode (co/serial)
                    //
)

{
    double pivot = 0.5;
    double range = 0.5;
    double progress = 1 / range;
    double *results = NUMdvector (0, nseeds);

    if (nn)
    {
        FeatureWeights *cs = NUMpvector (0, nseeds);
        for (long y = 0; y <= nseeds; y++)
        {
            cs[y] = FeatureWeights_create(pp->nx);
            if (!cs[y])
            {
                while (y--) forget(cs[y - 1]);
                return(NULL);
            }

        }

        for (long x = 1; x <= pp->nx; x++)
            cs[nseeds]->fweights->data[1][x] = pivot;

        results[nseeds] = FeatureWeights_evaluate(cs[nseeds], nn, pp, c, k, d);

        while (range > 0 && results[nseeds] < stop)
        {
            long best = nseeds;
            if (!Melder_progress1(range * progress, L"")) break;

            if (mode == 2)
            {
                for (long x = 1; x <= pp->nx; x++)
                {
                    for (long y = 0; y < nseeds; y++)
                    {
                        cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
                                                      OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
                        results[y] = FeatureWeights_evaluate(cs[y], nn, pp, c, k, d);
                    }
                    for (long q = 0; q < nseeds; q++)
                        if (results[q] > results[best]) best = q;

                    if (results[best] > results[nseeds])
                    {
                        for (long x = 1; x <= pp->nx; x++)
                            cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
                        results[nseeds] = results[best];
                    }
                }
            }
            else
            {
                for (long y = 0; y < nseeds; y++)
                {
                    for (long x = 1; x <= pp->nx; x++)
                    {
                        cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
                                                      OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
                    }
                    results[y] = FeatureWeights_evaluate(cs[y], nn, pp, c, k, d);
                }

                for (long q = 0; q < nseeds; q++)
                    if (results[q] > results[best]) best = q;

                if (results[best] > results[nseeds])
                {
                    for (long x = 1; x <= pp->nx; x++)
                        cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
                    results[nseeds] = results[best];
                }
            }
            range -= alfa;
        }

        for (long y = 0; y < nseeds; y++)
            forget(cs[y]);
		NUMpvector_free (cs, 0);

        Melder_progress1(1.0, NULL);
    	NUMdvector_free (results, 0);
        return(cs[nseeds]);
    }
    NUMdvector_free (results, 0);
    return(NULL);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Evaluate feature weights, wrapper aux.                                                  //
/////////////////////////////////////////////////////////////////////////////////////////////

double FeatureWeights_evaluate      // Obsolete - use *_EvaluateWithTestSet
                                    //            instead
(
///////////////////////////////
// Parameters                //
///////////////////////////////

    FeatureWeights fws,             // Weights to evaluate
                                    //
    KNN nn,                         // Classifier
                                    //
    Pattern pp,                     // test pattern
                                    //
    Categories c,                   // test categories
                                    //
    long k,                         // k(!)
                                    //
    int d                           // distance weighting
                                    //
)

{
    double hits = 0;
    Categories o = KNN_classifyToCategories(nn, pp, fws, k, d);

    for (long y = 1; y <= o->size; y++)
        if (FRIENDS(o->item[y], c->item[y])) hits++;

    hits /= o->size;
    forget(o);

    return(hits);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights according to the RELIEF-F algorithm                             //
/////////////////////////////////////////////////////////////////////////////////////////////

FeatureWeights FeatureWeights_computeRELIEF
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern pp,         // source pattern
                        //
    Categories c,       // source categories
                        //
    long k              // k(!)
                        //
)

{
    Pattern p = Data_copy(pp);
    FeatureWeights me = FeatureWeights_create(p->nx);
    if (me)
    {
        /////////////////////////////////
        // Initial weights <- 0        //
        /////////////////////////////////

        for (long i = 1; i <= p->nx; i++)
        {
            (my fweights)->data[1][i] = 0;
        }

        /////////////////////////////////
        // Normalization               //
        /////////////////////////////////

        double *min = NUMdvector (0, p->nx - 1), *max = NUMdvector (0, p->nx - 1);
        for (long x = 1; x <= p->nx; x++)
        {
            max[x] = p->z[1][x];
            min[x] = max[x];
        }

        for (long y = 1; y <= p->ny; y++)
        {
            for (long x = 1; x <= p->nx; x++)
            {
                if (p->z[y][x] > max[x]) max[x] = p->z[y][x];
                if (p->z[y][x] < min[x]) min[x] = p->z[y][x];
            }
        }

        double *alfa = NUMdvector (0, p->nx - 1);
        for (long x = 1; x <= p->nx; x++)
        {
            alfa[x] = max[x] - min[x];
        }

        for (long y = 1; y <= p->ny; y++)
        {
            for (long x = 1; x <= p->nx; x++)
            {
                if (alfa[x])
                {
                    p->z[y][x] = (p->z[y][x] - min[x]) / alfa[x];
                }
                else
                {
                    p->z[y][x] = 0;
                }
            }
        }

        /////////////////////////////////
        // Computing prior class probs //
        /////////////////////////////////

        double *priors = NUMdvector (0, c->size - 1);// Worst-case allocations
        long *classes = NUMlvector (0, c->size - 1);//
        long *enemies = NUMlvector (0, c->size - 1);//
        long *friends = NUMlvector (0, c->size - 1);//
        long nclasses = FeatureWeights_computePriors(c, classes, priors);
		Melder_assert (nclasses >= 2);

        /////////////////////////////////
        // Updating the w.vector       //
        /////////////////////////////////

        for (long y = 1; y <= p->ny; y++)
        {
            if (!Melder_progress1(1 - (double) y / p->ny, L"")) break;

            long nfriends = KNN_kFriends(p, p, c, y, k, friends);
            long nenemies = KNN_kUniqueEnemies(p, p, c, y, nclasses - 1, enemies);

            if (nfriends && nenemies)
            {
                double *classps = NUMdvector (0, nenemies - 1);
                for (long eq = 0; eq < nenemies; eq++)
                    for (long iq = 0; iq < nclasses; iq++)
                        if (FRIENDS(c->item[enemies[eq]], c->item[classes[iq]]))
                        {
                            classps[eq] = priors[iq];
                            break;
                        }

                for (long x = 1; x <= p->nx; x++)
                {
                    double p1 = 0;
                    double p2 = 0;

                    for (long ec = 0; ec < nfriends; ec++)
                    {
                        p1 += fabs(p->z[y][x] - p->z[friends[ec]][x]) / (p->ny * nfriends);
                    }

                    for (long ec = 0; ec < nenemies; ec++)
                    {
                        p2 += (fabs(p->z[y][x] - p->z[enemies[ec]][x]) * classps[ec]) / p->ny;
                    }

                    (my fweights)->data[1][x] = (my fweights)->data[1][x] - p1 + p2;
                }
                NUMdvector_free (classps, 0);
            }
        }
        Melder_progress1(1.0, NULL);
        NUMdvector_free (min, 0);
        NUMdvector_free (max, 0);
        NUMdvector_free (alfa, 0);
        NUMdvector_free (priors, 0);
        NUMlvector_free (classes, 0);
        NUMlvector_free (enemies, 0);
        NUMlvector_free (friends, 0);
        forget(p);
        return(me);
    }
    else
    {
        return(NULL);
    }
}

/* End of file FeatureWeights.c */
