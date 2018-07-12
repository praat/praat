#ifndef _LogisticRegression_h_
#define _LogisticRegression_h_
/* LogisticRegression.h
 *
 * Copyright (C) 2005-2011,2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Regression.h"

#include "LogisticRegression_def.h"

autoLogisticRegression LogisticRegression_create (conststring32 dependent1, conststring32 dependent2);

autoLogisticRegression Table_to_LogisticRegression (Table me, conststring32 columnsWithFactors_string,
	conststring32 columnWithDependent1_string, conststring32 columnWithDependent2_string);

void LogisticRegression_drawBoundary (LogisticRegression me, Graphics graphics, integer colx, double xmin, double xmax,
	integer coly, double ymin, double ymax, bool garnish);

/* End of file LogisticRegression.h */
#endif
