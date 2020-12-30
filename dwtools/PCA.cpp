/* PCA.cpp
 *
 * Principal Component Analysis
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20000511 PCA_TableOfReal_to_Configuration: added centralize option.
 	(later removed)
 djmw 20020327 PCA_TableOfReal_to_Configuration modified internals.
 djmw 20020418 Removed some causes for compiler warnings.
 djmw 20020502 modified call Eigen_TableOfReal_project_into.
 djmw 20030324 Added PCA_TableOfReal_getFractionVariance.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: oo_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20081119 Check in TableOfReal_to_PCA if TableOfReal_areAllCellsDefined
  djmw 20110304 Thing_new
*/

#include "Configuration.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Matrix_extensions.h"
#include "NUM2.h"
#include "PCA.h"
#include "TableOfReal_extensions.h"

#include "oo_DESTROY.h"
#include "PCA_def.h"
#include "oo_COPY.h"
#include "PCA_def.h"
#include "oo_EQUAL.h"
#include "PCA_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PCA_def.h"
#include "oo_WRITE_TEXT.h"
#include "PCA_def.h"
#include "oo_READ_TEXT.h"
#include "PCA_def.h"
#include "oo_WRITE_BINARY.h"
#include "PCA_def.h"
#include "oo_READ_BINARY.h"
#include "PCA_def.h"
#include "oo_DESCRIPTION.h"
#include "PCA_def.h"

Thing_implement (PCA, Eigen, 0);

void structPCA :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of components: ", numberOfEigenvalues);
	MelderInfo_writeLine (U"Number of dimensions: ", dimension);
	MelderInfo_writeLine (U"Number of observations: ", numberOfObservations);
}

autoPCA PCA_create (integer numberOfComponents, integer dimension) {
	try {
		autoPCA me = Thing_new (PCA);
		Eigen_init (me.get(), numberOfComponents, dimension);
		my labels = autoSTRVEC (dimension);
		my centroid = zero_VEC (dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PCA not created");
	}
}

void PCA_setNumberOfObservations (PCA me, integer numberOfObservations) {
	my numberOfObservations = numberOfObservations;
}

integer PCA_getNumberOfObservations (PCA me) {
	return my numberOfObservations;
}

void PCA_getEqualityOfEigenvalues (PCA me, integer from, integer to, int conservative, double *out_prob, double *out_chisq, double *out_df) {
	double sum = 0.0, sumln = 0.0;

	double prob = undefined, df = undefined, chisq = undefined;
	
	if (from == 0 && to == 0) {
		to = 1;
		from = my numberOfEigenvalues;
	}
	if (from < to && from > 0 && to <= my numberOfEigenvalues) {
		integer i;
		for (i = from; i <= to; i ++) {
			if (my eigenvalues [i] <= 0) {
				break;
			}
			sum += my eigenvalues [i];
			sumln += log (my eigenvalues [i]);
		}
		if (sum == 0.0)
			return;
		const integer r = i - from;
		double n = my numberOfObservations - 1;
		if (conservative)
			n -= from + (double) (r * (2 * r + 1) + 2) / (6.0 * r);

		df = r * (r + 1) / 2 - 1;
		chisq = n * (r * log (sum / r) - sumln);
		prob = NUMchiSquareQ (chisq, df);
	}
	if (out_prob)
		*out_prob = prob;
	if (out_chisq)
		*out_chisq = chisq;
	if (out_df)
		*out_df = df;
}

/* the low level routines 
 * 
 * The matrix M[numberOfRows, numberOfColumns] is interpreted as 'numberOfRows' vectors of dimension 'numberOfColumns'
 * The eigenstructure of the M'M will be calculated
 * 
 */

autoEigen PCA_to_Eigen (PCA me) {
	try {
		autoEigen thee = Eigen_create (my numberOfEigenvalues, my dimension);
		thy eigenvectors.all() <<= my eigenvectors.all();
		thy eigenvalues.all() <<= my eigenvalues.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Eigen created.");
	}
}

static autoPCA MAT_to_PCA (constMAT m, bool byColumns) {
	try {
		Melder_require (NUMdefined (m),
			U"All matrix elements should be defined.");
		Melder_require (NUMnorm (m, 2.0) > 0.0,
			U"Not all values in your table should be zero.");
		Melder_require (m.nrow > 1,
			U"The number of rows should be larger than 1.");
		autoMAT mcopy;
		if (byColumns) {
			if (m.ncol < m.nrow)
				Melder_warning (U"The number of columns in your table is less than the number of rows.");
			mcopy = transpose_MAT (m);
		} else {
			if (m.nrow < m.ncol)
				Melder_warning (U"The number of rows in your table is less than the number of columns.");
			mcopy = copy_MAT (m);
		}
		
		autoPCA thee = Thing_new (PCA);
		thy centroid = columnMeans_VEC (mcopy.get());
		mcopy.all()  -=  thy centroid.all();
		Eigen_initFromSquareRoot (thee.get(), mcopy.get());
		thy labels = autoSTRVEC (mcopy.ncol);
		PCA_setNumberOfObservations (thee.get(), mcopy.nrow);
		/*
			The covariance matrix C = A'A / (N-1). However, we have calculated
			the eigenstructure for A'A. This has no consequences for the
			eigenvectors, but the eigenvalues have to be divided by (N-1).
		*/
		thy eigenvalues.all()  *=  1.0 / (mcopy.nrow - 1);
		
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No PCA created from ", ( byColumns ? U"columns." : U"rows." ));
	}	
}

autoPCA TableOfReal_to_PCA_byRows (TableOfReal me) {
	try {
		autoPCA thee = MAT_to_PCA (my data.get(), false);
		Melder_assert (thy labels.size == my numberOfColumns);
		thy labels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": PCA not created.");
	}
}

autoPCA Matrix_to_PCA_byColumns (Matrix me) {
	try {
		autoPCA thee = MAT_to_PCA (my z.get(), true);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA created from columns.");
	}
}

autoPCA Matrix_to_PCA_byRows (Matrix me) {
	try {
		autoPCA thee = MAT_to_PCA (my z.get(), false);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA created from rows.");
	}
}

autoTableOfReal PCA_TableOfReal_to_TableOfReal_zscores (PCA me, TableOfReal thee, integer numberOfDimensions) {
	try {
		if (numberOfDimensions == 0 || numberOfDimensions > my numberOfEigenvalues)
			numberOfDimensions = my numberOfEigenvalues;
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfDimensions);
		for (integer i = 1; i <= thy numberOfRows; i ++) { /* row */
			for (integer j = 1; j <= numberOfDimensions; j ++) {
				const longdouble sigma = sqrt (my eigenvalues [j]);
				longdouble r = 0.0;
				for (integer k = 1; k <= my dimension; k ++)
					// eigenvector in row, data in row
					r += my eigenvectors [j] [k] * (thy data [i] [k] - my centroid [k]) / sigma;
				his data [i] [j] = (double) r;
			}
		}
		his rowLabels.all() <<= thy rowLabels.all();
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal (zscores) not created from PCA & TableOfReal.");
	}
}

autoTableOfReal PCA_TableOfReal_to_TableOfReal_projectRows (PCA me, TableOfReal thee, integer numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep == 0 || numberOfDimensionsToKeep > my numberOfEigenvalues)
			numberOfDimensionsToKeep = my numberOfEigenvalues;

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfDimensionsToKeep);
		mul_MAT_out (his data.get(), thy data.get(), my eigenvectors.horizontalBand (1, numberOfDimensionsToKeep).transpose());
		his rowLabels.all() <<= thy rowLabels.all();
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from PCA & TableOfReal.");
	}
}

autoConfiguration PCA_TableOfReal_to_Configuration (PCA me, TableOfReal thee, integer numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep == 0 || numberOfDimensionsToKeep > my numberOfEigenvalues)
			numberOfDimensionsToKeep = my numberOfEigenvalues;

		autoConfiguration him = Configuration_create (thy numberOfRows, numberOfDimensionsToKeep);
		mul_MAT_out (his data.get(), thy data.get(), my eigenvectors.horizontalBand(1, numberOfDimensionsToKeep).transpose());
		his rowLabels.all() <<= thy rowLabels.all();
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created from PCA & TableOfReal.");
	}
}

autoTableOfReal PCA_Configuration_to_TableOfReal_reconstruct (PCA me, Configuration thee) {
	try {
		Melder_require (thy numberOfColumns <= my numberOfEigenvalues,
			U"The number of columns in the configuration should not exceed the number of eigenvectors (", my numberOfEigenvalues, U").");
		const integer numberOfEigenvectorsToUse = std::min (thy numberOfColumns, my numberOfEigenvalues);
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, my dimension);
		Melder_assert (my labels.size == my dimension);
		his columnLabels.all() <<= my labels.all();
		his rowLabels.all() <<= thy rowLabels.all();

		mul_MAT_out (his data.get(), thy data.get (), my eigenvectors.horizontalBand (1, numberOfEigenvectorsToUse));

		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not reconstructed.");
	}
}

double PCA_TableOfReal_getFractionVariance (PCA me, TableOfReal thee, integer from, integer to) {
	try {
		if (from < 1 || from > to || to > thy numberOfColumns)
			return undefined;
		autoSSCP s = TableOfReal_to_SSCP (thee, 0, 0, 0, 0);
		autoSSCP sp = Eigen_SSCP_project (me, s.get());
		const double fraction = SSCP_getFractionVariation (sp.get(), from, to);
		return fraction;
	} catch (MelderError) {
		return undefined;
	}
}

autoTableOfReal PCA_to_TableOfReal_reconstruct1 (PCA me, conststring32 coefficients) {
	try {
		autoVEC pc = newVECfromString (coefficients);
		Melder_require (pc.size == my numberOfEigenvalues,
			U"The number of coefficients should equal the number of eigenvectors (", my numberOfEigenvalues, U").");
		autoConfiguration c = Configuration_create (1, pc.size);
		c -> data.row (1) <<= pc.all();
		autoTableOfReal him = PCA_Configuration_to_TableOfReal_reconstruct (me, c.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" not reconstructed.");
	}
}

/* End of file PCA.cpp */
