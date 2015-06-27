/* DataModeler_def.h */
/* David Weenink, 20140216 */

#define ooSTRUCT DataModeler
oo_DEFINE_CLASS (DataModeler, Function)
	oo_INT (type)	// polynomial, legendre ...
	oo_LONG (numberOfDataPoints)
	oo_LONG (numberOfParameters)
	oo_DOUBLE_VECTOR (x, numberOfDataPoints)
	oo_DOUBLE_VECTOR (y, numberOfDataPoints)
	oo_DOUBLE_VECTOR (sigmaY, numberOfDataPoints)
	oo_INT_VECTOR (dataPointStatus, numberOfDataPoints)
	oo_DOUBLE_VECTOR (parameter, numberOfParameters)
	oo_INT_VECTOR (parameterStatus, numberOfParameters)
	oo_DOUBLE (tolerance)
	oo_INT (useSigmaY)
	oo_OBJECT (Strings, 0, parameterNames)
	oo_OBJECT (Covariance, 0, parameterCovariances)
	#if oo_DECLARING
		double (*f_evaluate) (DataModeler me, double x, double p[]);
		void (*f_evaluateBasisFunctions) (DataModeler me, double x, double term[]);

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
	oo_COLLECTION (Ordered, datamodelers, DataModeler, 0)
	#if oo_DECLARING
		void v_info ()
			override;
	#endif
oo_END_CLASS (FormantModeler)
#undef ooSTRUCT

/* End of file DataModeler_def.h */	
