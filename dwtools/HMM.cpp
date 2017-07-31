/* HMM.cpp
 *
 * Copyright (C) 2010-2012,2015 David Weenink, 2015,2017 Paul Boersma
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

Thing_implement (HMM, Daata, 0);
Thing_implement (HMMState, Daata, 0);
Thing_implement (HMMStateList, Ordered, 0);
Thing_implement (HMMObservation, Daata, 0);
Thing_implement (HMMObservationList, Ordered, 0);
Thing_implement (HMMBaumWelch, Daata, 0);
Thing_implement (HMMViterbi, Daata, 0);
Thing_implement (HMMObservationSequence, Table, 0);
Thing_implement (HMMObservationSequenceBag, Collection, 0);
Thing_implement (HMMStateSequence, Strings, 0);

/*
	Whenever a routine returns ln(p), the result for p=0 is -INFINITY.
	On IEEE floating point hardware this number behaves reasonably.
	This means that when the variable q equals INFINITY, q + a -> INFINITY,
	where a is a finite number.
*/

// helpers
int NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b, double *x1, double *y1, double *x2, double *y2);
autoHMMObservation HMMObservation_create (const char32 *label, long numberOfComponents, long dimension, long storage);

long HMM_and_HMMObservationSequence_getLongestSequence (HMM me, HMMObservationSequence thee, long symbolNumber);
long StringsIndex_getLongestSequence (StringsIndex me, long index, long *pos);
long Strings_getLongestSequence (Strings me, char32 *string, long *pos);
autoHMMState HMMState_create (const char32 *label);

autoHMMBaumWelch HMMBaumWelch_create (long nstates, long nsymbols, long capacity);
void HMMBaumWelch_getGamma (HMMBaumWelch me);
autoHMMBaumWelch HMM_forward (HMM me, long *obs, long nt);
void HMMBaumWelch_reInit (HMMBaumWelch me);
void HMM_and_HMMBaumWelch_getXi (HMM me, HMMBaumWelch thee, long *obs);
void HMM_and_HMMBaumWelch_reestimate (HMM me, HMMBaumWelch thee);
void HMM_and_HMMBaumWelch_addEstimate (HMM me, HMMBaumWelch thee, long *obs);
void HMM_and_HMMBaumWelch_forward (HMM me, HMMBaumWelch thee, long *obs);
void HMM_and_HMMBaumWelch_backward (HMM me, HMMBaumWelch thee, long *obs);
void HMM_and_HMMViterbi_decode (HMM me, HMMViterbi thee, long *obs);
double HMM_getProbabilityOfObservations (HMM me, long *obs, long numberOfTimes);
autoTableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities);
autoStringsIndex HMM_and_HMMStateSequence_to_StringsIndex (HMM me, HMMStateSequence thee);


autoHMMViterbi HMMViterbi_create (long nstates, long ntimes);
autoHMMViterbi HMM_to_HMMViterbi (HMM me, long *obs, long ntimes);

// evaluate the numbers given to probabilities
static double *NUMwstring_to_probs (char32 *s, long nwanted) {
	long numbers_found;
	autoNUMvector<double> numbers (NUMstring_to_numbers (s, & numbers_found), 1);
	if (numbers_found != nwanted) {
		Melder_throw (U"You supplied ", numbers_found, U", while ", nwanted, U" numbers needed.");
	}
	double sum = 0;
	for (long i = 1; i <= numbers_found; i++) {
		if (numbers[i] < 0) {
			Melder_throw (U"Numbers have to be positive.");
		}
		sum += numbers [i];
	}
	if (sum <= 0) {
		Melder_throw (U"All probabilities cannot be zero.");
	}
	for (long i = 1; i <= numbers_found; i ++) {
		numbers [i] /= sum;
	}
	return numbers.transfer();
}

int NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b, double *x1, double *y1, double *x2, double *y2) {
	double ca = a * a + 1.0, bmyc = (b - yc);
	double cb = 2.0 * (a * bmyc - xc);
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
	autoHMMObservationSequence os = HMM_to_HMMObservationSequence (thee, 0, observationLength);
	double ce = HMM_and_HMMObservationSequence_getCrossEntropy (me, os.get());
	if (isundef (ce)) {
		return ce;
	}
	double ce2 = HMM_and_HMMObservationSequence_getCrossEntropy (thee, os.get());
	if (isundef (ce2)) {
		return ce2;
	}
	return ce - ce2;
}

/**************** HMMObservation ******************************/

static void HMMObservation_init (HMMObservation me, const char32 *label, long numberOfComponents, long dimension, long storage) {
	my label = Melder_dup (label);
	my gm = GaussianMixture_create (numberOfComponents, dimension, storage);
}

autoHMMObservation HMMObservation_create (const char32 *label, long numberOfComponents, long dimension, long storage) {
	try {
		autoHMMObservation me = Thing_new (HMMObservation);
		HMMObservation_init (me.get(), label, numberOfComponents, dimension, storage);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMObservation not created.");
	}
}

long Strings_getLongestSequence (Strings me, char32 *string, long *pos) {
	long length = 0, longest = 0, lpos = 0;
	for (long i = 1; i <= my numberOfStrings; i++) {
		if (Melder_equ (my strings[i], string)) {
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
	for (long i = 1; i <= my numberOfItems; i++) {
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

/**************** HMMState ******************************/

static void HMMState_init (HMMState me, const char32 *label) {
	my label = Melder_dup (label);
}

autoHMMState HMMState_create (const char32 *label) {
	try {
		autoHMMState me = Thing_new (HMMState);
		HMMState_init (me.get(), label);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMState not created.");
	}
}

void HMMState_setLabel (HMMState me, char32 *label) {
	Melder_free (my label);
	my label = Melder_dup (label);
}

/**************** HMMBaumWelch ******************************/

void structHMMBaumWelch :: v_destroy () noexcept {
	for (long it = 1; it <= capacity; it ++) {
		NUMmatrix_free (xi[it], 1, 1);
	}
	NUMvector_free (xi, 1);
	NUMvector_free (scale, 1);
	NUMmatrix_free (beta, 1, 1);
	NUMmatrix_free (alpha, 1, 1);
	NUMmatrix_free (gamma, 1, 1);
	NUMmatrix_free (aij_num, 0, 1);
	NUMmatrix_free (aij_denom, 0, 1);
	NUMmatrix_free (bik_num, 1, 1);
	NUMmatrix_free (bik_denom, 1, 1);
}

autoHMMBaumWelch HMMBaumWelch_create (long nstates, long nsymbols, long capacity) {
	try {
		autoHMMBaumWelch me = Thing_new (HMMBaumWelch);
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
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMBaumWelch not created.");
	}
}

void HMMBaumWelch_getGamma (HMMBaumWelch me) {
	for (long it = 1; it <= my numberOfTimes; it ++) {
		double sum = 0.0;
		for (long is = 1; is <= my numberOfStates; is ++) {
			my gamma [is] [it] = my alpha [is] [it] * my beta [is] [it];
			sum += my gamma [is] [it];
		}

		for (long is = 1; is <= my numberOfStates; is ++) {
			my gamma [is] [it] /= sum;
		}
	}
}

/**************** HMMViterbi ******************************/

autoHMMViterbi HMMViterbi_create (long nstates, long ntimes) {
	try {
		autoHMMViterbi me = Thing_new (HMMViterbi);
		my numberOfTimes = ntimes;
		my numberOfStates = nstates;
		my viterbi = NUMmatrix<double> (1, nstates, 1, ntimes);
		my bp = NUMmatrix<long> (1, nstates, 1 , ntimes);
		my path = NUMvector<long> (1, ntimes);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMViterbi not created.");
	}
}

/******************* HMMObservationSequence & HMMStateSequence ***/

autoHMMObservationSequence HMMObservationSequence_create (long numberOfItems, long dataLength) {
	try {
		autoHMMObservationSequence me = Thing_new (HMMObservationSequence);
		Table_initWithoutColumnNames (me.get(), numberOfItems, dataLength + 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMObservationSequence not created.");
	}
}

long HMMObservationSequence_getNumberOfObservations (HMMObservationSequence me) {
	return my rows.size;
}

void HMMObservationSequence_removeObservation (HMMObservationSequence me, long index) {
	Table_removeRow ( (Table) me, index);
}

autoStrings HMMObservationSequence_to_Strings (HMMObservationSequence me) {
	try {
		long numberOfStrings = my rows.size;
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<char32 *> (1, numberOfStrings);
		for (long i = 1; i <= numberOfStrings; i++) {
			thy strings[i] = Melder_dup_f (Table_getStringValue_Assert ( (Table) me, i, 1));
			(thy numberOfStrings) ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}

autoHMMObservationSequence Strings_to_HMMObservationSequence (Strings me) {
	try {
		autoHMMObservationSequence thee = HMMObservationSequence_create (my numberOfStrings, 0);
		for (long i = 1; i <= my numberOfStrings; i++) {
			Table_setStringValue ( (Table) thee.get(), i, 1, my strings[i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMObservationSequence created.");
	}
}

autoStringsIndex HMMObservationSequence_to_StringsIndex (HMMObservationSequence me) {
	try {
		autoStrings s = HMMObservationSequence_to_Strings (me);
		autoStringsIndex thee = Strings_to_StringsIndex (s.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

long HMM_and_HMMObservationSequence_getLongestSequence (HMM me, HMMObservationSequence thee, long symbolNumber) {
	autoStringsIndex si = HMM_and_HMMObservationSequence_to_StringsIndex (me, thee);
	// TODO
	(void) symbolNumber;
	return 1;
}

long HMMObservationSequenceBag_getLongestSequence (HMMObservationSequenceBag me) {
	long longest = 0;
	for (long i = 1; i <= my size; i ++) {
		HMMObservationSequence thee = my at [i];
		if (thy rows.size > longest) {
			longest = thy rows.size;
		}
	}
	return longest;
}

autoHMMStateSequence HMMStateSequence_create (long numberOfItems) {
	try {
		autoHMMStateSequence me = Thing_new (HMMStateSequence);
		my strings = NUMvector<char32 *> (1, numberOfItems);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMStateSequence not created.");
	}
}

autoStrings HMMStateSequence_to_Strings (HMMStateSequence me) {
	try {
		autoStrings thee = Thing_new (Strings);
		my structStrings :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}


/**************** HMM ******************************/

void structHMM :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of states: ", numberOfStates);
	for (long i = 1; i <= numberOfStates; i ++) {
		HMMState hmms = our states->at [i];
		MelderInfo_writeLine (U"  ", hmms -> label);
	}
	MelderInfo_writeLine (U"Number of symbols: ", numberOfObservationSymbols);
	for (long i = 1; i <= numberOfObservationSymbols; i ++) {
		HMMObservation hmms = our observationSymbols->at [i];
		MelderInfo_writeLine (U"  ", hmms -> label);
	}
}

static void HMM_init (HMM me, long numberOfStates, long numberOfObservationSymbols, int leftToRight) {
	my numberOfStates = numberOfStates;
	my numberOfObservationSymbols = numberOfObservationSymbols;
	my componentStorage = 1;
	my leftToRight = leftToRight;
	my states = HMMStateList_create ();
	my observationSymbols = HMMObservationList_create ();
	my transitionProbs = NUMmatrix<double> (0, numberOfStates, 1, numberOfStates + 1);
	my emissionProbs = NUMmatrix<double> (1, numberOfStates, 1, numberOfObservationSymbols);
}

autoHMM HMM_create (int leftToRight, long numberOfStates, long numberOfObservationSymbols) {
	try {
		autoHMM me = Thing_new (HMM);
		HMM_init (me.get(), numberOfStates, numberOfObservationSymbols, leftToRight);
		HMM_setDefaultStates (me.get());
		HMM_setDefaultObservations (me.get());
		HMM_setDefaultTransitionProbs (me.get());
		HMM_setDefaultStartProbs (me.get());
		HMM_setDefaultEmissionProbs (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMM not created.");
	}
}

void HMM_setDefaultStates (HMM me) {
	for (long i = 1; i <= my numberOfStates; i++) {
		autoHMMState hmms = HMMState_create (Melder_cat (U"S", i));
		HMM_addState_move (me, hmms.move());
	}
}

autoHMM HMM_createFullContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols, long numberOfFeatureStreams, long *dimensionOfStream, long *numberOfGaussiansforStream) {
	(void) leftToRight;
	(void) numberOfStates;
	(void) numberOfObservationSymbols;
	(void) numberOfFeatureStreams;
	(void) dimensionOfStream;
	(void) numberOfGaussiansforStream;
	return autoHMM();
}

autoHMM HMM_createContinuousModel (int leftToRight, long numberOfStates, long numberOfObservationSymbols, long numberOfMixtureComponentsPerSymbol, long componentDimension, long componentStorage) {
	try {
		autoHMM me = Thing_new (HMM);
		HMM_init (me.get(), numberOfStates, numberOfObservationSymbols, leftToRight);
		my numberOfMixtureComponents = numberOfMixtureComponentsPerSymbol;
		my componentDimension = componentDimension;
		my componentStorage = componentStorage;
		for (long i = 1; i <= numberOfStates; i++) {
			autoHMMState state = HMMState_create (Melder_cat (U"S", i));
			HMM_addState_move (me.get(), state.move());
		}
		for (long j = 1; j <= numberOfObservationSymbols; j++) {
			autoHMMObservation obs = HMMObservation_create (Melder_cat (U"s", j), numberOfMixtureComponentsPerSymbol, componentDimension, componentStorage);
			HMM_addObservation_move (me.get(), obs.move());
		}
		HMM_setDefaultTransitionProbs (me.get());
		HMM_setDefaultStartProbs (me.get());
		HMM_setDefaultEmissionProbs (me.get());
		HMM_setDefaultMixingProbabilities (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Continuous model HMM not created.");
	}
}

// for a simple non-hidden model leave either states empty or symbols empty !!!
autoHMM HMM_createSimple (int leftToRight, const char32 *states_string, const char32 *symbols_string) {
	try {
		autoHMM me = Thing_new (HMM);
		const char32 *states = states_string;
		const char32 *symbols = symbols_string;
		long numberOfStates = Melder_countTokens (states_string);
		long numberOfObservationSymbols = Melder_countTokens (symbols_string);

		if (numberOfStates == 0 and numberOfObservationSymbols == 0) {
			Melder_throw (U"No states and no symbols given.");
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

		HMM_init (me.get(), numberOfStates, numberOfObservationSymbols, leftToRight);

		for (char32 *token = Melder_firstToken (states); token != 0; token = Melder_nextToken ()) {
			autoHMMState state = HMMState_create (token);
			HMM_addState_move (me.get(), state.move());
		}
		for (char32 *token = Melder_firstToken (symbols); token != nullptr; token = Melder_nextToken ()) {
			autoHMMObservation symbol = HMMObservation_create (token, 0, 0, 0);
			HMM_addObservation_move (me.get(), symbol.move());
		}
		HMM_setDefaultTransitionProbs (me.get());
		HMM_setDefaultStartProbs (me.get());
		HMM_setDefaultEmissionProbs (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple HMM not created.");
	}
}

void HMM_setDefaultObservations (HMM me) {
	const char32 *def = my notHidden ? U"S" : U"s";
	for (long i = 1; i <= my numberOfObservationSymbols; i ++) {
		autoHMMObservation hmms = HMMObservation_create (Melder_cat (def, i), 0, 0, 0);
		HMM_addObservation_move (me, hmms.move());
	}
}

void HMM_setDefaultTransitionProbs (HMM me) {
	for (long i = 1; i <= my numberOfStates; i ++) {
		double p = ( my leftToRight ? 1.0 / (my numberOfStates - i + 1.0) : 1.0 / my numberOfStates );
		for (long j = 1; j <= my numberOfStates; j++) {
			my transitionProbs [i] [j] = ( my leftToRight && j < i ? 0.0 : p );
		}
	}
	// leftToRight must have end state!
	if (my leftToRight) my transitionProbs [my numberOfStates] [my numberOfStates] =
		    my transitionProbs [my numberOfStates] [my numberOfStates + 1] = 0.5;
}

void HMM_setDefaultStartProbs (HMM me) {
	double p = 1.0 / my numberOfStates;
	for (long j = 1; j <= my numberOfStates; j ++) {
		my transitionProbs [0] [j] = p;
	}
}

void HMM_setDefaultEmissionProbs (HMM me) {
	double p = 1.0 / my numberOfObservationSymbols;
	for (long i = 1; i <= my numberOfStates; i++)
		for (long j = 1; j <= my numberOfObservationSymbols; j ++) {
			my emissionProbs [i] [j] = my notHidden ? ( i == j ? 1.0 : 0.0 ) : p;
		}
}

void HMM_setDefaultMixingProbabilities (HMM me) {
	double mp = 1.0 / my numberOfMixtureComponents;
	for (long is = 1; is <= my numberOfObservationSymbols; is ++) {
		HMMObservation hmmo = my observationSymbols->at [is];
		for (long im = 1; im <= my numberOfMixtureComponents; im ++) {
			hmmo -> gm -> mixingProbabilities [im] = mp;
		}
	}
}

void HMM_setStartProbabilities (HMM me, char32 *probs) {
	try {
		autoNUMvector<double> p (NUMwstring_to_probs (probs, my numberOfStates), 1);
		for (long i = 1; i <= my numberOfStates; i++) {
			my transitionProbs[0][i] = p[i];
		}
	} catch (MelderError) {
		Melder_throw (me, U": no start probabilities set.");
	}
}

void HMM_setTransitionProbabilities (HMM me, long state_number, char32 *state_probs) {
	try {
		if (state_number > my states->size) {
			Melder_throw (U"State number too large.");
		}
		autoNUMvector<double> p (NUMwstring_to_probs (state_probs, my numberOfStates), 1);
		for (long i = 1; i <= my numberOfStates + 1; i ++) {
			my transitionProbs [state_number] [i] = p [i];
		}
	} catch (MelderError) {
		Melder_throw (me, U": no transition probabilities set.");
	}
}

void HMM_setEmissionProbabilities (HMM me, long state_number, char32 *emission_probs) {
	try {
		if (state_number > my states->size) {
			Melder_throw (U"State number too large.");
		}
		if (my notHidden) {
			Melder_throw (U"The emission probs of this model are fixed.");
		}
		autoNUMvector<double> p (NUMwstring_to_probs (emission_probs, my numberOfObservationSymbols), 1);
		for (long i = 1; i <= my numberOfObservationSymbols; i++) {
			my emissionProbs [state_number] [i] = p [i];
		}
	} catch (MelderError) {
		Melder_throw (me, U": no emission probabilities set.");
	}

}

void HMM_addObservation_move (HMM me, autoHMMObservation thee) {
	long ns = my observationSymbols->size + 1;
	if (ns > my numberOfObservationSymbols) {
		Melder_throw (U"Observation list is full.");
	}
	my observationSymbols -> addItemAtPosition_move (thee.move(), ns);
}

void HMM_addState_move (HMM me, autoHMMState thee) {
	long ns = my states->size + 1;
	if (ns > my numberOfStates) {
		Melder_throw (U"States list is full.");
	}
	my states -> addItemAtPosition_move (thee.move(), ns);
}

autoTableOfReal HMM_extractTransitionProbabilities (HMM me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfStates + 1, my numberOfStates + 1);
		for (long is = 1; is <= my numberOfStates; is ++) {
			HMMState hmms = my states->at [is];
			TableOfReal_setRowLabel (thee.get(), is + 1, hmms -> label);
			TableOfReal_setColumnLabel (thee.get(), is, hmms -> label);
			for (long js = 1; js <= my numberOfStates; js ++) {
				thy data [is + 1] [js] = my transitionProbs [is] [js];
			}
		}
		TableOfReal_setRowLabel (thee.get(), 1, U"START");
		TableOfReal_setColumnLabel (thee.get(), my numberOfStates + 1, U"END");
		for (long is = 1; is <= my numberOfStates; is ++) {
			thy data [1] [is] = my transitionProbs [0] [is];
			thy data [is + 1] [my numberOfStates + 1] = my transitionProbs [is] [my numberOfStates + 1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no transition probabilities extracted.");
	}
}

autoTableOfReal HMM_extractEmissionProbabilities (HMM me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfStates, my numberOfObservationSymbols);
		for (long js = 1; js <= my numberOfObservationSymbols; js ++) {
			HMMObservation hmms = my observationSymbols->at [js];
			TableOfReal_setColumnLabel (thee.get(), js, hmms -> label);
		}
		for (long is = 1; is <= my numberOfStates; is ++) {
			HMMState hmms = my states->at [is];
			TableOfReal_setRowLabel (thee.get(), is, hmms -> label);
			for (long js = 1; js <= my numberOfObservationSymbols; js ++) {
				thy data [is] [js] = my emissionProbs [is] [js];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no emission probabilities extracted.");
	};
}

double HMM_getExpectedValueOfDurationInState (HMM me, long istate) {
	if (istate < 0 || istate > my numberOfStates) {
		return undefined;
	}
	return 1.0 / (1.0 - my transitionProbs [istate] [istate]);
}

double HMM_getProbabilityOfStayingInState (HMM me, long istate, long numberOfTimeUnits) {
	if (istate < 0 || istate > my numberOfStates) {
		return undefined;
	}
	return pow (my transitionProbs [istate] [istate], numberOfTimeUnits - 1.0) * (1.0 - my transitionProbs[istate][istate]);
}

double HMM_and_HMM_getCrossEntropy (HMM me, HMM thee, long observationLength, int symmetric) {
	double ce1 = HMM_and_HMM_getCrossEntropy_asym (me, thee, observationLength);
	if (! symmetric || isundef (ce1)) {
		return ce1;
	}
	double ce2 = HMM_and_HMM_getCrossEntropy_asym (thee, me, observationLength);
	if (isundef (ce2)) {
		return ce2;
	}
	return (ce1 + ce2) / 2.0;
}

double HMM_and_HMM_and_HMMObservationSequence_getCrossEntropy (HMM me, HMM thee, HMMObservationSequence him) {
	double ce1 = HMM_and_HMMObservationSequence_getCrossEntropy (me, him);
	if (isundef (ce1)) {
		return ce1;
	}
	double ce2 = HMM_and_HMMObservationSequence_getCrossEntropy (thee, him);
	if (isundef (ce2)) {
		return ce2;
	}
	return (ce1 + ce2) / 2.0;
}

void HMM_draw (HMM me, Graphics g, int garnish) {
	double xwidth = sqrt (my numberOfStates);
	double rstate = 0.3 / xwidth, r = xwidth / 3.0;
	double xmax = 1.2 * xwidth / 2.0, xmin = -xmax, ymin = xmin, ymax = xmax;

	autoNUMvector<double> xs (1, my numberOfStates);
	autoNUMvector<double> ys (1, my numberOfStates);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	// heuristic: all states on a circle until we have a better graph drawing algorithm.
	xs[1] = ys[1] = 0;
	if (my numberOfStates > 1) {
		for (long is = 1; is <= my numberOfStates; is++) {
			double alpha = - NUMpi + NUMpi * 2.0 * (is - 1) / my numberOfStates;
			xs[is] = r * cos (alpha); ys[is] = r * sin (alpha);
		}
	}
	// reorder the positions such that state number 1 is most left and last state number is right.
	// if > 5 may be one in the middle with the most connections
	// ...
	// find fontsize
	int fontSize = Graphics_inqFontSize (g);
	const char32 *widest_label = U"";
	double max_width = 0.0;
	for (long is = 1; is <= my numberOfStates; is ++) {
		HMMState hmms = my states->at [is];
		double w = ( hmms -> label == nullptr ? 0.0 : Graphics_textWidth (g, hmms -> label) );
		if (w > max_width) {
			widest_label = hmms -> label;
			max_width = w;
		}
	}
	int new_fontSize = fontSize;
	while (max_width > 2.0 * rstate && new_fontSize > 4) {
		new_fontSize --;
		Graphics_setFontSize (g, new_fontSize);
		max_width = Graphics_textWidth (g, widest_label);
	}
	Graphics_setFontSize (g, new_fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (long is = 1; is <= my numberOfStates; is ++) {
		HMMState hmms = my states->at [is];
		Graphics_circle (g, xs [is], ys [is], rstate);
		Graphics_text (g, xs [is], ys [is], hmms -> label);
	}

	// draw connections from is to js
	// 1 -> 2 / 2-> : increase / decrease angle between 1 and 2 with pi /10
	// use cos(a+b) and cos(a -b) rules
	double cosb = cos (NUMpi / 10.0), sinb = sin (NUMpi / 10.0);
	for (long is = 1; is <= my numberOfStates; is ++) {
		double x1 = xs [is], y1 = ys [is];
		for (long js = 1; js <= my numberOfStates; js ++) {
			if (my transitionProbs [is] [js] > 0.0 && is != js) {
				double x2 = xs [js], y2 = ys [js];
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
	for (long is = 1; is <= my numberOfObservationSymbols; is ++) {
		HMMObservation s = my observationSymbols->at [is];
		GaussianMixture_unExpandPCA (s -> gm.get());
	}
}

autoHMMObservationSequence HMM_to_HMMObservationSequence (HMM me, long startState, long numberOfItems) {
	try {
		autoHMMObservationSequence thee = HMMObservationSequence_create (numberOfItems, my componentDimension);
		autoNUMvector<double> obs;
		autoNUMvector<double> buf;
		if (my componentDimension > 0) {
			obs.reset (1, my componentDimension);
			buf.reset (1, my componentDimension);
		}
		long istate = startState == 0 ? NUMgetIndexFromProbability (my transitionProbs[0], my numberOfStates, NUMrandomUniform (0.0, 1.0)) : startState;
		for (long i = 1; i <= numberOfItems; i++) {
			// Emit a symbol from istate

			long isymbol = NUMgetIndexFromProbability (my emissionProbs[istate], my numberOfObservationSymbols, NUMrandomUniform (0.0, 1.0));
			HMMObservation s = my observationSymbols->at [isymbol];

			if (my componentDimension > 0) {
				char32 *name;
				GaussianMixture_generateOneVector (s -> gm.get(), obs.peek(), &name, buf.peek());
				for (long j = 1; j <= my componentDimension; j++) {
					Table_setNumericValue ( (Table) thee.get(), i, 1 + j, obs[j]);
				}
			}

			Table_setStringValue (thee.get(), i, 1, s -> label);

			// get next state

			istate = NUMgetIndexFromProbability (my transitionProbs [istate], my numberOfStates + 1, NUMrandomUniform (0.0, 1.0));
			if (istate == my numberOfStates + 1) { // final state
				for (long j = numberOfItems; j > i; j --) {
					HMMObservationSequence_removeObservation (thee.get(), j);
				}
				break;
			}
		}
		HMM_unExpandPCA (me);
		return thee;
	} catch (MelderError) {
		HMM_unExpandPCA (me);
		Melder_throw (me, U":no HMMObservationSequence created.");
	}
}

autoHMMBaumWelch HMM_forward (HMM me, long *obs, long nt) {
	try {
		autoHMMBaumWelch thee = HMMBaumWelch_create (my numberOfStates, my numberOfObservationSymbols, nt);
		HMM_and_HMMBaumWelch_forward (me, thee.get(), obs);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMBaumWelch created.");
	}
}

autoHMMViterbi HMM_to_HMMViterbi (HMM me, long *obs, long ntimes) {
	try {
		autoHMMViterbi thee = HMMViterbi_create (my numberOfStates, ntimes);
		HMM_and_HMMViterbi_decode (me, thee.get(), obs);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMViterbi created.");
	}
}

void HMMBaumWelch_reInit (HMMBaumWelch me) {
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
			my aij_num[is][js] = 0.0;
			my aij_denom[is][js] = 0.0;
		}
	}
	for (long is = 1; is <= my numberOfStates; is++) {
		for (long js = 1; js <= my numberOfSymbols; js++) {
			my bik_num[is][js] = 0.0;
			my bik_denom[is][js] = 0.0;
		}
	}
}

void HMM_and_HMMObservationSequenceBag_learn (HMM me, HMMObservationSequenceBag thee, double delta_lnp, double minProb, int info) {
	try {
		// act as if all observation sequences are in memory
		long capacity = HMMObservationSequenceBag_getLongestSequence (thee);
		autoHMMBaumWelch bw = HMMBaumWelch_create (my numberOfStates, my numberOfObservationSymbols, capacity);
		bw -> minProb = minProb;
		if (info) {
			MelderInfo_open (); 
		}
		long iter = 0; double lnp;
		do {
			lnp = bw -> lnProb;
			HMMBaumWelch_reInit (bw.get());
			for (long ios = 1; ios <= thy size; ios ++) {
				HMMObservationSequence hmm_os = thy at [ios];
				autoStringsIndex si = HMM_and_HMMObservationSequence_to_StringsIndex (me, hmm_os); // TODO outside the loop or more efficiently
				long *obs = si -> classIndex, nobs = si -> numberOfItems; // convenience

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
					HMM_and_HMMBaumWelch_forward (me, bw.get(), obs + istart - 1); // get new alphas
					HMM_and_HMMBaumWelch_backward (me, bw.get(), obs + istart - 1); // get new betas
					HMMBaumWelch_getGamma (bw.get());
					HMM_and_HMMBaumWelch_getXi (me, bw.get(), obs + istart - 1);
					HMM_and_HMMBaumWelch_addEstimate (me, bw.get(), obs + istart - 1);
					istart = iend + 1;
				}
			}
			// we have processed all observation sequences, now it is time to estimate new probabilities.
			iter++;
			HMM_and_HMMBaumWelch_reestimate (me, bw.get());
			if (info) { 
				MelderInfo_writeLine (U"Iteration: ", iter, U" ln(prob): ", bw -> lnProb); 
			}
		} while (fabs ((lnp - bw -> lnProb) / bw -> lnProb) > delta_lnp);
		if (info) {
			MelderInfo_writeLine (U"******** Learning summary *********");
			MelderInfo_writeLine (U"  Processed ", thy size, U" sequences,");
			MelderInfo_writeLine (U"  consisting of ", bw -> totalNumberOfSequences, U" observation sequences.");
			MelderInfo_writeLine (U"  Longest observation sequence had ", capacity, U" items");
			MelderInfo_close();
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not learned.");
	}
}


// xc1 < xc2
void HMM_and_HMMStateSequence_drawTrellis (HMM me, HMMStateSequence thee, Graphics g, int connect, int garnish) {
	long numberOfTimes = thy numberOfStrings;
	autoStringsIndex si = HMM_and_HMMStateSequence_to_StringsIndex (me, thee);
	double xmin = 0.0, xmax = numberOfTimes + 1.0, ymin = 0.5, ymax = my numberOfStates + 0.5;

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
					Graphics_setLineWidth (g, indexedConnection ? 2.0 : 1.0);
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
	Graphics_setLineWidth (g, 1.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	if (garnish) {
		Graphics_drawInnerBox (g);
		for (long js = 1; js <= my numberOfStates; js ++) {
			HMMState hmms = my states->at [js];
			Graphics_markLeft (g, js, false, false, false, hmms -> label);
		}
		Graphics_marksBottomEvery (g, 1.0, 1.0, true, true, false);
		Graphics_textBottom (g, true, U"Time index");
	}
}

void HMM_drawBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xmin = 0.0, xmax = 1.0, ymin = 0.0, ymax = 1.0;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double xleft  = 0.1, xright = 0.9, r = 0.03;
	long np = 6;
	double dy = (1.0 - 0.3) / (np - 1);
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
		Graphics_text (g, x1, y1, U"%s__1_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%i1_");

		y1 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__2_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%i2_");

		y1 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__%N_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%%iN%_");

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x0 - 1.5 * r, y0, U"%s__%i_");

		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x0, 0.0, U"%t");
		Graphics_text (g, x, 0.0, U"%t+1");

		double y3 = 0.10;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x0, y3, U"%\\be__%t_(%i)%");
		Graphics_text (g, x, y3, U"%\\be__%t+1_(%j)");
	}
}

void HMM_drawForwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xmin = 0.0, xmax = 1.0, ymin = 0.0, ymax = 1.0;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double xleft = 0.1, xright = 0.9, r = 0.03;
	long np = 6;
	double dy = (1.0 - 0.3) / (np - 1);
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
			double ddy = 3.0 * dy / 4.0;
			Graphics_fillCircle (g, x, y + dy - ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 2 * ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 3 * ddy, 0.5 * r);
		}
		y -= dy;
	}
	if (garnish) {
		double x1 = xleft - 1.5 * r, x2 = x1 + 0.2, y1 = 0.9;

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__1_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__1%j_");

		y1 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__2_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__2%j_");

		y1 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__%N_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%%Nj%_");

		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x0 + 1.5 * r, y0, U"%s__%j_");

		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x, 0.0, U"%t");
		Graphics_text (g, x0, 0.0, U"%t+1");

		double y3 = 0.10;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x, y3, U"%\\al__%t_(%i)%");
		Graphics_text (g, x0, y3, U"%\\al__%t+1_(%j)");
	}
}

void HMM_drawForwardAndBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	double xfrac = 0.1, xs = 1.0 / (0.5 - xfrac), r = 0.03;
	Graphics_Viewport vp = Graphics_insetViewport (g, 0.0, 0.5-xfrac, 0.0, 1.0);
	HMM_drawForwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_insetViewport (g, 0.5 + xfrac, 1.0, 0.0, 1.0);
	HMM_drawBackwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_setWindow (g, 0.0, xs, 0.0, 1.0);
	if (garnish) {
		double rx1 = 1.0 + xs * 2.0 * xfrac + 0.1, rx2 = rx1 + 0.9 - 0.1, y1 = 0.1;
		Graphics_line (g, 0.9 + r, 0.5, rx1 - r, 0.5);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.9, 0.5 + r, U"%s__%i_");
		Graphics_text (g, rx1, 0.5 + r, U"%s__%j_");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
		Graphics_text (g, 1.0 + xfrac * xs, 0.5, U"%a__%%ij%_%b__%j_(O__%t+1_)");
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.1, 0.0, U"%t-1");
		Graphics_text (g, 0.9, 0.0, U"%t");
		Graphics_text (g, rx1, 0.0, U"%t+1");
		Graphics_text (g, rx2, 0.0, U"%t+2");
		Graphics_setLineType (g, Graphics_DASHED);
		double x4 = rx1 - 0.06, x3 = 0.9 + 0.06;
		Graphics_line (g, x3, 0.7, x3, 0.0);
		Graphics_line (g, x4, 0.7, x4, 0.0);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_arrow (g, x4, y1, x4 + 0.2, y1);
		Graphics_arrow (g, x3, y1, x3 - 0.2, y1);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_BOTTOM);
		Graphics_text (g, x3 - 0.01, y1, U"\\al__%t_(i)");
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (g, x4 + 0.01, y1, U"\\be__%t+1_(j)");
	}
}

void HMM_and_HMMBaumWelch_getXi (HMM me, HMMBaumWelch thee, long *obs) {

	for (long it = 1; it <= thy numberOfTimes - 1; it++) {
		double sum = 0.0;
		for (long is = 1; is <= thy numberOfStates; is++) {
			for (long js = 1; js <= thy numberOfStates; js++) {
				thy xi[it][is][js] = thy alpha[is][it] * thy beta[js][it + 1] *
					my transitionProbs[is][js] * my emissionProbs[js][ obs[it + 1] ];
				sum += thy xi[it][is][js];
			}
		}
		for (long is = 1; is <= my numberOfStates; is++) {
			for (long js = 1; js <= my numberOfStates; js++) {
				thy xi[it][is][js] /= sum;
			}
		}
	}
}

void HMM_and_HMMBaumWelch_addEstimate (HMM me, HMMBaumWelch thee, long *obs) {
	for (long is = 1; is <= my numberOfStates; is ++) {
		// only for valid start states with p > 0
		if (my transitionProbs [0] [is] > 0.0) {
			thy aij_num [0] [is] += thy gamma [is] [1];
			thy aij_denom [0] [is] += 1.0;
		}
	}

	for (long is = 1; is <= my numberOfStates; is ++) {
		double gammasum = 0.0;
		for (long it = 1; it <= thy numberOfTimes - 1; it ++) {
			gammasum += thy gamma [is] [it];
		}

		for (long js = 1; js <= my numberOfStates; js ++) {
			double xisum = 0.0;
			for (long it = 1; it <= thy numberOfTimes - 1; it ++) {
				xisum += thy xi [it] [is] [js];
			}
			// zero probs signal invalid connections, don't reestimate
			if (my transitionProbs [is] [js] > 0.0) {
				thy aij_num [is] [js] += xisum;
				thy aij_denom [is] [js] += gammasum;
			}
		}

		/*
			Only reestimate the emissionProbs for a hidden markov model.
			A not hidden model is emulated with fixed emissionProbs.
		*/
		if (! my notHidden) {
			gammasum += thy gamma [is] [thy numberOfTimes];   // now sum all, add last term
			for (long k = 1; k <= my numberOfObservationSymbols; k ++) {
				double gammasum_k = 0.0;
				for (long it = 1; it <= thy numberOfTimes; it ++) {
					if (obs [it] == k) {
						gammasum_k += thy gamma [is] [it];
					}
				}
				// only reestimate probs > 0 !
				if (my emissionProbs [is] [k] > 0.0) {
					thy bik_num [is] [k] += gammasum_k;
					thy bik_denom [is] [k] += gammasum;
				}
			}
		}
		// For a left-to-right model the final state determines the transition prob to go to the END state
		if (my leftToRight) {
			thy aij_num [is] [my numberOfStates + 1] += thy gamma [is] [thy numberOfTimes];
			thy aij_denom [is] [my numberOfStates + 1] += 1.0;
		}
	}
}

void HMM_and_HMMBaumWelch_reestimate (HMM me, HMMBaumWelch thee) {
	double p;
	/*
		If we only have a couple of training sequences and they all happen to start with the same symbol,
		one or more starting probabilities can be zero.

		What to do with the P_k (see Rabiner formulas 109-110)?
	*/
	for (long is = 1; is <= my numberOfStates; is ++) {
		/*
			If we have not enough observation sequences it can happen that some probabilities
			become zero. This means that for some future observation sequences the probability evaluation
			returns  p=0 for sequences where these transitions turn up. This makes recognition impossible and also comparisons between models are difficult.
			We can prevent this from happening by asumimg a minimal probability for valid transitions
			i.e. which have initially p > 0.
		*/
		if (my transitionProbs [0] [is] > 0.0) {
			p = thy aij_num [0] [is] / thy aij_denom [0] [is];
			my transitionProbs [0] [is] = p > 0.0 ? p : thy minProb;
		}
		for (long js = 1; js <= my numberOfStates; js ++) {
			if (my transitionProbs [is] [js] > 0.0) {
				p = thy aij_num [is] [js] / thy aij_denom [is] [js];
				my transitionProbs [is] [js] = p > 0.0 ? p : thy minProb;
			}
		}
		if (! my notHidden) {
			for (long k = 1; k <= my numberOfObservationSymbols; k ++) {
				if (my emissionProbs [is] [k] > 0.0) {
					p = thy bik_num [is] [k] / thy bik_denom [is] [k];
					my emissionProbs [is] [k] = p > 0.0 ? p : thy minProb;
				}
			}
		}
		if (my leftToRight && my transitionProbs[is][my numberOfStates + 1] > 0.0) {
			p = thy aij_num[is][my numberOfStates + 1] / thy aij_denom[is][my numberOfStates + 1];
			my transitionProbs[is][my numberOfStates + 1] = p > 0.0 ? p : thy minProb;
		}
	}
}

void HMM_and_HMMBaumWelch_forward (HMM me, HMMBaumWelch thee, long *obs) {
	// initialise at t = 1 & scale
	thy scale [1] = 0.0;
	for (long js = 1; js <= my numberOfStates; js ++) {
		thy alpha [js] [1] = my transitionProbs [0] [js] * my emissionProbs [js] [obs [1]];
		thy scale [1] += thy alpha [js] [1];
	}
	for (long js = 1; js <= my numberOfStates; js ++) {
		thy alpha [js] [1] /= thy scale [1];
	}
	// recursion
	for (long it = 2; it <= thy numberOfTimes; it ++) {
		thy scale [it] = 0.0;
		for (long js = 1; js <= my numberOfStates; js ++) {
			double sum = 0.0;
			for (long is = 1; is <= my numberOfStates; is ++) {
				sum += thy alpha [is] [it - 1] * my transitionProbs [is] [js];
			}

			thy alpha [js] [it] = sum * my emissionProbs [js] [obs [it]];
			thy scale [it] += thy alpha [js] [it];
		}

		for (long js = 1; js <= my numberOfStates; js ++) {
			thy alpha [js] [it] /= thy scale [it];
		}
	}

	for (long it = 1; it <= thy numberOfTimes; it ++) {
		thy lnProb += log (thy scale [it]);
	}
}

void HMM_and_HMMBaumWelch_backward (HMM me, HMMBaumWelch thee, long *obs) {
	for (long is = 1; is <= my numberOfStates; is ++) {
		thy beta [is] [thy numberOfTimes] = 1.0 / thy scale [thy numberOfTimes];
	}
	for (long it = thy numberOfTimes - 1; it >= 1; it --) {
		for (long is = 1; is <= my numberOfStates; is ++) {
			double sum = 0.0;
			for (long js = 1; js <= my numberOfStates; js ++) {
				sum += thy beta [js] [it + 1] * my transitionProbs [is] [js] * my emissionProbs [js] [obs [it + 1]];
			}
			thy beta [is] [it] = sum / thy scale [it];
		}
	}
}

/*************************** HMM decoding ***********************************/

// precondition: valid symbols, i.e. 1 <= o[i] <= my numberOfSymbols for i=1..nt
void HMM_and_HMMViterbi_decode (HMM me, HMMViterbi thee, long *obs) {
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

autoHMMStateSequence HMM_and_HMMObservationSequence_to_HMMStateSequence (HMM me, HMMObservationSequence thee) {
	try {
		autoStringsIndex si = HMM_and_HMMObservationSequence_to_StringsIndex (me, thee);
		long *obs = si -> classIndex; // convenience
		long numberOfUnknowns = StringsIndex_countItems (si.get(), 0);

		if (numberOfUnknowns > 0) {
			Melder_throw (U"Unknown observation symbol(s) (# = ", numberOfUnknowns, U").");
		}

		long numberOfTimes = thy rows.size;
		autoHMMViterbi v = HMM_to_HMMViterbi (me, obs, numberOfTimes);
		autoHMMStateSequence him = HMMStateSequence_create (numberOfTimes);
		// trace the path and get states
		for (long it = 1; it <= numberOfTimes; it ++) {
			HMMState hmms = my states->at [v -> path [it]];
			his strings [it] = Melder_dup (hmms -> label);
			his numberOfStrings ++;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMStateSequence created.");
	}
}

double HMM_and_HMMStateSequence_getProbability (HMM me, HMMStateSequence thee) {
	autoStringsIndex si = HMM_and_HMMStateSequence_to_StringsIndex (me, thee);
	long numberOfUnknowns = StringsIndex_countItems (si.get(), 0);
	long *index = si -> classIndex;

	if (index == 0) {
		return undefined;
	}
	if (numberOfUnknowns > 0) {
		Melder_warning (U"Unknown states (# = ", numberOfUnknowns, U").");
		return undefined;
	}
	double p0 = my transitionProbs [0] [index [1]];
	if (p0 == 0) {
		Melder_throw (U"You cannot start with this state.");
	}
	double lnp = log (p0);
	for (long it = 2; it <= thy numberOfStrings; it ++) {
		lnp += log (my transitionProbs [index [it - 1]] [index [it]]);
	}
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInState (HMM me, long itime, long istate) {
	if (istate < 1 || istate > my numberOfStates) {
		return undefined;
	}

	autoNUMvector<double> scale (1, itime);
	autoNUMvector<double> alpha_t (1, my numberOfStates);
	autoNUMvector<double> alpha_tm1 (1, my numberOfStates);

	for (long js = 1; js <= my numberOfStates; js ++) {
		alpha_t [js] = my transitionProbs [0] [js];
		scale [1] += alpha_t [js];
	}
	for (long js = 1; js <= my numberOfStates; js ++) {
		alpha_t [js] /= scale [1];
	}
	// recursion
	for (long it = 2; it <= itime; it ++) {
		for (long js = 1; js <= my numberOfStates; js ++) {
			alpha_tm1 [js] = alpha_t [js];
		}

		for (long js = 1; js <= my numberOfStates; js ++) {
			double sum = 0.0;
			for (long is = 1; is <= my numberOfStates; is ++) {
				sum += alpha_tm1 [is] * my transitionProbs [is] [js];
			}
			alpha_t [js] = sum;
			scale [it] += alpha_t [js];
		}

		for (long js = 1; js <= my numberOfStates; js ++) {
			alpha_t [js] /= scale [it];
		}
	}

	double lnp = 0.0;
	for (long it = 1; it <= itime; it ++) {
		lnp += log (scale [it]);
	}

	lnp = alpha_t [istate] > 0 ? lnp + log (alpha_t [istate]) : -INFINITY; // p = 0 -> ln(p)=-infinity  // ppgb FIXME infinity is een laag getal
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, long itime, long istate, long isymbol) {
	// for a notHidden model emissionProbs may be zero!
	if (isymbol < 1 || isymbol > my numberOfObservationSymbols || my emissionProbs[istate][isymbol] == 0) {
		return undefined;
	}
	double lnp = HMM_getProbabilityAtTimeBeingInState (me, itime, istate);
	return ( isundef (lnp) ? undefined : lnp + log (my emissionProbs [istate] [isymbol]) );
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
		Melder_throw (U"The observation sequence starts with a symbol which state has starting probability zero.");
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

double HMM_and_HMMObservationSequence_getProbability (HMM me, HMMObservationSequence thee) {
	autoStringsIndex si = HMM_and_HMMObservationSequence_to_StringsIndex (me, thee);
	long *index = si -> classIndex;
	long numberOfUnknowns = StringsIndex_countItems (si.get(), 0);
	if (numberOfUnknowns > 0) {
		Melder_throw (U"Unknown observations (# = ", numberOfUnknowns, U").");
	}
	return HMM_getProbabilityOfObservations (me, index, thy rows.size);
}

double HMM_and_HMMObservationSequence_getCrossEntropy (HMM me, HMMObservationSequence thee) {
	double lnp = HMM_and_HMMObservationSequence_getProbability (me, thee);
	return isundef (lnp) ? undefined :
	        -lnp / (NUMln10 * HMMObservationSequence_getNumberOfObservations (thee));
}

double HMM_and_HMMObservationSequence_getPerplexity (HMM me, HMMObservationSequence thee) {
	double ce = HMM_and_HMMObservationSequence_getCrossEntropy (me, thee);
	return isundef (ce) ? undefined : pow (2.0, ce);
}

autoHMM HMM_createFromHMMObservationSequence (HMMObservationSequence me, long numberOfStates, int leftToRight) {
	try {
		autoHMM thee = Thing_new (HMM);
		autoStrings s = HMMObservationSequence_to_Strings (me);
		autoDistributions d = Strings_to_Distributions (s.get());

		long numberOfObservationSymbols = d -> numberOfRows;
		thy notHidden = numberOfStates < 1;
		numberOfStates = numberOfStates > 0 ? numberOfStates : numberOfObservationSymbols;

		HMM_init (thee.get(), numberOfStates, numberOfObservationSymbols, leftToRight);

		for (long i = 1; i <= numberOfObservationSymbols; i++) {
			const char32 *label = d -> rowLabels[i];
			autoHMMObservation hmmo = HMMObservation_create (label, 0, 0, 0);
			HMM_addObservation_move (thee.get(), hmmo.move());
			if (thy notHidden) {
				autoHMMState hmms = HMMState_create (label);
				HMM_addState_move (thee.get(), hmms.move());
			}
		}
		if (! thy notHidden) {
			HMM_setDefaultStates (thee.get());
		}
		HMM_setDefaultTransitionProbs (thee.get());
		HMM_setDefaultStartProbs (thee.get());
		HMM_setDefaultEmissionProbs (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMM created.");
	}
}

autoTableOfReal HMMObservationSequence_to_TableOfReal_transitions (HMMObservationSequence me, int probabilities) {
	try {
		autoStrings thee = HMMObservationSequence_to_Strings (me);
		autoTableOfReal him = Strings_to_TableOfReal_transitions (thee.get(), probabilities);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no transitions created.");
	}
}

autoStringsIndex HMM_and_HMMObservationSequence_to_StringsIndex (HMM me, HMMObservationSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = NUMvector<char32 *> (1, my numberOfObservationSymbols);
		for (long is = 1; is <= my numberOfObservationSymbols; is ++) {
			HMMObservation hmmo = my observationSymbols->at [is];
			classes -> strings [is] = Melder_dup (hmmo -> label);
			classes -> numberOfStrings ++;
		}
		autoStrings obs = HMMObservationSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (obs.get(), classes.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoStringsIndex HMM_and_HMMStateSequence_to_StringsIndex (HMM me, HMMStateSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = NUMvector<char32 *> (1, my numberOfObservationSymbols);
		for (long is = 1; is <= my numberOfStates; is ++) {
			HMMState hmms = my states->at [is];
			classes -> strings [is] = Melder_dup (hmms -> label);
			classes -> numberOfStrings ++;
		}
		autoStrings sts = HMMStateSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (sts.get(), classes.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoTableOfReal HMM_and_HMMObservationSequence_to_TableOfReal_transitions (HMM me, HMMObservationSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_and_HMMObservationSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.get(), probabilities);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created for HMMObservationSequence.");
	}
}

autoTableOfReal HMM_and_HMMStateSequence_to_TableOfReal_transitions (HMM me, HMMStateSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_and_HMMStateSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.get(), probabilities);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created for HMMStateSequence.");
	}
}

autoTableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities) {
	try {
		long numberOfTypes = my classes->size;

		autoTableOfReal thee = TableOfReal_create (numberOfTypes + 1, numberOfTypes + 1);
		for (long i = 1; i <= numberOfTypes; i ++) {
			SimpleString s = (SimpleString) my classes->at [i];
			TableOfReal_setRowLabel (thee.get(), i, s -> string);
			TableOfReal_setColumnLabel (thee.get(), i, s -> string);
		}
		for (long i = 2; i <= my numberOfItems; i ++) {
			if (my classIndex [i - 1] > 0 && my classIndex [i] > 0) { // a zero is a restart!
				thy data [my classIndex [i-1]] [my classIndex [i]] ++;
			}
		}
		double sum = 0.0;
		for (long i = 1; i <= numberOfTypes; i ++) {
			double rowSum = 0.0, colSum = 0.0;
			for (long j = 1; j <= numberOfTypes; j ++) {
				rowSum += thy data [i] [j];
			}
			thy data [i] [numberOfTypes + 1] = rowSum;
			for (long j = 1; j <= numberOfTypes; j ++) {
				colSum += thy data [j] [i];
			}
			thy data[numberOfTypes + 1][i] = colSum;
			sum += colSum;
		}
		thy data [numberOfTypes + 1] [numberOfTypes + 1] = sum;
		if (probabilities && sum > 0) {
			for (long i = 1; i <= numberOfTypes; i ++) {
				if (thy data [i] [numberOfTypes + 1] > 0.0) {
					for (long j = 1; j <= numberOfTypes; j++) {
						thy data [i] [j] /= thy data [i] [numberOfTypes + 1];
					}
				}
			}
			for (long i = 1; i <= numberOfTypes; i ++) {
				thy data [i] [numberOfTypes + 1] /= sum;
				thy data [numberOfTypes + 1] [i] /= sum;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created.");
	}
}

autoTableOfReal Strings_to_TableOfReal_transitions (Strings me, int probabilities) {
	try {
		autoStringsIndex him = Strings_to_StringsIndex (me);
		autoTableOfReal thee = StringsIndex_to_TableOfReal_transitions (him.get(), probabilities);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created.");
	}
}

/* End of file HMM.cpp */
