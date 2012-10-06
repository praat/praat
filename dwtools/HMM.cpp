/* HMM.cpp
 *
 * Copyright (C) 2010-2012 David Weenink
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
 djmw 20110304 Thing_new
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
int NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b,
        double *x1, double *y1, double *x2, double *y2);
void HMM_Observation_init (I, const wchar_t *label, long numberOfComponents, long dimension, long storage);
HMM_Observation HMM_Observation_create (const wchar_t *label, long numberOfComponents, long dimension, long storage);

long HMM_and_HMM_ObservationSequence_getLongestSequence (HMM me, HMM_ObservationSequence thee, long symbolNumber);
long StringsIndex_getLongestSequence (StringsIndex me, long index, long *pos);
long Strings_getLongestSequence (Strings me, wchar_t *string, long *pos);
void HMM_State_init (I, const wchar_t *label);
HMM_State HMM_State_create (const wchar_t *label);

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
static double *NUMwstring_to_probs (wchar_t *s, long nwanted) {
	long numbers_found;
	autoNUMvector<double> numbers (NUMstring_to_numbers (s, &numbers_found), 1);
	if (numbers_found != nwanted) {
		Melder_throw ("You supplied ", numbers_found, ", while ", nwanted, " numbers needed.");
	}
	double sum = 0;
	for (long i = 1; i <= numbers_found; i++) {
		if (numbers[i] < 0) {
			Melder_throw ("Numbers have to be positive.");
		}
		sum += numbers[i];
	}
	if (sum <= 0) {
		Melder_throw ("All probabilities cannot be zero.");
	}
	for (long i = 1; i <= numbers_found; i++) {
		numbers[i] /= sum;
	}
	return numbers.transfer();
}

int NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b,
        double *x1, double *y1, double *x2, double *y2) {
	double ca = a * a + 1, bmyc = (b - yc);
	double cb = 2 * (a * bmyc - xc);
	double cc = bmyc * bmyc + xc * xc - r * r;
	long nroots = NUMsolveQuadraticEquation (ca, cb, cc, x1, x2);
	if (nroots == 1) {
		*y1 = a * *x1 + b;
		*x2 = *x1;
		*y2 = *y1;
	} else if (nroots == 2) {
		if (*x1 > *x2) {
			double tmp = *x1;
			*x1 = *x2;
			*x2 = tmp;
		}
		*y1 = *x1 * a + b; *y2 = *x2 * a + b;
	}
	return nroots;
}

// D(l_1,l_2)=1/n( log p(O_2|l_1) - log p(O_2|l_2)
static double HMM_and_HMM_getCrossEntropy_asym (HMM me, HMM thee, long observationLength) {
	autoHMM_ObservationSequence os = HMM_to_HMM_ObservationSequence (thee, 0, observationLength);
	double ce = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, os.peek());
	if (ce == NUMundefined || ce == INFINITY) {
		return ce;
	}
	double ce2 = HMM_and_HMM_ObservationSequence_getCrossEntropy (thee, os.peek());
	if (ce2 == NUMundefined || ce2 == INFINITY) {
		return ce2;
	}
	return ce - ce2;
}

/**************** HMM_Observation ******************************/

Thing_implement (HMM_Observation, Data, 0);

void HMM_Observation_init (I, const wchar_t *label, long numberOfComponents, long dimension, long storage) {
	iam (HMM_Observation);
	my label = Melder_wcsdup (label);
	my gm = GaussianMixture_create (numberOfComponents, dimension, storage);
}

HMM_Observation HMM_Observation_create (const wchar_t *label, long numberOfComponents, long dimension, long storage) {
	try {
		autoHMM_Observation me = Thing_new (HMM_Observation);
		HMM_Observation_init (me.peek(), label, numberOfComponents, dimension, storage);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_Observation not created.");
	}
}

long Strings_getLongestSequence (Strings me, wchar_t *string, long *pos) {
	long length = 0, longest = 0, lpos = 0;
	for (long i = 1; i <= my numberOfStrings; i++) {
		if (Melder_wcsequ (my strings[i], string)) {
			if (length == 0) {
				lpos = i;
			}
			length++;
		} else {
			if (length > 0) {
				if (length > longest) {
					longest = length; *pos = lpos;
				}
				length = 0;
			}
		}
	}
	return length;
}

long StringsIndex_getLongestSequence (StringsIndex me, long index, long *pos) {
	long length = 0, longest = 0, lpos = 0;
	for (long i = 1; i <= my numberOfElements; i++) {
		if (my classIndex[i] == index) {
			if (length == 0) {
				lpos = i;
			}
			length++;
		} else {
			if (length > 0) {
				if (length > longest) {
					longest = length; *pos = lpos;
				}
				length = 0;
			}
		}
	}
	return length;
}

/**************** HMM_State ******************************/

Thing_implement (HMM_State, Data, 0);

void HMM_State_init (I, const wchar_t *label) {
	iam (HMM_State);
	my label = Melder_wcsdup (label);
}

HMM_State HMM_State_create (const wchar_t *label) {
	try {
		autoHMM_State me = Thing_new (HMM_State);
		HMM_State_init (me.peek(), label);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_State not created.");
	}
}

void HMM_State_setLabel (HMM_State me, wchar_t *label) {
	Melder_free (my label);
	my label = Melder_wcsdup (label);
}

/**************** HMM_BaumWelch ******************************/

Thing_implement (HMM_BaumWelch, Data, 0);

void structHMM_BaumWelch :: v_destroy () {
	for (long it = 1; it <= numberOfTimes; it++) {
		NUMmatrix_free (xi[it], 1, 1);
	}
	NUMvector_free (xi, 1);
	NUMvector_free (scale, 1);
	NUMmatrix_free (beta, 1, 1);
	NUMmatrix_free (alpha, 1, 1);
	NUMmatrix_free (gamma, 1, 1);
}

HMM_BaumWelch HMM_BaumWelch_create (long nstates, long nsymbols, long capacity) {
	try {
		autoHMM_BaumWelch me = Thing_new (HMM_BaumWelch);
		my numberOfTimes = my capacity = capacity;
		my numberOfStates = nstates;
		my numberOfSymbols = nsymbols;
		my alpha = NUMmatrix<double> (1, nstates, 1, capacity);
		my beta = NUMmatrix<double> (1, nstates, 1, capacity);
		my scale = NUMvector<double> (1, capacity);
		my xi = NUMvector<double **> (1, capacity);
		my aij_num = NUMmatrix<double> (0, nstates, 1, nstates + 1);
		my aij_denom = NUMmatrix<double> (0, nstates, 1, nstates + 1);
		my bik_num = NUMmatrix<double> (1, nstates, 1, nsymbols);
		my bik_denom = NUMmatrix<double> (1, nstates, 1, nsymbols);
		my gamma = NUMmatrix<double> (1, nstates, 1, capacity);
		for (long it = 1; it <= capacity; it++) {
			my xi[it] = NUMmatrix<double> (1, nstates, 1, nstates);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_BaumWelch not created.");
	}
}

void HMM_BaumWelch_getGamma (HMM_BaumWelch me) {
	for (long it = 1; it <= my numberOfTimes; it++) {
		double sum = 0.0;
		for (long is = 1; is <= my numberOfStates; is++) {
			my gamma[is][it] = my alpha[is][it] * my beta[is][it];
			sum += my gamma[is][it];
		}

		for (long is = 1; is <= my numberOfStates; is++) {
			my gamma[is][it] /= sum;
		}
	}
}

/**************** HMM_Viterbi ******************************/

Thing_implement (HMM_Viterbi, Data, 0);

HMM_Viterbi HMM_Viterbi_create (long nstates, long ntimes) {
	try {
		autoHMM_Viterbi me = Thing_new (HMM_Viterbi);
		my numberOfTimes = ntimes;
		my numberOfStates = nstates;
		my viterbi = NUMmatrix<double> (1, nstates, 1 , ntimes);
		my bp = NUMmatrix<long> (1, nstates, 1 , ntimes);
		my path = NUMvector<long> (1, ntimes);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_Viterbi not created.");
	}
}

/******************* HMM_ObservationSequence & HMM_StateSequence ***/

Thing_implement (HMM_ObservationSequence, Table, 0);

HMM_ObservationSequence HMM_ObservationSequence_create (long numberOfItems, long dataLength) {
	try {
		autoHMM_ObservationSequence me = Thing_new (HMM_ObservationSequence);
		Table_initWithoutColumnNames (me.peek(), numberOfItems, dataLength + 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_ObservationSequence not created.");
	}
}

long HMM_ObservationSequence_getNumberOfObservations (HMM_ObservationSequence me) {
	return my rows -> size;
}

void HMM_ObservationSequence_removeObservation (HMM_ObservationSequence me, long index) {
	Table_removeRow ( (Table) me, index);
}

Strings HMM_ObservationSequence_to_Strings (HMM_ObservationSequence me) {
	try {
		long numberOfStrings = my rows -> size;
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<wchar_t *> (1, numberOfStrings);
		for (long i = 1; i <= numberOfStrings; i++) {
			thy strings[i] = Melder_wcsdup_f (Table_getStringValue_Assert ( (Table) me, i, 1));
			(thy numberOfStrings) ++;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Strings created.");
	}
}

HMM_ObservationSequence Strings_to_HMM_ObservationSequence (Strings me) {
	try {
		autoHMM_ObservationSequence thee = HMM_ObservationSequence_create (my numberOfStrings, 0);
		for (long i = 1; i <= my numberOfStrings; i++) {
			Table_setStringValue ( (Table) thee.peek(), i, 1, my strings[i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no HMM_ObservationSequence created.");
	}
}

StringsIndex HMM_ObservationSequence_to_StringsIndex (HMM_ObservationSequence me) {
	try {
		autoStrings s = HMM_ObservationSequence_to_Strings (me);
		autoStringsIndex thee = Strings_to_StringsIndex (s.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no StringsIndex created.");
	}
}

long HMM_and_HMM_ObservationSequence_getLongestSequence (HMM me, HMM_ObservationSequence thee, long symbolNumber) {
	autoStringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
	// TODO
	(void) symbolNumber;
	return 1;
}

Thing_implement (HMM_ObservationSequences, Collection, 0);

HMM_ObservationSequences HMM_ObservationSequences_create () {
	try {
		autoHMM_ObservationSequences me = Thing_new (HMM_ObservationSequences);
		Collection_init (me.peek(), classHMM_ObservationSequence, 1000);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_ObservationSequences not created.");
	}
}

long HMM_ObservationSequences_getLongestSequence (HMM_ObservationSequences me) {
	long longest = 0;
	for (long i = 1; i <= my size; i++) {
		HMM_ObservationSequence thee = (HMM_ObservationSequence) my item[i];
		if (thy rows -> size > longest) {
			longest = thy rows -> size;
		}
	}
	return longest;
}

Thing_implement (HMM_StateSequence, Strings, 0);

HMM_StateSequence HMM_StateSequence_create (long numberOfItems) {
	try {
		autoHMM_StateSequence me = Thing_new (HMM_StateSequence);
		my strings = NUMvector<wchar_t *> (1, numberOfItems);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM_StateSequence not created.");
	}
}

Strings HMM_StateSequence_to_Strings (HMM_StateSequence me) {
	try {
		autoStrings thee = Thing_new (Strings);
		my structStrings :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Strings created.");
	}
}


/**************** HMM ******************************/

Thing_implement (HMM, Data, 0);

void structHMM :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of states: ", Melder_integer (numberOfStates));
	for (long i = 1; i <= numberOfStates; i++) {
		HMM_State hmms = (HMM_State) states -> item[i];
		MelderInfo_writeLine (L"  ", hmms -> label);
	}
	MelderInfo_writeLine (L"Number of symbols: ", Melder_integer (numberOfObservationSymbols));
	for (long i = 1; i <= numberOfObservationSymbols; i++) {
		HMM_Observation hmms = (HMM_Observation) observationSymbols -> item[i];
		MelderInfo_writeLine (L"  ", hmms -> label);
	}
}

static void HMM_init (HMM me, long numberOfStates, long numberOfObservationSymbols, int leftToRight) {
	my numberOfStates = numberOfStates;
	my numberOfObservationSymbols = numberOfObservationSymbols;
	my componentStorage = 1;
	my leftToRight = leftToRight;
	my states = Ordered_create ();
	my observationSymbols = Ordered_create ();
	my transitionProbs = NUMmatrix<double> (0, numberOfStates, 1, numberOfStates + 1);
	my emissionProbs = NUMmatrix<double> (1, numberOfStates, 1, numberOfObservationSymbols);
}

HMM HMM_create (int leftToRight, long numberOfStates, long numberOfObservationSymbols) {
	try {
		autoHMM me = Thing_new (HMM);
		HMM_init (me.peek(), numberOfStates, numberOfObservationSymbols, leftToRight);
		HMM_setDefaultStates (me.peek());
		HMM_setDefaultObservations (me.peek());
		HMM_setDefaultTransitionProbs (me.peek());
		HMM_setDefaultStartProbs (me.peek());
		HMM_setDefaultEmissionProbs (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("HMM not created.");
	}
}

void HMM_setDefaultStates (HMM me) {
	autoMelderString label;
	for (long i = 1; i <= my numberOfStates; i++) {
		MelderString_append (&label, L"S", Melder_integer (i));
		autoHMM_State hmms = HMM_State_create (label.string);
		HMM_addState (me, hmms.transfer());
		MelderString_empty (&label);
	}
}

HMM HMM_createFullContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
                                   long numberOfFeatureStreams, long *dimensionOfStream, long *numberOfGaussiansforStream) {
	(void) leftToRight;
	(void) numberOfStates;
	(void) numberOfObservationSymbols;
	(void) numberOfFeatureStreams;
	(void) dimensionOfStream;
	(void) numberOfGaussiansforStream;
	return NULL;
}

HMM HMM_createContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols,
                               long numberOfMixtureComponentsPerSymbol, long componentDimension, long componentStorage) {
	try {
		MelderString label = { 0 };
		autoHMM me = Thing_new (HMM);
		HMM_init (me.peek(), numberOfStates, numberOfObservationSymbols, leftToRight);
		my numberOfMixtureComponents = numberOfMixtureComponentsPerSymbol;
		my componentDimension = componentDimension;
		my componentStorage = componentStorage;
		for (long i = 1; i <= numberOfStates; i++) {
			MelderString_append (&label, L"S", Melder_integer (i));
			autoHMM_State state = HMM_State_create (label.string);
			HMM_addState (me.peek(), state.transfer());
			MelderString_empty (&label);
		}
		for (long j = 1; j <= numberOfObservationSymbols; j++) {
			MelderString_append (&label, L"s", Melder_integer (j));
			autoHMM_Observation obs = HMM_Observation_create (label.string, numberOfMixtureComponentsPerSymbol, componentDimension, componentStorage);
			HMM_addObservation (me.peek(), obs.transfer());
			MelderString_empty (&label);
		}
		HMM_setDefaultTransitionProbs (me.peek());
		HMM_setDefaultStartProbs (me.peek());
		HMM_setDefaultEmissionProbs (me.peek());
		HMM_setDefaultMixingProbabilities (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Continuous model HMM not created.");
	}
}

// for a simple non-hidden model leave either states empty or symbols empty !!!
HMM HMM_createSimple (int leftToRight, const wchar_t *states_string, const wchar_t *symbols_string) {
	try {
		autoHMM me = Thing_new (HMM);
		const wchar_t *states = states_string;
		const wchar_t *symbols = symbols_string;
		long numberOfStates = Melder_countTokens (states_string);
		long numberOfObservationSymbols = Melder_countTokens (symbols_string);

		if (numberOfStates == 0 and numberOfObservationSymbols == 0) {
			Melder_throw ("No states and symbols.");
		}
		if (numberOfStates > 0) {
			if (numberOfObservationSymbols <= 0) {
				numberOfObservationSymbols = numberOfStates;
				symbols = states_string;
				my notHidden = 1;
			}
		} else if (numberOfObservationSymbols > 0) {
			numberOfStates = numberOfObservationSymbols;
			states = symbols_string;
			my notHidden = 1;
		}

		HMM_init (me.peek(), numberOfStates, numberOfObservationSymbols, leftToRight);

		for (wchar_t *token = Melder_firstToken (states); token != 0; token = Melder_nextToken ()) {
			autoHMM_State state = HMM_State_create (token);
			HMM_addState (me.peek(), state.transfer());
		}
		for (wchar_t *token = Melder_firstToken (symbols); token != NULL; token = Melder_nextToken ()) {
			autoHMM_Observation symbol = HMM_Observation_create (token, 0, 0, 0);
			HMM_addObservation (me.peek(), symbol.transfer());
		}
		HMM_setDefaultTransitionProbs (me.peek());
		HMM_setDefaultStartProbs (me.peek());
		HMM_setDefaultEmissionProbs (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Simple HMM not created.");
	}
}

void HMM_setDefaultObservations (HMM me) {
	autoMelderString symbol;
	const wchar_t *def = my notHidden ? L"S" : L"s";
	for (long i = 1; i <= my numberOfObservationSymbols; i++) {
		MelderString_append (&symbol, def, Melder_integer (i));
		autoHMM_Observation hmms = HMM_Observation_create (symbol.string, 0, 0, 0);
		HMM_addObservation (me, hmms.transfer());
		MelderString_empty (&symbol);
	}
}

void HMM_setDefaultTransitionProbs (HMM me) {
	for (long i = 1; i <= my numberOfStates; i++) {
		double p = my leftToRight ? 1.0 / (my numberOfStates - i + 1) : 1.0 / my numberOfStates;
		for (long j = 1; j <= my numberOfStates; j++) {
			my transitionProbs[i][j] = my leftToRight && j < i ? 0 : p;
		}
	}
	// leftToRight must have end state!
	if (my leftToRight) my transitionProbs[my numberOfStates][my numberOfStates] =
		    my transitionProbs[my numberOfStates][my numberOfStates + 1] = 0.5;
}

void HMM_setDefaultStartProbs (HMM me) {
	double p = 1.0 / my numberOfStates;
	for (long j = 1; j <= my numberOfStates; j++) {
		my transitionProbs[0][j] = p;
	}
}

void HMM_setDefaultEmissionProbs (HMM me) {
	double p = 1.0 / my numberOfObservationSymbols;
	for (long i = 1; i <= my numberOfStates; i++)
		for (long j = 1; j <= my numberOfObservationSymbols; j++) {
			my emissionProbs[i][j] = my notHidden ? (i == j ? 1 : 0) : p;
		}
}

void HMM_setDefaultMixingProbabilities (HMM me) {
	double mp = 1.0 / my numberOfMixtureComponents;
	for (long is = 1; is <= my numberOfObservationSymbols; is++) {
		HMM_Observation hmmo = (HMM_Observation) my observationSymbols -> item[is];
		for (long im = 1; im <= my numberOfMixtureComponents; im++) {
			hmmo -> gm -> mixingProbabilities[im] = mp;
		}
	}
}

void HMM_setStartProbabilities (HMM me, wchar_t *probs) {
	try {
		autoNUMvector<double> p (NUMwstring_to_probs (probs, my numberOfStates), 1);
		for (long i = 1; i <= my numberOfStates; i++) {
			my transitionProbs[0][i] = p[i];
		}
	} catch (MelderError) {
		Melder_throw (me, ": no start probabilities set.");
	}
}

void HMM_setTransitionProbabilities (HMM me, long state_number, wchar_t *state_probs) {
	try {
		if (state_number > my states -> size) {
			Melder_throw (L"State number too large.");
		}
		autoNUMvector<double> p (NUMwstring_to_probs (state_probs, my numberOfStates + 1), 1);
		for (long i = 1; i <= my numberOfStates + 1; i++) {
			my transitionProbs[state_number][i] = p[i];
		}
	} catch (MelderError) {
		Melder_throw (me, ": no transition probabilities set.");
	}
}

void HMM_setEmissionProbabilities (HMM me, long state_number, wchar_t *emission_probs) {
	try {
		if (state_number > my states -> size) {
			Melder_throw (L"State number too large.");
		}
		if (my notHidden) {
			Melder_throw (L"The emission probs of this model are fixed.");
		}
		autoNUMvector<double> p (NUMwstring_to_probs (emission_probs, my numberOfObservationSymbols), 1);
		for (long i = 1; i <= my numberOfObservationSymbols; i++) {
			my emissionProbs[state_number][i] = p[i];
		}
	} catch (MelderError) {
		Melder_throw (me, ": no emission probabilities set.");
	}

}

void HMM_addObservation (HMM me, thou) {
	thouart (HMM_Observation);
	long ns = my observationSymbols -> size + 1;
	if (ns > my numberOfObservationSymbols) {
		Melder_throw ("Observation list is full.");
	}
	Ordered_addItemPos (my observationSymbols, thee, ns);
}

void HMM_addState (HMM me, thou) {
	thouart (HMM_State);
	long ns = my states -> size + 1;
	if (ns > my numberOfStates) {
		Melder_throw ("States list is full.");
	}
	Ordered_addItemPos (my states, thee, ns);
}

TableOfReal HMM_extractTransitionProbabilities (HMM me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfStates + 1, my numberOfStates + 1);
		for (long is = 1; is <= my numberOfStates; is++) {
			HMM_State hmms = (HMM_State) my states -> item[is];
			TableOfReal_setRowLabel (thee.peek(), is + 1, hmms -> label);
			TableOfReal_setColumnLabel (thee.peek(), is, hmms -> label);
			for (long js = 1; js <= my numberOfStates; js++) {
				thy data[is + 1][js] = my transitionProbs[is][js];
			}
		}
		TableOfReal_setRowLabel (thee.peek(), 1, L"START");
		TableOfReal_setColumnLabel (thee.peek(), my numberOfStates + 1, L"END");
		for (long is = 1; is <= my numberOfStates; is++) {
			thy data[1][is] = my transitionProbs[0][is];
			thy data[is + 1][my numberOfStates + 1] = my transitionProbs[is][my numberOfStates + 1];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transition probabilities extracted.");
	}
}

TableOfReal HMM_extractEmissionProbabilities (HMM me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfStates, my numberOfObservationSymbols);
		for (long js = 1; js <= my numberOfObservationSymbols; js++) {
			HMM_Observation hmms = (HMM_Observation) my observationSymbols -> item[js];
			TableOfReal_setColumnLabel (thee.peek(), js, hmms -> label);
		}
		for (long is = 1; is <= my numberOfStates; is++) {
			HMM_State hmms = (HMM_State) my states -> item[is];
			TableOfReal_setRowLabel (thee.peek(), is, hmms -> label);
			for (long js = 1; js <= my numberOfObservationSymbols; js++) {
				thy data[is][js] = my emissionProbs[is][js];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no emission probabilities extracted.");
	};
}

double HMM_getExpectedValueOfDurationInState (HMM me, long istate) {
	if (istate < 0 || istate > my numberOfStates) {
		return NUMundefined;
	}
	return 1 / (1 - my transitionProbs[istate][istate]);
}

double HMM_getProbabilityOfStayingInState (HMM me, long istate, long numberOfTimeUnits) {
	if (istate < 0 || istate > my numberOfStates) {
		return NUMundefined;
	}
	return pow (my transitionProbs[istate][istate], numberOfTimeUnits - 1) * (1 - my transitionProbs[istate][istate]);
}

double HMM_and_HMM_getCrossEntropy (HMM me, HMM thee, long observationLength, int symmetric) {
	double ce1 = HMM_and_HMM_getCrossEntropy_asym (me, thee, observationLength);
	if (! symmetric || ce1 == NUMundefined || ce1 == INFINITY) {
		return ce1;
	}
	double ce2 = HMM_and_HMM_getCrossEntropy_asym (thee, me, observationLength);
	if (ce2 == NUMundefined || ce2 == INFINITY) {
		return ce2;
	}
	return (ce1 + ce2) / 2;
}

double HMM_and_HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM thee, HMM_ObservationSequence him) {
	double ce1 = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, him);
	if (ce1 == NUMundefined || ce1 == INFINITY) {
		return ce1;
	}
	double ce2 = HMM_and_HMM_ObservationSequence_getCrossEntropy (thee, him);
	if (ce2 == NUMundefined || ce2 == INFINITY) {
		return ce2;
	}
	return (ce1 + ce2) / 2;
}

void HMM_draw (HMM me, Graphics g, int garnish) {
	double xwidth = sqrt (my numberOfStates);
	double rstate = 0.3 / xwidth, r = xwidth / 3.0;
	double xmax = 1.2 * xwidth / 2, xmin = -xmax, ymin = xmin, ymax = xmax;

	autoNUMvector<double> xs (1, my numberOfStates);
	autoNUMvector<double> ys (1, my numberOfStates);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	// heuristic: all states on a circle until we have a better graph drawing algorithm.
	xs[1] = ys[1] = 0;
	if (my numberOfStates > 1) {
		for (long is = 1; is <= my numberOfStates; is++) {
			double alpha = - NUMpi + NUMpi * 2 * (is - 1) / my numberOfStates;
			xs[is] = r * cos (alpha); ys[is] = r * sin (alpha);
		}
	}
	// reorder the positions such that state number 1 is most left and last state number is right.
	// if > 5 may be one in the middle with the most connections
	// ...
	// find fontsize
	int fontSize = Graphics_inqFontSize (g);
	const wchar_t *widest_label = L"";
	double max_width = 0;
	for (long is = 1; is <= my numberOfStates; is++) {
		HMM_State hmms = (HMM_State) my states -> item[is];
		double w = hmms -> label == NULL ? 0 : Graphics_textWidth (g, hmms -> label);
		if (w > max_width) {
			widest_label = hmms -> label;
			max_width = w;
		}
	}
	int new_fontSize = fontSize;
	while (max_width > 2 * rstate && new_fontSize > 4) {
		new_fontSize --;
		Graphics_setFontSize (g, new_fontSize);
		max_width = Graphics_textWidth (g, widest_label);
	}
	Graphics_setFontSize (g, new_fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (long is = 1; is <= my numberOfStates; is++) {
		HMM_State hmms = (HMM_State) my states -> item[is];
		Graphics_circle (g, xs[is], ys[is], rstate);
		Graphics_text (g, xs[is], ys[is], hmms -> label);
	}

	// draw connections from is to js
	// 1 -> 2 / 2-> : increase / decrease angle between 1 and 2 with pi /10
	// use cos(a+b) and cos(a -b) rules
	double cosb = cos (NUMpi / 10), sinb = sin (NUMpi / 10);
	for (long is = 1; is <= my numberOfStates; is++) {
		double x1 = xs[is], y1 = ys[is];
		for (long js = 1; js <= my numberOfStates; js++) {
			if (my transitionProbs[is][js] > 0 && is != js) {
				double x2 = xs[js], y2 = ys[js];
				double dx = x2 - x1, dy = y2 - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				double cosabp = cosa * cosb - sina * sinb, cosabm = cosa * cosb + sina * sinb;
				double sinabp = cosa * sinb + sina * cosb, sinabm = -cosa * sinb + sina * cosb;
				Graphics_arrow (g, x1 + rstate * cosabp, y1 + rstate * sinabp, x2 - rstate * cosabm, y2 - rstate * sinabm);
			}
			if (is == js) {
				double dx = - x1, dy = - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				Graphics_doubleArrow (g, x1 - rstate * cosa, y1 - rstate * sina, x1 - 1.4 * rstate * cosa, y1 - 1.4 * rstate * sina);
			}
		}
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
	}
}

void HMM_unExpandPCA (HMM me) {
	if (my componentDimension <= 0) {
		return;    // nothing to do
	}
	for (long is = 1; is <= my numberOfObservationSymbols; is++) {
		HMM_Observation s = (HMM_Observation) my observationSymbols -> item[is];
		GaussianMixture_unExpandPCA (s -> gm);
	}
}

HMM_ObservationSequence HMM_to_HMM_ObservationSequence (HMM me, long startState, long numberOfItems) {
	try {
		autoHMM_ObservationSequence thee = HMM_ObservationSequence_create (numberOfItems, my componentDimension);
		autoNUMvector<double> obs;
		autoNUMvector<double> buf;
		if (my componentDimension > 0) {
			obs.reset (1, my componentDimension);
			buf.reset (1, my componentDimension);
		}
		long istate = startState == 0 ? NUMgetIndexFromProbability (my transitionProbs[0], my numberOfStates, NUMrandomUniform (0, 1)) : startState;
		for (long i = 1; i <= numberOfItems; i++) {
			// Emit a symbol from istate

			long isymbol = NUMgetIndexFromProbability (my emissionProbs[istate], my numberOfObservationSymbols, NUMrandomUniform (0, 1));
			HMM_Observation s = (HMM_Observation) my observationSymbols -> item[isymbol];

			if (my componentDimension > 0) {
				wchar_t *name;
				GaussianMixture_generateOneVector (s -> gm, obs.peek(), &name, buf.peek());
				for (long j = 1; j <= my componentDimension; j++) {
					Table_setNumericValue ( (Table) thee.peek(), i, 1 + j, obs[j]);
				}
			}

			Table_setStringValue ( (Table) thee.peek(), i, 1, s -> label);

			// get next state

			istate = NUMgetIndexFromProbability (my transitionProbs[istate], my numberOfStates + 1, NUMrandomUniform (0, 1));
			if (istate == my numberOfStates + 1) { // final state
				for (long j = numberOfItems; j > i; j--) {
					HMM_ObservationSequence_removeObservation (thee.peek(), j);
				}
				break;
			}
		}
		HMM_unExpandPCA (me);
		return thee.transfer();
	} catch (MelderError) {
		HMM_unExpandPCA (me);
		Melder_throw (me, ":no HMM_ObservationSequence created.");
	}
}

HMM_BaumWelch HMM_forward (HMM me, long *obs, long nt) {
	try {
		autoHMM_BaumWelch thee = HMM_BaumWelch_create (my numberOfStates, my numberOfObservationSymbols, nt);
		HMM_and_HMM_BaumWelch_forward (me, thee.peek(), obs);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no HMM_BaumWelch created.");
	}
}

HMM_Viterbi HMM_to_HMM_Viterbi (HMM me, long *obs, long ntimes) {
	try {
		autoHMM_Viterbi thee = HMM_Viterbi_create (my numberOfStates, ntimes);
		HMM_and_HMM_Viterbi_decode (me, thee.peek(), obs);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no HMM_Viterbi created.");
	}
}

void HMM_BaumWelch_reInit (HMM_BaumWelch me) {
	my totalNumberOfSequences = 0;
	my lnProb = 0;

	/*
		The _num and _denum matrices are asigned as += in the iteration loop and therefore need to be zeroed
		at the start of each new iteration.
		The elements of alpha, beta, scale, gamma & xi are always calculated directly and need not be
		initialised.
	*/
	for (long is = 0; is <= my numberOfStates; is++) {
		for (long js = 1; js <= my numberOfStates + 1; js++) {
			my aij_num[is][js] = 0;
			my aij_denom[is][js] = 0;
		}
	}
	for (long is = 1; is <= my numberOfStates; is++) {
		for (long js = 1; js <= my numberOfSymbols; js++) {
			my bik_num[is][js] = 0;
			my bik_denom[is][js] = 0;
		}
	}
}

void HMM_and_HMM_ObservationSequences_learn (HMM me, HMM_ObservationSequences thee, double delta_lnp, double minProb) {
	try {
		// act as if all observation sequences are in memory
		long capacity = HMM_ObservationSequences_getLongestSequence (thee);
		autoHMM_BaumWelch bw = HMM_BaumWelch_create (my numberOfStates, my numberOfObservationSymbols, capacity);
		bw -> minProb = minProb;
		MelderInfo_open ();
		long iter = 0; double lnp;
		do {
			lnp = bw -> lnProb;
			HMM_BaumWelch_reInit (bw.peek());
			for (long ios = 1; ios <= thy size; ios++) {
				MelderInfo_writeLine (L"Observation sequence: ", Melder_integer (ios));
				HMM_ObservationSequence hmm_os = (HMM_ObservationSequence) thy item[ios];
				autoStringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, hmm_os);
				long *obs = si -> classIndex, nobs = si -> numberOfElements; // convenience

				// Interpretation of unknowns: end of sequence

				long istart = 1, iend = nobs;
				while (istart <= nobs) {
					while (istart <= nobs && obs[istart] == 0) {
						istart++;
					};
					if (istart > nobs) {
						break;
					}
					iend = istart + 1;
					while (iend <= nobs && obs[iend] != 0) {
						iend++;
					}
					iend --;
					bw -> numberOfTimes = iend - istart + 1;
					(bw -> totalNumberOfSequences) ++;
					MelderInfo_writeLine (L"  sub observation: ", Melder_integer (bw -> totalNumberOfSequences));
					HMM_and_HMM_BaumWelch_forward (me, bw.peek(), obs + istart - 1); // get new alphas
					HMM_and_HMM_BaumWelch_backward (me, bw.peek(), obs + istart - 1); // get new betas
					HMM_BaumWelch_getGamma (bw.peek());
					HMM_and_HMM_BaumWelch_getXi (me, bw.peek(), obs + istart - 1);
					HMM_and_HMM_BaumWelch_addEstimate (me, bw.peek(), obs + istart - 1);
					istart = iend + 1;
				}
			}
			// we have processed all observation sequences, now it time to estimate new probabilities.
			iter++;
			HMM_and_HMM_BaumWelch_reestimate (me, bw.peek());
			MelderInfo_writeLine (L"Iteration: ", Melder_integer (iter), L" ln(prob): ", Melder_double (bw -> lnProb));
		} while (fabs ( (lnp - bw -> lnProb) / bw -> lnProb) > delta_lnp);

		MelderInfo_writeLine (L"******** Learning summary *********");
		MelderInfo_writeLine (L"  Processed ", Melder_integer (thy size), L" sequences,");
		MelderInfo_writeLine (L"  consisting of ", Melder_integer (bw -> totalNumberOfSequences), L" observation sequences.");
		MelderInfo_writeLine (L"  Longest observation sequence had ", Melder_integer (capacity), L" items");
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": not learned.");
	}
}


// xc1 < xc2
void HMM_and_HMM_StateSequence_drawTrellis (HMM me, HMM_StateSequence thee, Graphics g, int connect, int garnish) {
	long numberOfTimes = thy numberOfStrings;
	autoStringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
	double xmin = 0, xmax = numberOfTimes + 1, ymin = 0.5, ymax = my numberOfStates + 0.5;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double r = 0.2 / (numberOfTimes > my numberOfStates ? numberOfTimes : my numberOfStates);

	for (long it = 1; it <= numberOfTimes; it++) {
		for (long js = 1; js <= my numberOfStates; js++) {
			double xc = it, yc = js, x2 = it, y2 = js;
			Graphics_circle (g, xc, yc, r);
			if (it > 1) {
				for (long is = 1; is <= my numberOfStates; is++) {
					bool indexedConnection = si -> classIndex[it - 1] == is && si -> classIndex[it] == js;
					Graphics_setLineWidth (g, indexedConnection ? 2 : 1);
					Graphics_setLineType (g, indexedConnection ? Graphics_DRAWN : Graphics_DOTTED);
					double x1 = it - 1, y1 = is;
					if (connect || indexedConnection) {
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
	if (garnish) {
		Graphics_drawInnerBox (g);
		for (long js = 1; js <= my numberOfStates; js++) {
			HMM_State hmms = (HMM_State) my states -> item[js];
			Graphics_markLeft (g, js, 0, 0, 0, hmms -> label);
		}
		Graphics_marksBottomEvery (g, 1, 1, 1, 1, 0);
		Graphics_textBottom (g, 1, L"Time index");
	}
}

void HMM_drawBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xmin = 0, xmax = 1, ymin = 0, ymax = 1;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double xleft  = 0.1, xright = 0.9, r = 0.03;
	long np = 6;
	double dy = (1 - 0.3) / (np - 1);
	double x0 = xleft, y0 = 0.5;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_circle (g, x0, y0, r);
	double x = xright, y = 0.9;
	for (long i = 1; i <= np; i++) {
		if (i < 4 or i == np) {
			Graphics_circle (g, x, y, r);
			double xx = x0 - x, yy = y - y0;
			double c = sqrt (xx * xx + yy * yy);
			double cosa = xx / c, sina = yy / c;
			Graphics_line (g, x0 - r * cosa, y0 + r * sina, x + r * cosa, y - r * sina);
		} else if (i == 4) {
			double ddy = 3*dy/4;
			Graphics_fillCircle (g, x, y + dy - ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 2 * ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 3 * ddy, 0.5 * r);
		}
		y -= dy;
	}
	if (garnish) {
		double x1 = xright + 1.5 * r, x2 = x1 - 0.2, y1 = 0.9;

		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__1_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__%i1_");

		y1 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__2_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__%i2_");

		y1 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__%N_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__%%iN%_");

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x0 - 1.5 * r, y0, L"%s__%i_");

		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x0, 0, L"%t");
		Graphics_text (g, x, 0, L"%t+1");

		double y3 = 0.10;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x0, y3, L"%\\be__%t_(%i)%");
		Graphics_text (g, x, y3, L"%\\be__%t+1_(%j)");
	}
}

void HMM_drawForwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xmin = 0, xmax = 1, ymin = 0, ymax = 1;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double xleft  = 0.1, xright = 0.9, r = 0.03;
	long np = 6;
	double dy = (1 - 0.3) / (np - 1);
	double x0 = xright, y0 = 0.5;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_circle (g, x0, y0, r);
	double x = xleft, y = 0.9;
	for (long i = 1; i <= np; i++) {
		if (i < 4 or i == np) {
			Graphics_circle (g, x, y, r);
			double xx = x0 - x, yy = y - y0;
			double c = sqrt (xx * xx + yy * yy);
			double cosa = xx / c, sina = yy / c;
			Graphics_line (g, x0 - r * cosa, y0 + r * sina, x + r * cosa, y - r * sina);
		} else if (i == 4) {
			double ddy = 3*dy/4;
			Graphics_fillCircle (g, x, y + dy - ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 2 * ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 3 * ddy, 0.5 * r);
		}
		y -= dy;
	}
	if (garnish) {
		double x1 = xleft - 1.5 * r, x2 = x1 + 0.2, y1 = 0.9;

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__1_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__1%j_");

		y1 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__2_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__2%j_");

		y1 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, L"%s__%N_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, L"%a__%%Nj%_");

		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x0 + 1.5 * r, y0, L"%s__%j_");

		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x, 0, L"%t");
		Graphics_text (g, x0, 0, L"%t+1");

		double y3 = 0.10;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x, y3, L"%\\al__%t_(%i)%");
		Graphics_text (g, x0, y3, L"%\\al__%t+1_(%j)");
	}
}

void HMM_drawForwardAndBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xfrac = 0.1, xs =  1 / (0.5 - xfrac), r = 0.03;
	Graphics_Viewport vp = Graphics_insetViewport (g, 0, 0.5-xfrac, 0, 1);
	HMM_drawForwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_insetViewport (g, 0.5 + xfrac, 1, 0, 1);
	HMM_drawBackwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_setWindow (g, 0, xs, 0, 1);
	if (garnish) {
		double rx1 = 1 + xs * 2 * xfrac + 0.1, rx2 = rx1 + 0.9 - 0.1, y1 = 0.1;
		Graphics_line (g, 0.9 + r, 0.5, rx1 - r, 0.5);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.9, 0.5 + r, L"%s__%i_");
		Graphics_text (g, rx1, 0.5 + r, L"%s__%j_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
		Graphics_text (g, 1.0 + xfrac * xs, 0.5, L"%a__%%ij%_%b__%j_(O__%t+1_)");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.1, 0, L"%t-1");
		Graphics_text (g, 0.9, 0, L"%t");
		Graphics_text (g, rx1, 0, L"%t+1");
		Graphics_text (g, rx2, 0, L"%t+2");
		Graphics_setLineType (g, Graphics_DASHED);
		double x4 = rx1 - 0.06, x3 = 0.9 + 0.06;
		Graphics_line (g, x3, 0.7, x3, 0);
		Graphics_line (g, x4, 0.7, x4, 0);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_arrow (g, x4, y1, x4 + 0.2, y1);
		Graphics_arrow (g, x3, y1, x3 - 0.2, y1);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_BOTTOM);
		Graphics_text (g, x3 - 0.01, y1, L"\\al__%t_(i)");
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (g, x4 + 0.01, y1, L"\\be__%t+1_(j)");
	}
}

void HMM_and_HMM_BaumWelch_getXi (HMM me, HMM_BaumWelch thee, long *obs) {

	for (long it = 1; it <= thy numberOfTimes - 1; it++) {
		double sum = 0.0;
		for (long is = 1; is <= thy numberOfStates; is++) {
			for (long js = 1; js <= thy numberOfStates; js++) {
				thy xi[it][is][js] = thy alpha[is][it] * thy beta[js][it + 1] *
					my transitionProbs[is][js] * my emissionProbs[js][ obs[it + 1] ];
				sum += thy xi[it][is][js];
			}
		}
		for (long is = 1; is <= my numberOfStates; is++)
			for (long js = 1; js <= my numberOfStates; js++) {
				thy xi[it][is][js]  /= sum;
			}
	}
}

void HMM_and_HMM_BaumWelch_addEstimate (HMM me, HMM_BaumWelch thee, long *obs) {
	long is; // yes, outside

	for (is = 1; is <= my numberOfStates; is++) {
		// only for valid start states with p > 0
		if (my transitionProbs[0][is] > 0) {
			thy aij_num[0][is] += thy gamma[is][1];
			thy aij_denom[0][is] += 1;
		}
	}

	for (is = 1; is <= my numberOfStates; is++) {
		double gammasum = 0.0;
		for (long it = 1; it <= thy numberOfTimes - 1; it++) {
			gammasum += thy gamma[is][it];
		}

		for (long js = 1; js <= my numberOfStates; js++) {
			double xisum = 0.0;
			for (long it = 1; it <= thy numberOfTimes - 1; it++) {
				xisum += thy xi[it][is][js];
			}
			// zero probs signal invalid connections, don't reestimate
			if (my transitionProbs[is][js] > 0) {
				thy aij_num[is][js] += xisum;
				thy aij_denom[is][js] += gammasum;
			}
		}

		/*
			Only reestimate the emissionProbs for a hidden markov model.
			A not hidden model is emulated with fixed emissionProbs.
		*/
		if (!my notHidden) {
			gammasum += thy gamma[is][thy numberOfTimes]; // Now sum all, add last term
			for (long k = 1; k <= my numberOfObservationSymbols; k++) {
				double gammasum_k = 0.0;
				for (long it = 1; it <= thy numberOfTimes; it++) {
					if (obs[it] == k) {
						gammasum_k += thy gamma[is][it];
					}
				}
				// only reestimate probs > 0 !
				if (my emissionProbs[is][k] > 0) {
					thy bik_num[is][k] += gammasum_k;
					thy bik_denom[is][k] += gammasum;
				}
			}
		}
		// For a left-to-right model the final state determines the transition prob to go to the END state
		if (my leftToRight) {
			thy aij_num[is][my numberOfStates + 1] += thy gamma[is][thy numberOfTimes];
			thy aij_denom[is][my numberOfStates + 1] += 1;
		}
	}
}

void HMM_and_HMM_BaumWelch_reestimate (HMM me, HMM_BaumWelch thee) {
	double p;
	/*
		If we only have a couple of training sequences and they all happen to start with the same symbol,
		one or more starting probabilities can be zero.

		What to do with the P_k (see Rabiner formulas 109-110)?
	*/
	for (long is = 1; is <= my numberOfStates; is++) {
		/*
			If we have not enough observation sequences it can happen that some probabilities
			become zero. This means that for some future observation sequences the probability evaluation
			returns  p=0 for sequences where these transitions turn up. This makes recognition impossible and also comparisons between models are difficult.
			We can prevent this from happening by asumimg a minimal probability for valid transitions
			i.e. which have initially p > 0.
		*/
		if (my transitionProbs[0][is] > 0) {
			p = thy aij_num[0][is] / thy aij_denom[0][is];
			my transitionProbs[0][is] = p > 0 ? p : thy minProb;
		}
		for (long js = 1; js <= my numberOfStates; js++) {
			if (my transitionProbs[is][js] > 0) {
				p = thy aij_num[is][js] / thy aij_denom[is][js];
				my transitionProbs[is][js] = p > 0 ? p : thy minProb;
			}
		}
		if (! my notHidden) {
			for (long k = 1; k <= my numberOfObservationSymbols; k++) {
				if (my emissionProbs[is][k] > 0) {
					p = thy bik_num[is][k] / thy bik_denom[is][k];
					my emissionProbs[is][k] = p > 0 ? p : thy minProb;
				}
			}
		}
		if (my leftToRight && my transitionProbs[is][my numberOfStates + 1] > 0) {
			p = thy aij_num[is][my numberOfStates + 1] / thy aij_denom[is][my numberOfStates + 1];
			my transitionProbs[is][my numberOfStates + 1] = p > 0 ? p : thy minProb;
		}
	}
}

void HMM_and_HMM_BaumWelch_forward (HMM me, HMM_BaumWelch thee, long *obs) {
	// initialise at t = 1 & scale
	thy scale[1] = 0;
	for (long js = 1; js <= my numberOfStates; js++) {
		thy alpha[js][1] = my transitionProbs[0][js] * my emissionProbs[js][ obs[1] ];
		thy scale[1] += thy alpha[js][1];
	}
	for (long js = 1; js <= my numberOfStates; js++) {
		thy alpha[js][1] /= thy scale[1];
	}
	// recursion
	for (long it = 2; it <= thy numberOfTimes; it++) {
		thy scale[it] = 0.0;
		for (long js = 1; js <= my numberOfStates; js++) {
			double sum = 0.0;
			for (long is = 1; is <= my numberOfStates; is++) {
				sum += thy alpha[is][it - 1] * my transitionProbs[is][js];
			}

			thy alpha[js][it] = sum * my emissionProbs[js][ obs[it] ];
			thy scale[it] += thy alpha[js][it];
		}

		for (long js = 1; js <= my numberOfStates; js++) {
			thy alpha[js][it] /= thy scale[it];
		}
	}

	for (long it = 1; it <= thy numberOfTimes; it++) {
		thy lnProb += log (thy scale[it]);
	}
}

void HMM_and_HMM_BaumWelch_backward (HMM me, HMM_BaumWelch thee, long *obs) {
	for (long is = 1; is <= my numberOfStates; is++) {
		thy beta[is][thy numberOfTimes] = 1.0 / thy scale[thy numberOfTimes];
	}
	for (long it = thy numberOfTimes - 1; it >= 1; it--) {
		for (long is = 1; is <= my numberOfStates; is++) {
			double sum = 0.0;
			for (long js = 1; js <= my numberOfStates; js++) {
				sum += thy beta[js][it + 1] * my transitionProbs[is][js] * my emissionProbs[js][ obs[it + 1] ];
			}
			thy beta[is][it] = sum / thy scale[it];
		}
	}
}

/*************************** HMM decoding ***********************************/

// precondition: valid symbols, i.e. 1 <= o[i] <= my numberOfSymbols for i=1..nt
void HMM_and_HMM_Viterbi_decode (HMM me, HMM_Viterbi thee, long *obs) {
	long ntimes = thy numberOfTimes;
	// initialisation
	for (long is = 1; is <= my numberOfStates; is++) {
		thy viterbi[is][1] = my transitionProbs[0][is] * my emissionProbs[is][ obs[1] ];
		thy bp[is][1] = 0;
	}
	// recursion
	for (long it = 2; it <= ntimes; it++) {
		for (long is = 1; is <= my numberOfStates; is++) {
			// all transitions isp -> is from previous time to current
			double max_score = -1; // any negative number is ok
			for (long isp = 1; isp <= my numberOfStates; isp++) {
				double score = thy viterbi[isp][it - 1] * my transitionProbs[isp][is]; // * my emissionProbs[is][ obs[it] ]
				if (score > max_score) {
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
	for (long is = 2; is <= my numberOfStates; is++) {
		if (thy viterbi[is][ntimes] > thy prob) {
			thy prob = thy viterbi[ thy path[ntimes] = is ][ntimes];
		}
	}
	// trace back and get path
	for (long it = ntimes; it > 1; it--) {
		thy path[it - 1] = thy bp[ thy path[it] ][it];
	}
}

HMM_StateSequence HMM_and_HMM_ObservationSequence_to_HMM_StateSequence (HMM me, HMM_ObservationSequence thee) {
	try {
		autoStringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
		long *obs = si -> classIndex; // convenience
		long numberOfUnknowns = StringsIndex_countItems (si.peek(), 0);

		if (numberOfUnknowns > 0) {
			Melder_throw ("Unknown observation symbol(s) (# = ", Melder_integer (numberOfUnknowns), L").");
		}


		long numberOfTimes = thy rows -> size;
		autoHMM_Viterbi v = HMM_to_HMM_Viterbi (me, obs, numberOfTimes);
		autoHMM_StateSequence him = HMM_StateSequence_create (numberOfTimes);
		// trace the path and get states
		for (long it = 1; it <= numberOfTimes; it++) {
			HMM_State hmms = (HMM_State) my states -> item[ v -> path[it] ];
			his strings [it] = Melder_wcsdup (hmms -> label);
			his numberOfStrings ++;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no HMM_StateSequence created.");
	}
}

double HMM_and_HMM_StateSequence_getProbability (HMM me, HMM_StateSequence thee) {
	autoStringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
	long numberOfUnknowns = StringsIndex_countItems (si.peek(), 0);
	long *index = si -> classIndex;

	if (index == 0) {
		return NUMundefined;
	}
	if (numberOfUnknowns > 0) {
		Melder_warning (L"Unknown states (# = ", Melder_integer (numberOfUnknowns), L").");
		return NUMundefined;
	}
	double p0 = my transitionProbs [0][ index[1] ];
	if (p0 == 0) {
		Melder_throw ("You cannot start with this state.");
	}
	double lnp = log (p0);
	for (long it = 2; it <= thy numberOfStrings; it++) {
		lnp += log (my transitionProbs[ index[it - 1] ] [ index[it] ]);
	}
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInState (HMM me, long itime, long istate) {
	if (istate < 1 || istate > my numberOfStates) {
		return NUMundefined;
	}

	autoNUMvector<double> scale (1, itime);
	autoNUMvector<double> alpha_t (1, my numberOfStates);
	autoNUMvector<double> alpha_tm1 (1, my numberOfStates);

	for (long js = 1; js <= my numberOfStates; js++) {
		alpha_t[js] = my transitionProbs[0][js];
		scale[1] += alpha_t[js];
	}
	for (long js = 1; js <= my numberOfStates; js++) {
		alpha_t[js] /= scale[1];
	}
	// recursion
	for (long it = 2; it <= itime; it++) {
		for (long js = 1; js <= my numberOfStates; js++) {
			alpha_tm1[js] = alpha_t[js];
		}

		for (long js = 1; js <= my numberOfStates; js++) {
			double sum = 0.0;
			for (long is = 1; is <= my numberOfStates; is++) {
				sum += alpha_tm1[is] * my transitionProbs[is][js];
			}
			alpha_t[js] = sum;
			scale[it] += alpha_t[js];
		}

		for (long js = 1; js <= my numberOfStates; js++) {
			alpha_t[js] /= scale[it];
		}
	}

	double lnp = 0.0;
	for (long it = 1; it <= itime; it++) {
		lnp += log (scale[it]);
	}

	lnp = alpha_t[istate] > 0 ? lnp + log (alpha_t[istate]) : -INFINITY; // p = 0 -> ln(p)=-infinity
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, long itime, long istate, long isymbol) {
	// for a notHidden model emissionProbs may be zero!
	if (isymbol < 1 || isymbol > my numberOfObservationSymbols || my emissionProbs[istate][isymbol] == 0) {
		return NUMundefined;
	}
	double lnp = HMM_getProbabilityAtTimeBeingInState (me, itime, istate);
	return lnp != NUMundefined && lnp != -INFINITY ? lnp + log (my emissionProbs[istate][isymbol]) : lnp;
}

double HMM_getProbabilityOfObservations (HMM me, long *obs, long numberOfTimes) {
	autoNUMvector<double> scale (1, numberOfTimes);
	autoNUMvector<double> alpha_t (1, my numberOfStates);
	autoNUMvector<double> alpha_tm1 (1, my numberOfStates);

	// initialise
	for (long js = 1; js <= my numberOfStates; js++) {
		alpha_t[js] = my transitionProbs[0][js] * my emissionProbs[js][ obs[1] ];
		scale[1] += alpha_t[js];
	}
	if (scale[1] == 0) {
		Melder_throw ("The observation sequence starts with a symbol which state has starting probability zero.");
	}
	for (long js = 1; js <= my numberOfStates; js++) {
		alpha_t[js] /= scale[1];
	}

	// recursion
	for (long it = 2; it <= numberOfTimes; it++) {
		for (long js = 1; js <= my numberOfStates; js++) {
			alpha_tm1[js] = alpha_t[js];
		}

		for (long js = 1; js <= my numberOfStates; js++) {
			double sum = 0.0;
			for (long is = 1; is <= my numberOfStates; is++) {
				sum += alpha_tm1[is] * my transitionProbs[is][js];
			}

			alpha_t[js] = sum * my emissionProbs[js][ obs[it] ];
			scale[it] += alpha_t[js];
		}
		if (scale[it] <= 0) {
			return -INFINITY;
		}
		for (long js = 1; js <= my numberOfStates; js++) {
			alpha_t[js] /= scale[it];
		}
	}

	double lnp = 0;
	for (long it = 1; it <= numberOfTimes; it++) {
		lnp += log (scale[it]);
	}
	return lnp;
}

double HMM_and_HMM_ObservationSequence_getProbability (HMM me, HMM_ObservationSequence thee) {
	autoStringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
	long *index = si -> classIndex;
	long numberOfUnknowns = StringsIndex_countItems (si.peek(), 0);
	if (numberOfUnknowns > 0) {
		Melder_throw ("Unknown observations (# = ", numberOfUnknowns, ").");
	}
	return HMM_getProbabilityOfObservations (me, index, thy rows -> size);
}

double HMM_and_HMM_ObservationSequence_getCrossEntropy (HMM me, HMM_ObservationSequence thee) {
	double lnp = HMM_and_HMM_ObservationSequence_getProbability (me, thee);
	return lnp == NUMundefined ? NUMundefined : (lnp == -INFINITY ? INFINITY :
	        -lnp / (NUMln10 * HMM_ObservationSequence_getNumberOfObservations (thee)));
}

double HMM_and_HMM_ObservationSequence_getPerplexity (HMM me, HMM_ObservationSequence thee) {
	double ce = HMM_and_HMM_ObservationSequence_getCrossEntropy (me, thee);
	return ce == NUMundefined ? NUMundefined : (ce == INFINITY ? INFINITY : pow (2, ce));
}

HMM HMM_createFromHMM_ObservationSequence (HMM_ObservationSequence me, long numberOfStates, int leftToRight) {
	try {
		autoHMM thee = Thing_new (HMM);
		autoStrings s = HMM_ObservationSequence_to_Strings (me);
		autoDistributions d = Strings_to_Distributions (s.peek());

		long numberOfObservationSymbols = d -> numberOfRows;
		thy notHidden = numberOfStates < 1;
		numberOfStates = numberOfStates > 0 ? numberOfStates : numberOfObservationSymbols;

		HMM_init (thee.peek(), numberOfStates, numberOfObservationSymbols, leftToRight);

		for (long i = 1; i <= numberOfObservationSymbols; i++) {
			const wchar_t *label = d -> rowLabels[i];
			autoHMM_Observation hmmo = HMM_Observation_create (label, 0, 0, 0);
			HMM_addObservation (thee.peek(), hmmo.transfer());
			if (thy notHidden) {
				autoHMM_State hmms = HMM_State_create (label);
				HMM_addState (thee.peek(), hmms.transfer());
			}
		}
		if (! thy notHidden) {
			HMM_setDefaultStates (thee.peek());
		}
		HMM_setDefaultTransitionProbs (thee.peek());
		HMM_setDefaultStartProbs (thee.peek());
		HMM_setDefaultEmissionProbs (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no HMM created.");
	}
}

TableOfReal HMM_ObservationSequence_to_TableOfReal_transitions (HMM_ObservationSequence me, int probabilities) {
	try {
		autoStrings thee = HMM_ObservationSequence_to_Strings (me);
		autoTableOfReal him = Strings_to_TableOfReal_transitions (thee.peek(), probabilities);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transitions created.");
	}
}

StringsIndex HMM_and_HMM_ObservationSequence_to_StringsIndex (HMM me, HMM_ObservationSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = NUMvector<wchar_t *> (1, my numberOfObservationSymbols);
		for (long is = 1; is <= my numberOfObservationSymbols; is++) {
			HMM_Observation hmmo = (HMM_Observation) my observationSymbols -> item[is];
			classes -> strings[is] = Melder_wcsdup (hmmo -> label);
			(classes -> numberOfStrings) ++;
		}
		autoStrings obs = HMM_ObservationSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (obs.peek(), classes.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no StringsIndex created.");
	}
}

StringsIndex HMM_and_HMM_StateSequence_to_StringsIndex (HMM me, HMM_StateSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = NUMvector<wchar_t *> (1, my numberOfObservationSymbols);
		for (long is = 1; is <= my numberOfStates; is++) {
			HMM_State hmms = (HMM_State) my states -> item[is];
			classes -> strings[is] = Melder_wcsdup (hmms -> label);
			(classes -> numberOfStrings) ++;
		}
		autoStrings sts = HMM_StateSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (sts.peek(), classes.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no StringsIndex created.");
	}
}

TableOfReal HMM_and_HMM_ObservationSequence_to_TableOfReal_transitions (HMM me, HMM_ObservationSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_and_HMM_ObservationSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.peek(), probabilities);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transition table created for HMM_ObservationSequence.");
	}
}

TableOfReal HMM_and_HMM_StateSequence_to_TableOfReal_transitions (HMM me, HMM_StateSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_and_HMM_StateSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.peek(), probabilities);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transition table created for HMM_StateSequence.");
	}
}

TableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities) {
	try {
		long numberOfTypes = my classes -> size;

		autoTableOfReal thee = TableOfReal_create (numberOfTypes + 1, numberOfTypes + 1);
		for (long i = 1; i <= numberOfTypes; i++) {
			SimpleString s = (SimpleString) my classes -> item[i];
			TableOfReal_setRowLabel (thee.peek(), i, s -> string);
			TableOfReal_setColumnLabel (thee.peek(), i, s -> string);
		}
		for (long i = 2; i <= my numberOfElements; i++) {
			if (my classIndex[i - 1] > 0 && my classIndex[i] > 0) { // a zero is a restart!
				thy data [my classIndex[i - 1]][my classIndex[i]] ++;
			}
		}
		double sum = 0;
		for (long i = 1; i <= numberOfTypes; i++) {
			double rowSum = 0, colSum = 0;
			for (long j = 1; j <= numberOfTypes; j++) {
				rowSum += thy data[i][j];
			}
			thy data[i][numberOfTypes + 1] = rowSum;
			for (long j = 1; j <= numberOfTypes; j++) {
				colSum += thy data[j][i];
			}
			thy data[numberOfTypes + 1][i] = colSum;
			sum += colSum;
		}
		thy data[numberOfTypes + 1][numberOfTypes + 1] = sum;
		if (probabilities && sum > 0) {
			for (long i = 1; i <= numberOfTypes; i++) {
				if (thy data[i][numberOfTypes + 1] > 0) {
					for (long j = 1; j <= numberOfTypes; j++) {
						thy data[i][j] /= thy data[i][numberOfTypes + 1];
					}
				}
			}
			for (long i = 1; i <= numberOfTypes; i++) {
				thy data[i][numberOfTypes + 1] /= sum;
				thy data[numberOfTypes + 1][i] /= sum;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transition table created.");
	}
}

TableOfReal Strings_to_TableOfReal_transitions (Strings me, int probabilities) {
	try {
		autoStringsIndex him = Strings_to_StringsIndex (me);
		autoTableOfReal thee = StringsIndex_to_TableOfReal_transitions (him.peek(), probabilities);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no transition table created.");
	}
}

/* End of file HMM.cpp */
