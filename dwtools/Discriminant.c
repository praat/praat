/* Discriminant.c
 *
 * Copyright (C) 1993-2011 David Weenink
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

static void info (I) {
	iam (Discriminant);

	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of groups: ", Melder_integer (my numberOfGroups));
	MelderInfo_writeLine2 (L"Number of variables: ", Melder_integer (my dimension));
	MelderInfo_writeLine2 (L"Number of discriminant functions: ",
		Melder_integer (Discriminant_getNumberOfFunctions (me)));
	MelderInfo_writeLine2 (L"Number of observations (total): ",
		Melder_integer (Discriminant_getNumberOfObservations (me, 0)));
}

class_methods (Discriminant, Eigen)
	class_method_local (Discriminant, destroy)
	class_method_local (Discriminant, description)
	class_method_local (Discriminant, copy)
	class_method_local (Discriminant, equal)
	class_method_local (Discriminant, canWriteAsEncoding)
	class_method_local (Discriminant, writeText)
	class_method_local (Discriminant, readText)
	class_method_local (Discriminant, writeBinary)
	class_method_local (Discriminant, readBinary)
	class_method (info)
class_methods_end


Discriminant Discriminant_create (long numberOfGroups, long numberOfEigenvalues, long dimension)
{
	Discriminant me = Thing_new (Discriminant);

	if (! me) return NULL;
	my numberOfGroups = numberOfGroups;
	if (! Eigen_init (me, numberOfEigenvalues, dimension) ||
		((my groups = SSCPs_create ()) == NULL) ||
		((my total = SSCP_create (dimension)) == NULL) ||
		((my aprioriProbabilities = NUMdvector (1, numberOfGroups)) == NULL) ||
		((my costs = NUMdmatrix (1, numberOfGroups, 1, numberOfGroups)) == NULL)) forget (me);
	return me;
}

long Discriminant_groupLabelToIndex (Discriminant me, const wchar_t *label)
{
	wchar_t *name;

	for (long i = 1; i <= my numberOfGroups; i++)
	{
		if ((name = Thing_getName (my groups -> item[i])) && wcsequ (name, label))
			return i;
	}
	return 0;
}

long Discriminant_getNumberOfGroups (Discriminant me)
{
	return my numberOfGroups;
}

long Discriminant_getNumberOfObservations (Discriminant me, long group)
{
	if (group == 0)
	{
		SSCP sscp = my total;
		return sscp -> numberOfObservations;
	}
	else if (group >= 1 && group <= my numberOfGroups)
	{
		SSCP sscp = my groups -> item[group];
		return sscp -> numberOfObservations;
	}
	else return -1;
}

int Discriminant_setAprioriProbability (Discriminant me, long group, double p)
{
	if (group < 1 || group > my numberOfGroups) return Melder_error5
		(L"Group (", Melder_integer (group), L") must be in interval [1, ", Melder_integer(my numberOfGroups), L"].");
	if (p < 0 || p > 1) return Melder_error1 (L"Probability must be in interval [0,1]");
	my aprioriProbabilities[group] = p;
	return 1;
}

long Discriminant_getNumberOfFunctions (Discriminant me)
{
	long nf = MIN (my numberOfGroups - 1, my dimension);
	nf = MIN (nf, my numberOfEigenvalues);
	return nf;
}

int Discriminant_setGroupLabels (Discriminant me, Strings thee)
{
	long i;

	if (my numberOfGroups != thy numberOfStrings) return Melder_error1
		(L"Discriminant_setGroupLabels: The number of strings must equal the number of groups.");

	for (i=1; i <= my numberOfGroups; i++)
	{
		wchar_t *noname = L"", *name;
		name = thy strings[i];
		if (name == NULL) name = noname;
		Thing_setName (my groups -> item[i], name);
	}
	return 1;
}

Strings Discriminant_extractGroupLabels (Discriminant me)
{
	Strings thee = Thing_new (Strings);
	long i;

	if (thee == NULL) return NULL;
	thy strings = NUMpvector (1, my numberOfGroups);
	thy numberOfStrings = my numberOfGroups;
	if (thy strings == NULL) goto end;
	for (i=1; i <= my numberOfGroups; i++)
	{
		wchar_t *name = Thing_getName (my groups -> item[i]);
		thy strings[i] = Melder_wcsdup_e (name); cherror
	}
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

TableOfReal Discriminant_extractGroupCentroids (Discriminant me)
{
	TableOfReal thee = NULL;
	SSCP sscp = NULL;
	long i, m = my groups -> size, n = my dimension;

	if ((thee = TableOfReal_create (m, n)) == NULL) return NULL;

	for (i=1; i <= m; i++)
	{
		sscp = my groups -> item[i];
		TableOfReal_setRowLabel (thee, i, Thing_getName (sscp));
		NUMdvector_copyElements (sscp -> centroid, thy data[i], 1, n);
	}
	NUMstrings_copyElements (sscp -> columnLabels, thy columnLabels, 1, n);
	return thee;
}

TableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me)
{
	TableOfReal thee = NULL;
	SSCP sscp = NULL;
	long i, j, numberOfObservationsm1;
	long m = my groups -> size, n = my dimension;

	if ((thee = TableOfReal_create (m, n)) == NULL) return NULL;

	for (i=1; i <= m; i++)
	{
		sscp = my groups -> item[i];
		TableOfReal_setRowLabel (thee, i, Thing_getName (sscp));
		numberOfObservationsm1 = sscp -> numberOfObservations - 1;
		for (j=1; j <= n; j++) thy data[i][j] = numberOfObservationsm1 > 0 ?
			sqrt (sscp -> data[j][j] / numberOfObservationsm1) : NUMundefined;
	}
	NUMstrings_copyElements (sscp -> columnLabels, thy columnLabels, 1, n);
	return thee;
}

double Discriminant_getWilksLambda (Discriminant me, long from)
{
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	if (from >= numberOfFunctions) return 1;
	if (from < 1) from = 1;
	return NUMwilksLambda (my eigenvalues, 1 + from, numberOfFunctions);
}

/*
	raw r[j]: eigenvec[i][j]
	unstandardized u[j]: sqrt(N-g) * r[j]
	standardized s[j]: u[j] sqrt (w[i][i] / (N-g))
*/
TableOfReal Discriminant_extractCoefficients (Discriminant me, int choice)
{
	TableOfReal thee = NULL;
	SSCP within = NULL, total = my total;
	int raw = choice == 0, standardized = choice == 2;
	long i, j, nx = my dimension, ny = my numberOfEigenvalues;
	double *centroid = my total -> centroid;
	double scale = sqrt (total -> numberOfObservations - my numberOfGroups);


	thee = TableOfReal_create (ny, nx + 1);
	if (thee == NULL) return NULL;

	if (! NUMstrings_copyElements (my total -> columnLabels, thy columnLabels,
		1, nx)) goto end;

	if (standardized)
	{
		within = Discriminant_extractPooledWithinGroupsSSCP (me);
		if (within == NULL) goto end;
	}

	TableOfReal_setColumnLabel (thee, nx + 1, L"constant");
	(void) TableOfReal_setSequentialRowLabels (thee, 1, ny, L"function_", 1, 1);
	for (i = 1; i <= ny; i++)
	{
		double u0 = 0, ui;
		for (j=1; j <= nx; j++)
		{
			if (standardized) scale = sqrt (within -> data[j][j]);
			thy data[i][j] = ui = scale * my eigenvectors[i][j];;
			u0 += ui * centroid[j];
		}
		thy data[i][nx + 1] = raw ? 0 : -u0;
	}

end:

	if (standardized) forget (within);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static long Discriminant_getDegreesOfFreedom (Discriminant me)
{
	long i, ndf = 0;

	for (i=1; i <= my groups -> size; i++)
	{
		ndf += SSCP_getDegreesOfFreedom (my groups -> item[i]);
	}
	return ndf;
}

void Discriminant_getPartialDiscriminationProbability (Discriminant me,
	long numberOfDimensions, double *probability, double *chisq, long *ndf)
{
	long g = my numberOfGroups;
	long p = my dimension, k = numberOfDimensions;
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	double degreesOfFreedom = Discriminant_getDegreesOfFreedom (me);
	double lambda;

	*probability = 1; *chisq = 0; *ndf = 0;

	if (k >= numberOfFunctions) return;

	lambda = NUMwilksLambda (my eigenvalues, k + 1, numberOfFunctions);

	if (lambda == 1) return;

	*chisq = -(degreesOfFreedom + (g - p) / 2 - 1) * log (lambda);
	*ndf = (p - k) * (g - k - 1);
	*probability =  NUMchiSquareQ (*chisq, *ndf);
}

double Discriminant_getConcentrationEllipseArea (Discriminant me, long group,
	double scale, int confidence, int discriminantDirections, long d1, long d2)
{
	SSCPs groups = my groups;
	double area = NUMundefined;

	if (group < 1 || group > my numberOfGroups) return area;

	if (discriminantDirections)
	{
		SSCP thee = Eigen_and_SSCP_project (me, groups -> item[group]);
		if (thee != NULL)
		{
			area = SSCP_getConcentrationEllipseArea (thee, scale, confidence,
				d1, d2);
			forget (thee);
		}
	}
	else
	{
		area = SSCP_getConcentrationEllipseArea(groups -> item[group], scale,
			confidence, d1, d2);
	}
	return area;
}

double Discriminant_getLnDeterminant_group (Discriminant me, long group)
{
	Covariance c = NULL;
	double ln_d;

	if (group < 1 || group > my numberOfGroups) return NUMundefined;

	c = SSCP_to_Covariance(my groups -> item[group], 1);
	if (c == NULL) return NUMundefined;

	ln_d = SSCP_getLnDeterminant (c);
	forget (c);
	return ln_d;
}

double Discriminant_getLnDeterminant_total (Discriminant me)
{
	Covariance c = NULL;
	double ln_d;

	c = SSCP_to_Covariance(my total, 1);
	if (c == NULL) return NUMundefined;

	ln_d = SSCP_getLnDeterminant (c);
	forget (c);
	return ln_d;
}

SSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me)
{
	return SSCPs_to_SSCP_pool (my groups);
}

SSCP Discriminant_extractWithinGroupSSCP (Discriminant me, long index)
{
	SSCP thee;

	if (index < 1 || index > my numberOfGroups) return Melder_errorp3
		(L"Index must be in interval [1,", Melder_integer (my numberOfGroups), L"].");
	thee = Data_copy (my groups -> item[index]);
	return thee;
}

SSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me)
{
	SSCP b = NULL, w = NULL;
	long i, j, n = my total -> numberOfRows;

	if (((b = Data_copy (my total)) == NULL) ||
		((w = SSCPs_to_SSCP_pool (my groups)) == NULL)) goto end;

	for (i=1; i <= n; i++)
	{
		for (j=i; j <= n; j++)
		{
			b -> data[j][i] = (b -> data[i][j] -= w -> data[i][j]);
		}
	}

end:

	forget (w);
	if (Melder_hasError ()) forget (b);
	return b;
}

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g,
	int discriminantDirections, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int poolCovarianceMatrices,
	int garnish)
{
	(void) me;(void) g;(void) discriminantDirections;(void) d1;(void) d2;
	(void) xmin;(void) xmax;(void) ymin;
	(void) ymax;(void) fontSize;(void) poolCovarianceMatrices;(void) garnish;

}

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g,
	double scale, int confidence, wchar_t *label, int discriminantDirections, long d1, long d2,
	double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish)
{
	SSCPs thee;
	long numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	double *v1, *v2;

	if (! discriminantDirections)
	{
		SSCPs_drawConcentrationEllipses (my groups, g, scale, confidence, label,
			d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	if (numberOfFunctions <= 1)
	{
		Melder_warning1 (L"Discriminant_drawConcentrationEllipses: Nothing drawn "
			"because there is only one dimension in the discriminant space.");
		return;
	}

	/*
		Project SSCPs on eigenvectors.
	*/

	if (d1 == 0 && d2 == 0)
	{
		d1 = 1;
		d2 = MIN (numberOfFunctions, d1 + 1);
	}
	else if (d1 < 0 || d2 > numberOfFunctions) return;

	v1 = my eigenvectors[d1]; v2 = my eigenvectors[d2];

	if ((thee = SSCPs_toTwoDimensions (my groups, v1, v2)) == NULL) return;

	SSCPs_drawConcentrationEllipses (thee, g, scale, confidence, label, 1, 2,
		xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish)
	{
		wchar_t label[40];
    	Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	swprintf (label, 40, L"function %ld", d2);
    	Graphics_textLeft (g, 1, label);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	swprintf (label, 40, L"function %ld", d1);
		Graphics_textBottom (g, 1, label);
	}

	forget (thee);
}

Discriminant TableOfReal_to_Discriminant (I)
{
	iam (TableOfReal);
	Discriminant thee = Thing_new (Discriminant);
	TableOfReal mew = NULL;
	double **between = NULL, *centroid = NULL, scale, sum;
	long j, k, dimension = my numberOfColumns;

	if (thee == NULL) return NULL;

	if (! TableOfReal_areAllCellsDefined (me, 0, 0, 0, 0)) return NULL;

	if (NUMdmatrix_hasInfinities (my data, 1, my numberOfRows, 1, dimension))
	{
		(void) Melder_error1 (L"Table contains infinities.");
		goto end;
	}
	if (! TableOfReal_hasRowLabels (me))
	{
		(void) Melder_error1 (L"At least one of the rows has no label.");
		goto end;
	}
	mew = TableOfReal_sortOnlyByRowLabels (me);
	if (mew == NULL) goto end;
	if (! TableOfReal_hasColumnLabels (mew) &&
		! TableOfReal_setSequentialColumnLabels (mew, 0, 0, L"c", 1, 1))
	{
		(void) Melder_error1 (L"There were no column labels. We could not set them either.");
		goto end;
	}

	thy groups = TableOfReal_to_SSCPs_byLabel (mew);
	if (thy groups == NULL) goto end;
	thy total = TableOfReal_to_SSCP (mew, 0, 0, 0, 0);
	if (thy total == NULL) goto end;

	if ((thy numberOfGroups = thy groups -> size) < 2)
	{
		(void) Melder_error1 (L"Number of groups must be greater than one.");
		goto end;
	}

	TableOfReal_centreColumns_byRowLabel (mew);

	/*
		Overall centroid and apriori probabilities and costs.
	*/
	centroid = NUMdvector (1, dimension);
	if (centroid == NULL) goto end;
	between = NUMdmatrix (1, thy numberOfGroups, 1, dimension);
	if (between == NULL) goto end;
	thy aprioriProbabilities = NUMdvector (1, thy numberOfGroups);
	if (thy aprioriProbabilities == NULL) goto end;
	thy costs = NUMdmatrix (1, thy numberOfGroups, 1, thy numberOfGroups);
	if (thy costs == NULL) goto end;


	for (sum = 0, k = 1; k <= thy numberOfGroups; k++)
	{
		SSCP m = thy groups -> item[k];
		sum += scale = SSCP_getNumberOfObservations (m);
		for (j = 1; j <= dimension; j++)
		{
			centroid[j] += scale * m -> centroid[j];
		}
	}

	for	(j = 1; j <= dimension; j++) centroid[j] /= sum;

	for (k = 1; k <= thy numberOfGroups; k++)
	{
		SSCP m = thy groups -> item[k];
		scale = SSCP_getNumberOfObservations (m);
		thy aprioriProbabilities[k] = scale / my numberOfRows;
		for (j = 1; j <= dimension; j++)
		{
			between[k][j] = sqrt (scale) * (m -> centroid[j] - centroid[j]);
		}
	}

	/*
		We need to solve B'B.x = lambda W'W.x, where B'B and W'W are the between
		and within covariance matrices.
		We do not calculate these covariance matrices directly from the
		data but instead use the GSVD to solve for the eigenvalues and
		eigenvectors of the equation.
	*/

	if (! Eigen_initFromSquareRootPair (thee, between, thy numberOfGroups,
		dimension, mew -> data, my numberOfRows)) goto end;

	/*
		Default priors and costs
	*/

	for (k = 1; k <= thy numberOfGroups; k++)
	{
		for (j = k + 1; j <= thy numberOfGroups; j++)
		{
			thy costs[k][j] = thy costs[j][k] = 1;
		}
	}

end:

	forget (mew);
	NUMdvector_free (centroid, 1);
	NUMdmatrix_free (between, 1, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Configuration Discriminant_and_TableOfReal_to_Configuration
	(Discriminant me, TableOfReal thee, long numberOfDimensions)
{
	Configuration him = NULL;

	if (numberOfDimensions == 0)
	{
		numberOfDimensions = Discriminant_getNumberOfFunctions (me);
	}
	him = Configuration_create (thy numberOfRows, numberOfDimensions);
	if (him == NULL) return NULL;

	if (! Eigen_and_TableOfReal_project_into (me, thee, 1, thy numberOfColumns,
		& him, 1, numberOfDimensions) ||
		! TableOfReal_copyLabels (thee, him, 1, 0) ||
		! TableOfReal_setSequentialColumnLabels (him, 0, 0, L"Eigenvector ", 1, 1))
	{
		forget (him);
	}

	return him;
}

/*
	Calculate squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix (li) is the inverse L^-1 of the Cholesky decomposition
	S = L.L'.
*/
static double mahalanobisDistanceSq (double **li, long n, double *v, double *m,
	double *buf)
{
	long i, j;
	double chisq = 0;

	for (i = 1; i <= n; i++)
	{
		buf[i] = v[i] - m[i];
	}

	for (i = n; i > 0; i--)
	{
		double t = 0;
		for (j = 1; j <= i; j++)
		{
			t += li[i][j] * buf[j];
		}
		chisq += t * t;
	}
	return chisq;
}

TableOfReal Discriminant_and_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, long group, bool poolCovarianceMatrices)
{
	TableOfReal him = NULL;
	SSCP pool = NULL;
	Covariance covg = NULL, cov = NULL;

	if (group < 1 || group > my numberOfGroups) return Melder_errorp1 (L"Group does not exist.");

	if (((pool = SSCPs_to_SSCP_pool (my groups)) == NULL) ||
		((covg = SSCP_to_Covariance (pool, my numberOfGroups)) == NULL) ||
		((cov = SSCP_to_Covariance (my groups -> item[group], 1)) == NULL)) goto end;

	if (poolCovarianceMatrices) // use group mean instead of overall mean!
	{
		NUMdvector_copyElements (cov -> centroid, covg -> centroid, 1, cov -> numberOfColumns);
		him = Covariance_and_TableOfReal_mahalanobis (covg, thee, false);
	}
	else
	{
		him = Covariance_and_TableOfReal_mahalanobis (cov, thee, false);
	}
end:
	forget (pool);
	forget (cov);
	forget (covg);
	if (Melder_hasError ()) forget (him);
	return him;
}

ClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable
	(Discriminant me, TableOfReal thee, int poolCovarianceMatrices,
	int useAprioriProbabilities)
{
	ClassificationTable him = NULL;
	SSCPs groups = NULL;
	SSCP pool = NULL, *sscpvec = NULL;
	long g = Discriminant_getNumberOfGroups (me);
	long p = Eigen_getDimensionOfComponents (me);
	long i, j, k, m = thy numberOfRows, npool = 0;
	double lnd, logg = log (g);
	double *log_p = NULL, *log_apriori = NULL, *ln_determinant = NULL;
	double *buf = NULL;

	if (p != thy numberOfColumns) return Melder_errorp1
		(L"The number of columns does not agree with the dimension of the discriminant.");

	if (((log_p = NUMdvector (1, g)) == NULL) ||
		((log_apriori = NUMdvector (1, g)) == NULL) ||
		((ln_determinant = NUMdvector (1, g)) == NULL) ||
		((buf = NUMdvector (1, p)) == NULL) ||
		((sscpvec = (SSCP *) NUMpvector (1, g)) == NULL) ||
		((pool = SSCPs_to_SSCP_pool (my groups)) == NULL) ||
		((him = ClassificationTable_create (m, g)) == NULL) ||
		! NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m)) goto end;

	/*
		Scale the sscp to become a covariance matrix.
	*/

	for (i = 1; i <= p; i++)
	{
		for (k = i; k <= p; k++)
		{
			pool -> data[k][i] =
				(pool -> data[i][k] /= (pool -> numberOfObservations - g));
		}
	}

	if (poolCovarianceMatrices)
	{
		/*
			Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
			L^-1 will be used later in the Mahalanobis distance calculation:
			v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).
		*/

		if (! NUMlowerCholeskyInverse (pool -> data, p, &lnd)) goto end;
		for (j = 1; j <= g; j++)
		{
			ln_determinant[j] = lnd;
			sscpvec[j] = pool;
		}
		groups = my groups;
	}
	else
	{
		/*
			Calculate the inverses of all group covariance matrices.
			In case of a singular matrix, substitute inverse of pooled.
		*/

		if ((groups = Data_copy (my groups)) == NULL) goto end;

		for (npool = 0, j = 1; j <= g; j++)
		{
			SSCP t = groups -> item[j];
			long no = SSCP_getNumberOfObservations (t);
			for (i = 1; i <= p; i++)
			{
				for (k = i; k <= p; k++)
					{
						t -> data[k][i] = (t -> data[i][k] /= (no - 1));
					}
			}
			sscpvec[j] = groups -> item[j];
			if (! NUMlowerCholeskyInverse (t -> data, p, &ln_determinant[j]))
			{
				/*
					Try the alternative: the pooled covariance matrix.
					Clear the error.
				*/

				Melder_clearError ();
				if (npool == 0 &&
					! NUMlowerCholeskyInverse (pool -> data, p, &lnd)) goto end;
				npool++;
				sscpvec[j] = pool;
				ln_determinant[j] = lnd;
			}
		}
		if (npool > 0) Melder_warning3 (L"Discriminant_and_TableOfReal_to_ClassificationTable: ", Melder_integer (npool),
			L" groups use pooled covariance matrix.");
	}

	/*
		Labels for columns in ClassificationTable
	*/

	for (j = 1; j <= g; j++)
	{
		wchar_t *name = Thing_getName (my groups -> item[j]);
		if (! name ) name = L"?";
		TableOfReal_setColumnLabel (him, j, name);
	}

	/*
		Normalize the sum of the apriori probabilities to 1.
		Next take ln (p) because otherwise probabilities might be too small
		to represent.
	*/

	NUMdvector_normalize1 (my aprioriProbabilities, g);
	for (j = 1; j <= g; j++)
	{
		log_apriori[j] = useAprioriProbabilities ?
			log (my aprioriProbabilities[j]) : - logg;
	}

	/*
		Generalized squared distance function:
		D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)
	*/

	for (i = 1; i <= m; i++)
	{
		double norm = 0, pt_max = -1e38;
		for (j = 1; j <= g; j++)
		{
			SSCP t = groups -> item[j];
			double md = mahalanobisDistanceSq (sscpvec[j] -> data, p,
				thy data[i], t -> centroid, buf);
			double pt = log_apriori[j] - 0.5 * (ln_determinant[j] + md);
			if (pt > pt_max) pt_max = pt;
			log_p[j] = pt;
		}
		for (j = 1; j <= g; j++)
		{
			norm += (log_p[j] = exp (log_p[j] - pt_max));
		}
		for (j = 1; j <= g; j++)
		{
			his data[i][j] = log_p[j] / norm;
		}
	}

end:

	if (! poolCovarianceMatrices) forget (groups);
	NUMdvector_free (log_apriori, 1);
	NUMdvector_free (log_p, 1);
	NUMpvector_free (sscpvec, 1);
	NUMdvector_free (ln_determinant, 1);
	forget (pool);
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

ClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable_dw
	(Discriminant me, TableOfReal thee, int poolCovarianceMatrices,
	int useAprioriProbabilities, double alpha, double minProb,
	TableOfReal *displacements)
{
	ClassificationTable him = NULL;
	SSCPs groups = NULL;
	SSCP pool = NULL, *sscpvec = NULL;
	long g = Discriminant_getNumberOfGroups (me);
	long p = Eigen_getDimensionOfComponents (me);
	long i, j, k, m = thy numberOfRows, npool = 0;
	double lnd, logg = log (g);
	double *log_p = NULL, *log_apriori = NULL, *ln_determinant = NULL;
	double *buf = NULL, *displacement = NULL, *x = NULL;

	if (p != thy numberOfColumns) return Melder_errorp1
		(L"The number of columns does not agree with the dimension of the discriminant.");

	if (((log_p = NUMdvector (1, g)) == NULL) ||
		((log_apriori = NUMdvector (1, g)) == NULL) ||
		((ln_determinant = NUMdvector (1, g)) == NULL) ||
		((buf = NUMdvector (1, p)) == NULL) ||
		((displacement = NUMdvector (1, p)) == NULL) ||
		((x = NUMdvector (1, p)) == NULL) ||
		((sscpvec = (SSCP *) NUMpvector (1, g)) == NULL) ||
		((pool = SSCPs_to_SSCP_pool (my groups)) == NULL) ||
		((him = ClassificationTable_create (m, g)) == NULL) ||
		((*displacements = Data_copy (thee)) == NULL) ||
		! NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, m)) goto end;

	/*
		Scale the sscp to become a covariance matrix.
	*/

	for (i = 1; i <= p; i++)
	{
		for (k = i; k <= p; k++)
		{
			pool -> data[k][i] =
				(pool -> data[i][k] /= (pool -> numberOfObservations - g));
		}
	}

	if (poolCovarianceMatrices)
	{
		/*
			Covariance matrix S can be decomposed as S = L.L'. Calculate L^-1.
			L^-1 will be used later in the Mahalanobis distance calculation:
			v'.S^-1.v == v'.L^-1'.L^-1.v == (L^-1.v)'.(L^-1.v).
		*/

		if (! NUMlowerCholeskyInverse (pool -> data, p, &lnd)) goto end;
		for (j = 1; j <= g; j++)
		{
			ln_determinant[j] = lnd;
			sscpvec[j] = pool;
		}
		groups = my groups;
	}
	else
	{
		/*
			Calculate the inverses of all group covariance matrices.
			In case of a singular matrix, substitute inverse of pooled.
		*/

		if ((groups = Data_copy (my groups)) == NULL) goto end;

		for (npool = 0, j = 1; j <= g; j++)
		{
			SSCP t = groups -> item[j];
			long no = SSCP_getNumberOfObservations (t);
			for (i = 1; i <= p; i++)
			{
				for (k = i; k <= p; k++)
					{
						t -> data[k][i] = (t -> data[i][k] /= (no - 1));
					}
			}
			sscpvec[j] = groups -> item[j];
			if (! NUMlowerCholeskyInverse (t -> data, p, &ln_determinant[j]))
			{
				/*
					Try the alternative: the pooled covariance matrix.
					Clear the error.
				*/

				Melder_clearError ();
				if (npool == 0 &&
					! NUMlowerCholeskyInverse (pool -> data, p, &lnd)) goto end;
				npool++;
				sscpvec[j] = pool;
				ln_determinant[j] = lnd;
			}
		}
		if (npool > 0) Melder_warning3 (L"Discriminant_and_TableOfReal_to_ClassificationTable: ",
			Melder_integer (npool), L" groups use pooled covariance matrix.");
	}

	/*
		Labels for columns in ClassificationTable
	*/

	for (j = 1; j <= g; j++)
	{
		wchar_t *name = Thing_getName (my groups -> item[j]);
		if (! name ) name = L"?";
		TableOfReal_setColumnLabel (him, j, name);
	}

	/*
		Normalize the sum of the apriori probabilities to 1.
		Next take ln (p) because otherwise probabilities might be too small
		to represent.
	*/

	NUMdvector_normalize1 (my aprioriProbabilities, g);
	for (j = 1; j <= g; j++)
	{
		log_apriori[j] = useAprioriProbabilities ?
			log (my aprioriProbabilities[j]) : - logg;
	}

	/*
		Generalized squared distance function:
		D^2(x) = (x - mu)' S^-1 (x - mu) + ln (determinant(S)) - 2 ln (apriori)
	*/

	for (i = 1; i <= m; i++)
	{
		SSCP winner;
		double norm = 0, pt_max = -1e38;
		long iwinner = 1;
		for (k = 1; k <= p; k++)
		{
			x[k] = thy data[i][k] + displacement[k];
		}
		for (j = 1; j <= g; j++)
		{
			SSCP t = groups -> item[j];
			double md = mahalanobisDistanceSq (sscpvec[j] -> data, p,
				x, t -> centroid, buf);
			double pt = log_apriori[j] - 0.5 * (ln_determinant[j] + md);
			if (pt > pt_max)
			{
				pt_max = pt; iwinner = j;
			}
			log_p[j] = pt;
		}
		for (j = 1; j <= g; j++)
		{
			norm += (log_p[j] = exp (log_p[j] - pt_max));
		}

		for (j = 1; j <= g; j++)
		{
			his data[i][j] = log_p[j] / norm;
		}

		/*
			Save old displacement, calculate new displacement
		*/
		winner = groups -> item[iwinner];
		for (k = 1; k <= p; k++)
		{
			(*displacements) -> data[i][k] = displacement[k];
			if (his data[i][iwinner] > minProb)
			{
				double delta_k = winner -> centroid[k] - x[k];
				displacement[k] += alpha * delta_k;
			}
		}
	}

end:

	if (! poolCovarianceMatrices) forget (groups);
	NUMdvector_free (log_apriori, 1);
	NUMdvector_free (log_p, 1);
	NUMpvector_free (sscpvec, 1);
	NUMdvector_free (ln_determinant, 1);
	forget (pool);
	NUMdvector_free (displacement, 1);
	NUMdvector_free (x, 1);
	NUMdvector_free (buf, 1);
	if (Melder_hasError ())
	{
		forget (him);
		forget (*displacements);
	}
	return him;
}

Configuration TableOfReal_to_Configuration_lda (TableOfReal me,
	long numberOfDimensions)
{
	Configuration him = NULL;
	Discriminant thee = TableOfReal_to_Discriminant (me);

	if (thee == NULL) return NULL;

	him = Discriminant_and_TableOfReal_to_Configuration (thee, me,
		numberOfDimensions);

	forget (thee);
	return him;
}

#undef MAX
#undef MIN

/* End of file Discriminant.c */
