/* Eigen_and_Procrustes.cpp
 * Copyright (C) 2004-2019 David Weenink
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

/* djmw 2004 Initial version */

#include "Eigen_and_Procrustes.h"
#include "Configuration_and_Procrustes.h"
#include "NUM2.h"

autoProcrustes Eigens_to_Procrustes (Eigen me, Eigen thee, integer evec_from, integer evec_to) {
	try {
		const integer numberOfVectors = evec_to - evec_from + 1;
		const integer nmin = std::min (my numberOfEigenvalues, thy numberOfEigenvalues );
		Melder_require (my dimension == thy dimension,
			U"The eigenvectors should have the same dimension.");
		Melder_require (evec_from <= evec_to && evec_from > 0 && evec_to <= nmin,
			U"Eigenvector range is too large.");

		autoProcrustes him = Procrustes_create (numberOfVectors);
		autoMAT rotation;
		NUMprocrustes (my eigenvectors.horizontalBand (evec_from, evec_to).transpose(),
			thy eigenvectors.horizontalBand (evec_from, evec_to).transpose(), & rotation, nullptr, nullptr);
		his r.all() <<= rotation.all();
		return him;
	} catch (MelderError) {
		Melder_throw (U"Procrustes not created from Eigens.");
	}
}

/* End of file Eigen_and_Procrustes.cpp */
