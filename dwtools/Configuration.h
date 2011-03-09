#ifndef _Configuration_h_
#define _Configuration_h_
/* Configuration.h
 * 
 * Copyright (C) 1992-2011 David Weenink
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

/*
 djmw 20020315 GPL header
 djmw 20110306 Latest modification.
 */

#ifndef _Data_h_
	#include "Data.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#include "Configuration_def.h"
#define Configuration_methods TableOfReal_methods
oo_CLASS_CREATE (Configuration, TableOfReal);

Configuration Configuration_create (long numberOfPoints, long numberOfDimensions);
 
void Configuration_setMetric (Configuration me, long metric);

void Configuration_setDefaultWeights (Configuration me);
/* All w[i] = 1 */

void Configuration_setSqWeights (Configuration me, const double weight[]);
/* All w[i] = sqrt (weight[i]) */

void Configuration_randomize (Configuration me);
/*
	new x[i][j] = randomUniform (-1,1)
*/

void Configuration_normalize (Configuration me, double variance, int choice);
/*
	1. centre columns
	2. 
	choice == !0 : each column
	       == 0 : normalize matrix
*/

void Configuration_rotate (Configuration me, long dimension1, long dimension2, double angle_degrees);
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

Configuration Configuration_congruenceRotation (Configuration me, Configuration thee,
	long maximumNumberOfIterations, double tolerance);
/*
	Rotate thee for maximum congruence. Algorithm:
	Henk Kiers & Patrick Groenen (1996), "A monotonically convergent algorithm for
		orthogonal congruence rotation", Psychometrika 61, 375-389.
*/

Configuration Configuration_varimax (Configuration me, int normalizeRows,
	int quartimax, long maximumNumberOfIterations, double tolerance);
/*
	Perform varimax rotation. Algorithm with extra security from:
	Jos Ten Berge (1995), "Suppressing permutations or rigid planar rotations:
		a remedy against nonoptimal varimax rotations", Psychometrika 60, 437-446.
*/

int Configuration_rotateToPrincipalDirections (Configuration me);

void Configuration_draw (Configuration me, Graphics g, int xCoordinate, 
	int yCoordinate, double xmin, double xmax, double ymin, double ymax, 
	int labelSize, int useRowLabels, wchar_t *label, int garnish);
	
void Configuration_drawConcentrationEllipses (Configuration me, Graphics g, 
	double scale, int confidence, wchar_t *label, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int garnish);

Configuration TableOfReal_to_Configuration (I);

Configuration TableOfReal_to_Configuration_pca (TableOfReal me, 
	long numberOfDimensions);
/*
	Precondition:
		numberOfDimensions > 0
	Function:
		principal component analysis
	Postcondition:
		(Configuration) numberOfColumns = MIN (my numberOfColumns, numberOfDimensions)
*/

Configuration Configuration_createLetterRExample (int choice);
/*
  	Create a two-dimensional configuartion from the letter R.
  	choice = 1 : undistorted;
  	choice = 2 : result of monotone fit on distorted (d^2 + 5 +32.5*z)
*/

Configuration Configuration_createCarrollWishExample (void);


/************************** class Configurations **************************************/

#define Configurations_members Ordered_members
#define Configurations_methods Ordered_methods
class_create (Configurations, Ordered);

Configurations Configurations_create (void);

#ifdef __cplusplus
	}
#endif

#endif /* _Configuration_h_ */
