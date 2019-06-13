/* OTMulti_ex_metrics.cpp
 *
 * Copyright (C) 2014-2018 Paul Boersma
 * Forked from OTGrammar_ex_metrics.cpp, Copyright (C) 2001-2007,2009,2011,2012 Paul Boersma
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

#include "OTMulti.h"

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

static const conststring32 constraintNames [1+NUMBER_OF_CONSTRAINTS] { 0,
	U"WSP", U"FtNonfinal", U"Iambic", U"Parse", U"FootBin", U"WFL", U"WFR", U"Main-L", U"Main-R", U"AFL", U"AFR", U"Nonfinal",
	U"Trochaic", U"FtBimor", U"FtBisyl", U"Peripheral", U"MainNonfinal", U"HeadNonfinal", U"*Clash", U"*Lapse", U"WeightByPosition", U"*C\\mu" };

static void addCandidate (OTMulti me, conststring32 underlyingForm, integer numberOfSyllables, int stress [],
	bool footedToTheLeft [], bool footedToTheRight [], int surfaceWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	static const conststring32 syllable [] { U"L", U"L1", U"L2", U"H", U"H1", U"H2", U"K", U"K1", U"K2", U"J", U"J1", U"J2" };
	static const conststring32 syllableWithoutSecondaryStress [] { U"L", U"L1", U"L", U"H", U"H1", U"H", U"K", U"K1", U"K", U"J", U"J1", U"J" };
	char32 string [150];
	str32cpy (string, underlyingForm);
	str32cat (string, U" /");
	for (integer isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1)
			str32cat (string, U" ");
		if (footedToTheRight [isyll] || (! footedToTheLeft [isyll] && stress [isyll] != 0))
			str32cat (string, U"(");
		str32cat (string, syllable [stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
		if (footedToTheLeft [isyll] || (! footedToTheRight [isyll] && stress [isyll] != 0))
			str32cat (string, U")");
	}
	str32cat (string, U"/ [");
	for (integer isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (isyll > 1)
			str32cat (string, U" ");
		str32cat (string, ( overtFormsHaveSecondaryStress ? syllable : syllableWithoutSecondaryStress )
				[stress [isyll] + 3 * (surfaceWeightPattern [isyll] - 1)]);
	}
	str32cat (string, U"]");
	my candidates [++ my numberOfCandidates]. string = Melder_dup (string);
}

static void fillSurfaceWeightPattern (OTMulti me, conststring32 underlyingForm, integer numberOfSyllables, int stress [],
	bool footedToTheLeft [], bool footedToTheRight [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	int surfaceWeightPattern [1+7], minSurfaceWeight [1+7], maxSurfaceWeight [1+7];
	int weight1, weight2, weight3, weight4, weight5;
	for (integer isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		if (underlyingWeightPattern [isyll] < 3) {
			minSurfaceWeight [isyll] = maxSurfaceWeight [isyll] = underlyingWeightPattern [isyll];   // L -> L; H -> H
		} else {
			minSurfaceWeight [isyll] = 3, maxSurfaceWeight [isyll] = 4;   // C -> { J, K }
		}
	}
	surfaceWeightPattern [6] = surfaceWeightPattern [7] = 1;   // constant L
	for (weight1 = minSurfaceWeight [1]; weight1 <= maxSurfaceWeight [1]; weight1 ++) {
		surfaceWeightPattern [1] = weight1;
		for (weight2 = minSurfaceWeight [2]; weight2 <= maxSurfaceWeight [2]; weight2 ++) {
			surfaceWeightPattern [2] = weight2;
			if (numberOfSyllables == 2) {
				addCandidate (me, underlyingForm, 2, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
			} else for (weight3 = minSurfaceWeight [3]; weight3 <= maxSurfaceWeight [3]; weight3 ++) {
				surfaceWeightPattern [3] = weight3;
				if (numberOfSyllables == 3) {
					addCandidate (me, underlyingForm, 3, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
				} else for (weight4 = minSurfaceWeight [4]; weight4 <= maxSurfaceWeight [4]; weight4 ++) {
					surfaceWeightPattern [4] = weight4;
					if (numberOfSyllables == 4) {
						addCandidate (me, underlyingForm, 4, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
					} else for (weight5 = minSurfaceWeight [5]; weight5 <= maxSurfaceWeight [5]; weight5 ++) {
						surfaceWeightPattern [5] = weight5;
						addCandidate (me, underlyingForm, numberOfSyllables, stress, footedToTheLeft, footedToTheRight, surfaceWeightPattern, overtFormsHaveSecondaryStress);
					}
				}
			}
		}
	}
}

static void path (OTMulti me, conststring32 underlyingForm, integer numberOfSyllables, int stress [],
	int startingSyllable, bool footedToTheLeft_in [], bool footedToTheRight_in [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	bool footedToTheLeft [10], footedToTheRight [10];
	integer isyll;
	/* Localize all arguments. */
	for (isyll = 1; isyll <= startingSyllable; isyll ++) {
		footedToTheLeft [isyll] = footedToTheLeft_in [isyll];
		footedToTheRight [isyll] = footedToTheRight_in [isyll];
	}
	for (isyll = startingSyllable + 1; isyll <= numberOfSyllables; isyll ++)
		footedToTheLeft [isyll] = footedToTheRight [isyll] = 0;
	if (startingSyllable > numberOfSyllables) {
		fillSurfaceWeightPattern (me, underlyingForm, numberOfSyllables, stress, footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
	} else {
		path (me, underlyingForm, numberOfSyllables, stress, startingSyllable + 1,
			footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
		if (stress [startingSyllable] == 0 && startingSyllable < numberOfSyllables && stress [startingSyllable + 1] != 0) {
			footedToTheLeft [startingSyllable + 1] = true;
			footedToTheRight [startingSyllable] = true;
			path (me, underlyingForm, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
			footedToTheLeft [startingSyllable + 1] = false;
			footedToTheRight [startingSyllable] = false;
		}
		if (stress [startingSyllable] == 0 && startingSyllable > 1 && stress [startingSyllable - 1] != 0
		    && ! footedToTheLeft [startingSyllable - 1])
		{
			footedToTheRight [startingSyllable - 1] = true;
			footedToTheLeft [startingSyllable] = true;
			path (me, underlyingForm, numberOfSyllables, stress, startingSyllable + 1,
				footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
		}
	}
}

static void fillOvertStressPattern (OTMulti me, conststring32 underlyingForm, integer numberOfSyllables, int stress [], int underlyingWeightPattern [],
	int overtFormsHaveSecondaryStress)
{
	bool footedToTheLeft [10], footedToTheRight [10];
	for (int isyll = 1; isyll <= numberOfSyllables; isyll ++)
		footedToTheLeft [isyll] = footedToTheRight [isyll] = 0;
	path (me, underlyingForm, numberOfSyllables, stress, 1, footedToTheLeft, footedToTheRight, underlyingWeightPattern, overtFormsHaveSecondaryStress);
}

static int numberOfCandidates_noCodas [1+7] { 0, 1, 6, 24, 88, 300, 984, 3136 };
static int numberOfCandidates_codas [1+7] { 0, 1, 24, 192, 1408, 9600, 984, 3136 };

static void fillTableau (OTMulti me, integer numberOfSyllables, int underlyingWeightPattern [], int overtFormsHaveSecondaryStress, int includeCodas) {
	char32 underlyingForm [100];
	str32cpy (underlyingForm, U"|");
	for (integer isyll = 1; isyll <= numberOfSyllables; isyll ++) {
		static const conststring32 syllable_noCodas [] = { U"", U"L", U"H" };
		static const conststring32 syllable_codas [] = { U"", U"cv", U"cv:", U"cvc" };
		if (isyll > 1)
			str32cat (underlyingForm, includeCodas ? U"." : U" ");
		str32cat (underlyingForm, ( includeCodas ? syllable_codas : syllable_noCodas ) [underlyingWeightPattern [isyll]]);
	}
	str32cat (underlyingForm, U"|");
	for (integer mainStressed = 1; mainStressed <= numberOfSyllables; mainStressed ++) {
		int stress [10];
		stress [mainStressed] = 1;
		for (int secondary1 = false; secondary1 <= true; secondary1 ++) {
			stress [mainStressed <= 1 ? 2 : 1] = secondary1 ? 2 : 0;
			if (numberOfSyllables == 2) {
				fillOvertStressPattern (me, underlyingForm, 2, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
			} else for (int secondary2 = false; secondary2 <= true; secondary2 ++) {
				stress [mainStressed <= 2 ? 3 : 2] = secondary2 ? 2 : 0;
				if (numberOfSyllables == 3) {
					fillOvertStressPattern (me, underlyingForm, 3, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
				} else for (int secondary3 = false; secondary3 <= true; secondary3 ++) {
					stress [mainStressed <= 3 ? 4 : 3] = secondary3 ? 2 : 0;
					if (numberOfSyllables == 4) {
						fillOvertStressPattern (me, underlyingForm, 4, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
					} else for (int secondary4 = false; secondary4 <= true; secondary4 ++) {
						stress [mainStressed <= 4 ? 5 : 4] = secondary4 ? 2 : 0;
						if (numberOfSyllables == 5) {
							fillOvertStressPattern (me, underlyingForm, 5, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
						} else for (int secondary5 = false; secondary5 <= true; secondary5 ++) {
							stress [mainStressed <= 5 ? 6 : 5] = secondary5 ? 2 : 0;
							if (numberOfSyllables == 6) {
								fillOvertStressPattern (me, underlyingForm, 6, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
							} else for (int secondary6 = false; secondary6 <= true; secondary6 ++) {
								stress [mainStressed <= 6 ? 7 : 6] = secondary6 ? 2 : 0;
								fillOvertStressPattern (me, underlyingForm, 7, stress, underlyingWeightPattern, overtFormsHaveSecondaryStress);
							}
						}
					}
				}
			}
		}
	}
}

static void computeViolationMarks (OTCandidate me) {
	#define isHeavy(s)  ((s) == U'H' || (s) == U'J')
	#define isLight(s)  ((s) == U'L' || (s) == U'K')
	#define isSyllable(s)  (isHeavy (s) || isLight (s))
	#define isStress(s)  ((s) == U'1' || (s) == U'2')
	const char32 * const firstSlash = str32chr (my string.get(), U'/');
	const char32 * const lastSlash = str32chr (firstSlash + 1, U'/');
	my marks = newINTVECzero (my numberOfConstraints = NUMBER_OF_CONSTRAINTS);
	/* Violations of WSP: count all H not followed by 1 or 2. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isHeavy (p [0]) && ! isStress (p [1]))
			my marks [WSP] ++;
	}
	/* Violations of FtNonfinal: count all heads followed by ). */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] == ')')
			my marks [FtNonfinal] ++;
	}
	/* Violations of Iambic: count all heads not followed by ). */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [1] != ')')
			my marks [Iambic] ++;
	}
	/* Violations of Parse and Peripheral: count all syllables not between (). */
	int depth = 0;
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U'(') depth ++;
		else if (p [0] == U')') depth --;
		else if (isSyllable (p [0]) && depth != 1) {
			my marks [Parse] ++;
			if (p != firstSlash + 1 && p != lastSlash - 1)
				my marks [Peripheral] ++;
		}
	}
	/* Violations of FootBin: count all (L1) and (L2). */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isLight (p [0]) && p [-1] == '(' && isStress (p [1]) && p [2] == U')')
			my marks [FootBin] ++;
	}
	/* Violations of WFL: count all initial / not followed by (. */
	if (firstSlash [1] != U'(')
		my marks [WFL] = 1;
	/* Violations of WFR: count all final / not preceded by ). */
	if (lastSlash [-1] != ')')
		my marks [WFR] = 1;
	/* Violations of Main_L: count syllables from foot containing X1 to left edge. */
	{
		const char32 *p = str32chr (firstSlash, U'1');
		for (; *p != U'('; p --) { }
		for (; p != firstSlash; p --) {
			if (isSyllable (p [0]))
				my marks [Main_L] ++;
		}
	}
	/* Violations of Main_R: count syllables from foot containing X1 to right edge. */
	{
		const char32 *p = str32chr (firstSlash, U'1');
		for (; *p != U')'; p ++) { }
		for (; p != lastSlash; p ++) {
			if (isSyllable (p [0]))
				my marks [Main_R] ++;
		}
	}
	/* Violations of AFL: count syllables from every foot to left edge. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U'(') {
			for (const char32 *q = p; q != firstSlash; q --) {
				if (isSyllable (q [0]))
					my marks [AFL] ++;
			}
		}
	}
	/* Violations of AFR: count syllables from every foot to right edge. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U')') {
			for (const char32 *q = p; q != lastSlash; q ++) {
				if (isSyllable (q [0]))
					my marks [AFR] ++;
			}
		}
	}
	/* Violations of Nonfinal: count all final / preceded by ). */
	if (lastSlash [-1] == U')')
		my marks [Nonfinal] = 1;
	/* Violations of Trochaic: count all heads not preceded by (. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0]) && p [-2] != '(')
			my marks [Trochaic] ++;
	}
	/* Violations of FootBimoraic: count weight between (). */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U'(') {
			int weight = 0;
			for (p ++; p [0] != ')'; p ++) {
				if (isHeavy (p [0])) weight += 2;
				else if (isLight (p [0])) weight += 1;
			}
			if (weight != 2) my marks [FootBimoraic] ++;
		}
	}
	/* Violations of FootBisyllabic: count all (X1) and (X2). */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isSyllable (p [0]) && p [-1] == U'(' && isStress (p [1]) && p [2] == U')')
			my marks [FootBisyllabic] ++;
	}
	/* Violations of MainNonfinal: count all final / preceded by ) preceded by 1 in the same foot. */
	if (lastSlash [-1] == U')') {
		for (const char32 *p = lastSlash - 2; ; p --) {
			if (p [0] == '2') break;
			if (p [0] == '1') {
				my marks [MainNonfinal] = 1;
				break;
			}
		}
	}
	/* Violations of HeadNonfinal: count all final / preceded by ) directly preceded by 1, plus MainNonfinal. */
	if (lastSlash [-1] == U')') {
		if (lastSlash [-2] == U'1') {
			my marks [HeadNonfinal] = 2;
		} else {
			for (const char32 *p = lastSlash - 2; ; p --) {
				if (p [0] == U'2') break;
				if (p [0] == U'1') {
					my marks [HeadNonfinal] = 1;
					break;
				}
			}
		}
	}
	/* Violations of *Clash: count all 1 and 2 followed by an 1 or 2 after the next L or H. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (isStress (p [0])) {
			for (const char32 *q = p + 1; q != lastSlash; q ++) {
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
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
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
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U'K')
			my marks [WeightByPosition] ++;
	}
	/* Violations of *MoraicConsonant: count all J. */
	for (const char32 *p = firstSlash + 1; p != lastSlash; p ++) {
		if (p [0] == U'J')
			my marks [MoraicConsonant] ++;
	}
}

static void replaceOutput (OTCandidate me) {
	bool abstract = false;
	Melder_assert (my string);
	char32 newString [150], *q = & newString [0];
	for (const char32 *p = & my string [0]; *p != U'\0'; p ++) {
		if (p [0] == U' ') {
			*q ++ = p [-1] == U']' || p [1] == U'/' ? U' ' : U'.';
		} else if (isSyllable (p [0])) {
			*q ++ = U'c';
			if (abstract) {
				*q ++ = U'V';
				if (isStress (p [1])) {
					*q ++ = p [1];
				}
				if (p [0] == U'L') {
					;
				} else if (p [0] == U'H') {
					*q ++ = U'V';
				} else if (p [0] == U'K') {
					*q ++ = U'c';
				} else {
					*q ++ = U'C';
				}
			} else {
				*q ++ = U'v';
				if (p [0] == U'L') {
					;
				} else if (p [0] == 'H') {
					*q ++ = U':';
				} else {
					*q ++ = U'c';
				}
			}
		} else if (isStress (p [0]) && abstract) {
			;
		} else {
			if (p [0] == U'/') abstract = true;
			*q ++ = p [0];
		}
	}
	*q = U'\0';
	my string = Melder_dup (newString);
}

autoOTMulti OTMulti_create_metrics (
	kOTGrammar_createMetricsGrammar_initialRanking equal_footForm_wsp,
	int trochaicityConstraint, int includeFootBimoraic, int includeFootBisyllabic,
	int includePeripheral, int nonfinalityConstraint, int overtFormsHaveSecondaryStress,
	int includeClashAndLapse, int includeCodas)
{
	try {
		int underlyingWeightPattern [1+7], maximumUnderlyingWeight = includeCodas ? 3 : 2;
		autoOTMulti me = Thing_new (OTMulti);
		my constraints = NUMvector <structOTConstraint> (1, my numberOfConstraints = NUMBER_OF_CONSTRAINTS);
		for (integer icons = 1; icons <= NUMBER_OF_CONSTRAINTS; icons ++) {
			OTConstraint constraint = & my constraints [icons];
			constraint -> name = Melder_dup (constraintNames [icons]);
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
		}
		if (equal_footForm_wsp >= kOTGrammar_createMetricsGrammar_initialRanking::FOOT_FORM_HIGH) {
			/* Foot form constraints high. */
			my constraints [FtNonfinal]. ranking = 101.0;
			my constraints [Iambic]. ranking = 101.0;
			my constraints [Trochaic]. ranking = -1e9;
		}
		if (equal_footForm_wsp == kOTGrammar_createMetricsGrammar_initialRanking::WSP_HIGH) {
			/* Quantity sensitivity high, foot form constraints in the second stratum. */
			my constraints [WSP]. ranking = 102.0;
		}
		integer numberOfCandidates = 0;
		for (int numberOfSyllables = 2; numberOfSyllables <= 7; numberOfSyllables ++) {
			integer numberOfUnderlyingWeightPatterns = numberOfSyllables > 5 ? 1 : Melder_iround (pow (maximumUnderlyingWeight, numberOfSyllables));
			numberOfCandidates += ( includeCodas ? numberOfCandidates_codas : numberOfCandidates_noCodas ) [numberOfSyllables] * numberOfUnderlyingWeightPatterns;
		}
		my candidates = NUMvector <structOTCandidate> (1, numberOfCandidates);
		my numberOfCandidates = 0;
		for (int numberOfSyllables = 2; numberOfSyllables <= 7; numberOfSyllables ++) {
			integer numberOfUnderlyingWeightPatterns = numberOfSyllables > 5 ? 1 : Melder_iround (pow (maximumUnderlyingWeight, numberOfSyllables));
			for (integer isyll = 1; isyll <= numberOfSyllables; isyll ++) {
				underlyingWeightPattern [isyll] = 1;   /* L or cv */
			}
			for (integer iweightPattern = 1; iweightPattern <= numberOfUnderlyingWeightPatterns; iweightPattern ++) {
				fillTableau (me.get(), numberOfSyllables, underlyingWeightPattern, overtFormsHaveSecondaryStress, includeCodas);
				/*
				 * Cycle to next underlying weight pattern.
				 */
				underlyingWeightPattern [numberOfSyllables] += 1;
				for (integer isyll = numberOfSyllables; isyll >= 2; isyll --) {
					if (underlyingWeightPattern [isyll] > maximumUnderlyingWeight) {
						underlyingWeightPattern [isyll] = 1;
						underlyingWeightPattern [isyll - 1] += 1;
					}
				}
			}
		}
		Melder_assert (my numberOfCandidates == numberOfCandidates);
		/* Compute violation marks. */
		for (integer icand = 1; icand <= my numberOfCandidates; icand ++) {
			computeViolationMarks (& my candidates [icand]);
		}
		OTMulti_checkIndex (me.get());
		OTMulti_newDisharmonies (me.get(), 0.0);
		if (trochaicityConstraint == 1) {
			OTMulti_removeConstraint (me.get(), U"Trochaic");
		} else {
			OTMulti_removeConstraint (me.get(), U"FtNonfinal");
		}
		if (! includeFootBimoraic) OTMulti_removeConstraint (me.get(), U"FtBimor");
		if (! includeFootBisyllabic) OTMulti_removeConstraint (me.get(), U"FtBisyl");
		if (! includePeripheral) OTMulti_removeConstraint (me.get(), U"Peripheral");
		if (nonfinalityConstraint == 1) {
			OTMulti_removeConstraint (me.get(), U"MainNonfinal");
			OTMulti_removeConstraint (me.get(), U"HeadNonfinal");
		} else if (nonfinalityConstraint == 2) {
			OTMulti_removeConstraint (me.get(), U"HeadNonfinal");
			OTMulti_removeConstraint (me.get(), U"Nonfinal");
		} else {
			OTMulti_removeConstraint (me.get(), U"MainNonfinal");
			OTMulti_removeConstraint (me.get(), U"Nonfinal");
		}
		if (! includeClashAndLapse) {
			OTMulti_removeConstraint (me.get(), U"*Clash");
			OTMulti_removeConstraint (me.get(), U"*Lapse");
		}
		if (! includeCodas) {
			OTMulti_removeConstraint (me.get(), U"WeightByPosition");
			OTMulti_removeConstraint (me.get(), U"*C\\mu");
		}
		if (includeCodas) {
			for (integer icand = 1; icand <= my numberOfCandidates; icand ++) {
				replaceOutput (& my candidates [icand]);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Metrics grammar not created.");
	}
}

/* End of file OTMulti_ex_metrics.cpp */
