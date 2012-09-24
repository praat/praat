#ifndef _Regression_h_
#define _Regression_h_
/* Regression.h
 *
 * Copyright (C) 2005-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Table.h"

#include "Regression_def.h"
oo_CLASS_CREATE (RegressionParameter, Data);
oo_CLASS_CREATE (Regression, Data);

void Regression_init (I);
void Regression_addParameter (I, const wchar_t *label, double minimum, double maximum, double value);
long Regression_getFactorIndexFromFactorName_e (I, const wchar_t *factorName);

Thing_define (LinearRegression, Regression) {
};

LinearRegression LinearRegression_create (void);

LinearRegression Table_to_LinearRegression (Table me);

/* End of file Regression.h */
#endif
