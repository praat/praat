/* FeatureWeights.cpp
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2012 Paul Boersma
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
 * os 2007/05/29 Initial release
 * pb 2010/06/06 removed some array-creations-on-the-stack
 * pb 2011/03/08 tried to repair some of the header file chaos (several procedures here should be in KNN.c instead)
 * pb 2011/04/12 C++
 */

#include "FeatureWeights.h"
#include "KNN.h"   // BUG

#include "oo_DESTROY.h"
#include "FeatureWeights_def.h"
#include "oo_COPY.h"
#include "FeatureWeights_def.h"
#include "oo_EQUAL.h"
#include "FeatureWeights_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
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

void structFeatureWeights :: v_info ()
{
    this -> structData :: v_info ();
    MelderInfo_writeLine (L"Number of weights: ", Melder_integer (fweights -> numberOfColumns));
}

Thing_implement (FeatureWeights, Data, 0);

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
	try {
		autoFeatureWeights me = Thing_new (FeatureWeights);
		my fweights = TableOfReal_create (1, nweights);
		for (long i = 1; i <= nweights; i ++) {
			my fweights -> data [1] [i] = 1;
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FeatureWeights not created.");
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
        long ifriend = -1;
        for (long sc = 0; sc < nc; sc++)
            if (FeatureWeights_areFriends ((SimpleString) c->item[y], (SimpleString) c->item[indices[sc]])) ifriend = sc;

        if (ifriend < 0)
        {
            indices[nc] = y;
            priors[nc] = 1;
            nc++;
        }
        else
        {
            priors[ifriend]++;
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
	if (! me) return NULL;

	try {
		double pivot = 0.5;
		double range = 0.5;
		autoNUMvector <double> results (0L, nseeds);

		autoThingVector <FeatureWeights> cs (0L, nseeds);
		for (long y = 0; y <= nseeds; y++) {
			cs [y] = FeatureWeights_create (my input -> nx);
		}

		for (long x = 1; x <= my input -> nx; x ++)
			cs [nseeds] -> fweights -> data [1] [x] = pivot;

		results [nseeds] = KNN_evaluate (me, cs [nseeds], k, d, emode);

		while (range > 0 && results [nseeds] < stop)
		{
			long best = nseeds;

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
					results[y] = KNN_evaluate (me, cs [y], k, d, emode);
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

		FeatureWeights result = cs [nseeds];
		cs [nseeds] = NULL;   // prevent destruction
		return result;
	} catch (MelderError) {
		Melder_throw ("FeatureWeights: wrapper not computed.");
	}
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
	if (nn == NULL) return NULL;

	try {
		double pivot = 0.5;
		double range = 0.5;
		autoNUMvector <double> results (0L, nseeds);

		autoThingVector <FeatureWeights> cs (0L, nseeds);
		for (long y = 0; y <= nseeds; y++) {
			cs [y] = FeatureWeights_create (pp -> nx);
		}

		for (long x = 1; x <= pp -> nx; x ++)
			cs [nseeds] -> fweights -> data [1] [x] = pivot;

		results [nseeds] = FeatureWeights_evaluate (cs [nseeds], nn, pp, c, k, d);

		while (range > 0 && results [nseeds] < stop)
		{
			long best = nseeds;

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
					results[y] = FeatureWeights_evaluate (cs [y], nn, pp, c, k, d);
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

		FeatureWeights result = cs [nseeds];
		cs [nseeds] = NULL;   // prevent destruction
		return result;
	} catch (MelderError) {
		Melder_throw ("FeatureWeights: wrapper not computed.");
	}
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
	try {
		autoCategories o = KNN_classifyToCategories (nn, pp, fws, k, d);
		double hits = 0;
		for (long y = 1; y <= o->size; y++)
			if (FeatureWeights_areFriends ((SimpleString) o -> item [y], (SimpleString) c -> item [y])) hits ++;
		hits /= o -> size;
		return hits;
	} catch (MelderError) {
		throw;
		return 0;
	}
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
	autoPattern p = (Pattern) Data_copy (pp);
	autoFeatureWeights me = FeatureWeights_create (p -> nx);

	/////////////////////////////////
	// Initial weights <- 0        //
	/////////////////////////////////

	for (long i = 1; i <= p->nx; i++) {
		my fweights -> data [1] [i] = 0.0;
	}

	/////////////////////////////////
	// Normalization               //
	/////////////////////////////////

	autoNUMvector <double> min (0L, p->nx - 1);
	autoNUMvector <double> max (0L, p->nx - 1);
	for (long x = 1; x <= p -> nx; x ++) {
		max [x] = p -> z [1] [x];   // BUG: this will just crash because of array index out of bounds
		min [x] = max [x];
	}

	for (long y = 1; y <= p -> ny; y ++) {
		for (long x = 1; x <= p->nx; x++) {
			if (p->z[y][x] > max[x]) max[x] = p->z[y][x];
			if (p->z[y][x] < min[x]) min[x] = p->z[y][x];
		}
	}

	autoNUMvector <double> alfa (0L, p -> nx - 1);
	for (long x = 1; x <= p -> nx; x ++) {
		alfa [x] = max [x] - min [x];   // BUG: this will just crash because of array index out of bounds
	}

	for (long y = 1; y <= p->ny; y++) {
		for (long x = 1; x <= p->nx; x++) {
			if (alfa [x]) {
				p->z[y][x] = (p->z[y][x] - min[x]) / alfa[x];
			} else {
				p->z[y][x] = 0;
			}
		}
	}

	/////////////////////////////////
	// Computing prior class probs //
	/////////////////////////////////

	autoNUMvector <double> priors (0L, c->size - 1);   // worst-case allocations
	autoNUMvector <long> classes (0L, c->size - 1);//
	autoNUMvector <long> enemies (0L, c->size - 1);//
	autoNUMvector <long> friends (0L, c->size - 1);//
	long nclasses = FeatureWeights_computePriors (c, classes.peek(), priors.peek());
	Melder_assert (nclasses >= 2);

	/////////////////////////////////
	// Updating the w.vector       //
	/////////////////////////////////

	for (long y = 1; y <= p -> ny; y ++) {

		long nfriends = KNN_kFriends (p.peek(), p.peek(), c, y, k, friends.peek());
		long nenemies = KNN_kUniqueEnemies (p.peek(), p.peek(), c, y, nclasses - 1, enemies.peek());

		if (nfriends && nenemies) {
			autoNUMvector <double> classps (0L, nenemies - 1);
			for (long eq = 0; eq < nenemies; eq++) {
				for (long iq = 0; iq < nclasses; iq++) {
					if (FeatureWeights_areFriends ((SimpleString) c->item[enemies[eq]], (SimpleString) c->item[classes[iq]])) {
						classps[eq] = priors[iq];
						break;
					}
				}
			}
			for (long x = 1; x <= p->nx; x++) {
				double p1 = 0.0;
				double p2 = 0.0;
				for (long ec = 0; ec < nfriends; ec++) {
					p1 += fabs(p->z[y][x] - p->z[friends[ec]][x]) / (p->ny * nfriends);
				}
				for (long ec = 0; ec < nenemies; ec++) {
					p2 += (fabs(p->z[y][x] - p->z[enemies[ec]][x]) * classps[ec]) / p->ny;
				}
				my fweights -> data [1] [x] = my fweights -> data [1] [x] - p1 + p2;
			}
		}
	}
	return me.transfer();
}

/* End of file FeatureWeights.cpp */
