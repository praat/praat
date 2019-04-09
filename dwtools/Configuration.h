#ifndef _Configuration_h_
#define _Configuration_h_
/* Configuration.h
 *
 * Copyright (C) 1992-2017 David Weenink
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

#include "Graphics.h"
#include "TableOfReal.h"
#include "Collection.h"

#include "Configuration_def.h"


#pragma mark - class Configuration

autoConfiguration Configuration_create (integer numberOfPoints, integer numberOfDimensions);

void Configuration_setMetric (Configuration me, integer metric);

void Configuration_setDefaultWeights (Configuration me);
/* All w[i] = 1 */

void Configuration_setSqWeights (Configuration me, const double weight[]);
/* All w[i] = sqrt (weight[i]) */

void Configuration_randomize (Configuration me);
/*
	new x[i][j] = randomUniform (-1,1)
*/

void Configuration_normalize (Configuration me, double variance, bool choice);
/*
	1. centre columns
	2.
	choice == !0 : each column
	       == 0 : normalize matrix
*/

void Configuration_rotate (Configuration me, integer dimension1, integer dimension2, double angle_degrees);
/*
	Precondition:
		dimension1 != dimension2
		1 <= dimension1, dimension2 <= my numberOfColumns
*/

void Configuration_invertDimension (Configuration me, int dimension);
/*
	Function:
		Invert one dimension.
		for (i=1; i <= my numberOfRows; i++)
			my data[i][dimension] = - my data[i][dimension];
*/

autoConfiguration Configuration_congruenceRotation (Configuration me, Configuration thee,
	integer maximumNumberOfIterations, double tolerance);
/*
	Rotate thee for maximum congruence. Algorithm:
	Henk Kiers & Patrick Groenen (1996), "A monotonically convergent algorithm for
		orthogonal congruence rotation", Psychometrika 61, 375-389.
*/

autoConfiguration Configuration_varimax (Configuration me, bool normalizeRows, bool quartimax, integer maximumNumberOfIterations, double tolerance);
/*
	Perform varimax rotation. Algorithm with extra security from:
	Jos Ten Berge (1995), "Suppressing permutations or rigid planar rotations:
		a remedy against nonoptimal varimax rotations", Psychometrika 60, 437-446.
*/

void Configuration_rotateToPrincipalDirections (Configuration me);

void Configuration_draw (Configuration me, Graphics g, int xCoordinate,
	int yCoordinate, double xmin, double xmax, double ymin, double ymax,
	int labelSize, bool useRowLabels, conststring32 label, bool garnish);

void Configuration_drawConcentrationEllipses (Configuration me, Graphics g,
	double scale, bool confidence, conststring32 label, integer d1, integer d2, double xmin, double xmax,
	double ymin, double ymax, double fontSize, bool garnish);

autoConfiguration TableOfReal_to_Configuration (TableOfReal me);

autoConfiguration TableOfReal_to_Configuration_pca (TableOfReal me, integer numberOfDimensions);
/*
	Precondition:
		numberOfDimensions > 0
	Function:
		principal component analysis
	Postcondition:
		(Configuration) numberOfColumns = MIN (my numberOfColumns, numberOfDimensions)
*/

autoConfiguration Configuration_createLetterRExample (int choice);
/*
  	Create a two-dimensional configuartion from the letter R.
  	choice = 1 : undistorted;
  	choice = 2 : result of monotone fit on distorted (d^2 + 5 +32.5*z)
*/

autoConfiguration Configuration_createCarrollWishExample ();


#pragma mark - class ConfigurationList

Collection_define (ConfigurationList, OrderedOf, Configuration) {
};

/* End of file Configuration.h */
#endif
