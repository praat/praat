/* ICA.c
 *
 * Copyright (C) 2010-2011 David Weenink
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
  djmw 20101202 Initial version
*/

#include "ICA.h"
#include "Interpreter.h"
#include "NUM2.h"
#include "PCA.h"
#include "SVD.h"

// matrix multiply R = V*C*V', V is nrv x ncv, C is ncv x ncv, R is nrv x nrv
static void NUMdmatrices_multiply_VCVp (double **r, double **v, long nrv, long ncv, double **c, int csym)
{
	for (long i = 1; i <= nrv; i++)
	{
		long jstart = csym ? i : 1;
		for (long j = jstart; j <= nrv; j++)
		{
			// V_ik C_kl V'_lj = V_ik C_kl V_jl
			double vcv = 0;
			for (long k = 1; k <= ncv; k++)
			{
				for (long l = 1; l <= ncv; l++) { vcv += v[i][k] * c[k][l] * v[j][l]; }
			}
			r[i][j] = vcv;
			if (csym) r[j][i] = vcv;
		}
	}
}

// matrix multiply R = V'*C*V, V is nrv x ncv, C is ncv x ncv, R is nrv x nrv
static void NUMdmatrices_multiply_VpCV (double **r, double **v, long nrv, long ncv, double **c, int csym)
{
	for (long i = 1; i <= ncv; i++)
	{
		long jstart = csym ? i : 1;
		for (long j = jstart; j <= ncv; j++)
		{
			// V'_ik C_kl V_lj = V_ki C_kl V_lj
			double vcv = 0;
			for (long k = 1; k <= nrv; k++)
			{
				for (long l = 1; l <= ncv; l++) { vcv += v[k][i] * c[k][l] * v[l][j]; }
			}
			r[i][j] = vcv;
			if (csym) r[j][i] = vcv;
		}
	}
}

// matrix multiply V*C, V is nrv x ncv, C is ncv x ncc, R is nrv x ncc;
static void NUMdmatrices_multiply_VC (double **r, double **v, long nrv, long ncv, double **c, long ncc)
{
	for (long i = 1; i <= nrv; i++)
	{
		for (long j = 1; j <= ncc; j++)
		{
			// V_ik C_kj
			double vc = 0;
			for (long k = 1; k <= ncv; k++) { vc += v[i][k] * c[k][j]; }
			r[i][j] = vc;
		}
	}
}

// matrix multiply V'*C, V is nrv x ncv, C is nrv x ncc, R is ncv x ncc;
static void NUMdmatrices_multiply_VpC (double **r, double **v, long nrv, long ncv, double **c, long ncc)
{
	for (long i = 1; i <= ncv; i++)
	{
		for (long j = 1; j <= ncc; j++)
		{
			// V'_ik C_kj
			double vc = 0;
			for (long k = 1; k <= nrv; k++) { vc += v[k][i] * c[k][j]; }
			r[i][j] = vc;
		}
	}
}

// D += scalef * M * M', M = nrm x ncm, D is nrm x nrm
static void NUMdmatrices_multiplyScaleAdd (double **r, double **m, long nrm, long ncm, double scalef)
{
	for (long i = 1; i <= nrm; i++)
	{
		for (long j = 1; j <= nrm; j++)
		{
			// M_ik M'_kj = M_ik M_jk
			double mm = 0;
			for (long k = 1; k <= ncm; k++) { mm += m[i][k] * m[j][k]; }
			r[i][j] += scalef * mm;
		}
	}
}

/*
	d = diag(diag(W'*C0*W));
	W = W*d^(-1/2);

	D_ij = W'_ik C_kl W_lj => D_ii = W_ki C_kl W_li
*/
static void NUMdmatrix_normalizeColumnVectors (double **w, long nrw, long ncw, double **c)
{
	for (long i = 1; i <= ncw; i++)
	{
		double di = 0;
		for (long k = 1; k <= ncw; k++)
			for (long l = 1; l <= nrw; l++) { di += w[k][i] * c[k][l] * w[l][i]; }
		di = 1 / sqrt (di);
		for (long j = 1; j <= nrw; j++) { w[j][i] *= di; }
	}
}

static double NUMdmatrix_diagonalityMeasure (double **v, long dimension)
{
	double dmsq = 0;
	if (dimension < 2) return 0;
	for (long i = 1; i <= dimension; i++)
	{
		for (long j = 1; j <= dimension; j++)
		{
			if (i != j) { dmsq += v[i][j] * v[i][j]; }
		}
	}
	return dmsq / (dimension * (dimension -1 ));
}

static double NUMdmatrix_diagonalityIndex (double **v, long dimension)
{
	double dindex = 0;
	for (long irow = 1; irow <= dimension; irow++)
	{
		double rowmax = fabs(v[irow][1]), rowsum = 0;
		for (long icol = 2; icol <= dimension; icol++)
		{
			if (fabs (v[irow][icol]) > rowmax) rowmax = fabs (v[irow][icol]);
		}
		for (long icol = 1; icol <= dimension; icol++)
		{
			rowsum += fabs (v[irow][icol]) / rowmax;
		}
		dindex += rowsum - 1;
	}
	for (long icol = 1; icol <= dimension; icol++)
	{
		double colmax = fabs(v[icol][1]), colsum = 0;
		for (long irow = 2; irow <= dimension; irow++)
		{
			if (fabs (v[irow][icol]) > colmax) colmax = fabs (v[irow][icol]);
		}
		for (long irow = 1; irow <= dimension; irow++)
		{
			colsum += fabs (v[irow][icol]) / colmax;
		}
		dindex += colsum - 1;
	}
	return dindex;
}

/*
	This routine is modeled after qdiag.m from Andreas Ziehe, Pavel Laskov, Guido Nolte, Klaus-Robert Müller,
	A Fast Algorithm for Joint Diagonalization with Non-orthogonal Transformations and its Application to
	Blind Source Separation, Journal of Machine Learning Research 5 (2004), 777–800.
*/
static int Diagonalizer_and_CrossCorrelationTables_ffdiag (Diagonalizer me, CrossCorrelationTables thee, long maxNumberOfIterations, double delta)
{
	long iter = 0, dimension = my numberOfRows;
	double theta = 1, **w = NULL, **v = my data, **vnew = NULL, **cc = NULL;
	double dm_old, dm_new;
	CrossCorrelationTables ccts = NULL;

// start:
	ccts = CrossCorrelationTables_and_Diagonalizer_diagonalize (thee, me); cherror
	w = NUMdmatrix (1, dimension, 1, dimension); cherror
	vnew = NUMdmatrix (1, dimension, 1, dimension); cherror
	cc = NUMdmatrix (1, dimension, 1, dimension); cherror

	for (long i = 1; i<= dimension; i++) { w[i][i] = 1; }

	MelderInfo_open ();
	dm_new = CrossCorrelationTables_getDiagonalityMeasure (ccts, NULL, 0, 0);
	MelderInfo_writeLine5 (L"\nIteration ", Melder_integer (iter), L":  ", Melder_double (dm_new), L" (= diagonality measurement)");

	do
	{
		dm_old = dm_new;
		for (long i = 1; i <= dimension; i++)
		{
			for (long j = i + 1; j <= dimension; j++)
			{
				double zii = 0, zij = 0, zjj = 0, yij = 0, yji = 0; // zij = zji
				for (long k = 1; k <= ccts -> size; k++)
				{
					CrossCorrelationTable ct = ccts -> item [k];
					zii += ct -> data[i][i] * ct -> data[i][i];
					zij += ct -> data[i][i] * ct -> data[j][j];
					zjj += ct -> data[j][j] * ct -> data[j][j];
					yij += ct -> data[j][j] * ct -> data[i][j];
					yji += ct -> data[i][i] * ct -> data[i][j];
				}
				double denom = zjj * zii - zij * zij;
				if (denom != 0)
				{
					w[i][j] = (zij * yji - zii * yij) / denom;
					w[j][i] = (zij * yij - zjj * yji) / denom;
				}
			}
		}
		double norma = 0;
		for (long i = 1; i <= dimension; i++)
		{
			double normai = 0;
			for (long j = 1; j <= dimension; j++)
			{
				if (i != j) { normai += fabs (w[i][j]); }
			}
			if (normai > norma) norma = normai;
		}
		// evaluate the norm
		if (norma > theta)
		{
			double normf = 0;
			for (long i = 1; i <= dimension; i++)
				for (long j = 1; j <= dimension; j++)
					if (i != j) normf += w[i][j] * w[i][j];
			double scalef = theta / sqrt (normf);
			for (long i = 1; i <= dimension; i++)
			{
				for (long j = 1; j <= dimension; j++)
				{
					if (i != j) { w[i][j] *= scalef; }
				}
			}
		}
		// update V
		NUMdmatrix_copyElements (v, vnew, 1, dimension, 1, dimension);
		NUMdmatrices_multiply_VC (v, w, dimension, dimension, vnew, dimension);
		for (long k = 1; k <= ccts -> size; k++)
		{
			CrossCorrelationTable ct = ccts -> item[k];
			NUMdmatrix_copyElements (ct -> data, cc, 1, dimension, 1, dimension);
			NUMdmatrices_multiply_VCVp (ct -> data, w, dimension, dimension, cc, 1);
		}
		dm_new = CrossCorrelationTables_getDiagonalityMeasure (ccts, NULL, 0, 0);
		iter++;
		MelderInfo_writeLine5 (L"\nIteration ", Melder_integer (iter), L":  ", Melder_double (dm_new), L" (= diagonality measurement)");
	} while (fabs((dm_old - dm_new) / dm_new) > delta && iter < maxNumberOfIterations);

end:
	MelderInfo_close ();
	NUMdmatrix_free (w, 1, 1); NUMdmatrix_free (vnew, 1, 1);
	NUMdmatrix_free (cc, 1, 1); forget (ccts);
	return ! Melder_hasError ();
}

/*
	The folowing two routine are modeled after qdiag.m from
	R. Vollgraf and K. Obermayer, Quadratic Optimization for Simultaneous
	Matrix Diagonalization, IEEE Transaction on Signal Processing, 2006,
*/

static void update_one_column (CrossCorrelationTables me, double **d, double *wp, double *wvec, double scalef, double *work)
{
	long dimension = ((CrossCorrelationTable)(my item[1])) -> numberOfColumns;

	for (long ic = 2; ic <= my size; ic++) // exclude C0
	{
		SSCP cov = my item[ic];
		double **c = cov -> data;
		// m1 = C * wvec
		for (long i = 1; i <= dimension; i++)
		{
			double r = 0;
			for (long j = 1; j <= dimension; j++) { r += c[i][j] * wvec[j]; }
			work[i] = r;
		}
		// D = D +/- 2*p(t)*(m1*m1');
		for (long i = 1; i <= dimension; i++)
		{
			for (long j = 1; j <= dimension; j++)
			{
				d[i][j] += 2 * scalef * wp[ic] * work[i] * work[j];
			}
		}
	}
}

static int Diagonalizer_and_CrossCorrelationTable_qdiag (Diagonalizer me, CrossCorrelationTables thee, double *cweights, long maxNumberOfIterations, double delta)
{
	CrossCorrelationTable c0 = thy item[1];
	double **w = my data;
	long dimension = c0 -> numberOfColumns;
	Eigen eigen = NULL;
	CrossCorrelationTables ccts = NULL;
	double **pinv = NULL,**d = NULL, **p = NULL, **m1 = NULL, *wvec = NULL, *wnew = NULL, *mvec = NULL, **wc = NULL;

// start:
	eigen = new (Eigen); cherror
	ccts = Data_copy (thee); cherror
	pinv = NUMdmatrix (1, dimension, 1, dimension); cherror
	d    = NUMdmatrix (1, dimension, 1, dimension); cherror
	p    = NUMdmatrix (1, dimension, 1, dimension); cherror
	m1   = NUMdmatrix (1, dimension, 1, dimension); cherror
	wc   = NUMdmatrix (1, dimension, 1, dimension); cherror
	wvec = NUMdvector (1, dimension); cherror
	wnew = NUMdvector (1, dimension); cherror
	mvec = NUMdvector (1, dimension); cherror

	for (long i = 1; i <= dimension; i++) // Transpose W
		for (long j = 1; j <= dimension; j++) { wc[i][j] = w[j][i]; }

	// d = diag(diag(W'*C0*W));
	// W = W*d^(-1/2);

	NUMdmatrix_normalizeColumnVectors (wc, dimension, dimension, c0 -> data);

	// scale eigenvectors for sphering
	// [vb,db] = eig(C0);
	// P = db^(-1/2)*vb';

	if (! Eigen_initFromSymmetricMatrix (eigen, c0 -> data, dimension)) goto end;
	for (long i = 1; i <= dimension; i++)
	{
		if (eigen -> eigenvalues[i] < 0) return Melder_error1 (L"Covariance matrix not positive definite.");
		double scalef = 1 / sqrt (eigen -> eigenvalues[i]);
		for (long j = 1; j <= dimension; j++) { p[dimension - i + 1][j] = scalef * eigen -> eigenvectors[i][j]; }
	}

	// P*C[i]*P'

	for (long ic = 1; ic <= thy size; ic++)
	{
		CrossCorrelationTable cov1 = thy item[ic];
		CrossCorrelationTable cov2 = ccts -> item[ic];
		NUMdmatrices_multiply_VCVp (cov2 -> data, p, dimension, dimension, cov1 -> data, 1);
	}

	// W = P'\W == inv(P') * W

	if (! NUMpseudoInverse (p, dimension, dimension, pinv, 0)) goto end;

	NUMdmatrices_multiply_VpC (w, pinv, dimension, dimension, wc, dimension);

	// initialisation for order KN^3

	for (long ic = 2; ic <= thy size; ic++)
	{
		CrossCorrelationTable cov = ccts -> item[ic];
		// C * W
	    NUMdmatrices_multiply_VC (m1, cov -> data, dimension, dimension, w, dimension);
		// D += scalef * M1*M1'
		NUMdmatrices_multiplyScaleAdd (d, m1, dimension, dimension, 2 * cweights[ic]);
	}

	long iter = 0;
	double delta_w;
	MelderInfo_open ();
	do
	{
		/*
		 * the standard diagonality measure is rather expensive to calculate so we compare the norms of
		 * differences of eigenvectors.
		 */
		delta_w = 0;
		for (long kol = 1; kol <= dimension; kol++)
		{
			for (long i = 1; i <= dimension; i++) { wvec[i] = w[i][kol]; }

			update_one_column (ccts, d, cweights, wvec, -1, mvec);

			if (! Eigen_initFromSymmetricMatrix (eigen, d, dimension)) goto end;

			// Eigenvalues already sorted; get eigenvector of smallest !

			for (long i = 1; i <= dimension; i++) { wnew[i] = eigen -> eigenvectors[dimension][i]; }

			update_one_column (ccts, d, cweights, wnew, 1, mvec);
			for (long i = 1; i <= dimension; i++) { w[i][kol] = wnew[i]; }

			/*
			 * compare norms of eigenvectors. We have to compare ||wvec +/- w_new|| because eigenvectors
			 * may change sign.
			 */


			double normp = 0, normm = 0;
			for (long j = 1; j <= dimension; j++)
			{
				double dm = wvec[j] - wnew[j], dp = wvec[j] + wnew[j];
				normp += dm * dm; normm += dp * dp;
			}

			normp = normp < normm ? normp : normm;
			normp = sqrt (normp);
			delta_w = normp > delta_w ? normp : delta_w;
		}
		iter++;
		MelderInfo_writeLine5 (L"\nIteration ", Melder_integer (iter), L":  ", Melder_double (delta_w), L" (= vector norm difference)");
	} while (delta_w > delta && iter < maxNumberOfIterations);

	/* Revert the sphering W = P'*W;
	 * Take transpose to make W*C[i]W' diagonal instead of W'*C[i]*W => (P'*W)'=W'*P
	 * Calculate the "real" diagonality measure
	 */

	NUMdmatrix_copyElements (w, wc, 1, dimension, 1, dimension);
	NUMdmatrices_multiply_VpC (w, wc, dimension, dimension, p, dimension); // W = W'*P: final result

	double dm = CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure (thee, me, cweights, 1, thy size);
	MelderInfo_writeLine5 (L"\nDiagonality measure: ", Melder_double (dm), L" after ", Melder_integer (iter),
		L" iterations.");

end:
	MelderInfo_close ();
	forget (eigen); forget (ccts);
	NUMdmatrix_free (d, 1, 1); NUMdmatrix_free (m1, 1, 1); NUMdmatrix_free (p, 1, 1);
	NUMdmatrix_free (pinv, 1, 1); NUMdmatrix_free (wc, 1, 1);
	NUMdvector_free (wvec, 1); NUMdvector_free (wnew, 1); NUMdvector_free (mvec, 1);
	return ! Melder_hasError ();
}

int MixingMatrix_and_CrossCorrelationTables_improveUnmixing (MixingMatrix me, CrossCorrelationTables thee, long maxNumberOfIterations, double tol, int method)
{
	int status = 0;
	Diagonalizer him = MixingMatrix_to_Diagonalizer (me); cherror

	status = Diagonalizer_and_CrossCorrelationTables_improveDiagonality (him, thee, maxNumberOfIterations, tol, method) &&
		NUMpseudoInverse (his data, his numberOfRows, his numberOfColumns, my data, 0);
end:
	forget (him);
	return status;
}

/*
	This is for multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples, (z[i][k] - mean[i])(z[j][k + tau] - mean[j]))*samplingTime
*/
CrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me, double startTime, double endTime, double lagTime)
{
	if (endTime <= startTime) { startTime = my xmin; endTime = my xmax; }
	long ndelta = lagTime / my dx;
	long i1 = Sampled_xToNearestIndex (me, startTime);
	if (i1 < 1) i1 = 1;
	long i2 = Sampled_xToNearestIndex (me, endTime);
	if (i2 > my nx) i2 = my nx;
	i2 -= ndelta;
	long nsamples = i2 - i1 + 1;
	if (nsamples <= my ny) return Melder_errorp1 (L"Not enough samples");
	CrossCorrelationTable thee = CrossCorrelationTable_create (my ny);
	if (thee == NULL) return NULL;
	double **data = my z;

	for (long ichan = 1; ichan <= my ny; ichan++)
	{
		double *z = my z[ichan];
		double sum = 0;
		for (long k = i1; k <= i2; k++) sum += z[k];
		thy centroid[ichan] = sum / nsamples;
	}

	double *mean = thy centroid;

	for (long i = 1; i <= my ny; i++)
	{
		for (long j = i; j <= my ny; j++)
		{
			double cc = 0;
			for (long k = i1; k <= i2; k++) { cc += (data[i][k] - mean[i]) * (data[j][k + ndelta] - mean[j]); }
			thy data[j][i] = thy data[i][j] = cc * my dx;
		}
	}

	thy numberOfObservations = nsamples;

	return thee;
}

CrossCorrelationTables Sound_to_CrossCorrelationTables (Sound me, double startTime, double endTime, double lagTime, long ncovars)
{
	if (lagTime < my dx) lagTime = my dx;
	if (startTime + ncovars * lagTime > endTime) return Melder_errorp1 (L"Lag time too large.");
	if (endTime <= startTime) { startTime = my xmin; endTime = my xmax; }
	CrossCorrelationTables thee = CrossCorrelationTables_create ();
	if (thee == NULL) return NULL;
	for (long i = 1; i <= ncovars; i++)
	{
		double lag = (i - 1) * lagTime;
		CrossCorrelationTable ct = Sound_to_CrossCorrelationTable (me, startTime, endTime, lag);
		if (ct == NULL || ! Collection_addItem (thee, ct)) goto end;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Sound Sound_to_Sound_BSS (Sound me, double startTime, double endTime, long ncovars, double lagTime, long maxNumberOfIterations, double tol, int method)
{
	MixingMatrix him = Sound_to_MixingMatrix (me, startTime, endTime, ncovars, lagTime, maxNumberOfIterations, tol, method);
	if (him == NULL) return NULL;

	Sound thee = Sound_and_MixingMatrix_unmix (me, him);
	forget (him);
	return thee;
}

PCA Sound_to_PCA (Sound me, double startTime, double endTime)
{
	CrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, startTime, endTime, 0);
	if (thee == NULL) return NULL;
	PCA him = SSCP_to_PCA (thee);
	forget (thee);
	return him;
}

/************************ Diagonalizer **********************************/

class_methods (Diagonalizer, TableOfReal)
{
	class_methods_end
}

Diagonalizer Diagonalizer_create (long dimension)
{
	Diagonalizer me = new (Diagonalizer);
	if (me == NULL || ! TableOfReal_init (me, dimension, dimension)) goto end;
	for (long i = 1; i <= dimension; i++) { my data[i][i] = 1; }
end:
	if (Melder_hasError ()) forget (me);
	return me;
}


/************************ MixingMatrix **********************************/

class_methods (MixingMatrix, TableOfReal)
{
	class_methods_end
}

MixingMatrix MixingMatrix_create (long numberOfChannels, long numberOfComponents)
{
	MixingMatrix me = new (MixingMatrix);
	if (me == NULL || ! TableOfReal_init (me, numberOfChannels, numberOfComponents)) goto end;
	MixingMatrix_initializeRandom (me);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

MixingMatrix MixingMatrix_createSimple (long numberOfChannels, long numberOfComponents, wchar_t *elements)
{
	long inum = 1, irow, icol, ntokens = Melder_countTokens (elements);
	if (ntokens == 0) return Melder_errorp1 (L"No matrix elements.");
	long nwanted = numberOfChannels * numberOfComponents;

	MixingMatrix me = MixingMatrix_create (numberOfChannels, numberOfComponents);
	if (me == NULL) return NULL;

	// Construct the full matrix from the elements
	double number;
	for (wchar_t *token = Melder_firstToken (elements); token != NULL && inum <= ntokens; token = Melder_nextToken (), inum++)
	{
		irow = (inum - 1) / numberOfComponents + 1;
		icol = (inum - 1) % numberOfComponents + 1;
		if (! Interpreter_numericExpression (NULL, token, &number))
		{
			Melder_error5 (L"MixingMatrix: item ", Melder_integer (inum), L" \"", token, L"\"is not a number.");
			goto end;
		}
		my data[irow][icol] = number;
	}
	if (ntokens < nwanted)
	{
		for (long i = inum; i <= nwanted; i++)
		{
			irow = (inum - 1) / numberOfComponents + 1;
			icol = (inum - 1) % numberOfComponents + 1;
			my data[irow][icol] = number; // repeat the last number given!
		}
	}
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

void MixingMatrix_initializeRandom (MixingMatrix me)
{
	for (long i = 1; i <= my numberOfRows; i++)
	{
		for (long j = 1; j <= my numberOfColumns; j++)
		{
			my data[i][j] = NUMrandomGauss (0, 1);
		}
	}
}

/***************** Diagonalizer & MixingMatrix *************************/


Diagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me)
{
	if (my numberOfRows != my numberOfColumns) return Melder_errorp1 (L"The number of channels and the number of components must be equal.");
	Diagonalizer thee = Diagonalizer_create (my numberOfRows);
	if (thee == NULL|| ! NUMpseudoInverse (my data, my numberOfRows, my numberOfColumns, thy data, 0)) forget (thee);
	return thee;
}

MixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me)
{
	MixingMatrix thee = MixingMatrix_create (my numberOfRows, my numberOfColumns);
	if (thee == NULL || ! NUMpseudoInverse (my data, my numberOfRows, my numberOfColumns, thy data, 0)) forget (thee);
	return thee;
}

/********************* Sound & MixingMatrix ************************************/

Sound Sound_and_MixingMatrix_mix (Sound me, MixingMatrix thee)
{
	if (my ny != thy numberOfRows) return Melder_errorp1 (L"The MixingMatrix and the Sound must have the same number of channels.");
	Sound him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
	if (him != NULL)
	{
		for (long i = 1; i <= thy numberOfRows; i++)
		{
			for (long j = 1; j <= my nx; j++)
			{
				double mix = 0;
				for (long k = 1; k <= my ny; k++) { mix += thy data[i][k] * my z[k][j]; }
				his z[i][j] = mix;
			}
		}
	}
	return him;
}

Sound Sound_and_MixingMatrix_unmix (Sound me, MixingMatrix thee)
{
	Sound him = NULL;
	double **minv = NULL;
	if (my ny != thy numberOfRows) return Melder_errorp1 (L"The MixingMatrix and the Sound must have the same number of channels.");
//start:
	minv = NUMdmatrix (1, thy numberOfColumns, 1, thy numberOfRows); cherror

	if (! NUMpseudoInverse (thy data, thy numberOfRows, thy numberOfColumns, minv, 0)) goto end;
	him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1); cherror
	for (long i = 1; i <= thy numberOfColumns; i++)
	{
		for (long j = 1; j <= my nx; j++)
		{
			double s = 0;
			for (long k = 1; k <= my ny; k++) { s += minv[i][k] * my z[k][j]; }
			his z[i][j] = s;
		}
	}
end:
	NUMdmatrix_free (minv, 1, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

MixingMatrix Sound_to_MixingMatrix (Sound me, double startTime, double endTime, long ncovars, double lagTime, long maxNumberOfIterations, double tol, int method)
{
	MixingMatrix thee = NULL; CrossCorrelationTables ccs = NULL;
//start:
	ccs = Sound_to_CrossCorrelationTables (me, startTime, endTime, lagTime, ncovars); cherror
	thee = MixingMatrix_create (my ny, my ny); cherror
	if (! MixingMatrix_and_CrossCorrelationTables_improveUnmixing (thee, ccs, maxNumberOfIterations, tol, method)) goto end;

end:
	forget (ccs);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

MixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me)
{
	if (my numberOfColumns != my numberOfRows) return Melder_errorp1 (L"Number of rows and columns must be equal.");
	MixingMatrix thee = Data_copy (me);
	if (thee != NULL) Thing_overrideClass (thee, classMixingMatrix);
	return thee;
}

/************* CrossCorrelationTable *****************************/

static void classCrossCorrelationTable_info (I)
{
	iam (CrossCorrelationTable);
	classSSCP -> info (me);
	double dm = CrossCorrelationTable_getDiagonalitymeasure (me);
	MelderInfo_writeLine2 (L"Diagonality measure: ", Melder_double (dm));
}

class_methods (CrossCorrelationTable, SSCP)
{
	class_method_local (CrossCorrelationTable, info)
	class_methods_end
}

CrossCorrelationTable CrossCorrelationTable_create (long dimension)
{
	CrossCorrelationTable me = new (CrossCorrelationTable);
	if (me == NULL || ! SSCP_init (me, dimension, dimension)) forget (me);
	return me;
}

CrossCorrelationTable CrossCorrelationTable_createSimple (wchar_t *covars, wchar_t *centroid, long numberOfSamples)
{
	long dimension = Melder_countTokens (centroid);
	long ncovars = Melder_countTokens (covars);
	long ncovars_wanted = dimension * (dimension + 1) / 2;
	if (ncovars != ncovars_wanted) return Melder_errorp1 (L"The number of matrix elements and the number of "
		"centroid elements are not in concordance. There should be \"d(d+1)/2\" matrix values and \"d\" centroid values.");

	CrossCorrelationTable me = CrossCorrelationTable_create (dimension); cherror

	// Construct the full matrix from the upper-diagonal elements

	long inum = 1, irow = 1, icol, nmissing, inumc;
	for (wchar_t *token = Melder_firstToken (covars); token != NULL && inum <= ncovars_wanted; token = Melder_nextToken (), inum++)
	{
		double number;
		nmissing = (irow - 1) * irow / 2;
		inumc = inum + nmissing;
		irow = (inumc - 1) / dimension + 1;
		icol = ((inumc - 1) % dimension) + 1;
		if (! Interpreter_numericExpression (NULL, token, &number))
		{
			Melder_error5 (L"CrossCorrelationTable: item ", Melder_integer (inum), L" \"", token, L"\"is not a number.");
			goto end;
		}
		my data[irow][icol] = my data[icol][irow] = number;
		if (icol == dimension) irow++;
	}


	inum = 1;
	for (wchar_t *token = Melder_firstToken (centroid); token != NULL && inum <= dimension; token = Melder_nextToken (), inum++)
	{
		double number;
		if (! Interpreter_numericExpression (NULL, token, &number))
		{
			Melder_error5 (L"Centroid: item ", Melder_integer (inum), L" \"", token, L"\"is not a number.");
			goto end;
		}
		my centroid[inum] = number;
	}
	my numberOfObservations = numberOfSamples;
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

double CrossCorrelationTable_getDiagonalitymeasure (CrossCorrelationTable me)
{
	return NUMdmatrix_diagonalityMeasure (my data, my numberOfColumns);
}

/************* CrossCorrelationTables *****************************/

static void classCrossCorrelationTables_info (I)
{
	iam (CrossCorrelationTables);
	classOrdered -> info (me);
	CrossCorrelationTable thee = my item[1];
	MelderInfo_writeLine2 (L"  Number of rows and columns: ", Melder_integer (thy numberOfRows));
	for (long i = 1; i <= my size; i++)
	{
		double dm = CrossCorrelationTable_getDiagonalitymeasure (my item[i]);
		MelderInfo_writeLine4 (L"Diagonality measure for item ", Melder_integer (i), L": ", Melder_double (dm));
	}
}

class_methods (CrossCorrelationTables, Ordered)
{
	class_method_local (CrossCorrelationTables, info)
	class_methods_end
}

CrossCorrelationTables CrossCorrelationTables_create (void)
{
	CrossCorrelationTables me = new (CrossCorrelationTables);
	if (me == NULL || ! Ordered_init (me, classCrossCorrelationTable, 30)) forget (me);
	return me;
}

double CrossCorrelationTables_getDiagonalityMeasure (CrossCorrelationTables me, double *w, long start, long end)
{
	if (start >= end) { start = 1; end = my size; }
	if (start < 1) start = 1;
	if (end > my size) end = my size;
	long ntables = end - start + 1;
	long dimension = ((Covariance)(my item[1]))-> numberOfColumns;
	double dmsq = 0;
	for (long k = start; k <= end; k++)
	{
		CrossCorrelationTable thee = my item[k];
		double dmksq = NUMdmatrix_diagonalityMeasure (thy data, dimension);
		dmsq += w == NULL ? dmksq / ntables : dmksq * w[k];
	}
	return dmsq;
}

/************************** CrossCorrelationTables & Diagonalizer *******************************/

double CrossCorrelationTables_and_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTables me, Diagonalizer thee, double *w, long start, long end)
{
	CrossCorrelationTables him = CrossCorrelationTables_and_Diagonalizer_diagonalize (me, thee);
	if (him == NULL) return NUMundefined;
	double dm = CrossCorrelationTables_getDiagonalityMeasure (him, w, start, end);
	forget (him);
	return dm;
}

CrossCorrelationTable CrossCorrelationTable_and_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee)
{
	if (my numberOfRows != thy numberOfRows) return Melder_errorp1 (L"The CrossCorrelationTable and the Diagonalizer matrix dimensions must be equal.");
	CrossCorrelationTable him = CrossCorrelationTable_create (my numberOfColumns);
	if (him == NULL) return NULL;
	NUMdmatrices_multiply_VCVp (his data, thy data, my numberOfColumns, my numberOfColumns, my data, 1);
	return him;
}

CrossCorrelationTables CrossCorrelationTables_and_Diagonalizer_diagonalize (CrossCorrelationTables me, Diagonalizer thee)
{
	CrossCorrelationTables him = CrossCorrelationTables_create ();
	if (him == NULL) return NULL;
	for (long i = 1; i <= my size; i++)
	{
		CrossCorrelationTable ct = CrossCorrelationTable_and_Diagonalizer_diagonalize (my item[i], thee);
		if (ct == NULL || ! Collection_addItem (him, ct)) goto end;
	}
end:
	if (Melder_hasError ()) forget (him);
	return him;
}

Diagonalizer CrossCorrelationTables_to_Diagonalizer (CrossCorrelationTables me, long maxNumberOfIterations, double tol, int method)
{
	CrossCorrelationTable him = my item[1];
	Diagonalizer thee = Diagonalizer_create (his numberOfColumns);
	if (thee == NULL) return NULL;
	if (! Diagonalizer_and_CrossCorrelationTables_improveDiagonality (thee, me, maxNumberOfIterations, tol, method)) forget (thee);
	return thee;
}

int Diagonalizer_and_CrossCorrelationTables_improveDiagonality (Diagonalizer me, CrossCorrelationTables thee, long maxNumberOfIterations, double tol, int method)
{
	int status = 0;
	if (method == 1)
	{
		double *cweights = NUMdvector (1, thy size);
		if (cweights == NULL) return 0;
		for (long i = 1; i <= thy size; i++) { cweights[i] = 1.0 / thy size; }
		status = Diagonalizer_and_CrossCorrelationTable_qdiag (me, thee, cweights, maxNumberOfIterations, tol);
		NUMdvector_free (cweights, 1);
	}
	else
	{
		status = Diagonalizer_and_CrossCorrelationTables_ffdiag (me, thee, maxNumberOfIterations, tol);
	}
	return status;
}

Sound Sound_and_PCA_to_Sound_pc (Sound me, PCA thee, long numberOfComponents, int whiten)
{
	if (my ny != thy dimension) return Melder_errorp1 (L"The number of channels of the sound and the dimension of the PCA must be equal.");
	if (numberOfComponents > my ny) numberOfComponents = my ny;

	Sound him = Sound_create (numberOfComponents, my xmin, my xmax, my nx, my dx, my x1);
	if (him == NULL) return NULL;
	for (long i = 1; i <= his ny; i++)
	{
		double scalef = whiten ? 1 / sqrt (thy eigenvalues[i]) : 1;
		for (long j = 1; j <= his nx; j++)
		{
			double s = 0;
			for (long k = 1; k <= my ny; k++) { s += thy eigenvectors[i][k] * my z[k][j]; }
			his z[i][j] = s * scalef;
		}
	}
	return him;
}

/*
 * Generate n different cct's that have a common diagonalizer.
 */
CrossCorrelationTables CrossCorrelationTables_createTestSet (long dimension, long n, int firstPositiveDefinite, double sigma)
{
	SVD svd = NULL;
	CrossCorrelationTables me = NULL;
	double ** d = NULL, **v = NULL;

// start:

	d = NUMdmatrix (1, dimension, 1, dimension); cherror
	/*
	 * Start with a square matrix with random gaussian elements and make its singular value decomposition UDV'
	 * The V matrix will be the common diagonalizer matrix that we use.
	 */
	for (long i = 1; i <= dimension; i++) // Generate the rotation matrix
	{
		for (long j = 1; j <= dimension; j++) { d[i][j] = NUMrandomGauss (0, 1); }
	}

	v = NUMdmatrix (1, dimension, 1, dimension); cherror
	svd = SVD_create_d (d, dimension, dimension); cherror
	me = CrossCorrelationTables_create (); cherror

	for (long i = 1; i <= dimension; i++)
	{
		for (long j = 1; j <= dimension; j++) { d[i][j] = 0; }
	}

	/*
	 * Start with a diagonal matrix D and calculate V'DV
	 */
	for (long k = 1; k <= n; k++)
	{
		CrossCorrelationTable ct = CrossCorrelationTable_create (dimension);
		if (ct == NULL || ! Collection_addItem (me, ct)) goto end;
		double low = k == 1 && firstPositiveDefinite ? 0.1 : -1;
		for (long i = 1; i <= dimension; i++)
		{
			d[i][i] = NUMrandomUniform (low, 1);
		}
		for (long i = 1; i <= dimension; i++)
		{
			for (long j = 1; j <= dimension; j++)
			{
				v[i][j] = NUMrandomGauss (svd -> v[i][j], sigma);
			}
		}
		// we need V'DV, however our V has eigenvectors row-wise -> VDV'
		NUMdmatrices_multiply_VCVp (ct -> data, v, dimension, dimension, d, 1);
	}

end:
	NUMdmatrix_free (d, 1, 1); NUMdmatrix_free (v, 1, 1); forget (svd);
	if (Melder_hasError ()) forget (me);
	return me;
}

static int Sound_and_MixingMatrix_improveUnmixing_fica (Sound me, MixingMatrix thee, long maxNumberOfIterations, double tol, int method)
{
	long iter = 0;
	if (my ny != thy numberOfColumns) return Melder_error1 (L"Dimensions do not agree.");
	double **x = NULL; // the data for ica

//start:
	x = NUMdmatrix_copy (my z, 1, my ny, 1, my nx); cherror
	do
	{

		iter++;
	} while (/*fabs((dm_old - dm_new) / dm_new) > tol &&*/ iter < maxNumberOfIterations);

end:
	NUMdmatrix_free (x, 1, 1);
	return 1;
}

/* End of file ICA.c */
