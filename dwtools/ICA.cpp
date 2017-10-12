/* ICA.cpp
 *
 * Copyright (C) 2010-2012, 2015-2017 David Weenink
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
  djmw 20101202 Initial version
  djmw 20110304 Thing_new
*/

#include "ICA.h"
#include "Interpreter.h"
#include "NUM2.h"
#include "Sound_and_PCA.h"
#include "SVD.h"

// matrix multiply R = V*C*V', V is nrv x ncv, C is ncv x ncv, R is nrv x nrv
static void NUMdmatrices_multiply_VCVp (double **r, double **v, long nrv, long ncv, double **c, bool csym) {
	for (long i = 1; i <= nrv; i ++) {
		long jstart = csym ? i : 1;
		for (long j = jstart; j <= nrv; j ++) {
			// V_ik C_kl V'_lj = V_ik C_kl V_jl
			double vcv = 0.0;
			for (long k = 1; k <= ncv; k ++) {
				for (long l = 1; l <= ncv; l ++) {
					vcv += v [i] [k] * c [k] [l] * v [j] [l];
				}
			}
			r [i] [j] = vcv;
			if (csym) {
				r [j] [i] = vcv;
			}
		}
	}
}

#if 0
// matrix multiply R = V'*C*V, V is nrv x ncv, C is ncv x ncv, R is nrv x nrv
static void NUMdmatrices_multiply_VpCV (double **r, double **v, long nrv, long ncv, double **c, int csym) {
	for (long i = 1; i <= ncv; i++) {
		long jstart = csym ? i : 1;
		for (long j = jstart; j <= ncv; j++) {
			// V'_ik C_kl V_lj = V_ki C_kl V_lj
			double vcv = 0;
			for (long k = 1; k <= nrv; k++) {
				for (long l = 1; l <= ncv; l++) {
					vcv += v[k][i] * c[k][l] * v[l][j];
				}
			}
			r[i][j] = vcv;
			if (csym) {
				r[j][i] = vcv;
			}
		}
	}
}
#endif

// matrix multiply V*C, V is nrv x ncv, C is ncv x ncc, R is nrv x ncc;
static void NUMdmatrices_multiply_VC (double **r, double **v, long nrv, long ncv, double **c, long ncc) {
	for (long i = 1; i <= nrv; i ++) {
		for (long j = 1; j <= ncc; j ++) {
			// V_ik C_kj
			real80 vc = 0.0;
			for (long k = 1; k <= ncv; k ++) {
				vc += v [i] [k] * c [k] [j];
			}
			r [i] [j] = (double) vc;
		}
	}
}

// matrix multiply V'*C, V is nrv x ncv, C is nrv x ncc, R is ncv x ncc;
static void NUMdmatrices_multiply_VpC (double **r, double **v, long nrv, long ncv, double **c, long ncc) {
	for (long i = 1; i <= ncv; i ++) {
		for (long j = 1; j <= ncc; j ++) {
			// V'_ik C_kj
			real80 vc = 0.0;
			for (long k = 1; k <= nrv; k ++) {
				vc += v [k] [i] * c [k] [j];
			}
			r [i] [j] = (double) vc;
		}
	}
}

// D += scalef * M * M', M = nrm x ncm, D is nrm x nrm
static void NUMdmatrices_multiplyScaleAdd (double **r, double **m, long nrm, long ncm, double scalef) {
	for (long i = 1; i <= nrm; i++) {
		for (long j = 1; j <= nrm; j++) {
			// M_ik M'_kj = M_ik M_jk
			real80 mm = 0.0;
			for (long k = 1; k <= ncm; k++) {
				mm += m[i][k] * m[j][k];
			}
			r[i][j] += scalef * mm;
		}
	}
}

/*
	d = diag(diag(W'*C0*W));
	W = W*d^(-1/2);

	D_ij = W'_ik C_kl W_lj => D_ii = W_ki C_kl W_li
*/
static void NUMdmatrix_normalizeColumnVectors (double **w, long nrw, long ncw, double **c) {
	for (long i = 1; i <= ncw; i++) {
		real80 di = 0.0;
		for (long k = 1; k <= ncw; k ++)
			for (long l = 1; l <= nrw; l ++) {
				di += w [k] [i] * c [k] [l] * w [l] [i];
			}
		di = 1.0 / sqrt (di);
		for (long j = 1; j <= nrw; j ++) {
			w[j][i] *= di;
		}
	}
}

static double NUMdmatrix_diagonalityMeasure (double **v, long dimension) {
	real80 dmsq = 0;
	if (dimension < 2) {
		return 0.0;
	}
	for (long i = 1; i <= dimension; i ++) {
		for (long j = 1; j <= dimension; j ++) {
			if (i != j) {
				dmsq += v [i] [j] * v [i] [j];
			}
		}
	}
	return dmsq / (dimension * (dimension - 1));
}

#if 0
static double NUMdmatrix_diagonalityIndex (double **v, long dimension) {
	double dindex = 0;
	for (long irow = 1; irow <= dimension; irow++) {
		double rowmax = fabs (v[irow][1]), rowsum = 0;
		for (long icol = 2; icol <= dimension; icol++) {
			if (fabs (v[irow][icol]) > rowmax) {
				rowmax = fabs (v[irow][icol]);
			}
		}
		for (long icol = 1; icol <= dimension; icol++) {
			rowsum += fabs (v[irow][icol]) / rowmax;
		}
		dindex += rowsum - 1;
	}
	for (long icol = 1; icol <= dimension; icol++) {
		double colmax = fabs (v[icol][1]), colsum = 0;
		for (long irow = 2; irow <= dimension; irow++) {
			if (fabs (v[irow][icol]) > colmax) {
				colmax = fabs (v[irow][icol]);
			}
		}
		for (long irow = 1; irow <= dimension; irow++) {
			colsum += fabs (v[irow][icol]) / colmax;
		}
		dindex += colsum - 1;
	}
	return dindex;
}
#endif

/*
	This routine is modeled after qdiag.m from Andreas Ziehe, Pavel Laskov, Guido Nolte, Klaus-Robert Müller,
	A Fast Algorithm for Joint Diagonalization with Non-orthogonal Transformations and its Application to
	Blind Source Separation, Journal of Machine Learning Research 5 (2004), 777–800.
*/
static void Diagonalizer_and_CrossCorrelationTableList_ffdiag (Diagonalizer me, CrossCorrelationTableList thee, long maxNumberOfIterations, double delta) {
	try {
		long iter = 0, dimension = my numberOfRows;
		double **v = my data;

		autoCrossCorrelationTableList ccts = CrossCorrelationTableList_and_Diagonalizer_diagonalize (thee, me);
		autoNUMmatrix<double> w (1, dimension, 1, dimension);
		autoNUMmatrix<double> vnew (1, dimension, 1, dimension);
		autoNUMmatrix<double> cc (1, dimension, 1, dimension);

		for (long i = 1; i <= dimension; i ++) {
			w [i] [i] = 1.0;
		}

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		real80 dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
		try {
			real80 dm_old, theta = 1.0, dm_start = dm_new;
			do {
				dm_old = dm_new;
				for (long i = 1; i <= dimension; i ++) {
					for (long j = i + 1; j <= dimension; j ++) {
						real80 zii = 0.0, zij = 0.0, zjj = 0.0, yij = 0.0, yji = 0.0;   // zij == zji
						for (long k = 1; k <= ccts -> size; k ++) {
							CrossCorrelationTable ct = ccts -> at [k];
							zii += ct -> data [i] [i] * ct -> data [i] [i];
							zij += ct -> data [i] [i] * ct -> data [j] [j];
							zjj += ct -> data [j] [j] * ct -> data [j] [j];
							yij += ct -> data [j] [j] * ct -> data [i] [j];
							yji += ct -> data [i] [i] * ct -> data [i] [j];
						}
						real80 denom = zjj * zii - zij * zij;
						if (denom != 0.0) {
							w [i][j] = (zij * yji - zii * yij) / denom;
							w [j][i] = (zij * yij - zjj * yji) / denom;
						}
					}
				}
				real80 norma = 0.0;
				for (long i = 1; i <= dimension; i ++) {
					real80 normai = 0.0;
					for (long j = 1; j <= dimension; j ++) {
						if (i != j) {
							normai += fabs (w [i] [j]);
						}
					}
					if (normai > norma) {
						norma = normai;
					}
				}
				// evaluate the norm
				if (norma > theta) {
					real80 normf = 0.0;
					for (long i = 1; i <= dimension; i ++)
						for (long j = 1; j <= dimension; j ++)
							if (i != j) {
								normf += w [i] [j] * w [i] [j];
							}
					real80 scalef = theta / sqrt (normf);
					for (long i = 1; i <= dimension; i ++) {
						for (long j = 1; j <= dimension; j ++) {
							if (i != j) {
								w [i] [j] *= scalef;
							}
						}
					}
				}
				// update V
				NUMmatrix_copyElements (v, vnew.peek(), 1, dimension, 1, dimension);
				NUMdmatrices_multiply_VC (v, w.peek(), dimension, dimension, vnew.peek(), dimension);
				for (long k = 1; k <= ccts -> size; k ++) {
					CrossCorrelationTable ct = ccts -> at [k];
					NUMmatrix_copyElements (ct -> data, cc.peek(), 1, dimension, 1, dimension);
					NUMdmatrices_multiply_VCVp (ct -> data, w.peek(), dimension, dimension, cc.peek(), true);
				}
				dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
				iter++;
				Melder_progress ((double) iter / (double) maxNumberOfIterations, U"Iteration: ", iter, U", measure: ", (double) dm_new, U"\n fractional measure: ", (double)(dm_new / dm_start));
			} while (fabs ((dm_old - dm_new) / dm_new) > delta && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no joint diagonalization (ffdiag).");
	}
}

/*
	The folowing two routines are modeled after qdiag.m from
	R. Vollgraf and K. Obermayer, Quadratic Optimization for Simultaneous
	Matrix Diagonalization, IEEE Transaction on Signal Processing, 2006,
*/
static void update_one_column (CrossCorrelationTableList me, double **d, double *wp, double *wvec, double scalef, double *work) {
	long dimension = my at [1] -> numberOfColumns;

	for (long ic = 2; ic <= my size; ic ++) { // exclude C0
		SSCP cov = my at [ic];
		double **c = cov -> data;
		// m1 = C * wvec
		for (long i = 1; i <= dimension; i++) {
			double r = 0;
			for (long j = 1; j <= dimension; j++) {
				r += c[i][j] * wvec[j];
			}
			work[i] = r;
		}
		// D = D +/- 2*p(t)*(m1*m1');
		for (long i = 1; i <= dimension; i++) {
			for (long j = 1; j <= dimension; j++) {
				d[i][j] += 2 * scalef * wp[ic] * work[i] * work[j];
			}
		}
	}
}

static void Diagonalizer_and_CrossCorrelationTable_qdiag (Diagonalizer me, CrossCorrelationTableList thee, double *cweights, long maxNumberOfIterations, double delta) {
	try {
		CrossCorrelationTable c0 = thy at [1];
		double **w = my data;
		long dimension = c0 -> numberOfColumns;

		autoEigen eigen = Thing_new (Eigen);
		autoCrossCorrelationTableList ccts = Data_copy (thee);
		autoNUMmatrix<double> pinv (1, dimension, 1, dimension);
		autoNUMmatrix<double> d (1, dimension, 1, dimension);
		autoNUMmatrix<double> p (1, dimension, 1, dimension);
		autoNUMmatrix<double> m1 (1, dimension, 1, dimension);
		autoNUMmatrix<double> wc (1, dimension, 1, dimension);
		autoNUMvector<double> wvec (1, dimension);
		autoNUMvector<double> wnew (1, dimension);
		autoNUMvector<double> mvec (1, dimension);

		for (long i = 1; i <= dimension; i ++) // Transpose W
			for (long j = 1; j <= dimension; j ++) {
				wc [i] [j] = w [j] [i];
			}

		// d = diag(diag(W'*C0*W));
		// W = W*d^(-1/2);

		NUMdmatrix_normalizeColumnVectors (wc.peek(), dimension, dimension, c0 -> data);

		// scale eigenvectors for sphering
		// [vb,db] = eig(C0);
		// P = db^(-1/2)*vb';

		Eigen_initFromSymmetricMatrix (eigen.get(), c0 -> data, dimension);
		for (long i = 1; i <= dimension; i ++) {
			if (eigen -> eigenvalues[i] < 0) {
				Melder_throw (U"Covariance matrix not positive definite, eigenvalue[", i, U"] is negative.");
			}
			double scalef = 1.0 / sqrt (eigen -> eigenvalues [i]);
			for (long j = 1; j <= dimension; j ++) {
				p [dimension - i + 1] [j] = scalef * eigen -> eigenvectors [i] [j];
			}
		}

		// P*C[i]*P'

		for (long ic = 1; ic <= thy size; ic ++) {
			CrossCorrelationTable cov1 = thy at [ic];
			CrossCorrelationTable cov2 = ccts -> at [ic];
			NUMdmatrices_multiply_VCVp (cov2 -> data, p.peek(), dimension, dimension, cov1 -> data, true);
		}

		// W = P'\W == inv(P') * W

		NUMpseudoInverse (p.peek(), dimension, dimension, pinv.peek(), 0);

		NUMdmatrices_multiply_VpC (w, pinv.peek(), dimension, dimension, wc.peek(), dimension);

		// initialisation for order KN^3

		for (long ic = 2; ic <= thy size; ic ++) {
			CrossCorrelationTable cov = ccts -> at [ic];
			// C * W
			NUMdmatrices_multiply_VC (m1.peek(), cov -> data, dimension, dimension, w, dimension);
			// D += scalef * M1*M1'
			NUMdmatrices_multiplyScaleAdd (d.peek(), m1.peek(), dimension, dimension, 2 * cweights [ic]);
		}

		long iter = 0;
		double delta_w;

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		try {
			do {
				// the standard diagonality measure is rather expensive to calculate so we compare the norms of
				// differences of eigenvectors.

				delta_w = 0;
				for (long kol = 1; kol <= dimension; kol ++) {
					for (long i = 1; i <= dimension; i ++) {
						wvec [i] = w [i] [kol];
					}

					update_one_column (ccts.get(), d.peek(), cweights, wvec.peek(), -1, mvec.peek());

					Eigen_initFromSymmetricMatrix (eigen.get(), d.peek(), dimension);

					// Eigenvalues already sorted; get eigenvector of smallest !

					for (long i = 1; i <= dimension; i ++) {
						wnew [i] = eigen -> eigenvectors [dimension] [i];
					}

					update_one_column (ccts.get(), d.peek(), cweights, wnew.peek(), 1, mvec.peek());
					for (long i = 1; i <= dimension; i ++) {
						w [i] [kol] = wnew [i];
					}

					// compare norms of eigenvectors. We have to compare ||wvec +/- w_new|| because eigenvectors
					//  may change sign.

					double normp = 0, normm = 0;
					for (long j = 1; j <= dimension; j ++) {
						double dm = wvec [j] - wnew [j], dp = wvec [j] + wnew [j];
						normp += dm * dm; 
						normm += dp * dp;
					}

					normp = normp < normm ? normp : normm;
					normp = sqrt (normp);
					delta_w = normp > delta_w ? normp : delta_w;
				}
				iter ++;

				Melder_progress ((double) iter / (double) (maxNumberOfIterations + 1), U"Iteration: ", iter, U", norm: ", delta_w);
			} while (delta_w > delta && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}

		// Revert the sphering W = P'*W;
		// Take transpose to make W*C[i]W' diagonal instead of W'*C[i]*W => (P'*W)'=W'*P

		NUMmatrix_copyElements (w, wc.peek(), 1, dimension, 1, dimension);
		NUMdmatrices_multiply_VpC (w, wc.peek(), dimension, dimension, p.peek(), dimension); // W = W'*P: final result

		// Calculate the "real" diagonality measure
	//	double dm = CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (thee, me, cweights, 1, thy size);

	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no joint diagonalization (qdiag).");
	}
}

void MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (MixingMatrix me, CrossCorrelationTableList thee, long maxNumberOfIterations, double tol, int method) {
	autoDiagonalizer him = MixingMatrix_to_Diagonalizer (me);
	Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (him.get(), thee, maxNumberOfIterations, tol, method);
	NUMpseudoInverse (his data, his numberOfRows, his numberOfColumns, my data, 0);
}


/* Preconditions:
 * 	x[1..nrows][1..ncols], cc[1..nrows][1..nrows], centroid[1..nrows]
 * 	if (lag>0) {i2 + lag <= ncols} else {i1-lag >= 1}
 * 	no array boundary checks!
 * 	lag >= 0
 */
static void NUMcrossCorrelate_rows (double **x, long nrows, long icol1, long icol2, long lag, double **cc, double *centroid, double scale) {
	lag = labs (lag);
	long nsamples = icol2 - icol1 + 1 + lag;
	for (long i = 1; i <= nrows; i ++) {
		double sum = 0.0;
		for (long k = icol1; k <= icol2 + lag; k ++) {
			sum += x [i] [k];
		}
		centroid[i] = sum / nsamples;
	}
	for (long i = 1; i <= nrows; i ++) {
		for (long j = i; j <= nrows; j ++) {
			double ccor = 0;
			for (long k = icol1; k <= icol2; k ++) {
				ccor += (x [i] [k] - centroid [i]) * (x [j] [k + lag] - centroid [j]);
			}
			cc [j] [i] = cc [i] [j] = ccor * scale;
		}
	}
}

/*
	This is for multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples, (z[i][k] - mean[i])(z[j][k + tau] - mean[j]))*samplingTime
*/
autoCrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me, double startTime, double endTime, double lagStep) {
	try {
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		integer lag = Melder_iroundDown (lagStep / my dx);   // ppgb: voor al dit soort dingen geldt: waarom afronden naar beneden?
		long i1 = Sampled_xToNearestIndex (me, startTime);
		if (i1 < 1) {
			i1 = 1;
		}
		long i2 = Sampled_xToNearestIndex (me, endTime);
		if (i2 > my nx) {
			i2 = my nx;
		}
		i2 -= lag;
		long nsamples = i2 - i1 + 1;
		if (nsamples <= my ny) {
			Melder_throw (U"Not enough samples, choose a longer interval.");
		}
		autoCrossCorrelationTable thee = CrossCorrelationTable_create (my ny);

		NUMcrossCorrelate_rows (my z, my ny, i1, i2, lag, thy data, thy centroid, my dx);

		thy numberOfObservations = nsamples;

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": CrossCorrelationTable not created.");
	}
}

/* Calculate the CrossCorrelationTable between the channels of two multichannel sounds irrespective of the domains.
 * Both sounds are treated as if their domain runs from 0 to duration.
 * Outside the chosen interval the sounds are assumed to be zero
 */
autoCrossCorrelationTable Sounds_to_CrossCorrelationTable_combined (Sound me, Sound thee, double relativeStartTime, double relativeEndTime, double lagStep) {
	try {
		if (my dx != thy dx) {
			Melder_throw (U"Sampling frequencies must be equal.");
		}
		if (relativeEndTime <= relativeStartTime) {
			relativeStartTime = my xmin;
			relativeEndTime = my xmax;
		}
		integer ndelta = Melder_iroundDown (lagStep / my dx), nchannels = my ny + thy ny;
		long i1 = Sampled_xToNearestIndex (me, relativeStartTime);
		if (i1 < 1) {
			i1 = 1;
		}
		long i2 = Sampled_xToNearestIndex (me, relativeEndTime);
		if (i2 > my nx) {
			i2 = my nx;
		}
		i2 -= ndelta;
		long nsamples = i2 - i1 + 1;
		if (nsamples <= nchannels) {
			Melder_throw (U"Not enough samples");
		}
		autoCrossCorrelationTable him = CrossCorrelationTable_create (nchannels);
		autoNUMvector<double *> data (1, nchannels);
		for (long i = 1; i <= my ny; i ++) {
			data [i] = my z [i];
		}
		for (long i = 1; i <= thy ny; i ++) {
			data [i + my ny] = thy z [i];
		}

		NUMcrossCorrelate_rows (data.peek(), nchannels, i1, i2, ndelta, his data, his centroid, my dx);

		his numberOfObservations = nsamples;

		return him;
	} catch (MelderError) {
		Melder_throw (me, U": CrossCorrelationTable not created.");
	}
}

autoCovariance Sound_to_Covariance_channels (Sound me, double startTime, double endTime) {
    try {
        double lagStep = 0.0;
        autoCrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, startTime, endTime, lagStep);
        autoCovariance him = Thing_new (Covariance);
        thy structCrossCorrelationTable :: v_copy (him.get());
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": no Covariance created.");
    }
}

autoCrossCorrelationTableList Sound_to_CrossCorrelationTableList (Sound me, double startTime, double endTime, double lagStep, long ncovars) {
	try {
		if (lagStep < my dx) {
			lagStep = my dx;
		}
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		if (startTime + ncovars * lagStep >= endTime) {
			Melder_throw (U"Lag time too large.");
		}
		autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
		for (long i = 1; i <= ncovars; i ++) {
			double lag = (i - 1) * lagStep;
			autoCrossCorrelationTable ct = Sound_to_CrossCorrelationTable (me, startTime, endTime, lag);
			thy addItem_move (ct.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTableList created.");
	}
}

autoSound Sound_to_Sound_BSS (Sound me, double startTime, double endTime, long ncovars, double lagStep, long maxNumberOfIterations, double tol, int method) {
	try {
		autoMixingMatrix him = Sound_to_MixingMatrix (me, startTime, endTime, ncovars, lagStep, maxNumberOfIterations, tol, method);
		autoSound thee = Sound_and_MixingMatrix_unmix (me, him.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not separated.");
	}
}

/************************ Diagonalizer **********************************/

Thing_implement (Diagonalizer, TableOfReal, 0);

autoDiagonalizer Diagonalizer_create (long dimension) {
	try {
		autoDiagonalizer me = Thing_new (Diagonalizer);
		TableOfReal_init (me.get(), dimension, dimension);
		for (long i = 1; i <= dimension; i ++) {
			my data [i] [i] = 1.0;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Diagonalizer not created.");
	}
}

/***************** Diagonalizer & MixingMatrix *************************/


autoDiagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me) {
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw (U"The number of channels and the number of components must be equal.");
		}
		autoDiagonalizer thee = Diagonalizer_create (my numberOfRows);
		NUMpseudoInverse (my data, my numberOfRows, my numberOfColumns, thy data, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Diagonalizer created.");
	}
}

autoMixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me) {
	try {
		autoMixingMatrix thee = MixingMatrix_create (my numberOfRows, my numberOfColumns);
		MixingMatrix_setRandomGauss ( thee.get(), 0.0, 1.0);
		NUMpseudoInverse (my data, my numberOfRows, my numberOfColumns, thy data, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MixingMatrix created.");
	}
}

void MixingMatrix_and_Sound_improveUnmixing (MixingMatrix me, Sound thee, double startTime, double endTime, long ncovars, double lagStep, long maxNumberOfIterations, double tol, int method) {
	autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (thee, startTime, endTime, lagStep, ncovars);
	MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (me, ccs.get(), maxNumberOfIterations, tol, method);
}

autoMixingMatrix Sound_to_MixingMatrix (Sound me, double startTime, double endTime, long ncovars, double lagStep, long maxNumberOfIterations, double tol, int method) {
	try {
		autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (me, startTime, endTime, lagStep, ncovars);
		autoMixingMatrix thee = MixingMatrix_create (my ny, my ny);
		MixingMatrix_setRandomGauss (thee.get(), 0.0, 1.0);
		MixingMatrix_and_CrossCorrelationTableList_improveUnmixing (thee.get(), ccs.get(), maxNumberOfIterations, tol, method);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MixingMatrix created.");
	}
}

autoMixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me) {
	try {
		if (my numberOfColumns != my numberOfRows) {
			Melder_throw (U"Number of rows and columns must be equal.");
		}
		autoMixingMatrix thee = Thing_new (MixingMatrix);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to MixingMatrix.");
	}
}

/************* CrossCorrelationTable *****************************/

Thing_implement (CrossCorrelationTable, SSCP, 0);

void structCrossCorrelationTable :: v_info () {
	structSSCP :: v_info ();
	double dm = CrossCorrelationTable_getDiagonalityMeasure (this);
	MelderInfo_writeLine (U"Diagonality measure: ", dm);
}

autoCrossCorrelationTable CrossCorrelationTable_create (long dimension) {
	try {
		autoCrossCorrelationTable me = Thing_new (CrossCorrelationTable);
		SSCP_init (me.get(), dimension, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not created.");
	}
}

autoCrossCorrelationTable CrossCorrelationTable_createSimple (char32 *covars, char32 *centroid, long numberOfSamples) {
	try {
		long dimension = Melder_countTokens (centroid);
		long ncovars = Melder_countTokens (covars);
		long ncovars_wanted = dimension * (dimension + 1) / 2;
		if (ncovars != ncovars_wanted) Melder_throw (U"The number of matrix elements and the number of "
			U"centroid elements are not in concordance. There should be \"d(d+1)/2\" matrix values and \"d\" centroid values.");

		autoCrossCorrelationTable me = CrossCorrelationTable_create (dimension);

		/*
			Construct the full matrix from the upper-diagonal elements
		*/

		long inum = 1, irow = 1;
		for (char32 *token = Melder_firstToken (covars); token != nullptr && inum <= ncovars_wanted; token = Melder_nextToken (), inum ++) {
			double number;
			long nmissing = (irow - 1) * irow / 2;
			long inumc = inum + nmissing;
			irow = (inumc - 1) / dimension + 1;
			long icol = ( (inumc - 1) % dimension) + 1;
			Interpreter_numericExpression (nullptr, token, &number);
			my data [irow] [icol] = my data [icol] [irow] = number;
			if (icol == dimension) {
				irow ++;
			}
		}

		inum = 1;
		for (char32 *token = Melder_firstToken (centroid); token != nullptr && inum <= dimension; token = Melder_nextToken (), inum++) {
			double number;
			Interpreter_numericExpression (nullptr, token, & number);
			my centroid [inum] = number;
		}
		my numberOfObservations = numberOfSamples;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not created.");
	}
}

double CrossCorrelationTable_getDiagonalityMeasure (CrossCorrelationTable me) {
	return NUMdmatrix_diagonalityMeasure (my data, my numberOfColumns);
}

/************* CrossCorrelationTables *****************************/

void structCrossCorrelationTableList :: v_info () {
	our structThing :: v_info ();
	MelderInfo_writeLine (U"Contains ", our size, U" CrossCorrelationTable objects");
	CrossCorrelationTable thee = our at [1];
	MelderInfo_writeLine (U"Number of rows and columns: ", thy numberOfRows, U" in each CrossCorrelationTable");
	for (long i = 1; i <= our size; i ++) {
		double dm = CrossCorrelationTable_getDiagonalityMeasure (our at [i]);
		MelderInfo_writeLine (U"  Diagonality measure for item ", i, U": ", dm);
	}
}

autoCrossCorrelationTableList CrossCorrelationTables_to_CrossCorrelationTableList (OrderedOf<structCrossCorrelationTable> *me) {
	try {
		autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
		long numberOfRows = 0, numberOfColumns = 0, numberOfSelected = 0;
		for (long i = 1; i <= my size; i ++) {
			CrossCorrelationTable item = my at [i];
			numberOfSelected++;
			if (numberOfSelected == 1) {
				numberOfRows = item -> numberOfRows;
				numberOfColumns = item -> numberOfColumns;
			}
			if (item -> numberOfRows != numberOfRows || item -> numberOfColumns != numberOfColumns) {
				Melder_throw (U"Dimensions of table ", i, U" differs from the rest.");
			}
			autoCrossCorrelationTable myc = Data_copy (item);
			thy addItem_move (myc.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No CrossCorrelationTableList created from CrossCorrelationTable(s)");
	}
}

Thing_implement (CrossCorrelationTableList, SSCPList, 0);

double CrossCorrelationTableList_getDiagonalityMeasure (CrossCorrelationTableList me, double *w, long start, long end) {
	if (start >= end) {
		start = 1;
		end = my size;
	}
	if (start < 1) {
		start = 1;
	}
	if (end > my size) {
		end = my size;
	}
	long ntables = end - start + 1;
	long dimension = my at [1] -> numberOfColumns;
	double dmsq = 0;
	for (long k = start; k <= end; k ++) {
		CrossCorrelationTable thee = my at [k];
		double dmksq = NUMdmatrix_diagonalityMeasure (thy data, dimension);
		dmsq += (w ? dmksq * w [k] : dmksq / ntables);
	}
	return dmsq;
}

/************************** CrossCorrelationTables & Diagonalizer *******************************/

double CrossCorrelationTableList_and_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTableList me, Diagonalizer thee, double *w, long start, long end) {
	autoCrossCorrelationTableList him = CrossCorrelationTableList_and_Diagonalizer_diagonalize (me, thee);
	double dm = CrossCorrelationTableList_getDiagonalityMeasure (him.get(), w, start, end);
	return dm;
}

autoCrossCorrelationTable CrossCorrelationTable_and_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee) {
	try {
		if (my numberOfRows != thy numberOfRows) {
			Melder_throw (U"The CrossCorrelationTable and the Diagonalizer matrix dimensions must be equal.");
		}
		autoCrossCorrelationTable him = CrossCorrelationTable_create (my numberOfColumns);
		NUMdmatrices_multiply_VCVp (his data, thy data, my numberOfColumns, my numberOfColumns, my data, true);
		return him;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not diagonalized.");
	}
}

autoCrossCorrelationTableList CrossCorrelationTableList_and_Diagonalizer_diagonalize (CrossCorrelationTableList me, Diagonalizer thee) {
	try {
		autoCrossCorrelationTableList him = CrossCorrelationTableList_create ();
		for (long i = 1; i <= my size; i ++) {
			CrossCorrelationTable item = my at [i];
			autoCrossCorrelationTable ct = CrossCorrelationTable_and_Diagonalizer_diagonalize (item, thee);
			his addItem_move (ct.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTableList not diagonalized.");
	}
}

autoDiagonalizer CrossCorrelationTableList_to_Diagonalizer (CrossCorrelationTableList me, long maxNumberOfIterations, double tol, int method) {
	try {
		Melder_assert (my size > 0);
		CrossCorrelationTable him = my at [1];
		autoDiagonalizer thee = Diagonalizer_create (his numberOfColumns);
		Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (thee.get(), me, maxNumberOfIterations, tol, method);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Diagonalizer not created from CrossCorrelationTableList.");
	};
}

void Diagonalizer_and_CrossCorrelationTableList_improveDiagonality (Diagonalizer me, CrossCorrelationTableList thee, long maxNumberOfIterations, double tol, int method) {
	if (method == 1) {
		autoNUMvector<double> cweights (1, thy size);
		for (long i = 1; i <= thy size; i ++) {
			cweights [i] = 1.0 / thy size;
		}
		Diagonalizer_and_CrossCorrelationTable_qdiag (me, thee, cweights.peek(), maxNumberOfIterations, tol);
	} else {
		Diagonalizer_and_CrossCorrelationTableList_ffdiag (me, thee, maxNumberOfIterations, tol);
	}
}

autoSound Sound_whitenChannels (Sound me, double varianceFraction) {
    try {
        autoCovariance cov = Sound_to_Covariance_channels (me, 0.0, 0.0);
        autoSound thee = Sound_and_Covariance_whitenChannels (me, cov.get(), varianceFraction);
        return thee;
    } catch (MelderError) {
        Melder_throw (me, U": not whitened.");
    }
}

autoSound Sound_and_Covariance_whitenChannels (Sound me, Covariance thee, double varianceFraction) {
    try {
        autoPCA pca = SSCP_to_PCA (thee);
        long numberOfComponents = Eigen_getDimensionOfFraction (pca.get(), varianceFraction);
        autoSound him = Sound_and_PCA_whitenChannels (me, pca.get(), numberOfComponents);
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": not whitened from ", thee);
    }
}

/*
 * Generate n different cct's that have a common diagonalizer.
 */
autoCrossCorrelationTableList CrossCorrelationTableList_createTestSet (long dimension, long n, int firstPositiveDefinite, double sigma) {
	try {
		/*
			Start with a square matrix with random gaussian elements and make its singular value decomposition UDV'
			The V matrix will be the common diagonalizer matrix that we use.
		*/

		autoNUMmatrix<double> d (1, dimension, 1, dimension);
		for (long i = 1; i <= dimension; i ++) { // Generate the rotation matrix
			for (long j = 1; j <= dimension; j ++) {
				d [i] [j] = NUMrandomGauss (0.0, 1.0);
			}
		}
		autoNUMmatrix<double> v (1, dimension, 1, dimension);
		autoSVD svd = SVD_create_d (d.peek(), dimension, dimension);
		autoCrossCorrelationTableList me = CrossCorrelationTableList_create ();

		for (long i = 1; i <= dimension; i ++) {
			for (long j = 1; j <= dimension; j ++) {
				d [i] [j] = 0.0;
			}
		}

		// Start with a diagonal matrix D and calculate V'DV

		for (long k = 1; k <= n; k ++) {
			autoCrossCorrelationTable ct = CrossCorrelationTable_create (dimension);
			double low = (k == 1 && firstPositiveDefinite ? 0.1 : -1.0);
			for (long i = 1; i <= dimension; i ++) {
				d [i] [i] = NUMrandomUniform (low, 1.0);
			}
			for (long i = 1; i <= dimension; i++) {
				for (long j = 1; j <= dimension; j ++) {
					v [i] [j] = NUMrandomGauss (svd -> v [i] [j], sigma);
				}
			}
			// we need V'DV, however our V has eigenvectors row-wise -> VDV'
			NUMdmatrices_multiply_VCVp (ct -> data, v.peek(), dimension, dimension, d.peek(), true);
            my addItem_move (ct.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTableList test set not created.");
	}
}

#if 0
static void Sound_and_MixingMatrix_improveUnmixing_fica (Sound me, MixingMatrix thee, long maxNumberOfIterations, double /* tol */, int /* method */) {
	try {
		long iter = 0;
		if (my ny != thy numberOfColumns) {
			Melder_throw (U"Dimensions do not agree.");
		}
		autoNUMmatrix<double> x (NUMmatrix_copy (my z, 1, my ny, 1, my nx), 1, 1);
		do {
			iter ++;
		} while (/*fabs((dm_old - dm_new) / dm_new) > tol &&*/ iter < maxNumberOfIterations);
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U" .");
	}
}
#endif

/* End of file ICA.cpp */
