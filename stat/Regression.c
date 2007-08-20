/* Regression.c
 *
 * Copyright (C) 2005-2007 Paul Boersma
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

/*
 * pb 2005/05/01 created
 * pb 2006/12/10 MelderInfo
 * pb 2007/08/12 wchar_t
 */

#include "Regression.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Regression_def.h"
#include "oo_COPY.h"
#include "Regression_def.h"
#include "oo_EQUAL.h"
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

class_methods (RegressionParameter, Data)
	class_method_local (RegressionParameter, destroy)
	class_method_local (RegressionParameter, description)
	class_method_local (RegressionParameter, copy)
	class_method_local (RegressionParameter, equal)
	class_method_local (RegressionParameter, writeText)
	class_method_local (RegressionParameter, writeBinary)
	class_method_local (RegressionParameter, readText)
	class_method_local (RegressionParameter, readBinary)
class_methods_end

static void classRegression_info (I) {
	iam (Regression);
	long ivar;
	classData -> info (me);
	MelderInfo_writeLine2 (L"Intercept: ", Melder_double (my intercept));
	for (ivar = 1; ivar <= my parameters -> size; ivar ++) {
		RegressionParameter parm = my parameters -> item [ivar];
		MelderInfo_writeLine4 (L"Coefficient of independent variable ", parm -> label, L": ", Melder_double (parm -> value));
	}
}

class_methods (Regression, Data)
	class_method_local (Regression, destroy)
	class_method_local (Regression, description)
	class_method_local (Regression, copy)
	class_method_local (Regression, equal)
	class_method_local (Regression, writeText)
	class_method_local (Regression, writeBinary)
	class_method_local (Regression, readText)
	class_method_local (Regression, readBinary)
	class_method_local (Regression, info)
class_methods_end

int Regression_init (I) {
	iam (Regression);
	my parameters = Ordered_create (); cherror
end:
	iferror return 0;
	return 1;
}

int Regression_addParameter (I, const wchar_t *label, double value) {
	iam (Regression);
	RegressionParameter thee = new (RegressionParameter); cherror
	thy label = Melder_wcsdup (label); cherror
	thy value = value;
	Collection_addItem (my parameters, thee); cherror
end:
	iferror return 0;   /* BUG */
	return 1;
}

class_methods (LinearRegression, Regression)
class_methods_end

LinearRegression LinearRegression_create (void) {
	LinearRegression me = new (LinearRegression); cherror
	Regression_init (me); cherror
end:
	iferror forget (me);
	return me;
}

LinearRegression Table_to_LinearRegression (Table me) {
	long numberOfIndependentVariables = my numberOfColumns - 1, numberOfParameters = my numberOfColumns;
	long numberOfCells = my rows -> size, icell, ivar;
	double **u = NULL, *b = NULL, *x = NULL;
	LinearRegression thee = NULL;
	if (numberOfParameters < 1)   /* Includes intercept. */
		error1 (L"Not enough columns (has to be more than 1).")
	if (numberOfCells < numberOfParameters) {
		Melder_warning ("Solution is not unique (more parameters than cases).");
	}
	u = NUMdmatrix (1, numberOfCells, 1, numberOfParameters); cherror
	b = NUMdvector (1, numberOfCells); cherror
	x = NUMdvector (1, numberOfParameters); cherror
	thee = LinearRegression_create (); cherror
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		Regression_addParameter (thee, my columnHeaders [ivar]. label, 0.0); cherror
	}
	for (icell = 1; icell <= numberOfCells; icell ++) {
		for (ivar = 1; ivar < numberOfParameters; ivar ++) {
			u [icell] [ivar] = Table_getNumericValue (me, icell, ivar);
		}
		u [icell] [numberOfParameters] = 1.0;   /* For the intercept. */
		b [icell] = Table_getNumericValue (me, icell, my numberOfColumns);   /* The dependent variable. */
	}
	NUMsolveEquation_d (u, numberOfCells, numberOfParameters, b, NUMeps * numberOfCells, x);
	thy intercept = x [numberOfParameters];
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		RegressionParameter parm = thy parameters -> item [ivar];
		parm -> value = x [ivar];
	}
end:
	NUMdmatrix_free (u, 1, 1);
	NUMdvector_free (b, 1);
	NUMdvector_free (x, 1);
	iferror forget (thee);
	return thee;
}


class_methods (LogisticRegression, Regression)
class_methods_end

LogisticRegression LogisticRegression_create (void) {
	LogisticRegression me = new (LogisticRegression); cherror
	Regression_init (me); cherror
end:
	iferror forget (me);
	return me;
}

LogisticRegression Table_to_LogisticRegression (Table me) {
	long numberOfIndependentVariables = my numberOfColumns - 2, numberOfParameters = my numberOfColumns - 1;
	long numberOfCells = my rows -> size, icell, numberOfY0 = 0, numberOfY1 = 0, numberOfData = 0, ivar, jvar, kvar, iteration;
	double **x = NULL, *y0 = NULL, *y1 = NULL, *meanX = NULL, *stdevX = NULL;
	double logLikelihood = 1e300, previousLogLikelihood = 2e300;
	double **smallMatrix = NULL;
	LogisticRegression thee = NULL;
	if (numberOfParameters < 1)   /* Includes intercept. */
		error1 (L"Not enough columns (has to be more than 1).")
	/*
	 * Divide up the contents of the table into a number of independent variables (x) and two dependent variables (y0 and y1).
	 */
	x = NUMdmatrix (1, numberOfCells, 0, numberOfIndependentVariables); cherror   /* Column 0 is the intercept. */
	y0 = NUMdvector (1, numberOfCells); cherror
	y1 = NUMdvector (1, numberOfCells); cherror
	meanX = NUMdvector (1, numberOfIndependentVariables); cherror
	stdevX = NUMdvector (1, numberOfIndependentVariables); cherror
	smallMatrix = NUMdmatrix (0, numberOfIndependentVariables, 0, numberOfParameters); cherror
	thee = LogisticRegression_create (); cherror
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		Regression_addParameter (thee, my columnHeaders [ivar]. label, 0.0); cherror
	}
	for (icell = 1; icell <= numberOfCells; icell ++) {
		y0 [icell] = Table_getNumericValue (me, icell, numberOfIndependentVariables + 1);
		y1 [icell] = Table_getNumericValue (me, icell, numberOfIndependentVariables + 2);
		numberOfY0 += y0 [icell];
		numberOfY1 += y1 [icell];
		numberOfData += y0 [icell] + y1 [icell];
		x [icell] [0] = 1.0;   /* Intercept. */
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			x [icell] [ivar] = Table_getNumericValue (me, icell, ivar);
			meanX [ivar] += x [icell] [ivar] * (y0 [icell] + y1 [icell]);
		}
	}
	if (numberOfY0 == 0 && numberOfY1 == 0)
		error1 (L"No data in either class. Cannot determine result.")
	if (numberOfY0 == 0)
		error3 (L"No data in class ", my columnHeaders [numberOfIndependentVariables + 1]. label, L". Cannot determine result.")
	if (numberOfY1 == 0)
		error3 (L"No data in class ", my columnHeaders [numberOfIndependentVariables + 2]. label, L". Cannot determine result.")
	/*
	 * Normalize the data.
	 */
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		meanX [ivar] /= numberOfData;
		for (icell = 1; icell <= numberOfCells; icell ++) {
			x [icell] [ivar] -= meanX [ivar];
		}
	}
	for (icell = 1; icell <= numberOfCells; icell ++) {
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			stdevX [ivar] += x [icell] [ivar] * x [icell] [ivar] * (y0 [icell] + y1 [icell]);
		}
	}
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		stdevX [ivar] = sqrt (stdevX [ivar] / numberOfData);
		for (icell = 1; icell <= numberOfCells; icell ++) {
			x [icell] [ivar] /= stdevX [ivar];
		}
	}
	/*
	 * Initial state of iteration: the null model.
	 */
	thy intercept = log ((double) numberOfY1 / (double) numberOfY0);   /* Initial state of intercept: best guess for average log odds. */
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		RegressionParameter parm = thy parameters -> item [ivar];
		parm -> value = 0.0;   /* Initial state of dependence: none. */
	}
	for (iteration = 1; iteration <= 100; iteration ++) {
		previousLogLikelihood = logLikelihood;
		for (ivar = 0; ivar <= numberOfIndependentVariables; ivar ++) {
			for (jvar = ivar; jvar <= numberOfParameters; jvar ++) {
				smallMatrix [ivar] [jvar] = 0.0;
			}
		}
		/*
		 * Compute the current log likelihood.
		 */
		logLikelihood = 0.0;
		for (icell = 1; icell <= numberOfCells; icell ++) {
			double fittedLogit = thy intercept, fittedP, fittedQ, fittedLogP, fittedLogQ, fittedPQ, fittedVariance;
			for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
				RegressionParameter parm = thy parameters -> item [ivar];
				fittedLogit += parm -> value * x [icell] [ivar];
			}
			/*
			 * Basically we have fittedP = 1.0 / (1.0 + exp (- fittedLogit)),
			 * but that works neither for fittedP values near 0 nor for values near 1.
			 */
			if (fittedLogit > 15.0) {
				/*
				 * For large fittedLogit, fittedLogP = ln (1/(1+exp(-fittedLogit))) = -ln (1+exp(-fittedLogit)) =~ - exp(-fittedLogit)
				 */
				fittedLogP = - exp (- fittedLogit);
				fittedLogQ = - fittedLogit;
				fittedPQ = exp (- fittedLogit);
				fittedP = exp (fittedLogP);
				fittedQ = 1.0 - fittedP;
			} else if (fittedLogit < -15.0) {
				fittedLogP = fittedLogit;
				fittedLogQ = - exp (fittedLogit);
				fittedPQ = exp (fittedLogit);
				fittedP = exp (fittedLogP);
				fittedQ = 1 - fittedP;
			} else {
				fittedP = 1.0 / (1.0 + exp (- fittedLogit));
				fittedLogP = log (fittedP);
				fittedQ = 1.0 - fittedP;
				fittedLogQ = log (fittedQ);
				fittedPQ = fittedP * fittedQ;
			}
			logLikelihood += -2 * (y1 [icell] * fittedLogP + y0 [icell] * fittedLogQ);
			/*
			 * Matrix shifting stuff.
			 * Suppose a + b Sk + c Tk = ln (pk / qk),
			 * where {a, b, c} are the coefficients to be optimized,
			 * Sk and Tk are properties of stimulus k,
			 * and pk and qk are the fitted probabilities for y1 and y0, respectively, given stimulus k.
			 * Then ln pk = - ln (1 + qk / pk) = - ln (1 + exp (- (a + b Sk + c Tk)))
			 * d ln pk / da = 1 / (1 + exp (a + b Sk + c Tk)) = qk
			 * d ln pk / db = qk Sk
			 * d ln pk / dc = qk Tk
			 * d ln qk / da = - pk
			 * Now LL = Sum(k) (y1k ln pk + y0k ln qk)
			 * so that dLL/da = Sum(k) (y1k d ln pk / da + y0k ln qk / da) = Sum(k) (y1k qk - y0k pk)
			 */
			fittedVariance = fittedPQ * (y0 [icell] + y1 [icell]);
			for (ivar = 0; ivar <= numberOfIndependentVariables; ivar ++) {
				/*
				 * The last column gets the gradient of LL: dLL/da, dLL/db, dLL/dc.
				 */
				smallMatrix [ivar] [numberOfParameters] += x [icell] [ivar] * (y1 [icell] * fittedQ - y0 [icell] * fittedP);
				for (jvar = ivar; jvar <= numberOfIndependentVariables; jvar ++) {
					smallMatrix [ivar] [jvar] += x [icell] [ivar] * x [icell] [jvar] * fittedVariance;
				}
			}
		}
		if (fabs (logLikelihood - previousLogLikelihood) < 1e-11) {
			break;
		}
		/*
		 * Make matrix symmetric.
		 */
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			for (jvar = 0; jvar < ivar; jvar ++) {
				smallMatrix [ivar] [jvar] = smallMatrix [jvar] [ivar];
			}
		}
		/*
		 * Invert matrix in the simplest way, and shift and wipe the last column with it.
		 */
		for (ivar = 0; ivar <= numberOfIndependentVariables; ivar ++) {
			double pivot = smallMatrix [ivar] [ivar];   /* Save diagonal. */
			smallMatrix [ivar] [ivar] = 1.0;
			for (jvar = 0; jvar <= numberOfParameters; jvar ++) {
				smallMatrix [ivar] [jvar] /= pivot;
			}
			for (jvar = 0; jvar <= numberOfIndependentVariables; jvar ++) {
				if (jvar != ivar) {
					double temp = smallMatrix [jvar] [ivar];
					smallMatrix [jvar] [ivar] = 0.0;
					for (kvar = 0; kvar <= numberOfParameters; kvar ++) {
						smallMatrix [jvar] [kvar] -= temp * smallMatrix [ivar] [kvar];
					}
				}
			}
		}
		/*
		 * Update the parameters from the last column of smallMatrix.
		 */
		thy intercept += smallMatrix [0] [numberOfParameters];
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			RegressionParameter parm = thy parameters -> item [ivar];
			parm -> value += smallMatrix [ivar] [numberOfParameters];
		}
	}
	if (iteration > 100) {
		Melder_warning ("Logistic regression has not converged in 100 iterations. The results are unreliable.");
	}
	for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
		RegressionParameter parm = thy parameters -> item [ivar];
		parm -> value /= stdevX [ivar];
		thy intercept -= parm -> value * meanX [ivar];
	}
end:
	NUMdmatrix_free (x, 1, 0);
	NUMdvector_free (y0, 1);
	NUMdvector_free (y1, 1);
	NUMdvector_free (meanX, 1);
	NUMdvector_free (stdevX, 1);
	NUMdmatrix_free (smallMatrix, 0, 0);
	iferror forget (thee);
	return thee;
}

/*
Table Table_LogisticRegression_addProbabilities (Table me, LogisticRegression thee) {
	for (icell = 1; icell <= numberOfCells; icell ++) {
		double fittedLogit = parameters [0], fittedP, fittedQ, fittedLogP, fittedLogQ;
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			fittedLogit += parameters [ivar] * Table_getNumericValue (me, icell, ivar);
		}
		if (fittedLogit > 15.0) {
			fittedLogP = - exp (- fittedLogit);
			fittedLogQ = - fittedLogit;
			fittedP = exp (fittedLogP);
			fittedQ = 1.0 - fittedP;
		} else if (fittedLogit < -15.0) {
			fittedLogP = fittedLogit;
			fittedLogQ = - exp (fittedLogit);
			fittedP = exp (fittedLogP);
			fittedQ = 1 - fittedP;
		} else {
			fittedP = 1.0 / (1.0 + exp (- fittedLogit));
			fittedLogP = log (fittedP);
			fittedQ = 1.0 - fittedP;
			fittedLogQ = log (fittedQ);
		}
		Table_setNumericValue (thee, icell, numberOfIndependentVariables + 1, fittedQ);
		Table_setNumericValue (thee, icell, numberOfIndependentVariables + 2, fittedP);
	}
}
*/

/* End of file Regression.c */
