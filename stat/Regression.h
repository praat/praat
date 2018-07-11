#ifndef _Regression_h_
#define _Regression_h_
/* Regression.h
 *
 * Copyright (C) 2005-2011,2014,2015,2017 Paul Boersma
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

#include "Table.h"

#include "Regression_def.h"

void Regression_init (Regression me);
void Regression_addParameter (Regression me, conststring32 label, double minimum, double maximum, double value);
integer Regression_getFactorIndexFromFactorName_e (Regression me, conststring32 factorName);

Thing_define (LinearRegression, Regression) {
};

autoLinearRegression LinearRegression_create ();

autoLinearRegression Table_to_LinearRegression (Table me);

/* End of file Regression.h */
#endif
