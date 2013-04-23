/* OTGrammar.cpp
 *
 * Copyright (C) 1997-2012 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
 * pb 2007/08/08 wchar_t
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
 */

#include "OTGrammar.h"
#include "NUM.h"

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
	structData :: v_info ();
	long numberOfCandidates = 0, numberOfViolations = 0;
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		numberOfCandidates += tableaus [itab]. numberOfCandidates;
		for (long icand = 1; icand <= tableaus [itab]. numberOfCandidates; icand ++)
			for (long icons = 1; icons <= numberOfConstraints; icons ++)
				numberOfViolations += tableaus [itab]. candidates [icand]. marks [icons];
	}
	MelderInfo_writeLine (L"Decision strategy: ", kOTGrammar_decisionStrategy_getText (decisionStrategy));
	MelderInfo_writeLine (L"Number of constraints: ", Melder_integer (numberOfConstraints));
	MelderInfo_writeLine (L"Number of tableaus: ", Melder_integer (numberOfTableaus));
	MelderInfo_writeLine (L"Number of candidates: ", Melder_integer (numberOfCandidates));
	MelderInfo_writeLine (L"Number of violation marks: ", Melder_integer (numberOfViolations));
}

void structOTGrammar :: v_writeText (MelderFile file) {
	MelderFile_write (file, L"\n<", kOTGrammar_decisionStrategy_getText (decisionStrategy),
		L">\n", Melder_double (leak), L" ! leak\n", Melder_integer (numberOfConstraints), L" constraints");
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & constraints [icons];
		MelderFile_write (file, L"\nconstraint [", Melder_integer (icons), L"]: \"");
		for (const wchar_t *p = & constraint -> name [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, L"\" ", Melder_double (constraint -> ranking),
			L" ", Melder_double (constraint -> disharmony), L" ", Melder_double (constraint -> plasticity), L" ! ");
		for (const wchar_t *p = & constraint -> name [0]; *p; p ++) {
			if (*p == '\n') MelderFile_writeCharacter (file, ' ');
			else if (*p == '\\' && p [1] == 's' && p [2] == '{') p += 2;
			else if (*p == '}') { }
			else MelderFile_writeCharacter (file, *p);
		}
	}
	MelderFile_write (file, L"\n\n", Melder_integer (numberOfFixedRankings), L" fixed rankings");
	for (long irank = 1; irank <= numberOfFixedRankings; irank ++) {
		OTGrammarFixedRanking fixedRanking = & fixedRankings [irank];
		MelderFile_write (file, L"\n   ", Melder_integer (fixedRanking -> higher), L" ", Melder_integer (fixedRanking -> lower));
	}
	MelderFile_write (file, L"\n\n", Melder_integer (numberOfTableaus), L" tableaus");
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & tableaus [itab];
		MelderFile_write (file, L"\ninput [", Melder_integer (itab), L"]: \"");
		for (const wchar_t *p = & tableau -> input [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, L"\" ", Melder_integer (tableau -> numberOfCandidates));
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			MelderFile_write (file, L"\n   candidate [", Melder_integer (icand), L"]: \"");
			for (const wchar_t *p = & candidate -> output [0]; *p; p ++) {
				if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
				MelderFile_writeCharacter (file, *p);
			}
			MelderFile_writeCharacter (file, '\"');
			for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				MelderFile_write (file, L" ", Melder_integer (candidate -> marks [icons]));
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

void structOTGrammar :: v_readText (MelderReadText text) {
	int localVersion = Thing_version;
	OTGrammar_Parent :: v_readText (text);
	if (localVersion >= 1) {
		try {
			decisionStrategy = texgete1 (text, kOTGrammar_decisionStrategy_getValue);
		} catch (MelderError) {
			Melder_throw ("Trying to read decision strategy.");
		}
	}
	if (localVersion >= 2) {
		try {
			leak = texgetr8 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read leak.");
		}
	}
	try {
		numberOfConstraints = texgeti4 (text);
	} catch (MelderError) {
		Melder_throw ("Trying to read number of constraints.");
	}
	if (numberOfConstraints < 1) Melder_throw ("No constraints.");
	constraints = NUMvector <structOTGrammarConstraint> (1, numberOfConstraints);
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & constraints [icons];
		try {
			constraint -> name = texgetw2 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read name of constraint ", icons, ".");
		}
		try {
			constraint -> ranking = texgetr8 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read ranking of constraint ", icons, ".");
		}
		try {
			constraint -> disharmony = texgetr8 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read disharmony of constraint ", icons, ".");
		}
		if (localVersion < 2) {
			constraint -> plasticity = 1.0;
		} else {
			try {
				constraint -> plasticity = texgetr8 (text);
			} catch (MelderError) {
				Melder_throw ("Trying to read plasticity of constraint ", icons, ".");
			}
		}
	}
	try {
		numberOfFixedRankings = texgeti4 (text);
	} catch (MelderError) {
		Melder_throw ("Trying to read number of fixed rankings.");
	}
	if (numberOfFixedRankings >= 1) {
		fixedRankings = NUMvector <structOTGrammarFixedRanking> (1, numberOfFixedRankings);
		for (long irank = 1; irank <= numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & fixedRankings [irank];
			try {
				fixedRanking -> higher = texgeti4 (text);
			} catch (MelderError) {
				Melder_throw ("Trying to read the higher of constraint pair ", irank, ".");
			}
			try {
				fixedRanking -> lower = texgeti4 (text);
			} catch (MelderError) {
				Melder_throw ("Trying to read the lower of constraint pair ", irank, ".");
			}
		}
	}
	try {
		numberOfTableaus = texgeti4 (text);
	} catch (MelderError) {
		Melder_throw ("Trying to read number of tableaus.");
	}
	if (numberOfTableaus < 1) Melder_throw (L"No tableaus.");
	tableaus = NUMvector <structOTGrammarTableau> (1, numberOfTableaus);
	for (long itab = 1; itab <= numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & tableaus [itab];
		try {
			tableau -> input = texgetw2 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read input of tableau ", itab, ".");
		}
		try {
			tableau -> numberOfCandidates = texgeti4 (text);
		} catch (MelderError) {
			Melder_throw ("Trying to read number of candidates of tableau ", itab, ".");
		}
		if (tableau -> numberOfCandidates < 1) Melder_throw
			("No candidates in tableau ", itab,
			 " (input: ", tableau -> input, ")"
			 " in line ", MelderReadText_getLineNumber (text),
			 itab == 1 ? L"." : ", or perhaps wrong number of candidates for input " L_LEFT_GUILLEMET,
			 itab == 1 ? NULL : tableaus [itab - 1]. input,
			 itab == 1 ? NULL : L_RIGHT_GUILLEMET L".");
		tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates);
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			try {
				candidate -> output = texgetw2 (text);
			} catch (MelderError) {
				Melder_throw ("Trying to read candidate ", icand, " of tableau ", itab,
					" (input: ", tableau -> input, ") in line ", MelderReadText_getLineNumber (text), ".");
			}
			candidate -> numberOfConstraints = numberOfConstraints;   // redundancy, needed for writing binary
			candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints);
			for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				try {
					candidate -> marks [icons] = texgeti2 (text);
				} catch (MelderError) {
					Melder_throw
					("Trying to read number of violations of constraint ", icons,
					 " (", constraints [icons]. name, ")"
					 " of candidate ", icand,
					 " (", candidate -> output, ")"
					 " of tableau ", itab,
					 " (input: ", tableau -> input, ")"
					 " in line ", MelderReadText_getLineNumber (text), L".");
				}
			}
		}
	}
	OTGrammar_checkIndex (this);
}

Thing_implement (OTGrammar, Data, 2);

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
	return wcscmp (my constraints [icons]. name, my constraints [jcons]. name);
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

long OTGrammar_getTableau (OTGrammar me, const wchar_t *input) {
	long n = my numberOfTableaus;
	for (long i = 1; i <= n; i ++)
		if (wcsequ (my tableaus [i]. input, input))
			return i;
	Melder_throw ("Input \"", input, "\" not in list of tableaus.");
}

static void _OTGrammar_fillInHarmonies (OTGrammar me, long itab) {
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
			Melder_fatal ("_OTGrammar_fillInHarmonies: unimplemented decision strategy.");
		}
		candidate -> harmony = - disharmony;
	}
}

int OTGrammar_compareCandidates (OTGrammar me, long itab1, long icand1, long itab2, long icand2) {
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
			if (numberOfMarks1 < numberOfMarks2) return -1;   /* Candidate 1 is better than candidate 2. */
			if (numberOfMarks1 > numberOfMarks2) return +1;   /* Candidate 2 is better than candidate 1. */
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
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my constraints [icons]. disharmony > 0.0) {
				disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
				disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
			}
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
		my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += exp (my constraints [icons]. disharmony) * marks1 [icons];
			disharmony2 += exp (my constraints [icons]. disharmony) * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			double constraintDisharmony = my constraints [icons]. disharmony > 1.0 ? my constraints [icons]. disharmony : 1.0;
			disharmony1 += constraintDisharmony * marks1 [icons];
			disharmony2 += constraintDisharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else Melder_fatal ("Unimplemented decision strategy.");
	return 0;   /* The two total disharmonies are equal. */
}

static void _OTGrammar_fillInProbabilities (OTGrammar me, long itab) {
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
	Melder_assert (sumOfProbabilities > 0.0);   // Because at least one of them is 1.0.
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		candidate -> probability /= sumOfProbabilities;
	}
}

long OTGrammar_getWinner (OTGrammar me, long itab) {
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
			icand_best = icand;   /* The current candidate is the best candidate found so far. */
			numberOfBestCandidates = 1;
		} else if (comparison == 0) {
			numberOfBestCandidates += 1;   /* The current candidate is equally good as the best found before. */
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

void OTGrammar_getInterpretiveParse (OTGrammar me, const wchar_t *partialOutput, long *bestTableau, long *bestCandidate) {
	try {
		long itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				if (wcsstr (cand -> output, partialOutput)) {   /* T&S idea of surface->overt mapping */
					if (itab_best == 0) {
						itab_best = itab;   /* The first compatible input/output pair found is the first guess for the best candidate. */
						icand_best = icand;
						numberOfBestCandidates = 1;
					} else {
						int comparison = OTGrammar_compareCandidates (me, itab, icand, itab_best, icand_best);
						if (comparison == -1) {
							itab_best = itab;   /* The current input/output pair is the best candidate found so far. */
							icand_best = icand;
							numberOfBestCandidates = 1;
						} else if (comparison == 0) {
							numberOfBestCandidates += 1;   /* The current input/output pair is equally good as the best found before. */
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
		if (itab_best == 0) Melder_throw ("The partial output \"", partialOutput, "\" does not match any candidate for any input form.");
		if (bestTableau != NULL) *bestTableau = itab_best;
		if (bestCandidate != NULL) *bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw ("Interpretive parse not computed.");
	}
}

static void OTGrammar_getInterpretiveParse_opt (OTGrammar me, long ipartialOutput, long *bestTableau, long *bestCandidate) {
	try {
		long itab_best = 0, icand_best = 0, numberOfBestCandidates = 0;
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tableau -> candidates [icand];
				Melder_assert (cand -> partialOutputMatches != NULL);
				if (cand -> partialOutputMatches [ipartialOutput]) {   /* T&S idea of surface->overt mapping */
					if (itab_best == 0) {
						itab_best = itab;   /* The first compatible input/output pair found is the first guess for the best candidate. */
						icand_best = icand;
						numberOfBestCandidates = 1;
					} else {
						int comparison = OTGrammar_compareCandidates (me, itab, icand, itab_best, icand_best);
						if (comparison == -1) {
							itab_best = itab;   /* The current input/output pair is the best candidate found so far. */
							icand_best = icand;
							numberOfBestCandidates = 1;
						} else if (comparison == 0) {
							numberOfBestCandidates += 1;   /* The current input/output pair is equally good as the best found before. */
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
		if (bestTableau != NULL) *bestTableau = itab_best;
		if (bestCandidate != NULL) *bestCandidate = icand_best;
	} catch (MelderError) {
		Melder_throw ("Interpretive parse not computed.");
	}
}

bool OTGrammar_isPartialOutputGrammatical (OTGrammar me, const wchar_t *partialOutput) {
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (wcsstr (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, const wchar_t *partialOutput) {
	bool found = false;
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (wcsstr (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					found = true;
					/*
					 * All other grammatical candidates should match.
					 */
					for (long jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
						if (OTGrammar_compareCandidates (me, itab, jcand, itab, icand) == 0) {
							if (wcsstr (tableau -> candidates [jcand]. output, partialOutput) == NULL) {
								return false;   // Partial output is multiply optimal.
							}
						}
					}
				}
			}
		}
	}
	return found;
}

static int OTGrammar_crucialCell (OTGrammar me, long itab, long icand, long iwinner, long numberOfOptimalCandidates) {
	int icons;
	OTGrammarTableau tableau = & my tableaus [itab];
	if (tableau -> numberOfCandidates < 2) return 0;   /* If there is only one candidate, all cells can be greyed. */
	if (my decisionStrategy != kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) return my numberOfConstraints;   /* Nothing grey. */
	if (OTGrammar_compareCandidates (me, itab, icand, itab, iwinner) == 0) {   /* Candidate equally good as winner? */
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			long jcand, secondBest = 0;
			for (jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
				if (OTGrammar_compareCandidates (me, itab, jcand, itab, iwinner) != 0) {   /* A non-optimal candidate? */
					if (secondBest == 0) {
						secondBest = jcand;   /* First guess. */
					} else if (OTGrammar_compareCandidates (me, itab, jcand, itab, secondBest) < 0) {
						secondBest = jcand;   /* Better guess. */
					}
				}
			}
			if (secondBest == 0) return 0;   /* If all candidates are equally good, all cells can be greyed. */
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
	return my numberOfConstraints;   /* Nothing grey. */
}

static double OTGrammar_constraintWidth (Graphics g, const wchar_t *name) {
	wchar_t text [100], *newLine;
	wcscpy (text, name);
	newLine = wcschr (text, '\n');
	if (newLine) {
		double firstWidth, secondWidth;
		*newLine = '\0';
		firstWidth = Graphics_textWidth (g, text);
		secondWidth = Graphics_textWidth (g, newLine + 1);
		return firstWidth > secondWidth ? firstWidth : secondWidth;
	}
	return Graphics_textWidth (g, text);
}

void OTGrammar_drawTableau (OTGrammar me, Graphics g, bool vertical, const wchar_t *input) {
	try {
		double x, y, fontSize = Graphics_inqFontSize (g);
		Graphics_Colour colour = Graphics_inqColour (g);
		wchar_t text [200];
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
				if (wcschr (constraint -> name, '\n')) {
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
			if (wcschr (constraint -> name, '\n') && ! vertical) {
				wchar_t *newLine;
				wcscpy (text, constraint -> name);
				newLine = wcschr (text, '\n');
				*newLine = '\0';
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
				Graphics_text (g, x + 0.5 * width, y + headerHeight, text);
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
				Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 1.0) * fontSize / 12.0, L"\\pf");
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
				wchar_t markString [40];
				markString [0] = '\0';
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
							wcscat (markString, L"*");
						for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							wcscat (markString, L"+");
						wcscat (markString, L"!");
						for (long imark = winnerMarks + 2; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							wcscat (markString, L"*");
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							Graphics_setColour (g, Graphics_RED);
						}
						for (long imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
							wcscat (markString, L"*");
						for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
							wcscat (markString, L"+");
					}
				} else {
					for (long imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
						wcscat (markString, L"*");
					for (long imark = tableau -> candidates [icand]. marks [index]; imark < 0; imark ++)
						wcscat (markString, L"+");
				}
				Graphics_text (g, x + 0.5 * width, y + descent, markString);
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
					//value = value > -1e-300 ? 1000 : value < -1e300 ? -1000 : - log (- value);
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
		Melder_throw (me, ": tableau not drawn.");
	}
}

Strings OTGrammar_generateInputs (OTGrammar me, long numberOfTrials) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <wchar_t*> (1, thy numberOfStrings = numberOfTrials);
		for (long i = 1; i <= numberOfTrials; i ++) {
			long itab = NUMrandomInteger (1, my numberOfTableaus);
			thy strings [i] = Melder_wcsdup (my tableaus [itab]. input);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": inputs not generated.");
	}
}

Strings OTGrammar_getInputs (OTGrammar me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector <wchar_t *> (1, thy numberOfStrings = my numberOfTableaus);
		for (long i = 1; i <= my numberOfTableaus; i ++) {
			thy strings [i] = Melder_wcsdup (my tableaus [i]. input);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": inputs not gotten.");
	}
}

void OTGrammar_inputToOutput (OTGrammar me, const wchar_t *input, wchar_t *output, double evaluationNoise) {
	try {
		OTGrammar_newDisharmonies (me, evaluationNoise);
		long itab = OTGrammar_getTableau (me, input);
		long winner = OTGrammar_getWinner (me, itab);
		if (winner == 0)
			Melder_throw ("No winner");
		wcscpy (output, my tableaus [itab]. candidates [winner]. output);
	} catch (MelderError) {
		Melder_throw (me, ": output not computed from input \"", input, "\".");
	}
}

Strings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double evaluationNoise) {
	try {
		autoStrings him = Thing_new (Strings);
		long n = inputs -> numberOfStrings;
		his numberOfStrings = n;
		his strings = NUMvector <wchar_t*> (1, n);
		for (long i = 1; i <= n; i ++) {
			wchar_t output [100];
			OTGrammar_inputToOutput (me, inputs -> strings [i], output, evaluationNoise);
			his strings [i] = Melder_wcsdup (output);
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": outputs not computed.");
	}
}

Strings OTGrammar_inputToOutputs (OTGrammar me, const wchar_t *input, long n, double evaluationNoise) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = n;
		thy strings = NUMvector <wchar_t*> (1, n);
		for (long i = 1; i <= n; i ++) {
			wchar_t output [100];
			OTGrammar_inputToOutput (me, input, output, evaluationNoise);
			thy strings [i] = Melder_wcsdup (output);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": output not computed.");
	}
}

Distributions OTGrammar_to_Distribution (OTGrammar me, long trialsPerInput, double noise) {
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
		autoMelderProgress progress (L"OTGrammar: compute output distribution.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, L"Measuring input \"", tableau -> input, L"\"");
			/*
			 * Set the row labels to the output strings.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				static MelderString buffer = { 0 };
				MelderString_empty (& buffer);
				MelderString_append (& buffer, tableau -> input, L" \\-> ", tableau -> candidates [icand]. output);
				thy rowLabels [nout + icand] = Melder_wcsdup (buffer.string);
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": output distribution not computed.");
	}
}

PairDistribution OTGrammar_to_PairDistribution (OTGrammar me, long trialsPerInput, double noise) {
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
		autoMelderProgress progress (L"OTGrammar: compute output distribution.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, L"Measuring input \"", tableau -> input, L"\"");
			/*
			 * Copy the input and output strings to the target object.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				PairDistribution_add (thee.peek(), tableau -> input, tableau -> candidates [icand]. output, 0);
			}
			/*
			 * Compute a number of outputs and store the results.
			 */
			PairProbability *p = (PairProbability *) thy pairs -> item;   // may have changed after PairDistribution_add !!!
			for (long itrial = 1; itrial <= trialsPerInput; itrial ++) {
				OTGrammar_newDisharmonies (me, noise);
				long iwinner = OTGrammar_getWinner (me, itab);
				p [nout + iwinner] -> weight += 1;
			}
			/*
			 * Update the offset.
			 */
			nout += tableau -> numberOfCandidates;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": output distribution not computed.");
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

Distributions OTGrammar_measureTypology (OTGrammar me) {
	try {
		long totalNumberOfOutputs = 0, nout = 0, ncons = my numberOfConstraints, nperm, factorial [1+12];
		if (ncons > 12)
			Melder_throw ("Cannot handle more than 12 constraints.");
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
		autoMelderProgress progress (L"Measuring typology...");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			Melder_progress ((itab - 0.5) / my numberOfTableaus, L"Measuring input \"", tableau -> input, L"\"");
			/*
			 * Set the row labels to the output strings.
			 */
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				static MelderString buffer = { 0 };
				MelderString_empty (& buffer);
				MelderString_append (& buffer, tableau -> input, L" \\-> ", tableau -> candidates [icand]. output);
				thy rowLabels [nout + icand] = Melder_wcsdup_f (buffer.string);
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": typology not measured.");
	}
}

static double learningStep (double mean, double relativeSpreading) {
	return relativeSpreading == 0.0 ? mean : NUMrandomGauss (mean, relativeSpreading * mean);
}

static void OTGrammar_honourLocalRankings (OTGrammar me, double plasticity, double relativePlasticityNoise, int *grammarHasChanged) {
	int improved;
	do {
		improved = FALSE;
		for (long irank = 1; irank <= my numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
			OTGrammarConstraint higher = & my constraints [fixedRanking -> higher], lower = & my constraints [fixedRanking -> lower];
			while (higher -> ranking <= lower -> ranking) {
				lower -> ranking -= learningStep (plasticity, relativePlasticityNoise);
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
				improved = TRUE;
			}
		}
	} while (improved);
}

static void OTGrammar_modifyRankings (OTGrammar me, long itab, long iwinner, long iloser,
	int updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, int warnIfStalled, int *grammarHasChanged)
{
	try {
		OTGrammarTableau tableau = & my tableaus [itab];
		OTGrammarCandidate winner = & tableau -> candidates [iwinner], loser = & tableau -> candidates [iloser];
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
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
			if (winnerMarks > loserMarks) {
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_SYMMETRIC_ALL) {
			bool changed = false;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
					changed = true;
				}
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
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
			if (grammarHasChanged != NULL) *grammarHasChanged = changed;
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_UNCANCELLED) {
			int winningConstraints = 0, losingConstraints = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) losingConstraints ++;
				if (winnerMarks > loserMarks) winningConstraints ++;
			}
			if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
					//constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) * winningConstraints;
					if (grammarHasChanged != NULL) *grammarHasChanged = true;
				}
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					//constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * losingConstraints;
					if (grammarHasChanged != NULL) *grammarHasChanged = true;
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL) {
			int winningConstraints = 0, losingConstraints = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > 0) losingConstraints ++;
				if (winnerMarks > 0) winningConstraints ++;
			}
			if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTGrammarConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > 0) {
					if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks /*- winnerMarks*/;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
					if (grammarHasChanged != NULL) *grammarHasChanged = true;
				}
				if (winnerMarks > 0) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks /*- loserMarks*/;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					if (grammarHasChanged != NULL) *grammarHasChanged = true;
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
				int loserMarks = loser -> marks [my index [icons]];
				if (loserMarks < winnerMarks) break;
				if (loserMarks > winnerMarks) equivalent = false;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (warnIfStalled && ! equivalent)
					Melder_warning ("Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
						"Input: ", tableau -> input, "\nCorrect output: ", loser -> output, "\nLearner's output: ", winner -> output);
				return;
			}
			/*
			 * Determine the stratum into which some constraints will be demoted.
			 */
			pivotRanking = my constraints [my index [icons]]. ranking;
			if (updateRule == kOTGrammar_rerankingStrategy_EDCD_WITH_VACATION) {
				long numberOfConstraintsToDemote = 0;
				for (icons = 1; icons <= my numberOfConstraints; icons ++) {
					int winnerMarks = winner -> marks [icons];
					int loserMarks = loser -> marks [icons];
					if (loserMarks > winnerMarks) {
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
							if (grammarHasChanged != NULL) *grammarHasChanged = true;
						}
					}
				}
			}
			/*
			 * Demote all the uniquely violated constraints in the loser
			 * that have rankings not lower than the pivot.
			 */
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				long numberOfConstraintsDemoted = 0;
				int winnerMarks = winner -> marks [my index [icons]];   // for the vacation version, the order is important, therefore indirect
				int loserMarks = loser -> marks [my index [icons]];
				if (loserMarks > winnerMarks) {
					OTGrammarConstraint constraint = & my constraints [my index [icons]];
					double constraintStep = step * constraint -> plasticity;
					if (constraint -> ranking >= pivotRanking) {
						numberOfConstraintsDemoted += 1;
						constraint -> ranking = pivotRanking - numberOfConstraintsDemoted * constraintStep;   // this preserves the order of the demotees
						if (grammarHasChanged != NULL) *grammarHasChanged = true;
					}
				}
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_DEMOTION_ONLY) {
			/*
			 * Determine the crucial loser mark.
			 */
			long crucialLoserMark;
			OTGrammarConstraint offendingConstraint;
			long icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
				int loserMarks = loser -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw ("Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks)
					Melder_throw ("Demotion-only learning step: Loser wins! Should never happen.");
				if (loserMarks > winnerMarks) break;
			}
			if (icons > my numberOfConstraints)   // completed the loop?
				Melder_throw ("Loser equals correct candidate.");
			crucialLoserMark = icons;
			/*
			 * Demote the highest uniquely violated constraint in the loser.
			 */
			offendingConstraint = & my constraints [my index [crucialLoserMark]];
			double constraintStep = step * offendingConstraint -> plasticity;
			offendingConstraint -> ranking -= constraintStep;
			if (grammarHasChanged != NULL) *grammarHasChanged = true;
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN) {
			long numberOfUp = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					numberOfUp ++;
				}
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [icons];
					int loserMarks = loser -> marks [icons];
					if (winnerMarks > loserMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / numberOfUp;
						if (grammarHasChanged != NULL) *grammarHasChanged = true;
					}
				}
				long crucialLoserMark, winnerMarks = 0, loserMarks = 0;
				OTGrammarConstraint offendingConstraint;
				long icons = 1;
				for (; icons <= my numberOfConstraints; icons ++) {
					winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
					loserMarks = loser -> marks [my index [icons]];
					if (my constraints [my index [icons]]. tiedToTheRight)
						Melder_throw ("Demotion-only learning cannot handle tied constraints.");
					if (loserMarks < winnerMarks)
						Melder_throw ("Demotion-only learning step: Loser wins! Should never happen.");
					if (loserMarks > winnerMarks) break;
				}
				if (icons > my numberOfConstraints)   // completed the loop?
					Melder_throw ("Loser equals correct candidate.");
				crucialLoserMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the loser.
				 */
				offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN_2012) {
			long numberOfUp = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					numberOfUp ++;
				}
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					OTGrammarConstraint constraint = & my constraints [icons];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [icons];
					int loserMarks = loser -> marks [icons];
					if (winnerMarks > loserMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / (numberOfUp + 1);
						if (grammarHasChanged != NULL) *grammarHasChanged = true;
					}
				}
				long crucialLoserMark, winnerMarks = 0, loserMarks = 0;
				OTGrammarConstraint offendingConstraint;
				long icons = 1;
				for (; icons <= my numberOfConstraints; icons ++) {
					winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
					loserMarks = loser -> marks [my index [icons]];
					if (my constraints [my index [icons]]. tiedToTheRight)
						Melder_throw ("Demotion-only learning cannot handle tied constraints.");
					if (loserMarks < winnerMarks)
						Melder_throw ("Demotion-only learning step: Loser wins! Should never happen.");
					if (loserMarks > winnerMarks) break;
				}
				if (icons > my numberOfConstraints)   // completed the loop?
					Melder_throw ("Loser equals correct candidate.");
				crucialLoserMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the loser.
				 */
				offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGH_DOWN) {
			long numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				int loserMarks = loser -> marks [my index [icons]];
				if (loserMarks < winnerMarks) {
					numberOfUp ++;
				} else if (loserMarks > winnerMarks) {
					if (numberOfUp == 0) {
						numberOfDown ++;
						lowestDemotableConstraint = icons;
					}
				}
			}
			if (warnIfStalled && numberOfDown == 0) {
				Melder_warning ("Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
					"Input: ", tableau -> input, "\nCorrect output: ", loser -> output, "\nLearner's output: ", winner -> output);
				return;
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					long constraintIndex = my index [icons];
					OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					int loserMarks = loser -> marks [constraintIndex];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw ("Demotion-only learning cannot handle tied constraints.");
					if (loserMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 0.0);
					} else if (loserMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
		} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGH_DOWN_2012) {
			long numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
				int loserMarks = loser -> marks [my index [icons]];
				if (loserMarks < winnerMarks) {
					numberOfUp ++;
				} else if (loserMarks > winnerMarks) {
					if (numberOfUp == 0) {
						numberOfDown ++;
						lowestDemotableConstraint = icons;
					}
				}
			}
			if (warnIfStalled && numberOfDown == 0) {
				Melder_warning ("Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
					"Input: ", tableau -> input, "\nCorrect output: ", loser -> output, "\nLearner's output: ", winner -> output);
				return;
			}
			if (numberOfUp > 0) {
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					long constraintIndex = my index [icons];
					OTGrammarConstraint constraint = & my constraints [constraintIndex];
					double constraintStep = step * constraint -> plasticity;
					int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
					int loserMarks = loser -> marks [constraintIndex];
					if (my constraints [constraintIndex]. tiedToTheRight)
						Melder_throw ("Demotion-only learning cannot handle tied constraints.");
					if (loserMarks < winnerMarks) {
						if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
						constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 1.0);
					} else if (loserMarks > winnerMarks) {
						if (icons <= lowestDemotableConstraint) {
							if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
							constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
						}
					}
				}
				if (grammarHasChanged != NULL) *grammarHasChanged = true;
			}
		}
		if (honourLocalRankings && my numberOfFixedRankings) {
			OTGrammar_honourLocalRankings (me, plasticity, relativePlasticityNoise, grammarHasChanged);
		}
	} catch (MelderError) {
		Melder_throw (me, ": rankings not modified.");
	}
}

void OTGrammar_learnOne (OTGrammar me, const wchar_t *underlyingForm, const wchar_t *adultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, int newDisharmonies, int warnIfStalled, int *grammarHasChanged)
{
	try {
		if (newDisharmonies) OTGrammar_newDisharmonies (me, evaluationNoise);
		if (grammarHasChanged != NULL) *grammarHasChanged = FALSE;

		/*
		 * Evaluate the input in the learner's hypothesis.
		 */
		long itab = OTGrammar_getTableau (me, underlyingForm);
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
		if (wcsequ (winner -> output, adultOutput)) return;   // as far as we know, the grammar is already correct: don't update rankings

		/*
		 * Find (perhaps the learner's interpretation of) the adult winner (the "loser") in the learner's own tableau
		 * (Tesar & Smolensky call this the "winner").
		 */
		long iloser = 1;
		for (; iloser <= tableau -> numberOfCandidates; iloser ++) {
			OTGrammarCandidate loser = & tableau -> candidates [iloser];
			if (wcsequ (loser -> output, adultOutput)) break;
		}
		if (iloser > tableau -> numberOfCandidates)
			Melder_throw ("Cannot generate adult output \"", adultOutput, L"\".");

		/*
		 * Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
		 * The grammar will have to change.
		 */
		OTGrammar_modifyRankings (me, itab, iwinner, iloser, updateRule, honourLocalRankings,
			plasticity, relativePlasticityNoise, warnIfStalled, grammarHasChanged);
	} catch (MelderError) {
		Melder_throw (me, ": not learned from input \"", underlyingForm, "\" and adult output \"", adultOutput, "\".");
	}
}

void OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews)
{
	if (! inputs) inputs = outputs;
	try {
		long n = inputs -> numberOfStrings;
		if (outputs -> numberOfStrings != n)
			Melder_throw ("Numbers of strings in input and output are not equal.");
		for (long i = 1; i <= n; i ++) {
			for (long ichew = 1; ichew <= numberOfChews; ichew ++) {
				OTGrammar_learnOne (me, inputs -> strings [i], outputs -> strings [i],
					evaluationNoise, updateRule, honourLocalRankings,
					plasticity, relativePlasticityNoise, TRUE, TRUE, NULL);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not learned from ", inputs, " (inputs) and ", outputs, " (outputs).");
	}
}

void OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews)
{
	long idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		double plasticity = initialPlasticity;
		autoMelderMonitor monitor (L"Learning with full knowledge...");
		if (monitor.graphics()) {
			Graphics_clearWs (monitor.graphics());
		}
		for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
			for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
				wchar_t *input, *output;
				PairDistribution_peekPair (thee, & input, & output);
				++ idatum;
				if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
					Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50, 150);
					for (long icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
						Graphics_setGrey (monitor.graphics(), (double) icons / 14);
						Graphics_line (monitor.graphics(), idatum, my constraints [icons]. ranking,
							idatum, my constraints [icons]. ranking+1);
					}
					Graphics_flushWs (monitor.graphics());   // because drawing is faster than progress loop
				}
				Melder_monitor ((double) idatum / numberOfData,
					L"Processing input-output pair ", Melder_integer (idatum),
					L" out of ", Melder_integer (numberOfData), L": ", input, L" -> ", output);
				for (long ichew = 1; ichew <= numberOfChews; ichew ++) {
					OTGrammar_learnOne (me, input, output,
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, TRUE, TRUE, NULL);
				}
			}
			plasticity *= plasticityDecrement;
		}
	} catch (MelderError) {
		if (idatum > 1)
			Melder_error_ ("Only ", idatum - 1, " input-output pairs out of ", numberOfData, " were processed.");
		Melder_throw (me, ": did not complete learning from ", thee, ".");
	}
}

static long PairDistribution_getNumberOfAttestedOutputs (PairDistribution me, const wchar_t *input, wchar_t **attestedOutput) {
	long result = 0;
	for (long ipair = 1; ipair <= my pairs -> size; ipair ++) {
		PairProbability pair = (PairProbability) my pairs -> item [ipair];
		if (wcsequ (pair -> string1, input) && pair -> weight > 0.0) {
			if (attestedOutput != NULL) *attestedOutput = pair -> string2;
			result ++;
		}
	}
	return result;
}

bool OTGrammar_PairDistribution_findPositiveWeights_e (OTGrammar me, PairDistribution thee, double weightFloor, double marginOfSeparation) {
	NUMlinprog linprog = NULL;
	try {
		bool result = false;
		if (my decisionStrategy != kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR &&
			my decisionStrategy != kOTGrammar_decisionStrategy_LINEAR_OT &&
			my decisionStrategy != kOTGrammar_decisionStrategy_POSITIVE_HG &&
			my decisionStrategy != kOTGrammar_decisionStrategy_EXPONENTIAL_HG)
		{
			Melder_throw ("To find positive weights, the decision strategy has to be HarmonicGrammar, LinearOT, PositiveHG, or ExponentialHG.");
		}
		autoNUMvector <long> optimalCandidates (1, my numberOfTableaus);
		/*
		 * Check that there is exactly one optimal output for each input.
		 */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			wchar_t *attestedOutput = NULL;
			long numberOfAttestedOutputs = PairDistribution_getNumberOfAttestedOutputs (thee, tab -> input, & attestedOutput);
			if (numberOfAttestedOutputs == 0) {
				Melder_throw ("Input \"", my tableaus [itab]. input, "\" has no attested output.");
			} else if (numberOfAttestedOutputs > 1) {
				Melder_throw ("Input \"", my tableaus [itab]. input, "\" has more than one attested output.");
			} else {
				Melder_assert (attestedOutput != NULL);
				for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
					OTGrammarCandidate cand = & tab -> candidates [icand];
					if (wcsequ (attestedOutput, cand -> output)) {
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
			NUMlinprog_addVariable (linprog, weightFloor, NUMundefined, 1.0);
		}
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			long ioptimalCandidate = optimalCandidates [itab];
			Melder_assert (ioptimalCandidate >= 1);
			Melder_assert (ioptimalCandidate <= tab -> numberOfCandidates);
			OTGrammarCandidate optimalCandidate = & tab -> candidates [ioptimalCandidate];
			for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) if (icand != ioptimalCandidate) {
				OTGrammarCandidate cand = & tab -> candidates [icand];
				NUMlinprog_addConstraint (linprog, marginOfSeparation, NUMundefined);
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
		Melder_throw (me, ": positive weights not found.");
	}
}

void OTGrammar_reset (OTGrammar me, double ranking) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTGrammar_sort (me);
}

void OTGrammar_resetToRandomTotalRanking (OTGrammar me, double maximumRanking, double rankingDistance) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> ranking = 0.0;
	}
	OTGrammar_newDisharmonies (me, 1.0);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> disharmony = constraint -> ranking = maximumRanking - (icons - 1) * rankingDistance;
	}
	OTGrammar_sort (me);
}

void OTGrammar_setRanking (OTGrammar me, long constraint, double ranking, double disharmony) {
	try {
		if (constraint < 1 || constraint > my numberOfConstraints)
			Melder_throw ("There is no constraint with number ", constraint, ".");
		my constraints [constraint]. ranking = ranking;
		my constraints [constraint]. disharmony = disharmony;
		OTGrammar_sort (me);
	} catch (MelderError) {
		Melder_throw (me, ": ranking of constraint ", constraint, " not set.");
	}
}

void OTGrammar_setConstraintPlasticity (OTGrammar me, long constraint, double plasticity) {
	try {
		if (constraint < 1 || constraint > my numberOfConstraints)
			Melder_throw ("There is no constraint with number ", constraint, ".");
		my constraints [constraint]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, ": plasticity of constraint ", constraint, " not set.");
	}
}

long theSaveNumberOfConstraints, *theSaveIndex;
double *theSaveRankings, *theSaveDisharmonies;
int *theSaveTiedToTheLeft, *theSaveTiedToTheRight;
static void OTGrammar_save (OTGrammar me) {
	if (my numberOfConstraints != theSaveNumberOfConstraints) {
		NUMvector_free (theSaveIndex, 1); theSaveIndex = NULL;
		NUMvector_free (theSaveRankings, 1); theSaveRankings = NULL;
		NUMvector_free (theSaveDisharmonies, 1); theSaveDisharmonies = NULL;
		NUMvector_free (theSaveTiedToTheLeft, 1); theSaveTiedToTheLeft = NULL;
		NUMvector_free (theSaveTiedToTheRight, 1); theSaveTiedToTheRight = NULL;
		theSaveNumberOfConstraints = my numberOfConstraints;
	}
	if (theSaveIndex == NULL) theSaveIndex = NUMvector <long> (1, my numberOfConstraints);
	if (theSaveRankings == NULL) theSaveRankings = NUMvector <double> (1, my numberOfConstraints);
	if (theSaveDisharmonies == NULL) theSaveDisharmonies = NUMvector <double> (1, my numberOfConstraints);
	if (theSaveTiedToTheLeft == NULL) theSaveTiedToTheLeft = NUMvector <int> (1, my numberOfConstraints);
	if (theSaveTiedToTheRight == NULL) theSaveTiedToTheRight = NUMvector <int> (1, my numberOfConstraints);
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

void OTGrammar_learnOneFromPartialOutput (OTGrammar me, const wchar_t *partialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews, int warnIfStalled)
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
			int grammarHasChanged;
			OTGrammar_learnOne (me,
				my tableaus [assumedAdultInputTableau]. input,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output,
				evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticityNoise, FALSE, warnIfStalled, & grammarHasChanged);
			if (! grammarHasChanged) return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD && ichew > numberOfChews) {
			/*
			 * Is the partial output form grammatical by now?
			 */
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (! wcsequ (learnerCandidate -> output,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
			{   /* Still ungrammatical? */
				/*
				 * Backtrack as in Tesar & Smolensky 2000:69.
				 */
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not learned from partial adult output \"", partialAdultOutput, "\".");
	}
}

static void OTGrammar_learnOneFromPartialOutput_opt (OTGrammar me, long ipartialAdultOutput,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews, int warnIfStalled)
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
			int grammarHasChanged;
			OTGrammar_learnOne (me,
				my tableaus [assumedAdultInputTableau]. input,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output,
				evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticityNoise, FALSE, warnIfStalled, & grammarHasChanged);
			if (! grammarHasChanged) return;
		}
		if (numberOfChews > 1 && updateRule == kOTGrammar_rerankingStrategy_EDCD && ichew > numberOfChews) {
			/*
			 * Is the partial output form grammatical by now?
			 */
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (! wcsequ (learnerCandidate -> output,
				my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
			{   /* Still ungrammatical? */
				/*
				 * Backtrack as in Tesar & Smolensky 2000:69.
				 */
				OTGrammar_restore (me);
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not learned from partial adult output ", ipartialAdultOutput, ".");
	}
}

static OTHistory OTGrammar_createHistory (OTGrammar me, long storeHistoryEvery, long numberOfData) {
	try {
		long numberOfSamplingPoints = numberOfData / storeHistoryEvery, icons;   // e.g. 0, 20, 40, ...
		autoOTHistory thee = Thing_new (OTHistory);
		TableOfReal_init (thee.peek(), 2 + numberOfSamplingPoints * 2, 1 + my numberOfConstraints);
		TableOfReal_setColumnLabel (thee.peek(), 1, L"Datum");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			TableOfReal_setColumnLabel (thee.peek(), icons + 1, my constraints [icons]. name);
		}
		TableOfReal_setRowLabel (thee.peek(), 1, L"Initial state");
		thy data [1] [1] = 0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			thy data [1] [icons + 1] = my constraints [icons]. ranking;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": history not created.");
	}
}

static void OTGrammar_updateHistory (OTGrammar me, OTHistory thee, long storeHistoryEvery, long idatum, const wchar_t *input) {
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
		Melder_throw (me, ": history not updated.");
	}
}

static void OTGrammar_finalizeHistory (OTGrammar me, OTHistory thee, long idatum) {
	try {
		TableOfReal_setRowLabel (thee, thy numberOfRows, L"Final state");
		thy data [thy numberOfRows] [1] = idatum;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			thy data [thy numberOfRows] [icons + 1] = my constraints [icons]. ranking;
		}
	} catch (MelderError) {
		Melder_throw (me, ": history not finalized.");
	}
}

void OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double plasticity, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, OTHistory *history_out)
{
	try {
		autoOTHistory history;
		if (storeHistoryEvery) {
			history.reset (OTGrammar_createHistory (me, storeHistoryEvery, partialOutputs -> numberOfStrings));
		}
		try {
			for (long idatum = 1; idatum <= partialOutputs -> numberOfStrings; idatum ++) {
				try {
					OTGrammar_learnOneFromPartialOutput (me, partialOutputs -> strings [idatum],
						evaluationNoise, updateRule, honourLocalRankings,
						plasticity, relativePlasticityNoise, numberOfChews, FALSE);
				} catch (MelderError) {
					if (history.peek()) {
						OTGrammar_updateHistory (me, history.peek(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum]);   // so that we can inspect
					}
					throw;
				}
				if (history.peek()) {
					OTGrammar_updateHistory (me, history.peek(), storeHistoryEvery, idatum, partialOutputs -> strings [idatum]);
				}
			}
			if (history.peek()) {
				OTGrammar_finalizeHistory (me, history.peek(), partialOutputs -> numberOfStrings);
			}
			*history_out = history.transfer();
		} catch (MelderError) {
			*history_out = history.transfer();   // so that we can inspect
			throw;
		}
	} catch (MelderError) {
		Melder_throw (me, ": not learned from partial outputs ", partialOutputs, ".");
	}
}

static void OTGrammar_opt_deleteOutputMatching (OTGrammar me) {
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tab = & my tableaus [itab];
		for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
			OTGrammarCandidate cand = & tab -> candidates [icand];
			cand -> numberOfPotentialPartialOutputsMatching = 0;
			NUMvector_free (cand -> partialOutputMatches, 1);
			cand -> partialOutputMatches = NULL;
		}
	}
}

static void OTGrammar_Distributions_opt_createOutputMatching (OTGrammar me, Distributions thee, long columnNumber) {
	try {
		if (columnNumber > thy numberOfColumns)
			Melder_throw ("No column ", columnNumber, " in Distributions.");
		if (thy numberOfRows < 1)
			Melder_throw ("No candidates in Distributions.");
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
				OTGrammarCandidate cand = & tab -> candidates [icand];
				cand -> numberOfPotentialPartialOutputsMatching = thy numberOfRows;
				cand -> partialOutputMatches = NUMvector <signed char> (1, thy numberOfRows);
			}
		}
		for (long ipartialOutput = 1; ipartialOutput <= thy numberOfRows; ipartialOutput ++) {
			if (thy data [ipartialOutput] [columnNumber] > 0.0) {
				wchar_t *partialOutput = thy rowLabels [ipartialOutput];
				bool foundPartialOutput = false;
				for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
					OTGrammarTableau tab = & my tableaus [itab];
					for (long icand = 1; icand <= tab -> numberOfCandidates; icand ++) {
						OTGrammarCandidate cand = & tab -> candidates [icand];
						if (wcsstr (cand -> output, partialOutput)) {
							foundPartialOutput = true;
							cand -> partialOutputMatches [ipartialOutput] = true;
						}
					}
				}
				if (! foundPartialOutput)
					Melder_throw ("The partial output \"", partialOutput, "\" does not match any candidate for any input form.");
			}
		}
	} catch (MelderError) {
		OTGrammar_opt_deleteOutputMatching (me);
		throw;
	}
}

void OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, OTHistory *history_out)
{
	long idatum = 0;
	const long numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		autoOTHistory history;
		OTGrammar_Distributions_opt_createOutputMatching (me, thee, columnNumber);
		autoMelderMonitor monitor (L"Learning with limited knowledge...");
		if (monitor.graphics()) {
			Graphics_clearWs (monitor.graphics());
		}
		if (storeHistoryEvery) {
			history.reset (OTGrammar_createHistory (me, storeHistoryEvery, numberOfData));
		}
		try {
			double plasticity = initialPlasticity;
			for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
				for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
					long ipartialOutput;
					Distributions_peek_opt (thee, columnNumber, & ipartialOutput);
					++ idatum;
					if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
						Graphics_setWindow (monitor.graphics(), 0, numberOfData, 50, 150);
						for (long icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
							Graphics_setGrey (monitor.graphics(), (double) icons / 14);
							Graphics_line (monitor.graphics(), idatum, my constraints [icons]. ranking,
								idatum, my constraints [icons]. ranking+1);
						}
						Graphics_flushWs (monitor.graphics());   /* Because drawing is faster than progress loop. */
					}
					Melder_monitor ((double) idatum / numberOfData,
						L"Processing partial output ", Melder_integer (idatum), L" out of ", Melder_integer (numberOfData), L": ",
						thy rowLabels [ipartialOutput]);
					try {
						OTGrammar_learnOneFromPartialOutput_opt (me, ipartialOutput,
							evaluationNoise, updateRule, honourLocalRankings,
							plasticity, relativePlasticityNoise, numberOfChews, FALSE);   // no warning if stalled: RIP form is allowed to be harmonically bounded
					} catch (MelderError) {
						if (history.peek()) {
							OTGrammar_updateHistory (me, history.peek(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput]);
						}
						throw;
					}
					if (history.peek()) {
						OTGrammar_updateHistory (me, history.peek(), storeHistoryEvery, idatum, thy rowLabels [ipartialOutput]);
					}
				}
				plasticity *= plasticityDecrement;
			}
			if (history.peek()) {
				OTGrammar_finalizeHistory (me, history.peek(), numberOfData);
			}
			OTGrammar_opt_deleteOutputMatching (me);
			if (history_out)
				*history_out = history.transfer();
		} catch (MelderError) {
			OTGrammar_opt_deleteOutputMatching (me);
			if (history_out)
				*history_out = history.transfer();   // so that we can inspect
			throw;
		}
	} catch (MelderError) {
		if (idatum > 1)
			Melder_error_ ("Only ", idatum - 1, " input-output pairs out of ", numberOfData, " were processed.");
		Melder_throw (me, " & ", thee, ": not learned from partial outputs.");
	}
}

double OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfInputs)
{
	try {
		long numberOfCorrect = 0;
		for (long ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
			wchar_t *input, *adultOutput;
			PairDistribution_peekPair (thee, & input, & adultOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			long inputTableau = OTGrammar_getTableau (me, input);
			OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [OTGrammar_getWinner (me, inputTableau)];
			if (wcsequ (learnerCandidate -> output, adultOutput))
				numberOfCorrect ++;
		}
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": fraction correct not computed.");
	}
}

long OTGrammar_PairDistribution_getMinimumNumberCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfReplications)
{
	try {
		long minimumNumberCorrect = numberOfReplications;
		for (long ipair = 1; ipair <= thy pairs -> size; ipair ++) {
			PairProbability prob = (PairProbability) thy pairs -> item [ipair];
			if (prob -> weight > 0.0) {
				long numberOfCorrect = 0;
				wchar_t *input = prob -> string1, *adultOutput = prob -> string2;
				long inputTableau = OTGrammar_getTableau (me, input);
				for (long ireplication = 1; ireplication <= numberOfReplications; ireplication ++) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
					OTGrammarCandidate learnerCandidate = & my tableaus [inputTableau]. candidates [OTGrammar_getWinner (me, inputTableau)];
					if (wcsequ (learnerCandidate -> output, adultOutput))
						numberOfCorrect ++;
				}
				if (numberOfCorrect < minimumNumberCorrect)
					minimumNumberCorrect = numberOfCorrect;
			}
		}
		return minimumNumberCorrect;
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": minimum number correct not computed.");
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
			Distributions_peek_opt (thee, columnNumber, & ipartialOutput);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			long assumedAdultInputTableau, assumedAdultCandidate;
			OTGrammar_getInterpretiveParse_opt (me, ipartialOutput, & assumedAdultInputTableau, & assumedAdultCandidate);
			OTGrammarCandidate learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
			if (wcsequ (learnerCandidate -> output, my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
				numberOfCorrect ++;
		}
		OTGrammar_opt_deleteOutputMatching (me);
		return (double) numberOfCorrect / numberOfInputs;
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": fraction correct not computed.");
	}
}

void OTGrammar_removeConstraint (OTGrammar me, const wchar_t *constraintName) {
	try {
		long removed = 0;

		if (my numberOfConstraints <= 1)
			Melder_throw ("Cannot remove last remaining constraint.");

		/*
		 * Look for the constraint to be removed.
		 */
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [icons];
			if (wcsequ (constraint -> name, constraintName)) {
				removed = icons;
				break;
			}
		}
		if (removed == 0)
			Melder_throw ("No such constraint.");
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
					my fixedRankings = NULL;
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
		Melder_throw (me, ": constraint \"", constraintName, "\" not removed.");
	}
}

static void OTGrammarTableau_removeCandidate_unstripped (OTGrammarTableau me, long candidateNumber) {
	Melder_assert (candidateNumber >= 1);
	if (candidateNumber > my numberOfCandidates) Melder_fatal ("icand %ld, ncand %ld", candidateNumber, my numberOfCandidates);
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
		int grammarHasChanged;
		double previousStratum = 101.0;
		OTGrammar_learnOne (me, my tableaus [itab]. input, my tableaus [itab]. candidates [icand]. output,
			1e-3, kOTGrammar_rerankingStrategy_EDCD, FALSE, 1.0, 0.0, TRUE, TRUE, & grammarHasChanged);
		if (! grammarHasChanged) {
			OTGrammar_restore (me);
			return true;
		}
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
	return false;   /* Cannot occur. */
}

void OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, int singly) {
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
		Melder_throw (me, ": not all harmonically bounded candidates were removed.");
	}
}

Thing_define (OTGrammar_List4, Data) {
	// new data:
	public:
		long hi1, lo1, hi2, lo2;
};

Thing_implement (OTGrammar_List4, Data, 0);

void OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee) {
	/*
	 * Save.
	 */
	OTGrammarFixedRanking savedFixedRankings = my fixedRankings;   // dangle...
	my fixedRankings = NULL;   // ...undangle
	long savedNumberOfFixedRankings = my numberOfFixedRankings;
	OTGrammar_save (me);
	try {
		long ifixedRanking, icons, jcons, kcons, lcons, ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
		long ilist, jlist, itrial, iform;
		int improved;
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
			int grammarHasChangedDuringCycle = FALSE;
			OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
			OTGrammar_newDisharmonies (me, evaluationNoise);
			for (iform = 1; iform <= thy pairs -> size; iform ++) {
				PairProbability prob = (PairProbability) thy pairs -> item [iform];
				if (prob -> weight > 0.0) {
					int grammarHasChanged;
					OTGrammar_learnOne (me, prob -> string1, prob -> string2,
						evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, TRUE /* honour fixed rankings; very important */,
						1.0, 0.0, FALSE, TRUE, & grammarHasChanged);
					if (grammarHasChanged) {
						OTGrammar_newDisharmonies (me, evaluationNoise);
					}
					grammarHasChangedDuringCycle |= grammarHasChanged;
				}
			}
			if (! grammarHasChangedDuringCycle) break;
		}
		if (itrial > 40) {
			MelderInfo_writeLine (L"There are no total rankings that generate these input-output pairs.");
			throw MelderError ();
		}
		/*
		 * Test learnability of every possible ranked pair.
		 */
		my numberOfFixedRankings ++;
		autoNUMmatrix <int> obligatory (1, my numberOfConstraints, 1, my numberOfConstraints);
		MelderInfo_open ();
		autoMelderProgress progress (L"Finding obligatory rankings.");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, Melder_integer (ipair + 1), L"/", Melder_integer (npair), L": Trying ranking ",
					my constraints [icons]. name, L" >> ", my constraints [jcons]. name);
				ipair ++;
				for (itrial = 1; itrial <= 40; itrial ++) {
					int grammarHasChangedDuringCycle = FALSE;
					OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
					OTGrammar_newDisharmonies (me, evaluationNoise);
					for (iform = 1; iform <= thy pairs -> size; iform ++) {
						PairProbability prob = (PairProbability) thy pairs -> item [iform];
						if (prob -> weight > 0.0) {
							int grammarHasChanged;
							OTGrammar_learnOne (me, prob -> string1, prob -> string2,
								evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, TRUE /* honour fixed rankings; very important */,
								1.0, 0.0, FALSE, TRUE, & grammarHasChanged);
							if (grammarHasChanged) {
								OTGrammar_newDisharmonies (me, evaluationNoise);
							}
							grammarHasChangedDuringCycle |= grammarHasChanged;
						}
					}
					if (! grammarHasChangedDuringCycle) break;
				}
				if (itrial > 40) {
					obligatory [jcons] [icons] = TRUE;
					MelderInfo_writeLine (my constraints [jcons]. name, L" >> ", my constraints [icons]. name);
					MelderInfo_close ();
				}
			}
		}
		my numberOfFixedRankings ++;
		Melder_progress (0.0, L"");
		npair = npair * npair;
		autoOrdered list = Ordered_create ();
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
						Melder_progress ((double) ipair / npair, Melder_integer (ipair + 1), L"/", Melder_integer (npair));
						ipair ++;
						for (itrial = 1; itrial <= 40; itrial ++) {
							int grammarHasChangedDuringCycle = FALSE;
							OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
							OTGrammar_newDisharmonies (me, evaluationNoise);
							for (iform = 1; iform <= thy pairs -> size; iform ++) {
								PairProbability prob = (PairProbability) thy pairs -> item [iform];
								if (prob -> weight > 0.0) {
									int grammarHasChanged;
									OTGrammar_learnOne (me, prob -> string1, prob -> string2,
										evaluationNoise, kOTGrammar_rerankingStrategy_EDCD, TRUE /* honour fixed rankings; very important */,
										1.0, 0.0, FALSE, TRUE, & grammarHasChanged);
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
							Collection_addItem (list.peek(), listElement.transfer());
						}
					}
				}
			}
		}
		Melder_progress (1.0, L"");
		/*
		 * Improve list.
		 */
		improved = TRUE;
		while (improved) {
			improved = FALSE;
			for (ilist = 1; ilist <= list -> size; ilist ++) {
				for (jlist = 1; jlist <= list -> size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = (OTGrammar_List4) list -> item [ilist], elB = (OTGrammar_List4) list -> item [jlist];
					long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
					improved |= (ahi1 == bhi1 || obligatory [bhi1] [ahi1]) && (ahi2 == bhi2 || obligatory [bhi2] [ahi2]) &&
						(alo1 == blo1 || obligatory [alo1] [blo1]) && (alo2 == blo2 || obligatory [alo2] [blo2]);
					improved |= (ahi1 == bhi2 || obligatory [bhi2] [ahi1]) && (ahi2 == bhi1 || obligatory [bhi1] [ahi2]) &&
						(alo1 == blo2 || obligatory [alo1] [blo2]) && (alo2 == blo1 || obligatory [alo2] [blo1]);
					if (improved) {
						Collection_removeItem (list.peek(), jlist);
						break;
					}
				}
				if (improved) break;
			}
		}
		improved = TRUE;
		while (improved) {
			improved = FALSE;
			for (ilist = 1; ilist <= list -> size; ilist ++) {
				for (jlist = 1; jlist <= list -> size; jlist ++) if (ilist != jlist) {
					OTGrammar_List4 elA = (OTGrammar_List4) list -> item [ilist], elB = (OTGrammar_List4) list -> item [jlist];
					long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
					long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
					improved |= ahi1 == bhi1 && alo1 == blo1 && ahi2 == bhi2 && blo2 == bhi1 && alo2 == alo1;
					improved |= ahi1 == bhi2 && alo1 == blo2 && ahi2 == bhi1 && blo1 == bhi2 && alo2 == alo1;
					improved |= ahi2 == bhi1 && alo2 == blo1 && ahi1 == bhi2 && blo2 == bhi1 && alo1 == alo2;
					improved |= ahi2 == bhi2 && alo2 == blo2 && ahi1 == bhi1 && blo1 == bhi2 && alo1 == alo2;
					if (improved) {
						Collection_removeItem (list.peek(), jlist);
						break;
					}
				}
				if (improved) break;
			}
		}
		for (ilist = 1; ilist <= list -> size; ilist ++) {
			OTGrammar_List4 el = (OTGrammar_List4) list -> item [ilist];
			MelderInfo_write (my constraints [el -> hi1]. name, L" >> ", my constraints [el -> lo1]. name, L" OR ");
			MelderInfo_writeLine (my constraints [el -> hi2]. name, L" >> ", my constraints [el -> lo2]. name);
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
		Melder_throw (me, ": obligatory rankings not listed.");
	}
}

void OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, long columnNumber) {
	/*
	 * Save.
	 */
	OTGrammarFixedRanking savedFixedRankings = my fixedRankings;
	my fixedRankings = NULL;
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
		autoMelderProgress progress (L"Finding obligatory rankings.");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
				my fixedRankings [my numberOfFixedRankings]. higher = icons;
				my fixedRankings [my numberOfFixedRankings]. lower = jcons;
				OTGrammar_reset (me, 100.0);
				Melder_progress ((double) ipair / npair, Melder_integer (ipair + 1), L"/", Melder_integer (npair), L": Trying ranking ",
					my constraints [icons]. name, L" >> ", my constraints [jcons]. name);
				ipair ++;
				Melder_progressOff ();
				OTGrammar_Distributions_learnFromPartialOutputs (me, thee, columnNumber,
					1e-9, kOTGrammar_rerankingStrategy_EDCD, TRUE /* honour fixed rankings; very important */,
					1.0, 1000, 0.0, 1, 0.0, 1, 0, NULL);
				Melder_progressOn ();
				for (kcons = 1; kcons <= my numberOfConstraints; kcons ++) {
					if (my constraints [kcons]. ranking < 0.0) {
						MelderInfo_writeLine (my constraints [jcons]. name, L" >> ", my constraints [icons]. name);
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
		Melder_throw (me, ": obligatory rankings not listed.");
	}
}

static void printConstraintNames (OTGrammar me, MelderString *buffer) {
	wchar_t text [200];
	bool secondLine = false;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		if (wcschr (constraint -> name, '\n')) {
			wchar_t *newLine;
			wcscpy (text, constraint -> name);
			newLine = wcschr (text, '\n');
			*newLine = '\0';
			MelderString_append (buffer, L"\t", text);
			secondLine = true;
		} else {
			MelderString_append (buffer, L"\t", constraint -> name);
		}
	}
	MelderString_appendCharacter (buffer, '\n');
	if (secondLine) {
		MelderString_appendCharacter (buffer, '\t');
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			wchar_t *newLine = wcschr (constraint -> name, '\n');
			MelderString_append (buffer, L"\t", newLine ? newLine + 1 : L"");
		}
		MelderString_appendCharacter (buffer, '\n');
	}
}

void OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file) {
	try {
		autoMelderString buffer;
		MelderString_append (& buffer, L"CONSTRAINTS\t");
		printConstraintNames (me, & buffer);
		MelderString_append (& buffer, L"rankings\t");
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, L"\t", Melder_double (constraint -> ranking));
		}
		MelderString_append (& buffer, L"\ndisharmonies\t");
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			MelderString_append (& buffer, L"\t", Melder_double (constraint -> disharmony));
		}
		MelderString_appendCharacter (& buffer, '\n');
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			long winner = OTGrammar_getWinner (me, itab), numberOfOptimalCandidates = 0;
			for (long icons = 1; icons <= my numberOfConstraints + 1; icons ++) {
				MelderString_appendCharacter (& buffer, '\t');
			}
			MelderString_append (& buffer, L"\nINPUT\t", tableau -> input);
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
					candidateIsOptimal == false ? L"loser" : numberOfOptimalCandidates > 1 ? L"co-winner" : L"winner",
					L"\t",
					candidate -> output);
				for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
					int index = my index [icons];
					OTGrammarConstraint constraint = & my constraints [index];
					wchar_t markString [40];
					markString [0] = '\0';
					/*
					 * An exclamation mark can be drawn in this cell only if all of the following conditions are met:
					 * 1. the candidate is not optimal;
					 * 2. the constraint is not tied;
					 * 3. this is the crucial cell, i.e. the cells after it are drawn in grey.
					 */
					if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
						int winnerMarks = tableau -> candidates [winner]. marks [index];
						for (long imark = 1; imark <= winnerMarks + 1; imark ++)
							wcscat (markString, L"*");
						wcscat (markString, L"!");
						for (long imark = winnerMarks + 2; imark <= candidate -> marks [index]; imark ++)
							wcscat (markString, L"*");
					} else {
						if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
							crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
						{
							wcscat (markString, L"=");
						}
						for (long imark = 1; imark <= candidate -> marks [index]; imark ++)
							wcscat (markString, L"*");
					}
					MelderString_append (& buffer, L"\t", markString);
				}
				MelderString_appendCharacter (& buffer, '\n');
			}
		}
		MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
	} catch (MelderError) {
		Melder_throw (me, ": not saved to tab-separated file ", file, ".");
	}
}

/* End of file OTGrammar.cpp */
