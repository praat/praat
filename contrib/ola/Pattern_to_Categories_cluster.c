/* Pattern_to_Categories_cluster.c
 *
 * Copyright (C) 2007-2008 Ola So"der, 2010-2011 Paul Boersma
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
 * os 20070529 initial release
 * pb 20100606 removed some array-creations-on-the-stack
 * pb 20100606 corrected some arrary-index-out-of-bounds errors
 * pb 20100606 corrected some memory leaks
 * pb 2011/03/08 tried to repair some of the header file chaos (several procedures here should be in KNN.c instead)
 */

#include "Pattern_to_Categories_cluster.h"
#include "KNN.h"   // BUG

/////////////////////////////////////////////////////////////////////////////////////////////
// Pattern_to_Categories_cluster                                                                              //
/////////////////////////////////////////////////////////////////////////////////////////////

Categories Pattern_to_Categories_cluster
(
    ///////////////////////////////
    // Parameters                //
    ///////////////////////////////

    Pattern p,              // source
                            //
    FeatureWeights fws,     // feature weights
                            //
    long k,                 // k(!)
                            //
    double s,               // clustersize constraint 0 < s <= 1
                            //
    long m                  // reseed maximum
                            //
)

{
    Categories categories = NULL, output = NULL;
    KNN knn = NULL;
    Pattern centroids = NULL;
    double *sizes = NULL, *beta = NULL;
    long *seeds = NULL;

	categories = Categories_sequentialNumbers (k); cherror
    if (k == p->ny)
        return categories;

    knn = KNN_create(); cherror
    if(p->ny % k) 
        if (s > (double) (p->ny / k) / (double) (p->ny / k + 1)) 
            s = (double) (p->ny / k) / (double) (p->ny / k + 1);

    double progress = m;
    sizes = NUMdvector (0, k); cherror
    seeds = NUMlvector (0, k); cherror

    centroids = Pattern_create (k, p->nx);
    beta = NUMdvector (0, centroids->nx);

    do
    {
        double delta;
        long nfriends  = 0;
        if (!Melder_progress1(1 - (progress - m) / progress, L"")) break;

        for (long y = 1; y <= centroids->ny; y++)
        {
            int friend = 1;
            long ys = (long) lround(NUMrandomUniform(1, p->ny));

            if (nfriends)
            {
                while (friend)
                {
                    ys = (long) lround(NUMrandomUniform(1, p->ny));
                    for (long fc = 0; fc < nfriends; fc++)
                    {
                        friend = 0;
                        Melder_assert (fc < k);
                        if (seeds [fc] == ys)
                        {
                            friend = 1;
                            break;
                        }
                    }
                }
            }
			Melder_assert (nfriends <= k);
            seeds [nfriends++] = ys;

            for (long x = 1; x <= centroids->nx; x++)
                centroids->z[y][x] = p->z[ys][x];
        }
        do
        {
            delta = 0;
            KNN_learn (knn, centroids, categories, kOla_REPLACE, kOla_SEQUENTIAL);
            Categories interim = KNN_classifyToCategories (knn, p, fws, 1, kOla_FLAT_VOTING);

            for (long x = 1; x <= k; x++)
                sizes [x] = 0;

            for (long yp = 1; yp <= categories->size; yp++)
            {
                double alfa = 1;
                Melder_assert (yp <= centroids->ny);

                for (long x = 1; x <= centroids->nx; x++)
                {
                    beta[x] = centroids->z[yp][x];
                }

                for (long ys = 1; ys <= interim->size; ys++)
                {
                    if (FRIENDS(categories->item[yp], interim->item[ys]))
                    {
                        for (long x = 1; x <= p->nx; x++)
                        {
                        	Melder_assert (ys <= p->ny);
                            if (alfa == 1)
                            {
                                centroids->z[yp][x] = p->z[ys][x];
                            }
                            else
                            {
                                centroids->z[yp][x] += (p->z[ys][x] - centroids->z[yp][x]) / alfa;
                            }
                        }
                        Melder_assert (yp <= k);
                        sizes [yp] ++;
                        alfa++;
                    }
                }

                for (long x = 1; x <= centroids->nx; x++)
                {
                    delta += fabs (beta[x] - centroids->z[yp][x]);
                }
            }
            forget (interim);
        }
        while (delta);

        double smax = sizes [1];
        double smin = sizes [1];

        for (long x = 1; x <= k; x++)
        {
            if (smax < sizes [x]) smax = sizes [x];
            if (smin > sizes [x]) smin = sizes [x];
        }

        sizes [0] = smin / smax;
        --m;
    }
    while (sizes[0] < s && m > 0);

    Melder_progress1(1.0, NULL);

    output = KNN_classifyToCategories (knn, p, fws, 1, kOla_FLAT_VOTING); cherror

end:
    forget (centroids);
    forget (categories);
    forget (knn);
    NUMdvector_free (sizes, 0);
    NUMdvector_free (beta, 0);
    NUMlvector_free (seeds, 0);
    iferror return NULL;
    return output;
}

/* End of file Pattern_to_Categories_cluster.c */
