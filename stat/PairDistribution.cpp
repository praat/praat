/* PairDistribution.cpp
 *
 * Copyright (C) 1997-2012,2013,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PairDistribution.h"

#include "oo_DESTROY.h"
#include "PairDistribution_def.h"
#include "oo_COPY.h"
#include "PairDistribution_def.h"
#include "oo_EQUAL.h"
#include "PairDistribution_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PairDistribution_def.h"
#include "oo_WRITE_TEXT.h"
#include "PairDistribution_def.h"
#include "oo_READ_TEXT.h"
#include "PairDistribution_def.h"
#include "oo_WRITE_BINARY.h"
#include "PairDistribution_def.h"
#include "oo_READ_BINARY.h"
#include "PairDistribution_def.h"
#include "oo_DESCRIPTION.h"
#include "PairDistribution_def.h"

Thing_implement (PairProbability, Daata, 0);

Thing_implement (PairDistribution, Daata, 0);

void structPairDistribution :: v_info () {
	PairDistribution_Parent :: v_info ();
	MelderInfo_writeLine (U"Number of pairs: ", pairs.size);
}

autoPairProbability PairProbability_create (conststring32 string1, conststring32 string2, double weight) {
	autoPairProbability me = Thing_new (PairProbability);
	my string1 = Melder_dup (string1);
	my string2 = Melder_dup (string2);
	my weight = weight;
	return me;
}

autoPairDistribution PairDistribution_create () {
	try {
		autoPairDistribution me = Thing_new (PairDistribution);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PairDistribution not created.");
	}
}

int PairProbability_compare (PairProbability me, PairProbability thee) noexcept {
	return str32cmp (my string1.get(), thy string1.get());
}

static void PairDistribution_checkSpecifiedPairNumber (PairDistribution me, integer pairNumber) {
	if (pairNumber < 1)
		Melder_throw (me, U": the specified pair number is ", pairNumber, U", but should be at least 1.");
	if (pairNumber > my pairs.size)
		Melder_throw (me, U": the specified pair number is ", pairNumber, U", but should be at most my number of pairs (", my pairs.size, U").");
}

conststring32 PairDistribution_getString1 (PairDistribution me, integer pairNumber) {
	try {
		PairDistribution_checkSpecifiedPairNumber (me, pairNumber);
		PairProbability prob = my pairs.at [pairNumber];
		return prob -> string1.get();
	} catch (MelderError) {
		Melder_throw (me, U": string1 not retrieved.");
	}
}

conststring32 PairDistribution_getString2 (PairDistribution me, integer pairNumber) {
	try {
		PairDistribution_checkSpecifiedPairNumber (me, pairNumber);
		PairProbability prob = my pairs.at [pairNumber];
		return prob -> string2.get();
	} catch (MelderError) {
		Melder_throw (me, U": string2 not retrieved.");
	}
}

double PairDistribution_getWeight (PairDistribution me, integer pairNumber) {
	try {
		PairDistribution_checkSpecifiedPairNumber (me, pairNumber);
		PairProbability prob = my pairs.at [pairNumber];
		return prob -> weight;
	} catch (MelderError) {
		Melder_throw (me, U": weight not retrieved.");
	}
}

void PairDistribution_add (PairDistribution me, conststring32 string1, conststring32 string2, double weight) {
	autoPairProbability pair = PairProbability_create (string1, string2, weight);
	my pairs.addItem_move (pair.move());
}

void PairDistribution_removeZeroWeights (PairDistribution me) {
	for (integer ipair = my pairs.size; ipair > 0; ipair --) {
		PairProbability prob = my pairs.at [ipair];
		if (prob -> weight <= 0.0) {
			my pairs.removeItem (ipair);
		}
	}
}

void PairDistribution_swapInputsAndOutputs (PairDistribution me) {
	for (integer ipair = my pairs.size; ipair > 0; ipair --) {
		PairProbability prob = my pairs.at [ipair];
		std::swap (prob -> string1, prob -> string2);
	}
}

static double PairDistributions_getTotalWeight_checkPositive (PairDistribution me) {
	longdouble totalWeight = 0.0;
	for (integer ipair = 1; ipair <= my pairs.size; ipair ++) {
		PairProbability prob = my pairs.at [ipair];
		totalWeight += prob -> weight;
	}
	Melder_require (totalWeight > 0.0,
		me, U": the total probability weight is ", Melder_half (double (totalWeight)),
		U" but should be greater than zero for this operation."
	);
	return double (totalWeight);
}

void PairDistribution_to_Stringses (PairDistribution me, integer nout, autoStrings *strings1_out, autoStrings *strings2_out) {
	try {
		const integer nin = my pairs.size;
		if (nin < 1)
			Melder_throw (U"No candidates.");
		if (nout < 1)
			Melder_throw (U"Number of generated string pairs should be positive.");
		const double total = PairDistributions_getTotalWeight_checkPositive (me);
		autoStrings strings1 = Thing_new (Strings);
		strings1 -> numberOfStrings = nout;
		strings1 -> strings = autoSTRVEC (nout);
		autoStrings strings2 = Thing_new (Strings);
		strings2 -> numberOfStrings = nout;
		strings2 -> strings = autoSTRVEC (nout);
		for (integer iout = 1; iout <= nout; iout ++) {
			integer iin;
			do {
				const double rand = NUMrandomUniform (0, total);
				longdouble sum = 0.0;
				for (iin = 1; iin <= nin; iin ++) {
					PairProbability prob = my pairs.at [iin];
					sum += prob -> weight;
					if (rand <= sum) break;
				}
			} while (iin > nin);   /* Guard against rounding errors. */
			PairProbability const prob = my pairs.at [iin];
			if (! prob -> string1 || ! prob -> string2)
				Melder_throw (U"No string in probability pair ", iin, U".");
			strings1 -> strings [iout] = Melder_dup (prob -> string1.get());
			strings2 -> strings [iout] = Melder_dup (prob -> string2.get());
		}
		*strings1_out = strings1.move();
		*strings2_out = strings2.move();
	} catch (MelderError) {
		Melder_throw (me, U": generation of Stringses not performed.");
	}
}

void PairDistribution_peekPair (PairDistribution me, conststring32 *out_string1, conststring32 *out_string2) {
	try {
		*out_string1 = *out_string2 = nullptr;
		longdouble total = 0.0;
		const integer nin = my pairs.size;
		PairProbability prob;
		if (nin < 1) Melder_throw (U"No candidates.");
		integer iin;
		for (iin = 1; iin <= nin; iin ++) {
			prob = my pairs.at [iin];
			total += prob -> weight;
		}
		do {
			const double rand = NUMrandomUniform (0.0, double (total));
			longdouble sum = 0.0;
			for (iin = 1; iin <= nin; iin ++) {
				prob = my pairs.at [iin];
				sum += prob -> weight;
				if (rand <= sum) break;
			}
		} while (iin > nin);   // guard against rounding errors
		prob = my pairs.at [iin];
		if (! prob -> string1 || ! prob -> string2) Melder_throw (U"No string in probability pair ", iin, U".");
		*out_string1 = prob -> string1.get();
		*out_string2 = prob -> string2.get();
	} catch (MelderError) {
		Melder_throw (me, U": pair not peeked.");
	}
}

static double PairDistribution_getFractionCorrect (PairDistribution me, int which) {
	try {
		longdouble correct = 0.0;
		integer pairmin = 1, ipair;
		autoPairDistribution thee = Data_copy (me);
		thy pairs.sort (PairProbability_compare);
		double total = PairDistributions_getTotalWeight_checkPositive (thee.get());
		do {
			integer pairmax = pairmin;
			const conststring32 firstInput = thy pairs.at [pairmin] -> string1.get();
			for (ipair = pairmin + 1; ipair <= thy pairs.size; ipair ++) {
				PairProbability const prob = thy pairs.at [ipair];
				if (! str32equ (prob -> string1.get(), firstInput)) {
					pairmax = ipair - 1;
					break;
				}
			}
			if (ipair > thy pairs.size) pairmax = thy pairs.size;
			if (which == 0) {
				double pmax = 0.0;
				for (ipair = pairmin; ipair <= pairmax; ipair ++) {
					PairProbability const prob = thy pairs.at [ipair];
					const double p = prob -> weight / total;
					if (p > pmax) pmax = p;
				}
				correct += pmax;
			} else {
				longdouble sum = 0.0, p2 = 0.0;
				for (ipair = pairmin; ipair <= pairmax; ipair ++) {
					PairProbability const prob = thy pairs.at [ipair];
					const double p = prob -> weight / total;
					sum += p;
					p2 += p * p;
				}
				correct += p2 / sum;
			}
			pairmin = pairmax + 1;
		} while (pairmin <= thy pairs.size);
		return double (correct);
	} catch (MelderError) {
		Melder_throw (me, U": could not compute my fraction correct.");
	}
}

double PairDistribution_getFractionCorrect_maximumLikelihood (PairDistribution me) {
	return PairDistribution_getFractionCorrect (me, 0);
}

double PairDistribution_getFractionCorrect_probabilityMatching (PairDistribution me) {
	return PairDistribution_getFractionCorrect (me, 1);
}

double PairDistribution_Distributions_getFractionCorrect (PairDistribution me, Distributions dist, integer column) {
	try {
		longdouble correct = 0.0;
		integer pairmin = 1;
		char32 string [1000];
		Distributions_checkSpecifiedColumnNumberWithinRange (dist, column);
		autoPairDistribution thee = Data_copy (me);
		thy pairs.sort (PairProbability_compare);
		double total = PairDistributions_getTotalWeight_checkPositive (thee.get());
		do {
			integer pairmax = pairmin, ipair;
			longdouble sum = 0.0, sumDist = 0.0;
			const conststring32 firstInput = thy pairs.at [pairmin] -> string1.get();
			for (ipair = pairmin + 1; ipair <= thy pairs.size; ipair ++) {
				PairProbability prob = thy pairs.at [ipair];
				if (! str32equ (prob -> string1.get(), firstInput)) {
					pairmax = ipair - 1;
					break;
				}
			}
			if (ipair > thy pairs.size) pairmax = thy pairs.size;
			for (ipair = pairmin; ipair <= pairmax; ipair ++) {
				PairProbability prob = thy pairs.at [ipair];
				const double p = prob -> weight / total;
				double pout = 0.0;
				Melder_sprint (string, 1000, prob -> string1.get(), U" \\-> ", prob -> string2.get());
				for (integer idist = 1; idist <= dist -> numberOfRows; idist ++) {
					if (str32equ (string, dist -> rowLabels [idist].get())) {
						pout = dist -> data [idist] [column];
						break;
					}
				}
				sum += p * pout;
			}
			Melder_sprint (string, 1000, firstInput, U" \\-> ");
			const integer length = str32len (string);
			for (integer idist = 1; idist <= dist -> numberOfRows; idist ++)
				if (str32nequ (string, dist -> rowLabels [idist].get(), length))
					sumDist += dist -> data [idist] [column];
			if (sumDist != 0.0) correct += sum / sumDist;
			pairmin = pairmax + 1;
		} while (pairmin <= thy pairs.size);
		return double (correct);
	} catch (MelderError) {
		Melder_throw (me, U" & ", dist, U": could not compute our fraction correct.");
	}
}

autoTable PairDistribution_to_Table (PairDistribution me) {
	try {
		autoTable thee = Table_createWithColumnNames (my pairs.size, U"string1 string2 weight");
		for (integer ipair = 1; ipair <= my pairs.size; ipair ++) {
			PairProbability prob = my pairs.at [ipair];
			Table_setStringValue (thee.get(), ipair, 1, prob -> string1.get());
			Table_setStringValue (thee.get(), ipair, 2, prob -> string2.get());
			Table_setNumericValue (thee.get(), ipair, 3, prob -> weight);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

/* End of file PairDistribution.cpp */
