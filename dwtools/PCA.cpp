/* PCA.cpp
 *
 * Principal Component Analysis
 *
 * Copyright (C) 1993-2012, 2015-2016 David Weenink
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
 djmw 20000511 PCA_and_TableOfReal_to_Configuration: added centralize option.
 	(later removed)
 djmw 20020327 PCA_and_TableOfReal_to_Configuration modified internals.
 djmw 20020418 Removed some causes for compiler warnings.
 djmw 20020502 modified call Eigen_and_TableOfReal_project_into.
 djmw 20030324 Added PCA_and_TableOfReal_getFractionVariance.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20081119 Check in TableOfReal_to_PCA if TableOfReal_areAllCellsDefined
  djmw 20110304 Thing_new
*/

#include "Configuration.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Matrix_extensions.h"
#include "NUMlapack.h"
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

autoPCA PCA_create (long numberOfComponents, long dimension) {
	try {
		autoPCA me = Thing_new (PCA);
		Eigen_init (me.get(), numberOfComponents, dimension);
		my labels = NUMvector<char32 *> (1, dimension);
		my centroid = NUMvector<double> (1, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PCA not created");
	}
}

void PCA_setNumberOfObservations (PCA me, long numberOfObservations) {
	my numberOfObservations = numberOfObservations;
}

long PCA_getNumberOfObservations (PCA me) {
	return my numberOfObservations;
}

void PCA_getEqualityOfEigenvalues (PCA me, long from, long to, int conservative, double *p_prob, double *p_chisq, double *p_df) {
	double sum = 0.0, sumln = 0.0;

	double prob = undefined, df = undefined, chisq = undefined;
	
	if (from == 0 && to == 0) {
		to = 1;
		from = my numberOfEigenvalues;
	}
	if (from < to && from > 0 && to <= my numberOfEigenvalues) {
		long i;
		for (i = from; i <= to; i ++) {
			if (my eigenvalues [i] <= 0) {
				break;
			}
			sum += my eigenvalues [i];
			sumln += log (my eigenvalues [i]);
		}
		if (sum == 0.0) {
			return;
		}
		long r = i - from;
		long n = my numberOfObservations - 1;
		if (conservative) {
			n -= from + (r * (2 * r + 1) + 2) / (6 * r);
		}

		df = r * (r + 1) / 2 - 1;
		chisq = n * (r * log (sum / r) - sumln);
		prob = NUMchiSquareQ (chisq, df);
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

/* the low level routines 
 * 
 * The matrix M[numberOfRows, numberOfColumns] is interpreted as 'numberOfRows' vectors of dimension 'numberOfColumns'
 * The eigenstructure of the M'M will be calculated
 * 
 */

autoEigen PCA_to_Eigen (PCA me) {
	try {
		autoEigen thee = Eigen_create (my numberOfEigenvalues, my dimension);
		NUMmatrix_copyElements <double>(my eigenvectors, thy eigenvectors, 1, my numberOfEigenvalues, 1, my dimension);
		NUMvector_copyElements<double>(my eigenvalues, thy eigenvalues, 1, my numberOfEigenvalues);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Eigen created.");
	}
}

static autoPCA NUMdmatrix_to_PCA (double **m, long numberOfRows, long numberOfColumns, bool byColumns) {
	try {
		if (! NUMdmatrix_hasFiniteElements(m, 1, numberOfRows, 1, numberOfColumns)) {
			Melder_throw (U"At least one of the matrix elements is not finite or undefined.");
		}
		if (NUMfrobeniusnorm (numberOfRows, numberOfColumns, m) == 0.0) {
			Melder_throw (U"All values in your table are zero.");
		}
		autoNUMmatrix<double> mcopy;
		long numberOfRows2, numberOfColumns2;
		if (byColumns) {
			if (numberOfColumns < numberOfRows) {
				Melder_warning (U"The number of columns in your table is less than the number of rows. ");
			}
			numberOfRows2 = numberOfColumns, numberOfColumns2 = numberOfRows;
			mcopy.reset (1, numberOfRows2, 1, numberOfColumns2);
			for (long i = 1; i <= numberOfRows2; i ++) { // transpose
				for (long j = 1; j <= numberOfColumns2; j++) {
					mcopy [i] [j] = m [j] [i];
				}
			}
		} else {
			if (numberOfRows < numberOfColumns) {
				Melder_warning (U"The number of rows in your table is less than the number of columns. ");
			}
			numberOfRows2 = numberOfRows, numberOfColumns2 = numberOfColumns;
			mcopy.reset (1, numberOfRows2, 1, numberOfColumns2);
			NUMmatrix_copyElements<double>(m, mcopy.peek(), 1, numberOfRows2, 1, numberOfColumns2);
		}
		
		autoPCA thee = Thing_new (PCA);
		thy centroid = NUMvector<double> (1, numberOfColumns2);
		NUMcentreColumns (mcopy.peek(), 1, numberOfRows2, 1, numberOfColumns2, thy centroid);
		Eigen_initFromSquareRoot (thee.get(), mcopy.peek(), numberOfRows2, numberOfColumns2);
		thy labels = NUMvector<char32 *> (1, numberOfColumns2);

		PCA_setNumberOfObservations (thee.get(), numberOfRows2);

		/*
			The covariance matrix C = A'A / (N-1). However, we have calculated
			the eigenstructure for A'A. This has no consequences for the
			eigenvectors, but the eigenvalues have to be divided by (N-1).
		*/

		for (long i = 1; i <= thy numberOfEigenvalues; i++) {
			thy eigenvalues [i] /= (numberOfRows2 - 1);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No PCA created from ", byColumns ? U"columns." : U"rows.");
	}	
}

autoPCA TableOfReal_to_PCA_byRows (TableOfReal me) {
	try {
		autoPCA thee = NUMdmatrix_to_PCA (my data, my numberOfRows, my numberOfColumns, false);
		NUMstrings_copyElements (my columnLabels, thy labels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": PCA not created.");
	}
}

autoPCA Matrix_to_PCA_byColumns (Matrix me) {
	try {
		autoPCA thee = NUMdmatrix_to_PCA (my z, my ny, my nx, true);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA created from columns.");
	}
}

autoPCA Matrix_to_PCA_byRows (Matrix me) {
	try {
		autoPCA thee = NUMdmatrix_to_PCA (my z, my ny, my nx, false);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA created from rows.");
	}
}

autoTableOfReal PCA_and_TableOfReal_to_TableOfReal_zscores (PCA me, TableOfReal thee, long numberOfDimensions) {
	try {
		if (numberOfDimensions == 0 || numberOfDimensions > my numberOfEigenvalues) {
			numberOfDimensions = my numberOfEigenvalues;
		}
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfDimensions);
		for (long i = 1; i <= thy numberOfRows; i++) { /* row */
			for (long j = 1; j <= numberOfDimensions; j++) {
				double r = 0, sigma = sqrt (my eigenvalues[j]);
				for (long k = 1; k <= my dimension; k++) {
					// eigenvector in row, data in row
					r += my eigenvectors[j][k] * (thy data[i][k] - my centroid[k]) / sigma;
				}
				his data[i][j] = r;
			}
		}
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal (zscores) not created from PCA & TableOfReal.");
	}
}

autoTableOfReal PCA_and_TableOfReal_to_TableOfReal_projectRows (PCA me, TableOfReal thee, long numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep == 0 || numberOfDimensionsToKeep > my numberOfEigenvalues) {
			numberOfDimensionsToKeep = my numberOfEigenvalues;
		}

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfDimensionsToKeep);
		Eigen_and_TableOfReal_into_TableOfReal_projectRows (me, thee, 1, him.get(), 1, numberOfDimensionsToKeep);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from PCA & TableOfReal.");
	}
}

autoConfiguration PCA_and_TableOfReal_to_Configuration (PCA me, TableOfReal thee, long numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep == 0 || numberOfDimensionsToKeep > my numberOfEigenvalues) {
			numberOfDimensionsToKeep = my numberOfEigenvalues;
		}
		autoConfiguration him = Configuration_create (thy numberOfRows, numberOfDimensionsToKeep);
		Eigen_and_TableOfReal_into_TableOfReal_projectRows (me, thee, 1, him.get(), 1, numberOfDimensionsToKeep);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, U"pc", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created from PCA & TableOfReal.");
	}
}

autoTableOfReal PCA_and_Configuration_to_TableOfReal_reconstruct (PCA me, Configuration thee) {
	try {
		long npc = thy numberOfColumns;

		if (thy numberOfColumns > my dimension) {
			Melder_throw (U"The dimension of the Configuration must be less than or equal to the dimension of the PCA.");
		}

		if (npc > my numberOfEigenvalues) {
			npc = my numberOfEigenvalues;
		}

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, my dimension);
		NUMstrings_copyElements (my labels, his columnLabels, 1, my dimension);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);

		for (long i = 1; i <= thy numberOfRows; i++) {
			double *hisdata = his data[i];
			for (long k = 1; k <= npc; k++) {
				double *evec = my eigenvectors[k], pc = thy data[i][k];
				for (long j = 1; j <= my dimension; j++) {
					hisdata[j] += pc * evec[j];
				}
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not reconstructed.");
	}
}

double PCA_and_TableOfReal_getFractionVariance (PCA me, TableOfReal thee, long from, long to) {
	try {
		double fraction = undefined;

		if (from < 1 || from > to || to > thy numberOfColumns) {
			return undefined;
		}

		autoSSCP s = TableOfReal_to_SSCP (thee, 0, 0, 0, 0);
		autoSSCP sp = Eigen_and_SSCP_project (me, s.get());
		fraction = SSCP_getFractionVariation (sp.get(), from, to);
		return fraction;
	} catch (MelderError) {
		return undefined;
	}
}

autoTableOfReal PCA_to_TableOfReal_reconstruct1 (PCA me, char32 *numstring) {
	try {
		long npc;
		autoNUMvector<double> pc (NUMstring_to_numbers (numstring, & npc), 1);

		autoConfiguration c = Configuration_create (1, npc);
		for (long j = 1; j <= npc; j++) {
			c -> data [1][j] = pc[j];
		}
		autoTableOfReal him = PCA_and_Configuration_to_TableOfReal_reconstruct (me, c.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" not reconstructed.");
	}
}

/* End of file PCA.cpp */
