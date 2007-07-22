/* OTGrammar.c
 *
 * Copyright (C) 1997-2007 Paul Boersma
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
 */

#include "OTGrammar.h"

#include "oo_DESTROY.h"
#include "OTGrammar_def.h"
#include "oo_COPY.h"
#include "OTGrammar_def.h"
#include "oo_EQUAL.h"
#include "OTGrammar_def.h"
#include "oo_WRITE_BINARY.h"
#include "OTGrammar_def.h"
#include "oo_READ_BINARY.h"
#include "OTGrammar_def.h"
#include "oo_DESCRIPTION.h"
#include "OTGrammar_def.h"

static void classOTGrammar_info (I) {
	iam (OTGrammar);
	classData -> info (me);
	long numberOfCandidates = 0, itab, numberOfViolations = 0, icand, icons;
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		numberOfCandidates += my tableaus [itab]. numberOfCandidates;
		for (icand = 1; icand <= my tableaus [itab]. numberOfCandidates; icand ++)
			for (icons = 1; icons <= my numberOfConstraints; icons ++)
				numberOfViolations += my tableaus [itab]. candidates [icand]. marks [icons];
	}
	MelderInfo_writeLine2 ("Decision strategy: ", enumstring (OTGrammar_DECISION_STRATEGY, my decisionStrategy));
	MelderInfo_writeLine2 ("Number of constraints: ", Melder_integer (my numberOfConstraints));
	MelderInfo_writeLine2 ("Number of tableaus: ", Melder_integer (my numberOfTableaus));
	MelderInfo_writeLine2 ("Number of candidates: ", Melder_integer (numberOfCandidates));
	MelderInfo_writeLine2 ("Number of violation marks: ", Melder_integer (numberOfViolations));
}

static int writeText (I, MelderFile file) {
	iam (OTGrammar);
	MelderFile_write5 (file, L"\n<", Melder_peekAsciiToWcs (enumstring (OTGrammar_DECISION_STRATEGY, my decisionStrategy)),
		L">\n", Melder_integerW (my numberOfConstraints), L" constraints");
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		MelderFile_write3 (file, L"\nconstraint [", Melder_integerW (icons), L"]: \"");
		for (const char *p = & constraint -> name [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write5 (file, L"\" ", Melder_doubleW (constraint -> ranking), L" ", Melder_doubleW (constraint -> disharmony), L" ! ");
		for (const char *p = & constraint -> name [0]; *p; p ++) {
			if (*p == '\n') MelderFile_writeCharacter (file, ' ');
			else if (*p == '\\' && p [1] == 's' && p [2] == '{') p += 2;
			else if (*p == '}') { }
			else MelderFile_writeCharacter (file, *p);
		}
	}
	MelderFile_write3 (file, L"\n\n", Melder_integerW (my numberOfFixedRankings), L" fixed rankings");
	for (long irank = 1; irank <= my numberOfFixedRankings; irank ++) {
		OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
		MelderFile_write4 (file, L"\n   ", Melder_integerW (fixedRanking -> higher), L" ", Melder_integerW (fixedRanking -> lower));
	}
	MelderFile_write3 (file, L"\n\n", Melder_integerW (my numberOfTableaus), L" tableaus");
	for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		MelderFile_write3 (file, L"\ninput [", Melder_integerW (itab), L"]: \"");
		for (const char *p = & tableau -> input [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write2 (file, L"\" ", Melder_integerW (tableau -> numberOfCandidates));
		for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			MelderFile_write3 (file, L"\n   candidate [", Melder_integerW (icand), L"]: \"");
			for (const char *p = & candidate -> output [0]; *p; p ++) {
				if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
				MelderFile_writeCharacter (file, *p);
			}
			MelderFile_writeCharacter (file, '\"');
			for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
				MelderFile_write2 (file, L" ", Melder_integerW (candidate -> marks [icons]));
			}
		}
	}
	return 1;
}

void OTGrammar_checkIndex (OTGrammar me) {
	int icons;
	if (my index) return;
	my index = NUMlvector (1, my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTGrammar_sort (me);
}

static int readText (I, MelderReadString *text) {
	int localVersion = Thing_version;
	iam (OTGrammar);
	long icons, irank, itab, icand;
	if (! inherited (OTGrammar) readText (me, text)) return 0;
	if (localVersion >= 1) {
		if ((my decisionStrategy = texgete1 (text, & enum_OTGrammar_DECISION_STRATEGY)) < 0) return 0;
	}
	if ((my numberOfConstraints = texgeti4 (text)) < 1) return Melder_error ("No constraints.");
	if (! (my constraints = NUMstructvector (OTGrammarConstraint, 1, my numberOfConstraints))) return 0;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		if (! (constraint -> name = texgets2 (text))) return 0;
		constraint -> ranking = texgetr8 (text);
		constraint -> disharmony = texgetr8 (text);
	}
	if ((my numberOfFixedRankings = texgeti4 (text)) >= 1) {
		if (! (my fixedRankings = NUMstructvector (OTGrammarFixedRanking, 1, my numberOfFixedRankings))) return 0;
		for (irank = 1; irank <= my numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
			fixedRanking -> higher = texgeti4 (text);
			fixedRanking -> lower = texgeti4 (text);
		}
	}
	if ((my numberOfTableaus = texgeti4 (text)) < 1) return Melder_error ("No tableaus.");
	if (! (my tableaus = NUMstructvector (OTGrammarTableau, 1, my numberOfTableaus))) return 0;
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		if (! (tableau -> input = texgets2 (text))) return 0;
		if ((tableau -> numberOfCandidates = texgeti4 (text)) < 1) return Melder_error ("No candidates in tableau %ld.", itab);
		if (! (tableau -> candidates = NUMstructvector (OTGrammarCandidate, 1, tableau -> numberOfCandidates))) return 0;
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			if (! (candidate -> output = texgets2 (text))) return 0;
			candidate -> numberOfConstraints = my numberOfConstraints;   /* Redundancy, needed for writing binary. */
			if (! (candidate -> marks = NUMivector (1, candidate -> numberOfConstraints))) return 0;
			for (icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
				candidate -> marks [icons] = texgeti2 (text);
		}
	}
	OTGrammar_checkIndex (me);
	return 1;
}

class_methods (OTGrammar, Data)
	us -> version = 1;
	class_method_local (OTGrammar, destroy)
	class_method_local (OTGrammar, info)
	class_method_local (OTGrammar, description)
	class_method_local (OTGrammar, copy)
	class_method_local (OTGrammar, equal)
	class_method (writeText)
	class_method (readText)
	class_method_local (OTGrammar, writeBinary)
	class_method_local (OTGrammar, readBinary)
class_methods_end

#include "enum_c.h"
#include "OTGrammar_enums.h"

static void classOTHistory_info (I) {
	iam (OTHistory);
	inherited (OTHistory) info (me);
}

class_methods (OTHistory, TableOfReal)
	class_method_local (OTHistory, info)
class_methods_end

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
	return strcmp (my constraints [icons]. name, my constraints [jcons]. name);
}

void OTGrammar_sort (OTGrammar me) {
	long icons;
	constraintCompare_grammar = me;
	qsort (& my index [1], my numberOfConstraints, sizeof (long), constraintCompare);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		constraint -> tiedToTheLeft = icons > 1 &&
			my constraints [my index [icons - 1]]. disharmony == constraint -> disharmony;
		constraint -> tiedToTheRight = icons < my numberOfConstraints &&
			my constraints [my index [icons + 1]]. disharmony == constraint -> disharmony;
	}
}

void OTGrammar_newDisharmonies (OTGrammar me, double spreading) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, spreading)
			/*NUMrandomUniform (-spreading, spreading)*/;
	}
	OTGrammar_sort (me);
}

long OTGrammar_getTableau (OTGrammar me, const char *input) {
	long i, n = my numberOfTableaus;
	for (i = 1; i <= n; i ++) if (strequ (my tableaus [i]. input, input)) return i;
	return Melder_error ("Input \"%s\" not in list of tableaus.", input);
}

static void _OTGrammar_fillInHarmonies (OTGrammar me, long itab) {
	if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, OptimalityTheory)) return;
	OTGrammarTableau tableau = & my tableaus [itab];
	for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		OTGrammarCandidate candidate = & tableau -> candidates [icand];
		int *marks = candidate -> marks;
		double disharmony = 0.0;
		if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, HarmonicGrammar) ||
			my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, MaximumEntropy))
		{
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				disharmony += my constraints [icons]. disharmony * marks [icons];
			}
		} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, ExponentialHG)) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				disharmony += exp (my constraints [icons]. disharmony) * marks [icons];
			}
		} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, LinearOT)) {
			for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
				if (my constraints [icons]. disharmony > 0.0) {
					disharmony += my constraints [icons]. disharmony * marks [icons];
				}
			}
		} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, PositiveHG)) {
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
	long icons;
	if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, OptimalityTheory)) {
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
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
	} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, HarmonicGrammar) ||
		my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, MaximumEntropy))
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
			disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, LinearOT)) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my constraints [icons]. disharmony > 0.0) {
				disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
				disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
			}
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, ExponentialHG)) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += exp (my constraints [icons]. disharmony) * marks1 [icons];
			disharmony2 += exp (my constraints [icons]. disharmony) * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, PositiveHG)) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
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
	if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, MaximumEntropy)) {
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
				icand_best = icand;   /* The current candidate is the best candidate found so far. */
				numberOfBestCandidates = 1;
			} else if (comparison == 0) {
				numberOfBestCandidates += 1;   /* The current candidate is equally good as the best found before. */
				/*
				 * Give all candidates that are equally good an equal chance to become the winner.
				 */
				if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {
					icand_best = icand;
				}
			}
		}
	}
	return icand_best;
}

long OTGrammar_getNumberOfOptimalCandidates (OTGrammar me, long itab) {
	if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, MaximumEntropy)) return 1;
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

int OTGrammar_isCandidateGrammatical (OTGrammar me, long itab, long icand) {
	long jcand;
	for (jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++) {
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) < 0)
			return FALSE;
	}
	return TRUE;
}

int OTGrammar_isCandidateSinglyGrammatical (OTGrammar me, long itab, long icand) {
	long jcand;
	for (jcand = 1; jcand <= my tableaus [itab]. numberOfCandidates; jcand ++) {
		if (jcand != icand && OTGrammar_compareCandidates (me, itab, jcand, itab, icand) <= 0)
			return FALSE;
	}
	return TRUE;
}

int OTGrammar_getInterpretiveParse (OTGrammar me, const char *partialOutput, long *bestTableau, long *bestCandidate) {
	long itab_best = 0, icand_best = 0, itab, icand, numberOfBestCandidates = 0;
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (strstr (tableau -> candidates [icand]. output, partialOutput)) {   /* T&S idea of surface->overt mapping */
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
						if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {
							itab_best = itab;
							icand_best = icand;
						}
					}
				}
			}
		}
	}
	if (itab_best == 0) {
		Melder_error ("The partial output \"%s\" does not match any candidate for any input form.", partialOutput);
		goto end;
	}
end:
	if (bestTableau != NULL) *bestTableau = itab_best;
	if (bestCandidate != NULL) *bestCandidate = icand_best;
	iferror return Melder_error ("(OTGrammar: Get interpretive parse:) Not performed.");
	return 1;
}

int OTGrammar_isPartialOutputGrammatical (OTGrammar me, const char *partialOutput) {
	long itab, icand;
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (strstr (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int OTGrammar_isPartialOutputSinglyGrammatical (OTGrammar me, const char *partialOutput) {
	long itab, icand, jcand, found = FALSE;
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (strstr (tableau -> candidates [icand]. output, partialOutput)) {
				if (OTGrammar_isCandidateGrammatical (me, itab, icand)) {
					found = TRUE;
					/*
					 * All other grammatical candidates should match.
					 */
					for (jcand = 1; jcand <= tableau -> numberOfCandidates; jcand ++) {
						if (OTGrammar_compareCandidates (me, itab, jcand, itab, icand) == 0) {
							if (strstr (tableau -> candidates [jcand]. output, partialOutput) == NULL) {
								return FALSE;   /* Partial output is multiply optimal. */
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
	if (my decisionStrategy != enumi (OTGrammar_DECISION_STRATEGY, OptimalityTheory)) return my numberOfConstraints;   /* Nothing grey. */
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

static double OTGrammar_constraintWidth (Graphics g, const char *name) {
	char text [100], *newLine;
	strcpy (text, name);
	newLine = strchr (text, '\n');
	if (newLine) {
		double firstWidth, secondWidth;
		*newLine = '\0';
		firstWidth = Graphics_textWidth (g, text);
		secondWidth = Graphics_textWidth (g, newLine + 1);
		return firstWidth > secondWidth ? firstWidth : secondWidth;
	}
	return Graphics_textWidth (g, text);
}

void OTGrammar_drawTableau (OTGrammar me, Graphics g, const char *input) {
	long itab, winner, icons, icand, numberOfOptimalCandidates, imark;
	OTGrammarTableau tableau;
	double candWidth, margin, fingerWidth, doubleLineDx, doubleLineDy;
	double tableauWidth, rowHeight, headerHeight, descent, x, y, fontSize = Graphics_inqFontSize (g);
	char text [200];
	itab = OTGrammar_getTableau (me, input);
	if (! itab) { Melder_flushError ("This grammar accepts no input \"%s\".", input); return; }
	_OTGrammar_fillInHarmonies (me, itab);
	winner = OTGrammar_getWinner (me, itab);
	
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	margin = Graphics_dxMMtoWC (g, 1.0);
	fingerWidth = Graphics_dxMMtoWC (g, 7.0) * fontSize / 12.0;
	doubleLineDx = Graphics_dxMMtoWC (g, 0.9);
	doubleLineDy = Graphics_dyMMtoWC (g, 0.9);
	rowHeight = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72);
	descent = rowHeight * 0.5;
	/*
	 * Compute height of header row.
	 */
	headerHeight = rowHeight;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		if (strchr (constraint -> name, '\n')) {
			headerHeight *= 1.6;
			break;
		}
	}
	/*
	 * Compute longest candidate string.
	 * Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
	 */
	candWidth = Graphics_textWidth (g, input);
	tableau = & my tableaus [itab];
	numberOfOptimalCandidates = 0;
	for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
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
	tableauWidth = candWidth + doubleLineDx;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		tableauWidth += OTGrammar_constraintWidth (g, constraint -> name);
	}
	tableauWidth += margin * 2 * my numberOfConstraints;
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
	/*
	 * Draw input.
	 */
	y -= headerHeight;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_printf (g, x + 0.5 * candWidth, y + 0.5 * headerHeight, "%s", input);
	Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
	/*
	 * Draw constraint names.
	 */
	x += candWidth + doubleLineDx;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		double width = OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
		if (strchr (constraint -> name, '\n')) {
			char *newLine;
			strcpy (text, constraint -> name);
			newLine = strchr (text, '\n');
			*newLine = '\0';
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, text);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_text (g, x + 0.5 * width, y, newLine + 1);
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
	/*
	 * Draw candidates.
	 */
	y -= doubleLineDy;
	for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
		long crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
		int candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
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
			Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 1.0) * fontSize / 12.0, "\\pf");
			Graphics_setColour (g, Graphics_BLACK);
			Graphics_setFontSize (g, (int) fontSize);
		}
		Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
		/*
		 * Draw grey cell backgrounds.
		 */
		x = candWidth + 2 * doubleLineDx;
		Graphics_setGrey (g, 0.9);
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int index = my index [icons];
			OTGrammarConstraint constraint = & my constraints [index];
			double width = OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
			if (icons > crucialCell)
				Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
			x += width;
		}
		Graphics_setGrey (g, 0.0);
		/*
		 * Draw cell marks.
		 */
		x = candWidth + 2 * doubleLineDx;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int index = my index [icons];
			OTGrammarConstraint constraint = & my constraints [index];
			double width = OTGrammar_constraintWidth (g, constraint -> name) + margin * 2;
			char markString [40];
			markString [0] = '\0';
			if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, OptimalityTheory)) {
				/*
				 * An exclamation mark can be drawn in this cell only if all of the following conditions are met:
				 * 1. the candidate is not optimal;
				 * 2. the constraint is not tied;
				 * 3. this is the crucial cell, i.e. the cells after it are drawn in grey.
				 */
				if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
					int winnerMarks = tableau -> candidates [winner]. marks [index];
					for (imark = 1; imark <= winnerMarks + 1; imark ++)
						strcat (markString, "*");
					strcat (markString, "!");
					for (imark = winnerMarks + 2; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
						strcat (markString, "*");
				} else {
					if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
					    crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
					{
						Graphics_setColour (g, Graphics_RED);
					}
					for (imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
						strcat (markString, "*");
				}
			} else {
				for (imark = 1; imark <= tableau -> candidates [icand]. marks [index]; imark ++)
					strcat (markString, "*");
			}
			Graphics_text (g, x + 0.5 * width, y + descent, markString);
			Graphics_setColour (g, Graphics_BLACK);
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
		if (my decisionStrategy != enumi (OTGrammar_DECISION_STRATEGY, OptimalityTheory)) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			double value = tableau -> candidates [icand]. harmony;
			if (my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, ExponentialHG)) {
				value = value > -1e-300 ? 1000 : value < -1e300 ? -1000 : - log (- value);
			}
			Graphics_printf (g, x, y + descent, "%s", Melder_fixedA (value, 3));
		}
	}
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - tableau -> numberOfCandidates * rowHeight - doubleLineDy, y);
}

Strings OTGrammar_generateInputs (OTGrammar me, long numberOfTrials) {
	long i;
	Strings thee = new (Strings); cherror
	thy strings = NUMpvector (1, thy numberOfStrings = numberOfTrials); cherror
	for (i = 1; i <= numberOfTrials; i ++) {
		long itab = NUMrandomInteger (1, my numberOfTableaus);
		thy strings [i] = Melder_strdup (my tableaus [itab]. input); cherror
	}
end:	iferror forget (thee);
	return thee;
}

Strings OTGrammar_getInputs (OTGrammar me) {
	long i;
	Strings thee = new (Strings); cherror
	thy strings = NUMpvector (1, thy numberOfStrings = my numberOfTableaus); cherror
	for (i = 1; i <= my numberOfTableaus; i ++) {
		thy strings [i] = Melder_strdup (my tableaus [i]. input); cherror
	}
end:	iferror forget (thee);
	return thee;
}

int OTGrammar_inputToOutput (OTGrammar me, const char *input, char *output, double rankingSpreading) {
	long itab, winner;
	OTGrammar_newDisharmonies (me, rankingSpreading);
	itab = OTGrammar_getTableau (me, input); cherror
	winner = OTGrammar_getWinner (me, itab);
	if (! winner) { Melder_error ("No winner"); goto end; }
	strcpy (output, my tableaus [itab]. candidates [winner]. output);
end:
	iferror return Melder_error ("(OTGrammar_inputToOutput:) Not performed.");
	return 1;
}

Strings OTGrammar_inputsToOutputs (OTGrammar me, Strings inputs, double rankingSpreading) {
	Strings outputs = new (Strings);
	long i, n = inputs -> numberOfStrings;
	cherror
	outputs -> numberOfStrings = n;
	outputs -> strings = NUMpvector (1, n); cherror
	for (i = 1; i <= n; i ++) {
		char output [100];
		OTGrammar_inputToOutput (me, inputs -> strings [i], output, rankingSpreading); cherror
		outputs -> strings [i] = Melder_strdup (output); cherror
	}
end:
	iferror { forget (outputs); return Melder_errorp ("(OTGrammar_inputsToOutputs:) Not performed."); }
	return outputs;
}

Strings OTGrammar_inputToOutputs (OTGrammar me, const char *input, long n, double rankingSpreading) {
	Strings outputs = new (Strings);
	long i;
	cherror
	outputs -> numberOfStrings = n;
	outputs -> strings = NUMpvector (1, n); cherror
	for (i = 1; i <= n; i ++) {
		char output [100];
		OTGrammar_inputToOutput (me, input, output, rankingSpreading); cherror
		outputs -> strings [i] = Melder_strdup (output); cherror
	}
end:
	iferror return Melder_errorp ("(OTGrammar_inputToOutputs:) Not performed.");
	return outputs;
}

Distributions OTGrammar_to_Distribution (OTGrammar me, long trialsPerInput, double noise) {
	Distributions thee;
	long totalNumberOfOutputs = 0, nout = 0, itab, icand, itrial;
	/*
	 * Count the total number of outputs.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++)
		totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
	/*
	 * Create the distribution. One row for every output form.
	 */
	if ((thee = Distributions_create (totalNumberOfOutputs, 1)) == NULL) return NULL;
	/*
	 * Measure every input form.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		if (! Melder_progress ((itab - 0.5) / my numberOfTableaus, "Measuring input \"%s\"", tableau -> input))
			{ forget (thee); return NULL; }
		/*
		 * Set the row labels to the output strings.
		 */
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			char rowTitle [1000];
			sprintf (rowTitle, "%s \\-> %s", tableau -> input, tableau -> candidates [icand]. output);
			thy rowLabels [nout + icand] = Melder_strdup (rowTitle);
		}
		/*
		 * Compute a number of outputs and store the results.
		 */
		for (itrial = 1; itrial <= trialsPerInput; itrial ++) {
			long iwinner;
			OTGrammar_newDisharmonies (me, noise);
			iwinner = OTGrammar_getWinner (me, itab);
			thy data [nout + iwinner] [1] += 1;
		}
		/*
		 * Update the offset.
		 */
		nout += tableau -> numberOfCandidates;
	}
	Melder_progress (1.0, NULL);
	return thee;
}

PairDistribution OTGrammar_to_PairDistribution (OTGrammar me, long trialsPerInput, double noise) {
	PairDistribution thee;
	PairProbability *p;
	long totalNumberOfOutputs = 0, nout = 0, itab, icand, itrial;
	/*
	 * Count the total number of outputs.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++)
		totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
	/*
	 * Create the distribution. One row for every output form.
	 */
	if ((thee = PairDistribution_create ()) == NULL) return NULL;
	/*
	 * Measure every input form.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		if (! Melder_progress ((itab - 0.5) / my numberOfTableaus, "Measuring input \"%s\"", tableau -> input))
			{ forget (thee); return NULL; }
		/*
		 * Copy the input and output strings to the target object.
		 */
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			PairDistribution_add (thee, tableau -> input, tableau -> candidates [icand]. output, 0);
		}
		/*
		 * Compute a number of outputs and store the results.
		 */
		p = (PairProbability *) thy pairs -> item;   /* May have changed after PairDistribution_add !!! */
		for (itrial = 1; itrial <= trialsPerInput; itrial ++) {
			long iwinner;
			OTGrammar_newDisharmonies (me, noise);
			iwinner = OTGrammar_getWinner (me, itab);
			p [nout + iwinner] -> weight += 1;
		}
		/*
		 * Update the offset.
		 */
		nout += tableau -> numberOfCandidates;
	}
	Melder_progress (1.0, NULL);
	return thee;
}

static int honoursFixedRankings (OTGrammar me) {
	long i, icons;
	for (i = 1; i <= my numberOfFixedRankings; i ++) {
		long higher = my fixedRankings [i]. higher, lower = my fixedRankings [i]. lower;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my index [icons] == higher) break;   /* Detected higher before lower: OK. */
			if (my index [icons] == lower) return FALSE;
		}
	}
	return TRUE;
}

Distributions OTGrammar_measureTypology (OTGrammar me) {
	Distributions thee;
	long totalNumberOfOutputs = 0, nout = 0, itab, icand, ncons = my numberOfConstraints, icons, nperm, iperm, factorial [1+12];
	if (ncons > 12)
		return Melder_errorp ("(OTGrammar_measureTypology:) Cannot handle more than 12 constraints.");
	factorial [0] = 1;
	for (icons = 1; icons <= ncons; icons ++) {
		factorial [icons] = factorial [icons - 1] * icons;
	}
	nperm = factorial [ncons];
	/*
	 * Count the total number of outputs.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++)
		totalNumberOfOutputs += my tableaus [itab]. numberOfCandidates;
	/*
	 * Create the distribution. One row for every output form.
	 */
	if ((thee = Distributions_create (totalNumberOfOutputs, 1)) == NULL) return NULL;
	/*
	 * Measure every input form.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		if (! Melder_progress ((itab - 0.5) / my numberOfTableaus, "Measuring input \"%s\"", tableau -> input))
			{ forget (thee); return NULL; }
		/*
		 * Set the row labels to the output strings.
		 */
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			char rowTitle [1000];
			sprintf (rowTitle, "%s \\-> %s", tableau -> input, tableau -> candidates [icand]. output);
			thy rowLabels [nout + icand] = Melder_strdup (rowTitle);
		}
		/*
		 * Compute a number of outputs and store the results.
		 */
		for (iperm = 0; iperm < nperm; iperm ++) {
			long permleft = iperm, iwinner;
			/* Initialize to 12345 before permuting. */
			for (icons = 1; icons <= ncons; icons ++) {
				my index [icons] = icons;
			}
			for (icons = 1; icons < ncons; icons ++) {
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
	Melder_progress (1.0, NULL);
	return thee;
}

static double learningStep (double mean, double relativeSpreading) {
	return relativeSpreading == 0.0 ? mean : NUMrandomGauss (mean, relativeSpreading * mean);
}

static void OTGrammar_honourLocalRankings (OTGrammar me, double meanLearningStep, double relativeStdevLearningStep, int *grammarHasChanged) {
	int improved;
	do {
		long irank;
		improved = FALSE;
		for (irank = 1; irank <= my numberOfFixedRankings; irank ++) {
			OTGrammarFixedRanking fixedRanking = & my fixedRankings [irank];
			OTGrammarConstraint higher = & my constraints [fixedRanking -> higher], lower = & my constraints [fixedRanking -> lower];
			while (higher -> ranking <= lower -> ranking) {
				lower -> ranking -= learningStep (meanLearningStep, relativeStdevLearningStep);
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
				improved = TRUE;
			}
		}
	} while (improved);
}

static int OTGrammar_modifyRankings (OTGrammar me, long itab, long iwinner, long iloser,
	int strategy, int honourLocalRankings,
	double meanLearningStep, double relativeStdevLearningStep, int warnIfStalled, int *grammarHasChanged)
{
	OTGrammarTableau tableau = & my tableaus [itab];
	OTGrammarCandidate winner = & tableau -> candidates [iwinner], loser = & tableau -> candidates [iloser];
	double step = learningStep (meanLearningStep, relativeStdevLearningStep);
	bool multiplyStepByNumberOfViolations =
		my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, HarmonicGrammar) ||
		my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, LinearOT) ||
		my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, MaximumEntropy) ||
		my decisionStrategy == enumi (OTGrammar_DECISION_STRATEGY, PositiveHG);
	if (strategy == OTGrammar_SYMMETRIC_ONE) {
		long icons = NUMrandomInteger (1, my numberOfConstraints);
		int winnerMarks = winner -> marks [icons];
		int loserMarks = loser -> marks [icons];
		if (loserMarks > winnerMarks) {
			my constraints [icons]. ranking -=
				multiplyStepByNumberOfViolations ? (loserMarks - winnerMarks) * step : step;
			if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
		}
		if (winnerMarks > loserMarks) {
			my constraints [icons]. ranking +=
				multiplyStepByNumberOfViolations ? (winnerMarks - loserMarks) * step : step;
			if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
		}
	} else if (strategy == OTGrammar_SYMMETRIC_ALL) {
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				my constraints [icons]. ranking -=
					multiplyStepByNumberOfViolations ? (loserMarks - winnerMarks) * step : step;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
			if (winnerMarks > loserMarks) {
				my constraints [icons]. ranking +=
					multiplyStepByNumberOfViolations ? (winnerMarks - loserMarks) * step : step;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
		}
	} else if (strategy == OTGrammar_WEIGHTED_UNCANCELLED) {
		int winningConstraints = 0, losingConstraints = 0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) losingConstraints ++;
			if (winnerMarks > loserMarks) winningConstraints ++;
		}
		if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				my constraints [icons]. ranking -=
					multiplyStepByNumberOfViolations ? (loserMarks - winnerMarks) * step / losingConstraints : step / losingConstraints;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
			if (winnerMarks > loserMarks) {
				my constraints [icons]. ranking +=
					multiplyStepByNumberOfViolations ? (winnerMarks - loserMarks) * step / winningConstraints : step / winningConstraints;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
		}
	} else if (strategy == OTGrammar_WEIGHTED_ALL) {
		int winningConstraints = 0, losingConstraints = 0;
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > 0) losingConstraints ++;
			if (winnerMarks > 0) winningConstraints ++;
		}
		if (winningConstraints != 0) for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > 0) {
				my constraints [icons]. ranking -=
					multiplyStepByNumberOfViolations ? (loserMarks - winnerMarks) * step / losingConstraints : step / losingConstraints;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
			if (winnerMarks > 0) {
				my constraints [icons]. ranking +=
					multiplyStepByNumberOfViolations ? (winnerMarks - loserMarks) * step / winningConstraints : step / winningConstraints;
				if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
			}
		}
	} else if (strategy == OTGrammar_EDCD) {
		/*
		 * Determine the crucial winner mark.
		 */
		double pivotRanking;
		int equivalent = TRUE;
		long icons;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
			int loserMarks = loser -> marks [my index [icons]];
			if (loserMarks < winnerMarks) break;
			if (loserMarks > winnerMarks) equivalent = FALSE;
		}
		if (icons > my numberOfConstraints) {   /* Completed the loop? */
			if (warnIfStalled && ! equivalent)
				Melder_warning ("(OTGrammar_step:) Adult form has strict superset violations! EDCD stalls.\n"
					"Underlying form: %s\nAdult output: %s\nWinner output: %s", tableau -> input, loser -> output, winner -> output);
			goto end;
		}
		/*
		 * Determine the stratum into which some constraints will be demoted.
		 */
		pivotRanking = my constraints [my index [icons]]. ranking;
		/*
		 * Demote all the uniquely violated constraints in the loser
		 * that have rankings not lower than the pivot.
		 */
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [icons];
			int loserMarks = loser -> marks [icons];
			if (loserMarks > winnerMarks) {
				OTGrammarConstraint constraint = & my constraints [icons];
				if (constraint -> ranking >= pivotRanking) {
					constraint -> ranking = pivotRanking - step;
					if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
				}
			}
		}
	} else { Melder_assert (strategy == OTGrammar_DEMOTION_ONLY);
		/*
		 * Determine the crucial loser mark.
		 */
		long crucialLoserMark;
		OTGrammarConstraint offendingConstraint;
		long icons;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int winnerMarks = winner -> marks [my index [icons]];   /* Order is important, so indirect. */
			int loserMarks = loser -> marks [my index [icons]];
			if (my constraints [my index [icons]]. tiedToTheRight)
				{ Melder_error ("Demotion-only learning cannot handle tied constraints."); goto end; }
			if (loserMarks < winnerMarks)
				{ Melder_error ("Demotion-only learning step: Loser wins! Should never happen."); goto end; }
			if (loserMarks > winnerMarks) break;
		}
		if (icons > my numberOfConstraints)   /* Completed the loop? */
			{ Melder_error ("(OTGrammar_step:) Loser equals correct candidate."); goto end; }
		crucialLoserMark = icons;
		/*
		 * Demote the highest uniquely violated constraint in the loser.
		 */
		offendingConstraint = & my constraints [my index [crucialLoserMark]];
		offendingConstraint -> ranking -= step;
		if (grammarHasChanged != NULL) *grammarHasChanged = TRUE;
	}
	if (honourLocalRankings && my numberOfFixedRankings) {
		OTGrammar_honourLocalRankings (me, meanLearningStep, relativeStdevLearningStep, grammarHasChanged);
	}
end:
	iferror return 0;
	return 1;
}

int OTGrammar_learnOne (OTGrammar me, const char *underlyingForm, const char *adultOutput,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double meanLearningStep, double relativeStdevLearningStep, int newDisharmonies, int warnIfStalled, int *grammarHasChanged)
{
	long itab, iwinner, iloser;
	OTGrammarTableau tableau;
	OTGrammarCandidate winner, loser;
	if (newDisharmonies) OTGrammar_newDisharmonies (me, rankingSpreading);
	if (grammarHasChanged != NULL) *grammarHasChanged = FALSE;

	/*
	 * Evaluate the input in the learner's hypothesis.
	 */
	itab = OTGrammar_getTableau (me, underlyingForm); cherror
	tableau = & my tableaus [itab];

	/*
	 * Determine the "winner", i.e. the candidate that wins in the learner's grammar
	 * (Tesar & Smolensky call this the "loser").
	 */
	iwinner = OTGrammar_getWinner (me, itab);
	winner = & tableau -> candidates [iwinner];

	/*
	 * Error-driven: compare the adult winner (the correct candidate) and the learner's winner.
	 */
	if (strequ (winner -> output, adultOutput)) goto end;

	/*
	 * Find (perhaps the learner's interpretation of) the adult winner (the "loser") in the learner's own tableau
	 * (Tesar & Smolensky call this the "winner").
	 */
	for (iloser = 1; iloser <= tableau -> numberOfCandidates; iloser ++) {
		loser = & tableau -> candidates [iloser];
		if (strequ (loser -> output, adultOutput)) break;
	}
	if (iloser > tableau -> numberOfCandidates)
		{ Melder_error ("Cannot generate adult output \"%s\".", adultOutput); goto end; }

	/*
	 * Now we know that the current hypothesis prefers the (wrong) learner's winner over the (correct) adult output.
	 * The grammar will have to change.
	 */
	OTGrammar_modifyRankings (me, itab, iwinner, iloser, strategy, honourLocalRankings,
		meanLearningStep, relativeStdevLearningStep, warnIfStalled, grammarHasChanged); cherror

end:
	iferror return 0;
	return 1;
}

int OTGrammar_learn (OTGrammar me, Strings inputs, Strings outputs,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double meanLearningStep, double relativeStdevLearningStep, long numberOfChews)
{
	long n = inputs -> numberOfStrings, i, ichew;
	if (! inputs) inputs = outputs;
	if (outputs -> numberOfStrings != n)
		{ Melder_error ("Numbers of strings in input and output do not match."); goto end; }
	for (i = 1; i <= n; i ++)
		for (ichew = 1; ichew <= numberOfChews; ichew ++)
			if (! OTGrammar_learnOne (me, inputs -> strings [i], outputs -> strings [i],
				rankingSpreading, strategy, honourLocalRankings,
				meanLearningStep, relativeStdevLearningStep, TRUE, TRUE, NULL)) return 0;
end:
	iferror return Melder_error ("(OTGrammar_learn:) Not completed.");
	return 1;
}

int OTGrammar_PairDistribution_learn (OTGrammar me, PairDistribution thee,
	double evaluationNoise, int strategy, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews)
{
	long iplasticity, ireplication, ichew, idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	double plasticity = initialPlasticity;
	Graphics graphics = Melder_monitor (0.0, "Learning with full knowledge...");
	if (graphics) {
		Graphics_clearWs (graphics);
	}
	for (iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
		for (ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
			char *input, *output;
			if (! PairDistribution_peekPair (thee, & input, & output)) goto end;
			++ idatum;
			if (graphics && idatum % (numberOfData / 400 + 1) == 0) {
				long icons;
				Graphics_setWindow (graphics, 0, numberOfData, 50, 150);
				for (icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
					Graphics_setColour (graphics, icons + 1);
					Graphics_line (graphics, idatum, my constraints [icons]. ranking,
						idatum, my constraints [icons]. ranking+1);
				}
				Graphics_flushWs (graphics);   /* Because drawing is faster than progress loop. */
			}
			if (! Melder_monitor ((double) idatum / numberOfData,
				"Processing input-output pair %ld out of %ld: %s -> %s", idatum, numberOfData, input, output))
			{
				Melder_flushError ("Only %ld input-output pairs out of %ld were processed.", idatum - 1, numberOfData);
				goto end;
			}
			for (ichew = 1; ichew <= numberOfChews; ichew ++) {
				if (! OTGrammar_learnOne (me, input, output,
					evaluationNoise, strategy, honourLocalRankings,
					plasticity, relativePlasticityNoise, TRUE, TRUE, NULL)) goto end;
			}
		}
		plasticity *= plasticityDecrement;
	}
end:
	Melder_monitor (1.0, NULL);
	iferror return Melder_error ("OTGrammar did not complete learning from input-output pairs.");
	return 1;
}

void OTGrammar_reset (OTGrammar me, double ranking) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTGrammar_sort (me);
}

int OTGrammar_setRanking (OTGrammar me, long constraint, double ranking, double disharmony) {
	if (constraint < 1 || constraint > my numberOfConstraints)
		return Melder_error ("(OTGrammar_setRanking): No constraint %ld.", constraint);
	my constraints [constraint]. ranking = ranking;
	my constraints [constraint]. disharmony = disharmony;
	OTGrammar_sort (me);
	return 1;
}

long theSaveNumberOfConstraints, *theSaveIndex;
double *theSaveRankings, *theSaveDisharmonies;
int *theSaveTiedToTheLeft, *theSaveTiedToTheRight;
static int OTGrammar_save (OTGrammar me) {
	long icons;
	if (my numberOfConstraints != theSaveNumberOfConstraints) {
		NUMlvector_free (theSaveIndex, 1); theSaveIndex = NULL;
		NUMdvector_free (theSaveRankings, 1); theSaveRankings = NULL;
		NUMdvector_free (theSaveDisharmonies, 1); theSaveDisharmonies = NULL;
		NUMivector_free (theSaveTiedToTheLeft, 1); theSaveTiedToTheLeft = NULL;
		NUMivector_free (theSaveTiedToTheRight, 1); theSaveTiedToTheRight = NULL;
		theSaveNumberOfConstraints = my numberOfConstraints;
	}
	if (theSaveIndex == NULL) theSaveIndex = NUMlvector (1, my numberOfConstraints);
	if (theSaveRankings == NULL) theSaveRankings = NUMdvector (1, my numberOfConstraints);
	if (theSaveDisharmonies == NULL) theSaveDisharmonies = NUMdvector (1, my numberOfConstraints);
	if (theSaveTiedToTheLeft == NULL) theSaveTiedToTheLeft = NUMivector (1, my numberOfConstraints);
	if (theSaveTiedToTheRight == NULL) theSaveTiedToTheRight = NUMivector (1, my numberOfConstraints);
	cherror
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		theSaveIndex [icons] = my index [icons];
		theSaveRankings [icons] = my constraints [icons]. ranking;
		theSaveDisharmonies [icons] = my constraints [icons]. disharmony;
		theSaveTiedToTheLeft [icons] = my constraints [icons]. tiedToTheLeft;
		theSaveTiedToTheRight [icons] = my constraints [icons]. tiedToTheRight;
	}
end:
	iferror return 0;
	return 1;
}
static void OTGrammar_restore (OTGrammar me) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		my index [icons] = theSaveIndex [icons];
		my constraints [icons]. ranking = theSaveRankings [icons];
		my constraints [icons]. disharmony = theSaveDisharmonies [icons];
		my constraints [icons]. tiedToTheLeft = theSaveTiedToTheLeft [icons];
		my constraints [icons]. tiedToTheRight = theSaveTiedToTheRight [icons];
	}
}

int OTGrammar_learnOneFromPartialOutput (OTGrammar me, const char *partialAdultOutput,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double meanLearningStep, double relativeStdevLearningStep, long numberOfChews, int warnIfStalled)
{
	long ichew, assumedAdultInputTableau, assumedAdultCandidate;
	OTGrammar_newDisharmonies (me, rankingSpreading);

	if (numberOfChews > 1 && strategy == OTGrammar_EDCD) {
		OTGrammar_save (me);
	}
	for (ichew = 1; ichew <= numberOfChews; ichew ++) {
		int grammarHasChanged;
		OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate); cherror
		OTGrammar_learnOne (me,
			my tableaus [assumedAdultInputTableau]. input,
			my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output,
			rankingSpreading, strategy, honourLocalRankings,
			meanLearningStep, relativeStdevLearningStep, FALSE, warnIfStalled, & grammarHasChanged); cherror
		if (! grammarHasChanged) goto end;
	}
	if (numberOfChews > 1 && strategy == OTGrammar_EDCD && ichew > numberOfChews) {
		/*
		 * Is the partial output form grammatical by now?
		 */
		OTGrammarCandidate learnerCandidate;
		OTGrammar_getInterpretiveParse (me, partialAdultOutput, & assumedAdultInputTableau, & assumedAdultCandidate); cherror
		learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
		if (! strequ (learnerCandidate -> output,
		    my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
		{   /* Still ungrammatical? */
			/*
			 * Backtrack as in Tesar & Smolensky 2000:69.
			 */
			OTGrammar_restore (me);
		}
	}
end:
	iferror return 0;
	return 1;
}

static OTHistory OTGrammar_createHistory (OTGrammar me, long storeHistoryEvery, long numberOfData) {
	long numberOfSamplingPoints = numberOfData / storeHistoryEvery, icons;   /* E.g. 0, 20, 40, ... */
	OTHistory thee = new (OTHistory); cherror
	TableOfReal_init (thee, 2 + numberOfSamplingPoints * 2, 1 + my numberOfConstraints); cherror
	TableOfReal_setColumnLabel (thee, 1, "Datum");
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		TableOfReal_setColumnLabel (thee, icons + 1, my constraints [icons]. name); cherror
	}
	TableOfReal_setRowLabel (thee, 1, "Initial state"); cherror
	thy data [1] [1] = 0;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		thy data [1] [icons + 1] = my constraints [icons]. ranking;
	}
end:
	iferror forget (thee);
	return thee;
}

static int OTGrammar_updateHistory (OTGrammar me, OTHistory thee, long storeHistoryEvery, long idatum, const char *input) {
	if (idatum % storeHistoryEvery == 0) {
		long irow = 2 * idatum / storeHistoryEvery, icons;
		TableOfReal_setRowLabel (thee, irow, input); cherror
		thy data [irow] [1] = idatum;
		thy data [irow + 1] [1] = idatum;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			thy data [irow] [icons + 1] = my constraints [icons]. disharmony;
			thy data [irow + 1] [icons + 1] = my constraints [icons]. ranking;
		}
	}
end:
	iferror return 0;
	return 1;
}

static int OTGrammar_finalizeHistory (OTGrammar me, OTHistory thee, long idatum) {
	long icons;
	TableOfReal_setRowLabel (thee, thy numberOfRows, "Final state"); cherror
	thy data [thy numberOfRows] [1] = idatum;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		thy data [thy numberOfRows] [icons + 1] = my constraints [icons]. ranking;
	}
end:
	iferror return 0;
	return 1;
}

int OTGrammar_learnFromPartialOutputs (OTGrammar me, Strings partialOutputs,
	double rankingSpreading, int strategy, int honourLocalRankings,
	double meanLearningStep, double relativeStdevLearningStep, long numberOfChews,
	long storeHistoryEvery, OTHistory *history_out)
{
	long idatum = 0;
	OTHistory history = NULL;
	if (storeHistoryEvery) {
		history = OTGrammar_createHistory (me, storeHistoryEvery, partialOutputs -> numberOfStrings); cherror
	}
	for (idatum = 1; idatum <= partialOutputs -> numberOfStrings; idatum ++) {
		OTGrammar_learnOneFromPartialOutput (me, partialOutputs -> strings [idatum],
			rankingSpreading, strategy, honourLocalRankings,
			meanLearningStep, relativeStdevLearningStep, numberOfChews, FALSE);   /* Delay error check till after we save current state. */
		if (history) {
			OTGrammar_updateHistory (me, history, storeHistoryEvery, idatum, partialOutputs -> strings [idatum]);
		}
		cherror
	}
end:
	*history_out = history;   /* Even (or especially) in case of error, so that we can inspect. */
	iferror return Melder_error ("(OTGrammar_learnFromPartialOutputs:) Not completed.");
	if (history) {
		OTGrammar_finalizeHistory (me, history, partialOutputs -> numberOfStrings);
	}
	return 1;
}

int OTGrammar_Distributions_learnFromPartialOutputs (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, int strategy, int honourLocalRankings,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long numberOfChews,
	long storeHistoryEvery, OTHistory *history_out)
{
	long iplasticity, ireplication, idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	double plasticity = initialPlasticity;
	OTHistory history = NULL;
	Graphics graphics = Melder_monitor (0.0, "Learning with limited knowledge...");
	if (graphics) {
		Graphics_clearWs (graphics);
	}
	if (storeHistoryEvery) {
		history = OTGrammar_createHistory (me, storeHistoryEvery, numberOfData); cherror
	}
	for (iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
		for (ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
			char *partialOutput;
			if (! Distributions_peek (thee, columnNumber, & partialOutput)) goto end;
			++ idatum;
			if (graphics && idatum % (numberOfData / 400 + 1) == 0) {
				long icons;
				Graphics_setWindow (graphics, 0, numberOfData, 50, 150);
				for (icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
					Graphics_setColour (graphics, icons + 1);
					Graphics_line (graphics, idatum, my constraints [icons]. ranking,
						idatum, my constraints [icons]. ranking+1);
				}
				Graphics_flushWs (graphics);   /* Because drawing is faster than progress loop. */
			}
			if (! Melder_monitor ((double) idatum / numberOfData,
				"Processing partial output %ld out of %ld: %s", idatum, numberOfData, partialOutput))
			{
				Melder_flushError ("Only %ld partial outputs out of %ld were processed.", idatum - 1, numberOfData);
				goto end;
			}
			OTGrammar_learnOneFromPartialOutput (me, partialOutput,
				evaluationNoise, strategy, honourLocalRankings,
				plasticity, relativePlasticityNoise, numberOfChews, TRUE);
			if (history) {
				OTGrammar_updateHistory (me, history, storeHistoryEvery, idatum, partialOutput);
			}
			cherror
		}
		plasticity *= plasticityDecrement;
	}
end:
	Melder_monitor (1.0, NULL);
	if (history_out) *history_out = history;   /* Even (or especially) in case of error, so that we can inspect. */
	iferror return Melder_error ("OTGrammar did not complete learning from partial outputs.");
	if (history) {
		OTGrammar_finalizeHistory (me, history, numberOfData);
	}
	return 1;
}

int OTGrammar_PairDistribution_getFractionCorrect (OTGrammar me, PairDistribution thee,
	double evaluationNoise, long numberOfInputs, double *fractionCorrect)
{
	long ireplication, numberOfCorrect = 0, inputTableau;
	for (ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
		char *input, *adultOutput;
		OTGrammarCandidate learnerCandidate;
		PairDistribution_peekPair (thee, & input, & adultOutput); cherror
		OTGrammar_newDisharmonies (me, evaluationNoise);
		inputTableau = OTGrammar_getTableau (me, input); cherror
		learnerCandidate = & my tableaus [inputTableau]. candidates [OTGrammar_getWinner (me, inputTableau)];
		if (strequ (learnerCandidate -> output, adultOutput))
			numberOfCorrect ++;
	}
end:
	iferror return Melder_error ("(OTGrammar_PairDistribution_getFractionCorrect:) Not completed.");
	*fractionCorrect = (double) numberOfCorrect / numberOfInputs;
	return 1;
}

int OTGrammar_Distributions_getFractionCorrect (OTGrammar me, Distributions thee, long columnNumber,
	double evaluationNoise, long numberOfInputs, double *fractionCorrect)
{
	long ireplication, numberOfCorrect = 0, assumedAdultInputTableau, assumedAdultCandidate;
	for (ireplication = 1; ireplication <= numberOfInputs; ireplication ++) {
		char *partialOutput;
		OTGrammarCandidate learnerCandidate;
		Distributions_peek (thee, columnNumber, & partialOutput); cherror
		OTGrammar_newDisharmonies (me, evaluationNoise);
		OTGrammar_getInterpretiveParse (me, partialOutput, & assumedAdultInputTableau, & assumedAdultCandidate); cherror
		learnerCandidate = & my tableaus [assumedAdultInputTableau]. candidates [OTGrammar_getWinner (me, assumedAdultInputTableau)];
		if (strequ (learnerCandidate -> output, my tableaus [assumedAdultInputTableau]. candidates [assumedAdultCandidate]. output))
			numberOfCorrect ++;
	}
end:
	iferror return Melder_error ("(OTGrammar_Distributions_getFractionCorrect:) Not completed.");
	*fractionCorrect = (double) numberOfCorrect / numberOfInputs;
	return 1;
}

int OTGrammar_removeConstraint (OTGrammar me, const char *constraintName) {
	long icons, ifixed, jfixed, itab, icand, removed = 0;

	if (my numberOfConstraints <= 1)
		return Melder_error ("Cannot remove last constraint.");

	/*
	 * Look for the constraint to be removed.
	 */
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		if (strequ (constraint -> name, constraintName)) {
			removed = icons;
			break;
		}
	}
	if (removed == 0)
		return Melder_error ("No constraint \"%s\".", constraintName);
	/*
	 * Remove the constraint while reusing the memory space.
	 */
	my numberOfConstraints -= 1;
	/*
	 * Shift constraints.
	 */
	Melder_free (my constraints [removed]. name);
	for (icons = removed; icons <= my numberOfConstraints; icons ++) {
		my constraints [icons] = my constraints [icons + 1];
	}
	/*
	 * Remove or shift fixed rankings.
	 */
	for (ifixed = my numberOfFixedRankings; ifixed > 0; ifixed --) {
		OTGrammarFixedRanking fixed = & my fixedRankings [ifixed];
		if (fixed -> higher == removed || fixed -> lower == removed) {
			/*
			 * Remove fixed ranking.
			 */
			my numberOfFixedRankings -= 1;
			if (my numberOfFixedRankings == 0) {
				NUMstructvector_free (OTGrammarFixedRanking, my fixedRankings, 1);
			}
			for (jfixed = ifixed; jfixed <= my numberOfFixedRankings; jfixed ++) {
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
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			candidate -> numberOfConstraints -= 1;
			for (icons = removed; icons <= my numberOfConstraints; icons ++) {
				candidate -> marks [icons] = candidate -> marks [icons + 1];
			}
		}
	}
	/*
	 * Rebuild index.
	 */
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTGrammar_sort (me);
	return 1;
}

static int OTGrammarTableau_removeCandidate_unstripped (OTGrammarTableau me, long icand) {
	long jcand;
	Melder_assert (icand >= 1);
	if (icand > my numberOfCandidates) Melder_fatal ("icand %ld, ncand %ld", icand, my numberOfCandidates);
	/*
	 * Free up memory associated with this candidate.
	 */
	Melder_free (my candidates [icand]. output);
	NUMivector_free (my candidates [icand]. marks, 1);
	/*
	 * Remove.
	 */
	my numberOfCandidates -= 1;
	/*
	 * Shift.
	 */
	for (jcand = icand; jcand <= my numberOfCandidates; jcand ++) {
		OTGrammarCandidate candj = & my candidates [jcand];
		OTGrammarCandidate candj1 = & my candidates [jcand + 1];
		candj -> output = candj1 -> output;
		candj -> marks = candj1 -> marks;
	}
	return 1;
}

static int OTGrammarTableau_isHarmonicallyBounded (OTGrammarTableau me, long icand, long jcand) {
	OTGrammarCandidate candi = & my candidates [icand], candj = & my candidates [jcand];
	long icons;
	int equal = TRUE;
	if (icand == jcand) return FALSE;
	for (icons = 1; icons <= candi -> numberOfConstraints; icons ++) {
		if (candi -> marks [icons] < candj -> marks [icons]) return FALSE;
		if (candi -> marks [icons] > candj -> marks [icons]) equal = FALSE;
	}
	return ! equal;
}

static int OTGrammarTableau_candidateIsPossibleWinner (OTGrammar me, long itab, long icand) {
	long icons;
	OTGrammar_save (me);
	OTGrammar_reset (me, 100.0);
	for (;;) {
		int grammarHasChanged;
		double previousStratum = 101.0;
		OTGrammar_learnOne (me, my tableaus [itab]. input, my tableaus [itab]. candidates [icand]. output,
			1e-3, OTGrammar_EDCD, FALSE, 1.0, 0.0, TRUE, TRUE, & grammarHasChanged);
		if (! grammarHasChanged) {
			OTGrammar_restore (me);
			return TRUE;
		}
		OTGrammar_newDisharmonies (me, 0.0);
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			double stratum = my constraints [my index [icons]]. ranking;
			#if 0
			if (stratum < 50.0 - my numberOfConstraints) {
				OTGrammar_restore (me);
				return FALSE;   /* We detected a tumble. */
			}
			#else
			if (stratum < previousStratum) {
				if (stratum < previousStratum - 1.0) {
					OTGrammar_restore (me);
					return FALSE;   /* We detected a vacated stratum. */
				}
				previousStratum = stratum;
			}
			#endif
		}
	}
	return 0;   /* Cannot occur. */
}

int OTGrammar_removeHarmonicallyBoundedCandidates (OTGrammar me, int singly) {
	long itab, icand, jcand;
	/*
	 * First, the candidates that are harmonically bounded by one or more single other candidates have to be removed;
	 * otherwise, EDCD will stall.
	 */
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tab = & my tableaus [itab];
		for (icand = tab -> numberOfCandidates; icand >= 1; icand --) {
			for (jcand = 1; jcand <= tab -> numberOfCandidates; jcand ++) {
				if (OTGrammarTableau_isHarmonicallyBounded (tab, icand, jcand)) {
					OTGrammarTableau_removeCandidate_unstripped (tab, icand);
					break;
				}
			}
		}
		tab -> candidates = (OTGrammarCandidate) realloc (& tab -> candidates [1], sizeof (struct structOTGrammarCandidate) * tab -> numberOfCandidates) - 1;
	}
	if (! singly) {
		for (itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tab = & my tableaus [itab];
			for (icand = tab -> numberOfCandidates; icand >= 1; icand --) {
				if (! OTGrammarTableau_candidateIsPossibleWinner (me, itab, icand)) {
					OTGrammarTableau_removeCandidate_unstripped (tab, icand);
				}
			}
			tab -> candidates = (OTGrammarCandidate) realloc (& tab -> candidates [1], sizeof (struct structOTGrammarCandidate) * tab -> numberOfCandidates) - 1;
		}	
	}
	return 1;
}

#define OTGrammar_List4_members Data_members \
	long hi1, lo1, hi2, lo2;
#define OTGrammar_List4_methods Data_methods
class_create (OTGrammar_List4, Data);

class_methods (OTGrammar_List4, Data)
class_methods_end

int OTGrammar_PairDistribution_listObligatoryRankings (OTGrammar me, PairDistribution thee) {
	OTGrammarFixedRanking savedFixedRankings;
	long savedNumberOfFixedRankings = my numberOfFixedRankings;
	long ifixedRanking, icons, jcons, kcons, lcons, ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
	long ilist, jlist, itrial, iform;
	int **obligatory = NULL, improved;
	double evaluationNoise = 1e-9;
	Ordered list = NULL;
	/*
	 * Save.
	 */
	savedFixedRankings = my fixedRankings;
	OTGrammar_save (me);
	/*
	 * Add room for two more fixed rankings.
	 */
	my fixedRankings = NUMstructvector (OTGrammarFixedRanking, 1, my numberOfFixedRankings + 2);
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
			PairProbability prob = thy pairs -> item [iform];
			if (prob -> weight > 0.0) {
				int grammarHasChanged;
				OTGrammar_learnOne (me, prob -> string1, prob -> string2,
					evaluationNoise, OTGrammar_EDCD, TRUE /* honour fixed rankings; very important */,
					1.0, 0.0, FALSE, TRUE, & grammarHasChanged); cherror
				if (grammarHasChanged) {
					OTGrammar_newDisharmonies (me, evaluationNoise);
				}
				grammarHasChangedDuringCycle |= grammarHasChanged;
			}
		}
		if (! grammarHasChangedDuringCycle) break;
	}
	if (itrial > 40) {
		MelderInfo_writeLine1 ("There are no total rankings that generate these input-output pairs.");
		goto end;
	}
	/*
	 * Test learnability of every possible ranked pair.
	 */
	my numberOfFixedRankings ++;
	obligatory = NUMimatrix (1, my numberOfConstraints, 1, my numberOfConstraints); cherror
	MelderInfo_open ();
	Melder_progress (0.0, "");
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
			my fixedRankings [my numberOfFixedRankings]. higher = icons;
			my fixedRankings [my numberOfFixedRankings]. lower = jcons;
			OTGrammar_reset (me, 100.0);
			Melder_progress ((double) ipair / npair, "%ld/%ld: Trying ranking %s >> %s", ipair + 1, npair,
				my constraints [icons]. name, my constraints [jcons]. name);
			ipair ++;
			for (itrial = 1; itrial <= 40; itrial ++) {
				int grammarHasChangedDuringCycle = FALSE;
				OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
				OTGrammar_newDisharmonies (me, evaluationNoise);
				for (iform = 1; iform <= thy pairs -> size; iform ++) {
					PairProbability prob = thy pairs -> item [iform];
					if (prob -> weight > 0.0) {
						int grammarHasChanged;
						OTGrammar_learnOne (me, prob -> string1, prob -> string2,
							evaluationNoise, OTGrammar_EDCD, TRUE /* honour fixed rankings; very important */,
							1.0, 0.0, FALSE, TRUE, & grammarHasChanged); cherror
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
				MelderInfo_writeLine3 (my constraints [jcons]. name, " >> ", my constraints [icons]. name);
				MelderInfo_close ();
			}
		}
	}
	my numberOfFixedRankings ++;
	Melder_progress (0.0, "");
	npair = npair * npair;
	list = Ordered_create ();
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
					Melder_progress ((double) ipair / npair, "%ld/%ld", ipair + 1, npair);
					ipair ++;
					for (itrial = 1; itrial <= 40; itrial ++) {
						int grammarHasChangedDuringCycle = FALSE;
						OTGrammar_honourLocalRankings (me, 1.0, 0.0, & grammarHasChangedDuringCycle);
						OTGrammar_newDisharmonies (me, evaluationNoise);
						for (iform = 1; iform <= thy pairs -> size; iform ++) {
							PairProbability prob = thy pairs -> item [iform];
							if (prob -> weight > 0.0) {
								int grammarHasChanged;
								OTGrammar_learnOne (me, prob -> string1, prob -> string2,
									evaluationNoise, OTGrammar_EDCD, TRUE /* honour fixed rankings; very important */,
									1.0, 0.0, FALSE, TRUE, & grammarHasChanged); cherror
								if (grammarHasChanged) {
									OTGrammar_newDisharmonies (me, evaluationNoise);
								}
								grammarHasChangedDuringCycle |= grammarHasChanged;
							}
						}
						if (! grammarHasChangedDuringCycle) break;
					}
					if (itrial > 40) {
						OTGrammar_List4 listElement = new (OTGrammar_List4);
						listElement -> hi1 = jcons;
						listElement -> lo1 = icons;
						listElement -> hi2 = lcons;
						listElement -> lo2 = kcons;
						Collection_addItem (list, listElement);
					}
				}
			}
		}
	}
	Melder_progress (1.0, "");
	/*
	 * Improve list.
	 */
	improved = TRUE;
	while (improved) {
		improved = FALSE;
		for (ilist = 1; ilist <= list -> size; ilist ++) {
			for (jlist = 1; jlist <= list -> size; jlist ++) if (ilist != jlist) {
				OTGrammar_List4 elA = list -> item [ilist], elB = list -> item [jlist];
				long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
				long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
				improved |= (ahi1 == bhi1 || obligatory [bhi1] [ahi1]) && (ahi2 == bhi2 || obligatory [bhi2] [ahi2]) &&
					(alo1 == blo1 || obligatory [alo1] [blo1]) && (alo2 == blo2 || obligatory [alo2] [blo2]);
				improved |= (ahi1 == bhi2 || obligatory [bhi2] [ahi1]) && (ahi2 == bhi1 || obligatory [bhi1] [ahi2]) &&
					(alo1 == blo2 || obligatory [alo1] [blo2]) && (alo2 == blo1 || obligatory [alo2] [blo1]);
				if (improved) {
					Collection_removeItem (list, jlist);
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
				OTGrammar_List4 elA = list -> item [ilist], elB = list -> item [jlist];
				long ahi1 = elA -> hi1, alo1 = elA -> lo1, ahi2 = elA -> hi2, alo2 = elA -> lo2;
				long bhi1 = elB -> hi1, blo1 = elB -> lo1, bhi2 = elB -> hi2, blo2 = elB -> lo2;
				improved |= ahi1 == bhi1 && alo1 == blo1 && ahi2 == bhi2 && blo2 == bhi1 && alo2 == alo1;
				improved |= ahi1 == bhi2 && alo1 == blo2 && ahi2 == bhi1 && blo1 == bhi2 && alo2 == alo1;
				improved |= ahi2 == bhi1 && alo2 == blo1 && ahi1 == bhi2 && blo2 == bhi1 && alo1 == alo2;
				improved |= ahi2 == bhi2 && alo2 == blo2 && ahi1 == bhi1 && blo1 == bhi2 && alo1 == alo2;
				if (improved) {
					Collection_removeItem (list, jlist);
					break;
				}
			}
			if (improved) break;
		}
	}
	for (ilist = 1; ilist <= list -> size; ilist ++) {
		OTGrammar_List4 el = list -> item [ilist];
		MelderInfo_write4 (my constraints [el -> hi1]. name, " >> ", my constraints [el -> lo1]. name, " OR ");
		MelderInfo_writeLine3 (my constraints [el -> hi2]. name, " >> ", my constraints [el -> lo2]. name);
		MelderInfo_close ();
	}
end:
	Melder_progress (1.0, "");
	MelderInfo_close ();
	forget (list);
	NUMimatrix_free (obligatory, 1, 1);
	/*
	 * Remove room.
	 */
	my numberOfFixedRankings = savedNumberOfFixedRankings;
	NUMstructvector_free (OTGrammarFixedRanking, my fixedRankings, 1);
	/*
	 * Restore.
	 */
	my fixedRankings = savedFixedRankings;
	OTGrammar_restore (me);
	iferror return 0;
	return 1;
}

int OTGrammar_Distributions_listObligatoryRankings (OTGrammar me, Distributions thee, long columnNumber) {
	OTGrammarFixedRanking savedFixedRankings;
	long ifixedRanking, icons, jcons, kcons, ipair = 0, npair = my numberOfConstraints * (my numberOfConstraints - 1);
	/*
	 * Save.
	 */
	savedFixedRankings = my fixedRankings;
	OTGrammar_save (me);
	/*
	 * Add room for one more fixed ranking.
	 */
	my numberOfFixedRankings ++;
	my fixedRankings = NUMstructvector (OTGrammarFixedRanking, 1, my numberOfFixedRankings);
	for (ifixedRanking = 1; ifixedRanking < my numberOfFixedRankings; ifixedRanking ++) {
		my fixedRankings [ifixedRanking]. higher = savedFixedRankings [ifixedRanking]. higher;
		my fixedRankings [ifixedRanking]. lower = savedFixedRankings [ifixedRanking]. lower;
	}
	/*
	 * Test learnability of every possible ranked pair.
	 */
	MelderInfo_open ();
	Melder_progress (0.0, "");
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		for (jcons = 1; jcons <= my numberOfConstraints; jcons ++) if (icons != jcons) {
			my fixedRankings [my numberOfFixedRankings]. higher = icons;
			my fixedRankings [my numberOfFixedRankings]. lower = jcons;
			OTGrammar_reset (me, 100.0);
			Melder_progress ((double) ipair / npair, "%ld/%ld: Trying ranking %s >> %s", ipair + 1, npair,
				my constraints [icons]. name, my constraints [jcons]. name);
			ipair ++;
			Melder_progressOff ();
			OTGrammar_Distributions_learnFromPartialOutputs (me, thee, columnNumber,
				1e-9, OTGrammar_EDCD, TRUE /* honour fixed rankings; very important */,
				1.0, 1000, 0.0, 1, 0.0, 1, 0, NULL); cherror
			Melder_progressOn ();
			for (kcons = 1; kcons <= my numberOfConstraints; kcons ++) {
				if (my constraints [kcons]. ranking < 0.0) {
					MelderInfo_writeLine3 (my constraints [jcons]. name, " >> ", my constraints [icons]. name);
					break;
				}
			}
		}
	}
end:
	Melder_progress (1.0, "");
	MelderInfo_close ();
	/*
	 * Remove room.
	 */
	my numberOfFixedRankings --;
	NUMstructvector_free (OTGrammarFixedRanking, my fixedRankings, 1);
	/*
	 * Restore.
	 */
	my fixedRankings = savedFixedRankings;
	OTGrammar_restore (me);
	iferror return 0;
	return 1;
}

static void printConstraintNames (OTGrammar me, FILE *f) {
	long icons;
	char text [200];
	int secondLine = FALSE;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		if (strchr (constraint -> name, '\n')) {
			char *newLine;
			strcpy (text, constraint -> name);
			newLine = strchr (text, '\n');
			*newLine = '\0';
			fprintf (f, "\t%s", text);
			secondLine = TRUE;
		} else {
			fprintf (f, "\t%s", constraint -> name);
		}
	}
	fprintf (f, "\n");
	if (secondLine) {
		fprintf (f, "\t");
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			OTGrammarConstraint constraint = & my constraints [my index [icons]];
			char *newLine = strchr (constraint -> name, '\n');
			fprintf (f, "\t%s", newLine ? newLine + 1 : "");
		}
		fprintf (f, "\n");
	}
}

int OTGrammar_writeToHeaderlessSpreadsheetFile (OTGrammar me, MelderFile file) {
	long itab, icons;
	FILE *f = Melder_fopen (file, "w"); cherror
	fprintf (f, "CONSTRAINTS\t");
	printConstraintNames (me, f);
	fprintf (f, "rankings\t");
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		fprintf (f, "\t%f", constraint -> ranking);
	}
	fprintf (f, "\ndisharmonies\t");
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & my constraints [my index [icons]];
		fprintf (f, "\t%f", constraint -> disharmony);
	}
	fprintf (f, "\n");
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		long winner = OTGrammar_getWinner (me, itab), icand, numberOfOptimalCandidates = 0;
		for (icons = 1; icons <= my numberOfConstraints + 1; icons ++) {
			fprintf (f, "\t");
		}
		fprintf (f, "\nINPUT\t%s", tableau -> input);
		printConstraintNames (me, f);
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			if (OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0) {
				numberOfOptimalCandidates ++;
			}
		}
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			OTGrammarCandidate candidate = & tableau -> candidates [icand];
			int candidateIsOptimal = OTGrammar_compareCandidates (me, itab, icand, itab, winner) == 0;
			long crucialCell = OTGrammar_crucialCell (me, itab, icand, winner, numberOfOptimalCandidates);
			fprintf (f, "%s\t%s", candidateIsOptimal == FALSE ? "loser" :
				numberOfOptimalCandidates > 1 ? "co-winner" : "winner", candidate -> output);
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				int index = my index [icons], imark;
				OTGrammarConstraint constraint = & my constraints [index];
				char markString [40];
				markString [0] = '\0';
				/*
				 * An exclamation mark can be drawn in this cell only if all of the following conditions are met:
				 * 1. the candidate is not optimal;
				 * 2. the constraint is not tied;
				 * 3. this is the crucial cell, i.e. the cells after it are drawn in grey.
				 */
				if (icons == crucialCell && ! candidateIsOptimal && ! constraint -> tiedToTheLeft && ! constraint -> tiedToTheRight) {
					int winnerMarks = tableau -> candidates [winner]. marks [index];
					for (imark = 1; imark <= winnerMarks + 1; imark ++)
						strcat (markString, "*");
					strcat (markString, "!");
					for (imark = winnerMarks + 2; imark <= candidate -> marks [index]; imark ++)
						strcat (markString, "*");
				} else {
					if (! candidateIsOptimal && (constraint -> tiedToTheLeft || constraint -> tiedToTheRight) &&
					    crucialCell >= 1 && constraint -> disharmony == my constraints [my index [crucialCell]]. disharmony)
					{
						strcat (markString, "=");
					}
					for (imark = 1; imark <= candidate -> marks [index]; imark ++)
						strcat (markString, "*");
				}
				fprintf (f, "\t%s", markString);
			}
			fprintf (f, "\n");
		}
	}
	Melder_fclose (file, f); cherror
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
end:
	iferror return 0;
	return 1;
}

/* End of file OTGrammar.c */
