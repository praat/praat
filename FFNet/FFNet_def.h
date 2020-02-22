/* FFNet_def.h
 *
 * Copyright (C) 1994-2018 David Weenink
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
 djmw 19961104
 djmw 20020712 GPL header
 djmw 20060529 Added version number to oo_Collection
 djmw 20080122 float -> double
*/

#define ooSTRUCT FFNet
oo_DEFINE_CLASS (FFNet, Daata)

	oo_INTEGER (numberOfLayers)	/* number of layers */
	oo_FROM (1)
		oo_INTEGER (numberOfInputs)
		oo_INTEGER (numberOfOutputs)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			oo_INTVEC (numberOfUnitsInLayer, numberOfLayers + 1)
			numberOfInputs = numberOfUnitsInLayer [1];
			numberOfOutputs = numberOfUnitsInLayer [numberOfLayers + 1];
			for (integer ilayer = 1; ilayer <= numberOfLayers; ilayer ++)
				numberOfUnitsInLayer [ilayer] = numberOfUnitsInLayer [ilayer + 1];
			numberOfUnitsInLayer. resize (numberOfLayers);
		oo_VERSION_ELSE
			oo_INTVEC (numberOfUnitsInLayer, numberOfLayers)
		oo_VERSION_END
	#else
		oo_INTVEC (numberOfUnitsInLayer, numberOfLayers)
	#endif
	oo_INT (outputsAreLinear)
	oo_INT (nonLinearityType)
	oo_INT (costFunctionType)
	oo_COLLECTION (Categories, outputCategories, SimpleString, 0)
	oo_INTEGER (numberOfWeights)	/* number of weights */
	oo_VEC (w, numberOfWeights)
	
	#if ! oo_READING && ! oo_WRITING && ! oo_COMPARING
		oo_INTEGER (numberOfNodes)
		oo_INTEGER (dimension)

		#if oo_DECLARING
			double (*nonLinearity) (FFNet /* me */, double /* x */, double * /* deriv */);
    		void *nlClosure;
    		double (*costFunction) (FFNet /* me */, constVEC& /* target */);
			void *cfClosure;
		#endif
		
		#if oo_DECLARING
			oo_DOUBLE (accumulatedCost)
			oo_INTEGER (numberOfPatterns)
			oo_INTEGER (currentPattern)
			MAT inputPattern, targetActivation;
		#endif
		#if oo_DECLARING || oo_DESTROYING
			oo_OBJECT (Minimizer, 0, minimizer)
		#endif

		oo_VEC (activity, numberOfNodes)
		oo_INTVEC (isbias, numberOfNodes)
		oo_INTVEC (nodeFirst, numberOfNodes)
		oo_INTVEC (nodeLast, numberOfNodes)
		oo_INTVEC (wFirst, numberOfNodes)
		oo_INTVEC (wLast, numberOfNodes)
			
		oo_VEC (deriv, numberOfNodes)	
		oo_VEC (error, numberOfNodes)	
		oo_INTVEC (wSelected, numberOfWeights)
		oo_VEC (dw, numberOfWeights)
		oo_VEC (dwi, numberOfWeights)
	#endif

	#if oo_READING
		bookkeeping (this);
		FFNet_setNonLinearity (this, nonLinearityType);
		FFNet_setCostFunction (this, costFunctionType);
	#endif

	#if oo_COPYING
		thy nonLinearity = our nonLinearity;
    	thy nlClosure = our nlClosure;
    	thy costFunction = our costFunction;
		thy cfClosure = our cfClosure;
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FFNet)
#undef ooSTRUCT

/* End of file FFNet_def.h */	
