#ifndef _Discriminant_h_
#define _Discriminant_h_
/* Discriminant.h
 *
 * Copyright (C) 1993-2011, 2015-2016 David Weenink
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Graphics.h"
#include "Configuration.h"
#include "ClassificationTable.h"
#include "Eigen.h"
#include "SSCP.h"

#include "Discriminant_def.h"

autoDiscriminant Discriminant_create (long numberOfGroups, long numberOfEigenvalues, long dimension);

long Discriminant_groupLabelToIndex (Discriminant me, const char32 *label);

void Discriminant_setAprioriProbability (Discriminant me, long group, double p);

long Discriminant_getNumberOfGroups (Discriminant me);

long Discriminant_getNumberOfObservations (Discriminant me, long group);

long Discriminant_getNumberOfFunctions (Discriminant me);

double Discriminant_getWilksLambda (Discriminant me, long numberOfDimensions);

void Discriminant_getPartialDiscriminationProbability (Discriminant me,
	long numberOfDimensions, double *probability, double *chisq, double *df);

double Discriminant_getConcentrationEllipseArea (Discriminant me, long group,
	double scale, bool confidence, int discriminantDirections, long d1, long d2);

double Discriminant_getLnDeterminant_group (Discriminant me, long group);

double Discriminant_getLnDeterminant_total (Discriminant me);

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g, int discriminantDirections,
	long d1, long d2, double xmin, double xmax, double ymin, double ymax, int fontSize,
	int poolCovarianceMatrices, int garnish);

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g,
	double scale, bool confidence, char32 *label, int discriminantDirections,
	long d1, long d2, double xmin, double xmax, double ymin, double ymax,
	int fontSize, int garnish);

autoTableOfReal Discriminant_extractCoefficients (Discriminant me, int choice);

autoTableOfReal Discriminant_extractGroupCentroids (Discriminant me);

autoTableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me);

autoSSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me);

autoSSCP Discriminant_extractWithinGroupSSCP (Discriminant me, long index);

autoSSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me);

autoStrings Discriminant_extractGroupLabels (Discriminant me);

void Discriminant_setGroupLabels (Discriminant me, Strings thee);

autoConfiguration Discriminant_and_TableOfReal_to_Configuration	(Discriminant me, TableOfReal thee, long numberOfDimensions);

autoClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable
	(Discriminant me, TableOfReal thee, int poolCovarianceMatrices,	int useAprioriProbabilities);

autoClassificationTable Discriminant_and_TableOfReal_to_ClassificationTable_dw
	(Discriminant me, TableOfReal thee, int poolCovarianceMatrices,	int useAprioriProbabilities, double alpha, double minProb, autoTableOfReal *displacements);

autoTableOfReal Discriminant_and_TableOfReal_mahalanobis (Discriminant me, TableOfReal thee, long group, bool poolCovarianceMatrices);
/* Mahalanobis distance with respect to group mean */

autoDiscriminant TableOfReal_to_Discriminant (TableOfReal me);

autoConfiguration TableOfReal_to_Configuration_lda (TableOfReal me, long numberOfDimensions);

#endif /* _Discriminant_h_ */
