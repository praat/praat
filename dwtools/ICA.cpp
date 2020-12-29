/* ICA.cpp
 *
 * Copyright (C) 2010-2019 David Weenink
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

//TODO 20181013 massive cleanups only at pointers removed rest follows
// matrix multiply R = V*C*V', V is nrv x ncv, C is ncv x ncv, R is nrv x nrv
static void MATmul_VCVt_preallocated (MAT r, constMAT v, constMAT c, bool csym) {
	Melder_assert (r.nrow == r.ncol);
	Melder_assert (v.nrow == r.ncol);
	Melder_assert (c.nrow == c.ncol);
	Melder_assert (v.ncol == c.ncol);
	for (integer i = 1; i <= r.nrow; i ++) {
		const integer jstart = ( csym ? i : 1 );
		for (integer j = jstart; j <= r.nrow; j ++) {
			// V_ik C_kl V'_lj = V_ik C_kl V_jl
			longdouble vcv = 0.0;
			for (integer k = 1; k <= c.nrow; k ++) {
				for (integer l = 1; l <= c.nrow; l ++) {
					vcv += v [i] [k] * c [k] [l] * v [j] [l];
				}
			}
			r [i] [j] = vcv;
			if (csym)
				r [j] [i] = vcv;
		}
	}
}

// D += scalef * M * M', M = nrm x ncm, D is nrm x nrm
static void multiplyScaleAdd_preallocated (MAT r, constMAT m, double scalef) {
	Melder_assert (r.nrow == r.ncol && r.nrow == m.nrow);
	for (integer i = 1; i <= r.nrow; i ++) {
		for (integer j = 1; j <= r.nrow; j ++)
			r [i] [j] += scalef * NUMinner (m.row (i), m.row (j)); // M_ik M'_kj = M_ik M_jk
	}
}

/*
	d = diag(diag(W'*C0*W));
	W = W*d^(-1/2);

	D_ij = W'_ik C_kl W_lj => D_ii = W_ki C_kl W_li
*/
static void normalizeColumnVectors (MAT w, MAT c) { // TODO
	Melder_assert (w.nrow == c.ncol && w.ncol == c.nrow);
	for (integer i = 1; i <= w.ncol; i ++) {
		longdouble di = 0.0;
		for (integer k = 1; k <= w.ncol; k ++)
			for (integer l = 1; l <= w.nrow; l ++) {
				di += w [k] [i] * c [k] [l] * w [l] [i];
			}
		di = 1.0 / sqrt (di);
		for (integer j = 1; j <= w.nrow; j ++) {
			w [j] [i] *= di;
		}
	}
}

static double diagonalityMeasure (MAT v) {
	Melder_assert (v.nrow == v.ncol);
	longdouble dmsq = 0.0;
	if (v.nrow < 2)
		return 0.0;
	for (integer i = 1; i <= v.nrow; i ++) {
		for (integer j = 1; j <= v.nrow; j ++)
			if (i != j)
				dmsq += v [i] [j] * v [i] [j];
	}
	return (double) dmsq / (v.nrow * (v.nrow - 1));
}

/*
	This routine is modelled after qdiag.m from Andreas Ziehe, Pavel Laskov, Guido Nolte, Klaus-Robert Müller,
	A Fast Algorithm for Joint Diagonalization with Non-orthogonal Transformations and its Application to
	Blind Source Separation, Journal of Machine Learning Research 5 (2004), 777–800.
*/
static void Diagonalizer_CrossCorrelationTableList_ffdiag (Diagonalizer me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double delta) {
	try {
		integer iter = 0, dimension = my numberOfRows;

		autoCrossCorrelationTableList ccts = CrossCorrelationTableList_Diagonalizer_diagonalize (thee, me);
		autoMAT w = zero_MAT (dimension, dimension);
		autoMAT vnew = zero_MAT (dimension, dimension);
		autoMAT cc = zero_MAT (dimension, dimension);

		for (integer i = 1; i <= dimension; i ++)
			w [i] [i] = 1.0;

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		double dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
		try {
			double dm_old, theta = 1.0, dm_start = dm_new;
			do {
				dm_old = dm_new;
				for (integer i = 1; i <= dimension; i ++) {
					for (integer j = i + 1; j <= dimension; j ++) {
						longdouble zii = 0.0, zij = 0.0, zjj = 0.0, yij = 0.0, yji = 0.0;   // zij == zji
						for (integer k = 1; k <= ccts -> size; k ++) {
							CrossCorrelationTable ct = ccts -> at [k];
							zii += ct -> data [i] [i] * ct -> data [i] [i];
							zij += ct -> data [i] [i] * ct -> data [j] [j];
							zjj += ct -> data [j] [j] * ct -> data [j] [j];
							yij += ct -> data [j] [j] * ct -> data [i] [j];
							yji += ct -> data [i] [i] * ct -> data [i] [j];
						}
						const longdouble denom = zjj * zii - zij * zij;
						if (denom != 0.0) {
							w [i] [j] = double ((zij * yji - zii * yij) / denom);
							w [j] [i] = double ((zij * yij - zjj * yji) / denom);
						} else {
							// ppgb: prove that the result is valid
						}
					}
				}
				longdouble norma = 0.0;
				for (integer i = 1; i <= dimension; i ++) {
					longdouble normai = 0.0;
					for (integer j = 1; j <= dimension; j ++) {
						if (i != j)
							normai += fabs (w [i] [j]);
					}
					if (normai > norma)
						norma = normai;
				}
				// evaluate the norm
				if (norma > theta) {
					longdouble normf = 0.0;
					for (integer i = 1; i <= dimension; i ++)
						for (integer j = 1; j <= dimension; j ++)
							if (i != j)
								normf += w [i] [j] * w [i] [j];
					const longdouble scalef = theta / sqrt (normf);
					for (integer i = 1; i <= dimension; i ++)
						for (integer j = 1; j <= dimension; j ++)
							if (i != j)
								w [i] [j] *= scalef;
				}
				// update V
				vnew.all() <<= my data.all();
				mul_MAT_out (my data.get(), w.get(), vnew.get());
				for (integer k = 1; k <= ccts -> size; k ++) {
					const CrossCorrelationTable ct = ccts -> at [k];
					Melder_assert (ct -> data.nrow == dimension && ct -> data.ncol == dimension);   // ppgb 20180913
					cc.all() <<= ct -> data.all();
					MATmul_VCVt_preallocated (ct -> data.get(), w.get(), cc.get(), true);
				}
				dm_new = CrossCorrelationTableList_getDiagonalityMeasure (ccts.get(), nullptr, 0, 0);
				iter ++;
				Melder_progress ((double) iter / (double) maxNumberOfIterations, U"Iteration: ", iter, U", measure: ", (double) dm_new, U"\n fractional measure: ", (double)(dm_new / dm_start));
			} while (fabs (dm_old - dm_new) > std::max (fabs (delta * dm_new), NUMeps) && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no joint diagonalization (ffdiag).");
	}
}

/*
	The folowing two routines are modelled after qdiag.m from
	R. Vollgraf and K. Obermayer, Quadratic Optimization for Simultaneous
	Matrix Diagonalization, IEEE Transaction on Signal Processing, 2006,
*/
static void update_one_column (CrossCorrelationTableList me, MAT d, constVEC wp, constVEC wvec, double scalef, VEC work) {
	integer dimension = my at [1] -> numberOfColumns;

	for (integer ic = 2; ic <= my size; ic ++) { // exclude C0
		const SSCP cov = my at [ic];
		// m1 = C * wvec
		mul_VEC_out (work, cov -> data.get(), wvec);
		// D = D +/- 2*p(t)*(m1*m1');
		for (integer i = 1; i <= dimension; i ++) {
			for (integer j = 1; j <= dimension; j ++) {
				d [i] [j] += 2.0 * scalef * wp [ic] * work [i] * work [j];
			}
		}
	}
}

static void Diagonalizer_CrossCorrelationTable_qdiag (Diagonalizer me, CrossCorrelationTableList thee, VEC cweights, integer maxNumberOfIterations, double delta) {
	try {
		const CrossCorrelationTable c0 = thy at [1];
		const integer dimension = c0 -> numberOfColumns;

		autoEigen eigen = Thing_new (Eigen);
		autoCrossCorrelationTableList ccts = Data_copy (thee);
		autoMAT d = zero_MAT (dimension, dimension);
		autoMAT pinv = raw_MAT (dimension, dimension);
		autoMAT p = zero_MAT (dimension, dimension);
		autoMAT m1 = zero_MAT (dimension, dimension);
		autoVEC wvec = raw_VEC (dimension);
		autoVEC wnew = raw_VEC (dimension);
		autoVEC mvec = zero_VEC (dimension);

		autoMAT wc = transpose_MAT (my data.get());

		// d = diag(diag(W'*C0*W));
		// W = W*d^(-1/2);

		normalizeColumnVectors (wc.get(), c0 -> data.get());

		// scale eigenvectors for sphering
		// [vb,db] = eig(C0);
		// P = db^(-1/2)*vb';
		Eigen_initFromSymmetricMatrix (eigen.get(), c0 -> data.get());
		
		for (integer i = 1; i <= dimension; i ++) {
			Melder_require (eigen -> eigenvalues [i] >= 0.0,
				U"Covariance matrix should be positive definite. Eigenvalue [", i, U"] is negative.");
			const double scalef = 1.0 / sqrt (eigen -> eigenvalues [i]);
			p.row (dimension - i + 1) <<= eigen -> eigenvectors.row (i)  *  scalef;
		}

		// P*C [i]*P'

		for (integer ic = 1; ic <= thy size; ic ++) {
			const CrossCorrelationTable cov1 = thy at [ic];
			const CrossCorrelationTable cov2 = ccts -> at [ic];
			MATmul_VCVt_preallocated (cov2 -> data.get(), p.get(), cov1 -> data.get(), true);
		}

		// W = P'\W == inv(P') * W

		MATpseudoInverse (pinv.get (), p.get(), 0.0);
		mul_MAT_out (my data.get(), pinv.transpose(), wc.get());

		// initialisation for order KN^3

		for (integer ic = 2; ic <= thy size; ic ++) {
			const CrossCorrelationTable cov = ccts -> at [ic];
			// C * W
			mul_MAT_out (m1.get(), cov -> data.get(), my data.get());
			// D += scalef * M1*M1'
			multiplyScaleAdd_preallocated (d.get(), m1.get(), 2.0 * cweights [ic]);
		}

		integer iter = 0;
		double delta_w;

		autoMelderProgress progress (U"Simultaneous diagonalization of many CrossCorrelationTables...");
		try {
			do {
				// the standard diagonality measure is rather expensive to calculate so we compare the norms of
				// differences of eigenvectors.

				delta_w = 0.0;
				for (integer kol = 1; kol <= dimension; kol ++) {
					wvec.all() <<= my data.column (kol);

					update_one_column (ccts.get(), d.get(), cweights, wvec.get(), -1.0, mvec.get());

					Eigen_initFromSymmetricMatrix (eigen.get(), d.get());

					// Eigenvalues already sorted; get eigenvector of smallest !
					wnew.all() <<= eigen -> eigenvectors.row (dimension);

					update_one_column (ccts.get(), d.get(), cweights, wnew.get(), 1.0, mvec.get());
					my data.column (kol) <<= wnew.all();

					// compare norms of eigenvectors. We have to compare ||wvec +/- w_new|| because eigenvectors
					//  may change sign.

					double normp = 0.0, normm = 0.0;
					for (integer j = 1; j <= dimension; j ++) {
						const double dm = wvec [j] - wnew [j], dp = wvec [j] + wnew [j];
						normp += dm * dm;
						normm += dp * dp;
					}

					normp = std::min (normp, normm);
					normp = sqrt (normp);
					delta_w = std::max (normp, delta_w );
				}
				iter ++;

				Melder_progress ((double) iter / (double) (maxNumberOfIterations + 1), U"Iteration: ", iter, U", norm: ", delta_w);
			} while (delta_w > delta && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}

		// Revert the sphering W = P'*W;
		// Take transpose to make W*C [i]W' diagonal instead of W'*C [i]*W => (P'*W)'=W'*P
		wc.all() <<= my data.all();
		mul_MAT_out (my data.get(), wc.transpose(), p.get()); // W = W'*P: final result

		// Calculate the "real" diagonality measure
	//	double dm = CrossCorrelationTableList_Diagonalizer_getDiagonalityMeasure (thee, me, cweights, 1, thy size);

	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no joint diagonalization (qdiag).");
	}
}

void MixingMatrix_CrossCorrelationTableList_improveUnmixing (MixingMatrix me, CrossCorrelationTableList thee, integer maxNumberOfIterations, double tol, int method) {
	autoDiagonalizer him = MixingMatrix_to_Diagonalizer (me);
	Diagonalizer_CrossCorrelationTableList_improveDiagonality (him.get(), thee, maxNumberOfIterations, tol, method);
	MATpseudoInverse (my data.get(), his data.get(), 0);
}


/* Preconditions:
 * 	x [1..nrows] [1..ncols], cc [1..nrows] [1..nrows], centroid [1..nrows]
 * 	if (lag>0) {i2 + lag <= ncols} else {i1-lag >= 1}
 * 	no array boundary checks!
 * 	lag >= 0
 */
static void NUMcrossCorrelate_rows (constMAT x, integer icol1, integer icol2, integer lag, MAT inout_cc, VEC inout_centroid, double scale) {
	Melder_assert (inout_cc.nrow == inout_cc.ncol && inout_cc.nrow == x.nrow);
	lag = integer_abs (lag);
	const integer nsamples = icol2 - icol1 + 1 + lag;
	Melder_require (nsamples > 0, U"Not enough samples to perform crosscorrealtions."); 
	for (integer i = 1; i <= x.nrow; i ++)
		inout_centroid [i] = NUMmean (x.row (i).part (icol1, icol2 + lag));

	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = irow; icol <= x.nrow; icol ++) {
			longdouble ccor = 0.0;
			for (integer k = icol1; k <= icol2; k ++)
				ccor += (x [irow] [k] - inout_centroid [irow]) * (x [icol] [k + lag] - inout_centroid [icol]);
			inout_cc [icol] [irow] = inout_cc [irow] [icol] = ccor * scale;
		}
	}
}

/*
	This is for multi-channel "sounds" like EEG signals.
	The cross-correlation between channel i and channel j is defined as
		sum(k=1..nsamples, (z [i] [k] - mean [i])(z [j] [k + tau] - mean [j]))*samplingTime
*/
autoCrossCorrelationTable Sound_to_CrossCorrelationTable (Sound me,	double startTime, double endTime, double lagStep)
{
	try {
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		const integer lag = Melder_iround (lagStep / my dx);
		integer i1 = Sampled_xToNearestIndex (me, startTime);
		if (i1 < 1)
			i1 = 1;
		
		integer i2 = Sampled_xToNearestIndex (me, endTime);
		if (i2 > my nx)
			i2 = my nx;

		i2 -= lag;
		const integer nsamples = i2 - i1 + 1;
		
		Melder_require (nsamples > my ny,
			U"Not enough samples, choose a longer interval.");
		
		autoCrossCorrelationTable thee = CrossCorrelationTable_create (my ny);

		NUMcrossCorrelate_rows (my z.get(), i1, i2, lag, thy data.get(), thy centroid.get(), my dx);

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
autoCrossCorrelationTable Sounds_to_CrossCorrelationTable_combined (Sound me, Sound thee,
	double relativeStartTime, double relativeEndTime, double lagStep)
{
	try {
		Melder_require (my dx == thy dx,
			U"Sampling frequencies should be equal.");
		if (relativeEndTime <= relativeStartTime) {
			relativeStartTime = my xmin;
			relativeEndTime = my xmax;
		}
		const integer lag = Melder_iround (lagStep / my dx), nchannels = my ny + thy ny;
		integer i1 = Sampled_xToNearestIndex (me, relativeStartTime);
		if (i1 < 1)
			i1 = 1;

		integer i2 = Sampled_xToNearestIndex (me, relativeEndTime);
		if (i2 > my nx)
			i2 = my nx;

		i2 -= lag;
		integer nsamples = i2 - i1 + 1;
		Melder_require (nsamples > nchannels, U"Not enough samples");
		
		autoCrossCorrelationTable him = CrossCorrelationTable_create (nchannels);
		
		autoVEC centroid1 = raw_VEC (my ny);
		autoMAT x1x1 = raw_MAT (my ny, my ny);
		NUMcrossCorrelate_rows (my z.get(), i1, i2, lag, x1x1.get(), centroid1.get(), my dx);
		his centroid.part (1, my ny) <<= centroid1.all();
		for (integer irow = 1; irow <= my ny; irow ++)
			his data.row (irow).part (1, my ny) <<= x1x1.row (irow);

		autoVEC centroid2 = raw_VEC (thy ny);
		autoMAT x2x2 = raw_MAT (thy ny, thy ny);
		NUMcrossCorrelate_rows (thy z.get(), i1, i2, lag, x2x2.get(), centroid2.get(), my dx);
		his centroid.part (my ny + 1, nchannels) <<= centroid2.all();
		for (integer irow = 1; irow <= thy ny; irow ++)
			his data.row (my ny + irow).part (my ny + 1, nchannels) <<= x2x2.row (irow);

		for (integer irow = 1; irow <= my ny; irow ++) {
			for (integer icol = 1; icol <= thy ny; icol ++) {
				longdouble ccor = 0.0;
				for (integer k = i1; k <= i2; k ++)
					ccor += (my z [irow] [k] - centroid1 [irow]) * (thy z [icol] [k + lag] - centroid2 [icol]);
				his data [irow] [my ny + icol] = his data [my ny + icol] [irow] = ccor * my dx;
			}
		}

		his numberOfObservations = nsamples;

		return him;
	} catch (MelderError) {
		Melder_throw (me, U": CrossCorrelationTable not created.");
	}
}

autoCovariance Sound_to_Covariance_channels (Sound me, double startTime, double endTime) {
    try {
        const double lagStep = 0.0;
        autoCrossCorrelationTable thee = Sound_to_CrossCorrelationTable (me, startTime, endTime, lagStep);
        autoCovariance him = Thing_new (Covariance);
        thy structCrossCorrelationTable :: v_copy (him.get());
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": no Covariance created.");
    }
}

autoCrossCorrelationTableList Sound_to_CrossCorrelationTableList (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep)
{
	try {
		if (lagStep < my dx)
			lagStep = my dx;
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		Melder_require (startTime + numberOfCrossCorrelations * lagStep <= endTime,
			U"Lag time is too large.");
		
		autoCrossCorrelationTableList thee = CrossCorrelationTableList_create ();
		for (integer i = 1; i <= numberOfCrossCorrelations; i ++) {
			const double lag = (i - 1) * lagStep;
			autoCrossCorrelationTable ct = Sound_to_CrossCorrelationTable (me, startTime, endTime, lag);
			thy addItem_move (ct.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no CrossCorrelationTableList created.");
	}
}

autoSound Sound_to_Sound_BSS (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method)
{
	try {
		autoMixingMatrix him = Sound_to_MixingMatrix (me,
			startTime, endTime, numberOfCrossCorrelations, lagStep,
			maxNumberOfIterations, tol, method);
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
		for (integer i = 1; i <= dimension; i ++)
			my data [i] [i] = 1.0;

		return me;
	} catch (MelderError) {
		Melder_throw (U"Diagonalizer not created.");
	}
}

/***************** Diagonalizer & MixingMatrix *************************/


autoDiagonalizer MixingMatrix_to_Diagonalizer (MixingMatrix me) {
	try {
		Melder_require (my numberOfRows == my numberOfColumns,
			U"The number of channels and the number of components should be equal.");
		
		autoDiagonalizer thee = Diagonalizer_create (my numberOfRows);
		MATpseudoInverse (thy data.get(), my data.get(), 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Diagonalizer created.");
	}
}

autoMixingMatrix Diagonalizer_to_MixingMatrix (Diagonalizer me) {
	try {
		autoMixingMatrix thee = MixingMatrix_create (my numberOfRows, my numberOfColumns);
		MixingMatrix_setRandomGauss (thee.get(), 0.0, 1.0);
		MATpseudoInverse (thy data.get(), my data.get(), 0.0);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MixingMatrix created.");
	}
}

void MixingMatrix_Sound_improveUnmixing (MixingMatrix me, Sound thee, double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep, integer maxNumberOfIterations, double tol, int method) {
	autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (thee, startTime, endTime, numberOfCrossCorrelations, lagStep);
	MixingMatrix_CrossCorrelationTableList_improveUnmixing (me, ccs.get(), maxNumberOfIterations, tol, method);
}

autoMixingMatrix Sound_to_MixingMatrix (Sound me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method)
{
	try {
		autoCrossCorrelationTableList ccs = Sound_to_CrossCorrelationTableList (me, startTime, endTime, numberOfCrossCorrelations, lagStep);
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
		Melder_require (my numberOfColumns == my numberOfRows,
			U"Number of rows and columns should be equal.");
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
	const double dm = CrossCorrelationTable_getDiagonalityMeasure (this);
	MelderInfo_writeLine (U"Diagonality measure: ", dm);
}

autoCrossCorrelationTable CrossCorrelationTable_create (integer dimension) {
	try {
		autoCrossCorrelationTable me = Thing_new (CrossCorrelationTable);
		SSCP_init (me.get(), dimension, kSSCPstorage::COMPLETE);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not created.");
	}
}

autoCrossCorrelationTable CrossCorrelationTable_createSimple (conststring32 covars_string, conststring32 centroid_string, integer numberOfSamples) {
	try {
		autoSTRVEC covars = splitByWhitespace_STRVEC (covars_string);
		autoSTRVEC centroid = splitByWhitespace_STRVEC (centroid_string);
		const integer dimension = centroid.size;
		const integer ncovars = covars.size;
		const integer ncovars_wanted = dimension * (dimension + 1) / 2;
		
		Melder_require (ncovars == ncovars_wanted,
			U"The number of matrix elements and the number of "
			U"centroid elements should agree. There should be \"d(d+1)/2\" matrix values and \"d\" centroid values.");

		autoCrossCorrelationTable me = CrossCorrelationTable_create (dimension);

		/*
			Construct the full matrix from the upper-diagonal elements
		*/
		integer irow = 1;
		for (integer inum = 1; inum <= ncovars; inum ++) {
			const integer nmissing = (irow - 1) * irow / 2;
			const integer inumc = inum + nmissing;
			irow = (inumc - 1) / dimension + 1;
			const integer icol = (inumc - 1) % dimension + 1;
			double number;
			Interpreter_numericExpression (nullptr, covars [inum].get(), & number);
			my data [irow] [icol] = my data [icol] [irow] = number;
			if (icol == dimension)
				irow ++;
		}
		for (integer inum = 1; inum <= dimension; inum ++) {
			double number;
			Interpreter_numericExpression (nullptr, centroid [inum].get(), & number);
			my centroid [inum] = number;
		}
		my numberOfObservations = numberOfSamples;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not created.");
	}
}

double CrossCorrelationTable_getDiagonalityMeasure (CrossCorrelationTable me) {
	return diagonalityMeasure (my data.get());
}

/************* CrossCorrelationTables *****************************/

void structCrossCorrelationTableList :: v_info () {
	our structThing :: v_info ();
	MelderInfo_writeLine (U"Contains ", our size, U" CrossCorrelationTable objects");
	CrossCorrelationTable thee = our at [1];
	MelderInfo_writeLine (U"Number of rows and columns: ", thy numberOfRows, U" in each CrossCorrelationTable");
	for (integer i = 1; i <= our size; i ++) {
		const double dm = CrossCorrelationTable_getDiagonalityMeasure (our at [i]);
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
	if (start < 1)
		start = 1;
	if (end > my size)
		end = my size;
	const integer ntables = end - start + 1;
	double dmsq = 0;
	for (integer k = start; k <= end; k ++) {
		const CrossCorrelationTable thee = my at [k];
		double dmksq = diagonalityMeasure (thy data.get());
		dmsq += ( w ? dmksq * w [k] : dmksq / ntables );
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
		Melder_require (my numberOfRows == thy numberOfRows,
			U"The CrossCorrelationTable and the Diagonalizer matrix dimensions should be equal.");

		autoCrossCorrelationTable him = CrossCorrelationTable_create (my numberOfColumns);
		MATmul_VCVt_preallocated (his data.get(), thy data.get(), my data.get(), true);
		return him;
	} catch (MelderError) {
		Melder_throw (U"CrossCorrelationTable not diagonalized.");
	}
}

autoCrossCorrelationTableList CrossCorrelationTableList_Diagonalizer_diagonalize (CrossCorrelationTableList me, Diagonalizer thee) {
	try {
		autoCrossCorrelationTableList him = CrossCorrelationTableList_create ();
		for (integer i = 1; i <= my size; i ++) {
			const CrossCorrelationTable item = my at [i];
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
		autoVEC cweights = raw_VEC (thy size);
		cweights.all() <<= 1.0 / thy size;
		Diagonalizer_CrossCorrelationTable_qdiag (me, thee, cweights.get(), maxNumberOfIterations, tol);
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
		const integer numberOfComponents = Eigen_getDimensionOfFraction (pca.get(), varianceFraction);
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

		autoMAT d = randomGauss_MAT (dimension, dimension, 0.0, 1.0);
		autoMAT v = raw_MAT (dimension, dimension);
		autoSVD svd = SVD_createFromGeneralMatrix (d.get());
		autoCrossCorrelationTableList me = CrossCorrelationTableList_create ();

		d.all()  <<=  0.0;

		// Start with a diagonal matrix D and calculate V'DV

		for (integer k = 1; k <= n; k ++) {
			autoCrossCorrelationTable ct = CrossCorrelationTable_create (dimension);
			const double low = ( k == 1 && firstPositiveDefinite ? 0.1 : -1.0 );
			for (integer i = 1; i <= dimension; i ++)
				d [i] [i] = NUMrandomUniform (low, 1.0);
			for (integer i = 1; i <= dimension; i ++)
				for (integer j = 1; j <= dimension; j ++)
					v [i] [j] = NUMrandomGauss (svd -> v [i] [j], sigma);
			// we need V'DV, however our V has eigenvectors row-wise -> VDV'
			MATmul_VCVt_preallocated (ct -> data.get(), v.get(), d.get(), true);
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
		
		autoMAT x = newmatrixcopy (my z.get());
		do {
			iter ++;
		} while (/*fabs((dm_old - dm_new) / dm_new) > tol &&*/ iter < maxNumberOfIterations);
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U" .");
	}
}
#endif

/* End of file ICA.cpp */
