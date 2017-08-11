/* OTMulti.cpp
 *
 * Copyright (C) 2005-2012,2013,2015,2016,2017 Paul Boersma
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
 * pb 2005/06/11 the very beginning of computational bidirectional multi-level OT
 * pb 2006/05/16 guarded against cells with many violations
 * pb 2006/05/17 draw disharmonies above tableau
 * pb 2007/05/19 decision strategies
 * pb 2007/08/12 wchar
 * pb 2007/10/01 leak and constraint plasticity
 * pb 2007/10/01 can write as encoding
 * pb 2007/11/14 drawTableau: corrected direction of arrows for positive satisfactions
 * pb 2008/04/14 OTMulti_getConstraintIndexFromName
 * pb 2009/03/18 modern enums
 * pb 2010/06/05 corrected colours
 * pb 2011/03/01 multiple update rules; more decision strategies
 * pb 2011/03/29 C++
 * pb 2011/04/27 Melder_debug 41 and 42
 * pb 2011/05/15 prevented zero sums of probabilities in MaxEnt
 */

#include "OTMulti.h"

#include "oo_DESTROY.h"
#include "OTMulti_def.h"
#include "oo_COPY.h"
#include "OTMulti_def.h"
#include "oo_EQUAL.h"
#include "OTMulti_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "OTMulti_def.h"
#include "oo_WRITE_BINARY.h"
#include "OTMulti_def.h"
#include "oo_READ_BINARY.h"
#include "OTMulti_def.h"
#include "oo_DESCRIPTION.h"
#include "OTMulti_def.h"

void structOTMulti :: v_info ()
{
	structDaata :: v_info ();
	long numberOfViolations = 0;
	for (long icand = 1; icand <= numberOfCandidates; icand ++) {
		for (long icons = 1; icons <= numberOfConstraints; icons ++) {
			numberOfViolations += candidates [icand]. marks [icons];
		}
	}
	MelderInfo_writeLine (U"Decision strategy: ", kOTGrammar_decisionStrategy_getText (decisionStrategy));
	MelderInfo_writeLine (U"Number of constraints: ", numberOfConstraints);
	MelderInfo_writeLine (U"Number of candidates: ", numberOfCandidates);
	MelderInfo_writeLine (U"Number of violation marks: ", numberOfViolations);
}

void structOTMulti :: v_writeText (MelderFile file) {
	MelderFile_write (file, U"\n<", kOTGrammar_decisionStrategy_getText (decisionStrategy),
		U">\n", leak, U" ! leak\n", numberOfConstraints, U" constraints");
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
		OTConstraint constraint = & constraints [icons];
		MelderFile_write (file, U"\n\t\"");
		for (const char32 *p = & constraint -> name [0]; *p; p ++) {
			if (*p == U'\"') MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", constraint -> ranking,
			U" ", constraint -> disharmony, U" ", constraint -> plasticity);
	}
	MelderFile_write (file, U"\n\n", numberOfCandidates, U" candidates");
	for (long icand = 1; icand <= numberOfCandidates; icand ++) {
		OTCandidate candidate = & candidates [icand];
		MelderFile_write (file, U"\n\t\"");
		for (const char32 *p = & candidate -> string [0]; *p; p ++) {
			if (*p == U'\"') MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\"  ");
		for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
			MelderFile_write (file, U" ", candidate -> marks [icons]);
		}
	}
}

void OTMulti_checkIndex (OTMulti me) {
	if (my index) return;
	my index = NUMvector <long> (1, my numberOfConstraints);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTMulti_sort (me);
}

void structOTMulti :: v_readText (MelderReadText text, int formatVersion) {
	OTMulti_Parent :: v_readText (text, formatVersion);
	if (formatVersion >= 1) {
		try {
			decisionStrategy = texgete8 (text, kOTGrammar_decisionStrategy_getValue);
		} catch (MelderError) {
			Melder_throw (U"Decision strategy not read.");
		}
	}
	if (formatVersion >= 2) {
		try {
			leak = texgetr64 (text);
		} catch (MelderError) {
			Melder_throw (U"Trying to read leak.");
		}
	}
	if ((numberOfConstraints = texgeti32 (text)) < 1) Melder_throw (U"No constraints.");
	constraints = NUMvector <structOTConstraint> (1, numberOfConstraints);
	for (long icons = 1; icons <= numberOfConstraints; icons ++) {
		OTConstraint constraint = & constraints [icons];
		constraint -> name = texgetw16 (text);
		constraint -> ranking = texgetr64 (text);
		constraint -> disharmony = texgetr64 (text);
		if (formatVersion < 2) {
			constraint -> plasticity = 1.0;
		} else {
			try {
				constraint -> plasticity = texgetr64 (text);
			} catch (MelderError) {
				Melder_throw (U"Plasticity of constraint ", icons, U" not read.");
			}
		}
	}
	if ((numberOfCandidates = texgeti32 (text)) < 1) Melder_throw (U"No candidates.");
	candidates = NUMvector <structOTCandidate> (1, numberOfCandidates);
	for (long icand = 1; icand <= numberOfCandidates; icand ++) {
		OTCandidate candidate = & candidates [icand];
		candidate -> string = texgetw16 (text);
		candidate -> numberOfConstraints = numberOfConstraints;   // redundancy, needed for writing binary
		candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints);
		for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
			candidate -> marks [icons] = texgeti16 (text);
		}
	}
	OTMulti_checkIndex (this);
}

Thing_implement (OTMulti, Daata, 2);

long OTMulti_getConstraintIndexFromName (OTMulti me, const char32 *name) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		if (Melder_equ (my constraints [icons]. name, name)) {
			return icons;
		}
	}
	return 0;
}

static OTMulti constraintCompare_grammar;

static int constraintCompare (const void *first, const void *second) {
	OTMulti me = constraintCompare_grammar;
	long icons = * (long *) first, jcons = * (long *) second;
	OTConstraint ci = & my constraints [icons], cj = & my constraints [jcons];
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

void OTMulti_sort (OTMulti me) {
	constraintCompare_grammar = me;
	qsort (& my index [1], my numberOfConstraints, sizeof (long), constraintCompare);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [my index [icons]];
		constraint -> tiedToTheLeft = icons > 1 &&
			my constraints [my index [icons - 1]]. disharmony == constraint -> disharmony;
		constraint -> tiedToTheRight = icons < my numberOfConstraints &&
			my constraints [my index [icons + 1]]. disharmony == constraint -> disharmony;
	}
}

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, evaluationNoise);
	}
	OTMulti_sort (me);
}

int OTMulti_compareCandidates (OTMulti me, long icand1, long icand2) {
	int *marks1 = my candidates [icand1]. marks;
	int *marks2 = my candidates [icand2]. marks;
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
	} else {
		Melder_fatal (U"Unimplemented decision strategy.");
	}
	return 0;   /* None of the comparisons found a difference between the two candidates. Hence, they are equally good. */
}

int OTMulti_candidateMatches (OTMulti me, long icand, const char32 *form1, const char32 *form2) {
	const char32 *string = my candidates [icand]. string;
	return (form1 [0] == '\0' || str32str (string, form1)) && (form2 [0] == '\0' || str32str (string, form2));
}

static void _OTMulti_fillInHarmonies (OTMulti me, const char32 *form1, const char32 *form2) {
	if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) return;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
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
			Melder_fatal (U"_OTMulti_fillInHarmonies: unimplemented decision strategy.");
		}
		candidate -> harmony = - disharmony;
	}
}

static void _OTMulti_fillInProbabilities (OTMulti me, const char32 *form1, const char32 *form2) {
	double maximumHarmony = -1e308;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		if (candidate -> harmony > maximumHarmony) {
			maximumHarmony = candidate -> harmony;
		}
	}
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		candidate -> probability = exp (candidate -> harmony - maximumHarmony);
		Melder_assert (candidate -> probability >= 0.0 && candidate -> probability <= 1.0);
	}
	double sumOfProbabilities = 0.0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		sumOfProbabilities += candidate -> probability;
	}
	Melder_assert (sumOfProbabilities > 0.0);   // Because at least one of them is 1.0.
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		candidate -> probability /= sumOfProbabilities;
	}
}

class MelderError_OTMulti_NoMatchingCandidate: public MelderError {};

long OTMulti_getWinner (OTMulti me, const char32 *form1, const char32 *form2) {
	try {
		long icand_best = 0;
		if (my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY ||
			my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			_OTMulti_fillInHarmonies (me, form1, form2);
			_OTMulti_fillInProbabilities (me, form1, form2);
			double cutOff = NUMrandomUniform (0.0, 1.0);
			double sumOfProbabilities = 0.0;
			for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				sumOfProbabilities += my candidates [icand]. probability;
				if (sumOfProbabilities > cutOff) {
					icand_best = icand;
					break;
				}
			}
		} else {
			long numberOfBestCandidates = 0;
			for (long icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				if (icand_best == 0) {
					icand_best = icand;
					numberOfBestCandidates = 1;
				} else {
					int comparison = OTMulti_compareCandidates (me, icand, icand_best);
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
		}
		if (icand_best == 0) {
			Melder_appendError (U"The forms ", form1, U" and ", form2, U" do not match any candidate.");
			throw MelderError_OTMulti_NoMatchingCandidate ();   // BUG: NYI
		}
		return icand_best;
	} catch (MelderError) {
		Melder_throw (me, U": winner not determined.");
	}
}

static void OTMulti_modifyRankings (OTMulti me, long iwinner, long iloser,
	enum kOTGrammar_rerankingStrategy updateRule,
	double plasticity, double relativePlasticityNoise)
{
	bool *grammarHasChanged = nullptr;   // to be implemented
	bool warnIfStalled = false;   // to be implemented
	if (iwinner == iloser) return;
	OTCandidate winner = & my candidates [iwinner], loser = & my candidates [iloser];
	double step = relativePlasticityNoise == 0.0 ? plasticity : NUMrandomGauss (plasticity, relativePlasticityNoise * plasticity);
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
		OTConstraint constraint = & my constraints [icons];
		double constraintStep = step * constraint -> plasticity;
		int winnerMarks = winner -> marks [icons];
		int loserMarks = loser -> marks [icons];
		if (loserMarks > winnerMarks) {
			if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
			constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
			if (grammarHasChanged) *grammarHasChanged = true;
		}
		if (winnerMarks > loserMarks) {
			if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
			constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
			if (grammarHasChanged) *grammarHasChanged = true;
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy_SYMMETRIC_ALL) {
		bool changed = false;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
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
		if (changed && my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG) {
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
		int winningConstraints = 0, losingConstraints = 0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) losingConstraints ++;
			if (winnerMarks > loserMarks) winningConstraints ++;
		}
		if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
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
		if (changed && my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG) {
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
		int winningConstraints = 0, losingConstraints = 0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) losingConstraints ++;
			if (winnerMarks > loserMarks) winningConstraints ++;
		}
		if (winningConstraints != 0) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
					//constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) * winningConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					//constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * losingConstraints;
					if (grammarHasChanged) *grammarHasChanged = true;
				}
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
		if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++)  {
			OTConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > 0) {
				if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
				if (grammarHasChanged) *grammarHasChanged = true;
			}
			if (winnerMarks > 0) {
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
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
			int winnerMarks = winner -> marks [my index [icons]];   // order is important, so indirect
			int loserMarks = loser -> marks [my index [icons]];
			if (loserMarks < winnerMarks) break;
			if (loserMarks > winnerMarks) equivalent = false;
		}
		if (icons > my numberOfConstraints) {   // completed the loop?
			if (warnIfStalled && ! equivalent)
				Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
					U"Correct output: ", loser -> string, U"\nLearner's output: ", winner -> string);
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
					OTConstraint constraint = & my constraints [icons];
					if (constraint -> ranking >= pivotRanking) {
						numberOfConstraintsToDemote += 1;
					}
				}
			}
			if (numberOfConstraintsToDemote > 0) {
				for (icons = 1; icons <= my numberOfConstraints; icons ++) {
					OTConstraint constraint = & my constraints [icons];
					if (constraint -> ranking < pivotRanking) {
						constraint -> ranking -= numberOfConstraintsToDemote * step * constraint -> plasticity;
						if (grammarHasChanged) *grammarHasChanged = true;
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
			int winnerMarks = winner -> marks [my index [icons]];   // For the vacation version, the order is important, so indirect.
			int loserMarks = loser -> marks [my index [icons]];
			if (loserMarks > winnerMarks) {
				OTConstraint constraint = & my constraints [my index [icons]];
				double constraintStep = step * constraint -> plasticity;
				if (constraint -> ranking >= pivotRanking) {
					numberOfConstraintsDemoted += 1;
					constraint -> ranking = pivotRanking - numberOfConstraintsDemoted * constraintStep;   // This preserves the order of the demotees.
					if (grammarHasChanged) *grammarHasChanged = true;
				}
			}
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy_DEMOTION_ONLY) {
		/*
		 * Determine the crucial loser mark.
		 */
		long crucialLoserMark;
		OTConstraint offendingConstraint;
		long icons = 1;
		for (; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
			int loserMarks = loser -> marks [my index [icons]];
			if (my constraints [my index [icons]]. tiedToTheRight)
				Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
			if (loserMarks < winnerMarks) {
				if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
					Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
				} else {
					// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
				}
			}
			if (loserMarks > winnerMarks) break;
		}
		if (icons > my numberOfConstraints) {   // completed the loop?
			if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
				Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"", loser -> string, U"\", winner \"", winner -> string, U"\".");
			} else {
				// do nothing
			}
		} else {
			crucialLoserMark = icons;
			/*
			 * Demote the highest uniquely violated constraint in the loser.
			 */
			offendingConstraint = & my constraints [my index [crucialLoserMark]];
			double constraintStep = step * offendingConstraint -> plasticity;
			offendingConstraint -> ranking -= constraintStep;
			if (grammarHasChanged) *grammarHasChanged = true;
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN) {
		bool changed = false;
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
				OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / numberOfUp;
				}
			}
			long crucialLoserMark, winnerMarks = 0, loserMarks = 0;
			OTConstraint offendingConstraint;
			long icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				winnerMarks = winner -> marks [my index [icons]];   // order is important, so indirect
				loserMarks = loser -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
						Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
					} else {
						// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
					}
				}
				if (loserMarks > winnerMarks) break;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
					Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"", loser -> string, U"\", winner \"", winner -> string, U"\".");
				} else {
					// do nothing
				}
			} else {
				crucialLoserMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the loser.
				 */
				offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
			}
		}
		if (grammarHasChanged) *grammarHasChanged = changed;
	} else if (updateRule == kOTGrammar_rerankingStrategy_WEIGHTED_ALL_UP_HIGHEST_DOWN_2012) {
		bool changed = false;
		long numberOfUp = 0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (winnerMarks > loserMarks) {
				numberOfUp ++;
			}
		}
		if (/*true ||*/ numberOfUp > 0) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [icons];
				int loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / (numberOfUp + 1);
				}
			}
			long crucialLoserMark, winnerMarks = 0, loserMarks = 0;
			OTConstraint offendingConstraint;
			long icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
				loserMarks = loser -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
						Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
					} else {
						// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
					}
				}
				if (loserMarks > winnerMarks) break;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
					Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"", loser -> string, U"\", winner \"", winner -> string, U"\".");
				} else {
					// do nothing
				}
			} else {
				crucialLoserMark = icons;
				/*
				 * Demote the highest uniquely violated constraint in the loser.
				 */
				offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
			}
		}
		if (grammarHasChanged) *grammarHasChanged = changed;
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
		if (numberOfUp > 0) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				long constraintIndex = my index [icons];
				OTConstraint constraint = & my constraints [constraintIndex];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
				int loserMarks = loser -> marks [constraintIndex];
				if (my constraints [constraintIndex]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
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
			if (grammarHasChanged) *grammarHasChanged = true;
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
		if (numberOfUp > 0) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				long constraintIndex = my index [icons];
				OTConstraint constraint = & my constraints [constraintIndex];
				double constraintStep = step * constraint -> plasticity;
				int winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
				int loserMarks = loser -> marks [constraintIndex];
				if (my constraints [constraintIndex]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
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
			if (grammarHasChanged) *grammarHasChanged = true;
		}
	}
}

int OTMulti_learnOne (OTMulti me, const char32 *form1, const char32 *form2,
	enum kOTGrammar_rerankingStrategy updateRule, int direction, double plasticity, double relativePlasticityNoise)
{
	long iloser = OTMulti_getWinner (me, form1, form2);
	if (direction & OTMulti_LEARN_FORWARD) {
		if (Melder_debug == 47) OTMulti_newDisharmonies (me, 2.0);
		long iwinner = OTMulti_getWinner (me, form1, U"");
		if (Melder_debug != 47 || ! OTMulti_candidateMatches (me, iwinner, form2, U""))
			OTMulti_modifyRankings (me, iwinner, iloser, updateRule, plasticity, relativePlasticityNoise);
	}
	if (direction & OTMulti_LEARN_BACKWARD) {
		if (Melder_debug == 47) OTMulti_newDisharmonies (me, 2.0);
		long iwinner = OTMulti_getWinner (me, form2, U"");
		if (Melder_debug != 47 || ! OTMulti_candidateMatches (me, iwinner, form1, U""))
			OTMulti_modifyRankings (me, iwinner, iloser, updateRule, plasticity, relativePlasticityNoise);
	}
	return 1;
}

static autoTable OTMulti_createHistory (OTMulti me, long storeHistoryEvery, long numberOfData)
{
	try {
		long numberOfSamplingPoints = numberOfData / storeHistoryEvery;   // e.g. 0, 20, 40, ...
		autoTable thee = Table_createWithoutColumnNames (1 + numberOfSamplingPoints, 3 + my numberOfConstraints);
		Table_setColumnLabel (thee.get(), 1, U"Datum");
		Table_setColumnLabel (thee.get(), 2, U"Form1");
		Table_setColumnLabel (thee.get(), 3, U"Form2");
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			Table_setColumnLabel (thee.get(), 3 + icons, my constraints [icons]. name);
		}
		Table_setNumericValue (thee.get(), 1, 1, 0);
		Table_setStringValue (thee.get(), 1, 2, U"(initial)");
		Table_setStringValue (thee.get(), 1, 3, U"(initial)");
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			Table_setNumericValue (thee.get(), 1, 3 + icons, my constraints [icons]. ranking);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": history not created.");
	}
}

static int OTMulti_updateHistory (OTMulti me, Table thee, long storeHistoryEvery, long idatum, const char32 *form1, const char32 *form2)
{
	try {
		if (idatum % storeHistoryEvery == 0) {
			long irow = 1 + idatum / storeHistoryEvery;
			Table_setNumericValue (thee, irow, 1, idatum);
			Table_setStringValue (thee, irow, 2, form1);
			Table_setStringValue (thee, irow, 3, form2);
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				Table_setNumericValue (thee, irow, 3 + icons, my constraints [icons]. ranking);
			}
		}
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": history not updated.");
	}
}


void OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee, double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int direction,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long storeHistoryEvery, autoTable *history_out)
{
	long idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		double plasticity = initialPlasticity;
		autoMelderMonitor monitor (U"Learning from partial pairs...");
		if (monitor.graphics()) {
			Graphics_clearWs (monitor.graphics());
		}
		autoTable history;
		if (storeHistoryEvery) {
			history = OTMulti_createHistory (me, storeHistoryEvery, numberOfData);
		}
		for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
			for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
				char32 *form1, *form2;
				PairDistribution_peekPair (thee, & form1, & form2);
				++ idatum;
				if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
					long numberOfDrawnConstraints = my numberOfConstraints < 14 ? my numberOfConstraints : 14;
					if (numberOfDrawnConstraints > 0) {
						double sumOfRankings = 0.0;
						for (long icons = 1; icons <= numberOfDrawnConstraints; icons ++) {
							sumOfRankings += my constraints [icons]. ranking;
						}
						double meanRanking = sumOfRankings / numberOfDrawnConstraints;
						Graphics_beginMovieFrame (monitor.graphics(), nullptr);
						Graphics_setWindow (monitor.graphics(), 0, numberOfData, meanRanking - 50, meanRanking + 50);
						for (long icons = 1; icons <= numberOfDrawnConstraints; icons ++) {
							Graphics_setGrey (monitor.graphics(), (double) icons / numberOfDrawnConstraints);
							Graphics_line (monitor.graphics(), idatum, my constraints [icons]. ranking,
								idatum, my constraints [icons]. ranking+1);
						}
						Graphics_endMovieFrame (monitor.graphics(), 0.0);
					}
				}
				try {
					Melder_monitor ((double) idatum / numberOfData,
						U"Processing partial pair ", idatum, U" out of ", numberOfData,
						U":\n      ", form1, U"     ", form2);
				} catch (MelderError) {
					if (history_out)
						*history_out = history.move();   // so that we can inspect
					throw;
				}
				OTMulti_newDisharmonies (me, evaluationNoise);
				try {
					OTMulti_learnOne (me, form1, form2, updateRule, direction, plasticity, relativePlasticityNoise);
				} catch (MelderError) {
					if (history) {
						OTMulti_updateHistory (me, history.get(), storeHistoryEvery, idatum, form1, form2);
					}
					throw;
				}
				if (history) {
					OTMulti_updateHistory (me, history.get(), storeHistoryEvery, idatum, form1, form2);
				}
			}
			plasticity *= plasticityDecrement;
		}
		if (history_out)
			*history_out = history.move();
	} catch (MelderError) {
		if (idatum > 1)
			Melder_appendError (U"Only ", idatum - 1, U" input-output pairs out of ", numberOfData, U" were processed.");
		Melder_throw (me, U" & ", thee, U": learning from partial pairs not completed.");
	}
}

static long OTMulti_crucialCell (OTMulti me, long icand, long iwinner, long numberOfOptimalCandidates, const char32 *form1, const char32 *form2)
{
	if (my numberOfCandidates < 2) return 0;   // if there is only one candidate, all cells can be greyed
	if (OTMulti_compareCandidates (me, icand, iwinner) == 0) {   // candidate equally good as winner?
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			long secondBest = 0;
			for (long jcand = 1; jcand <= my numberOfCandidates; jcand ++) {
				if (OTMulti_candidateMatches (me, jcand, form1, form2) && OTMulti_compareCandidates (me, jcand, iwinner) != 0) {   // a non-optimal candidate?
					if (secondBest == 0) {
						secondBest = jcand;   // first guess
					} else if (OTMulti_compareCandidates (me, jcand, secondBest) < 0) {
						secondBest = jcand;   // better guess
					}
				}
			}
			if (secondBest == 0) return 0;   // if all candidates are equally good, all cells can be greyed
			return OTMulti_crucialCell (me, secondBest, iwinner, 1, form1, form2);
		}
	} else {
		int *candidateMarks = my candidates [icand]. marks;
		int *winnerMarks = my candidates [iwinner]. marks;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfCandidateMarks = candidateMarks [my index [icons]];
			int numberOfWinnerMarks = winnerMarks [my index [icons]];
			if (numberOfCandidateMarks > numberOfWinnerMarks) {
				return icons;
			}
		}
	}
	return my numberOfConstraints;   /* Nothing grey. */
}

static double OTMulti_constraintWidth (Graphics g, OTConstraint constraint, bool showDisharmony) {
	char32 text [100], *newLine;
	double maximumWidth = showDisharmony ? 0.8 * Graphics_textWidth_ps (g, Melder_fixed (constraint -> disharmony, 1), true) : 0.0,
		firstWidth, secondWidth;
	str32cpy (text, constraint -> name);
	newLine = str32chr (text, U'\n');
	if (newLine) {
		*newLine = '\0';
		firstWidth = Graphics_textWidth_ps (g, text, true);
		if (firstWidth > maximumWidth) maximumWidth = firstWidth;
		secondWidth = Graphics_textWidth_ps (g, newLine + 1, true);
		if (secondWidth > maximumWidth) maximumWidth = secondWidth;
		return maximumWidth;
	}
	firstWidth = Graphics_textWidth_ps (g, text, true);
	if (firstWidth > maximumWidth) maximumWidth = firstWidth;
	return maximumWidth;
}

void OTMulti_drawTableau (OTMulti me, Graphics g, const char32 *form1, const char32 *form2, bool vertical, bool showDisharmonies) {
	long winner, winner1 = 0, winner2 = 0;
	double x, y, fontSize = Graphics_inqFontSize (g);
	Graphics_Colour colour = Graphics_inqColour (g);
	char32 text [200];
	bool bidirectional = form1 [0] != U'\0' && form2 [0] != U'\0';
	try {
		winner = OTMulti_getWinner (me, form1, form2);
	} catch (MelderError) {
		Melder_clearError ();
		Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_rectangle (g, 0, 1, 0, 1);
		Graphics_text (g, 0.0, 0.5, U"(no matching candidates)");
		return;
	}

	if (bidirectional) {
		winner1 = OTMulti_getWinner (me, form1, U"");
		winner2 = OTMulti_getWinner (me, form2, U"");
	}
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	const double margin = Graphics_dxMMtoWC (g, 1.0);
	const double fingerWidth = Graphics_dxMMtoWC (g, 7.0) * fontSize / 12.0;
	const double doubleLineDx = Graphics_dxMMtoWC (g, 0.9);
	const double doubleLineDy = Graphics_dyMMtoWC (g, 0.9);
	const double rowHeight = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72);
	const double descent = rowHeight * 0.5;
	const double worldAspectRatio = Graphics_dyMMtoWC (g, 1.0) / Graphics_dxMMtoWC (g, 1.0);   // because Graphics_textWidth measures in the x direction only
	/*
	 * Compute height of header row.
	 */
	double headerHeight;
	if (vertical) {
		headerHeight = 0.0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
			double constraintTextWidth = Graphics_textWidth (g, constraint -> name);
			if (constraintTextWidth > headerHeight)
				headerHeight = constraintTextWidth;
		}
		headerHeight += margin * 2;
		headerHeight *= worldAspectRatio;
	} else {
		headerHeight = rowHeight;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
			if (str32chr (constraint -> name, U'\n')) {
				headerHeight += 0.7 * rowHeight;
				break;
			}
		}
	}
	/*
	 * Compute longest candidate string.
	 * Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
	 */
	double candWidth = Graphics_textWidth_ps (g, form1, true) + Graphics_textWidth_ps (g, form2, true);
	long numberOfMatchingCandidates = 0;
	long numberOfOptimalCandidates = 0;
	long numberOfOptimalCandidates1 = 0;
	long numberOfOptimalCandidates2 = 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		if ((form1 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form1, U"")) ||
		    (form2 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form2, U"")) ||
		    (form1 [0] == U'\0' && form2 [0] == U'\0'))
		{
			double width = Graphics_textWidth_ps (g, my candidates [icand]. string, true);
			if (width > candWidth) candWidth = width;
			numberOfMatchingCandidates ++;
			if (OTMulti_compareCandidates (me, icand, winner) == 0) {
				numberOfOptimalCandidates ++;
			}
			if (winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0) {
				numberOfOptimalCandidates1 ++;
			}
			if (winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0) {
				numberOfOptimalCandidates2 ++;
			}
		}
	}
	candWidth += fingerWidth * (bidirectional ? 3 : 1) + margin * 3;
	/*
	 * Compute tableau width.
	 */
	double tableauWidth = candWidth + doubleLineDx;
	if (vertical) {
		tableauWidth += rowHeight * my numberOfConstraints / worldAspectRatio;
	} else {
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
			tableauWidth += OTMulti_constraintWidth (g, constraint, showDisharmonies);
		}
		tableauWidth += margin * 2 * my numberOfConstraints;
	}
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   // left side of tableau
	y = 1.0 - doubleLineDy;
	if (showDisharmonies) y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
	/*
	 * Draw input.
	 */
	y -= headerHeight;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_text (g, x + 0.5 * candWidth, y + 0.5 * headerHeight,   form1, form2);
	Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
	/*
	 * Draw constraint names.
	 */
	x += candWidth + doubleLineDx;
	if (vertical) Graphics_setTextRotation (g, 90.0);
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [my index [icons]];
		double width = vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
		if (str32chr (constraint -> name, U'\n')) {
			char32 *newLine;
			Melder_sprint (text,200, constraint -> name);
			newLine = str32chr (text, U'\n');
			*newLine = U'\0';
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
		if (showDisharmonies) {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_setFontSize (g, 0.8 * fontSize);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, Melder_fixed (constraint -> disharmony, 1));
			Graphics_setFontSize (g, fontSize);
		}
		Graphics_line (g, x, y, x, y + headerHeight);
		Graphics_line (g, x, y, x + width, y);
		x += width;
	}
	if (vertical) Graphics_setTextRotation (g, 0.0);
	/*
	 * Draw candidates.
	 */
	y -= doubleLineDy;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++)
		if ((form1 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form1, U"")) ||
		    (form2 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form2, U"")) ||
		    (form1 [0] == U'\0' && form2 [0] == U'\0'))
	{
		long crucialCell = OTMulti_crucialCell (me, icand, winner, numberOfOptimalCandidates, form1, form2);
		int candidateIsOptimal = OTMulti_compareCandidates (me, icand, winner) == 0;
		int candidateIsOptimal1 = winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0;
		int candidateIsOptimal2 = winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0;
		/*
		 * Draw candidate transcription.
		 */
		x = doubleLineDx;
		y -= rowHeight;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x + candWidth - margin, y + descent, my candidates [icand]. string);
		if (candidateIsOptimal) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, (int) ((bidirectional ? 1.2 : 1.5) * fontSize));
			if (numberOfOptimalCandidates > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, bidirectional ? U"\\Vr" : U"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal1) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates1 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_text (g, x + margin + fingerWidth, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, U"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal2) {
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates2 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_setTextRotation (g, 180);
			Graphics_text (g, x + margin + fingerWidth * 2, y + descent - Graphics_dyMMtoWC (g, 0.0) * fontSize / 12.0, U"\\pf");
			Graphics_setTextRotation (g, 0);
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, (int) fontSize);
		}
		Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
		/*
		 * Draw grey cell backgrounds.
		 */
		if (! bidirectional && my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
			x = candWidth + 2 * doubleLineDx;
			Graphics_setGrey (g, 0.9);
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				int index = my index [icons];
				OTConstraint constraint = & my constraints [index];
				double width = vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
				if (icons > crucialCell)
					Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
				x += width;
			}
			Graphics_setColour (g, colour);
		}
		/*
		 * Draw cell marks.
		 */
		x = candWidth + 2 * doubleLineDx;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int index = my index [icons];
			OTConstraint constraint = & my constraints [index];
			double width = vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
			char32 markString [40];
			markString [0] = U'\0';
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal) {
					str32cpy (markString + str32len (markString), U"\\<-");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1) {
					str32cpy (markString + str32len (markString), U"\\<-");
				}
				if (candidateIsOptimal && ! candidateIsOptimal2) {
					str32cpy (markString + str32len (markString), U"\\<-");
				}
			}
			/*
			 * An exclamation mark can be drawn in this cell only if both of the following conditions are met:
			 * 1. the candidate is not optimal;
			 * 2. this is the crucial cell, i.e. the cells after it are drawn in grey.
			 */
			if (! bidirectional && icons == crucialCell && ! candidateIsOptimal &&
			    my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY)
			{
				int winnerMarks = my candidates [winner]. marks [index];
				if (winnerMarks + 1 > 5) {
					str32cpy (markString + str32len (markString), Melder_integer (winnerMarks + 1));
				} else {
					for (long imark = 1; imark <= winnerMarks + 1; imark ++)
						str32cpy (markString + str32len (markString), U"*");
				}
				for (long imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
					str32cpy (markString + str32len (markString), U"+");
				str32cpy (markString + str32len (markString), U"!");
				if (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1 > 5) {
					str32cpy (markString + str32len (markString), Melder_integer (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1));
				} else {
					for (long imark = winnerMarks + 2; imark <= my candidates [icand]. marks [index]; imark ++)
						str32cpy (markString + str32len (markString), U"*");
				}
			} else {
				if (my candidates [icand]. marks [index] > 5) {
					str32cpy (markString + str32len (markString), Melder_integer (my candidates [icand]. marks [index]));
				} else {
					for (long imark = 1; imark <= my candidates [icand]. marks [index]; imark ++)
						str32cpy (markString + str32len (markString), U"*");
					for (long imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
						str32cpy (markString + str32len (markString), U"+");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1) {
					str32cpy (markString + str32len (markString), U"\\->");
				}
				if (candidateIsOptimal && ! candidateIsOptimal2) {
					str32cpy (markString + str32len (markString), U"\\->");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal) {
					str32cpy (markString + str32len (markString), U"\\->");
				}
			}
			Graphics_text (g, x + 0.5 * width, y + descent, markString);
			Graphics_setColour (g, colour);
			Graphics_line (g, x, y, x, y + rowHeight);
			Graphics_line (g, x, y + rowHeight, x + width, y + rowHeight);
			x += width;
		}
	}
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	if (showDisharmonies) y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
}

void OTMulti_reset (OTMulti me, double ranking) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTMulti_sort (me);
}

void OTMulti_setRanking (OTMulti me, long constraint, double ranking, double disharmony) {
	try {
		if (constraint < 1)
			Melder_throw (U"The constraint number should be positive, not ", constraint, U".");
		if (constraint > my numberOfConstraints)
			Melder_throw (U"Constraint ", constraint, U" does not exist (there are only ", my numberOfConstraints, U" constraints).");
		my constraints [constraint]. ranking = ranking;
		my constraints [constraint]. disharmony = disharmony;
		OTMulti_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": ranking not set.");
	}
}

void OTMulti_setConstraintPlasticity (OTMulti me, long constraint, double plasticity) {
	try {
		if (constraint < 1)
			Melder_throw (U"The constraint number should be positive, not ", constraint, U".");
		if (constraint > my numberOfConstraints)
			Melder_throw (U"Constraint ", constraint, U" does not exist (there are only ", my numberOfConstraints, U" constraints).");
		my constraints [constraint]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, U": constraint plasticity not set.");
	}
}

void OTMulti_removeConstraint (OTMulti me, const char32 *constraintName) {
	try {
		long removed = 0;

		if (my numberOfConstraints <= 1)
			Melder_throw (me, U": cannot remove last constraint.");

		/*
		 * Look for the constraint to be removed.
		 */
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTConstraint constraint = & my constraints [icons];
			if (str32equ (constraint -> name, constraintName)) {
				removed = icons;
				break;
			}
		}
		if (removed == 0)
			Melder_throw (U"No constraint \"", constraintName, U"\".");
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
		 * Shift tableau rows.
		 */
		for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
			OTCandidate candidate = & my candidates [icand];
			candidate -> numberOfConstraints -= 1;
			for (long icons = removed; icons <= my numberOfConstraints; icons ++) {
				candidate -> marks [icons] = candidate -> marks [icons + 1];
			}
		}
		/*
		 * Rebuild index.
		 */
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
		OTMulti_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": constraint not removed.");
	}
}

void OTMulti_generateOptimalForm (OTMulti me, const char32 *form1, const char32 *form2, char32 *optimalForm, double evaluationNoise) {
	try {
		OTMulti_newDisharmonies (me, evaluationNoise);
		long winner = OTMulti_getWinner (me, form1, form2);
		str32cpy (optimalForm, my candidates [winner]. string);
	} catch (MelderError) {
		Melder_throw (me, U": optimal form not generated.");
	}
}

autoStrings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings thee, double evaluationNoise) {
	try {
		autoStrings outputs = Thing_new (Strings);
		long n = thy numberOfStrings;
		outputs -> numberOfStrings = n;
		outputs -> strings = NUMvector <char32 *> (1, n);
		for (long i = 1; i <= n; i ++) {
			char32 output [100];
			OTMulti_generateOptimalForm (me, thy strings [i], U"", output, evaluationNoise);
			outputs -> strings [i] = Melder_dup (output);
		}
		return outputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": optimal forms not generated.");
	}
}

autoStrings OTMulti_generateOptimalForms (OTMulti me, const char32 *form1, const char32 *form2, long numberOfTrials, double evaluationNoise) {
	try {
		autoStrings outputs = Thing_new (Strings);
		outputs -> numberOfStrings = numberOfTrials;
		outputs -> strings = NUMvector <char32 *> (1, numberOfTrials);
		for (long i = 1; i <= numberOfTrials; i ++) {
			char32 output [100];
			OTMulti_generateOptimalForm (me, form1, form2, output, evaluationNoise);
			outputs -> strings [i] = Melder_dup (output);
		}
		return outputs;
	} catch (MelderError) {
		Melder_throw (me, U": optimal forms not generated.");
	}
}

autoDistributions OTMulti_to_Distribution (OTMulti me, const char32 *form1, const char32 *form2,
	long numberOfTrials, double evaluationNoise)
{
	try {
		long totalNumberOfOutputs = 0, iout = 0;
		/*
		 * Count the total number of outputs.
		 */
		for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
			if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				totalNumberOfOutputs ++;
			}
		}
		/*
		 * Create the distribution. One row for every output form.
		 */
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1);
		autoNUMvector <long> index (1, my numberOfCandidates);
		/*
		 * Set the row labels to the output strings.
		 */
		iout = 0;
		for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
			if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				thy rowLabels [++ iout] = Melder_dup (my candidates [icand]. string);
				index [icand] = iout;
			}
		}
		/*
		 * Compute a number of outputs and store the results.
		 */
		for (long itrial = 1; itrial <= numberOfTrials; itrial ++) {
			OTMulti_newDisharmonies (me, evaluationNoise);
			long iwinner = OTMulti_getWinner (me, form1, form2);
			thy data [index [iwinner]] [1] += 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": distribution not computed.");
	}
}

/* End of file OTMulti.cpp */
