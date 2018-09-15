#ifndef _Configuration_AffineTransform_h_
#define _Configuration_AffineTransform_h_
/* Configuration_AffineTransform.c
 * 
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 20020315 GPL header
 djmw 20110307 Latest modification
 */

#include "Configuration.h"
#include "AffineTransform.h"

autoAffineTransform Configurations_to_AffineTransform_congruence (Configuration me,	Configuration thee, integer maximumNumberOfIterations, double tolerance);
/*
	Find the orthogonal rotation that maximizes the coefficients of congruence between the columns two configurations.
	
	Kiers & Groenen (1996), A monotonically convergent congruence algorithm for orthogona congruence rotation,
	Psychometrika (61), 375-389.
*/

autoConfiguration Configuration_AffineTransform_to_Configuration (Configuration me, AffineTransform thee);

#endif /* _Configuration_AffineTransform_h_ */
