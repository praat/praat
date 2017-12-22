/* ICA.cpp
 *
 * Copyright (C) 2010-2017 David Weenink
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
static void NUMdmatrices_multiply_VCVp (double **r, double **v, integer nrv, integer ncv, double **c, bool csym) {
	for (integer i = 1; i <= nrv; i ++) {
		integer jstart = csym ? i : 1;
		for (integer j = jstart; j <= nrv; j ++) {
			// V_ik C_kl V'_lj = V_ik C_kl V_jl
			double vcv = 0.0;
			for (integer k = 1; k <= ncv; k ++) {
				for (integer l = 1; l <= ncv; l ++) {
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
static void NUMdmatrices_multiply_VpCV (double **r, double **v, integer nrv, integer ncv, double **c, int csym) {
	for (integer i = 1; i <= ncv; i ++) {
		integer jstart = csym ? i : 1;
		for (integer j = jstart; j <= ncv; j ++) {
			// V'_ik C_kl V_lj = V_ki C_kl V_lj
			double vcv = 0;
			for (integer k = 1; k <= nrv; k ++) {
				for (integer l = 1; l <= ncv; l ++) {
					vcv += v [k] [i] * c [k] [l] * v [l] [j];
				}
			}
			r [i] [j] = vcv;
			if (csym) {
				r [j] [i] = vcv;
			}
		}
	}
}
#endif

// matrix multiply V*C, V is nrv x ncv, C is ncv x ncc, R is nrv x ncc;
static void NUMdmatrices_multiply_VC (double **r, double **v, integer nrv, integer ncv, double **c, integer ncc) {
	for (integer i = 1; i <= nrv; i ++) {
		for (integer j = 1; j <= ncc; j ++) {
			// V_ik C_kj
			real80 vc = 0.0;
			for (integer k = 1; k <= ncv; k ++) {
				vc += v [i] [k] * c [k] [j];
			}
			r [i] [j] = (double) vc;
		}
	}
}

// matrix multiply V'*C, V is nrv x ncv, C is nrv x ncc, R is ncv x ncc;
static void NUMdmatrices_multiply_VpC (double **r, double **v, integer nrv, integer ncv, double **c, integer ncc) {
	for (integer i = 1; i <= ncv; i ++) {
		for (integer j = 1; j <= ncc; j ++) {
			// V'_ik C_kj
			real80 vc = 0.0;
			for (integer k = 1; k <= nrv; k ++) {
				vc += v [k] [i] * c [k] [j];
			}
			r [i] [j] = (double) vc;
		}
	}
}

// D += scalef * M * M', M = nrm x ncm, D is nrm x nrm
static void NUMdmatrices_multiplyScaleAdd (double **r, double **m, integer nrm, integer ncm, double scalef) {
	for (integer i = 1; i <= nrm; i ++) {
		for (integer j = 1; j <= nrm; j ++) {
			// M_ik M'_kj = M_ik M_jk
			real80 mm = 0.0;
			for (integer k = 1; k <= ncm; k ++) {
				mm += m [i] [k] * m [j] [k];
			}
			r [i] [j] += scalef * mm;
		}
	}
}

/*
	d = diag(diag(W'*C0*W));
	W = W*d^(-1/2);

	D_ij = W'_ik C_kl W_lj => D_ii = W_ki C_kl W_li
*/
static void NUMdmatrix_normalizeColumnVectors (double **w, integer nrw, integer ncw, double **c) {
	for (integer i = 1; i <= ncw; i ++) {
		real80 di = 0.0;
		for (integer k = 1; k <= ncw; k ++)
			for (integer l = 1; l <= nrw; l ++) {
				di += w [k] [i] * c [k] [l] * w [l] [i];
			}
		di = 1.0 / sqrt (di);
		for (integer j = 1; j <= nrw; j ++) {
			w [j] [i] *= di;
		}
	}
}

static double NUMdmatrix_diagonalityMeasure (double **v, integer dimension) {
	real80 dmsq = 0;
	if (dimension < 2) {
		return 0.0;
	}
	for (integer i = 1; i <= dimension; i ++) {
		for (integer j = 1; j <= dimension; j ++) {
			if (i != j) {
				dmsq += v [i] [j] * v [i] [j];
			}
		}
	}
	return dmsq / (dimension * (dimension - 1));
}

#if 0
static double NUMdmatrix_diagonalityIndex (double **v, integer dimension) {
	double dindex = 0;
	for (integer irow = 1; irow <= dimension; irow ++) {
		double rowmax = fabs (v [irow] [1]), rowsum = 0;
		for (integer icol = 2; icol <= dimension; icol ++) {
			if (fabs (v [irow] [icol]) > rowmax) {
				rowmax = fabs (v [irow] [icol]);
			}
		}
		for (integer icol = 1; icol <= dimension; icol ++) {
			rowsum += fabs (v [irow] [icol]) / rowmax;
		}
		dindex += rowsum - 1;
	}
	for (integer icol = 1; icol <= dimension; icol ++) {
		double colmax = fabs (v [icol] [1]), colsum = 0;
		for (integer irow = 2; irow <= dimension; irow ++) {
			if (fabs (v [irow] [icol]) > colmax) {
				colmax = fabs (v [irow] [icol]);
			}
		}
		for (integer irow = 1; irow <= dimension; irow ++) {
			colsum += fabs (v [irow] [icol]) / colmax;
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
static void Diagonalizer_CrossCorrelationTableList_ffdiag (Diagonalizer me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double delta) {
	try {
		integer iter = 0, dimension = my numberOfRows;
		double **v = my data;

		autoCrossCorrelationTableList ccts = CrossCorrelationTableList_Diagonalizer_diagonalize (thee, me);
		autoNUMmatrix<double> w (1, dimension, 1, dimension);
		autoNUMmatrix<double> vnew (1, dimension, 1, dimension);
		autoNUMmatrix<double> cc (1, dimension, 1, dimension);

		for (integer i = 1; i <= dimension; i ++) {
			w [i] [i] = 1.0;
		}

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		real80 dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
		try {
			real80 dm_old, theta = 1.0, dm_start = dm_new;
			do {
				dm_old = dm_new;
				for (integer i = 1; i <= dimension; i ++) {
					for (integer j = i + 1; j <= dimension; j ++) {
						real80 zii = 0.0, zij = 0.0, zjj = 0.0, yij = 0.0, yji = 0.0;   // zij == zji
						for (integer k = 1; k <= ccts -> size; k ++) {
							CrossCorrelationTable ct = ccts -> at [k];
							zii += ct -> data [i] [i] * ct -> data [i] [i];
							zij += ct -> data [i] [i] * ct -> data [j] [j];
							zjj += ct -> data [j] [j] * ct -> data [j] [j];
							yij += ct -> data [j] [j] * ct -> data [i] [j];
							yji += ct -> data [i] [i] * ct -> data [i] [j];
						}
						real80 denom = zjj * zii - zij * zij;
						if (denom != 0.0) {
							w [i] [j] = (zij * yji - zii * yij) / denom;
							w [j] [i] = (zij * yij - zjj * yji) / denom;
						}
					}
				}
				real80 norma = 0.0;
				for (integer i = 1; i <= dimension; i ++) {
					real80 normai = 0.0;
					for (integer j = 1; j <= dimension; j ++) {
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
					for (integer i = 1; i <= dimension; i ++)
						for (integer j = 1; j <= dimension; j ++)
							if (i != j) {
								normf += w [i] [j] * w [i] [j];
							}
					real80 scalef = theta / sqrt (normf);
					for (integer i = 1; i <= dimension; i ++) {
						for (integer j = 1; j <= dimension; j ++) {
							if (i != j) {
								w [i] [j] *= scalef;
							}
						}
					}
				}
				// update V
				NUMmatrix_copyElements (v, vnew.peek(), 1, dimension, 1, dimension);
				NUMdmatrices_multiply_VC (v, w.peek(), dimension, dimension, vnew.peek(), dimension);
				for (integer k = 1; k <= ccts -> size; k ++) {
					CrossCorrelationTable ct = ccts -> at [k];
					NUMmatrix_copyElements (ct -> data, cc.peek(), 1, dimension, 1, dimension);
					NUMdmatrices_multiply_VCVp (ct -> data, w.peek(), dimension, dimension, cc.peek(), true);
				}
				dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
				iter ++;
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
	integer dimension = my at [1] -> numberOfColumns;

	for (integer ic = 2; ic <= my size; ic ++) { // exclude C0
		SSCP cov = my at [ic];
		double **c = cov -> data;
		// m1 = C * wvec
		for (integer i = 1; i <= dimension; i ++) {
			double r = 0;
			for (integer j = 1; j <= dimension; j ++) {
				r += c [i] [j] * wvec [j];
			}
			work [i] = r;
		}
		// D = D +/- 2*p(t)*(m1*m1');
		for (integer i = 1; i <= dimension; i ++) {
			for (integer j = 1; j <= dimension; j ++) {
				d [i] [j] += 2 * scalef * wp [ic] * work [i] * work [j];
			}
		}
	}
}

static void Diagonalizer_CrossCorrelationTable_qdiag (Diagonalizer me, CrossCorrelationTableList thee, double *cweights, integer maxNumberOfIterations, double delta) {
	try {
		CrossCorrelationTable c0 = thy at [1];
		double **w = my data;
		integer dimension = c0 -> numberOfColumns;

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

		for (integer i = 1; i <= dimension; i ++) // Transpose W
			for (integer j = 1; j <= dimension; j ++) {
				wc [i] [j] = w [j] [i];
			}

		// d = diag(diag(W'*C0*W));
		// W = W*d^(-1/2);

		NUMdmatrix_normalizeColumnVectors (wc.peek(), dimension, dimension, c0 -> data);

		// scale eigenvectors for sphering
		// [vb,db] = eig(C0);
		// P = db^(-1/2)*vb';

		Eigen_initFromSymmetricMatrix (eigen.get(), c0 -> data, dimension);
		for (integer i = 1; i <= dimension; i ++) {
			Melder_require (eigen -> eigenvalues [i] >= 0.0, U"Covariance matrix should be positive definite. Eigenvalue [", i, U"] is negative.");
			double scalef = 1.0 / sqrt (eigen -> eigenvalues [i]);
			for (integer j = 1; j <= dimension; j ++) {
				p [dimension - i + 1] [j] = scalef * eigen -> eigenvectors [i] [j];
			}
		}

		// P*C [i]*P'

		for (integer ic = 1; ic <= thy size; ic ++) {
			CrossCorrelationTable cov1 = thy at [ic];
			CrossCorrelationTable cov2 = ccts -> at [ic];
			NUMdmatrices_multiply_VCVp (cov2 -> data, p.peek(), dimension, dimension, cov1 -> data, true);
		}

		// W = P'\W == inv(P') * W

		NUMpseudoInverse (p.peek(), dimension, dimension, pinv.peek(), 0);

		NUMdmatrices_multiply_VpC (w, pinv.peek(), dimension, dimension, wc.peek(), dimension);

		// initialisation for order KN^3

		for (integer ic = 2; ic <= thy size; ic ++) {
			CrossCorrelationTable cov = ccts -> at [ic];
			// C * W
			NUMdmatrices_multiply_VC (m1.peek(), cov -> data, dimension, dimension, w, dimension);
			// D += scalef * M1*M1'
			NUMdmatrices_multiplyScaleAdd (d.peek(), m1.peek(), dimension, dimension, 2 * cweights [ic]);
		}

		integer iter = 0;
		double delta_w;

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		try {
			do {
				// the standard diagonality measure is rather expensive to calculate so we compare the norms of
				// differences of eigenvectors.

				delta_w = 0;
				for (integer kol = 1; kol <= dimension; kol ++) {
					for (integer i = 1; i <= dimension; i ++) {
						wvec [i] = w [i] [kol];
					}

					update_one_column (ccts.get(), d.peek(), cweights, wvec.peek(), -1, mvec.peek());

					Eigen_initFromSymmetricMatrix (eigen.get(), d.peek(), dimension);

					// Eigenvalues already sorted; get eigenvector of smallest !

					for (integer i = 1; i <= dimension; i ++) {
						wnew [i] = eigen -> eigenvectors [dimension] [i];
					}

					update_one_column (ccts.get(), d.peek(), cweights, wnew.peek(), 1, mvec.peek());
					for (integer i = 1; i <= dimension; i ++) {
						w [i] [kol] = wnew [i];
					}

					// compare norms of eigenvectors. We have to compare ||wvec +/- w_new|| because eigenvectors
					//  may change sign.

					double normp = 0, normm = 0;
					for (integer j = 1; j <= dimension; j ++) {
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
		// Take transpose to make W*C [i]W' diagonal instead of W'*C [i]*W => (P'*W)'=W'*P

		NUMmatrix_copyElements (w, wc.peek(), 1, dimension, 1, dimension);
		NUMdmatrices_multiply_VpC (w, wc.peek(), dimension, dimension, p.peek(), dimension); // W = W'*P: final result

		// Calculate the "real" diagonality measure
	//	double dm = CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure (thee, me, cweights, 1, thy size);

	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no joint diagonalization (qdiag).");
	}
}

void MixingMatrix_CrossCorrelationTableList_improveUnmixing (MixingMatrix me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double tol, int method) {
	autoDiagonalizer him = MixingMatrix_to_Diagonalizer (me);
	Diagonalizer_CrossCorrelationTableList_improveDiagonality (him.get(), thee, maxNumberOfIterations, tol, method);
	NUMpseudoInverse (his data, his numberOfRows, his numberOfColumns, my data, 0);
}


/* Preconditions:
 * 	x [1..nrows] [1..ncols], cc [1..nrows] [1..nrows], centroid [1..nrows]
 * 	if (lag>0) {i2 + lag <= ncols} else {i1-lag >= 1}
 * 	no array boundary checks!
 * 	lag >= 0
 */
static void NUMcrossCorrelate_rows (double **x, integer nrows, integer icol1, integer icol2, integer lag, double **cc, double *centroid, double scale) {
	lag = labs (lag);
	integer nsamples = icol2 - icol1 + 1 + lag;
	for (integer i = 1; i <= nrows; i ++) {
		double sum = 0.0;
		for (integer k = icol1; k <= icol2 + lag; k ++) {
			sum += x [i] [k];
		}
		centroid [i] = sum / nsamples;
	}
	for (integer i = 1; i <= nrows; i ++) {
		for (integer j = i; j <= nrows; j ++) {
			double ccor = 0;
			for (integer k = icol1; k <= icol2; k ++) {
				ccor += (x [i] [k] - centroid [i]) * (x [j] [k + lag] - centroid [j]);
			}
			cc [j] [i] = cc [i] [j] = ccor * scale;
		}
	}
}

/*
	This is for multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples, (z [i] [k] - mean [i])(z [j] [k + tau] - mean [j]))*samplingTime
*/
autoCrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me, double startTime, double endTime, double lagStep) {
	try {
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		integer lag = Melder_ifloor (lagStep / my dx);   // ppgb: voor al dit soort dingen geldt: waarom afronden naar beneden?
		integer i1 = Sampled_xToNearestIndex (me, startTime);
		if (i1 < 1) {
			i1 = 1;
		}
		integer i2 = Sampled_xToNearestIndex (me, endTime);
		if (i2 > my nx) {
			i2 = my nx;
		}
		i2 -= lag;
		integer nsamples = i2 - i1 + 1;
		
		Melder_require (nsamples > my ny, U"Not enough samples, choose a longer interval.");
		
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
		Melder_require (my dx == thy dx, U"Sampling frequencies should be equal.");
		if (relativeEndTime <= relativeStartTime) {
			relativeStartTime = my xmin;
			relativeEndTime = my xmax;
		}
		integer ndelta = Melder_ifloor (lagStep / my dx), nchannels = my ny + thy ny;
		integer i1 = Sampled_xToNearestIndex (me, relativeStartTime);
		if (i1 < 1) {
			i1 = 1;
		}
		integer i2 = Sampled_xToNearestIndex (me, relativeEndTime);
		if (i2 > my nx) {
			i2 = my nx;
		}
		i2 -= ndelta;
		integer nsamples = i2 - i1 + 1;
		Melder_require (nsamples > nchannels, U"Not enough samples");
		
		autoCrossCorrelationTable him = CrossCorrelationTable_create (nchannels);
		autoNUMvector<double *> data (1, nchannels);
		for (integer i = 1; i <= my ny; i ++) {
			data [i] = my z [i];
		}
		for (integer i = 1; i <= thy ny; i ++) {
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

autoCrossCorrelationTableList Sound_to_CrossCorrelationTableList (Sound me, double startTime, double endTime, double lagStep, integer ncovars) {
	try {
		if (lagStep < my dx) {
			lagStep = my dx;
		}
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		Melder_require (startTime + ncovars * lagStep <= endTime, U"Lag time is too large.");
		
		autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
		for (integer i = 1; i <= ncovars; i ++) {
			double lag = (i - 1) * lagStep;
			autoCrossCorrelationTable ct = Sound_to_CrossCorrelationTable (me, startTime, endTime, lag);
			thy addItem_move (ct.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTableList created.");
	}
}

autoSound Sound_to_Sound_BSS (Sound me, double startTime, double endTime, integer ncovars, double lagStep, integer maxNumberOfIterations, double tol, int method) {
	try {
		autoMixingMatrix him = Sound_to_MixingMatrix (me, startTime, endTime, ncovars, lagStep, maxNumberOfIterations, tol, method);
		autoSound thee = Sound_MixingMatrix_unmix (me, him.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not separated.");
	}
}

/************************ Diagonalizer **********************************/

Thing_implement (Diagonalizer, TableOfReal, 0);

autoDiagonalizer Diagonalizer_create (integer dimension) {
	try {
		autoDiagonalizer me = Thing_new (Diagonalizer);
		TableOfReal_init (me.get(), dimension, dimension);
		for (integer i = 1; i <= dimension; i ++) {
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
		Melder_require (my numberOfRows == my numberOfColumns, U"The number of channels and the number of components should be equal.");
		
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

void MixingMatrix_Sound_improveUnmixing (MixingMatrix me, Sound thee, double startTime, double endTime, integer ncovars, double lagStep, integer maxNumberOfIterations, double tol, int method) {
	autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (thee, startTime, endTime, lagStep, ncovars);
	MixingMatrix_CrossCorrelationTableList_improveUnmixing (me, ccs.get(), maxNumberOfIterations, tol, method);
}

autoMixingMatrix Sound_to_MixingMatrix (Sound me, double startTime, double endTime, integer ncovars, double lagStep, integer maxNumberOfIterations, double tol, int method) {
	try {
		autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (me, startTime, endTime, lagStep, ncovars);
		autoMixingMatrix thee = MixingMatrix_create (my ny, my ny);
		MixingMatrix_setRandomGauss (thee.get(), 0.0, 1.0);
		MixingMatrix_CrossCorrelationTableList_improveUnmixing (thee.get(), ccs.get(), maxNumberOfIterations, tol, method);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MixingMatrix created.");
	}
}

autoMixingMatrix TableOfReal_to_MixingMatrix (TableOfReal me) {
	try {
		Melder_require (my numberOfColumns == my numberOfRows, U"Number of rows and columns should be equal.");
		
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

autoCrossCorrelationTable CrossCorrelationTable_create (integer dimension) {
	try {
		autoCrossCorrelationTable me = Thing_new (CrossCorrelationTable);
		SSCP_init (me.get(), dimension, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not created.");
	}
}

autoCrossCorrelationTable CrossCorrelationTable_createSimple (char32 *covars, char32 *centroid, integer numberOfSamples) {
	try {
		integer dimension = Melder_countTokens (centroid);
		integer ncovars = Melder_countTokens (covars);
		integer ncovars_wanted = dimension * (dimension + 1) / 2;
		
		Melder_require (ncovars == ncovars_wanted, U"The number of matrix elements and the number of "
			U"centroid elements should agree. There should be \"d(d+1)/2\" matrix values and \"d\" centroid values.");

		autoCrossCorrelationTable me = CrossCorrelationTable_create (dimension);

		/*
			Construct the full matrix from the upper-diagonal elements
		*/

		integer inum = 1, irow = 1;
		for (char32 *token = Melder_firstToken (covars); token != nullptr && inum <= ncovars_wanted; token = Melder_nextToken (), inum ++) {
			double number;
			integer nmissing = (irow - 1) * irow / 2;
			integer inumc = inum + nmissing;
			irow = (inumc - 1) / dimension + 1;
			integer icol = ( (inumc - 1) % dimension) + 1;
			Interpreter_numericExpression (nullptr, token, &number);
			my data [irow] [icol] = my data [icol] [irow] = number;
			if (icol == dimension) {
				irow ++;
			}
		}

		inum = 1;
		for (char32 *token = Melder_firstToken (centroid); token != nullptr && inum <= dimension; token = Melder_nextToken (), inum ++) {
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
	for (integer i = 1; i <= our size; i ++) {
		double dm = CrossCorrelationTable_getDiagonalityMeasure (our at [i]);
		MelderInfo_writeLine (U"  Diagonality measure for item ", i, U": ", dm);
	}
}

autoCrossCorrelationTableList CrossCorrelationTables_to_CrossCorrelationTableList (OrderedOf<structCrossCorrelationTable> *me) {
	try {
		autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
		integer numberOfRows = 0, numberOfColumns = 0, numberOfSelected = 0;
		for (integer i = 1; i <= my size; i ++) {
			CrossCorrelationTable item = my at [i];
			numberOfSelected ++;
			if (numberOfSelected == 1) {
				numberOfRows = item -> numberOfRows;
				numberOfColumns = item -> numberOfColumns;
			}
			Melder_require (item -> numberOfRows == numberOfRows && item -> numberOfColumns == numberOfColumns, 
				U"Dimensions of table ", i, U" differs from the rest.");
		
			autoCrossCorrelationTable myc = Data_copy (item);
			thy addItem_move (myc.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No CrossCorrelationTableList created from CrossCorrelationTable(s)");
	}
}

Thing_implement (CrossCorrelationTableList, SSCPList, 0);

double CrossCorrelationTableList_getDiagonalityMeasure (CrossCorrelationTableList me, double *w, integer start, integer end) {
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
	integer ntables = end - start + 1;
	integer dimension = my at [1] -> numberOfColumns;
	double dmsq = 0;
	for (integer k = start; k <= end; k ++) {
		CrossCorrelationTable thee = my at [k];
		double dmksq = NUMdmatrix_diagonalityMeasure (thy data, dimension);
		dmsq += (w ? dmksq * w [k] : dmksq / ntables);
	}
	return dmsq;
}

/************************** CrossCorrelationTables & Diagonalizer *******************************/

double CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure (CrossCorrelationTableList me, Diagonalizer thee, double *w, integer start, integer end) {
	autoCrossCorrelationTableList him = CrossCorrelationTableList_Diagonalizer_diagonalize (me, thee);
	double dm = CrossCorrelationTableList_getDiagonalityMeasure (him.get(), w, start, end);
	return dm;
}

autoCrossCorrelationTable CrossCorrelationTable_Diagonalizer_diagonalize (CrossCorrelationTable me, Diagonalizer thee) {
	try {
		Melder_require (my numberOfRows == thy numberOfRows, U"The CrossCorrelationTable and the Diagonalizer matrix dimensions should be equal.");

		autoCrossCorrelationTable him = CrossCorrelationTable_create (my numberOfColumns);
		NUMdmatrices_multiply_VCVp (his data, thy data, my numberOfColumns, my numberOfColumns, my data, true);
		return him;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not diagonalized.");
	}
}

autoCrossCorrelationTableList CrossCorrelationTableList_Diagonalizer_diagonalize (CrossCorrelationTableList me, Diagonalizer thee) {
	try {
		autoCrossCorrelationTableList him = CrossCorrelationTableList_create ();
		for (integer i = 1; i <= my size; i ++) {
			CrossCorrelationTable item = my at [i];
			autoCrossCorrelationTable ct = CrossCorrelationTable_Diagonalizer_diagonalize (item, thee);
			his addItem_move (ct.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTableList not diagonalized.");
	}
}

autoDiagonalizer CrossCorrelationTableList_to_Diagonalizer (CrossCorrelationTableList me, integer maxNumberOfIterations, double tol, int method) {
	try {
		Melder_assert (my size > 0);
		CrossCorrelationTable him = my at [1];
		autoDiagonalizer thee = Diagonalizer_create (his numberOfColumns);
		Diagonalizer_CrossCorrelationTableList_improveDiagonality (thee.get(), me, maxNumberOfIterations, tol, method);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Diagonalizer not created from CrossCorrelationTableList.");
	};
}

void Diagonalizer_CrossCorrelationTableList_improveDiagonality (Diagonalizer me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double tol, int method) {
	if (method == 1) {
		autoNUMvector<double> cweights (1, thy size);
		for (integer i = 1; i <= thy size; i ++) {
			cweights [i] = 1.0 / thy size;
		}
		Diagonalizer_CrossCorrelationTable_qdiag (me, thee, cweights.peek(), maxNumberOfIterations, tol);
	} else {
		Diagonalizer_CrossCorrelationTableList_ffdiag (me, thee, maxNumberOfIterations, tol);
	}
}

autoSound Sound_whitenChannels (Sound me, double varianceFraction) {
    try {
        autoCovariance cov = Sound_to_Covariance_channels (me, 0.0, 0.0);
        autoSound thee = Sound_Covariance_whitenChannels (me, cov.get(), varianceFraction);
        return thee;
    } catch (MelderError) {
        Melder_throw (me, U": not whitened.");
    }
}

autoSound Sound_Covariance_whitenChannels (Sound me, Covariance thee, double varianceFraction) {
    try {
        autoPCA pca = SSCP_to_PCA (thee);
        integer numberOfComponents = Eigen_getDimensionOfFraction (pca.get(), varianceFraction);
        autoSound him = Sound_PCA_whitenChannels (me, pca.get(), numberOfComponents);
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": not whitened from ", thee);
    }
}

/*
 * Generate n different cct's that have a common diagonalizer.
 */
autoCrossCorrelationTableList CrossCorrelationTableList_createTestSet (integer dimension, integer n, int firstPositiveDefinite, double sigma) {
	try {
		/*
			Start with a square matrix with random gaussian elements and make its singular value decomposition UDV'
			The V matrix will be the common diagonalizer matrix that we use.
		*/

		autoNUMmatrix<double> d (1, dimension, 1, dimension);
		for (integer i = 1; i <= dimension; i ++) { // Generate the rotation matrix
			for (integer j = 1; j <= dimension; j ++) {
				d [i] [j] = NUMrandomGauss (0.0, 1.0);
			}
		}
		autoNUMmatrix<double> v (1, dimension, 1, dimension);
		autoSVD svd = SVD_create_d (d.peek(), dimension, dimension);
		autoCrossCorrelationTableList me = CrossCorrelationTableList_create ();

		for (integer i = 1; i <= dimension; i ++) {
			for (integer j = 1; j <= dimension; j ++) {
				d [i] [j] = 0.0;
			}
		}

		// Start with a diagonal matrix D and calculate V'DV

		for (integer k = 1; k <= n; k ++) {
			autoCrossCorrelationTable ct = CrossCorrelationTable_create (dimension);
			double low = (k == 1 && firstPositiveDefinite ? 0.1 : -1.0);
			for (integer i = 1; i <= dimension; i ++) {
				d [i] [i] = NUMrandomUniform (low, 1.0);
			}
			for (integer i = 1; i <= dimension; i ++) {
				for (integer j = 1; j <= dimension; j ++) {
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
static void Sound_MixingMatrix_improveUnmixing_fica (Sound me, MixingMatrix thee, integer maxNumberOfIterations, double /* tol */, int /* method */) {
	try {
		integer iter = 0;
		Melder_require (my ny == thy numberOfColumns, U"Dimensions should agree.");
		
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
