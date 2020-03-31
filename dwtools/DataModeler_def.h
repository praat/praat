/* DataModeler_def.h
 *
 * Copyright (C) 2014-2020 David Weenink
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

#define ooSTRUCT DataModelerData
oo_DEFINE_STRUCT (DataModelerData)

	oo_DOUBLE (x)
	oo_DOUBLE (y)
	oo_DOUBLE (sigmaY)
	oo_ENUM (kDataModelerData, status)
	
oo_END_STRUCT (	DataModelerData)
#undef ooSTRUCT

#define ooSTRUCT DataModelerParameter
oo_DEFINE_STRUCT (DataModelerParameter)

	oo_DOUBLE (value)
	oo_ENUM (kDataModelerParameter, status)
	
oo_END_STRUCT (DataModelerParameter)
#undef ooSTRUCT
	
	
#define ooSTRUCT DataModeler
oo_DEFINE_CLASS (DataModeler, Function)

	oo_ENUM (kDataModelerFunction, type)	// polynomial, legendre ...
	oo_INTEGER (numberOfDataPoints)
	oo_INTEGER (numberOfParameters)
	oo_STRUCTVEC (DataModelerData, data, numberOfDataPoints)
	oo_STRUCTVEC (DataModelerParameter, parameters, numberOfParameters)
	oo_DOUBLE (tolerance)
	oo_ENUM (kDataModelerWeights, weighData)
	oo_OBJECT (Strings, 0, parameterNames)
	oo_OBJECT (Covariance, 0, parameterCovariances)

	#if oo_DECLARING
		double (*f_evaluate) (DataModeler me, double x, vector<structDataModelerParameter> p);
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

/* End of file DataModeler_def.h */	
