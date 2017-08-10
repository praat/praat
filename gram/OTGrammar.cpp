/* OTGrammar.cpp
 *
 * Copyright (C) 1997-2012,2014,2015,2016,2017 Paul Boersma
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
	long numberOfCandidates = 0, numberOfViolations = 0;
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		numberOfCandidates += tableaus [itab]. numberOfCandidates;
		for (long icand = 1; icand <= tableaus [itab]. numberOfCandidates; icand ++)
			for (long icons = 1; icons <= numberOfConstraints; icons ++)
				numberOfViolations += tableaus [itab]. candidates [icand]. marks [icons];
	}
	MelderInfo_writeLine (U"Decision strategy: ", kOTGrammar_decisionStrategy_getText (decisionStrategy));
	MelderInfo_writeLine (U"Number of constraints: ", numberOfConstraints);
	MelderInfo_writeLine (U"Number of tableaus: ", numberOfTableaus);
	MelderInfo_writeLine (U"Number of candidates: ", numberOfCandidates);
	MelderInfo_writeLine (U"Number of violation marks: ", numberOfViolations);
}

void structOTGrammar :: v_writeText (MelderFile file) {
	MelderFile_write (file, U"\n<", kOTGrammar_decisionStrategy_getText (decisionStrategy),
		U">\n", leak, U" ! leak\n", numberOfConstraints, U" constraints");
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & constraints [icons];
		MelderFile_write (file, U"\nconstraint [", icons, U"]: \"");
		for (const char32 *p = & constraint -> name [0]; *p; p ++) {
			if (*p == '\"') MelderFile_writeCharacter (file, U'\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", constraint -> ranking,
			U" ", constraint -> disharmony, U" ", constraint -> plasticity, U" ! ");
		for (const char32 *p = & constraint -> name [0]; *p; p ++) {
			if (*p == '\n') MelderFile_writeCharacter (file, U' ');
			else if (*p == '\\' && p [1] == 's' && p [2] == '{') p += 2;
			else if (*p == '}') { }
			else MelderFile_writeCharacter (file, *p);
		}
	}
	MelderFile_write (file, U"\n\n", numberOfFixedRankings, U" fixed rankings");
	for (long irank = 1; irank <= numberOfFixedRankings; irank ++) {
		OTGrammarFixedRanking fixedRanking = & fixedRankings [irank];
		MelderFile_write (file, U"\n   ", fixedRanking -> higher, U" ", fixedRanking -> lower);
	}
	MelderFile_write (file, U"\n\n", numberOfTableaus, U" tableaus");
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & tableaus [itab];
		MelderFile_write (file, U"\ninput [", itab, U"]: \"");
		for (const char32 *p = & tableau -> input [0]; *p; p ++) {
			if (*p == '\"') MelderFile_writeCharacter (file, U'\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", tableau -> numberOfCandidates);
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			MelderFile_write (file, U"\n   candidate [", icand, U"]: \"");
			for (const char32 *p = & candidate -> output [0]; *p; p ++) {
				if (*p =='\"') MelderFile_writeCharacter (file, U'\"');   // Double any quotes within quotes.
				MelderFile_writeCharacter (file, *p);
			}
			MelderFile_writeCharacter (file, U'\"');
			for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				MelderFile_write (file, U" ", candidate -> marks [icons]);
			}
		}
	}
}

void OTGrammar_checkIndex (OTGrammar me) {
	int icons;
	if (my index) return;
	my index = NUMvector <long> (1, my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTGrammar_sort (me);
}

void structOTGrammar :: v_readText (MelderReadText text, int formatVersion) {
	OTGrammar_Parent :: v_readText (text, formatVersion);
	if (formatVersion >= 1) {
		try {
			decisionStrategy = texgete8 (text, kOTGrammar_decisionStrategy_getValue);
		} catch (MelderError) {
			Melder_throw (U"Trying to read decision strategy.");
		}
	}
	if (formatVersion >= 2) {
		try {
			leak = texgetr64 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read leak.");
		}
	}
	try {
		numberOfConstraints = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of constraints.");
	}
	if (numberOfConstraints < 1) Melder_throw (U"No constraints.");
	constraints = NUMvector <structOTGrammarConstraint> (1, numberOfConstraints);
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
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
		numberOfFixedRankings = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of fixed rankings.");
	}
	if (numberOfFixedRankings >= 1) {
		fixedRankings = NUMvector <structOTGrammarFixedRanking> (1, numberOfFixedRankings);
		for (long irank = 1; irank <= numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & fixedRankings [irank];
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
		numberOfTableaus = texgeti32 (text);
	} catch (MelderError) {
		Melder_throw (U"Trying to read number of tableaus.");
	}
	if (numberOfTableaus < 1) Melder_throw (U"No tableaus.");
	tableaus = NUMvector <structOTGrammarTableau> (1, numberOfTableaus);
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & tableaus [itab];
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
		if (tableau -> numberOfCandidates < 1) Melder_throw
			(U"No candidates in tableau ", itab,
			 U" (input: ", tableau -> input, U")"
			 U" in line ", MelderReadText_getLineNumber (text),
			 itab == 1 ? U"." : U", or perhaps wrong number of candidates for input " U_LEFT_GUILLEMET,
			 itab == 1 ? nullptr : tableaus [itab - 1]. input,
			 itab == 1 ? nullptr : U_RIGHT_GUILLEMET U".");
		tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates);
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			try {
				candidate -> output = texgetw16 (text);
			} catch (MelderError) {
				Melder_throw (U"Trying to read candidate ", icand, U" of tableau ", itab,
					U" (input: ", tableau -> input, U") in line ", MelderReadText_getLineNumber (text), U".");
			}
			candidate -> numberOfConstraints = numberOfConstraints;   // redundancy, needed for writing binary
			candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints);
			for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				try {
					candidate -> marks [icons] = texgeti16 (text);
				} catch (MelderError) {
					Melder_throw
					(U"Trying to read number of violations of constraint ", icons,
					 U" (", constraints [icons]. name, U")"
					 U" of candidate ", icand,
					 U" (", candidate -> output, U")"
					 U" of tableau ", itab,
					 U" (input: ", tableau -> input, U")"
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
	long icons = * (long *) first, jcons = * (long *) second;
	OTGrammarConstraint ci = & my constraints [icons], cj = & my constraints [jcons];
	/*
	 * Sort primarily by disharmony.
	 */
	if (ci -> disharmony > cj -> disharmony) return -1;
	if (ci -> disharmony < cj -> disharmony) return +1;
	/*
	 * Tied constraints are sorted alphabetically.
	 */
	return str32cmp (my constraints [icons]. name, my constraints [jcons]. name);
}

void OTGrammar_sort (OTGrammar me) {
	constraintCompare_grammar = me;
	qsort (& my index [1], my numberOfConstraints, sizeof (long), constraintCompare);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> tiedToTheLeft = icons > 1 &&
			my constraints [my index [icons - 1]]. disharmony == constraint -> disharmony;
		constraint -> tiedToTheRight = icons < my numberOfConstraints &&
			my constraints [my index [icons + 1]]. disharmony == constraint -> disharmony;
	}
}

void OTGrammar_newDisharmonies (OTGrammar me, double spreading) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, spreading)
			/*NUMrandomUniform (-spreading, spreading)*/;
	}
	OTGrammar_sort (me);
}

long OTGrammar_getTableau (OTGrammar me, const char32 *input) {
	long n = my numberOfTableaus;
	for (long i = 1; i <= n; i ++)
		if (str32equ (my tableaus [i]. input, input))
			return i;
	Melder_throw (U"Input \"", input, U"\" not in list of tableaus.");
}

static void _OTGrammar_fillInHarmonies (OTGrammar me, long itab) noexcept {
	if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) return;
	OTGrammarTableau tableau = & my tableaus [itab];
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		int *marks = candidate -> marks;
		double disharmony = 0.0;
		if (my decisionStrategy == kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR ||
			my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY)
		{
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				disharmony += my constraints [icons]. disharmony * marks [icons];
			}
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				disharmony += exp (my constraints [icons]. disharmony) * marks [icons];
			}
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				if (my constraints [icons]. disharmony > 0.0) {
					disharmony += my constraints [icons]. disharmony * marks [icons];
				}
			}
		} else if (my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				double constraintDisharmony = my constraints [icons]. disharmony > 1.0 ? my constraints [icons]. disharmony : 1.0;
				disharmony += constraintDisharmony * marks [icons];
			}
		} else {
			Melder_fatal (U"_OTGrammar_fillInHarmonies: unimplemented decision strategy.");
		}
		candidate -> harmony = - disharmony;
	}
}

int OTGrammar_compareCandidates (OTGrammar me, long itab1, long icand1, long itab2, long icand2) noexcept {
	int *marks1 = my tableaus [itab1]. candidates [icand1]. marks;
	int *marks2 = my tableaus [itab2]. candidates [icand2]. marks;
	if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfMarks1 = marks1 [my index [icons]];
			int numberOfMarks2 = marks2 [my index [icons]];
			/*
			 * Count tied constraints as one.
			 */
			while (my constraints [my index [icons]]. tiedToTheRight) {
				icons ++;
				numberOfMarks1 += marks1 [my index [icons]];
				numberOfMarks2 += marks2 [my index [icons]];
			}
			if (numberOfMarks1 < numberOfMarks2) return -1;   // candidate 1 is better than candidate 2
			if (numberOfMarks1 > numberOfMarks2) return +1;   // candidate 2 is better than candidate 1
		}
		/* If we arrive here, None of the comparisons found a difference between the two candidates. Hence, they are equally good. */
		return 0;
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR ||
		my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
			disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2) return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my constraints [icons]. disharmony > 0.0) {
				disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
				disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
			}
		}
		if (disharmony1 < disharmony2) return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2) return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
		my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += exp (my constraints [icons]. disharmony) * marks1 [icons];
			disharmony2 += exp (my constraints [icons]. disharmony) * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2) return +1;   // candidate 2 is better than candidate 1
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			double constraintDisharmony = my constraints [icons]. disharmony > 1.0 ? my constraints [icons]. disharmony : 1.0;
			disharmony1 += constraintDisharmony * marks1 [icons];
			disharmony2 += constraintDisharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   // candidate 1 is better than candidate 2
		if (disharmony1 > disharmony2) return +1;   // candidate 2 is better than candidate 1
	} else Melder_fatal (U"Unimplemented decision strategy.");
	return 0;   // the two total disharmonies are equal
}

static void _OTGrammar_fillInProbabilities (OTGrammar me, long itab) noexcept {
	OTGrammarTableau tableau = & my tableaus [itab];
	double maximumHarmony = tableau -> candidates [1]. harmony;
	for (long icand = 2; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		if (candidate -> harmony > maximumHarmony) {
			maximumHarmony = candidate -> harmony;
		}
	}
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		candidate -> probability = exp (candidate -> harmony - maximumHarmony);
		Melder_assert (candidate -> probability >= 0.0 && candidate -> probability <= 1.0);
	}
	double sumOfProbabilities = 0.0;
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		sumOfProbabilities += candidate -> probability;
	}
	Melder_assert (sumOfProbabilities > 0.0);   // because at least one of them is 1.0
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		candidate -> probability /= sumOfProbabilities;
	}
}

long OTGrammar_getWinner (OTGrammar me, long itab) noexcept {
	long icand_best = 1;
	if (my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		_OTGrammar_fillInHarmonies (me, itab);
		_OTGrammar_fillInProbabilities (me, itab);
		double cutOff = NUMrandomUniform (0.0, 1.0);
		double sumOfProbabilities = 0.0;
		for (long icand = 1; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
			sumOfProbabilities += my tableaus [itab]. candidates [icand]. probability;
			if (sumOfProbabilities > cutOff) {
				icand_best = icand;
				break;
			}
		}
	} else {
		long numberOfBestCandidates = 1;
		for (long icand = 2; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
			int comparison = OTGrammar_compareCandidates (me, itab, icand, itab, icand_best);
			if (comparison == -1) {
				icand_best = icand;   // the current candidate is the unique best candidate found so far
				numberOfBestCandidates = 1;
			} else if (comparison == 0) {
				numberOfBestCandidates += 1;   // the current candidate is equally good as the best found before
				/*
				 * Give all candidates that are equally good an equal chance to become the winner.
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

long OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, long itab) {
	if (my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY) return 1;
	long icand_best = 1, icand, numberOfBestCandidates = 1;
	for (icand = 2; icand <= my tableaus [itab]. numberOfCandidates; icand ++) {
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

bool OTGrammar_isCandidateGrammatical (OTGrammar me, long itab, long icand) {
	for (long jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++) {
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) < 0)
			return false;
	}
	return true;
}

bool OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, long itab, long icand) {
	for (long jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++) {
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) <= 0)
			return false;
	}
	return true;
}

void OTGrammar_getInterpretiveParse (OTGrammar me, const char32 *partialOutput, long *bestTableau, long *bestCandidate) {
	try {
		long itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				if (str32str (cand -> output, partialOutput)) {   // T&S' idea of surface->overt mapping
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
		if (itab_best == 0) Melder_throw (U"The partial output \"", partialOutput, U"\" does not match any candidate for any input form.");
		if (bestTableau) *bestTableau = itab_best;
		if (bestCandidate) *bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw (U"Interpretive parse not computed.");
	}
}

static void OTGrammar_getInterpretiveParse_opt (OTGrammar me, long ipartialOutput, long *bestTableau, long *bestCandidate) {
	try {
		long itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				Melder_assert (cand -> partialOutputMatches);
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
		if (bestTableau) *bestTableau = itab_best;
		if (bestCandidate) *bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw (U"Interpretive parse not computed.");
	}
}

bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, const char32 *partialOutput) {
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (str32str (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool OTGrammar_areAllPartialOutputsGrammatical (OTGrammar me, Strings thee) {
	for (long ioutput = 1; ioutput <= thy numberOfStrings; ioutput ++) {
		const char32 *partialOutput = thy strings [ioutput];
		if (! OTGrammar_isPartialOutputGrammatical (me, partialOutput)) {
			return false;
		}
	}
	return true;
}

bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, const char32 *partialOutput) {
	bool found = false;
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (str32str (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					found = true;
					/*
					 * All other grammatical candidates should match.
					 */
					for (long jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
						if (OTGrammar_compareCandidates (me, itab, jcand, itab, icand) == 0) {
							if (! str32str (tableau -> candidates [jcand]. output, partialOutput)) {
								return false;   // partial output is multiply optimal
							}
						}
					}
				}
			}
		}
	}
	return found;
}

bool OTGrammar_areAllPartialOutputsSinglyGrammatical (OTGrammar me, Strings thee) {
	for (long ioutput = 1; ioutput <= thy numberOfStrings; ioutput ++) {
		const char32 *partialOutput = thy strings [ioutput];
		if (! OTGrammar_isPartialOutputSinglyGrammatical (me, partialOutput)) {
			return false;
		}
	}
	return true;
}

static int OTGrammar_crucialCell (OTGrammar me, long itab, long icand, long iwinner, long numberOfOptimalCandidates) {
	int icons;
	OTGrammarTableau tableau = & my tableaus [itab];
	if (tableau -> numberOfCandidates < 2) return 0;   // if there is only one candidate, all cells can be greyed
	if (my decisionStrategy != kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) return my numberOfConstraints;   // nothing grey
	if (OTGrammar_compareCandidates (me, itab, icand, itab, iwinner) == 0) {   // candidate equally good as winner?
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			long jcand, secondBest = 0;
			for (jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
				if (OTGrammar_compareCandidates (me, itab, jcand, itab, iwinner) != 0) {   // a non-optimal candidate?
					if (secondBest == 0) {
						secondBest = jcand;   /* First guess. */
					} else if (OTGrammar_compareCandidates (me, itab, jcand, itab, secondBest) < 0) {
						secondBest = jcand;   /* Better guess. */
					}
				}
			}
			if (secondBest == 0) return 0;   // if all candidates are equally good, all cells can be greyed
			return OTGrammar_crucialCell (me, itab, secondBest, iwinner, 1);
		}
	} else {
		int *candidateMarks = tableau -> candidates [icand]. marks;
		int *winnerMarks = tableau -> candidates [iwinner]. marks;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfCandidateMarks = candidateMarks [my index [icons]];
			int numberOfWinnerMarks = winnerMarks [my index [icons]];
			while (my constraints [my index [icons]]. tiedToTheRight)
			{
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

static double OTGrammar_constraintWidth (Graphics g, const char32 *name) {
	char32 text [100], *newLine;
	str32cpy (text, name);
	newLine = str32chr (text, '\n');
	if (newLine) {
		double firstWidth, secondWidth;
		*newLine = '\0';
		firstWidth = Graphics_textWidth (g, text);
		secondWidth = Graphics_textWidth (g, newLine + 1);
		return firstWidth > secondWidth ? firstWidth : secondWidth;
	}
	return Graphics_textWidth (g, text);
}

void OTGrammar_drawTableau (OTGrammar me, Graphics g, bool vertical, const char32 *input) {
	try {
		double x, y, fontSize = Graphics_inqFontSize (g);
		Graphics_Colour colour = Graphics_inqColour (g);
		const long itab = OTGrammar_getTableau (me, input);
		_OTGrammar_fillInHarmonies (me, itab);
		const long winner = OTGrammar_getWinner (me, itab);
		
		Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
		const double margin = Graphics_dxMMtoWC (g, 1.0);
		const double fingerWidth = Graphics_dxMMtoWC (g, 7.0) * fontSize / 12.0;
		const double doubleLineDx = Graphics_dxMMtoWC (g, 0.9);
		const double doubleLineDy = Graphics_dyMMtoWC (g, 0.9);
		const double rowHeight = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72);
		const double descent = rowHeight * 0.5;
		const double worldAspectRatio = Graphics_dyMMtoWC (g, 1.0) / Graphics_dxMMtoWC (g, 1.0);   // because Graphics_textWidth measures in the x direction only
		/*
		 * Compute the height of the header row.
		 */
		double headerHeight;
		if (vertical) {
			headerHeight = 0.0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintTextWidth = Graphics_textWidth (g, constraint -> name);
				if (constraintTextWidth > headerHeight)
					headerHeight = constraintTextWidth;
			}
			headerHeight += margin * 2;
			headerHeight *= worldAspectRatio;
		} else {
			headerHeight = rowHeight;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				if (str32chr (constraint -> name, U'\n')) {
					headerHeight *= 1.6;
					break;
				}
			}
		}
		/*
		 * Compute longest candidate string.
		 * Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
		 */
		double candWidth = Graphics_textWidth (g, input);
		OTGrammarTableau tableau = & my tableaus [itab];
		long numberOfOptimalCandidates = 0;
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			double width = Graphics_textWidth (g, tableau -> candidates [icand]. output);
			if (OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0) {
				width += fingerWidth;
				numberOfOptimalCandidates ++;
			}
			if (width > candWidth) candWidth = width;
		}
		candWidth += margin * 3;
		/*
		 * Compute tableau width.
		 */
		double tableauWidth = candWidth + doubleLineDx;
		if (vertical) {
			tableauWidth += rowHeight * my numberOfConstraints / worldAspectRatio;
		} else {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				tableauWidth += OTGrammar_constraintWidth (g, constraint -> name);
			}
			tableauWidth += margin * 2 * my numberOfConstraints;
		}
		/*
		 * Draw box.
		 */
		x = doubleLineDx;   // left side of tableau
		y = 1.0 - doubleLineDy;
		Graphics_rectangle (g, x, x + tableauWidth,
			y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
		/*
		 * Draw input.
		 */
		y -= headerHeight;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, x + 0.5 * candWidth, y + 0.5 * headerHeight, input);
		Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
		/*
		 * Draw constraint names.
		 */
		x += candWidth + doubleLineDx;
		if (vertical) Graphics_setTextRotation (g, 90.0);
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			double width = vertical ? rowHeight / worldAspectRatio : OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
			if (str32chr (constraint -> name, U'\n') && ! vertical) {
				autoMelderString text;
				MelderString_copy (& text, constraint -> name);
				char32 *newLine = str32chr (text.string, U'\n');
				*newLine = U'\0';
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
				Graphics_text (g, x + 0.5 * width, y + headerHeight, text.string);
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
				Graphics_text (g, x + 0.5 * width, y, newLine + 1);
			} else if (vertical) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				Graphics_text (g, x + 0.5 * width, y + margin, constraint -> name);
			} else {
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
				Graphics_text (g, x + 0.5 * width, y + 0.5 * headerHeight, constraint -> name);
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
		 * Draw candidates.
		 */
		y -= doubleLineDy;
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			long crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
			bool candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
			/*
			 * Draw candidate transcription.
			 */
			x = doubleLineDx;
			y -= rowHeight;
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_text (g, x + candWidth - margin, y + descent, tableau -> candidates [icand]. output);
			if (candidateIsOptimal) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				Graphics_setFontSize (g, (int) (1.5 * fontSize));
				if (numberOfOptimalCandidates > 1) Graphics_setColour (g, Graphics_RED);
				Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 1.0) * fontSize / 12.0, U"☞");
				Graphics_setColour (g, colour);
				Graphics_setFontSize (g, (int) fontSize);
			}
			Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
			/*
			 * Draw grey cell backgrounds.
			 */
			x = candWidth + 2 * doubleLineDx;
			Graphics_setGrey (g, 0.9);
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int index = my index [icons];
				OTGrammarConstraint constraint = & my constraints [index];
				double width = vertical ? rowHeight / worldAspectRatio : OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
				if (icons > crucialCell)
					Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
				x += width;
			}
			Graphics_setColour (g, colour);
			/*
			 * Draw cell marks.
			 */
			x = candWidth + 2 * doubleLineDx;
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int index = my index [icons];
				OTGrammarConstraint constraint = & my constraints [index];
				double width = vertical ? rowHeight / worldAspectRatio : OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
				static MelderString markString;
				MelderString_empty (& markString);
				if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
					/*
					 * An exclamation mark can be drawn in this cell only if all of the following conditions are met:
					 * 1. the candidate is not optimal;
					 * 2. the constraint is not tied;
					 * 3. this is the crucial cell, i.e. the cells after it are drawn in grey.
					 */
					if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
						int winnerMarks = tableau -> candidates [winner]. marks [index];
						for (long imark = 1; imark <= winnerMarks + 1; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							MelderString_appendCharacter (& markString, U'+');
						MelderString_appendCharacter (& markString, U'!');
						for (long imark = winnerMarks + 2; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							Graphics_setColour (g, Graphics_RED);
						}
						for (long imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							MelderString_appendCharacter (& markString, U'+');
					}
				} else {
					for (long imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
						MelderString_appendCharacter (& markString, U'*');
					for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
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
			 * Draw harmony.
			 */
			if (my decisionStrategy != kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
				double value = tableau -> candidates [icand]. harmony;
				if (my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
					my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
				{
					//value = value > 1e-308 ? 1000 : value < -1e308 ? -1000 : - log (- value);
					Graphics_text (g, x, y + descent, Melder_float (Melder_half (value)));
				} else {
					Graphics_text (g, x, y + descent, Melder_fixed (value, 3));
				}
			}
		}
		/*
		 * Draw box.
		 */
		x = doubleLineDx;   // left side of tableau
		y = 1.0 - doubleLineDy;
		Graphics_rectangle (g, x, x + tableauWidth,
			y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
	} catch (MelderError) {
		Melder_throw (me, U": tableau not drawn.");
	}
}

autoStrings OTGrammar_generateInputs (OTGrammar me, long numberOfTrials) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, thy numberOfStrings = numberOfTrials);
		for (long i = 1; i <= numberOfTrials; i ++) {
			long itab = NUMrandomInteger (1, my numberOfTableaus);
			thy strings [i] = Melder_dup (my tableaus [itab]. input);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": inputs not generated.");
	}
}

autoStrings OTGrammar_getInputs (OTGrammar me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <char32 *> (1, thy numberOfStrings = my numberOfTableaus);
		for (long i = 1; i <= my numberOfTableaus; i ++) {
			thy strings [i] = Melder_dup (my tableaus [i]. input);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": inputs not gotten.");
	}
}

void OTGrammar_inputToOutput (OTGrammar me, const char32 *input, char32 *output, double evaluationNoise) {
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		long itab = OTGrammar_getTableau (me, input);
		long winner = OTGrammar_getWinner (me, itab);
		if (winner == 0)
			Melder_throw (U"No winner");
		str32cpy (output, my tableaus [itab]. candidates [winner]. output);
	} catch (MelderError) {
		Melder_throw (me, U": output not computed from input \"", input, U"\".");
	}
}

autoStrings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise) {
	try {
		autoStrings him = Thing_new (Strings);
		long n = inputs -> numberOfStrings;
		his numberOfStrings = n;
		his strings = NUMvector <char32 *> (1, n);
		for (long i = 1; i <= n; i ++) {
			char32 output [100];
			OTGrammar_inputToOutput (me, inputs -> strings [i], output, evaluationNoise);
			his strings [i] = Melder_dup (output);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": outputs not computed.");
	}
}

autoStrings OTGrammar_inputToOutputs (OTGrammar me, const char32 *input, long n, double evaluationNoise) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = n;
		thy strings = NUMvector <char32 *> (1, n);
		for (long i = 1; i <= n; i ++) {
			char32 output [100];
			OTGrammar_inputToOutput (me, input, output, evaluationNoise);
			thy strings [i] = Melder_dup (output);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output not computed.");
	}
}

autoDistributions OTGrammar_to_Distribution (OTGrammar me, long trialsPerInput, double noise) {
	try {
		long totalNumberOfOutputs = 0, nout = 0;
		/*
		 * Count the total number of outputs.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++)
			totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
		/*
		 * Create the distribution. One row for every output form.
		 */
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1); 
		/*
		 * Measure every input form.
		 */
		autoMelderProgress progress (U"OTGrammar: compute output distribution.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input, U"\"");
			/*
			 * Set the row labels to the output strings.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				thy rowLabels [nout + icand] = Melder_dup (Melder_cat (tableau -> input, U" \\-> ", tableau -> candidates [icand]. output));
			}
			/*
			 * Compute a number of outputs and store the results.
			 */
			for (long itrial = 1; itrial <= trialsPerInput; itrial ++) {
				OTGrammar_newDisharmonies (me, noise);
				long iwinner = OTGrammar_getWinner (me, itab);
				thy data [nout + iwinner] [1] += 1;
			}
			/*
			 * Update the offset.
			 */
			nout += tableau -> numberOfCandidates;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output distribution not computed.");
	}
}

autoPairDistribution OTGrammar_to_PairDistribution (OTGrammar me, long trialsPerInput, double noise) {
	try {
		long totalNumberOfOutputs = 0, nout = 0;
		/*
		 * Count the total number of outputs.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++)
			totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
		/*
		 * Create the distribution. One row for every output form.
		 */
		autoPairDistribution thee = PairDistribution_create ();
		/*
		 * Measure every input form.
		 */
		autoMelderProgress progress (U"OTGrammar: compute output distribution.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input, U"\"");
			/*
			 * Copy the input and output strings to the target object.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				PairDistribution_add (thee.get(), tableau -> input, tableau -> candidates [icand]. output, 0.0);
			}
			/*
			 * Compute a number of outputs and store the results.
			 */
			for (long itrial = 1; itrial <= trialsPerInput; itrial ++) {
				OTGrammar_newDisharmonies (me, noise);
				long iwinner = OTGrammar_getWinner (me, itab);
				thy pairs.at [nout + iwinner] -> weight += 1.0;
			}
			/*
			 * Update the offset.
			 */
			nout += tableau -> numberOfCandidates;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output distribution not computed.");
	}
}

static bool honoursFixedRankings (OTGrammar me) {
	for (long i = 1; i <= my numberOfFixedRankings; i ++) {
		long higher = my fixedRankings [i]. higher, lower = my fixedRankings [i]. lower;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my index [icons] == higher) break;   // detected higher before lower: OK
			if (my index [icons] == lower) return false;
		}
	}
	return true;
}

autoDistributions OTGrammar_measureTypology_WEAK (OTGrammar me) {
	try {
		long totalNumberOfOutputs = 0, nout = 0, ncons = my numberOfConstraints, nperm, factorial [1+12];
		if (ncons > 12)
			Melder_throw (U"Cannot handle more than 12 constraints.");
		factorial [0] = 1;
		for (long icons = 1; icons <= ncons; icons ++) {
			factorial [icons] = factorial [icons - 1] * icons;
		}
		nperm = factorial [ncons];
		/*
		 * Count the total number of outputs.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++)
			totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
		/*
		 * Create the distribution. One row for every output form.
		 */
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1);
		/*
		 * Measure every input form.
		 */
		autoMelderProgress progress (U"Measuring typology...");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, U"Measuring input \"", tableau -> input, U"\"");
			/*
			 * Set the row labels to the output strings.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				thy rowLabels [nout + icand] = Melder_dup (Melder_cat (tableau -> input, U" \\-> ", tableau -> candidates [icand]. output));
			}
			/*
			 * Compute a number of outputs and store the results.
			 */
			for (long iperm = 0; iperm < nperm; iperm ++) {
				long permleft = iperm, iwinner;
				/* Initialize to 12345 before permuting. */
				for (long icons = 1; icons <= ncons; icons ++) {
					my index [icons] = icons;
				}
				for (long icons = 1; icons < ncons; icons ++) {
					long fac = factorial [ncons - icons], shift = permleft / fac, dummy;
					/*
					 * Swap constraint with the one at a distance 'shift'.
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
			 * Update the offset.
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
		for (long irank = 1; irank <= my numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
			OTGrammarConstraint higher = & my constraints [fixedRanking -> higher], lower = & my constraints [fixedRanking -> lower];
			while (higher -> ranking <= lower -> ranking) {
				lower -> ranking -= learningStep (plasticity, relativePlasticityNoise);
				if (grammarHasChanged) *grammarHasChanged = true;
				improved = true;
			}
		}
	} while (improved);
}

static void OTGrammar_modifyRankings (OTGrammar me, long itab, long iwinner, long iadult,
	int updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, int warnIfStalled, bool *grammarHasChanged)
{
	try {
		OTGrammarTableau tableau = & my tableaus [itab];
		OTGrammarCandidate winner = & tableau -> candidates [iwinner], adult = & tableau -> candidates [iadult];
		double step = learningStep (plasticity, relativePlasticityNoise);
		bool multiplyStepByNumberOfViolations =
			my decisionStrategy == kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR ||
			my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT ||
			my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY ||
			my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
			my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY;
		if (Melder_debug != 0) {
			/*
			 * Perhaps override the standard update rule.
			 */
			if (Melder_debug == 26) multiplyStepByNumberOfViolations = false;   // OT-GLA
			else if (Melder_debug == 27) multiplyStepByNumberOfViolations = true;   // HG-GLA
		}
		if (updateRule == kOTGrammar_rerankingStrategy_SYMMETRIC_ONE) {
			long icons = NUMrandomInteger (1, my numberOfConstraints);
			OTGrammarConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			int winnerMarks = winner -> marks [icons];
			int adultMarks = adult -> marks [icons];
			if (adultMarks > winnerMarks) {
				if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
				if (grammarHasChanged) *grammarHasChanged = true;
			}
			if (winnerMarks > adultMarks) {
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
				if (grammarHasChanged) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_SYMMETRIC_ALL) {
			bool changed = false;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
					changed = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
					changed = true;
				}
			}
			if (changed && my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG)
			{
				double sumOfWeights = 0.0;
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					sumOfWeights += my constraints [icons]. ranking;
				}
				double averageWeight = sumOfWeights / my numberOfConstraints;
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					my constraints [icons]. ranking -= averageWeight;
				}
			}
			if (grammarHasChanged) *grammarHasChanged = changed;
		} else if (updateRule == kOTGrammar_rerankingStrategy_SYMMETRIC_ALL_SKIPPABLE) {
			bool changed = false;
			int winningConstraints = 0, adultConstraints = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) adultConstraints ++;
				if (winnerMarks > adultMarks) winningConstraints ++;
			}
			if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
					changed = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
					changed = true;
				}
			}
			if (changed && my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG)
			{
				double sumOfWeights = 0.0;
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					sumOfWeights += my constraints [icons]. ranking;
				}
				double averageWeight = sumOfWeights / my numberOfConstraints;
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					my constraints [icons]. ranking -= averageWeight;
				}
			}
			if (grammarHasChanged) *grammarHasChanged = changed;
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_UNCANCELLED) {
			int winningConstraints = 0, adultConstraints = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) adultConstraints ++;
				if (winnerMarks > adultMarks) winningConstraints ++;
			}
			if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / adultConstraints;
					//constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) * winningConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
				if (winnerMarks > adultMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					//constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * adultConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL) {
			int winningConstraints = 0, adultConstraints = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > 0) adultConstraints ++;
				if (winnerMarks > 0) winningConstraints ++;
			}
			if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (adultMarks > 0) {
					if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks /*- winnerMarks*/;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / adultConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
				if (winnerMarks > 0) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks /*- adultMarks*/;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_EDCD || updateRule == kOTGrammar_rerankingStrategy_EDCD_WITH_VACATION) {
			/*
			 * Determine the crucial winner mark.
			 */
			double pivotRanking;
			bool equivalent = true;
			long icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				int adultMarks = adult -> marks [my index [icons]];
				if (adultMarks < winnerMarks) break;
				if (adultMarks > winnerMarks) equivalent = false;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (warnIfStalled && ! equivalent)
					Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
						U"Input: ", tableau -> input, U"\nCorrect output: ", adult -> output, U"\nLearner's output: ", winner -> output);
				return;   // Tesar & Smolensky (2000: 67): "stopped dead in its tracks"
			}
			/*
			 * Determine the stratum into which some constraints will be demoted.
			 */
			pivotRanking = my constraints [my index [icons]]. ranking;
			if (updateRule == kOTGrammar_rerankingStrategy_EDCD_WITH_VACATION) {
				long numberOfConstraintsToDemote = 0;
				for (icons = 1; icons <= my numberOfConstraints; icons ++) {
					int winnerMarks = winner -> marks [icons];
					int adultMarks = adult -> marks [icons];
					if (adultMarks > winnerMarks) {
						OTGrammarConstraint constraint = & my constraints [icons];
						if (constraint -> ranking >= pivotRanking) {
							numberOfConstraintsToDemote += 1;
						}
					}
				}
				if (numberOfConstraintsToDemote > 0) {
					for (icons = 1; icons <= my numberOfConstraints; icons ++) {
						OTGrammarConstraint constraint = & my constraints [icons];
						if (constraint -> ranking < pivotRanking) {
							constraint -> ranking -= numberOfConstraintsToDemote * step * constraint -> plasticity;
							if (grammarHasChanged) *grammarHasChanged = true;
						}
					}
				}
			}
			/*
			 * Demote all the uniquely violated constraints in the adult form
			 * that have rankings not lower than the pivot.
			 */
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				long numberOfConstraintsDemoted = 0;
				int winnerMarks = winner -> marks [my index [icons]];   // for the vacation version, the order is important, therefore indirect
				int adultMarks = adult -> marks [my index [icons]];
				if (adultMarks > winnerMarks) {
					OTGrammarConstraint constraint = & my constraints [my index [icons]];
					double constraintStep = step * constraint -> plasticity;
					if (constraint -> ranking >= pivotRanking) {
						numberOfConstraintsDemoted += 1;
						constraint -> ranking = pivotRanking - numberOfConstraintsDemoted * constraintStep;   // this preserves the order of the demotees
						if (grammarHasChanged) *grammarHasChanged = true;
					}
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_DEMOTION_ONLY) {
			/*
			 * Determine the crucial adult mark.
			 */
			long crucialAdultMark;
			OTGrammarConstraint offendingConstraint;
			long icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, so we indirect
				int adultMarks = adult -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (adultMarks < winnerMarks)
					Melder_throw (U"Demotion-only learning step: Adult form wins! Should never happen.");
				if (adultMarks > winnerMarks) break;
			}
			if (icons > my numberOfConstraints)   // completed the loop?
				Melder_throw (U"Adult form equals correct candidate.");
			crucialAdultMark = icons;
			/*
			 * Demote the highest uniquely violated constraint in the adult form.
			 */
			offendingConstraint = & my constraints [my index [crucialAdultMark]];
			double constraintStep = step * offendingConstraint -> plasticity;
			offendingConstraint -> ranking -= constraintStep;
			if (grammarHasChanged) *grammarHasChanged = true;
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN) {
			long numberOfUp = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (winnerMarks > adultMarks) {
					numberOfUp ++;
				}
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [icons];
					int adultMarks = adult -> marks [icons];
					if (winnerMarks > adultMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / numberOfUp;
						if (grammarHasChanged) *grammarHasChanged = true;
					}
				}
				long crucialAdultMark, winnerMarks = 0, adultMarks = 0;
				OTGrammarConstraint offendingConstraint;
				long icons = 1;
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
				crucialAdultMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the adult form.
				 */
				offendingConstraint = & my constraints [my index [crucialAdultMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (grammarHasChanged) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN_2012) {
			long numberOfUp = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int adultMarks = adult -> marks [icons];
				if (winnerMarks > adultMarks) {
					numberOfUp ++;
				}
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [icons];
					int adultMarks = adult -> marks [icons];
					if (winnerMarks > adultMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / (numberOfUp + 1);
						if (grammarHasChanged) *grammarHasChanged = true;
					}
				}
				long crucialAdultMark, winnerMarks = 0, adultMarks = 0;
				OTGrammarConstraint offendingConstraint;
				long icons = 1;
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
				crucialAdultMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the adult form.
				 */
				offendingConstraint = & my constraints [my index [crucialAdultMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (grammarHasChanged) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGH_DOWN) {
			long numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				int adultMarks = adult -> marks [my index [icons]];
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
					U"Input: ", tableau -> input, U"\nCorrect output: ", adult -> output, U"\nLearner's output: ", winner -> output);
				return;
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					long constraintIndex = my index [icons];
					OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					int adultMarks = adult -> marks [constraintIndex];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					if (adultMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 0.0);
					} else if (adultMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (grammarHasChanged) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGH_DOWN_2012) {
			long numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				int adultMarks = adult -> marks [my index [icons]];
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
					U"Input: ", tableau -> input, U"\nCorrect output: ", adult -> output, U"\nLearner's output: ", winner -> output);
				return;
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					long constraintIndex = my index [icons];
					OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					int adultMarks = adult -> marks [constraintIndex];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
					if (adultMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - adultMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 1.0);
					} else if (adultMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations) constraintStep *= adultMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (grammarHasChanged) *grammarHasChanged = true;
			}
		}
		if (honourLocalRankings && my numberOfFixedRankings) {
			OTGrammar_honourLocalRankings (me, plasticity, relativePlasticityNoise, grammarHasChanged);
		}
	} catch (MelderError) {
		Melder_throw (me, U": rankings not modified.");
	}
}

void OTGrammar_learnOne (OTGrammar me, const char32 *input, const char32 *adultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, bool newDisharmonies, bool warnIfStalled, bool *grammarHasChanged)
{
	try {
		if (newDisharmonies) OTGrammar_newDisharmonies (me, evaluationNoise);
		if (grammarHasChanged) *grammarHasChanged = false;

		/*
		 * Evaluate the input in the learner's hypothesis.
		 */
		long itab = OTGrammar_getTableau (me, input);
		OTGrammarTableau tableau = & my tableaus [itab];

		/*
		 * Determine the "winner", i.e. the candidate that wins in the learner's grammar
		 * (Tesar & Smolensky call this the "loser").
		 */
		long iwinner = OTGrammar_getWinner (me, itab);
		OTGrammarCandidate winner = & tableau -> candidates [iwinner];

		/*
		 * Error-driven: compare the adult winner (the correct candidate) and the learner's winner.
		 */
		if (str32equ (winner -> output, adultOutput)) return;   // as far as we know, the grammar is already correct: don't update rankings

		/*
		 * Find (perhaps the learner's interpretation of) the adult output in the learner's own tableau
		 * (Tesar & Smolensky call this the "winner").
		 */
		long iadult = 1;
		for (; iadult <= tableau -> numberOfCandidates; iadult ++) {
			OTGrammarCandidate cand = & tableau -> candidates [iadult];
			if (str32equ (cand -> output, adultOutput)) break;
		}
		if (iadult > tableau -> numberOfCandidates)
			Melder_throw (U"Cannot generate adult output \"", adultOutput, U"\".");

		/*
		 * Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
		 * The grammar will have to change.
		 */
		OTGrammar_modifyRankings (me, itab, iwinner, iadult, updateRule, honourLocalRankings,
			plasticity, relativePlasticityNoise, warnIfStalled, grammarHasChanged);
	} catch (MelderError) {
		Melder_throw (me, U": not learned from input \"", input, U"\" and adult output \"", adultOutput, U"\".");
	}
}

void OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews)
{
	if (! inputs) inputs = outputs;
	try {
		long n = inputs -> numberOfStrings;
		if (outputs -> numberOfStrings != n)
			Melder_throw (U"Numbers of strings in input and output are not equal.");
		for (long i = 1; i <= n; i ++) {
			for (long ichew = 1; ichew <= numberOfChews; ichew ++) {
				OTGrammar_learnOne (me, inputs -> strings [i], outputs -> strings [i],
					evaluationNoise, updateRule, honourLocalRankings,
					plasticity, relativePlasticityNoise, true, true, nullptr);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from ", inputs, U" (inputs) and ", outputs, U" (outputs).");
	}
}

void OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews)
{
	long idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		double plasticity = initialPlasticity;
		autoMelderMonitor monitor (U"Learning with full knowledge...");
		if (monitor.graphics()) {
			Graphics_clearWs (monitor.graphics());
		}
		for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
			for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
				char32 *input, *output;
				PairDistribution_peekPair (thee, & input, & output);
				++ idatum;
				if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
					Graphics_beginMovieFrame (monitor.graphics(), nullptr);
					Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50, 150);
					for (long icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
						Graphics_setGrey (monitor.graphics(), (double) icons / 14);
						Graphics_line (monitor.graphics(), idatum, my constraints [icons]. ranking,
							idatum, my constraints [icons]. ranking+1);
					}
					Graphics_endMovieFrame (monitor.graphics(), 0.0);
				}
				Melder_monitor ((double) idatum / numberOfData,
					U"Processing input-output pair ", idatum,
					U" out of ", numberOfData, U": ", input, U" -> ", output);
				for (long ichew = 1; ichew <= numberOfChews; ichew ++) {
					OTGrammar_learnOne (me, input, output,
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, true, true, nullptr);
				}
			}
			plasticity *= plasticityDecrement;
		}
	} catch (MelderError) {
		if (idatum > 1)
			Melder_appendError (U"Only ", idatum - 1, U" input-output pairs out of ", numberOfData, U" were processed.");
		Melder_throw (me, U": did not complete learning from ", thee, U".");
	}
}

static long PairDistribution_getNumberOfAttestedOutputs (PairDistribution me, const char32 *input, char32 **attestedOutput) {
	long result = 0;
	for (long ipair = 1; ipair <= my pairs.size; ipair ++) {
		PairProbability pair = my pairs.at [ipair];
		if (str32equ (pair -> string1, input) && pair -> weight > 0.0) {
			if (attestedOutput) *attestedOutput = pair -> string2;
			result ++;
		}
	}
	return result;
}

bool OTGrammar_PairDistribution_findPositiveWeights_e (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation) {
	NUMlinprog linprog = nullptr;
	try {
		bool result = false;
		if (my decisionStrategy != kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR &&
			my decisionStrategy != kOTGrammar_decisionStrategy_LINEAR_OT &&
			my decisionStrategy != kOTGrammar_decisionStrategy_POSITIVE_HG &&
			my decisionStrategy != kOTGrammar_decisionStrategy_EXPONENTIAL_HG)
		{
			Melder_throw (U"To find positive weights, the decision strategy has to be HarmonicGrammar, LinearOT, PositiveHG, or ExponentialHG.");
		}
		autoNUMvector <long> optimalCandidates (1, my numberOfTableaus);
		/*
		 * Check that there is exactly one optimal output for each input.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			char32 *attestedOutput = nullptr;
			long numberOfAttestedOutputs = PairDistribution_getNumberOfAttestedOutputs (thee, tab -> input, & attestedOutput);
			if (numberOfAttestedOutputs == 0) {
				Melder_throw (U"Input \"", my tableaus [itab]. input, U"\" has no attested output.");
			} else if (numberOfAttestedOutputs > 1) {
				Melder_throw (U"Input \"", my tableaus [itab]. input, U"\" has more than one attested output.");
			} else {
				Melder_assert (attestedOutput);
				for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
					OTGrammarCandidate cand = & tab -> candidates [icand];
					if (str32equ (attestedOutput, cand -> output)) {
						optimalCandidates [itab] = icand;
					}
				}
			}
			Melder_assert (optimalCandidates [itab] != 0);
		}
		/*
		 * Create linear programming problem.
		 */
		linprog = NUMlinprog_new (false);
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			NUMlinprog_addVariable (linprog, weightFloor, undefined, 1.0);
		}
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			long ioptimalCandidate = optimalCandidates [itab];
			Melder_assert (ioptimalCandidate >= 1);
			Melder_assert (ioptimalCandidate <= tab -> numberOfCandidates);
			OTGrammarCandidate optimalCandidate = & tab -> candidates [ioptimalCandidate];
			for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) if (icand != ioptimalCandidate) {
				OTGrammarCandidate cand = & tab -> candidates [icand];
				NUMlinprog_addConstraint (linprog, marginOfSeparation, undefined);
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					NUMlinprog_addConstraintCoefficient (linprog, cand -> marks [icons] - optimalCandidate -> marks [icons]);
				}
			}
		}
		NUMlinprog_run (linprog);
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			double weighting = NUMlinprog_getPrimalValue (linprog, icons);
			Melder_assert (weighting >= weightFloor);
			my constraints [icons]. ranking = my constraints [icons]. disharmony =
				my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ? log (weighting) : weighting;
		}
		NUMlinprog_delete (linprog);
		return result;
	} catch (MelderError) {
		NUMlinprog_delete (linprog);
		Melder_throw (me, U": positive weights not found.");
	}
}

void OTGrammar_reset (OTGrammar me, double ranking) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTGrammar_sort (me);
}

void OTGrammar_resetToRandomRanking (OTGrammar me, double mean, double standardDeviation) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> disharmony = constraint -> ranking = NUMrandomGauss (mean, standardDeviation);
	}
	OTGrammar_sort (me);
}

void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance) {
	/*
	 * First put the constraints in a random order and build a random index.
	 */
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> ranking = 0.0;
	}
	OTGrammar_newDisharmonies (me, 1.0);
	/*
	 * Then use the random index to yield a cascade of rankings.
	 */
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> disharmony = constraint -> ranking = maximumRanking - (icons - 1) * rankingDistance;
	}
	OTGrammar_sort (me);
}

void OTGrammar_setRanking (OTGrammar me, long constraint, double ranking, double disharmony) {
	try {
		if (constraint < 1 || constraint > my numberOfConstraints)
			Melder_throw (U"There is no constraint with number ", constraint, U".");
		my constraints [constraint]. ranking = ranking;
		my constraints [constraint]. disharmony = disharmony;
		OTGrammar_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": ranking of constraint ", constraint, U" not set.");
	}
}

void OTGrammar_setConstraintPlasticity (OTGrammar me, long constraint, double plasticity) {
	try {
		if (constraint < 1 || constraint > my numberOfConstraints)
			Melder_throw (U"There is no constraint with number ", constraint, U".");
		my constraints [constraint]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, U": plasticity of constraint ", constraint, U" not set.");
	}
}

long theSaveNumberOfConstraints, *theSaveIndex;
double *theSaveRankings, *theSaveDisharmonies;
int *theSaveTiedToTheLeft, *theSaveTiedToTheRight;
static void OTGrammar_save (OTGrammar me) {
	if (my numberOfConstraints != theSaveNumberOfConstraints) {
		NUMvector_free (theSaveIndex, 1); theSaveIndex = nullptr;
		NUMvector_free (theSaveRankings, 1); theSaveRankings = nullptr;
		NUMvector_free (theSaveDisharmonies, 1); theSaveDisharmonies = nullptr;
		NUMvector_free (theSaveTiedToTheLeft, 1); theSaveTiedToTheLeft = nullptr;
		NUMvector_free (theSaveTiedToTheRight, 1); theSaveTiedToTheRight = nullptr;
		theSaveNumberOfConstraints = my numberOfConstraints;
	}
	if (! theSaveIndex) theSaveIndex = NUMvector <long> (1, my numberOfConstraints);
	if (! theSaveRankings) theSaveRankings = NUMvector <double> (1, my numberOfConstraints);
	if (! theSaveDisharmonies) theSaveDisharmonies = NUMvector <double> (1, my numberOfConstraints);
	if (! theSaveTiedToTheLeft) theSaveTiedToTheLeft = NUMvector <int> (1, my numberOfConstraints);
	if (! theSaveTiedToTheRight) theSaveTiedToTheRight = NUMvector <int> (1, my numberOfConstraints);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		theSaveIndex [icons] = my index [icons];
		theSaveRankings [icons] = my constraints [icons]. ranking;
		theSaveDisharmonies [icons] = my constraints [icons]. disharmony;
		theSaveTiedToTheLeft [icons] = my constraints [icons]. tiedToTheLeft;
		theSaveTiedToTheRight [icons] = my constraints [icons]. tiedToTheRight;
	}
}
static void OTGrammar_restore (OTGrammar me) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		my index [icons] = theSaveIndex [icons];
		my constraints [icons]. ranking = theSaveRankings [icons];
		my constraints [icons]. disharmony = theSaveDisharmonies [icons];
		my constraints [icons]. tiedToTheLeft = theSaveTiedToTheLeft [icons];
		my constraints [icons]. tiedToTheRight = theSaveTiedToTheRight [icons];
	}
}

void OTGrammar_learnOneFromPartialOutput (OTGrammar me, const char32 *partialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews, bool warnIfStalled)
{
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD) {
			OTGrammar_save (me);
		}
		long ichew = 1;
		for (; ichew <= numberOfChews; ichew ++) {
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			bool grammarHasChanged = false;
			OTGrammar_learnOne (me,
				my tableaus [assumedAdultInputTableau]. input,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output,
				evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticityNoise, Melder_debug == 47, warnIfStalled, & grammarHasChanged);
			if (! grammarHasChanged) return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD && ichew > numberOfChews) {
			/*
			 * Is the partial output form grammatical by now?
			 */
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (! str32equ (learnerCandidate -> output,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
			{   /* Still ungrammatical? */
				/*
				 * Backtrack as in Tesar & Smolensky 2000:69.
				 */
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from partial adult output \"", partialAdultOutput, U"\".");
	}
}

static void OTGrammar_learnOneFromPartialOutput_opt (OTGrammar me, const char32 *partialAdultOutput, long ipartialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews, bool warnIfStalled,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, long resampleForCorrectForm)
{
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD) {
			OTGrammar_save (me);
		}
		long ichew = 1;
		for (; ichew <= numberOfChews; ichew ++) {
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarTableau tableau = & my tableaus [assumedAdultInputTableau];
			OTGrammarCandidate assumedCorrect = & tableau -> candidates [assumedAdultCandidate];

			/*
			 * Determine the "winner", i.e. the candidate that wins in the learner's grammar
			 * (Tesar & Smolensky call this the "loser").
			 */
			if (resampleForVirtualProduction) OTGrammar_newDisharmonies (me, evaluationNoise);
			long iwinner = OTGrammar_getWinner (me, assumedAdultInputTableau);
			OTGrammarCandidate winner = & tableau -> candidates [iwinner];

			/*
			 * Error-driven: compare the adult winner (the correct candidate) and the learner's winner.
			 */
			if (compareOnlyPartialOutput) {
				if (str32str (winner -> output, partialAdultOutput)) return;   // as far as we know, the grammar is already correct: don't update rankings
			} else {
				if (str32equ (winner -> output, assumedCorrect -> output)) return;   // as far as we know, the grammar is already correct: don't update rankings
			}
			if (resampleForCorrectForm) {
				long itry = 1;
				for (; itry <= resampleForCorrectForm; itry ++) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
					long iwinner2 = OTGrammar_getWinner (me, assumedAdultInputTableau);
					OTGrammarCandidate winner2 = & tableau -> candidates [iwinner2];
					if (compareOnlyPartialOutput) {
						if (str32str (winner2 -> output, partialAdultOutput)) { assumedAdultCandidate = iwinner2; break; }
					} else {
						if (str32equ (winner2 -> output, assumedCorrect -> output)) { assumedAdultCandidate = iwinner2; break; }
					}
				}
				if (itry > resampleForCorrectForm) return;   // no match, so bail out
			}

			/*
			 * Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
			 * The grammar will have to change.
			 */
			bool grammarHasChanged = false;
			OTGrammar_modifyRankings (me, assumedAdultInputTableau, iwinner, assumedAdultCandidate, updateRule, honourLocalRankings,
				plasticity, relativePlasticityNoise, warnIfStalled, & grammarHasChanged);
			if (! grammarHasChanged) return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD && ichew > numberOfChews) {
			/*
			 * Is the partial output form grammatical by now?
			 */
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (! str32equ (learnerCandidate -> output,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
			{   /* Still ungrammatical? */
				/*
				 * Backtrack as in Tesar & Smolensky 2000:69.
				 */
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not learned from partial adult output ", partialAdultOutput, U".");
	}
}

static autoOTHistory OTGrammar_createHistory (OTGrammar me, long storeHistoryEvery, long numberOfData) {
	try {
		long numberOfSamplingPoints = numberOfData / storeHistoryEvery, icons;   // e.g. 0, 20, 40, ...
		autoOTHistory thee = Thing_new (OTHistory);
		TableOfReal_init (thee.get(), 2 + numberOfSamplingPoints * 2, 1 + my numberOfConstraints);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Datum");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			TableOfReal_setColumnLabel (thee.get(), icons + 1, my constraints [icons]. name);
		}
		TableOfReal_setRowLabel (thee.get(), 1, U"Initial state");
		thy data [1] [1] = 0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			thy data [1] [icons + 1] = my constraints [icons]. ranking;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": history not created.");
	}
}

static void OTGrammar_updateHistory (OTGrammar me, OTHistory thee, long storeHistoryEvery, long idatum, const char32 *input) {
	try {
		if (idatum % storeHistoryEvery == 0) {
			long irow = 2 * idatum / storeHistoryEvery;
			TableOfReal_setRowLabel (thee, irow, input);
			thy data [irow] [1] = idatum;
			thy data [irow + 1] [1] = idatum;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				thy data [irow] [icons + 1] = my constraints [icons]. disharmony;
				thy data [irow + 1] [icons + 1] = my constraints [icons]. ranking;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": history not updated.");
	}
}

static void OTGrammar_finalizeHistory (OTGrammar me, OTHistory thee, long idatum) {
	try {
		TableOfReal_setRowLabel (thee, thy numberOfRows, U"Final state");
		thy data [thy numberOfRows] [1] = idatum;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			thy data [thy numberOfRows] [icons + 1] = my constraints [icons]. ranking;
		}
	} catch (MelderError) {
		Melder_throw (me, U": history not finalized.");
	}
}

void OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, autoOTHistory *history_out)
{
	try {
		autoOTHistory history;
		if (storeHistoryEvery) {
			history = OTGrammar_createHistory (me, storeHistoryEvery, partialOutputs -> numberOfStrings);
		}
		try {
			for (long idatum = 1; idatum <= partialOutputs -> numberOfStrings; idatum ++) {
				try {
					OTGrammar_learnOneFromPartialOutput (me, partialOutputs -> strings [idatum],
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, numberOfChews, false);
				} catch (MelderError) {
					if (history) {
						OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum]);   // so that we can inspect
					}
					throw;
				}
				if (history) {
					OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum]);
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
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tab = & my tableaus [itab];
		for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
			OTGrammarCandidate cand = & tab -> candidates [icand];
			cand -> numberOfPotentialPartialOutputsMatching = 0;
			NUMvector_free (cand -> partialOutputMatches, 1);
			cand -> partialOutputMatches = nullptr;
		}
	}
}

static void OTGrammar_Distributions_opt_createOutputMatching (OTGrammar me, Distributions thee, long columnNumber) {
	try {
		if (columnNumber > thy numberOfColumns)
			Melder_throw (U"No column ", columnNumber, U" in Distributions.");
		if (thy numberOfRows < 1)
			Melder_throw (U"No candidates in Distributions.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tab -> candidates [icand];
				cand -> numberOfPotentialPartialOutputsMatching = thy numberOfRows;
				cand -> partialOutputMatches = NUMvector <bool> (1, thy numberOfRows);
			}
		}
		for (long ipartialOutput = 1; ipartialOutput <= thy numberOfRows; ipartialOutput ++) {
			if (thy data [ipartialOutput] [columnNumber] > 0.0) {
				char32 *partialOutput = thy rowLabels [ipartialOutput];
				bool foundPartialOutput = false;
				for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
					OTGrammarTableau tab = & my tableaus [itab];
					for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
						OTGrammarCandidate cand = & tab -> candidates [icand];
						if (str32str (cand -> output, partialOutput)) {
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

void OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, bool honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, autoOTHistory *history_out,
	bool resampleForVirtualProduction, bool compareOnlyPartialOutput, long resampleForCorrectForm)
{
	long idatum = 0;
	const long numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		autoOTHistory history;
		OTGrammar_Distributions_opt_createOutputMatching (me, thee, columnNumber);
		autoMelderMonitor monitor (U"Learning with limited knowledge...");
		if (monitor.graphics()) {
			Graphics_clearWs (monitor.graphics());
		}
		if (storeHistoryEvery) {
			history = OTGrammar_createHistory (me, storeHistoryEvery, numberOfData);
		}
		try {
			double plasticity = initialPlasticity;
			for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
				for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
					char32 *partialOutput;
					long ipartialOutput;
					Distributions_peek (thee, columnNumber, & partialOutput, & ipartialOutput);
					++ idatum;
					if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
						Graphics_beginMovieFrame (monitor.graphics(), nullptr);
						Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50, 150);
						for (long icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
							Graphics_setGrey (monitor.graphics(), (double) icons / 14);
							Graphics_line (monitor.graphics(), idatum, my constraints [icons]. ranking,
								idatum, my constraints [icons]. ranking+1);
						}
						Graphics_endMovieFrame (monitor.graphics(), 0.0);
					}
					Melder_monitor ((double) idatum / numberOfData,
						U"Processing partial output ", idatum, U" out of ", numberOfData, U": ",
						thy rowLabels [ipartialOutput]);
					try {
						OTGrammar_learnOneFromPartialOutput_opt (me, partialOutput, ipartialOutput,
							evaluationNoise, updateRule, honourLocalRankings,
							plasticity, relativePlasticityNoise, numberOfChews, false,
							resampleForVirtualProduction, compareOnlyPartialOutput, resampleForCorrectForm);   // no warning if stalled: RIP form is allowed to be harmonically bounded
					} catch (MelderError) {
						if (history) {
							OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput]);
						}
						throw;
					}
					if (history) {
						OTGrammar_updateHistory (me, history.get(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput]);
					}
				}
				plasticity *= plasticityDecrement;
			}
			if (history) {
				OTGrammar_finalizeHistory (me, history.get(), numberOfData);
			}
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
	double evaluationNoise, long numberOfInputs)
{
	try {
		long numberOfCorrect = 0;
		for (long ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
			char32 *input, *adultOutput;
			PairDistribution_peekPair (thee, & input, & adultOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			long inputTableau = OTGrammar_getTableau (me, input);
			OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [OTGrammar_getWinner (me, inputTableau)];
			if (str32equ (learnerCandidate -> output, adultOutput))
				numberOfCorrect ++;
		}
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": fraction correct not computed.");
	}
}

long OTGrammar_PairDistribution_getMinimumNumberCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfReplications)
{
	try {
		long minimumNumberCorrect = numberOfReplications;
		for (long ipair = 1; ipair <= thy pairs.size; ipair ++) {
			PairProbability prob = thy pairs.at [ipair];
			if (prob -> weight > 0.0) {
				long numberOfCorrect = 0;
				char32 *input = prob -> string1, *adultOutput = prob -> string2;
				long inputTableau = OTGrammar_getTableau (me, input);
				for (long ireplication = 1; ireplication <= numberOfReplications; ireplication ++) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
					OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [OTGrammar_getWinner (me, inputTableau)];
					if (str32equ (learnerCandidate -> output, adultOutput))
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

double OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, long numberOfInputs)
{
	try {
		long numberOfCorrect = 0;
		OTGrammar_Distributions_opt_createOutputMatching (me, thee, columnNumber);
		for (long ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
			long ipartialOutput;
			Distributions_peek (thee, columnNumber, nullptr, & ipartialOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (str32equ (learnerCandidate -> output, my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
				numberOfCorrect ++;
		}
		OTGrammar_opt_deleteOutputMatching (me);
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": fraction correct not computed.");
	}
}

void OTGrammar_removeConstraint (OTGrammar me, const char32 *constraintName) {
	try {
		long removed = 0;

		if (my numberOfConstraints <= 1)
			Melder_throw (U"Cannot remove last remaining constraint.");

		/*
		 * Look for the constraint to be removed.
		 */
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [icons];
			if (str32equ (constraint -> name, constraintName)) {
				removed = icons;
				break;
			}
		}
		if (removed == 0)
			Melder_throw (U"No such constraint.");
		/*
		 * Remove the constraint while reusing the memory space.
		 */
		my numberOfConstraints -= 1;
		/*
		 * Shift constraints.
		 */
		Melder_free (my constraints [removed]. name);
		for (long icons = removed; icons <= my numberOfConstraints; icons ++) {
			my constraints [icons] = my constraints [icons + 1];
		}
		/*
		 * Remove or shift fixed rankings.
		 */
		for (long ifixed = my numberOfFixedRankings; ifixed > 0; ifixed --) {
			OTGrammarFixedRanking fixed = & my fixedRankings [ifixed];
			if (fixed -> higher == removed || fixed -> lower == removed) {
				/*
				 * Remove fixed ranking.
				 */
				my numberOfFixedRankings -= 1;
				if (my numberOfFixedRankings == 0) {
					NUMvector_free <structOTGrammarFixedRanking> (my fixedRankings, 1);
					my fixedRankings = nullptr;
				}
				for (long jfixed = ifixed; jfixed <= my numberOfFixedRankings; jfixed ++) {
					my fixedRankings [jfixed] = my fixedRankings [jfixed + 1];
				}
			} else {
				/*
				 * Shift fixed ranking.
				 */
				if (fixed -> higher > removed) fixed -> higher -= 1;
				if (fixed -> lower > removed) fixed -> lower -= 1;
			}
		}
		/*
		 * Shift tableau rows.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate candidate = & tableau -> candidates [icand];
				candidate -> numberOfConstraints -= 1;
				for (long icons = removed; icons <= my numberOfConstraints; icons ++) {
					candidate -> marks [icons] = candidate -> marks [icons + 1];
				}
			}
		}
		/*
		 * Rebuild index.
		 */
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
		OTGrammar_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": constraint \"", constraintName, U"\" not removed.");
	}
}

static void OTGrammarTableau_removeCandidate_unstripped (OTGrammarTableau me, long candidateNumber) {
	Melder_assert (candidateNumber >= 1);
	if (candidateNumber > my numberOfCandidates) Melder_fatal (U"icand ", candidateNumber, U", ncand ", my numberOfCandidates);
	/*
	 * Free up memory associated with this candidate.
	 */
	Melder_free (my candidates [candidateNumber]. output);
	NUMvector_free (my candidates [candidateNumber]. marks, 1);   // dangle
	/*
	 * Remove.
	 */
	my numberOfCandidates -= 1;
	/*
	 * Shift.
	 */
	for (long jcand = candidateNumber; jcand <= my numberOfCandidates; jcand ++) {
		OTGrammarCandidate candj = & my candidates [jcand];
		OTGrammarCandidate candj1 = & my candidates [jcand + 1];
		candj -> output = candj1 -> output;
		candj -> marks = candj1 -> marks;   // undangle
	}
}

static bool OTGrammarTableau_isHarmonicallyBounded (OTGrammarTableau me, long icand, long jcand) {
	OTGrammarCandidate candi = & my candidates [icand], candj = & my candidates [jcand];
	bool equal = true;
	if (icand == jcand) return false;
	for (long icons = 1; icons <= candi -> numberOfConstraints; icons ++) {
		if (candi -> marks [icons] < candj -> marks [icons]) return false;
		if (candi -> marks [icons] > candj -> marks [icons]) equal = false;
	}
	return ! equal;
}

static bool OTGrammarTableau_candidateIsPossibleWinner (OTGrammar me, long itab, long icand) {
	OTGrammar_save (me);
	OTGrammar_reset (me, 100.0);
	for (;;) {
		bool grammarHasChanged = false;
		OTGrammar_learnOne (me, my tableaus [itab]. input, my tableaus [itab]. candidates [icand]. output,
			1e-3, kOTGrammar_rerankingStrategy_EDCD, false, 1.0, 0.0, true, true, & grammarHasChanged);
		if (! grammarHasChanged) {
			OTGrammar_restore (me);
			return true;
		}
		double previousStratum = 101.0;
		OTGrammar_newDisharmonies (me, 0.0);
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			double stratum = my constraints [my index [icons]]. ranking;
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
		 * First, the candidates that are harmonically bounded by one or more single other candidates have to be removed;
		 * otherwise, EDCD will stall.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (long icand = tab -> numberOfCandidates; icand >= 1; icand --) {
				for (long jcand = 1; jcand <= tab -> numberOfCandidates; jcand ++) {
					if (OTGrammarTableau_isHarmonicallyBounded (tab, icand, jcand)) {
						OTGrammarTableau_removeCandidate_unstripped (tab, icand);
						break;
					}
				}
			}
			tab -> candidates = (OTGrammarCandidate) realloc (& tab -> candidates [1], sizeof (struct structOTGrammarCandidate) * tab -> numberOfCandidates) - 1;
		}
		if (! singly) {
			for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
				OTGrammarTableau tab = & my tableaus [itab];
				for (long icand = tab -> numberOfCandidates; icand >= 1; icand --) {
					if (! OTGrammarTableau_candidateIsPossibleWinner (me, itab, icand)) {
						OTGrammarTableau_removeCandidate_unstripped (tab, icand);
					}
				}
				tab -> candidates = (OTGrammarCandidate) realloc (& tab -> candidates [1], sizeof (struct structOTGrammarCandidate) * tab -> numberOfCandidates) - 1;
			}	
		}
	} catch (MelderError) {
		Melder_throw (me, U": not all harmonically bounded candidates were removed.");
	}
}

Thing_define (OTGrammar_List4, Daata) {
	// new data:
	public:
		long hi1, lo1, hi2, lo2;
};

Thing_implement (OTGrammar_List4, Daata, 0);

void OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee) {
	/*
	 * Save.
	 */
	OTGrammarFixedRanking savedFixedRankings = my fixedRankings;   // dangle...
	my fixedRankings = nullptr;   // ...undangle
	long savedNumberOfFixedRankings = my numberOfFixedRankings;
	OTGrammar_save (me);
	try {
		long ifixedRanking, icons, jcons, kcons, lcons, ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
		long ilist, jlist, itrial, iform;
		bool improved;
		double evaluationNoise = 1e-9;
		/*
		 * Add room for two more fixed rankings.
		 */
		my fixedRankings = NUMvector <structOTGrammarFixedRanking> (1, my numberOfFixedRankings + 2);
		for (ifixedRanking = 1; ifixedRanking <= my numberOfFixedRankings; ifixedRanking ++) {
			my fixedRankings [ifixedRanking]. higher = savedFixedRankings [ifixedRanking]. higher;
			my fixedRankings [ifixedRanking]. lower = savedFixedRankings [ifixedRanking]. lower;
		}
		/*
		 * Test whether there are rankings at all for these output data.
		 */
		OTGrammar_reset (me, 100.0);
		for (itrial = 1; itrial <= 40; itrial ++) {
			bool grammarHasChangedDuringCycle = false;
			OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			for (iform = 1; iform <= thy pairs.size; iform ++) {
				PairProbability prob = thy pairs.at [iform];
				if (prob -> weight > 0.0) {
					bool grammarHasChanged = false;
					OTGrammar_learnOne (me, prob -> string1, prob -> string2,
						evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, true /* honour fixed rankings; very important */,
						1.0, 0.0, false, true, & grammarHasChanged);
					if (grammarHasChanged) {
						OTGrammar_newDisharmonies (me, evaluationNoise);
					}
					grammarHasChangedDuringCycle |= grammarHasChanged;
				}
			}
			if (! grammarHasChangedDuringCycle) break;
		}
		if (itrial > 40) {
			MelderInfo_writeLine (U"There are no total rankings that generate these input-output pairs.");
			throw MelderError ();
		}
		/*
		 * Test learnability of every possible ranked pair.
		 */
		my numberOfFixedRankings ++;
		autoNUMmatrix <bool> obligatory (1, my numberOfConstraints, 1, my numberOfConstraints);
		MelderInfo_open ();
		autoMelderProgress progress (U"Finding obligatory rankings.");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair, U": Trying ranking ",
					my constraints [icons]. name, U" >> ", my constraints [jcons]. name);
				ipair ++;
				for (itrial = 1; itrial <= 40; itrial ++) {
					bool grammarHasChangedDuringCycle = false;
					OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
					OTGrammar_newDisharmonies (me, evaluationNoise);
					for (iform = 1; iform <= thy pairs.size; iform ++) {
						PairProbability prob = thy pairs.at [iform];
						if (prob -> weight > 0.0) {
							bool grammarHasChanged = false;
							OTGrammar_learnOne (me, prob -> string1, prob -> string2,
								evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, true /* honour fixed rankings; very important */,
								1.0, 0.0, false, true, & grammarHasChanged);
							if (grammarHasChanged) {
								OTGrammar_newDisharmonies (me, evaluationNoise);
							}
							grammarHasChangedDuringCycle |= grammarHasChanged;
						}
					}
					if (! grammarHasChangedDuringCycle) break;
				}
				if (itrial > 40) {
					obligatory [jcons] [icons] = true;
					MelderInfo_writeLine (my constraints [jcons]. name, U" >> ", my constraints [icons]. name);
					MelderInfo_close ();
				}
			}
		}
		my numberOfFixedRankings ++;
		Melder_progress (0.0, U"");
		npair = npair * npair;
		OrderedOf<structOTGrammar_List4> list;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons && ! obligatory [jcons] [icons]) {
				my fixedRankings [my numberOfFixedRankings - 1]. higher = icons;
				my fixedRankings [my numberOfFixedRankings - 1]. lower = jcons;
				for (kcons = icons; kcons <= my numberOfConstraints; kcons ++) {
					for (lcons = 1; lcons <= my numberOfConstraints; lcons ++) if (kcons != lcons && ! obligatory [lcons] [kcons]) {
						if (icons == kcons && jcons >= lcons) continue;
						if (icons == lcons && jcons == kcons) continue;
						if (jcons == kcons && obligatory [lcons] [icons]) continue;
						if (icons == lcons && obligatory [jcons] [kcons]) continue;
						if (obligatory [lcons] [icons] && obligatory [jcons] [kcons]) continue;
						my fixedRankings [my numberOfFixedRankings]. higher = kcons;
						my fixedRankings [my numberOfFixedRankings]. lower = lcons;
						OTGrammar_reset (me, 100.0);
						Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair);
						ipair ++;
						for (itrial = 1; itrial <= 40; itrial ++) {
							bool grammarHasChangedDuringCycle = false;
							OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
							OTGrammar_newDisharmonies (me, evaluationNoise);
							for (iform = 1; iform <= thy pairs.size; iform ++) {
								PairProbability prob = thy pairs.at [iform];
								if (prob -> weight > 0.0) {
									bool grammarHasChanged = false;
									OTGrammar_learnOne (me, prob -> string1, prob -> string2,
										evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, true /* honour fixed rankings; very important */,
										1.0, 0.0, false, true, & grammarHasChanged);
									if (grammarHasChanged) {
										OTGrammar_newDisharmonies (me, evaluationNoise);
									}
									grammarHasChangedDuringCycle |= grammarHasChanged;
								}
							}
							if (! grammarHasChangedDuringCycle) break;
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
		 * Improve list.
		 */
		improved = true;
		while (improved) {
			improved = false;
			for (ilist = 1; ilist <= list.size; ilist ++) {
				for (jlist = 1; jlist <= list.size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = list.at [ilist];
					OTGrammar_List4 elB = list.at [jlist];
					long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
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
			for (ilist = 1; ilist <= list.size; ilist ++) {
				for (jlist = 1; jlist <= list.size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = list.at [ilist];
					OTGrammar_List4 elB = list.at [jlist];
					long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
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
		for (ilist = 1; ilist <= list.size; ilist ++) {
			OTGrammar_List4 el = list.at [ilist];
			MelderInfo_write (my constraints [el -> hi1]. name, U" >> ", my constraints [el -> lo1]. name, U" OR ");
			MelderInfo_writeLine (my constraints [el -> hi2]. name, U" >> ", my constraints [el -> lo2]. name);
			MelderInfo_close ();
		}
		MelderInfo_close ();
		/*
		 * Remove room.
		 */
		NUMvector_free <structOTGrammarFixedRanking> (my fixedRankings, 1);   // dangle
		/*
		 * Restore.
		 */
		my numberOfFixedRankings = savedNumberOfFixedRankings;
		my fixedRankings = savedFixedRankings;   // undangle
		OTGrammar_restore (me);
	} catch (MelderError) {
		MelderInfo_close ();
		/*
		 * Remove room.
		 */
		NUMvector_free <structOTGrammarFixedRanking> (my fixedRankings, 1);   // dangle
		/*
		 * Restore.
		 */
		my numberOfFixedRankings = savedNumberOfFixedRankings;
		my fixedRankings = savedFixedRankings;   // undangle
		OTGrammar_restore (me);
		Melder_throw (me, U": obligatory rankings not listed.");
	}
}

void OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, long columnNumber) {
	/*
	 * Save.
	 */
	OTGrammarFixedRanking savedFixedRankings = my fixedRankings;
	my fixedRankings = nullptr;
	OTGrammar_save (me);
	try {
		long ifixedRanking, icons, jcons, kcons, ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
		/*
		 * Add room for one more fixed ranking.
		 */
		my numberOfFixedRankings ++;
		my fixedRankings = NUMvector <structOTGrammarFixedRanking> (1, my numberOfFixedRankings);
		for (ifixedRanking = 1; ifixedRanking < my numberOfFixedRankings; ifixedRanking ++) {
			my fixedRankings [ifixedRanking]. higher = savedFixedRankings [ifixedRanking]. higher;
			my fixedRankings [ifixedRanking]. lower = savedFixedRankings [ifixedRanking]. lower;
		}
		/*
		 * Test learnability of every possible ranked pair.
		 */
		MelderInfo_open ();
		autoMelderProgress progress (U"Finding obligatory rankings.");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, ipair + 1, U"/", npair, U": Trying ranking ",
					my constraints [icons]. name, U" >> ", my constraints [jcons]. name);
				ipair ++;
				Melder_progressOff ();
				OTGrammar_Distributions_learnFromPartialOutputs (me, thee, columnNumber,
					1e-9, kOTGrammar_rerankingStrategy_EDCD, true /* honour fixed rankings; very important */,
					1.0, 1000, 0.0, 1, 0.0, 1, 0, nullptr, false, false, 0);
				Melder_progressOn ();
				for (kcons = 1; kcons <= my numberOfConstraints; kcons ++) {
					if (my constraints [kcons]. ranking < 0.0) {
						MelderInfo_writeLine (my constraints [jcons]. name, U" >> ", my constraints [icons]. name);
						break;
					}
				}
			}
		}
		MelderInfo_close ();
		/*
		 * Remove room.
		 */
		NUMvector_free <structOTGrammarFixedRanking> (my fixedRankings, 1);   // dangle
		/*
		 * Restore.
		 */
		my numberOfFixedRankings --;
		my fixedRankings = savedFixedRankings;   // undangle
		OTGrammar_restore (me);
	} catch (MelderError) {
		MelderInfo_close ();
		/*
		 * Remove room.
		 */
		NUMvector_free <structOTGrammarFixedRanking> (my fixedRankings, 1);   // dangle
		/*
		 * Restore.
		 */
		my numberOfFixedRankings --;
		my fixedRankings = savedFixedRankings;   // undangle
		OTGrammar_restore (me);
		Melder_throw (me, U": obligatory rankings not listed.");
	}
}

static void printConstraintNames (OTGrammar me, MelderString *buffer) {
	char32 text [200];
	bool secondLine = false;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		if (str32chr (constraint -> name, U'\n')) {
			char32 *newLine;
			str32cpy (text, constraint -> name);
			newLine = str32chr (text, U'\n');
			*newLine = '\0';
			MelderString_append (buffer, U"\t", text);
			secondLine = true;
		} else {
			MelderString_append (buffer, U"\t", constraint -> name);
		}
	}
	MelderString_appendCharacter (buffer, U'\n');
	if (secondLine) {
		MelderString_appendCharacter (buffer, U'\t');
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			char32 *newLine = str32chr (constraint -> name, U'\n');
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
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, U"\t", constraint -> ranking);
		}
		MelderString_append (& buffer, U"\ndisharmonies\t");
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, U"\t", constraint -> disharmony);
		}
		MelderString_appendCharacter (& buffer, U'\n');
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			long winner = OTGrammar_getWinner (me, itab), numberOfOptimalCandidates = 0;
			for (long icons = 1; icons <= my numberOfConstraints + 1; icons ++) {
				MelderString_appendCharacter (& buffer, U'\t');
			}
			MelderString_append (& buffer, U"\nINPUT\t", tableau -> input);
			printConstraintNames (me, & buffer);
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				if (OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0) {
					numberOfOptimalCandidates ++;
				}
			}
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate candidate = & tableau -> candidates [icand];
				bool candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
				long crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
				MelderString_append (& buffer,
					candidateIsOptimal == false ? U"loser" : numberOfOptimalCandidates > 1 ? U"co-winner" : U"winner",
					U"\t",
					candidate -> output);
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					int index = my index [icons];
					OTGrammarConstraint constraint = & my constraints [index];
					static MelderString markString;
					MelderString_empty (& markString);
					/*
					 * An exclamation mark can be drawn in this cell only if all of the following conditions are met:
					 * 1. the candidate is not optimal;
					 * 2. the constraint is not tied;
					 * 3. this is the crucial cell, i.e. the cells after it are drawn in grey.
					 */
					if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
						int winnerMarks = tableau -> candidates [winner]. marks [index];
						for (long imark = 1; imark <= winnerMarks + 1; imark ++)
							MelderString_appendCharacter (& markString, U'*');
						MelderString_appendCharacter (& markString, U'!');
						for (long imark = winnerMarks + 2; imark <= candidate -> marks [index]; imark ++)
							MelderString_appendCharacter (& markString, U'*');
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							MelderString_appendCharacter (& markString, U'=');
						}
						for (long imark = 1; imark <= candidate -> marks [index]; imark ++)
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
