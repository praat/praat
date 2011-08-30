/* Polynomial_def.h
 *
 * Copyright (C) 1993-2002 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 1999
 djmw 20020813 GPL header
*/

#define ooSTRUCT FunctionTerms
oo_DEFINE_CLASS (FunctionTerms, Function)

	oo_LONG (numberOfCoefficients)
	oo_DOUBLE_VECTOR (coefficients, numberOfCoefficients)

	#if oo_DECLARING
		// new methods:
			virtual double v_evaluate (double x);
			virtual void v_evaluate_z (dcomplex *z, dcomplex *p);
			virtual void v_evaluateTerms (double x, double terms[]);
			virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
			virtual long v_getDegree ();
	#endif
	
oo_END_CLASS (FunctionTerms)	
#undef ooSTRUCT


#define ooSTRUCT Spline
oo_DEFINE_CLASS (Spline, FunctionTerms)

	oo_LONG (degree)
	oo_LONG (numberOfKnots)
	oo_DOUBLE_VECTOR (knots, numberOfKnots)
	
	#if oo_DECLARING
		// overridden methods:
			virtual double v_evaluate (double x);
			virtual long v_getDegree ();
		// new methods:
			virtual long v_getOrder ();
	#endif

oo_END_CLASS (Spline)	
#undef ooSTRUCT


/* End of file Polynomial_def.h */	
