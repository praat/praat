/* OTGrammar_ex_metrics.c
 *
 * Copyright (C) 2001-2004 Paul Boersma
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
 * pb 2001/11/25
 * pb 2002/07/16 GPL
 * pb 2003/03/31 removeConstraint
 * pb 2003/05/09 added Peripheral and MainNonfinal
 * pb 2004/01/14 added HeadNonfinal
 * pb 2004/01/25 added overtFormsHaveSecondaryStress
 * pb 2004/07/27 added Clash and Lapse
 * pb 2004/08/11 complete rewrite in order to include WeightByPosition and *MoraicConsonant
 * pb 2004/12/03 corrected *Lapse
 */

#include "OTGrammar.h"

#define WSP  1
#define FtNonfinal  2
#define Iambic  3
#define Parse  4
#define FootBin  5
#define WFL  6
#define WFR  7
#define Main_L  8
#define Main_R  9
#define AFL  10
#define AFR  11
#define Nonfinal  12

#define Trochaic  13
#define FootBimoraic  14
#define FootBisyllabic  15
#define Peripheral  16
#define MainNonfinal  17
#define HeadNonfinal  18
#define Clash  19
#define Lapse  20

#define WeightByPosition  21
#define MoraicConsonant  22

#define NUMBER_OF_CONSTRAINTS  22

static char *constraintNames [1+NUMBER_OF_CONSTRAINTS] = { 0,
	"WSP", "FtNonfinal", "Iambic", "Parse", "FootBin", "WFL", "WFR", "Main-L", "Main-R", "AFL", "AFR", "Nonfinal",
	"Trochaic", "FtBimor", "FtBisyl", "Peripheral", "MainNonfinal", "HeadNonfinal", "*Clash", "*Lapse", "WeightByPosition", "*C\\mu" };

static void addCandidate (OTGrammarTableau me, long numberOfSyllables, int stress [],
	int footedToTheLeft [], int footedToTheRight [], int surfaceWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	static char *syllable [] = { "L", "L1", "L2", "H", "H1", "H2", "K", "K1", "K2", "J", "J1", "J2" };
	static char *syllableWithoutSecondaryStress [] = { "L", "L1", "L", "H", "H1", "H", "K", "K1", "K", "J", "J1", "J" };
	long isyll;
	char output [100];
	strcpy (output, "[");
	for (isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1) strcat (output, " ");
		strcat (output, ( overtFormsHaveSecondaryStress ? syllable : syllableWithoutSecondaryStress )
				[stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
	}
	strcat (output, "] \\-> /");
	for (isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1) strcat (output, " ");
		if (footedToTheRight [isyll] || ! footedToTheLeft [isyll] && stress [isyll] != 0) strcat (output, "(");
		strcat (output, syllable [stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
		if (footedToTheLeft [isyll] || ! footedToTheRight [isyll] && stress [isyll] != 0) strcat (output, ")");
	}
	strcat (output, "/");
	my candidates [++ my numberOfCandidates]. output = Melder_strdup (output); cherror
end:
	return;
}

static void fillSurfaceWeightPattern (OTGrammarTableau me, long numberOfSyllables, int stress [],
	int footedToTheLeft [], int footedToTheRight [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	long isyll;
	int surfaceWeightPattern [1+7], minSurfaceWeight [1+7], maxSurfaceWeight [1+7];
	int weight1, weight2, weight3, weight4, weight5;
	for (isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (underlyingWeightPattern [isyll] < 3) {
			minSurfaceWeight [isyll] = maxSurfaceWeight [isyll] = underlyingWeightPattern [isyll];   /* L -> L; H -> H */
		} else {
			minSurfaceWeight [isyll] = 3, maxSurfaceWeight [isyll] = 4;   /* C -> { J, K } */ 
		}
	}
	surfaceWeightPattern [6] = surfaceWeightPattern [7] = 1;   /* Constant L. */
	for (weight1 = minSurfaceWeight [1]; weight1 <= maxSurfaceWeight [1]; weight1 ++) {
		surfaceWeightPattern [1] = weight1;
		for (weight2 = minSurfaceWeight [2]; weight2 <= maxSurfaceWeight [2]; weight2 ++) {
			surfaceWeightPattern [2] = weight2;
			if (numberOfSyllables == 2) {
				addCandidate (me, 2, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress); cherror
			} else for (weight3 = minSurfaceWeight [3]; weight3 <= maxSurfaceWeight [3]; weight3 ++) {
				surfaceWeightPattern [3] = weight3;
				if (numberOfSyllables == 3) {
					addCandidate (me, 3, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress); cherror
				} else for (weight4 = minSurfaceWeight [4]; weight4 <= maxSurfaceWeight [4]; weight4 ++) {
					surfaceWeightPattern [4] = weight4;
					if (numberOfSyllables == 4) {
						addCandidate (me, 4, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress); cherror
					} else for (weight5 = minSurfaceWeight [5]; weight5 <= maxSurfaceWeight [5]; weight5 ++) {
						surfaceWeightPattern [5] = weight5;
						addCandidate (me, numberOfSyllables, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress); cherror
					}
				}
			}
		}
	}
end:
	return;
}

static void path (OTGrammarTableau me, long numberOfSyllables, int stress [],
	int startingSyllable, int footedToTheLeft_in [], int footedToTheRight_in [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	int footedToTheLeft [10], footedToTheRight [10];
	long isyll;
	/* Localize all arguments. */
	for (isyll = 1; isyll <= startingSyllable; isyll ++) {
		footedToTheLeft [isyll] = footedToTheLeft_in [isyll];
		footedToTheRight [isyll] = footedToTheRight_in [isyll];
	}
	for (isyll = startingSyllable + 1; isyll <= numberOfSyllables; isyll ++)
		footedToTheLeft [isyll] = footedToTheRight [isyll] = 0;
	if (startingSyllable > numberOfSyllables) {
		fillSurfaceWeightPattern (me, numberOfSyllables, stress, footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
	} else {
		path (me, numberOfSyllables, stress, startingSyllable + 1,
			footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
		if (stress [startingSyllable] == 0 && startingSyllable < numberOfSyllables && stress [startingSyllable + 1] != 0) {
			footedToTheLeft [startingSyllable + 1] = TRUE;
			footedToTheRight [startingSyllable] = TRUE;
			path (me, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
			footedToTheLeft [startingSyllable + 1] = FALSE;
			footedToTheRight [startingSyllable] = FALSE;
		}
		if (stress [startingSyllable] == 0 && startingSyllable > 1 && stress [startingSyllable - 1] != 0
		    && ! footedToTheLeft [startingSyllable - 1])
		{
			footedToTheRight [startingSyllable - 1] = TRUE;
			footedToTheLeft [startingSyllable] = TRUE;
			path (me, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
		}
	}
end:
	return;
}

static void fillOvertStressPattern (OTGrammarTableau me, long numberOfSyllables, int stress [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	int isyll, footedToTheLeft [10], footedToTheRight [10];
	for (isyll = 1; isyll <= numberOfSyllables; isyll ++)
		footedToTheLeft [isyll] = footedToTheRight [isyll] = 0;
	path (me, numberOfSyllables, stress, 1, footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
end:
	return;
}

static int fillTableau (OTGrammarTableau me, long numberOfSyllables, int underlyingWeightPattern [], int overtFormsHaveSecondaryStress, int includeCodas) {
	long isyll, mainStressed;
	int secondary1, secondary2, secondary3, secondary4, secondary5, secondary6;
	char input [100];
	static int numberOfCandidates_noCodas [1+7] = { 0, 1, 6, 24, 88, 300, 984, 3136 };
	static int numberOfCandidates_codas [1+7] = { 0, 1, 24, 192, 1408, 9600, 984, 3136 };
	strcpy (input, "|");
	for (isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		static char *syllable_noCodas [] = { "", "L", "H" };
		static char *syllable_codas [] = { "", "cv", "cv:", "cvc" };
		if (isyll > 1) strcat (input, includeCodas ? "." : " ");
		strcat (input, ( includeCodas ? syllable_codas : syllable_noCodas ) [underlyingWeightPattern [isyll]]);
	}
	strcat (input, "|");
	my input = Melder_strdup (input);
	my candidates = NUMstructvector (OTGrammarCandidate, 1, ( includeCodas ? numberOfCandidates_codas : numberOfCandidates_noCodas ) [numberOfSyllables]); cherror
	for (mainStressed = 1; mainStressed <= numberOfSyllables; mainStressed ++) {
		int stress [10];
		stress [mainStressed] = 1;
		for (secondary1 = FALSE; secondary1 <= TRUE; secondary1 ++) {
			stress [mainStressed <= 1 ? 2 : 1] = secondary1 ? 2 : 0;
			if (numberOfSyllables == 2) {
				fillOvertStressPattern (me, 2, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
			} else for (secondary2 = FALSE; secondary2 <= TRUE; secondary2 ++) {
				stress [mainStressed <= 2 ? 3 : 2] = secondary2 ? 2 : 0;
				if (numberOfSyllables == 3) {
					fillOvertStressPattern (me, 3, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
				} else for (secondary3 = FALSE; secondary3 <= TRUE; secondary3 ++) {
					stress [mainStressed <= 3 ? 4 : 3] = secondary3 ? 2 : 0;
					if (numberOfSyllables == 4) {
						fillOvertStressPattern (me, 4, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
					} else for (secondary4 = FALSE; secondary4 <= TRUE; secondary4 ++) {
						stress [mainStressed <= 4 ? 5 : 4] = secondary4 ? 2 : 0;
						if (numberOfSyllables == 5) {
							fillOvertStressPattern (me, 5, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
						} else for (secondary5 = FALSE; secondary5 <= TRUE; secondary5 ++) {
							stress [mainStressed <= 5 ? 6 : 5] = secondary5 ? 2 : 0;
							if (numberOfSyllables == 6) {
								fillOvertStressPattern (me, 6, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
							} else for (secondary6 = FALSE; secondary6 <= TRUE; secondary6 ++) {
								stress [mainStressed <= 6 ? 7 : 6] = secondary6 ? 2 : 0;
								fillOvertStressPattern (me, 7, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress); cherror
							}
						}
					}
				}
			}
		}
	}
end:
	iferror return 0;
	return 1;
}

static void computeViolationMarks (OTGrammarCandidate me) {
	#define isHeavy(s)  ((s) == 'H' || (s) == 'J')
	#define isLight(s)  ((s) == 'L' || (s) == 'K')
	#define isSyllable(s)  (isHeavy (s) || isLight (s))
	#define isStress(s)  ((s) == '1' || (s) == '2')
	int depth;
	char *firstSlash = strchr (my output, '/');
	char *lastSlash = & my output [strlen (my output) - 1];
	char *p, *q;
	my marks = NUMivector (1, my numberOfConstraints = NUMBER_OF_CONSTRAINTS); cherror
	/* Violations of WSP: count all H not followed by 1 or 2. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isHeavy (p [0]) && ! isStress (p [1]))
			my marks [WSP] ++;
	}
	/* Violations of FtNonfinal: count all heads followed by ). */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] == ')')
			my marks [FtNonfinal] ++;
	}
	/* Violations of Iambic: count all heads not followed by ). */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] != ')')
			my marks [Iambic] ++;
	}
	/* Violations of Parse and Peripheral: count all syllables not between (). */
	depth = 0;
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == '(') depth ++;
		else if (p [0] == ')') depth --;
		else if (isSyllable (p [0]) && depth != 1) {
			my marks [Parse] ++;
			if (p != firstSlash + 1 && p != lastSlash - 1)
				my marks [Peripheral] ++;
		}
	}
	/* Violations of FootBin: count all (L1) and (L2). */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isLight (p [0]) && p [-1] == '(' && isStress (p [1]) && p [2] == ')')
			my marks [FootBin] ++;
	}
	/* Violations of WFL: count all initial / not followed by (. */
	if (firstSlash [1] != '(')
		my marks [WFL] = 1;
	/* Violations of WFR: count all final / not preceded by ). */
	if (lastSlash [-1] != ')')
		my marks [WFR] = 1;
	/* Violations of Main_L: count syllables from foot containing X1 to left edge. */
	for (p = strchr (firstSlash, '1'); *p != '('; p --) { }
	for (; p != firstSlash; p --) {
		if (isSyllable (p [0]))
			my marks [Main_L] ++;
	}
	/* Violations of Main_R: count syllables from foot containing X1 to right edge. */
	for (p = strchr (firstSlash, '1'); *p != ')'; p ++) { }
	for (; p != lastSlash; p ++) {
		if (isSyllable (p [0]))
			my marks [Main_R] ++;
	}
	/* Violations of AFL: count syllables from every foot to left edge. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == '(') {
			for (q = p; q != firstSlash; q --) {
				if (isSyllable (q [0]))
					my marks [AFL] ++;
			}
		}
	}
	/* Violations of AFR: count syllables from every foot to right edge. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == ')') {
			for (q = p; q != lastSlash; q ++) {
				if (isSyllable (q [0]))
					my marks [AFR] ++;
			}
		}
	}
	/* Violations of Nonfinal: count all final / preceded by ). */
	if (lastSlash [-1] == ')')
		my marks [Nonfinal] = 1;
	/* Violations of Trochaic: count all heads not preceded by (. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [-2] != '(')
			my marks [Trochaic] ++;
	}
	/* Violations of FootBimoraic: count weight between (). */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == '(') {
			int weight = 0;
			for (p ++; p [0] != ')'; p ++) {
				if (isHeavy (p [0])) weight += 2;
				else if (isLight (p [0])) weight += 1;
			}
			if (weight != 2) my marks [FootBimoraic] ++;
		}
	}
	/* Violations of FootBisyllabic: count all (X1) and (X2). */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isSyllable (p [0]) && p [-1] == '(' && isStress (p [1]) && p [2] == ')')
			my marks [FootBisyllabic] ++;
	}
	/* Violations of MainNonfinal: count all final / preceded by ) preceded by 1 in the same foot. */
	if (lastSlash [-1] == ')') {
	    for (p = lastSlash - 2; ; p --) {
	    	if (p [0] == '2') break;
	    	if (p [0] == '1') {
				my marks [MainNonfinal] = 1;
				break;
			}
		}
	}
	/* Violations of HeadNonfinal: count all final / preceded by ) directly preceded by 1, plus MainNonfinal. */
	if (lastSlash [-1] == ')') {
		if (lastSlash [-2] == '1') {
			my marks [HeadNonfinal] = 2;
		} else {
		    for (p = lastSlash - 2; ; p --) {
		    	if (p [0] == '2') break;
		    	if (p [0] == '1') {
					my marks [HeadNonfinal] = 1;
					break;
				}
			}
		}
	}
	/* Violations of *Clash: count all 1 and 2 followed by an 1 or 2 after the next L or H. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0])) {
			for (q = p + 1; q != lastSlash; q ++) {
				if (isSyllable (q [0])) {
					if (isStress (q [1])) {
						my marks [Clash] ++;
					}
					break;
				}
			}
		}
	}
	/* Violations of *Lapse: count all sequences of three unstressed syllables. */
	depth = 0;
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (isSyllable (p [0])) {
			if (isStress (p [1])) {
				depth = 0;
			} else {
				if (++ depth > 2) {
					my marks [Lapse] ++;
				}
			}
		}
	}
	/* Violations of WeightByPosition: count all K. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == 'K')
			my marks [WeightByPosition] ++;
	}
	/* Violations of *MoraicConsonant: count all J. */
	for (p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == 'J')
			my marks [MoraicConsonant] ++;
	}
end:
	return;
}

static void replaceOutput (OTGrammarCandidate me) {
	const char *p;
	char newOutput [100], *q;
	int abstract = FALSE;
	Melder_assert (my output != NULL);
	for (p = & my output [0], q = & newOutput [0]; *p != '\0'; p ++) {
		if (p [0] == ' ') {
			*q ++ = p [-1] == ']' || p [1] == '/' ? ' ' : '.';
		} else if (isSyllable (p [0])) {
			*q ++ = 'c';
			if (abstract) {
				*q ++ = 'V';
				if (isStress (p [1])) {
					*q ++ = p [1];
				}
				if (p [0] == 'L') {
					;
				} else if (p [0] == 'H') {
					*q ++ = 'V';
				} else if (p [0] == 'K') {
					*q ++ = 'c';
				} else {
					*q ++ = 'C';
				}
			} else {
				*q ++ = 'v';
				if (p [0] == 'L') {
					;
				} else if (p [0] == 'H') {
					*q ++ = ':';
				} else {
					*q ++ = 'c';
				}
			}
		} else if (isStress (p [0]) && abstract) {
			;
		} else {
			if (p [0] == '/') abstract = TRUE;
			*q ++ = p [0];
		}
	}
	*q = '\0';
	Melder_free (my output);
	my output = Melder_strdup (newOutput);
}

OTGrammar OTGrammar_create_metrics (int equal_footForm_wsp, int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress,
	int includeClashAndLapse, int includeCodas)
{
	long icons, itab, icand, isyll;
	int numberOfSyllables;
	int underlyingWeightPattern [1+7], maximumUnderlyingWeight = includeCodas ? 3 : 2;
	long numberOfTableaus = includeCodas ? 9 + 27 + 81 + 243 + 2 : 62;
	OTGrammar me = new (OTGrammar); cherror
	my constraints = NUMstructvector (OTGrammarConstraint, 1, my numberOfConstraints = NUMBER_OF_CONSTRAINTS); cherror
	for (icons = 1; icons <= NUMBER_OF_CONSTRAINTS; icons ++) {
		OTGrammarConstraint constraint = & my constraints [icons];
		constraint -> name = Melder_strdup (constraintNames [icons]); cherror
		constraint -> ranking = 100.0;
	}
	if (equal_footForm_wsp >= 2) {
		/* Foot form constraints high. */
		my constraints [FtNonfinal]. ranking = 101.0;
		my constraints [Iambic]. ranking = 101.0;
		my constraints [Trochaic]. ranking = -1e9;
	}
	if (equal_footForm_wsp == 3) {
		/* Quantity sensitivity high, foot form constraints in the second stratum. */
		my constraints [WSP]. ranking = 102.0;
	}
	my tableaus = NUMstructvector (OTGrammarTableau, 1, numberOfTableaus); cherror
	for (numberOfSyllables = 2; numberOfSyllables <= 7; numberOfSyllables ++) {
		long numberOfUnderlyingWeightPatterns = numberOfSyllables > 5 ? 1 : floor (pow (maximumUnderlyingWeight, numberOfSyllables) + 0.5);
		long iweightPattern;
		for (isyll = 1; isyll <= numberOfSyllables; isyll ++) {
			underlyingWeightPattern [isyll] = 1;   /* L or cv */
		}
		for (iweightPattern = 1; iweightPattern <= numberOfUnderlyingWeightPatterns; iweightPattern ++) {
			fillTableau (& my tableaus [++ my numberOfTableaus], numberOfSyllables, underlyingWeightPattern, overtFormsHaveSecondaryStress, includeCodas);
			cherror
			/*
			 * Cycle to next underlying weight pattern.
			 */
			underlyingWeightPattern [numberOfSyllables] += 1;
			for (isyll = numberOfSyllables; isyll >= 2; isyll --) {
				if (underlyingWeightPattern [isyll] > maximumUnderlyingWeight) {
					underlyingWeightPattern [isyll] = 1;
					underlyingWeightPattern [isyll - 1] += 1;
				}
			}
		}
	}
	/* Compute violation marks. */
	for (itab = 1; itab <= my numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & my tableaus [itab];
		for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
			computeViolationMarks (& tableau -> candidates [icand]); cherror
		}
	}
	OTGrammar_checkIndex (me);
	OTGrammar_newDisharmonies (me, 0.0);
	if (trochaicityConstraint == 1) {
		OTGrammar_removeConstraint (me, "Trochaic");
	} else {
		OTGrammar_removeConstraint (me, "FtNonfinal");
	}
	if (! includeFootBimoraic) OTGrammar_removeConstraint (me, "FtBimor");
	if (! includeFootBisyllabic) OTGrammar_removeConstraint (me, "FtBisyl");
	if (! includePeripheral) OTGrammar_removeConstraint (me, "Peripheral");
	if (nonfinalityConstraint == 1) {
		OTGrammar_removeConstraint (me, "MainNonfinal");
		OTGrammar_removeConstraint (me, "HeadNonfinal");
	} else if (nonfinalityConstraint == 2) {
		OTGrammar_removeConstraint (me, "HeadNonfinal");
		OTGrammar_removeConstraint (me, "Nonfinal");
	} else {
		OTGrammar_removeConstraint (me, "MainNonfinal");
		OTGrammar_removeConstraint (me, "Nonfinal");
	}
	if (! includeClashAndLapse) {
		OTGrammar_removeConstraint (me, "*Clash");
		OTGrammar_removeConstraint (me, "*Lapse");
	}
	if (! includeCodas) {
		OTGrammar_removeConstraint (me, "WeightByPosition");
		OTGrammar_removeConstraint (me, "*C\\mu");
	}
	if (includeCodas) {
		for (itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				replaceOutput (& tableau -> candidates [icand]);
			}
		}
	}
end:
	iferror forget (me);
	return me;
}

/* End of file OTGrammar_ex_metrics.c */
