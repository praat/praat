/* Pattern_to_Categories_cluster.cpp
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2011,2016,2017 Paul Boersma
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
 * os 20070529 initial release
 * pb 20100606 removed some array-creations-on-the-stack
 * pb 20100606 corrected some arrary-index-out-of-bounds errors
 * pb 20100606 corrected some memory leaks
 * pb 2011/03/08 tried to repair some of the header file chaos (several procedures here should be in KNN.c instead)
 * pb 2011/04/13 C++
 */

#include "Pattern_to_Categories_cluster.h"
#include "KNN.h"   // BUG

/////////////////////////////////////////////////////////////////////////////////////////////
// Pattern_to_Categories_cluster                                                                              //
/////////////////////////////////////////////////////////////////////////////////////////////

autoCategories PatternList_to_Categories_cluster
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    PatternList p,          // source
                            //
    FeatureWeights fws,     // feature weights
                            //
    integer k,              // k(!)
                            //
    double s,               // clustersize constraint 0 < s <= 1
                            //
    integer m               // reseed maximum
                            //
)

{
		autoCategories categories = Categories_createWithSequentialNumbers (k);
		if (k == p->ny)
			return categories;

		autoKNN knn = KNN_create();
		if (p -> ny % k)
			if (s > (double) (p -> ny / k) / (double) (p -> ny / k + 1))   // FIXME check whether integer division is correct
				s = (double) (p -> ny / k) / (double) (p -> ny / k + 1);

		double progress = m;
		autoNUMvector <double> sizes ((integer) 0, k);
		autoNUMvector <integer> seeds ((integer) 0, k);

		autoPatternList centroids = PatternList_create (k, p -> nx);
		autoNUMvector <double> beta ((integer) 0, centroids -> nx);

		do
		{
			double delta;
			integer nfriends  = 0;
			Melder_progress (1 - (progress - m) / progress, U"");

			for (integer y = 1; y <= centroids->ny; y++)
			{
				int ifriend = 1;
				integer ys = Melder_iround (NUMrandomUniform (1, p -> ny));   // BUG probably wrong (the edges have half-probability)

				if (nfriends)
				{
					while (ifriend)
					{
						ys = Melder_iround (NUMrandomUniform (1, p->ny));   // BUG probably wrong (the edges have half-probability)
						for (integer fc = 0; fc < nfriends; fc++)
						{
							ifriend = 0;
							Melder_assert (fc < k);
							if (seeds [fc] == ys)
							{
								ifriend = 1;
								break;
							}
						}
					}
				}
				Melder_assert (nfriends <= k);
				seeds [nfriends++] = ys;

				for (integer x = 1; x <= centroids->nx; x++)
					centroids->z[y][x] = p->z[ys][x];
			}
			do
			{
				delta = 0;
				KNN_learn (knn.get(), centroids.get(), categories.get(), kOla_REPLACE, kOla_SEQUENTIAL);
				autoCategories interim = KNN_classifyToCategories (knn.get(), p, fws, 1, kOla_FLAT_VOTING);

				for (integer x = 1; x <= k; x ++)
					sizes [x] = 0;

				for (integer yp = 1; yp <= categories->size; yp ++)
				{
					double alfa = 1;
					Melder_assert (yp <= centroids -> ny);

					for (integer x = 1; x <= centroids -> nx; x ++)
					{
						beta [x] = centroids -> z [yp] [x];
					}

					for (integer ys = 1; ys <= interim->size; ys ++)
					{
						if (FeatureWeights_areFriends (categories->at [yp], interim->at [ys]))
						{
							for (integer x = 1; x <= p -> nx; x ++)
							{
								Melder_assert (ys <= p -> ny);
								if (alfa == 1)
								{
									centroids -> z [yp] [x] = p -> z [ys] [x];
								}
								else
								{
									centroids -> z [yp] [x] += (p -> z [ys] [x] - centroids -> z [yp] [x]) / alfa;
								}
							}
							Melder_assert (yp <= k);
							sizes [yp] ++;
							alfa ++;
						}
					}

					for (integer x = 1; x <= centroids -> nx; x ++)
					{
						delta += fabs (beta [x] - centroids -> z [yp] [x]);
					}
				}
			}
			while (delta != 0.0);

			double smax = sizes [1];
			double smin = sizes [1];

			for (integer x = 1; x <= k; x++)
			{
				if (smax < sizes [x]) smax = sizes [x];
				if (smin > sizes [x]) smin = sizes [x];
			}

			sizes [0] = smin / smax;
			-- m;
		}
		while (sizes[0] < s && m > 0);

		autoCategories output = KNN_classifyToCategories (knn.get(), p, fws, 1, kOla_FLAT_VOTING);

		return output;
}

/* End of file Pattern_to_Categories_cluster.cpp */
