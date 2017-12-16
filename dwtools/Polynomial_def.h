/* Polynomial_def.h
 *
 * Copyright (C) 1993-2002, 2016 David Weenink
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
 djmw 1999
 djmw 20020813 GPL header
*/

#define ooSTRUCT FunctionTerms
oo_DEFINE_CLASS (FunctionTerms, Function)

	oo_INTEGER (numberOfCoefficients)
	oo_DOUBLE_VECTOR (coefficients, numberOfCoefficients)
	
	#if !oo_READING && !oo_WRITING
		oo_INTEGER (_capacity)
	#endif
		
	#if oo_READING
		_capacity = numberOfCoefficients;
	#endif

	#if oo_DECLARING
		// new methods:
			virtual double v_evaluate (double x);
			virtual dcomplex v_evaluate_z (dcomplex z);
			virtual void v_evaluateTerms (double x, double terms[]);
			virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
			virtual integer v_getDegree ();
	#endif
	
oo_END_CLASS (FunctionTerms)	
#undef ooSTRUCT


#define ooSTRUCT Spline
oo_DEFINE_CLASS (Spline, FunctionTerms)

	oo_INTEGER (degree)
	oo_INTEGER (numberOfKnots)
	oo_DOUBLE_VECTOR (knots, numberOfKnots)
	
	#if oo_DECLARING
		// overridden methods:
			virtual double v_evaluate (double x) override;
			virtual integer v_getDegree () override;
		// new methods:
			virtual integer v_getOrder ();
	#endif

oo_END_CLASS (Spline)	
#undef ooSTRUCT


/* End of file Polynomial_def.h */	
