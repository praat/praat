/* Discriminant.c
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20011016 removed some causes for compiler warnings
 djmw 20020313 removed obsolete TableOfReal_sortByLabels method
 djmw 20020314 +Discriminant_extractWithinGroupSSCP,
 	+Discriminant_extractGroupLabels, +Discriminant_setGroupLabels.
 djmw 20020327 modified Discriminant_TableOfReal_to_Configuration
 djmw 20020418 Removed some causes for compiler warnings
 djmw 20020502 modified call Eigen_TableOfReal_project_into
 djmw 20030801 Discriminant_drawConcentrationEllipses extra argument
 djmw 20050405 Modified column label: eigenvector->Eigenvector
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071009 wchar
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20081119 Check in TableOfReal_to_Discriminant if TableOfReal_areAllCellsDefined
 djmw 20100107 +Discriminant_TableOfReal_mahalanobis
 djmw 20110304 Thing_new
*/

#include "Discriminant.h"
#include "SSCP.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "NUMlapack.h"
#include "SVD.h"
#include "NUM2.h"
#include "TableOfReal_extensions.h"

#include "oo_DESTROY.h"
#include "Discriminant_def.h"
#include "oo_COPY.h"
#include "Discriminant_def.h"
#include "oo_EQUAL.h"
#include "Discriminant_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Discriminant_def.h"
#include "oo_WRITE_TEXT.h"
#include "Discriminant_def.h"
#include "oo_READ_TEXT.h"
#include "Discriminant_def.h"
#include "oo_WRITE_BINARY.h"
#include "Discriminant_def.h"
#include "oo_READ_BINARY.h"
#include "Discriminant_def.h"
#include "oo_DESCRIPTION.h"
#include "Discriminant_def.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

Thing_implement (Discriminant, Daata, 1);

void structDiscriminant :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of groups: ", numberOfGroups);
	MelderInfo_writeLine (U"Number of eigenvalues: ", eigen -> numberOfEigenvalues);
	MelderInfo_writeLine (U"Dimension of eigenvector: ", eigen -> dimension);
	MelderInfo_writeLine (U"Number of discriminant functions: ", Discriminant_getNumberOfFunctions (this));
	MelderInfo_writeLine (U"Number of observations (total): ", Discriminant_getNumberOfObservations (this, 0));
}

autoDiscriminant Discriminant_create (integer numberOfGroups, integer numberOfEigenvalues, integer dimension) {
	try {
		autoDiscriminant me = Thing_new (Discriminant);
		my eigen = Eigen_create (numberOfEigenvalues, dimension);
		my numberOfGroups = numberOfGroups;
		my groups = SSCPList_create ();
		my total = SSCP_create (dimension);
		my aprioriProbabilities = NUMvector<double> (1, numberOfGroups);
		my costs = NUMmatrix<double> (1, numberOfGroups, 1, numberOfGroups);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Discriminant not created.");
	}
}

integer Discriminant_groupLabelToIndex (Discriminant me, const char32 *label) {
	const char32 *name;

	for (integer i = 1; i <= my numberOfGroups; i ++) {
		if (!! (name = Thing_getName (my groups -> at [i])) && str32equ (name, label)) {
			return i;
		}
	}
	return 0;
}

integer Discriminant_getNumberOfGroups (Discriminant me) {
	return my numberOfGroups;
}

integer Discriminant_getNumberOfObservations (Discriminant me, integer group) {
	if (group == 0) {
		return Melder_ifloor (my total -> numberOfObservations);
	} else if (group >= 1 && group <= my numberOfGroups) {
		SSCP sscp = my groups->at [group];
		return Melder_ifloor (sscp -> numberOfObservations);
	} else {
		return -1;
	}
}

void Discriminant_setAprioriProbability (Discriminant me, integer group, double p) {
	Melder_require (group > 0 && group <= my numberOfGroups, U"The group number (", group, U") should be in the interval [1, ", my numberOfGroups, U"]; the supplied value (", group, U") falls outside it.");
	Melder_require (p >= 0.0 && p <= 1.0, U"The probability should be in the interval [0, 1]");

	my aprioriProbabilities [group] = p;
}

integer Discriminant_getNumberOfFunctions (Discriminant me) {
	integer numberOfFunctions = MIN (my numberOfGroups - 1, my eigen -> dimension);
	numberOfFunctions = MIN (numberOfFunctions, my eigen ->  numberOfEigenvalues);
	return numberOfFunctions;
}

void Discriminant_setGroupLabels (Discriminant me, Strings thee) {
	Melder_require (my numberOfGroups == thy numberOfStrings, U"The number of strings should equal the number of groups.");

	for (integer i = 1; i <= my numberOfGroups; i ++) {
		const char32 *noname = U"", *name;
		name = thy strings [i];
		if (name == 0) {
			name = noname;
		}
		Thing_setName (my groups->at [i], name);
	}
}

autoStrings Discriminant_extractGroupLabels (Discriminant me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<char32 *> (1, my numberOfGroups);
		thy numberOfStrings = my numberOfGroups;
		for (integer i = 1; i <= my numberOfGroups; i ++) {
			const char32 *name = Thing_getName (my groups->at [i]);
			thy strings [i] = Melder_dup (name);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group labels not extracted.");
	}
}

autoTableOfReal Discriminant_extractGroupCentroids (Discriminant me) {
	try {
		integer m = my groups -> size, n = my eigen -> dimension;
		autoTableOfReal thee = TableOfReal_create (m, n);

		for (integer i = 1; i <= m; i ++) {
			SSCP sscp = my groups->at [i];
			TableOfReal_setRowLabel (thee.get(), i, Thing_getName (sscp));
			NUMvector_copyElements (sscp -> centroid, thy data [i], 1, n);
		}
		NUMstrings_copyElements (my groups->at [m] -> columnLabels, thy columnLabels, 1, n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group centroids not extracted.");
	}
}

autoTableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me) {
	try {
		integer m = my groups->size, n = my eigen -> dimension;
		autoTableOfReal thee = TableOfReal_create (m, n);

		for (integer i = 1; i <= m; i ++) {
			SSCP sscp = my groups->at [i];
			TableOfReal_setRowLabel (thee.get(), i, Thing_getName (sscp));
			integer numberOfObservationsm1 = Melder_ifloor (sscp -> numberOfObservations) - 1;
			for (integer j = 1; j <= n; j ++) {
				thy data [i] [j] = ( numberOfObservationsm1 > 0 ? sqrt (sscp -> data [j] [j] / numberOfObservationsm1) : undefined );
			}
		}
		NUMstrings_copyElements (my groups->at [m] -> columnLabels, thy columnLabels, 1, n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group standard deviations not extracted.");
	}
}

double Discriminant_getWilksLambda (Discriminant me, integer from) {
	integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	if (from >= numberOfFunctions) {
		return 1;
	}
	if (from < 1) {
		from = 1;
	}
	return NUMwilksLambda (my eigen -> eigenvalues, 1 + from, numberOfFunctions);
}

/*
	raw r [j]: eigenvec [i] [j]
	unstandardized u [j]: sqrt(N-g) * r [j]
	standardized s [j]: u [j] sqrt (w [i] [i] / (N-g))
*/
autoTableOfReal Discriminant_extractCoefficients (Discriminant me, int choice) {
	try {
		bool raw = choice == 0, standardized = choice == 2;
		integer nx = my eigen -> dimension, ny = my eigen -> numberOfEigenvalues;

		SSCP total = my total.get();
		autoTableOfReal thee = TableOfReal_create (ny, nx + 1);
		NUMstrings_copyElements (my total -> columnLabels, thy columnLabels, 1, nx);

		autoSSCP within;
		if (standardized) {
			within = Discriminant_extractPooledWithinGroupsSSCP (me);
		}

		TableOfReal_setColumnLabel (thee.get(), nx + 1, U"constant");
		TableOfReal_setSequentialRowLabels (thee.get(), 1, ny, U"function_", 1, 1);

		double scale = sqrt (total -> numberOfObservations - my numberOfGroups);
		double *centroid = my total -> centroid;
		for (integer i = 1; i <= ny; i ++) {
			double u0 = 0.0, ui;
			for (integer j = 1; j <= nx; j ++) {
				if (standardized) {
					scale = sqrt (within -> data [j] [j]);
				}
				thy data [i] [j] = ui = scale * my eigen -> eigenvectors [i] [j];;
				u0 += ui * centroid [j];
			}
			thy data [i] [nx + 1] = raw ? 0.0 : -u0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": coefficients not extracted.");
	}
}

static double Discriminant_getDegreesOfFreedom (Discriminant me) {
	double ndf = 0.0;
	for (integer i = 1; i <= my groups->size; i ++) {
		ndf += SSCP_getDegreesOfFreedom (my groups->at [i]);
	}
	return ndf;
}

void Discriminant_getPartialDiscriminationProbability (Discriminant me, integer numberOfDimensions, double *p_prob, double *p_chisq, double *p_df)
{
	integer g = my numberOfGroups;
	integer p = my eigen -> dimension, k = numberOfDimensions;
	integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	double degreesOfFreedom = Discriminant_getDegreesOfFreedom (me);

	double prob = undefined, chisq = undefined, df = undefined;

	if (k < numberOfFunctions) {
		double lambda = NUMwilksLambda (my eigen -> eigenvalues, k + 1, numberOfFunctions);
		if (lambda != 1.0) {
			chisq = - (degreesOfFreedom + (g - p) / 2.0 - 1.0) * log (lambda);
			df = (p - k) * (g - k - 1);
			if (p_prob) {
				prob =  NUMchiSquareQ (chisq, df);
			}
		}
	}
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_df) {
		*p_df = df;
	}
}

double Discriminant_getConcentrationEllipseArea (Discriminant me, integer group,
        double scale, bool confidence, bool discriminantDirections, integer d1, integer d2)
{
	double area = undefined;

	if (group < 1 || group > my numberOfGroups) {
		return area;
	}

	if (discriminantDirections) {
		autoSSCP thee = Eigen_SSCP_project (my eigen.get(), my groups->at [group]);
		area = SSCP_getConcentrationEllipseArea (thee.get(), scale, confidence, d1, d2);
	} else {
		area = SSCP_getConcentrationEllipseArea (my groups->at [group], scale, confidence, d1, d2);
	}
	return area;
}

double Discriminant_getLnDeterminant_group (Discriminant me, integer group) {
	if (group < 1 || group > my numberOfGroups) {
		return undefined;
	}
	autoCovariance c = SSCP_to_Covariance (my groups->at [group], 1);
	double ln_d = SSCP_getLnDeterminant (c.get());
	return ln_d;
}

double Discriminant_getLnDeterminant_total (Discriminant me) {
	autoCovariance c = SSCP_to_Covariance (my total.get(), 1);
	double ln_d = SSCP_getLnDeterminant (c.get());
	return ln_d;
}

autoSSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me) {
	return SSCPList_to_SSCP_pool (my groups.get());
}

autoSSCP Discriminant_extractWithinGroupSSCP (Discriminant me, integer index) {
	try {
		Melder_require (index > 0 && index <= my numberOfGroups, U"Index should be in interval [1,", my numberOfGroups, U"].");
		
		autoSSCP thee = Data_copy (my groups->at [index]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": within group SSCP not created.");
	}
}

autoSSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me) {
	try {
		integer n = my total -> numberOfRows;
		autoSSCP b = Data_copy (my total.get());
		autoSSCP w = SSCPList_to_SSCP_pool (my groups.get());
		for (integer i = 1; i <= n; i ++) {
			for (integer j = i; j <= n; j ++) {
				b -> data [j] [i] = (b -> data [i] [j] -= w -> data [i] [j]);
			}
		}
		return b;
	} catch (MelderError) {
		Melder_throw (me, U": between group SSCP not created.");
	}
}

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g, bool discriminantDirections, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, bool poolCovarianceMatrices, bool garnish) {
	(void) me; (void) g; (void) discriminantDirections; (void) d1; (void) d2;
	(void) xmin; (void) xmax; (void) ymin;
	(void) ymax; (void) fontSize; (void) poolCovarianceMatrices; (void) garnish;

}

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g, double scale, bool confidence, char32 *label,
	bool discriminantDirections, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, bool garnish)
{
	integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);

	if (! discriminantDirections) {
		SSCPList_drawConcentrationEllipses (my groups.get(), g, scale, confidence, label, d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	if (numberOfFunctions <= 1) {
		Melder_warning (U"Discriminant_drawConcentrationEllipses: Nothing drawn because there is only one dimension in the discriminant space.");
		return;
	}

	// Project SSCPs on eigenvectors.

	if (d1 == 0 && d2 == 0) {
		d1 = 1;
		d2 = MIN (numberOfFunctions, d1 + 1);
	} else if (d1 < 0 || d2 > numberOfFunctions) {
		return;
	}

	double *v1 = my eigen -> eigenvectors [d1];
	double *v2 = my eigen -> eigenvectors [d2];


	autoSSCPList thee = SSCPList_toTwoDimensions (my groups.get(), v1, v2);

	SSCPList_drawConcentrationEllipses (thee.get(), g, scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		char32 llabel [40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"function ", d2);
		Graphics_textLeft (g, true, llabel);
		Graphics_marksBottom (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"function ", d1);
		Graphics_textBottom (g, true, llabel);
	}
}

autoDiscriminant TableOfReal_to_Discriminant (TableOfReal me) {
	try {
		autoDiscriminant thee = Thing_new (Discriminant);
		integer dimension = my numberOfColumns;

		Melder_require (! NUMdmatrix_containsUndefinedElements (my data, 1, my numberOfRows, 1, my numberOfColumns), 
			U"There should be no undefined elements in the table.");
		
		Melder_require (TableOfReal_hasRowLabels (me), U"All rows should be labeled.");

		autoTableOfReal mew = TableOfReal_sortOnlyByRowLabels (me);
		if (! TableOfReal_hasColumnLabels (mew.get())) {
			TableOfReal_setSequentialColumnLabels (mew.get(), 0, 0, U"c", 1, 1);
		}

		thy groups = TableOfReal_to_SSCPList_byLabel (mew.get());
		thy total = TableOfReal_to_SSCP (mew.get(), 0, 0, 0, 0);

		if ((thy numberOfGroups = thy groups -> size) < 2) {
			Melder_throw (U"Number of groups should be greater than one.");
		}

		TableOfReal_centreColumns_byRowLabel (mew.get());

		// Overall centroid and apriori probabilities and costs.

		autoNUMvector<double> centroid (1, dimension);
		autoNUMmatrix<double> between (1, thy numberOfGroups, 1, dimension);
		thy aprioriProbabilities = NUMvector<double> (1, thy numberOfGroups);
		thy costs = NUMmatrix<double> (1, thy numberOfGroups, 1, thy numberOfGroups);

		double sum = 0, scale;
		for (integer k = 1; k <= thy numberOfGroups; k ++) {
			SSCP m = thy groups->at [k];
			sum += scale = SSCP_getNumberOfObservations (m);
			for (integer j = 1; j <= dimension; j ++) {
				centroid [j] += scale * m -> centroid [j];
			}
		}

		for	(integer j = 1; j <= dimension; j ++) {
			centroid [j] /= sum;
		}

		for (integer k = 1; k <= thy numberOfGroups; k ++) {
			SSCP m = thy groups->at [k];
			scale = SSCP_getNumberOfObservations (m);
			thy aprioriProbabilities [k] = scale / my numberOfRows;
			for (integer j = 1; j <= dimension; j ++) {
				between [k] [j] = sqrt (scale) * (m -> centroid [j] - centroid [j]);
			}
		}

		// We need to solve B'B.x = lambda W'W.x, where B'B and W'W are the between and within covariance matrices.
		// We do not calculate these covariance matrices directly from the data but instead use the GSVD to solve for
		// the eigenvalues and eigenvectors of the equation.
		
		thy eigen = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (thy eigen.get(), between.peek(), thy numberOfGroups, dimension, mew -> data, my numberOfRows);

		// Default priors and costs

		for (integer igroup = 1; igroup <= thy numberOfGroups; igroup ++) {
			for (integer jgroup = igroup + 1; jgroup <= thy numberOfGroups; jgroup ++) {
				thy costs [igroup] [jgroup] = thy costs [jgroup] [igroup] = 1.0;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Discriminant not created.");
	}
}

autoConfiguration Discriminant_TableOfReal_to_Configuration (Discriminant me, TableOfReal thee, integer numberOfDimensions) {
	try {
		if (numberOfDimensions == 0) {
			numberOfDimensions = Discriminant_getNumberOfFunctions (me);
		}
		autoConfiguration him = Configuration_create (thy numberOfRows, numberOfDimensions);
		Eigen_TableOfReal_into_TableOfReal_projectRows (my eigen.get(), thee, 1, him.get(), 1, numberOfDimensions);
		TableOfReal_copyLabels (thee, him.get(), 1, 0);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"Eigenvector ", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created.");
	}
}

/*
	Calculate squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix (li) is the inverse L^-1 of the Cholesky decomposition
	S = L.L'.
*/
static double mahalanobisDistanceSq (double **li, integer n, double *v, double *m, double *buf) {
	for (integer i = 1; i <= n; i ++) {
		buf [i] = v [i] - m [i];
	}

	double chisq = 0.0;
	for (integer i = n; i > 0; i--) {
		double t = 0.0;
		for (integer j = 1; j <= i; j ++) {
			t += li [i] [j] * buf [j];
		}
		chisq += t * t;
	}
	return chisq;
}

autoTableOfReal Discriminant_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, integer group, bool poolCovarianceMatrices) {
	try {
		Melder_require (group > 0 && group <= my numberOfGroups, U"Group should be in the range [1, ", my numberOfGroups, U"].");
		
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoCovariance covg = SSCP_to_Covariance (pool.get(), my numberOfGroups);
		autoCovariance cov = SSCP_to_Covariance (my groups->at [group], 1);
		autoTableOfReal him;
		if (poolCovarianceMatrices) { // use group mean instead of overall mean!
			NUMvector_copyElements (cov -> centroid, covg -> centroid, 1, cov -> numberOfColumns);
			him = Covariance_TableOfReal_mahalanobis (covg.get(), thee, false);
		} else {
			him = Covariance_TableOfReal_mahalanobis (cov.get(), thee, false);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created.");
	}
}

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities) {
	try {
		integer g = Discriminant_getNumberOfGroups (me);   // ppgb wat betekent g?
		integer p = Eigen_getDimensionOfComponents (my eigen.get());   // ppgb wat betekent p?
		integer m = thy numberOfRows;   // ppgb wat betekent m?

		Melder_require (p == thy numberOfColumns, U"The number of columns should agree with the dimension of the discriminant.");
		
		autoNUMvector<double> log_p (1, g);
		autoNUMvector<double> log_apriori (1, g);
		autoNUMvector<double> ln_determinant (1, g);
		autoNUMvector<double> buf (1, p);
		autoNUMvector<SSCP> sscpvec (1, g);
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoClassificationTable him = ClassificationTable_create (m, g);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m);

		// Scale the sscp to become a covariance matrix.

		for (integer i = 1; i <= p; i ++) {
			for (integer k = i; k <= p; k ++) {
				pool -> data [k] [i] = pool -> data [i] [k] /= pool -> numberOfObservations - g;
			}
		}

		double lnd;
		autoSSCPList agroups; SSCPList groups;   // ppgb FIXME dit kan niet goed izjn
		if (poolCovarianceMatrices) {
			/*
				Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
				L^-1 will be used later in the Mahalanobis distance calculation:
				v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).
			*/

			NUMlowerCholeskyInverse (pool -> data, p, & lnd);
			for (integer j = 1; j <= g; j ++) {
				ln_determinant [j] = lnd;
				sscpvec [j] = pool.get();
			}
			groups = my groups.get();
		} else {
			// Calculate the inverses of all group covariance matrices.
			// In case of a singular matrix, substitute inverse of pooled.

			agroups = Data_copy (my groups.get());
			groups = agroups.get();
			integer npool = 0;
			for (integer j = 1; j <= g; j ++) {
				SSCP t = groups->at [j];
				integer no = Melder_ifloor (SSCP_getNumberOfObservations (t));
				for (integer i = 1; i <= p; i ++) {
					for (integer k = i; k <= p; k ++) {
						t -> data [k] [i] = t -> data [i] [k] /= no - 1;
					}
				}
				sscpvec [j] = groups->at [j];
				try {
					NUMlowerCholeskyInverse (t -> data, p, & ln_determinant [j]);
				} catch (MelderError) {
					// Try the alternative: the pooled covariance matrix.
					// Clear the error.

					Melder_clearError ();
					if (npool == 0) {
						NUMlowerCholeskyInverse (pool -> data, p, & lnd);
					}
					npool ++;
					sscpvec [j] = pool.get();
					ln_determinant [j] = lnd;
				}
			}
			if (npool > 0) {
				Melder_warning (npool, U" groups use pooled covariance matrix.");
			}
		}

		// Labels for columns in ClassificationTable

		for (integer j = 1; j <= g; j ++) {
			const char32 *name = Thing_getName (my groups->at [j]);
			if (! name) {
				name = U"?";
			}
			TableOfReal_setColumnLabel (him.get(), j, name);
		}

		// Normalize the sum of the apriori probabilities to 1.
		// Next take ln (p) because otherwise probabilities might be too small to represent.

		NUMvector_normalize1 (my aprioriProbabilities, g);
		double logg = log (g);
		for (integer j = 1; j <= g; j ++) {
			log_apriori [j] = useAprioriProbabilities ? log (my aprioriProbabilities [j]) : - logg;
		}

		// Generalized squared distance function:
		// D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)

		for (integer i = 1; i <= m; i ++) {
			double norm = 0.0, pt_max = -1e308;
			for (integer j = 1; j <= g; j ++) {
				SSCP t = groups->at [j];
				double md = mahalanobisDistanceSq (sscpvec [j] -> data, p, thy data [i], t -> centroid, buf.peek());
				double pt = log_apriori [j] - 0.5 * (ln_determinant [j] + md);
				if (pt > pt_max) {
					pt_max = pt;
				}
				log_p [j] = pt;
			}
			for (integer j = 1; j <= g; j ++) {
				norm += log_p [j] = exp (log_p [j] - pt_max);
			}
			for (integer j = 1; j <= g; j ++) {
				his data [i] [j] = log_p [j] / norm;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"ClassificationTable from Discriminant & TableOfReal not created.");
	}
}

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable_dw (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities, double alpha, double minProb, autoTableOfReal *displacements) {
	try {
		integer g = Discriminant_getNumberOfGroups (me);
		integer p = Eigen_getDimensionOfComponents (my eigen.get());
		integer m = thy numberOfRows;

		Melder_require (p == thy numberOfColumns, U"The number of columns does not agree with the dimension of the discriminant.");

		autoNUMvector<double> log_p (1, g);
		autoNUMvector<double> log_apriori (1, g);
		autoNUMvector<double> ln_determinant (1, g);
		autoNUMvector<double> buf (1, p);
		autoNUMvector<double> displacement (1, p);
		autoNUMvector<double> x (1, p);
		autoNUMvector<SSCP> sscpvec (1, g);
		autoSSCP pool = SSCPList_to_SSCP_pool (my groups.get());
		autoClassificationTable him = ClassificationTable_create (m, g);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m);
		autoTableOfReal adisplacements = Data_copy (thee);

		// Scale the sscp to become a covariance matrix.

		for (integer i = 1; i <= p; i ++) {
			for (integer k = i; k <= p; k ++) {
				pool -> data [k] [i] = pool -> data [i] [k] /= pool -> numberOfObservations - g;
			}
		}

		double lnd;
		autoSSCPList agroups;
		SSCPList groups;
		if (poolCovarianceMatrices) {
			// Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
			// L^-1 will be used later in the Mahalanobis distance calculation:
			// v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).

			NUMlowerCholeskyInverse (pool -> data, p, & lnd);
			for (integer j = 1; j <= g; j ++) {
				ln_determinant [j] = lnd;
				sscpvec [j] = pool.get();
			}
			groups = my groups.get();
		} else {
			//Calculate the inverses of all group covariance matrices.
			// In case of a singular matrix, substitute inverse of pooled.

			agroups = Data_copy (my groups.get()); 
			groups = agroups.get();
			integer npool = 0;
			for (integer j = 1; j <= g; j ++) {
				SSCP t = groups->at [j];
				integer no = Melder_ifloor (SSCP_getNumberOfObservations (t));
				for (integer i = 1; i <= p; i ++) {
					for (integer k = i; k <= p; k ++) {
						t -> data [k] [i] = t -> data [i] [k] /= no - 1;
					}
				}
				sscpvec [j] = groups->at [j];
				try {
					NUMlowerCholeskyInverse (t -> data, p, & ln_determinant [j]);
				} catch (MelderError) {
					// Try the alternative: the pooled covariance matrix.
					// Clear the error.

					Melder_clearError ();
					if (npool == 0) {
						NUMlowerCholeskyInverse (pool -> data, p, & lnd);
					}
					npool ++;
					sscpvec [j] = pool.get();
					ln_determinant [j] = lnd;
				}
			}
			if (npool > 0) {
				Melder_warning (npool, U" groups use pooled covariance matrix.");
			}
		}

		// Labels for columns in ClassificationTable

		for (integer j = 1; j <= g; j ++) {
			const char32 *name = Thing_getName (my groups->at [j]);
			if (! name) {
				name = U"?";
			}
			TableOfReal_setColumnLabel (him.get(), j, name);
		}

		// Normalize the sum of the apriori probabilities to 1.
		// Next take ln (p) because otherwise probabilities might be too small to represent.

		double logg = log (g);
		NUMvector_normalize1 (my aprioriProbabilities, g);
		for (integer j = 1; j <= g; j ++) {
			log_apriori [j] = useAprioriProbabilities ? log (my aprioriProbabilities [j]) : - logg;
		}

		// Generalized squared distance function:
		// D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)

		for (integer i = 1; i <= m; i ++) {
			SSCP winner;
			double norm = 0, pt_max = -1e308;
			integer iwinner = 1;
			for (integer k = 1; k <= p; k ++) {
				x [k] = thy data [i] [k] + displacement [k];
			}
			for (integer j = 1; j <= g; j ++) {
				SSCP t = groups->at [j];
				double md = mahalanobisDistanceSq (sscpvec [j] -> data, p, x.peek(), t -> centroid, buf.peek());
				double pt = log_apriori [j] - 0.5 * (ln_determinant [j] + md);
				if (pt > pt_max) {
					pt_max = pt;
					iwinner = j;
				}
				log_p [j] = pt;
			}
			for (integer j = 1; j <= g; j ++) {
				norm += log_p [j] = exp (log_p [j] - pt_max);
			}

			for (integer j = 1; j <= g; j ++) {
				his data [i] [j] = log_p [j] / norm;
			}

			// Save old displacement, calculate new displacement

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

autoConfiguration TableOfReal_to_Configuration_lda (TableOfReal me, integer numberOfDimensions) {
	try {
		autoDiscriminant thee = TableOfReal_to_Discriminant (me);
		autoConfiguration him = Discriminant_TableOfReal_to_Configuration (thee.get(), me, numberOfDimensions);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Configuration with lda data not created.");
	}
}

#undef MAX
#undef MIN

/* End of file Discriminant.cpp */
