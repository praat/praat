/* DataModeler_def.h
 *
 * Copyright (C) 2014-2018 David Weenink
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


#define ooSTRUCT DataModeler
oo_DEFINE_CLASS (DataModeler, Function)

	oo_INT (type)	// polynomial, legendre ...
	oo_INTEGER (numberOfDataPoints)
	oo_INTEGER (numberOfParameters)
	oo_VEC (x, numberOfDataPoints)
	oo_VEC (y, numberOfDataPoints)
	oo_VEC (sigmaY, numberOfDataPoints)
	oo_INTVEC (dataPointStatus, numberOfDataPoints)
	oo_VEC (parameter, numberOfParameters)
	oo_INTVEC (parameterStatus, numberOfParameters)
	oo_DOUBLE (tolerance)
	oo_INT (useSigmaY)
	oo_OBJECT (Strings, 0, parameterNames)
	oo_OBJECT (Covariance, 0, parameterCovariances)

	#if oo_DECLARING
		double (*f_evaluate) (DataModeler me, double x, VEC p);
		void (*f_evaluateBasisFunctions) (DataModeler me, double x, VEC term);

		void v_info ()
			override;
	#endif

	#if oo_COPYING
		DataModeler_setBasisFunctions (thee, thy type);
	#endif

	#if oo_READING
		 DataModeler_setBasisFunctions (this, type);
	#endif

oo_END_CLASS (DataModeler)
#undef ooSTRUCT


#define ooSTRUCT FormantModeler
oo_DEFINE_CLASS (FormantModeler, Function)

	oo_COLLECTION_OF (OrderedOf, trackmodelers, DataModeler, 0)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FormantModeler)
#undef ooSTRUCT


/* End of file DataModeler_def.h */	
