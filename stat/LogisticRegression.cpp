/* LogisticRegression.cpp
 *
 * Copyright (C) 2005-2012 Paul Boersma
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
 * pb 2007/10/01 can write as encoding
 * pb 2007/11/18 split off from Regression.c
 * pb 2011/03/20 C++
 */

#include "LogisticRegression.h"
#include "UnicodeData.h"

#include "oo_DESTROY.h"
#include "LogisticRegression_def.h"
#include "oo_COPY.h"
#include "LogisticRegression_def.h"
#include "oo_EQUAL.h"
#include "LogisticRegression_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LogisticRegression_def.h"
#include "oo_WRITE_TEXT.h"
#include "LogisticRegression_def.h"
#include "oo_WRITE_BINARY.h"
#include "LogisticRegression_def.h"
#include "oo_READ_TEXT.h"
#include "LogisticRegression_def.h"
#include "oo_READ_BINARY.h"
#include "LogisticRegression_def.h"
#include "oo_DESCRIPTION.h"
#include "LogisticRegression_def.h"

Thing_implement (LogisticRegression, Regression, 0);

void structLogisticRegression :: v_info () {
	LogisticRegression_Parent :: v_info ();
	MelderInfo_writeLine (L"Dependent 1: ", dependent1);
	MelderInfo_writeLine (L"Dependent 2: ", dependent2);
	MelderInfo_writeLine (L"Interpretation:");
	MelderInfo_write (L"   ln (P(", dependent2, L")/P(", dependent1, L")) " UNITEXT_ALMOST_EQUAL_TO L" ", Melder_fixed (intercept, 6));
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_write (parm -> value < 0.0 ? L" - " : L" + ", Melder_fixed (fabs (parm -> value), 6), L" * ", parm -> label);
	}
	MelderInfo_writeLine (NULL);
	MelderInfo_writeLine (L"Log odds ratios:");
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_writeLine (L"   Log odds ratio of factor ", parm -> label, L": ", Melder_fixed ((parm -> maximum - parm -> minimum) * parm -> value, 6));
	}
	MelderInfo_writeLine (L"Odds ratios:");
	for (long ivar = 1; ivar <= parameters -> size; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (parameters -> item [ivar]);
		MelderInfo_writeLine (L"   Odds ratio of factor ", parm -> label, L": ", Melder_double (exp ((parm -> maximum - parm -> minimum) * parm -> value)));
	}
}

LogisticRegression LogisticRegression_create (const wchar_t *dependent1, const wchar_t *dependent2) {
	try {
		autoLogisticRegression me = Thing_new (LogisticRegression);
		Regression_init (me.peek());
		my dependent1 = Melder_wcsdup (dependent1);
		my dependent2 = Melder_wcsdup (dependent2);	
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("LogisticRegression not created.");
	}
}

static LogisticRegression _Table_to_LogisticRegression (Table me, long *factors, long numberOfFactors, long dependent1, long dependent2) {
	long numberOfParameters = numberOfFactors + 1;
	long numberOfCells = my rows -> size, numberOfY0 = 0, numberOfY1 = 0, numberOfData = 0;
	double logLikelihood = 1e300, previousLogLikelihood = 2e300;
	if (numberOfParameters < 1)   // includes intercept
		Melder_throw ("Not enough columns (has to be more than 1).");
	/*
	 * Divide up the contents of the table into a number of independent variables (x) and two dependent variables (y0 and y1).
	 */
	autoNUMmatrix <double> x (1, numberOfCells, 0, numberOfFactors);   // column 0 is the intercept
	autoNUMvector <double> y0 (1, numberOfCells);
	autoNUMvector <double> y1 (1, numberOfCells);
	autoNUMvector <double> meanX (1, numberOfFactors);
	autoNUMvector <double> stdevX (1, numberOfFactors);
	autoNUMmatrix <double> smallMatrix (0, numberOfFactors, 0, numberOfParameters);
	autoLogisticRegression thee = LogisticRegression_create (my columnHeaders [dependent1]. label, my columnHeaders [dependent2]. label);
	for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
		double minimum = Table_getMinimum (me, factors [ivar]);
		double maximum = Table_getMaximum (me, factors [ivar]);
		Regression_addParameter (thee.peek(), my columnHeaders [factors [ivar]]. label, minimum, maximum, 0.0);
	}
	for (long icell = 1; icell <= numberOfCells; icell ++) {
		y0 [icell] = Table_getNumericValue_Assert (me, icell, dependent1);
		y1 [icell] = Table_getNumericValue_Assert (me, icell, dependent2);
		numberOfY0 += y0 [icell];
		numberOfY1 += y1 [icell];
		numberOfData += y0 [icell] + y1 [icell];
		x [icell] [0] = 1.0;   /* Intercept. */
		for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
			x [icell] [ivar] = Table_getNumericValue_Assert (me, icell, factors [ivar]);
			meanX [ivar] += x [icell] [ivar] * (y0 [icell] + y1 [icell]);
		}
	}
	if (numberOfY0 == 0 && numberOfY1 == 0)
		Melder_throw ("No data in either class. Cannot determine result.");
	if (numberOfY0 == 0)
		Melder_throw ("No data in class ", my columnHeaders [dependent1]. label, ". Cannot determine result.");
	if (numberOfY1 == 0)
		Melder_throw ("No data in class ", my columnHeaders [dependent2]. label, ". Cannot determine result.");
	/*
	 * Normalize the data.
	 */
	for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
		meanX [ivar] /= numberOfData;
		for (long icell = 1; icell <= numberOfCells; icell ++) {
			x [icell] [ivar] -= meanX [ivar];
		}
	}
	for (long icell = 1; icell <= numberOfCells; icell ++) {
		for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
			stdevX [ivar] += x [icell] [ivar] * x [icell] [ivar] * (y0 [icell] + y1 [icell]);
		}
	}
	for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
		stdevX [ivar] = sqrt (stdevX [ivar] / numberOfData);
		for (long icell = 1; icell <= numberOfCells; icell ++) {
			x [icell] [ivar] /= stdevX [ivar];
		}
	}
	/*
	 * Initial state of iteration: the null model.
	 */
	thy intercept = log ((double) numberOfY1 / (double) numberOfY0);   // initial state of intercept: best guess for average log odds
	for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (thy parameters -> item [ivar]);
		parm -> value = 0.0;   // initial state of dependence: none
	}
	long iteration = 1;
	for (; iteration <= 100; iteration ++) {
		previousLogLikelihood = logLikelihood;
		for (long ivar = 0; ivar <= numberOfFactors; ivar ++) {
			for (long jvar = ivar; jvar <= numberOfParameters; jvar ++) {
				smallMatrix [ivar] [jvar] = 0.0;
			}
		}
		/*
		 * Compute the current log likelihood.
		 */
		logLikelihood = 0.0;
		for (long icell = 1; icell <= numberOfCells; icell ++) {
			double fittedLogit = thy intercept, fittedP, fittedQ, fittedLogP, fittedLogQ, fittedPQ, fittedVariance;
			for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
				RegressionParameter parm = static_cast<RegressionParameter> (thy parameters -> item [ivar]);
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
			for (long ivar = 0; ivar <= numberOfFactors; ivar ++) {
				/*
				 * The last column gets the gradient of LL: dLL/da, dLL/db, dLL/dc.
				 */
				smallMatrix [ivar] [numberOfParameters] += x [icell] [ivar] * (y1 [icell] * fittedQ - y0 [icell] * fittedP);
				for (long jvar = ivar; jvar <= numberOfFactors; jvar ++) {
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
		for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
			for (long jvar = 0; jvar < ivar; jvar ++) {
				smallMatrix [ivar] [jvar] = smallMatrix [jvar] [ivar];
			}
		}
		/*
		 * Invert matrix in the simplest way, and shift and wipe the last column with it.
		 */
		for (long ivar = 0; ivar <= numberOfFactors; ivar ++) {
			double pivot = smallMatrix [ivar] [ivar];   /* Save diagonal. */
			smallMatrix [ivar] [ivar] = 1.0;
			for (long jvar = 0; jvar <= numberOfParameters; jvar ++) {
				smallMatrix [ivar] [jvar] /= pivot;
			}
			for (long jvar = 0; jvar <= numberOfFactors; jvar ++) {
				if (jvar != ivar) {
					double temp = smallMatrix [jvar] [ivar];
					smallMatrix [jvar] [ivar] = 0.0;
					for (long kvar = 0; kvar <= numberOfParameters; kvar ++) {
						smallMatrix [jvar] [kvar] -= temp * smallMatrix [ivar] [kvar];
					}
				}
			}
		}
		/*
		 * Update the parameters from the last column of smallMatrix.
		 */
		thy intercept += smallMatrix [0] [numberOfParameters];
		for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
			RegressionParameter parm = static_cast<RegressionParameter> (thy parameters -> item [ivar]);
			parm -> value += smallMatrix [ivar] [numberOfParameters];
		}
	}
	if (iteration > 100) {
		Melder_warning (L"Logistic regression has not converged in 100 iterations. The results are unreliable.");
	}
	for (long ivar = 1; ivar <= numberOfFactors; ivar ++) {
		RegressionParameter parm = static_cast<RegressionParameter> (thy parameters -> item [ivar]);
		parm -> value /= stdevX [ivar];
		thy intercept -= parm -> value * meanX [ivar];
	}
	return thee.transfer();
}

LogisticRegression Table_to_LogisticRegression (Table me, const wchar_t *factors_columnLabelString,
	const wchar_t *dependent1_columnLabel, const wchar_t *dependent2_columnLabel)
{
	try {
		long numberOfFactors;
		autoNUMvector <long> factors_columnIndices (Table_getColumnIndicesFromColumnLabelString (me, factors_columnLabelString, & numberOfFactors), 1);
		long dependent1_columnIndex = Table_getColumnIndexFromColumnLabel (me, dependent1_columnLabel);
		long dependent2_columnIndex = Table_getColumnIndexFromColumnLabel (me, dependent2_columnLabel);
		autoLogisticRegression thee = _Table_to_LogisticRegression (me, factors_columnIndices.peek(), numberOfFactors, dependent1_columnIndex, dependent2_columnIndex);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": logistic regression not performed.");
	}
}

static inline double NUMmin2 (double a, double b) {
	return a < b ? a : b;
}

static inline double NUMmax2 (double a, double b) {
	return a > b ? a : b;
}

void LogisticRegression_drawBoundary (LogisticRegression me, Graphics graphics, long colx, double xleft, double xright,
	long coly, double ybottom, double ytop, bool garnish)
{
	RegressionParameter parmx = static_cast<RegressionParameter> (my parameters -> item [colx]);
	RegressionParameter parmy = static_cast<RegressionParameter> (my parameters -> item [coly]);
	if (xleft == xright) {
		xleft = parmx -> minimum;
		xright = parmx -> maximum;
	}
	if (ybottom == ytop) {
		ybottom = parmy -> minimum;
		ytop = parmy -> maximum;
	}
	double intercept = my intercept;
	for (long iparm = 1; iparm <= my parameters -> size; iparm ++) {
		if (iparm != colx && iparm != coly) {
			RegressionParameter parm = static_cast<RegressionParameter> (my parameters -> item [iparm]);
			intercept += parm -> value * (0.5 * (parm -> minimum + parm -> maximum));
		}
	}
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, xleft, xright, ybottom, ytop);
	double xbottom = (intercept + parmy -> value * ybottom) / - parmx -> value;
	double xtop = (intercept + parmy -> value * ytop) / - parmx -> value;
	double yleft = (intercept + parmx -> value * xleft) / - parmy -> value;
	double yright = (intercept + parmx -> value * xright) / - parmy -> value;
	double xmin = NUMmin2 (xleft, xright), xmax = NUMmax2 (xleft, xright);
	double ymin = NUMmin2 (ybottom, ytop), ymax = NUMmax2 (ybottom, ytop);
	//Melder_casual ("LogisticRegression_drawBoundary: %f %f %f %f %f %f %f %f",
	//	xmin, xmax, xbottom, xtop, ymin, ymax, yleft, yright);
	if (xbottom >= xmin && xbottom <= xmax) {   // line goes through bottom?
		if (xtop >= xmin && xtop <= xmax)   // line goes through top?
			Graphics_line (graphics, xbottom, ybottom, xtop, ytop);   // draw from bottom to top
		else if (yleft >= ymin && yleft <= ymax)   // line goes through left?
			Graphics_line (graphics, xbottom, ybottom, xleft, yleft);   // draw from bottom to left
		else if (yright >= ymin && yright <= ymax)   // line goes through right?
			Graphics_line (graphics, xbottom, ybottom, xright, yright);   // draw from bottom to right
	} else if (yleft >= ymin && yleft <= ymax) {   // line goes through left?
		if (yright >= ymin && yright <= ymax)   // line goes through right?
			Graphics_line (graphics, xleft, yleft, xright, yright);   // draw from left to right
		else if (xtop >= xmin && xtop <= xmax)   // line goes through top?
			Graphics_line (graphics, xleft, yleft, xtop, ytop);   // draw from left to top
	} else if (xtop >= xmin && xtop <= xmax) {   // line goes through top?
		if (yright >= ymin && yright <= ymax)   // line goes through right?
			Graphics_line (graphics, xtop, ytop, xright, yright);   // draw from top to right
	}
	Graphics_unsetInner (graphics);
	if (garnish) {
		Graphics_drawInnerBox (graphics);
		Graphics_textBottom (graphics, true, parmx -> label);
		Graphics_marksBottom (graphics, 2, true, true, false);
		Graphics_textLeft (graphics, true, parmy -> label);
		Graphics_marksLeft (graphics, 2, true, true, false);
	}
}

/*
Table Table_LogisticRegression_addProbabilities (Table me, LogisticRegression thee) {
	for (icell = 1; icell <= numberOfCells; icell ++) {
		double fittedLogit = parameters [0], fittedP, fittedQ, fittedLogP, fittedLogQ;
		for (ivar = 1; ivar <= numberOfIndependentVariables; ivar ++) {
			fittedLogit += parameters [ivar] * Table_getNumericValue_Assert (me, icell, ivar);
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

/* End of file LogisticRegression.cpp */
