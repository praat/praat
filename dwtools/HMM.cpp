/* HMM.cpp
 *
 * Copyright (C) 2010-2020 David Weenink, 2015,2017 Paul Boersma
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


#include "enums_getText.h"
#include "HMM_enums.h"
#include "enums_getValue.h"
#include "HMM_enums.h"

Thing_implement (HMM, Daata, 2);
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
autoHMMObservation HMMObservation_create (conststring32 label, integer numberOfComponents, integer dimension, kHMMstorage storage);

integer HMM_HMMObservationSequence_getLongestSequence (HMM me, HMMObservationSequence thee, integer symbolNumber);
integer StringsIndex_getLongestSequence (StringsIndex me, integer index, integer *out_pos);
integer Strings_getLongestSequence (Strings me, char32 *string, integer *out_pos);
autoHMMState HMMState_create (conststring32 label);

autoHMMBaumWelch HMMBaumWelch_create (integer nstates, integer nsymbols, integer capacity);
void HMMBaumWelch_getGamma (HMMBaumWelch me);
autoHMMBaumWelch HMM_forward (HMM me, constINTVEC obs);
void HMMBaumWelch_reInit (HMMBaumWelch me);
void HMM_HMMBaumWelch_getXi (HMM me, HMMBaumWelch thee, constINTVEC obs);
void HMM_HMMBaumWelch_reestimate (HMM me, HMMBaumWelch thee);
void HMM_HMMBaumWelch_addEstimate (HMM me, HMMBaumWelch thee, constINTVEC obs);
void HMM_HMMBaumWelch_forward (HMM me, HMMBaumWelch thee, constINTVEC obs);
void HMM_HMMBaumWelch_backward (HMM me, HMMBaumWelch thee, constINTVEC obs);
void HMM_HMMViterbi_decode (HMM me, HMMViterbi thee, constINTVEC obs);
double HMM_getProbabilityOfObservations (HMM me, constINTVEC obs);
autoTableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities);
autoStringsIndex HMM_HMMStateSequence_to_StringsIndex (HMM me, HMMStateSequence thee);


autoHMMViterbi HMMViterbi_create (integer nstates, integer ntimes);

// evaluate the numbers given to probabilities
static autoVEC NUMwstring_to_probs (conststring32 s, integer nwanted) {
	autoVEC numbers = newVECfromString (s);
	if (numbers.size != nwanted)
		Melder_throw (U"You supplied ", numbers.size, U", while ", nwanted, U" numbers needed.");
	longdouble sum = 0.0;
	for (integer i = 1; i <= numbers.size; i ++) {
		if (numbers [i] < 0.0)
			Melder_throw (U"Numbers have to be positive.");
		sum += numbers [i];
	}
	Melder_require (sum > 0.0,
		U"All probabilities cannot be zero.");
	for (integer i = 1; i <= numbers.size; i ++)
		numbers [i] /= sum;
	return numbers;
}

#if 0
static integer NUMget_line_intersection_with_circle (double xc, double yc, double r, double a, double b, double *out_x1, double *out_y1, double *out_x2, double *out_y2) {
	const double ca = a * a + 1.0, bmyc = (b - yc);
	const double cb = 2.0 * (a * bmyc - xc);
	const double cc = bmyc * bmyc + xc * xc - r * r;
	double x1, x2, y1, y2;
	const integer nroots = NUMsolveQuadraticEquation (ca, cb, cc, & x1, & x2);
	if (nroots == 1) {
		y1 = a * x1 + b;
		x2 = x1;
		y2 = y1;
	} else if (nroots == 2) {
		if (x1 > x2)
			std::swap (x1, x2);
		y1 = x1 * a + b;
		y2 = x2 * a + b;
	}
	if (out_x1)
		*out_x1 = x1;
	if (out_x2)
		*out_x2 = x2;
	if (out_y1)
		*out_y1 = y1;
	if (out_y2)
		*out_y2 = y2;
	return nroots;
}
#endif

// D(l_1,l_2)=1/n( log p(O_2|l_1) - log p(O_2|l_2)
static double HMM_HMM_getCrossEntropy_asym (HMM me, HMM thee, integer observationLength) {
	autoHMMObservationSequence os = HMM_to_HMMObservationSequence (thee, 0, observationLength);
	const double ce = HMM_HMMObservationSequence_getCrossEntropy (me, os.get());
	if (isundef (ce))
		return ce;
	const double ce2 = HMM_HMMObservationSequence_getCrossEntropy (thee, os.get());
	if (isundef (ce2))
		return ce2;
	return ce - ce2;
}

/**************** HMMObservation ******************************/

static void HMMObservation_init (HMMObservation me, conststring32 label, integer numberOfComponents, integer dimension, kHMMstorage storage) {
	my label = Melder_dup (label);
	my gm = GaussianMixture_create (numberOfComponents, dimension, storage == kHMMstorage::DIAGONALS ?
		kGaussianMixtureStorage::DIAGONALS : kGaussianMixtureStorage::COMPLETE);
}

autoHMMObservation HMMObservation_create (conststring32 label, integer numberOfComponents, integer dimension, kHMMstorage storage) {
	try {
		autoHMMObservation me = Thing_new (HMMObservation);
		HMMObservation_init (me.get(), label, numberOfComponents, dimension, storage);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMObservation not created.");
	}
}

integer Strings_getLongestSequence (Strings me, char32 *string, integer *out_pos) {
	integer length = 0, longest = 0, lpos = 0, pos = 0;
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		if (Melder_equ (my strings [i].get(), string)) {
			if (length == 0) {
				lpos = i;
			}
			length ++;
		} else {
			if (length > 0) {
				if (length > longest) {
					longest = length;
					pos = lpos;
				}
				length = 0;
			}
		}
	}
	if (out_pos)
		*out_pos = pos;
	return length;
}

integer StringsIndex_getLongestSequence (StringsIndex me, integer index, integer *out_pos) {
	integer length = 0, longest = 0, lpos = 0, pos = 0;
	for (integer i = 1; i <= my numberOfItems; i ++) {
		if (my classIndex [i] == index) {
			if (length == 0)
				lpos = i;
			length ++;
		} else {
			if (length > 0) {
				if (length > longest) {
					longest = length;
					pos = lpos;
				}
				length = 0;
			}
		}
	}
	if (out_pos)
		*out_pos = pos;
	return length;
}

/**************** HMMState ******************************/

static void HMMState_init (HMMState me, conststring32 label) {
	my label = Melder_dup (label);
}

autoHMMState HMMState_create (conststring32 label) {
	try {
		autoHMMState me = Thing_new (HMMState);
		HMMState_init (me.get(), label);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMState not created.");
	}
}

void HMMState_setLabel (HMMState me, char32 *label) {
	my label = Melder_dup (label);
}

/**************** HMMBaumWelch ******************************/

autoHMMBaumWelch HMMBaumWelch_create (integer nstates, integer nsymbols, integer capacity) {
	try {
		autoHMMBaumWelch me = Thing_new (HMMBaumWelch);
		my numberOfTimes = my capacity = capacity;
		my numberOfStates = nstates;
		my numberOfSymbols = nsymbols;
		my alpha = zero_MAT (nstates, capacity);
		my beta = zero_MAT (nstates, capacity);
		my scale = zero_VEC (capacity);
		my xi = zero_TEN3 (capacity, nstates, nstates); // TEN3
		my aij_num_p0 = zero_VEC (nstates + 1);
		my aij_num = zero_MAT (nstates, nstates + 1);
		my aij_denom_p0 = zero_VEC (nstates + 1);
		my aij_denom =  zero_MAT (nstates, nstates + 1);
		my bik_num = zero_MAT (nstates, nsymbols);
		my bik_denom = zero_MAT (nstates, nsymbols);
		my gamma = zero_MAT (nstates, capacity);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMBaumWelch not created.");
	}
}

void HMMBaumWelch_getGamma (HMMBaumWelch me) {
	for (integer it = 1; it <= my numberOfTimes; it ++) {
		my gamma.column (it) <<= my alpha.column (it)  *  my beta.column (it);
		my gamma.column (it)  /=  NUMsum (my gamma.column (it));
	}
}

/**************** HMMViterbi ******************************/

autoHMMViterbi HMMViterbi_create (integer nstates, integer ntimes) {
	try {
		autoHMMViterbi me = Thing_new (HMMViterbi);
		my numberOfTimes = ntimes;
		my numberOfStates = nstates;
		my viterbi = zero_MAT (nstates, ntimes);
		my bp = zero_INTMAT (nstates, ntimes);
		my path = zero_INTVEC (ntimes);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMViterbi not created.");
	}
}

/******************* HMMObservationSequence & HMMStateSequence ***/

autoHMMObservationSequence HMMObservationSequence_create (integer numberOfItems, integer dataLength) {
	try {
		autoHMMObservationSequence me = Thing_new (HMMObservationSequence);
		Table_initWithoutColumnNames (me.get(), numberOfItems, dataLength + 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMMObservationSequence not created.");
	}
}

integer HMMObservationSequence_getNumberOfObservations (HMMObservationSequence me) {
	return my rows.size;
}

void HMMObservationSequence_removeObservation (HMMObservationSequence me, integer index) {
	Table_removeRow (me, index);
}

autoStrings HMMObservationSequence_to_Strings (HMMObservationSequence me) {
	try {
		const integer numberOfStrings = my rows.size;
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (numberOfStrings);
		for (integer i = 1; i <= numberOfStrings; i ++)
			thy strings [i] = Melder_dup (Table_getStringValue_Assert (me, i, 1));
		thy numberOfStrings = numberOfStrings;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}

autoHMMObservationSequence Strings_to_HMMObservationSequence (Strings me) {
	try {
		autoHMMObservationSequence thee = HMMObservationSequence_create (my numberOfStrings, 0);
		for (integer i = 1; i <= my numberOfStrings; i ++)
			Table_setStringValue (thee.get(), i, 1, my strings [i].get());
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

integer HMM_HMMObservationSequence_getLongestSequence (HMM me, HMMObservationSequence thee, integer symbolNumber) {
	autoStringsIndex si = HMM_HMMObservationSequence_to_StringsIndex (me, thee);
	// TODO
	(void) symbolNumber;
	return 1;
}

integer HMMObservationSequenceBag_getLongestSequence (HMMObservationSequenceBag me) {
	integer longest = 0;
	for (integer i = 1; i <= my size; i ++) {
		HMMObservationSequence thee = my at [i];
		if (thy rows.size > longest)
			longest = thy rows.size;
	}
	return longest;
}

autoHMMStateSequence HMMStateSequence_create (integer numberOfItems) {
	try {
		autoHMMStateSequence me = Thing_new (HMMStateSequence);
		my strings = autoSTRVEC (numberOfItems);
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
	for (integer i = 1; i <= numberOfStates; i ++) {
		const HMMState hmms = our states->at [i];
		MelderInfo_writeLine (U"  ", hmms -> label.get());
	}
	MelderInfo_writeLine (U"Number of symbols: ", numberOfObservationSymbols);
	for (integer i = 1; i <= numberOfObservationSymbols; i ++) {
		const HMMObservation hmms = our observationSymbols->at [i];
		MelderInfo_writeLine (U"  ", hmms -> label.get());
	}
}

static void HMM_init (HMM me, integer numberOfStates, integer numberOfObservationSymbols, int leftToRight) {
	my numberOfStates = numberOfStates;
	my numberOfObservationSymbols = numberOfObservationSymbols;
	my componentStorage = kHMMstorage::DIAGONALS;
	my leftToRight = leftToRight;
	my states = HMMStateList_create ();
	my observationSymbols = HMMObservationList_create ();
	my initialStateProbs = zero_VEC (numberOfStates);
	my transitionProbs = zero_MAT (numberOfStates, numberOfStates + 1);
	my emissionProbs = zero_MAT (numberOfStates, numberOfObservationSymbols);
	HMM_setDefaultInitialStateProbs (me);
	HMM_setDefaultTransitionProbs (me);
	HMM_setDefaultEmissionProbs (me);
}

autoHMM HMM_create (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols) {
	try {
		autoHMM me = Thing_new (HMM);
		HMM_init (me.get(), numberOfStates, numberOfObservationSymbols, leftToRight);
		HMM_setDefaultStates (me.get());
		HMM_setDefaultObservations (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"HMM not created.");
	}
}

void HMM_setDefaultStates (HMM me) {
	for (integer i = 1; i <= my numberOfStates; i ++) {
		autoHMMState hmms = HMMState_create (Melder_cat (U"S", i));
		HMM_addState_move (me, hmms.move());
	}
}

autoHMM HMM_createFullContinuousModel (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols, integer numberOfFeatureStreams, integer *dimensionOfStream, integer *numberOfGaussiansforStream) {
	(void) leftToRight;
	(void) numberOfStates;
	(void) numberOfObservationSymbols;
	(void) numberOfFeatureStreams;
	(void) dimensionOfStream;
	(void) numberOfGaussiansforStream;
	return autoHMM();
}

autoHMM HMM_createContinuousModel (int leftToRight, integer numberOfStates, integer numberOfObservationSymbols, integer numberOfMixtureComponentsPerSymbol, integer componentDimension, kHMMstorage componentStorage) {
	try {
		autoHMM me = Thing_new (HMM);
		HMM_init (me.get(), numberOfStates, numberOfObservationSymbols, leftToRight);
		my numberOfMixtureComponents = numberOfMixtureComponentsPerSymbol;
		my componentDimension = componentDimension;
		my componentStorage = componentStorage;
		for (integer i = 1; i <= numberOfStates; i ++) {
			autoHMMState state = HMMState_create (Melder_cat (U"S", i));
			HMM_addState_move (me.get(), state.move());
		}
		for (integer j = 1; j <= numberOfObservationSymbols; j ++) {
			autoHMMObservation obs = HMMObservation_create (Melder_cat (U"s", j), numberOfMixtureComponentsPerSymbol, componentDimension, componentStorage);
			HMM_addObservation_move (me.get(), obs.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Continuous model HMM not created.");
	}
}

// for a simple non-hidden model leave either states empty or symbols empty !!!
autoHMM HMM_createSimple (int leftToRight, conststring32 states_string, conststring32 symbols_string) {
	try {
		autoSTRVEC states = splitByWhitespace_STRVEC (states_string);
		autoSTRVEC symbols = splitByWhitespace_STRVEC (symbols_string);
		autoHMM me = Thing_new (HMM);

		Melder_require (states.size > 0 || symbols.size > 0,
			U"The states and symbols should not both be empty.");
		
		if (symbols.size == 0) {
			symbols = newSTRVECcopy (states.get());
			my notHidden = 1;
		}
		if (states.size == 0) {
			states = newSTRVECcopy (symbols.get());
			my notHidden = 1;
		}

		HMM_init (me.get(), states.size, symbols.size, leftToRight);

		for (integer istate = 1; istate <= states.size; istate ++) {
			autoHMMState state = HMMState_create (states [istate].get());
			HMM_addState_move (me.get(), state.move());
		}
		for (integer isymbol = 1; isymbol <= symbols.size; isymbol ++) {
			autoHMMObservation symbol = HMMObservation_create (symbols [isymbol].get(), 0, 0, kHMMstorage::DIAGONALS);
			HMM_addObservation_move (me.get(), symbol.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple HMM not created.");
	}
}

void HMM_setDefaultObservations (HMM me) {
	const conststring32 def = ( my notHidden ? U"S" : U"s" );
	for (integer i = 1; i <= my numberOfObservationSymbols; i ++) {
		autoHMMObservation hmms = HMMObservation_create (Melder_cat (def, i), 0, 0, kHMMstorage::DIAGONALS);
		HMM_addObservation_move (me, hmms.move());
	}
}

void HMM_setDefaultTransitionProbs (HMM me) {
	if (my leftToRight) {
		for (integer irow = 1; irow <= my numberOfStates; irow ++) {
			const double p = 1.0 / (my numberOfStates - irow + 1.0);
			my transitionProbs.row (irow).part (irow, my numberOfStates) <<= p;
		}
		// leftToRight must have end state!
		my transitionProbs [my numberOfStates] [my numberOfStates] =
			my transitionProbs [my numberOfStates] [my numberOfStates + 1] = 0.5;
	} else {
		my transitionProbs.part (1, my numberOfStates, 1, my numberOfStates) <<= 1.0 / my numberOfStates;
		my transitionProbs.column (my numberOfStates + 1) <<= 0.0;
	}
}

void HMM_setDefaultInitialStateProbs (HMM me) {
	my initialStateProbs.get () <<= 1.0 / my numberOfStates;
}

void HMM_setDefaultEmissionProbs (HMM me) {
	if (my notHidden) {
		my emissionProbs.get () <<= 0.0;
		my emissionProbs.diagonal () <<= 1.0;
	} else 
		my emissionProbs.part (1, my numberOfStates, 1, my numberOfObservationSymbols) <<= 1.0 / my numberOfObservationSymbols;
}

void HMM_setDefaultMixingProbabilities (HMM me) {
	const double mp = 1.0 / my numberOfMixtureComponents;
	for (integer is = 1; is <= my numberOfObservationSymbols; is ++) {
		HMMObservation hmmo = my observationSymbols->at [is];
		hmmo -> gm -> mixingProbabilities.all()  <<=  mp;
	}
}

void HMM_setStartProbabilities (HMM me, conststring32 probs) {
	try {
		autoVEC p = NUMwstring_to_probs (probs, my numberOfStates);
		my initialStateProbs.get () <<= p.get();
	} catch (MelderError) {
		Melder_throw (me, U": no start probabilities set.");
	}
}

void HMM_setTransitionProbabilities (HMM me, integer state_number, conststring32 state_probs) {
	try {
		Melder_require (state_number <= my states->size,
			U"State number should not exceed ", my states->size, U".");
		autoVEC p = NUMwstring_to_probs (state_probs, my numberOfStates);
		my transitionProbs.row (state_number).part (1, my numberOfStates) <<= p.get ();
	} catch (MelderError) {
		Melder_throw (me, U": no transition probabilities set.");
	}
}

void HMM_setEmissionProbabilities (HMM me, integer state_number, conststring32 emission_probs) {
	try {
		Melder_require (state_number <= my states->size,
			U"State number should not exceed ", my states->size, U".");
		Melder_require (! my notHidden,
			U"The emission probabilities of this model are fixed.");
		autoVEC p = NUMwstring_to_probs (emission_probs, my numberOfObservationSymbols);
		my emissionProbs.row (state_number).part (1, my numberOfObservationSymbols) <<= p.get ();
	} catch (MelderError) {
		Melder_throw (me, U": no emission probabilities set.");
	}

}

void HMM_addObservation_move (HMM me, autoHMMObservation thee) {
	const integer ns = my observationSymbols->size + 1;
	Melder_require (ns <= my numberOfObservationSymbols, U"Observation list is full.");
	my observationSymbols -> addItemAtPosition_move (thee.move(), ns);
}

void HMM_addState_move (HMM me, autoHMMState thee) {
	const integer ns = my states -> size + 1;
	Melder_require (ns <= my numberOfStates, U"States list is full.");
	my states -> addItemAtPosition_move (thee.move(), ns);
}

autoTableOfReal HMM_extractTransitionProbabilities (HMM me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfStates + 1, my numberOfStates + 1);
		thy data.row (1).part (1, my numberOfStates) <<= my initialStateProbs.get();
		for (integer is = 1; is <= my numberOfStates; is ++) {
			const HMMState hmms = my states->at [is];
			TableOfReal_setRowLabel (thee.get(), is + 1, hmms -> label.get());
			TableOfReal_setColumnLabel (thee.get(), is, hmms -> label.get());
			thy data.row (is + 1).part (1, my numberOfStates) <<= my transitionProbs.row (is).part (1, my numberOfStates);
		}
		TableOfReal_setRowLabel (thee.get(), 1, U"START");
		TableOfReal_setColumnLabel (thee.get(), my numberOfStates + 1, U"END");
		//thy data.column (my numberOfStates + 1).part (2, my numberOfStates) <<= my transitionProbs.column (my numberOfStates + 1 );
		for (integer is = 1; is <= my numberOfStates; is ++) {
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
		for (integer js = 1; js <= my numberOfObservationSymbols; js ++) {
			const HMMObservation hmms = my observationSymbols->at [js];
			TableOfReal_setColumnLabel (thee.get(), js, hmms -> label.get());
		}
		for (integer is = 1; is <= my numberOfStates; is ++) {
			const HMMState hmms = my states->at [is];
			TableOfReal_setRowLabel (thee.get(), is, hmms -> label.get());
			for (integer js = 1; js <= my numberOfObservationSymbols; js ++)
				thy data [is] [js] = my emissionProbs [is] [js];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no emission probabilities extracted.");
	};
}

double HMM_getExpectedValueOfDurationInState (HMM me, integer istate) {
	if (istate < 0 || istate > my numberOfStates)
		return undefined;
	return 1.0 / (1.0 - my transitionProbs [istate] [istate]);
}

double HMM_getProbabilityOfStayingInState (HMM me, integer istate, integer numberOfTimeUnits) {
	if (istate < 0 || istate > my numberOfStates)
		return undefined;
	return pow (my transitionProbs [istate] [istate], numberOfTimeUnits - 1.0) * (1.0 - my transitionProbs [istate] [istate]);
}

double HMM_HMM_getCrossEntropy (HMM me, HMM thee, integer observationLength, int symmetric) {
	const double ce1 = HMM_HMM_getCrossEntropy_asym (me, thee, observationLength);
	if (! symmetric || isundef (ce1))
		return ce1;
	const double ce2 = HMM_HMM_getCrossEntropy_asym (thee, me, observationLength);
	if (isundef (ce2))
		return ce2;
	return (ce1 + ce2) / 2.0;
}

double HMM_HMM_HMMObservationSequence_getCrossEntropy (HMM me, HMM thee, HMMObservationSequence him) {
	const double ce1 = HMM_HMMObservationSequence_getCrossEntropy (me, him);
	if (isundef (ce1))
		return ce1;
	const double ce2 = HMM_HMMObservationSequence_getCrossEntropy (thee, him);
	if (isundef (ce2))
		return ce2;
	return (ce1 + ce2) / 2.0;
}

void HMM_draw (HMM me, Graphics g, bool garnish) {
	const double xwidth = sqrt (my numberOfStates);
	const double rstate = 0.3 / xwidth, r = xwidth / 3.0;
	const double xmax = 1.2 * xwidth / 2.0, xmin = -xmax, ymin = xmin, ymax = xmax;

	autoVEC xs = raw_VEC (my numberOfStates);
	autoVEC ys = raw_VEC (my numberOfStates);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	// heuristic: all states on a circle until we have a better graph drawing algorithm.
	xs [1] = ys [1] = 0.0;
	if (my numberOfStates > 1) {
		for (integer is = 1; is <= my numberOfStates; is ++) {
			const double alpha = - NUMpi + NUMpi * 2.0 * (is - 1) / my numberOfStates;
			xs [is] = r * cos (alpha); ys [is] = r * sin (alpha);
		}
	}
	// reorder the positions such that state number 1 is most left and last state number is right.
	// if > 5 may be one in the middle with the most connections
	// ...
	// find fontsize
	const double fontSize = Graphics_inqFontSize (g);
	conststring32 widest_label = U"";
	double max_width = 0.0;
	for (integer is = 1; is <= my numberOfStates; is ++) {
		const HMMState hmms = my states->at [is];
		const double w = ( hmms -> label ? Graphics_textWidth (g, hmms -> label.get()) : 0.0 );
		if (w > max_width) {
			widest_label = hmms -> label.get();
			max_width = w;
		}
	}
	double new_fontSize = fontSize;
	while (max_width > 2.0 * rstate && new_fontSize > 4) {
		new_fontSize --;
		Graphics_setFontSize (g, new_fontSize);
		max_width = Graphics_textWidth (g, widest_label);
	}
	Graphics_setFontSize (g, new_fontSize);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	for (integer is = 1; is <= my numberOfStates; is ++) {
		const HMMState hmms = my states->at [is];
		Graphics_circle (g, xs [is], ys [is], rstate);
		Graphics_text (g, xs [is], ys [is], hmms -> label.get());
	}

	// draw connections from is to js
	// 1 -> 2 / 2-> : increase / decrease angle between 1 and 2 with pi /10
	// use cos(a+b) and cos(a -b) rules
	const double cosb = cos (NUMpi / 10.0), sinb = sin (NUMpi / 10.0);
	for (integer is = 1; is <= my numberOfStates; is ++) {
		const double x1 = xs [is], y1 = ys [is];
		for (integer js = 1; js <= my numberOfStates; js ++) {
			if (my transitionProbs [is] [js] > 0.0 && is != js) {
				const double x2 = xs [js], y2 = ys [js];
				const double dx = x2 - x1, dy = y2 - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				const double cosabp = cosa * cosb - sina * sinb, cosabm = cosa * cosb + sina * sinb;
				const double sinabp = cosa * sinb + sina * cosb, sinabm = -cosa * sinb + sina * cosb;
				Graphics_arrow (g, x1 + rstate * cosabp, y1 + rstate * sinabp, x2 - rstate * cosabm, y2 - rstate * sinabm);
			}
			if (is == js) {
				const double dx = - x1, dy = - y1, h = sqrt (dx * dx + dy * dy), cosa = dx / h, sina = dy / h;
				Graphics_doubleArrow (g, x1 - rstate * cosa, y1 - rstate * sina, x1 - 1.4 * rstate * cosa, y1 - 1.4 * rstate * sina);
			}
		}
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
	}
}

void HMM_unExpandPCA (HMM me) {
	if (my componentDimension <= 0)
		return;    // nothing to do
	for (integer is = 1; is <= my numberOfObservationSymbols; is ++) {
		const HMMObservation s = my observationSymbols->at [is];
		GaussianMixture_unExpandPCA (s -> gm.get());
	}
}

autoHMMObservationSequence HMM_to_HMMObservationSequence (HMM me, integer startState, integer numberOfItems) {
	try {
		autoHMMObservationSequence thee = HMMObservationSequence_create (numberOfItems, my componentDimension);
		integer istate = ( startState == 0 ? NUMgetIndexFromProbability (my initialStateProbs.get(), NUMrandomUniform (0.0, 1.0)) : startState );
		if (my componentDimension > 0) {
			autoVEC obs = raw_VEC (my componentDimension);
			autoVEC buf = raw_VEC (my componentDimension);
			for (integer i = 1; i <= numberOfItems; i ++) {
				// Emit a symbol from istate

				const integer isymbol = NUMgetIndexFromProbability (my emissionProbs.row (istate), NUMrandomUniform (0.0, 1.0));
				const HMMObservation s = my observationSymbols->at [isymbol];
				GaussianMixture_generateOneVector_inline (s -> gm.get(), obs.get(), nullptr, buf.get());
				for (integer j = 1; j <= my componentDimension; j ++)
					Table_setNumericValue (thee.get(), i, 1 + j, obs [j]);

				Table_setStringValue (thee.get(), i, 1, s -> label.get());

				// get next state

				istate = NUMgetIndexFromProbability (my transitionProbs.row (istate).part(1, my numberOfStates), NUMrandomUniform (0.0, 1.0));
				if (istate == my numberOfStates + 1) { // final state
					for (integer j = numberOfItems; j > i; j --)
						HMMObservationSequence_removeObservation (thee.get(), j);
					break;
				}
			}
		} else {
			for (integer i = 1; i <= numberOfItems; i ++) {
				// Emit a symbol from istate

				const integer isymbol = NUMgetIndexFromProbability (my emissionProbs.row (istate), NUMrandomUniform (0.0, 1.0));
				HMMObservation s = my observationSymbols->at [isymbol];

				Table_setStringValue (thee.get(), i, 1, s -> label.get());

				// get next state

				istate = NUMgetIndexFromProbability (my transitionProbs.row (istate).part (1, my numberOfStates), NUMrandomUniform (0.0, 1.0));
				if (istate == my numberOfStates + 1) { // final state
					for (integer j = numberOfItems; j > i; j --)
						HMMObservationSequence_removeObservation (thee.get(), j);
					break;
				}
			}
		}

		HMM_unExpandPCA (me);
		return thee;
	} catch (MelderError) {
		HMM_unExpandPCA (me);
		Melder_throw (me, U":no HMMObservationSequence created.");
	}
}

autoHMMBaumWelch HMM_forward (HMM me, constINTVEC obs) {
	try {
		autoHMMBaumWelch thee = HMMBaumWelch_create (my numberOfStates, my numberOfObservationSymbols, obs.size);
		HMM_HMMBaumWelch_forward (me, thee.get(), obs);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMBaumWelch created.");
	}
}

static autoHMMViterbi HMM_to_HMMViterbi (HMM me, constINTVEC obs) {
	try {
		autoHMMViterbi thee = HMMViterbi_create (my numberOfStates, obs.size);
		HMM_HMMViterbi_decode (me, thee.get(), obs);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMViterbi created.");
	}
}

void HMMBaumWelch_reInit (HMMBaumWelch me) {
	my totalNumberOfSequences = 0;
	my lnProb = 0.0;

	/*
		The _num and _denum matrices are asigned as += in the iteration loop and therefore need to be zeroed
		at the start of each new iteration.
		The elements of alpha, beta, scale, gamma & xi are always calculated directly and need not be
		initialised.
	*/
	my aij_num_p0.all()  <<=  0.0;
	my aij_num.all()  <<=  0.0;
	my aij_denom_p0.all()  <<=  0.0;
	my aij_denom.all()  <<=  0.0;
	my bik_num.all()  <<=  0.0;
	my bik_denom.all()  <<=  0.0;
}

static integer HMM_getState_notHidden (HMM me, conststring32 stateLabel) {
	for (integer istate = 1; istate <= my states -> size; istate ++)
		if (Melder_cmp (my states -> at [istate] -> label.get(), stateLabel) == 0)
			return istate;
	return 0;	
}

/*
	Translate the observation sequences to a number of state sequences for not continuous markov models.
	A 0 state number signals the start of a new observation sequence.
*/
static autoINTVEC HMM_HMMObservationSequenceBag_getStateSequences (HMM me, HMMObservationSequenceBag thee) {
	/*
		Calculate storage needed for state sequence numbers.
	*/
	integer numberOfElements = 0;
	for (integer iseq = 1; iseq <= thy size; iseq ++) {
		const HMMObservationSequence hmm_os = thy at [iseq];
		numberOfElements += hmm_os -> rows.size + 1; // 1 extra for 0 state 
	}
	/*
		Get state sequence numbers.
	*/
	autoINTVEC stateSequenceNumbers = raw_INTVEC (numberOfElements);
	integer numberOfElements2 = 0;
	for (integer iseq = 1; iseq <= thy size; iseq ++) {
		const HMMObservationSequence hmm_os = thy at [iseq];
		for (integer islabel = 1; islabel <= hmm_os -> rows.size; islabel ++) {
			const conststring32 label = Table_getStringValue_Assert (hmm_os, islabel, 1);
			const integer stateNumber = HMM_getState_notHidden (me, label);
			Melder_require (stateNumber > 0,
				U"The ", islabel, U"th observation of sequence ", iseq, U" labeled", label, U" is not a valid state.");
			stateSequenceNumbers [++ numberOfElements2] = stateNumber;
		}
		stateSequenceNumbers [++ numberOfElements2] = 0; // signal transition to another observation sequence
	}
	Melder_assert (numberOfElements == numberOfElements2);
	return stateSequenceNumbers;
}
#if 0
static void HMM_smoothInitialStateProbs_naive (HMM me, double minProb) {
	for (integer is = 1; is <= my numberOfStates; is ++)
			my initialStateProbs [is] = std::max (my initialStateProbs [is], minProb );
	/*
		Normalize again as probabilities
	*/
	VECnormalize_inplace (my initialStateProbs.get(), 1.0, 1.0);
}

static void HMM_smoothTransitionProbs_naive (HMM me, double minProb) {
	for (integer irow = 1; irow <= my numberOfStates; irow ++)
		for (integer icol = 1; icol <= my numberOfStates; icol ++)
			my transitionProbs [irow] [icol] = std::max (my transitionProbs [irow] [icol], minProb);
	/*
		Normalize again as probabilities
	*/
	for (integer irow = 1; irow <= my numberOfStates; irow ++)
		VECnormalize_inplace (my transitionProbs.row (irow).part (1, my numberOfStates), 1.0, 1.0);
}

static void HMM_smoothEmissionProbs_naive (HMM me, double minProb) {
	for (integer is = 1; is <= my numberOfStates; is ++)
		for (integer k = 1; k <= my numberOfObservationSymbols; k ++)
			my emissionProbs [is] [k] = std::max (my emissionProbs [is] [k], minProb);
	for (integer irow = 1; irow <= my numberOfStates; irow ++)
		VECnormalize_inplace (my emissionProbs.row (irow).part (1, my numberOfStates), 1.0, 1.0);
}
#endif
/*
	For a not hidden markov model there is an analytical solution for the state transition probabilities
*/

static void HMM_HMMObservationSequenceBag_learn_notHidden (HMM me, HMMObservationSequenceBag thee, double minProb) {
	Melder_assert (my notHidden);
	autoINTVEC stateSequenceNumbers = HMM_HMMObservationSequenceBag_getStateSequences (me, thee);
	if (stateSequenceNumbers.size < 2)
		return;
	my transitionProbs.all()  <<=  0.0;
	integer inum = 2;
	integer stateNumber = stateSequenceNumbers [1];
	while (inum < stateSequenceNumbers.size) {
		const integer stateNumberTo = stateSequenceNumbers [inum ++];
		if (stateNumberTo != 0) {
			my transitionProbs [stateNumber] [stateNumberTo] += 1.0;
			stateNumber = stateNumberTo;
		} else {
			if (inum == stateSequenceNumbers.size)
				break;
			stateNumber = stateSequenceNumbers [inum ++];
		}
	}
	/*
		Normalize as probabilities
	*/
	for (integer irow = 1; irow <= my numberOfStates; irow ++)
		VECnormalize_inplace (my transitionProbs.row (irow).part (1, my numberOfStates), 1.0, 1.0);
	/* No need to set minimum probabilities */
}


void HMM_HMMObservationSequenceBag_learn (HMM me, HMMObservationSequenceBag thee, double delta_lnp, double minProb, int info) {
	try {
		if (my notHidden) {
			// For a not hidden markov model there is an analytical solution for the state transition probabilities
			HMM_HMMObservationSequenceBag_learn_notHidden (me, thee, minProb);
			return;
		}
		// act as if all observation sequences are in memory
		const integer capacity = HMMObservationSequenceBag_getLongestSequence (thee);
		autoHMMBaumWelch bw = HMMBaumWelch_create (my numberOfStates, my numberOfObservationSymbols, capacity);
		bw -> minProb = minProb;
		if (info)
			MelderInfo_open (); 
		integer iter = 0;
		double lnp;
		do {
			lnp = bw -> lnProb;
			HMMBaumWelch_reInit (bw.get());
			for (integer iseq = 1; iseq <= thy size; iseq ++) {
				const HMMObservationSequence hmm_os = thy at [iseq];
				autoStringsIndex si = HMM_HMMObservationSequence_to_StringsIndex (me, hmm_os); // TODO outside the loop or more efficiently
				INTVEC obs = si -> classIndex.get();
				const integer nobs = si -> numberOfItems; // convenience

				// Interpretation of unknowns: end of sequence

				integer istart = 1, iend = nobs;
				while (istart <= nobs) {
					while (istart <= nobs && obs [istart] == 0)
						istart ++;
					if (istart > nobs) break;

					iend = istart + 1;
					while (iend <= nobs && obs [iend] != 0)
						iend ++;
					iend --;
					bw -> numberOfTimes = iend - istart + 1;
					bw -> totalNumberOfSequences ++;
					HMM_HMMBaumWelch_forward (me, bw.get(), obs.part (istart, nobs)); // get new alphas
					HMM_HMMBaumWelch_backward (me, bw.get(), obs.part (istart, nobs)); // get new betas
					HMMBaumWelch_getGamma (bw.get());
					HMM_HMMBaumWelch_getXi (me, bw.get(), obs.part (istart, nobs));
					HMM_HMMBaumWelch_addEstimate (me, bw.get(), obs.part (istart, nobs));
					istart = iend + 1;
				}
			}
			// we have processed all observation sequences, now it is time to estimate new probabilities.
			iter ++;
			HMM_HMMBaumWelch_reestimate (me, bw.get());
			if (info)
				MelderInfo_writeLine (U"Iteration: ", iter, U" ln(prob): ", bw -> lnProb);
		} while (fabs (lnp - bw -> lnProb) > std::max (fabs (delta_lnp * bw -> lnProb), NUMeps));
		if (info) {
			MelderInfo_writeLine (U"******** Learning summary *********");
			MelderInfo_writeLine (U"  Processed ", thy size, U" sequence", ( thy size > 1 ? U"s," : U"," ));
			MelderInfo_writeLine (U"  consisting of ", bw -> totalNumberOfSequences, U" observation sequence",
			( bw -> totalNumberOfSequences > 1 ? U"s." : U"." ));
			MelderInfo_writeLine (U"  Longest observation sequence had ", capacity, U" items.");
			MelderInfo_close();
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not learned.");
	}
}


// xc1 < xc2
void HMM_HMMStateSequence_drawTrellis (HMM me, HMMStateSequence thee, Graphics g, bool connect, bool garnish) {
	const integer numberOfTimes = thy numberOfStrings;
	autoStringsIndex si = HMM_HMMStateSequence_to_StringsIndex (me, thee);
	const double xmin = 0.0, xmax = numberOfTimes + 1.0, ymin = 0.5, ymax = my numberOfStates + 0.5;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	const double r = 0.2 / ( numberOfTimes > my numberOfStates ? numberOfTimes : my numberOfStates );

	for (integer it = 1; it <= numberOfTimes; it ++) {
		for (integer js = 1; js <= my numberOfStates; js ++) {
			double xc = it, yc = js, x2 = it, y2 = js;
			Graphics_circle (g, xc, yc, r);
			if (it > 1) {
				for (integer is = 1; is <= my numberOfStates; is ++) {
					const bool indexedConnection = si -> classIndex [it - 1] == is && si -> classIndex [it] == js;
					Graphics_setLineWidth (g, ( indexedConnection ? 2.0 : 1.0 ));
					Graphics_setLineType (g, ( indexedConnection ? Graphics_DRAWN : Graphics_DOTTED ));
					const double x1 = it - 1, y1 = is;
					if (connect || indexedConnection) {
						const double a = (y1 - y2) / (x1 - x2), b = y1 - a * x1;
						// double xs11 = x1 - r / (a * a + 1), ys11 = a * xs11 + b;
						const double xs12 = x1 + r / (a * a + 1), ys12 = a * xs12 + b;
						const double xs21 = x2 - r / (a * a + 1), ys21 = a * xs21 + b;
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
		for (integer js = 1; js <= my numberOfStates; js ++) {
			const HMMState hmms = my states->at [js];
			Graphics_markLeft (g, js, false, false, false, hmms -> label.get());
		}
		Graphics_marksBottomEvery (g, 1.0, 1.0, true, true, false);
		Graphics_textBottom (g, true, U"Time index");
	}
}

void HMM_drawBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	const double xmin = 0.0, xmax = 1.0, ymin = 0.0, ymax = 1.0;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	const double xleft  = 0.1, xright = 0.9, r = 0.03;
	const integer np = 6;
	const double dy = (1.0 - 0.3) / (np - 1);
	const double x0 = xleft, y0 = 0.5;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_circle (g, x0, y0, r);
	const double x = xright;
	double y = 0.9;
	for (integer i = 1; i <= np; i ++) {
		if (i < 4 or i == np) {
			Graphics_circle (g, x, y, r);
			const double xx = x0 - x, yy = y - y0;
			const double c = sqrt (xx * xx + yy * yy);
			const double cosa = xx / c, sina = yy / c;
			Graphics_line (g, x0 - r * cosa, y0 + r * sina, x + r * cosa, y - r * sina);
		} else if (i == 4) {
			const double ddy = 3.0 * dy / 4.0;
			Graphics_fillCircle (g, x, y + dy - ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 2.0 * ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 3.0 * ddy, 0.5 * r);
		}
		y -= dy;
	}
	if (garnish) {
		const double x1 = xright + 1.5 * r, x2 = x1 - 0.2;
		double y1 = 0.9;

		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__1_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%i1_");

		y1 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__2_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%i2_");

		y1 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__%N_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__%%iN%_");

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x0 - 1.5 * r, y0, U"%s__%i_");

		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x0, 0.0, U"%t");
		Graphics_text (g, x, 0.0, U"%t+1");

		const double y3 = 0.10;
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x0, y3, U"%\\be__%t_(%i)%");
		Graphics_text (g, x, y3, U"%\\be__%t+1_(%j)");
	}
}

void HMM_drawForwardProbabilitiesIllustration (Graphics g, bool garnish) {
	constexpr double xmin = 0.0, xmax = 1.0, ymin = 0.0, ymax = 1.0;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	const double xleft = 0.1, xright = 0.9, r = 0.03;
	constexpr integer np = 6;
	const double dy = (1.0 - 0.3) / (np - 1);
	const double x0 = xright, y0 = 0.5;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_circle (g, x0, y0, r);
	const double x = xleft;
	double  y = 0.9;
	for (integer i = 1; i <= np; i ++) {
		if (i < 4 or i == np) {
			Graphics_circle (g, x, y, r);
			const double xx = x0 - x, yy = y - y0;
			const double c = sqrt (xx * xx + yy * yy);
			const double cosa = xx / c, sina = yy / c;
			Graphics_line (g, x0 - r * cosa, y0 + r * sina, x + r * cosa, y - r * sina);
		} else if (i == 4) {
			const double ddy = 3.0 * dy / 4.0;
			Graphics_fillCircle (g, x, y + dy - ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 2.0 * ddy, 0.5 * r);
			Graphics_fillCircle (g, x, y + dy - 3.0 * ddy, 0.5 * r);
		}
		y -= dy;
	}
	if (garnish) {
		const double x1 = xleft - 1.5 * r, x2 = x1 + 0.2, y1 = 0.9;

		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y1, U"%s__1_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y1, U"%a__1%j_");

		const double y2 = 0.9 - dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y2, U"%s__2_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y2, U"%a__2%j_");

		const double y3 = 0.9 - (np - 1) * dy;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x1, y3, U"%s__%N_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x2, y3, U"%a__%%Nj%_");

		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_text (g, x0 + 1.5 * r, y0, U"%s__%j_");

		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
		Graphics_text (g, x, 0.0, U"%t");
		Graphics_text (g, x0, 0.0, U"%t+1");

		const double y4 = 0.10;
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (g, x, y4, U"%\\al__%t_(%i)%");
		Graphics_text (g, x0, y4, U"%\\al__%t+1_(%j)");
	}
}

void HMM_drawForwardAndBackwardProbabilitiesIllustration (Graphics g, bool garnish) {
	const double xfrac = 0.1, xs = 1.0 / (0.5 - xfrac), r = 0.03;
	Graphics_Viewport vp = Graphics_insetViewport (g, 0.0, 0.5-xfrac, 0.0, 1.0);
	HMM_drawForwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_insetViewport (g, 0.5 + xfrac, 1.0, 0.0, 1.0);
	HMM_drawBackwardProbabilitiesIllustration (g, false);
	Graphics_resetViewport (g, vp);
	Graphics_setWindow (g, 0.0, xs, 0.0, 1.0);
	if (garnish) {
		const double rx1 = 1.0 + xs * 2.0 * xfrac + 0.1, rx2 = rx1 + 0.9 - 0.1, y1 = 0.1;
		Graphics_line (g, 0.9 + r, 0.5, rx1 - r, 0.5);
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.9, 0.5 + r, U"%s__%i_");
		Graphics_text (g, rx1, 0.5 + r, U"%s__%j_");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
		Graphics_text (g, 1.0 + xfrac * xs, 0.5, U"%a__%%ij%_%b__%j_(O__%t+1_)");
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
		Graphics_text (g, 0.1, 0.0, U"%t-1");
		Graphics_text (g, 0.9, 0.0, U"%t");
		Graphics_text (g, rx1, 0.0, U"%t+1");
		Graphics_text (g, rx2, 0.0, U"%t+2");
		Graphics_setLineType (g, Graphics_DASHED);
		const double x4 = rx1 - 0.06, x3 = 0.9 + 0.06;
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

void HMM_HMMBaumWelch_getXi (HMM me, HMMBaumWelch thee, constINTVEC obs) {
	Melder_assert (obs.size == thy numberOfTimes);
	for (integer it = 1; it <= thy numberOfTimes - 1; it ++) {
		longdouble sum = 0.0;
		MATVU xi_it = thy xi [it];
		for (integer is = 1; is <= thy numberOfStates; is ++) {
			for (integer js = 1; js <= thy numberOfStates; js ++) {
				xi_it [is] [js] = thy alpha [is] [it] * thy beta [js] [it + 1] *
					my transitionProbs [is] [js] * my emissionProbs [js] [ obs [it + 1] ];
				sum += xi_it [is] [js];
			}
		}
		xi_it  /=  double (sum);
	}
}

void HMM_HMMBaumWelch_addEstimate (HMM me, HMMBaumWelch thee, constINTVEC obs) {
	Melder_assert (obs.size == thy numberOfTimes);
	for (integer is = 1; is <= my numberOfStates; is ++) {
		// only for valid start states with p > 0
		if (my initialStateProbs [is] > 0.0) {
			thy aij_num_p0 [is] += thy gamma [is] [1];
			thy aij_denom_p0 [is] += 1.0;
		}
	}

	for (integer is = 1; is <= my numberOfStates; is ++) {
		double gammasum = NUMsum (thy gamma.row (is).part (1, thy numberOfTimes - 1));

		for (integer js = 1; js <= my numberOfStates; js ++) {
			longdouble xisum = 0.0;
			for (integer it = 1; it <= thy numberOfTimes - 1; it ++)
				xisum += thy xi [it] [is] [js];
			// zero probs signal invalid connections, don't reestimate
			if (my transitionProbs [is] [js] > 0.0) {
				thy aij_num [is] [js] += double (xisum);
				thy aij_denom [is] [js] += gammasum;
			}
		}

		/*
			Only reestimate the emissionProbs for a hidden markov model.
			A not hidden model is emulated with fixed emissionProbs.
		*/
		if (! my notHidden) {
			gammasum += thy gamma [is] [thy numberOfTimes];   // now sum all, add last term
			for (integer k = 1; k <= my numberOfObservationSymbols; k ++) {
				longdouble gammasum_k = 0.0;
				for (integer it = 1; it <= thy numberOfTimes; it ++)
					if (obs [it] == k)
						gammasum_k += thy gamma [is] [it];
				// only reestimate probs > 0 !
				if (my emissionProbs [is] [k] > 0.0) {
					thy bik_num [is] [k] += double (gammasum_k);
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

void HMM_HMMBaumWelch_reestimate (HMM me, HMMBaumWelch thee) {
	double p;
	/*
		If we only have a couple of training sequences and they all happen to start with the same symbol,
		one or more starting probabilities can be zero.

		What to do with the P_k (see Rabiner formulas 109-110)?
	*/
	for (integer is = 1; is <= my numberOfStates; is ++) {
		/*
			If we have not enough observation sequences it can happen that some probabilities
			become zero. This means that for some future observation sequences the probability evaluation
			returns  p=0 for sequences where these transitions turn up. This makes recognition impossible and 
			also comparisons between models are difficult.
			We can prevent this from happening by asumimg a minimal probability for valid transitions
			i.e. which have initially p > 0.
		*/
		if (my initialStateProbs [is] > 0.0) {
			p = thy aij_num_p0 [is] / thy aij_denom_p0 [is];
			my initialStateProbs [is] = ( p > 0.0 ? p : thy minProb );
		}
		for (integer js = 1; js <= my numberOfStates; js ++) {
			if (my transitionProbs [is] [js] > 0.0) {
				p = thy aij_num [is] [js] / thy aij_denom [is] [js];
				my transitionProbs [is] [js] = ( p > 0.0 ? p : thy minProb );
			}
		}
		if (! my notHidden) {
			for (integer k = 1; k <= my numberOfObservationSymbols; k ++) {
				if (my emissionProbs [is] [k] > 0.0) {
					p = thy bik_num [is] [k] / thy bik_denom [is] [k];
					my emissionProbs [is] [k] = ( p > 0.0 ? p : thy minProb );
				}
			}
		}
		if (my leftToRight && my transitionProbs [is] [my numberOfStates + 1] > 0.0) {
			p = thy aij_num [is] [my numberOfStates + 1] / thy aij_denom [is] [my numberOfStates + 1];
			my transitionProbs [is] [my numberOfStates + 1] = ( p > 0.0 ? p : thy minProb );
		}
	}
}

void HMM_HMMBaumWelch_forward (HMM me, HMMBaumWelch thee, constINTVEC obs) {
	// initialise at t = 1 & scale
	for (integer js = 1; js <= my numberOfStates; js ++)
		thy alpha [js] [1] = my initialStateProbs [js] * my emissionProbs [js] [obs [1]];
	thy scale [1] = NUMsum (thy alpha.column (1));
	thy alpha.column (1) /= thy scale [1];
	// recursion
	for (integer it = 2; it <= thy numberOfTimes; it ++) {
		thy scale [it] = 0.0;
		for (integer js = 1; js <= my numberOfStates; js ++) {
			double sum = 0.0;
			for (integer is = 1; is <= my numberOfStates; is ++) {
				sum += thy alpha [is] [it - 1] * my transitionProbs [is] [js];
			}

			thy alpha [js] [it] = sum * my emissionProbs [js] [obs [it]];
			thy scale [it] += thy alpha [js] [it];
		}

		for (integer js = 1; js <= my numberOfStates; js ++) {
			thy alpha [js] [it] /= thy scale [it];
		}
	}

	for (integer it = 1; it <= thy numberOfTimes; it ++) {
		thy lnProb += log (thy scale [it]);
	}
}

void HMM_HMMBaumWelch_backward (HMM me, HMMBaumWelch thee, constINTVEC obs) {
	Melder_assert (obs.size == thy numberOfTimes);
	for (integer is = 1; is <= my numberOfStates; is ++) {
		thy beta [is] [thy numberOfTimes] = 1.0 / thy scale [thy numberOfTimes];
	}
	for (integer it = thy numberOfTimes - 1; it >= 1; it --) {
		for (integer is = 1; is <= my numberOfStates; is ++) {
			longdouble sum = 0.0;
			for (integer js = 1; js <= my numberOfStates; js ++) {
				sum += thy beta [js] [it + 1] * my transitionProbs [is] [js] * my emissionProbs [js] [obs [it + 1]];
			}
			thy beta [is] [it] = double (sum) / thy scale [it];
		}
	}
}

/*************************** HMM decoding ***********************************/

// precondition: valid symbols, i.e. 1 <= o [i] <= my numberOfSymbols for i=1..nt
void HMM_HMMViterbi_decode (HMM me, HMMViterbi thee, constINTVEC obs) {
	Melder_assert (obs.size == thy numberOfTimes);
	const integer numberOfTimes = thy numberOfTimes;
	// initialisation
	for (integer is = 1; is <= my numberOfStates; is ++) {
		thy viterbi [is] [1] = my initialStateProbs [is] * my emissionProbs [is] [obs [1]];
		thy bp [is] [1] = 0;
	}
	// recursion
	for (integer it = 2; it <= numberOfTimes; it ++) {
		for (integer is = 1; is <= my numberOfStates; is ++) {
			// all transitions isp -> is from previous time to current
			double max_score = -1; // any negative number is ok
			for (integer isp = 1; isp <= my numberOfStates; isp ++) {
				const double score = thy viterbi [isp] [it - 1] * my transitionProbs [isp] [is]; // * my emissionProbs [is] [obs [it]]
				if (score > max_score) {
					max_score = score;
					thy bp [is] [it] = isp;
				}
			}
			thy viterbi [is] [it] = max_score * my emissionProbs [is] [ obs [it] ];
		}
	}
	// path starts at state with best end probability
	thy path [numberOfTimes] = 1;
	thy prob = thy viterbi [1] [numberOfTimes];
	for (integer is = 2; is <= my numberOfStates; is ++) {
		if (thy viterbi [is] [numberOfTimes] > thy prob)
			thy prob = thy viterbi [thy path [numberOfTimes] = is] [numberOfTimes];
	}
	// trace back and get path
	for (integer it = numberOfTimes; it > 1; it --)
		thy path [it - 1] = thy bp [thy path [it]] [it];
}

autoHMMStateSequence HMM_HMMObservationSequence_to_HMMStateSequence (HMM me, HMMObservationSequence thee) {
	try {
		autoStringsIndex si = HMM_HMMObservationSequence_to_StringsIndex (me, thee);
		INTVEC obs = si -> classIndex.get(); // convenience
		const integer numberOfUnknowns = StringsIndex_countItems (si.get(), 0);
		
		Melder_require (numberOfUnknowns == 0, U"Unknown observation symbol(s) (# = ", numberOfUnknowns, U").");

		const integer numberOfTimes = thy rows.size;
		autoHMMViterbi v = HMM_to_HMMViterbi (me, obs);
		autoHMMStateSequence him = HMMStateSequence_create (numberOfTimes);
		// trace the path and get states
		for (integer it = 1; it <= numberOfTimes; it ++) {
			const HMMState hmms = my states->at [v -> path [it]];
			his strings [it] = Melder_dup (hmms -> label.get());
			his numberOfStrings ++;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no HMMStateSequence created.");
	}
}

double HMM_HMMStateSequence_getProbability (HMM me, HMMStateSequence thee) {
	autoStringsIndex si = HMM_HMMStateSequence_to_StringsIndex (me, thee);
	const integer numberOfUnknowns = StringsIndex_countItems (si.get(), 0);
	constINTVEC index = si -> classIndex.get();

	if (NUMisEmpty (index))
		return undefined;
	if (numberOfUnknowns > 0) {
		Melder_warning (U"Unknown states (# = ", numberOfUnknowns, U").");
		return undefined;
	}
	const double p0 = my initialStateProbs [index [1]];
	Melder_require (p0 > 0.0,
		U"You should not start with a zero probability state.");
	
	double lnp = log (p0);
	for (integer it = 2; it <= thy numberOfStrings; it ++)
		lnp += log (my transitionProbs [index [it - 1]] [index [it]]);
	return lnp;
}

double HMM_getProbabilityAtTimeBeingInState (HMM me, integer itime, integer istate) {
	if (istate < 1 || istate > my numberOfStates)
		return undefined;

	autoVEC scale = zero_VEC (itime);
	autoVEC alpha_t = raw_VEC (my numberOfStates);
	autoVEC alpha_tm1 = zero_VEC (my numberOfStates);

	alpha_t.all() <<= my initialStateProbs.all();
	scale [1] = NUMsum (alpha_t.all());

	alpha_t. all() /= scale [1];
	
	// recursion
	for (integer it = 2; it <= itime; it ++) {
		alpha_tm1.all()  <<=  alpha_t.all();

		for (integer js = 1; js <= my numberOfStates; js ++) {
			longdouble sum = 0.0; // NUMinner (alpha_tm1.get(), my transitionProbs.column (js));
			for (integer is = 1; is <= my numberOfStates; is ++)
				sum += alpha_tm1 [is] * my transitionProbs [is] [js];
			alpha_t [js] = double (sum);
			scale [it] += alpha_t [js];
		}
		alpha_t. all() /= scale [it];
	}

	longdouble lnp = 0.0;
	for (integer it = 1; it <= itime; it ++)
		lnp += log (scale [it]);

	lnp = ( alpha_t [istate] > 0 ? lnp + log (alpha_t [istate]) : -INFINITY ); // p = 0 -> ln(p)=-infinity  // ppgb INFINITY is a small number
	return double (lnp);
}

double HMM_getProbabilityAtTimeBeingInStateEmittingSymbol (HMM me, integer itime, integer istate, integer isymbol) {
	// for a notHidden model emissionProbs may be zero!
	if (isymbol < 1 || isymbol > my numberOfObservationSymbols || my emissionProbs [istate] [isymbol] == 0.0)
		return undefined;
	const double lnp = HMM_getProbabilityAtTimeBeingInState (me, itime, istate);
	return ( isundef (lnp) ? undefined : lnp + log (my emissionProbs [istate] [isymbol]) );
}

double HMM_getProbabilityOfObservations (HMM me, constINTVEC obs) {
	Melder_assert (obs.size > 0);
	const integer numberOfTimes = obs.size;
	autoVEC scale = zero_VEC (numberOfTimes);
	autoVEC alpha_t = zero_VEC (my numberOfStates);
	autoVEC alpha_tm1 = zero_VEC (my numberOfStates);

	// initialise
	for (integer js = 1; js <= my numberOfStates; js ++) {
		alpha_t [js] = my initialStateProbs [js] * my emissionProbs [js] [obs [1]];
		scale [1] += alpha_t [js];
	}
	Melder_require (scale [1] > 0.0,
		U"The observation sequence should not start with a symbol whose state has zero starting probability.");
	
	alpha_t.all()  /=  scale [1];

	// recursion
	for (integer it = 2; it <= numberOfTimes; it ++) {
		alpha_tm1.all()  <<=  alpha_t.all();

		for (integer js = 1; js <= my numberOfStates; js ++) {
			longdouble sum = 0.0;
			for (integer is = 1; is <= my numberOfStates; is ++) {
				sum += alpha_tm1 [is] * my transitionProbs [is] [js];
			}
			alpha_t [js] = sum * my emissionProbs [js] [obs [it]];
			scale [it] += alpha_t [js];
		}
		if (scale [it] <= 0.0)
			return -INFINITY;
		for (integer js = 1; js <= my numberOfStates; js ++)
			alpha_t [js] /= scale [it];
	}

	double lnp = 0.0;
	for (integer it = 1; it <= numberOfTimes; it ++)
		lnp += log (scale [it]);
	return lnp;
}

double HMM_HMMObservationSequence_getProbability (HMM me, HMMObservationSequence thee) {
	autoStringsIndex si = HMM_HMMObservationSequence_to_StringsIndex (me, thee);
	const integer numberOfUnknowns = StringsIndex_countItems (si.get(), 0);
	Melder_require (numberOfUnknowns == 0, U"Unknown observations (# = ", numberOfUnknowns, U").");
	
	return HMM_getProbabilityOfObservations (me, si -> classIndex.get());
}

double HMM_HMMObservationSequence_getCrossEntropy (HMM me, HMMObservationSequence thee) {
	const double lnp = HMM_HMMObservationSequence_getProbability (me, thee);
	return ( isundef (lnp) ? undefined :
	        -lnp / (NUMln10 * HMMObservationSequence_getNumberOfObservations (thee)) );
}

double HMM_HMMObservationSequence_getPerplexity (HMM me, HMMObservationSequence thee) {
	const double ce = HMM_HMMObservationSequence_getCrossEntropy (me, thee);
	return ( isundef (ce) ? undefined : pow (2.0, ce) );
}

autoHMM HMM_createFromHMMObservationSequence (HMMObservationSequence me, integer numberOfStates, int leftToRight) {
	try {
		autoHMM thee = Thing_new (HMM);
		autoStrings s = HMMObservationSequence_to_Strings (me);
		autoDistributions d = Strings_to_Distributions (s.get());

		const integer numberOfObservationSymbols = d -> numberOfRows;
		thy notHidden = numberOfStates < 1;
		numberOfStates = ( numberOfStates > 0 ? numberOfStates : numberOfObservationSymbols );

		HMM_init (thee.get(), numberOfStates, numberOfObservationSymbols, leftToRight);

		for (integer i = 1; i <= numberOfObservationSymbols; i ++) {
			const conststring32 label = d -> rowLabels [i].get();
			autoHMMObservation hmmo = HMMObservation_create (label, 0, 0, kHMMstorage::DIAGONALS);
			HMM_addObservation_move (thee.get(), hmmo.move());
			if (thy notHidden) {
				autoHMMState hmms = HMMState_create (label);
				HMM_addState_move (thee.get(), hmms.move());
			}
		}
		if (! thy notHidden)
			HMM_setDefaultStates (thee.get());

		//HMM_setDefaultTransitionProbs (thee.get());
		//HMM_setDefaultInitialStateProbs (thee.get());
		//HMM_setDefaultEmissionProbs (thee.get());
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

autoStringsIndex HMM_HMMObservationSequence_to_StringsIndex (HMM me, HMMObservationSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = autoSTRVEC (my numberOfObservationSymbols);
		for (integer is = 1; is <= my numberOfObservationSymbols; is ++) {
			const HMMObservation hmmo = my observationSymbols->at [is];
			classes -> strings [is] = Melder_dup (hmmo -> label.get());
			classes -> numberOfStrings ++;
		}
		autoStrings obs = HMMObservationSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (obs.get(), classes.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoStringsIndex HMM_HMMStateSequence_to_StringsIndex (HMM me, HMMStateSequence thee) {
	try {
		autoStrings classes = Thing_new (Strings);
		classes -> strings = autoSTRVEC (my numberOfObservationSymbols);
		for (integer is = 1; is <= my numberOfStates; is ++) {
			const HMMState hmms = my states->at [is];
			classes -> strings [is] = Melder_dup (hmms -> label.get());
			classes -> numberOfStrings ++;
		}
		autoStrings sts = HMMStateSequence_to_Strings (thee);
		autoStringsIndex him = Stringses_to_StringsIndex (sts.get(), classes.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoTableOfReal HMM_HMMObservationSequence_to_TableOfReal_transitions (HMM me, HMMObservationSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_HMMObservationSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.get(), probabilities);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created for HMMObservationSequence.");
	}
}

autoTableOfReal HMM_HMMStateSequence_to_TableOfReal_transitions (HMM me, HMMStateSequence thee, int probabilities) {
	try {
		autoStringsIndex si = HMM_HMMStateSequence_to_StringsIndex (me, thee);
		autoTableOfReal him = StringsIndex_to_TableOfReal_transitions (si.get(), probabilities);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no transition table created for HMMStateSequence.");
	}
}

autoTableOfReal StringsIndex_to_TableOfReal_transitions (StringsIndex me, int probabilities) {
	try {
		const integer numberOfTypes = my classes->size;

		autoTableOfReal thee = TableOfReal_create (numberOfTypes + 1, numberOfTypes + 1);
		for (integer i = 1; i <= numberOfTypes; i ++) {
			const SimpleString s = (SimpleString) my classes->at [i];
			TableOfReal_setRowLabel (thee.get(), i, s -> string.get());
			TableOfReal_setColumnLabel (thee.get(), i, s -> string.get());
		}
		for (integer i = 2; i <= my numberOfItems; i ++)
			if (my classIndex [i - 1] > 0 && my classIndex [i] > 0) // a zero is a restart!
				thy data [my classIndex [i-1]] [my classIndex [i]] ++;

		longdouble sum = 0.0;
		for (integer i = 1; i <= numberOfTypes; i ++) {
			const double rowSum = NUMsum (thy data.row (i));
			const double colSum = NUMsum (thy data.column (i));
			thy data [i] [numberOfTypes + 1] = rowSum;
			thy data [numberOfTypes + 1] [i] = colSum;
			sum += colSum;
		}
		thy data [numberOfTypes + 1] [numberOfTypes + 1] = sum;
		if (probabilities && sum > 0.0) {
			for (integer i = 1; i <= numberOfTypes; i ++)
				if (thy data [i] [numberOfTypes + 1] > 0.0)
					for (integer j = 1; j <= numberOfTypes; j ++)
						thy data [i] [j] /= thy data [i] [numberOfTypes + 1];
			for (integer i = 1; i <= numberOfTypes; i ++) {
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
