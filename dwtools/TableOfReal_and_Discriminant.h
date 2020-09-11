#ifndef _TableOfReal_and_Discriminant_h_
#define _TableOfReal_and_Discriminant_h_
/* TableOfReal_and_Discriminant.h
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "ClassificationTable.h"
#include "Configuration.h"
#include "Covariance.h"
#include "Discriminant.h"
#include "TableOfReal.h"

autoDiscriminant TableOfReal_to_Discriminant (TableOfReal me);

autoTableOfReal Discriminant_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, integer group, bool poolCovarianceMatrices); 

autoTableOfReal Discriminant_TableOfReal_mahalanobis_all (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices);

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities);

autoClassificationTable Discriminant_TableOfReal_to_ClassificationTable_dw (Discriminant me, TableOfReal thee, bool poolCovarianceMatrices, bool useAprioriProbabilities, double alpha, double minProb, autoTableOfReal *displacements);

autoConfiguration Discriminant_TableOfReal_to_Configuration (Discriminant me, TableOfReal thee, integer numberOfDimensions);

autoConfiguration TableOfReal_to_Configuration_lda (TableOfReal me, integer numberOfDimensions);

#endif /* _TableOfReal_and_Discriminant_h_ */
