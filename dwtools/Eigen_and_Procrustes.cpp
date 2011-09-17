/* Eigen_and_Procrustes.cpp
 * Copyright (C) 2005-2011 David Weenink
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

/* djmw 2004 Initial version */

#include "Eigen_and_Procrustes.h"
#include "Configuration_and_Procrustes.h"
#include "NUM2.h"

Procrustes Eigens_to_Procrustes (I, thou, long evec_from, long evec_to) {
	try {
		iam (Eigen); thouart (Eigen);
		long nvectors = evec_to - evec_from + 1;
		long nmin = my numberOfEigenvalues < thy numberOfEigenvalues ? my numberOfEigenvalues : thy numberOfEigenvalues;

		if (my dimension != thy dimension) {
			Melder_throw ("The eigenvectors must have the same dimension.");
		}

		if (evec_from > evec_to || evec_from < 1 || evec_to > nmin) {
			Melder_throw ("Eigenvector range too large.");
		}

		autoNUMmatrix<double> x (1, my dimension, 1, nvectors);
		autoNUMmatrix<double> y (1, my dimension, 1, nvectors);

		for (long j = 1; j <= nvectors; j++) {
			for (long i = 1; i <= my dimension; i++) {
				x[i][j] =  my eigenvectors[evec_from + j - 1][i];
				y[i][j] = thy eigenvectors[evec_from + j - 1][i];
			}
		}

		autoProcrustes him = Procrustes_create (nvectors);

		NUMProcrustes (x.peek(), y.peek(), my dimension, nvectors, his r, NULL, NULL);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Procrustes not created from Eigens.");
	}
}


/* End of file Eigen_and_Procrustes.cpp */
