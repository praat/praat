/* OTGrammar_ex_metrics.cpp
 *
 * Copyright (C) 2001-2011 Paul Boersma
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
 * pb 2007/07/23 constraint plasticity
 * pb 2007/08/12 wchar_t
 * pb 2011/03/29 C++
 * pb 2011/06/29 C++
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

static const wchar_t *constraintNames [1+NUMBER_OF_CONSTRAINTS] = { 0,
	L"WSP", L"FtNonfinal", L"Iambic", L"Parse", L"FootBin", L"WFL", L"WFR", L"Main-L", L"Main-R", L"AFL", L"AFR", L"Nonfinal",
	L"Trochaic", L"FtBimor", L"FtBisyl", L"Peripheral", L"MainNonfinal", L"HeadNonfinal", L"*Clash", L"*Lapse", L"WeightByPosition", L"*C\\mu" };

static void addCandidate (OTGrammarTableau me, long numberOfSyllables, int stress [],
	int footedToTheLeft [], int footedToTheRight [], int surfaceWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	static const wchar_t *syllable [] = { L"L", L"L1", L"L2", L"H", L"H1", L"H2", L"K", L"K1", L"K2", L"J", L"J1", L"J2" };
	static const wchar_t *syllableWithoutSecondaryStress [] = { L"L", L"L1", L"L", L"H", L"H1", L"H", L"K", L"K1", L"K", L"J", L"J1", L"J" };
	wchar_t output [100];
	wcscpy (output, L"[");
	for (long isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1) wcscat (output, L" ");
		wcscat (output, ( overtFormsHaveSecondaryStress ? syllable : syllableWithoutSecondaryStress )
				[stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
	}
	wcscat (output, L"] \\-> /");
	for (long isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1) wcscat (output, L" ");
		if (footedToTheRight [isyll] || (! footedToTheLeft [isyll] && stress [isyll] != 0)) wcscat (output, L"(");
		wcscat (output, syllable [stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
		if (footedToTheLeft [isyll] || (! footedToTheRight [isyll] && stress [isyll] != 0)) wcscat (output, L")");
	}
	wcscat (output, L"/");
	my candidates [++ my numberOfCandidates]. output = Melder_wcsdup (output);
}

static void fillSurfaceWeightPattern (OTGrammarTableau me, long numberOfSyllables, int stress [],
	int footedToTheLeft [], int footedToTheRight [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	int surfaceWeightPattern [1+7], minSurfaceWeight [1+7], maxSurfaceWeight [1+7];
	int weight1, weight2, weight3, weight4, weight5;
	for (long isyll = 1; isyll <= numberOfSyllables; isyll ++) {
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
				addCandidate (me, 2, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
			} else for (weight3 = minSurfaceWeight [3]; weight3 <= maxSurfaceWeight [3]; weight3 ++) {
				surfaceWeightPattern [3] = weight3;
				if (numberOfSyllables == 3) {
					addCandidate (me, 3, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
				} else for (weight4 = minSurfaceWeight [4]; weight4 <= maxSurfaceWeight [4]; weight4 ++) {
					surfaceWeightPattern [4] = weight4;
					if (numberOfSyllables == 4) {
						addCandidate (me, 4, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
					} else for (weight5 = minSurfaceWeight [5]; weight5 <= maxSurfaceWeight [5]; weight5 ++) {
						surfaceWeightPattern [5] = weight5;
						addCandidate (me, numberOfSyllables, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
					}
				}
			}
		}
	}
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
			footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
		if (stress [startingSyllable] == 0 && startingSyllable < numberOfSyllables && stress [startingSyllable + 1] != 0) {
			footedToTheLeft [startingSyllable + 1] = TRUE;
			footedToTheRight [startingSyllable] = TRUE;
			path (me, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
			footedToTheLeft [startingSyllable + 1] = FALSE;
			footedToTheRight [startingSyllable] = FALSE;
		}
		if (stress [startingSyllable] == 0 && startingSyllable > 1 && stress [startingSyllable - 1] != 0
		    && ! footedToTheLeft [startingSyllable - 1])
		{
			footedToTheRight [startingSyllable - 1] = TRUE;
			footedToTheLeft [startingSyllable] = TRUE;
			path (me, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
		}
	}
}

static void fillOvertStressPattern (OTGrammarTableau me, long numberOfSyllables, int stress [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	int footedToTheLeft [10], footedToTheRight [10];
	for (int isyll = 1; isyll <= numberOfSyllables; isyll ++)
		footedToTheLeft [isyll] = footedToTheRight [isyll] = 0;
	path (me, numberOfSyllables, stress, 1, footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
}

static void fillTableau (OTGrammarTableau me, long numberOfSyllables, int underlyingWeightPattern [], int overtFormsHaveSecondaryStress, int includeCodas) {
	wchar_t input [100];
	static int numberOfCandidates_noCodas [1+7] = { 0, 1, 6, 24, 88, 300, 984, 3136 };
	static int numberOfCandidates_codas [1+7] = { 0, 1, 24, 192, 1408, 9600, 984, 3136 };
	wcscpy (input, L"|");
	for (long isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		static const wchar_t *syllable_noCodas [] = { L"", L"L", L"H" };
		static const wchar_t *syllable_codas [] = { L"", L"cv", L"cv:", L"cvc" };
		if (isyll > 1) wcscat (input, includeCodas ? L"." : L" ");
		wcscat (input, ( includeCodas ? syllable_codas : syllable_noCodas ) [underlyingWeightPattern [isyll]]);
	}
	wcscat (input, L"|");
	my input = Melder_wcsdup (input);
	my candidates = NUMvector <structOTGrammarCandidate> (1, ( includeCodas ? numberOfCandidates_codas : numberOfCandidates_noCodas ) [numberOfSyllables]);
	for (long mainStressed = 1; mainStressed <= numberOfSyllables; mainStressed ++) {
		int stress [10];
		stress [mainStressed] = 1;
		for (int secondary1 = FALSE; secondary1 <= TRUE; secondary1 ++) {
			stress [mainStressed <= 1 ? 2 : 1] = secondary1 ? 2 : 0;
			if (numberOfSyllables == 2) {
				fillOvertStressPattern (me, 2, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
			} else for (int secondary2 = FALSE; secondary2 <= TRUE; secondary2 ++) {
				stress [mainStressed <= 2 ? 3 : 2] = secondary2 ? 2 : 0;
				if (numberOfSyllables == 3) {
					fillOvertStressPattern (me, 3, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
				} else for (int secondary3 = FALSE; secondary3 <= TRUE; secondary3 ++) {
					stress [mainStressed <= 3 ? 4 : 3] = secondary3 ? 2 : 0;
					if (numberOfSyllables == 4) {
						fillOvertStressPattern (me, 4, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
					} else for (int secondary4 = FALSE; secondary4 <= TRUE; secondary4 ++) {
						stress [mainStressed <= 4 ? 5 : 4] = secondary4 ? 2 : 0;
						if (numberOfSyllables == 5) {
							fillOvertStressPattern (me, 5, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
						} else for (int secondary5 = FALSE; secondary5 <= TRUE; secondary5 ++) {
							stress [mainStressed <= 5 ? 6 : 5] = secondary5 ? 2 : 0;
							if (numberOfSyllables == 6) {
								fillOvertStressPattern (me, 6, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
							} else for (int secondary6 = FALSE; secondary6 <= TRUE; secondary6 ++) {
								stress [mainStressed <= 6 ? 7 : 6] = secondary6 ? 2 : 0;
								fillOvertStressPattern (me, 7, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
							}
						}
					}
				}
			}
		}
	}
}

static void computeViolationMarks (OTGrammarCandidate me) {
	#define isHeavy(s)  ((s) == 'H' || (s) == 'J')
	#define isLight(s)  ((s) == 'L' || (s) == 'K')
	#define isSyllable(s)  (isHeavy (s) || isLight (s))
	#define isStress(s)  ((s) == '1' || (s) == '2')
	int depth;
	wchar_t *firstSlash = wcschr (my output, '/');
	wchar_t *lastSlash = & my output [wcslen (my output) - 1];
	my marks = NUMvector <int> (1, my numberOfConstraints = NUMBER_OF_CONSTRAINTS);
	/* Violations of WSP: count all H not followed by 1 or 2. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isHeavy (p [0]) && ! isStress (p [1]))
			my marks [WSP] ++;
	}
	/* Violations of FtNonfinal: count all heads followed by ). */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] == ')')
			my marks [FtNonfinal] ++;
	}
	/* Violations of Iambic: count all heads not followed by ). */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] != ')')
			my marks [Iambic] ++;
	}
	/* Violations of Parse and Peripheral: count all syllables not between (). */
	depth = 0;
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == '(') depth ++;
		else if (p [0] == ')') depth --;
		else if (isSyllable (p [0]) && depth != 1) {
			my marks [Parse] ++;
			if (p != firstSlash + 1 && p != lastSlash - 1)
				my marks [Peripheral] ++;
		}
	}
	/* Violations of FootBin: count all (L1) and (L2). */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
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
	{
		wchar_t *p = wcschr (firstSlash, '1');
		for (; *p != '('; p --) { }
		for (; p != firstSlash; p --) {
			if (isSyllable (p [0]))
				my marks [Main_L] ++;
		}
	}
	/* Violations of Main_R: count syllables from foot containing X1 to right edge. */
	{
		wchar_t *p = wcschr (firstSlash, '1');
		for (; *p != ')'; p ++) { }
		for (; p != lastSlash; p ++) {
			if (isSyllable (p [0]))
				my marks [Main_R] ++;
		}
	}
	/* Violations of AFL: count syllables from every foot to left edge. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == '(') {
			for (wchar_t *q = p; q != firstSlash; q --) {
				if (isSyllable (q [0]))
					my marks [AFL] ++;
			}
		}
	}
	/* Violations of AFR: count syllables from every foot to right edge. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == ')') {
			for (wchar_t *q = p; q != lastSlash; q ++) {
				if (isSyllable (q [0]))
					my marks [AFR] ++;
			}
		}
	}
	/* Violations of Nonfinal: count all final / preceded by ). */
	if (lastSlash [-1] == ')')
		my marks [Nonfinal] = 1;
	/* Violations of Trochaic: count all heads not preceded by (. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [-2] != '(')
			my marks [Trochaic] ++;
	}
	/* Violations of FootBimoraic: count weight between (). */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
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
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isSyllable (p [0]) && p [-1] == '(' && isStress (p [1]) && p [2] == ')')
			my marks [FootBisyllabic] ++;
	}
	/* Violations of MainNonfinal: count all final / preceded by ) preceded by 1 in the same foot. */
	if (lastSlash [-1] == ')') {
		for (wchar_t *p = lastSlash - 2; ; p --) {
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
			for (wchar_t *p = lastSlash - 2; ; p --) {
				if (p [0] == '2') break;
				if (p [0] == '1') {
					my marks [HeadNonfinal] = 1;
					break;
				}
			}
		}
	}
	/* Violations of *Clash: count all 1 and 2 followed by an 1 or 2 after the next L or H. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0])) {
			for (wchar_t *q = p + 1; q != lastSlash; q ++) {
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
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
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
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == 'K')
			my marks [WeightByPosition] ++;
	}
	/* Violations of *MoraicConsonant: count all J. */
	for (wchar_t *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == 'J')
			my marks [MoraicConsonant] ++;
	}
}

static void replaceOutput (OTGrammarCandidate me) {
	int abstract = FALSE;
	Melder_assert (my output != NULL);
	wchar_t newOutput [100], *q = & newOutput [0];
	for (const wchar_t *p = & my output [0]; *p != '\0'; p ++) {
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
	my output = Melder_wcsdup_f (newOutput);
}

OTGrammar OTGrammar_create_metrics (int equal_footForm_wsp, int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress,
	int includeClashAndLapse, int includeCodas)
{
	try {
		int numberOfSyllables;
		int underlyingWeightPattern [1+7], maximumUnderlyingWeight = includeCodas ? 3 : 2;
		long numberOfTableaus = includeCodas ? 9 + 27 + 81 + 243 + 2 : 62;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = NUMvector <structOTGrammarConstraint> (1, my numberOfConstraints = NUMBER_OF_CONSTRAINTS);
		for (long icons = 1; icons <= NUMBER_OF_CONSTRAINTS; icons ++) {
			OTGrammarConstraint constraint = & my constraints [icons];
			constraint -> name = Melder_wcsdup (constraintNames [icons]);
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
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
		my tableaus = NUMvector <structOTGrammarTableau> (1, numberOfTableaus);
		for (numberOfSyllables = 2; numberOfSyllables <= 7; numberOfSyllables ++) {
			long numberOfUnderlyingWeightPatterns = numberOfSyllables > 5 ? 1 : (long) floor (pow (maximumUnderlyingWeight, numberOfSyllables) + 0.5);
			for (long isyll = 1; isyll <= numberOfSyllables; isyll ++) {
				underlyingWeightPattern [isyll] = 1;   /* L or cv */
			}
			for (long iweightPattern = 1; iweightPattern <= numberOfUnderlyingWeightPatterns; iweightPattern ++) {
				fillTableau (& my tableaus [++ my numberOfTableaus], numberOfSyllables, underlyingWeightPattern, overtFormsHaveSecondaryStress, includeCodas);
				/*
				 * Cycle to next underlying weight pattern.
				 */
				underlyingWeightPattern [numberOfSyllables] += 1;
				for (long isyll = numberOfSyllables; isyll >= 2; isyll --) {
					if (underlyingWeightPattern [isyll] > maximumUnderlyingWeight) {
						underlyingWeightPattern [isyll] = 1;
						underlyingWeightPattern [isyll - 1] += 1;
					}
				}
			}
		}
		/* Compute violation marks. */
		for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
				computeViolationMarks (& tableau -> candidates [icand]);
			}
		}
		OTGrammar_checkIndex (me.peek());
		OTGrammar_newDisharmonies (me.peek(), 0.0);
		if (trochaicityConstraint == 1) {
			OTGrammar_removeConstraint (me.peek(), L"Trochaic");
		} else {
			OTGrammar_removeConstraint (me.peek(), L"FtNonfinal");
		}
		if (! includeFootBimoraic) OTGrammar_removeConstraint (me.peek(), L"FtBimor");
		if (! includeFootBisyllabic) OTGrammar_removeConstraint (me.peek(), L"FtBisyl");
		if (! includePeripheral) OTGrammar_removeConstraint (me.peek(), L"Peripheral");
		if (nonfinalityConstraint == 1) {
			OTGrammar_removeConstraint (me.peek(), L"MainNonfinal");
			OTGrammar_removeConstraint (me.peek(), L"HeadNonfinal");
		} else if (nonfinalityConstraint == 2) {
			OTGrammar_removeConstraint (me.peek(), L"HeadNonfinal");
			OTGrammar_removeConstraint (me.peek(), L"Nonfinal");
		} else {
			OTGrammar_removeConstraint (me.peek(), L"MainNonfinal");
			OTGrammar_removeConstraint (me.peek(), L"Nonfinal");
		}
		if (! includeClashAndLapse) {
			OTGrammar_removeConstraint (me.peek(), L"*Clash");
			OTGrammar_removeConstraint (me.peek(), L"*Lapse");
		}
		if (! includeCodas) {
			OTGrammar_removeConstraint (me.peek(), L"WeightByPosition");
			OTGrammar_removeConstraint (me.peek(), L"*C\\mu");
		}
		if (includeCodas) {
			for (long itab = 1; itab <= my numberOfTableaus; itab ++) {
				OTGrammarTableau tableau = & my tableaus [itab];
				for (long icand = 1; icand <= tableau -> numberOfCandidates; icand ++) {
					replaceOutput (& tableau -> candidates [icand]);
				}
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Metrics grammar not created.");
	}
}

/* End of file OTGrammar_ex_metrics.cpp */
