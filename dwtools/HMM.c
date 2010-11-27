/* HMM.c
 *
 * Copyright (C) 2010 David Weenink
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

#include "Distributions_and_Strings.h"
#include "HMM.h"
#include "Index.h"
#include "NUM2.h"
#include "Strings_extensions.h"

#include "oo_DESTROY.h"
#include "HMM_def.h"
#include "oo_COPY.h"
#include "HMM_def.h"
#include "oo_EQUAL.h"
#include "HMM_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "HMM_def.h"
#include "oo_WRITE_TEXT.h"
#include "HMM_def.h"
#include "oo_WRITE_BINARY.h"
#include "HMM_def.h"
#include "oo_READ_TEXT.h"
#include "HMM_def.h"
#include "oo_READ_BINARY.h"
#include "HMM_def.h"
#include "oo_DESCRIPTION.h"
#include "HMM_def.h"

/*
	Whenever a routine returns ln(p), the result for p=0 is -INFINITY.
	On IEEE floating point hardware this number behaves reasonably.
	This means that when the variable q equals INFINITY, q + a -> INFINITY,
	where a is a finite number.
*/

// helpers
int HMM_Observation_init (I, wchar_t *label, long numberOfComponents, long dimension, long storage);
HMM_Observation HMM_Observation_create (wchar_t *label, long numberOfComponents, long dimension, long storage);


int HMM_State_init (I, wchar_t *label);
HMM_State HMM_State_create (wchar_t *label);

HMM_BaumWelch HMM_BaumWelch_create (long nstates, long nsymbols, long capacity);
void HMM_BaumWelch_getGamma (HMM_BaumWelch me);
HMM_BaumWelch HMM_forward (HMM me, long *obs, long nt);
void HMM_BaumWelch_reInit (HMM_BaumWelch me);
void HMM_and_HMM_BaumWelch_getXi (HMM me, HMM_BaumWelch thee, long *obs);
void HMM_and_HMM_BaumWelch_reestimate (HMM me, HMM_BaumWelch thee);
void HMM_and_HMM_BaumWelch_addEstimate (HMM me, HMM_BaumWelch thee, long *obs);
void HMM_and_HMM_BaumWelch_forward (HMM me, HMM_BaumWelch thee, long *obs);
void HMM_and_HMM_BaumWelch_backward (HMM me, HMM_BaumWelch thee, long *obs);
void HMM_and_HMM_Viterbi_decode (HMM me, HMM_Viterbi thee, long *obs);
double HMM_getProbabilityOfObservations (HMM me, long *obs, long numberOfTimes);
TableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities);
StringsIndex HMM_and_HMM_StateSequence_to_StringsIndex (HMM me, HMM_StateSequence thee);


HMM_Viterbi HMM_Viterbi_create (long nstates, long ntimes);
HMM_Viterbi HMM_to_HMM_Viterbi (HMM me, long *obs, long ntimes);

// evaluate the numbers given to probabilities
static double *NUMwstring_to_probs (wchar_t *prob_string, long nwanted)
{
	double *p = NULL, psum = 0;
	long ip = 1;

	p = NUMdvector (1, nwanted);
	if (p == NULL) return NULL;

	for (wchar_t *token = Melder_firstToken (prob_string); token != NULL && ip <= nwanted; token = Melder_nextToken (), ip++)
	{
		double prob = Melder_atof (token);
		p[ip] = prob;
		psum += prob;
	}
	ip--;
	for (long i = 1; i <= ip; i++) p[i] /= psum; // to probabilities
	return p;
}

int NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b,
	double *x1, double *y1, double *x2, double *y2)
{
	double ca = a * a + 1, bmyc = (b - yc);
	double cb = 2 * (a * bmyc - xc);
	double cc = bmyc * bmyc + xc * xc - r * r;
	long nroots = NUMsolveQuadraticEquation (ca, cb, cc, x1, x2);
	if (nroots == 1) { *y1 = a * *x1 + b; *x2 = *x1; *y2 = *y1; }
	else if (nroots == 2)
	{
		if (*x1 > *x2 ) { double tmp = *x1; *x1 = *x2; *x2 = tmp; }
		*y1 = *x1 * a + b; *y2 = *x2 * a + b; }
	return nroots;
}

// D(l_1,l_2)=1/n( log p(O_2|l_1) - log p(O_2|l_2)
static double HMM_and_HMM_getCrossEntropy_asym (HMM me, HMM thee, long observationLength)
{
	double ce, ce2;
	HMM_ObservationSequence os = HMM_to_HMM_ObservationSequence (thee, 0, observationLength);
	if (os == NULL) return NUMundefined;
	ce = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, os);
	if (ce == NUMundefined || ce == INFINITY) return ce;
	ce2 = HMM_and_HMM_ObservationSequence_getCrossEntropy (thee, os);
	forget (os);
	if (ce2 == NUMundefined || ce2 == INFINITY) return ce2;
	return ce - ce2;
}

/**************** HMM_Observation ******************************/

class_methods (HMM_Observation, Data)
{
	class_method_local (HMM_Observation, destroy)
	class_method_local (HMM_Observation, copy)
	class_method_local (HMM_Observation, equal)
	class_method_local (HMM_Observation, canWriteAsEncoding)
	class_method_local (HMM_Observation, writeText)
	class_method_local (HMM_Observation, readText)
	class_method_local (HMM_Observation, writeBinary)
	class_method_local (HMM_Observation, readBinary)
	class_method_local (HMM_Observation, description)
	class_methods_end
}

int HMM_Observation_init (I, wchar_t *label, long numberOfComponents, long dimension, long storage)
{
	iam (HMM_Observation);
	my label = Melder_wcsdup (label);
	if (numberOfComponents > 0 && dimension > 0 &&
		((my gm = GaussianMixture_create (numberOfComponents, dimension, storage)) == NULL)) return 0;
	return 1;
}

HMM_Observation HMM_Observation_create (wchar_t *label, long numberOfComponents, long dimension, long storage)
{
	HMM_Observation me = new (HMM_Observation);
	if (me == NULL || ! HMM_Observation_init (me, label, numberOfComponents, dimension, storage)) forget (me);
	return me;
}

long Strings_getLongestSequence (Strings me, wchar_t *string, long *pos)
{
	long length = 0, longest = 0, lpos = 0;
	for (long i = 1; i <= my numberOfStrings; i++)
	{
		if (Melder_wcsequ (my strings[i], string))
		{
			if (length == 0) lpos = i;
			length++;
		}
		else
		{
			if (length > 0)
			{
				if (length > longest)
				{
					longest = length; *pos = lpos;
				}
				length = 0;
			}
		}
	}
	return length;
}

long StringsIndex_getLongestSequence (StringsIndex me, long index, long *pos)
{
	long length = 0, longest = 0, lpos = 0;
	for (long i = 1; i <= my numberOfElements; i++)
	{
		if (my classIndex[i] == index)
		{
			if (length == 0) lpos = i;
			length++;
		}
		else
		{
			if (length > 0)
			{
				if (length > longest)
				{
					longest = length; *pos = lpos;
				}
				length = 0;
			}
		}
	}
	return length;
}

/**************** HMM_State ******************************/

int HMM_State_init (I, wchar_t *label)
{
	iam (HMM_State);
	my label = Melder_wcsdup (label);
	return 1;
}

HMM_State HMM_State_create (wchar_t *label)
{
	HMM_State me = new (HMM_State);
	if (me == NULL || ! HMM_State_init (me, label)) forget (me);
	return me;
}

class_methods (HMM_State, Data)
{
	class_method_local (HMM_State, destroy)
	class_method_local (HMM_State, copy)
	class_method_local (HMM_State, equal)
	class_method_local (HMM_State, canWriteAsEncoding)
	class_method_local (HMM_State, writeText)
	class_method_local (HMM_State, readText)
	class_method_local (HMM_State, writeBinary)
	class_method_local (HMM_State, readBinary)
	class_method_local (HMM_State, description)
	class_methods_end
}

void HMM_State_setLabel (HMM_State me, wchar_t *label)
{
	Melder_free (my label);
	my label = Melder_wcsdup (label);
}

/**************** HMM_BaumWelch ******************************/

static void classHMM_BaumWelch_destroy (I)
{
		iam (HMM_BaumWelch);
		for (long it = 1; it <= my numberOfTimes; it++) NUMdmatrix_free (my xi[it], 1, 1);
		NUMpvector_free (my xi, 1);
		NUMdvector_free (my scale, 1);
		NUMdmatrix_free (my beta, 1, 1);
		NUMdmatrix_free (my alpha, 1, 1);
		NUMdmatrix_free (my gamma, 1, 1);
}

class_methods (HMM_BaumWelch, Data)
{
	class_method_local (HMM_BaumWelch, destroy)
	class_methods_end
}

HMM_BaumWelch HMM_BaumWelch_create (long nstates, long nsymbols, long capacity)
{
	HMM_BaumWelch me = new (HMM_BaumWelch);
	if (me == NULL) return NULL;
	my numberOfTimes = my capacity = capacity;
	my numberOfStates = nstates;
	my numberOfSymbols = nsymbols;
	if (((my alpha = NUMdmatrix (1, nstates, 1, capacity)) == NULL) ||
		((my beta = NUMdmatrix (1, nstates, 1, capacity)) == NULL) ||
		((my scale = NUMdvector (1, capacity)) == NULL) ||
		((my xi = NUMpvector (1, capacity)) == NULL) ||
		((my aij_num = NUMdmatrix (0, nstates, 1, nstates + 1)) == NULL) ||
		((my aij_denom = NUMdmatrix (0, nstates, 1, nstates + 1)) == NULL) ||
		((my bik_num = NUMdmatrix (1, nstates, 1, nsymbols)) == NULL) ||
		((my bik_denom = NUMdmatrix (1, nstates, 1, nsymbols)) == NULL) ||
		((my gamma = NUMdmatrix (1, nstates, 1, capacity)) == NULL)) goto end;
	for (long it = 1; it <= capacity; it++)
	{
		my xi[it] = NUMdmatrix (1, nstates, 1, nstates);
		if (my xi[it] == NULL) goto end;
	}
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

int HMM_BaumWelch_increaseCapacity (HMM_BaumWelch me, long factor)
{
	long new_capacity = factor * my capacity;
	double **alpha = NULL, **beta = NULL, *scale = NULL, ***xi = NULL, **gamma = NULL;
	if (((alpha = NUMdmatrix (1, my numberOfStates, 1, new_capacity)) == NULL) ||
		((beta = NUMdmatrix (1, my numberOfStates, 1, new_capacity)) == NULL) ||
		((scale = NUMdvector (1, new_capacity)) == NULL) ||
		((xi = NUMpvector (1, new_capacity)) == NULL) ||
		((gamma = NUMdmatrix (1, my numberOfStates, 1, new_capacity)) == NULL)) goto end;
	for (long it = 1; it <= new_capacity; it++)
	{
		xi[it] = NUMdmatrix (1, my numberOfStates, 1, my numberOfStates);
		if (my xi[it] == NULL) goto end;
	}
	// copy data etc...
end:
	if (Melder_hasError ())
	{
	}
	return 1;
}

void HMM_BaumWelch_getGamma (HMM_BaumWelch me)
{
	for (long it = 1; it <= my numberOfTimes; it++)
	{
		double sum = 0.0;
		for (long is = 1; is <= my numberOfStates; is++)
		{
			my gamma[is][it] = my alpha[is][it] * my beta[is][it];
			sum += my gamma[is][it];
		}

		for (long is = 1; is <= my numberOfStates; is++)
		{
			my gamma[is][it] /= sum;
		}
	}
}

/**************** HMM_Viterbi ******************************/

class_methods (HMM_Viterbi, Data)
{
	class_method_local (HMM_Viterbi, destroy)
	class_method_local (HMM_Viterbi, copy)
	class_method_local (HMM_Viterbi, equal)
	class_method_local (HMM_Viterbi, canWriteAsEncoding)
	class_method_local (HMM_Viterbi, writeText)
	class_method_local (HMM_Viterbi, readText)
	class_method_local (HMM_Viterbi, writeBinary)
	class_method_local (HMM_Viterbi, readBinary)
	class_method_local (HMM_Viterbi, description)
	class_methods_end
}

HMM_Viterbi HMM_Viterbi_create (long nstates, long ntimes)
{
	HMM_Viterbi me = new (HMM_Viterbi);
	if (me == NULL) return NULL;
	my numberOfTimes = ntimes;
	my numberOfStates = nstates;
	if (((my viterbi = NUMdmatrix (1, nstates, 1 , ntimes)) == NULL) ||
		((my bp = NUMlmatrix (1, nstates, 1 , ntimes)) == NULL) ||
		((my path = NUMlvector (1, ntimes)) == NULL)) forget (me);
	return me;
}

/******************* HMM_ObservationSequence & HMM_StateSequence ***/

class_methods (HMM_ObservationSequence, Table)
{
	class_methods_end
}

HMM_ObservationSequence HMM_ObservationSequence_create (long numberOfItems, long dataLength)
{
	HMM_ObservationSequence me = new (HMM_ObservationSequence);
	if (! Table_initWithoutColumnNames (me, numberOfItems, dataLength + 1)) forget (me);
	return me;
}

long HMM_ObservationSequence_getNumberOfObservations (HMM_ObservationSequence me)
{
	return my rows -> size;
}

void HMM_ObservationSequence_removeObservation (HMM_ObservationSequence me, long index)
{
	Table_removeRow ((Table) me, index);
}

Strings HMM_ObservationSequence_to_Strings (HMM_ObservationSequence me)
{
	long numberOfStrings = my rows -> size;
	Strings thee = new (Strings);
	if (me == NULL || ((thy strings = NUMpvector (1, numberOfStrings)) == NULL)) goto end;
	for (long i = 1; i <= numberOfStrings; i++)
	{
			thy strings[i] = Melder_wcsdup (Table_getStringValue ((Table) me, i, 1));
			(thy numberOfStrings) ++;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

HMM_ObservationSequence Strings_to_HMM_ObservationSequence (Strings me)
{
	HMM_ObservationSequence thee = HMM_ObservationSequence_create (my numberOfStrings, 0);
	if (thee == NULL) return NULL;
	for (long i = 1; i <= my numberOfStrings; i++)
	{
		if (! Table_setStringValue ((Table) thee, i, 1, my strings[i])) goto end;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

StringsIndex HMM_ObservationSequence_to_StringsIndex (HMM_ObservationSequence me)
{
	StringsIndex thee = NULL;
	Strings s = HMM_ObservationSequence_to_Strings (me);
	if (s == NULL || ((thee = Strings_to_StringsIndex (s)) == NULL)) forget (thee);
	forget (s);
	return thee;
}

long HMM_and_HMM_ObservationSequence_getLongestSequence (HMM me, HMM_ObservationSequence thee, long symbolNumber)
{
	StringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
}

class_methods (HMM_ObservationSequences, Collection)
{
	class_methods_end
}

HMM_ObservationSequences HMM_ObservationSequences_create ()
{
	HMM_ObservationSequences me = new (HMM_ObservationSequences);
	if ((me == NULL) || ! Collection_init (me, classHMM_ObservationSequence, 10000)) forget (me);
	return me;
}

long HMM_ObservationSequences_getLongestSequence (HMM_ObservationSequences me)
{
	long longest = 0;
	for (long i = 1; i <= my size; i++)
	{
		HMM_ObservationSequence thee = my item[i];
		if (thy rows -> size > longest) longest = thy rows -> size;
	}
	return longest;
}

class_methods (HMM_StateSequence, Strings)
{
	class_methods_end
}

HMM_StateSequence HMM_StateSequence_create (long numberOfItems)
{
	HMM_StateSequence me = new (HMM_StateSequence);
	if (me == NULL || ((my strings = NUMpvector (1, numberOfItems)) == NULL)) forget (me);
	return me;
}

Strings HMM_StateSequence_to_Strings (HMM_StateSequence me)
{
	Strings thee = Data_copy (me);
	if (thee == NULL) return NULL;
	Thing_overrideClass (thee, classStrings);
	return thee;
}


/**************** HMM ******************************/

static void classHMM_info (I)
{
	iam (HMM);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of states: ", Melder_integer (my numberOfStates));
	for (long i = 1; i <= my numberOfStates; i++)
	{
		HMM_State hmms = my states -> item[i];
		MelderInfo_writeLine2 (L"  ", hmms -> label);
	}
	MelderInfo_writeLine2 (L"Number of symbols: ", Melder_integer (my numberOfObservationSymbols));
	for (long i = 1; i <= my numberOfObservationSymbols; i++)
	{
		HMM_Observation hmms = my observationSymbols -> item[i];
		MelderInfo_writeLine2 (L"  ", hmms -> label);
	}
}

class_methods (HMM, Data)
{
	class_method_local (HMM, info)
	class_method_local (HMM, destroy)
	class_method_local (HMM, copy)
	class_method_local (HMM, equal)
	class_method_local (HMM, canWriteAsEncoding)
	class_method_local (HMM, writeText)
	class_method_local (HMM, readText)
	class_method_local (HMM, writeBinary)
	class_method_local (HMM, readBinary)
	class_method_local (HMM, description)
	class_methods_end
}

int HMM_init (HMM me, long numberOfStates, long numberOfObservationSymbols, int leftToRight)
{
	my numberOfStates = numberOfStates;
	my numberOfObservationSymbols = numberOfObservationSymbols;
	my componentStorage = 1;
	my leftToRight = leftToRight;
	if (((my states = Ordered_create ()) == NULL) ||
		((my observationSymbols = Ordered_create ()) == NULL) ||
		((my transitionProbs = NUMdmatrix (0, numberOfStates, 1, numberOfStates + 1)) == NULL) ||
		(numberOfObservationSymbols > 0 && (my emissionProbs = NUMdmatrix (1, numberOfStates, 1, numberOfObservationSymbols)) == NULL)) return 0;
	return 1;
}

HMM HMM_create (int leftToRight, long numberOfStates, long numberOfObservationSymbols)
{
	HMM me = new (HMM);
	if ((me == NULL) || ! HMM_init (me, numberOfStates, numberOfObservationSymbols, leftToRight)) goto end;
	HMM_setDefaultStates (me);
	HMM_setDefaultObservations (me);
	HMM_setDefaultTransitionProbs (me);
	HMM_setDefaultStartProbs (me);
	HMM_setDefaultEmissionProbs (me);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}
void HMM_setDefaultStates (HMM me)
{
	MelderString label = { 0 };
	for (long i = 1; i <= my numberOfStates; i++)
	{
		MelderString_append2 (&label, L"S", Melder_integer (i));
		HMM_State hmms = HMM_State_create (label.string);
		HMM_addState (me, hmms);
		MelderString_empty (&label);
	}
}

HMM HMM_createFullContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfFeatureStreams, long *dimensionOfStream, long *numberOfGaussiansforStream)
{

}

HMM HMM_createContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
	long numberOfMixtureComponentsPerSymbol, long componentDimension, long componentStorage)
{
	MelderString label = { 0 };
	HMM me = new (HMM);
	if ((me == NULL) || ! HMM_init (me, numberOfStates, numberOfObservationSymbols, leftToRight)) goto end;
	my numberOfMixtureComponents = numberOfMixtureComponentsPerSymbol;
	my componentDimension = componentDimension;
	my componentStorage = componentStorage;
	for (long i = 1; i <= numberOfStates; i++)
	{
		MelderString_append2 (&label, L"S", Melder_integer (i));
		HMM_State state = HMM_State_create (label.string);
		if (state == NULL || ! HMM_addState (me, state)) goto end;
		MelderString_empty (&label);
	}
	for (long j = 1; j <= numberOfObservationSymbols; j++)
	{
		MelderString_append2 (&label, L"s", Melder_integer (j));
		HMM_Observation obs = HMM_Observation_create (label.string, numberOfMixtureComponentsPerSymbol, componentDimension, componentStorage);
		if (obs == NULL || ! HMM_addObservation (me, obs)) goto end;
		MelderString_empty (&label);
	}
	HMM_setDefaultTransitionProbs (me);
	HMM_setDefaultStartProbs (me);
	HMM_setDefaultEmissionProbs (me);
	HMM_setDefaultMixingProbabilities (me);

end:
	if (Melder_hasError ()) forget (me);
	return me;
}

// for a simple non-hidden model leave either states empty or symbols empty !!!
HMM HMM_createSimple (int leftToRight, wchar_t *states_string, wchar_t *symbols_string)
{
	HMM me = new (HMM);
	wchar_t *states = states_string;
	wchar_t *symbols = symbols_string;
	long numberOfStates = Melder_countTokens (states_string);
	long numberOfObservationSymbols = Melder_countTokens (symbols_string);

	if (numberOfStates > 0)
	{
		if (numberOfObservationSymbols <= 0)
		{
			numberOfObservationSymbols = numberOfStates;
			symbols = states_string;
			my notHidden = 1;
		}
	}
	else if (numberOfObservationSymbols > 0)
	{
		numberOfStates = numberOfObservationSymbols;
		states = symbols_string;
		my notHidden = 1;
	}
	else return Melder_errorp1 (L"No states and symbols.");

	if ((me == NULL) || ! HMM_init (me, numberOfStates, numberOfObservationSymbols, leftToRight)) goto end;

	for (wchar_t *token = Melder_firstToken (states); token != NULL; token = Melder_nextToken ())
	{
		HMM_State state = HMM_State_create (token);
		if (state == NULL || ! HMM_addState (me, state)) goto end;
	}
	for (wchar_t *token = Melder_firstToken (symbols); token != NULL; token = Melder_nextToken ())
	{
		HMM_Observation symbol = HMM_Observation_create (token, 0, 0, 0);
		if (symbol == NULL || ! HMM_addObservation (me, symbol)) goto end;
	}
	HMM_setDefaultTransitionProbs (me);
	HMM_setDefaultStartProbs (me);
	HMM_setDefaultEmissionProbs (me);

end:
	if (Melder_hasError ()) forget (me);
	return me;
}

void HMM_setDefaultObservations (HMM me)
{
	MelderString symbol = { 0 };
	wchar_t *def = my notHidden ? L"S" : L"s";
	for (long i = 1; i <= my numberOfObservationSymbols; i++)
	{
		MelderString_append2 (&symbol, def, Melder_integer (i));
		HMM_Observation hmms = HMM_Observation_create (symbol.string, 0, 0, 0);
		HMM_addObservation (me, hmms);
		MelderString_empty (&symbol);
	}
}

void HMM_setDefaultTransitionProbs (HMM me)
{
	for (long i = 1; i <= my numberOfStates; i++)
	{
		double p = my leftToRight ? 1.0 / (my numberOfStates - i + 1) : 1.0 / my numberOfStates;
		for (long j = 1; j <= my numberOfStates; j++) my transitionProbs[i][j] = my leftToRight && j < i ? 0 : p;
	}
	// leftToRight must have end state!
	if (my leftToRight) my transitionProbs[my numberOfStates][my numberOfStates] =
		my transitionProbs[my numberOfStates][my numberOfStates + 1] = 0.5;
}

void HMM_setDefaultStartProbs (HMM me)
{
	double p = 1.0 / my numberOfStates;
	for (long j = 1; j <= my numberOfStates; j++)
		my transitionProbs[0][j] = p;
}

void HMM_setDefaultEmissionProbs (HMM me)
{
	double p = 1.0 / my numberOfObservationSymbols;
	for (long i = 1; i <= my numberOfStates; i++)
		for (long j = 1; j <= my numberOfObservationSymbols; j++)
		{
			my emissionProbs[i][j] = my notHidden ? (i == j ? 1 : 0) : p;
		}
}

void HMM_setDefaultMixingProbabilities (HMM me)
{
	double mp = 1.0 / my numberOfMixtureComponents;
	for (long is = 1; is <= my numberOfObservationSymbols; is++)
	{
		HMM_Observation hmmo = my observationSymbols -> item[is];
		for (long im = 1; im <= my numberOfMixtureComponents; im++) hmmo -> gm -> mixingProbabilities[im] = mp;
	}
}

int HMM_setStartProbabilities (HMM me, wchar_t *probs)
{
	double *p = NUMwstring_to_probs (probs, my numberOfStates);
	if (p == NULL) return 0;
	for (long i = 1; i <= my numberOfStates; i++) my transitionProbs[0][i] = p[i];
	NUMdvector_free (p, 1);
	return 1;
}

int HMM_setTransitionProbabilities (HMM me, long state_number, wchar_t *state_probs)
{
	if (state_number > my states -> size) return Melder_error1 (L"State number too large.");
	double *p = NUMwstring_to_probs (state_probs, my numberOfStates + 1); // 1 extra for final state
	if (p == NULL) return 0;
	for (long i = 1; i <= my numberOfStates + 1; i++) { my transitionProbs[state_number][i] = p[i]; }
	NUMdvector_free (p, 1);
	return 1;
}

int HMM_setEmissionProbabilities (HMM me, long state_number, wchar_t *emission_probs)
{
	if (state_number > my states -> size) return Melder_error1 (L"State number too large.");
	if (my notHidden) return Melder_error1 (L"The emission probs of this model are fixed.");
	double *p = NUMwstring_to_probs (emission_probs, my numberOfObservationSymbols);
	for (long i = 1; i <= my numberOfObservationSymbols; i++)
	{
		my emissionProbs[state_number][i] = p[i];
	}
	NUMdvector_free (p, 1);
	return 1;
}

int HMM_addObservation (HMM me, thou)
{
	thouart (HMM_Observation);
	long ns = my observationSymbols -> size + 1;
	if (ns > my numberOfObservationSymbols) return Melder_error1 (L"Observation list is full.");
	return Ordered_addItemPos (my observationSymbols, thee, ns);
}

int HMM_addState (HMM me, thou)
{
	thouart (HMM_State);
	long ns = my states -> size + 1;
	if (ns > my numberOfStates) return Melder_error1 (L"States list is full.");
	return Ordered_addItemPos (my states, thee, ns);
}

TableOfReal HMM_extractTransitionProbabilities (HMM me)
{
	TableOfReal thee = TableOfReal_create (my numberOfStates + 1, my numberOfStates + 1);
	if (thee == NULL) return NULL;
	for (long is = 1; is <= my numberOfStates; is++)
	{
		HMM_State hmms = my states -> item[is];
		TableOfReal_setRowLabel (thee, is + 1, hmms -> label);
		TableOfReal_setColumnLabel (thee, is, hmms -> label);
		for (long js = 1; js <= my numberOfStates; js++) thy data[is + 1][js] = my transitionProbs[is][js];
	}
	TableOfReal_setRowLabel (thee, 1, L"START");
	TableOfReal_setColumnLabel (thee, my numberOfStates + 1, L"END");
	for (long is = 1; is <= my numberOfStates; is++)
	{
		thy data[1][is] = my transitionProbs[0][is];
		thy data[is + 1][my numberOfStates + 1] = my transitionProbs[is][my numberOfStates + 1];
	}
	return thee;
}

TableOfReal HMM_extractEmissionProbabilities (HMM me)
{
	TableOfReal thee = TableOfReal_create (my numberOfStates, my numberOfObservationSymbols);
	if (thee == NULL) return NULL;
	for (long js = 1; js <= my numberOfObservationSymbols; js++)
	{
		HMM_Observation hmms = my observationSymbols -> item[js];
		TableOfReal_setColumnLabel (thee, js, hmms -> label);
	}
	for (long is = 1; is <= my numberOfStates; is++)
	{
		HMM_State hmms = my states -> item[is];
		TableOfReal_setRowLabel (thee, is, hmms -> label);
		for (long js = 1; js <= my numberOfObservationSymbols; js++) thy data[is][js] = my emissionProbs[is][js];
	}
	return thee;
}

double HMM_getExpectedValueOfDurationInState (HMM me, long istate)
{
	if (istate < 0 || istate > my numberOfStates) return NUMundefined;
	return 1 / (1 - my transitionProbs[istate][istate]);
}

double HMM_getProbabilityOfStayingInState (HMM me, long istate, long numberOfTimeUnits)
{
	if (istate < 0 || istate > my numberOfStates) return NUMundefined;
	return pow(my transitionProbs[istate][istate], numberOfTimeUnits - 1) * (1 - my transitionProbs[istate][istate]);
}

double HMM_and_HMM_getCrossEntropy (HMM me, HMM thee, long observationLength, int symmetric)
{
	double ce1 = HMM_and_HMM_getCrossEntropy_asym (me, thee, observationLength);
	if (! symmetric || ce1 == NUMundefined || ce1 == INFINITY) return ce1;
	double ce2 = HMM_and_HMM_getCrossEntropy_asym (thee, me, observationLength);
	if (ce2 == NUMundefined || ce2 == INFINITY) return ce2;
	return (ce1 + ce2) / 2;
}

double HMM_and_HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM thee, HMM_ObservationSequence him)
{
	double ce1 = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, him);
	if (ce1 == NUMundefined || ce1 == INFINITY) return ce1;
	double ce2 = HMM_and_HMM_ObservationSequence_getCrossEntropy (thee, him);
	if (ce2 == NUMundefined || ce2 == INFINITY) return ce2;
	return (ce1 + ce2) / 2;
}

void HMM_draw (HMM me, Graphics g, int garnish)
{
	long is, js;
	double *xs = NULL, *ys = NULL;
	double xwidth = sqrt (my numberOfStates);
	double rstate = 0.3 / xwidth, r = xwidth / 3.0;
	double xmax = 1.2 * xwidth / 2, xmin = -xmax, ymin = xmin, ymax = xmax;

	if (((xs = NUMdvector (1, my numberOfStates)) == NULL) ||
		((ys = NUMdvector (1, my numberOfStates)) == NULL)) goto end;

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	// heuristic: all states on a circle until we have a better graph drawing algorithm.
	xs[1] = ys[1] = 0;
	if (my numberOfStates > 1)
	{
		for (is = 1; is <= my numberOfStates; is++)
		{
			double alpha = - NUMpi + NUMpi * 2 * (is - 1) / my numberOfStates;
			xs[is] = r * cos (alpha); ys[is] = r * sin (alpha);
		}
	}
	// reorder the positions such that state number 1 is most left and last state number is right.
	// if > 5 may be one in the middle with the most connections
	// ...
	// find fontsize
	int fontSize = Graphics_inqFontSize (g);
	wchar_t *widest_label = L"";
	double max_width = 0;
	for (is = 1; is <= my numberOfStates; is++)
	{
		HMM_State hmms = my states -> item[is];
		double w = hmms -> label == NULL ? 0 : Graphics_textWidth (g, hmms -> label);
		if (w > max_width)
		{
			widest_label = hmms -> label;
			max_width = w;
		}
	}
	int new_fontSize = fontSize;
	while (max_width > 2 * rstate && new_fontSize > 4)
	{
		new_fontSize --;
		Graphics_setFontSize (g, new_fontSize);
		max_width = Graphics_textWidth (g, widest_label);
	}
	Graphics_setFontSize (g, new_fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (is = 1; is <= my numberOfStates; is++)
	{
		HMM_State hmms = my states -> item[is];
		Graphics_circle (g, xs[is], ys[is], rstate);
		Graphics_text (g, xs[is], ys[is], hmms -> label);
	}

	// draw connections from is to js
	// 1 -> 2 / 2-> : increase / decrease angle between 1 and 2 with pi /10
	// use cos(a+b) and cos(a -b) rules
	double cosb = cos (NUMpi/10), sinb = sin (NUMpi / 10);
	for (is = 1; is <= my numberOfStates; is++)
	{
		double x1 = xs[is], y1 = ys[is];
		for (js = 1; js <= my numberOfStates; js++)
		{
			if (my transitionProbs[is][js] > 0 && is != js)
			{
				double x2 = xs[js], y2 = ys[js];
				double dx = x2 - x1, dy = y2 - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				double cosabp = cosa * cosb - sina * sinb, cosabm = cosa * cosb + sina * sinb;
				double sinabp = cosa * sinb + sina * cosb, sinabm = -cosa * sinb + sina * cosb;
				Graphics_arrow (g, x1 + rstate * cosabp, y1 + rstate * sinabp, x2 - rstate * cosabm, y2 - rstate * sinabm);
			}
			if (is == js)
			{
				double dx = - x1, dy = - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				Graphics_doubleArrow (g, x1 - rstate * cosa, y1 - rstate * sina, x1 - 1.4 * rstate * cosa, y1 - 1.4 * rstate * sina);
			}
		}
	}
	if (garnish) Graphics_drawInnerBox (g);
end:
	NUMdvector_free (xs, 1);
	NUMdvector_free (ys, 1);
}

void HMM_unExpandPCA (HMM me)
{
	if (my componentDimension <= 0) return; // nothing to do
	for (long is = 1; is <= my numberOfObservationSymbols; is++)
	{
		HMM_Observation s = my observationSymbols -> item[is];
		GaussianMixture_unExpandPCA (s -> gm);
	}
}

HMM_ObservationSequence HMM_to_HMM_ObservationSequence (HMM me, long startState, long numberOfItems)
{
	HMM_ObservationSequence thee = NULL;
	long istate = startState, isymbol;
	double *obs = NULL, *buf = NULL;

	if ((thee = HMM_ObservationSequence_create (numberOfItems, my componentDimension)) == NULL) goto end;

	if (my componentDimension > 0 &&
		(((obs = NUMdvector (1, my componentDimension)) == NULL) ||
		((buf = NUMdvector (1, my componentDimension)) == NULL))) goto end;

	if (startState == 0)
	{
		istate = NUMgetIndexFromProbability (my transitionProbs[0], my numberOfStates, NUMrandomUniform (0,1));
	}
	for (long i = 1; i <= numberOfItems; i++)
	{
		// Emit a symbol from istate

		isymbol = NUMgetIndexFromProbability (my emissionProbs[istate], my numberOfObservationSymbols, NUMrandomUniform (0,1));
		HMM_Observation s = my observationSymbols -> item[isymbol];

		if (my componentDimension > 0)
		{
			wchar_t *name;
			if (! GaussianMixture_generateOneVector (s -> gm, obs, name, buf)) goto end;
			for (long j = 1; j <= my componentDimension; j++) Table_setNumericValue ((Table) thee, i, 1 + j, obs[j]);
		}

		if (! Table_setStringValue ((Table) thee, i, 1, s -> label)) goto end;

		// get next state

		istate = NUMgetIndexFromProbability (my transitionProbs[istate], my numberOfStates + 1, NUMrandomUniform (0,1));
		if (istate == my numberOfStates + 1) // final state
		{
			for (long j = numberOfItems; j > i; j--) HMM_ObservationSequence_removeObservation (thee, j);
			break;
		}
	}
end:
	if (Melder_hasError ()) forget (thee);
	if (my componentDimension > 0)
	{
		NUMdvector_free (obs, 1); NUMdvector_free (buf, 1); HMM_unExpandPCA (me);
	}
	return thee;
}

HMM_BaumWelch HMM_forward (HMM me, long *obs, long nt)
{
	HMM_BaumWelch thee = HMM_BaumWelch_create (my numberOfStates, my numberOfObservationSymbols, nt);
	if (thee == NULL) return NULL;
	HMM_and_HMM_BaumWelch_forward (me, thee, obs);
	return thee;
}

HMM_Viterbi HMM_to_HMM_Viterbi (HMM me, long *obs, long ntimes)
{
	HMM_Viterbi thee = HMM_Viterbi_create (my numberOfStates, ntimes);
	if ( thee == NULL) return NULL;
	HMM_and_HMM_Viterbi_decode (me, thee, obs);
	return thee;
}

void HMM_BaumWelch_reInit (HMM_BaumWelch me)
{
	my totalNumberOfSequences = 0;
	my lnProb = 0;

	/*
		The _num and _denum matrices are asigned as += in the iteration loop and therefore need to be zeroed
		at the start of each new iteration.
		The elements of alpha, beta, scale, gamma & xi are always calculated directly and need not be
		initialised.
	*/
	for (long is = 0; is <= my numberOfStates; is++)
	{
		for (long js = 1; js <= my numberOfStates + 1; js++)
		{
			my aij_num[is][js] = 0;
			my aij_denom[is][js] = 0;
		}
	}
	for (long is = 1; is <= my numberOfStates; is++)
	{
		for (long js = 1; js <= my numberOfSymbols; js++)
		{
			my bik_num[is][js] = 0;
			my bik_denom[is][js] = 0;
		}
	}
}

int HMM_and_HMM_ObservationSequences_learn (HMM me, HMM_ObservationSequences thee, double delta_lnp, double minProb)
{
	// act as if all observation sequences are in memory
	double lnp;
	long iter = 0, capacity = HMM_ObservationSequences_getLongestSequence (thee);
	HMM_BaumWelch bw = HMM_BaumWelch_create (my numberOfStates, my numberOfObservationSymbols, capacity);
	bw -> minProb = minProb;
	if (bw == NULL) goto end;
	MelderInfo_open ();
	do
	{
		lnp = bw -> lnProb;
		HMM_BaumWelch_reInit (bw);
		for (long ios = 1; ios <= thy size; ios++)
		{
			MelderInfo_writeLine2 (L"Observation sequence: ", Melder_integer (ios));
			StringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thy item[ios]);
			if (si == NULL) goto endfor;
			long *obs = si -> classIndex, nobs = si -> numberOfElements; // convenience
			/*
				Interpretation of unknowns: end of sequence
			*/
			long istart = 1, iend = nobs;
			while (istart <= nobs)
			{
				while (istart <= nobs && obs[istart] == 0) { istart++; };
				if (istart > nobs) break;
				iend = istart + 1;
				while (iend <= nobs && obs[iend] != 0) { iend++; }
				iend --;
				bw -> numberOfTimes = iend - istart + 1;
				(bw -> totalNumberOfSequences)++;
				MelderInfo_writeLine2 (L"  sub observation: ", Melder_integer (bw -> totalNumberOfSequences));
				HMM_and_HMM_BaumWelch_forward (me, bw, obs + istart - 1); // get new alphas
				HMM_and_HMM_BaumWelch_backward (me, bw, obs + istart - 1); // get new betas
				HMM_BaumWelch_getGamma (bw);
				HMM_and_HMM_BaumWelch_getXi (me, bw, obs + istart - 1);
				HMM_and_HMM_BaumWelch_addEstimate (me, bw, obs + istart - 1);
				istart = iend + 1;
			}
endfor:
			forget (si);
			if (Melder_hasError ()) goto end;
		}
		// we have processed all observation sequences, now it time to estimate new probabilities.
		iter++;
		HMM_and_HMM_BaumWelch_reestimate (me, bw);
		MelderInfo_writeLine4 (L"Iteration: ", Melder_integer (iter), L" ln(prob): ", Melder_double (bw -> lnProb));
	} while (fabs((lnp - bw -> lnProb) / bw -> lnProb) > delta_lnp);
	MelderInfo_writeLine1 (L"******** Learning summary *********");
	MelderInfo_writeLine3 (L"  Processed ", Melder_integer (thy size), L" sequences,");
	MelderInfo_writeLine3 (L"  consisting of ", Melder_integer (bw -> totalNumberOfSequences), L" observation sequences.");
	MelderInfo_writeLine3 (L"  Longest observation sequence had ", Melder_integer (capacity), L" items");
end:
	MelderInfo_close ();
	forget (bw);
	return 1;
}


// xc1 < xc2
void HMM_and_HMM_StateSequence_drawTrellis (HMM me, HMM_StateSequence thee, Graphics g, int connect, int garnish)
{
	long numberOfTimes = thy numberOfStrings;
	StringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
	double xmin = 0, xmax = numberOfTimes + 1, ymin = 0.5, ymax = my numberOfStates + 0.5;
	if (si == NULL) return;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double r = 0.2 / (numberOfTimes > my numberOfStates ? numberOfTimes : my numberOfStates);

	for (long it = 1; it <= numberOfTimes; it++)
	{
		for (long js = 1; js <= my numberOfStates; js++)
		{
			double xc = it, yc = js, x2 = it, y2 = js;
			Graphics_circle (g, xc, yc, r);
			if (it > 1)
			{
				for (long is = 1; is <= my numberOfStates; is++)
				{
					bool indexedConnection = si -> classIndex[it - 1] == is && si -> classIndex[it] == js;
					Graphics_setLineWidth (g, indexedConnection ? 2 : 1);
					Graphics_setLineType (g, indexedConnection ? Graphics_DRAWN : Graphics_DOTTED);
					double x1 = it - 1, y1 = is;
					if (connect || indexedConnection)
					{
						double a = (y1 - y2) / (x1 - x2), b = y1 - a * x1;
						// double xs11 = x1 - r / (a * a + 1), ys11 = a * xs11 + b;
						double xs12 = x1 + r / (a * a + 1), ys12 = a * xs12 + b;
						double xs21 = x2 - r / (a * a + 1), ys21 = a * xs21 + b;
						// double xs22 = x2 + r / (a * a + 1), ys22 = a * xs22 + b;
						Graphics_line (g, xs12, ys12, xs21, ys21);
					}
				}
			}
		}
	}
	Graphics_unsetInner (g);
	Graphics_setLineWidth (g, 1);
	Graphics_setLineType (g, Graphics_DRAWN);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		for (long js = 1; js <= my numberOfStates; js++)
		{
			HMM_State hmms = my states -> item[js];
			Graphics_markLeft (g, js, 0, 0, 0, hmms -> label);
		}
		Graphics_marksBottomEvery (g, 1, 1, 1, 1, 0);
		Graphics_textBottom (g, 1, L"Index");
	}
	forget (si);
}

void HMM_and_HMM_BaumWelch_getXi (HMM me, HMM_BaumWelch thee, long *obs)
{

	for (long it = 1; it <= thy numberOfTimes - 1; it++)
	{
		double sum = 0.0;
		for (long is = 1; is <= thy numberOfStates; is++)
		{
			for (long js = 1; js <= thy numberOfStates; js++)
			{
				thy xi[it][is][js] = thy alpha[is][it] * thy beta[js][it + 1] *
					my transitionProbs[is][js] * my emissionProbs[js][ obs[it + 1] ];
				sum += thy xi[it][is][js];
			}
		}
		for (long is = 1; is <= my numberOfStates; is++)
			for (long js = 1; js <= my numberOfStates; js++)
				thy xi[it][is][js]  /= sum;
	}
}

void HMM_and_HMM_BaumWelch_addEstimate (HMM me, HMM_BaumWelch thee, long *obs)
{
	long is, js, it, k;

	for (is = 1; is <= my numberOfStates; is++)
	{
		// only for valid start states with p > 0
		if (my transitionProbs[0][is] > 0)
		{
			thy aij_num[0][is] += thy gamma[is][1];
			thy aij_denom[0][is] += 1;
		}
	}

	for (is = 1; is <= my numberOfStates; is++)
	{
		double gammasum = 0.0;
		for (it = 1; it <= thy numberOfTimes - 1; it++)
		{
			gammasum += thy gamma[is][it];
		}

		for (js = 1; js <= my numberOfStates; js++)
		{
			double xisum = 0.0;
			for (it = 1; it <= thy numberOfTimes - 1; it++)
			{
				xisum += thy xi[it][is][js];
			}
			// zero probs signal invalid connections, don't reestimate
			if (my transitionProbs[is][js] > 0)
			{
				thy aij_num[is][js] += xisum;
				thy aij_denom[is][js] += gammasum;
			}
		}

		/*
			Only reestimate the emissionProbs for a hidden markov model.
			A not hidden model is emulated with fixed emissionProbs.
		*/
		if (!my notHidden)
		{
			gammasum += thy gamma[is][thy numberOfTimes]; // Now sum all, add last term
			for (k = 1; k <= my numberOfObservationSymbols; k++)
			{
				double gammasum_k = 0.0;
				for (it = 1; it <= thy numberOfTimes; it++)
				{
					if (obs[it] == k) gammasum_k += thy gamma[is][it];
				}
				// only reestimate probs > 0 !
				if (my emissionProbs[is][k] > 0)
				{
					thy bik_num[is][k] += gammasum_k;
					thy bik_denom[is][k] += gammasum;
				}
			}
		}
		// For a left-to-right model the final state determines the transition prob to go to the END state
		if (my leftToRight)
		{
			thy aij_num[is][my numberOfStates + 1] +=thy gamma[is][thy numberOfTimes];
			thy aij_denom[is][my numberOfStates + 1] += 1;
		}
	}
}

void HMM_and_HMM_BaumWelch_reestimate (HMM me, HMM_BaumWelch thee)
{
	double p;
	/*
		If we only have a couple of training sequences and they all happen to start with the same symbol,
		one or more starting probabilities can be zero.

		What to do with the P_k (see Rabiner formulas 109-110)?
	*/
	for (long is = 1; is <= my numberOfStates; is++)
	{
		/*
			If we have not enough observation sequences it can happen that some probabilities
			become zero. This means that for some future observation sequences the probability evaluation
			returns  p=0 for sequences where these transitions turn up. This makes recognition impossible and also comparisons between models are difficult.
			We can prevent this from happening by asumimg a minimal probability for valid transitions
			i.e. which have initially p > 0.
		*/
		if (my transitionProbs[0][is] > 0)
		{
			p = thy aij_num[0][is] / thy aij_denom[0][is];
			my transitionProbs[0][is] = p > 0 ? p : thy minProb;
		}
		for (long js = 1; js <= my numberOfStates; js++)
		{
			if (my transitionProbs[is][js] > 0)
			{
				p = thy aij_num[is][js] / thy aij_denom[is][js];
				my transitionProbs[is][js] = p > 0 ? p : thy minProb;
			}
		}
		if (!my notHidden)
		{
			for (long k = 1; k <= my numberOfObservationSymbols; k++)
			{
				if (my emissionProbs[is][k] > 0)
				{
					p = thy bik_num[is][k] / thy bik_denom[is][k];
					my emissionProbs[is][k] = p > 0 ? p : thy minProb;
				}
			}
		}
		if (my leftToRight && my transitionProbs[is][my numberOfStates + 1] > 0)
		{
			p = thy aij_num[is][my numberOfStates + 1] / thy aij_denom[is][my numberOfStates + 1];
			my transitionProbs[is][my numberOfStates + 1] = p > 0 ? p : thy minProb;
		}
	}
}

void HMM_and_HMM_BaumWelch_forward (HMM me, HMM_BaumWelch thee, long *obs)
{
	long is, js, it;

	// initialise at t = 1 & scale
	thy scale[1] = 0;
	for (js = 1; js <= my numberOfStates; js++)
	{
		thy alpha[js][1] = my transitionProbs[0][js] * my emissionProbs[js][ obs[1] ];
		thy scale[1] += thy alpha[js][1];
	}
	for (js = 1; js <= my numberOfStates; js++)
		thy alpha[js][1] /= thy scale[1];
	// recursion
	for (it = 2; it <= thy numberOfTimes; it++)
	{
		thy scale[it] = 0.0;
		for (js = 1; js <= my numberOfStates; js++)
		{
			double sum = 0.0;
			for (is = 1; is <= my numberOfStates; is++)
				sum += thy alpha[is][it - 1] * my transitionProbs[is][js];

			thy alpha[js][it] = sum * my emissionProbs[js][ obs[it] ];
			thy scale[it] += thy alpha[js][it];
		}

		for (js = 1; js <= my numberOfStates; js++)
			thy alpha[js][it] /= thy scale[it];
	}

	for (it = 1; it <= thy numberOfTimes; it++)
		thy lnProb += log (thy scale[it]);
}

void HMM_and_HMM_BaumWelch_backward (HMM me, HMM_BaumWelch thee, long *obs)
{
	long is, js, it;

	for (is = 1; is <= my numberOfStates; is++)
		thy beta[is][thy numberOfTimes] = 1.0 / thy scale[thy numberOfTimes];
    for (it = thy numberOfTimes - 1; it >= 1; it--)
	{
		for (is = 1; is <= my numberOfStates; is++)
		{
			double sum = 0.0;
			for (js = 1; js <= my numberOfStates; js++)
			{
				sum += thy beta[js][it + 1] * my transitionProbs[is][js] * my emissionProbs[js][ obs[it + 1] ];
			}
			thy beta[is][it] = sum / thy scale[it];
		}
	}
}

/*************************** HMM decoding ***********************************/

// precondition: valid symbols, i.e. 1 <= o[i] <= my numberOfSymbols for i=1..nt
void HMM_and_HMM_Viterbi_decode (HMM me, HMM_Viterbi thee, long *obs)
{
	long it, is, isp, ntimes = thy numberOfTimes;
	// initialisation
	for (is = 1; is <= my numberOfStates; is++)
	{
		thy viterbi[is][1] = my transitionProbs[0][is] * my emissionProbs[is][ obs[1] ];
		thy bp[is][1] = 0;
	}
	// recursion
	for (it = 2; it <= ntimes; it++)
	{
		for (is = 1; is <= my numberOfStates; is++)
		{
			// all transitions isp -> is from previous time to current
			double max_score = -1; // any negative number is ok
			for (isp = 1; isp <= my numberOfStates; isp++)
			{
				double score = thy viterbi[isp][it - 1] * my transitionProbs[isp][is]; // * my emissionProbs[is][ obs[it] ]
				if (score > max_score)
				{
					max_score = score;
					thy bp[is][it] = isp;
				}
			}
			thy viterbi[is][it] = max_score * my emissionProbs[is][ obs[it] ];
		}
	}
	// path starts at state with best end probability
	thy path[ntimes] = 1;
	thy prob = thy viterbi[1][ntimes];
	for (is = 2; is <= my numberOfStates; is++)
	{
		if (thy viterbi[is][ntimes] > thy prob)
		{
			thy prob = thy viterbi[ thy path[ntimes] = is ][ntimes];
		}
	}
	// trace back and get path
	for (it = ntimes; it > 1; it--)
	{
		thy path[it - 1] = thy bp[ thy path[it] ][it];
	}
}

HMM_StateSequence HMM_and_HMM_ObservationSequence_to_HMM_StateSequence (HMM me, HMM_ObservationSequence thee)
{
	HMM_Viterbi v = NULL;
	HMM_StateSequence him = NULL;

	StringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
	if (si == NULL) return NULL;
	long *obs = si -> classIndex; // convenience
	long numberOfUnknowns = StringsIndex_countItems (si, 0);

	if (numberOfUnknowns > 0)
	{
		Melder_error3 (L"Unknown observation symbol(s) (# = ", Melder_integer (numberOfUnknowns), L")."); goto end;
	}

	long numberOfTimes = thy rows -> size;
	if (((v = HMM_to_HMM_Viterbi (me, obs, numberOfTimes)) == NULL) ||
		((him = HMM_StateSequence_create (numberOfTimes)) == NULL)) goto end;
	// trace the path and get states
	for (long it = 1; it <= numberOfTimes; it++)
	{
		HMM_State hmms = my states -> item[ v -> path[it] ];
		his strings [it] = Melder_wcsdup (hmms -> label);
		his numberOfStrings ++;
	}
end:
	forget (v); forget (si);
	if (Melder_hasError ()) forget (him);
	return him;
}

double HMM_and_HMM_StateSequence_getProbability (HMM me, HMM_StateSequence thee)
{
	double lnp = NUMundefined;
	StringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
	long numberOfUnknowns = StringsIndex_countItems (si, 0);
	long *index = si -> classIndex;

	if (index == NULL) return lnp;
	if (numberOfUnknowns > 0)
	{
		Melder_warning3 (L"Unknown states (# = ", Melder_integer (numberOfUnknowns), L").");
		goto end;
	}
	double p0 = my transitionProbs [0][ index[1] ];
	if (p0 == 0) { Melder_error1 (L"You cannot start with this state."); goto end; }
	lnp = log(p0);
	for (long it = 2; it <= thy numberOfStrings; it++)
	{
		lnp += log (my transitionProbs[ index[it - 1] ] [ index[it] ]);
	}
end:
	forget (si);
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInState (HMM me, long itime, long istate)
{
	double *scale = NULL, *alpha_t = NULL, *alpha_tm1 = NULL, lnp = NUMundefined;
	long it, is, js;

	if (istate < 1 || istate > my numberOfStates) return lnp;

	if (((scale = NUMdvector (1, itime)) == NULL) ||
		((alpha_t = NUMdvector (1, my numberOfStates)) == NULL) ||
		((alpha_tm1 = NUMdvector (1, my numberOfStates)) == NULL)) goto end;

	for (js = 1; js <= my numberOfStates; js++)
	{
		alpha_t[js] = my transitionProbs[0][js];
		scale[1] += alpha_t[js];
	}
	for (js = 1; js <= my numberOfStates; js++)
		alpha_t[js] /= scale[1];
	// recursion
	for (it = 2; it <= itime; it++)
	{
		for (js = 1; js <= my numberOfStates; js++) alpha_tm1[js] = alpha_t[js];

		for (js = 1; js <= my numberOfStates; js++)
		{
			double sum = 0.0;
			for (is = 1; is <= my numberOfStates; is++)
				sum += alpha_tm1[is] * my transitionProbs[is][js];

			alpha_t[js] = sum;
			scale[it] += alpha_t[js];
		}

		for (js = 1; js <= my numberOfStates; js++)
			alpha_t[js] /= scale[it];
	}

	lnp = 0.0;
	for (it = 1; it <= itime; it++)
		lnp += log (scale[it]);

	lnp = alpha_t[istate] > 0 ? lnp + log (alpha_t[istate]) : -INFINITY; // p = 0 -> ln(p)=-infinity

end:
	NUMdvector_free (scale, 1);
	NUMdvector_free (alpha_t, 1);
	NUMdvector_free (alpha_tm1, 1);
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, long itime, long istate, long isymbol)
{
	// for a notHidden model emissionProbs may be zero!
	if (isymbol < 1 || isymbol > my numberOfObservationSymbols || my emissionProbs[istate][isymbol] == 0) return NUMundefined;
	double lnp = HMM_getProbabilityAtTimeBeingInState (me, itime, istate);
	return lnp != NUMundefined && lnp != -INFINITY ? lnp + log (my emissionProbs[istate][isymbol]) : lnp;
}

double HMM_getProbabilityOfObservations (HMM me, long *obs, long numberOfTimes)
{
	long it, is, js;
	double *scale = NULL, *alpha_t = NULL, *alpha_tm1 = NULL, lnp = NUMundefined;

	if (((scale = NUMdvector (1, numberOfTimes)) == NULL) ||
		((alpha_t = NUMdvector (1, my numberOfStates)) == NULL) ||
		((alpha_tm1 = NUMdvector (1, my numberOfStates)) == NULL)) goto end;

	// initialise
	for (js = 1; js <= my numberOfStates; js++)
	{
		alpha_t[js] = my transitionProbs[0][js] * my emissionProbs[js][ obs[1] ];
		scale[1] += alpha_t[js];
	}
	if (scale[1] == 0)
	{
		Melder_error1 (L"The observation sequence starts with a symbol which state has starting probability zero.");
		return -INFINITY;
	}
	for (js = 1; js <= my numberOfStates; js++) alpha_t[js] /= scale[1];

	// recursion
	for (it = 2; it <= numberOfTimes; it++)
	{
		for (js = 1; js <= my numberOfStates; js++) alpha_tm1[js] = alpha_t[js];

		for (js = 1; js <= my numberOfStates; js++)
		{
			double sum = 0.0;
			for (is = 1; is <= my numberOfStates; is++)
				sum += alpha_tm1[is] * my transitionProbs[is][js];

			alpha_t[js] = sum * my emissionProbs[js][ obs[it] ];
			scale[it] += alpha_t[js];
		}
		if (scale[it] <= 0) return -INFINITY;
		for (js = 1; js <= my numberOfStates; js++)
			alpha_t[js] /= scale[it];
	}

	lnp = 0;
	for (it = 1; it <= numberOfTimes; it++)
		lnp += log(scale[it]);
end:
	NUMdvector_free (scale, 1);
	NUMdvector_free (alpha_t, 1);
	NUMdvector_free (alpha_tm1, 1);
	return lnp;
}

double HMM_and_HMM_ObservationSequence_getProbability (HMM me, HMM_ObservationSequence thee)
{
	double lnp = NUMundefined;
	StringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
	if (si == NULL) return lnp;
	long *index = si -> classIndex;
	long numberOfUnknowns = StringsIndex_countItems (si, 0);
	if (numberOfUnknowns > 0)
	{
		Melder_warning3 (L"Unknown observations (# = ", Melder_integer (numberOfUnknowns), L").");
		goto end;
	}
	lnp = HMM_getProbabilityOfObservations (me, index, thy rows -> size);
end:
	forget (si);
	return lnp;
}

double HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM_ObservationSequence thee)
{
	double lnp = HMM_and_HMM_ObservationSequence_getProbability (me, thee);
	return lnp == NUMundefined ? NUMundefined : (lnp == -INFINITY ? INFINITY :
		-lnp / (NUMln10 * HMM_ObservationSequence_getNumberOfObservations (thee)));
}

double HMM_and_HMM_ObservationSequence_getPerplexity (HMM me, HMM_ObservationSequence thee)
{
	double ce = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, thee);
	return ce == NUMundefined ? NUMundefined : (ce == INFINITY ? INFINITY : pow (2, ce));
}

HMM HMM_createFromHMM_ObservationSequence (HMM_ObservationSequence me, long numberOfStates, int leftToRight)
{
	HMM thee = NULL;
	Strings s = HMM_ObservationSequence_to_Strings (me);
	if (s == NULL) return NULL;

	Distributions d = Strings_to_Distributions ((Strings) s);
	if (d == NULL) goto end;

	long numberOfObservationSymbols = d -> numberOfRows;

	thee = new (HMM);
	if (thee == NULL) goto end;
	thy notHidden = numberOfStates < 1;
	numberOfStates = numberOfStates > 0 ? numberOfStates : numberOfObservationSymbols;

	if (!HMM_init (thee, numberOfStates, numberOfObservationSymbols, leftToRight)) goto end;
	for (long i = 1; i <= numberOfObservationSymbols; i++)
	{
		wchar_t *label = d -> rowLabels[i];
		HMM_Observation hmmo = HMM_Observation_create (label, 0, 0, 0);
		if (hmmo == NULL || ! HMM_addObservation (thee, hmmo)) goto end;
		if (thy notHidden)
		{
			HMM_State hmms = HMM_State_create (label);
			if (hmms == NULL || ! HMM_addState (thee, hmms)) goto end;
		}
	}
	if (! thy notHidden) HMM_setDefaultStates (thee);
	HMM_setDefaultTransitionProbs (thee);
	HMM_setDefaultStartProbs (thee);
	HMM_setDefaultEmissionProbs (thee);
end:
	forget (d); forget (s);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

TableOfReal HMM_ObservationSequence_to_TableOfReal_transitions (HMM_ObservationSequence me, int probabilities)
{
	Strings thee = HMM_ObservationSequence_to_Strings (me);
	TableOfReal him = NULL;

	if (thee != NULL)
	{
		him = Strings_to_TableOfReal_transitions (thee, probabilities);
		forget (thee);
	}
	return him;
}

StringsIndex HMM_and_HMM_ObservationSequence_to_StringsIndex (HMM me, HMM_ObservationSequence thee)
{
	Strings classes = new (Strings);
	if (classes == NULL || ((classes -> strings = NUMpvector (1, my numberOfObservationSymbols)) == NULL)) goto end;
	for (long is = 1; is <= my numberOfObservationSymbols; is++)
	{
		HMM_Observation hmmo = my observationSymbols -> item[is];
		classes -> strings[is] = Melder_wcsdup (hmmo -> label);
		(classes -> numberOfStrings) ++;
	}
	Strings obs = HMM_ObservationSequence_to_Strings (thee);
	if (obs == NULL) goto end;
	StringsIndex him = Stringses_to_StringsIndex (obs, classes);
end:
	forget (obs); forget (classes);
	if (Melder_hasError ()) forget (him);
	return him;
}

StringsIndex HMM_and_HMM_StateSequence_to_StringsIndex (HMM me, HMM_StateSequence thee)
{
	Strings classes = new (Strings);
	if (classes == NULL || ((classes -> strings = NUMpvector (1, my numberOfObservationSymbols)) == NULL)) goto end;
	for (long is = 1; is <= my numberOfStates; is++)
	{
		HMM_State hmms = my states -> item[is];
		classes -> strings[is] = Melder_wcsdup (hmms -> label);
		(classes -> numberOfStrings) ++;
	}
	Strings sts = HMM_StateSequence_to_Strings (thee);
	if (sts == NULL) goto end;
	StringsIndex him = Stringses_to_StringsIndex (sts, classes);
end:
	forget (sts); forget (classes);
	if (Melder_hasError ()) forget (him);
	return him;
}


TableOfReal HMM_and_HMM_ObservationSequence_to_TableOfReal_transitions (HMM me, HMM_ObservationSequence thee, int probabilities)
{
	StringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
	if (si == NULL) return NULL;

	TableOfReal him = StringsIndex_to_TableOfReal_transitions (si, probabilities);
	forget (si);
	if (Melder_hasError ()) forget (him);
	return him;
}

TableOfReal HMM_and_HMM_StateSequence_to_TableOfReal_transitions (HMM me, HMM_StateSequence thee, int probabilities)
{
	StringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
	if (si == NULL) return NULL;

	TableOfReal him = StringsIndex_to_TableOfReal_transitions (si, probabilities);
	forget (si);
	if (Melder_hasError ()) forget (him);
	return him;
}

TableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities)
{
	TableOfReal thee = NULL;
	long i, j, numberOfTypes = my classes -> size;

	if ((thee = TableOfReal_create (numberOfTypes + 1, numberOfTypes + 1)) == NULL) goto end;
	for (i = 1; i <= numberOfTypes; i++)
	{
		SimpleString s = my classes -> item[i];
		TableOfReal_setRowLabel (thee, i, s -> string);
		TableOfReal_setColumnLabel (thee, i, s -> string);
	}
	for (i = 2; i <= my numberOfElements; i++)
	{
		if (my classIndex[i-1] > 0 && my classIndex[i] > 0) // a zero is a restart!
		{
			thy data [my classIndex[i-1]][my classIndex[i]] ++;
		}
	}
	double sum = 0;
	for (i = 1; i <= numberOfTypes; i++)
	{
		double rowSum = 0, colSum = 0;
		for (j = 1; j <= numberOfTypes; j++) { rowSum += thy data[i][j]; }
		thy data[i][numberOfTypes + 1] = rowSum;
		for (j = 1; j <= numberOfTypes; j++) { colSum += thy data[j][i]; }
		thy data[numberOfTypes + 1][i] = colSum;
		sum += colSum;
	}
	thy data[numberOfTypes + 1][numberOfTypes + 1] = sum;
	if (probabilities && sum > 0)
	{
		for (i = 1; i <= numberOfTypes; i++)
		{
			if (thy data[i][numberOfTypes + 1] > 0)
			{
				for (j = 1; j <= numberOfTypes; j++) { thy data[i][j] /= thy data[i][numberOfTypes + 1]; }
			}
		}
		for (i = 1; i <= numberOfTypes; i++)
		{
			thy data[i][numberOfTypes + 1] /= sum;
			thy data[numberOfTypes + 1][i] /= sum;
		}
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

TableOfReal Strings_to_TableOfReal_transitions (I, int probabilities)
{
	iam (Strings);

	StringsIndex him = Strings_to_StringsIndex (me);
	if (him == NULL) return NULL;

	TableOfReal thee = StringsIndex_to_TableOfReal_transitions (him, probabilities);
	forget (him);
	return thee;
}

/* End of file HMM.c */