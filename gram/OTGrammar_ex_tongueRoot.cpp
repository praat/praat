/* OTGrammar_ex_tongueRoot.cpp
 *
 * Copyright (C) 1997-2005,2007,2009,2011-2013,2015-2017 Paul Boersma
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

#include "OTGrammar.h"

static const char32 *vowels [] = { U"i", U"e", U"\\sw", U"\\ic", U"\\ef", U"a" };
#define i  0
#define e  1
#define schwa  2
#undef I
#define I  3
#define E  4
#define a  5
#define isatr(v)  ((v) <= schwa)
#define isrtr(v)  ((v) >= I)
#define fliptr(v)  (((v) + 3) % 6)

static void countVowelViolations (int *marks, int ncons, int v) {
	if (v == I) marks [1] ++;
	if (v == schwa) marks [2] ++;
	if (ncons == 9) {
		if (v == E) marks [6] ++;
		if (v == a) marks [7] ++;
		if (v == e) marks [8] ++;
		if (v == i) marks [9] ++;
	}
}

static void OTGrammarCandidate_init (OTGrammarCandidate me, int ncons, int v1, int v2) {
	my output = Melder_dup (Melder_cat (vowels [v1], U"t", vowels [v2]));
	my marks = NUMvector <int> (1, my numberOfConstraints = ncons);
	/*
	 * Count vowel-gesture violations.
	 */
	countVowelViolations (my marks, ncons, v1);
	countVowelViolations (my marks, ncons, v2);
	/*
	 * Count contour-gesture violations.
	 */
	if (isatr (v1) != isatr (v2)) my marks [5] ++;
}

autoOTGrammar OTGrammar_create_tongueRoot_grammar (
	kOTGrammar_createTongueRootGrammar_constraintSet small_large,
	kOTGrammar_createTongueRootGrammar_ranking equal_random_infant_Wolof)
{
	try {
		int ncons = small_large == kOTGrammar_createTongueRootGrammar_constraintSet::FIVE ? 5 : 9, itab, v1, v2;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = NUMvector <structOTGrammarConstraint> (1, my numberOfConstraints = ncons);
		my constraints [1]. name = Melder_dup (U"*[rtr / hi]");
		my constraints [2]. name = Melder_dup (U"*[atr / lo]");
		my constraints [3]. name = Melder_dup (U"P\\s{ARSE}\n(rtr)");
		my constraints [4]. name = Melder_dup (U"P\\s{ARSE}\n(atr)");
		my constraints [5]. name = Melder_dup (U"*G\\s{ESTURE}\n(contour)");
		if (ncons == 9) {
			my constraints [6]. name = Melder_dup (U"*[rtr / mid]");
			my constraints [7]. name = Melder_dup (U"*[rtr / lo]");
			my constraints [8]. name = Melder_dup (U"*[atr / mid]");
			my constraints [9]. name = Melder_dup (U"*[atr / hi]");
		}
		if (equal_random_infant_Wolof == kOTGrammar_createTongueRootGrammar_ranking::EQUAL) {
			for (integer icons = 1; icons <= ncons; icons ++)
				my constraints [icons]. ranking = 100.0;
		} else if (equal_random_infant_Wolof == kOTGrammar_createTongueRootGrammar_ranking::RANDOM) {
			for (integer icons = 1; icons <= ncons; icons ++)
				my constraints [icons]. ranking = NUMrandomGauss (100.0, 10.0);
		} else if (equal_random_infant_Wolof == kOTGrammar_createTongueRootGrammar_ranking::INFANT) {
			for (integer icons = 1; icons <= ncons; icons ++)
				my constraints [icons]. ranking = 100.0;   // structural constraints
			my constraints [3]. ranking = 50.0;   // faithfulness constraints
			my constraints [4]. ranking = 50.0;
		} else {
			Melder_assert (equal_random_infant_Wolof == kOTGrammar_createTongueRootGrammar_ranking::WOLOF);
			my constraints [1]. ranking = 100.0;
			my constraints [2]. ranking =  10.0;
			my constraints [3]. ranking =  50.0;
			my constraints [4]. ranking =  20.0;
			my constraints [5]. ranking =  30.0;
			if (ncons == 9) {
				my constraints [6]. ranking =   0.0;
				my constraints [7]. ranking = -10.0;
				my constraints [8]. ranking =   0.0;
				my constraints [9]. ranking = -10.0;
			}
		}
		if (ncons == 9) {
			my fixedRankings = NUMvector <structOTGrammarFixedRanking> (1, my numberOfFixedRankings = 4);
			my fixedRankings [1]. higher = 1, my fixedRankings [1]. lower = 6;
			my fixedRankings [2]. higher = 6, my fixedRankings [2]. lower = 7;
			my fixedRankings [3]. higher = 2, my fixedRankings [3]. lower = 8;
			my fixedRankings [4]. higher = 8, my fixedRankings [4]. lower = 9;
		}
		my tableaus = NUMvector <structOTGrammarTableau> (1, my numberOfTableaus = 36);
		itab = 1;
		for (v1 = 0; v1 < 6; v1 ++) for (v2 = 0; v2 < 6; v2 ++) {
			OTGrammarTableau tableau = & my tableaus [itab];
			tableau -> input = Melder_dup (Melder_cat (vowels [v1], U"t", vowels [v2]));
			tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates = 4);
			/*
			 * Generate the four tongue-root variants as output candidates.
			 */
			OTGrammarCandidate_init (& tableau -> candidates [1], ncons, v1, v2);
				/* Faithful: no PARSE constraints violated. */
			OTGrammarCandidate_init (& tableau -> candidates [2], ncons, fliptr (v1), v2);
				/* First vowel flipped: violated one PARSE constraint. */
			OTGrammarCandidate_init (& tableau -> candidates [3], ncons, v1, fliptr (v2));
				/* Second vowel flipped. */
			OTGrammarCandidate_init (& tableau -> candidates [4], ncons, fliptr (v1), fliptr (v2));
				/* Both vowels flipped. */
			/*
			 * Count PARSE violations.
			 */
			if (isatr (v1)) {
				tableau -> candidates [2]. marks [4] ++;
				tableau -> candidates [4]. marks [4] ++;
			} else {
				tableau -> candidates [2]. marks [3] ++;
				tableau -> candidates [4]. marks [3] ++;
			}
			if (isatr (v2)) {
				tableau -> candidates [3]. marks [4] ++;
				tableau -> candidates [4]. marks [4] ++;
			} else {
				tableau -> candidates [3]. marks [3] ++;
				tableau -> candidates [4]. marks [3] ++;
			}
			itab ++;
		}
		OTGrammar_checkIndex (me.get());
		OTGrammar_newDisharmonies (me.get(), 0.0);
		for (integer icons = 1; icons <= my numberOfConstraints; icons ++)
			my constraints [icons]. plasticity = 1.0;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Tongue root grammar not created.");
	}
}

/* End of file OTGrammar_ex_tongueRoot.cpp */
