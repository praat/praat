/* FeatureWeights.cpp
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2012,2015,2016,2017 Paul Boersma
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
    our structDaata :: v_info ();
    MelderInfo_writeLine (U"Number of weights: ", our fweights -> numberOfColumns);
}

Thing_implement (FeatureWeights, Daata, 0);

/////////////////////////////////////////////////////////////////////////////////////////////
// Creation...    //
/////////////////////////////////////////////////////////////////////////////////////////////

autoFeatureWeights FeatureWeights_create
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    integer nweights       // number of weights
)

{
	try {
		autoFeatureWeights me = Thing_new (FeatureWeights);
		my fweights = TableOfReal_create (1, nweights);
		for (integer i = 1; i <= nweights; i ++) {
			my fweights -> data [1] [i] = 1;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FeatureWeights not created.");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute prior probabilities                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////

integer FeatureWeights_computePriors
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Categories c,       // source categories
                        //
    integer * indices,  // Out: instances indices ..
                        //  
    double * priors     // Out: .. and their prior probabilities
                        //
)

{
    integer nc = 0;
    for (integer y = 1; y <= c->size; y ++)
    {
        integer ifriend = -1;
        for (integer sc = 0; sc < nc; sc ++)
            if (FeatureWeights_areFriends (c->at [y], c->at [indices [sc]])) ifriend = sc;

        if (ifriend < 0)
        {
            indices [nc] = y;
            priors [nc] = 1;
            nc ++;
        }
        else
        {
            priors [ifriend] ++;
        }
    }
    for (integer q = 0; q < nc; q++) priors [q] /= c->size;
    return nc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////

autoFeatureWeights FeatureWeights_compute           // Obsolete
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList pp, // Source pattern
                    //
    Categories c,   // Source categories
                    //
    integer k       // k(!)
)

{
    return FeatureWeights_computeRELIEF (pp, c, k);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights (wrapper), evaluate using folding                               //
/////////////////////////////////////////////////////////////////////////////////////////////

autoFeatureWeights FeatureWeights_computeWrapperInt
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN me,         // Classifier
                    //
    integer k,      // k(!)
                    //
    int d,          // distance weighting
                    //
    integer nseeds, // the number of seeds
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
	if (! me) return autoFeatureWeights();

	try {
		double pivot = 0.5;
		double range = 0.5;
		autoNUMvector <double> results ((integer) 0, nseeds);

		autoThingVector <structFeatureWeights> cs ((integer) 0, nseeds);
		for (integer y = 0; y <= nseeds; y++) {
			cs [y] = FeatureWeights_create (my input -> nx);
		}

		for (integer x = 1; x <= my input -> nx; x ++)
			cs [nseeds] -> fweights -> data [1] [x] = pivot;

		results [nseeds] = KNN_evaluate (me, cs [nseeds].get(), k, d, emode);

		while (range > 0 && results [nseeds] < stop)
		{
			integer best = nseeds;

			if (mode == 2)
			{
				for (integer x = 1; x <= (my input)->nx; x ++)
				{
					for (integer y = 0; y < nseeds; y ++)
					{
						cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
													  OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
						results[y] = KNN_evaluate (me, cs [y].get(), k, d, emode);
					}
					for (integer q = 0; q < nseeds; q++)
						if (results[q] > results[best]) best = q;

					if (results[best] > results[nseeds])
					{
						for (integer x = 1; x <= (my input)->nx; x++)   // HELP FIXME the same index for an inner and an outer loop!!!
							cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
						results[nseeds] = results[best];
					}
				}
			}
			else
			{
				for (integer y = 0; y < nseeds; y++)
				{
					for (integer x = 1; x <= (my input)->nx; x++)
					{
						cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
													  OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
					}
					results[y] = KNN_evaluate (me, cs [y].get(), k, d, emode);
				}

				for (integer q = 0; q < nseeds; q++)
					if (results[q] > results[best]) best = q;

				if (results[best] > results[nseeds])
				{
					for (integer x = 1; x <= (my input)->nx; x++)
						cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
					results[nseeds] = results[best];
				}
			}
			range -= alfa;
		}

		autoFeatureWeights result = cs [nseeds].move();
		return result;
	} catch (MelderError) {
		Melder_throw (U"FeatureWeights: wrapper not computed.");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights (wrapper), evaluate using separate test set                     //
/////////////////////////////////////////////////////////////////////////////////////////////

autoFeatureWeights FeatureWeights_computeWrapperExt
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    KNN nn,         // Classifier
                    //
    PatternList pp, // test pattern
                    //
    Categories c,   // test categories
                    //
    integer k,      // k(!)
                    //
    int d,          // distance weighting
                    //
    integer nseeds, // the number of seeds
                    //
    double alfa,    // shrinkage factor
                    //
    double stop,    // stop at
                    //
    int mode        // mode (co/serial)
                    //
)

{
	if (! nn) return autoFeatureWeights();

	try {
		double pivot = 0.5;
		double range = 0.5;
		autoNUMvector <double> results ((integer) 0, nseeds);

		autoThingVector <structFeatureWeights> cs ((integer) 0, nseeds);
		for (integer y = 0; y <= nseeds; y++) {
			cs [y] = FeatureWeights_create (pp -> nx);
		}

		for (integer x = 1; x <= pp -> nx; x ++)
			cs [nseeds] -> fweights -> data [1] [x] = pivot;

		results [nseeds] = FeatureWeights_evaluate (cs [nseeds].get(), nn, pp, c, k, d);

		while (range > 0 && results [nseeds] < stop)
		{
			integer best = nseeds;

			if (mode == 2)
			{
				for (integer x = 1; x <= pp->nx; x++)
				{
					for (integer y = 0; y < nseeds; y++)
					{
						cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, (cs[nseeds]->fweights)->data[1][x] - range),
													  OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
						results[y] = FeatureWeights_evaluate (cs[y].get(), nn, pp, c, k, d);
					}
					for (integer q = 0; q < nseeds; q++)
						if (results[q] > results[best]) best = q;

					if (results[best] > results[nseeds])
					{
						for (integer x = 1; x <= pp->nx; x++)   // BUG: a loop over x inside a loop over x; just hope mode is never 2
							cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
						results[nseeds] = results[best];
					}
				}
			}
			else
			{
				for (integer y = 0; y < nseeds; y++)
				{
					for (integer x = 1; x <= pp->nx; x++)
					{
						cs[y]->fweights->data[1][x] = NUMrandomUniform(OlaMAX(0, cs[nseeds]->fweights->data[1][x] - range),
													  OlaMIN(1, cs[nseeds]->fweights->data[1][x] + range));
					}
					results[y] = FeatureWeights_evaluate (cs [y].get(), nn, pp, c, k, d);
				}

				for (integer q = 0; q < nseeds; q++)
					if (results[q] > results[best]) best = q;

				if (results[best] > results[nseeds])
				{
					for (integer x = 1; x <= pp->nx; x++)
						cs[nseeds]->fweights->data[1][x] = cs[best]->fweights->data[1][x];
					results[nseeds] = results[best];
				}
			}
			range -= alfa;
		}

		autoFeatureWeights result = cs [nseeds].move();
		return result;
	} catch (MelderError) {
		Melder_throw (U"FeatureWeights: wrapper not computed.");
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
    PatternList pp,                     // test pattern
                                    //
    Categories c,                   // test categories
                                    //
    integer k,                      // k(!)
                                    //
    int d                           // distance weighting
                                    //
)

{
	try {
		autoCategories o = KNN_classifyToCategories (nn, pp, fws, k, d);
		double hits = 0.0;
		for (integer y = 1; y <= o->size; y ++)
			if (FeatureWeights_areFriends (o->at [y], c->at [y])) hits ++;
		hits /= o->size;
		return hits;
	} catch (MelderError) {
		throw;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compute feature weights according to the RELIEF-F algorithm                             //
/////////////////////////////////////////////////////////////////////////////////////////////

autoFeatureWeights FeatureWeights_computeRELIEF
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList pp,     // source pattern
                        //
    Categories c,       // source categories
                        //
    integer k           // k(!)
                        //
)

{
	autoPatternList p = Data_copy (pp);
	autoFeatureWeights me = FeatureWeights_create (p -> nx);

	/////////////////////////////////
	// Initial weights <- 0        //
	/////////////////////////////////

	for (integer i = 1; i <= p->nx; i++) {
		my fweights -> data [1] [i] = 0.0;
	}

	/////////////////////////////////
	// Normalization               //
	/////////////////////////////////

	autoNUMvector <double> min ((integer) 0, p->nx - 1);
	autoNUMvector <double> max ((integer) 0, p->nx - 1);
	for (integer x = 1; x <= p -> nx; x ++) {
		max [x] = p -> z [1] [x];   // BUG: this will just crash because of array index out of bounds
		min [x] = max [x];
	}

	for (integer y = 1; y <= p -> ny; y ++) {
		for (integer x = 1; x <= p->nx; x++) {
			if (p->z[y][x] > max[x]) max[x] = p->z[y][x];
			if (p->z[y][x] < min[x]) min[x] = p->z[y][x];
		}
	}

	autoNUMvector <double> alfa ((integer) 0, p -> nx - 1);
	for (integer x = 1; x <= p -> nx; x ++) {
		alfa [x] = max [x] - min [x];   // BUG: this will just crash because of array index out of bounds
	}

	for (integer y = 1; y <= p->ny; y++) {
		for (integer x = 1; x <= p->nx; x++) {
			if (alfa [x] != 0.0) {
				p->z[y][x] = (p->z[y][x] - min[x]) / alfa[x];
			} else {
				p->z[y][x] = 0.0;
			}
		}
	}

	/////////////////////////////////
	// Computing prior class probs //
	/////////////////////////////////

	autoNUMvector <double> priors ((integer) 0, c->size - 1);   // worst-case allocations
	autoNUMvector <integer> classes ((integer) 0, c->size - 1);//
	autoNUMvector <integer> enemies ((integer) 0, c->size - 1);//
	autoNUMvector <integer> friends ((integer) 0, c->size - 1);//
	integer nclasses = FeatureWeights_computePriors (c, classes.peek(), priors.peek());
	Melder_assert (nclasses >= 2);

	/////////////////////////////////
	// Updating the w.vector       //
	/////////////////////////////////

	for (integer y = 1; y <= p -> ny; y ++) {

		integer nfriends = KNN_kFriends (p.get(), p.get(), c, y, k, friends.peek());
		integer nenemies = KNN_kUniqueEnemies (p.get(), p.get(), c, y, nclasses - 1, enemies.peek());

		if (nfriends && nenemies) {
			autoNUMvector <double> classps ((integer) 0, nenemies - 1);
			for (integer eq = 0; eq < nenemies; eq ++) {
				for (integer iq = 0; iq < nclasses; iq ++) {
					if (FeatureWeights_areFriends (c->at [enemies [eq]], c->at [classes [iq]])) {
						classps [eq] = priors [iq];
						break;
					}
				}
			}
			for (integer x = 1; x <= p -> nx; x ++) {
				double p1 = 0.0;
				double p2 = 0.0;
				for (integer ec = 0; ec < nfriends; ec ++) {
					p1 += fabs (p -> z [y] [x] - p -> z [friends [ec]] [x]) / (p -> ny * nfriends);
				}
				for (integer ec = 0; ec < nenemies; ec++) {
					p2 += (fabs (p->z[y][x] - p->z[enemies[ec]][x]) * classps[ec]) / p->ny;
				}
				my fweights -> data [1] [x] = my fweights -> data [1] [x] - p1 + p2;
			}
		}
	}
	return me;
}

/* End of file FeatureWeights.cpp */
