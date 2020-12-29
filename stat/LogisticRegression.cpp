/* LogisticRegression.cpp
 *
 * Copyright (C) 2005-2012,2015-2020 Paul Boersma
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

#include "LogisticRegression.h"
#include "../kar/UnicodeData.h"

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
	MelderInfo_writeLine (U"Dependent 1: ", our dependent1.get());
	MelderInfo_writeLine (U"Dependent 2: ", our dependent2.get());
	MelderInfo_writeLine (U"Interpretation:");
	MelderInfo_write (U"   ln (P(", our dependent2.get(), U")/P(", our dependent1.get(), U")) " UNITEXT_ALMOST_EQUAL_TO U" ", Melder_fixed (intercept, 6));
	for (integer ivar = 1; ivar <= parameters.size; ivar ++) {
		RegressionParameter parm = parameters.at [ivar];
		MelderInfo_write (parm -> value < 0.0 ? U" - " : U" + ", Melder_fixed (fabs (parm -> value), 6), U" * ", parm -> label.get());
	}
	MelderInfo_writeLine (U"");
	MelderInfo_writeLine (U"Log odds ratios:");
	for (integer ivar = 1; ivar <= parameters.size; ivar ++) {
		RegressionParameter parm = parameters.at [ivar];
		MelderInfo_writeLine (U"   Log odds ratio of factor ", parm -> label.get(), U": ", Melder_fixed ((parm -> maximum - parm -> minimum) * parm -> value, 6));
	}
	MelderInfo_writeLine (U"Odds ratios:");
	for (integer ivar = 1; ivar <= parameters.size; ivar ++) {
		RegressionParameter parm = parameters.at [ivar];
		MelderInfo_writeLine (U"   Odds ratio of factor ", parm -> label.get(), U": ", exp ((parm -> maximum - parm -> minimum) * parm -> value));
	}
}

autoLogisticRegression LogisticRegression_create (conststring32 dependent1, conststring32 dependent2) {
	try {
		autoLogisticRegression me = Thing_new (LogisticRegression);
		Regression_init (me.get());
		my dependent1 = Melder_dup (dependent1);
		my dependent2 = Melder_dup (dependent2);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LogisticRegression not created.");
	}
}

static autoLogisticRegression _Table_to_LogisticRegression (Table me, constINTVEC factors, integer dependent1, integer dependent2) {
	const integer numberOfFactors = factors.size;
	const integer numberOfParameters = numberOfFactors + 1;
	const integer numberOfCells = my rows.size;
	integer numberOfY0 = 0, numberOfY1 = 0, numberOfData = 0;
	double logLikelihood = 1e307, previousLogLikelihood = 1e308;
	if (numberOfParameters < 1)   // includes intercept
		Melder_throw (U"Not enough columns (should be more than 1).");
	/*
		Divide up the contents of the table into a number of independent variables (x) and two dependent variables (y0 and y1).
	*/
	autoMAT x = zero_MAT (numberOfCells, 1+numberOfFactors);   // column 1 is the intercept
	autoVEC y0 = zero_VEC (numberOfCells);
	autoVEC y1 = zero_VEC (numberOfCells);
	autoVEC meanX = zero_VEC (numberOfFactors);
	autoVEC stdevX = zero_VEC (numberOfFactors);
	autoMAT smallMatrix = zero_MAT (1+numberOfFactors, 1+numberOfParameters);
	autoLogisticRegression thee = LogisticRegression_create (my columnHeaders [dependent1]. label.get(), my columnHeaders [dependent2]. label.get());
	for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
		double minimum = Table_getMinimum (me, factors [ivar]);
		double maximum = Table_getMaximum (me, factors [ivar]);
		Regression_addParameter (thee.get(), my columnHeaders [factors [ivar]]. label.get(), minimum, maximum, 0.0);
	}
	for (integer icell = 1; icell <= numberOfCells; icell ++) {
		y0 [icell] = Table_getNumericValue_Assert (me, icell, dependent1);
		y1 [icell] = Table_getNumericValue_Assert (me, icell, dependent2);
		numberOfY0 += y0 [icell];
		numberOfY1 += y1 [icell];
		numberOfData += y0 [icell] + y1 [icell];
		x [icell] [1+0] = 1.0;   // intercept
		for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
			x [icell] [1+ivar] = Table_getNumericValue_Assert (me, icell, factors [ivar]);
			meanX [ivar] += x [icell] [1+ivar] * (y0 [icell] + y1 [icell]);
		}
	}
	if (numberOfY0 == 0 && numberOfY1 == 0)
		Melder_throw (U"No data in either class. Cannot determine result.");
	if (numberOfY0 == 0)
		Melder_throw (U"No data in class ", my columnHeaders [dependent1]. label.get(), U". Cannot determine result.");
	if (numberOfY1 == 0)
		Melder_throw (U"No data in class ", my columnHeaders [dependent2]. label.get(), U". Cannot determine result.");
	/*
	 * Normalize the data.
	 */
	for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
		meanX [ivar] /= numberOfData;
		for (integer icell = 1; icell <= numberOfCells; icell ++)
			x [icell] [1+ivar] -= meanX [ivar];
	}
	for (integer icell = 1; icell <= numberOfCells; icell ++)
		for (integer ivar = 1; ivar <= numberOfFactors; ivar ++)
			stdevX [ivar] += x [icell] [1+ivar] * x [icell] [1+ivar] * (y0 [icell] + y1 [icell]);
	for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
		stdevX [ivar] = sqrt (stdevX [ivar] / numberOfData);
		for (integer icell = 1; icell <= numberOfCells; icell ++)
			x [icell] [1+ivar] /= stdevX [ivar];
	}
	/*
	 * Initial state of iteration: the null model.
	 */
	thy intercept = log ((double) numberOfY1 / (double) numberOfY0);   // initial state of intercept: best guess for average log odds
	for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
		RegressionParameter parm = thy parameters.at [ivar];
		parm -> value = 0.0;   // initial state of dependence: none
	}
	integer iteration = 1;
	for (; iteration <= 100; iteration ++) {
		previousLogLikelihood = logLikelihood;
		for (integer ivar = 0; ivar <= numberOfFactors; ivar ++)
			for (integer jvar = ivar; jvar <= numberOfParameters; jvar ++)
				smallMatrix [1+ivar] [1+jvar] = 0.0;
		/*
		 * Compute the current log likelihood.
		 */
		logLikelihood = 0.0;
		for (integer icell = 1; icell <= numberOfCells; icell ++) {
			double fittedLogit = thy intercept, fittedP, fittedQ, fittedLogP, fittedLogQ, fittedPQ, fittedVariance;
			for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
				RegressionParameter parm = thy parameters.at [ivar];
				fittedLogit += parm -> value * x [icell] [1+ivar];
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
			for (integer ivar = 0; ivar <= numberOfFactors; ivar ++) {
				/*
				 * The last column gets the gradient of LL: dLL/da, dLL/db, dLL/dc.
				 */
				smallMatrix [1+ivar] [1+numberOfParameters] += x [icell] [1+ivar] * (y1 [icell] * fittedQ - y0 [icell] * fittedP);
				for (integer jvar = ivar; jvar <= numberOfFactors; jvar ++)
					smallMatrix [1+ivar] [1+jvar] += x [icell] [1+ivar] * x [icell] [1+jvar] * fittedVariance;
			}
		}
		if (fabs (logLikelihood - previousLogLikelihood) < 1e-11)
			break;
		/*
		 * Make matrix symmetric.
		 */
		for (integer ivar = 1; ivar <= numberOfFactors; ivar ++)
			for (integer jvar = 0; jvar < ivar; jvar ++)
				smallMatrix [1+ivar] [1+jvar] = smallMatrix [1+jvar] [1+ivar];
		/*
		 * Invert matrix in the simplest way, and shift and wipe the last column with it.
		 */
		for (integer ivar = 0; ivar <= numberOfFactors; ivar ++) {
			const double pivot = smallMatrix [1+ivar] [1+ivar];   /* Save diagonal. */
			smallMatrix [1+ivar] [1+ivar] = 1.0;
			for (integer jvar = 0; jvar <= numberOfParameters; jvar ++)
				smallMatrix [1+ivar] [1+jvar] /= pivot;
			for (integer jvar = 0; jvar <= numberOfFactors; jvar ++) {
				if (jvar != ivar) {
					const double temp = smallMatrix [1+jvar] [1+ivar];
					smallMatrix [1+jvar] [1+ivar] = 0.0;
					for (integer kvar = 0; kvar <= numberOfParameters; kvar ++)
						smallMatrix [1+jvar] [1+kvar] -= temp * smallMatrix [1+ivar] [1+kvar];
				}
			}
		}
		/*
		 * Update the parameters from the last column of smallMatrix.
		 */
		thy intercept += smallMatrix [1+0] [1+numberOfParameters];
		for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
			const RegressionParameter parm = thy parameters.at [ivar];
			parm -> value += smallMatrix [1+ivar] [1+numberOfParameters];
		}
	}
	if (iteration > 100)
		Melder_warning (U"Logistic regression has not converged in 100 iterations. The results are unreliable.");
	for (integer ivar = 1; ivar <= numberOfFactors; ivar ++) {
		const RegressionParameter parm = thy parameters.at [ivar];
		parm -> value /= stdevX [ivar];
		thy intercept -= parm -> value * meanX [ivar];
	}
	return thee;
}

autoLogisticRegression Table_to_LogisticRegression (Table me, conststring32 factors_columnLabelString,
	conststring32 dependent1_columnLabel, conststring32 dependent2_columnLabel)
{
	try {
		auto factors_columnIndices = Table_getColumnIndicesFromColumnLabelString (me, factors_columnLabelString);
		const integer dependent1_columnIndex = Table_getColumnIndexFromColumnLabel (me, dependent1_columnLabel);
		const integer dependent2_columnIndex = Table_getColumnIndexFromColumnLabel (me, dependent2_columnLabel);
		autoLogisticRegression thee = _Table_to_LogisticRegression (me, factors_columnIndices.get(), dependent1_columnIndex, dependent2_columnIndex);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": logistic regression not performed.");
	}
}

static inline double NUMmin2 (double a, double b) {
	return a < b ? a : b;
}

static inline double NUMmax2 (double a, double b) {
	return a > b ? a : b;
}

void LogisticRegression_drawBoundary (LogisticRegression me, Graphics graphics, integer colx, double xleft, double xright,
	integer coly, double ybottom, double ytop, bool garnish)
{
	RegressionParameter parmx = my parameters.at [colx];
	RegressionParameter parmy = my parameters.at [coly];
	if (xleft == xright) {
		xleft = parmx -> minimum;
		xright = parmx -> maximum;
	}
	if (ybottom == ytop) {
		ybottom = parmy -> minimum;
		ytop = parmy -> maximum;
	}
	double intercept = my intercept;
	for (integer iparm = 1; iparm <= my parameters.size; iparm ++) {
		if (iparm != colx && iparm != coly) {
			RegressionParameter parm = my parameters.at [iparm];
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
	trace (U"LogisticRegression_drawBoundary: ",
		xmin, U" ", xmax, U" ", xbottom, U" ", xtop, U" ", ymin, U" ", ymax, U" ", yleft, U" ", yright);
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
		Graphics_textBottom (graphics, true, parmx -> label.get());
		Graphics_marksBottom (graphics, 2, true, true, false);
		Graphics_textLeft (graphics, true, parmy -> label.get());
		Graphics_marksLeft (graphics, 2, true, true, false);
	}
}

/*
autoTable Table_LogisticRegression_addProbabilities (Table me, LogisticRegression thee) {
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
