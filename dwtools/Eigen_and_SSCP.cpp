/* Eigen_and_SSCP.c
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
 djmw 20020327
 djmw 20020813 GPL header
 djmw 20040219 Eigen_and_Covariance_project added.
*/

#include "Eigen_and_SSCP.h"

autoSSCP Eigen_SSCP_project (Eigen me, SSCP thee) {
	try {
		Melder_require (thy numberOfRows == my dimension,
			U"Dimensions don't agree.");
		autoSSCP him = SSCP_create (my numberOfEigenvalues);
		//Eigen_SSCP_into_SSCP_project (me, thee, him.get());
		MATmul3_XYsXt (his data.get(), my eigenvectors.get(), thy data.get());
		mul_VEC_out (his centroid.get(), my eigenvectors.get(), thy centroid.get());
		his numberOfObservations = SSCP_getNumberOfObservations (thee);
		return him;
	} catch (MelderError) {
		Melder_throw (U"SSCP not projected.");
	}
}

autoCovariance Eigen_Covariance_project (Eigen me, Covariance thee) {
	try {
		Melder_require (thy numberOfRows == my dimension,
			U"Dimensions should agree.");
		autoCovariance him = Covariance_create (my numberOfEigenvalues);
		MATmul3_XYsXt (his data.get(), my eigenvectors.get(), thy data.get());
		mul_VEC_out (his centroid.get(), my eigenvectors.get(), thy centroid.get());
		his numberOfObservations = SSCP_getNumberOfObservations (thee);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Covariance not projected.");
	}
}

/* End of file Eigen_and_SSCP.cpp */
