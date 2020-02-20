/* Spline_def.h
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

#define ooSTRUCT Spline
oo_DEFINE_CLASS (Spline, FunctionSeries)

	oo_INTEGER (degree)
	oo_INTEGER (numberOfKnots)
	oo_VEC (knots, numberOfKnots)
	
	#if oo_DECLARING
		double v_evaluate (double x)
			override;
		integer v_getDegree ()
			override;

		virtual integer v_getOrder ();
	#endif

oo_END_CLASS (Spline)	
#undef ooSTRUCT

/* End of file Spline_def.h */	
