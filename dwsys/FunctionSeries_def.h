/* FunctionSeries_def.h
 *
 * Copyright (C) 1993-2020 David Weenink
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

#define ooSTRUCT FunctionSeries
oo_DEFINE_CLASS (FunctionSeries, Function)

	oo_INTEGER (numberOfCoefficients)
	oo_VEC (coefficients, numberOfCoefficients)
	
	#if ! oo_READING && ! oo_WRITING
		oo_INTEGER (_capacity)
	#endif
		
	#if oo_READING
		_capacity = numberOfCoefficients;
	#endif

	#if oo_DECLARING
		virtual double v_evaluate (double x);
		virtual dcomplex v_evaluate_z (dcomplex z);
		virtual void v_evaluateTerms (double x, VEC terms);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
		virtual integer v_getDegree ();
		virtual void extendCapacity (integer newCapacity) {
			if (_capacity < newCapacity) {
				coefficients. resize (newCapacity);
				_capacity = newCapacity;
			}
		}
	#endif
	
oo_END_CLASS (FunctionSeries)	
#undef ooSTRUCT

/* End of file FunctionSeries_def.h */
