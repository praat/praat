/* Eigen_and_SSCP.c
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20020327
 djmw 20020813 GPL header
 djmw 20040219 Eigen_and_Covariance_project added.
*/

#include "Eigen_and_SSCP.h"

static void Eigen_SSCP_into_SSCP_project (Eigen me, SSCP thee, SSCP him) {
	for (integer i = 1; i <= my numberOfEigenvalues; i ++) {
		for (integer j = i; j <= my numberOfEigenvalues; j ++) {
			double tmp = 0;
			for (integer k = 1; k <= my dimension; k ++) {
				for (integer m = 1; m <= my dimension; m ++) {
					tmp += my eigenvectors [i] [k] * thy data [k] [m] * my eigenvectors [j] [m];
				}
			}
			his data [i] [j] = his data [j] [i] = tmp;
		}

		double tmp = 0;
		for (integer m = 1; m <= my dimension; m ++) {
			tmp += thy centroid [m] * my eigenvectors [i] [m];
		}
		his centroid [i] = tmp;
	}
	his numberOfObservations = SSCP_getNumberOfObservations (thee);
}


autoSSCP Eigen_SSCP_project (Eigen me, SSCP thee) {
	try {
		Melder_require (thy numberOfRows == my dimension, U"Dimensions don't agree.");
		autoSSCP him = SSCP_create (my numberOfEigenvalues);
		Eigen_SSCP_into_SSCP_project (me, thee, him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"SSCP not projected.");
	}
}

autoCovariance Eigen_Covariance_project (Eigen me, Covariance thee) {
	try {
		Melder_require (thy numberOfRows == my dimension, U"Dimensions should agree.");
		autoCovariance him = Covariance_create (my numberOfEigenvalues);
		Eigen_SSCP_into_SSCP_project (me, thee, him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Covariance not projected.");
	}
}

/* End of file Eigen_and_SSCP.cpp */
