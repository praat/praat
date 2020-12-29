/* OTMulti.cpp
 *
 * Copyright (C) 2005-2020 Paul Boersma
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
	integer numberOfViolations = 0;
	for (integer icand = 1; icand <= our numberOfCandidates; icand ++)
		for (integer icons = 1; icons <= our numberOfConstraints; icons ++)
			numberOfViolations += our candidates [icand]. marks [icons];
	MelderInfo_writeLine (U"Decision strategy: ", kOTGrammar_decisionStrategy_getText (decisionStrategy));
	MelderInfo_writeLine (U"Number of constraints: ", our numberOfConstraints);
	MelderInfo_writeLine (U"Number of candidates: ", our numberOfCandidates);
	MelderInfo_writeLine (U"Number of violation marks: ", numberOfViolations);
}

void structOTMulti :: v_writeText (MelderFile file) {
	MelderFile_write (file, U"\n<", kOTGrammar_decisionStrategy_getText (decisionStrategy),
		U">\n", leak, U" ! leak\n", our numberOfConstraints, U" constraints");
	for (integer icons = 1; icons <= our numberOfConstraints; icons ++) {
		OTConstraint constraint = & our constraints [icons];
		MelderFile_write (file, U"\n\t\"");
		for (const char32 *p = & constraint -> name [0]; *p != U'\0'; p ++) {
			if (*p == U'\"')
				MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\" ", constraint -> ranking,
				U" ", constraint -> disharmony, U" ", constraint -> plasticity);
	}
	MelderFile_write (file, U"\n\n", our numberOfCandidates, U" candidates");
	for (integer icand = 1; icand <= our numberOfCandidates; icand ++) {
		OTCandidate candidate = & our candidates [icand];
		MelderFile_write (file, U"\n\t\"");
		for (const char32 *p = & candidate -> string [0]; *p != U'\0'; p ++) {
			if (*p == U'\"')
				MelderFile_writeCharacter (file, U'\"');   // double any quotes within quotes
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write (file, U"\"  ");
		for (integer icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			MelderFile_write (file, U" ", candidate -> marks [icons]);
	}
}

void OTMulti_checkIndex (OTMulti me) {
	if (my index.size != 0)
		return;
	my index = to_INTVEC (my numberOfConstraints);
	OTMulti_sort (me);
}

void structOTMulti :: v_readText (MelderReadText text, int formatVersion) {
	OTMulti_Parent :: v_readText (text, formatVersion);
	if (formatVersion >= 1) {
		try {
			decisionStrategy = (kOTGrammar_decisionStrategy) texgete8 (text, (enum_generic_getValue) kOTGrammar_decisionStrategy_getValue);
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
	if ((our numberOfConstraints = texgeti32 (text)) < 1)
		Melder_throw (U"No constraints.");
	our constraints = newvectorzero <structOTConstraint> (our numberOfConstraints);
	for (integer icons = 1; icons <= our numberOfConstraints; icons ++) {
		OTConstraint constraint = & our constraints [icons];
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
	if ((numberOfCandidates = texgeti32 (text)) < 1)
		Melder_throw (U"No candidates.");
	our candidates = newvectorzero <structOTCandidate> (numberOfCandidates);
	for (integer icand = 1; icand <= numberOfCandidates; icand ++) {
		OTCandidate candidate = & our candidates [icand];
		candidate -> string = texgetw16 (text);
		candidate -> numberOfConstraints = our numberOfConstraints;   // redundancy, needed for writing binary
		candidate -> marks = raw_INTVEC (candidate -> numberOfConstraints);
		for (integer icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			candidate -> marks [icons] = texgeti16 (text);
	}
	OTMulti_checkIndex (this);
}

Thing_implement (OTMulti, Daata, 2);

integer OTMulti_getConstraintIndexFromName (OTMulti me, conststring32 name) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
		if (Melder_equ (my constraints [icons]. name.get(), name))
			return icons;
	return 0;
}

static OTMulti constraintCompare_grammar;

static int constraintCompare (const void *first, const void *second) {
	const OTMulti me = constraintCompare_grammar;
	const integer icons = * (integer *) first, jcons = * (integer *) second;
	const OTConstraint ci = & my constraints [icons], cj = & my constraints [jcons];
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

void OTMulti_sort (OTMulti me) {
	constraintCompare_grammar = me;
	qsort (& my index [1], my numberOfConstraints, sizeof (integer), constraintCompare);
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		const OTConstraint constraint = & my constraints [my index [icons]];
		constraint -> tiedToTheLeft = ( icons > 1 &&
				my constraints [my index [icons - 1]]. disharmony == constraint -> disharmony );
		constraint -> tiedToTheRight = ( icons < my numberOfConstraints &&
				my constraints [my index [icons + 1]]. disharmony == constraint -> disharmony );
	}
}

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		const OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, evaluationNoise);
	}
	OTMulti_sort (me);
}

int OTMulti_compareCandidates (OTMulti me, integer icand1, integer icand2) {
	INTVEC marks1 = my candidates [icand1]. marks.get();
	INTVEC marks2 = my candidates [icand2]. marks.get();
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
	} else {
		Melder_fatal (U"Unimplemented decision strategy.");
	}
	return 0;   // none of the comparisons found a difference between the two candidates; hence, they are equally good
}

int OTMulti_candidateMatches (OTMulti me, integer icand, conststring32 form1, conststring32 form2) {
	conststring32 string = my candidates [icand]. string.get();
	return (form1 [0] == U'\0' || str32str (string, form1)) && (form2 [0] == U'\0' || str32str (string, form2));
}

static void _OTMulti_fillInHarmonies (OTMulti me, conststring32 form1, conststring32 form2) {
	if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) return;
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		INTVEC marks = candidate -> marks.get();
		double disharmony = 0.0;
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
				double constraintDisharmony = std::max (my constraints [icons]. disharmony, 1.0);
				disharmony += constraintDisharmony * marks [icons];
			}
		} else {
			Melder_fatal (U"_OTMulti_fillInHarmonies: unimplemented decision strategy.");
		}
		candidate -> harmony = - disharmony;
	}
}

static void _OTMulti_fillInProbabilities (OTMulti me, conststring32 form1, conststring32 form2) {
	double maximumHarmony = -1e308;
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		if (candidate -> harmony > maximumHarmony)
			maximumHarmony = candidate -> harmony;
	}
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		candidate -> probability = exp (candidate -> harmony - maximumHarmony);
		Melder_assert (candidate -> probability >= 0.0 && candidate -> probability <= 1.0);
	}
	double sumOfProbabilities = 0.0;
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		sumOfProbabilities += candidate -> probability;
	}
	Melder_assert (sumOfProbabilities > 0.0);   // Because at least one of them is 1.0.
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
		OTCandidate candidate = & my candidates [icand];
		candidate -> probability /= sumOfProbabilities;
	}
}

class MelderError_OTMulti_NoMatchingCandidate: public MelderError {};

integer OTMulti_getWinner (OTMulti me, conststring32 form1, conststring32 form2) {
	try {
		integer icand_best = 0;
		if (my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY ||
			my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			_OTMulti_fillInHarmonies (me, form1, form2);
			_OTMulti_fillInProbabilities (me, form1, form2);
			const double cutOff = NUMrandomUniform (0.0, 1.0);
			longdouble sumOfProbabilities = 0.0;
			for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				sumOfProbabilities += my candidates [icand]. probability;
				if (sumOfProbabilities > cutOff) {
					icand_best = icand;
					break;
				}
			}
		} else {
			integer numberOfBestCandidates = 0;
			for (integer icand = 1; icand <= my numberOfCandidates; icand ++) if (OTMulti_candidateMatches (me, icand, form1, form2)) {
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
							Give all candidates that are equally good an equal chance to become the winner.
						*/
						if (Melder_debug == 41)
							icand_best = icand_best;   // keep first
						else if (Melder_debug == 42)
							icand_best = icand;   // take last
						else if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0)   // default: take random
							icand_best = icand;
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

static void OTMulti_modifyRankings (OTMulti me, integer iwinner, integer iloser,
	kOTGrammar_rerankingStrategy updateRule,
	double plasticity, double relativePlasticityNoise)
{
	bool *grammarHasChanged = nullptr;   // to be implemented
	bool warnIfStalled = false;   // to be implemented
	if (iwinner == iloser)
		return;
	OTCandidate winner = & my candidates [iwinner], loser = & my candidates [iloser];
	double step = relativePlasticityNoise == 0.0 ? plasticity : NUMrandomGauss (plasticity, relativePlasticityNoise * plasticity);
	bool multiplyStepByNumberOfViolations =
		my decisionStrategy == kOTGrammar_decisionStrategy::HARMONIC_GRAMMAR ||
		my decisionStrategy == kOTGrammar_decisionStrategy::LINEAR_OT ||
		my decisionStrategy == kOTGrammar_decisionStrategy::MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy::POSITIVE_HG ||
		my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
		my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY;
	if (Melder_debug != 0) {
		/*
			Perhaps override the standard update rule.
		*/
		if (Melder_debug == 26)
			multiplyStepByNumberOfViolations = false;   // OT-GLA
		else if (Melder_debug == 27)
			multiplyStepByNumberOfViolations = true;   // HG-GLA
	}
	if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ONE) {
		const integer icons = NUMrandomInteger (1, my numberOfConstraints);
		const OTConstraint constraint = & my constraints [icons];
		double constraintStep = step * constraint -> plasticity;
		const integer winnerMarks = winner -> marks [icons];
		const integer loserMarks = loser -> marks [icons];
		if (loserMarks > winnerMarks) {
			if (multiplyStepByNumberOfViolations)
				constraintStep *= loserMarks - winnerMarks;
			constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
			if (grammarHasChanged)
				*grammarHasChanged = true;
		}
		if (winnerMarks > loserMarks) {
			if (multiplyStepByNumberOfViolations)
				constraintStep *= winnerMarks - loserMarks;
			constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
			if (grammarHasChanged)
				*grammarHasChanged = true;
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ALL) {
		bool changed = false;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const OTConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= loserMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
				changed = true;
			}
			if (winnerMarks > loserMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - loserMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
				changed = true;
			}
		}
		if (changed && my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG) {
			longdouble sumOfWeights = 0.0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				sumOfWeights += my constraints [icons]. ranking;
			const double averageWeight = double (sumOfWeights) / my numberOfConstraints;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				my constraints [icons]. ranking -= averageWeight;
		}
		if (grammarHasChanged)
			*grammarHasChanged = changed;
	} else if (updateRule == kOTGrammar_rerankingStrategy::SYMMETRIC_ALL_SKIPPABLE) {
		bool changed = false;
		integer winningConstraints = 0, losingConstraints = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks)
				losingConstraints ++;
			if (winnerMarks > loserMarks)
				winningConstraints ++;
		}
		if (winningConstraints != 0) for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const OTConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= loserMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
				changed = true;
			}
			if (winnerMarks > loserMarks) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - loserMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
				changed = true;
			}
		}
		if (changed && my decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG) {
			longdouble sumOfWeights = 0.0;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				sumOfWeights += my constraints [icons]. ranking;
			const double averageWeight = double (sumOfWeights) / my numberOfConstraints;
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				my constraints [icons]. ranking -= averageWeight;
		}
		if (grammarHasChanged) *grammarHasChanged = changed;
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_UNCANCELLED) {
		integer winningConstraints = 0, losingConstraints = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks)
				losingConstraints ++;
			if (winnerMarks > loserMarks)
				winningConstraints ++;
		}
		if (winningConstraints != 0) {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= loserMarks - winnerMarks;
					constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
					//constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) * winningConstraints;
					if (grammarHasChanged)
						*grammarHasChanged = true;
				}
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
					//constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * losingConstraints;
					if (grammarHasChanged)
						*grammarHasChanged = true;
				}
			}
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL) {
		integer winningConstraints = 0, losingConstraints = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > 0)
				losingConstraints ++;
			if (winnerMarks > 0)
				winningConstraints ++;
		}
		if (winningConstraints != 0) for (integer icons = 1; icons <= my numberOfConstraints; icons ++)  {
			const OTConstraint constraint = & my constraints [icons];
			double constraintStep = step * constraint -> plasticity;
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (loserMarks > 0) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= loserMarks - winnerMarks;
				constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak) / losingConstraints;
				if (grammarHasChanged)
					*grammarHasChanged = true;
			}
			if (winnerMarks > 0) {
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - loserMarks;
				constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / winningConstraints;
				if (grammarHasChanged)
					*grammarHasChanged = true;
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
			const integer winnerMarks = winner -> marks [my index [icons]];   // order is important, so indirect
			const integer loserMarks = loser -> marks [my index [icons]];
			if (loserMarks < winnerMarks)
				break;
			if (loserMarks > winnerMarks)
				equivalent = false;
		}
		if (icons > my numberOfConstraints) {   // completed the loop?
			if (warnIfStalled && ! equivalent)
				Melder_warning (U"Correct output is harmonically bounded (by having strict superset violations as compared to the learner's output)! EDCD stalls.\n"
						U"Correct output: ", loser -> string.get(), U"\nLearner's output: ", winner -> string.get());
			return;
		}
		/*
			Determine the stratum into which some constraints will be demoted.
		*/
		pivotRanking = my constraints [my index [icons]]. ranking;
		if (updateRule == kOTGrammar_rerankingStrategy::EDCD_WITH_VACATION) {
			integer numberOfConstraintsToDemote = 0;
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer winnerMarks = winner -> marks [icons];
				const integer loserMarks = loser -> marks [icons];
				if (loserMarks > winnerMarks) {
					OTConstraint constraint = & my constraints [icons];
					if (constraint -> ranking >= pivotRanking)
						numberOfConstraintsToDemote += 1;
				}
			}
			if (numberOfConstraintsToDemote > 0) {
				for (icons = 1; icons <= my numberOfConstraints; icons ++) {
					const OTConstraint constraint = & my constraints [icons];
					if (constraint -> ranking < pivotRanking) {
						constraint -> ranking -= numberOfConstraintsToDemote * step * constraint -> plasticity;
						if (grammarHasChanged)
							*grammarHasChanged = true;
					}
				}
			}
		}
		/*
			Demote all the uniquely violated constraints in the loser
			that have rankings not lower than the pivot.
		*/
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			integer numberOfConstraintsDemoted = 0;
			const integer winnerMarks = winner -> marks [my index [icons]];   // for the vacation version, the order is important, so indirect
			const integer loserMarks = loser -> marks [my index [icons]];
			if (loserMarks > winnerMarks) {
				OTConstraint constraint = & my constraints [my index [icons]];
				double constraintStep = step * constraint -> plasticity;
				if (constraint -> ranking >= pivotRanking) {
					numberOfConstraintsDemoted += 1;
					constraint -> ranking = pivotRanking - numberOfConstraintsDemoted * constraintStep;   // this preserves the order of the demotees
					if (grammarHasChanged)
						*grammarHasChanged = true;
				}
			}
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy::DEMOTION_ONLY) {
		/*
			Determine the crucial loser mark.
		*/
		integer crucialLoserMark;
		OTConstraint offendingConstraint;
		integer icons = 1;
		for (; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [my index [icons]];   // order is important, so indirect
			const integer loserMarks = loser -> marks [my index [icons]];
			if (my constraints [my index [icons]]. tiedToTheRight)
				Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
			if (loserMarks < winnerMarks) {
				if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
					Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
				} else {
					// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
				}
			}
			if (loserMarks > winnerMarks)
				break;
		}
		if (icons > my numberOfConstraints) {   // completed the loop?
			if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
				Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"",
						loser -> string.get(), U"\", winner \"", winner -> string.get(), U"\".");
			} else {
				// do nothing
			}
		} else {
			crucialLoserMark = icons;
			/*
				Demote the highest uniquely violated constraint in the loser.
			*/
			offendingConstraint = & my constraints [my index [crucialLoserMark]];
			const double constraintStep = step * offendingConstraint -> plasticity;
			offendingConstraint -> ranking -= constraintStep;
			if (grammarHasChanged)
				*grammarHasChanged = true;
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGHEST_DOWN) {
		bool changed = false;
		integer numberOfUp = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (winnerMarks > loserMarks)
				numberOfUp ++;
		}
		if (numberOfUp > 0) {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / numberOfUp;
				}
			}
			integer winnerMarks = 0, loserMarks = 0;
			integer icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				winnerMarks = winner -> marks [my index [icons]];   // order is important, so indirect
				loserMarks = loser -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
						Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
					} else {
						// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
					}
				}
				if (loserMarks > winnerMarks)
					break;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
					Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"",
						loser -> string.get(), U"\", winner \"", winner -> string.get(), U"\".");
				} else {
					// do nothing
				}
			} else {
				const integer crucialLoserMark = icons;
				/*
					Demote the highest uniquely violated constraint in the loser.
				*/
				const OTConstraint offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
			}
		}
		if (grammarHasChanged)
			*grammarHasChanged = changed;
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGHEST_DOWN_2012) {
		bool changed = false;
		integer numberOfUp = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [icons];
			const integer loserMarks = loser -> marks [icons];
			if (winnerMarks > loserMarks)
				numberOfUp ++;
		}
		if (/*true ||*/ numberOfUp > 0) {
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const OTConstraint constraint = & my constraints [icons];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [icons];
				const integer loserMarks = loser -> marks [icons];
				if (winnerMarks > loserMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) / (numberOfUp + 1);
				}
			}
			integer winnerMarks = 0, loserMarks = 0;
			integer icons = 1;
			for (; icons <= my numberOfConstraints; icons ++) {
				winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
				loserMarks = loser -> marks [my index [icons]];
				if (my constraints [my index [icons]]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
						Melder_throw (U"Demotion-only learning step: Loser wins! Should never happen.");
					} else {
						// do nothing; the whole demotion-only idea does not really apply very well to non-OT decision strategies
					}
				}
				if (loserMarks > winnerMarks)
					break;
			}
			if (icons > my numberOfConstraints) {   // completed the loop?
				if (my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
					Melder_throw (U"(OTGrammar_step:) Loser equals correct candidate: loser \"",
						loser -> string.get(), U"\", winner \"", winner -> string.get(), U"\".");
				} else {
					// do nothing
				}
			} else {
				const integer crucialLoserMark = icons;
				/*
					Demote the highest uniquely violated constraint in the loser.
				*/
				const OTConstraint offendingConstraint = & my constraints [my index [crucialLoserMark]];
				double constraintStep = step * offendingConstraint -> plasticity;
				if (multiplyStepByNumberOfViolations)
					constraintStep *= winnerMarks - loserMarks;
				offendingConstraint -> ranking -= /*numberOfUp **/ constraintStep * (1.0 - offendingConstraint -> ranking * my leak);
			}
		}
		if (grammarHasChanged)
			*grammarHasChanged = changed;
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGH_DOWN) {
		integer numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
			const integer loserMarks = loser -> marks [my index [icons]];
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
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer constraintIndex = my index [icons];
				const OTConstraint constraint = & my constraints [constraintIndex];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
				const integer loserMarks = loser -> marks [constraintIndex];
				if (my constraints [constraintIndex]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 0.0);
				} else if (loserMarks > winnerMarks) {
					if (icons <= lowestDemotableConstraint) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= loserMarks - winnerMarks;
						constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
					}
				}
			}
			if (grammarHasChanged)
				*grammarHasChanged = true;
		}
	} else if (updateRule == kOTGrammar_rerankingStrategy::WEIGHTED_ALL_UP_HIGH_DOWN_2012) {
		integer numberOfDown = 0, numberOfUp = 0, lowestDemotableConstraint = 0;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer winnerMarks = winner -> marks [my index [icons]];   // the order is important, therefore indirect
			const integer loserMarks = loser -> marks [my index [icons]];
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
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				const integer constraintIndex = my index [icons];
				const OTConstraint constraint = & my constraints [constraintIndex];
				double constraintStep = step * constraint -> plasticity;
				const integer winnerMarks = winner -> marks [constraintIndex];   // the order is important, therefore indirect
				const integer loserMarks = loser -> marks [constraintIndex];
				if (my constraints [constraintIndex]. tiedToTheRight)
					Melder_throw (U"Demotion-only learning cannot handle tied constraints.");
				if (loserMarks < winnerMarks) {
					if (multiplyStepByNumberOfViolations)
						constraintStep *= winnerMarks - loserMarks;
					constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak) * numberOfDown / (numberOfUp + 1.0);
				} else if (loserMarks > winnerMarks) {
					if (icons <= lowestDemotableConstraint) {
						if (multiplyStepByNumberOfViolations)
							constraintStep *= loserMarks - winnerMarks;
						constraint -> ranking -= constraintStep * (1.0 - constraint -> ranking * my leak);
					}
				}
			}
			if (grammarHasChanged)
				*grammarHasChanged = true;
		}
	}
}

int OTMulti_learnOne (OTMulti me, conststring32 form1, conststring32 form2,
	enum kOTGrammar_rerankingStrategy updateRule, int direction, double plasticity, double relativePlasticityNoise)
{
	integer iloser = OTMulti_getWinner (me, form1, form2);
	if (direction & OTMulti_LEARN_FORWARD) {
		if (Melder_debug == 47) OTMulti_newDisharmonies (me, 2.0);
		integer iwinner = OTMulti_getWinner (me, form1, U"");
		if (Melder_debug != 47 || ! OTMulti_candidateMatches (me, iwinner, form2, U""))
			OTMulti_modifyRankings (me, iwinner, iloser, updateRule, plasticity, relativePlasticityNoise);
	}
	if (direction & OTMulti_LEARN_BACKWARD) {
		if (Melder_debug == 47) OTMulti_newDisharmonies (me, 2.0);
		integer iwinner = OTMulti_getWinner (me, form2, U"");
		if (Melder_debug != 47 || ! OTMulti_candidateMatches (me, iwinner, form1, U""))
			OTMulti_modifyRankings (me, iwinner, iloser, updateRule, plasticity, relativePlasticityNoise);
	}
	return 1;
}

static autoTable OTMulti_createHistory (OTMulti me, integer storeHistoryEvery, integer numberOfData)
{
	try {
		integer numberOfSamplingPoints = numberOfData / storeHistoryEvery;   // e.g. 0, 20, 40, ...
		autoTable thee = Table_createWithoutColumnNames (1 + numberOfSamplingPoints, 3 + my numberOfConstraints);
		Table_setColumnLabel (thee.get(), 1, U"Datum");
		Table_setColumnLabel (thee.get(), 2, U"Form1");
		Table_setColumnLabel (thee.get(), 3, U"Form2");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			Table_setColumnLabel (thee.get(), 3 + icons, my constraints [icons]. name.get());
		Table_setNumericValue (thee.get(), 1, 1, 0);
		Table_setStringValue (thee.get(), 1, 2, U"(initial)");
		Table_setStringValue (thee.get(), 1, 3, U"(initial)");
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			Table_setNumericValue (thee.get(), 1, 3 + icons, my constraints [icons]. ranking);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": history not created.");
	}
}

static int OTMulti_updateHistory (OTMulti me, Table thee, integer storeHistoryEvery, integer idatum, conststring32 form1, conststring32 form2)
{
	try {
		if (idatum % storeHistoryEvery == 0) {
			integer irow = 1 + idatum / storeHistoryEvery;
			Table_setNumericValue (thee, irow, 1, idatum);
			Table_setStringValue (thee, irow, 2, form1);
			Table_setStringValue (thee, irow, 3, form2);
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
				Table_setNumericValue (thee, irow, 3 + icons, my constraints [icons]. ranking);
		}
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": history not updated.");
	}
}


void OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee, double evaluationNoise, enum kOTGrammar_rerankingStrategy updateRule, int direction,
	double initialPlasticity, integer replicationsPerPlasticity, double plasticityDecrement,
	integer numberOfPlasticities, double relativePlasticityNoise, integer storeHistoryEvery, autoTable *history_out)
{
	integer idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	try {
		double plasticity = initialPlasticity;
		autoMelderMonitor monitor (U"Learning from partial pairs...");
		autoTable history;
		if (storeHistoryEvery)
			history = OTMulti_createHistory (me, storeHistoryEvery, numberOfData);
		for (integer iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
			for (integer ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
				conststring32 form1, form2;
				PairDistribution_peekPair (thee, & form1, & form2);
				++ idatum;
				if (monitor.graphics() && idatum % (numberOfData / 400 + 1) == 0) {
					integer numberOfDrawnConstraints = my numberOfConstraints < 14 ? my numberOfConstraints : 14;
					if (numberOfDrawnConstraints > 0) {
						longdouble sumOfRankings = 0.0;
						for (integer icons = 1; icons <= numberOfDrawnConstraints; icons ++)
							sumOfRankings += my constraints [icons]. ranking;
						double meanRanking = double (sumOfRankings) / numberOfDrawnConstraints;
						Graphics_beginMovieFrame (monitor.graphics(), nullptr);
						Graphics_setWindow (monitor.graphics(), 0.0, numberOfData, meanRanking - 50.0, meanRanking + 50.0);
						for (integer icons = 1; icons <= numberOfDrawnConstraints; icons ++) {
							Graphics_setGrey (monitor.graphics(), (double) icons / numberOfDrawnConstraints);
							Graphics_line (monitor.graphics(),
									idatum, my constraints [icons]. ranking,
									idatum, my constraints [icons]. ranking + 1);
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
					if (history)
						OTMulti_updateHistory (me, history.get(), storeHistoryEvery, idatum, form1, form2);
					throw;
				}
				if (history)
					OTMulti_updateHistory (me, history.get(), storeHistoryEvery, idatum, form1, form2);
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

static integer OTMulti_crucialCell (OTMulti me, integer icand, integer iwinner, integer numberOfOptimalCandidates, conststring32 form1, conststring32 form2)
{
	if (my numberOfCandidates < 2) return 0;   // if there is only one candidate, all cells can be greyed
	if (OTMulti_compareCandidates (me, icand, iwinner) == 0) {   // candidate equally good as winner?
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			integer secondBest = 0;
			for (integer jcand = 1; jcand <= my numberOfCandidates; jcand ++) {
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
		const constINTVEC candidateMarks = my candidates [icand]. marks.get();
		const constINTVEC winnerMarks = my candidates [iwinner]. marks.get();
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer numberOfCandidateMarks = candidateMarks [my index [icons]];
			const integer numberOfWinnerMarks = winnerMarks [my index [icons]];
			if (numberOfCandidateMarks > numberOfWinnerMarks)
				return icons;
		}
	}
	return my numberOfConstraints;   // nothing grey
}

static double OTMulti_constraintWidth (Graphics g, OTConstraint constraint, bool showDisharmony) {
	char32 text [100], *newLine;
	double maximumWidth = ( showDisharmony ? 0.8 * Graphics_textWidth_ps (g, Melder_fixed (constraint -> disharmony, 1), true) : 0.0 ),
		firstWidth, secondWidth;
	str32cpy (text, constraint -> name.get());
	newLine = str32chr (text, U'\n');
	if (newLine) {
		*newLine = U'\0';
		firstWidth = Graphics_textWidth_ps (g, text, true);
		if (firstWidth > maximumWidth)
			maximumWidth = firstWidth;
		secondWidth = Graphics_textWidth_ps (g, newLine + 1, true);
		if (secondWidth > maximumWidth)
			maximumWidth = secondWidth;
		return maximumWidth;
	}
	firstWidth = Graphics_textWidth_ps (g, text, true);
	if (firstWidth > maximumWidth)
		maximumWidth = firstWidth;
	return maximumWidth;
}

void OTMulti_drawTableau (OTMulti me, Graphics g, conststring32 form1, conststring32 form2, bool vertical, bool showDisharmonies) {
	integer winner, winner1 = 0, winner2 = 0;
	double x, y, fontSize = Graphics_inqFontSize (g);
	MelderColour colour = Graphics_inqColour (g);
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
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const OTConstraint constraint = & my constraints [icons];
			const double constraintTextWidth = Graphics_textWidth (g, constraint -> name.get());
			if (constraintTextWidth > headerHeight)
				headerHeight = constraintTextWidth;
		}
		headerHeight += margin * 2;
		headerHeight *= worldAspectRatio;
	} else {
		headerHeight = rowHeight;
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const OTConstraint constraint = & my constraints [icons];
			if (str32chr (constraint -> name.get(), U'\n')) {
				headerHeight += 0.7 * rowHeight;
				break;
			}
		}
	}
	/*
		Compute longest candidate string.
		Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
	*/
	double candWidth = Graphics_textWidth_ps (g, form1, true) + Graphics_textWidth_ps (g, form2, true);
	integer numberOfMatchingCandidates = 0;
	integer numberOfOptimalCandidates = 0;
	integer numberOfOptimalCandidates1 = 0;
	integer numberOfOptimalCandidates2 = 0;
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++) {
		if ((form1 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form1, U"")) ||
		    (form2 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form2, U"")) ||
		    (form1 [0] == U'\0' && form2 [0] == U'\0'))
		{
			const double width = Graphics_textWidth_ps (g, my candidates [icand]. string.get(), true);
			if (width > candWidth)
				candWidth = width;
			numberOfMatchingCandidates ++;
			if (OTMulti_compareCandidates (me, icand, winner) == 0)
				numberOfOptimalCandidates ++;
			if (winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0)
				numberOfOptimalCandidates1 ++;
			if (winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0)
				numberOfOptimalCandidates2 ++;
		}
	}
	candWidth += fingerWidth * (bidirectional ? 3 : 1) + margin * 3;
	/*
		Compute tableau width.
	*/
	longdouble tableauWidth = candWidth + doubleLineDx;
	if (vertical) {
		tableauWidth += rowHeight * my numberOfConstraints / worldAspectRatio;
	} else {
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const OTConstraint constraint = & my constraints [icons];
			tableauWidth += OTMulti_constraintWidth (g, constraint, showDisharmonies);
		}
		tableauWidth += margin * 2 * my numberOfConstraints;
	}
	/*
		Draw box.
	*/
	x = doubleLineDx;   // left side of tableau
	y = 1.0 - doubleLineDy;
	if (showDisharmonies)
		y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
			y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
	/*
		Draw input.
	*/
	y -= headerHeight;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_text (g, x + 0.5 * candWidth, y + 0.5 * headerHeight,   form1, form2);
	Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
	/*
		Draw constraint names.
	*/
	x += candWidth + doubleLineDx;
	if (vertical)
		Graphics_setTextRotation (g, 90.0);
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		const OTConstraint constraint = & my constraints [my index [icons]];
		const double width = ( vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2 );
		if (str32chr (constraint -> name.get(), U'\n')) {
			char32 *newLine;
			Melder_sprint (text,200, constraint -> name.get());
			newLine = str32chr (text, U'\n');
			*newLine = U'\0';
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, text);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_text (g, x + 0.5 * width, y, newLine + 1);
		} else if (vertical) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_text (g, x + 0.5 * width, y + margin, constraint -> name.get());
		} else {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			Graphics_text (g, x + 0.5 * width, y + 0.5 * headerHeight, constraint -> name.get());
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
	if (vertical)
		Graphics_setTextRotation (g, 0.0);
	/*
		Draw candidates.
	*/
	y -= doubleLineDy;
	for (integer icand = 1; icand <= my numberOfCandidates; icand ++)
		if ((form1 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form1, U"")) ||
		    (form2 [0] != U'\0' && OTMulti_candidateMatches (me, icand, form2, U"")) ||
		    (form1 [0] == U'\0' && form2 [0] == U'\0'))
	{
		const integer crucialCell = OTMulti_crucialCell (me, icand, winner, numberOfOptimalCandidates, form1, form2);
		const bool candidateIsOptimal = ( OTMulti_compareCandidates (me, icand, winner) == 0 );
		const bool candidateIsOptimal1 = ( winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0 );
		const bool candidateIsOptimal2 = ( winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0 );
		/*
			Draw candidate transcription.
		*/
		x = doubleLineDx;
		y -= rowHeight;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x + candWidth - margin, y + descent, my candidates [icand]. string.get());
		if (candidateIsOptimal) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, ( bidirectional ? 1.2 : 1.5 ) * fontSize);
			if (numberOfOptimalCandidates > 1)
				Graphics_setColour (g, Melder_RED);
			Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, bidirectional ? U"\\Vr" : U"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, fontSize);
		}
		if (candidateIsOptimal1) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, 1.5 * fontSize);
			if (numberOfOptimalCandidates1 > 1)
				Graphics_setColour (g, Melder_RED);
			Graphics_text (g, x + margin + fingerWidth, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, U"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, fontSize);
		}
		if (candidateIsOptimal2) {
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_setFontSize (g, 1.5 * fontSize);
			if (numberOfOptimalCandidates2 > 1)
				Graphics_setColour (g, Melder_RED);
			Graphics_setTextRotation (g, 180);
			Graphics_text (g, x + margin + fingerWidth * 2, y + descent - Graphics_dyMMtoWC (g, 0.0) * fontSize / 12.0, U"\\pf");
			Graphics_setTextRotation (g, 0);
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, fontSize);
		}
		Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
		/*
			Draw grey cell backgrounds.
		*/
		if (! bidirectional && my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY) {
			x = candWidth + 2 * doubleLineDx;
			Graphics_setGrey (g, 0.9);
			for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
				integer index = my index [icons];
				OTConstraint constraint = & my constraints [index];
				const double width = ( vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2 );
				if (icons > crucialCell)
					Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
				x += width;
			}
			Graphics_setColour (g, colour);
		}
		/*
			Draw cell marks.
		*/
		x = candWidth + 2 * doubleLineDx;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
			const integer index = my index [icons];
			const OTConstraint constraint = & my constraints [index];
			const double width = ( vertical ? rowHeight / worldAspectRatio : OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2 );
			char32 markString [40];
			markString [0] = U'\0';
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal)
					str32cat (markString, U"\\<-");
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1)
					str32cat (markString, U"\\<-");
				if (candidateIsOptimal && ! candidateIsOptimal2)
					str32cat (markString, U"\\<-");
			}
			/*
				An exclamation mark can be drawn in this cell only if both of the following conditions are met:
				1. the candidate is not optimal;
				2. this is the crucial cell, i.e. the cells after it are drawn in grey.
			*/
			if (! bidirectional && icons == crucialCell && ! candidateIsOptimal &&
			    my decisionStrategy == kOTGrammar_decisionStrategy::OPTIMALITY_THEORY)
			{
				const integer winnerMarks = my candidates [winner]. marks [index];
				if (winnerMarks + 1 > 5) {
					str32cat (markString, Melder_integer (winnerMarks + 1));
				} else {
					for (integer imark = 1; imark <= winnerMarks + 1; imark ++)
						str32cat (markString, U"*");
				}
				for (integer imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
					str32cat (markString, U"+");
				str32cat (markString, U"!");
				if (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1 > 5) {
					str32cat (markString, Melder_integer (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1));
				} else {
					for (integer imark = winnerMarks + 2; imark <= my candidates [icand]. marks [index]; imark ++)
						str32cat (markString, U"*");
				}
			} else {
				if (my candidates [icand]. marks [index] > 5) {
					str32cat (markString, Melder_integer (my candidates [icand]. marks [index]));
				} else {
					for (integer imark = 1; imark <= my candidates [icand]. marks [index]; imark ++)
						str32cat (markString, U"*");
					for (integer imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
						str32cat (markString, U"+");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1)
					str32cat (markString, U"\\->");
				if (candidateIsOptimal && ! candidateIsOptimal2)
					str32cat (markString, U"\\->");
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal)
					str32cat (markString, U"\\->");
			}
			Graphics_text (g, x + 0.5 * width, y + descent, markString);
			Graphics_setColour (g, colour);
			Graphics_line (g, x, y, x, y + rowHeight);
			Graphics_line (g, x, y + rowHeight, x + width, y + rowHeight);
			x += width;
		}
	}
	/*
		Draw box.
	*/
	x = doubleLineDx;   // left side of tableau
	y = 1.0 - doubleLineDy;
	if (showDisharmonies)
		y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
}

void OTMulti_reset (OTMulti me, double ranking) {
	for (integer icons = 1; icons <= my numberOfConstraints; icons ++) {
		const OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTMulti_sort (me);
}

void OTMulti_setRanking (OTMulti me, integer constraint, double ranking, double disharmony) {
	try {
		Melder_require (constraint >= 1,
			U"The constraint number should be positive, not ", constraint, U".");
		Melder_require (constraint <= my numberOfConstraints,
			U"Constraint ", constraint, U" does not exist (there are only ", my numberOfConstraints, U" constraints).");
		my constraints [constraint]. ranking = ranking;
		my constraints [constraint]. disharmony = disharmony;
		OTMulti_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": ranking not set.");
	}
}

void OTMulti_setConstraintPlasticity (OTMulti me, integer constraint, double plasticity) {
	try {
		Melder_require (constraint >= 1,
			U"The constraint number should be positive, not ", constraint, U".");
		Melder_require (constraint <= my numberOfConstraints,
			U"Constraint ", constraint, U" does not exist (there are only ", my numberOfConstraints, U" constraints).");
		my constraints [constraint]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, U": constraint plasticity not set.");
	}
}

void OTMulti_removeConstraint (OTMulti me, conststring32 constraintName) {
	try {
		if (my numberOfConstraints <= 1)
			Melder_throw (me, U": cannot remove last constraint.");
		integer constraintToBeRemoved = OTMulti_getConstraintIndexFromName (me, constraintName);
		if (constraintToBeRemoved == 0)
			Melder_throw (U"No constraint \"", constraintName, U"\".");
		/*
			Remove the constraint while reusing the memory space.
		*/
		my constraints [constraintToBeRemoved]. destroy ();
		my constraints. remove (constraintToBeRemoved);
		my numberOfConstraints -= 1;   // maintain invariant
		Melder_assert (my numberOfConstraints == my constraints.size);
		/*
			Shift tableau rows.
		*/
		for (integer icand = 1; icand <= my numberOfCandidates; icand ++) {
			const OTCandidate candidate = & my candidates [icand];
			candidate -> marks. remove (constraintToBeRemoved);
			candidate -> numberOfConstraints -= 1;   // maintain invariant
			Melder_assert (candidate -> numberOfConstraints == candidate -> marks.size);
		}
		/*
			Rebuild index.
		*/
		my index. resize (my numberOfConstraints);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			my index [icons] = icons;
		OTMulti_sort (me);
	} catch (MelderError) {
		Melder_throw (me, U": constraint not removed.");
	}
}

autostring32 OTMulti_generateOptimalForm (OTMulti me, conststring32 form1, conststring32 form2, double evaluationNoise) {
	try {
		OTMulti_newDisharmonies (me, evaluationNoise);
		integer winner = OTMulti_getWinner (me, form1, form2);
		return Melder_dup (my candidates [winner]. string.get());
	} catch (MelderError) {
		Melder_throw (me, U": optimal form not generated.");
	}
}

autoStrings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings thee, double evaluationNoise) {
	try {
		autoStrings outputs = Thing_new (Strings);
		integer n = thy numberOfStrings;
		outputs -> numberOfStrings = n;
		outputs -> strings = autoSTRVEC (n);
		for (integer i = 1; i <= n; i ++)
			outputs -> strings [i] = OTMulti_generateOptimalForm (me, thy strings [i].get(), U"", evaluationNoise);
		return outputs;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": optimal forms not generated.");
	}
}

autoStrings OTMulti_generateOptimalForms (OTMulti me, conststring32 form1, conststring32 form2, integer numberOfTrials, double evaluationNoise) {
	try {
		autoStrings outputs = Thing_new (Strings);
		outputs -> numberOfStrings = numberOfTrials;
		outputs -> strings = autoSTRVEC (numberOfTrials);
		for (integer i = 1; i <= numberOfTrials; i ++)
			outputs -> strings [i] = OTMulti_generateOptimalForm (me, form1, form2, evaluationNoise);
		return outputs;
	} catch (MelderError) {
		Melder_throw (me, U": optimal forms not generated.");
	}
}

autoDistributions OTMulti_to_Distribution (OTMulti me, conststring32 form1, conststring32 form2,
	integer numberOfTrials, double evaluationNoise)
{
	try {
		integer totalNumberOfOutputs = 0, iout = 0;
		/*
			Count the total number of outputs.
		*/
		for (integer icand = 1; icand <= my numberOfCandidates; icand ++)
			if (OTMulti_candidateMatches (me, icand, form1, form2))
				totalNumberOfOutputs ++;
		/*
			Create the distribution. One row for every output form.
		*/
		autoDistributions thee = Distributions_create (totalNumberOfOutputs, 1);
		autoINTVEC index = raw_INTVEC (my numberOfCandidates);
		/*
			Set the row labels to the output strings.
		*/
		iout = 0;
		for (integer icand = 1; icand <= my numberOfCandidates; icand ++) {
			if (OTMulti_candidateMatches (me, icand, form1, form2)) {
				thy rowLabels [++ iout] = Melder_dup (my candidates [icand]. string.get());
				index [icand] = iout;
			}
		}
		/*
			Compute a number of outputs and store the results.
		*/
		for (integer itrial = 1; itrial <= numberOfTrials; itrial ++) {
			OTMulti_newDisharmonies (me, evaluationNoise);
			integer iwinner = OTMulti_getWinner (me, form1, form2);
			thy data [index [iwinner]] [1] += 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": distribution not computed.");
	}
}

/* End of file OTMulti.cpp */
