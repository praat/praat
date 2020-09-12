#ifndef _Discriminant_h_
#define _Discriminant_h_
/* Discriminant.h
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

#include "Eigen.h"
#include "Graphics.h"
#include "SSCP.h"

#include "Discriminant_def.h"

autoDiscriminant Discriminant_create (integer numberOfGroups, integer numberOfEigenvalues, integer dimension);

integer Discriminant_groupLabelToIndex (Discriminant me, conststring32 label);

void Discriminant_setAprioriProbability (Discriminant me, integer group, double p);

integer Discriminant_getNumberOfGroups (Discriminant me);

integer Discriminant_getNumberOfObservations (Discriminant me, integer group);

integer Discriminant_getNumberOfFunctions (Discriminant me);

double Discriminant_getWilksLambda (Discriminant me, integer numberOfDimensions);

void Discriminant_getPartialDiscriminationProbability (Discriminant me,
	integer numberOfDimensions, double *out_probability, double *out_chisq, double *out_df);

double Discriminant_getConcentrationEllipseArea (Discriminant me, integer group,
	double scale, bool confidence, bool discriminantDirections, integer d1, integer d2);

double Discriminant_getLnDeterminant_group (Discriminant me, integer group);

double Discriminant_getLnDeterminant_total (Discriminant me);

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g, bool discriminantDirections,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize,
	bool poolCovarianceMatrices, bool garnish);

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g,
	double scale, bool confidence, conststring32 label, bool discriminantDirections,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax,
	double fontSize, bool garnish);

autoTableOfReal Discriminant_extractCoefficients (Discriminant me, integer choice);

autoTableOfReal Discriminant_extractGroupCentroids (Discriminant me);

autoTableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me);

autoSSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me);

autoSSCP Discriminant_extractWithinGroupSSCP (Discriminant me, integer index);

autoSSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me);

autoStrings Discriminant_extractGroupLabels (Discriminant me);

void Discriminant_setGroupLabels (Discriminant me, Strings thee);

#endif /* _Discriminant_h_ */
