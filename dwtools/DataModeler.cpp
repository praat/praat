/* DataModeler.cpp
 *
 * Copyright (C) 2014-2022 David Weenink, 2017 Paul Boersma
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
 * ainteger with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataModeler.h"
#include "NUM2.h"
#include "NUMmachar.h"
#include "SVD.h"
#include "Strings_extensions.h"
#include "Sound_and_LPC_robust.h"
#include "Table_extensions.h"

#include "oo_DESTROY.h"
#include "DataModeler_def.h"
#include "oo_COPY.h"
#include "DataModeler_def.h"
#include "oo_EQUAL.h"
#include "DataModeler_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "DataModeler_def.h"
#include "oo_WRITE_TEXT.h"
#include "DataModeler_def.h"
#include "oo_WRITE_BINARY.h"
#include "DataModeler_def.h"
#include "oo_READ_TEXT.h"
#include "DataModeler_def.h"
#include "oo_READ_BINARY.h"
#include "DataModeler_def.h"
#include "oo_DESCRIPTION.h"
#include "DataModeler_def.h"

#include "enums_getText.h"
#include "DataModeler_enums.h"
#include "enums_getValue.h"
#include "DataModeler_enums.h"

Thing_implement (DataModeler, Function, 1);

void structDataModeler :: v1_info () {
	// skipping parent classes?
	MelderInfo_writeLine (U"   Time domain:");
	MelderInfo_writeLine (U"      Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"      End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"      Total duration: ", xmax - xmin, U" seconds");
	const double rSquared = DataModeler_getCoefficientOfDetermination (this, nullptr, nullptr);
	double ndof, probability;
	const double chisq = DataModeler_getChiSquaredQ (this, & probability, & ndof);
	MelderInfo_writeLine (U"   Fit:");
	MelderInfo_writeLine (U"      Number of data points: ", numberOfDataPoints);
	MelderInfo_writeLine (U"      Number of parameters: ", numberOfParameters);
	MelderInfo_writeLine (U"      Each data point has ",  (weighData == kDataModelerWeights::EQUAL_WEIGHTS ? U" the same weight (estimated)." :
		( weighData == kDataModelerWeights::ONE_OVER_SIGMA ? U"a different weight (sigmaY)." : 
		( weighData == kDataModelerWeights::RELATIVE_ ? U"a different relative weight (Y_value/sigmaY)." :
		U"a different weight (SQRT(sigmaY))." ) ) ));
	MelderInfo_writeLine (U"      Chi squared: ", chisq);
	MelderInfo_writeLine (U"      Number of degrees of freedom: ", ndof);
	MelderInfo_writeLine (U"      Probability: ", probability);
	MelderInfo_writeLine (U"      R-squared: ", rSquared);
	for (integer ipar = 1; ipar <= numberOfParameters; ipar ++) {
		const double sigma = ( parameters [ipar]. status == kDataModelerParameterStatus::FIXED_ ? 0 :
			(isdefined (parameterCovariances -> data [ipar] [ipar]) ? sqrt (parameterCovariances -> data [ipar] [ipar]) : undefined) );
		MelderInfo_writeLine (U"      p [", ipar, U"] = ", parameters [ipar]. value, U"; sigma = ", sigma);
	}
}

static double constant_evaluate (DataModeler /* me */, double /* xin */, vector<structDataModelerParameter> p) {
	return p [1]. value;
}

static void constant_evaluateBasisFunctions (DataModeler /* me */, double /* xin */, VEC terms) {
	terms  <<=  1.0;
}

static double linear_evaluate (DataModeler /* me */, double /* xin */, vector<structDataModelerParameter> /* p */) {
	return undefined;
}

static void linear_evaluateBasisFunctions (DataModeler /* me */, double /* xin */, VEC terms) {
	terms  <<=  undefined;
}

static double polynomial_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const longdouble xscaled = (2.0 * x - my xmin - my xmax) / 2.0;
	longdouble xpi = 1.0, result = p [1]. value;
	for (integer ipar = 2; ipar <= p.size; ipar ++) {
		xpi *= xscaled;
		result += p [ipar]. value * xpi;
	}
	return (double) result;
}

static void polynome_evaluateBasisFunctions (DataModeler me, double x, VEC term) {
	Melder_assert (term.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const longdouble xscaled = (2.0 * x - my xmin - my xmax) / 2.0;
	longdouble termp = term [1] = 1.0;
	for (integer ipar = 2; ipar <= my numberOfParameters; ipar ++) {
		termp *= xscaled;
		term [ipar] = (double) termp;
	}
}

static double legendre_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	/*
		From domain [xmin, xmax] to domain [-1, 1]
	*/
	const longdouble xscaled = (2.0 * x - my xmin - my xmax) / (my xmax - my xmin);
	longdouble result = p [1]. value;
	if (p.size > 1) {
		const longdouble twox = 2.0 * xscaled;
		longdouble pti, ptim1, ptim2 = 1.0,f2 = xscaled, d = 1.0;
		result += p [2]. value * (ptim1 = xscaled);
		for (integer ipar = 3; ipar <= p.size; ipar ++) {
			const longdouble f1 = d ++;
			f2 += twox;
			result += p [ipar]. value * (pti = (f2 * ptim1 - f1 * ptim2) / d);
			ptim2 = ptim1;
			ptim1 = pti;
		}
	}
	return (double) result;
}

static void legendre_evaluateBasisFunctions (DataModeler me, double x, VEC term) {
	Melder_assert (term.size == my numberOfParameters);
	term [1] = 1.0;
	/*
		transform x from domain [xmin, xmax] to domain [-1, 1]
	*/
	const longdouble xscaled = (2.0 * x - my xmin - my xmax) / (my xmax - my xmin);
	if (my numberOfParameters > 1) {
		const longdouble twox = 2.0 * xscaled;
		longdouble f2 = term [2] = xscaled, d = 1.0;
		for (integer ipar = 3; ipar <= my numberOfParameters; ipar ++) {
			const longdouble f1 = d ++;
			f2 += twox;
			term [ipar] = (double) ((f2 * term [ipar - 1] - f1 * term [ipar - 2]) / d);
		}
	}
}

static double sigmoid_plus_constant_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	Melder_assert (p.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
		No need to translate because x and p [3] are in the same domain. 
	*/
	longdouble result = p [1]. value;
	if (p [2]. value != 0.0)
		result += p [2]. value / (1.0 + exp (- (x - p [3]. value) / p [4]. value));
	return (double) result;
}

static double sigmoid_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	Melder_assert (p.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
		No need to translate because x and p [3] are in the same domain.
	*/
	const double result = p [1]. value / (1.0 + exp (- (x - p [2]. value) / p [3]. value));
	return result;
}

static double exponential_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	Melder_assert (p.size == my numberOfParameters);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const double xscaled = x - 0.5 * (my xmin + my xmax);
	return p [1]. value * exp (p [2]. value * xscaled);
}

static double exponential_plus_constant_evaluate (DataModeler me, double x, vector<structDataModelerParameter> p) {
	Melder_assert (p.size >= 3);
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const double xscaled = x - 0.5 * (my xmin + my xmax);
	return p [1]. value + p [2]. value * exp (p [3]. value * xscaled);
}

static void dummy_evaluateBasisFunctions (DataModeler /* me */, double /* x */, VEC term) {
	term  <<=  undefined;
}

static autoVEC DataModeler_solveDesign (DataModeler me, constMAT const& design, constVEC const& y, autoMAT *covariance) {
	Melder_require (design.nrow == y.size,
		U"The design matrix and the estimate should have the same number of rows.");
	autoSVD svd = SVD_createFromGeneralMatrix (design);
	if (! NUMfpp)
		NUMmachar ();
	SVD_zeroSmallSingularValues (svd.get(), ( my tolerance > 0.0 ? my tolerance : my numberOfDataPoints * NUMfpp -> eps ));
	autoVEC solution = SVD_solve (svd.get(), y);
	if (covariance) {
		autoMAT covar = SVD_getSquared (svd.get(), true);
		*covariance = covar.move();
	}
	return solution;
}

/*
	Model: y [i] = a * exp (b * x [i]), i=1..n, solve for a, b.
	log(y(x)= log(a) + b * x is linear model
	Precondition: y [i] > 0 || y [i] < 0
*/
static void exponential_fit (DataModeler me) {
	if (my parameters [1]. status == kDataModelerParameterStatus::FIXED_ && my parameters [2]. status == kDataModelerParameterStatus::FIXED_)
		return;
	autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
	double ymin, ymax;
	DataModeler_getExtremaY (me, & ymin, & ymax);
	const double sign = ymin * ymax;
	Melder_require (sign >= 0.0,
		U"All data should have the same sign.");
	const double xtr = 0.5 * (my xmin + my xmax);
	if (my parameters [1]. status == kDataModelerParameterStatus::FIXED_) {
		/*
			Model: z(x) = b * x, where z(x) = log(y) - log (a)
			A minimization of the squared error in the log domain gives greater weight to small values.
			To compensate, we weigh with the y value. Weisstein, Eric W. "Least Squares Fitting--Exponential." From MathWorld--A Wolfram Web Resource. https://mathworld.wolfram.com/LeastSquaresFittingExponential.html 
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 1);
		autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k]. status != kDataModelerData::INVALID) {
				design [++ index] [1] = (my data [k]. x - xtr) * weights [k] * my data [k]. y;
				yEstimate [index] = (log (my data [k]. y) - log (my parameters [1]. value)) * weights [k] * my data [k]. y;
			}
		}
		design.resize (index, 1);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		my parameters [2]. value = solution [1];
	} else if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
		/*
			Model: y(x) = a * f(x), where f(x) = exp (b * x)
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 1);
		autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k].status != kDataModelerData::INVALID) {
				design [++ index] [1] = exp (my parameters [2]. value * (my data [k].x - xtr)) * weights [k];
				yEstimate [index] = my data [k].y * weights [k];
			}
		}
		design.resize (index, 1);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		my parameters [1]. value = solution [1];
	} else {
		/*
			Model z(x)= a + b * x, where z(x) = log(y(x))
			A minimization of the squared error in the log domain gives greater weight to small values.
			To compensate we weigh with the y value.
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 2);
		autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k].status != kDataModelerData::INVALID) {
				design [++ index] [1] = 1.0 * my data [k].y * weights [k];
				design [index] [2] = (my data [k].x - xtr) * my data [k].y * weights [k];
				yEstimate [index] = log ( sign >= 0.0 ? my data [k].y : - my data [k].y ) * my data [k].y * weights [k];
			}
		}
		design.resize (index, 2);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		const double a = exp (solution [1]);
		my parameters [1]. value = ( sign >= 0.0 ? a : - a );
		my parameters [2]. value = solution [2];
	}
}

/*
	Model: y [i] = constant + b * exp (c * x [i]), i=1..n, solve for constant, b & c.
	Solution according to Jean Jacquelin (2009), Régressions et équations intégrales, https://fr.scribd.com/doc/14674814/Regressions-et-equations-integrales,
	pages 16-17.
	Precondition: x [i] must be increasing.
*/

static void linear_exponent_evaluateBasisFunctions (DataModeler me, double xin, VEC term) {
	Melder_assert (term.size >= 2); // our model is a two parameter one!
	/*
		From domain [xmin, xmax] to domain [-(xmax -xmin)/2, (xmax-xmin)/2]
	*/
	const double x = xin - 0.5 * (my xmin + my xmax);
	term [1] = 1.0;
	term [2] = exp (my parameters [3]. value * x);
}

static void exponential_plus_constant_fit (DataModeler me) {
	if (my parameters [1].status == kDataModelerParameterStatus::FIXED_) {
		if (my parameters [2].status == kDataModelerParameterStatus::FIXED_ &&
			my parameters [3].status == kDataModelerParameterStatus::FIXED_)
				return;
		/*
			Model: z(x) = b * exp (c * x), where z(x) = y(x) - a.
		*/
		autoDataModeler thee = DataModeler_createFromDataModeler (me, 2, kDataModelerFunction::EXPONENTIAL);
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (thy data [k]. status != kDataModelerData::INVALID) {
				thy data [k]. y -= my parameters [1]. value;
			}
		}
		DataModeler_fit (thee.get());
		my parameters [2]. value = thy parameters [1]. value;
		my parameters [3]. value = thy parameters [2]. value;
	} else if (my parameters [3]. status == kDataModelerParameterStatus::FIXED_) {
		if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
			/*
				Model: z(x)= a, where z(x) = y(x) - b * exp(c * x)
			*/
			autoDataModeler thee = DataModeler_createFromDataModeler (me, 1, kDataModelerFunction::LINEAR);
			thy f_evaluate = constant_evaluate;
			thy f_evaluateBasisFunctions = constant_evaluateBasisFunctions;
			my parameters [1]. value = 0.0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (thy data [k]. status != kDataModelerData::INVALID) {
					thy data [k]. y -= my f_evaluate (me, thy data [k]. x, my parameters.get());   // z(x) = y(x) - b * exp(c * x)
				}
			}
			DataModeler_fit (thee.get());
			my parameters [1]. value = thy parameters [1]. value;
		} else {
			/*
				Model: z(x) = a + b * f(x), where f(x) = exp (c * x).
				Fit as linear model with the third parameter fixed!
			*/
			autoDataModeler thee = DataModeler_createFromDataModeler (me, 2, kDataModelerFunction::LINEAR);
			thy parameters.resize (thy numberOfParameters + 1);
			thy parameters [thy numberOfParameters + 1]. value = my parameters [3]. value;
			thy parameters [thy numberOfParameters + 1]. status = kDataModelerParameterStatus::FIXED_EXTRA;
			thy f_evaluate = exponential_plus_constant_evaluate;
			thy f_evaluateBasisFunctions = linear_exponent_evaluateBasisFunctions;
			DataModeler_fit (thee.get());
			my parameters [1]. value = thy parameters [1]. value;
			my parameters [2]. value = thy parameters [2]. value;
		}
	} else {
		/*
			First we determine c.
			Model: z(x) = A * f1(x) + B * f2(x), where z(x) = y(x) - y [1], f1(x) = x - x [1], f2(x) = integral (x1, x, y(x)dx)
			A = - a * c, B = c
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 2);
		autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
		autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
		const longdouble x1 = my data [1].x, y1 = my data [1].y;
		longdouble xkm1 = x1, ykm1 = y1, sk = 0.0;
		/*
			First row of design has only zero's, skip it.
		*/
		integer index = 0;
		for (integer k = 2; k <= my numberOfDataPoints; k ++) {
			if (my data [k] .status != kDataModelerData::INVALID) {
				const longdouble xk = my data [k].x, yk = my data [k].y;
				sk += 0.5 * (yk + ykm1) * (xk - xkm1); // Jacquelin, Eq. (7)
				design [++ index] [1] = double (xk - x1) * weights [k];   // Jacquelin, Eq. (9)
				design [index] [2] = double (sk) * weights [k];
				yEstimate [index] = double (yk - y1) * weights [k];
				xkm1 = xk;
				ykm1 = yk;
			}
		}
		design.resize (index, 2);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		const double c = solution [2];
		my parameters [3]. value = c;
		if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
			/*
				Model: z(x)= a, where z(x) = y(x) - b * exp(c * x)
			*/
			autoDataModeler thee = DataModeler_createFromDataModeler (me, 1, kDataModelerFunction::LINEAR);
			thy f_evaluate = constant_evaluate;
			thy f_evaluateBasisFunctions = constant_evaluateBasisFunctions;
			my parameters [1]. value = 0.0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (thy data [k]. status != kDataModelerData::INVALID) {
					thy data [k]. y -= my f_evaluate (me, thy data [k]. x, my parameters.get());// z(x) = y(x) - b * exp(c * x)
				}
			}
			DataModeler_fit (thee.get());
			my parameters [1]. value = thy parameters [1]. value;
		} else {
			/*
				As if c were fixed
				Model: y(x) = a + b * f(x), where f(x) = exp (c * x).
			*/
			autoDataModeler thee = DataModeler_createFromDataModeler (me, 2, kDataModelerFunction::LINEAR);
			thy parameters.resize (thy numberOfParameters + 1);
			thy parameters [thy numberOfParameters + 1]. value = c;
			thy parameters [thy numberOfParameters + 1]. status = kDataModelerParameterStatus::FIXED_EXTRA;
			thy f_evaluate = exponential_plus_constant_evaluate;
			thy f_evaluateBasisFunctions = linear_exponent_evaluateBasisFunctions;
			DataModeler_fit (thee.get());
			my parameters [1]. value = thy parameters [1]. value;
			my parameters [2]. value = thy parameters [2]. value;
			my parameters [3]. value = c;
		}
	}
	/*
		error propagation ?
	*/
	my parameterCovariances -> data.get()  <<=  undefined;
}

static void modelLinearTrendWithSigmoid (DataModeler me, double *out_lambda, double *out_sigma) {
		/* 
			Set mu in the middle of the interval and make lambda = 2 * ymean.
			Calculate sigma such that the model goes through (xmin,model(xmin)) and (xmax, model(xmax))
		 	deltaY = y(xmax) - y(xmin) = lambda / (1 + exp (- (xmax - mu) / sigma)) - lambda / (1 + exp (- (xmin - mu) / sigma))
		 	Then sigma = 0.5 *(xmax - xmin) / ln ((lambda + deltaY) / (lambda - deltaY)))
		*/
		autoDataModeler thee = DataModeler_createFromDataModeler(me, 2, kDataModelerFunction::POLYNOME);
		DataModeler_fit (thee.get());
		const double lambda = 2.0 * thy parameters [1]. value;
		const double yAtXmin = DataModeler_getModelValueAtX (thee.get(), thy xmin);
		const double yAtXmax = DataModeler_getModelValueAtX (thee.get(), thy xmax);
		const double deltaY = yAtXmax - yAtXmin, deltaX = my xmax - my xmin;
		const double sigma = 0.5 * deltaX / log ((lambda + deltaY) / (lambda - deltaY));
		if (out_lambda)
			*out_lambda = lambda;
		if (out_sigma)
			*out_sigma = sigma;	
}

/*
	Function: y(x) = b / (1 + exp (- (x - mu) / sigma))
	Model: z(x) = A * f1(x) + b * f2(x) + C * f3(x), where 
		z (x)= y (x) * ln (y (x)), f1 (x) = integral (0, x, y(x)dx), f2 (x) = x * y (x), f3 = y (x),
		A = -1 / (lambda * sigma), B = 1 / sigma, C = ln (lambda) - ln (1 + exp ((mu - x1) / sigma))
	Non-iterative solution according to  Jean Jacquelin (2009), Régressions et équations intégrales, https://fr.scribd.com/doc/14674814/Regressions-et-equations-integrales,
	pages 16-17.
	Precondition: x [i] must be increasing.
*/
static void sigmoid_fit (DataModeler me) {
	autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
	autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
	double lambda = my parameters [1]. value;
	double mu = my parameters [2]. value;
	double sigma = my parameters [3]. value;
	if (my parameters [1]. status == kDataModelerParameterStatus::FIXED_) {
		if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
			if (my parameters [3]. status == kDataModelerParameterStatus::FIXED_)
				return;
			/*
				Model: z(X)*ln(z(X) = A * f5 (X) + B * f6 (X), where f5 (X) = z (X) * X - z (X) * integral (x [1], X, z(x)dx)), 
				f6 (X) = y (X), z(X) = y (X) / lambda and X [k] = x [k] - mu - x1
				A = 1 / sigma, B = ln (y (x1))
			*/
			autoMAT design = zero_MAT (my numberOfDataPoints, 2);
			longdouble sk = 0.0, x1 = my data [1].x ;   // no need to subtract mu!
			longdouble xkm1 = 0.0, ykm1 = 0.0;
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const longdouble xk = my data [k].x, yk = my data [k].y / lambda;
					sk += 0.5 * (yk + ykm1) * (xk - xkm1);   // invariant under translations in x
					const double f1x = double (yk * sk);
					const double f2x = double ((xk - mu - x1) * yk);   // X [k]
					design [++ index] [1] = (f2x - f1x) * weights [k];
					design [index] [2] = double (yk) * weights [k];
					yEstimate [index] = double (yk) * log (double (yk)) * weights [k];
					xkm1 = xk;
					ykm1 = yk;
				}
			}
			design.resize (index, 2);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			sigma = 1.0 /solution [1];
		} else if (my parameters [3]. status == kDataModelerParameterStatus::FIXED_) {
			/*
				Model: z(X)*ln(z(X) = C * f3 (X), where z(X)= y(X)/lambda + f1 (X) / sigma - f2 (X) / sigma, X [l] = x [k] - x1
			*/
			autoMAT design = zero_MAT (my numberOfDataPoints, 1);
			longdouble sk = 0.0, x1 = my data [1].x;
			longdouble xkm1 = 0.0, ykm1 = 0.0;
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const longdouble xk = my data [k].x, yk = my data [k].y / lambda;
					sk += 0.5 * (yk + ykm1) * (xk - xkm1);   // xk - xkm1 ==  X [k] - X [k-1]
					const double f1x = double (yk * sk);
					const double f2x = double ((xk - x1) * yk);
					design [++ index] [1] = double (yk) * weights [k];
					yEstimate [index] = (double (yk) * log (double (yk)) + f1x / sigma - f2x / sigma) * weights [k];
					xkm1 = xk;
					ykm1 = yk;
				}
			}
			design.resize (index, 1);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			const double lnarg = exp (-solution [1]) - 1.0;
			if (lnarg > 0.0)
				mu = double (x1) + sigma * log (lnarg);
			else
				mu = undefined;
		} else {
			/*
				Model: z*ln(z) = A * f4(X) + B * f3 (X), where z(X) = y(X) / lambda, f4(X)= -f1(X) + f2(X), f3 (X) = z(X), A = 1/sigma, B = - ln (1+exp(-(mu - x1)/sigma)) and X [k] = x [k] - x [1].
			*/
			autoMAT design = zero_MAT (my numberOfDataPoints, 2);
			longdouble sk = 0.0, x1 = my data [1].x;
			longdouble xkm1 = 0.0, ykm1 = 0.0;
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const longdouble xk = my data [k].x, yk = my data [k].y / lambda;
					sk += 0.5 * (yk + ykm1) * (xk - xkm1);
					const double f1x = double (yk * sk);
					const double f2x = double ((xk - x1) * yk);
					design [++ index] [1] = (f2x - f1x) * weights [k];
					design [index] [2] = double (yk) * weights [k]; // f3
					yEstimate [index] = double (yk) * log (double (yk)) * weights [k];
					xkm1 = xk;
					ykm1 = yk;
				}
			}
			design.resize (index, 2);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			sigma = 1.0 / solution [1];
			const double lnarg = exp (-solution [2]) - 1.0;
			if (lnarg > 0.0)
				mu = double (x1) + sigma * log (lnarg);
			else
				mu = undefined;
		}
	} else if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_ &&
		my parameters [3]. status == kDataModelerParameterStatus::FIXED_) {
			/*
				Model: y(x) = E * f4 (x), where f4(x) = 1 /(1 + exp (- (x - mu) / sigma))
			*/
			autoMAT design = zero_MAT (my numberOfDataPoints, 1);
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const double yk = my data [k].y, xk = my data [k].x;
					const double f4x = 1.0 / (1.0 + exp (- (xk - mu) / sigma));
					design [++ index] [1] = f4x * weights [k];
					yEstimate [index] = yk * weights [k];
				}
			}
			design.resize (index, 1);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			lambda = solution [1];
	} else if (my parameters [3]. status == kDataModelerParameterStatus::FIXED_) {
		/*
			Model: z(x) = A * f1 (x) + C * f3(x), where z(x) = y(x)*ln(y(x)) - f2 (x) / sigma
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 2);
		longdouble sk = 0.0, x1 = my data [1].x;
		longdouble xkm1 = 0.0, ykm1 = 0.0;
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k]. status != kDataModelerData::INVALID) {
				const longdouble xk = my data [k].x - x1, yk = my data [k].y;
				const double f2x = double (xk * yk);
				sk += 0.5 * (yk + ykm1) * (xk - xkm1);
				const double f1x = double (yk * sk);
				design [++ index] [1] = f1x * weights [k];
				design [index] [2] = double (yk) * weights [k];
				yEstimate [index] = (double (yk) * log (double (yk)) - f2x / sigma) * weights [k];
				xkm1 = xk;
				ykm1 = yk;
			}
		}
		design.resize (index, 2);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		sigma = my parameters [3]. value;
		lambda = -1.0 / (solution [1] * sigma);
		const double lnarg = lambda * exp (-solution [2]) - 1.0;
		if (lnarg > 0.0)
			mu = double (x1) + sigma * log (lnarg);
		else
			mu = undefined;
	} else {
		/*
			Model: z(x) =  A * f1 (x) + B * f2(x) + C * f3(x), where z(x) = y(x)*ln(y(x))
		*/
		autoMAT design = zero_MAT (my numberOfDataPoints, 3);
		longdouble sk = 0.0, x1 = my data [1].x;
		longdouble xkm1 = 0.0, ykm1 = 0.0;
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k]. status != kDataModelerData::INVALID) {
				const longdouble xk = my data [k].x, yk = my data [k].y;
				sk += 0.5 * (yk + ykm1) * (xk - xkm1);
				const double f1x = double (yk * sk);
				const double f2x = double ((xk - x1) * yk);
				design [++ index] [1] = f1x * weights [k];
				design [index] [2] = f2x * weights [k];
				design [index] [3] = double (yk) * weights [k];
				yEstimate [index] = double (yk) * log (double (yk)) * weights [k];
				xkm1 = xk;
				ykm1 = yk;
			}
		}
		design.resize (index, 3);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		sigma = 1.0 / solution [2];
		lambda = - solution [2] / solution [1];
		if (my parameters [2]. status != kDataModelerParameterStatus::FIXED_) {
			my parameters [1]. value = lambda;
			const double lnarg = lambda * exp (-solution [3]) - 1.0;
			if (lnarg > 0.0)
				mu = double (x1) + sigma * log (lnarg);
			else {
				modelLinearTrendWithSigmoid (me, & lambda, & sigma);
				mu = 0.5 * (my xmin + my xmax);
			}
		}
	}
	my parameters [1]. value = lambda;
	my parameters [2]. value = mu;
	my parameters [3]. value = sigma;
	/*
		error propagation ?
	*/
	my parameterCovariances -> data.get()  <<=  undefined;
}

/*
	Model: y(x) = gamma + lambda / (1 + exp (- (x - mu) / sigma))
	Solution according to  Jean Jacquelin (2009), Régressions et équations intégrales, https://fr.scribd.com/doc/14674814/Regressions-et-equations-integrales,
	pages 38 and following.
	The author makes a mistake in the derivation of the relation between the a,b,c,d and
	gamma, lambda, mu and sigma of the model.
	lambda = ± 1/s * sqrt (b^2-4ac) (the artile wrongly shows "sqrt (b^2+4ac)")
	gamma = (−b ∓ sqrt (b^2 − 4ac))/ (2*a)
	sigma = ∓ 1 / sqrt (b^2 − 4ac)
	mu = x [1]+ sigma * ln (lambda/(d-gamma) - 1)
	Two models are indistinguishable if:
		gamma' = gamma + lambda
		sigma' = -sigma
		lambda' = - lambda
	Precondition: x [i] are increasing order.
*/
static void sigmoid_plus_constant_fit (DataModeler me) {
	double gamma = my parameters [1]. value, lambda = my parameters [2]. value;
	double mu = my parameters [3]. value, sigma = my parameters [4]. value;
	if (my parameters [1]. status == kDataModelerParameterStatus::FIXED_) {
		/*
			Model z(x) = lambda / (1 + exp (- (x - mu) / sigma)) where z(x) = y(x) - gamma.
		*/
		autoDataModeler thee = DataModeler_createFromDataModeler (me, 3, kDataModelerFunction::SIGMOID);
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k]. status != kDataModelerData::INVALID)
				thy data [k]. y -= gamma;
		}
		DataModeler_fit (thee.get());
		lambda = thy parameters [1]. value;
		mu = thy parameters [2]. value;
		sigma = thy parameters [3]. value;
	} else if (my parameters [3]. status == kDataModelerParameterStatus::FIXED_ &&
		my parameters [4]. status == kDataModelerParameterStatus::FIXED_) {
		if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
			/*
				Model: z(x) = gamma, where z(x) = y(x) - lambda / (1 + exp (- (x - mu)/ sigma))
			*/
			autoMAT design = raw_MAT (my numberOfDataPoints, 1);
			autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
			autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const longdouble xk = my data [k].x, yk = my data [k].y;
					const double fx = lambda / (1.0 + exp (- (double (xk) - mu) / sigma));
					design [++ index] [1] = 1.0 * weights [k];
					yEstimate [index] = (double (yk) - fx) * weights [k];
				}
			}
			design.resize (index, 1);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			gamma = solution [1];
		} else {
			/*
				Model: y(x) = gamma  + lambda * f(x), where f(x) = 1 / (1 + exp (-(x -mu)/sigma))
			*/
			autoMAT design = raw_MAT (my numberOfDataPoints, 2);
			autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
			autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
			integer index = 0;
			for (integer k = 1; k <= my numberOfDataPoints; k ++) {
				if (my data [k]. status != kDataModelerData::INVALID) {
					const longdouble xk = my data [k]. x, yk = my data [k]. y;
					const double fx = 1.0 / (1.0 + exp (- (double (xk) - mu) / sigma));
					design [++ index] [1] = 1.0 * weights [k];
					design [index] [2] = fx * weights [k];
					yEstimate [index] = double (yk) * weights [k];
				}
			}
			design.resize (index, 2);
			yEstimate.resize (index);
			autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
			gamma = solution [1];
			lambda = solution [2];
		}
	} else {
		autoMAT design = raw_MAT (my numberOfDataPoints, 4);
		autoVEC yEstimate = raw_VEC (my numberOfDataPoints);
		autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
		longdouble s1k = 0.0, s2k = 0.0, x1 = my data [1].x, xkm1 = x1, ykm1 = 0.0;
		integer index = 0;
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			if (my data [k]. status != kDataModelerData::INVALID) {
				const longdouble xk = my data [k].x, yk = my data [k].y;
				s1k += 0.5 * (yk + ykm1) * (xk - xkm1);
				s2k += 0.5 * (yk * yk + ykm1 * ykm1) * (xk - xkm1);
				design [++ index] [1] = double (s2k) * weights [k];
				design [index] [2] = double (s1k) * weights [k];
				design [index] [3] = double (xk - x1) * weights [k];
				design [index] [4] = 1.0 * weights [k];
				yEstimate [index] = double (yk) * weights [k];
				xkm1 = xk;
				ykm1 = yk;
			}
		}
		design.resize (index, 4);
		yEstimate.resize (index);
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), nullptr);
		const double a = solution [1], b = solution [2];
		const double c = solution [3], d = solution [4];
		auto setMu = [&, d, x1] () -> double {
			mu = undefined;
			const double lnarg = lambda / (d - gamma) - 1.0;
			if (lnarg > 0.0)
				mu = double (x1) + sigma * log (lnarg);
			return mu;
		};
		if (my parameters [2]. status == kDataModelerParameterStatus::FIXED_) {
			sigma = - 1.0 / (a * lambda);
			gamma = 0.5 * lambda * (sigma * b - 1.0);
			if (my parameters [3]. status != kDataModelerParameterStatus::FIXED_)
				setMu ();
		} if (my parameters [4]. status == kDataModelerParameterStatus::FIXED_) {
			lambda = - 1.0 / (a * sigma);
			gamma = 0.5 * lambda * (sigma * b - 1.0);
			setMu ();
		} else {
			const double dissq = b * b - 4.0 * a * c;
			bool modelFitIsBad = true;
			if (dissq > 0.0) {
				const double dis = sqrt (dissq);
				modelFitIsBad = false;
				lambda = dis / a; 
				gamma = (-b - dis) / (2.0 * a);
				sigma = - 1.0 / dis;
				if (my parameters [3]. status != kDataModelerParameterStatus::FIXED_) {
					if (! isdefined (setMu ())) {
						lambda = -dis / a;
						gamma = (-b + dis) / (2.0 * a);
						sigma = 1.0 / dis;
						if (! isdefined (setMu ()))
							modelFitIsBad = true;
					}
					if (DataModeler_getCoefficientOfDetermination (me, nullptr, nullptr) < 0.0) // model fit is bad!
						modelFitIsBad = true;
				}
			}
			if (modelFitIsBad) {
				modelLinearTrendWithSigmoid (me, & lambda, & sigma);
				gamma = 0.0;
				mu = 0.5 * (my xmin + my xmax);
				my parameters [3]. value = mu;
			}
		}
	}
	my parameters [1]. value = gamma;
	my parameters [2]. value = lambda;
	my parameters [3]. value = mu;
	my parameters [4]. value = sigma;
	/*
		error propagation ?
	*/
	my parameterCovariances -> data.get()  <<=  undefined;
}

static void series_fit (DataModeler me) {
	try {
		/*
			Count the number of free parameters to be fitted
		*/
		const integer numberOfFreeParameters = DataModeler_getNumberOfFreeParameters (me);
		if (numberOfFreeParameters == 0)
			return;
		const integer numberOfValidDataPoints = DataModeler_getNumberOfValidDataPoints (me);
		if (numberOfValidDataPoints - numberOfFreeParameters < 0)
			return;
		autoVEC yEstimate = zero_VEC (numberOfValidDataPoints);
		autoVEC term = zero_VEC (my numberOfParameters);
		autovector<structDataModelerParameter> fixedParameters = newvectorcopy (my parameters.all());
		autoMAT design = zero_MAT (numberOfValidDataPoints, numberOfFreeParameters);
		autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
		/*
			For function evaluation with only the FIXED parameters
		*/
		for (integer ipar = 1; ipar <= my parameters.size; ipar ++)
			if (my parameters [ipar]. status != kDataModelerParameterStatus::FIXED_)
				fixedParameters [ipar]. value = 0.0;

		/*
			We solve for the parameters p by minimizing the chi-squared function:
			chiSquared = sum (i=1...n, (y [i] - sum (k=1..m, p [k] X [k] (x [i])) / sigma [i] ),
			where n is the 'numberOfValidDataPoints', m is the 'numberOfFreeParameters',
				- x [i] and y [i] are the i-th datapoint x and y values, respectively,
				- sum (k=1..m, p [k] X [k] (x [i]) is the model estimation at x [i],
				- X [k] (x [i]) is the k-th function term evaluated at x [i],
				- and y [i] has been measured with some uncertainty sigma [i].
			If we define the design matrix matrix A [i] [j] = X [j] (x [i]) / sigma [i] and
			the vector b [i] = y [i] / sigma [i], the problem can be stated as 
			minimize the norm ||A.p - b|| for p.
			This problem can be solved by SVD.
		*/
		integer idata = 1;
		for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
			if (my data [ipoint]. status != kDataModelerData::INVALID) {
				const double xi = my data [ipoint].x, yi = my data [ipoint].y;
				const double yFixed = my f_evaluate (me, xi, fixedParameters.get());
				// individual terms of the function
				my f_evaluateBasisFunctions (me, xi, term.get());
				for (integer icol = 1, ipar = 1; ipar <= my numberOfParameters; ipar ++)
					if (my parameters [ipar]. status == kDataModelerParameterStatus::FREE)
						design [idata] [icol ++] = term [ipar] * weights [ipoint];
				/*
					Only 'residual variance' must be explained by the model
				*/
				yEstimate [idata ++] = (yi - yFixed)  * weights [ipoint];
			}
		}
		autoMAT covar;
		autoVEC solution = DataModeler_solveDesign (me, design.get(), yEstimate.get(), & covar);
		/*
			Put the calculated parameters at the correct position in 'my parameters'
		*/
		Covariance cov = my parameterCovariances.get();
		for (integer kpar = 1, ipar = 1; ipar <= my numberOfParameters; ipar ++) {
			if (my parameters [ipar]. status != kDataModelerParameterStatus::FIXED_)
				my parameters [ipar]. value = solution [kpar ++];
			cov -> centroid [ipar] = my parameters [ipar]. value;
		}
		cov -> numberOfObservations = numberOfValidDataPoints;
		/*
			Estimate covariances between parameters
		*/
		if (numberOfFreeParameters < my numberOfParameters) {
			cov -> data.all()  <<=  0.0;   // set fixed parameters variances and covariances to zero
			for (integer irow = 1, ipar = 1; ipar <= my numberOfParameters; ipar ++) {
				if (my parameters [ipar]. status != kDataModelerParameterStatus::FIXED_) {
					for (integer icol = 1, jpar = 1; jpar <= my numberOfParameters; jpar ++)
						if (my parameters [jpar]. status != kDataModelerParameterStatus::FIXED_)
							cov -> data [ipar] [jpar] = covar [irow] [icol ++];
					irow ++;
				}
			}
		} else {
			my parameterCovariances -> data = covar.move();
		}
	} catch (MelderError) {
		Melder_throw (U"DataModeler no fit.");
	}
}

static void chisqFromZScores (VEC zscores, double *out_chisq, integer *out_numberOfValidZScores) {
	double chisq = 0.0;
	integer numberOfValidZScores = 0;
	for (integer ipoint = 1; ipoint <= zscores.size; ipoint ++) {
		if (isdefined (zscores [ipoint])) {
			chisq += zscores [ipoint] * zscores [ipoint];
			numberOfValidZScores ++;
		}
	}
	if (out_chisq)
		*out_chisq = chisq;
	if (out_numberOfValidZScores)
		*out_numberOfValidZScores = numberOfValidZScores;
}

double DataModeler_getModelValueAtX (DataModeler me, double x) {
	double f = undefined;
	if (x >= my xmin && x <= my xmax)
		f = my f_evaluate (me, x, my parameters.get());
	return f;
}

double DataModeler_getModelValueAtIndex (DataModeler me, integer index) {
	double f = undefined;
	if (index > 0 && index <= my numberOfDataPoints)
		f = my f_evaluate (me, my data [index]. x, my parameters.get());
	return f;
}

void DataModeler_getExtremaY (DataModeler me, double *out_ymin, double *out_ymax) {
	MelderExtremaWithInit extrema;
	for (integer i = 1; i <= my numberOfDataPoints; i++)
		if (my data [i]. status != kDataModelerData::INVALID)
			extrema.update (my data [i]. y);

	if (out_ymin)
		*out_ymin = extrema.min;
	if (out_ymax)
		*out_ymax = extrema.max;
}

double DataModeler_getDataPointYValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my data [index]. status != kDataModelerData::INVALID)
		value = my data [index]. y;
	return value;
}

double DataModeler_getDataPointXValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfDataPoints && my data [index]. status != kDataModelerData::INVALID)
		value = my data [index]. x;
	return value;
}

void DataModeler_setDataPointYValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints)
		my data [index]. y = value;
}

void DataModeler_setDataPointXValue (DataModeler me, integer index, double value) {
	if (index > 0 && index <= my numberOfDataPoints)
		my data [index]. x = value;
}

void DataModeler_setDataPointValues (DataModeler me, integer index, double xvalue, double yvalue) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my data [index]. x = xvalue;
		my data [index]. y = yvalue;
	}
}

void DataModeler_setDataPointYSigma (DataModeler me, integer index, double sigma) {
	if (index > 0 && index <= my numberOfDataPoints)
		my data [index]. sigmaY = sigma;
}

double DataModeler_getDataPointYSigma (DataModeler me, integer index) {
	double sigma = undefined;
	if (index > 0 && index <= my numberOfDataPoints)
		sigma = my data [index]. sigmaY;
	return sigma;
}

kDataModelerData DataModeler_getDataPointStatus (DataModeler me, integer index) {
	kDataModelerData value = kDataModelerData::INVALID;
	if (index > 0 && index <= my numberOfDataPoints)
		value = my data [index]. status;
	return value;
}

void DataModeler_setDataPointStatus (DataModeler me, integer index, kDataModelerData status) {
	if (index > 0 && index <= my numberOfDataPoints) {
		if (status == kDataModelerData::VALID && isundef (my data [index] .y))
			Melder_throw (U"Your data value is undefined. First set the value and then its status.");
		my data [index]. status = status;
	}
}

void DataModeler_setDataPointValueAndStatus (DataModeler me, integer index, double value, kDataModelerData dataStatus) {
	if (index > 0 && index <= my numberOfDataPoints) {
		my data [index]. y = value;
		my data [index]. status = dataStatus;
	}
}

void DataModeler_setParameterValue (DataModeler me, integer index, double value, kDataModelerParameterStatus status) {
	if (index > 0 && index <= my numberOfParameters) {
		my parameters [index]. value = value;
		my parameters [index]. status = status;
	}
}

void DataModeler_setParameterValueFixed (DataModeler me, integer index, double value) {
	Melder_require (my type == kDataModelerFunction::POLYNOME || my type == kDataModelerFunction::LEGENDRE,
		U"This would change the model type, which is not possible yet.");
	DataModeler_setParameterValue (me, index, value, kDataModelerParameterStatus::FIXED_);
}

double DataModeler_getParameterValue (DataModeler me, integer index) {
	double value = undefined;
	if (index > 0 && index <= my numberOfParameters)
		value = my parameters [index]. value;
	return value;
}

autoVEC DataModeler_listParameterValues (DataModeler me) {
	autoVEC result = raw_VEC (my numberOfParameters);
	for (integer k = 1; k <= my numberOfParameters; k ++)
		result [k] = my parameters [k]. value;
	return result;
}

kDataModelerParameterStatus DataModeler_getParameterStatus (DataModeler me, integer index) {
	kDataModelerParameterStatus status = kDataModelerParameterStatus::UNDEFINED;
	if (index > 0 && index <= my numberOfParameters)
		status = my parameters [index]. status;
	return status;
}

double DataModeler_getParameterStandardDeviation (DataModeler me, integer index) {
	double stdev = undefined;
	if (index > 0 && index <= my numberOfParameters)
		stdev = sqrt (my parameterCovariances -> data [index] [index]);
	return stdev;
}

double DataModeler_getVarianceOfParameters (DataModeler me, integer fromIndex, integer toIndex, integer *out_numberOfFreeParameters) {
	double variance = undefined;
	getAutoNaturalNumbersWithinRange (& fromIndex, & toIndex, my numberOfParameters, U"parameter");
	integer numberOfFreeParameters = 0;	
	variance = 0;
	for (integer ipar = fromIndex; ipar <= toIndex; ipar ++) {
		if (my parameters [ipar]. status != kDataModelerParameterStatus::FIXED_) {
			variance += my parameterCovariances -> data [ipar] [ipar];
			numberOfFreeParameters ++;
		}
	}	
	if (out_numberOfFreeParameters)
		*out_numberOfFreeParameters = numberOfFreeParameters;
	return variance;
}

void DataModeler_setParametersFree (DataModeler me, integer fromIndex, integer toIndex) {
	getAutoNaturalNumbersWithinRange (& fromIndex, & toIndex, my numberOfParameters, U"parameter");
	for (integer ipar = fromIndex; ipar <= toIndex; ipar ++)
		my parameters [ipar]. status = kDataModelerParameterStatus::FREE;
}

void DataModeler_setParameterValuesToZero (DataModeler me, double numberOfSigmas) {
	integer numberOfChangedParameters = 0;
	for (integer ipar = my numberOfParameters; ipar > 0; ipar --) {
		if (my parameters [ipar]. status != kDataModelerParameterStatus::FIXED_) {
			const double value = my parameters [ipar]. value;
			double sigmas = numberOfSigmas * DataModeler_getParameterStandardDeviation (me, ipar);
			if ((value - sigmas) * (value + sigmas) < 0) {
				DataModeler_setParameterValueFixed (me, ipar, 0.0);
				numberOfChangedParameters ++;
			}
		}
	}
}

integer DataModeler_getNumberOfFreeParameters (DataModeler me) {
	integer numberOfFreeParameters = 0;
	for (integer ipar = 1; ipar <= my numberOfParameters; ipar ++)
		if (my parameters [ipar]. status == kDataModelerParameterStatus::FREE)
			numberOfFreeParameters ++;
	return numberOfFreeParameters;
}

integer DataModeler_getNumberOfFixedParameters (DataModeler me) {
	return my numberOfParameters - DataModeler_getNumberOfFreeParameters (me);
}

integer DataModeler_getNumberOfValidDataPoints (DataModeler me) {
	integer numberOfValidDataPoints = 0;
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++)
		if (my data [ipoint]. status != kDataModelerData::INVALID)
			numberOfValidDataPoints ++;
	return numberOfValidDataPoints;
}

integer DataModeler_getNumberOfInvalidDataPoints (DataModeler me) {
	return my numberOfDataPoints - DataModeler_getNumberOfValidDataPoints  (me);
}

void DataModeler_setTolerance (DataModeler me, double tolerance) {
	my tolerance = ( tolerance > 0.0 ? tolerance : my numberOfDataPoints * NUMfpp -> eps );
}

double DataModeler_getDegreesOfFreedom (DataModeler me) {
	integer numberOfDataPoints = 0;
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++)
		if (my data [ipoint]. status != kDataModelerData::INVALID)
			numberOfDataPoints ++;
	const double ndf = numberOfDataPoints - DataModeler_getNumberOfFreeParameters (me);
	return ndf;
}

/*
	Interpret the values in sigmaY as 1 / sigmay or 1 / sqrt (sigmaY) or y/sigmaY. 
	If equal weighing than get the sigma form the residual sum of squares between model and data.
*/
autoVEC DataModeler_getDataPointsWeights (DataModeler me, kDataModelerWeights weighData) {
	autoVEC weights = zero_VEC (my numberOfDataPoints);
	if (weighData == kDataModelerWeights::EQUAL_WEIGHTS) {
			/*
				We weigh with the inverse of the standard deviation of the data to give
				subsequent Chi squared tests a meaningful interpretation.
			*/
			const double stdev = DataModeler_getDataStandardDeviation (me);
			Melder_require (isdefined (stdev),
				U"Not enough data points to calculate standard deviation.");
			weights.all()  <<=  1.0 / stdev;
	} else {	
		for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
			if (my data [ipoint]. status == kDataModelerData::INVALID)
				continue; // invalid points get weight 0.
			const double sigma = my data [ipoint]. sigmaY;
			double weight = 1.0;
			if (isdefined (sigma) && sigma > 0.0) {
				if (weighData == kDataModelerWeights::ONE_OVER_SIGMA)
					weight = 1.0 / sigma;
				else if (weighData == kDataModelerWeights::RELATIVE_)
					weight = my data [ipoint]. y / sigma;
				else if (weighData == kDataModelerWeights::ONE_OVER_SQRTSIGMA) {
					weight = 1.0 / sqrt (sigma);
				}
			}
			weights [ipoint] = weight;
		}
	}
	return weights;
}

autoVEC DataModeler_getZScores (DataModeler me) {
	try {
		autoVEC zscores = raw_VEC (my numberOfDataPoints);
		autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
		for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
			double z = undefined;
			if (my data [ipoint]. status != kDataModelerData::INVALID) {
				const double estimate = my f_evaluate (me, my data [ipoint] .x, my parameters.get());
				z = (my data [ipoint]. y - estimate) * weights [ipoint]; // 1/sigma
			}
			zscores [ipoint] = z;
		}
		return zscores;
	} catch (MelderError) {
		Melder_throw (U"No z-scores calculated.");
	}
}

autoVEC DataModeler_getChisqScoresFromZScores (DataModeler me, constVEC zscores, bool substituteAverage) {
	Melder_assert (zscores.size == my numberOfDataPoints);
	autoVEC chisq = raw_VEC (zscores.size);
	integer numberOfDefined = 0;
	double sumchisq = 0.0;
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
		chisq [ipoint] = undefined;
		if (isdefined (zscores [ipoint])) {
			chisq [ipoint] = zscores [ipoint] * zscores [ipoint];
			sumchisq += chisq [ipoint];
			numberOfDefined ++;
		}
	}
	if (substituteAverage && numberOfDefined != my numberOfDataPoints && numberOfDefined > 0) {
		for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
			if (isundef (chisq [ipoint]))
				chisq [ipoint] = sumchisq / numberOfDefined;
		}
	}
	return chisq;
}

double DataModeler_getChiSquaredQ (DataModeler me, double *out_prob, double *out_df) {
	double chisq;
	integer numberOfValidZScores;
	autoVEC zscores = DataModeler_getZScores (me);
	chisqFromZScores (zscores.get(), & chisq, & numberOfValidZScores);
	const double ndf = DataModeler_getDegreesOfFreedom (me);
	
	if (out_prob)
		*out_prob = NUMchiSquareQ (chisq, ndf);
	if (out_df)
		*out_df = ndf;
	return chisq;
}

double DataModeler_getWeightedMean (DataModeler me) {
	double ysum = 0.0, wsum = 0.0;
	autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++)
		if (my data [ipoint]. status != kDataModelerData::INVALID) {
			ysum += my data [ipoint] .y * weights [ipoint];
			wsum += weights [ipoint];
		}
	return ysum / wsum;
}

double DataModeler_getCoefficientOfDetermination (DataModeler me, double *out_ssreg, double *out_sstot) {

	/*
		We cannot use the standard expressions for ss_tot, and ss_reg because our data are weighted by 1 / sigma [i].
		We need the weighted mean and we need to weigh all sums-of-squares accordingly;
		if all sigma [i] terms are equal, the formulas reduce to the standard ones.
		Ref: A. Buse (1973): Goodness of Fit in Generalized Least Squares Estimation, The American Statician, vol 27, 106-108
	 */

	const double ymean = DataModeler_getWeightedMean (me);
	autoVEC weights = DataModeler_getDataPointsWeights (me, my weighData);
	longdouble sstot = 0.0, ssreg = 0.0;
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
		if (my data [ipoint]. status != kDataModelerData::INVALID) {
			double diff = (my data [ipoint]. y - ymean) * weights [ipoint];
			sstot += diff * diff; // total sum of squares
			const double estimate = my f_evaluate (me, my data [ipoint] .x, my parameters.get());
			diff = (estimate - my data [ipoint]. y)  * weights [ipoint];
			ssreg += diff * diff; // regression sum of squares
		}
	}
	const double rSquared = ( sstot > 0.0 ? 1.0 - double (ssreg / sstot) : 1.0 );
	
	if (out_ssreg)
		*out_ssreg = double (sstot - ssreg);
	if (out_sstot)
		*out_sstot = double (sstot);
	return rSquared;
}

void DataModeler_drawBasisFunction_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	integer iterm, bool scale, integer numberOfPoints)
{
	if (iterm > my numberOfParameters)
		return;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	autoVEC x = raw_VEC (numberOfPoints);
	autoVEC y = raw_VEC (numberOfPoints);
	autoVEC term = raw_VEC (my numberOfParameters);
	for (integer i = 1; i <= numberOfPoints; i ++) {
		x [i] = xmin + (i - 0.5) * (xmax - xmin) / numberOfPoints;
		my f_evaluateBasisFunctions (me, x [i], term.get());
		y [i] = term [iterm];
		y [i] = ( scale ? y [i] * my parameters [iterm]. value : y [i] );
	}
	if (ymax <= ymin) {
		MelderExtremaWithInit extrema;
		for (integer i = 1; i <= numberOfPoints; i ++)
			extrema.update (y [i]);
		ymax = extrema.max;
		ymin = extrema.min;
		
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer i = 2; i <= numberOfPoints; i ++)
		Graphics_line (g, x [i-1], y [i-1], x [i], y [i]);
}

integer DataModeler_drawingSpecifiers_x (DataModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax) {
	if (*xmax <= *xmin) {
		*xmin = my xmin;
		*xmax = my xmax;
	}
	*ixmin = 2;
	while (my data [*ixmin] .x < *xmin && *ixmin < my numberOfDataPoints)
		(*ixmin) ++;
	(*ixmin) --;
	*ixmax = my numberOfDataPoints - 1;
	while (my data [*ixmax] .x > *xmax && *ixmax > 1)
		(*ixmax) --;
	(*ixmax) ++;
	return *ixmax - *ixmin + 1;
}

void DataModeler_drawOutliersMarked_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, conststring32 mark, double marksFontSize)
{
	integer ixmin, ixmax;
	if (DataModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) < 1)
		return;
	autoVEC zscores = DataModeler_getZScores (me);
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setFontSize (g, marksFontSize);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	const double currentFontSize = Graphics_inqFontSize (g);
	for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
		if (my data [ipoint]. status != kDataModelerData::INVALID) {
			const double x = my data [ipoint]. x, y = my data [ipoint]. y;
			if (x >= xmin && x <= xmax && y >= ymin && y <= ymax)
				if (fabs (zscores [ipoint]) > numberOfSigmas)
					Graphics_text (g, x, y, mark);
		}
	}
	Graphics_setFontSize (g, currentFontSize);
}

void DataModeler_draw_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters, bool errorbars, bool connectPoints, double barWidth_wc, bool drawDots)
{
	Function_unidirectionalAutowindow (me, & xmin, & xmax);

	integer ixmin = 1;
	while (ixmin <= my numberOfDataPoints && my data [ixmin]. x < xmin)
		ixmin ++;
	integer ixmax = my numberOfDataPoints;
	while (ixmax > 0 && my data [ixmax]. x > xmax)
		ixmax --;
	if (ixmin > ixmax)
		return; // nothing to draw
	getAutoNaturalNumberWithinRange (& numberOfParameters, my numberOfParameters);	
	vector<structDataModelerParameter> parameters = my parameters.part (1, numberOfParameters);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double x1, y1, x2, y2;
	bool x1defined = false, x2defined = false;
	for (integer ipoint = ixmin; ipoint <= ixmax; ipoint ++) {
		if (my data [ipoint]. status != kDataModelerData::INVALID) {
			const double x = my data [ipoint]. x, y = my data [ipoint]. y;
			if (! x1defined) {
				x1 = x;
				y1 = ( estimated ? my f_evaluate (me, x, parameters) : y );
				x1defined = true;
			} else {
				x2 = x;
				y2 = ( estimated ? my f_evaluate (me, x, parameters) : y );
				x2defined = true;
			}
			if (x1defined && drawDots) {
				if (y >= ymin && y <= ymax)
					Graphics_speckle (g, x, y);
			}
			if (x2defined) { // if (x1defined && x2defined)
				if (connectPoints) {
					double xo1, yo1, xo2, yo2;
					if (NUMclipLineWithinRectangle (x1, y1, x2, y2,
						xmin, ymin, xmax, ymax, & xo1, & yo1, & xo2, & yo2)) {
						Graphics_line (g, xo1, yo1, xo2, yo2);
					}
				}
				x1 = x;
				y1 = y2;
			}
			const double sigma = my data [ipoint] .sigmaY;
			if (errorbars && isdefined (sigma) && sigma > 0 && x1defined) {
				const double ym = y1;
				double yt = ym + 0.5 * sigma, yb = ym - 0.5 * sigma;
				if (estimated) {
					yt = ( (y - y1) > 0.0 ? y : y1 );
					yb = ( (y - y1) > 0.0 ? y1 : y );
				}
				bool topOutside = yt > ymax, bottomOutside = yb < ymin;
				yt = ( topOutside ? ymax : yt );
				yb = ( bottomOutside ? ymin : yb );
				Graphics_line (g, x1, yb, x1, yt);
				if (barWidth_wc > 0.0 && ! estimated) {
					double xl = x1 - 0.5 * barWidth_wc;
					double xr = xl + barWidth_wc;
					if (! topOutside)
						Graphics_line (g, xl, yt, xr, yt);
					if (! bottomOutside)
						Graphics_line (g, xl, yb, xr, yb);
				}
			}
		}
	}
}


void DataModeler_drawModel_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, integer numberOfPoints) {
	Function_bidirectionalAutowindow (me, & xmin, & xmax);
	autoVEC x = raw_VEC (numberOfPoints), y = raw_VEC (numberOfPoints);
	const double dx = (xmax - xmin) / numberOfPoints;
	for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
		x [ipoint] = xmin + (ipoint - 1) * dx;
		y [ipoint] = my f_evaluate (me, x [ipoint], my parameters.get());
	}
	if (ymin == 0.0 && ymax == 0.0) {
		ymin = NUMmin (y.get());
		ymax = NUMmax (y.get());
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer ipoint = 2; ipoint <= numberOfPoints; ipoint ++) {
		double segment_x1, segment_y1, segment_x2, segment_y2;
		if (NUMclipLineWithinRectangle (x [ipoint - 1], y [ipoint - 1], x [ipoint], y [ipoint],
			xmin, ymin, xmax, ymax, & segment_x1, & segment_y1, & segment_x2, & segment_y2))
				Graphics_line (g, segment_x1, segment_y1, segment_x2, segment_y2);
	}
}

void DataModeler_drawModel (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, integer numberOfPoints, bool garnish) {
	Function_bidirectionalAutowindow (me, & xmin, & xmax);
	Graphics_setInner (g);
	DataModeler_drawModel_inside (me, g, xmin, xmax, ymin, ymax, numberOfPoints);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DataModeler_drawTrack_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters)
{
	const bool errorbars = false, connectPoints = true;
	const double barWidth_mm = 0;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_mm, 0);
}

void DataModeler_drawTrack (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool garnish) {
	if (ymax <= ymin)
		DataModeler_getExtremaY (me, & ymin, & ymax);
	Graphics_setInner (g);
	DataModeler_drawTrack_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DataModeler_speckle_inside (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool estimated, integer numberOfParameters, bool errorbars, double barWidth_wc) {
	bool connectPoints = false;
	DataModeler_draw_inside (me, g, xmin, xmax, ymin, ymax, estimated, numberOfParameters, errorbars, connectPoints, barWidth_wc, 1);
}

void DataModeler_speckle (DataModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool estimated, integer numberOfParameters, bool errorbars, double barWidth_mm, bool garnish)
{
	if (ymax <= ymin)
		DataModeler_getExtremaY (me, & ymin, & ymax);
	Graphics_setInner (g);
	if (numberOfParameters == 0)
		numberOfParameters = my numberOfParameters;
	DataModeler_speckle_inside (me, g, xmin, xmax, ymin, ymax,
		estimated, numberOfParameters, errorbars, barWidth_mm);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoTable DataModeler_to_Table_zscores (DataModeler me) {
	try {
		const conststring32 columnNames [] = { U"x", U"z" };
		autoTable ztable = Table_createWithColumnNames (my numberOfDataPoints, ARRAY_TO_STRVEC (columnNames));
		autoVEC zscores = DataModeler_getZScores (me);
		for (integer ipoint = 1; ipoint <= my numberOfDataPoints; ipoint ++) {
			Table_setNumericValue (ztable.get(), ipoint, 1, my  data [ipoint] .x);
			Table_setNumericValue (ztable.get(), ipoint, 2, zscores [ipoint]);
		}
		return ztable;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

void DataModeler_normalProbabilityPlot (DataModeler me, Graphics g, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish) {
	try {
		autoTable thee = DataModeler_to_Table_zscores (me);
		Table_normalProbabilityPlot (thee.get(), g, 2, numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void DataModeler_setBasisFunctions (DataModeler me, kDataModelerFunction type) {
	if (type == kDataModelerFunction::LINEAR) {
		my f_evaluate = linear_evaluate;
		my f_evaluateBasisFunctions = linear_evaluateBasisFunctions;
		my fit = series_fit;
	} else if (type == kDataModelerFunction::LEGENDRE) {
		my f_evaluate = legendre_evaluate;
		my f_evaluateBasisFunctions = legendre_evaluateBasisFunctions;
		my fit = series_fit;
	} else if (type == kDataModelerFunction::POLYNOME) {
		my f_evaluate = polynomial_evaluate;
		my f_evaluateBasisFunctions = polynome_evaluateBasisFunctions;
		my fit = series_fit;
	} else if (type == kDataModelerFunction::SIGMOID) {
		my f_evaluate = sigmoid_evaluate;
		my f_evaluateBasisFunctions = dummy_evaluateBasisFunctions;
		my fit = sigmoid_fit;
	} else if (type == kDataModelerFunction::SIGMOID_PLUS_CONSTANT) {
		my f_evaluate = sigmoid_plus_constant_evaluate;
		my f_evaluateBasisFunctions = dummy_evaluateBasisFunctions;
		my fit = sigmoid_plus_constant_fit;
	} else if (type == kDataModelerFunction::EXPONENTIAL) {
		my f_evaluate = exponential_evaluate;
		my f_evaluateBasisFunctions = dummy_evaluateBasisFunctions;
		my fit = exponential_fit;
	} else if (type == kDataModelerFunction::EXPONENTIAL_PLUS_CONSTANT) {
		my f_evaluate = exponential_plus_constant_evaluate;
		my f_evaluateBasisFunctions = dummy_evaluateBasisFunctions;
		my fit = exponential_plus_constant_fit;
	}
	my type = type;
}

void DataModeler_init (DataModeler me, double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type) {
	my xmin = xmin;
	my xmax = xmax;
	DataModeler_setBasisFunctions (me, type);
	my numberOfDataPoints = numberOfDataPoints;
	my data = newvectorzero<structDataModelerData> (numberOfDataPoints);
	my numberOfParameters = ( (type == kDataModelerFunction::EXPONENTIAL) ? 2 :
		(type == kDataModelerFunction::EXPONENTIAL_PLUS_CONSTANT || type == kDataModelerFunction::SIGMOID) ? 3 :
		(type == kDataModelerFunction::SIGMOID_PLUS_CONSTANT ? 4 : numberOfParameters) );
	Melder_require (my numberOfParameters > 0,
		U"The number of parameters should be greater than zero.");
	my parameters = newvectorzero<structDataModelerParameter> (my numberOfParameters);
	for (integer ipar = 1; ipar <= my numberOfParameters; ipar ++)
		my parameters [ipar]. status = kDataModelerParameterStatus::FREE;
	my parameterNames = Strings_createFixedLength (my numberOfParameters);
	my parameterCovariances = Covariance_create (my numberOfParameters);
	my type = type;
}

autoDataModeler DataModeler_create (double xmin, double xmax, integer numberOfDataPoints, integer numberOfParameters, kDataModelerFunction type) {
	try {
		autoDataModeler me = Thing_new (DataModeler);
		DataModeler_init (me.get(), xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		return me;
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}
}

autoDataModeler DataModeler_createSimple (double xmin, double xmax,
	integer numberOfDataPoints, constVECVU const& parameterValues, double gaussianNoiseStd, kDataModelerFunction type)
{
	try {
		Melder_require (xmin < xmax,
			U"The domain should be defined properly.");
		
		autoDataModeler me = DataModeler_create (xmin, xmax, numberOfDataPoints, parameterValues.size, type);
		for (integer ipar = 1; ipar <= parameterValues.size; ipar ++)
			my parameters [ipar]. value = parameterValues [ipar];   // parameters status ok
		// generate the data that beinteger to the parameter values
		for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++) {
			my data [ipoint]. x = xmin + (ipoint - 0.5) * (xmax - xmin) / numberOfDataPoints;
			const double modelY = my f_evaluate (me.get(), my data [ipoint]. x, my parameters.get());
			my data [ipoint]. y = modelY + NUMrandomGauss (0.0, gaussianNoiseStd);
			my data [ipoint]. sigmaY = undefined;
		}
		my weighData = kDataModelerWeights::EQUAL_WEIGHTS;
		return me;
	} catch (MelderError) {
		Melder_throw (U"No simple DataModeler created.");
	}
}

autoDataModeler DataModeler_createFromDataModeler (DataModeler thee, integer numberOfParameters, kDataModelerFunction type) {
	try {
		autoDataModeler me = DataModeler_create (thy xmin, thy xmax, thy numberOfDataPoints, numberOfParameters, type);
		for (integer k = 1; k <= my numberOfDataPoints; k ++) {
			my data [k]. status = my data [k]. status;
			my data [k]. x = thy data [k]. x;
			my data [k]. y = thy data [k]. y;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"No DataModeler could be created.");
	}
}

void DataModeler_fit (DataModeler me) {
	try {
		my fit (me);
	} catch (MelderError) {
		Melder_throw (U"DataModeler no fit.");
	}
}

void DataModeler_setDataWeighing (DataModeler me, kDataModelerWeights weighData) {
	if (my weighData != weighData) {
		my weighData = weighData;
		DataModeler_fit (me); // because sigma has changed!
	}
}

autoCovariance DataModeler_to_Covariance_parameters (DataModeler me) {
	try {
		autoCovariance cov = Data_copy (my parameterCovariances.get());
		return cov;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

autoDataModeler Table_to_DataModeler (Table me, double xmin, double xmax, integer xcolumn, integer ycolumn, integer sigmacolumn, integer numberOfParameters,  kDataModelerFunction type) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, xcolumn);
		Table_checkSpecifiedColumnNumberWithinRange (me, ycolumn);
		const bool hasSigmaColumn = ( sigmacolumn > 0 );
		if (hasSigmaColumn)
			Table_checkSpecifiedColumnNumberWithinRange (me, sigmacolumn);
		const integer numberOfRows = my rows.size;
		integer numberOfData = 0;
		autoVEC x = raw_VEC (numberOfRows);
		autoVEC y = raw_VEC (numberOfRows);
		autoVEC sy = raw_VEC (numberOfRows);
		for (integer i = 1; i <= numberOfRows; i ++) {
			const double val = Table_getNumericValue_Assert (me, i, xcolumn);
			if (isdefined (val)) {
				x [++ numberOfData] = val;
				if (numberOfData > 1) {
					if (val < x [numberOfData - 1]) {
						Melder_throw (U"Data with x-values should be sorted.");
					} else if (val == x [numberOfData - 1]) {
						Melder_throw (U"All x-values should be different.");
					}
				}
				y [numberOfData] = Table_getNumericValue_Assert (me, i, ycolumn);
				sy [numberOfData] = ( hasSigmaColumn ? Table_getNumericValue_Assert (me, i, sigmacolumn) : undefined );
			}
		}
		if (xmax <= xmin)
			NUMextrema (x.part (1, numberOfData), & xmin, & xmax);
		Melder_require (xmin < xmax,
			U"The range of the x-values is too small.");
		
		integer numberOfDataPoints = 0, validData = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax)
				numberOfDataPoints ++;
		}
		/*
			Some models have a fixed number of parameters
		*/
		autoDataModeler thee = DataModeler_create (xmin, xmax, numberOfDataPoints, numberOfParameters, type);
		numberOfDataPoints = 0;
		for (integer i = 1; i <= numberOfData; i ++) {
			if (x [i] >= xmin && x [i] <= xmax) {
				thy data [++ numberOfDataPoints]. x = x [i];
				thy data [numberOfDataPoints]. status = kDataModelerData::INVALID;
				if (isdefined (y [i])) {
					thy data [numberOfDataPoints]. y = y [i];
					thy data [numberOfDataPoints]. sigmaY = sy [i];
					thy data [numberOfDataPoints]. status = kDataModelerData::VALID;
					validData ++;
				}
			}
		}
		thy numberOfDataPoints = numberOfDataPoints;
		Melder_require (validData >= thy numberOfParameters,
			U"The number of parameters should not exceed the number of data points.");
		
		DataModeler_setDataWeighing (thee.get(), ( hasSigmaColumn ? kDataModelerWeights::ONE_OVER_SIGMA : kDataModelerWeights::EQUAL_WEIGHTS));
		thy tolerance = 1e-8;
		DataModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Datamodeler not created from Table.");
	}
}

double DataModeler_getResidualSumOfSquares (DataModeler me, integer *out_numberOfValidDataPoints) {
	integer numberOfValidDataPoints = 0;
	longdouble residualSS = 0.0;
	for (integer i = 1; i <= my numberOfDataPoints; i ++) {
		if (my data [i]. status != kDataModelerData::INVALID) {
			++ numberOfValidDataPoints;
			residualSS += sqr (my data [i]. y - my f_evaluate (me, my data [i]. x, my parameters.get()));
		}
	}
	if (out_numberOfValidDataPoints)
		*out_numberOfValidDataPoints = numberOfValidDataPoints;
	return ( numberOfValidDataPoints > 0 ? (double) residualSS : undefined );
}

void DataModeler_reportChiSquared (DataModeler me) {
	MelderInfo_writeLine (U"Chi squared test:");
	MelderInfo_writeLine (( my weighData == kDataModelerWeights::EQUAL_WEIGHTS ? U"Standard deviation is estimated from the data." :
		( my weighData == kDataModelerWeights::ONE_OVER_SIGMA ? U"Sigmas are used as estimate for local standard deviations." :
		( my weighData == kDataModelerWeights::RELATIVE_ ? U"1/Q's are used as estimate for local standard deviations." :
		U"Sqrt sigmas are used as estimate for local standard deviations." ) ) ));
	double ndf, probability;
	const double chisq = DataModeler_getChiSquaredQ (me, & probability, & ndf);
	MelderInfo_writeLine (U"Chi squared = ", chisq);
	MelderInfo_writeLine (U"Probability = ", probability);
	MelderInfo_writeLine (U"Number of degrees of freedom = ", ndf);	
}

double DataModeler_getDataStandardDeviation (DataModeler me) {
	try {
		integer numberOfDataPoints = 0;
		autoVEC y = raw_VEC (my numberOfDataPoints);
		for (integer i = 1; i <= my numberOfDataPoints; i ++)
			if (my data [i]. status != kDataModelerData::INVALID)
				y [++ numberOfDataPoints] = my data [i]. y;
		y. resize (numberOfDataPoints);   // fake shrink
		return NUMstdev (y.get());
	} catch (MelderError) {
		Melder_throw (U"Cannot estimate sigma.");
	}
}

/* End of file DataModeler.cpp */
