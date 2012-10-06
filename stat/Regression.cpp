/* Regression.cpp
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

#include "Regression.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Regression_def.h"
#include "oo_COPY.h"
#include "Regression_def.h"
#include "oo_EQUAL.h"
#include "Regression_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Regression_def.h"
#include "oo_WRITE_TEXT.h"
#include "Regression_def.h"
#include "oo_WRITE_BINARY.h"
#include "Regression_def.h"
#include "oo_READ_TEXT.h"
#include "Regression_def.h"
#include "oo_READ_BINARY.h"
#include "Regression_def.h"
#include "oo_DESCRIPTION.h"
#include "Regression_def.h"

Thing_implement (RegressionParameter, Data, 0);

void structRegression :: v_info () {
	Regression_Parent :: v_info ();
	MelderInfo_writeLine (L"Factors:");
	MelderInfo_writeLine (L"   Number of factors: ", Melder_integer (parameters -> size));
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_writeLine (L"   Factor ", Melder_integer (ivar), L": ", parm -> label);
	}
	MelderInfo_writeLine (L"Fitted coefficients:");
	MelderInfo_writeLine (L"   Intercept: ", Melder_double (intercept));
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_writeLine (L"   Coefficient of factor ", parm -> label, L": ", Melder_double (parm -> value));
	}
	MelderInfo_writeLine (L"Ranges of values:");
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_writeLine (L"   Range of factor ", parm -> label, L": minimum ",
			Melder_double (parm -> minimum), L", maximum ", Melder_double (parm -> maximum));
	}
}

Thing_implement (Regression, Data, 0);

void Regression_init (I) {
	iam (Regression);
	my parameters = Ordered_create ();
}

void Regression_addParameter (I, const wchar_t *label, double minimum, double maximum, double value) {
	iam (Regression);
	try {
		autoRegressionParameter thee = Thing_new (RegressionParameter);
		thy label = Melder_wcsdup (label);
		thy minimum = minimum;
		thy maximum = maximum;
		thy value = value;
		Collection_addItem (my parameters, thee.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": parameter not added.");
	}
}

long Regression_getFactorIndexFromFactorName_e (I, const wchar_t *factorName) {
	iam (Regression);
	for (long iparm = 1; iparm <= my parameters -> size; iparm ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (my parameters -> item [iparm]);
		if (Melder_wcsequ (factorName, parm -> label)) return iparm;
	}
	Melder_throw (Thing_messageName (me), L" has no parameter named \"", factorName, L"\".");
}

Thing_implement (LinearRegression, Regression, 0);

LinearRegression LinearRegression_create (void) {
	try {
		autoLinearRegression me = Thing_new (LinearRegression);
		Regression_init (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("LinearRegression not created.");
	}
}

LinearRegression Table_to_LinearRegression (Table me) {
	try {
		long numberOfIndependentVariables = my numberOfColumns - 1, numberOfParameters = my numberOfColumns;
		long numberOfCells = my rows -> size, icell, ivar;
		if (numberOfParameters < 1)   /* Includes intercept. */
			Melder_throw ("Not enough columns (has to be more than 1).");
		if (numberOfCells < numberOfParameters) {
			Melder_warning (L"Solution is not unique (more parameters than cases).");
		}
		autoNUMmatrix <double> u (1, numberOfCells, 1, numberOfParameters);
		autoNUMvector <double> b (1, numberOfCells);
		autoNUMvector <double> x (1, numberOfParameters);
		autoLinearRegression thee = LinearRegression_create ();
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			double minimum = Table_getMinimum (me, ivar);
			double maximum = Table_getMaximum (me, ivar);
			Regression_addParameter (thee.peek(), my columnHeaders [ivar]. label, minimum, maximum, 0.0);
		}
		for (icell = 1; icell <= numberOfCells; icell ++) {
			for (ivar = 1; ivar < numberOfParameters; ivar ++) {
				u [icell] [ivar] = Table_getNumericValue_Assert (me, icell, ivar);
			}
			u [icell] [numberOfParameters] = 1.0;   /* For the intercept. */
			b [icell] = Table_getNumericValue_Assert (me, icell, my numberOfColumns);   /* The dependent variable. */
		}
		NUMsolveEquation (u.peek(), numberOfCells, numberOfParameters, b.peek(), NUMeps * numberOfCells, x.peek());
		thy intercept = x [numberOfParameters];
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			RegressionParameter parm = static_cast<RegressionParameter> (thy parameters -> item [ivar]);
			parm -> value = x [ivar];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": linear regression not performed.");
	}
}

/* End of file Regression.cpp */
