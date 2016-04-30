/* OTGrammar_enums.h
 *
 * Copyright (C) 2006-2011,2013,2014,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kOTGrammar_decisionStrategy, 0)
	enums_add (kOTGrammar_decisionStrategy, 0, OPTIMALITY_THEORY, U"OptimalityTheory")
	enums_add (kOTGrammar_decisionStrategy, 1, HARMONIC_GRAMMAR, U"HarmonicGrammar")
	enums_add (kOTGrammar_decisionStrategy, 2, LINEAR_OT, U"LinearOT")
	enums_add (kOTGrammar_decisionStrategy, 3, EXPONENTIAL_HG, U"ExponentialHG")
	enums_add (kOTGrammar_decisionStrategy, 4, MAXIMUM_ENTROPY, U"MaximumEntropy")
	enums_add (kOTGrammar_decisionStrategy, 5, POSITIVE_HG, U"PositiveHG")
	enums_add (kOTGrammar_decisionStrategy, 6, EXPONENTIAL_MAXIMUM_ENTROPY, U"ExponentialMaximumEntropy")
enums_end (kOTGrammar_decisionStrategy, 6, OPTIMALITY_THEORY)

enums_begin (kOTGrammar_rerankingStrategy, 0)
	enums_add (kOTGrammar_rerankingStrategy, 0, DEMOTION_ONLY, U"Demotion only")
	enums_add (kOTGrammar_rerankingStrategy, 1, SYMMETRIC_ONE, U"Symmetric one")
	enums_add (kOTGrammar_rerankingStrategy, 2, SYMMETRIC_ALL, U"Symmetric all")
	enums_add (kOTGrammar_rerankingStrategy, 3, SYMMETRIC_ALL_SKIPPABLE, U"Symmetric all (skippable)")
	enums_add (kOTGrammar_rerankingStrategy, 4, WEIGHTED_UNCANCELLED, U"Weighted uncancelled")
	enums_add (kOTGrammar_rerankingStrategy, 5, WEIGHTED_ALL, U"Weighted all")
	enums_add (kOTGrammar_rerankingStrategy, 6, EDCD, U"EDCD")
	enums_add (kOTGrammar_rerankingStrategy, 7, EDCD_WITH_VACATION, U"EDCD with vacation")
	enums_add (kOTGrammar_rerankingStrategy, 8, WEIGHTED_ALL_UP_HIGHEST_DOWN, U"Weighted all up, highest down")
	enums_add (kOTGrammar_rerankingStrategy, 9, WEIGHTED_ALL_UP_HIGHEST_DOWN_2012, U"Weighted all up, highest down (2012)")
	enums_add (kOTGrammar_rerankingStrategy, 10, WEIGHTED_ALL_UP_HIGH_DOWN, U"Weighted all up, high down")
	enums_add (kOTGrammar_rerankingStrategy, 11, WEIGHTED_ALL_UP_HIGH_DOWN_2012, U"Weighted all up, high down (2012)")
enums_end (kOTGrammar_rerankingStrategy, 11, SYMMETRIC_ALL)

/* End of file OTGrammar_enums.h */
