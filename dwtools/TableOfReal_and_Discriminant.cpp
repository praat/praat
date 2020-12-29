/* TableOfReal_and_Discriminant.cpp
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "TableOfReal_and_Discriminant.h"

autoDiscriminant TableOfReal_to_Discriminant (TableOfReal me) {
	try {
		autoDiscriminant thee = Thing_new (Discriminant);
		const integer dimension = my numberOfColumns;

		Melder_require (NUMdefined (my data.get()),
			U"There should be no undefined elements in the table.");
		Melder_require (TableOfReal_hasRowLabels (me),
			U"All rows should be labeled.");

		autoTableOfReal mew = TableOfReal_sortOnlyByRowLabels (me);
		if (! TableOfReal_hasColumnLabels (mew.get()))
			TableOfReal_setSequentialColumnLabels (mew.get(), 0, 0, U"c", 1, 1);

		thy groups = TableOfReal_to_SSCPList_byLabel (mew.get());
		thy total = TableOfReal_to_SSCP (mew.get(), 0, 0, 0, 0);

		if ((thy numberOfGroups = thy groups -> size) < 2)
			Melder_throw (U"Number of groups should be greater than one.");

		TableOfReal_centreColumns_byRowLabel (mew.get());

		// Overall centroid and apriori probabilities and costs.

		autoVEC centroid = zero_VEC (dimension);
		autoMAT between = zero_MAT (thy numberOfGroups, dimension);
		thy aprioriProbabilities = raw_VEC (thy numberOfGroups);

		longdouble sum = 0.0;
		for (integer k = 1; k <= thy numberOfGroups; k ++) {
			const SSCP m = thy groups->at [k];
			const double scale = SSCP_getNumberOfObservations (m);
			centroid.all()  +=  scale  *  m -> centroid.all();
			sum += scale;
		}
		centroid.all()  /=  double (sum);

		for (integer k = 1; k <= thy numberOfGroups; k ++) {
			const SSCP m = thy groups->at [k];
			const double scale = SSCP_getNumberOfObservations (m);
			thy aprioriProbabilities [k] = scale / my numberOfRows;
			between.row (k) <<= m -> centroid.all()  -  centroid.all();
			between.row (k)  *=  sqrt (scale);
		}

		// We need to solve B'B.x = lambda W'W.x, where B'B and W'W are the between and within covariance matrices.
		// We do not calculate these covariance matrices directly from the data but instead use the GSVD to solve for
		// the eigenvalues and eigenvectors of the equation.
		
		thy eigen = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (thy eigen.get(), between.get(), mew -> data.get());

		/*
			Costs.
		*/
		thy costs = raw_MAT (thy numberOfGroups, thy numberOfGroups);
		
		thy costs.all()  <<=  1.0;
		thy costs.diagonal()  <<=  0.0;
		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Discriminant not created.");
	}
}


autoTableOfReal Discriminant_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, integer group, bool poolCovarianceMatrices) {
	try {
		Melder_require (group > 0 && group <= my numberOfGroups,
			U"Group should be in the range [1, ", my numberOfGroups, U"].");
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoCovariance covg = SSCP_to_Covariance (pool.get(), my numberOfGroups);
		autoCovariance cov = SSCP_to_Covariance (my groups->at [group], 1);
		autoTableOfReal him;
		if (poolCovarianceMatrices) { // use group mean instead of overall mean!
			covg -> centroid.all() <<= cov -> centroid.all();
			him = Covariance_TableOfReal_mahalanobis (covg.get(), thee, false);
		} else {
			him = Covariance_TableOfReal_mahalanobis (cov.get(), thee, false);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created.");
	}
}

autoTableOfReal Discriminant_TableOfReal_mahalanobis_all (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices) {
	autoCovariance covg;
	if (poolCovarianceMatrices) {
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		covg = SSCP_to_Covariance (pool.get(), my numberOfGroups);
	}
	autoTableOfReal him = TableOfReal_create (thy numberOfRows, 1);
	his rowLabels.all() <<= thy rowLabels.all();
	for (integer igroup = 1 ; igroup <= my numberOfGroups; igroup ++) {
		conststring32 label = Thing_getName (my groups->at [igroup]);
		autoCovariance cov = SSCP_to_Covariance (my groups->at [igroup], 1);
		autoTableOfReal groupMahalanobis;
		if (poolCovarianceMatrices) {
			covg -> centroid.all() <<= cov -> centroid.all();
			groupMahalanobis = Covariance_TableOfReal_mahalanobis (covg.get(), thee, false);
		} else {
			groupMahalanobis = Covariance_TableOfReal_mahalanobis (cov.get(), thee, false);
		}
		for (integer idata = 1; idata <= thy numberOfRows; idata ++)
			if (Melder_equ (label, his rowLabels [idata].get()))
				his data [idata] [1] = groupMahalanobis-> data [idata] [1];
	}
	return him;
}

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities) {
	try {
		const integer numberOfGroups = Discriminant_getNumberOfGroups (me);
		const integer dimension = Eigen_getDimensionOfComponents (my eigen.get());

		Melder_require (dimension == thy numberOfColumns,
			U"The number of columns should agree with the dimension of the discriminant.");
		
		autoVEC log_p = raw_VEC (numberOfGroups);
		autoVEC log_apriori = raw_VEC (numberOfGroups);
		autoVEC ln_determinant = raw_VEC (numberOfGroups);
		autoVEC buf = raw_VEC (dimension);
		
		autovector <SSCP> sscpvec = newvectorzero <SSCP> (numberOfGroups);
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoClassificationTable him = ClassificationTable_create (thy numberOfRows, numberOfGroups);
		his rowLabels.all() <<= thy rowLabels.all();

		/*
			Scale the sscp to become a covariance matrix.
		*/
		pool -> data.get()  *=  1.0 / (pool -> numberOfObservations - numberOfGroups);
		
		double lnd;
		autoSSCPList agroups;
		SSCPList groups;   // ppgb FIXME dit kan niet goed izjn
		if (poolCovarianceMatrices) {
			/*
				Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
				L^-1 will be used later in the Mahalanobis distance calculation:
				v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).
			*/
			if (Melder_debug == 52)
				Melder_casual (U"***** before lower Cholesky inverse: \n", pool -> data.all());
			MATlowerCholeskyInverse_inplace (pool -> data.get(), & lnd);
			if (Melder_debug == 52)
				Melder_casual (U"***** after lower Cholesky inverse: \n", pool -> data.all());
			for (integer j = 1; j <= numberOfGroups; j ++) {
				ln_determinant [j] = lnd;
				sscpvec [j] = pool.get();
			}
			groups = my groups.get();
		} else {
			/*
				Calculate the inverses of all group covariance matrices.
				In case of a singular matrix, substitute inverse of pooled.
			*/
			agroups = Data_copy (my groups.get());
			groups = agroups.get();
			integer npool = 0;
			for (integer j = 1; j <= numberOfGroups; j ++) {
				const SSCP t = groups->at [j];
				const integer no = Melder_ifloor (SSCP_getNumberOfObservations (t));
				t -> data.get()  *=  1.0 / (no - 1);
				
				sscpvec [j] = groups->at [j];
				try {
					MATlowerCholeskyInverse_inplace (t -> data.get(), & ln_determinant [j]);
				} catch (MelderError) {
					/*
						Clear the error.
						Try the alternative: the pooled covariance matrix.
					*/
					Melder_clearError ();
					if (npool == 0)
						MATlowerCholeskyInverse_inplace (pool -> data.get(), & lnd);
					npool ++;
					sscpvec [j] = pool.get();
					ln_determinant [j] = lnd;
				}
			}
			if (npool > 0)
				Melder_warning (npool, U" groups use pooled covariance matrix.");
		}

		/*
			Labels for columns in ClassificationTable
		*/
		for (integer j = 1; j <= numberOfGroups; j ++) {
			conststring32 name = Thing_getName (my groups->at [j]);
			if (! name)
				name = U"?";
			TableOfReal_setColumnLabel (him.get(), j, name);
		}

		/*
			Normalize the sum of the apriori probabilities to 1.
			Next take ln (p) because otherwise probabilities might be too small to represent.
		*/
		if (Melder_debug == 52)
			Melder_casual (U"***** before normalizing priors: \n", my aprioriProbabilities.all());
		VECnormalize_inplace (my aprioriProbabilities.get(), 1.0, 1.0);
		if (Melder_debug == 52)
			Melder_casual (U"***** after normalizing priors: \n", my aprioriProbabilities.all());
		const double logg = log (numberOfGroups);
		for (integer j = 1; j <= numberOfGroups; j ++)
			log_apriori [j] = ( useAprioriProbabilities ? log (my aprioriProbabilities [j]) : - logg );

		/*
			Generalized squared distance function:
			D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)
		*/
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			double norm = 0.0, pt_max = -1e308;
			for (integer j = 1; j <= numberOfGroups; j ++) {
				const SSCP t = groups->at [j];
				const double md = NUMmahalanobisDistanceSquared (sscpvec [j] -> data.get(), thy data.row (i), t -> centroid.get());
				if (Melder_debug == 52)
					Melder_casual (U"***** Mahalanobis distance (squared): ", i, U" ", j, U" ", md);
				const double pt = log_apriori [j] - 0.5 * (ln_determinant [j] + md);
				if (pt > pt_max)
					pt_max = pt;
				log_p [j] = pt;
			}
			for (integer j = 1; j <= numberOfGroups; j ++)
				norm += log_p [j] = exp (log_p [j] - pt_max);
			for (integer j = 1; j <= numberOfGroups; j ++)
				his data [i] [j] = log_p [j] / norm;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"ClassificationTable from Discriminant & TableOfReal not created.");
	}
}

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable_dw (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities, double alpha, double minProb, autoTableOfReal *displacements) {
	try {
		const integer g = Discriminant_getNumberOfGroups (me);
		const integer p = Eigen_getDimensionOfComponents (my eigen.get());
		const integer m = thy numberOfRows;

		Melder_require (p == thy numberOfColumns,
			U"The number of columns does not agree with the dimension of the discriminant.");

		autoVEC log_p = raw_VEC (g);
		autoVEC log_apriori = raw_VEC (g);
		autoVEC ln_determinant = raw_VEC (g);
		autoVEC buf = raw_VEC (p);
		autoVEC displacement = raw_VEC (p);
		autoVEC x = zero_VEC (p);
		autovector <SSCP> sscpvec = newvectorzero <SSCP> (g);
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoClassificationTable him = ClassificationTable_create (m, g);
		his rowLabels.all() <<= thy rowLabels.all();
		autoTableOfReal adisplacements = Data_copy (thee);

		/*
			Scale the sscp to become a covariance matrix.
		*/
		
		pool -> data.get()  *=  1.0 / (pool -> numberOfObservations - g);

		double lnd;
		autoSSCPList agroups;
		SSCPList groups;
		if (poolCovarianceMatrices) {
			
			/*
				Covariance matrix S can be Cholesky decomposed as S = L.L'. 
				Calculate L^-1.
				L^-1 will be used later in the Mahalanobis distance calculation:
				v'.S^-1.v = v'.L^-1'.L^-1.v = (L^-1.v)'.(L^-1.v).
			*/

			MATlowerCholeskyInverse_inplace (pool -> data.get(), & lnd);
			for (integer j = 1; j <= g; j ++) {
				ln_determinant [j] = lnd;
				sscpvec [j] = pool.get();
			}
			groups = my groups.get();
		} else {
			
			/*
				Calculate the inverses of all group covariance matrices.
				In case of a singular matrix, substitute inverse of pooled.
			*/

			agroups = Data_copy (my groups.get()); 
			groups = agroups.get();
			integer npool = 0;
			for (integer j = 1; j <= g; j ++) {
				const SSCP t = groups->at [j];
				const integer no = Melder_ifloor (SSCP_getNumberOfObservations (t));
				t -> data.get()  *=  1.0 / (no - 1);
				
				sscpvec [j] = groups->at [j];
				try {
					MATlowerCholeskyInverse_inplace (t -> data.get(), & ln_determinant [j]);
				} catch (MelderError) {
					
					/*
						Clear the error.
						Try the alternative: the pooled covariance matrix.
					*/

					Melder_clearError ();
					if (npool == 0)
						MATlowerCholeskyInverse_inplace (pool -> data.get(), & lnd);
					npool ++;
					sscpvec [j] = pool.get();
					ln_determinant [j] = lnd;
				}
			}
			if (npool > 0)
				Melder_warning (npool, U" groups use pooled covariance matrix.");
		}

		/*
			Labels for columns in ClassificationTable
		*/

		for (integer j = 1; j <= g; j ++) {
			conststring32 name = Thing_getName (my groups->at [j]);
			if (! name)
				name = U"?";
			TableOfReal_setColumnLabel (him.get(), j, name);
		}

		/*
			Normalize the sum of the apriori probabilities to 1.
			Next take ln (p) because otherwise probabilities might be too small to represent.
		*/

		const double logg = log (g);
		VECnormalize_inplace (my aprioriProbabilities.get(), 1.0, 1.0);
		for (integer j = 1; j <= g; j ++) {
			log_apriori [j] = ( useAprioriProbabilities ? log (my aprioriProbabilities [j]) : - logg );
		}

		/*
			Generalized squared distance function:
			D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)
		*/
		
		for (integer i = 1; i <= m; i ++) {
			SSCP winner;
			double norm = 0, pt_max = -1e308;
			integer iwinner = 1;
			for (integer k = 1; k <= p; k ++)
				x [k] = thy data [i] [k] + displacement [k];
			for (integer j = 1; j <= g; j ++) {
				const SSCP t = groups->at [j];
				const double md = NUMmahalanobisDistanceSquared (sscpvec [j] -> data.get(), x.get(), t -> centroid.get());
				const double pt = log_apriori [j] - 0.5 * (ln_determinant [j] + md);
				if (pt > pt_max) {
					pt_max = pt;
					iwinner = j;
				}
				log_p [j] = pt;
			}
			for (integer j = 1; j <= g; j ++)
				norm += log_p [j] = exp (log_p [j] - pt_max);

			for (integer j = 1; j <= g; j ++)
				his data [i] [j] = log_p [j] / norm;

			/*
				Save old displacement, calculate new displacement
			*/

			winner = groups->at [iwinner];
			for (integer k = 1; k <= p; k ++) {
				adisplacements -> data [i] [k] = displacement [k];
				if (his data [i] [iwinner] > minProb) {
					double delta_k = winner -> centroid [k] - x [k];
					displacement [k] += alpha * delta_k;
				}
			}
		}
		*displacements = adisplacements.move();
		return him;
	} catch (MelderError) {
		Melder_throw (U"ClassificationTable for Weenink procedure not created.");
	}
}


autoConfiguration Discriminant_TableOfReal_to_Configuration (Discriminant me, TableOfReal thee, integer numberOfDimensions) {
	try {
		Melder_require (thy numberOfColumns == my eigen -> dimension,
			U"The number of columns in the TableOfReal (", thy numberOfColumns, U") should be equal to the dimension of the eigenvectors of the Discriminant (", my eigen -> dimension, U").");
		if (numberOfDimensions == 0)
			numberOfDimensions = Discriminant_getNumberOfFunctions (me);
		Melder_require (numberOfDimensions <= my eigen -> numberOfEigenvalues,
			U"The number of dimensions should not exceed the number of eigenvectors in the Discriminant (", my eigen -> numberOfEigenvalues, U").");
		autoConfiguration him = Configuration_create (thy numberOfRows, numberOfDimensions);
		mul_MAT_out (his data.get(), thy data.get(), my eigen -> eigenvectors.horizontalBand (1, numberOfDimensions).transpose ());
		TableOfReal_copyLabels (thee, him.get(), 1, 0);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"Eigenvector ", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created.");
	}
}

autoConfiguration TableOfReal_to_Configuration_lda (TableOfReal me, integer numberOfDimensions) {
	try {
		autoDiscriminant thee = TableOfReal_to_Discriminant (me);
		autoConfiguration him = Discriminant_TableOfReal_to_Configuration (thee.get(), me, numberOfDimensions);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Configuration with lda data not created.");
	}
}

/* End of file TableOfReal_and_Discriminant.cpp */
