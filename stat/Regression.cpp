/* Regression.cpp
 *
 * Copyright (C) 2005-2011,2014,2015,2016,2017 Paul Boersma
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

Thing_implement (RegressionParameter, Daata, 0);

void structRegression :: v_info () {
	Regression_Parent :: v_info ();
	MelderInfo_writeLine (U"Factors:");
	MelderInfo_writeLine (U"   Number of factors: ", our parameters.size);
	for (integer ivar = 1; ivar <= our parameters.size; ivar ++) {
		RegressionParameter parm = our parameters.at [ivar];
		MelderInfo_writeLine (U"   Factor ", ivar, U": ", parm -> label.get());
	}
	MelderInfo_writeLine (U"Fitted coefficients:");
	MelderInfo_writeLine (U"   Intercept: ", intercept);
	for (integer ivar = 1; ivar <= our parameters.size; ivar ++) {
		RegressionParameter parm = our parameters.at [ivar];
		MelderInfo_writeLine (U"   Coefficient of factor ", parm -> label.get(), U": ", parm -> value);
	}
	MelderInfo_writeLine (U"Ranges of values:");
	for (integer ivar = 1; ivar <= our parameters.size; ivar ++) {
		RegressionParameter parm = our parameters.at [ivar];
		MelderInfo_writeLine (U"   Range of factor ", parm -> label.get(), U": minimum ",
			parm -> minimum, U", maximum ", parm -> maximum);
	}
}

Thing_implement (Regression, Daata, 0);

void Regression_init (Regression me) {
	//my parameters = Ordered_create ();
}

void Regression_addParameter (Regression me, conststring32 label, double minimum, double maximum, double value) {
	try {
		autoRegressionParameter thee = Thing_new (RegressionParameter);
		thy label = Melder_dup (label);
		thy minimum = minimum;
		thy maximum = maximum;
		thy value = value;
		my parameters.addItem_move (thee.move());
	} catch (MelderError) {
		Melder_throw (me, U": parameter not added.");
	}
}

integer Regression_getFactorIndexFromFactorName_e (Regression me, conststring32 factorName) {
	for (integer iparm = 1; iparm <= my parameters.size; iparm ++) {
		RegressionParameter parm = my parameters.at [iparm];
		if (Melder_equ (factorName, parm -> label.get()))
			return iparm;
	}
	Melder_throw (me, U" has no parameter named \"", factorName, U"\".");
}

Thing_implement (LinearRegression, Regression, 0);

autoLinearRegression LinearRegression_create () {
	try {
		autoLinearRegression me = Thing_new (LinearRegression);
		Regression_init (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"LinearRegression not created.");
	}
}

autoLinearRegression Table_to_LinearRegression (Table me) {
	try {
		const integer numberOfIndependentVariables = my numberOfColumns - 1, numberOfParameters = my numberOfColumns;
		if (numberOfParameters < 1)   // includes intercept
			Melder_throw (U"Not enough columns (has to be more than 1).");
		integer numberOfCells = my rows.size;
		if (numberOfCells == 0)
			Melder_throw (U"Not enough rows (0).");
		if (numberOfCells < numberOfParameters) {
			Melder_warning (U"Solution is not unique (more parameters than cases).");
		}
		autoMAT u = raw_MAT (numberOfCells, numberOfParameters);
		autoVEC b = raw_VEC (numberOfCells);
		autoLinearRegression thee = LinearRegression_create ();
		for (integer ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			const double minimum = Table_getMinimum (me, ivar);
			const double maximum = Table_getMaximum (me, ivar);
			Regression_addParameter (thee.get(), my columnHeaders [ivar]. label.get(), minimum, maximum, 0.0);
		}
		for (integer icell = 1; icell <= numberOfCells; icell ++) {
			for (integer ivar = 1; ivar < numberOfParameters; ivar ++) {
				u [icell] [ivar] = Table_getNumericValue_Assert (me, icell, ivar);
			}
			u [icell] [numberOfParameters] = 1.0;   // for the intercept
			b [icell] = Table_getNumericValue_Assert (me, icell, my numberOfColumns);   // the dependent variable
		}
		autoVEC x = newVECsolve (u.get(), b.get(), NUMeps * numberOfCells);
		thy intercept = x [numberOfParameters];
		for (integer ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			const RegressionParameter parm = thy parameters.at [ivar];
			parm -> value = x [ivar];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": linear regression not performed.");
	}
}

/* End of file Regression.cpp */
