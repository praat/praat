/* Discriminant.c
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20011016 removed some causes for compiler warnings
 djmw 20020313 removed obsolete TableOfReal_sortByLabels method
 djmw 20020314 +Discriminant_extractWithinGroupSSCP,
 	+Discriminant_extractGroupLabels, +Discriminant_setGroupLabels.
 djmw 20020327 modified Discriminant_and_TableOfReal_to_Configuration
 djmw 20020418 Removed some causes for compiler warnings
 djmw 20020502 modified call Eigen_and_TableOfReal_project_into
 djmw 20030801 Discriminant_drawConcentrationEllipses extra argument
 djmw 20050405 Modified column label: eigenvector->Eigenvector
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071009 wchar_t
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20081119 Check in TableOfReal_to_Discriminant if TableOfReal_areAllCellsDefined
 djmw 20100107 +Discriminant_and_TableOfReal_mahalanobis
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

Thing_implement (Discriminant, Eigen, 0);

void structDiscriminant :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of groups: ", Melder_integer (numberOfGroups));
	MelderInfo_writeLine (L"Number of variables: ", Melder_integer (dimension));
	MelderInfo_writeLine (L"Number of discriminant functions: ",
	                       Melder_integer (Discriminant_getNumberOfFunctions (this)));
	MelderInfo_writeLine (L"Number of observations (total): ",
	                       Melder_integer (Discriminant_getNumberOfObservations (this, 0)));
}

Discriminant Discriminant_create (long numberOfGroups, long numberOfEigenvalues, long dimension) {
	try {
		autoDiscriminant me = Thing_new (Discriminant);
		my numberOfGroups = numberOfGroups;
		Eigen_init (me.peek(), numberOfEigenvalues, dimension);
		my groups = SSCPs_create ();
		my total = SSCP_create (dimension);
		my aprioriProbabilities = NUMvector<double> (1, numberOfGroups);
		my costs = NUMmatrix<double> (1, numberOfGroups, 1, numberOfGroups);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Discriminant not created.");
	}
}

long Discriminant_groupLabelToIndex (Discriminant me, const wchar_t *label) {
	wchar_t *name;

	for (long i = 1; i <= my numberOfGroups; i++) {
		if ( (name = Thing_getName ( (Thing) my groups -> item[i])) && wcsequ (name, label)) {
			return i;
		}
	}
	return 0;
}

long Discriminant_getNumberOfGroups (Discriminant me) {
	return my numberOfGroups;
}

long Discriminant_getNumberOfObservations (Discriminant me, long group) {
	if (group == 0) {
		SSCP sscp = (SSCP) my total;
		return sscp -> numberOfObservations;
	} else if (group >= 1 && group <= my numberOfGroups) {
		SSCP sscp = (SSCP) my groups -> item[group];
		return sscp -> numberOfObservations;
	} else {
		return -1;
	}
}

void Discriminant_setAprioriProbability (Discriminant me, long group, double p) {
	if (group < 1 || group > my numberOfGroups) {
		Melder_throw ("The group number (", group, ") must be in the interval [1, ", my numberOfGroups, "]; the supplied value (", group, ") falls outside it.");
	}
	if (p < 0 || p > 1) {
		Melder_throw ("The probability must be in the interval [0, 1]; the supplied value (", p, ") falls outside it.");
	}
	my aprioriProbabilities[group] = p;
}

long Discriminant_getNumberOfFunctions (Discriminant me) {
	long nf = MIN (my numberOfGroups - 1, my dimension);
	nf = MIN (nf, my numberOfEigenvalues);
	return nf;
}

void Discriminant_setGroupLabels (Discriminant me, Strings thee) {
	if (my numberOfGroups != thy numberOfStrings) Melder_throw
		("The number of strings must equal the number of groups.");

	for (long i = 1; i <= my numberOfGroups; i++) {
		const wchar_t *noname = L"", *name;
		name = thy strings[i];
		if (name == 0) {
			name = noname;
		}
		Thing_setName ( (Thing) my groups -> item[i], name);
	}
}

Strings Discriminant_extractGroupLabels (Discriminant me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<wchar_t *> (1, my numberOfGroups);
		thy numberOfStrings = my numberOfGroups;
		for (long i = 1; i <= my numberOfGroups; i++) {
			wchar_t *name = Thing_getName ( (Thing) my groups -> item[i]);
			thy strings[i] = Melder_wcsdup (name);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": group labels not extracted.");
	}
}

TableOfReal Discriminant_extractGroupCentroids (Discriminant me) {
	try {
		long m = my groups -> size, n = my dimension;
		autoTableOfReal thee = TableOfReal_create (m, n);

		for (long i = 1; i <= m; i++) {
			SSCP sscp = (SSCP) my groups -> item[i];
			TableOfReal_setRowLabel (thee.peek(), i, Thing_getName (sscp));
			NUMvector_copyElements (sscp -> centroid, thy data[i], 1, n);
		}
		NUMstrings_copyElements ( ( (SSCP) my groups -> item[m]) -> columnLabels, thy columnLabels, 1, n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": group centroids not extracted.");
	}
}

TableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me) {
	try {
		long m = my groups -> size, n = my dimension;
		autoTableOfReal thee = TableOfReal_create (m, n);

		for (long i = 1; i <= m; i++) {
			SSCP sscp = (SSCP) my groups -> item[i];
			TableOfReal_setRowLabel (thee.peek(), i, Thing_getName (sscp));
			long numberOfObservationsm1 = sscp -> numberOfObservations - 1;
			for (long j = 1; j <= n; j++) {
				thy data[i][j] = numberOfObservationsm1 > 0 ? sqrt (sscp -> data[j][j] / numberOfObservationsm1) : NUMundefined;
			}
		}
		NUMstrings_copyElements ( ( (SSCP) my groups -> item[m]) -> columnLabels, thy columnLabels, 1, n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": group standard deviations not extracted.");
	}
}

double Discriminant_getWilksLambda (Discriminant me, long from) {
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	if (from >= numberOfFunctions) {
		return 1;
	}
	if (from < 1) {
		from = 1;
	}
	return NUMwilksLambda (my eigenvalues, 1 + from, numberOfFunctions);
}

/*
	raw r[j]: eigenvec[i][j]
	unstandardized u[j]: sqrt(N-g) * r[j]
	standardized s[j]: u[j] sqrt (w[i][i] / (N-g))
*/
TableOfReal Discriminant_extractCoefficients (Discriminant me, int choice) {
	try {
		int raw = choice == 0, standardized = choice == 2;
		long nx = my dimension, ny = my numberOfEigenvalues;

		SSCP total = my total;
		autoTableOfReal thee = TableOfReal_create (ny, nx + 1);
		NUMstrings_copyElements (my total -> columnLabels, thy columnLabels, 1, nx);

		autoSSCP within = 0;
		if (standardized) {
			within.reset (Discriminant_extractPooledWithinGroupsSSCP (me));
		}

		TableOfReal_setColumnLabel (thee.peek(), nx + 1, L"constant");
		TableOfReal_setSequentialRowLabels (thee.peek(), 1, ny, L"function_", 1, 1);

		double scale = sqrt (total -> numberOfObservations - my numberOfGroups);
		double *centroid = my total -> centroid;
		for (long i = 1; i <= ny; i++) {
			double u0 = 0, ui;
			for (long j = 1; j <= nx; j++) {
				if (standardized) {
					scale = sqrt (within -> data[j][j]);
				}
				thy data[i][j] = ui = scale * my eigenvectors[i][j];;
				u0 += ui * centroid[j];
			}
			thy data[i][nx + 1] = raw ? 0 : -u0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": coefficients not extracted.");
	}
}

static long Discriminant_getDegreesOfFreedom (Discriminant me) {
	long ndf = 0;
	for (long i = 1; i <= my groups -> size; i++) {
		ndf += SSCP_getDegreesOfFreedom (my groups -> item[i]);
	}
	return ndf;
}

void Discriminant_getPartialDiscriminationProbability (Discriminant me,
        long numberOfDimensions, double *probability, double *chisq, long *ndf) {
	long g = my numberOfGroups;
	long p = my dimension, k = numberOfDimensions;
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	double degreesOfFreedom = Discriminant_getDegreesOfFreedom (me);
	double lambda;

	*probability = 1; *chisq = 0; *ndf = 0;

	if (k >= numberOfFunctions) {
		return;
	}

	lambda = NUMwilksLambda (my eigenvalues, k + 1, numberOfFunctions);

	if (lambda == 1) {
		return;
	}

	*chisq = - (degreesOfFreedom + (g - p) / 2 - 1) * log (lambda);
	*ndf = (p - k) * (g - k - 1);
	*probability =  NUMchiSquareQ (*chisq, *ndf);
}

double Discriminant_getConcentrationEllipseArea (Discriminant me, long group,
        double scale, int confidence, int discriminantDirections, long d1, long d2) {
	SSCPs groups = my groups;
	double area = NUMundefined;

	if (group < 1 || group > my numberOfGroups) {
		return area;
	}

	if (discriminantDirections) {
		autoSSCP thee = Eigen_and_SSCP_project (me, (SSCP) groups -> item[group]);
		area = SSCP_getConcentrationEllipseArea (thee.peek(), scale, confidence, d1, d2);
	} else {
		area = SSCP_getConcentrationEllipseArea (groups -> item[group], scale, confidence, d1, d2);
	}
	return area;
}

double Discriminant_getLnDeterminant_group (Discriminant me, long group) {
	if (group < 1 || group > my numberOfGroups) {
		return NUMundefined;
	}
	autoCovariance c = SSCP_to_Covariance ( (SSCP) my groups -> item[group], 1);
	double ln_d = SSCP_getLnDeterminant (c.peek());
	return ln_d;
}

double Discriminant_getLnDeterminant_total (Discriminant me) {
	autoCovariance c = SSCP_to_Covariance (my total, 1);
	double ln_d = SSCP_getLnDeterminant (c.peek());
	return ln_d;
}

SSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me) {
	return SSCPs_to_SSCP_pool (my groups);
}

SSCP Discriminant_extractWithinGroupSSCP (Discriminant me, long index) {
	try {
		if (index < 1 || index > my numberOfGroups) Melder_throw
			("Index must be in interval [1,", my numberOfGroups, "].");
		autoSSCP thee = Data_copy ( (SSCP) my groups -> item[index]);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": within group SSCP not created.");
	}
}

SSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me) {
	try {
		long n = my total -> numberOfRows;
		autoSSCP b = Data_copy (my total);
		autoSSCP w = SSCPs_to_SSCP_pool (my groups);
		for (long i = 1; i <= n; i++) {
			for (long j = i; j <= n; j++) {
				b -> data[j][i] = (b -> data[i][j] -= w -> data[i][j]);
			}
		}
		return b.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": between group SSCP not created.");
	}
}

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g,
                                      int discriminantDirections, long d1, long d2, double xmin, double xmax,
                                      double ymin, double ymax, int fontSize, int poolCovarianceMatrices,
                                      int garnish) {
	(void) me; (void) g; (void) discriminantDirections; (void) d1; (void) d2;
	(void) xmin; (void) xmax; (void) ymin;
	(void) ymax; (void) fontSize; (void) poolCovarianceMatrices; (void) garnish;

}

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g,
        double scale, int confidence, wchar_t *label, int discriminantDirections, long d1, long d2,
        double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);

	if (! discriminantDirections) {
		SSCPs_drawConcentrationEllipses (my groups, g, scale, confidence, label,
		                                 d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	if (numberOfFunctions <= 1) {
		Melder_warning (L"Discriminant_drawConcentrationEllipses: Nothing drawn "
		                "because there is only one dimension in the discriminant space.");
		return;
	}

	// Project SSCPs on eigenvectors.

	if (d1 == 0 && d2 == 0) {
		d1 = 1;
		d2 = MIN (numberOfFunctions, d1 + 1);
	} else if (d1 < 0 || d2 > numberOfFunctions) {
		return;
	}

	double *v1 = my eigenvectors[d1];
	double *v2 = my eigenvectors[d2];

	autoSSCPs thee = SSCPs_toTwoDimensions (my groups, v1, v2);

	SSCPs_drawConcentrationEllipses (thee.peek(), g, scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		wchar_t llabel[40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		swprintf (llabel, 40, L"function %ld", d2);
		Graphics_textLeft (g, 1, llabel);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		swprintf (llabel, 40, L"function %ld", d1);
		Graphics_textBottom (g, 1, llabel);
	}
}

Discriminant TableOfReal_to_Discriminant (I) {
	iam (TableOfReal);
	try {
		autoDiscriminant thee = Thing_new (Discriminant);
		long dimension = my numberOfColumns;

		TableOfReal_areAllCellsDefined (me, 0, 0, 0, 0);

		if (NUMdmatrix_hasInfinities (my data, 1, my numberOfRows, 1, dimension)) {
			Melder_throw ("Table contains infinities.");
		}

		if (! TableOfReal_hasRowLabels (me)) {
			Melder_throw ("At least one of the rows has no label.");
		}

		autoTableOfReal mew = TableOfReal_sortOnlyByRowLabels (me);
		if (! TableOfReal_hasColumnLabels (mew.peek())) {
			TableOfReal_setSequentialColumnLabels (mew.peek(), 0, 0, L"c", 1, 1);
		}

		thy groups = TableOfReal_to_SSCPs_byLabel (mew.peek());
		thy total = TableOfReal_to_SSCP (mew.peek(), 0, 0, 0, 0);

		if ( (thy numberOfGroups = thy groups -> size) < 2) {
			Melder_throw ("Number of groups must be greater than one.");
		}

		TableOfReal_centreColumns_byRowLabel (mew.peek());

		// Overall centroid and apriori probabilities and costs.

		autoNUMvector<double> centroid (1, dimension);
		autoNUMmatrix<double> between (1, thy numberOfGroups, 1, dimension);
		thy aprioriProbabilities = NUMvector<double> (1, thy numberOfGroups);
		thy costs = NUMmatrix<double> (1, thy numberOfGroups, 1, thy numberOfGroups);

		double sum = 0, scale;
		for (long k = 1; k <= thy numberOfGroups; k++) {
			SSCP m = (SSCP) thy groups -> item[k];
			sum += scale = SSCP_getNumberOfObservations (m);
			for (long j = 1; j <= dimension; j++) {
				centroid[j] += scale * m -> centroid[j];
			}
		}

		for	(long j = 1; j <= dimension; j++) {
			centroid[j] /= sum;
		}

		for (long k = 1; k <= thy numberOfGroups; k++) {
			SSCP m = (SSCP) thy groups -> item[k];
			scale = SSCP_getNumberOfObservations (m);
			thy aprioriProbabilities[k] = scale / my numberOfRows;
			for (long j = 1; j <= dimension; j++) {
				between[k][j] = sqrt (scale) * (m -> centroid[j] - centroid[j]);
			}
		}

		// We need to solve B'B.x = lambda W'W.x, where B'B and W'W are the between and within covariance matrices.
		// We do not calculate these covariance matrices directly from the data but instead use the GSVD to solve for
		// the eigenvalues and eigenvectors of the equation.

		Eigen_initFromSquareRootPair (thee.peek(), between.peek(), thy numberOfGroups, dimension, mew -> data, my numberOfRows);

		// Default priors and costs

		for (long k = 1; k <= thy numberOfGroups; k++) {
			for (long j = k + 1; j <= thy numberOfGroups; j++) {
				thy costs[k][j] = thy costs[j][k] = 1;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Discriminant not created.");
	}
}

Configuration Discriminant_and_TableOfReal_to_Configuration (Discriminant me, TableOfReal thee, long numberOfDimensions) {
	try {
		if (numberOfDimensions == 0) {
			numberOfDimensions = Discriminant_getNumberOfFunctions (me);
		}
		autoConfiguration him = Configuration_create (thy numberOfRows, numberOfDimensions);
		Configuration thim = him.peek();
		Eigen_and_TableOfReal_project_into (me, thee, 1, thy numberOfColumns, & thim, 1, numberOfDimensions);
		TableOfReal_copyLabels (thee, thim, 1, 0);
		TableOfReal_setSequentialColumnLabels (him.peek(), 0, 0, L"Eigenvector ", 1, 1);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Configuration not created.");
	}
}

/*
	Calculate squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix (li) is the inverse L^-1 of the Cholesky decomposition
	S = L.L'.
*/
static double mahalanobisDistanceSq (double **li, long n, double *v, double *m,
                                     double *buf) {
	for (long i = 1; i <= n; i++) {
		buf[i] = v[i] - m[i];
	}

	double chisq = 0;
	for (long i = n; i > 0; i--) {
		double t = 0;
		for (long j = 1; j <= i; j++) {
			t += li[i][j] * buf[j];
		}
		chisq += t * t;
	}
	return chisq;
}

TableOfReal Discriminant_and_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, long group, bool poolCovarianceMatrices) {
	try {
		if (group < 1 || group > my numberOfGroups) {
			Melder_throw ("Group does not exist.");
		}
		autoSSCP pool = SSCPs_to_SSCP_pool (my groups);
		autoCovariance covg = SSCP_to_Covariance (pool.peek(), my numberOfGroups);
		autoCovariance cov = SSCP_to_Covariance ( (SSCP) my groups -> item[group], 1);
		autoTableOfReal him = 0;
		if (poolCovarianceMatrices) { // use group mean instead of overall mean!
			NUMvector_copyElements (cov -> centroid, covg -> centroid, 1, cov -> numberOfColumns);
			him.reset (Covariance_and_TableOfReal_mahalanobis (covg.peek(), thee, false));
		} else {
			him.reset (Covariance_and_TableOfReal_mahalanobis (cov.peek(), thee, false));
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal not created.");
	}
}

ClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable (Discriminant me, TableOfReal thee,
        int poolCovarianceMatrices, int useAprioriProbabilities) {
	try {
		long g = Discriminant_getNumberOfGroups (me);
		long p = Eigen_getDimensionOfComponents (me);
		long m = thy numberOfRows;

		if (p != thy numberOfColumns) Melder_throw
			("The number of columns does not agree with the dimension of the discriminant.");

		autoNUMvector<double> log_p (1, g);
		autoNUMvector<double> log_apriori (1, g);
		autoNUMvector<double> ln_determinant (1, g);
		autoNUMvector<double> buf (1, p);
		autoNUMvector<SSCP> sscpvec (1, g);
		autoSSCP pool = SSCPs_to_SSCP_pool (my groups);
		autoClassificationTable him = ClassificationTable_create (m, g);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m);

		// Scale the sscp to become a covariance matrix.

		for (long i = 1; i <= p; i++) {
			for (long k = i; k <= p; k++) {
				pool -> data[k][i] = (pool -> data[i][k] /= (pool -> numberOfObservations - g));
			}
		}

		double lnd;
		autoSSCPs agroups = 0; SSCPs groups;
		if (poolCovarianceMatrices) {
			/*
				Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
				L^-1 will be used later in the Mahalanobis distance calculation:
				v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).
			*/

			NUMlowerCholeskyInverse (pool -> data, p, &lnd);
			for (long j = 1; j <= g; j++) {
				ln_determinant[j] = lnd;
				sscpvec[j] = pool.peek();
			}
			groups = (SSCPs) my groups;
		} else {
			// Calculate the inverses of all group covariance matrices.
			// In case of a singular matrix, substitute inverse of pooled.

			agroups.reset (Data_copy ( (SSCPs) my groups)); groups = agroups.peek();
			long npool = 0;
			for (long j = 1; j <= g; j++) {
				SSCP t = (SSCP) groups -> item[j];
				long no = SSCP_getNumberOfObservations (t);
				for (long i = 1; i <= p; i++) {
					for (long k = i; k <= p; k++) {
						t -> data[k][i] = (t -> data[i][k] /= (no - 1));
					}
				}
				sscpvec[j] = (SSCP) groups -> item[j];
				try {
					NUMlowerCholeskyInverse (t -> data, p, &ln_determinant[j]);
				} catch (MelderError) {
					// Try the alternative: the pooled covariance matrix.
					// Clear the error.

					Melder_clearError ();
					if (npool == 0) {
						NUMlowerCholeskyInverse (pool -> data, p, &lnd);
					}
					npool++;
					sscpvec[j] = pool.peek();
					ln_determinant[j] = lnd;
				}
			}
			if (npool > 0) {
				Melder_warning (Melder_integer (npool), L" groups use pooled covariance matrix.");
			}
		}

		// Labels for columns in ClassificationTable

		for (long j = 1; j <= g; j++) {
			const wchar_t *name = Thing_getName ( (Thing) my groups -> item[j]);
			if (! name) {
				name = L"?";
			}
			TableOfReal_setColumnLabel (him.peek(), j, name);
		}

		// Normalize the sum of the apriori probabilities to 1.
		// Next take ln (p) because otherwise probabilities might be too small to represent.

		NUMvector_normalize1 (my aprioriProbabilities, g);
		double logg = log (g);
		for (long j = 1; j <= g; j++) {
			log_apriori[j] = useAprioriProbabilities ? log (my aprioriProbabilities[j]) : - logg;
		}

		// Generalized squared distance function:
		// D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)

		for (long i = 1; i <= m; i++) {
			double norm = 0, pt_max = -1e38;
			for (long j = 1; j <= g; j++) {
				SSCP t = (SSCP) groups -> item[j];
				double md = mahalanobisDistanceSq (sscpvec[j] -> data, p, thy data[i], t -> centroid, buf.peek());
				double pt = log_apriori[j] - 0.5 * (ln_determinant[j] + md);
				if (pt > pt_max) {
					pt_max = pt;
				}
				log_p[j] = pt;
			}
			for (long j = 1; j <= g; j++) {
				norm += (log_p[j] = exp (log_p[j] - pt_max));
			}
			for (long j = 1; j <= g; j++) {
				his data[i][j] = log_p[j] / norm;
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("ClassificationTable from Discriminant & TableOfReal not created.");
	}
}

ClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable_dw (Discriminant me, TableOfReal thee,
        int poolCovarianceMatrices, int useAprioriProbabilities, double alpha, double minProb, TableOfReal *displacements) {
	try {
		long g = Discriminant_getNumberOfGroups (me);
		long p = Eigen_getDimensionOfComponents (me);
		long m = thy numberOfRows;

		if (p != thy numberOfColumns) Melder_throw
			("The number of columns does not agree with the dimension of the discriminant.");

		autoNUMvector<double> log_p (1, g);
		autoNUMvector<double> log_apriori (1, g);
		autoNUMvector<double> ln_determinant (1, g);
		autoNUMvector<double> buf (1, p);
		autoNUMvector<double> displacement (1, p);
		autoNUMvector<double> x (1, p);
		autoNUMvector<SSCP> sscpvec (1, g);
		autoSSCP pool = SSCPs_to_SSCP_pool (my groups);
		autoClassificationTable him = ClassificationTable_create (m, g);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m);
		autoTableOfReal adisplacements = Data_copy (thee);

		// Scale the sscp to become a covariance matrix.

		for (long i = 1; i <= p; i++) {
			for (long k = i; k <= p; k++) {
				pool -> data[k][i] = (pool -> data[i][k] /= (pool -> numberOfObservations - g));
			}
		}

		double lnd;
		autoSSCPs agroups = 0; SSCPs groups;
		if (poolCovarianceMatrices) {
			// Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
			// L^-1 will be used later in the Mahalanobis distance calculation:
			// v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).

			NUMlowerCholeskyInverse (pool -> data, p, &lnd);
			for (long j = 1; j <= g; j++) {
				ln_determinant[j] = lnd;
				sscpvec[j] = pool.peek();
			}
			groups = my groups;
		} else {
			//Calculate the inverses of all group covariance matrices.
			// In case of a singular matrix, substitute inverse of pooled.

			agroups.reset (Data_copy ( (SSCPs) my groups)); groups = agroups.peek();
			long npool = 0;
			for (long j = 1; j <= g; j++) {
				SSCP t = (SSCP) groups -> item[j];
				long no = SSCP_getNumberOfObservations (t);
				for (long i = 1; i <= p; i++) {
					for (long k = i; k <= p; k++) {
						t -> data[k][i] = (t -> data[i][k] /= (no - 1));
					}
				}
				sscpvec[j] = (SSCP) groups -> item[j];
				try {
					NUMlowerCholeskyInverse (t -> data, p, &ln_determinant[j]);
				} catch (MelderError) {
					// Try the alternative: the pooled covariance matrix.
					// Clear the error.

					Melder_clearError ();
					if (npool == 0) {
						NUMlowerCholeskyInverse (pool -> data, p, &lnd);
					}
					npool++;
					sscpvec[j] = pool.peek();
					ln_determinant[j] = lnd;
				}
			}
			if (npool > 0) {
				Melder_warning (Melder_integer (npool), L" groups use pooled covariance matrix.");
			}
		}

		// Labels for columns in ClassificationTable

		for (long j = 1; j <= g; j++) {
			const wchar_t *name = Thing_getName ( (Thing) my groups -> item[j]);
			if (! name) {
				name = L"?";
			}
			TableOfReal_setColumnLabel (him.peek(), j, name);
		}

		// Normalize the sum of the apriori probabilities to 1.
		// Next take ln (p) because otherwise probabilities might be too small to represent.

		double logg = log (g);
		NUMvector_normalize1 (my aprioriProbabilities, g);
		for (long j = 1; j <= g; j++) {
			log_apriori[j] = useAprioriProbabilities ? log (my aprioriProbabilities[j]) : - logg;
		}

		// Generalized squared distance function:
		// D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)

		for (long i = 1; i <= m; i++) {
			SSCP winner;
			double norm = 0, pt_max = -1e38;
			long iwinner = 1;
			for (long k = 1; k <= p; k++) {
				x[k] = thy data[i][k] + displacement[k];
			}
			for (long j = 1; j <= g; j++) {
				SSCP t = (SSCP) groups -> item[j];
				double md = mahalanobisDistanceSq (sscpvec[j] -> data, p, x.peek(), t -> centroid, buf.peek());
				double pt = log_apriori[j] - 0.5 * (ln_determinant[j] + md);
				if (pt > pt_max) {
					pt_max = pt; iwinner = j;
				}
				log_p[j] = pt;
			}
			for (long j = 1; j <= g; j++) {
				norm += (log_p[j] = exp (log_p[j] - pt_max));
			}

			for (long j = 1; j <= g; j++) {
				his data[i][j] = log_p[j] / norm;
			}

			// Save old displacement, calculate new displacement

			winner = (SSCP) groups -> item[iwinner];
			for (long k = 1; k <= p; k++) {
				adisplacements -> data[i][k] = displacement[k];
				if (his data[i][iwinner] > minProb) {
					double delta_k = winner -> centroid[k] - x[k];
					displacement[k] += alpha * delta_k;
				}
			}
		}
		*displacements = adisplacements.transfer();
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("ClassificationTable for Weenink procedure not created.");
	}
}

Configuration TableOfReal_to_Configuration_lda (TableOfReal me, long numberOfDimensions) {
	try {
		autoDiscriminant thee = TableOfReal_to_Discriminant (me);
		autoConfiguration him = Discriminant_and_TableOfReal_to_Configuration (thee.peek(), me, numberOfDimensions);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Configuration with lda data not created.");
	}
}

#undef MAX
#undef MIN

/* End of file Discriminant.c 1043*/
