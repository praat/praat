
/* Pattern_to_Categories_cluster.c
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

/* $URL: svn://pegasos.dyndns.biz/praat/trunk/kNN/Pattern_to_Categories_cluster.c $
 * $Rev: 137 $
 * $Author: stix $
 * $Date: 2008-08-10 19:34:07 +0200 (Sun, 10 Aug 2008) $
 * $Id: Pattern_to_Categories_cluster.c 137 2008-08-10 17:34:07Z stix $
 */

/*
 * os 20070529 Initial release
 */

#include "Pattern_to_Categories_cluster.h"

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
    KNN knn = KNN_create();
    if (knn)
    {
        if (m < 0) m = 1000;
        if (k > p->ny) k = p->ny;
        if (k < 1) k = 1;
        if (s > 1 || s <= 0) s = 0.0001;
        if (p->ny % k) if (s > (double) (p->ny / k) / (double) (p->ny / k + 1)) s = (double) (p->ny / k) / (double) (p->ny / k + 1);

        double progress = m;
        double sizes[k + 1];
        long seeds[k];

        Pattern centroids = Pattern_create(k, p->nx);
        Categories categories = Categories_sequentialNumbers(k);

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
                            if (seeds[fc] == ys)
                            {
                                friend = 1;
                                break;
                            }
                        }
                    }
                }

                seeds[nfriends++] = ys;

                for (long x = 1; x <= centroids->nx; x++)
                {
                    centroids->z[y][x] = p->z[ys][x];
                }
            }
            do
            {
                delta = 0;
                KNN_learn(knn, centroids, categories, kOla_REPLACE, kOla_SEQUENTIAL);
                Categories interim = KNN_classifyToCategories(knn, p, fws, 1, kOla_FLAT_VOTING);

                for (long x = 1; x <= k; x++)
                {
                    sizes[x] = 0;
                }

                for (long yp = 1; yp <= categories->size; yp++)
                {
                    double alfa = 1;
                    double beta[centroids->nx];

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
                                if (alfa == 1)
                                {
                                    centroids->z[yp][x] = p->z[ys][x];
                                }
                                else
                                {
                                    centroids->z[yp][x] += (p->z[ys][x] - centroids->z[yp][x]) / alfa;
                                }
                            }
                            sizes[yp]++;
                            alfa++;
                        }
                    }

                    for (long x = 1; x <= centroids->nx; x++)
                    {
                        delta += fabs(beta[x] - centroids->z[yp][x]);
                    }
                }
                forget(interim);
            }
            while (delta);

            double smax = sizes[1];
            double smin = sizes[1];

            for (long x = 1; x <= k; x++)
            {
                if (smax < sizes[x]) smax = sizes[x];
                if (smin > sizes[x]) smin = sizes[x];
            }

            sizes[0] = smin / smax;
            m--;
        }
        while (sizes[0] < s && m > 0);

        Melder_progress1(1.0, NULL);

        Categories output = KNN_classifyToCategories(knn, p, fws, 1, kOla_FLAT_VOTING);

        forget(centroids);
        forget(categories);
        forget(knn);

        return(output);
    }
    else
    {
        return(NULL);
    }
}

/* End of file Pattern_to_Categories_cluster.c */

