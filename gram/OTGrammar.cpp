/* OTGrammar.cpp
 *
 * Copyright (C) 1997-2020 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2002/11/04 randomize in case of equal candidates
 * pb 2003/05/08 better superset violation warning
 * pb 2003/05/23 made superset violation warning conditional
 * pb 2003/10/15 backtrack in case of failing multiple chews for EDCD
 * pb 2003/10/15 crucial ties option
 * pb 2004/01/17 OTGrammar_Distributions_getFractionCorrect
 * pb 2004/08/08 OTGrammar_removeHarmonicallyBoundedCandidates
 * pb 2004/08/09 bug removal: more complete OTGrammar_save and restore (affected multiple-chew correctness),
 *     changing the 114.5 in Boersma (Phonology 2003) to 118.1
 * pb 2004/08/09 suppressed superset violation in case of identical constraint violation patterns such
 *     as for /(L L2) L (L2 L) (L1 L)/ and /(L2 L) L (L L2) (L1 L)/, thus restricting the warning to cases
 *     of *strict* superset violations
 * pb 2004/08/11 repaired memory leak in OTGrammarTableau_removeCandidate_unstripped
 * pb 2004/09/10 monitor rankings during learning from PairDistribution or Distributions
 * pb 2004/10/16 struct structOTxx
 * pb 2005/01/24 write to headerless spreadsheet file
 * pb 2005/04/19 OTHistory
 * pb 2005/06/30 learning from partial pairs
 * pb 2005/12/11 OTGrammar_honourlocalRankings: 
 * pb 2005/12/11 OTGrammar_PairDistribution_listObligatoryRankings (depth 1)
 * pb 2006/01/05 new decision strategies: HarmonicGrammar and LinearOT
 * pb 2006/01/21 better procedure name
 * pb 2006/02/02 new decision strategy: ExponentialHG
 * pb 2006/12/08 MelderInfo
 * pb 2007/04/22 multiply learning step by number of violations (for HarmonicGrammar and LinearOT)
 * pb 2007/04/25 new decision strategy: MaximumEntropy
 * pb 2007/04/30 many improvements
 * pb 2007/05/20 new decision strategy: PositiveHG
 * pb 2007/06/21 corrected PositiveHG
 * pb 2007/06/21 made spreadsheet file readable as Table
 * pb 2007/07/24 leak and constraint plasticity
 * pb 2007/07/27 leak and constraint plasticity also written...
 * pb 2007/08/08 wchar
 * pb 2007/10/01 can write as encoding
 * pb 2008/03/03 EDCD with vacation
 * pb 2008/03/07 Demote one with vacation
 * pb 2008/03/07 Reset to random total ranking
 * pb 2008/03/27 Exponential HG: reset average weight to zero after every change
 * pb 2008/03/28 Exponential HG: set update rule to HG-GLA rather than OT-GLA
 * pb 2008/03/31 OTGrammar_PairDistribution_findPositiveWeights
 * pb 2008/04/08 made (OTGrammar & Distributions) learnFromPartialOutputs and getFractionCorrect five times faster
 * pb 2008/04/12 split off NUMlinprog
 * pb 2008/05/31 new decision strategy: ExponentialMaximumEntropy
 * pb 2009/03/09 new update rule: Weighted all up, highest down
 * pb 2009/07/07 OTGrammar_PairDistribution_getMinimumNumberCorrect
 * pb 2010/06/05 corrected colours
 * pb 2011/03/01 renamed "strategy" to "updateRule", "meanLearningStep" to "plasticity", "rankingSpreading" to "evaluationNoise"
 * pb 2011/03/22 C++
 * pb 2011/04/27 Melder_debug 41 and 42
 * pb 2011/07/14 C++
 * pb 2014/02/27 skippable symmetric all
 * pb 2014/07/25 RRIP
 */

#include "OTGrammar.h"

#include "oo_DESTROY.h"
#include "OTGrammar_def.h"
#include "oo_COPY.h"
#include "OTGrammar_def.h"
#include "oo_EQUAL.h"
#include "OTGrammar_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "OTGrammar_def.h"
#include "oo_WRITE_BINARY.h"
#include "OTGrammar_def.h"
#include "oo_READ_BINARY.h"
#include "OTGrammar_def.h"
#include "oo_DESCRIPTION.h"
#include "OTGrammar_def.h"

#include "enums_getText.h"
#include "OTGrammar_enums.h"
#include "enums_getValue.h"
#include "OTGrammar_enums.h"

void structOTGrammar :: v_info ()
{
	structDaata :: v_info ();
	integer numberOfCandidates = 0, numberOfViolations = 0;
	for (integer itab = 1; itab <= numberOfTableaus; itab ++) {
		numberOfCandidates += our tableaus [itab]. numberOfCandidates;
		for (integer icand = 1; icand <= our tableaus [itab]. numberOfCandidates; icand ++)
			for (integer icons = 1; icons <= our numberOfConstraints; icons ++)
				numberOfViolations += our tableaus [itab]. candidates [icand]. marks [icons];
	}
	MelderInfo_writeLine (U"Decision strategy: ", kOTGrammar_decisionStrategy_getText (decisionStrategy));
	MelderInfo_writeLine (U"Number of constraints: ", numberOfConstraints);
	MelderInfo_writeLine (U"Number of tableaus: ", numberOfTableaus);
	MelderInfo_writeLine (U"Number of candidates: ", numberOfCandidates);
	MelderInfo_writeLine (U"Number of violation marks: ", numberOfViolations);
}

void structOTGrammar :: v_writeText (MelderFile file) {
	MelderFile_write (file, U"\n<", kOTGrammar_decisionStrategy_getText (decisionStrategy),
		U">\n", leak, U" ! leak\n", our numberOfConstraints, U" constraints");
	for (integer icons = 1; icons <= our numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & constraints [icons];
		MelderFile_write (file, U"\nconstraint [", icons, U"]: \"");
		for (const char32 *p = & constraint -> name [0]; *p; p ++) {
			if (*p == U'\"')
				MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", constraint -> ranking,
			U" ", constraint -> disharmony, U" ", constraint -> plasticity, U" ! ");
		for (const char32 *p = & constraint -> name [0]; *p; p ++) {
			if (*p == U'\n')
				MelderFile_writeCharacter (file, U' ');
			else if (*p == U'\\' && p [1] == U's' && p [2] == U'{')
				p += 2;
			else if (*p == U'}')
				{ }
			else
				MelderFile_writeCharacter (file, *p);
		}
	}
	MelderFile_write (file, U"\n\n", our numberOfFixedRankings, U" fixed rankings");
	for (integer irank = 1; irank <= our numberOfFixedRankings; irank ++) {
		OTGrammarFixedRanking fixedRanking = & our fixedRankings [irank];
		MelderFile_write (file, U"\n   ", fixedRanking -> higher, U" ", fixedRanking -> lower);
	}
	MelderFile_write (file, U"\n\n", our numberOfTableaus, U" tableaus");
	for (integer itab = 1; itab <= our numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & our tableaus [itab];
		MelderFile_write (file, U"\ninput [", itab, U"]: \"");
		for (const char32 *p = & tableau -> input [0]; *p; p ++) {
			if (*p == U'\"')
				MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", tableau -> numberOfCandidates);
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			MelderFile_write (file, U"\n   candidate [", icand, U"]: \"");
			for (const char32 *p = & candidate -> output [0]; *p; p ++) {
				if (*p == U'\"')
					MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
				MelderFile_writeCharacter (file, *p);
			}
			MelderFile_writeCharacter (file, U'\"');
			for (integer icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				MelderFile_write (file, U" ", candidate -> marks [icons]);
			}
		}
	}
}

void OTGrammar_checkIndex (OTGrammar me) {
	if (my index.size != 0) return;
	my index = to_INTVEC (my numberOfConstraints);
	OTGrammar_sort (me);
}

void structOTGrammar :: v_readText (MelderReadText text, int formatVersion) {
	OTGrammar_Parent :: v_readText (text, formatVersion);
	if (formatVersion >= 1) {
		try {
			our decisionStrategy = (kOTGrammar_decisionStrategy) texgete8 (text, (enum_generic_getValue) kOTGrammar_decisionStrategy_getValue);
		} catch (MelderError) {
			Melder_throw (U"Trying to read decision strategy.");
		}
	}
	if (formatVersion >= 2) {
		try {
			our leak = texgetr64 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read leak.");
		}
	}
	try {
		our numberOfConstraints = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of constraints.");
	}
	if (our numberOfConstraints < 1)
		Melder_throw (U"No constraints.");
	our constraints = newvectorzero <structOTGrammarConstraint> (our numberOfConstraints);
	for (integer icons = 1; icons <= our numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & constraints [icons];
		try {
			constraint -> name = texgetw16 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read name of constraint ", icons, U".");
		}
		try {
			constraint -> ranking = texgetr64 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read ranking of constraint ", icons, U".");
		}
		try {
			constraint -> disharmony = texgetr64 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read disharmony of constraint ", icons, U".");
		}
		if (formatVersion < 2) {
			constraint -> plasticity = 1.0;
		} else {
			try {
				constraint -> plasticity = texgetr64 (text);
			} catch (MelderError) {
				Melder_throw (U"Trying to read plasticity of constraint ", icons, U".");
			}
		}
	}
	try {
		our numberOfFixedRankings = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of fixed rankings.");
	}
	if (our numberOfFixedRankings >= 1) {
		our fixedRankings = newvectorzero <structOTGrammarFixedRanking> (numberOfFixedRankings);
		for (integer irank = 1; irank <= our numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & our fixedRankings [irank];
			try {
				fixedRanking -> higher = texgeti32 (text);
			} catch (MelderError) {
				Melder_throw (U"Trying to read the higher of constraint pair ", irank, U".");
			}
			try {
				fixedRanking -> lower = texgeti32 (text);
			} catch (MelderError) {
				Melder_throw (U"Trying to read the lower of constraint pair ", irank, U".");
			}
		}
	}
	try {
		our numberOfTableaus = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of tableaus.");
	}
	if (our numberOfTableaus < 1)
		Melder_throw (U"No tableaus.");
	our tableaus = newvectorzero <structOTGrammarTableau> (numberOfTableaus);
	for (integer itab = 1; itab <= our numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & our tableaus [itab];
		try {
			tableau -> input = texgetw16 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read input of tableau ", itab, U".");
		}
		try {
			tableau -> numberOfCandidates = texgeti32 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read number of candidates of tableau ", itab, U".");
		}
		Melder_require (tableau -> numberOfCandidates > 0,
			U"No candidates in tableau ", itab,
			U" (input: ", tableau -> input.get(), U")"
			U" in line ", MelderReadText_getLineNumber (text),
			itab == 1 ? U"." : U", or perhaps wrong number of candidates for input «",
			itab == 1 ? nullptr : our tableaus [itab - 1]. input.get(),
			itab == 1 ? nullptr : U"»."
		);
		tableau -> candidates = newvectorzero <structOTGrammarCandidate> (tableau -> numberOfCandidates);
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			try {
				candidate -> output = texgetw16 (text);
			} catch (MelderError) {
				Melder_throw (U"Trying to read candidate ", icand, U" of tableau ", itab,
					U" (input: ", tableau -> input.get(), U") in line ", MelderReadText_getLineNumber (text), U".");
			}
			candidate -> numberOfConstraints = numberOfConstraints;   // redundancy, needed for writing binary
			candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints);
			for (integer icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				try {
					candidate -> marks [icons] = texgeti16 (text);
				} catch (MelderError) {
					Melder_throw
					(U"Trying to read number of violations of constraint ", icons,
					 U" (", our constraints [icons]. name.get(), U")"
					 U" of candidate ", icand,
					 U" (", candidate -> output.get(), U")"
					 U" of tableau ", itab,
					 U" (input: ", tableau -> input.get(), U")"
					 U" in line ", MelderReadText_getLineNumber (text), U".");
				}
			}
		}
	}
	OTGrammar_checkIndex (this);
}

Thing_implement (OTGrammar, Daata, 2);

Thing_implement (OTHistory, TableOfReal, 0);

static OTGrammar constraintCompare_grammar;

static int constraintCompare (const void *first, const void *second) {
	OTGrammar me = constraintCompare_grammar;
	integer icons = * (integer *) first, jcons = * (integer *) second;
	OTGrammarConstraint ci = & my constraints [icons], cj = & my constraints [jcons];
	/*
		Sort primarily by disharmony.
	*/
	if (ci -> disharmony > cj -> disharmony)
		return -1;
	if (ci -> disharmony < cj -> disharmony)
		return +1;
	/*
		Tied constraints are sorted alphabetically.
	*/
	return str32cmp (my constraints [icons]. name.get(), my constraints [jcons]. name.get());
}

void OTGrammar_sort (OTGrammar me) {
	constraintCompare_grammar = me;
	qsort (& my index [1], my numberOfConstraints, sizeof (integer), constraintCompare);
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> tiedToTheLeft = ( icons > 1 &&
			my constraints [my index [icons - 1]]. disharmony == constraint -> disharmony );
		constraint -> tiedToTheRight = ( icons < my numberOfConstraints &&
			my constraints [my index [icons + 1]]. disharmony == constraint -> disharmony );
	}
}

void OTGrammar_newDisharmonies (OTGrammar me, double spreading) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, spreading)
			/*NUMrandomUniform (-spreading, spreading)*/;
	}
	OTGrammar_sort (me);
}

integer OTGrammar_getTableau (OTGrammar me, conststring32 input) {
	for (integer itab = 1; itab <= my numberOfTableaus; itab ++)
		if (str32equ (my tableaus [itab]. input.get(), input))
			return itab;
	Melder_throw (U"Input \"", input, U"\" not in list of tableaus.");
}

static void _OTGrammar_fillInHarmonies (OTGrammar me, integer itab) noexcept {
	if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY)
		return;
	OTGrammarTableau tableau = & my tableaus [itab];
	for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		INTVEC marks = candidate -> marks.get();
		longdouble disharmony = 0.0;
		if (my decisionStrategy == kOTGrammar_decisionStrategy::HARMONIC_GRAMMAR ||
			my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY)
		{
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				disharmony += my constraints [icons]. disharmony * marks [icons];
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				disharmony += exp (my constraints [icons]. disharmony) * marks [icons];
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy::LINEAR_OT) {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				if (my constraints [icons]. disharmony > 0.0)
					disharmony += my constraints [icons]. disharmony * marks [icons];
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy::POSITIVE_HG) {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const double constraintDisharmony = std::max (my constraints [icons]. disharmony, 1.0);
				disharmony += constraintDisharmony * marks [icons];
			}
		} else {
			Melder_fatal (U"_OTGrammar_fillInHarmonies: unimplemented decision strategy.");
		}
		candidate -> harmony = - (double) disharmony;
	}
}

int OTGrammar_compareCandidates (OTGrammar me, integer itab1, integer icand1, integer itab2, integer icand2) noexcept {
	INTVEC marks1 = my tableaus [itab1]. candidates [icand1]. marks.get();
	INTVEC marks2 = my tableaus [itab2]. candidates [icand2]. marks.get();
	if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			integer numberOfMarks1 = marks1 [my index [icons]];
			integer numberOfMarks2 = marks2 [my index [icons]];
			/*
				Count tied constraints as one.
			*/
			while (my constraints [my index [icons]]. tiedToTheRight) {
				icons ++;
				numberOfMarks1 += marks1 [my index [icons]];
				numberOfMarks2 += marks2 [my index [icons]];
			}
			if (numberOfMarks1 < numberOfMarks2)
				return -1;   // candidate 1 is better than candidate 2
			if (numberOfMarks1 > numberOfMarks2)
				return +1;   // candidate 2 is better than candidate 1
		}
		/*
			If we arrive here, none of the comparisons found a difference
			between the two candidates. Hence, they are equally good.
		*/
		return 0;
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy::HARMONIC_GRAMMAR ||
		my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
			disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2)
			return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2)
			return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy::LINEAR_OT) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my constraints [icons]. disharmony > 0.0) {
				disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
				disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
			}
		}
		if (disharmony1 < disharmony2)
			return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2)
			return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
		my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += exp (my constraints [icons]. disharmony) * marks1 [icons];
			disharmony2 += exp (my constraints [icons]. disharmony) * marks2 [icons];
		}
		if (disharmony1 < disharmony2)
			return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2)
			return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy::POSITIVE_HG) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			double constraintDisharmony = std::max (my constraints [icons]. disharmony, 1.0);
			disharmony1 += constraintDisharmony * marks1 [icons];
			disharmony2 += constraintDisharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2)
			return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2)
			return +1;   // candidate 2 is better than candidate 1
	} else
		Melder_fatal (U"Unimplemented decision strategy.");
	return 0;   // the two total disharmonies are equal
}

static void _OTGrammar_fillInProbabilities (OTGrammar me, integer itab) noexcept {
	OTGrammarTableau tableau = & my tableaus [itab];
	double maximumHarmony = tableau -> candidates [1]. harmony;
	for (integer icand = 2; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		if (candidate -> harmony > maximumHarmony)
			maximumHarmony = candidate -> harmony;
	}
	for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		candidate -> probability = exp (candidate -> harmony - maximumHarmony);
		Melder_assert (candidate -> probability >= 0.0 && candidate -> probability <= 1.0);
	}
	longdouble sumOfProbabilities = 0.0;
	for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		sumOfProbabilities += candidate -> probability;
	}
	Melder_assert (sumOfProbabilities > 0.0);   // because at least one of them is 1.0
	for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		candidate -> probability /= double (sumOfProbabilities);
	}
}

integer OTGrammar_getWinner (OTGrammar me, integer itab) noexcept {
	integer icand_best = 1;
	if (my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		_OTGrammar_fillInHarmonies (me, itab);
		_OTGrammar_fillInProbabilities (me, itab);
		double cutOff = NUMrandomUniform (0.0, 1.0);
		longdouble sumOfProbabilities = 0.0;
		for (integer icand = 1; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
			sumOfProbabilities += my tableaus [itab]. candidates [icand]. probability;
			if (sumOfProbabilities > cutOff) {
				icand_best = icand;
				break;
			}
		}
	} else {
		integer numberOfBestCandidates = 1;
		for (integer icand = 2; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
			int comparison = OTGrammar_compareCandidates (me, itab, icand, itab, icand_best);
			if (comparison == -1) {
				icand_best = icand;   // the current candidate is the unique best candidate found so far
				numberOfBestCandidates = 1;
			} else if (comparison == 0) {
				numberOfBestCandidates += 1;   // the current candidate is equally good as the best found before
				/*
					Give all candidates that are equally good an equal chance to become the winner.
				*/
				if (Melder_debug == 41) {
					icand_best = icand_best;   // keep first
				} else if (Melder_debug == 42) {
					icand_best = icand;   // take last
				} else if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {   // default: take random
					icand_best = icand;
				}
			}
		}
	}
	return icand_best;
}

integer OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, integer itab) {
	if (my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY) return 1;
	integer icand_best = 1, numberOfBestCandidates = 1;
	for (integer icand = 2; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
		int comparison = OTGrammar_compareCandidates (me, itab, icand, itab, icand_best);
		if (comparison == -1) {
			icand_best = icand;   // the current candidate is the best candidate found so far
			numberOfBestCandidates = 1;
		} else if (comparison == 0) {
			numberOfBestCandidates += 1;   // the current candidate is equally good as the best found before
		}
	}
	return numberOfBestCandidates;
}

bool OTGrammar_isCandidateGrammatical (OTGrammar me, integer itab, integer icand) {
	for (integer jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++)
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) < 0)
			return false;
	return true;
}

bool OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, integer itab, integer icand) {
	for (integer jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++)
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) <= 0)
			return false;
	return true;
}

void OTGrammar_getInterpretiveParse (OTGrammar me, conststring32 partialOutput, integer *out_bestTableau, integer *out_bestCandidate) {
	try {
		integer itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				if (str32str (cand -> output.get(), partialOutput)) {   // T&S' idea of surface->overt mapping
					if (itab_best == 0) {
						itab_best = itab;   // the first compatible input/output pair found is the first guess for the best candidate
						icand_best = icand;
						numberOfBestCandidates = 1;
					} else {
						int comparison = OTGrammar_compareCandidates (me, itab, icand, itab_best, icand_best);
						if (comparison == -1) {
							itab_best = itab;   // the current input/output pair is the best candidate found so far
							icand_best = icand;
							numberOfBestCandidates = 1;
						} else if (comparison == 0) {
							numberOfBestCandidates += 1;   // the current input/output pair is equally good as the best found before
							/*
							 * Give all candidates that are equally good an equal chance to become the winner.
							 */
							if (Melder_debug == 41) {
								itab_best = itab_best;
								icand_best = icand_best;   // keep first
							} else if (Melder_debug == 42) {
								itab_best = itab;
								icand_best = icand;   // take last
							} else if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {   // default: take random
								itab_best = itab;
								icand_best = icand;
							}
						}
					}
				}
			}
		}
		if (itab_best == 0)
			Melder_throw (U"The partial output \"", partialOutput, U"\" does not match any candidate for any input form.");
		if (out_bestTableau)
			*out_bestTableau = itab_best;
		if (out_bestCandidate)
			*out_bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw (U"Interpretive parse not computed.");
	}
}

static void OTGrammar_getInterpretiveParse_opt (OTGrammar me, integer ipartialOutput, integer *out_bestTableau, integer *out_bestCandidate) {
	try {
		integer itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				if (cand -> partialOutputMatches [ipartialOutput]) {   // T&S' idea of surface->overt mapping
					if (itab_best == 0) {
						itab_best = itab;   // the first compatible input/output pair found is the first guess for the best candidate
						icand_best = icand;
						numberOfBestCandidates = 1;
					} else {
						int comparison = OTGrammar_compareCandidates (me, itab, icand, itab_best, icand_best);
						if (comparison == -1) {
							itab_best = itab;   // the current input/output pair is the best candidate found so far
							icand_best = icand;
							numberOfBestCandidates = 1;
						} else if (comparison == 0) {
							numberOfBestCandidates += 1;   // the current input/output pair is equally good as the best found before
							/*
							 * Give all candidates that are equally good an equal chance to become the winner.
							 */
							if (Melder_debug == 41) {
								itab_best = itab_best;
								icand_best = icand_best;   // keep first
							} else if (Melder_debug == 42) {
								itab_best = itab;
								icand_best = icand;   // take last
							} else if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {   // default: take random
								itab_best = itab;
								icand_best = icand;
							}
						}
					}
				}
			}
		}
		Melder_assert (itab_best != 0);
		if (out_bestTableau)
			*out_bestTableau = itab_best;
		if (out_bestCandidate)
			*out_bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw (U"Interpretive parse not computed.");
	}
}

bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, conststring32 partialOutput) {
	for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (str32str (tableau -> candidates [icand]. output.get(), partialOutput))
				if (OTGrammar_isCandidateGrammatical (me, itab, icand))
					return true;
		}
	}
	return false;
}

bool OTGrammar_areAllPartialOutputsGrammatical (OTGrammar me, Strings thee) {
	for (integer ioutput = 1; ioutput <= thy numberOfStrings; ioutput ++) {
		conststring32 partialOutput = thy strings [ioutput].get();
		if (! OTGrammar_isPartialOutputGrammatical (me, partialOutput))
			return false;
	}
	return true;
}

bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, conststring32 partialOutput) {
	bool found = false;
	for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (str32str (tableau -> candidates [icand]. output.get(), partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					found = true;
					/*
						All other grammatical candidates should match.
					*/
					for (integer jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
						if (OTGrammar_compareCandidates (me, itab, jcand, itab, icand) == 0)
							if (! str32str (tableau -> candidates [jcand]. output.get(), partialOutput))
								return false;   // partial output is multiply optimal
					}
				}
			}
		}
	}
	return found;
}

bool OTGrammar_areAllPartialOutputsSinglyGrammatical (OTGrammar me, Strings thee) {
	for (integer ioutput = 1; ioutput <= thy numberOfStrings; ioutput ++) {
		conststring32 partialOutput = thy strings [ioutput].get();
		if (! OTGrammar_isPartialOutputSinglyGrammatical (me, partialOutput))
			return false;
	}
	return true;
}

static integer OTGrammar_crucialCell (OTGrammar me, integer itab, integer icand, integer iwinner, integer numberOfOptimalCandidates) {
	OTGrammarTableau tableau = & my tableaus [itab];
	if (tableau -> numberOfCandidates < 2) return 0;   // if there is only one candidate, all cells can be greyed
	if (my decisionStrategy != kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) return my numberOfConstraints;   // nothing grey
	if (OTGrammar_compareCandidates (me, itab, icand, itab, iwinner) == 0) {   // candidate equally good as winner?
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			integer secondBest = 0;
			for (integer jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
				if (OTGrammar_compareCandidates (me, itab, jcand, itab, iwinner) != 0) {   // a non-optimal candidate?
					if (secondBest == 0) {
						secondBest = jcand;   // first guess
					} else if (OTGrammar_compareCandidates (me, itab, jcand, itab, secondBest) < 0) {
						secondBest = jcand;   // better guess
					}
				}
			}
			if (secondBest == 0) return 0;   // if all candidates are equally good, all cells can be greyed
			return OTGrammar_crucialCell (me, itab, secondBest, iwinner, 1);
		}
	} else {
		const constINTVEC candidateMarks = tableau -> candidates [icand]. marks.get();
		const constINTVEC winnerMarks = tableau -> candidates [iwinner]. marks.get();
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			integer numberOfCandidateMarks = candidateMarks [my index [icons]];
			integer numberOfWinnerMarks = winnerMarks [my index [icons]];
			while (my constraints [my index [icons]]. tiedToTheRight) {
				icons ++;
				numberOfCandidateMarks += candidateMarks [my index [icons]];
				numberOfWinnerMarks += winnerMarks [my index [icons]];
			}
			if (numberOfCandidateMarks > numberOfWinnerMarks)
				return icons;
		}
	}
	return my numberOfConstraints;   // nothing grey
}

static double OTGrammar_constraintWidth (Graphics g, conststring32 name) {
	char32 text [100];
	str32cpy (text, name);
	char32 *newLine = str32chr (text, U'\n');
	if (newLine) {
		double firstWidth, secondWidth;
		*newLine = U'\0';
		firstWidth = Graphics_textWidth (g, text);
		secondWidth = Graphics_textWidth (g, newLine + 1);
		return firstWidth > secondWidth ? firstWidth : secondWidth;
	}
	return Graphics_textWidth (g, text);
}

void OTGrammar_drawTableau (OTGrammar me, Graphics g, bool vertical, conststring32 input) {
	try {
		const double fontSize = Graphics_inqFontSize (g);
		MelderColour colour = Graphics_inqColour (g);
		const integer itab = OTGrammar_getTableau (me, input);
		_OTGrammar_fillInHarmonies (me, itab);
		const integer winner = OTGrammar_getWinner (me, itab);
		
		Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
		const double margin = Graphics_dxMMtoWC (g, 1.0);
		const double fingerWidth = Graphics_dxMMtoWC (g, 7.0) * fontSize / 12.0;
		const double doubleLineDx = Graphics_dxMMtoWC (g, 0.9);
		const double doubleLineDy = Graphics_dyMMtoWC (g, 0.9);
		const double rowHeight = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72);
		const double descent = rowHeight * 0.5;
		const double worldAspectRatio = Graphics_dyMMtoWC (g, 1.0) / Graphics_dxMMtoWC (g, 1.0);   // because Graphics_textWidth measures in the x direction only
		/*
			Compute the height of the header row.
		*/
		double headerHeight;
		if (vertical) {
			headerHeight = 0.0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTGrammarConstraint constraint = & my constraints [icons];
				const double constraintTextWidth = Graphics_textWidth (g, constraint -> name.get());
				if (constraintTextWidth > headerHeight)
					headerHeight = constraintTextWidth;
			}
			headerHeight += margin * 2;
			headerHeight *= worldAspectRatio;
		} else {
			headerHeight = rowHeight;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				if (str32chr (constraint -> name.get(), U'\n')) {
					headerHeight *= 1.6;
					break;
				}
			}
		}
		/*
			Compute longest candidate string.
			Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
		*/
		double candWidth = Graphics_textWidth (g, input);
		OTGrammarTableau tableau = & my tableaus [itab];
		integer numberOfOptimalCandidates = 0;
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			double width = Graphics_textWidth (g, tableau -> candidates [icand]. output.get());
			if (OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0) {
				width += fingerWidth;
				numberOfOptimalCandidates ++;
			}
			if (width > candWidth)
				candWidth = width;
		}
		candWidth += margin * 3;
		/*
			Compute tableau width.
		*/
		double tableauWidth = candWidth + doubleLineDx;
		if (vertical) {
			tableauWidth += rowHeight * my numberOfConstraints / worldAspectRatio;
		} else {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				tableauWidth += OTGrammar_constraintWidth (g, constraint -> name.get());
			}
			tableauWidth += margin * 2 * my numberOfConstraints;
		}
		/*
			Draw box.
		*/
		double x = doubleLineDx;   // left side of tableau
		double y = 1.0 - doubleLineDy;
		Graphics_rectangle (g, x, x + tableauWidth,
				y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
		/*
			Draw input.
		*/
		y -= headerHeight;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x + 0.5 * candWidth, y + 0.5 * headerHeight, input);
		Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
		/*
			Draw constraint names.
		*/
		x += candWidth + doubleLineDx;
		if (vertical)
			Graphics_setTextRotation (g, 90.0);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			double width = vertical ? rowHeight / worldAspectRatio : OTGrammar_constraintWidth (g, constraint -> name.get()) + margin * 2;
			if (str32chr (constraint -> name.get(), U'\n') && ! vertical) {
				autoMelderString text;
				MelderString_copy (& text, constraint -> name.get());
				char32 *newLine = str32chr (text.string, U'\n');
				*newLine = U'\0';
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
				Graphics_text (g, x + 0.5 * width, y + headerHeight, text.string);
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
				Graphics_text (g, x + 0.5 * width, y, newLine + 1);
			} else if (vertical) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				Graphics_text (g, x + 0.5 * width, y + margin, constraint -> name.get());
			} else {
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
				Graphics_text (g, x + 0.5 * width, y + 0.5 * headerHeight, constraint -> name.get());
			}
			if (constraint -> tiedToTheLeft)
				Graphics_setLineType (g, Graphics_DOTTED);
			Graphics_line (g, x, y, x, y + headerHeight);
			Graphics_setLineType (g, Graphics_DRAWN);
			Graphics_line (g, x, y, x + width, y);
			x += width;
		}
		if (vertical) Graphics_setTextRotation (g, 0.0);
		/*
			Draw candidates.
		*/
		y -= doubleLineDy;
		for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			integer crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
			bool candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
			/*
				Draw candidate transcription.
			*/
			x = doubleLineDx;
			y -= rowHeight;
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_text (g, x + candWidth - margin, y + descent, tableau -> candidates [icand]. output.get());
			if (candidateIsOptimal) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				Graphics_setFontSize (g, 1.5 * fontSize);
				if (numberOfOptimalCandidates > 1) Graphics_setColour (g, Melder_RED);
				Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 1.0) * fontSize / 12.0, U"☞");
				Graphics_setColour (g, colour);
				Graphics_setFontSize (g, fontSize);
			}
			Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
			/*
				Draw grey cell backgrounds.
			*/
			x = candWidth + 2 * doubleLineDx;
			Graphics_setGrey (g, 0.9);
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer index = my index [icons];
				OTGrammarConstraint constraint = & my constraints [index];
				const double width = ( vertical ? rowHeight / worldAspectRatio :
						OTGrammar_constraintWidth (g, constraint -> name.get()) + margin * 2 );
				if (icons > crucialCell)
					Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
				x += width;
			}
			Graphics_setColour (g, colour);
			/*
				Draw cell marks.
			*/
			x = candWidth + 2 * doubleLineDx;
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer index = my index [icons];
				OTGrammarConstraint constraint = & my constraints [index];
				const double width = vertical ? rowHeight / worldAspectRatio : OTGrammar_constraintWidth (g, constraint -> name.get()) + margin * 2;
				static MelderString markString;
				MelderString_empty (& markString);
				if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
					/*
						An exclamation mark can be drawn in this cell only if all of the following conditions are met:
						1. the candidate is not optimal;
						2. the constraint is not tied;
						3. this is the crucial cell, i.e. the cells after it are drawn in grey.
					*/
					if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
						const integer winnerMarks = tableau -> candidates [winner]. marks [index];
						for (integer imark = 1; imark <= winnerMarks + 1; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						for (integer imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							MelderString_appendCharacter (& markString, U'+');
						MelderString_appendCharacter (& markString, U'!');
						for (integer imark = winnerMarks + 2; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							Graphics_setColour (g, Melder_RED);
						}
						for (integer imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						for (integer imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							MelderString_appendCharacter (& markString, U'+');
					}
				} else {
					for (integer imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
						MelderString_appendCharacter (& markString, U'*');
					for (integer imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
						MelderString_appendCharacter (& markString, U'+');
				}
				Graphics_text (g, x + 0.5 * width, y + descent, markString.string);
				Graphics_setColour (g, colour);
				if (constraint -> tiedToTheLeft)
					Graphics_setLineType (g, Graphics_DOTTED);
				Graphics_line (g, x, y, x, y + rowHeight);
				Graphics_setLineType (g, Graphics_DRAWN);
				Graphics_line (g, x, y + rowHeight, x + width, y + rowHeight);
				x += width;
			}
			/*
				Draw harmony.
			*/
			if (my decisionStrategy != kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				const double value = tableau -> candidates [icand]. harmony;
				if (my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
					my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
				{
					Graphics_text (g, x, y + descent, Melder_float (Melder_half (value)));
				} else {
					Graphics_text (g, x, y + descent, Melder_fixed (value, 3));
				}
			}
		}
		/*
			Draw box.
		*/
		x = doubleLineDx;   // left side of tableau
		y = 1.0 - doubleLineDy;
		Graphics_rectangle (g, x, x + tableauWidth,
			y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
	} catch (MelderError) {
		Melder_throw (me, U": tableau not drawn.");
	}
}

autoStrings OTGrammar_generateInputs (OTGrammar me, integer numberOfTrials) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (thy numberOfStrings = numberOfTrials);
		for (integer i = 1; i <= numberOfTrials; i ++) {
			integer itab = NUMrandomInteger (1, my numberOfTableaus);
			thy strings [i] = Melder_dup (my tableaus [itab]. input.get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": inputs not generated.");
	}
}

autoStrings OTGrammar_getInputs (OTGrammar me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (thy numberOfStrings = my numberOfTableaus);
		for (integer i = 1; i <= my numberOfTableaus; i ++)
			thy strings [i] = Melder_dup (my tableaus [i]. input.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": inputs not gotten.");
	}
}

autostring32 OTGrammar_inputToOutput (OTGrammar me, conststring32 input, double evaluationNoise) {
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		integer itab = OTGrammar_getTableau (me, input);
		integer winner = OTGrammar_getWinner (me, itab);
		if (winner == 0)
			Melder_throw (U"No winner");
		return Melder_dup (my tableaus [itab]. candidates [winner]. output.get());
	} catch (MelderError) {
		Melder_throw (me, U": output not computed from input \"", input, U"\".");
	}
}

autoStrings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise) {
	try {
		autoStrings him = Thing_new (Strings);
		integer n = inputs -> numberOfStrings;
		his numberOfStrings = n;
		his strings = autoSTRVEC (n);
		for (integer i = 1; i <= n; i ++)
			his strings [i] = OTGrammar_inputToOutput (me, inputs -> strings [i].get(), evaluationNoise);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": outputs not computed.");
	}
}

autoStrings OTGrammar_inputToOutputs (OTGrammar me, conststring32 input, integer n, double evaluationNoise) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = n;
		thy strings = autoSTRVEC (n);
		for (integer i = 1; i <= n; i ++)
			thy strings [i] = OTGrammar_inputToOutput (me, input, evaluationNoise);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output not computed.");
	}
}

autoDistributions OTGrammar_to_Distribution (OTGrammar me, integer trialsPerInput, double noise) {
	try {
		integer totalNumberOfOutputs = 0, nout = 0;
		/*
			Count the total number of outputs.
		*/
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++)
			totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
		/*
			Create the distribution. One row for every output form.
		*/
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1);
		/*
			Measure every input form.
		*/
		autoMelderProgress progress (U"OTGrammar: compute output distribution.");
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input.get(), U"\"");
			/*
				Set the row labels to the output strings.
			*/
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				thy rowLabels [nout + icand] = Melder_dup (
					Melder_cat (tableau -> input.get(), U" \\-> ", tableau -> candidates [icand]. output.get())
				);
			}
			/*
				Compute a number of outputs and store the results.
			*/
			for (integer itrial = 1; itrial <= trialsPerInput; itrial ++) {
				OTGrammar_newDisharmonies (me, noise);
				integer iwinner = OTGrammar_getWinner (me, itab);
				thy data [nout + iwinner] [1] += 1;
			}
			/*
				Update the offset.
			*/
			nout += tableau -> numberOfCandidates;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output distribution not computed.");
	}
}

autoPairDistribution OTGrammar_to_PairDistribution (OTGrammar me, integer trialsPerInput, double noise) {
	try {
		integer nout = 0;
		/*
			Create the distribution. One row for every output form.
		*/
		autoPairDistribution thee = PairDistribution_create ();
		/*
			Measure every input form.
		*/
		autoMelderProgress progress (U"OTGrammar: compute output distribution.");
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input.get(), U"\"");
			/*
				Copy the input and output strings to the target object.
			*/
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				PairDistribution_add (thee.get(), tableau -> input.get(), tableau -> candidates [icand]. output.get(), 0.0);
			}
			/*
				Compute a number of outputs and store the results.
			*/
			for (integer itrial = 1; itrial <= trialsPerInput; itrial ++) {
				OTGrammar_newDisharmonies (me, noise);
				integer iwinner = OTGrammar_getWinner (me, itab);
				thy pairs.at [nout + iwinner] -> weight += 1.0;
			}
			/*
				Update the offset.
			*/
			nout += tableau -> numberOfCandidates;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output distribution not computed.");
	}
}

static bool honoursFixedRankings (OTGrammar me) {
	for (integer i = 1; i <= my numberOfFixedRankings; i ++) {
		integer higher = my fixedRankings [i]. higher, lower = my fixedRankings [i]. lower;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my index [icons] == higher)
				break;   // detected higher before lower: OK
			if (my index [icons] == lower)
				return false;
		}
	}
	return true;
}

autoDistributions OTGrammar_measureTypology_WEAK (OTGrammar me) {
	try {
		integer totalNumberOfOutputs = 0, nout = 0, nperm, factorial [1+12];
		if (my numberOfConstraints > 12)
			Melder_throw (U"Cannot handle more than 12 constraints.");
		factorial [0] = 1;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			factorial [icons] = factorial [icons - 1] * icons;
		nperm = factorial [my numberOfConstraints];
		/*
			Count the total number of outputs.
		*/
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++)
			totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
		/*
			Create the distribution. One row for every output form.
		*/
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1);
		/*
			Measure every input form.
		*/
		autoMelderProgress progress (U"Measuring typology...");
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input.get(), U"\"");
			/*
				Set the row labels to the output strings.
			*/
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				thy rowLabels [nout + icand] = Melder_dup (
					Melder_cat (tableau -> input.get(), U" \\-> ", tableau -> candidates [icand]. output.get())
				);
			}
			/*
				Compute a number of outputs and store the results.
			*/
			for (integer iperm = 0; iperm < nperm; iperm ++) {
				integer permleft = iperm, iwinner;
				/* Initialize to 12345 before permuting. */
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
					my index [icons] = icons;
				for (integer icons = 1; icons < my numberOfConstraints; icons ++) {
					integer fac = factorial [my numberOfConstraints - icons], shift = permleft / fac, dummy;
					/*
						Swap constraint with the one at a distance 'shift'.
					*/
					dummy = my index [icons];
					my index [icons] = my index [icons + shift];
					my index [icons + shift] = dummy;
					permleft %= fac;
				}
				if (honoursFixedRankings (me)) {
					iwinner = OTGrammar_getWinner (me, itab);
					thy data [nout + iwinner] [1] += 1;
				}
			}
			/*
				Update the offset.
			*/
			nout += tableau -> numberOfCandidates;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": typology not measured.");
	}
}

static double learningStep (double mean, double relativeSpreading) {
	return relativeSpreading == 0.0 ? mean : NUMrandomGauss (mean, relativeSpreading * mean);
}

static void OTGrammar_honourLocalRankings (OTGrammar me, double plasticity, double relativePlasticityNoise, bool *grammarHasChanged) {
	bool improved;
	do {
		improved = false;
		for (integer irank = 1; irank <= my numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
			OTGrammarConstraint higher = & my constraints [fixedRanking -> higher], lower = & my constraints [fixedRanking -> lower];
			while (higher -> ranking <= lower -> ranking) {
				lower -> ranking -= learningStep (plasticity, relativePlasticityNoise);
				if (grammarHasChanged)
					*grammarHasChanged = true;
				improved = true;
			}
		}
	} while (improved);
}

static void OTGrammar_modifyRankings (OTGrammar me, integer itab, integer iwinner, integer iadult,
	kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, bool warnIfStalled, bool *out_grammarHasChanged)
{
	try {
		OTGrammarTableau tableau = & my tableaus [itab];
		OTGrammarCandidate winner = & tableau -> candidates [iwinner], adult = & tableau -> candidates [iadult];
		double step = learningStep (plasticity, relativePlasticityNoise);
		bool multiplyStepByNumberOfViolations =
			my decisionStrategy == kOTGrammar_decisionStrategy::HARMONIC_GRAMMAR ||
			my decisionStrategy == kOTGrammar_decisionStrategy::LINEAR_OT ||
			my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY ||
			my decisionStrategy == kOTGrammar_decisionStrategy::POSITIVE_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY;
		if (Melder_debug != 0) {
			/*
			 * Perhaps override the standard update rule.
			 */
			if (Melder_debug == 26)
				multiplyStepByNumberOfViolations = false;   // OT-GLA
			else if (Melder_debug == 27)
				multiplyStepByNumberOfViolations = true;   // HG-GLA
		}
		if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ONE) {
			const integer icons = NUMrandomInteger (1, my numberOfConstraints);
			const OTGrammarConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			const integer winnerMarks = winner -> marks [icons];
			const integer adultMarks = adult -> marks [icons];
			if (adultMarks > winnerMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= adultMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
				if (out_grammarHasChanged)
					*out_grammarHasChanged = true;
			}
			if (winnerMarks > adultMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - adultMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
				if (out_grammarHasChanged)
					*out_grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ALL) {
			bool changed = false;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
					changed = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
					changed = true;
				}
			}
			if (changed && my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG)
			{
				longdouble sumOfWeights = 0.0;
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
					sumOfWeights += my constraints [icons]. ranking;
				const double averageWeight = (double) sumOfWeights / my numberOfConstraints;
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
					my constraints [icons]. ranking -= averageWeight;
			}
			if (out_grammarHasChanged)
				*out_grammarHasChanged = changed;
		} else if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ALL_SKIPPABLE) {
			bool changed = false;
			integer winningConstraints = 0, adultConstraints = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks)
					adultConstraints ++;
				if (winnerMarks > adultMarks)
					winningConstraints ++;
			}
			if (winningConstraints != 0) for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
					changed = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
					changed = true;
				}
			}
			if (changed && my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG) {
				longdouble sumOfWeights = 0.0;
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
					sumOfWeights += my constraints [icons]. ranking;
				const double averageWeight = (double) sumOfWeights / my numberOfConstraints;
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
					my constraints [icons]. ranking -= averageWeight;
			}
			if (out_grammarHasChanged)
				*out_grammarHasChanged = changed;
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_UNCANCELLED) {
			integer winningConstraints = 0, adultConstraints = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks)
					adultConstraints ++;
				if (winnerMarks > adultMarks)
					winningConstraints ++;
			}
			if (winningConstraints != 0) for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / adultConstraints;
					//constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) * winningConstraints;
					if (out_grammarHasChanged)
						*out_grammarHasChanged = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					//constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * adultConstraints;
					if (out_grammarHasChanged)
						*out_grammarHasChanged = true;
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL) {
			integer winningConstraints = 0, adultConstraints = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > 0)
					adultConstraints ++;
				if (winnerMarks > 0)
					winningConstraints ++;
			}
			if (winningConstraints != 0) for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (adultMarks > 0) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= adultMarks /*- winnerMarks*/;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / adultConstraints;
					if (out_grammarHasChanged)
						*out_grammarHasChanged = true;
				}
				if (winnerMarks > 0) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks /*- adultMarks*/;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					if (out_grammarHasChanged)
						*out_grammarHasChanged = true;
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::EDCD || updateRule == kOTGrammar_rerankingStrategy::EDCD_WITH_VACATION) {
			/*
				Determine the crucial winner mark.
			*/
			double pivotRanking;
			bool equivalent = true;
			integer icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				const integer adultMarks = adult -> marks [my index [icons]];
				if (adultMarks < winnerMarks)
					break;
				if (adultMarks > winnerMarks)
					equivalent = false;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (warnIfStalled && ! equivalent)
					Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
						U"Input: ", tableau -> input.get(), U"\nCorrect output: ", adult -> output.get(), U"\nLearner's output: ", winner -> output.get());
				return;   // Tesar & Smolensky (2000: 67): "stopped dead in its tracks"
			}
			/*
				Determine the stratum into which some constraints will be demoted.
			*/
			pivotRanking = my constraints [my index [icons]]. ranking;
			if (updateRule == kOTGrammar_rerankingStrategy::EDCD_WITH_VACATION) {
				integer numberOfConstraintsToDemote = 0;
				for (icons = 1; icons <= my numberOfConstraints; icons ++) {
					const integer winnerMarks = winner -> marks [icons];
					const integer adultMarks = adult -> marks [icons];
					if (adultMarks > winnerMarks) {
						const OTGrammarConstraint constraint = & my constraints [icons];
						if (constraint -> ranking >= pivotRanking)
							numberOfConstraintsToDemote += 1;
					}
				}
				if (numberOfConstraintsToDemote > 0) {
					for (icons = 1; icons <= my numberOfConstraints; icons ++) {
						const OTGrammarConstraint constraint = & my constraints [icons];
						if (constraint -> ranking < pivotRanking) {
							constraint -> ranking -= numberOfConstraintsToDemote * step * constraint -> plasticity;
							if (out_grammarHasChanged)
								*out_grammarHasChanged = true;
						}
					}
				}
			}
			/*
				Demote all the uniquely violated constraints in the adult form
				that have rankings not lower than the pivot.
			*/
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				integer numberOfConstraintsDemoted = 0;
				const integer winnerMarks = winner -> marks [my index [icons]];   // for the vacation version, the order is important, therefore indirect
				const integer adultMarks = adult -> marks [my index [icons]];
				if (adultMarks > winnerMarks) {
					const OTGrammarConstraint constraint = & my constraints [my index [icons]];
					const double constraintStep = step * constraint -> plasticity;
					if (constraint -> ranking >= pivotRanking) {
						numberOfConstraintsDemoted += 1;
						constraint -> ranking = pivotRanking - numberOfConstraintsDemoted * constraintStep;   // this preserves the order of the demotees
						if (out_grammarHasChanged)
							*out_grammarHasChanged = true;
					}
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::DEMOTION_ONLY) {
			/*
				Determine the crucial adult mark.
			*/
			integer crucialAdultMark;
			OTGrammarConstraint offendingConstraint;
			integer icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, so we indirect
				const integer adultMarks = adult -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (adultMarks < winnerMarks)
					Melder_throw (U"Demotion-only learning step: Adult form wins! Should never happen.");
				if (adultMarks > winnerMarks)
					break;
			}
			if (icons > my numberOfConstraints)   // completed the loop?
				Melder_throw (U"Adult form equals correct candidate.");
			crucialAdultMark = icons;
			/*
				Demote the highest uniquely violated constraint in the adult form.
			*/
			offendingConstraint = & my constraints [my index [crucialAdultMark]];
			double constraintStep = step * offendingConstraint -> plasticity;
			offendingConstraint -> ranking -= constraintStep;
			if (out_grammarHasChanged)
				*out_grammarHasChanged = true;
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGHEST_DOWN) {
			integer numberOfUp = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (winnerMarks > adultMarks)
					numberOfUp ++;
			}
			if (numberOfUp > 0) {
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
					const OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					const integer winnerMarks = winner -> marks [icons];
					const integer adultMarks = adult -> marks [icons];
					if (winnerMarks > adultMarks) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / numberOfUp;
						if (out_grammarHasChanged)
							*out_grammarHasChanged = true;
					}
				}
				integer winnerMarks = 0, adultMarks = 0;
				integer icons = 1;
				for (; icons <= my numberOfConstraints; icons ++) {
					winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
					adultMarks = adult -> marks [my index [icons]];
					if (my constraints [my index [icons]]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					if (adultMarks < winnerMarks)
						Melder_throw (U"Demotion-only learning step: Adult form wins! Should never happen.");
					if (adultMarks > winnerMarks) break;
				}
				if (icons > my numberOfConstraints)   // completed the loop?
					Melder_throw (U"Adult form equals correct candidate.");
				const integer crucialAdultMark = icons;
				/*
					Demote the highest uniquely violated constraint in the adult form.
				*/
				const OTGrammarConstraint offendingConstraint = & my constraints [my index [crucialAdultMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - adultMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (out_grammarHasChanged)
					*out_grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGHEST_DOWN_2012) {
			integer numberOfUp = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer adultMarks = adult -> marks [icons];
				if (winnerMarks > adultMarks)
					numberOfUp ++;
			}
			if (numberOfUp > 0) {
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
					const OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					const integer winnerMarks = winner -> marks [icons];
					const integer adultMarks = adult -> marks [icons];
					if (winnerMarks > adultMarks) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / (numberOfUp + 1);
						if (out_grammarHasChanged)
							*out_grammarHasChanged = true;
					}
				}
				integer winnerMarks = 0, adultMarks = 0;
				integer icons = 1;
				for (; icons <= my numberOfConstraints; icons ++) {
					winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
					adultMarks = adult -> marks [my index [icons]];
					if (my constraints [my index [icons]]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					if (adultMarks < winnerMarks)
						Melder_throw (U"Demotion-only learning step: Adult form wins! Should never happen.");
					if (adultMarks > winnerMarks) break;
				}
				if (icons > my numberOfConstraints)   // completed the loop?
					Melder_throw (U"Adult form equals correct candidate.");
				const integer crucialAdultMark = icons;
				/*
					Demote the highest uniquely violated constraint in the adult form.
				*/
				const OTGrammarConstraint offendingConstraint = & my constraints [my index [crucialAdultMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - adultMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (out_grammarHasChanged)
					*out_grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGH_DOWN) {
			integer numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				const integer adultMarks = adult -> marks [my index [icons]];
				if (adultMarks < winnerMarks) {
					numberOfUp ++;
				} else if (adultMarks > winnerMarks) {
					if (numberOfUp == 0) {
						numberOfDown ++;
						lowestDemotableConstraint = icons;
					}
				}
			}
			if (warnIfStalled && numberOfDown == 0) {
				Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
					U"Input: ", tableau -> input.get(), U"\nCorrect output: ", adult -> output.get(), U"\nLearner's output: ", winner -> output.get());
				return;
			}
			if (numberOfUp > 0) {
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
					const integer constraintIndex = my index [icons];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					const OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					const integer winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					const integer adultMarks = adult -> marks [constraintIndex];
					if (adultMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 0.0);
					} else if (adultMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations)
								constraintStep *= adultMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (out_grammarHasChanged) *out_grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGH_DOWN_2012) {
			integer numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				const integer adultMarks = adult -> marks [my index [icons]];
				if (adultMarks < winnerMarks) {
					numberOfUp ++;
				} else if (adultMarks > winnerMarks) {
					if (numberOfUp == 0) {
						numberOfDown ++;
						lowestDemotableConstraint = icons;
					}
				}
			}
			if (warnIfStalled && numberOfDown == 0) {
				Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
					U"Input: ", tableau -> input.get(), U"\nCorrect output: ", adult -> output.get(), U"\nLearner's output: ", winner -> output.get());
				return;
			}
			if (numberOfUp > 0) {
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
					const integer constraintIndex = my index [icons];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					const OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					const integer winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					const integer adultMarks = adult -> marks [constraintIndex];
					if (adultMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 1.0);
					} else if (adultMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations)
								constraintStep *= adultMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (out_grammarHasChanged)
					*out_grammarHasChanged = true;
			}
		}
		if (honourLocalRankings && my numberOfFixedRankings)
			OTGrammar_honourLocalRankings (me, plasticity, relativePlasticityNoise, out_grammarHasChanged);
	} catch (MelderError) {
		Melder_throw (me, U": rankings not modified.");
	}
}

void OTGrammar_learnOne (OTGrammar me, conststring32 input, conststring32 adultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, bool newDisharmonies, bool warnIfStalled, bool *out_grammarHasChanged)
{
	try {
		if (newDisharmonies)
			OTGrammar_newDisharmonies (me, evaluationNoise);
		if (out_grammarHasChanged)
			*out_grammarHasChanged = false;

		/*
			Evaluate the input in the learner's hypothesis.
		*/
		integer itab = OTGrammar_getTableau (me, input);
		OTGrammarTableau tableau = & my tableaus [itab];

		/*
			Determine the "winner", i.e. the candidate that wins in the learner's grammar
			(Tesar & Smolensky call this the "loser").
		*/
		integer iwinner = OTGrammar_getWinner (me, itab);
		OTGrammarCandidate winner = & tableau -> candidates [iwinner];

		/*
			Error-driven: compare the adult winner (the correct candidate) and the learner's winner.
		*/
		if (str32equ (winner -> output.get(), adultOutput)) return;   // as far as we know, the grammar is already correct: don't update rankings

		/*
			Find (perhaps the learner's interpretation of) the adult output in the learner's own tableau
			(Tesar & Smolensky call this the "winner").
		*/
		integer iadult = 1;
		for (; iadult <= tableau -> numberOfCandidates; iadult ++) {
			OTGrammarCandidate cand = & tableau -> candidates [iadult];
			if (str32equ (cand -> output.get(), adultOutput))
				break;
		}
		if (iadult > tableau -> numberOfCandidates)
			Melder_throw (U"Cannot generate adult output \"", adultOutput, U"\".");

		/*
			Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
			The grammar will have to change.
		*/
		OTGrammar_modifyRankings (me, itab, iwinner, iadult, updateRule, honourLocalRankings,
			plasticity, relativePlasticityNoise, warnIfStalled, out_grammarHasChanged);
	} catch (MelderError) {
		Melder_throw (me, U": not learned from input \"", input, U"\" and adult output \"", adultOutput, U"\".");
	}
}

void OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, integer numberOfChews)
{
	if (! inputs)
		inputs = outputs;
	try {
		const integer n = inputs -> numberOfStrings;
		Melder_require (outputs -> numberOfStrings == n,
			U"Numbers of strings in input and output should be equal.");
		for (integer i = 1; i <= n; i ++) {
			for (integer ichew = 1; ichew <= numberOfChews; ichew ++)
				OTGrammar_learnOne (me, inputs -> strings [i].get(), outputs -> strings [i].get(),
					evaluationNoise, updateRule, honourLocalRankings,
					plasticity, relativePlasticityNoise, true, true, nullptr
				);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from ", inputs, U" (inputs) and ", outputs, U" (outputs).");
	}
}

void OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer numberOfChews)
{
	integer idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		double plasticity = initialPlasticity;
		autoMelderMonitor monitor (U"Learning with full knowledge...");
		for (integer iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
			for (integer ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
				conststring32 input, output;
				PairDistribution_peekPair (thee, & input, & output);
				++ idatum;
				if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
					Graphics_beginMovieFrame (monitor.graphics(), nullptr);
					Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50, 150);
					for (integer icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
						Graphics_setGrey (monitor.graphics(), (double) icons / 14);
						Graphics_line (monitor.graphics(),
							idatum, my constraints [icons]. ranking,
							idatum, my constraints [icons]. ranking + 1.0
						);
					}
					Graphics_endMovieFrame (monitor.graphics(), 0.0);
				}
				Melder_monitor ((double) idatum / numberOfData,
					U"Processing input-output pair ", idatum,
					U" out of ", numberOfData, U": ", input, U" -> ", output
				);
				for (integer ichew = 1; ichew <= numberOfChews; ichew ++)
					OTGrammar_learnOne (me, input, output,
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, true, true, nullptr
					);
			}
			plasticity *= plasticityDecrement;
		}
	} catch (MelderError) {
		if (idatum > 1)
			Melder_appendError (U"Only ", idatum - 1, U" input-output pairs out of ", numberOfData, U" were processed.");
		Melder_throw (me, U": did not complete learning from ", thee, U".");
	}
}

static integer PairDistribution_getNumberOfAttestedOutputs (PairDistribution me, conststring32 input, conststring32 *out_attestedOutput) {
	integer result = 0;
	for (integer ipair = 1; ipair <= my pairs.size; ipair ++) {
		PairProbability pair = my pairs.at [ipair];
		if (str32equ (pair -> string1.get(), input) && pair -> weight > 0.0) {
			if (out_attestedOutput) *out_attestedOutput = pair -> string2.get();
			result ++;
		}
	}
	return result;
}

bool OTGrammar_PairDistribution_findPositiveWeights (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation) {
	NUMlinprog linprog = nullptr;
	try {
		bool result = false;
		if (my decisionStrategy != kOTGrammar_decisionStrategy::HARMONIC_GRAMMAR &&
			my decisionStrategy != kOTGrammar_decisionStrategy::LINEAR_OT &&
			my decisionStrategy != kOTGrammar_decisionStrategy::POSITIVE_HG &&
			my decisionStrategy != kOTGrammar_decisionStrategy::EXPONENTIAL_HG)
		{
			Melder_throw (U"To find positive weights, the decision strategy should be HarmonicGrammar, LinearOT, PositiveHG, or ExponentialHG.");
		}
		autoINTVEC optimalCandidates = raw_INTVEC (my numberOfTableaus);
		/*
			Check that there is exactly one optimal output for each input.
		*/
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			conststring32 attestedOutput = nullptr;
			integer numberOfAttestedOutputs = PairDistribution_getNumberOfAttestedOutputs (thee, tab -> input.get(), & attestedOutput);
			if (numberOfAttestedOutputs == 0) {
				Melder_throw (U"Input \"", tab -> input.get(), U"\" has no attested output.");
			} else if (numberOfAttestedOutputs > 1) {
				Melder_throw (U"Input \"", tab -> input.get(), U"\" has more than one attested output.");
			} else {
				Melder_assert (attestedOutput);
				for (integer icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
					OTGrammarCandidate cand = & tab -> candidates [icand];
					if (str32equ (attestedOutput, cand -> output.get()))
						optimalCandidates [itab] = icand;
				}
			}
			Melder_assert (optimalCandidates [itab] != 0);
		}
		/*
			Create linear programming problem.
		*/
		linprog = NUMlinprog_new (false);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			NUMlinprog_addVariable (linprog, weightFloor, undefined, 1.0);
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			const integer ioptimalCandidate = optimalCandidates [itab];
			Melder_assert (ioptimalCandidate >= 1);
			Melder_assert (ioptimalCandidate <= tab -> numberOfCandidates);
			OTGrammarCandidate optimalCandidate = & tab -> candidates [ioptimalCandidate];
			for (integer icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
				if (icand != ioptimalCandidate) {
					OTGrammarCandidate cand = & tab -> candidates [icand];
					NUMlinprog_addConstraint (linprog, marginOfSeparation, undefined);
					for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
						NUMlinprog_addConstraintCoefficient (linprog, cand -> marks [icons] - optimalCandidate -> marks [icons]);
				}
			}
		}
		NUMlinprog_run (linprog);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const double weighting = NUMlinprog_getPrimalValue (linprog, icons);
			Melder_assert (weighting >= weightFloor);
			my constraints [icons]. ranking = my constraints [icons]. disharmony =
				my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ? log (weighting) : weighting;
		}
		NUMlinprog_delete (linprog);
		return result;
	} catch (MelderError) {
		NUMlinprog_delete (linprog);
		Melder_throw (me, U": positive weights not found.");
	}
}

void OTGrammar_reset (OTGrammar me, double ranking) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTGrammar_sort (me);
}

void OTGrammar_resetToRandomRanking (OTGrammar me, double mean, double standardDeviation) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> disharmony = constraint -> ranking = NUMrandomGauss (mean, standardDeviation);
	}
	OTGrammar_sort (me);
}

void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance) {
	/*
		First put the constraints in a random order and build a random index.
	*/
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> ranking = 0.0;
	}
	OTGrammar_newDisharmonies (me, 1.0);
	/*
		Then use the random index to yield a cascade of rankings.
	*/
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> disharmony = constraint -> ranking = maximumRanking - (icons - 1) * rankingDistance;
	}
	OTGrammar_sort (me);
}

void OTGrammar_setRanking (OTGrammar me, integer constraint, double ranking, double disharmony) {
	try {
		Melder_require (constraint > 0 && constraint <= my numberOfConstraints,
			U"There is no constraint with number ", constraint, U".");
		my constraints [constraint]. ranking = ranking;
		my constraints [constraint]. disharmony = disharmony;
		OTGrammar_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": ranking of constraint ", constraint, U" not set.");
	}
}

void OTGrammar_setConstraintPlasticity (OTGrammar me, integer constraint, double plasticity) {
	try {
		Melder_require (constraint > 0 && constraint <= my numberOfConstraints,
			U"There is no constraint with number ", constraint, U".");
		my constraints [constraint]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, U": plasticity of constraint ", constraint, U" not set.");
	}
}

integer theSaveNumberOfConstraints;
autoINTVEC theSaveIndex;
autoVEC theSaveRankings, theSaveDisharmonies;
autoBOOLVEC theSaveTiedToTheLeft, theSaveTiedToTheRight;
static void OTGrammar_save (OTGrammar me) {
	if (my numberOfConstraints != theSaveNumberOfConstraints) {
		theSaveIndex = raw_INTVEC (my numberOfConstraints);
		theSaveRankings = raw_VEC (my numberOfConstraints);
		theSaveDisharmonies = raw_VEC (my numberOfConstraints);
		theSaveTiedToTheLeft = raw_BOOLVEC (my numberOfConstraints);
		theSaveTiedToTheRight = raw_BOOLVEC (my numberOfConstraints);
		theSaveNumberOfConstraints = my numberOfConstraints;
	}
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		theSaveIndex [icons] = my index [icons];
		theSaveRankings [icons] = my constraints [icons]. ranking;
		theSaveDisharmonies [icons] = my constraints [icons]. disharmony;
		theSaveTiedToTheLeft [icons] = my constraints [icons]. tiedToTheLeft;
		theSaveTiedToTheRight [icons] = my constraints [icons]. tiedToTheRight;
	}
}
static void OTGrammar_restore (OTGrammar me) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		my index [icons] = theSaveIndex [icons];
		my constraints [icons]. ranking = theSaveRankings [icons];
		my constraints [icons]. disharmony = theSaveDisharmonies [icons];
		my constraints [icons]. tiedToTheLeft = theSaveTiedToTheLeft [icons];
		my constraints [icons]. tiedToTheRight = theSaveTiedToTheRight [icons];
	}
}

void OTGrammar_learnOneFromPartialOutput (OTGrammar me, conststring32 partialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, integer numberOfChews, bool warnIfStalled)
{
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy::EDCD)
			OTGrammar_save (me);
		integer ichew = 1;
		for (; ichew <= numberOfChews; ichew ++) {
			integer assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			bool grammarHasChanged = false;
			OTGrammar_learnOne (me,
				my tableaus [assumedAdultInputTableau]. input.get(),
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output.get(),
				evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticityNoise, Melder_debug == 47, warnIfStalled, & grammarHasChanged
			);
			if (! grammarHasChanged)
				return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy::EDCD && ichew > numberOfChews) {
			/*
				Is the partial output form grammatical by now?
			*/
			integer assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			const integer ilearnerCandidate = OTGrammar_getWinner (me, assumedAdultInputTableau);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [ilearnerCandidate];
			if (! str32equ (learnerCandidate -> output.get(),
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output.get()))
			{   /* Still ungrammatical? */
				/*
					Backtrack as in Tesar & Smolensky 2000:69.
				*/
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from partial adult output \"", partialAdultOutput, U"\".");
	}
}

static void OTGrammar_learnOneFromPartialOutput_opt (OTGrammar me, conststring32 partialAdultOutput, integer ipartialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, integer numberOfChews, bool warnIfStalled,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, integer resampleForCorrectForm)
{
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy::EDCD)
			OTGrammar_save (me);
		integer ichew = 1;
		for (; ichew <= numberOfChews; ichew ++) {
			integer assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarTableau tableau = & my tableaus [assumedAdultInputTableau];
			OTGrammarCandidate assumedCorrect = & tableau -> candidates [assumedAdultCandidate];

			/*
				Determine the "winner", i.e. the candidate that wins in the learner's grammar
				(Tesar & Smolensky call this the "loser").
			*/
			if (resampleForVirtualProduction)
				OTGrammar_newDisharmonies (me, evaluationNoise);
			integer iwinner = OTGrammar_getWinner (me, assumedAdultInputTableau);
			OTGrammarCandidate winner = & tableau -> candidates [iwinner];

			/*
				Error-driven: compare the adult winner (the correct candidate) and the learner's winner.
			*/
			if (compareOnlyPartialOutput) {
				if (str32str (winner -> output.get(), partialAdultOutput))
					return;   // as far as we know, the grammar is already correct: don't update rankings
			} else {
				if (str32equ (winner -> output.get(), assumedCorrect -> output.get()))
					return;   // as far as we know, the grammar is already correct: don't update rankings
			}
			if (resampleForCorrectForm) {
				integer itry = 1;
				for (; itry <= resampleForCorrectForm; itry ++) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
					integer iwinner2 = OTGrammar_getWinner (me, assumedAdultInputTableau);
					OTGrammarCandidate winner2 = & tableau -> candidates [iwinner2];
					if (compareOnlyPartialOutput) {
						if (str32str (winner2 -> output.get(), partialAdultOutput)) {
							assumedAdultCandidate = iwinner2;
							break;
						}
					} else {
						if (str32equ (winner2 -> output.get(), assumedCorrect -> output.get())) {
							assumedAdultCandidate = iwinner2;
							break;
						}
					}
				}
				if (itry > resampleForCorrectForm)
					return;   // no match, so bail out
			}

			/*
				Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
				The grammar will have to change.
			*/
			bool grammarHasChanged = false;
			OTGrammar_modifyRankings (me, assumedAdultInputTableau, iwinner, assumedAdultCandidate, updateRule, honourLocalRankings,
					plasticity, relativePlasticityNoise, warnIfStalled, & grammarHasChanged);
			if (! grammarHasChanged)
				return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy::EDCD && ichew > numberOfChews) {
			/*
				Is the partial output form grammatical by now?
			*/
			integer assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			const integer ilearnerCandidate = OTGrammar_getWinner (me, assumedAdultInputTableau);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [ilearnerCandidate];
			if (! str32equ (learnerCandidate -> output.get(),
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output.get()))
			{   /* Still ungrammatical? */
				/*
					Backtrack as in Tesar & Smolensky 2000:69.
				*/
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from partial adult output ", partialAdultOutput, U".");
	}
}

static autoOTHistory OTGrammar_createHistory (OTGrammar me, integer storeHistoryEvery, integer numberOfData) {
	try {
		integer numberOfSamplingPoints = numberOfData / storeHistoryEvery;   // e.g. 0, 20, 40, ...
		autoOTHistory thee = Thing_new (OTHistory);
		TableOfReal_init (thee.get(), 2 + numberOfSamplingPoints * 2, 1 + my numberOfConstraints);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Datum");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			TableOfReal_setColumnLabel (thee.get(), icons + 1, my constraints [icons]. name.get());
		TableOfReal_setRowLabel (thee.get(), 1, U"Initial state");
		thy data [1] [1] = 0.0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			thy data [1] [icons + 1] = my constraints [icons]. ranking;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": history not created.");
	}
}

static void OTGrammar_updateHistory (OTGrammar me, OTHistory thee, integer storeHistoryEvery, integer datumNumber, conststring32 input) {
	try {
		if (datumNumber % storeHistoryEvery == 0) {
			integer rowNumber = 2 * datumNumber / storeHistoryEvery;
			TableOfReal_setRowLabel (thee, rowNumber, input);
			thy data [rowNumber] [1] = datumNumber;
			thy data [rowNumber + 1] [1] = datumNumber;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				thy data [rowNumber] [icons + 1] = my constraints [icons]. disharmony;
				thy data [rowNumber + 1] [icons + 1] = my constraints [icons]. ranking;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": history not updated.");
	}
}

static void OTGrammar_finalizeHistory (OTGrammar me, OTHistory thee, integer datumNumber) {
	try {
		TableOfReal_setRowLabel (thee, thy numberOfRows, U"Final state");
		thy data [thy numberOfRows] [1] = datumNumber;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			thy data [thy numberOfRows] [icons + 1] = my constraints [icons]. ranking;
	} catch (MelderError) {
		Melder_throw (me, U": history not finalized.");
	}
}

void OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, integer numberOfChews,
	integer storeHistoryEvery, autoOTHistory *history_out)
{
	try {
		autoOTHistory history;
		if (storeHistoryEvery) {
			history = OTGrammar_createHistory (me, storeHistoryEvery, partialOutputs -> numberOfStrings);
		}
		try {
			for (integer idatum = 1; idatum <= partialOutputs -> numberOfStrings; idatum ++) {
				try {
					OTGrammar_learnOneFromPartialOutput (me, partialOutputs -> strings [idatum].get(),
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, numberOfChews, false);
				} catch (MelderError) {
					if (history) {
						OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum].get());   // so that we can inspect
					}
					throw;
				}
				if (history) {
					OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum].get());
				}
			}
			if (history) {
				OTGrammar_finalizeHistory (me, history.get(), partialOutputs -> numberOfStrings);
			}
			*history_out = history.move();
		} catch (MelderError) {
			*history_out = history.move();   // so that we can inspect
			throw;
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from partial outputs ", partialOutputs, U".");
	}
}

static void OTGrammar_opt_deleteOutputMatching (OTGrammar me) {
	for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tab = & my tableaus [itab];
		for (integer icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
			OTGrammarCandidate cand = & tab -> candidates [icand];
			cand -> numberOfPotentialPartialOutputsMatching = 0;
			cand -> partialOutputMatches.reset();
		}
	}
}

static void OTGrammar_Distributions_opt_createOutputMatching (OTGrammar me, Distributions thee, integer columnNumber) {
	try {
		if (columnNumber > thy numberOfColumns)
			Melder_throw (U"No column ", columnNumber, U" in Distributions.");
		if (thy numberOfRows < 1)
			Melder_throw (U"No candidates in Distributions.");
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (integer icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tab -> candidates [icand];
				cand -> numberOfPotentialPartialOutputsMatching = thy numberOfRows;
				cand -> partialOutputMatches = zero_BOOLVEC (thy numberOfRows);
			}
		}
		for (integer ipartialOutput = 1; ipartialOutput <= thy numberOfRows; ipartialOutput ++) {
			if (thy data [ipartialOutput] [columnNumber] > 0.0) {
				conststring32 partialOutput = thy rowLabels [ipartialOutput].get();
				bool foundPartialOutput = false;
				for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
					OTGrammarTableau tab = & my tableaus [itab];
					for (integer icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
						OTGrammarCandidate cand = & tab -> candidates [icand];
						if (str32str (cand -> output.get(), partialOutput)) {
							foundPartialOutput = true;
							cand -> partialOutputMatches [ipartialOutput] = true;
						}
					}
				}
				if (! foundPartialOutput)
					Melder_throw (U"The partial output \"", partialOutput, U"\" does not match any candidate for any input form.");
			}
		}
	} catch (MelderError) {
		OTGrammar_opt_deleteOutputMatching (me);
		throw;
	}
}

void OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, integer columnNumber,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer numberOfChews,
	integer storeHistoryEvery, autoOTHistory *history_out,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, integer resampleForCorrectForm)
{
	integer idatum = 0;
	const integer numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		autoOTHistory history;
		OTGrammar_Distributions_opt_createOutputMatching (me, thee, columnNumber);
		autoMelderMonitor monitor (U"Learning with limited knowledge...");
		if (storeHistoryEvery)
			history = OTGrammar_createHistory (me, storeHistoryEvery, numberOfData);
		try {
			double plasticity = initialPlasticity;
			for (integer iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
				for (integer ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
					conststring32 partialOutput;
					integer ipartialOutput;
					Distributions_peek (thee, columnNumber, & partialOutput, & ipartialOutput);
					++ idatum;
					if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
						Graphics_beginMovieFrame (monitor.graphics(), nullptr);
						Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50.0, 150.0);
						for (integer icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
							Graphics_setGrey (monitor.graphics(), (double) icons / 14);
							Graphics_line (monitor.graphics(),
								idatum, my constraints [icons]. ranking,
								idatum, my constraints [icons]. ranking + 10.0
							);
						}
						Graphics_endMovieFrame (monitor.graphics(), 0.0);
					}
					Melder_monitor ((double) idatum / numberOfData,
						U"Processing partial output ", idatum, U" out of ", numberOfData, U": ",
						thy rowLabels [ipartialOutput].get()
					);
					try {
						OTGrammar_learnOneFromPartialOutput_opt (me, partialOutput, ipartialOutput,
							evaluationNoise, updateRule, honourLocalRankings,
							plasticity, relativePlasticityNoise, numberOfChews, false,
							resampleForVirtualProduction, compareOnlyPartialOutput, resampleForCorrectForm
						);   // no warning if stalled: RIP form is allowed to be harmonically bounded
					} catch (MelderError) {
						if (history)
							OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput].get());
						throw;
					}
					if (history)
						OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput].get());
				}
				plasticity *= plasticityDecrement;
			}
			if (history)
				OTGrammar_finalizeHistory (me, history.get(), numberOfData);
			OTGrammar_opt_deleteOutputMatching (me);
			if (history_out)
				*history_out = history.move();
		} catch (MelderError) {
			OTGrammar_opt_deleteOutputMatching (me);
			if (history_out)
				*history_out = history.move();   // so that we can inspect
			throw;
		}
	} catch (MelderError) {
		if (idatum > 1)
			Melder_appendError (U"Only ", idatum - 1, U" input-output pairs out of ", numberOfData, U" were processed.");
		Melder_throw (me, U" & ", thee, U": not learned from partial outputs.");
	}
}

double OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, integer numberOfInputs)
{
	try {
		integer numberOfCorrect = 0;
		for (integer ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
			conststring32 input, adultOutput;
			PairDistribution_peekPair (thee, & input, & adultOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			integer inputTableau = OTGrammar_getTableau (me, input);
			const integer ilearnerCandidate = OTGrammar_getWinner (me, inputTableau);
			OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [ilearnerCandidate];
			if (str32equ (learnerCandidate -> output.get(), adultOutput))
				numberOfCorrect ++;
		}
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": fraction correct not computed.");
	}
}

integer OTGrammar_PairDistribution_getMinimumNumberCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, integer numberOfReplications)
{
	try {
		integer minimumNumberCorrect = numberOfReplications;
		for (integer ipair = 1; ipair <= thy pairs.size; ipair ++) {
			PairProbability prob = thy pairs.at [ipair];
			if (prob -> weight > 0.0) {
				integer numberOfCorrect = 0;
				conststring32 input = prob -> string1.get(), adultOutput = prob -> string2.get();
				integer inputTableau = OTGrammar_getTableau (me, input);
				for (integer ireplication = 1; ireplication <= numberOfReplications; ireplication ++) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
					const integer ilearnerCandidate = OTGrammar_getWinner (me, inputTableau);
					OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [ilearnerCandidate];
					if (str32equ (learnerCandidate -> output.get(), adultOutput))
						numberOfCorrect ++;
				}
				if (numberOfCorrect < minimumNumberCorrect)
					minimumNumberCorrect = numberOfCorrect;
			}
		}
		return minimumNumberCorrect;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": minimum number correct not computed.");
	}
}

double OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, integer columnNumber,
	double evaluationNoise, integer numberOfInputs)
{
	try {
		integer numberOfCorrect = 0;
		OTGrammar_Distributions_opt_createOutputMatching (me, thee, columnNumber);
		for (integer ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
			integer ipartialOutput;
			Distributions_peek (thee, columnNumber, nullptr, & ipartialOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			integer assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			const integer ilearnerCandidate = OTGrammar_getWinner (me, assumedAdultInputTableau);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [ilearnerCandidate];
			if (str32equ (learnerCandidate -> output.get(), my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output.get()))
				numberOfCorrect ++;
		}
		OTGrammar_opt_deleteOutputMatching (me);
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": fraction correct not computed.");
	}
}

void OTGrammar_removeConstraint (OTGrammar me, conststring32 constraintName) {
	try {
		integer removed = 0;
		Melder_require (my numberOfConstraints > 1,
			U"Cannot remove last remaining constraint.");

		/*
			Look for the constraint to be removed.
		*/
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [icons];
			if (str32equ (constraint -> name.get(), constraintName)) {
				removed = icons;
				break;
			}
		}
		if (removed == 0)
			Melder_throw (U"No such constraint.");
		/*
			Remove the constraint while reusing the memory space.
		*/
		my constraints [removed]. destroy ();
		Melder_assert (! my constraints [removed]. name);
		my constraints. remove (removed);
		my numberOfConstraints -= 1;   // maintain invariant
		Melder_assert (my numberOfConstraints == my constraints.size);
		/*
			Remove or shift fixed rankings.
		*/
		for (integer ifixed = my numberOfFixedRankings; ifixed > 0; ifixed --) {
			OTGrammarFixedRanking fixed = & my fixedRankings [ifixed];
			if (fixed -> higher == removed || fixed -> lower == removed) {
				/*
					Remove fixed ranking.
				*/
				my fixedRankings [ifixed]. destroy();
				my fixedRankings. remove (ifixed);
				my numberOfFixedRankings -= 1;   // maintain invariant
				Melder_assert (my numberOfFixedRankings == my fixedRankings.size);
			} else {
				/*
					Shift fixed ranking.
				*/
				if (fixed -> higher > removed)
					fixed -> higher -= 1;
				if (fixed -> lower > removed)
					fixed -> lower -= 1;
			}
		}
		/*
			Shift tableau rows.
		*/
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate candidate = & tableau -> candidates [icand];
				candidate -> marks. remove (removed);
				candidate -> numberOfConstraints -= 1;   // maintain invariant
				Melder_assert (candidate -> numberOfConstraints == candidate -> marks.size);
			}
		}
		/*
			Rebuild index.
		*/
		my index. resize (my numberOfConstraints);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			my index [icons] = icons;
		OTGrammar_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": constraint \"", constraintName, U"\" not removed.");
	}
}

static void OTGrammarTableau_removeCandidate_unstripped (OTGrammarTableau me, integer candidateNumber) {
	Melder_assert (candidateNumber >= 1);
	if (candidateNumber > my numberOfCandidates)
		Melder_fatal (U"icand ", candidateNumber, U", ncand ", my numberOfCandidates);

	my candidates [candidateNumber]. destroy ();
	Melder_assert (! my candidates [candidateNumber]. output);   // check leak
	Melder_assert (my candidates [candidateNumber]. marks.size == 0);
	Melder_assert (my candidates [candidateNumber]. marks.cells == nullptr);   // check leak
	my candidates. remove (candidateNumber);
	my numberOfCandidates -= 1;   // maintain invariant
	Melder_assert (my numberOfCandidates == my candidates.size);
}

static bool OTGrammarTableau_isHarmonicallyBounded (OTGrammarTableau me, integer icand, integer jcand) {
	OTGrammarCandidate candi = & my candidates [icand], candj = & my candidates [jcand];
	bool equal = true;
	if (icand == jcand)
		return false;
	for (integer icons = 1; icons <= candi -> numberOfConstraints; icons ++) {
		if (candi -> marks [icons] < candj -> marks [icons])
			return false;
		if (candi -> marks [icons] > candj -> marks [icons])
			equal = false;
	}
	return ! equal;
}

static bool OTGrammarTableau_candidateIsPossibleWinner (OTGrammar me, integer itab, integer icand) {
	OTGrammar_save (me);
	OTGrammar_reset (me, 100.0);
	for (;;) {
		bool grammarHasChanged = false;
		OTGrammar_learnOne (me, my tableaus [itab]. input.get(), my tableaus [itab]. candidates [icand]. output.get(),
			1e-3, kOTGrammar_rerankingStrategy::EDCD, false, 1.0, 0.0, true, true, & grammarHasChanged);
		if (! grammarHasChanged) {
			OTGrammar_restore (me);
			return true;
		}
		double previousStratum = 101.0;
		OTGrammar_newDisharmonies (me, 0.0);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const double stratum = my constraints [my index [icons]]. ranking;
			#if 0
			if (stratum < 50.0 - my numberOfConstraints) {
				OTGrammar_restore (me);
				return false;   // we detected a tumble
			}
			#else
			if (stratum < previousStratum) {
				if (stratum < previousStratum - 1.0) {
					OTGrammar_restore (me);
					return false;   // we detected a vacated stratum
				}
				previousStratum = stratum;
			}
			#endif
		}
	}
	return false;   // cannot occur
}

void OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, bool singly) {
	try {
		/*
			First, the candidates that are harmonically bounded by one or more single other candidates have to be removed;
			otherwise, EDCD will stall.
		*/
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (integer icand = tab -> numberOfCandidates; icand >= 1; icand --) {
				for (integer jcand = 1; jcand <= tab -> numberOfCandidates; jcand ++) {
					if (OTGrammarTableau_isHarmonicallyBounded (tab, icand, jcand)) {
						OTGrammarTableau_removeCandidate_unstripped (tab, icand);
						break;
					}
				}
			}
			//tab -> candidates.shrinkToFit();
		}
		if (! singly) {
			for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
				OTGrammarTableau tab = & my tableaus [itab];
				for (integer icand = tab -> numberOfCandidates; icand >= 1; icand --) {
					if (! OTGrammarTableau_candidateIsPossibleWinner (me, itab, icand))
						OTGrammarTableau_removeCandidate_unstripped (tab, icand);
				}
				//tab -> candidates.shrinkToFit();
			}	
		}
	} catch (MelderError) {
		Melder_throw (me, U": not all harmonically bounded candidates were removed.");
	}
}

Thing_define (OTGrammar_List4, Daata) {
	// new data:
		integer hi1, lo1, hi2, lo2;
};

Thing_implement (OTGrammar_List4, Daata, 0);

void OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee) {
	/*
		Save.
	*/
	integer savedNumberOfFixedRankings = my numberOfFixedRankings;
	autovector <structOTGrammarFixedRanking> savedFixedRankings = my fixedRankings.move();   // BUG: is not restored upon throw
	OTGrammar_save (me);
	try {
		integer ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
		integer itrial;
		const double evaluationNoise = 1e-9;
		/*
			Add room for two more fixed rankings.
		*/
		my fixedRankings = newvectorzero <structOTGrammarFixedRanking> (my numberOfFixedRankings + 2);
		for (integer ifixedRanking = 1; ifixedRanking <= my numberOfFixedRankings; ifixedRanking ++) {
			my fixedRankings [ifixedRanking]. higher = savedFixedRankings [ifixedRanking]. higher;
			my fixedRankings [ifixedRanking]. lower = savedFixedRankings [ifixedRanking]. lower;
		}
		/*
			Test whether there are rankings at all for these output data.
		*/
		OTGrammar_reset (me, 100.0);
		for (itrial = 1; itrial <= 40; itrial ++) {
			bool grammarHasChangedDuringCycle = false;
			OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			for (integer iform = 1; iform <= thy pairs.size; iform ++) {
				PairProbability prob = thy pairs.at [iform];
				if (prob -> weight > 0.0) {
					bool grammarHasChanged = false;
					OTGrammar_learnOne (me, prob -> string1.get(), prob -> string2.get(),
						evaluationNoise, kOTGrammar_rerankingStrategy::EDCD, true /* honour fixed rankings; very important */,
						1.0, 0.0, false, true, & grammarHasChanged
					);
					if (grammarHasChanged)
						OTGrammar_newDisharmonies (me, evaluationNoise);
					grammarHasChangedDuringCycle |= grammarHasChanged;
				}
			}
			if (! grammarHasChangedDuringCycle)
				break;
		}
		if (itrial > 40) {
			MelderInfo_writeLine (U"There are no total rankings that generate these input-output pairs.");
			throw MelderError ();
		}
		/*
			Test learnability of every possible ranked pair.
		*/
		my numberOfFixedRankings ++;
		autoBOOLMAT obligatory = zero_BOOLMAT (my numberOfConstraints, my numberOfConstraints);
		MelderInfo_open ();
		autoMelderProgress progress (U"Finding obligatory rankings.");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (integer jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair, U": Trying ranking ",
					my constraints [icons]. name.get(), U" >> ", my constraints [jcons]. name.get());
				ipair ++;
				for (itrial = 1; itrial <= 40; itrial ++) {
					bool grammarHasChangedDuringCycle = false;
					OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
					OTGrammar_newDisharmonies (me, evaluationNoise);
					for (integer iform = 1; iform <= thy pairs.size; iform ++) {
						PairProbability prob = thy pairs.at [iform];
						if (prob -> weight > 0.0) {
							bool grammarHasChanged = false;
							OTGrammar_learnOne (me, prob -> string1.get(), prob -> string2.get(),
								evaluationNoise, kOTGrammar_rerankingStrategy::EDCD, true /* honour fixed rankings; very important */,
								1.0, 0.0, false, true, & grammarHasChanged);
							if (grammarHasChanged)
								OTGrammar_newDisharmonies (me, evaluationNoise);
							grammarHasChangedDuringCycle |= grammarHasChanged;
						}
					}
					if (! grammarHasChangedDuringCycle)
						break;
				}
				if (itrial > 40) {
					obligatory [jcons] [icons] = true;
					MelderInfo_writeLine (my constraints [jcons]. name.get(), U" >> ", my constraints [icons]. name.get());
					MelderInfo_close ();
				}
			}
		}
		my numberOfFixedRankings ++;
		Melder_progress (0.0, U"");
		npair = npair * npair;
		OrderedOf<structOTGrammar_List4> list;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (integer jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons && ! obligatory [jcons] [icons]) {
				my fixedRankings [my numberOfFixedRankings - 1]. higher = icons;
				my fixedRankings [my numberOfFixedRankings - 1]. lower = jcons;
				for (integer kcons = icons; kcons <= my numberOfConstraints; kcons ++) {
					for (integer lcons = 1; lcons <= my numberOfConstraints; lcons ++) if (kcons != lcons && ! obligatory [lcons] [kcons]) {
						if (icons == kcons && jcons >= lcons)
							continue;
						if (icons == lcons && jcons == kcons)
							continue;
						if (jcons == kcons && obligatory [lcons] [icons])
							continue;
						if (icons == lcons && obligatory [jcons] [kcons])
							continue;
						if (obligatory [lcons] [icons] && obligatory [jcons] [kcons])
							continue;
						my fixedRankings [my numberOfFixedRankings]. higher = kcons;
						my fixedRankings [my numberOfFixedRankings]. lower = lcons;
						OTGrammar_reset (me, 100.0);
						Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair);
						ipair ++;
						for (itrial = 1; itrial <= 40; itrial ++) {
							bool grammarHasChangedDuringCycle = false;
							OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
							OTGrammar_newDisharmonies (me, evaluationNoise);
							for (integer iform = 1; iform <= thy pairs.size; iform ++) {
								PairProbability prob = thy pairs.at [iform];
								if (prob -> weight > 0.0) {
									bool grammarHasChanged = false;
									OTGrammar_learnOne (me, prob -> string1.get(), prob -> string2.get(),
											evaluationNoise, kOTGrammar_rerankingStrategy::EDCD, true /* honour fixed rankings; very important */,
											1.0, 0.0, false, true, & grammarHasChanged);
									if (grammarHasChanged)
										OTGrammar_newDisharmonies (me, evaluationNoise);
									grammarHasChangedDuringCycle |= grammarHasChanged;
								}
							}
							if (! grammarHasChangedDuringCycle)
								break;
						}
						if (itrial > 40) {
							autoOTGrammar_List4 listElement = Thing_new (OTGrammar_List4);
							listElement -> hi1 = jcons;
							listElement -> lo1 = icons;
							listElement -> hi2 = lcons;
							listElement -> lo2 = kcons;
							list. addItem_move (listElement.move());
						}
					}
				}
			}
		}
		Melder_progress (1.0);
		/*
			Improve list.
		*/
		bool improved = true;
		while (improved) {
			improved = false;
			for (integer ilist = 1; ilist <= list.size; ilist ++) {
				for (integer jlist = 1; jlist <= list.size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = list.at [ilist];
					OTGrammar_List4 elB = list.at [jlist];
					integer ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					integer bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
					improved |= (ahi1 == bhi1 || obligatory [bhi1] [ahi1]) && (ahi2 == bhi2 || obligatory [bhi2] [ahi2]) &&
						(alo1 == blo1 || obligatory [alo1] [blo1]) && (alo2 == blo2 || obligatory [alo2] [blo2]);
					improved |= (ahi1 == bhi2 || obligatory [bhi2] [ahi1]) && (ahi2 == bhi1 || obligatory [bhi1] [ahi2]) &&
						(alo1 == blo2 || obligatory [alo1] [blo2]) && (alo2 == blo1 || obligatory [alo2] [blo1]);
					if (improved) {
						list. removeItem (jlist);
						break;
					}
				}
				if (improved) break;
			}
		}
		improved = true;
		while (improved) {
			improved = false;
			for (integer ilist = 1; ilist <= list.size; ilist ++) {
				for (integer jlist = 1; jlist <= list.size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = list.at [ilist];
					OTGrammar_List4 elB = list.at [jlist];
					integer ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					integer bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
					improved |= ahi1 == bhi1 && alo1 == blo1 && ahi2 == bhi2 && blo2 == bhi1 && alo2 == alo1;
					improved |= ahi1 == bhi2 && alo1 == blo2 && ahi2 == bhi1 && blo1 == bhi2 && alo2 == alo1;
					improved |= ahi2 == bhi1 && alo2 == blo1 && ahi1 == bhi2 && blo2 == bhi1 && alo1 == alo2;
					improved |= ahi2 == bhi2 && alo2 == blo2 && ahi1 == bhi1 && blo1 == bhi2 && alo1 == alo2;
					if (improved) {
						list. removeItem (jlist);
						break;
					}
				}
				if (improved) break;
			}
		}
		for (integer ilist = 1; ilist <= list.size; ilist ++) {
			OTGrammar_List4 el = list.at [ilist];
			MelderInfo_write (my constraints [el -> hi1]. name.get(), U" >> ", my constraints [el -> lo1]. name.get(), U" OR ");
			MelderInfo_writeLine (my constraints [el -> hi2]. name.get(), U" >> ", my constraints [el -> lo2]. name.get());
			MelderInfo_close ();
		}
		MelderInfo_close ();

		/*
			Restore.
		*/
		my numberOfFixedRankings = savedNumberOfFixedRankings;
		my fixedRankings = savedFixedRankings.move();
		OTGrammar_restore (me);
	} catch (MelderError) {
		MelderInfo_close ();
		/*
			Restore.
		*/
		my numberOfFixedRankings = savedNumberOfFixedRankings;
		my fixedRankings = savedFixedRankings.move();
		OTGrammar_restore (me);
		Melder_throw (me, U": obligatory rankings not listed.");
	}
}

void OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, integer columnNumber) {
	/*
		Save.
	*/
	autovector <structOTGrammarFixedRanking> savedFixedRankings = my fixedRankings.move();
	OTGrammar_save (me);
	try {
		integer ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
		/*
			Add room for one more fixed ranking.
		*/
		my numberOfFixedRankings ++;
		my fixedRankings = newvectorzero <structOTGrammarFixedRanking> (my numberOfFixedRankings);
		for (integer ifixedRanking = 1; ifixedRanking < my numberOfFixedRankings; ifixedRanking ++) {
			my fixedRankings [ifixedRanking]. higher = savedFixedRankings [ifixedRanking]. higher;
			my fixedRankings [ifixedRanking]. lower = savedFixedRankings [ifixedRanking]. lower;
		}
		/*
			Test learnability of every possible ranked pair.
		*/
		MelderInfo_open ();
		autoMelderProgress progress (U"Finding obligatory rankings.");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (integer jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair, U": Trying ranking ",
					my constraints [icons]. name.get(), U" >> ", my constraints [jcons]. name.get());
				ipair ++;
				Melder_progressOff ();
				OTGrammar_Distributions_learnFromPartialOutputs (me, thee, columnNumber,
					1e-9, kOTGrammar_rerankingStrategy::EDCD, true /* honour fixed rankings; very important */,
					1.0, 1000, 0.0, 1, 0.0, 1, 0, nullptr, false, false, 0);
				Melder_progressOn ();
				for (integer kcons = 1; kcons <= my numberOfConstraints; kcons ++) {
					if (my constraints [kcons]. ranking < 0.0) {
						MelderInfo_writeLine (my constraints [jcons]. name.get(), U" >> ", my constraints [icons]. name.get());
						break;
					}
				}
			}
		}
		MelderInfo_close ();
		/*
			Restore.
		*/
		my numberOfFixedRankings --;
		my fixedRankings = savedFixedRankings.move();
		OTGrammar_restore (me);
	} catch (MelderError) {
		MelderInfo_close ();
		/*
			Restore.
		*/
		my numberOfFixedRankings --;
		my fixedRankings = savedFixedRankings.move();
		OTGrammar_restore (me);
		Melder_throw (me, U": obligatory rankings not listed.");
	}
}

static void printConstraintNames (OTGrammar me, MelderString *buffer) {
	char32 text [200];
	bool secondLine = false;
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		if (str32chr (constraint -> name.get(), U'\n')) {
			char32 *newLine;
			str32cpy (text, constraint -> name.get());
			newLine = str32chr (text, U'\n');
			*newLine = U'\0';
			MelderString_append (buffer, U"\t", text);
			secondLine = true;
		} else {
			MelderString_append (buffer, U"\t", constraint -> name.get());
		}
	}
	MelderString_appendCharacter (buffer, U'\n');
	if (secondLine) {
		MelderString_appendCharacter (buffer, U'\t');
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			char32 *newLine = str32chr (constraint -> name.get(), U'\n');
			MelderString_append (buffer, U"\t", newLine ? newLine + 1 : U"");
		}
		MelderString_appendCharacter (buffer, U'\n');
	}
}

void OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file) {
	try {
		autoMelderString buffer;
		MelderString_copy (& buffer, U"CONSTRAINTS\t");
		printConstraintNames (me, & buffer);
		MelderString_append (& buffer, U"rankings\t");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, U"\t", constraint -> ranking);
		}
		MelderString_append (& buffer, U"\ndisharmonies\t");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, U"\t", constraint -> disharmony);
		}
		MelderString_appendCharacter (& buffer, U'\n');
		for (integer itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			integer winner = OTGrammar_getWinner (me, itab), numberOfOptimalCandidates = 0;
			for (integer icons = 1; icons <= my numberOfConstraints + 1; icons ++)
				MelderString_appendCharacter (& buffer, U'\t');
			MelderString_append (& buffer, U"\nINPUT\t", tableau -> input.get());
			printConstraintNames (me, & buffer);
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				if (OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0)
					numberOfOptimalCandidates ++;
			}
			for (integer icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate candidate = & tableau -> candidates [icand];
				bool candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
				integer crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
				MelderString_append (& buffer,
					candidateIsOptimal == false ? U"loser" : numberOfOptimalCandidates > 1 ? U"co-winner" : U"winner",
					U"\t",
					candidate -> output.get());
				for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
					integer index = my index [icons];
					OTGrammarConstraint constraint = & my constraints [index];
					static MelderString markString;
					MelderString_empty (& markString);
					/*
						An exclamation mark can be drawn in this cell only if all of the following conditions are met:
						1. the candidate is not optimal;
						2. the constraint is not tied;
						3. this is the crucial cell, i.e. the cells after it are drawn in grey.
					*/
					if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
						const integer winnerMarks = tableau -> candidates [winner]. marks [index];
						for (integer imark = 1; imark <= winnerMarks + 1; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						MelderString_appendCharacter (& markString, U'!');
						for (integer imark = winnerMarks + 2; imark <= candidate -> marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							MelderString_appendCharacter (& markString, U'=');
						}
						for (integer imark = 1; imark <= candidate -> marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
					}
					MelderString_append (& buffer, U"\t", markString.string);
				}
				MelderString_appendCharacter (& buffer, U'\n');
			}
		}
		MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
	} catch (MelderError) {
		Melder_throw (me, U": not saved to tab-separated file ", file, U".");
	}
}

/* End of file OTGrammar.cpp */
