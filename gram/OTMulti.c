/* OTMulti.c
 *
 * Copyright (C) 2005-2010 Paul Boersma
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
 * pb 2005/06/11 the very beginning of computational bidirectional multi-level OT
 * pb 2006/05/16 guarded against cells with many violations
 * pb 2006/05/17 draw disharmonies above tableau
 * pb 2007/05/19 decision strategies
 * pb 2007/08/12 wchar_t
 * pb 2007/10/01 leak and constraint plasticity
 * pb 2007/10/01 can write as encoding
 * pb 2007/11/14 drawTableau: corrected direction of arrows for positive satisfactions
 * pb 2008/04/14 OTMulti_getConstraintIndexFromName
 * pb 2009/03/18 modern enums
 * pb 2010/06/05 corrected colours
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

static void classOTMulti_info (I) {
	iam (OTMulti);
	classData -> info (me);
	long numberOfViolations = 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++)
		for (long icons = 1; icons <= my numberOfConstraints; icons ++)
			numberOfViolations += my candidates [icand]. marks [icons];
	MelderInfo_writeLine2 (L"Decision strategy: ", kOTGrammar_decisionStrategy_getText (my decisionStrategy));
	MelderInfo_writeLine2 (L"Number of constraints: ", Melder_integer (my numberOfConstraints));
	MelderInfo_writeLine2 (L"Number of candidates: ", Melder_integer (my numberOfCandidates));
	MelderInfo_writeLine2 (L"Number of violation marks: ", Melder_integer (numberOfViolations));
}

static int writeText (I, MelderFile file) {
	iam (OTMulti);
	MelderFile_write7 (file, L"\n<", kOTGrammar_decisionStrategy_getText (my decisionStrategy),
		L">\n", Melder_double (my leak), L" ! leak\n", Melder_integer (my numberOfConstraints), L" constraints");
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		MelderFile_write1 (file, L"\n\t\"");
		for (const wchar_t *p = & constraint -> name [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write6 (file, L"\" ", Melder_double (constraint -> ranking),
			L" ", Melder_double (constraint -> disharmony), L" ", Melder_double (constraint -> plasticity));
	}
	MelderFile_write3 (file, L"\n\n", Melder_integer (my numberOfCandidates), L" candidates");
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		MelderFile_write1 (file, L"\n\t\"");
		for (const wchar_t *p = & candidate -> string [0]; *p; p ++) {
			if (*p =='\"') MelderFile_writeCharacter (file, '\"');   // Double any quotes within quotes.
			MelderFile_writeCharacter (file, *p);
		}
		MelderFile_write1 (file, L"\"  ");
		for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++) {
			MelderFile_write2 (file, L" ", Melder_integer (candidate -> marks [icons]));
		}
	}
	return 1;
}

void OTMulti_checkIndex (OTMulti me) {
	int icons;
	if (my index) return;
	my index = NUMlvector (1, my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTMulti_sort (me);
}

static int readText (I, MelderReadText text) {
	int localVersion = Thing_version;
	iam (OTMulti);
	if (! inherited (OTMulti) readText (me, text)) return 0;
	if (localVersion >= 1) {
		if ((my decisionStrategy = texgete1 (text, kOTGrammar_decisionStrategy_getValue)) < 0) return Melder_error1 (L"Trying to read decision strategy.");
	}
	if (localVersion >= 2) {
		my leak = texgetr8 (text); iferror return Melder_error1 (L"Trying to read leak.");
	}
	if ((my numberOfConstraints = texgeti4 (text)) < 1) return Melder_error1 (L"No constraints.");
	if (! (my constraints = NUMstructvector (OTConstraint, 1, my numberOfConstraints))) return 0;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if ((constraint -> name = texgetw2 (text)) == NULL) return 0;
		constraint -> ranking = texgetr8 (text);
		constraint -> disharmony = texgetr8 (text);
		if (localVersion < 2) {
			constraint -> plasticity = 1.0;
		} else {
			constraint -> plasticity = texgetr8 (text); iferror return Melder_error3 (L"Trying to read plasticity of constraint ", Melder_integer (icons), L".");;
		}
	}
	if ((my numberOfCandidates = texgeti4 (text)) < 1) return Melder_error1 (L"No candidates.");
	if (! (my candidates = NUMstructvector (OTCandidate, 1, my numberOfCandidates))) return 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		if ((candidate -> string = texgetw2 (text)) == NULL) return 0;
		candidate -> numberOfConstraints = my numberOfConstraints;   /* Redundancy, needed for writing binary. */
		if ((candidate -> marks = NUMivector (1, candidate -> numberOfConstraints)) == NULL) return 0;
		for (long icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			candidate -> marks [icons] = texgeti2 (text);
	}
	OTMulti_checkIndex (me);
	return 1;
}

class_methods (OTMulti, Data)
	us -> version = 2;
	class_method_local (OTMulti, destroy)
	class_method_local (OTMulti, info)
	class_method_local (OTMulti, description)
	class_method_local (OTMulti, copy)
	class_method_local (OTMulti, equal)
	class_method_local (OTMulti, canWriteAsEncoding)
	class_method (writeText)
	class_method (readText)
	class_method_local (OTMulti, writeBinary)
	class_method_local (OTMulti, readBinary)
class_methods_end

long OTMulti_getConstraintIndexFromName (OTMulti me, const wchar_t *name) {
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		if (Melder_wcsequ (my constraints [icons]. name, name))
			return icons;
	}
	return 0;
}

void OTMulti_sort (OTMulti me) {
	long icons, jcons;
	for (icons = 1; icons < my numberOfConstraints; icons ++) {
		OTConstraint ci = & my constraints [my index [icons]];
		double maximum = ci -> disharmony;
		long jmax = icons, dummy;
		for (jcons = icons + 1; jcons <= my numberOfConstraints; jcons ++) {
			OTConstraint cj = & my constraints [my index [jcons]];
			double disharmonyj = cj -> disharmony;
			/*
			 * Sort only by disharmony; tied constraints should not exist.
			 */
			if (disharmonyj > maximum) {
				maximum = disharmonyj;
				jmax = jcons;
			}
		}
		dummy = my index [icons]; my index [icons] = my index [jmax]; my index [jmax] = dummy;   /* Swap. */
	}
}

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, evaluationNoise);
	}
	OTMulti_sort (me);
}

int OTMulti_compareCandidates (OTMulti me, long icand1, long icand2) {
	int *marks1 = my candidates [icand1]. marks;
	int *marks2 = my candidates [icand2]. marks;
	long icons;
	if (my decisionStrategy == kOTGrammar_decisionStrategy_OPTIMALITY_THEORY) {
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfMarks1 = marks1 [my index [icons]];
			int numberOfMarks2 = marks2 [my index [icons]];
			if (numberOfMarks1 < numberOfMarks2) return -1;   /* Candidate 1 is better than candidate 2. */
			if (numberOfMarks1 > numberOfMarks2) return +1;   /* Candidate 2 is better than candidate 1. */
		}
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR ||
		my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY)
	{
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
			disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			if (my constraints [icons]. disharmony > 0.0) {
				disharmony1 += my constraints [icons]. disharmony * marks1 [icons];
				disharmony2 += my constraints [icons]. disharmony * marks2 [icons];
			}
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			disharmony1 += exp (my constraints [icons]. disharmony) * marks1 [icons];
			disharmony2 += exp (my constraints [icons]. disharmony) * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else if (my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG) {
		double disharmony1 = 0.0, disharmony2 = 0.0;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			double constraintDisharmony = my constraints [icons]. disharmony > 1.0 ? my constraints [icons]. disharmony : 1.0;
			disharmony1 += constraintDisharmony * marks1 [icons];
			disharmony2 += constraintDisharmony * marks2 [icons];
		}
		if (disharmony1 < disharmony2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (disharmony1 > disharmony2) return +1;   /* Candidate 2 is better than candidate 1. */
	} else Melder_fatal ("Unimplemented decision strategy.");
	return 0;   /* None of the comparisons found a difference between the two candidates. Hence, they are equally good. */
}

int OTMulti_candidateMatches (OTMulti me, long icand, const wchar_t *form1, const wchar_t *form2) {
	const wchar_t *string = my candidates [icand]. string;
	return (form1 [0] == '\0' || wcsstr (string, form1)) && (form2 [0] == '\0' || wcsstr (string, form2));
}

long OTMulti_getWinner (OTMulti me, const wchar_t *form1, const wchar_t *form2) {
	long icand_best = 0, numberOfBestCandidates = 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (me, icand, form1, form2)) {
			if (icand_best == 0) {
				icand_best = icand;
				numberOfBestCandidates = 1;
			} else {
				int comparison = OTMulti_compareCandidates (me, icand, icand_best);
				if (comparison == -1) {
					icand_best = icand;
					numberOfBestCandidates = 1;
				} else if (comparison == 0) {
					numberOfBestCandidates += 1;
					if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {
						icand_best = icand;
					}
				}
			}
		}
	}
	if (icand_best == 0) error5 (L"The forms ", form1, L" and ", form2, L" do not match any candidate.")
end:
	iferror return Melder_error1 (L"(OTMulti: Get winner (two):) Not performed.");
	return icand_best;
}

static int OTMulti_modifyRankings (OTMulti me, long iwinner, long iloser,
	double plasticity, double relativePlasticityNoise)
{
	OTCandidate winner = & my candidates [iwinner], loser = & my candidates [iloser];
	double step = relativePlasticityNoise == 0.0 ? plasticity :
		NUMrandomGauss (plasticity, relativePlasticityNoise * plasticity);
	bool multiplyStepByNumberOfViolations =
		my decisionStrategy == kOTGrammar_decisionStrategy_HARMONIC_GRAMMAR ||
		my decisionStrategy == kOTGrammar_decisionStrategy_LINEAR_OT ||
		my decisionStrategy == kOTGrammar_decisionStrategy_MAXIMUM_ENTROPY ||
		my decisionStrategy == kOTGrammar_decisionStrategy_POSITIVE_HG;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		double constraintStep = step * constraint -> plasticity;
		int winnerMarks = winner -> marks [icons];
		int loserMarks = loser -> marks [icons];
		if (loserMarks > winnerMarks) {
			if (multiplyStepByNumberOfViolations) constraintStep *= loserMarks - winnerMarks;
			constraint -> ranking -= constraintStep * (1.0 + constraint -> ranking * my leak);
		}
		if (winnerMarks > loserMarks) {
			if (multiplyStepByNumberOfViolations) constraintStep *= winnerMarks - loserMarks;
			constraint -> ranking += constraintStep * (1.0 - constraint -> ranking * my leak);
		}
	}
end:
	iferror return 0;
	return 1;
}

int OTMulti_learnOne (OTMulti me, const wchar_t *form1, const wchar_t *form2,
	int direction, double plasticity, double relativePlasticityNoise)
{
	long iloser;
	iloser = OTMulti_getWinner (me, form1, form2); cherror
	if (direction & OTMulti_LEARN_FORWARD) {
		long iwinner = OTMulti_getWinner (me, form1, L""); cherror
		OTMulti_modifyRankings (me, iwinner, iloser, plasticity, relativePlasticityNoise); cherror
	}
	if (direction & OTMulti_LEARN_BACKWARD) {
		long iwinner = OTMulti_getWinner (me, form2, L""); cherror
		OTMulti_modifyRankings (me, iwinner, iloser, plasticity, relativePlasticityNoise); cherror
	}
end:
	iferror return 0;
	return 1;
}

static Table OTMulti_createHistory (OTMulti me, long storeHistoryEvery, long numberOfData) {
	long numberOfSamplingPoints = numberOfData / storeHistoryEvery;   /* E.g. 0, 20, 40, ... */
	Table thee = Table_createWithoutColumnNames (1 + numberOfSamplingPoints, 3 + my numberOfConstraints); cherror
	Table_setColumnLabel (thee, 1, L"Datum");
	Table_setColumnLabel (thee, 2, L"Form1");
	Table_setColumnLabel (thee, 3, L"Form2");
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		Table_setColumnLabel (thee, 3 + icons, my constraints [icons]. name); cherror
	}
	Table_setNumericValue (thee, 1, 1, 0); cherror
	Table_setStringValue (thee, 1, 2, L"(initial)"); cherror
	Table_setStringValue (thee, 1, 3, L"(initial)"); cherror
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		Table_setNumericValue (thee, 1, 3 + icons, my constraints [icons]. ranking);
	}
end:
	iferror forget (thee);
	return thee;
}

static int OTMulti_updateHistory (OTMulti me, Table thee, long storeHistoryEvery, long idatum, const wchar_t *form1, const wchar_t *form2) {
	if (idatum % storeHistoryEvery == 0) {
		long irow = 1 + idatum / storeHistoryEvery;
		Table_setNumericValue (thee, irow, 1, idatum); cherror
		Table_setStringValue (thee, irow, 2, form1); cherror
		Table_setStringValue (thee, irow, 3, form2); cherror
		for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
			Table_setNumericValue (thee, irow, 3 + icons, my constraints [icons]. ranking);
		}
	}
end:
	iferror return 0;
	return 1;
}


int OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee, double evaluationNoise, int direction,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise, long storeHistoryEvery, Table *history_out)
{
	long idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	double plasticity = initialPlasticity;
	Table history = NULL;
	if (storeHistoryEvery) {
		history = OTMulti_createHistory (me, storeHistoryEvery, numberOfData); cherror
	}
	Graphics graphics = Melder_monitor1 (0.0, L"Learning with full knowledge...");
	if (graphics) {
		Graphics_clearWs (graphics);
	}
	for (long iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
		for (long ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
			wchar_t *form1, *form2;
			if (! PairDistribution_peekPair (thee, & form1, & form2)) goto end;
			++ idatum;
			if (graphics && idatum % (numberOfData / 400 + 1) == 0) {
				long numberOfDrawnConstraints = my numberOfConstraints < 14 ? my numberOfConstraints : 14;
				if (numberOfDrawnConstraints > 0) {
					double sumOfRankings = 0.0;
					for (long icons = 1; icons <= numberOfDrawnConstraints; icons ++) {
						sumOfRankings += my constraints [icons]. ranking;
					}
					double meanRanking = sumOfRankings / numberOfDrawnConstraints;
					Graphics_setWindow (graphics, 0, numberOfData, meanRanking - 50, meanRanking + 50);
					for (long icons = 1; icons <= numberOfDrawnConstraints; icons ++) {
						Graphics_setGrey (graphics, (double) icons / numberOfDrawnConstraints);
						Graphics_line (graphics, idatum, my constraints [icons]. ranking,
							idatum, my constraints [icons]. ranking+1);
					}
					Graphics_flushWs (graphics);   /* Because drawing is faster than progress loop. */
				}
			}
			if (! Melder_monitor8 ((double) idatum / numberOfData,
				L"Processing partial pair ", Melder_integer (idatum), L" out of ", Melder_integer (numberOfData),
					L":\n      ", form1, L"     ", form2))
			{
				Melder_flushError ("Only %ld partial pairs out of %ld were processed.", idatum - 1, numberOfData);
				goto end;
			}
			OTMulti_newDisharmonies (me, evaluationNoise);
			if (! OTMulti_learnOne (me, form1, form2, direction, plasticity, relativePlasticityNoise)) goto end;
			if (history) {
				OTMulti_updateHistory (me, history, storeHistoryEvery, idatum, form1, form2);
			}
		}
		plasticity *= plasticityDecrement;
	}
end:
	Melder_monitor1 (1.0, NULL);
	iferror return Melder_error1 (L"OTMulti did not complete learning from partial pairs.");
	*history_out = history;
	return 1;
}

static long OTMulti_crucialCell (OTMulti me, long icand, long iwinner, long numberOfOptimalCandidates, const wchar_t *form1, const wchar_t *form2) {
	long icons;
	if (my numberOfCandidates < 2) return 0;   /* If there is only one candidate, all cells can be greyed. */
	if (OTMulti_compareCandidates (me, icand, iwinner) == 0) {   /* Candidate equally good as winner? */
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			long jcand, secondBest = 0;
			for (jcand = 1; jcand <= my numberOfCandidates; jcand ++) {
				if (OTMulti_candidateMatches (me, jcand, form1, form2) && OTMulti_compareCandidates (me, jcand, iwinner) != 0) {   /* A non-optimal candidate? */
					if (secondBest == 0) {
						secondBest = jcand;   /* First guess. */
					} else if (OTMulti_compareCandidates (me, jcand, secondBest) < 0) {
						secondBest = jcand;   /* Better guess. */
					}
				}
			}
			if (secondBest == 0) return 0;   /* If all candidates are equally good, all cells can be greyed. */
			return OTMulti_crucialCell (me, secondBest, iwinner, 1, form1, form2);
		}
	} else {
		int *candidateMarks = my candidates [icand]. marks;
		int *winnerMarks = my candidates [iwinner]. marks;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfCandidateMarks = candidateMarks [my index [icons]];
			int numberOfWinnerMarks = winnerMarks [my index [icons]];
			if (numberOfCandidateMarks > numberOfWinnerMarks)
				return icons;
		}
	}
	return my numberOfConstraints;   /* Nothing grey. */
}

static double OTMulti_constraintWidth (Graphics g, OTConstraint constraint, int showDisharmony) {
	wchar_t text [100], *newLine;
	double maximumWidth = showDisharmony ? 0.8 * Graphics_textWidth_ps (g, Melder_fixed (constraint -> disharmony, 1), TRUE) : 0.0,
		firstWidth, secondWidth;
	wcscpy (text, constraint -> name);
	newLine = wcschr (text, '\n');
	if (newLine) {
		*newLine = '\0';
		firstWidth = Graphics_textWidth_ps (g, text, TRUE);
		if (firstWidth > maximumWidth) maximumWidth = firstWidth;
		secondWidth = Graphics_textWidth_ps (g, newLine + 1, TRUE);
		if (secondWidth > maximumWidth) maximumWidth = secondWidth;
		return maximumWidth;
	}
	firstWidth = Graphics_textWidth_ps (g, text, TRUE);
	if (firstWidth > maximumWidth) maximumWidth = firstWidth;
	return maximumWidth;
}

void OTMulti_drawTableau (OTMulti me, Graphics g, const wchar_t *form1, const wchar_t *form2, int showDisharmonies) {
	long winner, winner1 = 0, winner2 = 0, numberOfMatchingCandidates;
	long numberOfOptimalCandidates, numberOfOptimalCandidates1, numberOfOptimalCandidates2;
	double candWidth, margin, fingerWidth, doubleLineDx, doubleLineDy;
	double tableauWidth, rowHeight, headerHeight, descent, x, y, fontSize = Graphics_inqFontSize (g);
	Graphics_Colour colour = Graphics_inqColour (g);
	wchar_t text [200];
	int bidirectional = form1 [0] != '\0' && form2 [0] != '\0';
	winner = OTMulti_getWinner (me, form1, form2);
	if (winner == 0) {
		Melder_clearError ();
		Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_rectangle (g, 0, 1, 0, 1);
		Graphics_text (g, 0.0, 0.5, L"(no matching candidates)");
		return;
	}

	if (bidirectional) {
		winner1 = OTMulti_getWinner (me, form1, L"");
		winner2 = OTMulti_getWinner (me, form2, L"");
	}
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
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if (wcschr (constraint -> name, '\n')) {
			headerHeight += 0.7 * rowHeight;
			break;
		}
	}
	/*
	 * Compute longest candidate string.
	 * Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
	 */
	candWidth = Graphics_textWidth_ps (g, form1, TRUE) + Graphics_textWidth_ps (g, form2, TRUE);
	numberOfMatchingCandidates = 0;
	numberOfOptimalCandidates = numberOfOptimalCandidates1 = numberOfOptimalCandidates2 = 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		if ((form1 [0] != '\0' && OTMulti_candidateMatches (me, icand, form1, L"")) ||
		    (form2 [0] != '\0' && OTMulti_candidateMatches (me, icand, form2, L"")) ||
		    (form1 [0] == '\0' && form2 [0] == '\0'))
		{
			double width = Graphics_textWidth_ps (g, my candidates [icand]. string, TRUE);
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
	tableauWidth = candWidth + doubleLineDx;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		tableauWidth += OTMulti_constraintWidth (g, constraint, showDisharmonies);
	}
	tableauWidth += margin * 2 * my numberOfConstraints;
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	if (showDisharmonies) y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
	/*
	 * Draw input.
	 */
	y -= headerHeight;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_text2 (g, x + 0.5 * candWidth, y + 0.5 * headerHeight, form1, form2);
	Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
	/*
	 * Draw constraint names.
	 */
	x += candWidth + doubleLineDx;
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [my index [icons]];
		double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
		if (wcschr (constraint -> name, '\n')) {
			wchar_t *newLine;
			wcscpy (text, constraint -> name);
			newLine = wcschr (text, '\n');
			*newLine = '\0';
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, text);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_text (g, x + 0.5 * width, y, newLine + 1);
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
	/*
	 * Draw candidates.
	 */
	y -= doubleLineDy;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++)
		if ((form1 [0] != '\0' && OTMulti_candidateMatches (me, icand, form1, L"")) ||
		    (form2 [0] != '\0' && OTMulti_candidateMatches (me, icand, form2, L"")) ||
		    (form1 [0] == '\0' && form2 [0] == '\0'))
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
			Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, bidirectional ? L"\\Vr" : L"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal1) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates1 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_text (g, x + margin + fingerWidth, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, L"\\pf");
			Graphics_setColour (g, colour);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal2) {
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates2 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_setTextRotation (g, 180);
			Graphics_text (g, x + margin + fingerWidth * 2, y + descent - Graphics_dyMMtoWC (g, 0.0) * fontSize / 12.0, L"\\pf");
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
				double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
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
			double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
			wchar_t markString [40];
			markString [0] = '\0';
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal) {
					wcscat (markString, L"\\<-");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1) {
					wcscat (markString, L"\\<-");
				}
				if (candidateIsOptimal && ! candidateIsOptimal2) {
					wcscat (markString, L"\\<-");
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
					wcscat (markString, Melder_integer (winnerMarks + 1));
				} else {
					for (long imark = 1; imark <= winnerMarks + 1; imark ++)
						wcscat (markString, L"*");
				}
				for (long imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
					wcscat (markString, L"+");
				wcscat (markString, L"!");
				if (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1 > 5) {
					wcscat (markString, Melder_integer (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1));
				} else {
					for (long imark = winnerMarks + 2; imark <= my candidates [icand]. marks [index]; imark ++)
						wcscat (markString, L"*");
				}
			} else {
				if (my candidates [icand]. marks [index] > 5) {
					wcscat (markString, Melder_integer (my candidates [icand]. marks [index]));
				} else {
					for (long imark = 1; imark <= my candidates [icand]. marks [index]; imark ++)
						wcscat (markString, L"*");
					for (long imark = my candidates [icand]. marks [index]; imark < 0; imark ++)
						wcscat (markString, L"+");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] > 0) {
				if (candidateIsOptimal && ! candidateIsOptimal1) {
					wcscat (markString, L"\\->");
				}
				if (candidateIsOptimal && ! candidateIsOptimal2) {
					wcscat (markString, L"\\->");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index] < 0) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal) {
					wcscat (markString, L"\\->");
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
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTMulti_sort (me);
}

int OTMulti_setRanking (OTMulti me, long constraint, double ranking, double disharmony) {
	if (constraint < 1 || constraint > my numberOfConstraints)
		return Melder_error ("(OTGrammar_setRanking): No constraint %ld.", constraint);
	my constraints [constraint]. ranking = ranking;
	my constraints [constraint]. disharmony = disharmony;
	OTMulti_sort (me);
	return 1;
}

int OTMulti_setConstraintPlasticity (OTMulti me, long constraint, double plasticity) {
	if (constraint < 1 || constraint > my numberOfConstraints)
		return Melder_error ("(OTMulti_setConstraintPlasticity): No constraint %ld.", constraint);
	my constraints [constraint]. plasticity = plasticity;
	return 1;
}

int OTMulti_removeConstraint (OTMulti me, const wchar_t *constraintName) {
	long removed = 0;

	if (my numberOfConstraints <= 1)
		return Melder_error1 (L"Cannot remove last constraint.");

	/*
	 * Look for the constraint to be removed.
	 */
	for (long icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if (wcsequ (constraint -> name, constraintName)) {
			removed = icons;
			break;
		}
	}
	if (removed == 0)
		return Melder_error3 (L"No constraint \"", constraintName, L"\".");
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
	return 1;
}

int OTMulti_generateOptimalForm (OTMulti me, const wchar_t *form1, const wchar_t *form2, wchar_t *optimalForm, double evaluationNoise) {
	long winner;
	OTMulti_newDisharmonies (me, evaluationNoise);
	winner = OTMulti_getWinner (me, form1, form2);
	if (! winner) error1 (L"No winner")
	wcscpy (optimalForm, my candidates [winner]. string);
end:
	iferror return Melder_error1 (L"(OTMulti_generateOptimalForm:) Not performed.");
	return 1;
}

Strings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings forms, double evaluationNoise) {
	Strings outputs = new (Strings);
	long i, n = forms -> numberOfStrings;
	cherror
	outputs -> numberOfStrings = n;
	outputs -> strings = NUMpvector (1, n); cherror
	for (i = 1; i <= n; i ++) {
		wchar_t output [100];
		OTMulti_generateOptimalForm (me, forms -> strings [i], L"", output, evaluationNoise); cherror
		outputs -> strings [i] = Melder_wcsdup_e (output); cherror
	}
end:
	iferror { forget (outputs); return Melder_errorp ("(OTMulti_Strings_generateOptimalForms:) Not performed."); }
	return outputs;
}

Strings OTMulti_generateOptimalForms (OTMulti me, const wchar_t *form1, const wchar_t *form2, long numberOfTrials, double evaluationNoise) {
	Strings outputs = new (Strings); cherror
	outputs -> numberOfStrings = numberOfTrials;
	outputs -> strings = NUMpvector (1, numberOfTrials); cherror
	for (long i = 1; i <= numberOfTrials; i ++) {
		wchar_t output [100];
		OTMulti_generateOptimalForm (me, form1, form2, output, evaluationNoise); cherror
		outputs -> strings [i] = Melder_wcsdup_e (output); cherror
	}
end:
	iferror return Melder_errorp ("(OTMulti_generateOptimalForms:) Not performed.");
	return outputs;
}

Distributions OTMulti_to_Distribution (OTMulti me, const wchar_t *form1, const wchar_t *form2,
	long numberOfTrials, double evaluationNoise)
{
	Distributions thee = NULL;
	long totalNumberOfOutputs = 0, iout = 0, *index = NULL;
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
	thee = Distributions_create (totalNumberOfOutputs, 1); cherror
	index = NUMlvector (1, my numberOfCandidates); cherror
	/*
	 * Set the row labels to the output strings.
	 */
	iout = 0;
	for (long icand = 1; icand <= my numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (me, icand, form1, form2)) {
			thy rowLabels [++ iout] = Melder_wcsdup_e (my candidates [icand]. string); cherror
			index [icand] = iout;
		}
	}
	/*
	 * Compute a number of outputs and store the results.
	 */
	for (long itrial = 1; itrial <= numberOfTrials; itrial ++) {
		long iwinner;
		OTMulti_newDisharmonies (me, evaluationNoise);
		iwinner = OTMulti_getWinner (me, form1, form2);
		thy data [index [iwinner]] [1] += 1;
	}
end:
	NUMlvector_free (index, 1);
	iferror forget (thee);
	return thee;
}

/* End of file OTMulti.c */
