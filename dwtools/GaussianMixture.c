/* GaussianMixture.c
 *
 * Copyright (C) 2010 David Weenink
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
  djmw 20101021 Initial version
*/
#include "Distributions_and_Strings.h"
#include "GaussianMixture.h"
#include "NUMlapack.h"
#include "NUMmachar.h"
#include "NUM2.h"
#include "Strings_extensions.h"

#include "oo_DESTROY.h"
#include "GaussianMixture_def.h"
#include "oo_COPY.h"
#include "GaussianMixture_def.h"
#include "oo_EQUAL.h"
#include "GaussianMixture_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_READ_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_READ_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_DESCRIPTION.h"
#include "GaussianMixture_def.h"

extern machar_Table NUMfpp;

wchar_t *GaussianMixture_criterionText (int criterion)
{
	wchar_t *criterionText[6] =  { L"(1/n)*LLH", L"(1/n)*MDL", L"(1/n)*BIC", L"(1/n)*AIC", L"(1/n)*AICc", L"(1/n)*CD_LLH" };
	return criterion >= 0 && criterion < 6 ? criterionText[criterion] : L"(1/n)*ln(p)";
}

void GaussianMixture_removeComponent (GaussianMixture me, long component);
void GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, long component, double **p, long numberOfRows);
int GaussianMixture_and_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, long component, double **p);
int GaussianMixture_and_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double **gamma, double *lnp);
double GaussianMixture_getLikelihoodValue (GaussianMixture me, double **p, long numberOfRows, int onlyLikelyhood);
void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, double **p, long numberOfRows);

static void NUMdvector_scaleAsProbabilities (double *v, long n)
{
	double sum = 0;
	for (long i = 1; i <= n; i++) { sum += v[i]; }
	if (sum > 0) for (long i = 1; i <= n; i++) { v[i] /= sum; }
}

static void GaussianMixture_updateCovariance (GaussianMixture me, long component, double **data, long numberOfRows, double **p)
{
	if (component < 1 || component > my numberOfComponents) return;
	Covariance thee = my covariances -> item[component];

	double mixprob = my mixingProbabilities[component];
	double gsum = p[numberOfRows + 1][component];
	// update the means

	for (long j = 1; j <= thy numberOfColumns; j++)
	{
		thy centroid[j] = 0;
		for (long i = 1; i <= numberOfRows; i++)
		{
			double gamma = mixprob * p[i][component] / p[i][my numberOfComponents + 1];
			thy centroid[j] += gamma * data[i][j] ; // eq. Bishop 9.17
		}
		thy centroid[j] /= gsum;
	}

	// update covariance with the new mean

	if (thy numberOfRows == 1) // 1xn covariance
	{
		for (long j = 1; j <= thy numberOfColumns; j++) thy data[1][j] = 0;
		for (long i = 1; i <= numberOfRows; i++)
		{
			double gamma = mixprob * p[i][component] / p[i][my numberOfComponents + 1];
			double gdn = gamma / gsum;
			for (long j = 1; j <= thy numberOfColumns; j++)
			{
				double xj = thy centroid[j] - data[i][j];
				thy data[1][j] += gdn * xj * xj;
			}
		}
	}
	else  // nxn covariance
	{
		for (long j = 1; j <= thy numberOfRows; j++)
			for (long k = j; k <= thy numberOfColumns; k++)
				thy data[k][j] = thy data[j][k] = 0;
		for (long i = 1; i <= numberOfRows; i++)
		{
			double gamma = mixprob * p[i][component] / p[i][my numberOfComponents + 1];
			double gdn = gamma / gsum; // we cannot divide by nk - 1, this could cause instability
			for (long j = 1; j <= thy numberOfColumns; j++)
			{
				double xj = thy centroid[j] - data[i][j];
				for (long k = j; k <= thy numberOfColumns; k++)
				{
					thy data[j][k] = thy data[k][j] += gdn * xj * (thy centroid[k] - data[i][k]);
				}
			}
		}
	}
	thy numberOfObservations = my mixingProbabilities[component] * numberOfRows;
}

static void GaussianMixture_addCovarianceFraction (GaussianMixture me, long im, Covariance him, double fraction)
{
	if (im < 1 || im > my numberOfComponents || fraction == 0) return;

	Covariance thee = my covariances -> item[im];

	// prevent instability: add lambda fraction of global covariances

	if (thy numberOfRows == 1)
	{
		for (long j = 1; j <= thy numberOfColumns; j++)
		{
			thy data[1][j] += fraction * his data[j][j];
		}
	}
	else
	{
		for (long j = 1; j <= thy numberOfColumns; j++)
		{
			for (long k = j; k <= thy numberOfColumns; k++)
				thy data[k][j] = thy data[j][k] += fraction * his data[j][k];
		}
	}
}

static void classGaussianMixture_info (I)
{
	iam (GaussianMixture);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of components: ", Melder_integer (my numberOfComponents));
	MelderInfo_writeLine2 (L"Dimension of component: ", Melder_integer (my dimension));
	MelderInfo_writeLine1 (L"Mixing probabilities:");
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		MelderInfo_writeLine7 (L"  ", Melder_integer (im), L": p = ", Melder_double (my mixingProbabilities[im]),
			L"  Name =  \"", Thing_getName (my covariances -> item[im]), L"\"");
	}
}

class_methods (GaussianMixture, Data)
{
	class_method_local (GaussianMixture, info)
	class_method_local (GaussianMixture, destroy)
	class_method_local (GaussianMixture, copy)
	class_method_local (GaussianMixture, equal)
	class_method_local (GaussianMixture, canWriteAsEncoding)
	class_method_local (GaussianMixture, writeText)
	class_method_local (GaussianMixture, readText)
	class_method_local (GaussianMixture, writeBinary)
	class_method_local (GaussianMixture, readBinary)
	class_method_local (GaussianMixture, description)
	class_methods_end
}

static void GaussianMixture_setLabelsFromTableOfReal (GaussianMixture me, thou)
{
	thouart (TableOfReal);
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		for (long j = 1; j <= my dimension; j++)
			TableOfReal_setColumnLabel (cov, j, thy columnLabels[j]);
	}
}

// only from big to reduced or same
static int Covariance_into_Covariance (Covariance me, Covariance thee)
{
	if (my numberOfColumns != thy numberOfColumns) return 0;

	SSCP_unExpand (thee); // to its original state

	thy numberOfObservations = my numberOfObservations;
	// copy centroid & column labels
	for (long ic = 1; ic <= my numberOfColumns; ic++) thy centroid[ic] = my centroid[ic];
	if (! NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, thy numberOfColumns)) return 0;
	// Are the matrix sizes equal
	if (my numberOfRows == thy numberOfRows)
	{
		if (! NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, thy numberOfRows)) return 0;
		NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return 1;
	}
	else
	{
		for (long ir = 1; ir <= my numberOfRows; ir++)
		{
			for (long ic = ir; ic <= my numberOfColumns; ic++)
			{
				long dij = ic - ir;
				if (dij < thy numberOfRows) thy data[dij + 1][ic] = my data[ir][ic];
			}
		}
	}
	return 1;
}

static void GaussianMixture_setDefaultMixtureNames (GaussianMixture me)
{
	MelderString s = { 0 };
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		MelderString_append2 (&s, L"m", Melder_integer (im));
		Thing_setName (cov, s.string);
		MelderString_empty (&s);
	}
	MelderString_free (&s);
}

GaussianMixture GaussianMixture_create (long numberOfComponents, long dimension, long storage)
{
	GaussianMixture me = new (GaussianMixture);
	if (me == NULL) return NULL;
	my numberOfComponents = numberOfComponents;
	my dimension = dimension;
	if (((my mixingProbabilities = NUMdvector (1, numberOfComponents)) == NULL) ||
		((my covariances = Ordered_create ()) == NULL)) goto end;
	for (long im = 1; im <= numberOfComponents; im++)
	{
		Covariance cov = Covariance_create_reduceStorage (dimension, storage);
		if (cov == NULL || ! Ordered_addItemPos (my covariances, cov, im)) goto end;
	}
	for (long im = 1; im <= numberOfComponents; im++) my mixingProbabilities[im] = 1.0 / numberOfComponents;
	GaussianMixture_setDefaultMixtureNames (me);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

/* c is double vector 1..dimension !!!!!! */
int GaussianMixture_generateOneVector (GaussianMixture me, double *c, wchar_t *covname, double *buf)
{
	double p = NUMrandomUniform (0, 1);
	long im = NUMgetIndexFromProbability (my mixingProbabilities, my numberOfComponents, p);
	Covariance thee = my covariances -> item[im];
	covname = thy name;
	if (thy numberOfRows == 1) // 1xn reduced form
	{
		for (long i = 1; i <= my dimension; i++)
		{
			c[i] = NUMrandomGauss (thy centroid[i], sqrt (thy data[1][i]));
		}
	}
	else // nxn
	{
		if (thy pca == NULL && ! SSCP_expandPCA (thee)) return 0; // on demand expanding
		Covariance_and_PCA_generateOneVector (thee, thy pca, c, buf);
	}
	return 1;
}

GaussianMixture TableOfReal_to_GaussianMixture_fromRowLabels (I, long storage)
{
	iam (TableOfReal);
	GaussianMixture thee = NULL;
	Strings rowLabels = TableOfReal_extractRowLabels (me);
	Distributions dist = Strings_to_Distributions (rowLabels);
	long numberOfComponents = dist -> numberOfRows;

	if ((thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage)) == NULL) goto end;

	GaussianMixture_setLabelsFromTableOfReal (thee, me);

	for (long i = 1; i <= numberOfComponents; i++)
	{
		Covariance cov = NULL;
		TableOfReal tab = TableOfReal_extractRowsWhereLabel (me, kMelder_string_EQUAL_TO, dist -> rowLabels[i]);
		if (tab == NULL) goto endloop;
		cov = TableOfReal_to_Covariance (tab);
		if (cov == NULL) goto endloop;
		Covariance_into_Covariance (cov, thy covariances -> item[i]);
		Thing_setName (thy covariances -> item[i], dist -> rowLabels[i]);
endloop:
		forget (tab); forget (cov);
		if (Melder_hasError ()) goto end;
	}
	for (long im = 1; im <= numberOfComponents; im++)
	{
		thy mixingProbabilities[im] = dist -> data[im][1] / my numberOfRows;
	}
end:
	forget (dist); forget (rowLabels);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Covariance GaussianMixture_to_Covariance_between (GaussianMixture me)
{
	double nobs_total = 0;

	Covariance thee = Covariance_create (my dimension);
	if (thee == NULL) return NULL;


	//	First the new centroid, based on the mixture centroids

	for (long i = 1; i <= my numberOfComponents; i++)
	{
		Covariance him = my covariances -> item[i];
		double nobs = his numberOfObservations; // the weighting factor
		for (long ic = 1; ic <= my dimension; ic++)
		{
			thy centroid[ic] += nobs * his centroid[ic];
		}
		nobs_total += nobs;
	}
	for (long ic = 1; ic <= my dimension; ic++)
	{
		thy centroid[ic] /= nobs_total;
	}

	Covariance cov = my covariances -> item[1];
	for (long i = 1; i <= thy numberOfColumns; i++)
	{
		if (cov -> columnLabels[i])
		{
			TableOfReal_setColumnLabel (thee, i, cov -> columnLabels[i]);
			TableOfReal_setRowLabel (thee, i, cov -> columnLabels[i]); // if diagonal !
		}
	}

	// Between covariance, scale by the number of observations

	for (long i = 1; i <= my numberOfComponents; i++)
	{
		Covariance him = my covariances -> item[i];
		double nobs = his numberOfObservations - 1; // we loose 1 degree of freedom
		for (long ir = 1; ir <= my dimension; ir++)
		{
			double dir = his centroid[ir] - thy centroid[ir];
			for (long ic = ir; ic <= my dimension; ic++)
			{
				double dic = his centroid[ic] - thy centroid[ic];
				thy data[ir][ic] = thy data[ic][ir] += nobs * dir * dic;
			}
		}
	}

	// Scale back

	for (long ir = 1; ir <= my dimension; ir++)
	{
		for (long ic = ir; ic <= my dimension; ic++)
		{
			thy data[ir][ic] = thy data[ic][ir] /= nobs_total;
		}
	}

	thy numberOfObservations = nobs_total;

	if (Melder_hasError ()) forget (thee);
	return thee;
}

Covariance GaussianMixture_to_Covariance_within (GaussianMixture me)
{
	Covariance thee = Covariance_create (my dimension);
	if (thee == NULL) return NULL;

	for (long i = 1; i <= my numberOfComponents; i++)
	{
		double p = my mixingProbabilities[i];
		Covariance him = my covariances -> item[i];
		if (his numberOfRows == 1)
		{
			for (long ic = 1; ic <= my dimension; ic++)
			thy data[ic][ic] += p * his data[1][ic];
		}
		else
		{
			for (long ir = 1; ir <= my dimension; ir++)
			{
				for (long ic = ir; ic <= my dimension; ic++)
				{
					thy data[ir][ic] = thy data[ic][ir] += p * his data[ir][ic];
				}
			}
		}
		thy numberOfObservations += his numberOfObservations - 1; // we loose a degree of freedom?
	}

	// Leave centroid at 0 so we can add the within and between covariance nicely

	TableOfReal_copyLabels (my covariances -> item[1], thee, 1, 1);

	if (Melder_hasError ()) forget (thee);
	return thee;
}

Covariance GaussianMixture_to_Covariance_total (GaussianMixture me)
{
	Covariance thee = GaussianMixture_to_Covariance_between (me);
	if (thee == NULL) return NULL;
	Covariance within = GaussianMixture_to_Covariance_within (me);
	if (within == NULL) goto end;

	for (long ir = 1; ir <= my dimension; ir++)
	{
		for (long ic = ir; ic <= my dimension; ic++)
		{
			thy data[ir][ic] = thy data[ic][ir] += within -> data[ir][ic];
		}
	}

end:
	forget (within);
	return thee;
}

Covariance GaussianMixture_extractComponent(GaussianMixture me, long component)
{
	if (component < 1 || component > my numberOfComponents) return Melder_errorp1 (L"Illegal component.");
	return Data_copy (my covariances -> item[component]);
}

TableOfReal GaussianMixture_extractMixingProbabilities (GaussianMixture me)
{
	TableOfReal thee = TableOfReal_create (my numberOfComponents, 2);
	if (thee == NULL) return thee;
	TableOfReal_setColumnLabel (thee, 1, L"p");
	TableOfReal_setColumnLabel (thee, 2, L"n");
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		thy data[im][1] = my mixingProbabilities[im];
		thy data[im][2] = cov -> numberOfObservations;
		TableOfReal_setRowLabel (thee, im, Thing_getName (cov));
	}
	return thee;
}

TableOfReal GaussianMixture_extractCentroids (GaussianMixture me)
{
	TableOfReal thee = TableOfReal_create (my numberOfComponents, my dimension);
	if (thee == NULL) return thee;

	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		if (im == 1)
		{
			for (long j = 1; j <= my dimension; j++) TableOfReal_setColumnLabel (thee, j, cov -> columnLabels[j]);
		}
		TableOfReal_setRowLabel (thee, im, Thing_getName (cov));
		for (long j = 1; j <= my dimension; j++)
			thy data[im][j] = cov -> centroid[j];
	}

	return thee;
}

PCA GaussianMixture_to_PCA (GaussianMixture me)
{
	Covariance him = GaussianMixture_to_Covariance_total (me);
	if (him == NULL) return NULL;
	PCA thee = SSCP_to_PCA (him);
	forget (him);
	return thee;
}

int GaussianMixture_getIntervalsAlongDirections (GaussianMixture me, long d1, long d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax)
{
	*xmin = *xmax = *ymin = *ymax = NUMundefined;
	if (d1 < 1 || d1 > my dimension || d2 < 1 || d2 > my dimension) return 0;
	SSCPs sscps = SSCPs_extractTwoDimensions ((SSCPs) my covariances, d1, d2);
	if (sscps == NULL) return 0;
	SSCPs_getEllipsesBoundingBoxCoordinates (sscps, -nsigmas, 0, xmin, xmax, ymin, ymax);
	forget (sscps);
	return 1;
}

int GaussianMixture_getIntervalAlongDirection (GaussianMixture me, long d, double nsigmas, double *xmin, double *xmax)
{
	double ymin, ymax;
	return GaussianMixture_getIntervalsAlongDirections (me, d, d, nsigmas, xmin, xmax, &ymin, &ymax);
}

int GaussianMixture_and_PCA_getIntervalsAlongDirections (GaussianMixture me, PCA thee, long d1, long d2, double nsigmas, double *xmin, double *xmax, double *ymin, double *ymax)
{
	*xmin = *xmax = *ymin = *ymax = NUMundefined;
	if (my dimension != thy dimension || d1 < 1 || d1 > my dimension || d2 < 1 || d2 > my dimension) return 0;
	SSCPs sscps = SSCPs_toTwoDimensions ((SSCPs) my covariances, thy eigenvectors[d1], thy eigenvectors[d2]);
	if (sscps == NULL) return 0;
	SSCPs_getEllipsesBoundingBoxCoordinates (sscps, -nsigmas, 0, xmin, xmax, ymin, ymax);
	forget (sscps);
	return 1;
}


int GaussianMixture_and_PCA_getIntervalAlongDirection (GaussianMixture me, PCA thee, long d, double nsigmas, double *xmin, double *xmax)
{
	double ymin, ymax;
	return GaussianMixture_and_PCA_getIntervalsAlongDirections (me, thee, d, d, nsigmas, xmin, xmax, &ymin, &ymax);
}

void GaussianMixture_and_PCA_drawMarginalPdf (GaussianMixture me, PCA thee, Graphics g, long d, double xmin, double xmax, double ymin, double ymax, long npoints, long nbins, int garnish)
{
	if (my dimension != thy dimension || d < 1 || d > my dimension)
	{ Melder_warning1 (L"Dimensions don't agree."); return;}

	if (npoints <= 1) npoints = 1000;
	double *p = NUMdvector (1, npoints);
	if (p == 0) return;
	double nsigmas = 2;

	if (xmax <= xmin &&
		! GaussianMixture_and_PCA_getIntervalAlongDirection (me, thee, d, nsigmas, &xmin, &xmax)) goto end;

	double pmax = 0, dx = (xmax - xmin) / npoints, x1 = xmin + 0.5 * dx;
	double scalef = nbins <= 0 ? 1 : 1; // TODO
	for (long i = 1; i <= npoints; i++)
	{
		double x = x1 + (i - 1) * dx;
		p[i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, thy eigenvectors[d], x);
		if (p[i] > pmax) pmax = p[i];
	}
	if (ymin >= ymax) { ymin = 0; ymax = pmax; }

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p, 1, npoints, x1, xmax - 0.5 * dx);
	Graphics_unsetInner (g);

	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, 1, 1, 0, NULL);
		Graphics_markBottom (g, xmax, 1, 1, 0, NULL);
		Graphics_markLeft (g, ymin, 1, 1, 0, NULL);
		Graphics_markLeft (g, ymax, 1, 1, 0, NULL);
	}

end:
	NUMdvector_free (p, 1);
}

void GaussianMixture_drawMarginalPdf (GaussianMixture me, Graphics g, long d, double xmin, double xmax, double ymin, double ymax, long npoints, long nbins, int garnish)
{
	if (d < 1 || d > my dimension)
	{ Melder_warning1 (L"Dimension doesn't agree."); return;}
	if (npoints <= 1) npoints = 1000;
	double *p = NUMdvector (1, npoints);
	if (p == 0) return;
	double nsigmas = 2, *v = NUMdvector (1, my dimension);
	if (v == NULL) goto end;

	if (xmax <= xmin &&
		! GaussianMixture_getIntervalAlongDirection (me, d, nsigmas, &xmin, &xmax)) goto end;

	double pmax = 0, dx = (xmax - xmin) / (npoints - 1);
	double scalef = nbins <= 0 ? 1 : 1; // TODO
	for (long i = 1; i <= npoints; i++)
	{
		double x = xmin + (i - 1) * dx;
		for (long k = 1; k <= my dimension; k++) v[k] = k == d ? 1 : 0;
		p[i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, v, x);
		if (p[i] > pmax) pmax = p[i];
	}
	if (ymin >= ymax) { ymin = 0; ymax = pmax; }

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p, 1, npoints, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, 1, 1, 0, NULL);
		Graphics_markBottom (g, xmax, 1, 1, 0, NULL);
		Graphics_markLeft (g, ymin, 1, 1, 0, NULL);
		Graphics_markLeft (g, ymax, 1, 1, 0, NULL);
	}

end:
	NUMdvector_free (p, 1); NUMdvector_free (v, 1);
}

void GaussianMixture_and_PCA_drawConcentrationEllipses (GaussianMixture me, PCA him, Graphics g, double scale,
	int confidence, wchar_t *label, long d1, long d2,
	double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish)
{
	if (my dimension != his dimension) { Melder_warning1 (L"Dimensions don't agree."); return;}
	SSCPs thee = NULL;
	int d1_inverted = 0, d2_inverted = 0;

	if (d1 < 0) { d1 = abs(d1); Eigen_invertEigenvector (him, d1); d1_inverted = 1; }
	if (d2 < 0) { d2 = abs(d2); Eigen_invertEigenvector (him, d2); d2_inverted = 1; }

	if ((thee = SSCPs_toTwoDimensions ((SSCPs) my covariances, his eigenvectors[d1], his eigenvectors[d2])) == NULL) goto end;

	if (d1_inverted) Eigen_invertEigenvector (him, d1);
	if (d2_inverted) Eigen_invertEigenvector (him, d2);

	SSCPs_drawConcentrationEllipses (thee, g, -scale, confidence, label, 1, 2,
		xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish)
	{
		wchar_t label[40];
    	Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	swprintf (label, 40, L"pc %ld", d2);
    	Graphics_textLeft (g, 1, label);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
    	swprintf (label, 40, L"pc %ld", d1);
		Graphics_textBottom (g, 1, label);
	}
end:
	forget (thee);
}

void GaussianMixture_drawConcentrationEllipses (GaussianMixture me, Graphics g,
	double scale, int confidence, wchar_t *label, int pcaDirections, long d1, long d2,
	double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish)
{
	if (d1 == 0 && d2 == 0) { d1 = 1; d2 = 2; }
	if (abs(d1) > my dimension || abs(d2) > my dimension) return;

	if (! pcaDirections)
	{
		SSCPs_drawConcentrationEllipses ((SSCPs) my covariances, g, -scale, confidence, label,
			abs(d1), abs(d2), xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	PCA him = GaussianMixture_to_PCA (me);
	if (him == NULL) return;

	GaussianMixture_and_PCA_drawConcentrationEllipses (me, him, g, scale, confidence, label, d1, d2,
	xmin, xmax, ymin, ymax, fontSize, garnish);

	forget (him);
}

int GaussianMixture_initialGuess (GaussianMixture me, TableOfReal thee, double nSigmas, double ru_range)
{

	Covariance cov_t = TableOfReal_to_Covariance (thee);
	if (cov_t == NULL) return 0;

	// assume equal probabilities for mixture
	// assume equal covariance matrices
	// spread centroids on an ellips in pc1-pc2 plane?

	if (my dimension == 1)
	{
		double dm = 2 * sqrt (cov_t -> data[1][1]) / my numberOfComponents;
		double m1 = cov_t -> centroid[1] - dm;
		for (long im = 1; im <= my numberOfComponents; im++)
		{
			Covariance covi = my covariances -> item[im];
			covi -> centroid[1] = m1;
			covi -> data[1][1] = dm * dm;
			m1 += dm;
			covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
		}
	}
	else
	{
		double a, b, cs, sn;
		PCA pca = NULL; SSCP s2d = NULL; Configuration means2d = NULL; TableOfReal means = NULL;

		if (((pca = SSCP_to_PCA (cov_t)) == NULL) ||
			((s2d = SSCP_toTwoDimensions (cov_t, pca -> eigenvectors[1], pca -> eigenvectors[2])) == NULL) ||
			((means2d = Configuration_create (my numberOfComponents, 2)) == NULL)) goto end;

		NUMeigencmp22 (s2d -> data[1][1], s2d -> data[1][2], s2d -> data[2][2], &a, &b, &cs, &sn);

		a = nSigmas * sqrt(a); b = nSigmas * sqrt (b);
		double angle = 0, angle_inc = NUM2pi / my numberOfComponents;
		for (long im = 1; im <= my numberOfComponents; im++, angle += angle_inc)
		{
			double xc = a * (1 + NUMrandomUniform (-ru_range, ru_range)) * cos (angle);
			double yc = b * (1 + NUMrandomUniform (-ru_range, ru_range)) * sin (angle);
			means2d -> data[im][1] = s2d -> centroid[1] + xc * cs - yc * sn;
			means2d -> data[im][2] = s2d -> centroid[2] + xc * sn + yc * cs;
		}

		// reconstruct the n-dimensional means from the 2-d from the eigenvectors

		means = PCA_and_Configuration_to_TableOfReal_reconstruct (pca, means2d);

		for (long im = 1; im <= my numberOfComponents; im++)
		{
			Covariance covi = my covariances -> item[im];
			for (long ic = 1; ic <= my dimension; ic++) { covi -> centroid[ic] = means -> data[im][ic]; }
			covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
		}

		// Trick: use the new means to get the between SSCP,
		// if there is only one component the cov_b will be zero!

		Covariance cov_b = GaussianMixture_to_Covariance_between (me);
		double var_t = SSCP_getTotalVariance (cov_t);
		double var_b = SSCP_getTotalVariance (cov_b);
		if (var_b >= var_t) // we have chosen our initial values too far out
		{
			double scale = 0.9 * sqrt (var_t / var_b);
			for (long im = 1; im <= my numberOfComponents; im++)
			{
				Covariance covi = my covariances -> item[im];
				for (long ic = 1; ic <= my dimension; ic++)
				{
					covi -> centroid[ic] -= (1 - scale) * (covi -> centroid[ic] - cov_b -> centroid[ic]);
				}
			}
			forget (cov_b);
			cov_b = GaussianMixture_to_Covariance_between (me);
		}

		// Within variances are now (total - between) / numberOfComponents;

		for (long ir = 1; ir <= my dimension; ir++)
		{
			for (long ic = ir; ic <= my dimension; ic++)
			{
				double scalef = my numberOfComponents == 1 ? 1 : (var_b / var_t ) / my numberOfComponents;
				cov_t -> data[ic][ir] = cov_t -> data[ir][ic] *= scalef;
			}
		}

		// Copy them

		for (long im = 1; im <= my numberOfComponents; im++)
		{
			Covariance cov = my covariances -> item[im];
			if (cov -> numberOfRows == 1)
			{
				for (long ic = 1; ic <= my dimension; ic++) cov -> data[1][ic] = cov_t -> data[ic][ic];
			}
			else
			{
				for (long ir = 1; ir <= my dimension; ir++)
				{
					for (long ic = ir; ic <= my dimension; ic++)
					{
						cov -> data[ir][ic] = cov -> data[ic][ir] = cov_t -> data[ir][ic];
					}
				}
			}
		}

end:
		forget (cov_b); forget (means); forget (means2d); forget (s2d); forget (pca); forget (cov_t);
	}
	return ! Melder_hasError ();
}


ClassificationTable GaussianMixture_and_TableOfReal_to_ClassificationTable (GaussianMixture me, TableOfReal thee)
{
	double ln2pid = - 0.5 * my dimension * log (NUM2pi), *lnN = NULL;
	ClassificationTable him = ClassificationTable_create (thy numberOfRows, my numberOfComponents);
	if (him == NULL) return NULL;

	if ((lnN = NUMdvector (1, my numberOfComponents)) == NULL) goto end;
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		if (! SSCP_expandLowerCholesky (cov)) goto end;
		TableOfReal_setColumnLabel (him, im, Thing_getName (cov));
	}

	for (long i = 1; i <=  thy numberOfRows; i++)
	{
		double psum = 0;
		for (long im = 1; im <= my numberOfComponents; im++)
		{
			Covariance cov = my covariances -> item[im];
			double dsq = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data[i], cov -> centroid, cov -> numberOfRows, my dimension);
			lnN[im] = ln2pid - 0.5 * (cov -> lnd + dsq);
			psum += his data[i][im] = my mixingProbabilities[im] * exp (lnN[im]);
		}
		if (psum == 0) // p's might be too small (underflow), make the largest equal to sfmin
		{
			double lnmax = -INFINITY; long imm = 1;
			for (long im = 1; im <= my numberOfComponents; im++) { if (lnN[im] > lnmax) lnmax = lnN[im]; imm = im; }
			his data[i][imm] = NUMfpp -> sfmin;
		}

		// for (long im = 1; im <= my numberOfComponents; im++) { his data[i][im] /= psum; }

		TableOfReal_setRowLabel (him, i, thy rowLabels[i]);
	}

end:
	NUMdvector_free (lnN, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

int GaussianMixture_and_TableOfReal_getGammas (GaussianMixture me, TableOfReal thee, double **gamma, double *lnp)
{
	double ln2pid = - 0.5 * my dimension * log (NUM2pi), *lnN = NULL;
	double *nk = gamma[thy numberOfRows + 1];

	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		if (! SSCP_expandLowerCholesky (cov)) goto end;
	}

	if ((lnN = NUMdvector (1, my numberOfComponents)) == NULL) return 0;

	for (long im = 1; im <= my numberOfComponents; im++) nk[im] = 0;

	*lnp = 0;
	for (long i = 1; i <=  thy numberOfRows; i++)
	{
		double rowsum = 0;
		for (long im = 1; im <= my numberOfComponents; im++)
		{
			Covariance cov = my covariances -> item[im];
			double dsq = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data[i], cov -> centroid, cov -> numberOfRows, my dimension);
			lnN[im] = ln2pid - 0.5 * (cov -> lnd + dsq);
			gamma[i][im] = my mixingProbabilities[im] * exp (lnN[im]); // eq. Bishop 9.16
			rowsum += gamma[i][im];
		}

		// If the gamma[i]'s are too small, their sum will be zero and the scaling will overflow

		if (rowsum == 0) continue; // This is ok because gamma[i]'s will all be zero

		// scale gamma and get log(likehood) (Bishop eq. 9.40)

		for (long im = 1; im <= my numberOfComponents; im++)
		{
			gamma[i][im] /= rowsum; // eq. Bishop 9.16
			nk[im] += gamma[i][im]; // eq. Bishop 9.18
			*lnp += gamma[i][im] * (log (my mixingProbabilities[im])  + lnN[im]); // eq. Bishop 9.40
		}
	}
end:
	NUMdvector_free (lnN, 1);

	return 1;
}

double GaussianMixture_getHarmony (GaussianMixture me, long component, double **p, long numberOfRows)
{
	double icb = 1, ice = my numberOfComponents, harmony = 0;
	if (component > 0 && component <= my numberOfComponents) { icb = ice = component; }
	for (long i = 1; i <= numberOfRows; i++)
	{
		double rowsum = 0;
		for (long ic = 1; ic <= my numberOfComponents; ic ++)
		{
			rowsum += my mixingProbabilities[ic] * p[i][ic];
		}
		for (long ic = icb; ic <= ice; ic++)
		{
			double pa = my mixingProbabilities[ic] * p[i][ic];
			if (rowsum > 0) harmony += pa * log (pa) / rowsum;
		}
	}
	return harmony / numberOfRows;
}

int GaussianMixture_splitComponent (GaussianMixture me, long component)
{
	if (component < 1 || component > my numberOfComponents) return 0;
	Covariance thee = my covariances -> item[component], cov1 = NULL, cov2 = NULL;

	double *mixingProbabilities = NUMdvector (1, my numberOfComponents + 1);
	if (mixingProbabilities == NULL) return 0;

	if (! SSCP_expandPCA (thee)) return 0; // Always new PCA because we cannot be sure of data unchanged.

	if (((cov1 = Data_copy (thee)) == NULL) || ((cov2 = Data_copy (thee)) == NULL)) goto end;
	SSCP_unExpandPCA (cov1); SSCP_unExpandPCA (cov2);

	// Eventually cov1 replaces component, cov2 at end

	for (long i = 1; i <= my numberOfComponents; i++) { mixingProbabilities[i] = my mixingProbabilities[i]; }
	double gamma = 0.5, lambda = 0.5, eta = 0.5, mu = 0.5;
	mixingProbabilities[component] = gamma * my mixingProbabilities[component];
	mixingProbabilities[my numberOfComponents + 1] = (1 - gamma) * my mixingProbabilities[component];
	double mp12 =  mixingProbabilities[component] / mixingProbabilities[my numberOfComponents + 1];
	double factor1 = (eta - eta * lambda * lambda - 1) / gamma + 1;
	double factor2 = (eta * lambda * lambda - eta - lambda * lambda) / (1 - gamma) + 1;
	double *ev = thy pca -> eigenvectors[1];
	double d2 = thy pca -> eigenvalues[1];

	for (long i = 1; i <= my dimension; i++)
	{
		cov1 -> centroid[i] -= (1 / sqrt(mp12)) * sqrt (d2) * mu * ev[i];
		cov2 -> centroid[i] +=       sqrt(mp12) * sqrt (d2) * mu * ev[i];

		if (thy numberOfRows == 1) // diagonal
		{
			cov1 -> data[1][i] = cov1 -> data [1][i] / mp12 + factor1 * d2;
			cov1 -> data[1][i] = cov2 -> data [i][i] * mp12 + factor2 * d2;
		}
		else
		{
			for (long j = i; j <= my dimension; j++)
			{
				cov1 -> data[j][i] = cov1 -> data[i][j] = cov1 -> data [i][j] / mp12 + factor1 * d2 * ev[i] * ev[j];
				cov2 -> data[j][i] = cov2 -> data[i][j] = cov2 -> data [i][j] * mp12 + factor2 * d2 * ev[i] * ev[j];
			}
		}
	}
	cov1 -> numberOfObservations *= gamma; cov2 -> numberOfObservations *= 1 - gamma;

	// Replace cov1 at component + add cov2. If something goes wrong we must be able to restore original!

	my covariances -> item[component] = cov1;
	Thing_setName (cov2, Melder_wcscat3 (Thing_getName (cov2), L"-", Melder_integer (my numberOfComponents+1)));
	if (! Collection_addItem (my covariances, cov2)) goto end;
	my numberOfComponents++;
	forget (thee); // Now we can savely remove
	NUMdvector_free (my mixingProbabilities, 1);
	my mixingProbabilities = mixingProbabilities;
end:
	if (Melder_hasError ())
	{
		forget (cov1); forget (cov2);
		NUMdvector_free (mixingProbabilities, 1);
		my covariances -> item[component] = thee;
		return 0;
	}
	return 1;
}

int GaussianMixture_and_TableOfReal_getProbabilities (GaussianMixture me, TableOfReal thee, long component, double **p)
{
	double ln2pid = my dimension * log (NUM2pi);

	// Update only one component or all?

	long icb = 1, ice = my numberOfComponents;
	if (component > 0 && component <= my numberOfComponents) { icb = ice = component; }

	for (long ic = icb; ic <= ice; ic++)
	{
		Covariance him = my covariances -> item[ic];
		if (! SSCP_expandLowerCholesky (him)) return 0;

		for (long i = 1; i <= thy numberOfRows; i++)
		{
			double dsq = NUMmahalanobisDistance_chi (his lowerCholesky, thy data[i], his centroid, his numberOfRows, my dimension);
			p[i][ic] = exp (- 0.5 * (ln2pid + his lnd + dsq));
		}
	}

	GaussianMixture_updateProbabilityMarginals (me, p, thy numberOfRows);

	return 1;
}

int GaussianMixture_expandPCA (GaussianMixture me)
{
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance him = my covariances -> item[im];
		if (his numberOfRows == 1) return 0;
		if ((his pca = SSCP_to_PCA (him)) == NULL) return 0;
	}
	return 1;
}

void GaussianMixture_unExpandPCA (GaussianMixture me)
{
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		SSCP_unExpandPCA (my covariances -> item[im]);
	}
}

int GaussianMixture_and_TableOfReal_improveLikelihood (GaussianMixture me, thou, double delta_lnp,
	long maxNumberOfIterations, double lambda, int criterion)
{
	thouart (TableOfReal);
	wchar_t *criterionText = GaussianMixture_criterionText (criterion);
	long iter = 0;
	double lnp = 0, lnp_prev, **p = NULL;

	/*
		The global covariance matrix is added with scaling coefficient lambda during updating the
		mixture covariances to prevent numerical instabilities.
	*/

	Covariance covg = TableOfReal_to_Covariance (thee);
	if (covg == NULL) goto end;

	if ((p = NUMdmatrix (1, thy numberOfRows + 1, 1, my numberOfComponents + 1)) == NULL) goto end;
	double *nk = p[thy numberOfRows + 1]; // last row has the column marginals n(k)
	if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p))
	{
		MelderInfo_writeLine1 (L"Iteration stopped, may be too much components?");
		goto end;
	}
	lnp = GaussianMixture_getLikelihoodValue (me, p, thy numberOfRows, criterion);
	MelderInfo_writeLine6 (L"\nIteration ", Melder_integer (iter), L":  ", Melder_double (lnp / thy numberOfRows), L" (=  ", criterionText);
	MelderInfo_open ();
	do {

		// E-step: get responsabilities (gamma) with current parameters
		// See C. Bishop (2006), Pattern reconition and machine learning, Springer, page 439...

		lnp_prev = lnp; iter++;
		// M-step: 1. new means & covariances

		for (long im = 1; im <= my numberOfComponents; im++)
		{
			GaussianMixture_updateCovariance (me, im, thy data, thy numberOfRows, p);
			GaussianMixture_addCovarianceFraction (me, im, covg, lambda);
		}

		// M-step: 2. new mixingProbabilities

		for (long im = 1; im <= my numberOfComponents; im++)
		{
			my mixingProbabilities[im] = nk[im] / thy numberOfRows;
		}
		if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p)) break;
		lnp = GaussianMixture_getLikelihoodValue (me, p, thy numberOfRows, criterion);
		MelderInfo_writeLine6 (L"\nIteration ", Melder_integer (iter), L":  ", Melder_double (lnp / thy numberOfRows),
			L" (=  ", criterionText);
	} while (fabs((lnp - lnp_prev)/ lnp_prev) > delta_lnp && iter < maxNumberOfIterations);

	// During EM, covariances were underestimated by a factor of (n-1)/n. Correction now.

	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		if (cov -> numberOfObservations > 1.5)
		{
			if (cov -> numberOfRows == 1)
			{
				for (long j = 1; j <= thy numberOfColumns; j++)
					cov -> data[1][j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1);
			}
			else
			{
				for (long j = 1; j <= thy numberOfColumns; j++)
					for (long k = j; k <= thy numberOfColumns; k++)
						cov -> data[j][k] = cov -> data[k][j] *= cov -> numberOfObservations / (cov -> numberOfObservations - 1);
			}
		}
	}
end:

	MelderInfo_close ();
	NUMdmatrix_free (p, 1, 1); forget (covg);

	return ! Melder_hasError ();
}

long GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me)
{
	Covariance thee = my covariances -> item[1];
	// if diagonal) d (means) + d (stdev)
	// else  n + n(n+1)/2
	return thy numberOfRows == 1 ? 2 * thy numberOfColumns : thy numberOfColumns * (thy numberOfColumns + 3) / 2;
}

void GaussianMixture_updateProbabilityMarginals (GaussianMixture me, double **p, long numberOfRows)
{
	long nocp1 = my numberOfComponents + 1, norp1 = numberOfRows + 1;

	for (long ic = 1; ic <= my numberOfComponents; ic++) { p[norp1][ic] = 0; }
	for (long i = 1; i <= numberOfRows; i++)
	{
		double rowsum = 0;
		for (long ic = 1; ic <= my numberOfComponents; ic++)
		{
			rowsum += my mixingProbabilities[ic] * p[i][ic];
		}
		p[i][nocp1] = rowsum;
		for (long ic = 1; ic <= my numberOfComponents; ic++)
		{
			p[norp1][ic] += my mixingProbabilities[ic] * p[i][ic] / p[i][nocp1];
		}
	}
}

void GaussianMixture_removeComponent_bookkeeping (GaussianMixture me, long component, double **p, long numberOfRows)
{
	// p is (NumberOfRows+1) by (numberOfComponents+1) !

	for (long i = 1; i <= numberOfRows + 1; i++)
	{
		for (long ic = component; ic <= my numberOfComponents; ic++)
		{
			p[i][ic] = p[i][ic + 1];
		}
	}
	GaussianMixture_updateProbabilityMarginals (me, p, numberOfRows);
	GaussianMixture_removeComponent (me, component);
}

double GaussianMixture_and_TableOfReal_getLikelihoodValue (GaussianMixture me, thou, int criterion)
{
	thouart (TableOfReal);
	double value = NUMundefined;

	double **p = NUMdmatrix (1, thy numberOfRows + 1, 1, my numberOfComponents + 1);
	if (p != NULL && GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p))
	{
		value = GaussianMixture_getLikelihoodValue (me, p, thy numberOfRows, criterion);
	}

	NUMdmatrix_free (p, 1, 1);
	return value;
}

double GaussianMixture_getLikelihoodValue (GaussianMixture me, double **p, long numberOfRows, int criterion)
{
	/*
		Because we try to _maximize_ a criterion, all criteria are negative numbers.
	*/

	if (criterion == GaussianMixture_CD_LIKELIHOOD)
	{
		double lnpcd = 0;
		for (long i = 1; i <= numberOfRows; i++)
		{
			double psum = 0, lnsum = 0;
			for (long ic = 1; ic <= my numberOfComponents; ic++)
			{
				double pp = my mixingProbabilities[ic] * p[i][ic];
				psum += pp;
				lnsum += pp * log (pp);
			}
			if (psum > 0) lnpcd += lnsum / psum;
		}
		return lnpcd;
	}

	// The common factor for all other criteria is the log(likelihood)

	double lnp = 0;
	for (long i = 1; i <= numberOfRows; i++)
	{
		double psum = 0;
		for (long ic = 1; ic <= my numberOfComponents; ic++)
		{
			psum += my mixingProbabilities[ic] * p[i][ic];
		}
		if (psum > 0) lnp += log (psum);
	}

	if (criterion == GaussianMixture_LIKELIHOOD)
	{
		return lnp;
	}

	double npars = GaussianMixture_getNumberOfParametersInComponent (me), np = npars * my numberOfComponents;
	if (criterion == GaussianMixture_MDL)
	{
		/* Equation (15) in
			Mario A.T. Figueiredo, and Anil K. Jain, Unsupervised Learning of Finite Mixture Models :
			IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 24, NO. 3, MARCH 2002

			L(theta,Y)= N/2*sum(m=1..k, log(n*alpha[k]/12)) +k/2*ln(n/12) +k(N+1)/2
				- log (sum(i=1..n, sum(m=1..k, alpha[k]*p(k))))
		*/
		double logmpn = 0;
		for (long ic = 1; ic <= my numberOfComponents; ic++)
		{
			logmpn += log (my mixingProbabilities[ic]);
		}

		// a rewritten L(theta,Y) is

		return lnp - 0.5 * my numberOfComponents * (npars + 1) * (log (numberOfRows / 12.0) + 1)
			+ 0.5 * npars * logmpn;
	}
	else if (criterion == GaussianMixture_BIC)
	{
		return 2 * lnp - np * log (numberOfRows);
	}
	else if (criterion == GaussianMixture_AIC)
	{
		return 2 * (lnp - np);
	}
	else if (criterion == GaussianMixture_AICC)
	{
		double np = npars * my numberOfComponents;
		return 2 * (lnp - np * (numberOfRows / (numberOfRows - np - 1)));
	}
	return lnp;
}

GaussianMixture GaussianMixture_and_TableOfReal_to_GaussianMixture_CEMM (GaussianMixture gm, thou, long minNumberOfComponents, double delta_l, long maxNumberOfIterations, double lambda, int criterion)
{
	thouart (TableOfReal);
	wchar_t *criterionText = GaussianMixture_criterionText(criterion);
	int deleteWeakComponents = minNumberOfComponents > 0;
	double lmax = -INFINITY, lprev;
	GaussianMixture best = NULL, me = Data_copy (gm);
	if (me == NULL) return NULL;

	double **p = NUMdmatrix (1, thy numberOfRows + 2, 1, my numberOfComponents + 1);
	if (p == NULL) return NULL;

	double *gsum = p[thy numberOfRows + 1]; // convenience array with sums

	Covariance covg = TableOfReal_to_Covariance (thee);
	if (covg == NULL) goto end;

	long iter = 0, component;

	double npars = GaussianMixture_getNumberOfParametersInComponent (me);
	double nparsd2 = deleteWeakComponents ? npars / 2 : 0;

	// Initial E-step: Update all p's.

	if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p)) goto end;

	double lnew = GaussianMixture_getLikelihoodValue (me, p, thy numberOfRows, criterion);

	MelderInfo_open ();
	MelderInfo_writeLine6 (L"iter: ", Melder_integer (iter), criterionText,
		Melder_double (lnew / thy numberOfRows), L", Components: ", Melder_integer (my numberOfComponents));

	while (my numberOfComponents >= minNumberOfComponents)
	{
		do
		{
			iter++; component = 1; lprev = lnew;
			while (component <= my numberOfComponents)
			{
				// M-step for means and covariances
				GaussianMixture_updateProbabilityMarginals (me, p, thy numberOfRows);
				GaussianMixture_updateCovariance (me, component, thy data, thy numberOfRows, p);
				if (lambda > 0) GaussianMixture_addCovarianceFraction (me, component, covg, lambda);

				// Now check if enough support for a component exists

				double support_im = gsum[component] - nparsd2, support = 0;
				for (long ic = 1; ic <= my numberOfComponents; ic++)
				{
					double support_ic = gsum[ic] - nparsd2;
					if (support_ic > 0) support += support_ic;
				}
				my mixingProbabilities[component] = support_im > 0 ? support_im : 0;
				if (support > 0) my mixingProbabilities[component] /= support;

				NUMdvector_scaleAsProbabilities (my mixingProbabilities, my numberOfComponents);

				if (my mixingProbabilities[component] > 0)  // update p for component
				{
					if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, component, p)) goto end;
					component++;
				}
				else
				{
					// "Remove" the component column from p by shifting row values

					GaussianMixture_removeComponent_bookkeeping (me, component, p, thy numberOfRows);

					// Now numberOfComponents is one less!

					MelderInfo_writeLine2 (L"Removed component ", Melder_integer (component));
				}
			}
			/*
				L(theta,Y)=N/2 sum(m=1..k, log(n*mixingP[m]/12))+k/2log(n/12)+k/2(N+1)-loglikelihood
				reduces to:
				k/2 (N+1){log(n/12)+1}+N/2sum(m=1..k,mixingP[m]) - loglikelihood
			*/

			lnew = GaussianMixture_getLikelihoodValue (me, p, thy numberOfRows, criterion);

			MelderInfo_writeLine8 (L"iter: ", Melder_integer (iter), L", ", criterionText, L"= ",
				Melder_double (lnew / thy numberOfRows), L", Components: ", Melder_integer (my numberOfComponents));

		} while (lnew > lprev && fabs((lprev - lnew)/ lnew) > delta_l && iter < maxNumberOfIterations);
		if (lnew > lmax)
		{
			if (best != NULL) forget (best);
			best = Data_copy (me);
			lmax = lnew;
			if (! deleteWeakComponents) goto end;
		}
		if (my numberOfComponents > 1) // remove smallest component
		{
			component = 1;
			double mpmin = my mixingProbabilities[component];
			for (long ic = 2; ic <= my numberOfComponents; ic++)
			{
				if (my mixingProbabilities[ic] < mpmin) { mpmin = my mixingProbabilities[ic]; component = ic; }
			}

			GaussianMixture_removeComponent_bookkeeping (me, component, p, thy numberOfRows);
		}
		else break;
	}
end:
	MelderInfo_close ();
	NUMdmatrix_free (p, 1, 1); forget (covg); forget (me);
	if (Melder_hasError ()) forget (best);
	return best;
}

// The numberOfElemnts per covariance needs to be updated later
void GaussianMixture_removeComponent (GaussianMixture me, long component)
{
	if (component < 1 || component > my numberOfComponents || my numberOfComponents == 1) return;

	Collection_removeItem (my covariances, component);
	my numberOfComponents --;

	for (long ic = component; ic <= my numberOfComponents; ic++)
	{
		my mixingProbabilities[ic] = my mixingProbabilities[ic + 1];
	}

	NUMdvector_scaleAsProbabilities (my mixingProbabilities, my numberOfComponents);
}

GaussianMixture TableOfReal_to_GaussianMixture (I, long numberOfComponents, double delta_lnp, long maxNumberOfIterations, double lambda, int storage, int criterion)
{
	iam (TableOfReal);

	if (my numberOfRows < 2 * numberOfComponents) return Melder_errorp1 (L"The number of data points must at least be twice the number of components.");

	GaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);
	if (thee == NULL) return NULL;

	GaussianMixture_setLabelsFromTableOfReal (thee, me);

	GaussianMixture_initialGuess (thee, me, 1, 0.05);
	if (maxNumberOfIterations <= 0) return thee;

	if (! GaussianMixture_and_TableOfReal_improveLikelihood (thee, me, delta_lnp, maxNumberOfIterations, lambda, criterion)) goto end;

end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Correlation GaussianMixture_and_TableOfReal_to_Correlation (GaussianMixture me, thou)
{
	thouart (TableOfReal);
	if (my dimension != thy numberOfColumns) return NULL;
	ClassificationTable ct = GaussianMixture_and_TableOfReal_to_ClassificationTable (me, thee);
	if (ct == NULL) return NULL;
	Correlation him = ClassificationTable_to_Correlation_columns (ct);
	forget (ct);
	return him;
}

double GaussianMixture_getProbabilityAtPosition_string (GaussianMixture me, wchar_t *vector)
{
	long i = 0;
	double *v = NUMdvector (1, my dimension);
	if (v == NULL) return NUMundefined;

	for (wchar_t *token = Melder_firstToken (vector); token != NULL; token = Melder_nextToken ())
	{
		v[++i] = Melder_atof (token);
		if (i == my dimension) break;
	}
	double p = GaussianMixture_getProbabilityAtPosition (me, v);
	NUMdvector_free (v, 1);
	return p;
}

double GaussianMixture_getMarginalProbabilityAtPosition (GaussianMixture me, double *vector, double x)
{
	double p = 0;
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		double pim = Covariance_getMarginalProbabilityAtPosition (my covariances -> item[im], vector, x);
		p += my mixingProbabilities[im] * pim;
	}
	return p;
}

double GaussianMixture_getProbabilityAtPosition (GaussianMixture me, double *xpos)
{
	double p = 0;
	for (long im = 1; im <= my numberOfComponents; im++)
	{
		double pim = Covariance_getProbabilityAtPosition (my covariances -> item[im], xpos);
		p += my mixingProbabilities[im] * pim;
	}
	return p;
}

Matrix GaussianMixture_and_PCA_to_Matrix_density (GaussianMixture me, PCA thee, long d1, long d2, double xmin, double xmax, long nx, double ymin, double ymax, long ny)
{
	Matrix him = NULL;
	double *v = NULL, nsigmas = 2;

	if (my dimension != thy dimension) return Melder_errorp1 (L"Dimensions must be equal.");
	if (d1 > thy numberOfEigenvalues || d2 > thy numberOfEigenvalues) return Melder_errorp1 (L"Direction index too high.");

	if ((v = NUMdvector (1, my dimension)) == NULL) return NULL;

	if (xmax == xmin || ymax == ymin)
	{
		double xmind, xmaxd, ymind, ymaxd;
		if (! GaussianMixture_and_PCA_getIntervalsAlongDirections (me, thee, d1, d2, nsigmas, &xmind, &xmaxd, &ymind, &ymaxd)) goto end;
		if (xmax == xmin) { xmin = xmind; xmax = xmaxd; }
		if (ymax == ymin) { ymin = ymind; ymax = ymaxd; }
	}
	// xmin,xmax and ymin,ymax are coordinates in the pc1 vs pc2 plane

	double dx = fabs (xmax - xmin)/ nx, dy = fabs (ymax - ymin) / ny;
	double x1 = xmin + 0.5 * dx, y1 = ymin + 0.5 * dy;
	him = Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
	if (him == NULL) goto end;

	for (long i = 1; i <= ny; i++)
	{
		double y = y1 + (i - 1) * dy;
		for (long j = 1; j <= nx; j++)
		{
			double x = x1 + (j - 1) * dx;
			for (long k = 1; k <= my dimension; k++)
			{
				v[k] = x * thy eigenvectors[d1][k] + y * thy eigenvectors[d2][k];
			}
			his z[i][j] = GaussianMixture_getProbabilityAtPosition (me, v);
		}
	}

end:
	NUMdvector_free (v, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

TableOfReal GaussianMixture_to_TableOfReal_randomSampling (GaussianMixture me, long numberOfPoints)
{
	double *buf = NULL;
	Covariance cov = my covariances -> item[1];
	TableOfReal thee = TableOfReal_create (numberOfPoints, my dimension);
	if (thee == NULL || ((buf = NUMdvector (1, my dimension)) == NULL) ||
		! NUMstrings_copyElements (cov -> columnLabels, thy columnLabels, 1, my dimension)) goto end;
	for (long i = 1; i <= numberOfPoints; i++)
	{
		wchar_t *covname;
		if (! GaussianMixture_generateOneVector (me, thy data[i], covname, buf)) goto end;
		TableOfReal_setRowLabel (thee, i, covname);
	}
end:
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (thee);
	GaussianMixture_unExpandPCA (me);
	return thee;
}

TableOfReal GaussianMixture_and_TableOfReal_to_TableOfReal_BHEPNormalityTests (GaussianMixture me, thou, double h)
{
	thouart (TableOfReal);
	long n = thy numberOfRows, d = thy numberOfColumns, nocp1 = my numberOfComponents + 1;
	double d2 = d / 2.0, prob, tnb, lnmu, lnvar;

	if (d != my dimension) return NULL;

	/* We cannot use a classification table because this could weigh a far-off data point with high probability */

	double **p = NUMdmatrix (1, thy numberOfRows + 1, 1, my numberOfComponents + 1);
	if (p == NULL) return NULL;

	if (! GaussianMixture_and_TableOfReal_getProbabilities (me, thee, 0, p)) goto end;

	// prob, beta, tnbo, lnmu, lnvar, ndata, ncol
	TableOfReal him = TableOfReal_create (my numberOfComponents, 7);
	if (him == NULL) goto end;

	// labels

	long iprob = 1, ih = 2, itnb = 3, ilnmu = 4, ilnvar = 5, indata = 6, id = 7;
	wchar_t *label[8] = { L"", L"p", L"h", L"tnb", L"lnmu", L"lnvar", L"ndata", L"d" };
	for (long icol = 1; icol <= 7; icol++)
	{
		TableOfReal_setColumnLabel (him, icol,label[icol]);
	}
	for (long irow = 1; irow <= my numberOfComponents; irow++)
	{
		Covariance cov = my covariances -> item[irow];
		TableOfReal_setRowLabel (him, irow, Thing_getName (cov));
	}

	for (long icol = 1 ; icol <= my numberOfComponents; icol++)
	{
		his data[icol][indata] = p[n + 1][icol];
	}

	for (long im = 1; im <= my numberOfComponents; im++)
	{
		Covariance cov = my covariances -> item[im];
		double mixingP = my mixingProbabilities[im];
		double nd = his data[im][indata];
		double beta = h > 0 ? NUMsqrt1_2 / h : NUMsqrt1_2 * pow ((1.0 + 2 * d ) / 4, 1.0 / (d + 4 )) * pow (nd, 1.0 / (d + 4));
		double beta2 = beta * beta, beta4 = beta2 * beta2, beta8 = beta4 * beta4;
		double gamma = 1 + 2 * beta2, gamma2 = gamma * gamma, gamma4 = gamma2 * gamma2;
		double delta = 1.0 + beta2 * (4 + 3 * beta2), delta2 = delta * delta;
		double mu = 1.0 - pow (gamma, -d2) * (1.0 + d * beta2 / gamma + d * (d + 2) * beta4 / (2 * gamma2));
		double var = 2.0 * pow (1 + 4 * beta2, -d2)
			+ 2.0 * pow (gamma,  -d) * (1.0 + 2 * d * beta4 / gamma2  + 3 * d * (d + 2) * beta8 / (4 * gamma4))
			- 4.0 * pow (delta, -d2) * (1.0 + 3 * d * beta4 / (2 * delta) + d * (d + 2) * beta8 / (2 * delta2));
		double mu2 = mu * mu;

		prob = tnb = lnmu = lnvar = NUMundefined;

		if (! SSCP_expandLowerCholesky (cov))
		{
			tnb = 4 * nd;
		}
		else
		{
			double djk, djj, sumjk = 0, sumj = 0;
			double b1 = beta2 / 2, b2 = b1 / (1.0 + beta2);

			/* Heinze & Wagner (1997), page 3
				We use d[j][k] = ||Y[j]-Y[k]||^2 = (Y[j]-Y[k])'S^(-1)(Y[j]-Y[k])
				So d[j][k]= d[k][j] and d[j][j] = 0
			*/
			for (long j = 1; j <= n; j++)
			{
				double wj = p[j][nocp1] > 0 ? mixingP * p[j][im] / p[j][nocp1] : 0;
				for (long k = 1; k < j; k++)
				{
					djk = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data[j], thy data[k], d, d);
					double w = p[k][nocp1] > 0 ? wj * mixingP * p[k][im] / p[k][nocp1] : 0;
					sumjk += 2 * w * exp (-b1 * djk); // factor 2 because d[j][k] == d[k][j]
				}
				sumjk += wj * wj; // for k == j. Is this ok now for probability weighing ????
				djj = NUMmahalanobisDistance_chi (cov -> lowerCholesky, thy data[j], cov -> centroid, d, d);
				sumj += wj * exp (-b2 * djj);
			}
			tnb = (1.0 / nd) * sumjk - 2.0 * pow (1.0 + beta2, - d2) * sumj + nd * pow (gamma, - d2); // n *
		}
		his data[im][ilnmu] = lnmu = 0.5 * log (mu2 * mu2 / (mu2 + var)); //log (sqrt (mu2 * mu2 /(mu2 + var)));
		his data[im][ilnvar] = lnvar = sqrt (log ((mu2 + var) / mu2));
		his data[im][iprob] = prob = NUMlogNormalQ (tnb, lnmu, lnvar);
		his data[im][ih] = NUMsqrt1_2 / beta;
		his data[im][id] = d;
		his data[im][itnb] = tnb;
	}
end:
	NUMdmatrix_free (p, 1, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

/* End of file GaussianMixture.c */
